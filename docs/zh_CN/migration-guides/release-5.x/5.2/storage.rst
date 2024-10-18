存储
====

:link_to_translation:`en:[English]`

NVS 加密
--------

- 在集成 HMAC 外设 (``SOC_HMAC_SUPPORTED``) 的 SoC 上，启用 :doc:`../../../security/flash-encryption` 时将不再自动启用 :doc:`../../../api-reference/storage/nvs_encryption`。
- 因此需显式启用 NVS 加密，并按照需要选择基于 flash 加密或基于 HMAC 外设的方案。即使未启用 flash 加密，也可选择基于 HMAC 外设的方案 (:ref:`CONFIG_NVS_SEC_KEY_PROTECTION_SCHEME`)。
- 启用 flash 加密后，未集成 HMAC 外设的 SoC 仍会自动启用 NVS 加密。

SPI flash 驱动
^^^^^^^^^^^^^^^^^^^^^^

XMC-C 系列闪存 suspend 功能的支持已被移除。根据闪存制造商的反馈，在某些情况下，XMC-C 闪存需要在 resume 和下一条命令之间间隔 1 毫秒。这对于软件请求来说时间太长。基于上述原因，为了安全使用 suspend 功能，我们决定取消对 XMC-C 系列闪存挂起的支持。但是你依旧可以通过打开 `CONFIG_SPI_FLASH_FORCE_ENABLE_XMC_C_SUSPEND` 来强行使能这个功能。如果您有任何疑问，请联系 espressif 商务支持。
