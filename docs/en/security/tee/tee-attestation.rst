Attestation
===========

Overview
--------

Attestation is the process of securely providing information about a device to external parties through a cryptographically secured token. It enables IoT devices, especially those without user interfaces or in remote locations, to introduce themselves securely to networks and IoT platforms by offering attestation information.

Thus, the attestation service is employed by the device to communicate evidence regarding its credentials and operational state to a remote relying party. The collected information is structured into the Entity Attestation Token (EAT) format.

To ensure security, the EAT is cryptographically protected. The remote relying party can then verify the authenticity of the EAT and make decisions about engaging with the device based on its contents.

.. note::

  - Support for Attestation can be toggled using the option :ref:`CONFIG_SECURE_TEE_ATTESTATION` (enabled by default).

Attestation Flow
----------------

.. seqdiag::
    :caption: ESP-TEE Attestation Flow
    :align: center

    seqdiag tee_attestation {
        activation = none;
        node_width = 125;
        node_height = 60;
        edge_length = 275;
        default_shape = roundedbox;
        default_fontsize = 13;

        ET  [label = "Entity"];
        RP  [label = "Relying Party"];
        AS  [label = "Attestation Service"];

        === Attestation ===
        RP -> ET [label = "Attestation Request"];
        ET -> ET [label = "EAT Generation\nand Signature", rightnote = "Attestor"];
        ET -> RP [label = "Attestation Evidence"];
        RP -> AS [label = "Attestation Evidence"];
        AS -> AS [label = "Evidence\nVerification", rightnote = "Verifier"];
        AS -> RP [label = "Attestation Result"];
        RP -> ET [label = "Relying Party Decision"];
    }

#. The Relying Party sends an Attestation Request (AR) to the device REE with some challenge or nonce (usually a random number).
#. The device REE forwards the request to the device TEE attestation service through the secure service call interface.
#. The TEE assembles the Entity Attestation Token (EAT) (the attestation evidence) by gathering all the relevant claim information as outlined in the EAT claim table. It then generates a signature on the claim data, appending it to the token.
#. The evidence is transmitted back to the remote attestation service (verifier) through the device REE and the relying party. In certain cases, the Relying Party itself may act as the attestation service.
#. The attestation service assesses the received evidence and, upon successful verification, compares the EAT claim data with the available reference values.
#. The relying party determines the trustworthiness of the device based on the attestation result and its predefined policy. Subsequently, it communicates this determination to the device.


Entity Attestation Token
------------------------

The Entity Attestation Token (EAT) is a small blob of data that contains claims and is cryptographically signed. The cryptographic signing secures the token, so the means to convey the token does not need to provide any security.

When an attestation request is sent to the device, it generates the EAT in the predefined JSON format as given below. The token contains all the necessary information about the hardware and software entities on device. Here, an ECDSA key-pair from the TEE secure storage is used for signing the EAT, utilizing the slot ID mentioned in the attestation request. The EAT contains the **compressed** ECDSA public key as well as the ``R`` and ``S`` components of the signature.

    .. figure:: ../../../_static/esp_tee/tee_attestation_evidence.png
        :align: center
        :scale: 75%
        :alt: ESP TEE Attestation Evidence
        :figclass: align-center

        ESP-TEE: Attestation Evidence

EAT: Header
^^^^^^^^^^^

.. list-table::
    :header-rows: 1
    :widths: 25 65 65
    :align: center

    * - **Claim**
      - **Description**
      - **Comments**
    * - Magic
      - Identifies the commencement of the EAT
      -
    * - Encryption Algorithm
      - Specifies encryption algorithm details if the EAT is encrypted
      -
    * - Signing Algorithm
      - Specifies signing algorithm details if the EAT is signed
      - Currently, only ``ecdsa_secp256r1_sha256`` is supported
    * - Key ID
      - Identifies the key to be utilized by the device for encryption and signing
      - TEE secure storage key ID (string)

EAT: Claim Table
^^^^^^^^^^^^^^^^

.. list-table::
    :header-rows: 1
    :widths: 25 65 65
    :align: center

    * - **Claim**
      - **Description**
      - **Comments**
    * - Authentication Challenge
      - Challenge data provided by the caller to protect against replay attacks. This is typically a cryptographic nonce (random value) or a hash of data that includes a nonce. When using a data hash, the caller must ensure replay protection by incorporating a nonce into the hashed data.
      -
    * - Client ID
      - Relying Party identification
      -
    * - Device ID
      - Device identification (should be unique)
      - SHA256 digest of the device MAC address
    * - Device Version
      - SoC H/W revision from eFuse
      -
    * - Instance ID
      - Instance identification
      - Public-key hash (SHA256) from the current secure storage slot for the active session
    * - Certification Reference
      - PSA certification ID
      -
    * - Device State
      - Development/Production Mode
      -
    * - Software Measurement
      - Details of the active firmware components - for e.g. app version, ESP-IDF version, digest, etc. There will be one entry of this type in the EAT for each firmware in the device.
      -

EAT: Software Measurement
^^^^^^^^^^^^^^^^^^^^^^^^^

.. list-table::
    :header-rows: 1
    :widths: 30 60 60
    :align: center

    * - **Key**
      - **Description**
      - **Comments**
    * - Version
      - Entity image version
      - Not applicable to the bootloader
    * - ESP-IDF Version
      -
      - Not applicable to the bootloader
    * - Secure Version
      - For firmware anti-rollback support
      - Applicable only to the REE application
    * - Digest: Type
      -
      - Currently, only ``SHA256`` is supported
    * - Digest: Calculated Value
      -
      -
    * - Digest: Validation
      - Whether the calculated digest matches the one embedded in the firmware image
      -
    * - Secure Boot signature verification status
      -
      -
    * - Secure Padding status
      -
      - For more details, refer :ref:`here <secure_padding>`.


Sample EAT in JSON format
^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: JSON

  {
    "header": {
      "magic": "44fef7cc",
      "encr_alg": "",
      "sign_alg": "ecdsa_secp256r1_sha256",
      "key_id": "tee_att_key0"
    },
    "eat": {
      "auth_challenge":"dcb9b53143ad6b081dad1a05c7ebda4e314d388762215799cf24ed52e9387678"
      "client_id": 262974944,
      "device_ver": 1,
      "device_id": "e8cddb2a7f9a5a7c61735d6dda26e4bd153c6d772a9be6f26bd321dfe25e0ac8",
      "instance_id": "1adba85e0df997fd961f25a9e312430cef162b5c69466cd5b172f1e65ac7360c",
      "psa_cert_ref": "0716053550477-10100",
      "device_status": 255,
      "sw_claims": {
        "tee": {
          "type": 1,
          "ver": "v0.3.0",
          "idf_ver": "v5.1.2-139-g07d83a7ced",
          "secure_ver": 0,
          "part_chip_rev": {
            "min": 0,
            "max": 99
          },
          "part_digest": {
            "type": 0,
            "calc_digest": "f732e7f285b7de7ac3167a867711eddbf17a2a05513d35e41cd1ebf2e0958b2e",
            "digest_validated": true,
            "sign_verified": true,
            "secure_padding": true
          }
        },
        "app": {
          "type": 2,
          "ver": "v0.1.0",
          "idf_ver": "v5.1.2-139-g07d83a7ced",
          "secure_ver": 0,
          "part_chip_rev": {
            "min": 0,
            "max": 99
          },
          "part_digest": {
            "type": 0,
            "calc_digest": "21e114fd30b9234c501525990dfab71d00348c531bb64224feff9deb32e66f9f",
            "digest_validated": true,
            "sign_verified": true,
            "secure_padding": true
          }
        },
        "bootloader": {
          "type": 0,
          "ver": "",
          "idf_ver": "",
          "secure_ver": -1,
          "part_chip_rev": {
            "min": 0,
            "max": 99
          },
          "part_digest": {
            "type": 0,
            "calc_digest": "516148649a7f670b894391ded9d64a0e8604c5cec9a1eeb0014d2549cdaa4725",
            "digest_validated": true,
            "sign_verified": true
          }
        }
      }
    },
    "public_key": {
      "compressed": "02a45c6c94c4be7722bd2513f4ccbc4daa369747e6e96e0f9f7a2eba055dee6d46"
    },
    "sign": {
      "r": "37bcc8ed9c15a4712c18fe20b257992e5d9ec273b6261675f247667b4575495b",
      "s": "28ce15da73880f7d5ee303948769b197077208f1f242aaee448e9ed23f9085fa"
    }
  }

Application Example
-------------------

The :example:`tee_attestation <security/tee/tee_attestation>` example demonstrates how to generate an entity attestation token containing validation details for all active firmware components (bootloader, TEE, and REE app).

API Reference
-------------

.. include-build-file:: inc/initial_attestation.inc
