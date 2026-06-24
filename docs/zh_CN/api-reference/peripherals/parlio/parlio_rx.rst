并行 IO RX 驱动程序
=====================

:link_to_translation:`en:[English]`

本文介绍了 ESP-IDF 中的并行 IO RX 驱动程序的功能，章节目录如下：

.. contents::
    :local:
    :depth: 2

简介
----

并行 IO RX 单元属于通用的并行接口的一部分，以下简称为 RX 单元。支持通过 GDMA 在并行总线上实现从外部设备接收数据，鉴于 IO 数据的灵活性，RX 单元可用作通用接口，连接多种外围设备。该驱动的主要应用场景包括：

- 用于高速数据采集，如摄像头、传感器数据读取
- 作为从机与其他主机进行高速并行通信
- 逻辑分析仪和信号监控应用

快速入门
--------

本节将带你快速了解如何使用 RX 单元驱动。通过简单的示例演示如何使用软帧界定符进行数据接收，展示如何创建 RX 单元并启动它，如何发起接收事务，以及如何注册事件回调函数。一般的使用流程如下：

创建和使能 RX 单元
^^^^^^^^^^^^^^^^^^

首先，我们需要创建一个 RX 单元实例。以下代码展示了如何创建一个 RX 单元实例：

.. code:: c

    parlio_rx_unit_handle_t rx_unit = NULL;
    parlio_rx_unit_config_t config = {
        .clk_src = PARLIO_CLK_SRC_DEFAULT,      // 选择默认的时钟源
        .data_width = 4,                        // 数据宽度为 4 位
        .clk_in_gpio_num = -1,                  // 不使用外部时钟源
        .clk_out_gpio_num = EXAMPLE_PIN_CLK,    // 输出时钟引脚
        .valid_gpio_num = EXAMPLE_PIN_VALID,    // 有效信号引脚
        .data_gpio_nums = {
            EXAMPLE_PIN_DATA0,
            EXAMPLE_PIN_DATA1,
            EXAMPLE_PIN_DATA2,
            EXAMPLE_PIN_DATA3,
            [4 ... (PARLIO_RX_UNIT_MAX_DATA_WIDTH - 1)] = -1,
        },
        .exp_clk_freq_hz = 1 * 1000 * 1000,     // 期望时钟频率为 1 MHz
        .trans_queue_depth = 10,                // 事务队列深度为 10
        .max_recv_size = 1024,                  // 最大接收大小为 1024 字节
    };
    // 创建 RX 单元实例
    ESP_ERROR_CHECK(parlio_new_rx_unit(&config, &rx_unit));
    // 使能 RX 单元，并重置事务队列
    ESP_ERROR_CHECK(parlio_rx_unit_enable(rx_unit, true));

当创建 RX 单元实例时，我们需要通过 :cpp:type:`parlio_rx_unit_config_t` 配置时钟源、数据宽度和期望时钟频率等参数。然后调用 :cpp:func:`parlio_new_rx_unit` 函数创建一个新的 RX 单元实例，该函数将返回一个指向新实例的句柄。实例句柄实际上是一个指向 RX 单元内存对象的指针，类型为 :cpp:type:`parlio_rx_unit_handle_t`。

以下是 :cpp:type:`parlio_rx_unit_config_t` 结构体的配置参数及其解释：

.. list::
    -  :cpp:member:`parlio_rx_unit_config_t::clk_src` 设置 RX 单元的时钟源。可用时钟源列在 :cpp:type:`parlio_clock_source_t` 中，只能选择其中一个。不同的时钟源会在分辨率，精度和功耗上有所不同。
    -  :cpp:member:`parlio_rx_unit_config_t::clk_in_gpio_num` 使用外部时钟作为时钟源，设置相应的时钟输入的 GPIO 编号。否则，设置为 -1，驱动程序将使用内部 :cpp:member:`parlio_rx_unit_config_t::clk_src` 作为时钟源。
    -  :cpp:member:`parlio_rx_unit_config_t::ext_clk_freq_hz` 外部输入时钟源的频率，仅当 :cpp:member:`parlio_rx_unit_config_t::clk_in_gpio_num` 不为 -1 时有效。
    -  :cpp:member:`parlio_rx_unit_config_t::exp_clk_freq_hz` 设定期望的采样/位时钟频率，由内部时钟源或外部时钟源分频得到。
    -  :cpp:member:`parlio_rx_unit_config_t::clk_out_gpio_num` 输出时钟信号的 GPIO 编号（如果支持）。未使用则设置为 -1。
    -  :cpp:member:`parlio_rx_unit_config_t::data_width` RX 单元数据总线宽度，必须为 2 的幂次方，且不能大于 {IDF_TARGET_SOC_PARLIO_RX_UNIT_MAX_DATA_WIDTH}。
    -  :cpp:member:`parlio_rx_unit_config_t::data_gpio_nums` RX 数据 GPIO 编号，未使用的 GPIO 设置为 -1。
    -  :cpp:member:`parlio_rx_unit_config_t::valid_gpio_num` 有效信号的 GPIO 编号，未使用则设置为 -1。有效信号用于指示数据线上的数据是否有效。
    -  :cpp:member:`parlio_rx_unit_config_t::trans_queue_depth` 内部事务队列深度。队列越深，在待处理队列中可以准备的事务越多。
    -  :cpp:member:`parlio_rx_unit_config_t::max_recv_size` 一次事务的最大接收大小（以字节为单位）。这决定了每个事务将使用的 DMA 节点数量。
    -  :cpp:member:`parlio_rx_unit_config_t::flags` 通常用来微调驱动的一些行为，包括以下选项
    -  :cpp:member:`parlio_rx_unit_config_t::flags::free_clk` 输入的外部时钟是否为自由运行时钟。自由运行时钟会一直保持运行（例如 I2S bclk），非自由运行时钟会在有数据传输时启动，总线空闲时停止（例如 SPI）。
    :SOC_PARLIO_RX_CLK_SUPPORT_GATING: -  :cpp:member:`parlio_rx_unit_config_t::flags::clk_gate_en` 启用 RX 时钟门控，输出时钟将由有效 GPIO 控制。
    :SOC_PARLIO_SUPPORT_SLEEP_RETENTION: -  :cpp:member:`parlio_rx_unit_config_t::flags::allow_pd` 设置为允许断电。当设置此标志时，驱动程序将在进入/退出睡眠模式之前/之后备份/恢复 PARLIO 寄存器。

.. note::

    如果当前芯片中所有的 RX 单元都已经被申请使用，那么 :cpp:func:`parlio_new_rx_unit` 函数会返回 :c:macro:`ESP_ERR_NOT_FOUND` 错误。

RX 单元在使用前必须要先使能，使能函数 :cpp:func:`parlio_rx_unit_enable` 可以将驱动的内部状态机切换到激活状态，这里面还会包括一些系统性服务的申请/注册等工作，如申请电源管理锁，重置事务队列。与使能函数相对应的是禁用函数 :cpp:func:`parlio_rx_unit_disable`，它会释放所有的系统性服务。

创建帧界定符
^^^^^^^^^^^^

在发起接收事务之前，我们需要创建帧界定符来定义 RX 单元应该如何界定数据帧的开始和结束。RX 单元支持三种类型的帧界定符：

**电平帧界定符**：使用电平信号对数据进行分帧。

.. code:: c

    parlio_rx_delimiter_handle_t level_delimiter = NULL;
    parlio_rx_level_delimiter_config_t level_config = {
        .valid_sig_line_id = 4,                     // 使用数据线 4 作为有效信号输入
        .sample_edge = PARLIO_SAMPLE_EDGE_POS,      // 正边沿采样
        .bit_pack_order = PARLIO_BIT_PACK_ORDER_MSB, // 从 MSB 开始打包位
        .eof_data_len = 256,                        // 256 字节后触发 EOF 中断，若为 0 则当有效信号失效时触发 EOF 中断
        .timeout_ticks = 1000,                      // 有效信号失效 1000 个时钟周期后， 仍未达到足够 EOF 的数据，触发超时中断，若为 0 则不触发超时中断
        .flags = {
            .active_low_en = false,                 // 高电平有效
        },
    };
    ESP_ERROR_CHECK(parlio_new_rx_level_delimiter(&level_config, &level_delimiter));

**脉冲帧界定符**：使用脉冲信号对数据进行分帧。

.. code:: c

    parlio_rx_delimiter_handle_t pulse_delimiter = NULL;
    parlio_rx_pulse_delimiter_config_t pulse_config = {
        .valid_sig_line_id = 4,                     // 使用数据线 4 作为有效信号输入
        .sample_edge = PARLIO_SAMPLE_EDGE_NEG,      // 负边沿采样
        .bit_pack_order = PARLIO_BIT_PACK_ORDER_MSB, // 从 MSB 开始打包位
        .eof_data_len = 128,                        // 128 字节后触发 EOF 中断，若为 0 则当有效信号失效时触发 EOF 中断
        .timeout_ticks = 500,                       // 有效信号失效 500 个时钟周期后， 仍未达到足够 EOF 的数据，触发超时中断，若为 0 则不触发超时中断
        .flags = {
            .start_bit_included = false,            // 起始位不包含在数据中
            .end_bit_included = false,              // 结束位不包含在数据中
            .has_end_pulse = true,                  // 有结束脉冲来终止
            .pulse_invert = false,                  // 不对输入的脉冲信号做反相
        },
    };
    ESP_ERROR_CHECK(parlio_new_rx_pulse_delimiter(&pulse_config, &pulse_delimiter));

**软帧界定符**：使用软件定义的数据长度对数据进行分帧。

.. code:: c

    parlio_rx_delimiter_handle_t soft_delimiter = NULL;
    parlio_rx_soft_delimiter_config_t soft_config = {
        .sample_edge = PARLIO_SAMPLE_EDGE_POS,      // 正边沿采样
        .bit_pack_order = PARLIO_BIT_PACK_ORDER_MSB, // 从 MSB 开始打包位
        .eof_data_len = 512,                        // 512 字节后结束帧，由于没有其他结束依据，软帧界定符必须设置该字段
        .timeout_ticks = 0,                         // 无超时
    };
    ESP_ERROR_CHECK(parlio_new_rx_soft_delimiter(&soft_config, &soft_delimiter));

发起 RX 接收事务
^^^^^^^^^^^^^^^^

使能 RX 单元并创建帧界定符后，我们就可以配置接收参数并调用 :cpp:func:`parlio_rx_unit_receive` 启动 RX 事务。以下代码展示了如何发起 RX 单元接收事务：

.. code:: c

    #define PAYLOAD_SIZE 512

    // 分配 DMA 兼容缓冲区
    uint8_t *payload = heap_caps_calloc(1, PAYLOAD_SIZE, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT | MALLOC_CAP_DMA);

    // 配置 RX 单元接收参数
    parlio_receive_config_t receive_config = {
        .delimiter = soft_delimiter,        // 使用上面创建的软帧界定符
        .flags = {
            .partial_rx_en = false,         // 禁用部分接收模式
            .indirect_mount = false,        // 直接挂载用户缓冲区到 DMA
        },
    };

    // 启动软帧界定符（仅软帧界定符需要）
    ESP_ERROR_CHECK(parlio_rx_soft_delimiter_start_stop(rx_unit, soft_delimiter, true));

    // 启动接收事务
    ESP_ERROR_CHECK(parlio_rx_unit_receive(rx_unit, payload, PAYLOAD_SIZE, &receive_config));

    // 等待接收事务完成
    ESP_ERROR_CHECK(parlio_rx_unit_wait_all_done(rx_unit, 5000)); // 等待最多 5 秒

    // 停止软帧界定符
    ESP_ERROR_CHECK(parlio_rx_soft_delimiter_start_stop(rx_unit, soft_delimiter, false));

RX 单元以字节为单位接收数据，接收的数据长度取决于帧界定符配置。调用 :cpp:func:`parlio_rx_unit_receive` 启动 RX 事务，该函数需要接收相关的参数，如单元句柄、payload buffer以及 payload 大小（以 **字节** 为单位）。此外，还需要在 :cpp:type:`parlio_receive_config_t` 中提供专用于该次接收的特定配置。

以下是 :cpp:type:`parlio_receive_config_t` 结构体的配置参数及其解释：

.. list::

    - :cpp:member:`parlio_receive_config_t::delimiter` 用于此接收事务的帧界定符。
    - :cpp:member:`parlio_receive_config_t::flags` 通常用来微调接收的一些行为，包括以下选项
    - :cpp:member:`parlio_receive_config_t::flags::partial_rx_en` 如果一次事物很长，可以拆分多次完成数据的接收。
    - :cpp:member:`parlio_receive_config_t::flags::indirect_mount` 启用此标志以使用内部 DMA 缓冲区而不是用户 payload 缓冲区。数据将在每次中断时复制到 payload 中。

:cpp:func:`parlio_rx_unit_receive` 会在内部构建一个事务描述符，并将其发送到作业队列中，该队列通常会在 ISR 上下文中被调度。因此，在 :cpp:func:`parlio_rx_unit_receive` 返回时，该事务可能尚未启动。注意，你不能在事务结束前就去回收或者修改 payload 中的内容。通过 :cpp:func:`parlio_rx_unit_register_event_callbacks` 来注册事件回调，可以在事务完成的时候被通知。为确保完成所有挂起的事务，你还可以调用 :cpp:func:`parlio_rx_unit_wait_all_done`，这样你就得到了一个带阻塞的接收功能。

注册事件回调
^^^^^^^^^^^^

由于 :cpp:func:`parlio_rx_unit_receive` 是一个异步接口，我们可能会想知道接收事务什么时候完成或何时接收到部分数据。以下代码展示了如何注册事件回调：

.. code:: c

    static bool on_partial_receive_callback(parlio_rx_unit_handle_t rx_unit, const parlio_rx_event_data_t *edata, void *user_ctx)
    {
        // 当接收到部分数据时调用（用于无限事务）。可在回调中作简单处理，如队列、任务操作，或将接受完成的数据拷贝到用户 buffer 中
        return false;
    }

    static bool on_receive_done_callback(parlio_rx_unit_handle_t rx_unit, const parlio_rx_event_data_t *edata, void *user_ctx)
    {
        // 当接收事务完成时调用
        BaseType_t high_task_wakeup = pdFalse;
        TaskHandle_t task = (TaskHandle_t)user_ctx;

        // 通知等待的任务
        vTaskNotifyGiveFromISR(task, &high_task_wakeup);
        return (high_task_wakeup == pdTRUE);
    }

    static bool on_timeout_callback(parlio_rx_unit_handle_t rx_unit, const parlio_rx_event_data_t *edata, void *user_ctx)
    {
        // 当接收超时时调用
        return false;
    }

    parlio_rx_event_callbacks_t cbs = {
        .on_partial_receive = on_partial_receive_callback,
        .on_receive_done = on_receive_done_callback,
        .on_timeout = on_timeout_callback,
    };
    ESP_ERROR_CHECK(parlio_rx_unit_register_event_callbacks(rx_unit, &cbs, xTaskGetCurrentTaskHandle()));

当 RX 单元生成接收完成或超时等事件时，会通过中断告知 CPU。如果需要在发生特定事件时调用函数，可以调用 :cpp:func:`parlio_rx_unit_register_event_callbacks` 向 RX 单元驱动程序的中断服务程序 (ISR) 注册事件回调。由于回调函数是在 ISR 中调用的，因此在回调函数中应该避免执行复杂的操作（包括任何可能导致阻塞的操作），以免影响系统的实时性。

有关 RX 单元支持的事件回调，请参阅 :cpp:type:`parlio_rx_event_callbacks_t`：

- :cpp:member:`parlio_rx_event_callbacks_t::on_partial_receive` 为"接收到部分数据"的事件设置回调函数，函数原型声明为 :cpp:type:`parlio_rx_callback_t`。
- :cpp:member:`parlio_rx_event_callbacks_t::on_receive_done` 为"接收完成"的事件设置回调函数，函数原型声明为 :cpp:type:`parlio_rx_callback_t`。
- :cpp:member:`parlio_rx_event_callbacks_t::on_timeout` 为"接收超时"的事件设置回调函数，函数原型声明为 :cpp:type:`parlio_rx_callback_t`。超时时间由如下帧界定符配置决定 :cpp:member:`parlio_rx_level_delimiter_config_t::timeout_ticks`, :cpp:member:`parlio_rx_pulse_delimiter_config_t::timeout_ticks` 或 :cpp:member:`parlio_rx_soft_delimiter_config_t::timeout_ticks`。

资源回收
^^^^^^^^

当不再需要使用 RX 单元时，应该调用 :cpp:func:`parlio_del_rx_unit` 函数来释放软硬件资源。删除前请确保 RX 单元已经处于禁用状态。同时记得删除帧界定符。

.. code:: c

    ESP_ERROR_CHECK(parlio_rx_unit_disable(rx_unit));
    ESP_ERROR_CHECK(parlio_del_rx_unit(rx_unit));
    ESP_ERROR_CHECK(parlio_del_rx_delimiter(soft_delimiter));
    free(payload);

进阶功能
--------

在了解了基本用法后，我们可以进一步探索 RX 单元驱动的更多高级玩法。

使用外部时钟作为 RX 单元的时钟源
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

RX 单元可以选择各种不同的时钟源，其中外部时钟源较为特殊。我们通过配置 :cpp:member:`parlio_rx_unit_config_t::clk_src`，:cpp:member:`parlio_rx_unit_config_t::clk_in_gpio_num` 以及 :cpp:member:`parlio_rx_unit_config_t::ext_clk_freq_hz` 来启用外部时钟源输入：

.. code-block:: c
   :emphasize-lines: 3,5,6

    parlio_rx_unit_handle_t rx_unit = NULL;
    parlio_rx_unit_config_t config = {
        .clk_src = PARLIO_CLK_SRC_EXTERNAL,         // 选择外部时钟源
        .data_width = 4,                            // 数据宽度为 4 位
        .clk_in_gpio_num = EXAMPLE_PIN_CLK_IN,      // 设置外部时钟源输入引脚
        .ext_clk_freq_hz = 10 * 1000 * 1000,       // 外部时钟源频率为 10 MHz
        .exp_clk_freq_hz = 10 * 1000 * 1000,       // 期望时钟频率匹配外部时钟
        .valid_gpio_num = EXAMPLE_PIN_VALID,        // 有效信号引脚
        .data_gpio_nums = {
            EXAMPLE_PIN_DATA0,
            EXAMPLE_PIN_DATA1,
            EXAMPLE_PIN_DATA2,
            EXAMPLE_PIN_DATA3,
            [4 ... (PARLIO_RX_UNIT_MAX_DATA_WIDTH - 1)] = -1,
        },
        .trans_queue_depth = 10,
        .max_recv_size = 1024,
        .flags = {
            .free_clk = true,                       // 外部时钟为自由运行
        },
    };
    // 创建 RX 单元实例
    ESP_ERROR_CHECK(parlio_new_rx_unit(&config, &rx_unit));
    // 使能 RX 单元
    ESP_ERROR_CHECK(parlio_rx_unit_enable(rx_unit, true));

.. note::
    使用外部时钟源时，请确保 :cpp:member:`parlio_rx_unit_config_t::ext_clk_freq_hz` 与外部时钟的实际频率匹配，以确保正常运行。

无限接收事务
^^^^^^^^^^^^

RX 单元支持无限接收事务，可以以流的方式连续接收数据。这对于逻辑分析仪或连续数据监控等应用很有用：

.. code:: c

    // 配置无限接收事务
    parlio_receive_config_t receive_config = {
        .delimiter = soft_delimiter,
        .flags = {
            .partial_rx_en = true,          // 启用无限/部分接收模式
            .indirect_mount = true,         // 使用内部缓冲区避免数据损坏
        },
    };

    // 启动软帧界定符
    ESP_ERROR_CHECK(parlio_rx_soft_delimiter_start_stop(rx_unit, soft_delimiter, true));

    // 启动无限接收事务
    ESP_ERROR_CHECK(parlio_rx_unit_receive(rx_unit, payload, PAYLOAD_SIZE, &receive_config));

    // 事务将无限期继续，随着数据接收会触发部分接收回调
    // 需要时使用 parlio_rx_soft_delimiter_start_stop 停止事务

    vTaskDelay(pdMS_TO_TICKS(5000)); // 让它运行 5 秒

    // 停止无限事务
    ESP_ERROR_CHECK(parlio_rx_soft_delimiter_start_stop(rx_unit, soft_delimiter, false));

在无限接收模式下，每次内部缓冲区被填满时都会触发 :cpp:member:`parlio_rx_event_callbacks_t::on_partial_receive` 回调，如果启用了 :cpp:member:`parlio_receive_config_t::flags::indirect_mount`，数据将被复制到用户缓冲区。

ISR 上下文接收
^^^^^^^^^^^^^^

对于需要极低延迟的应用，RX 单元驱动程序提供了 :cpp:func:`parlio_rx_unit_receive_from_isr`，可以从 ISR 上下文中调用，例如在事件回调中：

.. code:: c

    static bool on_receive_done_isr_callback(parlio_rx_unit_handle_t rx_unit, const parlio_rx_event_data_t *edata, void *user_ctx)
    {
        // 从 ISR 上下文立即排队另一个接收事务
        parlio_receive_config_t *config = (parlio_receive_config_t *)user_ctx;
        uint8_t *next_buffer = get_next_buffer(); // 用户定义的函数

        bool hp_task_woken = false;
        esp_err_t ret = parlio_rx_unit_receive_from_isr(rx_unit, next_buffer, BUFFER_SIZE, config, &hp_task_woken);
        if (ret != ESP_OK) {
            // 处理错误
        }

        return hp_task_woken;
    }

电源管理
^^^^^^^^

当电源管理 :ref:`CONFIG_PM_ENABLE` 被启用的时候，系统在进入睡眠前可能会调整或禁用时钟源，会导致 RX 单元内部的时间基准无法按预期工作。

为了防止这种情况发生，RX 单元驱动内部创建了一个电源管理锁。锁的类型会根据不同的时钟源来设置。驱动程序将在 :cpp:func:`parlio_rx_unit_enable` 中拿锁，并在 :cpp:func:`parlio_rx_unit_disable` 中释放锁。这意味着，无论电源管理策略如何，在这两个函数之间系统不会进入睡眠模式，时钟源也不会被禁用或调整频率，任何 RX 事务都可以保证正常工作。

.. only:: SOC_PARLIO_SUPPORT_SLEEP_RETENTION

    除了关闭时钟源外，系统在进入睡眠模式时还可以关闭 RX 单元的电源以进一步降低功耗。要实现这一点，需要将 :cpp:member:`parlio_rx_unit_config_t::flags::allow_pd` 设置为 ``true``。在系统进入睡眠模式之前，RX 单元的寄存器上下文会被备份到内存中，并在系统唤醒后恢复。请注意，启用此选项虽然可以降低功耗，但会增加内存的使用量。

关于线程安全
^^^^^^^^^^^^

驱动使用了临界区保证了对寄存器的原子操作。句柄内部的关键成员也受临界区保护。驱动内部的状态机使用了原子指令保证了线程安全，并且使用线程安全的 FreeRTOS 队列来管理接收事务。因此，RX 单元的 API 可以在多线程环境下使用，无需自行加锁。

关于 Cache 安全
^^^^^^^^^^^^^^^

在文件系统进行 Flash 读写操作时，为了避免 Cache 从 Flash 加载指令和数据时出现错误，系统会暂时禁用 Cache 功能。这会导致 RX 单元的中断处理程序在此期间无法响应，从而使用户的回调函数无法及时执行。如果希望在 Cache 被禁用期间，中断处理程序仍能正常运行，可以启用 :ref:`CONFIG_PARLIO_RX_ISR_CACHE_SAFE` 选项。

.. note::

    请注意，在启用该选项后，所有的中断回调函数及其上下文数据 **必须存放在内部存储空间** 中。因为在 Cache 被禁用时，系统无法从 Flash 中加载数据和指令。

.. only:: SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND or SOC_SPIRAM_XIP_SUPPORTED

    .. note::

        当启用了以下选项时，系统在进行 Flash 读写操作时不会自动禁用 Cache, 因此无需启用 :ref:`CONFIG_PARLIO_RX_ISR_CACHE_SAFE`。

        .. list::
            :SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND: - :ref:`CONFIG_SPI_FLASH_AUTO_SUSPEND`
            :SOC_SPIRAM_XIP_SUPPORTED: - :ref:`CONFIG_SPIRAM_XIP_FROM_PSRAM`

关于性能
^^^^^^^^

为了提升中断处理的实时响应能力，RX 单元驱动提供了 :ref:`CONFIG_PARLIO_RX_ISR_HANDLER_IN_IRAM` 选项。启用该选项后，中断处理程序将被放置在内部 RAM 中运行，从而减少了从 Flash 加载指令时可能出现的缓存丢失带来的延迟。

.. note::

    但是，中断处理程序调用的用户回调函数和用户上下文数据仍然可能位于 Flash 中，缓存缺失的问题还是会存在，这需要用户自己将回调函数和数据放入内部 RAM 中，比如使用 :c:macro:`IRAM_ATTR` 和 :c:macro:`DRAM_ATTR`。

另外请注意，由于 :cpp:member:`parlio_receive_config_t::flags::indirect_mount` 选项会使用内部分配的 DMA 缓冲区，而不是用户 payload 缓冲区。数据会在中断时复制到 payload 中。因此，使用该选项时，会略微降低数据吞吐效率。

其他 Kconfig 选项
^^^^^^^^^^^^^^^^^

- :ref:`CONFIG_PARLIO_ENABLE_DEBUG_LOG` 选项允许强制启用 RX 单元驱动的所有调试日志，无论全局日志级别设置如何。启用此选项可以帮助开发人员在调试过程中获取更详细的日志信息，从而更容易定位和解决问题。此选项与 TX 单元驱动程序共用。

关于资源消耗
^^^^^^^^^^^^

使用 :doc:`/api-guides/tools/idf-size` 工具可以查看 RX 单元驱动的代码和数据消耗。以下是测试前提条件（以 ESP32-H2 为例）：

- 编译器优化等级设置为 ``-Os``，以确保代码尺寸最小化。
- 默认日志等级设置为 ``ESP_LOG_INFO``，以平衡调试信息和性能。
- 关闭以下驱动优化选项：
    - :ref:`CONFIG_PARLIO_RX_ISR_HANDLER_IN_IRAM` - 中断处理程序不放入 IRAM。
    - :ref:`CONFIG_PARLIO_RX_ISR_CACHE_SAFE` - 不启用 Cache 安全选项。

**注意，以下数据不是精确值，仅供参考，在不同型号的芯片和不同版本的 IDF 上，数据会有所出入。**

+-----------------+------------+-------+------+-------+-------+------------+---------+-------+
| Component Layer | Total Size | DIRAM | .bss | .data | .text | Flash Code | .rodata | .text |
+=================+============+=======+======+=======+=======+============+=========+=======+
| soc             | 100        | 0     | 0    | 0     | 0     | 100        | 0       | 100   |
+-----------------+------------+-------+------+-------+-------+------------+---------+-------+
| hal             | 18         | 0     | 0    | 0     | 0     | 18         | 0       | 18    |
+-----------------+------------+-------+------+-------+-------+------------+---------+-------+
| driver          | 9666       | 0     | 0    | 0     | 0     | 9666       | 618     | 9048  |
+-----------------+------------+-------+------+-------+-------+------------+---------+-------+

此外，每一个 RX 单元句柄会从 heap 中动态申请约 ``700`` 字节的内存（事务队列深度按 10 计算）。如果还使能了 :cpp:member:`parlio_rx_unit_config_t::flags::allow_pd` 选项，那么每个 RX 单元还会在睡眠期间额外消耗约 ``32`` 字节的内存用于保存寄存器上下文。

应用示例
--------

* :example:`peripherals/parlio/parlio_rx/logic_analyzer` 演示了如何使用并行 IO RX 外设来实现逻辑分析仪。该分析仪可以以高频率并行采样多个 GPIO 上的数据，还可以探测内部或外部信号，并将原始采样数据保存至 Flash 中或者输出到 TCP 流。

API 参考
--------

.. include-build-file:: inc/parlio_rx.inc
