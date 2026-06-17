分区 API
========

:link_to_translation:`en:[English]`

概述
--------

``esp_partition`` 组件提供了高层次的 API 函数，用于访问定义在 :doc:`/api-guides/partition-tables` 中的分区。这些 API 基于 :doc:`/api-reference/peripherals/spi_flash/index` 提供的低层次 API。

.. _flash-partition-apis:

分区表 API
-------------------

ESP-IDF 工程使用分区表保存 SPI flash 各区信息，包括引导加载程序、各种应用程序二进制文件、数据及文件系统等。请参阅 :doc:`/api-guides/partition-tables`，查看详细信息。

该组件在 ``esp_partition.h`` 中声明了一些 API 函数，用以枚举在分区表中找到的分区，并对这些分区执行操作：

- :cpp:func:`esp_partition_find`：在分区表中查找特定类型的条目，返回一个不透明迭代器；
- :cpp:func:`esp_partition_get`：返回一个结构体，描述给定迭代器的分区；
- :cpp:func:`esp_partition_next`：将迭代器移至下一个找到的分区；
- :cpp:func:`esp_partition_iterator_release`：释放 :cpp:func:`esp_partition_find` 中返回的迭代器；
- :cpp:func:`esp_partition_find_first`：返回描述 :cpp:func:`esp_partition_find` 中找到的第一个分区的结构；
- :cpp:func:`esp_partition_read`、:cpp:func:`esp_partition_write` 和 :cpp:func:`esp_partition_erase_range` 等同于 :cpp:func:`esp_flash_read`、:cpp:func:`esp_flash_write` 和 :cpp:func:`esp_flash_erase_region`，但在分区边界内执行。

应用示例
-------------

- :example:`storage/partition_api/partition_ops` 演示了如何对分区表执行读、写和擦除操作。

- :example:`storage/parttool` 演示了如何使用分区工具执行读、写、擦除分区、检索分区信息和转储整个分区表等操作。

- :example:`storage/partition_api/partition_find` 演示了如何搜索分区表，并根据分区类型、子类型和标签/名称等约束条件返回匹配的分区。

- :example:`storage/partition_api/partition_mmap` 演示了如何配置 MMU，将分区映射到内存地址空间以进行读操作，并验证写入和读取的数据。

将二进制文件烧录到分区
-------------------------------

``esp_partition_register_target`` 函数允许将二进制文件注册到特定分区进行烧录。它会创建一个按分区划分的烧录目标（例如，``idf.py mypart-flash``，其中 ``mypart`` 是 ``partitions.csv`` 中分区的名称），并可选择将该二进制文件包含在 ``idf.py flash`` 中。

该函数在 :component_file:`esp_partition/project_include.cmake` 中定义。

**在项目的 CMakeLists.txt 中的基本用法：**

.. code-block:: cmake

   esp_partition_register_target(my_partition "${CMAKE_BINARY_DIR}/my_data.bin" FLASH_IN_PROJECT)

这会创建一个 ``my_partition-flash`` 目标，并在运行 ``idf.py flash`` 时包含该二进制文件。

**参数：**

- ``partition_name`` — 在分区表中定义的分区名称。
- ``binary_path`` — 要烧录的二进制文件路径。
- ``FLASH_IN_PROJECT`` （可选） — 在运行 ``idf.py flash`` 时也会烧录此二进制文件。
- ``ALWAYS_PLAINTEXT`` （可选） — 即使启用了 flash 加密，仍以非加密方式烧录。用于不支持加密的文件系统（例如 SPIFFS）。不得与分区表中标记为 ``encrypted`` 的分区一起使用。
- ``DEPENDS <targets>`` （可选） — 在烧录之前必须构建的 CMake 目标（例如，生成二进制文件的自定义目标）。
- ``FLASH_IN_PROJECT_DEPENDENCY_TARGETS <targets>`` （可选） — 额外的烧录目标（例如 ``encrypted-flash``），这些目标也应依赖于 ``DEPENDS`` 中指定的目标。需要 ``FLASH_IN_PROJECT``。

**组件集成示例：**

某些 IDF 组件在内部使用了 ``esp_partition_register_target``：

- 使用 :example:`storage/partition_api/partition_ops` 烧录带有预定义内容的自定义分区。
- :component_file:`fatfs/project_include.cmake` — ``fatfs_create_spiflash_image`` 会创建一个 FATFS 镜像并注册 ``<partition>-flash`` 目标。
- :component_file:`nvs_flash/project_include.cmake` — ``nvs_create_partition_image`` 会创建 NVS 镜像，并注册 ``<partition>-flash`` 目标。
- :component_file:`spiffs/project_include.cmake` — ``spiffs_create_partition_image`` 会创建 SPIFFS 镜像并注册 ``<partition>-flash`` 目标。

其他资源
-------------

- :doc:`../../api-guides/partition-tables`
- :doc:`../system/ota` 提供了高层 API，用于更新存储在 flash 中的应用程序。
- :doc:`nvs_flash` 提供了结构化 API，用于在 SPI flash 中存储小块数据。


.. _api-reference-partition-table:

分区表 API 参考
-------------------------------

.. include-build-file:: inc/esp_partition.inc
