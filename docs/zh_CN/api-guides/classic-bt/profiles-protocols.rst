规范和协议
===========

:link_to_translation:`en:[English]`

协议 (Protocol) 定义了完成特定功能的消息格式和过程，例如数据传输、链路控制、安全服务和服务信息交换等。另一方面，蓝牙规范 (Profile) 则定义了蓝牙系统中从 PHY 到 L2CAP 及核心规范外的其他协议所需的功能和特性。

以下是 ESP-Bluedroid 支持的经典蓝牙协议和规范：

- 协议：L2CAP、SDP、AVDTP、AVCTP、RFCOMM
- 规范：GAP、A2DP、AVRCP、SPP、HFP

协议模型如图 :ref:`bluetooth-protocol-model` 所示。

.. _bluetooth-protocol-model:

.. figure:: ../../../_static/bluetooth-protocol-model.png
    :align: center
    :width: 60%
    :alt: 蓝牙协议模型

    蓝牙协议模型

如图 :ref:`bluetooth-protocol-model` 所示，L2CAP 和 SDP 是经典蓝牙最小主机协议栈的必备组成部分，AVDTP、AV/C 和 AVCTP 并不属于核心规范，仅用于特定规范。


L2CAP
--------

蓝牙逻辑链路控制和适配协议 (Logical Link Control and Adaptation Protocol, L2CAP) 是 OSI 2 层协议，支持上层的协议复用、分段和重组及服务质量信息的传递。L2CAP 可以让不同的应用程序共享一条 ACL-U 逻辑链路。应用程序和服务协议可通过一个面向信道的接口，与 L2CAP 进行交互，从而与其他设备上的等效实体进行连接。

L2CAP 信道共支持 6 种模式，可通过 L2CAP 信道配置过程进行选择，不同模式的应用场合不同，主要差别在于可提供的 QoS 不同。这些模式分别是：

- 基本 L2CAP 模式
- 流量控制模式
- 重传模式
- 加强重传模式
- 流模式
- 基于 LE Credit 的流量控制模式

其中，ACL-U 逻辑链路支持的操作模式包括基本 L2CAP 模式、加强重传模式和流模式。L2CAP 信道为支持的固定信道，也支持帧校验序列 (Frame Check Sequence, FCS)。


SDP
--------

服务发现协议 (Service Discovery Protocol, SDP) 允许应用程序发现其他对等蓝牙设备提供的服务，并确定可用服务的特征。SDP 包含 SDP 服务器和 SDP 客户端之间的通信。服务器维护一个描述服务特性的服务记录表。客户端可通过发出 SDP 请求，从服务器维护的服务记录表中进行信息检索。


GAP
--------

通用访问规范 (Generic Access Profile, GAP) 提供有关设备可发现性、可连接性和安全性的模式和过程描述。


A2DP 和 AVRCP
----------------

高级音频分发规范 (Advanced Audio Distribution Profile, A2DP) 定义了在 ACL 信道上，实现高质量单声道或立体声音频内容传输的协议和过程。A2DP 负责处理音频流，通常与音频／视频远程控制规范 (Audio/Video Remote Control Profile, AVRCP)（包括音频／视频控制功能）一起使用。图 :ref:`profile-dependencies` 描述了这些规范的结构和关系图：


.. _profile-dependencies:

.. figure:: ../../../_static/profile-dependencies.png
    :align: center
    :width: 50%
    :alt: 规范关系图

    规范关系图


如图 :ref:`profile-dependencies` 所示，A2DP 的工作基于 GAP 以及通用音频/ 视频分发规范 (Generic Audio/Video Distribution Profile, GAVDP)，定义了建立音频／视频流的过程。

A2DP 中共定义了两个角色：Source (SRC) 和 Sink (SNK)。SRC 代表数字音频流的源端，SNK 代表数字音频流的接收端。

AVRCP 中共定义了两个角色：控制器 (CT) 和目标 (TG)。控制器可通过向目标发送命令帧，发起事务。控制器的常见例子包括个人电脑、PDA 和移动电话等。目标可接收控制器发送的命令帧，并生成相应的响应帧。目标的常见例子包括音频播放器或耳机。

在目前的 A2DP 解决方案中，SBC 是唯一支持的音频编解码器，SBC 在 A2DP 规范中是必须的编解码格式。目前方案中实现的规范、协议版本是 A2DP V1.4 和 AVDTP V1.3。

AVDTP 协议定义了蓝牙设备之间在 L2CAP 协议层上建立和传输媒体流的二进制业务。作为 A2DP 的基本传输协议，AVDTP 建立在 L2CAP 层协议之上，由 “一个协商媒体流参数的信令传输实体” 和 “一个传输媒体流的实体” 组成。

A2DP 规范规定了 AVDTP 传输功能的基本服务。根据当前服务功能的配置，基本服务功能中提供了媒体传输和媒体编解码器。

AVRCP 定义了支持音频／ 视频遥控的应用场景的各项需求。AVRCP 中的命令主要分为三个大组：

- **AV/C 数字接口命令集:** 其特定命令子集被采用，且通过 AVCTP 协议传输。
- **浏览命令:** 可通过 AVCTP 浏览信道，提供浏览功能。
- **封面艺术命令:** 用于传输与媒体项目有关的图像，通过基于 OBEX 协议的蓝牙基本图像规范 (BIP) 实现。

AVRCP 使用了其中的两套 AV/C 命令：其一包括 AV/C 规范中定义的 PASS THROUGH、UNIT INFO 和 SUBUNIT INFO 命令；其二是 AVRCP 专用 AV/C 命令，作为对 Bluetooth SIG Vendor Dependent 的扩展。

AV/C 命令通过 AVCTP 控制信道发送。PASS THROUGH 命令可通过控制器上的按钮，向面板子单元传送用户操作，并提供一个简单的通用机制来控制目标。例如，PASS THROUGH 中的操作 ID 包括播放、暂停、停止、调高音量和调低音量等常用指令。

为了保证互操作性，AVRCP 将 A/V 功能分为四类：

- 播放机/ 录像机
- 监控器/ 放大器
- 调音器
- 菜单

目前的方案提供了 AVRCP V1.6 和 AVCTP V1.4。AVRCP 支持功能的默认配置属于第二类，即监视器/放大器。此外，方案还提供了用于发送 PASS THROUGH 命令的 API。

A2DP 和 AVRCP 经常一起使用。在目前的解决方案中，下层主机堆栈实现了 AVDTP 和 AVCTP 逻辑，并独立为 A2DP 和 AVRCP 提供接口。然而，在堆栈上层中，两个规范组合匹配成为 “AV” 模块。例如，BTA 层提供一个统一的 “AV” 接口，而在 BTC 层中，状态机将处理两种规范的事务。然而，A2DP 和 AVRCP 的 API 是分别提供的。
