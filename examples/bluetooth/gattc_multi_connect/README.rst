ESP-IDF GATT MULTIPLE CLIENT demo
========================

This is the demo for user to use ESP_APIs to create a GATT Multi-connection Client.
The gattc_multi_connect demo can connect three ble slaves at the same time.
Modify the name of gatt_server demo named ESP_GATTS_DEMO_a, ESP_GATTS_DEMO_b and ESP_GATTS_DEMO_c, then run the three demos,
the gattc_multi_connect demo will connect the three gatt_server demos, and then exchange data.
Of course you can also modify the code to connect more devices, we default to connect up to 4 devices, more than 4 you need to modify menuconfig.

