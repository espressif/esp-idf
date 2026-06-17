.. _arch-iso-transport:

ESP-BLE-ISO
===========

:link_to_translation:`en:[English]`

.. |br| raw:: html

   <br/>

.. contents:: 目录
   :local:
   :depth: 2

ESP-BLE-ISO 提供了上层规范所需的每一个 BLE 传输原语：ACL 连接状态、广播与扫描、周期性广播同步、HCI 命令路径、ISO（CIS 和 BIS）、GATT、GAP 和 L2CAP，以及 :ref:`并发与线程安全 <arch-concurrency>` 中描述的 ISO 任务事件循环和全局锁。这些都不是音频专用的；ESP-IDF 蓝牙 LE Audio 只是目前位于其上的规范层。本章依次介绍整个组件：连接管理、广播与扫描、HCI 命令路径、ISO 子系统、GAP、GATT 和 L2CAP，以及公共 API。

整个组件中反复出现两个约定，值得在此一次性说明：

- ``_safe`` **后缀**。大多数内部操作都成对存在：一个核心函数假定全局 ISO 锁已被持有，以及一个同名、带 ``_safe`` 后缀的包装函数，它获取锁、调用核心函数、再释放锁。公共入口点以及来自 ISO 任务之外的调用者使用 ``_safe`` 变体；已经在锁下运行的代码（ISO 任务处理函数，或另一个核心函数）直接调用裸变体。这就是 :ref:`保护共享状态 <arch-host-lock>` 中那把唯一的全局锁如何被统一应用、而不会意外重入的方式 —— 尽管该互斥锁是递归的，这种拆分仍让每个调用点处的锁边界保持明确。
- ``bt_le_*`` / ``bt_*`` **命名**。以 ``bt_`` 开头的符号是 ``host/common`` 中定义的与主机无关的接口；以 ``bt_le_bluedroid_`` 或 ``bt_le_nimble_`` 开头的符号是仅由一个构建编译的适配器实现。

  - 有一族是例外：``host/common`` 中的 ``hci_le_*`` ISO 事件处理函数（例如 ``hci_le_biginfo_adv_report``）是以它们所解码的 HCI 事件命名的，而非以 ``bt_`` 接口命名，并且它们是内部处理函数，而非公共接口的一部分。

.. _arch-concurrency:

并发与线程安全
------------------------

主机协议栈之上的所有 ESP-IDF 蓝牙 LE Audio 处理都运行在单个任务上 —— **ISO 任务**\ （``iso_task``）—— 并且对所有共享状态的访问都由一把全局递归互斥锁串行化，即 **ISO 锁**\ （``bt_le_host_lock``）。这两种机制共同保证了协议栈的线程安全；理解它们是厘清顺序与竞态问题的关键。

ISO 任务事件循环
~~~~~~~~~~~~~~~~~~~~~~~~

ISO 任务（位于 ``host/common/task.c``）是一个永远循环的单个 FreeRTOS 任务，负责排空协议栈其余部分通过 ``bt_le_iso_task_post()`` 投递给它的工作。每一项携带一个事件类型标签，任务据此将其分发给对应类别的处理函数 —— 定时器、GAP、GATT、ISO HCI、ISO 发送完成或 ISO 接收数据。

投递的工作并非单一队列，而是\ **三个优先级层**\ ，每层是一条独立的 FreeRTOS 队列，合并到任务所阻塞等待的一个队列集（queue set）中。每次被唤醒时，任务都\ **严格按优先级**\ 处理各层 —— 先 critical、再 normal、最后 floodable —— 每轮循环处理一项，并在下一轮重新优先检查 critical 层：

.. list-table:: ISO 任务优先级层
   :header-rows: 1
   :widths: 16 12 22 50

   * - 层
     - 深度
     - 溢出时
     - 事件
   * - **Critical**
     - 32
     - 丢弃最新项
     - ISO 接收数据与发送完成 —— 延迟敏感的数据通路，由控制器任务以非阻塞方式投递。
   * - **Normal**
     - 64
     - 阻塞（绝不丢弃）
     - 定时器、GAP 生命周期、GATT 和 ISO HCI 事件 —— 可靠；生产者阻塞直到有空位。
   * - **Floodable**
     - 32
     - 丢弃最新项
     - 高频、尽力而为的 GAP 报告 —— 扩展广播、周期性广播和 BIGInfo 报告 —— 以非阻塞方式投递。

这种拆分的目的是让 GAP 报告的突发洪峰无法拖延 ISO 数据：critical 层总是最先被排空，而两个非阻塞层在满时丢弃其最新项，而不是阻塞其生产者。事件落入哪一层与运行它的处理函数无关 —— floodable 层上的扩展广播和周期性广播报告，由与 normal 层 GAP 事件相同的 ``bt_le_gap_handle_event`` 分发；floodable 层的 BIGInfo 报告，由与普通 ISO HCI 事件相同的 ``bt_le_iso_handle_hci_event`` 分发。

.. note::

   单个任务排空全部三层，因此事件仍然\ **一次一个地处理、永不重叠**\ ，无论由哪个任务产生 —— 协议栈其余部分所依赖的串行化保证保持不变。各层改变的是\ **顺序**\ ：事件按优先级顺序分发，而非按投递顺序，因此一个高优先级的 ISO 数据事件可以先于更早投递的 GAP 报告被处理。依赖串行化保证的代码仍然正确；假定跨事件类别严格先入先出顺序的代码则不然。

该任务有几个值得记住的特性：

- 它运行在 **4 KB 栈**\ 上。每个上层（包括应用和规范回调）都在这个栈上执行，因此必须避免深层调用链和大的栈上分配缓冲区（例如根据配置值确定大小的数组）。
- 每个入队项携带一个事件类型标签和一个堆上分配的负载；由生产者分配负载，处理函数在处理完后释放它 —— 但在某个非阻塞层已满时被丢弃的项例外，它由生产者释放。
- 它的 CPU 核和优先级与当前激活的主机协议栈保持一致，如 :ref:`双主机设计 <arch-dual-host>` 中所述。

一个可选的\ **分发监视器**\ （``CONFIG_BT_ISO_DISPATCH_MONITOR``，默认关闭）会为任务分发的每个回调计时，并保留按事件类型的统计 —— 计数、最大时长，以及超过 ``CONFIG_BT_ISO_DISPATCH_THRESHOLD_US``\ （默认 2000 微秒，约为 7.5–10 毫秒 SDU 间隔的四分之一）的回调的慢计数。该统计表会周期性（``CONFIG_BT_ISO_DISPATCH_DUMP_PERIOD_S``，默认 10 秒）以及在反初始化时打印，用于发现运行时间长到足以拖延 ISO 数据通路的回调。它会增加每次分发的计时开销并测量挂钟时间，因此仅用于性能分析。

.. _arch-host-lock:

保护共享状态
------------------------

共享状态 —— 连接表、周期性广播同步表、GATT 订阅列表、GATT 服务端配置、ISO 簿记 —— 可以从多个任务访问。协议栈\ **不**\ 使用细粒度的、按结构划分的锁。相反，线程安全依赖于一把粗粒度的\ **全局递归互斥锁** —— 即 **ISO 锁**，``bt_le_host_lock`` / ``bt_le_host_unlock``\ （在 ``host/common/host.c`` 中基于 Zephyr 的 ``k_mutex`` 实现）。

规则简单且处处适用：**任何触碰共享状态的代码路径都必须先获取这把互斥锁。**\ 实践中这意味着：

- **公共 API 函数**\ ，从应用任务调用，在进入时获取锁，在返回前释放。
- **ISO 任务事件处理函数**\ ，在每个处理函数中读取或修改共享状态的部分前后获取锁。
- **NimBLE 同步回调**\ （订阅变更、收到的通知、GATT 服务端属性访问路径），在触碰共享状态前获取锁。
- **定时器回调**\ 同样获取锁。

由于某个变量的每一个读者和写者都持有同一把锁，任何两个任务都不可能并发触碰它。这把互斥锁是\ **递归的**\ ，因此已经持有锁的处理函数可以调用会重新获取它的辅助函数而不会死锁。

如果在一个短的有限时间内无法获取锁，``bt_le_host_lock`` 会调用 ``abort()`` 而非继续执行。锁被持有这么久意味着协议栈已经卡死（死锁或卡住的回调），这是编程错误，而非需要在运行时恢复的状况。这里特意使用 ``abort()`` 而非 ``assert()``，因为 ``assert()`` 在 ``NDEBUG`` 构建中会变成空操作，会让调用者在未持有锁的情况下进入临界区，从而重新引入竞态。解锁路径还会额外验证调用任务是否为当前持有者，从而捕获不配对的解锁。

.. important::

   这里有两种不同的串行化机制在起作用，它们覆盖不同的路径：

   #. **三条优先级队列背后的单个消费者**\ 串行化所有投递到 ISO 任务的事件（源自主机协议栈的 GAP、GATT、ISO 和定时器事件）。已投递的事件一次一个地处理、永不重叠，不过跨层时它们按优先级顺序而非投递顺序分发。
   #. **全局 ISO 锁**\ 串行化其他上下文 —— 直接的应用 API 调用和 NimBLE 同步回调 —— 使其与 ISO 任务处理函数互斥。

   一份假设两个已投递事件并发运行的缺陷报告几乎总是误报：单消费者从根本上排除了这种情况。

.. _arch-callback-context:

回调执行上下文
------------------------

应用注册的回调（GAP 事件处理函数、GATT 属性处理函数、规范事件回调）并不总是运行在同一个任务上，而且在一个重要场景下，这个任务\ **在两种主机之间不同**\ 。了解上下文很重要：运行在 ISO 任务上的回调共享其 4 KB 栈，绝不能阻塞它；而运行在主机任务上的回调，则在主机的属性访问路径内部同步执行。

.. list-table:: 已注册回调运行在哪个任务上
   :header-rows: 1
   :widths: 44 28 28

   * - 回调类别
     - Bluedroid
     - NimBLE
   * - GAP 事件（连接、断开、安全变更、PA 同步）
     - ISO 任务
     - ISO 任务
   * - GATT 客户端通知与发现结果
     - ISO 任务
     - ISO 任务
   * - GATT 客户端读写完成
     - ISO 任务
     - **NimBLE 主机任务**
   * - ISO 事件（连接、数据、发送完成）
     - ISO 任务
     - ISO 任务
   * - 定时器回调
     - ISO 任务
     - ISO 任务
   * - GATT 服务端属性读/写与 CCC 订阅，以及任何由入站写同步驱动的规范回调
     - ISO 任务
     - **NimBLE 主机任务**

有两行将回调放在 NimBLE 主机任务上，而非 ISO 任务。GATT 客户端读写完成落在那里，是因为 NimBLE 通过其主机任务上的完成回调报告流程结果，适配器在 ISO 锁下原地（inline）调用应用回调，而不是重新投递它 —— 相比之下，Bluedroid 会把每个 BTA GATT 事件都投递到 ISO 任务。GATT 服务端属性访问那一行有更鲜明的原因：两种主机以相反的方式完成访问。在 Bluedroid 上，请求被投递到 ISO 任务，已注册的回调在那里运行，而响应随后\ **异步**\ 发送，如下图所示：

.. mermaid::

   sequenceDiagram
       participant P as Peer
       participant BTU as BTU task
       participant T as ISO task
       participant CB as attr read/write callback
       P->>BTU: ATT 读 / 写请求
       BTU->>T: 投递 GATTS 事件
       T->>CB: 调用回调(在 ISO 锁下)
       CB-->>T: 值 / 状态
       T->>BTU: BTA_GATTS_SendRsp(异步)
       BTU-->>P: ATT 响应

在 NimBLE 上则相反，主机要求\ **同步**\ 返回该值，因此回调在主机任务上原地运行，并直接把值交回：

.. mermaid::

   sequenceDiagram
       participant P as Peer
       participant N as NimBLE host task
       participant CB as attr read/write callback
       P->>N: ATT 读 / 写请求
       N->>CB: 原地调用回调(在 ISO 锁下)
       CB-->>N: 值 / 状态
       N-->>P: 同步发送响应

.. _arch-event-flow:

跨层事件流
------------------------

下图追踪一个典型的入站事件 —— 由控制器引发的 GAP、GATT 客户端或 ISO 事件 —— 从控制器一路向上到应用回调，并展示两种主机的差异之处。

.. mermaid::

   %%{init: {'sequence': {'noteAlign': 'left'}}}%%
   sequenceDiagram
       participant C as BLE controller
       participant H as Host stack task
       participant A as ESP-BLE-ISO adapter
       participant T as ISO task
       participant CB as App/Profile callbacks
       C->>H: HCI 事件
       Note over H: Bluedroid:在 HCI 主机任务上接收,回调运行在 BTU<br/>NimBLE:单个主机任务两者都做
       H->>A: 主机回调
       Note over A: Bluedroid:BTA 回调<br/>NimBLE:*_cb_safe(获取 ISO 锁)
       A->>A: 在堆上构建事件负载
       A->>T: bt_le_iso_task_post() — 入队
       T->>T: 出队,获取 ISO 锁
       T->>CB: 在 ISO 任务上下文中调用回调
       Note over C,T: ISO 数据 / 发送完成是例外 —— 控制器直接调用已注册的回调,绕过主机协议栈
       C-->>T: bt_le_iso_task_post() — 入队(直接回调,在控制器任务上)
       T->>T: 出队,获取 ISO 锁
       T->>CB: 在 ISO 任务上下文中的通道回调

应用和规范回调运行在何处由这个流程决定，并且因主机而异：

- 在 **Bluedroid** 上，\ **每个上层回调都在 ISO 任务上下文中运行**\ ，无论由什么触发 —— GAP、GATT 客户端（通知、发现、读或写完成）、GATT 服务端、ISO 或定时器。适配器把每个 BTA 事件都投递到队列，因此没有例外。这些回调在持有 ISO 锁时运行于任务的 4 KB 栈上，从其中调用阻塞式传输 API 会拖住整个事件循环。
- 在 **NimBLE** 上，情况相同，但有两个类别例外，它们\ **原地运行在 NimBLE 主机任务上**\ 而非 ISO 任务：**GATT 服务端属性访问**\ （值必须同步返回）和 **GATT 客户端读写完成**\ （NimBLE 通过主机任务上的完成回调报告结果，适配器直接调用它）。因此，由二者之一驱动的规范回调 —— 例如服务端的控制点写处理函数，或客户端的读或写完成 —— 运行在主机任务上，而非 ISO 任务，且不经过队列。这正是 :ref:`回调执行上下文 <arch-callback-context>` 中标记的那两行。

分层与文件映射
------------------------

该组件被组织为：一个与主机无关的 ``host/common`` 层、两个 ``host/adapter`` 层、``host/iso`` 下的一个独立 ISO 引擎，以及 ``host/utils`` 下的共享辅助代码。与主机无关的文件如下：

.. list-table:: ``host/common`` —— 与主机无关的传输
   :header-rows: 1
   :widths: 24 76

   * - 文件
     - 作用
   * - ``host.c``
     - 初始化与反初始化编排；全局递归锁（``bt_le_host_lock`` / ``bt_le_host_unlock``）。
   * - ``task.c``
     - ISO 任务事件循环、其三条优先级队列和队列集，以及 ``bt_le_iso_task_post()``。
   * - ``conn.c``
     - ACL 连接表和连接事件监听器的分发。
   * - ``adv.c``
     - 扩展广播集的记录管理。
   * - ``scan.c``
     - 扫描、周期性广播同步，以及 BIGInfo 报告。
   * - ``hci.c``
     - 构建 HCI 命令缓冲区并分发给当前激活的适配器。
   * - ``iso.c``
     - 解码 ISO HCI 元事件，并把 ISO 数据通路桥接到引擎。
   * - ``gatt.c``
     - 与主机无关的 GATT 状态（订阅、属性数据库缓存）。
   * - ``l2cap.c``
     - 通用的 L2CAP 通道与服务端分发。
   * - ``app/gap.c``
     - 面向应用的 GAP 入口点。
   * - ``app/gatt.c``
     - 面向应用的 GATT 入口点。

ISO 状态机独立于通用层的其余部分，位于 ``host/iso/iso.c`` 中：它负责 CIG、BIG 和 ISO 通道逻辑，以及支撑它们的、按配置确定大小的静态池。它是 ``host/iso`` 下唯一的文件，在下文 :ref:`ISO 子系统 <arch-iso>` 中详述。

每个适配器目录都为其主机镜像了通用接口：``host/adapter/bluedroid`` 和 ``host/adapter/nimble`` 各自提供 ``gap.c``、``gatt``、``iso.c`` 和主机相关的头文件；``host/utils`` 则持有地址、UUID、CRC、加密、定时器和缓冲区等两层都无需重新实现的辅助代码。

.. note::

   存在两个名为 ``iso.c`` 的文件，扮演不同角色。``host/iso/iso.c`` 是引擎 —— CIS 和 BIG 状态机以及公共 ISO 通道 API。``host/common/iso.c`` 是粘合层 —— 它把原始 HCI ISO 元事件解码到引擎的处理函数，并承载 ISO 数据的收发通路。适配器的 ``iso.c`` 文件把引擎命令翻译成主机原生调用。

连接管理
------------------------

``conn.c`` 负责 ACL 连接表 —— 一个以连接句柄为键的 ``bt_conn`` 对象数组 —— 以及连接事件监听器的注册列表。连接生命周期完全由主机适配器驱动：当控制器报告一个连接事件时，适配器调用某个监听器入口点，后者更新连接表，并把事件分发给上层通过 ``bt_conn_cb_register`` 注册的每一个 ``bt_conn_cb``。

这些监听器入口点构成了与主机无关的接缝：

- ``bt_le_acl_conn_connected_listener`` 和 ``bt_le_acl_conn_disconnected_listener`` 添加和移除表项。
- ``bt_le_acl_conn_security_changed_listener``、``..._identity_resolved_listener``、``..._pairing_completed_listener`` 和 ``..._bond_deleted_listener`` 承载安全与配对绑定事件。

由于适配器通过 ISO 任务投递这些事件（参见 :ref:`双主机设计 <arch-dual-host>`），已注册的 ``bt_conn_cb`` 回调在两种主机上都运行于 ISO 任务上下文 —— 即 :ref:`回调执行上下文 <arch-callback-context>` 中的 GAP 事件那一行。作用于活动连接的操作（``bt_conn_set_security``、``bt_conn_disconnect``、``bt_conn_get_info``）和查找辅助函数（``bt_conn_lookup_handle``、``bt_le_acl_conn_find``、``bt_conn_foreach``）都在 ISO 锁下从同一张表读取。这些事件的主机相关来源 —— 适配器调用监听器时所在的主机任务，以及它如何映射原生事件结构 —— 在下文 :ref:`应用事件接口 <arch-app-event>` 中介绍。

广播与扫描
------------------------

``adv.c`` 被刻意保持得很小：它按句柄跟踪扩展广播集（``bt_le_ext_adv_find`` / ``bt_le_ext_adv_new_safe`` / ``bt_le_ext_adv_delete_safe``）。广播集是广播 ISO 组所附着的锚点，因此这张表是 ISO 子系统中 BIG 广播端流程的前提。

``scan.c`` 覆盖三项相关职责：

- **扫描**。``bt_le_scan_cb`` 注册表；广播报告到达 ``bt_le_scan_recv_listener`` 并被递交给应用。
- **周期性广播同步**。同步表（``bt_le_per_adv_sync_new`` / ``..._delete`` / ``..._lookup_addr``）及其监听器 —— ``..._establish_listener``、``..._lost_listener`` 和 ``..._report_recv_listener`` —— 跟踪对广播源周期性广播序列的同步。
- **BIGInfo 报告**。``hci_le_biginfo_adv_report`` 呈现搭载在周期性广播序列上的 BIGInfo。BIGInfo 携带接收端同步到广播等时组所需的参数，因此该处理函数是从扫描进入 BIG 接收端流程的桥梁。

因此，周期性广播同步是接收广播音频的入口点：扫描、同步到周期性广播序列、读取 BIGInfo，然后请求 ISO 子系统同步到该 BIG。

HCI 命令路径
------------------------

ISO 引擎从不直接与主机协议栈对话。它用 ``bt_hci_cmd_create(opcode, len)`` 构建一个标准 HCI 命令缓冲区，并用 ``bt_hci_cmd_send_sync(opcode, buf, rsp)`` 提交它（二者都在 ``host/common/hci.c`` 中）。``bt_hci_cmd_send_sync`` 是一个分发到当前激活适配器 ISO 命令入口点的单行函数，而这正是两种主机急剧分化之处：

.. list-table:: 各主机的 HCI 命令转换
   :header-rows: 1
   :widths: 20 40 40

   * - 方面
     - Bluedroid
     - NimBLE
   * - 入口点
     - ``bt_le_bluedroid_iso_cmd_send_sync``
     - ``bt_le_nimble_iso_cmd_send_sync``
   * - 转换
     - 一个 opcode 分支；每个命令都通过一条私有的直连 HCI（direct-HCI）路径作为原始 HCI 命令转发。
     - 一个 opcode 分支；每个命令都被拆解到 NimBLE 的类型化 ``ble_hs_hci_*`` ISO 辅助函数。
   * - 同步
     - ``adapter/bluedroid/hci.c`` 中的一个私有完成信号量；完成回调运行在 HCI 层任务上。
     - 在 ``ble_hs_hci_*`` 调用内部处理。

Bluedroid 自带命令路径的原因是并发。Bluedroid 的 BTU 任务使用单个全局槽位来把一个同步命令与其完成相匹配；若从 ISO 任务经由同一个槽位下发 ISO 命令，会与 BTU 任务竞争。``adapter/bluedroid/hci.c`` 中的直连 HCI 路径完全绕开了这一点：每个命令携带自己的完成回调，调用者在一个专用信号量上等待，因此那个共享的 BTU 槽位从不被触碰。该回调运行在 HCI 层任务上并保持极简 —— 它把响应复制到一个静态接收区并发出信号量，不获取任何锁。NimBLE 不需要这些，因为它的 ``ble_hs_hci_*`` 辅助函数已经提供了一个自包含的同步命令接口。

.. mermaid::

   sequenceDiagram
       participant CORE as ISO engine (host/iso/iso.c)
       participant HCI as bt_hci_cmd_send_sync (common/hci.c)
       participant AD as Active adapter
       participant CTRL as Controller
       CORE->>HCI: 构建命令缓冲区(opcode + 参数)
       HCI->>AD: *_iso_cmd_send_sync(opcode)
       Note over AD: Bluedroid:direct-HCI(私有信号量)<br/>NimBLE:ble_hs_hci_* 类型化辅助函数
       AD->>CTRL: HCI 命令
       alt Command Complete(例如 Set CIG Parameters)
           CTRL-->>AD: Command Complete
           AD-->>CORE: 状态 + 返回参数
       else Command Status(例如 Create CIS)
           CTRL-->>AD: Command Status
           AD-->>CORE: 状态(无返回参数)
           Note over CORE,CTRL: 真正的结果稍后作为一个 LE 元事件到达<br/>(例如 CIS Established),<br/>在下文 ISO 子系统中处理
       end

.. _arch-iso:

ISO 子系统
------------------------

ISO 子系统分布在三处：引擎（``host/iso/iso.c``）、元事件与数据粘合层（``host/common/iso.c``），以及适配器（``host/adapter/*/iso.c``）。引擎拥有三个按配置确定大小的静态池 —— 一个用于 ISO 通道（``CONFIG_BT_ISO_MAX_CHAN``）、一个用于连接等时组（``CONFIG_BT_ISO_MAX_CIG``）、一个用于广播等时组（``CONFIG_BT_ISO_MAX_BIG``）。每个公共引擎操作都遵循本节开头的 ``_safe`` 约定。

入站 ISO 元事件在两种主机上形态相同：适配器注册一个主机原生的 ISO 事件回调，封装该事件，并向 ISO 任务投递一个 ``ISO_HCI_EVENT`` 项；随后 ``host/common/iso.c`` 解码 LE 子事件并将其分发给引擎。一处线格式（wire-format）差异在此被吸收：NimBLE 的元事件结构已经包含子事件码，而 Bluedroid 适配器会在前面补上它，因此 ``host/common/iso.c`` 中的解码器无论在哪种主机上都看到统一的布局。

.. mermaid::

   %%{init: {'sequence': {'noteAlign': 'left'}}}%%
   sequenceDiagram
       participant CTRL as Controller
       participant AD as Active adapter
       participant T as ISO task
       participant CORE as ISO engine
       participant APP as App callback
       CTRL->>AD: ISO 元事件(HCI)
       Note over AD: Bluedroid:BTM ISO 回调(BTU 任务)<br/>NimBLE:ISO 回调(主机任务)
       AD->>T: 投递 ISO_HCI_EVENT
       CTRL-->>T: ISO 数据,投递 ISO_RX_DATA
       T->>CORE: 在 common/iso.c 中处理,然后引擎处理函数
       CORE->>APP: 通道回调(在 ISO 锁下)

连接 ISO（CIS）
~~~~~~~~~~~~~~~~~~~~~~~~

连接等时流（CIS）是点对点的，并依托一条 ACL 连接。两种角色通过不同的入口点驱动引擎：

.. list-table:: CIS 角色
   :header-rows: 1
   :widths: 16 44 40

   * - 角色
     - 流程
     - 关键符号
   * - Central
     - 配置一个组，然后向对端建立一条或多条流；控制器在每条流建立时予以确认。
     - ``bt_iso_cig_create``,\ |br|\ ``bt_iso_cig_reconfigure``,\ |br|\ ``bt_iso_cig_terminate``,\ |br|\ ``bt_iso_chan_connect`` / ``hci_le_cis_established``
   * - Peripheral
     - 注册一个服务端，由它决定是否接受入站的流；随后每个对端请求被接受或拒绝，若接受则予以确认。
     - ``bt_iso_server_register`` /\ |br|\ ``hci_le_cis_req`` / ``hci_le_cis_established``

广播 ISO（BIG）
~~~~~~~~~~~~~~~~~~~~~~~~

广播等时组（BIG）是无连接的，并依托一条周期性广播序列，而非 ACL 连接：

.. list-table:: BIG 角色
   :header-rows: 1
   :widths: 16 44 40

   * - 角色
     - 流程
     - 关键符号
   * - Broadcaster
     - 把一个 BIG 附着到一个承载周期性序列的扩展广播集上；控制器确认该组，之后即可向各条流馈送数据。
     - ``bt_iso_big_create`` / ``hci_le_big_complete``,\ |br|\ ``bt_iso_big_terminate`` / ``hci_le_big_terminate``
   * - Receiver
     - 同步到周期性序列，读取其 BIGInfo，然后同步到该组；同步丢失会被报告回来。
     - ``bt_iso_big_sync`` / ``hci_le_big_sync_established``,\ |br|\ ``hci_le_big_sync_lost``

接收端流程始于 ``scan.c``：在已建立的周期性广播同步上由 ``hci_le_biginfo_adv_report`` 递交的 BIGInfo，正是 ``bt_iso_big_sync`` 用来加入该组的输入。

数据通路与数据流
~~~~~~~~~~~~~~~~~~~~~~~~

仅建立一条 CIS 或 BIS 还不足以传输音频；每条流都必须绑定到控制器的 ISO 数据通路。``bt_iso_setup_data_path`` 绑定一个方向（发送用输入，接收用输出），``bt_iso_remove_data_path`` 释放它。一条已连接但未建立数据通路的流不承载任何 SDU：建立流和绑定其数据通路是两个独立步骤，因此在某个上层显式建立通路之前，音频不会流动。

一旦数据通路建立，SDU 就经由 ``host/common/iso.c`` 流动：

- **发送**。应用用 ``bt_iso_chan_send``（或带时间戳的 ``bt_iso_chan_send_ts``）提交一个 SDU；引擎把它交给 ``bt_le_iso_tx``。如果控制器有空闲缓冲区且前面没有排队项，该 SDU 直接发往控制器；否则它被保留在主机侧的发送队列中。控制器的发送完成信号作为一个 ``ISO_TX_COMP`` 事件返回并投递到 ISO 任务，在那里 ``bt_le_iso_handle_tx_comp`` 会按控制器当前空闲缓冲区的数量尽可能多地发送排队的 SDU，然后调用应用的发送完成回调。
- **接收**。一个到来的 SDU 由适配器递交、封装，并作为一个 ``ISO_RX_DATA`` 事件投递；``bt_le_iso_handle_rx_data`` 把它传给引擎中的 ``bt_iso_recv``，后者调用通道的接收回调。

两个方向都汇聚到 ISO 任务：发送完成和接收递交都是普通的排队事件，因此应用的 ISO 回调运行在 ISO 任务上下文、其 4 KB 栈上、在 ISO 锁下，与 :ref:`跨层事件流 <arch-event-flow>` 中所有其他排队事件完全一样。

ESP-BLE-ISO 其余的传输原语是 GATT、GAP 和 L2CAP。与组件其余部分一样，与主机无关的 API（``bt_gatt_*``、``bt_l2cap_*`` 和各 GAP 辅助函数）位于 ``host/common`` 中，并分发给所编译的那个适配器。

GATT
------------------------

GATT 横跨\ **客户端**\ 侧（本设备在对端进行发现、读、写和订阅）、**服务端**\ 侧（本设备暴露供对端访问的属性），以及限定二者的 ATT **MTU 交换**。这三者都在 ``host/common/gatt.c`` 中基于当前激活的适配器实现。

客户端
~~~~~~~~~~~~~~~~~~~~~~~~

``host/common/gatt.c`` 中与主机无关的客户端 API —— ``bt_gatt_discover``、``bt_gatt_read``、``bt_gatt_write``、``bt_gatt_write_without_response_cb``、``bt_gatt_subscribe`` 和 ``bt_gatt_unsubscribe`` —— 分发到当前激活适配器的 ``bt_le_*_gattc_*`` 实现。两个适配器以截然不同的机制支撑同一套 API：

.. list-table:: 各主机的 GATT 客户端支撑
   :header-rows: 1
   :widths: 22 39 39

   * - 方面
     - Bluedroid
     - NimBLE
   * - 发现
     - BTA GATTC 流程；BTA 拥有已发现的属性缓存。
     - 一次性完整遍历属性表，缓存在 ``gatt.db.c`` 中。
   * - 流程串行化
     - BTA GATTC 自带的按连接队列。
     - ``gatt.nrp.c`` 中的 NRP 队列；插入时对 params 和数据进行深拷贝。
   * - 结果递交
     - 每个 BTA GATT 事件都投递到 ISO 任务。
     - 通知和 ``bt_gatt_discover`` 结果投递到 ISO 任务；对端属性表遍历以及读写完成在主机任务上原地运行。

发现
^^^^^^^^^^^^^^^^^^^^^^^^

客户端发现有两条不同的流程，运行在不同的任务上。这种拆分的存在是因为两种主机在是否维护属性缓存上有差异：Bluedroid 的 BTA GATTC 会发现对端的属性表并自动缓存，而 NimBLE 不会 —— 因此移植在 ``gatt.db.c`` 中补上了该缓存。

**遍历对端的属性表（构建缓存）。**\ 当上层调用 ``bt_gattc_disc_start`` 时，NimBLE 适配器（``bt_le_nimble_gattc_db_auto_disc``）用 NimBLE 的 ``ble_gattc_disc_*`` 流程对对端执行真正的 ATT 发现，把整张表完整遍历一次 —— 每个主服务、包含服务、特征和描述符，包括每个 CCCD。这些流程回调运行在 **NimBLE 主机任务**\ 上，并填充按连接缓存的数据库；只有在遍历完成后，上层才在 ISO 任务上被通知。在 Bluedroid 上，这一步是隐式的：``bt_le_bluedroid_gattc_disc_start`` 驱动 BTA GATTC，由它执行 ATT 流程并维护自己的缓存。

**响应一项发现请求。**\ 当上层 —— 例如音频规范 —— 为特定属性调用 ``bt_gatt_discover`` 时，NimBLE 适配器把一个发现事件投递到 **ISO 任务**，在那里缓存的层次结构在本地回答该请求、无需进一步的 ATT 流量，并调用调用方的回调。在 Bluedroid 上，等价的结果作为 ISO 任务上的发现事件从 BTA 的缓存中呈现。

订阅生命周期
^^^^^^^^^^^^^^^^^^^^^^^^

订阅以与主机无关的方式跟踪。``bt_gatt_subscribe`` 把每个 ``bt_gatt_subscribe_params`` 记录在一个按连接的列表（``gattc_sub``）上，并且，除非已存在等价的订阅，否则通过适配器写入 CCC。有两点行为值得注意：

- 订阅在 CCC 写完成之前就被追加到列表，因为有些对端在回复 CCC 写之前就发送了第一条通知。
- ``subscribe`` 回调被\ **同步**\ 调用，在调用方的上下文中完成该流程，而不是在 CCC 写响应之后。在 NimBLE 上，这与缓存的数据库相配合：CCCD 句柄已经已知，因此无需发现往返，订阅路径也不会阻塞。

断开时，``bt_le_acl_conn_disconnected_gatt_listener`` 按固定顺序清理：先运行上层的断开回调，然后 ``gattc_sub_clear`` 遍历订阅列表，清除每个表项的句柄和值，并重新初始化该列表。因此上层\ **绝不能**\ 在断开回调内部清零自己的 subscribe params —— 此时这些 params 仍然挂在列表上，而通用层紧接着就会清除它们。

流程串行化
^^^^^^^^^^^^^^^^^^^^^^^^

**背景**：``gatt.nrp.c`` 存在的原因与 ``gatt.db.c`` 相同：它提供一项 BTA GATTC 原生提供、而 NimBLE 没有的服务。ATT 在每条连接上同一时刻只允许一个未完成的请求；BTA GATTC 把这一点隐藏在它自己的按连接流程队列之后，而 NimBLE 把它留给调用方来遵守。

**NRP 的作用**：``gatt.nrp.c`` 中的 NRP（need-response PDU，需响应 PDU）队列是 NimBLE 侧的等价物：一个按连接的单一队列，涵盖每一个需要响应的 PDU —— 客户端的读、写和订阅，加上服务端的指示（在下文服务端一节中描述）—— 因此同一时刻只有一个在途，其余的等待轮到自己。

**入队规则**：对客户端流程，它桥接到上层期望的「params 加回调」模型：``bt_le_nimble_gatt_nrp_insert`` 把一个读、写或订阅入队 —— 队列空闲时立即下发，否则保留它直到在途流程完成。``bt_le_nimble_gatt_nrp_remove`` 完成队首表项并启动下一个；``bt_le_nimble_gatt_nrp_clear`` 在断开时丢弃待处理的表项。读有三种形式 —— 按 UUID、长（分片）和单次 —— 每种都为每个结果递交一个事件，并显式处理流程结束。

**深拷贝**：``bt_le_nimble_gatt_nrp_insert`` 对 params 和任何数据进行深拷贝，因此调用方可以传入一个栈缓冲区并在返回时释放它。

**EATT 注记**：「同一时刻一个在途」的规则遵循 ATT「单一承载、每连接一个未完成请求」的模型。EATT 为每条连接引入多个并发承载，因此支持它将意味着重新审视 NRP 队列，以允许按承载的并发，而非单一流程在途。

回调上下文由此而来。NimBLE 在其主机任务上递交读或写完成，NRP 处理函数在那里、在 ISO 锁下调用应用回调 —— 与 GATT 服务端属性访问相同的主机任务上下文。Bluedroid 则把每个 BTA GATT 完成都投递到 ISO 任务，因此相应的回调在那里运行。这就是 :ref:`回调执行上下文 <arch-callback-context>` 的「读写完成」那一行。

服务端
~~~~~~~~~~~~~~~~~~~~~~~~

``host/common/gatt.c`` 的服务端侧负责注册服务（``bt_gatt_service_register``），提供标准的属性读辅助函数（``bt_gatt_attr_read`` 以及服务、包含服务、特征和 CCC 的各变体）、CCC 写路径（``bt_gatt_attr_write_ccc``、``bt_gatts_sub_changed``），以及出站的通知与指示 API（``bt_gatt_notify_cb``、``bt_gatt_indicate``）。两种主机之间差异最大的行为是入站属性访问如何完成，这正是 :ref:`回调执行上下文 <arch-callback-context>` 中首次描述的那个例外：

- Bluedroid 把读或写投递到 ISO 任务；已注册的属性回调在那里运行，响应则通过 BTA 异步返回给对端，并由一个专用的服务端信号量协调。
- NimBLE 在主机任务上原地运行属性回调，因为它的访问回调必须同步返回值。

出站传输同样遵循各主机的模型。在 NimBLE 上，通知是一次同步的缓冲区复制与下发，而指示 —— 它必须等待对端的确认 —— 则通过 NRP 排队，并在确认到达时完成。在 Bluedroid 上，二者都经过 BTA。无论哪种情况，交给 notify 或 indicate 的负载都会在调用返回前被复制，因此调用方可以立即重用它们的缓冲区。

.. _arch-iso-mtu:

MTU 交换
~~~~~~~~~~~~~~~~~~~~~~~~

ATT MTU 交换让两个对端把 ATT MTU 从其 23 字节的默认值抬高，以便单个 PDU 能承载更大的属性值。它是一个 GATT 流程：**客户端**\ 发送 ATT Exchange MTU Request，**服务端**\ 响应，两个对端首选值中较小的那个成为生效的 MTU。这些 GATT 角色独立于 GAP 的 central/peripheral 角色 —— 一个设备通常同时运行客户端和服务端，无论是哪一侧打开了连接 —— 因此这个行为最好按 GATT 角色来理解。

**作为 GATT 服务端。**\ 两种主机始终响应对端的 ATT Exchange MTU Request 并接受协商出的值。这无需应用做任何事，且与 GAP 角色无关。

**作为 GATT 客户端。**\ 设备是否自行抬高 MTU，取决于主机协议栈，并且在 Bluedroid 上取决于 GAP 角色：

- 在 Bluedroid 上这是自动的：当一个 GATT 客户端连接打开时，GATTC 适配器发起该交换（``handle_gattc_open_event`` 调用 ``BTA_GATTC_ConfigureMTU``，在 ``BTA_GATTC_Enh_Open`` 路径内部）。实践中，这会为一个 **central**\ 触发；而对于一个 **peripheral**，GATT 客户端的打开发生在发现启动路径内部，而后者本身又被 MTU 更新事件所阻挡 —— 一个使其无法发起的循环依赖。
- 在 NimBLE 上，组件不发起任何东西，因此由应用用 ``ble_gattc_exchange_mtu()`` 触发它 —— 通常在安全变更事件之后紧接着调用。在 Bluedroid 上，同一个调用对 MTU 而言是空操作，因为适配器已经交换过它了。

由于 GATT 服务端只会响应，一次交换 —— 由先充当客户端的那一侧发起 —— 就为整条连接确定了 MTU；同一条连接上同时也是客户端的对端会重用已协商的值，而不是再运行一次交换。设备所提供的首选值，以及某个规范为何会把它抬高到 ATT 默认值之上，由传输层之上的那一层设定：对于蓝牙 LE Audio，参见 :ref:`ATT MTU <arch-audio-mtu>`。

.. _arch-l2cap:

L2CAP（草案）
------------------------

.. warning::

   L2CAP 面向连接的通道目前是\ **早期草案，尚未正式支持**。该实现尚不完整 —— 它只存在于 NimBLE 上（``host/adapter/nimble/l2cap.c``），且\ **没有 Bluedroid 支持** —— 其 API 和行为都是临时的，可能会改变。请勿在生产环境中依赖该层。

``host/common/l2cap.c`` 提供基于信用（credit-based）的 L2CAP 面向连接的通道：一个服务端注册表（``bt_l2cap_server_register``）、出站通道操作（``bt_l2cap_chan_connect``、``bt_l2cap_chan_disconnect``、``bt_l2cap_chan_send``），以及主机在通道被接受、连接、断开或收到数据时调用的入站事件处理函数（``bt_le_l2cap_accept``、``bt_le_l2cap_connected``、``bt_le_l2cap_disconnected``、``bt_le_l2cap_received``）。协议栈中唯一的使用方是对象传输服务，它通过一条基于信用的通道搬运批量对象；不使用对象传输的规范从不打开通道。

.. _arch-app-event:

应用事件接口
------------------------

GAP 和 GATT 事件不会从适配器直达应用。它们会经过 ``host/common/app/gap.c`` 和 ``host/common/app/gatt.c`` 中一个很薄的应用事件接口 —— GAP 和 GATT 事件离开和进入应用的唯一一点。下面的段落跟随一个入站事件从适配器一路向上到应用回调，然后是反向的注入路径。

**注册。**\ 应用通过 ``bt_le_gap_app_cb_register`` 和 ``bt_le_gatt_app_cb_register`` 为每个类别恰好注册一个回调。每个类别只保留一个回调指针，因此所有 GAP 事件有一个应用汇聚点，所有 GATT 事件有一个。

**来源 —— 适配器到 ISO 任务。**\ 在接口能递交一个事件之前，适配器先从主机获取它并把它投递到 ISO 任务队列。对 GAP，Bluedroid 适配器在 ``bt_le_bluedroid_gap_init`` 中注册一个 BTA/BTM GAP 回调；该回调运行在 BTU 任务上，构造一个事件，并用 ``bt_le_bluedroid_gap_post_event`` 入队。NimBLE 适配器提供一个 ``ble_gap_event`` 回调，其 ``*_cb_safe`` 包装函数获取 ISO 锁并投递该事件。GATT 事件由 GATT 适配器以相同方式获取，已在上文 GATT 中详述。除此之外 GAP 本身没有引擎 —— 它只是覆盖在其他模块所拥有的状态之上的一个事件接口：``conn.c`` 中的 ACL 连接表（ACL 连接与断开、安全与身份变更、绑定删除），以及 ``scan.c`` 中的扫描和周期性广播同步状态（扩展扫描报告、PA 同步建立、丢失与报告、BIGInfo）。

**递交 —— ISO 任务到应用。**\ 当 ISO 任务出队一个 GAP 或 GATT 事件时，它调用 ``bt_le_gap_handle_event`` 或 ``bt_le_gatt_handle_event``（来自 ``task.c``）。它们按事件类型分发，构造一个有类型的 ``bt_le_gap_app_event`` 或 ``bt_le_gatt_app_event``，调用已注册的回调，并释放排队的负载。由于这运行在 ISO 任务内部，应用回调在 ISO 任务上下文、在 ISO 锁下执行 —— 即 :ref:`回调执行上下文 <arch-callback-context>` 中描述的那个上下文。递交的 GAP 事件包括 ACL 连接与断开、安全与身份变更、绑定删除、扩展扫描报告、周期性广播同步状态和 BIGInfo；递交的 GATT 事件包括 ATT MTU 变更、GATT 客户端发现完成和 GATT 服务端订阅变更。

**注入 —— 应用到 ISO 任务。**\ 反方向让应用把一个事件投递到 ISO 任务，通过 ``bt_le_gap_app_post_event``（公开暴露为 ``esp_ble_iso_gap_app_post_event``）。它因主机而异：在 Bluedroid 上它转发给 ``bt_le_bluedroid_gap_post_event``，在 NimBLE 上转发给 ``bt_le_nimble_gap_post_event``。在这些组件出厂的配置下，注入路径只在 NimBLE 上使用。

.. mermaid::

   flowchart TB
       ADP["适配器<br/>(主机事件)"]
       T["ISO 任务队列"]
       AIF["app/ 接口<br/>(gap.c, gatt.c)"]
       APP["应用<br/>(一个 GAP 和<br/>一个 GATT 回调)"]
       ADP -->|投递原始事件| T
       T -->|出队并分发| AIF
       AIF -->|有类型的应用事件| APP
       APP -->|调用 app_post_event| AIF
       AIF -->|注入队列| T

**接口为何如此设计。**\ 三个目标驱动了这个设计：

- **无论哪种主机都是同一个事件模型。**\ 适配器把每个原生事件 —— Bluedroid 上的 BTA/BTM 回调、NimBLE 上的 ``ble_gap_event`` —— 在它到达应用之前归一化成同一个有类型的 ``bt_le_gap_app_event`` / ``bt_le_gatt_app_event``。因此上层 —— 示例和音频规范 —— 处理一组完全相同的事件，从不针对主机协议栈做分支。这是该接口最重要的特性：在两种主机之间迁移一个应用，无需改动其事件处理。
- **内部使用方也会收到该事件。**\ 有些事件不仅供应用使用：``host/iso/iso.c`` 中的 ISO 引擎和预构建的音频库也会消费它们 —— 一次周期性广播同步及其 BIGInfo 会驱动一次 BIG 同步，而连接、断开和安全变更会驱动各规范。因此接口保证一份副本到达 ISO 任务，内部工作在那里于 ISO 锁下运行。在 Bluedroid 上，BTU 任务的回调直接投递它；在 NimBLE 上，应用用 ``esp_ble_iso_gap_app_post_event`` 把它收到的事件转发进 ISO 任务（参见 :ref:`各主机集成差异 <arch-iso-host-diff>`）。
- **与普通 BLE 应用共存。**\ 向 ISO 任务投递一份副本并不会消耗掉该事件。在 Bluedroid 上，同一个回调仍会把原生事件转发给 Bluedroid 的应用回调层（BTC），因此一个同时通过 ``esp_ble_gap_register_callback`` 注册的应用，会继续收到它用于自己的、非音频的 BLE 工作；在 NimBLE 上，应用已经在它的 ``ble_gap_event`` 回调内部，可以继续在那里处理它需要的其他任何事情。ESP-IDF 蓝牙 LE Audio 应用和一个传统 BLE 应用可以在一个设备上并行运行。

.. important::

   这些应用和规范回调运行在 **ISO 任务上**\ （参见 :ref:`回调执行上下文 <arch-callback-context>`），因此回调必须快速返回。在其中阻塞、休眠、等待 I/O 或运行长时间计算，会拖住那个唯一的事件循环，而所有其他 GAP、GATT 以及 —— 最关键的 —— ISO 数据事件都依赖它，这会直接表现为音频延迟或丢失。请把任何耗时工作交给另一个任务。

.. _arch-iso-api:

公共 API
------------------------

到目前为止描述的一切都是内部实现。应用只看到一个公共头文件 ``api/include/esp_ble_iso_common_api.h``，它把传输层暴露为一个小巧的 ``esp_ble_iso_*`` API，用于纯 ISO 用例 —— 不带蓝牙 LE Audio 规范的 CIS 或 BIS。

形态与约定
~~~~~~~~~~~~~~~~~~~~~~~~

公共 API 是覆盖在 :ref:`传输层 <arch-iso-transport>` ISO 引擎之上的一层薄外观：

- **不透明类型**。``esp_ble_iso_chan_t``、``esp_ble_iso_cig_t`` 和 ``esp_ble_iso_big_t`` 等公共类型是内部 ``bt_iso_*`` 结构体的 typedef，``esp_ble_conn_t`` 是 ``bt_conn`` 的 typedef。应用把它们作为不透明句柄持有。
- **错误码。**\ 每个函数返回 ``esp_err_t``，而非引擎内部使用的负 ``errno`` 值。
- **入口加锁。**\ 每次调用都经由其 ``_safe`` 包装函数进入引擎，因此公共 API 是获取全局 ISO 锁的最外层；应用自己从不管理该锁。
- **单次初始化**。``esp_ble_iso_common_init`` 接收一个 ``esp_ble_iso_init_info_t``，其唯一字段是应用的 GAP 回调；此后 ISO 事件通过该回调或通过各通道的操作递交。

功能分组
~~~~~~~~~~~~~~~~~~~~~~~~

.. list-table:: ESP-BLE-ISO 公共 API 按用途分类
   :header-rows: 1
   :widths: 24 42 34

   * - 分组
     - 代表性函数
     - 用途
   * - 初始化
     - ``esp_ble_iso_common_init``
     - 注册 GAP 回调并启动传输层。
   * - CIS —— central
     - ``esp_ble_iso_cig_create``,\ |br|\ ``esp_ble_iso_cig_reconfigure``,\ |br|\ ``esp_ble_iso_cig_terminate``,\ |br|\ ``esp_ble_iso_chan_connect``
     - 配置一个连接等时组并建立其各条流。
   * - CIS —— peripheral
     - ``esp_ble_iso_server_register``,\ |br|\ ``esp_ble_iso_server_unregister``
     - 接受入站的连接等时流。
   * - BIG —— broadcaster
     - ``esp_ble_iso_big_ext_adv_add``,\ |br|\ ``esp_ble_iso_big_create``,\ |br|\ ``esp_ble_iso_big_terminate``,\ |br|\ ``esp_ble_iso_big_register_cb``
     - 通过一个广播集广播一个等时组。
   * - BIG —— receiver
     - ``esp_ble_iso_big_sync``
     - 同步到一个广播等时组。
   * - 数据通路
     - ``esp_ble_iso_setup_data_path``,\ |br|\ ``esp_ble_iso_remove_data_path``,\ |br|\ ``esp_ble_iso_chan_send``,\ |br|\ ``esp_ble_iso_chan_send_ts``
     - 把一条流绑定到控制器的 ISO 数据通路并搬运 SDU。
   * - 信息
     - ``esp_ble_iso_chan_get_info``,\ |br|\ ``esp_ble_iso_chan_get_tx_sync``
     - 查询通道与发送定时信息。
   * - 辅助
     - ``esp_ble_iso_data_parse``
     - 解析长度-类型-值（LTV）编码的数据。

这些与 :ref:`ISO 子系统 <arch-iso>` 中的引擎操作一一对应；公共层只增加了锁、错误转换和不透明 typedef。

.. _arch-iso-host-diff:

各主机集成差异
~~~~~~~~~~~~~~~~~~~~~~~~

公共 API 中有两部分专门因为两种主机以不同方式路由连接和 GAP 事件而存在。二者解决同一个问题 —— 让引擎看到应用所发起连接的事件 —— 各以其主机所要求的方式：

- ``esp_ble_iso_gap_app_post_event`` **仅在 NimBLE 上需要**。当应用在 NimBLE 上发起一个连接或扫描时，主机把 GAP 事件递交给应用注册的回调；应用用这个函数把它们转发进引擎。在 Bluedroid 上，引擎安装自己的 BTM GAP 回调并直接捕获事件，因此无需转发。
- ``esp_ble_iso_bluedroid_get_gattc_if`` **仅用于 Bluedroid**。它返回引擎内部的 BTA GATTC 接口，应用在发起连接时把它传给 ``esp_ble_gattc_open``，使由此产生的 ACL 事件路由回引擎，从而避免第二次 BTA GATTC 注册。NimBLE 的对应物是上面的事件转发。
