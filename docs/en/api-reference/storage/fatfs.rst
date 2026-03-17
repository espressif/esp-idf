FAT Filesystem Support
======================

:link_to_translation:`zh_CN:[中文]`

ESP-IDF uses the `FatFs <http://elm-chan.org/fsw/ff/00index_e.html>`_ library to work with FAT filesystems through the FatFs component. After a FAT filesystem volume is mounted, the VFS layer exposes standard C library and POSIX file APIs.

.. note::

    FatFs is an ambiguous term, as it refers to the filesystem itself, the upstream library, as well as the component in ESP-IDF. For clarity, this documentation uses the following terms:

    - **FAT filesystem**: the on-disk FAT format (FAT12/FAT16/FAT32); exFAT (see :ref:`Optional exFAT Support <fatfs-optional-exfat-support>`).
    - **FatFs library**: the upstream `FatFs library <http://elm-chan.org/fsw/ff/00index_e.html>`_ used by ESP-IDF.
    - **FatFs component**: the ESP-IDF integration layer around the FatFs library (mount helpers, wrappers, and related APIs) in :component:`fatfs`.

    Other terms relevant to this documentation are explained in the :ref:`Glossary <fatfs-glossary>` section.

.. _fatfs-mount-and-use:

Mount and Use FatFs
-------------------

The FatFs component provides convenience wrappers for mounting filesystems through the VFS layer. Use these wrappers for most use cases instead of mounting the filesystem manually. For manual setup instructions, refer to :ref:`Manually mounting a FAT filesystem partition <fatfs-manual-mount>`.

The general mounting workflow is:

#. Select a mount path (for example, ``"/spiflash"`` or ``"/sdcard"``) and set ``esp_vfs_fat_mount_config_t``.
#. Mount the volume with a convenience helper from :component_file:`fatfs/vfs/esp_vfs_fat.h`:

    - :cpp:func:`esp_vfs_fat_spiflash_mount_rw_wl` for SPI flash partitions with wear leveling
    - :cpp:func:`esp_vfs_fat_spiflash_mount_ro` for read-only SPI flash partitions **without** wear leveling
    - :cpp:func:`esp_vfs_fat_sdmmc_mount` or :cpp:func:`esp_vfs_fat_sdspi_mount` for SD cards

#. Use standard file APIs (``stdio.h`` or POSIX) on paths under the mount path.
#. Close open files and call the matching unmount helper.

.. _fatfs-configuration-options:

Configuration Options
---------------------

The following configuration options are available for the FatFs component:

* ``CONFIG_FATFS_LONG_FILENAMES`` - Selects how the FatFs library handles long filename (LFN) support. The available options are :ref:`CONFIG_FATFS_LFN_NONE <CONFIG_FATFS_LFN_NONE>` to disable LFN support and limit names to the `8.3 format <https://en.wikipedia.org/wiki/8.3_filename>`_ (SFN only), :ref:`CONFIG_FATFS_LFN_HEAP <CONFIG_FATFS_LFN_HEAP>` to enable LFN support with the LFN working buffer stored on the heap (default), and :ref:`CONFIG_FATFS_LFN_STACK <CONFIG_FATFS_LFN_STACK>` to enable LFN support with the LFN working buffer stored on the stack. For details, see `FatFs filenames <http://elm-chan.org/fsw/ff/doc/filename.html>`_.
* :ref:`CONFIG_FATFS_VOLUME_COUNT` - Sets the number of logical FatFs volumes. Increasing this value can increase baseline memory usage.
* :ref:`CONFIG_FATFS_ALLOC_PREFER_EXTRAM` - If enabled, the FatFs library prefers external RAM when allocating internal buffers. If external RAM allocation fails, it falls back to internal RAM. This can have a noticeable performance cost on hot I/O paths. Disable this option to prioritize performance; enable it to reduce internal RAM usage.
* :ref:`CONFIG_FATFS_ALLOC_PREFER_ALIGNED_WORK_BUFFERS` - If enabled, the FatFs library tries to allocate heap work buffers in DMA-capable, cache-aligned memory first so SDMMC transfers avoid extra copies. This option is useful on targets that use PSRAM with SDMMC DMA (for example ESP32-P4). If this option and :ref:`CONFIG_FATFS_ALLOC_PREFER_EXTRAM` are both enabled, the FatFs library tries DMA-capable RAM first, then external RAM, then internal RAM.
* :ref:`CONFIG_FATFS_USE_DYN_BUFFERS` - If enabled, the FatFs library allocates instance buffers separately and sizes them according to each mounted volume's logical sector size. This option is useful when multiple FatFs instances use different logical sector sizes, as it can reduce memory usage. If disabled, all instances use buffers sized for the largest configured logical sector size.
* :ref:`CONFIG_FATFS_PER_FILE_CACHE` - If enabled, each open file uses a separate cache buffer. This improves I/O performance but increases RAM usage when multiple files are open. If disabled, a single shared cache is used, which reduces RAM usage but can increase storage read/write operations.
* :ref:`CONFIG_FATFS_USE_FASTSEEK` - If enabled, POSIX :cpp:func:`lseek` runs faster. Fast seek does not work for files opened in write mode. To use fast seek, open the file in read-only mode, or close and reopen it in read-only mode.
* :ref:`CONFIG_FATFS_FAST_SEEK_BUFFER_SIZE` - Sets the CLMT (Cluster Link Map Table) buffer size used by fast seek when :ref:`CONFIG_FATFS_USE_FASTSEEK` is enabled. Larger buffers can improve seek behavior on larger files, but use more RAM.
* :ref:`CONFIG_FATFS_VFS_FSTAT_BLKSIZE` - Sets the default stdio file buffer block size used through VFS. This option is mainly relevant for stdio-based I/O (for example ``fread``/``fgets``) and is not the primary tuning knob for direct POSIX ``read``/``write`` paths. Larger values can improve buffered read throughput, but increase heap usage.
* :ref:`CONFIG_FATFS_IMMEDIATE_FSYNC` - If enabled, the FatFs library calls :cpp:func:`f_sync` automatically after each call to :cpp:func:`write`, :cpp:func:`pwrite`, :cpp:func:`link`, :cpp:func:`truncate`, and :cpp:func:`ftruncate`. This option improves file consistency and size-reporting accuracy, but decreases performance because it triggers frequent disk operations.
* :ref:`CONFIG_FATFS_LINK_LOCK` - If enabled, this option guarantees API thread safety for the :cpp:func:`link` function. Disabling this option can help applications that perform frequent small file operations (for example, file logging). When disabled, the copy performed by :cpp:func:`link` is non-atomic. In that case, using :cpp:func:`link` on a large file on the same volume from another task is not guaranteed to be thread-safe.
* Other relevant options include :ref:`CONFIG_FATFS_FS_LOCK`, :ref:`CONFIG_FATFS_TIMEOUT_MS`, and ``CONFIG_FATFS_CHOOSE_CODEPAGE`` (especially ``CONFIG_FATFS_CODEPAGE_DYNAMIC`` for code-size impact). Additional options include ``CONFIG_FATFS_SECTOR_SIZE``, ``CONFIG_FATFS_MAX_LFN``, ``CONFIG_FATFS_API_ENCODING``, and ``CONFIG_FATFS_USE_STRFUNC_CHOICE``.

These options control how the FatFs library calculates and reports free space:

* :ref:`CONFIG_FATFS_DONT_TRUST_FREE_CLUSTER_CNT` - If set to 1, the FatFs library ignores the free cluster count. The default value is 0.
* :ref:`CONFIG_FATFS_DONT_TRUST_LAST_ALLOC` - If set to 1, the FatFs library ignores the last allocation number. The default value is 0.

.. note::

    Setting these options to 1 can improve :cpp:func:`f_getfree` accuracy, but it reduces performance, for example by forcing a full FAT scan.

Differences from the POSIX Standard
-----------------------------------

#. :cpp:func:`link`: FAT filesystems do not support hard links, therefore :cpp:func:`link` copies file contents instead. (This applies only to files on FAT filesystem volumes.)
#. :cpp:func:`unlink`: If ``CONFIG_FATFS_FS_LOCK`` is enabled, removing an open file fails with ``EBUSY``. Otherwise, behavior is undefined and can corrupt the filesystem.
#. ``.`` and ``..`` are not supported as references to the current and parent directory.

.. _fatfs-formatting:

Formatting a FAT Filesystem
---------------------------

Formatting creates a new FAT filesystem on the target volume and erases the existing directory structure and file data on that volume. Use it for first-time provisioning, or to recover from ``FR_NO_FILESYSTEM``.

For mount-time auto-formatting, set ``esp_vfs_fat_mount_config_t.format_if_mount_failed`` to ``true`` before calling a mount helper. This is useful when the volume is expected to be blank on first boot. When formatting is triggered this way, the mount configuration also controls the new filesystem layout:

* ``allocation_unit_size`` sets the cluster size used by :cpp:func:`f_mkfs`. Larger values can improve throughput, but waste more space for many small files.
* ``use_one_fat`` creates one FAT instead of two. This saves some space, but reduces redundancy.

For explicit formatting after initialization, use the helper that matches the storage type:

* :cpp:func:`esp_vfs_fat_spiflash_format_rw_wl` or :cpp:func:`esp_vfs_fat_spiflash_format_cfg_rw_wl` for SPI flash partitions with wear leveling
* :cpp:func:`esp_vfs_fat_sdcard_format` or :cpp:func:`esp_vfs_fat_sdcard_format_cfg` for SD cards

The SPI flash formatting helpers can be called whether the filesystem is currently mounted or not. The SD card formatting helpers require the card (but not the filesystem) to be mounted first.

Read-only SPI flash partitions mounted with :cpp:func:`esp_vfs_fat_spiflash_mount_ro` cannot be reformatted on the target. To populate a read-only partition, generate the image on the host with :ref:`FatFs Partition Generator <fatfs-partition-generator>`.

.. _fatfs-partition-generator:

FatFs Partition Generator
-------------------------

ESP-IDF provides a FatFs partition generator, :component_file:`wl_fatfsgen.py <fatfs/wl_fatfsgen.py>`, which is integrated into the build system.

Use this tool to create filesystem images on a host and populate them from a specified host folder.

The script is based on the partition generator (:component_file:`fatfsgen.py <fatfs/fatfsgen.py>`). In addition to generating a FatFs partition, ``wl_fatfsgen.py`` initializes wear leveling, so it is typically used for writable SPI flash filesystems. Use ``fatfsgen.py`` instead for SD card images and read-only SPI flash images.

This tool supports short and long file names, FAT12, and FAT16. Long file names are limited to 255 characters and can contain multiple periods (``.``) in the file name, but not in the path. They can also contain the following additional characters: ``+``, ``,``, ``;``, ``=``, ``[`` and ``]``.

An in-depth description of the FatFs partition generator and analyzer can be found at :doc:`Generating and parsing FAT partition on host <fatfsgen>`.

Build System Integration with FatFs Partition Generator
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Invoke the FatFs generator directly from the CMake build system by calling ``fatfs_create_spiflash_image``::

    fatfs_create_spiflash_image(<partition> <base_dir> [optional arguments])

To generate a **read-only** partition without wear leveling support, use ``fatfs_create_rawflash_image``::

    fatfs_create_rawflash_image(<partition> <base_dir> [optional arguments])

Call ``fatfs_create_spiflash_image`` or ``fatfs_create_rawflash_image`` from the project's ``CMakeLists.txt``.

The function arguments are:

#. partition - Name of the partition as defined in the partition table (e.g., :example_file:`storage/fatfs/fatfsgen/partitions_example.csv`).

#. base_dir - Directory encoded into the FAT filesystem partition and optionally flashed to the target. Make sure the partition table defines an appropriate partition size.

#. flag ``FLASH_IN_PROJECT`` - Optionally flash the image automatically with app binaries, partition tables, and other outputs when you run ``idf.py flash -p <PORT>``. If ``FLASH_IN_PROJECT`` is not specified, the image is still generated, but you must flash it manually by using ``esptool`` or a custom build-system target.
#. flag ``PRESERVE_TIME`` - Optionally preserve timestamps from the source folder in the target image. Without this flag, the tool sets every timestamp to the FatFs library default initial time (1 January 1980).

#. flag ``ONE_FAT`` - Optionally generate a FAT filesystem volume with one FAT (File Allocation Table) instead of two. This increases free space in the FAT filesystem volume by ``number of logical sectors used by one FAT * logical sector size``, but it also increases corruption risk.

For example::

    fatfs_create_spiflash_image(my_fatfs_partition my_folder FLASH_IN_PROJECT)

If you generate an image for an SD card, ESP-IDF does not currently provide a host-side flashing step to write that image directly to the card. Use other host tooling, such as ``dd``, to deploy the image.

For an example project, see :example:`storage/fatfs/fatfsgen`.

.. _fatfs-partition-analyzer:

FatFs Partition Parser
----------------------

The :component_file:`fatfsparse.py <fatfs/fatfsparse.py>` tool performs the reverse operation of :component_file:`fatfsgen.py <fatfs/fatfsgen.py>`. It reconstructs a host folder structure from a FAT filesystem image.

Usage::

    ./fatfsparse.py [-h] [--wl-layer {detect,enabled,disabled}] [--verbose] fatfs_image.img

The ``--verbose`` option prints detailed information from the FAT filesystem image boot sector before the tool generates the folder structure.

.. _fatfs-minimum-partition-size:

Size Requirements and Limits
----------------------------

The FatFs component supports FAT12, FAT16, and FAT32 filesystem types, and can optionally support exFAT (see :ref:`Optional exFAT Support <fatfs-optional-exfat-support>`).
The filesystem type depends on the number of clusters on the volume:

.. math::

    cluster\_count = data\_logical\_sectors / logical\_sectors\_per\_cluster

The minimum partition size therefore depends on the number of logical sectors required by FAT metadata and by at least one data cluster. For the terminology used below, see :ref:`Glossary <fatfs-glossary>`.

.. _fatfs-minimum-partition-size-raw:

Raw FAT Filesystem Size
^^^^^^^^^^^^^^^^^^^^^^^

The required size of a raw FAT filesystem consists of the FAT filesystem metadata plus the data area:

.. math::

    fatfs_size = (reserved + fats + root\_dir + data) * logical\_sector\_size

Where:

* ``reserved`` is the number of reserved logical sectors
* ``fats`` is the total number of logical sectors occupied by all FAT copies
* ``root_dir`` is the number of logical sectors occupied by the root directory on FAT12/FAT16
* ``data`` is the number of logical sectors in the data area

The FAT area depends on the selected FAT type and the number of clusters:

.. list-table::
   :widths: 18 25 57
   :header-rows: 1

   * - FAT Type
     - Cluster Count
     - FAT Sizing
   * - FAT12
     - ``<= 4085``
     - Each FAT entry uses 12 bits, so one FAT copy requires ``ceil(((cluster_count + 2) * 3 / 2) / logical_sector_size)`` logical sectors.
   * - FAT16
     - ``4085 < cluster_count < 65526``
     - Each FAT entry uses 16 bits, so one FAT copy requires ``ceil(((cluster_count + 2) * 2) / logical_sector_size)`` logical sectors.
   * - FAT32
     - ``>= 65526``
     - Each FAT entry uses 32 bits, so one FAT copy requires ``ceil(((cluster_count + 2) * 4) / logical_sector_size)`` logical sectors.
   * - exFAT
     - ``16 .. 0x7FFFFFFD``
     - Each FAT entry uses 32 bits, so one FAT copy requires ``ceil(((cluster_count + 2) * 4) / logical_sector_size)`` logical sectors.

For FAT12 and FAT16, the root directory is outside the data area, so:

.. math::

    root\_dir = ceil((root\_dir\_entries * 32) / logical\_sector\_size)

For FAT32, the root directory is stored in the data area, so ``root_dir = 0`` in the formula above.

exFAT uses a different metadata layout. In addition to the FAT area, it requires an allocation bitmap and an up-case table, and its root directory is stored in the data area. As a result, the FAT12/FAT16 root-directory formula above does not apply to exFAT.

When exFAT support is enabled, the minimum exFAT volume size is 4096 logical sectors and the maximum cluster count is ``0x7FFFFFFD``.

In practice:

* Increasing the partition size adds data logical sectors and therefore increases usable storage space.
* On small FAT12/FAT16 volumes, the root directory can occupy 1 logical sector. On larger ones, it can occupy 4 logical sectors.
* For FAT12, FAT16, and FAT32, the default layout uses two FATs. Compared with ``use_one_fat = true``, this adds the logical sectors required by one extra FAT copy. exFAT uses a single FAT.
* For read-only partitions without wear leveling and with a 512-byte logical sector size, the practical minimum partition size is 2 KB. This minimum layout uses 4 logical sectors: 1 reserved sector, 1 FAT sector, 1 root-directory sector, and 1 data sector.

This minimum raw layout is relevant for media such as SD cards and read-only raw flash partitions.

.. _fatfs-minimum-partition-size-wl:

FAT Filesystem with Wear Leveling
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

For a flash partition with wear leveling, the total size is the raw FAT filesystem size from :ref:`Raw FAT Filesystem Size <fatfs-minimum-partition-size-raw>` plus the wear-leveling metadata area:

.. math::

    partition_size = wl_metadata_flash_sectors * flash_sector_size + fatfs_size

Where:

* ``wl_metadata_flash_sectors`` is the number of flash sectors reserved for wear-leveling metadata
* ``flash_sector_size`` is the SPI flash erase-sector size. On almost all ESP boards it is 4096 bytes
* ``fatfs_size`` is the raw FAT filesystem size, as defined in :ref:`Raw FAT Filesystem Size <fatfs-minimum-partition-size-raw>`

The minimum supported partition size depends on the WL configuration:

.. list-table::
   :widths: 28 18 54
   :header-rows: 1

   * - Condition
     - Minimum Size
     - Notes
   * - ``CONFIG_WL_SECTOR_SIZE = 4096``
     - 32 KB
     - The minimum layout consists of 4 flash sectors reserved by the wear leveling layer plus the minimum raw FAT filesystem layout of 4 logical sectors. In this configuration, the logical sector size and the flash-sector size are both 4096 bytes.
   * - ``CONFIG_WL_SECTOR_SIZE = 512`` and WL Performance mode
     - 20 KB
     - The raw FAT filesystem still needs the same minimum layout of 4 logical sectors, but the wear leveling layer keeps its metadata in 4096-byte flash sectors.
   * - ``CONFIG_WL_SECTOR_SIZE = 512`` and WL Safety mode
     - 28 KB
     - Same raw FAT minimum as above, plus 2 additional flash sectors reserved by the wear leveling layer.

For more details, see :doc:`File System Considerations <../../api-guides/file-system-considerations>`.

.. note::
    The wear leveling layer protects the underlying flash by distributing write and erase cycles across the managed storage region. Its effectiveness therefore scales with the size of that region. Small partitions restrict wear distribution and can significantly reduce protection, so their use is strongly discouraged. For more information, refer to :doc:`wear leveling <wear-levelling>`.

Optimizing
----------

FatFs behavior can be tuned for different priorities by choosing the right configuration options and filesystem layout.

The main variables that affect behavior are:

* Buffer placement and sizing (:ref:`CONFIG_FATFS_ALLOC_PREFER_ALIGNED_WORK_BUFFERS`, :ref:`CONFIG_FATFS_ALLOC_PREFER_EXTRAM`, :ref:`CONFIG_FATFS_USE_DYN_BUFFERS`, :ref:`CONFIG_FATFS_PER_FILE_CACHE`, and application I/O buffer sizes)
* Wear leveling logical sector size and mode (``CONFIG_WL_SECTOR_SIZE_*`` and ``CONFIG_WL_SECTOR_MODE_*``)
* Sync strategy (:ref:`CONFIG_FATFS_IMMEDIATE_FSYNC`)
* Workload pattern (transaction sizes, sequential vs random access, read vs write ratio)

Optimizing for I/O Performance
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

For throughput-oriented workloads:

* Keep :ref:`CONFIG_FATFS_IMMEDIATE_FSYNC` disabled unless your consistency requirements need it.
* If peak speed is the top priority, disable :ref:`CONFIG_FATFS_ALLOC_PREFER_EXTRAM` so buffers stay in internal RAM.
* Prefer larger read/write transaction sizes over many small operations.
* Align transaction sizes to the active sector size when possible (for example 512 B or 4096 B), and pad writes if needed to reduce partial-sector overhead.
* For SPI flash with wear leveling, prefer ``CONFIG_WL_SECTOR_SIZE_4096`` when RAM budget allows, as it is generally more efficient.
* If using 512-byte WL sectors, use ``CONFIG_WL_SECTOR_MODE_PERF`` when your application can accept the higher power-loss risk during flash-sector erase.
* Prefer POSIX ``read``/``write`` over ``fread``/``fwrite`` on hot paths when possible. For broader speed guidance, see :doc:`Maximizing Execution Speed <../../api-guides/performance/speed>`.
* On SDMMC DMA targets (for example ESP32-P4 with PSRAM), enable :ref:`CONFIG_FATFS_ALLOC_PREFER_ALIGNED_WORK_BUFFERS` to reduce extra buffer copies.
* Enable :ref:`CONFIG_FATFS_USE_FASTSEEK` for read-heavy workloads with long backward seeks.

.. note::
    For a baseline of what to expect in a performance-optimized scenario, see :example:`storage/perf_benchmark`.

Optimizing for memory usage
^^^^^^^^^^^^^^^^^^^^^^^^^^^

* Disable :ref:`CONFIG_FATFS_PER_FILE_CACHE` to use a single shared cache when reducing RAM usage is the priority.
* Tune ``esp_vfs_fat_mount_config_t.max_files`` (see :ref:`Mount and Use FatFs <fatfs-mount-and-use>`) as low as practical; each simultaneously open file increases RAM usage.
* If :ref:`CONFIG_FATFS_PER_FILE_CACHE` is enabled, prefer ``CONFIG_WL_SECTOR_SIZE_512`` to reduce per-file cache size.
* If :ref:`CONFIG_FATFS_PER_FILE_CACHE` is disabled, ``CONFIG_WL_SECTOR_SIZE_4096`` may be a better tradeoff.
* Enable :ref:`CONFIG_FATFS_ALLOC_PREFER_EXTRAM` on targets with external RAM support to reduce internal RAM pressure, but expect lower I/O performance.
* Consider ``CONFIG_FATFS_LONG_FILENAMES = CONFIG_FATFS_LFN_NONE`` when SFN (8.3) filenames are acceptable.
* If long filenames are required, reduce ``CONFIG_FATFS_MAX_LFN`` to the smallest value that meets your needs.
* Enable :ref:`CONFIG_FATFS_USE_DYN_BUFFERS` so each mounted volume uses buffers sized to its actual sector size.

Optimizing for storage efficiency
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

For maximizing usable filesystem capacity:

* For generated read-only images, consider the ``ONE_FAT`` option in ``fatfs_create_spiflash_image`` (see :ref:`FatFs Partition Generator <fatfs-partition-generator>`) to use one FAT instead of two.
* Consider ``CONFIG_WL_SECTOR_SIZE_512`` when minimizing overhead is a priority, as smaller sectors can improve usable storage efficiency in some layouts.
* Size partitions with margin. Very small partitions have proportionally higher metadata and wear leveling overhead.
* Keep long filenames short where possible; long names consume additional directory entries.
* For read-only data, consider raw FAT partitions without wear leveling where appropriate.

Advanced operations
-------------------

Manual mounting may be required when you use storage media that ESP-IDF does not support directly or when you need access to low-level FatFs library functions.

.. _fatfs-optional-exfat-support:

Optional exFAT Support
^^^^^^^^^^^^^^^^^^^^^^

The upstream FatFs library includes optional exFAT support, but ESP-IDF does not enable it by default.

If your project requires exFAT, you must modify the FatFs configuration in the :component:`fatfs` component manually and integrate the resulting configuration into your build and validation flow. ESP-IDF does not provide a dedicated menuconfig option or a standard high-level enablement path for exFAT.

Before enabling exFAT, review any licensing or other intellectual-property considerations that may apply to your product, distribution model, and target markets.

Architecture Overview
^^^^^^^^^^^^^^^^^^^^^

The following diagram illustrates the layered architecture of FatFs and its relationship with other ESP-IDF components:

::

    +---------------------------------------------------------------+
    |              Newlib/Picolib (C Standard Library)              |
    |           fopen, fread, fwrite, fclose, fseek, etc.           |
    +---------------------------------------------------------------+
    |                  VFS (Virtual File System)                    |
    |  POSIX API: open, read, write, close, lseek, stat, etc.       |
    +---------------------------------------------------------------+
    |                       FatFs Library                           |
    |       f_mount, f_open, f_read, f_write, f_close, f_mkfs       |
    +---------------------------------------------------------------+
    |                      Disk I/O Layer                           |
    |  ff_diskio_register, disk_read, disk_write, disk_ioctl, etc.  |
    +-------------------------------+-------------------------------+
    |   Wear Leveling  (optional)   |                               |
    |  wl_mount, wl_read, wl_write  |        SD/MMC Driver          |
    +-------------------------------+                               |
    |         esp_partition         |   sdmmc_read/write_sectors    |
    |    esp_partition_read/write   |                               |
    +-------------------------------+-------------------------------+
    |           SPI Flash           |           SD Card             |
    +-------------------------------+-------------------------------+

.. _fatfs-manual-mount:

Manually Mounting a FAT Filesystem Partition
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When mounting a FAT filesystem partition manually, follow the same general flow used by the convenience wrappers described in :ref:`Mount and Use FatFs <fatfs-mount-and-use>`: register the filesystem with the FatFs library, then mount it through the VFS.

#.
   Register the filesystem with the VFS by calling :cpp:func:`esp_vfs_fat_register`. This allocates the ``FATFS`` structure and sets up the interface between the VFS and the FatFs library. Specify:

    - The path prefix where the filesystem will be mounted (for example, ``"/sdcard"`` or ``"/spiflash"``)
    - The FatFs drive number
    - A variable that receives a pointer to the ``FATFS`` structure

#. Register the disk I/O driver for the drive number used in Step 1 by calling :cpp:func:`ff_diskio_register`.

#. Mount the filesystem for the drive number passed to :cpp:func:`esp_vfs_fat_register` by calling :cpp:func:`f_mount`.

   If no filesystem is present on the target logical drive, :cpp:func:`f_mount` fails with ``FR_NO_FILESYSTEM``. In that case, create a new FAT filesystem on the drive with :cpp:func:`f_mkfs`, and then call :cpp:func:`f_mount` again.

   For SD cards, create a partition table first with :cpp:func:`f_fdisk`. For details, see the `FatFs documentation <http://elm-chan.org/fsw/ff/doc/mount.html>`_.

To unmount, follow these general steps:

#. Unmount the filesystem by calling :cpp:func:`f_mount` with the same drive number and a NULL ``FATFS*`` argument.

#. Unregister the disk I/O driver by calling :cpp:func:`ff_diskio_register` with the same drive number and a NULL ``ff_diskio_impl_t*`` argument.

#. Remove the VFS registration and free the ``FATFS`` structure allocated in Step 1 of the mounting procedure by calling :cpp:func:`esp_vfs_fat_unregister_path` with the mount path.

When calling low-level functions provided by the FatFs library, use paths without the mount-point prefix. For example, use ``"/hello.txt"``.

.. _fatfs-diskio-layer:

FatFs Disk I/O Layer
^^^^^^^^^^^^^^^^^^^^

FatFs provides APIs to register disk I/O drivers at runtime. These APIs form the interface between medium-specific access functions and the FatFs library.

The disk I/O layer uses :cpp:struct:`ff_diskio_impl_t` to provide medium access functions. Pass this structure to :cpp:func:`ff_diskio_register`.

ESP-IDF provides convenience wrappers for the supported storage media. These include:

    - :cpp:func:`ff_diskio_register_wl_partition` for SPI flash partitions with wear leveling
    - :cpp:func:`ff_diskio_register_raw_partition` for read-only SPI flash partitions **without** wear leveling
    - :cpp:func:`ff_diskio_register_sdmmc` for both SDMMC and SPI connected SD cards

For API reference, see :ref:`Disk I/O API Reference <fatfs-diskio-api-reference>`.

.. _fatfs-glossary:

Glossary
--------

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Term
     - Description
   * - VFS
     - Virtual File System, an abstraction layer provided by ESP-IDF that allows applications to access different underlying filesystems through unified POSIX and ``stdio.h`` interfaces.
   * - Logical Sector
     - The logical block size presented to FatFs and the disk I/O layer. For wear-leveled SPI flash, this is the WL sector size. For read-only raw SPI flash partitions, this is the flash-sector size. For SD cards, it is typically 512 bytes.
   * - WL Sector
     - The logical sector size exposed by the wear leveling layer to FatFs. Depending on ``CONFIG_WL_SECTOR_SIZE``, it is either 512 bytes or 4096 bytes.
   * - Flash Sector
     - The underlying SPI flash erase unit used by the flash chip. On nearly all ESP boards, this is 4096 bytes. It can differ from the WL sector size presented to FatFs when wear leveling is configured for 512-byte sectors.
   * - Cluster
     - The minimum logical FAT allocation unit, consisting of one or more contiguous logical sectors.
   * - FAT
     - File Allocation Table, which records the usage status of each cluster and the linkage relationships between file data blocks.
   * - Wear Leveling
     - A technique to extend the lifespan of flash memory by distributing write operations evenly across storage cells, preventing specific blocks from wearing out prematurely due to frequent erasing.
   * - Drive Number
     - A number (0-9) used by FatFs to identify different logical drives, each corresponding to an independent filesystem instance.
   * - Mount
     - The process of associating a filesystem with a specified path, enabling applications to access the filesystem contents through that path.
   * - SFN/LFN
     - Short File Name (8.3 format) and Long File Name (up to 255 characters).

.. _fatfs-application-examples:

Application Examples
--------------------

- :example:`storage/fatfs/getting_started` demonstrates the minimal setup required to store persistent data on SPI flash with FatFs, including mounting the filesystem, opening a file, performing basic read and write operations, and unmounting the filesystem.

- :example:`storage/fatfs/fs_operations` demonstrates advanced FatFs operations, including reading and writing files, creating, moving, and deleting files and directories, and inspecting file details.

- :example:`storage/fatfs/ext_flash` demonstrates how to operate an external SPI flash formatted with FatFs, including initializing the SPI bus, configuring the flash chip, registering it as a partition, and performing read and write operations.

.. _fatfs-high-level-api-reference:

High-level API Reference
------------------------

.. include-build-file:: inc/esp_vfs_fat.inc

.. _fatfs-diskio-api-reference:

Disk I/O API Reference
^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfunction:: ff_diskio_register
.. doxygenstruct:: ff_diskio_impl_t
    :members:
.. doxygenfunction:: ff_diskio_register_sdmmc
.. doxygenfunction:: ff_diskio_register_wl_partition
.. doxygenfunction:: ff_diskio_register_raw_partition
