# ESP-BLE-MESH Examples

[ESP-BLE-MESH]($IDF_PATH/components/bt/esp_ble_mesh/) is the official Bluetooth® Mesh stack of Espressif Systems. We will provide long-term support for new features, performance optimization, etc.

Please help note that breaking changes may be introduced into ESP-BLE-MESH on [minor IDF versions](https://docs.espressif.com/projects/esp-idf/en/latest/versions.html).

Note: To use examples in this directory, you need to have Bluetooth enabled in configuration, and either Bluedroid or NimBLE can be selected as the host stack.

# Example Layout

This directory includes examples to demonstrate ESP-BLE-MESH functionality based on [Zephyr Bluetooth Mesh stack](https://github.com/zephyrproject-rtos/zephyr/tree/master/subsys/bluetooth/mesh).

## ble_mesh_console

This example demonstrates how ESP-BLE-MESH uses Console for message transmitting/receiving tests.

#### ble_mesh_node

This example shows how ESP32 acts as a BLE Mesh Node and sends vendor messages for testing.

See [ble_mesh_node](ble_mesh_console/ble_mesh_node) folder for more details.

#### ble_mesh_provisioner

This example shows how ESP32 acts as a BLE Mesh Provisioner and sends vendor messages for testing.

See [ble_mesh_provisioner](ble_mesh_console/ble_mesh_provisioner) folder for more details.

## ble_mesh_fast_provision

This example illustrates the solution of ESP-BLE-MESH Fast Provisioning.

#### ble_mesh_fast_prov_client

This example shows how ESP32, acting as a BLE Mesh Fast Provisioning Client, provisions other unprovisioned devices and then controls the nodes.

See [ble_mesh_fast_prov_client](ble_mesh_fast_provision/ble_mesh_fast_prov_client) folder for more details.

#### ble_mesh_fast_prov_server

This example illustrates the process that:
1. ESP32 as a BLE Mesh Fast Provisioning Server is provisioned into a node;
2. ESP32 as a Temporary Provisioner provisions other unprovisioned devices.

See [ble_mesh_fast_prov_server](ble_mesh_fast_provision/ble_mesh_fast_prov_server) folder for more details.

## ble_mesh_node

This example demonstrates how ESP32 acts as a BLE Mesh node with Generic OnOff Server model or Generic OnOff Client model on board.

#### onoff_client

This example shows how ESP32 acts as a BLE Mesh Node with Generic OnOff Client model in the Primary Element.

See [onoff_client](ble_mesh_node/onoff_client) folder for more details.

#### onoff_server

This example shows how ESP32 acts as a BLE Mesh Node with only Generic OnOff Server model in the Primary Element.

See [onoff_server](ble_mesh_node/onoff_server) folder for more details.

## ble_mesh_provisioner

This example shows how ESP32 acts as a BLE Mesh Provisioner and provisions other unprovisioned devices.

See [ble_mesh_provisioner](ble_mesh_provisioner) folder for more details.

## ble_mesh_wifi_coexist

This example shows how ESP32 acts as a BLE Mesh Fast Provisioning Server and coexists with Wi-Fi iperf functionality.

See [ble_mesh_wifi_coexist](ble_mesh_wifi_coexist) folder for more details.

# More

See the [README.md](../../README.md) file in the upper level [examples](../../) directory for more information about examples.

