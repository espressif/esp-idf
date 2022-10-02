网络
===========

:link_to_translation:`en:[English]`

以太网
**************

esp_eth_ioctl() API
-------------------

此前，:cpp:func:`esp_eth_ioctl` API 存在以下问题：

    - 在某些情况下，第三个参数（数据类型为 ``void /*``）可以接受 ``int``/``bool`` 类型实参（而非指针）作为输入。然而，文档中未描述这些情况。
    - 为了将 ``int``/``bool`` 类型实参作为第三个参数传递，实参将被强制转换为 ``void *`` 类型，以防出现如下所示的编译器警告。此等转换可能引起 :cpp:func:`esp_eth_ioctl` 函数的滥用。

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

``esp_eth_set_default_handlers()`` 和 ``esp_eth_clear_default_handlers()`` 函数现已删除。现在可以自动处理以太网默认 IP 层处理程序的注册。如您在注册以太网/IP 事件处理程序之前已经按照建议完全初始化以太网驱动和网络接口，则无需执行任何操作（除了删除受影响的函数）。否则，在注册用户事件处理程序后，应随即启动以太网驱动。

PHY 地址自动检测
---------------------------

以太网 PHY 地址自动检测函数 ``esp_eth_detect_phy_addr()`` 已重命名为 :cpp:func:`esp_eth_phy_802_3_detect_phy_addr`，其声明移至 :component_file:`esp_eth/include/esp_eth_phy_802_3.h`。


SPI 以太网模块初始化
--------------------------------------

SPI 以太网模块的初始化过程已经简化。此前，您需要在实例化 SPI 以太网 MAC 之前，使用 :cpp:func:`spi_bus_add_device` 手动分配 SPI 设备。

现在，由于 SPI 设备已在内部分配，您无需再调用 :cpp:func:`spi_bus_add_device`。:cpp:class:`eth_dm9051_config_t`、:cpp:class:`eth_w5500_config_t` 和 :cpp:class:`eth_ksz8851snl_config_t` 配置结构体现已包含 SPI 设备配置成员（例如，可以微调可能依赖 PCB 设计的 SPI 时序）。``ETH_DM9051_DEFAULT_CONFIG``、``ETH_W5500_DEFAULT_CONFIG`` 和 ``ETH_KSZ8851SNL_DEFAULT_CONFIG`` 配置初始化宏也已接受新的参数输入。了解 SPI 以太网模块初始化示例，请查看 :doc:`以太网 API 参考指南<../../api-reference/network/esp_eth>`。


.. _tcpip-adapter:

TCP/IP 适配器
****************

TCP/IP 适配器是在 ESP-IDF v4.1 之前使用的网络接口抽象组件。本文档概述了从 tcpip_adapter API 迁移至 :doc:`/api-reference/network/esp_netif` 的过程。


更新网络连接代码
--------------------------------


网络软件栈初始化
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- 您只需用 ``esp_netif_init()`` 替换 ``tcpip_adapter_init()``，注意 ``esp_netif_init()`` 函数现将返回标准错误代码。了解详细信息，请参考 :doc:`/api-reference/network/esp_netif`。
- ``esp_netif_deinit()`` 函数用于反初始化网络软件栈。
- 您还需用 ``#include "esp_netif.h"`` 替换 ``#include "tcpip_adapter.h"``。


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

其他 tcpip_adapter API 更换
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

所有 tcpip_adapter 函数都有对应的 esp-netif。请参考以下章节中的 esp_netif.h 部分，了解更多信息：

* :component_file:`Setters/Getters <esp_netif/include/esp_netif.h#L241>`
* :component_file:`DHCP <esp_netif/include/esp_netif.h#L387>`
* :component_file:`DNS <esp_netif/include/esp_netif.h#L516>`
* :component_file:`IP address <esp_netif/include/esp_netif.h#L568>`

默认事件处理程序
^^^^^^^^^^^^^^^^^^^^^^

事件处理程序已从 tcpip_adapter 移至相应驱动程序代码。从应用程序的角度来看，这一变更不会产生任何影响，所有事件仍将以相同的方式处理。请注意，在与 IP 相关的事件处理程序中，应用程序代码通常以 esp-netif 结构体而非 LwIP 结构体的形式接收 IP 地址。两种结构体均兼容二进制格式。


打印地址的首选方式如下所示：

.. code-block:: c

    ESP_LOGI(TAG, "got ip:" IPSTR "\n", IP2STR(&event->ip_info.ip));

不建议使用下述方式：

.. code-block:: c

    ESP_LOGI(TAG, "got ip:%s\n", ip4addr_ntoa(&event->ip_info.ip));

``ip4addr_ntoa()`` 为 LwIP API，因此 esp-netif 还提供了替代函数 ``esp_ip4addr_ntoa()``，然而总得来说仍推荐使用 ``IP2STR()`` 这一方法。

IP 地址
^^^^^^^^^^^^

推荐使用 esp-netif 定义的 IP 结构。请注意，在启用默认兼容性时，LwIP 结构体仍然可以工作。

* :component_file:`esp-netif IP address definitions <esp_netif/include/esp_netif_ip_addr.h#L96>`

后续步骤
^^^^^^^^^^^^^^

为了令移植应用程序可以使用 :doc:`/api-reference/network/esp_netif`，还需在组件配置中禁用 tcpip_adapter 兼容层。请前往 ``ESP NETIF Adapter`` > ``Enable backward compatible tcpip_adapter interface`` 进行设置，并检查项目是否编译成功。

TCP/IP 适配器涉及大量依赖项，禁用兼容层可能有助于将应用程序与使用特定 TCP/IP 软件栈的 API 分离开来。
