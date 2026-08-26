
.. first_boot_enc

.. code-block:: none

    rst:0x1 (POWERON),boot:0xc (SPI_FAST_FLASH_BOOT)
    SPIWP:0xee
    mode:DIO, clock div:2
    load:0x4086c410,len:0xd5c
    load:0x4086e610,len:0x4584
    load:0x40875888,len:0x2bac
    entry 0x4086c410
    I (25) boot: ESP-IDF v5.1-dev-4270-g4bff4ed6e5-dirty 2nd stage bootloader
    I (25) boot: compile time Mar 27 2023 16:48:49
    I (27) boot: chip revision: v0.0
    I (30) boot.esp32c6: SPI Speed      : 40MHz
    I (35) boot.esp32c6: SPI Mode       : DIO
    I (40) boot.esp32c6: SPI Flash Size : 2MB
    I (44) boot: Enabling RNG early entropy source...
    W (50) bootloader_random: bootloader_random_enable() has not been implemented yet
    I (58) boot: Partition Table:
    I (62) boot: ## Label            Usage          Type ST Offset   Length
    I (69) boot:  0 nvs              WiFi data        01 02 0000a000 00006000
    I (76) boot:  1 storage          Unknown data     01 ff 00010000 00001000
    I (84) boot:  2 factory          factory app      00 00 00020000 00100000
    I (91) boot:  3 nvs_key          NVS keys         01 04 00120000 00001000
    I (99) boot:  4 custom_nvs       WiFi data        01 02 00121000 00006000
    I (106) boot: End of partition table
    I (110) esp_image: segment 0: paddr=00020020 vaddr=42018020 size=090e8h ( 37096) map
    I (126) esp_image: segment 1: paddr=00029110 vaddr=40800000 size=06f08h ( 28424) load
    I (134) esp_image: segment 2: paddr=00030020 vaddr=42000020 size=12fd8h ( 77784) map
    I (151) esp_image: segment 3: paddr=00043000 vaddr=40806f08 size=03c00h ( 15360) load
    I (158) boot: Loaded app from partition at offset 0x20000
    I (158) boot: Checking flash encryption...
    I (160) efuse: Batch mode of writing fields is enabled
    I (165) flash_encrypt: Generating new flash encryption key...
    I (174) efuse: Writing EFUSE_BLK_KEY0 with purpose 4
    W (178) flash_encrypt: Not disabling UART bootloader encryption
    I (184) flash_encrypt: Disable UART bootloader cache...
    I (190) flash_encrypt: Disable JTAG...
    I (197) efuse: BURN BLOCK4
    I (204) efuse: BURN BLOCK4 - OK (write block == read block)
    I (206) efuse: BURN BLOCK0
    I (212) efuse: BURN BLOCK0 - OK (all write block bits are set)
    I (216) efuse: Batch mode. Prepared fields are committed
    I (222) esp_image: segment 0: paddr=00000020 vaddr=4086c410 size=00d5ch (  3420)
    I (231) esp_image: segment 1: paddr=00000d84 vaddr=4086e610 size=04584h ( 17796)
    I (240) esp_image: segment 2: paddr=00005310 vaddr=40875888 size=02bach ( 11180)
    I (632) flash_encrypt: bootloader encrypted successfully
    I (679) flash_encrypt: partition table encrypted and loaded successfully
    I (680) flash_encrypt: Encrypting partition 1 at offset 0x10000 (length 0x1000)...
    I (732) flash_encrypt: Done encrypting
    I (732) esp_image: segment 0: paddr=00020020 vaddr=42018020 size=090e8h ( 37096) map
    I (741) esp_image: segment 1: paddr=00029110 vaddr=40800000 size=06f08h ( 28424)
    I (747) esp_image: segment 2: paddr=00030020 vaddr=42000020 size=12fd8h ( 77784) map
    I (765) esp_image: segment 3: paddr=00043000 vaddr=40806f08 size=03c00h ( 15360)
    I (769) flash_encrypt: Encrypting partition 2 at offset 0x20000 (length 0x100000)...
    I (13025) flash_encrypt: Done encrypting
    I (13025) flash_encrypt: Encrypting partition 3 at offset 0x120000 (length 0x1000)...
    I (13074) flash_encrypt: Done encrypting
    I (13075) efuse: BURN BLOCK0
    I (13077) efuse: BURN BLOCK0 - OK (all write block bits are set)
    I (13078) flash_encrypt: Flash encryption completed
    I (13083) boot: Resetting with flash encryption enabled...


------

.. already_en_enc

.. code-block:: none

    rst:0x3 (LP_SW_HPSYS),boot:0xc (SPI_FAST_FLASH_BOOT)
    Saved PC:0x4001974a
    SPIWP:0xee
    mode:DIO, clock div:2
    load:0x4086c410,len:0xd5c
    load:0x4086e610,len:0x4584
    load:0x40875888,len:0x2bac
    entry 0x4086c410
    I (24) boot: ESP-IDF v5.1-dev-4270-g4bff4ed6e5-dirty 2nd stage bootloader
    I (24) boot: compile time Mar 27 2023 16:48:49
    I (25) boot: chip revision: v0.0
    I (29) boot.esp32c6: SPI Speed      : 40MHz
    I (34) boot.esp32c6: SPI Mode       : DIO
    I (39) boot.esp32c6: SPI Flash Size : 2MB
    I (43) boot: Enabling RNG early entropy source...
    W (49) bootloader_random: bootloader_random_enable() has not been implemented yet
    I (57) boot: Partition Table:
    I (60) boot: ## Label            Usage          Type ST Offset   Length
    I (68) boot:  0 nvs              WiFi data        01 02 0000a000 00006000
    I (75) boot:  1 storage          Unknown data     01 ff 00010000 00001000
    I (83) boot:  2 factory          factory app      00 00 00020000 00100000
    I (90) boot:  3 nvs_key          NVS keys         01 04 00120000 00001000
    I (98) boot:  4 custom_nvs       WiFi data        01 02 00121000 00006000
    I (105) boot: End of partition table
    I (109) esp_image: segment 0: paddr=00020020 vaddr=42018020 size=090e8h ( 37096) map
    I (126) esp_image: segment 1: paddr=00029110 vaddr=40800000 size=06f08h ( 28424) load
    I (134) esp_image: segment 2: paddr=00030020 vaddr=42000020 size=12fd8h ( 77784) map
    I (152) esp_image: segment 3: paddr=00043000 vaddr=40806f08 size=03c00h ( 15360) load
    I (159) boot: Loaded app from partition at offset 0x20000
    I (159) boot: Checking flash encryption...
    I (160) flash_encrypt: flash encryption is enabled (1 plaintext flashes left)
    I (168) boot: Disabling RNG early entropy source...
    W (173) bootloader_random: bootloader_random_enable() has not been implemented yet
    I (193) cpu_start: Pro cpu up.
    W (202) clk: esp_perip_clk_init() has not been implemented yet
    I (208) cpu_start: Pro cpu start user code
    I (209) cpu_start: cpu freq: 160000000 Hz
    I (209) cpu_start: Application information:
    I (211) cpu_start: Project name:     flash_encryption
    I (217) cpu_start: App version:      v5.1-dev-4270-g4bff4ed6e5-dirty
    I (224) cpu_start: Compile time:     Mar 27 2023 16:49:00
    I (230) cpu_start: ELF file SHA256:  df1dd35054510e16...
    I (236) cpu_start: ESP-IDF:          v5.1-dev-4270-g4bff4ed6e5-dirty
    I (243) cpu_start: Min chip rev:     v0.0
    I (248) cpu_start: Max chip rev:     v0.99
    I (253) cpu_start: Chip rev:         v0.0
    I (258) heap_init: Initializing. RAM available for dynamic allocation:
    I (265) heap_init: At 4080B9E0 len 00070C30 (451 KiB): D/IRAM
    I (271) heap_init: At 4087C610 len 00002F54 (11 KiB): STACK/DIRAM
    I (278) heap_init: At 50000010 len 00003FF0 (15 KiB): RTCRAM
    I (285) spi_flash: detected chip: generic
    I (289) spi_flash: flash io: dio
    W (293) spi_flash: Detected size(4096k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
    W (306) flash_encrypt: Flash encryption mode is DEVELOPMENT (not secure)
    I (314) sleep: Configure to isolate all GPIO pins in sleep state
    I (320) sleep: Enable automatic switching of GPIO sleep configuration
    I (327) coexist: coex firmware version: 5315623
    I (333) coexist: coexist rom version 5b8dcfa
    I (338) app_start: Starting scheduler on CPU0
    I (342) main_task: Started on CPU0
    I (342) main_task: Calling app_main()

    Example to check Flash Encryption status
    This is esp32c6 chip with 1 CPU core(s), WiFi/BLE, silicon revision v0.0, 2MB external flash
    FLASH_CRYPT_CNT eFuse value is 1
    Flash encryption feature is enabled in DEVELOPMENT mode


------
