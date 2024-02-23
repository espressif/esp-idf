| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Async Requests Handlers HTTPD Server Example

The Example demonstrates how to handle multiple long running simultaneous requests
within the HTTPD server. It has the following URIs:

    1. URI \long for demonstrating async requests running in the background
    2. URI \quick for demonstrating that quick requests are still responsive
    2. URI \ index page

## How to use example

### Hardware Required

* An ESP-Dev-Board (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for power supply and programming

### Configure the project

```
idf.py menuconfig
```
* Open the project configuration menu (`idf.py menuconfig`) to configure Wi-Fi or Ethernet. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](../../README.md) for more details.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (14257) example_common: Connected to example_netif_sta
I (14267) example_common: - IPv4 address: 192.168.20.85,
I (14277) example_common: - IPv6 address: fe80:0000:0000:0000:7edf:a1ff:fea4:3454, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (14287) example: starting async req task worker
I (14287) example: starting async req task worker
I (14297) example: Starting server on port: '80'
I (14307) example: Registering URI handlers
I (14307) main_task: Returned from app_main()
I (15547) wifi:<ba-add>idx:1 (ifx:0, 68:d7:9a:81:26:1e), tid:0, ssn:0, winSize:64
I (19627) example: uri: /
I (25877) example: uri: /quick
I (33247) example: uri: /long
I (33247) example: invoking /long
I (33247) example: uri: /long
```
