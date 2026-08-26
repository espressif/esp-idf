SM3 Hash Accelerator
====================

:link_to_translation:`zh_CN:[中文]`

SM3 (GM/T 0004-2012) is a cryptographic hash function of the Chinese ShangMi (SM) standards. It reads the message in 64-byte blocks and produces a 256-bit digest, comparable to SHA-256. On {IDF_TARGET_NAME}, the SHA accelerator computes SM3 in hardware.

The ``esp_sm3`` API is the only interface to the SM3 hardware. SM3 is not available through the Mbed TLS or PSA Crypto APIs.

.. only:: esp32s31

    An eFuse (``DIS_SM_CRYPT``) can disable all the SM crypto functions permanently. On a chip with this eFuse set, :cpp:func:`esp_sm3_create` and :cpp:func:`esp_sm3` return :c:macro:`ESP_ERR_NOT_SUPPORTED`. Always check the return value.

One-Shot Digest
---------------

Use :cpp:func:`esp_sm3` when the whole message is in one buffer:

.. code-block:: c

    #include "esp_sm3.h"

    uint8_t digest[ESP_SM3_DIGEST_LEN];

    esp_err_t err = esp_sm3(message, message_len, digest, sizeof(digest));
    if (err != ESP_OK) {
        // No digest was written. Handle the error.
    }

Streaming Digest
----------------

Use the context functions when the message arrives in parts:

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

:cpp:func:`esp_sm3_create` allocates the context and writes the handle to ``ctx``. If the function returns :c:macro:`ESP_ERR_NO_MEM` or :c:macro:`ESP_ERR_NOT_SUPPORTED`, it writes NULL to ``ctx``. The context is opaque. The application must not read or write it.

:cpp:func:`esp_sm3_delete` erases the message bytes and the digest state. Then it frees the context. The handle is not valid after the call. If the application does not call this function, the context stays allocated.

Call :cpp:func:`esp_sm3_delete` after :cpp:func:`esp_sm3_finish`. Call it also to abandon an operation.

Concurrency
-----------

:cpp:func:`esp_sm3_update` and :cpp:func:`esp_sm3_finish` acquire the SHA peripheral. They hold it only while the hardware processes a message block, and they release it before they return. :cpp:func:`esp_sm3_create` and :cpp:func:`esp_sm3_delete` never touch the peripheral.

:cpp:func:`esp_sm3_update` acquires the peripheral only when the new bytes complete at least one 64-byte block. A call with fewer bytes copies them into the context and returns. :cpp:func:`esp_sm3_finish` always acquires the peripheral, because the padding completes the last block.

An operation can therefore stay open for a long time at no cost to other users of the peripheral. Operations with different contexts can run in parallel from different tasks.

Do not use one context from two tasks at the same time.

API Reference
-------------

.. include-build-file:: inc/esp_sm3.inc
