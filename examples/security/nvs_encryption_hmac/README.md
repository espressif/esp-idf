| Supported Targets | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- | -------- | -------- | -------- |

# NVS Encryption with HMAC-based encryption key protection scheme

## Overview

This example demonstrates NVS encryption using the HMAC peripheral, wherein the encryption keys are derived from the HMAC key burnt in eFuse. Since the derivation of the encryption keys occurs at runtime, they are not stored in the flash. Thus, this feature does not require a separate `nvs_keys` partition and _also does not require flash encryption enabled_.

## How to use the example

### Hardware Required

This example can be executed on any development board with a supported Espressif SOC chip - possessing a built-in HMAC peripheral (see `Supported Targets` table above).

### Configure the project

Before the project configuration and build, be sure to set the correct chip target using `idf.py set-target <chip_name>`.

Open the project configuration menu (`idf.py menuconfig`).

#### Configure the eFuse key ID storing the HMAC key

- Set the eFuse key ID storing the HMAC key at `Component config → NVS Security Provider → eFuse key ID storing the HMAC key`.
The HMAC key stored at this key block will be used to generate the encryption keys for the default NVS partition (`nvs`), initialised with `nvs_flash_init()`. Note that the example will fail to build without setting this config option to the correct value (the default value is out of range).

- Users can program their own HMAC key in the configured block before running the example - refer below snippet. The example checks if the configured block is empty or already programmed with an HMAC key - if empty, a new key is generated at runtime and stored in the block, or else the provided key is used. While burning the key prior to flashing the app, please make sure that the config value is set to the eFuse block holding the HMAC key.

```shell
# Burning the HMAC-key in eFuse block 0 - key ID 0
espefuse.py -p PORT burn_key BLOCK_KEY0 hmac_key_file.bin HMAC_UP
```

### Build and Flash

Build the project and flash it to the board, then run the monitor tool to view the serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type `Ctrl-]`.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

### Example Output

```log
I (300) nvs_sec_provider: NVS Encryption - Registering HMAC-based scheme...
I (308) app_start: Starting scheduler on CPU0
I (313) main_task: Started on CPU0
I (313) main_task: Calling app_main()
I (313) example: Initialising the default NVS partition
I (333) nvs: NVS partition "nvs" is encrypted.
I (603) example: Initialising the custom NVS partition
I (613) example: NVS partition "custom_nvs" is encrypted.
I (623) example: Key: u8_key | Val: 255
I (623) example: Key: i8_key | Val: -128
I (623) example: Key: u16_key | Val: 65535
I (633) example: Key: u32_key | Val: 4294967295
I (633) example: Key: i32_key | Val: -2147483648
I (643) example: Key: str_key | Val: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
Fusce quis risus justo.
Suspendisse egestas in nisi sit amet auctor.
Pellentesque rhoncus dictum sodales.
In justo erat, viverra at interdum eget, interdum vel dui.

I (663) custom_nvs: 0x3ffc5f5c   fe ff ff ff 00 00 00 00  fe ff ff ff ff ff ff ff  |................|
I (673) custom_nvs: 0x3ffc5f6c   ff ff ff ff ff ff ff ff  ff ff ff ff 84 2d ba b9  |.............-..|
I (683) custom_nvs: 0x3ffc5f7c   aa aa aa fa ff ff ff ff  ff ff ff ff ff ff ff ff  |................|
I (693) custom_nvs: 0x3ffc5f8c   ff ff ff ff ff ff ff ff  ff ff ff ff ff ff ff ff  |................|
I (703) custom_nvs: 0x3ffc5f9c   ca 8b c3 bb 2d c2 33 d6  6b d4 a7 3d 31 0e 9c 36  |....-.3.k..=1..6|
I (713) custom_nvs: 0x3ffc5fac   39 7f bc d4 5c 6d f8 98  de 0a 90 50 21 23 ff 04  |9...\m.....P!#..|
I (723) custom_nvs: 0x3ffc5fbc   ce f9 23 6f 2c d6 07 08  2d 0e d2 f2 a5 af 5a 2e  |..#o,...-.....Z.|
I (733) custom_nvs: 0x3ffc5fcc   c9 61 bd fc 96 fc 12 87  1b 8c cb fb 51 2c ed a2  |.a..........Q,..|
...
...
...
I (1133) custom_nvs: 0x3ffc624c   ff ff ff ff ff ff ff ff  ff ff ff ff ff ff ff ff  |................|
I (1143) main_task: Returned from app_main()
```
