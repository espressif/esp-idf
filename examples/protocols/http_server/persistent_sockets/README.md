| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# HTTPD Server Persistent Sockets Example

The Example consists of HTTPD server persistent sockets demo.
This sort of persistency enables the server to have independent sessions/contexts per client.

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

### Test the example

In order to test the HTTPD server persistent sockets demo :

* run the test script "python scripts/adder.py \<IP\> \<port\> \<N\>"
* the provided test script sends (POST) numbers from 1 to N to the server which has a URI POST handler for adding these numbers into an accumulator that is valid throughout the lifetime of the connection socket, hence persistent
* the script does a GET before closing and displays the final value of the accumulator

See the README.md file in the upper level 'examples' directory for more information about examples.

## Example Output

```
I (9570) example_connect: - IPv4 address: 192.168.194.219
I (9580) example_connect: - IPv6 address: fe80:0000:0000:0000:266f:28ff:fe80:2c74, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (9590) example: Starting server on port: '80'
I (9600) example: Registering URI handlers
I (12860) example: /adder visitor count = 1
I (12860) example: /adder PUT handler read 0
I (12860) example: /adder PUT allocating new session
I (13070) example: /adder visitor count = 2
I (13070) example: /adder handler read 1
I (13090) example: /adder visitor count = 3
I (13090) example: /adder handler read 2
I (13110) example: /adder visitor count = 4
I (13110) example: /adder handler read 3
I (13170) example: /adder visitor count = 5
I (13170) example: /adder handler read 4
I (13190) example: /adder visitor count = 6
I (13190) example: /adder handler read 5
I (13210) example: /adder visitor count = 7
I (13210) example: /adder handler read 6
I (13270) example: /adder visitor count = 8
I (13270) example: /adder handler read 7
I (13290) example: /adder visitor count = 9
I (13290) example: /adder handler read 8
I (13300) example: /adder visitor count = 10
I (13310) example: /adder handler read 9
I (13370) example: /adder visitor count = 11
I (13370) example: /adder handler read 10
I (13390) example: /adder visitor count = 12
I (13390) example: /adder GET handler send 55
I (13420) example: /adder Free Context function called
```
