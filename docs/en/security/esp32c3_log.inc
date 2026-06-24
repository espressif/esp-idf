
.. first_boot_enc

.. code-block:: none

    rst:0x1 (POWERON),boot:0xf (SPI_FAST_FLASH_BOOT)
    SPIWP:0xee
    mode:DIO, clock div:2
    load:0x3fcd6260,len:0x1b8
    load:0x3fcd6418,len:0x2538
    load:0x403ce000,len:0x704
    load:0x403d0000,len:0x34f0
    entry 0x403ce03e
    I (12) boot: ESP-IDF qa-test-v4.3-20201113-766-g870d 2nd stage bootloader
    I (13) boot: compile time 12:10:57
    I (13) boot: chip revision: 0
    I (16) boot.esp32c3: SPI Speed      : 40MHz
    I (17) boot.esp32c3: SPI Mode       : DIO
    I (19) boot.esp32c3: SPI Flash Size : 2MB
    I (22) boot: Enabling RNG early entropy source...
    I (28) boot: Partition Table:
    I (30) boot: ## Label            Usage          Type ST Offset   Length
    I (33) boot:  0 nvs              WiFi data        01 02 0000a000 00006000
    I (37) boot:  1 storage          Unknown data     01 ff 00010000 00001000
    I (41) boot:  2 factory          factory app      00 00 00020000 00100000
    I (44) boot: End of partition table
    I (46) esp_image: segment 0: paddr=0x00020020 vaddr=0x3c020020 size=0x05508 ( 21768) map
    I (61) esp_image: segment 1: paddr=0x00025530 vaddr=0x3fc88780 size=0x014cc (  5324) load
    I (64) esp_image: segment 2: paddr=0x00026a04 vaddr=0x40380000 size=0x08780 ( 34688) load
    0x40380000: _vector_table at ??:?

    I (81) esp_image: segment 3: paddr=0x0002f18c vaddr=0x00000000 size=0x00e8c (  3724)
    I (84) esp_image: segment 4: paddr=0x00030020 vaddr=0x42000020 size=0x171a8 ( 94632) map
    I (132) boot: Loaded app from partition at offset 0x20000
    I (133) boot: Checking flash encryption...
    I (137) efuse: Batch mode of writing fields is enabled
    I (140) flash_encrypt: Generating new flash encryption key...
    I (144) efuse: Writing EFUSE_BLK_KEY0 with purpose 4
    W (148) flash_encrypt: Not disabling UART bootloader encryption
    I (152) flash_encrypt: Disable UART bootloader cache...
    I (155) flash_encrypt: Disable JTAG...
    I (161) efuse: Batch mode. Prepared fields are committed
    I (162) esp_image: segment 0: paddr=0x00000020 vaddr=0x3fcd6260 size=0x001b8 (   440)
    I (164) esp_image: segment 1: paddr=0x000001e0 vaddr=0x3fcd6418 size=0x02538 (  9528)
    I (173) esp_image: segment 2: paddr=0x00002720 vaddr=0x403ce000 size=0x00704 (  1796)
    I (174) esp_image: segment 3: paddr=0x00002e2c vaddr=0x403d0000 size=0x034f0 ( 13552)
    I (571) flash_encrypt: bootloader encrypted successfully
    I (627) flash_encrypt: partition table encrypted and loaded successfully
    I (628) flash_encrypt: Encrypting partition 1 at offset 0x10000 (length 0x1000)...
    I (685) flash_encrypt: Done encrypting
    I (686) esp_image: segment 0: paddr=0x00020020 vaddr=0x3c020020 size=0x05508 ( 21768) map
    I (696) esp_image: segment 1: paddr=0x00025530 vaddr=0x3fc88780 size=0x014cc (  5324)
    I (699) esp_image: segment 2: paddr=0x00026a04 vaddr=0x40380000 size=0x08780 ( 34688)
    0x40380000: _vector_table at ??:?

    I (715) esp_image: segment 3: paddr=0x0002f18c vaddr=0x00000000 size=0x00e8c (  3724)
    I (717) esp_image: segment 4: paddr=0x00030020 vaddr=0x42000020 size=0x171a8 ( 94632) map
    I (760) flash_encrypt: Encrypting partition 2 at offset 0x20000 (length 0x100000)...
    I (14797) flash_encrypt: Done encrypting
    I (14801) flash_encrypt: Flash encryption completed
    I (14802) boot: Resetting with flash encryption enabled...


------

.. already_en_enc

.. code-block:: none

    rst:0x3 (RTC_SW_SYS_RST),boot:0xf (SPI_FAST_FLASH_BOOT)
    Saved PC:0x403d0dde
    SPIWP:0xee
    mode:DIO, clock div:2
    load:0x3fcd6260,len:0x1b8
    load:0x3fcd6418,len:0x2538
    load:0x403ce000,len:0x704
    load:0x403d0000,len:0x34f0
    entry 0x403ce03e
    I (15) boot: ESP-IDF qa-test-v4.3-20201113-766-g870d 2nd stage bootloader
    I (15) boot: compile time 12:10:57
    I (16) boot: chip revision: 0
    I (19) boot.esp32c3: SPI Speed      : 40MHz
    I (19) boot.esp32c3: SPI Mode       : DIO
    I (22) boot.esp32c3: SPI Flash Size : 2MB
    I (24) boot: Enabling RNG early entropy source...
    I (30) boot: Partition Table:
    I (32) boot: ## Label            Usage          Type ST Offset   Length
    I (36) boot:  0 nvs              WiFi data        01 02 0000a000 00006000
    I (39) boot:  1 storage          Unknown data     01 ff 00010000 00001000
    I (43) boot:  2 factory          factory app      00 00 00020000 00100000
    I (47) boot: End of partition table
    I (49) esp_image: segment 0: paddr=0x00020020 vaddr=0x3c020020 size=0x05508 ( 21768) map
    I (64) esp_image: segment 1: paddr=0x00025530 vaddr=0x3fc88780 size=0x014cc (  5324) load
    I (67) esp_image: segment 2: paddr=0x00026a04 vaddr=0x40380000 size=0x08780 ( 34688) load
    0x40380000: _vector_table at ??:?

    I (86) esp_image: segment 3: paddr=0x0002f18c vaddr=0x00000000 size=0x00e8c (  3724)
    I (88) esp_image: segment 4: paddr=0x00030020 vaddr=0x42000020 size=0x171a8 ( 94632) map
    I (139) boot: Loaded app from partition at offset 0x20000
    I (139) boot: Checking flash encryption...
    I (144) flash_encrypt: flash encryption is enabled (1 plaintext flashes left)
    I (148) boot: Disabling RNG early entropy source...
    I (160) cpu_start: Pro cpu start user code
    I (160) cpu_start: cpu freq: 40000000
    I (161) cpu_start: Application information:
    I (161) cpu_start: Project name:     flash_encryption
    I (164) cpu_start: App version:      qa-test-v4.3-20201113-766-g870d
    I (168) cpu_start: Compile time:     Dec 21 2020 12:10:55
    I (171) cpu_start: ELF file SHA256:  209e8947c2e6a6a6...
    I (174) cpu_start: ESP-IDF:          qa-test-v4.3-20201113-766-g870d
    I (178) heap_init: Initializing. RAM available for dynamic allocation:
    I (181) heap_init: At 3FC8A9F0 len 00035610 (213 KiB): D/IRAM
    I (184) heap_init: At 3FCC0000 len 0001F260 (124 KiB): STACK/DRAM
    I (188) heap_init: At 50000000 len 00002000 (8 KiB): FAKEDRAM
    W (192) flash_encrypt: Flash encryption mode is DEVELOPMENT (not secure)
    I (195) spi_flash: detected chip: gd
    I (197) spi_flash: flash io: dio
    W (199) spi_flash: Detected size(4096k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
    I (207) cpu_start: Starting scheduler.

    Example to check Flash Encryption status
    This is esp32c3 chip with 1 CPU core(s), WiFi/BLE, silicon revision 0, 2MB external flash
    FLASH_CRYPT_CNT eFuse value is 1
    Flash encryption feature is enabled in DEVELOPMENT mode


------
