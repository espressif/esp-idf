Wear Levelling APIs
===================

Overview
--------
Most of the flash devices and specially SPI flash devices that are used in ESP32
have sector based organization and have limited amount of erase/modification cycles
per memory sector. To avoid situation when one sector reach the limit of erases when
other sectors was used not often, we have made a component that avoid this situation.
The wear levelling component share the amount of erases between all sectors in the
memory without user interaction.
The wear_levelling component contains APIs related to reading, writing, erasing,
memory mapping data in the external SPI flash through the partition component. It
also has higher-level APIs which work with FAT filesystem defined in
the :doc:`FAT filesystem </api-reference/storage/fatfs>`.

The wear levelling component, together with FAT FS component, works with FAT FS sector size 4096
bytes which is standard size of the flash devices. In this mode the component has best performance,
but needs additional memoty in the RAM. To save internal memory the component has two additional modes
to work with sector size 512 bytes: Performance and Safety modes. In Performance mode by erase sector
operation data will be stored to the RAM, sector will be erased and then data will be stored
back to the flash. If by this operation power off situation will occur, the complete 4096 bytes
will be lost. To prevent this the Safety mode was implemented. In safety mode the data will be first
stored to the flash and after sector will be erased, will be stored back. If power off situation will
occur, after power on, the data will be recovered.
By default defined the sector size 512 bytes and Performance mode. To change these values please use
the configuration menu.


The wear levelling component does not cache data in RAM. Write and erase functions
modify flash directly, and flash contents is consistent when the function returns.


Wear Levelling access APIs
--------------------------

This is the set of APIs for working with data in flash:

- ``wl_mount`` mount wear levelling module for defined partition
- ``wl_unmount`` used to unmount levelling module
- ``wl_erase_range`` used to erase range of addresses in flash
- ``wl_write`` used to write data to the partition
- ``wl_read`` used to read data from the partition
- ``wl_size`` return size of avalible memory in bytes
- ``wl_sector_size`` returns size of one sector

Generally, try to avoid using the raw wear levelling functions in favor of
filesystem-specific functions.

Memory Size
-----------

The memory size calculated in the wear Levelling module based on parameters of
partition. The module use few sectors of flash for internal data.

