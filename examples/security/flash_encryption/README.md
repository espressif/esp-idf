| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- |

# Flash Encryption

The example demonstrates the flash encryption application by providing a code using FATFS and NVS partitions on a device with the flash encryption enabled.
As the flash encryption can be enabled either in Development or Release mode, a guidance of how to use either mode is provided. 

The example code checks if the flash encryption feature is enabled/disabled and if enabled, it prints a status information of all the eFuses related to the flash encryption mode (Development/Release) and FLASH_CRYPT_CNT (for ESP32) or SPI_BOOT_CRYPT_CNT (for ESP32-S2 and newer targets).

The example also demonstrates: 

1. Writing and reading encrypted partitions in the flash memory.
2. Initialization of FATFS, formatting, writing and reading file. Both encrypted as well as non-encrypted.
3. Initialization of encrypted NVS partition. Both auto-generated as well as pre-generated NVS key scenarios are presented.
4. Flashing the example in Development as well as Release mode.

### NVS example
The example demonstrates default and custom NVS partition initialisation when the flash encryption is enabled. From the code perspective the use of NVS API is transparent regardless of the flash encryption mode.

### FATFS example
FATFS example function finds non-encrypted partition `fat_not_encr`, erases it, creates FATFS, formats and mounts the file system. Then it creates file `/spiflash/inner.txt` using `fopen`, writes test string using `fprintf` and closes the file using `fclose`. Then it uses `fopen` and `fgets` to read the string back to verify it is correctly written. The last step of the example on unencrypted partition uses `esp_flash_read` to locate the test string in the underlying partition without involving potential cryptographic layer. The test string offset from the beginning of the partition is returned to the second part of the example.

The second part of the example repeats FATFS related steps on the encrypted partition named `fat_encrypted`. The same sequence of steps as in the non encrypted partition means, that the file data should be present on the same offset in the partition. Therefore, direct partition reading using `esp_flash_read` uses the offset returned by the first part of example. Depending on encryption settings in the menuconfig, the test string is then read in the visible form `(SECURE_FLASH_ENC_ENABLED not set)` or in encrypted form `(SECURE_FLASH_ENC_ENABLED=y)`

## How to use the example

### Hardware Required

### Configure the project

```
idf.py menuconfig
```

#### Flash encryption configuration
Enable the flash encryption mode (Development or Release) under (`menuconfig -> Security Features`). Default usage mode is Development (recommended during test and development phase).

Note: After enabling flash encryption, the bootloader size increases, which means that default offset of the partition table (`menuconfig -> Partition Table -> Offset of partition table`) has to be increased from default value of 0x8000 to prevent the bootloader from overlapping with the partition table. In this example, the offset of the partition table is incereased to 0xD000.

If you chose the Release mode, for better security, you can also disable UART ROM download mode using (`menuconfig -> Security features -> UART ROM download mode -> Permanently disabled`)
Note: This option, after first start of application, makes the device forever inaccessible using UART. Use it with care.

This example demonstrates working with several non-default partitions, therefore a custom partition configuration file is selected. The custom file option (`menuconfig -> Partition Table -> Partition table = Custom partition table CSV`) and the name of file is set in (`menuconfig -> Partition Table -> Custom partition CSV file`). The additional entries in the custom partition table CSV file on top of default ones are described in the NVS and FATFS related subsections below.

For better security, the NVS encryption (`menuconfig -> Component config -> NVS -> Enable NVS encryption`) is enabled by default when the flash encryption is enabled in the menuconfig. If you decide to disable the NVS encryption, you can skip the NVS configuration step given below.

#### Configuration for NVS encryption
With the NVS encryption is enabled, the partition table must contain additional [NVS key partition](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/nvs_flash.html#nvs-key-partition) atop of all the NVS partitions holding data. This example uses two NVS partitions (`storage`, `nvs`) and one NVS key partition `nvs_keys`. All the partitions required are already preconfigured in `partitions_example.csv`

The partition configuration for the NVS encryption involves generating NVS encryption keys to be stored in [NVS key partition](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/nvs_flash.html#nvs-key-partition). The NVS encryption keys can be either automatically generated by ESP32-based chip during the first run of the application, or generated and flashed to the device by the application developer.

Automatic generation of NVS encryption keys on ESP32-based chips:
When the NVS encryption is enabled, the `nvs_flash_init` API function can internally generate the XTS encryption keys on the ESP32-based chip. The API function finds the first [NVS key partition](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/nvs_flash.html#nvs-key-partition), i.e. a partition of type  `data` and subtype `nvs_keys` and when this partition is empty, the API function automatically generates NVS keys. (Consult the [`nvs_flash_init`](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/nvs_flash.html#_CPPv414nvs_flash_initv) API documentation in the ESP-IDF programming guide for more details).

**Please note that `nvs_keys` partition has to be completely erased before starting the application. Otherwise the application may generate `ESP_ERR_NVS_CORRUPT_KEY_PART` error code assuming that `nvs_keys` partition was not empty and contains malformed data.**

Flash pre-generated NVS encryption keys:
This method is useful when application developer requires presence of known NVS encryption keys on a device. I.e. to allow easier analysis of the NVS partition on a host computer.
For convenience, file `sample_encryption_keys.bin` with a sample of pre-generated NVS encryption keys is provided.

#### Configuration for FATFS encryption
FATFS encryption example uses two additional partitions in the partition table. Both partitions are of type `data` and subtype `fat`. The first partition `fat_encrypted` has the encrypted flag set, the second partition `fat_not_encr` has the encryption flag off. Both partitions are defined in the partition configuration file `partitions_example.csv`

### Building

```
idf.py build
```

### Flashing

There are two flash encryption modes: Development and Release.
ESP32-based device's bootloader in the Development mode can encrypt the data to be flashed, whilst the bootloader in the Release mode cannot. Therefore the data to be flashed in Release mode has to be encrypted on a host system in advance. The workflows for both options are described below.

Prerequisites:
- Flashing port of connected ESP32-based device is stored in the environment variable PORT, e.g. `export PORT=/dev/cu.usbserial-14320`
- Current directory is set to the example root, e.g. `esp-idf/examples/security/flash_encryption`

#### Development mode

1. Build and flash the partition table:
```
idf.py --port $PORT partition-table partition-table-flash
```

2. If you want use pre-generated NVS encryption keys, flash the `sample_encryption_keys.bin` into the `nvs_key`partition (on the flash) with the help of [parttool.py](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/partition-tables.html#partition-tool-parttool-py):
```
parttool.py --port $PORT --partition-table-offset 0xD000 write_partition --partition-name="nvs_key" --input sample_encryption_keys.bin
```
The sample [NVS key partition](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/nvs_flash.html#nvs-key-partition) partition used in this example is generated with the help of [NVS Partition Generator Utility](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/nvs_partition_gen.html#nvs-partition-generator-utility)


3. When building the project and flashing it to the board FOR THE FIRST TIME (after enabling the flash encryption feature in menuconfig), run the following command to program the target and monitor the output:

```
idf.py --port $PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

4. When reprogramming the device, subsequently use the following command for encrypted write of the new non-encrypted application's binary:

```
idf.py --port $PORT encrypted-app-flash monitor
```

5. Please note that above-mentioned command programs only the 'app' partition. In order to reprogram all the partitions (bootloader, partition table and application) and let the bootloader encrypt them on the device, use:

```
idf.py --port $PORT encrypted-flash monitor
```

#### Release mode

Full example including host generated NVS encryption keys and pre-encryption of the flash on the host is provided for ESP32 ECO3.
Steps:

1. Generate an unique flash encryption key on a host
```
espsecure.py generate_flash_encryption_key example_flash_encryption_key.bin
```

Alternatively, if you want to further develop the workflow, it might be useful to use all-zeroes "easy to restore" flash encryption key instead. However, never use this key in a production!
```
dd if=/dev/zero of=example_flash_encryption_key.bin bs=1 count=32
```

2. NVS encryption key
This workflow uses NVS encryption key provided by the example repository. It will be flashed to the `nvs_key` partition.
File name is `sample_encryption_keys.bin`

3. Prepare data partitions to be flashed
Developers requiring pre-initialized partitions can use NVS Partition Generator Utility for NVS or FAT File System Generator for FAT filesystem.
This example assumes empty partitions.

```
dd if=/dev/zero bs=1 count=0x1000 of=build/storage.bin
dd if=/dev/zero bs=1 count=0x6000 of=build/nvs.bin
dd if=/dev/zero bs=1 count=0x6000 of=build/custom_nvs.bin
dd if=/dev/zero bs=1 count=0x96000 of=build/fat_encrypted.bin
dd if=/dev/zero bs=1 count=0x96000 of=build/fat_not_encr.bin
```

4. Encrypt flash artefacts on a host using flash encryption key. 
First, figure out the offsets of particular artefacts in the flash memory, as it is one of the inputs to the encryption algorithm. 
Lookup the section listing partition table in output of the command `idf.py build` i.e.
```
Partition table binary generated. Contents:
*******************************************************************************
# ESP-IDF Partition Table
# Name, Type, SubType, Offset, Size, Flags
nvs,data,nvs,0xe000,24K,
storage,data,255,0x14000,4K,encrypted
factory,app,factory,0x20000,1M,
nvs_key,data,nvs_keys,0x120000,4K,encrypted
custom_nvs,data,nvs,0x121000,24K,
fat_encrypted,data,fat,0x127000,600K,encrypted
fat_not_encr,data,fat,0x1bd000,600K,
*******************************************************************************
```

The offsets of the bootloader, application code and the partition table binaries can be found in `build/flash_args`
```
0x1000 bootloader/bootloader.bin
0x20000 flash_encryption.bin
0xd000 partition_table/partition-table.bin
```

From the example application perspective it is necessary to encrypt every artefact except for 'nvs', 'custom_nvs' and 'fat_not_encr' partitions, as their content is expected to be plaintext.

```
espsecure.py encrypt_flash_data --keyfile example_flash_encryption_key.bin --output build/bootloader_encrypted.bin --address 0x1000 build/bootloader/bootloader.bin
espsecure.py encrypt_flash_data --keyfile example_flash_encryption_key.bin --output build/flash_encryption_encrypted.bin --address 0x20000 build/flash_encryption.bin
espsecure.py encrypt_flash_data --keyfile example_flash_encryption_key.bin --output build/partition_table_encrypted.bin --address 0xd000 build/partition_table/partition-table.bin
espsecure.py encrypt_flash_data --keyfile example_flash_encryption_key.bin --output build/storage_encrypted.bin --address 0x14000 build/storage.bin
espsecure.py encrypt_flash_data --keyfile example_flash_encryption_key.bin --output build/sample_encryption_keys_encrypted.bin --address 0x120000 sample_encryption_keys.bin
espsecure.py encrypt_flash_data --keyfile example_flash_encryption_key.bin --output build/fat_encrypted_encrypted.bin --address 0x127000 build/fat_encrypted.bin
```

5. Merge all the flash artefacts into one file
```
esptool.py --chip ESP32 merge_bin -o build\merged_encrypted_flash.bin --flash_mode dio --flash_size 4MB \
0x1000 build/bootloader_encrypted.bin \
0xd000 build/partition_table_encrypted.bin \
0xe000 build/nvs.bin \
0x14000 build/storage_encrypted.bin \
0x20000 build/flash_encryption_encrypted.bin \
0x120000 build/sample_encryption_keys_encrypted.bin \
0x121000 build/nvs.bin \
0x127000 build/fat_encrypted_encrypted.bin \
0x1bd000 build/fat_not_encr.bin
```
Ignore the warning `Warning: Image file at 0x1000 doesn't look like an image file, so not changing any flash settings.`

Output flash binary file is `build\merged_encrypted_flash.bin`

6. Burn the encryption key and set the efuses in ESP32-based device
This step can be performed only once. If the workflow is using non-zero flash encryption key generated in step 1, keep the encryption key file on a safe place for future application updates. 

```
espefuse.py --chip esp32 --do-not-confirm --port $PORT burn_key flash_encryption example_flash_encryption_key.bin
espefuse.py --chip esp32 --do-not-confirm --port $PORT burn_efuse FLASH_CRYPT_CONFIG 0xf
espefuse.py --chip esp32 --do-not-confirm --port $PORT burn_efuse FLASH_CRYPT_CNT 127
```

7. Burn security eFuses
WARNING: This step can be performed only once! 
```
espefuse.py --chip esp32 --do-not-confirm --port $PORT burn_efuse DISABLE_DL_ENCRYPT 0x1
espefuse.py --chip esp32 --do-not-confirm --port $PORT burn_efuse DISABLE_DL_DECRYPT 0x1
espefuse.py --chip esp32 --do-not-confirm --port $PORT burn_efuse DISABLE_DL_CACHE 0x1
espefuse.py --chip esp32 --do-not-confirm --port $PORT burn_efuse JTAG_DISABLE 0x1
```

8. Flash the image to the ESP32-based device
```
python -m esptool --chip esp32 --port $PORT -b 460800 write_flash --force --flash_mode dio \
--flash_size 4MB --flash_freq 40m \
0x0000 build\merged_encrypted_flash.bin
```

9. Monitor the output, then reset the device 
```
idf.py --port $PORT monitor
```

10. You can verify the efuse settings using
```
espefuse.py --chip esp32 --port $PORT summary
```

For subsequent application update, only steps 4,5,8 are required. 


## Example Output

### Development mode

When running the example without enabling flash encryption, the output would be as follows (on ESP32):

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
....
I (509) example: FAT partition "fat_not_encr" is not encrypted. Size is (0x96000 bytes) 
....
I (3429) example: Read partition using esp_flash_read until test string is found
I (3649) example: 0x3ffb4f90   74 68 65 20 71 75 69 63  6b 20 62 72 6f 77 6e 20  |the quick brown |
I (3649) example: 0x3ffb4fa0   66 6f 78 20 6a 75 6d 70  65 64 20 6f 76 65 72 20  |fox jumped over |
I (3659) example: 0x3ffb4fb0   74 68 65 20 6c 61 7a 79  20 64 6f 67              |the lazy dog|
I (3669) example: Test string was found at offset (0x7000)
....
I (3679) example: FAT partition "fat_encrypted" is not encrypted. Size is (0x96000 bytes)
....
I (7099) example: Read partition using esp_flash_read at expected offset (0x7000) 
I (7099) example: 0x3ffb4f84   74 68 65 20 71 75 69 63  6b 20 62 72 6f 77 6e 20  |the quick brown |
I (7099) example: 0x3ffb4f94   66 6f 78 20 6a 75 6d 70  65 64 20 6f 76 65 72 20  |fox jumped over |
I (7109) example: 0x3ffb4fa4   74 68 65 20 6c 61 7a 79  20 64 6f 67              |the lazy dog|
I (7119) example: Data matches test string
```
Note: Above, the partition `fat_encrypted` has flag `encrypted` set in partition table, but it's actual state 
is `not encrypted` until the flash encryption is enabled in security part of menuconfig as demonstrated below.
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
....
I (583) example: FAT partition "fat_not_encr" is not encrypted. Size is (0x96000 bytes)
....
I (3863) example: Read partition using esp_flash_read until test string is found
I (4083) example: 0x3ffb4fb0   74 68 65 20 71 75 69 63  6b 20 62 72 6f 77 6e 20  |the quick brown |
I (4083) example: 0x3ffb4fc0   66 6f 78 20 6a 75 6d 70  65 64 20 6f 76 65 72 20  |fox jumped over |
I (4093) example: 0x3ffb4fd0   74 68 65 20 6c 61 7a 79  20 64 6f 67              |the lazy dog|
I (4103) example: Test string was found at offset (0x7000)
....
I (4113) example: FAT partition "fat_encrypted" is encrypted. Size is (0x96000 bytes) 
....
I (8033) example: Read partition using esp_flash_read at expected offset (0x7000) 
I (8033) example: 0x3ffb4fa4   a7 ea d5 a7 ed cf f6 f7  4a a2 54 a0 4f 92 73 7b  |........J.T.O.s{|
I (8043) example: 0x3ffb4fb4   63 eb 5d fc 14 b9 da 3b  f2 be d0 94 de eb b2 dc  |c.]....;........|
I (8053) example: 0x3ffb4fc4   38 aa 14 62 b7 23 61 7d  b6 34 43 53              |8..b.#a}.4CS|
I (8063) example: Data does not match test string
```
If the NVS encryption is enabled, then the output will show the status of the encrypted partition as follows

```
I (667) example_nvs: NVS partition "nvs" is encrypted.
```

### Release mode

In the Release mode no in-place encryption happens after the first reset, as the bootloader assumes the flashed data is already encrypted. The log below shows the run of application after successfully finished flashing.
```
Example to check Flash Encryption status
This is esp32 chip with 2 CPU core(s), WiFi/BT/BLE, silicon revision v1.0, 4MB external flash
FLASH_CRYPT_CNT eFuse value is 127
Flash encryption feature is enabled in RELEASE mode
Erasing partition "storage" (0x1000 bytes)
Writing data with esp_partition_write:
I (483) example: 0x3ffb5000   00 01 02 03 04 05 06 07  08 09 0a 0b 0c 0d 0e 0f  |................|
I (483) example: 0x3ffb5010   10 11 12 13 14 15 16 17  18 19 1a 1b 1c 1d 1e 1f  |................|
Reading with esp_partition_read:
I (493) example: 0x3ffb4fe0   00 01 02 03 04 05 06 07  08 09 0a 0b 0c 0d 0e 0f  |................|
I (503) example: 0x3ffb4ff0   10 11 12 13 14 15 16 17  18 19 1a 1b 1c 1d 1e 1f  |................|
Reading with esp_flash_read:
I (513) example: 0x3ffb4fe0   16 0a 38 06 c8 29 31 32  b2 1f 5a ca da e9 47 99  |..8..)12..Z...G.|
I (523) example: 0x3ffb4ff0   1d ca d6 6d 55 ad 31 57  d3 d9 df 77 43 f9 4c ca  |...mU.1W...wC.L.|
....
I (543) example_fatfs: FAT partition "fat_not_encr" is not encrypted. Size is (0x96000 bytes)
....
I (3573) example_fatfs: Read partition using esp_flash_read until test string is found
I (3793) example_fatfs: 0x3ffb4fc0   74 68 65 20 71 75 69 63  6b 20 62 72 6f 77 6e 20  |the quick brown |
I (3803) example_fatfs: 0x3ffb4fd0   66 6f 78 20 6a 75 6d 70  65 64 20 6f 76 65 72 20  |fox jumped over |
I (3803) example_fatfs: 0x3ffb4fe0   74 68 65 20 6c 61 7a 79  20 64 6f 67              |the lazy dog|
I (3813) example_fatfs: Test string was found at offset (0x7000)
....
I (3823) example_fatfs: FAT partition "fat_encrypted" is encrypted. Size is (0x96000 bytes) 
....
I (7303) example_fatfs: Read partition using esp_flash_read at expected offset (0x7000) 
I (7303) example_fatfs: 0x3ffb4fb4   3b 8f f4 e1 c1 f4 73 43  d9 28 3f 57 f3 2b d2 b0  |;.....sC.(?W.+..|
I (7313) example_fatfs: 0x3ffb4fc4   41 7a 8f 09 81 1d 92 51  74 8a ee 2a c3 64 8f 75  |Az.....Qt..*.d.u|
I (7323) example_fatfs: 0x3ffb4fd4   61 fe 08 b6 b4 0c cb 08  5a b8 65 29              |a.......Z.e)|
I (7333) example_fatfs: Data does not match test string
```
If the NVS encryption is enabled, then the output will show the status of the encrypted partition as follows
```
I (7373) nvs: NVS partition "nvs" is encrypted.
I (7413) example: NVS partition "custom_nvs" is encrypted.
```

## Troubleshooting

It is also possible to use esptool.py utility to read the eFuse values and check if flash encryption is enabled or not

```
python $IDF_PATH/components/esptool_py/esptool/espefuse.py --port $PORT summary
```

If FLASH_CRYPT_CNT (for ESP32) or SPI_BOOT_CRYPT_CNT (for ESP32-S2 and newer targets) eFuse value is non-zero, the flash encryption is enabled


