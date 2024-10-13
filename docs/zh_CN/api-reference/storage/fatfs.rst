FAT 文件系统
======================

:link_to_translation:`en:[English]`

ESP-IDF 使用 `FatFs <http://elm-chan.org/fsw/ff/00index_e.html>`_ 库来实现 FAT 文件系统。FatFs 库位于 ``fatfs`` 组件中，支持直接使用，也可以借助 C 标准库和 POSIX API 通过 VFS（虚拟文件系统）使用 FatFs 库的大多数功能。

此外，我们对 FatFs 库进行了扩展，新增了支持可插拔磁盘 I/O 调度层，从而允许在运行时将 FatFs 驱动映射到物理磁盘。

.. _using-fatfs-with-vfs:

FatFs 与 VFS 配合使用
----------------------------

头文件 :component_file:`fatfs/vfs/esp_vfs_fat.h` 定义了连接 FatFs 和 VFS 的函数。

函数 :cpp:func:`esp_vfs_fat_register` 分配一个 ``FATFS`` 结构，并在 VFS 中注册特定路径前缀。如果文件路径以此前缀开头，则对此文件的后续操作将转至 FatFs API。

函数 :cpp:func:`esp_vfs_fat_unregister_path` 删除在 VFS 中的注册，并释放 ``FATFS`` 结构。

多数应用程序在使用 ``esp_vfs_fat_`` 函数时，采用如下步骤：

#. 调用 :cpp:func:`esp_vfs_fat_register`，指定：
    - 挂载文件系统的路径前缀（例如，``"/sdcard"`` 或 ``"/spiflash"``）
    - FatFs 驱动编号
    - 一个用于接收指向 ``FATFS`` 结构指针的变量

#. 调用 :cpp:func:`ff_diskio_register`，为步骤 1 中的驱动编号注册磁盘 I/O 驱动；

#. 如需使用与传递到 :cpp:func:`esp_vfs_fat_register` 相同的驱动编号挂载文件系统，可调用 FatFs 函数 :cpp:func:`f_mount`。如果目标逻辑驱动上不存在该文件系统，:cpp:func:`f_mount` 将调用失败并报告 ``FR_NO_FILESYSTEM`` 错误。此时，应首先调用 :cpp:func:`f_mkfs`，在驱动上创建新的 FatFS 结构体，然后重新调用 :cpp:func:`f_mount`。注意，应在上述步骤之前调用 :cpp:func:`f_fdisk` 对 SD 卡进行分区。请参考 `FatFs 文档 <http://elm-chan.org/fsw/ff/doc/mount.html>`_，查看更多信息；

#. 调用 C 标准库和 POSIX API 对路径中带有步骤 1 中所述前缀的文件（例如，``"/sdcard/hello.txt"``）执行打开、读取、写入、擦除、复制等操作。文件系统默认使用 `8.3 文件名 <https://en.wikipedia.org/wiki/8.3_filename>`_ 格式 (SFN)。如需使用长文件名 (LFN)，启用 :ref:`CONFIG_FATFS_LONG_FILENAMES` 选项。请参考 `FatFs 文件系统 <http://elm-chan.org/fsw/ff/doc/filename.html>`_，查看更多信息；

#. 可以直接调用 FatFs 库函数，但需要使用没有 VFS 前缀的路径，如 ``"/hello.txt"``；

#. 关闭所有打开的文件；

#. 调用 FatFs 函数 :cpp:func:`f_mount` 并使用 NULL ``FATFS*`` 参数，为与上述编号相同的驱动卸载文件系统；

#. 调用 FatFs 函数 :cpp:func:`ff_diskio_register` 并使用 NULL ``ff_diskio_impl_t*`` 参数和相同的驱动编号，来释放注册的磁盘 I/O 驱动；

#. 调用 :cpp:func:`esp_vfs_fat_unregister_path` 并使用文件系统挂载的路径将 FatFs 从 VFS 中移除，并释放步骤 1 中分配的 ``FATFS`` 结构。

便捷函数 :cpp:func:`esp_vfs_fat_sdmmc_mount`、:cpp:func:`esp_vfs_fat_sdspi_mount` 和 :cpp:func:`esp_vfs_fat_sdcard_unmount` 对上述步骤进行了封装，并加入了对 SD 卡初始化的处理。我们将在下一章节详细介绍以上函数。

.. note::

    FAT 文件系统不支持硬链接，因此调用 :cpp:func:`link` 后会复制文件内容（仅适用于 FatFs 卷上的文件）。


.. _using-fatfs-with-vfs-and-sdcards:

FatFs 与 VFS 和 SD 卡配合使用
---------------------------------

头文件 :component_file:`fatfs/vfs/esp_vfs_fat.h` 定义了便捷函数 :cpp:func:`esp_vfs_fat_sdmmc_mount`、 :cpp:func:`esp_vfs_fat_sdspi_mount` 和 :cpp:func:`esp_vfs_fat_sdcard_unmount`。这些函数分别执行上一章节的步骤 1-3 和步骤 7-9，并初始化 SD 卡，但仅提供有限的错误处理功能。我们鼓励开发人员查看源代码，将更多高级功能集成到产品应用中。

便捷函数 :cpp:func:`esp_vfs_fat_sdmmc_unmount` 用于卸载文件系统并释放从 :cpp:func:`esp_vfs_fat_sdmmc_mount` 函数获取的资源。


FatFs 与 VFS 配合使用（只读模式下）
--------------------------------------

头文件 :component_file:`fatfs/vfs/esp_vfs_fat.h` 也定义了两个便捷函数 :cpp:func:`esp_vfs_fat_spiflash_mount_ro` 和 :cpp:func:`esp_vfs_fat_spiflash_unmount_ro`。上述两个函数分别对 FAT 只读分区执行步骤 1-3 和步骤 7-9。有些数据分区仅在工厂配置时写入一次，之后在整个硬件生命周期内都不会再有任何改动。利用上述两个函数处理这种数据分区非常方便。

配置选项
--------

FatFs 组件有以下配置选项：

* :ref:`CONFIG_FATFS_USE_FASTSEEK` - 如果启用该选项，POSIX :cpp:func:`lseek` 函数将以更快的速度执行。快速查找不适用于编辑模式下的文件，所以，使用快速查找时，应在只读模式下打开（或者关闭然后重新打开）文件。
* :ref:`CONFIG_FATFS_IMMEDIATE_FSYNC` - 如果启用该选项，FatFs 将在每次调用 :cpp:func:`write`、:cpp:func:`pwrite`、:cpp:func:`link`、:cpp:func:`truncate` 和 :cpp:func:`ftruncate` 函数后，自动调用 :cpp:func:`f_sync` 以同步最近的文件改动。该功能可提高文件系统中文件的一致性和文件大小报告的准确性，但由于需要频繁进行磁盘操作，性能将会受到影响。
* :ref:`CONFIG_FATFS_LINK_LOCK` - 如果启用该选项，可保证 API 的线程安全，但如果应用程序需要快速频繁地进行小文件操作（例如将日志记录到文件），则可能有必要禁用该选项。请注意，如果禁用该选项，调用 :cpp:func:`link` 后的复制操作将是非原子的，此时如果在不同任务中对同一卷上的大文件调用 :cpp:func:`link`，则无法确保线程安全。


.. _fatfs-diskio-layer:

FatFs 磁盘 I/O 层
-------------------

我们对 FatFs API 函数进行了扩展，实现了运行期间注册磁盘 I/O 驱动。

上述 API 为 SD/MMC 卡提供了磁盘 I/O 函数实现方式，可使用 :cpp:func:`ff_diskio_register_sdmmc` 函数注册指定的 FatFs 驱动编号。

.. doxygenfunction:: ff_diskio_register
.. doxygenstruct:: ff_diskio_impl_t
    :members:
.. doxygenfunction:: ff_diskio_register_sdmmc
.. doxygenfunction:: ff_diskio_register_wl_partition
.. doxygenfunction:: ff_diskio_register_raw_partition


.. _fatfs-partition-generator:

FatFs 分区生成器
-------------------------

我们为 FatFs (:component_file:`wl_fatfsgen.py<fatfs/wl_fatfsgen.py>`) 提供了分区生成器，该生成器集成在构建系统中，方便用户在自己的项目中使用。

该生成器可以在主机上创建文件系统镜像，并用指定的主机文件夹内容对其进行填充。

该脚本是建立在分区生成器的基础上 (:component_file:`fatfsgen.py<fatfs/fatfsgen.py>`)，目前除了可以生成分区外，也可以初始化磨损均衡。

目前的最新版本支持短文件名、长文件名、FAT12 和 FAT16。长文件名的上限是 255 个字符，文件名中可以包含多个 ``.`` 字符以及其他字符，如 ``+``、``,``、``;``、``=``、``[`` and ``]`` 等。

如需进一步了解 FatFs 分区生成器或分区分析器，请查看 :doc:`Generating and parsing FAT partition on host <./fatfsgen>`。

构建系统中使用 FatFs 分区生成器
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

通过调用 ``fatfs_create_partition_image`` 可以直接从 CMake 构建系统中调用 FatFs 分区生成器::

    fatfs_create_spiflash_image(<partition> <base_dir> [FLASH_IN_PROJECT])

如果不希望在生成分区时使用磨损均衡，可以使用 ``fatfs_create_rawflash_image``::

    fatfs_create_rawflash_image(<partition> <base_dir> [FLASH_IN_PROJECT])

``fatfs_create_spiflash_image`` 以及 ``fatfs_create_rawflash_image`` 必须从项目的 CMakeLists.txt 中调用。

如果决定使用 ``fatfs_create_rawflash_image`` （不支持磨损均衡），请注意它仅支持在设备中以只读模式安装。


该函数的参数如下：

#. partition - 分区的名称，需要在分区表中定义（如 :example_file:`storage/fatfsgen/partitions_example.csv`）。

#. base_dir - 目录名称，该目录会被编码为 FatFs 分区，也可以选择将其被烧录进设备。但注意必须在分区表中指定合适的分区大小。

#. ``FLASH_IN_PROJECT`` 标志 - 可选参数，用户可以通过指定 ``FLASH_IN_PROJECT``，选择在执行 ``idf.py flash -p <PORT>`` 时让分区镜像自动与应用程序二进制文件、分区表等一同烧录进设备。

#. ``PRESERVE_TIME`` 标志 - 可选参数，用户可强制让目标镜像保留源文件夹的时间戳。如果不保留，每个目标镜像的时间戳都将设置为 FATFS 默认初始时间（1980 年 1 月 1 日）。

#. ``ONE_FAT`` 标志 - 可选参数，支持生成仅包含单个 FAT（文件分配表）的 FATFS 卷。与包含两个 FAT 的 FATFS 卷相比，这样做可以拥有相对较大的可用空间（通过 ``FAT 使用的扇区数 * 扇区大小`` 计算），但会增加 FATFS 卷损坏的风险。

例如::

    fatfs_create_partition_image(my_fatfs_partition my_folder FLASH_IN_PROJECT)

没有指定 FLASH_IN_PROJECT 时也可以生成分区镜像，但是用户需要使用 ``esptool.py`` 或自定义的构建系统目标对其手动烧录。

相关示例请查看 :example:`storage/fatfsgen`。

.. _fatfs-partition-analyzer:

FatFs 分区分析器
------------------

我们为 FatFs 提供分区分析器 (:component_file:`fatfsparse.py<fatfs/fatfsparse.py>`)。

该分析器为 FatFs 分区生成器 (:component_file:`fatfsgen.py<fatfs/fatfsgen.py>`) 的逆向工具，可以根据 FatFs 镜像在主机上生成文件夹结构。

可以使用::

    ./fatfsparse.py [-h] [--wl-layer {detect,enabled,disabled}] [--verbose] fatfs_image.img

生成文件夹结构之前，参数 --verbose 将根据 FatFs 镜像的引导扇区在终端打印详细信息。

高级 API 参考
------------------------

.. include-build-file:: inc/esp_vfs_fat.inc
