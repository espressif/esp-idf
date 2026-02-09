Secure Storage
==============

Overview
--------
The TEE Secure Storage service provides persistent storage for securely storing sensitive data, such as cryptographic keys, cloud credentials, or other general-purpose information. It uses a dedicated flash partition of type ``data`` and subtype ``nvs``. The TEE ensures both confidentiality and integrity of the stored data.

.. only:: SOC_HMAC_SUPPORTED

  TEE Secure Storage adopts the :doc:`../../api-reference/storage/nvs_flash` partition format and uses the HMAC peripheral-based XTS-AES encryption scheme, as detailed :ref:`here <nvs_encr_hmac_scheme>`. The AES encryption keys are derived from an HMAC key programmed in eFuse with the purpose :cpp:enumerator:`esp_efuse_purpose_t::ESP_EFUSE_KEY_PURPOSE_HMAC_UP`. Please note that the TEE Secure storage does not support the :ref:`NVS Flash Encryption-based scheme <nvs_encr_flash_enc_scheme>`.

.. only:: not SOC_HMAC_SUPPORTED

  TEE Secure Storage adopts the :doc:`../../api-reference/storage/nvs_flash` partition format and uses the HMAC-based XTS-AES encryption scheme.  There is, however, an important difference in how the HMAC step is handled.  Since {IDF_TARGET_NAME} lacks a hardware HMAC peripheral, the HMAC computation is carried out in software with assistance from the SHA peripheral.The AES encryption keys are derived from an eFuse key with the purpose :cpp:enumerator:`esp_efuse_purpose_t::ESP_EFUSE_KEY_PURPOSE_USER`. Please note that the TEE Secure storage does not support the :ref:`NVS Flash Encryption-based scheme <nvs_encr_flash_enc_scheme>`.

.. important::

  - One eFuse block is required to store the HMAC key used for deriving the NVS encryption keys. This key is exclusive to the TEE and **CANNOT** be used by the REE for any purpose.
  - The required key must be programmed into eFuse before firmware execution, as TEE Secure Storage does not support generating it on-device. If no valid key with the required purpose is found in the configured eFuse block, an error will be raised at runtime.

Additionally, the secure storage provides interfaces for performing the following cryptographic services from the TEE using securely stored key material:

#. Authenticated encryption and decryption using the ``aes256_gcm`` algorithm
#. Message signing and public key retrieval using ECDSA with the following curves

   .. list::
      - ``ecdsa_secp256r1``
      :SOC_ECDSA_SUPPORT_CURVE_P384: - ``ecdsa_secp384r1``


.. note::

  As per the current implementation, the TEE Secure Storage partition **must** have the label ``secure_storage``.

.. only:: SOC_HMAC_SUPPORTED

  TEE secure storage also supports ECDSA signing with keys derived via PBKDF2 (Password-Based Key Derivation Function 2), using an HMAC key programmed in eFuse along with a user-provided salt. This mechanism enables ECDSA signing on the P-256 curve without requiring storage of the actual private keys. The eFuse HMAC key ID for the PBKDF2 operations is specified via the :ref:`CONFIG_SECURE_TEE_PBKDF2_EFUSE_HMAC_KEY_ID` option.

  .. important::

    - The eFuse HMAC key ID used for PBKDF2-based signing **CANNOT** be the same as the one used for deriving TEE secure storage encryption keys (i.e., :ref:`CONFIG_SECURE_TEE_SEC_STG_EFUSE_HMAC_KEY_ID`).
    - This eFuse ID is also exclusive to the TEE and **CANNOT** be used by the REE for any purpose.

Internals
---------

Each data object consisting of the type, associated metadata flags (e.g., ``WRITE_ONCE``), and the actual payload is encapsulated in a structured format and stored as a variable-length NVS blob in the secure storage partition.

.. note::

  As per the current implementation, all data objects in the TEE Secure Storage are to be stored in the ``tee_sec_stg_ns`` namespace.

Currently, TEE secure storage supports storing the following cryptographic keys:

#. ``aes256`` keys, including the key and initialization vector (IV)
#. ECDSA key-pairs for the following curves, including both public and private components

   .. list::
      - ``ecdsa_secp256r1``
      :SOC_ECDSA_SUPPORT_CURVE_P384: - ``ecdsa_secp384r1``


All assets related to TEE secure storage are protected by the APM peripheral and are inaccessible to the REE application. Any direct access attempts will result in a system fault. Future updates are planned to add support for additional key types and general-purpose data storage.

The TEE Secure Storage feature supports two modes for determining how the NVS encryption keys are derived (see :ref:`CONFIG_SECURE_TEE_SEC_STG_MODE`):

  - **Development** Mode: Encryption keys are embedded (constant for all instances) in the ESP-TEE firmware.
  - **Release** Mode: Encryption keys are derived via the HMAC peripheral (or software-based HMAC implementation) using a key stored in eFuse, specified by :ref:`CONFIG_SECURE_TEE_SEC_STG_EFUSE_HMAC_KEY_ID`.

.. only:: SOC_HMAC_SUPPORTED

  .. note::

      - The valid range for :ref:`CONFIG_SECURE_TEE_SEC_STG_EFUSE_HMAC_KEY_ID` is from ``0`` (:cpp:enumerator:`hmac_key_id_t::HMAC_KEY0`) to ``5`` (:cpp:enumerator:`hmac_key_id_t::HMAC_KEY5`). By default, this config is set to ``-1`` and must be configured before building the TEE application.

      - The following commands can be used to generate and program the HMAC key into the required eFuse block:

        ::

            # Generate a random 32-byte HMAC key
            openssl rand -out hmac_key_file.bin 32

            # Program the HMAC key into the eFuse block
            espefuse -p PORT burn-key <BLOCK_KEY0-5> hmac_key_file.bin HMAC_UP

.. only:: not SOC_HMAC_SUPPORTED

  .. note::

      - The valid range for :ref:`CONFIG_SECURE_TEE_SEC_STG_EFUSE_HMAC_KEY_ID` is from ``0`` (:cpp:enumerator:`esp_efuse_block_t::EFUSE_BLK_KEY0`) to ``5`` (:cpp:enumerator:`esp_efuse_block_t::EFUSE_BLK_KEY5`). By default, this config is set to ``-1`` and must be configured before building the TEE application.

      - The following commands can be used to generate and program the USER purpose key into the required eFuse block:

        ::

            # Generate a random 32-byte key
            openssl rand -out hmac_key_file.bin 32

            # Program the USER purpose key into the eFuse block
            espefuse -p PORT burn-key --no-read-protect <BLOCK_KEY0-5> hmac_key_file.bin USER

    .. warning::

        - When programming the key into eFuse, ensure that it is **NOT** marked as read-protected (use the ``--no-read-protect`` flag). If the key is read-protected, the TEE will be unable to access it.
        - However, this does not weaken security: the APM peripheral already blocks software access to the key, and any illegal read or write attempt from the REE triggers a fault.

Tools
-----

The :doc:`../../api-reference/storage/nvs_partition_gen` tool can be used to generate binary images compatible with the NVS format for use with TEE Secure Storage. Since TEE Secure Storage stores data objects using a custom structured format, an additional step is required to convert input data into this format prior to image generation and encryption.

To support this process, the :component_file:`esp_tee_sec_stg_keygen.py<esp_tee/scripts/esp_tee_sec_stg_keygen/esp_tee_sec_stg_keygen.py>` script is provided for generating secure key blobs corresponding to the various supported cryptographic algorithms. These key blobs are then referenced in the input CSV file (format described :ref:`here <nvs-csv-file-format>`) and passed to the NVS Partition Generator utility to produce an encrypted images suitable for TEE Secure Storage.

Refer the detailed steps given :component_file:`here<esp_tee/scripts/esp_tee_sec_stg_keygen/README.md>` on generating key blobs and encrypted NVS partition images for TEE Secure Storage.

Application Example
-------------------

The :example:`tee_secure_storage <security/tee/tee_secure_storage>` example demonstrates how to generate ECDSA key pairs and AES-256 keys in the TEE secure storage and use them for signing messages and encrypting/decrypting data.

API Reference
-------------

.. note::

    To use the TEE Secure Storage APIs in your project, ensure that the :component:`tee_sec_storage <esp_tee/subproject/components/tee_sec_storage>` component is listed as a local dependency in the component manager manifest file `idf_component.yml <https://docs.espressif.com/projects/idf-component-manager/en/latest/reference/manifest_file.html>`_. Refer to the :example:`tee_secure_storage <security/tee/tee_secure_storage>` example for guidance.

.. include-build-file:: inc/esp_tee_sec_storage.inc
