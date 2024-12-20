# Mainpage  {#mainpage}

VolvoPenta Connect --- Connecting an VolvoPenta D2 55 to the NMEA2000

## Description

This Project features a device, which connects all the important data of a
VolvoPenta D2-55 diesel engine to the NMEA2000 bus.

### Hardware Description

The hardware of the devices features the following parts

- ESP32 Development board ESP32-WROOM (az-delivery-devkit-v4)
- 2x Speed Input (engine, driveshaft) via hall or IR
- 2x alternator speed via W signal and Optocoupler
- 4x voltage measurement (oil pressure, engine temp etc) via MCP3204
- 1wire for DS18S20 temperature sensors (sea water, gearbox, ...)
- 1x NiCr-Ni Thermocouple via Max6675 (exhaust)
- 2x contact input via Opto Coupler (future use)
- 1x contact 3.3V highside (future use)
- 1x relay output for switching (future use)
- 1x Buzzer for Alarms
- 2x Buttons
- 1x i2c for LCD Display
- 1x isolated NMEA2000 connection via ISO1050

### Software Description

The software measures all the signals, calculates alarmsand transfer them to the
NMEA2000 network. The NMEA2000 part uses the following Software

[NMEA2000 library](https://ttlappalainen.github.io/NMEA2000/changes.html)

In order to have a possibility to see all values and warnings directly, there
is a 4x20 LCD Panel via i2c included
