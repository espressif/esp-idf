Controller && VHCI
==================

Application Example
-------------------

Check :example:`bluetooth/hci` folder in ESP-IDF examples, which contains the following application:

* This is a Bluetooth® Low Energy (Bluetooth LE) advertising demo with virtual HCI interface. Send Reset/ADV_PARAM/ADV_DATA/ADV_ENABLE HCI command for Bluetooth Low Energy advertising - :example:`bluetooth/hci/controller_vhci_ble_adv`.

.. only:: esp32s3

    Please note that ESP32-S3 shares the same :component_file:`bt/include/esp32c3/include/esp_bt.h` and :component_file:`bt/controller/esp32c3/bt.c` files with ESP32-C3.

API Reference
-------------

.. include-build-file:: inc/esp_bt.inc
