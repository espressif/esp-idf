Wi-Fi
=====

:link_to_translation:`zh_CN:[中文]`

Introduction
------------

The WiFi libraries provide support for configuring and monitoring the ESP32 WiFi networking functionality. This includes configuration for:

- Station mode (aka STA mode or WiFi client mode). ESP32 connects to an access point.
- AP mode (aka Soft-AP mode or Access Point mode). Stations connect to the ESP32.
- Combined AP-STA mode (ESP32 is concurrently an access point and a station connected to another access point).

- Various security modes for the above (WPA, WPA2, WEP, etc.)
- Scanning for access points (active & passive scanning).
- Promiscuous mode for monitoring of IEEE802.11 WiFi packets.


Application Examples
--------------------

The :example:`wifi` directory of ESP-IDF examples contains the following applications:

Code examples for Wi-Fi are provided in the :example:`wifi` directory of ESP-IDF examples.

In addition, there is a simple `esp-idf-template <https://github.com/espressif/esp-idf-template>`_ application that shows how to connect ESP32 module to an Access Point.


API Reference
-------------

.. include:: /_build/inc/esp_wifi.inc
.. include:: /_build/inc/esp_wifi_types.inc


