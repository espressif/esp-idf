Wi-Fi Security
==============

{IDF_TARGET_NAME} Wi-Fi Security Features
-----------------------------------------
- Support for Protected Management Frames (PMF)
- Support for WPA3-Personal

In addition to traditional security methods (WEP/WPA-TKIP/WPA2-CCMP), {IDF_TARGET_NAME} Wi-Fi supports state-of-the-art security protocols, namely Protected Management, Wi-Fi Protected Access 3. WPA3 provides better privacy and robustness against known attacks on traditional modes.

Protected Management Frames (PMF)
---------------------------------

Introduction
++++++++++++

In Wi-Fi, management frames such as beacons, probes, (de)authentication, (dis)association are used by non-AP stations to scan and connect to an AP. Unlike data frames, these frames are sent unencrypted.
An attacker can use eavesdropping and packet injection to send spoofed (de)authentication/(dis)association frames at the right time, leading to attacks such as Denial-of-Service (DOS) and man-in-the-middle

PMF provides protection against these attacks by encrypting unicast management frames and providing integrity checks for broadcast management frames. These include deauthentication, disassociation and robust management frames. It also provides Secure Association (SA) teardown mechanism to prevent spoofed association/authentication frames from disconnecting already connected clients.

There are 3 types of PMF configuration modes on Station side -
 - PMF Optional
 - PMF Required
 - PMF Disabled

API & Usage
+++++++++++

{IDF_TARGET_NAME} supports PMF only in Station mode. Station defaults to PMF Optional mode and disabling PMF is not possible. For even higher security, PMF required mode can be enabled by setting the ``required`` flag in `pmf_cfg` while using the :cpp:func:`esp_wifi_set_config` API. This will result in Station only connecting to a PMF enabled AP and rejecting all other AP's.

.. attention::

    ``capable`` flag in `pmf_cfg` is deprecated and set to true internally. This is to take the additional security benefit of PMF whenever possible.

WiFi Enterprise
---------------------------------

Introduction
++++++++++++

Enterprise security is the secure authentication mechanism for enterprise wireless networks. It uses RADIUS server for authentication of network users before connecting to the Access Point. The authentication process is based on 802.1X policy and comes with different Extended Authentication Protocol (EAP) methods such as TLS, TTLS, PEAP and EAP-FAST. RADIUS server authenticates the users based on their credentials (username and password), digital certificates or both.

**{IDF_TARGET_NAME} supports WiFi Enterprise only in station mode.**

{IDF_TARGET_NAME} Supports **WPA2-Enterprise** and **WPA3-Enterprise**. WPA3-Enterprise builds upon the foundation of WPA2-Enterprise with the additional requirement of using Protected Management Frames (PMF) and server certificate validation on all WPA3 connections. **WPA3-Enterprise also offers an addition secure mode using 192-bit minimum-strength security protocols and cryptographic tools to better protect sensitive data.** The 192-bit security mode offered by WPA3-Enterprise ensures the right combination of cryptographic tools are used and sets a consistent baseline of security within a WPA3 network. WPA3-Enterprise 192-bit mode is only supported by modules having :ref:`CONFIG_ESP_WIFI_GMAC_SUPPORT` :ref:`CONFIG_ESP_WIFI_GMAC_SUPPORT` support. Enable :ref:`CONFIG_WPA_SUITE_B_192` flag to support WPA3-Enterprise with 192-bit mode.

{IDF_TARGET_NAME} supports the following EAP methods:
  - EAP-TLS: This is a certificate-based method and only requires SSID and EAP-IDF.
  - PEAP: This is a Protected EAP method. Username and password are mandatory.
  - EAP-TTLS: This is a credential-based method. Only server authentication is mandatory while user authentication is optional. Username and Password are mandatory. It supports different Phase2 methods, such as:
     - PAP: Password Authentication Protocol.
     - CHAP: Challenge Handshake Authentication Protocol.
     - MSCHAP and MSCHAP-V2.
  - EAP-FAST: This is an authentication method based on Protected Access Credentials (PAC) which also uses identity and password.

Example :example:`wifi/wifi_enterprise` demonstrates all the supported WiFi Enterprise methods except EAP-FAST. Please refer :example:`wifi/wifi_eap_fast` for EAP-FAST example. EAP method can be selected from the Example Configuration menu in ``idf.py menuconfig``. Refer to :idf_file:`examples/wifi/wifi_enterprise/README.md` for information on how to generate certificates and run the example.

WPA3-Personal
-------------

Introduction
++++++++++++

Wi-Fi Protected Access-3 (WPA3) is a set of enhancements to Wi-Fi access security intended to replace the current WPA2 standard. It includes new features and capabilities that offer significantly better protection against different types of attacks. It improves upon WPA2-Personal in following ways:

  - WPA3 uses Simultaneous Authentication of Equals (SAE), which is password-authenticated key agreement method based on Diffie-Hellman key exchange. Unlike WPA2, the technology is resistant to offline-dictionary attack, where the attacker attempts to determine shared password based on captured 4-way handshake without any further network interaction.
  - Disallows outdated protocols such as TKIP, which is susceptible to simple attacks like MIC key recovery attack.
  - Mandates Protected Management Frames (PMF), which provides protection for unicast and multicast robust management frames which include Disassoc and Deauth frames. This means that the attacker cannot disrupt an established WPA3 session by sending forged Assoc frames to the AP or Deauth/Disassoc frames to the Station.
  - Provides forward secrecy, which means the captured data cannot be decrypted even if password is compromised after data transmission.

**{IDF_TARGET_NAME} station also supports following additional Wi-Fi CERTIFIED WPA3™ features.**

 - **Transition Disable** : WPA3 defines transition modes for client devices so that they can connect to a network even when some of the APs in that network do not support the strongest security mode. Client device implementations typically configure network profiles in a transition mode by default. However, such a client device could be subject to an active downgrade attack in which the attacker causes the client device to use a lower security mode in order to exploit a vulnerability with that mode. WPA3 has introduced the Transition Disable feature to mitigate such attacks, by enabling client devices to change from a transition mode to an “only” mode when connecting to a network, once that network indicates it fully supports the higher security mode. Enable :cpp:type:`transition_disable` in :cpp:type:`wifi_sta_config_t` to enable this feature for {IDF_TARGET_NAME} station.

 - **SAE PWE Methods**: {IDF_TARGET_NAME} station supports SAE Password Element derivation method `Hunting And Pecking` and `Hash to Element (H2E)`. H2E is computationally efficient as it uses less iterations than Hunt and Peck, also it mitigates side channel attacks. These can be configured using parameter :cpp:type:`sae_pwe_h2e` from :cpp:type:`wifi_sta_config_t` for station. Hunt and peck, H2E both can be enabled by using :cpp:enumerator:`WPA3_SAE_PWE_BOTH` configuration.

Please refer to `Security <https://www.wi-fi.org/discover-wi-fi/security>`_ section of Wi-Fi Alliance's official website for further details.

Setting up WPA3 Personal with {IDF_TARGET_NAME}
+++++++++++++++++++++++++++++++++++++++++++++++

A config option :ref:`CONFIG_ESP32_WIFI_ENABLE_WPA3_SAE` is provided to Enable/Disable WPA3 for station. By default it is kept enabled, if disabled {IDF_TARGET_NAME} will not be able to establish a WPA3 connection. Additionally, since PMF is mandated by WPA3 protocol, PMF Mode Optional is set by default for station. PMF Required can be configured using WiFi config.

Refer to `Protected Management Frames (PMF)`_ on how to set this mode.

After configuring all required settings for WPA3-Personal station, application developers need not worry about the underlying security mode of the AP. WPA3-Personal is now the highest supported protocol in terms of security, so it will be automatically selected for the connection whenever available. For example, if an AP is configured to be in WPA3 Transition Mode, where it will advertise as both WPA2 and WPA3 capable, Station will choose WPA3 for the connection with above settings.
Note that Wi-Fi stack size requirement will increase 3kB when "Enable WPA3-Personal" is used.
