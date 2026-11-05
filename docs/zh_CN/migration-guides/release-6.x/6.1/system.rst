系统
====

:link_to_translation:`en:[English]`

SPIRAM DMA 预留池
-----------------

启用 :ref:`CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL` 时，内部 DMA 预留池的能力优先级调整为：

- **调整前**：中优先级 ``MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL``；低优先级 ``MALLOC_CAP_DEFAULT``。
- **调整后**：中优先级 ``MALLOC_CAP_DMA``；低优先级 ``MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL``。

请求 ``MALLOC_CAP_INTERNAL`` 但不包含 ``MALLOC_CAP_DMA`` 的分配（例如 FreeRTOS 任务栈和 TCB）不再优先使用预留池，仅在其他内部堆耗尽后才会使用。显式 ``MALLOC_CAP_DMA`` 请求仍会优先使用预留池。

当 :ref:`CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL` 为非 0 值时，``malloc()`` 优先尝试内部内存的那一轮现在会跳过仅以低优先级提供 ``MALLOC_CAP_DEFAULT`` 的堆：预留池，以及部分芯片上的 RTC RAM 和 SPM（例如 ESP32-P4 上的 SPM 和 RTC RAM）。以前会落在这些堆中的小块 ``malloc()`` 分配现在会优先放入 PSRAM；这些堆仅在 PSRAM 耗尽后才会被使用。必须位于内部存储器的 buffer 应使用 ``heap_caps_malloc()`` 并指定 ``MALLOC_CAP_INTERNAL`` 进行分配。
