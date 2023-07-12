GATT Server API
===============

:link_to_translation:`zh_CN:[中文]`

Application Examples
--------------------

Check :example:`bluetooth/bluedroid/ble` folder in ESP-IDF examples, which contains the following demos and their tutorials:

* This is a GATT server demo and its tutorial. This demo creates a GATT service with an attribute table, which releases the user from the operation of adding attributes one by one. This is the recommended method of adding attributes (officially recommended).

    - :example:`bluetooth/bluedroid/ble/gatt_server_service_table`
    - :example_file:`GATT Server Service Table Example Walkthrough <bluetooth/bluedroid/ble/gatt_server_service_table/tutorial/Gatt_Server_Service_Table_Example_Walkthrough.md>`

* This is a GATT server demo and its tutorial. This demo creates a GATT service by adding attributes one by one as defined by Bluedroid. The recommended method of adding attributes is presented in the example below.

    - :example:`bluetooth/bluedroid/ble/gatt_server`
    - :example_file:`GATT Server Example Walkthrough <bluetooth/bluedroid/ble/gatt_server/tutorial/Gatt_Server_Example_Walkthrough.md>`

* This is a demo similar to Bluetooth® Low Energy (Bluetooth LE) SPP. In this demo, GATT server can receive data from UART and then send the data to the peer device automatically.

    - :example:`bluetooth/bluedroid/ble/ble_spp_server`

API Reference
-------------

.. include-build-file:: inc/esp_gatts_api.inc

