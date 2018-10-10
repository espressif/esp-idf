分区表
======

概述
----

单个 ESP32 的闪存可以包含多个应用程序，以及多种不同类型的数据（例如校准数据、文件系统、参数存储器等）。因此，我们需要在闪存的 :ref:`默认偏移地址 <CONFIG_PARTITION_TABLE_OFFSET>` 0x8000 处烧写一张分区表。

分区表的长度为 0xC00 字节（最多可以保存 95 个分区表条目），在分区表数据的后面保存着它的 MD5 校验和，它用于验证分区表的完整性。如果使用了ESP32的 :doc:`安全启动 </security/secure-boot>` 功能，那么分区表还会被签名，该签名被附加在分区表的后面。

分区表中的每个条目都会包含名称（标签）、类型（应用程序、数据等）、子类型以及在闪存中的偏移量（分区的加载地址）。

使用分区表最简单的方法就是通过 `make menuconfig` 选择一张预定义的分区表：

-  "Single factory app, no OTA"
-  "Factory app, two OTA definitions"

在这两种情况下，出厂应用程序被烧写到闪存的 0x10000 偏移地址处，运行 ``make partition_table`` 命令可以打印出当前使用的分区表的信息摘要。

内置分区表
------------

以下是 "Single factory app, no OTA" 配置打印的信息摘要：

.. code:: bash

   # Espressif ESP32 Partition Table
   # Name,   Type, SubType, Offset,  Size,   Flags
   nvs,      data, nvs,     0x9000,  0x6000,
   phy_init, data, phy,     0xf000,  0x1000,
   factory,  app,  factory, 0x10000, 1M,

-  闪存的 0x10000 (64KB) 偏移地址处存放二进制应用程序，它被标记为 "factory"，引导程序默认会加载运行该分区的应用程序。
-  分区表中还定义了两个数据区域，分别用于存储 NVS 库专用分区和 PHY 初始化数据。

以下是 "Factory app, two OTA definitions" 配置打印的信息摘要：

.. code:: bash

   # Espressif ESP32 Partition Table
   # Name,   Type, SubType, Offset,   Size,   Flags
   nvs,      data, nvs,     0x9000,   0x4000,
   otadata,  data, ota,     0xd000,   0x2000,
   phy_init, data, phy,     0xf000,   0x1000,
   factory,  app,  factory, 0x10000,  1M,
   ota_0,    app,  ota_0,   0x110000, 1M,
   ota_1,    app,  ota_1,   0x210000, 1M,

-  分区表中定义了三个应用程序分区，这三个分区的类型都被设置为 “app”，但是子类型不同。出厂应用程序（factory）位于 0x10000 偏移地址处，其余两个是 OTA 应用程序（ota_0，ota_1）。
-  新增了一个名为 “otadata” 的数据分区，用于保存 OTA 升级时候需要的数据。引导程序会查询该分区的数据用以判断该从哪个 OTA 应用程序分区加载程序。如果 “otadata” 分区是空的，则会执行出厂程序。

创建自定义分区表
----------------

如果在 menuconfig 中选择了 “Custom partition table CSV”，则还需要输入该分区表的 CSV 文件在项目中的路径。CSV 文件可以根据需要描述任意数量的分区信息。

CSV 文件的格式与上面摘要中打印的格式相同，但是在 CSV 文件中并非所有字段都是必需的。例如下面是一个自定义的 OTA 分区表的 CSV 文件：

.. code:: bash

   # Name,   Type, SubType,  Offset,   Size,  Flags
   nvs,      data, nvs,      0x9000,  0x4000
   otadata,  data, ota,      0xd000,  0x2000
   phy_init, data, phy,      0xf000,  0x1000
   factory,  app,  factory,  0x10000,  1M
   ota_0,    app,  ota_0,    ,         1M
   ota_1,    app,  ota_1,    ,         1M
   nvs_key,  data, nvs_keys, ,        0x1000

-  字段之间的空格会被忽略，任何以 ``#`` 开头的行（注释）也会被忽略。
-  CSV 文件中的每个非注释行都定义了一个分区。
-  每个分区的 ``Offset`` 字段可以为空，``gen_esp32part.py`` 工具会从分区表位置的后面开始自动计算并填充该分区的偏移地址，同时确保每个分区的偏移地址正确对齐。

Name 字段
~~~~~~~~~

Name 字段可以是任何有意义的名称，这对 ESP32 来说并不是特别重要。超过 16 个字符长度的名字将会被截断。

Type 字段
~~~~~~~~~

Type 字段可以指定为 app (0) 或者 data (1)，甚至可以直接使用数字 0-254（或者十六进制 0x00-0xFE）来指定。类型 0x00-0x3F 被保留用于 esp-idf 的核心功能。

如果您的应用程序需要保存数据，请在 0x40-0xFE 的范围内添加自定义的分区类型。

引导程序会忽略 app (0) 和 data (1) 以外的任何分区类型。

SubType 字段
~~~~~~~~~~~~

8 比特的子类型字段与给定的分区类型有关，esp-idf 目前仅仅指定了 “app” 和 “data” 分区的子类型字段。

app 子类型
~~~~~~~~~~~~

当类型被定义为 ``app`` 时，子类型字段可以指定为 factory (0)，ota_0 (0x10) ... ota_15 (0x1F) 或者 test (0x20)。

-  factory (0) 是默认的 app 分区，如果不存在 data/ota 分区，引导程序会运行 app/factory 分区中的程序。如果存在 data/ota 分区，则会读取该分区的数据，进而判断应该启动哪个 OTA 镜像。

   -  OTA 升级永远都不会更新 factory 分区中的程序。
   -  如果想节省 OTA 项目中的闪存，可以删除 factory 分区，使用 ota_0 分区替代。
-  ota_0 (0x10) ... ota_15 (0x1F) 便是 OTA 应用程序分区，引导程序根据 OTA 数据分区中的数据来决定从哪个 OTA 应用程序分区中加载程序。如果使用了 OTA 功能，那么至少需要包含两个 OTA 应用程序分区（ota_0 和 ota_1）。更多详细信息请参考 :doc:`OTA 文档 </api-reference/system/ota>` 。
-  test (0x2) 是工厂测试过程的保留子类型，当前 esp-idf 的引导程序并不支持这种子类型。

data 子类型
~~~~~~~~~~~~~

当类型被定义为 ``data`` 时，子类型字段可以指定为 ota (0)，phy (1)，nvs (2) 和 nvs_keys (4)。

-  ota (0) 即 :ref:`OTA 数据分区 <ota_data_partition>` ，用于存储当前所选的 OTA 应用程序的信息。这个分区的大小需要设定为 0x2000。更多详细信息请参考 :doc:`OTA 文档 <../api-reference/system/ota>` 。
-  phy (1) 分区用于存放 PHY 初始化数据，这样就可以为每个设备（而不是在固件中）单独配置 PHY，而不是在固件中设定好PHY的初始化数据。

   -  在默认的配置中并不使用 phy 分区， PHY 的初始化数据被编译进了应用程序中。因此可以从分区表中删除此分区以节省空间。
   -  如果要从此分区加载 PHY 初始化数据，请运行 ``make menuconfig`` 并且使能 :ref:`CONFIG_ESP32_PHY_INIT_DATA_IN_PARTITION` 选项。您还需要将 PHY 的初始化数据手动烧写到闪存中，esp-idf 编译系统不会自动完成该操作。
-  nvs (2) 是专门给 :doc:`非易失性存储 (NVS) API <../api-reference/storage/nvs_flash>` 使用的分区。

   -  NVS 用于存储每台设备的 PHY 校准数据（区别于 PHY 初始化数据）。
   -  NVS 用于存储 WiFi 数据（如果使用了 :cpp:func:`esp_wifi_set_storage(WIFI_STORAGE_FLASH) <esp_wifi_set_storage>` 初始化函数）。
   -  NVS API 还可以用于其他应用程序数据。
   -  强烈建议您在项目中分配至少 0x3000 字节的空间给 NVS 分区。
   -  如果使用 NVS API 来存储大量数据，请增加 NVS 分区的大小（默认是 0x6000 字节）。
-  nvs_keys (4) 是 NVS 秘钥分区。详细信息请参考 :doc:`非易失性存储 (NVS) API <../api-reference/storage/nvs_flash>` 文档。

   -  它用于在启用 `NVS 加密` 功能的时候存储加密密钥。
   -  此分区的大小需要设定为 4096 字节（即最小的分区大小）。

其它数据子类型保留给未来的 esp-idf 使用。

Offset 和 Size 字段
~~~~~~~~~~~~~~~~~~~

没有指定偏移地址的分区会紧跟在前一个分区之后，如果第一个分区没有指定偏移地址，那么该分区会紧跟在分区表之后。

应用程序分区的偏移地址必须要与 0x10000 (64K) 对齐，如果将偏移字段留空， ``gen_esp32part.py`` 工具会自动计算得到一个满足对齐要求的偏移地址。如果给应用程序的分区指定了未对齐的偏移地址，该工具会报错。

大小和偏移可以以十进制形式指定，也可以以 0x 为前缀的十六进制形式指定，或者以 K 或 M 作为单位指定（分别代表 1024 和 1024*1024 字节）。

如果希望分区表中的分区可以使用任何的起始偏移量 (:ref:`CONFIG_PARTITION_TABLE_OFFSET`)，请将分区表（CSV文件）中所有分区的偏移字段都留空。类似的，如果更改了分区表的偏移地址，则要注意所有留空的分区偏移可能会被更改到新的合适的位置，此时一些固定的偏移地址可能会与分区表冲突，从而导致错误。

Flags 字段
~~~~~~~~~~

当前仅支持 ``encrypted`` 标记，如果标记字段被设置为了 ``encrypted``，那么该分区将会被加密（假如启用了 :doc:`闪存加密 </security/flash-encryption>` 的功能）。

.. note:: ``app`` 分区始终会被加密，不管标记字段是否被设置。

生成二进制分区表
----------------

烧写到 ESP32 中的分区表是二进制格式的，而不是 CSV 文件本身。:component_file:`partition_table/gen_esp32part.py` 工具可以用来在 CSV 文件和二进制文件之间进行转换。

如果您在 ``make menuconfig`` 中指定了 CSV 分区表的名称，然后执行 ``make partition_table`` 那么该转化过程会在编译的过程中自动完成。

手动将 CSV 转换为 二进制文件：

.. code:: bash

   python gen_esp32part.py input_partitions.csv binary_partitions.bin

将二进制文件转换为 CSV 文件：

.. code:: bash

   python gen_esp32part.py binary_partitions.bin input_partitions.csv

在标准输出（stdout）上打印二进制分区表的内容（这正是执行 ``make partition_table`` 时显示的信息摘要）：

.. code:: bash

   python gen_esp32part.py binary_partitions.bin

MD5 校验和
~~~~~~~~~~

二进制格式的分区表中包含有基于分区表计算得到的 MD5 校验和，此校验和用于在程序引导阶段检查分区表的完整性。

可以通过 ``gen_esp32part.py`` 的 ``--disable-md5sum`` 选项或者 ``menuconfig`` 中的 :ref:`CONFIG_PARTITION_TABLE_MD5` 选项来禁止生成 MD5 校验和。当老版本的引导程序无法处理 MD5 校验和而失败，并且打印错误信息 ``invalid magic number 0xebeb`` 的时候就需要禁止 MD5 。

烧写分区表
----------

-  ``make partition_table-flash`` ：使用 esptool.py 工具烧写分区表。
-  ``make flash`` ：会烧写所有内容，包括分区表。

手动烧写的命令会作为 ``make partition_table`` 命令执行过程的一部分被打印在终端上。

.. note:: 更新分区表的时候不会删除旧分区表存储的数据，您可以使用 ``make erase_flash`` 命令或者 ``esptool.py erase_flash`` 命令来擦除整块闪存的内容。
