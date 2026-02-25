====================
异步 CRC (Async CRC)
====================

:link_to_translation:`en:[English]`

本文档介绍了 ESP-IDF 中异步 CRC (Async CRC) 驱动程序的功能。目录如下：

.. contents::
    :local:
    :depth: 2

概述
====

异步 CRC 驱动程序提供使用通用 DMA 外设的硬件加速 CRC 计算。它支持 AHB-GDMA 和 AXI-GDMA 后端，提供灵活的 CRC 计算，支持可配置的多项式、初始值、位反转选项和最终 XOR 处理。

主要功能包括：

- 使用通用 DMA 进行硬件加速的 CRC 计算
- 支持 8 位、16 位和 32 位 CRC 算法
- 带回调通知的异步 API
- 支持超时的阻塞 API
- 支持可配置积压大小的请求队列
- 支持 AHB 和 AXI DMA 后端

应用场景包括：

- 通信协议的数据完整性验证
- 文件和固件校验和计算
- 网络数据包验证
- 存储数据验证

快速开始
========

本节简要介绍如何使用异步 CRC 驱动程序。通过实际示例，演示如何初始化驱动程序、配置 CRC 参数以及执行异步和阻塞 CRC 计算。

异步 CRC 的典型使用流程如下：

.. blockdiag::
    :scale: 100%
    :caption: 异步 CRC 驱动程序的一般使用流程（点击放大）
    :align: center

    blockdiag {
        default_fontsize = 14;
        node_width = 250;
        node_height = 80;
        class emphasis [color = pink, style = dashed];

        install [label="esp_async_crc_install_gdma_*"];
        calc [label="esp_async_crc_calc"];
        wait [label="等待回调"];
        process [label="处理结果"];
        uninstall [label="esp_async_crc_uninstall"];

        install -> calc -> wait -> process;
        process -> calc [folded];
        calc -> uninstall [folded];
    }

创建和安装驱动程序
------------------

首先，您需要安装异步 CRC 驱动程序。该驱动程序根据芯片的功能支持 AHB-GDMA 和 AXI-GDMA 后端：

.. code:: c

    async_crc_handle_t crc_hdl = NULL;
    async_crc_config_t config = {
        .backlog = 8,          // 队列中最大挂起请求数
        .dma_burst_size = 16,  // DMA 突发传输大小（字节）
    };

    // 使用 AHB-GDMA 后端安装（如果可用）
    ESP_ERROR_CHECK(esp_async_crc_install_gdma_ahb(&config, &crc_hdl));

    // 或使用 AXI-GDMA 后端安装（如果可用）
    // ESP_ERROR_CHECK(esp_async_crc_install_gdma_axi(&config, &crc_hdl));

.. note::

    **选择 AHB-GDMA 和 AXI-GDMA 后端**

    后端选择取决于芯片的功能和性能要求：

    - **AHB-GDMA**：大多数 ESP 芯片均可使用。连接到 AHB 总线，适用于通用 DMA 操作。最适合：

      - 标准性能要求
      - 大多数 ESP 芯片变体的兼容性

    - **AXI-GDMA**：在支持 AXI 总线的高端 ESP 芯片上可用。为内存密集型操作提供更高的带宽和更好的性能。最适合：

      - 高吞吐量 CRC 计算
      - 处理大量数据
      - 需要最大性能的应用
      - 更高效地访问外部存储器（PSRAM）

创建驱动程序实例时，您需要配置：

- **backlog**：可排队等待的最大 CRC 请求数。较高的值使用更多内存，但在突发工作负载下提供更好的吞吐量。
- **dma_burst_size**：DMA 传输突发大小（字节）。

驱动程序句柄 ``crc_hdl`` 是一个不透明指针，用于所有后续操作。

执行异步 CRC 计算
-----------------

异步 API 允许您在不阻塞的情况下排队 CRC 计算：

.. code:: c

    static bool crc_complete_callback(async_crc_handle_t crc_hdl, async_crc_event_data_t *edata, void *cb_args)
    {
        uint32_t result = edata->crc_result;
        // 进一步处理 CRC 结果
        // 例如，发送到任务队列，记录日志等。
        return false;
    }

    // 配置 CRC-32 的 CRC 参数
    async_crc_params_t params = {
        .width = 32,
        .polynomial = 0x04C11DB7,
        .init_value = 0xFFFFFFFF,
        .final_xor_value = 0xFFFFFFFF,
        .reverse_input = true,
        .reverse_output = true,
    };

    // 启动异步 CRC 计算
    const char *data = "Hello, World!";
    size_t data_len = strlen(data);

    ESP_ERROR_CHECK(esp_async_crc_calc(crc_hdl, data, data_len, &params, crc_complete_callback, NULL));

CRC 计算完成时会在中断上下文中调用回调函数。回调接收：

- **crc_hdl**：驱动程序句柄
- **edata**：包含 CRC 结果的事件数据
- **cb_args**：在 ``esp_async_crc_calc`` 期间传递的用户定义参数

执行阻塞 CRC 计算
-----------------

对于更简单的使用场景或不需要异步操作的情况，请使用阻塞 API：

.. code:: c

    uint32_t crc_result = 0;
    async_crc_params_t params = {
        .width = 32,
        .polynomial = 0x04C11DB7,
        .init_value = 0xFFFFFFFF,
        .final_xor_value = 0xFFFFFFFF,
        .reverse_input = true,
        .reverse_output = true,
    };

    const char *data = "Hello, World!";
    size_t data_len = strlen(data);

    // 阻塞 CRC，1000ms 超时
    ESP_ERROR_CHECK(esp_crc_calc_blocking(crc_hdl, data, data_len, &params, 1000, &crc_result));

    printf("CRC 结果: 0x%08X\n", crc_result);

阻塞 API 支持：

- **timeout_ms >= 0**：等待指定的毫秒数
- **timeout_ms < 0**：无限期等待

卸载驱动程序
------------

当不再需要驱动程序时：

.. code:: c

    ESP_ERROR_CHECK(esp_async_crc_uninstall(crc_hdl));

如果存在挂起的操作或 CRC 引擎正忙，卸载函数将返回 :c:macro:`ESP_ERR_INVALID_STATE`。请在卸载前确保所有操作已完成。

CRC 参数配置
============

异步 CRC 驱动程序通过 :cpp:type:`async_crc_params_t` 结构支持灵活的 CRC 算法配置。

CRC 宽度
---------

:cpp:member:`async_crc_params_t::width` 字段指定 CRC 位宽度：

- **8**：8 位 CRC（如 CRC-8、CRC-8/MAXIM）
- **16**：16 位 CRC（如 CRC-16/CCITT、CRC-16/IBM）
- **32**：32 位 CRC（如 CRC-32、CRC-32/BZIP2）

多项式
--------

:cpp:member:`async_crc_params_t::polynomial` 字段以十六进制格式指定 CRC 多项式。常见的多项式值包括：

- CRC-32: ``0x04C11DB7``
- CRC-16/CCITT: ``0x1021``
- CRC-16/IBM: ``0x8005``
- CRC-8/MAXIM: ``0x31``

初始值
---------

:cpp:member:`async_crc_params_t::init_value` 字段设置处理前的初始 CRC 值。常见的初始值：

- ``0xFFFFFFFF`` 用于 CRC-32
- ``0x0000`` 用于许多 CRC-16 变体
- ``0x00`` 用于许多 CRC-8 变体

最终 XOR 值
-----------

:cpp:member:`async_crc_params_t::final_xor_value` 字段指定在与最终 CRC 结果进行异或之前的值。这通常是 CRC-32 的 ``0xFFFFFFFF``，但对于某些变体可以是 ``0x0000``。

位反转选项
-----------

- :cpp:member:`async_crc_params_t::reverse_input` 如果为 true，在处理前反转每个输入字节的位顺序
- :cpp:member:`async_crc_params_t::reverse_output` 如果为 true，在应用最终 XOR 之前反转最终 CRC 结果的位顺序

这些选项影响不同 CRC 算法的反射设置。

常见 CRC 配置
-------------

下表列出了常见的 CRC 配置：

+----------------+----------+---------------+---------------+------------------+---------------+---------------+
| CRC 算法       | 位宽     | 多项式        | 初始值        | 最终 XOR 值      | 反转输入      | 反转输出      |
+================+==========+===============+===============+==================+===============+===============+
| CRC-32         | 32       | 0x04C11DB7    | 0xFFFFFFFF    | 0xFFFFFFFF       | true          | true          |
+----------------+----------+---------------+---------------+------------------+---------------+---------------+
| CRC-16/CCITT   | 16       | 0x1021        | 0x0000        | 0x0000           | false         | false         |
+----------------+----------+---------------+---------------+------------------+---------------+---------------+
| CRC-16/IBM     | 16       | 0x8005        | 0x0000        | 0x0000           | true          | true          |
+----------------+----------+---------------+---------------+------------------+---------------+---------------+
| CRC-8/MAXIM    | 8        | 0x31          | 0x00          | 0x00             | true          | true          |
+----------------+----------+---------------+---------------+------------------+---------------+---------------+

线程安全
========

异步 CRC 驱动程序设计为线程安全的，可以从多个任务中使用。该驱动程序采用 **无竞争有限状态机（FSM）** 架构，确保线程安全并正确处理并发 CRC 请求。

线程安全保证
------------

- 所有公共 API 可以同时从不同任务调用
- 驱动程序对内部状态使用原子操作和临界区保护
- 请求队列确保并发调用被正确串行化

ISR 上下文限制
--------------

异步 API 和阻塞 API 都不能从中断上下文调用。具体来说：

- :cpp:func:`esp_async_crc_calc`：涉及内存分配/释放、DMA 准备工作和非 ISR 安全的日志函数
- :cpp:func:`esp_crc_calc_blocking`：使用可能阻塞的同步原语

回调限制
--------

回调函数（:cpp:type:`async_crc_isr_cb_t`）在中断上下文中执行。因此：

- 不要执行阻塞操作（如 ``vTaskDelay``、带超时的 ``xQueueSend``）
- 保持执行时间最小化，以免影响系统中断延迟
- 不要使用 ``malloc`` 或类似函数分配内存
- 只使用 ISR 安全的 FreeRTOS API（如 ``xQueueSendFromISR``, ``xSemaphoreGiveFromISR``）
- 如果回调唤醒了高优先级任务，返回 ``true``

使用队列的回调示例：

.. code:: c

    static bool crc_callback(async_crc_handle_t crc_hdl, async_crc_event_data_t *edata, void *cb_args)
    {
        QueueHandle_t queue = (QueueHandle_t)cb_args;
        BaseType_t high_task_awoken = pdFALSE;
        // 通过 ISR 安全队列将结果发送到任务
        xQueueSendFromISR(queue, &edata->crc_result, &high_task_awoken);
        return high_task_awoken == pdTRUE;
    }

缓冲区要求
==========

异步 CRC 驱动程序对数据缓冲区有特定要求。

内存类型
--------

数据缓冲区可以来自内部存储区域（DRAM/IRAM）或外部存储区域（PSRAM, Flash）中。驱动程序自动处理两者：

.. code:: c

    // 内部 RAM
    static char internal_data[] = "Data in internal RAM";
    esp_async_crc_calc(crc_hdl, internal_data, strlen(internal_data), &params, callback, NULL);

    // 外部 Flash
    static const char *flash_data = "Data in external Flash";
    esp_async_crc_calc(crc_hdl, flash_data, strlen(flash_data), &params, callback, NULL);

性能注意事项
============

积压配置
--------

``backlog`` 配置影响性能：

- 小积压（4-8）：内存使用量较低，高负载下可能会产生背压
- 大积压（16+）：突发工作负载的吞吐量更好，内存使用量更高

根据应用的内存约束和工作负载模式进行选择。

DMA 突发大小
------------

``dma_burst_size`` 影响 DMA 传输效率：

- 较大的突发大小可以提高吞吐量
- 典型值：16、32、64 字节

最佳值取决于芯片的 DMA 控制器功能。

应用示例
========

- :example:`peripherals/dma/async_crc` 演示了如何通过交互式控制台 CLI 使用异步 CRC 驱动程序。

API 参考
========

异步 CRC 驱动程序函数
---------------------

.. include-build-file:: inc/esp_async_crc.inc
