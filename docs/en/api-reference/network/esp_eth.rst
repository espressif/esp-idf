Ethernet
========

:link_to_translation:`zh_CN:[中文]`

Application Example
-------------------

-   Ethernet basic example: :example:`ethernet/basic`.
-   Ethernet iperf example: :example:`ethernet/iperf`.
-   Ethernet to Wi-Fi AP "router": :example:`ethernet/eth2ap`.

Ethernet Driver Model
---------------------

  * :component_file:`esp_eth/include/esp_eth.h`

Ethernet Common Interface
-------------------------

  * :component_file:`esp_eth/include/esp_eth_com.h`

Ethernet MAC Interface
----------------------

  * :component_file:`esp_eth/include/esp_eth_mac.h`

Ethernet PHY Interface
----------------------

  * :component_file:`esp_eth/include/esp_eth_phy.h`

Ethernet PHY Common Registers
-----------------------------

  * :component_file:`esp_eth/include/eth_phy_regs_struct.h`

API Reference - Driver Model
----------------------------

.. include-build-file:: inc/esp_eth.inc

API Reference - Common Interface
--------------------------------

.. include-build-file:: inc/esp_eth_com.inc

API Reference - MAC Interface
-----------------------------

.. include-build-file:: inc/esp_eth_mac.inc

API Reference - PHY Interface
-----------------------------

.. include-build-file:: inc/esp_eth_phy.inc

API Reference - Glue for esp_netif
----------------------------------

.. include-build-file:: inc/esp_eth_netif_glue.inc
