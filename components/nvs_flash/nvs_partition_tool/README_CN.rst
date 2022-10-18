NVS 分区解析程序
======================

:link_to_translation:`en:[English]`

介绍
-------

NVS 分区解析程序 :component_file:`nvs_flash/nvs_partition_parser/nvs_read.py` 加载并解析 NVS 存储分区，以便于调试和数据提取。该程序还支持完整性检查功能，可扫描分区中可能存在的错误。

加密分区
-----------

此程序不支持解密。如需解密 NVS 分区，请使用 :doc:`NVS 分区生成程序 </api-reference/storage/nvs_partition_gen>`。该工具支持 NVS 分区加解密。

使用方法
-----------

该程序通过 `-d` 或 `-dump` 选项提供了六种不同的输出方式：
    - `all` (默认) - 打印所有带有元数据的条目。
    - `written` - 只打印带有元数据的写入条目。
    - `minimal` - 打印写入的 `namespace:key = value` 对。
    - `namespaces` - 打印所有写入的命名空间。
    - `blobs` - 打印所有 blob 和字符串（若 blob 和字符串是以分块的形式，则对其进行重组）。
    - `storage_info` - 打印每一页面的条目状态计数。

.. 注意:: 该程序还提供 `none` 选项，该选项不会打印任何东西。如果 NVS 分区的内容并不相关，可以将该选项和完整性检查选项一起使用。

该程序支持完整性检查功能，选择选项 `-i` 或 `--integrity-check` 即可运行。此功能可扫描整个分区，并打印出可能存在的错误。当此功能和 `-d none` 一起使用时，可只打印可能存在的错误。
