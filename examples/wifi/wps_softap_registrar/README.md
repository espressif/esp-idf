| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# Wi-Fi WPS Registrar Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example shows how to use WPS registrar sotAP mode in ESP. The WPS protocol simplifies the process of connecting to a Wi-Fi softAP from a station.

## How to use example

Before project configuration and build, be sure to set the correct chip target using `idf.py set-target <chip_name>`.

### Hardware Required

* A development board with ESP32/ESP32-S2/ESP32-C3/ESP32-S3 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming

### Configure the project

Open the project configuration menu (`idf.py menuconfig`).

In the `Example Configuration` menu:

* Use `WPS mode` to select the type.
* Select `PBC`, `PIN` or `disable`.

In `PBC` mode, the ESP will wait for the WPS initialization (usually by pressing WPS button on the Wi-Fi station).

In `PIN` mode, the ESP will enter the WPS mode and you'll see a pin code on the terminal(If not given through user config). Enter this pin code in your station and then the ESP will connect to it.

(See your station's manual and configuration for WPS compatibility.)

### Build and Flash

Build the project and flash it to the board, then run the monitor tool to view the serial output:

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for all the steps to configure and use the ESP-IDF to build projects.

* [ESP-IDF Getting Started Guide on ESP32](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html)
* [ESP-IDF Getting Started Guide on ESP32-S2](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/get-started/index.html)
* [ESP-IDF Getting Started Guide on ESP32-C3](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/get-started/index.html)
* [ESP-IDF Getting Started Guide on ESP32-S3](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/index.html)
* [ESP-IDF Getting Started Guide on ESP32-C2](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c2/get-started/index.html)


## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
