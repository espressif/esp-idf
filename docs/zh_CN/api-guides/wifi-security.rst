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
---------------------------------

简介
++++++++++++

企业级安全是企业无线网的安全认证机制，采用 RADIUS 服务器在设备接入到接入点 (AP) 前认证网络用户。该机制基于 802.1X 标准完成认证，并采用多种扩展认证协议 (EAP) 方法，如 TLS、TTLS、PEAP 和 EAP-FAST。RADIUS 服务器根据用户凭证（用户名和密码）、数字证书或两者的组合进行用户认证。

.. note::

  {IDF_TARGET_NAME} 仅在 station 模式下支持企业级 Wi-Fi。

{IDF_TARGET_NAME} 支持 **企业级 WPA2** 和 **企业级 WPA3**。企业级 WPA3 构建在企业级 WPA2 的基础之上，并额外要求在所有 WPA3 连接中使用受保护的管理帧 (PMF) 和服务器证书验证。此外，**企业级 WPA3 还提供了一种更安全的模式，使用 192 位最低强度的安全协议和加密工具，更好地保护敏感数据**。企业级 WPA3 的 192 位模式可以确保用户正确使用密码工具组合，并在 WPA3 网络中设立了一致的安全基准。需要注意的是，只有支持 :c:macro:`SOC_WIFI_GCMP_SUPPORT` 的模组才支持企业级 WPA3 的 192 位模式。如需使用该模式，请启用 :ref:`CONFIG_ESP_WIFI_SUITE_B_192` 标志。

{IDF_TARGET_NAME} 支持以下 EAP 方法：
  - EAP-TLS：该认证方法基于证书实现，仅需提供 SSID 和 EAP-IDF。
  - PEAP：该认证方法为受保护的 EAP 方法，必须提供用户名和密码。
  - EAP-TTLS：该认证方法基于凭证实现，必须提供服务器证书，用户证书为可选项。该方法支持多种第二阶段方法，如：
     - PAP：密码认证协议
     - CHAP：挑战握手认证协议
     - MSCHAP 和 MSCHAP-V2
  - EAP-FAST：该认证方法基于受保护访问凭证 (PAC) 实现，同时也使用身份和密码。目前，需禁用 :ref:`CONFIG_ESP_WIFI_MBEDTLS_TLS_CLIENT` 标志以使用此功能。

示例 :example:`wifi/wifi_enterprise` 展示了除 EAP-FAST 之外的所有支持的企业级 Wi-Fi 方法。有关 ESP-FAST 的示例，请参阅 :example:`wifi/wifi_eap_fast`。可以在 ``idf.py menuconfig`` 的示例配置菜单中选择 EAP 方法。请参阅 :idf_file:`examples/wifi/wifi_enterprise/README.md` 了解如何生成证书及如何运行示例。

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

 - **SAE 公钥 (PK)**：由于小型公共网络中的密码与多个用户共享，攻击者可能更容易破解密码，发动伪装双子攻击。为阻止这类攻击，个人级 WPA3 引入了 SAE-PK 扩展认证机制。SAE-PK 认证交换与常规 SAE 交换非常相似，唯一区别在于 AP 会向客户端发送数字签名。客户端基于密码指纹验证 AP 断言的公钥，并使用公钥验证签名。因此，即使攻击者知道密码，也不知道生成有效签名所需的私钥，避免客户端遭受伪装双子攻击。请启用 :ref:`CONFIG_ESP_WIFI_ENABLE_SAE_PK`，并在 :cpp:type:`wifi_sta_config_t` 中使用 :cpp:type:`sae_pk_mode` 进行相应配置，为 {IDF_TARGET_NAME} 的 station 模式添加 SAE PK 支持。

 - **SAE PWE Methods**: {IDF_TARGET_NAME} 的 station 和 SoftAP 模式都支持 SAE 密码元素的推导方法 `Hunting And Pecking` 和 `Hash to Element (H2E)`。H2E 所需迭代次数较少，因此计算效率更高，还可以减轻侧通道攻击的风险。这些方法可以分别在 station 模式和 SoftAP 中通过参数 :cpp:type:`sae_pwe_h2e` 配置，其中 :cpp:type:`wifi_sta_config_t` 用于 station 模式，:cpp:type:`wifi_ap_config_t` 用于 SoftAP 模式。请使用配置选项 :cpp:enumerator:`WPA3_SAE_PWE_BOTH` 启用 `Hunting And Pecking` 和 `H2E` 方法。

请参阅 Wi-Fi 联盟官网的 `安全性 <https://www.wi-fi.org/discover-wi-fi/security>`_ 部分，了解更多详情。

在 {IDF_TARGET_NAME} 上设置 WPA3
++++++++++++++++++++++++++++++++++++++

配置选项 :ref:`CONFIG_ESP_WIFI_ENABLE_WPA3_SAE` 用于在 station 上启用或禁用 WPA3，该选项默认启用，如果禁用，则 {IDF_TARGET_NAME} 无法建立 WPA3 连接。Wi-Fi 组件中还提供了配置选项 :ref:`CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT`，用于在 SoftAP 上启用或禁用 WPA3。此外，由于 WPA3 协议强制使用 PMF，因此，station 和 SoftAP 的 PMF 模式均默认为可选 PMF。可以使用 Wi-Fi 配置选项将 PMF 模式配置为强制 PMF。WPA3 SoftAP 仅支持使用强制 PMF 模式，如无特别指定，则将在 NVS 中隐式配置并存储该模式。

要了解如何设置 PMF 模式，请参阅 `受保护的管理帧 (PMF)`_。

在为 WPA3-Personal station 配置完所需设置后，应用程序开发人员无需再对 AP 的底层安全模式进行操作。从安全性来看，WPA3-Personal 是目前支持的最高级别协议，因此只要该协议可用，设备就会自动选择将该协议用于连接。例如，如果将 AP 配置为 WPA3 过渡模式，即同时支持 WPA2 和 WPA3，station 将根据以上设置连接 WPA3。

在为 WPA3-Personal 的 SoftAP 模式配置完所需设置后，应用程序开发人员必须在 :cpp:type:`wifi_ap_config_t` 中的 `authmode` 设置 ``WIFI_AUTH_WPA3_PSK``，启动以 WPA3 安全模式运行的 AP。也可将 SoftAP 配置为使用 ``WIFI_AUTH_WPA2_WPA3_PSK`` 的混合模式。

注意，启用 :ref:`CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT` 时，二进制文件大小将增加约 6.5 千字节。

增强型开放式™ Wi-Fi 安全协议
------------------------------

简介
++++++++++++

增强型开放式™ Wi-Fi 安全协议为连接到开放（公共）Wi-Fi 的用户提供更高的安全性和隐私保护，尤其适用于无需验证用户身份或无法分发凭证的场景。在此协议下，每位用户都可以获得独特的个人加密密钥，保障用户设备与 Wi-Fi 网络间的数据交换安全。同时，受保护的管理帧还进一步增强了接入点与用户设备之间管理流量的安全性。增强型开放式™ Wi-Fi 安全协议基于机会性无线加密 (OWE) 实现，OWE 过渡模式可以从开放的未加密 WLAN 完美过渡到 OWE WLAN，不会对终端用户体验造成不良影响。

.. note::

  {IDF_TARGET_NAME} 仅在 station 模式下支持增强型开放式™ Wi-Fi 安全协议。


在 {IDF_TARGET_NAME} 上设置 OWE
++++++++++++++++++++++++++++++++++++++

配置选项 :ref:`CONFIG_ESP_WIFI_ENABLE_WPA3_OWE_STA` 和 :cpp:type:`wifi_sta_config_t` 中的配置参数 :cpp:type:`owe_enabled` 可以为 station 模式启用 OWE 支持。除上述配置外，请将 :cpp:type:`wifi_scan_threshold_t` 中的 `authmode` 设置为 ``WIFI_AUTH_OPEN`` 以使用 OWE 过渡模式。
