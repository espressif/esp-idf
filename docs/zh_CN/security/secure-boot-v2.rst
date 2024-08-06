:orphan:

安全启动 (secure boot) v2
============================

:link_to_translation:`en:[English]`

{IDF_TARGET_SBV2_SCHEME:default="RSA-PSS", esp32c2="ECDSA", esp32c6="RSA-PSS 或 ECDSA", esp32h2="RSA-PSS 或 ECDSA", esp32p4="RSA-PSS 或 ECDSA", esp32c5="RSA-PSS 或 ECDSA"}

{IDF_TARGET_SBV2_KEY:default="RSA-3072", esp32c2="ECDSA-256 或 ECDSA-192", esp32c6="RSA-3072、ECDSA-256 或 ECDSA-192", esp32h2="RSA-3072、ECDSA-256 或 ECDSA-192", esp32p4="RSA-3072、ECDSA-256 或 ECDSA-192", esp32c5="RSA-3072、ECDSA-256、或 ECDSA-192"}

{IDF_TARGET_SECURE_BOOT_OPTION_TEXT:default="", esp32c6="推荐使用 RSA，其验证时间更短。可以在菜单中选择 RSA 和 ECDSA 方案。", esp32h2="推荐使用 RSA，其验证时间更短。可以在菜单中选择 RSA 和 ECDSA 方案。", esp32p4="推荐使用 RSA，其验证时间更短。可以在菜单中选择 RSA 和 ECDSA 方案。", esp32c5="推荐使用 RSA，其验证时间更短。可以在菜单中选择 RSA 和 ECDSA 方案。"}

{IDF_TARGET_ECO_VERSION:default="", esp32="（v3.0 及以上版本）", esp32c3="（v0.3 及以上版本）"}

{IDF_TARGET_RSA_TIME:default="", esp32c6="约 2.7 ms", esp32h2="约 4.5 ms", esp32p4="约 2.4 ms"}

{IDF_TARGET_ECDSA_TIME:default="", esp32c6="约 21.5 ms", esp32h2="约 36 ms", esp32p4="约 10.3 ms"}

{IDF_TARGET_CPU_FREQ:default="", esp32c6="160 MHz", esp32h2="96 MHz", esp32p4="360 MHz"}

{IDF_TARGET_SBV2_DEFAULT_SCHEME:default="RSA", esp32c2="ECDSA (v2)"}

{IDF_TARGET_EFUSE_WR_DIS_RD_DIS:default="ESP_EFUSE_WR_DIS_RD_DIS", esp32="ESP_EFUSE_WR_DIS_EFUSE_RD_DISABLE"}

.. important::

    本文档介绍了安全启动 v2，{IDF_TARGET_NAME} {IDF_TARGET_ECO_VERSION} 支持该启动模式。

    .. only:: esp32

        芯片版本低于 v3.0 的 ESP32 安全启动请参阅 :doc:`secure-boot-v1`。如果你的芯片版本支持安全启动 v2，推荐使用此模式，相比安全启动 v1 更安全且灵活。

    安全启动 v2 使用基于 {IDF_TARGET_SBV2_SCHEME} 的应用程序和引导加载程序 (bootloader) :ref:`second-stage-bootloader` 验证。若需要使用 {IDF_TARGET_SBV2_SCHEME} 方案对应用程序签名，且无需对 bootloader 签名，同样可以参考本文档。

.. only:: esp32

    ESP32 v3.0 及以上芯片版本支持 ``Secure Boot v2`` 和 ``RSA App Signing Scheme`` 选项，可通过 :ref:`CONFIG_ESP32_REV_MIN` 设置芯片版本为 `v3.0` 及以上启用这两个选项。

.. only:: esp32c3

    自 ESP32-C3 芯片版本 v0.3 起，提供了 ``Secure Boot v2`` 选项。请设置 :ref:`CONFIG_ESP32C3_REV_MIN` 为高于或等于 `v0.3`，以在 menuconfig 中使用上述选项。


.. note::

    在本指南中，最常用的命令形式为 ``idf.py secure-<command>``，这是对应 ``espsecure.py <command>`` 的封装。基于 ``idf.py`` 的命令能提供更好的用户体验，但与基于 ``espsecure.py`` 的命令相比，可能会损失一部分高级功能。

背景
----------

安全启动通过检查每个启动的软件是否已签名来确保设备不会运行任何未经授权（即未签名）的代码。在 {IDF_TARGET_NAME} 上，这些软件包括第二阶段的 bootloader 和每个应用程序的二进制文件。注意，第一阶段的 bootloader 是无法更改的 ROM 代码，因此不需要签名。

.. only:: esp32 or (SOC_SECURE_BOOT_V2_RSA and not SOC_SECURE_BOOT_V2_ECC)

    {IDF_TARGET_NAME} {IDF_TARGET_ECO_VERSION} 使用基于 RAS 的安全启动验证方案，即安全启动 v2。

.. only:: SOC_SECURE_BOOT_V2_ECC and not SOC_SECURE_BOOT_V2_RSA

    {IDF_TARGET_NAME} 引入了基于 ECC 的安全启动验证方案，即安全启动 v2。

.. only:: SOC_SECURE_BOOT_V2_RSA and SOC_SECURE_BOOT_V2_ECC

    {IDF_TARGET_NAME} 可以选择基于 {IDF_TARGET_SBV2_SCHEME} 的安全启动验证方案。

{IDF_TARGET_NAME} 的安全启动包括以下步骤：

1. 第一阶段 bootloader (ROM boot) 仍处于 ROM 中，加载第二阶段 bootloader，并验证第二阶段 bootloader 的 {IDF_TARGET_SBV2_SCHEME} 签名。验证通过后方可进入第二阶段。

2. 第二阶段 bootloader 加载特定应用程序镜像时，会验证应用程序的 {IDF_TARGET_SBV2_SCHEME} 签名。若验证通过，则执行应用程序镜像。


优势
----------

- {IDF_TARGET_SBV2_SCHEME} 的公钥存储在设备上，而相应的 {IDF_TARGET_SBV2_SCHEME} 私钥存储在私密位置，设备无法访问。

.. only:: esp32 or esp32c2

    - 芯片在量产时只能生成并存储一个公钥。

.. only:: SOC_EFUSE_REVOKE_BOOT_KEY_DIGESTS

    - 芯片在量产时最多能生成并存储三个公钥。

    - {IDF_TARGET_NAME} 支持永久注销个别公钥，对此可以选择保守或激进的配置。

      - 保守配置：在此情况下，只有在 bootloader 和应用程序成功迁移到新密钥后才会注销旧密钥。
      - 激进配置：在此情况下，只要使用此密钥验证失败，就会立即注销该密钥。

- 应用程序和软件 bootloader 采用相同的镜像格式和签名验证方法。

- 设备不存储任何机密信息，因此可以免受被动侧通道攻击的影响，如时序分析或功耗分析。


使用安全启动 v2
----------------------

以下为使用安全启动 v2 流程的概述。有关如何启用安全启动，请参阅 :ref:`secure-boot-v2-howto`。

安全启动 v2 使用专用的 *签名块* 验证 bootloader 镜像和应用程序二进制镜像，每个镜像末尾都附加了一个单独生成的签名块。

.. only:: esp32

  在 ESP32 芯片版本 v3.0 中，bootloader 或应用程序镜像只能附加一个签名块。

.. only:: esp32c2

  在 {IDF_TARGET_NAME} 中，bootloader 或应用程序镜像只能附加一个签名块。

.. only:: SOC_EFUSE_REVOKE_BOOT_KEY_DIGESTS

  在 {IDF_TARGET_NAME} 中，bootloader 或应用程序镜像至多可以附加三个签名块。

每个签名块包含前一个镜像的签名和相应的 {IDF_TARGET_SBV2_KEY} 公钥。有关格式详情，请参阅 :ref:`signature-block-format`。{IDF_TARGET_SBV2_KEY} 公钥的摘要存储在 eFuse 中。

应用程序镜像不仅在每次启动时验证，也会在每次空中升级 (OTA) 时验证。如果当前所选 OTA 应用程序镜像无法验证，bootloader 将回退，并寻找其他正确签名的应用程序镜像。

安全启动 v2 流程遵循以下步骤：

1. 启动时，ROM 代码检查 eFuse 中的安全启动 v2 位。如果禁用了安全启动，则执行普通启动；如果启用了安全启动，将继续以下步骤。

2. ROM 代码验证 bootloader 的签名块，请参阅 :ref:`verify_signature-block`。如果验证失败，启动过程将中止。

3. ROM 代码使用原始镜像数据、相应的签名块以及 eFuse 验证 bootloader 镜像，请参阅 :ref:`verify_image`。如果验证失败，启动过程将中止。

4. ROM 代码执行 bootloader 。

5. bootloader 验证应用程序镜像的签名块，请参阅 :ref:`verify_signature-block`。如果验证失败，启动过程将中止。

6. bootloader 使用原始镜像数据、相应的签名块以及 eFuse 验证 bootloader 镜像，请参阅 :ref:`verify_image`。如果验证失败，启动过程将中止。如果验证失败，但发现了其他应用程序镜像， bootloader 将使用步骤 5 到 7 验证另一个镜像。该过程将重复，直至找到有效镜像，或所有镜像验证完毕。

7. bootloader 执行经验证的应用程序镜像。


.. _signature-block-format:

签名块格式
----------------------

签名块以 4 KB 的整数倍为起始位置，拥有独立 flash 扇区。签名计算覆盖了镜像中的所有字节，包括填充字节，请参阅 :ref:`secure_padding`。

.. only:: SOC_SECURE_BOOT_V2_RSA and SOC_SECURE_BOOT_V2_ECC

    .. note::

        {IDF_TARGET_NAME} 可以选择 RSA 或 ECDSA 方案，每个设备只能选择一种方案。

        与 RSA 相比，ECDSA 拥有类似的安全性，但密钥长度更短。据估计，使用 P-256 曲线的 ECDSA 签名安全性大致相当于具有 3072 位密钥的 RSA。然而，ECDSA 签名验证耗时明显长于 RSA 签名验证。

        如果需要快速启动，建议使用 RSA；如果需要较短的密钥，建议使用 ECDSA。

        .. only:: not esp32p4 or not esp32c5

        .. list-table:: 签名验证耗时比较
            :widths: 10 10 20
            :header-rows: 1

            * - **验证方案**
              - **耗时**
              - **CPU 频率**
            * - RSA-3072
              - {IDF_TARGET_RSA_TIME}
              - {IDF_TARGET_CPU_FREQ}
            * - ECDSA-P256
              - {IDF_TARGET_ECDSA_TIME}
              - {IDF_TARGET_CPU_FREQ}

        上表比较了特定方案中验证签名所需的时间，不代表启动时间。

各签名块内容如下表所示：

.. only:: esp32 or SOC_SECURE_BOOT_V2_RSA

    .. list-table:: RSA 签名块的内容
        :widths: 10 10 40
        :header-rows: 1

        * - **偏移量**
          - **大小（字节）**
          - **描述**
        * - 0
          - 1
          - 魔法字节。
        * - 1
          - 1
          - 版本号字节，当前为 0x02，安全启动 v1 的版本号字节为 0x01。
        * - 2
          - 2
          - 填充字节。保留，应设置为 0。
        * - 4
          - 32
          - 仅针对镜像内容的 SHA-256 哈希值，不包括签名块。
        * - 36
          - 384
          - 用于验证签名的 RSA 公模数，在 RFC8017 中为 'n' 值。
        * - 420
          - 4
          - 用于验证签名的 RSA 公指数，在 RFC8017 中为 'e' 值。
        * - 424
          - 384
          - 预先计算的 R，派生自 'n'。
        * - 808
          - 4
          - 预先计算的 M'，派生自 'n'。
        * - 812
          - 384
          - 对镜像内容的 RSA-PSS 签名结果（RFC8017 中的 8.1.1 节），使用以下 PSS 参数计算：SHA256 哈希值、MGF1 函数、32 字节盐长度、默认尾部字段 0xBC。
        * - 1196
          - 4
          - CRC32 的前 1196 字节。
        * - 1200
          - 16
          - 长度填充为 1216 字节的零填充。


    .. note::

      R 和 M' 用于硬件辅助的蒙哥马利乘法 (Montgomery Multiplication)。

.. only:: SOC_SECURE_BOOT_V2_ECC

    .. list-table:: ECDSA 签名块的内容
        :widths: 10 10 40
        :header-rows: 1

        * - **偏移量**
          - **大小（字节）**
          - **描述**
        * - 0
          - 1
          - 魔法字节。
        * - 1
          - 1
          - 版本号字节，当前为 0x03。
        * - 2
          - 2
          - 填充字节。保留，应设置为 0。
        * - 4
          - 32
          - 仅针对镜像内容的 SHA-256 哈希值，不包括签名块。
        * - 36
          - 1
          - 曲线 ID。1 代表 NIST192p 曲线，2 代表 NIST256p 曲线。
        * - 37
          - 64
          - ECDSA 公钥：32 字节的 X 坐标，后跟 32 字节的 Y 坐标。
        * - 101
          - 64
          - 对镜像内容的 ECDSA 签名结果（RFC6090 中的 5.3.2 节）：32 字节的 R 组件，后跟 32 字节的 S 组件。
        * - 165
          - 1031
          - 保留。
        * - 1196
          - 4
          - 前面 1196 字节的 CRC32。
        * - 1200
          - 16
          - 长度填充为 1216 字节的零填充。

签名扇区的其余部分是已擦除的 flash (0xFF)，支持在前一个签名块之后写入其他签名块。


.. _secure_padding:

安全填充
--------------

在安全启动 v2 方案中，应用程序镜像经过处理，会填充到与 flash MMU 页面大小边界对齐，确保只有经过验证的内容会映射到内部地址空间，这称为安全填充。填充后会进行镜像签名计算，随后将签名块 (4 KB) 附加到镜像上。

.. list::

    - 默认 flash MMU 页面大小为 64 KB
    :SOC_MMU_PAGE_SIZE_CONFIGURABLE: - {IDF_TARGET_NAME} 支持配置 flash MMU 页面大小，``CONFIG_MMU_PAGE_SIZE`` 根据 :ref:`CONFIG_ESPTOOLPY_FLASHSIZE` 设置
    - 在进行由 ``esptool.py`` 执行的 ``elf2image`` 转换时，可以通过使用选项 ``--secure-pad-v2`` 应用安全填充

带有安全填充和签名块的安全启动 v2 签名镜像的内容如下表所示：

.. list-table:: 已签名应用程序的内容
        :widths: 20 20 20
        :header-rows: 1

        * - **偏移量**
          - **大小 (KB)**
          - **描述**
        * - 0
          - 580
          - 未签名的应用程序大小，作为示例
        * - 580
          - 60
          - 安全填充，与下一个 64 KB 边界对齐
        * - 640
          - 4
          - 签名块

.. note::

    注意，应用程序镜像始终从下一个 flash MMU 页面大小的边界开始，默认为 64 KB。因此使用上述签名块之后剩余的空间可以存储其他数据分区，如 ``nvs``。


.. _verify_signature-block:

验证签名块
-----------------------------

如果签名块的第一个字节是 ``0xe7``，并且偏移量 1196 处存储了有效的 CRC32，则签名块有效，否则无效。


.. _verify_image:

验证镜像
-----------------------------

如果存储在某个签名块中的公钥是适用于当前设备的有效公钥，且该签名块中存储的签名与从 flash 中读取的镜像数据计算出的签名匹配，则该镜像通过验证。

1. 将嵌入在 bootloader 签名块中的公钥生成的 SHA-256 哈希摘要与存储在 eFuse 中的摘要进行比较，如果公钥的哈希摘要无法与 eFuse 中的任何哈希摘要匹配，则验证失败。

2. 生成应用程序镜像摘要，将其与签名块中的镜像摘要进行匹配，如果无法匹配，则验证失败。

.. only:: esp32 or (SOC_SECURE_BOOT_V2_RSA and not SOC_SECURE_BOOT_V2_ECC)

    3. 使用公钥，采用 RSA-PSS（RFC8017 的第 8.1.2 节）算法，验证 bootloader 镜像的签名，并与步骤 (2) 中计算的镜像摘要比较。

.. only:: SOC_SECURE_BOOT_V2_ECC and not SOC_SECURE_BOOT_V2_RSA

    3. 使用公钥，采用 ECDSA（RFC6090 的第 5.3.3 节）算法，验证 bootloader 镜像的签名，并与步骤 (2) 中计算的镜像摘要比较。

.. only:: SOC_SECURE_BOOT_V2_ECC and SOC_SECURE_BOOT_V2_RSA

    1. 使用公钥，采用 RSA-PSS（RFC8017 的第 8.1.2 节）算法或 ECDSA（RFC6090 的第 5.3.3 节）算法，验证 bootloader 镜像的签名，并与步骤 (2) 中计算的镜像摘要比较。


bootloader 大小
------------------

启用安全启动和/或 flash 加密都会增加 bootloader 的大小，因此可能需要更新分区表偏移量，请参阅 :ref:`bootloader-size`。

禁用 :ref:`CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES` 时，bootloader 将使用 ``esptool`` 的 ``elf2image`` 命令中的 ``--pad-to-size`` 选项进行扇区填充，每个扇区大小为 4 KB。


.. _efuse-usage:

使用 eFuse
-----------

.. only:: esp32

    ESP32 芯片版本 v3.0：

    - ABS_DONE_1 - 在启动时启用安全启动保护。

    - BLK2 - 存储公钥的 SHA-256 摘要。公钥模数、指数、预先计算的 R 和 M' 值的 SHA-256 哈希摘要都将写入 eFuse 密钥块。这个摘要大小为 776 字节，偏移量从 36 到 812，如 :ref:`signature-block-format` 所示。注意，必须设置写保护位，但切勿设置读保护位。

.. only:: not esp32

    - SECURE_BOOT_EN - 在启动时启用安全启动保护。

.. only:: SOC_EFUSE_KEY_PURPOSE_FIELD

    - KEY_PURPOSE_X - 将 SECURE_BOOT_DIGESTX (X = 0, 1, 2) 烧录到 KEY_PURPOSE_X (X = 0, 1, 2, 3, 4, 5)，设置密钥块功能。例如：若设置 KEY_PURPOSE_2 为 SECURE_BOOT_DIGEST1，则 BLOCK_KEY2 将具有安全启动 v2 公钥摘要。注意，必须设置写保护位，该字段无读保护位。

    - BLOCK_KEYX - 该块包含其在 KEY_PURPOSE_X 中烧录的功能的对应数据，并存储公钥的 SHA-256 哈希摘要。公钥模数、指数、预先计算的 R 和 M' 值的 SHA-256 哈希摘要都将写入 eFuse 密钥块。这个摘要大小为 776 字节，偏移量从 36 到 812，如 :ref:`signature-block-format` 所示。注意，必须设置写保护位，但切勿设置读保护位。

    - KEY_REVOKEX - 与 3 个密钥块中的每一个相对应的注销标记。例如，设置 KEY_REVOKE2 将注销密钥功能为 SECURE_BOOT_DIGEST2 的密钥块。

    - SECURE_BOOT_AGGRESSIVE_REVOKE - 启用激进的密钥注销。只要与此密钥的验证失败，密钥就会立即注销。

    为确保后续不会有攻击者添加受信任的密钥，应使用 KEY_REVOKEX 注销所有未使用的密钥摘要槽。若未启用 :ref:`CONFIG_SECURE_BOOT_ALLOW_UNUSED_DIGEST_SLOTS`，应用程序启动时，将在 :cpp:func:`esp_secure_boot_init_checks` 中检查和修复注销操作。

密钥必须为可读密钥，以便软件访问。如果密钥设置了读保护，软件只能读取到全为零的数据，导致签名验证失败，启动中止。


.. _secure-boot-v2-howto:

启用安全启动 v2
----------------------------

1. 打开 :ref:`project-configuration-menu`，在 ``Security features`` 下设置 ``Enable hardware Secure Boot in bootloader`` 以启用安全启动模式。

.. only:: esp32

    2. 对于 ESP32，安全启动 v2 仅适用于 ESP32 芯片版本 v3.0 及以上版本。请将芯片版本更改至 ESP32 芯片版本 v3.0 以查看 ``Secure Boot v2`` 选项。更改芯片版本时，请将 ``Component Config`` > ``ESP32- Specific`` 中的 ``Minimum Supported ESP32 Revision`` 设置为 v3.0。

    3. 在项目目录的基础上，明确指定安全启动签名密钥的路径。

    4. 在 ``UART ROM download mode`` 中选择所需的 UART ROM 下载模式。为避免在开发阶段该模式一直处于禁用状态，UART ROM 模式默认启用，但这是一个潜在的不安全选项。为获得更好的安全性，建议禁用 UART 下载模式。

.. only:: SOC_SECURE_BOOT_V2_RSA or SOC_SECURE_BOOT_V2_ECC

    2. 选择 ``Secure Boot v2`` 选项，并默认将 ``App Signing Scheme`` 设置为 {IDF_TARGET_SBV2_DEFAULT_SCHEME}。{IDF_TARGET_SECURE_BOOT_OPTION_TEXT}

    3. 在项目目录的基础上，明确指定安全启动签名密钥的路径。

    4. 在 ``UART ROM download mode`` 中选择所需 UART ROM 选项。默认情况下，通常建议将其设置为 ``Permanently switch to Secure mode``。对于生产设备，最安全的选项是将其设置为 ``Permanently disabled``。

5. 按需设置其他 menuconfig 选项，随后退出 menuconfig 并保存配置。

6. 初次运行 ``idf.py build`` 时，如果未找到签名密钥，将打印错误消息，并提供通过 ``idf.py secure-generate-signing-key`` 生成签名密钥的命令。

.. important::

   通过此方法生成的签名密钥将使用操作系统和其 Python 安装中提供的最佳随机数源，在 OSX/Linux 上为 `/dev/urandom`，在 Windows 上为 `CryptGenRandom()`。如果此随机数源不足以提供足够的安全性，那么生成的私钥也不足以提供足够的安全性。

.. important::

   在生产环境下，建议使用 OpenSSL 或其他行业标准的加密程序生成密钥对，详情请参阅 :ref:`secure-boot-v2-generate-key`。

7. 运行 ``idf.py bootloader`` 构建启用了安全启动的 bootloader ，构建输出中会包含一个烧录命令的提示，使用 ``esptool.py write_flash`` 烧录。

8. 当你准备好烧录 bootloader 时，请运行指定命令并等待烧录完成。注意，此处的指定命令需要手动输入，构建系统不会执行此过程。

9. 运行 ``idf.py flash`` 构建并烧录分区表以及刚刚构建的应用程序镜像，该镜像使用步骤 6 中生成的签名密钥进行签名。

.. note::

  如果启用了安全启动，``idf.py flash`` 不会烧录 bootloader 。

10.  重置 {IDF_TARGET_NAME}，它将启动你烧录的软件 bootloader 。该软件 bootloader 会在芯片上启用安全启动，然后验证应用程序镜像签名，并启动应用程序。请查看 {IDF_TARGET_NAME} 的串行控制器输出，确保已启用安全启动，且没有因构建配置发生错误。

.. note::

  在烧录了有效的分区表和应用程序镜像之前，安全启动不会启用，避免在系统完全配置前发生意外情况。

.. note::

  如果在初次启动过程中重置或关闭了 {IDF_TARGET_NAME}，它会在下次启动时重新开始上述步骤。

11. 在后续启动过程中，安全启动硬件会验证软件 bootloader 是否更改，软件 bootloader 会使用其附加的签名块中经验证的公钥部分，验证已签名的应用程序镜像。


启用安全启动后的限制
-----------------------------------------

- 任何更新过的 bootloader 或应用程序都需要使用与已存储在 eFuse 中的摘要相匹配的密钥来签名。

- 注意，启用安全启动或 flash 加密会禁用 ROM 中的 USB-OTG USB 栈，阻止通过该端口进行串行仿真或设备固件更新 (DFU)。

烧录读保护密钥
~~~~~~~~~~~~~~~

一旦启用安全启动，就无法再对 eFuses 进行读保护，这可以避免攻击者对存储公共密钥摘要的 eFuse 块进行读保护，进而导致系统无法验证和处理签名，系统服务无法正常运行。

如果第二阶段 bootloader 启用了 :doc:`/security/flash-encryption`，它会确保在第一次启动时生成的 flash 加密密钥被读保护。

如需在设备启用安全启动后对密钥进行读保护，如：

.. list::
  :SOC_FLASH_ENC_SUPPORTED:* flash 加密密钥

  :SOC_HMAC_SUPPORTED:* HMAC 密钥

  :SOC_ECDSA_SUPPORTED:* ECDSA 密钥

  :SOC_KEY_MANAGER_SUPPORTED:* 密钥管理器密钥

请在启用安全启动的同时启用配置项 :ref:`CONFIG_SECURE_BOOT_V2_ALLOW_EFUSE_RD_DIS`，以防止禁用 eFuses 读保护功能。

建议在启用安全启动之前，完成全部密钥的烧录。

如需启用配置项 :ref:`CONFIG_SECURE_BOOT_V2_ALLOW_EFUSE_RD_DIS`，请在所有读保护 eFuses 密钥烧录后，使用 ``esp_efuse.h`` 的 ``esp_efuse_write_field_bit()`` API 烧录 eFuses {IDF_TARGET_EFUSE_WR_DIS_RD_DIS}。


.. _secure-boot-v2-generate-key:

生成安全启动签名密钥
----------------------------------

构建系统会提示你，使用 ``idf.py secure-generate-signing-key`` 命令生成新签名密钥。

.. only:: esp32 or SOC_SECURE_BOOT_V2_RSA

   参数 ``--version 2`` 会为安全启动 v2 生成 RSA 3072 私钥。此外，也可以传递 ``--scheme rsa3072`` 生成 RSA 3072 私钥。

.. only:: SOC_SECURE_BOOT_V2_ECC

   传递 ``--version 2 --scheme ecdsa256`` 或 ``--version 2 --scheme ecdsa192`` 选择 ECDSA 方案，生成相应的 ECDSA 私钥。

签名密钥的强度取决于 (a) 系统的随机数源和 (b) 所用算法的正确性。对于生产设备，建议从具有高质量熵源的系统生成签名密钥，并使用最佳的可用 {IDF_TARGET_SBV2_SCHEME} 密钥生成工具。

例如，使用 OpenSSL 命令行生成签名密钥时：

.. only:: esp32 or SOC_SECURE_BOOT_V2_RSA

    生成 RSA 3072 密钥

    .. code-block::

      openssl genrsa -out my_secure_boot_signing_key.pem 3072

.. only:: SOC_SECURE_BOOT_V2_ECC

    生成 ECC NIST192p 曲线密钥

    .. code-block::

      openssl ecparam -name prime192v1 -genkey -noout -out my_secure_boot_signing_key.pem

    生成 ECC NIST256p 曲线密钥

    .. code-block::

      openssl ecparam -name prime256v1 -genkey -noout -out my_secure_boot_signing_key.pem

注意，安全启动系统的强度取决于能否保持签名密钥的私密性。


.. _remote-sign-v2-image:

远程镜像签名
------------------------

使用 ``idf.py`` 进行签名
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

对于生产构建，将签名密钥存储在远程签名服务器上，而不是本地构建机器上，是一种比较好的方案，这也是默认的 ESP-IDF 安全启动配置。可以使用命令行工具 ``espsecure.py`` 在远程系统上为应用程序镜像和分区表数据签名，供安全启动使用。

使用远程签名时，请禁用选项 :ref:`CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES`，并构建固件。此时，私钥无需存在于构建系统中。

构建完应用程序镜像和分区表后，构建系统会使用 ``idf.py`` 打印签名步骤：

.. code-block::

  idf.py secure-sign-data BINARY_FILE --keyfile PRIVATE_SIGNING_KEY

上述命令将镜像签名附加到现有的二进制文件中，你可以使用 `--output` 参数将签名后的二进制文件写入单独的文件：

.. code-block::

  idf.py secure-sign-data --keyfile PRIVATE_SIGNING_KEY --output SIGNED_BINARY_FILE BINARY_FILE


使用预计算的签名进行签名
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

如果你拥有为镜像生成的有效预计算签名及相应公钥，你可以使用这些签名生成一个签名扇区，并将其附加到镜像中。注意，预计算的签名应计算在镜像中的所有字节，包括安全填充字节。

在此情况下，应禁用选项 :ref:`CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES` 来构建固件镜像。该镜像将进行安全填充，并使用以下命令，生成带签名的二进制文件：

.. code-block::

  idf.py secure-sign-data --pub-key PUBLIC_SIGNING_KEY --signature SIGNATURE_FILE --output SIGNED_BINARY_FILE BINARY_FILE

上述命令会验证签名，生成签名块（请参阅 :ref:`signature-block-format`），并将其附加到二进制文件中。


使用外部硬件安全模块 (HSM) 进行签名
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

为了提高安全性，你可能会使用外部硬件安全模块 (HSM) 存储私钥，该私钥无法直接访问，但具备一个接口，可以生成二进制文件及其相应公钥的签名。

在此情况下，请禁用选项 :ref:`CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES` 并构建固件。随后，可以将已进行安全填充的镜像提供给外部硬件安全模块来生成签名。请参阅 `使用外部 HSM 签名 <https://docs.espressif.com/projects/esptool/en/latest/{IDF_TARGET_PATH_NAME}/espsecure/index.html#remote-signing-using-an-external-hsm>`_ 生成已签名镜像。

.. only:: SOC_EFUSE_REVOKE_BOOT_KEY_DIGESTS

    .. note::

      在上述三种远程签名工作流程中，已签名的二进制文件将写入提供给 ``--output`` 参数的文件名中。选项 ``--append_signatures`` 支持将多个签名（最多 3 个）附加到镜像中。

.. only:: not SOC_EFUSE_REVOKE_BOOT_KEY_DIGESTS

    .. note::

      在上述三种远程签名工作流程中，已签名的二进制文件将写入提供给 ``--output`` 参数的文件名中。


使用安全启动的建议
--------------------------

* 在具备高质量熵源的系统上生成签名密钥。
* 时刻对签名密钥保密，泄漏此密钥将危及安全启动系统。
* 不允许第三方使用 ``idf.py secure-`` 命令来观察密钥生成或签名过程的任何细节，这两个过程都容易受到定时攻击或其他侧信道攻击的威胁。
* 在安全启动配置中启用所有安全启动选项，包括 flash 加密、禁用 JTAG、禁用 BASIC ROM 解释器和禁用 UART bootloader 的加密 flash 访问。
* 结合 :doc:`flash-encryption` 使用安全启动，防止本地读取 flash 内容。

.. only:: SOC_EFUSE_REVOKE_BOOT_KEY_DIGESTS

    密钥管理
    --------------

    * 应独立计算并分别存储 1 到 3 个 {IDF_TARGET_SBV2_KEY} 公钥对（密钥 #0, #1, #2）。
    * 完成烧录后，应设置 KEY_DIGEST eFuse 为写保护位。
    * 未使用的 KEY_DIGEST 槽必须烧录其相应的 KEY_REVOKE eFuse，以永久禁用。请在设备离开工厂前完成此操作。
    * 烧录 eFuse 可以由软件 bootloader 在首次从 menuconfig 启用 ``Secure Boot v2`` 后进行，也可以使用 ``espefuse.py``，后者与 ROM 中的串行 bootloader 通信。
    * KEY_DIGEST 应从密钥摘要 #0 开始，按顺序编号。如果使用了密钥摘要 #1，则必须使用密钥摘要 #0。如果使用了密钥摘要 #2，则必须使用密钥摘要 #0 和 #1。
    * 软件 bootloader 不支持 OTA 升级，它将至少由一个私钥签名，也可能使用全部三个私钥，并在工厂内烧录。
    * 应用程序应仅由单个私钥签名，其他私钥应妥善保管。但如果需要注销某些私钥，也可以使用多个签名私钥，请参阅下文的 :ref:`secure-boot-v2-key-revocation`。


    多个密钥管理
    -------------

    * 在烧录 bootloader 之前，应使用设备整个生命周期所需的所有私钥对 bootloader 签名。
    * 构建系统每次只能使用一个私钥签名，如果需要，你必须手动运行命令以附加更多签名。
    * 你可以使用 ``idf.py secure-sign-data`` 的附加功能，此命令也将在启用安全启动 v2 的 bootloader 编译的末尾显示。

    .. code-block::

        idf.py secure-sign-data -k secure_boot_signing_key2.pem --append_signatures -o signed_bootloader.bin build/bootloader/bootloader.bin

    * 使用多个私钥签名时，建议独立签名这些私钥，可以的话请在不同服务器上进行签名，并将它们分开存储。
    * 可以使用以下命令查看附加到二进制文件的签名：

    .. code-block::

        espsecure.py signature_info_v2 datafile.bin

    .. _secure-boot-v2-key-revocation:

    注销密钥管理
    --------------

    * 密钥按线性顺序处理，即密钥 #0、密钥 #1、密钥 #2。
    * 应用程序每次应只使用一个密钥签名，尽量避免暴露未使用的私钥。
    * bootloader 可以使用来自工厂的多个函数签名。

    .. note::

        请注意，启用配置 :ref:`CONFIG_SECURE_BOOT_ALLOW_UNUSED_DIGEST_SLOTS` 只能确保 **应用程序** 不会撤销未使用的摘要槽。
        若想在设备首次启动时启用安全启动，那么即使启用了上述配置，bootloader 也会在启用安全启动时撤销未使用的摘要槽，因为保留未使用的密钥槽会构成安全隐患。
        如果在开发流程中需要保留未使用摘要槽，则应从外部启用安全启动 (:ref:`enable-secure-boot-v2-externally`)，而不是在启动设备时启用安全启动，这样 bootloader 就无需启用安全启动，从而避免安全隐患。

    保守方法
    ~~~~~~~~~~~~

    假设一个受信任的私钥 (N-1) 受到威胁，需要升级到新的密钥对 (N)。

    1. 服务器发送一次 OTA 更新，包含使用新的私钥 (#N) 签名的应用程序。
    2. 新的 OTA 更新写入未使用的 OTA 应用程序分区。
    3. 验证新应用程序的签名块。对比公钥与 eFuse 中烧录的摘要，并使用已验证的公钥验证应用程序。
    4. 将活动分区设置为新的 OTA 应用程序分区。
    5. 设备重置并加载使用密钥 #N-1 验证的 bootloader ，随后启动使用密钥 #N 验证的新应用程序。
    6. 新应用程序使用密钥 #N 验证 bootloader ，这是最后的检查，然后运行代码注销密钥 #N-1，即设置 KEY_REVOKE eFuse 位。
    7. 可以使用 API `esp_ota_revoke_secure_boot_public_key()` 注销密钥 #N-1。

    * 类似的方法也可以用于物理重新烧录，以使用新的密钥，还可以同时更改 bootloader 的内容。


    .. _secure-boot-v2-aggressive-key-revocation:

    激进方法
    ~~~~~~~~~~~~~~

    ROM 代码具备一项额外功能，即在签名验证失败时可以注销公钥摘要。

    请烧录 ``SECURE_BOOT_AGGRESSIVE_REVOKE`` eFuse 或启用 :ref:`CONFIG_SECURE_BOOT_ENABLE_AGGRESSIVE_KEY_REVOKE` 以启用此功能。

    注销密钥仅适用于成功启用了安全启动的情况。此外，在签名块无效或镜像摘要无效的情况下不会注销密钥，仅在签名验证失败时，即在 :ref:`verify_image` 的第 3 步中验证失败时，才会执行注销操作。

    一旦注销了密钥，它将无法再用于验证镜像签名。该功能提供了强大的物理攻击防护，但如果由于签名验证失败而注销了所有密钥，可能会导致设备再也无法使用。


.. _secure-boot-v2-technical-details:

技术细节
-----------------

以下章节包含安全启动元件的详细参考描述：


手动命令
~~~~~~~~~~~~~~~

安全启动已集成到 ESP-IDF 构建系统中，因此 ``idf.py build`` 将进行应用程序镜像签名。启用 :ref:`CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES` 后，``idf.py bootloader`` 将生成一个已签名的 bootloader 。

然而，也可以使用 ``idf.py`` 工具生成独立的签名和摘要。

二进制镜像签名：

.. code-block::

  idf.py secure-sign-data --keyfile ./my_signing_key.pem --output ./image_signed.bin image-unsigned.bin

Keyfile 是包含 {IDF_TARGET_SBV2_KEY} 签名私钥的 PEM 文件。


.. _secure-boot-v2-and-flash-encr:

安全启动 & flash 加密
------------------------------

如果使用安全启动时没有启用 :doc:`flash-encryption`，可能会发生 ``time-of-check to time-of-use`` 攻击，即在验证并运行镜像后交换 flash 内容。因此，建议同时使用这两个功能。

.. only:: esp32c2

    .. important::

       {IDF_TARGET_NAME} 只有一个 eFuse 密钥块，用于存储两种密钥：安全启动和 flash 加密，但 eFuse 密钥块只能烧录一次，因此建议同时烧录这两种密钥。注意，``Secure Boot`` 和 ``Flash Encryption`` 无法分别启用，否则后续写入 eFuse 密钥块将返回错误。


.. _signed-app-verify-v2:

在未启用硬件安全启动时对应用程序进行签名校验
--------------------------------------------

无需启用硬件安全启动选项，即可在 OTA 更新时验证应用程序的安全启动 v2 签名。这种方法采用了与安全启动 v2 相同的应用程序签名方案，但不同于硬件安全启动，软件安全启动无法阻止能够写入 flash 的攻击者绕过签名验证。

如果在启动时无法接受安全启动验证的延迟，和/或威胁模型不包括物理访问或攻击者在 flash 中写入 bootloader 或应用程序分区，则适合使用未启用硬件安全启动的验证。

在此模式下，当前运行的应用程序签名块中的公钥将用于验证新更新的应用程序签名。更新时，不会验证运行中的应用程序签名，而是假定它有效。通过这种方式，系统建立了从当前运行的应用程序到新更新的应用程序之间的信任链。

因此，请务必确保烧录到设备的初始应用程序已签名。应用程序启动时会进行检查，如果没有找到签名，应用程序将中止，并且将无法再进行任何更新。若应用程序在未找到签名时仍继续更新，则可能导致设备损坏，后续任何更新都无法得到应用。应用程序应只包含一个位于第一位置的有效签名块。注意，不同于安全启动 v2，系统在启动时不会验证运行中的应用程序的签名，只会验证位于第一位置的签名块，并忽略其他附加的签名块。

.. only:: not esp32

    虽然使用硬件安全启动时支持多个受信任的密钥，但如果配置了无需安全启动的签名检查，则仅使用签名块中的第一个公钥验证更新。如果需要多个受信任的公钥，必须启用完整的安全启动功能。

.. note::

    若非确信未启用硬件安全启动的验证已满足应用程序的安全需要，建议使用完整的硬件安全启动。


.. _signed-app-verify-v2-howto:

启用已签名的应用程序验证
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

1. 打开 :ref:`project-configuration-menu` > ``Security features``。

.. only:: esp32

    2. 确保 ``App Signing Scheme`` 设置为 ``RSA``。对于 ESP32 芯片版本 v3.0 的芯片，请将 :ref:`CONFIG_ESP32_REV_MIN` 设置为 ``v3.0``，启用 ``RSA`` 选项

.. only:: SOC_SECURE_BOOT_V2_RSA and not SOC_SECURE_BOOT_V2_ECC

    2. 确保 ``App Signing Scheme`` 设置为 ``RSA``。

.. only:: SOC_SECURE_BOOT_V2_ECC and not SOC_SECURE_BOOT_V2_RSA

    1. 确保 ``App Signing Scheme`` 设置为 ``ECDSA (v2)``。

.. only:: SOC_SECURE_BOOT_V2_RSA and SOC_SECURE_BOOT_V2_ECC

    2. 设置 ``App Signing Scheme`` 为 ``RSA`` 或 ``ECDSA (v2)``。


3. 启用 :ref:`CONFIG_SECURE_SIGNED_APPS_NO_SECURE_BOOT`。

4. 默认情况下，选择 ``Sign binaries during build`` 选项将启用 ``Require signed app images`` 功能，该功能会在构建过程中自动对二进制文件签名，在 ``Secure Boot private signing key`` 中指定的文件将用于镜像签名。

5. 如果禁用了 ``Sign binaries during build`` 选项，则必须按照 :ref:`remote-sign-v2-image` 中的说明，手动签名所有应用程序二进制文件。

.. warning::

    注意，所有烧录的应用程序都必须经过签名，可以在构建过程中签名，也可以在构建后签名。


进阶功能
-----------------

JTAG 调试
~~~~~~~~~~~~~~

启用安全启动模式时，eFuse 会默认禁用 JTAG。初次启动时，bootloader 即禁用 JTAG 调试功能，并启用安全启动模式。

有关在启用安全启动或已签名应用程序验证的情况下使用 JTAG 调试的更多信息，请参阅 :ref:`jtag-debugging-security-features`。
