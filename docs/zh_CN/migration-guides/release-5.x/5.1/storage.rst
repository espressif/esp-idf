存储
=======

:link_to_translation:`en:[English]`

FatFs
-----

``esp_vfs_fat_sdmmc_unmount()`` 已被弃用。 您可以使用 :cpp:func:`esp_vfs_fat_sdcard_unmount()` 代替。 此接口在更早的 IDF 版本中已被弃用， 但是尚未添加弃用警告。 自 IDF v5.1 起， 调用这个 ``esp_vfs_fat_sdmmc_unmount()`` 接口将会产生 deprecation 警告。
