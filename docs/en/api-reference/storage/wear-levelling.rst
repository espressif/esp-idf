.. include:: ../../../../components/wear_levelling/README.rst

See also
--------

- :doc:`FAT Filesystem <./fatfs>`
- :doc:`Partition Table documentation <../../api-guides/partition-tables>`

Application Example
-------------------

An example which combines the wear levelling driver with the FATFS library is provided in the :example:`storage/wear_levelling` directory. This example initializes the wear levelling driver, mounts FATFS partition, as well as writes and reads data from it using POSIX and C library APIs. See the :example:`storage/wear_levelling/README.md` file for more information.

High level API Reference
------------------------

Header Files
^^^^^^^^^^^^

* :component_file:`fatfs/src/esp_vfs_fat.h`

Functions
^^^^^^^^^

.. doxygenfunction:: esp_vfs_fat_spiflash_mount
.. doxygenstruct:: esp_vfs_fat_mount_config_t
    :members:
.. doxygenfunction:: esp_vfs_fat_spiflash_unmount

Mid level API Reference
-----------------------

.. include:: /_build/inc/wear_levelling.inc

