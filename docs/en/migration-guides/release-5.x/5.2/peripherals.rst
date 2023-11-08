Peripherals
===========

:link_to_translation:`zh_CN:[中文]`

.. only:: SOC_UART_SUPPORTED

    UART
    ----

    - :c:macro:`UART_FIFO_LEN` is deprecated. Please use :c:macro:`UART_HW_FIFO_LEN` instead.

.. only:: SOC_I2C_SUPPORTED

    I2C
    ---

    I2C driver has been redesigned (see :doc:`I2C API Reference <../../../api-reference/peripherals/i2c>`), which aims to unify the interface and extend the usage of I2C peripheral. Although it is recommended to use the new driver APIs, the legacy driver is still available in the previous include path ``driver/i2c.h``.

    The major breaking changes in concept and usage are listed as follows:

    Major Changes in Concepts
    ~~~~~~~~~~~~~~~~~~~~~~~~~

    - ``i2c_config_t`` which was used to configure the I2C bus, but it doesn't really tell whether to configure master or slave. So in the new design, master and slave initialization are separate, user can call ``i2c_master_bus_config_t`` or ``i2c_slave_config_t``.
    - ``i2c_mode_t`` which was used to tell whether I2C controller works in slave mode or master mode. This enumerator has been deprecated. In the new driver, users don'tneed to manually set the mode anymore since master and slave APIs are different.
    - ``i2c_rw_t`` which was used to tell whether I2C master controller is performing a `write` or a `read` operation. This is now deprecated.
    - ``i2c_addr_mode_t`` was renamed to ``i2c_addr_bit_len_t``.
    -  In the legacy driver, operations needed to be chained with a command list (dynamically or statically created). The new driver now handles this internally, making the operations more size and space efficient.
    -  Capability flags like ``I2C_SCLK_SRC_FLAG_FOR_NOMAL`` are used to select clock source in the legacy driver. In the new driver, users can select clock source directly.

    Major Changes in Usage
    ~~~~~~~~~~~~~~~~~~~~~~

    -  I2C bus initialization is done in two parts: first, initialization of the bus with :cpp:func:`i2c_new_master_bus`, then, initialization of the I2C device with :cpp:func:`i2c_master_bus_add_device`.
    -  ``i2c_reset_tx_fifo`` and ``i2c_reset_rx_fifo`` have been removed, since it is never required to reset the fifo by users. Whole bus can still be reset by calling ``i2c_master_bus_reset``.
    -  ``i2c_cmd_link_xxx`` functions have been removed, user doesn't need to use link to link commands on its own.
    -  ``i2c_master_write_to_device`` has been renamed to ``i2c_master_transmit``.
    -  ``i2c_master_read_from_device`` has been renamed to ``i2c_master_receive``.
    -  ``i2c_master_write_read_device`` has been renamed to ``i2c_master_transmit_receive``
    -  ``i2c_slave_write_buffer`` has been renamed to ``i2c_slave_transmit``
    -  ``i2c_slave_read_buffer`` has been renamed to ``i2c_slave_receive``
