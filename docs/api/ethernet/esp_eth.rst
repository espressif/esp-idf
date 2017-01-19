ETHERNET
========

Application Example
-------------------

Ethernet example: :example:`ethernet/ethernet`.

API Reference
-------------

Header Files
^^^^^^^^^^^^

  * :component_file:`ethernet/include/esp_eth.h`

Macros
^^^^^^


Type Definitions
^^^^^^^^^^^^^^^^

.. doxygentypedef:: eth_phy_check_link_func
.. doxygentypedef:: eth_phy_check_init_func
.. doxygentypedef:: eth_phy_get_speed_mode_func
.. doxygentypedef:: eth_phy_get_duplex_mode_func
.. doxygentypedef:: eth_phy_func
.. doxygentypedef:: eth_tcpip_input_func
.. doxygentypedef:: eth_gpio_config_func
.. doxygentypedef:: eth_phy_get_partner_pause_enable_func

Enumerations
^^^^^^^^^^^^

.. doxygenenum:: eth_mode_t
.. doxygenenum:: eth_speed_mode_t
.. doxygenenum:: eth_duplex_mode_t
.. doxygenenum:: eth_phy_base_t

Structures
^^^^^^^^^^

.. doxygenstruct:: eth_config_t
    :members:


Functions
^^^^^^^^^

.. doxygenfunction:: esp_eth_init
.. doxygenfunction:: esp_eth_tx
.. doxygenfunction:: esp_eth_enable
.. doxygenfunction:: esp_eth_disable
.. doxygenfunction:: esp_eth_get_mac
.. doxygenfunction:: esp_eth_smi_write
.. doxygenfunction:: esp_eth_smi_read
.. doxygenfunction:: esp_eth_free_rx_buf
