蓝牙\ :sup:`™` 低功耗音频
==========================

:link_to_translation:`en:[English]`

蓝牙低功耗音频是蓝牙核心规范 5.2 中引入的音频架构。阅读本系列文档前，应注意区分以下两个概念：

- **标准**\ 指蓝牙技术联盟 (Bluetooth SIG) 定义的规范 (Profile)、服务 (Service) 与角色 (Role) 体系，其核心为构建在 LE 等时传输之上的通用音频框架 (GAF)。它与平台无关，详见 :doc:`蓝牙低功耗音频标准 <ble-audio-introduction>`。
- **实现**\ 是在特定平台上实现该标准的软件。ESP-IDF 的实现详见 :doc:`ESP-IDF 蓝牙低功耗音频架构 <ble-audio-architecture-overview>`。

在 ESP-IDF 中，该实现由两个组件提供：

- **ESP-BLE-ISO** 提供等时传输 —— 承载音频的连接式和广播式等时流 (CIS/BIS)。参见其 :doc:`API 参考 <../../api-reference/bluetooth/esp-ble-iso>`。
- **ESP-BLE-AUDIO** 提供通用音频框架 (GAF) 规范和服务，构建在 ESP-BLE-ISO 之上。参见其 :doc:`API 参考 <../../api-reference/bluetooth/esp-ble-audio>`。

这两个组件均可运行于 ESP-IDF 的两个蓝牙主机协议栈 **Bluedroid** 或 **NimBLE** 之上。在可行范围内，两者的 API 与行为保持一致；当行为差异不可避免时，将在架构文档中说明，并解释原因。

如需了解各项功能的支持状态，参见 :doc:`功能支持状态 <ble-audio-feature-support-status>`。

.. toctree::
    :maxdepth: 1

    ble-audio-introduction
    ble-audio-architecture-overview
    ble-audio-feature-support-status
