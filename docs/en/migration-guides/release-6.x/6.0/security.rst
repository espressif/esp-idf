Security
========

:link_to_translation:`zh_CN:[中文]`

Mbed TLS
--------

Starting from **ESP-IDF v6.0**, some already deprecated mbedtls header files like ``esp32/aes.h``, ``esp32/sha.h``, ``esp32s2/aes.h``, ``esp32s2/sha.h`` and ``esp32s2/gcm.h`` have been removed, instead, you should include ``aes/esp_aes.h``, ``sha/sha_core.h`` and ``aes/esp_aes_gcm.h`` respectively.

.. only:: SOC_SHA_SUPPORTED

    The SHA module headers ``sha/sha_dma.h`` and ``sha/sha_block.h`` are also deprecated and removed. You should include ``sha/sha_core.h`` instead.

**Removed Deprecated APIs**

The following deprecated functions have been removed:

- :cpp:func:`esp_aes_encrypt` – Use :cpp:func:`esp_internal_aes_encrypt` instead.
- :cpp:func:`esp_aes_decrypt` – Use :cpp:func:`esp_internal_aes_decrypt` instead.
- :cpp:func:`esp_crypto_shared_gdma_start` – Use :cpp:func:`esp_crypto_shared_gdma_start_axi_ahb` instead.

Note that the new AES functions return error codes for better error handling, unlike the old void functions.

Bootloader Support
------------------

**Removed Deprecated APIs**

The following deprecated functions have been removed:

- :cpp:func:`esp_secure_boot_verify_signature_block` – Use :cpp:func:`esp_secure_boot_verify_ecdsa_signature_block` instead.

.. only:: SOC_HMAC_SUPPORTED

    NVS Security Provider
    ---------------------

    - When NVS encryption is enabled on SoCs with the HMAC peripheral that have flash encryption enabled, the HMAC-based NVS encryption scheme is now selected as default instead of the flash encryption-based scheme. If your application previously used the flash encryption-based scheme, you need to manually configure the NVS encryption scheme to flash encryption from HMAC through ``menuconfig`` or your project's ``sdkconfig`` (i.e., setting ``CONFIG_NVS_SEC_KEY_PROTECT_USING_FLASH_ENC=y``).
