ESP-IDF Gatt Client Multi Connection Demo
========================

This is the demo of APIs to create a GATT multi-connection client. It can be used to connect to three GATT servers at the same time. 

To test this demo, please run [gatt_server_demo](../gatt_server) to create three GATT server devices, namely ESP_GATTS_DEMO_a, ESP_GATTS_DEMO_b and ESP_GATTS_DEMO_c, `Gatt_client_multi_connection_demo` will connect to these three gatt server demos, and then exchange data.

The code can be modified to connect to more devices (up to 4 devices by default). If you need to connect to more devices (more than 4 devices), you need to change `BT/BLE MAX ACL CONNECTIONS` in menuconfig.

Please check the [tutorial](tutorial/Gatt_Client_Multi_Connection_Example_Walkthrough.md) for more information about this example.
