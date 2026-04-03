RSA 数字签名外设 (RSA_DS)
==========================

:link_to_translation:`en:[English]`

RSA 数字签名外设 (RSA_DS) 提供基于 RSA 的消息签名硬件加速，并使用预加密参数来计算签名。这些参数通过 HMAC 作为密钥派生函数进行加密，而 HMAC 则以 eFuse 作为输入密钥。

.. only:: SOC_KEY_MANAGER_SUPPORTED

    在 {IDF_TARGET_NAME} 上，RSA 数字签名外设 (RSA_DS) 也可以使用存储在密钥管理器中的密钥，而非 eFuse 密钥块。AES 加密密钥可以通过密钥管理器直接部署，类型为 :cpp:enumerator:`ESP_KEY_MGR_DS_KEY`。详情请参阅 :ref:`key-manager`。

以上过程均在硬件中完成，因此在计算签名时，软件无法获取 RSA 参数的解密密钥，也无法获取 HMAC 密钥派生函数的输入密钥。

签名计算所涉及的硬件信息以及所用寄存器的有关信息，请参阅 **{IDF_TARGET_NAME} 技术参考手册** > **RSA 数字签名外设 (RSA_DS)** [`PDF <{IDF_TARGET_TRM_CN_URL}#digsig>`__]。


私钥参数
--------

RSA 签名的私钥参数存储在 flash 中。为防止发生未经授权的访问，这些参数都进行了 AES 加密。此时，HMAC 模块被作为密钥派生函数，用于计算 AES 加密了的私钥参数。同时，HMAC 模块本身使用的来自 eFuse 密钥块的密钥也具有读取保护，可以防止发生未经授权的访问。

调用签名计算时，软件只需指定使用的 eFuse 密钥、相应的 eFuse 密钥用途、加密的 RSA 参数位置以及待签名的数据或信息。

密钥生成
---------

在使用 RSA_DS 外设前，需首先创建并存储 HMAC 密钥和 RSA 私钥，此步骤可在 {IDF_TARGET_NAME} 上通过软件完成，也可在主机上进行。在 ESP-IDF 中，可以使用 :cpp:func:`esp_efuse_write_block` 设置 HMAC 密钥，并使用 :cpp:func:`esp_hmac_calculate` 对 RSA 私钥参数进行加密。

计算并组装私钥参数的详细信息，请参阅 **{IDF_TARGET_NAME} 技术参考手册** > **RSA 数字签名外设 (RSA_DS)** [`PDF <{IDF_TARGET_TRM_CN_URL}#digsig>`__]。

在 ESP-IDF 中进行数字签名计算
----------------------------------

在 ESP-IDF 中进行数字签名计算的工作流程，以及所用寄存器的有关信息，请参阅 **{IDF_TARGET_NAME} 技术参考手册** > **RSA 数字签名外设 (RSA_DS)** [`PDF <{IDF_TARGET_TRM_CN_URL}#digsig>`__]。

要进行数字签名计算，需要准备以下三个参数：

#. 用作 HMAC 密钥的 eFuse 密钥块 ID
#. 加密私钥参数的位置
#. 待签名的数据或信息

**底层 API（原始 RSA）**

签名计算需要一些时间，因此 ESP-IDF 提供了两种可用的 API：第一种是 :cpp:func:`esp_ds_sign`，调用此 API 后，程序会在计算完成前保持阻塞状态。如果在计算过程中，软件需要执行其他操作，则可以调用 :cpp:func:`esp_ds_start_sign`，用另一种方式启动签名计算，然后周期性地调用 :cpp:func:`esp_ds_is_busy`，检查计算是否已完成。一旦计算完成，即可调用 :cpp:func:`esp_ds_finish_sign` 来获取签名结果。

API 函数 :cpp:func:`esp_ds_sign` 和 :cpp:func:`esp_ds_start_sign` 在 RSA_DS 外设的帮助下计算原始 RSA 签名。该签名必须转换为合适的格式（例如 PKCS#1 v1.5 或 PSS），以用于 TLS 或其他协议。

.. note::

    上述为 RSA_DS 的基本构件，其消息长度是固定的。为了对任意消息生成签名，通常会将实际消息的哈希值作为输入，并将其填充到所需长度。

**PSA 密码学驱动程序**

RSA_DS 外设也通过 **PSA Crypto RSA_DS 驱动程序** 对外提供访问接口，因此可以使用标准 PSA API 执行签名（PKCS#1 v1.5 或 PSS）和 RSA 解密（PKCS#1 v1.5 或 OAEP）。在 ``Component config`` > ``mbedTLS`` 中启用 ``CONFIG_MBEDTLS_HARDWARE_RSA_DS_PERIPHERAL``。要在 ESP-TLS 中配合使用 RSA_DS 外设（例如 TLS 客户端认证），请参阅 ESP-TLS 文档中的 :ref:`digital-signature-with-esp-tls`。

.. _configure-the-ds-peripheral:

TLS 连接所需的 RSA_DS 外设配置
------------------------------------

在 {IDF_TARGET_NAME} 芯片上使用 TLS 连接之前，必须先配置 RSA_DS 外设，具体步骤如下：

1) 随机生成一个 256 位的值，作为 ``初始化向量`` (IV)。
2) 随机生成一个 256 位的值，作为 ``HMAC_KEY``。
3) 使用客户端私钥 (RAS) 和上述步骤生成的参数，计算加密的私钥参数。
4) 将 256 位的 ``HMAC_KEY`` 烧录到 eFuse 中，只支持 RSA_DS 外设读取。

更多细节，请参阅 **{IDF_TARGET_NAME} 技术参考手册** > **RSA 数字签名外设 (RSA_DS)** [`PDF <{IDF_TARGET_TRM_CN_URL}#digsig>`__]。

如果要以开发为目的配置 RSA_DS 外设，可以使用 `esp-secure-cert-tool <https://pypi.org/project/esp-secure-cert-tool>`_。

在完成 RSA_DS 外设配置后获得的加密私钥参数应保存在 flash 中。应用需要从 flash 读取 RSA_DS 数据（例如，通过 `esp_secure_cert_mgr <https://github.com/espressif/esp_secure_cert_mgr>`_ 组件提供的 API。详情请参阅 `component/README <https://github.com/espressif/esp_secure_cert_mgr#readme>`_）。关于在 ESP-TLS 中使用 RSA_DS 外设的方法，请参阅 :ref:`digital-signature-with-esp-tls`。

通过 PSA Crypto 使用 RSA_DS 外设
------------------------------------

要在应用代码（不使用 ESP-TLS）中将 RSA_DS 外设用于签名或解密，请启用 ``CONFIG_MBEDTLS_HARDWARE_RSA_DS_PERIPHERAL``。使用加密的密钥数据 (:cpp:type:`esp_ds_data_t`)、eFuse 密钥块 ID 以及以 bit 为单位的 RSA 密钥长度填充 ``esp_ds_data_ctx_t``。确保在创建密钥时（例如通过 :cpp:func:`esp_ds_encrypt_params` 或 RSA_DS 配置工具）设置 :cpp:type:`esp_ds_data_t` 的 ``rsa_length`` 字段，然后将该上下文封装到 ``esp_rsa_ds_opaque_key_t`` 中，使用 ``PSA_KEY_LIFETIME_ESP_RSA_DS_VOLATILE`` 将其作为 PSA 不透明密钥导入，并调用 ``psa_sign_hash()`` 或 ``psa_asymmetric_decrypt()``：

.. code-block:: c

    #include "psa/crypto.h"
    #include "psa_crypto_driver_esp_rsa_ds.h"

    // ds_ctx 指向 esp_ds_data_ctx_t（例如来自安全证书或 NVS）
    esp_ds_data_ctx_t *ds_ctx = ...;
    esp_rsa_ds_opaque_key_t rsa_ds_opaque_key = {
        .ds_data_ctx = ds_ctx,
    };

    psa_key_attributes_t attrs = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_type(&attrs, PSA_KEY_TYPE_RSA_KEY_PAIR);
    psa_set_key_bits(&attrs, ds_ctx->rsa_length_bits);
    psa_set_key_usage_flags(&attrs, PSA_KEY_USAGE_SIGN_HASH);
    psa_set_key_algorithm(&attrs, PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_256));
    psa_set_key_lifetime(&attrs, PSA_KEY_LIFETIME_ESP_RSA_DS_VOLATILE);

    psa_key_id_t key_id;
    psa_status_t status = psa_import_key(&attrs,
                                         (const uint8_t *)&rsa_ds_opaque_key,
                                         sizeof(rsa_ds_opaque_key),
                                         &key_id);
    psa_reset_key_attributes(&attrs);
    if (status != PSA_SUCCESS) {
        // 处理错误
    }

    // 对哈希值进行签名（例如消息的 SHA-256）
    uint8_t hash[32] = { ... };
    uint8_t signature[256];
    size_t sig_len;
    status = psa_sign_hash(key_id, PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_256),
                           hash, sizeof(hash), signature, sizeof(signature), &sig_len);

    psa_destroy_key(key_id);

使用 RSA_DS 外设进行 SSL 双向认证
------------------------------------

此前位于 ``examples/protocols/mqtt/ssl_ds`` 目录下的 SSL 双向认证示例现已随独立的 `espressif/mqtt <https://components.espressif.com/components/espressif/mqtt>`__ 组件一同提供。请参照该组件文档获取 SSL RSA_DS 示例，并与 ESP-MQTT 一同构建。该示例仍使用 ``mqtt_client`` （由 ESP-MQTT 实现），通过双向认证 TLS 连接至 ``test.mosquitto.org``，其中 TLS 通信层仍由 ESP-TLS 实现。

.. only:: SOC_KEY_MANAGER_SUPPORTED

    如果 :cpp:member:`esp_ds_data_ctx_t::efuse_key_id` 和 :cpp:member:`esp_rsa_ds_opaque_key_t::key_recovery_info` 均已设置，ESP-DS PSA 驱动程序将优先使用基于密钥管理器的 DS 密钥，而非基于 eFuse 的 DS 密钥。

API 参考
--------

.. include-build-file:: inc/psa_crypto_driver_esp_rsa_ds_contexts.inc
