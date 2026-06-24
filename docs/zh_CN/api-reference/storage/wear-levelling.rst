.. include:: ../../../../components/wear_levelling/README_CN.rst

另请参阅
-----------

- :doc:`./fatfs`
- :doc:`../../api-guides/partition-tables`

应用示例
-------------------

:example:`storage/wear_levelling` 演示了如何使用磨损均衡库和 FatFS 库在分区中存储文件，并使用 POSIX 和 C 库 API 从中写入和读取数据。

高级 API 参考
------------------------

头文件
^^^^^^^^^^^^

* :component_file:`fatfs/vfs/esp_vfs_fat.h`

有关高级磨损均衡函数 :cpp:func:`esp_vfs_fat_spiflash_mount_rw_wl`、 :cpp:func:`esp_vfs_fat_spiflash_unmount_rw_wl` 和结构体 :cpp:class:`esp_vfs_fat_mount_config_t` 的详细内容，请参见 :doc:`./fatfs`。

中层 API 参考
-----------------------

.. include-build-file:: inc/wear_levelling.inc
