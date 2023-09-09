| Supported Targets | ESP32 | ESP32-C3 | ESP32-C6 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- |

ESP-BLE-MESH and Wi-Fi Coexistence Example
=============================================

This example is used to measure the maximum throughput value that Wi-Fi can achieve when ESP-BLE-MESH is provisioning, sending and receiving messages. Users can implement their own application scenarios based on the throughput value in this example. If users are not familiar with how to use ESP-BLE-MESH, please refer to [ESP-BLE-MESH Node Example](../onoff_models/onoff_server/README.md).

This example is divided into two main parts:

- Wi-Fi Iperf protocol
- ESP-BLE-MESH Fast Provisioning Server

For information on how to use the Iperf protocol for Wi-Fi, please refer to [Wi-Fi Iperf README](../../../wifi/iperf/README.md)

When Wi-Fi Iperf is enabled, ESP-BLE-MESH can use fast provision for device provisioning. The node in this example contains one element that supports the following models:

- Configuration Server model: This model is mainly for node configuration after the device joins the ESP-BLE-MESH network, such as relay, friend feature, NetKey, etc.
- Configuration Client model: This model is used to get or set the configuration of other nodes, such as relay, friend feature, adding NetKey, etc.
- Generic On/Off Server model: This model is mainly used to indicate the value of node's On/Off state, and different state values ​​determine the different states of the device (such as light on and off).
- Generic On/Off Client model: This model controls the on/off of the device by setting the On/Off state of other nodes.
- Fast Provision Server model: This model is a vendor server model implemented for fast provisioning. When the device is configured to join the network, it will temporarily act as a Provisioner for other devices; when the provisioning is completed, it will resume the node role and pass the addresses assigned to all nodes to the Fast Provision Client model through this model.
- Fast Provision Client model: This model needs to be used together with the Fast Provision Server model to get all addresses assigned to the nodes by the Fast Provisioning Server.