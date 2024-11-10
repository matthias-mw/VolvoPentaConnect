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
  resetFlag();
}

// ********************************************************
// Acknowledge the error flag
void ErrorFlag::acknowledgeFlag()
{
  flagIsAcknowledged = true;
}

// ********************************************************
// Set the error flag
void ErrorFlag::setFlag()
{ 
  flagStatus = true;
  flagSetTimeStamp = millis();
}

// ********************************************************
// Reset the error flag
void ErrorFlag::resetFlag()
{
  flagStatus = false;
  flagSetTimeStamp = 0;
  flagIsAcknowledged = false;
}

// ********************************************************
// Check if the error flag is set
bool ErrorFlag::isFlagSet()
{
  return flagStatus;
}

// ********************************************************
// Check if the error flag is acknowledged
bool ErrorFlag::isFlagAcknowledged()
{
  return flagIsAcknowledged;
}

// ********************************************************
// Get the timestamp when the error flag was set
unsigned long ErrorFlag::getTimeStampWhenSet()
{
  return flagSetTimeStamp;
}
