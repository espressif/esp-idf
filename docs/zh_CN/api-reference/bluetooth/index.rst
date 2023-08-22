蓝牙 API
*************

:link_to_translation:`en:[English]`

.. toctree::
   :maxdepth: 2

   bt_common
   bt_le
   :SOC_BT_CLASSIC_SUPPORTED: classic_bt
   controller_vhci
   :SOC_BLE_MESH_SUPPORTED: esp-ble-mesh
   nimble/index

ESP-IDF 目前支持两个主机堆栈。基于 Bluedroid 的堆栈（默认）支持传统蓝牙和低功耗蓝牙 (Bluetooth® LE)，而基于 Apache NimBLE 的堆栈仅支持低功耗蓝牙。用户可参考如下信息进行选择：

* 对于同时涉及传统蓝牙和低功耗蓝牙的用例，应该选用 Bluedroid。
* 对于仅涉及低功耗蓝牙的用例，建议选用 NimBLE。在代码占用和运行时，NimBLE 对内存的要求较低，因此适用于此类场景。

.. only:: esp32

    请点击下方链接，查看 ESP32 蓝牙架构：

      * `ESP32 蓝牙架构 (PDF) <https://espressif.com/sites/default/files/documentation/esp32_bluetooth_architecture_cn.pdf>`_

蓝牙 API 的示例代码存放于 ESP-IDF 示例项目的 :example:`bluetooth/bluedroid` 目录下。

下面的示例给出了详细介绍：

* :example_file:`GATT 客户端示例 <bluetooth/bluedroid/ble/gatt_client/tutorial/Gatt_Client_Example_Walkthrough.md>`
* :example_file:`GATT 服务端服务表格示例 <bluetooth/bluedroid/ble/gatt_server_service_table/tutorial/Gatt_Server_Service_Table_Example_Walkthrough.md>`
* :example_file:`GATT 服务端示例 <bluetooth/bluedroid/ble/gatt_server/tutorial/Gatt_Server_Example_Walkthrough.md>`
* :example_file:`GATT 客户端安全性示例 <bluetooth/bluedroid/ble/gatt_security_client/tutorial/Gatt_Security_Client_Example_Walkthrough.md>`
* :example_file:`GATT 服务端安全性示例 <bluetooth/bluedroid/ble/gatt_security_server/tutorial/Gatt_Security_Server_Example_Walkthrough.md>`
* :example_file:`GATT 客户端多连接示例 <bluetooth/bluedroid/ble/gattc_multi_connect/tutorial/Gatt_Client_Multi_Connection_Example_Walkthrough.md>`
