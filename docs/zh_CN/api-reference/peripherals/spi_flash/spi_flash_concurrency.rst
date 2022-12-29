.. _concurrency-constraints-flash:

SPI1 flash 并发约束
=========================================

指令/数据 cache（用以执行固件）与 SPI1 外设（由像 SPI flash 驱动一样的驱动程序控制）共享 SPI0/1 总线。因此，SPI1 外设上的操作会对整个系统造成显著的影响。这类操作包括调用 SPI flash API 或者 SPI1 总线上的其他驱动、任何 flash 操作（如读取、写入、擦除）或是由其他用户定义的 SPI 操作（对主 flash 或是其他 SPI 从机）。

.. only:: not (esp32c3 or SOC_SPIRAM_XIP_SUPPORTED)

    在 {IDF_TARGET_NAME} 上，flash 读取/写入/擦除时，cache 必须被禁用。

.. only:: esp32c3

    在 {IDF_TARGET_NAME} 上，默认启用的配置选项 :ref:`CONFIG_SPI_FLASH_AUTO_SUSPEND` 允许 flash/PSRAM 的 cache 访问和 SPI1 的操作并发执行。请参阅 :ref:`auto-suspend`，查看详细信息。

    在该选项被禁用的情况下，读取/写入/擦除 flash 时，cache 必须被禁用。使用驱动访问 SPI1 的相关约束参见 :ref:`impact_disabled_cache`。这些约束会带来更多的 IRAM/DRAM 消耗。

.. only:: SOC_SPIRAM_XIP_SUPPORTED

    在 {IDF_TARGET_NAME} 上，启用配置选项 :ref:`CONFIG_SPIRAM_FETCH_INSTRUCTIONS` （默认禁用）和 :ref:`CONFIG_SPIRAM_RODATA` （默认禁用）后将允许 flash/PSRAM 的 cache 访问和 SPI1 的操作并发执行。请参阅 :ref:`xip_from_psram`，查看详细信息。

    在上述选项被禁用的情况下，读取/写入/擦除 flash 时，cache 必须被禁用。使用驱动访问 SPI1 的相关约束参见 :ref:`impact_disabled_cache`。这些约束会带来更多的 IRAM/DRAM 消耗。

.. _impact_disabled_cache:

当 cache 被禁用时
----------------------------

此时，在 flash 擦写操作中，所有的 CPU 都只能执行 IRAM 中的代码，而且必须从 DRAM 中读取数据。如果您使用本文档中 API 函数，上述限制将自动生效且透明（无需您额外关注），但这些限制可能会影响系统中的其他任务的性能。

.. only:: esp32c3

    .. note::

        启用 :ref:`CONFIG_SPI_FLASH_AUTO_SUSPEND` 时，cache 不会被禁用，其中的操作将通过硬件仲裁器来协调。

.. only:: SOC_SPIRAM_XIP_SUPPORTED

    .. note::

        同时启用 :ref:`CONFIG_SPIRAM_FETCH_INSTRUCTIONS` 和 :ref:`CONFIG_SPIRAM_RODATA` 选项后，cache 不会被禁用。

.. only:: not CONFIG_FREERTOS_UNICORE

    为避免意外读取 flash cache，一个 CPU 在启动 flash 写入或擦除操作时，另一个 CPU 将阻塞。在 flash 操作完成前，所有 CPU 上，所有的非 IRAM 安全的中断都会被禁用。

.. only:: CONFIG_FREERTOS_UNICORE

    为避免意外读取 flash cache，在 flash 操作完成前，所有 CPU 上，所有的非 IRAM 安全的中断都会被禁用。

另请参阅 :ref:`esp_flash_os_func` 和 :ref:`spi_bus_lock`。

除 SPI0/1 以外，SPI 总线上的其他 flash 芯片则不受这种限制。

请参阅 :ref:`应用程序内存分布 <memory-layout>`，查看内部 RAM（如 IRAM、DRAM）和 flash cache 的区别。


.. _iram-safe-interrupt-handlers:

IRAM 安全中断处理程序
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

如果您需要在 flash 操作期间运行中断处理程序（比如低延迟操作），请在 :doc:`注册中断处理程序 </api-reference/system/intr_alloc>` 时设置 ``ESP_INTR_FLAG_IRAM``。

请确保中断处理程序访问的所有数据和函数（包括其调用的数据和函数）都存储在 IRAM 或 DRAM 中。参见 :ref:`how-to-place-code-in-iram`。

在函数或符号未被正确放入 IRAM/DRAM 的情况下，中断处理程序在 flash 操作期间从 flash cache 中读取数据时，会导致程序崩溃。这可能是因为代码未被正确放入 IRAM 而产生非法指令异常，也可能是因为常数未被正确放入 DRAM 而读取到垃圾数据。

.. note::

    在 ISRs 中处理字符串时，不建议使用 `printf` 和其他输出函数。为了方便调试，在从 ISRs 中获取数据时，请使用 :cpp:func:`ESP_DRAM_LOGE` 和类似的宏。请确保 ``TAG`` 和格式字符串都放置于 ``DRAM`` 中。

非 IRAM 安全中断处理程序
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

如果在注册时没有设置 `ESP_INTR_FLAG_IRAM` 标志，当 cache 被禁用时，将不会执行中断处理程序。一旦 cache 恢复，非 IRAM 安全的中断将重新启用，中断处理程序随即再次正常运行。这意味着，只要 cache 被禁用，将不会发生相应的硬件事件。


.. only:: esp32c3

   .. include:: auto_suspend.inc

.. only:: SOC_SPIRAM_XIP_SUPPORTED

   .. include:: xip_from_psram.inc