Storage
=======

:link_to_translation:`zh_CN:[中文]`

FatFs
-----

``esp_vfs_fat_sdmmc_unmount()`` is now deprecated, you can use :cpp:func:`esp_vfs_fat_sdcard_unmount()` instead. This API is deprecated in previous IDF versions, but without deprecation warning and migration guide. Since IDF v5.1, calling this ``esp_vfs_fat_sdmmc_unmount()`` API will generate deprecation warning.
