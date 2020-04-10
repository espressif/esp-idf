| Supported Targets | ESP32 |
| ----------------- | ----- |

# Himem API Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example uses the [Himem API](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/system/himem.html) to run a memory test of the upper 4 MiB of an 8 MiB PSRAM chip.

The ESP32 has the ability to access external SPI RAM in the same way internal memory can be accessed. In other words, if enabled in project configuration, you can allocate memory in external RAM using standard C allocation APIs like `malloc()`, `calloc()`, etc.

However, because of hardware limitations, this only works for up to 4 MiB of external memory. If you have, for instance, an ESP32-WROVER module with 8 MiB of PSRAM, you cannot use the upper 4 MiB of PSRAM this way. However, this memory is not wasted, using the Himem API (which essentially is a bank switching scheme for the upper memory regions), the upper 4 MiB is still usable. 

The Himem subsystem does this by reserving some amount of address space, then allowing applications to swap in and out normally unreachable ranges of physical SPI RAM. While this does not allow transparent access in the way memory allocated with `malloc()` does, it does provide an usable way to store data for e.g., large audio or video buffers in the upper 4 MiB.

This example uses the Himem API to run a simple memory test of the entire range of upper memory. It illustrates how to allocate address space to bankswitch the physical memory in, allocate the physical memory, and switch it in or out of the allocated address space.

## How to use example

### Hardware Required

This example requires an ESP32 with external SPI RAM connected, for instance an ESP32-WROVER module. The example is intended to run on an ESP32 with 8 MiB external RAM connected. It will still run on an ESP32 with 4 MiB external RAM, but in practice using Himem with such a setup does not make much sense.

### Configure the project

```
idf.py menuconfig
```

* Make sure that SPI RAM bank switching is enabled. (Compiling the example with default values will automatically enable this.) It can be found under `Component config > ESP32-specific > Support for external, SPI-connected RAM  > SPI RAM config`.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
rst:0x1 (POWERON_RESET),boot:0x1e (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:2
load:0x3fff0018,len:4
load:0x3fff001c,len:6124
load:0x40078000,len:10084
load:0x40080400,len:6552
entry 0x40080764
I (28) boot: ESP-IDF v3.2-dev-1455-ga51d5706f-dirty 2nd stage bootloader
I (29) boot: compile time 18:51:28
I (30) boot: Enabling RNG early entropy source...
I (35) boot: SPI Speed      : 40MHz
I (39) boot: SPI Mode       : DIO
I (43) boot: SPI Flash Size : 4MB
I (47) boot: Partition Table:
I (51) boot: ## Label            Usage          Type ST Offset   Length
I (58) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (66) boot:  1 phy_init         RF data          01 01 0000f000 00001000
I (73) boot:  2 factory          factory app      00 00 00010000 00100000
I (81) boot: End of partition table
I (85) esp_image: segment 0: paddr=0x00010020 vaddr=0x3f400020 size=0x0dde0 ( 56800) map
I (114) esp_image: segment 1: paddr=0x0001de08 vaddr=0x3ff80000 size=0x00000 (     0) load
I (114) esp_image: segment 2: paddr=0x0001de10 vaddr=0x3ff80000 size=0x00000 (     0) load
I (120) esp_image: segment 3: paddr=0x0001de18 vaddr=0x3ffb0000 size=0x01fb4 (  8116) load
I (132) esp_image: segment 4: paddr=0x0001fdd4 vaddr=0x3ffb1fb4 size=0x00000 (     0) load
I (138) esp_image: segment 5: paddr=0x0001fddc vaddr=0x40080000 size=0x00234 (   564) load
I (147) esp_image: segment 6: paddr=0x00020018 vaddr=0x400d0018 size=0x180d4 ( 98516) map
I (191) esp_image: segment 7: paddr=0x000380f4 vaddr=0x40080234 size=0x001cc (   460) load
I (191) esp_image: segment 8: paddr=0x000382c8 vaddr=0x40080400 size=0x0e14c ( 57676) load
I (221) esp_image: segment 9: paddr=0x0004641c vaddr=0x400c0000 size=0x00000 (     0) load
I (222) esp_image: segment 10: paddr=0x00046424 vaddr=0x50000000 size=0x00000 (     0) load
I (228) esp_image: segment 11: paddr=0x0004642c vaddr=0x50000000 size=0x00000 (     0) load
I (245) boot: Loaded app from partition at offset 0x10000
I (246) boot: Disabling RNG early entropy source...
I (250) spiram: Found 64MBit SPI RAM device
I (254) spiram: SPI RAM mode: flash 40m sram 40m
I (259) spiram: PSRAM initialized, cache is in low/high (2-core) mode.
I (266) cpu_start: Pro cpu up.
I (270) cpu_start: Starting app cpu, entry point is 0x40081174
I (0) cpu_start: App cpu up.
I (735) spiram: SPI SRAM memory test OK
I (735) heap_init: Initializing. RAM available for dynamic allocation:
I (735) heap_init: At 3FFAE6E0 len 00001920 (6 KiB): DRAM
I (741) heap_init: At 3FFB3448 len 0002CBB8 (178 KiB): DRAM
I (748) heap_init: At 3FFE0440 len 00003AE0 (14 KiB): D/IRAM
I (754) heap_init: At 3FFE4350 len 0001BCB0 (111 KiB): D/IRAM
I (760) heap_init: At 4008E54C len 00011AB4 (70 KiB): IRAM
I (767) cpu_start: Pro cpu start user code
I (771) spiram: Adding pool of 2112K of external SPI memory to heap allocator
I (121) esp_himem: Initialized. Using last 62 32KB address blocks for bank switching on 6080 KB of physical memory.
I (122) cpu_start: Starting scheduler on PRO CPU.
I (0) cpu_start: Starting scheduler on APP CPU.
I (132) spiram: Reserving pool of 32K of internal memory for DMA/internal allocations
Himem has 6080 KiB of memory, 6080 KiB of which is free. Testing the free memory...
Done!
```
