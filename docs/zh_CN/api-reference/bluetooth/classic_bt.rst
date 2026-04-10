经典蓝牙\ :sup:`®`
===============================

:link_to_translation:`en:[English]`

**经典蓝牙 API** 提供用于实现传统蓝牙功能的 API，包括音频流传输、设备通信，以及通过串口配置文件进行数据交换。它支持多种蓝牙配置文件，使 ESP 设备能够在无线音频、远程控制和数据传输等场景中同时扮演源设备或接收设备的角色。

经典蓝牙 API 提供以下主要功能：

- 核心协议支持 （**GAP**、**L2CAP** 和 **SDP**）
- 串行数据通信 (**SPP**)
- 高质量音频流传输 (**A2DP**)
- 媒体播放控制 (**AVRCP**)
- 免提通话支持 (**HFP**)
- 输入设备连接 （**HID** 主机和外设角色）

----

经典蓝牙 API 在 ESP-IDF 中分为以下部分：

**核心协议**

- :doc:`Bluetooth GAP <esp_gap_bt>`

  **通用访问配置文件 (GAP)：** 设备发现、配对及安全管理

- :doc:`Bluetooth L2CAP <esp_l2cap_bt>`

  **逻辑链路控制与适配协议 (L2CAP)：** 数据多路复用和通道管理

- :doc:`Bluetooth SDP <esp_sdp>`

  **服务发现协议 (SDP)：** 发现远程设备的服务和属性

**通信配置文件**

- :doc:`Bluetooth SPP <esp_spp>`

  **串口配置文件 (SPP)：** 通过蓝牙模拟串行通信通道进行数据交换

**音频与媒体配置文件**

- :doc:`Bluetooth A2DP <esp_a2dp>`

  **高级音频分发配置文件 (A2DP)：** 高质量音频流传输（源和接收端）

- :doc:`Bluetooth AVRCP <esp_avrc>`

  **音视频远程控制配置文件 (AVRCP)：** 媒体播放控制（播放、暂停、音量调节）

**免提配置文件 (HFP)**

- :doc:`Bluetooth HFP Define <esp_hf_defs>`：HFP 角色共享的核心定义
- :doc:`Bluetooth HFP Client <esp_hf_client>`：实现免提单元角色（例如耳机、车载套件）
- :doc:`Bluetooth HFP AG <esp_hf_ag>`：实现音频网关角色（例如手机）

**人机接口设备 (HID)**

- :doc:`Bluetooth HID Device <esp_hidd>`：实现外设角色，如键盘、鼠标或游戏手柄
- :doc:`Bluetooth HID Host <esp_hidh>`：实现主机角色，用于连接远程 HID 外设

每个部分通常包含 **概述**、**应用示例** 和 **API 参考**，涵盖用途、主要功能、示例用法及详细 API 文档。


.. toctree::
   :maxdepth: 1
   :hidden:

   Bluetooth GAP <esp_gap_bt>
   Bluetooth L2CAP <esp_l2cap_bt>
   Bluetooth SDP <esp_sdp>
   Bluetooth SPP <esp_spp>
   Bluetooth A2DP <esp_a2dp>
   Bluetooth AVRCP <esp_avrc>
   Bluetooth HFP Define <esp_hf_defs>
   Bluetooth HFP Client <esp_hf_client>
   Bluetooth HFP AG <esp_hf_ag>
   Bluetooth HID Device <esp_hidd>
   Bluetooth HID Host <esp_hidh>
