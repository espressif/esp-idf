lwIP
====

:link_to_translation:`zh_CN:[中文]`

ESP-IDF uses the open source `lwIP lightweight TCP/IP stack`_. The ESP-IDF version of lwIP (`esp-lwip`_) has some modifications and additions compared to the upstream project.

Supported APIs
--------------

ESP-IDF supports the following lwIP TCP/IP stack functions:

- `BSD Sockets API`_
- `Netconn API`_ is enabled but not officially supported for ESP-IDF applications

.. _lwip-dns-limitation:

Adapted APIs
^^^^^^^^^^^^

    .. warning::

        When using any lwIP API other than the `BSD Sockets API`_, please make sure that the API is thread-safe. To check if a given API call is thread-safe, enable the :ref:`CONFIG_LWIP_CHECK_THREAD_SAFETY` configuration option and run the application. This enables lwIP to assert the correct access of the TCP/IP core functionality. If the API is not accessed or locked properly from the appropriate `lwIP FreeRTOS Task`_, the execution will be aborted. The general recommendation is to use the :doc:`/api-reference/network/esp_netif` component to interact with lwIP.

Some common lwIP app APIs are supported indirectly by ESP-IDF:

- Dynamic Host Configuration Protocol (DHCP) Server & Client are supported indirectly via the :doc:`/api-reference/network/esp_netif` functionality.
- Domain Name System (DNS) is supported in lwIP; DNS servers could be assigned automatically when acquiring a DHCP address, or manually configured using the :doc:`/api-reference/network/esp_netif` API.

.. note::

    DNS server configuration in lwIP is global, not interface-specific. If you are using multiple network interfaces with distinct DNS servers, exercise caution to prevent inadvertent overwrites of one interface's DNS settings when acquiring a DHCP lease from another interface.

- Simple Network Time Protocol (SNTP) is also supported via the :doc:`/api-reference/network/esp_netif`, or directly via the :component_file:`lwip/include/apps/esp_sntp.h` functions, which also provide thread-safe API to :component_file:`lwip/lwip/src/include/lwip/apps/sntp.h` functions, see also :ref:`system-time-sntp-sync`.
- ICMP Ping is supported using a variation on the lwIP ping API, see :doc:`/api-reference/protocols/icmp_echo`.
- ICMPv6 Ping, supported by lwIP's ICMPv6 Echo API, is used to test IPv6 network connectivity. For more information, see :example:`protocols/sockets/icmpv6_ping`.
- NetBIOS lookup is available using the standard lwIP API. :example:`protocols/http_server/restful_server` has the option to demonstrate using NetBIOS to look up a host on the LAN.
- mDNS uses a different implementation to the lwIP default mDNS, see :doc:`/api-reference/protocols/mdns`. But lwIP can look up mDNS hosts using standard APIs such as ``gethostbyname()`` and the convention ``hostname.local``, provided the :ref:`CONFIG_LWIP_DNS_SUPPORT_MDNS_QUERIES` setting is enabled.
- The PPP implementation in lwIP can be used to create PPPoS (PPP over serial) interface in ESP-IDF. Please refer to the documentation of the :doc:`/api-reference/network/esp_netif` component to create and configure a PPP network interface, by means of the ``ESP_NETIF_DEFAULT_PPP()`` macro defined in :component_file:`esp_netif/include/esp_netif_defaults.h`. Additional runtime settings are provided via :component_file:`esp_netif/include/esp_netif_ppp.h`. PPPoS interfaces are typically used to interact with NBIoT/GSM/LTE modems. More application-level friendly API is supported by the `esp_modem <https://components.espressif.com/component/espressif/esp_modem>`_ library, which uses this PPP lwIP module behind the scenes.

BSD Sockets API
---------------

The BSD Sockets API is a common cross-platform TCP/IP sockets API that originated in the Berkeley Standard Distribution of UNIX but is now standardized in a section of the POSIX specification. BSD Sockets are sometimes called POSIX Sockets or Berkeley Sockets.

As implemented in ESP-IDF, lwIP supports all of the common usages of the BSD Sockets API.

References
^^^^^^^^^^

A wide range of BSD Sockets reference materials are available, including:

- `Single UNIX Specification - BSD Sockets page <https://pubs.opengroup.org/onlinepubs/007908799/xnsix.html>`_
- `Berkeley Sockets - Wikipedia page <https://en.wikipedia.org/wiki/Berkeley_sockets>`_

Examples
^^^^^^^^

A number of ESP-IDF examples show how to use the BSD Sockets APIs:

- :example:`protocols/sockets/tcp_server`
- :example:`protocols/sockets/tcp_client`
- :example:`protocols/sockets/udp_server`
- :example:`protocols/sockets/udp_client`
- :example:`protocols/sockets/udp_multicast`
- :example:`protocols/http_request`: this simplified example uses a TCP socket to send an HTTP request, but :doc:`/api-reference/protocols/esp_http_client` is a much better option for sending HTTP requests

Supported Functions
^^^^^^^^^^^^^^^^^^^

The following BSD socket API functions are supported. For full details, see :component_file:`lwip/lwip/src/include/lwip/sockets.h`.

- ``socket()``
- ``bind()``
- ``accept()``
- ``shutdown()``
- ``getpeername()``
- ``getsockopt()`` & ``setsockopt()``: see `Socket Options`_
- ``close()``: via :doc:`/api-reference/storage/vfs`
- ``read()``, ``readv()``, ``write()``, ``writev()``: via :doc:`/api-reference/storage/vfs`
- ``recv()``, ``recvmsg()``, ``recvfrom()``
- ``send()``, ``sendmsg()``, ``sendto()``
- ``select()``: via :doc:`/api-reference/storage/vfs`
- ``poll()`` : on ESP-IDF, ``poll()`` is implemented by calling ``select()`` internally, so using ``select()`` directly is recommended, if a choice of methods is available
- ``fcntl()``: see `fcntl()`_

Non-standard functions:

- ``ioctl()``: see `ioctl()`_

.. note::

  Some lwIP application sample code uses prefixed versions of BSD APIs, e.g., ``lwip_socket()``, instead of the standard ``socket()``. Both forms can be used with ESP-IDF, but using standard names is recommended.

Socket Error Handling
^^^^^^^^^^^^^^^^^^^^^

BSD Socket error handling code is very important for robust socket applications. Normally, socket error handling involves the following aspects:

- Detecting the error
- Getting the error reason code
- Handling the error according to the reason code

In lwIP, we have two different scenarios for handling socket errors:

- Socket API returns an error. For more information, see `Socket API Errors`_.
- ``select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout)`` has an exception descriptor indicating that the socket has an error. For more information, see `select() Errors`_.

Socket API Errors
+++++++++++++++++

**Error detection**

  - We can know that the socket API fails according to its return value.

**Get the error reason code**

  - When socket API fails, the return value does not contain the failure reason and the application can get the error reason code by accessing ``errno``. Different values indicate different meanings. For more information, see `Socket Error Reason Code`_.

Example:

.. code-block::

        int err;
        int sockfd;

        if (sockfd = socket(AF_INET,SOCK_STREAM,0) < 0) {
            // the error code is obtained from errno
            err = errno;
            return err;
        }

``select()`` Errors
+++++++++++++++++++

**Error detection**

  - Socket error when ``select()`` has exception descriptor.

**Get the error reason code**

  - If the ``select()`` indicates that the socket fails, we can not get the error reason code by accessing ``errno``, instead we should call ``getsockopt()`` to get the failure reason code. Since ``select()`` has exception descriptor, the error code is not given to ``errno``.

.. note::

    The ``getsockopt()`` function has the following prototype: ``int getsockopt(int s, int level, int optname, void *optval, socklen_t *optlen)``. Its purpose is to get the current value of the option of any type, any state socket, and store the result in ``optval``. For example, when you get the error code on a socket, you can get it by ``getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &err, &optlen)``.

Example:

.. code-block::

        int err;

        if (select(sockfd + 1, NULL, NULL, &exfds, &tval) <= 0) {
            err = errno;
            return err;
        } else {
            if (FD_ISSET(sockfd, &exfds)) {
                // select() exception set using getsockopt()
                int optlen = sizeof(int);
                getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &err, &optlen);
                return err;
            }
        }

Socket Error Reason Code
++++++++++++++++++++++++

Below is a list of common error codes. For a more detailed list of standard POSIX/C error codes, please see `newlib errno.h <https://github.com/espressif/newlib-esp32/blob/master/newlib/libc/include/sys/errno.h>`_ and the platform-specific extensions :component_file:`newlib/platform_include/errno.h`.

.. list-table::
    :header-rows: 1
    :widths: 50 50
    :align: center

    * - Error code
      - Description
    * - ECONNREFUSED
      - Connection refused
    * - EADDRINUSE
      - Address already in use
    * - ECONNABORTED
      - Software caused connection abort
    * - ENETUNREACH
      - Network is unreachable
    * - ENETDOWN
      - Network interface is not configured
    * - ETIMEDOUT
      - Connection timed out
    * - EHOSTDOWN
      - Host is down
    * - EHOSTUNREACH
      - Host is unreachable
    * - EINPROGRESS
      - Connection already in progress
    * - EALREADY
      - Socket already connected
    * - EDESTADDRREQ
      - Destination address required
    * - EPROTONOSUPPORT
      - Unknown protocol

Socket Options
^^^^^^^^^^^^^^

The ``getsockopt()`` and ``setsockopt()`` functions allow getting and setting per-socket options respectively.

Not all standard socket options are supported by lwIP in ESP-IDF. The following socket options are supported:

Common Options
++++++++++++++

Used with level argument ``SOL_SOCKET``.

- ``SO_REUSEADDR``: available if :ref:`CONFIG_LWIP_SO_REUSE` is set, whose behavior can be customized by setting :ref:`CONFIG_LWIP_SO_REUSE_RXTOALL`
- ``SO_KEEPALIVE``
- ``SO_BROADCAST``
- ``SO_ACCEPTCONN``
- ``SO_RCVBUF``: available if :ref:`CONFIG_LWIP_SO_RCVBUF` is set
- ``SO_SNDTIMEO`` / ``SO_RCVTIMEO``
- ``SO_ERROR``: only used with ``select()``, see `Socket Error Handling`_
- ``SO_TYPE``
- ``SO_NO_CHECK``: for UDP sockets only

IP Options
++++++++++

Used with level argument ``IPPROTO_IP``.

- ``IP_TOS``
- ``IP_TTL``
- ``IP_PKTINFO``: available if :ref:`CONFIG_LWIP_NETBUF_RECVINFO` is set

For multicast UDP sockets:

- ``IP_MULTICAST_IF``
- ``IP_MULTICAST_LOOP``
- ``IP_MULTICAST_TTL``
- ``IP_ADD_MEMBERSHIP``
- ``IP_DROP_MEMBERSHIP``

TCP Options
+++++++++++

TCP sockets only. Used with level argument ``IPPROTO_TCP``.

- ``TCP_NODELAY``

Options relating to TCP keepalive probes:

- ``TCP_KEEPALIVE``: int value, TCP keepalive period in milliseconds
- ``TCP_KEEPIDLE``: same as ``TCP_KEEPALIVE``, but the value is in seconds
- ``TCP_KEEPINTVL``: int value, the interval between keepalive probes in seconds
- ``TCP_KEEPCNT``: int value, number of keepalive probes before timing out

IPv6 Options
++++++++++++

IPv6 sockets only. Used with level argument ``IPPROTO_IPV6``.

- ``IPV6_CHECKSUM``
- ``IPV6_V6ONLY``

For multicast IPv6 UDP sockets:

- ``IPV6_JOIN_GROUP`` / ``IPV6_ADD_MEMBERSHIP``
- ``IPV6_LEAVE_GROUP`` / ``IPV6_DROP_MEMBERSHIP``
- ``IPV6_MULTICAST_IF``
- ``IPV6_MULTICAST_HOPS``
- ``IPV6_MULTICAST_LOOP``

``fcntl()``
^^^^^^^^^^^

The ``fcntl()`` function is a standard API for manipulating options related to a file descriptor. In ESP-IDF, the :doc:`/api-reference/storage/vfs` layer is used to implement this function.

When the file descriptor is a socket, only the following ``fcntl()`` values are supported:

- ``O_NONBLOCK`` to set or clear non-blocking I/O mode. Also supports ``O_NDELAY``, which is identical to ``O_NONBLOCK``.
- ``O_RDONLY``, ``O_WRONLY``, ``O_RDWR`` flags for different read or write modes. These flags can only be read using ``F_GETFL``, and cannot be set using ``F_SETFL``. A TCP socket returns a different mode depending on whether the connection has been closed at either end or is still open at both ends. UDP sockets always return ``O_RDWR``.

``ioctl()``
^^^^^^^^^^^

The ``ioctl()`` function provides a semi-standard way to access some internal features of the TCP/IP stack. In ESP-IDF, the :doc:`/api-reference/storage/vfs` layer is used to implement this function.

When the file descriptor is a socket, only the following ``ioctl()`` values are supported:

- ``FIONREAD`` returns the number of bytes of the pending data already received in the socket's network buffer.
- ``FIONBIO`` is an alternative way to set/clear non-blocking I/O status for a socket, equivalent to ``fcntl(fd, F_SETFL, O_NONBLOCK, ...)``.

Netconn API
-----------

lwIP supports two lower-level APIs as well as the BSD Sockets API: the Netconn API and the Raw API.

The lwIP Raw API is designed for single-threaded devices and is not supported in ESP-IDF.

The Netconn API is used to implement the BSD Sockets API inside lwIP, and it can also be called directly from ESP-IDF apps. This API has lower resource usage than the BSD Sockets API. In particular, it can send and receive data without firstly copying it into internal lwIP buffers.

.. important::

  Espressif does not test the Netconn API in ESP-IDF. As such, this functionality is **enabled but not supported**. Some functionality may only work correctly when used from the BSD Sockets API.

For more information about the Netconn API, consult `lwip/lwip/src/include/lwip/api.h <http://www.nongnu.org/lwip/2_0_x/api_8h.html>`_ and `part of the **unofficial** lwIP Application Developers Manual <https://lwip.fandom.com/wiki/Netconn_API>`_.

lwIP FreeRTOS Task
------------------

lwIP creates a dedicated TCP/IP FreeRTOS task to handle socket API requests from other tasks.

A number of configuration items are available to modify the task and the queues (mailboxes) used to send data to/from the TCP/IP task:

- :ref:`CONFIG_LWIP_TCPIP_RECVMBOX_SIZE`
- :ref:`CONFIG_LWIP_TCPIP_TASK_STACK_SIZE`
- :ref:`CONFIG_LWIP_TCPIP_TASK_AFFINITY`

IPv6 Support
------------

Both IPv4 and IPv6 are supported in a dual-stack configuration and are enabled by default. Both IPv6 and IPv4 may be disabled separately if they are not needed, see :ref:`lwip-ram-usage`.

IPv6 support is limited to **Stateless Autoconfiguration** only. **Stateful configuration** is not supported in ESP-IDF, nor in upstream lwIP.

IPv6 Address configuration is defined by means of these protocols or services:

- **SLAAC** IPv6 Stateless Address Autoconfiguration (RFC-2462)
- **DHCPv6** Dynamic Host Configuration Protocol for IPv6 (RFC-8415)

None of these two types of address configuration is enabled by default, so the device uses only Link Local addresses or statically-defined addresses.

.. _lwip-ivp6-autoconfig:

Stateless Autoconfiguration Process
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To enable address autoconfiguration using the Router Advertisement protocol, please enable:

- :ref:`CONFIG_LWIP_IPV6_AUTOCONFIG`

This configuration option enables IPv6 autoconfiguration for all network interfaces, which differs from the upstream lwIP behavior, where the autoconfiguration needs to be explicitly enabled for each ``netif`` with ``netif->ip6_autoconfig_enabled=1``.

.. _lwip-ivp6-dhcp6:

DHCPv6
^^^^^^

DHCPv6 in lwIP is very simple and supports only stateless configuration. It could be enabled using:

- :ref:`CONFIG_LWIP_IPV6_DHCP6`

Since the DHCPv6 works only in its stateless configuration, the :ref:`lwip-ivp6-autoconfig` has to be enabled as well via :ref:`CONFIG_LWIP_IPV6_AUTOCONFIG`.

Moreover, the DHCPv6 needs to be explicitly enabled from the application code using:

.. code-block::

    dhcp6_enable_stateless(netif);

DNS Servers in IPv6 Autoconfiguration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In order to autoconfigure DNS server(s), especially in IPv6-only networks, we have these two options:

- Recursive Domain Name System (DNS): this belongs to the Neighbor Discovery Protocol (NDP) and uses :ref:`lwip-ivp6-autoconfig`.

  The number of servers must be set :ref:`CONFIG_LWIP_IPV6_RDNSS_MAX_DNS_SERVERS`, this option is disabled by default, i.e., set to 0.

- DHCPv6 stateless configuration, uses :ref:`lwip-ivp6-dhcp6` to configure DNS servers. Note that this configuration assumes IPv6 Router Advertisement Flags (RFC-5175) to be set to

    - Managed Address Configuration Flag = 0
    - Other Configuration Flag = 1

ESP-lwIP Custom Modifications
-----------------------------

Additions
^^^^^^^^^

The following code is added, which is not present in the upstream lwIP release:

Thread-Safe Sockets
+++++++++++++++++++

It is possible to ``close()`` a socket from a different thread than the one that created it. The ``close()`` call blocks, until any function calls currently using that socket from other tasks have returned.

It is, however, not possible to delete a task while it is actively waiting on ``select()`` or ``poll()`` APIs. It is always necessary that these APIs exit before destroying the task, as this might corrupt internal structures and cause subsequent crashes of the lwIP. These APIs allocate globally referenced callback pointers on the stack so that when the task gets destroyed before unrolling the stack, the lwIP could still hold pointers to the deleted stack.

On-Demand Timers
++++++++++++++++

lwIP IGMP and MLD6 feature both initialize a timer in order to trigger timeout events at certain times.

The default lwIP implementation is to have these timers enabled all the time, even if no timeout events are active. This increases CPU usage and power consumption when using automatic Light-sleep mode. ``ESP-lwIP`` default behavior is to set each timer ``on demand``, so it is only enabled when an event is pending.

To return to the default lwIP behavior, which is always-on timers, disable :ref:`CONFIG_LWIP_TIMERS_ONDEMAND`.

lwIP Timers API
+++++++++++++++

When not using Wi-Fi, the lwIP timer can be turned off via the API to reduce power consumption.

The following API functions are supported. For full details, see :component_file:`lwip/lwip/src/include/lwip/timeouts.h`.

- ``sys_timeouts_init()``
- ``sys_timeouts_deinit()``

Additional Socket Options
+++++++++++++++++++++++++

- Some standard IPV4 and IPV6 multicast socket options are implemented, see `Socket Options`_.

- Possible to set IPV6-only UDP and TCP sockets with ``IPV6_V6ONLY`` socket option, while normal lwIP is TCP-only.

IP Layer Features
+++++++++++++++++

- IPV4-source-based routing implementation is different

- IPV4-mapped IPV6 addresses are supported

NAPT and Port Forwarding
++++++++++++++++++++++++

IPV4 network address port translation (NAPT) and port forwarding are supported. However, the enabling of NAPT is limited to a single interface.

- To use NAPT for forwarding packets between two interfaces, it needs to be enabled on the interface connecting to the target network. For example, to enable internet access for Ethernet traffic through the Wi-Fi interface, NAPT must be enabled on the Ethernet interface.
- Usage of NAPT is demonstrated in :example:`network/vlan_support`.

.. _lwip-custom-hooks:

Customized lwIP Hooks
+++++++++++++++++++++

The original lwIP supports implementing custom compile-time modifications via ``LWIP_HOOK_FILENAME``. This file is already used by the ESP-IDF port layer, but ESP-IDF users could still include and implement any custom additions via a header file defined by the macro ``ESP_IDF_LWIP_HOOK_FILENAME``. Here is an example of adding a custom hook file to the build process, and the hook is called ``my_hook.h``, located in the project's ``main`` folder:

.. code-block:: cmake

   idf_component_get_property(lwip lwip COMPONENT_LIB)
   target_compile_options(${lwip} PRIVATE "-I${PROJECT_DIR}/main")
   target_compile_definitions(${lwip} PRIVATE "-DESP_IDF_LWIP_HOOK_FILENAME=\"my_hook.h\"")

Customized lwIP Options From ESP-IDF Build System
++++++++++++++++++++++++++++++++++++++++++++++++++

The most common lwIP options are configurable through the component configuration menu. However, certain definitions need to be injected from the command line. The CMake function ``target_compile_definitions()`` can be employed to define macros, as illustrated below:

.. code-block:: cmake

   idf_component_get_property(lwip lwip COMPONENT_LIB)
   target_compile_definitions(${lwip} PRIVATE "-DETHARP_SUPPORT_VLAN=1")

This approach may not work for function-like macros, as there is no guarantee that the definition will be accepted by all compilers, although it is supported in GCC. To address this limitation, the ``add_definitions()`` function can be utilized to define the macro for the entire project, for example: ``add_definitions("-DFALLBACK_DNS_SERVER_ADDRESS(addr)=\"IP_ADDR4((addr), 8,8,8,8)\"")``.

Alternatively, you can define your function-like macro in a header file which will be pre-included as an lwIP hook file, see :ref:`lwip-custom-hooks`.

Limitations
^^^^^^^^^^^

ESP-IDF additions to lwIP still suffer from the global DNS limitation, described in :ref:`lwip-dns-limitation`. To address this limitation from application code, the ``FALLBACK_DNS_SERVER_ADDRESS()`` macro can be utilized to define a global DNS fallback server accessible from all interfaces. Alternatively, you have the option to maintain per-interface DNS servers and reconfigure them whenever the default interface changes.

Calling ``send()`` or ``sendto()`` repeatedly on a UDP socket may eventually fail with ``errno`` equal to ``ENOMEM``. This failure occurs due to the limitations of buffer sizes in the lower-layer network interface drivers. If all driver transmit buffers are full, the UDP transmission will fail. For applications that transmit a high volume of UDP datagrams and aim to avoid any dropped datagrams by the sender, it is advisable to implement error code checking and employ a retransmission mechanism with a short delay.

.. only:: esp32

    Increasing the number of TX buffers in the :ref:`Wi-Fi <CONFIG_ESP_WIFI_TX_BUFFER>` or :ref:`Ethernet <CONFIG_ETH_DMA_TX_BUFFER_NUM>` project configuration as applicable may also help.

.. only:: not esp32 and SOC_WIFI_SUPPORTED

    Increasing the number of TX buffers in the :ref:`Wi-Fi <CONFIG_ESP_WIFI_TX_BUFFER>` project configuration may also help.

.. _lwip-performance:

Performance Optimization
------------------------

TCP/IP performance is a complex subject, and performance can be optimized toward multiple goals. The default settings of ESP-IDF are tuned for a compromise between throughput, latency, and moderate memory usage.

Maximum Throughput
^^^^^^^^^^^^^^^^^^

Espressif tests ESP-IDF TCP/IP throughput using the iperf test application: https://iperf.fr/, please refer to :ref:`improve-network-speed` for more details about the actual testing and using the optimized configuration.

.. important::

  Suggest applying changes a few at a time and checking the performance each time with a particular application workload.

- If a lot of tasks are competing for CPU time on the system, consider that the lwIP task has configurable CPU affinity (:ref:`CONFIG_LWIP_TCPIP_TASK_AFFINITY`) and runs at fixed priority (18, ``ESP_TASK_TCPIP_PRIO``). To optimize CPU utilization, consider assigning competing tasks to different cores or adjusting their priorities to lower values. For additional details on built-in task priorities, please refer to :ref:`built-in-task-priorities`.

- If using ``select()`` function with socket arguments only, disabling :ref:`CONFIG_VFS_SUPPORT_SELECT` will make ``select()`` calls faster.

- If there is enough free IRAM, select :ref:`CONFIG_LWIP_IRAM_OPTIMIZATION` and :ref:`CONFIG_LWIP_EXTRA_IRAM_OPTIMIZATION` to improve TX/RX throughput.

.. only:: SOC_WIFI_SUPPORTED

    If using a Wi-Fi network interface, please also refer to :ref:`wifi-buffer-usage`.

Minimum Latency
^^^^^^^^^^^^^^^

Except for increasing buffer sizes, most changes that increase throughput also decrease latency by reducing the amount of CPU time spent in lwIP functions.

- For TCP sockets, lwIP supports setting the standard ``TCP_NODELAY`` flag to disable Nagle's algorithm.

.. _lwip-ram-usage:

Minimum RAM Usage
^^^^^^^^^^^^^^^^^

Most lwIP RAM usage is on-demand, as RAM is allocated from the heap as needed. Therefore, changing lwIP settings to reduce RAM usage may not change RAM usage at idle, but can change it at peak.

- Reducing :ref:`CONFIG_LWIP_MAX_SOCKETS` reduces the maximum number of sockets in the system. This also causes TCP sockets in the ``WAIT_CLOSE`` state to be closed and recycled more rapidly when needed to open a new socket, further reducing peak RAM usage.
- Reducing :ref:`CONFIG_LWIP_TCPIP_RECVMBOX_SIZE`, :ref:`CONFIG_LWIP_TCP_RECVMBOX_SIZE` and :ref:`CONFIG_LWIP_UDP_RECVMBOX_SIZE` reduce RAM usage at the expense of throughput, depending on usage.
- Reducing :ref:`CONFIG_LWIP_TCP_ACCEPTMBOX_SIZE` reduce RAM usage by limiting concurrent accepted connections.
- Reducing :ref:`CONFIG_LWIP_TCP_MSL` and :ref:`CONFIG_LWIP_TCP_FIN_WAIT_TIMEOUT` reduces the maximum segment lifetime in the system. This also causes TCP sockets in the ``TIME_WAIT`` and ``FIN_WAIT_2`` states to be closed and recycled more rapidly.
- Disabling :ref:`CONFIG_LWIP_IPV6` can save about 39 KB for firmware size and 2 KB RAM when the system is powered up and 7 KB RAM when the TCP/IP stack is running. If there is no requirement for supporting IPV6, it can be disabled to save flash and RAM footprint.
- Disabling :ref:`CONFIG_LWIP_IPV4` can save about 26 KB of firmware size and 600 B RAM on power up and 6 KB RAM when the TCP/IP stack is running. If the local network supports IPv6-only configuration, IPv4 can be disabled to save flash and RAM footprint.

.. only:: SOC_WIFI_SUPPORTED

    If using Wi-Fi, please also refer to :ref:`wifi-buffer-usage`.


Peak Buffer Usage
+++++++++++++++++

The peak heap memory that lwIP consumes is the **theoretically-maximum memory** that the lwIP driver consumes. Generally, the peak heap memory that lwIP consumes depends on:

 - the memory required to create a UDP connection: ``lwip_udp_conn``
 - the memory required to create a TCP connection: ``lwip_tcp_conn``
 - the number of UDP connections that the application has: ``lwip_udp_con_num``
 - the number of TCP connections that the application has: ``lwip_tcp_con_num``
 - the TCP TX window size: ``lwip_tcp_tx_win_size``
 - the TCP RX window size: ``lwip_tcp_rx_win_size``

**So, the peak heap memory that the lwIP consumes can be calculated with the following formula:**
  lwip_dynamic_peek_memory =  (lwip_udp_con_num * lwip_udp_conn)  + (lwip_tcp_con_num * (lwip_tcp_tx_win_size + lwip_tcp_rx_win_size + lwip_tcp_conn))

Some TCP-based applications need only one TCP connection. However, they may choose to close this TCP connection and create a new one when an error occurs (e.g., a sending failure). This may result in multiple TCP connections existing in the system simultaneously, because it may take a long time for a TCP connection to close, according to the TCP state machine, refer to RFC793.


.. _lwIP lightweight TCP/IP stack: https://savannah.nongnu.org/projects/lwip/
.. _esp-lwip: https://github.com/espressif/esp-lwip
