Wi-Fi Security
==============

{IDF_TARGET_NAME} Wi-Fi Security Features
-----------------------------------------
- Support for Protected Management Frames (PMF)
- Support for WPA3-Personal

In addition to traditional security methods (WEP/WPA-TKIP/WPA2-CCMP), {IDF_TARGET_NAME} Wi-Fi supports state-of-the-art security protocols, namely Protected Management Frames based on 802.11w standard and Wi-Fi Protected Access 3 (WPA3-Personal). Together, PMF and WPA3 provide better privacy and robustness against known attacks on traditional modes.

Protected Management Frames (PMF)
---------------------------------

Introduction
++++++++++++

In Wi-Fi, management frames such as beacons, probes, (de)authentication, (dis)association are used by non-AP stations to scan and connect to an AP. Unlike data frames, these frames are sent unencrypted.
An attacker can use eavesdropping and packet injection to send spoofed (de)authentication/(dis)association frames at the right time, leading to the following attacks in case of unprotected management frame exchanges.

 - DOS attack on one or all clients in the range of the attacker.
 - Tearing down existing association on AP side by sending association request.
 - Forcing a client to perform 4-way handshake again in case PSK is compromised in order to get PTK.
 - Getting SSID of hidden network from association request.
 - Launching man-in-the-middle attack by forcing clients to deauth from legitimate AP and associating to a rogue one.

PMF provides protection against these attacks by encrypting unicast management frames and providing integrity checks for broadcast management frames. These include deauthentication, disassociation and robust management frames. It also provides Secure Association (SA) teardown mechanism to prevent spoofed association/authentication frames from disconnecting already connected clients.

There are 3 types of PMF configuration modes on both Station and AP side -
 - PMF Optional
 - PMF Required
 - PMF Disabled

Depending on the PMF configuration on Station and AP side, the resulting connection will behave differently. Below table summarises all possible outcomes.

+--------------+------------------------+---------------------------+
| STA Setting  | AP Setting             |  Outcome                  |
+==============+========================+===========================+
| PMF Optional |  PMF Optional/Required | Mgmt Frames Protected     |
+--------------+------------------------+---------------------------+
| PMF Optional |  PMF Disabled          | Mgmt Frames Not Protected |
+--------------+------------------------+---------------------------+
| PMF Required |  PMF Optional/Required | Mgmt Frames Protected     |
+--------------+------------------------+---------------------------+
| PMF Required |  PMF Disabled          | STA refuses Connection    |
+--------------+------------------------+---------------------------+
| PMF Disabled |  PMF Optional/Disabled | Mgmt Frames Not Protected |
+--------------+------------------------+---------------------------+
| PMF Disabled |  PMF Required          | AP refuses Connection     |
+--------------+------------------------+---------------------------+

API & Usage
+++++++++++

{IDF_TARGET_NAME} supports PMF only in Station mode. Station defaults to PMF Optional mode and disabling PMF is not possible. For even higher security, PMF required mode can be enabled by setting the ``required`` flag in `pmf_cfg` while using the :cpp:func:`esp_wifi_set_config` API. This will result in Station only connecting to a PMF enabled AP and rejecting all other AP's. An example of this configuration is given below.

   .. code-block:: c

    wifi_config_t wifi_config = {
        .sta = {
             .ssid = EXAMPLE_WIFI_SSID,
             .password = EXAMPLE_WIFI_PASSWORD,
             .pmf_cfg = {
                .required = true
             }
        }
    };

.. attention::

    ``capable`` flag in `pmf_cfg` is deprecated and set to true internally. This is to take the additional security benefit of PMF whenever possible.


WPA3-Personal
-------------

Introduction
++++++++++++

Wi-Fi Protected Access-3 (WPA3) is a set of enhancements to Wi-Fi access security intended to replace the current WPA2 standard. It includes new features and capabilities that offer significantly better protection against different types of attacks. It improves upon WPA2-Personal in following ways:

  - WPA3 uses Simultaneous Authentication of Equals (SAE), which is password-authenticated key agreement method based on Diffie-Hellman key exchange. Unlike WPA2, the technology is resistant to offline-dictionary attack, where the attacker attempts to determine shared password based on captured 4-way handshake without any further network interaction.
  - Disallows outdated protocols such as TKIP, which is susceptible to simple attacks like MIC key recovery attack.
  - Mandates Protected Management Frames (PMF), which provides protection for unicast and multicast robust management frames which include Disassoc and Deauth frames. This means that the attacker cannot disrupt an established WPA3 session by sending forged Assoc frames to the AP or Deauth/Disassoc frames to the Station.
  - Provides forward secrecy, which means the captured data cannot be decrypted even if password is compromised after data transmission.

Please refer to `Security <https://www.wi-fi.org/discover-wi-fi/security>`_ section of Wi-Fi Alliance's official website for further details.

Setting up WPA3 with {IDF_TARGET_NAME}
++++++++++++++++++++++++++++++++++++++

In IDF Menuconfig under Wi-Fi component, a config option "Enable WPA3-Personal" is provided to Enable/Disable WPA3. By default it is kept enabled, if disabled {IDF_TARGET_NAME} will not be able to establish a WPA3 connection. Currently, WPA3 is supported only in the Station mode. Additionally, since PMF is mandated by WPA3 protocol, PMF Mode should be set to either Optional or Required while setting WiFi config.

Refer to `Protected Management Frames (PMF)`_ on how to set this mode.

After these settings are done, Station is ready to use WPA3-Personal. Application developers need not worry about the underlying security mode of the AP. WPA3-Personal is now the highest supported protocol in terms of security, so it will be automatically selected for the connection whenever available. For example, if an AP is configured to be in WPA3 Transition Mode, where it will advertise as both WPA2 and WPA3 capable, Station will choose WPA3 for the connection with above settings.
Note that Wi-Fi stack size requirement will increase 3kB when WPA3 is used.
