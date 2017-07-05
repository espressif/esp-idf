ETHERNET
========

Application Example
-------------------

Ethernet example: :example:`ethernet/ethernet`.

PHY Interfaces
--------------

The configured PHY model(s) are set in software by configuring the eth_config_t structure for the given PHY.

Headers include a default configuration structure. These default configurations will need some members overriden or re-set before they can be used for a particular PHY hardware configuration. Consult the Ethernet example to see how this is done.

  * :component_file:`ethernet/include/eth_phy/phy.h` (common)
  * :component_file:`ethernet/include/eth_phy/phy_tlk110.h`
  * :component_file:`ethernet/include/eth_phy/phy_lan8720.h`

PHY Configuration Constants
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenvariable:: phy_tlk110_default_ethernet_config
.. doxygenvariable:: phy_lan8720_default_ethernet_config


API Reference - Ethernet
------------------------

.. include:: /_build/inc/esp_eth.inc

API Reference - PHY Common
--------------------------

.. include:: /_build/inc/phy.inc

API Reference - PHY TLK110
--------------------------

.. include:: /_build/inc/phy_tlk110.inc

API Reference - PHY LAN8720
---------------------------

.. include:: /_build/inc/phy_lan8720.inc


