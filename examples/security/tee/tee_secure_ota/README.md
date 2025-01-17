| Supported Targets | ESP32-C6 |
| ----------------- | -------- |

# TEE: Secure OTA example

## Overview

- This example illustrates a secure Over-The-Air (OTA) update for the TEE app as well as the non-secure app.
- For the TEE update, the REE fetches the new TEE image from the HTTP/S server and securely writes it to the TEE passive partition using the secure service call interface. It also updates the TEE OTA data partition for switching the active TEE partition for subsequent boots.
- During the next boot, if the TEE image self-test fails, the device will revert to the old TEE image. Otherwise, the passive TEE partition will be erased, and the device will proceed to boot into the REE normally.
- The user OTA workflow is based on the `esp_https_ota` component APIs.

## How to use the example

### Hardware Required

This example can be executed on any development board with a Espressif SOC chip supporting the TEE framework (see Supported Targets table above).

### Configure the project

Before the project configuration and build, be sure to set the correct chip target using `idf.py set-target <chip_name>`.

Open the project configuration menu (`idf.py menuconfig`).

- Set the Wi-Fi SSID and password at `(Top) → Example Connection Configuration → WiFi SSID / WiFi Password`

### Build and Flash

Build the project and flash it to the board, then run the monitor tool to view the serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type `Ctrl-]`.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

### Usage

- Use console commands to perform OTA for the TEE app as well as the non-secure app: `tee_ota <img_url>` or `user_ota <img_url>`

#### Notes

- This example uses the ESP-IDF certificate bundle by default, so you can host your image online and download it directly.
- For testing purposes, if you want to start a local HTTP/S server, you can refer to the guide in OTA examples parent directory - see [here](../../../system/ota/). Note that you will need to embed the CA certificate in the firmware for such cases.
