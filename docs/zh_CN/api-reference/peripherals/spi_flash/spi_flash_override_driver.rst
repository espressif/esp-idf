覆盖默认芯片驱动程序
====================

:link_to_translation:`en:[English]`

.. warning::

    自定义 SPI flash 芯片驱动程序是一项高级功能，进行此操作时应自担风险（请参阅下面的注意事项）。

在初始化 SPI flash 驱动程序时（即，:cpp:func:`esp_flash_init`），驱动程序会在芯片检测步骤中遍历默认芯片驱动程序列表，并找到可以支持当前连接的 flash 芯片的驱动程序。默认芯片驱动程序由 ESP-IDF 提供，因此会随着 ESP-IDF 的版本一起更新。当然，ESP-IDF 也允许自定义芯片驱动程序。

在自定义芯片驱动程序时，应注意以下事项：

    1. 自定义芯片驱动可能会用到非公开的 ESP-IDF 函数。ESP-IDF 版本不同，这些函数也有极小的可能会随之变化，有的会修复驱动程序中的错误，也有可能会破坏代码。
    2. 针对某些芯片驱动程序的 ESP-IDF 错误修复并不会自动应用到自定义的芯片驱动程序中。
    3. 如果未能正确处理针对 flash 的保护，可能会出现一些随机的可靠性问题。
    4. 如果升级到支持更多芯片的新 ESP-IDF 版本，则必须手动将这些新的芯片驱动程序添加到自定义芯片驱动程序列表中。否则，驱动程序将仅搜索自定义列表中已有的驱动程序。


创建自定义芯片驱动程序并覆盖 ESP-IDF 默认驱动程序列表的步骤
-----------------------------------------------------------

引导加载程序 flash 驱动程序
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

请仔细阅读本章节内容，从而实现引导加载程序驱动程序，确保 ESP 芯片能成功启动。错误的代码会引发 ESP 芯片无法启动等问题。注意，引导加载程序中的问题无法通过 OTA 修复，因此在进行任何改动前，请充分理解以下内容。

目前，引导加载程序中有两个部分可以覆盖：``bootloader_flash_unlock`` 和 ``bootloader_flash_qe_support_list`` （及其大小）。请参阅以下内容。

请确保所有命令均已成功发送和识别。如果 flash 接收到未知命令，则可能会静默忽略该指令并继续工作，且不会重新启用保护。这可能会导致误写入或误擦除，也可能造成误锁定问题。

所有引导加载程序 flash 驱动程序函数都位于一个引导加载程序组件中，该组件应放置在 ``bootloader_components`` 中。例如，:example:`storage/custom_flash_driver/bootloader_components/`。只有 ``bootloader_components`` 中的组件会自动添加到引导加载程序的组件列表中。

组件的 ``CMakeLists.txt`` 文件中有细微差别。自定义的 flash 函数通过覆盖现有的弱函数生效。在 ``CMakeLists.txt`` 文件中使用链接器参数 `-u`，显式指定被覆盖的函数名，确保自定义函数能链接到引导加载程序中。详情请参阅 :example_file:`storage/custom_flash_driver/bootloader_components/bootloader_flash/CMakeLists.txt`。

引导加载程序 flash 解锁
~~~~~~~~~~~~~~~~~~~~~~~

``bootloader_flash_unlock`` 函数用于解锁 flash 写保护。一旦 flash 被意外锁定，IDF 可以帮助解锁。默认情况下，解锁函数会清除状态寄存器和配置寄存器中除 QE 位以外的所有位。

请不要修改 QE 位，否则芯片将无法在四线模式下运行。

请检查以下默认设置。如果你的 flash 芯片表现与此不同，则需要进行一些修改。

在修改前，可以将 ``components/bootloader_support/bootloader_flash/src/bootloader_flash.c`` 中的 ``bootloader_flash_unlock_default`` 函数复制到自定义组件文件夹中的文件里。重命名该函数为 ``bootloader_flash_unlock`` 以覆盖 IDF 引导加载程序函数。例如，:example_file:`storage/custom_flash_driver/bootloader_components/bootloader_flash/bootloader_flash_unlock_custom.c`。

此时，``bootloader_flash_unlock_default`` 已声明为弱符号，链接的函数将被替换为修改后的 ``bootloader_flash_unlock``。因此，当 IDF 的第二阶段引导加载程序启动时，将执行 ``bootloader_flash_unlock`` 实现。为确保你的函数被正确链接，可启用 ``CONFIG_BOOTLOADER_LOG_LEVEL_DEBUG``，查看日志中是否有 ``Using overridden bootloader_flash_unlock`` 提示。

默认函数包含三种常见行为：

- 情况 1（**默认**）：请检查 flash 芯片的 QE 位是否位于状态寄存器-2 的第一位，并通过命令 01H (WRSR) 后发送的第二个字节进行写入。如果属实，则符合默认行为，无需进行其他操作。
- 情况 2：如果 flash 芯片不支持情况 1 中使用 01H + 2 字节写入的方式，则检查 flash 芯片的 QE 位是否位于状态寄存器-2 的第一位，并通过命令 31H 写入。此时，请在函数 :cpp:func:`is_qe_bit_in_bit1_at_reg2` 中添加芯片 ID。例如：

.. code-block:: c

    IRAM_ATTR bool is_qe_bit_in_bit1_at_reg2(const esp_rom_spiflash_chip_t* chip)
    {
        bool ret = true;
        switch (chip->device_id) {
        /****GD 系列***/
        case 0xC84016:
        case 0xC84017:
        case 0xC84018:
            break;
        /**** flash 系列 ****/
        case /*flash ID*/:
            break;
        default:
            ret = false;
        }
        return ret;
    }

- 情况 3：请检查 flash 芯片中的 QE 位是否位于状态寄存器-1 的第六位，由命令 01H 写入。如果属实，请在函数 :cpp:func:`is_qe_bit_in_bit6_at_reg1` 中添加芯片 ID。例如：

.. code-block:: c

    IRAM_ATTR bool is_qe_bit_in_bit6_at_reg1(const esp_rom_spiflash_chip_t* chip)
    {
        bool ret = true;
        switch (chip->device_id) {
        /***ISSI 系列***/
        case 0x9D4016:
        case 0x9D4017:
            break;

        /***MXIC 系列***/
        case 0xC22016:
        case 0xC22017:
            break;

        /****flash 系列***/
        case /*flash ID*/:
            break;
        default:
            ret = false;
        }
        return ret;
    }

- 情况 4：如果你的芯片不符合上述三种情况，请在 ``bootloader_flash_unlock`` 函数中添加一个新的 `if` 块以及相应的行为函数。建议将 `if` 块中的确定性函数命名为 ``is_qe_bit_in_bit_x_at_reg_x_`` （其中 x 代表行为）。参考示例 :example_file:`storage/custom_flash_driver/bootloader_components/bootloader_flash/bootloader_flash_unlock_custom.c`。

引导加载程序 flash 四线模式支持
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

指针 ``bootloader_flash_qe_support_list`` 用于在引导加载程序中迭代选择正确的行为，使 flash 芯片在四线模式下工作。请阅读以下内容，启用 flash 状态寄存器中的 QE 位，使 flash 在四线模式下工作。

* 情况 1：如果 QE 位位于状态寄存器-2 的第一位，那么可以通过命令 31H 写入。这是默认的行为，无需进行其他操作。
* 情况 2：如果芯片的 QE 位在其他位置，或需要使用其他命令，请自行添加支持。

想要自行添加支持，可以从 `flash_qio_mode.c <https://github.com/espressif/esp-idf/blob/master/components/bootloader_support/bootloader_flash/src/flash_qio_mode.c>`_ 中复制 ``bootloader_flash_qe_support_list_user`` 函数到你的文件中，将函数重命名为 ``bootloader_flash_qe_support_list``。与此同时，请自定义相应的 ``bootloader_flash_qe_list_count``。

将你的 flash 芯片信息（如芯片的名称、ID 和写寄存器的函数等）添加到 ``bootloader_flash_qio_support_list`` 函数中，也可以复用现有的函数，如 ``bootloader_read_status_8b_rdsr``。

如果现有的函数不能满足需求，则可以使用 ``bootloader_execute_flash_command`` 来自定义函数，如 ``bootloader_read_status_otp_mode_8b`` 和 ``bootloader_write_status_otp_mode_8b``。详情可参阅 `bootloader_flash_custom.c <https://github.com/espressif/esp-flash-drivers/tree/main/esp_flash_nor/bootloader_flash_driver/bootloader_flash_custom.c>`_。

整合以上内容：

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

        /* 如果没有其他 ID 匹配当前条目，则最终条目为默认条目

            以上方法适用于下列芯片：
            GigaDevice (制造商 ID 0xC8, flash ID 包括 4016),
            FM25Q32 (仅 QOUT 模式, 制造商 ID 0xA1, flash ID 包括 4016)
            BY25Q32 (制造商 ID 0x68, flash ID 包括 4016)
       */
        { NULL,          0xFF,    0xFFFF, 0xFFFF,   bootloader_read_status_8b_rdsr2,       bootloader_write_status_8b_wrsr2,      1 },
    };
    const DRAM_ATTR bootloader_qio_info_t* bootloader_flash_qe_support_list = bootloader_flash_qe_support_list_user;
    uint8_t DRAM_ATTR bootloader_flash_qe_list_count = (sizeof(bootloader_flash_qe_support_list_user) / sizeof(bootloader_qio_info_t));

应用程序 flash 驱动程序
^^^^^^^^^^^^^^^^^^^^^^^

通用 flash 驱动程序
~~~~~~~~~~~~~~~~~~~

应用程序中的 flash 驱动程序用于读取、写入、擦除、保存数据等操作，且支持 OTA 等高级功能。可参考下列指南，为你的 flash 芯片自定义驱动程序。

- 步骤 1：`default_registered_chips` 的最后一项应为 `通用芯片驱动程序 <https://github.com/espressif/esp-idf/blob/master/components/spi_flash/spi_flash_chip_generic.c>`_。如果你的 flash 芯片无法匹配以上列出的任何一个芯片驱动程序，则将使用通用驱动。请检查你的 flash 芯片行为与通用驱动是否存在差异，包括但不限于不同的命令、dummy 周期、数据字节以及状态寄存器。
- 步骤 2：如果你的 flash 芯片行为与通用驱动存在差异，则需要实现自定义的芯片驱动程序。请创建一个名为 ``spi_flash_chip_<vendor>.c`` 的新文件，在其中实现特定行为。可以将 ``esp_flash_chip_generic`` 结构体复制到文件中进行修改。记得在文件中包含 ``esp_flash_chips/spi_flash_chip_generic.h``。详情请参阅示例 `esp_flash_nor <https://github.com/espressif/esp-flash-drivers/tree/main/esp_flash_nor/>`_。

  .. note::

      芯片驱动程序头文件位于 ``esp_flash_chips/`` 目录中（例如 ``components/spi_flash/include/esp_flash_chips/``）。这些头文件是**半公开的** - 它们面向需要实现自定义芯片驱动程序的高级用户，但**不被视为稳定的 API**，可能会在未通知的情况下更改。

- 步骤 3：实现与通用驱动程序存在差异的函数，并从 ``spi_flash_chip_t`` 结构体中指向这些函数。注意：如果 flash 芯片的某些行为与通用驱动相同，那么可以保留通用驱动程序的函数，无需自定义，只要为与通用驱动不同的部分编写自定义函数即可。请参考以下示例：

.. important::

    flash 芯片的硬件设计各不相同，因此启用 :ref:`CONFIG_SPI_FLASH_AUTO_SUSPEND` 选项暂停 flash 时应仔细且系统地进行测试。如果想在量产过程中使用挂起功能，请联系 `乐鑫商务部 <https://www.espressif.com/zh-hans/contact-us/sales-questions>`_。

.. code-block:: c

    const DRAM_ATTR spi_flash_chip_t esp_flash_chip_eon = {
        .name = chip_name,
        .timeout = &spi_flash_chip_generic_timeout,  /*<! 默认行为*/
        .probe = spi_flash_chip_eon_probe,   /*<! EON 特定 */
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

    - 编写自定义的 flash 芯片驱动程序时，可以通过 ``spi_flash_chip_***(vendor)_get_caps`` 来设置芯片功能，并将函数指针 ``get_chip_caps`` 指向 ``spi_flash_chip_***_get_caps`` 函数，确保该函数在后续使用过程中不会被误改或覆盖。步骤如下：

        1. 查看芯片技术规格书，确认你的 flash 芯片是否具有 ``spi_flash_caps_t`` 中列出的功能。
        2. 编写一个名为 ``spi_flash_chip_***(vendor)_get_caps`` 的函数。如果你的 flash 芯片支持 ``挂起`` 和 ``读取唯一 ID`` 的功能，可参考以下示例进行编写。
        3. 将 ``spi_flash_chip_t`` 结构体中的指针 ``get_chip_caps`` 指向函数 ``spi_flash_chip_***_get_caps``。

        .. code-block:: c

            spi_flash_caps_t spi_flash_chip_***(vendor)_get_caps(esp_flash_t *chip)
            {
                spi_flash_caps_t caps_flags = 0;
                // 不支持 32 位地址的 flash
                caps_flags |= SPI_FLAHS_CHIP_CAP_SUSPEND;
                // 读取唯一 ID
                caps_flags |= SPI_FLASH_CHIP_CAP_UNIQUE_ID;
                return caps_flags;
            }

        .. code-block:: c

            const spi_flash_chip_t esp_flash_chip_eon = {
                // 其他函数指针
                .get_chip_caps = spi_flash_chip_eon_get_caps,
            };

    - :example:`storage/custom_flash_driver` 示例同样展示了实现过程，并且演示了如何覆盖默认的芯片驱动程序列表。

- 步骤 4：创建一个头文件（例如 ``spi_flash_chip_<vendor>.h``）并使用 `extern` 声明结构体，以便其他组件或源代码复用结构体。将所有芯片驱动程序（包括源文件及其头文件）封装为一个芯片驱动程序组件，并将包含路径和源文件添加到该组件的 ``CMakeLists.txt`` 文件中。

- 步骤 5：在 cache 被禁用时，通过 ``linker.lf`` 文件把要使用的所有芯片驱动程序放入内部 RAM 中。详情请参阅 :doc:`/api-guides/linker-script-generation`。请确保 ``linker.lf`` 包含了你添加的所有源文件。

- 步骤 6：在项目中添加一个新的组件，例如 ``custom_chip_driver``。在 ``custom_chip_driver/chip_drivers.c`` 文件中将芯片对象列在 ``default_registered_chips`` 下。启用 :ref:`CONFIG_SPI_FLASH_OVERRIDE_CHIP_DRIVER_LIST` 配置选项，防止编译和链接由 ESP-IDF 提供的默认芯片驱动程序列表 ``default_registered_chips``；相反，链接器会搜索由你自定义的同名结构体 ``default_registered_chips``。详情请参阅 :example_file:`storage/custom_flash_driver/components/custom_chip_driver/chip_drivers.c`。

- 步骤 7：构建项目，你将看到新的 flash 驱动程序。

高性能 flash 实现
~~~~~~~~~~~~~~~~~

高性能模式在高于 80 MHz 的频率下运行。请查阅 *直流电气特性* 章节，判断芯片是否支持在高于 80 MHz 的频率下工作。`高性能文件 <https://github.com/espressif/components/spi_flash/spi_flash_hpm_enable.c>`_ 中已经预定义了部分高性能模式下的行为，如果你的 flash 芯片符合指定行为，请按照 ``bootloader_flash_unlock`` 部分介绍的方法扩展列表。如果你的 flash 芯片有不同的行为，请添加新行为并覆盖 ``spi_flash_hpm_enable_list`` 行为表。

.. important::

    频率设置为 80 MHz 以上的 flash 芯片应进行仔细的测试，因为此时系统对于时序的要求非常严格。如果想在量产过程中使用高性能模式的功能，请联系 `乐鑫商务部 <https://www.espressif.com/zh-hans/contact-us/sales-questions>`_。

示例
----

请参阅 :example:`storage/custom_flash_driver`，该示例通过使用 `外部组件 <https://github.com/espressif/esp-flash-drivers/tree/main/esp_flash_nor>`_，增加了对自定义 flash 的支持，实现自定义驱动程序；本文则展示了如何覆盖 IDF 驱动程序。
