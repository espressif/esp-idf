ESP-IDF Gatt Server Demo
========================

This is the demo of APIs to create a GATT service by adding attributes one by one. However, this method is defined by Bluedroid and is difficult for users to use.

Hence, we also allow users to create a GATT service with an attribute table, which releases the user from adding attributes one by one. And it is recommended for users to use. For more information about this method, please refer to [gatt_server_service_table_demo](../gatt_server_service_table).

This demo creates GATT a service and then starts advertising, waiting to be connected to a GATT client. 

To test this demo, we can run the [gatt_client_demo](../gatt_client), which can scan for and connect to this demo automatically. They will start exchanging data once the GATT client has enabled the notification function of the GATT server.

Please check the [tutorial](tutorial/Gatt_Server_Example_Walkthrough.md) for more information about this example.

