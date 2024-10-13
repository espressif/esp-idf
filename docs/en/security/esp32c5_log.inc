
.. first_boot_enc

.. code-block:: none

    rst:0x1 (POWERON),boot:0x3d (SPI_FAST_FLASH_BOOT)
    SPI mode:DIO, clock div:2
    load:0x40855c10,len:0x2c34
    load:0x4084c7a0,len:0x6f8
    load:0x4084e9a0,len:0x41c4
    entry 0x4084c804
    I (32) boot: ESP-IDF v5.3-dev-3860-g5d36288649-dirty 2nd stage bootloader
    I (33) boot: compile time May  7 2024 17:31:13
    I (34) boot: chip revision: v0.0
    I (37) boot.esp32c5: SPI Speed      : 40MHz
    I (42) boot.esp32c5: SPI Mode       : DIO
    I (47) boot.esp32c5: SPI Flash Size : 2MB
    I (52) boot: Enabling RNG early entropy source...
    I (64) boot: Partition Table:
    I (68) boot: ## Label            Usage          Type ST Offset   Length
    I (75) boot:  0 nvs              WiFi data        01 02 0000e000 00006000
    I (82) boot:  1 storage          Unknown data     01 ff 00014000 00001000
    I (90) boot:  2 factory          factory app      00 00 00020000 00100000
    I (97) boot:  3 nvs_key          NVS keys         01 04 00120000 00001000
    I (105) boot:  4 custom_nvs       WiFi data        01 02 00121000 00006000
    I (113) boot: End of partition table
    I (117) esp_image: segment 0: paddr=00020020 vaddr=42010020 size=09674h ( 38516) map
    I (169) esp_image: segment 1: paddr=0002969c vaddr=40800000 size=0697ch ( 27004) load
    I (197) esp_image: segment 2: paddr=00030020 vaddr=42000020 size=0f4fch ( 62716) map
    I (256) esp_image: segment 3: paddr=0003f524 vaddr=4080697c size=00c28h (  3112) load
    I (261) esp_image: segment 4: paddr=00040154 vaddr=408075b0 size=00d18h (  3352) load
    I (269) boot: Loaded app from partition at offset 0x20000
    I (270) boot: Checking flash encryption...
    I (273) efuse: Batch mode of writing fields is enabled
    I (279) flash_encrypt: Generating new flash encryption key...
    I (297) efuse: Writing EFUSE_BLK_KEY0 with purpose 4
    I (300) flash_encrypt: Disable UART bootloader encryption...
    I (306) flash_encrypt: Disable JTAG...
    I (312) efuse: BURN BLOCK4
    I (317) efuse: BURN BLOCK4 - OK (write block == read block)
    I (320) efuse: BURN BLOCK0
    I (326) efuse: BURN BLOCK0 - OK (write block == read block)
    I (330) efuse: Batch mode. Prepared fields are committed
    I (335) esp_image: segment 0: paddr=00002020 vaddr=40855c10 size=02c34h ( 11316)
    I (353) esp_image: segment 1: paddr=00004c5c vaddr=4084c7a0 size=006f8h (  1784)
    I (356) esp_image: segment 2: paddr=0000535c vaddr=4084e9a0 size=041c4h ( 16836)
    I (1091) flash_encrypt: bootloader encrypted successfully
    I (1182) flash_encrypt: partition table encrypted and loaded successfully
    I (1183) flash_encrypt: Encrypting partition 1 at offset 0x14000 (length 0x1000)...
    I (1271) flash_encrypt: Done encrypting
    I (1272) esp_image: segment 0: paddr=00020020 vaddr=42010020 size=09674h ( 38516) map
    I (1309) esp_image: segment 1: paddr=0002969c vaddr=40800000 size=0697ch ( 27004)
    I (1336) esp_image: segment 2: paddr=00030020 vaddr=42000020 size=0f4fch ( 62716) map
    I (1395) esp_image: segment 3: paddr=0003f524 vaddr=4080697c size=00c28h (  3112)
    I (1400) esp_image: segment 4: paddr=00040154 vaddr=408075b0 size=00d18h (  3352)
    I (1405) flash_encrypt: Encrypting partition 2 at offset 0x20000 (length 0x100000)...
    I (24123) flash_encrypt: Done encrypting
    I (24124) flash_encrypt: Encrypting partition 3 at offset 0x120000 (length 0x1000)...
    I (24215) flash_encrypt: Done encrypting
    I (24216) flash_encrypt: Setting CRYPT_CNT for permanent encryption
    I (24217) efuse: BURN BLOCK0
    I (24221) efuse: BURN BLOCK0 - OK (all write block bits are set)
    I (24226) flash_encrypt: Flash encryption completed
    I (24231) boot: Resetting with flash encryption enabled...


------

.. already_en_enc

.. code-block:: none

    rst:0x1 (POWERON),boot:0x3d (SPI_FAST_FLASH_BOOT)
    SPI mode:DIO, clock div:2
    load:0x40855c10,len:0x2be8
    load:0x4084c7a0,len:0x6f8
    load:0x4084e9a0,len:0x418c
    entry 0x4084c804
    I (32) boot: ESP-IDF v5.3-dev-3860-g5d36288649 2nd stage bootloader
    I (33) boot: compile time May  7 2024 17:24:43
    I (34) boot: chip revision: v0.0
    I (37) boot.esp32c5: SPI Speed      : 40MHz
    I (42) boot.esp32c5: SPI Mode       : DIO
    I (46) boot.esp32c5: SPI Flash Size : 2MB
    I (51) boot: Enabling RNG early entropy source...
    I (64) boot: Partition Table:
    I (67) boot: ## Label            Usage          Type ST Offset   Length
    I (74) boot:  0 nvs              WiFi data        01 02 0000e000 00006000
    I (82) boot:  1 storage          Unknown data     01 ff 00014000 00001000
    I (89) boot:  2 factory          factory app      00 00 00020000 00100000
    I (97) boot:  3 nvs_key          NVS keys         01 04 00120000 00001000
    I (104) boot:  4 custom_nvs       WiFi data        01 02 00121000 00006000
    I (113) boot: End of partition table
    I (116) esp_image: segment 0: paddr=00020020 vaddr=42010020 size=095c4h ( 38340) map
    I (169) esp_image: segment 1: paddr=000295ec vaddr=40800000 size=06a2ch ( 27180) load
    I (197) esp_image: segment 2: paddr=00030020 vaddr=42000020 size=0f4d4h ( 62676) map
    I (256) esp_image: segment 3: paddr=0003f4fc vaddr=40806a2c size=00b78h (  2936) load
    I (261) esp_image: segment 4: paddr=0004007c vaddr=408075b0 size=00d18h (  3352) load
    I (269) boot: Loaded app from partition at offset 0x20000
    I (270) boot: Checking flash encryption...
    I (273) efuse: Batch mode of writing fields is enabled
    I (278) flash_encrypt: Generating new flash encryption key...
    I (295) efuse: Writing EFUSE_BLK_KEY0 with purpose 4
    W (300) flash_encrypt: Not disabling UART bootloader encryption
    I (305) flash_encrypt: Disable JTAG...
    I (312) efuse: BURN BLOCK4
    I (317) efuse: BURN BLOCK4 - OK (write block == read block)
    I (319) efuse: BURN BLOCK0
    I (325) efuse: BURN BLOCK0 - OK (all write block bits are set)
    I (330) efuse: Batch mode. Prepared fields are committed
    I (335) esp_image: segment 0: paddr=00002020 vaddr=40855c10 size=02be8h ( 11240)
    I (353) esp_image: segment 1: paddr=00004c10 vaddr=4084c7a0 size=006f8h (  1784)
    I (356) esp_image: segment 2: paddr=00005310 vaddr=4084e9a0 size=0418ch ( 16780)
    I (1131) flash_encrypt: bootloader encrypted successfully
    I (1229) flash_encrypt: partition table encrypted and loaded successfully
    I (1230) flash_encrypt: Encrypting partition 1 at offset 0x14000 (length 0x1000)...
    I (1325) flash_encrypt: Done encrypting
    I (1325) esp_image: segment 0: paddr=00020020 vaddr=42010020 size=095c4h ( 38340) map
    I (1362) esp_image: segment 1: paddr=000295ec vaddr=40800000 size=06a2ch ( 27180)
    I (1389) esp_image: segment 2: paddr=00030020 vaddr=42000020 size=0f4d4h ( 62676) map
    I (1448) esp_image: segment 3: paddr=0003f4fc vaddr=40806a2c size=00b78h (  2936)
    I (1453) esp_image: segment 4: paddr=0004007c vaddr=408075b0 size=00d18h (  3352)
    I (1458) flash_encrypt: Encrypting partition 2 at offset 0x20000 (length 0x100000)...
    I (24332) flash_encrypt: Done encrypting
    I (24332) flash_encrypt: Encrypting partition 3 at offset 0x120000 (length 0x1000)...
    I (24422) flash_encrypt: Done encrypting
    I (24423) efuse: BURN BLOCK0
    I (24425) efuse: BURN BLOCK0 - OK (all write block bits are set)
    I (24427) flash_encrypt: Flash encryption completed
    I (24431) boot: Resetting with flash encryption enabled...
    ESP-ROM:esp32c5-20240329
    Build:Mar 29 2024
    rst:0x3 (RTC_SW_HPSYS),boot:0x3d (SPI_FAST_FLASH_BOOT)
    Core0 Saved PC:0x4084fe1c
    use legacy efuse key
    SPI mode:DIO, clock div:2
    load:0x40855c10,len:0x2be8
    load:0x4084c7a0,len:0x6f8
    load:0x4084e9a0,len:0x418c
    entry 0x4084c804
    I (39) boot: ESP-IDF v5.3-dev-3860-g5d36288649 2nd stage bootloader
    I (40) boot: compile time May  7 2024 17:24:43
    I (41) boot: chip revision: v0.0
    I (44) boot.esp32c5: SPI Speed      : 40MHz
    I (48) boot.esp32c5: SPI Mode       : DIO
    I (53) boot.esp32c5: SPI Flash Size : 2MB
    I (58) boot: Enabling RNG early entropy source...
    I (70) boot: Partition Table:
    I (74) boot: ## Label            Usage          Type ST Offset   Length
    I (81) boot:  0 nvs              WiFi data        01 02 0000e000 00006000
    I (89) boot:  1 storage          Unknown data     01 ff 00014000 00001000
    I (96) boot:  2 factory          factory app      00 00 00020000 00100000
    I (103) boot:  3 nvs_key          NVS keys         01 04 00120000 00001000
    I (111) boot:  4 custom_nvs       WiFi data        01 02 00121000 00006000
    I (119) boot: End of partition table
    I (123) esp_image: segment 0: paddr=00020020 vaddr=42010020 size=095c4h ( 38340) map
    I (177) esp_image: segment 1: paddr=000295ec vaddr=40800000 size=06a2ch ( 27180) load
    I (207) esp_image: segment 2: paddr=00030020 vaddr=42000020 size=0f4d4h ( 62676) map
    I (269) esp_image: segment 3: paddr=0003f4fc vaddr=40806a2c size=00b78h (  2936) load
    I (274) esp_image: segment 4: paddr=0004007c vaddr=408075b0 size=00d18h (  3352) load
    I (283) boot: Loaded app from partition at offset 0x20000
    I (283) boot: Checking flash encryption...
    I (286) flash_encrypt: flash encryption is enabled (1 plaintext flashes left)
    I (294) boot: Disabling RNG early entropy source...
    I (318) cpu_start: Unicore app
    I (326) cpu_start: Pro cpu start user code
    I (330) cpu_start: cpu freq: 40000000 Hz
    I (335) app_init: Application information:
    I (340) app_init: Project name:     flash_encryption
    I (345) app_init: App version:      qa-test-v5.3-20240419-267-g5d36
    I (352) app_init: Compile time:     May  7 2024 17:24:35
    I (358) app_init: ELF file SHA256:  712933cde...
    I (364) app_init: ESP-IDF:          v5.3-dev-3860-g5d36288649
    I (370) efuse_init: Min chip rev:     v0.0
    I (375) efuse_init: Max chip rev:     v0.99
    I (380) efuse_init: Chip rev:         v0.0
    I (385) heap_init: Initializing. RAM available for dynamic allocation:
    I (392) heap_init: At 408091A0 len 00051730 (325 KiB): RAM
    I (398) heap_init: At 4085A8D0 len 00004C30 (19 KiB): RAM
    I (404) heap_init: At 50000000 len 00003FE8 (15 KiB): RTCRAM
    I (426) spi_flash: detected chip: generic
    I (427) spi_flash: flash io: dio
    W (431) spi_flash: Detected size(4096k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
    W (445) flash_encrypt: Flash encryption mode is DEVELOPMENT (not secure)
    I (452) nvs_sec_provider: NVS Encryption - Registering Flash encryption-based scheme...
    I (466) main_task: Started on CPU0
    I (466) main_task: Calling app_main()

    Example to check Flash Encryption status
    This is esp32c5 chip with 1 CPU core(s), WiFi/BLE, silicon revision v0.0, 2MB external flash
    FLASH_CRYPT_CNT eFuse value is 1
    Flash encryption feature is enabled in DEVELOPMENT mode


------
