Security
========

:link_to_translation:`zh_CN:[中文]`

Mbed TLS
--------

    Starting from **ESP-IDF v6.0**, some already deprecated mbedtls header files like ``esp32/aes.h``, ``esp32/sha.h``, ``esp32s2/aes.h``, ``esp32s2/sha.h`` and ``esp32s2/gcm.h`` have been removed, instead, you should include ``aes/esp_aes.h``, ``sha/sha_core.h`` and ``aes/esp_aes_gcm.h`` respectively.

    .. only:: SOC_SHA_SUPPORTED

        The SHA module headers ``sha/sha_dma.h`` and ``sha/sha_block.h`` are also deprecated and removed. You should include ``sha/sha_core.h`` instead.
