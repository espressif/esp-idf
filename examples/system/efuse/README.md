| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-P4 | ESP32-S2 | ESP32-S3 | Linux |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | ----- |

# eFuse Example

## Overview

This example shows how to use the eFuse API. It demonstrates read and write operations with fields from the common and custom eFuse tables. For more information about eFuse, see the “eFuse Manager” chapter in the documentation and the “eFuse Controller” chapter in ESP32 TRM.

The eFuse is a single bit of non-volatile memory with the restriction that once an eFuse bit is programmed to 1, it can not be reverted to 0.
The eFuse fields can be useful to store: device types, serial numbers, some system variables, etc.

Note that the bits already written cannot be reset to the original state. For debugging purposes, the `CONFIG_EFUSE_VIRTUAL` option is provided. This option will block physical burning. All work happens with an array in RAM. Use `CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH` to keep eFuse changes after reboots.
In this example, all write operations are wrapped in `#ifdef CONFIG_EFUSE_VIRTUAL ... # endif` to prevent accidental burn while testing the features.

## How to use example

This example first reads the eFuse field, then writes a value, then reads it again. The program can be seen in the logs.

The program uses two eFuse tables:

 * The common table (for IDF purposes).
 * The custom table (for custom purposes).

The custom table has five fields:

```
MODULE_VERSION,                   EFUSE_BLK3,    56,  8,    Module version
DEVICE_ROLE,                      EFUSE_BLK3,    64,  3,    Device role
SETTING_1,                        EFUSE_BLK3,    67,  6,    Setting 1
SETTING_2,                        EFUSE_BLK3,    73,  5,    Setting 2
CUSTOM_SECURE_VERSION,            EFUSE_BLK3,    78, 16,    Custom secure version
```

### Hardware Required

To run this example, you need to use the ESP32 developer board. If your ESP32 has the `None` coding scheme of eFuse, then write and read operations have no limitations. But if it has the `3/4` or `RS` coding scheme, the writing may lead to an error due to the fact that some coded value was written earlier. A simple way to find out what coding scheme your chip has, you can see in the log while esptool is working or the log of the program:

```
esptool v5.0.2
Connected to ESP32 on /dev/ttyUSB0
Chip type:          ESP32-U4WDH (revision v3.0)
Features:           Wi-Fi, BT, Dual Core + LP Core, 240MHz, Embedded Flash, Vref calibration in eFuse, Coding Scheme None
Crystal frequency:  40MHz
MAC:                d8:a0:1d:40:ac:90
```

The esptool also has a command to get general information about the chip `esptool chip-id`:

```
esptool v5.0.2
Connected to ESP32 on /dev/ttyUSB0
Chip type:          ESP32-U4WDH (revision v3.0)
Features:           Wi-Fi, BT, Dual Core + LP Core, 240MHz, Embedded Flash, Vref calibration in eFuse, Coding Scheme None
Crystal frequency:  40MHz
MAC:                d8:a0:1d:40:ac:90

Stub flasher running.

Warning: ESP32 has no chip ID. Reading MAC address instead.
MAC:                d8:a0:1d:40:ac:90

Hard resetting via RTS pin...
```

### Configure the project

```
idf.py menuconfig
```

In this example, there is `sdkconfig.default`, which already includes all the necessary settings. No need to configure.

 * `CONFIG_EFUSE_CUSTOM_TABLE=y` - It allows using the custom table.
 * `CONFIG_EFUSE_VIRTUAL=y` - All read and writes operations are redirected to RAM instead of eFuse registers. Not really burning eFuses.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

Running `idf.py monitor` is important for this example, not only to view the UART log. The application prints special `IDF_MONITOR_EXECUTE_ESPEFUSE_* EFSR:..` messages, and `idf.py monitor` recognizes them, runs the corresponding `espefuse` command, and prints the decoded eFuse information in the console. If you only use `idf.py flash` or another serial terminal, you will only see the raw marker lines with an efuse token and will not get the additional `espefuse` output shown below.

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.


## Example Output

For ``RS`` coding scheme (esp32c3):

```
ESP-ROM:esp32c3-eco6-20230321
Build:Mar 21 2023
rst:0x1 (POWERON),boot:0xc (SPI_FAST_FLASH_BOOT)
SPIWP:0xee
mode:DIO, clock div:1
load:0x3fcd5990,len:0x1750
load:0x403cbf10,len:0xd00
load:0x403ce710,len:0x304c
entry 0x403cbf1a
W (25) boot.esp32c3: eFuse virtual mode is enabled. If Secure boot or Flash encryption is enabled then it does not provide any security. FOR TESTING ONLY!
W (31) efuse: [Virtual] Loading virtual efuse blocks from real efuses
I (49) boot: ESP-IDF v6.1-dev-1458-ga7a9419ed8f-dirt 2nd stage bootloader
I (49) boot: compile time Dec 23 2025 13:37:44
I (50) boot: chip revision: v1.0
I (52) boot: efuse block revision: v1.3
I (55) boot.esp32c3: SPI Speed      : 80MHz
I (59) boot.esp32c3: SPI Mode       : DIO
I (63) boot.esp32c3: SPI Flash Size : 2MB
I (67) boot: Enabling RNG early entropy source...
I (71) boot: Partition Table:
I (74) boot: ## Label            Usage          Type ST Offset   Length
I (80) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (87) boot:  1 phy_init         RF data          01 01 0000f000 00001000
I (93) boot:  2 factory          factory app      00 00 00010000 00100000
I (100) boot: End of partition table
I (103) esp_image: segment 0: paddr=00010020 vaddr=3c010020 size=06a24h ( 27172) map
I (115) esp_image: segment 1: paddr=00016a4c vaddr=3fc89200 size=0175ch (  5980) load
I (119) esp_image: segment 2: paddr=000181b0 vaddr=40380000 size=07e68h ( 32360) load
I (131) esp_image: segment 3: paddr=00020020 vaddr=42000020 size=0e6d4h ( 59092) map
I (142) esp_image: segment 4: paddr=0002e6fc vaddr=40387e68 size=011b4h (  4532) load
I (143) esp_image: segment 5: paddr=0002f8b8 vaddr=50000000 size=00024h (    36) load
I (151) boot: Loaded app from partition at offset 0x10000
I (153) boot: Disabling RNG early entropy source...
I (169) cpu_start: Unicore app
I (177) cpu_start: GPIO 20 and 21 are used as console UART I/O pins
I (178) cpu_start: Pro cpu start user code
I (178) cpu_start: cpu freq: 160000000 Hz
I (180) app_init: Application information:
I (183) app_init: Project name:     efuse
I (187) app_init: App version:      v6.1-dev-1458-ga7a9419ed8f-dirt
I (193) app_init: Compile time:     Dec 23 2025 13:37:47
I (198) app_init: ELF file SHA256:  64e4d02c1...
I (203) app_init: ESP-IDF:          v6.1-dev-1458-ga7a9419ed8f-dirt
I (209) efuse_init: Min chip rev:     v0.3
I (212) efuse_init: Max chip rev:     v1.99 
I (216) efuse_init: Chip rev:         v1.0
I (220) heap_init: Initializing. RAM available for dynamic allocation:
I (227) heap_init: At 3FC8BE70 len 00034190 (208 KiB): RAM
I (232) heap_init: At 3FCC0000 len 0001C710 (113 KiB): Retention RAM
I (238) heap_init: At 3FCDC710 len 0000294C (10 KiB): Retention RAM
I (244) heap_init: At 50000024 len 00001FC4 (7 KiB): RTCRAM
I (250) spi_flash: detected chip: generic
I (253) spi_flash: flash io: dio
W (256) spi_flash: Detected size(4096k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
W (268) efuse_init: eFuse virtual mode is enabled. If Secure boot or Flash encryption is enabled then it does not provide any security. FOR TESTING ONLY!
W (281) efuse: [Virtual] Loading virtual efuse blocks from real efuses
I (288) sleep_gpio: Configure to isolate all GPIO pins in sleep state
I (294) sleep_gpio: Enable automatic switching of GPIO sleep configuration
I (301) main_task: Started on CPU0
I (301) main_task: Calling app_main()
I (301) example: Start eFuse example
I (301) example: Initial token dump to log
I (311) example: IDF_MONITOR_EXECUTE_ESPEFUSE_SUMMARY EFSR:esp32c3:100:AAAAAAAAAAAAAAAAAAAAAAAAAIAAAAAA:zIH3-VVgAAAAAAAAAAAAS8kmEVKwQgYB:ZSd8yloMSAJssOWmfZQw8lFbphuTZH574QcV3ggAAAA::::::::::lcxoUA --extend-efuse-table main/esp_efuse_custom_table.csv
--- Executing monitor command: espefuse --token EFSR:esp32c3:100:AAAAAAAAAAAAAAAAAAAAAAAAAIAAAAAA:zIH3-VVgAAAAAAAAAAAAS8kmEVKwQgYB:ZSd8yloMSAJssOWmfZQw8lFbphuTZH574QcV3ggAAAA::::::::::lcxoUA --extend-efuse-table main/esp_efuse_custom_table.csv summary --active
espefuse v5.1.0

=== Run "summary" command ===
EFUSE_NAME (Block) Description  = [Meaningful Value] [Readable/Writeable] (Hex Value)
----------------------------------------------------------------------------------------
Calibration fuses:
K_RTC_LDO (BLOCK1)                                 BLOCK1 K_RTC_LDO                                   = 77 R/W (0b1001101)
K_DIG_LDO (BLOCK1)                                 BLOCK1 K_DIG_LDO                                   = 68 R/W (0b1000100)
V_RTC_DBIAS20 (BLOCK1)                             BLOCK1 voltage of rtc dbias20                      = 144 R/W (0x90)
V_DIG_DBIAS20 (BLOCK1)                             BLOCK1 voltage of digital dbias20                  = 130 R/W (0x82)
DIG_DBIAS_HVT (BLOCK1)                             BLOCK1 digital dbias when hvt                      = 21 R/W (0b10101)
THRES_HVT (BLOCK1)                                 BLOCK1 pvt threshold when hvt                      = 400 R/W (0b0110010000)
TEMP_CALIB (BLOCK2)                                Temperature calibration data                       = -10.600000000000001 R/W (0b101101010)
OCODE (BLOCK2)                                     ADC OCode                                          = 101 R/W (0x65)
ADC1_INIT_CODE_ATTEN0 (BLOCK2)                     ADC1 init code at atten0                           = 442 R/W (0b0110111010)
ADC1_INIT_CODE_ATTEN1 (BLOCK2)                     ADC1 init code at atten1                           = 588 R/W (0b1001001100)
ADC1_INIT_CODE_ATTEN2 (BLOCK2)                     ADC1 init code at atten2                           = 612 R/W (0b1001100100)
ADC1_INIT_CODE_ATTEN3 (BLOCK2)                     ADC1 init code at atten3                           = 735 R/W (0b1011011111)
ADC1_CAL_VOL_ATTEN0 (BLOCK2)                       ADC1 calibration voltage at atten0                 = 535 R/W (0b1000010111)
ADC1_CAL_VOL_ATTEN1 (BLOCK2)                       ADC1 calibration voltage at atten1                 = 31 R/W (0b0000011111)
ADC1_CAL_VOL_ATTEN2 (BLOCK2)                       ADC1 calibration voltage at atten2                 = 533 R/W (0b1000010101)
ADC1_CAL_VOL_ATTEN3 (BLOCK2)                       ADC1 calibration voltage at atten3                 = 567 R/W (0b1000110111)

Config fuses:
ERR_RST_ENABLE (BLOCK0)                            Use BLOCK0 to check error record registers         = with check R/W (0b1)

Flash fuses:
FLASH_CAP (BLOCK1)                                 Flash capacity                                     = 4M R/W (0b001)
FLASH_TEMP (BLOCK1)                                Flash temperature                                  = 105C R/W (0b01)
FLASH_VENDOR (BLOCK1)                              Flash vendor                                       = XMC R/W (0b001)

Identity fuses:
BLK_VERSION_MINOR (BLOCK1)                         BLK_VERSION_MINOR                                  = 3 R/W (0b011)
WAFER_VERSION_MAJOR (BLOCK1)                       WAFER_VERSION_MAJOR                                = 1 R/W (0b01)
OPTIONAL_UNIQUE_ID (BLOCK2)                        Optional unique 128-bit ID                        
   = 65 27 7c ca 5a 0c 48 02 6c b0 e5 a6 7d 94 30 f2 R/W 
BLK_VERSION_MAJOR (BLOCK2)                         BLK_VERSION_MAJOR of BLOCK2                        = With calibration R/W (0b01)

Jtag fuses:

Mac fuses:
MAC (BLOCK1)                                       MAC address                                       
   = 60:55:f9:f7:81:cc (OK) R/W 

Security fuses:

Spi Pad fuses:

Usb fuses:

User fuses:

Vdd fuses:

Wdt fuses:


I (331) example: Coding Scheme RS (Reed-Solomon coding)
I (331) example: read efuse fields
I (341) example: 1. read MAC address: 60:55:f9:f7:81:cc
I (341) example: 2. read secure_version: 0
I (351) example: 3. read custom fields
I (351) example: module_version = 0
I (351) example: device_role = None
I (361) example: setting_1 = 0
I (361) example: setting_2 = 0
I (361) example: custom_secure_version = 0
W (371) example: This example does not burn any efuse in reality only virtually
W (371) example: Write operations in efuse fields are performed virtually
I (381) example: write custom efuse fields
I (381) example: In the case of 3/4 or RS coding scheme, you cannot write efuse fields separately
I (391) example: You should use the batch mode of writing fields for this
I (401) efuse: Batch mode of writing fields is enabled
I (401) example: Token dump of staged eFuse writes:
I (411) example: IDF_MONITOR_EXECUTE_ESPEFUSE_SUMMARY EFSW:esp32c3:100::::AAAAAAAAAAEayAcAAAAAAAAAAAAAAAAAAAAAAAAAAAA:::::::::7jMAoA --extend-efuse-table main/esp_efuse_custom_table.csv
--- Executing monitor command: espefuse --token EFSW:esp32c3:100::::AAAAAAAAAAEayAcAAAAAAAAAAAAAAAAAAAAAAAAAAAA:::::::::7jMAoA --extend-efuse-table main/esp_efuse_custom_table.csv summary --active
espefuse v5.1.0

=== Run "summary" command ===
EFUSE_NAME (Block) Description  = [Meaningful Value] [Readable/Writeable] (Hex Value)
----------------------------------------------------------------------------------------
Config fuses:
BLOCK_USR_DATA (BLOCK3)                            User data                                         
   = 00 00 00 00 00 00 00 01 1a c8 07 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W 

Flash fuses:

Identity fuses:
WAFER_VERSION_MAJOR (BLOCK1)                       WAFER_VERSION_MAJOR                                = 1 R/W (0b01)

Jtag fuses:

Mac fuses:

Security fuses:

Spi Pad fuses:

Usb fuses:

User fuses:
MODULE_VERSION (BLOCK3)                            Module version (56-63)                             = 1 R/W (0x01)
DEVICE_ROLE (BLOCK3)                               Device role (64-66)                                = 2 R/W (0b010)
SETTING_1 (BLOCK3)                                 Setting 1 (67-72)                                  = 3 R/W (0b000011)
SETTING_2 (BLOCK3)                                 Setting 2 (73-77)                                  = 4 R/W (0b00100)
CUSTOM_SECURE_VERSION (BLOCK3)                     Custom secure version (78-93)                      = 31 R/W (0x001f)

Vdd fuses:

Wdt fuses:


W (421) efuse: Virtual efuses enabled: Not really burning eFuses
I (431) efuse: Batch mode. Prepared fields are committed
I (431) example: module_version = 1
I (441) example: device_role = Slave
I (441) example: setting_1 = 3
I (441) example: setting_2 = 4
I (451) example: custom_secure_version = 5
I (451) example: Done
I (451) main_task: Returned from app_main()
```

And for ``3/4`` coding scheme:


```
I (0) cpu_start: Starting scheduler on APP CPU.
I (327) example: Coding Scheme 3/4
I (327) example: Uses common and custom tables
I (337) example: read efuse fields
I (337) example: 1. read MAC address: 84:0d:8e:18:8e:44
I (347) example: 2. read secure_version: 0
I (347) example: 3. read custom fields
I (357) example: module_version = 0
I (357) example: device_role = None
I (367) example: setting_1 = 0
I (367) example: setting_2 = 0
I (367) example: custom_secure_version = 0
W (377) example: This example does not burn any efuse in reality only virtually
W (387) example: Write operations in efuse fields are performed virtually
I (387) example: In the case of 3/4 coding scheme, you cannot write efuse fields separately
I (397) example: You should use the batch mode of writing fields for this
I (407) efuse: Batch mode of writing fields is enabled
W (417) efuse: Virtual efuses enabled: Not really burning eFuses
I (417) efuse: Batch mode of writing fields is disabled
I (427) example: module_version = 1
I (427) example: device_role = Slave
I (437) example: setting_1 = 3
I (437) example: setting_2 = 4
I (437) example: custom_secure_version = 5
I (447) example: Done
```


### How to create the new custom table

In the main folder of the example there is a custom eFuse table - `esp_efuse_custom_table.csv`. You can add there some fields and run `idf.py efuse-custom-table`, it will generate all the necessary source files. If all is ok then you will see:

```
...
Parsing efuse CSV input file /home/kostia/esp/esp-idf/components/efuse/esp32/esp_efuse_table.csv ...
Verifying efuse table...
Parsing efuse CSV input file /home/kostia/esp/esp-idf/examples/system/efuse/main/esp_efuse_custom_table.csv ...
Verifying efuse table...
Creating efuse *.h file /home/kostia/esp/esp-idf/examples/system/efuse/main/include/esp_efuse_custom_table.h ...
Creating efuse *.c file /home/kostia/esp/esp-idf/examples/system/efuse/main/esp_efuse_custom_table.c ...
Max number of bits in BLK 192
```

If the following fields were not changed in the CSV file: field_name, efuse_block, bit_start or bit_count, then a new generation will not occur, and you will see this message `Source files do not require updating correspond to csv file`.

This command will create into main folder two files: `esp_efuse_custom_table.c` and `include/esp_efuse_custom_table.h`.

Do not forget to add these files for the Cmake build system:

```
idf_component_register(SRCS "efuse_main.c" "esp_efuse_custom_table.c"
                       INCLUDE_DIRS "." "include")
```

If you are not sure which eFuse bits are free you can run `idf.py show-efuse-table`. It prints a sorted list of the common and custom tables.
