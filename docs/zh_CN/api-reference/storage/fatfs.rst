FAT 文件系统
======================

:link_to_translation:`en:[English]`

ESP-IDF 通过 FatFs 组件使用 `FatFs <http://elm-chan.org/fsw/ff/00index_e.html>`_ 库来实现 FAT 文件系统。挂载 FAT 文件系统卷后，VFS 层会暴露标准 C 库和 POSIX API。

.. note::

    FatFs 可用于指代多个概念，如文件系统本身、上游库，或 ESP-IDF 组件。为清晰起见，本文档使用以下术语：

    - **FAT 文件系统**：磁盘上的 FAT 格式 (FAT12/FAT16/FAT32)，exFAT（参见 :ref:`可选的 exFAT 支持 <fatfs-optional-exfat-support>`）。
    - **FatFs 库**：ESP-IDF 使用的上游 `FatFs 库 <http://elm-chan.org/fsw/ff/00index_e.html>`_。
    - **FatFs 组件**：ESP-IDF 中围绕 FatFs 库的集成层（挂载辅助函数、封装器和相关 API），位于 :component:`fatfs`。

    本文档相关的其他术语在 :ref:`术语表 <fatfs-glossary>` 中进行了解释。

.. _fatfs-mount-and-use:

挂载和使用 FatFs
----------------

FatFs 组件提供了便利的封装函数，用于通过 VFS 层挂载文件系统。在大多数用例中，应使用这些封装函数，而不是手动挂载文件系统。有关手动设置的说明，请参考 :ref:`手动挂载 FAT 文件系统分区 <fatfs-manual-mount>`。

一般的挂载流程是：

#. 选择挂载路径（例如 ``"/spiflash"`` 或 ``"/sdcard"``），并设置 ``esp_vfs_fat_mount_config_t``。
#. 使用 :component_file:`fatfs/vfs/esp_vfs_fat.h` 中的便利辅助函数挂载卷：

    - :cpp:func:`esp_vfs_fat_spiflash_mount_rw_wl` 用于带磨损均衡的 SPI flash 分区。
    - :cpp:func:`esp_vfs_fat_spiflash_mount_ro` 用于 **不带** 磨损均衡的只读 SPI flash 分区。
    - :cpp:func:`esp_vfs_fat_sdmmc_mount` 或 :cpp:func:`esp_vfs_fat_sdspi_mount` 用于 SD 卡。

#. 在挂载路径下的路径上使用标准文件 API（``stdio.h`` 或 POSIX）。
#. 关闭打开的文件，并调用相应的卸载辅助函数。

.. _fatfs-configuration-options:

配置选项
--------------------

FatFs 组件提供以下配置选项：

* ``CONFIG_FATFS_LONG_FILENAMES`` - 选择 FatFs 库如何处理长文件名 (LFN) 支持。可用选项包括 :ref:`CONFIG_FATFS_LFN_NONE <CONFIG_FATFS_LFN_NONE>` 以禁用 LFN 支持并将名称限制为 `8.3 格式 <https://en.wikipedia.org/wiki/8.3_filename>`_ （仅 SFN）， :ref:`CONFIG_FATFS_LFN_HEAP <CONFIG_FATFS_LFN_HEAP>` 以启用 LFN 支持并将 LFN 工作缓冲区存储在堆上（默认），以及 :ref:`CONFIG_FATFS_LFN_STACK <CONFIG_FATFS_LFN_STACK>` 以启用 LFN 支持并将 LFN 工作缓冲区存储在栈上。详细信息请参阅 `FatFs 文件名 <http://elm-chan.org/fsw/ff/doc/filename.html>`_。
* :ref:`CONFIG_FATFS_VOLUME_COUNT` - 设置逻辑 FatFs 卷的数量。增加此值可能会增加基础内存使用量。
* :ref:`CONFIG_FATFS_ALLOC_PREFER_EXTRAM` - 如果启用，FatFs 库在分配内部缓冲区时优先使用外部 RAM。如果外部 RAM 分配失败，则回退到内部 RAM。这可能会对热 I/O 路径产生明显的性能开销。禁用此选项可优先考虑性能；启用可减少内部 RAM 使用量。
* :ref:`CONFIG_FATFS_ALLOC_PREFER_ALIGNED_WORK_BUFFERS` - 如果启用，FatFs 库首先尝试在支持 DMA、缓存对齐的内存中分配堆工作缓冲区，以便 SDMMC 传输避免额外的拷贝。此选项在使用 PSRAM 和 SDMMC DMA 的目标芯片上（例如 ESP32-P4）非常有用。如果同时启用此选项和 :ref:`CONFIG_FATFS_ALLOC_PREFER_EXTRAM`，FatFs 库会先尝试支持 DMA 的 RAM，然后是外部 RAM，最后是内部 RAM。
* :ref:`CONFIG_FATFS_USE_DYN_BUFFERS` - 如果启用，FatFs 库会单独分配实例缓冲区，并根据每个已挂载卷的逻辑扇区大小调整其大小。当多个 FatFs 实例使用不同的逻辑扇区大小时，此选项非常有用，因为它可以减少内存使用量。如果禁用，所有实例都使用为最大配置逻辑扇区大小调整大小的缓冲区。
* :ref:`CONFIG_FATFS_PER_FILE_CACHE` - 如果启用，每个打开的文件使用单独的缓存缓冲区。这提高了 I/O 性能，但当多个文件打开时会增加 RAM 使用量。如果禁用，则使用单个共享缓存，这减少了 RAM 使用量，但可能会增加存储读写操作。
* :ref:`CONFIG_FATFS_USE_FASTSEEK` - 如果启用，POSIX :cpp:func:`lseek` 运行更快。快速定位不适用于以写入模式打开的文件。要使用快速查找，请以只读模式打开文件，或关闭后以只读模式重新打开。
* :ref:`CONFIG_FATFS_FAST_SEEK_BUFFER_SIZE` - 设置当 :ref:`CONFIG_FATFS_USE_FASTSEEK` 启用时快速查找使用的 CLMT（簇链接映射表）缓冲区大小。较大的缓冲区可以改善较大文件上的查找行为，但会使用更多 RAM。
* :ref:`CONFIG_FATFS_VFS_FSTAT_BLKSIZE` - 设置通过 VFS 使用的默认 stdio 文件缓冲区块大小。此选项主要与基于 stdio 的 I/O（例如 ``fread``/``fgets``）相关，不是直接 POSIX ``read``/``write`` 路径的主要调整参数。较大的值可以提高缓冲读取吞吐量，但会增加堆使用量。
* :ref:`CONFIG_FATFS_IMMEDIATE_FSYNC` - 如果启用，FatFs 库会在每次调用 :cpp:func:`write`、:cpp:func:`pwrite`、:cpp:func:`link`、:cpp:func:`truncate` 和 :cpp:func:`ftruncate` 后自动调用 :cpp:func:`f_sync`。此选项提高了文件一致性和大小报告的准确性，但由于会触发频繁的磁盘操作而降低了性能。
* :ref:`CONFIG_FATFS_LINK_LOCK` - 如果启用，此选项保证 :cpp:func:`link` 函数的 API 线程安全。禁用此选项可以帮助执行频繁小文件操作（例如文件日志记录）的应用程序。禁用时，:cpp:func:`link` 执行的拷贝是非原子的。在这种情况下，从另一个任务在同一卷上对大文件使用 :cpp:func:`link` 不保证线程安全。
* 其他相关选项包括 :ref:`CONFIG_FATFS_FS_LOCK`、:ref:`CONFIG_FATFS_TIMEOUT_MS` 和 ``CONFIG_FATFS_CHOOSE_CODEPAGE`` （特别是 ``CONFIG_FATFS_CODEPAGE_DYNAMIC`` 对代码大小的影响）。其他选项包括 ``CONFIG_FATFS_SECTOR_SIZE``、``CONFIG_FATFS_MAX_LFN``、``CONFIG_FATFS_API_ENCODING`` 和 ``CONFIG_FATFS_USE_STRFUNC_CHOICE``。

这些选项控制 FatFs 库如何计算和报告可用空间：

* :ref:`CONFIG_FATFS_DONT_TRUST_FREE_CLUSTER_CNT` - 如果设置为 1，FatFs 库忽略空闲簇计数。默认值为 0。
* :ref:`CONFIG_FATFS_DONT_TRUST_LAST_ALLOC` - 如果设置为 1，FatFs 库忽略上次分配编号。默认值为 0。

.. note::

    将这些选项设置为 1 可以提高 :cpp:func:`f_getfree` 的准确性，但会降低性能，例如强制进行完整的 FAT 扫描。

与 POSIX 标准的差异
-----------------------------------

#. :cpp:func:`link`：FAT 文件系统不支持硬链接，因此 :cpp:func:`link` 会复制文件内容（这仅适用于 FAT 文件系统卷上的文件）。
#. :cpp:func:`unlink`：如果启用 ``CONFIG_FATFS_FS_LOCK``，删除打开的文件会失败并返回 ``EBUSY``。否则，行为未定义，可能会损坏文件系统。
#. 不支持将 ``.`` 和 ``..`` 作为当前目录和上级目录的引用。

.. _fatfs-formatting:

格式化 FAT 文件系统
---------------------------

格式化会在目标卷上创建一个新的 FAT 文件系统，并擦除该卷上现有的目录结构和文件数据。用于首次配置，或从 ``FR_NO_FILESYSTEM`` 中恢复。

对于挂载时自动格式化，请在调用挂载辅助函数之前将 ``esp_vfs_fat_mount_config_t.format_if_mount_failed`` 设置为 ``true``。当卷在首次启动时预期为空白时，这非常有用。当以这种方式触发格式化时，挂载配置还控制新文件系统的布局：

* ``allocation_unit_size`` 设置 :cpp:func:`f_mkfs` 使用的簇大小。较大的值可以提高吞吐量，但对于许多小文件会浪费更多空间。
* ``use_one_fat`` 创建一个 FAT 而不是两个。这节省了一些空间，但降低了冗余性。

对于初始化后的显式格式化，请使用与存储类型匹配的辅助函数：

* :cpp:func:`esp_vfs_fat_spiflash_format_rw_wl` 或 :cpp:func:`esp_vfs_fat_spiflash_format_cfg_rw_wl` 用于带磨损均衡的 SPI flash 分区。
* :cpp:func:`esp_vfs_fat_sdcard_format` 或 :cpp:func:`esp_vfs_fat_sdcard_format_cfg` 用于 SD 卡。

无论当前是否已挂载文件系统，都可以调用 SPI flash 格式化辅助函数。SD 卡格式化辅助函数要求先挂载卡（但不是文件系统）。

使用 :cpp:func:`esp_vfs_fat_spiflash_mount_ro` 挂载的只读 SPI flash 分区不能在目标上重新格式化。要填充只读分区，请在主机上使用 :ref:`FatFs 分区生成器 <fatfs-partition-generator>` 生成镜像。

.. _fatfs-partition-generator:

FatFs 分区生成器
-------------------------

ESP-IDF 提供了一个 FatFs 分区生成器 :component_file:`wl_fatfsgen.py <fatfs/wl_fatfsgen.py>`，该工具已集成到构建系统中。

使用此工具在主机上创建文件系统镜像，并从指定的主机文件夹填充它们。

该脚本基于分区生成器 (:component_file:`fatfsgen.py <fatfs/fatfsgen.py>`)。除了生成 FatFs 分区外，``wl_fatfsgen.py`` 还初始化磨损均衡，因此通常用于可写的 SPI flash 文件系统。对于 SD 卡镜像和只读 SPI flash 镜像，请改用 ``fatfsgen.py``。

此工具支持短文件名和长文件名、FAT12 和 FAT16。长文件名最多 255 个字符，文件名中可以包含多个点（``.``），但路径中不能包含。它们还可以包含以下附加字符：``+``、``,``、``;``、``=``、``[`` 和 ``]``。

有关 FatFs 分区生成器和分析器的详细说明，请参阅 :doc:`在主机上生成和解析 FAT 分区 <fatfsgen>`。

与 FatFs 分区生成器的构建系统集成
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

通过调用 ``fatfs_create_spiflash_image`` 直接从 CMake 构建系统调用 FatFs 生成器：

.. code-block:: none

    fatfs_create_spiflash_image(<partition> <base_dir> [optional arguments])

要生成不带磨损均衡支持的 **只读** 分区，请使用 ``fatfs_create_rawflash_image``：

.. code-block:: none

    fatfs_create_rawflash_image(<partition> <base_dir> [optional arguments])

在项目的 ``CMakeLists.txt`` 中调用 ``fatfs_create_spiflash_image`` 或 ``fatfs_create_rawflash_image``。

函数参数为：

#. partition - 分区表中定义的分区名称（例如，:example_file:`storage/fatfs/fatfsgen/partitions_example.csv`）。

#. base_dir - 编码到 FAT 文件系统分区中并可选择烧录到目标的目录。确保分区表定义了适当的分区大小。

#. 标志 ``FLASH_IN_PROJECT`` - 可选择在运行 ``idf.py flash -p <PORT>`` 时自动将镜像与应用程序二进制文件、分区表和其他输出一起烧录。如果未指定 ``FLASH_IN_PROJECT``，镜像仍然会生成，但你必须使用 ``esptool`` 或自定义构建系统目标手动烧录。
#. 标志 ``PRESERVE_TIME`` - 可选择保留目标镜像中源文件夹的时间戳。如果没有此标志，该工具将所有时间戳设置为 FatFs 库默认初始时间（1980 年 1 月 1 日）。

#. 标志 ``ONE_FAT`` - 可选择生成具有一个 FAT（文件分配表）而不是两个的 FAT 文件系统卷。这通过 ``一个 FAT 使用的逻辑扇区数 * 逻辑扇区大小`` 增加了 FAT 文件系统卷中的可用空间，但也增加了损坏风险。

例如：

.. code-block:: none

    fatfs_create_spiflash_image(my_fatfs_partition my_folder FLASH_IN_PROJECT)

如果你为 SD 卡生成镜像，ESP-IDF 目前不提供主机端烧录步骤来将镜像直接写入卡。请使用其他主机工具（例如 ``dd``）来部署镜像。

有关示例项目，请参阅 :example:`storage/fatfs/fatfsgen`。

.. _fatfs-partition-analyzer:

FatFs 分区解析器
----------------------

:component_file:`fatfsparse.py <fatfs/fatfsparse.py>` 工具执行 :component_file:`fatfsgen.py <fatfs/fatfsgen.py>` 的逆向操作。它从 FAT 文件系统镜像重建主机文件夹结构。

用法：

.. code-block:: none

    ./fatfsparse.py [-h] [--wl-layer {detect,enabled,disabled}] [--verbose] fatfs_image.img

``--verbose`` 选项在工具生成文件夹结构之前打印 FAT 文件系统镜像引导扇区的详细信息。

.. _fatfs-minimum-partition-size:

大小要求和限制
----------------------------

FatFs 组件支持 FAT12、FAT16 和 FAT32 文件系统类型，并且可以选择支持 exFAT（参见 :ref:`可选的 exFAT 支持 <fatfs-optional-exfat-support>`）。

文件系统类型取决于卷上的簇数：

.. math::

    cluster\_count = data\_logical\_sectors / logical\_sectors\_per\_cluster

因此，最小分区大小取决于 FAT 元数据和至少一个数据簇所需的逻辑扇区数。对于下面使用的术语，请参见 :ref:`术语表 <fatfs-glossary>`。

.. _fatfs-minimum-partition-size-raw:

原始 FAT 文件系统大小
^^^^^^^^^^^^^^^^^^^^^^^

原始 FAT 文件系统的所需大小包括 FAT 文件系统元数据加上数据区：

.. math::

    fatfs\_size = (reserved + fats + root\_dir + data) × logical\_sector\_size

其中：

* ``reserved`` 是保留的逻辑扇区数
* ``fats`` 是所有 FAT 副本占用的逻辑扇区总数
* ``root_dir`` 是 FAT12/FAT16 上根目录占用的逻辑扇区数
* ``data`` 是数据区中的逻辑扇区数

FAT 区域取决于所选的 FAT 类型和簇数：

.. list-table::
   :widths: 18 25 57
   :header-rows: 1

   * - FAT 类型
     - 簇数
     - FAT 大小计算
   * - FAT12
     - ``<= 4085``
     - 每个 FAT 条目使用 12 位，因此一个 FAT 副本需要 ``ceil(((cluster_count + 2) × 3 / 2) / logical_sector_size)`` 个逻辑扇区。
   * - FAT16
     - ``4085 < cluster_count < 65526``
     - 每个 FAT 条目使用 16 位，因此一个 FAT 副本需要 ``ceil(((cluster_count + 2) × 2) / logical_sector_size)`` 个逻辑扇区。
   * - FAT32
     - ``>= 65526``
     - 每个 FAT 条目使用 32 位，因此一个 FAT 副本需要 ``ceil(((cluster_count + 2) × 4) / logical_sector_size)`` 个逻辑扇区。
   * - exFAT
     - ``16 .. 0x7FFFFFFD``
     - 每个 FAT 条目使用 32 位，因此一个 FAT 副本需要 ``ceil(((cluster_count + 2) × 4) / logical_sector_size)`` 个逻辑扇区。

对于 FAT12 和 FAT16，根目录在数据区之外，因此：

.. math::

    root\_dir = ceil((root\_dir\_entries × 32) / logical\_sector\_size)

对于 FAT32，根目录存储在数据区中，因此上述公式中 ``root_dir = 0``。

exFAT 使用不同的元数据布局。除了 FAT 区域外，它还需要一个分配位图和一个大小写转换表，其根目录存储在数据区中。因此，上述 FAT12/FAT16 根目录公式不适用于 exFAT。

当启用 exFAT 支持时，最小 exFAT 卷大小为 4096 个逻辑扇区，最大簇数为 ``0x7FFFFFFD``。

在实践中：

* 增加分区大小会添加数据逻辑扇区，从而增加可用存储空间。
* 在小的 FAT12/FAT16 卷上，根目录可以占用 1 个逻辑扇区。在较大的卷上，它可以占用 4 个逻辑扇区。
* 对于 FAT12、FAT16 和 FAT32，默认布局使用两个 FAT。与 ``use_one_fat = true`` 相比，这增加了一个额外 FAT 副本所需的逻辑扇区。exFAT 使用单个 FAT。
* 对于不带磨损均衡且逻辑扇区大小为 512 字节的只读分区，实际最小分区大小为 2 KB。此最小布局使用 4 个逻辑扇区：1 个保留扇区、1 个 FAT 扇区、1 个根目录扇区和 1 个数据扇区。

此最小原始布局适用于 SD 卡和只读原始 flash 分区等介质。

.. _fatfs-minimum-partition-size-wl:

带磨损均衡的 FAT 文件系统
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

对于带磨损均衡的 flash 分区，总大小为 :ref:`原始 FAT 文件系统大小 <fatfs-minimum-partition-size-raw>` 加上磨损均衡元数据区：

.. math::

    partition\_size = wl\_metadata\_flash\_sectors × flash\_sector\_size + fatfs\_size

其中：

* ``wl_metadata_flash_sectors`` 是为磨损均衡元数据保留的 flash 扇区数
* ``flash_sector_size`` 是 SPI flash 擦除扇区大小。在几乎所有 ESP 开发板上，它是 4096 字节
* ``fatfs_size`` 是原始 FAT 文件系统大小，如 :ref:`原始 FAT 文件系统大小 <fatfs-minimum-partition-size-raw>` 中所定义

最小支持的分区大小取决于 WL 配置：

.. list-table::
   :widths: 28 18 54
   :header-rows: 1

   * - 条件
     - 最小大小
     - 说明
   * - ``CONFIG_WL_SECTOR_SIZE = 4096``
     - 32 KB
     - 最小布局由磨损均衡层保留的 4 个 flash 扇区加上最小原始 FAT 文件系统布局的 4 个逻辑扇区组成。在此配置中，逻辑扇区大小和 flash 扇区大小均为 4096 字节。
   * - ``CONFIG_WL_SECTOR_SIZE = 512`` 且 WL 性能模式
     - 20 KB
     - 原始 FAT 文件系统仍然需要相同的最小布局的 4 个逻辑扇区，但磨损均衡层将其元数据保留在 4096 字节的 flash 扇区中。
   * - ``CONFIG_WL_SECTOR_SIZE = 512`` 且 WL 安全模式
     - 28 KB
     - 与上述原始 FAT 最小值相同，加上磨损均衡层保留的 2 个额外 flash 扇区。

有关更多详细信息，请参阅 :doc:`文件系统注意事项 <../../api-guides/file-system-considerations>`。

.. note::

    磨损均衡层通过在受管存储区域中均匀分布写入和擦除周期来保护底层 flash。因此，其有效性随该区域的大小而缩放。小分区会限制磨损分布，并可能显著降低保护效果，因此强烈不建议使用。有关更多信息，请参阅 :doc:`磨损均衡 <wear-levelling>`。

优化
----------

通过选择正确的配置选项和文件系统布局，可以针对不同的优先级调整 FatFs 行为。

影响行为的主要变量是：

* 缓冲区位置和大小（:ref:`CONFIG_FATFS_ALLOC_PREFER_ALIGNED_WORK_BUFFERS`、:ref:`CONFIG_FATFS_ALLOC_PREFER_EXTRAM`、:ref:`CONFIG_FATFS_USE_DYN_BUFFERS`、:ref:`CONFIG_FATFS_PER_FILE_CACHE` 和应用程序 I/O 缓冲区大小）。
* 磨损均衡逻辑扇区大小和模式（``CONFIG_WL_SECTOR_SIZE_*`` 和 ``CONFIG_WL_SECTOR_MODE_*``）。
* 同步策略（:ref:`CONFIG_FATFS_IMMEDIATE_FSYNC`）。
* 工作负载模式（事务大小、顺序访问与随机访问、读取与写入比率）。

优化 I/O 性能
^^^^^^^^^^^^^^^^^^^^^^

对于面向吞吐量的工作负载：

* 除非一致性要求需要，否则保持禁用 :ref:`CONFIG_FATFS_IMMEDIATE_FSYNC`。
* 如果峰值速度是最高优先级，请禁用 :ref:`CONFIG_FATFS_ALLOC_PREFER_EXTRAM`，以便缓冲区保留在内部 RAM 中。
* 优先选择较大的读写事务大小，而不是许多小操作。
* 尽可能将事务大小与活动扇区大小（例如 512 B 或 4096 B）对齐，并在需要时填充写入以减少部分扇区开销。
* 对于带磨损均衡的 SPI flash，当 RAM 预算允许时，优先选择 ``CONFIG_WL_SECTOR_SIZE_4096``，因为它通常更高效。
* 如果使用 512 字节的 WL 扇区，当应用程序可以接受 flash 扇区擦除期间更高的断电风险时，请使用 ``CONFIG_WL_SECTOR_MODE_PERF``。
* 在热路径上尽可能优先选择 POSIX ``read``/``write`` 而不是 ``fread``/``fwrite``。有关更广泛的速度指导，请参阅 :doc:`最大化执行速度 <../../api-guides/performance/speed>`。
* 在 SDMMC DMA 目标（例如带有 PSRAM 的 ESP32-P4）上，启用 :ref:`CONFIG_FATFS_ALLOC_PREFER_ALIGNED_WORK_BUFFERS` 以减少额外的缓冲区拷贝。
* 对于具有长反向查找的读取密集型工作负载，启用 :ref:`CONFIG_FATFS_USE_FASTSEEK`。

.. note::

    有关性能优化场景下预期基准的参考，请参阅 :example:`storage/perf_benchmark`。

优化内存使用
^^^^^^^^^^^^^^^^^^^

* 当减少 RAM 使用是优先事项时，禁用 :ref:`CONFIG_FATFS_PER_FILE_CACHE` 以使用单个共享缓存。
* 尽可能低地调整 ``esp_vfs_fat_mount_config_t.max_files`` （参见 :ref:`挂载和使用 FatFs <fatfs-mount-and-use>`）；每个同时打开的文件都会增加 RAM 使用量。
* 如果启用了 :ref:`CONFIG_FATFS_PER_FILE_CACHE`，优先选择 ``CONFIG_WL_SECTOR_SIZE_512`` 以减少每文件缓存大小。
* 如果禁用了 :ref:`CONFIG_FATFS_PER_FILE_CACHE`，``CONFIG_WL_SECTOR_SIZE_4096`` 可能是更好的权衡。
* 在支持外部 RAM 的目标上启用 :ref:`CONFIG_FATFS_ALLOC_PREFER_EXTRAM` 以减少内部 RAM 压力，但预计 I/O 性能会降低。
* 当 SFN (8.3) 文件名可接受时，考虑使用 ``CONFIG_FATFS_LONG_FILENAMES = CONFIG_FATFS_LFN_NONE``。
* 如果需要长文件名，将 ``CONFIG_FATFS_MAX_LFN`` 减小到满足需求的最小值。
* 启用 :ref:`CONFIG_FATFS_USE_DYN_BUFFERS`，以便每个挂载的卷使用根据其实际扇区大小调整大小的缓冲区。

优化存储效率
^^^^^^^^^^^^^^^^^^^^^^^^^

为了最大化可用文件系统容量：

* 对于生成的只读镜像，考虑在 ``fatfs_create_spiflash_image`` 中使用 ``ONE_FAT`` 选项（参见 :ref:`FatFs 分区生成器 <fatfs-partition-generator>`）以使用一个 FAT 而不是两个。
* 当最小化开销是优先事项时，考虑使用 ``CONFIG_WL_SECTOR_SIZE_512``，因为在某些布局中，较小的扇区可以提高可用存储效率。
* 为分区留有余量。非常小的分区具有不成比例的高元数据和磨损均衡开销。
* 尽可能保持长文件名简短；长名称会消耗额外的目录条目。
* 对于只读数据，在适当的情况下考虑不带磨损均衡的原始 FAT 分区。

高级操作
-------------------

当你使用 ESP-IDF 不直接支持的存储介质，或者当你需要访问底层 FatFs 库函数时，可能需要手动挂载。

.. _fatfs-optional-exfat-support:

可选的 exFAT 支持
^^^^^^^^^^^^^^^^^^^^^^

上游 FatFs 库包括可选的 exFAT 支持，但 ESP-IDF 默认不启用它。

如果你的项目需要 exFAT，你必须在 :component:`fatfs` 组件中手动修改 FatFs 配置，并将生成的配置集成到你的构建和验证流程中。ESP-IDF 不提供专门的 menuconfig 选项或标准的 exFAT 高级启用路径。

在启用 exFAT 之前，请审查可能适用于你的产品、分销模式和目标市场的任何许可或其他知识产权考虑因素。

架构概述
^^^^^^^^^^^^^^^^^^^^^

下图说明了 FatFs 的分层架构及其与 ESP-IDF 其他组件的关系：

.. code-block:: none

    +---------------------------------------------------------------+
    |              Newlib/Picolib (C 标准库)                        |
    |           fopen, fread, fwrite, fclose, fseek 等              |
    +---------------------------------------------------------------+
    |                  VFS (虚拟文件系统)                           |
    |  POSIX API: open, read, write, close, lseek, stat 等          |
    +---------------------------------------------------------------+
    |                       FatFs 库                                |
    |       f_mount, f_open, f_read, f_write, f_close, f_mkfs       |
    +---------------------------------------------------------------+
    |                      磁盘 I/O 层                              |
    |  ff_diskio_register, disk_read, disk_write, disk_ioctl 等     |
    +-------------------------------+-------------------------------+
    |   磨损均衡（可选）             |                               |
    |  wl_mount, wl_read, wl_write  |        SD/MMC 驱动            |
    +-------------------------------+                               |
    |         esp_partition         |   sdmmc_read/write_sectors    |
    |    esp_partition_read/write   |                               |
    +-------------------------------+-------------------------------+
    |           SPI flash           |           SD 卡               |
    +-------------------------------+-------------------------------+

.. _fatfs-manual-mount:

手动挂载 FAT 文件系统分区
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

手动挂载 FAT 文件系统分区时，请遵循 :ref:`挂载和使用 FatFs <fatfs-mount-and-use>` 中描述的便利封装函数使用的相同一般流程：向 FatFs 库注册文件系统，然后通过 VFS 挂载它。

#. 通过调用 :cpp:func:`esp_vfs_fat_register` 向 VFS 注册文件系统。这会分配 ``FATFS`` 结构，并设置 VFS 和 FatFs 库之间的接口。指定：

    - 文件系统将挂载的路径前缀（例如 ``"/sdcard"`` 或 ``"/spiflash"``）。
    - FatFs 驱动器号。
    - 一个接收指向 ``FATFS`` 结构的指针的变量。

#. 通过调用 :cpp:func:`ff_diskio_register` 为步骤 1 中使用的驱动器号注册磁盘 I/O 驱动程序。

#. 通过调用 :cpp:func:`f_mount` 为传递给 :cpp:func:`esp_vfs_fat_register` 的驱动器号挂载文件系统。

   如果目标逻辑驱动器上不存在文件系统，:cpp:func:`f_mount` 将失败并返回 ``FR_NO_FILESYSTEM``。在这种情况下，使用 :cpp:func:`f_mkfs` 在驱动器上创建一个新的 FAT 文件系统，然后再次调用 :cpp:func:`f_mount`。

   对于 SD 卡，首先使用 :cpp:func:`f_fdisk` 创建分区表。有关详细信息，请参阅 `FatFs 文档 <http://elm-chan.org/fsw/ff/doc/mount.html>`_。

要卸载，请遵循以下一般步骤：

#. 通过使用相同的驱动器号和 NULL ``FATFS*`` 参数调用 :cpp:func:`f_mount` 来卸载文件系统。

#. 通过使用相同的驱动器号和 NULL ``ff_diskio_impl_t*`` 参数调用 :cpp:func:`ff_diskio_register` 来注销磁盘 I/O 驱动程序。

#. 通过使用挂载路径调用 :cpp:func:`esp_vfs_fat_unregister_path` 来删除 VFS 注册并释放在挂载过程的步骤 1 中分配的 ``FATFS`` 结构。

调用 FatFs 库提供的底层函数时，使用不带挂载点前缀的路径。例如，使用 ``"/hello.txt"``。

.. _fatfs-diskio-layer:

FatFs 磁盘 I/O 层
^^^^^^^^^^^^^^^^^^^^

FatFs 提供 API 以在运行时注册磁盘 I/O 驱动程序。这些 API 构成了介质特定访问函数和 FatFs 库之间的接口。

磁盘 I/O 层使用 :cpp:struct:`ff_diskio_impl_t` 来提供介质访问函数。将此结构传递给 :cpp:func:`ff_diskio_register`。

ESP-IDF 为支持的存储介质提供便利封装函数。这些包括：

    - :cpp:func:`ff_diskio_register_wl_partition` 用于带磨损均衡的 SPI flash 分区。
    - :cpp:func:`ff_diskio_register_raw_partition` 用于 **不带** 磨损均衡的只读 SPI flash 分区。
    - :cpp:func:`ff_diskio_register_sdmmc` 用于 SDMMC 和 SPI 连接的 SD 卡。

有关 API 参考，请参见 :ref:`磁盘 I/O API 参考 <fatfs-diskio-api-reference>`。

.. _fatfs-glossary:

术语表
--------

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - 术语
     - 描述
   * - VFS
     - 虚拟文件系统，ESP-IDF 提供的抽象层，允许应用程序通过统一的 POSIX 和 ``stdio.h`` 接口访问不同的底层文件系统。
   * - 逻辑扇区
     - 呈现给 FatFs 和磁盘 I/O 层的逻辑块大小。对于带磨损均衡的 SPI flash，这是 WL 扇区大小。对于只读原始 SPI flash 分区，这是 flash 扇区大小。对于 SD 卡，通常为 512 字节。
   * - WL 扇区
     - 磨损均衡层暴露给 FatFs 的逻辑扇区大小。根据 ``CONFIG_WL_SECTOR_SIZE``，可以是 512 字节或 4096 字节。
   * - flash 扇区
     - flash 芯片使用的底层 SPI flash 擦除单元。在几乎所有 ESP 开发板上，这是 4096 字节。当磨损均衡配置为 512 字节扇区时，它可以不同于呈现给 FatFs 的 WL 扇区大小。
   * - 簇
     - 最小的逻辑 FAT 分配单元，由一个或多个连续的逻辑扇区组成。
   * - FAT
     - 文件分配表，记录每个簇的使用状态以及文件数据块之间的链接关系。
   * - 磨损均衡
     - 一种通过均匀分布写入操作来延长 flash 存储器寿命的技术，防止特定块因频繁擦除而过早磨损。
   * - 驱动器号
     - FatFs 用于标识不同逻辑驱动器的数字 (0-9)，每个驱动器对应一个独立的文件系统实例。
   * - 挂载
     - 将文件系统与指定路径关联的过程，使应用程序能够通过该路径访问文件系统内容。
   * - SFN/LFN
     - 短文件名（8.3 格式）和长文件名（最多 255 个字符）。

.. _fatfs-application-examples:

应用程序示例
--------------------

- :example:`storage/fatfs/getting_started` 演示了使用 FatFs 在 SPI flash 上存储持久数据所需的最小设置，包括挂载文件系统、打开文件、执行基本的读写操作和卸载文件系统。

- :example:`storage/fatfs/fs_operations` 演示了高级 FatFs 操作，包括读写文件、创建、移动和删除文件和目录，以及检查文件详细信息。

- :example:`storage/fatfs/ext_flash` 演示了如何操作格式化为 FatFs 的外部 SPI flash，包括初始化 SPI 总线、配置 flash 芯片、将其注册为分区以及执行读写操作。

.. _fatfs-high-level-api-reference:

高级 API 参考
------------------------

.. include-build-file:: inc/esp_vfs_fat.inc

.. _fatfs-diskio-api-reference:

磁盘 I/O API 参考
^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfunction:: ff_diskio_register
.. doxygenstruct:: ff_diskio_impl_t
    :members:
.. doxygenfunction:: ff_diskio_register_sdmmc
.. doxygenfunction:: ff_diskio_register_wl_partition
.. doxygenfunction:: ff_diskio_register_raw_partition
