网络
=====

:link_to_translation:`en:[English]`

以太网
******

``esp_eth_phy_802_3_reset_hw()`` API 变更
------------------------------------------

此变更仅适用于维护自己的以太网 PHY 驱动程序，该驱动程序基于 :component_file:`esp_eth/src/phy/esp_eth_phy_802_3.c` 通用函数。现在 :cpp:func:`esp_eth_phy_802_3_reset_hw` API 仅接受一个参数，并使用初始化期间定义的内部时序配置重置以太网 PHY。以前，该 API 需要一个 ``reset_assert_us`` 参数来指定复位引脚断言时间。此参数已删除。

使用示例：

.. code-block:: c

    esp_eth_phy_802_3_reset_hw(phy_802_3);


移除 RMII 时钟 Kconfig 选项
---------------------------

已从 `components/esp_eth` 中移除了以下 RMII 时钟相关的 Kconfig 选项。时钟配置现在仅通过 EMAC 配置结构体进行管理。

**移除的选项**：
- `ETH_PHY_INTERFACE_RMII`、`ETH_RMII_CLK_INPUT`、`ETH_RMII_CLK_OUTPUT`
- `ETH_RMII_CLK_IN_GPIO`、`ETH_RMII_CLK_OUTPUT_GPIO0`、`ETH_RMII_CLK_OUT_GPIO`

**迁移方式**：

.. code-block:: c

    // 之前：通过 Kconfig 配置
    // CONFIG_ETH_RMII_CLK_INPUT=y

    // 之后：在代码中显式配置
    eth_esp32_emac_config_t emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();
    emac_config.clock_config.rmii.clock_mode = EMAC_CLK_OUT;  // 或 EMAC_CLK_EXT_IN
    emac_config.clock_config.rmii.clock_gpio = 0;  // ESP32 使用 GPIO0


**影响**：使用 ``ETH_ESP32_EMAC_DEFAULT_CONFIG()`` 的应用程序可继续正常工作。自定义时钟配置需在 EMAC 配置结构体中显式设置，或使用 `Ethernet Init 组件 <https://components.espressif.com/components/espressif/ethernet_init>`_。


以太网 PHY 和以太网 SPI 模块驱动已从 ESP-IDF 移至外部仓库
-------------------------------------------------------------

以太网 PHY 和以太网 SPI 模块驱动已从 ESP-IDF 中移除，并迁移至 `esp-eth-drivers <https://github.com/espressif/esp-eth-drivers>`_ 仓库。如果你在项目中使用这些驱动，需要将其作为组件集成。相关驱动现已在 `ESP 组件库 <https://components.espressif.com/>`_ 上提供。

**移除的 API**：
- :cpp:func:`esp_eth_phy_new_ip101`
- :cpp:func:`esp_eth_phy_new_lan87xx`
- :cpp:func:`esp_eth_phy_new_rtl8201`
- :cpp:func:`esp_eth_phy_new_dp83848`
- :cpp:func:`esp_eth_phy_new_ksz80xx`
- :cpp:func:`esp_eth_mac_new_dm9051`
- :cpp:func:`esp_eth_phy_new_dm9051`
- :cpp:func:`esp_eth_mac_new_ksz8851snl`
- :cpp:func:`esp_eth_phy_new_ksz8851snl`
- :cpp:func:`esp_eth_mac_new_w5500`
- :cpp:func:`esp_eth_phy_new_w5500`


**影响**：原本依赖 ESP-IDF 内置以太网 PHY 和以太网 SPI 模块驱动的应用将无法继续工作。

**迁移方式**：

请通过 `idf.py add-dependency` 命令，从 `IDF 组件管理器 <https://components.espressif.com/>`_ 添加驱动组件到你的项目，并包含相应的 `esp_eth_phy_xxxx.h` 和 `esp_eth_mac_xxxx.h` 头文件，或使用 `Ethernet Init 组件 <https://components.espressif.com/components/espressif/ethernet_init>`_。


PTP API 变更
---------------

以太网 `esp_eth_ioctl` PTP 配置和控制命令已被移除。

**移除的 ioctl 命令**：
- :cpp:enumerator:`eth_mac_esp_io_cmd_t::ETH_MAC_ESP_CMD_PTP_ENABLE`
- :cpp:enumerator:`eth_mac_esp_io_cmd_t::ETH_MAC_ESP_CMD_S_PTP_TIME`
- :cpp:enumerator:`eth_mac_esp_io_cmd_t::ETH_MAC_ESP_CMD_G_PTP_TIME`
- :cpp:enumerator:`eth_mac_esp_io_cmd_t::ETH_MAC_ESP_CMD_ADJ_PTP_FREQ`
- :cpp:enumerator:`eth_mac_esp_io_cmd_t::ETH_MAC_ESP_CMD_ADJ_PTP_TIME`
- :cpp:enumerator:`eth_mac_esp_io_cmd_t::ETH_MAC_ESP_CMD_S_TARGET_TIME`
- :cpp:enumerator:`eth_mac_esp_io_cmd_t::ETH_MAC_ESP_CMD_S_TARGET_CB`

**影响**：使用已移除 ioctl 命令的应用程序将不再工作。

**迁移**：

请改用新的 PTP API。


ESP-NETIF
*********

移除弃用的 :cpp:func:`esp_netif_next`
-------------------------------------

已从 :doc:`/api-reference/network/esp_netif` 中移除弃用的迭代辅助函数 :cpp:func:`esp_netif_next`。该 API 本质上不安全，因为在迭代过程中它既不锁定接口列表，也不锁定 TCP/IP 上下文。

请使用以下替代方案之一：

- 仅在完全控制的上下文中直接调用 :cpp:func:`esp_netif_next_unsafe`，或在 :cpp:func:`esp_netif_tcpip_exec` 函数内部调用该函数，以便在 TCP/IP 上下文中安全运行。
- 使用 :cpp:func:`esp_netif_find_if` 并配合谓词函数查找特定接口，无需手动迭代。

迁移方式
~~~~~~~~~

之前用法：

.. code-block:: c

    esp_netif_t *it = NULL;
    while ((it = esp_netif_next(it)) != NULL) {
        // 使用 "it"
    }

之后用法（在受控上下文中非安全迭代）：

.. code-block:: c

    esp_netif_t *it = NULL;
    while ((it = esp_netif_next_unsafe(it)) != NULL) {
        // 使用 "it"
    }

推荐用法（在 TCP/IP 上下文中迭代）：

.. code-block:: c

    static esp_err_t iterate_netifs(void *ctx)
    {
        esp_netif_t *it = NULL;
        while ((it = esp_netif_next_unsafe(it)) != NULL) {
            // 使用 "it"
        }
        return ESP_OK;
    }

    // 在 TCP/IP 上下文中安全执行迭代
    ESP_ERROR_CHECK(esp_netif_tcpip_exec(iterate_netifs, NULL));

替代方案（使用谓词查找）：

.. code-block:: c

    static bool match_by_key(void *ctx, esp_netif_t *netif)
    {
        const char *wanted = (const char *)ctx;
        const char *key = esp_netif_get_ifkey(netif);
        return key && strcmp(key, wanted) == 0;
    }

    esp_netif_t *target = esp_netif_find_if(match_by_key, (void *)"WIFI_STA_DEF");
    if (target) {
        // 使用 "target"
    }


DHCP 服务器 DNS 选项行为
-------------------------

``LWIP_DHCPS_ADD_DNS`` 宏已被移除。

在此之前，在 SoftAP 上运行 DHCP 服务器时，如果没有设置 DNS 选项，则服务器的 IP 地址会被自动公布为 DNS 服务器。

**当前行为：**

从本版本开始，DHCP 服务器只有在显式配置了 :cpp:func:`esp_netif_dhcps_option` 并启用了 ``ESP_NETIF_DOMAIN_NAME_SERVER`` 选项时，才会在 DHCP offer 报文中包含 DNS 信息。此时，SoftAP 接口当前配置的主 DNS 和/或备用 DNS 地址将被发送给客户端。

如果没有启用该选项，DHCP 服务器会将自己的 IP 地址作为 DNS 服务器发送给客户端，从而与之前的默认行为保持一致。

**迁移说明：**

如果应用程序依赖自定义 DNS 设置，开发者应：

1. 使用 :cpp:func:`esp_netif_dhcps_option` 并启用 ``ESP_NETIF_DOMAIN_NAME_SERVER`` 选项，让 DHCP 服务器在 offer 报文中包含 DNS 信息。
2. 使用 :cpp:func:`esp_netif_set_dns_info` 为 SoftAP 接口配置一个或多个 DNS 服务器地址。
3. 若需完全禁止发送 DNS 信息，仍需配置 :cpp:func:`esp_netif_dhcps_option`，但应通过 :cpp:func:`esp_netif_set_dns_info` 将 DNS 服务器地址设置为 ``0.0.0.0``。

这样开发者可以：

- 复现旧的行为（通告 SoftAP IP），
- 提供自定义的 DNS 服务器（例如公共解析器），
- 通过将 DNS 地址设置为 ``0.0.0.0`` 来完全禁止 DNS 信息通告。


LWIP
****

lwIP TCP/IP 线程名称由 "tiT" 更改为 "tcpip"。


SNTP 头文件移除
---------------

已弃用的 ``sntp.h`` 头文件在 IDF v6.0 中已被移除。应用程序现在应该包含 ``esp_sntp.h`` 而不是 ``sntp.h`` 来使用 SNTP 功能。

Ping API 移除
-------------

已弃用的 ping API 和头文件在 IDF v6.0 中已被移除：

- ``esp_ping.h`` 头文件已被移除
- ``ping.h`` 头文件已被移除
- 函数 ``ping_init()``、``ping_deinit()``、``esp_ping_set_target()``、``esp_ping_get_target()`` 和 ``esp_ping_result()`` 已被移除

应用程序现在应该使用来自 ``ping/ping_sock.h`` 的基于套接字的 ping API：

.. code-block:: c

    #include "ping/ping_sock.h"
    // 创建 ping 会话
    esp_ping_config_t config = ESP_PING_DEFAULT_CONFIG();
    config.target_addr = target_ip;
    esp_ping_callbacks_t cbs = {
        .on_ping_success = on_ping_success,
        .on_ping_timeout = on_ping_timeout,
        .on_ping_end = on_ping_end,
    };
    esp_ping_handle_t ping;
    esp_ping_new_session(&config, &cbs, &ping);
    esp_ping_start(ping);

完整示例请参考 :example:`protocols/icmp_echo`。
