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

#include <lookUpTable.h>

#define N2KUpdatePeriodFast 1000
#define N2KUpdatePeriodSlow 2000

static unsigned long timeUpdatedFast = millis();
static unsigned long timeUpdatedSlow = millis();
static bool blink = false;

LookUpTable1D tab(AXIS_TCO_MES, MAP_TCO_MES, TCO_AXIS_LEN, TCO_MAP_PREC);

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
  Wire.begin();

  // Start the DS18B20 sensor
  oneWireSensors.begin();

  // Setup NMEA2000 Interface
  setupN2K();

  // Setup all Measurement Channels
  data.setUpMeasurementChannels();

  // List all connected oneWire Devices
  data.listOneWireDevices();

  // Create mutex before starting tasks
  xMutexVolvoN2kData = xSemaphoreCreateMutex();
  xMutexStdOut = xSemaphoreCreateMutex();

  // Create TaskMeasureOnewire with priority 0 at core 0
  xTaskCreatePinnedToCore(
      taskMeasureOneWire,        /* Function to implement the task */
      "TaskMeasureOneWire",      /* Name of the task */
      1300,                      /* Stack size in words */
      NULL,                      /* Task input parameter */
      0,                         /* Priority of the task */
      &TaskMeasureOneWireHandle, /* Task handle. */
      0);                        /* Core where the task should run */

  // Create TaskMeasureFast with priority 1 at core 0
  xTaskCreatePinnedToCore(
      taskMeasureFast,
      "TaskMeasureFast",
      1300,
      NULL,
      1,
      &TaskMeasureFastHandle,
      0);
}

//***************************************************************
// Standard IdleTask
void loop()
{

  // Check N2k Messages
  NMEA2000.ParseMessages();

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
    // send data
    SendN2kEngineParmSlow(&VolvoDataForN2k);

    // non blocking delay for the fast measuring
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
    // convert data
    data.convertDataToN2k(&VolvoDataForN2k);
    // send data to N2K
    SendN2kEngineParmFast(&VolvoDataForN2k);

    // non blocking delay for the fast measuring
    vTaskDelay(pdMS_TO_TICKS(249));
  }
}