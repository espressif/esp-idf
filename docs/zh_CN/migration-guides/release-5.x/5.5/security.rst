安全性
======

:link_to_translation:`en:[English]`

.. only:: SOC_SHA_SUPPORTED

    Mbed TLS
    --------

    从 **ESP-IDF v5.5** 开始，SHA 子功能 API（:cpp:func:`esp_sha_block` 和 :cpp:func:`esp_sha_dma`）的使用方式发生了变化。

    在此之前，这些 API 会在内部自动设置 SHA 模式。但从 ESP-IDF v5.5 起，需要在调用 API 前显式设置 SHA 模式。

    例如，如果你打算使用 **SHA-256** 算法，必须首先调用 :cpp:func:`esp_sha_set_mode` 并传入参数 ``SHA2_256``：

    .. code-block:: c

        esp_sha_set_mode(SHA2_256);
