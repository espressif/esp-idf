GAP API
=======

:link_to_translation:`en:[English]`

应用示例
-------------------

以下示例及其教程存放在 ESP-IDF 项目示例的 :example:`bluetooth/bluedroid/ble` 目录下。

* 以下为 SMP（安全管理协议）安全客户端示例及教程。该示例程序会初始化其安全参数，并充当 GATT 客户端，可向对端设备发送安全请求，完成加密过程。

    - :example:`bluetooth/bluedroid/ble/gatt_security_client`
    - :example_file:`GATT 安全客户端示例演示 <bluetooth/bluedroid/ble/gatt_security_client/tutorial/Gatt_Security_Client_Example_Walkthrough.md>`

* 以下为 SMP 安全服务器示例及其教程。该示例程序会初始化其安全参数，并充当 GATT 客户端，可向对端设备发送配对请求，完成加密过程。

    - :example:`bluetooth/bluedroid/ble/gatt_security_server`
    - :example_file:`GATT 安全服务器示例演示 <bluetooth/bluedroid/ble/gatt_security_server/tutorial/Gatt_Security_Server_Example_Walkthrough.md>`

API 参考
-------------

.. include-build-file:: inc/esp_gap_ble_api.inc

