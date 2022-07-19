Storage
=======

Breaking changes:
~~~~~~~~~~~~~~~~~

f_mkfs() signature change in FATFS v0.14
----------------------------------------

New signature is ``FRESULT f_mkfs (const TCHAR* path, const MKFS_PARM* opt, void* work, UINT len);`` which now uses ``MKFS_PARM`` struct as a second argument.

Partition table generation no longer supports misaligned partitions
-------------------------------------------------------------------

When generating a partiton table, ``esp-idf`` will no longer accept partitions which offset does not align to 4kB. This change only affects generating new partition tables, reading and writing to already existing partitions remains unchanged.

esp_vfs_semihost_register() signature change
--------------------------------------------

New signature is ``esp_err_t esp_vfs_semihost_register(const char* base_path);`` Absolute path as a second parameter will no longer in use. Instead, the OpenOCD command ``ESP_SEMIHOST_BASEDIR`` should be used to set the full path on the host.

NVS
---

``nvs_entry_find()``, ``nvs_entry_next()`` and ``nvs_entry_info()`` always return ``esp_err_t`` now instead of ``void`` or ``nvs_iterator_t``. This provides better error reporting when parameters are invalid or something goes wrong internally than returning ``nullptr`` instead of a valid iterator or checking parameters with ``assert``. ``nvs_entry_find()`` and ``nvs_entry_next()`` modify iterators via parameters now instead of returning an iterator.

The old programming pattern to iterate over an NVS partition was as follows:

.. highlight:: c

::

    nvs_iterator_t it = nvs_entry_find(<nvs_partition_name>, <namespace>, NVS_TYPE_ANY);
    while (it != NULL) {
            nvs_entry_info_t info;
            nvs_entry_info(it, &info);
            it = nvs_entry_next(it);
            printf("key '%s', type '%d' \n", info.key, info.type);
    };

The new programming pattern to iterate over an NVS partition is now:

.. highlight:: c

::

    nvs_iterator_t it = nullptr;
    esp_err_t res = nvs_entry_find(<nvs_partition_name>, <namespace>, NVS_TYPE_ANY, &it);
    while(res == ESP_OK) {
        nvs_entry_info_t info;
        nvs_entry_info(it, &info); // Can omit error check if parameters are guaranteed to be non-NULL
        printf("key '%s', type '%d' \n", info.key, info.type);
        res = nvs_entry_next(&it);
    }
    nvs_release_iterator(it);

Signature Changes
^^^^^^^^^^^^^^^^^

``nvs_iterator_t nvs_entry_find(const char *part_name, const char *namespace_name, nvs_type_t type)`` changes to ``esp_err_t nvs_entry_find(const char *part_name, const char *namespace_name, nvs_type_t type, nvs_iterator_t *output_iterator)``. The iterator is returned via the parameter ``output_iterator`` instead of a return value. This allows reporting additional errors, like e.g. memory errors, via the new return value.

``nvs_iterator_t nvs_entry_next(nvs_iterator_t iterator)`` changes to ``esp_err_t nvs_entry_next(nvs_iterator_t *it)``. This allows reporting parameter errors and internal errors, like e.g. flash errors.

``void nvs_entry_info(nvs_iterator_t iterator, nvs_entry_info_t *out_info)`` changes to ``esp_err_t nvs_entry_info(const nvs_iterator_t iterator, nvs_entry_info_t *out_info)`` to allow reporting parameter errors.

Iterator Validity
^^^^^^^^^^^^^^^^^

Note that due to the new signatures, it is possible to have an invalid iterator from ``nvs_entry_find()``, if there is a parameter errors. Hence, it is important to initialize the iterator with ``NULL`` before using ``nvs_entry_find()`` to avoid complex error checking before calling ``nvs_release_iterator()``. A good example is the programming pattern above.

Removed SDSPI deprecated API
----------------------------

Removed structure ``sdspi_slot_config_t`` and fuction ``sdspi_host_init_slot``. These were replaced by a structure ``sdspi_device_config_t`` and a fuction ``sdspi_host_init_device`` respectively.

SPI Flash Interface
-------------------

Version before v5.0, spi flash functions in rom can be included by ``esp32**/rom/spi_flash.h``. However, your code written for different chips may be filled with ROM headers of different versions. At the meantime not all the APIs can be used on all chips.

Therefore, the common APIs are extracted to ``esp_rom_spiflash.h``. Although it's not a breaking change, it is strongly recommended to only use the functions with prefix ``esp_rom_spiflash`` included by ``esp_rom_spiflash.h`` for better cross-compatibility.

To make the API clearer, we renamed the function ``esp_rom_spiflash_lock`` to ``esp_rom_spiflash_set_bp``. We renamed ``esp_rom_spiflash_unlock`` to ``esp_rom_spiflash_clear_bp``.

ENUM type ``esp_flash_speed_t`` has been deprecated. From now on, you can directly parse the real clock frequency value to the flash initialization structure. For example, if you want the flash frequency is 80M, you can write the code like:

.. code:: c

    esp_flash_spi_device_config_t dev_cfg = {
        // Other members
        .freq_mhz = 80,
        // Other members
    };

Breaking changes in legacy APIs
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In order to make spi_flash driver more stable, legacy spi_flash driver is removed on v5.0. Legacy spi_flash driver refers to default spi_flash driver since v3.0 and spi_flash driver with configuration option ``CONFIG_SPI_FLASH_USE_LEGACY_IMPL`` switched on on v4.0 series. The major breaking change is legacy spi_flash driver is not supported on new version anymore. Therefore, the configuration option ``CONFIG_SPI_FLASH_USE_LEGACY_IMPL`` is removed. After that, following functions will no longer exist. But meanwhile, you can use our new APIs instead.

+---------------------------------+-------------------------------+
|         Removed items           |          Replacement          |
+=================================+===============================+
| ``spi_flash_erase_sector()``    | ``esp_flash_erase_region``    |
+---------------------------------+-------------------------------+
| ``spi_flash_erase_range()``     | ``esp_flash_erase_region``    |
+---------------------------------+-------------------------------+
| ``spi_flash_write``             | ``esp_flash_write``           |
+---------------------------------+-------------------------------+
| ``spi_flash_read()``            | ``esp_flash_read``            |
+---------------------------------+-------------------------------+
| ``spi_flash_write_encrypted()`` | ``esp_flash_write_encrypted`` |
+---------------------------------+-------------------------------+
| ``spi_flash_read_encrypted``    | ``esp_flash_read_encrypted``  |
+---------------------------------+-------------------------------+

.. note::

    New functions with prefix ``esp_flash`` accept an additional ``esp_flash_t*`` parameter.  You can simply set it to NULL means that the function will operate the main flash(``esp_flash_default_chip``)

Header ``esp_spi_flash.h`` has been deprecated, system functions are no longer public. To make use of flash memory mapping APIs, you should include ``spi_flash_mmap.h`` instead.
