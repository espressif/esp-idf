| Supported Targets | ESP32 |
| ----------------- | ----- |

ESP-IDF BT-L2CAP-SERVER EXAMPLE
===================================

This example is to show how to use the APIs of **Logical Link Control and Adaptation Layer Protocol** (**L2CAP**) to create an L2CAP server. We also provide demo `bt_l2cap_client` to create L2CAP client. In fact, you can create L2CAP clients and L2CAP servers on a single device at the same time.

## How to use example

### Hardware Required

This example is designed to run on commonly available ESP32 development board, e.g. ESP32-DevKitC. To operate the example, it should be connected to the L2CAP client running on another ESP32 board.

### Configure the project

1. Open the project configuration menu:

```
idf.py menuconfig
```

2. Enable the BT-L2CAP functionality by choosing the path as following:

`Component config --> Bluetooth --> Bluedroid Options --> BT-L2CAP`

3. If you want to limit the number of connected devices, please make sure set the `BT/BLE MAX ACL CONNECTIONS` and `BR/EDR ACL Max Connections` with same value you want.

`Component config --> Bluetooth --> Bluedroid Options --> BT/BLE MAX ACL CONNECTIONS(1~7)`
and
`Component config --> Bluetooth --> Bluetooth --> Bluetooth controller --> BR/EDR ACL Max Connections`

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

After the program is started, the example starts inquiry scan and page scan, awaiting being discovered and connected. Other bluetooth devices can discover a device named "ESP_BT_L2CAP_SERVER". ESP-IDF examples for other L2CAP client can be used to connect to the local device.

When you run this example, the explain prints the following at the very begining:
```
I (1398) L2CAP_TAG: ESP_BT_L2CAP_INIT_EVT: status:0

I (1398) L2CAP_TAG: ESP_BT_L2CAP_START_EVT: status:0, hdl:0x0, sec_id:0x37

I (1408) SDP_TAG: ESP_SDP_INIT_EVT: status:0

I (1408) SDP_TAG: ESP_SDP_CREATE_RECORD_COMP_EVT: status:0

I (1418) L2CAP_TAG: event: 10
I (1428) L2CAP_TAG: Own address:[c4:dd:57:5b:e7:46]

```

## Troubleshooting
* This example just demonstrates how to use the SDP and L2CAP APIs. This example cannot establish an l2cap connection with other devices (smartphones, computers, etc.) because the UUID is unknown and does not conform to the Bluetooth protocol standard.
* You can complete OBEX related profiles through SPP, SDP and L2CAP APIs.

## Workflow of an L2CAP Connection
* This [example_workflow](../bt_l2cap_client//tutorial/example_workflow.md) file describes the workflow of an L2CAP connection.