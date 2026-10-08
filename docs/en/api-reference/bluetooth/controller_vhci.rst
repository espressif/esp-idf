Controller & HCI
==================================================

:link_to_translation:`zh_CN:[中文]`

.. only:: esp32 or esp32c3 or esp32s3 or esp32c2 or esp32c5 or esp32c6 or esp32c61 or esp32h2 or esp32h21 or esp32h4 or esp32s31

    Application Examples
    --------------------

    - :example:`bluetooth/hci/controller_hci_uart` demonstrates how to configure the Bluetooth Controller's HCI to communicate over UART on {IDF_TARGET_NAME}, enabling communication with an external Bluetooth host stack.

    .. only:: esp32

        - :example:`bluetooth/hci/ble_adv_scan_combined` demonstrates how to use Bluetooth capabilities for advertising and scanning with a virtual Host Controller Interface (HCI). This example shows how to implement some host functionalities without a host and displays scanned advertising reports from other devices.

        - :example:`bluetooth/hci/controller_vhci_ble_adv` demonstrates how to use the ESP-IDF VHCI ble_advertising app to perform advertising without a host and display received HCI events from the controller.

API Reference
-------------

.. include-build-file:: inc/esp_bt.inc


HCI Vendor-specific (VS) Commands
--------------------------------------

Espressif's HCI VS commands are exclusively designed for use with Espressif's Bluetooth Host stack or internal debugging purposes. Application developers **should not** initialize or invoke these VS commands in their applications. Please refer to :doc:`bt_vhci` for detailed information.
