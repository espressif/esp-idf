椭圆曲线数字签名算法 (ECDSA)
============================

:link_to_translation:`en:[English]`

椭圆曲线数字签名算法 (ECDSA) 是数字签名算法 (DSA) 基于椭圆曲线密码学的变体。

{IDF_TARGET_NAME} 的 ECDSA 外设为计算 ECDSA 签名提供了一个安全高效的环境，不仅能确保签名过程的机密性，防止信息泄露，也提供了快速的计算。在签名过程中使用的 ECDSA 私钥只能由硬件外设访问，软件无法读取。

ECDSA 外设可以为 TLS 双向身份验证等用例建立 **安全设备身份认证**。

支持的特性
----------

.. list::

    - ECDSA 数字签名生成和验证
    :SOC_ECDSA_SUPPORT_CURVE_P384: - 三种不同的椭圆曲线，P-192，P-256 和 P-384（FIPS 186-3 规范）
    :not SOC_ECDSA_SUPPORT_CURVE_P384: - 两种不同的椭圆曲线，P-192 和 P-256（FIPS 186-3 规范）
    :SOC_ECDSA_SUPPORT_CURVE_P384: - ECDSA 操作中用于散列消息的三种哈希算法，SHA-224, SHA-256 和 SHA-384（FIPS PUB 180-4 规范）
    :not SOC_ECDSA_SUPPORT_CURVE_P384: - ECDSA 操作中哈希消息的两种哈希算法，SHA-224 和 SHA-256（FIPS PUB 180-4 规范）


{IDF_TARGET_NAME} 上的 ECDSA
----------------------------

在 {IDF_TARGET_NAME} 上，ECDSA 模块使用烧录到 eFuse 块中的密钥。密码模块外的任何资源都不可访问此密钥（默认模式），从而避免密钥泄露。

ECDSA 密钥存储
^^^^^^^^^^^^^^

ECDSA 私钥存储在 eFuse 密钥块中。所需的密钥块数量取决于曲线大小：

- **P-192 和 P-256 曲线**：需要一个 eFuse 密钥块（256 位）
- **P-384 曲线**：需要两个 eFuse 密钥块（总共 512 位）

使用 P-384 曲线或其他需要两个密钥块的曲线时，必须使用相应的宏将两个密钥块编号组合为一个整数，以便 ECDSA 外设能够识别：

- **对于 mbedTLS 应用程序**：使用 :c:macro:`MBEDTLS_ECDSA_COMBINE_KEY_BLOCKS` 宏（定义在 ``ecdsa/ecdsa_alt.h`` 中）
- **对于 HAL 应用程序**：使用 :c:macro:`HAL_ECDSA_COMBINE_KEY_BLOCKS` 宏（定义在 ``hal/ecdsa_types.h`` 中）
- **对于 ESP-TLS 应用程序**：使用 :c:macro:`ESP_TLS_ECDSA_COMBINE_KEY_BLOCKS` 宏（定义在 ``esp_tls.h`` 中）

你还可以使用相应的提取宏来获取各个密钥块编号：

- **对于 mbedTLS 应用程序**：使用 :c:macro:`MBEDTLS_ECDSA_EXTRACT_KEY_BLOCKS` 宏
- **对于 HAL 应用程序**：使用 :c:macro:`HAL_ECDSA_EXTRACT_KEY_BLOCKS` 宏
- **对于 ESP-TLS 应用程序**：使用 :c:macro:`ESP_TLS_ECDSA_EXTRACT_KEY_BLOCKS` 宏

以下是使用这些宏的示例：

.. code-block:: c

    #include "ecdsa/ecdsa_alt.h"

    // 示例：使用需要两个密钥块的 P-384 曲线
    // 假设要使用密钥块 4 和 5
    uint8_t block_low = 4;   // 较低编号的密钥块
    uint8_t block_high = 5;  // 较高编号的密钥块

    // 将两个密钥块编号组合成一个整数
    // 注意：第一个参数是高位块，第二个参数是低位块
    uint16_t combined_blocks = MBEDTLS_ECDSA_COMBINE_KEY_BLOCKS(block_high, block_low);

    // 在 ECDSA 操作中使用 combined_blocks 值
    // 此值可以传递给 mbedTLS ECDSA 函数

    // 之后提取各个密钥块编号
    uint8_t extracted_block_low, extracted_block_high;
    MBEDTLS_ECDSA_EXTRACT_KEY_BLOCKS(combined_blocks, &extracted_block_high, &extracted_block_low);

    // extracted_block_low 的值为 4，extracted_block_high 的值为 5

ECDSA 密钥可以通过 ``idf.py`` 脚本在外部编程。以下是关于编程 ECDSA 密钥的示例：

.. code:: bash

   idf.py efuse-burn-key <BLOCK_NUM> </path/to/ecdsa_private_key.pem> ECDSA_KEY

.. only:: SOC_EFUSE_BLOCK9_KEY_PURPOSE_QUIRK

    .. note::

        五个物理 eFuse 块可作为 ECDSA 模块的密钥：块 4 ~ 块 8。例如，对于块 4（第一个密钥块），参数为 ``BLOCK_KEY0``。

.. only:: not SOC_EFUSE_BLOCK9_KEY_PURPOSE_QUIRK

    .. note::

        六个物理 eFuse 块可作为 ECDSA 模块的密钥：块 4 ~ 块 9。例如，对于块 4（第一个密钥块），参数为 ``BLOCK_KEY0``。


另外，ECDSA 密钥也可以通过在目标上运行的应用程序进行编程。

以下代码片段使用 :cpp:func:`esp_efuse_write_key` 将 eFuse 中的物理密钥块 0 的密钥目的设置为 :cpp:enumerator:`esp_efuse_purpose_t::ESP_EFUSE_KEY_PURPOSE_ECDSA_KEY`:

.. code-block:: c

    #include "esp_efuse.h"

    const uint8_t key_data[32] = { ... };

    esp_err_t status = esp_efuse_write_key(EFUSE_BLK_KEY0,
                        ESP_EFUSE_KEY_PURPOSE_ECDSA_KEY,
                        key_data, sizeof(key_data));

    if (status == ESP_OK) {
        // written key
    } else {
        // writing key failed, maybe written already
    }


.. only:: SOC_ECDSA_P192_CURVE_DEFAULT_DISABLED

    ECDSA 曲线配置
    -----------------

    .. only:: esp32h2

        ESP32-H2 的 ECDSA 外设支持 ECDSA-P192 和 ECDSA-P256 两种曲线操作。但从 ESP32-H2 版本 1.2 开始，默认仅启用 ECDSA-P256 操作。可以通过以下配置项启用 ECDSA-P192 操作：

    .. only:: not esp32h2

        {IDF_TARGET_NAME} 的 ECDSA 外设支持 ECDSA-P192 和 ECDSA-P256 两种曲线操作，但默认仅启用 ECDSA-P256 操作。可以通过以下配置项启用 ECDSA-P192 操作：

    - :ref:`CONFIG_ESP_ECDSA_ENABLE_P192_CURVE` 启用对 ECDSA-P192 曲线操作的支持，使设备可以同时执行 192 位和 256 位的 ECDSA 曲线操作。但请注意，如果 eFuse 写保护期间已永久禁用 ECDSA-P192 操作，则启用该配置项也无法重新启用该功能。

    - :cpp:func:`esp_efuse_enable_ecdsa_p192_curve_mode()` 可用于以编程方式启用 ECDSA-P192 曲线操作。它会向 eFuse 写入相应值，从而使设备支持 P-192 和 P-256 曲线操作。但请注意，若对应的 eFuse 区域已被写保护，则此 API 将调用失败。

.. only:: SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE

    生成确定性签名
    --------------

    {IDF_TARGET_NAME} 的 ECDSA 外设还支持使用确定性推导参数 K 来生成确定性签名，详见 `RFC 6979 <https://tools.ietf.org/html/rfc6979>`_ 第 3.2 节。

生成非确定性签名
----------------

对 TRNG 的依赖
^^^^^^^^^^^^^^

ECDSA 外设依靠硬件真随机数生成器 (TRNG) 来满足其内部熵要求，从而生成非确定性签名。在创建 ECDSA 签名时，算法需要生成一个随机整数，在 `RFC 6090 <https://tools.ietf.org/html/rfc6090>`_ 第 5.3.2 节有说明。

在应用程序中启动 ECDSA 计算（主要是签名）之前，请确保硬件 :doc:`RNG <../system/random>` 已经启用。

应用程序概述
------------

有关如何使用 ECDSA 外设建立 TLS 双向身份验证连接的详细信息，请参阅 :ref:`ecdsa-peri-with-esp-tls` 指南。

通过覆盖 ECDSA 签名以及验证 API，可以集成 Mbed TLS 堆栈中的 ECDSA 外设。请注意，ECDSA 外设并不支持所有曲线或哈希算法。因此，在不满足硬件要求时，实现会退回到软件。

对于特定的 TLS 上下文，可用额外的 API 来填充某些字段（例如私钥 ctx），以区分路由到硬件的路径。ESP-TLS 层在内部集成了这些 API，因此在应用程序层不需要额外的操作。对于自定义用例，请参阅以下 API 详细信息。

API 参考
--------

.. include-build-file:: inc/ecdsa_alt.inc
