外设
============

:link_to_translation:`en:[English]`

.. only:: SOC_UART_SUPPORTED

    UART
    ----

    - :c:macro:`UART_FIFO_LEN` 已弃用，更新为 :c:macro:`UART_HW_FIFO_LEN`。

.. only:: SOC_I2C_SUPPORTED

    I2C
    ---

    I2C 驱动已经被重新设计在 :doc:`I2C <../../../api-reference/peripherals/i2c>`, 以统一接口并扩展 I2C 外设的使用。尽管我们推荐使用新驱动的接口, 但用户仍然可以通过包含路径 ``driver/i2c.h`` 来使用老驱动。

    主要的概念上和用法上的改变如下所示:

    主要概念更新
    ~~~~~~~~~~~~~~~~~~

    -  用于初始化整个 I2C 总线的结构体 ``i2c_config_t`` 已经被移除. 在新驱动中, 从机和主机是分开的. 用户可以独立调用 :cpp:type:`i2c_master_bus_config_t` 和 :cpp:type:`i2c_slave_config_t`。
    - ``i2c_mode_t`` 用来判断 I2C 控制器是工作在从模式还是主模式。该枚举器已被弃用。在新驱动程序中，用户无需自行设置，驱动程序会妥善处理。
    - ``i2c_rw_t`` 用来判断 I2C 主控制器是在进行 "写 "还是 "读"。现在，它已被弃用。
    - ``i2c_addr_mode_t`` 被重命名为 ``i2c_addr_bit_len_t``.
    -  在老驱动中，你需要将 I2C 命令通过 ``I2C_LINK_RECOMMENDED_SIZE``, ``i2c_cmd_link_create_static``, 等链接成链表. 在新驱动中，你不需要这么做，你只需要调用相关的接口函数即可。
    -  在老驱动中，选择时钟通过例如 ``I2C_SCLK_SRC_FLAG_FOR_NOMAL`` 的时钟标志位. 在新驱动中，你可以直接选择时钟源。

    主要用法更新
    ~~~~~~~~~~~~~~~~~~

    -  I2C 总线初始化通过两部分完成. 第一步通过 :cpp:func:`i2c_new_master_bus` 初始化 I2C 总线, 然后初始化 I2C 设备通过 :cpp:func:`i2c_master_bus_add_device`。
    - ``i2c_reset_tx_fifo`` 和 ``i2c_reset_rx_fifo`` 已被删除，因为用户不太需要重置 fifo。但可以通过 ``i2c_master_bus_reset`` 重置整个总线。
    - 删除了 ``i2c_cmd_link_xxx`` 函数，用户不需要自己使用 link 来链接命令。
    - ``i2c_master_write_to_device`` 更名为 ``i2c_master_transmit``。
    - ``i2c_master_read_from_device`` 已更名为 ``i2c_master_receive``。
    - ``i2c_master_write_read_device`` 已更名为 ``i2c_master_transmit_receive``。
    - ``i2c_slave_write_buffer`` 重命名为 ``i2c_slave_transmit``。
    - ``i2c_slave_read_buffer`` 已更名为 ``i2c_slave_receive``。
