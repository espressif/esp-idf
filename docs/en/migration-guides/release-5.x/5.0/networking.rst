Networking
===========

:link_to_translation:`zh_CN:[中文]`

Wi-Fi
*****

Callback Function Type ``esp_now_recv_cb_t``
--------------------------------------------

Previously, the first parameter of ``esp_now_recv_cb_t`` was of type ``const uint8_t *mac_addr``, which only included the address of ESP-NOW peer device.

This now changes. The first parameter is of type ``esp_now_recv_info_t``, which has members ``src_addr``, ``des_addr`` and ``rx_ctrl``. Therefore, the following updates are required:

    - Redefine ESP-NOW receive callback function.

    - ``src_addr`` can be used to replace original ``mac_addr``.

    - ``des_addr`` is the destination MAC address of ESP-NOW packet, which can be unitcast or broadcast address. With ``des_addr``, the user can distinguish unitcast and broadcast ESP-NOW packets where broadcast ESP-NOW packets can be non-encrypted even when encryption policy is configured for the ESP-NOW.

    - ``rx_ctrl`` is Rx control info of the ESP-NOW packet, which provides more information about the packet.

Please refer to the ESP-NOW example: :example_file:`wifi/espnow/main/espnow_example_main.c`

Ethernet
********

``esp_eth_ioctl()`` API
-----------------------

Previously, the :cpp:func:`esp_eth_ioctl` API had the following issues:

    - The third parameter (which is of type ``void *``) would accept an ``int``/``bool`` type arguments (i.e., not pointers) as input in some cases. However, these cases were not documented properly.
    - To pass ``int``/``bool`` type argument as the third parameter, the argument had to be "unnaturally" casted to a ``void *`` type, to prevent a compiler warning as demonstrated in the code snippet below. This casting could lead to misuse of the :cpp:func:`esp_eth_ioctl` function.

.. code-block:: c

    esp_eth_ioctl(eth_handle, ETH_CMD_S_FLOW_CTRL, (void *)true);

Therefore, the usage of :cpp:func:`esp_eth_ioctl` is now unified. Arguments to the third parameter must be passed as pointers to a specific data type to/from where data is stored/read by :cpp:func:`esp_eth_ioctl`. The code snippets below demonstrate the usage of :cpp:func:`esp_eth_ioctl`.

Usage example to set Ethernet configuration:

.. code-block:: c

    eth_duplex_t new_duplex_mode = ETH_DUPLEX_HALF;
    esp_eth_ioctl(eth_handle, ETH_CMD_S_DUPLEX_MODE, &new_duplex_mode);

Usage example to get Ethernet configuration:

.. code-block:: c

    eth_duplex_t duplex_mode;
    esp_eth_ioctl(eth_handle, ETH_CMD_G_DUPLEX_MODE, &duplex_mode);

KSZ8041/81 and LAN8720 Driver Update
------------------------------------

The KSZ8041/81 and LAN8720 drivers are updated to support more devices (i.e., generations) from their associated product families. The drivers can recognize particular chip numbers and their potential support by the driver.

As a result, the specific "chip number" functions calls are replaced by generic ones as follows:

* Removed ``esp_eth_phy_new_ksz8041()`` and ``esp_eth_phy_new_ksz8081()``, and use :cpp:func:`esp_eth_phy_new_ksz80xx` instead
* Removed ``esp_eth_phy_new_lan8720()``, and use :cpp:func:`esp_eth_phy_new_lan87xx` instead


ESP NETIF Glue Event Handlers
-----------------------------

``esp_eth_set_default_handlers()`` and ``esp_eth_clear_default_handlers()`` functions are removed. Registration of the default IP layer handlers for Ethernet is now handled automatically. If you have already followed the suggestion to fully initialize the Ethernet driver and network interface before registering their Ethernet/IP event handlers, then no action is required (except for deleting the affected functions). Otherwise, you may start the Ethernet driver right after they register the user event handler.

PHY Address Auto-detect
-----------------------

The Ethernet PHY address auto-detect function ``esp_eth_detect_phy_addr()`` is renamed to :cpp:func:`esp_eth_phy_802_3_detect_phy_addr` and its header declaration is moved to :component_file:`esp_eth/include/esp_eth_phy_802_3.h`.


SPI-Ethernet Module Initialization
-----------------------------------

The SPI-Ethernet Module initialization is now simplified. Previously, you had to manually allocate an SPI device using :cpp:func:`spi_bus_add_device` before instantiating the SPI-Ethernet MAC.

Now, you no longer need to call :cpp:func:`spi_bus_add_device` as SPI devices are allocated internally. As a result, the :cpp:class:`eth_dm9051_config_t`, :cpp:class:`eth_w5500_config_t`, and :cpp:class:`eth_ksz8851snl_config_t` configuration structures are updated to include members for SPI device configuration (e.g., to allow fine tuning of SPI timing which may be dependent on PCB design). Likewise, the ``ETH_DM9051_DEFAULT_CONFIG``, ``ETH_W5500_DEFAULT_CONFIG``, and ``ETH_KSZ8851SNL_DEFAULT_CONFIG`` configuration initialization macros are updated to accept new input parameters. Refer to :doc:`Ethernet API Reference Guide <../../../api-reference/network/esp_eth>` for an example of SPI-Ethernet Module initialization.

Ethernet Driver
---------------

APIs for creating MAC instances (`esp_eth_mac_new_*()`) have been reworked to accept two parameters, instead of one common configuration. Now, the configuration includes

* Vendor specific MAC configuration
* Ethernet driver MAC configuration

This is applicable to internal Ethernet MAC :cpp:func:`esp_eth_mac_new_esp32()` as well as to external MAC devices, such as :cpp:func:`esp_eth_mac_new_ksz8851snl()`, :cpp:func:`esp_eth_mac_new_dm9051()`, and :cpp:func:`esp_eth_mac_new_w5500()`

.. _tcpip-adapter:

TCP/IP Adapter
*****************

The TCP/IP Adapter was a network interface abstraction component used in ESP-IDF prior to v4.1. This section outlines migration from ``tcpip_adapter`` API to its successor :doc:`/api-reference/network/esp_netif`.


Updating Network Connection Code
--------------------------------


Network Stack Initialization
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- You may simply replace ``tcpip_adapter_init()`` with ``esp_netif_init()``. However, please should note that the ``esp_netif_init()`` function now returns standard error codes. See :doc:`/api-reference/network/esp_netif` for more details.
- The ``esp_netif_deinit()`` function is provided to de-initialize the network stack.
- You should also replace ``#include "tcpip_adapter.h"`` with ``#include "esp_netif.h"``.


Network Interface Creation
^^^^^^^^^^^^^^^^^^^^^^^^^^

Previously, the TCP/IP Adapter defined the following network interfaces statically:

- WiFi Station
- WiFi Access Point
- Ethernet

This now changes. Network interface instance should be explicitly constructed, so that the :doc:`/api-reference/network/esp_netif` can connect to the TCP/IP stack. For example, after the TCP/IP stack and the event loop are initialized, the initialization code for WiFi must explicitly call ``esp_netif_create_default_wifi_sta();`` or ``esp_netif_create_default_wifi_ap();``.

Please refer to the example initialization code for these three interfaces:

- WiFi Station: :example_file:`wifi/getting_started/station/main/station_example_main.c`
- WiFi Access Point: :example_file:`wifi/getting_started/softAP/main/softap_example_main.c`
- Ethernet: :example_file:`ethernet/basic/main/ethernet_example_main.c`

Other ``tcpip_adapter`` API Replacement
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

All the ``tcpip_adapter`` functions have their ``esp-netif`` counter-part. Please refer to the ``esp_netif.h`` grouped into these sections:

*  :component_file:`Setters/Getters <esp_netif/include/esp_netif.h#L241>`
*  :component_file:`DHCP <esp_netif/include/esp_netif.h#L387>`
*  :component_file:`DNS <esp_netif/include/esp_netif.h#L516>`
*  :component_file:`IP address <esp_netif/include/esp_netif.h#L568>`

Default Event Handlers
^^^^^^^^^^^^^^^^^^^^^^

Event handlers are moved from ``tcpip_adapter`` to appropriate driver code. There is no change from application code perspective, as all events should be handled in the same way. Please note that for IP-related event handlers, application code usually receives IP addresses in the form of an ``esp-netif`` specific struct instead of the LwIP structs. However, both structs are binary compatible.


This is the preferred way to print the address:

.. code-block:: c

    ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));

Instead of

.. code-block:: c

    ESP_LOGI(TAG, "got ip:%s", ip4addr_ntoa(&event->ip_info.ip));

Since ``ip4addr_ntoa()`` is a LwIP API, the esp-netif provides ``esp_ip4addr_ntoa()`` as a replacement. However, the above method using ``IP2STR()`` is generally preferred.

IP Addresses
^^^^^^^^^^^^

You are advised to use ``esp-netif`` defined IP structures. Please note that with default compatibility enabled, the LwIP structs still work.

*  :component_file:`esp-netif IP address definitions <esp_netif/include/esp_netif_ip_addr.h#L96>`
