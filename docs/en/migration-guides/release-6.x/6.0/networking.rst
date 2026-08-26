Networking
===========

:link_to_translation:`zh_CN:[中文]`

Ethernet
********

``esp_eth_phy_802_3_reset_hw()`` API Changes
--------------------------------------------

This change only applies if you maintain your own Ethernet PHY driver based on :component_file:`esp_eth/src/phy/esp_eth_phy_802_3.c` common functions. The :cpp:func:`esp_eth_phy_802_3_reset_hw` API accepts only one parameter now and resets the Ethernet PHY with internal timing configuration which is defined during initialization. Previously, the API required a ``reset_assert_us`` parameter to specify the reset pin assertion time. This parameter has been removed.

Usage example:

.. code-block:: c

    esp_eth_phy_802_3_reset_hw(phy_802_3);


Removed RMII Clock Kconfig Options
----------------------------------

Removed the following RMII clock Kconfig options from `components/esp_eth`. Clock configuration is now handled exclusively through EMAC config structures.

**Removed Options**:
- `ETH_PHY_INTERFACE_RMII`, `ETH_RMII_CLK_INPUT`, `ETH_RMII_CLK_OUTPUT`
- `ETH_RMII_CLK_IN_GPIO`, `ETH_RMII_CLK_OUTPUT_GPIO0`, `ETH_RMII_CLK_OUT_GPIO`

**Migration**:

.. code-block:: c

    // Before: Configuration via Kconfig
    // CONFIG_ETH_RMII_CLK_INPUT=y

    // After: Explicit configuration in code
    eth_esp32_emac_config_t emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();
    emac_config.clock_config.rmii.clock_mode = EMAC_CLK_OUT;  // or EMAC_CLK_EXT_IN
    emac_config.clock_config.rmii.clock_gpio = 0;  // GPIO0 for ESP32


**Impact**: Applications using ``ETH_ESP32_EMAC_DEFAULT_CONFIG()`` continue to work. Custom clock configurations must be set explicitly in the EMAC config structure or use the `Ethernet Init component <https://components.espressif.com/components/espressif/ethernet_init>`_.


Ethernet PHY and Ethernet SPI Module drivers are moved from ESP-IDF to external repo
------------------------------------------------------------------------------------

The Ethernet PHY and Ethernet SPI Module drivers have been removed from ESP-IDF and have been migrated to `esp-eth-drivers <https://github.com/espressif/esp-eth-drivers>`_ repository. If you are using these drivers, you need to use the drivers as component in your project. The drivers are now available in the `ESP Component Registry <https://components.espressif.com/>`_.

**Removed APIs**:
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


**Impact**: Applications using Ethernet PHY and Ethernet SPI Module drivers that used to be part of ESP-IDF will no longer work.

**Migration**:

Add driver component from `IDF Component Manager <https://components.espressif.com/>`_ to your project using `idf.py add-dependency` and include `esp_eth_phy_xxxx.h` and `esp_eth_mac_xxxx.h` or use the `Ethernet Init component <https://components.espressif.com/components/espressif/ethernet_init>`_.


PTP API Changes
---------------

Ethernet `esp_eth_ioctl` PTP configuration and control commands have been removed.

**Removed ioctl Commands**:
- :cpp:enumerator:`eth_mac_esp_io_cmd_t::ETH_MAC_ESP_CMD_PTP_ENABLE`
- :cpp:enumerator:`eth_mac_esp_io_cmd_t::ETH_MAC_ESP_CMD_S_PTP_TIME`
- :cpp:enumerator:`eth_mac_esp_io_cmd_t::ETH_MAC_ESP_CMD_G_PTP_TIME`
- :cpp:enumerator:`eth_mac_esp_io_cmd_t::ETH_MAC_ESP_CMD_ADJ_PTP_FREQ`
- :cpp:enumerator:`eth_mac_esp_io_cmd_t::ETH_MAC_ESP_CMD_ADJ_PTP_TIME`
- :cpp:enumerator:`eth_mac_esp_io_cmd_t::ETH_MAC_ESP_CMD_S_TARGET_TIME`
- :cpp:enumerator:`eth_mac_esp_io_cmd_t::ETH_MAC_ESP_CMD_S_TARGET_CB`

**Impact**: Applications using the removed ioctl commands will no longer work.

**Migration**:

Use the new PTP API instead.


ESP-NETIF
*********

Removed Deprecated :cpp:func:`esp_netif_next`
------------------------------------------------

The deprecated iteration helper :cpp:func:`esp_netif_next` has been removed from :doc:`/api-reference/network/esp_netif`. This API was inherently unsafe because it did not lock the interface list or the TCP/IP context during iteration.

Use one of the following alternatives:

- Directly call :cpp:func:`esp_netif_next_unsafe` only in contexts you fully control, or inside :cpp:func:`esp_netif_tcpip_exec` for safe execution within the TCP/IP context.
- Use :cpp:func:`esp_netif_find_if` with a predicate to search for specific interfaces without manual iteration.

Migration
~~~~~~~~~

Before:

.. code-block:: c

    esp_netif_t *it = NULL;
    while ((it = esp_netif_next(it)) != NULL) {
        // use "it"
    }

After (iterate unsafely in a controlled context):

.. code-block:: c

    esp_netif_t *it = NULL;
    while ((it = esp_netif_next_unsafe(it)) != NULL) {
        // use "it"
    }

Recommended (iterate within TCP/IP context):

.. code-block:: c

    static esp_err_t iterate_netifs(void *ctx)
    {
        esp_netif_t *it = NULL;
        while ((it = esp_netif_next_unsafe(it)) != NULL) {
            // use "it"
        }
        return ESP_OK;
    }

    // Execute iteration safely in TCP/IP context
    ESP_ERROR_CHECK(esp_netif_tcpip_exec(iterate_netifs, NULL));

Alternative (find with predicate):

.. code-block:: c

    static bool match_by_key(void *ctx, esp_netif_t *netif)
    {
        const char *wanted = (const char *)ctx;
        const char *key = esp_netif_get_ifkey(netif);
        return key && strcmp(key, wanted) == 0;
    }

    esp_netif_t *target = esp_netif_find_if(match_by_key, (void *)"WIFI_STA_DEF");
    if (target) {
        // use "target"
    }


DHCP Server DNS Option Behavior
-------------------------------

The ``LWIP_DHCPS_ADD_DNS`` macro has been removed.

Previously, when running a DHCP server on SoftAP, if no DNS offer option was set, the server IP address was automatically advertised as the DNS server.

**Current behavior:**

From this release onward, the DHCP server includes DNS information in its offers only when explicitly configured using :cpp:func:`esp_netif_dhcps_option` with the ``ESP_NETIF_DOMAIN_NAME_SERVER`` option. In that case, the currently configured main and/or backup DNS addresses for the SoftAP interface are sent to clients.

If the option is not enabled, the DHCP server's own IP address is sent as the DNS server, which preserves the previous default behavior.

**Migration:**

If applications rely on custom DNS settings, developers should:

1. Enable the DHCP server to include DNS information in its offers using :cpp:func:`esp_netif_dhcps_option` with the ``ESP_NETIF_DOMAIN_NAME_SERVER`` option.
2. Configure one or more DNS server addresses for the SoftAP interface using :cpp:func:`esp_netif_set_dns_info`.
3. If no DNS information should be sent at all, configure :cpp:func:`esp_netif_dhcps_option` but set the DNS server address to ``0.0.0.0`` using :cpp:func:`esp_netif_set_dns_info`.

This allows developers to:

- replicate the old behavior (advertising the SoftAP IP),
- provide custom DNS servers (for example, public resolvers), or
- suppress DNS information entirely by setting the DNS server to ``0.0.0.0``.


LWIP
****

The lwIP TCP/IP thread name has changed from "tiT" to "tcpip".


SNTP Header File Removal
------------------------

The deprecated ``sntp.h`` header file has been removed in IDF v6.0. Applications should now include ``esp_sntp.h`` instead of ``sntp.h`` for SNTP functionality.

Ping API Removal
----------------

The deprecated ping APIs and headers have been removed in IDF v6.0:

- ``esp_ping.h`` header file has been removed
- ``ping.h`` header file has been removed
- Functions ``ping_init()``, ``ping_deinit()``, ``esp_ping_set_target()``, ``esp_ping_get_target()``, and ``esp_ping_result()`` have been removed

Applications should now use the socket-based ping API from ``ping/ping_sock.h``:

.. code-block:: c

    #include "ping/ping_sock.h"
    // Create ping session
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

For a complete example, see :example:`protocols/icmp_echo`.
