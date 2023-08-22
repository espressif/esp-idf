GATT Client API
===============

Application Example
-------------------

Check :example:`bluetooth/bluedroid/ble` folder in ESP-IDF examples, which contains the following demos and their tutorials:

* This is a GATT client demo and its tutorial. This demo can scan for devices, connect to the GATT server and discover its services.

    - :example:`bluetooth/bluedroid/ble/gatt_client`
    - :example_file:`GATT Client Example Walkthrough <bluetooth/bluedroid/ble/gatt_client/tutorial/Gatt_Client_Example_Walkthrough.md>`

* This is a multiple connection demo and its tutorial. This demo can connect to multiple GATT server devices and discover their services.

    - :example:`bluetooth/bluedroid/ble/gattc_multi_connect`
    - :example_file:`GATT Client Multi-connection Example Walkthrough <bluetooth/bluedroid/ble/gattc_multi_connect/tutorial/Gatt_Client_Multi_Connection_Example_Walkthrough.md>`

* This is a demo similar to Bluetooth® Low Energy (Bluetooth LE) SPP. This demo, which acts as a GATT client, can receive data from UART and then send the data to the peer device automatically.

    - :example:`bluetooth/bluedroid/ble/ble_spp_client`

API Reference
-------------

.. include-build-file:: inc/esp_gattc_api.inc

