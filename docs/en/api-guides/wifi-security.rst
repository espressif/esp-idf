Wi-Fi Security
==============

:link_to_translation:`zh_CN:[中文]`

{IDF_TARGET_NAME} Wi-Fi Security Features
-----------------------------------------

- Support for Protected Management Frames (PMF)
- Support for WPA3-Personal
- Support for Opportunistic Wireless Encryption (OWE)

In addition to traditional security methods (WEP/WPA-TKIP/WPA2-CCMP), {IDF_TARGET_NAME} Wi-Fi supports state-of-the-art security protocols, namely Protected Management Frames (PMF), Wi-Fi Protected Access 3 and Enhanced Open™ based on Opportunistic Wireless Encryption. WPA3 provides better privacy and robustness against known attacks on traditional modes. Enhanced Open™ enhances the security and privacy of users connecting to open (public) Wireless Networks without authentication.

Protected Management Frames (PMF)
---------------------------------

Introduction
++++++++++++

In Wi-Fi, management frames such as beacons, probes, authentication/deauthentication, and association/disassociation are used by non-AP stations to scan and connect to an AP. Unlike data frames, these frames are sent unencrypted.

An attacker can use eavesdropping and packet injection to send spoofed authentication/deauthentication or association/disassociation frames at the right time, leading to attacks such as Denial-of-Service (DOS) and man-in-the-middle.

PMF provides protection against these attacks by encrypting unicast management frames and providing integrity checks for broadcast management frames. These include deauthentication, disassociation, and robust management frames. It also provides a Secure Association (SA) teardown mechanism to prevent spoofed association/authentication frames from disconnecting already connected clients.

There are three types of PMF configuration modes on both the station and AP sides:

 - PMF Optional
 - PMF Required
 - PMF Disabled

API & Usage
+++++++++++

{IDF_TARGET_NAME} supports PMF in both the station and SoftAP mode. For both, the default mode is PMF Optional. For even higher security, PMF Required mode can be enabled by setting the ``required`` flag in ``pmf_cfg`` while using the :cpp:func:`esp_wifi_set_config` API. This results in the device only connecting to a PMF-enabled device and rejecting others. PMF Optional can be disabled using :cpp:func:`esp_wifi_disable_pmf_config` API. If SoftAP is started in WPA3 or WPA2/WPA3 mixed mode, trying to disable PMF results in an error.

.. attention::

    ``capable`` flag in ``pmf_cfg`` is deprecated and set to ``true`` internally. This is to take the additional security benefit of PMF whenever possible.

Wi-Fi Enterprise
---------------------------------

Introduction
++++++++++++++

Wi-Fi Enterprise provides secure authentication mechanisms for enterprise wireless networks. It is based on the IEEE 802.1X standard and requires a RADIUS server to authenticate network users before they connect to an Access Point (AP). Depending on the Extended Authentication Protocol (EAP) method used, authentication can rely on user credentials (username and password), digital certificates, or both.

When {IDF_TARGET_NAME}, operating in station mode, connects to an Enterprise AP, it initiates an authentication request. The AP forwards this request to the configured RADIUS server, which validates the station based on the selected EAP method and configured parameters.

.. note::

  {IDF_TARGET_NAME} supports Wi-Fi Enterprise only in station mode.

WPA2-Enterprise and WPA3-Enterprise
++++++++++++++++++++++++++++++++++++++

{IDF_TARGET_NAME} supports both **WPA2-Enterprise** and **WPA3-Enterprise**.

**WPA2-Enterprise** provides authentication using 802.1X/EAP and relies on secure credentials or certificates. For secure connectivity, the AP and station negotiate and agree on an appropriate cipher suite. {IDF_TARGET_NAME} supports the following:

- 802.1X/EAP (WPA) AKM method
- AES-CCM cipher suite
- Additional cipher suites supported by mbedtls when the `USE_MBEDTLS_CRYPTO` flag is enabled

**WPA3-Enterprise** enhances WPA2-Enterprise by requiring Protected Management Frames (PMF) and mandatory server certificate validation on all WPA3 connections.

WPA3-Enterprise also introduces an additional secure mode using **192-bit minimum-strength security protocols** (“Suite B”). This mode ensures a consistent baseline of cryptographic strength.

- WPA3-Enterprise 192-bit mode is supported only on modules that have :c:macro:`SOC_WIFI_GCMP_SUPPORT`.
- To enable WPA3-Enterprise 192-bit mode, configure :ref:`CONFIG_ESP_WIFI_SUITE_B_192`.

Supported EAP Methods
+++++++++++++++++++++++++

{IDF_TARGET_NAME} supports multiple EAP methods for Enterprise authentication. These methods may require different combinations of SSID, identity, username/password, CA certificate, or client certificate.

The following EAP methods are supported:

- **EAP-TLS**

  Certificate-based method requiring SSID and EAP identity. Client certificates are used for authentication.

- **PEAP**

  A Protected EAP method requiring a username and password.

- **EAP-TTLS**

  A credential-based method. Server authentication is mandatory; user authentication is optional depending on the Phase 2 method. Username and password are typically required. Supported Phase 2 methods include:

  - PAP: Password Authentication Protocol
  - CHAP: Challenge Handshake Authentication Protocol
  - MSCHAP
  - MSCHAP-V2

- **EAP-FAST**

  An authentication method based on Protected Access Credentials (PAC), requiring an identity and password. To use EAP-FAST, :ref:`CONFIG_ESP_WIFI_MBEDTLS_TLS_CLIENT` must be **disabled**.

Examples
++++++++++

- :example:`wifi/wifi_eap_fast`

  Demonstrates connecting {IDF_TARGET_NAME} to an AP using EAP-FAST, including CA certificate installation, credential configuration, enabling Enterprise mode, and connecting to the AP.

- :example:`wifi/wifi_enterprise`

  Demonstrates connecting {IDF_TARGET_NAME} using EAP-TLS, EAP-PEAP, and EAP-TTLS. For details on generating certificates with OpenSSL and running the example, refer to :example_file:`wifi/wifi_enterprise/README.md`.


WPA3-Personal
-------------

Introduction
++++++++++++

Wi-Fi Protected Access-3 (WPA3) is a set of enhancements to Wi-Fi access security intended to replace the current WPA2 standard. It includes new features and capabilities that offer significantly better protection against different types of attacks. It improves upon WPA2-Personal in the following ways:

  - WPA3 uses Simultaneous Authentication of Equals (SAE), which is a password-authenticated key agreement method based on Diffie-Hellman key exchange. Unlike WPA2, the technology is resistant to offline-dictionary attacks, where the attacker attempts to determine a shared password based on a captured 4-way handshake without any further network interaction.
  - Disallows outdated protocols such as TKIP, which is susceptible to simple attacks like MIC key recovery attacks.
  - Mandates Protected Management Frames (PMF), which provides protection for unicast and multicast robust management frames which include Disassoc and Deauth frames. This means that the attacker cannot disrupt an established WPA3 session by sending forged Assoc frames to the AP or Deauth/Disassoc frames to the station.
  - Provides forward secrecy, which means the captured data cannot be decrypted even if the password is compromised after data transmission.

{IDF_TARGET_NAME} station also supports following additional Wi-Fi CERTIFIED WPA3™ features:

 - **Transition Disable** : WPA3 defines transition modes for client devices so that they can connect to a network even when some of the APs in that network do not support the strongest security mode. Client device implementations typically configure network profiles in a transition mode by default. However, such a client device could be subject to an active downgrade attack in which the attacker causes the client device to use a lower security mode in order to exploit a vulnerability with that mode. WPA3 has introduced the Transition Disable feature to mitigate such attacks, by enabling client devices to change from a transition mode to an "only" mode when connecting to a network, once that network indicates it fully supports the higher security mode. Enable :cpp:type:`transition_disable` in :cpp:type:`wifi_sta_config_t` to enable this feature for {IDF_TARGET_NAME} station.

 - **SAE PUBLIC-KEY (PK)** : As the password at small public networks is shared with multiple users, it may be relatively easy for an attacker to find out the password, which is sufficient to launch an evil twin attack. Such attacks are prevented by an extension to WPA3-Personal called SAE-PK. The SAE-PK authentication exchange is very similar to the regular SAE exchange, with the addition of a digital signature sent by the AP to the client device. The client device validates the public key asserted by the AP based on the password fingerprint and verifies the signature using the public key. So even if the attacker knows the password, it does not know the private key to generate a valid signature, and therefore the client device is protected against an evil twin attack. Enable :ref:`CONFIG_ESP_WIFI_ENABLE_SAE_PK` and :cpp:type:`sae_pk_mode` in :cpp:type:`wifi_sta_config_t` to add support of SAE PK for {IDF_TARGET_NAME} station.

 - **SAE PWE Methods**: {IDF_TARGET_NAME} station as well as SoftAP supports SAE Password Element derivation method `Hunting And Pecking` and `Hash to Element (H2E)`. H2E is computationally efficient as it uses fewer iterations than Hunt and Peck, and also it mitigates side-channel attacks. These can be configured using the parameter :cpp:type:`sae_pwe_h2e` from :cpp:type:`wifi_sta_config_t` and :cpp:type:`wifi_ap_config_t` for station and SoftAP respectively. Hunt and peck, H2E both can be enabled by using :cpp:enumerator:`WPA3_SAE_PWE_BOTH` configuration.

Please refer to the `Security <https://www.wi-fi.org/discover-wi-fi/security>`_ section of Wi-Fi Alliance's official website for further details.

Setting up WPA3 Personal with {IDF_TARGET_NAME}
+++++++++++++++++++++++++++++++++++++++++++++++

A configuration option :ref:`CONFIG_ESP_WIFI_ENABLE_WPA3_SAE` is provided to enable/disable WPA3 for the station. By default, it is kept enabled. If disabled, {IDF_TARGET_NAME} will not be able to establish a WPA3 connection. Also under the Wi-Fi component, a configuration option :ref:`CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT` is provided to enable/disable WPA3 for SoftAP. Additionally, since PMF is mandated by WPA3 protocol, PMF Optional is set by default for station and SoftAP. PMF Required can be configured using Wi-Fi configuration. For WPA3 SoftAP, PMF Required is mandatory and will be configured and stored in NVS implicitly if not specified by the user.

Refer to `Protected Management Frames (PMF)`_ on how to set this mode.

After configuring all required settings for the WPA3-Personal station, application developers need not worry about the underlying security mode of the AP. WPA3-Personal is now the highest supported protocol in terms of security, so it is automatically selected for the connection whenever available. For example, if an AP is configured to be in WPA3 Transition Mode, where it advertises as both WPA2 and WPA3 capable, the station chooses WPA3 for the connection with the above settings.

After configuring all required setting for WPA3-Personal SoftAP, application developers have to set ``WIFI_AUTH_WPA3_PSK`` for `authmode` in :cpp:type:`wifi_ap_config_t` to start AP in WPA3 security. SoftAP can be also configured to use ``WIFI_AUTH_WPA2_WPA3_PSK`` mixed mode.

Note that binary size will be increased by about 6.5 kilobytes after enabling :ref:`CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT`.

Wi-Fi Enhanced Open™
--------------------

Introduction
++++++++++++

Enhanced Open™ is used for providing security and privacy to users connecting to open (public) wireless networks, particularly in scenarios where user authentication is not desired or distribution of credentials impractical. Each user is provided with unique individual encryption keys that protect data exchange between a user device and the Wi-Fi network. Protected Management Frames further protect management traffic between the access point and the user device. Enhanced Open™ is based on the Opportunistic Wireless Encryption (OWE) standard. OWE Transition Mode enables a seamless transition from Open unencrypted WLANs to OWE WLANs without adversely impacting the end-user experience.

.. note::

  {IDF_TARGET_NAME} supports Wi-Fi Enhanced Open™ only in station mode.


Setting up OWE with {IDF_TARGET_NAME}
++++++++++++++++++++++++++++++++++++++

A configuration option :ref:`CONFIG_ESP_WIFI_ENABLE_WPA3_OWE_STA` and configuration parameter :cpp:type:`owe_enabled` in :cpp:type:`wifi_sta_config_t` is provided to enable OWE support for the station. To use OWE transition mode, along with the configuration provided above, `authmode` from :cpp:type:`wifi_scan_threshold_t` should be set to ``WIFI_AUTH_OPEN``.
