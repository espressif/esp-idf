Mbed TLS
========

:link_to_translation:`en:[English]`

`Mbed TLS <https://github.com/Mbed-TLS/mbedtls>`_ 是一个 C 代码库，用于实现加密基元、X.509 证书操作以及 SSL/TLS 和 DTLS 协议。该库代码占用空间小，适合嵌入式系统使用。

.. note::

    ESP-IDF 使用的 Mbed TLS `复刻仓库 <https://github.com/espressif/mbedtls>`_ 中包含对原生 Mbed TLS 的补丁。这些补丁与某些模块的硬件例程有关，如 ``bignum (MPI)`` 和 ``ECC``。

Mbed TLS 提供以下功能：

- TCP/IP 通信功能：监听、连接、接收、读/写。
- SSL/TLS 通信功能：初始化、握手、读/写。
- X.509 功能：CRT、CRL 和密钥处理
- 随机数生成
- 哈希
- 加密/解密

TLS 版本支持 SSL 3.0, TLS 1.0、TLS 1.1、TLS 1.2 和 TLS 1.3，但是最新的 ESP-IDF 上 Mbed TLS 已经移除了 SSL 3.0、TLS 1.0 和 TLS 1.1。DTLS 版本支持 DTLS 1.0、DTLS 1.1 和 DTLS 1.2，但最新的 ESP-IDF 上 Mbed TLS 已经移除了 DTLS 1.0。


Mbed TLS 文档
------------------

Mbed TLS 文档请参阅以下（上游）指针：

- `API Reference`_
- `Knowledge Base`_

ESP-IDF 的 Mbed TLS 支持
------------------------------

请在 `此处 <https://github.com/espressif/mbedtls/wiki#mbed-tls-support-in-esp-idf>`__ 查找 ESP-IDF 不同分支上的 Mbed TLS 版本信息。

.. note::

    参考 :ref:`migration_guide_mbedtls` 从 Mbed TLS 2.x 版本迁移到 3.0 及以上版本。

应用示例
-------------

ESP-IDF 中的示例使用 :doc:`/api-reference/protocols/esp_tls`，为访问常用的 TLS 功能提供了一个简化 API 接口。

参考示例 :example:`protocols/https_server/simple` （简单的 HTTPS 服务器）和 :example:`protocols/https_request` （发出 HTTPS 请求）了解更多信息。

如需直接使用 Mbed TLS API，请参考示例 :example:`protocols/https_mbedtls`。


其他选项
----------

:doc:`/api-reference/protocols/esp_tls` 是底层 SSL/TLS 库的抽象层，因此可以选择使用 Mbed TLS 或 wolfSSL 作为底层库。默认情况下，仅 Mbed TLS 可在 ESP-IDF 中使用，而 wolfSSL 在 `<https://github.com/espressif/esp-wolfSSL>`_ 公开，还提供了上游子模块指针的相关信息。

如需了解更多相关信息或比较 Mbed TLS 和 wolfSSL，请参考文档 :ref:`ESP-TLS: Underlying SSL/TLS Library Options <esp_tls_wolfssl>`。


重要配置
-------------

``Component Config -> mbedTLS`` 中的部分重要配置选项如下表所示。点击 :ref:`此处 <CONFIG_MBEDTLS_MEM_ALLOC_MODE>` 获取完整配置选项列表。

.. list::

    - :ref:`CONFIG_MBEDTLS_SSL_PROTO_TLS1_2`: 支持 TLS 1.2
    - :ref:`CONFIG_MBEDTLS_SSL_PROTO_TLS1_3`: 支持 TLS 1.3
    - :ref:`CONFIG_MBEDTLS_CERTIFICATE_BUNDLE`: 支持受信任的根证书包（更多信息请参考 :doc:`/api-reference/protocols/esp_crt_bundle`）
    - :ref:`CONFIG_MBEDTLS_CLIENT_SSL_SESSION_TICKETS`: 支持 TLS 会话恢复：客户端会话票证
    - :ref:`CONFIG_MBEDTLS_SERVER_SSL_SESSION_TICKETS`: 支持 TLS 会话恢复：服务会话票证
    :SOC_SHA_SUPPORTED: - :ref:`CONFIG_MBEDTLS_HARDWARE_SHA`: 支持硬件 SHA 加速
    :SOC_AES_SUPPORTED: - :ref:`CONFIG_MBEDTLS_HARDWARE_AES`: 支持硬件 AES 加速
    :SOC_MPI_SUPPORTED: - :ref:`CONFIG_MBEDTLS_HARDWARE_MPI`: 支持硬件 MPI (bignum) 加速
    :SOC_ECC_SUPPORTED: - :ref:`CONFIG_MBEDTLS_HARDWARE_ECC`: 支持硬件 ECC 加速

.. note::

    Mbed TLS v3.0.0 及其更新版本仅支持 TLS 1.2 和 TLS 1.3，不支持 SSL 3.0、TLS 1.0、TLS 1.1、和 DTLS 1.0)。TLS 1.3 尚在试验阶段，仅支持客户端。要了解更多信息，请点击 `此处 <https://github.com/espressif/mbedtls/blob/9bb5effc3298265f829878825d9bd38478e67514/docs/architecture/tls13-support.md>`__。


性能和内存调整
------------------

.. _reducing_ram_usage_mbedtls:

减少内存使用
^^^^^^^^^^^^^^^^^

下表展示了在不同配置下，用 Mbed TLS 作为 SSL/TLS 库运行示例 :example:`protocols/https_request` （启用服务器验证）时，内存的实际使用情况。

.. list-table::
    :header-rows: 1
    :widths: 25 60 30
    :align: center

    * - Mbed TLS 测试
      - 相关配置
      - 堆使用（近似）
    * - 默认
      - NA
      - 42196 B
    * - 启用 SSL 动态 buffer 长度
      - :ref:`CONFIG_MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH`
      -  42120 B
    * - 禁用保留对端证书
      - :ref:`CONFIG_MBEDTLS_SSL_KEEP_PEER_CERTIFICATE`
      - 38533 B
    * - 启用动态 buffer 功能
      - :ref:`CONFIG_MBEDTLS_DYNAMIC_BUFFER`
        :ref:`CONFIG_MBEDTLS_DYNAMIC_FREE_CONFIG_DATA`
        :ref:`CONFIG_MBEDTLS_DYNAMIC_FREE_CA_CERT`
      - 22013 B

.. note::

    这些值会随着配置选项和 Mbed TLS 版本的变化而变化。


减小固件大小
^^^^^^^^^^^^^^^^^^^^

在 ``Component Config -> mbedTLS`` 中，有多个 Mbed TLS 功能默认为启用状态。如果不需要这些功能，可将其禁用以减小固件大小。要了解更多信息，请参考 :ref:`Minimizing Binary Size <minimizing_binary_mbedtls>` 文档。


.. _`API Reference`: https://mbed-tls.readthedocs.io/projects/api/en/v3.4.1/
.. _`Knowledge Base`: https://mbed-tls.readthedocs.io/en/latest/kb/
