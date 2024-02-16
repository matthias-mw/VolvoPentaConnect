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
 *
 * - Prozessor:         ESP32-WROOM
 * - Hardware:          az-delivery-devkit-v4
 **************************************************************/

#include <hardwareDef.h>

#include <stdint.h>
// This will automatically choose right CAN library and create suitable
// NMEA2000 object
#include <NMEA2000_CAN.h>
#include <N2kMessages.h>

#include <process_n2k.h>
#include <datapoint.h>
#include <acquire_data.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "LiquidCrystal_PCF8574.h"

#include <lookUpTable.h>

#define N2KUpdatePeriodFast 1000
#define N2KUpdatePeriodSlow 2000

static unsigned long timeUpdatedFast = millis();
static unsigned long timeUpdatedSlow = millis();
static bool blink = false;

static uint8_t lcdPage = 0;

TwoWire Wire_1 = TwoWire(0x3f);

LiquidCrystal_PCF8574 lcd(0x3F); // set the LCD address to 0x27

/// Buffer for Crystal  LCD Display 4x20 (4 lines a 20 char)
char lcdDisplay[4][20];

/// Class that contains a map to convert the measured voltage into tEngine
LookUpTable1D mapTCO(AXIS_TCO_MES, MAP_TCO_MES, TCO_AXIS_LEN, TCO_MAP_PREC);

/// Class that contains a map to convert the measured voltage into pOil
LookUpTable1D mapPOIL(AXIS_POIL_MES, MAP_POIL_MES, POIL_AXIS_LEN, POIL_MAP_PREC);

/// class that contains all measured data
AcquireData data;

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

TaskHandle_t TaskUpdateLCDHandle;

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

void taskUpdateLCD(void *pvParameters);

//***************************************************************
// Setup Task
void setup()
{

  // Wait that all Sensors can settle
  delay(1000);

  // Start Serial Output/Input
  Serial.begin(115200);

  // Init all the PINs
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);
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

  // Start I2C
  // Wire.begin();

  // Start the DS18B20 sensor
  oneWireSensors.begin();

  // Setup NMEA2000 Interface
  setupN2K();

  // Setup all Measurement Channels
  data.setUpMeasurementChannels();

  // List all connected oneWire Devices
  data.listOneWireDevices();

  // Setup LCD Display
  Wire_1.begin(21, 22);   // custom i2c port on ESP
  Wire_1.setClock(80000); // set 80kHz (PCF8574 max speed 100kHz)

  lcd.begin(20, 4, Wire_1);
  lcd.setBacklight(255); // TODO: Check Function

  // Create mutex before starting tasks
  xMutexVolvoN2kData = xSemaphoreCreateMutex();
  xMutexStdOut = xSemaphoreCreateMutex();

  // Create TaskMeasureOnewire with priority 1 at core 0
  xTaskCreatePinnedToCore(
      taskMeasureOneWire,        /* Function to implement the task */
      "TaskMeasureOneWire",      /* Name of the task */
      1700,                      /* Stack size in words */
      NULL,                      /* Task input parameter */
      1,                         /* Priority of the task */
      &TaskMeasureOneWireHandle, /* Task handle. */
      0);                        /* Core where the task should run */

  // Create TaskMeasureFast with priority 3 at core 0
  xTaskCreatePinnedToCore(
      taskMeasureFast,
      "TaskMeasureFast",
      1700,
      NULL,
      3,
      &TaskMeasureFastHandle,
      0);

  // Create TaskMeasureFast with priority 1 at core 0
  xTaskCreatePinnedToCore(
      taskUpdateLCD,        /* Function to implement the task */
      "TaskUpdateLCD",      /* Name of the task */
      3000,                 /* Stack size in words */
      NULL,                 /* Task input parameter */
      2,                    /* Priority of the task */
      &TaskUpdateLCDHandle, /* Task handle. */
      0);
}

//***************************************************************
// Standard IdleTask
void loop()
{

  // Check N2k Messages
  NMEA2000.ParseMessages();

  // Datenausgabe auf den Standard Terminal via USB
  if ((timeUpdatedFast + N2KUpdatePeriodFast) < millis())
  {
    timeUpdatedFast = millis();

    if (xSemaphoreTake(xMutexStdOut, (TickType_t)50) == pdTRUE)
    {
      data.showDataOnTerminal();
      xSemaphoreGive(xMutexStdOut);
    }
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
      Serial.print(" Measure Task is started -> free stack: ");
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

    char lcdbuf[21];

    data.updateLCDPage(lcdPage);

    // prepare screen
    lcd.home();
    lcd.clear();
    lcd.setCursor(0, 0);
    // copy the buffer to the screen
    // 1st line continues at 3d line
    // 2nd line continues at 4th line

 
    strncpy(lcdbuf, &lcdDisplay[0][0], 20);
    lcdbuf[20] = '\0';
    lcd.print(lcdbuf); // print the line to screen
 
    lcd.setCursor(0, 1);
    strncpy(lcdbuf, &lcdDisplay[1][0], 20);
    lcdbuf[20] = '\0';
    lcd.print(lcdbuf); // print the line to screen
 
    lcd.setCursor(0, 2);
    strncpy(lcdbuf, &lcdDisplay[2][0], 20);
    lcdbuf[20] = '\0';
    lcd.print(lcdbuf); // print the line to screen

    lcd.setCursor(0, 3);
    strncpy(lcdbuf, &lcdDisplay[3][0], 20);
    lcdbuf[20] = '\0';
    lcd.print(lcdbuf); // print the line to screen

    lcdPage++;

    if (lcdPage > 4)
    {
      lcdPage = 0;
    }
 
    // non blocking delay for the lcd Display
    vTaskDelay(pdMS_TO_TICKS(4000));
  }
}