Controller & HCI
==================================================

Application Example
-------------------

Check :example:`bluetooth/hci` folder in ESP-IDF examples, which contains the following application:

* This is a BLE advertising demo with virtual HCI interface. Send Reset/ADV_PARAM/ADV_DATA/ADV_ENABLE HCI command for BLE advertising - :example:`bluetooth/hci/controller_vhci_ble_adv`.

API Reference
-------------

.. include-build-file:: inc/esp_bt.inc


HCI Vendor-specific (VS) Commands
--------------------------------------

Espressif's HCI VS commands are exclusively designed for use with Espressif's Bluetooth Host stack or internal debugging purposes. Application developers **should not** initialize or invoke these VS commands in their applications. Please refer to :doc:`bt_vhci`  for detailed information.

