Unified Provisioning
^^^^^^^^^^^^^^^^^^^^

Overview
>>>>>>>>
Unified provisioning support in the ESP-IDF provides an extensible mechanism to the developers to configure the device with the Wi-Fi credentials and/or other custom configuration using various transports and different security schemes. Depending on the use-case it provides a complete and ready solution for Wi-Fi network provisioning along with example iOS and Android applications. Or developers can extend the device-side and phone-app side implementations to accommodate their requirements for sending additional configuration data. Following are the important features of this implementation.

1. *Extensible Protocol:* The protocol is completely flexible and it offers the ability for the developers to send custom configuration in the provisioning process. The data representation too is left to the application to decide.
2. *Transport Flexibility:* The protocol can work on Wi-Fi (SoftAP + HTTP server) or on BLE as a transport protocol. The framework provides an ability to add support for any other transport easily as long as command-response behaviour can be supported on the transport. 
3. *Security Scheme Flexibility:* It's understood that each use-case may require different security scheme to secure the data that is exchanged in the provisioning process. Some applications may work with SoftAP that's WPA2 protected or BLE with "just-works" security. Or the applications may consider the transport to be insecure and may want application level security. The unified provisioning framework allows application to choose the security as deemed suitable.
4. *Compact Data Representation:* The protocol uses `Google Protobufs <https://developers.google.com/protocol-buffers/>`_ as a data representation for session setup and Wi-Fi provisioning. They provide a compact data representation and ability to parse the data in multiple programming languages in native format. Please note that this data representation is not forced on application specific data and the developers may choose the representation of their choice.

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

        === 1. Transport specific discovery and connection ===
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
        CLIENT --> DEVICE [label="App specific Set Config (optional)"];
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
Unified provisioning subsystem supports Wi-Fi (SoftAP+HTTP server) and BLE (GATT based) transport schemes. Following points need to be considered while selecting the best possible transport for provisioning.

1. BLE based transport has an advantage that in the provisioning process, the BLE communication channel stays intact between the device and the client. That provides reliable provisioning feedback.
2. BLE based provisioning implementation makes the user-experience better from the phone apps as on Android and iOS both, the phone app can discover and connect to the device without requiring user to go out of the phone app
3. BLE transport however consumes ~110KB memory at runtime. If the product does not use the BLE or BT functionality after provisioning is done, almost all the memory can be reclaimed back and can be added into the heap.
4. SoftAP based transport is highly interoperable; however as the same radio is shared between SoftAP and Station interface, the transport is not reliable in the phase when the Wi-Fi connection to external AP is attempted. Also, the client may roam back to different network when the SoftAP changes the channel at the time of Station connection.
5. SoftAP transport does not require much additional memory for the Wi-Fi use-cases
6. SoftAP based provisioning requires the phone app user to go to "System Settings" to connect to Wi-Fi network hosted by the device in case of iOS. The discovery (scanning) as well as connection API is not available for the iOS applications.

Deciding on Security
>>>>>>>>>>>>>>>>>>>>
Depending on the transport and other constraints the security scheme needs to be selected by the application developers. Following considerations need to be given from the provisioning security perspective:
1. The configuration data sent from the client to the device and the response has to be secured.
2. The client should authenticate the device it is connected to.
3. The device manufacturer may choose proof-of-possession - a unique per device secret to be entered on the provisioning client as a security measure to make sure that the user can provisions the device in the possession.

There are two levels of security schemes. The developer may select one or combination depending on requirements.

1. *Transport Security:* SoftAP provisioning may choose WPA2 protected security with unique per-device passphrase. Per-device unique passphrase can also act as a proof-of-possession. For BLE, "just-works" security can be used as a transport level security after understanding the level of security it provides.
2. *Application Security:* The unified provisioning subsystem provides application level security (*security1*) that provides data protection and authentication (through proof-of-possession) if the application does not use the transport level security or if the transport level security is not sufficient for the use-case.

Device Discovery
>>>>>>>>>>>>>>>>
The advertisement and device discovery is left to the application and depending on the protocol chosen, the phone apps and device firmware application can choose appropriate method to advertise and discovery.

For the SoftAP+HTTP transport, typically the SSID (network name) of the AP hosted by the device can be used for discovery.

For the BLE transport device name or primary service included in the advertisement or combination of both can be used for discovery.

Architecture
>>>>>>>>>>>>
The below diagram shows architecture of unified provisioning.

.. figure:: ../../../_static/unified_provisioning.png
    :align: center
    :alt: Unified Provisioning Architecture

    Unified Provisioning Architecture

It relies on the base layer called :doc:`protocomm` (Protocol Communication) which provides a framework for security schemes and transport mechanisms. Wi-Fi Provisioning layer uses Protocomm to provide simple callbacks to the application for setting the configuration and getting the Wi-Fi status. The application has control over implementation of these callbacks. In addition application can directly use protocomm to register custom handlers.

Application creates a protocomm instance which is mapped to a specific transport and specific security scheme. Each transport in the protocomm has a concept of an "end-point" which corresponds to logical channel for communication for specific type of information. For example security handshake happens on a different endpoint than the Wi-Fi configuration endpoint. Each end-point is identified using a string and depending on the transport internal representation of the end-point changes. In case of SoftAP+HTTP transport the end-point corresponds to URI whereas in case of BLE the end-point corresponds to GATT characteristic with specific UUID. Developers can create custom end-points and implement handler for the data that is received or sent over the same end-point.

Security Schemes
>>>>>>>>>>>>>>>>
At present unified provisioning supports two security schemes:
1. Security0 - No security (No encryption)
2. Security1 - Curve25519 based key exchange, shared key derivation and AES256-CTR mode encryption of the data. It supports two modes :

    a. Authorized - Proof of Possession (PoP) string used to authorize session and derive shared key
    b. No Auth (Null PoP) - Shared key derived through key exchange only

Security1 scheme details are shown in the below sequence diagram

.. seqdiag::
    :caption: Security1
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

Sample Code
>>>>>>>>>>>
Please refer to :doc:`protocomm` and :doc:`wifi_provisioning` for API guides and code snippets on example usage.

Various use case implementations can be found as examples under :example:`provisioning`.

Provisioning Tools
>>>>>>>>>>>>>>>>>>

Provisioning applications are available for various platforms, along with source code:

* Android : `esp-idf-provisioning-android <https://github.com/espressif/esp-idf-provisioning-android>`_
* iOS : `esp-idf-provisioning-ios <https://github.com/espressif/esp-idf-provisioning-ios>`_
* Linux/MacOS/Windows : :idf:`tools/esp_prov` (a python based command line tool for provisioning)

The phone applications offer simple UI and thus more user centric, while the command line application is useful as a debugging tool for developers.
