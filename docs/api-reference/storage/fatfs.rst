FAT Filesystem Support
======================

ESP-IDF uses `FatFs <http://elm-chan.org/fsw/ff/00index_e.html>`_ library to work with FAT filesystems. FatFs library resides in ``fatfs`` component. Although it can be used directly, many of its features can be accessed via VFS using C standard library and POSIX APIs.

Additionally, FatFs has been modified to support run-time pluggable disk IO layer. This allows mapping of FatFs drives to physical disks at run-time.

Using FatFs with VFS
--------------------

``esp_vfs_fat.h`` header file defines functions to connect FatFs with VFS. ``esp_vfs_fat_register`` function allocates a ``FATFS`` structure, and registers a given path prefix in VFS. Subsequent operations on files starting with this prefix are forwarded to FatFs APIs. ``esp_vfs_fat_unregister_path`` function deletes the registration with VFS, and frees the ``FATFS`` structure.

Most applications will use the following flow when working with ``esp_vfs_fat_`` functions:

1. Call ``esp_vfs_fat_register``, specifying path prefix where the filesystem has to be mounted (e.g. ``"/sdcard"``, ``"/spiflash"``), FatFs drive number, and a variable which will receive a pointer to ``FATFS`` structure.

2. Call ``ff_diskio_register`` function to register disk IO driver for the drive number used in step 1.

3. Call ``f_mount`` function (and optionally ``f_fdisk``, ``f_mkfs``) to mount the filesystem using the same drive number which was passed to ``esp_vfs_fat_register``. See FatFs documentation for more details.

4. Call POSIX and C standard library functions to open, read, write, erase, copy files, etc. Use paths starting with the prefix passed to ``esp_vfs_register`` (such as ``"/sdcard/hello.txt"``).

5. Optionally, call FatFs library functions directly. Use paths without a VFS prefix in this case (``"/hello.txt"``).

6. Close all open files.

7. Call ``f_mount`` function for the same drive number, with NULL ``FATFS*`` argument, to unmount the filesystem.

8. Call ``ff_diskio_register`` with NULL ``ff_diskio_impl_t*`` argument and the same drive number.

9. Call ``esp_vfs_fat_unregister_path`` with the path where the file system is mounted to remove FatFs from VFS, and free the ``FATFS`` structure allocated on step 1.

Convenience functions, ``esp_vfs_fat_sdmmc_mount`` and ``esp_vfs_fat_sdmmc_unmount``, which wrap these steps and also handle SD card initialization, are described in the next section. 

.. doxygenfunction:: esp_vfs_fat_register
.. doxygenfunction:: esp_vfs_fat_unregister_path


Using FatFs with VFS and SD cards
---------------------------------

``esp_vfs_fat.h`` header file also provides a convenience function to perform steps 1–3 and 7–9, and also handle SD card initialization: ``esp_vfs_fat_sdmmc_mount``. This function does only limited error handling. Developers are encouraged to look at its source code and incorporate more advanced versions into production applications. ``esp_vfs_fat_sdmmc_unmount`` function unmounts the filesystem and releases resources acquired by ``esp_vfs_fat_sdmmc_mount``.

.. doxygenfunction:: esp_vfs_fat_sdmmc_mount

.. doxygenstruct:: esp_vfs_fat_mount_config_t
    :members:

.. doxygenfunction:: esp_vfs_fat_sdmmc_unmount

FatFS disk IO layer
-------------------

FatFs has been extended with an API to register disk IO driver at runtime.

Implementation of disk IO functions for SD/MMC cards is provided. It can be registered for the given FatFs drive number using ``ff_diskio_register_sdmmc`` function.

.. doxygenfunction:: ff_diskio_register

.. doxygenstruct:: ff_diskio_impl_t
    :members:

.. doxygenfunction:: ff_diskio_register_sdmmc

