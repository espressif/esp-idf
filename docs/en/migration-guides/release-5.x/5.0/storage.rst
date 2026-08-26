Storage
=======

New Component for the Partition APIs
------------------------------------

Breaking change: all the Partition API code has been moved to a new component :component:`esp_partition`. For the complete list of affected functions and data-types, see header file :component_file:`esp_partition.h <esp_partition/include/esp_partition.h>`.

These API functions and data-types were previously a part of the :component:`spi_flash` component, and thus possible dependencies on the ``spi_flash`` in existing applications may cause the build failure, in case of direct esp_partition_* APIs/data-types use (for instance, ``fatal error: esp_partition.h: No such file or directory`` at lines with ``#include "esp_partition.h"``). If you encounter such an issue, please update your project's CMakeLists.txt file as follows:

Original dependency setup:

.. code-block:: cmake

   idf_component_register(...
                          REQUIRES spi_flash)

Updated dependency setup:

.. code-block:: cmake

   idf_component_register(...
                          REQUIRES spi_flash esp_partition)

.. note::

   Please update relevant ``REQUIRES`` or ``PRIV_REQUIRES`` section according to your project. The above-presented code snippet is just an example.

If the issue persists, please let us know and we will assist you with your code migration.


SDMMC/SDSPI
-----------

SD card frequency on SDMMC/SDSPI interface can be now configured through ``sdmmc_host_t.max_freq_khz`` to a specific value, not only ``SDMMC_FREQ_PROBING`` (400 kHz), ``SDMMC_FREQ_DEFAULT`` (20 MHz), or ``SDMMC_FREQ_HIGHSPEED`` (40 MHz). Previously, in case you have specified a custom frequency other than any of the above-mentioned values, the closest lower-or-equal one was selected anyway.

Now, the underlying drivers calculate the nearest fitting value, given by available frequency dividers instead of an enumeration item selection. This could cause troubles in communication with your SD card without a change of the existing application code.If you encounter such an issue, please, keep trying different frequencies around your desired value unless you find the one working well. To check the frequency value calculated and actually applied, use ``void sdmmc_card_print_info(FILE* stream, const sdmmc_card_t* card)`` function.

FatFs
-----

FatFs is now updated to v0.14. As a result, the function signature of ``f_mkfs()`` has changed. The new signature is ``FRESULT f_mkfs (const TCHAR* path, const MKFS_PARM* opt, void* work, UINT len);`` which uses ``MKFS_PARM`` struct as a second argument.

Partition Table
---------------

The partition table generator no longer supports misaligned partitions. When generating a partition table, ``ESP-IDF`` only accepts partitions with offsets that align to 4 KB. This change only affects generating new partition tables. Reading and writing to already existing partitions remains unchanged.


VFS
---

The ``esp_vfs_semihost_register()`` function signature is changed as follows:

- The new signature is ``esp_err_t esp_vfs_semihost_register(const char* base_path);``
- The ``host_path`` parameter of the old signature no longer exists. Instead, the OpenOCD command ``ESP_SEMIHOST_BASEDIR`` should be used to set the full path on the host.

Function Signature Changes
^^^^^^^^^^^^^^^^^^^^^^^^^^

The following functions now return ``esp_err_t`` instead of ``void`` or ``nvs_iterator_t``. Previously, when parameters were invalid or when something goes wrong internally, these functions would ``assert()`` or return a ``nullptr``. With an ``esp_err_t`` returned, you can get better error reporting.

- :cpp:func:`nvs_entry_find`
- :cpp:func:`nvs_entry_next`
- :cpp:func:`nvs_entry_info`

Because the ``esp_err_t`` return type changes, the usage patterns of ``nvs_entry_find()`` and ``nvs_entry_next()`` become different. Both functions now modify iterators via parameters instead of returning an iterator.

The old programming pattern to iterate over an NVS partition was as follows:

.. code-block:: c

    nvs_iterator_t it = nvs_entry_find(<nvs_partition_name>, <namespace>, NVS_TYPE_ANY);
    while (it != NULL) {
            nvs_entry_info_t info;
            nvs_entry_info(it, &info);
            it = nvs_entry_next(it);
            printf("key '%s', type '%d'", info.key, info.type);
    };

The new programming pattern to iterate over an NVS partition is now:

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

Iterator Validity
^^^^^^^^^^^^^^^^^

Note that because the function signature changes, if there is a parameter error, you may get an invalid iterator from ``nvs_entry_find()``. Hence, it is important to initialize the iterator to ``NULL`` before using ``nvs_entry_find()``, so that you can avoid complex error checking before calling ``nvs_release_iterator()``. A good example is the programming pattern above.


Removed SDSPI Deprecated API
----------------------------

Structure ``sdspi_slot_config_t`` and function ``sdspi_host_init_slot()`` are removed, and replaced by structure ``sdspi_device_config_t`` and function ``sdspi_host_init_device()`` respectively.

ROM SPI Flash
^^^^^^^^^^^^^

In versions before v5.0, ROM SPI flash functions were included via ``esp32**/rom/spi_flash.h``. Thus, code written to support different ESP chips might be filled with ROM headers of different targets. Furthermore, not all of the APIs could be used on all ESP chips.

Now, the common APIs are extracted to ``esp_rom_spiflash.h``. Although it is not a breaking change, you are strongly recommended to only use the functions from this header (i.e., prefixed with ``esp_rom_spiflash`` and included by ``esp_rom_spiflash.h``) for better cross-compatibility between ESP chips.

To make ROM SPI flash APIs clearer, the following functions are also renamed:

- ``esp_rom_spiflash_lock()`` to ``esp_rom_spiflash_set_bp()``
- ``esp_rom_spiflash_unlock()`` to ``esp_rom_spiflash_clear_bp()``

SPI Flash Driver
^^^^^^^^^^^^^^^^

The ``esp_flash_speed_t`` ``enum`` type is now deprecated. Instead, you may now directly pass the real clock frequency value to the flash configuration structure. The following example demonstrates how to configure a flash frequency of 80MHz:

.. code-block:: c

    esp_flash_spi_device_config_t dev_cfg = {
        // Other members
        .freq_mhz = 80,
        // Other members
    };

Legacy SPI Flash Driver
^^^^^^^^^^^^^^^^^^^^^^^

To make SPI flash drivers more stable, the legacy SPI flash driver is removed from v5.0. The legacy SPI flash driver refers to default spi_flash driver since v3.0, and the SPI flash driver with configuration option ``CONFIG_SPI_FLASH_USE_LEGACY_IMPL`` enabled since v4.0. The major breaking change here is that the legacy spi_flash driver is no longer supported from v5.0. Therefore, the legacy driver APIs and the ``CONFIG_SPI_FLASH_USE_LEGACY_IMPL`` configuration option are both removed. Please use the new spi_flash driver's APIs instead.

.. list-table::
    :widths: 50 50
    :header-rows: 1

    * - Removed items
      - Replacement
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

    New functions with prefix ``esp_flash`` accept an additional ``esp_flash_t*`` parameter. You can simply set it to NULL. This will make the function to run the main flash (``esp_flash_default_chip``).

The ``esp_spi_flash.h`` header is deprecated as system functions are no longer public. To use flash memory mapping APIs, you may include ``spi_flash_mmap.h`` instead.
