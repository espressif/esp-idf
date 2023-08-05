.. include:: ../../../../components/wear_levelling/README_CN.rst

另请参阅
-----------

- :doc:`./fatfs`
- :doc:`../../api-guides/partition-tables`

应用示例
-------------------

:example:`storage/wear_levelling` 中提供了一款磨损均衡驱动与 FatFs 库结合使用的示例。该示例初始化磨损均衡驱动，挂载 FAT 文件系统分区，并使用 POSIX（可移植操作系统接口）和 C 库 API 从中写入和读取数据。如需了解更多信息，请参考 :example_file:`storage/wear_levelling/README.md`。

高级 API 参考
------------------------

头文件
^^^^^^^^^^^^

* :component_file:`fatfs/vfs/esp_vfs_fat.h`

有关高级磨损均衡函数 :cpp:func:`esp_vfs_fat_spiflash_mount_rw_wl`、 :cpp:func:`esp_vfs_fat_spiflash_unmount_rw_wl` 和结构体 :cpp:class:`esp_vfs_fat_mount_config_t` 的详细内容，请参见 :doc:`./fatfs`。

中层 API 参考
-----------------------

.. include-build-file:: inc/wear_levelling.inc
