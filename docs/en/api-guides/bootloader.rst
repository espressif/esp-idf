Bootloader
=====================
:link_to_translation:`zh_CN:[中文]`

Bootloader performs the following functions:

1. Minimal initial configuration of internal modules;
2. Select the application partition to boot, based on the partition table and ota_data (if any);
3. Load this image to RAM (IRAM & DRAM) and transfer management to it.

Bootloader is located at the address `0x1000` in the flash.

Bootloader compatibility
-------------------------

It is recommended to update to newer :doc:`versions of ESP-IDF </versions>`: when they are released. The OTA (over the air) update process can flash new apps in the field but cannot flash a new bootloader. For this reason, the bootloader supports booting apps built from newer versions of ESP-IDF.

The bootloader does not support booting apps from older versions of ESP-IDF. When updating ESP-IDF manually on an existing product that might need to downgrade the app to an older version, keep using the older ESP-IDF bootloader binary as well.

.. note::

   If testing an OTA update for an existing product in production, always test it using the same ESP-IDF bootloader binary that is deployed in production.

.. only:: esp32

    Before ESP-IDF V2.1
    ^^^^^^^^^^^^^^^^^^^

    Bootloaders built from very old versions of ESP-IDF (before ESP-IDF V2.1) perform less hardware configuration than newer versions. When using a bootloader from these early ESP-IDF versions and building a new app, enable the config option :ref:`CONFIG_ESP32_COMPATIBLE_PRE_V2_1_BOOTLOADERS`.

    Before ESP-IDF V3.1
    ^^^^^^^^^^^^^^^^^^^

    Bootloaders built from versions of ESP-IDF before V3.1 do not support MD5 checksums in the partition table binary. When using a bootloader from these ESP-IDF versions and building a new app, enable the config option :ref:`CONFIG_ESP32_COMPATIBLE_PRE_V3_1_BOOTLOADERS`.

SPI Flash Configuration
^^^^^^^^^^^^^^^^^^^^^^^

Each ESP-IDF application or bootloader .bin file contains a header with :ref:`CONFIG_ESPTOOLPY_FLASHMODE`, :ref:`CONFIG_ESPTOOLPY_FLASHFREQ`, :ref:`CONFIG_ESPTOOLPY_FLASHSIZE` embedded in it. These are used to configure the SPI flash during boot.

The :first stage bootloader in ROM reads the second stage bootloader header from flash and uses these settings to load it. However, at this time the system clock speed is lower than configured and not all flash modes are supported.  When the second stage bootloader then runs and re-configures the flash, it reads values from the currently selected app binary header not the bootloader header. This allows an OTA update to change the SPI flash settings in use.

.. only:: esp32

   Bootloaders prior to ESP-IDF V4.0 used the bootloader's own header to configure the SPI flash, meaning these values could not be changed in an update. To maintain compatibility with older bootloaders, the app re-initializes the flash settings during app startup using the configuration found in the app header.

FACTORY reset
---------------------------
The user can write a basic working firmware and load it into the factory partition. 
Next, update the firmware via OTA (over the air). The updated firmware will be loaded into an OTA app partition slot and the OTA data partition is updated to boot from this partition. 
If you want to be able to roll back to the factory firmware and clear the settings, then you need to set :ref:`CONFIG_BOOTLOADER_FACTORY_RESET`.
The factory reset mechanism allows to reset the device to factory settings:

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

Boot from TEST firmware
------------------------
The user can write a special firmware for testing in production, and run it as needed. The partition table also needs a dedicated partition for this testing firmware (See `partition table`). 
To trigger a test app you need to set :ref:`CONFIG_BOOTLOADER_APP_TEST`. 

:ref:`CONFIG_BOOTLOADER_NUM_PIN_APP_TEST` - GPIO number to boot TEST partition. The selected GPIO will be configured as an input with internal pull-up enabled. To trigger a test app, this GPIO must be pulled low on reset. 
After the GPIO input is deactivated and the device reboots, the normally configured application will boot (factory or any OTA slot). 

:ref:`CONFIG_BOOTLOADER_HOLD_TIME_GPIO` - this is hold time of GPIO for reset/test mode (by default 5 seconds). The GPIO must be held low continuously for this period of time after reset before a factory reset or test partition boot (as applicable) is performed.

Fast boot from Deep Sleep
-------------------------
The bootloader has the :ref:`CONFIG_BOOTLOADER_SKIP_VALIDATE_IN_DEEP_SLEEP` option which allows to reduce the wake-up time (useful to reduce consumption). This option is available when the :ref:`CONFIG_SECURE_BOOT` option is disabled. Reduction of time is achieved due to the lack of image verification. During the first boot, the bootloader stores the address of the application being launched in the RTC FAST memory. And during the awakening, this address is used for booting without any checks, thus fast loading is achieved.

Customer bootloader
---------------------
The current bootloader implementation allows the customer to override it. To do this, you must copy the folder `/esp-idf/components/bootloader` and then edit `/your_project/components/bootloader/subproject/main/bootloader_main.c`.
In the bootloader space, you can not use the drivers and functions from other components. If necessary, then the required functionality should be placed in the folder bootloader (note that this will increase its size).
It is necessary to monitor its size because there can be overlays in memory with a partition table leading to damage. At the moment the bootloader is limited to the partition table from the address `0x8000`.

