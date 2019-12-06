# ESP-MQTT SSL example with PSK verification 

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example connects to a local broker configured to PSK authentication

## How to use example

### Hardware Required

This example can be executed on any ESP32 board, the only required interface is WiFi (or ethernet) to connect to a MQTT 
broker with preconfigured PSK verification method.

#### Mosquitto settings
In case of using mosquitto broker, here is how to enable PSK authentication in `mosquitto.config`, 
```
psk_hint hint
psk_file path_to_your_psk_file
allow_anonymous true
```
Note: Last line enables anonymous mode, as this example does not use mqtt username and password.

PSK file then has to contain pairs of hints and keys, as shown below:
```
hint:BAD123
```

Important note: Keys are stored as text hexadecimal values in PSK file, while the example code stores key as plain binary
as required by MQTT API. (See the example source for details: `"BAD123" -> 0xBA, 0xD1, 0x23`)

### Configure the project

* Run `make menuconfig` (or `idf.py menuconfig` if using CMake build system)
* Configure Wi-Fi or Ethernet under "Example Connection Configuration" menu. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](../../README.md) for more details.
* When using Make build system, set `Default serial port` under `Serial flasher config`.

### Build and Flash


(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (2160) example_connect: Ethernet Link Up
I (4650) example_connect: Connected to Ethernet
I (4650) example_connect: IPv4 address: 192.168.0.1
I (4650) MQTTS_EXAMPLE: [APP] Free memory: 244792 bytes
I (4660) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
D (4670) MQTT_CLIENT: MQTT client_id=ESP32_c6B4F8
D (4680) MQTT_CLIENT: Core selection disabled
I (4680) MQTTS_EXAMPLE: Other event id:7
D (4680) esp-tls: host:192.168.0.2: strlen 13
D (4700) esp-tls: ssl psk authentication
D (4700) esp-tls: handshake in progress...
D (4720) MQTT_CLIENT: Transport connected to mqtts://192.168.0.2:8883
I (4720) MQTT_CLIENT: Sending MQTT CONNECT message, type: 1, id: 0000
D (4720) MQTT_CLIENT: mqtt_message_receive: first byte: 0x20
D (4730) MQTT_CLIENT: mqtt_message_receive: read "remaining length" byte: 0x2
D (4730) MQTT_CLIENT: mqtt_message_receive: total message length: 4 (already read: 2)
D (4740) MQTT_CLIENT: mqtt_message_receive: read_len=2
D (4750) MQTT_CLIENT: mqtt_message_receive: transport_read():4 4
D (4750) MQTT_CLIENT: Connected
I (4760) MQTTS_EXAMPLE: MQTT_EVENT_CONNECTED
D (4760) MQTT_CLIENT: mqtt_enqueue id: 4837, type=8 successful
D (4770) OUTBOX: ENQUEUE msgid=4837, msg_type=8, len=18, size=18
D (4770) MQTT_CLIENT: Sent subscribe topic=/topic/qos0, id: 4837, type=8 successful
I (4780) MQTTS_EXAMPLE: sent subscribe successful, msg_id=4837
D (4790) MQTT_CLIENT: mqtt_enqueue id: 58982, type=8 successful
D (4790) OUTBOX: ENQUEUE msgid=58982, msg_type=8, len=18, size=36
D (4800) MQTT_CLIENT: Sent subscribe topic=/topic/qos1, id: 58982, type=8 successful
I (4810) MQTTS_EXAMPLE: sent subscribe successful, msg_id=58982
```

