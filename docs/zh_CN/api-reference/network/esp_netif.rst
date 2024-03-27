ESP-NETIF
=========

:link_to_translation:`en:[English]`

ESP-NETIF 库的作用主要体现在以下两方面：

- 在 TCP/IP 协议栈之上为应用程序提供抽象层，允许应用程序在 IP 栈间选择。
- 在底层 TCP/IP 协议栈 API 非线程安全的情况下，也能提供线程安全的 API。

ESP-IDF 目前只为 lwIP TCP/IP 协议栈实现了 ESP-NETIF。然而，该适配器本身不依赖特定 TCP/IP 实现，因而支持不同实现方式。

ESP-IDF 支持实现了 BSD API 的自定义 TCP/IP 协议栈。有关不使用 lwIP 时构建 ESP-IDF 的详细信息，请参阅 :idf_file:`components/esp_netif_stack/README.md`。

部分 ESP-NETIF 的 API 函数可以被应用程序直接调用，如：获取或设置接口 IP 地址、配置 DHCP 等。其他 ESP-NETIF 的 API 函数则供网络驱动层在 ESP-IDF 内部调用。

应用程序通常无需直接调用 ESP-NETIF 的 API，它们会由默认网络事件句柄调用。


.. _esp-netif structure:

ESP-NETIF 架构
----------------------

.. code-block:: text


                         |            (A) 用户代码                 |
                         |                 Apps                   |
        .................| 初始化           设置          事件      |
        .                +----------------------------------------+
        .                   .                |           *
        .                   .                |           *
    --------+            +===========================+   *     +-----------------------+
            |            | new/config   get/set/apps |   *     | init                  |
            |            |                           |...*.....| Apps (DHCP, SNTP)     |
            |            |---------------------------|   *     |                       |
      初始化 |            |                           |****     |                       |
      启动   |************|    事件句柄                |*********|  DHCP                 |
      停止   |            |                           |         |                       |
            |            |---------------------------|         |                       |
            |            |                           |         |    NETIF              |
      +-----|            |                           |         +-----------------+     |
      | glue|---<----|---|  esp_netif_transmit       |--<------| netif_output    |     |
      |     |        |   |                           |         |                 |     |
      |     |--->----|---|  esp_netif_receive        |-->------| netif_input     |     |
      |     |        |   |                           |         + ----------------+     |
      |     |...<....|...|  esp_netif_free_rx_buffer |...<.....| packet buffer         |
      +-----|     |  |   |                           |         |                       |
            |     |  |   |                           |         |         (D)           |
      (B)   |     |  |   |          (C)              |         +-----------------------+
    --------+     |  |   +===========================+
      通信         |  |                                               网络堆栈
      驱动         |  |           ESP-NETIF
                  |  |                                         +------------------+
                  |  |   +---------------------------+.........| 开启/关闭         |
                  |  |   |                           |         |                  |
                  |  -<--|  l2tap_write              |-----<---|  写入             |
                  |      |                           |         |                  |
                  ---->--|  esp_vfs_l2tap_eth_filter |----->---|  读取             |
                         |                           |         |                  |
                         |            (E)            |         +------------------+
                         +---------------------------+
                                                                     用户代码
                               ESP-NETIF L2 TAP


图中不同线段对应的数据流和事件流
-----------------------------------------------

* ``........``     从用户代码到 ESP-NETIF 和通信驱动的初始化线。

* ``--<--->--``    数据包在通信媒介与 TCP/IP 协议栈之间往返。

* ``********``     聚集在 ESP-NETIF 中的事件传递到驱动程序、用户代码和网络堆栈中。

* ``|``            用户设置及运行时间配置。

ESP-NETIF 交互
---------------------

A) 用户代码样板
^^^^^^^^^^^^^^^^^^^^^^^^^^

通过使用 ESP-NETIF API 抽象，应用程序与用于通信介质的特定 IO 驱动程序以及配置的 TCP/IP 网络栈之间的交互可以概括如下：

A) 初始化代码

  1) 初始化 IO 驱动
  2) 创建新的 ESP-NETIF 实例，并完成以下配置：

    * ESP-NETIF 的特定选项（flags、行为、名称）
    * 网络栈堆选项（netif init 和 input 函数，非公开信息）
    * IO 驱动的特定选项（发送、释放 rx 缓冲区功能、IO 驱动句柄）

  3) 将 IO 驱动句柄附加到上述步骤所创建的 ESP-NETIF 实例
  4) 配置事件句柄

    * 对 IO 驱动定义的公共接口使用默认句柄；或为定制的行为或新接口定义特定的句柄
    * 为应用程序相关事件（如 IP 丢失或获取）注册句柄

B) 通过 ESP-NETIF API 与网络接口交互

  1) 获取、设置 TCP/IP 相关参数（如 DHCP，IP 等）
  2) 接收 IP 事件（连接或断连）
  3) 控制应用程序生命周期（启用或禁用接口）


B) 通信驱动、IO 驱动和媒介驱动
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

对于 ESP-NETIF，通信驱动具有以下两个重要作用：

1) 事件句柄：定义与 ESP-NETIF 交互的行为模式（如：连接以太网 -> 开启 netif）

2) 胶合 IO 层：调整输入或输出函数以使用 ESP-NETIF 的传输、接收，并清空接收缓冲区

  * 给适当的 ESP-NETIF 对象安装 driver_transmit，以便将网络堆栈中传出的数据包传输给 IO 驱动
  * 调用函数 :cpp:func:`esp_netif_receive()` 以便将传入的数据传输给网络堆栈


C) ESP-NETIF
^^^^^^^^^^^^

ESP-NETIF 是 IO 驱动和网络堆栈间的媒介，用于连通两者之间的数据包传输路径。它提供了一组接口，用于在运行时将驱动程序附加到 ESP-NETIF 对象并在编译期间配置网络堆栈。此外，ESP-NETIF 还提供了一组 API，用于控制网络接口的生命周期及其 TCP/IP 属性。ESP-NETIF 的公共接口大体上可以分为以下六组：

1) 初始化 API（用于创建并配置 ESP-NETIF 实例）
2) 输入或输出 API（用于在 IO 驱动和网络堆栈间传输数据）
3) 事件或行为 API

  * 管理网络接口生命周期
  * ESP-NETIF 为设计事件句柄提供了构建模块

4) 基本网络接口属性设置器和获取器 API
5) 网络堆栈抽象 API：实现用户与 TCP/IP 堆栈交互

  * 启用或禁用接口
  * DHCP 服务器和客户端 API
  * DNS API
  * `SNTP API`_

6) 驱动转换工具 API


D) 网络堆栈
^^^^^^^^^^^^^^^^

网络堆栈与应用程序代码在公共接口方面无公开交互，需通过 ESP-NETIF API 实现完全抽象。


E) ESP-NETIF L2 TAP 接口
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
ESP-NETIF L2 TAP 接口是 ESP-IDF 访问用户应用程序中的数据链路层（OSI/ISO 中的 L2）以进行帧接收和传输的机制。在嵌入式开发中，它通常用于实现非 IP 相关协议，如 PTP 和 Wake on LAN 等。请注意，目前 ESP-NETIF L2 TAP 接口仅支持以太网 (IEEE 802.3)。

使用 VFS 的文件描述符访问 ESP-NETIF L2 TAP 接口，VFS 文件描述符会提供类似文件的接口（调用 ``open()``、 ``read()``、 ``write()`` 等函数访问），详情请参阅 :doc:`/api-reference/storage/vfs`。

ESP-NETIF 只提供一个 L2 TAP 接口设备（路径名），但由于 ESP-NETIF L2 TAP 接口也可作为第二层基础设施的通用入口点，因此可以同时打开多个不同配置的文件描述符。特定文件描述符的具体配置很关键，它可以配置为仅允许访问由 ``if_key`` （如 `ETH_DEF`）标识的特定网络接口，并根据帧类型（如 IEEE 802.3 中的以太网类型）过滤特定帧。由于 ESP-NETIF L2 TAP 需要与 IP 堆栈同时存在，因此不应将 IP 相关流量（IP、ARP 等）直接传递给用户应用程序，此时则需要通过配置过滤特定帧实现这一点。在未过滤的情况下，即使该选项仍可配置，也不建议在标准用例中使用。过滤的另一优势在于，过滤后，用户应用程序只能访问它感兴趣的帧类型，其余的流量会传递到其他 L2 TAP 文件描述符或 IP 堆栈。

ESP-NETIF L2 TAP 接口使用手册
---------------------------------------

初始化
^^^^^^^^^^^^^^
要使用 ESP-NETIF L2 TAP 接口，需要首先通过 Kconfig 配置 :ref:`CONFIG_ESP_NETIF_L2_TAP` 启用接口，随后通过 :cpp:func:`esp_vfs_l2tap_intf_register()` 注册。请在完成上述步骤后再使用 VFS 函数。

``open()``
^^^^^^^^^^
ESP-NETIF L2 TAP 注册完成后，可使用路径名 "/dev/net/tap" 访问。同一路径名最多可以被打开 :ref:`CONFIG_ESP_NETIF_L2_TAP_MAX_FDS` 次，多个具有不同配置的文件描述符可以访问数据链路层的各个帧。

ESP-NETIF L2 TAP 可以使用 ``O_NONBLOCK`` 文件状态标志打开，确保 ``read()`` 不会阻塞。请注意，在当前实现中，当访问网络接口时，由于网络接口被多个 ESP-NETIF L2 TAP 文件描述符和 IP 栈共享，且缺乏列队机制，因此 ``write()`` 可能会受阻塞。使用 ``fcntl()`` 检索和修改文件状态标志。

成功时，``open()`` 返回新的文件描述符（非负整数）。出错时，返回 -1，并设置 ``errno`` 以标识错误。

``ioctl()``
^^^^^^^^^^^
由于新打开的 ESP-NETIF L2 TAP 文件描述符尚未绑定任意网络接口或配置任意帧类型过滤器，使用前，用户需通过以下选项完成配置：

  * ``L2TAP_S_INTF_DEVICE`` - 将文件描述符绑定到特定网络接口的选项，该网络接口由其 ``if_key`` 标识。ESP-NETIF 网络接口的 ``if_key`` 作为第三个参数传输给 ``ioctl()``。ESP-IDF 中，默认网络接口 ``if_key`` 的使用存放在 :component_file:`esp_netif/include/esp_netif_defaults.h` 头文件中。
  * ``L2TAP_S_DEVICE_DRV_HNDL`` - 将文件描述符绑定到特定网络接口的另一选项。此时，网络接口直接由其 IO 驱动句柄（例如以太网中的 :cpp:type:`esp_eth_handle_t`）标识。IO 驱动句柄将作为第三个参数传输给 ``ioctl()``。
  * ``L2TAP_S_RCV_FILTER`` - 设置过滤器，将特定类型的帧传递到文件描述符。在以太网中，帧是基于长度和以太网类型字段过滤的。如果过滤器值设置为小于或等于 0x05DC，则将以太网类型字段视作 IEEE802.3 长度字段，并将该字段中所有值在 <0, 0x05DC> 区间内的帧传递到文件描述符中。随后，由用户应用程序执行 IEEE802.2 逻辑链路控制 (LLC) 的解析。如果过滤器值设置为大于 0x05DC，则将以太网类型字段视为代表协议标识，仅将与设置值相等的帧传递到文件描述符中。

上述配置选项都支持通过对应获取器选项读取当前配置。

.. warning::
    首先调用 ``L2TAP_S_INTF_DEVICE`` 或 ``L2TAP_S_DEVICE_DRV_HNDL`` 将文件描述符绑定到特定网络接口，随后方可调用 ``L2TAP_S_RCV_FILTER`` 选项。

.. note::
    当前不支持识别 VLAN 标记帧。如果用户需要处理 VLAN 标记帧，应将过滤器设置为等于 VLAN 标记（即 0x8100 或 0x88A8），并在用户应用程序中处理 VLAN 标记帧。

.. note::
    当用户应用程序不需要使用 IP 栈时， ``L2TAP_S_DEVICE_DRV_HNDL`` 将非常适用，也无需初始化 ESP-NETIF。但在此情况下，网络接口无法通过 ``if_key`` 来识别，需要通过 IO 驱动程序句柄直接标识网络接口。

| 成功时，``ioctl()`` 返回 0。出错时，返回 -1，并设置 ``errno`` 以指示错误类型:
| * EBADF - 文件描述符无效。
| * EACCES - 此状态下无法改变选项（例如文件描述符尚未绑定到网络接口）。
| * EINVAL - 配置参数无效。同一网络接口上的其他文件描述符已经使用了以太网类型过滤器。
| * ENODEV - 此文件描述符尝试分配给的网络接口不存在。
| * ENOSYS - 不支持该操作，传递的配置选项不存在。

``fcntl()``
^^^^^^^^^^^
``fcntl()`` 配置已开启的 ESP-NETIF L2 TAP 文件描述符属性。

以下命令调控与文件描述符相关的状态标志：

  * ``F_GETFD`` - 函数返回文件描述符标志，忽略第三个参数。
  * ``F_SETFD`` - 将文件描述符标志设置为第三个参数的指定值。返回零。

| 成功时，``ioctl()`` 返回 0。出错时，返回 -1，并设置 ``errno`` 以指示错误类型。
| * EBADF - 文件描述符无效。
| * ENOSYS - 不支持该命令。

``read()``
^^^^^^^^^^
已开启并完成配置的 ESP-NETIF L2 TAP 文件描述符可通过 ``read()`` 获取入站帧。读取可以是阻塞或非阻塞的，具体取决于 ``O_NONBLOCK`` 文件状态标志的实际状态。当文件状态标志设置为阻塞时，读取程序将等待，直到接收到帧，并将上下文切换到其他任务。当文件状态标志设置为非阻塞时，立即返回读取程序。在此情况下，如果已经帧已经入队，则返回一帧，否则函数指示队列为空。与文件描述符关联的队列帧数量受 :ref:`CONFIG_ESP_NETIF_L2_TAP_RX_QUEUE_SIZE` Kconfig 选项限制。一旦队列里帧的数量达到配置的阈值，新到达的帧将被丢弃，直到队列有足够的空间接受传入的流量（队尾丢弃队列管理）。

| 成功时，``read()`` 函数返回读取的字节数。当目标缓冲区的大小为 0 时，函数返回 0。出错时，函数返回 -1，并设置 ``errno`` 以指示错误类型。
| * EBADF - 文件描述符无效。
| * EAGAIN - 文件描述符标记为非阻塞 (``O_NONBLOCK``)，但读取受阻塞。

``write()``
^^^^^^^^^^^
通过已开启并完成配置的 ESP-NETIF L2 TAP 文件描述符可以将原始数据链路层帧发送到网络接口，用户应用程序负责构建除物理接口设备自动添加的字段外的整个帧。在以太网链路中，用户应用程序需要构建以下字段：源或目的 MAC 地址、以太网类型、实际协议头和用户数据，字段长度如下表：

.. list-table::
    :header-rows: 1
    :widths: 20 20 20 30
    :align: center

    * - 目的 MAC
      - 源 MAC
      - 类型/长度
      - 负载（协议头/数据）
    * - 6 B
      - 6 B
      - 2 B
      - 0-1486 B

换句话说，ESP-NETIF L2 TAP 接口不会对数据帧进行额外处理，只会检查数据帧的以太网类型是否与文件描述符配置的过滤器相同。如果以太网类型不同，则会返回错误，并且不发送数据帧。需要注意的是，由于网络接口是由多个 ESP-NETIF L2 TAP 文件描述符和 IP 栈共享的资源，且当前缺乏列队机制，当前实现中的 ``write()`` 在进入网络接口时可能会受阻塞，。

| 成功时，``write()`` 返回已写入的字节数。如果输入缓冲区的大小为 0，则返回 0。出错时，则返回 -1，并设置 ``errno`` 以指示错误类型。
| * EBADF - 文件描述符无效。
| * EBADMSG - 帧的以太网类型与文件描述符配置的过滤器不同。
| * EIO - 网络接口不可用或正忙。

``close()``
^^^^^^^^^^^
已开启的 ESP-NETIF L2 TAP 文件描述符可以通过 ``close()`` 函数关闭，释放其分配到的资源。ESP-NETIF L2 TAP 实现的 ``close()`` 函数可能会受阻塞，但它是线程安全的，可以从与实际使用文件描述符的任务不同的任务中调用。如果出现一个任务在 I/O 操作中被阻塞、另一个任务试图关闭文件描述符的情况，则第一个任务会解除阻塞，其读取程序以错误结束。

| 成功时，``close()`` 返回 0。出错时，则返回 -1， 并设置 ``errno`` 以指示错误类型。
| * EBADF - 文件描述符无效。

``select()``
^^^^^^^^^^^^
``select()`` 函数按标准方法使用，启用 :ref:`CONFIG_VFS_SUPPORT_SELECT` 即可使用该函数。


.. _esp_netif-sntp-api:

SNTP API
--------

SNTP 的简要介绍、初始化代码和基本模式请参阅 :doc:`系统时间 </api-reference/system/system_time>` 的 :ref:`system-time-sntp-sync` 小节。

本节介绍了使用 SNTP 服务特定用例的详细信息，包括静态配置的服务器、使用 DHCP 提供的服务器或两者兼备的情况，操作流程如下：

1) 调用 :cpp:func:`esp_netif_sntp_init()` 初始化服务并完成配置。此操作只能执行一次（除非已调用 :cpp:func:`esp_netif_sntp_deinit()` 销毁 SNTP 服务）。
2) 调用 :cpp:func:`esp_netif_sntp_start()` 启动服务。如果在前一步中已经默认启动了服务，则不需要此步骤。如果需使用通过 DHCP 获取的 NTP 服务器，推荐在完成连接后显式启动该服务。注意，应在连接前启用通过 DHCP 获取的 NTP 服务器选项，并在连接后再启用 SNTP 服务。
3) 需要时，可调用 :cpp:func:`esp_netif_sntp_sync_wait()` 等待系统时间同步。
4) 调用 :cpp:func:`esp_netif_sntp_deinit()` 停止并销毁服务。


使用静态定义服务器的基本模式
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

连接到网络后，使用默认配置初始化该模块。注意，在配置结构体中可提供多个 NTP 服务器：

.. code-block:: c

    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG_MULTIPLE(2,
                               ESP_SNTP_SERVER_LIST("time.windows.com", "pool.ntp.org" ) );
    esp_netif_sntp_init(&config);

.. note::

    要配置多个 SNTP 服务器，需要更新 lwIP 配置，请参阅 :ref:`CONFIG_LWIP_SNTP_MAX_SERVERS`。


使用 DHCP 获取的 SNTP 服务器
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

首先，激活 lwIP 配置选项，相关配置请参阅 :ref:`CONFIG_LWIP_DHCP_GET_NTP_SRV`。其次，在使用 DHCP 选项、且不使用 NTP 服务器的情况下初始化 SNTP 模块，代码如下：

.. code-block:: c

    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG_MULTIPLE(0, {} );
    config.start = false;                       // 启动 SNTP 服务
    config.server_from_dhcp = true;             // 接收来自 DHCP 服务器的 NTP 服务提供方案
    esp_netif_sntp_init(&config);

连接成功后，可通过以下代码启动服务器：

.. code-block:: c

    esp_netif_sntp_start();

.. note::

    也可选择在初始化期间启动服务（即默认 ``config.start=true``）。注意，此时连接尚未完成，可能导致初始 SNTP 请求失败，并增加后续各次请求之间的延迟时间。


同时使用静态和动态服务器
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

同时使用静态和动态服务器的流程与使用 DHCP 获取的 SNTP 服务器基本相同。配置时，用户应确保在通过 DHCP 获取 NTP 服务器时刷新静态服务器配置。底层 lwIP 代码会在接受 DHCP 提供的信息时清理其余的 NTP 服务器列表。因此，ESP-NETIF SNTP 模块会保存静态配置的服务器，并在获取 DHCP 租约后对其重新配置。

典型配置依次如下，提供特定 ``IP_EVENT`` 更新配置，并提供第一个服务器的索引完成重新配置（例如，设置 ``config.index_of_first_server=1`` 会将 DHCP 提供的服务器保留在索引 0，而将静态配置的服务器保留在索引 1）。

.. code-block:: c

    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
    config.start = false;                       // 启动 SNTP 服务（连接成功后）
    config.server_from_dhcp = true;             // 接收来自 DHCP 服务器的 NTP 服务提供方案
    config.renew_servers_after_new_IP = true;   // 让 esp-netif 在接收到 DHCP 租约后更新配置的 SNTP 服务器
    config.index_of_first_server = 1;           // 服务器 1 开始更新，保留从 DHCP 获取的服务器 0 的设置
    config.ip_event_to_renew = IP_EVENT_STA_GOT_IP;  // 基于 IP 事件刷新配置

随后，调用 :cpp:func:`esp_netif_sntp_start()` 启用服务。


ESP-NETIF 编程手册
-----------------------------

请参阅示例部分，了解默认接口的基本初始化：


.. only:: SOC_WIFI_SUPPORTED

    - Wi-Fi 基站 :example_file:`wifi/getting_started/station/main/station_example_main.c`

- 以太网 :example_file:`ethernet/basic/main/ethernet_example_main.c`

- L2 TAP :example_file:`protocols/l2tap/main/l2tap_main.c`

.. only:: CONFIG_ESP_WIFI_SOFTAP_SUPPORT

    - Wi-Fi 接入点 :example_file:`wifi/getting_started/softAP/main/softap_example_main.c`

更多示例请参阅 :doc:`/api-reference/network/esp_netif_driver`。


.. only:: SOC_WIFI_SUPPORTED

    Wi-Fi 默认初始化
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^

    初始化代码以及注册默认接口（例如接入点和基站）的事件处理程序都在单独的 API 中提供，以便为大多数应用程序提供简单的启动代码：

    * :cpp:func:`esp_netif_create_default_wifi_sta()`

    .. only:: CONFIG_ESP_WIFI_SOFTAP_SUPPORT

        * :cpp:func:`esp_netif_create_default_wifi_ap()`

    注意，这些函数返回 ``esp_netif`` 句柄，即分配并配置了默认设置的网络接口对象的指针，这意味着：

    * 如果应用程序使用 :cpp:func:`esp_netif_destroy_default_wifi()` 提供网络去初始化，则创建的对象必须被销毁。

    * 这些 *默认* 接口不能被多次创建，除非使用 :cpp:func:`esp_netif_destroy()` 删除已创建的句柄。

    .. only:: CONFIG_ESP_WIFI_SOFTAP_SUPPORT

        * 在 ``AP+STA`` 模式下使用 Wi-Fi 时，须创建以上全部接口。


API 参考
-------------

.. include-build-file:: inc/esp_netif.inc
.. include-build-file:: inc/esp_netif_sntp.inc
.. include-build-file:: inc/esp_netif_types.inc
.. include-build-file:: inc/esp_netif_ip_addr.inc
.. include-build-file:: inc/esp_vfs_l2tap.inc


.. only:: SOC_WIFI_SUPPORTED

    Wi-Fi 默认 API 参考
    ^^^^^^^^^^^^^^^^^^^^^^^^^^

    .. include-build-file:: inc/esp_wifi_default.inc
