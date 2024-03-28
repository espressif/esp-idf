Bootloader
==========

:link_to_translation:`zh_CN:[中文]`

The ESP-IDF Software Bootloader performs the following functions:

1. Minimal initial configuration of internal modules;
2. Initialize :doc:`/security/flash-encryption` and/or :doc:`Secure </security/secure-boot-v2>` features, if configured;
3. Select the application partition to boot, based on the partition table and ota_data (if any);
4. Load this image to RAM (IRAM & DRAM) and transfer management to the image that was just loaded.

Bootloader is located at the address {IDF_TARGET_CONFIG_BOOTLOADER_OFFSET_IN_FLASH} in the flash.

For a full description of the startup process including the ESP-IDF bootloader, see :doc:`startup`.

.. _bootloader-compatibility:

Bootloader Compatibility
------------------------

It is recommended to update to newer :doc:`versions of ESP-IDF </versions>`: when they are released. The OTA (over the air) update process can flash new apps in the field but cannot flash a new bootloader. For this reason, the bootloader supports booting apps built from newer versions of ESP-IDF.

The bootloader does not support booting apps from older versions of ESP-IDF. When updating ESP-IDF manually on an existing product that might need to downgrade the app to an older version, keep using the older ESP-IDF bootloader binary as well.

.. note::

   If testing an OTA update for an existing product in production, always test it using the same ESP-IDF bootloader binary that is deployed in production.

.. only:: esp32

    Before ESP-IDF V2.1
    ^^^^^^^^^^^^^^^^^^^

    Bootloaders built from very old versions of ESP-IDF (before ESP-IDF V2.1) perform less hardware configuration than newer versions. When using a bootloader from these early ESP-IDF versions and building a new app, enable the config option :ref:`CONFIG_APP_COMPATIBLE_PRE_V2_1_BOOTLOADERS`.

    Before ESP-IDF V3.1
    ^^^^^^^^^^^^^^^^^^^

    Bootloaders built from versions of ESP-IDF before V3.1 do not support MD5 checksums in the partition table binary. When using a bootloader from these ESP-IDF versions and building a new app, enable the config option :ref:`CONFIG_APP_COMPATIBLE_PRE_V3_1_BOOTLOADERS`.

    Before ESP-IDF V5.1
    ^^^^^^^^^^^^^^^^^^^

    Bootloaders built from versions of ESP-IDF prior to V5.1 do not support :ref:`CONFIG_ESP_SYSTEM_ESP32_SRAM1_REGION_AS_IRAM`. When using a bootloader from these ESP-IDF versions and building a new app, you should not use this option.


SPI Flash Configuration
^^^^^^^^^^^^^^^^^^^^^^^

Each ESP-IDF application or bootloader .bin file contains a header with :ref:`CONFIG_ESPTOOLPY_FLASHMODE`, :ref:`CONFIG_ESPTOOLPY_FLASHFREQ`, :ref:`CONFIG_ESPTOOLPY_FLASHSIZE` embedded in it. These are used to configure the SPI flash during boot.

The :ref:`first-stage-bootloader` in ROM reads the :ref:`second-stage-bootloader` header information from flash and uses this information to load the rest of the :ref:`second-stage-bootloader` from flash. However, at this time the system clock speed is lower than configured and not all flash modes are supported. When the :ref:`second-stage-bootloader` then runs, it will reconfigure the flash using values read from the currently selected app binary's header (and NOT from the :ref:`second-stage-bootloader` header). This allows an OTA update to change the SPI flash settings in use.

.. only:: esp32

   Bootloaders prior to ESP-IDF V4.0 used the bootloader's own header to configure the SPI flash, meaning these values could not be changed in an update. To maintain compatibility with older bootloaders, the app re-initializes the flash settings during app startup using the configuration found in the app header.

Log Level
---------

The default bootloader log level is "Info". By setting the :ref:`CONFIG_BOOTLOADER_LOG_LEVEL` option, it is possible to increase or decrease this level. This log level is separate from the log level used in the app (see :doc:`/api-reference/system/log`).

Reducing bootloader log verbosity can improve the overall project boot time by a small amount.

Factory Reset
-------------

Sometimes it is desirable to have a way for the device to fall back to a known-good state, in case of some problem with an update.

To roll back to the original "factory" device configuration and clear any user settings, configure the config item :ref:`CONFIG_BOOTLOADER_FACTORY_RESET` in the bootloader.

The factory reset mechanism allows the device to be factory reset in two ways:

- Clear one or more data partitions. The :ref:`CONFIG_BOOTLOADER_DATA_FACTORY_RESET` option allows users to specify which data partitions will be erased when the factory reset is executed.

  Users can specify the names of partitions as a comma-delimited list with optional spaces for readability. (Like this: ``nvs, phy_init, nvs_custom``).

  Make sure that the names of partitions specified in the option are the same as those found in the partition table. Partitions of type "app" cannot be specified here.

- Boot from "factory" app partition. Enabling the :ref:`CONFIG_BOOTLOADER_OTA_DATA_ERASE` option will cause the device to boot from the default "factory" app partition after a factory reset (or if there is no factory app partition in the partition table then the default ota app partition is selected instead). This reset process involves erasing the OTA data partition which holds the currently selected OTA partition slot. The "factory" app partition slot (if it exists) is never updated via OTA, so resetting to this allows reverting to a "known good" firmware application.

Either or both of these configuration options can be enabled independently.

In addition, the following configuration options control the reset condition:

- :ref:`CONFIG_BOOTLOADER_NUM_PIN_FACTORY_RESET`- The input GPIO number used to trigger a factory reset. This GPIO must be pulled low or high (configurable) on reset to trigger this.

- :ref:`CONFIG_BOOTLOADER_HOLD_TIME_GPIO`- this is hold time of GPIO for reset/test mode (by default 5 seconds). The GPIO must be held continuously for this period of time after reset before a factory reset or test partition boot (as applicable) is performed.

- :ref:`CONFIG_BOOTLOADER_FACTORY_RESET_PIN_LEVEL` - configure whether a factory reset should trigger on a high or low level of the GPIO. If the GPIO has an internal pullup then this is enabled before the pin is sampled, consult the {IDF_TARGET_NAME} datasheet for details on pin internal pullups.

.. only:: SOC_RTC_FAST_MEM_SUPPORTED

    If an application needs to know if the factory reset has occurred, users can call the function :cpp:func:`bootloader_common_get_rtc_retain_mem_factory_reset_state`.

    - If the status is read as true, the function will return the status, indicating that the factory reset has occurred. The function then resets the status to false for subsequent factory reset judgement.
    - If the status is read as false, the function will return the status, indicating that the factory reset has not occurred, or the memory where this status is stored is invalid.

    Note that this feature reserves some RTC FAST memory (the same size as the :ref:`CONFIG_BOOTLOADER_SKIP_VALIDATE_IN_DEEP_SLEEP` feature).

.. only:: not SOC_RTC_FAST_MEM_SUPPORTED

    Sometimes an application needs to know if the factory reset has occurred. The {IDF_TARGET_NAME} chip does not have RTC FAST memory, so there is no API to detect it. Instead, there is a workaround: you need an NVS partition that will be erased by the bootloader if factory reset occurs (add this partition to :ref:`CONFIG_BOOTLOADER_DATA_FACTORY_RESET`). In this NVS partition, create a "factory_reset_state" token that will be increased in the application. If the "factory_reset_state" is 0 then the factory reset has occurred.

.. _bootloader_boot_from_test_firmware:

Boot from Test Firmware
------------------------

It is possible to write a special firmware app for testing in production, and boot this firmware when needed. The project partition table will need a dedicated app partition entry for this testing app, type ``app`` and subtype ``test`` (see :doc:`/api-guides/partition-tables`).

Implementing a dedicated test app firmware requires creating a totally separate ESP-IDF project for the test app (each project in ESP-IDF only builds one app). The test app can be developed and tested independently of the main project, and then integrated at production testing time as a pre-compiled .bin file which is flashed to the address of the main project's test app partition.

To support this functionality in the main project's bootloader, set the configuration item :ref:`CONFIG_BOOTLOADER_APP_TEST` and configure the following three items:

- :ref:`CONFIG_BOOTLOADER_NUM_PIN_APP_TEST` - GPIO number to boot test partition. The selected GPIO will be configured as an input with internal pull-up enabled. This GPIO must be pulled low or high (configurable) on reset to trigger this.

  Once the GPIO input is released and the device has been rebooted, the default boot sequence will be enabled again to boot the factory partition or any OTA app partition slot.

- :ref:`CONFIG_BOOTLOADER_HOLD_TIME_GPIO` - this is the hold time of GPIO for reset/test mode (by default 5 seconds). The GPIO must be held continuously for this period of time after reset before a factory reset or test partition boot (as applicable) is performed.

- :ref:`CONFIG_BOOTLOADER_APP_TEST_PIN_LEVEL` - configure whether a test partition boot should trigger on a high or low level of the GPIO. If the GPIO has an internal pull-up, then this is enabled before the pin is sampled. Consult the {IDF_TARGET_NAME} datasheet for details on pin internal pull-ups.

Rollback
--------

Rollback and anti-rollback features must be configured in the bootloader as well.

Consult the :ref:`app_rollback` and :ref:`anti-rollback` sections in the :doc:`OTA API reference document </api-reference/system/ota>`.

.. _bootloader-watchdog:

Watchdog
--------

The chips come equipped with two groups of watchdog timers: Main System Watchdog Timer (MWDT_WDT) and RTC Watchdog Timer (RTC_WDT). Both watchdog timer groups are enabled when the chip is powered up. However, in the bootloader, they will both be disabled. If :ref:`CONFIG_BOOTLOADER_WDT_ENABLE` is set (which is the default behavior), RTC_WDT is re-enabled. It tracks the time from the bootloader is enabled until the user's main function is called. In this scenario, RTC_WDT remains operational and will automatically reset the chip if no application successfully starts within 9 seconds. This functionality is particularly useful in preventing lockups caused by an unstable power source during startup.

- The timeout period can be adjusted by setting :ref:`CONFIG_BOOTLOADER_WDT_TIME_MS` and recompiling the bootloader.
- The RTC Watchdog can be disabled in the bootloader by disabling the :ref:`CONFIG_BOOTLOADER_WDT_ENABLE` setting and recompiling the bootloader. This is not recommended.
- See :ref:`app-hardware-watchdog-timers` to learn how RTC_WDT is used in the application.

.. _bootloader-size:

Bootloader Size
---------------

{IDF_TARGET_MAX_BOOTLOADER_SIZE:default = "64 KB (0x10000 bytes)", esp32 = "48 KB (0xC000 bytes)"}
{IDF_TARGET_MAX_PARTITION_TABLE_OFFSET:default = "0x12000", esp32 = "0xE000"}
.. Above is calculated as 0x1000 at start of flash + IDF_TARGET_MAX_BOOTLOADER_SIZE + 0x1000 signature sector

When enabling additional bootloader functions, including :doc:`/security/flash-encryption` or Secure Boot, and especially if setting a high :ref:`CONFIG_BOOTLOADER_LOG_LEVEL` level, then it is important to monitor the bootloader .bin file's size.

When using the default :ref:`CONFIG_PARTITION_TABLE_OFFSET` value 0x8000, the size limit is {IDF_TARGET_CONFIG_PARTITION_TABLE_OFFSET} bytes.

If the bootloader binary is too large, then the bootloader build will fail with an error "Bootloader binary size [..] is too large for partition table offset". If the bootloader binary is flashed anyhow then the {IDF_TARGET_NAME} will fail to boot - errors will be logged about either invalid partition table or invalid bootloader checksum.

Options to work around this are:

- Set :ref:`bootloader compiler optimization <CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION>` back to "Size" if it has been changed from this default value.
- Reduce :ref:`bootloader log level <CONFIG_BOOTLOADER_LOG_LEVEL>`. Setting log level to Warning, Error or None all significantly reduce the final binary size (but may make it harder to debug).
- Set :ref:`CONFIG_PARTITION_TABLE_OFFSET` to a higher value than 0x8000, to place the partition table later in the flash. This increases the space available for the bootloader. If the :doc:`partition table </api-guides/partition-tables>` CSV file contains explicit partition offsets, they will need changing so no partition has an offset lower than ``CONFIG_PARTITION_TABLE_OFFSET + 0x1000``. (This includes the default partition CSV files supplied with ESP-IDF.)

When Secure Boot V2 is enabled, there is also an absolute binary size limit of {IDF_TARGET_MAX_BOOTLOADER_SIZE} (excluding the 4 KB signature), because the bootloader is first loaded into a fixed size buffer for verification.

.. only:: SOC_RTC_FAST_MEM_SUPPORTED

    Fast Boot from Deep-Sleep
    -------------------------

    The bootloader has the :ref:`CONFIG_BOOTLOADER_SKIP_VALIDATE_IN_DEEP_SLEEP` option which allows the wake-up time from Deep-sleep to be reduced (useful for reducing power consumption). This option is available when :ref:`CONFIG_SECURE_BOOT` option is disabled. Reduction of time is achieved due to the lack of image verification. During the first boot, the bootloader stores the address of the application being launched in the RTC FAST memory. And during the awakening, this address is used for booting without any checks, thus fast loading is achieved.

Custom Bootloader
-----------------

The current bootloader implementation allows a project to extend it or modify it. There are two ways of doing it: by implementing hooks or by overriding it. Both ways are presented in :example:`custom_bootloader` folder in ESP-IDF examples:

* `bootloader_hooks` which presents how to connect some hooks to the bootloader initialization
* `bootloader_override` which presents how to override the bootloader implementation

In the bootloader space, you cannot use the drivers and functions from other components. If necessary, then the required functionality should be placed in the project's `bootloader_components` directory (note that this will increase its size).

If the bootloader grows too large then it can collide with the partition table, which is flashed at offset 0x8000 by default. Increase the :ref:`partition table offset <CONFIG_PARTITION_TABLE_OFFSET>` value to place the partition table later in the flash. This increases the space available for the bootloader.
