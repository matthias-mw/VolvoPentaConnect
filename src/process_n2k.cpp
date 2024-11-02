// Doxygen Dokumentation
/*! \file 	process_n2k.cpp
 *  \brief  Processing all N2K Task on High Level
 *
 * \author 		Matthias Werner
 * \date		02/2023
 *
 * - Prozessor:         ESP32-WROOM
 * - Hardware:          az-delivery-devkit-v4
 **************************************************************/

#include <process_n2k.h>

//****************************************
// Setup for the Nk2 Module
void setupN2K()
{

  // build Version Numbers
  char hwVers[25] = HW_DESCRIPTION;
  char swVers[25];
  char tmp[5];

  snprintf(tmp, sizeof(tmp), " v%d  ", HW_VERSION_MAJOR);
  strncat(hwVers, tmp, sizeof(hwVers) - strlen(hwVers) - 1);
  strncat(hwVers, HW_VERSION_DATE, sizeof(hwVers) - strlen(hwVers) - 1);

  snprintf(swVers, sizeof(swVers), "%d.%d.%d  ", SW_VERSION_MAJOR, SW_VERSION_MINOR, SW_VERSION_PATCH);
  strncat(swVers, SW_VERSION_DATE, sizeof(swVers) - strlen(swVers) - 1);

  // Set Product information
  NMEA2000.SetProductInformation(N2K_MANUFACTURER_MODEL_SERIAL, // Manufacturer's Model serial code
                                 N2K_MANUFACTURER_PRODUCT_CODE, // Manufacturer's product code
                                 N2K_MANUFACTURER_MODEL_ID,     // Manufacturer's Model ID
                                 swVers,                        // Manufacturer's Software version code
                                 hwVers,                        // Manufacturer's Model version
                                 2                              // Load Equivalency 2x50mA
  );
  // Set device information
  NMEA2000.SetDeviceInformation(N2K_UNIQUE_DEVICE_ID, // Unique number for each device. Use e.g. Serial number.
                                160,                  // Device function=Engine Gateway
                                50,                   // Device class=Propulsion
                                174                   //  Manufacturer Code
  );

  // Uncomment 2 rows below to see, what device will send to bus. Use e.g. OpenSkipper or Actisense NMEA Reader

  // NMEA2000.SetForwardStream(&Serial);
  //  If you want to use simple ascii monitor like Arduino Serial Monitor, uncomment next line
  // NMEA2000.SetForwardType(tNMEA2000::fwdt_Text); // Show in clear text. Leave uncommented for default Actisense format.

  // If you also want to see all traffic on the bus use N2km_ListenAndNode instead of N2km_NodeOnly below
  NMEA2000.SetMode(tNMEA2000::N2km_NodeOnly, 0xAA);
  // NMEA2000.SetDebugMode(tNMEA2000::dm_Actisense); // Uncomment this, so you can test code without CAN bus chips on Arduino Mega
  // NMEA2000.EnableForward(false); // Disable all msg forwarding to USB (=Serial)
  //  Here we tell library, which PGNs we transmit
  NMEA2000.ExtendTransmitMessages(TransmitMessages);

  NMEA2000.Open();
}

//*************************************************************
// Sends out all the messages in a fast timeframe
void SendN2kEngineParmFast(VolvoPentaData *n2kVolvoData)
{
  tN2kMsg N2kMsg;

  static unsigned char EngineInstance = 0;
  static unsigned char EngineInstanceAlt1 = 1;
  
  // Data not measured now
  double EngineOilTemp = N2kDoubleNA;
  bool flagCheckEngine = false;
  bool flagLowFuelPress = false;

  // Check if the Semaphore used for Dataprotection is initialzed
  if (xMutexVolvoN2kData != NULL)
  {
    // See if we can obtain the semaphore. If the semaphore is not
    // available wait 5ms to see if it becomes free.
    if (xSemaphoreTake(xMutexVolvoN2kData, (TickType_t)5) == pdTRUE)
    {
      // Send exhaust temperature
      SetN2kTemperatureExt(N2kMsg, 0, 0, N2kts_ExhaustGasTemperature, (n2kVolvoData->exhaust_temperature));
      NMEA2000.SendMsg(N2kMsg);

      // Send engine speed
      SetN2kEngineParamRapid(N2kMsg, EngineInstance, n2kVolvoData->engine_speed, 0, 0);
      NMEA2000.SendMsg(N2kMsg);

      // send engine dynamic data
      SetN2kEngineDynamicParam(N2kMsg, EngineInstance, n2kVolvoData->engine_oel_pressure, EngineOilTemp, n2kVolvoData->engine_coolant_temperature, n2kVolvoData->batterie_voltage, N2kDoubleNA, n2kVolvoData->engine_seconds, N2kDoubleNA, N2kDoubleNA, N2kInt8NA, N2kInt8NA, flagCheckEngine);
      NMEA2000.SendMsg(N2kMsg);

      // send Balmar Alternator Data as "Instance 1"
      SetN2kEngineDynamicParam(N2kMsg, EngineInstanceAlt1, N2kDoubleNA, N2kDoubleNA, n2kVolvoData->alternator1_temperature, N2kDoubleNA, N2kDoubleNA, N2kDoubleNA, N2kDoubleNA, N2kDoubleNA, N2kInt8NA, N2kInt8NA, flagCheckEngine);
      NMEA2000.SendMsg(N2kMsg);
      // send Balmar Alternator Data as "Instance 1"
      SetN2kEngineParamRapid(N2kMsg, EngineInstanceAlt1, n2kVolvoData->alternator1_speed, 0, 0);
      NMEA2000.SendMsg(N2kMsg);

      // send gearbox data
      SetN2kPGN127493(N2kMsg, EngineInstance, N2kTG_Unknown, N2kDoubleNA, n2kVolvoData->gearbox_temperature, 0);
      NMEA2000.SendMsg(N2kMsg);
    }
    // unlock the resource again
    xSemaphoreGive(xMutexVolvoN2kData);
  }
}

//*************************************************************
// Sends out all the messages in a fast timeframe
void SendN2kEngineParmSlow(VolvoPentaData *n2kVolvoData)
{
  tN2kMsg N2kMsg;

  // Check if the Semaphore used for Dataprotection is initialzed
  if (xMutexVolvoN2kData != NULL)
  {
    // See if we can obtain the semaphore. If the semaphore is not
    // available wait 5ms to see if it becomes free.
    if (xSemaphoreTake(xMutexVolvoN2kData, (TickType_t)5) == pdTRUE)
    {
      // Send Temperature of Seewater Cool Pipe
      SetN2kTemperatureExt(N2kMsg, 0, 0, N2kts_HeatIndexTemperature, (n2kVolvoData->engine_coolant_temperature_wall));
      NMEA2000.SendMsg(N2kMsg);

      // unlock the resource again
      xSemaphoreGive(xMutexVolvoN2kData);
    }
  }
}