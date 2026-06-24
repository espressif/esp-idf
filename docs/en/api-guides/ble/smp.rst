SMP
=============

:link_to_translation:`zh_CN:[中文]`

The Security Manager Protocol (SMP) is implemented within Bluetooth Low Energy's GAP module. SMP is responsible for generating encryption and identity keys, defining a protocol for pairing and key distribution, and ensuring secure communication between devices in the protocol stack. This process requires an established connection at the data link layer and compliance with specific security standards.

The GAP SMP allows two devices to encrypt their communication at the data link layer by setting appropriate security levels, as specified in the SMP chapter of the Bluetooth Core Specification. Before detailing the implementation of GAP SMP, the following key concepts should be clarified:

- **Pairing:** The process in which two devices establish a connection with a defined security level.

- **Bonding:** The process in which at least one device sends security-related information (such as Long-Term Key (LTK), Connection Signature Resolving Key (CSRK), or Identity Resolving Key (IRK)) for future connections. If bonding occurs, key distribution follows pairing. However, bonding is not required for pairing. During pairing, devices exchange characteristics to determine if they support bonding. If neither device supports bonding, no security information is stored.

- **Authentication:** This process verifies the identity of a connected device, ensuring that it is not a third-party attacker. A de-authentication link does not automatically indicate that the connection is insecure. When both devices confirm the security attributes of the link encryption key, they are considered authenticated.

  During pairing, the Short-Term Key (STK) authentication generates a keyword. For devices with input/output and Out-of-Band (OOB) functions, all generated and exchanged keys possess Man-in-the-Middle (MITM) protection attributes. In this case, larger PINs or OOB keys enhance security. Conversely, if the Just Works method is employed, the generated and exchanged keys lack MITM protection.

- **Authorization:** The process of granting permission to perform certain operations at the application layer. Some applications require explicit authorization before use. If permission is not granted, the operation fails.


Bluetooth LE Encryption
--------------------------

Encryption in Bluetooth LE can be achieved in two ways:

- **Initial Encryption (No Bonding):** If two Bluetooth LE devices have not bonded, encryption occurs during the pairing process. The decision to bond or not depends on the pairing configuration.
- **Reconnection Encryption (Bonding):** If two devices have bonded, encryption is initiated using previously exchanged keys. In this case, one device triggers encryption by reusing stored keys from the bonding process.

The flowchart below illustrates how a master device initiates an encryption request in Just Works mode (*click to enlarge*):


.. seqdiag::
    :caption: The Flowchart of Encryption in Just Works Mode
    :align: center
    :scale: 200%

    seqdiag just-works-encrypt {
        activation = none;
        edge_length = 160;
        span_height = 20;
        default_shape = roundedbox;
        default_fontsize = 14;

        "Input\n[Master]";
        "API\n[Master]";
        "SMP\n[Master]";
        "SMP\n[Slave]";
        "API\n[Slave]";
        "Output\n[Slave]";

        "Input\n[Master]" -> "API\n[Master]" [label="esp_ble_set_encryption"];
        "API\n[Master]" -> "SMP\n[Master]";
        "SMP\n[Master]" -> "SMP\n[Slave]" [label="smp_pair_req"];
        "SMP\n[Slave]" -> "API\n[Slave]";
        "API\n[Slave]" -> "Output\n[Slave]" [label="esp_gap_ble_sec_req_evt"];
        "API\n[Slave]" <- "Output\n[Slave]" [label="esp_ble_gap_security_rsp"];
        "SMP\n[Slave]" <- "API\n[Slave]";
        "SMP\n[Master]" <- "SMP\n[Slave]" [label="smp_pair_rsq"];
        "SMP\n[Master]" -> "SMP\n[Slave]" [label="[ENCRYPT]", color=blue];
        "SMP\n[Master]" <- "SMP\n[Slave]" [label="[ENCRYPT]", color=blue];
        "API\n[Master]" <- "SMP\n[Master]";
        "SMP\n[Slave]" -> "API\n[Slave]";
        "Input\n[Master]" <- "API\n[Master]" [label="esp_gap_ble_auth_cmpl_evt"];
        "API\n[Slave]" -> "Output\n[Slave]" [label="esp_gap_ble_auth_cmpl_evt"];
    }


The flowchart below illustrates how a master device initiates an encryption request in Passkey Entry mode (*click to enlarge*):


.. seqdiag::
    :caption: The Flowchart of Encryption in Passkey Entry Mode
    :align: center
    :scale: 200%

    seqdiag passkey-entry-encrypt {
        activation = none;
        edge_length = 160;
        span_height = 20;
        default_shape = roundedbox;
        default_fontsize = 14;

        "Input\n[Master]";
        "API\n[Master]";
        "SMP\n[Master]";
        "SMP\n[Slave]";
        "API\n[Slave]";
        "Output\n[Slave]";

        "Input\n[Master]" -> "API\n[Master]" [label="esp_ble_set_encryption"];
        "API\n[Master]" -> "SMP\n[Master]";
        "SMP\n[Master]" -> "SMP\n[Slave]" [label="smp_pair_req"];
        "SMP\n[Slave]" -> "API\n[Slave]";
        "API\n[Slave]" -> "Output\n[Slave]" [label="esp_gap_ble_sec_req_evt"];
        "API\n[Slave]" <- "Output\n[Slave]" [label="esp_ble_gap_security_rsp"];
        "SMP\n[Slave]" <- "API\n[Slave]";
        "SMP\n[Master]" <- "SMP\n[Slave]" [label="smp_pair_rsq"];
        "Input\n[Master]" <- "API\n[Master]" [label="esp_gap_ble_passkey_notify_evt displays the passkey to the user, the peer device should enter this key and send it to the SMP stack"];
        "API\n[Slave]" <- "Output\n[Slave]" [label="esp_ble_passkey_reply send the passkey to the SMP stack which is displayed in the peer device"];
        "SMP\n[Master]" -> "SMP\n[Slave]" [label="[ENCRYPT]", color=blue];
        "SMP\n[Master]" <- "SMP\n[Slave]" [label="[ENCRYPT]", color=blue];
        "API\n[Master]" <- "SMP\n[Master]";
        "SMP\n[Slave]" -> "API\n[Slave]";
        "Input\n[Master]" <- "API\n[Master]" [label="esp_gap_ble_auth_cmpl_evt"];
        "API\n[Slave]" -> "Output\n[Slave]" [label="esp_gap_ble_auth_cmpl_evt"];
    }


Bluetooth LE Bonding
----------------------

Bonding between two Bluetooth LE devices is achieved via a GAP API call. According to the Bluetooth Core Specification, the purpose of bonding is to enable two Bluetooth LE devices, once encrypted by SMP, to use the same keys for link encryption when reconnecting. This process simplifies reconnection. During pairing, the devices exchange encryption keys and store them for long-term use.

The bonding process is illustrated in the following flowchart (*click to enlarge*):


.. seqdiag::
    :caption: The Flowchart of Bluetooth LE Bonding Process
    :align: center
    :scale: 200%

    seqdiag ble_bonding {
        activation = none;
        edge_length = 160;
        span_height = 20;
        default_shape = roundedbox;
        default_fontsize = 14;

        "Input\n[Master]";
        "API\n[Master]";
        "SMP\n[Master]";
        "LLC\n[Master]";
        "LLC\n[Slave]";
        "SMP\n[Slave]";
        "API\n[Slave]";
        "Output\n[Slave]";

        "Input\n[Master]" -> "API\n[Master]" [label="esp_ble_set_encryption"];
        "API\n[Master]" -> "SMP\n[Master]" [label="continues if bonding is completed with the peer device"];
        "SMP\n[Master]" -> "LLC\n[Master]" [label="llc_start_enc_cmd"];
        "LLC\n[Master]" -> "LLC\n[Slave]";
        "LLC\n[Slave]"  -> "SMP\n[Slave]" [label="llc_ltk_req_evt"];
        "LLC\n[Slave]"  <- "SMP\n[Slave]" [label="llc_ltk_req_reply_cmd"];
        "LLC\n[Master]" -> "LLC\n[Slave]" [label="[ENCRYPT]", color=blue];
        "LLC\n[Master]" <- "LLC\n[Slave]" [label="[ENCRYPT]", color=blue];
        "SMP\n[Master]" <- "LLC\n[Master]" [label="llc_enc_change_evt"];
        "LLC\n[Slave]"  -> "SMP\n[Slave]" [label="llc_enc_change_evt"];
        "API\n[Master]" <- "SMP\n[Master]";
        "SMP\n[Slave]" -> "API\n[Slave]";
        "Input\n[Master]" <- "API\n[Master]" [label="esp_gap_ble_auth_cmpl_evt"];
        "API\n[Slave]" -> "Output\n[Slave]" [label="esp_gap_ble_auth_cmpl_evt"];
    }


.. note::

   Bonding must be initiated by the master device after a connection has been established.


Implementation of SMP
----------------------------

The Bluetooth LE SMP calls encryption APIs in Bluetooth LE GAP, registers the Bluetooth LE GAP callbacks, and obtains the current encryption status through the return values of events.
