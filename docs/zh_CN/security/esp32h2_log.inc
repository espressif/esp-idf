
.. first_boot_enc

.. code-block:: none

    rst:0x1 (POWERON),boot:0xc (SPI_FAST_FLASH_BOOT)
    SPIWP:0xee
    mode:DIO, clock div:1
    load:0x4083cfd0,len:0xc0c
    load:0x4083efd0,len:0x45c0
    load:0x40846248,len:0x2b48
    entry 0x4083cfd0
    I (25) boot: ESP-IDF v5.1-dev-4270-g4bff4ed6e5 2nd stage bootloader
    I (25) boot: compile time Mar 27 2023 16:40:47
    I (26) boot: chip revision: v0.0
    I (29) boot.esp32h2: SPI Speed      : 64MHz
    I (34) boot.esp32h2: SPI Mode       : DIO
    I (39) boot.esp32h2: SPI Flash Size : 2MB
    I (44) boot: Enabling RNG early entropy source...
    W (49) bootloader_random: bootloader_random_enable() has not been implemented yet
    I (57) boot: Partition Table:
    I (61) boot: ## Label            Usage          Type ST Offset   Length
    I (68) boot:  0 nvs              WiFi data        01 02 0000a000 00006000
    I (76) boot:  1 storage          Unknown data     01 ff 00010000 00001000
    I (83) boot:  2 factory          factory app      00 00 00020000 00100000
    I (90) boot:  3 nvs_key          NVS keys         01 04 00120000 00001000
    I (98) boot:  4 custom_nvs       WiFi data        01 02 00121000 00006000
    I (106) boot: End of partition table
    I (110) esp_image: segment 0: paddr=00020020 vaddr=42020020 size=096e0h ( 38624) map
    I (128) esp_image: segment 1: paddr=00029708 vaddr=40800000 size=06910h ( 26896) load
    I (135) esp_image: segment 2: paddr=00030020 vaddr=42000020 size=1b708h (112392) map
    I (163) esp_image: segment 3: paddr=0004b730 vaddr=40806910 size=02a08h ( 10760) load
    I (167) esp_image: segment 4: paddr=0004e140 vaddr=40809320 size=00e5ch (  3676) load
    I (172) boot: Loaded app from partition at offset 0x20000
    I (175) boot: Checking flash encryption...
    I (180) efuse: Batch mode of writing fields is enabled
    I (186) flash_encrypt: Generating new flash encryption key...
    I (194) efuse: Writing EFUSE_BLK_KEY0 with purpose 4
    W (198) flash_encrypt: Not disabling UART bootloader encryption
    I (204) flash_encrypt: Disable JTAG...
    I (212) efuse: BURN BLOCK4
    I (219) efuse: BURN BLOCK4 - OK (write block == read block)
    I (221) efuse: BURN BLOCK0
    I (227) efuse: BURN BLOCK0 - OK (write block == read block)
    I (231) efuse: Batch mode. Prepared fields are committed
    I (236) esp_image: segment 0: paddr=00000020 vaddr=4083cfd0 size=00c0ch (  3084)
    I (245) esp_image: segment 1: paddr=00000c34 vaddr=4083efd0 size=045c0h ( 17856)
    I (255) esp_image: segment 2: paddr=000051fc vaddr=40846248 size=02b48h ( 11080)
    I (381) flash_encrypt: bootloader encrypted successfully
    I (396) flash_encrypt: partition table encrypted and loaded successfully
    I (397) flash_encrypt: Encrypting partition 1 at offset 0x10000 (length 0x1000)...
    I (411) flash_encrypt: Done encrypting
    I (412) esp_image: segment 0: paddr=00020020 vaddr=42020020 size=096e0h ( 38624) map
    I (423) esp_image: segment 1: paddr=00029708 vaddr=40800000 size=06910h ( 26896)
    I (430) esp_image: segment 2: paddr=00030020 vaddr=42000020 size=1b708h (112392) map
    I (458) esp_image: segment 3: paddr=0004b730 vaddr=40806910 size=02a08h ( 10760)
    I (461) esp_image: segment 4: paddr=0004e140 vaddr=40809320 size=00e5ch (  3676)
    I (464) flash_encrypt: Encrypting partition 2 at offset 0x20000 (length 0x100000)...
    I (3600) flash_encrypt: Done encrypting
    I (3600) flash_encrypt: Encrypting partition 3 at offset 0x120000 (length 0x1000)...
    I (3612) flash_encrypt: Done encrypting
    I (3613) efuse: BURN BLOCK0
    I (3616) efuse: BURN BLOCK0 - OK (all write block bits are set)
    I (3617) flash_encrypt: Flash encryption completed
    I (3622) boot: Resetting with flash encryption enabled...


------

.. already_en_enc

.. code-block:: none

    rst:0x3 (LP_SW_HPSYS),boot:0xc (SPI_FAST_FLASH_BOOT)
    Saved PC:0x400031a6
    SPIWP:0xee
    mode:DIO, clock div:1
    load:0x4083cfd0,len:0xc0c
    load:0x4083efd0,len:0x45c0
    load:0x40846248,len:0x2b48
    entry 0x4083cfd0
    I (27) boot: ESP-IDF v5.1-dev-4270-g4bff4ed6e5 2nd stage bootloader
    I (28) boot: compile time Mar 27 2023 16:40:47
    I (28) boot: chip revision: v0.0
    I (32) boot.esp32h2: SPI Speed      : 64MHz
    I (37) boot.esp32h2: SPI Mode       : DIO
    I (41) boot.esp32h2: SPI Flash Size : 2MB
    I (46) boot: Enabling RNG early entropy source...
    W (51) bootloader_random: bootloader_random_enable() has not been implemented yet
    I (60) boot: Partition Table:
    I (63) boot: ## Label            Usage          Type ST Offset   Length
    I (71) boot:  0 nvs              WiFi data        01 02 0000a000 00006000
    I (78) boot:  1 storage          Unknown data     01 ff 00010000 00001000
    I (86) boot:  2 factory          factory app      00 00 00020000 00100000
    I (93) boot:  3 nvs_key          NVS keys         01 04 00120000 00001000
    I (100) boot:  4 custom_nvs       WiFi data        01 02 00121000 00006000
    I (108) boot: End of partition table
    I (112) esp_image: segment 0: paddr=00020020 vaddr=42020020 size=096e0h ( 38624) map
    I (131) esp_image: segment 1: paddr=00029708 vaddr=40800000 size=06910h ( 26896) load
    I (140) esp_image: segment 2: paddr=00030020 vaddr=42000020 size=1b708h (112392) map
    I (171) esp_image: segment 3: paddr=0004b730 vaddr=40806910 size=02a08h ( 10760) load
    I (175) esp_image: segment 4: paddr=0004e140 vaddr=40809320 size=00e5ch (  3676) load
    I (180) boot: Loaded app from partition at offset 0x20000
    I (183) boot: Checking flash encryption...
    I (188) flash_encrypt: flash encryption is enabled (1 plaintext flashes left)
    I (195) boot: Disabling RNG early entropy source...
    W (201) bootloader_random: bootloader_random_disable() has not been implemented yet
    I (221) cpu_start: Pro cpu up.
    W (229) clk: esp_perip_clk_init() has not been implemented yet
    I (236) cpu_start: Pro cpu start user code
    I (236) cpu_start: cpu freq: 96000000 Hz
    I (237) cpu_start: Application information:
    I (239) cpu_start: Project name:     flash_encryption
    I (245) cpu_start: App version:      v5.1-dev-4270-g4bff4ed6e5
    I (251) cpu_start: Compile time:     Mar 27 2023 16:40:56
    I (257) cpu_start: ELF file SHA256:  42c8d825941d8050...
    I (263) cpu_start: ESP-IDF:          v5.1-dev-4270-g4bff4ed6e5
    I (270) cpu_start: Min chip rev:     v0.0
    I (274) cpu_start: Max chip rev:     v0.99
    I (279) cpu_start: Chip rev:         v0.0
    I (284) heap_init: Initializing. RAM available for dynamic allocation:
    I (291) heap_init: At 4080B010 len 00042370 (264 KiB): D/IRAM
    I (298) heap_init: At 4084D380 len 00002B60 (10 KiB): STACK/DIRAM
    I (305) spi_flash: detected chip: generic
    I (309) spi_flash: flash io: dio
    W (313) spi_flash: Detected size(4096k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
    W (326) flash_encrypt: Flash encryption mode is DEVELOPMENT (not secure)
    I (334) app_start: Starting scheduler on CPU0
    I (339) main_task: Started on CPU0
    I (339) main_task: Calling app_main()

    Example to check Flash Encryption status
    This is esp32h2 chip with 1 CPU core(s), WiFi/BLE, silicon revision v0.0, 2MB external flash
    FLASH_CRYPT_CNT eFuse value is 1
    Flash encryption feature is enabled in DEVELOPMENT mode


------
