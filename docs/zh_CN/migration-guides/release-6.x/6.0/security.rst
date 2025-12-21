安全性
======

:link_to_translation:`en:[English]`

Mbed TLS
--------

从 **ESP-IDF v6.0** 开始，一些已废弃的 mbedtls 头文件已被移除，例如 ``esp32/aes.h``、``esp32/sha.h``、``esp32s2/aes.h``、``esp32s2/sha.h`` 和 ``esp32s2/gcm.h``。请改为分别包含 ``aes/esp_aes.h``、``sha/sha_core.h`` 和 ``aes/esp_aes_gcm.h``。

.. only:: SOC_SHA_SUPPORTED

    SHA 模块头文件 ``sha/sha_dma.h`` 和 ``sha/sha_block.h`` 也已废弃并被移除，请改为包含 ``sha/sha_core.h``。

PSA Crypto 迁移
~~~~~~~~~~~~~~~

在 ESP-IDF v6.0 中，多个 ESP-IDF 组件已从使用传统 Mbed TLS 加密 API（例如 ``mbedtls_sha*_*()``、``mbedtls_md*_*()`` 等）迁移到使用 `PSA Crypto API <https://arm-software.github.io/psa-api/>`__。

此迁移使 ESP-IDF 与 Mbed TLS v4.x 保持一致，其中 PSA Crypto 是主要的加密接口，并支持在可用时通过 PSA 驱动使用 ESP-IDF 硬件加速。

Mbed TLS v4.0 迁移
^^^^^^^^^^^^^^^^^^

ESP-IDF v6.0 更新到 Mbed TLS v4.0，其中 **PSA Crypto 是主要的加密接口** （TF-PSA-Crypto 提供加密功能；Mbed TLS 专注于 TLS 和 X.509）。这可能会影响直接使用 Mbed TLS 加密原语、TLS 配置或 Mbed TLS 内部/私有声明的应用程序。

- **重大变更**: 在 Mbed TLS v4.0 中，**大多数传统加密 API 已被移除**，PSA Crypto 是主要接口。如果你的应用程序直接使用传统 ``mbedtls_*`` 加密原语，你可能需要迁移到 PSA Crypto API。
- **重大变更**: 在任何加密操作之前，包括间接操作（如解析密钥/证书或启动 TLS 握手），都必须调用 ``psa_crypto_init()``。ESP-IDF 会在正常启动期间初始化 PSA；但是，在正常启动序列之前运行的代码必须显式调用 ``psa_crypto_init()``。
- **新增 API**: 新增了 ``esp_ecdsa_free_pk_context(mbedtls_pk_context *key_ctx)`` （位于 ``ecdsa/ecdsa_alt.h``）。如果你的应用程序使用 ``esp_ecdsa_set_pk_context()`` 用硬件支持的 ECDSA 密钥初始化 PK 上下文，请使用 ``esp_ecdsa_free_pk_context()`` 释放它。在基于 PSA 的 Mbed TLS v4.x 中，``mbedtls_pk_free()`` 不会释放在这种情况下手动创建的密钥对结构。
- **重大变更**: 以前需要应用程序提供 RNG 回调（``f_rng``、``p_rng``）的 API 在 Mbed TLS v4.0 中已更改为使用 PSA RNG。请将应用程序代码更新为新的函数原型（例如 X.509 写入 API、SSL cookie 设置和 SSL ticket 设置）。
- **重大变更**: TLS 1.2 / DTLS 1.2 的互操作性可能会受到影响，因为 Mbed TLS v4.0 移除了对基于有限域 DHE 和不具有前向保密性的 RSA 密钥交换（以及静态 ECDH）的支持。如果对等方需要已移除的套件，TLS 连接可能会失败；请相应更新服务器/客户端的密码套件配置。
- **重大变更**: 使用少于 250 位椭圆曲线（例如 secp192r1/secp224r1）的证书/对等方在证书和 TLS 中不再受支持。
- **注意**: 避免依赖 Mbed TLS 私有声明（例如 ``mbedtls/private/`` 下的头文件或通过 ``MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS`` / ``MBEDTLS_ALLOW_PRIVATE_ACCESS`` 启用的声明）。这些私有接口可能会在没有通知的情况下更改。
- **注意**: PSA Crypto 迁移（TF-PSA-Crypto）可能会增加 flash 占用，具体取决于启用的功能。以下是参考数据：

  .. list-table::
     :header-rows: 1
     :widths: 30 15 15 15 10

     * - 示例
       - 非 PSA 构建 (字节)
       - PSA 迁移后 (字节)
       - 差异 (字节)
       - 差异 (%)
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

参考资料
^^^^^^^^

- :idf_file:`Mbed TLS 4.0 迁移指南 <components/mbedtls/mbedtls/docs/4.0-migration-guide.md>`
- :idf_file:`TF-PSA-Crypto 1.0 迁移指南 <components/mbedtls/mbedtls/tf-psa-crypto/docs/1.0-migration-guide.md>`
- :idf_file:`TF-PSA-Crypto PSA 过渡说明 <components/mbedtls/mbedtls/tf-psa-crypto/docs/psa-transition.md>`

上游 Mbed TLS PSA 说明
^^^^^^^^^^^^^^^^^^^^^^

一些以前应用程序可能访问过的数据结构和内部实现，在使用基于 PSA 的 Mbed TLS 版本时不再可用。如果你的应用程序依赖于直接访问 Mbed TLS 内部状态（例如作为结构体字段的熵/DRBG 上下文），请迁移到受支持的公共 API。

ESP-IDF 上的 PSA 持久存储 (ITS)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ESP-IDF 提供了由 NVS 支持的 PSA 内部可信存储 (ITS) 实现，因此可以在没有文件系统的情况下使用 PSA 持久存储。如果你的应用程序使用 PSA 持久密钥/存储，请确保 NVS 在首次使用前可用并已初始化。

已移除的废弃 API（Mbed TLS / 加密）
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

以下废弃函数已被移除：

- :cpp:func:`esp_aes_encrypt` – 请使用 :cpp:func:`esp_internal_aes_encrypt` 代替。
- :cpp:func:`esp_aes_decrypt` – 请使用 :cpp:func:`esp_internal_aes_decrypt` 代替。
- :cpp:func:`esp_crypto_shared_gdma_start` – 请使用 :cpp:func:`esp_crypto_shared_gdma_start_axi_ahb` 代替。

注意，新的 AES 函数返回错误代码以提供更好的错误处理，与返回 void 的旧函数不同。

BluFi
-----

BluFi（通过 BLE 进行 Wi-Fi 配网）受 ESP-IDF v6.0 中 Mbed TLS v4.x / PSA Crypto 迁移的影响。

- **重大变更**: BluFi 协议版本已更新（``BTC_BluFi_SUB_VER`` 从 ``0x03`` 升级到 ``0x04``）。ESP-IDF 使用的 BluFi 安全协商实现也已更新为使用 PSA Crypto（请参阅更新后的 ``examples/bluetooth/blufi`` 示例）。

  **影响**: 基于旧版 BluFi 加密/协议实现构建的现有 BluFi 客户端应用程序（例如手机应用）可能无法再与使用 ESP-IDF v6.0 构建的设备互操作。这通常表现为尝试配网时 BluFi 协商/连接失败。

  **需要采取的措施**: 请同时更新两端：

  - 将设备固件更新到 ESP-IDF v6.0。
  - 将 BluFi 客户端应用程序更新到与 ESP-IDF v6.0 使用的更新后 BluFi 协议/安全协商兼容的版本。

引导加载程序支持
----------------

**已移除的废弃 API**

以下废弃函数已被移除：

- :cpp:func:`esp_secure_boot_verify_signature_block` – 请使用 :cpp:func:`esp_secure_boot_verify_ecdsa_signature_block` 代替。

.. only:: SOC_HMAC_SUPPORTED

    NVS 安全方案
    ------------

    - 当 SoC 具备 HMAC 外设并启用了 flash 加密时，如果同时还启用了 NVS 加密，则默认会选择基于 HMAC 的 NVS 加密方案，而不是基于 flash 加密的方案。如果你的应用程序之前使用基于 flash 加密的方案，则需要通过 ``menuconfig`` 或项目的 ``sdkconfig`` 文件，手动将 NVS 加密方案从 HMAC 配置为 flash 加密（即设置 ``CONFIG_NVS_SEC_KEY_PROTECT_USING_FLASH_ENC=y``）。
