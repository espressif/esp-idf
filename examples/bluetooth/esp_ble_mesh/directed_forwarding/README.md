| Supported Targets | ESP32 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | --------- | -------- | -------- |

# Directed Forwarding

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example is primarily intended to demonstrate the newly added directed forwarding feature in Mesh Protocol v1.1.

## How to use example

Please refer to this [tutorial](tutorial/BLE_Mesh_Directed_Forwarding_Example_Walkthrough.md) for detailed instructions on how to run it.


### Hardware Required
You need prepare at least two ESP series development boards. We recommend using the [ESP32-C3-DevKitM-1](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/hw-reference/esp32c3/user-guide-devkitm-1.html) or [ESP32-C6-DevKitC-1](https://docs.espressif.com/projects/espressif-esp-dev-kits/en/latest/esp32c6/esp32-c6-devkitc-1/user_guide.html#). Alternatively, you can use other development boards, but ensure that each board has at least one LED and one button for operation.
### Configure the project

```
idf.py menuconfig
```

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output
After all devices have joined the network, you need to press the button on the provisioner(the device which runs df_client) to complete the final setup. Once the setup is complete, provisioner will provide the following output:

```
You could click to send message by directed forwarding
After the message handled, you could click again to send message by flooding
```
After seeing the above output, you can press the button again to send a network message. When the message is sent in directed forwarding, the following output will be displayed: 
```
Will send a message to node(`node unicast`) with DF
```
When the message is sent in a flooding, the following output will be displayed:
```
Will send a message to node(`node unicast`) with Flooding
```
## Troubleshooting

See common troubleshooting for Directed Forwarding examples from [ESP-BLE-MESH FAQ](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/esp-ble-mesh/ble-mesh-index.html#esp-ble-mesh-faq).

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
