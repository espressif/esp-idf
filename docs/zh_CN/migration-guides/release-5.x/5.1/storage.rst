存储
=======

:link_to_translation:`en:[English]`

FatFs
-----

``esp_vfs_fat_sdmmc_unmount()`` 已弃用，可以使用 :cpp:func:`esp_vfs_fat_sdcard_unmount()` 代替。此接口在更早的 ESP-IDF 版本中已弃用，但是尚未添加弃用警告。自 ESP-IDF v5.1 起，调用 ``esp_vfs_fat_sdmmc_unmount()`` 接口将会产生弃用警告。


SPI_FLASH
---------

- :cpp:func:`spi_flash_get_counters` 已弃用，请使用 :cpp:func:`esp_flash_get_counters` 代替。
- :cpp:func:`spi_flash_dump_counters` 已弃用，请使用 :cpp:func:`esp_flash_dump_counters` 代替。
- :cpp:func:`spi_flash_reset_counters` 已弃用，请使用 :cpp:func:`esp_flash_reset_counters` 代替。
