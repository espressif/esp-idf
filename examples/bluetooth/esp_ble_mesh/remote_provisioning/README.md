| Supported Targets | ESP32 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- |
# Remote Provisioning

(See the README.md file in the upper level 'examples' directory for more information about examples.)

The example is primarily used to demonstrate the new Remote Provisioning feature added in Mesh Protocol v1.1.
## How to use example

Please refer to this [tutorial](tutorial/BLE_Mesh_Remote_Provisioning_Example_Walkthrough.md) for detailed instructions on how to run it.

### Hardware Required
You need prepare at least three ESP series development boards. We recommend using the [ESP32-C3-DevKitM-1](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/hw-reference/esp32c3/user-guide-devkitm-1.html) or [ESP32-C6-DevKitC-1](https://docs.espressif.com/projects/espressif-esp-dev-kits/en/latest/esp32c6/esp32-c6-devkitc-1/user_guide.html#). Alternatively, you can use other development boards, but ensure that each board has at least one LED and one button for operation.
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
When a device that runs the `rpr_server` joins the network, the device that runs the `rpr_client` will have the following log output:

```
The Remote Provisioning Server have been provisioned, You could click button to start remote provisioning
```
After seeing this output log, you can press the button on the device running `rpr_client` to initiate the Remote Provisioning process.

After successfully joining the network through Remote Provisioning, the device running `rpr_client` will have the following log output: 
```
Remote Prov Client Prov Complete
Net Idx: `net index`
Node addr: `node address`
Node element num: `element num`
Node UUID: `UUID`
```
(The content wrapped with ` ` will vary depending on the device you are actually using.)

Afterwards, you can press the button on the device running `rpr_client` to toggle the LED settings of other devices in the network.

## Troubleshooting

See common troubleshooting for Directed Forwarding examples from [ESP-BLE-MESH FAQ](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/esp-ble-mesh/ble-mesh-index.html#esp-ble-mesh-faq).

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)