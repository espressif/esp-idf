FAT Filesystem Support
======================

:link_to_translation:`zh_CN:[中文]`

ESP-IDF uses the `FatFs <http://elm-chan.org/fsw/ff/00index_e.html>`_ library to work with FAT filesystems. FatFs resides in the ``fatfs`` component. Although the library can be used directly, many of its features can be accessed via VFS, using the C standard library and POSIX API functions.

Additionally, FatFs has been modified to support the runtime pluggable disk I/O layer. This allows mapping of FatFs drives to physical disks at runtime.


Using FatFs with VFS
--------------------

The header file :component_file:`fatfs/vfs/esp_vfs_fat.h` defines the functions for connecting FatFs and VFS.

The function :cpp:func:`esp_vfs_fat_register` allocates a ``FATFS`` structure and registers a given path prefix in VFS. Subsequent operations on files starting with this prefix are forwarded to FatFs APIs.
The function :cpp:func:`esp_vfs_fat_unregister_path` deletes the registration with VFS, and frees the ``FATFS`` structure.

Most applications use the following workflow when working with ``esp_vfs_fat_`` functions:

1. Call :cpp:func:`esp_vfs_fat_register` to specify:
    - Path prefix where to mount the filesystem (e.g. ``"/sdcard"``, ``"/spiflash"``)
    - FatFs drive number
    - A variable which will receive the pointer to the ``FATFS`` structure

2. Call :cpp:func:`ff_diskio_register` to register the disk I/O driver for the drive number used in Step 1.

3. Call the FatFs function ``f_mount``, and optionally ``f_fdisk``, ``f_mkfs``, to mount the filesystem using the same drive number which was passed to :cpp:func:`esp_vfs_fat_register`. For more information, see `FatFs documentation <http://www.elm-chan.org/fsw/ff/doc/mount.html>`.

4. Call the C standard library and POSIX API functions to perform such actions on files as open, read, write, erase, copy, etc. Use paths starting with the path prefix passed to :cpp:func:`esp_vfs_register` (for example, ``"/sdcard/hello.txt"``).

5. Optionally, call the FatFs library functions directly. In this case, use paths without a VFS prefix (for example, ``"/hello.txt"``).

6. Close all open files.

7. Call the FatFs function ``f_mount`` for the same drive number, with NULL ``FATFS*`` argument, to unmount the filesystem.

8. Call the FatFs function :cpp:func:`ff_diskio_register` with NULL ``ff_diskio_impl_t*`` argument and the same drive number to unregister the disk I/O driver.

9. Call :cpp:func:`esp_vfs_fat_unregister_path` with the path where the file system is mounted to remove FatFs from VFS, and free the ``FATFS`` structure allocated in Step 1.

The convenience functions ``esp_vfs_fat_sdmmc_mount`` and ``esp_vfs_fat_sdmmc_unmount`` wrap the steps described above and also handle SD card initialization. These two functions are described in the next section. 

.. doxygenfunction:: esp_vfs_fat_register
.. doxygenfunction:: esp_vfs_fat_unregister_path


Using FatFs with VFS and SD cards
---------------------------------

The header file :component_file:`fatfs/vfs/esp_vfs_fat.h` defines convenience functions :cpp:func:`esp_vfs_fat_sdmmc_mount` and :cpp:func:`esp_vfs_fat_sdmmc_unmount`. These function perform Steps 1–3 and 7–9 respectively and handle SD card initialization, but provide only limited error handling. Developers are encouraged to check its source code and incorporate more advanced features into production applications.

The convenience function :cpp:func:`esp_vfs_fat_sdmmc_unmount` unmounts the filesystem and releases the resources acquired by :cpp:func:`esp_vfs_fat_sdmmc_mount`.

.. doxygenfunction:: esp_vfs_fat_sdmmc_mount
.. doxygenstruct:: esp_vfs_fat_mount_config_t
    :members:
.. doxygenfunction:: esp_vfs_fat_sdmmc_unmount


Using FatFs with VFS in read-only mode
--------------------------------------

The header file :component_file:`fatfs/vfs/esp_vfs_fat.h` also defines the convenience functions :cpp:func:`esp_vfs_fat_rawflash_mount` and :cpp:func:`esp_vfs_fat_rawflash_unmount`. These functions perform Steps 1-3 and 7-9 respectively for read-only FAT partitions. These are particularly helpful for data partitions written only once during factory provisioning which will not be changed by production application throughout the lifetime of the hardware.

.. doxygenfunction:: esp_vfs_fat_rawflash_mount
.. doxygenfunction:: esp_vfs_fat_rawflash_unmount


FatFS disk IO layer
-------------------

FatFs has been extended with API functions that register the disk I/O driver at runtime.

They provide implementation of disk I/O functions for SD/MMC cards and can be registered for the given FatFs drive number using the function :cpp:func:`ff_diskio_register_sdmmc`.

.. doxygenfunction:: ff_diskio_register
.. doxygenstruct:: ff_diskio_impl_t
    :members:
.. doxygenfunction:: ff_diskio_register_sdmmc
.. doxygenfunction:: ff_diskio_register_wl_partition
.. doxygenfunction:: ff_diskio_register_raw_partition


