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

  // Set Product information
  NMEA2000.SetProductInformation("12345678",             // Manufacturer's Model serial code
                                 100,                    // Manufacturer's product code
                                 "VolvoPenta Monitor",   // Manufacturer's Model ID
                                 "0.0.0.1 (2022-12-04)", // Manufacturer's Software version code
                                 "0.0.0.1 (2022-12-04)"  // Manufacturer's Model version
  );
  // Set device information
  NMEA2000.SetDeviceInformation(001500, // Unique number. Use e.g. Serial number.
                                160,    // Device function=Engine Gateway. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                50,     // Device class=Propulsion See codes on  http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                2040    // Just choosen free from code list on http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf
  );
  // Uncomment 2 rows below to see, what device will send to bus. Use e.g. OpenSkipper or Actisense NMEA Reader

  // NMEA2000.SetForwardStream(&Serial);
  //  If you want to use simple ascii monitor like Arduino Serial Monitor, uncomment next line
  // NMEA2000.SetForwardType(tNMEA2000::fwdt_Text); // Show in clear text. Leave uncommented for default Actisense format.

  // If you also want to see all traffic on the bus use N2km_ListenAndNode instead of N2km_NodeOnly below
  NMEA2000.SetMode(tNMEA2000::N2km_NodeOnly, 0xFE);
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
    SetN2kTemperature(N2kMsg, 0, 0, N2kts_EngineRoomTemperature, (data.engine_room_temperature + 273.15));
    NMEA2000.SendMsg(N2kMsg);

    // Send exhaust temperature
    SetN2kTemperature(N2kMsg, 0, 0, N2kts_ExhaustGasTemperature, (data.exhaust_temperature));
    NMEA2000.SendMsg(N2kMsg);




    // Send engine speed
    SetN2kEngineParamRapid(N2kMsg, EngineInstance, data.engine_speed, 0, 0);
    NMEA2000.SendMsg(N2kMsg);

    // send engine data
    SetN2kEngineDynamicParam(N2kMsg, EngineInstance, data.oel_pressure, EngineOilTemp, data.coolant_temperature + 273.15, data.batterie_voltage, FuelRate, data.engine_hours, N2kDoubleNA, N2kDoubleNA, N2kInt8NA, N2kInt8NA, flagCheckEngine);
    NMEA2000.SendMsg(N2kMsg);

    // send gearbox data
    SetN2kPGN127493 (N2kMsg, EngineInstance,N2kTG_Unknown, N2kDoubleNA, data.exhaust_temperature,0);
    NMEA2000.SendMsg(N2kMsg);

    // Serial.print(millis()); Serial.println(", Temperature send ready");
  }
}