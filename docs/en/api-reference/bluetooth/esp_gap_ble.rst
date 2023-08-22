GAP API
=======

:link_to_translation:`zh_CN:[中文]`

Application Example
-------------------

Check the :example:`bluetooth/bluedroid/ble` folder in ESP-IDF examples, which contains the following demos and their tutorials:

* The following shows an SMP security client demo with its tutorial. This demo initiates its security parameters and acts as a GATT client, which can send a security request to the peer device and then complete the encryption procedure.

    - :example:`bluetooth/bluedroid/ble/gatt_security_client`
    - :example_file:`GATT Security Client Example Walkthrough <bluetooth/bluedroid/ble/gatt_security_client/tutorial/Gatt_Security_Client_Example_Walkthrough.md>`

* The following shows an SMP security server demo with its tutorial. This demo initiates its security parameters and acts as a GATT server, which can send a pair request to the peer device and then complete the encryption procedure.

    - :example:`bluetooth/bluedroid/ble/gatt_security_server`
    - :example_file:`GATT Security Server Example Walkthrough <bluetooth/bluedroid/ble/gatt_security_server/tutorial/Gatt_Security_Server_Example_Walkthrough.md>`

API Reference
-------------

.. include-build-file:: inc/esp_gap_ble_api.inc

