功能支持状态
============

:link_to_translation:`en:[English]`

本页跟踪 ESP-IDF 中蓝牙低功耗音频功能的支持状态 —— ESP-BLE-AUDIO 提供的通用音频框架 (GAF) 规范和服务，以及 ESP-BLE-ISO 提供的等时传输。

.. note::

   下表中标为支持的蓝牙低功耗音频功能目前为\ **预览版**\ ：其 API 和行为均为暂定，可能在未来版本中变化。

下表列出了 ESP-IDF 中当前支持的蓝牙低功耗音频规范和服务。

.. list-table::
    :header-rows: 1
    :widths: 28 12 60

    * - 规范 / 服务
      - 支持状态
      - 说明
    * - LE 等时通道 (CIS / BIS)
      - 支持
      - 通过 :doc:`ESP-BLE-ISO <../../api-reference/bluetooth/esp-ble-iso>` 直接访问 ISO。
    * - BAP
      - 支持
      - 全部六个 BAP 角色：单播客户端、单播服务器、广播源、广播接收端、广播助手、扫描委托设备。
    * - PACS
      - 支持
      - 用于 BAP 单播服务器和广播接收端。
    * - ASCS
      - 支持
      - 用于 BAP 单播服务器。
    * - BASS
      - 支持
      - 用于 BAP 扫描委托设备和广播助手。
    * - CAP
      - 支持
      - 全部三个 CAP 角色：接受端、发起端、指挥端。
    * - CAS
      - 支持
      - 每个 CAP 接受端上的必选服务。
    * - CSIP / CSIS
      - 支持
      - 集成员和集协调器角色。
    * - VCP / VCS
      - 支持
      - 音量渲染器和音量控制器角色。
    * - VOCS
      - 支持
      - 每路输出的音量偏移控制；作为可选子服务包含在 VCS 中。
    * - AICS
      - 支持
      - 音频输入控制；作为可选子服务包含在 VCS 和 MICS 中。
    * - MICP / MICS
      - 支持
      - 麦克风设备和麦克风控制器角色。
    * - MCP / MCS
      - 部分支持
      - 媒体控制服务器和媒体控制客户端角色已支持。基于 OTP/OTS 的媒体对象传输暂不支持。
    * - CCP / TBS
      - 支持
      - 通话控制服务器和通话控制客户端角色，包括 GTBS 和 TBS。
    * - HAP / HAS
      - 支持
      - 助听器和助听器单播客户端角色，包括通过 HAS 进行预设读写。
    * - TMAP / TMAS
      - 支持
      - 全部六个 TMAP 角色：CG、CT、UMS、UMR、BMS、BMR。
    * - GMAP / GMAS
      - 支持
      - 全部四个 GMAP 角色：UGG、UGT、BGS、BGR。
    * - PBP
      - 支持
      - 公共广播源和公共广播接收端角色。
    * - OTP / OTS
      - 不支持
      - 对象传输规范/服务（MCP/MCS 用于媒体对象传输）暂不支持。

.. note::

   规范和服务的定义参见 :doc:`蓝牙低功耗音频标准 <ble-audio-introduction>`。

   通用蓝牙低功耗功能支持参见 :doc:`主要功能支持状态 <../ble/ble-feature-support-status>`。
