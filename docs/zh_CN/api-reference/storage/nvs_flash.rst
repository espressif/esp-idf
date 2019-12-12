.. include:: ../../../../components/nvs_flash/README_CN.rst

NVS 分区生成程序
------------------

NVS 分区生成程序帮助生成 NVS 分区二进制文件，可使用烧录程序将二进制文件单独烧录至特定分区。烧录至分区上的键值对由 CSV 文件提供，详情请参考 :doc:`NVS 分区生成程序 <nvs_partition_gen>`。 

应用示例
-------------------

ESP-IDF :example:`storage` 目录下提供了两个代码示例：

:example:`storage/nvs_rw_value`

  演示如何读取及写入 NVS 单个整数值。

  此示例中的值表示 ESP32 模组重启次数。NVS 中数据不会因为模组重启而丢失，因此只有将这一值存储于 NVS 中，才能起到重启次数计数器的作用。 

  该示例也演示了如何检测读取/写入操作是否成功，以及某个特定值是否在 NVS 中尚未初始化。诊断程序以纯文本形式提供，帮助您追踪程序流程，及时发现问题。

:example:`storage/nvs_rw_blob`　

  演示如何读取及写入 NVS 单个整数值和 Blob（二进制大对象），并在 NVS 中存储这一数值，即便 ESP32 模组重启也不会消失。

    * value - 记录 ESP32 模组软重启次数和硬重启次数。
    * blob - 内含记录模组运行次数的表格。此表格将被从 NVS 读取至动态分配的 RAM 上。每次手动软重启后，表格内运行次数即增加一次，新加的运行次数被写入 NVS。下拉 GPIO0 即可手动软重启。

  该示例也演示了如何执行诊断程序以检测读取/写入操作是否成功。


API 参考
-------------

.. include:: /_build/inc/nvs_flash.inc

.. include:: /_build/inc/nvs.inc
