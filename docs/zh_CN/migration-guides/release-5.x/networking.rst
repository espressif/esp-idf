Networking
===========

:link_to_translation:`en:[English]`

Ethernet
**********

esp_eth_ioctl() API
-------------------
:cpp:func:`esp_eth_ioctl` third argument could take `int` (`bool`) number as an input in some cases. However, it was not properly documented and, in addition, the number had to be "unnaturally" type casted to `void *` datatype to prevent compiler warnings as shown in below example:

.. highlight:: c

::

    esp_eth_ioctl(eth_handle, ETH_CMD_S_FLOW_CTRL, (void *)true);


This could lead to misuse of the :cpp:func:`esp_eth_ioctl`. Therefore, ESP-IDF 5.0 unified usage of :cpp:func:`esp_eth_ioctl`. Its third argument now always acts as pointer to a memory location of specific type from/to where the configuration option is read/stored.

Usage example to set Ethernet configuration:

.. highlight:: c

::

    eth_duplex_t new_duplex_mode = ETH_DUPLEX_HALF;
    esp_eth_ioctl(eth_handle, ETH_CMD_S_DUPLEX_MODE, &new_duplex_mode);

Usage example to get Ethernet configuration:

.. highlight:: c

::

    eth_duplex_t duplex_mode;
    esp_eth_ioctl(eth_handle, ETH_CMD_G_DUPLEX_MODE, &duplex_mode);


KSZ8041/81 and LAN8720 Driver Update
------------------------------------
KSZ8041/81 and LAN8720 Drivers were updated to support more devices (generations) from associated product family. The drivers are able to recognize particular chip number and its potential support by the driver.

As a result, the specific "chip number" functions calls were replaced by generic ones as follows:

* `esp_eth_phy_new_ksz8041` and `esp_eth_phy_new_ksz8081` were removed, use :cpp:func:`esp_eth_phy_new_ksz80xx` instead
* `esp_eth_phy_new_lan8720` was removed, use :cpp:func:`esp_eth_phy_new_lan87xx` instead


ESP NETIF Glue Event Handlers
-----------------------------
``esp_eth_set_default_handlers()`` and ``esp_eth_clear_default_handlers()`` functions were removed. Registration of the default IP layer handlers for Ethernet is now handled automatically. If users have already followed the recommendation to fully initialize the Ethernet driver and network interface prior to registering their Ethernet/IP event handlers, then no action is required (except for deleting the affected functions). Otherwise, users should ensure that they register the user event handlers as the last thing prior to starting the Ethernet driver.

PHY Address Auto-detect
-----------------------
Ethernet PHY address auto-detect function ``esp_eth_detect_phy_addr`` was renamed to :cpp:func:`esp_eth_phy_802_3_detect_phy_addr` and its header declaration was moved to :component_file:`esp_eth/include/esp_eth_phy_802_3.h`.

.. _tcpip-adapter:

TCP/IP 适配器
*************

TCP/IP 适配器是在 ESP-IDF v4.1 之前使用的网络接口抽象组件。本文档概述了从 tcpip_adapter 移出至其后继者 :doc:`/api-reference/network/esp_netif` 的过程。


更新网络连接代码
--------------------------------


网络软件栈初始化
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

只需将 ``tcpip_adapter_init()`` 替换为 ``esp_netif_init()``。请注意，:doc:`/api-reference/network/esp_netif` 初始化 API 可返回标准错误代码，还可以使用 ``esp_netif_deinit()`` 进行去初始化。

此外，还需将 ``#include "tcpip_adapter.h"`` 替换为 ``#include "esp_netif.h"``。


创建网络接口
^^^^^^^^^^^^^^^^^^^^^^^^^^

TCP/IP 适配器静态定义了三个接口：

- Wi-Fi Station
- Wi-Fi AP
- 以太网

网络接口的设计应严格参考 :doc:`/api-reference/network/esp_netif`，以使其能够连接到 TCP/IP 软件栈。
例如，在 TCP/IP 软件栈和事件循环初始化完成后，Wi-Fi 的初始化代码必须显示调用 ``esp_netif_create_default_wifi_sta();`` 或 ``esp_netif_create_default_wifi_ap();``。
请参阅这三个接口的初始化代码示例：

- Wi-Fi Station: :example_file:`wifi/getting_started/station/main/station_example_main.c`
- Wi-Fi AP: :example_file:`wifi/getting_started/softAP/main/softap_example_main.c`
- 以太网: :example_file:`ethernet/basic/main/ethernet_example_main.c`


更换 tcpip_adapter API
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

所有 tcpip_adapter 函数都有对应的 esp-netif。请参阅 esp_netif.h 查看更多信息：

* :component_file:`Setters/Getters <esp_netif/include/esp_netif.h#L241>`
* :component_file:`DHCP <esp_netif/include/esp_netif.h#L387>`
* :component_file:`DNS <esp_netif/include/esp_netif.h#L516>`
* :component_file:`IP address <esp_netif/include/esp_netif.h#L568>`


默认事件处理程序
^^^^^^^^^^^^^^^^^^^^^^

事件处理程序已经从 tcpip_adapter 移动到相应的驱动程序代码。从应用程序的角度来看，这不会带来任何影响，所有事件仍以相同的方式处理。
请注意，在与 IP 相关的事件处理程序中，应用程序代码通常以 esp-netif 结构体的形式接收 IP 地址，该结构体并非 LwIP 结构，但兼容二进制格式。
打印地址的首选方式如下所示：

.. code-block:: c

    ESP_LOGI(TAG, "got ip:" IPSTR "\n", IP2STR(&event->ip_info.ip));

不建议使用下述方式：

.. code-block:: c

    ESP_LOGI(TAG, "got ip:%s\n", ip4addr_ntoa(&event->ip_info.ip));

由于 ``ip4addr_ntoa()`` 为 LwIP API，因此 esp-netif 还提供了替代函数 ``esp_ip4addr_ntoa()``，但整体而言，仍推荐使用第一种方法。


IP 地址
^^^^^^^^^^^^

推荐使用 esp-netif 定义的 IP 结构。请注意，在启用默认兼容性时，LwIP 结构体仍然可以工作。
* :component_file:`esp-netif IP address definitions <esp_netif/include/esp_netif_ip_addr.h#L96>`


下一步
^^^^^^^^^^

为了令移植应用程序可以使用 :doc:`/api-reference/network/esp_netif`，还需在组件配置中禁用 tcpip_adapter 兼容层。
方法为：``ESP NETIF Adapter`` -> ``Enable backward compatible tcpip_adapter interface``，并检查工程是否编译成功。
TCP/IP 适配器涉及大量依赖项，这一步可能有助于将应用程序与使用特定 TCP/IP 软件栈的 API 分离开来。
