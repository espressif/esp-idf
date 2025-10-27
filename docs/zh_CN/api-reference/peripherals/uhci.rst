UART DMA (UHCI)
===============

:link_to_translation:`en:[English]`

本文档描述了 ESP-IDF 中 UART DMA（UHCI）驱动的功能。目录如下：

.. contents::
    :local:
    :depth: 2

概述
------------

本文档将介绍如何将 UART 与 DMA 结合使用，以在高波特率下传输或接收大数据量。 {IDF_TARGET_NAME} 的 {IDF_TARGET_SOC_UART_HP_NUM} 个 UART 控制器通过主机控制接口（HCI）共享一组 DMA TX/RX 通道。在以下文档中，UHCI 是指控制 UART DMA 的实体。

.. note::

    UART DMA 与 BT 共享 HCI 硬件，因此请勿同时使用 BT HCI 和 UART DMA，哪怕它们使用的是不同的 UART 端口。

快速入门
-----------

本节将快速指导您如何使用 UHCI 驱动。通过一个简单的传输和接收示例，展示了如何创建和启动 UHCI、启动传输和接收事务以及注册事件回调函数。一般使用流程如下：

创建并启用 UHCI 控制器
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

UHCI 控制器需要通过 :cpp:type:`uhci_controller_config_t` 进行配置。

如果在 :cpp:type:`uhci_controller_config_t` 完成了配置，用户可以调用 :cpp:func:`uhci_new_controller` 来分配并初始化一个 UHCI 控制器。此函数如果运行正常，将返回一个 UHCI 控制器句柄。此外，UHCI 必须与已初始化的 UART 驱动程序一起工作。以下代码可供参考。

.. code:: c

    #define EX_UART_NUM 1       // 定义 UART 端口

    // 关于 UART 端口配置项，请参考 UART 编程指南
    // 请注意波特率有可能受限于串口芯片
    uart_config_t uart_config = {
        .baud_rate = 1 * 1000 * 1000,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    // UART 参数配置
    ESP_ERROR_CHECK(uart_param_config(EX_UART_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(EX_UART_NUM, UART_TX_IO, UART_RX_IO, -1, -1));

    uhci_controller_config_t uhci_cfg = {
        .uart_port = EX_UART_NUM,                 // 将指定 UART 端口连接到 UHCI 硬件
        .tx_trans_queue_depth = 30,               // 发送队列的队列深度
        .max_receive_internal_mem = 10 * 1024,    // 内部接收内存大小，更多信息请参考 API 注释。
        .max_transmit_size = 10 * 1024,           // 单次传输的最大传输量，单位是字节
        .dma_burst_size = 32,                     // 突发传输大小
        .rx_eof_flags.idle_eof = 1,               // 结束帧的条件，用户可以选择 `idle_eof`, `rx_brk_eof` 和 `length_eof`, 关于更多信息请参考 API 注释.
    };

    uhci_controller_handle_t uhci_ctrl;

    ESP_ERROR_CHECK(uhci_new_controller(&uhci_cfg, &uhci_ctrl));

注册事件回调
^^^^^^^^^^^^^^^^^^^^^^^^

当 UHCI 控制器上发生事件（例如传输或接收完成）时，CPU通过中断被通知此事件。如果在某些事件发生时需要调用特定函数，可以通过调用 :cpp:func:`uhci_register_event_callbacks` 为 TX 和 RX 方向分别注册回调。 由于注册的回调函数在中断上下文中调用，用户应确保回调函数不会阻塞，例如仅调用带有 `FromISR` 后缀的 FreeRTOS API。回调函数具有布尔返回值，指示回调是否解除了更高优先级任务的阻塞状态。

UHCI 事件回调在 :cpp:type:`uhci_event_callbacks_t` 中列出：

- :cpp:member:`uhci_event_callbacks_t::on_tx_trans_done` 为“传输完成”事件设置回调函数。函数原型声明为 :cpp:type:`uhci_tx_done_callback_t`。

- :cpp:member:`uhci_event_callbacks_t::on_rx_trans_event` 为“接收事件”设置回调函数。函数原型声明为 :cpp:type:`uhci_rx_event_callback_t`。

.. note::

    “rx-trans-event” 事件并不等同于“接收完成”。在一次接收事务中，该回调函数也可能在“部分接收”时被多次调用，此时可以通过 :cpp:member:`uhci_rx_event_data_t::flags::totally_received` 标志区分“部分接收”和“接收完成”。

用户还可以通过 :cpp:func:`uhci_register_event_callbacks` 中的参数 `user_data` 保存自己的上下文。用户数据会直接传递给每个回调函数。

在回调函数中，用户可以获取由驱动填充的事件特定数据，该数据保存在 ``edata`` 中。注意， ``edata`` 指针仅在回调期间有效，请勿尝试保存该指针并在回调函数外部使用。

TX 事件数据在 :cpp:type:`uhci_tx_done_event_data_t` 中定义：

- :cpp:member:`uhci_tx_done_event_data_t::buffer` 表示 ``buffer`` 已经发送完成。

RX 事件数据在 :cpp:type:`uhci_rx_event_data_t` 中定义：

- :cpp:member:`uhci_rx_event_data_t::data` 指向接收到的数据。数据保存在 :cpp:func:`uhci_receive` 函数的 ``buffer`` 参数中。用户在回调返回之前不应释放此接收缓冲区。
- :cpp:member:`uhci_rx_event_data_t::recv_size` 表示接收到的数据大小。此值不会大于 :cpp:func:`uhci_receive` 函数的 ``buffer_size`` 参数。
- :cpp:member:`uhci_rx_event_data_t::flags::totally_received` 指示当前接收缓冲区是否是事务中的最后一个。

启动 UHCI 传输
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

:cpp:func:`uhci_transmit` 是一个非阻塞函数，这意味着在调用后会立即返回。您可以通过 :cpp:member:`uhci_event_callbacks_t::on_tx_trans_done` 相关回调指示事务完成。我们还提供了一个函数 :cpp:func:`uhci_wait_all_tx_transaction_done` 来阻塞线程，等待所有事务完成。

以下代码显示了如何通过 UHCI 接收数据：

.. code:: c

    uint8_t data_wr[DATA_LENGTH];
    for (int i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i;
    }
    ESP_ERROR_CHECK(uhci_transmit(uhci_ctrl, data_wr, DATA_LENGTH));
    // 等待所有传输完成
    ESP_ERROR_CHECK(uhci_wait_all_tx_transaction_done(uhci_ctrl, -1));

启动 UHCI 接收
^^^^^^^^^^^^^^^^^^^^^^^^^

:cpp:func:`uhci_receive` 是一个非阻塞函数，这意味着该函数在调用后会立即返回。用户可以通过 :cpp:member:`uhci_rx_event_data_t::recv_size` 获取相关的回调，以指示接收事件并判断事务是否完成。

以下代码展示了如何通过 UHCI 传输数据：

.. code:: c

    // 全局变量：队列的句柄
    QueueHandle_t uhci_queue;

    IRAM_ATTR static bool s_uhci_rx_event_cbs(uhci_controller_handle_t uhci_ctrl, const uhci_rx_event_data_t *edata, void *user_ctx)
    {
        // 参数 `user_ctx` 是由函数 `uhci_register_event_callbacks` 的第三个参数传递的。
        uhci_context_t *ctx = (uhci_context_t *)user_ctx;
        BaseType_t xTaskWoken = 0;
        uhci_event_t evt = 0;
        if (edata->flags.totally_received) {
            evt = UHCI_EVT_EOF;
            ctx->receive_size += edata->recv_size;
            memcpy(ctx->p_receive_data, edata->data, edata->recv_size);
        } else {
            evt = UHCI_EVT_PARTIAL_DATA;
            ctx->receive_size += edata->recv_size;
            memcpy(ctx->p_receive_data, edata->data, edata->recv_size);
            ctx->p_receive_data += edata->recv_size;
        }

        xQueueSendFromISR(uhci_queue, &evt, &xTaskWoken);
        return xTaskWoken;
    }

    // 在任务中
    uhci_event_callbacks_t uhci_cbs = {
        .on_rx_trans_event = s_uhci_rx_event_cbs,
    };

    // 注册回调，并开始启动回收
    ESP_ERROR_CHECK(uhci_register_event_callbacks(uhci_ctrl, &uhci_cbs, ctx));
    ESP_ERROR_CHECK(uhci_receive(uhci_ctrl, pdata, 100));

    uhci_event_t evt;
    while (1) {
        // 一个在任务中的队列用来接收 UHCI 抛出的事件
        if (xQueueReceive(uhci_queue, &evt, portMAX_DELAY) == pdTRUE) {
            if (evt == UHCI_EVT_EOF) {
                printf("Received size: %d\n", ctx->receive_size);
                break;
            }
        }
    }

在 API :cpp:func:`uhci_receive` 接口中，参数 ``read_buffer`` 是用户必须提供的缓冲区，参数 ``buffer_size`` 表示用户提供的缓冲区大小。在 UHCI 控制器的配置结构中，参数 :cpp:member:`uhci_controller_config_t::max_receive_internal_mem` 指定了内部 DMA 工作空间的期望大小。软件将根据此工作空间大小分配一定数量的 DMA 节点，这些节点形成一个循环链表。

当一个节点被填满，但接收尚未完成时，将触发 :cpp:member:`uhci_event_callbacks_t::on_rx_trans_event` 事件，且 :cpp:member:`uhci_rx_event_data_t::flags::totally_received` 的值为 0。 当所有数据接收完成时，该事件将再次被触发，并且 :cpp:member:`uhci_rx_event_data_t::flags::totally_received` 的值为 1。

此机制允许用户使用相对较小的缓冲区实现连续且快速的接收，而无需分配与接收总数据量相等大小的缓冲区。

.. note::

    在接收完成之前，:cpp:func:`uhci_receive` 的参数 ``read_buffer`` 不可被释放。

卸载 UHCI 控制器
^^^^^^^^^^^^^^^^^^^^^^^^^

如果不再需要已安装的 UHCI 控制器，建议通过调用 :cpp:func:`uhci_del_controller` 回收资源，以释放底层硬件。

.. code:: c

    ESP_ERROR_CHECK(uhci_del_controller(uhci_ctrl));

高级功能
-----------------

在理解了基本用法后，我们可以进一步探索 UHCI 驱动的高级功能。

关于低功耗
^^^^^^^^^^^^^^^^

当启用电源管理时（即开启 :ref:`CONFIG_PM_ENABLE`），系统在进入睡眠前可能会调整或禁用时钟源。因此，UHCI 内部的 FIFO 可能无法正常工作。

通过创建电源管理锁，驱动程序可以避免上述问题. 驱动会根据不同的时钟源设置锁的类型. 驱动程序将在 :cpp:func:`uhci_receive` 或 :cpp:func:`uhci_transmit` 中获取锁，并在事务完成中断中释放锁。这意味着，这两个函数之间的任何 UHCI 事务都能保证正常稳定运行。

缓存安全
^^^^^^^^^^

默认情况下，当由于写入或擦除主 Flash 导致缓存被禁用时，UHCI 所依赖的中断会被延迟. 因此，事务完成中断可能无法及时处理，这在实时应用中是不可接受的。更糟糕的是，当 UHCI 事务依赖 **乒乓** 中断来连续编码或复制 UHCI 缓冲区时，延迟的中断可能会导致不可预测的结果。

通过启用 Kconfig 选项 :ref:`CONFIG_UHCI_ISR_CACHE_SAFE`，可实现以下功能：

1. 即使缓存被禁用，中断也能被服务。
2. 将 ISR 使用的所有函数放入 IRAM [1]_
3. 将驱动对象放入 DRAM，防止其意外映射到 PSRAM。

此选项允许中断处理程序在缓存禁用时运行，但代价是增加了 IRAM 的消耗。

资源消耗
^^^^^^^^^^^^^^^^^^^^

使用 :doc:`/api-guides/tools/idf-size` 工具检查 UHCI 驱动的代码和数据消耗。以下是基于 ESP32-C3 的测试结果（仅供参考，不同芯片型号可能会有所不同）:

**请注意以下数据仅供参考，不同芯片型号可能会有所不同.**

启用 :ref:`CONFIG_UHCI_ISR_CACHE_SAFE` 时的资源消耗：

.. list-table:: 资源消耗
    :widths: 10 10 10 10 10 10 10 10 10
    :header-rows: 1

    * - Component Layer
      - Total Size
      - DIRAM
      - .bss
      - .data
      - .text
      - Flash Code
      - Flash Data
      - .rodata
    * - UHCI
      - 5733
      - 680
      - 8
      - 34
      - 638
      - 4878
      - 175
      - 175

禁用 :ref:`CONFIG_UHCI_ISR_CACHE_SAFE` 时的资源消耗：

.. list-table:: 资源消耗
    :widths: 10 10 10 10 10 10 10 10 10 10
    :header-rows: 1

    * - Component Layer
      - Total Size
      - DIRAM
      - .bss
      - .data
      - .text
      - Flash Code
      - .text
      - Flash Data
      - .rodata
    * - UHCI
      - 5479
      - 42
      - 8
      - 34
      - 0
      - 5262
      - 5262
      - 175
      - 175

关于性能
^^^^^^^^

为了提升中断处理的实时响应能力， UHCI 驱动提供了 :ref:`CONFIG_UHCI_ISR_HANDLER_IN_IRAM` 选项。启用该选项后，中断处理程序将被放置在内部 RAM 中运行，从而减少了从 Flash 加载指令时可能出现的缓存丢失带来的延迟。

.. note::

    但是，中断处理程序调用的用户回调函数和用户上下文数据仍然可能位于 Flash 中，缓存缺失的问题还是会存在，这需要用户自己将回调函数和数据放入内部 RAM 中，比如使用 :c:macro:`IRAM_ATTR` 和 :c:macro:`DRAM_ATTR`。

线程安全
^^^^^^^^^^^^^

驱动程序保证工厂函数 :cpp:func:`uhci_new_controller`、:cpp:func:`uhci_register_event_callbacks` 和 :cpp:func:`uhci_del_controller` 的线程安全。这意味着用户可以从不同的 RTOS 任务中调用它们，而无需额外的锁保护。

其他 Kconfig 选项
^^^^^^^^^^^^^^^^^^^^^

- :ref:`CONFIG_UHCI_ENABLE_DEBUG_LOG` 选项允许强制启用 UHCI 驱动的所有调试日志，无论全局日志级别设置如何。启用此选项可以帮助开发人员在调试过程中获取更详细的日志信息，从而更容易定位和解决问题，但会增加固件二进制文件的大小。

应用示例
--------------------

- :example:`peripherals/uart/uart_dma_ota` 演示了如何使用 UART DMA 以 1Mbps 波特率快速 OTA 更新芯片固件。

API 参考
-------------

.. include-build-file:: inc/uhci.inc
.. include-build-file:: inc/components/esp_driver_uart/include/driver/uhci_types.inc
.. include-build-file:: inc/components/hal/include/hal/uhci_types.inc

.. [1]
   回调函数（例如 :cpp:member:`uhci_event_callbacks_t::on_tx_trans_done` 、:cpp:member:`uhci_event_callbacks_t::on_rx_trans_event`）及其调用的函数也应驻留在 IRAM 中，用户需要自行注意这一点。
