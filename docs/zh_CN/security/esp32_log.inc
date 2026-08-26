
.. first_boot_enc

.. code-block:: bash

    --- idf_monitor on /dev/cu.SLAB_USBtoUART 115200 ---
    --- Quit: Ctrl+] | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H ---
    ets Jun  8 2016 00:22:57

    rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
    configsip: 0, SPIWP:0xee
    clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
    mode:DIO, clock div:2
    load:0x3fff0018,len:4
    load:0x3fff001c,len:8452
    load:0x40078000,len:13608
    load:0x40080400,len:6664
    entry 0x40080764
    I (28) boot: ESP-IDF v4.0-dev-850-gc4447462d-dirty 2nd stage bootloader
    I (29) boot: compile time 15:37:14
    I (30) boot: Enabling RNG early entropy source...
    I (35) boot: SPI Speed      : 40MHz
    I (39) boot: SPI Mode       : DIO
    I (43) boot: SPI Flash Size : 4MB
    I (47) boot: Partition Table:
    I (51) boot: ## Label            Usage          Type ST Offset   Length
    I (58) boot:  0 nvs              WiFi data        01 02 0000a000 00006000
    I (66) boot:  1 phy_init         RF data          01 01 00010000 00001000
    I (73) boot:  2 factory          factory app      00 00 00020000 00100000
    I (81) boot: End of partition table
    I (85) esp_image: segment 0: paddr=0x00020020 vaddr=0x3f400020 size=0x0808c ( 32908) map
    I (105) esp_image: segment 1: paddr=0x000280b4 vaddr=0x3ffb0000 size=0x01ea4 (  7844) load
    I (109) esp_image: segment 2: paddr=0x00029f60 vaddr=0x40080000 size=0x00400 (  1024) load
    0x40080000: _WindowOverflow4 at esp-idf/esp-idf/components/freertos/xtensa_vectors.S:1778

    I (114) esp_image: segment 3: paddr=0x0002a368 vaddr=0x40080400 size=0x05ca8 ( 23720) load
    I (132) esp_image: segment 4: paddr=0x00030018 vaddr=0x400d0018 size=0x126a8 ( 75432) map
    0x400d0018: _flash_cache_start at ??:?

    I (159) esp_image: segment 5: paddr=0x000426c8 vaddr=0x400860a8 size=0x01f4c (  8012) load
    0x400860a8: prvAddNewTaskToReadyList at esp-idf/esp-idf/components/freertos/tasks.c:4561

    I (168) boot: Loaded app from partition at offset 0x20000
    I (168) boot: Checking flash encryption...
    I (168) flash_encrypt: Generating new flash encryption key...
    I (187) flash_encrypt: Read & write protecting new key...
    I (187) flash_encrypt: Setting CRYPT_CONFIG efuse to 0xF
    W (188) flash_encrypt: Not disabling UART bootloader encryption
    I (195) flash_encrypt: Disable UART bootloader decryption...
    I (201) flash_encrypt: Disable UART bootloader MMU cache...
    I (208) flash_encrypt: Disable JTAG...
    I (212) flash_encrypt: Disable ROM BASIC interpreter fallback...
    I (219) esp_image: segment 0: paddr=0x00001020 vaddr=0x3fff0018 size=0x00004 (     4)
    I (227) esp_image: segment 1: paddr=0x0000102c vaddr=0x3fff001c size=0x02104 (  8452)
    I (239) esp_image: segment 2: paddr=0x00003138 vaddr=0x40078000 size=0x03528 ( 13608)
    I (249) esp_image: segment 3: paddr=0x00006668 vaddr=0x40080400 size=0x01a08 (  6664)
    I (657) esp_image: segment 0: paddr=0x00020020 vaddr=0x3f400020 size=0x0808c ( 32908) map
    I (669) esp_image: segment 1: paddr=0x000280b4 vaddr=0x3ffb0000 size=0x01ea4 (  7844)
    I (672) esp_image: segment 2: paddr=0x00029f60 vaddr=0x40080000 size=0x00400 (  1024)
    0x40080000: _WindowOverflow4 at esp-idf/esp-idf/components/freertos/xtensa_vectors.S:1778

    I (676) esp_image: segment 3: paddr=0x0002a368 vaddr=0x40080400 size=0x05ca8 ( 23720)
    I (692) esp_image: segment 4: paddr=0x00030018 vaddr=0x400d0018 size=0x126a8 ( 75432) map
    0x400d0018: _flash_cache_start at ??:?

    I (719) esp_image: segment 5: paddr=0x000426c8 vaddr=0x400860a8 size=0x01f4c (  8012)
    0x400860a8: prvAddNewTaskToReadyList at esp-idf/esp-idf/components/freertos/tasks.c:4561

    I (722) flash_encrypt: Encrypting partition 2 at offset 0x20000...
    I (13229) flash_encrypt: Flash encryption completed
    I (13229) boot: Resetting with flash encryption enabled...

------

.. already_en_enc

.. code-block:: bash

    rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
    configsip: 0, SPIWP:0xee
    clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
    mode:DIO, clock div:2
    load:0x3fff0018,len:4
    load:0x3fff001c,len:8452
    load:0x40078000,len:13652
    ho 0 tail 12 room 4
    load:0x40080400,len:6664
    entry 0x40080764
    I (30) boot: ESP-IDF v4.0-dev-850-gc4447462d-dirty 2nd stage bootloader
    I (30) boot: compile time 16:32:53
    I (31) boot: Enabling RNG early entropy source...
    I (37) boot: SPI Speed      : 40MHz
    I (41) boot: SPI Mode       : DIO
    I (45) boot: SPI Flash Size : 4MB
    I (49) boot: Partition Table:
    I (52) boot: ## Label            Usage          Type ST Offset   Length
    I (60) boot:  0 nvs              WiFi data        01 02 0000a000 00006000
    I (67) boot:  1 phy_init         RF data          01 01 00010000 00001000
    I (75) boot:  2 factory          factory app      00 00 00020000 00100000
    I (82) boot: End of partition table
  I (86) esp_image: segment 0: paddr=0x00020020 vaddr=0x3f400020 size=0x0808c ( 32908) map
    I (107) esp_image: segment 1: paddr=0x000280b4 vaddr=0x3ffb0000 size=0x01ea4 (  7844) load
    I (111) esp_image: segment 2: paddr=0x00029f60 vaddr=0x40080000 size=0x00400 (  1024) load
    0x40080000: _WindowOverflow4 at esp-idf/esp-idf/components/freertos/xtensa_vectors.S:1778

    I (116) esp_image: segment 3: paddr=0x0002a368 vaddr=0x40080400 size=0x05ca8 ( 23720) load
    I (134) esp_image: segment 4: paddr=0x00030018 vaddr=0x400d0018 size=0x126a8 ( 75432) map
    0x400d0018: _flash_cache_start at ??:?

    I (162) esp_image: segment 5: paddr=0x000426c8 vaddr=0x400860a8 size=0x01f4c (  8012) load
    0x400860a8: prvAddNewTaskToReadyList at esp-idf/esp-idf/components/freertos/tasks.c:4561

    I (171) boot: Loaded app from partition at offset 0x20000
    I (171) boot: Checking flash encryption...
    I (171) flash_encrypt: flash encryption is enabled (3 plaintext flashes left)
    I (178) boot: Disabling RNG early entropy source...
    I (184) cpu_start: Pro cpu up.
    I (188) cpu_start: Application information:
    I (193) cpu_start: Project name:     flash-encryption
    I (198) cpu_start: App version:      v4.0-dev-850-gc4447462d-dirty
    I (205) cpu_start: Compile time:     Jun 17 2019 16:32:52
    I (211) cpu_start: ELF file SHA256:  8770c886bdf561a7...
    I (217) cpu_start: ESP-IDF:          v4.0-dev-850-gc4447462d-dirty
    I (224) cpu_start: Starting app cpu, entry point is 0x40080e4c
    0x40080e4c: call_start_cpu1 at esp-idf/esp-idf/components/{IDF_TARGET_PATH_NAME}/cpu_start.c:265

    I (0) cpu_start: App cpu up.
    I (235) heap_init: Initializing. RAM available for dynamic allocation:
    I (241) heap_init: At 3FFAE6E0 len 00001920 (6 KiB): DRAM
    I (247) heap_init: At 3FFB2EC8 len 0002D138 (180 KiB): DRAM
    I (254) heap_init: At 3FFE0440 len 00003AE0 (14 KiB): D/IRAM
    I (260) heap_init: At 3FFE4350 len 0001BCB0 (111 KiB): D/IRAM
    I (266) heap_init: At 40087FF4 len 0001800C (96 KiB): IRAM
    I (273) cpu_start: Pro cpu start user code
    I (291) cpu_start: Starting scheduler on PRO CPU.
    I (0) cpu_start: Starting scheduler on APP CPU.

    Sample program to check Flash Encryption
    This is ESP32 chip with 2 CPU cores, WiFi/BT/BLE, silicon revision 1, 4MB external flash
    Flash encryption feature is enabled
    Flash encryption mode is DEVELOPMENT
    Flash in encrypted mode with flash_crypt_cnt = 1
    Halting...

------
