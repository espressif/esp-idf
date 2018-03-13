Wi-Fi
=====

Introduction
------------

The WiFi libraries provide support for configuring and monitoring the ESP32 WiFi networking functionality. This includes configuration for:

- Station mode (aka STA mode or WiFi client mode). ESP32 connects to an access point.
- AP mode (aka Soft-AP mode or Access Point mode). Stations connect to the ESP32.
- Combined AP-STA mode (ESP32 is concurrently an access point and a station connected to another access point).

- Various security modes for the above (WPA, WPA2, WEP, etc.)
- Scanning for access points (active & passive scanning).
- Promiscuous mode monitoring of IEEE802.11 WiFi packets.

Application Examples
--------------------

See :example:`wifi` directory of ESP-IDF examples that contains the following applications:

* Simple application showing how to connect ESP32 module to an Access Point - `esp-idf-template <https://github.com/espressif/esp-idf-template>`_.

* Using power save mode of Wi-Fi - :example:`wifi/power_save`.


API Reference
-------------

.. include:: /_build/inc/esp_wifi.inc
.. include:: /_build/inc/esp_wifi_types.inc


