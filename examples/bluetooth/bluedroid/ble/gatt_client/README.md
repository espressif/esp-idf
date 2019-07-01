ESP-IDF Gatt Client Demo
========================

This is the demo for users to use ESP_APIs to create a GATT Client.

To test this demo, you can run the [gatt_server_demo](../gatt_server), which creates services and starts advertising. `Gatt_client_demo` will start scanning and connect to the `gatt_server_demo` automatically.

This demo will enable gatt server's notification function once the connection is established and then the devices start exchanging data.

Please check the [tutorial](tutorial/Gatt_Client_Example_Walkthrough.md) for more information about this example.
