多连接指南
=============================

:link_to_translation:`en:[English]`

介绍
----

下表展示了每个 ESP 低功耗蓝牙主机所支持的最大多连接个数。在多连接场景下，需要对连接参数进行合理配置。通常情况下，随着连接数量的增加，连接间隔（Connection Interval）也应相应增大。具体的参数配置建议和 sdkconfig 详细信息，请参阅下表中的相应示例代码。

在本文档中，最大连接数指设备作为中心设备或外围设备时，能够同时维持的最大连接数量。

主机 (Host) SDKconfig
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. table:: ESP 低功耗蓝牙主机支持的最大连接数

   +---------------+-----------------------------+------------------------------+---------------------+
   | 主机 (Host)   |        最大连接个数         |          SDKconfig           |        示例         |
   +===============+=============================+==============================+=====================+
   |               |                             | |bluedroid_enable_config|    |                     |
   | ESP-Bluedroid | |max_bluedroid_connections| |                              | |bluedroid_example| |
   |               |                             | |bluedroid_connection_num|   |                     |
   +---------------+-----------------------------+------------------------------+---------------------+
   | ESP-NimBLE    | |max_nimble_connections|    |   |nimble_connection_num|    |   |nimble_example|  |
   +---------------+-----------------------------+------------------------------+---------------------+


控制器 (Controller) SDKconfig
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. only:: esp32

   - :ref:`BTDM_CTRL_BLE_MAX_CONN <CONFIG_BTDM_CTRL_BLE_MAX_CONN>`

   配置项 **BTDM_CTRL_BLE_MAX_CONN** 用于指定控制器 (Controller) 可同时支持的最大低功耗蓝牙连接数。该值必须与上表中主机端配置的最大连接数保持一致。

.. only:: esp32c3 or esp32s3

   - :ref:`BT_CTRL_BLE_MAX_ACT <CONFIG_BT_CTRL_BLE_MAX_ACT>`

   配置项 **BT_CTRL_BLE_MAX_ACT** 用于定义控制器 (Controller) 可同时处理的最大 Bluetooth LE 活动数。每个 Bluetooth LE 活动都会占用一个资源，包括：

   - 连接
   - 广播
   - 扫描
   - 周期性同步

   因此，该参数应按以下方式配置：

   **最大连接数 + 所需的广播、扫描及周期性同步实例数量**

   **示例：** 如果主机 (Host) 配置为支持最多 8 个连接，且应用需要同时运行 1 个广播实例和 1 个扫描实例，建议将 **BT_CTRL_BLE_MAX_ACT** 设置为 10。

.. only:: not esp32 and not esp32c3 and not esp32s3

   - 无需进行控制器 (Controller) 相关的 SDK 配置。


注意
----

1. 应用能够支持的最大连接数在很大程度上取决于整体内存使用情况。建议禁用不必要的功能，以优化多连接性能。

2. 当设备处于外围角色时，连接稳定性和整体性能将受到中心设备和协商连接参数的影响。

.. only:: not esp32 and not esp32c3 and not esp32s3 and not esp32c2

   3. 由于 ESP-Bluedroid 的内存占用相对更高，其支持的最大连接数会少于 ESP-Nimble。

   4. 如果您的应用需要的同时连接数量超过上述数值，请联系 `乐鑫客户支持团队 <https://www.espressif.com/zh-hans/contact-us/sales-questions>`__ 以获取进一步的协助。

.. |bluedroid_enable_config| replace:: :ref:`BT_MULTI_CONNECTION_ENBALE <CONFIG_BT_MULTI_CONNECTION_ENBALE>`
.. |bluedroid_connection_num| replace:: :ref:`BT_ACL_CONNECTIONS <CONFIG_BT_ACL_CONNECTIONS>`
.. |bluedroid_example| replace:: :example:`multi_conn <bluetooth/bluedroid/ble/ble_multi_conn>`
.. |nimble_connection_num| replace:: :ref:`BT_NIMBLE_MAX_CONNECTIONS <CONFIG_BT_NIMBLE_MAX_CONNECTIONS>`
.. |nimble_example| replace:: :example:`multi_conn<bluetooth/nimble/ble_multi_conn>`

.. only:: esp32 or esp32c3 or esp32s3

   .. |max_bluedroid_connections| replace:: 9
   .. |max_nimble_connections| replace:: 9

.. only:: esp32c2

   .. |max_bluedroid_connections| replace:: 2
   .. |max_nimble_connections| replace:: 2

.. only:: esp32h2

   .. |max_bluedroid_connections| replace:: 15
   .. |max_nimble_connections| replace:: 70

.. only:: esp32c6 or esp32c5 or esp32c61

   .. |max_bluedroid_connections| replace:: 50
   .. |max_nimble_connections| replace:: 70
