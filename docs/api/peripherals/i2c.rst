I2C
===========

Overview
--------

ESP32 has two I2C controllers which can be set as master mode or slave mode.

Application Example
-------------------

I2C master and slave example: `examples/peripherals/i2c <https://github.com/espressif/esp-idf/tree/master/examples/peripherals/i2c>`_.

API Reference
-------------

Header Files
^^^^^^^^^^^^

  * `driver/include/driver/i2c.h <https://github.com/espressif/esp-idf/blob/master/components/driver/include/driver/i2c.h>`_

Macros
^^^^^^

.. doxygendefine:: I2C_APB_CLK_FREQ
.. doxygendefine:: I2C_FIFO_LEN

Type Definitions
^^^^^^^^^^^^^^^^

.. doxygentypedef:: i2c_cmd_handle_t

Enumerations
^^^^^^^^^^^^

.. doxygenenum:: i2c_mode_t
.. doxygenenum:: i2c_rw_t
.. doxygenenum:: i2c_trans_mode_t
.. doxygenenum:: i2c_opmode_t
.. doxygenenum:: i2c_port_t
.. doxygenenum:: i2c_addr_mode_t

Structures
^^^^^^^^^^

.. doxygenstruct:: i2c_config_t
    :members:


Functions
^^^^^^^^^

.. doxygenfunction:: i2c_driver_install
.. doxygenfunction:: i2c_driver_delete
.. doxygenfunction:: i2c_param_config
.. doxygenfunction:: i2c_reset_tx_fifo
.. doxygenfunction:: i2c_reset_rx_fifo
.. doxygenfunction:: i2c_isr_register
.. doxygenfunction:: i2c_isr_free
.. doxygenfunction:: i2c_set_pin
.. doxygenfunction:: i2c_master_start
.. doxygenfunction:: i2c_master_write_byte
.. doxygenfunction:: i2c_master_write
.. doxygenfunction:: i2c_master_read_byte
.. doxygenfunction:: i2c_master_read
.. doxygenfunction:: i2c_master_stop
.. doxygenfunction:: i2c_master_cmd_begin
.. doxygenfunction:: i2c_slave_write_buffer
.. doxygenfunction:: i2c_slave_read
.. doxygenfunction:: i2c_set_period
.. doxygenfunction:: i2c_get_period
.. doxygenfunction:: i2c_set_start_timing
.. doxygenfunction:: i2c_get_start_timing
.. doxygenfunction:: i2c_set_stop_timing
.. doxygenfunction:: i2c_get_stop_timing
.. doxygenfunction:: i2c_set_data_timing
.. doxygenfunction:: i2c_get_data_timing
.. doxygenfunction:: i2c_set_data_mode
.. doxygenfunction:: i2c_get_data_mode
.. doxygenfunction:: i2c_cmd_link_create
.. doxygenfunction:: i2c_cmd_link_delete

