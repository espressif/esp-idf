Migrate Ethernet Drivers to ESP-IDF 5.0
=======================================

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

