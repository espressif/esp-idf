文件系统注意事项
==================

:link_to_translation:`en:[English]`

本文档旨在推荐最适合你的应用程序的文件系统。主要介绍了 ESP-IDF 所支持的文件系统的特定功能和属性，这些功能属性对于典型用例而言十分重要。如需获取每种文件系统的详细信息，请参阅相应的文档。

目前，ESP-IDF 框架支持三种文件系统。ESP-IDF 还提供了 API，用于批量处理文件系统的挂载和卸载。通过 C/POSIX 标准文件 API 能实现文件和目录的访问，使所有应用程序都可以使用相同接口，不受底层文件系统的影响。

- :ref:`FatFS <fatfs-fs-section>`
- :ref:`SPIFFS <spiffs-fs-section>`
- :ref:`LittleFS <littlefs-fs-section>`

这三种文件系统都是基于已有的第三方库，对其进行封装和修改后集成到 ESP-IDF。

ESP-IDF 还提供了非易失性存储 (NVS) 库 API，适用于简单的数据存储需求，通过键访问关联的值。该库虽然本身不是一个功能齐全的文件系统，但便于存储配置信息、校准数据以及类似信息。更多详细信息，请参阅 :ref:`NVS 库 <nvs-fs-section>` 小节。

上述文件系统的重要属性和功能如下表所示：

.. list-table::
    :widths: 20 40 40 40
    :header-rows: 1

    * -
      - FatFS
      - SPIFFS
      - LittleFS
    * - 功能
      - * 实现 MS FAT12、FAT16、FAT32 文件系统格式，可选 exFAT 文件系统的不同实现方式
        * 在大多数硬件平台上广泛兼容的通用文件系统
        * 文档完善
        * 线程安全
      - * 专为嵌入式系统中的 NOR flash 设备研发，低 RAM 占用
        * 达到静态磨损均衡
        * 不再维护，且文档较少
        * 线程安全
      - * 具备故障安全性，具备自身磨损均衡功能，固定 RAM（与文件系统大小无关）
        * 文档完善
        * 线程安全
    * - 存储单位与限制
      - * 簇（1～128 扇区）
        * 支持的扇区大小：512 B，4096 B
        * FAT12：簇大小 512 B ～ 8 kB，最多 4085 个簇
        * FAT16：簇大小 512 B ～ 64 kB，最多 65525 个簇
        * FAT32：簇大小 512 B ～ 32 kB，最多 268435455 个簇
      - * 逻辑页、由页组成的逻辑块
        * 典型配置：页大小 = 256 B，块大小 = 64 kB
      - * 块、元数据对
        * 典型配置：块大小 = 4 kB
    * - 磨损均衡
      - 可选（用于 SPI flash）
      - 集成
      - 集成
    * - 最小分区大小
      - * 8 个扇区启用磨损均衡（4 个 FATFS 扇区 + 4 个磨损均衡扇区，磨损均衡扇区大小为 4096 B）：
        * 最少再加 4 个扇区
        * 实际大小由磨损均衡配置（安全、性能）决定
      - * 6 个逻辑块
        * 每块 8 个页
      - 未指定，理论上 2 个块
    * - 最大分区大小
      - * FAT12：约 32 MB（8 kB 簇）
        * FAT16：约 4 GB（64 kB 簇，理论值）
        * FAT32：约 8 TB（32 kB 簇，理论值）
      - 未指定绝对最大值，不推荐超过每块 1024 页
      - 未指定，理论上约 2 GB
    * - 目录支持
      - * 是（普通 FAT 目录中最多 65536 个条目）
        * 限制：
            * FAT12：根目录最多 224 个文件
            * FAT16：根目录最多 512 个文件
            * FAT32：根目录只是另一个目录
      - 否
      - 是
    * - 电源故障保护
      - 否
      - 部分（见 :ref:`spiffs-fs-section`）
      - 是（集成）
    * - 加密
      - 是
      - 否
      - 是
    * - 支持目标
      - * SPI flash (NOR)
        * SD 卡
      - SPI flash (NOR)
      - * SPI flash (NOR)
        * SD 卡 (IDF >= v5.0)

如需比较使用不同配置和参数的文件系统性能，请参阅存储性能基准测试示例 :example:`storage/perf_benchmark` 获取详细信息。


.. _fatfs-fs-section:

FatFS
----------

FatFS 是最适用于常见应用的文件系统，如文件/目录访问、数据存储、日志记录等。它能够自动识别特定的 FAT 文件系统类型，可与 PC 及其他平台良好兼容。FatFS 支持分区加密、只读模式、可选的 SPI flash 磨损均衡（SD 卡使用其内置的磨损均衡），并配备辅助的主机工具：生成器和分析器、Python 脚本。它还支持 SDMMC 访问。较为显著的缺点是应对突然断电能力不足。为降低这方面的不稳定性，ESP-IDF 的 FatFS 默认设置部署了两个 FAT 表副本。可以通过设置 :cpp:member:`esp_vfs_fat_mount_config_t::use_one_fat` 标志来禁用此选项（2-FAT 相关操作完全由 FatFS 库处理）。有关更多信息，请参见相关示例。

**相关文档:**

- `FatFS 源站点 <http://elm-chan.org/fsw/ff/>`_
- 更多关于 `FAT 表大小限制 <https://en.wikipedia.org/wiki/Design_of_the_FAT_file_system#Size_limits>`_
- :ref:`using-fatfs-with-vfs`
- :ref:`using-fatfs-with-vfs-and-sdcards`
- ESP-IDF FatFS 工具： :ref:`FatFS 分区生成器 <fatfs-partition-generator>` 和 :ref:`FatFS 分区分析器 <fatfs-partition-analyzer>`

**示例:**

* :example:`storage/sd_card` 演示了如何访问使用 FAT 文件系统的 SD 卡。
* :example:`storage/fatfs/ext_flash` 演示了如何访问使用 FAT 文件系统的外部 flash。


.. _spiffs-fs-section:

SPIFFS
------------

SPIFFS 文件系统提供了一定程度的断电安全性（参见重启后修复函数 :cpp:func:`esp_spiffs_check`），并具有内置磨损均衡机制。由于其垃圾回收机制，当专用分区使用超过大约 70% 时，文件系统性能可能会下降，且不支持目录。SPIFFS 适用于仅需管理少量（可能是大型）文件，并要求较高数据一致性的场景。通常，SPIFFS 比 FatFS 占用更少的 RAM 空间，并支持最大 128 MB 的 flash 芯片。需要注意的是，SPIFFS 已不再开发和维护，因此请仔细斟酌是否优先选择此文件系统。

**相关文档:**

* :doc:`SPIFFS 文件系统 <../api-reference/storage/spiffs>`
* :ref:`生成 SPIFFS 镜像的工具 <spiffs-generator>`

**示例:**

* :example:`storage/spiffs` 演示了如何在 {IDF_TARGET_NAME} 上使用 SPIFFS 文件系统。


.. _littlefs-fs-section:

LittleFS
----------------------

LittleFS 是一种基于块的文件系统，专为微控制器和嵌入式设备设计。它具有良好的断电恢复能力，可实现动态磨损均衡，并且 RAM 占用少。此外，其限制配置可调整，支持直接在 SD/MMC 卡上操作。此文件系统适用于通用的应用，唯一的缺点是文件系统与其他平台的兼容性较差（不如 FatFS 文件系统）。

LittleFS 可以作为外部组件从 `ESP Component Registry <https://components.espressif.com/>`_ 获取。详细信息请参阅 `LittleFS 组件介绍 <https://components.espressif.com/components/joltwallet/littlefs>`_，以了解如何将该文件系统应用到项目中。

**相关文档:**

* `LittleFS 项目主页（源码、文档） <https://github.com/littlefs-project/littlefs>`_
* `LittleFS 辅助工具和相关项目 <https://github.com/littlefs-project/littlefs?tab=readme-ov-file#related-projects>`_
* `ESP-IDF 的 LittleFS 移植版 <https://github.com/joltwallet/esp_littlefs>`_
* `ESP-IDF LittleFS 组件 <https://components.espressif.com/components/joltwallet/littlefs>`_

**示例:**

* :example:`storage/littlefs` 演示了如何在 {IDF_TARGET_NAME} 上使用 LittleFS 文件系统。

.. _nvs-fs-section:

NVS 库
----------

非易失性存储（NVS）适用于需要处理大量键值对的应用场景，如应用系统配置。为了方便使用，键空间被划分为多个命名空间，每个命名空间是一个独立的存储区域。除了支持基本的数据类型（最大支持 64 位整数），NVS 还支持零终止字符串和长度可变的二进制大对象数据 (blob)。

NVS 具有如下特性：

* 支持 flash 磨损均衡。
* 突然断电保护（数据以确保原子更新的方式存储）。
* 支持加密（AES-XTS）。
* 提供设备生产过程和离线分析需要的数据准备工具。

在开发与 NVS 相关的代码时需要注意以下事项：

* 推荐的使用场景：用于存储不经常更改的配置数据。
* NVS 不适用于日志记录或其他需要频繁大量数据更新的场景，而适用于小规模的更新和低频率的写入。另一个限制是 flash 页面擦除的最大次数，对于 NOR flash 设备，通常为 100,000 左右。
* 如果需存储的数据组更新速率差异较大，建议为每个数据组创建不同的 NVS flash 分区，便于管理磨损均衡，降低数据损坏的风险。
* 默认的 NVS 分区（标记为 "nvs"）由其他 ESP-IDF 组件（如 Wi-Fi、蓝牙等）使用。为避免与其他组件发生冲突，建议为相关数据创建单独的分区。
* NVS 存储在 flash 中的分配单位是一个页面，即 4,096 字节。每个 NVS 分区至少需要三个页面才能正常工作。一个页面始终被保留，不用于数据存储。
* 在写入或更新现有数据之前，必须在 NVS 分区中有足够的空闲空间来存储旧数据和新数据。NVS 库不支持部分更新，因此跨 flash 页的大型 blob 数据可能需要更长的写入时间和更多的空间占用，增加实际操作的复杂性。
* NVS 库无法在不符合规格的电源环境中确保一致性，例如使用电池或太阳能面板供电的系统。在这种情况下，flash 数据的错误解析可能会导致 NVS flash 分区损坏。开发者应提供数据恢复代码，例如，基于具有工厂设置的只读数据分区。
* 已初始化的 NVS 库会占用 RAM 空间，这个占用量随着 flash 分区和缓存的键数量的增加线性增长。

**只读 NVS 分区：**

* 只读分区可用于存储运行时不应被修改的数据，例如固件或配置数据。
* 可以在分区表 CSV 文件中将 NVS 分区标记为 ``readonly``。只读 NVS 分区的最小尺寸可以小至一页（4 KiB/``0x1000``），而标准读写 NVS 分区则无法设置如此小的尺寸。
* ``0x1000`` 和 ``0x2000`` 大小的分区始终为只读分区，而大小为 ``0x3000`` 及以上的分区则支持读写（在代码中仍可以只读模式打开）。

**相关文档:**

- 有关 API 和 NVS 库的更多信息，请参阅 :doc:`非易失性存储库 <../api-reference/storage/nvs_flash>`。
- 有关批量生产的信息，请参阅 :doc:`NVS 分区生成程序 <../api-reference/storage/nvs_partition_gen>`。
- 有关离线 NVS 分区解析的信息，请参阅 :doc:`NVS 分区解析程序 <../api-reference/storage/nvs_partition_parse>`。
- 有关只读 NVS 分区的信息，请参阅 :ref:`只读 NVS <read-only-nvs>`。

**示例:**

- :example:`storage/nvs/nvs_rw_value` 演示了如何写入和读取一个整数值。
- :example:`storage/nvs/nvs_rw_blob` 演示如何写入和读取一个 blob。
- :example:`security/nvs_encryption_hmac` 演示了如何用 HMAC 外设进行 NVS 加密，并通过 efuse 中的 HMAC 密钥生成加密密钥。
- :example:`security/flash_encryption` 演示了如何进行 flash 加密，包括创建和使用 NVS 分区。


文件处理设计注意事项
----------------------

关于将可靠的存储功能集成到应用程序中，建议如下：

* 尽可能使用 C 标准库文件 API（ISO 或 POSIX）。这种高级接口可以保证在切换到其他文件系统时，无需进行太多更改。ESP-IDF 支持的所有文件系统作为底层实现，供 C 标准库调用，开发代码无需了解文件系统的实现细节。每个系统唯一特有的部分是格式化、挂载以及诊断/修复功能。
* 将文件系统相关的代码分离并封装，尽量简化后续的更新。
* 采用合理的应用程序文件存储结构：
    * 尽可能均匀分布负载。使用适当数量的目录/子目录（例如，FAT12 的根目录只能存储 224 条记录）。
    * 避免使用过多、过大的文件（相对而言，文件数量过多更易引发问题）。每个文件相当于系统内部“数据库”中的一条记录，随着文件数量增加，管理这些文件所需的开销可能会超过实际存储的数据量。这会快速消耗文件系统资源，导致应用程序启动失败，这在嵌入式系统中尤为常见。
    * 格外注意 SPI Flash 内存中执行的写入或擦除操作的次数（例如，在 FatFS 中每次写入都需要擦除全部的写入区域）。NOR flash 设备每个扇区通常可以承受超过 100,000 次擦除循环，磨损均衡机制延长了设备寿命。磨损均衡机制作为独立组件在相应的驱动程序堆栈中实现，对应用程序来说是透明的。由于磨损均衡算法会在给定的分区空间中轮换 flash 内存扇区，所以需要一些可用的磁盘空间进行虚拟扇区的调整。假设创建了一个“精确”划分的分区，被应用数据完全占用，磨损均衡机制就不再作用，造成设备快速老化。在实际应用中，flash 写入频率为 500 毫秒的项目，能够在几天内毁坏 ESP32 flash（真实案例）。
    * 基于上述考虑，建议使用适当的较大分区，以确保数据的安全裕度。通常，预留额外的 flash 空间产生的成本要比被迫解决突发故障的成本更低。
    * 根据应用程序实际所需，谨慎选择文件系统。例如，NVS 不建议用于存储生产数据，因为它不适合存储过多的项目（NVS 分区推荐最大容量大约为 128 kB）。


分区加密
------------

{IDF_TARGET_NAME} 芯片具有多种特性，支持加密芯片主 SPI Flash 内存中各分区内容。相关信息请参阅 :doc:`flash 加密 <../security/flash-encryption>` 和 :doc:`NVS 加密 <../api-reference/storage/nvs_encryption>`。这两种加密方式都使用 AES 算法，flash 加密提供硬件驱动的加密方案，对软件层完全透明；而 NVS 加密是基于 mbedTLS 组件实现的软件功能（如果芯片型号支持，mbedTLS 可以内部使用 AES 硬件加速器）。需注意，NVS 加密需启用 flash 加密，因为 NVS 加密需要一个专有的加密分区来存储密钥。然而 NVS 的内部结构与 flash 加密设计不兼容，二者实际是互相独立的。

鉴于存储安全方案和 {IDF_TARGET_NAME} 芯片设计，主要文档中可能并未明确以下两点：

* flash 加密仅适用于主 SPI flash 存储。这是因为它具有一个缓存模块，所有的透明加密 API 都通过该缓存模块进行。外部 flash 分区没有此缓存支持，所以无法利用原生 flash 加密功能来加密外部 flash 分区。
* 采取以下方式部署外部分区加密：实现自定义 SPI flash 驱动（参见 :example:`storage/custom_flash_driver`），或自定义驱动栈中的更高层，例如提供自定义的 :ref:`FatFS 磁盘 I/O 层 <fatfs-diskio-layer>`。
