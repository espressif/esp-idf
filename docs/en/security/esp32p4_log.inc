
.. first_boot_enc

.. code-block:: none

    rst:0x1 (POWERON),boot:0x30f (SPI_FAST_FLASH_BOOT)
    SPI mode:DIO, clock div:1
    load:0x4ff2abd0,len:0xa9c
    load:0x4ff2cbd0,len:0x4898
    load:0x4ff33e48,len:0x2c08
    entry 0x4ff2abdc
    I (29) boot: ESP-IDF v5.3-dev-3079-g96ed5ffadf 2nd stage bootloader
    I (29) boot: compile time Apr  1 2024 16:35:58
    I (29) boot: Multicore bootloader
    I (35) boot: chip revision: v0.0
    I (37) boot.esp32p4: SPI Speed      : 80MHz
    I (42) boot.esp32p4: SPI Mode       : DIO
    I (47) boot.esp32p4: SPI Flash Size : 2MB
    I (52) boot: Enabling RNG early entropy source...
    I (65) boot: Partition Table:
    I (69) boot: ## Label            Usage          Type ST Offset   Length
    I (76) boot:  0 nvs              WiFi data        01 02 0000e000 00006000
    I (84) boot:  1 storage          Unknown data     01 ff 00014000 00001000
    I (91) boot:  2 factory          factory app      00 00 00020000 00100000
    I (98) boot:  3 nvs_key          NVS keys         01 04 00120000 00001000
    I (106) boot:  4 custom_nvs       WiFi data        01 02 00121000 00006000
    I (115) boot: End of partition table
    I (118) esp_image: segment 0: paddr=00020020 vaddr=40020020 size=0abc8h ( 43976) map
    E (126) fpga_rng: Project configuration is for internal FPGA use, RNG will not work
    I (145) esp_image: segment 1: paddr=0002abf0 vaddr=30100000 size=00044h (    68) load
    I (147) esp_image: segment 2: paddr=0002ac3c vaddr=4ff00000 size=053dch ( 21468) load
    I (158) esp_image: segment 3: paddr=00030020 vaddr=40000020 size=1a884h (108676) map
    I (185) esp_image: segment 4: paddr=0004a8ac vaddr=4ff053dc size=06dech ( 28140) load
    I (194) esp_image: segment 5: paddr=000516a0 vaddr=4ff0c1d0 size=013c0h (  5056) load
    I (199) boot: Loaded app from partition at offset 0x20000
    I (200) boot: Checking flash encryption...
    I (202) efuse: Batch mode of writing fields is enabled
    I (208) flash_encrypt: Generating new flash encryption key...
    E (214) fpga_rng: Project configuration is for internal FPGA use, RNG will not work
    I (224) efuse: Writing EFUSE_BLK_KEY0 with purpose 4
    W (228) flash_encrypt: Not disabling UART bootloader encryption
    I (235) flash_encrypt: Disable UART bootloader cache...
    I (241) flash_encrypt: Disable JTAG...
    I (246) efuse: BURN BLOCK4
    I (252) efuse: BURN BLOCK4 - OK (write block == read block)
    I (255) efuse: BURN BLOCK0
    I (260) efuse: BURN BLOCK0 - OK (write block == read block)
    I (264) efuse: Batch mode. Prepared fields are committed
    I (270) esp_image: segment 0: paddr=00002020 vaddr=4ff2abd0 size=00a9ch (  2716)
    I (280) esp_image: segment 1: paddr=00002ac4 vaddr=4ff2cbd0 size=04898h ( 18584)
    I (291) esp_image: segment 2: paddr=00007364 vaddr=4ff33e48 size=02c08h ( 11272)
    I (840) flash_encrypt: bootloader encrypted successfully
    I (904) flash_encrypt: partition table encrypted and loaded successfully
    I (904) flash_encrypt: Encrypting partition 1 at offset 0x14000 (length 0x1000)...
    I (982) flash_encrypt: Done encrypting
    I (982) esp_image: segment 0: paddr=00020020 vaddr=40020020 size=0abc8h ( 43976) map
    I (994) esp_image: segment 1: paddr=0002abf0 vaddr=30100000 size=00044h (    68)
    I (996) esp_image: segment 2: paddr=0002ac3c vaddr=4ff00000 size=053dch ( 21468)
    I (1006) esp_image: segment 3: paddr=00030020 vaddr=40000020 size=1a884h (108676) map
    I (1033) esp_image: segment 4: paddr=0004a8ac vaddr=4ff053dc size=06dech ( 28140)
    I (1041) esp_image: segment 5: paddr=000516a0 vaddr=4ff0c1d0 size=013c0h (  5056)
    I (1044) flash_encrypt: Encrypting partition 2 at offset 0x20000 (length 0x100000)...
    I (17573) flash_encrypt: Done encrypting
    I (17573) flash_encrypt: Encrypting partition 3 at offset 0x120000 (length 0x1000)...
    I (17645) flash_encrypt: Done encrypting
    I (17646) efuse: BURN BLOCK0
    I (17648) efuse: BURN BLOCK0 - OK (all write block bits are set)
    I (17649) flash_encrypt: Flash encryption completed
    I (17655) boot: Resetting with flash encryption enabled...

------

.. already_en_enc

.. code-block:: none

    ESP-ROM:esp32p4-20230811
    Build:Aug 11 2023
    rst:0x3 (SW_SYS_RESET),boot:0x30f (SPI_FAST_FLASH_BOOT)
    use legacy efuse key
    SPI mode:DIO, clock div:1
    load:0x4ff2abd0,len:0xa9c
    load:0x4ff2cbd0,len:0x4898
    load:0x4ff33e48,len:0x2c08
    entry 0x4ff2abdc
    I (27) boot: ESP-IDF v5.3-dev-3079-g96ed5ffadf 2nd stage bootloader
    I (27) boot: compile time Apr  1 2024 16:35:58
    I (28) boot: Multicore bootloader
    I (33) boot: chip revision: v0.0
    I (35) boot.esp32p4: SPI Speed      : 80MHz
    I (40) boot.esp32p4: SPI Mode       : DIO
    I (45) boot.esp32p4: SPI Flash Size : 2MB
    I (50) boot: Enabling RNG early entropy source...
    I (63) boot: Partition Table:
    I (67) boot: ## Label            Usage          Type ST Offset   Length
    I (74) boot:  0 nvs              WiFi data        01 02 0000e000 00006000
    I (82) boot:  1 storage          Unknown data     01 ff 00014000 00001000
    I (89) boot:  2 factory          factory app      00 00 00020000 00100000
    I (97) boot:  3 nvs_key          NVS keys         01 04 00120000 00001000
    I (104) boot:  4 custom_nvs       WiFi data        01 02 00121000 00006000
    I (113) boot: End of partition table
    I (116) esp_image: segment 0: paddr=00020020 vaddr=40020020 size=0abc8h ( 43976) map
    E (124) fpga_rng: Project configuration is for internal FPGA use, RNG will not work
    I (144) esp_image: segment 1: paddr=0002abf0 vaddr=30100000 size=00044h (    68) load
    I (146) esp_image: segment 2: paddr=0002ac3c vaddr=4ff00000 size=053dch ( 21468) load
    I (156) esp_image: segment 3: paddr=00030020 vaddr=40000020 size=1a884h (108676) map
    I (183) esp_image: segment 4: paddr=0004a8ac vaddr=4ff053dc size=06dech ( 28140) load
    I (192) esp_image: segment 5: paddr=000516a0 vaddr=4ff0c1d0 size=013c0h (  5056) load
    I (198) boot: Loaded app from partition at offset 0x20000
    I (198) boot: Checking flash encryption...
    I (200) flash_encrypt: flash encryption is enabled (1 plaintext flashes left)
    I (208) boot: Disabling RNG early entropy source...
    I (233) cpu_start: Multicore app
    I (249) cpu_start: Pro cpu start user code
    I (249) cpu_start: cpu freq: 360000000 Hz
    I (250) app_init: Application information:
    I (252) app_init: Project name:     flash_encryption
    I (258) app_init: App version:      qa-test-test-dynamic-pipeline-f
    I (265) app_init: Compile time:     Apr  1 2024 16:35:55
    I (271) app_init: ELF file SHA256:  c71f148d8...
    I (276) app_init: ESP-IDF:          v5.3-dev-3079-g96ed5ffadf
    I (283) efuse_init: Min chip rev:     v0.0
    I (287) efuse_init: Max chip rev:     v0.99
    I (292) efuse_init: Chip rev:         v0.0
    I (297) heap_init: Initializing. RAM available for dynamic allocation:
    I (304) heap_init: At 4FF0ECD0 len 0002C2F0 (176 KiB): RAM
    I (310) heap_init: At 4FF3AFC0 len 00004BE4 (18 KiB): RAM
    I (316) heap_init: At 4FF40000 len 00060000 (384 KiB): RAM
    I (323) heap_init: At 50108000 len 00007FE8 (31 KiB): RTCRAM
    I (329) heap_init: At 30100044 len 00001FBC (7 KiB): TCM
    I (336) spi_flash: detected chip: generic
    I (340) spi_flash: flash io: dio
    W (344) spi_flash: Detected size(4096k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
    W (357) flash_encrypt: Flash encryption mode is DEVELOPMENT (not secure)
    I (365) main_task: Started on CPU0
    I (375) main_task: Calling app_main()

    Example to check Flash Encryption status
    This is esp32p4 chip with 2 CPU core(s), WiFi, silicon revision v0.0, 2MB external flash
    FLASH_CRYPT_CNT eFuse value is 1
    Flash encryption feature is enabled in DEVELOPMENT mode
    Erasing partition "storage" (0x1000 bytes)
    Writing data with esp_partition_write:
    I (455) example: 0x4ff112a0   00 01 02 03 04 05 06 07  08 09 0a 0b 0c 0d 0e 0f  |................|
    I (455) example: 0x4ff112b0   10 11 12 13 14 15 16 17  18 19 1a 1b 1c 1d 1e 1f  |................|
    Reading with esp_partition_read:
    I (465) example: 0x4ff112c0   00 01 02 03 04 05 06 07  08 09 0a 0b 0c 0d 0e 0f  |................|
    I (475) example: 0x4ff112d0   10 11 12 13 14 15 16 17  18 19 1a 1b 1c 1d 1e 1f  |................|
    Reading with esp_flash_read:
    I (485) example: 0x4ff112c0   8a 20 78 9a cc bd 90 3a  96 0b 50 25 09 3b df c9  |. x....:..P%.;..|
    I (495) example: 0x4ff112d0   d5 d7 0b fe af d2 cb 9a  3f e7 50 9f e7 b5 98 fc  |........?.P.....|
    I (595) main_task: Returned from app_main()

------
