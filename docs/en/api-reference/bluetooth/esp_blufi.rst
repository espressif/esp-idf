BLUFI API
=========

Overview
--------
BLUFI is a profile based GATT to config ESP32 WIFI to connect/disconnect AP or setup a softap and etc.
Use should concern these things:

1. The event sent from profile. Then you need to do something as the event indicate.
2. Security reference. You can write your own Security functions such as symmetrical encryption/decryption and checksum functions. Even you can define the "Key Exchange/Negotiation" procedure.

Application Example
-------------------

Check :example:`bluetooth/bluedroid/ble` folder in ESP-IDF examples, which contains the following application:

* This is the BLUFI demo. This demo can set ESP32's wifi to softap/station/softap&station mode and config wifi connections - :example:`bluetooth/bluedroid/ble/blufi`

API Reference
-------------

.. include:: /_build/inc/esp_blufi_api.inc

