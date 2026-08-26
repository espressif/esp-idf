========================
Asynchronous Memory Copy
========================

:link_to_translation:`zh_CN:[中文]`

The Async Memcpy driver uses DMA to copy data while the CPU performs other work. Use it for sufficiently large buffers when waiting for ``memcpy`` would delay useful work, such as preparing the next frame or processing the previous buffer.

This document starts with a blocking copy, then shows how to submit copies without blocking the calling task.

.. contents::
    :local:
    :depth: 2

Before You Start
================

The driver is available only on targets that support asynchronous memory copy. Add ``esp_driver_dma`` to your project's component dependencies before including ``esp_async_memcpy.h``.

DMA must be able to access both buffers. Allocate destination buffers in DMA-capable RAM. Whether a particular DMA backend supports PSRAM depends on the target and backend.

.. important::

    Do not read or modify the destination buffer until its copy has completed. Do not modify the source buffer until its copy has completed either.

Quick Start
===========

The typical workflow is:

.. mermaid::

    flowchart TD
        install["Install driver"] --> choose{"How should the task wait?"}
        choose --> blocking["Blocking copy<br/>esp_memcpy_blocking"]
        choose --> async["Async copy<br/>esp_async_memcpy"]
        async --> callback["Callback notifies task"]
        blocking --> use["Use destination buffer"]
        callback --> use
        use --> more{"More copies?"}
        more -->|Yes| choose
        more -->|No| uninstall["Uninstall driver"]

        classDef blocking fill:#E8F1FB,stroke:#3B82C4,color:#1B4F72
        classDef async fill:#F3E8FF,stroke:#8B5CF6,color:#5B2C8A
        classDef result fill:#E8F5E9,stroke:#43A047,color:#1B5E20
        classDef cleanup fill:#F5F5F5,stroke:#757575,color:#424242
        class blocking blocking
        class async,callback async
        class use,result result
        class uninstall cleanup

Scenario 1: Copy One Buffer and Wait
=====================================

Start with :cpp:func:`esp_memcpy_blocking` if the next operation needs the copied data immediately. It uses DMA for suitable buffers and waits until the copy is complete. For small buffers, it safely falls back to a CPU copy.

.. code-block:: c

    #include "esp_async_memcpy.h"

    async_memcpy_handle_t memcpy_hdl = NULL;
    async_memcpy_config_t config = {
        .backlog = 1,
        .weight = 0,
        .dma_burst_size = 16,
    };

    // Explicitly select the AHB GDMA backend.
    ESP_ERROR_CHECK(esp_async_memcpy_install_gdma_ahb(&config, &memcpy_hdl));

    // src and dst are DMA-accessible buffers. The call returns after dst is ready.
    ESP_ERROR_CHECK(esp_memcpy_blocking(memcpy_hdl, dst, src, copy_size, -1));

    // It is now safe to use dst.
    process_data(dst, copy_size);

    ESP_ERROR_CHECK(esp_async_memcpy_uninstall(memcpy_hdl));

``timeout_ms`` must be ``-1``, which waits indefinitely. The blocking API must be called from task context, not from an ISR.

Installing the Driver
---------------------

Select a DMA backend explicitly when installing the driver. The AHB GDMA backend used in the previous example is available only on targets with AHB GDMA support. Choose an install function that is available on your target and matches the DMA engine your application intends to use:

.. list::

    :SOC_CP_DMA_SUPPORTED: - :cpp:func:`esp_async_memcpy_install_cpdma`
    :SOC_AHB_GDMA_SUPPORTED: - :cpp:func:`esp_async_memcpy_install_gdma_ahb`
    :SOC_AXI_GDMA_SUPPORTED: - :cpp:func:`esp_async_memcpy_install_gdma_axi`
    :SOC_LP_AHB_GDMA_SUPPORTED: - :cpp:func:`esp_async_memcpy_install_gdma_lp_ahb`
    :SOC_DW_GDMA_SUPPORTED: - :cpp:func:`esp_async_memcpy_install_dw_gdma`

For a single blocking copy, set :cpp:member:`async_memcpy_config_t::backlog` to 1. Increase it when multiple copies can be pending. :cpp:member:`async_memcpy_config_t::dma_burst_size` controls the burst size in bytes; start with 16 and tune it only after measuring your workload. Set :cpp:member:`async_memcpy_config_t::weight` to 0 unless weighted arbitration is supported and your application needs to adjust its average bus bandwidth.

Scenario 2: Continue Working While DMA Copies
==============================================

Use :cpp:func:`esp_async_memcpy` when the task has useful work to do while DMA transfers the buffer. The function queues the request and returns before the copy finishes. A callback then notifies the task that owns the destination buffer.

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

    // DMA is copying. Do work that does not access src or dst here.
    prepare_next_operation();

    xSemaphoreTake(done, portMAX_DELAY);
    // The callback has run and dst is ready.
    process_data(dst, copy_size);

The driver is thread-safe, so tasks can submit requests through the same handle. Requests are processed in submission order. Set ``backlog`` high enough for the maximum number of copies that your application may have pending.

.. warning::

    The callback runs in ISR context. Keep it short and use only ISR-safe functions, such as ``xSemaphoreGiveFromISR`` or ``xQueueSendFromISR``. Do not call blocking APIs, perform lengthy processing, or submit another copy from the callback.

Buffer Size and Alignment
=========================

The driver handles unaligned source and destination addresses. It uses the CPU for unaligned edge bytes and DMA for the cache-aligned body, so applications do not need to manually align ordinary buffers.

For :cpp:func:`esp_async_memcpy`, a cached destination buffer must be at least two cache lines long. Smaller requests return :c:macro:`ESP_ERR_INVALID_SIZE`; use standard ``memcpy`` instead. :cpp:func:`esp_memcpy_blocking` automatically uses a CPU copy for that case.

.. note::

    DMA is not automatically faster for every transfer. For a short copy, CPU ``memcpy`` avoids DMA setup overhead. Measure with representative buffer sizes before moving a performance-critical path to DMA.

Finishing and Releasing the Driver
==================================

Keep the driver installed while it is needed. Before calling :cpp:func:`esp_async_memcpy_uninstall`, wait for every queued copy to finish and ensure no task can submit another request. The handle and its resources are no longer valid after a successful uninstall.

ETM Event
=========

.. only:: SOC_ETM_SUPPORTED and SOC_GDMA_SUPPORT_ETM

    A completed copy can generate an event for the :doc:`ETM </api-reference/peripherals/etm>` module. Call :cpp:func:`esp_async_memcpy_new_etm_event` with :cpp:enumerator:`ASYNC_MEMCPY_ETM_EVENT_COPY_DONE` to obtain the event handle, then connect it to an ETM task as described in the :doc:`ETM documentation </api-reference/peripherals/etm>`.

API Reference
=============

Async Memcpy Driver Functions
-----------------------------

.. include-build-file:: inc/esp_async_memcpy.inc
