Unified Provisioning
^^^^^^^^^^^^^^^^^^^^

:link_to_translation:`zh_CN:[中文]`

Overview
>>>>>>>>

The unified provisioning support in the ESP-IDF provides an extensible mechanism to the developers to configure the device with the Wi-Fi credentials and/or other custom configuration using various transports and different security schemes. Depending on the use case, it provides a complete and ready solution for Wi-Fi network provisioning along with example iOS and Android applications. The developers can choose to extend the device-side and phone-app side implementations to accommodate their requirements for sending additional configuration data. The following are the important features of this implementation:

1. **Extensible Protocol**

The protocol is completely flexible and it offers the ability for the developers to send custom configuration in the provisioning process. The data representation is also left to the application to decide.

2. **Transport Flexibility**

The protocol can work on Wi-Fi (SoftAP + HTTP server) or on Bluetooth LE as a transport protocol. The framework provides an ability to add support for any other transport easily as long as command-response behavior can be supported on the transport.

3. **Security Scheme Flexibility**

It is understood that each use case may require different security scheme to secure the data that is exchanged in the provisioning process. Some applications may work with SoftAP that is WPA2 protected or Bluetooth LE with the "just-works" security. Or the applications may consider the transport to be insecure and may want application-level security. The unified provisioning framework allows the application to choose the security as deemed suitable.

4. **Compact Data Representation**

The protocol uses `Google Protobufs <https://developers.google.com/protocol-buffers/>`_ as a data representation for session setup and Wi-Fi provisioning. They provide a compact data representation and ability to parse the data in multiple programming languages in native format. Please note that this data representation is not forced on application-specific data and the developers may choose the representation of their choice.

Typical Provisioning Process
>>>>>>>>>>>>>>>>>>>>>>>>>>>>

.. seqdiag::
    :caption: Typical Provisioning Process
    :align: center

    seqdiag typical-prov-process {
        activation = none;
        node_width = 80;
        node_height = 60;
        edge_length = 360;
        span_height = 5;
        default_shape = roundedbox;
        default_fontsize = 12;

        CLIENT  [label = "Client"];
        DEVICE  [label = "Device"];

        === 1. Transport-specific discovery and connection ===
        DEVICE -> CLIENT [label="Some form of beaconing"];
        CLIENT -> DEVICE [label="Client connects"];
        === 2. Session Establishment ====
        CLIENT -> DEVICE [label="Get Version Request"];
        DEVICE -> CLIENT [label="Get Version Response"];
        CLIENT -> DEVICE [label="Session Setup Request"];
        DEVICE -> CLIENT [label="Session Setup Response"];
        CLIENT --> DEVICE;
        ... One or multiple steps as per protocol ...
        DEVICE --> CLIENT
        === 3. Configuration ===
        CLIENT --> DEVICE [label="App-specific Set Config (optional)"];
        DEVICE --> CLIENT [label="Set Config Response (optional)"];
        CLIENT -> DEVICE [label="Wi-Fi SetConfig(SSID, Passphrase...)"];
        DEVICE -> CLIENT [label="Wi-Fi SetConfig response"];
        CLIENT -> DEVICE [label="Wi-Fi ApplyConfig cmd"];
        DEVICE -> CLIENT [label="Wi-Fi ApplyConfig resp"];
        CLIENT -> DEVICE [label="Wi-Fi GetStatus cmd (repeated)"];
        DEVICE -> CLIENT [label="Wi-Fi GetStatus resp (repeated)"];
        === 4. Close connection ===
        DEVICE -> CLIENT [label="Close Connection"];
    }

Deciding on Transport
>>>>>>>>>>>>>>>>>>>>>

The unified provisioning subsystem supports Wi-Fi (SoftAP+HTTP server) and Bluetooth LE (GATT based) transport schemes. The following points need to be considered while selecting the best possible transport for provisioning:

1. The Bluetooth LE-based transport has the advantage of maintaining an intact communication channel between the device and the client during the provisioning, which ensures reliable provisioning feedback.
2. The Bluetooth LE-based provisioning implementation makes the user experience better from the phone apps as on Android and iOS both, the phone app can discover and connect to the device without requiring the user to go out of the phone app.
3. However, the Bluetooth LE transport consumes about 110 KB memory at runtime. If the product does not use the Bluetooth LE or Bluetooth functionality after provisioning is done, almost all the memory can be reclaimed and added into the heap.
4. The SoftAP-based transport is highly interoperable. However, there are a few considerations:

    - The device uses the same radio to host the SoftAP and also to connect to the configured AP. Since these could potentially be on different channels, it may cause connection status updates not to be reliably received by the phone

    - The phone (client) has to disconnect from its current AP in order to connect to the SoftAP. The original network will get restored only when the provisioning process is complete, and the softAP is taken down.

5. The SoftAP transport does not require much additional memory for the Wi-Fi use cases.
6. The SoftAP-based provisioning requires the phone-app user to go to ``System Settings`` to connect to the Wi-Fi network hosted by the device in the iOS system. The discovery (scanning) as well as connection APIs are not available for the iOS applications.

Deciding on Security
>>>>>>>>>>>>>>>>>>>>

Depending on the transport and other constraints, the security scheme needs to be selected by the application developers. The following considerations need to be given from the provisioning-security perspective:

1. The configuration data sent from the client to the device and the response have to be secured.
2. The client should authenticate the device that it is connected to.
3. The device manufacturer may choose proof-of-possession (PoP), a unique per-device secret to be entered on the provisioning client as a security measure to make sure that only the user can provision the device in their possession.

There are two levels of security schemes, of which the developer may select one or a combination, depending on requirements.

1. **Transport Security**

For SoftAP provisioning, developers may choose WPA2-protected security with unique per-device passphrase. Unique per-device passphrase can also act as a proof-of-possession. For Bluetooth LE, the "just-works" security can be used as a transport-level security after assessing its provided level of security.

2. **Application Security**

The unified provisioning subsystem provides the application-level security (`Security 1 Scheme`_) that provides data protection and authentication through PoP, if the application does not use the transport-level security, or if the transport-level security is not sufficient for the use case.

Device Discovery
>>>>>>>>>>>>>>>>

The advertisement and device discovery is left to the application and depending on the protocol chosen, the phone apps and device-firmware application can choose appropriate method for advertisement and discovery.

For the SoftAP+HTTP transport, typically the SSID (network name) of the AP hosted by the device can be used for discovery.

For the Bluetooth LE transport, device name or primary service included in the advertisement or a combination of both can be used for discovery.

Architecture
>>>>>>>>>>>>

The below diagram shows the architecture of unified provisioning:

.. figure:: ../../../_static/unified_provisioning.png
    :align: center
    :alt: Unified Provisioning Architecture

    Unified Provisioning Architecture

It relies on the base layer called :doc:`protocomm` (protocomm) which provides a framework for security schemes and transport mechanisms. The Wi-Fi Provisioning layer uses protocomm to provide simple callbacks to the application for setting the configuration and getting the Wi-Fi status. The application has control over implementation of these callbacks. In addition, the application can directly use protocomm to register custom handlers.

The application creates a protocomm instance which is mapped to a specific transport and specific security scheme. Each transport in the protocomm has a concept of an "end-point" which corresponds to the logical channel for communication for specific type of information. For example, security handshake happens on a different endpoint from the Wi-Fi configuration endpoint. Each end-point is identified using a string and depending on the transport internal representation of the end-point changes. In case of the SoftAP+HTTP transport, the end-point corresponds to URI, whereas in case of Bluetooth LE, the end-point corresponds to the GATT characteristic with specific UUID. Developers can create custom end-points and implement handler for the data that is received or sent over the same end-point.

.. _provisioning_security_schemes:

Security Schemes
>>>>>>>>>>>>>>>>

At present, the unified provisioning supports the following security schemes:

1. Security 0

No security (No encryption).

2. Security 1

Curve25519-based key exchange, shared key derivation and AES256-CTR mode encryption of the data. It supports two modes :

    a. Authorized - Proof of Possession (PoP) string used to authorize session and derive shared key.

    b. No Auth (Null PoP) - Shared key derived through key exchange only.

3. Security 2

SRP6a-based shared key derivation and AES256-GCM mode encryption of the data.

.. note::

    The respective security schemes need to be enabled through the project configuration menu. Please refer to :ref:`enabling-protocomm-security-version` for more details.

Security 1 Scheme
>>>>>>>>>>>>>>>>>

The Security 1 scheme details are shown in the below sequence diagram:

.. seqdiag::
    :caption: Security 1
    :align: center

    seqdiag security1 {
        activation = none;
        node_width = 80;
        node_height = 60;
        edge_length = 480;
        span_height = 5;
        default_shape = roundedbox;
        default_fontsize = 12;

        CLIENT  [label = "Client"];
        DEVICE  [label = "Device"];

        === Security 1 ===
        CLIENT -> CLIENT [label = "Generate\nKey Pair", rightnote = "{cli_privkey, cli_pubkey} = curve25519_keygen()"];
        CLIENT -> DEVICE [label = "SessionCmd0(cli_pubkey)"];
        DEVICE -> DEVICE [label = "Generate\nKey Pair", leftnote = "{dev_privkey, dev_pubkey} = curve25519_keygen()"];
        DEVICE -> DEVICE [label = "Initialization\nVector", leftnote = "dev_rand = gen_16byte_random()"];
        DEVICE -> DEVICE [label = "Shared Key", leftnote = "shared_key(No PoP) = curve25519(dev_privkey, cli_pubkey) \nshared_key(with PoP) = curve25519(dev_privkey, cli_pubkey) ^ SHA256(pop)"];
        DEVICE -> CLIENT [label = "SessionResp0(dev_pubkey, dev_rand)"];
        CLIENT -> CLIENT [label = "Shared Key", rightnote = "shared_key(No PoP) = curve25519(cli_privkey, dev_pubkey)\nshared_key(with PoP) = curve25519(cli_privkey, dev_pubkey) ^ SHA256(pop)"];
        CLIENT -> CLIENT [label = "Verification\nToken", rightnote = "cli_verify = aes_ctr_enc(key=shared_key, data=dev_pubkey, nonce=dev_rand)"];
        CLIENT -> DEVICE [label = "SessionCmd1(cli_verify)"];
        DEVICE -> DEVICE [label = "Verify Client", leftnote = "check (dev_pubkey == aes_ctr_dec(cli_verify...)"];
        DEVICE -> DEVICE [label = "Verification\nToken", leftnote = "dev_verify = aes_ctr_enc(key=shared_key, data=cli_pubkey, nonce=(prev-context))"];
        DEVICE -> CLIENT [label = "SessionResp1(dev_verify)"];
        CLIENT -> CLIENT [label = "Verify Device", rightnote = "check (cli_pubkey == aes_ctr_dec(dev_verify...)"];
    }


Security 2 Scheme
>>>>>>>>>>>>>>>>>

The Security 2 scheme is based on the Secure Remote Password (SRP6a) protocol, see `RFC 5054 <https://datatracker.ietf.org/doc/html/rfc5054>`_.

The protocol requires the Salt and Verifier to be generated beforehand with the help of the identifying username ``I`` and the plaintext password ``p``. The Salt and Verifier are then stored on {IDF_TARGET_NAME}.

- The password ``p`` and the username ``I`` are to be provided to the Phone App (Provisioning entity) by suitable means, e.g., QR code sticker.

Details about the Security 2 scheme are shown in the below sequence diagram:

.. seqdiag::
    :caption: Security 2
    :align: center

    seqdiag security2 {
        activation = none;
        node_width = 80;
        node_height = 60;
        edge_length = 550;
        span_height = 5;
        default_shape = roundedbox;
        default_fontsize = 12;

        CLIENT  [label = "Client\n(PhoneApp)"];
        DEVICE  [label = "Device\n(ESP)"];

        === Security 2 ===
        CLIENT -> CLIENT [label = "Generate\nKey Pair", rightnote = "a (cli_privkey) = 256 bit random value,
        A (cli_pubkey) = g^a.
        g - generator, N - large safe prime,
        All arithmetic operations are performed in ring of integers modulo N,
        thus all occurrences like y^z should be read as y^z modulo N."];
                CLIENT -> DEVICE [label = "SessionCmd0(cli_pubkey A, username I)"];
                DEVICE -> DEVICE [label = "Obtain\n Salt and Verifier", leftnote = "Obtain salt and verifier stored on esp
        Salt s = 256 bit random value,
        Verifier v  = g^x where x = H(s | I | p)"];
                DEVICE -> DEVICE [label = "Generate\nKey Pair", leftnote = "b (dev_privkey) = 256 bit random value
        B(dev_pubkey) = k*v + g^b where k = H(N, g)"];
                DEVICE -> DEVICE [label = "Shared Key", leftnote = "Shared Key K = H(S) where,
        S = (A * v^u) ^ b
        u = H(A, B)"];
                DEVICE -> CLIENT [label = "SessionResp0(dev_pubkey B, dev_rand)"];
                CLIENT -> CLIENT [label = "Shared Key", rightnote = "shared_key(K) = H(S) where,
        S = (B - k*v) ^ (a + ux),
        u = H(A, B),
        k = H(N, g),
        v = g^x,
        x = H(s | I | p).

        "];
                CLIENT -> CLIENT [label = "Verification\nToken", rightnote = "client_proof M =  H[H(N) XOR H(g) | H(I) | s | A | B | K]"];
                CLIENT -> DEVICE [label = "SessionCmd1(client_proof M1)"];
                DEVICE -> DEVICE [label = "Verify Client", leftnote = "device generates M1 =  H[H(N) XOR H(g) | H(I) | s | A | B | K]
        device verifies this M1 with the M1 obtained from Client"];
                DEVICE -> DEVICE [label = "Verification\nToken", leftnote = "
        Device generate device_proof M2 = H(A, M, K)"];
                DEVICE -> DEVICE [label = "Initialization\nVector", leftnote = "dev_rand = gen_16byte_random()
        This random number is to be used for AES-GCM operation
         for encryption and decryption of the data using the shared secret"];
                DEVICE -> CLIENT [label = "SessionResp1(device_proof M2, dev_rand)"];
                CLIENT -> CLIENT [label = "Verify Device", rightnote = "Client calculates device proof M2 as M2 = H(A, M, K)
        client verifies this M2 with M2 obtained from device"];
    }

Sample Code
>>>>>>>>>>>

Please refer to :doc:`protocomm` and :doc:`wifi_provisioning` for API guides and code snippets on example usage.

Application implementation can be found as an example under :example:`provisioning`.

Provisioning Tools
>>>>>>>>>>>>>>>>>>

Provisioning applications are available for various platforms, along with source code:

* Android:
    * `Bluetooth LE Provisioning app on Play Store <https://play.google.com/store/apps/details?id=com.espressif.provble>`_.
    * `SoftAP Provisioning app on Play Store <https://play.google.com/store/apps/details?id=com.espressif.provsoftap>`_.
    * Source code on GitHub: `esp-idf-provisioning-android <https://github.com/espressif/esp-idf-provisioning-android>`_.
* iOS:
    * `Bluetooth LE Provisioning app on App Store <https://apps.apple.com/in/app/esp-ble-provisioning/id1473590141>`_.
    * `SoftAP Provisioning app on App Store <https://apps.apple.com/in/app/esp-softap-provisioning/id1474040630>`_.
    * Source code on GitHub: `esp-idf-provisioning-ios <https://github.com/espressif/esp-idf-provisioning-ios>`_.
* Linux/macOS/Windows: :idf:`tools/esp_prov`, a Python-based command line tool for provisioning.

The phone applications offer simple UI and are thus more user centric, while the command-line application is useful as a debugging tool for developers.
