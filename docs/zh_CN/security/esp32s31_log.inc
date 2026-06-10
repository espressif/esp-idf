
.. first_boot_enc

.. code-block:: none

    ESP-ROM:esp32s31-20251218
    Build:Jan 21 2026
    rst:0x1 (POWERON),boot:0x58 (SPI_FAST_FLASH_BOOT)
    SPI mode:DIO, clock div:1
    load:0x2f074230,len:0x2944
    load:0x2f06a2b0,len:0xa20
    load:0x2f06cfb0,len:0x4690
    entry 0x2f06a304
    I (33) boot: ESP-IDF v6.1-dev-5047-ga20ab68fd55-dirt 2nd stage bootloader
    I (34) boot: compile time May 22 2026 17:30:12
    I (35) boot: Multicore bootloader
    I (38) boot: chip revision: v0.0
    I (39) boot: efuse block revision: v0.0
    I (42) boot.esp32s31: SPI Speed      : 80MHz
    I (46) boot.esp32s31: SPI Mode       : DIO
    I (50) boot.esp32s31: SPI Flash Size : 4MB
    I (54) boot: Enabling RNG early entropy source...
    I (59) boot: Partition Table:
    I (61) boot: ## Label            Usage          Type ST Offset   Length
    I (67) boot:  0 nvs              WiFi data        01 02 0000e000 00006000
    I (74) boot:  1 storage          Unknown data     01 ff 00014000 00001000
    I (80) boot:  2 factory          factory app      00 00 00020000 00100000
    I (87) boot:  3 nvs_key          NVS keys         01 04 00120000 00001000
    I (93) boot:  4 custom_nvs       WiFi data        01 02 00121000 00006000
    I (100) boot:  5 fat_encrypted    Unknown data     01 81 00127000 00096000
    I (107) boot:  6 fat_not_encr     Unknown data     01 81 001bd000 00096000
    I (115) boot: End of partition table
    I (117) esp_image: segment 0: paddr=00020020 vaddr=40030020 size=09cb0h ( 40112) map
    I (145) esp_image: segment 1: paddr=00029cd8 vaddr=2f000000 size=06340h ( 25408) load
    I (157) esp_image: segment 2: paddr=00030020 vaddr=40000020 size=21c2ch (138284) map
    I (201) esp_image: segment 3: paddr=00051c54 vaddr=2f006340 size=03814h ( 14356) load
    I (209) esp_image: segment 4: paddr=00055470 vaddr=2f009b80 size=02508h (  9480) load
    I (221) boot: Loaded app from partition at offset 0x20000
    I (222) boot: Checking flash encryption...
    I (223) efuse: Batch mode of writing fields is enabled
    I (224) flash_encrypt: Generating new flash encryption key...
    I (244) efuse: Writing EFUSE_BLK_KEY0 with purpose 2
    I (249) efuse: Writing EFUSE_BLK_KEY1 with purpose 3
    I (259) flash_encrypt: Disable UART bootloader cache...
    I (264) flash_encrypt: Disable JTAG...
    I (269) efuse: BURN BLOCK5
    I (272) efuse: BURN BLOCK5 - OK (write block == read block)
    I (277) efuse: BURN BLOCK4
    I (279) efuse: BURN BLOCK4 - OK (write block == read block)
    I (283) efuse: BURN BLOCK0
    I (287) efuse: BURN BLOCK0 - OK (write block == read block)
    I (291) efuse: BURN BLOCK0
    I (294) efuse: BURN BLOCK0 - OK (all write block bits are set)
    I (298) efuse: Batch mode. Prepared fields are committed
    I (304) esp_image: segment 0: paddr=00002020 vaddr=2f074230 size=02944h ( 10564)
    I (314) esp_image: segment 1: paddr=0000496c vaddr=2f06a2b0 size=00a20h (  2592)
    I (320) esp_image: segment 2: paddr=00005394 vaddr=2f06cfb0 size=04690h ( 18064)
    I (1187) flash_encrypt: bootloader encrypted successfully
    I (1292) flash_encrypt: partition table encrypted and loaded successfully
    I (1293) flash_encrypt: Encrypting partition 1 at offset 0x14000 (length 0x1000)...
    I (1398) flash_encrypt: Done encrypting
    I (1400) esp_image: segment 0: paddr=00020020 vaddr=40030020 size=09cb0h ( 40112) map
    I (1415) esp_image: segment 1: paddr=00029cd8 vaddr=2f000000 size=06340h ( 25408)
    I (1426) esp_image: segment 2: paddr=00030020 vaddr=40000020 size=21c2ch (138284) map
    I (1470) esp_image: segment 3: paddr=00051c54 vaddr=2f006340 size=03814h ( 14356)
    I (1477) esp_image: segment 4: paddr=00055470 vaddr=2f009b80 size=02508h (  9480)
    I (1484) flash_encrypt: Encrypting partition 2 at offset 0x20000 (length 0x379a0)...
    I (7433) flash_encrypt: Done encrypting
    I (7433) flash_encrypt: Encrypting partition 3 at offset 0x120000 (length 0x1000)...
    I (7529) flash_encrypt: Done encrypting
    I (7529) flash_encrypt: Encrypting partition 5 at offset 0x127000 (length 0x96000)...
    I (22094) flash_encrypt: Done encrypting
    I (22095) efuse: BURN BLOCK0
    I (22096) efuse: BURN BLOCK0 - OK (all write block bits are set)
    I (22098) flash_encrypt: Flash encryption completed
    I (22099) boot: Resetting with flash encryption enabled...


------


.. first_boot_enc_km

.. code-block:: none

    ESP-ROM:esp32s31-20251218
    Build:Jan 21 2026
    rst:0x1 (POWERON),boot:0x58 (SPI_FAST_FLASH_BOOT)
    SPI mode:DIO, clock div:1
    load:0x2f074230,len:0x2964
    load:0x2f06a2b0,len:0xa20
    load:0x2f06cfb0,len:0x4b24
    entry 0x2f06a304
    I (33) boot: ESP-IDF v6.1-dev-5047-ga20ab68fd55-dirt 2nd stage bootloader
    I (34) boot: compile time May 22 2026 17:33:18
    I (35) boot: Multicore bootloader
    I (38) boot: chip revision: v0.0
    I (39) boot: efuse block revision: v0.0
    I (42) boot.esp32s31: SPI Speed      : 80MHz
    I (46) boot.esp32s31: SPI Mode       : DIO
    I (50) boot.esp32s31: SPI Flash Size : 4MB
    I (54) boot: Enabling RNG early entropy source...
    I (59) boot: Partition Table:
    I (61) boot: ## Label            Usage          Type ST Offset   Length
    I (67) boot:  0 nvs              WiFi data        01 02 0000e000 00006000
    I (74) boot:  1 storage          Unknown data     01 ff 00014000 00001000
    I (80) boot:  2 factory          factory app      00 00 00020000 00100000
    I (87) boot:  3 nvs_key          NVS keys         01 04 00120000 00001000
    I (93) boot:  4 custom_nvs       WiFi data        01 02 00121000 00006000
    I (100) boot:  5 fat_encrypted    Unknown data     01 81 00127000 00096000
    I (107) boot:  6 fat_not_encr     Unknown data     01 81 001bd000 00096000
    I (115) boot: End of partition table
    I (117) esp_image: segment 0: paddr=00020020 vaddr=40030020 size=09cb0h ( 40112) map
    I (145) esp_image: segment 1: paddr=00029cd8 vaddr=2f000000 size=06340h ( 25408) load
    I (157) esp_image: segment 2: paddr=00030020 vaddr=40000020 size=21c2ch (138284) map
    I (201) esp_image: segment 3: paddr=00051c54 vaddr=2f006340 size=03814h ( 14356) load
    I (209) esp_image: segment 4: paddr=00055470 vaddr=2f009b80 size=02508h (  9480) load
    I (221) boot: Loaded app from partition at offset 0x20000
    I (222) boot: Checking flash encryption...
    I (223) efuse: Batch mode of writing fields is enabled
    I (224) flash_encrypt: Deploying new flash encryption key using Key Manager
    I (383) flash_encrypt: Disable UART bootloader cache...
    I (384) flash_encrypt: Disable JTAG...
    I (385) efuse: BURN BLOCK0
    I (389) efuse: BURN BLOCK0 - OK (write block == read block)
    I (393) efuse: Batch mode. Prepared fields are committed
    I (398) esp_image: segment 0: paddr=00002020 vaddr=2f074230 size=02964h ( 10596)
    I (409) esp_image: segment 1: paddr=0000498c vaddr=2f06a2b0 size=00a20h (  2592)
    I (415) esp_image: segment 2: paddr=000053b4 vaddr=2f06cfb0 size=04b24h ( 19236)
    I (1267) flash_encrypt: bootloader encrypted successfully
    I (1368) flash_encrypt: partition table encrypted and loaded successfully
    I (1369) flash_encrypt: Encrypting partition 1 at offset 0x14000 (length 0x1000)...
    I (1472) flash_encrypt: Done encrypting
    I (1473) esp_image: segment 0: paddr=00020020 vaddr=40030020 size=09cb0h ( 40112) map
    I (1489) esp_image: segment 1: paddr=00029cd8 vaddr=2f000000 size=06340h ( 25408)
    I (1499) esp_image: segment 2: paddr=00030020 vaddr=40000020 size=21c2ch (138284) map
    I (1543) esp_image: segment 3: paddr=00051c54 vaddr=2f006340 size=03814h ( 14356)
    I (1551) esp_image: segment 4: paddr=00055470 vaddr=2f009b80 size=02508h (  9480)
    I (1558) flash_encrypt: Encrypting partition 2 at offset 0x20000 (length 0x379a0)...
    I (7451) flash_encrypt: Done encrypting
    I (7451) flash_encrypt: Encrypting partition 3 at offset 0x120000 (length 0x1000)...
    I (7548) flash_encrypt: Done encrypting
    I (7549) flash_encrypt: Encrypting partition 5 at offset 0x127000 (length 0x96000)...
    I (22115) flash_encrypt: Done encrypting
    I (22116) efuse: BURN BLOCK0
    I (22118) efuse: BURN BLOCK0 - OK (all write block bits are set)
    I (22119) flash_encrypt: Flash encryption completed
    I (22121) boot: Resetting with flash encryption enabled...


------


.. already_en_enc

.. code-block:: none

    ESP-ROM:esp32s31-20251218
    Build:Jan 21 2026
    rst:0x1 (POWERON),boot:0x58 (SPI_FAST_FLASH_BOOT)
    use legacy efuse key
    SPI mode:DIO, clock div:1
    load:0x2f074230,len:0x2944
    load:0x2f06a2b0,len:0xa20
    load:0x2f06cfb0,len:0x4690
    entry 0x2f06a304
    I (35) boot: ESP-IDF v6.1-dev-5047-ga20ab68fd55-dirt 2nd stage bootloader
    I (36) boot: compile time May 22 2026 17:30:12
    I (37) boot: Multicore bootloader
    I (40) boot: chip revision: v0.0
    I (41) boot: efuse block revision: v0.0
    I (44) boot.esp32s31: SPI Speed      : 80MHz
    I (48) boot.esp32s31: SPI Mode       : DIO
    I (52) boot.esp32s31: SPI Flash Size : 4MB
    I (56) boot: Enabling RNG early entropy source...
    I (61) boot: Partition Table:
    I (63) boot: ## Label            Usage          Type ST Offset   Length
    I (69) boot:  0 nvs              WiFi data        01 02 0000e000 00006000
    I (76) boot:  1 storage          Unknown data     01 ff 00014000 00001000
    I (82) boot:  2 factory          factory app      00 00 00020000 00100000
    I (89) boot:  3 nvs_key          NVS keys         01 04 00120000 00001000
    I (95) boot:  4 custom_nvs       WiFi data        01 02 00121000 00006000
    I (102) boot:  5 fat_encrypted    Unknown data     01 81 00127000 00096000
    I (108) boot:  6 fat_not_encr     Unknown data     01 81 001bd000 00096000
    I (117) boot: End of partition table
    I (119) esp_image: segment 0: paddr=00020020 vaddr=40030020 size=09cb0h ( 40112) map
    I (149) esp_image: segment 1: paddr=00029cd8 vaddr=2f000000 size=06340h ( 25408) load
    I (162) esp_image: segment 2: paddr=00030020 vaddr=40000020 size=21c2ch (138284) map
    I (212) esp_image: segment 3: paddr=00051c54 vaddr=2f006340 size=03814h ( 14356) load
    I (221) esp_image: segment 4: paddr=00055470 vaddr=2f009b80 size=02508h (  9480) load
    I (233) boot: Loaded app from partition at offset 0x20000
    I (234) boot: Checking flash encryption...
    I (234) flash_encrypt: flash encryption is enabled (1 plaintext flashes left)
    I (237) boot: Disabling RNG early entropy source...
    I (254) cpu_start: Multicore app
    I (257) cpu_start: Pro cpu start user code
    I (258) cpu_start: cpu freq: 40000000 Hz
    I (262) app_init: Application information:
    I (266) app_init: Project name:     flash_encryption
    I (270) app_init: App version:      v6.1-dev-5047-ga20ab68fd55-dirt
    I (276) app_init: Compile time:     May 22 2026 17:30:25
    I (281) app_init: ELF file SHA256:  cebb4e540...
    I (286) app_init: ESP-IDF:          v6.1-dev-5047-ga20ab68fd55-dirt
    I (292) efuse_init: Min chip rev:     v0.0
    I (295) efuse_init: Max chip rev:     v0.99
    I (299) efuse_init: Chip rev:         v0.0
    I (303) heap_init: Initializing. RAM available for dynamic allocation:
    I (310) heap_init: At 2F00DA20 len 0006D590 (437 KiB): RAM
    I (315) heap_init: At 2F07AFB0 len 000041C0 (16 KiB): RAM
    I (320) heap_init: At 2E000000 len 00007FE8 (31 KiB): RTCRAM
    I (327) spi_flash: detected chip: gd
    I (329) spi_flash: flash io: dio
    W (332) flash_encrypt: Flash encryption mode is DEVELOPMENT (not secure)
    I (339) sleep_gpio: Configure to isolate all GPIO pins in sleep state
    I (345) sleep_gpio: Enable automatic switching of GPIO sleep configuration
    I (353) main_task: Started on CPU0
    I (373) main_task: Calling app_main()

    Example to check Flash Encryption status
    This is esp32s31 chip with 2 CPU core(s), WiFi/BLE, silicon revision v0.0, 4MB external flash
    FLASH_CRYPT_CNT eFuse value is 1
    Flash encryption feature is enabled in DEVELOPMENT mode


------


.. already_en_enc_km

.. code-block:: none

    ESP-ROM:esp32s31-20251218
    Build:Jan 21 2026
    rst:0x1 (POWERON),boot:0x58 (SPI_FAST_FLASH_BOOT)
    use sector0 for km info
    use KM derived key
    SPI mode:DIO, clock div:1
    load:0x2f074230,len:0x2964
    load:0x2f06a2b0,len:0xa20
    load:0x2f06cfb0,len:0x4b24
    entry 0x2f06a304
    I (37) boot: ESP-IDF v6.1-dev-5047-ga20ab68fd55-dirt 2nd stage bootloader
    I (38) boot: compile time May 22 2026 17:33:18
    I (39) boot: Multicore bootloader
    I (42) boot: chip revision: v0.0
    I (43) boot: efuse block revision: v0.0
    I (46) boot.esp32s31: SPI Speed      : 80MHz
    I (50) boot.esp32s31: SPI Mode       : DIO
    I (54) boot.esp32s31: SPI Flash Size : 4MB
    I (58) boot: Enabling RNG early entropy source...
    I (62) boot: Partition Table:
    I (65) boot: ## Label            Usage          Type ST Offset   Length
    I (71) boot:  0 nvs              WiFi data        01 02 0000e000 00006000
    I (78) boot:  1 storage          Unknown data     01 ff 00014000 00001000
    I (84) boot:  2 factory          factory app      00 00 00020000 00100000
    I (91) boot:  3 nvs_key          NVS keys         01 04 00120000 00001000
    I (97) boot:  4 custom_nvs       WiFi data        01 02 00121000 00006000
    I (104) boot:  5 fat_encrypted    Unknown data     01 81 00127000 00096000
    I (110) boot:  6 fat_not_encr     Unknown data     01 81 001bd000 00096000
    I (119) boot: End of partition table
    I (121) esp_image: segment 0: paddr=00020020 vaddr=40030020 size=09cb0h ( 40112) map
    I (151) esp_image: segment 1: paddr=00029cd8 vaddr=2f000000 size=06340h ( 25408) load
    I (164) esp_image: segment 2: paddr=00030020 vaddr=40000020 size=21c2ch (138284) map
    I (214) esp_image: segment 3: paddr=00051c54 vaddr=2f006340 size=03814h ( 14356) load
    I (223) esp_image: segment 4: paddr=00055470 vaddr=2f009b80 size=02508h (  9480) load
    I (235) boot: Loaded app from partition at offset 0x20000
    I (236) boot: Checking flash encryption...
    I (236) flash_encrypt: flash encryption is enabled (1 plaintext flashes left)
    I (239) boot: Disabling RNG early entropy source...
    I (256) cpu_start: Multicore app
    I (259) cpu_start: Pro cpu start user code
    I (260) cpu_start: cpu freq: 40000000 Hz
    I (264) app_init: Application information:
    I (268) app_init: Project name:     flash_encryption
    I (272) app_init: App version:      v6.1-dev-5047-ga20ab68fd55-dirt
    I (278) app_init: Compile time:     May 22 2026 17:33:32
    I (283) app_init: ELF file SHA256:  3907456c7...
    I (288) app_init: ESP-IDF:          v6.1-dev-5047-ga20ab68fd55-dirt
    I (294) efuse_init: Min chip rev:     v0.0
    I (297) efuse_init: Max chip rev:     v0.99
    I (301) efuse_init: Chip rev:         v0.0
    I (305) heap_init: Initializing. RAM available for dynamic allocation:
    I (312) heap_init: At 2F00DA20 len 0006D590 (437 KiB): RAM
    I (317) heap_init: At 2F07AFB0 len 000041C0 (16 KiB): RAM
    I (322) heap_init: At 2E000000 len 00007FE8 (31 KiB): RTCRAM
    I (329) spi_flash: detected chip: gd
    I (331) spi_flash: flash io: dio
    W (334) flash_encrypt: Flash encryption mode is DEVELOPMENT (not secure)
    I (341) sleep_gpio: Configure to isolate all GPIO pins in sleep state
    I (347) sleep_gpio: Enable automatic switching of GPIO sleep configuration
    I (355) main_task: Started on CPU0
    I (375) main_task: Calling app_main()

    Example to check Flash Encryption status
    This is esp32s31 chip with 2 CPU core(s), WiFi/BLE, silicon revision v0.0, 4MB external flash
    FLASH_CRYPT_CNT eFuse value is 1
    Flash encryption feature is enabled in DEVELOPMENT mode


------
