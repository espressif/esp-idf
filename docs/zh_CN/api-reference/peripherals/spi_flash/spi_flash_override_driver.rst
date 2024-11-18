覆盖默认芯片驱动程序
====================

:link_to_translation:`en:[English]`

.. warning::

   自定义 SPI Flash 芯片驱动程序是一项高级功能，进行此操作时应自担风险。（请参阅下面的注意事项）

在初始化 SPI Flash 驱动程序时（即，:cpp:func:`esp_flash_init`），驱动程序会在芯片检测步骤中遍历默认芯片驱动程序列表，并找到可以支持当前连接的 Flash 芯片的驱动程序。默认芯片驱动程序由 ESP-IDF 提供，因此会随着 ESP-IDF 的版本一起更新。当然，ESP-IDF 也允许自定义芯片驱动程序。

在自定义芯片驱动程序时，应注意以下事项：

   1. 自定义芯片驱动可能会用到非公开的 ESP-IDF 函数。ESP-IDF 版本不同，这些函数也有极小的可能会随之变化，有的会修复驱动程序中的错误，也有可能会破坏代码。
   2. 针对某些芯片驱动程序的 ESP-IDF 错误修复并不会自动应用到自定义的芯片驱动程序中。
   3. 如果未能正确处理针对 flash 的保护，可能会出现一些随机的可靠性问题。
   4. 如果升级到支持更多芯片的新 ESP-IDF 版本，则必须手动将这些新的芯片驱动程序添加到自定义芯片驱动程序列表中。否则，驱动程序将仅搜索自定义列表中已有的驱动程序。


创建自定义芯片驱动程序并覆盖 ESP-IDF 默认驱动程序列表的步骤
-----------------------------------------------------------

.. highlight: cmake

1. 启用 :ref:`CONFIG_SPI_FLASH_OVERRIDE_CHIP_DRIVER_LIST` 配置选项，确保不会编译和链接 ESP-IDF 的默认芯片驱动列表 (default_registered_chips)，而链接器会搜索用户自定义的同名结构体 (default_registered_chips)。
2. 在项目中添加一个新组件，例如 ``custom_chip_driver``。
3. 从 ESP-IDF 的 ``spi_flash`` 组件中复制必要的芯片驱动程序文件。这可能包括：

   - ``spi_flash_chip_drivers.c`` （该文件中定义了 ``default_registered_chips`` 结构体）
   - 与你的 flash 型号最匹配的任何 ``spi_flash_chip_*.c`` 文件
   - ``CMakeLists.txt`` 和 ``linker.lf`` 文件

   适当修改上述文件，如：

   - 将 ``default_registered_chips`` 变量更改为非静态，并删除相关的 #ifdef 逻辑。
   - 更新 ``linker.lf`` 文件，重命名分片头部和库名，从而匹配新组件。
   - 如果复用其他驱动程序，不在 spi_flash 组件中的分片头部需要在名称中添加 ``spi_flash/`` 前缀。

.. note::

   - 在编写自己的 flash 芯片驱动程序时，可以通过 ``spi_flash_chip_***(vendor)_get_caps`` 来设置 flash 芯片的功能，并将函数指针 ``get_chip_caps`` 指向 ``spi_flash_chip_***_get_caps`` 函数以进行保护。步骤如下：

      1. 通过查阅 flash 技术规格书，确定自己的 flash 芯片具有 ``spi_flash_caps_t`` 所列出的功能。
      2. 编写一个名为 ``spi_flash_chip_***(vendor)_get_caps`` 的函数。如果 flash 支持 ``suspend`` 和 ``read unique id``，可参考以下示例。
      3. 将指针 ``get_chip_caps`` （在 ``spi_flash_chip_t`` 中）指向第二步中编写的函数。

      .. code-block:: c

         spi_flash_caps_t spi_flash_chip_***(vendor)_get_caps(esp_flash_t *chip)
         {
           spi_flash_caps_t caps_flags = 0;
           // 32-bit-address flash is not supported
           flash-suspend is supported
           caps_flags |= SPI_FLAHS_CHIP_CAP_SUSPEND;
           // flash read unique id.
           caps_flags |= SPI_FLASH_CHIP_CAP_UNIQUE_ID;
           return caps_flags;
         }

      .. code-block:: c

         const spi_flash_chip_t esp_flash_chip_eon = {
            // Other function pointers
            .get_chip_caps = spi_flash_chip_eon_get_caps,
         };

   - 也可以在示例 :example:`storage/custom_flash_driver` 中查看如何实现此功能。该示例演示了如何覆盖默认芯片驱动列表。

4. 为 ``custom_chip_driver`` 组件编写一个新的 ``CMakeLists.txt`` 文件，其中包含额外的一行，添加了从 ``spi_flash`` 到 ``custom_chip_driver`` 的链接依赖项::

        idf_component_register(SRCS "spi_flash_chip_drivers.c"
                               "spi_flash_chip_mychip.c"  # modify as needed
                               REQUIRES hal
                               PRIV_REQUIRES spi_flash
                               LDFRAGMENTS linker.lf)
        idf_component_add_link_dependency(FROM spi_flash)

   - CMakeLists.txt 组件的示例可查阅 :example_file:`storage/custom_flash_driver/components/custom_chip_driver/CMakeLists.txt`

5. ``linker.lf`` 用于在禁用缓存时，把要使用的每个芯片驱动程序都放入内部 RAM 中。详情请参阅 :doc:`/api-guides/linker-script-generation`。请确保此文件包含所有添加的源文件。

6. 构建你的项目，将会看到新的 flash 驱动程序已投入使用。
