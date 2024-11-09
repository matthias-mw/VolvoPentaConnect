
// Doxygen Documentation
/*! \file 	errorflag.h
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

#ifndef ERRORFLAG_H
#define ERRORFLAG_H

#include <Arduino.h>

/*! \class ErrorFlag
  * \brief A class to handle error flags.
  *   
  *   This class contains methods to set, reset and check the status of an error flag.
  */
class ErrorFlag {
public:
    /*!
     * \brief Constructor for ErrorFlag.
     */
    ErrorFlag();

    /*!
     * \brief Method to set the error flag.
     * This Method sets the error flag and stores the timestamp 
     * when the flag was set.
     */
    void setFlag(){flagStatus = true; flagSetTimeStamp = millis();};

    /*!
     * \brief Method to reset the error flag.
     */
    void resetFlag() {flagStatus = false;};

    /*!
     * \brief Method to check if the flag is set.
     * \return True if the flag is set, false otherwise.
     */
    bool isFlagSet(){return flagStatus;};

    /*!
     * \brief Method to get the timestamp when the flag was set.
     * \return Time in milliseconds when the flag was set.
     */
    unsigned long getTimeStampWhenSet() {return flagSetTimeStamp;};

    /*!
     * \brief Method to check if the error flag is acknowledged.
     * \return True if the flag is acknowledged, false otherwise.
     */
    bool isFlagAcknowledged() {return flagIsAcknowledged;};

    /*!
     * \brief Method to set the flag as served.
     */
    void acknowledgeFlag() {flagIsAcknowledged = true;};

private:

    /// Status of the flag
    bool flagStatus; 

    /// Timestamp when the flag was set
    uint32_t flagSetTimeStamp;

    /// Info if the flag is acknowledged
    bool flagIsAcknowledged; 
};


#endif // ERRORFLAG_H