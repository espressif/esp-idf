:orphan:

.. _secure-boot-bootloader-size:

Bootloader Size
===============

{IDF_TARGET_DEFAULT_MAX_BOOTLOADER_SIZE:default = "0x8000 (32768)", esp32 = "0x7000 (28672)", esp32s2 = "0x7000 (28672)"}
{IDF_TARGET_MAX_BOOTLOADER_SIZE:default = "64KB (0x10000 bytes)", esp32 = "48KB (0xC000 bytes)"}
{IDF_TARGET_MAX_PARTITION_TABLE_OFFSET:default = "0x12000", esp32 = "0xE000"}
.. Above is calculated as 0x1000 at start of flash + IDF_TARGET_MAX_BOOTLOADER_SIZE + 0x1000 signature sector

When secure boot is enabled the bootloader app binary ``bootloader.bin`` may exceed the bootloader binary size limit and overlap with the partition table. This is especially likely if flash encryption is enabled as well. When using the default :ref:`CONFIG_PARTITION_TABLE_OFFSET` value 0x8000, the size limit is {IDF_TARGET_DEFAULT_MAX_BOOTLOADER_SIZE} bytes.

If the bootloader binary is too large, then the bootloader build will fail with an error "Bootloader binary size [..] is too large for partition table offset". If the bootloader binary is flashed anyhow then the {IDF_TARGET_NAME} will fail to boot - errors will be logged about either invalid partition table or invalid bootloader checksum.

.. note::

   The bootloader size check only happens in the CMake Build System, when using the legacy GNU Make Build System the size is not checked but the {IDF_TARGET_NAME} will fail to boot if bootloader is too large.

When Secure Boot V2 is enabled, there is also an absolute binary size limit of {IDF_TARGET_MAX_BOOTLOADER_SIZE} (excluding the 4KB signature), because the bootloader is first loaded into a fixed size buffer for verification.

Options to work around this are:

- Set :ref:`bootloader compiler optimization <CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION>` back to "Size" if it has been changed from this default value.
- Reduce :ref:`bootloader log level <CONFIG_BOOTLOADER_LOG_LEVEL>`. Setting log level to Warning, Error or None all significantly reduce the final binary size (but may make it harder to debug).
- Set :ref:`CONFIG_PARTITION_TABLE_OFFSET` to a higher value than 0x8000, to place the partition table later in the flash. This increases the space available for the bootloader. If the :doc:`partition table </api-guides/partition-tables>` CSV file contains explicit partition offsets, they will need changing so no partition has an offset lower than ``CONFIG_PARTITION_TABLE_OFFSET + 0x1000``. (This includes the default partition CSV files supplied with ESP-IDF.)

Note that because of the absolute binary size limit, when using Secure Boot V2 there is no benefit to moving the partition table any higher than offset {IDF_TARGET_MAX_PARTITION_TABLE_OFFSET}.
