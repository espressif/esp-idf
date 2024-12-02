ESP-NETIF
*********

:link_to_translation:`en:[English]`

ESP-NETIF 库的作用主要体现在以下两方面：

- 在 TCP/IP 协议栈之上为应用程序提供抽象层，允许应用程序在 IP 栈间选择。
- 在底层 TCP/IP 协议栈 API 非线程安全的情况下，也能提供线程安全的 API。

ESP-IDF 目前只为 lwIP TCP/IP 协议栈实现了 ESP-NETIF。然而，该适配器本身不依赖于特定的 TCP/IP 实现，因而支持不同实现方式。

一些 ESP-NETIF API 函数是为应用程序代码调用而设计的，例如获取或设置接口 IP 地址和配置 DHCP，其他函数则由网络驱动层的内部 ESP-IDF 使用。大多数情况下，应用程序不需要直接调用 ESP-NETIF API，因为这些 API 由默认的网络事件处理程序调用。

如果你只对默认配置下的常见网络接口感兴趣，请参考 :ref:`esp_netif_user`，查看如何初始化默认接口并注册事件处理程序。

如果想了解更多关于 ESP-NETIF 库与其他组件交互的信息，请参考 :ref:`esp-netif structure`。

如果想自定义应用程序的网络接口，例如设置静态 IP 地址或在运行时更新配置，请参考 :ref:`esp_netif_programmer`。

如果想开发自己的网络驱动程序，实现对新 TCP/IP 协议栈的支持或以其他方式自定义 ESP-NETIF，请参考 :ref:`esp_netif_developer`。

.. _esp_netif_user:

ESP-NETIF 用户指南
==================

通常只需要在启动后创建一个默认的网络接口，并在关闭时销毁它即可（请参阅 :ref:`esp_netif_init`）。在分配新 IP 地址或失去 IP 地址时接收通知也是非常有用的（请参阅 :ref:`esp-netif-ip-events`）。


.. _esp_netif_init:

初始化
------

ESP-NETIF 组件使用了系统事件，典型的网络启动代码如下所示（为了清晰起见，省略了错误处理部分，完整版启动代码请参阅 :example_file:`ethernet/basic/main/ethernet_example_main.c`）：

.. code-block:: c

    // 1) 初始化 TCP/IP 协议栈和事件循环
    esp_netif_init();
    esp_event_loop_create_default();

    // 2) 创建网络接口句柄
    esp_netif = esp_netif_new(&config);

    // 3) 创建网络接口驱动程序（如以太网）及其网络层连接
    // 并注册 ESP-NETIF 事件（如，在发生连接事件时将启用接口）
    esp_netif_glue_t glue = driver_glue(driver);

    // 4) 将驱动程序的胶水层附加到网络接口句柄上
    esp_netif_attach(esp_netif, glue);

    // 5) 注册用户端的事件处理程序
    esp_event_handler_register(DRIVER_EVENT, ...);  // 观察驱动状态，例如链路连接
    esp_event_handler_register(IP_EVENT, ...);      // 观察 ESP-NETIF 状态，例如获取 IP


.. note::

    以上步骤需严格按照顺序执行，因为网络接口驱动程序注册系统事件时会使用默认的事件循环。

        - 默认的事件循环应在初始化接口驱动程序 **之前** 创建，因为驱动程度通常需要注册系统事件处理程序。
        - 注册应用程序事件处理程序应在调用 :cpp:func:`esp_netif_attach` **之后** 进行，因为事件处理程序按其注册顺序被调用。为了确保系统处理程序先被调用，应该在其后注册应用处理程序。

    步骤 ``2)``、``3)``、``4)`` 对于大多数常见用例来说相当复杂，因此 ESP-NETIF 提供了一些预配置的接口和便利函数，从而使用最常见的配置创建最常见的网络接口。

.. note::

    每个网络接口都需要单独初始化，因此如果要使用多个接口，必须为每个接口运行步骤 ``2)`` 到步骤 ``5)``，而步骤 ``1)`` 仅需执行一次。


有关如何创建和配置接口并将网络接口驱动程序附加到接口（步骤 ``2)``、``3)``、``4)``），请参阅 :ref:`create_esp_netif`。

有关如何使用 ESP-NETIF 事件处理程序（步骤 ``5)``），请参阅 :ref:`esp-netif-ip-events`。


.. _create_esp_netif:

常见的网络接口
^^^^^^^^^^^^^^

初始化网络接口可能有些复杂，ESP-NETIF 提供了一些创建常见接口（例如 Wi-Fi 和以太网）的快捷方式。

请参考以下示例来了解如何初始化默认接口：

.. list::

    :SOC_WIFI_SUPPORTED: - :example:`wifi/getting_started/station` 演示了如何使用 station 功能将 {IDF_TARGET_NAME} 连接到 AP。

    :CONFIG_ESP_WIFI_SOFTAP_SUPPORT: - :example:`wifi/getting_started/softAP` 演示了如何使用 SoftAP 功能将 {IDF_TARGET_NAME} 配置为 AP。

    - :example:`ethernet/basic` 演示了如何使用以太网驱动程序，将其添加到 `esp_netif`，并获取一个可 ping 的 IP 地址。

    - :example:`protocols/l2tap` 演示了如何使用 ESP-NETIF L2 TAP 接口访问数据链路层，以接收和传输帧，实现非 IP 协议，并使用特定的 EthTypes 回显以太网帧。

    - :example:`protocols/static_ip` 演示了如何将 Wi-Fi 配置为 station，包括设置静态 IP、子网掩码、网关和 DNS 服务器。

.. only:: SOC_WIFI_SUPPORTED

Wi-Fi 默认初始化
^^^^^^^^^^^^^^^^

初始化代码以及注册默认接口（例如 softAP 和基站）的事件处理程序都在单独的 API 中提供，从而简化应用程序的启动代码：

* :cpp:func:`esp_netif_create_default_wifi_sta()`

.. only:: CONFIG_ESP_WIFI_SOFTAP_SUPPORT

    * :cpp:func:`esp_netif_create_default_wifi_ap()`

.. only:: SOC_WIFI_SUPPORTED

    请注意，这些函数会返回 ``esp_netif`` 句柄，即分配并配置了默认设置的网络接口对象的指针，这意味着：

    * 如果应用程序使用 :cpp:func:`esp_netif_destroy_default_wifi()` 提供网络去初始化，则创建的对象必须被销毁。

    * 这些 *默认* 接口不能被多次创建，除非使用 :cpp:func:`esp_netif_destroy_default_wifi()` 删除已创建的句柄。


.. only:: CONFIG_ESP_WIFI_SOFTAP_SUPPORT

    * 在 ``AP+STA`` 模式下使用 Wi-Fi 时，须创建以上全部接口。请参考示例 :example_file:`wifi/softap_sta/main/softap_sta.c`。

.. _esp-netif-ip-events:

IP 事件
-------

:ref:`esp_netif_init` 代码的最后一部分（步骤 ``5)``）注册了两组事件处理程序：

* **网络接口驱动事件**：用于通知驱动的生命周期状态，例如 Wi-Fi 站点加入 AP 或断开连接。处理这些事件不在 ESP-NETIF 组件的范围内。值得一提的是，ESP-NETIF 也使用相同的事件将网络接口设置为所需状态。因此，如果应用程序使用驱动事件来确定网络接口的特定状态，则应在注册系统处理程序 **之后** 再注册事件处理程序（通常在将驱动程序附加到接口时进行注册）。这就是为什么事件处理程序的注册是在 :ref:`esp_netif_init` 代码的最后一步进行。

* **IP 事件**：用于通知 IP 地址的变化，例如分配新地址或失去有效地址。这些事件的特定类型可参见 :cpp:type:`ip_event_t`。每个常见的接口都有相应的 ``GOT_IP`` 和 ``LOST_IP`` 事件。

由于网络的异步特性，注册事件处理程序非常关键，因为网络状态的变化可能是不可预测的。通过注册事件处理程序，应用程序可以及时响应这些变化，确保在网络事件发生时采取适当的措施。

.. note::

    丢失 IP 事件由一个可配置的定时器触发，配置项为 :ref:`CONFIG_ESP_NETIF_IP_LOST_TIMER_INTERVAL`。当 IP 地址丢失时定时器启动，事件将在配置的时间间隔后触发，默认值为 120 秒。将时间间隔设置为 0 时可禁用该事件。

.. _esp-netif structure:

ESP-NETIF 架构
--------------

.. code-block:: text


                         |            (A) 用户代码                 |
                         |                应用程序                 |
        .................| 初始化            设置             事件  |
        .                +----------------------------------------+
        .                   .                |           *
        .                   .                |           *
    --------+            +===========================+   *     +-----------------------+
            |            | 新建/配置   获取/设置/应用程序|   *     | 初始化                 |
            |            |                           |...*.....| 应用程序 (DHCP, SNTP)  |
            |            |---------------------------|   *     |                       |
      初始化 |            |                           |****     |                       |
      启动   |************|  事件处理程序               |*********|  DHCP                 |
      停止   |            |                           |         |                       |
            |            |---------------------------|         |                       |
            |            |                           |         |    NETIF              |
      +-----|            |                           |         +-----------------+     |
      |胶水层|---<----|---|  esp_netif_transmit       |--<------| netif_output    |     |
      |     |        |   |                           |         |                 |     |
      |     |--->----|---|  esp_netif_receive        |-->------| netif_input     |     |
      |     |        |   |                           |         + ----------------+     |
      |     |...<....|...|  esp_netif_free_rx_buffer |...<.....| 数据包 buffer          |
      +-----|     |  |   |                           |         |                       |
            |     |  |   |                           |         |         (D)           |
      (B)   |     |  |   |          (C)              |         +-----------------------+
    --------+     |  |   +===========================+                 网络堆栈
  网络             |  |           ESP-NETIF
  接口             |  |
  驱动             |  |   +--------------------------------+         +------------------+
                  |  |   |                                |.........| 开启/关闭          |
                  |  |   |                                |         |                  |
                  |  -<--|  l2tap_write                   |-----<---|  写入             |
                  |      |                                |         |                  |
                  ---->--| esp_vfs_l2tap_eth_filter_frame |----->---|  读取             |
                         |                                |         |        (A)       |
                         |            (E)                 |         +------------------+
                         +--------------------------------+                用户代码
                               ESP-NETIF L2 TAP


图中不同线段对应的数据流和事件流
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* ``........``     从用户代码到 ESP-NETIF 和网络接口驱动的初始化线

* ``--<--->--``    数据包在通信媒介与 TCP/IP 协议栈之间往返

* ``********``     聚集在 ESP-NETIF 中的事件传递到驱动程序、用户代码和网络堆栈中

* ``|``            用户设置及运行时间配置

ESP-NETIF 交互
^^^^^^^^^^^^^^

A) 用户代码样板
'''''''''''''''

通过使用 ESP-NETIF API 抽象，应用程序与用于通信介质（网络接口驱动）的特定 IO 驱动程序以及配置的 TCP/IP 网络栈之间的交互可以概括如下：

A) 初始化代码

    1) 初始化 IO 驱动
    2) 创建新的 ESP-NETIF 实例，并完成以下配置：

        * ESP-NETIF 的特定选项（标志、行为、名称）
        * 网络栈堆选项（netif 初始化和输入函数，非公开信息）
        * IO 驱动的特定选项（发送、释放 rx buffer 函数、IO 驱动句柄）

    3) 将 IO 驱动句柄附加到上述步骤所创建的 ESP-NETIF 实例
    4) 配置事件处理程序

        * 对 IO 驱动定义的公共接口使用默认处理程序；或为定制的行为或新接口定义特定的处理程序
        * 为应用程序相关事件（如 IP 丢失或获取）注册处理程序

B) 通过 ESP-NETIF API 与网络接口交互

    1) 获取、设置 TCP/IP 相关参数（如 DHCP、IP 等）
    2) 接收 IP 事件（连接或断连）
    3) 控制应用程序生命周期（启用或禁用接口）


B) 网络接口驱动
'''''''''''''''

对于 ESP-NETIF，网络接口驱动（也被称为 I/O 驱动或媒体驱动）具有以下两大作用：

1) 事件驱动程序：定义与 ESP-NETIF 交互的行为模式（如：连接以太网 -> 开启 netif）

2) 胶合 IO 层：调整输入或输出函数以使用 ESP-NETIF 的传输、接收，并清空接收 buffer

    * 给适当的 ESP-NETIF 对象安装 driver_transmit，以便将网络堆栈中传出的数据包传输给 IO 驱动
    * 调用函数 :cpp:func:`esp_netif_receive()` 以便将传入的数据传输给网络堆栈


C) ESP-NETIF
''''''''''''

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
    * :ref:`esp_netif-sntp-api`

6) 驱动转换工具 API


D) 网络堆栈
'''''''''''

网络堆栈与应用程序代码在公共接口方面无公开交互，需通过 ESP-NETIF API 实现完全抽象。


E) ESP-NETIF L2 TAP 接口
''''''''''''''''''''''''

ESP-NETIF L2 TAP 接口是 ESP-IDF 中的一种机制，用于让用户应用程序访问数据链路层（OSI/ISO 中的 L2），以便接收和传输帧。在嵌入式开发中，它通常用于实现与 IP 无关的协议，例如 如 PTP 和 Wake on LAN 等。请注意，目前仅支持以太网 (IEEE 802.3)。更多关于 L2 TAP 的信息，请参阅 :ref:`esp_netif_l2tap`。

.. _esp_netif_programmer:

ESP-NETIF 编程指南
==================

在某些情况下，仅初始化一个默认的网络接口、使用该接口并连接到本地网络是不够的。此时请参考编程指南：:doc:`/api-reference/network/esp_netif_programming`。

通常需要在以下用例中使用特定的 ESP-NETIF API 集：

* :ref:`esp_netif_set_ip`
* :ref:`esp_netif_set_dhcp`
* :ref:`esp_netif-sntp-api`
* :ref:`esp_netif_l2tap`
* :ref:`esp_netif_other_events`
* :ref:`esp_netif_api_reference`

.. _esp_netif_developer:

ESP-NETIF 开发者指南
====================

在某些情况下，用户应用程序可能需要自定义 ESP-NETIF、注册自定义驱动程序、甚至使用自定义的 TCP/IP 协议栈。此时请参考 :doc:`/api-reference/network/esp_netif_driver`。
