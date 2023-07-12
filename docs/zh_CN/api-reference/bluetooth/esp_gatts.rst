GATT 服务器 API
=====================

:link_to_translation:`en:[English]`

应用示例
-------------------

以下示例及其教程存放在 ESP-IDF 示例的 :example:`bluetooth/bluedroid/ble` 目录下：

* 以下为 GATT 服务器示例及其教程，该示例创建了一个带有属性表的 GATT 服务器，无需逐个添加属性。建议通过以下方式添加属性（官方推荐）。

    - :example:`bluetooth/bluedroid/ble/gatt_server_service_table`
    - :example_file:`GATT 服务器服务表示例 <bluetooth/bluedroid/ble/gatt_server_service_table/tutorial/Gatt_Server_Service_Table_Example_Walkthrough.md>`

* 以下为 GATT 服务器示例及其教程，该示例逐个添加 Bluedroid 定义的属性，创建 GATT 服务。建议通过以下方式添加属性。

    - :example:`bluetooth/bluedroid/ble/gatt_server`
    - :example_file:`GATT 服务器示例 <bluetooth/bluedroid/ble/gatt_server/tutorial/Gatt_Server_Example_Walkthrough.md>`

* 以下示例与低功耗蓝牙 (Bluetooth® LE) SPP 类似。在该示例中，GATT 服务器可以接收来自 UART 的数据，并将数据自动发送到对端设备。

    - :example:`bluetooth/bluedroid/ble/ble_spp_server`

API 参考
-------------

.. include-build-file:: inc/esp_gatts_api.inc

