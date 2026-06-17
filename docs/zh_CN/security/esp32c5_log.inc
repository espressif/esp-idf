
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

.. first_boot_enc_km

.. code-block:: none

    ESP-ROM:esp32c5-eco3-20250704
    Build:Jul  4 2025
    rst:0x1 (POWERON),boot:0x18 (SPI_FAST_FLASH_BOOT)
    SPI mode:DIO, clock div:1
    load:0x40855820,len:0x3a60
    load:0x4084bba0,len:0xdf4
    load:0x4084e5a0,len:0x566c
    entry 0x4084bbaa
    I (23) boot: ESP-IDF v6.0-dev-3140-ge16a2fb13ad-dirt 2nd stage bootloader
    I (24) boot: compile time Oct 27 2025 15:35:09
    I (67) boot: chip revision: v1.1
    I (70) boot: efuse block revision: v0.3
    I (88) boot.esp32c5: SPI Speed      : 80MHz
    I (92) boot.esp32c5: SPI Mode       : DIO
    I (96) boot.esp32c5: SPI Flash Size : 2MB
    I (103) boot: Enabling RNG early entropy source...
    I (138) boot: Partition Table:
    I (141) boot: ## Label            Usage          Type ST Offset   Length
    I (154) boot:  0 nvs              WiFi data        01 02 0000e000 00006000
    I (168) boot:  1 phy_init         RF data          01 01 00014000 00001000
    I (182) boot:  2 factory          factory app      00 00 00020000 00100000
    I (189) boot: End of partition table
    I (283) esp_image: segment 0: paddr=00020020 vaddr=42070020 size=32f1ch (208668) map
    I (377) esp_image: segment 1: paddr=00052f44 vaddr=40800000 size=0a2a4h ( 41636) load
    I (445) esp_image: segment 2: paddr=0005d1f0 vaddr=4080a300 size=02ccch ( 11468) load
    I (512) esp_image: segment 3: paddr=0005fec4 vaddr=00000000 size=00154h (   340)
    I (580) esp_image: segment 4: paddr=00060020 vaddr=42000020 size=63804h (407556) map
    I (761) boot: Loaded app from partition at offset 0x20000
    I (764) boot: Checking flash encryption...
    I (788) efuse: Batch mode of writing fields is enabled
    I (807) flash_encrypt: Deploying new flash encryption key using Key Manager
    W (898) flash_encrypt: Not disabling UART bootloader encryption
    I (904) flash_encrypt: Disable UART bootloader cache...
    W (917) flash_encrypt: Not disabling JTAG - SECURITY COMPROMISED
    I (930) efuse: BURN BLOCK0
    I (935) efuse: BURN BLOCK0 - OK (all write block bits are set)
    I (938) efuse: Batch mode. Prepared fields are committed
    I (1028) esp_image: segment 0: paddr=00002020 vaddr=40855820 size=03a60h ( 14944)
    I (1096) esp_image: segment 1: paddr=00005a88 vaddr=4084bba0 size=00df4h (  3572)
    I (1165) esp_image: segment 2: paddr=00006884 vaddr=4084e5a0 size=0566ch ( 22124)
    I (1508) flash_encrypt: bootloader encrypted successfully
    I (1535) flash_encrypt: partition table encrypted and loaded successfully
    I (1616) esp_image: segment 0: paddr=00020020 vaddr=42070020 size=32f1ch (208668) map
    I (1711) esp_image: segment 1: paddr=00052f44 vaddr=40800000 size=0a2a4h ( 41636)
    I (1779) esp_image: segment 2: paddr=0005d1f0 vaddr=4080a300 size=02ccch ( 11468)
    I (1847) esp_image: segment 3: paddr=0005fec4 vaddr=00000000 size=00154h (   340)
    I (1916) esp_image: segment 4: paddr=00060020 vaddr=42000020 size=63804h (407556) map
    I (2096) flash_encrypt: Encrypting partition 2 at offset 0x20000 (length 0xa3850)...
    I (5652) flash_encrypt: Done encrypting
    I (5667) efuse: BURN BLOCK0
    I (5671) efuse: BURN BLOCK0 - OK (all write block bits are set)
    I (5675) flash_encrypt: Flash encryption completed
    I (5679) boot: Resetting with flash encryption enabled...


------


.. already_en_enc

.. code-block:: none

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


.. already_en_enc_km

.. code-block:: none

    ESP-ROM:esp32c5-eco3-20250704
    Build:Jul  4 2025
    rst:0x1 (POWERON),boot:0x18 (SPI_FAST_FLASH_BOOT)
    use sector0 for km info
    use KM derived key
    SPI mode:DIO, clock div:1
    load:0x40855820,len:0x3a94
    load:0x4084bba0,len:0xd84
    load:0x4084e5a0,len:0x5670
    entry 0x4084bbaa
    I (27) boot: ESP-IDF v6.0-dev-3139-gb813f413096-dirt 2nd stage bootloader
    I (28) boot: compile time Oct 27 2025 18:14:49
    W (39) MMU: mmu_ll_write_entry mmu_id=0, entry_id=511, mmu_val=0x00000000, target=1
    W (53) MMU: mmu_ll_write_entry mmu_id=0, entry_id=511, mmu_val=0x00000000, target=1
    W (67) MMU: mmu_ll_write_entry mmu_id=0, entry_id=511, mmu_val=0x00000000, target=1
    W (80) MMU: mmu_ll_write_entry mmu_id=0, entry_id=511, mmu_val=0x00000000, target=1
    W (94) MMU: mmu_ll_write_entry mmu_id=0, entry_id=511, mmu_val=0x00000000, target=1
    W (108) MMU: mmu_ll_write_entry mmu_id=0, entry_id=511, mmu_val=0x00000000, target=1
    I (116) boot: chip revision: v1.1
    I (119) boot: efuse block revision: v0.3
    I (137) boot.esp32c5: SPI Speed      : 80MHz
    I (141) boot.esp32c5: SPI Mode       : DIO
    I (145) boot.esp32c5: SPI Flash Size : 2MB
    I (149) boot: Enabling RNG early entropy source...
    W (163) MMU: mmu_ll_write_entry mmu_id=0, entry_id=0, mmu_val=0x00000000, target=1
    I (191) boot: Partition Table:
    I (194) boot: ## Label            Usage          Type ST Offset   Length
    I (208) boot:  0 nvs              WiFi data        01 02 0000e000 00006000
    I (222) boot:  1 phy_init         RF data          01 01 00014000 00001000
    I (235) boot:  2 factory          factory app      00 00 00020000 00100000
    I (242) boot: End of partition table
    I (396) esp_image: segment 0: paddr=00020020 vaddr=42070020 size=32f1ch (208668) map
    I (539) esp_image: segment 1: paddr=00052f44 vaddr=40800000 size=0a2a4h ( 41636) load
    I (629) esp_image: segment 2: paddr=0005d1f0 vaddr=4080a300 size=02ccch ( 11468) load
    I (719) esp_image: segment 3: paddr=0005fec4 vaddr=00000000 size=00154h (   340)
    I (816) esp_image: segment 4: paddr=00060020 vaddr=42000020 size=6378ch (407436) map
    I (1114) boot: Loaded app from partition at offset 0x20000
    I (1116) boot: Checking flash encryption...
    I (1133) flash_encrypt: flash encryption is enabled (1 plaintext flashes left)
    I (1140) boot: Disabling RNG early entropy source...
    I (1301) MSPI Timing: Enter flash timing tuning
    I (1434) cpu_start: Unicore app
    I (1468) cpu_start: GPIO 12 and 11 are used as console UART I/O pins
    I (1479) cpu_start: Pro cpu start user code
    I (1483) cpu_start: cpu freq: 240000000 Hz
    I (1493) app_init: Application information:
    I (1496) app_init: Project name:     mbedtls_test
    I (1501) app_init: App version:      qa-test-esp32c61-master-2025070
    I (1507) app_init: Compile time:     Oct 27 2025 15:56:18
    I (1512) app_init: ELF file SHA256:  c5f7f520c...
    I (1517) app_init: ESP-IDF:          v6.0-dev-3140-ge16a2fb13ad-dirt
    I (1528) efuse_init: Min chip rev:     v1.0
    I (1532) efuse_init: Max chip rev:     v1.99
    I (1536) efuse_init: Chip rev:         v1.1
    I (1669) heap_init: Initializing. RAM available for dynamic allocation:
    I (1680) heap_init: At 4080FCB0 len 0004C8F0 (306 KiB): RAM
    I (1685) heap_init: At 4085C5A0 len 00002F58 (11 KiB): RAM
    I (1695) heap_init: At 50000000 len 00003FE8 (15 KiB): RTCRAM
    I (1800) spi_flash: detected chip: generic
    I (1804) spi_flash: flash io: dio
    W (1807) spi_flash: Detected size(8192k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
    W (1833) flash_encrypt: Flash encryption mode is DEVELOPMENT (not secure)
    I (1867) sleep_gpio: Configure to isolate all GPIO pins in sleep state
    I (1873) sleep_gpio: Enable automatic switching of GPIO sleep configuration
    I (1926) main_task: Started on CPU0
    I (1926) main_task: Calling app_main()
    I (1936) main_task: Returned from app_main()


------
