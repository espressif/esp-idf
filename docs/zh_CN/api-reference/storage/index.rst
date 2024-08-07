存储 API
***********

:link_to_translation:`en:[English]`

本节提供高层次的存储 API 的参考文档。这些 API 基于如 SPI flash、SD/MMC 等低层次驱动。

- :doc:`分区表 API <partition>` 基于 :doc:`/api-guides/partition-tables` ，允许以块为单位访问 SPI flash。
- :doc:`非易失性存储库 (NVS) <nvs_flash>` 在 SPI NOR flash 上实现了一个有容错性，和磨损均衡功能的键值对存储。
- :doc:`虚拟文件系统 (VFS) <vfs>` 库提供了一个用于注册文件系统驱动的接口。SPIFFS、FAT 以及多种其他的文件系统库都基于 VFS。
- :doc:`SPIFFS <spiffs>` 是一个专为 SPI NOR flash 优化的磨损均衡的文件系统，非常适用于小分区和低吞吐率的应用。
- :doc:`FAT <fatfs>` 是一个可用于 SPI flash 或者 SD/MMC 存储卡的标准文件系统。
- :doc:`磨损均衡 <wear-levelling>` 库实现了一个适用于 SPI NOR flash 的 flash 翻译层 (FTL)，用于 flash 中 FAT 分区的容器。

与存储安全相关的信息，请参考 :doc:` 存储安全 <storage-security>`。

.. note::

    建议使用高层次的 API（``esp_partition`` 或者文件系统）而非低层次驱动 API 去访问 SPI NOR flash。

    由于 NOR flash 和乐鑫硬件的一些限制，访问主 flash 会影响各个系统的性能。关于这些限制的更多信息，参见 :doc:`/api-reference/peripherals/spi_flash/index`。

.. toctree::
   :maxdepth: 1

   fatfs
   fatfsgen
   mass_mfg.rst
   nvs_flash
   nvs_encryption
   nvs_partition_gen.rst
   nvs_partition_parse.rst
   sdmmc
   partition
   spiffs
   vfs
   wear-levelling
   storage-security.rst

.. list-table:: 存储 API 相关例程
    :widths: 25 75
    :header-rows: 0

    * - **例程**
      - **描述**
    * - :doc:`fatfs`
      -
    * - :example:`wear_leveling <storage/wear_levelling>`
      - 演示了如何在内部 flash 上使用 FATFS 磨损均衡。
    * - :example:`ext_flash_fatfs <storage/ext_flash_fatfs>`
      - 演示了如何在外部 flash 上使用 FATFS 磨损均衡。
    * - :example:`fatfsgen <storage/fatfsgen>`
      - 演示了在主机计算机上使用 Python 工具生成 FATFS 镜像的功能。
    * - :doc:`nvs_flash`
      -
    * - :example:`nvs_rw_blob <storage/nvs_rw_blob>`
      - 演示了如何在 NVS flash 中使用 C 语言 API 读写 blob 数据类型。
    * - :example:`nvs_rw_value <storage/nvs_rw_value>`
      - 演示了如何在 NVS flash 中使用 C 语言 API 读写整数数据类型。
    * - :example:`nvs_rw_value_cxx <storage/nvs_rw_value_cxx>`
      - 演示了如何在 NVS flash 中使用 C++ 语言 API 读写整数数据类型。
    * - :example:`nvsgen <storage/nvsgen>`
      - 演示了如何使用基于 Python 的 NVS 镜像生成工具，根据 CSV 文件内容创建 NVS 分区镜像。
    * - :doc:`spiffs`
      -
    * - :example:`spiffs <storage/spiffs>`
      - 演示了如何使用 SPIFFS API 初始化文件系统，并使用 POSIX 函数处理文件。
    * - :example:`spiffsgen <storage/spiffsgen>`
      - 演示了在主机计算机上使用 Python 工具生成 SPIFFS 镜像的功能。
    * - :doc:`partition`
      -
    * - :example:`partition_api <storage/partition_api>`
      - 介绍了用于查找特定分区、执行基本 I/O 操作以及通过 CPU 内存映射使用分区的 API 函数。
    * - :example:`parttool <storage/parttool>`
      - 演示了在主机计算机上使用 Python 工具生成分区镜像的功能。
    * - :doc:`vfs`
      -
    * - :example:`littlefs <storage/littlefs>`
      - 演示了如何使用 LittleFS 组件初始化文件系统，并使用 POSIX 函数处理文件。
    * - :example:`semihost_vfs <storage/semihost_vfs>`
      - 演示了如何使用 VFS API，利用 POSIX 函数使 ESP 设备访问通过 JTAG 连接的主机上的文件。
