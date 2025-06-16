安全性
=======

:link_to_translation:`en:[English]`

Mbed TLS
--------

    从 **ESP-IDF v6.0** 开始，一些已废弃的 mbedtls 头文件已被移除，例如 ``esp32/aes.h``、``esp32/sha.h``、``esp32s2/aes.h``、``esp32s2/sha.h`` 和 ``esp32s2/gcm.h``。请改为分别包含 ``aes/esp_aes.h``、``sha/sha_core.h`` 和 ``aes/esp_aes_gcm.h``。

    .. only:: SOC_SHA_SUPPORTED

        SHA 模块头文件 ``sha/sha_dma.h`` 和 ``sha/sha_block.h`` 也已废弃并被移除，请改为包含 ``sha/sha_core.h``。
