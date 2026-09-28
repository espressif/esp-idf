Bluetooth® OPP API
==================

:link_to_translation:`zh_CN:[中文]`

Overview
--------

OPP (Object Push Profile) enables pushing objects such as vCards and other files between Bluetooth devices over OBEX. It is commonly used for contact exchange, file sharing, and similar one-way object transfer scenarios. The OPP API provides functionality for both server and client roles.

Application Examples
--------------------

- :example:`bluetooth/bluedroid/classic_bt/bt_opp_server` demonstrates how to implement an OPP server that receives objects.
- :example:`bluetooth/bluedroid/classic_bt/bt_opp_client` demonstrates how to implement an OPP client that discovers the server by device name and sends sample vCards.

API Reference
-------------

.. include-build-file:: inc/esp_opp_api.inc
