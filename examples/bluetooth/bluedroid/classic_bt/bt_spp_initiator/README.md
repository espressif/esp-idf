| Supported Targets | ESP32 |
| ----------------- | ----- |

# ESP-IDF BT-SPP-INITATOR demo

This is the demo for user to use ESP_APIs to create a **Serial Port Protocol** (**SPP**) initiator and we aggregate **Secure Simple Pair** (**SSP**) into this demo to show how to use SPP when creating your own APPs.

## How to use example

### Hardware Required

This example is designed to run on commonly available ESP32 development board, e.g. ESP32-DevKitC. To operate it should be connected to an SPP Acceptor running on another device.

### Configure the project

```
idf.py menuconfig
```

In `menuconfig` path: `Coponent config --> Bluetooth--> Bluedroid Options --> SPP` and `Coponent config --> Bluetooth--> Bluedroid Options --> Secure Simple Pair`.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Troubleshouting

- Set `SPP_SHOW_MODE` as `SPP_SHOW_DATA` or `SPP_SHOW_SPEED` in code (should be same with bt_spp_acceptor).

- After the program started, It will connect to bt_spp_acceptor and send data.

- We haven't do the same update to `bt_acceptor_demo` for the sake of reducing the size of ESP_IDF, but transplanting of input module is supported.

- We also show the Security Simple Pair in this SPP demo. Users can set the IO Capability and Security Mode for their device (security level is fixed level 4). The default security mode of this demo is `ESP_SPP_SEC_AUTHENTICATE` which support MITM (Man In The Middle) protection. For more information about Security Simple Pair on ESP32, please refer to [ESP32_SSP](../bt_spp_acceptor/ESP32_SSP.md).

## FAQ
Q: How can we reach the maximum throughput when using SPP?
A: The default MTU size of classic Bluetooth SPP on ESP32 is 990 bytes, and higher throughput can be achieved in the case that data chunck size is close to the MTU size or multiple of MTU size. For example, sending 100 bytes data per second is much better than sending 10 bytes every 100 milliseconds.

Q: What is the difference between the event `ESP_SPP_CONG_EVT` and the parameter `cong` of the event `ESP_SPP_WRITE_EVT`?
A: The event `ESP_SPP_CONG_EVT` shows the changing status from `congest` to `uncongest`, or form `uncongest` to `congest`. Congestion can have many causes, such as using out of the credit which is sent by peer, reaching the high watermark of the Tx buffer, the congestion at Bluetooth L2CAP layer and so on. The parameter `cong` of the event `ESP_SPP_WRITE_EVT` shows a snapshot of the state of the flow control manager after the write operation is completed. The user needs to carefully consider retransmitting or continuing to write according to these two events. The ESP32 offers an VFS mode of SPP which hides the details of retransmitting, but it will block the caller and is not more efficient than the callback mode.

Q: How many SPP clients does ESP32 support?
A: The ESP32 supports maximum 8 SPP clients, which including virtual SPP connections. Virtual SPP connection means that SPP clients can connect to the different SPP servers running on the same peer device. However the number of SPP clients (excluding virtual connections) shall not exceed the number of Bluetooth ACL connections.
