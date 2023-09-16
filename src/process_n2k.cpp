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

void setupN2K()
{

  //build Version Numbers
  char hwVers [25] = HW_DESCRIPTION;
  char swVers [25];
  char tmp[5];

  snprintf(tmp,sizeof(tmp), " v%d  ", HW_VERSION_MAJOR);
  strncat(hwVers, tmp ,sizeof(hwVers) - strlen(hwVers) - 1);
  strncat(hwVers, HW_VERSION_DATE ,sizeof(hwVers) - strlen(hwVers) - 1);
  
  snprintf(swVers,sizeof(swVers), "%d.%d.%d  ",SW_VERSION_MAJOR,SW_VERSION_MINOR,SW_VERSION_PATCH);
  strncat(swVers, SW_VERSION_DATE ,sizeof(swVers) - strlen(swVers) - 1);

  // Set Product information
  NMEA2000.SetProductInformation(N2K_MANUFACTURER_MODEL_SERIAL, // Manufacturer's Model serial code
                                 N2K_MANUFACTURER_PRODUCT_CODE, // Manufacturer's product code
                                 N2K_MANUFACTURER_MODEL_ID,   // Manufacturer's Model ID
                                 swVers, // Manufacturer's Software version code
                                 hwVers, // Manufacturer's Model version
                                 2                       // Load Equivalency 2x50mA
  );
  // Set device information
  NMEA2000.SetDeviceInformation(N2K_UNIQUE_DEVICE_ID,  // Unique number for each device. Use e.g. Serial number.
                                160,    // Device function=Engine Gateway
                                50,     // Device class=Propulsion 
                                174    //  Manufacturer Code 
  );

  // Uncomment 2 rows below to see, what device will send to bus. Use e.g. OpenSkipper or Actisense NMEA Reader

  // NMEA2000.SetForwardStream(&Serial);
  //  If you want to use simple ascii monitor like Arduino Serial Monitor, uncomment next line
  // NMEA2000.SetForwardType(tNMEA2000::fwdt_Text); // Show in clear text. Leave uncommented for default Actisense format.

  // If you also want to see all traffic on the bus use N2km_ListenAndNode instead of N2km_NodeOnly below
  NMEA2000.SetMode(tNMEA2000::N2km_NodeOnly, 0x22);
  // NMEA2000.SetDebugMode(tNMEA2000::dm_Actisense); // Uncomment this, so you can test code without CAN bus chips on Arduino Mega
  // NMEA2000.EnableForward(false); // Disable all msg forwarding to USB (=Serial)
  //  Here we tell library, which PGNs we transmit
  NMEA2000.ExtendTransmitMessages(TransmitMessages);

  NMEA2000.Open();
}

#define TempUpdatePeriod 500

void SendN2kEngineParm(VolvoPentaData data)
{
  static unsigned long TempUpdated = millis();
  tN2kMsg N2kMsg;

  unsigned char EngineInstance = 0;


  //Data not measured now
  double EngineOilTemp = 57;
  double FuelRate = 0;
  bool flagCheckEngine = false;
  bool flagLowFuelPress = false;



  if (TempUpdated + TempUpdatePeriod < millis())
  {
    TempUpdated = millis();

    // Send Engine room temperature
    SetN2kTemperatureExt(N2kMsg, 0, 0, N2kts_EngineRoomTemperature, (data.engine_room_temperature));
    NMEA2000.SendMsg(N2kMsg);

    // Send exhaust temperature
    SetN2kTemperatureExt(N2kMsg, 0, 0, N2kts_ExhaustGasTemperature, (data.exhaust_temperature));
    NMEA2000.SendMsg(N2kMsg);

    // Send engine speed
    SetN2kEngineParamRapid(N2kMsg, EngineInstance, data.engine_speed, 0, 0);
    NMEA2000.SendMsg(N2kMsg);

    // send engine data
    SetN2kEngineDynamicParam(N2kMsg, EngineInstance, data.engine_oel_pressure, EngineOilTemp, data.engine_coolant_temperature, data.batterie_voltage, FuelRate, data.engine_hours, N2kDoubleNA, N2kDoubleNA, N2kInt8NA, N2kInt8NA, flagCheckEngine);
    NMEA2000.SendMsg(N2kMsg);

    // send gearbox data
    SetN2kPGN127493 (N2kMsg, EngineInstance,N2kTG_Unknown, N2kDoubleNA, data.gearbox_temperature,0);
    NMEA2000.SendMsg(N2kMsg);

    // Serial.print(millis()); Serial.println(", Temperature send ready");
  }
}