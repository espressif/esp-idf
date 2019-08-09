# ESP-MQTT advanced published test
(See the README.md file in the upper level 'examples' directory for more information about examples.)

Main purpose of this example is to test the MQTT library to correctly publish and receive messages (of different size and sequences) over different transports. 
It is possible to run this example manually without any test to exercise how the MQTT library deals with

- reception of fragmented messages
- runtime updates of URI

## How to use example

This example waits for user input to provide these parameters:
- transport: string parameter, one of: tcp, ssl, ws, wss
- pattern: sample string to be transmitted as message
- pattern repeats: number of repeats of pattern in one MQTT message
- repeated: number of repeats ESP32 publishes the message, also ESP32 expects to receive the same message the same number of repeats
- qos: number specifying qos, one of: 0, 1, 2

### Hardware Required

This example can be executed on any ESP32 board, the only required interface is WiFi and connection to internet.

### Configure the project

* Open the project configuration menu (`idf.py menuconfig`)
* Configure Wi-Fi or Ethernet under "Example Connection Configuration" menu. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](../../README.md) for more details.
* When using Make build system, set `Default serial port` under `Serial flasher config`.
* Set brokers for all 4 transports (TCP, SSL, WS, WSS), also set certificate if needed
* Set topics for publishing from and to ESP32

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (4730) event: sta ip: 192.168.0.125, mask: 255.255.255.0, gw: 192.168.0.2
I (4730) PUBLISH_TEST: [APP] Free memory: 236728 bytes
I (4730) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
D (4740) MQTT_CLIENT: MQTT client_id=ESP32_09885C
I (31360) PUBLISH_TEST: PATTERN:1234 REPEATED:10 PUBLISHED:10
```
- User enters "tcp 1234 10 10 1"
```
EXPECTED STRING 1234123412341234123412341234123412341234, SIZE:40
W (31360) MQTT_CLIENT: Client asked to stop, but was not started
I (31360) PUBLISH_TEST: [TCP transport] Startup..
D (31370) MQTT_CLIENT: Core selection disabled
I (31370) PUBLISH_TEST: Note free memory: 224652 bytes
I (31370) PUBLISH_TEST: Other event id:7
D (31390) MQTT_CLIENT: Transport connected to mqtt://192.168.0.163:1883
I (31400) MQTT_CLIENT: Sending MQTT CONNECT message, type: 1, id: 0000
D (31410) MQTT_CLIENT: Connected
I (31410) PUBLISH_TEST: MQTT_EVENT_CONNECTED
D (31410) MQTT_CLIENT: mqtt_enqueue id: 31184, type=8 successful
D (31410) OUTBOX: ENQUEUE msgid=31184, msg_type=8, len=20, size=20
D (31420) MQTT_CLIENT: Sent subscribe topic=/xxx.topic123, id: 31184, type=8 successful
I (31430) PUBLISH_TEST: sent subscribe successful, msg_id=31184
D (31440) MQTT_CLIENT: mqtt_enqueue id: 16584, type=3 successful
D (31440) OUTBOX: ENQUEUE msgid=16584, msg_type=3, len=59, size=79
I (31450) PUBLISH_TEST: [16584] Publishing...
D (31450) MQTT_CLIENT: msg_type=9, msg_id=31184
D (31460) MQTT_CLIENT: pending_id=16584, pending_msg_count = 2
D (31460) OUTBOX: DELETED msgid=31184, msg_type=8, remain size=59
D (31470) MQTT_CLIENT: Subscribe successful
I (31470) PUBLISH_TEST: MQTT_EVENT_SUBSCRIBED, msg_id=31184
D (31480) MQTT_CLIENT: msg_type=4, msg_id=16584
D (31480) MQTT_CLIENT: pending_id=16584, pending_msg_count = 1
D (31490) OUTBOX: DELETED msgid=16584, msg_type=3, remain size=0
D (31500) MQTT_CLIENT: received MQTT_MSG_TYPE_PUBACK, finish QoS1 publish
I (31500) PUBLISH_TEST: MQTT_EVENT_PUBLISHED, msg_id=16584
D (31510) MQTT_CLIENT: mqtt_enqueue id: 44615, type=3 successful
D (31520) OUTBOX: ENQUEUE msgid=44615, msg_type=3, len=59, size=59
I (31530) PUBLISH_TEST: [44615] Publishing...
...
```
