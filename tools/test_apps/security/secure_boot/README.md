| Supported Targets | ESP32 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# Secure Boot

The example checks if the secure boot feature is enabled/disabled and if enabled prints the secure boot version (Version 1 / Version 2) and FLASH_CRYPT_CNT eFuse value.

## How to use example

### Hardware Required

Any of the following ESP module:
* ESP32 (supports Secure Boot V1)
* ESP32-ECO3 (supports  Secure Boot V2 & Secure Boot V1)
* ESP32S2 (supports Secure Boot V2)
* ESP32C3-ECO3 (supports Secure Boot V2)
* ESP32S3 (supports Secure Boot V2)
* ESP32P4 (supports Secure Boot V2)

It is recommended to use Secure Boot V2 from ESP32-ECO3 onwards.

### Configure the project

```
idf.py menuconfig
```

* Set serial port under Serial Flasher Options.

* Enable the `Enable hardware Secure Boot` under Security Features. Default version for ESP32 is Secure Boot V1. The chip revision should be changed to revision 3(ESP32- ECO3) to view the Secure Boot V2 option.

* To change the chip revision, set "Component Config" -> "ESP32- Specific"->"Minimum Supported ESP32 Revision" to Rev 3. Now, set Secure Boot V2 option can now be enabled under "Enable hardware Secure Boot in bootloader" -> "Secure Boot Version".

* Specify the apt secure boot signing key path. If not present generate one using `python $IDF_PATH/components/esptool_py/esptool/espsecure.py generate_signing_key --version {VERSION} secure_boot_signing_key.pem`

* Ensure Bootloader log verbosity is Info under Bootloader config.

* Select Single factory app, no OTA under Partition Table options. Change the partition table offset to 0xb000 from 0x8000 since after enabling secure boot the size of bootloader is increased.

### Build and Flash

-  The below steps can be used in any application to enable secure boot. 

- Secure Boot is an irreversible operation, please read the Secure Boot section in ESP-IDF Programming Manual.

- Secure boot will be enabled on building the project after enabling hardware secure boot feature in _Security features_ in menuconfig and flashing it to the board FOR THE FIRST TIME.

- The bootloader will not be automatically flashed when secure boot is enabled. Running `idf.py bootloader` will print a command to flash the bootloader on the ESP32. Run this command manually to flash the bootloader.

Run following command to program the partition table and application on the ESP32 and monitor the output:
```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

- On subsequent boots, the secure boot hardware will verify the software bootloader has not changed and the software bootloader will verify the signed app image (using the validated public key portion of its appended signature block).

## Example Output

## Troubleshooting

---

# Secure Boot tests (For internal use only)

Purpose of the test case (`pytest_secure_boot.py`) is to test the secure boot implementation and detect if it is broken. It consists of positive and negative test cases.

### Hardware required

* FPGA setup with ESP32C3/ESP32S3/ESP32P4 image

* COM port for programming and export it as ESPPORT
    e.g `export ESPPORT=/dev/ttyUSB0`

* Use another COM port for resetting efuses and connect its DTR pin to efuse reset pin on the FPGA board. Export it as EFUSEPORT
    e.g `export EFUSEPORT=/dev/ttyUSB1`

### Configure the project

```
export IDF_ENV_FPGA=1

idf.py set-target esp32c3   #(or esp32s3 / esp32p4)

idf.py menuconfig
```

Under `Security features`

- Enable the `Enable hardware Secure Boot`

- Set the secure boot signing key ("test_rsa_3072_key.pem")

- Set UART ROM download mode to ENABLED (Required for the script to read the EFUSE)

- Install pytest requirements

    ```
    bash $IDF_PATH/install.sh --enable-pytest
    ```

### Build and test

- Build the example

    ```
    idf.py build
    ```

- Run the example test

    ```
    pytest --target esp32c3
    ```
