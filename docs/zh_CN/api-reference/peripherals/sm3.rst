SM3 杂凑加速器
==============

:link_to_translation:`en:[English]`

SM3（GM/T 0004-2012）是中国商用密码（SM）标准中的密码杂凑算法。它以 64 字节为分组读取消息，输出 256 位摘要，与 SHA-256 相当。在 {IDF_TARGET_NAME} 上，SHA 加速器以硬件方式计算 SM3。

``esp_sm3`` API 是访问 SM3 硬件的唯一接口。Mbed TLS 和 PSA Crypto API 均不提供 SM3。

.. only:: esp32s31

    eFuse ``DIS_SM_CRYPT`` 可永久禁用全部 SM 密码功能。在烧写了该 eFuse 的芯片上，:cpp:func:`esp_sm3_create` 和 :cpp:func:`esp_sm3` 返回 :c:macro:`ESP_ERR_NOT_SUPPORTED`。请始终检查返回值。

一次性摘要
----------

当整条消息位于同一个缓冲区时，使用 :cpp:func:`esp_sm3`：

.. code-block:: c

    #include "esp_sm3.h"

    uint8_t digest[ESP_SM3_DIGEST_LEN];

    esp_err_t err = esp_sm3(message, message_len, digest, sizeof(digest));
    if (err != ESP_OK) {
        // 未写入摘要，请处理该错误。
    }

流式摘要
--------

当消息分多次到达时，使用上下文函数：

.. code-block:: c

    #include "esp_sm3.h"

    uint8_t digest[ESP_SM3_DIGEST_LEN];
    esp_sm3_ctx_handle_t ctx = NULL;

    esp_err_t err = esp_sm3_create(&ctx);
    if (err != ESP_OK) {
        return err;
    }

    err = esp_sm3_update(ctx, part1, part1_len);
    if (err == ESP_OK) {
        err = esp_sm3_update(ctx, part2, part2_len);
    }
    if (err == ESP_OK) {
        err = esp_sm3_finish(ctx, digest, sizeof(digest));
    }

    esp_sm3_delete(ctx);

:cpp:func:`esp_sm3_create` 会分配上下文，并把句柄写入 ``ctx``。如果该函数返回 :c:macro:`ESP_ERR_NO_MEM` 或 :c:macro:`ESP_ERR_NOT_SUPPORTED`，则向 ``ctx`` 写入 NULL。上下文是不透明的，应用程序不得读写其内容。

:cpp:func:`esp_sm3_delete` 会清除上下文中的消息字节和摘要状态，然后释放该上下文。调用之后句柄失效。如果应用程序不调用该函数，该上下文会一直占用内存。

请在调用 :cpp:func:`esp_sm3_finish` 之后调用 :cpp:func:`esp_sm3_delete`。中止操作时同样需要调用。

并发
----

:cpp:func:`esp_sm3_update` 和 :cpp:func:`esp_sm3_finish` 会占用 SHA 外设。它们仅在硬件处理消息分组期间持有该外设，并在返回前释放。:cpp:func:`esp_sm3_create` 和 :cpp:func:`esp_sm3_delete` 不会访问该外设。

只有当新增字节凑满至少一个 64 字节分组时，:cpp:func:`esp_sm3_update` 才会占用该外设。字节不足时，该函数只把它们复制到上下文中并返回。:cpp:func:`esp_sm3_finish` 总会占用该外设，因为填充会凑满最后一个分组。

因此，一个操作可以长时间保持打开状态，而不会给该外设的其他使用者带来开销。使用不同上下文的操作可以在不同任务中并行执行。

不要在两个任务中同时使用同一个上下文。

API 参考
--------

.. include-build-file:: inc/esp_sm3.inc
