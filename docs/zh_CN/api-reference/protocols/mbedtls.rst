Mbed TLS
========

:link_to_translation:`en:[English]`

`Mbed TLS <https://github.com/Mbed-TLS/mbedtls>`_ 是一个 C 代码库，用于实现加密基元、X.509 证书操作以及 SSL/TLS 和 DTLS 协议。该库代码占用空间小，适合嵌入式系统使用。

.. note::

    ESP-IDF 使用的 Mbed TLS `复刻仓库 <https://github.com/espressif/mbedtls>`_ 中包含对原生 Mbed TLS 的补丁。这些补丁与某些模块的硬件例程有关，如 ``bignum (MPI)`` 和 ``ECC``。

Mbed TLS 通过提供以下内容支持 TLS 1.2、TLS 1.3 和 DTLS 1.2 通信：

- TCP/IP 通信功能：监听、连接、接收、读/写。
- SSL/TLS 通信功能：初始化、握手、读/写。
- X.509 功能：CRT、CRL 和密钥处理
- 随机数生成
- 哈希
- 加密/解密

.. note::

    Mbed TLS v3.x.x 系列仅支持 TLS 1.2 和 TLS 1.3 协议。对 SSL 3.0、TLS 1.0/1.1 和 DTLS 1.0 的支持已被删除（弃用）。从 Mbed TLS v3.6.0 版本开始完全支持 TLS 1.3，在此版本之前，某些功能仍处于试验状态。有关更多详细信息，请参阅 :component_file:`Mbed TLS ChangeLog <mbedtls/mbedtls/ChangeLog>`。

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

预设配置
^^^^^^^^

ESP-IDF 为 Mbed TLS 提供了基于预设的配置系统，用于简化设置并为不同的应用场景提供优化后的初始配置。这套系统可以和现有的手动配置方式一同使用，先提供基础配置，然后可通过 menuconfig 或其他配置文件对其进行自定义。

.. list-table::
    :header-rows: 1
    :widths: 15 25 35
    :align: center

    * - 预设
      - 使用场景
      - 主要特性
    * - **默认**
      - 通用应用
      - • 支持 TLS 1.2 和 1.3
        • 启用证书包
        • 硬件加速
        • 支持完整的加密算法套件
    * - **最小化**
      - 资源受限应用
      - • 仅 TLS 1.2 客户端
        • RSA 和 PSK 密钥交换
        • AES-128 CBC/CTR 模式
        • 基本的 X.509 解析
    * - **蓝牙 (BT)**
      - 蓝牙应用
      - • 针对低功耗蓝牙安全性需求进行优化
        • 支持 ECC P-256 曲线
        • 最小的 TLS 开销
        • 蓝牙特定算法

使用预设配置
^^^^^^^^^^^^

预设配置是 mbedTLS 配置的 **起点**，你可以直接使用，也可以借助 ESP-IDF 提供的标准配置方式对其进一步自定义。

要使用预设配置，请在调用 ``project()`` **之前**，先在项目的 ``CMakeLists.txt`` 文件中添加以下代码：

.. code-block:: cmake

    # 包含默认预设（适用于大多数应用）
    list(APPEND sdkconfig_defaults $ENV{IDF_PATH}/components/mbedtls/config/mbedtls_preset_default.conf)

    # 适用于资源受限的应用
    list(APPEND sdkconfig_defaults $ENV{IDF_PATH}/components/mbedtls/config/mbedtls_preset_minimal.conf)

    # 适用于蓝牙应用
    list(APPEND sdkconfig_defaults $ENV{IDF_PATH}/components/mbedtls/config/mbedtls_preset_bt.conf)

    # 标准的 ESP-IDF 项目设置
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
    project(my_project)

.. note::

    预设配置存放于 ``components/mbedtls/config/`` 中，你可以自定义预设配置或将其作为基础模板使用。

自定义预设配置
^^^^^^^^^^^^^^

在应用预设后，你可以使用以下任一种方法进一步自定义配置：

**方法 1：使用 menuconfig（推荐）**

.. code-block:: bash

    # 在 CMakeLists.txt 中应用预设后
    idf.py menuconfig

前往 ``Component Config`` > ``mbedTLS`` 修改设置，你的更改将覆盖预设的默认值。

**方法 2：通过其他配置文件**

也可以通过创建其他配置文件，将预设与自定义的配置相结合：

.. code-block:: cmake

    # 基于最小预设，添加自定义设置
    list(APPEND SDKCONFIG_DEFAULTS
        $ENV{IDF_PATH}/components/mbedtls/config/mbedtls_preset_minimal.conf
        ${CMAKE_CURRENT_SOURCE_DIR}/my_custom_mbedtls.conf
    )


从手动配置迁移
^^^^^^^^^^^^^^

预设系统对手动配置进行了补充。假设目前你手动配置了 mbedTLS：

**选项 1：保留当前配置**

当前的手动配置无需任何更改即可工作。

**选项 2：迁移到预设并对其进行自定义**

1. **选择基础预设**，该预设最接近你当前的配置。
2. 在 ``CMakeLists.txt`` 中 **应用预设**。
3. **使用 menuconfig** 调整设置，匹配需求。
4. **全面测试** 以确保功能完整。

配置类别
^^^^^^^^

新的 mbedTLS 配置系统按逻辑进行分类，便于查找：

**核心配置**
    包括内存分配、线程和调试选项的基本 mbedTLS 设置。

**TLS 协议配置**
    TLS/DTLS 协议版本、模式（客户端/服务器）和特定协议的功能。

**对称加密算法**
    块密码（AES、ARIA 等）、加密算法模式（CBC、GCM 等）和对称密码学。

**非对称加密算法**
    RSA、ECC 和其他公钥密码学算法。

**哈希函数**
    消息摘要算法（SHA-256、SHA-512 等）和 HMAC。

**硬件加速**
    为加密操作提供 ESP32 专用的硬件加速支持。

**证书支持**
    X.509 证书解析、验证和证书包管理。


应用示例
--------

ESP-IDF 中的示例使用 :doc:`/api-reference/protocols/esp_tls`，为访问常用的 TLS 功能提供了简化的 API 接口。

参考示例 :example:`protocols/https_server/simple` （简单的 HTTPS 服务器）和 :example:`protocols/https_request` （发起 HTTPS 请求）了解更多信息。

如需直接使用 Mbed TLS API，请参考示例 :example:`protocols/https_mbedtls`。该示例演示了如何用 Mbed TLS 创建 HTTPS 连接。具体做法是配置安全的套接字，并使用证书包进行验证。


重要配置
--------

Mbed TLS 配置系统支持预设配置。``Component Config`` > ``mbedTLS`` 中的部分重要配置选项如下所示。点击 :ref:`此处 <CONFIG_MBEDTLS_MEM_ALLOC_MODE>` 获取完整配置选项列表。

**核心配置：**

.. list::

    :SOC_SHA_SUPPORTED: - :ref:`CONFIG_MBEDTLS_HARDWARE_SHA`：支持硬件 SHA 加速
    :SOC_AES_SUPPORTED: - :ref:`CONFIG_MBEDTLS_HARDWARE_AES`：支持硬件 AES 加速
    :SOC_MPI_SUPPORTED: - :ref:`CONFIG_MBEDTLS_HARDWARE_MPI`：支持硬件 MPI（大数）加速
    :SOC_ECC_SUPPORTED: - :ref:`CONFIG_MBEDTLS_HARDWARE_ECC`：支持硬件 ECC 加速
    - :ref:`CONFIG_MBEDTLS_MEM_ALLOC_MODE`：内存分配策略（内部/外部/自定义）
    - :ref:`CONFIG_MBEDTLS_ASYMMETRIC_CONTENT_LEN`：用于内存优化的非对称输入/输出片段长度
    - :ref:`CONFIG_MBEDTLS_DYNAMIC_BUFFER`：启用动态 TX/RX buffer 分配
    - :ref:`CONFIG_MBEDTLS_DEBUG`：启用 mbedTLS 调试（有助于调试）

**TLS 协议配置：**

.. list::

    - :ref:`CONFIG_MBEDTLS_TLS_ENABLED`：启用 TLS 协议支持
    - :ref:`CONFIG_MBEDTLS_SSL_PROTO_TLS1_2`：支持 TLS 1.2（推荐）
    - :ref:`CONFIG_MBEDTLS_SSL_PROTO_TLS1_3`：支持 TLS 1.3（最新标准）
    - :ref:`CONFIG_MBEDTLS_SSL_PROTO_DTLS`：支持基于 UDP 的 DTLS
    - :ref:`CONFIG_MBEDTLS_CLIENT_SSL_SESSION_TICKETS`：支持 TLS 会话恢复（客户端会话票据）
    - :ref:`CONFIG_MBEDTLS_SERVER_SSL_SESSION_TICKETS`：支持 TLS 会话恢复（服务器会话票据）
    - :ref:`CONFIG_MBEDTLS_SSL_ALPN`：支持应用层协议协商
    - :ref:`CONFIG_MBEDTLS_SSL_SERVER_NAME_INDICATION`：支持服务器名称指示 (SNI)

**证书支持：**

.. list::

    - :ref:`CONFIG_MBEDTLS_CERTIFICATE_BUNDLE`：支持受信任的根证书包（详情请参阅 :doc:`/api-reference/protocols/esp_crt_bundle`）
    - :ref:`CONFIG_MBEDTLS_X509_USE_C`：启用 X.509 证书支持
    - :ref:`CONFIG_MBEDTLS_PEM_PARSE_C`：读取并解析 PEM 格式的证书
    - :ref:`CONFIG_MBEDTLS_PEM_WRITE_C`：编写 PEM 格式的证书
    - :ref:`CONFIG_MBEDTLS_X509_CRT_PARSE_C`：解析 X.509 证书
    - :ref:`CONFIG_MBEDTLS_X509_CRL_PARSE_C`：解析 X.509 证书吊销列表

**加密算法：**

.. list::

    - :ref:`CONFIG_MBEDTLS_AES_C`：支持 AES 块密码
    - :ref:`CONFIG_MBEDTLS_RSA_C`：RSA 公钥密码系统
    - :ref:`CONFIG_MBEDTLS_ECP_C`：支持椭圆曲线密码学
    - :ref:`CONFIG_MBEDTLS_ECDSA_C`：椭圆曲线数字签名算法
    - :ref:`CONFIG_MBEDTLS_ECDH_C`：椭圆曲线 Diffie-Hellman 密钥交换
    - :ref:`CONFIG_MBEDTLS_SHA256_C`：SHA-256 哈希函数
    - :ref:`CONFIG_MBEDTLS_SHA512_C`：SHA-512 哈希函数
    - :ref:`CONFIG_MBEDTLS_GCM_C`：Galois/Counter 模式用于认证加密

.. note::

    新的配置结构将各项配置按照“核心配置”、“TLS 协议配置”、“对称加密算法”、“非对称加密算法”、“哈希函数”和“硬件加速”等类别进行分类，便于查找和管理配置。

调试 mbedTLS
^^^^^^^^^^^^

为启用调试，请添加以下配置：

.. code-block:: kconfig

    CONFIG_MBEDTLS_DEBUG=y
    CONFIG_MBEDTLS_DEBUG_LEVEL=3
    CONFIG_LOG_DEFAULT_LEVEL_DEBUG=y

性能优化
^^^^^^^^

为获得最佳性能，请尽可能 **启用硬件加速**：

.. code-block:: kconfig

    CONFIG_MBEDTLS_HARDWARE_AES=y
    CONFIG_MBEDTLS_HARDWARE_SHA=y
    CONFIG_MBEDTLS_HARDWARE_MPI=y
    CONFIG_MBEDTLS_HARDWARE_ECC=y

性能和内存调整
--------------

.. _reducing_ram_usage_mbedtls:

减少内存使用
^^^^^^^^^^^^^^

下表展示了在不同配置下，用 Mbed TLS 作为 SSL/TLS 库运行示例 :example:`protocols/https_request` （启用服务器验证）时，内存的实际使用情况。

.. list-table::
    :header-rows: 1
    :widths: 25 60 30
    :align: center

    * - Mbed TLS 测试
      - 相关配置
      - 堆使用（近似值）
    * - 默认
      - NA
      - 42196 B
    * - 启用 SSL 动态 buffer 长度
      - :ref:`CONFIG_MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH`
      -  42120 B
    * - 禁用保留对端证书
      - :ref:`CONFIG_MBEDTLS_SSL_KEEP_PEER_CERTIFICATE`
      - 38533 B
    * - 启用动态 TX/RX buffer
      - :ref:`CONFIG_MBEDTLS_DYNAMIC_BUFFER`
        :ref:`CONFIG_MBEDTLS_DYNAMIC_FREE_CONFIG_DATA`
        ::ref:`CONFIG_MBEDTLS_DYNAMIC_FREE_CA_CERT`
      - 22013 B

.. note::

    这些值会随着配置选项和 Mbed TLS 版本的变化而变化。


减小固件大小
^^^^^^^^^^^^^^^^^^

在 ``Component Config`` > ``mbedTLS`` 配置中，多个 Mbed TLS 功能已默认启用。如无需使用，可以禁用以减小固件大小。详情请参阅 :ref:`最小化固件大小 <minimizing_binary_mbedtls>`。


.. _`API Reference`: https://mbed-tls.readthedocs.io/projects/api/en/v3.6.5/
.. _`Knowledge Base`: https://mbed-tls.readthedocs.io/en/latest/kb/
