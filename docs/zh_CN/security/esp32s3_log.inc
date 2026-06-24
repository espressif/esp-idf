
.. first_boot_enc

.. code-block:: none

    ESP-ROM:esp32s3-20210327
    Build:Mar 27 2021
    rst:0x1 (POWERON),boot:0x8 (SPI_FAST_FLASH_BOOT)
    SPIWP:0xee
    mode:DIO, clock div:1
    load:0x3fcd0270,len:0x2598
    load:0x403b6000,len:0x878
    load:0x403ba000,len:0x3dd4
    entry 0x403b61c0
    I (27) boot: ESP-IDF v4.4-dev-2003-g72fdecc1b7-dirty 2nd stage bootloader
    I (28) boot: compile time 14:15:37
    I (28) boot: chip revision: 0
    I (32) boot.esp32s3: SPI Speed      : 80MHz
    I (36) boot.esp32s3: SPI Mode       : DIO
    I (41) boot.esp32s3: SPI Flash Size : 2MB
    I (46) boot: Enabling RNG early entropy source...
    I (58) boot: Partition Table:
    I (62) boot: ## Label            Usage          Type ST Offset   Length
    I (69) boot:  0 nvs              WiFi data        01 02 0000a000 00006000
    I (76) boot:  1 storage          Unknown data     01 ff 00010000 00001000
    I (84) boot:  2 factory          factory app      00 00 00020000 00100000
    I (91) boot:  3 nvs_key          NVS keys         01 04 00120000 00001000
    I (99) boot: End of partition table
    I (103) esp_image: segment 0: paddr=00020020 vaddr=3c020020 size=08118h ( 33048) map
    I (117) esp_image: segment 1: paddr=00028140 vaddr=3fc8fa30 size=023f4h (  9204) load
    I (122) esp_image: segment 2: paddr=0002a53c vaddr=40374000 size=05adch ( 23260) load
    I (134) esp_image: segment 3: paddr=00030020 vaddr=42000020 size=1a710h (108304) map
    I (156) esp_image: segment 4: paddr=0004a738 vaddr=40379adc size=05f48h ( 24392) load
    I (162) esp_image: segment 5: paddr=00050688 vaddr=600fe000 size=00010h (    16) load
    I (167) boot: Loaded app from partition at offset 0x20000
    I (168) boot: Checking flash encryption...
    I (173) efuse: Batch mode of writing fields is enabled
    I (179) flash_encrypt: Generating new flash encryption key...
    I (188) efuse: Writing EFUSE_BLK_KEY0 with purpose 4
    W (194) flash_encrypt: Not disabling UART bootloader encryption
    I (197) flash_encrypt: Disable UART bootloader cache...
    I (203) flash_encrypt: Disable JTAG...
    I (212) efuse: Batch mode. Prepared fields are committed
    I (214) esp_image: segment 0: paddr=00000020 vaddr=3fcd0270 size=02598h (  9624)
    I (223) esp_image: segment 1: paddr=000025c0 vaddr=403b6000 size=00878h (  2168)
    I (230) esp_image: segment 2: paddr=00002e40 vaddr=403ba000 size=03dd4h ( 15828)
    I (534) flash_encrypt: bootloader encrypted successfully
    I (578) flash_encrypt: partition table encrypted and loaded successfully
    I (578) flash_encrypt: Encrypting partition 1 at offset 0x10000 (length 0x1000)...
    I (628) flash_encrypt: Done encrypting
    I (629) esp_image: segment 0: paddr=00020020 vaddr=3c020020 size=08118h ( 33048) map
    I (636) esp_image: segment 1: paddr=00028140 vaddr=3fc8fa30 size=023f4h (  9204)
    I (640) esp_image: segment 2: paddr=0002a53c vaddr=40374000 size=05adch ( 23260)
    I (651) esp_image: segment 3: paddr=00030020 vaddr=42000020 size=1a710h (108304) map
    I (675) esp_image: segment 4: paddr=0004a738 vaddr=40379adc size=05f48h ( 24392)
    I (679) esp_image: segment 5: paddr=00050688 vaddr=600fe000 size=00010h (    16)
    I (680) flash_encrypt: Encrypting partition 2 at offset 0x20000 (length 0x100000)...
    I (11571) flash_encrypt: Done encrypting
    I (11571) flash_encrypt: Encrypting partition 3 at offset 0x120000 (length 0x1000)...
    I (11617) flash_encrypt: Done encrypting
    I (11618) flash_encrypt: Flash encryption completed
    I (11623) boot: Resetting with flash encryption enabled...

------

.. already_en_enc

.. code-block:: none

    ESP-ROM:esp32s3-20210327
    Build:Mar 27 2021
    rst:0x3 (RTC_SW_SYS_RST),boot:0x8 (SPI_FAST_FLASH_BOOT)
    Saved PC:0x403bb1d6
    SPIWP:0xee
    mode:DIO, clock div:1
    load:0x3fcd0270,len:0x2598
    load:0x403b6000,len:0x878
    load:0x403ba000,len:0x3dd4
    entry 0x403b61c0
    I (35) boot: ESP-IDF v4.4-dev-2003-g72fdecc1b7-dirty 2nd stage bootloader
    I (35) boot: compile time 14:15:37
    I (35) boot: chip revision: 0
    I (39) boot.esp32s3: SPI Speed      : 80MHz
    I (44) boot.esp32s3: SPI Mode       : DIO
    I (48) boot.esp32s3: SPI Flash Size : 2MB
    I (53) boot: Enabling RNG early entropy source...
    I (65) boot: Partition Table:
    I (69) boot: ## Label            Usage          Type ST Offset   Length
    I (76) boot:  0 nvs              WiFi data        01 02 0000a000 00006000
    I (84) boot:  1 storage          Unknown data     01 ff 00010000 00001000
    I (91) boot:  2 factory          factory app      00 00 00020000 00100000
    I (99) boot:  3 nvs_key          NVS keys         01 04 00120000 00001000
    I (106) boot: End of partition table
    I (110) esp_image: segment 0: paddr=00020020 vaddr=3c020020 size=08118h ( 33048) map
    I (126) esp_image: segment 1: paddr=00028140 vaddr=3fc8fa30 size=023f4h (  9204) load
    I (129) esp_image: segment 2: paddr=0002a53c vaddr=40374000 size=05adch ( 23260) load
    I (141) esp_image: segment 3: paddr=00030020 vaddr=42000020 size=1a710h (108304) map
    I (166) esp_image: segment 4: paddr=0004a738 vaddr=40379adc size=05f48h ( 24392) load
    I (172) esp_image: segment 5: paddr=00050688 vaddr=600fe000 size=00010h (    16) load
    I (177) boot: Loaded app from partition at offset 0x20000
    I (178) boot: Checking flash encryption...
    I (183) flash_encrypt: flash encryption is enabled (1 plaintext flashes left)
    I (190) boot: Disabling RNG early entropy source...
    I (214) cpu_start: Pro cpu up.
    I (214) cpu_start: Starting app cpu, entry point is 0x40374fa8
    I (0) cpu_start: App cpu up.
    I (228) cpu_start: Pro cpu start user code
    I (228) cpu_start: cpu freq: 160000000
    I (228) cpu_start: Application information:
    I (231) cpu_start: Project name:     flash_encryption
    I (237) cpu_start: App version:      v4.4-dev-2003-g72fdecc1b7-dirty
    I (244) cpu_start: Compile time:     Jul 12 2021 14:15:34
    I (250) cpu_start: ELF file SHA256:  a7e6343c6a1c2215...
    I (256) cpu_start: ESP-IDF:          v4.4-dev-2003-g72fdecc1b7-dirty
    I (263) heap_init: Initializing. RAM available for dynamic allocation:
    I (270) heap_init: At 3FC92810 len 0004D7F0 (309 KiB): D/IRAM
    I (277) heap_init: At 3FCE0000 len 0000EE34 (59 KiB): STACK/DRAM
    I (283) heap_init: At 3FCF0000 len 00008000 (32 KiB): DRAM
    I (290) spi_flash: detected chip: generic
    I (294) spi_flash: flash io: dio
    W (298) spi_flash: Detected size(8192k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
    I (311) flash_encrypt: Flash encryption mode is DEVELOPMENT (not secure)
    I (318) cpu_start: Starting scheduler on PRO CPU.
    I (0) cpu_start: Starting scheduler on APP CPU.

    Example to check Flash Encryption status
    This is esp32s3 chip with 2 CPU core(s), WiFi/BLE, silicon revision 0, 2MB external flash
    FLASH_CRYPT_CNT eFuse value is 1
    Flash encryption feature is enabled in DEVELOPMENT mode

------
