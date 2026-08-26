ESP-BLE-MESH Terminology
========================

:link_to_translation:`en:[English]`

.. _ble-mesh-terminology-role:

.. list-table:: 表 1 ESP-BLE-MESH 术语 - 身份
  :widths: 10 40 60
  :header-rows: 1

  * - 术语
    - 官方定义
    - 详细说明
  * - 未配网设备
    - "A device that is not a member of a mesh network is known as an unprovisioned device."
    - 示例：照明装置、温控设备、制造设备和电动门等。
  * - 节点
    - "A node is a provisioned device."
    - 经配网加入 ESP-BLE-MESH 网络后，未配网设备的身份转变成节点。节点（如照明装置、温控设备、制造设备和电动门）是指能在蓝牙 ESP-BLE-MESH 网络中发送、接收或中继消息的设备，且节点可以选择性地支持一个或多个子网。
  * - 中继节点
    - "A node that supports the Relay feature and has the Relay feature enabled is known as a Relay node."
    - 中继节点接收并中继 ESP-BLE-MESH 消息，因此消息可以传输得更远。用户可以根据节点的状态来决定是否使能节点的中继功能。消息可以中继多次，每次中继为“一跳”，消息最多可有 126 跳，足以让消息在广阔的区域内传输。
  * - 代理节点
    - "A node that supports the Proxy feature and has the Proxy feature enabled is known as a Proxy node."
    - 代理节点从一个承载层（通常包括广播承载层和 GATT 承载层）接收消息，并通过另一个承载层重新发送消息。其目的是将只支持 GATT 承载层的通讯设备接入到 ESP-BLE-MESH 网络中。通常而言，手机 App 需要一个代理节点才能接入 Mesh 网络。没有代理节点，手机 App 无法与 Mesh 网络中成员通信。
  * - 好友节点
    - "A node that supports the Friend feature, has the Friend feature enabled, and has a friendship with a node that supports the Low Power feature is known as a Friend node."
    - 好友节点相当于低功耗节点（LPN）的备份，可存储发往低功耗节点的消息及安全更新信息；当低功耗节点需要这些存储的信息时，这些信息便会被传输至低功耗节点。低功耗节点必须与支持好友特性的另一节点建立“友谊”，以减少其接收器的占空比，从而降低低功耗节点的功耗。低功耗节点需要找到好友节点，与其建立友谊关系，其中涉及的过程称为“友谊建立”。低功耗节点与好友节点的搭配可让低功耗节点规划对无线电的使用，从而以适当或较低的频率接收消息，无需保持收听状态。低功耗节点会轮询好友节点以查看是否有新的消息。
  * - 低功耗节点
    - "A node that supports the Low Power feature and has a friendship with a node that supports the Friend feature is known as a Low Power node."
    - 低功耗节点通过轮询从好友节点获取信息，比如消息、安全更新等。
  * - 启动配置设备（以下称为 Provisioner）
    - "A node that is capable of adding a device to a mesh network."
    - 能够配网未配网设备的设备称为启动配置设备。这一流程通常需要通过产品制造商的提供的 App 来实现，并可在网关、智能手机、平板电脑和其他载体上使用。


.. _ble-mesh-terminology-composition:

.. list-table:: 表 2 ESP-BLE-MESH 术语 - 节点构成
  :widths: 10 40 60
  :header-rows: 1

  * - 术语
    - 官方定义
    - 详细说明
  * - 状态
    - "A value representing a condition of an element that is exposed by an element of a node."
    - ESP-BLE-MESH 网络中的每台设备都具有一组独立的状态值，表示设备的某些状态，比如照明设备的亮度、颜色等状态。更改状态值会修改设备本身的物理状态，比如更改设备的开关状态值实际是在打开或关闭设备。
  * - 模型
    - "A model defines the basic functionality of a node."
    - 一个节点可能包含多个模型，而每个模型定义了节点的基本功能，比如节点所需要的状态、控制状态的消息以及处理消息所产生的动作等。节点功能的实现是基于模型的，模型可分为 SIG 模型和自定义模型，前者由 SIG 定义，而后者由用户定义。
  * - 元素
    - "An addressable entity within a device."
    - 一个节点可以包含一个或多个元素，每个元素都有一个单播地址和一个或多个模型，并且同一元素所包含的模型不可以出现重复。
  * - 节点构成状态
    - "The Composition Data state contains information about a node, the elements it includes, and the supported models."
    - 通过读取节点构成状态的值，用户可以了解节点的基本信息，比如元素的数量及每个元素中的模型。Provisioner 通过获取这个消息对设备进一步配置，比如配置节点的订阅地址与发布地址。


.. _ble-mesh-terminology-features:

.. list-table:: 表 3 ESP-BLE-MESH 术语 - 特性
  :widths: 10 40 60
  :header-rows: 1

  * - 术语
    - 官方定义
    - 详细说明
  * - 低功耗特性
    - "The ability to operate within a mesh network at significantly reduced receiver duty cycles only in conjunction with a node supporting the Friend feature."
    - 低功耗功能可降低节点的功耗。当低功耗节点寻找好友节点、且附近有多个好友节点时，它会通过算法选择最适合的好友节点。
  * - 好友特性
    - "The ability to help a node supporting the Low Power feature to operate by storing messages destined for those nodes."
    - 通过使能好友特性，节点可以帮助存储低功耗节点的信息。使能了好友特性的节点可能会产生更大的功耗和内存消耗。
  * - 中继特性
    - "The ability to receive and retransmit mesh messages over the advertising bearer to enable larger networks."
    - 中继特性能让 ESP-BLE-MESH 的消息在节点之间实现多次跳跃，传输距离可超过两个节点之间直接进行无线电传输的范围，从而覆盖整个网络。使能了中继特性的节点中继消息时，只中继其所在子网的消息，不中继其它子网的消息。使能了中继特性的节点中继分段消息时不考虑数据的完整性。节点每收到一条分段消息便直接中继，不会等待收到完整的消息。
  * - 代理特性
    - "The ability to receive and retransmit mesh messages between GATT and advertising bearers."
    - 代理特性的目的是允许不具备广播承载层的节点访问 ESP-BLE-MESH 网络。代理特性通常为需要和手机 App 连接的节点所用。


.. _ble-mesh-terminology-provisioning:

.. list-table:: 表 4 ESP-BLE-MESH 术语 - 配置入网
  :widths: 10 40 60
  :header-rows: 1

  * - 术语
    - 官方定义
    - 详细说明
  * - PB-ADV
    - "PB-ADV is a provisioning bearer used to provision a device using Generic Provisioning PDUs over the advertising channels."
    - PB-ADV 通过广播通道传输配网过程中产生的数据包。只有 Provisioner 和未配网设备都支持 PB-ADV 时才可使用这种方法进行配网。
  * - PB-GATT
    - "PB-GATT is a provisioning bearer used to provision a device using Proxy PDUs to encapsulate Provisioning PDUs within the Mesh Provisioning Service."
    - PB-GATT 通过连接通道传输配网过程中产生的数据包。如果未配网设备想使用此方式进行配网，其需要实现相关的 Mesh Provisioning Service。未实现此服务的未配网设备不能通过 PB-GATT 承载层配网接入 mesh 网络。
  * - PB-Remote
    - "The PB-Remote provisioning bearer uses the existing mesh network to provision an unprovisioned device that is not within immediate radio range of the Provisioner."
    - PB-Remote 是一种使用现有的网状网络来为无法与配网器直接通信的未配网设备进行配网的配网承载方式。
  * - 配置入网
    - "Provisioning is a process of adding an unprovisioned device to a mesh network, managed by a Provisioner."
    - 经过配网，“未配网设备”的身份转变为“节点”，成为 ESP-BLE-MESH 网络中的一员。
  * - 认证方式
    - "Authentication is a step during the provisioning of nodes."
    - 未配网设备有四种认证方法：输入带外 (Input OOB)、输出带外 (Output OOB)、静态带外 (Static OOB) 和无带外 (No OOB)。
  * - 输入带外 (Input OOB)
    - Input Out-of-Band
    - 比如，Provisioner 生成并显示随机数，然后提示用户采取适当操作将随机数输入未配网的设备中。以照明开关为例，用户可以在一定时间内数次按下按钮，以这种形式输入 Provisioner 端显示的随机数。输入带外认证方法与输出带外的认证方法类似，但设备的角色相反。
  * - 输出带外 (Output OOB)
    - Output Out-of-Band
    - 比如，未配网设备会选择一个随机数，并通过与其功能兼容的方式输出该数字。如果未配网设备是一个灯泡，则其能够闪烁指定的次数。如果未配网设备有 LCD 屏幕，则可以将随机数显示为多位数值。启动 Provisioner 的用户需要输入观察到的数字，来认证未配网的设备。
  * - 静态带外 (Static OOB)
    - Static Out-of-Band
    - 静态 OOB 的认证方法：使用静态 OOB 信息。如果需要使用无 OOB 信息，请将静态 OOB 字段赋值为 0。如果需要使用 OOB 信息，请使用静态 OOB 信息认证正在配网的设备。
  * - 无带外 (No OOB)
    - No Out-of-Band
    - 无 OOB 的认证方法：将“静态 OOB”字段赋值为 0。采用这种方式相当于不认证未配网的设备。
  * - 基于证书的配网 (Certificate-based Provisioning)
    - Certificate-based Out-of-Band
    - 基于证书的配网功能利用公钥基础设施来验证未配网设备的公钥和UUID信息。


.. _ble-mesh-terminology-address:

.. list-table:: 表 5 ESP-BLE-MESH 术语 - 地址
  :widths: 10 40 60
  :header-rows: 1

  * - 术语
    - 官方定义
    - 详细说明
  * - 未分配地址
    - "This is a special address type, with a value of 0x0000. Its use indicates that an Element has not yet been configured or had a Unicast Address assigned to it."
    - 未配置的元素地址或未分配的元素地址都称为未分配地址。鉴于这些元素没有固定的地址，它们不会用于消息的传输。建议在设置用户代码的地址之前，将该地址的值设为未分配地址。
  * - 单播地址
    - "A unicast address is a unique address allocated to each element."
    - 在配网期间，Provisioner 会给网络中处于生命周期内节点的每个元素分配一个单播地址。单播地址可能会出现在消息的源/目标地址字段中。发送到单播地址的消息只能由拥有该单播地址的元素进行处理。
  * - 虚拟地址
    - "A virtual address represents a set of destination addresses. Each virtual address logically represents a Label UUID, which is a 128-bit value that does not have to be managed centrally."
    - 虚拟地址与特定的 UUID 标签相关联，可以用作模型的发布地址或订阅地址。UUID 标签是与一个或多个节点的元素相关联的 128 位值。虚拟地址的第 15 位和第 14 位分别设置为 1 和 0。从第 13 位到第 0 位设置为散列值（提供 16384 个散列值）。散列是 UUID 标签的派生。使用订阅元素检查完整的 128 位 UUID 十分低效，而散列值提供了一种更有效的方法来确定最终将哪些消息发送到哪些元素。
  * - 群组地址
    - "A group address is an address that is programmed into zero or more elements."
    - 群组地址是 ESP-BLE-MESH 网络中的另一种多播地址，通常用于将节点进行分组。发送到 all-proxies 地址的信息应由启用了代理功能的所有节点的主要元素处理。 发送到 all-friends 地址的消息应由启用了好友功能的所有节点的主要元素处理。 发送到 all-relays 地址的消息应由启用了中继功能的所有节点的主要元素处理。 发送到 all-nodes 地址的消息应由所有节点的主要元素处理。


.. _ble-mesh-terminology-security:

.. list-table:: 表 6 ESP-BLE-MESH 术语 - 安全
  :widths: 10 40 60
  :header-rows: 1

  * - 术语
    - 官方定义
    - 详细说明
  * - 设备密钥 (DevKey)
    - "There is also a device key, which is a special application key that is unique to each node, is known only to the node and a Configuration Client, and is used to secure communications between the node and a Configuration Client."
    - 设备密钥让您能够配网未配网设备、配置节点。设备密钥用来加密配置信息，即配置设备时 Provisioner 和节点之间传输的消息。
  * - 应用密钥 (AppKey)
    - "Application keys are used to secure communications at the upper transport layer."
    - 应用密钥用于应用数据传递至应用层过程中对应用数据的解密，和应用层下发过程中对数据的加密。网络中的一些节点有特定的用途，并且可以根据应用程序的需求对一些潜在敏感数据的访问进行限制。通过特定的应用密钥，这些节点与特定应用程序相关联。通常而言，使用不同应用密钥的领域有安全（楼宇门禁、机房门禁和 CEO 办公室门禁）、照明（工厂、外部楼宇和人行道）和 HVAC 系统。应用密钥绑定在网络密钥上，这意味着应用密钥仅在绑定网络密钥的情况下使用。每一个应用密钥仅可绑定到一个网络密钥。
  * - 泛洪安全资料
    - "The master security material is derived from the network key (NetKey) and can be used by other nodes in the same network. Messages encrypted with master security material can be decoded by any node in the same network. "
    - 使用好友安全材料加密的相应友谊消息有：1. 好友轮询 (Friend Poll)，2. 好友更新 (Friend Update)，3. 好友订阅列表 (Friend Subscription List)，添加/删除/确认，4. 好友节点发送到低功耗节点的“已存储消息”，使用泛洪安全材料加密的相应友谊消息有：1. 好友清除 (Friend Clear)，2. 好友清除确认 (Friend Clear Confirm)。根据应用程序的设置，从低功耗节点发送到好友节点的消息会使用友谊安全材料或泛洪安全材料进行加密，前者用于低功耗节点与好友节点之间的消息传输，而后者用于其他网络消息。
  * - 定向安全资料
    - "The directed security material is derived from the network key (NetKey) and can be used by other nodes in the directed forwarding path."
    - 定向安全资料是从网络密钥（NetKey）派生的，可以被定向转发路径中的其他节点使用。使用定向安全资料加密的消息可以被同一定向转发路径中的任何节点解码。

.. _ble-mesh-terminology-message:

.. list-table:: 表 7 ESP-BLE-MESH 术语 - 消息
  :widths: 10 40 60
  :header-rows: 1

  * - 术语
    - 官方定义
    - 详细说明
  * - 重组 / 分包
    - "Segmentation and reassembly (SAR) is a method of communication network, which is divided into small units before transmitting packets and reassembled in a proper order at the communication receiving end."
    - 底层传输层会自动分包过大的消息。接收端会回复一条应答消息，根据应答消息，发送端会重新向接收端发送其未接收到的数据包。这些都是底层传输层自动完成的。未分包的消息最多携带 15 个字节，其中 4 个字节是 transMIC，所以剩余 11 个字节；在分包的情况下，前面的包中每包有 12 个有效字节，最后一个包中有 8 个有效字节。特殊情况：一个较短的包需要底层传输端强制分包，这种情况下有 8 个有效字节。
  * - 无应答 / 有应答
    - "There are two types of messages: Unacknowledged or Acknowledged."
    - 根据接收端是否需要发送应答消息，发送的消息可分为两种。发送端需要设置最大重传次数。

.. _ble-mesh-terminology-foundation-models:

.. list-table:: 表 8 ESP-BLE-MESH 术语 - 基础模型
  :widths: 10 40 60
  :header-rows: 1

  * - 术语
    - 官方定义
    - 详细说明
  * - Configuration Server Model
    - "This model is used to represent a mesh network configuration of a device."
    - 节点必须包含 Configuration Server Model，其负责维护配置相关的状态。Configuration Server Model 维护的状态包含：网络密钥名单 (NetKey List)、应用密钥名单 (AppKey List)、模型绑定的应用密钥名单 (Model to AppKey List)、节点身份 (Node Identity)、密钥更新阶段 (Key Refresh Phase)、心跳消息发布 (Heartbeat Publish)、心跳消息订阅 (Heartbeat Subscription)、网络传输 (Network Transmit) 和中继重传 (Relay Retransmit) 等。
  * - Configuration Client Model
    - "The model is used to represent an element that can control and monitor the configuration of a node."
    - Configuration Client Model 通过消息控制 Configuration Server Model 维护的状态。Provisioner 必须包含 Configuration Client Model，有了该模型才可发送 "Configuration Composition Data Get" 等配置消息。
  * - Health Server Model
    - "This model is used to represent a mesh network diagnostics of a device."
    - Health Server Model 主要用于设备检查自身状态，查看自身是否发生错误。Health Server model 维护的状态包含：当前故障 (Current Fault)、已登记故障 (Registered Fault)、健康周期 (Health Period) 和关注计时器 (Attention Timer)。
  * - Health Client Model
    - "The model is used to represent an element that can control and monitor the health of a node."
    - Health Client Model 通过消息控制 Health Server Model 维护的状态。该模型可通过消息 “Health Fault Get” 获取其他节点的自检信息。
  * - Remote Provisioning Server model
    - "The model is used to support the functionality of provisioning a remote device over the mesh network and to perform the Node Provisioning Protocol Interface procedures."
    - 该模型用于支持通过网状网络对远程设备进行供应，并执行节点供应协议接口程序。
  * - Remote Provisioning Client model
    - "The model is used to support the functionality of provisioning devices into a mesh network by interacting with a mesh node that supports the Remote Provisioning Server model."
    - 该模型用于与支持远程供应服务器模型的网状节点进行交互，以支持将设备供应到网状网络的功能。
  * - Directed Forwarding Configuration Server model
    - "The model is used to support the configuration of the directed forwarding functionality of a node."
    - 该模型用于支持节点的定向转发功能的配置。
  * - Directed Forwarding Configuration Client model
    - "The model is used to support the functionality of a node that can configure the directed forwarding functionality of another node."
    - 该模型用于支持一个节点配置另一个节点的定向转发功能的功能。
  * - Bridge Configuration Server model
    - "The model is used to support the configuration of the subnet bridge functionality of a node."
    - 该模型用于支持节点的子网桥接功能的配置。
  * - Bridge Configuration Client model
    - "The model is used to support the functionality of a node that can configure the subnet bridge functionality of another node."
    - 该模型用于支持一个节点配置另一个节点的子网桥接功能的功能。
  * - Mesh Private Beacon Server model
    - "The model is used to support the configuration of the Mesh Private beacons functionality of a node."
    - 该模型用于支持节点的 Mesh 私有信标功能的配置。
  * - Mesh Private Beacon Client model
    - "The model is used to support the functionality of a node that can configure the Mesh Private beacons functionality of another node."
    - 该模型用于支持一个节点配置另一个节点的 Mesh 私有信标功能的功能。
  * - On-Demand Private Proxy Server model
    - "The model is used to support the configuration of the advertising with Private Network Identity type functionality of a node."
    - 该模型用于支持节点的私有网络身份类型广告配置功能。
  * - On-Demand Private Proxy Client model
    - "The model is used to support the functionality of a node that can configure the advertising with Private Network Identity type functionality of another node."
    - 该模型用于支持一个节点配置另一个节点的私有网络身份类型广告功能的功能。
  * - SAR Configuration Server model
    - "The model is used to support the configuration of the segmentation and reassembly behavior of a node."
    - 该模型用于支持节点的分段和重组行为的配置。
  * - SAR Configuration Client model
    - "The SAR Configuration Client model is used to support the functionality of configuring the behavior of the lower transport layer of a node that supports the SAR Configuration Server model."
    - SAR配置客户端模型用于支持配置支持 SAR 配置服务器模型的节点的较低传输层行为的功能。
  * - Solicitation PDU RPL Configuration Server model
    - "The Solicitation PDU RPL Configuration Server model is used to support the functionality of removing items from the solicitation replay protection list of a node."
    - Solicitation PDU RPL 配置服务器模型用于支持从节点的请求重放保护列表中移除项目的功能。
  * - Solicitation PDU RPL Configuration Client model
    - "The model is used to support the functionality of removing addresses from the solicitation replay protection list of a node that supports the Solicitation PDU RPL Configuration Server model."
    - 该模型用于支持支持 Solicitation PDU RPL 配置服务器模型的节点从其请求重放保护列表中移除地址的功能。
  * - Opcodes Aggregator Server model
    - "The model is used to support the functionality of processing a sequence of access layer messages."
    - 该模型用于支持处理一系列访问层消息的功能。
  * - Opcodes Aggregator Client model
    - "The model is used to support the functionality of dispatching a sequence of access layer messages to nodes supporting the Opcodes Aggregator Server model."
    - 该模型用于支持将一系列访问层消息分派给支持 Opcodes Aggregator Server 模型的节点的功能。
  * - Large Composition Data Server model
    - "The model is used to support the functionality of exposing pages of Composition Data that do not fit in a Config Composition Data Status message and to expose metadata of the model instances."
    - 该模型用于支持暴露不适应于 Config Composition Data Status 消息中的组成数据页的功能，并暴露模型实例的元数据。
  * - Large Composition Data Client model
    - "The model is used to support the functionality of reading pages of Composition Data that do not fit in a Config Composition Data Status message and reading the metadata of the model instances."
    - 该模型用于支持读取不适应于 Config Composition Data Status 消息中的组成数据页的功能，并读取模型实例的元数据。


.. _ble-mesh-terminology-network-management:

.. list-table:: 表 9 ESP-BLE-MESH 术语 - 网络管理
  :widths: 10 40 60
  :header-rows: 1

  * - 术语
    - 官方定义
    - 详细说明
  * - 密钥更新程序
    - "This procedure is used when the security of one or more network keys and/or one or more of the application keys has been compromised or could be compromised."
    - 密钥更新程序用于更新 ESP-BLE-MESH 网络的网络密钥和应用密钥。当一个或多个网络密钥和/或一个或多个应用密钥的安全受到威胁或可能受到威胁时，会启动密钥更新程序。通常而言，在网络中某些节点移除后可以进行密钥更新。
  * - IV 更新程序
    - "A node can also use an IV Update procedure to signal to peer nodes that it is updating the IV Index."
    - IV 更新程序用于更新 ESP-BLE-MESH 网络的 IV Index 的值，这个值和消息加密时所需的随机数相关。为了保证随机数的值不重复，所以将这个值定期增加。IV Index 是一个 32 位的值，是一种共享网络资源，比如一个 mesh 网中的所有节点共享一个 IV Index 值。IV Index 从 0x00000000 开始，在 IV 更新过程中递增，并由特定的进程维护，以保证整个 Mesh 网内共享一个 IV Index。当节点认为它有耗尽其序列号的风险，或它确定另一个节点即将耗尽其序列号时，可以启动该程序。注意：每次的更新时间不得低于 96 小时。节点接收到 secure network beacon 或者确定自己的序列号大于特定值时，会触发 IV 更新程序。

官方定义摘自 `ESP-BLE-MESH Glossary of Terms <https://www.bluetooth.com/learn-about-bluetooth/recent-enhancements/mesh/mesh-glossary/>`_.
查看更多术语，也请参照上述网址。
