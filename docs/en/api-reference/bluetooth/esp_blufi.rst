BluFi API
=========

Overview
--------

BluFi is a profile based GATT to config ESP32 Wi-Fi to connect/disconnect AP or setup a softap and etc.

Use should concern these things:

1. The event sent from profile. Then you need to do something as the event indicate.
2. Security reference. You can write your own Security functions such as symmetrical encryption/decryption and checksum functions. Even you can define the "Key Exchange/Negotiation" procedure.

Application Examples
--------------------

- :example:`bluetooth/blufi` how to use the Blufi function to configure the Wi-Fi connection to an AP via a Bluetooth channel on {IDF_TARGET_NAME}.

API Reference
-------------

.. include-build-file:: inc/esp_blufi_api.inc
