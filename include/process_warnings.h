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

// Forward declaration
class AcquireData;

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
    ProcessWarnings(AcquireData &data);

    /*!
     * \brief checks and processes all warnings.
     *
     * This method check and processes all warnings from the
     * data acquisition object. It checks if a warning is active
     * and if so, it operates the warning LED.
     *
     */
    void checkAndProcessWarnings();

    /*!
     * \brief acknowledge all warnings
     * This method acknowledges all warnings and resets the
     * warning flags.
     *
     */
    void acknowledgeAllWarnings();

    /*!
     * \brief Checks if there is an active warning.
     * This method checks if there is an active warning which is
     * not acknowledged.
     *
     * \return {bool} True if there is an active warning which is
     * not acknowledged.
     */
    bool isWarningActive() { return warningActive; }

private:
    /*!
     * \brief operate warning LED
     * This method operates the warning LED and flashes it,
     * depending on the given state.
     *
     * \param warningActive {bool}  State of the warning
     */
    void operateWarningLED(bool warningActive);

    /*!
     * \brief operate the warning buzzer
     * This method operates the warning buzzer and sounds it,
     * depending on the given state.
     *
     * \param warningActive {bool}  State of the warning
     */
    void operateWarningBuzzer(bool warningActive);

    /*! Counter for flashing the warning LED */
    uint32_t warningLedCounter = 0;

    /// state of the warning LED
    bool warningLedState = false;

    /// there is an active warning
    bool warningActive = false;

    /// bedounce value for the warning
    uint32_t warningDebounce = 0;

    /// Reference to an AcquireData object with all the sensor data.
    AcquireData &data;
};

#endif // PROCESS_WARNINGS_H