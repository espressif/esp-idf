# ESP BLE Mesh Examples

[ESP BLE Mesh]($IDF_PATH/components/bt/esp_ble_mesh/) is the official BLE Mesh stack of Espressif Systems. We will provide long-term support for new features, performance optimization, etc.

Please help note that breaking changes may be introduced into ESP BLE Mesh on [minor IDF versions](https://docs.espressif.com/projects/esp-idf/en/latest/versions.html).

Note: To use examples in this directory, you need to have Bluetooth enabled in configuration and Bluedroid selected as the host stack. ESP BLE Mesh will support NimBLE host soon, and currently the integration of NimBLE host with ESP BLE Mesh stack is under development.

# Example Layout

This directory includes examples to demonstrate BLE Mesh functionality based on Zephyr (https://github.com/zephyrproject-rtos/zephyr) Mesh stack.

## ble_mesh_client_model

This example shows how ESP32 acts as a BLE Mesh Node with Generic OnOff Client & Server Models in the Primary Element.

See the [README.md](./ble_mesh_client_model/README.md) file in the example [ble_mesh_client_model](./ble_mesh_client_model/).

## ble_mesh_console

This example demonstrates how ESP BLE Mesh uses Console for message transmitting/receiving tests.

### ble_mesh_node

This example shows how ESP32 acts as a BLE Mesh Node and sends vendor messages for testing.

See the [README.md](./ble_mesh_console/ble_mesh_node/README.md) file in the example [ble_mesh_node](./ble_mesh_console/ble_mesh_node/).

### ble_mesh_provisioner

This example shows how ESP32 acts as a BLE Mesh Provisioner and sends vendor messages for testing.

See the [README.md](./ble_mesh_console/ble_mesh_provisioner/README.md) file in the example [ble_mesh_provisioner](./ble_mesh_console/ble_mesh_provisioner/).

## ble_mesh_fast_provision

This example illustrates the solution of ESP BLE Mesh Fast Provisioning.

### ble_mesh_fast_prov_client

This example shows how ESP32, acting as a BLE Mesh Fast Provisioning Client, provisions other unprovisioned devices and then control the nodes.

See the [README.md](./ble_mesh_fast_provision/ble_mesh_fast_prov_client/README.md) file in the example [ble_mesh_fast_prov_client](./ble_mesh_fast_provision/ble_mesh_fast_prov_client/).

### ble_mesh_fast_prov_server

This example illustrates the process that:
1. Firstly as a BLE Mesh Fast Provisioning Server, ESP32 is provisioned into a Node;
2. and then provisions other unprovisioned devices as a Temporary Provisioner.

See the [README.md](./ble_mesh_fast_provision/ble_mesh_fast_prov_server/README.md) file in the example [ble_mesh_fast_prov_server](./ble_mesh_fast_provision/ble_mesh_fast_prov_server/).

## ble_mesh_node

This example shows how ESP32 acts as a BLE Mesh Node with only Generic OnOff Server Model in the Primary Element.

See the [README.md](./ble_mesh_node/README.md) file in the example [ble_mesh_node](./ble_mesh_node/).

## ble_mesh_provisioner

This example shows how ESP32 acts as a BLE Mesh Provisioner and provisions other unprovisioned devices.

See the [README.md](./ble_mesh_provisioner/README.md) file in the example [ble_mesh_provisioner](./ble_mesh_provisioner/).

## ble_mesh_wifi_coexist

This example shows how ESP32 acts as a BLE Mesh Fast Provisioning Server and coexists with Wi-Fi iperf functionality.

See the [README.md](./ble_mesh_wifi_coexist/README.md) file in the example [ble_mesh_wifi_coexist](./ble_mesh_wifi_coexist/).

# More

See the [README.md](../../README.md) file in the upper level [examples](../../) directory for more information about examples.

