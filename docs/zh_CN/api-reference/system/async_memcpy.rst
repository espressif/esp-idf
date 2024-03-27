异步内存复制
========================

:link_to_translation:`en:[English]`

概述
--------

{IDF_TARGET_NAME} 有一个 DMA 引擎，能够以异步方式帮助 CPU 完成内部内存复制操作。

异步 memcpy API 中封装了所有 DMA 配置和操作，:cpp:func:`esp_async_memcpy` 的签名与标准 C 库的 ``memcpy`` 函数基本相同。

DMA 允许多个内存复制请求在首个请求完成之前排队，即允许计算和内存复制的重叠。此外，通过注册事件回调函数，还可以知道内存复制请求完成的准确时间。

.. only:: SOC_AHB_GDMA_SUPPORT_PSRAM

    如果异步 memcpy 是基于 AHB GDMA 构建的，那么也可以用适合的对齐方式从 PSRAM 复制数据或向其中复制数据。

.. only:: SOC_AXI_GDMA_SUPPORT_PSRAM

    如果异步 memcpy 是基于 AXI GDMA 构建的，那么也可以用适合的对齐方式从 PSRAM 复制数据或向其中复制数据。


配置并安装驱动
----------------------------

安装异步 memcpy 驱动的方法取决于底层 DMA 引擎：

.. list::

    :SOC_CP_DMA_SUPPORTED: - :cpp:func:`esp_async_memcpy_install_cpdma` 用于安装基于 CP DMA 引擎的异步 memcpy 驱动。
    :SOC_AHB_GDMA_SUPPORTED: - :cpp:func:`esp_async_memcpy_install_gdma_ahb` 用于安装基于 AHB GDMA 引擎的异步 memcpy 驱动。
    :SOC_AXI_GDMA_SUPPORTED: - :cpp:func:`esp_async_memcpy_install_gdma_axi` 用于安装基于 AXI GDMA 引擎的异步 memcpy 驱动。
    - :cpp:func:`esp_async_memcpy_install` 是一个通用 API，用于安装带有默认 DMA 引擎的异步 memcpy 驱动。如果 SoC 具有 CP DMA 引擎，则默认 DMA 引擎为 CP DMA，否则，默认 DMA 引擎为 AHB GDMA。

在 :cpp:type:`async_memcpy_config_t` 中设置驱动配置：

* :cpp:member:`backlog`：此项用于配置首个请求完成前可以排队的最大内存复制事务数量。如果将此字段设置为零，会应用默认值 4。
* :cpp:member:`sram_trans_align`：声明 SRAM 中数据地址和复制大小的对齐方式，如果数据没有对齐限制，则设置为零。如果设置为四的倍数值（即 4X），驱动程序将内部启用突发模式，这有利于某些和性能相关的应用程序。
* :cpp:member:`psram_trans_align`：声明 PSRAM 中数据地址和复制大小的对齐方式。如果 memcpy 的目标地址位于 PSRAM 中，用户必须给出一个有效值（只支持 16、32、64）。如果设置为零，会默认采用 16 位对齐。在内部，驱动程序会根据对齐方式来配置 DMA 访问 PSRAM 时所用的块大小。
* :cpp:member:`flags`：此项可以启用一些特殊的驱动功能。

.. code-block:: c

    async_memcpy_config_t config = ASYNC_MEMCPY_DEFAULT_CONFIG();
    // 更新底层 DMA 引擎支持的最大数据流
    config.backlog = 8;
    async_memcpy_handle_t driver = NULL;
    ESP_ERROR_CHECK(esp_async_memcpy_install(&config, &driver)); // 使用默认 DMA 引擎安装驱动

发送内存复制请求
------------------------

使用 :cpp:func:`esp_async_memcpy` API 将内存复制请求发送到 DMA 引擎。在驱动程序成功安装后才能调用该 API。此 API 是线程安全的，因此可以从不同的任务中调用。

与 libc 版本的 ``memcpy`` 不同，你可以选择给 :cpp:func:`esp_async_memcpy` 设置一个回调函数，以便在内存复制完成时收到通知。注意，回调是在 ISR 上下文中执行的，请不要在回调中调用任何阻塞函数。

回调函数的原型是 :cpp:type:`async_memcpy_isr_cb_t`。回调函数只有在借助 RTOS API（如 :cpp:func:`xSemaphoreGiveFromISR`）唤醒了高优先级任务后才能返回 true。

.. code-block:: c

    // 回调实现，在 ISR 上下文中运行
    static bool my_async_memcpy_cb(async_memcpy_handle_t mcp_hdl, async_memcpy_event_t *event, void *cb_args)
    {
        SemaphoreHandle_t sem = (SemaphoreHandle_t)cb_args;
        BaseType_t high_task_wakeup = pdFALSE;
        xSemaphoreGiveFromISR(semphr, &high_task_wakeup); // 如果解锁了一些高优先级任务，则将 high_task_wakeup 设置为 pdTRUE
        return high_task_wakeup == pdTRUE;
    }

    // 创建一个信号量，在异步 memcpy 完成时进行报告
    SemaphoreHandle_t semphr = xSemaphoreCreateBinary();

    // 从用户的上下文中调用
    ESP_ERROR_CHECK(esp_async_memcpy(driver_handle, to, from, copy_len, my_async_memcpy_cb, my_semaphore));
    // 其他事项
    xSemaphoreTake(my_semaphore, portMAX_DELAY); // 等待 buffer 复制完成


卸载驱动
----------------

使用 :cpp:func:`esp_async_memcpy_uninstall` 卸载异步 memcpy 驱动。无需在每次 memcpy 操作后手动卸载。如果你的应用程序不再需要此驱动，此 API 可以帮助回收内存和其他硬件资源。

.. only:: SOC_ETM_SUPPORTED and SOC_GDMA_SUPPORT_ETM

    ETM 事件
    ---------

    在异步 memcpy 操作完成时会生成一个事件，此事件能够与 :doc:`ETM </api-reference/peripherals/etm>` 模块进行交互。可以调用 :cpp:func:`esp_async_memcpy_new_etm_event` 获取 ETM 事件句柄。

    如需了解如何将此事件连接到 ETM 通道，请参考文档 :doc:`ETM </api-reference/peripherals/etm>`。

API 参考
-------------

.. include-build-file:: inc/esp_async_memcpy.inc
