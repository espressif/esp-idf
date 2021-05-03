Bootloader
=====================
:link_to_translation:`zh_CN:[中文]`

{IDF_TARGET_BOOTLOADER_OFFSET:default="0x0", esp32="0x1000", esp32s2="0x1000"}

The ESP-IDF Software Bootloader performs the following functions:

1. Minimal initial configuration of internal modules;
2. Initialize :doc:`/security/flash-encryption` and/or :doc:`Secure </security/secure-boot-v2>` features, if configured;
3. Select the application partition to boot, based on the partition table and ota_data (if any);
4. Load this image to RAM (IRAM & DRAM) and transfer management to it.

Bootloader is located at the address {IDF_TARGET_BOOTLOADER_OFFSET} in the flash.

For a full description of the startup process including the the ESP-IDF bootloader, see :doc:`startup`.

Bootloader compatibility
------------------------

It is recommended to update to newer :doc:`versions of ESP-IDF </versions>`: when they are released. The OTA (over the air) update process can flash new apps in the field but cannot flash a new bootloader. For this reason, the bootloader supports booting apps built from newer versions of ESP-IDF.

The bootloader does not support booting apps from older versions of ESP-IDF. When updating ESP-IDF manually on an existing product that might need to downgrade the app to an older version, keep using the older ESP-IDF bootloader binary as well.

Factory reset
-------------
The user can write a basic working firmware and load it into the factory app partition.

Next, update the firmware via OTA (over the air). The updated firmware will be loaded into an OTA app partition slot and the OTA data partition is updated to boot from this partition.

If you want to be able to roll back to the factory firmware and clear the settings, then you need to set :ref:`CONFIG_BOOTLOADER_FACTORY_RESET`. The factory reset mechanism allows to reset the device to factory settings:

- Clear one or more data partitions.
- Boot from "factory" partition.

:ref:`CONFIG_BOOTLOADER_DATA_FACTORY_RESET` allows customers to select which data partitions will be erased when the factory reset is executed. 
Can specify the names of partitions through comma-delimited with optional spaces for readability. (Like this: "nvs, phy_init, nvs_custom, ..."). 
Make sure that the name specified in the partition table and here are the same. 
Partitions of type "app" cannot be specified here.

:ref:`CONFIG_BOOTLOADER_OTA_DATA_ERASE` - the device will boot from "factory" partition after a factory reset. The OTA data partition will be cleared.

:ref:`CONFIG_BOOTLOADER_NUM_PIN_FACTORY_RESET`- number of the GPIO input for factory reset uses to trigger a factory reset, this GPIO must be pulled low on reset to trigger this. 

:ref:`CONFIG_BOOTLOADER_HOLD_TIME_GPIO`- this is hold time of GPIO for reset/test mode (by default 5 seconds). The GPIO must be held low continuously for this period of time after reset before a factory reset or test partition boot (as applicable) is performed.

Partition table.::

	# Name,   Type, SubType, Offset,   Size, Flags
	# Note: if you have increased the bootloader size, make sure to update the offsets to avoid overlap
	nvs,      data, nvs,     0x9000,   0x4000
	otadata,  data, ota,     0xd000,   0x2000
	phy_init, data, phy,     0xf000,   0x1000
	factory,  0,    0,       0x10000,  1M
	test,     0,    test,    ,         512K
	ota_0,    0,    ota_0,   ,         512K
	ota_1,    0,    ota_1,   ,         512K

.. _bootloader_boot_from_test_firmware:

Boot from test app partition
----------------------------
The user can write a special firmware for testing in production, and run it as needed. The partition table also needs a dedicated partition for this testing firmware (See `partition table`). 
To trigger a test app you need to set :ref:`CONFIG_BOOTLOADER_APP_TEST`. 

:ref:`CONFIG_BOOTLOADER_NUM_PIN_APP_TEST` - GPIO number to boot TEST partition. The selected GPIO will be configured as an input with internal pull-up enabled. To trigger a test app, this GPIO must be pulled low on reset. 
After the GPIO input is deactivated and the device reboots, the normally configured application will boot (factory or any OTA slot). 

:ref:`CONFIG_BOOTLOADER_HOLD_TIME_GPIO` - this is hold time of GPIO for reset/test mode (by default 5 seconds). The GPIO must be held low continuously for this period of time after reset before a factory reset or test partition boot (as applicable) is performed.

Fast boot from Deep Sleep
-------------------------
The bootloader has the :ref:`CONFIG_BOOTLOADER_SKIP_VALIDATE_IN_DEEP_SLEEP` option which allows to reduce the wake-up time (useful to reduce consumption). This option is available when the :ref:`CONFIG_SECURE_BOOT` option is disabled. Reduction of time is achieved due to the lack of image verification. During the first boot, the bootloader stores the address of the application being launched in the RTC FAST memory. And during the awakening, this address is used for booting without any checks, thus fast loading is achieved.

Custom bootloader
-----------------

The current bootloader implementation allows a project to override it. To do this, you must copy the directory ``/esp-idf/components/bootloader`` to your project components directory and then edit ``/your_project/components/bootloader/subproject/main/bootloader_start.c``.

In the bootloader space, you cannot use the drivers and functions from other components. If necessary, then the required functionality should be placed in the project's ``bootloader`` directory (note that this will increase its size).

If the bootloader grows too large then it can collide with the partition table, which is flashed at offset 0x8000 by default. Increase the :ref:`partition table offset <CONFIG_PARTITION_TABLE_OFFSET>` value to place the partition table later in the flash. This increases the space available for the bootloader.

.. note:: The first time you copy the bootloader into an existing project, the project may fail to build as paths have changed unexpectedly. If this happens, run ``idf.py fullclean`` (or delete the project build directory) and then build again.
