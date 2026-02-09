| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- | -------- | -------- |

# ESP-MQTT SSL Sample application

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example shows how to use the external ESP-MQTT client (managed component `espressif/mqtt`) from an ESP-IDF project. It connects to an MQTT broker over TLS and demonstrates basic operations: connect, subscribe, publish, receive data, and unsubscribe. You can switch between MQTT 3.1.1 and MQTT 5.0 via menuconfig.

This example connects to the broker test.mosquitto.org (Please note that the public broker is maintained by the community so may not be always available)

Two TLS validation configurations are available:
- Certificate bundle (default): Uses ESP‑IDF's certificate bundle and targets `test.mosquitto.org:8886` (Let's Encrypt chain).
- Embedded Mosquitto CA: Pins `mosquitto.org.crt` to validate `test.mosquitto.org:8883` (mirrors the legacy SSL example).

## Features

- Uses the managed component `espressif/mqtt` (fetched via IDF Component Manager)
- TLS connection with server verification using the certificate bundle
- Basic operations: subscribe, publish (QoS0, QoS1), receive data, unsubscribe

- Important: Changing the certificate validation method does not automatically update the Broker URI in menuconfig. Make sure the port matches your selection:
  - Bundle: use `mqtts://test.mosquitto.org:8886`
  - Mosquitto CA: use `mqtts://test.mosquitto.org:8883`
- Certificate bundle mode uses `esp_crt_bundle_attach` and targets port `8886`.
- Mosquitto CA mode embeds and pins `mosquitto.org.crt` and targets port `8883`.
- The managed component dependency is declared in `main/idf_component.yml` and will be downloaded automatically.

### Hardware Required

This example can be executed on any ESP32 board, the only required interface is WiFi and connection to internet.

### Configure the project

* Open the project configuration menu (`idf.py menuconfig`)
* Configure Wi-Fi or Ethernet under "Example Connection Configuration" menu. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](../../README.md) for more details.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (3714) event: sta ip: 192.168.0.139, mask: 255.255.255.0, gw: 192.168.0.2
I (3714) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (3964) MQTT_CLIENT: Sending MQTT CONNECT message, type: 1, id: 0000
I (4164) MQTTS_EXAMPLE: MQTT_EVENT_CONNECTED
I (4174) MQTTS_EXAMPLE: sent publish successful, msg_id=41464
I (4174) MQTTS_EXAMPLE: sent subscribe successful, msg_id=17886
I (4174) MQTTS_EXAMPLE: sent subscribe successful, msg_id=42970
I (4184) MQTTS_EXAMPLE: sent unsubscribe successful, msg_id=50241
I (4314) MQTTS_EXAMPLE: MQTT_EVENT_PUBLISHED, msg_id=41464
I (4484) MQTTS_EXAMPLE: MQTT_EVENT_SUBSCRIBED, msg_id=17886
I (4484) MQTTS_EXAMPLE: sent publish successful, msg_id=0
I (4684) MQTTS_EXAMPLE: MQTT_EVENT_SUBSCRIBED, msg_id=42970
I (4684) MQTTS_EXAMPLE: sent publish successful, msg_id=0
I (4884) MQTT_CLIENT: deliver_publish, message_length_read=19, message_length=19
I (4884) MQTTS_EXAMPLE: MQTT_EVENT_DATA
TOPIC=/topic/qos0
DATA=data
I (5194) MQTT_CLIENT: deliver_publish, message_length_read=19, message_length=19
I (5194) MQTTS_EXAMPLE: MQTT_EVENT_DATA
TOPIC=/topic/qos0
DATA=data
```

