数字签名 (DS)
=============

:link_to_translation:`en:[English]`

数字签名 (DS) 模块利用 RSA 硬件加速器为信息签名。HMAC 作为密钥派生函数，使用 eFuse 作为输入密钥，输出一组加密参数。随后，数字签名模块利用这组预加密的参数，计算出签名。以上过程均在硬件中完成，因此在计算签名时，软件无法获取 RSA 参数的解密密钥，也无法获取 HMAC 密钥派生函数的输入密钥。

签名计算所涉及的硬件信息以及所用寄存器的有关信息，请参阅 **{IDF_TARGET_NAME} 技术参考手册** > **数字签名 (DS)** [`PDF <{IDF_TARGET_TRM_CN_URL}#digsig>`__]。


私钥参数
--------

RSA 签名的私钥参数存储在 flash 中。为防止发生未经授权的访问，这些参数都进行了 AES 加密。此时，HMAC 模块被作为密钥派生函数，用于计算 AES 加密了的私钥参数。同时，HMAC 模块本身使用的来自 eFuse 密钥块的密钥也具有读取保护，可以防止发生未经授权的访问。

调用签名计算时，软件只需指定使用的 eFuse 密钥、相应的 eFuse 密钥用途、加密的 RSA 参数位置以及待签名的数据或信息。

密钥生成
---------

在使用 DS 外设前，需首先创建并存储 HMAC 密钥和 RSA 私钥，此步骤可在 {IDF_TARGET_NAME} 上通过软件完成，也可在主机上进行。在 ESP-IDF 中，可以使用 :cpp:func:`esp_efuse_write_block` 设置 HMAC 密钥，并使用 :cpp:func:`esp_hmac_calculate` 对 RSA 私钥参数进行加密。

计算并组装私钥参数的详细信息，请参阅 **{IDF_TARGET_NAME} 技术参考手册** > **数字签名 (DS)** [`PDF <{IDF_TARGET_TRM_CN_URL}#digsig>`__]。

在 ESP-IDF 中进行数字签名计算
----------------------------------

在 ESP-IDF 中进行数字签名计算的工作流程，以及所用寄存器的有关信息，请参阅 **{IDF_TARGET_NAME} 技术参考手册** > **数字签名 (DS)** [`PDF <{IDF_TARGET_TRM_CN_URL}#digsig>`__]。

要进行数字签名计算，需要准备以下三个参数：

#. 用作 HMAC 密钥的 eFuse 密钥块 ID
#. 加密私钥参数的位置
#. 待签名的数据或信息

由于签名计算需要一些时间，ESP-IDF 提供了两种可用的 API。第一种是 :cpp:func:`esp_ds_sign`，调用此 API 后，程序会在计算完成前保持阻塞状态。如果在计算过程中，软件需要执行其他操作，则可以调用 :cpp:func:`esp_ds_start_sign`，用另一种方式启动签名计算，然后周期性地调用 :cpp:func:`esp_ds_is_busy`，检查计算是否已完成。一旦计算完成，即可调用 :cpp:func:`esp_ds_finish_sign` 来获取签名结果。

API :cpp:func:`esp_ds_sign` 和 :cpp:func:`esp_ds_start_sign` 会借助 DS 外设计算明文 RSA 签名。RSA 签名需要转换成合适的格式，以供进一步使用。例如，MbedTLS SSL 栈支持 PKCS#1 格式，使用 API :cpp:func:`esp_ds_rsa_sign` 可以直接获得 PKCS#1 v1.5 格式的签名，该 API 内部调用了 :cpp:func:`esp_ds_start_sign` 函数，并将签名转换成 PKCS#1 v1.5 格式。

.. note::

    此处只是最基本的 DS 构造块，其消息必须是固定长度。为在任意消息上创建签名，通常会将实际消息的哈希值作为输入，并将其填充到所需长度。乐鑫计划在未来提供一个 API 来实现这个功能。

.. _configure-the-ds-peripheral:

TLS 连接所需的 DS 外设配置
------------------------------

在 {IDF_TARGET_NAME} 芯片上使用 TLS 连接之前，必须先配置 DS 外设，具体步骤如下：

1) 随机生成一个 256 位的值，作为 ``初始化向量`` (IV)。
2) 随机生成一个 256 位的值，作为 ``HMAC_KEY``。
3) 使用客户端私钥 (RAS) 和上述步骤生成的参数，计算加密的私钥参数。
4) 将 256 位的 ``HMAC_KEY`` 烧录到 eFuse 中，只支持 DS 外设读取。

更多细节，请参阅 **{IDF_TARGET_NAME} 技术参考手册** > **数字签名 (DS)** [`PDF <{IDF_TARGET_TRM_CN_URL}#digsig>`__]。

如果要以开发为目的配置 DS 外设，可以使用 `esp-secure-cert-tool <https://pypi.org/project/esp-secure-cert-tool>`_。

配置完 DS 外设后获取的加密私钥参数需要保存在 flash 中并传递给 DS 外设，DS 外设将使用这些参数完成数字签名。随后，应用程序需要从 flash 中读取 DS 数据，这可以通过 `esp_secure_cert_mgr <https://github.com/espressif/esp_secure_cert_mgr>`_ 组件提供的 API 完成。更多细节，请参阅 `component/README <https://github.com/espressif/esp_secure_cert_mgr#readme>`_。

在 esp_tls 仓库内部，`ESP-TLS` 负责完成初始化 DS 外设、执行数字签名的过程。更多细节，请参阅 :ref:`digital-signature-with-esp-tls`。

如 `ESP-TLS` 文档所述，应用程序只需将加密私钥参数作为 `ds_data` 传递给 esp_tls 上下文，esp_tls 仓库内部就会执行所有必要操作，以初始化 DS 外设，并执行数字签名。

使用 DS 外设进行 SSL 双向认证
-----------------------------

示例 :example:`protocols/mqtt/ssl_ds` 展示了如何使用 DS 外设进行 SSL 双向认证。在示例中，使用了 `mqtt_client` （通过 `ESP-MQTT` 实现），通过 SSL 传输连接到代理服务器 ``test.mosquitto.org``，并进行 SSL 双向认证。SSL 部分在内部使用 `ESP-TLS` 完成。更多细节，请参阅 :example_file:`protocols/mqtt/ssl_ds/README.md`。

API 参考
--------

.. include-build-file:: inc/esp_ds.inc
