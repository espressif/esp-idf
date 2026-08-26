Bluetooth® HID Host API
========================

Overview
--------

A Bluetooth HID host is a device or software that is capable of connecting and communicating with Bluetooth HID devices, such as keyboards, mice. Users can use the Bluetooth HID Host APIs to send output data or control commands to the HID devices, enabling them to control the behavior or settings of the devices.

Application Examples
--------------------

- :example:`bluetooth/esp_hid_host` demonstrates how to use the ESP-IDF Bluetooth HID Host APIs to create a Bluetooth Classic, Bluetooth LE, or Bluetooth dual-mode HID host that scans and connects to the most recently discovered Bluetooth HID device, dumps the HID device information, and receives data sent by the HID device.

API Reference
-------------

.. include-build-file:: inc/esp_hidh_api.inc
