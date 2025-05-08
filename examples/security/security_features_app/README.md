| Supported Targets | ESP32-C3 | ESP32-S3 |
| ----------------- | -------- | -------- |

# Security Features Application

This example demonstrates how to enable all of the available security features on the supported target at once (see `Supported Targets` table above).
This example shall use combination of host based python tools and firmware for enabling all the related security eFuses.
For simplicity, the security features such as Secure Boot V2, Flash Encryption, NVS Encryption shall be enabled through host based python tools (e.g., espefuse).
Some additional security eFuses shall be enabled in the firmware.

**The device that has followed all the steps mentioned below can be used for production use-cases.**

> [!CAUTION]
> The instructions in the example directly burn eFuses and once done, it cannot be reverted. Please go through the below steps carefully before executing the example. All the steps must be followed without any changes and in the exact sequence, otherwise the device may end up in an unrecoverable state. Follow the [QEMU workflow](#enable-security-features-with-help-of-qemu) if you want to test the example without the risk of bricking an actual device.
> Some target may provide additional configuration and functionalities. Please visit the documentation for the respective target for more information.

### Hardware Required

* A development board with supported target SoC (see `Supported Targets` table above)
* A USB cable for power supply and programming

**Note: The hardware is not required if you plan to run the example with help of [QEMU](#enable-security-features-with-help-of-qemu).**

## Pre-requisites

### 1. Set ESPPORT

In the example, we need to use the Serial port in nearly all the commands. To make it easier, we shall set the ESPPORT environment variable at once and reuse it later. See the documentation about [Connecting the ESP device to PC](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/establish-serial-connection.html#connect-esp32-to-pc) to find out the Serial port.

```sh
export ESPPORT=/* Serial port to which esp is connected */
```

<details>
    <summary>Setup serial port for QEMU</summary>
    If you are enabling eFuses on ESP32-C3 emulated using QEMU then the serial port should be set as follows:

    export ESPPORT=socket://localhost:5555

   Please note that this value is set in correspondence with the command to start serial connection with QEMU, do not change the values.

</details>

**Make sure to perform this step every time when you open a new terminal to use `esptool/idf` commands.**

### 2. Set the target

The target can be set with following command:

```sh
idf.py set-target <chip_name>
```

### 3. Erase flash

Erase the flash on the device to ensure a clean state.

```shell
idf.py -p $ESPPORT erase-flash
```

**Note: This step is not required for QEMU emulation.**

### 4. Installing qemu (optional)

If you want to enable the security features on a target which has been virtually emulated using qemu then you need to install the necessary packages.

The detailed instructions on how to use QEMU can be found in the [QEMU documentation](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/tools/qemu.html).

<details>
<summary>Target specific documentation</summary>

For target specific documentation, run the following command after setting the [target](#2-set-the-target)

```sh
idf.py docs -sp api-guides/tools/qemu.html
```
</details>

## Enabling Security Features

We shall enable the necessary security features one by one as follows:

### Enabling Secure Boot V2

For more details about Secure Boot V2 protocol checkout the [Secure boot V2 documentation](https://docs.espressif.com/projects/esp-idf/en/latest/security/secure-boot-v2.html).

<details>
<summary>Target specific documentation</summary>

For target specific documentation, run the following command after setting the [target](#2-set-the-target)

```sh
idf.py docs -sp security/secure-boot-v2.html
```
</details>

Please follow below steps to enable Secure Boot V2:


1. Generate Signing Key

	```
	idf.py secure-generate-signing-key -v2 --scheme rsa3072 secure_boot_signing_key.pem
	```

2. Generate Public Key Digest

	```
	idf.py secure-generate-key-digest --keyfile secure_boot_signing_key.pem --output digest.bin
	```

3. Burn the key digest in eFuse

	```
	idf.py --port $ESPPORT efuse-burn-key BLOCK_KEY0 digest.bin SECURE_BOOT_DIGEST0
	```

	<details>
	<summary>Burn key digest for QEMU</summary>
	For QEMU emulation, the above command can be updated as follows:

	```
	idf.py qemu efuse-burn-key BLOCK_KEY0 digest.bin SECURE_BOOT_DIGEST0
	```
	</details>

	where,
	* `$ESPPORT` is the serial port to which the esp32 chip is connected.
	* We have used `BLOCK_KEY0` here to store the Secure Boot V2 digest. Generally, the `BLOCK` can be a free key block from `BLOCK_KEY0` to `BLOCK_KEY5`.


4. Enable Secure Boot V2

	```
	idf.py --port $ESPPORT efuse-burn SECURE_BOOT_EN
	```

	<details>
	<summary>Enable Secure Boot v2 for QEMU</summary>
	For QEMU emulation, the above command can be updated as follows:

	```
	idf.py qemu efuse-burn SECURE_BOOT_EN
	```
	</details>

With above steps the Secure Boot V2 feature shall be enabled. The firmware build step is configured to generate signed binaries for `bootloader` and `application` by default (so there is no need to manually sign).
The necessary `security eFuses` are yet to be burned. They shall be burned by the application when first launched.

#### Use multiple Secure Boot V2 signing keys
**It is recommended to use multiple secure boot v2 signing keys**.

When the application is built (later in the workflow) the `bootloader` and `application` shall only be signed with the first key. To sign it with multiple keys, please follow below additional steps:

-  Repeat `Step 1` to `Step 3` for `secure_boot_signing_key_2.pem` and `secure_boot_signing_key_3.pem` respectively.
-  Sign it with remaining two keys by executing following commands with adding `-a` option for `secure_boot_signing_key_2.pem` and `secure_boot_signing_key_3.pem` respectively:

	```
	idf.py secure-sign-data --version 2 --keyfile <signing_key_file> --output my-app-signed.bin security_features_app.bin
	```
	```
	idf.py secure-sign-data --version 2 --keyfile <signing_key_file> --output my-app-signed.bin security_features_app.bin
	```

**Note: The working directory of above command is set to `build`. If the key file path is relative, then it should be relative to the `build` directory**

### Enabling Flash Encryption

Details about the Flash Encryption protocol can be found at the [Flash Encryption documentation](https://docs.espressif.com/projects/esp-idf/en/latest/security/flash-encryption.html).

<details>
<summary>Target specific documentation</summary>

For target specific documentation, run the following command after setting the [target](#2-set-the-target)

```sh
idf.py docs -sp security/flash-encryption.html
```
</details>

The example also demonstrates reading and writing encrypted partitions in flash memory.

Follow below steps to enable Flash Encryption:

1. Generate Flash Encryption keys

	```
	idf.py secure-generate-flash-encryption-key my_flash_encryption_key.bin
	```

	Note: It is recommended to ensure that the RNG used by host machine to generate the flash encryption key has good entropy.

2. Burn Flash Encryption keys

	```
	idf.py --port $ESPPORT efuse-burn-key BLOCK_KEY1 my_flash_encryption_key.bin XTS_AES_128_KEY
	```

	<details>
	<summary>Burn Flash Encryption Key for QEMU</summary>
	For QEMU emulation, the above command can be updated as follows:

	```
	idf.py qemu efuse-burn-key BLOCK_KEY1 my_flash_encryption_key.bin XTS_AES_128_KEY
	```
	</details>
	

    We have used `BLOCK_KEY1` here to store the Flash Encryption key. Generally, the `BLOCK` can be a free key block from `BLOCK_KE0` to `BLOCK_KEY4`.

3. Enable Flash Encryption

	```
	idf.py --port $ESPPORT efuse-burn SPI_BOOT_CRYPT_CNT 7
	```
	
	<details>
	<summary>Enable Flash Encryption for QEMU</summary>
	For QEMU emulation, the above command can be updated as follows:

	```
	idf.py qemu efuse-burn SPI_BOOT_CRYPT_CNT 7
	```
	</details>
    
At this point the Flash Encryption feature is enabled for the device. The necessary `security eFuses` shall be enabled by the `security_features_app` firmware.

#### Encrypting the partitions

After the application is built (Later in the workflow), all partitions that need encryption can be encrypted with the following command:

```
idf.py secure-encrypt-flash-data --aes-xts --keyfile ../my_flash_encryption_key.bin --address /* Placeholder for partition offset */ --output /* Placeholder for Output File */ /* Placeholder for File to encrypt */
```

The bootloader offset for esp32c3 is `0x0`. The partition table offset for the example has been set to `0xD000` which can be changed through menuconfig. The partition offset for other partitions can be obtained by running ```idf.py partition-table```

For this example we need to encrypt only the following 3 partitions: `bootloader.bin`, `partition-table.bin`, `security_features_app.bin`.

It can be done with following commands:

First create a directory to store all the encrypted binaries

```shell
mkdir build/encrypted_data
```

Encrypt all the binaries one by one and store in `build/encrypted_data`

```shell
idf.py secure-encrypt-flash-data --aes-xts --keyfile ../my_flash_encryption_key.bin --address 0x0 --output encrypted_data/bootloader-enc.bin bootloader/bootloader.bin
```

```shell
idf.py secure-encrypt-flash-data --aes-xts --keyfile ../my_flash_encryption_key.bin --address 0xD000 --output encrypted_data/partition-table-enc.bin partition_table/partition-table.bin
```

```shell
idf.py secure-encrypt-flash-data --aes-xts --keyfile ../my_flash_encryption_key.bin --address 0x20000 --output encrypted_data/security_features-enc.bin security_features_app.bin
```

Please refer to [Encrypted Partition](https://docs.espressif.com/projects/esp-idf/en/latest/security/flash-encryption.html#encrypted-partitions) to check which partitions must be encrypted by default.

### Enabling NVS Encryption

We shall use the `HMAC based NVS encryption scheme`, Please find more details in the [NVS encryption documentation](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/storage/nvs_encryption.html#nvs-encryption-hmac-peripheral-based-scheme)

<details>
<summary>Target specific documentation</summary>

For target specific documentation, run the following command after setting the [target](#2-set-the-target)

```sh
idf.py docs -sp api-reference/storage/nvs_encryption.html
```
</details>

For generation of NVS encryption keys and NVS partition, we shall use [NVS partition generator](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/storage/nvs_partition_gen.html#nvs-partition-generator-utility).

<details>
<summary>Target specific documentation</summary>

For target specific documentation, run the following command after setting the [target](#2-set-the-target)

```sh
idf.py docs -sp api-reference/storage/nvs_partition_gen.html#nvs-partition-generator-utility
```
</details>

We shall use the [nvs_partition_gen.py](../../../components/nvs_flash/nvs_partition_generator/nvs_partition_gen.py) script for the operations.

1. Generate HMAC key and NVS encryption key

	```
	idf.py secure-generate-nvs-partition-key --keyfile ncs_encr_key.bin --encryption-scheme HMAC --hmac-keyfile hmac_key.bin
	```
2. Burn the HMAC key in eFuse

	```
	idf.py --port $ESPPORT efuse-burn-key BLOCK_KEY2 keys/hmac_key.bin HMAC_UP
	```

	<details>
	<summary>Burn HMAC key for QEMU</summary>
	For QEMU emulation, the above command can be updated as follows:

	```
	idf.py qemu efuse-burn-key BLOCK_KEY2 keys/hmac_key.bin HMAC_UP
	```
	</details>
    
    We have used `BLOCK_KEY2` here to store the HMAC key. Generally, `BLOCK` can be a free keyblock between `BLOCK_KEY0` and `BLOCK_KEY5`.

	If you want to change the value of the eFuse key block for this example, make sure to update the same value in `menuconfig → Component config → NVS Security Provider → eFuse key ID storing the HMAC key`.

3. Generate encrypted NVS partition.

	If you don't want to put external data in the NVS partition then you may skip this step.
	See [Generating NVS partition](https://docs.espressif.com/projects/esp-idf/en/stable/api-reference/storage/nvs_partition_gen.html#generate-encrypted-nvs-partition) for detailed information on generating the encrypted NVS partition.
	Execute following command to generate the encrypted NVS partition.

	```
	idf.py secure-encrypt-nvs-partition -k keys/nvs_encr_key.bin /* CSV placeholder */ nvs_encr_partition.bin /* NVS partition offset */
	```
	where,
	* `CSV placeholder`: CSV file which contains data of the NVS partition. See [CSV file format](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/storage/nvs_partition_gen.html#csv-file-format) for more details.
	* `NVS partition offset`: NVS partition offset. Can be found out by executing `idf.py partition-table`

4. This shall generate `nvs_encr_partition.bin` which we shall flash later at the [Flash stage](README.md#flash) of the workflow.

	<details>
	<summary>Target specific documentation</summary>

	For target specific documentation, run the following command after setting the [target](#2-set-the-target)

	```sh
	idf.py docs -sp api-reference/storage/nvs_partition_gen.html
	```
	</details>

### Enabling Secure JTAG Return Material Authorization (RMA)

The target provides an ability to disable JTAG access in the device for the software. This can be re-enabled in future after authentication using a unique token generated beforehand. This way the module can be opened up by bypassing security features after authentication for debugging purposes. This way when a secure locked device comes back to the ODM/OEM due to some issue, the module can be opened up for debug after successful authentication.

1. Generate the HMAC key

    ```shell
    python ../hmac_soft_jtag/jtag_example_helper.py generate_hmac_key secure_jtag_hmac_key.bin
    ```

    This key needs to be stored at a secure place in order to re-generate the secure token afterwards.

2. Generate the secure token

    ```shell
    python ../hmac_soft_jtag/jtag_example_helper.py generate_token secure_jtag_hmac_key.bin secure_jtag_token.bin
    ```

    The example directly consumes this token data and re-enables the software disabled JTAG interface. The re-enablement can be tested by attempting a JTAG connection with the device after JTAG is enabled by the firmware. More details about JTAG debugging can be found [here](https://docs.espressif.com/projects/esp-idf/en/stable/api-guides/jtag-debugging/index.html).
    If this is not generated, the example uses a test-only token which is already present in the example.

3. Burn the key in the eFuse

    ```shell
    idf.py --port $ESPPORT efuse-burn-key BLOCK_KEY3 secure_jtag_hmac_key.bin HMAC_DOWN_JTAG 
    ```

	<details>
	<summary>Burn JTAG key for QEMU</summary>
	For QEMU emulation, the above command can be updated as follows:

    ```shell
    idf.py qemu efuse-burn-key BLOCK_KEY3 secure_jtag_hmac_key.bin HMAC_DOWN_JTAG
    ```

	</details>

    We have used `BLOCK_KEY3` here to store the HMAC key. Generally, `BLOCK` can be a free keyblock between `BLOCK_KEY0` and `BLOCK_KEY5`.

4. Disable software access for JTAG

    ```shell
    idf.py --port $ESPPORT efuse-burn SOFT_DIS_JTAG 7
    ```

	<details>
	<summary>Disable JTAG key for QEMU</summary>
	For QEMU emulation, the above command can be updated as follows:

    ```shell
    idf.py qemu efuse-burn SOFT_DIS_JTAG 7
    ```

	</details>

    After this the JTAG cannot be accessed before it is re-enabled through software with correct token.

5. Configuring appropriate JTAG interface

    By default esp32 is set to use the [built-in JTAG interface](https://docs.espressif.com/projects/esp-idf/en/stable/api-guides/jtag-debugging/configure-builtin-jtag.html). Please follow the steps given [here](https://docs.espressif.com/projects/esp-idf/en/stable/api-guides/jtag-debugging/configure-other-jtag.html) to configure the alternative JTAG interface.
    Please note that JTAG configuration cannot be done once the application firmware is flashed on the device.

## Build

### Build the example

The example can be built with following command

```sh
idf.py build
```

### Secure Boot V2 signing

The example is configured to build the signed binaries for the  `bootloader.bin` and `security_features.bin` (application).
These shall be signed with the first secure boot key.

If you want to use multiple Secure Boot V2 signing keys for the image then please perform the step of [Signing with multiple Secure Boot V2 keys](README.md#use-multiple-secure-boot-v2-signing-keys).

### Encrypting partitions

At this point, we shall encrypt all the necessary partitions. Please perform [Encrypting the partitions](README.md#encrypting-the-partitions) step to do the same.

## Flash

At this point we shall have all the necessary partitions which are ready to flash.

The offsets at which the partitions need to be flashed can be found out by executing `idf.py partition-table`.

The partitions can be flashed with help of the `esptool` utility.

```sh
esptool.py -p $ESPPORT write_flash /* Placeholder for offset */ /* Placeholder for file name */
```

Along with these, esptool command may need some additional options.
Please check the output of `idf.py build` command executed earlier for all the necessary options that need to be provided with esptool.

For this example the following command can be used

```sh
esptool.py --chip <chip_name> -b 115200 --before default_reset --after no_reset --no-stub -p $ESPPORT write_flash 0x0 build/encrypted_data/bootloader-enc.bin 0xd000 build/encrypted_data/partition-table-enc.bin 0x20000 build/encrypted_data/security_features-enc.bin --force
```

</details>

## Monitor the output

Run the monitor tool to view serial output

```sh
idf.py -p $ESPPORT monitor
```

## Example Output

On the first boot-up, there would be prints about firmware not being secure. Please ignore the prints as we shall enable all necessary security eFuses in our application. On the Second boot onwards, you shall not see any such prints.

### ROM bootloader verifying software bootloader

```text
ESP-ROM:esp32c3-api1-20210207
Build:Feb  7 2021
rst:0x1 (POWERON),boot:0x8 (SPI_FAST_FLASH_BOOT)
SPIWP:0xee
mode:DIO, clock div:1
Valid secure boot key blocks: 0
secure boot verification succeeded
load:0x3fcd5990,len:0x3b94
load:0x403cc710,len:0xb9c
load:0x403ce710,len:0x5ba8
entry 0x403cc71a
```

#### Early logs for Secure Boot V2 and Flash Encryption

```text
I (101) esp_image: Verifying image signature...
I (106) secure_boot_v2: Verifying with RSA-PSS...
I (107) secure_boot_v2: Signature verified successfully!
I (108) boot: Loaded app from partition at offset 0x20000
I (109) secure_boot_v2: enabling secure boot v2...
I (109) secure_boot_v2: secure boot v2 is already enabled, continuing..
I (110) boot: Checking flash encryption...
I (110) flash_encrypt: flash encryption is enabled (0 plaintext flashes left)
I (111) boot: Disabling RNG early entropy source...
```

#### Flash Encryption warning on first boot (should be ignored)

```text
W (156) spi_flash: Detected size(4096k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
E (157) flash_encrypt: Flash encryption settings error: app is configured for RELEASE but efuses are set for DEVELOPMENT
E (160) flash_encrypt: Mismatch found in security options in bootloader menuconfig and efuse settings. Device is not secure.
```

#### Unused Secure Boot V2 Digests getting revoked

```text
I (162) efuse: Batch mode of writing fields is enabled
W (163) secure_boot: Unused SECURE_BOOT_DIGEST1 should be revoked. Fixing..
W (164) secure_boot: Unused SECURE_BOOT_DIGEST2 should be revoked. Fixing..
I (165) efuse: BURN BLOCK0
I (175) efuse: BURN BLOCK0 - OK (all write block bits are set)
I (177) efuse: Batch mode. Prepared fields are committed
I (177) secure_boot: Fixed
I (179) efuse: BURN BLOCK0
I (189) efuse: BURN BLOCK0 - OK (all write block bits are set)
```

#### Enablement of relevant security eFuses

```text
W (199) flash_encrypt: Not disabled UART bootloader encryption (set DIS_DOWNLOAD_MANUAL_ENCRYPT->1)
W (199) flash_encrypt: Not disabled UART bootloader cache (set DIS_DOWNLOAD_ICACHE->1)
W (199) flash_encrypt: Not disabled JTAG PADs (set DIS_PAD_JTAG->1)
W (199) flash_encrypt: Not disabled USB JTAG (set DIS_USB_JTAG->1)
W (199) flash_encrypt: Not disabled direct boot mode (set DIS_DIRECT_BOOT->1)
W (199) flash_encrypt: Not write-protected DIS_ICACHE (set WR_DIS_DIS_ICACHE->1)
I (199) flash_encrypt: Disable UART bootloader encryption...
I (199) efuse: BURN BLOCK0
I (209) efuse: BURN BLOCK0 - OK (all write block bits are set)
I (219) flash_encrypt: Disable UART bootloader cache...
I (219) efuse: BURN BLOCK0
I (229) efuse: BURN BLOCK0 - OK (all write block bits are set)
I (229) flash_encrypt: Disable JTAG...
I (229) efuse: BURN BLOCK0
I (239) efuse: BURN BLOCK0 - OK (all write block bits are set)
I (249) efuse: BURN BLOCK0
I (249) efuse: BURN BLOCK0 - OK (all write block bits are set)
I (259) efuse: BURN BLOCK0
I (269) efuse: BURN BLOCK0 - OK (all write block bits are set)
I (269) efuse: BURN BLOCK0
I (279) efuse: BURN BLOCK0 - OK (all write block bits are set)
I (289) efuse: BURN BLOCK0
I (299) efuse: BURN BLOCK0 - OK (all write block bits are set)
W (299) secure_boot: Not disabled JTAG in the soft way (set SOFT_DIS_JTAG->max)
W (299) secure_boot: Not enabled AGGRESSIVE KEY REVOKE (set SECURE_BOOT_AGGRESSIVE_REVOKE->1)
I (299) secure_boot: Enabling Security download mode...
I (299) secure_boot: Disable hardware & software JTAG...
I (299) efuse: BURN BLOCK0
I (309) efuse: BURN BLOCK0 - OK (all write block bits are set)
I (319) efuse: BURN BLOCK0
I (329) efuse: BURN BLOCK0 - OK (all write block bits are set)
I (329) secure_boot: Prevent read disabling of additional efuses...
```

#### Final status of Secure Boot V2 and Flash Encryption

```text
I (329) security_features_app: Flash Encryption is enabled in Release Mode
I (329) security_features_app: Secure Boot is enabled in Release Mode
```

#### Flash Encryption demo

```text
Erasing partition "storage" (0x1000 bytes)
Writing data with esp_partition_write:
I (339) security_features_app: 0x3fc8fa40   00 01 02 03 04 05 06 07  08 09 0a 0b 0c 0d 0e 0f  |................|
I (339) security_features_app: 0x3fc8fa50   10 11 12 13 14 15 16 17  18 19 1a 1b 1c 1d 1e 1f  |................|
Reading with esp_partition_read:
I (339) security_features_app: 0x3fc8fa60   00 01 02 03 04 05 06 07  08 09 0a 0b 0c 0d 0e 0f  |................|
I (339) security_features_app: 0x3fc8fa70   10 11 12 13 14 15 16 17  18 19 1a 1b 1c 1d 1e 1f  |................|
Reading with esp_flash_read:
I (339) security_features_app: 0x3fc8fa60   b3 e8 57 98 45 1f 33 de  71 30 71 9b 48 a2 d7 71  |..W.E.3.q0q.H..q|
I (339) security_features_app: 0x3fc8fa70   28 fd fb 54 39 fb 4f 47  be cd 7a e1 55 70 09 6d  |(..T9.OG..z.Up.m|
```

#### NVS encryption

These logs show that, the initialisation of the encrypted NVS partitions is successful.

```text
I (349) nvs: NVS partition "nvs" is encrypted.
I (359) security_features_app: NVS partition "custom_nvs" is encrypted.
```

### JTAG Status

These logs show that the JTAG is disabled

```text
I (208) security_features_app: JTAG Status: Not enabled
```

## Enable Security Features with help of QEMU

Espressif fork of [QEMU](https://github.com/espressif/qemu) offers the ability to emulate `esp32c3/esp32s3` target on the host machine with help of `QEMU`. That way all of the above security features can be enabled on the target that is emulated on the host machine. A major advantage of this is that no hardware is lost while trying out the security features.

Below are the commands that can be used to to emulate the target device on host machine.

### QEMU Configuration

1. Build the example

    Perform the [Build](README.md#build) step and all necessary substeps (e.g. encrypting partition). Please make sure the file names of newly generated files and their locations in the commands are not changed.

2. Build qemu image

    The qemu image can be built with following command

    ```sh
	idf.py merge-bin --merge-args ../qemu/qemu_flash_args -o qemu/security_features_flash_image.bin
    ```

**NOTE: The `idf.py merge-bin` command runs with `build` as the working directory. Make sure the relative path provided are relative to the `build` directory

### Run example on QEMU

The following command can be used to run example on qemu

```sh
idf.py qemu --flash-file build/qemu/security_features_flash_image.bin monitor
```

The qemu session can be closed by pressing `CTRL+ ]`.
