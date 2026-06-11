.. _arch-audio:

ESP-BLE-AUDIO
=============

:link_to_translation:`en:[English]`

.. |br| raw:: html

   <br/>

.. contents:: 目录

ESP-BLE-AUDIO 直接位于 ESP-BLE-ISO 之上，实现蓝牙低功耗音频的各项规范和服务。它是 ESP-IDF 蓝牙低功耗音频应用实际编程所面向的那一层，通过 ``esp_ble_audio_*`` API 访问。

组件布局
------------------------

该组件有两类截然不同的代码，而这一区分对本文档很重要：

- **开源、本文涵盖**。公共 API 头文件 (``api/include/esp_ble_audio_*_api.h``)、``host/adapter/bluedroid/profiles`` 和 ``host/adapter/nimble/profiles`` 下的 GATT 服务适配器、``host/services/ots`` 下的对象传输服务，以及 ``host/common/init.c`` 中的初始化编排。
- **预构建、不在范围内**。ESP-IDF 蓝牙低功耗音频的规范与控制器逻辑 —— 单播与广播客户端，音量、麦克风、媒体和通话控制，集合协调，以及顶层规范 —— 以预构建的、按目标芯片的库形式提供（``lib/lib/<target>/libble_audio.a``，用 ``add_prebuilt_library`` 链接）。本文档不描述其内部实现；它描述围绕它的公共契约，以及在重要之处，一个已注册回调运行在哪个任务上。

分层
------------------------

.. mermaid::

   %%{init: {'flowchart': {'nodeSpacing': 40, 'rankSpacing': 40, 'subGraphTitleMargin': {'top': 6, 'bottom': 14}}}}%%
   flowchart TB
       APP["应用"]
       API["ESP-BLE-AUDIO 公共 API<br/>(esp_ble_audio_*)"]
       subgraph AUDIO["ESP-BLE-AUDIO"]
         direction LR
         LIB["预构建的规范与<br/>控制器库"]
         SVC["GATT 服务适配器<br/>(各主机)"]
         OTS["对象传输服务"]
         LIB ~~~ SVC
         LIB ~~~ OTS
       end
       ISO["ESP-BLE-ISO 传输层<br/>(GATT, GAP, ISO, L2CAP, ISO 任务)"]
       APP --> API --> AUDIO --> ISO

.. _arch-audio-svc-ctrl:

服务与控制器
------------------------

蓝牙低功耗音频的功能分为两类构建块，组件也反映了这一划分：

- **GATT 服务**\ 是对端进行读、写和订阅的属性表。它们在开源的各主机适配器 (``host/adapter/*/profiles``) 中实现，每个服务一个源文件，构建在 ESP-BLE-ISO 的 GATT 服务端层之上。按其所属规范分组（如蓝牙低功耗音频规范中那样），这组服务是 PACS、ASCS 和 BASS (BAP)，MCS (MCP)，TBS (CCP)，CSIS (CSIP)，MICS (MICP)，VCS (VCP)，CAS (CAP)，TMAS (TMAP) 和 HAS (HAP)。
- **规范客户端与控制器**\ 是驱动这些服务并编排各条流的状态机：基本音频规范和通用音频规范，音量、麦克风、媒体和通话控制，协调集识别，以及顶层的电话与媒体、游戏、公共广播规范。这部分逻辑位于预构建库中；应用通过对应的 ``esp_ble_audio_*_api.h`` 头文件访问它。

GATT 服务在 :ref:`GATT 服务 <arch-audio-services>` 中详述，客户端与控制器在 :ref:`客户端与控制器 <arch-audio-profiles>` 中详述。

规范回调运行在何处
------------------------

一个已注册的 ESP-IDF 蓝牙低功耗音频回调运行在递交底层传输事件的那个任务上，因此 :ref:`回调执行上下文 <arch-callback-context>` 中的规则直接适用：

- 由\ **通知**\ 驱动的回调 —— 例如作为 GATT 通知到达的 ASCS 或音量状态变更 —— 在两种主机上都运行于 ISO 任务。
- 由\ **GATT 客户端读或写完成**\ 驱动的回调 —— 例如读取对端的 PAC 记录 —— 在 Bluedroid 上运行于 ISO 任务，但在 NimBLE 上运行于\ **NimBLE 主机任务**。
- 由对\ **本地服务的入站属性写**\ 驱动的回调 —— 例如客户端配置一个 ASE —— 在 Bluedroid 上运行于 ISO 任务，但在 NimBLE 上运行于\ **NimBLE 主机任务**。

其后果贯穿整个协议栈：在 ISO 任务上，回调共享 4 KB 栈，绝不能阻塞事件循环；而在 NimBLE 主机任务上，它在主机的属性路径内部执行。无论哪种情况，全局 ISO 锁都被持有，因此无论回调运行在哪个任务上，规范状态都保持一致。

.. _arch-audio-services:

GATT 服务
------------------------

蓝牙低功耗音频通过一组固定的 GATT 服务暴露其能力和状态。这些是各规范的服务端侧 —— 对端进行读、写和订阅的属性表 —— 并且，如 :ref:`服务与控制器 <arch-audio-svc-ctrl>` 中所确立的，它们是 ESP-BLE-AUDIO 的开源部分：``host/adapter/bluedroid/profiles`` 和 ``host/adapter/nimble/profiles`` 下每个服务一个源文件。每个服务背后的状态，以及其控制点的命令处理，位于预构建库中；本节涵盖这些服务及其适配器，而非那部分逻辑。

.. _arch-audio-adapter:

服务适配器模式
~~~~~~~~~~~~~~~~~~~~~~~~

每个服务都以相同的方式实现。预构建库拥有服务属性表的规范化、与主机无关的定义（一个 ``bt_gatt_service``）；开源适配器把该表注册到当前激活主机的 GATT 服务端，然后把主机分配的句柄回填到库的定义上，以便库在通知或指示时能寻址自己的属性。两种主机在如何完成注册上有差异：

- **NimBLE** 适配器以 NimBLE 的原生形式（一个 ``ble_gatt_svc_def``）第二次声明该服务，用 ``ble_gatts_add_svcs`` 注册它，检查其特征与库的定义相符，并通过一个 ``*_attr_handle_set`` 步骤把分配的句柄映射回去。注册是同步、单阶段的。
- **Bluedroid** 适配器把库的 ``bt_gatt_service`` 交给一个共享的 ``bt_le_bluedroid_svc_init`` 辅助函数，由它驱动 BTA GATTS。由于 BTA 以两个异步阶段注册一个服务，每个 Bluedroid 服务适配器都暴露一对匹配的 ``*_init`` 和 ``*_start``，并用 GATT 服务端层的服务端信号量协调完成。

一旦服务被注册，对端对其属性的访问遵循 :ref:`回调执行上下文 <arch-callback-context>` 的各主机属性访问路径：在 NimBLE 主机任务上同步完成，或在 Bluedroid 上投递到 ISO 任务。

.. _arch-audio-deferred-add:

**何时添加服务（延迟注册）**

**机制**：并非所有服务都在 ``esp_ble_audio_common_init`` 期间被添加到 GATT 表。构建默认定义了 ``BLE_AUDIO_SVC_DEFERRED_ADD``，它会把一个服务保留到应用通过其 ``esp_ble_audio_<profile>_register`` 调用注册对应角色为止 —— 预构建库把该调用转化为该服务的 ``bt_le_<service>_init`` 注册 —— 而不是在 init 期间添加它。

**原因**：一个固件镜像通常编译进了比任何单个应用所用更多的蓝牙低功耗音频能力：启用一个服务的 Kconfig 角色选项会把它的代码编译进来，但应用可能永远不会担任那个角色。如果每个编译进来的服务都在 init 时被添加，一个连接上来的对端就会发现、并可能访问一个其背后尚无支撑状态的服务的属性 —— 应用还没注册它，因此预构建库无法对它做出有意义的应答。

**对应用的影响**：延迟添加把一个未使用但已编译进来的能力挡在 GATT 表之外，直到应用主动选用它，因此对端永远只会发现那些被完整支撑的服务。

服务参考
~~~~~~~~~~~~~~~~~~~~~~~~

.. list-table:: 蓝牙低功耗音频 GATT 服务
   :header-rows: 1
   :widths: 10 24 38 11 17

   * - 服务
     - 全称
     - 关键特征
     - 适配器
     - 规范
   * - PACS
     - 已发布音频能力服务 (Published Audio Capabilities Service)
     - Sink 和 Source PAC 记录、音频位置、可用与支持的音频上下文。
     - ``pacs.c``
     - BAP
   * - ASCS
     - 音频流控制服务 (Audio Stream Control Service)
     - Sink 和 Source 音频流端点 (ASE) 以及 ASE 控制点。
     - ``ascs.c``
     - BAP
   * - BASS
     - 广播音频扫描服务 (Broadcast Audio Scan Service)
     - 广播接收状态以及广播音频扫描控制点。
     - ``bass.c``
     - BAP
   * - MCS
     - 媒体控制服务 (Media Control Service)
     - 媒体播放器名称与曲目信息、媒体控制点及相关状态。
     - ``mcs.c``
     - MCP
   * - TBS
     - 电话承载服务 (Telephone Bearer Service)
     - 承载信息、通话状态以及通话控制点。
     - ``tbs.c``
     - CCP
   * - CSIS
     - 协调集识别服务 (Coordinated Set Identification Service)
     - 集合身份解析密钥 (SIRK)、集合大小、集合成员锁和排名。
     - ``csis.c``
     - CSIP
   * - MICS
     - 麦克风控制服务 (Microphone Control Service)
     - 麦克风静音；包含 AICS。
     - ``mics.c``
     - MICP
   * - VCS
     - 音量控制服务 (Volume Control Service)
     - 音量状态、音量控制点和音量标志；包含 VOCS 和 AICS。
     - ``vcs.c``
     - VCP
   * - CAS
     - 通用音频服务 (Common Audio Service)
     - 标识一个通用音频设备；可包含 CSIS。
     - ``cas.c``
     - CAP
   * - TMAS
     - 电话与媒体音频服务 (Telephony and Media Audio Service)
     - 设备的 TMAP 角色。
     - ``tmas.c``
     - TMAP
   * - HAS
     - 助听器访问服务 (Hearing Access Service)
     - 助听器特性、预设控制点和当前预设索引。
     - ``has.c``
     - HAP

每个服务都由应用通过其所属规范的 ``esp_ble_audio_*_api.h`` 头文件配置（例如 VCS 用 ``esp_ble_audio_vcp_api.h``，PACS 用 ``esp_ble_audio_pacs_api.h``）；这些 API 是后续客户端与控制器参考的一部分。

包含服务与控制点
~~~~~~~~~~~~~~~~~~~~~~~~

有几个服务不是独立的，而是被另一个服务所包含：

- **CAS** 在设备是某个协调集的成员时包含 **CSIS**，因此对端通过通用音频设备发现集合成员关系。
- **VCS** 包含零个或多个 **VOCS**\ （音量偏移控制服务）和 **AICS**\ （音频输入控制服务）实例 —— 每个独立偏移的输出一个 VOCS，每个音频输入一个 AICS。
- **MICS** 包含零个或多个 **AICS** 实例，每个音频输入一个。

因此 AICS 和 VOCS 有公共 API 头文件（``esp_ble_audio_aics_api.h`` 和 ``esp_ble_audio_vocs_api.h``），但没有独立的适配器文件；包含它们的服务把它们作为自己属性表的一部分注册。

有几个服务 —— ASCS、BASS、MCS、TBS、CSIS、VCS 和 HAS —— 暴露一个控制点：一个可写的特征，它携带一条命令，并通过通知受影响的状态特征来应答。命令处理位于预构建库中；适配器只把写入向内传递、把由此产生的通知向外传递，运行在 :ref:`回调执行上下文 <arch-callback-context>` 所描述的任务上。

.. _arch-audio-profiles:

客户端与控制器
------------------------

各规范及其客户端和控制器角色是 ESP-BLE-AUDIO 中以预构建库形式提供的部分。本节在本文档所能达到的层面描述它们：它们的角色、应用驱动它们所用的公共 API，以及它们的回调运行在哪个任务上。内部状态机不在范围内。

一个客户端或控制器角色通过一条连接作用于远端设备 —— 例如一个音量控制器改变某个渲染器的音量 —— 而一个渲染器、设备、成员或服务端角色在本地应答，并由上一节中的某个 GATT 服务支撑。一个应用可以同时担任多个角色。

规范之间的关系
~~~~~~~~~~~~~~~~~~~~~~~~

蓝牙低功耗音频各规范彼此构建于其上。\ **基本音频规范 (BAP)**\ 是基础：它建立并承载音频流，既包括单播（通过连接等时流）也包括广播（通过广播等时流）。\ **控制规范** —— 音量、麦克风、协调集、媒体和通话控制 —— 管理各项功能，每个都由一个 GATT 服务支撑。\ **通用音频规范 (CAP)**\ 协调其他规范，使一个操作在一个集合的每个成员上一致地生效。\ **顶层规范** —— 电话与媒体音频 (TMAP)、游戏音频 (GMAP)、公共广播 (PBP) 和助听器访问 (HAP)—— 是下层各层的既定组合。

.. mermaid::

   flowchart TB
       TOP["顶层规范 — TMAP、GMAP、PBP、HAP"]
       CAP["CAP — 跨协调集的协同控制"]
       CTRL["控制规范 — VCP、MICP、CSIP、MCP、CCP"]
       BAP["BAP — 单播 (CIS) 与广播 (BIS) 流"]
       SVC["蓝牙低功耗音频 GATT 服务"]
       ISO["ESP-BLE-ISO 传输层"]
       TOP --> CAP
       CAP --> BAP
       CAP --> CTRL
       TOP --> CTRL
       CTRL --> SVC
       BAP --> ISO

规范参考
~~~~~~~~~~~~~~~~~~~~~~~~

.. list-table:: 蓝牙低功耗音频规范
   :header-rows: 1
   :widths: 10 27 45 18

   * - 规范
     - 全称
     - 角色
     - API 头文件
   * - BAP
     - 基本音频规范 (Basic Audio Profile)
     - 单播客户端与服务端、广播源与广播接收端、广播助手、扫描委托设备。
     - ``bap_api.h``
   * - CAP
     - 通用音频规范 (Common Audio Profile)
     - 在一个协调集上的发起者、指挥者与切换。
     - ``cap_api.h``
   * - VCP
     - 音量控制规范 (Volume Control Profile)
     - 音量控制器（客户端）和音量渲染器（服务端）。
     - ``vcp_api.h``
   * - MICP
     - 麦克风控制规范 (Microphone Control Profile)
     - 麦克风控制器（客户端）和麦克风设备（服务端）。
     - ``micp_api.h``
   * - CSIP
     - 协调集识别规范 (Coordinated Set Identification Profile)
     - 集合协调者（客户端）和集合成员（服务端）。
     - ``csip_api.h``
   * - MCP
     - 媒体控制规范 (Media Control Profile)
     - 媒体控制客户端（客户端）和媒体代理（服务端）。
     - ``mcc_api.h``、``media_proxy_api.h``
   * - CCP
     - 通话控制规范 (Call Control Profile)
     - 通话控制客户端（服务端侧即电话承载服务）。
     - ``ccp_api.h``
   * - TMAP
     - 电话与媒体音频规范 (Telephony and Media Audio Profile)
     - 组合 CAP、BAP 和各控制规范的角色配置。
     - ``tmap_api.h``
   * - GMAP
     - 游戏音频规范 (Gaming Audio Profile)
     - 面向低延迟游戏音频的角色配置。
     - ``gmap_api.h``
   * - PBP
     - 公共广播规范 (Public Broadcast Profile)
     - 在 BAP 广播之上的公共广播通告辅助。
     - ``pbp_api.h``

API 形态与角色
~~~~~~~~~~~~~~~~~~~~~~~~

每个规范在其 ``esp_ble_audio_<profile>_api.h`` 头文件中暴露一套小巧、统一的 API：应用为它所担任的角色注册一个回调结构体，并调用函数来启动操作 —— 发现一个对端、配置一条流、设置音量、发起通话。结果和状态变更随后异步地通过该回调到达，运行在 :ref:`回调执行上下文 <arch-callback-context>` 所描述的任务上 —— 多数情况下是 ISO 任务，对于 GATT 读写完成和对本地服务的入站写则是 NimBLE 主机任务。

由于注册和分发在各规范之间是统一的，向一个应用添加一个角色总是同样的形态：启用该角色的 Kconfig 选项、注册其回调结构体、并通过其函数驱动它。角色选项还决定了哪些 GATT 服务被编译进来，正如下面的初始化流程所示。

对象传输（草案）
------------------------

.. warning::

   对象传输目前是\ **早期草案，尚未正式支持**。它构建在草案 L2CAP 通道 (:ref:`L2CAP <arch-l2cap>`) 之上，因此只运行在 NimBLE 上，**不支持 Bluedroid**，其 API 和行为都是临时的，可能会改变。请勿在生产环境中依赖它。

对象传输服务 (OTS) 通过 :ref:`L2CAP <arch-l2cap>` 中那条基于信用的 L2CAP 通道搬运批量对象 —— 比一次普通 GATT 读所能承载的更大。在蓝牙低功耗音频中它被媒体控制使用：一个媒体播放器暴露诸如当前曲目分段和分组结构之类的对象，客户端用 OTS 传输它们，经由媒体 API（``esp_ble_audio_mcs_get_ots`` 和媒体控制客户端）访问。

与各规范不同，OTS 是开源的，位于 ``host/services/ots`` 下。它的各个部分是：

- 一个\ **服务端**\ (``ots.c``) 和一个\ **客户端**\ (``ots_client.c``)；
- **对象动作控制点**\ (``ots_oacp.c``)，它承载读、写、创建等对象操作；
- **对象列表控制点**\ (``ots_olcp.c``)，它在对象列表中导航；
- 一个\ **对象管理器**\ (``ots_obj_manager.c``) 和一个\ **目录列表**\ 对象 (``ots_dir_list.c``)；
- **L2CAP 传输**\ (``ots_l2cap.c``)，它通过基于信用的通道承载对象数据。

.. _arch-init-flow:

初始化流程
------------------------

一个 ESP-IDF 蓝牙低功耗音频应用用两次调用启动协议栈，与传输层对应：先 ``esp_ble_audio_common_init``，后 ``esp_ble_audio_common_start``。

``esp_ble_audio_common_init`` 运行 ``init.c`` 中的 ``bt_le_audio_init``，它首先与预构建库建立边界 —— 检查共享结构 ABI 是否匹配，并把当前激活的 Kconfig 值推送进库 —— 然后调用主机相关的 ``bt_le_{host}_audio_init``。该主机初始化函数启动标准的 GAP 和 GATT 服务，然后按固定顺序通过各自的适配器注册每个已启用的蓝牙低功耗音频服务：PACS、ASCS、BASS、TMAS、GTBS、HAS、CSIS 和 CAS，然后是媒体、音量和麦克风服务。每个服务仅在其 Kconfig 角色选项被设置时才编译进来，因此一个构建恰好包含其角色所需的服务。那个固定顺序是这些服务注册的顺序；它们中大多数实际何时进入 GATT 表是另一个问题 —— 默认情况下，添加被延迟到应用按角色的 ``esp_ble_audio_<profile>_register`` 调用，而不是在 init 这里发生（参见 :ref:`延迟注册 <arch-audio-deferred-add>`）。

``esp_ble_audio_common_start`` 运行 ``bt_le_audio_start`` 并分发到主机相关的 start，它首先从 ``start_info`` 注册协调集服务（CSIS 和 CAS），然后提交 GATT 服务端。在 Bluedroid 上，提交是第二个阶段，它真正启动 init 期间注册的 BTA 服务 —— 即 :ref:`服务适配器模式 <arch-audio-adapter>` 中的 ``*_init`` 与 ``*_start`` 配对；在 NimBLE 上，它调用 ``ble_gatts_start``，然后把分配的属性句柄回填到库中。

.. mermaid::

   %%{init: {'sequence': {'noteAlign': 'left'}}}%%
   sequenceDiagram
       participant APP as Application
       participant CMN as ESP-BLE-AUDIO (init.c)
       participant LIB as Prebuilt library
       participant AD as Host adapter
       APP->>CMN: esp_ble_audio_common_init (gap_cb, gatt_cb)
       CMN->>LIB: 检查 ABI,推送配置
       CMN->>AD: bt_le_{host}_audio_init
       Note over AD: 启动 GAP/GATT,然后按固定顺序<br/>注册已启用的服务
       APP->>CMN: esp_ble_audio_common_start
       CMN->>AD: bt_le_{host}_audio_start
       Note over AD: 从 start_info 注册 CSIS/CAS,然后提交 GATT 服务端<br/>Bluedroid:第二阶段启动已注册的 BTA 服务<br/>NimBLE:ble_gatts_start,然后回填属性句柄

.. _arch-audio-mtu:

ATT MTU
~~~~~~~~~~~~~~~~~~~~~~~~

蓝牙低功耗音频依赖一个足够大的 ATT MTU 来承载其控制 PDU。

**规范下限**：基本音频规范 (BAP) 要求的最低 ATT MTU 仅为 64 字节（公共的 ``ESP_BLE_AUDIO_ATT_MTU_MIN``）。

**为何选 128**：64 字节不足以同时操作四个 ASE，因此 ESP-IDF 把默认值提高到 128 字节 (``BLE_AUDIO_ATT_MTU_MIN``)，为蓝牙低功耗音频所依赖的 ASCS 和 PACS 控制 PDU 留出余量。两种主机都在初始化期间 (``bt_le_{host}_audio_init``) 把设备的首选 ATT MTU 设为该值，Bluedroid 通过 ``BTA_GATT_SetLocalMTU``、NimBLE 通过 ``ble_att_set_preferred_mtu``。

**协商规则**：该值是设备作为客户端时提供、作为服务端时接受的值，因此最终生效的 MTU 是两个对端首选值中较小的那个。

交换本身就是 :ref:`MTU 交换 <arch-iso-mtu>` 中描述的那个 GATT 流程：客户端发送请求，服务端响应，一次协商管辖整条连接。在通常的蓝牙低功耗音频拓扑中 —— 手机作为 central、ESP 设备作为 peripheral —— 每个设备都\ **同时**\ 运行一个 GATT 客户端和一个 GATT 服务端，因为 GATT 角色独立于 GAP 角色。然而单次 MTU 交换覆盖整条连接：手机的客户端发起它，ESP 的服务端响应，随后那一个协商出的值便管辖它们之间所有的 ATT 流量 —— 无论某一时刻哪一侧充当客户端或服务端：

.. mermaid::

   sequenceDiagram
       box transparent 手机 (GAP central)
       participant PS as GATT server
       participant PC as GATT client
       end
       box transparent ESP 设备 (GAP peripheral)
       participant ES as GATT server
       participant EC as GATT client
       end
       PC->>ES: ATT Exchange MTU Request
       ES-->>PC: ATT Exchange MTU Response
       Note over ES: 服务端只响应,从不发起
       Note over PS,EC: 那一个协商出的 MTU(两侧的较小值)<br/>管辖所有 ATT 流量,双向
       PC->>ES: 服务发现
       PC->>ES: 订阅(CCCD),然后 PACS / ASCS 读、写
       EC->>PS: 服务发现(peripheral 作为客户端,复用同一 MTU)

另外两个角色 —— 手机的 GATT 服务端和 ESP 的 GATT 客户端 —— 也存在于同一条连接上：ESP 的客户端可以对手机的服务端运行自己的服务发现，但这两个角色都不会运行第二次 MTU 交换；二者都重用手机的客户端已经协商好的值。由于 ESP peripheral 只响应、从不发起 MTU 交换，那一次协商的节奏由 central 把控，而非由 peripheral 的主机协议栈。

端到端流程
------------------------

下面的四条流程把各层串联起来，对应两个单播角色和两个广播角色。它们刻意保持高层次 —— 每个箭头可能代表多次交换 —— 并展示每一步由哪个组件负责。

单播发起端 (Unicast Initiator)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

单播发起端 (Initiator) 连接到一个接受端 (Acceptor)，配置其音频流端点，建立一条连接等时流，并开始发送音频：

.. mermaid::

   sequenceDiagram
       participant APP as Application (initiator)
       participant AUD as ESP-BLE-AUDIO
       participant ISO as ESP-BLE-ISO
       participant CTRL as Controller
       APP->>ISO: 扫描并连接(GAP)
       APP->>ISO: 启动 GATT 发现
       AUD->>ISO: 发现 PACS 和 ASCS,订阅 ASE
       AUD->>ISO: 配置 ASE(写 ASE 控制点)
       AUD->>ISO: 创建 CIG 并连接 CIS
       ISO->>CTRL: LE Set CIG Parameters, Create CIS
       CTRL->>ISO: CIS Established 事件
       AUD->>ISO: 建立 ISO 数据通路(输入)
       APP->>CTRL: 发送 ISO SDU
       CTRL->>ISO: ISO 发送完成
       Note over APP,CTRL: 后续 SDU 重复发送 / 发送完成的循环

流的建立 —— 发现 PACS 和 ASCS、配置各 ASE、创建 CIG、连接 CIS 和建立数据通路 —— 是由 ESP-BLE-AUDIO 规范库在 ESP-BLE-ISO 的 GATT 和 ISO 层之上驱动的，而非由应用直接驱动。应用只通过 ``esp_ble_audio_*`` API 驱动高层流程 —— 连接、启动发现、启动流和发送音频 —— 而 ESP-BLE-AUDIO 代为执行底层的 ESP-BLE-ISO 操作。

单播接受端 (Unicast Acceptor)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

单播接受端 (Acceptor) 进行广播，接受来自发起端的连接，并在接收音频之前让发起端配置其音频流端点：

.. mermaid::

   sequenceDiagram
       participant APP as Application (acceptor)
       participant AUD as ESP-BLE-AUDIO
       participant ISO as ESP-BLE-ISO
       participant CTRL as Controller
       APP->>ISO: 广播(GAP,音频通告)
       CTRL->>ISO: ACL 连接已建立(peripheral)
       CTRL->>ISO: 远端 initiator 写 ASE 控制点
       ISO->>AUD: ASCS 服务端应用 config / QoS / enable
       AUD->>APP: config / QoS / enable 回调
       APP->>AUD: 响应(接受 / 拒绝 + QoS 偏好)
       AUD->>ISO: 通知 ASE 状态和控制点结果
       ISO->>CTRL: 发送给远端 initiator
       CTRL->>ISO: CIS Established 事件
       AUD->>ISO: 建立 ISO 数据通路(输出)
       CTRL->>ISO: ISO SDU
       ISO->>APP: ISO 任务上的接收回调
       Note over APP,CTRL: 后续 ISO SDU 重复接收 / 接收回调的循环

与发起端不同，接受端是被动的。远端发起端通过写 ASE 控制点驱动 ASCS 状态机，接受端的 ESP-BLE-AUDIO ASCS 服务端把每一步 —— config、QoS、enable —— 作为回调呈现给应用。每个回调返回应用的响应 —— 接受或拒绝，外加编解码器配置时的 QoS 偏好 —— ASCS 服务端把它作为更新后的 ASE 状态和控制点结果通知回发起端。连接等时流由发起端建立（接受端是 peripheral）；ESP-BLE-AUDIO 建立 ISO 数据通路，并通过 ISO 任务上的接收回调把收到的 SDU 递交给应用。与发起端一样，应用从不直接调用 ESP-BLE-ISO 的 ISO API。

广播源 (Broadcast Source)
~~~~~~~~~~~~~~~~~~~~~~~~~~

广播源 (Broadcast Source) 配置其音频，启动一条承载 BASE 的周期性广播序列，并在没有任何连接的情况下把音频发送进一个广播等时组：

.. mermaid::

   sequenceDiagram
       participant APP as Application (broadcast source)
       participant AUD as ESP-BLE-AUDIO
       participant ISO as ESP-BLE-ISO
       participant CTRL as Controller
       APP->>AUD: 创建广播 Source(codec、QoS、BASE)
       APP->>ISO: 启动扩展和周期性广播(GAP,承载 BASE)
       APP->>AUD: 启动广播 Source
       AUD->>ISO: 创建 BIG
       ISO->>CTRL: LE Create BIG
       CTRL->>ISO: BIG Complete 事件
       AUD->>ISO: 建立 ISO 数据通路(输入)
       AUD->>APP: 流已启动回调
       APP->>CTRL: 发送 ISO SDU
       CTRL->>ISO: ISO 发送完成
       Note over APP,CTRL: 后续 SDU 重复发送 / 发送完成的循环

应用通过 ``esp_ble_audio_*`` API 驱动高层流程 —— 创建广播源、启动广播、启动广播源和发送音频 —— 而 ESP-BLE-AUDIO 构建 BASE、在 ESP-BLE-ISO 的 ISO 子系统之上创建 BIG，并建立数据通路。与单播发起端一样，底层的 ESP-BLE-ISO 操作由 ESP-BLE-AUDIO 执行，而非应用。广播源没有连接也没有 GATT：它只进行广播和发送。

广播接收端 (Broadcast Sink)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

广播接收端 (Broadcast Sink) 同步到广播端的周期性序列，加入其广播等时组并接收音频：

.. mermaid::

   sequenceDiagram
       participant APP as Application (broadcast sink)
       participant AUD as ESP-BLE-AUDIO
       participant ISO as ESP-BLE-ISO
       participant CTRL as Controller
       APP->>ISO: 扫描并同步到周期性广播
       CTRL->>ISO: BIGInfo 报告
       AUD->>ISO: BIG 同步(若加密则带广播码)
       ISO->>CTRL: LE BIG Create Sync
       CTRL->>ISO: BIG Sync Established 事件
       AUD->>ISO: 建立 ISO 数据通路(输出)
       CTRL->>ISO: ISO SDU
       ISO->>APP: ISO 任务上的接收回调
       Note over APP,CTRL: 后续 ISO SDU 重复接收 / 接收回调的循环

应用驱动高层 —— 周期性广播同步（通过 GAP）和广播接收端的启动 —— 而 ESP-BLE-AUDIO 的广播接收端规范执行 BIG 同步和数据通路。两者都依托 ESP-BLE-ISO：用它的扫描层做 PA 同步和 BIGInfo 报告，用它的 ISO 子系统做 BIG 同步和数据通路。如果该组是加密的而广播码错误，控制器仍会同步，但流很快会因消息完整性检查失败，因此应用应把同步后早期的失败视为很可能是广播码错误。

.. _arch-audio-api:

公共 API
------------------------

应用通过 ``esp_ble_audio_*`` API 驱动 ESP-IDF 蓝牙低功耗音频实现。它有两个接口面：一个公共头文件 ``api/include/esp_ble_audio_common_api.h``，用于启动和横切的入口点；以及每个规范和服务一个 ``<profile>_api.h`` 头文件，用于角色相关的操作 —— 即 API 的主体，已在 :ref:`客户端与控制器 <arch-audio-profiles>` 中编目。

形态与约定
~~~~~~~~~~~~~~~~~~~~~~~~

公共 API 位于 ESP-BLE-ISO :ref:`传输层 <arch-iso-transport>` 和预构建规范库之上，并遵循与传输层公共 API 相同的约定：

- **两次调用启动**。``esp_ble_audio_common_init`` 接收一个 ``esp_ble_audio_init_info_t`` —— 一个 GAP 回调和一个 GATT 回调，即 :ref:`应用事件接口 <arch-app-event>` 中那唯一的应用汇聚点。``esp_ble_audio_common_start`` 接收一个 ``esp_ble_audio_start_info_t``，携带要启动的协调集 (CSIS) 服务实例。这两次调用内部做了什么，是 :ref:`初始化流程 <arch-init-flow>` 的主题：与预构建库的一次握手 —— 共享结构 ABI（Application Binary Interface，应用二进制接口）检查和 Kconfig 配置推送 —— 随后是逐个服务的注册。
- **不透明、与传输层别名的类型**。应用所处理的事件类型 —— ``esp_ble_audio_gap_app_event_t`` 和 ``esp_ble_audio_gatt_app_event_t`` —— 是传输层 ``bt_le_gap_app_event`` / ``bt_le_gatt_app_event`` 的 typedef，而 ``ESP_BLE_AUDIO_GAP_EVENT_*`` / ``ESP_BLE_AUDIO_GATT_EVENT_*`` 代码是传输层代码的别名，因此音频层和 ISO 层呈现同一个事件模型。
- **错误码**。每个函数返回 ``esp_err_t``。
- **按角色注册**。一个规范角色遵循 :ref:`客户端与控制器 <arch-audio-profiles>` 中那个统一的形态：启用该角色的 Kconfig 选项，通过该角色的 ``<profile>_api.h`` 注册其回调结构体，并通过该头文件的函数驱动它。注册一个角色也正是把它的 GATT 服务添加到表中的操作（参见 :ref:`延迟注册 <arch-audio-deferred-add>`）。

功能分组
~~~~~~~~~~~~~~~~~~~~~~~~

.. list-table:: ESP-BLE-AUDIO 公共 API 按用途分类
   :header-rows: 1
   :widths: 24 42 34

   * - 分组
     - 代表性函数
     - 用途
   * - 初始化
     - ``esp_ble_audio_common_init``
     - 注册 GAP 和 GATT 回调，并启动预构建库、GAP 和 GATT。
   * - 启动
     - ``esp_ble_audio_common_start``
     - 从 ``start_info`` 注册协调集服务（CSIS、CAS），并启动 GATT 服务端（Bluedroid 上的第二个 BTA 阶段）。
   * - GATT 发现
     - ``esp_ble_audio_gattc_disc_start``
     - 在一条连接上启动对对端服务的 GATT 发现。
   * - 事件转发
     - ``esp_ble_audio_gap_app_post_event``,\ |br|\ ``esp_ble_audio_gatt_app_post_event``
     - 把主机的 GAP 和 GATT 事件转发进引擎（仅 NimBLE —— 见下文）。
   * - LTV 辅助
     - ``esp_ble_audio_data_parse``,\ |br|\ ``esp_ble_audio_data_get_val``
     - 解析蓝牙低功耗音频通篇使用的长度-类型-值元数据。
   * - 按规范的角色
     - 各 ``<profile>_api.h`` 的注册与操作函数
     - 担任并驱动一个规范角色 —— 即 API 的主体（参见 :ref:`客户端与控制器 <arch-audio-profiles>`）。

各主机集成差异
~~~~~~~~~~~~~~~~~~~~~~~~

与 ESP-BLE-ISO 一样，唯一因主机而异的入口点是事件转发函数，它们存在的原因相同 —— 让引擎看到应用所驱动连接的 GAP 和 GATT 事件：

- ``esp_ble_audio_gap_app_post_event`` 和 ``esp_ble_audio_gatt_app_post_event`` **仅在 NimBLE 上需要**：主机把 GAP 和 GATT 事件递交给应用注册的回调，应用用它们把事件向内转发。在 Bluedroid 上，适配器安装自己的 BTA/BTM 回调并直接捕获事件，因此无需转发 —— ``esp_ble_audio_gatt_app_post_event`` 在 Bluedroid 构建中甚至是隐藏的，因此在那里调用它是一个编译期错误。
