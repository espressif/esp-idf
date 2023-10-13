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

.. note::

    建议使用高层次的 API（``esp_partition`` 或者文件系统）而非低层次驱动 API 去访问 SPI NOR flash。

    由于 NOR flash 和乐鑫硬件的一些限制，访问主 flash 会影响各个系统的性能。关于这些限制的更多信息，参见 :doc:`/api-reference/peripherals/spi_flash/index`。

.. toctree::
   :maxdepth: 1

   fatfs
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

此部分 API 代码示例存放在 ESP-IDF 示例项目的 :example:`storage` 目录下。
