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
