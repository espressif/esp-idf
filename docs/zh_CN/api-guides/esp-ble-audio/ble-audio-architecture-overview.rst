.. _ble-audio-architecture:

ESP-IDF 蓝牙 LE Audio 架构
==============================

:link_to_translation:`en:[English]`

本文档描述 ESP-IDF 蓝牙 LE Audio 协议栈，以及它所基于的通用 BLE 传输层的内部架构，涉及两个蓝牙组件：

- **ESP-BLE-ISO** —— 通用的 BLE 传输层。它提供 GATT、GAP、ISO、L2CAP 和 HCI 原语，以及用于串行化所有主机事件的专用处理任务和锁模型。它\ **并非蓝牙 LE Audio 专用** —— 它是一个自包含的传输层，任何上层规范都可以构建于其上，也可以\ **独立运行**：它拥有自己的公共 API（``esp_ble_iso_*``）和自己的初始化流程，因此应用程序可以直接使用它，而无需 ESP-BLE-AUDIO 或其上的任何规范层。
- **ESP-BLE-AUDIO** —— 上层规范层。它实现蓝牙 LE Audio 的各项规范和服务（PACS、ASCS、BASS、BAP、CAP、VCP、MICP、CSIP、MCP、CCP、TMAP 等），并通过公共 ``esp_ble_audio_*`` API 对外暴露。

.. note::

   ESP-BLE-AUDIO 目前是 ESP-BLE-ISO 的唯一使用方，但并非唯一的预期使用方，而且它完全不是必需的。

   ESP-BLE-ISO 是一个独立组件：它可以单独构建和初始化，并通过其公共 ``esp_ble_iso_*`` API 驱动，用于纯 ISO 场景（CIS 或 BIS，不带任何规范层）—— 参见 :ref:`ESP-BLE-ISO 公共 API <arch-iso-api>`。

   该传输层也与规范无关：其 GATT、GAP、ISO、L2CAP 和 HCI 原语都是通用的 BLE 构建块，其他规范 —— 例如未来的 HID-over-ISO —— 预期也会构建在同一组件之上。本文档中凡描述 ESP-BLE-ISO 之处，其行为均适用于任何使用方，而不仅限于蓝牙 LE Audio。

这两个组件都可运行在 **Bluedroid** 或 **NimBLE** 两种 BLE 主机协议栈之一之上，并隐藏在一个公共接口之后。明确揭示两种主机之间的行为差异是本文档的首要目标之一，因为它们决定了回调运行所处的任务上下文、连接建立期间的操作顺序，以及事件的分发方式。

.. note::

   本指南面向两类读者。概览部分给出层级层面的思维模型，适用于任何集成两个组件之一公共 API 的开发者（分别参见 :ref:`ESP-BLE-ISO <arch-iso-api>` 与 :ref:`ESP-BLE-AUDIO <arch-audio-api>`）。后续章节则深入到实现层面（任务、队列、锁、各主机的分发路径），面向维护组件本身的开发者。源码以文件路径和符号名引用，而非行号，因此这些引用会随代码演进而保持有效。

.. |br| raw:: html

   <br/>

.. contents:: 目录
   :local:
   :depth: 1

架构概览
----------------

分层结构
~~~~~~~~~~~~~~~~

ESP-IDF 蓝牙 LE Audio 被组织为一组相互协作的分层。每一层只与其正下方的一层通信，所有与主机相关的知识都被限制在每个组件内部的适配器子层中。

.. mermaid::

   flowchart TB
       APP["应用 / 示例（app 任务）"]
       AUDIO["ESP-BLE-AUDIO —<br/>蓝牙 LE Audio 规范与服务<br/>(esp_ble_audio_* API)"]
       ISO["ESP-BLE-ISO — 传输原语：<br/>GATT、GAP、ISO、L2CAP、HCI<br/>全部由全局 ISO 锁串行化 —<br/>多数事件运行在 ISO 任务事件循环上"]
       HOST["BLE 主机协议栈<br/>(Bluedroid 或 NimBLE)"]
       CTRL["BLE 控制器"]
       APP <--> AUDIO
       AUDIO <--> ISO
       ISO <--> HOST
       HOST <--> CTRL
       ISO <-. ISO 数据通路 .-> CTRL

.. note::

   实线链路是控制路径，它在两个方向上都遵循分层结构 —— 命令向下流动，事件向上流动。\ **ISO 数据通路**\ 是例外（虚线链路）：在核心规范定义的两个数据通路方向上，它都绕过了主机协议栈。在\ **输入**\ 数据通路（主机到控制器）上，ESP-BLE-ISO 将 SDU 直接写入控制器传输接口，控制器再通过回调将发送完成报告回 ISO 任务；在\ **输出**\ 数据通路（控制器到主机）上，控制器通过同类回调递交接收到的 ISO 数据。两条路径都不经过主机协议栈的 HCI 事件分发，从而使高速率的音频数据通路保持简短。

应用程序只调用公共 ``esp_ble_audio_*`` API（对于纯 ISO 用例则调用 ``esp_ble_iso_*``）。公共 API 之下的一切都是内部实现，可能在不同版本之间发生变化。

两个组件
~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 16 40 44

   * - 组件
     - 职责
     - 关键目录
   * - ESP-BLE-ISO
     - - ACL 连接管理
       - 广播与扫描
       - GATT 客户端与服务端
       - ISO（CIS 和 BIS）
       - HCI 和 L2CAP
       - ISO 任务事件循环
       - ``esp_ble_iso_*`` API
     - - ``host/common``
       - ``host/adapter/bluedroid``
       - ``host/adapter/nimble``
       - ``api/include``
   * - ESP-BLE-AUDIO
     - - 蓝牙 LE Audio GATT 服务
       - 规范客户端与控制器
       - 对象传输服务（OTS）
       - ``esp_ble_audio_*`` API
     - - ``host/common``
       - ``host/adapter/bluedroid/profiles``
       - ``host/adapter/nimble/profiles``
       - ``host/services/ots``
       - ``api/include``

.. _arch-dual-host:

双主机设计
----------------

每个组件都被拆分为\ **与主机无关的通用**\ 部分和\ **主机相关的适配器**\ 部分。``host/common`` 下的目录持有一份不依赖主机协议栈的实现，而 ``host/adapter/bluedroid`` 和 ``host/adapter/nimble`` 则持有直接与各主机交互的代码。最终只会编译其中一个适配器，由当前激活的主机选择（``CONFIG_BT_BLUEDROID_ENABLED`` 或 ``CONFIG_BT_NIMBLE_ENABLED``）。通用层定义了两个适配器都需实现的内部接口，因此上层永远无需针对主机协议栈做分支判断。

这两种主机在行为上并不完全一致。它们在回调运行所处的任务、入站事件如何到达 ISO 任务，以及若干流程级细节（MTU 交换、服务发现、ISO 建立）上都有差异，这些将在各分层章节中介绍。结构性差异总结如下；其中对应用开发者最重要的一项 —— 回调运行所处的任务 —— 在 :ref:`回调执行上下文 <arch-callback-context>` 中详述。

.. list-table:: 主机层面的差异
   :header-rows: 1
   :widths: 26 37 37

   * - 方面
     - Bluedroid
     - NimBLE
   * - 引发入站事件的任务
     - BTU 任务（BTA GATT 和 GAP 回调在此运行）。
     - NimBLE 主机任务。
   * - 适配器如何转发事件
     - BTA 回调构造一个事件并将其投递到 ISO 任务队列。
     - ``*_cb_safe`` 包装函数先获取 ISO 锁；随后大多数事件被投递到 ISO 任务队列。
   * - 同步例外
     - GATT 服务端的属性访问以异步方式完成：请求被投递到 ISO 任务，响应稍后发送。
     - GATT 服务端的属性访问在主机任务上原地（inline）完成，因为 NimBLE 要求同步返回属性值。
   * - ISO 任务所在 CPU 核
     - 绑定到所配置的 Bluedroid 核。
     - 绑定到所配置的 NimBLE 核。
   * - ISO 任务优先级
     - 与 BTU 任务优先级保持一致。
     - 与 NimBLE 主机任务优先级保持一致。

.. note::

   ISO 任务被特意赋予与主机协议栈主任务相同的 CPU 核和相同的优先级（参见 ``host/common/include/common/task.h``）。让传输任务运行在与主机相同的优先级上，可使二者协作式调度，而非相互抢占。

组件内部
----------------

各组件的实现细节见下表，涵盖任务、队列、锁以及各主机的分发路径，面向在组件内部工作的维护者：

.. list-table::
   :header-rows: 1
   :widths: 26 74

   * - 页面
     - 涵盖内容
   * - :ref:`ESP-BLE-ISO <arch-iso-transport>`
     - - 并发模型 —— 单一的 ISO 任务与全局锁，以及每个回调运行所处的任务
       - 连接管理
       - 广播、扫描与周期性广播同步
       - HCI 命令路径
       - ISO 子系统 —— CIS 和 BIG 状态机，以及 SDU 数据通路
       - GATT 客户端与服务端，以及 ATT MTU 交换
       - L2CAP（草案）与应用事件接口
       - ``esp_ble_iso_*`` 公共 API
   * - :ref:`ESP-BLE-AUDIO <arch-audio>`
     - - 开源规范适配器与预构建库
       - GATT 服务与服务适配器模式
       - 规范客户端与控制器
       - 对象传输（草案）
       - 初始化流程与 ATT-MTU 取值
       - 端到端单播与广播流程
       - ``esp_ble_audio_*`` 公共 API

.. toctree::
   :hidden:
   :maxdepth: 1

   ble-audio-architecture-iso
   ble-audio-architecture-lea

横切关注点
----------------

本节汇集跨越两个组件、并在整个实现中反复出现的不变式和实际关注点。

主机对等
~~~~~~~~~~~~~~~~

最重要的维护不变式是：两个主机适配器在行为上保持等价。它们之上的通用层假定行为完全相同，因此对一个适配器的连接、清理、发现或事件路径的改动，必须在另一个上镜像 —— 否则上层会因主机不同而表现不同。在两者确实无法匹配之处，这种不对称是有意为之的，且其完整集合很小：

.. list-table:: 有意的各主机差异
   :header-rows: 1
   :widths: 30 35 35

   * - 方面
     - Bluedroid
     - NimBLE
   * - 引发入站事件的任务
     - BTU 任务
     - NimBLE 主机任务
   * - GATT 服务端属性访问
     - 投递到 ISO 任务；响应通过 BTA 异步发送
     - 在主机任务上原地；值同步返回
   * - GATT 客户端读/写完成
     - 投递到 ISO 任务
     - 在主机任务上原地
   * - HCI ISO 命令
     - 私有的直连 HCI 路径，带自己的完成信号量
     - 类型化的 ``ble_hs_hci_*`` 辅助函数
   * - GATT 服务注册
     - 两阶段且异步（``*_init`` 然后 ``*_start``）
     - 单阶段 ``ble_gatts_add_svcs``
   * - L2CAP 面向连接的通道
     - 未实现（TODO）
     - 已实现（草案）
   * - 对象传输服务（OTS）
     - 未实现（构建在 L2CAP 之上）
     - 已实现（草案）
   * - 连接发起的事件路由
     - 共享引擎的 BTA GATTC 接口（``esp_ble_iso_bluedroid_get_gattc_if``）
     - 把 GAP 事件转发进引擎（``*_gap_app_post_event``）

超出这些范围的任何差异都需要同样的论证，并记录在引入它的地方。

层边界
~~~~~~~~~~~~~~~~

该架构依赖几条调用者和维护者都应尊重的边界：

- **公共 API 是唯一稳定的接口面**。``esp_ble_iso_*`` 和 ``esp_ble_audio_*`` API 之下的一切都是内部实现，可能在不同版本之间改变。
- **两种 BLE 主机协议栈是被适配，而非被修改**。适配器层的存在正是为了让组件永不改动 Bluedroid 或 NimBLE 本身；主机行为被视为既定。
- **规范与控制器逻辑是一个具有固定 ABI 的预构建库**。开源代码只通过在初始化时校验的共享函数表和配置表与它交互；它的内部实现不属于这个契约。

常见陷阱
~~~~~~~~~~~~~~~~

:ref:`并发与线程安全 <arch-concurrency>` 中的并发模型有一些容易弄错的实际后果：

- **不要在回调中阻塞**。大多数回调运行在 ISO 任务上；在那里阻塞会拖住协议栈中所有其他事件。它的栈只有 4 KB，因此必须避免深层调用链和大的栈上分配缓冲区 —— 包括根据配置值确定大小的数组。
- **已投递的事件之间不会相互竞争**。由于单个任务排空 ISO 任务的各队列，两个都已投递的事件不可能并发运行；假定它们可以的推断几乎总是错误的。
- **在 NimBLE 上要注意回调所在的任务**。一次 GATT 读或写完成、或对本地服务的一次入站写，运行在 NimBLE 主机任务上而非 ISO 任务；在不同路径间共享的代码必须在两者上都正确。
- **ISO 锁排序的是访问，而非完成**。持有它保证互斥，但不保证你发起的某个操作已经完成 —— 许多传输操作在稍后的事件上才完成。

内存占用
~~~~~~~~~~~~~~~~

开源的 ESP-BLE-ISO 和 ESP-BLE-AUDIO 代码有小而确定的固定开销：

- ISO 任务使用一个 4 KB 栈和三条优先级事件队列（32 + 64 + 32 项），由单个消费者排空。
- ISO 引擎的通道、组和广播池由 ``CONFIG_BT_ISO_MAX_CHAN``、``CONFIG_BT_ISO_MAX_CIG`` 和 ``CONFIG_BT_ISO_MAX_BIG`` 确定大小；HCI 命令池只持有一个缓冲区。
- 每条连接上，NimBLE 缓存已发现的属性数据库，并为在途的 GATT 流程持有深拷贝的参数；订阅在两种主机上都用按连接的列表跟踪。
- GATT 服务属性表在初始化时分配，大小由已启用的服务决定。

由于这些池和表的大小都来自 Kconfig，开源代码的占用在构建时就固定了，除了在途的 SDU 缓冲区之外没有按流的动态增长。预构建的规范与控制器库是例外：它动态分配自己的规范和控制器状态 —— 在初始化时以及在连接和流建立时 —— 因此这部分占用并非在构建时固定。

附录 —— 文件与目录映射
--------------------------------

ESP-BLE-ISO
~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 34 66

   * - 目录
     - 内容
   * - ``api/include``
     - 公共 ``esp_ble_iso_*`` 头文件。
   * - ``host/common``
     - 与主机无关的传输：连接、广播、扫描、GATT、HCI、ISO 粘合、L2CAP、ISO 任务和 ISO 锁；``app/`` 持有应用的 GAP 和 GATT 事件接口。
   * - ``host/iso``
     - ISO 引擎（CIS 和 BIG 状态机及通道 API）。
   * - ``host/adapter/bluedroid``,\ |br|\ ``host/adapter/nimble``
     - GAP、GATT、ISO、HCI 和（NimBLE）L2CAP 的各主机适配器。
   * - ``host/utils``
     - 地址、UUID、CRC、加密、定时器和缓冲区辅助函数。

ESP-BLE-AUDIO
~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 34 66

   * - 目录
     - 内容
   * - ``api/include``
     - 公共 ``esp_ble_audio_*_api.h`` 头文件，每个规范和服务一个。
   * - ``host/common``
     - 初始化编排（``init.c``）。
   * - ``host/adapter/bluedroid/profiles``,\ |br|\ ``host/adapter/nimble/profiles``
     - GATT 服务适配器（PACS、ASCS、BASS、CAS、CSIS、HAS、MCS、MICS、TBS、TMAS、VCS）。
   * - ``host/services/ots``
     - 对象传输服务。
   * - ``lib``
     - 预构建的、按目标芯片的规范与控制器库。
