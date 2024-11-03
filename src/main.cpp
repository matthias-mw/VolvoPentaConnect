// Doxygen Dokumentation
/*! \file 	main.cpp
 *  \brief  Hauptprogramm des VolvoPentaConnect Projektes
 *
 *  Mit dem VolvoPentaConnect Projekt sollen aktuelle Messwerte
 *  und Daten von dem VolvoPenta Diesel Motor über NMEA2000
 *  CAN-BUS direkt in das Netzwerk im Boot eingespielt werden.
 *
 * \author 		Matthias Werner
 * \date		10 Sep 2022
 * \version PROJECT_VERSION
 * - Prozessor:         ESP32-WROOM
 * - Hardware:          az-delivery-devkit-v4
 **************************************************************/

#include <hardwareDef.h>
#include <stdint.h>
#include "nvs_flash.h"
#include "nvs.h"

// This will automatically choose right CAN library and create suitable
// NMEA2000 object
#include <NMEA2000_CAN.h>
#include <N2kMessages.h>

#include <process_n2k.h>
#include <datapoint.h>
#include <acquire_data.h>
#include <display_data.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <lookUpTable.h>

/// Milliseconds for updating the terminal output
#define UPDATE_TERMINAL_PERIOD 1000

/// Millisecond counter for Updating the Terminal Output
static unsigned long timeUpdatedCnt = millis();

/// Button 1 debounce counter
uint16_t btn1DebounceCnt = 0;
/// Button 1 debounce counter
uint16_t btn2DebounceCnt = 0;
/// LongPressButton1 is served
bool lngPressButton1Served = false;
/// LongPressButton2 is served
bool lngPressButton2Served = false;

/// Class that contains a map to convert the measured voltage into tEngine
LookUpTable1D mapTCO(AXIS_TCO_MES, MAP_TCO_MES, TCO_AXIS_LEN, TCO_MAP_PREC);

/// Class that contains a map to convert the measured voltage into pOil
LookUpTable1D mapPOIL(AXIS_POIL_MES, MAP_POIL_MES, POIL_AXIS_LEN, POIL_MAP_PREC);

/// class that contains all measured data
AcquireData data;

/// class that contains all data for the LCD Panel
DisplayData lcdDisplayData(data);

/// structure that hold all data ready for N2k sending
tVolvoPentaData VolvoDataForN2k;
/// Mutex for protecting data integrity of \ref VolvoDataForN2k
SemaphoreHandle_t xMutexVolvoN2kData = NULL;
/// Mutex for protection stdout
SemaphoreHandle_t xMutexStdOut = NULL;

/************************************************************************
 * \brief Task Handle for task measuring fast signals
 */
TaskHandle_t TaskMeasureFastHandle;
/************************************************************************
 * \brief Task Handle for task measuring oneWire signals
 */
TaskHandle_t TaskMeasureOneWireHandle;

/************************************************************************
 * \brief Task Handle for task LCD Panel Update
 */
TaskHandle_t TaskUpdateLCDHandle;

/************************************************************************
 * \brief Task Handle for task Button Interpretation
 */
TaskHandle_t TaskInterpretButtonHandle;

/************************************************************************
 * \brief Task Handle for Data storage permanent
 */
TaskHandle_t TaskInterpretStorePermanentData;

/************************************************************************
 * \brief Task for measuring oneWire signals
 *
 * This tasks measures all oneWire signals, converts them to N2K format
 * and sends out corresponding N2K messages \ref SendN2kEngineParmSlow.
 *
 * \param parameter {type}
 */
void taskMeasureOneWire(void *parameter);

/************************************************************************
 * \brief Task for measuring fast signals
 *
 * This tasks measures all fast signals, converts them to N2K format
 * and sends out corresponding N2K messages \ref SendN2kEngineParmFast.
 *
 * \param parameter {type}
 */
void taskMeasureFast(void *pvParameters);

/************************************************************************
 * \brief Task for LCD Panel update
 *
 * This tasks will update the LCD Panel.
 *
 * \param parameter {type}
 */
void taskUpdateLCD(void *pvParameters);

/************************************************************************
 * \brief Task for Button Interpretation
 *
 * This tasks will check the buttons continuously and issues certain
 * commands from the user interaction.
 *
 * \param parameter {type}
 */
void taskInterpretButton(void *pvParameters);

/************************************************************************
 * \brief Task for permanent Data storage
 *
 * This tasks will store all the data which is need permanent
 *
 * \param parameter {type}
 */
void taskStorePermanentData(void *pvParameters);

//***************************************************************
// Setup Task
void setup()
{
  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // Setup LCD Display
  lcdDisplayData.setupLCDPanel();
  lcdDisplayData.setLcdCurrentPage(WELCOME_PAGE);

  // Wait that all Sensors can settle
  delay(1000);

  // Start Serial Output/Input
  Serial.begin(115200);

  // Init all the PINs
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LED_PIN_OFF);
  pinMode(BUTTON1_PIN, INPUT);
  pinMode(BUTTON2_PIN, INPUT);

  pinMode(WARNING_OUTPUT_PIN, OUTPUT);
  digitalWrite(WARNING_OUTPUT_PIN, LOW);
  pinMode(RELAY_OUTPUT_PIN, OUTPUT);
  digitalWrite(RELAY_OUTPUT_PIN, LOW);

  pinMode(CONTACT1_PIN, INPUT);
  pinMode(CONTACT2_PIN, INPUT);
  pinMode(CONTACT3_PIN, INPUT);

  pinMode(LT_ON_PIN, OUTPUT);
  digitalWrite(LT_ON_PIN, HIGH);
  pinMode(NOT_CS_THERMO_PIN, OUTPUT);
  digitalWrite(NOT_CS_THERMO_PIN, HIGH);
  pinMode(NOT_CS_ADC_PIN, OUTPUT);
  digitalWrite(NOT_CS_ADC_PIN, HIGH);

  pinMode(ONEWIRE_PIN, INPUT);

  pinMode(ENGINE_RPM_PIN, INPUT);
  pinMode(SHAFT_RPM_PIN, INPUT);
  pinMode(ALTERNATOR1_RPM_PIN, INPUT);
  pinMode(ALTERNATOR2_RPM_PIN, INPUT);

  pinMode(ESP32_CAN_TX_PIN, OUTPUT);
  pinMode(ESP32_CAN_RX_PIN, INPUT);

  // Start the DS18B20 sensor
  oneWireSensors.begin();

  // Setup NMEA2000 Interface
  setupN2K();

  // Setup all Measurement Channels
  data.setUpMeasurementChannels();

  // List all connected oneWire Devices
  data.listOneWireDevices();

  // restore NVM Data
  data.restoreNVMdata();

  // Setup LCD Display
  lcdDisplayData.setLcdCurrentPage(PAGE_ENGINE);

  // Create mutex before starting tasks
  xMutexVolvoN2kData = xSemaphoreCreateMutex();
  xMutexStdOut = xSemaphoreCreateMutex();

  // Create TaskMeasureOnewire with priority 1 at core 0
  xTaskCreatePinnedToCore(
      taskMeasureOneWire,        /* Function to implement the task */
      "TaskMeasureOneWire",      /* Name of the task */
      1600,                      /* Stack size in words */
      NULL,                      /* Task input parameter */
      1,                         /* Priority of the task */
      &TaskMeasureOneWireHandle, /* Task handle. */
      0);                        /* Core where the task should run */

  // Create TaskMeasureFast with priority 3 at core 0
  xTaskCreatePinnedToCore(
      taskMeasureFast,        /* Function to implement the task */
      "TaskMeasureFast",      /* Name of the task */
      1600,                   /* Stack size in words */
      NULL,                   /* Task input parameter */
      3,                      /* Priority of the task */
      &TaskMeasureFastHandle, /* Task handle. */
      0);                     /* Core where the task should run */

  // Create TaskUpdateLCD with priority 2 at core 0
  xTaskCreatePinnedToCore(
      taskUpdateLCD,        /* Function to implement the task */
      "TaskUpdateLCD",      /* Name of the task */
      3000,                 /* Stack size in words */
      NULL,                 /* Task input parameter */
      2,                    /* Priority of the task */
      &TaskUpdateLCDHandle, /* Task handle. */
      0);                   /* Core where the task should run */

  // Create TaskInterpretButton with priority 2 at core 0
  xTaskCreatePinnedToCore(
      taskInterpretButton,        /* Function to implement the task */
      "TaskInterpretButton",      /* Name of the task */
      3000,                       /* Stack size in words */
      NULL,                       /* Task input parameter */
      2,                          /* Priority of the task */
      &TaskInterpretButtonHandle, /* Task handle. */
      0);                         /* Core where the task should run */

  // Create TaskInterpretButton with priority 1 at core 0
  xTaskCreatePinnedToCore(
      taskStorePermanentData,           /* Function to implement the task */
      "TaskStorePermanentData",         /* Name of the task */
      2100,                             /* Stack size in words */
      NULL,                             /* Task input parameter */
      1,                                /* Priority of the task */
      &TaskInterpretStorePermanentData, /* Task handle. */
      0);
}

//***************************************************************
// Standard IdleTask
void loop()
{

  // Check N2k Messages
  NMEA2000.ParseMessages();

  // Datenausgabe auf den Standard Terminal via USB
  if ((timeUpdatedCnt + UPDATE_TERMINAL_PERIOD) < millis())
  {
    timeUpdatedCnt = millis();

// Debugging
#ifdef DEBUG_LEVEL
    if (DEBUG_LEVEL == 1)
    {
      if (xSemaphoreTake(xMutexStdOut, (TickType_t)50) == pdTRUE)
      {
        data.showDataOnTerminal();
        xSemaphoreGive(xMutexStdOut);
      }
    }
#endif // DEBUG_LEVEL

  }
}

//***************************************************************
// Task to measure and send OneWire Data on a regular basis
void taskMeasureOneWire(void *parameter)
{

  while (1)
  {
// just to debug the stacksize
#ifdef DEBUG_TASK_STACK_SIZE

    if (xSemaphoreTake(xMutexStdOut, (TickType_t)50) == pdTRUE)
    {
      UBaseType_t stackHighWaterMark;
      stackHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
      Serial.print(millis());
      Serial.print(" OneWire Task is started -> free stack: ");
      Serial.println(stackHighWaterMark);

      xSemaphoreGive(xMutexStdOut);
    }

#endif // DEBUG_TASK_STACK_SIZE

    // measure onewire devices
    data.measureOnewire();
    // convert data
    data.convertDataToN2k(&VolvoDataForN2k);
    // send data to NMEA2000 Bus
    SendN2kEngineParmSlow(&VolvoDataForN2k);

    // non blocking delay for the slow measuring
    vTaskDelay(pdMS_TO_TICKS(300));
  }
}

//***************************************************************
// Task to measure and send fast signals on a regular basis
void taskMeasureFast(void *pvParameters)
{

  while (1)
  {
// just to debug the stacksize
#ifdef DEBUG_TASK_STACK_SIZE
    if (xSemaphoreTake(xMutexStdOut, (TickType_t)50) == pdTRUE)
    {
      UBaseType_t stackHighWaterMark;
      stackHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
      Serial.print(millis());
      Serial.print(" Measure Fast Task is started -> free stack: ");
      Serial.println(stackHighWaterMark);

      xSemaphoreGive(xMutexStdOut);
    }

#endif // DEBUG_TASK_STACK_SIZE

    // measure all fast signals
    data.measureVoltage();
    data.measureSpeed();
    data.measureExhaustTemperature();
    data.checkContacts();
    // calculate values
    data.calculateVolvoPentaSensors();
    data.calcEngineSeconds();

    // convert data
    data.convertDataToN2k(&VolvoDataForN2k);
    // send data to NMEA2000 Bus
    SendN2kEngineParmFast(&VolvoDataForN2k);

    // non blocking delay for the fast measuring
    vTaskDelay(pdMS_TO_TICKS(249));
  }
}

//***************************************************************
// Task to Update the LCD Display
void taskUpdateLCD(void *pvParameters)
{

  while (1)
  {
// just to debug the stacksize
#ifdef DEBUG_TASK_STACK_SIZE
    if (xSemaphoreTake(xMutexStdOut, (TickType_t)50) == pdTRUE)
    {
      UBaseType_t stackHighWaterMark;
      stackHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
      Serial.print(millis());
      Serial.print(" Update LCD Task is started -> free stack: ");
      Serial.println(stackHighWaterMark);

      xSemaphoreGive(xMutexStdOut);
    }

#endif // DEBUG_TASK_STACK_SIZE

    // Update the Content of the LCD Panel for an specific page
    // on standard just update the 4th line
    // with the actual measured values
    lcdDisplayData.updateLcdContent(true);

    // Update LED Backlight Brightness
    lcdDisplayData.updateLcdBacklight();

    // non blocking delay for the lcd Display
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

//***************************************************************
// Task to Interpret the buttons
void taskInterpretButton(void *pvParameters)
{
  while (1)
  {
// just to debug the stacksize
#ifdef DEBUG_TASK_STACK_SIZE
    if (xSemaphoreTake(xMutexStdOut, (TickType_t)50) == pdTRUE)
    {
      UBaseType_t stackHighWaterMark;
      stackHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
      Serial.print(millis());
      Serial.print(" Button Task is started -> free stack: ");
      Serial.println(stackHighWaterMark);

      xSemaphoreGive(xMutexStdOut);
    }

#endif // DEBUG_TASK_STACK_SIZE

    // ============================================
    // Check Button 1
    // ============================================
    if (digitalRead(BUTTON1_PIN))
    {
      digitalWrite(STATUS_LED_PIN, LED_PIN_ON);

      // ++++++++++ Do on Button Release ++++++++++
      // increase debounce counter
      btn1DebounceCnt++;
      // enable lcd backlight
      lcdDisplayData.resetLcdBacklightCounter();
      // detect long press event
      if (btn1DebounceCnt >= BUTTON_LONG_PRESS)
      {
        // =========================
        // long press action
        // =========================
        if (lngPressButton1Served == false)
        {
          
          // ++++++++++ Only TO Set EngineHour Start +++++++
          // Set Engine Runtime in Seconds
          //data.initEngineHours(214*60*60);

          lcdDisplayData.setLcdCurrentPage(WELCOME_PAGE);

          // mark long press event as served
          lngPressButton1Served = true;

// Debugging
#ifdef DEBUG_LEVEL
          if (DEBUG_LEVEL > 2)
          {
            Serial.println("Button 1 -> Long press action...");
          }

#endif // DEBUG_LEVEL
        }
      }
    }
    else
    {
      digitalWrite(STATUS_LED_PIN, LED_PIN_OFF);

      // ++++++++++ Do on Button Release ++++++++++
      // detect short press event
      if ((btn1DebounceCnt >= BUTTON_DEBOUNCE) && (btn1DebounceCnt < BUTTON_LONG_PRESS))
      {
        // =========================
        // short press action
        // =========================
        // LCD Backlight to full
        lcdDisplayData.setLcdCurrentPage(PAGE_TEMPERATURE);
        lcdDisplayData.resetLcdBacklightCounter();

// Debugging
#ifdef DEBUG_LEVEL
        if (DEBUG_LEVEL > 2)
        {
          Serial.println("Button 1 -> Short press action...");
        }

#endif // DEBUG_LEVEL
      }
      // reset debounce counter
      btn1DebounceCnt = 0;
      // reset long press event
      lngPressButton1Served = false;
    }

    // ============================================
    // Check Button 2 -> DownSwitch
    // ============================================
    if (digitalRead(BUTTON2_PIN))
    {
      // ++++++++++ Do on Button Release ++++++++++
      // increase debounce counter
      btn2DebounceCnt++;
      // enable lcd backlight
      lcdDisplayData.resetLcdBacklightCounter();
      // detect long press event
      if (btn2DebounceCnt >= BUTTON_LONG_PRESS)
      {
        // =========================
        // long press action
        // =========================
        if (lngPressButton2Served == false)
        {
          // go to LCD Page PAGE_1WIRE_LIST
          lcdDisplayData.setLcdCurrentPage(PAGE_1WIRE_LIST);
          
          // mark long press event as served
          lngPressButton2Served = true;

// Debugging
#ifdef DEBUG_LEVEL
          if (DEBUG_LEVEL > 2)
          {
            Serial.println("Button 2 -> Long press action...");
          }

#endif // DEBUG_LEVEL
        }
      }
    }
    else
    {
      // ++++++++++ Do on Button Release ++++++++++
      // detect short press event
      if ((btn2DebounceCnt >= BUTTON_DEBOUNCE) && (btn2DebounceCnt < BUTTON_LONG_PRESS))
      {
        // =========================
        // short press action
        // =========================
        lcdDisplayData.increaseLcdCurrentPage();

// Debugging
#ifdef DEBUG_LEVEL
        if (DEBUG_LEVEL > 2)
        {
          Serial.println("Button 2 -> Short press action...");
        }

#endif // DEBUG_LEVEL
      }

      // reset debounce counter
      btn2DebounceCnt = 0;
      // reset long press event
      lngPressButton2Served = false;
    }
    // non blocking delay for the button
    vTaskDelay(pdMS_TO_TICKS(150));
  }
}

//***************************************************************
// Task for permanent Data storage
void taskStorePermanentData(void *pvParameters)
{
  while (1)
  {
// just to debug the stacksize
#ifdef DEBUG_TASK_STACK_SIZE
    if (xSemaphoreTake(xMutexStdOut, (TickType_t)50) == pdTRUE)
    {
      UBaseType_t stackHighWaterMark;
      stackHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
      Serial.print(millis());
      Serial.print(" Store Data Task is started -> free stack: ");
      Serial.println(stackHighWaterMark);

      xSemaphoreGive(xMutexStdOut);
    }

#endif // DEBUG_TASK_STACK_SIZE

    // store NVM data
    data.storeNVMdata();

    // non blocking delay for the button (every 5min)
    vTaskDelay(pdMS_TO_TICKS(1000 * 60 * 1));
  }
}