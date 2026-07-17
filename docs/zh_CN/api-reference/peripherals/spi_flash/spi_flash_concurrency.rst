.. _concurrency-constraints-flash:

SPI0/1 上 Flash 的并发约束
===========================================

:link_to_translation:`zh_CN:[中文]`

SPI0/1 总线在缓存和 SPI1 外设（由包括此 SPI Flash 驱动在内的驱动程序控制）之间共享。对 SPI1 的操作可能会对缓存以及整个系统造成重大影响。连接到其他 SPI 总线的 flash 芯片没有此类约束和影响，不在本文档的讨论范围中。

SPI0/1 总线上可能发生三种活动：

- Flash 写入操作（通过 SPI1）。例如，擦除、页面编程或状态寄存器写入命令（例如，``SE``、``PP`` 和 ``WRSR``）。在这些命令期间，flash 处于不可读状态。CPU 和缓存必须等待直到写入命令完成。以下 API 可以触发写入命令：

    - 调用非加密 SPI flash 写入 API（:cpp:func:`esp_flash_write`、:cpp:func:`esp_flash_erase_region` 等）
    - 调用 :cpp:func:`esp_flash_write_encrypted`

- 短操作（通过 SPI1，包括非写入 flash 命令）。以下 API 可以触发短操作：

    .. list::

        - 调用非加密 SPI flash 读取 API（:cpp:func:`esp_flash_read` 等）
        :esp32: - 或 SPI1 总线上的其他驱动程序用于用户定义的 SPI 操作（启用实验性功能 :ref:`CONFIG_SPI_FLASH_SHARE_SPI1_BUS`）

- 缓存读取（通过 SPI0）。以下 API 和操作可以触发缓存读取：

    - 从 SPI Flash 或 PSRAM 执行代码
    - 从 SPI Flash 或 PSRAM 获取 .data/.rodata/.bss 段的静态数据
    - 通过堆或 `esp_himem` 对 PSRAM 的所有其他读/写操作
    - 从映射到 SPI Flash 的区域读取，包括：

        - 类似 mmap 的函数：:cpp:func:`spi_flash_mmap`、:cpp:func:`spi_flash_mmap_pages`、:cpp:func:`esp_mmu_map`、:cpp:func:`bootloader_mmap` 和 :cpp:func:`esp_partition_mmap`。
        - 依赖 :cpp:func:`spi_flash_mmap` 的函数：:cpp:func:`esp_partition_find`、:cpp:func:`esp_partition_register_external`。
        - 加密 flash 读/写 API：:cpp:func:`esp_flash_read_encrypted` 和 :cpp:func:`esp_flash_write_encrypted` （在 esp32 上，或用于数据验证）。

.. only:: esp32

    在所有 SPI1 操作期间缓存会被禁用，因此无法访问 Flash/PSRAM，大多数任务将被禁用。有关更多详细信息，请参阅 :ref:`cache_disabled`。

.. only:: not esp32

    所有 SPI flash API 通过驱动程序提供的某些内部互斥锁实现互斥访问。

    对于所有 SPI1 操作（读/写），默认情况下在这些操作期间缓存会被禁用，因此无法访问 Flash/PSRAM，大多数任务将被禁用。有关更多详细信息，请参阅 :ref:`cache_disabled`。


.. only:: SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND or SOC_SPIRAM_XIP_SUPPORTED

    有一些选项可以帮助减轻缓存禁用的影响。写入操作的影响在不同模式下是不同的。

    .. only:: SOC_SPIRAM_XIP_SUPPORTED

        - **XIP from PSRAM**：在此模式下，所有过去从 Flash 执行的段都改为从 PSRAM 加载和执行。因此，缓存能在 flash 擦除/写入期间保持启用状态，代码执行在大多数情况下不会受到写入操作的影响。有关更多详细信息，请参阅 :ref:`xip_from_psram`。

    .. only:: SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND

        - **Auto Suspend**：在此模式下，当 flash 区域发生缓存未命中时，允许暂停 flash 写入以透明地从中读取，但会有一些延迟。因此，缓存保持启用状态，代码在写入操作期间影响不会很大。

            这是一个可选功能，依赖于特殊的 SPI Flash 型号，因此默认禁用。有关更多详细信息，请参阅 :doc:`spi_flash_optional_feature` 和 :ref:`auto-suspend`。


有关软件实现的详细信息，请参阅 :ref:`esp_flash_os_func` 和 :ref:`spi_bus_lock`。


.. _cache_disabled:

缓存禁用（默认）
------------------------

.. only:: esp32

    在 SPI1 操作期间缓存会被禁用。所有 SPI1 操作将自动透明地禁用缓存。

.. only:: not esp32

    默认情况下，在 SPI1 操作期间缓存会被禁用。所有 SPI1 操作将自动透明地禁用缓存。

.. only:: SOC_HP_CPU_HAS_MULTIPLE_CORES

    当禁用缓存时，所有非 IRAM 安全的中断将被禁用，所有其他任务将被暂停。另一个核心将在一个忙循环中空转。只有 IRAM 安全的中断处理程序将被执行。这些将在 Flash 操作完成时恢复。

.. only:: not SOC_HP_CPU_HAS_MULTIPLE_CORES

    当禁用缓存时，所有非 IRAM 安全的中断将被禁用，所有其他任务将被暂停。只有 IRAM 安全的中断处理程序将被执行。这些将在 Flash 操作完成时恢复。

有关如何在禁用缓存时防止中断处理程序被禁用的信息，请参阅 :ref:`iram-safe-interrupt-handlers`。

当禁用缓存时，所有 CPU 应该只从内部 RAM 执行代码和访问数据。有关内部 RAM（例如，IRAM、DRAM）和 flash 缓存之间的差异，请参阅 :ref:`应用程序内存布局 <memory-layout>` 文档。


.. _iram-safe-interrupt-handlers:

IRAM 安全中断处理程序
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

如果需要在 flash 操作期间运行中断处理程序（比如低延迟操作），请在 :doc:`注册中断处理程序 </api-reference/system/intr_alloc>` 时设置 ``ESP_INTR_FLAG_IRAM``。

请确保中断处理程序访问的所有数据和函数（包括其调用的数据和函数）都存储在 IRAM 或 DRAM 中。参见 :ref:`how-to-place-code-in-iram`。

在未将函数或符号正确放入 IRAM/DRAM 的情况下，在 flash 操作期间，中断处理程序从 flash cache 中读取数据时，会导致程序崩溃。这可能是因为代码未正确放入 IRAM，产生了非法指令异常，也可能是因为常数未正确放入 DRAM，读取到了垃圾数据。

.. note::

    在 ISRs 中处理字符串时，不建议使用 ``printf`` 和其他输出函数。为了方便调试，在从 ISRs 中获取数据时，请使用 :cpp:func:`ESP_DRAM_LOGE` 和类似的宏。请确保 ``TAG`` 和格式字符串都放置于 ``DRAM`` 中。

非 IRAM 安全中断处理程序
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

如果在注册时没有设置 ``ESP_INTR_FLAG_IRAM`` 标志，当禁用 cache 时，将不会执行中断处理程序。一旦 cache 恢复，非 IRAM 安全的中断将重新启用，中断处理程序随即再次正常运行。这意味着，只要禁用了 cache，就不会发生相应的硬件事件。

.. only:: SOC_DMA_CAN_ACCESS_FLASH

    当 DMA 从 Flash 读取数据时
    -----------------------------

    Flash 器件不允许在擦除/编程时读取，即使数据不在正在被擦除/编程的区域中。

    当 flash 正在被擦除/编程时，DMA 读取的 Flash 数据是不可预测的。建议在擦除或写入之前停止 DMA 对 Flash 的访问。如果无法停止 DMA（例如，LCD 需要持续刷新存储在 Flash 中的图像数据），建议将此类数据复制到 PSRAM 或内部 SRAM。


.. only:: SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND

   .. include:: auto_suspend.inc

.. only:: SOC_SPIRAM_XIP_SUPPORTED

   .. include:: xip_from_psram.inc
