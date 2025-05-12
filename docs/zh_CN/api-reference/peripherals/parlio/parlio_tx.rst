并行 IO TX 驱动程序
=====================

:link_to_translation:`en:[English]`

本文介绍了 ESP-IDF 中的并行 IO TX 驱动程序的功能，章节目录如下：

.. contents::
    :local:
    :depth: 2

简介
----

并行 IO TX 单元属于通用的并行接口的一部分，以下简称为 TX 单元。支持通过 GDMA 在并行总线上实现外部设备和内部存储器之间的数据通信，鉴于 IO 数据的灵活性，TX 单元可用作通用接口，连接多种外围设备。该驱动的主要应用场景包括：

- 用于驱动 LCD、LED 显示屏
- 与其他设备进行高速并行通信
- 当其他外设数量不够时，使用 TX 单元来模拟该外设的时序。

快速入门
--------

本节将带你快速了解如何使用 TX 单元驱动。通过简单的示例模拟 QPI (Quad Peripheral Interface) 发送时序，展示如何使用创建 TX 单元并启动它，如何发起传输事务，以及如何注册事件回调函数。一般的使用流程如下：

创建和使能 TX 单元
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

首先，我们需要创建一个 TX 单元实例。以下代码展示了如何创建一个用于模拟 QPI 的 TX 单元实例：

.. code:: c

    parlio_tx_unit_handle_t tx_unit = NULL;
    parlio_tx_unit_config_t config = {
        .clk_src = PARLIO_CLK_SRC_DEFAULT,      // 选择默认的时钟源
        .data_width = 4,                        // 数据宽度为 4 位
        .clk_in_gpio_num = -1,                  // 不使用外部时钟源
        .valid_gpio_num = EXAMPLE_PIN_CS,       // 使用有效信号作为片选
        .clk_out_gpio_num = EXAMPLE_PIN_CLK,
        .data_gpio_nums = {
            EXAMPLE_PIN_DATA0,
            EXAMPLE_PIN_DATA1,
            EXAMPLE_PIN_DATA2,
            EXAMPLE_PIN_DATA3,
        },
        .output_clk_freq_hz = 10 * 1000 * 1000, // 输出时钟频率为 10 MHz
        .trans_queue_depth = 32,                // 待处理事务队列深度为 32
        .max_transfer_size = 256,               // 一次传输的最大传输大小为 256 字节
        .sample_edge = PARLIO_SAMPLE_EDGE_NEG,  // 在时钟下降沿采样数据
        .flags = {
            .invert_valid_out = true, // 有效信号默认高电平有效，通过反转，我们用来模拟 QPI 的时序中的片选信号 CS
        }
    };
    // 创建 TX 单元实例
    ESP_ERROR_CHECK(parlio_new_tx_unit(&config, &tx_unit));
    // 使能 TX 单元
    ESP_ERROR_CHECK(parlio_tx_unit_enable(tx_unit));

当创建 TX 单元实例时，我们需要通过 :cpp:type:`parlio_tx_unit_config_t` 配置时钟源、数据宽度和输出时钟频率等参数。然后调用 :cpp:func:`parlio_new_tx_unit` 函数创建一个新的 TX 单元实例，该函数将返回一个指向新实例的句柄。实例句柄实际上是一个指向 TX 单元内存对象的指针，类型为 :cpp:type:`parlio_tx_unit_handle_t`。

以下是 :cpp:type:`parlio_tx_unit_config_t` 结构体的配置参数及其解释：

.. list::
    -  :cpp:member:`parlio_tx_unit_config_t::clk_src` 设置 TX 单元的时钟源。可用时钟源列在 :cpp:type:`parlio_clock_source_t` 中，只能选择其中一个。不同的时钟源会在分辨率，精度和功耗上有所不同。
    -  :cpp:member:`parlio_tx_unit_config_t::clk_in_gpio_num` 使用外部时钟作为时钟源，设置相应的时钟输入的 GPIO 编号。否则，设置为 -1，驱动程序将使用内部 :cpp:member:`parlio_tx_unit_config_t::clk_src` 作为时钟源。该选项的优先级高于 :cpp:member:`parlio_tx_unit_config_t::clk_src`。
    -  :cpp:member:`parlio_tx_unit_config_t::input_clk_src_freq_hz` 外部输入时钟源的频率，仅当 :cpp:member:`parlio_tx_unit_config_t::clk_in_gpio_num` 不为 -1 时有效。
    -  :cpp:member:`parlio_tx_unit_config_t::output_clk_freq_hz` 设定输出时钟的频率，由内部时钟源或外部时钟源分频得到。注意，并非所有频率都可以得到，当无法得到设置的频率时，驱动会自动调整至接近的频率。
    -  :cpp:member:`parlio_tx_unit_config_t::clk_out_gpio_num` 输出时钟信号的 GPIO 编号。
    -  :cpp:member:`parlio_tx_unit_config_t::data_width`  TX 单元数据总线宽度，必须为 2 的幂次方，且不能大于 {IDF_TARGET_SOC_PARLIO_TX_UNIT_MAX_DATA_WIDTH}。
    -  :cpp:member:`parlio_tx_unit_config_t::data_gpio_nums`  TX 数据 GPIO 编号，未使用的 GPIO 设置为 -1。
    -  :cpp:member:`parlio_tx_unit_config_t::valid_gpio_num` 有效信号的 GPIO 编号，未使用则设置为 -1。有效信号会在 TX 传输数据时保持高电平。注意，在部分芯片上启用有效信号会占用 MSB 数据位，导致 TX 单元的最大数据宽度减少 1 位，此时数据总线宽度的最大可配置为 :c:macro:`SOC_PARLIO_TX_UNIT_MAX_DATA_WIDTH` / 2， 因此请检查 :cpp:func:`parlio_new_tx_unit` 的返回值。
    -  :cpp:member:`parlio_tx_unit_config_t::valid_start_delay` 有效信号将在数据发送之前保持“有效”状态的时钟周期。此配置选项依赖于特定的硬件功能，如果在不支持的芯片上启用它，或配置了无效的值，你将看到类似 ``invalid valid delay`` 的错误消息。
    -  :cpp:member:`parlio_tx_unit_config_t::valid_stop_delay` 有效信号将在数据发送完成之后保持“有效”状态的时钟周期。此配置选项依赖于特定的硬件功能，如果在不支持的芯片上启用它，或配置了无效的值，你将看到类似 ``invalid valid delay`` 的错误消息。
    -  :cpp:member:`parlio_tx_unit_config_t::trans_queue_depth` 内部事务队列深度。队列越深，在待处理队列中可以准备的事务越多。
    -  :cpp:member:`parlio_tx_unit_config_t::max_transfer_size` 一次传输的最大传输大小（以字节为单位）。
    -  :cpp:member:`parlio_tx_unit_config_t::dma_burst_size` DMA 突发传输大小（以字节为单位），必须为 2 的幂次方。
    -  :cpp:member:`parlio_tx_unit_config_t::sample_edge`  TX 单元的数据采样边缘。
    -  :cpp:member:`parlio_tx_unit_config_t::bit_pack_order` 设置字节内数据位出现的顺序（仅当数据宽度 < 8 时有效）。
    -  :cpp:member:`parlio_tx_unit_config_t::flags` 通常用来微调驱动的一些行为，包括以下选项
    -  :cpp:member:`parlio_tx_unit_config_t::flags::invert_valid_out` 决定是否在将 TX 单元有效信号发送到 GPIO 管脚前反转信号。
    :SOC_PARLIO_TX_CLK_SUPPORT_GATING: -  :cpp:member:`parlio_tx_unit_config_t::flags::clk_gate_en` 启用 TX 单元时钟门控，输出时钟默认由数据总线的 MSB 位控制，即通过向 :cpp:member:`parlio_tx_unit_config_t::data_gpio_nums` [:c:macro:`SOC_PARLIO_TX_UNIT_MAX_DATA_WIDTH` - 1] 写入高电平使能时钟输出，低电平禁用，此时需要配置数据总线宽度为 :c:macro:`SOC_PARLIO_TX_UNIT_MAX_DATA_WIDTH`。注意，若有效信号输出和时钟门控同时启用，时钟门控则来自有效信号，对数据总线宽度没有额外要求（部分芯片上有效信号会占用 MSB 数据位，总线宽度的最大可配置为 :c:macro:`SOC_PARLIO_TX_UNIT_MAX_DATA_WIDTH` / 2）。
    :SOC_PARLIO_SUPPORT_SLEEP_RETENTION: -  :cpp:member:`parlio_tx_unit_config_t::flags::allow_pd` 配置驱动程序是否允许系统在睡眠模式下关闭外设电源。在进入睡眠之前，系统将备份 TX 单元寄存器上下文，当系统退出睡眠模式时，这些上下文将被恢复。关闭外设可以节省更多功耗，但代价是消耗更多内存来保存寄存器上下文。你需要在功耗和内存消耗之间做权衡。此配置选项依赖于特定的硬件功能，如果在不支持的芯片上启用它，你将看到类似 ``register back up is not supported`` 的错误消息。

.. note::

    如果当前芯片中所有的 TX 单元都已经被申请使用，那么 :cpp:func:`parlio_new_tx_unit` 函数会返回 :c:macro:`ESP_ERR_NOT_FOUND` 错误。

TX 单元在使用前必须要先使能，使能函数 :cpp:func:`parlio_tx_unit_enable` 可以将驱动的内部状态机切换到激活状态，这里面还会包括一些系统性服务的申请/注册等工作，如申请电源管理锁。与使能函数相对应的是禁用函数 :cpp:func:`parlio_tx_unit_disable`，它会释放所有的系统性服务。

.. note::

    调用 :cpp:func:`parlio_tx_unit_enable` 和 :cpp:func:`parlio_tx_unit_disable` 函数时，需要成对使用。这意味着，你不能连续调用两次 :cpp:func:`parlio_tx_unit_enable` 或 :cpp:func:`parlio_tx_unit_disable` 函数。这种成对调用的原则确保了资源的正确管理和释放。

.. note::

    请注意，TX 单元使能后会检查当前的作业队列，一旦队列中存在尚未开始的传输事务，驱动将立即发起一次传输。

发起 TX 传输事务
^^^^^^^^^^^^^^^^^^^^^^^

使能 TX 单元后，我们就可以配置传输的一些参数，并调用 :cpp:func:`parlio_tx_unit_transmit` 启动 TX 事务。以下代码展示了如何发起 TX 单元传输事务：

.. code:: c

    #define PAYLOAD_SIZE 128

    // 配置 TX 单元传输参数
    parlio_transmit_config_t transmit_config = {
        .idle_value = 0x00, // 空闲状态下所有数据线均为低电平
    };

    // 准备需要发送的数据
    uint8_t payload[PAYLOAD_SIZE] = {0};
    for (int i = 0; i < PAYLOAD_SIZE; i++) {
        payload[i] = i;
    }

    // 第一次调用 parlio_tx_unit_transmit 时，没有传输事务正在执行，立即开始本次传输事务
    ESP_ERROR_CHECK(parlio_tx_unit_transmit(tx_unit, payload, PAYLOAD_SIZE * sizeof(uint8_t) * 8, &transmit_config));
    // 第二次调用 parlio_tx_unit_transmit 时，前一个事务可能还没有完成，驱动将第二个事务发送到作业队列中，并在前一个事务完成后的 ISR 上下文中进行调度
    ESP_ERROR_CHECK(parlio_tx_unit_transmit(tx_unit, payload, PAYLOAD_SIZE * sizeof(uint8_t) * 8, &transmit_config));
    // （可选）等待 TX 单元完成所有事务
    ESP_ERROR_CHECK(parlio_tx_unit_wait_all_done(tx_unit, -1));

TX 单元以比特为单位进行传输，且传输的比特长度必须配置为对应总线宽度的倍数。调用 :cpp:func:`parlio_tx_unit_transmit` 启动 TX 事务，该函数需要接收传输相关的参数，如单元句柄、payload buffer以及 payload 大小（以 **bit** 为单位）。此外，还需要在 :cpp:type:`parlio_transmit_config_t` 中提供专用于该次传输特定的配置。

以下是 :cpp:type:`parlio_transmit_config_t` 结构体的配置参数及其解释：

.. list::

    - :cpp:member:`parlio_transmit_config_t::idle_value` 设置 TX 单元发送完毕后空闲状态时数据线上的值。该值在调用 :cpp:func:`parlio_tx_unit_disable` 禁用 TX 单元后依然会保持。
    :SOC_BITSCRAMBLER_SUPPORTED: - :cpp:member:`parlio_transmit_config_t::bitscrambler_program` 指向比特调节器程序的二进制文件的指针。若此次传输不使用比特调节器，则设置为 ``NULL``。
    - :cpp:member:`parlio_transmit_config_t::flags` 通常用来微调传输的一些行为，包括以下选项
    - :cpp:member:`parlio_transmit_config_t::flags::queue_nonblocking` 设置当传输队列满的时候该函数是否需要等待。如果该值设置为 ``true`` 那么当遇到队列满的时候，该函数会立即返回错误代码 :c:macro:`ESP_ERR_INVALID_STATE`。否则，函数会阻塞当前线程，直到传输队列有空档。
    :SOC_PARLIO_TX_SUPPORT_LOOP_TRANSMISSION: - :cpp:member:`parlio_transmit_config_t::flags::loop_transmission` 设置为 ``true``，会启用无限循环发送机制。此时，除非手动调用 :cpp:func:`parlio_tx_unit_disable`，否则发送不会停止，也不会生成“完成发送”事件。由于循环由 DMA 控制， TX 单元可以在几乎不需要 CPU 干预的情况下，生成周期性序列。

:cpp:func:`parlio_tx_unit_transmit` 会在内部构建一个事务描述符，并将其发送到作业队列中，该队列通常会在 ISR 上下文中被调度。因此，在 :cpp:func:`parlio_tx_unit_transmit` 返回时，该事务可能尚未启动。注意，你不能在事务结束前就去回收或者修改 payload 中的内容。通过 :cpp:func:`parlio_tx_unit_register_event_callbacks` 来注册事件回调，可以在事务完成的时候被通知。为确保完成所有挂起的事务，你还可以调用 :cpp:func:`parlio_tx_unit_wait_all_done`，这样你就得到了一个带阻塞的发送功能。

经过简单的配置，我们就可以以 QPI 的格式发送数据，如下图波形所示：

.. wavedrom:: /../_static/diagrams/parlio/parlio_tx/sim_qpi_waveform.json

注册事件回调
^^^^^^^^^^^^^^^^^^^^^^^^

由于 :cpp:func:`parlio_tx_unit_transmit` 是一个异步接口，我们可能会想知道传输事务什么时候完成。以下代码展示了如何注册传输事务完成的事件回调：

.. code:: c

    static bool test_parlio_tx_done_callback(parlio_tx_unit_handle_t tx_unit, const parlio_tx_done_event_data_t *edata, void *user_ctx)
    {
        // 处理事件回调的一般流程：
        // 1. 从 user_ctx 中拿到用户上下文数据（需事先从 test_parlio_tx_done_callback 中传入）
        // 2. 执行用户自定义操作
        // 3. 返回上述操作期间是否有高优先级的任务被唤醒了，以便通知调度器做切换任务

        BaseType_t high_task_wakeup = pdFalse;
        // 将 FreeRTOS 任务句柄作为用户上下文
        QTaskHandle_t task = (TaskHandle_t)user_ctx;
        // 在传输完成时，我们向指定任务发送任务通知
        vTaskNotifyGiveFromISR(task, &high_task_wakeup);
        // 返回高优先级任务是否已被该函数唤醒
        return (high_task_wakeup == pdTRUE);
    }

    parlio_tx_event_callbacks_t cbs = {
        // 将 test_parlio_tx_done_callback 作为传输事务完成的事件回调函数
        .on_trans_done = test_parlio_tx_done_callback,
    };
    ESP_ERROR_CHECK(parlio_tx_unit_register_event_callbacks(tx_unit, &cbs, xTaskGetCurrentTaskHandle()));

当 TX 单元生成发送完成等事件时，会通过中断告知 CPU。如果需要在发生特定事件时调用函数，可以调用 :cpp:func:`parlio_tx_unit_register_event_callbacks` 向  TX 单元驱动程序的中断服务程序 (ISR) 注册事件回调。由于回调函数是在 ISR 中调用的，因此在回调函数中应该避免执行复杂的操作（包括任何可能导致阻塞的操作），以免影响系统的实时性。:cpp:func:`parlio_tx_unit_register_event_callbacks` 还允许用户传递一个上下文指针，以便在回调函数中访问用户定义的数据。

有关 TX 单元支持的事件回调，请参阅 :cpp:type:`parlio_tx_event_callbacks_t`：

- :cpp:member:`parlio_tx_event_callbacks_t::on_trans_done` 为“发送完成”的事件设置回调函数，函数原型声明为 :cpp:type:`parlio_tx_done_callback_t`。

资源回收
^^^^^^^^

当不再需要使用 TX 单元时，应该调用 :cpp:func:`parlio_del_tx_unit` 函数来释放软硬件资源。删除前请确保 TX 单元已经处于禁用状态。

.. code:: c

    ESP_ERROR_CHECK(parlio_tx_unit_disable(tx_unit));
    ESP_ERROR_CHECK(parlio_del_tx_unit(tx_unit));

进阶功能
--------

在了解了基本用法后，我们可以进一步探索 tx 单元驱动的更多高级玩法。

使用外部时钟作为 TX 单元的时钟源
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

TX 单元可以选择各种不同的时钟源，其中外部时钟源较为特殊。我们通过配置 :cpp:member:`parlio_tx_unit_config_t::clk_src` ， :cpp:member:`parlio_tx_unit_config_t::clk_in_gpio_num` 以及 :cpp:member:`parlio_tx_unit_config_t::input_clk_src_freq_hz` 来启用外部时钟源输入：

.. code-block:: c
   :emphasize-lines: 3,5,6

    parlio_tx_unit_handle_t tx_unit = NULL;
    parlio_tx_unit_config_t config = {
        .clk_src = PARLIO_CLK_SRC_EXTERNAL,         // 选择外部时钟源
        .data_width = 4,                            // 数据宽度为 4 位
        .clk_in_gpio_num = EXAMPLE_PIN_CLK_IN,      // 设置外部时钟源输入引脚
        .input_clk_src_freq_hz = 10 * 1000 * 1000,  // 外部时钟源频率为 10 MHz
        .valid_gpio_num = -1,                       // 不使用有效信号
        .clk_out_gpio_num = EXAMPLE_PIN_CLK_OUT,
        .data_gpio_nums = {
            EXAMPLE_PIN_DATA0,
            EXAMPLE_PIN_DATA1,
            EXAMPLE_PIN_DATA2,
            EXAMPLE_PIN_DATA3,
        },
        .output_clk_freq_hz = 5 * 1000 * 1000, // 输出时钟频率为 5 MHz。注意，不能超过输入时钟频率
        .trans_queue_depth = 32,
        .max_transfer_size = 256,
        .sample_edge = PARLIO_SAMPLE_EDGE_NEG,  // 在时钟下降沿采样数据
    };
    // 创建 TX 单元实例
    ESP_ERROR_CHECK(parlio_new_tx_unit(&config, &tx_unit));
    // 使能 TX 单元
    ESP_ERROR_CHECK(parlio_tx_unit_enable(tx_unit));

   #define PAYLOAD_SIZE 64

    // 配置 TX 单元传输参数
    parlio_transmit_config_t transmit_config = {
        .idle_value = 0x00, // 空闲状态下所有数据线均为低电平
    };

    // 准备需要发送的数据
    uint8_t payload[PAYLOAD_SIZE] = {0};
    for (int i = 0; i < PAYLOAD_SIZE; i++) {
        payload[i] = i;
    }

    // 开始传输事务
    ESP_ERROR_CHECK(parlio_tx_unit_transmit(tx_unit, payload, PAYLOAD_SIZE * sizeof(uint8_t) * 8, &transmit_config));

外部时钟输入的波形如下图所示：

.. wavedrom:: /../_static/diagrams/parlio/parlio_tx/external_clock_input_waveform.json

.. note::
    :cpp:member:`parlio_tx_unit_config_t::input_clk_src_freq_hz` 与 :cpp:member:`parlio_tx_unit_config_t::output_clk_freq_hz` 的比值决定了 TX 单元内部的时钟分频系数。
    当外部时钟实际频率与 :cpp:member:`parlio_tx_unit_config_t::input_clk_src_freq_hz` 不一致时，TX 单元实际生成的输出时钟频率也会相应改变。

.. only:: SOC_PARLIO_TX_SUPPORT_LOOP_TRANSMISSION

    无限循环发送
    ^^^^^^^^^^^^^^^^^^^^^^^^

    {IDF_TARGET_NAME} 支持无限循环发送，即 TX 单元可以在不需要 CPU 干预的情况下，生成周期性序列。通过配置 :cpp:member:`parlio_transmit_config_t::flags::loop_transmission`，我们就可以启用无限循环发送

    .. code-block:: c
       :emphasize-lines: 32

        parlio_tx_unit_handle_t tx_unit = NULL;
        parlio_tx_unit_config_t config = {
            .clk_src = PARLIO_CLK_SRC_DEFAULT,      // 选择默认的时钟源
            .data_width = 4,                        // 数据宽度为 4 位
            .clk_in_gpio_num = -1,                  // 不使用外部时钟源
            .valid_gpio_num = -1,                   // 不使用有效信号
            .clk_out_gpio_num = EXAMPLE_PIN_CLK,
            .data_gpio_nums = {
                EXAMPLE_PIN_DATA0,
                EXAMPLE_PIN_DATA1,
                EXAMPLE_PIN_DATA2,
                EXAMPLE_PIN_DATA3,
            },
            .output_clk_freq_hz = 10 * 1000 * 1000, // 输出时钟频率为 10 MHz
            .trans_queue_depth = 32,
            .max_transfer_size = 256,
            .sample_edge = PARLIO_SAMPLE_EDGE_NEG,  // 在时钟下降沿采样数据
            .flags = {
                .invert_valid_out = true,  // 有效信号默认高电平有效，通过反转，我们用来模拟 QPI 的时序中的片选信号 CS
            }
        };
        // 创建 TX 单元实例
        ESP_ERROR_CHECK(parlio_new_tx_unit(&config, &tx_unit));
        // 使能 TX 单元
        ESP_ERROR_CHECK(parlio_tx_unit_enable(tx_unit));

        #define PAYLOAD_SIZE 64

        // 配置 TX 单元传输参数
        parlio_transmit_config_t transmit_config = {
            .idle_value = 0x00, // 空闲状态下所有数据线均为低电平
            .loop_transmission = true, // 启用无限循环发送
        };

        // 准备需要发送的数据
        uint8_t payload[PAYLOAD_SIZE] = {0};
        for (int i = 0; i < PAYLOAD_SIZE; i++) {
            payload[i] = i;
        }

        // 开始循环传输事务
        ESP_ERROR_CHECK(parlio_tx_unit_transmit(tx_unit, payload, PAYLOAD_SIZE * sizeof(uint8_t) * 8, &transmit_config));

    循环发送波形如下图所示：

    .. wavedrom:: /../_static/diagrams/parlio/parlio_tx/loop_transmission_waveform.json

    此时，除非手动调用 :cpp:func:`parlio_tx_unit_disable`，否则发送不会停止，也不会生成“完成发送”事件。

    .. note::

        如果启用无限循环发送后需要修改发送内容，可以配置 :cpp:member:`parlio_transmit_config_t::flags::loop_transmission` 并再次调用 :cpp:func:`parlio_tx_unit_transmit` 传入新的 payload buffer，驱动会在旧 buffer 完整发送后，切换到新传入的 buffer。因此需要用户自行维护好两块buffer，避免旧 buffer 被提早修改或者回收导致产生数据不连贯的现象。

.. only:: SOC_BITSCRAMBLER_SUPPORTED

    .. _parlio-tx-bitscrambler-decorator:

    配合比特调节器 (BitScrambler) 产生自定义的比特流
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    我们可以通过编写 :doc:`比特调节器 </api-reference/peripherals/bitscrambler>` 汇编代码来控制 DMA 通路上的数据，进而实现一些简单的编码工作。相较于使用 CPU 做编码工作，比特调节器的性能更高，且不会占用 CPU 资源，但是受限于 BitScrambler 有限的指令存储器空间，它无法实现复杂的编码工作。

    编写好比特调节器程序后，通过调用 :cpp:func:`parlio_tx_unit_decorate_bitscrambler` 启用比特调节器。并在 :cpp:member:`parlio_transmit_config_t::bitscrambler_program` 配置本次传输使用比特调节器程序的二进制文件。不同的传输事务可以使用不同的比特调节器程序。该二进制文件必须符合比特调节器的汇编语言规范，并且在运行时会被加载到比特调节器的指令存储器中。如何编写并编译比特调节器程序请参考 :doc:`比特调节器编程指南 </api-reference/peripherals/bitscrambler>`。

    :cpp:func:`parlio_tx_unit_decorate_bitscrambler` 和 :cpp:func:`parlio_tx_unit_undecorate_bitscrambler` 需要成对使用。在删除 TX 单元时，需要先调用 :cpp:func:`parlio_tx_unit_undecorate_bitscrambler` 移除比特调节器。

电源管理
^^^^^^^^^^^^^^^^

当电源管理 :ref:`CONFIG_PM_ENABLE` 被启用的时候，系统在进入睡眠前可能会调整或禁用时钟源，会导致 TX 单元内部的时间基准无法按预期工作。

为了防止这种情况发生， TX 单元驱动内部创建了一个电源管理锁。锁的类型会根据不同的时钟源来设置。驱动程序将在 :cpp:func:`parlio_tx_unit_enable` 中拿锁，并在 :cpp:func:`parlio_tx_unit_disable` 中释放锁。这意味着，无论电源管理策略如何，在这两个函数之间系统不会进入睡眠模式，时钟源也不会被禁用或调整频率，任何 TX 事务都可以保证正常工作。

.. only:: SOC_PARLIO_SUPPORT_SLEEP_RETENTION

    除了关闭时钟源外，系统在进入睡眠模式时还可以关闭 TX 单元的电源以进一步降低功耗。要实现这一点，需要将 :cpp:member:`parlio_tx_unit_config_t::allow_pd` 设置为 ``true``。在系统进入睡眠模式之前，TX 单元的寄存器上下文会被备份到内存中，并在系统唤醒后恢复。请注意，启用此选项虽然可以降低功耗，但会增加内存的使用量。因此，在使用该功能时需要在功耗和内存消耗之间进行权衡。

关于线程安全
^^^^^^^^^^^^^

驱动使用了临界区保证了对寄存器的原子操作。句柄内部的关键成员也受临界区保护。驱动内部的状态机使用了原子指令保证了线程安全，并且使用线程安全的 FreeRTOS 队列来管理发送事务。因此， TX 单元的 API 可以在多线程环境下使用，无需自行加锁。

关于 Cache 安全
^^^^^^^^^^^^^^^^

在文件系统进行 Flash 读写操作时，为了避免 Cache 从 Flash 加载指令和数据时出现错误，系统会暂时禁用 Cache 功能。这会导致 TX 单元的中断处理程序在此期间无法响应，从而使用户的回调函数无法及时执行。如果希望在 Cache 被禁用期间，中断处理程序仍能正常运行，可以启用 :ref:`CONFIG_PARLIO_TX_ISR_CACHE_SAFE` 选项。

.. note::

    请注意，在启用该选项后，所有的中断回调函数及其上下文数据 **必须存放在内部存储空间** 中。因为在 Cache 被禁用时，系统无法从 Flash 中加载数据和指令。

.. only:: SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND or SOC_SPIRAM_XIP_SUPPORTED

    .. note::

        当启用了以下选项时，系统在进行 Flash 读写操作时不会自动禁用 Cache, 因此无需启用 :ref:`CONFIG_PARLIO_TX_ISR_CACHE_SAFE`。

        .. list::
            :SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND: - :ref:`CONFIG_SPI_FLASH_AUTO_SUSPEND`
            :SOC_SPIRAM_XIP_SUPPORTED: - :ref:`CONFIG_SPIRAM_XIP_FROM_PSRAM`

关于性能
^^^^^^^^

为了提升中断处理的实时响应能力， TX 单元驱动提供了 :ref:`CONFIG_PARLIO_TX_ISR_HANDLER_IN_IRAM` 选项。启用该选项后，中断处理程序将被放置在内部 RAM 中运行，从而减少了从 Flash 加载指令时可能出现的缓存丢失带来的延迟。

.. note::

    但是，中断处理程序调用的用户回调函数和用户上下文数据仍然可能位于 Flash 中，缓存缺失的问题还是会存在，这需要用户自己将回调函数和数据放入内部 RAM 中，比如使用 :c:macro:`IRAM_ATTR` 和 :c:macro:`DRAM_ATTR`。

其他 Kconfig 选项
^^^^^^^^^^^^^^^^^

- :ref:`CONFIG_PARLIO_ENABLE_DEBUG_LOG` 选项允许强制启用 TX 单元驱动的所有调试日志，无论全局日志级别设置如何。启用此选项可以帮助开发人员在调试过程中获取更详细的日志信息，从而更容易定位和解决问题。此选项与 RX 单元驱动程序共用。

关于资源消耗
^^^^^^^^^^^^

使用 :doc:`/api-guides/tools/idf-size` 工具可以查看 TX 单元驱动的代码和数据消耗。以下是测试前提条件（以 ESP32-H2 为例）：

- 编译器优化等级设置为 ``-Os``，以确保代码尺寸最小化。
- 默认日志等级设置为 ``ESP_LOG_INFO``，以平衡调试信息和性能。
- 关闭以下驱动优化选项：
    - :ref:`CONFIG_PARLIO_TX_ISR_HANDLER_IN_IRAM` - 中断处理程序不放入 IRAM。
    - :ref:`CONFIG_PARLIO_TX_ISR_CACHE_SAFE` - 不启用 Cache 安全选项。

**注意，以下数据不是精确值，仅供参考，在不同型号的芯片和不同版本的 IDF 上，数据会有所出入。**

+-----------------+------------+-------+------+-------+-------+------------+---------+-------+
| Component Layer | Total Size | DIRAM | .bss | .data | .text | Flash Code | .rodata | .text |
+=================+============+=======+======+=======+=======+============+=========+=======+
| soc             | 92         | 0     | 0    | 0     | 0     | 92         | 0       | 92    |
+-----------------+------------+-------+------+-------+-------+------------+---------+-------+
| hal             | 18         | 0     | 0    | 0     | 0     | 18         | 0       | 18    |
+-----------------+------------+-------+------+-------+-------+------------+---------+-------+
| driver          | 6478       | 12    | 12   | 0     | 0     | 6466       | 586     | 5880  |
+-----------------+------------+-------+------+-------+-------+------------+---------+-------+

此外，每一个 TX 单元句柄会从 heap 中动态申请约 ``800`` 字节的内存（传输队列深度按 4 计算）。如果还使能了 :cpp:member:`parlio_tx_unit_config_t::flags::allow_pd` 选项，那么每个 TX 单元还会在睡眠期间额外消耗约 ``32`` 字节的内存用于保存寄存器上下文。

应用示例
--------

.. list::

    - :example:`peripherals/parlio/parlio_tx/simple_rgb_led_matrix` 演示了如何使用 {IDF_TARGET_NAME} 的 TX 单元驱动支持 HUB75 接口的 RGB LED 矩阵板，并使用 LVGL 库来展示简单的 UI 元素。
    :SOC_PARLIO_TX_SUPPORT_LOOP_TRANSMISSION: - :example:`peripherals/parlio/parlio_tx/advanced_rgb_led_matrix` 演示了如何使用 {IDF_TARGET_NAME} 的 TX 单元的无限循环发送特性支持 HUB75 接口的 RGB LED 矩阵板。相比 simple_rgb_led_matrix 示例，不需要手动执行循环扫描，使用更加灵活。
    :SOC_PARLIO_SUPPORT_SPI_LCD: - :example:`peripherals/lcd/parlio_simulate` 演示了如何使用并行 IO 外设的 TX 单元驱动 SPI 或 I80 接口的屏幕。

API 参考
-------------

.. include-build-file:: inc/parlio_tx.inc
