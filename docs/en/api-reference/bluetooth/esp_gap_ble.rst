GAP API
=======

Overview
--------

`Instructions`_

.. _Instructions: ../template.html

Application Example
-------------------

Check :example:`bluetooth` folder in ESP-IDF examples, which contains the following demos and their tutorials:

* This is a SMP security client demo and its tutorial. This demo initiates its security parameters and acts as a GATT client, which can send a security request to the peer device and then complete the encryption procedure.

    - :example:`bluetooth/gatt_security_client`  
    - :example_file:`GATT Security Client Example Walkthrough <bluetooth/gatt_security_client/tutorial/Gatt_Security_Client_Example_Walkthrough.md>`

* This is a SMP security server demo and its tutorial. This demo initiates its security parameters and acts as a GATT server, which can send a pair request to the peer device and then complete the encryption procedure. 

    - :example:`bluetooth/gatt_security_server`
    - :example_file:`GATT Security Server Example Walkthrough <bluetooth/gatt_security_server/tutorial/Gatt_Security_Server_Example_Walkthrough.md>`

API Reference
-------------

.. include:: /_build/inc/esp_gap_ble_api.inc

