存储
=======

SPI flash 驱动
^^^^^^^^^^^^^^^^^^^^^^

XMC-C 系列闪存 suspend 功能的支持已被移除。根据闪存制造商的反馈，在某些情况下，XMC-C 闪存需要在 resume 和下一条命令之间间隔 1 毫秒。这对于软件请求来说时间太长。基于上述原因，为了安全使用 suspend 功能，我们决定取消 XMC-C 系列的闪存挂起支持。但是你依旧可以强行通过 `CONFIG_SPI_FLASH_FORCE_ENABLE_XMC_C_SUSPEND` 的配置使能这个功能。如果您有任何疑问，请联系 espressif 业务支持。
