| Supported Targets | ESP32 |
| ----------------- | ----- |

# ESP-IDF BT-SPP-INITATOR demo

This example is to show how to use the APIs of **Serial Port Protocol** (**SPP**) to create an SPP initiator which performs as a client, and it will register into the VFS. we aggregate **Secure Simple Pair** (**SSP**) into this demo to show how to use SPP when creating your own APPs. We also provide the demo `bt_spp_acceptor` or the demo `bt_spp_vfs_acceptor` to create an SPP acceptor which performs as a server. In fact, you can create SPP acceptors and SPP initiators on a single device at the same time.

## How to use example

### Hardware Required

This example is designed to run on commonly available ESP32 development board, e.g. ESP32-DevKitC. To operate the example, you should be connect to an SPP acceptor running on a smartphone, a computer or on another ESP32 development board.

### Configure the project
1. Open the project configuration menu:
```
idf.py menuconfig
```

2. Enable the SPP functionality by choosing the path as following:

`Component config --> Bluetooth --> Bluedroid Options --> SPP`.

3. SSP is enabled as default in this example. If you prefer the legacy pairing, you shall disable it in the following path.

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

After the program starts, the example will initiate a Bluetooth discovery procedure and filter out the peer device by the name in the EIR(Extended Inquiry Response). After discovering the SPP service, it will connect to the SPP acceptor and send data.

## FAQ
Please refer the FAQ part in the [README.md](../bt_spp_initiator/README.md)
