// Doxygen Dokumentation
/*! \file 	process_n2k.h
 *  \brief  Processing all N2K Task on High Level
 *
 * This File contains all the necessary high level methods to handle the 
 * NMEA2000 connectivity and all Data from and to the bus.
 *
 * \author 		Matthias Werner
 * \date		02/2023
 *
 * - Prozessor:         ESP32-WROOM
 * - Hardware:          az-delivery-devkit-v4
*/
#ifndef PROCESS_N2K_H
#define PROCESS_N2K_H

#include <Arduino.h>
#include <versionInfo.h>
#include <hardwareDef.h>
#include <NMEA2000.h>
#include <N2kMessages.h>

/// NMEA2000 Object from <NMEA2000_CAN.h>
extern tNMEA2000 &NMEA2000;
/// Mutex for protecting data integrity of \ref VolvoDataForN2k
extern SemaphoreHandle_t xMutexVolvoN2kData;

/// List of messages the device will transmit.
const unsigned long TransmitMessages[] PROGMEM={127493L,127489L,127488L,130316L,0};

/*! ************************************************************************
 * \struct  VolvoPentaData
 * \brief   Structure contains all data ready to be send to N2K bus
 *
 * The data in this struct has been collected and converted (units etc...)
 * to be directly used inside the N2K functions.
 */
typedef struct VolvoPentaData{

  /// engine coolant temperature in kelvin
  double engine_coolant_temperature = 0;
  /// engine coolant temperature at the pipe in kelvin
  double engine_coolant_temperature_wall = 0;
  /// engine oel pressure in Pascal
  double engine_oel_pressure = 0;
  /// voltage of the starter batterie in volt
  double battery_voltage = 0;
  /// Alternator 1 temperature in kelvin
  double alternator1_temperature = 0;
  /// exhaust gas temperature in kelvin
  double exhaust_temperature = 0;
  /// gearbox temperature in kelvin
  double gearbox_temperature = 0;
  /// engine speed in rev per minute
  double engine_speed = 0;
  /// prop shaft speed in rev per minute
  double shaft_speed = 0;
  /// alternator 1 speed in rev per minute
  double alternator1_speed = 0;
  /// alternator 2 speed in rev per minute
  double alternator2_speed = 0;
  /// engine hours run in seconds
  double engine_seconds = 0;
  /// engine status bits (1)
  tN2kEngineDiscreteStatus1 engineDiscreteStatus1;
  /// engine status bits (2)
  tN2kEngineDiscreteStatus2 engineDiscreteStatus2;
  /// alternator status bits (1)
  tN2kEngineDiscreteStatus1 alternatorDiscreteStatus1;

}tVolvoPentaData;

/*! ************************************************************************
 * \brief Setup for the Nk2 Module
 * 
 * This method sets up all the necessary information for the NMEA2000 
 * communication, such as manufacturer and device informations.
 *
 */
void setupN2K();

/*! ************************************************************************
 * \brief Sends out all the messages in a fast timeframe
 * 
 * This method sends out all N2k Messages in a fast task \ref taskMeasureFast
 * which measures all fast signals and sends them out.
 *
 * \param data contains all measured engine data
 */
void SendN2kEngineParmFast(VolvoPentaData *data);

/*! ************************************************************************
 * \brief Sends out all the messages in a slow timeframe
 * 
 * This method sends out all N2k Messages in a slow task which measures
 * all Onewire Sensors  in \ref taskMeasureOneWire
 *
 * \param data contains all measured engine data
 */
void SendN2kEngineParmSlow(VolvoPentaData *data);

#endif // PROCESS_N2K_H