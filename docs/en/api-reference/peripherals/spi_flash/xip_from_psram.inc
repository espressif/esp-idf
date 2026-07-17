.. _xip_from_psram:

Executing Code from PSRAM
-------------------------

Select :ref:`CONFIG_SPIRAM_XIP_FROM_PSRAM` config to enable this mode. In this mode, code is executed from PSRAM, and the cache will not be disabled during write APIs in most cases.

In this mode, the flash ``.text`` sections (used for instructions) and the flash ``.rodata`` sections (used for read-only data) will be loaded into PSRAM at startup. The corresponding virtual addresses will be mapped to PSRAM. You do not need to ensure that code and data executed while the flash is being erased or programmed reside in IRAM.

Exception: Cache-Mapped Regions in Flash
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Due to the restriction from SPI Nor Flash parts, access to cache mapped regions in flash (mapped via APIs like spi_flash_mmap) is still not allowed while the flash is being erased/written, regardless of whether the erase/write region and the mapped region overlap. In this case, cache should still be disabled to prevent reading corrupted data from the cache.

To prevent cache disabling, a lock is implemented inside the SPI Flash driver to ensure mutual exclusion between cache mapping and flash writing, and most ESP-IDF APIs that perform flash mapping use this flag. If mmap-like APIs are called by yourself, you can specify this flag :cpp:enumerator:`SPI_FLASH_MMAP_FLAG_BLOCKS_WRITE` to prevent cache disabling. You cannot use this flag in a task that uses ``esp_flash_erase_*`` or ``esp_flash_write`` between ``spi_flash_mmap`` and ``spi_flash_munmap`` (regardless of whether the write region and mapped region overlap), otherwise it will cause a deadlock. See :ref:`blocks_write_flag` for more details about the flag.

If mmap-like APIs are called without this flag, the cache will still be disabled when flash erasing or writing happens.
