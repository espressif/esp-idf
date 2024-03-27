存储
=======

分区 API 的新组件
------------------------------------

非兼容性更新：所有的分区 API 代码都已迁移到新组件 :component:`esp_partition` 中。如需查看所有受影响的函数及数据类型，请参见头文件 :component_file:`esp_partition.h <esp_partition/include/esp_partition.h>`。

在以前，这些 API 函数和数据类型属于 :component:`spi_flash` 组件。因此，在现有的应用程序中或将依赖 ``spi_flash``，这也意味着在直接使用 esp_partition_* API/数据类型时，可能会导致构建过程失败（比如，在出现 ``#include "esp_partition.h"`` 的行中报错 ``fatal error: esp_partition.h: No such file or directory``）。如果遇到类似问题，请按以下步骤更新项目中的 CMakeLists.txt 文件：

原有的依赖性设置：

.. code-block:: cmake

   idf_component_register(...
                          REQUIRES spi_flash)

更新后的依赖性设置：

.. code-block:: cmake

   idf_component_register(...
                          REQUIRES spi_flash esp_partition)

.. note::

   请根据项目的实际情况，更新相应的 ``REQUIRES`` 或是 ``PRIV_REQUIRES`` 部分。上述代码片段仅为范例。

如果问题仍未解决，请联系我们，我们将协助你进行代码迁移。


SDMMC/SDSPI
-----------

用户现可通过 ``sdmmc_host_t.max_freq_khz`` 将 SDMMC/SDSPI 接口上的 SD 卡频率配置为特定值，不再局限于之前的 ``SDMMC_FREQ_PROBING`` (400 kHz)、 ``SDMMC_FREQ_DEFAULT`` (20 MHz) 或是 ``SDMMC_FREQ_HIGHSPEED`` (40 MHz)。此前，如果用户配置了上述三个给定频率之外的值，用户所选频率将自动调整为与其最为接近的给定值。

更新后，底层驱动将计算与用户配置的特定值最为接近的合适频率。相对于枚举项选择，该频率现由可用的分频器提供。不过，如果尚未更新现有的应用代码，可能会导致与 SD 卡的通信过程出现问题。如发现上述问题，请继续尝试配置与期望值接近的不同频率，直到找到合适的频率。如需查看底层驱动的计算结果以及实际应用的频率，请使用 ``void sdmmc_card_print_info(FILE* stream, const sdmmc_card_t* card)`` 函数。

FatFs
-----

FatFs 已更新至 v0.14， ``f_mkfs()`` 函数签名也已变更。新签名为 ``FRESULT f_mkfs (const TCHAR* path, const MKFS_PARM* opt, void* work, UINT len);``，使用 ``MKFS_PARM`` 结构体作为第二个实参。

分区表
---------------

分区表生成器不再支持未对齐的分区。生成分区表时， ``ESP-IDF`` 将只接受偏移量与 4 KB 对齐的分区。此变更仅影响新生成的分区表，不影响读写现有分区。


VFS
---

``esp_vfs_semihost_register()`` 函数签名有所更改：

- 新签名为 ``esp_err_t esp_vfs_semihost_register(const char* base_path);``
- 旧签名的 ``host_path`` 参数不再存在，请使用 OpenOCD 命令 ``ESP_SEMIHOST_BASEDIR`` 设置主机上的完整路径。

函数签名更改
^^^^^^^^^^^^^^^^^^^^^^^^^^

以下函数现将返回 ``esp_err_t``，而非 ``void`` 或 ``nvs_iterator_t``。此前，当参数无效或内部出现问题时，这些函数将 ``assert()`` 或返回 ``nullptr``。通过返回 ``esp_err_t``，你将获得更加实用的错误报告。

- :cpp:func:`nvs_entry_find`
- :cpp:func:`nvs_entry_next`
- :cpp:func:`nvs_entry_info`

由于 ``esp_err_t`` 返回类型的更改， ``nvs_entry_find()`` 和 ``nvs_entry_next()`` 的使用模式也发生了变化。上述函数现均通过参数修改迭代器，而非返回一个迭代器。

迭代 NVS 分区的旧编程模式如下所示：

.. code-block:: c

    nvs_iterator_t it = nvs_entry_find(<nvs_partition_name>, <namespace>, NVS_TYPE_ANY);
    while (it != NULL) {
            nvs_entry_info_t info;
            nvs_entry_info(it, &info);
            it = nvs_entry_next(it);
            printf("key '%s', type '%d'", info.key, info.type);
    };

现在，迭代 NVS 分区的编程模式已更新为：

.. code-block:: c

    nvs_iterator_t it = nullptr;
    esp_err_t res = nvs_entry_find(<nvs_partition_name>, <namespace>, NVS_TYPE_ANY, &it);
    while(res == ESP_OK) {
        nvs_entry_info_t info;
        nvs_entry_info(it, &info); // Can omit error check if parameters are guaranteed to be non-NULL
        printf("key '%s', type '%d'", info.key, info.type);
        res = nvs_entry_next(&it);
    }
    nvs_release_iterator(it);

迭代器有效性
^^^^^^^^^^^^^^^^^

请注意，由于函数签名的改动，如果存在参数错误，则可能从 ``nvs_entry_find()`` 获得无效迭代器。因此，请务必在使用 ``nvs_entry_find()`` 之前将迭代器初始化为 ``NULL``，以免在调用 ``nvs_release_iterator()`` 之前进行复杂的错误检查。上述编程模式便是一个很好的例子。


删除 SDSPI 弃用的 API
-------------------------------------

结构体 ``sdspi_slot_config_t`` 和函数 ``sdspi_host_init_slot()`` 现已删除，并由结构体 ``sdspi_device_config_t`` 和函数 ``sdspi_host_init_device()`` 替代。

ROM SPI flash
^^^^^^^^^^^^^

在 v5.0 之前的版本中，ROM SPI flash 函数一般通过 ``esp32**/rom/spi_flash.h`` 得以体现。因此，为支持不同 ESP 芯片而编写的代码可能会填充不同目标的 ROM 头文件。此外，并非所有 API 都可以在全部的 ESP 芯片上使用。

现在，常用 API 已提取至 ``esp_rom_spiflash.h``。尽管这不能算作重大变更，我们强烈建议仅使用此头文件中的函数（即以 ``esp_rom_spiflash`` 为前缀并包含在 ``esp_rom_spiflash.h`` 中），以获得不同 ESP 芯片之间更佳的交叉兼容性。

为了提高 ROM SPI flash API 的可读性，以下函数也进行了重命名：

- ``esp_rom_spiflash_lock()`` 更名为 ``esp_rom_spiflash_set_bp()``
- ``esp_rom_spiflash_unlock()`` 更名为 ``esp_rom_spiflash_clear_bp()``

SPI flash 驱动
^^^^^^^^^^^^^^^^^^^^^^

``esp_flash_speed_t`` ``enum`` 类型现已弃用。现在，可以直接将实际时钟频率值传递给 flash 配置结构。下为配置 80 MHz flash 频率的示例：

.. code-block:: c

    esp_flash_spi_device_config_t dev_cfg = {
        // Other members
        .freq_mhz = 80,
        // Other members
    };

旧版 SPI flash 驱动
^^^^^^^^^^^^^^^^^^^^^^^

为了使 SPI flash 驱动更为稳定，v5.0 已经删除旧版 SPI flash 驱动。旧版 SPI flash 驱动程序是指自 v3.0 以来的默认 SPI flash 驱动程序，以及自 v4.0 以来启用配置选项 ``CONFIG_SPI_FLASH_USE_LEGACY_IMPL`` 的 SPI flash 驱动。从 v5.0 开始，我们将不再支持旧版 SPI flash 驱动程序。因此，旧版驱动 API 和 ``CONFIG_SPI_FLASH_USE_LEGACY_IMPL`` 配置选项均已删除，请改用新 SPI flash 驱动的 API。

.. list-table::
    :widths: 50 50
    :header-rows: 1

    * - 删除项目
      - 替代项目
    * - ``spi_flash_erase_sector()``
      - ``esp_flash_erase_region()``
    * - ``spi_flash_erase_range()``
      - ``esp_flash_erase_region()``
    * - ``spi_flash_write()``
      - ``esp_flash_write()``
    * - ``spi_flash_read()``
      - ``esp_flash_read()``
    * - ``spi_flash_write_encrypted()``
      - ``esp_flash_write_encrypted()``
    * - ``spi_flash_read_encrypted()``
      - ``esp_flash_read_encrypted()``

.. note::

    带有前缀 ``esp_flash`` 的新函数接受额外的 ``esp_flash_t*`` 参数。你可以直接将其设置为 NULL，从而使函数运行主 flash (``esp_flash_default_chip``)。

由于系统函数不再是公共函数， ``esp_spi_flash.h`` 头文件已停止使用。若要使用 flash 映射 API，请使用 ``spi_flash_mmap.h``。
