蓝牙\ :sup:`®` API
***********************

:link_to_translation:`en:[English]`

本章节提供 ESP-IDF 中蓝牙的 API 参考。ESP-IDF 支持两种主机协议栈：**Bluedroid** 和 **NimBLE**。

- **Bluedroid** （默认协议栈）：支持经典蓝牙和低功耗蓝牙，适用于同时使用这两种技术的应用场景。
- **NimBLE**：仅支持低功耗蓝牙，因代码体积小、内存占用低，适合资源受限的应用。

可通过下方链接查看相应的 API 文档和示例应用。

----

**控制器接口 API**

蓝牙主机协议栈与控制器之间的底层接口。

.. toctree::
    :maxdepth: 1

    controller_vhci


**Bluedroid 协议栈 API**

ESP-IDF 默认的主机协议栈，支持经典蓝牙和低功耗蓝牙。

.. toctree::
    :maxdepth: 1

    bt_common
    :SOC_BT_CLASSIC_SUPPORTED: classic_bt
    bt_le

有关蓝牙架构和功能的详细信息，请参阅 API 指南中的以下文档：

.. only:: SOC_BT_CLASSIC_SUPPORTED

    :doc:`../../api-guides/bt-architecture/index`、 :doc:`../../api-guides/classic-bt/index`、 :doc:`../../api-guides/ble/index`

.. only:: not SOC_BT_CLASSIC_SUPPORTED

    :doc:`../../api-guides/bt-architecture/index`、 :doc:`../../api-guides/ble/index`

**NimBLE 协议栈 API**

专为低功耗蓝牙设计的轻量级主机协议栈。

.. toctree::
    :maxdepth: 1

    nimble/index

更多详细信息和上游文档的 API 参考，请参阅 `Apache Mynewt NimBLE 用户指南 <https://mynewt.apache.org/latest/network/index.html>`_。

.. only:: SOC_BLE_MESH_SUPPORTED

    **ESP-BLE-MESH API**

    实现低功耗蓝牙 Mesh 网络功能。

    .. toctree::
        :maxdepth: 1

        esp-ble-mesh

----

示例与教程
------------

ESP-IDF 提供了丰富的示例代码和实践教程，路径见 examples 目录：

- **Bluedroid**: :example:`bluetooth/bluedroid`
- **NimBLE**: :example:`bluetooth/nimble`

使用 Bluedroid 协议栈进行开发的分步示例教程：

* :example_file:`GATT 客户端示例 <bluetooth/bluedroid/ble/gatt_client/tutorial/Gatt_Client_Example_Walkthrough.md>`
* :example_file:`GATT 服务端服务表格示例 <bluetooth/bluedroid/ble/gatt_server_service_table/tutorial/Gatt_Server_Service_Table_Example_Walkthrough.md>`
* :example_file:`GATT 服务端示例 <bluetooth/bluedroid/ble/gatt_server/tutorial/Gatt_Server_Example_Walkthrough.md>`
* :example_file:`GATT 客户端安全性示例 <bluetooth/bluedroid/ble/gatt_security_client/tutorial/Gatt_Security_Client_Example_Walkthrough.md>`
* :example_file:`GATT 服务端安全性示例 <bluetooth/bluedroid/ble/gatt_security_server/tutorial/Gatt_Security_Server_Example_Walkthrough.md>`
* :example_file:`GATT 客户端多连接示例 <bluetooth/bluedroid/ble/gattc_multi_connect/tutorial/Gatt_Client_Multi_Connection_Example_Walkthrough.md>`

使用 NimBLE 协议栈进行开发的分步示例教程：

* :example_file:`低功耗蓝牙中心设备示例 <bluetooth/nimble/blecent/tutorial/blecent_walkthrough.md>`
* :example_file:`低功耗蓝牙心率示例 <bluetooth/nimble/blehr/tutorial/blehr_walkthrough.md>`
* :example_file:`低功耗蓝牙外围设备示例 <bluetooth/nimble/bleprph/tutorial/bleprph_walkthrough.md>`
