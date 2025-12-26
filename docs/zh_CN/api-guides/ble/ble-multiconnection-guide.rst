多连接指南
=============================

:link_to_translation:`en:[English]`

介绍
----

下表展示了每个 ESP 低功耗蓝牙主机所支持的最大多连接个数。在多连接场景下，需要对连接参数进行合理配置。通常情况下，随着连接数量的增加，连接间隔（Connection Interval）也应相应增大。具体的参数配置建议和 sdkconfig 详细信息，请参阅下表中的相应示例代码。

在本文档中，最大连接数指设备作为中心设备或外围设备时，能够同时维持的最大连接数量。

.. table:: ESP 低功耗蓝牙主机支持的最大连接数

   +---------------+-----------------------------+------------------------------+---------------------+
   | 主机（Host）  |        最大连接个数         |          SDKconfig           |        示例         |
   +===============+=============================+==============================+=====================+
   |               |                             | |bluedroid_enable_config|    |                     |
   | ESP-Bluedroid | |max_bluedroid_connections| |                              | |bluedroid_example| |
   |               |                             | |bluedroid_connection_num|   |                     |
   +---------------+-----------------------------+------------------------------+---------------------+
   | ESP-NimBLE    | |max_nimble_connections|    |   |nimble_connection_num|    |   |nimble_example|  |
   +---------------+-----------------------------+------------------------------+---------------------+


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
