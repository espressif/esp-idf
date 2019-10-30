Bluetooth API
*************

:link_to_translation:`zh_CN:[中文]`

.. toctree::
   :maxdepth: 2

   Bluetooth Controller && VHCI <controller_vhci>
   Bluetooth Common <bt_common>
   Bluetooth LE <bt_le>
   Bluetooth Classic <classic_bt>
   NimBLE <nimble/index>
   ESP-BLE-MESH <esp-ble-mesh>

ESP-IDF currently supports two host stacks. The Bluedroid based stack (default) supports classic Bluetooth as well as BLE. On the other hand, Apache NimBLE based stack is BLE only. For users to make a choice:
* For usecases involving classic Bluetooth as well as BLE, Bluedroid should be used.
* For BLE-only usecases, using NimBLE is recommended. It is less demanding in terms of code footprint and runtime memory, making it suitable for such scenarios.


For the overview of the ESP32 Bluetooth stack architecture, follow the links below:

* `ESP32 Bluetooth Architecture (PDF) [English] <http://espressif.com/sites/default/files/documentation/esp32_bluetooth_architecture_en.pdf>`_
* `ESP32 Bluetooth Architecture (PDF) [中文] <http://espressif.com/sites/default/files/documentation/esp32_bluetooth_architecture_cn.pdf>`_

Code examples for this API section are provided in the :example:`bluetooth/bluedroid` directory of ESP-IDF examples.

The following examples contain detailed walkthroughs:

* :example_file:`GATT Client Example Walkthrough <bluetooth/bluedroid/ble/gatt_client/tutorial/Gatt_Client_Example_Walkthrough.md>`
* :example_file:`GATT Server Service Table Example Walkthrough <bluetooth/bluedroid/ble/gatt_server_service_table/tutorial/Gatt_Server_Service_Table_Example_Walkthrough.md>`
* :example_file:`GATT Server Example Walkthrough <bluetooth/bluedroid/ble/gatt_server/tutorial/Gatt_Server_Example_Walkthrough.md>`
* :example_file:`GATT Security Client Example Walkthrough <bluetooth/bluedroid/ble/gatt_security_client/tutorial/Gatt_Security_Client_Example_Walkthrough.md>`
* :example_file:`GATT Security Server Example Walkthrough <bluetooth/bluedroid/ble/gatt_security_server/tutorial/Gatt_Security_Server_Example_Walkthrough.md>`
* :example_file:`GATT Client Multi-connection Example Walkthrough <bluetooth/bluedroid/ble/gattc_multi_connect/tutorial/Gatt_Client_Multi_Connection_Example_Walkthrough.md>`
