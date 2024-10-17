NVS 加密
==============

:link_to_translation:`en:[English]`

概述
--------

本文档主要介绍 NVS 加密功能，这一功能有助于实现设备在 flash 中的安全存储。

存储在 NVS 分区中的数据可以用 XTS-AES 进行加密，与磁盘加密标准 IEEE P1619 中提到的加密方式类似。加密时，每个条目都被视作一个 ``sector``，而条目的相对地址（相对于分区起始位置）作为 ``sector-number`` 输入加密算法。

.. only:: SOC_HMAC_SUPPORTED

    根据要使用的具体方案，可以选择启用 :ref:`CONFIG_NVS_ENCRYPTION` 和 :ref:`CONFIG_NVS_SEC_KEY_PROTECTION_SCHEME` > ``CONFIG_NVS_SEC_KEY_PROTECT_USING_FLASH_ENC`` 或 ``CONFIG_NVS_SEC_KEY_PROTECT_USING_HMAC`` 实现 NVS 加密。


NVS 加密：基于 flash 加密的方案
-------------------------------------

在这个方案中，NVS 加密所需的密钥存储在另一个分区中，该分区用 :doc:`../../security/flash-encryption` 进行保护。因此，使用该方案时，必须先启用 :doc:`../../security/flash-encryption`。

.. only:: SOC_HMAC_SUPPORTED

    启用 :doc:`../../security/flash-encryption` 时需同时启用 NVS 加密，因为 Wi-Fi 驱动程序会将凭据（如 SSID 和密码）储存在默认的 NVS 分区中。如已启用平台级加密，那么则需要同时启用 NVS 加密。

.. only:: not SOC_HMAC_SUPPORTED

    启用 :doc:`../../security/flash-encryption` 时，默认启用 NVS 加密。这是因为 Wi-Fi 驱动程序会将凭据（如 SSID 和密码）储存在默认的 NVS 分区中。如已启用平台级加密，那么则需要同时默认启用 NVS 加密。

要用这一方案进行 NVS 加密，分区表中必须包含 :ref:`nvs_encr_key_partition`。在分区表选项 ( ``menuconfig`` > ``Partition Table`` ) 中，有两个包含 :ref:`nvs_encr_key_partition` 的分区表，可通过项目配置菜单 ( ``idf.py menuconfig``) 进行选择。要了解如何配置和使用 NVS 加密功能，请参考示例 :example:`security/flash_encryption`。

.. _nvs_encr_key_partition:

NVS 密钥分区
^^^^^^^^^^^^^^^^^

应用如果要使用 NVS 加密（使用基于 flash 加密的方案）编译时，须使用类型为 ``data`` 和子类型为 ``nvs_keys`` 的密钥分区。该分区应被标记为 ``encrypted`` 且最小为 4 KB （最小分区大小）。参考 :doc:`../../api-guides/partition-tables` 了解详情。在分区表选项 ( ``menuconfig`` > ``Partition Table``) 中，有两个包含 :ref:`nvs_encr_key_partition` 的额外分区表，可以直接用于 NVS 加密。分区的结构如下所示：

.. highlight:: none

::

    +-----------+--------------+-------------+----+
    |              XTS encryption key (32)        |
    +---------------------------------------------+
    |              XTS tweak key (32)             |
    +---------------------------------------------+
    |                  CRC32 (4)                  |
    +---------------------------------------------+

可以通过以下两种方式之一生成 :ref:`nvs_encr_key_partition` 中的 XTS 加密密钥：

**在 {IDF_TARGET_NAME} 芯片上生成密钥**

    * 启用 NVS 加密时，可使用 API 函数 :cpp:func:`nvs_flash_init` 来初始化加密的默认 NVS 分区。该 API 函数会内部生成 ESP 芯片的 XTS 加密密钥，并找到第一个 :ref:`nvs_encr_key_partition`。

    * 然后，该 API 函数使用 :component_file:`nvs_flash/include/nvs_flash.h` 提供的 :cpp:func:`nvs_flash_generate_keys` API 函数，自动生成 NVS 密钥并存储到该分区。只有当相应的密钥分区为空时，才会生成和存储新密钥。然后，就可以利用 :cpp:func:`nvs_flash_secure_init_partition` 用此密钥分区来读取安全配置，以初始化自定义的加密 NVS 分区。

    * API 函数 :cpp:func:`nvs_flash_secure_init` 和 :cpp:func:`nvs_flash_secure_init_partition` 不会内部生成密钥。如果要使用这两个 API 函数初始化加密的 NVS 分区，可以在启动后使用 ``nvs_flash.h`` 提供的 :cpp:func:`nvs_flash_generate_keys` API 函数生成密钥，然后由该 API 函数将生成的密钥以加密形式写入密钥分区中。

    .. note::

        请注意，在使用此方法启动应用程序前，必须完全擦除 ``nvs_keys`` 分区。否则，应用程序可能会生成 :c:macro:`ESP_ERR_NVS_CORRUPT_KEY_PART` 错误代码，该代码假设 ``nvs_keys`` 分区不为空并且包含格式错误的数据。可以使用以下命令来实现：
        ::

            parttool.py --port PORT --partition-table-file=PARTITION_TABLE_FILE --partition-table-offset PARTITION_TABLE_OFFSET erase_partition --partition-type=data --partition-subtype=nvs_keys

            # 如果启用了 Flash Encryption 或 Secure Boot，需要使用 "--esptool-erase-args=force" 来抑制错误：
            # "Active security features detected, erasing flash is disabled as a safety measure.  Use --force to override ..."
            parttool.py --port PORT --esptool-erase-args=force --partition-table-file=PARTITION_TABLE_FILE --partition-table-offset PARTITION_TABLE_OFFSET erase_partition --partition-type=data --partition-subtype=nvs_keys

**使用预生成的 NVS 密钥分区**

    如果 :ref:`nvs_encr_key_partition` 中的密钥不是由应用程序生成，则需要使用预先生成的密钥分区。可以使用 :doc:`/api-reference/storage/nvs_partition_gen` 生成包含 XTS 加密密钥的 :ref:`nvs_encr_key_partition`。然后使用以下两个命令将预生成的密钥分区存储到 flash 上：

    1. 构建并烧写分区表
    ::

        idf.py partition-table partition-table-flash

    2. 使用 :component_file:`parttool.py<partition_table/parttool.py>` （参见 :doc:`/api-guides/partition-tables` 中分区工具相关章节）将密钥存储在 flash 上的 :ref:`nvs_encr_key_partition` 中
    ::

        parttool.py --port PORT --partition-table-offset PARTITION_TABLE_OFFSET write_partition --partition-name="name of nvs_key partition" --input NVS_KEY_PARTITION_FILE

        # 如果启用了 Flash Encryption 或 Secure Boot，需要使用 "--esptool-erase-args=force" 来抑制错误：
        # "Active security features detected, erasing flash is disabled as a safety measure.  Use --force to override ..."
        parttool.py --port PORT --esptool-erase-args=force --partition-table-offset PARTITION_TABLE_OFFSET write_partition --partition-name="name of nvs_key partition" --input NVS_KEY_PARTITION_FILE

    .. note::
        如果设备是在 flash 加密开发模式下加密的，那么要更新 NVS 密钥分区就需要使用 :component_file:`parttool.py <partition_table/parttool.py>` 来加密 NVS 密钥分区，并提供一个指向你构建目录中未加密分区表的指针 (build/partition_table)，因为设备上的分区表也是加密的。命令如下：
        ::

            parttool.py --esptool-write-args encrypt --port PORT --partition-table-file=PARTITION_TABLE_FILE --partition-table-offset PARTITION_TABLE_OFFSET write_partition --partition-name="nvs_key 分区名称" --input NVS_KEY_PARTITION_FILE

            # 如果启用了 Flash Encryption 或 Secure Boot，需要使用 "--esptool-erase-args=force" 来抑制错误：
            # "Active security features detected, erasing flash is disabled as a safety measure.  Use --force to override ..."
            parttool.py --esptool-erase-args=force --esptool-write-args encrypt --port PORT --partition-table-file=PARTITION_TABLE_FILE --partition-table-offset PARTITION_TABLE_OFFSET write_partition --partition-name="name of nvs_key partition" --input NVS_KEY_PARTITION_FILE

由于密钥分区被标记为 ``encrypted``，且 :doc:`../../security/flash-encryption` 已启用，引导程序会在首次启动时使用 flash 加密密钥对此分区进行加密。

一个应用程序可以使用不同的密钥对不同的 NVS 分区进行加密，从而拥有多个密钥分区。应用程序应为加密或解密操作提供正确的密钥分区和密钥信息。

.. only:: SOC_HMAC_SUPPORTED

    NVS 加密：基于 HMAC 外设的方案
    --------------------------------------------

    此方案中，用于 NVS 加密的 XTS 密钥来自 eFuse 中编程的 HMAC 密钥，其目的是 :cpp:enumerator:`esp_efuse_purpose_t::ESP_EFUSE_KEY_PURPOSE_HMAC_UP`。由于加密密钥在运行时生成，不存储在 flash 中，因此这个功能不需要单独的 :ref:`nvs_encr_key_partition`。

    .. note::

        通过这个方案， **无需启用 flash 加密** 就能在 {IDF_TARGET_NAME} 上实现安全存储。

    .. important::

        注意，此方案使用一个 eFuse 块来存储获取加密密钥所需的 HMAC 密钥。

    - NVS 加密启用时后，可用 API 函数 :cpp:func:`nvs_flash_init` 来初始化加密的默认 NVS 分区。该 API 函数首先检查 :ref:`CONFIG_NVS_SEC_HMAC_EFUSE_KEY_ID` 处是否存在一个 HMAC 密钥。

    .. note::

        :ref:`CONFIG_NVS_SEC_HMAC_EFUSE_KEY_ID` 配置的有效范围为 ``0`` (:cpp:enumerator:`hmac_key_id_t::HMAC_KEY0`) 到 ``5`` (:cpp:enumerator:`hmac_key_id_t::HMAC_KEY5`)。默认情况下该配置为 ``6`` (:cpp:enumerator:`hmac_key_id_t::HMAC_KEY_MAX`)，须在构建用户应用程序之前进行修改。

    - 如果找不到密钥，会内部生成一个密钥，并储存在 :ref:`CONFIG_NVS_SEC_HMAC_EFUSE_KEY_ID` 指定的 eFuse 块中。
    - 如果找到用于 :cpp:enumerator:`esp_efuse_purpose_t::ESP_EFUSE_KEY_PURPOSE_HMAC_UP` 的密钥，该密钥也会用于 XTS 加密密钥的生成。
    - 如果指定的 eFuse 块被 :cpp:enumerator:`esp_efuse_purpose_t::ESP_EFUSE_KEY_PURPOSE_HMAC_UP` 以外目的的密钥占用，则会引发错误。

    - 然后，API :cpp:func:`nvs_flash_init` 使用 :component_file:`nvs_flash/include/nvs_flash.h` 提供的 :cpp:func:`nvs_flash_generate_keys_v2` API 函数，自动生成所需的 NVS 密钥。该密钥还可用于读取安全配置（参见 :cpp:func:`nvs_flash_read_security_cfg_v2`）并通过 :cpp:func:`nvs_flash_secure_init_partition` 初始化自定义的加密 NVS 分区。

    - API 函数 :cpp:func:`nvs_flash_secure_init` 和 :cpp:func:`nvs_flash_secure_init_partition` 不会内部生成密钥。使用这些 API 函数初始化加密的 NVS 分区时，可在启动后用 API 函数 :cpp:func:`nvs_flash_generate_keys_v2` 生成密钥，或使用 :cpp:func:`nvs_flash_read_security_cfg_v2` 获取并填充 NVS 安全配置结构 :cpp:type:`nvs_sec_cfg_t`，将其输入到上述 API 中。

    .. note:: 可以使用以下命令预先在 eFuse 中设置自己的 HMAC 密钥：
        ::

            idf.py -p PORT efuse-burn-key <BLOCK_KEYN> <hmac_key_file.bin> HMAC_UP

加密读/写
--------------------

NVS API 函数 ``nvs_get_*`` 或 ``nvs_set_*`` 也可用于读取和写入加密的 NVS 分区。

**加密默认的 NVS 分区**

- 要为默认 NVS 分区启用加密，无需额外的步骤。在启用 :ref:`CONFIG_NVS_ENCRYPTION` 时，API 函数 :cpp:func:`nvs_flash_init` 会根据使用的方案（由 :ref:`CONFIG_NVS_SEC_KEY_PROTECTION_SCHEME` 设置）在内部执行一些额外步骤，为默认的 NVS 分区启用加密。

- 在基于 flash 加密的方案中，加密密钥由找到的第一个 :ref:`nvs_encr_key_partition` 生成。在 HMAC 方案中，密钥由 :ref:`CONFIG_NVS_SEC_HMAC_EFUSE_KEY_ID` 中烧录的 HMAC 密钥生成（参考 API 文档以了解更多详细信息）。

另外，还可使用 API 函数 :cpp:func:`nvs_flash_secure_init` 为默认 NVS 分区启用加密。

**加密自定义 NVS 分区**

- 要为一个自定义的 NVS 分区启用加密，使用 API 函数 :cpp:func:`nvs_flash_secure_init_partition` 代替 :cpp:func:`nvs_flash_init_partition`。

- 使用 API 函数 :cpp:func:`nvs_flash_secure_init` 和 :cpp:func:`nvs_flash_secure_init_partition` 时，为了在启用加密的情况下执行 NVS 读/写操作，应用程序应遵守以下步骤：

    1. 填充 NVS 安全配置结构 :cpp:type:`nvs_sec_cfg_t`

        * 对基于 flash 加密的方案

            - 使用 API 函数 `esp_partition_find*` 查找密钥分区和 NVS 数据分区。
            - 使用 API 函数 :cpp:func:`nvs_flash_read_security_cfg` 或 :cpp:func:`nvs_flash_generate_keys` 填充 :cpp:type:`nvs_sec_cfg_t` 结构体。

        .. only:: SOC_HMAC_SUPPORTED

            * 对基于 HMAC 的方案

                - 用 :cpp:type:`nvs_sec_config_hmac_t` 为设置特定方案配置数据，并使用 API :cpp:func:`nvs_sec_provider_register_hmac` 注册此基于 HMAC 的方案。该 API 也将用于填充特定方案的句柄（参见 :cpp:type:`nvs_sec_scheme_t`）。
                - 使用 API 函数 :cpp:func:`nvs_flash_read_security_cfg_v2` 或 :cpp:func:`nvs_flash_generate_keys_v2` 填充 :cpp:type:`nvs_sec_cfg_t` 结构体。

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

    2. 使用 API 函数 :cpp:func:`nvs_flash_secure_init` 或 :cpp:func:`nvs_flash_secure_init_partition` 初始化 NVS flash 分区。
    3. 使用 API 函数 :cpp:func:`nvs_open` 或 :cpp:func:`nvs_open_from_partition` 打开一个命名空间。
    4. 使用 ``nvs_get_*`` 或 ``nvs_set_*`` 执行 NVS 读/写操作。
    5. 使用 :cpp:func:`nvs_flash_deinit` 取消初始化 NVS 分区。

.. only:: SOC_HMAC_SUPPORTED

    .. note::
        在采用基于 HMAC 的方案时，可以在不启用任何 NVS 加密的配置选项的情况下开始上述工作流：:ref:`CONFIG_NVS_ENCRYPTION`，:ref:`CONFIG_NVS_SEC_KEY_PROTECTION_SCHEME` -> `CONFIG_NVS_SEC_KEY_PROTECT_USING_HMAC` 和 :ref:`CONFIG_NVS_SEC_HMAC_EFUSE_KEY_ID`，以使用 :cpp:func:`nvs_flash_secure_init` API 加密默认分区及自定义的 NVS 分区。


NVS Security Provider
---------------------

组件 :component:`nvs_sec_provider` 存储了 NVS 加密方案的所有特定实现代码，并且适用于未来的方案。此组件充当 :component:`nvs_flash` 组件处理加密密钥的接口。组件 :component:`nvs_sec_provider` 有自己的配置菜单，选定的安全方案和相应设置基于这一菜单注册到 :component:`nvs_flash` 组件。

.. only:: SOC_HMAC_SUPPORTED

    该组件通过工厂函数注册了特殊的安全框架，可以实现出厂即用的安全方案。在该方案中，无需使用 API 来生成、读取加密密钥（如 :cpp:func:`nvs_sec_provider_register_hmac`）。要了解 API 的使用，参考示例 :example:`security/nvs_encryption_hmac`。


API 参考
-------------

.. include-build-file:: inc/nvs_sec_provider.inc
