.. include:: ../../../../components/wear_levelling/README_CN.rst

另请参阅
-----------

- :doc:`FAT 文件系统 <./fatfs>`
- :doc:`分区表 <../../api-guides/partition-tables>`

应用示例  
-------------------

:example:`storage/wear_levelling` 中提供了一款磨损均衡驱动与 FatFs 库结合使用的示例。该示例初始化磨损均衡驱动，挂载 FAT 文件系统分区，并使用 POSIX（可移植操作系统接口）和 C 库 API 从中写入和读取数据。如需了解更多信息，请参考 :example:`storage/wear_levelling/README.md`。

高级 API 参考
------------------------

头文件
^^^^^^^^^^^^

* :component_file:`fatfs/src/esp_vfs_fat.h`

函数
^^^^^^^^^

.. doxygenfunction:: esp_vfs_fat_spiflash_mount
.. doxygenstruct:: esp_vfs_fat_mount_config_t
    :members:
.. doxygenfunction:: esp_vfs_fat_spiflash_unmount

中层 API 参考
-----------------------

.. include:: /_build/inc/wear_levelling.inc

