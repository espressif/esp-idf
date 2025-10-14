安全性
=======

:link_to_translation:`en:[English]`

Mbed TLS
--------

从 **ESP-IDF v6.0** 开始，一些已废弃的 mbedtls 头文件已被移除，例如 ``esp32/aes.h``、``esp32/sha.h``、``esp32s2/aes.h``、``esp32s2/sha.h`` 和 ``esp32s2/gcm.h``。请改为分别包含 ``aes/esp_aes.h``、``sha/sha_core.h`` 和 ``aes/esp_aes_gcm.h``。

.. only:: SOC_SHA_SUPPORTED

    SHA 模块头文件 ``sha/sha_dma.h`` 和 ``sha/sha_block.h`` 也已废弃并被移除，请改为包含 ``sha/sha_core.h``。

**已移除的废弃 API**

以下废弃函数已被移除：

- :cpp:func:`esp_aes_encrypt` – 请使用 :cpp:func:`esp_internal_aes_encrypt` 代替。
- :cpp:func:`esp_aes_decrypt` – 请使用 :cpp:func:`esp_internal_aes_decrypt` 代替。
- :cpp:func:`esp_crypto_shared_gdma_start` – 请使用 :cpp:func:`esp_crypto_shared_gdma_start_axi_ahb` 代替。

注意，新的 AES 函数返回错误代码以提供更好的错误处理，与返回 void 的旧函数不同。

引导加载程序支持
------------------

**已移除的废弃 API**

以下废弃函数已被移除：

- :cpp:func:`esp_secure_boot_verify_signature_block` – 请使用 :cpp:func:`esp_secure_boot_verify_ecdsa_signature_block` 代替。

.. only:: SOC_HMAC_SUPPORTED

    NVS 安全方案
    ----------------

    - 当 SoC 具备 HMAC 外设并启用了 flash 加密时，如果同时还启用了 NVS 加密，则默认会选择基于 HMAC 的 NVS 加密方案，而不是基于 flash 加密的方案。如果你的应用程序之前基于 flash 加密，则需要通过 ``menuconfig`` 或项目的 ``sdkconfig`` 文件，手动将 NVS 加密方案从 HMAC 配置为 flash 加密（即设置 ``CONFIG_NVS_SEC_KEY_PROTECT_USING_FLASH_ENC=y``）。
