SPI flash API
=================

:link_to_translation:`en:[English]`

概述
--------

spi_flash 组件提供外部 flash 数据读取、写入、擦除和内存映射相关的 API 函数。

关于更多高层次的用于访问分区（分区表定义于 :doc:`分区表 </api-guides/partition-tables>`）的 API 函数，参见 :doc:`/api-reference/storage/partition` 。

.. note::

    访问主 flash 芯片时，建议使用上述 ``esp_partition_*`` API 函数，而非低层级的 ``esp_flash_*`` API 函数。分区表 API 函数根据存储在分区表中的数据，进行边界检查并计算在 flash 中的正确偏移量。不过，仍支持使用 ``esp_flash_*`` 函数直接访问外部（额外）的 SPI flash 芯片。

与 ESP-IDF v4.0 之前的 API 不同，这一版 ``esp_flash_*`` API 功能并不局限于主 SPI flash 芯片（即运行程序的 SPI flash 芯片）。通过使用不同的芯片指针，可以访问连接到 SPI0/1 或 SPI2 总线的外部 flash 芯片。

.. note::

    大多数 ``esp_flash_*`` API 使用 SPI1，SPI2 等外设而非通过 SPI0 上的 cache。这使得它们不仅能访问主 flash，也能访问外部 flash。

    而由于 cache 的限制，所有经过 cache 的操作都只能对主 flash 进行。这些操作的地址同样受到 cache 能力的限制。Cache 无法访问外部 flash 或者高于它能力的地址段。这些 cache 操作包括：mmap、加密读写、执行代码或者访问在 flash 中的变量。

.. note::

    ESP-IDF v4.0 之后的 flash API 不再是 **原子** 的。因此，如果读操作执行过程中发生写操作，且读操作和写操作的 flash 地址出现重叠，读操作返回的数据可能会包含旧数据和新数据（新数据为写操作更新产生的数据）。

.. note::

    仅有主 flash 芯片支持加密操作，外接（经 SPI1 使用其他不同片选访问，或经其它 SPI 总线访问）的 flash 芯片则不支持加密操作。硬件的限制也决定了仅有主 flash 支持从 cache 当中读取。

flash 功能支持情况
-----------------------------------

支持的 flash 列表
^^^^^^^^^^^^^^^^^^^^^

不同厂家的 flash 特性有不同的操作方式，因此需要特殊的驱动支持。当前驱动支持大多数厂家 flash 24 位地址范围内的快速/慢速读，以及二线模式 (DIO/DOUT)，因为他们不需要任何厂家的自定义命令。

当前驱动支持以下厂家/型号的 flash 的四线模式 (QIO/QOUT)：

1. ISSI
2. GD
3. MXIC
4. FM
5. Winbond
6. XMC
7. BOYA

.. note::

    只有 {IDF_TARGET_NAME} 支持上述某个 flash 时，芯片的驱动才默认支持这款 flash。可使用 menuconfig 中的 ``Component config`` > ``SPI flash driver`` > ``Auto-detect flash chips`` 选项来使能/禁用某个 flash。

flash 可选的功能
^^^^^^^^^^^^^^^^^^^^

.. toctree::
    :hidden:

    spi_flash_optional_feature

有一些功能可能不是所有的 flash 芯片都支持，或不是所有的 ESP 芯片都支持。这些功能包括：

.. only:: esp32s3

    -  OPI flash - 表示 flash 支持 8 线模式。

-  32 比特地址的 flash 支持 - 通常意味着拥有大于 16 MB 内存空间的大容量 flash 需要更长的地址去访问。

.. only:: esp32s3

    -  高性能 (HPM) 模式 - 表示 flash 工作频率大于 80 MHz。

-  flash 的私有 ID (unique ID) - 表示 flash 支持它自己的 64-bit 独有 ID。

.. only:: SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND

    -  暂停与恢复 - 表示 flash 可以在读/写的过程中接受暂停/恢复的命令。{IDF_TARGET_NAME} 可以在 flash 正在写/擦除的过程中保持 cache 开启，并能随机读取 flash 中的内容。

如果想使用这些功能，则需保证 {IDF_TARGET_NAME} 支持这些功能，且产品里所使用的 flash 芯片也要支持这些功能。请参阅 :doc:`spi_flash_optional_feature`，查看更多信息。

也可以自定义 flash 芯片驱动。请参阅 :doc:`spi_flash_override_driver`，查看详细信息。

.. toctree::
   :hidden:

   自定义 flash 芯片驱动 <spi_flash_override_driver>

初始化 flash 设备
---------------------------

在使用 ``esp_flash_*`` API 之前，需要在 SPI 总线上初始化芯片，步骤如下：

1. 调用 :cpp:func:`spi_bus_initialize` 初始化 SPI 总线。此函数将初始化总线上设备间共享的资源，如 I/O、DMA、中断等。

2. 调用 :cpp:func:`spi_bus_add_flash_device` 将 flash 设备连接到总线上。然后分配内存，填充 ``esp_flash_t`` 结构体，同时初始化 CS I/O。

3. 调用 :cpp:func:`esp_flash_init` 与芯片进行通信。后续操作会依据芯片类型不同而有差异。

.. note:: 当前，已支持多个 flash 芯片连接到同一总线。

SPI flash 访问 API
--------------------

如下所示为处理 flash 中数据的函数集：

- :cpp:func:`esp_flash_read`：将数据从 flash 读取到 RAM；
- :cpp:func:`esp_flash_write`：将数据从 RAM 写入到 flash；
- :cpp:func:`esp_flash_erase_region`：擦除 flash 中指定区域的数据；
- :cpp:func:`esp_flash_erase_chip`：擦除整个 flash；
- :cpp:func:`esp_flash_get_chip_size`：返回 menuconfig 中设置的 flash 芯片容量（以字节为单位）。

一般来说，请尽量避免对主 SPI flash 芯片直接使用原始 SPI flash 函数。如需对主 SPI flash 芯片进行操作，请使用 :ref:`分区专用函数 <flash-partition-apis>`。

SPI flash 容量
--------------

SPI flash 容量由引导加载程序镜像头部（烧录偏移量为 0x1000）的一个字段进行配置。

默认情况下，引导程序被写入 flash 时，``esptool.py`` 会自动检测 SPI flash 容量，同时使用正确容量更新引导程序的头部。也可以在工程配置中设置 :ref:`CONFIG_ESPTOOLPY_FLASHSIZE`，生成固定的 flash 容量。

如需在运行时覆盖已配置的 flash 容量，请配置 ``g_rom_flashchip`` 结构中的 ``chip_size``。``esp_flash_*`` 函数使用此容量（于软件和 ROM 中）进行边界检查。

SPI1 flash 并发约束
-----------------------------------------

.. toctree::
    :hidden:

    spi_flash_concurrency

.. attention::

    指令/数据 cache（用以执行固件）与 SPI1 外设（由像 SPI flash 驱动一样的驱动程序控制）共享 SPI0/1 总线。因此，在 SPI1 总线上调用 SPI flash API（包括访问主 flash）会对整个系统造成显著的影响。请参阅 :doc:`spi_flash_concurrency`，查看详细信息。


SPI flash 加密
--------------------

SPI flash 内容支持加密，并在硬件层进行透明解密。

请参阅 :doc:`flash 加密 </security/flash-encryption>`，查看详细信息。

内存映射 API
------------------

{IDF_TARGET_CACHE_SIZE:default="64 KB",esp32c2=16~64 KB}

{IDF_TARGET_NAME} 的内存硬件可以将 flash 部分区域映射到指令地址空间和数据地址空间。此映射仅用于读操作，不能通过写入 flash 映射的存储区域来改变 flash 中的内容。

flash 在 {IDF_TARGET_CACHE_SIZE} 页进行映射。内存映射硬件既可将 flash 映射到数据地址空间，也能映射到指令地址空间。请查看技术参考手册，了解内存映射硬件的详细信息及有关限制。

请注意，有些页被用于将应用程序映射到内存中，因此实际可用的页会少于硬件提供的总数。

启用 :doc:`flash 加密 </security/flash-encryption>` 时，使用内存映射区域从 flash 读取数据是解密 flash 的唯一方法，解密需在硬件层进行。

内存映射 API 在 ``spi_flash_mmap.h`` 和 ``esp_partition.h`` 中声明：

- :cpp:func:`spi_flash_mmap`：将 flash 物理地址区域映射到 CPU 指令空间或数据空间；
- :cpp:func:`spi_flash_munmap`：取消上述区域的映射；
- :cpp:func:`esp_partition_mmap`：将分区的一部分映射至 CPU 指令空间或数据空间；

 :cpp:func:`spi_flash_mmap` 和 :cpp:func:`esp_partition_mmap` 的区别如下：

- :cpp:func:`spi_flash_mmap`：需要给定一个 {IDF_TARGET_CACHE_SIZE} 对齐的物理地址；
- :cpp:func:`esp_partition_mmap`：给定分区内任意偏移量即可，此函数根据需要将返回的指针调整至指向映射内存。

内存映射以页为单位，即使传递给 ``esp_partition_mmap`` 的是一个分区，分区外的数据也是也是可以被读取到的，不会受到分区边界的影响。

.. note::

    由于 mmap 是由 cache 支持的，因此，mmap 也仅能用在主 flash 上。

SPI flash 实现
--------------

``esp_flash_t`` 结构体包含芯片数据和该 API 的三个重要部分：

1. 主机驱动，为访问芯片提供硬件支持；
2. 芯片驱动，为不同芯片提供兼容性服务；
3. OS 函数，在不同阶段（一级或二级 Boot 或者应用程序阶段）为部分 OS 函数（如锁、延迟）提供支持。

主机驱动
^^^^^^^^^^^^^^^

主机驱动依赖 ``hal/include/hal`` 文件夹下 ``spi_flash_types.h`` 定义的 ``spi_flash_host_driver_t`` 接口。该接口提供了一些常用的函数，用于与芯片通信。

在 SPI HAL 文件中，有些函数是基于现有的 {IDF_TARGET_NAME} memory-spi 来实现的。但是，由于 {IDF_TARGET_NAME} 的速度限制，HAL 层无法提供某些读命令的高速实现（所以这些命令根本没有在 HAL 的文件中被实现）。``memspi_host_driver.h`` 和 ``.c`` 文件使用 HAL 提供的 ``common_command`` 函数实现上述读命令的高速版本，并将所有它实现的以及 HAL 函数封装为 ``spi_flash_host_driver_t`` 供更上层调用。

仅通过 GPIO，也可实现自己的主机驱动。只要实现了 ``spi_flash_host_driver_t`` 中所有函数，不管底层硬件是什么，esp_flash API 都可以访问 flash。

芯片驱动
^^^^^^^^^^^

芯片驱动在 ``spi_flash_chip_driver.h`` 中进行定义，并将主机驱动提供的基本函数进行封装以供 API 层使用。

有些操作需在执行前先发送命令，或在执行后读取状态，因此有些芯片需要不同的命令或值以及通信方式。

``generic chip`` 芯片代表了常见的 flash 芯片，其他芯片驱动可以在这种通用芯片的基础上进行开发。

芯片驱动依赖主机驱动。

.. _esp_flash_os_func:

OS 函数
^^^^^^^^^^^^

.. toctree::
   :hidden:

   ../spi_features

OS 函数层目前支持访问锁和延迟的方法。

锁（见 :ref:`spi_bus_lock`）用于解决同一 SPI 总线上的设备访问和 SPI flash 芯片访问之间的冲突。例如：

1. 经 SPI1 总线访问 flash 芯片时，应当禁用 cache（平时用于获取代码和 PSRAM 数据）。

2. 经其他总线访问 flash 芯片时，应当禁用 flash 上 SPI 主驱动器注册的 ISR 以避免冲突。

3. SPI 主驱动器上某些没有 CS 线或者 CS 线受软件（如 SDSPI）控制的设备需要在一段时间内独占总线。

延时则用于某些长时操作，需要主机处于等待状态或执行轮询。

顶层 API 将芯片驱动和 OS 函数封装成一个完整的组件，并提供参数检查。

使用 OS 函数还可以在一定程度上避免在擦除大块 flash 区域时出现看门狗超时的情况。在这段时间内，CPU 将被 flash 擦除任务占用，从而阻止其他任务的执行，包括为看门狗定时器 (WDT) 供电的空闲任务。若已选中配置选项 :ref:`CONFIG_ESP_TASK_WDT_PANIC`，并且 flash 操作时间长于看门狗的超时时间，系统将重新启动。

不过，由于不同的 flash 芯片擦除时间不同，flash 驱动几乎无法兼容，很难完全规避超时的风险，这一点需要格外注意。请遵照以下指南：

1. 建议启用 :ref:`CONFIG_SPI_FLASH_YIELD_DURING_ERASE` 选项，允许调度器在擦除 flash 时进行重新调度。此外，还可以使用下列参数。

- 在 menuconfig 中增加 :ref:`CONFIG_SPI_FLASH_ERASE_YIELD_TICKS` 或减少 :ref:`CONFIG_SPI_FLASH_ERASE_YIELD_DURATION_MS` 的时间。
- 在 menuconfig 中增加 :ref:`CONFIG_ESP_TASK_WDT_TIMEOUT_S` 的时间，以设置更长的看门狗超时周期。然而，看门狗超时周期拉长后，可能无法再检测到以前可检测到的超时。

1. 请注意，在进行长时间的 SPI flash 操作时，启用 :ref:`CONFIG_ESP_TASK_WDT_PANIC` 选项将会在超时时触发紧急处理程序。不过，启用该选项也可以帮助处理应用程序中的意外异常，请根据实际情况决定是否需要启用这个选项。

2. 在开发过程中，请根据项目对擦除 flash 的具体要求和时间限制，谨慎进行 flash 操作。在配置 flash 擦除超时周期时，请在实际产品要求的基础上留出合理的冗余时间，从而提高产品的可靠性。

.. _spi-flash-implementation-details:

实现细节
------------

必须确保操作期间，两个 CPU 均未从 flash 运行代码，实现细节如下：

- 单核模式下，SDK 在执行 flash 操作前将禁用中断或调度算法。
- 双核模式下，SDK 需确保两个 CPU 均未运行 flash 代码。

如果有 SPI flash API 在 CPU A（PRO 或 APP）上调用，它使用 ``esp_ipc_call`` API 在 CPU B 上运行 ``spi_flash_op_block_func`` 函数。``esp_ipc_call`` API 会在 CPU B 上唤醒一个高优先级任务，即运行 ``spi_flash_op_block_func`` 函数。运行该函数将禁用 CPU B 上的 cache，并使用 ``s_flash_op_can_start`` 旗帜来标志 cache 已禁用。然后，CPU A 上的任务也会禁用 cache 并继续执行 flash 操作。

执行 flash 操作时，CPU A 和 CPU B 仍然可以执行中断操作。默认中断代码均存储于 RAM 中，如果新添加了中断分配 API，则应添加一个标志位以请求在 flash 操作期间禁用该新分配的中断。

flash 操作完成后，CPU A 上的函数将设置另一标志位，即 ``s_flash_op_complete``，用以通知 CPU B 上的任务可以重新启用 cache 并释放 CPU。接着，CPU A 上的函数也重新启用 cache，并将控制权返还给调用者。

另外，所有 API 函数均受互斥量 ``s_flash_op_mutex`` 保护。

在单核环境中（启用 :ref:`CONFIG_FREERTOS_UNICORE`），需要禁用上述两个 cache，以防发生 CPU 间通信。

相关文档
-----------------

.. list::

    - :doc:`spi_flash_optional_feature`
    - :doc:`spi_flash_concurrency`
    :CONFIG_ESP_ROM_HAS_SPI_FLASH: - :doc:`spi_flash_idf_vs_rom`

.. toctree::
    :hidden:

    spi_flash_idf_vs_rom


SPI flash API 参考
-------------------------

.. include-build-file:: inc/esp_flash_spi_init.inc
.. include-build-file:: inc/esp_flash.inc
.. include-build-file:: inc/spi_flash_mmap.inc
.. include-build-file:: inc/spi_flash_types.inc
.. include-build-file:: inc/esp_flash_err.inc
.. include-build-file:: inc/esp_spi_flash_counters.inc

flash 加密 API 参考
-----------------------------

.. include-build-file:: inc/esp_flash_encrypt.inc
