| Supported Targets | ESP32 |
| ----------------- | ----- |

# Flash Encryption

The example checks if the flash encryption feature is enabled/disabled and if enabled prints the flash encryption mode (DEVELOPMENT / RELEASE) and FLASH_CRYPT_CNT eFuse value.

The example also demonstrates writing and reading encrypted partitions in flash.

## How to use example

### Hardware Required

### Configure the project

```
idf.py menuconfig
```

* Enable the flash encryption mode (Development or Release) under Security Features. Default usage mode is Development (recommended during test and development phase).

Note: After enabling flash encryption, the bootloader size increases, which means that the offset of the partition table must be changed to 0x9000 from 0x8000 to prevent the bootloader from overlapping with the partition table. In this example, the default offset of the partition table is 0x9000.

### Build and Flash

When building the project and flashing it to the board FOR THE FIRST TIME after enabling flash encryption feature in menuconfig, run following command to program ESP32 and monitor the output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

When reprogramming the device subsequently use following command for encrypted write of new plaintext application:

```
idf.py -p PORT encrypted-app-flash monitor
```

Please note above command programs only the app partition. In order to reprogram all partitions (bootloader, partition table and application) in encrypted form use:

```
idf.py -p PORT encrypted-flash monitor
```

## Example Output

When running the example without enabling flash encryption, the output would be as follows:

```
Example to check Flash Encryption status
This is ESP32 chip with 2 CPU cores, WiFi/BT/BLE, silicon revision 0, 2MB external flash
FLASH_CRYPT_CNT eFuse value is 0
Flash encryption feature is disabled
Erasing partition "storage" (0x1000 bytes)
Writing data with esp_partition_write:
I (378) example: 0x3ffb4dc0   00 01 02 03 04 05 06 07  08 09 0a 0b 0c 0d 0e 0f  |................|
I (378) example: 0x3ffb4dd0   10 11 12 13 14 15 16 17  18 19 1a 1b 1c 1d 1e 1f  |................|
Reading with esp_partition_read:
I (388) example: 0x3ffb4da0   00 01 02 03 04 05 06 07  08 09 0a 0b 0c 0d 0e 0f  |................|
I (398) example: 0x3ffb4db0   10 11 12 13 14 15 16 17  18 19 1a 1b 1c 1d 1e 1f  |................|
Reading with spi_flash_read:
I (408) example: 0x3ffb4da0   00 01 02 03 04 05 06 07  08 09 0a 0b 0c 0d 0e 0f  |................|
I (418) example: 0x3ffb4db0   10 11 12 13 14 15 16 17  18 19 1a 1b 1c 1d 1e 1f  |................|
```

After enabling flash encryption in Development mode, the output shows the process of enabling the flash encryption:

```
I (168) boot: Checking flash encryption...
I (168) flash_encrypt: Generating new flash encryption key...
I (187) flash_encrypt: Read & write protecting new key...
I (187) flash_encrypt: Setting CRYPT_CONFIG efuse to 0xF
W (188) flash_encrypt: Not disabling UART bootloader encryption
I (195) flash_encrypt: Disable UART bootloader decryption...
I (201) flash_encrypt: Disable UART bootloader MMU cache...
I (208) flash_encrypt: Disable JTAG...
I (212) flash_encrypt: Disable ROM BASIC interpreter fallback...
....
....
....
I (13229) flash_encrypt: Flash encryption completed
I (13229) boot: Resetting with flash encryption enabled...
```

Once the flash encryption is enabled the device will reset itself. At this stage the flash contents are in encrypted form. The output would be similar to:

```
Example to check Flash Encryption status
This is ESP32 chip with 2 CPU cores, WiFi/BT/BLE, silicon revision 0, 4MB external flash
FLASH_CRYPT_CNT eFuse value is 1
Flash encryption feature is enabled in DEVELOPMENT mode
Erasing partition "storage" (0x1000 bytes)
Writing data with esp_partition_write:
I (451) example: 0x3ffb4dc0   00 01 02 03 04 05 06 07  08 09 0a 0b 0c 0d 0e 0f  |................|
I (451) example: 0x3ffb4dd0   10 11 12 13 14 15 16 17  18 19 1a 1b 1c 1d 1e 1f  |................|
Reading with esp_partition_read:
I (461) example: 0x3ffb4da0   00 01 02 03 04 05 06 07  08 09 0a 0b 0c 0d 0e 0f  |................|
I (471) example: 0x3ffb4db0   10 11 12 13 14 15 16 17  18 19 1a 1b 1c 1d 1e 1f  |................|
Reading with spi_flash_read:
I (491) example: 0x3ffb4b30   35 9b f2 07 b4 6d 40 89  28 b4 1e 22 98 7b 4a 36  |5....m@.(..".{J6|
I (491) example: 0x3ffb4b40   ba 89 81 67 77 a3 60 5e  0a e7 51 01 b3 58 c2 f6  |...gw.`^..Q..X..|
```

## Troubleshooting

It is also possible to use esptool.py utility to read the eFuse values and check if flash encryption is enabled or not

```
python $IDF_PATH/components/esptool_py/esptool/espefuse.py --port PORT summary
```

If FLASH_CRYPT_CNT eFuse value is non-zero flash encryption is enabled

