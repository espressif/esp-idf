FAT 文件系统
======================

:link_to_translation:`en:[English]`

ESP-IDF 使用 `FatFs <http://elm-chan.org/fsw/ff/00index_e.html>`_ 库来实现 FAT 文件系统。FatFs 库位于 ``fatfs`` 组件中，您可以直接使用，也可以借助 C 标准库和 POSIX API 通过 VFS（虚拟文件系统）使用 FatFs 库的大多数功能。

此外，我们对 FatFs 库进行了扩展，新增了支持可插拔磁盘 I/O 调度层，从而允许在运行时将 FatFs 驱动映射到物理磁盘。

FatFs 与 VFS 配合使用
----------------------------

:component_file:`fatfs/vfs/esp_vfs_fat.h` 头文件定义了连接 FatFs 和 VFS 的函数。

函数 :cpp:func:`esp_vfs_fat_register` 分配一个 ``FATFS`` 结构，并在 VFS 中注册特定路径前缀。如果文件路径以此前缀开头，则对此文件的后续操作将转至 FatFs API。函数 :cpp:func:`esp_vfs_fat_unregister_path` 删除在 VFS 中的注册，并释放 ``FATFS`` 结构。

多数应用程序在使用 ``esp_vfs_fat_`` 函数时，采用如下步骤：

1. 调用 :cpp:func:`esp_vfs_fat_register`，指定：

    - 挂载文件系统的路径前缀（例如，``"/sdcard"`` 或 ``"/spiflash"``）
    - FatFs 驱动编号
    - 一个用于接收指向 ``FATFS`` 结构指针的变量

2. 调用 :cpp:func:`ff_diskio_register` 为上述步骤中的驱动编号注册磁盘 I/O 驱动；

3. 调用 FatFs 函数 ``f_mount``，或 ``f_fdisk``， ``f_mkfs``，并使用与传递到 :cpp:func:`esp_vfs_fat_register` 相同的驱动编号挂载文件系统。请参考 `FatFs 文档 <http://www.elm-chan.org/fsw/ff/doc/mount.html>`_，查看更多信息；

4. 调用 C 标准库和 POSIX API 对路径中带有步骤 1 中所述前缀的文件（例如，``"/sdcard/hello.txt"``）执行打开、读取、写入、擦除、复制等操作。

5. 您可以选择直接调用 FatFs 库函数，但需要使用没有 VFS 前缀的路径（例如，``"/hello.txt"``）；

6. 关闭所有打开的文件；

7. 调用 ``f_mount`` 并使用 NULL ``FATFS*`` 参数为与上述编号相同的驱动卸载文件系统；

8. 调用 FatFs 函数 :cpp:func:`ff_diskio_register` 并使用 NULL ``ff_diskio_impl_t*`` 参数和相同的驱动编号来释放注册的磁盘 I/O 驱动。

9. 调用 :cpp:func:`esp_vfs_fat_unregister_path` 并使用文件系统挂载的路径将 FatFs 从 NVS 中移除，并释放步骤 1 中分配的 FatFs 结构。

``esp_vfs_fat_sdmmc_mount`` 和 ``esp_vfs_fat_sdmmc_unmount`` 这两个便捷函数对上述步骤进行了封装，并加入对 SD 卡初始化的处理，非常便捷。我们将在下一章节详细介绍这两个函数。

.. doxygenfunction:: esp_vfs_fat_register
.. doxygenfunction:: esp_vfs_fat_unregister_path


FatFs 与 VFS 和 SD 卡配合使用
---------------------------------

:component_file:`fatfs/vfs/esp_vfs_fat.h` 头文件定义了两个便捷函数 :cpp:func:`esp_vfs_fat_sdmmc_mount` 和 :cpp:func:`esp_vfs_fat_sdmmc_unmount`。这两个函数分别执行上一章节的步骤 1-3 和步骤 7-9，并初始化 SD 卡，但仅提供有限的错误处理功能。我们鼓励开发人员查看源代码并将更多高级功能集成到产品应用中。

:cpp:func:`esp_vfs_fat_sdmmc_unmount` 函数用于卸载文件系统并释放从 :cpp:func:`esp_vfs_fat_sdmmc_mount` 函数获取的资源。

.. doxygenfunction:: esp_vfs_fat_sdmmc_mount
.. doxygenstruct:: esp_vfs_fat_mount_config_t
    :members:
.. doxygenfunction:: esp_vfs_fat_sdmmc_unmount


FatFs 与 VFS 配合使用（只读模式下）
--------------------------------------

:component_file:`fatfs/vfs/esp_vfs_fat.h` 头文件也定义了两个便捷函数 :cpp:func:`esp_vfs_fat_rawflash_mount` 和 :cpp:func:`esp_vfs_fat_rawflash_unmount`。上述两个函数分别对 FAT 只读分区执行步骤 1-3 和步骤 7-9。有些数据分区仅在工厂时写入一次，之后在整个硬件生命周期内都不会再有任何改动。利用上述两个函数处理这种数据分区非常方便。

.. doxygenfunction:: esp_vfs_fat_rawflash_mount
.. doxygenfunction:: esp_vfs_fat_rawflash_unmount


FatFs 磁盘 I/O 层
-------------------

我们对 FatFs API 函数进行了扩展，实现了运行期间注册磁盘 I/O 驱动。

上述 API 为 SD/MMC 卡提供了磁盘 I/O 函数实现方式，可使用 :cpp:func:`ff_diskio_register_sdmmc` 注册指定的 FatFs 驱动编号。

.. doxygenfunction:: ff_diskio_register
.. doxygenstruct:: ff_diskio_impl_t
    :members:
.. doxygenfunction:: ff_diskio_register_sdmmc
.. doxygenfunction:: ff_diskio_register_wl_partition
.. doxygenfunction:: ff_diskio_register_raw_partition


