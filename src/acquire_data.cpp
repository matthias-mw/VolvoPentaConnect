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


extern tAcquireData data;

//****************************************
// Construct a new tAcquireDataobject
tAcquireData::tAcquireData()
{

  
}

//****************************************
// Stores the data into a Datapoint
void tAcquireData::_StoreData(tDataPoint &db, double value, uint32_t timestamp)
{
  // Update the Value of the Datapoint
  db.updateValue(value, timestamp);
}

//****************************************
// Measure all OneWire Sensors
void tAcquireData::measureOnewire()
{

  // Measure Wall Sensor Cooling
  oneWireSensors.requestTemperaturesByAddress(oWtCoolWall);
  double temp = oneWireSensors.getTempC(oWtCoolWall);
  _StoreData(tCoolWall, temp, millis());

  // Measure Engine Room Sensor
  oneWireSensors.requestTemperaturesByAddress(oWtEngRoom);
  temp = oneWireSensors.getTempC(oWtEngRoom);
  _StoreData(tEngRoom, temp, millis());

  // Measure Gearbox Sensor
  oneWireSensors.requestTemperaturesByAddress(oWtGearbox);
  temp = oneWireSensors.getTempC(oWtGearbox);
  _StoreData(tGearbox, temp, millis());
}

//****************************************
// Measure all voltages
void tAcquireData::measureVoltage()
{

  double voltage;


  // measure ESP32 AD-Channel
  voltage = analogRead(UBAT_ADC_PIN);
  voltage = ADC_CH34_LUT[(int)voltage];
  voltage = voltage / 4096 * ACH_CH34_FACTOR + ACH_CH34_OFFSET;
  this->_StoreData(this->uTest, voltage, millis());


}

//****************************************
//
void tAcquireData::showDataOnTerminal()
{
  uint8_t i = 1;

  // clear the terminal
  while (i-- > 0)
    Serial.println();

  tCoolWall.printDatapointFull();
  tGearbox.printDatapointFull();
  tEngRoom.printDatapointFull();
  uBat.printDatapointFull();
  uTest.printDatapointFull();
  uTest2.printDatapointFull();
}

//****************************************
// Listing of all OneWire Devices at the bus
void tAcquireData::listOneWireDevices()
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


//**********************************************
// Set up the Engine speed timer and interrupt
void tAcquireData::_setUpEngineSpeedInt(void)
{
  // Init the PIN for engine speed frequency
  pinMode(ENGINE_RPM_PIN, INPUT_PULLUP);    
  // attache the pin on falling edge to an interrupt and specify ISR 
  attachInterrupt(digitalPinToInterrupt(ENGINE_RPM_PIN), handleEngineSpeedInterrupt, FALLING); 

  // set up the engine speed timer
  // - the associated timer is TMR0
  // - prescaler of 80 gives 1ns as time per tick at 80Mhz
  // - timer counts upward 
  data.engSpeedCalc.Timer= timerBegin(0, TIMER_PRESCALER_FOR_1NS, true);                                            

  // start the timer
  timerStart(data.engSpeedCalc.Timer);
}

//**********************************************
// Set up the Engine speed timer and interrupt
void tAcquireData::_setUpShaftSpeedInt(void)
{
  // Init the PIN for engine speed frequency
  pinMode(SHAFT_RPM_PIN, INPUT_PULLUP);    
  // attache the pin on falling edge to an interrupt and specify ISR 
  attachInterrupt(digitalPinToInterrupt(SHAFT_RPM_PIN), handleShaftSpeedInterrupt, FALLING); 

  // set up the engine speed timer
  // - the associated timer is TMR0
  // - prescaler of 80 gives 1ns as time per tick at 80Mhz
  // - timer counts upward 
  data.shaftSpeedCalc.Timer= timerBegin(0, TIMER_PRESCALER_FOR_1NS, true);                                            

  // start the timer
  timerStart(data.shaftSpeedCalc.Timer);
}
//**********************************************
// Set up the Alternator 1 speed timer and interrupt
void tAcquireData::_setUpAlternator1SpeedInt(void)
{
  // Init the PIN for engine speed frequency
  pinMode(ALTERNATOR1_RPM_PIN, INPUT_PULLUP);    
  // attache the pin on falling edge to an interrupt and specify ISR 
  attachInterrupt(digitalPinToInterrupt(ALTERNATOR1_RPM_PIN), handleAlternator1SpeedInterrupt, FALLING); 

  // set up the engine speed timer
  // - the associated timer is TMR1
  // - prescaler of 80 gives 1ns as time per tick at 80Mhz
  // - timer counts upward 
  data.alternator1SpeedCalc.Timer= timerBegin(1, TIMER_PRESCALER_FOR_1NS, true);                                            

  // start the timer
  timerStart(data.alternator1SpeedCalc.Timer);
}

//**********************************************
// Set up the Alternator 2 speed timer and interrupt
void tAcquireData::_setUpAlternator2SpeedInt(void)
{
  // Init the PIN for engine speed frequency
  pinMode(ALTERNATOR2_RPM_PIN, INPUT_PULLUP);    
  // attache the pin on falling edge to an interrupt and specify ISR 
  attachInterrupt(digitalPinToInterrupt(ALTERNATOR2_RPM_PIN), handleAlternator2SpeedInterrupt, FALLING); 

  // set up the engine speed timer
  // - the associated timer is TMR2
  // - prescaler of 80 gives 1ns as time per tick at 80Mhz
  // - timer counts upward 
  data.alternator2SpeedCalc.Timer= timerBegin(1, TIMER_PRESCALER_FOR_1NS, true);                                            

  // start the timer
  timerStart(data.alternator2SpeedCalc.Timer);
}

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

//***********************************************
// Calculate rotational speed
double tAcquireData::calcNumberOfRevs(tSpeedCalc *tmrValues)
{
  double RPM = 0;
  // Lock the RAM and prevent other tasks from reading/writing
  taskENTER_CRITICAL(&tmrValues->muxTMR);
  if (tmrValues->PeriodCount != 0)
    // PeriodCount in 0.000001 of a second
    RPM = 1000000.00 / tmrValues->PeriodCount;
  // prevent div by zero if there is no signal
  if (millis() > tmrValues->TimestampLastInt + 500)
    RPM = 0; // No signals RPM=0;

  // Unlock the RAM
  taskEXIT_CRITICAL(&tmrValues->muxTMR);

  return (RPM);
}