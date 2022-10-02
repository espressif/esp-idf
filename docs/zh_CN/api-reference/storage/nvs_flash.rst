非易失性存储库
============================

:link_to_translation:`en:[English]`

简介
------------

非易失性存储 (NVS) 库主要用于在 flash 中存储键值格式的数据。本文档将详细介绍 NVS 常用的一些概念。

底层存储
^^^^^^^^^^^^^^^^^^

NVS 库通过调用 :ref:`esp_partition <flash-partition-apis>` API 使用主 flash 的部分空间，即类型为 ``data`` 且子类型为 ``nvs`` 的所有分区。应用程序可调用 :cpp:func:`nvs_open` API 选择使用带有 ``nvs`` 标签的分区，也可以通过调用 :cpp:func:`nvs_open_from_partition` API 选择使用指定名称的任意分区。

NVS 库后续版本可能会增加其他存储器后端，来将数据保存至其他 flash 芯片（SPI 或 I2C 接口）、RTC 或 FRAM 中。

.. note:: 如果 NVS 分区被截断（例如，更改分区表布局时），则应擦除分区内容。可以使用 ESP-IDF 构建系统中的 ``idf.py erase-flash`` 命令擦除 flash 上的所有内容。

.. note:: NVS 最适合存储一些较小的数据，而非字符串或二进制大对象 (BLOB) 等较大的数据。如需存储较大的 BLOB 或者字符串，请考虑使用基于磨损均衡库的 FAT 文件系统。


键值对
^^^^^^^^^^^^^^^

NVS 的操作对象为键值对，其中键是 ASCII 字符串，当前支持的最大键长为 15 个字符。值可以为以下几种类型：

-  整数型：``uint8_t``、``int8_t``、``uint16_t``、``int16_t``、``uint32_t``、``int32_t``、``uint64_t`` 和 ``int64_t``；
-  以 0 结尾的字符串；
-  可变长度的二进制数据 (BLOB)

.. note::

    字符串值当前上限为 4000 字节，其中包括空终止符。BLOB 值上限为 508,000 字节或分区大小的 97.6% 减去 4000 字节，以较低值为准。

后续可能会增加对 ``float`` 和 ``double`` 等其他类型数据的支持。

键必须唯一。为现有的键写入新的值可能产生如下结果：

-  如果新旧值数据类型相同，则更新值；
-  如果新旧值数据类型不同，则返回错误。

读取值时也会执行数据类型检查。如果读取操作的数据类型与该值的数据类型不匹配，则返回错误。


命名空间
^^^^^^^^^^

为了减少不同组件之间键名的潜在冲突，NVS 将每个键值对分配给一个命名空间。命名空间的命名规则遵循键名的命名规则，例如，最多可占 15 个字符。此外，单个 NVS 分区最多只能容纳 254 个不同的命名空间。命名空间的名称在调用 :cpp:func:`nvs_open` 或 :cpp:type:`nvs_open_from_partition` 中指定，调用后将返回一个不透明句柄，用于后续调用 ``nvs_get_*``、``nvs_set_*`` 和 ``nvs_commit`` 函数。这样，一个句柄关联一个命名空间，键名便不会与其他命名空间中相同键名冲突。请注意，不同 NVS 分区中具有相同名称的命名空间将被视为不同的命名空间。

NVS 迭代器
^^^^^^^^^^^^^

迭代器允许根据指定的分区名称、命名空间和数据类型轮询 NVS 中存储的键值对。

您可以使用以下函数，执行相关操作：

- ``nvs_entry_find``：创建一个不透明句柄，用于后续调用 ``nvs_entry_next`` 和 ``nvs_entry_info`` 函数；
- ``nvs_entry_next``：让迭代器指向下一个键值对；
- ``nvs_entry_info``：返回每个键值对的信息。

总的来说，所有通过 :cpp:func:`nvs_entry_find` 获得的迭代器（包括 ``NULL`` 迭代器）都必须使用 :cpp:func:`nvs_release_iterator` 释放。
一般情况下，:cpp:func:`nvs_entry_find` 和 :cpp:func:`nvs_entry_next` 会将给定的迭代器设置为 ``NULL`` 或为一个有效的迭代器。但如果出现参数错误（如返回 ``ESP_ERR_NVS_NOT_FOUND``），给定的迭代器不会被修改。因此，在调用 :cpp:func:`nvs_entry_find` 之前最好将迭代器初始化为 ``NULL``，这样可以避免在释放迭代器之前进行复杂的错误检查。


安全性、篡改性及鲁棒性
^^^^^^^^^^^^^^^^^^^^^^^^^^

NVS 与 {IDF_TARGET_NAME} flash 加密系统不直接兼容。但如果 NVS 加密与 {IDF_TARGET_NAME} flash 加密一起使用时，数据仍可以加密形式存储。详情请参阅 :ref:`nvs_encryption`。

如果未启用 NVS 加密，任何对 flash 芯片有物理访问权限的用户都可以修改、擦除或添加键值对。NVS 加密启用后，如果不知道相应的 NVS 加密密钥，则无法修改或添加键值对并将其识别为有效键值对。但是，针对擦除操作没有相应的防篡改功能。

当 flash 处于不一致状态时，NVS 库会尝试恢复。在任何时间点关闭设备电源，然后重新打开电源，不会导致数据丢失；但如果关闭设备电源时正在写入新的键值对，这一键值对可能会丢失。该库还应该能够在 flash 中存在任何随机数据的情况下正常初始化。


.. _nvs_encryption:

NVS 加密
--------------

NVS 分区内存储的数据可使用 AES-XTS 进行加密，类似于 IEEE P1619 磁盘加密标准中提到的加密方式。为了实现加密，每个条目被均视为一个扇区，并将条目相对地址（相对于分区开头）传递给加密算法，用作扇区号。可通过 :ref:`CONFIG_NVS_ENCRYPTION` 启用 NVS 加密。NVS 加密所需的密钥存储于其他分区，并且被 :doc:`Flash 加密 <../../security/flash-encryption>` 保护。因此，在使用 NVS 加密前应先启用 :doc:`Flash 加密 <../../security/flash-encryption>`。

启用 :doc:`Flash 加密 <../../security/flash-encryption>` 时，默认启用 NVS 加密。这是因为 Wi-Fi 驱动在默认的 NVS 分区中存储了凭证（如 SSID 和密码）。如已启用平台级加密，那么同时默认启用 NVS 加密有其必要性。

使用 NVS 加密，分区表必须包含 :ref:`nvs_key_partition`。在分区表选项 (``menuconfig`` > ``Partition Table``) 下，为 NVS 加密提供了两个包含 :ref:`nvs_key_partition` 的分区表，您可以通过工程配置菜单 (``idf.py menuconfig``) 进行选择。请参考 :example:`security/flash_encryption` 中的例子，了解如何配置和使用 NVS 加密功能。

.. _nvs_key_partition:

NVS 密钥分区
^^^^^^^^^^^^^^^^^

应用程序如果想使用 NVS 加密，则需要编译进一个类型为 `data`，子类型为 `key` 的密钥分区。该分区应标记为 `已加密` 且最小为 4096 字节。如需了解更多详细信息，请参考 :doc:`分区表 <../../api-guides/partition-tables>`。在分区表选项 (``menuconfig`` > ``Partition Table``) 下提供了两个包含 :ref:`nvs_key_partition` 的额外分区表，可以直接用于 :ref:`nvs_encryption`。这些分区的具体结构见下表：

.. highlight:: none

::

    +-----------+--------------+-------------+----+
    |              XTS encryption key (32)        |
    +---------------------------------------------+
    |              XTS tweak key (32)             |
    +---------------------------------------------+
    |                  CRC32 (4)                  |
    +---------------------------------------------+

可以通过以下两种方式生成 :ref:`nvs_key_partition` 中的 XTS 加密密钥：

1. 在 ESP 芯片上生成密钥：

    启用 NVS 加密时，可用 :cpp:func:`nvs_flash_init` API 函数来初始化加密的默认 NVS 分区，在内部生成 ESP 芯片上的 XTS 加密密钥。在找到 :ref:`nvs_key_partition` 后，API 函数利用 :component_file:`nvs_flash/include/nvs_flash.h` 提供的 :cpp:func:`nvs_flash_generate_keys` 函数，自动生成并存储该分区中的 NVS 密钥。只有当各自的密钥分区为空时，才会生成并存储新的密钥。可以借助 :cpp:func:`nvs_flash_secure_init_partition` 用同一个密钥分区来读取安全配置，以初始化一个定制的加密 NVS 分区。

    API 函数 :cpp:func:`nvs_flash_secure_init` 和 :cpp:func:`nvs_flash_secure_init_partition` 不在内部产生密钥。当这些 API 函数用于初始化加密的 NVS 分区时，可以在启动后使用 `nvs_flash.h` 提供的 :cpp:func:`nvs_flash_generate_keys` API 函数生成密钥，以加密的形式把密钥写到密钥分区上。

2. 使用预先生成的密钥分区：

    若 :ref:`nvs_key_partition` 中的密钥不是由应用程序生成，则需要使用预先生成的密钥分区。可以使用 :doc:`NVS 分区生成工具 </api-reference/storage/nvs_partition_gen>` 生成包含 XTS 加密密钥的 :ref:`nvs_key_partition`。用户可以借助以下两个命令，将预先生成的密钥分区储存在 flash 上：

    i) 建立并烧录分区表
    ::

        idf.py partition-table partition-table-flash

    ii) 调用 :component_file:`parttool.py<partition_table/parttool.py>`，将密钥存储在 flash 上的 :ref:`nvs_key_partition` 中。详见 :doc:` 分区表 </api-guides/partition-tables>` 的分区工具部分。
    ::

        parttool.py --port PORT --partition-table-offset PARTITION_TABLE_OFFSET write_partition --partition-name="name of nvs_key partition" --input NVS_KEY_PARTITION_FILE
    
    .. note:: 如需在设备处于 flash 加密开发模式时更新 NVS 密钥分区，请调用 :component_file:`parttool.py <partition_table/parttool.py>` 对 NVS 密钥分区进行加密。同时，由于设备上的分区表也已加密，您还需要在构建目录（build/partition_table）中提供一个指向未加密分区表的指针。您可以使用如下命令：
        ::

            parttool.py --esptool-write-args encrypt --port PORT --partition-table-file=PARTITION_TABLE_FILE --partition-table-offset PARTITION_TABLE_OFFSET write_partition --partition-name="name of nvs_key partition" --input NVS_KEY_PARTITION_FILE

由于分区已标记为 `已加密`，而且启用了 :doc:`Flash 加密 <../../security/flash-encryption>`，引导程序在首次启动时将使用 flash 加密对密钥分区进行加密。

应用程序可以使用不同的密钥对不同的 NVS 分区进行加密，这样就会需要多个加密密钥分区。应用程序应为加解密操作提供正确的密钥或密钥分区。

加密读取/写入
^^^^^^^^^^^^^^^^^^^^

``nvs_get_*`` 和 ``nvs_set_*`` 等 NVS API 函数同样可以对 NVS 加密分区执行读写操作。

**加密默认的 NVS 分区：**
无需额外步骤即可启用默认 NVS 分区的加密。启用 :ref:`CONFIG_NVS_ENCRYPTION` 时， :cpp:func:`nvs_flash_init` API 函数会在内部使用找到的第一个 :ref:`nvs_key_partition` 执行额外步骤，以启用默认 NVS 分区的加密（详情请参考 API 文档）。另外，:cpp:func:`nvs_flash_secure_init` API 函数也可以用来启用默认 NVS 分区的加密。

**加密一个自定义的 NVS 分区：**
使用 :cpp:func:`nvs_flash_secure_init_partition` API 函数启用自定义 NVS 分区的加密，而非 :cpp:func:`nvs_flash_init_partition`。

使用 :cpp:func:`nvs_flash_secure_init` 和 :cpp:func:`nvs_flash_secure_init_partition` API 函数时，应用程序如需在加密状态下执行 NVS 读写操作，应遵循以下步骤：

    1. 使用 ``esp_partition_find*`` API 查找密钥分区和 NVS 数据分区；
    2. 使用 ``nvs_flash_read_security_cfg`` 或 ``nvs_flash_generate_keys`` API 填充 ``nvs_sec_cfg_t`` 结构；
    3. 使用 ``nvs_flash_secure_init`` 或 ``nvs_flash_secure_init_partition`` API 初始化 NVS flash 分区；
    4. 使用 ``nvs_open`` 或 ``nvs_open_from_partition`` API 打开命名空间；
    5. 使用 ``nvs_get_*`` 或 ``nvs_set_*`` API 执行 NVS 读取/写入操作；
    6. 使用 ``nvs_flash_deinit`` API 释放已初始化的 NVS 分区。

NVS 分区生成程序
------------------

NVS 分区生成程序帮助生成 NVS 分区二进制文件，可使用烧录程序将二进制文件单独烧录至特定分区。烧录至分区上的键值对由 CSV 文件提供，详情请参考 :doc:`NVS 分区生成程序 <nvs_partition_gen>`。

应用示例
-------------------

ESP-IDF :example:`storage` 目录下提供了数个代码示例：

:example:`storage/nvs_rw_value`

  演示如何读取及写入 NVS 单个整数值。

  此示例中的值表示 {IDF_TARGET_NAME} 模组重启次数。NVS 中数据不会因为模组重启而丢失，因此只有将这一值存储于 NVS 中，才能起到重启次数计数器的作用。

  该示例也演示了如何检测读取/写入操作是否成功，以及某个特定值是否在 NVS 中尚未初始化。诊断程序以纯文本形式提供，帮助您追踪程序流程，及时发现问题。

:example:`storage/nvs_rw_blob`　

  演示如何读取及写入 NVS 单个整数值和 BLOB（二进制大对象），并在 NVS 中存储这一数值，即便 {IDF_TARGET_NAME} 模组重启也不会消失。

    * value - 记录 {IDF_TARGET_NAME} 模组软重启次数和硬重启次数。
    * blob - 内含记录模组运行次数的表格。此表格将被从 NVS 读取至动态分配的 RAM 上。每次手动软重启后，表格内运行次数即增加一次，新加的运行次数被写入 NVS。下拉 GPIO0 即可手动软重启。

  该示例也演示了如何执行诊断程序以检测读取/写入操作是否成功。

:example:`storage/nvs_rw_value_cxx`

  这个例子与 :example:`storage/nvs_rw_value` 完全一样，只是使用了 C++ 的 NVS 句柄类。

内部实现
---------

键值对日志
^^^^^^^^^^^^^^^^^^^^^^

NVS 按顺序存储键值对，新的键值对添加在最后。因此，如需更新某一键值对，实际是在日志最后增加一对新的键值对，同时将旧的键值对标记为已擦除。

页面和条目
^^^^^^^^^^^^^^^^^

NVS 库在其操作中主要使用两个实体：页面和条目。页面是一个逻辑结构，用于存储部分的整体日志。逻辑页面对应 flash 的一个物理扇区，正在使用中的页面具有与之相关联的 *序列号*。序列号赋予了页面顺序，较高的序列号对应较晚创建的页面。页面有以下几种状态：

空或未初始化
    页面对应的 flash 扇区为空白状态（所有字节均为 ``0xff``）。此时，页面未存储任何数据且没有关联的序列号。

活跃状态
    此时 flash 已完成初始化，页头部写入 flash，页面已具备有效序列号。页面中存在一些空条目，可写入数据。任意时刻，至多有一个页面处于活跃状态。

写满状态
    Flash 已写满键值对，状态不再改变。
    用户无法向写满状态下的页面写入新键值对，但仍可将一些键值对标记为已擦除。

擦除状态
    未擦除的键值对将移至其他页面，以便擦除当前页面。这一状态仅为暂时性状态，即 API 调用返回时，页面应脱离这一状态。如果设备突然断电，下次开机时，设备将继续把未擦除的键值对移至其他页面，并继续擦除当前页面。

损坏状态
    页头部包含无效数据，无法进一步解析该页面中的数据，因此之前写入该页面的所有条目均无法访问。相应的 flash 扇区并不会被立即擦除，而是与其他处于未初始化状态的扇区一起等待后续使用。这一状态可能对调试有用。

Flash 扇区映射至逻辑页面并没有特定的顺序，NVS 库会检查存储在 flash 扇区的页面序列号，并根据序列号组织页面。

::

    +--------+     +--------+     +--------+     +--------+
    | Page 1 |     | Page 2 |     | Page 3 |     | Page 4 |
    | Full   +---> | Full   +---> | Active |     | Empty  |   <- 状态
    | #11    |     | #12    |     | #14    |     |        |   <- 序列号
    +---+----+     +----+---+     +----+---+     +---+----+
        |               |              |             |
        |               |              |             |
        |               |              |             |
    +---v------+  +-----v----+  +------v---+  +------v---+
    | Sector 3 |  | Sector 0 |  | Sector 2 |  | Sector 1 |    <- 物理扇区
    +----------+  +----------+  +----------+  +----------+

页面结构
^^^^^^^^^^^^^^^^^^^

当前，我们假设 flash 扇区大小为 4096 字节，并且 {IDF_TARGET_NAME} flash 加密硬件在 32 字节块上运行。未来有可能引入一些编译时可配置项（可通过 menuconfig 进行配置），以适配具有不同扇区大小的 flash 芯片。但目前尚不清楚 SPI flash 驱动和 SPI flash cache 之类的系统组件是否支持其他扇区大小。

页面由头部、条目状态位图和条目三部分组成。为了实现与 {IDF_TARGET_NAME} flash 加密功能兼容，条目大小设置为 32 字节。如果键值为整数型，条目则保存一个键值对；如果键值为字符串或 BLOB 类型，则条目仅保存一个键值对的部分内容（更多信息详见条目结构描述）。

页面结构如下图所示，括号内数字表示该部分的大小（以字节为单位）。

::

    +-----------+--------------+-------------+-------------------------+
    | State (4) | Seq. no. (4) | version (1) | Unused (19) | CRC32 (4) |   页头部 (32)
    +-----------+--------------+-------------+-------------------------+
    |                Entry state bitmap (32)                           |
    +------------------------------------------------------------------+
    |                       Entry 0 (32)                               |
    +------------------------------------------------------------------+
    |                       Entry 1 (32)                               |
    +------------------------------------------------------------------+
    /                                                                  /
    /                                                                  /
    +------------------------------------------------------------------+
    |                       Entry 125 (32)                             |
    +------------------------------------------------------------------+

头部和条目状态位图写入 flash 时不加密。如果启用了 {IDF_TARGET_NAME} flash 加密功能，则条目写入 flash 时将会加密。

通过将 0 写入某些位可以定义页面状态值，表示状态改变。因此，如果需要变更页面状态，并不一定要擦除页面，除非要将其变更为 *擦除* 状态。

头部中的 ``version`` 字段反映了所用的 NVS 格式版本。为实现向后兼容，版本升级从 0xff 开始依次递减（例如，version-1 为 0xff，version-2 为 0xfe，以此类推）。

头部中 CRC32 值是由不包含状态值的条目计算所得（4 到 28 字节）。当前未使用的条目用 ``0xff`` 字节填充。

条目结构和条目状态位图的详细信息见下文描述。

条目和条目状态位图
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

每个条目可处于以下三种状态之一，每个状态在条目状态位图中用两位表示。位图中的最后四位 (256 - 2 * 126) 未使用。

空 (2'b11)
    条目还未写入任何内容，处于未初始化状态（全部字节为 ``0xff``）。

写入（2'b10）
    一个键值对（或跨多个条目的键值对的部分内容）已写入条目中。

擦除（2'b00）
    条目中的键值对已丢弃，条目内容不再解析。


.. _structure_of_entry:

条目结构
^^^^^^^^^^^^^^^^^^

如果键值类型为基础类型，即 1 - 8 个字节长度的整数型，条目将保存一个键值对；如果键值类型为字符串或 BLOB 类型，条目将保存整个键值对的部分内容。另外，如果键值为字符串类型且跨多个条目，则键值所跨的所有条目均保存在同一页面。BLOB 则可以切分为多个块，实现跨多个页面。BLOB 索引是一个附加的固定长度元数据条目，用于追踪 BLOB 块。目前条目仍支持早期 BLOB 格式（可读取可修改），但这些 BLOB 一经修改，即以新格式储存至条目。

::

    +--------+----------+----------+----------------+-----------+---------------+----------+
    | NS (1) | Type (1) | Span (1) | ChunkIndex (1) | CRC32 (4) |    Key (16)   | Data (8) |
    +--------+----------+----------+----------------+-----------+---------------+----------+

                                             Primitive  +--------------------------------+
                                            +-------->  |     Data (8)                   |
                                            | Types     +--------------------------------+
                       +-> Fixed length --
                       |                    |           +---------+--------------+---------------+-------+
                       |                    +-------->  | Size(4) | ChunkCount(1)| ChunkStart(1) | Rsv(2)|
        Data format ---+                    BLOB Index  +---------+--------------+---------------+-------+
                       |
                       |                             +----------+---------+-----------+
                       +->   Variable length   -->   | Size (2) | Rsv (2) | CRC32 (4) |
                            (Strings, BLOB Data)     +----------+---------+-----------+


条目结构中各个字段含义如下：

命名空间 (NS, NameSpace)
    该条目的命名空间索引，详细信息参见命名空间实现章节。

类型 (Type)
    一个字节表示的值的数据类型，:component_file:`nvs_flash/include/nvs_handle.hpp` 下的 :cpp:type:`ItemType` 枚举了可能的类型。

跨度 (Span)
    该键值对所用的条目数量。如果键值为整数型，条目数量即为 1。如果键值为字符串或 BLOB，则条目数量取决于值的长度。

块索引 (ChunkIndex)
    用于存储 BLOB 类型数据块的索引。如果键值为其他数据类型，则此处索引应写入 ``0xff``。

CRC32
    对条目下所有字节进行校验后，所得的校验和（CRC32 字段不计算在内）。

键 (Key)
    即以零结尾的 ASCII 字符串，字符串最长为 15 字节，不包含最后一个字节的零终止符。

数据 (Data)
    如果键值类型为整数型，则数据字段仅包含键值。如果键值小于八个字节，使用 ``0xff`` 填充未使用的部分（右侧）。

    如果键值类型为 BLOB 索引条目，则该字段的八个字节将保存以下数据块信息：

    - 块大小
        整个 BLOB 数据的大小（以字节为单位）。该字段仅用于 BLOB 索引类型条目。

    - ChunkCount
        存储过程中 BLOB 分成的数据块总量。该字段仅用于 BLOB 索引类型条目。

    - ChunkStart
        BLOB 第一个数据块的块索引，后续数据块索引依次递增，步长为 1。该字段仅用于 BLOB 索引类型条目。

    如果键值类型为字符串或 BLOB 数据块，数据字段的这八个字节将保存该键值的一些附加信息，如下所示：

    - 数据大小
        实际数据的大小（以字节为单位）。如果键值类型为字符串，此字段也应将零终止符包含在内。此字段仅用于字符串和 BLOB 类型条目。

    - CRC32
        数据所有字节的校验和，该字段仅用于字符串和 BLOB 类型条目。

可变长度值（字符串和 BLOB）写入后续条目，每个条目 32 字节。第一个条目的 `Span` 字段将指明使用了多少条目。


命名空间
^^^^^^^^^^

如上所述，每个键值对属于一个命名空间。命名空间标识符（字符串）也作为键值对的键，存储在索引为 0 的命名空间中。与这些键对应的值就是这些命名空间的索引。

::

    +-------------------------------------------+
    | NS=0 Type=uint8_t Key="wifi" Value=1      |   Entry describing namespace "wifi"
    +-------------------------------------------+
    | NS=1 Type=uint32_t Key="channel" Value=6  |   Key "channel" in namespace "wifi"
    +-------------------------------------------+
    | NS=0 Type=uint8_t Key="pwm" Value=2       |   Entry describing namespace "pwm"
    +-------------------------------------------+
    | NS=2 Type=uint16_t Key="channel" Value=20 |   Key "channel" in namespace "pwm"
    +-------------------------------------------+


条目哈希列表
^^^^^^^^^^^^^^

为了减少对 flash 执行的读操作次数，Page 类对象均设有一个列表，包含一对数据：条目索引和条目哈希值。该列表可大大提高检索速度，而无需迭代所有条目并逐个从 flash 中读取。``Page::findItem`` 首先从哈希列表中检索条目哈希值，如果条目存在，则在页面内给出条目索引。由于哈希冲突，在哈希列表中检索条目哈希值可能会得到不同的条目，对 flash 中条目再次迭代可解决这一冲突。

哈希列表中每个节点均包含一个 24 位哈希值和 8 位条目索引。哈希值根据条目命名空间、键名和块索引由 CRC32 计算所得，计算结果保留 24 位。为减少将 32 位条目存储在链表中的开销，链表采用了数组的双向链表。每个数组占用 128 个字节，包含 29 个条目、两个链表指针和一个 32 位计数字段。因此，每页额外需要的 RAM 最少为 128 字节，最多为 640 字节。

API 参考
-------------

.. include-build-file:: inc/nvs_flash.inc

.. include-build-file:: inc/nvs.inc
