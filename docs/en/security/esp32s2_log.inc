
.. first_boot_enc

.. code-block:: none

  ESP-ROM:esp32s2-rc4-20191025
  Build:Oct 25 2019
  rst:0x1 (POWERON),boot:0x8 (SPI_FAST_FLASH_BOOT)
  SPIWP:0xee
  mode:DIO, clock div:1
  load:0x3ffe6260,len:0x78
  load:0x3ffe62d8,len:0x231c
  load:0x4004c000,len:0x9d8
  load:0x40050000,len:0x3cf8
  entry 0x4004c1ec
  I (48) boot: ESP-IDF qa-test-v4.3-20201113-777-gd8e1 2nd stage bootloader
  I (48) boot: compile time 11:24:04
  I (48) boot: chip revision: 0
  I (52) boot.esp32s2: SPI Speed      : 80MHz
  I (57) boot.esp32s2: SPI Mode       : DIO
  I (62) boot.esp32s2: SPI Flash Size : 2MB
  I (66) boot: Enabling RNG early entropy source...
  I (72) boot: Partition Table:
  I (75) boot: ## Label            Usage          Type ST Offset   Length
  I (83) boot:  0 nvs              WiFi data        01 02 0000a000 00006000
  I (90) boot:  1 storage          Unknown data     01 ff 00010000 00001000
  I (98) boot:  2 factory          factory app      00 00 00020000 00100000
  I (105) boot: End of partition table
  I (109) esp_image: segment 0: paddr=0x00020020 vaddr=0x3f000020 size=0x0618c ( 24972) map
  I (124) esp_image: segment 1: paddr=0x000261b4 vaddr=0x3ffbcae0 size=0x02624 (  9764) load
  I (129) esp_image: segment 2: paddr=0x000287e0 vaddr=0x40022000 size=0x00404 (  1028) load
  I (136) esp_image: segment 3: paddr=0x00028bec vaddr=0x40022404 size=0x0742c ( 29740) load
  0x40022404: _coredump_iram_end at ??:?

  I (153) esp_image: segment 4: paddr=0x00030020 vaddr=0x40080020 size=0x1457c ( 83324) map
  0x40080020: _stext at ??:?

  I (171) esp_image: segment 5: paddr=0x000445a4 vaddr=0x40029830 size=0x032ac ( 12972) load
  I (181) boot: Loaded app from partition at offset 0x20000
  I (181) boot: Checking flash encryption...
  I (181) efuse: Batch mode of writing fields is enabled
  I (188) flash_encrypt: Generating new flash encryption key...
  W (199) flash_encrypt: Not disabling UART bootloader encryption
  I (201) flash_encrypt: Disable UART bootloader cache...
  I (207) flash_encrypt: Disable JTAG...
  I (212) efuse: Batch mode of writing fields is disabled
  I (217) esp_image: segment 0: paddr=0x00001020 vaddr=0x3ffe6260 size=0x00078 (   120)
  I (226) esp_image: segment 1: paddr=0x000010a0 vaddr=0x3ffe62d8 size=0x0231c (  8988)
  I (236) esp_image: segment 2: paddr=0x000033c4 vaddr=0x4004c000 size=0x009d8 (  2520)
  I (243) esp_image: segment 3: paddr=0x00003da4 vaddr=0x40050000 size=0x03cf8 ( 15608)
  I (651) flash_encrypt: bootloader encrypted successfully
  I (704) flash_encrypt: partition table encrypted and loaded successfully
  I (704) flash_encrypt: Encrypting partition 1 at offset 0x10000 (length 0x1000)...
  I (765) flash_encrypt: Done encrypting
  I (766) esp_image: segment 0: paddr=0x00020020 vaddr=0x3f000020 size=0x0618c ( 24972) map
  I (773) esp_image: segment 1: paddr=0x000261b4 vaddr=0x3ffbcae0 size=0x02624 (  9764)
  I (778) esp_image: segment 2: paddr=0x000287e0 vaddr=0x40022000 size=0x00404 (  1028)
  I (785) esp_image: segment 3: paddr=0x00028bec vaddr=0x40022404 size=0x0742c ( 29740)
  0x40022404: _coredump_iram_end at ??:?

  I (799) esp_image: segment 4: paddr=0x00030020 vaddr=0x40080020 size=0x1457c ( 83324) map
  0x40080020: _stext at ??:?

  I (820) esp_image: segment 5: paddr=0x000445a4 vaddr=0x40029830 size=0x032ac ( 12972)
  I (823) flash_encrypt: Encrypting partition 2 at offset 0x20000 (length 0x100000)...
  I (13869) flash_encrypt: Done encrypting
  I (13870) flash_encrypt: Flash encryption completed
  I (13870) boot: Resetting with flash encryption enabled...


------

.. already_en_enc

.. code-block:: none

  ESP-ROM:esp32s2-rc4-20191025
  Build:Oct 25 2019
  rst:0x3 (RTC_SW_SYS_RST),boot:0x8 (SPI_FAST_FLASH_BOOT)
  Saved PC:0x40051242
  SPIWP:0xee
  mode:DIO, clock div:1
  load:0x3ffe6260,len:0x78
  load:0x3ffe62d8,len:0x231c
  load:0x4004c000,len:0x9d8
  load:0x40050000,len:0x3cf8
  entry 0x4004c1ec
  I (56) boot: ESP-IDF qa-test-v4.3-20201113-777-gd8e1 2nd stage bootloader
  I (56) boot: compile time 11:24:04
  I (56) boot: chip revision: 0
  I (60) boot.esp32s2: SPI Speed      : 80MHz
  I (65) boot.esp32s2: SPI Mode       : DIO
  I (69) boot.esp32s2: SPI Flash Size : 2MB
  I (74) boot: Enabling RNG early entropy source...
  I (80) boot: Partition Table:
  I (83) boot: ## Label            Usage          Type ST Offset   Length
  I (90) boot:  0 nvs              WiFi data        01 02 0000a000 00006000
  I (98) boot:  1 storage          Unknown data     01 ff 00010000 00001000
  I (105) boot:  2 factory          factory app      00 00 00020000 00100000
  I (113) boot: End of partition table
  I (117) esp_image: segment 0: paddr=0x00020020 vaddr=0x3f000020 size=0x0618c ( 24972) map
  I (132) esp_image: segment 1: paddr=0x000261b4 vaddr=0x3ffbcae0 size=0x02624 (  9764) load
  I (137) esp_image: segment 2: paddr=0x000287e0 vaddr=0x40022000 size=0x00404 (  1028) load
  I (144) esp_image: segment 3: paddr=0x00028bec vaddr=0x40022404 size=0x0742c ( 29740) load
  0x40022404: _coredump_iram_end at ??:?

  I (161) esp_image: segment 4: paddr=0x00030020 vaddr=0x40080020 size=0x1457c ( 83324) map
  0x40080020: _stext at ??:?

  I (180) esp_image: segment 5: paddr=0x000445a4 vaddr=0x40029830 size=0x032ac ( 12972) load
  I (190) boot: Loaded app from partition at offset 0x20000
  I (191) boot: Checking flash encryption...
  I (191) flash_encrypt: flash encryption is enabled (1 plaintext flashes left)
  I (199) boot: Disabling RNG early entropy source...
  I (216) cache: Instruction cache 	: size 8KB, 4Ways, cache line size 32Byte
  I (216) cpu_start: Pro cpu up.
  I (268) cpu_start: Pro cpu start user code
  I (268) cpu_start: cpu freq: 160000000
  I (268) cpu_start: Application information:
  I (271) cpu_start: Project name:     flash_encryption
  I (277) cpu_start: App version:      qa-test-v4.3-20201113-777-gd8e1
  I (284) cpu_start: Compile time:     Dec 21 2020 11:24:00
  I (290) cpu_start: ELF file SHA256:  30fd1b899312fef7...
  I (296) cpu_start: ESP-IDF:          qa-test-v4.3-20201113-777-gd8e1
  I (303) heap_init: Initializing. RAM available for dynamic allocation:
  I (310) heap_init: At 3FF9E000 len 00002000 (8 KiB): RTCRAM
  I (316) heap_init: At 3FFBF898 len 0003C768 (241 KiB): DRAM
  I (323) heap_init: At 3FFFC000 len 00003A10 (14 KiB): DRAM
  W (329) flash_encrypt: Flash encryption mode is DEVELOPMENT (not secure)
  I (336) spi_flash: detected chip: generic
  I (341) spi_flash: flash io: dio
  W (345) spi_flash: Detected size(4096k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
  I (358) cpu_start: Starting scheduler on PRO CPU.

  Example to check Flash Encryption status
  This is esp32s2 chip with 1 CPU core(s), WiFi, silicon revision 0, 2MB external flash
  FLASH_CRYPT_CNT eFuse value is 1
  Flash encryption feature is enabled in DEVELOPMENT mode

------
