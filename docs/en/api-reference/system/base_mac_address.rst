Base MAC address
================

Overview
--------

Serveral MAC addresses (universally administered by IEEE) are uniquely assigned to the networking interfaces (WiFi/BT/Ethernet). 
The final octet of each universally administered MAC address increases by one. Only the first one which is called base MAC address 
of them is stored in EFUSE or external storage, the others are generated from it. Here, 'generate' means adding 0, 1, 2 and 3 
(respectively) to the final octet of the base MAC address. 

If the universally administered MAC addresses are not enough for all of the networking interfaces. Local administered MAC addresses
which are derived from universally administered MAC addresses are assigned to the reset of networking interfaces. 

A `definition of local vs universal MAC address can be found on Wikipedia <https://en.wikipedia.org/wiki/MAC_address#Universal_vs._local>`_.

The number of universally administered MAC address can be configured using ``make menuconfig``.

Base MAC address
^^^^^^^^^^^^^^^^

If using the default base MAC address factory programmed by Espressif in BLK0 of EFUSE, nothing needs to be done.

If using a custom base MAC address stored in BLK3 of EFUSE, call API ``esp_efuse_mac_get_custom()`` to get the base MAC address
which is stored in BLK3 of EFUSE. If correct MAC address is returned, then call ``esp_base_mac_addr_set()`` to set the base MAC 
address for system to generate the MAC addresses used by the networking interfaces(WiFi/BT/Ethernet).
There are 192 bits storage spaces for custom to store base MAC address in BLK3 of EFUSE. They are EFUSE_BLK3_RDATA0, 
EFUSE_BLK3_RDATA1, EFUSE_BLK3_RDATA2, EFUSE_BLK3_RDATA3, EFUSE_BLK3_RDATA4 and EFUSE_BLK3_RDATA5, each of them is 32 bits 
register. The format of the 192 bits storage spaces is:

.. highlight:: none

::

    ------------------------------------------------------
    Field       |Bits |Range     |Description
    ------------------------------------------------------
    version     |8    |[191:184] |1: useful. 0: useless
    ------------------------------------------------------
    reserve     |112  |[183:72]  |reserved
    ------------------------------------------------------
    mac address |64   |[71:8]    |base MAC address
    ------------------------------------------------------
    mac crc     |8    |[7:0]     |crc of base MAC address
    ------------------------------------------------------

If using base MAC address stored in external storage, firstly get the base MAC address stored in external storage, then call 
API ``esp_base_mac_addr_set()`` to set the base MAC address for system to generate the MAC addresses used by the networking 
interfaces(WiFi/BT/Ethernet).

All of the steps must be done before initializing the networking interfaces(WiFi/BT/Ethernet). It is recommended to do it in 
``app_main()`` which can be referenced in :example:`system/base_mac_address`.

Number of universally administered MAC address
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If the number of universal MAC addresses is two, only two interfaces (WiFi station and Bluetooth) receive a universally 
administered MAC address. These are generated sequentially by adding 0 and 1 (respectively) to the base MAC address. 
The remaining two interfaces (WiFi softap and Ethernet) receive local MAC addresses. These are derived from the universal 
WiFi station and Bluetooth MAC addresses, respectively.

If the number of universal MAC addresses is four, all four interfaces (WiFi station, WiFi softap, Bluetooth and Ethernet) 
receive a universally administered MAC address. These are generated sequentially by adding 0, 1, 2 and 3 (respectively) 
to the final octet of the base MAC address.

When using the default (Espressif-assigned) base MAC address, either setting can be used. When using a custom universal MAC 
address range, the correct setting will depend on the allocation of MAC addresses in this range (either 2 or 4 per device.)
        
API Reference
-------------

Header Files
^^^^^^^^^^^^

  * :component_file:`esp32/include/esp_system.h`


Functions
---------

.. doxygenfunction:: esp_base_mac_addr_set
.. doxygenfunction:: esp_efuse_mac_get_custom
