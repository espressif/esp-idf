| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# ESP-MQTT custom outbox sample application 
(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example is a slightly modified version of the tcp example to show how to configure a custom outbox.
This example connects to the broker URI selected using `idf.py menuconfig` (using mqtt tcp transport) and as a demonstration subscribes/unsubscribes and send a message on certain topic.
(Please note that the public broker is maintained by the community so may not be always available, for details please see this [disclaimer](https://iot.eclipse.org/getting-started/#sandboxes))

Note: If the URI equals `FROM_STDIN` then the broker address is read from stdin upon application startup (used for testing)

It uses ESP-MQTT library which implements mqtt client to connect to mqtt broker.

## Necessary changes to customize the outbox

To customize the outbox the first step is to enable it in the menuconfig option. 

With this option enabled, the default implementation isn't defined and the function definition needs to be added to mqtt component. 
Any extra dependencies needed by the new sources also need to be added to the mqtt component. Refer to the example CMakeLists.txt file
for the details on how to do it. 

## The custom outbox in the example

For the sake of this example the customized outbox implements the same functionalits of the regular but using C++ as a language. 

The implementation uses [C++ Polymorphic memory resources]() to control memory allocations and limit the usage of the memory. 

## How to use example

### Hardware Required

This example can be executed on any ESP32 board, the only required interface is WiFi and connection to internet.

### Configure the project

* Open the project configuration menu (`idf.py menuconfig`)
* Configure Wi-Fi or Ethernet under "Example Connection Configuration" menu. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](../../README.md) for more details.

Note that the mandatory configurations for this example, mqtt custom outbox and C++ exceptions are automatically added by the `sdkconfig.defaults` file.
### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (4635) example_common: Connected to example_netif_sta
I (4645) example_common: - IPv4 address: 192.168.33.206,
I (4645) example_common: - IPv6 address: fe80:0000:0000:0000:7e9e:bdff:fecf:00c0, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (4655) Monotonic: Monotonic: 400 bytes allocated, 400 total bytes in use
I (4665) Monotonic: Monotonic: 1000 bytes allocated, 1400 total bytes in use
I (4675) Monotonic: Monotonic: 128 bytes allocated, 1528 total bytes in use
I (4685) Pool: Pool: 32 bytes allocated, 32 total bytes in use
I (4685) Monotonic: Monotonic: 7688 bytes allocated, 9216 total bytes in use
I (4695) Monotonic: Monotonic: 128 bytes allocated, 9344 total bytes in use
I (4705) Pool: Pool: 480 bytes allocated, 512 total bytes in use
I (4715) Monotonic: Monotonic: 992 bytes allocated, 10336 total bytes in use
I (4715) Monotonic: Monotonic: 128 bytes allocated, 10464 total bytes in use
I (4725) Pool: Pool: 23 bytes allocated, 535 total bytes in use
I (4735) MQTT_EXAMPLE: Enqueued msg_id=14345
I (4735) Pool: Pool: 29 bytes allocated, 564 total bytes in use
I (4745) MQTT_EXAMPLE: Enqueued msg_id=3507
I (4745) MQTT_EXAMPLE: Other event id:7
I (4755) main_task: Returned from app_main()
I (5085) MQTT_EXAMPLE: MQTT_EVENT_CONNECTED
I (5085) Pool: Pool: 23 bytes allocated, 587 total bytes in use
I (5085) MQTT_EXAMPLE: sent publish successful, msg_id=47425
I (5085) Pool: Pool: 18 bytes allocated, 605 total bytes in use
I (5095) MQTT_EXAMPLE: sent subscribe successful, msg_id=60709
I (5105) Pool: Pool: 18 bytes allocated, 623 total bytes in use
I (5105) MQTT_EXAMPLE: sent subscribe successful, msg_id=33273
I (5395) Pool: Pool: 23 bytes deallocated, 623 total bytes in use
I (5395) MQTT_EXAMPLE: MQTT_EVENT_PUBLISHED, msg_id=47425
I (6005) Pool: Pool: 18 bytes deallocated, 623 total bytes in use
I (6005) MQTT_EXAMPLE: MQTT_EVENT_SUBSCRIBED, msg_id=60709
I (6005) MQTT_EXAMPLE: sent publish successful, msg_id=0
I (6015) Pool: Pool: 18 bytes deallocated, 623 total bytes in use
I (6015) MQTT_EXAMPLE: MQTT_EVENT_SUBSCRIBED, msg_id=33273
I (6025) MQTT_EXAMPLE: sent publish successful, msg_id=0
I (6035) MQTT_EXAMPLE: MQTT_EVENT_DATA
TOPIC=/topic/qos1
DATA=data_3
I (6315) MQTT_EXAMPLE: MQTT_EVENT_DATA
TOPIC=/topic/qos1
DATA=data_3
I (6315) Pool: Pool: 23 bytes deallocated, 623 total bytes in use
I (6315) MQTT_EXAMPLE: MQTT_EVENT_PUBLISHED, msg_id=14345
I (6615) MQTT_EXAMPLE: MQTT_EVENT_DATA
TOPIC=/topic/qos0
DATA=data
```
