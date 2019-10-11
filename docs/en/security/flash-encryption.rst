
Flash Encryption
================

This document provides introduction to Flash encryption concept on ESP32 and demonstrates how this feature can be used during development as well as production by the user using a sample example. The primary intention of the document is to act as a quick start guide to test and verify flash encryption operations. The details of the flash encryption block can be found in the `ESP32 Technical reference manual`_.

.. _ESP32 Technical Reference Manual: https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf

Introduction
------------

Flash encryption is a feature for encrypting the contents of the ESP32's attached SPI flash. When flash encryption is enabled, physical readout of the SPI flash is not sufficient to recover most flash contents. Encryption is applied by flashing the ESP32 with plaintext data, and (if encryption is enabled) the bootloader encrypts the data in place on first boot.

With flash encryption enabled, following kinds of flash data are encrypted by default:

  - Bootloader
  - Partition Table
  - All "app" type partitions

  Other type of flash data are encrypted conditionally:

  - Secure boot bootloader digest (if secure boot is enabled)
  - Any partition marked with the "encrypted" flag in the partition table

Flash encryption is separate from the :doc:`Secure Boot <secure-boot>` feature, and you can use flash encryption without enabling secure boot. However, for a secure environment both should be used simultaneously.

.. important::
   For production use, flash encryption should be enabled in the "Release" mode only.

.. important::
  Enabling flash encryption limits the options for further updates of the ESP32. Make sure to read this document (including :ref:`flash-encryption-limitations`) and understand the implications of enabling flash encryption.

.. _flash-encryption-efuse:

eFuse Used During Flash Encryption Process
-------------------------------------------
The flash encryption operation is controlled by various eFuses available on ESP32. Below is the list of eFuse and their description:

 ::

        eFuse                 Description                             Can be locked for   Default
                                                                      reading/writing     Value

 .. code-block:: none

        Coding scheme         This 2 bit wide eFuse controls the          Yes              0
                              actual number of bits to be used
                              from BLOCK1 to derive final 256 bit
                              AES key. The coding scheme value is
                              decoded as below:
                              0: 256 bits
                              1: 192 bits
                              2: 128 bits
                              Final AES key is derived based on the
                              FLASH_CRYPT_CONFIG value

        BLOCK1                256 bit wide eFuse block for storing        Yes              x
                              AES key

        FLASH_CRYPT_CONFIG    4 bit wide eFuse which controls the         Yes              0xF
                              AES encryption process

        download_dis_encrypt  When set, disables the flash encryption     Yes              0
                              operation while running in UART
                              download mode

        download_dis_decrypt  When set, disables the flash decryption     Yes              0
                              operation while running in UART
                              download mode

        FLASH_CRYPT_CNT       7 bit eFuse which enables/disables          Yes              0
                              encryption at boot time

                              Even number of bits set (0, 2, 4, 6):
                              encrypt flash at boot time
                              Odd number of bits set (1, 3, 5, 7): do
                              not encrypt flash at boot time


Read and write access to above bits is controlled by appropriate bits in ``efuse_wr_disable`` and ``efuse_rd_disable`` registers. More information about ESP32 eFuse can be found at :doc:`eFuse manager <../api-reference/system/efuse>`.


Flash Encryption Process
------------------------

Assuming the eFuse values are in default state and second stage bootloader is compiled to support flash encryption, the flash encryption process executes as below:

- On first power-on reset, all data in flash is un-encrypted (plaintext). First stage loader (ROM) will load the second stage loader in IRAM.
- Second stage bootloader will read the flash_crypt_cnt (=00000000b) eFuse value and since the value is 0 (even number of bits set) it will configure and enable the flash encryption block. It will also program ``FLASH_CRYPT_CFG`` eFuse to value 0xF.
- The flash encryption block will generate AES-256 bit key and store into BLOCK1 eFuse. This operation is performed in hardware and the key can not be accessed by software.
- Next the flash encryption block will encrypt the flash contents (based on partition table flag value). Encrypting in-place can take some time (up to a minute for large partitions).
- Second stage bootloader then sets the first available bit in flash_crypt_cnt (=00000001b) to mark the flash contents as encrypted (odd number of bits set).
- For :ref:`flash_enc_release_mode` second stage bootloader will program ``download_dis_encrypt``, ``download_dis_decrypt`` & ``download_dis_cache`` eFuse bits to 1 to prevent UART bootloader from decrypting the flash contents. It will also write protect the ``FLASH_CRYPT_CNT`` eFuse bits.
- For :ref:`flash_enc_development_mode` second stage bootloader will program only ``download_dis_decrypt`` & ``download_dis_cache`` eFuse bits to allow UART bootloader reflashing of encrypted binaries. Also ``FLASH_CRYPT_CNT`` eFuse bits will NOT be write protected.
- The second stage bootloader then reboots the device to start executing encrypted image. It will transparently decrypt the flash contents and load into IRAM.

During development stage there is a frequent need to program different plaintext flash images and test the flash encryption process. This requires UART download mode to be able to load new plaintext images as many number of times as required. However during manufacturing or production UART download mode should not be allowed to access flash contents due to security reason. Hence this requires two different ESP32 configurations: one for development and other for production. Following section describes :ref:`flash_enc_development_mode` and :ref:`flash_enc_release_mode` for flash encryption and a step by step process to use them.

.. important::
  Development mode as the name suggests should be used ONLY DURING DEVELOPMENT as it does not prevent modification and possible read back of encrypted flash contents.


Steps to Setup Flash Encryption
-------------------------------

.. _flash_enc_development_mode:

Development Mode
^^^^^^^^^^^^^^^^

It is possible to run flash encryption process for development using either ESP32 internally generated key or external host generated keys.

Using ESP32 Generated Flash Encryption Key
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

As mentioned above :ref:`flash_enc_development_mode` allows user to download as many plaintext images using UART download mode. Following steps needs to be done to test flash encryption process:

- Ensure you have a ESP32 device with default flash encryption eFuse settings as shown in :ref:`flash-encryption-efuse`.

- Navigate to flash encryption sample application in ``$IDF_PATH/examples/security/flash_encryption`` folder. This sample application will print the status of flash encryption: enabled or disabled. It will print the ``FLASH_CRYPT_CNT`` eFuse value.   

- Enable flash encryption support in second stage bootloader. In :ref:`project-configuration-menu`, navigate to "Security Features".

- Select :ref:`Enable flash encryption on boot <CONFIG_SECURE_FLASH_ENC_ENABLED>`.

- By default the mode is set for **Development**.

- Select appropriate Bootloader log verbosity under Bootloader config.

- Update to the partition table offset may be required since after enabling flash encryption the size of bootloader is increased. See :ref:`secure-boot-bootloader-size`
	
- Save the configuration and exit.

Build and flash the complete image including: bootloader, partition table and app. These partitions are initially written to the flash unencrypted.

  ::

	idf.py flash monitor

Once the flashing is complete device will reset and on next boot second stage bootloader will encrypt the flash app partition and then reset. Now the sample application would get decrypted at runtime and executed. Below is a sample output when ESP32 boots after flash encryption is enabled for the first time.

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


 Once the flash encryption is enabled, on subsequent boot the output would mention that flash encryption is already enabled.

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


At this stage if user wants to update modified plaintext application image to flash in encrypted format it can be done using following command:

 ::

	idf.py encrypted-app-flash monitor

.. _encrypt_partitions:

Encrypt Multiple Partitions
^^^^^^^^^^^^^^^^^^^^^^^^^^^

If all partitions needs to be updated in encrypted format, it can be done as

 ::

  idf.py encrypted-flash monitor

.. _pregenerated-flash-encryption-key:

Using Host Generated Flash Encryption Key
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
It is possible to pregenerate the flash encryption key on the host computer and burn it into the ESP32's eFuse key block. This allows data to be pre-encrypted on the host and flashed to the ESP32 without needing a plaintext flash update. This feature allows encrypted flashing in both :ref:`flash_enc_development_mode` and :ref:`flash_enc_release_mode` modes.

.. note:: This option is not recommended for production unless a separate key is generated for each individual device.

- Ensure you have a ESP32 device with default flash encryption eFuse settings as shown in :ref:`flash-encryption-efuse`.

- Generate a random key with espsecure.py::

      espsecure.py generate_flash_encryption_key my_flash_encryption_key.bin

- Burn the key to the device (one time only). **This must be done before first encrypted boot**, otherwise the ESP32 will generate a random key that software can't access or modify::

      espefuse.py --port PORT burn_key flash_encryption my_flash_encryption_key.bin

- Enable flash encryption support in second stage bootloader. In :ref:`project-configuration-menu`, navigate to "Security Features".

- Select :ref:`Enable flash encryption on boot <CONFIG_SECURE_FLASH_ENC_ENABLED>`.

- By default the mode is set for **Development**.

- Select appropriate Bootloader log verbosity under Bootloader config.

- Update to the partition table offset may be required since after enabling flash encryption the size of bootloader is increased. See :ref:`secure-boot-bootloader-size`

- Save the configuration and exit.

Build and flash the complete image including: bootloader, partition table and app. These partitions are initially written to the flash unencrypted

  ::

	idf.py flash monitor

On next boot second stage bootloader will encrypt the flash app partition and then reset. Now the sample application would get decrypted at runtime and executed.

At this stage if user wants to update new plaintext application image to flash they should issue following command

  ::

	idf.py encrypted-app-flash monitor

For reprogramming all partitions in encrypted format follow :ref:`encrypt_partitions`.


.. _flash_enc_release_mode:

Release Mode
^^^^^^^^^^^^

In Release mode UART bootloader can not perform flash encryption operations and new plaintext images can be downloaded ONLY using OTA scheme which will encrypt the plaintext image before writing to flash.

- Ensure you have a ESP32 device with default flash encryption eFuse settings as shown in :ref:`flash-encryption-efuse`.

- Enable flash encryption support in second stage bootloader. In :ref:`project-configuration-menu`, navigate to "Security Features".

- Select :ref:`Enable flash encryption on boot <CONFIG_SECURE_FLASH_ENC_ENABLED>`.

- Select **Release Mode**, by default the mode is set for **Development**. Please note **once the Release mode is selected the ``download_dis_encrypt`` and ``download_dis_decrypt`` eFuse bits will be programmed to disable UART bootloader access to flash contents**.

- Select appropriate Bootloader log verbosity under Bootloader config.

- Update to the partition table offset may be required since after enabling flash encryption the size of bootloader is increased. See :ref:`secure-boot-bootloader-size`

- Save the configuration and exit.

Build and flash the complete image including: bootloader, partition table and app. These partitions are initially written to the flash unencrypted

  ::

    idf.py flash monitor

On next boot second stage bootloader will encrypt the flash app partition and then reset. Now the sample application should execute correctly.

Once the flash encryption is enabled in Release mode the bootloader will write protect the ``FLASH_CRYPT_CNT`` eFuse.

For subsequent plaintext update in field OTA scheme should be used. Please refer :doc:`OTA <../api-reference/system/ota>` for further details.

Possible Failures
^^^^^^^^^^^^^^^^^
Once flash encryption is enabled and if the ``FLASH_CRYPT_CNT`` eFuse value has an odd number of bits set then all the partitions (which are marked with encryption flag) are expected to contain encrypted ciphertext. Below are three typical failure cases if the ESP32 is loaded with plaintext data:

1. In case the bootloader partition is re-updated with plaintext bootloader image the ROM loader will fail to load the bootloader and following type of failure will be displayed:

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

.. note:: This error also appears in the flash contents is erased or corrupted.

2. In case the bootloader is encrypted but partition table is re-updated with plaintext partition table image the bootloader will fail to read the partition table and following type of failure will be displayed:

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

3. In case the bootloader & partition table are encrypted but application is re-updated with plaintext application image the bootloader will fail load the new application and following type of failure will be displayed:

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

Key Points About Flash Encryption
---------------------------------

- The contents of the flash are encrypted using AES-256. The flash encryption key is stored in eFuse internal to the chip, and is (by default) protected from software access.

- The `flash encryption algorithm` is AES-256, where the key is "tweaked" with the offset address of each 32 byte block of flash. This means every 32 byte block (two consecutive 16 byte AES blocks) is encrypted with a unique key derived from the flash encryption key.

- Flash access is transparent via the flash cache mapping feature of ESP32 - any flash regions which are mapped to the address space will be transparently decrypted when read.

	It may be desirable for some data partitions to remain unencrypted for ease of access, or to use flash-friendly update algorithms that are ineffective if the data is encrypted. NVS partitions for non-volatile storage cannot be encrypted since NVS library is not directly compatible with flash encryption. Refer to :ref:`NVS Encryption <nvs_encryption>` for more details.


- If flash encryption may be enabled, the programmer must take certain precautions when writing code that :ref:`uses encrypted flash <using-encrypted-flash>`.

- If secure boot is enabled, reflashing the bootloader of an encrypted device requires a "Reflashable" secure boot digest (see :ref:`flash-encryption-and-secure-boot`).

.. note:: The bootloader app binary ``bootloader.bin`` may become too large when both secure boot and flash encryption are enabled. See :ref:`secure-boot-bootloader-size`.

.. important::
   Do not interrupt power to the ESP32 while the first boot encryption pass is running. If power is interrupted, the flash contents will be corrupted and require flashing with unencrypted data again. A reflash like this will not count towards the flashing limit.


.. _using-encrypted-flash:

Using Encrypted Flash
---------------------

ESP32 app code can check if flash encryption is currently enabled by calling :cpp:func:`esp_flash_encryption_enabled`. Also, device can identify the flash encryption mode by calling :cpp:func:`esp_get_flash_encryption_mode`.

Once flash encryption is enabled, some care needs to be taken when accessing flash contents from code.

Scope of Flash Encryption
^^^^^^^^^^^^^^^^^^^^^^^^^

Whenever the ``FLASH_CRYPT_CNT`` eFuse is set to a value with an odd number of bits set, all flash content which is accessed via the MMU's flash cache is transparently decrypted. This includes:

- Executable application code in flash (IROM).
- All read-only data stored in flash (DROM).
- Any data accessed via :cpp:func:`spi_flash_mmap`.
- The software bootloader image when it is read by the ROM bootloader.

.. important::
   The MMU flash cache unconditionally decrypts all data. Data which is stored unencrypted in the flash will be "transparently decrypted" via the flash cache and appear to software like random garbage.

Reading Encrypted Flash
^^^^^^^^^^^^^^^^^^^^^^^
To read data without using a flash cache MMU mapping, we recommend using the partition read function :cpp:func:`esp_partition_read`. When using this function, data will only be decrypted when it is read from an encrypted partition. Other partitions will be read unencrypted. In this way, software can access encrypted and non-encrypted flash in the same way.

Data which is read via other SPI read APIs are not decrypted:

- Data read via :cpp:func:`spi_flash_read` is not decrypted.
- Data read via ROM function :cpp:func:`SPIRead` is not decrypted (this function is not supported in esp-idf apps).
- Data stored using the Non-Volatile Storage (NVS) API is always stored and read decrypted from the perspective of flash encryption. It is up to the library to provide encryption feature if required. Refer to :ref:`NVS Encryption <nvs_encryption>` for more details.


Writing Encrypted Flash
^^^^^^^^^^^^^^^^^^^^^^^

Where possible, we recommend using the partition write function ``esp_partition_write``. When using this function, data will only be encrypted when writing to encrypted partitions. Data will be written to other partitions unencrypted. In this way, software can access encrypted and non-encrypted flash in the same way.

The ``esp_spi_flash_write`` function will write data when the write_encrypted parameter is set to true. Otherwise, data will be written unencrypted.

The ROM function ``esp_rom_spiflash_write_encrypted`` will write encrypted data to flash, the ROM function ``SPIWrite`` will write unencrypted to flash. (these function are not supported in esp-idf apps).

Because data is encrypted in blocks, the minimum write size for encrypted data is 16 bytes (and the alignment is 16 bytes).

.. _updating-encrypted-flash:

Updating Encrypted Flash
------------------------

.. _updating-encrypted-flash-ota:

OTA Updates
^^^^^^^^^^^

OTA updates to encrypted partitions will automatically write encrypted, as long as the ``esp_partition_write`` function is used.

.. _updating-encrypted-flash-serial:


Disabling Flash Encryption
--------------------------

If you've accidentally enabled flash encryption for some reason, the next flash of plaintext data will soft-brick the ESP32 (the device will reboot continuously, printing the error ``flash read err, 1000``).

If flash encryption is enabled in Development mode, you can disable flash encryption again by writing ``FLASH_CRYPT_CNT`` eFuse. This can only be done three times per chip.

- First, open :ref:`project-configuration-menu` and disable :ref:`Enable flash encryption boot <CONFIG_SECURE_FLASH_ENC_ENABLED>` under "Security Features".
- Exit menuconfig and save the new configuration.
- Run ``idf.py menuconfig`` again and double-check you really disabled this option! *If this option is left enabled, the bootloader will immediately re-enable encryption when it boots*.
- Run ``idf.py flash`` to build and flash a new bootloader and app, without flash encryption enabled.
- Run ``espefuse.py`` (in ``components/esptool_py/esptool``) to disable the FLASH_CRYPT_CNT::
    espefuse.py burn_efuse FLASH_CRYPT_CNT

Reset the ESP32 and flash encryption should be disabled, the bootloader will boot as normal.

.. _flash-encryption-limitations:

Limitations of Flash Encryption
-------------------------------

Flash encryption prevents plaintext readout of the encrypted flash, to protect firmware against unauthorised readout and modification. It is important to understand the limitations of the flash encryption system:

- Flash encryption is only as strong as the key. For this reason, we recommend keys are generated on the device during first boot (default behaviour). If generating keys off-device, ensure proper procedure is followed and don't share the same key between all production devices.

- Not all data is stored encrypted. If storing data on flash, check if the method you are using (library, API, etc.) supports flash encryption.

- Flash encryption does not prevent an attacker from understanding the high-level layout of the flash. This is because the same AES key is used for every pair of adjacent 16 byte AES blocks. When these adjacent 16 byte blocks contain identical content (such as empty or padding areas), these blocks will encrypt to produce matching pairs of encrypted blocks. This may allow an attacker to make high-level comparisons between encrypted devices (i.e. to tell if two devices are probably running the same firmware version).

- For the same reason, an attacker can always tell when a pair of adjacent 16 byte blocks (32 byte aligned) contain two identical 16 byte sequences. Keep this in mind if storing sensitive data on the flash, design your flash storage so this doesn't happen (using a counter byte or some other non-identical value every 16 bytes is sufficient). :ref:`NVS Encryption <nvs_encryption>` deals with this and is suitable for many uses.

- Flash encryption alone may not prevent an attacker from modifying the firmware of the device. To prevent unauthorised firmware from running on the device, use flash encryption in combination with :doc:`Secure Boot <secure-boot>`.

.. _flash-encryption-and-secure-boot:

Flash Encryption and Secure Boot
---------------------------------

It is recommended to use flash encryption and secure boot together. However, if Secure Boot is enabled then additional restrictions apply to reflashing the device:

- :ref:`updating-encrypted-flash-ota` are not restricted (provided the new app is signed correctly with the Secure Boot signing key).
- :ref:`Plaintext serial flash updates <updating-encrypted-flash-serial>` are only possible if the :ref:`Reflashable <CONFIG_SECURE_BOOTLOADER_MODE>` Secure Boot mode is selected and a Secure Boot key was pre-generated and burned to the ESP32 (refer to :ref:`Secure Boot <secure-boot-reflashable>` docs.). In this configuration, ``idf.py bootloader`` will produce a pre-digested bootloader and secure boot digest file for flashing at offset 0x0. When following the plaintext serial reflashing steps it is necessary to re-flash this file before flashing other plaintext data.
- :ref:`Reflashing via Pregenerated Flash Encryption Key <pregenerated-flash-encryption-key>` is still possible, provided the bootloader is not reflashed. Reflashing the bootloader requires the same :ref:`Reflashable <CONFIG_SECURE_BOOTLOADER_MODE>` option to be enabled in the Secure Boot config.

.. _flash-encryption-advanced-features:

Flash Encryption Advanced Features
----------------------------------

The following information is useful for advanced use of flash encryption:

Encrypted Partition Flag
^^^^^^^^^^^^^^^^^^^^^^^^

Some partitions are encrypted by default. Otherwise, it is possible to mark any partition as requiring encryption:

In the :doc:`partition table <../api-guides/partition-tables>` description CSV files, there is a field for flags.

Usually left blank, if you write "encrypted" in this field then the partition will be marked as encrypted in the partition table, and data written here will be treated as encrypted (same as an app partition)::

   # Name,   Type, SubType, Offset,  Size, Flags
   nvs,      data, nvs,     0x9000,  0x6000
   phy_init, data, phy,     0xf000,  0x1000
   factory,  app,  factory, 0x10000, 1M
   secret_data, 0x40, 0x01, 0x20000, 256K, encrypted

- None of the default partition tables include any encrypted data partitions.

- It is not necessary to mark "app" partitions as encrypted, they are always treated as encrypted.

- The "encrypted" flag does nothing if flash encryption is not enabled.

- It is possible to mark the optional ``phy`` partition with ``phy_init`` data as encrypted, if you wish to protect this data from physical access readout or modification.

- It is not possible to mark the ``nvs`` partition as encrypted.

.. _uart-bootloader-encryption:

Enabling UART Bootloader Encryption/Decryption
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

By default, on first boot the flash encryption process will burn eFuses ``DISABLE_DL_ENCRYPT``, ``DISABLE_DL_DECRYPT`` and ``DISABLE_DL_CACHE``:

- ``DISABLE_DL_ENCRYPT`` disables the flash encryption operations when running in UART bootloader boot mode.
- ``DISABLE_DL_DECRYPT`` disables transparent flash decryption when running in UART bootloader mode, even if FLASH_CRYPT_CNT is set to enable it in normal operation.
- ``DISABLE_DL_CACHE`` disables the entire MMU flash cache when running in UART bootloader mode.

It is possible to burn only some of these eFuses, and write-protect the rest (with unset value 0) before the first boot, in order to preserve them. For example::

  espefuse.py --port PORT burn_efuse DISABLE_DL_DECRYPT
  espefuse.py --port PORT write_protect_efuse DISABLE_DL_ENCRYPT

(Note that all 3 of these eFuses are disabled via one write protect bit, so write protecting one will write protect all of them. For this reason, it's necessary to set any bits before write-protecting.)

.. important::
   Write protecting these eFuses to keep them unset is not currently very useful, as ``esptool.py`` does not support reading encrypted flash.

.. important::
   If ``DISABLE_DL_DECRYPT`` is left unset (0) this effectively makes flash encryption useless, as an attacker with physical access can use UART bootloader mode (with custom stub code) to read out the flash contents.

.. _setting-flash-crypt-config:

Setting FLASH_CRYPT_CONFIG
^^^^^^^^^^^^^^^^^^^^^^^^^^

The ``FLASH_CRYPT_CONFIG`` eFuse determines the number of bits in the flash encryption key which are "tweaked" with the block offset. See :ref:`flash-encryption-algorithm` for details.

First boot of the bootloader always sets this value to the maximum `0xF`.

It is possible to write these eFuse manually, and write protect it before first boot in order to select different tweak values. This is not recommended.

It is strongly recommended to never write protect ``FLASH_CRYPT_CONFIG`` when it the value is zero. If this eFuse is set to zero, no bits in the flash encryption key are tweaked and the flash encryption algorithm is equivalent to AES ECB mode.


Technical Details
-----------------

The following sections provide some reference information about the operation of flash encryption.

.. _flash-encryption-algorithm:

Flash Encryption Algorithm
^^^^^^^^^^^^^^^^^^^^^^^^^^

- AES-256 operates on 16 byte blocks of data. The flash encryption engine encrypts and decrypts data in 32 byte blocks, two AES blocks in series.

- The main flash encryption key is stored in eFuse (BLOCK1) and by default is protected from further writes or software readout.

- AES-256 key size is 256 bits (32 bytes), read from eFuse block 1. The hardware AES engine uses the key in reversed byte order to the order stored in the eFuse block.
  - If ``CODING_SCHEME`` eFuse is set to 0 (default "None" Coding Scheme) then the eFuse key block is 256 bits and the key is stored as-is (in reversed byte order).
  - If ``CODING_SCHEME`` eFuse is set to 1 (3/4 Encoding) then the eFuse key block is 192 bits (in reversed byte order), so overall entropy is reduced. The hardware flash encryption still operates on a 256-bit key, after being read (and un-reversed), the key is extended by as ``key = key[0:255] + key[64:127]``.

- AES algorithm is used inverted in flash encryption, so the flash encryption "encrypt" operation is AES decrypt and the "decrypt" operation is AES encrypt. This is for performance reasons and does not alter the effectiveness of the algorithm.

- Each 32 byte block (two adjacent 16 byte AES blocks) is encrypted with a unique key. The key is derived from the main flash encryption key in eFuse, XORed with the offset of this block in the flash (a "key tweak").

- The specific tweak depends on the setting of ``FLASH_CRYPT_CONFIG`` eFuse. This is a 4 bit eFuse, where each bit enables XORing of a particular range of the key bits:

  - Bit 1, bits 0-66 of the key are XORed.
  - Bit 2, bits 67-131 of the key are XORed.
  - Bit 3, bits 132-194 of the key are XORed.
  - Bit 4, bits 195-256 of the key are XORed.

  It is recommended that ``FLASH_CRYPT_CONFIG`` is always left to set the default value `0xF`, so that all key bits are XORed with the block offset. See :ref:`setting-flash-crypt-config` for details.

- The high 19 bits of the block offset (bit 5 to bit 23) are XORed with the main flash encryption key. This range is chosen for two reasons: the maximum flash size is 16MB (24 bits), and each block is 32 bytes so the least significant 5 bits are always zero.

- There is a particular mapping from each of the 19 block offset bits to the 256 bits of the flash encryption key, to determine which bit is XORed with which. See the variable ``_FLASH_ENCRYPTION_TWEAK_PATTERN`` in the ``espsecure.py`` source code for the complete mapping.

- To see the full flash encryption algorithm implemented in Python, refer to the `_flash_encryption_operation()` function in the ``espsecure.py`` source code.
