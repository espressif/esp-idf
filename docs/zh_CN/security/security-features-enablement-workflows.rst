..
  WARNING: The steps of each section in the document are referenced at multiple places. If you are changing the step number by adding/deleting a step then make sure to update the references respectively.

启用安全功能的工作流程
======================

:link_to_translation:`en:[English]`

{IDF_TARGET_CRYPT_CNT:default="SPI_BOOT_CRYPT_CNT",esp32="FLASH_CRYPT_CNT"}
{IDF_TARGET_CRYPT_CNT_MAX_VAL:default="7",esp32="127"}
{IDF_TARGET_SBV2_DEFAULT_SCHEME:default="RSA", esp32c2="ECDSA (V2)"}
{IDF_TARGET_FLASH_ENC_ARGS:default="--aes_xts", esp32=""}

概述
----

启用 ESP32 SoC 的安全功能时，建议保证不间断的电源供应。在此过程中，如果发生电力故障，可能会引起难以调试的问题，甚至在某些情况下可能导致永久性启动故障。

这份指南介绍了一系列工作流程，从而在外部主机的协助下启用设备的安全功能。这些工作流程分为多个阶段，每个阶段都会在主机上生成签名/加密密钥，从而在发生电力或其他故障时，提高恢复几率。此外，在主机的协助下，这些流程将加快整体配置过程（例如，在主机上加密固件要比在设备上加密更快）。


目标
----

#. 用逐步指令简化启用安全功能的传统工作流程。
#. 设计比基于固件的传统工作流更加灵活的工作流。
#. 将工作流划分为多个小操作，从而提高可靠性。
#. 消除对 :ref:`second-stage-bootloader` （固件引导加载程序）的依赖。

准备工作
--------

* ``esptool``：确保已安装 ``esptool``。可运行以下命令安装：

.. code:: bash

    pip install esptool

目录
----

* :ref:`enable-flash-encryption-and-secure-boot-v2-externally`
* :ref:`enable-flash-encryption-externally`
* :ref:`enable-secure-boot-v2-externally`
* :ref:`enable-nvs-encryption-externally`

启用安全功能
------------

.. _enable-flash-encryption-and-secure-boot-v2-externally:

外部启用 flash 加密和 Secure Boot v2
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. important::

    建议在生产用例中同时启用 flash 加密和 Secure Boot v2。

外部启用 flash 加密和 Secure Boot v2 时，须遵循以下启用顺序：

#. 按照 :ref:`enable-flash-encryption-externally` 中列出的步骤启用 flash 加密功能。
#. 按照 :ref:`enable-secure-boot-v2-externally` 中列出的步骤启用 Secure Boot v2 功能。

须严格遵循以上顺序，因为启用 Secure Boot (SB) v2 时，要确保 SB v2 密钥可读。通过启用 ``RD_DIS`` (``ESP_EFUSE_WR_DIS_RD_DIS``) 的写保护，确保了密钥的可读性。但是，这也为启用 flash 加密带来了困难，因为 flash 加密 (FE) 密钥须保持不可读状态。产生这种冲突的原因是 ``RD_DIS`` 已受到写保护，因此无法对 FE 密钥进行读保护。

.. _enable-flash-encryption-externally:

外部启用 flash 加密
^^^^^^^^^^^^^^^^^^^

在这种情况下，所有与 flash 加密相关的 eFuse 都是借助 espefuse 工具写入的。关于 flash 加密过程的详细信息，请参阅 :doc:`/security/flash-encryption`。

1. 确保有一块 {IDF_TARGET_NAME}，其默认 flash 加密 eFuse 设置如 :ref:`flash-encryption-efuse` 所示

    参考 :ref:`flash-encryption-status`，查看 flash 加密状态。

    此时需要擦除芯片上的 flash，且 flash 加密必须尚未启用。请运行以下命令进行擦除：

    .. code:: bash

        esptool.py --port PORT erase_flash

2. 生成一个 flash 加密密钥

    运行以下命令可以生成一个随机的 flash 加密密钥：

    .. only:: not SOC_FLASH_ENCRYPTION_XTS_AES

        .. code-block:: bash

            espsecure.py generate_flash_encryption_key my_flash_encryption_key.bin

    .. only:: SOC_FLASH_ENCRYPTION_XTS_AES_256

        如果 :ref:`生成的 AES-XTS 密钥大小 <CONFIG_SECURE_FLASH_ENCRYPTION_KEYSIZE>` 为 AES-128（256 位密钥）：

        .. code-block:: bash

            espsecure.py generate_flash_encryption_key my_flash_encryption_key.bin

        如果 :ref:`生成的 AES-XTS 密钥的大小 <CONFIG_SECURE_FLASH_ENCRYPTION_KEYSIZE>` 为 AES-256（512 位密钥）：

        .. code-block:: bash

            espsecure.py generate_flash_encryption_key --keylen 512 my_flash_encryption_key.bin


    .. only:: SOC_FLASH_ENCRYPTION_XTS_AES_128 and not SOC_FLASH_ENCRYPTION_XTS_AES_256 and not SOC_EFUSE_CONSISTS_OF_ONE_KEY_BLOCK

        .. code-block:: bash

            espsecure.py generate_flash_encryption_key my_flash_encryption_key.bin

    .. only:: SOC_FLASH_ENCRYPTION_XTS_AES_128 and SOC_EFUSE_CONSISTS_OF_ONE_KEY_BLOCK

        如果 :ref:` 生成的 AES-XTS 密钥的大小 <CONFIG_SECURE_FLASH_ENCRYPTION_KEYSIZE>` 为 AES-128（256 位密钥）：

        .. code-block:: bash

            espsecure.py generate_flash_encryption_key my_flash_encryption_key.bin

        如果 :ref:`生成的 AES-XTS 密钥的大小 <CONFIG_SECURE_FLASH_ENCRYPTION_KEYSIZE>` 是从 128 位（SHA256（128 位））派生的 AES-128 密钥：

        .. code-block:: bash

            espsecure.py generate_flash_encryption_key --keylen 128 my_flash_encryption_key.bin

3. 将 flash 加密密钥烧录到 eFuse 中

    .. warning::

        这个操作 **无法回退**。

    运行以下命令进行烧录：

    .. only:: not SOC_FLASH_ENCRYPTION_XTS_AES

        .. code-block:: bash

            espefuse.py --port PORT burn_key flash_encryption my_flash_encryption_key.bin

    .. only:: SOC_FLASH_ENCRYPTION_XTS_AES_256

        .. code-block:: bash

            espefuse.py --port PORT burn_key BLOCK my_flash_encryption_key.bin KEYPURPOSE

        其中， ``BLOCK`` 是位于 ``BLOCK_KEY0`` 和 ``BLOCK_KEY5`` 之间的空闲密钥块， ``KEYPURPOSE`` 是 ``XTS_AES_256_KEY_1``， ``XTS_AES_256_KEY_2`` 或 ``XTS_AES_128_KEY``。有关密钥用途的说明，请参阅 `{IDF_TARGET_NAME} 技术参考手册 <{IDF_TARGET_TRM_EN_URL}>`__。

        对于 AES-128（256 位密钥）- ``XTS_AES_128_KEY``：

        .. code-block:: bash

            espefuse.py --port PORT burn_key BLOCK my_flash_encryption_key.bin XTS_AES_128_KEY

        对于 AES-256（512 位密钥）- ``XTS_AES_256_KEY_1`` 和 ``XTS_AES_256_KEY_2``。 ``espefuse.py`` 支持通过虚拟密钥用途 ``XTS_AES_256_KEY`` 将这两个密钥用途和一个 512 位密钥一起烧录到两个单独的密钥块中。使用时， ``espefuse.py`` 会把密钥的前 256 位烧录到指定的 ``BLOCK``，并把相应块的密钥用途烧录为 ``XTS_AES_256_KEY_1``。密钥的后 256 位会被烧录到 ``BLOCK`` 后的第一个空闲密钥块，相应块的密钥用途会烧录为 ``XTS_AES_256_KEY_2``。

        .. code-block:: bash

            espefuse.py --port PORT burn_key BLOCK my_flash_encryption_key.bin XTS_AES_256_KEY

        如果要指定使用两个块，那么可以将密钥分成两个 256 位密钥并手动烧录，以 ``XTS_AES_256_KEY_1`` 和 ``XTS_AES_256_KEY_2`` 作为密钥用途：

        .. code-block:: bash

            split -b 32 my_flash_encryption_key.bin my_flash_encryption_key.bin
            espefuse.py --port PORT burn_key BLOCK my_flash_encryption_key.bin.aa XTS_AES_256_KEY_1
            espefuse.py --port PORT burn_key BLOCK+1 my_flash_encryption_key.bin.ab XTS_AES_256_KEY_2


    .. only:: SOC_FLASH_ENCRYPTION_XTS_AES_128 and not SOC_FLASH_ENCRYPTION_XTS_AES_256 and not SOC_EFUSE_CONSISTS_OF_ONE_KEY_BLOCK

        .. code-block:: bash

            espefuse.py --port PORT burn_key BLOCK my_flash_encryption_key.bin XTS_AES_128_KEY

        其中， ``BLOCK`` 是 ``BLOCK_KEY0`` 和 ``BLOCK_KEY5`` 之间的空闲密钥块。

    .. only:: SOC_FLASH_ENCRYPTION_XTS_AES_128 and SOC_EFUSE_CONSISTS_OF_ONE_KEY_BLOCK

        对于 AES-128 (256 位密钥) - ``XTS_AES_128_KEY`` (``XTS_KEY_LENGTH_256`` eFuse 会被烧录为 1)：

        .. code-block:: bash

            espefuse.py --port PORT burn_key BLOCK_KEY0 flash_encryption_key256.bin XTS_AES_128_KEY

        对于从 SHA256（128 eFuse 位）派生的 AES-128 密钥 - ``XTS_AES_128_KEY_DERIVED_FROM_128_EFUSE_BITS``。FE 密钥会被写入 eFuse BLOCK_KEY0 的后半部分。前 128 位不会被使用，并保持可供软件读取状态。使用 espefuse 工具的特殊模式，可以用任何 espefuse 命令将数据写入其中，可参考下文 ``同时烧录两个密钥``。

        .. code-block:: bash

            espefuse.py --port PORT burn_key BLOCK_KEY0 flash_encryption_key128.bin XTS_AES_128_KEY_DERIVED_FROM_128_EFUSE_BITS

        同时烧录两个密钥（Secure Boot 和 flash 加密）：

        .. code-block:: bash

            espefuse.py --port PORT --chip esp32c2 burn_key_digest secure_boot_signing_key.pem \
                                                    burn_key BLOCK_KEY0 flash_encryption_key128.bin XTS_AES_128_KEY_DERIVED_FROM_128_EFUSE_BITS


    .. only:: SOC_EFUSE_BLOCK9_KEY_PURPOSE_QUIRK

        .. warning::

            对于 {IDF_TARGET_NAME}，XTS_AES 密钥不能使用 BLOCK9 (BLOCK_KEY5)。


4. 烧录 ``{IDF_TARGET_CRYPT_CNT}`` eFuse

    如果你只想在 **开发** 模式下启用 flash 加密，并在将来可能会禁用 flash 加密，可将下面命令中的 {IDF_TARGET_CRYPT_CNT} 值从 {IDF_TARGET_CRYPT_CNT_MAX_VAL} 更新为 0x1。（不推荐在生产中使用）

    .. code-block:: bash

        espefuse.py --port PORT --chip {IDF_TARGET_PATH_NAME} burn_efuse {IDF_TARGET_CRYPT_CNT} {IDF_TARGET_CRYPT_CNT_MAX_VAL}

    .. only:: esp32

        在使用 {IDF_TARGET_NAME} 时，还要烧录 ``FLASH_CRYPT_CONFIG``。通过运行以下命令进行烧录：

        .. code-block:: bash

            espefuse.py --port PORT --chip {IDF_TARGET_PATH_NAME} burn_efuse FLASH_CRYPT_CONFIG 0xF

5. 烧录下列与 flash 加密相关的安全 eFuse

    A) 烧录安全 eFuse

    .. important::

        对于生产用例，强烈建议烧录下列所有的 eFuse。

    .. list::

        :esp32: - ``DISABLE_DL_ENCRYPT``：禁用 UART 引导加载程序加密访问。
        :esp32: - ``DISABLE_DL_DECRYPT``：禁用 UART 引导加载程序解密访问。
        :esp32: - ``DISABLE_DL_CACHE``：禁用 UART 引导加载程序 flash cache 访问
        :esp32: - ``JTAG_DISABLE``：禁用 JTAG
        :SOC_EFUSE_DIS_BOOT_REMAP: - ``DIS_BOOT_REMAP``：禁用将 ROM 映射到 RAM 地址空间的功能
        :SOC_EFUSE_DIS_DOWNLOAD_ICACHE: - ``DIS_DOWNLOAD_ICACHE``：禁用 UART cache
        :SOC_EFUSE_DIS_DOWNLOAD_DCACHE: - ``DIS_DOWNLOAD_DCACHE``：禁用 UART cache
        :SOC_EFUSE_HARD_DIS_JTAG: - ``HARD_DIS_JTAG``：硬禁用 JTAG 外设
        :SOC_EFUSE_DIS_DIRECT_BOOT: - ``DIS_DIRECT_BOOT``：禁用直接引导（旧版 SPI 引导模式）
        :SOC_EFUSE_DIS_LEGACY_SPI_BOOT: - ``DIS_LEGACY_SPI_BOOT``：禁用旧版 SPI 引导模式
        :SOC_EFUSE_DIS_USB_JTAG: - ``DIS_USB_JTAG``：禁止从 USB 切换到 JTAG
        :SOC_EFUSE_DIS_PAD_JTAG: - ``DIS_PAD_JTAG``：永久禁用 JTAG
        :not esp32: - ``DIS_DOWNLOAD_MANUAL_ENCRYPT``：禁用 UART 引导加载程序加密访问
        :SOC_EFUSE_DIS_DOWNLOAD_MSPI: - ``DIS_DOWNLOAD_MSPI``：禁用下载模式下的 MSPI 访问

    可运行以下命令烧录相应的 eFuse：

    .. code:: bash

        espefuse.py burn_efuse --port PORT EFUSE_NAME 0x1

    .. note::

        请将 ``EFUSE_NAME`` 更新为需要烧录的 eFuse。可以在上述命令中添加多个 efuse 同时进行烧录（例如：``EFUSE_NAME VAL EFUSE_NAME2 VAL2``）。有关 `espefuse.py` 的更多信息，请参阅 `此文档 <https://docs.espressif.com/projects/esptool/en/latest/esp32/espefuse/index.html>`__。

    .. only:: esp32

        B) 对安全 eFuse 采用写保护

        在烧录相应 eFuse 后，需要对安全配置进行 write_protect。请烧录下列 eFuse：

        .. code:: bash

            espefuse.py --port PORT write_protect_efuse DIS_CACHE

        .. note::

            以上 eFuse 的写保护还对其他多个 eFuse 起效。详情请参阅 {IDF_TARGET_NAME} eFuse 表。

    .. only:: SOC_EFUSE_DIS_ICACHE

        B) 对安全 eFuse 采用写保护

        在烧录相应 eFuse 后，需要对安全配置进行 write_protect。请烧录下列 eFuse：

        .. code:: bash

            espefuse.py --port PORT write_protect_efuse DIS_ICACHE

        .. note::

            以上 eFuse 的写保护还对其他多个 eFuse 起效。详情请参阅 {IDF_TARGET_NAME} eFuse 表。

6. 配置项目

    项目的引导加载程序和应用程序二进制文件必须使用默认配置的 flash 加密发布模式进行构建。

    如下所示，可以在 menuconfig 中设置 flash 加密发布模式：

    .. list::

        - :ref:`启动时启用 flash 加密 <CONFIG_SECURE_FLASH_ENC_ENABLED>`。
        :esp32: - :ref:`选择发布模式 <CONFIG_SECURE_FLASH_ENCRYPTION_MODE>` （注意，若选择发布模式，则将烧录 ``DISABLE_DL_ENCRYPT`` 和 ``DISABLE_DL_DECRYPT`` eFuse 位，ROM 下载模式下 flash 加密硬件将被禁用）。
        :esp32: - :ref:`选择 UART ROM 下载模式（永久禁用（推荐））<CONFIG_SECURE_UART_ROM_DL_MODE>` （注意，此选项仅在 :ref:`CONFIG_ESP32_REV_MIN` 设为 3 (ESP32 V3) 时可用）。UART ROM 下载模式在默认设置中自动启用，但建议永久禁用此模式以减少攻击者可用的选项。
        :not esp32: - :ref:`选择发布模式 <CONFIG_SECURE_FLASH_ENCRYPTION_MODE>` （注意，若选择发布模式，则将烧录 ``EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT`` eFuse 位，ROM 下载模式下 flash 加密硬件将被禁用）。
        :not esp32: - :ref:`选择 UART ROM 下载模式（永久切换到安全模式（推荐））<CONFIG_SECURE_UART_ROM_DL_MODE>`。这是推荐的默认选项，如果不需要，也可将其更改为永久禁用 UART ROM 下载模式。
        - :ref:`选择适当的引导程序日志级别 <CONFIG_BOOTLOADER_LOG_LEVEL>`。
        - 保存配置并退出。

7. 构建、加密并烧录二进制文件

    可以在主机上运行下列命令来加密二进制文件：

    .. code-block:: bash

        espsecure.py encrypt_flash_data {IDF_TARGET_FLASH_ENC_ARGS} --keyfile my_flash_encryption_key.bin --address {IDF_TARGET_CONFIG_BOOTLOADER_OFFSET_IN_FLASH} --output bootloader-enc.bin build/bootloader/bootloader.bin

        espsecure.py encrypt_flash_data {IDF_TARGET_FLASH_ENC_ARGS} --keyfile my_flash_encryption_key.bin --address 0x8000 --output partition-table-enc.bin build/partition_table/partition-table.bin

        espsecure.py encrypt_flash_data {IDF_TARGET_FLASH_ENC_ARGS} --keyfile my_flash_encryption_key.bin --address 0x10000 --output my-app-enc.bin build/my-app.bin

    上述命令中的偏移量仅适用于示例固件，请通过检查分区表条目或运行 `idf.py partition-table` 来获取你固件的实际偏移量。请注意，不需要加密所有二进制文件，只需加密在分区表定义文件中带有 ``encrypted`` 标记的文件，其他二进制文件只作为构建过程的普通输出进行烧录。

    使用 ``esptool.py`` 可以将上述文件烧写到各自的偏移地址。要查看所有推荐的 ``esptool.py`` 命令行选项，请查阅 ``idf.py build`` 构建成功后打印的输出。

    若应用程序包含分区 ``otadata`` 和 ``nvs_encryption_keys``，则该分区也需加密。详情请参阅 :ref:`encrypted-partitions`。

    .. note::

        如果 {IDF_TARGET_NAME} 启动时无法识别烧录的密文，请检查密钥是否匹配、命令行参数是否精确匹配及偏移量的正确性。偏移量必须正确，因为当偏移量改变时，密文也会改变。

        .. only:: esp32

            如果 ESP32 在 eFuse 中使用非默认的 :ref:`FLASH_CRYPT_CONFIG 值 <setting-flash-crypt-config>`，需要将 ``--flash_crypt_conf`` 参数传递给 ``espsecure.py`` 以设置匹配值。如果设备自行配置 flash 加密，则不会发生这种情况，但是如果手动烧录了 eFuses 启用 flash 加密，就有可能发生。

    使用 ``espsecure.py decrypt_flash_data`` 命令时，可以用相同的选项（和不同的输入或输出文件）来解密密文 flash 或之前加密的文件。

8. 确保 ROM 下载模式安全

    .. warning::

        请在最后烧录以下位。烧录后，espefuse 工具将无法再用于烧录其他 eFuse。

    .. only:: esp32

        禁用 UART ROM DL 模式：

            .. list::

                - ``UART_DOWNLOAD_DIS``：禁用 UART ROM 下载模式

                运行以下指令，烧录 eFuse：

                .. code:: bash

                    espefuse.py --port PORT burn_efuse UART_DOWNLOAD_DIS

    .. only:: not esp32

        启用安全下载模式：

            .. list::

                - ``ENABLE_SECURITY_DOWNLOAD``：启用安全 ROM 下载模式

                运行以下指令，烧录 eFuse：

                .. code:: bash

                    espefuse.py --port PORT burn_efuse ENABLE_SECURITY_DOWNLOAD

.. important::

    9. 从主机上删除 flash 加密密钥

        一旦为设备启用了 flash 加密，密钥 **必须立即删除**。这能确保主机以后不为同一设备生成加密二进制文件，从而减少 flash 加密密钥漏洞。

flash 加密指南
~~~~~~~~~~~~~~

* 建议为每个设备生成唯一的 flash 加密密钥用于生产用例。
* 确保主机用于生成 flash 加密密钥的 RNG 具有良好的熵。
* 更多详细信息请参阅 :ref:`flash-encryption-limitations`。

.. _enable-secure-boot-v2-externally:

外部启用 Secure Boot v2
^^^^^^^^^^^^^^^^^^^^^^^

在此工作流中，我们会使用 ``espsecure`` 工具生成签名密钥，并使用 ``espefuse`` 工具烧录相关 eFuse。关于 Secure Boot v2 流程的详细信息，请参阅 :doc:`/security/secure-boot-v2`。

1. 生成 Secure Boot v2 签名私钥

    .. only:: esp32 or SOC_SECURE_BOOT_V2_RSA

        运行以下命令可以生成 RSA3072 方案的 Secure Boot v2 签名密钥：

        .. code:: bash

            espsecure.py generate_signing_key --version 2 --scheme rsa3072 secure_boot_signinig_key.pem

    .. only:: SOC_SECURE_BOOT_V2_ECC

        运行以下命令可以生成 ECDSA 方案的 Secure Boot v2 签名密钥：

        .. code:: bash

            bashespsecure.py generate_signing_key --version 2 --scheme ecdsa256 secure_boot_signing_key.pem

        将上述命令中的方案更改为 ``ecdsa192``，可生成 ecdsa192 私钥。

    .. only:: SOC_EFUSE_REVOKE_BOOT_KEY_DIGESTS

        每次可以在 Secure Boot v2 中使用 3 个密钥。这些密钥应独立计算，分开存储。同一个命令也可以使用不同的密钥文件名，生成多个 Secure Boot v2 签名密钥。建议使用多个密钥，以降低对单个密钥的依赖。

2. 生成公钥摘要

    运行以下命令可以为上一步生成的私钥生成公钥摘要：

    .. code:: bash

        espsecure.py digest_sbv2_public_key --keyfile secure_boot_signing_key.pem --output digest.bin

    .. only:: SOC_EFUSE_REVOKE_BOOT_KEY_DIGESTS

        如果有多个摘要，应将每个摘要保存在一个单独的文件中。

3. 在 eFuse 中烧录密钥摘要

    运行以下命令可以在 eFuse 中烧录公钥摘要：

    .. only:: esp32

        .. code:: bash

            espefuse.py --port PORT --chip esp32 burn_key secure_boot_v2 digest.bin

    .. only:: esp32c2

        .. code:: bash

            espefuse.py --port PORT --chip esp32c2 burn_key KEY_BLOCK0 digest.bin SECURE_BOOT_DIGEST

    .. only:: SOC_EFUSE_REVOKE_BOOT_KEY_DIGESTS

        .. code:: bash

            espefuse.py --port PORT --chip {IDF_TARGET_PATH_NAME} burn_key BLOCK digest.bin SECURE_BOOT_DIGEST0

        其中，``BLOCK`` 是 ``BLOCK_KEY0`` 和 ``BLOCK_KEY5`` 之间的一个空闲密钥块。

        如果有多个摘要，可以将密钥用途分别更改为 ``SECURE_BOOT_DIGEST1`` 和 ``SECURE_BOOT_DIGEST2``，从而依次烧录其他摘要。

4. 启用 Secure Boot v2

    运行以下命令启用 Secure Boot v2 eFuse：

    .. only:: esp32

        .. code:: bash

            espefuse.py --port PORT --chip esp32 burn_efuse ABS_DONE_1

    .. only:: not esp32

        .. code:: bash

            espefuse.py --port PORT --chip {IDF_TARGET_PATH_NAME} burn_efuse SECURE_BOOT_EN

5. 烧录相关 eFuse

    A) 烧录安全 eFuse

    .. important::

        对于生产用例，强烈建议烧录下列所有 eFuse。

    .. list::

        :esp32: - ``JTAG_DISABLE``：禁用 JTAG。
        :SOC_EFUSE_DIS_BOOT_REMAP: - ``DIS_BOOT_REMAP``：禁用将 ROM 重新映射到 RAM 地址空间的功能。
        :SOC_EFUSE_HARD_DIS_JTAG: - ``HARD_DIS_JTAG``：硬禁用 JTAG 外设。
        :SOC_EFUSE_SOFT_DIS_JTAG: - ``SOFT_DIS_JTAG``：禁止软件对 JTAG 外设的访问。
        :SOC_EFUSE_DIS_DIRECT_BOOT:- ``DIS_DIRECT_BOOT``: 禁用直接引导（旧版 SPI 引导模式）。
        :SOC_EFUSE_DIS_LEGACY_SPI_BOOT: - ``DIS_LEGACY_SPI_BOOT``：禁用旧版 SPI 引导模式。
        :SOC_EFUSE_DIS_USB_JTAG: - ``DIS_USB_JTAG``：禁止从 USB 切换到 JTAG
        :SOC_EFUSE_DIS_PAD_JTAG: - ``DIS_PAD_JTAG``：永久禁用 JTAG。
        :SOC_EFUSE_REVOKE_BOOT_KEY_DIGESTS: - ``SECURE_BOOT_AGGRESSIVE_REVOKE``：主动吊销密钥摘要。详请请参阅 :ref:`secure-boot-v2-aggressive-key-revocation`。

    运行以下命令烧录相应的 eFuse：

    .. code:: bash

        espefuse.py burn_efuse --port PORT EFUSE_NAME 0x1

    .. note::

        请将 EFUSE_NAME 更新为需烧录的 eFuse。在上述命令中添加多个 eFuse 可以同时烧录（例如：EFUSE_NAME VAL EFUSE_NAME2 VAL2）。有关 `espefuse.py` 的更多信息，请参阅 `此文档 <https://docs.espressif.com/projects/esptool/en/latest/esp32/espefuse/index.html>`__。

    B) 与 Secure Boot v2 相关的 eFuse

    i) 禁用读保护选项：

    在 eFuse 中烧录的 Secure Boot 摘要必须保持可读，否则会导致安全启动失败。烧录以下 eFuse 可防止意外启用此密钥块的读保护：

    .. code:: bash

        espefuse.py -p $ESPPORT write_protect_efuse RD_DIS

    .. important::

        烧录此 eFuse 后，不能为任何密钥启用读保护。例如，如果此时需要对密钥进行读保护的 flash 加密尚未启用，则之后也无法启用。请确保在此之后没有其他 efuse 密钥需要读保护。

    .. only:: SOC_EFUSE_REVOKE_BOOT_KEY_DIGESTS

        ii) 吊销密钥摘要：

        在我们烧录 Secure Boot 密钥时，需要吊销未使用的摘要槽。可以通过运行以下命令吊销相应的槽：

        .. code:: bash

            espefuse.py --port PORT --chip {IDF_TARGET_PATH_NAME} burn_efuse EFUSE_REVOKE_BIT

        上述命令中的 ``EFUSE_REVOKE_BIT`` 可以是 ``SECURE_BOOT_KEY_REVOKE0`` 或 ``SECURE_BOOT_KEY_REVOKE1`` 或 ``SECURE_BOOT_KEY_REVOKE2``。注意，只有未使用的密钥摘要必须吊销。一旦吊销，相应的摘要就不能再次使用。

6. 构建二进制文件

    默认情况下，ROM 引导加载程序只会验证 :ref:`second-stage-bootloader` （固件引导加载程序）。只有在启用 :ref:`CONFIG_SECURE_BOOT` 选项（并将 :ref:`CONFIG_SECURE_BOOT_VERSION` 设置为 ``SECURE_BOOT_V2_ENABLED``）时，固件引导加载程序才会在构建引导加载程序时验证应用程序分区。

    A) 打开 :ref:`project-configuration-menu`，在 ``Security features`` 中设置 ``Enable hardware Secure Boot in bootloader`` 启用 Secure Boot。

    .. only:: esp32

        对于 ESP32，Secure Boot v2 仅适用于 ESP32 ECO3 及以上版本。要查看 ``Secure Boot v2`` 选项，芯片版本应更改为 v3.0 (ECO3)。要更改芯片版本，请在 ``Component Config -> Hardware Settings -> Chip Revision`` 中将 ``Minimum Supported ESP32 Revision`` 设置为 ``Rev 3.0 (ECO3)``。

    .. only:: SOC_SECURE_BOOT_V2_RSA or SOC_SECURE_BOOT_V2_ECC

        选中 ``Secure Boot v2`` 选项， ``App Signing Scheme`` 将被默认设置为 {IDF_TARGET_SBV2_DEFAULT_SCHEME}。

    B) 在 :ref:`project-configuration-menu` 中为项目禁用 :ref:`CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES` 选项，以确保所有生成的二进制文件都受到安全保护且未签名，避免生成签名的二进制文件，因为需要使用 ``espsecure`` 工具手动签名二进制文件。

7. 构建、签名并烧录二进制文件

    完成上述配置后，可以用 ``idf.py build`` 命令构建引导加载程序和应用程序二进制文件。

    Secure Boot v2 工作流程只验证 ``bootloader`` 和 ``application`` 二进制文件，因此只需要对这些二进制文件进行签名。其他二进制文件（例如 ``partition-table.bin``）可以在构建后直接进行烧录。

    运行以下命令对 ``bootloader.bin`` 和 ``app.bin`` 二进制文件进行签名：

    .. code:: bash

        espsecure.py sign_data --version 2 --keyfile secure_boot_signing_key.pem --output bootloader-signed.bin build/bootloader/bootloader.bin

        espsecure.py sign_data --version 2 --keyfile secure_boot_signing_key.pem --output my-app-signed.bin build/my-app.bin

    .. only:: SOC_EFUSE_REVOKE_BOOT_KEY_DIGESTS

        如要使用多个安全引导密钥，可在同一个已签名二进制文件中添加用新密钥签名的块，如下所示：

        .. code:: bash

            espsecure.py sign_data --keyfile secure_boot_signing_key2.pem --version 2 --amend_signatures -o bootloader-signed.bin bootloader-signed.bin

            espsecure.py sign_data --keyfile secure_boot_signing_key2.pem --version 2 --apend_signatures -o my-app-signed.bin my-app-signed.bin

        如果有第三个密钥，则可以重复以上过程。注意：输入和输出文件不能用相同名字来命名。

    运行以下命令来检查附加到二进制文件的签名：

    .. code:: bash

        espsecure.py signature_info_v2 bootloader-signed.bin

    然后使用 ``esptool.py`` 将上述文件和其他二进制文件（如分区表）烧录到各自的偏移地址。要查看所有推荐的 ``esptool.py`` 命令行选项，请参阅 ``idf.py build`` 的输出结果。要获得固件的 flash 偏移地址，可查找分区表条目或运行 ``idf.py partition-table`` 查看。

8. 确保 ROM 下载模式安全

    .. warning::

        请在最后烧录以下位。烧录后，espefuse 工具将无法再用于烧录其他 eFuse。

    .. only:: esp32

        禁用 UART ROM DL 模式：

        .. list::

            - ``UART_DOWNLOAD_DIS``：禁用 UART ROM 下载模式

            运行以下指令，烧录 eFuse：

            .. code:: bash

                espefuse.py --port PORT burn_efuse UART_DOWNLOAD_DIS

    .. only:: not esp32

        启用安全下载模式：

        .. list::

            - ``ENABLE_SECURITY_DOWNLOAD``：启用安全 ROM 下载模式

            运行以下指令，烧录 eFuse：

            .. code:: bash

                espefuse.py --port PORT burn_efuse ENABLE_SECURITY_DOWNLOAD

Secure Boot v2 指南
~~~~~~~~~~~~~~~~~~~

* 建议将 Secure Boot 密钥存储在高度安全的地方，如可以使用物理或云 HSM 来存储 Secure Boot 私钥。请参阅 :ref:`remote-sign-v2-image` 获取更多详细信息。

.. only:: SOC_EFUSE_REVOKE_BOOT_KEY_DIGESTS

    * 建议使用所有可用的摘要槽，降低对单个私钥的依赖。

.. _enable-nvs-encryption-externally:

启用外部 NVS 加密
^^^^^^^^^^^^^^^^^

有关 NVS 加密及相关方案的详细信息，请参阅 :doc:`NVS 加密 </api-reference/storage/nvs_encryption>`。

.. only:: SOC_HMAC_SUPPORTED

    .. _enable-nvs-encryption-based-on-hmac:

    基于 HMAC 启用 NVS 加密
    ~~~~~~~~~~~~~~~~~~~~~~~

    1. 生成 HMAC 密钥和 NVS 加密密钥

        在基于 HMAC 的 NVS 加密方案中，有两个密钥：

        * HMAC 密钥 - 256 位的 HMAC 密钥，应存储在 eFuse 中。
        * NVS 加密密钥 - 用于加密 NVS 分区，在命令运行时通过 HMAC 密钥派生。

        使用以下命令，通过 :component_file:`nvs_flash/nvs_partition_generator/nvs_partition_gen.py` 脚本可以生成上述密钥：

        .. code:: bash

            python3 nvs_partition_gen.py generate-key --key_protect_hmac --kp_hmac_keygen --kp_hmac_keyfile hmac_key.bin --keyfile nvs_encr_key.bin

        运行上述命令后，``keys`` 文件夹下会生成相应的密钥。

    2. 在 eFuse 中烧录 HMAC 密钥

        使用以下命令在 {IDF_TARGET_NAME} 的 eFuse 中烧录 NVS 密钥：

        .. code:: bash

            espefuse.py --port PORT burn_key BLOCK hmac_key.bin HMAC_UP

        其中，``BLOCK`` 是 ``BLOCK_KEY0`` 和 ``BLOCK_KEY5`` 之间的一个空闲密钥块。

    3. 生成加密的 NVS 分区

        主机上将会生成加密 NVS 分区。有关生成加密 NVS 分区的详细信息，请参阅读 :ref:`generate-encrypted-nvs-partition`。为此，CSV 文件中应该包含 NVS 文件的全部内容。详情请参阅 :ref:`nvs-csv-file-format`。

        使用以下命令，可以生成加密的 NVS 分区：

        .. code:: bash

            python3 nvs_partition_gen.py encrypt sample_singlepage_blob.csv nvs_encr_partition.bin 0x3000 --inputkey keys/nvs_encr_key.bin

        下面解释一些命令参数：

        * CSV 文件名 - 此命令中，``sample_singlepage_blob.csv`` 是指包含 NVS 数据的 CSV 文件，请将其替换为所选择的文件。

        * NVS 分区偏移量 - 这是 {IDF_TARGET_NAME} flash 中存储 NVS 分区的偏移地址。通过在项目目录下执行 ``idf.py partition-table`` 命令，可以找到 NVS 分区偏移地址。请将上述命令中的示例值 ``0x3000`` 调整为正确的偏移量。

    4. 配置项目

        * 通过设置 :ref:`CONFIG_NVS_ENCRYPTION`，启用 `NVS 加密`。

        * 将 :ref:`CONFIG_NVS_SEC_KEY_PROTECTION_SCHEME` 设置为 ``CONFIG_NVS_SEC_KEY_PROTECT_USING_HMAC``，启用基于 HMAC 的 NVS 加密。

        * 通过设置 :ref:`CONFIG_NVS_SEC_HMAC_EFUSE_KEY_ID`，将 HMAC eFuse 密钥 ID 设为步骤 2 中烧录 eFuse 密钥的 ID。

    5. 烧录 NVS 分区

        使用 ``esptool.py`` 命令，将步骤 3 中生成的 NVS 分区 (``nvs_encr_partition.bin``) 烧录到相应的偏移地址。要查看所有推荐的 ``esptool.py`` 命令行选项，请查阅 ``idf.py build`` 构建成功后打印的输出。

        如果芯片启用了 flash 加密，请先加密分区再进行烧录。详情请参阅 `flash 加密工作流程 <enable-flash-encryption-externally_>`_ 的相关烧录步骤。

.. _enable-flash-enc-based-nvs-encryption:

基于 flash 加密启用 NVS 加密
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

在这种情况下，主机上生成 NVS 加密密钥，并将其烧录到芯片上，借助 :doc:`flash 加密 </security/flash-encryption>` 功能进行保护。

1. 生成 NVS 加密密钥

    使用 :doc:`NVS 分区生成工具 </api-reference/storage/nvs_partition_gen>`，可以生成相应的密钥。在主机上生成加密密钥，并将该密钥以加密状态存储在 {IDF_TARGET_NAME} 的 flash 中。

    使用以下命令，通过 :component_file:`nvs_flash/nvs_partition_generator/nvs_partition_gen.py` 脚本生成密钥：

    .. code:: bash

        python3 nvs_partition_gen.py generate-key --keyfile nvs_encr_key.bin

    ``keys`` 文件夹中将生成相应的密钥。

2. 生成加密的 NVS 分区

    在主机上生成实际的加密 NVS 分区，详情请参阅 :ref:`generate-encrypted-nvs-partition`。为此，CSV 文件应包含 NVS 文件数据，详情请参阅 :ref:`nvs-csv-file-format`。

    使用以下命令，可以生成加密的 NVS 分区：

    .. code:: bash

        python3 nvs_partition_gen.py encrypt sample_singlepage_blob.csv nvs_encr_partition.bin 0x3000 --inputkey keys/nvs_encr_key.bin

    下文解释了上述命令中的一些参数：

    * CSV 文件名 - 上述命名中的 `sample_singlepage_blob.csv` 是指包含 NVS 数据的 CSV 文件，请将其替换为所选文件。

    * NVS 分区偏移量 - 这是 NVS 分区在 {IDF_TARGET_NAME} 的 flash 中存储时的偏移地址。在项目目录中执行 ``idf.py partition-table`` 命令，可以找到 NVS 分区的偏移量。请将上述命令中的示例值 ``0x3000`` 替换为正确的偏移量。

3. 配置项目

    * 通过启用 :ref:`CONFIG_NVS_ENCRYPTION` 来启用 `NVS 加密`。
    * 通过将 :ref:`CONFIG_NVS_SEC_KEY_PROTECTION_SCHEME` 设置为 ``CONFIG_NVS_SEC_KEY_PROTECT_USING_FLASH_ENC``，配置 NVS 使用基于 flash 加密的方案。

4. 烧录 NVS 分区和 NVS 加密密钥

    使用 ``esptool.py`` 命令，将 NVS 分区 (``nvs_encr_partition.bin``) 和 NVS 加密密钥 (``nvs_encr_key.bin``) 烧录到各自的偏移地址。通过 ``idf.py build`` 成功后打印的输出，可查看所有推荐的 ``esptool.py`` 命令行选项。

    若芯片启用了 flash 加密，请在烧录之前先加密分区。详情请参阅 `flash 加密工作流程 <enable-flash-encryption-externally_>`_ 中与烧录相关的步骤。
