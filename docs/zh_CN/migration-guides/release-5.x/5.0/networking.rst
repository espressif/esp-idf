网络
===========

:link_to_translation:`en:[English]`

Wi-Fi
*****

回调函数类型 ``esp_now_recv_cb_t``
----------------------------------

此前 ``esp_now_recv_cb_t`` 的第一个参数的类型是 ``const uint8_t *mac_addr``，该参数只包含对端 ESP-NOW 设备的地址。

现在该函数有所更新。第一个参数的类型变更为 ``esp_now_recv_info_t``， 它包含三个成员变量 ``src_addr``，``des_addr`` 和 ``rx_ctrl``。因此，需要进行如下更新:

    - 重新定义的 ESP-NOW 收包回调函数。

    - ``src_addr`` 可以等价替换原来的 ``mac_addr``。

    - ``des_addr`` 是 ESP-NOW 包的目的 MAC 地址，可以是单播或广播地址。使用 ``des_addr`` 可以区分单播或广播的 ESP-NOW 包，其中，即使是在加密的 ESP-NOW 配置中，广播的 ESP-NOW 包也可以是非加密的。

    - ``rx_ctrl`` 是 ESP-NOW 包的 Rx control info，它包含此包的更多有用信息。

请参考 ESP-NOW 样例：:example_file:`wifi/espnow/main/espnow_example_main.c`

以太网
**************

``esp_eth_ioctl()`` API
-----------------------

此前，:cpp:func:`esp_eth_ioctl` API 存在以下问题：

    - 在某些情况下，第三个参数（数据类型为 ``void /*``）可以接受 ``int``/``bool`` 类型实参（而非指针）作为输入。然而，文档中未描述这些情况。
    - 为了将 ``int``/``bool`` 类型实参作为第三个参数传递，实参将强制转换为 ``void *`` 类型，以防出现如下所示的编译器警告。此等转换可能引起 :cpp:func:`esp_eth_ioctl` 函数的滥用。

.. code-block:: c

    esp_eth_ioctl(eth_handle, ETH_CMD_S_FLOW_CTRL, (void *)true);

因此，我们统一了 :cpp:func:`esp_eth_ioctl` 的用法。现在，该结构体的第三个参数在传递时必须作为指向特定数据类型的指针，表示 :cpp:func:`esp_eth_ioctl` 读取/存储数据的位置。:cpp:func:`esp_eth_ioctl` 的用法如下列代码所示。

设置以太网配置的用例如下：

.. code-block:: c

    eth_duplex_t new_duplex_mode = ETH_DUPLEX_HALF;
    esp_eth_ioctl(eth_handle, ETH_CMD_S_DUPLEX_MODE, &new_duplex_mode);

获取以太网配置的用例如下：

.. code-block:: c

    eth_duplex_t duplex_mode;
    esp_eth_ioctl(eth_handle, ETH_CMD_G_DUPLEX_MODE, &duplex_mode);

KSZ8041/81 和 LAN8720 驱动更新
--------------------------------------------

KSZ8041/81 和 LAN8720 驱动现已更新，以支持相关产品系列中的更多设备（如新一代设备）。上述驱动能够识别特定芯片编号及驱动提供的潜在支持。

更新之后，通用函数将替代特定“芯片编号”函数得以调用：

* 删除 ``esp_eth_phy_new_ksz8041()`` 以及 ``esp_eth_phy_new_ksz8081()``，转而使用 :cpp:func:`esp_eth_phy_new_ksz80xx`
* 删除 ``esp_eth_phy_new_lan8720()``，转而使用 :cpp:func:`esp_eth_phy_new_lan87xx`


ESP NETIF Glue 时间处理程序
-----------------------------------

``esp_eth_set_default_handlers()`` 和 ``esp_eth_clear_default_handlers()`` 函数现已删除。现在可以自动处理以太网默认 IP 层处理程序的注册。如果在注册以太网/IP 事件处理程序之前，你已经按照建议，完全初始化以太网驱动和网络接口，则无需执行任何操作（除了删除受影响的函数）。否则，在注册用户事件处理程序后，应随即启动以太网驱动。

PHY 地址自动检测
---------------------------

以太网 PHY 地址自动检测函数 ``esp_eth_detect_phy_addr()`` 已重命名为 :cpp:func:`esp_eth_phy_802_3_detect_phy_addr`，其声明移至 :component_file:`esp_eth/include/esp_eth_phy_802_3.h`。


SPI 以太网模块初始化
--------------------------------------

SPI 以太网模块的初始化过程已经简化。此前，你需要在实例化 SPI 以太网 MAC 之前，使用 :cpp:func:`spi_bus_add_device` 手动分配 SPI 设备。

现在，SPI 设备已在内部分配，因此无需再调用 :cpp:func:`spi_bus_add_device`。:cpp:class:`eth_dm9051_config_t`、:cpp:class:`eth_w5500_config_t` 和 :cpp:class:`eth_ksz8851snl_config_t` 配置结构体现已包含 SPI 设备配置成员（例如，可以微调可能依赖 PCB 设计的 SPI 时序）。``ETH_DM9051_DEFAULT_CONFIG``、``ETH_W5500_DEFAULT_CONFIG`` 和 ``ETH_KSZ8851SNL_DEFAULT_CONFIG`` 配置初始化宏也已接受新的参数输入。了解 SPI 以太网模块初始化示例，请查看 :doc:`以太网 API 参考指南<../../../api-reference/network/esp_eth>`。

Ethernet 驱动
----------------

用于创建 MAC 实例的 API (`esp_eth_mac_new_*()`) 的输入参数由一个配置参数改为两个，这两个参数用于

* 供应商特定的 MAC 配置
* Ethernet 驱动 MAC 配置

该更新不仅适用于内部 Ethernet MAC :cpp:func:`esp_eth_mac_new_esp32()` 也适用于外部 MAC 设备，如 :cpp:func:`esp_eth_mac_new_ksz8851snl()`、 :cpp:func:`esp_eth_mac_new_dm9051()` 和 :cpp:func:`esp_eth_mac_new_w5500()`。

.. _tcpip-adapter:

TCP/IP 适配器
****************

TCP/IP 适配器是在 ESP-IDF v4.1 之前使用的网络接口抽象组件。本文档概述了从 ``tcpip_adapter`` API 迁移至 :doc:`/api-reference/network/esp_netif` 的过程。


更新网络连接代码
--------------------------------


网络软件栈初始化
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- 你只需用 ``esp_netif_init()`` 替换 ``tcpip_adapter_init()``，注意 ``esp_netif_init()`` 函数现将返回标准错误代码。了解详细信息，请参考 :doc:`/api-reference/network/esp_netif`。
- ``esp_netif_deinit()`` 函数用于反初始化网络软件栈。
- 你还需用 ``#include "esp_netif.h"`` 替换 ``#include "tcpip_adapter.h"``。


创建网络接口
^^^^^^^^^^^^^^^^^^^^^^^^^^

更新之前，TCP/IP 适配器静态定义了以下三个接口：

- Wi-Fi Station
- Wi-Fi AP
- 以太网

接口定义现已更新。网络接口的设计应严格参考 :doc:`/api-reference/network/esp_netif`，使其能够连接至 TCP/IP 软件栈。例如，在 TCP/IP 软件栈和事件循环初始化完成后，Wi-Fi 的初始化代码必须显示调用 ``esp_netif_create_default_wifi_sta();`` 或 ``esp_netif_create_default_wifi_ap();``。

请参考上述三个接口的初始化代码示例：

- Wi-Fi Station：:example_file:`wifi/getting_started/station/main/station_example_main.c`
- Wi-Fi AP：:example_file:`wifi/getting_started/softAP/main/softap_example_main.c`
- 以太网：:example_file:`ethernet/basic/main/ethernet_example_main.c`

其他 ``tcpip_adapter`` API 更换
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

所有 ``tcpip_adapter`` 函数都有对应的 ``esp-netif``。请参考以下章节中的 ``esp_netif.h`` 部分，了解更多信息：

* :component_file:`Setters/Getters <esp_netif/include/esp_netif.h#L241>`
* :component_file:`DHCP <esp_netif/include/esp_netif.h#L387>`
* :component_file:`DNS <esp_netif/include/esp_netif.h#L516>`
* :component_file:`IP address <esp_netif/include/esp_netif.h#L568>`

默认事件处理程序
^^^^^^^^^^^^^^^^^^^^^^

事件处理程序已从 ``tcpip_adapter`` 移至相应驱动程序代码。从应用程序的角度来看，这一变更不会产生任何影响，所有事件仍将以相同的方式处理。请注意，在与 IP 相关的事件处理程序中，应用程序代码通常以 ``esp-netif`` 结构体而非 LwIP 结构体的形式接收 IP 地址。两种结构体均兼容二进制格式。


打印地址的首选方式如下所示：

.. code-block:: c

    ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));

不建议使用下述方式：

.. code-block:: c

    ESP_LOGI(TAG, "got ip:%s", ip4addr_ntoa(&event->ip_info.ip));

``ip4addr_ntoa()`` 为 LwIP API，因此 esp-netif 还提供了替代函数 ``esp_ip4addr_ntoa()``，然而总得来说仍推荐使用 ``IP2STR()`` 这一方法。

IP 地址
^^^^^^^^^^^^

推荐使用 ``esp-netif`` 定义的 IP 结构。请注意，在启用默认兼容性时，LwIP 结构体仍然可以工作。

* :component_file:`esp-netif IP address definitions <esp_netif/include/esp_netif_ip_addr.h#L96>`
