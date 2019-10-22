
Flash 加密
============

:link_to_translation:`en:[English]`

本文档将介绍 ESP32 的 Flash 加密功能，并通过示例展示用户如何在开发及生产过程中使用此功能。本文档旨在引导用户快速入门如何测试及验证 Flash 加密的相关操作。有关 Flash 加密块的详细信息可参见 `ESP32 技术参考手册`_。

.. _ESP32 技术参考手册: https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_cn.pdf

概述
------

Flash 加密功能用于加密与 ESP32 搭载使用的 SPI Flash 中的内容。启用 Flash 加密功能后，物理读取 SPI Flash 便无法恢复大部分 Flash 内容。通过明文数据烧录 ESP32 可应用加密功能，（若已启用加密功能）引导加载程序会在首次启动时对数据进行加密。

启用 Flash 加密后，系统将默认加密下列类型的 Flash 数据：

  - 引导加载程序 
  - 分区表
  - 所有 “app” 类型的分区

  其他类型的 Flash 数据将视情况进行加密：

  - 安全启动引导加载程序摘要（如果已启用安全启动）
  - 分区表中标有“加密”标记的分区

Flash 加密与 :doc:`安全启动<secure-boot>` 功能各自独立，您可以在关闭安全启动的状态下使用 Flash 加密。但是，为了安全的计算机环境，二者应同时使用。在关闭安全启动的状态下，需运行其他配置来确保 Flash 加密的有效性。详细信息可参见 :ref:`flash-encryption-without-secure-boot`。 

.. important::
  启用 Flash 加密将限制后续 ESP32 更新。请务必阅读本文档（包括 :ref:`flash-encryption-limitations`）了解启用 Flash 加密的影响。

.. _flash-encryption-efuse:

Flash 加密过程中使用的 eFuse
------------------------------
Flash 加密操作由 ESP32 上的多个 eFuse 控制。以下是这些 eFuse 列表及其描述：                                   

 ::

        eFuse                 描述                                      是否可锁定为        默认
                                                                        读取/写入          值

 .. code-block:: none

        编码方案                该 2 位宽 eFuse 控制 BLOCK1 中使用的实际        是              0
                              位数，从而获得最终的 256 位 AES 密钥。编码
                              方案值解码如下：
                              0: 256 bits
                              1: 192 bits
                              2: 128 bits
                              最终的 AES 密钥根据 FLASH_CRYPT_CONFIG 的                                            
                              值产生。

        BLOCK1                存储 AES 密钥的 256 位宽 eFuse 块                是              x
                                                         
        FLASH_CRYPT_CONFIG    4 位宽 eFuse，控制 AES 加密进程                  是              0xF

        download_dis_encrypt  设置后，在 UART 下载模式运行时关闭 Flash 加        是              0
                              密操作
                              
        download_dis_decrypt  设置后，在 UART 下载模式运行时关闭 Flash 解        是              0
                              密操作
                              
        FLASH_CRYPT_CNT       7 位 eFuse，在启动时启用/关闭加密功能              是              0
                              
                              偶数位（0，2，4，6）：
                              启动时加密 Flash
                              奇数位（1，3，5，7）：
                              启动时不加密 Flash

对上述位的读写访问由 ``efuse_wr_disable`` 和 ``efuse_rd_disable`` 寄存器中的相应位控制。有关 ESP32 eFuse 的详细信息可参见 :doc:`eFuse 管理器 <../api-reference/system/efuse>`。

Flash 的加密过程
------------------

假设 eFuse 值处于默认状态，且第二阶段的引导加载程序编译为支持 Flash 加密，则 Flash 加密过程执行如下：

- 首次上电复位时，Flash 中的所有数据都是未加密形式（明文数据）。第一阶段加载器 (Rom) 将在 IRAM 中加载第二阶段加载器。
- 第二阶段引导加载程序将读取 flash_crypt_cnt (=00000000b) eFuse 值，因为该值为 0（偶数位），第二阶段引导加载程序将配置并启用 Flash 加密块，同时将 ``FLASH_CRYPT_CFG`` eFuse 的值编程为 0xF。
- Flash 加密块将生成 AES-256 位密钥，并将其储存于 BLOCK1 eFuse 中。该操作在硬件中执行，软件将无法访问此密钥。
- 接着，Flash 加密块将加密 Flash 的内容（根据分区表的标记值）。原地加密可能会有耗时（取决于大分区的耗时）。
- 随后，第二阶段引导加载程序将在 flash_crypt_cnt (=00000001b) 中设置第一个可用位，从而标记已加密的 Flash 内容（偶数位）。
- 在 :ref:`flash_enc_release_mode` 下，第二阶段引导加载程序将把 ``download_dis_encrypt``、``download_dis_decrypt`` 和 ``download_dis_cache`` 的 eFuse 位改写为 1，防止 UART 引导加载程序解密 Flash 的内容。同时也将写保护 ``FLASH_CRYPT_CNT`` 的 eFuse 位。
- 在 :ref:`flash_enc_development_mode` 下，第二阶段引导加载程序将仅改写 ``download_dis_decrypt`` 和 ``download_dis_cache`` 的 eFuse 位，从而允许 UART 引导加载程序重新烧录加密的二进制文件。同时将不会写保护 ``FLASH_CRYPT_CNT`` 的 eFuse 位。
- 然后，第二阶段引导加载程序重启设备并开始执行加密映像，同时将透明解密 Flash 的内容并将其加载至 IRAM。

在开发阶段常需编写不同的明文 Flash 映像，以及测试 Flash 的加密过程。这要求 UART 下载模式能够根据需求不断加载新的明文映像。但是，在量产和生产过程中，出于安全考虑，UART 下载模式不应有权限访问 Flash 内容。因此需要有两种不同的 ESP32 配置：一种用于开发，另一种用于生产。以下章节介绍了 Flash 加密的 :ref:`flash_enc_development_mode` 和 :ref:`flash_enc_release_mode` 及其使用指南。

.. important::
  顾名思义，开发模式应 **仅开发过程** 使用，因为该模式可以修改和回读加密的 Flash 内容。

设置 Flash 加密的步骤
----------------------

.. _flash_enc_development_mode:

开发模式
^^^^^^^^^^

可使用 ESP32 内部生成的密钥或外部主机生成的密钥在开发中运行 Flash 加密。

使用 ESP32 生成的 Flash 加密密钥
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

正如上文所说，:ref:`flash_enc_development_mode` 允许用户使用 UART 下载模式多次下载明文映像。需完成以下步骤测试 Flash 加密过程： 

- 确保您的 ESP32 设备有 :ref:`flash-encryption-efuse` 中所示的 Flash 加密 eFuse 的默认设置。

- 可在 ``$IDF_PATH/examples/security/flash_encryption`` 文件夹中找到 Flash 加密的示例应用程序。该示例应用程序中有显示 Flash 加密的状态（启用或关闭）以及 ``FLASH_CRYPT_CNT`` eFuse 值。

- 在第二阶段引导加载程序中启用 Flash 加密支持。请前往 :ref:`project-configuration-menu`，选择 "Security Features"。

- 选择 :ref:`Enable flash encryption on boot <CONFIG_SECURE_FLASH_ENC_ENABLED>`。

- 默认设置模式为 **开发模式**。

- 在引导加载程序 config 下选择适当详细程度的日志。

- 启用 Flash 加密将增大引导加载程序，因而可能需更新分区表偏移。请参见 :ref:`secure-boot-bootloader-size`。
	
- 保存配置并退出。

构建并烧录完整的映像包括：引导加载程序、分区表和 app。这些分区最初以未加密形式写入 Flash。

  ::

	idf.py flash monitor

一旦烧录完成，设备将重置，在下次启动时，第二阶段引导加载程序将加密 Flash 的 app 分区，然后重置该分区。现在，示例应用程序将在运行时解密并执行命令。以下是首次启用 Flash 加密后 ESP32 启动时的样例输出。

 ::

    --- idf_monitor on /dev/cu.SLAB_USBtoUART 115200 ---
    --- Quit: Ctrl+] | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H ---
    ets Jun  8 2016 00:22:57

    rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
    configsip: 0, SPIWP:0xee
    clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
    mode:DIO, clock div:2
    load:0x3fff0018,len:4
    load:0x3fff001c,len:8452
    load:0x40078000,len:13608
    load:0x40080400,len:6664
    entry 0x40080764
    I (28) boot: ESP-IDF v4.0-dev-850-gc4447462d-dirty 2nd stage bootloader
    I (29) boot: compile time 15:37:14
    I (30) boot: Enabling RNG early entropy source...
    I (35) boot: SPI Speed      : 40MHz
    I (39) boot: SPI Mode       : DIO
    I (43) boot: SPI Flash Size : 4MB
    I (47) boot: Partition Table:
    I (51) boot: ## Label            Usage          Type ST Offset   Length
    I (58) boot:  0 nvs              WiFi data        01 02 0000a000 00006000
    I (66) boot:  1 phy_init         RF data          01 01 00010000 00001000
    I (73) boot:  2 factory          factory app      00 00 00020000 00100000
    I (81) boot: End of partition table
    I (85) esp_image: segment 0: paddr=0x00020020 vaddr=0x3f400020 size=0x0808c ( 32908) map
    I (105) esp_image: segment 1: paddr=0x000280b4 vaddr=0x3ffb0000 size=0x01ea4 (  7844) load
    I (109) esp_image: segment 2: paddr=0x00029f60 vaddr=0x40080000 size=0x00400 (  1024) load
    0x40080000: _WindowOverflow4 at esp-idf/esp-idf/components/freertos/xtensa_vectors.S:1778

    I (114) esp_image: segment 3: paddr=0x0002a368 vaddr=0x40080400 size=0x05ca8 ( 23720) load
    I (132) esp_image: segment 4: paddr=0x00030018 vaddr=0x400d0018 size=0x126a8 ( 75432) map
    0x400d0018: _flash_cache_start at ??:?

    I (159) esp_image: segment 5: paddr=0x000426c8 vaddr=0x400860a8 size=0x01f4c (  8012) load
    0x400860a8: prvAddNewTaskToReadyList at esp-idf/esp-idf/components/freertos/tasks.c:4561

    I (168) boot: Loaded app from partition at offset 0x20000
    I (168) boot: Checking flash encryption...
    I (168) flash_encrypt: Generating new flash encryption key...
    I (187) flash_encrypt: Read & write protecting new key...
    I (187) flash_encrypt: Setting CRYPT_CONFIG efuse to 0xF
    W (188) flash_encrypt: Not disabling UART bootloader encryption
    I (195) flash_encrypt: Disable UART bootloader decryption...
    I (201) flash_encrypt: Disable UART bootloader MMU cache...
    I (208) flash_encrypt: Disable JTAG...
    I (212) flash_encrypt: Disable ROM BASIC interpreter fallback...
    I (219) esp_image: segment 0: paddr=0x00001020 vaddr=0x3fff0018 size=0x00004 (     4) 
    I (227) esp_image: segment 1: paddr=0x0000102c vaddr=0x3fff001c size=0x02104 (  8452) 
    I (239) esp_image: segment 2: paddr=0x00003138 vaddr=0x40078000 size=0x03528 ( 13608) 
    I (249) esp_image: segment 3: paddr=0x00006668 vaddr=0x40080400 size=0x01a08 (  6664) 
    I (657) esp_image: segment 0: paddr=0x00020020 vaddr=0x3f400020 size=0x0808c ( 32908) map
    I (669) esp_image: segment 1: paddr=0x000280b4 vaddr=0x3ffb0000 size=0x01ea4 (  7844) 
    I (672) esp_image: segment 2: paddr=0x00029f60 vaddr=0x40080000 size=0x00400 (  1024) 
    0x40080000: _WindowOverflow4 at esp-idf/esp-idf/components/freertos/xtensa_vectors.S:1778

    I (676) esp_image: segment 3: paddr=0x0002a368 vaddr=0x40080400 size=0x05ca8 ( 23720) 
    I (692) esp_image: segment 4: paddr=0x00030018 vaddr=0x400d0018 size=0x126a8 ( 75432) map
    0x400d0018: _flash_cache_start at ??:?

    I (719) esp_image: segment 5: paddr=0x000426c8 vaddr=0x400860a8 size=0x01f4c (  8012) 
    0x400860a8: prvAddNewTaskToReadyList at esp-idf/esp-idf/components/freertos/tasks.c:4561

    I (722) flash_encrypt: Encrypting partition 2 at offset 0x20000...
    I (13229) flash_encrypt: Flash encryption completed
    I (13229) boot: Resetting with flash encryption enabled...


 启用 Flash 加密后，在下次启动时输出将显示已启用 Flash 加密。

 ::

  rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
  configsip: 0, SPIWP:0xee
  clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
  mode:DIO, clock div:2
  load:0x3fff0018,len:4
  load:0x3fff001c,len:8452
  load:0x40078000,len:13652
  ho 0 tail 12 room 4
  load:0x40080400,len:6664
  entry 0x40080764
  I (30) boot: ESP-IDF v4.0-dev-850-gc4447462d-dirty 2nd stage bootloader
  I (30) boot: compile time 16:32:53
  I (31) boot: Enabling RNG early entropy source...
  I (37) boot: SPI Speed      : 40MHz
  I (41) boot: SPI Mode       : DIO
  I (45) boot: SPI Flash Size : 4MB
  I (49) boot: Partition Table:
  I (52) boot: ## Label            Usage          Type ST Offset   Length
  I (60) boot:  0 nvs              WiFi data        01 02 0000a000 00006000
  I (67) boot:  1 phy_init         RF data          01 01 00010000 00001000
  I (75) boot:  2 factory          factory app      00 00 00020000 00100000
  I (82) boot: End of partition table
  I (86) esp_image: segment 0: paddr=0x00020020 vaddr=0x3f400020 size=0x0808c ( 32908) map
  I (107) esp_image: segment 1: paddr=0x000280b4 vaddr=0x3ffb0000 size=0x01ea4 (  7844) load
  I (111) esp_image: segment 2: paddr=0x00029f60 vaddr=0x40080000 size=0x00400 (  1024) load
  0x40080000: _WindowOverflow4 at esp-idf/esp-idf/components/freertos/xtensa_vectors.S:1778

  I (116) esp_image: segment 3: paddr=0x0002a368 vaddr=0x40080400 size=0x05ca8 ( 23720) load
  I (134) esp_image: segment 4: paddr=0x00030018 vaddr=0x400d0018 size=0x126a8 ( 75432) map
  0x400d0018: _flash_cache_start at ??:?

  I (162) esp_image: segment 5: paddr=0x000426c8 vaddr=0x400860a8 size=0x01f4c (  8012) load
  0x400860a8: prvAddNewTaskToReadyList at esp-idf/esp-idf/components/freertos/tasks.c:4561

  I (171) boot: Loaded app from partition at offset 0x20000
  I (171) boot: Checking flash encryption...
  I (171) flash_encrypt: flash encryption is enabled (3 plaintext flashes left)
  I (178) boot: Disabling RNG early entropy source...
  I (184) cpu_start: Pro cpu up.
  I (188) cpu_start: Application information:
  I (193) cpu_start: Project name:     flash-encryption
  I (198) cpu_start: App version:      v4.0-dev-850-gc4447462d-dirty
  I (205) cpu_start: Compile time:     Jun 17 2019 16:32:52
  I (211) cpu_start: ELF file SHA256:  8770c886bdf561a7...
  I (217) cpu_start: ESP-IDF:          v4.0-dev-850-gc4447462d-dirty
  I (224) cpu_start: Starting app cpu, entry point is 0x40080e4c
  0x40080e4c: call_start_cpu1 at esp-idf/esp-idf/components/esp32/cpu_start.c:265

  I (0) cpu_start: App cpu up.
  I (235) heap_init: Initializing. RAM available for dynamic allocation:
  I (241) heap_init: At 3FFAE6E0 len 00001920 (6 KiB): DRAM
  I (247) heap_init: At 3FFB2EC8 len 0002D138 (180 KiB): DRAM
  I (254) heap_init: At 3FFE0440 len 00003AE0 (14 KiB): D/IRAM
  I (260) heap_init: At 3FFE4350 len 0001BCB0 (111 KiB): D/IRAM
  I (266) heap_init: At 40087FF4 len 0001800C (96 KiB): IRAM
  I (273) cpu_start: Pro cpu start user code
  I (291) cpu_start: Starting scheduler on PRO CPU.
  I (0) cpu_start: Starting scheduler on APP CPU.

  Sample program to check Flash Encryption
  This is ESP32 chip with 2 CPU cores, WiFi/BT/BLE, silicon revision 1, 4MB external flash
  Flash encryption feature is enabled
  Flash encryption mode is DEVELOPMENT
  Flash in encrypted mode with flash_crypt_cnt = 1
  Halting...

在此阶段，如果用户希望以加密格式将已修改的明文应用程序映像更新到 Flash 中，可使用以下命令：

 ::

	idf.py encrypted-app-flash monitor

.. _encrypt_partitions:

加密多重分区
^^^^^^^^^^^^^

如果所有分区都需以加密格式更新，则可使用以下命令：

 ::

  idf.py encrypted-flash monitor

.. _pregenerated-flash-encryption-key:

使用主机生成的 Flash 加密密钥
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

可在主机中预生成 Flash 加密密钥，并将其烧录到 ESP32 的 eFuse 密钥块中。这样，无需明文 Flash 更新便可以在主机上预加密数据并将其烧录到 ESP32 中。该功能允许在 :ref:`flash_enc_development_mode` 和 :ref:`flash_enc_release_mode` modes 两模式下加密烧录。

- 确保您的 ESP32 设备有 :ref:`flash-encryption-efuse` 中所示 Flash 加密 eFuse 的默认设置。

- 使用 espsecure.py 随机生成一个密钥::

      espsecure.py generate_flash_encryption_key my_flash_encryption_key.bin

- 将该密钥烧录到设备上（一次性）。 **该步骤须在第一次加密启动前完成**，否则 ESP32 将随机生成一个软件无权限访问或修改的密钥::

      espefuse.py --port PORT burn_key flash_encryption my_flash_encryption_key.bin

- 在第二阶段引导加载程序中启用 Flash 加密支持。请前往 :ref:`project-configuration-menu`，选择 “Security Features”。

- 选择 :ref:`Enable flash encryption on boot <CONFIG_SECURE_FLASH_ENC_ENABLED>`。

- 模式默认设置为 **开发模式**。

- 在引导加载程序 config 下选择适当详细程度的日志。

- 启用 Flash 加密将增大引导加载程序，因而可能需要更新分区表偏移。可参见 See :ref:`secure-boot-bootloader-size`。

- 保存配置并退出。
	
构建并烧录完整的映像包括：引导加载程序、分区表和 app。这些分区最初以未加密形式写入 Flash

  ::

	idf.py flash monitor

下次启动时，第二阶段引导加载程序将加密 Flash 的 app 分区并重置该分区。现在，示例应用程序将在运行时解密并执行命令。

在此阶段，如果用户希望将新的明文应用程序映像更新到 Flash，应调用以下命令

  ::

	idf.py encrypted-app-flash monitor

如何以加密格式重新编程所有分区，可参考 :ref:`encrypt_partitions`。

.. _flash_enc_release_mode:

释放模式
^^^^^^^^^^

在释放模式下，UART 引导加载程序无法执行 Flash 加密操作，**只能** 使用 OTA 方案下载新的明文映像，该方案将在写入 Flash 前加密明文映像。

- 确保您的 ESP32 设备有 :ref:`flash-encryption-efuse` 中所示 Flash 加密 eFuse 的默认设置。

- 在第二阶段引导加载程序中启用 Flash 加密支持。请前往 :ref:`project-configuration-menu`，选择 “Security Features”。

- 选择 :ref:`Enable flash encryption on boot <CONFIG_SECURE_FLASH_ENC_ENABLED>`。

- 选择 **释放模式**，模式默认设置为 **开发模式**。请注意，**一旦选择了释放模式，``download_dis_encrypt`` 和 ``download_dis_decrypt`` eFuse 位将被编程为禁止 UART 引导加载程序访问 Flash 的内容**。

- 在引导加载程序 config 下选择适当详细程度的日志。

- 启用 Flash 加密将增大引导加载程序，因而可能需要更新分区表偏移。可参见 See :ref:`secure-boot-bootloader-size`。

- 保存配置并退出。

构建并烧录完整的映像包括：引导加载程序、分区表和 app。这些分区最初以未加密形式写入 Flash

  ::

    idf.py flash monitor

下次启动时，第二阶段引导加载程序将加密 Flash app 分区并重置该分区。现在，示例应用程序应正确执行命令。

一旦在释放模式下启用 Flash 加密，引导加载程序将写保护 ``FLASH_CRYPT_CNT`` eFuse。

应使用 OTA 方案对字段中的明文进行后续更新。详情可参见 :doc:`OTA <../api-reference/system/ota>`。

可能出现的错误
^^^^^^^^^^^^^^^^

启用 Flash 加密后，如果 ``FLASH_CRYPT_CNT`` eFuse 值中有奇数位，则所有（标有加密标志的）分区都应包含加密密文。以下为 ESP32 加载明文数据会产生的三种典型错误情况：

1. 如果通过明文引导加载程序映像重新更新了引导加载程序分区，则 ROM 加载器将无法加载 引导加载程序，并会显示以下错误类型：

  ::

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

2. 如果引导加载程序已加密，但使用明文分区表映像重新更新了分区表，则引导加载程序将无法读取分区表，并会显示以下错误类型：

  ::

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

3. 如果引导加载程序和分区表已加密，但使用明文应用程序映像重新更新了应用程序，则引导加载程序将无法加载新的应用程序，并会显示以下错误类型：

  ::

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

Flash 加密的要点
-------------------

- 使用 AES-256 加密 Flash 的内容。Flash 加密密钥存储于 eFuse 内部的芯片中，并（默认）受保护防止软件访问。

- `flash 加密算法` 采用的是 AES-256，其中密钥随着 Flash 的每个 32 字节块的偏移地址“调整”。这意味着，每个 32 字节块（2 个连续的 16 字节 AES 块）使用从 Flash 加密密钥中产生的一个特殊密钥进行加密。

- 通过 ESP32 的 Flash 缓存映射功能，Flash 可支持透明访问——读取任何映射到地址空间的 Flash 区域时，都将透明解密该区域。

	为便于访问，某些数据分区最好保持未加密状态，或者也可使用对已加密数据无效的 Flash 友好型更新算法。由于 NVS 库无法与 Flash 加密直接兼容，因此无法加密非易失性存储器的 NVS 分区。详情可参见 :ref:`NVS 加密 <nvs_encryption>`。

- 如果可能已启用 Flash 加密，则编写 :ref:`使用加密 flash <using-encrypted-flash>` 的代码时，编程人员须小心谨慎。

- 如果已启用安全启动，则重新烧录加密设备的引导加载程序则需要“可重新烧录”的安全启动摘要（可参见 :ref:`flash-encryption-and-secure-boot`）。

.. note:: 同时启用安全启动和 Flash 加密后，引导加载程序 app 二进制文件 ``bootloader.bin`` 可能会过大。参见 :ref:`secure-boot-bootloader-size`。

.. important::
   在首次启动加密过程中，请勿中断 ESP32 的电源。如果电源中断，Flash 的内容将受到破坏，并需要重新烧录未加密数据。而这类重新烧录将不计入烧录限制次数。

.. _using-encrypted-flash:

使用加密的 Flash
-------------------

ESP32 app 代码可通过调用函数 :cpp:func:`esp_flash_encryption_enabled` 来确认当前是否已启用 Flash 加密。同时，设备可通过调用函数 :cpp:func:`esp_get_flash_encryption_mode` 来识别使用的 Flash 加密模式。

启用 Flash 加密后，使用代码访问 Flash 内容时需加注意。

Flash 加密的范围
^^^^^^^^^^^^^^^^^^

只要 ``FLASH_CRYPT_CNT`` eFuse 设置为奇数位的值，所有通过 MMU 的 Flash 缓存访问的 Flash 内容都将被透明解密。包括：

- Flash 中可执行的应用程序代码 (IROM)。
- 所有存储于 Flash 中的只读数据 (DROM)。
- 通过函数 :cpp:func:`spi_flash_mmap` 访问的任意数据。
- ROM 引导加载程序读取的软件引导加载程序映像。

.. important::
   MMU Flash 缓存将无条件解密所有数据。Flash 中未加密存储的数据将通过 Flash 缓存“被透明解密”，并在软件中存储为随机垃圾数据。

读取加密的 Flash
^^^^^^^^^^^^^^^^^^

如在不使用 Flash 缓存 MMU 映射的情况下读取数据，推荐使用分区读取函数 :cpp:func:`esp_partition_read`。使用该函数时，只有从加密分区读取的数据才会被解密。其他分区的数据将以未加密形式读取。这样，软件便能同样访问加密和未加密的 Flash。

通过其他 SPI 读取 API 读取的数据均未解密：

- 通过函数 :cpp:func:`spi_flash_read` 读取的数据均未解密。
- 通过 ROM 函数 :cpp:func:`SPIRead` 读取的数据均未解密（esp-idf app 不支持该函数）。
- 使用非易失性存储器 (NVS) API 存储的数据始终从 Flash 加密的角度进行存储和读取解密。如有需要，则由库提供加密功能。详情可参见 :ref:`NVS 加密 <nvs_encryption>`。

写入加密的 Flash
^^^^^^^^^^^^^^^^^^

在可能的情况下，推荐使用分区写入函数 ``esp_partition_write``。使用该函数时，只有向加密分区写入的数据才会被加密。而写入其他分区的数据均未加密。这样，软件便可同样访问加密和未加密的 Flash。

当 write_encrypted 参数设置为“是”时，函数 ``esp_spi_flash_write`` 将写入数据。否则，数据将以未加密形式写入。

ROM 函数 ``esp_rom_spiflash_write_encrypted`` 将在 Flash 中写入加密数据，而 ROM 函数 ``SPIWrite`` 将在 Flash 中写入未加密数据（esp-idf app 不支持上述函数）。

由于数据均采用块加密方式，加密数据最小的写入大小为 16 字节（16字节对齐）。

.. _updating-encrypted-flash:

更新加密的 Flash
-------------------

.. _updating-encrypted-flash-ota:

OTA 更新
^^^^^^^^^^

只要使用了函数 ``esp_partition_write``，则加密分区的 OTA 更新将自动以加密形式写入。 

.. _updating-encrypted-flash-serial:

关闭 Flash 加密
-----------------

若因某些原因意外启用了 Flash 加密，则接下来烧录明文数据时将使 ESP32 软砖（设备不断重启，并报错 ``flash read err, 1000``）。

可通过写入 ``FLASH_CRYPT_CNT`` eFuse 再次关闭 Flash 加密（仅适用于开发模式下）：

- 首先，前往 :ref:`project-configuration-menu`，在“安全性能”目录下关闭 :ref:`启用 Flash 加密启动 <CONFIG_SECURE_FLASH_ENC_ENABLED>`。
- 退出 menuconfig 并保存最新配置。
- 再次运行 ``idf.py menuconfig`` 并复核是否确认已关闭该选项！*如果该选项仍处于已启用状态，则引导加载程序会在启动后立即重新启用加密*。
- 在未启用 Flash 加密的状态下，运行 ``idf.py flash`` 构建并烧录新的引导加载程序与 app。
- 运行 ``espefuse.py`` （``components/esptool_py/esptool`` 中）以关闭 FLASH_CRYPT_CNT::

    espefuse.py burn_efuse FLASH_CRYPT_CNT

重置 ESP32，Flash 加密应处于关闭状态，引导加载程序将正常启动。

.. _flash-encryption-limitations:

Flash 加密的局限性
--------------------

Flash 加密可防止从加密 Flash 中读取明文，从而保护固件防止未经授权的读取与修改。了解 Flash 加密系统的局限之处亦十分重要：

- Flash 加密功能与密钥同样稳固。因而，推荐您首次启动设备时在设备上生成密钥（默认行为）。如果在设备外生成密钥，请确保遵循正确的后续步骤。

- 并非所有数据都是加密存储。因而在 Flash 上存储数据时，请检查您使用的存储方式（库、API等）是否支持 Flash 加密。

- Flash 加密无法防止攻击者获取 Flash 的高层次布局信息。这是因为同一个 AES 密钥要用于每对相邻的 16 字节 AES 块。当这些相邻的 16 字节块中包含相同内容时（如空白或填充区域），这些字节块将加密以产生匹配的加密块对。这可能使得攻击者可在加密设备间进行高层次对比（例如，确认两设备是否可能在运行相同的固件版本）。

- 出于相同原因，攻击者始终可获知一对相邻的 16 字节块（32 字节对齐）何时包含相同内容。因此，在 Flash 上存储敏感数据时应牢记这点，并进行相关设置避免该情况发生（可使用计数器字节或每 16 字节设置不同的值即可）。

- 单独使用 Flash 加密可能无法防止攻击者修改本设备的固件。为防止设备上运行未经授权的固件，可搭配 Flash 加密使用 :doc:`安全启动 <secure-boot>`。

.. _flash-encryption-and-secure-boot:

Flash 加密与安全启动
----------------------

推荐搭配使用 Flash 加密与安全启动。但是，如果已启用安全启动，则重新烧录设备时会受到其他限制：

- :ref:`updating-encrypted-flash-ota` 不受限制（如果新的 app 已使用安全启动签名密钥进行正确签名）。
- 只有当选择 :ref:`可再次烧录 <CONFIG_SECURE_BOOTLOADER_MODE>` 安全启动模式，且安全启动密钥已预生成并烧录至 ESP32（可参见 :ref:`安全启动 <secure-boot-reflashable>`），则 :ref:`明文串行 flash 更新 <updating-encrypted-flash-serial>` 可实现。在该配置下，``idf.py bootloader`` 将生成简化的引导加载程序和安全启动摘要文件，用于在偏移量 0x0 处进行烧录。当进行明文串行重新烧录步骤时，须在烧录其他明文数据前重新烧录此文件。
- 假设未重新烧录引导加载程序，:ref:`使用预生成的 Flash 加密密钥重新烧录 <pregenerated-flash-encryption-key>` 仍可实现。重新烧录引导加载程序时，需在安全启动配置中启用相同的 :ref:`可重新烧录 <CONFIG_SECURE_BOOTLOADER_MODE>` 选项。

.. _flash-encryption-without-secure-boot:

使用无安全启动的 Flash 加密
-----------------------------

尽管 Flash 加密与安全启动可独立使用，但强烈建议您将这二者 **搭配使用** 以确保更高的安全性。

.. _flash-encryption-advanced-features:

Flash 加密的高级功能
----------------------

以下信息可帮助您使用 Flash 加密的高级功能：

加密分区标志
^^^^^^^^^^^^^

部分分区默认为已加密。除此之外，可将任意分区标记为需加密：

在 :doc:`分区表 <../api-guides/partition-tables>` 文档对 CSV 文件的描述中有标志字段。

该字段通常保留为空白。如果在字段中写入"encrypted"，则这个分区将在分区表中标记为已加密，此处写入的数据也视为加密数据（app 分区同样适用）::

   # Name,   Type, SubType, Offset,  Size, Flags
   nvs,      data, nvs,     0x9000,  0x6000
   phy_init, data, phy,     0xf000,  0x1000
   factory,  app,  factory, 0x10000, 1M
   secret_data, 0x40, 0x01, 0x20000, 256K, encrypted

- 默认分区表都不包含任何加密数据分区。

- "app"分区一般都视为加密分区，因此无需将其标记为已加密。

- 如果未启用 Flash 加密，则"encrypted"标记无效。

- 可将带有 ``phy_init`` 数据的可选 ``phy`` 分区标记为已加密，保护该数据防止物理访问读取或修改。

- ``nvs`` 分区无法标记为已加密。

.. _uart-bootloader-encryption:

启用 UART 引导加载程序加密/解密
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

默认情况下，首次启动 Flash 加密过程中将烧录 eFuse ``DISABLE_DL_ENCRYPT``、``DISABLE_DL_DECRYPT`` 和 ``DISABLE_DL_CACHE``：

- ``DISABLE_DL_ENCRYPT`` 在 UART 引导加载程序启动模式下运行时，终止 Flash 加密操作。
- ``DISABLE_DL_DECRYPT`` 在 UART 引导加载程序模式下运行时，终止透明 Flash 解密（即使 FLASH_CRYPT_CNT 已设置为在正常操作中启用 Flash 透明解密）。
- ``DISABLE_DL_CACHE`` 在 UART 引导加载程序模式下运行时终止整个 MMU flash 缓存。

为了完整保存数据，可在首次启动前仅烧录部分 eFuse，并写保护其他部分（未设置值为 0）。例如::

  espefuse.py --port PORT burn_efuse DISABLE_DL_DECRYPT
  espefuse.py --port PORT write_protect_efuse DISABLE_DL_ENCRYPT

（请注意，一个写保护位即可关闭这 3 个 eFuse，因此，写保护一个 eFuse 将写保护上述所有 eFuse。所以，在写保护前须设置任意位）。

.. important::
   由于 ``esptool.py`` 不支持读取加密的 Flash，因此目前基本无法通过写保护这些 eFuse 来将其保持为未设状态。

.. important::
   如果保留 ``DISABLE_DL_DECRYPT`` 未设置（为 0），则实际上将使 Flash 加密无效，因为此时有物理访问权限的攻击者便可使用 UART 引导加载程序模式（使用自定义存根代码）读取 Flash 的内容。

.. _setting-flash-crypt-config:

设置 FLASH_CRYPT_CONFIG
^^^^^^^^^^^^^^^^^^^^^^^^^^^

``FLASH_CRYPT_CONFIG`` eFuse 决定 Flash 加密密钥中随块偏移“调整”的位数。详情可参见 :ref:`flash-encryption-algorithm`。

首次启动 引导加载程序时，该值始终设置为最大 `0xF`。

可手动写入这些 eFuse，并在首次启动前对其写保护，以便选择不同的调整值。但不推荐该操作。

当 ``FLASH_CRYPT_CONFIG`` 的值为 0 时，强烈建议始终不对其进行写保护。如果该 eFuse 设置为 0，则 Flash 加密密钥中无调整位，且 Flash 加密算法相当于 AES ECB 模式。

技术细节
-----------

下节将提供 Flash 加密操作的相关信息。

.. _flash-encryption-algorithm:

Flash 加密算法
^^^^^^^^^^^^^^^^

- AES-256 在 16 字节的数据块上运行。Flash 加密引擎在 32 字节的数据块和 2 个 串行 AES 块上加密或解密数据。

- Flash 加密的主密钥存储于 eFuse (BLOCK1) 中，默认受保护防止进一步写入或软件读取。

- AES-256 密钥大小为 256 位（32 字节），从 eFuse block 1 中读取。硬件 AES 引擎使用反字节序密钥于 eFuse 块中存储的字节序。

  - 如果 ``CODING_SCHEME`` eFuse 设置为 0（默认“无”编码方案），则 eFuse 密钥块为 256 位，且密钥按原方式存储（反字节序）。
  - 如果 ``CODING_SCHEME`` eFuse 设置为 1（3/4 编码），则 eFuse 密钥块为 192 位（反字节序），信息熵总量减少。硬件 Flash 加密仍在 256 字节密钥上运行，在读取后（字节序未反向），密钥扩展为 ``key = key[0:255] + key[64:127]``。

- Flash 加密中使用了逆向 AES 算法，因此 Flash 加密的“加密”操作相当于 AES 解密，而其“解密”操作则相当于 AES 加密。这是为了优化性能，不会影响算法的有效性。

- 每个 32 字节块（2 个相邻的 16 字节 AES 块）都由一个特殊的密钥进行加密。该密钥由 eFuse 中 Flash 加密的主密钥产生，并随 Flash 中该字节块的偏移进行 XOR 运算（一次“密钥调整”）。

- 具体调整量取决于 ``FLASH_CRYPT_CONFIG`` eFuse 的设置。该 eFuse 共 4 位，每位可对特定范围的密钥位进行 XOR 运算：

  - Bit 1，对密钥的 0-66 位进行 XOR 运算。
  - Bit 2，对密钥的 67-131 位进行 XOR 运算。
  - Bit 3，对密钥的 132-194 位进行 XOR 运算。
  - Bit 4，对密钥的 195-256 位进行 XOR 运算。

  建议将 ``FLASH_CRYPT_CONFIG`` 的值始终保留为默认值 `0xF`，这样所有密钥位都随块偏移进行 XOR 运算。详情可参见 :ref:`setting-flash-crypt-config`。

- 块偏移的 19 个高位（第 5-23 位）由 Flash 加密的主密钥进行 XOR 运算。选定该范围的原因为：Flash 的最大尺寸为 16MB（24 位），每个块大小为 32 字节，因而 5 个最低有效位始终为 0。

- 从 19 个块偏移位中每个位到 Flash 加密密钥的 256 位都有一个特殊的映射，以决定与哪个位进行 XOR 运算。有关完整映射可参见 ``espsecure.py`` 源代码中的变量 ``_FLASH_ENCRYPTION_TWEAK_PATTERN``。

- 有关在 Python 中实现的完整 Flash 加密算法，可参见 ``espsecure.py`` 源代码中的函数 `_flash_encryption_operation()`。
