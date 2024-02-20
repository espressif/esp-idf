Partitions API
==============

:link_to_translation:`zh_CN:[中文]`

Overview
--------

The ``esp_partition`` component has higher-level API functions which work with partitions defined in the :doc:`/api-guides/partition-tables`. These APIs are based on lower level API provided by :doc:`/api-reference/peripherals/spi_flash/index`.

.. _flash-partition-apis:

Partition Table API
-------------------

ESP-IDF projects use a partition table to maintain information about various regions of SPI flash memory (bootloader, various application binaries, data, filesystems). More information can be found in :doc:`/api-guides/partition-tables`.

This component provides API functions to enumerate partitions found in the partition table and perform operations on them. These functions are declared in ``esp_partition.h``:

- :cpp:func:`esp_partition_find` checks a partition table for entries with specific type, returns an opaque iterator.
- :cpp:func:`esp_partition_get` returns a structure describing the partition for a given iterator.
- :cpp:func:`esp_partition_next` shifts the iterator to the next found partition.
- :cpp:func:`esp_partition_iterator_release` releases iterator returned by :cpp:func:`esp_partition_find`.
- :cpp:func:`esp_partition_find_first` is a convenience function which returns the structure describing the first partition found by :cpp:func:`esp_partition_find`.
- :cpp:func:`esp_partition_read`, :cpp:func:`esp_partition_write`, :cpp:func:`esp_partition_erase_range` are equivalent to :cpp:func:`esp_flash_read`, :cpp:func:`esp_flash_write`, :cpp:func:`esp_flash_erase_region`, but operate within partition boundaries.


See Also
--------

- :doc:`../../api-guides/partition-tables`
- :doc:`../system/ota` provides high-level API for updating applications stored in flash.
- :doc:`nvs_flash` provides a structured API for storing small pieces of data in SPI flash.


.. _api-reference-partition-table:

API Reference - Partition Table
-------------------------------

.. include-build-file:: inc/esp_partition.inc
