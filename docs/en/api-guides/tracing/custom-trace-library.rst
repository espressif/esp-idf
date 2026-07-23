.. _app_trace-integrating-a-custom-trace-library:

Integrating a Custom Trace Library
==================================

:link_to_translation:`zh_CN:[中文]`

The :doc:`esp_trace <index>` component lets a third-party trace recorder plug into ESP-IDF without patching the framework. External encoders, such as SEGGER SystemView, use this path. For the high-level design, see :doc:`architecture`.

An external component provides:

- An **encoder adapter**, registered via ``ESP_TRACE_REGISTER_ENCODER()``, which formats trace data into the recorder's protocol.
- An ``esp_trace_freertos_impl.h`` header that defines the FreeRTOS trace hooks needed by the recorder.

The encoder is independent from the host link. It can use any registered :doc:`transport <transports>`, such as apptrace over JTAG/UART, USB Serial JTAG, or a custom transport.

Encoder Port
------------

An encoder implements :cpp:struct:`esp_trace_encoder_vtable_t`.

- ``init`` and ``write`` are the only required callbacks.
- ``start`` / ``stop`` / ``flush`` are dispatched from :cpp:func:`esp_trace_start`, :cpp:func:`esp_trace_stop`, and :cpp:func:`esp_trace_flush`.
- ``panic_handler`` is called from the panic path so the adapter can flush without taking normal locks.
- ``take_lock`` / ``give_lock`` provide the encoder's cross-core serialization; the core adds no locking of its own.

The encoder instance (:cpp:struct:`esp_trace_encoder`) holds its function table, the transport bound for the active trace session, and any encoder-specific state. See the struct reference for the exact fields.

If the encoder needs transport-specific settings, configure its bound transport from ``init`` using typed configuration keys, for example ``ESP_TRACE_TRANSPORT_CFG_HEADER_SIZE`` to set the transport header size.

Register the encoder at link time; the core looks it up by the configured name:

.. code-block:: c

    ESP_TRACE_REGISTER_ENCODER("sysview", &s_sysview_vt);

Transport Port
--------------

A transport implements :cpp:struct:`esp_trace_transport_vtable_t`.

Register it with ``ESP_TRACE_REGISTER_TRANSPORT("name", &vtable)``. Most projects only need a custom encoder and can use an existing transport. Implement a transport only when you need a new host link. See :doc:`transports` for the apptrace transport.

Locking and Reentrancy
----------------------

Runtime callbacks such as ``write``, ``flush`` / ``flush_nolock``, ``read``, ``take_lock`` / ``give_lock``, and ``panic_handler`` can run from FreeRTOS trace hooks. Some of them can also run from ISR context, and some are called while the encoder lock is held.

.. warning::

    Do not call FreeRTOS or IDF APIs that themselves emit trace hooks from these callbacks. Anything that triggers a ``trace*()`` macro re-enters the tracing path and can recurse into your encoder, deadlock on the encoder's non-recursive spinlock, or call a task-only API from ISR context.

Specifically avoid these APIs from runtime callbacks:

- Task APIs that yield (``vTaskDelay``, ``vTaskSuspend``, ``xTaskNotify*``)
- Queue / semaphore / mutex APIs (``xQueueSend`` / ``xQueueReceive``, ``xSemaphoreTake`` / ``xSemaphoreGive``)
- Stream and message buffer APIs
- Heap allocations that may take an internal mutex

Use lock-free or spinlock-only primitives (``esp_trace_lock_*``, ``esp_trace_rb_*``), low-level register access, atomics, and ``esp_rom_*`` helpers in runtime callbacks. Do heavier work, such as FreeRTOS API calls or allocations, only in ``init()`` before tracing starts.

For transports that need a complex driver or network stack, keep the runtime callback as a producer only. Copy the trace data into a preallocated, trace-safe buffer, such as an ``esp_trace_rb_*`` ring buffer, and return quickly. A worker task created during ``init()`` consumes that buffer and calls APIs such as SPI master, sockets, StreamBuffer, or other FreeRTOS facilities outside the trace callback path and outside the encoder lock. Because the callback must not block or wake a task through yielding APIs, have the worker poll the buffer (or wake on a transport-level event) and drop data when the buffer is full rather than pushing backpressure onto the callback.

FreeRTOS Trace Hooks
--------------------

To capture FreeRTOS events, the external component implementing a trace encoder should provide an ``esp_trace_freertos_impl.h`` header, defining the desired trace macros (``traceTASK_SWITCHED_IN()``, ``traceISR_ENTER()``, and so on). ``esp_trace`` includes this header when :ref:`CONFIG_ESP_TRACE_LIB_EXTERNAL <CONFIG_ESP_TRACE_LIB_EXTERNAL>` is enabled.

Application Example
-------------------

- :example:`system/esp_trace_custom_library` is a minimal template that wires up an external encoder, demonstrates the FreeRTOS trace-hook include chain, and shows cross-core serialization through the encoder lock.
