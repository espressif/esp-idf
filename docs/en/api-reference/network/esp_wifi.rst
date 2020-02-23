Wi-Fi
=====

:link_to_translation:`zh_CN:[中文]`

Introduction
------------

The WiFi libraries provide support for configuring and monitoring the {IDF_TARGET_NAME} WiFi networking functionality. This includes configuration for:

- Station mode (aka STA mode or WiFi client mode). {IDF_TARGET_NAME} connects to an access point.
- AP mode (aka Soft-AP mode or Access Point mode). Stations connect to the {IDF_TARGET_NAME}.
- Combined AP-STA mode ({IDF_TARGET_NAME} is concurrently an access point and a station connected to another access point).

- Various security modes for the above (WPA, WPA2, WEP, etc.)
- Scanning for access points (active & passive scanning).
- Promiscuous mode for monitoring of IEEE802.11 WiFi packets.


Application Examples
--------------------

The :example:`wifi` directory of ESP-IDF examples contains the following applications:

Code examples for Wi-Fi are provided in the :example:`wifi` directory of ESP-IDF examples.

In addition, there is a simple `esp-idf-template <https://github.com/espressif/esp-idf-template>`_ application to demonstrate a minimal IDF project structure.


API Reference
-------------

.. include-build-file:: inc/esp_wifi.inc
.. include-build-file:: inc/esp_wifi_types.inc


