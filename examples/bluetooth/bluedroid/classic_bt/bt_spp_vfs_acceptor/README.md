| Supported Targets | ESP32 |
| ----------------- | ----- |

## ESP-IDF BT-SPP-VFS-ACCEPTOR demo

This example is to show how to use the APIs of **Serial Port Protocol** (**SPP**) to create an SPP acceptor which performs as a server, and it will register into the VFS. We aggregate **Secure Simple Pair** (**SSP**) into this demo to show how to use SPP when creating your own APPs. We also provide the demo `bt_spp_initiator` or the demo `bt_spp_vfs_initiator` to create an SPP initiator which performs as a client. In fact, you can create SPP acceptors and SPP initiators on a single device at the same time.

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

After the program starts, the example will start an SPP acceptor. The example will print the received data after the SPP connection is established. You can connect to the server and send data with another ESP32 development board, Andriod phone or computer which performs as the SPP initiator.

## FAQ
Please refer the FAQ part in the [README.md](../bt_spp_acceptor/README.md)