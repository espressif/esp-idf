安全性
======

:link_to_translation:`en:[English]`

Mbed TLS
--------

从 **ESP-IDF v6.0** 开始，一些已废弃的 mbedtls 头文件已被移除，例如 ``esp32/aes.h``、``esp32/sha.h``、``esp32s2/aes.h``、``esp32s2/sha.h`` 和 ``esp32s2/gcm.h``。请改为分别包含 ``aes/esp_aes.h``、``sha/sha_core.h`` 和 ``aes/esp_aes_gcm.h``。

.. only:: SOC_SHA_SUPPORTED

    SHA 模块头文件 ``sha/sha_dma.h`` 和 ``sha/sha_block.h`` 也已废弃并被移除，请改为包含 ``sha/sha_core.h``。

PSA Crypto 迁移
~~~~~~~~~~~~~~~~~

在 ESP-IDF v6.0 中，多个 ESP-IDF 组件已完成从传统 Mbed TLS 加密 API（例如 ``mbedtls_sha*_*()``、``mbedtls_md*_*()`` 等）向 `PSA Crypto API <https://arm-software.github.io/psa-api/>`__ 的迁移。

此次迁移使 ESP-IDF 与将 PSA Crypto 作为主要加密接口的 Mbed TLS v4.x 保持同步，并能够在支持时通过 PSA 驱动程序启用 ESP-IDF 硬件加速功能。

Mbed TLS v4.0 迁移
^^^^^^^^^^^^^^^^^^^^^

ESP-IDF v6.0 已升级至 Mbed TLS v4.0，**PSA Crypto 成为主要加密接口** （TF-PSA-Crypto 提供加密功能，Mbed TLS 侧重于 TLS 和 X.509）。此变更可能直接影响使用 Mbed TLS 加密原语、TLS 配置或 Mbed TLS 内部/私有声明的应用程序。

- **重大变更**：在 Mbed TLS v4.0 中，**大多数传统加密 API 已被移除**，PSA Crypto 成为主要接口。如果应用直接使用传统的 ``mbedtls_*`` 加密原语，则无法直接兼容，需迁移到 PSA Crypto API。
- **重大变更**：在执行任何加密操作之前（包括解析密钥/证书或启动 TLS 握手等操作），必须调用 ``psa_crypto_init()``。ESP-IDF 在常规启动过程中会初始化 PSA，因此大多数应用保持兼容；但对于早于常规启动流程运行的代码，需显式调用 ``psa_crypto_init()``。
- **新增 API**：新增 ``esp_ecdsa_free_pk_context(mbedtls_pk_context *key_ctx)``，参见 ``ecdsa/ecdsa_alt.h``。如果应用使用 ``esp_ecdsa_set_pk_context()`` 初始化包含硬件 ECDSA 密钥的 PK 上下文，应使用 ``esp_ecdsa_free_pk_context()`` 进行释放。在基于 PSA 的 Mbed TLS v4.x 中，``mbedtls_pk_free()`` 无法释放此种情况下手动创建的密钥对结构。
- **重大变更**：原需应用提供 RNG 回调（``f_rng``、``p_rng``）的 API，在 Mbed TLS v4.0 中已改为使用 PSA RNG。使用旧函数原型的代码将无法直接兼容，需要更新为新的 API 定义（例如 X.509 写入 API、SSL cookie 设置以及 SSL ticket 设置）。
- **重大变更**：TLS 1.2 / DTLS 1.2 的互操作性可能受到影响，因为 Mbed TLS v4.0 移除了对基于有限域 DHE、无前向保密的 RSA 密钥交换（以及静态 ECDH）的支持。如果对等端需要使用已移除的密码套件，TLS 连接将不兼容并可能失败；需相应更新服务器或客户端的密码套件配置。
- **重大变更**：证书或对等端中使用的椭圆曲线位数小于 250 位（例如 secp192r1/secp224r1）已不再受支持。
- **注意**：

  - 避免依赖 Mbed TLS 的私有声明（例如 ``mbedtls/private/`` 目录下的头文件，或通过 ``MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS`` / ``MBEDTLS_ALLOW_PRIVATE_ACCESS`` 启用的声明）。这些私有接口可能未经通知即发生变更。
  - PSA Crypto 迁移 (TF-PSA-Crypto) 可能会增加 flash 占用，具体取决于启用的功能。以下示例可作为参考：

    .. list-table::
       :header-rows: 1
       :widths: 30 15 15 15 10

       * - 示例
         - 非 PSA 构建（字节）
         - PSA 迁移（字节）
         - 差值（字节）
         - 差值（%）
       * - :example:`protocols/esp_http_client`
         - 609041
         - 646293
         - 37252
         - 5.76
       * - :example:`protocols/https_server`
         - 871021
         - 898717
         - 27696
         - 3.08
       * - :example:`protocols/http_server/simple`
         - 785825
         - 826909
         - 41084
         - 4.97

参考文档
^^^^^^^^^^

- :idf_file:`Mbed TLS 4.0 迁移指南 <components/mbedtls/mbedtls/docs/4.0-migration-guide.md>`
- :idf_file:`TF-PSA-Crypto 1.0 迁移指南 <components/mbedtls/mbedtls/tf-psa-crypto/docs/1.0-migration-guide.md>`
- :idf_file:`TF-PSA-Crypto PSA 过渡说明 <components/mbedtls/mbedtls/tf-psa-crypto/docs/psa-transition.md>`

上游 Mbed TLS PSA 说明
^^^^^^^^^^^^^^^^^^^^^^^^^^^

部分应用程序此前可能访问的数据结构和内部状态，在使用基于 PSA 的 Mbed TLS 版本时已不可用。如果应用依赖直接访问 Mbed TLS 的内部状态（例如以结构体字段形式访问熵/DRBG 上下文），请迁移到受支持的公共 API。

ESP-IDF 中的 PSA 持久化存储 (ITS)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ESP-IDF 提供了基于 NVS 的 PSA 内部可信存储 (ITS) 实现，因此无需文件系统即可使用 PSA 持久化存储。如果应用使用 PSA 持久化密钥/存储，请确保在首次使用前 NVS 已就绪并初始化完毕。

已移除的弃用 API (Mbed TLS / crypto)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

以下废弃函数已被移除：

- :cpp:func:`esp_aes_encrypt` – 请使用 :cpp:func:`esp_internal_aes_encrypt` 代替。
- :cpp:func:`esp_aes_decrypt` – 请使用 :cpp:func:`esp_internal_aes_decrypt` 代替。
- :cpp:func:`esp_crypto_shared_gdma_start` – 请使用 :cpp:func:`esp_crypto_shared_gdma_start_axi_ahb` 代替。

注意，新的 AES 函数返回错误代码以提供更好的错误处理，与返回 void 的旧函数不同。

BluFi
-----

BluFi（基于 BLE 的 Wi-Fi 配网）功能受到 ESP-IDF v6.0 中 Mbed TLS v4.x / PSA Crypto 迁移的影响。

- **重大变更**：BluFi 协议版本已更新（``BTC_BluFi_SUB_VER`` 从 ``0x03`` 升级至 ``0x04``）。ESP-IDF 使用的 BluFi 安全协商实现也同步更新为基于 PSA Crypto 的方案（参考更新后的 ``examples/bluetooth/BluFi`` 示例）。

- **影响**：基于旧版 BluFi 加密/协议实现构建的现有 BluFi 客户端应用（例如手机 App），可能无法再与使用 ESP-IDF v6.0 构建的设备互通。通常表现为在配网时出现 BluFi 协商或连接失败。

- **必要操作**：请同步更新两端应用：

  - 将设备固件升级至 ESP-IDF v6.0。
  - 将 BluFi 客户端应用更新至兼容 ESP-IDF v6.0 新版 BluFi 协议和安全协商的版本。

引导加载程序支持
----------------

**已移除的废弃 API**

以下废弃函数已被移除：

- :cpp:func:`esp_secure_boot_verify_signature_block` – 请使用 :cpp:func:`esp_secure_boot_verify_ecdsa_signature_block` 代替。

.. only:: SOC_HMAC_SUPPORTED

    NVS 安全方案
    ------------

    - 当 SoC 具备 HMAC 外设并启用了 flash 加密时，如果同时还启用了 NVS 加密，则默认会选择基于 HMAC 的 NVS 加密方案，而不是基于 flash 加密的方案。如果你的应用程序之前使用基于 flash 加密的方案，则需要通过 ``menuconfig`` 或项目的 ``sdkconfig`` 文件，手动将 NVS 加密方案从 HMAC 配置为 flash 加密（即设置 ``CONFIG_NVS_SEC_KEY_PROTECT_USING_FLASH_ENC=y``）。
