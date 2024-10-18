存储
=======

:link_to_translation:`en:[English]`

FatFs
-----

``esp_vfs_fat_sdmmc_unmount()`` 已被弃用。 您可以使用 :cpp:func:`esp_vfs_fat_sdcard_unmount()` 代替。 此接口在更早的 IDF 版本中已被弃用， 但是尚未添加弃用警告。 自 IDF v5.1 起， 调用这个 ``esp_vfs_fat_sdmmc_unmount()`` 接口将会产生 deprecation 警告。

SPI flash 驱动
^^^^^^^^^^^^^^^^^^^^^^

XMC-C 系列闪存 suspend 功能的支持已被移除。根据闪存制造商的反馈，在某些情况下，XMC-C 闪存需要在 resume 和下一条命令之间间隔 1 毫秒。这对于软件请求来说时间太长。基于上述原因，为了安全使用 suspend 功能，我们决定取消对 XMC-C 系列闪存挂起的支持。但是你依旧可以通过打开 `CONFIG_SPI_FLASH_FORCE_ENABLE_XMC_C_SUSPEND` 来强行使能这个功能。如果您有任何疑问，请联系 espressif 商务支持。
