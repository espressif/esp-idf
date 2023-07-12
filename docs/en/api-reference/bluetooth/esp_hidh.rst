Bluetooth® HID Host API
========================

Overview
--------

A Bluetooth HID host is a device or software that is capable of connecting and communicating with Bluetooth HID devices, such as keyboards, mice. Users can use the Bluetooth HID Host APIs to send output data or control commands to the HID devices, enabling them to control the behavior or settings of the devices.

Application Example
-------------------

Check :example:`bluetooth` folder in ESP-IDF examples, which contains the following application:

* Example :example:`bluetooth/esp_hid_host` is implemented using the generic esp_hid APIs. esp_hid APIs are build upon the Bluetooth HID APIs and can be a reference.


API Reference
-------------

.. include-build-file:: inc/esp_hidh_api.inc
