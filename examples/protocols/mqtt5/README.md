| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# ESP-MQTT sample application
(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example connects to the broker URI selected using `idf.py menuconfig` (using mqtt tcp transport) and as a demonstration subscribes/unsubscribes and send a message on certain topic.
(Please note that the public broker is maintained by the community so may not be always available, for details please see this [disclaimer](https://iot.eclipse.org/getting-started/#sandboxes))

Note: If the URI equals `FROM_STDIN` then the broker address is read from stdin upon application startup (used for testing)

It uses ESP-MQTT library which implements mqtt client to connect to mqtt broker with MQTT version 5.

The more details about MQTT v5, please refer to [official website](https://docs.oasis-open.org/mqtt/mqtt/v5.0/os/mqtt-v5.0-os.html)

## How to use example

### Hardware Required

This example can be executed on any ESP32 board, the only required interface is WiFi and connection to internet.

### Configure the project

* Open the project configuration menu (`idf.py menuconfig`)
* Configure Wi-Fi or Ethernet under "Example Connection Configuration" menu. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](../../README.md) for more details.
* MQTT v5 protocol (`CONFIG_MQTT_PROTOCOL_5`) under "ESP-MQTT Configurations" menu is enabled by `sdkconfig.defaults`.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (5119) esp_netif_handlers: example_connect: sta ip: 192.168.3.143, mask: 255.255.255.0, gw: 192.168.3.1
I (5119) example_connect: Got IPv4 event: Interface "example_connect: sta" address: 192.168.3.143
I (5619) example_connect: Got IPv6 event: Interface "example_connect: sta" address: fe80:0000:0000:0000:c64f:33ff:fe24:6645, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (5619) example_connect: Connected to example_connect: sta
I (5629) example_connect: - IPv4 address: 192.168.3.143
I (5629) example_connect: - IPv6 address: fe80:0000:0000:0000:c64f:33ff:fe24:6645, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (5649) MQTT5_EXAMPLE: Other event id:7
W (6299) wifi:<ba-add>idx:0 (ifx:0, 34:29:12:43:c5:40), tid:7, ssn:0, winSize:64
I (7439) MQTT5_EXAMPLE: MQTT_EVENT_CONNECTED
I (7439) MQTT5_EXAMPLE: sent publish successful, msg_id=53118
I (7439) MQTT5_EXAMPLE: sent subscribe successful, msg_id=41391
I (7439) MQTT5_EXAMPLE: sent subscribe successful, msg_id=13695
I (7449) MQTT5_EXAMPLE: sent unsubscribe successful, msg_id=55594
I (7649) mqtt5_client: MQTT_MSG_TYPE_PUBACK return code is -1
I (7649) MQTT5_EXAMPLE: MQTT_EVENT_PUBLISHED, msg_id=53118
I (8039) mqtt5_client: MQTT_MSG_TYPE_SUBACK return code is 0
I (8049) MQTT5_EXAMPLE: MQTT_EVENT_SUBSCRIBED, msg_id=41391
I (8049) MQTT5_EXAMPLE: sent publish successful, msg_id=0
I (8059) mqtt5_client: MQTT_MSG_TYPE_SUBACK return code is 2
I (8059) MQTT5_EXAMPLE: MQTT_EVENT_SUBSCRIBED, msg_id=13695
I (8069) MQTT5_EXAMPLE: sent publish successful, msg_id=0
I (8079) MQTT5_EXAMPLE: MQTT_EVENT_DATA
I (8079) MQTT5_EXAMPLE: key is board, value is esp32
I (8079) MQTT5_EXAMPLE: key is u, value is user
I (8089) MQTT5_EXAMPLE: key is p, value is password
I (8089) MQTT5_EXAMPLE: payload_format_indicator is 1
I (8099) MQTT5_EXAMPLE: response_topic is /topic/test/response
I (8109) MQTT5_EXAMPLE: correlation_data is 123456
I (8109) MQTT5_EXAMPLE: content_type is 
I (8119) MQTT5_EXAMPLE: TOPIC=/topic/qos1
I (8119) MQTT5_EXAMPLE: DATA=data_3
I (8129) mqtt5_client: MQTT_MSG_TYPE_UNSUBACK return code is 0
I (8129) MQTT5_EXAMPLE: MQTT_EVENT_UNSUBSCRIBED, msg_id=55594
I (8139) mqtt_client: Client asked to disconnect
I (9159) MQTT5_EXAMPLE: MQTT_EVENT_DISCONNECTED
```
