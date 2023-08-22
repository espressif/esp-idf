Bluetooth® HID Device API
=========================

Overview
--------

A Bluetooth HID device is a device providing the service of human or other data input and output to and from a Bluetooth HID Host. Users can use the Bluetooth HID Device APIs to make devices like keyboards, mice, joysticks and so on.

Application Example
-------------------

Check :example:`bluetooth/bluedroid/classic_bt` folder in ESP-IDF examples, which contains the following application:

* This is an example of Bluetooth HID mouse device. The device running this example can be discovered and connected by a Bluetooth HID Host device such as a PC, and the pointer will move left and right after HID connection is established - :example:`bluetooth/bluedroid/classic_bt/bt_hid_mouse_device`

API Reference
-------------

.. include-build-file:: inc/esp_hidd_api.inc
