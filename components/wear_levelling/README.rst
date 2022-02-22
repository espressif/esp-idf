Wear Levelling API
==================

:link_to_translation:`zh_CN:[中文]`

Overview
--------

Most of flash memory and especially SPI flash that is used in {IDF_TARGET_NAME} has a sector-based organization and also has a limited number of erase/modification cycles per memory sector. The wear levelling component helps to distribute wear and tear among sectors more evenly without requiring any attention from the user.

The wear levelling component provides API functions related to reading, writing, erasing, and memory mapping of data in external SPI flash through the partition component. The component also has higher-level API functions which work with the FAT filesystem defined in :doc:`FAT filesystem </api-reference/storage/fatfs>`.

The wear levelling component, together with the FAT FS component, uses FAT FS sectors of 4096 bytes, which is a standard size for flash memory. With this size, the component shows the best performance but needs additional memory in RAM.

To save internal memory, the component has two additional modes which both use sectors of 512 bytes:

- **Performance mode.** Erase sector operation data is stored in RAM, the sector is erased, and then data is copied back to flash memory. However, if a device is powered off for any reason, all 4096 bytes of data is lost.
- **Safety mode.** The data is first saved to flash memory, and after the sector is erased, the data is saved back. If a device is powered off, the data can be recovered as soon as the device boots up.

The default settings are as follows:

- Sector size is 512 bytes
- Performance mode

You can change the settings through the configuration menu.

The wear levelling component does not cache data in RAM. The write and erase functions modify flash directly, and flash contents are consistent when the function returns.


Wear Levelling access API functions
-----------------------------------

This is the set of API functions for working with data in flash:

- ``wl_mount`` - initializes the wear levelling module and mounts the specified partition
- ``wl_unmount`` - unmounts the partition and deinitializes the wear levelling module
- ``wl_erase_range`` - erases a range of addresses in flash
- ``wl_write`` - writes data to a partition
- ``wl_read`` - reads data from a partition
- ``wl_size`` - returns the size of available memory in bytes
- ``wl_sector_size`` - returns the size of one sector

As a rule, try to avoid using raw wear levelling functions and use filesystem-specific functions instead.


Memory Size
-----------

The memory size is calculated in the wear levelling module based on partition parameters. The module uses some sectors of flash for internal data.
