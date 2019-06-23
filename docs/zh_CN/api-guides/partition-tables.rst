分区表
======
:link_to_translation:`en:[English]`

概述
----

每片 ESP32 的 flash 可以包含多个应用程序，以及多种不同类型的数据（例如校准数据、文件系统数据、参数存储器数据等）。因此，我们需要引入分区表的概念。

具体来说，ESP32 在 flash 的 :ref:`默认偏移地址 <CONFIG_PARTITION_TABLE_OFFSET>` 0x8000 处烧写一张分区表。该分区表的长度为 0xC00 字节（最多可以保存 95 条分区表条目）。分区表数据后还保存着该表的 MD5 校验和，用于验证分区表的完整性。此外，如果芯片使能了 :doc:`安全启动 </security/secure-boot>` 功能，则该分区表后还会保存签名信息。

分区表中的每个条目都包括以下几个部分：Name（标签）、Type（app、data 等）、SubType 以及在 flash 中的偏移量（分区的加载地址）。

在使用分区表时，最简单的方法就是用 `idf.py menuconfig` 选择一张预定义的分区表：

-  "Single factory app, no OTA"
-  "Factory app, two OTA definitions"

在以上两种选项中，出厂应用程序均将被烧录至 flash 的 0x10000 偏移地址处。这时，运行 `idf.py partition_table` ，即可以打印当前使用分区表的信息摘要。

内置分区表
------------

以下是 "Single factory app, no OTA" 选项的分区表信息摘要：


   # Espressif ESP32 Partition Table
   # Name,   Type, SubType, Offset,  Size,   Flags
   nvs,      data, nvs,     0x9000,  0x6000,
   phy_init, data, phy,     0xf000,  0x1000,
   factory,  app,  factory, 0x10000, 1M,

-  flash 的 0x10000 (64KB) 偏移地址处存放一个标记为 "factory" 的二进制应用程序，且 Bootloader 将默认加载这个应用程序。
-  分区表中还定义了两个数据区域，分别用于存储 NVS 库专用分区和 PHY 初始化数据。

以下是 "Factory app, two OTA definitions" 选项的分区表信息摘要：

   # Espressif ESP32 Partition Table
   # Name,   Type, SubType, Offset,   Size,   Flags
   nvs,      data, nvs,     0x9000,   0x4000,
   otadata,  data, ota,     0xd000,   0x2000,
   phy_init, data, phy,     0xf000,   0x1000,
   factory,  app,  factory, 0x10000,  1M,
   ota_0,    app,  ota_0,   0x110000, 1M,
   ota_1,    app,  ota_1,   0x210000, 1M,

-  分区表中定义了三个应用程序分区，这三个分区的类型都被设置为 “app”，但具体 app 类型不同。其中，位于 0x10000 偏移地址处的为出厂应用程序（factory），其余两个为 OTA 应用程序（ota_0，ota_1）。
-  新增了一个名为 “otadata” 的数据分区，用于保存 OTA 升级时候需要的数据。Bootloader 会查询该分区的数据，以判断该从哪个 OTA 应用程序分区加载程序。如果 “otadata” 分区为空，则会执行出厂程序。

创建自定义分区表
----------------

如果在 ``menuconfig`` 中选择了 “Custom partition table CSV”，则还需要输入该分区表的 CSV 文件在项目中的路径。CSV 文件可以根据需要，描述任意数量的分区信息。

CSV 文件的格式与上面摘要中打印的格式相同，但是在 CSV 文件中并非所有字段都是必需的。例如下面是一个自定义的 OTA 分区表的 CSV 文件：

   # Name,   Type, SubType,  Offset,   Size,  Flags
   nvs,      data, nvs,      0x9000,  0x4000
   otadata,  data, ota,      0xd000,  0x2000
   phy_init, data, phy,      0xf000,  0x1000
   factory,  app,  factory,  0x10000,  1M
   ota_0,    app,  ota_0,    ,         1M
   ota_1,    app,  ota_1,    ,         1M
   nvs_key,  data, nvs_keys, ,        0x1000

-  字段之间的空格会被忽略，任何以 ``#`` 开头的行（注释）也会被忽略。
-  CSV 文件中的每个非注释行均为一个分区定义。
-  每个分区的 ``Offset`` 字段可以为空，``gen_esp32part.py`` 工具会从分区表位置的后面开始自动计算并填充该分区的偏移地址，同时确保每个分区的偏移地址正确对齐。

Name 字段
~~~~~~~~~

Name 字段可以是任何有意义的名称，但不能超过 16 个字符（之后的内容将被截断）。该字段对 ESP32 并不是特别重要。

Type 字段
~~~~~~~~~

Type 字段可以指定为 app (0) 或者 data (1)，也可以直接使用数字 0-254（或者十六进制 0x00-0xFE）。注意，0x00-0x3F 不得使用（预留给 esp-idf 的核心功能）。

如果您的应用程序需要保存数据，请在 0x40-0xFE 内添加一个自定义分区类型。

注意，bootloader 将忽略 app (0) 和 data (1) 以外的其他分区类型。


SubType 字段
~~~~~~~~~~~~

SubType 字段长度为 8 bit，内容与具体 Type 有关。目前，esp-idf 仅仅规定了 “app” 和 “data” 两种子类型。
   
* 当 Type 定义为 ``app`` 时，SubType 字段可以指定为 factory (0)，ota_0 (0x10) ... ota_15 (0x1F) 或者 test (0x20)。

   -  factory (0) 是默认的 app 分区。Bootloader 将默认加在该应用程序。但如果存在类型为 data/ota 分区，则 Bootloader 将加载 data/ota 分区中的数据，进而判断启动哪个 OTA 镜像文件。
      -  OTA 升级永远都不会更新 factory 分区中的内容。
      -  如果您希望在 OTA 项目中预留更多 flash，可以删除 factory 分区，转而使用 ota_0 分区。

   -  ota_0 (0x10) ... ota_15 (0x1F) 为 OTA 应用程序分区，Bootloader 将根据 OTA 数据分区中的数据来决定加载哪个 OTA 应用程序分区中的程序。在使用 OTA 功能时，应用程序应至少拥有 2 个 OTA 应用程序分区（ota_0 和 ota_1）。更多详细信息，请参考 :doc:`OTA 文档 </api-reference/system/ota>` 。
   -  test (0x2) 为预留 app 子类型，用于工厂测试过程。注意，目前，esp-idf 并不支持这种子类型。
   
* 当 Type 定义为 ``data`` 时，SubType 字段可以指定为 ota (0)，phy (1)，nvs (2) 或者 nvs_keys (4)。

   -  ota (0) 即 :ref:`OTA 数据分区 <ota_data_partition>` ，用于存储当前所选的 OTA 应用程序的信息。这个分区的大小需要设定为 0x2000。更多详细信息，请参考 :doc:`OTA 文档 <../api-reference/system/ota>` 。
   -  phy (1) 分区用于存放 PHY 初始化数据，从而保证可以为每个设备单独配置 PHY，而非必须采用固件中的统一 PHY 初始化数据。
 
      -  默认配置下，phy 分区并不启用，而是直接将 phy 初始化数据编译至应用程序中，从而节省分区表空间（直接将此分区删掉）。
      -  如果需要从此分区加载 phy 初始化数据，请运行 ``idf.py menuconfig``，并且使能 :ref:`CONFIG_ESP32_PHY_INIT_DATA_IN_PARTITION` 选项。此时，您还需要手动将 phy 初始化数据烧至设备 flash（esp-idf 编译系统并不会自动完成该操作）。
   -  nvs (2) 是专门给 :doc:`非易失性存储 (NVS) API <../api-reference/storage/nvs_flash>` 使用的分区。

      -  用于存储每台设备的 PHY 校准数据（注意，并不是 PHY 初始化数据）。
      -  用于存储 Wi-Fi 数据（如果使用了 :cpp:func:`esp_wifi_set_storage(WIFI_STORAGE_FLASH) <esp_wifi_set_storage>` 初始化函数）。
      -  NVS API 还可以用于其他应用程序数据。
      -  强烈建议您应为 NVS 分区分配至少 0x3000 字节空间。
      -  如果使用 NVS API 存储大量数据，请增加 NVS 分区的大小（默认是 0x6000 字节）。
   -  nvs_keys (4) 是 NVS 秘钥分区。详细信息，请参考 :doc:`非易失性存储 (NVS) API <../api-reference/storage/nvs_flash>` 文档。

      -  用于存储加密密钥（如果启用了 `NVS 加密` 功能）。
      -  此分区应至少设定为 4096 字节。

其它数据子类型已预留给 esp-idf 的未来使用。

Offset 和 Size 字段
~~~~~~~~~~~~~~~~~~~

分区若为指定偏移地址，则会紧跟着前一个分区之后开始。若此分区为首个分区，则将紧跟着分区表开始。

app 分区的偏移地址必须要与 0x10000 (64K) 对齐，如果将偏移字段留空，``gen_esp32part.py`` 工具会自动计算得到一个满足对齐要求的偏移地址。如果 app 分区的偏移地址没有与 0x10000 (64K) 对齐，则该工具会报错。

app 分区的大小和偏移地址可以采用十进制数、以 0x 为前缀的十六进制数，且支持 K 或 M 的倍数单位（分别代表 1024 和 1024*1024 字节）。

如果您希望允许分区表中的分区采用任意起始偏移量 (:ref:`CONFIG_PARTITION_TABLE_OFFSET`)，请将分区表（CSV 文件）中所有分区的偏移字段都留空。注意，此时，如果您更改了分区表中任意分区的偏移地址，则其他分区的偏移地址也会跟着改变。这种情况下，如果您之前还曾设定某个分区采用固定偏移地址，则可能造成分区表冲突，从而导致报错。

Flags 字段
~~~~~~~~~~

当前仅支持 ``encrypted`` 标记。如果 Flags 字段设置为 ``encrypted``，且已启用 :doc:`Flash Encryption </security/flash-encryption>` 功能，则该分区将会被加密。

.. note:: 

   ``app`` 分区始终会被加密，不管 Flags 字段是否设置。

生成二进制分区表
----------------

烧写到 ESP32 中的分区表采用二进制格式，而不是 CSV 文件本身。此时，:component_file:`partition_table/gen_esp32part.py` 工具可以实现 CSV 和二进制文件之间的转换。

如果您在 ``idf.py menuconfig`` 指定了分区表 CSV 文件的名称，然后执行 ``idf.py partition_table``。这时，转换将在编译过程中自动完成。

手动将 CSV 文件转换为二进制文件:

   python gen_esp32part.py input_partitions.csv binary_partitions.bin

手动将二进制文件转换为 CSV 文件:

   python gen_esp32part.py binary_partitions.bin input_partitions.csv

在标准输出（stdout）上，打印二进制分区表的内容（在运行  ``idf.py partition_table`` 时，我们正是这样打印上文展示的信息摘要的）:

   python gen_esp32part.py binary_partitions.bin

MD5 校验和
~~~~~~~~~~

二进制格式的分区表中含有一个 MD5 校验和。这个 MD5 校验和是根据分区表内容计算的，可在设备启动阶段，用于验证分区表的完整性。

注意，一些版本较老的 bootloader 无法支持 MD5 校验，如果发现 MD5 校验和则将报错 ``invalid magic number 0xebeb``。此时，用户可通过 ``gen_esp32part.py`` 的 ``--disable-md5sum`` 选项或者 ``menuconfig`` 的 :ref:`CONFIG_PARTITION_TABLE_MD5` 选项关闭 MD5 校验。

烧写分区表
----------

-  ``idf.py partition_table-flash`` ：使用 esptool.py 工具烧写分区表。
-  ``idf.py flash`` ：会烧写所有内容，包括分区表。

在执行 ``idf.py partition_table`` 命令时，手动烧写分区表的命令也将打印在终端上。

.. note:: 

   分区表的更新并不会擦除根据之前分区表存储的数据。此时，您可以使用 ``idf.py erase_flash`` 命令或者 ``esptool.py erase_flash`` 命令来擦除 flash 中的所有内容。

.. _secure boot: security/secure-boot.rst
