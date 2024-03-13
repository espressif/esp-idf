Optional features for flash
===========================

Some features are not supported on all ESP chips and Flash chips. You can check the list below for more information.

-  `Auto Suspend & Resume <#auto-suspend-&-resume>`__

-  `Flash unique ID <#flash-unique-id>`__

-  `High performance mode <#high-performance-mode>`__

-  `OPI flash support <#opi-flash-support>`__

-  `32-bit Address Flash Chips <#32-bit-address-flash-chips>`__

.. note::

    When Flash optional features listed in this page are used, aside from the capability of ESP chips, and ESP-IDF verison you are using, you will also need to make sure these features are supported by flash chips used.

    - If you are using an official Espressif modules/SiP. Some of the modules/SiPs always support the feature, in this case you can see these features listed in the datasheet. Otherwise please contact `Espressif's business team <https://www.espressif.com/en/contact-us/sales-questions>`_ to know if we can supply such products for you.

    - If you are making your own modules with your own bought flash chips, and you need features listed above. Please contact your vendor if they support the those features, and make sure that the chips can be supplied continuously.

.. attention::

    This document only shows that IDF code has supported the features of those flash chips. It's not a list of stable flash chips certified by Espressif. If you build your own hardware from flash chips with your own brought flash chips (even with flash listed in this page), you need to validate the reliability of flash chips yourself.

Auto Suspend & Resume
---------------------

This feature is only supported on ESP32-C3 for now.

The support for ESP32-S3, ESP32-C2, ESP32-C6, ESP32-H2 may be added in the future.

.. only:: esp32c3

    List of Flash chips that support this feature:

    1. XM25QxxC series.

    .. attention::

        There are multiple limitations about the auto-suspend feature, please do read :ref:`auto-suspend` for more information before you enable this feature.

Flash unique ID
---------------

This feature is supported on all Espressif chips.

Unique ID is not flash id, which means flash has 64-Bit unique ID for each device. The instruction to read the unique ID (4Bh) accesses a factory-set read-only 64-bit number that is unique to each flash device. This ID number helps you to recognize each single device. Not all flash vendors support this feature. If you try to read the unique ID on a chip which does not have this feature, the behavior is not determined. The support list is as follows.


List of Flash chips that support this feature:

1. ISSI
2. GD
3. TH
4. FM
5. Winbond
6. XMC
7. BOYA

.. _hpm-doc:

High performance mode
---------------------

This featuer is only supported on ESP32-S3 for now.

The support for ESP32-S2, ESP32-C3, ESP32-C6, ESP32-H2 may be added in the future.

.. only:: esp32s3

    .. note::

        This section is provided for Dual mode (DOUT/DIO) and Quad mode (QIO/QOUT) flash chips. Octal flash used on ESP-chips support High performance mode by default so far, you can refer to the octal flash support list below.

    High performance mode (HPM) means that the SPI1 and flash chip works under high frequency. Usually, when the operating frequency of the flash is greater than 80 MHz, it is considered that the flash works under HPM.

    As far as we acknowledged, there are more than three strategies for High Performance Mode (HPM) in typical SPI flash parts. For some flash chips, HPM is controlled by dummy cycle bit in the registers, while for other chips, it can be controlled by other bits (like HPM bit) in the register, or some special command. The difference in strategies requires the driver to explicitly add support for each chip.

    .. attention::

        It is hard to create several strategies to cover all situations, so all flash chips using HPM need to be supported explicitly. Therefore, if you try to use a flash not listed in :ref:`hpm_dc_support_list`, it might cause some error. So, when you try to use the flash chip beyond supported list, please test properly.

    Moreover, when the `Dummy Cycle adjustment` strategy is adopted by the flash chip, the flash remains in a state in which DC is different from the default value after a software reset. The sub mode of HPM that adjusts the dummy cycle to run at higher frequency in the application is called `HPM-DC`. `HPM-DC` feature needs a feature `DC Aware` to be enabled in the bootloader. Otherwise different DC value will forbid the 2nd bootloader from being boot up after reset.

    To enable High Performance Mode:

    1. De-select :ref:`CONFIG_ESPTOOLPY_OCT_FLASH` and :ref:`CONFIG_ESPTOOLPY_FLASH_MODE_AUTO_DETECT`. HPM is not used for Octal flash, enabling related options may bypass HPM functions.

    2. Enable ``CONFIG_SPI_FLASH_HPM_ENA`` option.

    3. Switch Flash frequency to HPM ones. For example, ``CONFIG_ESPTOOLPY_FLASHFREQ_120M``.

    4. Make sure the config option for `HPM-DC` feature (under ``CONFIG_SPI_FLASH_HPM_DC`` choices) is selected correctly according to whether the bootloader supports `DC Aware`.

        - If bootloader supports `DC Aware`, select ``CONFIG_SPI_FLASH_HPM_DC_AUTO``. This allows the usage of flash chips that adopted `Dummy Cycle adjustment` strategy.

        - If bootloader doesn't support `DC Aware`, select ``CONFIG_SPI_FLASH_HPM_DC_DISABLE``. It avoid consequences caused by running HPM-DC with non-DC-aware bootloaders. But please avoid using flash chips that adopts `Dummy Cycle adjustment` strategy if ``CONFIG_SPI_FLASH_HPM_DC_DISABLE`` is selected. See list of flash models that adpot DC strategy below.

    Check whether the bootloader supports `DC Aware` in the following way:

    - If you are starting a new project, it's suggested to enable `DC Aware` by selecting :ref:`CONFIG_BOOTLOADER_FLASH_DC_AWARE` option in the bootloader menu. Please note that, you won't be able to modify this option via OTA, because the support is in the bootloader.

    - If you are working on an existing project and want to update `HPM-DC` config option in the app via OTA, check the sdkconfig file used to build your bootloader: (Upgrading ESP-IDF version may make this file different from the one used by bootloader to build.)

        - For latest version (ESP-IDF v4.4.7+, v5.0.7+, v5.1.4+, v5.2 and above), if :ref:`CONFIG_BOOTLOADER_FLASH_DC_AWARE` is selected, the bootloader supports `DC Aware`.

        - For versions in this range: (v4.4.4-v4.4.6, v5.0-v5.0.6, and v5.1-v5.1.3), if ``CONFIG_ESPTOOLPY_FLASHFREQ_120M`` is selected, the bootloader supports `DC Aware`. In this case, enable :ref:`CONFIG_BOOTLOADER_FLASH_DC_AWARE` to confirm this (though it will not affect bootloader in devices in the field).

        - For versions below v4.4.4, the bootloader doesn't support `DC Aware`.

    .. _hpm_dc_support_list:

    Quad Flash HPM support list
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Flash chips that don't need HPM-DC:

    1. GD25Q64C (ID: 0xC84017)
    2. GD25Q32C (ID: 0xC84016)
    3. ZB25VQ32B (ID: 0x5E4016)

    Following flash chips also have HPM feature, but requires the bootloader to support `DC Aware`:

    1. GD25Q64E (ID: 0xC84017)
    2. GD25Q128E (ID: 0xC84018)
    3. XM25QH64C (ID: 0x204017)
    4. XM25QH128C (ID: 0x204018)

.. _oct-flash-doc:

OPI flash support
-----------------

This feature is only supporetd on ESP32-S3 for now.

OPI flash means that the flash chip supports octal peripheral interface, which has octal I/O pins. Different octal flash has different configurations and different commands. Hence, it is necessary to carefully check the support list.

.. only:: esp32s3

    .. note::

       To know how to configure menuconfig for a board with different Flash and PSRAM, please refer to the :ref:`SPI Flash and External SPI RAM Configuration <flash-psram-configuration>`

    List of Flash chips that support this feature:

    1. MX25UM25645G
    2. MX25UM12345G


.. _32-bit-flash-doc:

32-bit Address Flash Chips
--------------------------

This feature is supported on all Espressif chips (with various restrictions to application).

Most NOR flash chips used by Espressif chips use 24-bits address, which can cover 16 MBytes memory. However, for larger memory (usually equal to or larger than 16 MBytes), flash uses a 32-bits address to address larger memory. Regretfully, 32-bits address chips have vendor-specific commands, so we need to support the chips one by one.

List of Flash chips that support this feature:

1. W25Q256
2. GD25Q256

.. important::

    Over 16 MBytes space on flash mentioned above can be only used for ``data saving``, like file system. If your data/instructions over 16 MBytes spaces need to be mapped to MMU (so as to be accessed by the CPU), please upgrade to ESP-IDF v5.2 and read the latest docs.
