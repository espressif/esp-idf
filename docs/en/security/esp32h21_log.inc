
.. first_boot_enc

.. code-block:: none

    rst:0x1 (POWERON),boot:0x1f (SPI_FAST_FLASH_BOOT)
    SPIWP:0xee
    mode:DIO, clock div:2
    load:0x40842cf0,len:0x27a4
    load:0x40839580,len:0x944
    load:0x4083ba78,len:0x3f44
    entry 0x408395ee
    I (31) boot: ESP-IDF v5.5-dev-1478-gc15e6e1fb66-dirt 2nd stage bootloader
    I (32) boot: compile time Jan 16 2025 12:04:40
    I (34) boot: chip revision: v0.0
    I (34) boot: efuse block revision: v0.0
    I (37) boot.esp32h21: SPI Speed      : 32MHz
    I (41) boot.esp32h21: SPI Mode       : DIO
    I (45) boot.esp32h21: SPI Flash Size : 4MB
    I (49) boot: Enabling RNG early entropy source...
    I (61) boot: Partition Table:
    I (64) boot: ## Label            Usage          Type ST Offset   Length
    I (70) boot:  0 nvs              WiFi data        01 02 0000e000 00006000
    I (77) boot:  1 storage          Unknown data     01 ff 00014000 00001000
    I (83) boot:  2 factory          factory app      00 00 00020000 00100000
    I (90) boot:  3 nvs_key          NVS keys         01 04 00120000 00001000
    I (96) boot:  4 custom_nvs       WiFi data        01 02 00121000 00006000
    I (103) boot:  5 fat_encrypted    Unknown data     01 81 00127000 00096000
    I (109) boot:  6 fat_not_encr     Unknown data     01 81 001bd000 00096000
    I (116) boot: End of partition table
    I (119) esp_image: segment 0: paddr=00020020 vaddr=42030020 size=0a6e0h ( 42720) map
    I (153) esp_image: segment 1: paddr=0002a708 vaddr=40800000 size=05910h ( 22800) load
    I (165) esp_image: segment 2: paddr=00030020 vaddr=42000020 size=2525ch (152156) map
    I (232) esp_image: segment 3: paddr=00055284 vaddr=40805910 size=0282ch ( 10284) load
    I (239) esp_image: segment 4: paddr=00057ab8 vaddr=40808140 size=00eb8h (  3768) load
    I (248) boot: Loaded app from partition at offset 0x20000
    I (249) boot: Checking flash encryption...
    I (250) efuse: Batch mode of writing fields is enabled
    I (251) flash_encrypt: Using pre-loaded flash encryption key in efuse
    W (257) flash_encrypt: Not disabling UART bootloader encryption
    I (263) flash_encrypt: Disable JTAG...
    I (266) efuse: BURN BLOCK0
    I (271) efuse: BURN BLOCK0 - OK (all write block bits are set)
    I (274) efuse: Batch mode. Prepared fields are committed
    I (279) esp_image: segment 0: paddr=00000020 vaddr=40842cf0 size=027a4h ( 10148)
    I (289) esp_image: segment 1: paddr=000027cc vaddr=40839580 size=00944h (  2372)
    I (294) esp_image: segment 2: paddr=00003118 vaddr=4083ba78 size=03f44h ( 16196)
    I (924) flash_encrypt: bootloader encrypted successfully
    I (1003) flash_encrypt: partition table encrypted and loaded successfully
    I (1004) flash_encrypt: Encrypting partition 1 at offset 0x14000 (length 0x1000)...
    I (1082) flash_encrypt: Done encrypting
    I (1083) esp_image: segment 0: paddr=00020020 vaddr=42030020 size=0a6e0h ( 42720) map
    I (1103) esp_image: segment 1: paddr=0002a708 vaddr=40800000 size=05910h ( 22800)
    I (1115) esp_image: segment 2: paddr=00030020 vaddr=42000020 size=2525ch (152156) map
    I (1182) esp_image: segment 3: paddr=00055284 vaddr=40805910 size=0282ch ( 10284)
    I (1188) esp_image: segment 4: paddr=00057ab8 vaddr=40808140 size=00eb8h (  3768)
    I (1192) flash_encrypt: Encrypting partition 2 at offset 0x20000 (length 0x389a0)...
    I (5680) flash_encrypt: Done encrypting
    I (5681) flash_encrypt: Encrypting partition 3 at offset 0x120000 (length 0x1000)...
    I (5755) flash_encrypt: Done encrypting
    I (5756) flash_encrypt: Encrypting partition 5 at offset 0x127000 (length 0x96000)...
    I (14753) flash_encrypt: Done encrypting
    I (14754) efuse: BURN BLOCK0
    I (14757) efuse: BURN BLOCK0 - OK (all write block bits are set)
    I (14759) flash_encrypt: Flash encryption completed
    I (14760) boot: Resetting with flash encryption enabled...

------

.. already_en_enc

.. code-block:: none

    rst:0x3 (LP_SW_HPSYS),boot:0x1f (SPI_FAST_FLASH_BOOT)
    Saved PC:0x4000321c
    SPIWP:0xee
    mode:DIO, clock div:2
    load:0x40842cf0,len:0x27a4
    load:0x40839580,len:0x944
    load:0x4083ba78,len:0x3f44
    entry 0x408395ee
    I (36) boot: ESP-IDF v5.5-dev-1478-gc15e6e1fb66-dirt 2nd stage bootloader
    I (37) boot: compile time Jan 16 2025 12:04:40
    I (38) boot: chip revision: v0.0
    I (39) boot: efuse block revision: v0.0
    I (42) boot.esp32h21: SPI Speed      : 32MHz
    I (46) boot.esp32h21: SPI Mode       : DIO
    I (50) boot.esp32h21: SPI Flash Size : 4MB
    I (53) boot: Enabling RNG early entropy source...
    I (66) boot: Partition Table:
    I (68) boot: ## Label            Usage          Type ST Offset   Length
    I (74) boot:  0 nvs              WiFi data        01 02 0000e000 00006000
    I (81) boot:  1 storage          Unknown data     01 ff 00014000 00001000
    I (87) boot:  2 factory          factory app      00 00 00020000 00100000
    I (94) boot:  3 nvs_key          NVS keys         01 04 00120000 00001000
    I (101) boot:  4 custom_nvs       WiFi data        01 02 00121000 00006000
    I (107) boot:  5 fat_encrypted    Unknown data     01 81 00127000 00096000
    I (114) boot:  6 fat_not_encr     Unknown data     01 81 001bd000 00096000
    I (121) boot: End of partition table
    I (124) esp_image: segment 0: paddr=00020020 vaddr=42030020 size=0a6e0h ( 42720) map
    I (160) esp_image: segment 1: paddr=0002a708 vaddr=40800000 size=05910h ( 22800) load
    I (173) esp_image: segment 2: paddr=00030020 vaddr=42000020 size=2525ch (152156) map
    I (249) esp_image: segment 3: paddr=00055284 vaddr=40805910 size=0282ch ( 10284) load
    I (257) esp_image: segment 4: paddr=00057ab8 vaddr=40808140 size=00eb8h (  3768) load
    I (266) boot: Loaded app from partition at offset 0x20000
    I (267) boot: Checking flash encryption...
    I (268) flash_encrypt: flash encryption is enabled (1 plaintext flashes left)
    I (271) boot: Disabling RNG early entropy source...
    I (295) cpu_start: Unicore app
    I (300) cpu_start: Pro cpu start user code
    I (304) cpu_start: cpu freq: 32000000 Hz
    I (307) app_init: Application information:
    I (311) app_init: Project name:     flash_encryption
    I (316) app_init: App version:      v5.5-dev-1478-gc15e6e1fb66-dirt
    I (322) app_init: Compile time:     Jan 16 2025 12:04:30
    I (327) app_init: ELF file SHA256:  ba86ca637...
    I (331) app_init: ESP-IDF:          v5.5-dev-1478-gc15e6e1fb66-dirt
    I (337) efuse_init: Min chip rev:     v0.0
    I (341) efuse_init: Max chip rev:     v0.99
    I (345) efuse_init: Chip rev:         v0.0
    I (349) heap_init: Initializing. RAM available for dynamic allocation:
    I (355) heap_init: At 40809F70 len 00043410 (269 KiB): RAM
    I (360) heap_init: At 4084D380 len 00002B60 (10 KiB): RAM
    I (377) spi_flash: detected chip: generic
    I (379) spi_flash: flash io: dio
    W (382) flash_encrypt: Flash encryption mode is DEVELOPMENT (not secure)

------
