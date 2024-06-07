| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# HTTP server advanced tests

(See the README.md file in the upper level 'examples' directory for more information about examples.)

HTTP server example to perform some advanced tests

## How to use example

### Hardware Required

* A development board with ESP32/ESP32-S2/ESP32-C3 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
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
I (5561) example_connect: - IPv4 address: 192.168.194.219
I (5561) example_connect: - IPv6 address: fe80:0000:0000:0000:266f:28ff:fe80:2c74, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (5581) TESTS: Started HTTP server on port: '1234'
I (5581) TESTS: Max URI handlers: '9'
I (5581) TESTS: Max Open Sessions: '7'
I (5591) TESTS: Max Header Length: '512'
I (5591) TESTS: Max URI Length: '512'
I (5601) TESTS: Max Stack Size: '4096'
I (5601) TESTS: Registering basic handlers
I (5601) TESTS: No of handlers = 9
I (5611) TESTS: Success
```
