Wi-Fi 安全性
==============

:link_to_translation:`en:[English]`

{IDF_TARGET_NAME} Wi-Fi 安全功能
-----------------------------------------

- 支持受保护的管理帧 (PMF)
- 支持第三代 Wi-Fi 访问保护 (WPA3-Personal)
- 支持机会性无线加密 (OWE)

除传统安全方法 (WEP/WPA-TKIP/WPA2-CCMP) 外，{IDF_TARGET_NAME} Wi-Fi 还支持最先进的安全协议，即受保护的管理帧 (PMF)、第三代 Wi-Fi 访问保护和基于机会性无线加密技术的增强型开放式™ Wi-Fi 安全协议。WPA3 能更好地保护隐私，并在已知针对传统模式的攻击下具有更强的鲁棒性。增强型开放式™ Wi-Fi 安全协议可以在用户连接到无需认证的开放（公共）Wi-Fi 时，确保用户设备安全，并保护隐私。

受保护的管理帧 (PMF)
---------------------------------

简介
++++++++++++

Wi-Fi 网络中，非 AP 设备使用如信标、探测、（解）身份验证和（断）关联等管理帧，扫描并连接到 AP。不同于数据帧，管理帧在传输时不会加密。

由此，攻击者可以利用窃听和数据包注入，在适当时机发送伪造的（解）身份验证或（断）关联管理帧，发起拒绝服务 (DoS) 和中间人攻击等攻击。

PMF 通过加密单播管理帧以及为广播管理帧提供完整性检查，来应对上述解身份验证、解关联和鲁棒管理帧等攻击。此外，PMF 还提供了安全关联 (SA) 拆除机制，防止伪造的关联或验证帧使已连接的客户端断开连接。

station 和 AP 支持三类 PMF 配置模式：

 - 可选 PMF
 - 强制 PMF
 - 禁用 PMF

API 及用法
++++++++++++

在 station 模式和 SoftAP 模式下，{IDF_TARGET_NAME} 都支持 PMF，两者均默认配置为可选 PMF。为了更高的安全性，可以在使用 :cpp:func:`esp_wifi_set_config` API 时设置 ``pmf_cfg`` 中的 ``required`` 标志，启用强制 PMF 模式。启用后，设备将仅连接到启用了 PMF 的设备，并拒绝其他设备发起的连接。使用 :cpp:func:`esp_wifi_disable_pmf_config` API 可以禁用可选 PMF 配置模式。如果在 WPA3 或 WPA2/WPA3 混合模式下启动了 SoftAP 模式，尝试禁用 PMF 会导致错误。

.. attention::

    为最大限度地利用 PMF 的额外安全性优势，已弃用 ``pmf_cfg`` 中的 ``capable`` 标志，并在内部设置为 ``true``。

企业级 Wi-Fi
----------------------

简介
++++++++

企业级 Wi-Fi (Wi-Fi Enterprise) 为企业无线网络提供安全认证机制。该机制基于 IEEE 802.1X 标准运作，并依赖 RADIUS 服务器在设备连接到接入点 (AP) 之前进行用户身份验证。根据所使用的可扩展认证协议 (EAP) 方法，认证可以基于用户凭证（用户名和密码）、数字证书，或两者结合。

当 {IDF_TARGET_NAME} 以 station 模式连接企业级 AP 时，会发起认证请求。AP 会将该请求转发给配置的 RADIUS 服务器，由其根据所选 EAP 方法和参数对设备进行验证。

.. note::

  {IDF_TARGET_NAME} 仅在 station 模式下支持企业级 Wi-Fi。

企业级 WPA2 和企业级 WPA3
++++++++++++++++++++++++++++++

{IDF_TARGET_NAME} 支持 **企业级 WPA2 (WPA2-Enterprise)** 和 **企业级 WPA3 (WPA3-Enterprise)**。

**企业级 WPA2** 使用 802.1X/EAP 进行身份验证，并依赖安全的凭证或证书。为建立安全连接，AP 与 station 会协商并选定适当的加密套件。{IDF_TARGET_NAME} 支持以下内容：

- 802.1X/EAP（WPA）AKM 方法
- AES-CCM cipher suite
- 启用 `USE_MBEDTLS_CRYPTO` 后可使用 mbedtls 支持的其他 cipher suite

**企业级 WPA3** 在企业级 WPA2 的基础上加强了安全性，并对所有 WPA3 连接增加两项强制要求：必须启用受保护管理帧 (PMF)，并强制执行服务器证书验证。

此外，企业级 WPA3 还支持 **192 位最低强度安全模式 (Suite B)**，提供更高的加密强度。

- 企业级 WPA3 的 192 位模式仅适用于支持 :c:macro:`SOC_WIFI_GCMP_SUPPORT` 的芯片。
- 若需启用该模式，请配置 :menuitem:`CONFIG_ESP_WIFI_SUITE_B_192`。

支持的 EAP 方法
+++++++++++++++++++++

{IDF_TARGET_NAME} 支持多种企业级认证的 EAP 方法。不同方法可能需要不同的配置参数，如 SSID、身份标识、用户名/密码、CA 证书或客户端证书等。

支持以下 EAP 方法：

- **EAP-TLS**

  基于证书的认证方式，需要提供 SSID 和 EAP 身份；通过客户端证书进行设备认证。

- **PEAP**

  一种受保护的 EAP 方法，需要提供用户名和密码。

- **EAP-TTLS**

  一种基于凭证的认证方式。服务器认证为必选；用户认证取决于 Phase 2 方法。通常需要提供用户名和密码。支持的 Phase 2 方法包括：

  - PAP：密码认证协议
  - CHAP：挑战握手认证协议
  - MSCHAP
  - MSCHAP-V2

- **EAP-FAST**

  基于受保护访问凭证 (PAC) 的认证方法，需要身份标识和密码。使用 EAP-FAST 时需要 **禁用** :menuitem:`CONFIG_ESP_WIFI_MBEDTLS_TLS_CLIENT`。

示例
++++++++

- :example:`wifi/wifi_eap_fast`

  演示如何使用 EAP-FAST 将 {IDF_TARGET_NAME} 连接到企业级 Wi-Fi，包括 CA 证书安装、凭证配置、启用企业级模式以及连接 AP。

- :example:`wifi/wifi_enterprise`

  演示使用 EAP-TLS、EAP-PEAP 和 EAP-TTLS 连接企业级 Wi-Fi。有关使用 OpenSSL 生成证书及运行示例的说明，请参考 :example_file:`wifi/wifi_enterprise/README.md`。


个人级 WPA3
-------------

简介
++++++++++++

第三代 Wi-Fi 访问保护 (WPA3) 是一组强化的 Wi-Fi 接入安全性标准，旨在取代当前的 WPA2 标准。WPA3 包含了新的功能和属性，可以提供更显著的保护效果，应对不同类型的攻击。相比 WPA2-Personal，WPA 3-Personal 有以下改进：

  - WPA3 使用对等实体同时验证 (SAE) 技术，这是一种基于 Diffie-Hellman 密钥交换的密码验证密钥协商方法。与 WPA2 不同，SAE 技术能够抵抗离线字典攻击，即攻击者在无需进一步网络交互的情况下，尝试通过窃听的四次握手确定共享密钥。
  - 禁用过时协议，如 TKIP 协议，该协议容易受到如 MIC 密钥恢复攻击等简单攻击。
  - 强制使用受保护的管理帧 (PMF) 保护单播和组播鲁棒管理帧，包括 Disassoc 和 Deauth 帧。这意味着攻击者无法通过向 AP 发送伪造的 Assoc 帧或向 station 发送 Deauth/Disassoc 帧来中断已建立的 WPA3 会话。
  - 提供前向保密功能，即使攻击者在数据传输后成功破解密码，也无法解密捕获的数据。

{IDF_TARGET_NAME} 的 station 模式还支持以下额外的 Wi-Fi CERTIFIED WPA3™ 功能：

 - **禁用过渡**：WPA3 为客户端定义了过渡模式。该模式下，即使网络中的某些 AP 不支持最强的安全模式，客户端也能正常接入网络。客户端通常会默认将网络配置文件配置为过渡模式。然而，这类客户端可能会遭受主动降级攻击，即攻击者会引导客户端使用强度较低的安全模式，以利用该模式的漏洞。为减轻这种攻击的影响，WPA3 引入了禁用过渡功能。当连接到某个网络时，若该网络已全面支持更高级的安全模式，则支持客户端从过渡模式切换到“纯粹”模式。请在 :cpp:type:`wifi_sta_config_t` 中启用 :cpp:type:`transition_disable`，为 {IDF_TARGET_NAME} 的 station 模式启用此功能。

 - **SAE 公钥 (PK)**：由于小型公共网络中的密码与多个用户共享，攻击者可能更容易破解密码，发动伪装双子攻击。为阻止这类攻击，个人级 WPA3 引入了 SAE-PK 扩展认证机制。SAE-PK 认证交换与常规 SAE 交换非常相似，唯一区别在于 AP 会向客户端发送数字签名。客户端基于密码指纹验证 AP 断言的公钥，并使用公钥验证签名。因此，即使攻击者知道密码，也不知道生成有效签名所需的私钥，避免客户端遭受伪装双子攻击。请启用 :menuitem:`CONFIG_ESP_WIFI_ENABLE_SAE_PK`，并在 :cpp:type:`wifi_sta_config_t` 中使用 :cpp:type:`sae_pk_mode` 进行相应配置，为 {IDF_TARGET_NAME} 的 station 模式添加 SAE PK 支持。

 - **SAE PWE Methods**: {IDF_TARGET_NAME} 的 station 和 SoftAP 模式都支持 SAE 密码元素的推导方法 `Hunting And Pecking` 和 `Hash to Element (H2E)`。H2E 所需迭代次数较少，因此计算效率更高，还可以减轻侧通道攻击的风险。这些方法可以分别在 station 模式和 SoftAP 中通过参数 :cpp:type:`sae_pwe_h2e` 配置，其中 :cpp:type:`wifi_sta_config_t` 用于 station 模式，:cpp:type:`wifi_ap_config_t` 用于 SoftAP 模式。请使用配置选项 :cpp:enumerator:`WPA3_SAE_PWE_BOTH` 启用 `Hunting And Pecking` 和 `H2E` 方法。

请参阅 Wi-Fi 联盟官网的 `安全性 <https://www.wi-fi.org/discover-wi-fi/security>`_ 部分，了解更多详情。

在 {IDF_TARGET_NAME} 上设置 WPA3
++++++++++++++++++++++++++++++++++++++

配置选项 :menuitem:`CONFIG_ESP_WIFI_ENABLE_WPA3_SAE` 用于在 station 上启用或禁用 WPA3，该选项默认启用，如果禁用，则 {IDF_TARGET_NAME} 无法建立 WPA3 连接。Wi-Fi 组件中还提供了配置选项 :menuitem:`CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT`，用于在 SoftAP 上启用或禁用 WPA3。此外，由于 WPA3 协议强制使用 PMF，因此，station 和 SoftAP 的 PMF 模式均默认为可选 PMF。可以使用 Wi-Fi 配置选项将 PMF 模式配置为强制 PMF。WPA3 SoftAP 仅支持使用强制 PMF 模式，如无特别指定，则将在 NVS 中隐式配置并存储该模式。

要了解如何设置 PMF 模式，请参阅 `受保护的管理帧 (PMF)`_。

在为 WPA3-Personal station 配置完所需设置后，应用程序开发人员无需再对 AP 的底层安全模式进行操作。从安全性来看，WPA3-Personal 是目前支持的最高级别协议，因此只要该协议可用，设备就会自动选择将该协议用于连接。例如，如果将 AP 配置为 WPA3 过渡模式，即同时支持 WPA2 和 WPA3，station 将根据以上设置连接 WPA3。

在为 WPA3-Personal 的 SoftAP 模式配置完所需设置后，应用程序开发人员必须在 :cpp:type:`wifi_ap_config_t` 中的 `authmode` 设置 ``WIFI_AUTH_WPA3_PSK``，启动以 WPA3 安全模式运行的 AP。也可将 SoftAP 配置为使用 ``WIFI_AUTH_WPA2_WPA3_PSK`` 的混合模式。

注意，启用 :menuitem:`CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT` 时，二进制文件大小将增加约 6.5 千字节。

增强型开放式™ Wi-Fi 安全协议
------------------------------

简介
++++++++++++

增强型开放式™ Wi-Fi 安全协议为连接到开放（公共）Wi-Fi 的用户提供更高的安全性和隐私保护，尤其适用于无需验证用户身份或无法分发凭证的场景。在此协议下，每位用户都可以获得独特的个人加密密钥，保障用户设备与 Wi-Fi 网络间的数据交换安全。同时，受保护的管理帧还进一步增强了接入点与用户设备之间管理流量的安全性。增强型开放式™ Wi-Fi 安全协议基于机会性无线加密 (OWE) 实现，OWE 过渡模式可以从开放的未加密 WLAN 完美过渡到 OWE WLAN，不会对终端用户体验造成不良影响。

.. note::

  {IDF_TARGET_NAME} 仅在 station 模式下支持增强型开放式™ Wi-Fi 安全协议。


在 {IDF_TARGET_NAME} 上设置 OWE
++++++++++++++++++++++++++++++++++++++

station 模式：

配置选项 :menuitem:`CONFIG_ESP_WIFI_ENABLE_WPA3_OWE_STA` 和 :cpp:type:`wifi_sta_config_t` 中的配置参数 :cpp:type:`owe_enabled` 可以为 station 模式启用 OWE 支持。除上述配置外，请将 :cpp:type:`wifi_scan_threshold_t` 中的 `authmode` 设置为 ``WIFI_AUTH_OPEN`` 以使用 OWE 过渡模式。


SoftAP 模式：

需通过 menuconfig 启用配置选项 :menuitem:`CONFIG_ESP_WIFI_ENABLE_WPA3_OWE_SOFTAP`，并将 :cpp:type:`wifi_ap_config_t` 中的配置参数 `authmode` 设置为 ``WIFI_AUTH_OWE``。SoftAP 不支持 OWE 过渡模式，请仅配置 ``WIFI_AUTH_OWE``。

Wi-Fi 隐私增强
--------------------------

MAC 地址用于设备连接 Wi-Fi 网络。由于 MAC 地址具有唯一且静态的特点，并且在传输时未加密，因此可能会被捕获和追踪。{IDF_TARGET_NAME} 支持 Wi-Fi 隐私增强功能，包括 MAC 地址随机化、序列号随机化、GAS 对话令牌多样化和厂商自定义序列号管理。这些功能通过增强隐私保护，避免设备在扫描或连接网络时被持续追踪。

要使用此功能，请在 menuconfig 中启用配置选项 :menuitem:`CONFIG_ESP_WIFI_PRIVACY_ENHANCEMENTS_ENABLED`。

{IDF_TARGET_NAME} 还会在未连接网络时，定期自动重置随机 MAC 地址。重置周期通过 menuconfig 中的 :menuitem:`CONFIG_ESP_WIFI_RMAC_AUTO_RESET_INTERVAL` 选项配置（有效范围：1 至 24 小时，默认值为 12 小时）。

.. note::

   仅当 station 未连接到任何 AP 时，:menuitem:`CONFIG_ESP_WIFI_RMAC_AUTO_RESET_INTERVAL` 才会生成并设置新的随机 MAC 地址。如果 station 已连接到 AP，则不会中断连接，并会继续使用相同的随机 MAC 地址。如果定期自动重置定时器在 station 处于连接状态时超时，定时器将在下一次断开连接时被重新装载或触发。

   在启用 Wi-Fi Mesh 或 ESP-NOW 时，不支持且无法使用 Wi-Fi 隐私增强功能。


{IDF_TARGET_NAME} 在满足以下条件时支持扫描过程中的隐私增强：

  - 在 menuconfig 中启用配置选项 :menuitem:`CONFIG_ESP_WIFI_PRIVACY_ENHANCEMENTS_ENABLED`
  - 扫描类型为 :cpp:enumerator:`WIFI_SCAN_TYPE_ACTIVE`
  - station 未连接到任何 AP

{IDF_TARGET_NAME} 在满足以下条件时支持连接过程中的隐私增强：

  - 在 menuconfig 中启用配置选项 :menuitem:`CONFIG_ESP_WIFI_PRIVACY_ENHANCEMENTS_ENABLED`
  - 使用 :cpp:func:`esp_wifi_set_config` 设置新的 Wi-Fi 配置


要获取 MAC 地址，请使用 API :cpp:func:`esp_wifi_get_mac`。


.. only:: SOC_WIFI_NAN_SUPPORT

    NAN（Wi-Fi Aware\ :sup:`TM`）安全
    -------------------------------------

    简介
    ++++

    Wi-Fi Aware\ :sup:`TM` （NAN，Neighbor Awareness Networking，邻居感知网络）让设备无需 AP 即可发现服务并建立 NAN 数据路径 (NDP)。默认情况下，NDP 以明文传输数据，与开放式 Wi-Fi 网络类似。NAN 安全在 NDP 握手期间为发布方与订阅方派生出成对密钥 (ND-PMK)，并加密随后的单播数据帧，从而保护 NDP。

    {IDF_TARGET_NAME} 支持两种 NAN 安全机制。两者都会建立加密的 NDP，但密钥材料的配置方式不同：

    - **基于密码短语/PMK (NCS-SK-128)** —— 双方通过带外方式共享密码短语（或原始的 32 字节 PMK）。协议栈据此派生 ND-PMK。这相当于 WPA2/WPA3-Personal 的预共享密钥，只是用于 NAN。
    - **基于配对 (NCS-PK-PASN-128)** —— 双方事先不共享密钥，而是运行基于 PASN 的 NAN 配对协议来协商 NPK/NIK，协商时可以选择用引导方法（例如 PIN 码）进行认证。缓存的 NPK/NIK 可供后续 NDP 复用。

    基于密码短语/PMK 的安全 (NCS-SK-128)
    ++++++++++++++++++++++++++++++++++++++++++++

    这是最简单的 NAN 安全模式，可在独立 ESP-IDF 中使用（无需外部组件）。双方在相互匹配的发布/订阅服务上配置同一密钥，可以是密码短语，也可以是原始的 32 字节 PMK。协议栈为每条凭据派生 ND-PMK 和 ND-PMKID，并在空口通告 SCID 列表；订阅方将发布方的 SCID 列表与本地已配置的凭据进行匹配。

    安全按服务配置，相关字段位于 :cpp:type:`wifi_nan_publish_cfg_t`/:cpp:type:`wifi_nan_subscribe_cfg_t`：

    - 将 ``security_reqd`` 设为 ``1``，将该服务标记为需要安全。
    - 将 ``security_cfg`` 指向 :cpp:type:`wifi_nan_discovery_security_params_t`，并在其中填入一条或多条 :cpp:type:`wifi_nan_credential_t` （密码短语或 PMK，每条凭据通过 ``csid`` 绑定一个密码套件 ID）。
    - 也可启用 ``group_data_prot`` （ND-GTK 组地址数据保护）。

    单个服务最多可附加 :c:macro:`ESP_WIFI_NAN_MAX_CREDS_PER_SVC` 条凭据，因此同一服务可以使用不同的密码短语或 PMK，与多个对端共享。双方必须在对应服务上至少配置一条相互匹配的凭据，否则 NDP 协商会失败。

    .. warning::

      组管理帧保护 (BIP, IGTKSA/BIGTKSA) 是 **设备全局** 设置，而不是按服务设置。调用 :cpp:func:`esp_wifi_nan_sync_start` 启动 NAN 时，必须通过 :cpp:type:`wifi_nan_sync_config_t` 的 ``group_mgmt_prot`` 启用组管理帧保护。IGTK/BIGTK 的安装、CSIA 能力信息的构造以及 NDP 协商都取决于这一设备全局值。因此，如果应用程序只设置 :cpp:type:`wifi_nan_discovery_security_params_t` 中按服务的 ``group_mgmt_prot`` 位，设备会在未启用组管理帧保护的情况下继续运行，且不会提示错误。按服务的 ``group_mgmt_prot`` 位是保留位，本身不起作用。

    .. note::

      目前仅支持 ``WIFI_NAN_CSID_NCS_SK_128``。NAN 安全需要配置选项 :ref:`CONFIG_ESP_WIFI_NAN_SECURITY` 和 :ref:`CONFIG_ESP_WIFI_MBEDTLS_CRYPTO`。

    基于配对的安全 (NCS-PK-PASN-128)
    +++++++++++++++++++++++++++++++++++++++++

    NAN 配对让两台设备在没有预共享密钥的情况下，借助 PIN 码等引导方法建立长期信任关系 (NPK/NIK)。协商得到的 NPK/NIK 可以缓存（并持久保存到 NVS），这样后续 NDP 无需重复配对握手即可受到保护。

    配对按服务配置：:cpp:type:`wifi_nan_publish_cfg_t`/:cpp:type:`wifi_nan_subscribe_cfg_t` 的 ``pairing`` 字段指向 :cpp:type:`wifi_nan_pairing_cfg_t`，其中包括是否启用配对建立、是否启用配对验证、NPK/NIK 缓存、引导方法位图以及 comeback 延时。

    .. note::

      NAN 配对 (``WIFI_NAN_CSID_NCS_PK_PASN_128``) 需要配置选项 :ref:`CONFIG_ESP_WIFI_NAN_PAIRING`，以及 **Wi-Fi Aware** 外部组件 (``espressif/wifi_aware``)；它 **不能** 在独立 ESP-IDF 中使用。关于如何将该组件添加到工程中，请参阅 :doc:`/api-guides/wifi-driver/wifi-modes`。

    应用示例
    ++++++++

    - :example:`wifi/wifi_aware/nan_console` 通过 ``publish`` 和 ``subscribe`` 控制台命令配置 NAN 安全。指定 ``-p``/``--passphrase`` 或 ``-k``/``--pmk`` 可使该服务启用安全；开放服务无需安全参数。完整命令说明请参阅该示例的 README。
