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
