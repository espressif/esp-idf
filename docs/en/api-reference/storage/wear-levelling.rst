.. include:: ../../../../components/wear_levelling/README.rst

See Also
--------

- :doc:`./fatfs`
- :doc:`../../api-guides/partition-tables`

Application Example
-------------------

An example that combines the wear levelling driver with the FATFS library is provided in the :example:`storage/wear_levelling` directory. This example initializes the wear levelling driver, mounts FatFs partition, as well as writes and reads data from it using POSIX and C library APIs. See :example_file:`storage/wear_levelling/README.md` for more information.

High-level API Reference
------------------------

Header Files
^^^^^^^^^^^^

* :component_file:`fatfs/vfs/esp_vfs_fat.h`

High-level wear levelling functions :cpp:func:`esp_vfs_fat_spiflash_mount_rw_wl`, :cpp:func:`esp_vfs_fat_spiflash_unmount_rw_wl` and struct :cpp:class:`esp_vfs_fat_mount_config_t` are described in :doc:`./fatfs`.

Mid-level API Reference
-----------------------

.. include-build-file:: inc/wear_levelling.inc
