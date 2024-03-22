.. _xip_from_psram:

从 PSRAM 执行代码功能
----------------------

选择 :ref:`CONFIG_SPIRAM_XIP_FROM_PSRAM` 配置以启用此模式。在此模式下，代码从 PSRAM 执行，在大多数情况下，缓存不会在写入 API 期间被禁用。

在此模式下，flash ``.text`` 段（用于指令）和 flash ``.rodata`` 段（用于只读数据）将在启动时加载到 PSRAM。相应的虚拟地址将映射到 PSRAM。您无需确保在 flash 被擦除/编程时执行的代码/数据位于 IRAM 中。

例外：当缓存映射 flash 中的区域时
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

由于 SPI Nor Flash 器件的限制，在 flash 被擦除/写入时，仍然不允许访问 Flash 中的缓存映射区域（通过 spi_flash_mmap 等 API 映射），无论擦写区域和映射区域是否重合。在这种情况下，在擦写 Flash 时仍应禁用缓存以防止从缓存读取错误的数据。

为了防止缓存禁用，在 SPI Flash 驱动程序中实现了一个锁，以确保缓存映射的存在和 Flash 写入是互斥的，大多数进行了 Flash 映射的 ESP-IDF API 都使用了该标志。如果您自己调用类似 mmap 的 API，可以指定此标志 :cpp:enumerator:`SPI_FLASH_MMAP_FLAG_BLOCKS_WRITE` 以防止缓存禁用。您不能在 ``spi_flash_mmap`` 和 ``spi_flash_munmap`` 之间使用 ``esp_flash_erase_*`` 或者 ``esp_flash_write`` 的任务中使用此标志（无论写入区域和映射区域是否重叠），否则会造成死锁。有关该标志的详细信息，请参阅 :ref:`blocks_write_flag`。

如果在没有此标志的情况下调用类似 mmap 的 API，当 Flash 发生擦除或者写入时，缓存仍将被禁用。
