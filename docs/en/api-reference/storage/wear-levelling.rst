.. include:: ../../../../components/wear_levelling/README.rst

See Also
--------

- :doc:`./fatfs`
- :doc:`../../api-guides/partition-tables`

Application Examples
--------------------

- :example:`storage/wear_levelling` demonstrates how to use the wear levelling library and FatFS library to store files in a partition, as well as write and read data from these files using POSIX and C library APIs.

High-level API Reference
------------------------

Header Files
^^^^^^^^^^^^

* :component_file:`fatfs/vfs/esp_vfs_fat.h`

High-level wear levelling functions :cpp:func:`esp_vfs_fat_spiflash_mount_rw_wl`, :cpp:func:`esp_vfs_fat_spiflash_unmount_rw_wl` and struct :cpp:class:`esp_vfs_fat_mount_config_t` are described in :doc:`./fatfs`.

Mid-level API Reference
-----------------------

.. include-build-file:: inc/wear_levelling.inc
