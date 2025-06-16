| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

ESP-IDF Gattc and Gatts Coexistence example
==============================================

This example demonstrates the coexistence of gattc and gatts.

This example creates a GATT service and starts ADV. The ADV name is `ESP_GATTS_DEMO`, then waits to be connected. At the same time, a gatt client is created, the ADV name is `ESP_GATTS_DEMO`, the device is connected, and the data is exchanged. If the device is not found within 120 seconds, the example will stop scanning.

ESP-IDF also allows users to create a GATT service via an attribute table, rather than add attributes one by one. And it is recommended for users to use. For more information about this method, please refer to [gatt_server_service_table_demo](../../ble/gatt_server_service_table).

To test this example, you can run the [gatt_client_demo](../../ble/gatt_client), which can scan for and connect to this example automatically, and run [gatt_server_demo](../../ble/gatt_server), Waiting to be connected. They will start exchanging data once the GATT client has enabled the notification function of the GATT server.

Please check the [tutorial](../../ble/gatt_server/tutorial/Gatt_Server_Example_Walkthrough.md) for more information about the gatts part of this example.
Please check the [tutorial](../../ble/gatt_client/tutorial/Gatt_Client_Example_Walkthrough.md) for more information about the gattc part of this example.

