Storage
=======

:link_to_translation:`zh_CN:[中文]`

FatFs
-----

``esp_vfs_fat_sdmmc_unmount()`` is now deprecated, you can use :cpp:func:`esp_vfs_fat_sdcard_unmount()` instead. This API is deprecated in previous IDF versions, but without deprecation warning and migration guide. Since IDF v5.1, calling this ``esp_vfs_fat_sdmmc_unmount()`` API will generate deprecation warning.


SPI_FLASH
---------

- :cpp:func:`spi_flash_get_counters` is deprecated, please use :cpp:func:`esp_flash_get_counters` instead.
- :cpp:func:`spi_flash_dump_counters` is deprecated, please use :cpp:func:`esp_flash_dump_counters` instead.
- :cpp:func:`spi_flash_reset_counters` is deprecated, please use :cpp:func:`esp_flash_reset_counters` instead.

SPI Flash Driver
^^^^^^^^^^^^^^^^

XMC-C series flash suspend support has been removed. According to feedback from the flash manufacturer, in some situations the XMC-C flash would require a 1ms interval between resume and next command. This is too long for a software request. Based on the above reason, in order to use suspend safely, we decide to remove flash suspend support from XMC-C series. But you can still force enable it via `CONFIG_SPI_FLASH_FORCE_ENABLE_XMC_C_SUSPEND`. If you have any questions, please contact espressif business support.