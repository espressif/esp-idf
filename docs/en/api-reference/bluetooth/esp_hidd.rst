Bluetooth® HID Device API
=========================

Overview
--------

A Bluetooth HID device is a device providing the service of human or other data input and output to and from a Bluetooth HID Host. Users can use the Bluetooth HID Device APIs to make devices like keyboards, mice, joysticks and so on.

Application Examples
--------------------

- :example:`bluetooth/bluedroid/classic_bt/bt_hid_mouse_device` demonstrates how to implement a Bluetooth HID device by simulating a Bluetooth HID mouse device that periodically sends reports to a remote Bluetooth HID host.

- :example:`bluetooth/esp_hid_device` demonstrates how to create a Bluetooth Classic or Bluetooth LE HID device, which can function as a mouse or a remote control, depending on the mode set by the user.

API Reference
-------------

.. include-build-file:: inc/esp_hidd_api.inc
