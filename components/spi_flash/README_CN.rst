SPI Flash API
=================

:link_to_translation:`en:[English]`

概述
--------

SPI Flash 组件提供外部 flash 数据读取、写入、擦除和内存映射相关的 API 函数，同时也提供了更高层级的，面向分区的 API 函数（定义在 :doc:`分区表 </api-guides/partition-tables>` 中）。

与 ESP-IDF V4.0 之前的 API 不同，这一版 API 功能并不局限于主 SPI Flash 芯片（即运行程序的 SPI Flash 芯片）。使用不同的芯片指针，您可以通过 SPI0/1 或 HSPI/VSPI 总线访问外部 flash。

.. note::

    ESP-IDF V4.0 之后的 flash API 不再是原子的。因此，如果 flash 操作地址有重叠，且写操作与读操作同时执行，读操作可能会返回一部分写入之前的数据，返回一部分写入之后的数据。

Kconfig 选项 :ref:`CONFIG_SPI_FLASH_USE_LEGACY_IMPL` 可将 ``spi_flash_*`` 函数切换至 ESP-IDF V4.0 之前的实现。但是，如果同时使用新旧 API，代码量可能会增多。

即便未启用 :ref:`CONFIG_SPI_FLASH_USE_LEGACY_IMPL`，加密读取和加密写入操作也均使用旧实现。因此，仅有主 flash 芯片支持加密操作，其他不同片选（经 SPI1 访问的 flash 芯片）则不支持加密操作。

初始化 Flash 设备
---------------------------

在使用 ``esp_flash_*`` API 之前，您需要在 SPI 总线上初始化芯片。

1. 调用 :cpp:func:`spi_bus_initialize` 初始化 SPI 总线，此函数将初始化总线上设备间共享的资源，如 I/O、DMA 及中断等。

2. 调用 :cpp:func:`spi_bus_add_flash_device` 将 flash 设备连接到总线上。然后分配内存，填充 ``esp_flash_t`` 结构体，同时初始化 CS I/O。

3. 调用 :cpp:func:`esp_flash_init` 与芯片进行通信。后续操作会依据芯片类型不同而有差异。

.. note:: 目前，多个 flash 芯片可连接到同一总线。但尚不支持在同一个 SPI 总线上使用 ``esp_flash_*`` 和 ``spi_device_*`` 设备。

SPI Flash 访问 API
--------------------

如下所示为处理 flash 中数据的函数集：

- :cpp:func:`esp_flash_read`：将数据从 flash 读取到 RAM；
- :cpp:func:`esp_flash_write`：将数据从 RAM 写入到 flash；
- :cpp:func:`esp_flash_erase_region`：擦除 flash 中指定区域的数据；
- :cpp:func:`esp_flash_erase_chip`：擦除整个 flash；
- :cpp:func:`esp_flash_get_chip_size`：返回 menuconfig 中设置的 flash 芯片容量（以字节为单位）。

一般来说，请尽量避免对主 SPI flash 芯片直接使用原始 SPI flash 函数，如需对主 SPI flash 芯片进行操作，请使用 :ref:`分区专用函数 <flash-partition-apis>`。

SPI Flash 容量
--------------

SPI flash 容量存储于引导程序映像头部（烧录偏移量为 0x1000）的一个字段。

默认情况下，引导程序写入 flash 时，esptool.py 将引导程序写入 flash 时，会自动检测 SPI flash 容量，同时使用正确容量更新引导程序的头部。您也可以在工程配置中设置 :envvar:`CONFIG_ESPTOOLPY_FLASHSIZE`，生成固定的 flash 容量。

如需在运行时覆盖已配置的 flash 容量，请配置 ``g_rom_flashchip`` 结构中的 ``chip_size``。``esp_flash_*`` 函数使用此容量（于软件和 ROM 中）进行边界检查。

SPI1 Flash 并发约束
-----------------------------------------

由于 SPI1 flash 也被用于执行固件（通过指令 cache 或数据 cache ），因此在执行读取、写入及擦除操作时，必须禁用这些 cache。这意味着在执行 flash 写操作时，两个 CPU 必须从 IRAM 运行代码，且只能从 DRAM 中读取数据。

如果您使用本文档中 API 函数，上述限制将自动生效且透明（无需您额外关注），但这些限制可能会影响系统中的其他任务的性能。

除 SPI0/1 以外的 SPI 总线上的其它 flash 芯片则不受这种限制。

请参阅 :ref:`应用程序内存分布 <memory-layout>`，查看 IRAM、DRAM 和 flash cache 的区别。

为避免意外读取 flash cache，一个 CPU 在启动 flash 写入或擦除操作时，另一个 CPU 将阻塞，并且在 flash 操作完成前，两个 CPU 上的所有的非 IRAM 安全的中断都会被禁用。

.. _iram-safe-interrupt-handlers:

IRAM 安全中断处理程序
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

如果您需要在 flash 操作期间运行中断处理程序（比如低延迟操作），请在 :doc:`注册中断处理程序 </api-reference/system/intr_alloc>` 时设置 ``ESP_INTR_FLAG_IRAM``。

请确保中断处理程序访问的所有数据和函数（包括其调用的数据和函数）都存储在 IRAM 或 DRAM 中。

为函数添加 ``IRAM_ATTR`` 属性::

    #include "esp_attr.h"

    void IRAM_ATTR gpio_isr_handler(void* arg)
    {
        // ...
    }


为常量添加 ``DRAM_ATTR`` 和 ``DRAM_STR`` 属性::

    void IRAM_ATTR gpio_isr_handler(void* arg)
    {
       const static DRAM_ATTR uint8_t INDEX_DATA[] = { 45, 33, 12, 0 };
       const static char *MSG = DRAM_STR("I am a string stored in RAM");
    }

辨别哪些数据应标记为 ``DRAM_ATTR`` 可能会比较困难，除非明确标记为 ``DRAM_ATTR``，否则编译器依然可能将某些变量或表达式当做常量（即便没有 ``const`` 标记），并将其放入 flash。

如果函数或符号未被正确放入 IRAM/DRAM 中，当中断处理程序在 flash 操作期间从 flash cache 中读取数据，则会产生非法指令异常（这是因为代码未被正确放入 IRAM）或读取垃圾数据（这是因为常数未被正确放入 DRAM），而导致崩溃。

.. _flash-partition-apis:

分区表 API
-------------------

ESP-IDF 工程使用分区表保存 SPI flash 各区信息，包括引导程序、各种应用程序二进制文件、数据及文件系统等。请参考 :doc:`分区表 </api-guides/partition-tables>`，查看详细信息。

该组件在 ``esp_partition.h`` 中声明了一些 API 函数，用以枚举在分区表中找到的分区，并对这些分区执行操作：

- :cpp:func:`esp_partition_find`：在分区表中查找特定类型的条目，返回一个不透明迭代器；
- :cpp:func:`esp_partition_get`：返回一个结构，描述给定迭代器的分区；
- :cpp:func:`esp_partition_next`：将迭代器移至下一个找到的分区；
- :cpp:func:`esp_partition_iterator_release`：释放 ``esp_partition_find`` 中返回的迭代器；
- :cpp:func:`esp_partition_find_first`：返回一个结构，描述 ``esp_partition_find`` 中找到的第一个分区；
- :cpp:func:`esp_partition_read`、:cpp:func:`esp_partition_write` 和 :cpp:func:`esp_partition_erase_range` 在分区边界内执行，等同于 :cpp:func:`spi_flash_read`、:cpp:func:`spi_flash_write` 和 :cpp:func:`spi_flash_erase_range`。

.. note::
    请在应用程序代码中使用上述 ``esp_partition_*`` API 函数，而非低层级的 ``spi_flash_*`` API 函数。分区表 API 函数根据存储在分区表中的数据，进行边界检查并计算在 flash 中的正确偏移量。

SPI Flash 加密
--------------------

您可以对 SPI flash 内容进行加密，并在硬件层对其进行透明解密。

请参阅 :doc:`Flash 加密 </security/flash-encryption>`，查看详细信息。

内存映射 API
------------------

ESP32 内存硬件可以将 flash 部分区域映射到指令地址空间和数据地址空间，此映射仅用于读操作。不能通过写入 flash 映射的存储区域来改变 flash 中内容。

Flash 以 64 KB 页为单位进行地址映射。内存映射硬件最多可将 4 MB flash 映射到数据地址空间，将 16 MB flash 映射到指令地址空间。请参考《ESP32 技术参考手册》查看内存映射硬件的详细信息。

请注意，有些 64 KB 页还用于将应用程序映射到内存中，因此实际可用的 64 KB 页会更少一些。

:doc:`Flash 加密 </security/flash-encryption>` 启用时，使用内存映射区域从 flash 读取数据是解密 flash 的唯一方法，解密需在硬件层进行。

内存映射 API 在 ``esp_spi_flash.h`` 和 ``esp_partition.h`` 中声明：

- :cpp:func:`spi_flash_mmap`：将 flash 物理地址区域映射到 CPU 指令空间或数据空间；
- :cpp:func:`spi_flash_munmap`：取消上述区域的映射；
- :cpp:func:`esp_partition_mmap`：将分区的一部分映射至 CPU 指令空间或数据空间；

 :cpp:func:`spi_flash_mmap` 和 :cpp:func:`esp_partition_mmap` 的区别如下：

- :cpp:func:`spi_flash_mmap`：需要给定一个 64 KB 对齐的物理地址；
- :cpp:func:`esp_partition_mmap`：给定分区内任意偏移量即可，此函数根据需要将返回的指针调整至指向映射内存。

内存映射在 64 KB 块中进行，如果分区已传递给 ``esp_partition_mmap``，则可读取分区外数据。

实现
--------------

``esp_flash_t`` 结构包含芯片数据和该 API 的三个重要部分：

1. 主机驱动，为访问芯片提供硬件支持；
2. 芯片驱动，为不同芯片提供兼容性服务；
3. OS 函数，在不同阶段（一级或二级 Boot 或者应用程序阶段）为部分 OS 函数提供支持（如一些锁、延迟）。

主机驱动
^^^^^^^^^^^^^^^

主机驱动依赖 ``soc/include/hal`` 文件夹下 ``spi_flash_host_drv.h`` 定义的 ``spi_flash_host_driver_t`` 接口。该接口提供了一些与芯片通信常用的函数。

在 SPI HAL 文件中，有些函数是基于现有的 ESP32 memory-spi 来实现的。但是，由于 ESP32 速度限制，HAL 层无法提供某些读命令的高速实现（所以这些命令根本没有在 HAL 的文件中被实现）。``memspi_host_driver.h`` 和 ``.c`` 文件使用 HAL 提供的 ``common_command`` 函数实现上述读命令的高速版本，并将所有它实现的及 HAL 函数封装为 ``spi_flash_host_driver_t`` 供更上层调用。

您也可以实现自己的主机驱动，甚至只通过简单的 GPIO。只要实现了 ``spi_flash_host_driver_t`` 中所有函数，不管底层硬件是什么，esp_flash API 都可以访问 flash。

芯片驱动
^^^^^^^^^^^

芯片驱动在 ``spi_flash_chip_driver.h`` 中进行定义，并将主机驱动提供的基本函数进行封装以供 API 层使用。

有些操作需在执行前先发送命令，或在执行后读取状态，因此有些芯片需要不同的命令或值以及通信方式。

``generic chip`` 芯片代表了常见的 flash 芯片，其他芯片驱动可以在通用芯片的基础上进行开发。

芯片驱动依赖主机驱动。

OS 函数
^^^^^^^^^^^^

OS 函数层提供访问锁和延迟的方法。

该锁定用于解决 SPI Flash 芯片访问和其他函数之间的冲突。例如，经 SPI0/1 访问 flash 芯片时，应当禁用 cache（平时用于取代码和 PSRAM 数据）。另一种情况是，一些没有 CS 线或者 CS 线受软件控制的设备（如通过 SPI 接口的 SD 卡控制）需要在一段时间内独占总线。

延时则用于某些长时操作，需要主机处于等待状态或执行轮询。

顶层 API 将芯片驱动和 OS 函数封装成一个完整的组件，并提供参数检查。
