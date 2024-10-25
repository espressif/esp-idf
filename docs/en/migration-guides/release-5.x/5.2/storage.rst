Storage
=======

:link_to_translation:`zh_CN:[中文]`

NVS Encryption
--------------

- For SoCs with the HMAC peripheral (``SOC_HMAC_SUPPORTED``), turning on :doc:`../../../security/flash-encryption` will no longer automatically turn on :doc:`../../../api-reference/storage/nvs_encryption`.
- You will need to explicitly turn on NVS encryption and select the required scheme (flash encryption-based or HMAC peripheral-based). You can select the HMAC peripheral-based scheme (:ref:`CONFIG_NVS_SEC_KEY_PROTECTION_SCHEME`), even if flash encryption is not enabled.
- SoCs without the HMAC peripheral will still automatically turn on NVS encryption when flash encryption is enabled.

SPI Flash Driver
^^^^^^^^^^^^^^^^

XMC-C series flash suspend support has been removed. According to feedback from the flash manufacturer, in some situations the XMC-C flash would require a 1ms interval between resume and next command. This is too long for a software request. Based on the above reason, in order to use suspend safely, we decide to remove flash suspend support from XMC-C series. But you can still force enable it via `CONFIG_SPI_FLASH_FORCE_ENABLE_XMC_C_SUSPEND`. If you have any questions, please contact espressif business support.
