ESP-IDF GATT SERVER create attribute table demo
===============================================

This is the demo for user to use ESP_APIs to create a GATT Server attribute table.
The table is easy to use to create GATT server service database without use each "attribute create" functions.
Actually, there are two way to create server service and characteristics.
One is use the esp_gatts_create_service or esp_ble_gatts_add_char and etc.
The other way is use esp_ble_gatts_create_attr_tab.
The important things: the two ways cannot use in the same service, but can use in different service.

Please check the [tutorial](tutorial/GATT_Server_Service_Table_Example_Walkthrough.md) for more information about this example.
