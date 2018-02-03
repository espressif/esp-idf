GATT SERVER API
===============

Overview
--------

`Instructions`_

.. _Instructions: ../template.html

Application Example
-------------------

Check :example:`bluetooth` folder in ESP-IDF examples, which contains the following demos and their tutorials:

* This is a GATT sever demo and its tutorial. This demo creates a GATT service with an attribute table, which releases the user from adding attributes one by one. This is the recommended method of adding attributes. 

    - :example:`bluetooth/gatt_server_service_table`
    - :example_file:`GATT Server Service Table Example Walkthrough <bluetooth/gatt_server_service_table/tutorial/Gatt_Server_Service_Table_Example_Walkthrough.md>`

* This is a GATT server demo and its tutorial. This demo creates a GATT service by adding attributes one by one as defined by Bluedroid. The recommended method of adding attributes is presented in example above.

    - :example:`bluetooth/gatt_server`
    - :example_file:`GATT Server Example Walkthrough <bluetooth/gatt_server/tutorial/Gatt_Server_Example_Walkthrough.md>`

* This is a BLE SPP-Like demo. This demo, which acts as a GATT server, can receive data from UART and then send the data to the peer device automatically.

    - :example:`bluetooth/ble_spp_server`

API Reference
-------------

.. include:: /_build/inc/esp_gatts_api.inc

