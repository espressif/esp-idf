Security
========

:link_to_translation:`zh_CN:[中文]`

.. only:: SOC_SHA_SUPPORTED

    Mbed TLS
    --------

    Starting from **ESP-IDF v5.5**, there is a change in how the SHA sub-function APIs, :cpp:func:`esp_sha_block` and :cpp:func:`esp_sha_dma`, are used.

    Previously, these APIs used to set the SHA mode internally, however, in the updated version, you must explicitly set the SHA mode before invoking them.

    For instance, if you intend to use the **SHA-256** algorithm, you must first call :cpp:func:`esp_sha_set_mode` with the argument ``SHA2_256``:

    .. code-block:: c

        esp_sha_set_mode(SHA2_256);
