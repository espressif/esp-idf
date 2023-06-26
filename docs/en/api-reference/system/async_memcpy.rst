The Async memcpy API
====================

Overview
--------

{IDF_TARGET_NAME} has a DMA engine which can help to offload internal memory copy operations from the CPU in a asynchronous way.

The async memcpy API wraps all DMA configurations and operations, the signature of :cpp:func:`esp_async_memcpy` is almost the same to the standard libc one.

Thanks to the benefit of the DMA, we don't have to wait for each memory copy to be done before we issue another memcpy request. By the way, it's still possible to know when memcpy is finished by listening in the memcpy callback function.

.. only:: esp32s2

    .. note::
        Memory copy from/to external PSRAM is not supported on ESP32-S2, :cpp:func:`esp_async_memcpy` will abort returning an error if buffer address is not in SRAM.

Configure and Install driver
----------------------------

:cpp:func:`esp_async_memcpy_install` is used to install the driver with user's configuration. Please note that async memcpy has to be called with the handle returned from :cpp:func:`esp_async_memcpy_install`.

Driver configuration is described in :cpp:type:`async_memcpy_config_t`:

* :cpp:member:`backlog`: This is used to configure the maximum number of DMA operations being processed at the same time.
* :cpp:member:`sram_trans_align`: Declare SRAM alignment for both data address and copy size, set to zero if the data has no restriction in alignment. If set to a quadruple value (i.e. 4X), the driver will enable the burst mode internally, which is helpful for some performance related application.
* :cpp:member:`psram_trans_align`: Declare PSRAM alignment for both data address and copy size. User has to give it a valid value (only 16, 32, 64 are supported) if the destination of memcpy is located in PSRAM. The default alignment (i.e. 16) will be applied if it's set to zero. Internally, the driver configures the size of block used by DMA to access PSRAM, according to the alignment.
* :cpp:member:`flags`: This is used to enable some special driver features.

:c:macro:`ASYNC_MEMCPY_DEFAULT_CONFIG` provides a default configuration, which specifies the backlog to 8.

.. highlight:: c

::

    async_memcpy_config_t config = ASYNC_MEMCPY_DEFAULT_CONFIG();
    // update the maximum data stream supported by underlying DMA engine
    config.backlog = 16;
    async_memcpy_t driver = NULL;
    ESP_ERROR_CHECK(esp_async_memcpy_install(&config, &driver)); // install driver, return driver handle

Send memory copy request
------------------------

:cpp:func:`esp_async_memcpy` is the API to send memory copy request to DMA engine. It must be called after driver is installed successfully. This API is thread safe, so it can be called from different tasks.

Different from the libc version of ``memcpy``, user should also pass a callback to :cpp:func:`esp_async_memcpy`, if it's necessary to be notified when the memory copy is done. The callback is executed in the ISR context, make sure you won't violate the restriction applied to ISR handler.

Besides that, the callback function should reside in IRAM space by applying ``IRAM_ATTR`` attribute. The prototype of the callback function is :cpp:type:`async_memcpy_isr_cb_t`, please note that, the callback function should return true if it wakes up a high priority task by some API like :cpp:func:`xSemaphoreGiveFromISR`.

.. highlight:: c

::

    // Callback implementation, running in ISR context
    static IRAM_ATTR bool my_async_memcpy_cb(async_memcpy_t mcp_hdl, async_memcpy_event_t *event, void *cb_args)
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

Uninstall driver (optional)
---------------------------

:cpp:func:`esp_async_memcpy_uninstall` is used to uninstall asynchronous memcpy driver. It's not necessary to uninstall the driver after each memcpy operation. If you know your application won't use this driver anymore, then this API can recycle the memory for you.

.. only:: SOC_ETM_SUPPORTED and SOC_GDMA_SUPPORT_ETM

    ETM Event
    ---------

    Async memory copy is able to generate an event when one async memcpy operation is done. This event can be used to interact with the :doc:`ETM </api-reference/peripherals/etm>` module. You can call :cpp:func:`esp_async_memcpy_new_etm_event` to get the ETM event handle.

    For how to connect the event to an ETM channel, please refer to the :doc:`ETM </api-reference/peripherals/etm>` documentation.

API Reference
-------------

.. include-build-file:: inc/esp_async_memcpy.inc
