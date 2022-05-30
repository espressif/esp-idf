Migrate Storage to ESP-IDF 5.0
==================================

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
