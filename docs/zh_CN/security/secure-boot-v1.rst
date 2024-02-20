安全启动
===========

:link_to_translation:`en:[English]`

.. important::

    本文档中的参考均与安全启动 V1，即基于 AES 的安全启动方案有关。版本高于 ESP32 v3.0 的芯片推荐选用 :doc:`secure-boot-v2`。

    ESP32 v3.0 和 ESP32-S2 的安全启动请参阅 :doc:`secure-boot-v2`。

安全启动功能用于确保芯片上仅运行签名后的代码，每次系统重置都会验证从 flash 加载的数据。

安全启动功能独立于 :doc:`flash-encryption` 功能，可以在不加密 flash 内容的情况下使用。然而，为确保使用环境安全，建议同时启用安全启动和 flash 加密功能。详情请参阅 :ref:`secure-boot-and-flash-encr`。

.. important::

    启用安全启动将限制进一步更新 {IDF_TARGET_NAME}。请仔细阅读本文档，了解启用安全启动的影响。


背景信息
----------

- 大部分数据存储在 flash 中，关键数据存储在芯片内部的 eFuse 中，无法通过软件访问。因此，为确保安全启动正常运行，无需额外对 flash 访问进行物理保护。

- eFuse 用于永久存储安全引导信息，安全引导密钥存放在 eFuse BLOCK2 中。单个 eFuse 位 ABS_DONE_0 也会进行烧录或写入 1，保持芯片上的安全启动始终启用。有关 eFuse 的更多内容，请参阅 *{IDF_TARGET_NAME} 技术参考手册* > *eFuse 控制器 (EFUSE)* [`PDF <{IDF_TARGET_TRM_CN_URL}#efuse>`__]。

- 为理解安全启动过程，请熟悉标准 :doc:`../api-guides/startup`。

- 安全启动过程会验证启动过程的两个阶段，包括初始软件引导加载程序和随后的分区与应用加载，形成一条信任链。


安全启动过程概述
----------------------------

以下是对安全启动过程的宏观概述，操作指南请参阅 :ref:`secure-boot-how-to`，进一步的技术细节请参阅 :ref:`secure-boot-technical-details`：

1. 启用安全启动的选项可以在 ``Secure Boot Configuration`` 下的 :ref:`project-configuration-menu` 中找到。

2. 安全启动默认在构建过程中签名镜像和分区表数据。配置项 ``Secure boot private signing key`` 是指向 PEM 格式文件中 ECDSA 公钥/私钥对的文件路径。

3. 启用安全启动支持的 ESP-IDF 负责构建软件引导加载程序镜像，签名验证所需公钥已集成到引导加载程序镜像中。此软件引导加载程序镜像将刷写到 0x1000 偏移地址。

4. 初次启动时，软件引导加载程序按以下方式启用安全启动：

   - 硬件安全启动支持生成设备安全引导加载程序密钥以及安全摘要。密钥借助硬件随机数生成，随后存储在 eFuse 中，受读写保护。硬件安全启动还支持生成安全摘要，摘要源自密钥、初始向量 (IV) 和引导加载程序镜像内容。
   - 安全摘要将刷写到 flash 中的 0x0 偏移地址。
   - 根据安全启动配置，烧录 eFuse 以禁用 JTAG 和 ROM 基本解释器。**强烈建议** 禁用 JTAG 和 ROM 基本解释器。
   - 烧录 ABS_DONE_0 eFuse 后，引导加载程序将始终启用安全启动，软件引导加载程序也随之受到保护。此后，芯片只有在摘要匹配时才会启动引导加载程序镜像。

5. 在后续启动过程中，ROM 引导加载程序检测到安全启动 eFuse 已烧录，读取 0x0 处保存的摘要，然后使用硬件安全启动支持来将其与新计算的摘要进行对比。如果摘要不匹配，启动过程终止。摘要读取和对比完全由硬件完成，软件无法读取计算的摘要。更多技术详情，请参阅 :ref:`secure-boot-hardware-support`。

6. 在安全启动模式下运行时，软件引导加载程序使用安全启动签名密钥，该密钥的公钥已嵌入引导加载程序本身，并作为引导加载程序的一部分进行验证，以在启动前验证附加到所有后续分区表和应用程序镜像的签名。


密钥
----

安全启动过程中使用以下密钥：

- 安全引导加载程序密钥是一个 256 位 AES 密钥，存储在 eFuse 块 2 中。引导加载程序可以使用内部硬件随机数生成器自行生成该密钥，无需手动提供。也可以选择提供该密钥，请参阅 :ref:`secure-boot-reflashable`。在启用安全启动之前，存储该密钥的 eFuse 受到读取和写入保护，防止软件访问。

  - eFuse 块 2 默认编码方案为 ``None``，块中存储了一个 256 位密钥。在某些版本的 {IDF_TARGET_NAME} 上，编码方案设置为 ``3/4 Encoding``，CODING_SCHEME eFuse 值为 1，且块中必须存储一个 192 位密钥。

  .. only:: esp32

    详情请参阅 *{IDF_TARGET_NAME} 技术参考手册* > *eFuse 控制器 (EFUSE)* > *系统参数 coding_scheme* [`PDF <{IDF_TARGET_TRM_CN_URL}#efuse>`__].

  算法始终使用 256 位密钥进行操作。对于 192 位密钥，则通过重复某些位来将其扩展至 256 位，请参阅 :ref:`secure-bootloader-digest-algorithm`。

- ``secure boot signing key`` 是标准的 ECDSA 公钥/私钥对，以 PEM 格式存储，请参阅 :ref:`secure-boot-image-signing-algorithm`。

  - 该密钥对的公钥用于验证签名，但不用于创建签名。它编译到软件引导加载程序中，并在启动继续前验证启动过程的第二阶段，包括分区表和应用程序镜像。该公钥可以自由分发，无需保密。

  - 该密钥对的私钥必须严格保密。一旦持有该私钥，即可用它对配置了安全启动和匹配公钥的引导加载程序进行身份验证。


引导加载程序大小
-------------------

启用安全启动和/或 flash 加密都会增加引导加载程序的大小，因此可能需要更新分区表偏移量，请参阅 :ref:`bootloader-size`。


.. _secure-boot-how-to:

启用安全启动
-------------------------

1. 打开 :ref:`project-configuration-menu`，在 ``Secure Boot Configuration`` 中选择 ``One-time Flash`` 选项。如果要了解备选的 ``Reflashable`` 选项，请参阅 :ref:`secure-boot-reflashable`。

2. 为安全启动签名密钥选择名称。此选项将在启用安全启动后出现，密钥文件可以存放在系统的任何位置。如果使用相对路径，系统将基于项目目录确认该路径指向的实际位置。注意，无需在命名前准备好密钥文件。

3. 根据需要设置其他 menuconfig 选项。需注意，``Bootloader Config`` 选项对应的引导加载程序仅支持烧录一次。随后，退出 menuconfig 并保存配置。

4. 初次运行 ``idf.py build`` 时，如果找不到签名密钥，将打印错误信息，并提供一个通过 ``espsecure.py generate_signing_key`` 生成签名密钥的命令。

.. important::

   由此方法生成的签名密钥将使用操作系统及其 Python 安装中提供的最佳随机数源，在 OSX/Linux 上为 `/dev/urandom`，在 Windows 上为 `CryptGenRandom()`。此随机数源的安全性将会直接影响私钥的强度。

.. important::

   在生产环境下，建议使用 OpenSSL 或其他行业标准的加密程序生成密钥对，详情请参阅 :ref:`secure-boot-generate-key`。

5. 运行 ``idf.py bootloader`` 构建启用了安全启动的引导加载程序，构建输出中包含一个使用 ``esptool.py write_flash`` 烧录命令的提示。

.. _secure-boot-resume-normal-flashing:

6. 准备就绪后，运行上述命令并等待烧录完成。注意，此处的指定命令需要手动输入，构建系统不会执行此过程。**注意，完成该一次性烧录后，不支持再对引导加载程序进行任何修改**。

7. 运行 ``idf.py flash`` 构建并烧录分区表和刚刚构建的应用程序镜像。应用程序镜像将使用在步骤 4 中生成的签名密钥签名。

.. note::

  如果启用了安全启动，``idf.py flash`` 不会烧录引导加载程序。

8. 重置 {IDF_TARGET_NAME} 以启动烧录的软件引导加载程序。该软件引导加载程序会在芯片上启用安全启动，验证应用程序镜像签名，并启动应用程序。请查看 {IDF_TARGET_NAME} 的串行控制器输出，确保已启用安全启动，且没有因构建配置发生错误。

.. note::

  为避免在系统配置完成前发生意外情况，仅当有效的分区表和应用程序镜像烧录成功后，才会启用安全启动。

.. note::

  如果在初次启动过程中重置或关闭了 {IDF_TARGET_NAME}，它会在下次启动时重新开始上述步骤。

9. 在后续启动过程中，安全启动硬件将使用安全引导加载程序密钥验证软件引导加载程序是否更改，随后，软件引导加载程序将使用安全启动签名密钥的公钥部分来验证已签名的分区表和应用程序镜像。


.. _secure-boot-reflashable:

可重复烧录的软件引导加载程序
-------------------------------

对于生产设备，推荐使用 ``Secure Boot: One-Time Flash`` 配置。该模式下，每个设备都对应拥有始终存储在设备内部的唯一密钥。

除上述配置外，也可选择 :ref:`CONFIG_SECURE_BOOTLOADER_MODE` 模式。在该备选模式下，可以提供一个二进制密钥文件作为安全引导加载程序密钥。此时，可以生成新的引导加载程序镜像，并为这些镜像生成安全启动摘要。

在 ESP-IDF 构建过程中，该 256 位密钥文件派生自用户生成的 ECDSA 应用程序签名密钥，请参阅下文的 :ref:`secure-boot-generate-key` 步骤。该私钥的 SHA-256 摘要用作 eFuse 中的安全引导加载程序密钥，如果编码方案为 ``None``，则使用完整的 256 位密钥；如果编码方案为 ``3/4 Encoding``，则将密钥截断为 192 字节。这样一来，只需要生成或保护单个密钥文件。

.. note::

  尽管该方案可行，但在生产环境中，强烈建议不要仅生成一个安全引导加载程序密钥并将其烧录到每个设备上。推荐使用 ``One-Time Flash`` 选项。

请按以下步骤启用可重复烧录的引导加载程序：

1. 在 :ref:`project-configuration-menu` 中，选择 ``Bootloader Config`` > :ref:`CONFIG_SECURE_BOOT` > ``CONFIG_SECURE_BOOT_V1_ENABLED`` > :ref:`CONFIG_SECURE_BOOTLOADER_MODE` > ``Reflashable``。

2. 如有需要，按照设备使用的编码方案设置 :ref:`CONFIG_SECURE_BOOTLOADER_KEY_ENCODING`。编码方案将在 ``esptool.py`` 连接到芯片时显示在 ``Features`` 行中，或在 ``espefuse.py summary`` 输出中显示。

3. 请按 :ref:`secure-boot-generate-key` 中的步骤生成签名密钥。生成的密钥文件路径必须在 ``Secure Boot Configuration`` 菜单中指定。

4. 运行 ``idf.py bootloader`` 将创建一个二进制密钥文件，该文件派生自用于签名的私钥。同时将打印两组烧录步骤。第一组步骤包括一个 ``espefuse.py burn_key secure_boot_v1 path_to/secure-bootloader-key-xxx.bin`` 命令，用于将引导加载程序密钥写入 eFuse，此密钥仅可烧录一次。第二组步骤可使用预计算的摘要重新烧录引导加载程序，该摘要在构建过程中生成。

5. 从 :ref:`一次性烧录步骤 6 <secure-boot-resume-normal-flashing>` 继续，烧录引导加载程序并启用安全启动。请密切监视控制器日志输出，确保安全启动配置正确无误。


.. _secure-boot-generate-key:

生成安全启动签名密钥
----------------------------------

构建系统将提供一个命令，用于通过 ``espsecure.py generate_signing_key`` 生成新的签名密钥。这个命令使用 python-ecdsa 库，而该库则使用 Python 的 ``os.urandom()`` 作为随机数源。

签名密钥的强度取决于系统的随机数源和所用算法的正确性。对于生产设备，建议从具有高质量熵源的系统生成签名密钥，并使用最佳的可用 EC 密钥生成工具。

例如，可以采用以下 OpenSSL 命令行生成签名密钥：

.. code-block::

  openssl ecparam -name prime256v1 -genkey -noout -out my_secure_boot_signing_key.pem

注意，安全启动系统的强度取决于能否保持签名密钥的私密性。


.. _remote-sign-image:

远程镜像签名
------------------------

生产构建中，建议使用远程签名服务器，而非将签名密钥存储在构建机器上，这也是默认的 ESP-IDF 安全启动配置。可以使用命令行工具 ``espsecure.py`` 在远程系统上为应用程序镜像和分区表数据签名，供安全启动使用。

使用远程签名时，请禁用选项 ``Sign binaries during build``。此时，签名私钥无需存在于构建系统，但签名公钥必须存在，它会编译到引导加载程序中，并在 OTA 更新期间验证镜像签名。

可以采用以下命令从私钥中提取公钥：

.. code-block::

  espsecure.py extract_public_key --keyfile PRIVATE_SIGNING_KEY PUBLIC_VERIFICATION_KEY

请在 ``Secure boot public signature verification key`` 下的 menuconfig 中指定公共签名验证密钥的路径，构建安全引导加载程序。

构建完应用程序镜像和分区表后，构建系统会使用 ``espsecure.py`` 打印签名步骤：

.. code-block::

  espsecure.py sign_data --keyfile PRIVATE_SIGNING_KEY BINARY_FILE

上述命令将镜像签名附加到现有的二进制文件中，可以使用 `--output` 参数将签名后的二进制文件写入单独的文件：

.. code-block::

  espsecure.py sign_data --keyfile PRIVATE_SIGNING_KEY --output SIGNED_BINARY_FILE BINARY_FILE


使用安全启动的建议
--------------------------

* 在具备高质量熵源的系统上生成签名密钥。
* 时刻对签名密钥保密，泄漏此密钥将危及安全启动系统。
* 不允许第三方查看 ``espsecure.py`` 进行密钥生成或签名过程的任何细节，因为这样容易受到定时或其他侧信道攻击的威胁。
* 在安全启动配置中启用所有安全启动选项，包括 flash 加密、禁用 JTAG、禁用 BASIC ROM 解释器和禁用 UART 引导加载程序的加密 flash 访问。
* 结合 :doc:`flash-encryption` 使用安全启动，防止本地读取 flash 内容。


.. _secure-boot-technical-details:

技术细节
-----------------

以下小节包含安全启动元件的详细参考描述：


.. _secure-boot-hardware-support:

安全启动硬件支持
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

安全启动验证的第一阶段为检查软件引导加载程序，该检查通过硬件完成。{IDF_TARGET_NAME} 的安全启动支持硬件提供以下三种基本操作：

1. 通过硬件随机数生成器生成一系列随机字节。

2. 使用存储在 eFuse 块 2 中的密钥生成数据摘要，通常是从 flash 中提取的引导加载程序镜像。eFuse 中的密钥可以且应设置为读取和写入保护，防止软件访问。有关此算法的完整详细信息，请参阅 `安全引导加载程序摘要算法`_。只有在 eFuse ABS_DONE_0 **未** 烧录，即仍为 0 时，才能通过软件读取摘要。

3. 使用与步骤 2 相同的算法从数据中生成摘要，该数据通常是从 flash 中提取的引导加载程序镜像，然后比较生成的摘要与 buffer 中提供的预计算摘要，预计算摘要通常从 flash 偏移 0x0 处读取。硬件返回一个 true/false 的比较结果，无需向软件提供摘要。即使 eFuse ABS_DONE_0 已经烧录，此功能依旧可用。


.. _secure-bootloader-digest-algorithm:

安全引导加载程序摘要算法
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

输入二进制数据镜像，该算法会生成并输出摘要 (digest)，此摘要在硬件文档中有时也称摘要 (abstract)。

请前往 :component:`/esptool_py` 目录，查看 ``espsecure.py`` 工具，了解算法使用的 Python 版本。具体而言，请查看 ``digest_secure_bootloader`` 命令。

以下带有 (^) 标记的项目用于满足硬件限制，而非密码学限制。

1. 以反向字节顺序从 eFuse 块 2 中读取 AES 密钥。如果编码方案设置为 ``3/4 Encoding``，请使用与 :ref:`flash-encryption-algorithm` 中描述的算法相同的算法，将 192 位密钥扩展到 256 位。
2. 给镜像添加 128 位随机生成的 IV 前缀。
3. 如果镜像长度不是 128 的倍数，则使用 0xFF 将镜像填充至恰为 128 字节的倍数。(^)
4. 对于输入镜像的每个 16 字节明文块：

   - 反转明文输入块的字节顺序。(^)
   - 将 AES256 以 ECB 模式应用于明文块。
   - 反转密文输出块的字节顺序。(^)
   - 将其追加到总体密文输出。

5. 对密文中的每个 4 字节字进行字节交换。(^)
6. 计算密文的 SHA-512 摘要。
7. 对上述计算得到的摘要中的每个 4 字节字进行字节交换。(^)

输出摘要为 192 字节的数据：128 字节的 IV，后跟 64 字节的 SHA-512 摘要。


.. _secure-boot-image-signing-algorithm:

镜像签名算法
~~~~~~~~~~~~~~~~~~~~~~~

按照 `RFC 6979 <https://tools.ietf.org/html/rfc6979>`_ 中的规定，指定确定性的 ECDSA。

- 使用 NIST256p 曲线。OpenSSL 将此曲线称为 prime256v1，有时也称 secp256r1。
- 哈希函数采用 SHA256。
- 用于存储的密钥格式为 PEM。

  - 在引导加载程序中，用于签名签证的公钥作为 64 个原始字节烧录。

- 镜像签名为 68 字节：4 字节的版本号（当前为 0），后跟 64 字节的签名数据。这 68 字节会添加到应用程序镜像或分区表数据末尾。


手动命令
~~~~~~~~~~~~~~~

安全启动已集成到 ESP-IDF 构建系统中，因此若启用了安全启动，``idf.py build`` 将自动签名应用程序镜像。如果 menuconfig 配置了相应选项，``idf.py bootloader`` 将生成引导加载程序摘要。

然而，也可以使用 ``espsecure.py`` 工具生成独立的签名和摘要。

可以使用以下命令进行二进制镜像签名：

.. code-block::

  espsecure.py sign_data --keyfile ./my_signing_key.pem --output ./image_signed.bin image-unsigned.bin

keyfile 是包含 ECDSA 签名私钥的 PEM 文件。

可以使用以下命令生成引导加载程序摘要：

.. code-block::

  espsecure.py digest_secure_bootloader --keyfile ./securebootkey.bin --output ./bootloader-digest.bin build/bootloader/bootloader.bin

keyfile 是设备的 32 字节原始安全启动密钥。

``espsecure.py digest_secure_bootloader`` 命令的输出是一个包含摘要和附加的引导加载程序的独立文件。可以使用以下命令将合并的摘要和引导加载程序烧录到设备上：

.. code-block::

  esptool.py write_flash 0x0 bootloader-digest.bin


.. _secure-boot-and-flash-encr:

安全启动 & flash 加密
------------------------------

如果使用安全启动时没有启用 :doc:`flash-encryption`，可能会发生 ``time-of-check to time-of-use`` 攻击，即在验证并运行镜像后交换 flash 内容。因此，建议同时使用这两个功能。


.. _signed-app-verify:

在未启用硬件安全启动时验证已签名的应用程序
----------------------------------------------------

即使没有启用硬件安全启动选项，也可以检查应用程序的完整性。这种方法使用与硬件安全启动相同的应用程序签名方案，但与硬件安全启动不同的是，它不会阻止引导加载程序的物理更新。这意味着设备可以防止远程网络访问，但无法阻止物理访问。与使用硬件安全启动相比，不使用硬件安全启动更加简单。具体操作请参阅 :ref:`signed-app-verify-how-to`。

应用程序可以在更新时验证，也可以在启动时验证。

- 更新时验证：启用此选项后，每当使用 ``esp_ota_ops.h`` API 执行 OTA 更新，系统都会自动检查签名。如果启用硬件安全启动，则此选项保持启用且无法禁用。如果未启用硬件安全启动，仍然可以通过启用此选项更好地防护网络攻击，防止伪造 OTA 更新。

- 启动时验证：启用此选项后，引导加载程序会编译代码，验证应用程序在启动前是否已签名。如果启用硬件安全启动，则此选项保持启用且无法禁用。如果未启用安全启动，该选项本身无法更好地防护网络攻击，因此多数用户会保持禁用该选项。


.. _signed-app-verify-how-to:

启用已签名的应用程序验证
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

1. 打开 :ref:`project-configuration-menu` > ``Security features`` > 启用 :ref:`CONFIG_SECURE_SIGNED_APPS_NO_SECURE_BOOT`。

2. 启用 ``Bootloader verifies app signatures``，在启动时验证应用程序。

3. 默认情况下，选择 ``Require signed app images`` 选项将启用 ``Sign binaries during build`` 功能，该功能会在构建过程中对二进制文件签名。在 ``Secure boot private signing key`` 中指定的文件将用于镜像签名。

4. 如果禁用了 ``Sign binaries during build`` 选项，则需要在 ``Secure boot public signature verification key`` 中输入用于验证已签名图像的公钥文件路径。

   此时，私钥应按照 :ref:`secure-boot-generate-key` 中的说明生成；公钥验证密钥和已签名镜像应按照 :ref:`remote-sign-image` 中的说明生成。


进阶功能
-----------------

JTAG 调试
~~~~~~~~~~~~~~

启用安全启动模式时，eFuse 会默认禁用 JTAG。初次启动时，引导加载程序即禁用 JTAG 调试功能，并启用安全启动模式。

有关在启用安全启动或已签名应用程序验证的情况下使用 JTAG 调试的更多信息，请参阅 :ref:`jtag-debugging-security-features`。
