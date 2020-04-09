| Supported Targets | ESP32 |
| ----------------- | ----- |

# Secure Boot

The example checks if the secure boot feature is enabled/disabled and if enabled prints the secure boot version (Version 1 / Version 2) and FLASH_CRYPT_CNT eFuse value.

## How to use example

### Hardware Required

ESP32 (supports Secure Boot V1) or ESP32-ECO3 (supports  Secure Boot V2 & Secure Boot V1). It is recommended to use Secure Boot V2 from ESP32-ECO3 onwards.

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

