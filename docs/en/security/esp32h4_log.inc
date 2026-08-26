
.. first_boot_enc

.. code-block:: none

    rst:0x1 (POWERON),boot:0x3f (SPI_FAST_FLASH_BOOT)
    SPI mode:DIO, clock div:2
    load:0x408565d0,len:0x2878
    load:0x4084d150,len:0x820
    load:0x4084f350,len:0x4088
    entry 0x4084d1a4
    I (138) boot: ESP-IDF v6.1-dev-2651-g37b56060853 2nd stage bootloader
    I (140) boot: compile time Feb 13 2026 14:20:36
    I (141) boot: Multicore bootloader
    I (149) boot: chip revision: v0.0
    I (154) boot: efuse block revision: v0.0
    I (166) boot.esp32h4: SPI Speed      : 24MHz
    I (177) boot.esp32h4: SPI Mode       : DIO
    I (189) boot.esp32h4: SPI Flash Size : 4MB
    I (201) boot: Enabling RNG early entropy source...
    I (237) boot: Partition Table:
    I (245) boot: ## Label            Usage          Type ST Offset   Length
    I (265) boot:  0 nvs              WiFi data        01 02 0000e000 00006000
    I (284) boot:  1 storage          Unknown data     01 ff 00014000 00001000
    I (304) boot:  2 factory          factory app      00 00 00020000 00100000
    I (324) boot:  3 nvs_key          NVS keys         01 04 00120000 00001000
    I (344) boot:  4 custom_nvs       WiFi data        01 02 00121000 00006000
    I (364) boot:  5 fat_encrypted    Unknown data     01 81 00127000 00096000
    I (383) boot:  6 fat_not_encr     Unknown data     01 81 001bd000 00096000
    I (405) boot: End of partition table
    I (414) esp_image: segment 0: paddr=00020020 vaddr=42030020 size=09b18h ( 39704) map
    I (487) esp_image: segment 1: paddr=00029b40 vaddr=40810000 size=064d8h ( 25816) load
    I (511) esp_image: segment 2: paddr=00030020 vaddr=42000020 size=21ab8h (137912) map
    I (611) esp_image: segment 3: paddr=00051ae0 vaddr=408164d8 size=02154h (  8532) load
    I (622) esp_image: segment 4: paddr=00053c3c vaddr=40818630 size=01ebch (  7868) load
    I (663) boot: Loaded app from partition at offset 0x20000
    I (674) boot: Checking flash encryption...
    I (686) efuse: Batch mode of writing fields is enabled
    I (700) flash_encrypt: Generating new flash encryption key...
    E (716) fpga_rng: Project configuration is for internal FPGA use, RNG will not work
    I (739) efuse: Writing EFUSE_BLK_KEY0 with purpose 4
    W (753) flash_encrypt: Not disabling UART bootloader encryption
    I (769) flash_encrypt: Disable UART bootloader cache...
    I (784) flash_encrypt: Disable JTAG...
    I (797) efuse: BURN BLOCK4
    I (815) efuse: BURN BLOCK4 - OK (write block == read block)
    I (818) efuse: BURN BLOCK0
    I (834) efuse: BURN BLOCK0 - OK (write block == read block)
    I (842) efuse: Batch mode. Prepared fields are committed
    I (857) esp_image: segment 0: paddr=00002020 vaddr=408565d0 size=02878h ( 10360)
    I (885) esp_image: segment 1: paddr=000048a0 vaddr=4084d150 size=00820h (  2080)
    I (902) esp_image: segment 2: paddr=000050c8 vaddr=4084f350 size=04088h ( 16520)
    I (2796) flash_encrypt: bootloader encrypted successfully
    I (3015) flash_encrypt: partition table encrypted and loaded successfully
    I (3016) flash_encrypt: Encrypting partition 1 at offset 0x14000 (length 0x1000)...
    I (3242) flash_encrypt: Done encrypting
    I (3244) esp_image: segment 0: paddr=00020020 vaddr=42030020 size=09b18h ( 39704) map
    I (3276) esp_image: segment 1: paddr=00029b40 vaddr=40810000 size=064d8h ( 25816)
    I (3298) esp_image: segment 2: paddr=00030020 vaddr=42000020 size=21ab8h (137912) map
    I (3398) esp_image: segment 3: paddr=00051ae0 vaddr=408164d8 size=02154h (  8532)
    I (3408) esp_image: segment 4: paddr=00053c3c vaddr=40818630 size=01ebch (  7868)
    I (3445) flash_encrypt: Encrypting partition 2 at offset 0x20000 (length 0x35b20)...
    I (16257) flash_encrypt: Done encrypting
    I (16258) flash_encrypt: Encrypting partition 3 at offset 0x120000 (length 0x1000)...
    I (16474) flash_encrypt: Done encrypting
    I (16475) flash_encrypt: Encrypting partition 5 at offset 0x127000 (length 0x96000)...
    I (46265) flash_encrypt: Done encrypting
    I (46267) efuse: BURN BLOCK0
    I (46275) efuse: BURN BLOCK0 - OK (all write block bits are set)
    I (46279) flash_encrypt: Flash encryption completed
    I (46281) boot: Resetting with flash encryption enabled...

------

.. already_en_enc

.. code-block:: none

    rst:0x3 (RTC_SW_HPSYS),boot:0x3f (SPI_FAST_FLASH_BOOT)
    use legacy efuse key
    SPI mode:DIO, clock div:2
    load:0x408565d0,len:0x2878
    load:0x4084d150,len:0x820
    load:0x4084f350,len:0x4088
    entry 0x4084d1a4
    I (157) boot: ESP-IDF v6.1-dev-2651-g37b56060853 2nd stage bootloader
    I (159) boot: compile time Feb 13 2026 14:20:36
    I (160) boot: Multicore bootloader
    I (168) boot: chip revision: v0.0
    I (173) boot: efuse block revision: v0.0
    I (184) boot.esp32h4: SPI Speed      : 24MHz
    I (196) boot.esp32h4: SPI Mode       : DIO
    I (208) boot.esp32h4: SPI Flash Size : 4MB
    I (219) boot: Enabling RNG early entropy source...
    I (256) boot: Partition Table:
    I (264) boot: ## Label            Usage          Type ST Offset   Length
    I (284) boot:  0 nvs              WiFi data        01 02 0000e000 00006000
    I (303) boot:  1 storage          Unknown data     01 ff 00014000 00001000
    I (323) boot:  2 factory          factory app      00 00 00020000 00100000
    I (343) boot:  3 nvs_key          NVS keys         01 04 00120000 00001000
    I (363) boot:  4 custom_nvs       WiFi data        01 02 00121000 00006000
    I (382) boot:  5 fat_encrypted    Unknown data     01 81 00127000 00096000
    I (402) boot:  6 fat_not_encr     Unknown data     01 81 001bd000 00096000
    I (424) boot: End of partition table
    I (433) esp_image: segment 0: paddr=00020020 vaddr=42030020 size=09b18h ( 39704) map
    I (510) esp_image: segment 1: paddr=00029b40 vaddr=40810000 size=064d8h ( 25816) load
    I (535) esp_image: segment 2: paddr=00030020 vaddr=42000020 size=21ab8h (137912) map
    I (648) esp_image: segment 3: paddr=00051ae0 vaddr=408164d8 size=02154h (  8532) load
    I (659) esp_image: segment 4: paddr=00053c3c vaddr=40818630 size=01ebch (  7868) load
    I (700) boot: Loaded app from partition at offset 0x20000
    I (711) boot: Checking flash encryption...
    I (722) flash_encrypt: flash encryption is enabled (1 plaintext flashes left)
    I (743) boot: Disabling RNG early entropy source...
    I (817) cpu_start: Multicore app
    I (821) cpu_start: Pro cpu start user code
    I (829) cpu_start: cpu freq: 32000000 Hz
    I (840) app_init: Application information:
    I (851) app_init: Project name:     flash_encryption
    I (865) app_init: App version:      v6.1-dev-2651-g37b56060853
    I (882) app_init: Compile time:     Feb 13 2026 14:20:40
    I (897) app_init: ELF file SHA256:  1ed3e1bde...
    I (910) app_init: ESP-IDF:          v6.1-dev-2651-g37b56060853
    I (927) efuse_init: Min chip rev:     v0.0
    I (938) efuse_init: Max chip rev:     v0.99
    I (950) efuse_init: Chip rev:         v0.0
    I (962) heap_init: Initializing. RAM available for dynamic allocation:
    I (981) heap_init: At 4081BE20 len 0003F530 (253 KiB): RAM
    I (996) heap_init: At 4085B350 len 00004B58 (18 KiB): RAM
    I (1016) spi_flash: detected chip: gd
    I (1022) spi_flash: flash io: dio
    W (1033) flash_encrypt: Flash encryption mode is DEVELOPMENT (not secure)
    I (1054) sleep_gpio: Configure to isolate all GPIO pins in sleep state
    I (1071) sleep_gpio: Enable automatic switching of GPIO sleep configuration
    I (1094) sleep_clock: Modem Power, Clock and Reset sleep retention initialization
    I (1111) nvs_sec_provider: NVS Encryption - Registering Flash encryption-based scheme...
    I (1140) main_task: Started on CPU0
    I (1150) main_task: Calling app_main()

    Example to check Flash Encryption status
    This is esp32h4 chip with 2 CPU core(s), WiFi/BLE, silicon revision v0.0, 4MB external flash
    FLASH_CRYPT_CNT eFuse value is 1
    Flash encryption feature is enabled in DEVELOPMENT mode
    Erasing partition "storage" (0x1000 bytes)
    Writing data with esp_partition_write:
    I (1220) example: 0x4081e800   00 01 02 03 04 05 06 07  08 09 0a 0b 0c 0d 0e 0f  |................|
    I (1220) example: 0x4081e810   10 11 12 13 14 15 16 17  18 19 1a 1b 1c 1d 1e 1f  |................|
    Reading with esp_partition_read:
    I (1230) example: 0x4081e820   00 01 02 03 04 05 06 07  08 09 0a 0b 0c 0d 0e 0f  |................|
    I (1240) example: 0x4081e830   10 11 12 13 14 15 16 17  18 19 1a 1b 1c 1d 1e 1f  |................|
    Reading with esp_flash_read:
    I (1250) example: 0x4081e820   8a 20 78 9a cc bd 90 3a  96 0b 50 25 09 3b df c9  |. x....:..P%.;..|
    I (1260) example: 0x4081e830   d5 d7 0b fe af d2 cb 9a  3f e7 50 9f e7 b5 98 fc  |........?.P.....|
    I (4170) main_task: Returned from app_main()


------
