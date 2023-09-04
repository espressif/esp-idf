| Supported Targets | ESP32 |
| ----------------- | ----- |

## ESP-IDF BT-SPP-ACCEPTOR demo

This example is to show how to use the APIs of **Serial Port Protocol** (**SPP**) to create an SPP acceptor which performs as a server. We aggregate **Secure Simple Pair** (**SSP**) into this demo to show how to use SPP when creating your own APPs. We also provide the demo `bt_spp_initiator` or the demo `bt_spp_vfs_initiator` to create an SPP initiator which performs as a client. In fact, you can create SPP acceptors and SPP initiators on a single device at the same time.

## How to use example

### Hardware Required

This example is designed to run on commonly available ESP32 development board, e.g. ESP32-DevKitC. To operate the example, it should be connected to an SPP Initiator running on a smartphone, a computer or on another ESP32 development board.

### Configure the project

1. Open the project configuration menu:

```
idf.py menuconfig
```

2. Enable the SPP functionality by choosing the path as following:

`Component config --> Bluetooth --> Bluedroid Options --> SPP`

3. If you want to limit the number of connected devices, please make sure set the `BT/BLE MAX ACL CONNECTIONS` and `BR/EDR ACL Max Connections` with same value you want.

`Component config --> Bluetooth --> Bluedroid Options --> BT/BLE MAX ACL CONNECTIONS(1~7)`
and
`Component config --> Bluetooth --> Bluetooth --> Bluetooth controller --> BR/EDR ACL Max Connections`


4. SSP is enabled as default in this example. If you prefer the legacy pairing, you shall disable it in the following path.

`SPP Example Configuration --> Secure Simple Pair`.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Description

After the program starts, the example will start an SPP acceptor. The example will calculate the data rate or just print the received data after the SPP connection is established. You can connect to the server and send data with another ESP32 development board, Andriod phone or computer which performs as the SPP initiator.

## Trouble shooting

- To see the information of data, users shall set `SPP_SHOW_MODE` as `SPP_SHOW_DATA` or `SPP_SHOW_SPEED` in code(should be same with `bt_spp_initiator`, if the peer device runs it). When setting `SPP_SHOW_MODE` as `SPP_SHOW_DATA`, if the data rate is too high or the data length is too long, it is strongly recommended to process them in other lower priority application task rather than in this callback directly. Since the printing takes too much time, and it may stuck the Bluetooth stack.

## FAQ
Q: How to change the process of SSP?
A: Users can set the IO Capability and Security Mask for their device (fixed Security Mode, Security Mode 4). In short, the Security Mask sets the security level for authentication stage and the IO Capability determines the way of user interaction during pairing. The default Security Mask of this demo is `ESP_SPP_SEC_AUTHENTICATE` which support MITM (Man In The Middle) protection. For more information about Security Simple Pair on ESP32, please refer to [ESP32_SSP](./ESP32_SSP.md).


Q: How many SPP servers does ESP32 support?
A: For now, the maximum number of SPP servers is 6, which is limited by the maximum number of SDP records. When the SPP server is successfully started, the unique SCN (Server Channel Number) will be mapped to the SPP server.

Q: Is SPP absolutely reliable?
A: For now, most Bluetooth stacks implement the SPP based on the L2CAP Basic Mode, and the reliability only depends on the controller. If errors(e.g. CRC) are missed by controller, the L2CAP packet will not be retransmitted, so it is recommended that you should implement the retransmission at the application layer. For more information about L2CAP operation modes, please refer to Bluetooth Core Specification, Version 4.2 or later, Volume 3, Part A: Logical Link Control and Adaptation Protocol Specification.