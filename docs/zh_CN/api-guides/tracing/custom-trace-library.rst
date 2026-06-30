.. _app_trace-integrating-a-custom-trace-library:

集成自定义跟踪库
================

:link_to_translation:`en:[English]`

:doc:`esp_trace <index>` 组件允许第三方跟踪记录器在不修改 ESP-IDF 的情况下接入框架。SEGGER SystemView 等外部编码器正是通过这种方式集成到跟踪系统中。有关高层设计，请参阅 :doc:`architecture`。

外部组件需提供：

- 一个 **编码器适配器**，通过 ``ESP_TRACE_REGISTER_ENCODER()`` 注册，用于将跟踪数据格式化为该记录器的协议。
- 一个 ``esp_trace_freertos_impl.h`` 头文件，用于定义记录器所需的 FreeRTOS 跟踪钩子。

编码器独立于主机链路。它可以使用任何已注册的 :doc:`传输 <transports>`，例如 JTAG/UART 上的 apptrace、USB Serial JTAG 或自定义传输。

编码器端口
----------

编码器实现 :cpp:struct:`esp_trace_encoder_vtable_t`。

- 只有 ``init`` 和 ``write`` 为必需回调。
- ``start`` / ``stop`` / ``flush`` 由 :cpp:func:`esp_trace_start`、:cpp:func:`esp_trace_stop`、:cpp:func:`esp_trace_flush` 调度。
- ``panic_handler`` 在 Panic 路径上被调用，使适配器可以在不使用常规锁的情况下刷新。
- ``take_lock`` / ``give_lock`` 提供编码器的多核序列化；核心自身不添加任何加锁。

编码器实例（:cpp:struct:`esp_trace_encoder`）保存其函数表、当前活动跟踪会话所绑定的传输以及编码器特定状态。具体字段请参阅该结构体的参考文档。

如果编码器需要设置传输参数，请在 ``init`` 中通过带类型的配置键配置所绑定的传输，例如使用 ``ESP_TRACE_TRANSPORT_CFG_HEADER_SIZE`` 设置传输数据头大小。

在链接时注册编码器；核心按所配置的名称查找：

.. code-block:: c

    ESP_TRACE_REGISTER_ENCODER("sysview", &s_sysview_vt);

传输端口
--------

传输实现 :cpp:struct:`esp_trace_transport_vtable_t`。

使用 ``ESP_TRACE_REGISTER_TRANSPORT("name", &vtable)`` 注册。大多数项目只需要自定义编码器，并可使用已有传输。仅当需要新的主机链路时才需要实现传输。apptrace 传输参见 :doc:`transports`。

加锁与重入
----------

``write``、``flush`` / ``flush_nolock``、``read``、``take_lock`` / ``give_lock``、``panic_handler`` 等运行时回调可能从 FreeRTOS 跟踪钩子中调用。其中一些回调也可能从 ISR 上下文调用，另一些回调会在持有编码器锁时调用。

.. warning::

    不要从这些回调中调用本身会触发跟踪钩子的 FreeRTOS 或 IDF API。任何会触发 ``trace*()`` 宏的操作都会重新进入跟踪路径，可能递归进入你的编码器、在编码器的非递归自旋锁上死锁，或在 ISR 上下文中调用仅限任务的 API。

在运行时回调中应特别避免：

- 会让出的任务 API（``vTaskDelay``、``vTaskSuspend``、``xTaskNotify*``）
- 队列 / 信号量 / 互斥量 API（``xQueueSend`` / ``xQueueReceive``、``xSemaphoreTake`` / ``xSemaphoreGive``）
- 流缓冲区和消息缓冲区 API
- 可能获取内部互斥量的堆分配

在运行时回调中，应使用无锁或仅自旋锁的原语（``esp_trace_lock_*``、``esp_trace_rb_*``）、底层寄存器访问、原子操作以及 ``esp_rom_*`` 辅助函数。较重的工作（例如 FreeRTOS API 调用或内存分配）只应在 ``init()`` 中、跟踪开始前完成。

对于需要复杂驱动或网络协议栈的传输，运行时回调应只作为生产者使用。将跟踪数据复制到预分配且适合跟踪路径使用的缓冲区中，例如 ``esp_trace_rb_*`` 环形缓冲区，然后尽快返回。由 ``init()`` 创建的工作任务从该缓冲区取出数据，并在跟踪回调路径和编码器锁之外调用 SPI master、socket、StreamBuffer 或其他 FreeRTOS API。由于回调不能阻塞，也不能通过会让出的 API 唤醒任务，工作任务应轮询缓冲区（或在传输层事件上唤醒）；当缓冲区已满时应丢弃数据，而不是把背压传导回回调。

FreeRTOS 跟踪钩子
-----------------

为捕获 FreeRTOS 事件，外部组件需提供 ``esp_trace_freertos_impl.h`` 头文件，其中定义所需的跟踪宏（``traceTASK_SWITCHED_IN()``、``traceISR_ENTER()`` 等）。当 ``CONFIG_ESP_TRACE_LIB_EXTERNAL=y`` 时，``esp_trace`` 会包含该头文件。所需的 CMake 配置（用于适配器注册的 ``WHOLE_ARCHIVE``，以及使头文件对 ``esp_trace`` 可见）参见 :component_file:`esp_trace 组件 README <esp_trace/README.md>`。

应用示例
--------

- :example:`system/esp_trace_custom_library` 是一个最简模板，演示如何接入外部编码器、说明 FreeRTOS 跟踪钩子头文件的包含链，以及通过编码器锁实现多核序列化。
