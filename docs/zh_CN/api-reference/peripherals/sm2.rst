SM2 数字签名 (SM2)
==================

:link_to_translation:`en:[English]`

SM2 是中国国家标准 GB/T 32918 定义的椭圆曲线公钥算法。{IDF_TARGET_NAME} 的 ECC 加速器支持 SM2 推荐曲线（GB/T 32918.5）。``esp_sm2`` API 使用该硬件派生公钥、生成签名和验证签名。

所有运算均在 ECC 加速器上完成，API 不使用软件大数库。涉及私钥或随机数的点乘运算以恒定时间模式执行。

该 API 中的所有缓冲区均为 32 字节（:c:macro:`ESP_SM2_PARAM_LEN`），采用大端字节序。

摘要输入
--------

该 API 签名和验证的对象是 32 字节摘要，而非消息本身。GB/T 32918.2 将摘要定义为 ``e = SM3(ZA || M)``，其中 ``M`` 为消息，``ZA`` 为签名者身份标识与曲线参数的 SM3 杂凑值。这两个杂凑值均可使用 ``esp_sm3`` API 计算。调用者必须先计算摘要，再调用该 API。

生成密钥对
----------

私钥是范围在 ``[1, n-2]`` 内的 32 字节随机值 ``d``，其中 ``n`` 为曲线的阶。反复生成随机候选值，直到 API 接受为止：

.. code-block:: c

    #include "esp_sm2.h"
    #include "esp_random.h"

    uint8_t priv_key[ESP_SM2_PARAM_LEN];
    uint8_t pub_x[ESP_SM2_PARAM_LEN];
    uint8_t pub_y[ESP_SM2_PARAM_LEN];
    esp_err_t err;

    do {
        esp_fill_random(priv_key, sizeof(priv_key));
        err = esp_sm2_get_pubkey(priv_key, pub_x, pub_y);
    } while (err == ESP_ERR_INVALID_ARG);
    ESP_ERROR_CHECK(err);

私钥由调用者负责保管。请将私钥存放在安全位置，并擦除所有不再需要的副本。

对摘要签名
----------

.. code-block:: c

    uint8_t digest[ESP_SM2_PARAM_LEN];   /* e = SM3(ZA || M)，由调用者计算 */
    uint8_t sig_r[ESP_SM2_PARAM_LEN];
    uint8_t sig_s[ESP_SM2_PARAM_LEN];

    ESP_ERROR_CHECK(esp_sm2_sign(digest, priv_key, sig_r, sig_s));

:cpp:func:`esp_sm2_sign` 在输入格式错误时返回 ``ESP_ERR_INVALID_ARG``，在用尽全部重试次数时返回 ``ESP_FAIL``。只有随机数发生器故障才会耗尽重试次数。

.. warning::

    签名随机数来自 :cpp:func:`esp_fill_random`。签名前请确保真随机熵源处于激活状态。带偏差的随机数会使攻击者能够从签名中计算出私钥。关于随机数发生器产生真随机数的条件，请参阅 :doc:`../system/random`。

验证签名
--------

.. code-block:: c

    esp_err_t res = esp_sm2_verify(digest, pub_x, pub_y, sig_r, sig_s);
    if (res == ESP_OK) {
        /* 签名有效。 */
    }

:cpp:func:`esp_sm2_verify` 在使用公钥前，先通过硬件验证该公钥位于曲线上。仅当签名有效时返回 ``ESP_OK``；输入格式错误时返回 ``ESP_ERR_INVALID_ARG``；签名或公钥无效时返回 ``ESP_FAIL``。

.. only:: SOC_EFUSE_DIS_SM_CRYPT

    可用性
    ------

    ``DIS_SM_CRYPT`` eFuse 会永久禁用 SM2 和 SM3 硬件。烧写该 eFuse 后，该 API 中的函数返回 ``ESP_ERR_NOT_SUPPORTED``。无效参数仍会返回 ``ESP_ERR_INVALID_ARG``。

应用示例
--------

测试应用 :component_file:`esp_security/test_apps/crypto_drivers/main/test_esp_sm2.c` 演示了完整流程：基于已知答案向量的密钥派生、签名、篡改检测，以及基于生成向量的签名验证。

API 参考
--------

.. include-build-file:: inc/esp_sm2.inc
