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