ETHERNET
========

Application Example
-------------------

ethernet example: `examples/17_ethernet <https://github.com/espressif/esp-idf/tree/master/examples/17_ethernet>`_.

API Reference
-------------

Header Files
^^^^^^^^^^^^

  * `components/ethernet/include/esp_eth.h <https://github.com/espressif/esp-idf/blob/master/components/ethernet/include/esp_eth.h>`_

Macros
^^^^^^


Type Definitions
^^^^^^^^^^^^^^^^

.. doxygentypedef:: eth_phy_fun
.. doxygentypedef:: eth_tcpip_input_fun
.. doxygentypedef:: eth_gpio_config_func

Enumerations
^^^^^^^^^^^^

.. doxygenenum:: eth_mode_t
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

