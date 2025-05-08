| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- |

# NVS Bootloader

The purpose of this example is to show how to use the simplified, read-only API of NVS flash that can be used as a part of bootloader.

A very practical application of being able to access the NVS in the bootloader build would be faster device restoration, where-in the application stores the device's current state/configurations and post a reset it would read the NVS to restore the device's last state, without waiting for application to boot-up.

## Usage of this example:

Simply compile it:
```
idf.py build
```

Then flash it and open the monitor with the following command:
```
idf.py flash monitor
```

If everything went well, the console output should contain three blocks of log messages.
For detailed explanation, please see the comments in the source code file `nvs_bootloader_example.c`

First block shows indication of invalid parameters.

```
I (765) nvs_bootloader_example_utils: Showing request data only
I (765) nvs_bootloader_example_utils: ## Namespace        Key              DT
I (766) nvs_bootloader_example_utils:  0 sunny_day        u8               U8
I (766) nvs_bootloader_example_utils:  1 too_long_sunny_day u8               I8
I (766) nvs_bootloader_example_utils:  2 sunny_day        too_long_dark_key I32
I (767) nvs_bootloader_example_utils:  3 clowny_day       blobeee          BLOB
I (767) nvs_bootloader_example_utils:  4 sunny_day        string_10_chars  STR
I (767) nvs_bootloader_example_utils:  5 sunny_day        string_10_chars  STR
E (769) nvs_bootloader_example: Invalid arguments passed to the read function
I (770) nvs_bootloader_example_utils: Request data with invalid arguments. Function returned ESP_ERR_INVALID_ARG
I (770) nvs_bootloader_example_utils: ## Result                    Namespace        Key              DT
I (771) nvs_bootloader_example_utils:  0 ESP_ERR_NVS_NOT_FOUND     sunny_day        u8               U8
E (772) nvs_bootloader_example_utils:  1 ESP_ERR_NVS_INVALID_NAME  too_long_sunny_day u8               I8
E (772) nvs_bootloader_example_utils:  2 ESP_ERR_NVS_KEY_TOO_LONG  sunny_day        too_long_dark_key I32
E (772) nvs_bootloader_example_utils:  3 ESP_ERR_INVALID_ARG       clowny_day       blobeee          BLOB
E (773) nvs_bootloader_example_utils:  4 ESP_ERR_INVALID_SIZE      sunny_day        string_10_chars  STR
E (773) nvs_bootloader_example_utils:  5 ESP_ERR_INVALID_SIZE      sunny_day        string_10_chars  STR
```

Second block shows indication of inconsistencies detected during the data reading. There can be mix of correctly read entries and entries with errors.

```
I (779) nvs_bootloader_example_utils: Showing request data only
I (779) nvs_bootloader_example_utils: ## Namespace        Key              DT
I (779) nvs_bootloader_example_utils:  0 sunny_day        u8               I8
I (780) nvs_bootloader_example_utils:  1 sunny_day        i32_             I32
I (780) nvs_bootloader_example_utils:  2 clowny_day       i8               I8
I (780) nvs_bootloader_example_utils:  3 sunny_day        string_10_chars  STR
I (780) nvs_bootloader_example_utils:  4 sunny_day        u32              U32
I (781) nvs_bootloader_example_utils:  5 sunny_day        u32              U32
I (1227) nvs_bootloader_example: Data read from NVS partition
I (1227) nvs_bootloader_example_utils: Result data. Function returned ESP_OK
I (1227) nvs_bootloader_example_utils: ## Result                    Namespace        Key              DT  Value
E (1228) nvs_bootloader_example_utils:  0 ESP_ERR_NVS_TYPE_MISMATCH sunny_day        u8               I8
E (1228) nvs_bootloader_example_utils:  1 ESP_ERR_NVS_NOT_FOUND     sunny_day        i32_             I32
E (1228) nvs_bootloader_example_utils:  2 ESP_ERR_NVS_NOT_FOUND     clowny_day       i8               I8
E (1229) nvs_bootloader_example_utils:  3 ESP_ERR_INVALID_SIZE      sunny_day        string_10_chars  STR
I (1229) nvs_bootloader_example_utils:  4 ESP_OK                    sunny_day        u32              U32 4294967295
E (1229) nvs_bootloader_example_utils:  5 ESP_ERR_NVS_NOT_FOUND     sunny_day        u32              U32
```

Final block of log messages shows the successful reading of various data

```
I (1230) nvs_bootloader_example_utils: Showing request data only
I (1230) nvs_bootloader_example_utils: ## Namespace        Key              DT
I (1230) nvs_bootloader_example_utils:  0 sunny_day        u8               U8
I (1231) nvs_bootloader_example_utils:  1 sunny_day        i32              I32
I (1231) nvs_bootloader_example_utils:  2 cloudy_day       i8               I8
I (1231) nvs_bootloader_example_utils:  3 sunny_day        u32              U32
I (1231) nvs_bootloader_example_utils:  4 sunny_day        i8               I8
I (1232) nvs_bootloader_example_utils:  5 sunny_day        u16              U16
I (1232) nvs_bootloader_example_utils:  6 sunny_day        i16              I16
I (1232) nvs_bootloader_example_utils:  7 sunny_day        string_10_chars  STR
I (1665) nvs_bootloader_example: Data read from NVS partition
I (1665) nvs_bootloader_example_utils: Result data. Function returned ESP_OK
I (1666) nvs_bootloader_example_utils: ## Result                    Namespace        Key              DT  Value
I (1666) nvs_bootloader_example_utils:  0 ESP_OK                    sunny_day        u8               U8  255
I (1666) nvs_bootloader_example_utils:  1 ESP_OK                    sunny_day        i32              I32 -2147483648
I (1667) nvs_bootloader_example_utils:  2 ESP_OK                    cloudy_day       i8               I8  -13
I (1667) nvs_bootloader_example_utils:  3 ESP_OK                    sunny_day        u32              U32 4294967295
I (1668) nvs_bootloader_example_utils:  4 ESP_OK                    sunny_day        i8               I8  -128
I (1668) nvs_bootloader_example_utils:  5 ESP_OK                    sunny_day        u16              U16 65535
I (1668) nvs_bootloader_example_utils:  6 ESP_OK                    sunny_day        i16              I16 -20000
I (1669) nvs_bootloader_example_utils:  7 ESP_OK                    sunny_day        string_10_chars  STR Text_67890
```

At the end the firmware starts regular application and just prints the message:

```
I (3212) main_task: Calling app_main()
User application is loaded and running.
I (3212) main_task: Returned from app_main()
```

## Organisation of this example

The code demonstrating the use of nvs in bootloader is in `bootloader_components/nvs_bootloader_example`. This code is executed during bootloader run.
Bootloader hooks technique is used to extend the default bootloader. The main function demonstrating the functionality is `bootloader_after_init()`
in the `src/nvs_bootloader_example`

Below is a short explanation of files in the project folder.

```
├── CMakeLists.txt
├── main
│   ├── CMakeLists.txt
│   └── main.c                                  Regular application, just prints message and ends
├── bootloader_components
│   └── nvs_bootloader_example
│       ├── CMakeLists.txt
│       ├── nvs_bootloader_example_utils.c      Supplementary functions for easier logging the content of request / response data
│       └── nvs_bootloader_example.c            Implementation of main test. All supplenentary information in the form of comments can be found here.
├── nvs_data.csv                                Initial content of the nvs partition. Data to be read by the nvs_bootloader_read are here
└── README.md                                   This is the file you are currently reading
```

The example creates request/response array `read_list[]`, populates it with identifiers of the data to be read.
Function `nvs_bootloader_read()` tries to find respective data in the partition (here `"nvs"`) and if the data is found, it populates the request/response array with data. For nvs entries either not found or not matching are indicated in response array as well.
Function `log_nvs_bootloader_read_list()`is used before and after reading from nvs to show request/response data to the console.

# Encrypted NVS Bootloader

This example is extended to support reading encrypted NVS partition when NVS encryption is enabled.

## Usage of this example:

Enable NVS encryption using your preferred scheme. Please find more details regarding the `flash encryption based NVS encryption scheme` and the `HMAC based NVS encryption scheme` in the [NVS encryption documentation](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/storage/nvs_encryption.html).

(Note: In case you select the `HMAC based NVS encryption scheme`, make sure that you burn the below mentioned [HMAC key](./main/nvs_enc_hmac_key.bin) in the efuses.)

For generating the encrypted NVS partitions, we shall use [NVS partition generator](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/storage/nvs_partition_gen.html#nvs-partition-generator-utility).
We shall use the [nvs_partition_gen.py](../../../../components/nvs_flash/nvs_partition_generator/nvs_partition_gen.py) script for the operations.

Along with the above mentioned file structure, the project folder also contains pre-generated encrypted partitions and the partition corresponding to the selected NVS encryption scheme is flashed along with the build artefacts using the `main/CMakeLists.txt`.

In case the data in `nvs_data.csv` is modified, these encrypted NVS partitions can be re-generated using the following commands:

1. NVS Encryption using the flash encryption scheme

```
python nvs_partition_gen.py encrypt $IDF_PATH/examples/storage/nvs/nvs_bootloader/nvs_data.csv $IDF_PATH/examples/storage/nvs/nvs_bootloader/main/nvs_encrypted.bin 0x6000 --inputkey $IDF_PATH/examples/storage/nvs/nvs_bootloader/main/encryption_keys.bin
```

2. NVS Encryption using the HMAC scheme

```
python nvs_partition_gen.py encrypt $IDF_PATH/examples/storage/nvs/nvs_bootloader/nvs_data.csv $IDF_PATH/examples/storage/nvs/nvs_bootloader/main/nvs_encrypted_hmac.bin 0x6000 --keygen --key_protect_hmac --kp_hmac_inputkey $IDF_PATH/examples/storage/nvs/nvs_bootloader/main/nvs_enc_hmac_key.bin
```

Build the application using configurations corresponding to the NVS encryption scheme that you have selected:

```
idf.py set-target <target>

# For NVS encryption using flash encryption scheme
cat sdkconfig.ci.nvs_enc_flash_enc >> sdkconfig

OR

# For NVS encryption using the HMAC scheme
cat sdkconfig.ci.nvs_enc_hmac >> sdkconfig

idf.py build
```

Then flash it and open the monitor with the following command:
```
idf.py flash monitor
```

If everything went well, the console output should contain the same three blocks of log messages that are mentioned above.

### Running the example using QEMU

You could quickly try out this example using QEMU. Refer this [link](https://github.com/espressif/esp-toolchain-docs/blob/main/qemu/README.md#choose-your-target) to know which targets are currently supported in QEMU.

#### Using the NVS encryption's flash encryption scheme

1. Configure the application with the corresponding configurations

```
idf.py set-target <qemu-supported-targets>

cat sdkconfig.ci.nvs_enc_flash_enc >> sdkconfig

# Disable the below config as it was enabled as a CI related configuration, thus enabling flash encryption during boot-up in QEMU
echo "CONFIG_SECURE_FLASH_REQUIRE_ALREADY_ENABLED=n" >> sdkconfig

```

2. Building the app

```
idf.py build
```

3. Running the app

```
idf.py qemu monitor
```

#### Using the NVS encryption's HMAC scheme

1. Build the application using the corresponding configurations

```
idf.py set-target <qemu-supported-targets>

cat sdkconfig.ci.nvs_enc_hmac >> sdkconfig

# Disable the below config as it was enabled as a CI related configuration, thus enabling flash encryption during boot-up in QEMU
echo "CONFIG_SECURE_FLASH_REQUIRE_ALREADY_ENABLED=n" >> sdkconfig
```

2. Building the app

```
idf.py build
```

3. Burn the related HMAC key in the efuses

```
idf.py qemu efuse-burn-key BLOCK_KEY0 main/nvs_enc_hmac_key.bin HMAC_UP
```

4. Running the app

```
idf.py qemu monitor
```
