# Websocket Sample application

(See the README.md file in the upper level 'examples' directory for more information about examples.)
This example will shows how to set up and communicate over a websocket.

## How to Use Example

### Hardware Required

This example can be executed on any ESP32 board, the only required interface is WiFi and connection to internet or a local server.

### Configure the project

* Open the project configuration menu (`idf.py menuconfig`)
* Configure Wi-Fi or Ethernet under "Example Connection Configuration" menu. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](../../README.md) for more details.
* Configure the websocket endpoint URI under "Example Configuration", if "WEBSOCKET_URI_FROM_STDIN" is selected then the example application will connect to the URI it reads from stdin (used for testing)

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (482) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (2492) example_connect: Ethernet Link Up
I (4472) tcpip_adapter: eth ip: 192.168.2.137, mask: 255.255.255.0, gw: 192.168.2.2
I (4472) example_connect: Connected to Ethernet
I (4472) example_connect: IPv4 address: 192.168.2.137
I (4472) example_connect: IPv6 address: fe80:0000:0000:0000:bedd:c2ff:fed4:a92b
I (4482) WEBSOCKET: Connecting to ws://echo.websocket.org...
I (5012) WEBSOCKET: WEBSOCKET_EVENT_CONNECTED
I (5492) WEBSOCKET: Sending hello 0000
I (6052) WEBSOCKET: WEBSOCKET_EVENT_DATA
W (6052) WEBSOCKET: Received=hello 0000

I (6492) WEBSOCKET: Sending hello 0001
I (7052) WEBSOCKET: WEBSOCKET_EVENT_DATA
W (7052) WEBSOCKET: Received=hello 0001

I (7492) WEBSOCKET: Sending hello 0002
I (8082) WEBSOCKET: WEBSOCKET_EVENT_DATA
W (8082) WEBSOCKET: Received=hello 0002

I (8492) WEBSOCKET: Sending hello 0003
I (9152) WEBSOCKET: WEBSOCKET_EVENT_DATA
W (9162) WEBSOCKET: Received=hello 0003

```

