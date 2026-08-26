
.. first_boot_enc

.. code-block:: none

rst:0x1 (POWERON),boot:0x1f (SPI_FAST_FLASH_BOOT)
SPI mode:DIO, clock div:2
load:0x40845ce0,len:0x2b7c
load:0x4083c570,len:0x740
load:0x4083ea70,len:0x3e08
entry 0x4083c5d4
I (40) boot: ESP-IDF v5.4-dev-908-g874388c628-dirty 2nd stage bootloader
I (41) boot: compile time Jun  7 2024 13:56:46
I (42) boot: chip revision: v0.0
I (45) boot.esp32c61: SPI Speed      : 40MHz
I (50) boot.esp32c61: SPI Mode       : DIO
I (55) boot.esp32c61: SPI Flash Size : 2MB
I (60) boot: Enabling RNG early entropy source...
I (72) boot: Partition Table:
I (76) boot: ## Label            Usage          Type ST Offset   Length
I (83) boot:  0 nvs              WiFi data        01 02 0000e000 00006000
I (90) boot:  1 storage          Unknown data     01 ff 00014000 00001000
I (98) boot:  2 factory          factory app      00 00 00020000 00100000
I (105) boot:  3 nvs_key          NVS keys         01 04 00120000 00001000
I (113) boot:  4 custom_nvs       WiFi data        01 02 00121000 00006000
I (121) boot: End of partition table
I (125) esp_image: segment 0: paddr=00020020 vaddr=42018020 size=0a39ch ( 41884) map
I (155) esp_image: segment 1: paddr=0002a3c4 vaddr=40800000 size=05c54h ( 23636) load
I (164) esp_image: segment 2: paddr=00030020 vaddr=42000020 size=167b0h ( 92080) map
I (194) esp_image: segment 3: paddr=000467d8 vaddr=40805c54 size=01ea8h (  7848) load
I (198) esp_image: segment 4: paddr=00048688 vaddr=40807b00 size=00e84h (  3716) load
I (205) boot: Loaded app from partition at offset 0x20000
I (206) boot: Checking flash encryption...
I (211) efuse: Batch mode of writing fields is enabled
I (217) flash_encrypt: Generating new flash encryption key...
I (233) efuse: Writing EFUSE_BLK_KEY0 with purpose 4
W (238) flash_encrypt: Not disabling UART bootloader encryption
I (244) flash_encrypt: Disable UART bootloader cache...
I (249) flash_encrypt: Disable JTAG...
I (256) efuse: BURN BLOCK4
I (261) efuse: BURN BLOCK4 - OK (write block == read block)
I (264) efuse: BURN BLOCK0
I (269) efuse: BURN BLOCK0 - OK (all write block bits are set)
I (274) efuse: Batch mode. Prepared fields are committed
I (279) esp_image: segment 0: paddr=00000020 vaddr=40845ce0 size=02b7ch ( 11132)
I (290) esp_image: segment 1: paddr=00002ba4 vaddr=4083c570 size=00740h (  1856)
I (296) esp_image: segment 2: paddr=000032ec vaddr=4083ea70 size=03e08h ( 15880)
I (945) flash_encrypt: bootloader encrypted successfully
I (1021) flash_encrypt: partition table encrypted and loaded successfully
I (1022) flash_encrypt: Encrypting partition 1 at offset 0x14000 (length 0x1000)...
I (1099) flash_encrypt: Done encrypting
I (1100) esp_image: segment 0: paddr=00020020 vaddr=42018020 size=0a39ch ( 41884) map
I (1115) esp_image: segment 1: paddr=0002a3c4 vaddr=40800000 size=05c54h ( 23636)
I (1123) esp_image: segment 2: paddr=00030020 vaddr=42000020 size=167b0h ( 92080) map
I (1153) esp_image: segment 3: paddr=000467d8 vaddr=40805c54 size=01ea8h (  7848)
I (1157) esp_image: segment 4: paddr=00048688 vaddr=40807b00 size=00e84h (  3716)
I (1160) flash_encrypt: Encrypting partition 2 at offset 0x20000 (length 0x100000)...
I (20460) flash_encrypt: Done encrypting
I (20460) flash_encrypt: Encrypting partition 3 at offset 0x120000 (length 0x1000)...
I (20535) flash_encrypt: Done encrypting
I (20536) efuse: BURN BLOCK0
I (20538) efuse: BURN BLOCK0 - OK (all write block bits are set)
I (20540) flash_encrypt: Flash encryption completed
I (20544) boot: Resetting with flash encryption enabled...


------

.. already_en_enc

.. code-block:: none

rst:0x3 (RTC_SW_HPSYS),boot:0x1f (SPI_FAST_FLASH_BOOT)
Core0 Saved PC:0x4083faea
SPI mode:DIO, clock div:2
load:0x40845ce0,len:0x2b7c
load:0x4083c570,len:0x740
load:0x4083ea70,len:0x3e08
entry 0x4083c5d4
I (45) boot: ESP-IDF v5.4-dev-908-g874388c628-dirty 2nd stage bootloader
I (46) boot: compile time Jun  7 2024 13:56:46
I (47) boot: chip revision: v0.0
I (50) boot.esp32c61: SPI Speed      : 40MHz
I (55) boot.esp32c61: SPI Mode       : DIO
I (60) boot.esp32c61: SPI Flash Size : 2MB
I (65) boot: Enabling RNG early entropy source...
I (77) boot: Partition Table:
I (81) boot: ## Label            Usage          Type ST Offset   Length
I (88) boot:  0 nvs              WiFi data        01 02 0000e000 00006000
I (95) boot:  1 storage          Unknown data     01 ff 00014000 00001000
I (103) boot:  2 factory          factory app      00 00 00020000 00100000
I (110) boot:  3 nvs_key          NVS keys         01 04 00120000 00001000
I (118) boot:  4 custom_nvs       WiFi data        01 02 00121000 00006000
I (126) boot: End of partition table
I (130) esp_image: segment 0: paddr=00020020 vaddr=42018020 size=0a39ch ( 41884) map
I (162) esp_image: segment 1: paddr=0002a3c4 vaddr=40800000 size=05c54h ( 23636) load
I (172) esp_image: segment 2: paddr=00030020 vaddr=42000020 size=167b0h ( 92080) map
I (207) esp_image: segment 3: paddr=000467d8 vaddr=40805c54 size=01ea8h (  7848) load
I (211) esp_image: segment 4: paddr=00048688 vaddr=40807b00 size=00e84h (  3716) load
I (217) boot: Loaded app from partition at offset 0x20000
I (218) boot: Checking flash encryption...
I (223) flash_encrypt: flash encryption is enabled (1 plaintext flashes left)
I (231) boot: Disabling RNG early entropy source...
I (255) cpu_start: Unicore app
I (263) cpu_start: Pro cpu start user code
I (268) cpu_start: cpu freq: 40000000 Hz
I (272) app_init: Application information:
I (277) app_init: Project name:     flash_encryption
I (283) app_init: App version:      v5.4-dev-908-g874388c628-dirty
I (290) app_init: Compile time:     Jun  7 2024 13:56:41
I (296) app_init: ELF file SHA256:  3e962f7e5...
I (301) app_init: ESP-IDF:          v5.4-dev-908-g874388c628-dirty
I (308) efuse_init: Min chip rev:     v0.0
I (313) efuse_init: Max chip rev:     v0.99
I (317) efuse_init: Chip rev:         v0.0
I (322) heap_init: Initializing. RAM available for dynamic allocation:
I (330) heap_init: At 40809890 len 00041160 (260 KiB): RAM
I (336) heap_init: At 4084A9F0 len 00004BE0 (18 KiB): RAM
I (355) spi_flash: detected chip: generic
I (358) spi_flash: flash io: dio
W (362) spi_flash: Detected size(4096k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
W (376) flash_encrypt: Flash encryption mode is DEVELOPMENT (not secure)
I (383) nvs_sec_provider: NVS Encryption - Registering Flash encryption-based scheme...
I (393) main_task: Started on CPU0
I (393) main_task: Calling app_main()

Example to check Flash Encryption status
This is esp32c61 chip with 1 CPU core(s), WiFi/BLE, silicon revision v0.0, 2MB external flash
FLASH_CRYPT_CNT eFuse value is 1
Flash encryption feature is enabled in DEVELOPMENT mode

------
