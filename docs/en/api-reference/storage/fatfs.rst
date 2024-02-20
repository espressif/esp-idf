FAT Filesystem Support
======================

:link_to_translation:`zh_CN:[中文]`

ESP-IDF uses the `FatFs <http://elm-chan.org/fsw/ff/00index_e.html>`_ library to work with FAT filesystems. FatFs resides in the ``fatfs`` component. Although the library can be used directly, many of its features can be accessed via VFS using the C standard library and POSIX API functions.

Additionally, FatFs has been modified to support the runtime pluggable disk I/O layer. This allows mapping of FatFs drives to physical disks at runtime.


Using FatFs with VFS
--------------------

The header file :component_file:`fatfs/vfs/esp_vfs_fat.h` defines the functions for connecting FatFs and VFS.

The function :cpp:func:`esp_vfs_fat_register` allocates a ``FATFS`` structure and registers a given path prefix in VFS. Subsequent operations on files starting with this prefix are forwarded to FatFs APIs.

The function :cpp:func:`esp_vfs_fat_unregister_path` deletes the registration with VFS, and frees the ``FATFS`` structure.

Most applications use the following workflow when working with ``esp_vfs_fat_`` functions:

#. Call :cpp:func:`esp_vfs_fat_register` to specify:
    - Path prefix where to mount the filesystem (e.g., ``"/sdcard"``, ``"/spiflash"``)
    - FatFs drive number
    - A variable which receives the pointer to the ``FATFS`` structure

#. Call :cpp:func:`ff_diskio_register` to register the disk I/O driver for the drive number used in Step 1.

#. To mount the filesystem using the same drive number which was passed to :cpp:func:`esp_vfs_fat_register`, call the FatFs function :cpp:func:`f_mount`. If the filesystem is not present on the target logical drive, :cpp:func:`f_mount` will fail with the ``FR_NO_FILESYSTEM`` error. In such case, call :cpp:func:`f_mkfs` to create a fresh FatFS structure on the drive first, and then call :cpp:func:`f_mount` again. Note that SD cards need to be partitioned with :cpp:func:`f_fdisk` prior to previously described steps. For more information, see `FatFs documentation <http://elm-chan.org/fsw/ff/doc/mount.html>`_.

#. Call the C standard library and POSIX API functions to perform such actions on files as open, read, write, erase, copy, etc. Use paths starting with the path prefix passed to :cpp:func:`esp_vfs_register` (for example, ``"/sdcard/hello.txt"``). The filesystem uses `8.3 filenames <https://en.wikipedia.org/wiki/8.3_filename>`_ format (SFN) by default. If you need to use long filenames (LFN), enable the :ref:`CONFIG_FATFS_LONG_FILENAMES` option. Please refer to `FatFs filenames <http://elm-chan.org/fsw/ff/doc/filename.html>`_ for more details.

#. Optionally, call the FatFs library functions directly. In this case, use paths without a VFS prefix, for example, ``"/hello.txt"``.

#. Close all open files.

#. Call the FatFs function :cpp:func:`f_mount` for the same drive number with NULL ``FATFS*`` argument to unmount the filesystem.

#. Call the FatFs function :cpp:func:`ff_diskio_register` with NULL ``ff_diskio_impl_t*`` argument and the same drive number to unregister the disk I/O driver.

#. Call :cpp:func:`esp_vfs_fat_unregister_path` with the path where the file system is mounted to remove FatFs from VFS, and free the ``FATFS`` structure allocated in Step 1.

The convenience functions :cpp:func:`esp_vfs_fat_sdmmc_mount`, :cpp:func:`esp_vfs_fat_sdspi_mount`, and :cpp:func:`esp_vfs_fat_sdcard_unmount` wrap the steps described above and also handle SD card initialization. These functions are described in the next section.

.. note::

   Because FAT filesystem does not support hardlinks, :cpp:func:`link` copies contents of the file instead. (This only applies to files on FatFs volumes.)


Using FatFs with VFS and SD Cards
---------------------------------

The header file :component_file:`fatfs/vfs/esp_vfs_fat.h` defines convenience functions :cpp:func:`esp_vfs_fat_sdmmc_mount`, :cpp:func:`esp_vfs_fat_sdspi_mount`, and :cpp:func:`esp_vfs_fat_sdcard_unmount`. These functions perform Steps 1–3 and 7–9 respectively and handle SD card initialization, but provide only limited error handling. Developers are encouraged to check its source code and incorporate more advanced features into production applications.

The convenience function :cpp:func:`esp_vfs_fat_sdmmc_unmount` unmounts the filesystem and releases the resources acquired by :cpp:func:`esp_vfs_fat_sdmmc_mount`.


Using FatFs with VFS in Read-Only Mode
--------------------------------------

The header file :component_file:`fatfs/vfs/esp_vfs_fat.h` also defines the convenience functions :cpp:func:`esp_vfs_fat_spiflash_mount_ro` and :cpp:func:`esp_vfs_fat_spiflash_unmount_ro`. These functions perform Steps 1-3 and 7-9 respectively for read-only FAT partitions. These are particularly helpful for data partitions written only once during factory provisioning, which will not be changed by production application throughout the lifetime of the hardware.

Configuration options
---------------------

The following configuration options are available for the FatFs component:

* :ref:`CONFIG_FATFS_USE_FASTSEEK` - If enabled, the POSIX :cpp:func:`lseek` function will be performed faster. The fast seek does not work for files in write mode, so to take advantage of fast seek, you should open (or close and then reopen) the file in read-only mode.
* :ref:`CONFIG_FATFS_IMMEDIATE_FSYNC` - If enabled, the FatFs will automatically call :cpp:func:`f_sync` to flush recent file changes after each call of :cpp:func:`write`, :cpp:func:`pwrite`, :cpp:func:`link`, :cpp:func:`truncate` and :cpp:func:`ftruncate` functions. This feature improves file-consistency and size reporting accuracy for the FatFs, at a price on decreased performance due to frequent disk operations.
* :ref:`CONFIG_FATFS_LINK_LOCK` - If enabled, this option guarantees the API thread safety, while disabling this option might be necessary for applications that require fast frequent small file operations (e.g., logging to a file). Note that if this option is disabled, the copying performed by :cpp:func:`link` will be non-atomic. In such case, using :cpp:func:`link` on a large file on the same volume in a different task is not guaranteed to be thread safe.


FatFS Disk IO Layer
-------------------

FatFs has been extended with API functions that register the disk I/O driver at runtime.

These APIs provide implementation of disk I/O functions for SD/MMC cards and can be registered for the given FatFs drive number using the function :cpp:func:`ff_diskio_register_sdmmc`.

.. doxygenfunction:: ff_diskio_register
.. doxygenstruct:: ff_diskio_impl_t
    :members:
.. doxygenfunction:: ff_diskio_register_sdmmc
.. doxygenfunction:: ff_diskio_register_wl_partition
.. doxygenfunction:: ff_diskio_register_raw_partition


.. _fatfs-partition-generator:

FatFs Partition Generator
-------------------------

We provide a partition generator for FatFs (:component_file:`wl_fatfsgen.py <fatfs/wl_fatfsgen.py>`) which is integrated into the build system and could be easily used in the user project.

The tool is used to create filesystem images on a host and populate it with content of the specified host folder.

The script is based on the partition generator (:component_file:`fatfsgen.py <fatfs/fatfsgen.py>`). Apart from generating partition, it can also initialize wear levelling.

The latest version supports both short and long file names, FAT12 and FAT16. The long file names are limited to 255 characters and can contain multiple periods (``.``) characters within the filename and additional characters ``+``, ``,``, ``;``, ``=``, ``[`` and ``]``.

An in-depth description of the FatFs partition generator and analyzer can be found at :doc:`Generating and parsing FAT partition on host <fatfsgen>`.

Build System Integration with FatFs Partition Generator
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

It is possible to invoke FatFs generator directly from the CMake build system by calling ``fatfs_create_spiflash_image``::

    fatfs_create_spiflash_image(<partition> <base_dir> [FLASH_IN_PROJECT])

If you prefer generating partition without wear levelling support, you can use ``fatfs_create_rawflash_image``::

    fatfs_create_rawflash_image(<partition> <base_dir> [FLASH_IN_PROJECT])

``fatfs_create_spiflash_image`` respectively ``fatfs_create_rawflash_image`` must be called from project's CMakeLists.txt.

If you decide for any reason to use ``fatfs_create_rawflash_image`` (without wear levelling support), beware that it supports mounting only in read-only mode in the device.


The arguments of the function are as follows:

#. partition - the name of the partition as defined in the partition table (e.g., :example_file:`storage/fatfsgen/partitions_example.csv`).

#. base_dir - the directory that will be encoded to FatFs partition and optionally flashed into the device. Beware that you have to specify the suitable size of the partition in the partition table.

#. flag ``FLASH_IN_PROJECT`` - optionally, users can have the image automatically flashed together with the app binaries, partition tables, etc. on ``idf.py flash -p <PORT>`` by specifying ``FLASH_IN_PROJECT``.

#. flag ``PRESERVE_TIME`` - optionally, users can force preserving the timestamps from the source folder to the target image. Without preserving the time, every timestamp will be set to the FATFS default initial time (1st January 1980).

#. flag ``ONE_FAT`` - optionally, users can still choose to generate a FATFS volume with a single FAT (file allocation table) instead of two. This makes the free space in the FATFS volume a bit larger (by ``number of sectors used by FAT * sector size``) but also more prone to corruption.

For example::

    fatfs_create_spiflash_image(my_fatfs_partition my_folder FLASH_IN_PROJECT)

If FLASH_IN_PROJECT is not specified, the image will still be generated, but you will have to flash it manually using ``esptool.py`` or a custom build system target.

For an example, see :example:`storage/fatfsgen`.


FatFs Partition Analyzer
------------------------

(:component_file:`fatfsparse.py <fatfs/fatfsparse.py>`) is a partition analyzing tool for FatFs.

It is a reverse tool of (:component_file:`fatfsgen.py <fatfs/fatfsgen.py>`), i.e., it can generate the folder structure on the host based on the FatFs image.

Usage::

    ./fatfsparse.py [-h] [--wl-layer {detect,enabled,disabled}] [--verbose] fatfs_image.img

Parameter --verbose prints detailed information from boot sector of the FatFs image to the terminal before folder structure is generated.

High-level API Reference
------------------------

.. include-build-file:: inc/esp_vfs_fat.inc
