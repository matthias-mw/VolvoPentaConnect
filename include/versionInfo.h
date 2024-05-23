// Doxygen Dokumentation
/*! \file 	versionInfo.h
 *  \brief  Definition of Device and Software Version Infos
 *  
 * Single Point of information for device and version control
 * related information.
 *
 * \author 	Matthias Werner
 * \date		Sep 2023
 *
 * - Hardware:          az-delivery-devkit-v4
 * - Prozessor:         ESP32-WROOM
 *
 * ************************************************************
 * \version
 *	1.0		
 *
 *************************************************************/

#ifndef _versionInfo_H_
#define _versionInfo_H_

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//     --- General version control --- 
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

/// Major version number of the software project
#define SW_VERSION_MAJOR 0
/// Minor version number of the software project
#define SW_VERSION_MINOR 2
/// Patch version number of the software project
#define SW_VERSION_PATCH 0
/// Date for current version of the software project
#define SW_VERSION_DATE  "05-2024"


/// Major version number of the hardware project
#define HW_VERSION_MAJOR 1
/// Description of the hardware project
#define HW_DESCRIPTION "VPM" 
/// Date for current version of the hardware project
#define HW_VERSION_DATE  "05-2023"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//     --- NMEA2000 related Device Information  --- 
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

/// NMEA2000 Manufacturer's Model serial code
#define N2K_MANUFACTURER_MODEL_SERIAL "1234"
/// NMEA2000 Manufacturer's product code
#define N2K_MANUFACTURER_PRODUCT_CODE 1
/// NMEA2000 Manufacturer's Model ID
#define N2K_MANUFACTURER_MODEL_ID "D2-55 Monitor"
/// NMEA2000 Unique ID for each produced device
#define N2K_UNIQUE_DEVICE_ID 2




#endif //_versionInfo_H_