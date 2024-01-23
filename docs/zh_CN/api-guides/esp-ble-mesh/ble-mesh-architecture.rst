ESP-BLE-MESH 架构
=================

:link_to_translation:`en:[English]`

本文档将介绍 ESP-BLE-MESH 的架构概览、架构实现和辅助程序。

- ESP-BLE-MESH 架构概览

  - 描述了 ESP-BLE-MESH 架构的 5 大部分及每个部分的功能。

- ESP-BLE-MESH 架构实现

  - 描述了 ESP-BLE-MESH 文件的基本功能、文件与 ESP-BLE-MESH 架构的对应关系及文件间调用的接口。

- ESP-BLE-MESH 辅助程序

  - 描述了 ESP-BLE-MESH 的辅助程序，比如 Mesh 网络管理，Mesh 特性等。

1. ESP-BLE-MESH 架构概览
------------------------

目前，ESP-BLE-MESH 已经实现了 Mesh Profile 的大多数功能及 Mesh Model 规范中定义的所有 Client Model。未支持的功能/模型尚在开发中，会尽快提供。ESP-BLE-MESH 已通过 Bluetooth SIG 蓝牙技术联盟的 `认证 <https://launchstudio.bluetooth.com/ListingDetails/76255>`__。

.. figure:: ../../../_static/esp-ble-mesh-architecture.png
  :align: center

  图 1.1 ESP-BLE-MESH 架构图

ESP-BLE-MESH 架构主要由以下 5 大部分组成：

- ``Mesh 协议栈``

  - ``Mesh Networking`` 负责 ESP-BLE-MESH 节点的网络消息处理等。
  - ``Mesh Provisioning`` 负责 ESP-BLE-MESH 设备的启动配置流程。
  - ``Mesh Models`` 负责实现 SIG 定义的模型。

- ``网络管理``

  - 负责实现网络管理程序，包括节点删除程序、网络索引 (IV Index) 恢复程序等。

- ``特性``

  - 包括 ESP-BLE-MESH 特性，如低功耗特性、好友特性、中继特性等。

- ``Mesh 承载层``

  - 包括 ``广播承载层`` 和 ``GATT 承载层``。承载层对于 ESP-BLE-MESH 协议栈至关重要，因为协议栈基于蓝牙低功耗技术构建而成，其必须利用承载层通过 BLE 广播通道和连接通道进行数据传输。

- ``应用程序``

  - 基于 ESP-BLE-MESH 协议栈和 ``Mesh Models``。
  - 通过调用 API 和处理事件，``Applications`` 实现了与 ESP-BLE-MESH 协议栈中的 ``Mesh Networking`` 和 ``Mesh Provisioning`` 的交互，也实现了与 ``Mesh Models`` 中一系列模型的交互。


1.1 Mesh 协议栈
---------------

1.1.1 Mesh Networking
^^^^^^^^^^^^^^^^^^^^^

协议栈架构中的 ``Mesh Networking`` 实现了如下功能:

- Mesh 网络中节点间的通讯。
- Mesh 网络中消息的加解密。
- Mesh 网络资源的管理，如网络秘钥 (NetKey)、网络索引等。
- Mesh 网络消息的分包与重组。
- 消息在不同模型间的模型映射。
- 更多功能，请参见 :doc:`ble-mesh-feature-list`。

``Mesh Networking`` 功能的实现是基于层级结构的。每一层的功能如表 1.1 所示：

.. list-table:: 表 1.1  Mesh Networking 框架描述
  :widths: 40 150
  :header-rows: 1

  * - 层级
    - 功能
  * - 接入层
    - 接入层定义应用程序数据的格式，还对上层传输层对数据包的加密和解密进行定义和控制。
  * - 上层传输层
    - 上层传输层对接入层进出的应用数据进行加密、解密和认证，同时也处理被称为 “传输控制消息”的特殊消息，这种消息包括了与“友谊”和心跳包相关的消息。
  * - 底层传输层
    - 底层传输层处理 PDU 的分包和重组。
  * - 网络层
    - 网络层定义网络消息的地址类型和格式，实现设备的中继功能。

1.1.2 Mesh Provisioning
^^^^^^^^^^^^^^^^^^^^^^^

协议栈架构中的 ``Mesh Provisioning`` 实现了如下功能：

- 对未配网设备的配网。
- Mesh 网络资源的分配（单播地址、网络索引和网络秘钥）。
- 配网期间对 4 种验证方法的支持。
- 更多功能，请参见 :doc:`ble-mesh-feature-list`。

``Mesh Provisioning`` 功能的实现是基于层级结构的。每一层的功能如表 1.2 所示：

.. list-table:: 表 1.2  Mesh Provisioning 框架描述
  :widths: 40 150
  :header-rows: 1

  * - 层级
    - 功能
  * - Provisioning PDUs
    - 通过配网协议处理不同层级的 Provisioning PDUs。
  * - Generic Provisioning PDU/Proxy PDU
    - 使用 Generic Provisioning 层或代理协议层将 Provisioning PDU 传输到未配网的设备。
  * - PB-ADV/PB-GATT
    - 这些层级定义了 Provisioning PDUs 作为可分包和重组的消息进行传输的方式。
  * - Advertising/Provisioning Service
    - Provisioning bearer 定义了会话建立的方式，该方式用来将 Generic Provisioning 层的传输包传送到设备。

1.1.3 Mesh Models
^^^^^^^^^^^^^^^^^

协议栈架构中的 ``Mesh Models`` 实现了如下功能：

- Configuration Client/Server Models
- Health Client/Server Models
- Generic Client/Server Models
- Sensor Client/Server Models
- Time and Scenes Client/Server Models
- Lighting Client/Server Models

每一层的功能如表 1.3 所示：

.. list-table:: 表 1.3  Mesh Models 框架描述
  :widths: 40 150
  :header-rows: 1

  * - 层级
    - 功能
  * - 模型层
    - 模型层实现用于标准化典型用户场景操作的模型，包括 Generic Client/Server Models、Sensor Client/Server Models、Time and Scenes Client/Server Models、Lighting Client/Server Models 和若干自定义模型。
  * - 基础模型层
    - 基础模型层实现与 ESP-BLE-MESH 网络配置、管理和自我诊断等相关的模型。

1.2 Mesh 网络管理
-----------------

``网络管理`` 实现了如下功能：

- 节点移除程序：用于将节点从网络中移除。
- 网络索引恢复程序：用于恢复节点的网络索引。
- 网络索引更新程序：用于更新节点的网络索引。
- 秘钥更新程序：用于更新节点的网络秘钥、应用秘钥 (AppKey) 等。
- 网络创建程序：用于创建 mesh 网络。
- NVS 存储器：用于存储节点的网络信息。

1.3 Mesh 特性
-------------

``特性``  包括以下几项：

- 低功耗特性：用于降低节点的能耗。
- 好友特性：用于为低功耗节点存储消息。
- 中继特性：用于中继/转发节点通过广播承载层收到的网络 PDU.
- Proxy Server/Client 是代理协议中的两个节点角色，其使节点可以通过面向连接的承载层收发 Network PDUs、mesh beacons、代理配置消息和 Provisioning PDU。

1.4 Mesh 承载层
---------------

协议栈框架中的 ``承载层`` 负责 ESP-BLE-MESH 协议栈和低功耗蓝牙核心协议间的数据传输。

``承载层`` 可视为是基于蓝牙低功耗核心协议的载体层，其实现了 ESP-BLE-MESH 协议栈数据的接收和传输。

.. list-table:: 表 1.3  Mesh 承载层描述
  :widths: 40 150
  :header-rows: 1

  * - 层级
    - 功能
  * - GATT 承载层
    - GATT 承载层使用代理协议通过 GATT 连接在两个设备之间发送和接收 ``Proxy PDUs``。
  * - 广播承载层
    - 使用广播承载层时，必须使用低功耗蓝牙广播通道来发送 mesh 数据包, 数据包中的 AD Type 需要设置为 mesh 数据包的类型。

1.5 Mesh ``应用层``
--------------------

协议栈框架图中的 ``应用层`` 通过调用 ESP-BLE-MESH 协议栈提供的 API 并处理协议栈上报的事件来实现相应的功能，有一些常见应用，比如网关、照明等。

应用层和 ``API / 事件`` 之间的交互

- 应用层调用 API

  - 调用配网相关的 API 进行配网。
  - 调用模型相关的 API 发送消息。
  - 调用设备属性相关的 API 获取设备的本地信息。

- 应用层处理事件

  应用层的设计基于事件设计，事件将参数传输给应用层。事件主要分为两大类。

  - 调用 API 完成的事件。
      - 比如接收消息的节点。
  - 协议栈主动上报给应用层的事件。
      - 协议栈主动上报的事件。
      - 模型主动上报的事件。

- 事件通过应用层注册的回调函数进行上报，同时回调函数中也会包含对事件的相应处理。

``API /事件`` 与 ESP-BLE-MESH 协议栈的交互

- 用户使用的 API 主要调用 ``Mesh Networking``、 ``Mesh Provisioning`` 和 ``Mesh Models`` 提供的函数。

-  ``API /事件`` 和协议栈的交互不会跨越协议栈的层级进行操作。比如 API 不会调用 ``Network Layer`` 相关的函数。

2. ESP-BLE-MESH 架构实现
------------------------

ESP-BLE-MESH 架构的设计和实现是基于层级和模块的。具体而言，第 2.1 节（Mesh 网络的实现），第 2.2 节（Mesh 配网实现）和第 2.3 节（Mesh 层级实现）基于层级思想，第 2.4 节（网格模型的实现）基于模块思想。

- **层级思想**: 基于层级思想，网络架构根据 Mesh Profile Specification 中指定的层级设计而成。每层都有独特的文件，文件包括该层的 API 等。具体设计如图 2.1 所示。

- **模块思想**: 每个文件实现一个独立的功能，供其它程序调用。

.. figure:: ../../../_static/esp-ble-mesh-interface.png
  :align: center

  图 2.1 ESP-BLE-MESH 架构实现图

ESP-BLE-MESH 架构采用分层的方式进行设计，数据包的处理所经过的层级顺序是固定的，也就是数据包的处理过程会形成一个 ``消息流``。因此，我们可以从图 2.1 的协议栈接口图中看到消息流。

2.1 Mesh 协议栈的实现
---------------------

2.1.1 Mesh Networking 实现
^^^^^^^^^^^^^^^^^^^^^^^^^^

``Mesh Networking`` 中的文件列表和每个文件实现的功能如表 2.1 所示：

.. list-table:: 表 2.1  Mesh Networking 文件描述
  :widths: 40 150
  :header-rows: 1

  * - 文件
    - 功能
  * - :component_file:`access.c <bt/esp_ble_mesh/core/access.c>`
    - ESP-BLE-MESH 接入层
  * - :component_file:`transport.c <bt/esp_ble_mesh/core/transport.c>`
    - ESP-BLE-MESH 底层/上层传输层
  * - :component_file:`net.c <bt/esp_ble_mesh/core/net.c>`
    - ESP-BLE-MESH 网络层
  * - :component_file:`adv.c <bt/esp_ble_mesh/core/adv.c>`
    - 用于发送 ESP-BLE-MESH 广播包的任务，一个用于处理收到的广播包的回调以及用于分配 adv 缓冲区的 API

2.1.2 Mesh Provisioning 实现
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

由于 Node/Provisioner 共存的限制，Mesh Provisioning 的实现分为两大模块。

实现 Node 启动配置的特定文件如表 2.2 所示：

.. list-table:: 表 2.2  Mesh Provisioning（节点）文件描述
  :widths: 40 150
  :header-rows: 1

  * - 文件
    - 功能
  * - :component_file:`prov_node.c <bt/esp_ble_mesh/core/prov_node.c>`
    - ESP-BLE-MESH 节点配网 (PB-ADV & PB-GATT)
  * - :component_file:`proxy_server.c <bt/esp_ble_mesh/core/proxy_server.c>`
    - ESP-BLE-MESH 节点代理服务器相关功能
  * - :component_file:`beacon.c <bt/esp_ble_mesh/core/beacon.c>`
    - 用于处理 ESP-BLE-MESH Beacon 的 API

实现 Provisioner 配置功能的特定文件如表 2.3 所示：

.. list-table:: 表 2.3  Mesh Provisioning (Provisioner) 文件描述
  :widths: 40 150
  :header-rows: 1

  * - 文件
    - 功能
  * - :component_file:`prov_pvnr.c <bt/esp_ble_mesh/core/prov_pvnr.c>`
    - ESP-BLE-MESH Provisioner 配置入网 (PB-ADV & PB-GATT)
  * - :component_file:`proxy_client.c <bt/esp_ble_mesh/core/proxy_client.c>`
    - ESP-BLE-MESH 代理客户端相关功能
  * - :component_file:`pvnr_mgmt.c <bt/esp_ble_mesh/core/pvnr_mgmt.c>`
    - ESP-BLE-MESH Provisioner 网络相关功能

2.1.3 Mesh Models 实现
^^^^^^^^^^^^^^^^^^^^^^

Mesh Models 用于实现节点中所包含的模型的具体功能。服务器模型主要用于维护节点状态。客户端模型主要用于获取和修改节点状态。

.. list-table:: Table 2.4  Mesh Models 文件描述
  :widths: 40 150
  :header-rows: 1

  * - 文件
    - 功能
  * - :component_file:`cfg_cli.c <bt/esp_ble_mesh/core/cfg_cli.c>`
    - 发送 Configuration Client 消息，接收相应应答消息
  * - :component_file:`cfg_srv.c <bt/esp_ble_mesh/core/cfg_srv.c>`
    - 接收 Configuration Client 消息，发送适当应答消息
  * - :component_file:`health_cli.c <bt/esp_ble_mesh/core/health_cli.c>`
    - 发送 Health Client 消息，接收相应应答消息
  * - :component_file:`health_srv.c <bt/esp_ble_mesh/core/health_srv.c>`
    - 接收 Health Client 消息，发送适当应答消息
  * - :component_file:`client_common.c <bt/esp_ble_mesh/models/client/client_common.c>`
    - ESP-BLE-MESH 模型相关操作
  * - :component_file:`generic_client.c <bt/esp_ble_mesh/models/client/generic_client.c>`
    - 发送 ESP-BLE-MESH Generic Client 消息，接收相应应答消息
  * - :component_file:`lighting_client.c <bt/esp_ble_mesh/models/client/lighting_client.c>`
    - 发送 ESP-BLE-MESH Lighting Client 消息，接收相应应答消息
  * - :component_file:`sensor_client.c <bt/esp_ble_mesh/models/client/sensor_client.c>`
    - 发送 ESP-BLE-MESH Sensor Client 消息，接收相应应答消息
  * - :component_file:`time_scene_client.c <bt/esp_ble_mesh/models/client/time_scene_client.c>`
    - 发送 ESP-BLE-MESH Time Scene Client 消息，接收相应应答消息
  * - :component_file:`generic_server.c <bt/esp_ble_mesh/models/server/generic_server.c>`
    - 发送 ESP-BLE-MESH Generic Client 消息，发送相应应答消息
  * - :component_file:`lighting_server.c <bt/esp_ble_mesh/models/server/lighting_server.c>`
    - 接收 ESP-BLE-MESH Lighting Client 消息，发送相应应答消息
  * - :component_file:`sensor_server.c <bt/esp_ble_mesh/models/server/sensor_server.c>`
    - 接收 ESP-BLE-MESH Sensor Client 消息，发送相应应答消息
  * - :component_file:`time_scene_server.c <bt/esp_ble_mesh/models/server/time_scene_server.c>`
    - 接收 ESP-BLE-MESH Time Scene Client 消息，发送相应应答消息

2.2 Mesh Bearers 实现
^^^^^^^^^^^^^^^^^^^^^

Mesh Bearers 在实现时充分考虑了可移植性。当 ESP-BLE-MESH 协议栈需要移植到其它平台时，用户只需要修改 :component_file:`adapter.c <bt/esp_ble_mesh/core/bluedroid_host/adapter.c>` 就能移植成功。

.. list-table:: 表 2.5  Mesh Bearers 文件描述
  :widths: 40 150
  :header-rows: 1

  * - 文件
    - 功能
  * - :component_file:`adapter.c <bt/esp_ble_mesh/core/bluedroid_host/adapter.c>`
    - ESP-BLE-MESH 承载层适配文件。此文件提供用于接收和发送 ESP-BLE-MESH ADV 和 GATT 相关数据包的接口。

.. note::

  :component_file:`adapter.c <bt/esp_ble_mesh/core/bluedroid_host/adapter.c>` 是对 Mesh 网络框架中 ``Advertising Bearer`` 和 ``GATT Bearer`` 的实现。

2.3 Mesh Applications 实现
^^^^^^^^^^^^^^^^^^^^^^^^^^

我们提供了一系列用于客户开发的应用示例，用户可以基于 :ref:`esp-ble-mesh-examples` 开发产品。

1. ESP-BLE-MESH 辅助程序
-------------------------

辅助程序指的是 ESP-BLE-MESH 协议栈中可选的功能。辅助程序的设计通常通过 :ref:`CONFIG_BLE_MESH` 来实现代码的裁剪。

3.1 特性
^^^^^^^^

- 低功耗
- 好友
- 中继
- 代理客户端/代理服务器

3.2 网络管理
^^^^^^^^^^^^

- 节点移除程序
- 网络索引恢复程序
- 网络索引更新程序
- 秘钥更新程序
- 网络创建程序
- NVS 存储器

3.3 辅助程序实现
^^^^^^^^^^^^^^^^

采用独立模块的设计主要考虑到两个因素：

- 该模块不具备分层实现的条件，其实现可以完全独立，不需要依赖其它模块。
- 模块中的函数会被反复使用到，因此最好设计成独立模块。独立模块如表 3.1 所示：

.. list-table:: 表 3.1 模块文件描述
  :widths: 40 150
  :header-rows: 1

  * - 文件
    - 功能
  * - :component_file:`lpn.c <bt/esp_ble_mesh/core/lpn.c>`
    - ESP-BLE-MESH 低功耗功能
  * - :component_file:`friend.c <bt/esp_ble_mesh/core/friend.c>`
    - ESP-BLE-MESH 好友功能
  * - :component_file:`net.c <bt/esp_ble_mesh/core/net.c>`
    - ESP-BLE-MESH 中继功能、网络创建、网络索引更新程序、网络索引恢复程序、秘钥更新程序相关功能
  * - :component_file:`proxy_server.c <bt/esp_ble_mesh/core/proxy_server.c>`
    - ESP-BLE-MESH 代理服务器相关功能
  * - :component_file:`proxy_client.c <bt/esp_ble_mesh/core/proxy_client.c>`
    - ESP-BLE-MESH 代理客户端相关功能
  * - :component_file:`settings.c <bt/esp_ble_mesh/core/storage/settings.c>`
    - ESP-BLE-MESH NVS 存储器功能
  * - :component_file:`main.c <bt/esp_ble_mesh/core/main.c>`
    - ESP-BLE-MESH 协议栈初始化，协议栈使能，节点移除相关功能
