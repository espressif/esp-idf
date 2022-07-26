Networking
===========

:link_to_translation:`zh_CN:[中文]`

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

TCP/IP Adapter
*****************

TCP/IP Adapter was a network interface abstraction component used in ESP-IDF prior to v4.1. This page outlines migration from tcpip_adapter API to its successor :doc:`/api-reference/network/esp_netif`.


Updating network connection code
--------------------------------


Network stack initialization
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Simply replace ``tcpip_adapter_init()`` with ``esp_netif_init()``. Please note that the :doc:`/api-reference/network/esp_netif` initialization API returns standard error code and the ``esp_netif_deinit()`` for un-initialization is available.

Also replace ``#include "tcpip_adapter.h"`` with ``#include "esp_netif.h"``.


Network interface creation
^^^^^^^^^^^^^^^^^^^^^^^^^^

TCP/IP Adapter defined these three interfaces statically:

- WiFi Station
- WiFi Access Point
- Ethernet

Network interface instance shall be explicitly constructed for the :doc:`/api-reference/network/esp_netif` to enable its connection to the TCP/IP stack.
For example initialization code for WiFi has to explicitly call ``esp_netif_create_default_wifi_sta();`` or ``esp_netif_create_default_wifi_ap();`` after the TCP/IP stack and the event loop have been initialized.
Please consult an example initialization code for these three interfaces:

- WiFi Station: :example_file:`wifi/getting_started/station/main/station_example_main.c`
- WiFi Access Point: :example_file:`wifi/getting_started/softAP/main/softap_example_main.c`
- Ethernet: :example_file:`ethernet/basic/main/ethernet_example_main.c`


Replacing other tcpip_adapter API
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

All the tcpip_adapter functions have their esp-netif counter-part. Please refer to the esp_netif.h grouped into these sections:

*  :component_file:`Setters/Getters <esp_netif/include/esp_netif.h#L241>`
*  :component_file:`DHCP <esp_netif/include/esp_netif.h#L387>`
*  :component_file:`DNS <esp_netif/include/esp_netif.h#L516>`
*  :component_file:`IP address <esp_netif/include/esp_netif.h#L568>`


Default event handlers
^^^^^^^^^^^^^^^^^^^^^^

Event handlers are moved from tcpip_adapter to appropriate driver code. There is no change from application code perspective, all events shall be handled in the same way.
Please note that within IP related event handlers, application code usually receives IP addresses in a form of esp-netif specific struct (not the LwIP structs, but binary compatible).
This is the preferred way of printing the address:

.. code-block:: c

           ESP_LOGI(TAG, "got ip:" IPSTR "\n", IP2STR(&event->ip_info.ip));

Instead of

.. code-block:: c

           ESP_LOGI(TAG, "got ip:%s\n", ip4addr_ntoa(&event->ip_info.ip));

Since ``ip4addr_ntoa()`` is a LwIP API, the esp-netif provides ``esp_ip4addr_ntoa()`` as a replacement, but the above method is generally preferred.


IP addresses
^^^^^^^^^^^^

It is preferred to use esp-netif defined IP structures. Please note that the LwIP structs will still work when default compatibility enabled.
*  :component_file:`esp-netif IP address definitions <esp_netif/include/esp_netif_ip_addr.h#L96>`


Next steps
^^^^^^^^^^

Additional step in porting an application to fully benefit from the :doc:`/api-reference/network/esp_netif` is to disable the tcpip_adapter compatibility layer in the component configuration:
``ESP NETIF Adapter`` -> ``Enable backward compatible tcpip_adapter interface`` and check if the project compiles.
TCP/IP adapter brings many include dependencies and this step might help in decoupling the application from using specific TCP/IP stack API directly.
