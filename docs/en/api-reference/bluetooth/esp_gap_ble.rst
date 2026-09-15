GAP API
=======

:link_to_translation:`zh_CN:[中文]`

Application Examples
--------------------

- :example:`bluetooth/bluedroid/ble/gatt_security_client` demonstrates how to use ESP BLE security APIs on {IDF_TARGET_NAME} to establish a secure connection and encrypt communication with peer devices while acting as a GATT client.

- :example:`bluetooth/bluedroid/ble/gatt_security_server` demonstrates how to use ESP BLE security APIs on {IDF_TARGET_NAME} to establish a secure connection and encrypt communication with peer devices while acting as a GATT server.

- :example:`bluetooth/bluedroid/ble/ble_enc_adv_data/enc_adv_data_prph` demonstrates Encrypted Advertising Data and how to publish Key Material with :cpp:func:`esp_ble_gap_set_key_material`. See also :doc:`esp_ble_ead`.

API Reference
-------------

.. include-build-file:: inc/esp_gap_ble_api.inc
