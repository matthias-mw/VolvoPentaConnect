// Doxygen Documentation
/*! \file 	errorflag.cpp
 *  \brief  Header file for handling error flags
 *
 * This file contains the declarations for functions and classes
 * used to handle an error flag in the system.
 *
 * \author 		Matthias Werner
 * \date		November 2024
 * - Prozessor:         ESP32-WROOM
 * - Hardware:          az-delivery-devkit-v4
 */

#include <errorflag.h>


// ********************************************************
// Constructor
ErrorFlag::ErrorFlag()
{
    // Initialization code
    flagStatus = false;
    flagSetTimeStamp = 0;
}

