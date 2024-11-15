.. _i2c-slave-v1:

I2C Slave v1.0
==============

.. warning::

    This I2C slave driver version 1 will be removed when idf v6.0 update. We suggest you use I2C slave version 2 via :ref:`CONFIG_I2C_ENABLE_SLAVE_DRIVER_VERSION_2`.

After installing the I2C slave driver by :cpp:func:`i2c_new_slave_device`, {IDF_TARGET_NAME} is ready to communicate with other I2C masters as a slave.

Install I2C slave device
~~~~~~~~~~~~~~~~~~~~~~~~

I2C slave requires the configuration specified by :cpp:type:`i2c_slave_config_t`:

.. list::

    - :cpp:member:`i2c_slave_config_t::i2c_port` sets the I2C port used by the controller.
    - :cpp:member:`i2c_slave_config_t::sda_io_num` sets the GPIO number for serial data bus (SDA).
    - :cpp:member:`i2c_slave_config_t::scl_io_num` sets the GPIO number for serial clock bus (SCL).
    - :cpp:member:`i2c_slave_config_t::clk_source` selects the source clock for I2C bus. The available clocks are listed in :cpp:type:`i2c_clock_source_t`. For the effect on power consumption of different clock source, please refer to `Power Management <#power-management>`__  section.
    - :cpp:member:`i2c_slave_config_t::send_buf_depth` sets the sending buffer length.
    - :cpp:member:`i2c_slave_config_t::slave_addr` sets the slave address.
    - :cpp:member:`i2c_slave_config_t::intr_priority` sets the priority of the interrupt. If set to ``0`` , then the driver will use a interrupt with low or medium priority (priority level may be one of 1, 2 or 3), otherwise use the priority indicated by :cpp:member:`i2c_slave_config_t::intr_priority`. Please use the number form (1, 2, 3), instead of the bitmask form ((1<<1), (1<<2), (1<<3)). Please pay attention that once the interrupt priority is set, it cannot be changed until :cpp:func:`i2c_del_slave_bus` is called.
    - :cpp:member:`i2c_slave_config_t::addr_bit_len`. Set this variable to ``I2C_ADDR_BIT_LEN_10`` if the slave should have a 10-bit address.
    :SOC_I2C_SLAVE_CAN_GET_STRETCH_CAUSE: - :cpp:member:`i2c_slave_config_t::stretch_en`. Set this variable to true, then the slave controller stretch will work. Please refer to [`TRM <{IDF_TARGET_TRM_EN_URL}#i2c>`__] to learn how I2C stretch works.
    :SOC_I2C_SLAVE_CAN_GET_STRETCH_CAUSE: - :cpp:member:`i2c_slave_config_t::broadcast_en`. Set this to true to enable the slave broadcast. When the slave receives the general call address 0x00 from the master and the R/W bit followed is 0, it responds to the master regardless of its own address.
    :SOC_I2C_SLAVE_SUPPORT_I2CRAM_ACCESS: - :cpp:member:`i2c_slave_config_t::access_ram_en`. Set this to true to enable the non-FIFO mode. Thus the I2C data FIFO can be used as RAM, and double addressing will be synchronised opened.
    :SOC_I2C_SLAVE_SUPPORT_SLAVE_UNMATCH: - :cpp:member:`i2c_slave_config_t::slave_unmatch_en`. Set this to true to enable the slave unmatch interrupt. If the command address sent by master can't match the slave address, then unmatch interrupt will be triggered.

Once the :cpp:type:`i2c_slave_config_t` structure is populated with mandatory parameters, :cpp:func:`i2c_new_slave_device` can be called to allocate and initialize an I2C master bus. This function will return an I2C bus handle if it runs correctly. Specifically, when there are no more I2C port available, this function will return :c:macro:`ESP_ERR_NOT_FOUND` error.

.. code:: c

    i2c_slave_config_t i2c_slv_config = {
        .addr_bit_len = I2C_ADDR_BIT_LEN_7,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = TEST_I2C_PORT,
        .send_buf_depth = 256,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .slave_addr = 0x58,
    };

    i2c_slave_dev_handle_t slave_handle;
    ESP_ERROR_CHECK(i2c_new_slave_device(&i2c_slv_config, &slave_handle));

Uninstall I2C slave device
~~~~~~~~~~~~~~~~~~~~~~~~~~

If a previously installed I2C bus is no longer needed, it's recommended to recycle the resource by calling :cpp:func:`i2c_del_slave_device`, so that to release the underlying hardware.


I2C Slave Write
~~~~~~~~~~~~~~~

The send buffer of the I2C slave is used as a FIFO to store the data to be sent. The data will queue up until the master requests them. You can call :cpp:func:`i2c_slave_transmit` to transfer data.

Simple example for writing data to FIFO:

.. code:: c

    uint8_t *data_wr = (uint8_t *) malloc(DATA_LENGTH);

    i2c_slave_config_t i2c_slv_config = {
        .addr_bit_len = I2C_ADDR_BIT_LEN_7,   // 7-bit address
        .clk_source = I2C_CLK_SRC_DEFAULT,    // set the clock source
        .i2c_port = TEST_I2C_PORT,            // set I2C port number
        .send_buf_depth = 256,                // set TX buffer length
        .scl_io_num = I2C_SLAVE_SCL_IO,       // SCL GPIO number
        .sda_io_num = I2C_SLAVE_SDA_IO,       // SDA GPIO number
        .slave_addr = 0x58,                   // slave address
    };

    i2c_slave_dev_handle_t slave_handle;
    ESP_ERROR_CHECK(i2c_new_slave_device(&i2c_slv_config, &slave_handle));
    for (int i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i;
    }

    ESP_ERROR_CHECK(i2c_slave_transmit(slave_handle, data_wr, DATA_LENGTH, 10000));

I2C Slave Read
~~~~~~~~~~~~~~

Whenever the master writes data to the slave, the slave will automatically store data in the receive buffer. This allows the slave application to call the function :cpp:func:`i2c_slave_receive` as its own discretion. As :cpp:func:`i2c_slave_receive` is designed as a non-blocking interface, users need to register callback :cpp:func:`i2c_slave_register_event_callbacks` to know when the receive has finished.

.. code:: c

    static IRAM_ATTR bool i2c_slave_rx_done_callback(i2c_slave_dev_handle_t channel, const i2c_slave_rx_done_event_data_t *edata, void *user_data)
    {
        BaseType_t high_task_wakeup = pdFALSE;
        QueueHandle_t receive_queue = (QueueHandle_t)user_data;
        xQueueSendFromISR(receive_queue, edata, &high_task_wakeup);
        return high_task_wakeup == pdTRUE;
    }

    uint8_t *data_rd = (uint8_t *) malloc(DATA_LENGTH);
    uint32_t size_rd = 0;

    i2c_slave_config_t i2c_slv_config = {
        .addr_bit_len = I2C_ADDR_BIT_LEN_7,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = TEST_I2C_PORT,
        .send_buf_depth = 256,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .slave_addr = 0x58,
    };

    i2c_slave_dev_handle_t slave_handle;
    ESP_ERROR_CHECK(i2c_new_slave_device(&i2c_slv_config, &slave_handle));

    s_receive_queue = xQueueCreate(1, sizeof(i2c_slave_rx_done_event_data_t));
    i2c_slave_event_callbacks_t cbs = {
        .on_recv_done = i2c_slave_rx_done_callback,
    };
    ESP_ERROR_CHECK(i2c_slave_register_event_callbacks(slave_handle, &cbs, s_receive_queue));

    i2c_slave_rx_done_event_data_t rx_data;
    ESP_ERROR_CHECK(i2c_slave_receive(slave_handle, data_rd, DATA_LENGTH));
    xQueueReceive(s_receive_queue, &rx_data, pdMS_TO_TICKS(10000));
    // Receive done.

.. only:: SOC_I2C_SLAVE_SUPPORT_I2CRAM_ACCESS

    Put Data In I2C Slave RAM
    ~~~~~~~~~~~~~~~~~~~~~~~~~

    I2C slave FIFO mentioned above can be used as RAM, which means user can access the RAM directly via address fields. For example, write data to the third RAM block with following graph. Before using this, please note that :cpp:member:`i2c_slave_config_t::access_ram_en` needs to be set to true.

    .. figure:: ../../../_static/diagrams/i2c/i2c_slave_write_slave_ram.png
        :align: center
        :alt: Put data in I2C slave RAM

        Put data in I2C slave RAM

    .. code:: c

        uint8_t data_rd[DATA_LENGTH_RAM] = {0};

        i2c_slave_config_t i2c_slv_config = {
            .addr_bit_len = I2C_ADDR_BIT_LEN_7,
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .i2c_port = TEST_I2C_PORT,
            .send_buf_depth = 256,
            .scl_io_num = I2C_SLAVE_SCL_IO,
            .sda_io_num = I2C_SLAVE_SDA_IO,
            .slave_addr = 0x58,
            .flags.access_ram_en = true,
        };

        // Master writes to slave.

        i2c_slave_dev_handle_t slave_handle;
        ESP_ERROR_CHECK(i2c_new_slave_device(&i2c_slv_config, &slave_handle));
        ESP_ERROR_CHECK(i2c_slave_read_ram(slave_handle, 0x5, data_rd, DATA_LENGTH_RAM));
        ESP_ERROR_CHECK(i2c_del_slave_device(slave_handle));

    Get Data From I2C Slave RAM
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~

    Data can be stored in the RAM with a specific offset by the slave controller, and the master can read this data directly via the RAM address. For example, if the data is stored in the third RAM block, master can read this data by the following graph. Before using this, please note that :cpp:member:`i2c_slave_config_t::access_ram_en` needs to be set to true.

    .. figure:: ../../../_static/diagrams/i2c/i2c_slave_read_slave_ram.png
        :align: center
        :alt: Get data from I2C slave RAM

        Get data from I2C slave RAM

    .. code:: c

        uint8_t data_wr[DATA_LENGTH_RAM] = {0};

        i2c_slave_config_t i2c_slv_config = {
            .addr_bit_len = I2C_ADDR_BIT_LEN_7,
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .i2c_port = TEST_I2C_PORT,
            .send_buf_depth = 256,
            .scl_io_num = I2C_SLAVE_SCL_IO,
            .sda_io_num = I2C_SLAVE_SDA_IO,
            .slave_addr = 0x58,
            .flags.access_ram_en = true,
        };

        i2c_slave_dev_handle_t slave_handle;
        ESP_ERROR_CHECK(i2c_new_slave_device(&i2c_slv_config, &slave_handle));
        ESP_ERROR_CHECK(i2c_slave_write_ram(slave_handle, 0x2, data_wr, DATA_LENGTH_RAM));
        ESP_ERROR_CHECK(i2c_del_slave_device(slave_handle));

I2C slave callbacks
~~~~~~~~~~~~~~~~~~~

When an I2C slave bus triggers an interrupt, a specific event will be generated and notify the CPU. If you have some function that needs to be called when those events occurred, you can hook your function to the ISR (Interrupt Service Routine) by calling :cpp:func:`i2c_slave_register_event_callbacks`. Since the registered callback functions are called in the interrupt context, users should ensure the callback function doesn't attempt to block (e.g. by making sure that only FreeRTOS APIs with ``ISR`` suffix are called from the function). The callback function has a boolean return value, to tell the caller whether a high priority task is woken up by it.

I2C slave event callbacks are listed in the :cpp:type:`i2c_slave_event_callbacks_t`.

.. list::

    - :cpp:member:`i2c_slave_event_callbacks_t::on_recv_done` sets a callback function for "receive-done" event. The function prototype is declared in :cpp:type:`i2c_slave_received_callback_t`.
    :SOC_I2C_SLAVE_CAN_GET_STRETCH_CAUSE: - :cpp:member:`i2c_slave_event_callbacks_t::on_stretch_occur` sets a callback function for "stretch" cause. The function prototype is declared in :cpp:type:`i2c_slave_stretch_callback_t`.
