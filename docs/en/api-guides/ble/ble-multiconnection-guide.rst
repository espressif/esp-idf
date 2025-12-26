Multi-Connection Guide
==========================================

:link_to_translation:`zh_CN:[中文]`

Introduction
--------------

The following table provides an overview of the maximum number of concurrent connections supported for each ESP Bluetooth LE Host. In multi-connection scenarios, connection parameters must be configured appropriately. In general, as the number of connections increases, the connection interval should be increased accordingly. For detailed parameter configuration recommendations and SDK configuration details, please refer to the corresponding example code in the following table.

In this document, the maximum number of connections refers to the maximum number of simultaneous active connections that the device can maintain, whether operating as a central or peripheral.

.. table:: Maximum Concurrent Connections by ESP Bluetooth LE Host

   +---------------+-----------------------------+------------------------------+---------------------+
   |     Host      | Max Connections             |        SDKconfig             |      Example        |
   +===============+=============================+==============================+=====================+
   |               |                             | |bluedroid_enable_config|    |                     |
   | ESP-Bluedroid | |max_bluedroid_connections| |                              | |bluedroid_example| |
   |               |                             | |bluedroid_connection_num|   |                     |
   +---------------+-----------------------------+------------------------------+---------------------+
   | ESP-NimBLE    | |max_nimble_connections|    |   |nimble_connection_num|    |   |nimble_example|  |
   +---------------+-----------------------------+------------------------------+---------------------+


Note
----

1. The ability to support multiple connections highly depends on the application’s overall memory usage. It is recommended to disable unnecessary features to optimize multi-connection performance.

2. When the device operates in the peripheral role, connection stability and overall performance will be influenced by the central device and the negotiated connection parameters.

.. only:: not esp32 and not esp32c3 and not esp32s3 and not esp32c2

   3. Due to the relatively higher memory usage of ESP-Bluedroid, it supports fewer concurrent connections compared to ESP-Nimble.

   4. If your application requires more simultaneous connections than the values listed above, please contact our `customer support team <https://www.espressif.com/en/contact-us/sales-questions>`__ for further assistance.

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
