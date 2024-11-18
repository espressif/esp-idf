# Security Examples

## Security Features App

This example demonstrates how to enable most of the security features on the ESP32 series SoC with the help of host-based tools such as (idf.py, esptool, espsecure). The example also supports the use of QEMU emulated ESP32-C3 for testing the security features without the need for physical hardware.
Note: Currently, this example is only supported on the ESP32-C3 SoC.
The example demonstrates the following security features:
* Secure Boot V2
* Flash Encryption
* HMAC-based NVS Encryption
* Secure JTAG Return Material Access

## Flash Encryption

The Flash Encryption example for the ESP32 series SoC demonstrates how to enable and use flash encryption in Development and Release modes.
It includes:
* Checking the status of flash encryption and relevant eFuses.
* Writing to and reading from encrypted flash partitions.
* Initializing FATFS and NVS partitions, both encrypted and non-encrypted.
* Flashing the example in different modes, with guidance on handling encrypted data.

## NVS Encryption - HMAC

This example demonstrates how to use NVS (Non-Volatile Storage) encryption with an HMAC-based encryption key protection scheme on Espressif SoC chips. The encryption keys are derived from an HMAC key stored in eFuse, ensuring they are not stored in flash memory. This approach eliminates the need for a separate nvs_keys partition and does not require flash encryption to be enabled.
The steps include:
* Generating a secure HMAC key using the host tool.
* Burning the HMAC key into the eFuse.
* Enabling the HMAC-based NVS encryption scheme.
* Reading and writing to the encrypted NVS partition.

## HMAC Soft JTAG

The "JTAG Re-enable Example" demonstrates how to use the HMAC peripheral to securely re-enable a software-disabled JTAG interface after authentication on supported ESP32 Series SoCs. This is useful for implementing the Return Material Access (RMA) feature for a device.
The steps include:
* Generating a 32-byte HMAC key.
* Burning the key to an eFuse.
* Generating token data from the key.
* Using commands to re-enable JTAG access.

