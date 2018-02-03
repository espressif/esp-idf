GATT CLIENT API
===============

Overview
--------

`Instructions`_

.. _Instructions: ../template.html

Application Example
-------------------

Check :example:`bluetooth` folder in ESP-IDF examples, which contains the following demos and their tutorials:

* This is a GATT client demo and its tutorial. This demo can scan for devices, connect to the GATT server and discover its services. 

    - :example:`bluetooth/gatt_client`
    - :example_file:`GATT Client Example Walkthrough <bluetooth/gatt_client/tutorial/Gatt_Client_Example_Walkthrough.md>`

* This is a multiple connection demo and its tutorial. This demo can connect to multiple GATT server devices and discover their services. 

    - :example:`bluetooth/gattc_multi_connect`
    - :example_file:`GATT Client Multi-connection Example Walkthrough <bluetooth/gattc_multi_connect/tutorial/Gatt_Client_Multi_Connection_Example_Walkthrough.md>`

* This is a BLE SPP-Like demo. This demo, which acts as a GATT client, can receive data from UART and then send the data to the peer device automatically.

    - :example:`bluetooth/ble_spp_client` 

API Reference
-------------

.. include:: /_build/inc/esp_gattc_api.inc

