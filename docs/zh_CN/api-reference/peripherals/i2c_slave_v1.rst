.. _i2c-slave-v1:

I2C Slave v1.0
=================

.. warning::

    该 I2C 从机驱动程序 v1.0 将在 ESP-IDF v6.0 中删除。我们建议你通过 :ref:`CONFIG_I2C_ENABLE_SLAVE_DRIVER_VERSION_2` 使用 I2C 驱动程序 v2.0。

通过 :cpp:func:`i2c_new_slave_device` 安装 I2C 从机驱动程序后，{IDF_TARGET_NAME} 就可以作为从站与其他 I2C Master 通信了。

安装 I2C 从机设备
~~~~~~~~~~~~~~~~~~~

I2C 从机设备需要 :cpp:type:`i2c_slave_config_t` 指定的配置：

.. list::

    - :cpp:member:`i2c_slave_config_t::i2c_port` 设置控制器使用的 I2C 端口。
    - :cpp:member:`i2c_slave_config_t::sda_io_num` 设置串行数据总线 (SDA) 的 GPIO 编号。
    - :cpp:member:`i2c_slave_config_t::scl_io_num` 设置串行时钟总线 (SCL) 的 GPIO 编号。
    - :cpp:member:`i2c_slave_config_t::clk_source` 选择 I2C 总线的时钟源。可用时钟列表见 :cpp:type:`i2c_clock_source_t`。有关不同时钟源对功耗的影响，请参阅 `电源管理 <#power-management>`__。
    - :cpp:member:`i2c_slave_config_t::send_buf_depth` 设置发送 buffer 的长度。
    - :cpp:member:`i2c_slave_config_t::slave_addr` 设置从机地址。
    - :cpp:member:`i2c_slave_config_t::intr_priority` 设置中断的优先级。如果设置为 ``0`` ，则驱动程序将使用低或中优先级的中断（优先级可设为 1、2 或 3 中的一个），若未设置，则将使用 :cpp:member:`i2c_slave_config_t::intr_priority` 指示的优先级。请使用数字形式（1、2、3），不要用位掩码形式（(1<<1)、(1<<2)、(1<<3)）。请注意，中断优先级一旦设置完成，在调用 :cpp:func:`i2c_del_slave_bus` 之前都无法更改。
    - :cpp:member:`i2c_slave_config_t::addr_bit_len`。如果需要从机设备具有 10 位地址，则将该成员变量设为 ``I2C_ADDR_BIT_LEN_10``。
    :SOC_I2C_SLAVE_CAN_GET_STRETCH_CAUSE: - :cpp:member:`i2c_slave_config_t::stretch_en`。如果要启用从机控制器拉伸功能，请将该成员变量设为 true。有关 I2C 拉伸的工作原理，请参阅 [`TRM <{IDF_TARGET_TRM_EN_URL}#i2c>`__]。
    :SOC_I2C_SLAVE_CAN_GET_STRETCH_CAUSE: - :cpp:member:`i2c_slave_config_t::broadcast_en`。如果要启用从机广播，请将该成员变量设为 true。当从机设备接收到来自主机设备的通用调用地址 0x00，且后面的读写位为 0 时，无论从机设备自身地址如何，都会响应主机设备。
    :SOC_I2C_SLAVE_SUPPORT_I2CRAM_ACCESS: - :cpp:member:`i2c_slave_config_t::access_ram_en`。如果要启用 non-FIFO 模式，请将该成员变量设为 true，则 I2C 数据 FIFO 可用作 RAM，并将同步打开双地址。
    :SOC_I2C_SLAVE_SUPPORT_SLAVE_UNMATCH: - :cpp:member:`i2c_slave_config_t::slave_unmatch_en`。将该成员变量设为 true，将启用从机设备不匹配中断。如果主机设备发送的命令地址与从机设备地址不匹配，则会触发不匹配中断。

一旦填充好 :cpp:type:`i2c_slave_config_t` 结构体的必要参数，就可调用 :cpp:func:`i2c_new_slave_device` 来分配和初始化 I2C 主机总线。如果函数运行正确，则将返回一个 I2C 总线句柄。若没有可用的 I2C 端口，此函数将返回 :c:macro:`ESP_ERR_NOT_FOUND` 错误。

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

卸载 I2C 从机设备
~~~~~~~~~~~~~~~~~~

如果不再需要之前安装的 I2C 总线，建议调用 :cpp:func:`i2c_del_slave_device` 来回收资源，以释放底层硬件。


I2C 从机写入
~~~~~~~~~~~~~

I2C 从机的发送 buffer 可作为 FIFO 来存储要发送的数据。在主机请求这些数据前，它们会一直排队。可通过调用 :cpp:func:`i2c_slave_transmit` 来传输数据。

将数据写入 FIFO 的简单示例：

.. code:: c

    uint8_t *data_wr = (uint8_t *) malloc(DATA_LENGTH);

    i2c_slave_config_t i2c_slv_config = {
        .addr_bit_len = I2C_ADDR_BIT_LEN_7,   // 7 位地址
        .clk_source = I2C_CLK_SRC_DEFAULT,    // 设置时钟源
        .i2c_port = TEST_I2C_PORT,            // 设置 I2C 端口编号
        .send_buf_depth = 256,                // 设置 TX buffer 长度
        .scl_io_num = I2C_SLAVE_SCL_IO,       // SCL 管脚编号
        .sda_io_num = I2C_SLAVE_SDA_IO,       // SDA 管脚编号
        .slave_addr = 0x58,                   // 从机地址
    };

    i2c_slave_dev_handle_t slave_handle;
    ESP_ERROR_CHECK(i2c_new_slave_device(&i2c_slv_config, &slave_handle));
    for (int i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i;
    }

    ESP_ERROR_CHECK(i2c_slave_transmit(slave_handle, data_wr, DATA_LENGTH, 10000));

I2C 从机读取
~~~~~~~~~~~~~

每当主机将数据写入从机，从机都会自动将数据存储在接收 buffer 中，从而使从机应用程序能自由调用 :cpp:func:`i2c_slave_receive`。:cpp:func:`i2c_slave_receive` 为非阻塞接口，因此要想知道接收是否完成，需注册回调函数 :cpp:func:`i2c_slave_register_event_callbacks`。

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
    // 接收完成。

.. only:: SOC_I2C_SLAVE_SUPPORT_I2CRAM_ACCESS

    将数据放入 I2C 从机 RAM 中
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~

    如上所述，I2C 从机 FIFO 可被用作 RAM，即可以通过地址字段直接访问 RAM。例如，可参照下图将数据写入第三个 RAM 块。请注意，在进行操作前需要先将 :cpp:member:`i2c_slave_config_t::access_ram_en` 设为 true。

    .. figure:: ../../../_static/diagrams/i2c/i2c_slave_write_slave_ram.png
        :align: center
        :alt: 将数据放入 I2C 从机 RAM 中

        将数据放入 I2C 从机 RAM 中

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

        // 主机将数据写入从机。

        i2c_slave_dev_handle_t slave_handle;
        ESP_ERROR_CHECK(i2c_new_slave_device(&i2c_slv_config, &slave_handle));
        ESP_ERROR_CHECK(i2c_slave_read_ram(slave_handle, 0x5, data_rd, DATA_LENGTH_RAM));
        ESP_ERROR_CHECK(i2c_del_slave_device(slave_handle));

    从 I2C 从机 RAM 中获取数据
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~

    数据可被存储在相对从机一定偏移量的 RAM 中，且主机可直接通过 RAM 地址读取这些数据。例如，如果数据被存储在第三个 RAM 块中，则主机可参照下图读取这些数据。请注意，在操作前需要先将 :cpp:member:`i2c_slave_config_t::access_ram_en` 设为 true。

    .. figure:: ../../../_static/diagrams/i2c/i2c_slave_read_slave_ram.png
        :align: center
        :alt: 从 I2C 从机 RAM 中获取数据

        从 I2C 从机 RAM 中获取数据

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

I2C 从机回调
~~~~~~~~~~~~~

当 I2C 从机总线触发中断时，将生成特定事件并通知 CPU。如果需要在发生这些事件时调用某些函数，可通过 :cpp:func:`i2c_slave_register_event_callbacks` 将这些函数挂接到中断服务程序 (ISR) 上。由于注册的回调函数是在中断上下文中被调用的，所以应确保这些函数不会导致延迟（例如，确保仅从函数中调用带有 ``ISR`` 后缀的 FreeRTOS API）。回调函数需要返回一个布尔值，告诉调用者是否唤醒了高优先级任务。

I2C 从机事件回调函数列表见 :cpp:type:`i2c_slave_event_callbacks_t`。

.. list::

    - :cpp:member:`i2c_slave_event_callbacks_t::on_recv_done` 可设置用于“接收完成”事件的回调函数。该函数原型在 :cpp:type:`i2c_slave_received_callback_t` 中声明。
    :SOC_I2C_SLAVE_CAN_GET_STRETCH_CAUSE: - :cpp:member:`i2c_slave_event_callbacks_t::on_stretch_occur` 可设置用于“时钟拉伸”事件的回调函数。该函数原型在 :cpp:type:`i2c_slave_stretch_callback_t` 中声明。
