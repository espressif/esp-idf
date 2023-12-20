ESP-TLS
=======

:link_to_translation:`en:[English]`

概述
--------

ESP-TLS 组件提供简化 API 接口，用于访问常用 TLS 功能，支持如 CA 认证验证、SNI、ALPN 协商和非阻塞连接等常见场景，相关配置可在数据结构体 ``esp_tls_cfg_t`` 中指定。配置完成后，使用以下 API 进行 TLS 通信：

    * :cpp:func:`esp_tls_init`：初始化 TLS 连接句柄。
    * :cpp:func:`esp_tls_conn_new_sync`：开启新的阻塞式 TLS 连接。
    * :cpp:func:`esp_tls_conn_new_async`：开启新的非阻塞式 TLS 连接。
    * :cpp:func:`esp_tls_conn_read`：读取 TLS 层之上的应用数据。
    * :cpp:func:`esp_tls_conn_write`：将应用数据写入 TLS 连接。
    * :cpp:func:`esp_tls_conn_destroy`：释放连接。

任何应用层协议，如 HTTP1、HTTP2 等，均可调用 ESP-TLS 组件接口实现。

应用示例
-------------------

使用 ESP-TLS 建立安全套接字连接的 HTTPS 简单示例，请参阅 :example:`protocols/https_request`。

ESP-TLS 组件的树形结构
-------------------------------------

    .. code-block:: none

        ├── esp_tls.c
        ├── esp_tls.h
        ├── esp_tls_mbedtls.c
        ├── esp_tls_wolfssl.c
        └── private_include
            ├── esp_tls_mbedtls.h
            └── esp_tls_wolfssl.h

ESP-TLS 组件文件 :component_file:`esp-tls/esp_tls.h` 包含该组件的公共 API 头文件。在 ESP-TLS 组件内部，为了实现安全会话功能，会使用 MbedTLS 和 WolfSSL 两个 SSL/TLS 库中的其中一个进行安全会话的建立，与 MbedTLS 相关的 API 存放在 :component_file:`esp-tls/private_include/esp_tls_mbedtls.h`，而与 WolfSSL 相关的 API 存放在 :component_file:`esp-tls/private_include/esp_tls_wolfssl.h`。

.. _esp_tls_server_verification:

TLS 服务器验证
-----------------------

ESP-TLS 在客户端提供了多种验证 TLS 服务器的选项，如验证对端服务器的服务器证书、或使用预共享密钥验证服务器。用户应在 :cpp:type:`esp_tls_cfg_t` 结构体中选择以下任一选项完成 TLS 服务器验证，若未做选择，则客户端默认在 TLS 连接创建时，会返回错误。

    *  **cacert_buf** 和 **cacert_bytes**：以缓冲区的形式向 :cpp:type:`esp_tls_cfg_t` 结构体提供 CA 证书，ESP-TLS 将使用缓冲区中的 CA 证书验证服务器。注意，须在 :cpp:type:`esp_tls_cfg_t` 结构体中设置以下变量：

        * ``cacert_buf`` - 指针，指向包含 CA 证书的缓冲区。
        * ``cacert_bytes`` - CA 证书大小（以字节为单位）。
    * **use_global_ca_store**： ``global_ca_store`` 可一次性完成初始化及设置，并用于验证 ESP-TLS 连接的服务器，注意需要在这些服务器各自的 :cpp:type:`esp_tls_cfg_t` 结构体中设置 ``use_global_ca_store = true``。有关初始化和设置 ``global_ca_store`` 的不同 API，请参阅文末的 API 参考。
    * **crt_bundle_attach**：ESP x509 证书包 API 提供了便捷的服务器验证方法，即打包一组自定义的 x509 根证书，用于 TLS 服务器验证，详情请参阅 :doc:`ESP x509 证书包 </api-reference/protocols/esp_crt_bundle>`。
    * **psk_hint_key**：要使用预共享密钥验证服务器，必须在 ESP-TLS menuconfig 中启用 :ref:`CONFIG_ESP_TLS_PSK_VERIFICATION`，然后向结构体 :cpp:type:`esp_tls_cfg_t` 提供指向 PSK 提示和密钥的指针。若未选择有关服务器验证的其他选项，ESP-TLS 将仅用 PSK 验证服务器。
    * **跳过服务器验证**：该选项并不安全，仅供测试使用。在 ESP-TLS menuconfig 中启用 :ref:`CONFIG_ESP_TLS_INSECURE` 和 :ref:`CONFIG_ESP_TLS_SKIP_SERVER_CERT_VERIFY` 可启用该选项，此时，若未在 :cpp:type:`esp_tls_cfg_t` 结构体选择其他服务器验证选项，ESP-TLS 将默认跳过服务器验证。

    .. warning::

        启用 **跳过服务器验证** 选项存在潜在风险，若未通过 API 或 ``ca_store`` 等其他机制提供服务器证书，可能导致设备与伪造身份的服务器建立 TLS 连接。

ESP-TLS 服务器证书选择回调
----------------------------------

使用 MbedTLS 协议栈时，ESP-TLS 组件支持设置服务器证书选择回调函数。此时，在服务器握手期间可选择使用哪个服务器证书，该回调可获取客户端发送的 "Client Hello" 消息中提供的 TLS 扩展（ALPN、SPI 等），并基于此选择传输哪个服务器证书给客户端。要启用此功能，请在 ESP-TLS menuconfig 中启用 :ref:`CONFIG_ESP_TLS_SERVER_CERT_SELECT_HOOK`。

证书选择回调可在结构体 :cpp:type:`esp_tls_cfg_t` 中配置，具体如下：

.. code-block:: c

    int cert_selection_callback(mbedtls_ssl_context *ssl)
    {
        /* 回调应执行的代码 */
        return 0;
    }

    esp_tls_cfg_t cfg = {
        cert_select_cb = cert_section_callback,
    };

.. _esp_tls_wolfssl:

底层 SSL/TLS 库选择
----------------------------------

ESP-TLS 组件支持以 MbedTLS 或 WolfSSL 作为其底层 SSL/TLS 库，默认仅使用 MbedTLS，WolfSSL 的 SSL/TLS 库可在 https://github.com/espressif/esp-wolfssl 上公开获取，该仓库提供二进制格式的 WolfSSL 组件，并提供了一些示例帮助用户了解相关 API。有关许可证和其他选项，请参阅仓库的 ``README.md`` 文件。下文介绍了在工程中使用 WolfSSL 的具体流程。

.. note::

    库选项位于 ESP-TLS 内部，因此切换库不会更改工程的 ESP-TLS 特定代码。

在 ESP-IDF 使用 WolfSSL
----------------------------------------

要在工程中使用 WolfSSL，可采取以下两种方式：

1) 使用以下三行命令，将 WolfSSL 作为组件直接添加到工程中::

    （首先用 cd 命令进入工程目录）
    mkdir components
    cd components
    git clone https://github.com/espressif/esp-wolfssl.git

2) 将 WolfSSL 作为额外组件添加到工程中。

* 使用以下命令下载 WolfSSL::

    git clone https://github.com/espressif/esp-wolfssl.git

* 参照 `wolfssl/examples <https://github.com/espressif/esp-wolfssl/tree/master/examples>`_ 示例，在工程的 ``CMakeLists.txt`` 文件中设置 ``EXTRA_COMPONENT_DIRS``，从而在 ESP-IDF 中包含 ESP-WolfSSL，详情请参阅 :doc:`构建系统 </api-guides/build-system>` 中的 :ref:`optional_project_variable` 小节。

完成上述步骤后，可以在工程配置菜单中将 WolfSSL 作为底层 SSL/TLS 库，具体步骤如下::

    idf.py menuconfig > ESP-TLS > SSL/TLS Library > Mbedtls/Wolfssl

MbedTLS 与 WolfSSL 对比
--------------------------------------

下表是在使用 WolfSSL 和 MbedTLS 两种 SSL/TLS 库，并将所有相关配置设置为默认值时，运行具有服务器身份验证的 :example:`protocols/https_request` 示例的比较结果。对于 MbedTLS，IN_CONTENT 长度和 OUT_CONTENT 长度分别设置为 16384 字节和 4096 字节。

.. list-table::
    :header-rows: 1
    :widths: 40 30 30
    :align: center

    * - 属性
      - WolfSSL
      - MbedTLS
    * - 总消耗堆空间
      - ~ 19 KB
      - ~ 37 KB
    * - 任务栈使用
      - ~ 2.2 KB
      - ~ 3.6 KB
    * - 二进制文件大小
      - ~ 858 KB
      - ~ 736 KB

.. note::

    若配置选项不同或相应库的版本不同，得到的值可能与上表不同。

.. only:: esp32

    ESP-TLS 中的 ATECC608A（安全元件）
    --------------------------------------------------

    ESP-TLS 支持在 ESP32 系列芯片上使用 ATECC608A 加密芯片，但必须将 MbedTLS 作为 ESP-TLS 的底层 SSL/TLS 协议栈。未经手动更改，ESP-TLS 默认以 MbedTLS 为其底层 TLS/SSL 协议栈。

    .. note::

        在 ESP32 上的 ATECC608A 芯片必须预先配置，详情请参阅 `esp_cryptoauth_utility <https://github.com/espressif/esp-cryptoauthlib/blob/master/esp_cryptoauth_utility/README.md#esp_cryptoauth_utility>`_。

    要启用安全元件支持，并将其应用于工程 TLS 连接，请遵循以下步骤：

    1) 在工程中添加 `esp-cryptoauthlib <https://github.com/espressif/esp-cryptoauthlib>`_，详情请参阅 `如何在 ESP-IDF 中使用 esp-cryptoauthlib <https://github.com/espressif/esp-cryptoauthlib#how-to-use-esp-cryptoauthlib-with-esp-idf>`_。

    2) 启用以下 menuconfig 选项::

        menuconfig > Component config > ESP-TLS > Use Secure Element (ATECC608A) with ESP-TLS

    3) 选择 ATECC608A 芯片类型::

        menuconfig > Component config > esp-cryptoauthlib > Choose Type of ATECC608A chip

    如需了解更多 ATECC608A 芯片类型，或需了解如何获取连接到特定 ESP 模块的 ATECC608A 芯片类型，请参阅 `ATECC608A 芯片类型 <https://github.com/espressif/esp-cryptoauthlib/blob/master/esp_cryptoauth_utility/README.md#find-type-of-atecc608a-chip-connected-to-esp32-wroom32-se>`_。

    4) 在 :cpp:type:`esp_tls_cfg_t` 中提供以下配置，在 ESP-TLS 中启用 ATECC608A。

    .. code-block:: c

            esp_tls_cfg_t cfg = {
                /* 其他配置选项 */
                .use_secure_element = true,
            };

.. only:: SOC_DIG_SIGN_SUPPORTED

    .. _digital-signature-with-esp-tls:

    ESP-TLS 的数字签名
    ----------------------------------

    ESP-TLS 支持在 {IDF_TARGET_NAME} 中使用数字签名 (DS)，但只有当 ESP-TLS 以 MbedTLS（默认协议栈）为底层 SSL/TLS 协议栈时，才支持使用 TLS 的数字签名。有关数字签名的详细信息，请参阅 :doc:`数字签名 (DS) </api-reference/peripherals/ds>`。有关数字签名的技术细节（例如私钥参数计算），请参阅 **{IDF_TARGET_NAME} 技术参考手册** > **数字签名 (DS)** [`PDF <{IDF_TARGET_TRM_EN_URL}#digsig>`__]。在使用数字签名前，应预先配置数字签名外设，请参阅 :ref:`configure-the-ds-peripheral`。

    数字签名外设必须用所需的加密私钥参数初始化，相应参数在配置数字签名外设时获取。具备所需的数字签名上下文，即数字签名参数时，ESP-TLS 会在内部初始化数字签名外设。要将数字签名上下文传递给 ESP-TLS 上下文，请参阅以下代码段。注意，在删除 TLS 连接之前，不应释放传递给 ESP-TLS 上下文的数字签名上下文。

    .. code-block:: c

            #include "esp_tls.h"
            esp_ds_data_ctx_t *ds_ctx;
            /* 使用加密的私钥参数初始化 ds_ctx，这类参数可以从 nvs 中读取，或由应用程序代码提供 */
            esp_tls_cfg_t cfg = {
                .clientcert_buf = /* 客户端证书 */,
                .clientcert_bytes = /* 客户端证书长度 */,
                /* 其他配置选项 */
                .ds_data = (void *)ds_ctx,
            };

    .. note::

        当使用数字签名进行 TLS 连接时，除其他必要参数外，仅需提供客户端证书 (``clientcert_buf``) 和数字签名参数 (``ds_data``) ，此时可将客户端密钥 (``clientkey_buf``) 设置为 NULL。

    * 使用数字签名外设进行双向认证的示例请参阅 :example:`SSL 双向认证 <protocols/mqtt/ssl_mutual_auth>`，该示例使用 ESP-TLS 实现 TLS 连接。

.. only:: SOC_ECDSA_SUPPORTED

    .. _ecdsa-peri-with-esp-tls:

    在 ESP-TLS 中使用 ECDSA 外设
    -----------------------------

    ESP-TLS 支持在 {IDF_TARGET_NAME} 中使用 ECDSA 外设。使用 ECDSA 外设时，ESP-TLS 必须与 MbedTLS 一起作为底层 SSL/TLS 协议栈，并且 ECDSA 的私钥应存储在 eFuse 中。请参考 `espefuse.py <https://docs.espressif.com/projects/esptool/en/latest/esp32/espefuse/index.html>`__ 文档，了解如何在 eFuse 中烧写 ECDSA 密钥。
    在 ESP-TLS 中启用 ECDSA 外设前，请将 :cpp:member:`esp_tls_cfg_t::use_ecdsa_peripheral` 设置为 `true`，并将 :cpp:member:`esp_tls_cfg_t::ecdsa_key_efuse_blk` 设置为存储了 ECDSA 密钥的 eFuse 块 ID。
    这样就可以使用 ECDSA 外设进行私钥操作。由于客户私钥已经存储在 eFuse 中，因此无需将其传递给 :cpp:type:`esp_tls_cfg_t`。

    .. code-block:: c

        #include "esp_tls.h"
        esp_tls_cfg_t cfg = {
            .use_ecdsa_peripheral = true,
            .ecdsa_key_efuse_blk = /* 存储 ECDSA 私钥的 eFuse 块 */,
        };

    .. note::

        在 TLS 中使用 ECDSA 外设时，只支持 ``MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256`` 密码套件。如果使用 TLS v1.3，则支持 ``MBEDTLS_TLS1_3_AES_128_GCM_SHA256`` 密码套件。


TLS 加密套件
------------------------------------

ESP-TLS 支持在客户端模式下设置加密套件列表，TLS 密码套件列表用于向服务器传递所支持的密码套件信息，用户可以根据自己需求增减加密套件，且适用于任何 TLS 协议栈配置。如果服务器支持列表中的任一密码套件，则 TLS 连接成功，反之连接失败。

连接客户端时，在 :cpp:type:`esp_tls_cfg_t` 结构体中设置 ``ciphersuites_list`` 的步骤如下：

.. code-block:: c

    /* 加密套件列表必须以 0 结尾，并且在整个 TLS 连接期间，加密套件的内存地址空间有效 */
    static const int ciphersuites_list[] = {MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384, MBEDTLS_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384, 0};
    esp_tls_cfg_t cfg = {
        .ciphersuites_list = ciphersuites_list,
    };

ESP-TLS 不会检查 ``ciphersuites_list`` 的有效性，因此需调用 :cpp:func:`esp_tls_get_ciphersuites_list` 获取 TLS 协议栈中支持的加密套件列表，并检查设置的加密套件是否在支持的加密套件列表中。

.. note::

   此功能仅在 MbedTLS 协议栈中有效。

TLS 协议版本
--------------------

ESP-TLS 能够为 TLS 连接设置相应的 TLS 协议版本，指定版本将用于建立专用 TLS 连接。也就是说，在运行时不同的 TLS 连接可以配置到 TLS 1.2、TLS 1.3 等不同协议版本。

.. note::

   目前，仅在 MbedTLS 作为 ESP-TLS 的底层 SSL/TLS 协议栈时支持此功能。

要在 ESP-TLS 中设置 TLS 协议版本，请设置 :cpp:member:`esp_tls_cfg_t::tls_version`，从 :cpp:type:`esp_tls_proto_ver_t` 中选择所需版本。如未指定协议版本字段，将默认根据服务器要求建立 TLS 连接。

ESP-TLS 连接的协议版本可按如下方式配置：

    .. code-block:: c

        #include "esp_tls.h"
        esp_tls_cfg_t cfg = {
            .tls_version = ESP_TLS_VER_TLS_1_2,
        };

API 参考
-------------

.. include-build-file:: inc/esp_tls.inc
.. include-build-file:: inc/esp_tls_errors.inc
