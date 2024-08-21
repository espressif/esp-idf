主要功能支持状态
================

:link_to_translation:`en:[English]`

本文档介绍了乐鑫低功耗蓝牙模块主要功能在 {IDF_TARGET_NAME} 上的支持状态。

|supported_def| **该功能已完成开发和内部测试。** [1]_

|experimental_def| **该功能已完成开发，正在进行内部测试。**
你可以探索这些功能以进行评估和反馈，但应注意可能出现的问题。

|developing_def| **该功能目前正在积极开发中， 预计在 YYYY/MM 月底之前支持。**
请关注此表以获得该功能的最新进展。
如果确实有紧急的开发需求，请联系 `乐鑫客户支持团队 <https://www.espressif.com/zh-hans/contact-us/sales-questions>`__ 以了解是否可以进行功能试用。

|unsupported_def| **该功能在此芯片上不支持。** 如果你有相关需求，请优先选择其他支持该功能的乐鑫芯片系列。
如果当前的乐鑫产品都不支持此功能，请联系 `乐鑫客户支持团队 <https://www.espressif.com/zh-hans/contact-us/sales-questions>`__ ，我们的研发团队会对你的需求进行内部可行性评估。

|NA_def| 具有此标签的功能可能为以下两种类型：
    - **Host-only 功能**：该功能仅存在于 HCI 层之上，例如 GATT Caching。此类功能不需要蓝牙控制器的支持。
    - **Controller-only 功能**：该功能仅存在于 HCI 层之下，且无法通过主机API配置或启用，例如 Advertising Channel Index 。此类功能不需要蓝牙主机的支持。

.. list-table::
    :width: 100%
    :widths: auto
    :header-rows: 1

    * - .. centered:: 核心协议
      - .. centered:: 主要功能
      - .. centered:: ESP 控制器
      - .. centered:: ESP-Bluedroid 主机
      - .. centered:: ESP-NimBLE 主机
    * - .. centered:: |4.2|
      - LE Data Packet Length Extension
      - |supported|
      - |supported|
      - |supported|
    * -
      - LE Secure Connections
      - |supported|
      - |supported|
      - |supported|
    * -
      - Link Layer Privacy
      - |supported|
      - |supported|
      - |supported|
    * -
      - Link Layer Extended Filter Policies
      - |supported|
      - |supported|
      - |supported|
    * - .. centered:: |5.0|
      - 2 Msym/s PHY for LE
      - .. only:: esp32

            |unsupported|
        .. only:: not esp32

            |supported|
      - .. only:: esp32

            |unsupported|
        .. only:: not esp32

            |supported|
      - .. only:: esp32

            |unsupported|
        .. only:: not esp32

            |supported|
    * -
      - LE Long Range (Coded PHY S=2/S=8)
      - .. only:: esp32

            |unsupported|
        .. only:: not esp32

            |supported|
      - .. only:: esp32

            |unsupported|
        .. only:: not esp32

            |supported|
      - .. only:: esp32

            |unsupported|
        .. only:: not esp32

            |supported|
    * -
      - High Duty Cycle Non-Connectable Advertising
      - .. only:: esp32

            |unsupported|
        .. only:: not esp32

            |supported|
      - .. only:: esp32

            |unsupported|
        .. only:: not esp32

            |supported|
      - .. only:: esp32

            |unsupported|
        .. only:: not esp32

            |supported|
    * -
      - LE Advertising Extensions
      - .. only:: esp32

            |unsupported|
        .. only:: not esp32

            |supported|
      - .. only:: esp32

            |unsupported|
        .. only:: not esp32

            |supported|
      - .. only:: esp32

            |unsupported|
        .. only:: not esp32

            |supported|
    * -
      - LE Channel Selection Algorithm #2
      - .. only:: esp32

            |unsupported|
        .. only:: not esp32

            |supported|
      - .. only:: esp32

            |unsupported|
        .. only:: not esp32

            |supported|
      - .. only:: esp32

            |unsupported|
        .. only:: not esp32

            |supported|
    * - .. centered:: |5.1|
      - Angle of Arrival (AoA)/Angle of Departure (AoD)
      - |unsupported|
      - |unsupported|
      - |unsupported|
    * -
      - GATT Caching
      - |NA|
      - |experimental|
      - |experimental|
    * -
      - Advertising Channel Index
      - |unsupported|
      - |NA|
      - |NA|
    * -
      - Periodic Advertising Sync Transfer
      - .. only:: esp32 or esp32c3 or esp32s3

            |unsupported|
        .. only:: esp32c6 or esp32h2 or esp32c2 or esp32c5

            |experimental|
      - .. only:: esp32 or esp32c3 or esp32s3

            |unsupported|
        .. only:: esp32c6 or esp32h2 or esp32c2 or esp32c5

            |experimental|
      - .. only:: esp32 or esp32c3 or esp32s3

            |unsupported|
        .. only:: esp32c6 or esp32h2 or esp32c2 or esp32c5

            |experimental|
    * - .. centered:: |5.2|
      - LE Isochronous Channels (BIS/CIS)
      - |unsupported|
      - |unsupported|
      - |unsupported|
    * -
      - Enhanced Attribute Protocol
      - |NA|
      - |unsupported|
      - |developing202412|
    * -
      - LE Power Control
      - .. only:: esp32 or esp32c2

            |unsupported|
        .. only:: esp32c6 or esp32h2 or esp32c3 or esp32s3 or esp32c5

            |experimental|
      - |unsupported|
      - .. only:: esp32 or esp32c2

            |unsupported|
        .. only:: esp32c6 or esp32h2 or esp32c3 or esp32s3 or esp32c5

            |experimental|
    * - .. centered:: |5.3|
      - AdvDataInfo in Periodic Advertising
      - .. only:: esp32 or esp32c3 or esp32s3

            |unsupported|
        .. only:: esp32c6 or esp32c2 or esp32h2 or esp32c5

            |supported|
      - .. only:: esp32 or esp32c3 or esp32s3

            |unsupported|
        .. only:: esp32c6 or esp32c2 or esp32h2 or esp32c5

           |supported|
      - .. only:: esp32 or esp32c3 or esp32s3

            |unsupported|
        .. only:: esp32c6 or esp32c2 or esp32h2 or esp32c5

            |supported|
    * -
      - LE Enhanced Connection Update (Connection Subrating)
      - .. only:: esp32 or esp32c6 or esp32c2 or esp32h2 or esp32c5

            |unsupported|
        .. only:: esp32c3 or esp32s3

            |experimental|
      - |unsupported|
      - .. only:: esp32 or esp32c6 or esp32c2 or esp32h2 or esp32c5

            |unsupported|
        .. only:: esp32c3 or esp32s3

            |experimental|
    * -
      - LE Channel Classification
      - .. only:: esp32 or esp32c3 or esp32s3 or esp32c2

            |unsupported|
        .. only:: esp32c6 or esp32h2 or esp32c5

            |experimental|
      - .. only:: esp32 or esp32c3 or esp32s3 or esp32c2

            |unsupported|
        .. only:: esp32c6 or esp32h2 or esp32c5

            |experimental|
      - .. only:: esp32 or esp32c3 or esp32s3 or esp32c2

            |unsupported|
        .. only:: esp32c6 or esp32h2 or esp32c5

            |experimental|
    * - .. centered:: |5.4|
      - Advertising Coding Selection
      - .. only:: esp32 or esp32c6 or esp32c2 or esp32h2 or esp32c5

            |unsupported|
        .. only:: esp32c3 or esp32s3

            |experimental|
      - |unsupported|
      - .. only:: esp32 or esp32c6 or esp32c2 or esp32h2 or esp32c5

            |unsupported|
        .. only:: esp32c3 or esp32s3

            |experimental|
    * -
      - Encrypted Advertising Data
      - |NA|
      - |unsupported|
      - |experimental|
    * -
      - LE GATT Security Levels Characteristic
      - |NA|
      - |unsupported|
      - |developing202412|
    * -
      - Periodic Advertising with Responses
      - |unsupported|
      - |unsupported|
      - |unsupported|

.. [1]
   如果想了解支持功能的 Bluetooth SIG 认证状态，
   请查阅 `SIG 蓝牙产品数据库 <https://qualification.bluetooth.com/Listings/Search>`__ 。

对于大部分开发需要在控制器 (Controller) 完成的功能，其主机层 (Host) 的支持状态将会受限于控制器层的支持状态。
如果你计划将乐鑫低功耗蓝牙控制器和主机跑在不同的乐鑫芯片上，则主机的功能将不再受限于这颗跑主机的芯片上的控制器的功能支持状态，
请参阅 :doc:`ESP 主机主要功能支持状态 <host-feature-support-status>` 。

请注意，本文档不构成对客户的约束性承诺。
以上所列出来的功能支持状态信息仅供参考，可能会在不通知的情况下发生更改。
建议联系 `乐鑫客户支持团队 <https://www.espressif.com/zh-hans/contact-us/sales-questions>`__ 以获取最新信息，并确认功能是否适合你的特定需求。


.. |supported| image:: ../../../_static/ble/feature_status/supported.svg
   :class: align-center
   :width: 65px
.. |developing202412| image:: ../../../_static/ble/feature_status/developing202412.svg
   :class: align-center
   :width: 120px
.. |unsupported| image:: ../../../_static/ble/feature_status/unsupported.svg
   :class: align-center
   :width: 75px
.. |experimental| image:: ../../../_static/ble/feature_status/experimental.svg
   :class: align-center
   :width: 75px
.. |NA| image:: ../../../_static/ble/feature_status/NA.svg
   :class: align-center
   :width: 25px
.. |supported_def| image:: ../../../_static/ble/feature_status/supported.svg
.. |developing_def| image:: ../../../_static/ble/feature_status/developingYYYYMM.svg
.. |unsupported_def| image:: ../../../_static/ble/feature_status/unsupported.svg
.. |experimental_def| image:: ../../../_static/ble/feature_status/experimental.svg
.. |NA_def| image:: ../../../_static/ble/feature_status/NA.svg
.. |4.2| replace:: `4.2 <https://www.bluetooth.com/specifications/specs/core-specification-4-2/>`__
.. |5.0| replace:: `5.0 <https://www.bluetooth.com/specifications/specs/core-specification-5-0/>`__
.. |5.1| replace:: `5.1 <https://www.bluetooth.com/specifications/specs/core-specification-5-1/>`__
.. |5.2| replace:: `5.2 <https://www.bluetooth.com/specifications/specs/core-specification-5-2/>`__
.. |5.3| replace:: `5.3 <https://www.bluetooth.com/specifications/specs/core-specification-5-3/>`__
.. |5.4| replace:: `5.4 <https://www.bluetooth.com/specifications/specs/core-specification-5-4/>`__
