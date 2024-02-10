// Doxygen Documentation
/*! \file 	acquire_data.cpp
 *  \brief  Acquire data from all sensors
 *
 * This File contains all the necessary methods to acquire the
 * data from all sensors.
 *
 * \author 		Matthias Werner
 * \date		02/2023
 *
 * - Prozessor:         ESP32-WROOM
 * - Hardware:          az-delivery-devkit-v4
 */

#include <acquire_data.h>

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONEWIRE_PIN);
// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature oneWireSensors(&oneWire);

extern AcquireData data;

//****************************************
// Construct a new AcquireDataobject
AcquireData::AcquireData()
{
}

//****************************************
// Setup all the measurement channels
void AcquireData::setUpMeasurementChannels()
{

  // Setup the Interrupts for Speed Measurement
  _setUpEngineSpeedInt();
  _setUpShaftSpeedInt();
  _setUpAlternator1SpeedInt();
  _setUpAlternator2SpeedInt();

  // Start the MCP3204 Chip for ADC Conversation
  mcp3204.selectVSPI();
  mcp3204.begin(NOT_CS_ADC_PIN);
  mcp3204.setSPIspeed(500000);
}

//****************************************
//
void AcquireData::showDataOnTerminal()
{
  uint8_t i = 1;

  // clear the terminal
  // while (i-- > 0)
  //   Serial.println();

  // tSeaOutletWall.printDatapointFull();
  // tGearbox.printDatapointFull();
  // tAlternator.printDatapointFull();
  // uBat.printDatapointFull();
  // nMot.printDatapointFull();
  // nShaft.printDatapointFull();
  // nAlternator1.printDatapointFull();
  // nAlternator2.printDatapointFull();
  // tExhaust.printDatapointFull();
  // uMcp3204Ch1.printDatapointFull();
  // uMcp3204Ch2.printDatapointFull();
  // uMcp3204Ch3.printDatapointFull();
  // uMcp3204Ch4.printDatapointFull();
  // flgContact1.printDatapointFull();
  // flgContact2.printDatapointFull();
  // flgContact3.printDatapointFull();

  tSeaOutletWall.printDatapointShort();
  tGearbox.printDatapointShort();
  tAlternator.printDatapointShort();
  tExhaust.printDatapointShort();
  Serial.println();

  nMot.printDatapointShort();
  nShaft.printDatapointShort();
  nAlternator1.printDatapointShort();
  nAlternator2.printDatapointShort();
  Serial.println();

  uBat.printDatapointShort();
  uMcp3204Ch1.printDatapointShort();
  uMcp3204Ch2.printDatapointShort();
  uMcp3204Ch3.printDatapointShort();
  uMcp3204Ch4.printDatapointShort();
  Serial.println();

  flgContact1.printDatapointShort();
  flgContact2.printDatapointShort();
  flgContact3.printDatapointShort();
  Serial.println();
}

void AcquireData::updateLCDPage(uint8_t page)
{

  char buffer[21];
  int length;

  switch (page)
  {
  case 1:
    // fill the screen buffer
    length = sprintf(buffer, "Seawater     %3.1f C", tSeaOutletWall.getValue());
    strncpy(&lcdDisplay[0][0], buffer, 20);

    length = sprintf(buffer, "Engine       %3.1f C", tSeaOutletWall.getValue());
    strncpy(&lcdDisplay[1][0], buffer, 20);

    length = sprintf(buffer, "Alternator   %3.1f C", tAlternator.getValue());
    strncpy(&lcdDisplay[2][0], buffer, 20);

    length = sprintf(buffer, "Exhaust      %3.1f C", tExhaust.getValue());
    strncpy(&lcdDisplay[3][0], buffer, 20);

    break;

  default:
    // fill the screen buffer
    length = sprintf(buffer, "n_Eng   %4d U/min", (uint16_t)nMot.getValue());
    strncpy(&lcdDisplay[0][0], buffer, 20);

    length = sprintf(buffer, "n_Gear  %4d U/min", (uint16_t)nShaft.getValue());
    strncpy(&lcdDisplay[1][0], buffer, 20);

    length = sprintf(buffer, "n_Alt1  %4d U/min", (uint16_t)nAlternator1.getValue());
    strncpy(&lcdDisplay[2][0], buffer, 20);

    length = sprintf(buffer, "n_Alt2  %4d U/min", (uint16_t)nAlternator2.getValue());
    strncpy(&lcdDisplay[3][0], buffer, 20);

    break;
  }
}

//****************************************
// Stores the data into a Datapoint
void AcquireData::_StoreData(tDataPoint &db, double value, uint32_t timestamp)
{
  // Update the Value of the Datapoint
  db.updateValue(value, timestamp);
}

//****************************************
// Measure Exhaust Temperature
void AcquireData::measureExhaustTemperature()
{

  double tMeasure = -200;

  tMeasure = thermoNiCr_Ni.readCelsius();
  this->_StoreData(this->tExhaust, tMeasure, millis());
}

//****************************************
// Checking the states of the contact
void AcquireData::checkContacts()
{

  bool state = false;

  // check the states of all contacts (Low Active)
  state = !digitalRead(CONTACT1_PIN);
  this->_StoreData(this->flgContact1, (double)state, millis());
  state = !digitalRead(CONTACT2_PIN);
  this->_StoreData(this->flgContact2, (double)state, millis());
  state = !digitalRead(CONTACT3_PIN);
  this->_StoreData(this->flgContact3, (double)state, millis());
}

//****************************************
// Measure all voltages
void AcquireData::measureVoltage()
{
  double voltage;
  int voltageMax;
  // measure ESP32 AD-Channel UBat
  voltage = analogRead(UBAT_ADC_PIN);
  voltage = ADC_CH36_LUT[(int)voltage];
  voltage = voltage / 4096 * ACH_CH36_FACTOR + ACH_CH36_OFFSET;
  this->_StoreData(this->uBat, voltage, millis());

  // measure MCP3204 Channel 1
  voltageMax = mcp3204.maxValue();
  voltage = (double)mcp3204.analogRead(0);
  voltage = voltage / voltageMax * MCP3204_VREF * MCP3204_CH1_FAC;
  this->_StoreData(this->uMcp3204Ch1, voltage, millis());

  // measure MCP3204 Channel 2
  voltage = (double)mcp3204.analogRead(1);
  voltage = voltage / voltageMax * MCP3204_VREF * MCP3204_CH2_FAC;
  this->_StoreData(this->uMcp3204Ch2, voltage, millis());

  // measure MCP3204 Channel 3
  voltage = (double)mcp3204.analogRead(2);
  voltage = voltage / voltageMax * MCP3204_VREF * MCP3204_CH3_FAC;
  this->_StoreData(this->uMcp3204Ch3, voltage, millis());

  // measure MCP3204 Channel 4
  voltage = (double)mcp3204.analogRead(3);
  voltage = voltage / voltageMax * MCP3204_VREF * MCP3204_CH4_FAC;
  this->_StoreData(this->uMcp3204Ch4, voltage, millis());

  // Serial.print(millis());
  // Serial.print("\t mcp3204:\t");
  // for (int channel = 0 ; channel < mcp3204.channels(); channel++)
  // {
  //   uint16_t val = mcp3204.analogRead(channel);
  //   Serial.print(val);
  //   Serial.print("\t");
  //   delay(1);       // added so single reads are better visible on a scope
  // }
  // Serial.println();
}

//==============================================================================
//==============================================================================
// method's for 1-Wire  measurement
//==============================================================================
//==============================================================================

//****************************************
// Listing of all OneWire Devices at the bus
void AcquireData::listOneWireDevices()
{

  uint8_t i;
  uint8_t deviceCount;
  DeviceAddress address;

  // locate devices on the bus
  Serial.println("Locating devices...");
  Serial.print("Found ");
  deviceCount = oneWireSensors.getDeviceCount();
  Serial.print(deviceCount, DEC);
  Serial.println(" devices.");
  deviceCount = oneWireSensors.getDS18Count();
  Serial.print(deviceCount, DEC);
  Serial.println(" DS18S20 Sensors.");
  Serial.println("");

  // print addresses to terminal
  Serial.println("Printing addresses...");
  for (int i = 0; i < deviceCount; i++)
  {
    Serial.print("Sensor ");
    Serial.print(i + 1);
    Serial.print(" : ");
    oneWireSensors.getAddress(address, i);
    for (uint8_t i = 0; i < 8; i++)
    {
      Serial.print("0x");
      if (address[i] < 0x10)
        Serial.print("0");
      Serial.print(address[i], HEX);
      if (i < 7)
        Serial.print(", ");
    }
    Serial.println("");
  }
}

//****************************************
// Measure all OneWire Sensors
void AcquireData::measureOnewire()
{

  // Measure Wall Sensor Seawater outlet
  oneWireSensors.requestTemperaturesByAddress(oWtSeaOutletWall);
  double temp = oneWireSensors.getTempC(oWtSeaOutletWall);
  this->_StoreData(this->tSeaOutletWall, temp, millis());

  // Measure Balmar Alternator Sensor
  oneWireSensors.requestTemperaturesByAddress(oWtAlternator);
  temp = oneWireSensors.getTempC(oWtAlternator);
  this->_StoreData(this->tAlternator, temp, millis());

  // Measure Gearbox Sensor
  oneWireSensors.requestTemperaturesByAddress(oWtGearbox);
  temp = oneWireSensors.getTempC(oWtGearbox);
  this->_StoreData(this->tGearbox, temp, millis());
}

//==============================================================================
//==============================================================================
// Method's for Rotational Speed  measurement
//==============================================================================
//==============================================================================

//**********************************************
// Set up the Engine speed timer and interrupt
void AcquireData::_setUpEngineSpeedInt(void)
{
  // Init the PIN for engine speed frequency
  pinMode(ENGINE_RPM_PIN, INPUT_PULLUP);
  // attache the pin on falling edge to an interrupt and specify ISR
  attachInterrupt(digitalPinToInterrupt(ENGINE_RPM_PIN), handleEngineSpeedInterrupt, FALLING);

  // set up the engine speed timer
  // - the associated timer is TMR0
  // - prescaler of 80 gives 1ns as time per tick at 80Mhz
  // - timer counts upward
  data.engSpeedCalc.Timer = timerBegin(0, TIMER_PRESCALER_FOR_1NS, true);

  // start the timer
  timerStart(data.engSpeedCalc.Timer);
}

//**********************************************
// Set up the Engine speed timer and interrupt
void AcquireData::_setUpShaftSpeedInt(void)
{
  // Init the PIN for engine speed frequency
  pinMode(SHAFT_RPM_PIN, INPUT_PULLUP);
  // attache the pin on falling edge to an interrupt and specify ISR
  attachInterrupt(digitalPinToInterrupt(SHAFT_RPM_PIN), handleShaftSpeedInterrupt, FALLING);

  // set up the engine speed timer
  // - the associated timer is TMR0
  // - prescaler of 80 gives 1ns as time per tick at 80Mhz
  // - timer counts upward
  data.shaftSpeedCalc.Timer = timerBegin(0, TIMER_PRESCALER_FOR_1NS, true);

  // start the timer
  timerStart(data.shaftSpeedCalc.Timer);
}
//**********************************************
// Set up the Alternator 1 speed timer and interrupt
void AcquireData::_setUpAlternator1SpeedInt(void)
{
  // Init the PIN for engine speed frequency
  pinMode(ALTERNATOR1_RPM_PIN, INPUT_PULLUP);
  // attache the pin on falling edge to an interrupt and specify ISR
  attachInterrupt(digitalPinToInterrupt(ALTERNATOR1_RPM_PIN), handleAlternator1SpeedInterrupt, FALLING);

  // set up the engine speed timer
  // - the associated timer is TMR1
  // - prescaler of 80 gives 1ns as time per tick at 80Mhz
  // - timer counts upward
  data.alternator1SpeedCalc.Timer = timerBegin(1, TIMER_PRESCALER_FOR_1NS, true);

  // start the timer
  timerStart(data.alternator1SpeedCalc.Timer);
}

//**********************************************
// Set up the Alternator 2 speed timer and interrupt
void AcquireData::_setUpAlternator2SpeedInt(void)
{
  // Init the PIN for engine speed frequency
  pinMode(ALTERNATOR2_RPM_PIN, INPUT_PULLUP);
  // attache the pin on falling edge to an interrupt and specify ISR
  attachInterrupt(digitalPinToInterrupt(ALTERNATOR2_RPM_PIN), handleAlternator2SpeedInterrupt, FALLING);

  // set up the engine speed timer
  // - the associated timer is TMR2
  // - prescaler of 80 gives 1ns as time per tick at 80Mhz
  // - timer counts upward
  data.alternator2SpeedCalc.Timer = timerBegin(1, TIMER_PRESCALER_FOR_1NS, true);

  // start the timer
  timerStart(data.alternator2SpeedCalc.Timer);
}

//***********************************************
// Calculate rotational speed
double AcquireData::_calcNumberOfRevs(tSpeedCalc *tmrValues)
{
  double RPM = 0;
  // Lock the RAM and prevent other tasks from reading/writing
  taskENTER_CRITICAL(&tmrValues->muxTMR);
  if (tmrValues->PeriodCount != 0)
    // PeriodCount in 0.000001 of a second
    RPM = 1000000.00 / tmrValues->PeriodCount * 60;
  // prevent div by zero if there is no signal
  if (millis() > tmrValues->TimestampLastInt + 500)
    RPM = 0; // No signals RPM=0;

  // Unlock the RAM
  taskEXIT_CRITICAL(&tmrValues->muxTMR);

  return (RPM);
}

//****************************************
// Measure all Speeds
void AcquireData::measureSpeed()
{
  double speed;

  // measure Engine Speed
  speed = _calcNumberOfRevs(&engSpeedCalc);
  this->_StoreData(this->nMot, speed, millis());
  // measure Shaft Speed
  speed = _calcNumberOfRevs(&shaftSpeedCalc);
  this->_StoreData(this->nShaft, speed, millis());
  // measure Alternator1 Speed
  speed = _calcNumberOfRevs(&alternator1SpeedCalc);
  this->_StoreData(this->nAlternator1, speed, millis());
  // measure Alternator2 Speed
  speed = _calcNumberOfRevs(&alternator2SpeedCalc);
  this->_StoreData(this->nAlternator2, speed, millis());
}

//****************************************
// Convert all measured data into N2kData formats
void AcquireData::convertDataToN2k(tVolvoPentaData *data)
{

  // Check if the Semaphore used for Dataprotection is initialzed
  if (xMutexVolvoN2kData != NULL)
  {
    // See if we can obtain the semaphore. If the semaphore is not
    // available wait 5ms to see if it becomes free.
    if (xSemaphoreTake(xMutexVolvoN2kData, (TickType_t)5) == pdTRUE)
    {

      data->engine_hours = 1234;

      data->engine_coolant_flow = 0;
      data->engine_coolant_temperature = 345;
      data->engine_coolant_temperature_wall = this->tSeaOutletWall.getValue() + 273.15;
      ;
      data->engine_room_temperature = this->tAlternator.getValue() + 273.15;
      ;
      data->gearbox_temperature = this->tGearbox.getValue() + 273.15;
      data->exhaust_temperature = this->tExhaust.getValue() + 273.15;

      data->engine_oel_pressure = 23456;

      data->engine_speed = this->nMot.getValue();
      data->shaft_speed = this->nShaft.getValue();
      data->alternator1_speed = this->nAlternator1.getValue();
      data->alternator2_speed = this->nAlternator2.getValue();

      data->batterie_voltage = this->uBat.getValue();

      data->flg_coolant_temperature_ok = false;
      data->flg_engine_oel_pressure_ok = false;

// Simulationsdata
#ifdef USE_SIM_DATA_N2K

      data->engine_hours = 1234;

      data->engine_coolant_flow = 0;
      data->engine_coolant_temperature = 345;
      data->engine_coolant_temperature_wall = 456;
      data->engine_room_temperature = 333;
      data->gearbox_temperature = 543;
      data->exhaust_temperature = 678;

      data->engine_oel_pressure = 23456;

      data->engine_speed = 1234;
      data->shaft_speed = 5676;
      data->alternator1_speed = 2345;
      data->alternator2_speed = 3456;

      data->batterie_voltage = 12.99;

      data->flg_coolant_temperature_ok = false;
      data->flg_engine_oel_pressure_ok = false;

#endif
      // unlock the resource again
      xSemaphoreGive(xMutexVolvoN2kData);
    }
  }
}

//==============================================================================
//==============================================================================
// Interrupt Service Routines for Rotational Speed  measurement
//==============================================================================
//==============================================================================

//****************************************************
// Handle the interrupt triggered by the engine speed
void IRAM_ATTR handleEngineSpeedInterrupt()
{
  // Lock the RAM and prevent other tasks from reading/writing
  taskENTER_CRITICAL_ISR(&data.engSpeedCalc.muxTMR);
  // value of timer at interrupt
  uint64_t TempVal = timerRead(data.engSpeedCalc.Timer);
  // period count between falling edges in 0.000001 of a second
  data.engSpeedCalc.PeriodCount = TempVal - data.engSpeedCalc.StartValue;
  // puts latest reading as start for next calculation
  data.engSpeedCalc.StartValue = TempVal;
  data.engSpeedCalc.TimestampLastInt = millis();
  // Unlock the RAM
  taskEXIT_CRITICAL_ISR(&data.engSpeedCalc.muxTMR);
}

//****************************************************
// Handle the interrupt triggered by the shaft speed
void IRAM_ATTR handleShaftSpeedInterrupt()
{
  // Lock the RAM and prevent other tasks from reading/writing
  taskENTER_CRITICAL_ISR(&data.shaftSpeedCalc.muxTMR);
  // value of timer at interrupt
  uint64_t TempVal = timerRead(data.shaftSpeedCalc.Timer);
  // period count between falling edges in 0.000001 of a second
  data.shaftSpeedCalc.PeriodCount = TempVal - data.shaftSpeedCalc.StartValue;
  // puts latest reading as start for next calculation
  data.shaftSpeedCalc.StartValue = TempVal;
  data.shaftSpeedCalc.TimestampLastInt = millis();
  // Unlock the RAM
  taskEXIT_CRITICAL_ISR(&data.shaftSpeedCalc.muxTMR);
}

//****************************************************
// Handle the interrupt triggered by the engine speed
void IRAM_ATTR handleAlternator1SpeedInterrupt()
{
  // Lock the RAM and prevent other tasks from reading/writing
  taskENTER_CRITICAL_ISR(&data.alternator1SpeedCalc.muxTMR);
  // value of timer at interrupt
  uint64_t TempVal = timerRead(data.alternator1SpeedCalc.Timer);
  // period count between falling edges in 0.000001 of a second
  data.alternator1SpeedCalc.PeriodCount = TempVal - data.alternator1SpeedCalc.StartValue;
  // puts latest reading as start for next calculation
  data.alternator1SpeedCalc.StartValue = TempVal;
  data.alternator1SpeedCalc.TimestampLastInt = millis();
  // Unlock the RAM
  taskEXIT_CRITICAL_ISR(&data.alternator1SpeedCalc.muxTMR);
}

//****************************************************
// Handle the interrupt triggered by the engine speed
void IRAM_ATTR handleAlternator2SpeedInterrupt()
{
  // Lock the RAM and prevent other tasks from reading/writing
  taskENTER_CRITICAL_ISR(&data.alternator2SpeedCalc.muxTMR);
  // value of timer at interrupt
  uint64_t TempVal = timerRead(data.alternator2SpeedCalc.Timer);
  // period count between falling edges in 0.000001 of a second
  data.alternator2SpeedCalc.PeriodCount = TempVal - data.alternator2SpeedCalc.StartValue;
  // puts latest reading as start for next calculation
  data.alternator2SpeedCalc.StartValue = TempVal;
  data.alternator2SpeedCalc.TimestampLastInt = millis();
  // Unlock the RAM
  taskEXIT_CRITICAL_ISR(&data.alternator2SpeedCalc.muxTMR);
}
