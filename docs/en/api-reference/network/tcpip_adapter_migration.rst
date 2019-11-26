TCP/IP Adapter Migration Guide
==============================

TCP/IP Adapter is a network interface abstraction component used in IDF prior to v4.1. This page outlines migration from tcpip_adapter API
to its successor :doc:`/api-reference/network/esp_netif`.


Updating network connection code
--------------------------------


Network stack initialization
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Simply replace ``tcpip_adapter_init()`` with ``esp_netif_init()``. Please note that the :doc:`/api-reference/network/esp_netif` initialization API returns standard error code and the ``esp_netif_deinit()``
for un-initialization is available.

Also replace ``#include "tcpip_adapter.h"`` with ``#include "esp_netif.h"``.


Network interface creation
^^^^^^^^^^^^^^^^^^^^^^^^^^

TCP/IP Adapter defined these three interfaces statically:

- WiFi Station
- WiFi Access Point
- Ethernet

Network interface instance shall be explicitly constructed for the :doc:`/api-reference/network/esp_netif` to enable its connection to the TCP/IP stack.
For example initialization code for WiFi has to explicitly call ``esp_netif_create_default_wifi_sta();`` or ``esp_netif_create_default_wifi_ap();`` after the TCP/IP stack and the event loop
have been initialized.
Please consult an example initialization code for these three interfaces:

- WiFi Station: :example:`examples/wifi/getting_started/station/main/station_example_main.c`
- WiFi Access Point: :example:`examples/wifi/getting_started/softAP/main/softap_example_main.c`
- Ethernet :example:`examples/ethernet/basic/main/ethernet_example_main.c`


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
Please note that within IP related event handlers, application code usually receives IP addresses in a form of esp-netif specific struct (not the lwIP structs, but binary compatible).
This is the preferred way of printing the address:

.. code-block:: c

           ESP_LOGI(TAG, "got ip:" IPSTR "\n", IP2STR(&event->ip_info.ip));

Instead of

.. code-block:: c

           ESP_LOGI(TAG, "got ip:%s\n", ip4addr_ntoa(&event->ip_info.ip));

Since ``ip4addr_ntoa()`` is a lwIP API, the esp-netif provides ``esp_ip4addr_ntoa()`` as a replacement, but the above method is generally preferred.


IP addresses
^^^^^^^^^^^^

It is preferred to use esp-netif defined IP structures. Please note that the lwIP structs will still work when default compatibility enabled.
*  :component_file:`esp-netif IP address definitions <esp_netif/include/esp_netif_ip_addr.h#L96>`


Next steps
^^^^^^^^^^

Additional step in porting an application to fully benefit from the :doc:`/api-reference/network/esp_netif` is to disable the tcpip_adapter compatibility layer in the component configuration:
``ESP NETIF Adapter`` -> ``Enable backward compatible tcpip_adapter interface`` and check if the project compiles.
TCP/IP adapter brings many include dependencies and this step might help in decoupling the application from using specific TCP/IP stack API directly.
