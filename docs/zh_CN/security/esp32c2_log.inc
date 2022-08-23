
.. first_boot_enc

.. code-block:: none

    ESP-ROM:esp8684-api1-20211015
    Build:Oct 15 2021
    rst:0x1 (POWERON),boot:0xc (SPI_FAST_FLASH_BOOT)
    SPIWP:0xee
    mode:DIO, clock div:1
    load:0x3fcd6190,len:0x2a84
    load:0x403ae000,len:0x830
    load:0x403b0000,len:0x42a0
    entry 0x403ae000
    I (21) boot: ESP-IDF v5.0-dev-2717-g0d1e015-dirty 2nd stage bootloader
    I (21) boot: compile time 19:36:15
    I (21) boot: chip revision: 0
    I (24) boot.esp32c2: MMU Page Size  : 64K
    I (29) boot.esp32c2: SPI Speed      : 60MHz
    I (34) boot.esp32c2: SPI Mode       : DIO
    I (39) boot.esp32c2: SPI Flash Size : 2MB
    I (43) boot: Enabling RNG early entropy source...
    I (49) boot: Partition Table:
    I (52) boot: ## Label            Usage          Type ST Offset   Length
    I (60) boot:  0 nvs              WiFi data        01 02 00010000 00006000
    I (67) boot:  1 phy_init         RF data          01 01 00016000 00001000
    I (75) boot:  2 factory          factory app      00 00 00020000 00100000
    I (82) boot: End of partition table
    I (86) esp_image: segment 0: paddr=00020020 vaddr=3c010020 size=06858h ( 26712) map
    I (101) esp_image: segment 1: paddr=00026880 vaddr=3fca9a60 size=01430h (  5168) load
    I (104) esp_image: segment 2: paddr=00027cb8 vaddr=40380000 size=08360h ( 33632) load
    I (120) esp_image: segment 3: paddr=00030020 vaddr=42000020 size=0f67ch ( 63100) map
    I (134) esp_image: segment 4: paddr=0003f6a4 vaddr=40388360 size=01700h (  5888) load
    I (139) boot: Loaded app from partition at offset 0x20000
    I (139) boot: Checking flash encryption...
    I (142) efuse: Batch mode of writing fields is enabled
    I (148) flash_encrypt: Generating new flash encryption key...
    I (155) efuse: Writing EFUSE_BLK_KEY0 with purpose 1
    W (161) flash_encrypt: Not disabling UART bootloader encryption
    I (167) flash_encrypt: Disable UART bootloader cache...
    I (175) flash_encrypt: Disable JTAG...
    I (190) efuse: BURN BLOCK3
    I (195) efuse: BURN BLOCK3 - OK (write block == read block)
    I (204) efuse: BURN BLOCK0
    I (208) efuse: BURN BLOCK0 - OK (write block == read block)
    I (213) efuse: Batch mode. Prepared fields are committed
    I (219) esp_image: segment 0: paddr=00000020 vaddr=3fcd6190 size=02a84h ( 10884)
    I (229) esp_image: segment 1: paddr=00002aac vaddr=403ae000 size=00830h (  2096)
    I (236) esp_image: segment 2: paddr=000032e4 vaddr=403b0000 size=042a0h ( 17056)
    I (679) flash_encrypt: bootloader encrypted successfully
    I (731) flash_encrypt: partition table encrypted and loaded successfully
    I (731) esp_image: segment 0: paddr=00020020 vaddr=3c010020 size=06858h ( 26712) map
    I (741) esp_image: segment 1: paddr=00026880 vaddr=3fca9a60 size=01430h (  5168)
    I (745) esp_image: segment 2: paddr=00027cb8 vaddr=40380000 size=08360h ( 33632)
    I (759) esp_image: segment 3: paddr=00030020 vaddr=42000020 size=0f67ch ( 63100) map
    I (774) esp_image: segment 4: paddr=0003f6a4 vaddr=40388360 size=01700h (  5888)
    I (776) flash_encrypt: Encrypting partition 2 at offset 0x20000 (length 0x100000)...
    I (6429) flash_encrypt: Done encrypting
    I (6429) efuse: BURN BLOCK0
    I (6432) efuse: BURN BLOCK0 - OK (all write block bits are set)
    I (6438) flash_encrypt: Flash encryption completed
    I (6443) boot: Resetting with flash encryption enabled...

------

.. already_en_enc

.. code-block:: none

    ESP-ROM:esp8684-api1-20211015
    Build:Oct 15 2021
    rst:0x3 (RTC_SW_SYS_RST),boot:0xc (SPI_FAST_FLASH_BOOT)
    Saved PC:0x403b0f9e
    SPIWP:0xee
    mode:DIO, clock div:1
    load:0x3fcd6190,len:0x2a84
    load:0x403ae000,len:0x830
    load:0x403b0000,len:0x42a0
    entry 0x403ae000
    I (23) boot: ESP-IDF v5.0-dev-2717-g0d1e015-dirty 2nd stage bootloader
    I (23) boot: compile time 19:36:15
    I (23) boot: chip revision: 0
    I (27) boot.esp32c2: MMU Page Size  : 64K
    I (32) boot.esp32c2: SPI Speed      : 60MHz
    I (36) boot.esp32c2: SPI Mode       : DIO
    I (41) boot.esp32c2: SPI Flash Size : 2MB
    I (46) boot: Enabling RNG early entropy source...
    I (51) boot: Partition Table:
    I (55) boot: ## Label            Usage          Type ST Offset   Length
    I (62) boot:  0 nvs              WiFi data        01 02 00010000 00006000
    I (70) boot:  1 phy_init         RF data          01 01 00016000 00001000
    I (77) boot:  2 factory          factory app      00 00 00020000 00100000
    I (85) boot: End of partition table
    I (89) esp_image: segment 0: paddr=00020020 vaddr=3c010020 size=06858h ( 26712) map
    I (103) esp_image: segment 1: paddr=00026880 vaddr=3fca9a60 size=01430h (  5168) load
    I (107) esp_image: segment 2: paddr=00027cb8 vaddr=40380000 size=08360h ( 33632) load
    I (123) esp_image: segment 3: paddr=00030020 vaddr=42000020 size=0f67ch ( 63100) map
    I (138) esp_image: segment 4: paddr=0003f6a4 vaddr=40388360 size=01700h (  5888) load
    I (143) boot: Loaded app from partition at offset 0x20000
    I (143) boot: Checking flash encryption...
    I (146) flash_encrypt: flash encryption is enabled (1 plaintext flashes left)
    I (154) boot: Disabling RNG early entropy source...
    I (171) cpu_start: Pro cpu up.
    I (179) cpu_start: Pro cpu start user code
    I (179) cpu_start: cpu freq: 120000000 Hz
    I (179) cpu_start: Application information:
    I (182) cpu_start: Project name:     hello_world
    I (187) cpu_start: App version:      v5.0-dev-2717-g0d1e015-dirty
    I (194) cpu_start: Compile time:     May 20 2022 19:35:55
    I (200) cpu_start: ELF file SHA256:  04592ac3c9304cdc...
    I (206) cpu_start: ESP-IDF:          v5.0-dev-2717-g0d1e015-dirty
    I (213) heap_init: Initializing. RAM available for dynamic allocation:
    I (220) heap_init: At 3FCABCB0 len 0002C350 (176 KiB): D/IRAM
    I (226) heap_init: At 3FCD8000 len 0000742C (29 KiB): STACK/DRAM
    I (234) spi_flash: detected chip: generic
    I (238) spi_flash: flash io: dio
    W (242) flash_encrypt: Flash encryption mode is DEVELOPMENT (not secure)
    I (249) sleep: Configure to isolate all GPIO pins in sleep state
    I (256) sleep: Enable automatic switching of GPIO sleep configuration
    W (263) INT_WDT: ESP32-C2 only has one timer group
    I (268) cpu_start: Starting scheduler.
    Hello world!
    This is esp32c2 chip with 1 CPU core(s), WiFi/BLE, silicon revision 0, 2MB external flash
    Minimum free heap size: 195052 bytes

    FLASH_CRYPT_CNT eFuse value is 1
    Flash encryption feature is enabled in DEVELOPMENT mode

------
