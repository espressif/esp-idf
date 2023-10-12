Asynchronous Memory Copy
========================

:link_to_translation:`zh_CN:[中文]`

Overview
--------

{IDF_TARGET_NAME} has a DMA engine which can help to offload internal memory copy operations from the CPU in an asynchronous way.

The async memcpy API wraps all DMA configurations and operations. The signature of :cpp:func:`esp_async_memcpy` is almost the same as the standard libc ``memcpy`` function.

The DMA allows multiple memory copy requests to be queued up before the first one is completed, which allows overlap of computation and memory copy. Moreover, it is still possible to know the exact time when a memory copy request is completed by registering an event callback.

.. only:: SOC_AHB_GDMA_SUPPORT_PSRAM

    If the async memcpy is constructed upon the AHB GDMA, it is also possible to copy data from/to PSRAM with a proper alignment.

.. only:: SOC_AXI_GDMA_SUPPORT_PSRAM

    If the async memcpy is constructed upon the AXI GDMA, it is also possible to copy data from/to PSRAM with a proper alignment.


Configure and Install Driver
----------------------------

There are several ways to install the async memcpy driver, depending on the underlying DMA engine:

.. list::

    :SOC_CP_DMA_SUPPORTED: - :cpp:func:`esp_async_memcpy_install_cpdma` is used to install the async memcpy driver based on the CP DMA engine.
    :SOC_AHB_GDMA_SUPPORTED: - :cpp:func:`esp_async_memcpy_install_gdma_ahb` is used to install the async memcpy driver based on the AHB GDMA engine.
    :SOC_AXI_GDMA_SUPPORTED: - :cpp:func:`esp_async_memcpy_install_gdma_axi` is used to install the async memcpy driver based on the AXI GDMA engine.
    - :cpp:func:`esp_async_memcpy_install` is a generic API to install the async memcpy driver with a default DMA engine. If the SoC has the CP DMA engine, the default DMA engine is CP DMA. Otherwise, the default DMA engine is AHB GDMA.

Driver configuration is described in :cpp:type:`async_memcpy_config_t`:

* :cpp:member:`backlog`: This is used to configure the maximum number of memory copy transactions that can be queued up before the first one is completed. If this field is set to zero, then the default value 4 will be applied.
* :cpp:member:`sram_trans_align`: Declare SRAM alignment for both data address and copy size, set to zero if the data has no restriction in alignment. If set to a quadruple value (i.e., 4X), the driver will enable the burst mode internally, which is helpful for some performance related application.
* :cpp:member:`psram_trans_align`: Declare PSRAM alignment for both data address and copy size. User has to give it a valid value (only 16, 32, 64 are supported) if the destination of memcpy is located in PSRAM. The default alignment (i.e., 16) will be applied if it is set to zero. Internally, the driver configures the size of block used by DMA to access PSRAM, according to the alignment.
* :cpp:member:`flags`: This is used to enable some special driver features.

.. code-block:: c

    async_memcpy_config_t config = ASYNC_MEMCPY_DEFAULT_CONFIG();
    // update the maximum data stream supported by underlying DMA engine
    config.backlog = 8;
    async_memcpy_handle_t driver = NULL;
    ESP_ERROR_CHECK(esp_async_memcpy_install(&config, &driver)); // install driver with default DMA engine

Send Memory Copy Request
------------------------

:cpp:func:`esp_async_memcpy` is the API to send memory copy request to DMA engine. It must be called after driver is installed successfully. This API is thread safe, so it can be called from different tasks.

Different from the libc version of ``memcpy``, you can optionally pass a callback to :cpp:func:`esp_async_memcpy`, so that you can be notified when the memory copy is finished. Note that the callback is executed in the ISR context, please make sure you will not call any blocking functions in the callback.

The prototype of the callback function is :cpp:type:`async_memcpy_isr_cb_t`. The callback function should only return true if it wakes up a high priority task by RTOS APIs like :cpp:func:`xSemaphoreGiveFromISR`.

.. code-block:: c

    // Callback implementation, running in ISR context
    static bool my_async_memcpy_cb(async_memcpy_handle_t mcp_hdl, async_memcpy_event_t *event, void *cb_args)
    {
        SemaphoreHandle_t sem = (SemaphoreHandle_t)cb_args;
        BaseType_t high_task_wakeup = pdFALSE;
        xSemaphoreGiveFromISR(semphr, &high_task_wakeup); // high_task_wakeup set to pdTRUE if some high priority task unblocked
        return high_task_wakeup == pdTRUE;
    }

    // Create a semaphore used to report the completion of async memcpy
    SemaphoreHandle_t semphr = xSemaphoreCreateBinary();

    // Called from user's context
    ESP_ERROR_CHECK(esp_async_memcpy(driver_handle, to, from, copy_len, my_async_memcpy_cb, my_semaphore));
    // Do something else here
    xSemaphoreTake(my_semaphore, portMAX_DELAY); // Wait until the buffer copy is done


Uninstall Driver
----------------

:cpp:func:`esp_async_memcpy_uninstall` is used to uninstall asynchronous memcpy driver. It is not necessary to uninstall the driver after each memcpy operation. If you know your application will not use this driver anymore, then this API can recycle the memory and other hardware resources for you.

.. only:: SOC_ETM_SUPPORTED and SOC_GDMA_SUPPORT_ETM

    ETM Event
    ---------

    Async memory copy is able to generate an event when one async memcpy operation is done. This event can be used to interact with the :doc:`ETM </api-reference/peripherals/etm>` module. You can call :cpp:func:`esp_async_memcpy_new_etm_event` to get the ETM event handle.

    For how to connect the event to an ETM channel, please refer to the :doc:`ETM </api-reference/peripherals/etm>` documentation.

API Reference
-------------

.. include-build-file:: inc/esp_async_memcpy.inc
