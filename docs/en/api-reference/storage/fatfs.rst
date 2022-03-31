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

3. Call the FatFs function ``f_mount``, and optionally ``f_fdisk``, ``f_mkfs``, to mount the filesystem using the same drive number which was passed to :cpp:func:`esp_vfs_fat_register`. For more information, see `FatFs documentation <http://www.elm-chan.org/fsw/ff/doc/mount.html>`_.

4. Call the C standard library and POSIX API functions to perform such actions on files as open, read, write, erase, copy, etc. Use paths starting with the path prefix passed to :cpp:func:`esp_vfs_register` (for example, ``"/sdcard/hello.txt"``). The filesystem uses `8.3 filenames <https://en.wikipedia.org/wiki/8.3_filename>`_ format (SFN) by default. If you need to use long filenames (LFN), enable the :ref:`CONFIG_FATFS_LONG_FILENAMES` option. More details on the FatFs filenames are available `here <http://elm-chan.org/fsw/ff/doc/filename.html>`_.

5. Optionally, by enabling the option :ref:`CONFIG_FATFS_USE_FASTSEEK`, use the POSIX lseek function to perform it faster, the fast seek will not work for files in write mode, so to take advantage of fast seek, you should open (or close and then reopen) the file in read-only mode.

6. Optionally, call the FatFs library functions directly. In this case, use paths without a VFS prefix (for example, ``"/hello.txt"``).

7. Close all open files.

8. Call the FatFs function ``f_mount`` for the same drive number, with NULL ``FATFS*`` argument, to unmount the filesystem.

9. Call the FatFs function :cpp:func:`ff_diskio_register` with NULL ``ff_diskio_impl_t*`` argument and the same drive number to unregister the disk I/O driver.

10. Call :cpp:func:`esp_vfs_fat_unregister_path` with the path where the file system is mounted to remove FatFs from VFS, and free the ``FATFS`` structure allocated in Step 1.

The convenience functions ``esp_vfs_fat_sdmmc_mount``, ``esp_vfs_fat_sdspi_mount`` and ``esp_vfs_fat_sdcard_unmount`` wrap the steps described above and also handle SD card initialization. These two functions are described in the next section.

.. doxygenfunction:: esp_vfs_fat_register
.. doxygenfunction:: esp_vfs_fat_unregister_path


Using FatFs with VFS and SD cards
---------------------------------

The header file :component_file:`fatfs/vfs/esp_vfs_fat.h` defines convenience functions :cpp:func:`esp_vfs_fat_sdmmc_mount`, :cpp:func:`esp_vfs_fat_sdspi_mount` and :cpp:func:`esp_vfs_fat_sdcard_unmount`. These function perform Steps 1–3 and 7–9 respectively and handle SD card initialization, but provide only limited error handling. Developers are encouraged to check its source code and incorporate more advanced features into production applications.

The convenience function :cpp:func:`esp_vfs_fat_sdmmc_unmount` unmounts the filesystem and releases the resources acquired by :cpp:func:`esp_vfs_fat_sdmmc_mount`.

.. doxygenfunction:: esp_vfs_fat_sdmmc_mount
.. doxygenfunction:: esp_vfs_fat_sdmmc_unmount
.. doxygenfunction:: esp_vfs_fat_sdspi_mount
.. doxygenstruct:: esp_vfs_fat_mount_config_t
    :members:
.. doxygenfunction:: esp_vfs_fat_sdcard_unmount


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


FATFS partition generator
-------------------------

We provide a partition generator for FATFS (:component_file:`wl_fatfsgen.py<fatfs/wl_fatfsgen.py>`) which is integrated into the build system and could be easily used in the user project.

The tool is used to create filesystem images on a host and populate it with content of the specified host folder.

The script is based on the partition generator (:component_file:`fatfsgen.py<fatfs/fatfsgen.py>`) and except for generating partition also initializes wear levelling.

The latest version supports both short and long file names, FAT12 and FAT16. The long file names are limited to 255 characters, and can contain multiple period (".") characters within the filename and additional characters "+", ",", ";", "=", "[" and also "]". The long files name characters are encoded using utf-16, on the contrary to short file names encoded using utf-8.

Build system integration with FATFS partition generator
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

It is possible to invoke FATFS generator directly from the CMake build system by calling ``fatfs_create_spiflash_image``::

    fatfs_create_spiflash_image(<partition> <base_dir> [FLASH_IN_PROJECT])

If you prefer generating partition without wear levelling support you can use ``fatfs_create_rawflash_image``::

    fatfs_create_rawflash_image(<partition> <base_dir> [FLASH_IN_PROJECT])

``fatfs_create_spiflash_image`` respectively ``fatfs_create_rawflash_image`` must be called from project's CMakeLists.txt.

If you decided because of any reason to use ``fatfs_create_rawflash_image`` (without wear levelling support), beware that it supports mounting only in read-only mode in the device.


The arguments of the function are as follows:

1. partition - the name of the partition as defined in the partition table (e.g. :example_file:`storage/fatfsgen/partitions_example.csv`).

2. base_dir - the directory that will be encoded to FATFS partition and optionally flashed into the device. Beware that you have to specified suitable size of the partition in the partition table.

3. flag ``FLASH_IN_PROJECT`` - optionally, user can opt to have the image automatically flashed together with the app binaries, partition tables, etc. on ``idf.py flash -p <PORT>`` by specifying ``FLASH_IN_PROJECT``.

For example::

    fatfs_create_spiflash_image(my_fatfs_partition my_folder FLASH_IN_PROJECT)

If FLASH_IN_PROJECT is not specified, the image will still be generated, but you will have to flash it manually using ``esptool.py`` or a custom build system target.

For an example, see :example:`storage/fatfsgen`.
