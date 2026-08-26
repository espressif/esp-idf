============
异步内存复制
============

:link_to_translation:`en:[English]`

异步内存复制驱动使用 DMA 搬运数据，让 CPU 可以同时处理其他工作。它适合较大的 buffer，例如 CPU 在等待 ``memcpy`` 时，本可以准备下一帧或处理上一块数据的场景。

本文先完成一次阻塞复制，再介绍如何在复制期间继续执行任务。

.. contents::
    :local:
    :depth: 2

开始前
======

该驱动仅在支持异步内存复制的目标芯片上可用。在包含 ``esp_async_memcpy.h`` 前，请为项目组件添加 ``esp_driver_dma`` 依赖。

DMA 必须能访问源和目标 buffer。目标 buffer 应分配在 DMA 可访问的 RAM 中。特定 DMA 后端是否支持 PSRAM，取决于目标芯片和所选后端。

.. important::

    在复制完成前，不要读取或修改目标 buffer；也不要修改源 buffer。

快速开始
========

典型流程如下：

.. mermaid::

    flowchart TD
        install["安装驱动"] --> choose{"任务如何等待？"}
        choose --> blocking["阻塞复制<br/>esp_memcpy_blocking"]
        choose --> async["异步复制<br/>esp_async_memcpy"]
        async --> callback["回调通知任务"]
        blocking --> use["使用目标 buffer"]
        callback --> use
        use --> more{"还要继续复制？"}
        more -->|是| choose
        more -->|否| uninstall["卸载驱动"]

        classDef blocking fill:#E8F1FB,stroke:#3B82C4,color:#1B4F72
        classDef async fill:#F3E8FF,stroke:#8B5CF6,color:#5B2C8A
        classDef result fill:#E8F5E9,stroke:#43A047,color:#1B5E20
        classDef cleanup fill:#F5F5F5,stroke:#757575,color:#424242
        class blocking blocking
        class async,callback async
        class use,result result
        class uninstall cleanup

场景 1：复制一个 Buffer 并等待
===============================

如果下一步操作必须立刻使用复制结果，先使用 :cpp:func:`esp_memcpy_blocking`。它会对合适的 buffer 使用 DMA，并等待复制完成；对于较小的 buffer，它会安全地回退到 CPU 复制。

.. code-block:: c

    #include "esp_async_memcpy.h"

    async_memcpy_handle_t memcpy_hdl = NULL;
    async_memcpy_config_t config = {
        .backlog = 1,
        .weight = 0,
        .dma_burst_size = 16,
    };

    // 显式选择 AHB GDMA 后端。
    ESP_ERROR_CHECK(esp_async_memcpy_install_gdma_ahb(&config, &memcpy_hdl));

    // src 和 dst 是 DMA 可访问的 buffer。函数返回时，dst 已经准备就绪。
    ESP_ERROR_CHECK(esp_memcpy_blocking(memcpy_hdl, dst, src, copy_size, -1));

    // 现在可以安全使用 dst。
    process_data(dst, copy_size);

    ESP_ERROR_CHECK(esp_async_memcpy_uninstall(memcpy_hdl));

``timeout_ms`` 必须为 ``-1``，表示一直等待到复制完成。阻塞 API 必须在任务上下文中调用，不能在 ISR 中调用。

安装驱动
--------

安装驱动时请显式选择 DMA 后端。前面示例使用的 AHB GDMA 后端仅适用于支持 AHB GDMA 的目标芯片。请选择目标芯片支持且符合应用需求的 DMA 引擎：

.. list::

    :SOC_CP_DMA_SUPPORTED: - :cpp:func:`esp_async_memcpy_install_cpdma`
    :SOC_AHB_GDMA_SUPPORTED: - :cpp:func:`esp_async_memcpy_install_gdma_ahb`
    :SOC_AXI_GDMA_SUPPORTED: - :cpp:func:`esp_async_memcpy_install_gdma_axi`
    :SOC_LP_AHB_GDMA_SUPPORTED: - :cpp:func:`esp_async_memcpy_install_gdma_lp_ahb`
    :SOC_DW_GDMA_SUPPORTED: - :cpp:func:`esp_async_memcpy_install_dw_gdma`

对于一次阻塞复制，将 :cpp:member:`async_memcpy_config_t::backlog` 设为 1 即可；若可能同时等待多个复制请求，应增大该值。:cpp:member:`async_memcpy_config_t::dma_burst_size` 设置 DMA 突发大小，单位为字节；可从 16 开始，仅在性能测试后再调整。除非目标芯片支持加权仲裁且应用需要调节平均总线带宽，否则将 :cpp:member:`async_memcpy_config_t::weight` 设为 0。

场景 2：在 DMA 复制期间继续工作
=================================

如果任务在 DMA 搬运期间还有其他工作，使用 :cpp:func:`esp_async_memcpy`。该函数将请求加入队列后立即返回，不会等待复制完成。随后由回调通知拥有目标 buffer 的任务。

.. code-block:: c

    #include "freertos/FreeRTOS.h"
    #include "freertos/semphr.h"
    #include "esp_async_memcpy.h"

    static bool copy_done_cb(async_memcpy_handle_t memcpy_hdl,
                             async_memcpy_event_t *event,
                             void *user_ctx)
    {
        BaseType_t high_task_woken = pdFALSE;
        SemaphoreHandle_t done = (SemaphoreHandle_t)user_ctx;

        xSemaphoreGiveFromISR(done, &high_task_woken);
        return high_task_woken == pdTRUE;
    }

    SemaphoreHandle_t done = xSemaphoreCreateBinary();

    ESP_ERROR_CHECK(esp_async_memcpy(memcpy_hdl, dst, src, copy_size,
                                     copy_done_cb, done));

    // DMA 正在复制。在此执行不会访问 src 或 dst 的其他工作。
    prepare_next_operation();

    xSemaphoreTake(done, portMAX_DELAY);
    // 回调已运行，dst 已准备就绪。
    process_data(dst, copy_size);

驱动是线程安全的，多个任务可以通过同一个句柄提交请求。请求会按提交顺序处理。请将 ``backlog`` 设为应用可能同时待处理的最大复制请求数。

.. warning::

    回调运行在 ISR 上下文。应保持简短，并且只能调用 ISR-safe 函数，例如 ``xSemaphoreGiveFromISR`` 或 ``xQueueSendFromISR``。不要在回调中调用阻塞 API、执行耗时处理或提交新的复制请求。

Buffer 大小与对齐
==================

该驱动支持未对齐的源和目标地址。它使用 CPU 复制未对齐的边缘字节，并使用 DMA 复制按缓存行对齐的主体，因此普通应用无需手动对齐 buffer。

对于 :cpp:func:`esp_async_memcpy`，若目标 buffer 位于缓存区，其长度至少应为两个缓存行。较小的请求会返回 :c:macro:`ESP_ERR_INVALID_SIZE`，应改用标准 ``memcpy``。:cpp:func:`esp_memcpy_blocking` 在这种情况下会自动使用 CPU 复制。

.. note::

    DMA 并不会自动让每次复制更快。短复制的 DMA 配置开销通常高于收益。请使用接近实际场景的 buffer 大小进行测量，再决定是否将性能关键路径迁移到 DMA。

结束与释放驱动
==============

在需要时持续保持驱动已安装。调用 :cpp:func:`esp_async_memcpy_uninstall` 前，请等待所有排队的复制完成，并确保没有任务会再提交请求。成功卸载后，句柄及其资源不再有效。

ETM 事件
========

.. only:: SOC_ETM_SUPPORTED and SOC_GDMA_SUPPORT_ETM

    复制完成时可以产生供 :doc:`ETM </api-reference/peripherals/etm>` 模块使用的事件。调用 :cpp:func:`esp_async_memcpy_new_etm_event` 并传入 :cpp:enumerator:`ASYNC_MEMCPY_ETM_EVENT_COPY_DONE`，即可获取事件句柄。如何将事件连接到 ETM 任务，请参阅 :doc:`ETM 文档 </api-reference/peripherals/etm>`。

API 参考
========

异步内存复制驱动程序函数
------------------------

.. include-build-file:: inc/esp_async_memcpy.inc
