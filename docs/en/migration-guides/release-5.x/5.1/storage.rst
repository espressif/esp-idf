Storage
=======

:link_to_translation:`zh_CN:[中文]`

FatFs
-----

``esp_vfs_fat_sdmmc_unmount()`` is now deprecated, and you can use :cpp:func:`esp_vfs_fat_sdcard_unmount()` instead. This API is deprecated in previous ESP-IDF versions, but without a deprecation warning or migration guide. Since ESP-IDF v5.1, calling this ``esp_vfs_fat_sdmmc_unmount()`` API will generate a deprecation warning.


SPI_FLASH
---------

- :cpp:func:`spi_flash_get_counters` is deprecated, please use :cpp:func:`esp_flash_get_counters` instead.
- :cpp:func:`spi_flash_dump_counters` is deprecated, please use :cpp:func:`esp_flash_dump_counters` instead.
- :cpp:func:`spi_flash_reset_counters` is deprecated, please use :cpp:func:`esp_flash_reset_counters` instead.
