flash 加密
============

{IDF_TARGET_CRYPT_CNT:default="SPI_BOOT_CRYPT_CNT",esp32="FLASH_CRYPT_CNT"}

{IDF_TARGET_ESP32_V3_ONLY:default="", esp32="(ESP32 V3 only)"}

{IDF_TARGET_ENCRYPT_COMMAND:default="espsecure.py encrypt_flash_data --aes_xts", esp32="espsecure.py encrypt_flash_data"}

:link_to_translation:`en:[English]`

本文档旨在引导用户快速了解 {IDF_TARGET_NAME} 的 flash 加密功能，通过应用程序代码示例向用户演示如何在开发及生产过程中测试及验证 flash 加密的相关操作。


概述
------

flash 加密功能用于加密与 {IDF_TARGET_NAME} 搭载使用的片外 flash 中的内容。启用 flash 加密功能后，固件会以明文形式烧录，然后在首次启动时将数据进行加密。因此，物理读取 flash 将无法恢复大部分 flash 内容。

.. only:: esp32

    :doc:`安全启动<secure-boot-v2>` 是一个独立的功能，可以与 flash 加密一起使用，从而创建更安全的环境。

.. important::

    对于生产用途，flash 加密仅应在“发布”模式下启用。

.. important::

    启用 flash 加密将限制后续 {IDF_TARGET_NAME} 更新。在使用 flash 加密功能前，请务必阅读本文档了解其影响。

.. _encrypted-partitions:

加密分区
--------------------

启用 flash 加密后，会默认加密以下类型的数据：

- :ref:`second-stage-bootloader` （固件引导加载程序）
- 分区表
- :ref:`nvs_encr_key_partition`
- Otadata
- 所有 ``app`` 类型的分区

其他类型的数据将视情况进行加密：

- 分区表中标有 ``encrypted`` 标志的分区。如需了解详情，请参考 :ref:`encrypted-partition-flag`。
- 如果启用了安全启动，则会对安全启动引导程序摘要进行加密（见下文）。

.. _flash-encryption-efuse:

相关 eFuses
------------------------------

flash 加密操作由 {IDF_TARGET_NAME} 上的多个 eFuse 控制。以下是这些 eFuse 列表及其描述，下表中的各 eFuse 名称也在 espefuse.py 工具中使用，为了能在 eFuse API 中使用，请在名称前加上 ``ESP_EFUSE_``，如：esp_efuse_read_field_bit(ESP_EFUSE_DISABLE_DL_ENCRYPT)。

.. Comment: As text in cells of list-table header rows does not wrap, it is necessary to make 0 header rows and apply bold typeface to the first row. Otherwise, the table goes beyond the html page limits on the right.

.. only:: not SOC_FLASH_ENCRYPTION_XTS_AES

    .. list-table:: flash 加密过程中使用的 eFuses
       :widths: 25 40 10
       :header-rows: 0

       * - **eFuse**
         - **描述**
         - **位深**
       * - ``CODING_SCHEME``
         - 控制用于产生最终 256 位 AES 密钥的 block1 的实际位数。可能的值：``0`` 代表 256 位，``1`` 代表 192 位，``2`` 代表 128 位。最终的 AES 密钥根据 ``FLASH_CRYPT_CONFIG`` 值得出。
         - 2
       * - ``flash_encryption`` (block1)
         - AES 密钥存储。
         - 256 位密钥块
       * - ``FLASH_CRYPT_CONFIG``
         - 控制 AES 加密过程。
         - 4
       * - ``DISABLE_DL_ENCRYPT``
         - 设置后，在固件下载模式运行时禁用 flash 加密操作。
         - 1
       * - ``DISABLE_DL_DECRYPT``
         - 设置后，在 UART 固件下载模式运行时禁用 flash 解密操作。
         - 1
       * - ``{IDF_TARGET_CRYPT_CNT}``
         - 通过 :math:`2^n` 数字来表示 flash 的内容是否已被加密.

           * 如果设置了奇数个比特位（例如 ``0b0000001`` 或 ``0b0000111``）, 表示 flash 的内容已加密。读取时，内容需要进行透明解密。
           * 如果设置了偶数个比特位（例如 ``0b0000000`` 或 ``0b0000011``）, 表示 flash 的内容未被加密 (即明文)。

           随着每次连续的 flash 未加密（例如烧录一个新的未加密二进制文件）与进行 flash 加密（通过 :ref:`启动时启用 flash 加密功能 <CONFIG_SECURE_FLASH_ENC_ENABLED>` 选项）, ``{IDF_TARGET_CRYPT_CNT}`` 的下一个的最高有效位 (MSB) 会被设置。
         - 7


.. only:: SOC_FLASH_ENCRYPTION_XTS_AES_256

    .. list-table:: flash 加密过程中使用的 eFuses
       :widths: 25 40 10
       :header-rows: 0

       * - **eFuse**
         - **描述**
         - **位深**
       * - ``BLOCK_KEYN``
         - AES 密钥存储，N 在 0-5 之间。
         - XTS_AES_128 有一个 256 位密钥块，XTS_AES_256 有两个 256 位密钥块（共 512 位）。
       * - ``KEY_PURPOSE_N``
         - 控制 eFuse 块 ``BLOCK_KEYN`` 的目的，其中 N 在 0-5 之间。可能的值：``2`` 代表 ``XTS_AES_256_KEY_1``，``3`` 代表 ``XTS_AES_256_KEY_2``，``4`` 代表 ``XTS_AES_128_KEY``。最终 AES 密钥是基于其中一个或两个目的 eFuses 值推导。有关各种可能的组合，请参阅 *{IDF_TARGET_NAME} 技术参考手册* > *外部内存加密和解密（XTS_AES)* [`PDF <{IDF_TARGET_TRM_CN_URL}#extmemencr>`__]。
         - 4
       * - ``DIS_DOWNLOAD_MANUAL_ENCRYPT``
         - 设置后，在下载启动模式下禁用 flash 加密。
         - 1
       * - ``{IDF_TARGET_CRYPT_CNT}``
         - 设置 SPI 启动模式后，可启用加密和解密。如果在 eFuse 中设置了 1 或 3 个比特位，则启用该功能，否则将禁用。
         - 3

.. only:: SOC_FLASH_ENCRYPTION_XTS_AES_128 and not SOC_FLASH_ENCRYPTION_XTS_AES_256 and not SOC_EFUSE_CONSISTS_OF_ONE_KEY_BLOCK

    .. list-table:: flash 加密过程中使用的 eFuses
       :widths: 25 40 10
       :header-rows: 0

       * - **eFuse**
         - **描述**
         - **位深**
       * - ``BLOCK_KEYN``
         - AES 密钥存储，N 在 0-5 之间。
         - 256 位密钥块
       * - ``KEY_PURPOSE_N``
         - 控制 eFuse 块 ``BLOCK_KEYN`` 的目的，其中 N 在 0 到 5 之间。对于 flash 加密，唯一的有效值是 4，代表 ``XTS_AES_128_KEY``。
         - 4
       * - ``DIS_DOWNLOAD_MANUAL_ENCRYPT``
         - 设置后，则在下载引导模式时禁用 flash 加密。
         - 1
       * - ``{IDF_TARGET_CRYPT_CNT}``
         - 设置 SPI 启动模式后，可启用加密和解密。如果在 eFuse 中设置 1 或 3 个比特位，则启用该功能，否则将禁用。
         - 3

.. only:: SOC_FLASH_ENCRYPTION_XTS_AES_128 and SOC_EFUSE_CONSISTS_OF_ONE_KEY_BLOCK

    .. list-table:: flash 加密过程中使用的 eFuses
       :widths: 25 40 10
       :header-rows: 0

       * - **eFuse**
         - **描述**
         - **位深**
       * - ``XTS_KEY_LENGTH_256``
         - 控制用于得出最终 256 位 AES 密钥的 eFuse 比特的实际数量。可能的值：``1`` 使用 eFuse 块的全部 256 位作为密钥，``0`` 使用 eFuse 块的低 128 位作为密钥（高 128 位保留给安全启动密钥）。对于 128 位选项，最终的 AES 密钥会以 SHA256 (EFUSE_KEY0_FE_128BIT) 的形式得出。
         - 1
       * - ``BLOCK_KEY0``
         - AES 密钥存储
         - 256 位或 128 位密钥块
       * - ``DIS_DOWNLOAD_MANUAL_ENCRYPT``
         - 设置后，则在下载引导模式时禁用 flash 加密。
         - 1
       * - ``{IDF_TARGET_CRYPT_CNT}``
         - 设置 SPI 启动模式后，可启用加密和解密。如果在 eFuse 中设置 1 或 3 个比特位，则启用该功能，否则将禁用。
         - 3

.. note::

  * 上表中列出的所有 eFuse 位都提供读/写访问控制。
  * 这些位的默认值是 0。

对上述 eFuse 位的读写访问由 ``WR_DIS`` 和 ``RD_DIS`` 寄存器中的相应字段控制。有关 {IDF_TARGET_NAME} eFuse 的详细信息，请参考 :doc:`eFuse 管理器 <../api-reference/system/efuse>`。要使用 espefuse.py 更改 eFuse 字段的保护位，请使用以下两个命令：read_protect_efuse 和 write_protect_efuse。例如 ``espefuse.py write_protect_efuse DISABLE_DL_ENCRYPT``。

.. only:: esp32c2

    .. important::

       {IDF_TARGET_NAME} 具有安全启动和 flash 加密两个密钥，但仅有一个 eFuse 密钥块。由于 eFuse 密钥块仅支持一次烧录，故应将密钥同时同批进行烧录。请勿单独启用“安全启动”或 “flash 加密”，否则在 eFuse 密钥块随后的写入中将返回错误。

flash 的加密过程
------------------

假设 eFuse 值处于默认状态，且固件的引导加载程序编译为支持 flash 加密，则 flash 加密的具体过程如下：

.. only:: not SOC_FLASH_ENCRYPTION_XTS_AES

  1. 第一次开机复位时，flash 中的所有数据都是未加密的（明文）。ROM 引导加载程序加载固件引导加载程序。

  2. 固件的引导加载程序将读取 ``{IDF_TARGET_CRYPT_CNT}`` eFuse 值 (``0b0000000``)。因为该值为 0（偶数位），固件的引导加载程序将配置并启用 flash 加密块，同时将 ``FLASH_CRYPT_CONFIG`` eFuse 的值编程为 0xF。关于 flash 加密块的更多信息，请参考 *{IDF_TARGET_NAME} 技术参考手册* > *eFuse 控制器 (eFuse)* > *flash 加密块* [`PDF <{IDF_TARGET_TRM_CN_URL}#efuse>`__]。

  3. 固件引导加载程序首先检查 eFuse 中是否已经存在有效密钥（例如用 espefuse 工具烧写的密钥），如果存在，则会跳过密钥生成，并将该密钥用于 flash 加密过程。否则，固件引导加载程序会使用 RNG（随机数发生器）模块生成一个 AES-256 位密钥，并将其写入 ``flash_encryption`` eFuse 中。由于已设置了 ``flash_encryption`` eFuse 的读保护位和写保护位，因此无法通过软件访问密钥。flash 加密操作完全在硬件中完成，无法通过软件访问密钥。

  4. flash 加密块将加密 flash 的内容（固件的引导加载程序、应用程序、以及标有 ``加密`` 标志的分区）。就地加密可能会耗些时间（对于大分区最多需要一分钟）。

  5. 固件引导加载程序将在 ``{IDF_TARGET_CRYPT_CNT}`` (0b0000001) 中设置第一个可用位来对已加密的 flash 内容进行标记。设置奇数个比特位。

  6. 对于 :ref:`flash-enc-development-mode`，固件引导加载程序仅设置 ``DISABLE_DL_DECRYPT`` 和 ``DISABLE_DL_CACHE`` 的 eFuse 位，以便 UART 引导加载程序重新烧录加密的二进制文件。此外， ``{IDF_TARGET_CRYPT_CNT}`` 的 eFuse 位不受写入保护。

  7. 对于 :ref:`flash-enc-release-mode`，固件引导加载程序设置 ``DISABLE_DL_ENCRYPT``、``DISABLE_DL_DECRYPT`` 和 ``DISABLE_DL_CACHE`` 的 eFuse 位为 1，以防止 UART 引导加载程序解密 flash 内容。它还写保护 ``{IDF_TARGET_CRYPT_CNT}`` eFuse 位。要修改此行为，请参阅 :ref:`uart-bootloader-encryption`。

  8. 重新启动设备以开始执行加密镜像。固件引导加载程序调用 flash 解密块来解密 flash 内容，然后将解密的内容加载到 IRAM 中。

.. only:: SOC_FLASH_ENCRYPTION_XTS_AES_256

  1. 第一次开机复位时，flash 中的所有数据都是未加密的（明文）。ROM 引导加载程序加载固件引导加载程序。

  2. 固件的引导加载程序将读取 ``{IDF_TARGET_CRYPT_CNT}`` eFuse 值 (``0b000``)。因为该值为 0（偶数位），固件引导加载程序将配置并启用 flash 加密块。关于 flash 加密块的更多信息，请参考 *{IDF_TARGET_NAME} 技术参考手册* > *eFuse 控制器 (eFuse)* > *自动加密块* [`PDF <{IDF_TARGET_TRM_CN_URL}#efuse>`__]。

  3. 固件引导加载程序首先检查 eFuse 中是否已经存在有效密钥（例如用 espefuse 工具烧写的密钥），如果存在，则会跳过密钥生成，并将该密钥用于 flash 加密过程。否则，固件引导加载程序使用 RNG（随机数发生器）模块生成一个 256 位或 512 位的密钥，具体位数取决于 :ref:`生成的 XTS-AES 密钥的大小 <CONFIG_SECURE_FLASH_ENCRYPTION_KEYSIZE>`，然后将其分别写入一个或两个 `BLOCK_KEYN` eFuse 中。软件也为存储密钥的块更新了 ``KEY_PURPOSE_N``。由于上述一个或两个 ``BLOCK_KEYN`` eFuse 已设置了读保护和写保护位，因此无法通过软件访问密钥。``KEY_PURPOSE_N`` 字段也受写保护。flash 加密操作完全在硬件中完成，无法通过软件访问密钥。

  4. flash 加密块将加密 flash 的内容（固件的引导加载程序、应用程序、以及标有“加密”标志的分区）。就地加密可能会耗些时间（对于大分区最多需要一分钟）。

  5. 固件引导加载程序将在 ``{IDF_TARGET_CRYPT_CNT}`` (0b001) 中设置第一个可用位来对已加密的 flash 内容进行标记。设置奇数位。

  6. 对于 :ref:`flash-enc-development-mode`，固件引导加载程序允许 UART 引导加载程序重新烧录加密后的二进制文件。同时，``{IDF_TARGET_CRYPT_CNT}`` eFuse 位不受写入保护。此外，固件引导加载程序默认置位以下 eFuse 位：

    .. list::

      :esp32s2: - ``DIS_BOOT_REMAP``
      - ``DIS_DOWNLOAD_ICACHE``
      - ``DIS_DOWNLOAD_DCACHE``
      - ``HARD_DIS_JTAG``
      - ``DIS_LEGACY_SPI_BOOT``

  7. 对于 :ref:`flash-enc-release-mode`，固件引导加载程序设置所有在开发模式下设置的 eFuse 位。它还写保护 ``{IDF_TARGET_CRYPT_CNT}`` eFuse 位。要修改此行为，请参阅 :ref:`uart-bootloader-encryption`。

  8. 重新启动设备以开始执行加密镜像。固件引导加载程序调用 flash 解密块来解密 flash 内容，然后将解密的内容加载到 IRAM 中。

.. only:: SOC_FLASH_ENCRYPTION_XTS_AES_128 and not SOC_FLASH_ENCRYPTION_XTS_AES_256 and not SOC_EFUSE_CONSISTS_OF_ONE_KEY_BLOCK

  1. 第一次开机复位时，flash 中的所有数据都是未加密的（明文）。ROM 引导加载程序加载固件引导加载程序。

  2. 固件的引导加载程序将读取 ``{IDF_TARGET_CRYPT_CNT}`` eFuse 值 (``0b000``)。因为该值为 0（偶数位），固件引导加载程序将配置并启用 flash 加密块。关于 flash 加密块的更多信息，请参考 `{IDF_TARGET_NAME} 技术参考手册 <{IDF_TARGET_TRM_CN_URL}>`_。

  3. 固件的引导加载程序使用 RNG（随机数发生器）模块生成 256 位密钥，然后将其写入 `BLOCK_KEYN` eFuse。软件也为存储密钥的块更新了 ``KEY_PURPOSE_N``。由于 ``BLOCK_KEYN`` eFuse 已设置了读保护和写保护位，因此无法通过软件访问密钥。``KEY_PURPOSE_N`` 字段也受写保护。flash 加密操作完全在硬件中完成，无法通过软件访问密钥。如果 eFuse 中已经存在有效密钥（例如用 espefuse 工具烧写的密钥），则会跳过密钥生成，并将该密钥用于 flash 加密过程。

  4. flash 加密块将加密 flash 的内容（固件的引导加载程序、应用程序、以及标有“加密”标志的分区）。就地加密可能会耗些时间（对于大分区最多需要一分钟）。

  5. 固件引导加载程序将在 ``{IDF_TARGET_CRYPT_CNT}`` (0b001) 中设置第一个可用位来对已加密的 flash 内容进行标记。设置奇数位。

  6. 对于 :ref:`flash-enc-development-mode`，固件引导加载程序允许 UART 引导加载程序重新烧录加密后的二进制文件。同时，``{IDF_TARGET_CRYPT_CNT}`` eFuse 位不受写入保护。此外，默认情况下，固件引导加载程序设置 ``DIS_DOWNLOAD_ICACHE``、 ``DIS_PAD_JTAG``、 ``DIS_USB_JTAG`` 和 ``DIS_LEGACY_SPI_BOOT`` eFuse 位。

  7. 对于 :ref:`flash-enc-release-mode`，固件引导加载程序设置所有在开发模式下设置的 eFuse 位以及 ``DIS_DOWNLOAD_MANUAL_ENCRYPT``。它还写保护 ``{IDF_TARGET_CRYPT_CNT}`` eFuse 位。要修改此行为，请参阅 :ref:`uart-bootloader-encryption`。

  8. 重新启动设备以开始执行加密镜像。固件引导加载程序调用 flash 解密块来解密 flash 内容，然后将解密的内容加载到 IRAM 中。

.. only:: SOC_FLASH_ENCRYPTION_XTS_AES_128 and SOC_EFUSE_CONSISTS_OF_ONE_KEY_BLOCK

  1. 第一次开机复位时，flash 中的所有数据都是未加密的（明文）。ROM 引导加载程序加载固件引导加载程序。

  2. 固件的引导加载程序将读取 ``{IDF_TARGET_CRYPT_CNT}`` eFuse 值 (``0b000``)。因为该值为 0（偶数位），固件引导加载程序将配置并启用 flash 加密块。关于 flash 加密块的更多信息，请参考 `{IDF_TARGET_NAME} 技术参考手册 <{IDF_TARGET_TRM_CN_URL}>`_。

  3. 固件的引导加载程序使用 RNG（随机数发生器）模块生成 256 位或 128 位密钥（具体位数取决于 :ref:`生成的 XTS-AES 密钥大小 <CONFIG_SECURE_FLASH_ENCRYPTION_KEYSIZE>`），然后将其写入 `BLOCK_KEY0` eFuse。同时，根据所选选项，软件对 ``XTS_KEY_LENGTH_256`` 进行更新。由于 ``BLOCK_KEY0`` eFuse 已设置写保护和读保护位，故无法通过软件访问密钥。flash 加密操作完全在硬件中完成，无法通过软件访问密钥。若使用 128 位 flash 加密密钥，则整个 eFuse 密钥块都受写保护，但只有低 128 位受读保护，高 128 位是可读的，以满足安全启动的需要。如果 flash 加密密钥是 256 位，那么 ``XTS_KEY_LENGTH_256`` 为 1，否则为 0。为防止意外将 eFuse 从 0 改为 1，为 RELEASE 模式设置了一个写保护位。如果 eFuse 中已经存在有效密钥（例如用 espefuse 工具烧写的密钥），则跳过密钥生成，并将该密钥用于 flash 加密过程。

  4. flash 加密块将加密 flash 的内容（固件的引导加载程序、应用程序、以及标有“加密”标志的分区）。就地加密可能会耗些时间（对于大分区最多需要一分钟）。

  5. 固件引导加载程序将在 ``{IDF_TARGET_CRYPT_CNT}`` (0b001) 中设置第一个可用位来对已加密的 flash 内容进行标记。设置奇数位。

  6. 对于 :ref:`flash-enc-development-mode`，固件引导加载程序允许 UART 引导加载程序重新烧录加密后的二进制文件。同时，``{IDF_TARGET_CRYPT_CNT}`` eFuse 位不受写入保护。此外，默认情况下，固件引导加载程序设置 ``DIS_DOWNLOAD_ICACHE``、``DIS_PAD_JTAG`` 和 ``DIS_DIRECT_BOOT`` eFuse 位。

  7. 对于 :ref:`flash-enc-release-mode`，固件引导加载程序设置所有在开发模式下设置的 eFuse 位以及 ``DIS_DOWNLOAD_MANUAL_ENCRYPT``。它还写保护 ``{IDF_TARGET_CRYPT_CNT}`` eFuse 位。要修改此行为，请参阅 :ref:`uart-bootloader-encryption`。

  8. 重新启动设备以开始执行加密镜像。固件引导加载程序调用 flash 解密块来解密 flash 内容，然后将解密的内容加载到 IRAM 中。

在开发阶段常需编写不同的明文 flash 镜像并测试 flash 的加密过程。这要求固件下载模式能够根据需求不断加载新的明文镜像。但是，在制造和生产过程中，出于安全考虑，固件下载模式不应有权限访问 flash 内容。

因此需要有两种不同的 flash 加密配置：一种用于开发，另一种用于生产。详情请参考 `flash 加密设置`_ 小节。



flash 加密设置
----------------------

提供以下 flash 加密模式：

- :ref:`flash-enc-development-mode` - 建议仅在开发过程中使用。因为在这种模式下，仍然可以将新的明文固件烧录到设备，并且引导加载程序将使用存储在硬件中的密钥对该固件进行透明加密。此操作间接允许从 flash 中读出固件明文。
- :ref:`flash-enc-release-mode` - 推荐用于制造和生产。因为在这种模式下，如果不知道加密密钥，则不可能将明文固件烧录到设备。

本节将详细介绍上述 flash 加密模式，并且逐步说明如何使用它们。

.. _flash-enc-development-mode:

开发模式
^^^^^^^^^^

在开发过程中，可使用 {IDF_TARGET_NAME} 内部生成的密钥或外部主机生成的密钥进行 flash 加密。


使用 {IDF_TARGET_NAME} 生成的密钥
"""""""""""""""""""""""""""""""""""""

开发模式允许用户使用固件下载模式下载多个明文镜像。

测试 flash 加密过程需完成以下步骤：

1. 确保你的 {IDF_TARGET_NAME} 设备有 :ref:`flash-encryption-efuse` 中所示的 flash 加密 eFuse 的默认设置。

  请参考如何检查 :ref:`flash-encryption-status`。

2. 在 :ref:`项目配置菜单 <project-configuration-menu>`，执行以下操作：

  .. list::

    - :ref:`启动时使能 flash 加密 <CONFIG_SECURE_FLASH_ENC_ENABLED>`。
    - :ref:`选择加密模式 <CONFIG_SECURE_FLASH_ENCRYPTION_MODE>` （默认是 **开发模式**）。
    :esp32: - :ref:`选择 UART ROM 下载模式 <CONFIG_SECURE_UART_ROM_DL_MODE>` （默认是 **启用**）。请注意，对于 ESP32 芯片，该选项仅在 :ref:`CONFIG_ESP32_REV_MIN` 级别设置为 3 时 (ESP32 V3) 可用。
    :not esp32: - :ref:`选择 UART ROM 下载模式 <CONFIG_SECURE_UART_ROM_DL_MODE>` （默认是 **启用**）。
    :SOC_FLASH_ENCRYPTION_XTS_AES_OPTIONS: - 设置 :ref:`生成的 XTS-AES 密钥大小 <CONFIG_SECURE_FLASH_ENCRYPTION_KEYSIZE>`。
    - :ref:`选择适当详细程度的引导加载程序日志 <CONFIG_BOOTLOADER_LOG_LEVEL>`。
    - 保存配置并退出。

启用 flash 加密将增大引导加载程序，因而可能需更新分区表偏移量。请参考 :ref:`引导加载程序大小 <bootloader-size>`。

3. 运行以下命令来构建和烧录完整的镜像。

  .. code-block:: bash

      idf.py flash monitor

  .. note::

       这个命令不包括任何应该写入 flash 分区的用户文件。请在运行此命令前手动写入这些文件，否则在写入前应单独对这些文件进行加密。

该命令将向 flash 写入未加密的镜像：固件引导加载程序、分区表和应用程序。烧录完成后，{IDF_TARGET_NAME} 将复位。在下一次启动时，固件引导加载程序会加密：固件引导加载程序、应用程序分区和标记为“加密”的分区，然后复位。就地加密可能需要时间，对于大分区最多需要一分钟。之后，应用程序在运行时解密并执行命令。

下面是启用 flash 加密后 {IDF_TARGET_NAME} 首次启动时的样例输出：

.. include:: {IDF_TARGET_PATH_NAME}_log.inc
   :start-after: first_boot_enc
   :end-before: ------

启用 flash 加密后，在下次启动时输出将显示已启用 flash 加密，样例输出如下：

.. include:: {IDF_TARGET_PATH_NAME}_log.inc
   :start-after: already_en_enc
   :end-before: ------

在此阶段，如果用户需要更新或重新烧录二进制文件，请参考 :ref:`encrypt-partitions`。


.. _pregenerated-flash-encryption-key:

使用主机生成的密钥
""""""""""""""""""""""""

可在主机中预生成 flash 加密密钥，并将其烧录到 eFuse 密钥块中。这样，无需明文 flash 更新便可以在主机上预加密数据并将其烧录。该功能可在 :ref:`flash-enc-development-mode` 和 :ref:`flash-enc-release-mode` 两模式下使用。如果没有预生成的密钥，数据将以明文形式烧录，然后 {IDF_TARGET_NAME} 对数据进行就地加密。

.. note::

    不建议在生产中使用该方法，除非为每个设备都单独生成一个密钥。

.. only:: esp32c2

    .. note::

       请注意， {IDF_TARGET_NAME} 只有一个 eFuse 密钥块，同时用于安全启动和 flash 加密密钥。因此，如果使用了安全启动密钥，则主机生成的 flash 加密密钥必须与安全启动密钥一起写入，否则将无法使用安全启动。

使用主机生成的密钥需完成以下步骤：

1. 确保你的 {IDF_TARGET_NAME} 设备有 :ref:`flash-encryption-efuse` 中所示的 flash 加密 eFuse 的默认设置。

  请参考如何检查 :ref:`flash-encryption-status`。

2. 通过运行以下命令生成一个随机密钥：

  .. only:: not SOC_FLASH_ENCRYPTION_XTS_AES

      .. code-block:: bash

          espsecure.py generate_flash_encryption_key my_flash_encryption_key.bin

  .. only:: SOC_FLASH_ENCRYPTION_XTS_AES_256

    如果 :ref:`生成的 XTS-AES 密钥大小 <CONFIG_SECURE_FLASH_ENCRYPTION_KEYSIZE>` 是 AES-128（256 位密钥）：

      .. code-block:: bash

          espsecure.py generate_flash_encryption_key my_flash_encryption_key.bin

    如果 :ref:`生成的 XTS-AES 密钥大小 <CONFIG_SECURE_FLASH_ENCRYPTION_KEYSIZE>` 是 AES-256（512 位密钥）：

      .. code-block:: bash

          espsecure.py generate_flash_encryption_key --keylen 512 my_flash_encryption_key.bin


  .. only:: SOC_FLASH_ENCRYPTION_XTS_AES_128 and not SOC_FLASH_ENCRYPTION_XTS_AES_256 and not SOC_EFUSE_CONSISTS_OF_ONE_KEY_BLOCK

      .. code-block:: bash

          espsecure.py generate_flash_encryption_key my_flash_encryption_key.bin

.. only:: SOC_FLASH_ENCRYPTION_XTS_AES_128 and SOC_EFUSE_CONSISTS_OF_ONE_KEY_BLOCK

      如果 :ref:`生成的 XTS-AES 密钥大小 <CONFIG_SECURE_FLASH_ENCRYPTION_KEYSIZE>` 是 AES-128（256 位密钥）:

     .. code-block:: bash

          espsecure.py generate_flash_encryption_key my_flash_encryption_key.bin

      或者如果 :ref:`生成的 XTS-AES 密钥大小 <CONFIG_SECURE_FLASH_ENCRYPTION_KEYSIZE>` 是由 128 位导出的 AES-128 密钥（SHA256（128 位））:

      .. code-block:: bash

          espsecure.py generate_flash_encryption_key --keylen 128 my_flash_encryption_key.bin

3. **在第一次加密启动前**，使用以下命令将该密钥烧录到设备上，这个操作只能执行 **一次**。

  .. only:: not SOC_FLASH_ENCRYPTION_XTS_AES

    .. code-block:: bash

        espefuse.py --port PORT burn_key flash_encryption my_flash_encryption_key.bin

  .. only:: SOC_FLASH_ENCRYPTION_XTS_AES_256

    .. code-block:: bash

        espefuse.py --port PORT burn_key BLOCK my_flash_encryption_key.bin KEYPURPOSE

    其中 ``BLOCK`` 是 ``BLOCK_KEY0`` 和 ``BLOCK_KEY5`` 之间的空闲密钥区。而 ``KEYPURPOSE`` 是 ``AES_256_KEY_1``、``XTS_AES_256_KEY_2`` 或 ``XTS_AES_128_KEY``。关于密钥用途，请参考 `{IDF_TARGET_NAME} 技术参考手册 <{IDF_TARGET_TRM_CN_URL}>`_。

    对于 AES-128（256 位密钥）- ``XTS_AES_128_KEY``:

    .. code-block:: bash

        espefuse.py --port PORT burn_key BLOCK my_flash_encryption_key.bin XTS_AES_128_KEY

    对于 AES-256（512 位密钥）- ``XTS_AES_256_KEY_1`` 和 ``XTS_AES_256_KEY_2``。espefuse.py 支持通过虚拟密钥用途 ``XTS_AES_256_KEY`` 将这两个密钥用途和一个 512 位密钥一起烧录到两个独立的密钥块。使用此功能时，``espefuse.py`` 将把密钥的前 256 位烧录到指定的 ``BLOCK``，并把相应的区块密钥用途烧录到 ``XTS_AES_256_KEY_1``。密钥的后 256 位将被烧录到 ``BLOCK`` 后的第一个空闲密钥块，并把相应的密钥用途烧录到 ``XTS_AES_256_KEY_2``。

    .. code-block:: bash

        espefuse.py  --port PORT  burn_key BLOCK my_flash_encryption_key.bin XTS_AES_256_KEY

    如果你想指定使用哪两个区块，则可以将密钥分成两个 256 位密钥，并分别使用 ``XTS_AES_256_KEY_1`` 和 ``XTS_AES_256_KEY_2`` 为密钥用途进行手动烧录：

    .. code-block:: bash

      split -b 32 my_flash_encryption_key.bin my_flash_encryption_key.bin.
      espefuse.py  --port PORT  burn_key BLOCK my_flash_encryption_key.bin.aa XTS_AES_256_KEY_1
      espefuse.py  --port PORT  burn_key BLOCK+1 my_flash_encryption_key.bin.ab XTS_AES_256_KEY_2


  .. only:: SOC_FLASH_ENCRYPTION_XTS_AES_128 and not SOC_FLASH_ENCRYPTION_XTS_AES_256 and not SOC_EFUSE_CONSISTS_OF_ONE_KEY_BLOCK

    .. code-block:: bash

        espefuse.py --port PORT burn_key BLOCK my_flash_encryption_key.bin XTS_AES_128_KEY

    其中 ``BLOCK`` 是 ``BLOCK_KEY0`` 和 ``BLOCK_KEY5`` 之间的一个空闲密钥区。

  .. only:: SOC_FLASH_ENCRYPTION_XTS_AES_128 and SOC_EFUSE_CONSISTS_OF_ONE_KEY_BLOCK

    对于 AES-128（256 位密钥）- ``XTS_AES_128_KEY`` （ ``XTS_KEY_LENGTH_256`` eFuse 将被烧录为 1）:

    .. code-block:: bash

        espefuse.py  --port PORT  burn_key BLOCK_KEY0 flash_encryption_key256.bin XTS_AES_128_KEY

    对于由 128 位导出的 AES-128 密钥（SHA256（128 位））- ``XTS_AES_128_KEY_DERIVED_FROM_128_EFUSE_BITS``。flash 加密密钥会被写入 eFuse BLOCK_KEY0 的低位，留出高 128 位以支持软件读取。如小节 ``同时烧录两个密钥`` 所示，在 espefuse 工具的特殊模式下，你可以使用任意 espefuse 命令来写入数据。

    .. code-block:: bash

        espefuse.py  --port PORT  burn_key BLOCK_KEY0 flash_encryption_key128.bin XTS_AES_128_KEY_DERIVED_FROM_128_EFUSE_BITS

    同时烧录两个密钥（安全启动和 flash 加密）:

    .. code-block:: bash

        espefuse.py --port PORT --chip esp32c2  burn_key_digest secure_boot_signing_key.pem \
                                                burn_key BLOCK_KEY0 flash_encryption_key128.bin XTS_AES_128_KEY_DERIVED_FROM_128_EFUSE_BITS

  如果未烧录密钥并在启用 flash 加密后启动设备，{IDF_TARGET_NAME} 将生成一个软件无法访问或修改的随机密钥。

4. 在 :ref:`项目配置菜单 <project-configuration-menu>` 中进行如下设置：

    - :ref:`启动时启用 flash 加密功能 <CONFIG_SECURE_FLASH_ENC_ENABLED>`
    - :ref:`选择加密模式 <CONFIG_SECURE_FLASH_ENCRYPTION_MODE>` （默认为 **开发模式**）
    - :ref:`选择适当详细程度的引导加载程序日志 <CONFIG_BOOTLOADER_LOG_LEVEL>`
    - 保存配置并退出

启用 flash 加密将增大引导加载程序，因而可能需更新分区表偏移量。请参考 :ref:`引导加载程序大小 <bootloader-size>`。

5. 运行以下命令来构建并烧录完整的镜像：

  .. code-block:: bash

      idf.py flash monitor

  .. note::

      这个命令不包括任何应该被写入 flash 上的分区的用户文件。请在运行此命令前手动写入这些文件，否则在写入前应单独对这些文件进行加密。

  该命令将向 flash 写入未加密的镜像：固件引导加载程序、分区表和应用程序。烧录完成后，{IDF_TARGET_NAME} 将复位。在下一次启动时，固件引导加载程序会加密：固件引导加载程序、应用程序分区和标记为 ``加密`` 的分区，然后复位。就地加密可能需要时间，对于大的分区来说可能耗时一分钟。之后，应用程序在运行时被解密并执行。

如果使用开发模式，那么更新和重新烧录二进制文件最简单的方法是 :ref:`encrypt-partitions`。

如果使用发布模式，那么可以在主机上预先加密二进制文件，然后将其作为密文烧录。具体请参考 :ref:`manual-encryption`。


.. _encrypt-partitions:

重新烧录更新后的分区
""""""""""""""""""""""""""""""

如果用户以明文方式更新了应用程序代码并需要重新烧录，则需要在烧录前对其进行加密。请运行以下命令一次完成应用程序的加密与烧录：

.. code-block:: bash

    idf.py encrypted-app-flash monitor

如果所有分区都需要以加密形式更新，请运行：

.. code-block:: bash

    idf.py encrypted-flash monitor


.. _flash-enc-release-mode:

发布模式
^^^^^^^^^^

在发布模式下，UART 引导加载程序无法执行 flash 加密操作，**只能** 使用 OTA 方案下载新的明文镜像，该方案将在写入 flash 前加密明文镜像。

使用该模式需要执行以下步骤：

1. 确保你的 {IDF_TARGET_NAME} 设备有 :ref:`flash-encryption-efuse` 中所示的 flash 加密 eFuse 的默认设置。

  请参考如何检查 :ref:`flash-encryption-status`。

2. 在 :ref:`项目配置菜单 <project-configuration-menu>`，执行以下操作：

  .. list::

    - :ref:`启动时使能 flash 加密 <CONFIG_SECURE_FLASH_ENC_ENABLED>`
    :esp32: - :ref:`选择发布模式 <CONFIG_SECURE_FLASH_ENCRYPTION_MODE>` （注意一旦选择了发布模式，``DISABLE_DL_ENCRYPT`` 和 ``DISABLE_DL_DECRYPT`` eFuse 位将被编程为在 ROM 下载模式下禁用 flash 加密硬件）
    :esp32: - :ref:`选择 UART ROM 下载模式（推荐永久性禁用）<CONFIG_SECURE_UART_ROM_DL_MODE>` （注意该选项仅在 :ref:`CONFIG_ESP32_REV_MIN` 级别设置为 3 时 (ESP32 V3) 可用。）默认选项是保持启用 UART ROM 下载模式，然而建议永久禁用该模式，以减少攻击者可用的选项。
    :not esp32: - :ref:`选择发布模式 <CONFIG_SECURE_FLASH_ENCRYPTION_MODE>` （注意一旦选择了发布模式，``EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT`` eFuse 位将被编程为在 ROM 下载模式下禁用 flash 加密硬件。）
    :not esp32: - :ref:`选择 UART ROM 下载（推荐永久性的切换到安全模式）<CONFIG_SECURE_UART_ROM_DL_MODE>`。这是默认且推荐使用的选项。如果不需要该模式，也可以改变此配置设置永久地禁用 UART ROM 下载模式。
    - :ref:`选择适当详细程度的引导加载程序日志 <CONFIG_BOOTLOADER_LOG_LEVEL>`
    - 保存配置并退出

启用 flash 加密将增大引导加载程序，因而可能需更新分区表偏移量。请参考 :ref:`引导加载程序大小 <bootloader-size>`。

3. 运行以下命令来构建并烧录完整的镜像：

  .. code-block:: bash

      idf.py flash monitor


  .. note::

      这个命令不包括任何应该被写入 flash 分区的用户文件。请在运行此命令前手动写入这些文件，否则在写入前应单独对这些文件进行加密。

  该命令将向 flash 写入未加密的镜像：固件引导加载程序、分区表和应用程序。烧录完成后，{IDF_TARGET_NAME} 将复位。在下一次启动时，固件引导加载程序会加密：固件引导加载程序、应用程序分区和标记为 ``加密`` 的分区，然后复位。就地加密可能需要时间，对于大的分区来说可能耗时一分钟。之后，应用程序在运行时被解密并执行。

一旦在发布模式下启用 flash 加密，引导加载程序将写保护 ``{IDF_TARGET_CRYPT_CNT}`` eFuse。

请使用 :ref:`OTA 方案 <updating-encrypted-flash-ota>` 对字段中的明文进行后续更新。

.. note::

    如果用户已经预先生成了 flash 加密密钥并存储了一个副本，并且 UART 下载模式没有通过 :ref:`CONFIG_SECURE_UART_ROM_DL_MODE` {IDF_TARGET_ESP32_V3_ONLY} 永久禁用，那么可以通过使用 ``{IDF_TARGET_ENCRYPT_COMMAND}`` 预加密文件，从而在在本地更新 flash，然后烧录密文。请参考 :ref:`manual-encryption`。

.. _flash-encrypt-best-practices:

最佳实践
^^^^^^^^^^^^^^

在生产中使用 flash 加密时：

.. list::

   - 不要在多个设备之间重复使用同一个 flash 加密密钥，这样攻击者就无法从一台设备上复制加密数据后再将其转移到第二台设备上。
   :esp32: - 在使用 ESP32 V3 时，如果生产设备不需要 UART ROM 下载模式，那么则该禁用该模式以增加设备安全性。这可以通过在应用程序启动时调用 :cpp:func:`esp_efuse_disable_rom_download_mode` 来实现。或者，可将项目 :ref:`CONFIG_ESP32_REV_MIN` 级别配置为 3（仅针对 ESP32 V3），然后选择 :ref:`CONFIG_SECURE_UART_ROM_DL_MODE` 为“永久性的禁用 ROM 下载模式（推荐）”。在早期的 ESP32 版本上无法禁用 ROM 下载模式。
   :not esp32: - 如果不需要 UART ROM 下载模式，则应完全禁用该模式，或者永久设置为“安全下载模式”。安全下载模式永久性地将可用的命令限制在更新 SPI 配置、更改波特率、基本的 flash 写入和使用 `get_security_info` 命令返回当前启用的安全功能摘要。默认在发布模式下第一次启动时设置为安全下载模式。要完全禁用下载模式，请选择 :ref:`CONFIG_SECURE_UART_ROM_DL_MODE` 为“永久禁用 ROM 下载模式（推荐）”或在运行时调用 :cpp:func:`esp_efuse_disable_rom_download_mode`。
   - 启用 :doc:`安全启动<secure-boot-v2>` 作为额外的保护层，防止攻击者在启动前有选择地破坏 flash 中某部分。

外部启用 flash 加密
----------------------------------

在上述过程中，对与 flash 加密相关的 eFuse 是通过固件引导加载程序烧写的，或者，也可以借助 ``espefuse`` 工具烧写 eFuse。如需了解详情，请参考 :ref:`enable-flash-encryption-externally`。

可能出现的错误
-----------------

一旦启用 flash 加密，``{IDF_TARGET_CRYPT_CNT}`` 的 eFuse 值将设置为奇数位。这意味着所有标有加密标志的分区都会包含加密的密本。如果 {IDF_TARGET_NAME} 错误地加载了明文数据，则会出现以下三种典型的错误情况：

1. 如果通过 **明文固件引导加载程序镜像** 重新烧录了引导加载程序分区，则 ROM 加载器将无法加载固件引导加载程序，并会显示以下错误类型：

.. only:: esp32

  .. code-block:: bash

      rst:0x3 (SW_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
      flash read err, 1000
      ets_main.c 371
      ets Jun  8 2016 00:22:57

      rst:0x7 (TG0WDT_SYS_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
      flash read err, 1000
      ets_main.c 371
      ets Jun  8 2016 00:22:57

      rst:0x7 (TG0WDT_SYS_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
      flash read err, 1000
      ets_main.c 371
      ets Jun  8 2016 00:22:57

      rst:0x7 (TG0WDT_SYS_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
      flash read err, 1000
      ets_main.c 371
      ets Jun  8 2016 00:22:57

      rst:0x7 (TG0WDT_SYS_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
      flash read err, 1000
      ets_main.c 371
      ets Jun  8 2016 00:22:57

.. only:: not esp32

  .. code-block:: bash

      rst:0x3 (SW_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
      invalid header: 0xb414f76b
      invalid header: 0xb414f76b
      invalid header: 0xb414f76b
      invalid header: 0xb414f76b
      invalid header: 0xb414f76b
      invalid header: 0xb414f76b
      invalid header: 0xb414f76b

  .. note::

    不同应用程序中无效头文件的值不同。

.. note::

    如果 flash 内容被擦除或损坏，也会出现这个错误。

2. 如果固件的引导加载程序已加密，但通过 **明文分区表镜像** 重新烧录了分区表，引导加载程序将无法读取分区表，从而出现以下错误：

  .. code-block:: bash

      rst:0x3 (SW_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
      configsip: 0, SPIWP:0xee
      clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
      mode:DIO, clock div:2
      load:0x3fff0018,len:4
      load:0x3fff001c,len:10464
      ho 0 tail 12 room 4
      load:0x40078000,len:19168
      load:0x40080400,len:6664
      entry 0x40080764
      I (60) boot: ESP-IDF v4.0-dev-763-g2c55fae6c-dirty 2nd stage bootloader
      I (60) boot: compile time 19:15:54
      I (62) boot: Enabling RNG early entropy source...
      I (67) boot: SPI Speed      : 40MHz
      I (72) boot: SPI Mode       : DIO
      I (76) boot: SPI Flash Size : 4MB
      E (80) flash_parts: partition 0 invalid magic number 0x94f6
      E (86) boot: Failed to verify partition table
      E (91) boot: load partition table error!

3. 如果引导加载程序和分区表已加密，但使用 **明文应用程序镜像** 重新烧录了应用程序，引导加载程序将无法加载应用程序，从而出现以下错误：

  .. code-block:: bash

      rst:0x3 (SW_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
      configsip: 0, SPIWP:0xee
      clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
      mode:DIO, clock div:2
      load:0x3fff0018,len:4
      load:0x3fff001c,len:8452
      load:0x40078000,len:13616
      load:0x40080400,len:6664
      entry 0x40080764
      I (56) boot: ESP-IDF v4.0-dev-850-gc4447462d-dirty 2nd stage bootloader
      I (56) boot: compile time 15:37:14
      I (58) boot: Enabling RNG early entropy source...
      I (64) boot: SPI Speed      : 40MHz
      I (68) boot: SPI Mode       : DIO
      I (72) boot: SPI Flash Size : 4MB
      I (76) boot: Partition Table:
      I (79) boot: ## Label            Usage          Type ST Offset   Length
      I (87) boot:  0 nvs              WiFi data        01 02 0000a000 00006000
      I (94) boot:  1 phy_init         RF data          01 01 00010000 00001000
      I (102) boot:  2 factory          factory app      00 00 00020000 00100000
      I (109) boot: End of partition table
      E (113) esp_image: image at 0x20000 has invalid magic byte
      W (120) esp_image: image at 0x20000 has invalid SPI mode 108
      W (126) esp_image: image at 0x20000 has invalid SPI size 11
      E (132) boot: Factory app partition is not bootable
      E (138) boot: No bootable app partitions in the partition table


.. _flash-encryption-status:

{IDF_TARGET_NAME} flash 加密状态
-----------------------------------------

1. 确保你的 {IDF_TARGET_NAME} 设备有 :ref:`flash-encryption-efuse` 中所示的 flash 加密 eFuse 的默认设置。

要检查你的 {IDF_TARGET_NAME} 设备上是否启用了 flash 加密，请执行以下操作之一：

- 将应用示例 :example:`security/flash_encryption` 烧录到你的设备上。此应用程序会打印 ``{IDF_TARGET_CRYPT_CNT}`` eFuse 值，以及是否启用了 flash 加密。

- :doc:`查询设备所连接的串口名称 <../get-started/establish-serial-connection>`，在以下命令中将 ``PORT`` 替换为串口名称后运行：

  .. code-block:: bash

      espefuse.py -p PORT summary


.. _reading-writing-content:

在加密的 flash 中读写数据
-------------------------------------

{IDF_TARGET_NAME} 应用程序代码可以通过调用函数 :cpp:func:`esp_flash_encryption_enabled` 来检查当前是否启用了 flash 加密。此外，设备可以通过调用函数 :cpp:func:`esp_get_flash_encryption_mode` 来识别 flash 加密模式。

一旦启用 flash 加密，使用代码访问 flash 内容时要更加小心。


flash 加密范围
^^^^^^^^^^^^^^^^^^^^^^^^^

当 ``{IDF_TARGET_CRYPT_CNT}`` eFuse 设置为奇数位的值，所有通过 MMU 的 flash 缓存访问的 flash 内容都将被透明解密。包括：

- flash 中可执行的应用程序代码 (IROM)。
- 所有存储于 flash 中的只读数据 (DROM)。
- 通过函数 :cpp:func:`spi_flash_mmap` 访问的任意数据。
- ROM 引导加载程序读取的固件引导加载程序镜像。

.. important::

    MMU flash 缓存将无条件解密所有数据。flash 中未加密存储的数据将通过 flash 缓存“被透明解密”，并在软件中存储为随机垃圾数据。


读取加密的 flash
^^^^^^^^^^^^^^^^^^

如果需要在不使用 flash 缓存 MMU 映射的情况下读取数据，推荐使用分区读取函数 :cpp:func:`esp_partition_read`。该函数只会解密从加密分区读取的数据。从未加密分区读取的数据不会被解密。这样，软件便能以相同的方式访问加密和未加密的 flash。

也可以使用以下 SPI flash API 函数:

- 通过函数 :cpp:func:`esp_flash_read` 读取不会被解密的原（加密）数据。
- 通过函数 :cpp:func:`esp_flash_read_encrypted` 读取和解密数据。

使用非易失性存储器 (NVS) API 存储的数据始终从 flash 加密的角度进行存储和读取解密。如有需要，则由库提供加密功能。详情可参考 :doc:`../api-reference/storage/nvs_encryption`。


写入加密的 flash
^^^^^^^^^^^^^^^^^^

推荐使用分区写入函数 :cpp:func:`esp_partition_write`。此函数只会在将数据写入加密分区时加密数据，而写入未加密分区的数据不会被加密。通过这种方式，软件可以以相同的方式访问加密和非加密 flash。

也可以使用函数 :cpp:func:`esp_flash_write_encrypted` 预加密和写入数据。

此外，esp-idf 应用程序中存在但不支持以下 ROM 函数：

- ``esp_rom_spiflash_write_encrypted`` 预加密并将数据写入 flash
- ``SPIWrite`` 将未加密的数据写入 flash

由于数据是按块加密的，加密数据最小的写入大小为 16 字节，对齐也是 16 字节。


.. _updating-encrypted-flash:

更新加密的 flash
-------------------

.. _updating-encrypted-flash-ota:

OTA 更新
^^^^^^^^^^

如果使用函数 :cpp:func:`esp_partition_write`，对加密分区的 OTA 更新将自动以加密形式写入。

在为已加密设备的 OTA 更新构建应用程序镜像之前，启用项目配置菜单中的 :ref:`启动时使能 flash 加密 <CONFIG_SECURE_FLASH_ENC_ENABLED>` 选项。

请参考 :doc:`OTA <../api-reference/system/ota>` 获取更多关于 ESP-IDF OTA 更新的信息。


.. _updating-encrypted-flash-serial:

通过串口更新加密 flash
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

通过串行引导加载程序烧录加密设备，需要串行引导加载程序下载接口没有通过 eFuse 被永久禁用。

在开发模式下，推荐的方法是 :ref:`encrypt-partitions`。

在发布模式下，如果主机上有存储在 eFuse 中的相同密钥的副本，那么就可以在主机上对文件进行预加密，然后进行烧录，具体请参考 :ref:`manual-encryption`。

关闭 flash 加密
-----------------

如果意外启用了 flash 加密，则明文数据的 flash 会使 {IDF_TARGET_NAME} 无法正常启动。设备将不断重启，并报错 ``flash read err, 1000`` 或 ``invalid header: 0xXXXXXX``。

.. only:: esp32

  对于开发模式下的 flash 加密，可以通过烧录 ``{IDF_TARGET_CRYPT_CNT}`` efuse 来关闭加密。每个芯片仅有 3 次机会，请执行以下步骤：

.. only:: not esp32

  对于开发模式下的 flash 加密，可以通过烧录 ``{IDF_TARGET_CRYPT_CNT}`` efuse 来关闭加密。每个芯片仅有 1 次机会，请执行以下步骤：

#. 在 :ref:`项目配置菜单 <project-configuration-menu>` 中，禁用 :ref:`启动时使能 flash 加密 <CONFIG_SECURE_FLASH_ENC_ENABLED>` 选项，然后保存并退出。
#. 再次打开项目配置菜单，再次检查你是否已经禁用了该选项，如果这个选项仍被启用，引导加载程序在启动时将立即重新启用加密功能。
#. 在禁用 flash 加密后，通过运行 ``idf.py flash`` 来构建和烧录新的引导加载程序和应用程序。
#. 使用 ``espefuse.py`` （在 ``components/esptool_py/esptool`` 中）以关闭 ``{IDF_TARGET_CRYPT_CNT}``，运行:

  .. code-block:: bash

      espefuse.py burn_efuse {IDF_TARGET_CRYPT_CNT}

重置 {IDF_TARGET_NAME}，flash 加密应处于关闭状态，引导加载程序将正常启动。


flash 加密的要点
---------------------------------

.. list::

  :esp32: - 使用 AES-256 加密 flash。flash 加密密钥存储于芯片内部的 ``flash_encryption`` eFuse 中，并（默认）受保护，防止软件访问。

  :esp32: - flash 加密算法采用的是 AES-256，其中密钥随着 flash 的每个 32 字节块的偏移地址“调整”。这意味着，每个 32 字节块（2 个连续的 16 字节 AES 块）使用从 flash 加密密钥中产生的一个特殊密钥进行加密。

  :esp32s2 or esp32s3: - 使用 XTS-AES-128 或 XTS-AES-256 加密 flash。flash 加密密钥分别为 256 位和 512 位，存储于芯片内部一个或两个 ``BLOCK_KEYN`` eFuse 中，并（默认）受保护，防止软件访问。

  :esp32c3: - 使用 XTS-AES-128 加密 flash。 flash 加密密钥为 256 位，存储于芯片内部的 ``BLOCK_KEYN`` eFuse 中，并（默认）受保护，防止软件访问。

  :esp32c2: - 使用 XTS-AES-128 加密 flash。 flash 加密密钥为 128 位或 256 位，存储于芯片内部的 ``BLOCK_KEY0`` eFuse 中，并（默认）受保护，防止软件访问。

  - 通过 {IDF_TARGET_NAME} 的 flash 缓存映射功能，flash 可支持透明访问——任何映射到地址空间的 flash 区域在读取时都将被透明地解密。

    为便于访问，某些数据分区最好保持未加密状态，或者也可使用对已加密数据无效的 flash 友好型更新算法。由于 NVS 库无法与 flash 加密直接兼容，因此无法加密非易失性存储器的 NVS 分区。详情可参见 :ref:`NVS 加密 <nvs_encryption>`。

  - 如果以后可能需要启用 flash 加密，则编程人员在编写 :ref:`使用加密 flash <reading-writing-content>` 代码时需小心谨慎。

  - 如果已启用安全启动，重新烧录加密设备的引导加载程序则需要“可重新烧录”的安全启动摘要（可参考 :ref:`flash-encryption-and-secure-boot`）。

  启用 flash 加密将增大引导加载程序，因此可能需更新分区表偏移量。请参考 :ref:`引导加载程序大小 <bootloader-size>`。

    .. important::

        在首次启动加密过程中，请勿切断 {IDF_TARGET_NAME} 的电源。如果电源被切断，flash 的内容将受到破坏，并需要重新烧录未加密数据。而这类重新烧录将不计入烧录限制次数。


.. _flash-encryption-limitations:

flash 加密的局限性
--------------------

flash 加密可以保护固件，防止未经授权的读取与修改。了解 flash 加密系统的局限之处亦十分重要：

.. list::

    - flash 加密功能与密钥同样稳固。因而，推荐在首次启动设备时，在设备上生成密钥（默认行为）。如果在设备外生成密钥，请确保遵循正确的后续步骤，不要在所有生产设备之间使用相同的密钥。
    - 并非所有数据都是加密存储。因而在 flash 上存储数据时，请检查你使用的存储方式（库、API 等）是否支持 flash 加密。
    - flash 加密无法防止攻击者获取 flash 的高层次布局信息。这是因为每对相邻的 16 字节 AES 块都使用相邻的 AES 密钥。当这些相邻的 16 字节块中包含相同内容时（如空白或填充区域），这些字节块将加密以产生匹配的加密块对。这让攻击者可在加密设备间进行高层次对比（例如，确认两设备是否可能运行相同的固件版本）。
    :esp32: - 出于相同原因，攻击者始终可获知一对相邻的 16 字节块（32 字节对齐）何时包含相同的 16 字节序列。因此，在 flash 上存储敏感数据时应牢记这点，可进行相关设置避免该情况发生（可使用计数器字节或每 16 字节设置不同的值即可）。具体请参考 :ref:`NVS 加密 <nvs_encryption>`。
    - 单独使用 flash 加密可能无法防止攻击者修改本设备的固件。为防止设备上运行未经授权的固件，可搭配 flash 加密使用 :doc:`安全启动 <secure-boot-v2>`。

.. _flash-encryption-and-secure-boot:

flash 加密与安全启动
----------------------

推荐 flash 加密与安全启动搭配使用。但是，如果已启用安全启动，则重新烧录设备时会受到其他限制：

- 如果新的应用程序已使用安全启动签名密钥正确签名，则 :ref:`updating-encrypted-flash-ota` 不受限制。

.. only:: esp32

    - 只有当选择 :ref:`可再次烧录 <CONFIG_SECURE_BOOTLOADER_MODE>` 安全启动模式，且安全启动密钥已预生成并烧录至 {IDF_TARGET_NAME} 时（可参见 :ref:`安全启动 <secure-boot-reflashable>`），:ref:`明文串行 flash 更新 <updating-encrypted-flash-serial>` 才可能实现。在该配置下，``idf.py bootloader`` 将生成简化的引导加载程序和安全启动摘要文件，在偏移量 0x0 处进行烧录。当进行明文串行重新烧录步骤时，需在烧录其他明文数据前重新烧录此文件。

    - 如果未重新烧录引导加载程序，则仍然可以 :ref:`使用预生成的 flash 加密密钥重新烧录 <pregenerated-flash-encryption-key>`。重新烧录引导加载程序时，需在安全启动配置中启用相同的 :ref:`可重新烧录 <CONFIG_SECURE_BOOTLOADER_MODE>` 选项。

.. _flash-encryption-without-secure-boot:

flash 加密的高级功能
--------------------------------

以下部分介绍了 flash 加密的高级功能。

.. _encrypted-partition-flag:

加密分区标志
^^^^^^^^^^^^^

部分分区默认为已加密。通过在分区的标志字段中添加 “encrypted” 标志，可在分区表描述中将其他分区标记为需要加密。在这些标记分区中的数据会和应用程序分区一样视为加密数据。

.. code-block:: bash

   # Name,   Type, SubType, Offset,  Size, Flags
   nvs,      data, nvs,     0x9000,  0x6000
   phy_init, data, phy,     0xf000,  0x1000
   factory,  app,  factory, 0x10000, 1M
   secret_data, 0x40, 0x01, 0x20000, 256K, encrypted

请参考 :doc:`分区表 <../api-guides/partition-tables>` 获取更多关于分区表描述的具体信息。

关于分区加密，还需了解以下信息：

- 默认分区表都不包含任何加密数据分区。
- 启用 flash 加密后，"app" 分区一般都视为加密分区，因此无需标记。
- 如果未启用 flash 加密，则 "encrypted" 标记无效。
- 将可选 ``phy`` 分区标记为 "encrypted"，可以防止物理访问读取或修改 ``phy_init`` 数据。
- ``nvs`` 分区无法标记为 "encrypted" 因为 NVS 库与 flash 加密不直接兼容。


.. _uart-bootloader-encryption:

启用 UART 引导加载程序加密/解密
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

在第一次启动时，flash 加密过程默认会烧录以下 eFuse：

.. only:: esp32

    - ``DISABLE_DL_ENCRYPT`` 在 UART 引导加载程序启动模式下运行时，禁止 flash 加密操作。
    - ``DISABLE_DL_DECRYPT`` 在 UART 引导加载程序模式下运行时，禁止透明 flash 解密（即使 eFuse ``{IDF_TARGET_CRYPT_CNT}`` 已设置为在正常操作中启用 flash 透明解密）。
    - ``DISABLE_DL_CACHE`` 在 UART 引导加载程序模式下运行时禁止整个 MMU flash 缓存。

.. only:: not esp32

  .. list::

    - ``DIS_DOWNLOAD_MANUAL_ENCRYPT`` 在 UART 引导加载程序启动模式下运行时，禁止 flash 加密操作。
    :esp32s2 or esp32s3: - ``DIS_DOWNLOAD_ICACHE`` 和 ``DIS_DOWNLOAD_DCACHE`` 在 UART 引导加载程序模式下运行时禁止整个 MMU flash 缓存。
    :esp32c3 or esp32c2: - ``DIS_DOWNLOAD_ICACHE`` 在 UART 引导加载程序模式下运行时禁止整个 MMU flash 缓存。
    :esp32s2: - ``HARD_DIS_JTAG`` 禁用 JTAG。
    :esp32c3: - ``DIS_PAD_JTAG`` 和 ``DIS_USB_JTAG`` 禁用 JTAG。
    :esp32s3: - ``HARD_DIS_JTAG`` 和 ``DIS_USB_JTAG`` 禁用 JTAG。
    - ``DIS_DIRECT_BOOT``（即之前的 ``DIS_LEGACY_SPI_BOOT``）禁用传统的 SPI 启动模式。

为了能启用这些功能，可在首次启动前仅烧录部分 eFuse，并用未设置值 0 写保护其他部分。例如:

.. only:: esp32

  .. code-block:: bash

    espefuse.py --port PORT burn_efuse DISABLE_DL_DECRYPT
    espefuse.py --port PORT write_protect_efuse DISABLE_DL_ENCRYPT

.. only:: not esp32

  .. code-block:: bash

    espefuse.py --port PORT burn_efuse DIS_DOWNLOAD_MANUAL_ENCRYPT
    espefuse.py --port PORT write_protect_efuse DIS_DOWNLOAD_MANUAL_ENCRYPT

  .. note::

      请注意在写保护前设置所有适当的位！

      一个位可以控制三个 eFuse 的写保护，这意味着写保护一个 eFuse 位将写保护所有未设置的 eFuse 位。

  由于 ``esptool.py`` 目前不支持读取加密 flash，所以对这些 eFuse 进行写保护从而使其保持未设置目前来说并不是很有用。

.. only:: esp32

  .. important::

      保持 ``DISABLE_DL_DECRYPT`` 未设置 (0) 会使 flash 加密无效。

      对芯片具有物理访问权限的攻击者会使用 UART 引导加载程序模式（使用自定义存根代码）读取 flash 的内容。


.. only:: esp32

    .. _setting-flash-crypt-config:

    设置 FLASH_CRYPT_CONFIG
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^

    ``FLASH_CRYPT_CONFIG`` eFuse 决定 flash 加密密钥中随块偏移“调整”的位数。详情可参考 :ref:`flash-encryption-algorithm`。

    首次启动固件引导加载程序时，该值始终设置为最大值 `0xF`。

    可手动写入这些 eFuse，并在首次启动前对其写保护，以便选择不同的调整值。但不推荐该操作。

    强烈建议在 ``FLASH_CRYPT_CONFIG`` 未设置时，不要对其进行写保护。否则，它的值将永久为零，而 flash 加密密钥中则无调整位。这导致 flash 加密算法等同于 AES ECB 模式。

JTAG 调试
^^^^^^^^^^^^^^

默认情况下，当启用 flash 加密（开发或发布模式）时，将通过 eFuse 禁用 JTAG 调试。引导加载程序在首次启动时执行此操作，同时启用 flash 加密。

请参考 :ref:`jtag-debugging-security-features` 了解更多关于使用 JTAG 调试与 flash 加密的信息。


.. _manual-encryption:

手动加密文件
^^^^^^^^^^^^^^^^^^^^

手动加密或解密文件需要在 eFuse 中预烧录 flash 加密密钥（请参阅 :ref:`pregenerated-flash-encryption-key`）并在主机上保留一份副本。 如果 flash 加密配置在开发模式下，那么则不需要保留密钥的副本或遵循这些步骤，可以使用更简单的 :ref:`encrypt-partitions` 步骤。

密钥文件应该是单个原始二进制文件（例如：``key.bin``）。

例如，以下是将文件 ``build/my-app.bin`` 进行加密、烧录到偏移量 0x10000 的步骤。运行 ``espsecure.py``，如下所示:

.. only:: esp32

    .. code-block:: bash

       espsecure.py encrypt_flash_data --keyfile /path/to/key.bin --address 0x10000 --output my-app-ciphertext.bin build/my-app.bin

.. only:: not esp32

    .. code-block:: bash

       espsecure.py encrypt_flash_data --aes_xts --keyfile /path/to/key.bin --address 0x10000 --output my-app-ciphertext.bin build/my-app.bin

然后可以使用 ``esptool.py`` 将文件 ``my-app-ciphertext.bin`` 写入偏移量 0x10000。 关于为 ``esptool.py`` 推荐的所有命令行选项，请查看 idf.py build 成功时打印的输出。

.. note::

    如果 {IDF_TARGET_NAME} 在启动时无法识别烧录进去的密文文件，请检查密钥是否匹配以及命令行参数是否完全匹配，包括偏移量是否正确。

  .. only:: esp32

    如果你的 ESP32 在 eFuse 中使用了非默认的 :ref:`FLASH_CRYPT_CONFIG 值 <setting-flash-crypt-config>`，那么则需要向 ``espsecure.py`` 传递 ``--flash_crypt_conf`` 参数以设置匹配的值。如果设备自己设置了 flash 加密就不会出现这种情况，但如果手动烧录 eFuse 来启用 flash 加密就可能发生这种情况。

``espsecure.py decrypt_flash_data`` 命令可以使用同样的选项（和不同的输入/输出文件）来解密 flash 密文或之前加密的文件。


.. only:: SOC_SPIRAM_SUPPORTED and not esp32

    片外 RAM
    ------------

    启用 flash 加密后，任何通过缓存从片外 SPI RAM 读取和写入的数据也将被加密/解密。这个实现的方式以及使用的密钥与 flash 加密相同。如果启用 flash 加密，则片外 SPI RAM 的加密也会被启用，无法单独控制此功能。


技术细节
-----------

以下章节将提供 flash 加密操作的相关信息。

.. only:: not SOC_FLASH_ENCRYPTION_XTS_AES

  .. _flash-encryption-algorithm:

  flash 加密算法
  ^^^^^^^^^^^^^^^^

  - AES-256 在 16 字节的数据块上运行。flash 加密引擎在 32 字节的数据（2 个 串行 AES 块）上加密或解密数据。

  - flash 加密的主密钥存储于 ``flash_encryption`` eFuse 中，默认受保护防止进一步写入或软件读取。

  - AES-256 密钥大小为 256 位（32 字节），从 ``flash_encryption`` eFuse 中读取。与 ``flash_encryption`` 中的存储顺序相比，硬件 AES 引擎使用的是相反的字节顺序的密钥。

    - 如果 ``CODING_SCHEME`` eFuse 设置为 0（默认“无”编码方案），则 eFuse 密钥块为 256 位，且密钥按原方式存储（反字节序）。
    - 如果 ``CODING_SCHEME`` eFuse 设置为 1（3/4 编码），则 eFuse 密钥块为 192 位（反字节序），信息熵总量减少。硬件 flash 加密仍在 256 字节密钥上运行，在读取后（字节序未反向），密钥扩展为 ``key = key[0:255] + key[64:127]``。

  - flash 加密中使用了逆向 AES 算法，因此 flash 加密的“加密”操作相当于 AES 解密，而其“解密”操作则相当于 AES 加密。这是为了优化性能，不会影响算法的有效性。

  - 每个 32 字节块（2 个相邻的 16 字节 AES 块）都由一个特殊的密钥进行加密。该密钥由 ``flash_encryption`` 中 flash 加密的主密钥产生，并随 flash 中该字节块的偏移进行 XOR 运算（一次“密钥调整”）。

  - 具体调整量取决于 ``FLASH_CRYPT_CONFIG`` eFuse 的设置。该 eFuse 共 4 位，每位可对特定范围的密钥位进行 XOR 运算：

    - Bit 1，对密钥的 0-66 位进行 XOR 运算。
    - Bit 2，对密钥的 67-131 位进行 XOR 运算。
    - Bit 3，对密钥的 132-194 位进行 XOR 运算。
    - Bit 4，对密钥的 195-256 位进行 XOR 运算。

    建议将 ``FLASH_CRYPT_CONFIG`` 的值始终保留为默认值 ``0xF``，这样所有密钥位都随块偏移进行 XOR 运算。详情可参见 :ref:`setting-flash-crypt-config`。

  - 块偏移的 19 个高位（第 5-23 位）由 flash 加密的主密钥进行 XOR 运算。选定该范围的原因为：flash 的大小最大为 16 MB（24 位），每个块大小为 32 字节，因而 5 个最低有效位始终为 0。

  - 从 19 个块偏移位中每个位到 flash 加密密钥的 256 位都有一个特殊的映射，以决定与哪个位进行 XOR 运算。有关完整映射可参见 ``espsecure.py`` 源代码中的变量 ``_FLASH_ENCRYPTION_TWEAK_PATTERN``。

- 有关在 Python 中实现的完整 flash 加密算法，可参见 ``espsecure.py`` 源代码中的函数 ``_flash_encryption_operation()``。

.. only:: SOC_FLASH_ENCRYPTION_XTS_AES_256

  .. _flash-encryption-algorithm:

  flash 加密算法
  ^^^^^^^^^^^^^^^^^^^^^^^^^^

  - {IDF_TARGET_NAME} 使用 XTS-AES 块密码模式进行 flash 加密，密钥大小为 256 位或 512 位。

  - XTS-AES 是一种专门为光盘加密设计的块密码模式，它解决了其它潜在模式如 AES-CTR 在此使用情景下的不足。有关 XTS-AES 算法的详细描述，请参考 `IEEE Std 1619-2007 <https://ieeexplore.ieee.org/document/4493450>`_。

  - flash 加密的密钥存储于一个或两个 ``BLOCK_KEYN`` eFuse 中，默认受保护防止进一步写入或软件读取。

  - 有关在 Python 中实现的完整 flash 加密算法，可参见 ``espsecure.py`` 源代码中的函数 ``_flash_encryption_operation()``。

.. only:: SOC_FLASH_ENCRYPTION_XTS_AES_128 and not SOC_FLASH_ENCRYPTION_XTS_AES_256 and not SOC_EFUSE_CONSISTS_OF_ONE_KEY_BLOCK


  .. _flash-encryption-algorithm:

  flash 加密算法
  ^^^^^^^^^^^^^^^^^^^^^^^^^^

  - {IDF_TARGET_NAME} 使用 XTS-AES 块密码模式进行 flash 加密，密钥大小为 256 位。

  - XTS-AES 是一种专门为光盘加密设计的块密码模式，它解决了其它潜在模式如 AES-CTR 在此使用情景下的不足。有关 XTS-AES 算法的详细描述，请参考 `IEEE Std 1619-2007 <https://ieeexplore.ieee.org/document/4493450>`_。

  - flash 加密的密钥存储于一个 ``BLOCK_KEYN`` eFuse 中，默认受保护防止进一步写入或软件读取。

  - 有关在 Python 中实现的完整 flash 加密算法，可参见 ``espsecure.py`` 源代码中的函数 ``_flash_encryption_operation()``。

.. only:: SOC_FLASH_ENCRYPTION_XTS_AES_128 and SOC_EFUSE_CONSISTS_OF_ONE_KEY_BLOCK

  .. _flash-encryption-algorithm:

  flash 加密算法
  ^^^^^^^^^^^^^^^^^^^^^^^^^^

  - {IDF_TARGET_NAME} 使用 XTS-AES 块密码模式进行 flash 加密，密钥大小为 256 位。如果 128 位的密钥存储于 eFuse 密钥块中，那么最终的 256 位 AES 密钥将以 SHA256(EFUSE_KEY0_FE_128BIT) 的形式获得。

  - XTS-AES 是一种专门为光盘加密设计的块密码模式，它弥补了其他潜在模式如 AES-CTR 在此使用场景下的不足。有关 XTS-AES 算法的详细描述，请参考 `IEEE Std 1619-2007 <https://ieeexplore.ieee.org/document/4493450>`_。

  - flash 加密的密钥存储于一个 ``BLOCK_KEY0`` eFuse 中，默认受保护防止进一步写入或软件读取。

  - 有关在 Python 中实现的完整 flash 加密算法，可参见 ``espsecure.py`` 源代码中的函数 ``_flash_encryption_operation()``。
