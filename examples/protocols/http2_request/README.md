# HTTP/2 Request Example

Establish an HTTP/2 connection with https://http2.github.io
- Performs a GET on /index.html

## How to use example
Before project configuration and build, be sure to set the correct chip target using `idf.py set-target <chip_name>`.

### Hardware Required

* A development board with ESP32/ESP32-S2/ESP32-C3 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for power supply and programming

### Configure the project

```
idf.py menuconfig
```
Open the project configuration menu (`idf.py menuconfig`) to configure Wi-Fi or Ethernet. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](../../README.md) for more details.

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
I (5609) example_connect: - IPv4 address: 192.168.0.103
I (5609) example_connect: - IPv6 address: fe80:0000:0000:0000:ae67:b2ff:fe45:0194, type: ESP_IP6_ADDR_IS_LINK_LOCAL
Connecting to server
Connection done
[get-response] <!DOCTYPE HTML>
<html lang="en">
.
.
.
Body of index.html
.
.
.
.
</html>
[get-response] Frame fully received
[get-response] Stream Closed
```
