| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# NMEA Parser Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example will show how to parse NMEA-0183 data streams output from GPS/BDS/GLONASS modules based on ESP UART Event driver and ESP event loop library.
For the convenience of the presentation, this example will only parse the following basic statements:
* GGA
* GSA
* GSV
* RMC
* GLL
* VTG

See [Limitation for multiple navigation system](#Limitation) for more information about this example.

Usually, modules will also output some vendor specific statements which common nmea library can not cover. In this example, the NMEA Parser will propagate all unknown statements to the user, where a custom handler can parse information from it.

## How to use example

### Hardware Required

To run this example, you need a dev board that is based on Espressif SoC (e.g. ESP32-WROVER Kit). For test purpose, you also need a GPS module. Here we take the [ATGM332D-5N](http://www.icofchina.com/pro/mokuai/2016-08-01/5.html) as an example to show how to parse the NMEA statements and output common information such as UTC time, latitude, longitude, altitude, speed and so on.

#### Pin Assignment:

**Note:** GPIO5 is used by default as the RX pin, you can change it by `idf.py menuconfig` > `Example Configuration` > `NMEA_PARSER_UART_RXD`.

| ESP                        | GPS             |
| -------------------------- | --------------- |
| UART-RX (GPIO5 by default) | GPS-TX          |
| GND                        | GND             |
| 5V                         | VCC             |

**Note:** UART TX pin is not necessary if you only use UART to receive data.


### Configure the project

Open the project configuration menu (`idf.py menuconfig`). Then go into `Example Configuration` menu.

- Set the size of ring buffer used by uart driver in `NMEA Parser Ring Buffer Size` option.
- Set the stack size of the NMEA Parser task in `NMEA Parser Task Stack Size` option.
- Set the priority of the NMEA Parser task in `NMEA Parser Task Priority` option.
- In the `NMEA Statement support` submenu, you can choose the type of statements that you want to parse. **Note:** you should choose at least one statement to parse.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build and flash the project..

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```bash
I (0) cpu_start: Starting scheduler on APP CPU.
I (317) uart: queue free spaces: 16
I (317) nmea_parser: NMEA Parser init OK
I (1067) gps_demo: 2018/12/4 13:59:34 =>
						latitude   = 31.20177°N
						longitude = 121.57933°E
						altitude   = 17.30m
						speed      = 0.370400m/s
W (1177) gps_demo: Unknown statement:$GPTXT,01,01,01,ANTENNA OK*35
I (2067) gps_demo: 2018/12/4 13:59:35 =>
						latitude   = 31.20177°N
						longitude  = 121.57933°E
						altitude   = 17.30m
						speed      = 0.000000m/s
W (2177) gps_demo: Unknown statement:$GPTXT,01,01,01,ANTENNA OK*35
I (3067) gps_demo: 2018/12/4 13:59:36 =>
						latitude   = 31.20178°N
						longitude  = 121.57933°E
						altitude   = 17.30m
						speed      = 0.000000m/s
W (3177) gps_demo: Unknown statement:$GPTXT,01,01,01,ANTENNA OK*35
I (4067) gps_demo: 2018/12/4 13:59:37 =>
						latitude   = 31.20178°N
						longitude  = 121.57933°E
						altitude   = 17.30m
						speed      = 0.000000m/s
W (4177) gps_demo: Unknown statement:$GPTXT,01,01,01,ANTENNA OK*35
I (5067) gps_demo: 2018/12/4 13:59:38 =>
						latitude   = 31.20178°N
						longitude  = 121.57933°E
						altitude   = 17.30m
						speed      = 0.685240m/s
W (5177) gps_demo: Unknown statement:$GPTXT,01,01,01,ANTENNA OK*35
```
As shown above, the ESP board finally got the information after parsed the NMEA0183 format statements. But as we didn't add `GPTXT` type statement in the library (that means it is UNKNOWN to NMEA Parser library), so it was propagated to user without any process.

## Troubleshooting

1. I cannot receive any statements from GPS although I have checked all the pin connections.
   * Test your GPS via other terminal (e.g. minicom, putty) to check the right communication parameters (e.g. baudrate supported by GPS).

## Limitation
If the GPS module supports multiple satellite navigation system (e.g. GPS, BDS), then the satellite ids and descriptions may be delivered in different statements (e.g. GPGSV, BDGSV, GPGSA, BDGSA), depend on the version of NMEA protocol used by the GPS module. This example currently can only record id and description of satellites from one navigation system.
However, for other statements, this example can parse them correctly whatever the navigation system is.

### Steps to skip the limitation
1. Uncheck the `GSA` and `GSV` statements in menuconfig
2. In the `gps_event_handler` will get a signal called `GPS_UNKNOWN`, and the unknown statement itself (It's a deep copy of the original statement).
3. Manually parse the unknown statements and get the satellites' descriptions.

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
