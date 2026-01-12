Overriding Default Chip Drivers
===============================

:link_to_translation:`zh_CN:[中文]`

.. warning::

    Customizing SPI Flash Chip Drivers is considered an "expert" feature. The user should only do so at their own risk (see the notes below).

During the SPI Flash driver's initialization (i.e., :cpp:func:`esp_flash_init`), there is a chip detection step during which the driver iterates through a Default Chip Driver List and determine which chip driver can properly support the currently connected flash chip. The Default Chip Drivers are provided by the ESP-IDF, thus are updated in together with each ESP-IDF version. However ESP-IDF also allows to customize their own chip drivers.

Note the following points when customizing chip drivers:

    1. You may need to rely on some non-public ESP-IDF functions, which have slight possibility to change between ESP-IDF versions. On the one hand, these changes may be useful bug fixes for your driver, on the other hand, they may also be breaking changes (i.e., breaks your code).
    2. Some ESP-IDF bug fixes to other chip drivers are not automatically applied to your own custom chip drivers.
    3. If the protection of flash is not handled properly, there may be some random reliability issues.
    4. If you update to a newer ESP-IDF version that has support for more chips, you will have to manually add those new chip drivers into your custom chip driver list. Otherwise the driver will only search for the drivers in custom list you provided.


Steps For Creating Custom Chip Drivers and Overriding the ESP-IDF Default Driver List
-------------------------------------------------------------------------------------

Bootloader Flash Driver
^^^^^^^^^^^^^^^^^^^^^^^

To implement the bootloader driver so that the ESP chip can boot successfully, please read this part carefully. Wrong code will make ESP chip fail to boot or introduce random issues. Issues in the bootloader cannot be fixed via OTA. Please fully comprehend below parts before making any changes.

Currently, there are two parts to override in bootloader: ``bootloader_flash_unlock`` and ``bootloader_flash_qe_support_list`` (and its size). See sections below.

Ensure that all commands are sent and recognized successfully. If the flash receives an unknown command, it may silently ignore it and continue operating without re-enabling protection. This could lead to mis-writing, erasing, or locking issues.

All bootloader flash driver functions reside in a component for the bootloader, which should be placed under ``bootloader_components``. For example, :example:`storage/custom_flash_driver/bootloader_components/`. Only the components under ``bootloader_components`` will be added into the component list of bootloader automatically.

There is a slight difference in the ``CMakeLists.txt`` of the component. The customized flash functions take effect by overriding existing weak ones. In order to link them into the bootloader, use the linker argument `-u` against the overriding functions in the ``CMakeLists.txt``. See :example_file:`storage/custom_flash_driver/bootloader_components/bootloader_flash/CMakeLists.txt`.

Bootloader flash unlock
~~~~~~~~~~~~~~~~~~~~~~~

``bootloader_flash_unlock`` function is used to unlock flash write protection, which means once the flash is locked by accident, IDF is able to unlock it. By default, the unlock function will clear all bits in status registers and configuration registers except QE bit.

Please don not modify the QE bit; otherwise, your chip will not be able to run under QUAD modes.

Please check the default case below. If your flash have different behavior from that, some modifications are required.

You can start from copying ``bootloader_flash_unlock_default`` from ``components/bootloader_support/bootloader_flash/src/bootloader_flash.c`` into a file in your customized component folder before modifying it. The function should be renamed to ``bootloader_flash_unlock`` to override the IDF bootloader function. For example, :example_file:`storage/custom_flash_driver/bootloader_components/bootloader_flash/bootloader_flash_unlock_custom.c`.

After that, your implementation of ``bootloader_flash_unlock`` will be linked instead of IDF default one, which has been declared as a weak symbol. So when IDF second stage bootloader boots, your implementation of ``bootloader_flash_unlock`` will be executed. If you want to make sure your function is truly linked, you can enable ``CONFIG_BOOTLOADER_LOG_LEVEL_DEBUG`` and you will see a ``Using overridden bootloader_flash_unlock`` log.

The default function includes three common behaviors, please check first:

- Case 1 (**Default**): Check if the QE bit in your flash chip is located at bit 1 in status register-2, written by the second byte after command 01H (WRSR). If so, this matches the default behavior, and no further action is needed.
- Case 2: Check if the QE bit in your flash chip is located at bit 1 in status register-2, written by command 31H (and your flash does not support Case 1, which uses 01H + 2 bytes to write it). If this is the case, add your chip ID to the function :cpp:func:`is_qe_bit_in_bit1_at_reg2`. For example:

.. code-block:: c

    IRAM_ATTR bool is_qe_bit_in_bit1_at_reg2(const esp_rom_spiflash_chip_t* chip)
    {
        bool ret = true;
        switch (chip->device_id) {
        /****GD series***/
        case 0xC84016:
        case 0xC84017:
        case 0xC84018:
            break;
        /**** your flash series ****/
        case /*your flash ID*/:
            break;
        default:
            ret = false;
        }
        return ret;
    }

- Case 3: Check whether the QE bit in your flash chip is located at bit 6 in status register-1, which can be written by command 01H. If so, you need to add your chip ID in function :cpp:func:`is_qe_bit_in_bit6_at_reg1`. For example:

.. code-block:: c

    IRAM_ATTR bool is_qe_bit_in_bit6_at_reg1(const esp_rom_spiflash_chip_t* chip)
    {
        bool ret = true;
        switch (chip->device_id) {
        /***ISSI series***/
        case 0x9D4016:
        case 0x9D4017:
            break;

        /***MXIC series***/
        case 0xC22016:
        case 0xC22017:
            break;

        /****your flash series***/
        case /*your flash ID*/:
            break;
        default:
            ret = false;
        }
        return ret;
    }

- Case 4: If the three cases mentioned above cannot cover your usage, please add another `if` block and the corresponding behavior function in function ``bootloader_flash_unlock``. The determination function in the `if` block is suggested to be named after ``is_qe_bit_in_bit_x_at_reg_x_`` (x stands for behavior). Refer to example :example_file:`storage/custom_flash_driver/bootloader_components/bootloader_flash/bootloader_flash_unlock_custom.c`.

Bootloader Flash Quad Mode Support
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Pointer ``bootloader_flash_qe_support_list`` is used for iteration in bootloader for selecting the correct behavior to enable flash chip work under QUAD mode. To operate the flash in QUAD mode, enabling the QE bit in the flash status register is necessary. If you want to use your flash chip under QUAD mode, please read this part and implement the necessary changes.

* Case 1: If QE bit is placed at bit 1 in status register-2 to be written by command 31H, nothing needs to be done because this is the default behavior.
* Case 2: If QE bit on your chip is placed at different places, or need to use different command. Please add your own support.

To add your own support, you can start from copying the ``bootloader_flash_qe_support_list_user`` function from `flash_qio_mode.c <https://github.com/espressif/esp-idf/blob/master/components/bootloader_support/bootloader_flash/src/flash_qio_mode.c>`_ into your file, renaming to ``bootloader_flash_qe_support_list``. Please also define a corresponding ``bootloader_flash_qe_list_count``.

Add the details of your flash chip, including the chip's name, ID, and the functions to write registers, into ``bootloader_flash_qio_support_list``. You can also reuse the existing functions like ``bootloader_read_status_8b_rdsr``.

If the existing functions do not fully meet your needs, you can define your own functions using ``bootloader_execute_flash_command``, such as ``bootloader_read_status_otp_mode_8b`` and ``bootloader_write_status_otp_mode_8b``. For example, see `bootloader_flash_custom.c <https://github.com/espressif/esp-flash-drivers/tree/main/esp_flash_nor/bootloader_flash_driver/bootloader_flash_custom.c>`_.

Put everything together:

.. code-block:: c

    const DRAM_ATTR bootloader_qio_info_t bootloader_flash_qe_support_list_user[] = {
        /*   Manufacturer,   mfg_id, flash_id, id mask, Read Status,                Write Status,               QIE Bit */
        { "MXIC",        0xC2,   0x2000, 0xFF00,    bootloader_read_status_8b_rdsr,        bootloader_write_status_8b_wrsr,       6 },
        { "ISSI",        0x9D,   0x4000, 0xCF00,    bootloader_read_status_8b_rdsr,        bootloader_write_status_8b_wrsr,       6 },
        { "WinBond",     0xEF,   0x4000, 0xFF00,    bootloader_read_status_16b_rdsr_rdsr2, bootloader_write_status_16b_wrsr,      9 },
        { "GD",          0xC8,   0x4000, 0xFFFF,    bootloader_read_status_16b_rdsr_rdsr2, bootloader_write_status_16b_wrsr,      9 },
        { "XM25QU64A",   0x20,   0x3817, 0xFFFF,    bootloader_read_status_8b_xmc25qu64a,  bootloader_write_status_8b_xmc25qu64a, 6 },
        { "TH",          0xCD,   0x6000, 0xFF00,    bootloader_read_status_16b_rdsr_rdsr2, bootloader_write_status_16b_wrsr,      9 },
        { "EON",         0x1C,   0x7000, 0xFF00,    bootloader_read_status_otp_mode_8b,    bootloader_write_status_otp_mode_8b,   6 },

        /* Final entry is default entry, if no other IDs have matched

            This approach works for chips including:
            GigaDevice (mfg ID 0xC8, flash IDs including 4016),
            FM25Q32 (QOUT mode only, mfg ID 0xA1, flash IDs including 4016)
            BY25Q32 (mfg ID 0x68, flash IDs including 4016)
        */
        { NULL,          0xFF,    0xFFFF, 0xFFFF,   bootloader_read_status_8b_rdsr2,       bootloader_write_status_8b_wrsr2,      1 },
    };
    const DRAM_ATTR bootloader_qio_info_t* bootloader_flash_qe_support_list = bootloader_flash_qe_support_list_user;
    uint8_t DRAM_ATTR bootloader_flash_qe_list_count = (sizeof(bootloader_flash_qe_support_list_user) / sizeof(bootloader_qio_info_t));

App Flash Driver
^^^^^^^^^^^^^^^^

Generic Flash Driver
~~~~~~~~~~~~~~~~~~~~

The flash driver in the application is used to read, write, erase, and save data. It also supports some advanced features like OTA. Below is a guide on how to customize the driver for your specific flash model.

- Step 1: The last item of `default_registered_chips` should be the `generic chip driver <https://github.com/espressif/esp-idf/blob/master/components/spi_flash/spi_flash_chip_generic.c>`_. If your flash chip does not match any of the chip drivers listed above, it will use the generic driver. Check for any differences in behavior between your flash and the generic driver, including but not limited to different commands, dummy cycles, data bytes, and status registers.
- Step 2: If you have found something different from the generic driver, you need to implement your own chip driver. Create a new file named ``spi_flash_chip_<vendor>.c`` to implement the specific behavior, and copy the ``esp_flash_chip_generic`` structure into it as a starting point. Remember to include ``spi_flash_chip_generic.h``. Here is an example `esp_flash_nor <https://github.com/espressif/esp-flash-drivers/tree/main/esp_flash_nor/>`_.
- Step 3: Implement the functions with difference and point to them from the ``spi_flash_chip_t``. Note: if some behavior of your flash is the same as the generic one, retain the generic driver functions without customization. Only implement the parts that differ. Here is an example:

.. important::

    Flash work for suspend (i.e., enabling :ref:`CONFIG_SPI_FLASH_AUTO_SUSPEND`) should be tested carefully and systematically due to different flash hardware design. If you want to use the suspend feature for mass production, please contact `Espressif's business team <https://www.espressif.com/en/contact-us/sales-questions>`_.

.. code-block:: c

    const DRAM_ATTR spi_flash_chip_t esp_flash_chip_eon = {
        .name = chip_name,
        .timeout = &spi_flash_chip_generic_timeout,  /*<! default behavior*/
        .probe = spi_flash_chip_eon_probe,   /*<! EON specific */
        .reset = spi_flash_chip_generic_reset,
        .detect_size = spi_flash_chip_generic_detect_size,
        .erase_chip = spi_flash_chip_generic_erase_chip,
        .erase_sector = spi_flash_chip_generic_erase_sector,
        .erase_block = spi_flash_chip_generic_erase_block,
        .sector_size = 4 * 1024,
        .block_erase_size = 64 * 1024,

        .get_chip_write_protect = spi_flash_chip_generic_get_write_protect,
        .set_chip_write_protect = spi_flash_chip_generic_set_write_protect,

        .num_protectable_regions = 0,
        .protectable_regions = NULL,
        .get_protected_regions = NULL,
        .set_protected_regions = NULL,

        .read = spi_flash_chip_generic_read,
        .write = spi_flash_chip_generic_write,
        .program_page = spi_flash_chip_generic_page_program,
        .page_size = 256,
        .write_encrypted = spi_flash_chip_generic_write_encrypted,

        .wait_idle = spi_flash_chip_generic_wait_idle,
        .set_io_mode = spi_flash_chip_eon_set_io_mode,
        .get_io_mode = spi_flash_chip_eon_get_io_mode,

        .read_reg = spi_flash_chip_generic_read_reg,
        .yield = spi_flash_chip_generic_yield,
        .sus_setup = spi_flash_chip_eon_suspend_cmd_conf,
        .get_chip_caps = spi_flash_chip_eon_get_caps,
    };

.. note::

    - When writing your own flash chip driver, you can set your flash chip capabilities through ``spi_flash_chip_***(vendor)_get_caps`` and point the function pointer ``get_chip_caps`` to the ``spi_flash_chip_***_get_caps`` function for protection. The steps are as follows.

        1. Please check whether your flash chip has the capabilities listed in ``spi_flash_caps_t`` by checking the flash datasheet.
        2. Write a function named ``spi_flash_chip_***(vendor)_get_caps``. Take the example below as a reference (if the flash supports ``suspend`` and ``read unique id``).
        3. Point the pointer ``get_chip_caps`` (in ``spi_flash_chip_t``) to ``spi_flash_chip_***_get_caps``.

        .. code-block:: c

            spi_flash_caps_t spi_flash_chip_***(vendor)_get_caps(esp_flash_t *chip)
            {
                spi_flash_caps_t caps_flags = 0;
                // 32-bit address flash is not supported
                caps_flags |= SPI_FLAHS_CHIP_CAP_SUSPEND;
                // Read flash unique id
                caps_flags |= SPI_FLASH_CHIP_CAP_UNIQUE_ID;
                return caps_flags;
            }

        .. code-block:: c

            const spi_flash_chip_t esp_flash_chip_eon = {
                // Other function pointers
                .get_chip_caps = spi_flash_chip_eon_get_caps,
            };

    - You can also see how to implement this in the example :example:`storage/custom_flash_driver`. This example demonstrates how to override the default chip driver list.

- Step 4: Create a header file (e.g., ``spi_flash_chip_<vendor>.h``) and declare the structure using `extern`, so that other components or source code can reuse this structure. Wrap all your chip drivers (including source files and their headers) into a chip-driver component. Add the include path and the source file to the component ``CMakeLists.txt``.

- Step 5: The ``linker.lf`` is used to put every chip driver that you are going to use whilst cache is disabled into internal RAM. See :doc:`/api-guides/linker-script-generation` for more details. Make sure this file covers all the source files that you add.

- Step 6: Add a new component in your project, e.g., ``custom_chip_driver``. List your chip object under ``default_registered_chips`` in ``custom_chip_driver/chip_drivers.c``. Then enable the :ref:`CONFIG_SPI_FLASH_OVERRIDE_CHIP_DRIVER_LIST` configuration option. This prevents compilation and linking of the default chip driver list (``default_registered_chips``) provided by ESP-IDF. Instead, the linker searches for the structure of the same name (``default_registered_chips``) that must be provided by you. You can refer to :example_file:`storage/custom_flash_driver/components/custom_chip_driver/chip_drivers.c`.

- Step 7: Build your project, and you will see the new flash driver in use.

High Performance Flash Implementation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The high performance mode operates at frequencies higher than 80 MHz. Please check the datasheet for your flash and to determine which approach can reach to frequencies higher than 80 MHz, as listed in *DC Characteristics* section. Some behavior is already defined in the `high performance file <https://github.com/espressif/components/spi_flash/spi_flash_hpm_enable.c>`_ . If your flash meets the specified behavior, extend the list as introduced in the ``bootloader_flash_unlock`` section. If your flash has different behavior, please add the new behavior and override the behavior table ``spi_flash_hpm_enable_list``.

.. important::

    Flash with a frequency set above 80 MHz should be tested carefully due to its strict timing requirements. If you want to use the high performance mode feature for mass production, please contact `Espressif's business team <https://www.espressif.com/en/contact-us/sales-questions>`_.

Example
-------

See :example:`storage/custom_flash_driver`, which uses an `external component <https://github.com/espressif/esp-flash-drivers/tree/main/esp_flash_nor>`_ to add support for custom flash and implement customized drivers, while this document mainly focuses on illustrating how to override the IDF driver.
