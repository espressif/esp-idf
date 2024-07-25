NVS Encryption
==============

:link_to_translation:`zh_CN:[中文]`

Overview
--------

This guide provides an overview of the NVS encryption feature. NVS encryption helps to achieve secure storage on the device flash memory.

Data stored in NVS partitions can be encrypted using XTS-AES in the manner similar to the one mentioned in disk encryption standard IEEE P1619. For the purpose of encryption, each entry is treated as one ``sector`` and relative address of the entry (w.r.t., partition-start) is fed to the encryption algorithm as ``sector-number``.

.. only:: SOC_HMAC_SUPPORTED

    NVS encryption can be facilitated by enabling :ref:`CONFIG_NVS_ENCRYPTION` and :ref:`CONFIG_NVS_SEC_KEY_PROTECTION_SCHEME` > ``CONFIG_NVS_SEC_KEY_PROTECT_USING_FLASH_ENC`` or ``CONFIG_NVS_SEC_KEY_PROTECT_USING_HMAC`` depending on the scheme to be used.


NVS Encryption: Flash Encryption-Based Scheme
---------------------------------------------

In this scheme, the keys required for NVS encryption are stored in yet another partition, which is protected using :doc:`Flash Encryption <../../security/flash-encryption>`. Therefore, enabling :doc:`Flash Encryption <../../security/flash-encryption>` becomes a prerequisite for NVS encryption here.

.. only:: SOC_HMAC_SUPPORTED

    NVS encryption should be enabled when :doc:`../../security/flash-encryption` is enabled because the Wi-Fi driver stores credentials (like SSID and passphrase) in the default NVS partition. It is important to encrypt them if platform-level encryption is already enabled.

.. only:: not SOC_HMAC_SUPPORTED

    NVS encryption is enabled by default when :doc:`../../security/flash-encryption` is enabled. This is done because Wi-Fi driver stores credentials (like SSID and passphrase) in the default NVS partition. It is important to encrypt them as default choice if platform-level encryption is already enabled.

For using NVS encryption using this scheme, the partition table must contain the :ref:`nvs_encr_key_partition`. Two partition tables containing the :ref:`nvs_encr_key_partition` are provided for NVS encryption under the partition table option (``menuconfig`` > ``Partition Table``). They can be selected with the project configuration menu (``idf.py menuconfig``). Please refer to the example :example:`security/flash_encryption` for how to configure and use the NVS encryption feature.

.. _nvs_encr_key_partition:

NVS Key Partition
^^^^^^^^^^^^^^^^^

An application requiring NVS encryption support (using the Flash Encryption-based scheme) needs to be compiled with a key-partition of the type ``data`` and subtype ``nvs_keys``. This partition should be marked as ``encrypted`` and its size should be the minimum partition size (4 KB). Refer to :doc:`../../api-guides/partition-tables` for more details. Two additional partition tables which contain the :ref:`nvs_encr_key_partition` are provided under the partition table option (``menuconfig`` > ``Partition Table``). They can be directly used for NVS encryption. The structure of these partitions is depicted below:

.. highlight:: none

::

    +-----------+--------------+-------------+----+
    |              XTS encryption key (32)        |
    +---------------------------------------------+
    |              XTS tweak key (32)             |
    +---------------------------------------------+
    |                  CRC32 (4)                  |
    +---------------------------------------------+

The XTS encryption keys in the :ref:`nvs_encr_key_partition` can be generated in one of the following two ways.

**Generate the keys on {IDF_TARGET_NAME} chip itself**

    * When NVS encryption is enabled, the :cpp:func:`nvs_flash_init` API function can be used to initialize the encrypted default NVS partition. The API function internally generates the XTS encryption keys on the ESP chip. The API function finds the first :ref:`nvs_encr_key_partition`.

    * Then the API function automatically generates and stores the NVS keys in that partition by making use of the :cpp:func:`nvs_flash_generate_keys` API function provided by :component_file:`nvs_flash/include/nvs_flash.h`. New keys are generated and stored only when the respective key partition is empty. The same key partition can then be used to read the security configurations for initializing a custom encrypted NVS partition with help of :cpp:func:`nvs_flash_secure_init_partition`.

    * The API functions :cpp:func:`nvs_flash_secure_init` and :cpp:func:`nvs_flash_secure_init_partition` do not generate the keys internally. When these API functions are used for initializing encrypted NVS partitions, the keys can be generated after startup using the :cpp:func:`nvs_flash_generate_keys` API function provided by ``nvs_flash.h``. The API function then writes those keys onto the key-partition in encrypted form.

    .. note::

        Please note that ``nvs_keys`` partition must be completely erased before you start the application in this approach. Otherwise the application may generate the :c:macro:`ESP_ERR_NVS_CORRUPT_KEY_PART` error code assuming that ``nvs_keys`` partition is not empty and contains malformatted data. You can use the following command for this:
        ::

            parttool.py --port PORT --partition-table-file=PARTITION_TABLE_FILE --partition-table-offset PARTITION_TABLE_OFFSET erase_partition --partition-type=data --partition-subtype=nvs_keys

            # If Flash Encryption or Secure Boot are enabled then add "--esptool-erase-args=force" to suppress the error:
            # "Active security features detected, erasing flash is disabled as a safety measure.  Use --force to override ..."
            parttool.py --port PORT --esptool-erase-args=force --partition-table-file=PARTITION_TABLE_FILE --partition-table-offset PARTITION_TABLE_OFFSET erase_partition --partition-type=data --partition-subtype=nvs_keys

**Use a pre-generated NVS key partition**

    This option will be required by the user when keys in the :ref:`nvs_encr_key_partition` are not generated by the application. The :ref:`nvs_encr_key_partition` containing the XTS encryption keys can be generated with the help of :doc:`NVS Partition Generator Utility </api-reference/storage/nvs_partition_gen>`. Then the user can store the pre-generated key partition on the flash with help of the following two commands:

    1. Build and flash the partition table
    ::

        idf.py partition-table partition-table-flash

    2. Store the keys in the :ref:`nvs_encr_key_partition` (on the flash) with the help of :component_file:`parttool.py <partition_table/parttool.py>` (see Partition Tool section in :doc:`partition-tables </api-guides/partition-tables>` for more details)
    ::

        parttool.py --port PORT --partition-table-offset PARTITION_TABLE_OFFSET write_partition --partition-name="name of nvs_key partition" --input NVS_KEY_PARTITION_FILE

        # If Flash Encryption or Secure Boot are enabled then add "--esptool-erase-args=force" to suppress the error:
        # "Active security features detected, erasing flash is disabled as a safety measure.  Use --force to override ..."
        parttool.py --port PORT --esptool-erase-args=force --partition-table-offset PARTITION_TABLE_OFFSET write_partition --partition-name="name of nvs_key partition" --input NVS_KEY_PARTITION_FILE

    .. note::
        If the device is encrypted in flash encryption development mode and you want to renew the NVS key partition, you need to tell :component_file:`parttool.py<partition_table/parttool.py>` to encrypt the NVS key partition and you also need to give it a pointer to the unencrypted partition table in your build directory (build/partition_table) since the partition table on the device is encrypted, too. You can use the following command:
        ::

            parttool.py --esptool-write-args encrypt --port PORT --partition-table-file=PARTITION_TABLE_FILE --partition-table-offset PARTITION_TABLE_OFFSET write_partition --partition-name="name of nvs_key partition" --input NVS_KEY_PARTITION_FILE

            # If Flash Encryption or Secure Boot are enabled then add "--esptool-erase-args=force" to suppress the error:
            # "Active security features detected, erasing flash is disabled as a safety measure.  Use --force to override ..."
            parttool.py --esptool-erase-args=force --esptool-write-args encrypt --port PORT --partition-table-file=PARTITION_TABLE_FILE --partition-table-offset PARTITION_TABLE_OFFSET write_partition --partition-name="name of nvs_key partition" --input NVS_KEY_PARTITION_FILE

Since the key partition is marked as ``encrypted`` and :doc:`Flash Encryption <../../security/flash-encryption>` is enabled, the bootloader will encrypt this partition using flash encryption key on the first boot.

It is possible for an application to use different keys for different NVS partitions and thereby have multiple key-partitions. However, it is a responsibility of the application to provide the correct key-partition and keys for encryption or decryption.

.. only:: SOC_HMAC_SUPPORTED

    NVS Encryption: HMAC Peripheral-Based Scheme
    --------------------------------------------

    In this scheme, the XTS keys required for NVS encryption are derived from an HMAC key programmed in eFuse with the purpose :cpp:enumerator:`esp_efuse_purpose_t::ESP_EFUSE_KEY_PURPOSE_HMAC_UP`. Since the encryption keys are derived at runtime, they are not stored anywhere in the flash. Thus, this feature does not require a separate :ref:`nvs_encr_key_partition`.

    .. note::

        This scheme enables us to achieve secure storage on {IDF_TARGET_NAME} **without enabling flash encryption**.

    .. important::

        Please take note that this scheme uses one eFuse block for storing the HMAC key required for deriving the encryption keys.

    - When NVS encryption is enabled, the :cpp:func:`nvs_flash_init` API function can be used to initialize the encrypted default NVS partition. The API function first checks whether an HMAC key is present at :ref:`CONFIG_NVS_SEC_HMAC_EFUSE_KEY_ID`.

    .. note::

        The valid range for the config :ref:`CONFIG_NVS_SEC_HMAC_EFUSE_KEY_ID` is from ``0`` (:cpp:enumerator:`hmac_key_id_t::HMAC_KEY0`) to ``5`` (:cpp:enumerator:`hmac_key_id_t::HMAC_KEY5`). By default, the config is set to ``6`` (:cpp:enumerator:`hmac_key_id_t::HMAC_KEY_MAX`), which have to be configured before building the user application.

    - If no key is found, a key is generated internally and stored at the eFuse block specified at :ref:`CONFIG_NVS_SEC_HMAC_EFUSE_KEY_ID`.
    - If a key is found with the purpose :cpp:enumerator:`esp_efuse_purpose_t::ESP_EFUSE_KEY_PURPOSE_HMAC_UP`, the same is used for the derivation of the XTS encryption keys.
    - If the specified eFuse block is found to be occupied with a key with a purpose other than :cpp:enumerator:`esp_efuse_purpose_t::ESP_EFUSE_KEY_PURPOSE_HMAC_UP`, an error is thrown.

    - The API :cpp:func:`nvs_flash_init` then automatically generates the NVS keys on demand by using the :cpp:func:`nvs_flash_generate_keys_v2` API function provided by the :component_file:`nvs_flash/include/nvs_flash.h`. The same keys can also be used to read the security configurations (see :cpp:func:`nvs_flash_read_security_cfg_v2`) for initializing a custom encrypted NVS partition with help of :cpp:func:`nvs_flash_secure_init_partition`.

    - The API functions :cpp:func:`nvs_flash_secure_init` and :cpp:func:`nvs_flash_secure_init_partition` do not generate the keys internally. When these API functions are used for initializing encrypted NVS partitions, the keys can be generated after startup using the :cpp:func:`nvs_flash_generate_keys_v2` API function or take and populate the NVS security configuration structure :cpp:type:`nvs_sec_cfg_t` with :cpp:func:`nvs_flash_read_security_cfg_v2` and feed them into the above APIs.

    .. note:: Users can program their own HMAC key in eFuse block beforehand by using the following command:
        ::

            idf.py -p PORT efuse-burn-key <BLOCK_KEYN> <hmac_key_file.bin> HMAC_UP

Encrypted Read/Write
--------------------

The same NVS API functions ``nvs_get_*`` or ``nvs_set_*`` can be used for reading of, and writing to an encrypted NVS partition as well.

**Encrypt the default NVS partition**

- To enable encryption for the default NVS partition, no additional step is necessary. When :ref:`CONFIG_NVS_ENCRYPTION` is enabled, the :cpp:func:`nvs_flash_init` API function internally performs some additional steps to enable encryption for the default NVS partition depending on the scheme being used (set by :ref:`CONFIG_NVS_SEC_KEY_PROTECTION_SCHEME`).

- For the flash encryption-based scheme, the first :ref:`nvs_encr_key_partition` found is used to generate the encryption keys while for the HMAC one, keys are generated using the HMAC key burnt in eFuse at :ref:`CONFIG_NVS_SEC_HMAC_EFUSE_KEY_ID` (refer to the API documentation for more details).

Alternatively, :cpp:func:`nvs_flash_secure_init` API function can also be used to enable encryption for the default NVS partition.

**Encrypt a custom NVS partition**

- To enable encryption for a custom NVS partition, :cpp:func:`nvs_flash_secure_init_partition` API function is used instead of :cpp:func:`nvs_flash_init_partition`.

- When :cpp:func:`nvs_flash_secure_init` and :cpp:func:`nvs_flash_secure_init_partition` API functions are used, the applications are expected to follow the steps below in order to perform NVS read/write operations with encryption enabled:

    1. Populate the NVS security configuration structure :cpp:type:`nvs_sec_cfg_t`

        * For the Flash Encryption-based scheme

            - Find key partition and NVS data partition using ``esp_partition_find*`` API functions.
            - Populate the :cpp:type:`nvs_sec_cfg_t` struct using the :cpp:func:`nvs_flash_read_security_cfg` or :cpp:func:`nvs_flash_generate_keys` API functions.

        .. only:: SOC_HMAC_SUPPORTED

            * For the HMAC-based scheme

                - Set the scheme-specific config data with :cpp:type:`nvs_sec_config_hmac_t` and register the HMAC-based scheme with the API :cpp:func:`nvs_sec_provider_register_hmac` which will also populate the scheme-specific handle (see :cpp:type:`nvs_sec_scheme_t`).
                - Populate the :cpp:type:`nvs_sec_cfg_t` struct using the :cpp:func:`nvs_flash_read_security_cfg_v2` or :cpp:func:`nvs_flash_generate_keys_v2` API functions.

            .. code-block:: c

                    nvs_sec_cfg_t cfg = {};
                    nvs_sec_scheme_t *sec_scheme_handle = NULL;

                    nvs_sec_config_hmac_t sec_scheme_cfg = {};
                    hmac_key_id_t hmac_key = HMAC_KEY0;
                    sec_scheme_cfg.hmac_key_id = hmac_key;

                    ret = nvs_sec_provider_register_hmac(&sec_scheme_cfg, &sec_scheme_handle);
                    if (ret != ESP_OK) {
                        return ret;
                    }

                    ret = nvs_flash_read_security_cfg_v2(sec_scheme_handle, &cfg);
                    if (ret != ESP_OK) {
                        if (ret == ESP_ERR_NVS_SEC_HMAC_KEY_NOT_FOUND) {
                            ret = nvs_flash_generate_keys_v2(&sec_scheme_handle, &cfg);
                            if (ret != ESP_OK) {
                                ESP_LOGE(TAG, "Failed to generate NVS encr-keys!");
                                return ret;
                            }
                        }
                        ESP_LOGE(TAG, "Failed to read NVS security cfg!");
                        return ret;
                    }

    2. Initialise NVS flash partition using the :cpp:func:`nvs_flash_secure_init` or :cpp:func:`nvs_flash_secure_init_partition` API functions.
    3. Open a namespace using the :cpp:func:`nvs_open` or :cpp:func:`nvs_open_from_partition` API functions.
    4. Perform NVS read/write operations using ``nvs_get_*`` or ``nvs_set_*``.
    5. Deinitialise an NVS partition using :cpp:func:`nvs_flash_deinit`.

.. only:: SOC_HMAC_SUPPORTED

    .. note::
        While using the HMAC-based scheme, the above workflow can be used without enabling any of the config options for NVS encryption - :ref:`CONFIG_NVS_ENCRYPTION`, :ref:`CONFIG_NVS_SEC_KEY_PROTECTION_SCHEME` -> ``CONFIG_NVS_SEC_KEY_PROTECT_USING_HMAC`` and :ref:`CONFIG_NVS_SEC_HMAC_EFUSE_KEY_ID` to encrypt the default as well as custom NVS partitions with :cpp:func:`nvs_flash_secure_init` API.


NVS Security Provider
---------------------

The component :component:`nvs_sec_provider` stores all the implementation-specific code for the NVS encryption schemes and would also accommodate any future schemes. This component acts as an interface to the :component:`nvs_flash` component for the handling of encryption keys. :component:`nvs_sec_provider` has a configuration menu of its own, based on which the selected security scheme and the corresponding settings are registered for the :component:`nvs_flash` component.

.. only:: SOC_HMAC_SUPPORTED

    This component offers factory functions with which a particular security scheme can be registered without having to worry about the APIs to generate and read the encryption keys (e.g., :cpp:func:`nvs_sec_provider_register_hmac`). Refer to the :example:`security/nvs_encryption_hmac` example for API usage.


API Reference
-------------

.. include-build-file:: inc/nvs_sec_provider.inc
