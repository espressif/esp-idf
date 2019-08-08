# PPP over Serial (PPPoS) client example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

A general PPP application consists of two parts: PPP server which is provided by cellular modem module and PPP client which is provided by ESP32 in this example.
Standard operating systems like Windows and Unix integrate a full PPP stack and provide a way to setup PPP connection at the same time. But how can we get access to Internet by PPP protocol in a resource constrained system? Fortunately, the PPP protocol has already been implemented in lwIP, but it doesn't supply a common way to setup a PPP connection.
This example introduces a library focusing on sending and parsing AT commands, and also provides useful functions to set up PPP connection.
When PPP connection has been established, the IP packet flow from application side will be transmitted to Internet by cellular module. This example shows how to act as a MQTT client after the PPPoS channel created by using [ESP-MQTT](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/protocols/mqtt.html) APIs.

## How to use example

### Hardware Required

To run this example, you need an ESP32 dev board (e.g. ESP32-WROVER Kit) or ESP32 core board (e.g. ESP32-DevKitC).
For test purpose, you also need a cellular modem module. Here we take the [SIM800L](http://www.simcom.com/product/showproduct.php?lang=en&id=277) and [BG96](https://www.quectel.com/product/bg96.htm) as an example.
You can also try other modules as long as they embedded PPP protocol.

**Note:** Since SIM800L only support **2G** which will **not** work in some countries. And also keep in mind that in some other countries it will stop working soon (many remaining 2G networks will be switched off in the next 2-3 years). So you should **check with your local providers for further details** if you try this example with any 2G modules.

#### Pin Assignment

**Note:** The following pin assignments are used by default which can be changed in menuconfig.

| ESP32  | Cellular Modem |
| ------ | -------------- |
| GPIO25 | RX             |
| GPIO26 | TX             |
| GND    | GND            |
| 5V     | VCC            |

### Configure the project

Open the project configuration menu (`idf.py menuconfig`). Then go into `Example Configuration` menu.

- Choose the modem module in `Choose supported modem device(DCE)` option, currently we only support BG96 and SIM800L.
- Set the access point name in `Set Access Point Name(APN)` option, which should depend on the operator of your SIM card.
- Set the username and password for PPP authentication in `Set username for authentication` and `Set password for authentication` options.
- Select `Send MSG before power off` if you want to send a short message in the end of this example, and also you need to set the phone number correctly in `Peer Phone Number(with area code)` option.
- In `UART Configuration` menu, you need to set the GPIO numbers of UART and task specific parameters such as stack size, priority.

**Note:** During PPP setup, we should specify the way of authentication negotiation. By default it's configured to `PAP`. You can change to others (e.g. `CHAP`) in `Component config-->LWIP-->Enable PPP support` menu.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build and flash the project..

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

The example will get module and operator's information after start up, and then go into PPP mode to start mqtt client operations. This example will also send a short message to someone's phone if you have enabled this feature in menuconfig.

### BG96 Output

```bash
I (1276) pppos_example: Module: BG96
I (1276) pppos_example: Operator: "CHINA MOBILE CMCC"
I (1276) pppos_example: IMEI: 866425030121349
I (1276) pppos_example: IMSI: 460007454185220
I (1476) pppos_example: rssi: 27, ber: 99
I (1676) pppos_example: Battery voltage: 3908 mV
I (1876) pppos_example: Modem PPP Started
I (2656) pppos_example: Modem Connect to PPP Server
I (2656) pppos_example: ~~~~~~~~~~~~~~
I (2656) pppos_example: IP          : 10.65.71.127
I (2656) pppos_example: Netmask     : 255.255.255.255
I (2666) pppos_example: Gateway     : 10.64.64.64
I (2666) pppos_example: Name Server1: 211.136.112.50
I (2676) pppos_example: Name Server2: 211.136.150.66
I (2676) pppos_example: ~~~~~~~~~~~~~~
I (2686) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (2696) pppos_example: MQTT other event id: 7
I (3426) MQTT_CLIENT: Sending MQTT CONNECT message, type: 1, id: 0000
I (3856) pppos_example: MQTT_EVENT_CONNECTED
I (3856) pppos_example: sent subscribe successful, msg_id=20132
I (4226) pppos_example: MQTT_EVENT_SUBSCRIBED, msg_id=20132
I (4226) pppos_example: sent publish successful, msg_id=0
I (4646) pppos_example: MQTT_EVENT_DATA
TOPIC=/topic/esp-pppos
DATA=esp32-pppos
I (4696) pppos_example: Modem PPP Stopped
I (9466) pppos_example: Send send message [Welcome to ESP32!] ok
I (9666) pppos_example: Power down
```

### SIM800L Output
```bash
I (1276) pppos_example: Module: SIMCOM_SIM800L
I (1276) pppos_example: Operator: "CHINA MOBILE"
I (1276) pppos_example: IMEI: 865992039850864
I (1276) pppos_example: IMSI: 460007454185220
I (1476) pppos_example: rssi: 25, ber: 0
I (1676) pppos_example: Battery voltage: 4674 mV
I (1876) pppos_example: Modem PPP Started
I (2806) pppos_example: Modem Connect to PPP Server
I (2806) pppos_example: ~~~~~~~~~~~~~~
I (2806) pppos_example: IP          : 10.188.173.2
I (2806) pppos_example: Netmask     : 255.255.255.255
I (2816) pppos_example: Gateway     : 192.168.254.254
I (2816) pppos_example: Name Server1: 211.136.112.50
I (2826) pppos_example: Name Server2: 211.136.150.66
I (2826) pppos_example: ~~~~~~~~~~~~~~
I (2836) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (2846) pppos_example: MQTT other event id: 7
I (8156) MQTT_CLIENT: Sending MQTT CONNECT message, type: 1, id: 0000
I (8826) pppos_example: MQTT_EVENT_CONNECTED
I (8826) pppos_example: sent subscribe successful, msg_id=26237
I (9526) pppos_example: MQTT_EVENT_SUBSCRIBED, msg_id=26237
I (9526) pppos_example: sent publish successful, msg_id=0
I (10326) pppos_example: MQTT_EVENT_DATA
TOPIC=/topic/esp-pppos
DATA=esp32-pppos
I (10376) pppos_example: Modem PPP Stopped
I (14526) pppos_example: Send send message [Welcome to ESP32!] ok
I (15076) pppos_example: Power down
```

## Troubleshooting
1. Why sending AT commands always failed and this example just keeping rebooting? e.g.

```bash
E (626) sim800: sim800_sync(293): send command failed
E (626) sim800: sim800_init(628): sync failed
```
   * Make sure your modem module is in command mode stably before you run this example.

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
