# GarduinoProject

## What is Garduino

Garduino is a project for switching some outputs on a timer basis. It is designed for watring the garden, hence the name.
It supports multiple outputs (Channels), which are connected to relays for switching loads (water valves, lights, etc.).
Each channel can have additional sensors (soil-moisture, rain, movement), to prevent or postpone activating the channel (e.g. do not water if it is raining).
For each channel there can be a start time and duration (or amount of liters, measured by a flow sensor) as well as a repeat interval (e.g. start 10:00, repeat every 12 hours, so watering will occure at 10:00 and 22:00).
There is also a posibility to let the start time be adjusted by the sun. A setpoint (sunrise, noon, sunset) and an offset (+/- X minutes) can be set, to adapt the watering hours over the course of the year (e.g. always water 1 hour before sunset).
It can be configured via config file on SD and also features a simple webserver, deployed on a ESP8266.

## Hardware

* Board: Wemos Mega 2560 with integrated ESP8266
* Shield: Arduino Sensor Shield v2.0 (for easier wiring)
* Display: RepRapDiscount Full Graphic Smart Controller
* RTC: DS3231 (SPI)
* Temp./Hum. Sensor: AM 2321 (SPI) (for monitoring the enclosure temperature)
* Relays: 2x 4xRely-Board 5V
* Valves: 4x Seleonid valves 5V
* Flow-Sensor: Inline water flow sensor (stop watering after X liter)
* Fan: 80mm 12V radial fan (for cooling the enclosure)
* Buttons: 4x Watertight buttons with 5V LED (for manually de-/activating the valves)
* Outlets: 4x 240V outlets to connect low loads (garden lights, waterpump)
* Powersupply: 240V -> 12V & 5V


## Dependencies

### Standard Arduino Libraries

* SPI
* SD
* EEPROM
* Wire

### Eternal Libraries

* AM232X      (https://github.com/RobTillaart/AM232X)
* DS3232RTC   (https://github.com/JChristensen/DS3232RTC)
* FileConfig  (https://github.com/Chade/FileConfig)
* LCDMenuLib2 (https://github.com/Jomelo/LCDMenuLib2)
* Time        (https://github.com/PaulStoffregen/Time)
* Timezone    (https://github.com/JChristensen/Timezone)
* U8g2        (https://github.com/olikraus/U8g2_Arduino)

