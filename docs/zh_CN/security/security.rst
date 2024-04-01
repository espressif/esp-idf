安全
====

{IDF_TARGET_CIPHER_SCHEME:default="RSA", esp32h2="RSA 或 ECDSA", esp32p4="RSA 或 ECDSA"}

{IDF_TARGET_SIG_PERI:default="DS", esp32h2="DS 或 ECDSA", esp32p4="DS 或 ECDSA"}

:link_to_translation:`en:[English]`

本指南概述了乐鑫解决方案中可用的整体安全功能。从 **安全** 角度考虑，强烈建议在使用乐鑫平台和 ESP-IDF 软件栈设计产品时参考本指南。

目标
----

高级安全目标包括：

#. 防止执行不受信任的代码
#. 保护存储在外部 flash 中代码的可信度和完整性
#. 保护设备身份
#. 安全存储机密数据
#. 设备身份验证与加密通信

平台安全
---------

.. _secure_boot-guide:

安全启动
~~~~~~~~

安全启动功能可以确保设备仅执行通过身份认证的软件。安全启动时，会验证 :doc:`../api-guides/startup` 中所涉及的所有 **可变** 软件实体，形成信任链。设备启动以及 OTA 更新过程中都会进行签名认证。

关于安全启动功能的更多详情，请参阅 :doc:`secure-boot-v2`。

.. only:: esp32

    关于 ESP32 在发布 ECO3 前的安全启动功能，请参阅 :doc:`secure-boot-v1`。

.. important::

    强烈建议在所有生产设备上启用安全启动功能。

安全启动最佳实践
^^^^^^^^^^^^^^^^

* 在具备高质量熵源的系统上生成签名密钥。
* 签名密钥始终保密；签名密钥泄露会危及安全启动系统。
* 禁止第三方使用 ``espsecure.py`` 观察密钥生成或签名过程的相关细节，这两个过程都容易受到时序攻击或其他侧信道攻击的影响。
* 确保正确烧录所有安全性 eFuse，包括禁用调试接口以及非必需的启动介质（例如 UART 下载模式）等。


.. _flash_enc-guide:

flash 加密
~~~~~~~~~~~~~~~~~~~

flash 加密功能可以加密外部 flash 中的内容，从而保护存储在 flash 中软件或数据的 **机密性** 。

关于该功能的更多详情，请参阅 :doc:`flash-encryption`。

.. only:: SOC_SPIRAM_SUPPORTED and not esp32

    如果 {IDF_TARGET_NAME} 连接了外部 SPI RAM，那么写入或读取到 SPI RAM 的内容将会分别进行加密和解密。当启用 flash 加密时，上述过程将通过 MMU 的 flash 缓存实现。以上加密和解密过程为存储在 SPI RAM 中的数据提供了额外的安全层，有助于安全地启用 ``CONFIG_MBEDTLS_EXTERNAL_MEM_ALLOC`` 等特定配置选项。

flash 加密最佳实践
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* 建议在生产环境中使用 flash 加密的发布模式。
* 建议为每个设备生成唯一的 flash 加密密钥。
* 启用 :ref:`secure_boot-guide` 作为额外保护层，防止 flash 在启动前遭受恶意攻击。


.. only:: SOC_DIG_SIGN_SUPPORTED

    设备身份
    ~~~~~~~~~~~~~~~

    在 {IDF_TARGET_NAME} 中，数字签名外设借助硬件加速，通过 HMAC 算法生成 RSA 数字签名。RSA 私钥仅限设备硬件访问，软件无法获取，保证了设备上存储密钥的安全性。

    .. only:: SOC_ECDSA_SUPPORTED

        {IDF_TARGET_NAME} 还支持 ECDSA 外设，用于生成硬件加速的 ECDSA 数字签名。ECDSA 私钥支持直接编程到 eFuse 块中，并在软件中标记为读保护。

    {IDF_TARGET_SIG_PERI} 外设可以建立与远程终端之间的 **安全设备身份**，如基于 {IDF_TARGET_CIPHER_SCHEME} 加密算法的 TLS 双向认证。

    .. only:: not SOC_ECDSA_SUPPORTED

        详情请参阅 :doc:`../api-reference/peripherals/ds`。

    .. only:: SOC_ECDSA_SUPPORTED

        详情请参阅 :doc:`../api-reference/peripherals/ecdsa` 及 :doc:`../api-reference/peripherals/ds`。

.. only:: SOC_MEMPROT_SUPPORTED or SOC_CPU_IDRAM_SPLIT_USING_PMP

    内存保护
    ~~~~~~~~~~~~~~~~~

    {IDF_TARGET_NAME} 可以通过架构或 PMS 等特定外设实现 **内存保护**，强制执行和监控内存以及某些外设的权限属性。使用相应外设，ESP-IDF 应用程序启动代码可以配置数据内存的读取/写入权限以及指令内存的读取/执行权限。如有任何操作尝试违反这些权限属性，如写入指令内存区域，将触发违规中断，导致系统 panic。

    使用该功能需启用配置选项 :ref:`CONFIG_ESP_SYSTEM_MEMPROT_FEATURE`，该选项默认启用。请注意，该功能的 API 是 **私有** 的，仅供 ESP-IDF 代码使用。

    .. note::

        内存保护功能可以防止因软件漏洞导致的远程代码注入。

.. only:: SOC_CRYPTO_DPA_PROTECTION_SUPPORTED

    差分功耗分析 (DPA) 保护
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    {IDF_TARGET_NAME} 支持针对 DPA 相关安全攻击的保护机制。DPA 保护通过动态调整加密外设的时钟频率，在其运行期间模糊了功耗消耗记录。时钟变化范围会根据配置的 DPA 安全级别改变。更多详情请参阅技术参考手册。

    通过 :ref:`CONFIG_ESP_CRYPTO_DPA_PROTECTION_LEVEL` 可以调整 DPA 级别。级别越高安全性越强，但也可能会影响性能。默认启用最低级别 DPA 保护，可以根据安全需求修改。

    .. note::

        请注意，为确保 DPA 保护正常工作，必须启用硬件 :doc:`RNG <../api-reference/system/random>`。

调试接口
~~~~~~~~~~~~~~~~

JTAG
^^^^

.. list::

    - 如果启用了任一安全功能，则 JTAG 接口将保持禁用。更多详情请参阅 :ref:`jtag-debugging-security-features`。
    - 如果不启用其他安全功能，也可以使用 :ref:`efuse_API` 禁用 JTAG 接口。
    :SOC_HMAC_SUPPORTED: - {IDF_TARGET_NAME} 支持软禁用 JTAG 接口，并且可以通过 HMAC 烧录密钥重新启用，请参阅 :ref:`hmac_for_enabling_jtag`。

UART 下载模式
^^^^^^^^^^^^^^^^^^

.. only:: esp32

    对于 ESP32 ECO3，如果在发布配置中启用了任一安全功能，UART 下载模式将保持禁用。也可以在运行时调用 :cpp:func:`esp_efuse_disable_rom_download_mode` 禁用该模式。

    .. important::

        如果禁用了 UART 下载模式，则无法在设备上使用 ``esptool.py``。

.. only:: SOC_SUPPORTS_SECURE_DL_MODE

    {IDF_TARGET_NAME} 中，如果启用了任一安全功能，则会激活安全 UART 下载模式。

    * 要启用安全 UART 下载模式，也可以调用 :cpp:func:`esp_efuse_enable_rom_secure_download_mode`。
    * 该模式下，禁止执行通过 UART 下载模式下载的任意代码。
    * 该模式将限制部分涉及更新 SPI 配置的命令，如更改波特率、基本的 flash 写入以及通过 ``get_security_info`` 返回当前启用的安全功能摘要。
    * 要完全禁用安全 UART 下载模式，可以将 :ref:`CONFIG_SECURE_UART_ROM_DL_MODE` 设置为建议选项 ``Permanently disable ROM Download Mode``，或者在运行时调用 :cpp:func:`esp_efuse_disable_rom_download_mode`。

    .. important::

        安全 UART 下载模式下，仅支持使用 ``--no-stub`` 参数调用 ``esptool.py``。

.. only:: SOC_WIFI_SUPPORTED

    网络安全
    --------------------

    Wi-Fi
    ~~~~~

    除传统安全协议 WEP/WPA-TKIP/WPA2-CCMP 外，ESP-IDF 的 Wi-Fi 驱动程序还支持其他先进的安全协议。详情请参阅 :doc:`../api-guides/wifi-security`。

    TLS（传输层安全性协议）
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    建议在 ESP 设备的所有外部通信中使用 TLS，如云通信、OTA 更新等。:doc:`mbedTLS <../api-reference/protocols/mbedtls>` 是 ESP-IDF 官方支持的 TLS 协议栈。

    TLS 默认集成在 :doc:`../api-reference/protocols/esp_http_client`、 :doc:`../api-reference/protocols/esp_https_server` 和其他几个 ESP-IDF 预置的组件中。

    .. note::

        推荐使用 ESP-IDF 协议组件已确认安全的默认配置。请勿禁用 HTTPS 和类似的安全相关配置。

    ESP-TLS 抽象层
    ^^^^^^^^^^^^^^^^^^^

    ESP-IDF 为最常用的 TLS 功能提供了一个抽象层，因此，建议应用程序使用由 :doc:`../api-reference/protocols/esp_tls` 提供的 API。

    :ref:`esp_tls_server_verification` 部分着重描述了在设备端建立服务器身份的多种方式。

    ESP 证书捆绑包
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^

    调用 :doc:`../api-reference/protocols/esp_crt_bundle` API 即可包含一组自定义 x509 根证书，用于验证 TLS 服务器。对于绝大部分的标准 TLS 服务器，都可以使用证书捆绑包轻松验证服务器身份。

    .. important::

        强烈建议基于 X.509 证书验证服务器身份，谨防与 **伪造** 服务器建立通信。


    根证书管理
    ^^^^^^^^^^^^^

    内嵌在应用程序内的根证书必须谨慎管理。更新根证书列表或 :doc:`../api-reference/protocols/esp_crt_bundle` 都可能影响与远程端点的 TLS 连接，包括与 OTA 更新服务器的连接。在某些情况下，此类问题可能会在后续 OTA 更新中出现，导致设备永远无法进行 OTA 更新。

    根证书列表更新可能出于以下原因：

    - 新固件的远程端点不同。
    - 现有证书过期。
    - 证书已从上游证书包中添加或撤销。
    - 市场份额统计数据的变化引起证书列表的变化（``CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_DEFAULT_CMN`` 情况）。

    其他相关建议：

    - 请考虑启用 :ref:`ota_rollback`，将成功连接至 OTA 更新服务器作为取消回滚过程的检查点，从而确保更新后的固件成功连接至 OTA 更新服务器。否则，回滚过程将导致设备回退到之前的固件版本。
    - 如果计划启用 :ref:`CONFIG_MBEDTLS_HAVE_TIME_DATE` 选项，请确保具备时间同步机制 (SNTP) 和足够的受信任证书。

产品安全
----------------

.. only:: SOC_WIFI_SUPPORTED

    安全配网
    ~~~~~~~~~~~~~~~~~~~

    安全配网是指将 ESP 设备安全接入 Wi-Fi 网络的过程。该机制还支持在初始配网阶段从配网实体（如智能手机等）获取额外的自定义配置数据。

    ESP-IDF 提供了多种安全方案，可以在 ESP 设备和配网实体之间建立安全会话，具体方案请参阅 :ref:`provisioning_security_schemes`。

    关于该功能的更多详情和代码示例，请参阅 :doc:`../api-reference/provisioning/wifi_provisioning`。

    .. note::

        乐鑫提供了 Android 和 iOS 手机应用程序及其源代码，以便进一步根据产品需求定制安全配网方案。

安全 OTA 更新
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- OTA 更新必须通过安全传输进行，如 HTTPS。
- ESP-IDF 为此提供了一个简化的抽象层，即 :doc:`../api-reference/system/esp_https_ota`。
- 如果启用了 :ref:`secure_boot-guide`，则服务器应托管已签名的应用程序镜像。
- 如果启用了 :ref:`flash_enc-guide`，则服务器端不需要额外操作，在 flash 写入时，设备将自动加密。
- OTA 更新的 :ref:`ota_rollback` 可以在验证完应用程序的功能后，再将应用程序切换为 ``active`` 状态。


防回滚保护
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

防回滚保护功能确保设备仅执行特定版本的应用程序，即应满足设备 eFuse 存储的安全版本条件。因此，即使已由合法密钥信任和签名，应用程序可能包含已撤销的安全功能或凭据，因此设备必须拒绝执行此类应用程序。

ESP-IDF 仅支持在应用程序使用该功能，并通过二级引导加载程序管理。安全版本存储在设备 eFuse 中，并在启动时和 OTA 更新期间与应用程序镜像头进行比较。

关于启用此功能的更多详情，请参阅 :ref:`anti-rollback`。

加密固件分发
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

OTA 更新期间，使用加密的固件分发，可以确保在从服务器 **传输** 到设备的过程中，应用程序保持加密。OTA 更新期间，这可以作为在 TLS 通信之上的额外保护层，保护应用程序身份。

关于加密固件分发的工作示例，请参阅 :ref:`ota_updates_pre-encrypted-firmware`。

安全存储
~~~~~~~~~~~~~~

安全存储指在设备上以安全方式存储应用程序的特定数据，即将数据存储在外部 flash 中。外部 flash 通常是可读写的 flash 分区，用于存储设备特定的配置数据，如 Wi-Fi 凭据。

ESP-IDF 提供了 **NVS（非易失性存储）** 管理组件，允许加密数据分区。该功能与上文提到的 :ref:`flash_enc-guide` 平台功能相关。

关于该功能的工作原理和启用说明，请参阅 :ref:`nvs_encryption`。

.. important::

    ESP-IDF 组件会默认将 Wi-Fi 证书等设备特定数据写入 NVS 默认分区，建议使用 **NVS 加密** 功能来保护这些数据。

安全设备控制
~~~~~~~~~~~~~~~~~~~~~

ESP-IDF 提供了 ESP 本地控制组件，可以通过 ``Wi-Fi/Ethernet + HTTP`` 或 ``BLE`` 安全地控制 ESP 设备。

关于该功能的更多详情，请参阅 :doc:`../api-reference/protocols/esp_local_ctrl`。

安全策略
---------------

ESP-IDF GitHub 代码库内含 `安全政策介绍`_。

公告
~~~~~~~~~~

- 乐鑫会发布重要 `安全公告`_ ，包括硬件和软件相关公告。
- ESP-IDF 软件组件的相关安全公告会发布在 `GitHub 仓库`_ 。

软件更新
~~~~~~~~~~~~~~~~

ESP-IDF 会及时处理针对组件和第三方库的相关报告，并修复关键安全问题。修复内容会逐步同步到 ESP-IDF 的所有适用版本分支中。

ESP-IDF 的发布说明将涵盖各 ESP-IDF 组件和第三方库的相应安全问题和 CVE 编号。

.. important::

    为获取所有关键安全修复，建议定期更新到 ESP-IDF 的最新 Bugfix 版本。


.. _`安全政策介绍`: https://github.com/espressif/esp-idf/blob/master/SECURITY.md
.. _`安全公告`: https://www.espressif.com/en/support/documents/advisories
.. _`GitHub 仓库`: https://github.com/espressif/esp-idf/security/advisories
