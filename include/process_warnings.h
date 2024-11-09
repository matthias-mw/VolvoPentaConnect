// Doxygen Documentation
/*! \file 	process_warnings.h
 *  \brief  Header file for processing warnings
 *
 * This file contains the declarations for functions and classes
 * used to process warnings in the system.
 *
 * \author 		Matthias Werner
 * \date		November 2024
 * - Prozessor:         ESP32-WROOM
 * - Hardware:          az-delivery-devkit-v4
 */

#ifndef PROCESS_WARNINGS_H
#define PROCESS_WARNINGS_H

#include <Arduino.h>
#include <hardwareDef.h>
#include <acquire_data.h>

/// Delay counter for flashing LED
#define WARNING_LED_DELAY 500


/*! \class ProcessWarnings
 *  \brief A class to handle and process system warnings.
 * 
 * This class contains methods process and log all 
 * system warnings. Therfore accepts warnings from the 
 * data acquisition object and processes them.
 */
class ProcessWarnings
{ 
public:
    /*!
     * \brief Constructor for ProcessWarnings.
     * \param data Reference to an AcquireData object.
     */
    ProcessWarnings( AcquireData &data);

    /*!
     * \brief checks and processes all warnings.
     *
     * This method check and processes all warnings from the 
     * data acquisition object.
     * 
     */
    void checkWarnings();


private:
    
    /*!
     * \brief operate warning LED
     * This method operates the warning LED and flashes it,
     * depending on the given state.
     * 
     * \param warningActive {bool}  State of the warning
     */
    void operateWarningLED( bool warningActive);

    /*! Counter for flashing the warning LED */
    uint32_t warningLedCounter = 0;    

    /// state of the warning LED
    bool warningLedState = false;

    /// Reference to an AcquireData object with all the sensor data.
    AcquireData &data;

};

#endif // PROCESS_WARNINGS_H