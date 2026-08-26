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
      - .. only:: SOC_BLE_50_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_50_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_50_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_50_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_50_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_50_SUPPORTED

            |unsupported|
    * -
      - LE Long Range (Coded PHY S=2/S=8)
      - .. only:: SOC_BLE_50_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_50_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_50_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_50_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_50_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_50_SUPPORTED

            |unsupported|
    * -
      - High Duty Cycle Non-Connectable Advertising
      - .. only:: SOC_BLE_50_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_50_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_50_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_50_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_50_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_50_SUPPORTED

            |unsupported|
    * -
      - LE Advertising Extensions
      - .. only:: SOC_BLE_50_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_50_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_50_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_50_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_50_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_50_SUPPORTED

            |unsupported|
    * -
      - LE Channel Selection Algorithm #2
      - .. only:: SOC_BLE_50_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_50_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_50_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_50_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_50_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_50_SUPPORTED

            |unsupported|
    * - .. centered:: |5.1|
      - .. only:: esp32h2

            Angle of Arrival (AoA)/Angle of Departure (AoD) [2]_
        .. only:: not esp32h2

            Angle of Arrival (AoA)/Angle of Departure (AoD)
      - .. only:: SOC_BLE_CTE_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_CTE_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_CTE_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_CTE_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_CTE_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_CTE_SUPPORTED

            |unsupported|
    * -
      - GATT Caching
      - |NA|
      - |experimental|
      - |experimental|
    * -
      - Randomized Advertising Channel Indexing
      - .. only:: esp32c2 or not SOC_ESP_NIMBLE_CONTROLLER

            |unsupported|
        .. only:: SOC_ESP_NIMBLE_CONTROLLER and not esp32c2

            |experimental|
      - |NA|
      - |NA|
    * -
      - Periodic Advertising Sync Transfer
      - .. only:: SOC_ESP_NIMBLE_CONTROLLER

            |supported|
        .. only:: not SOC_ESP_NIMBLE_CONTROLLER

            |unsupported|
      - .. only:: SOC_ESP_NIMBLE_CONTROLLER

            |supported|
        .. only:: not SOC_ESP_NIMBLE_CONTROLLER

            |unsupported|
      - .. only:: SOC_ESP_NIMBLE_CONTROLLER

            |supported|
        .. only:: not SOC_ESP_NIMBLE_CONTROLLER

            |unsupported|
    * - .. centered:: |5.2|
      - LE Isochronous Channels (BIS/CIS)
      - .. only:: SOC_BLE_ISO_SUPPORTED

            |experimental|
        .. only:: not SOC_BLE_ISO_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_ISO_SUPPORTED

            |experimental|
        .. only:: not SOC_BLE_ISO_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_ISO_SUPPORTED

            |experimental|
        .. only:: not SOC_BLE_ISO_SUPPORTED

            |unsupported|
    * -
      - Enhanced Attribute Protocol
      - |NA|
      - |unsupported|
      - |experimental|
    * -
      - LE Power Control
      - .. only:: SOC_BLE_POWER_CONTROL_SUPPORTED

            |experimental|
        .. only:: not SOC_BLE_POWER_CONTROL_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_POWER_CONTROL_SUPPORTED

            |experimental|
        .. only:: not SOC_BLE_POWER_CONTROL_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_POWER_CONTROL_SUPPORTED

            |experimental|
        .. only:: not SOC_BLE_POWER_CONTROL_SUPPORTED

            |unsupported|
    * - .. centered:: |5.3|
      - AdvDataInfo in Periodic Advertising
      - .. only:: SOC_BLE_PERIODIC_ADV_ENH_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_PERIODIC_ADV_ENH_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_PERIODIC_ADV_ENH_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_PERIODIC_ADV_ENH_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_PERIODIC_ADV_ENH_SUPPORTED

            |supported|
        .. only:: not SOC_BLE_PERIODIC_ADV_ENH_SUPPORTED

            |unsupported|
    * -
      - LE Enhanced Connection Update (Connection Subrating)
      - .. only:: SOC_BLE_SUBRATE_SUPPORTED

            |experimental|
        .. only:: not SOC_BLE_SUBRATE_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_SUBRATE_SUPPORTED

            |experimental|
        .. only:: not SOC_BLE_SUBRATE_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_SUBRATE_SUPPORTED

            |experimental|
        .. only:: not SOC_BLE_SUBRATE_SUPPORTED

            |unsupported|
    * -
      - LE Channel Classification
      - .. only:: SOC_ESP_NIMBLE_CONTROLLER and not esp32c2

            |experimental|
        .. only:: not SOC_ESP_NIMBLE_CONTROLLER or esp32c2

            |unsupported|
      - .. only:: SOC_ESP_NIMBLE_CONTROLLER and not esp32c2

            |experimental|
        .. only:: not SOC_ESP_NIMBLE_CONTROLLER or esp32c2

            |unsupported|
      - .. only:: SOC_ESP_NIMBLE_CONTROLLER and not esp32c2

            |experimental|
        .. only:: not SOC_ESP_NIMBLE_CONTROLLER or esp32c2

            |unsupported|
    * - .. centered:: |5.4|
      - Advertising Coding Selection
      - .. only:: (SOC_ESP_NIMBLE_CONTROLLER or esp32c3 or esp32s3) and not esp32c2

            |experimental|
        .. only:: not ((SOC_ESP_NIMBLE_CONTROLLER or esp32c3 or esp32s3) and not esp32c2)

            |unsupported|
      - .. only:: (SOC_ESP_NIMBLE_CONTROLLER or esp32c3 or esp32s3) and not esp32c2

            |experimental|
        .. only:: not ((SOC_ESP_NIMBLE_CONTROLLER or esp32c3 or esp32s3) and not esp32c2)

            |unsupported|
      - .. only:: (SOC_ESP_NIMBLE_CONTROLLER or esp32c3 or esp32s3) and not esp32c2

            |experimental|
        .. only:: not ((SOC_ESP_NIMBLE_CONTROLLER or esp32c3 or esp32s3) and not esp32c2)

            |unsupported|
    * -
      - Encrypted Advertising Data
      - |NA|
      - |experimental|
      - |experimental|
    * -
      - LE GATT Security Levels Characteristic
      - |NA|
      - |unsupported|
      - |experimental|
    * -
      - Periodic Advertising with Responses
      - .. only:: SOC_BLE_PERIODIC_ADV_WITH_RESPONSE

            |experimental|
        .. only:: not SOC_BLE_PERIODIC_ADV_WITH_RESPONSE

            |unsupported|
      - .. only:: SOC_BLE_PERIODIC_ADV_WITH_RESPONSE

            |experimental|
        .. only:: not SOC_BLE_PERIODIC_ADV_WITH_RESPONSE

            |unsupported|
      - .. only:: SOC_BLE_PERIODIC_ADV_WITH_RESPONSE

            |experimental|
        .. only:: not SOC_BLE_PERIODIC_ADV_WITH_RESPONSE

            |unsupported|
    * - .. centered:: |6.0|
      - Channel Sounding
      - |unsupported|
      - |unsupported|
      - |unsupported|
    * -
      - LL Extended Feature Set
      - .. only:: SOC_ESP_NIMBLE_CONTROLLER and not esp32c2

            |developing202612|
        .. only:: not SOC_ESP_NIMBLE_CONTROLLER or esp32c2

            |unsupported|
      - .. only:: SOC_ESP_NIMBLE_CONTROLLER and not esp32c2

            |developing202612|
        .. only:: not SOC_ESP_NIMBLE_CONTROLLER or esp32c2

            |unsupported|
      - .. only:: SOC_ESP_NIMBLE_CONTROLLER and not esp32c2

            |developing202612|
        .. only:: not SOC_ESP_NIMBLE_CONTROLLER or esp32c2

            |unsupported|
    * -
      - Decision-Based Advertising Filtering
      - |unsupported|
      - |unsupported|
      - |unsupported|
    * -
      - Enhancements for ISOAL
      - .. only:: SOC_BLE_ISO_SUPPORTED

            |experimental|
        .. only:: not SOC_BLE_ISO_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_ISO_SUPPORTED

            |experimental|
        .. only:: not SOC_BLE_ISO_SUPPORTED

            |unsupported|
      - .. only:: SOC_BLE_ISO_SUPPORTED

            |experimental|
        .. only:: not SOC_BLE_ISO_SUPPORTED

            |unsupported|
    * -
      - Monitoring Advertisers
      - .. only:: SOC_ESP_NIMBLE_CONTROLLER and not esp32c2

            |developing202612|
        .. only:: not SOC_ESP_NIMBLE_CONTROLLER or esp32c2

            |unsupported|
      - .. only:: SOC_ESP_NIMBLE_CONTROLLER and not esp32c2

            |developing202612|
        .. only:: not SOC_ESP_NIMBLE_CONTROLLER or esp32c2

            |unsupported|
      - .. only:: SOC_ESP_NIMBLE_CONTROLLER and not esp32c2

            |developing202612|
        .. only:: not SOC_ESP_NIMBLE_CONTROLLER or esp32c2

            |unsupported|
    * -
      - Frame Space Update
      - |unsupported|
      - |unsupported|
      - |unsupported|
    * - .. centered:: |6.1|
      - Randomized Resolvable Private Address (RPA) Updates
      - |unsupported|
      - |unsupported|
      - |unsupported|
    * - .. centered:: |6.2|
      - HCI USB LE Isochronous Support
      - |unsupported|
      - |unsupported|
      - |unsupported|
    * -
      - LE Test Mode Enhancements
      - |unsupported|
      - |unsupported|
      - |unsupported|
    * -
      - Shorter Connection Intervals
      - |unsupported|
      - |unsupported|
      - |unsupported|
    * -
      - Channel Sounding Amplitude-based Attack Resilience
      - |unsupported|
      - |unsupported|
      - |unsupported|
    * - .. centered:: |6.3|
      - Channel Sounding Inline Phase Correction Term Transfer
      - |unsupported|
      - |unsupported|
      - |unsupported|
    * -
      - Channel Sounding PHY-specific RTT Accuracy
      - |unsupported|
      - |unsupported|
      - |unsupported|
    * -
      - Running Out of Bits
      - |unsupported|
      - |unsupported|
      - |unsupported|

.. [1]
   如果想了解支持功能的 Bluetooth SIG 认证状态，
   请查阅 `SIG 蓝牙产品数据库 <https://qualification.bluetooth.com/Listings/Search>`__ 。

.. only:: esp32h2

    .. [2]
       注意： 在 ESP32-H2 上，当低功耗蓝牙连接启用加密时，不支持面向连接的低功耗蓝牙测向（AoA/AoD）功能。

对于大部分开发需要在控制器 (Controller) 完成的功能，其主机层 (Host) 的支持状态将会受限于控制器层的支持状态。
如果你计划将乐鑫低功耗蓝牙控制器和主机跑在不同的乐鑫芯片上，则主机的功能将不再受限于这颗跑主机的芯片上的控制器的功能支持状态，
请参阅 :doc:`ESP 主机主要功能支持状态 <host-feature-support-status>` 。

请注意，本文档不构成对客户的约束性承诺。
以上所列出来的功能支持状态信息仅供参考，可能会在不通知的情况下发生更改。
建议联系 `乐鑫客户支持团队 <https://www.espressif.com/zh-hans/contact-us/sales-questions>`__ 以获取最新信息，并确认功能是否适合你的特定需求。


.. |supported| image:: ../../../_static/ble/feature_status/supported.svg
   :class: align-center
   :width: 65px
.. |developing202512| image:: ../../../_static/ble/feature_status/developing202512.svg
   :class: align-center
   :width: 120px
.. |developing202603| image:: ../../../_static/ble/feature_status/developing202603.svg
   :class: align-center
   :width: 125px
.. |developing202612| image:: ../../../_static/ble/feature_status/developing202612.svg
   :class: align-center
   :width: 125px
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
.. |6.0| replace:: `6.0 <https://www.bluetooth.com/specifications/specs/core-specification-6-0/>`__
.. |6.1| replace:: `6.1 <https://www.bluetooth.com/specifications/specs/core-specification-6-1/>`__
.. |6.2| replace:: `6.2 <https://www.bluetooth.com/specifications/specs/core-specification-6-2/>`__
.. |6.3| replace:: `6.3 <https://www.bluetooth.com/specifications/specs/core-specification-6-3/>`__
