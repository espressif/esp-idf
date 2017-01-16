RMT
========

Overview
--------

The RMT (Remote Control) module driver can be used to send and receive infrared remote control signals. Due to flexibility of RMT module, the driver can also be used to generate many other types of signals.

Application Example
-------------------

NEC remote control TX and RX example: `examples/peripherals/rmt_nec_tx_rx <https://github.com/espressif/esp-idf/tree/master/examples/peripherals/rmt_nec_tx_rx>`_.

API Reference
-------------

Header Files
^^^^^^^^^^^^

  * `driver/rmt.h <https://github.com/espressif/esp-idf/blob/master/components/driver/include/driver/rmt.h>`_

Macros
^^^^^^

.. doxygendefine:: RMT_MEM_BLOCK_BYTE_NUM
.. doxygendefine:: RMT_MEM_ITEM_NUM

Enumerations
^^^^^^^^^^^^

.. doxygenenum:: rmt_channel_t
.. doxygenenum:: rmt_mem_owner_t
.. doxygenenum:: rmt_source_clk_t
.. doxygenenum:: rmt_data_mode_t
.. doxygenenum:: rmt_mode_t
.. doxygenenum:: rmt_idle_level_t
.. doxygenenum:: rmt_carrier_level_t

Structures
^^^^^^^^^^

.. doxygenstruct:: rmt_tx_config_t
    :members:

.. doxygenstruct:: rmt_rx_config_t
    :members:

.. doxygenstruct:: rmt_config_t
    :members:


Functions
^^^^^^^^^

.. doxygenfunction:: rmt_set_clk_div
.. doxygenfunction:: rmt_get_clk_div
.. doxygenfunction:: rmt_set_rx_idle_thresh
.. doxygenfunction:: rmt_get_rx_idle_thresh
.. doxygenfunction:: rmt_set_mem_block_num
.. doxygenfunction:: rmt_get_mem_block_num
.. doxygenfunction:: rmt_set_tx_carrier
.. doxygenfunction:: rmt_set_mem_pd
.. doxygenfunction:: rmt_get_mem_pd
.. doxygenfunction:: rmt_tx_start
.. doxygenfunction:: rmt_tx_stop
.. doxygenfunction:: rmt_rx_start
.. doxygenfunction:: rmt_rx_stop
.. doxygenfunction:: rmt_memory_rw_rst
.. doxygenfunction:: rmt_set_memory_owner
.. doxygenfunction:: rmt_get_memory_owner
.. doxygenfunction:: rmt_set_tx_loop_mode
.. doxygenfunction:: rmt_get_tx_loop_mode
.. doxygenfunction:: rmt_set_rx_filter
.. doxygenfunction:: rmt_set_source_clk
.. doxygenfunction:: rmt_get_source_clk
.. doxygenfunction:: rmt_set_idle_level
.. doxygenfunction:: rmt_get_status
.. doxygenfunction:: rmt_set_intr_enable_mask
.. doxygenfunction:: rmt_clr_intr_enable_mask
.. doxygenfunction:: rmt_set_rx_intr_en
.. doxygenfunction:: rmt_set_err_intr_en
.. doxygenfunction:: rmt_set_tx_intr_en
.. doxygenfunction:: rmt_set_evt_intr_en
.. doxygenfunction:: rmt_set_pin
.. doxygenfunction:: rmt_config
.. doxygenfunction:: rmt_isr_register
.. doxygenfunction:: rmt_fill_tx_items
.. doxygenfunction:: rmt_driver_install
.. doxygenfunction:: rmt_driver_uninstall
.. doxygenfunction:: rmt_write_items
.. doxygenfunction:: rmt_wait_tx_done
.. doxygenfunction:: rmt_get_ringbuf_handler

