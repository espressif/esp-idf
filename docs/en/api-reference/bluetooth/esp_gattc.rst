GATT Client API
===============

Application Examples
--------------------

- :example:`bluetooth/bluedroid/ble/gatt_client` demonstrates how to create a GATT Client that connects to a GATT server, enabling the server's notification function to discover its service.

- :example:`bluetooth/bluedroid/ble/gattc_multi_connect` demonstrates how to create a GATT Client that connects to multiple GATT servers, enabling their notification functions to discover their services.

- :example:`bluetooth/bluedroid/coex/gattc_gatts_coex` demonstrates the coexistence of GATT client and GATT server by creating a GATT service, starting ADV, and exchanging data between a GATT client and device.

API Reference
-------------

.. include-build-file:: inc/esp_gattc_api.inc

