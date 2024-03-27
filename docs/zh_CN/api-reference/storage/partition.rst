分区 API
========

:link_to_translation:`en:[English]`

概述
--------

``esp_partition`` 组件提供了高层次的 API 函数，用于访问定义在 :doc:`/api-guides/partition-tables` 中的分区。这些 API 基于 :doc:`/api-reference/peripherals/spi_flash/index` 提供的低层次 API。

.. _flash-partition-apis:

分区表 API
-------------------

ESP-IDF 工程使用分区表保存 SPI flash 各区信息，包括引导程序、各种应用程序二进制文件、数据及文件系统等。请参阅 :doc:`/api-guides/partition-tables`，查看详细信息。

该组件在 ``esp_partition.h`` 中声明了一些 API 函数，用以枚举在分区表中找到的分区，并对这些分区执行操作：

- :cpp:func:`esp_partition_find`：在分区表中查找特定类型的条目，返回一个不透明迭代器；
- :cpp:func:`esp_partition_get`：返回一个结构体，描述给定迭代器的分区；
- :cpp:func:`esp_partition_next`：将迭代器移至下一个找到的分区；
- :cpp:func:`esp_partition_iterator_release`：释放 :cpp:func:`esp_partition_find` 中返回的迭代器；
- :cpp:func:`esp_partition_find_first`：返回描述 :cpp:func:`esp_partition_find` 中找到的第一个分区的结构；
- :cpp:func:`esp_partition_read`、:cpp:func:`esp_partition_write` 和 :cpp:func:`esp_partition_erase_range` 等同于 :cpp:func:`esp_flash_read`、:cpp:func:`esp_flash_write` 和 :cpp:func:`esp_flash_erase_region`，但在分区边界内执行。


另请参考
------------

- :doc:`../../api-guides/partition-tables`
- :doc:`../system/ota` 提供了高层 API 用于更新存储在 flash 中的 app 固件。
- :doc:`nvs_flash` 提供了结构化 API 用于存储 SPI flash 中的碎片数据。


.. _api-reference-partition-table:

分区表 API 参考
-------------------------------

.. include-build-file:: inc/esp_partition.inc
