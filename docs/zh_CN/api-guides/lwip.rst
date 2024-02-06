lwIP
====

:link_to_translation:`en:[English]`

ESP-IDF 使用开源的 `lwIP 轻量级 TCP/IP 协议栈`_，该版 lwIP (`esp-lwip`_) 相对上游项目做了修改和增补。

支持的 API
--------------

ESP-IDF 支持以下 lwIP TCP/IP 协议栈功能：

- `BSD 套接字 API`_
- `Netconn API`_ 已启用，但暂无对 ESP-IDF 应用程序的官方支持

.. _lwip-dns-limitation:

适配的 API
^^^^^^^^^^^^

    .. warning::

        在使用除 `BSD 套接字 API`_ 外的任意 lwIP API 时，请确保所用 API 为线程安全。请启用 :ref:`CONFIG_LWIP_CHECK_THREAD_SAFETY` 配置选项并运行应用程序，检查所用 API 是否线程安全。此时，lwIP 断言 TCP/IP 核心功能可以正确访问。如果未能从正确的 `lwIP FreeRTOS 任务`_ 访问，或没有正确锁定，则执行中止。建议使用 :doc:`/api-reference/network/esp_netif` 组件与 lwIP 交互。

ESP-IDF 间接支持以下常见的 lwIP 应用程序 API：

- 动态主机设置协议 (DHCP) 服务器和客户端，由 :doc:`/api-reference/network/esp_netif` 功能间接支持。
- 域名系统 (DNS)；获取 DHCP 地址时，可以自动分配 DNS 服务器，也可以通过 :doc:`/api-reference/network/esp_netif` API 手动配置。

.. note::

    lwIP 中的 DNS 服务器配置为全局配置，而非针对特定接口的配置。如需同时使用不同 DNS 服务器的多个网络接口，在从一个接口获取 DHCP 租约时，请注意避免意外覆盖另一个接口的 DNS 设置。

- 简单网络时间协议 (SNTP)，由 :doc:`/api-reference/network/esp_netif` 功能间接支持，或通过 :component_file:`lwip/include/apps/esp_sntp.h` 中的函数直接支持。该函数还为 :component_file:`lwip/lwip/src/include/lwip/apps/sntp.h` 函数提供了线程安全的 API，请参阅 :ref:`system-time-sntp-sync`。
- ICMP Ping，由 lwIP ping API 的变体支持，请参阅 :doc:`/api-reference/protocols/icmp_echo`。
- ICMPv6 Ping，由 lwIP 的 ICMPv6 Echo API 支持，用于测试 IPv6 网络连接情况。有关详细信息，请参阅 :example:`protocols/sockets/icmpv6_ping`。
- NetBIOS 查找，由标准的 lwIP API 支持，:example:`protocols/http_server/restful_server` 示例中提供了使用 NetBIOS 在局域网中查找主机的选项。
- mDNS 与 lwIP 的默认 mDNS 使用不同实现方式，请参阅 :doc:`/api-reference/protocols/mdns`。但启用 :ref:`CONFIG_LWIP_DNS_SUPPORT_MDNS_QUERIES` 设置项后，lwIP 可以使用 ``gethostbyname()`` 等标准 API 和 ``hostname.local`` 约定查找 mDNS 主机。
- lwIP 中的 PPP 实现可用于在 ESP-IDF 中创建 PPPoS（串行 PPP）接口。请参阅 :doc:`/api-reference/network/esp_netif` 组件文档，使用 :component_file:`esp_netif/include/esp_netif_defaults.h` 中定义的 ``ESP_NETIF_DEFAULT_PPP()`` 宏创建并配置 PPP 网络接口。:component_file:`esp_netif/include/esp_netif_ppp.h` 中提供了其他的运行时设置。PPPoS 接口通常用于与 NBIoT/GSM/LTE 调制解调器交互。`esp_modem <https://components.espressif.com/component/espressif/esp_modem>`_ 仓库还支持更多应用层友好的 API，该仓库内部使用了上述 PPP lwIP 模块。

BSD 套接字 API
-----------------

BSD 套接字 API 是一种常见的跨平台 TCP/IP 套接字 API，最初源于 UNIX 操作系统的伯克利标准发行版，现已标准化为 POSIX 规范的一部分。BSD 套接字有时也称 POSIX 套接字，或伯克利套接字。

在 ESP-IDF 中，lwIP 支持 BSD 套接字 API 的所有常见用法。

参考
^^^^^^^^^^

BSD 套接字的相关参考资料十分丰富，包括但不限于：

- `单一 UNIX 规范 - BSD 套接字 <https://pubs.opengroup.org/onlinepubs/007908799/xnsix.html>`_
- `伯克利套接字 - 维基百科 <https://en.wikipedia.org/wiki/Berkeley_sockets>`_

示例
^^^^^^^^

以下为 ESP-IDF 中使用 BSD 套接字 API 的部分示例：

- :example:`protocols/sockets/tcp_server`
- :example:`protocols/sockets/tcp_client`
- :example:`protocols/sockets/udp_server`
- :example:`protocols/sockets/udp_client`
- :example:`protocols/sockets/udp_multicast`
- :example:`protocols/http_request`：此简化示例使用 TCP 套接字发送 HTTP 请求，但更推荐使用 :doc:`/api-reference/protocols/esp_http_client` 发送 HTTP 请求

支持的函数
^^^^^^^^^^^^^^^^^^^

在 ESP-IDF 中，lwIP 支持以下 BSD 套接字 API 函数，详情请参阅 :component_file:`lwip/lwip/src/include/lwip/sockets.h`。

- ``socket()``
- ``bind()``
- ``accept()``
- ``shutdown()``
- ``getpeername()``
- ``getsockopt()`` 和 ``setsockopt()``：请参阅 `套接字选项`_
- ``close()``：通过 :doc:`/api-reference/storage/vfs` 调用
- ``read()``、``readv()``、``write()``、``writev()``：通过 :doc:`/api-reference/storage/vfs` 调用
- ``recv()``、``recvmsg()``、``recvfrom()``
- ``send()``、``sendmsg()``、``sendto()``
- ``select()``：通过 :doc:`/api-reference/storage/vfs` 调用
- ``poll()``：ESP-IDF 通过在内部调用 ``select()`` 实现 ``poll()``，因此，建议直接调用 ``select()``
- ``fcntl()``：请参阅 `fcntl()`_

非标准函数：

- ``ioctl()``：请参阅 `ioctl()`_

.. note::

  部分 lwIP 应用程序示例代码使用了带前缀的 BSD API，如 ``lwip_socket()``，而非标准 ``socket()``。ESP-IDF 支持使用以上两种形式，但更建议使用标准名称。

套接字错误处理
^^^^^^^^^^^^^^^^^^^^^

要使套接字应用程序保持稳定，BSD 套接字错误处理代码至关重要。套接字错误处理通常涉及以下几个方面：

- 错误检测
- 获取错误原因代码
- 根据错误原因代码处理错误

在 lwIP 中，处理套接字错误分以下两种情况：

- 套接字 API 返回错误，请参阅 `套接字 API 错误`_。
- ``select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout)`` 包含异常描述符，表示套接字出现错误，详情请参阅 `select() 错误`_。

套接字 API 错误
+++++++++++++++++

**错误检测**

  - 根据返回值判断套接字 API 是否出错。

**获取错误原因代码**

  - 套接字 API 出错时，其返回值不包含失败原因，可以通过应用程序访问 ``errno`` 获取错误原因代码。不同返回值具有不同含义，详情请参阅 `套接字错误原因代码`_。

示例：

.. code-block::

        int err;
        int sockfd;

        if (sockfd = socket(AF_INET,SOCK_STREAM,0) < 0) {
            // 从 errno 获取错误代码
            err = errno;
            return err;
        }

``select()`` 错误
+++++++++++++++++++++++

**错误检测**

  - ``select()`` 包含异常描述符时的套接字错误。

**获取错误原因代码**

  - 如果 ``select()`` 报告套接字错误，访问 ``errno`` 无法获取错误原因代码，此时，应调用 ``getsockopt()``。因为当 ``select()`` 包含异常描述符时，错误代码不会直接赋值给 ``errno``。

.. note::

    ``getsockopt()`` 函数具有以下原型：``int getsockopt(int s, int level, int optname, void *optval, socklen_t *optlen)``。原型可以获取任意类型、任意状态套接字选项的当前值，并将结果存储在 ``optval`` 中。例如，要在套接字上获取错误代码，可以通过 ``getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &err, &optlen)`` 实现。

示例：

.. code-block::

        int err;

        if (select(sockfd + 1, NULL, NULL, &exfds, &tval) <= 0) {
            err = errno;
            return err;
        } else {
            if (FD_ISSET(sockfd, &exfds)) {
                // 使用 getsockopt() 获取 select() 异常集
                int optlen = sizeof(int);
                getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &err, &optlen);
                return err;
            }
        }

套接字错误原因代码
++++++++++++++++++++++++

以下是常见错误代码列表。有关标准 POSIX/C 错误代码的详细列表，请参阅 `newlib errno.h <https://github.com/espressif/newlib-esp32/blob/master/newlib/libc/include/sys/errno.h>`_ 和特定平台扩展 :component_file:`newlib/platform_include/errno.h`。

.. list-table::
    :header-rows: 1
    :widths: 50 50
    :align: center

    * - 错误代码
      - 描述
    * - ECONNREFUSED
      - 拒绝连接
    * - EADDRINUSE
      - 地址已在使用中
    * - ECONNABORTED
      - 软件导致连接中断
    * - ENETUNREACH
      - 网络不可达
    * - ENETDOWN
      - 未配置网络接口
    * - ETIMEDOUT
      - 连接超时
    * - EHOSTDOWN
      - 主机已关闭
    * - EHOSTUNREACH
      - 主机不可达
    * - EINPROGRESS
      - 连接已在进行中
    * - EALREADY
      - 套接字已连接
    * - EDESTADDRREQ
      - 需要目标地址
    * - EPROTONOSUPPORT
      - 未知协议

套接字选项
^^^^^^^^^^^^^^

``getsockopt()`` 支持获取套接字选项，``setsockopt()`` 支持设置套接字选项。

在 ESP-IDF 中，lwIP 并不支持所有标准套接字选项。以下套接字选项受 lwIP 支持：

常见选项
++++++++++++++

与级别参数 ``SOL_SOCKET`` 一起使用。

- ``SO_REUSEADDR``：如果 :ref:`CONFIG_LWIP_SO_REUSE` 已启用，则该选项可用，可以设置 :ref:`CONFIG_LWIP_SO_REUSE_RXTOALL` 自定义其行为
- ``SO_KEEPALIVE``
- ``SO_BROADCAST``
- ``SO_ACCEPTCONN``
- ``SO_RCVBUF``：如果 :ref:`CONFIG_LWIP_SO_RCVBUF` 已启用，则该选项可用
- ``SO_SNDTIMEO`` / ``SO_RCVTIMEO``
- ``SO_ERROR``：此选项仅支持与 ``select()`` 一起使用，请参阅 `套接字错误处理`_
- ``SO_TYPE``
- ``SO_NO_CHECK``：仅适用于 UDP 套接字

IP 选项
++++++++++

与级别参数 ``IPPROTO_IP`` 一起使用。

- ``IP_TOS``
- ``IP_TTL``
- ``IP_PKTINFO``：如果 :ref:`CONFIG_LWIP_NETBUF_RECVINFO` 已启用，则该选项可用

对于组播 UDP 套接字：

- ``IP_MULTICAST_IF``
- ``IP_MULTICAST_LOOP``
- ``IP_MULTICAST_TTL``
- ``IP_ADD_MEMBERSHIP``
- ``IP_DROP_MEMBERSHIP``

TCP 选项
+++++++++++

只适用于 TCP 套接字，与级别参数 ``IPPROTO_TCP`` 一起使用。

- ``TCP_NODELAY``

与 TCP 保活探测相关的选项：

- ``TCP_KEEPALIVE``：整数值，以毫秒为单位，设置 TCP 保活探测周期
- ``TCP_KEEPIDLE``：整数值，以秒为单位，与 ``TCP_KEEPALIVE`` 相同
- ``TCP_KEEPINTVL``：整数值，以秒为单位，设置保活探测间隔
- ``TCP_KEEPCNT``：整数值，设置超时前进行的保活探测次数

IPv6 选项
++++++++++++

只适用于 IPv6 套接字，与级别参数 ``IPPROTO_IPV6`` 一起使用。

- ``IPV6_CHECKSUM``
- ``IPV6_V6ONLY``

对于组播 IPv6 UDP 套接字：

- ``IPV6_JOIN_GROUP`` / ``IPV6_ADD_MEMBERSHIP``
- ``IPV6_LEAVE_GROUP`` / ``IPV6_DROP_MEMBERSHIP``
- ``IPV6_MULTICAST_IF``
- ``IPV6_MULTICAST_HOPS``
- ``IPV6_MULTICAST_LOOP``

``fcntl()``
^^^^^^^^^^^

``fcntl()`` 函数是设置与文件描述符相关选项的标准 API。在 ESP-IDF 中，使用 :doc:`/api-reference/storage/vfs` 层实现该函数。

当文件描述符为套接字时，仅支持以下 ``fcntl()`` 值：

- ``O_NONBLOCK`` 用于置位或清除非阻塞 I/O 模式。``O_NDELAY`` 也受支持，与前者功能相同。
- ``O_RDONLY``、``O_WRONLY``、``O_RDWR`` 标志用于不同的读或写模式，只能用 ``F_GETFL`` 读取，且无法用 ``F_SETFL`` 设置。根据连接状况，即两端开启或任一端关闭，TCP 套接字会返回不同模式，而 UDP 套接字始终返回 ``O_RDWR``。

``ioctl()``
^^^^^^^^^^^

``ioctl()`` 函数以半标准的方式访问 TCP/IP 协议栈的部分内部功能。ESP-IDF 通过 :doc:`/api-reference/storage/vfs` 层实现此函数。

当文件描述符为套接字时，仅支持以下 ``ioctl()`` 值：

- ``FIONREAD`` 返回套接字网络 buffer 中接收的待处理字节数。
- ``FIONBIO`` 和 ``fcntl(fd, F_SETFL, O_NONBLOCK, ...)`` 相同，也可置位或清除套接字非阻塞 I/O 状态。

Netconn API
-----------

lwIP 支持两种较低级别的 API 和 BSD 套接字 API，即 Netconn API 和 Raw API。

lwIP Raw API 适用于单线程设备，无法在 ESP-IDF 中使用。

Netconn API 用于在 lwIP 内部使用 BSD 套接字 API，支持直接从 ESP-IDF 的应用程序调用。相较于 BSD 套接字 API，该 API 占用资源更少。无需提前将数据复制到内部 lwIP buffer，即可使用 Netconn API 发送和接收数据。

.. important::

    乐鑫尚未在 ESP-IDF 中测试 Netconn API，因此 **此功能已启用，但尚无官方支持**。对于某些功能，可能只有在从 BSD 套接字 API 中使用时才能正常运作。

有关 Netconn API 的更多信息，请参阅 `lwip/lwip/src/include/lwip/api.h <http://www.nongnu.org/lwip/2_0_x/api_8h.html>`_ 和 `lwIP 应用程序 **非官方** 开发手册的一部分 <https://lwip.fandom.com/wiki/Netconn_API>`_。

lwIP FreeRTOS 任务
------------------

lwIP 创建了专用的 TCP/IP FreeRTOS 任务，处理来自其他任务的套接字 API 请求。

以下配置项可用于修改任务，并调整向 TCP/IP 任务发送数据和从 TCP/IP 任务接收数据的队列（邮箱）：

- :ref:`CONFIG_LWIP_TCPIP_RECVMBOX_SIZE`
- :ref:`CONFIG_LWIP_TCPIP_TASK_STACK_SIZE`
- :ref:`CONFIG_LWIP_TCPIP_TASK_AFFINITY`

IPv6 支持
------------

系统支持 IPv4 和 IPv6 的双栈功能，并默认启用这两种协议。如无需要，可将其禁用，请参阅 :ref:`lwip-ram-usage`。

在 ESP-IDF 中，IPv6 支持仅限 **无状态自动配置**，不支持 **有状态配置**，上游的 lwIP 也不支持 **有状态配置**。

IPv6 地址配置通过以下协议或服务定义：

- 支持 **SLAAC** IPv6 无状态地址配置 (RFC-2462)
- 支持 **DHCPv6** IPv6 动态主机配置协议 (RFC-8415)

以上两种地址配置默认处于禁用状态，设备仅使用链路本地地址或静态定义的地址。

.. _lwip-ivp6-autoconfig:

无状态自动配置流程
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

要通过路由器通告协议启用地址自动配置，请启用此配置选项：

- :ref:`CONFIG_LWIP_IPV6_AUTOCONFIG`

该配置选项启用了所有网络接口的 IPv6 自动配置。而在上游 lwIP 中，需要设置 ``netif->ip6_autoconfig_enabled=1``，针对每个 ``netif`` 明确启用自动配置。

.. _lwip-ivp6-dhcp6:

DHCPv6
^^^^^^

lwIP 中的 DHCPv6 非常简单，仅支持无状态配置，可通过以下配置选项启用：

- :ref:`CONFIG_LWIP_IPV6_DHCP6`

由于 DHCPv6 仅在无状态配置下工作，因此还需要通过 :ref:`CONFIG_LWIP_IPV6_AUTOCONFIG` 启用 :ref:`lwip-ivp6-autoconfig`。

此外，还需要使用以下语句，在应用程序代码中明确启用 DHCPv6：

.. code-block::

    dhcp6_enable_stateless(netif);

IPv6 自动配置中的 DNS 服务器
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

要自动配置 DNS 服务器，尤其是在仅使用 IPv6 的网络中配置，可使用以下两种选项：

- 递归域名系统 (DNS)，属于邻居发现协议 (NDP) 的一部分，可使用 :ref:`lwip-ivp6-autoconfig`。

  DNS 服务器的数量必须设置为 :ref:`CONFIG_LWIP_IPV6_RDNSS_MAX_DNS_SERVERS`，该选项默认禁用，即置位为 0。

- DHCPv6 无状态配置，使用 :ref:`lwip-ivp6-dhcp6` 配置 DNS 服务器。注意，此配置假设 IPv6 路由通告标志 (RFC-5175) 进行了如下设置

    - 管理地址配置标志 (Managed Address Configuration Flag) = 0
    - 其他配置标志 (Other Configuration Flag) = 1

ESP-lwIP 自定义修改
-----------------------------

补充内容
^^^^^^^^^

以下代码均为新增代码，尚未包含至上游 lwIP 版本：

线程安全的套接字
+++++++++++++++++++

调用 ``close()`` 可以从不同于创建套接字的线程中关闭该套接字。该调用持续阻塞，直至其他任务中使用该套接字的函数调用返回。

然而，任务处于主动等待 ``select()`` 或 ``poll()`` API 的状态时，无法删除该任务。销毁任务前，这些 API 必须先退出，否则可能会破坏内部数据结构，并导致后续 lwIP 崩溃。这些 API 在栈上分配了全局引用的回调指针，因此，在未完全卸载栈的情况下删除任务时，lwIP 仍可以持有指向已删除栈的指针。

按需定时器
++++++++++++++++

lwIP 中的 IGMP 和 MLD6 功能都会初始化一个定时器，以便在特定时间触发超时事件。

即便没有活动的超时事件，lwIP 也会默认始终启用这些定时器，增加自动 Light-sleep 模式下的 CPU 使用率和功耗。``ESP-lwIP`` 则默认将各定时器设置为 ``按需`` 使用，即只在有待处理事件时启用。

如果要返回默认 lwIP 设置，即始终启用定时器，请禁用 :ref:`CONFIG_LWIP_TIMERS_ONDEMAND`。

lwIP 定时器 API
+++++++++++++++

不使用 Wi-Fi 时，可以通过 API 关闭 lwIP 定时器，减少功耗。

以下 API 函数均受支持，详情请参阅 :component_file:`lwip/lwip/src/include/lwip/timeouts.h`。

- ``sys_timeouts_init()``
- ``sys_timeouts_deinit()``

附加套接字选项
+++++++++++++++++++++++++

- 目前已实现部分标准 IPV4 和 IPV6 组播套接字选项，详情请参阅 `套接字选项`_。

- 使用 ``IPV6_V6ONLY`` 套接字选项，可以设置仅使用 IPV6 的 UDP 和 TCP 套接字，而 lwIP 一般只支持 TCP 套接字。

IP 层特性
+++++++++++++++++

- IPV4 源地址基础路由实现不同

- 支持 IPV4 映射 IPV6 地址

NAPT 和端口转发
++++++++++++++++++++++++

支持 IPv4 网络地址端口转换（NAPT）和端口转发。然而，仅限于单个接口启用 NAPT。

- 要在两个接口之间使用 NAPT 转发数据包，必须在连接到目标网络的接口上启用 NAPT。例如，为了通过 Wi-Fi 接口为以太网流量启用互联网访问，必须在以太网接口上启用 NAPT。
- NAPT 的使用示例可参考 :example:`network/vlan_support`。

.. _lwip-custom-hooks:

自定义 lwIP 钩子
+++++++++++++++++++++

原始 lwIP 支持通过 ``LWIP_HOOK_FILENAME`` 实现自定义的编译时修改。ESP-IDF 端口层已使用该文件，但仍支持通过由宏 ``ESP_IDF_LWIP_HOOK_FILENAME`` 定义的头文件，在 ESP-IDF 中包含并实现自定义添加。以下示例展示了向构建过程添加自定义钩子文件的过程，其中钩子文件名为 ``my_hook.h``，位于项目的 ``main`` 文件夹中：

.. code-block:: cmake

   idf_component_get_property(lwip lwip COMPONENT_LIB)
   target_compile_options(${lwip} PRIVATE "-I${PROJECT_DIR}/main")
   target_compile_definitions(${lwip} PRIVATE "-DESP_IDF_LWIP_HOOK_FILENAME=\"my_hook.h\"")

使用 ESP-IDF 构建系统自定义 lwIP 选项
++++++++++++++++++++++++++++++++++++++++++++++++++

组件配置菜单可以配置常见的 lwIP 选项，但是一些自定义选项需要通过命令行添加。CMake 函数 ``target_compile_definitions()`` 可以用于定义宏，示例如下：

.. code-block:: cmake

   idf_component_get_property(lwip lwip COMPONENT_LIB)
   target_compile_definitions(${lwip} PRIVATE "-DETHARP_SUPPORT_VLAN=1")

使用这种方法可能无法定义函数式宏。虽然 GCC 支持此类定义，但是未必所有编译器都会接受。为了解决这一限制，可以使用 ``add_definitions()`` 函数为整个项目定义宏，例如 ``add_definitions("-DFALLBACK_DNS_SERVER_ADDRESS(addr)=\"IP_ADDR4((addr), 8,8,8,8)\"")``。

另一种方法是在头文件中定义函数式宏，该头文件将预先包含在 lwIP 钩子文件中，请参考 :ref:`lwip-custom-hooks`。

限制
^^^^^^^^^^^

如 :ref:`lwip-dns-limitation` 所述，ESP-IDF 中的 lwIP 扩展功能仍然受到全局 DNS 限制的影响。为了在应用程序代码中解决这一限制，可以使用 ``FALLBACK_DNS_SERVER_ADDRESS()`` 宏定义所有接口能够访问的全局 DNS 备用服务器，或者单独维护每个接口的 DNS 服务器，并在默认接口更改时重新配置。

在 UDP 套接字上重复调用 ``send()`` 或 ``sendto()`` 最终可能会导致错误。此时 ``errno`` 报错为 ``ENOMEM``，错误原因是底层网络接口驱动程序中的 buffer 大小有限。当所有驱动程序的传输 buffer 已满时，UDP 传输事务失败。如果应用程序需要发送大量 UDP 数据报，且不希望发送方丢弃数据报，建议检查错误代码，采用短延迟的重传机制。

.. only:: esp32

    在 :ref:`Wi-Fi <CONFIG_ESP_WIFI_TX_BUFFER>` 或 :ref:`Ethernet <CONFIG_ETH_DMA_TX_BUFFER_NUM>` 项目配置中适当增加传输 buffer 数量，或许可以缓解此情况。

.. only:: not esp32 and SOC_WIFI_SUPPORTED

    在 :ref:`Wi-Fi <CONFIG_ESP_WIFI_TX_BUFFER>` 项目配置中适当增加传输 buffer 数量，或许可以缓解此情况。

.. _lwip-performance:

性能优化
------------------------

影响 TCP/IP 性能因素较多，可以从多方面进行优化。经调整，ESP-IDF 的默认设置已在 TCP/IP 的吞吐量、响应时间和内存使用间达到平衡。

最大吞吐量
^^^^^^^^^^^^^^^^^^

乐鑫使用 iperf 测试应用程序 https://iperf.fr/ 测试了 ESP-IDF 的 TCP/IP 吞吐量。关于实际测试和优化配置的更多信息，请参考 :ref:`improve-network-speed`。

.. important::

  建议逐步应用更改，并在每次更改后，通过特定应用程序的工作负载检查性能。

- 如果系统中有许多任务抢占 CPU 时间，可以考虑调整 lwIP 任务的 CPU 亲和性 (:ref:`CONFIG_LWIP_TCPIP_TASK_AFFINITY`)，并以固定优先级 (18, ``ESP_TASK_TCPIP_PRIO``) 运行。为优化 CPU 使用，可以考虑将竞争任务分配给不同核心，或将其优先级调整至较低值。有关内置任务优先级的更多详情，请参阅 :ref:`built-in-task-priorities`。

- 如果使用仅带有套接字参数的 ``select()`` 函数，禁用 :ref:`CONFIG_VFS_SUPPORT_SELECT` 可以更快地调用 ``select()``。

- 如果有足够的空闲 IRAM，可以选择 :ref:`CONFIG_LWIP_IRAM_OPTIMIZATION` 和 :ref:`CONFIG_LWIP_EXTRA_IRAM_OPTIMIZATION`，提高 TX/RX 吞吐量。

.. only:: SOC_WIFI_SUPPORTED

    如果使用 Wi-Fi 网络接口，请参阅 :ref:`wifi-buffer-usage`。

最低延迟
^^^^^^^^^^^^^^^

除增加 buffer 大小外，大多数增加吞吐量的设置会减少 lwIP 函数占用 CPU 的时间，进而降低延迟，缩短响应时间。

- 对于 TCP 套接字，lwIP 支持设置标准的 ``TCP_NODELAY`` 标记以禁用 Nagle 算法。

.. _lwip-ram-usage:

最小内存使用
^^^^^^^^^^^^^^^^^

由于 RAM 按需从堆中分配，多数 lwIP 的 RAM 使用也按需分配。因此，更改 lwIP 设置减少 RAM 使用时，或许不会改变空闲时的 RAM 使用量，但可以改变高峰期的 RAM 使用量。

- 减少 :ref:`CONFIG_LWIP_MAX_SOCKETS` 可以减少系统中的最大套接字数量。更改此设置，会让处于 ``WAIT_CLOSE`` 状态的 TCP 套接字在需要打开新套接字时更快地关闭和复用，进一步降低峰值 RAM 使用量。
- 减少 :ref:`CONFIG_LWIP_TCPIP_RECVMBOX_SIZE`、:ref:`CONFIG_LWIP_TCP_RECVMBOX_SIZE` 和 :ref:`CONFIG_LWIP_UDP_RECVMBOX_SIZE` 可以减少 RAM 使用量，但会影响吞吐量，具体取决于使用情况。
- 减少 :ref:`CONFIG_LWIP_TCP_ACCEPTMBOX_SIZE` 可以通过限制同时接受的连接数来减少 RAM 使用量。
- 减少 :ref:`CONFIG_LWIP_TCP_MSL` 和 :ref:`CONFIG_LWIP_TCP_FIN_WAIT_TIMEOUT` 可以减少系统中的最大分段寿命，同时会使处于 ``TIME_WAIT`` 和 ``FIN_WAIT_2`` 状态的 TCP 套接字能更快地关闭和复用。
- 禁用 :ref:`CONFIG_LWIP_IPV6` 可以在系统启动时节省大约 39 KB 的固件大小和 2 KB 的 RAM，并在运行 TCP/IP 栈时节省 7 KB 的 RAM。如果无需支持 IPV6，可以禁用 IPv6，减少 flash 和 RAM 占用。
- 禁用 :ref:`CONFIG_LWIP_IPV4` 可以在系统启动时节省大约 26 KB 的固件大小和 600 B 的 RAM，并在运行 TCP/IP 栈时节省 6 KB 的 RAM。如果本地网络仅支持 IPv6 配置，可以禁用 IPv4，减少 flash 和 RAM 占用。

.. only:: SOC_WIFI_SUPPORTED

    如果使用 Wi-Fi，请参阅 :ref:`wifi-buffer-usage`。


最大 buffer 使用
+++++++++++++++++

lwIP 消耗的最大堆内存即 lwIP 驱动程序 **理论上可能消耗的最大内存**，通常取决于以下因素：

 - 创建 UDP 连接所需的内存：``lwip_udp_conn``
 - 创建 TCP 连接所需的内存：``lwip_tcp_conn``
 - 应用程序拥有的 UDP 连接数量：``lwip_udp_con_num``
 - 应用程序拥有的 TCP 连接数量：``lwip_tcp_con_num``
 - TCP 的 TX 窗口大小：``lwip_tcp_tx_win_size``
 - TCP 的 RX 窗口大小：``lwip_tcp_rx_win_size``

**因此，lwIP 消耗的最大堆内存可以用以下公式计算：**
  lwip_dynamic_peek_memory =  (lwip_udp_con_num * lwip_udp_conn)  + (lwip_tcp_con_num * (lwip_tcp_tx_win_size + lwip_tcp_rx_win_size + lwip_tcp_conn))

某些基于 TCP 的应用程序只需要一个 TCP 连接。然而，当出现错误（如发送失败）时，应用程序可能会关闭此 TCP 连接，并创建一个新的连接。根据 TCP 状态机和 RFC793，关闭 TCP 连接可能需要很长时间，这可能导致系统中同时存在多个 TCP 连接。


.. _lwIP 轻量级 TCP/IP 协议栈: https://savannah.nongnu.org/projects/lwip/
.. _esp-lwip: https://github.com/espressif/esp-lwip
