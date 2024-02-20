分区表
======

:link_to_translation:`en:[English]`

概述
----

每片 {IDF_TARGET_NAME} 的 flash 可以包含多个应用程序，以及多种不同类型的数据（例如校准数据、文件系统数据、参数存储数据等）。因此，我们在 flash 的 :ref:`默认偏移地址 <CONFIG_PARTITION_TABLE_OFFSET>` 0x8000 处烧写一张分区表。

分区表的长度为 0xC00 字节，最多可以保存 95 条分区表条目。MD5 校验和附加在分区表之后，用于在运行时验证分区表的完整性。分区表占据了整个 flash 扇区，大小为 0x1000 (4 KB)。因此，它后面的任何分区至少需要位于 (:ref:`默认偏移地址 <CONFIG_PARTITION_TABLE_OFFSET>`) + 0x1000 处。

分区表中的每个条目都包括以下几个部分：Name（标签）、Type（app、data 等）、SubType 以及在 flash 中的偏移量（分区的加载地址）。

在使用分区表时，最简单的方法就是打开项目配置菜单（``idf.py menuconfig``），并在 :ref:`CONFIG_PARTITION_TABLE_TYPE` 下选择一个预定义的分区表：

-  "Single factory app, no OTA"
-  "Factory app, two OTA definitions"

在以上两种选项中，出厂应用程序均将被烧录至 flash 的 0x10000 偏移地址处。这时，运行 `idf.py partition-table` ，即可以打印当前使用分区表的信息摘要。

内置分区表
------------

以下是 "Single factory app, no OTA" 选项的分区表信息摘要::

   # ESP-IDF Partition Table
   # Name,   Type, SubType, Offset,  Size,   Flags
   nvs,      data, nvs,     0x9000,  0x6000,
   phy_init, data, phy,     0xf000,  0x1000,
   factory,  app,  factory, 0x10000, 1M,

-  flash 的 0x10000 (64 KB) 偏移地址处存放一个标记为 "factory" 的二进制应用程序，且启动加载器将默认加载这个应用程序。
-  分区表中还定义了两个数据区域，分别用于存储 NVS 库专用分区和 PHY 初始化数据。

以下是 "Factory app, two OTA definitions" 选项的分区表信息摘要::

  # ESP-IDF Partition Table
  # Name,   Type, SubType, Offset,  Size, Flags
  nvs,      data, nvs,     0x9000,  0x4000,
  otadata,  data, ota,     0xd000,  0x2000,
  phy_init, data, phy,     0xf000,  0x1000,
  factory,  app,  factory, 0x10000,  1M,
  ota_0,    app,  ota_0,   0x110000, 1M,
  ota_1,    app,  ota_1,   0x210000, 1M,

-  分区表中定义了三个应用程序分区，这三个分区的类型都被设置为 “app”，但具体 app 类型不同。其中，位于 0x10000 偏移地址处的为出厂应用程序 (factory)，其余两个为 OTA 应用程序（ota_0，ota_1）。
-  新增了一个名为 “otadata” 的数据分区，用于保存 OTA 升级时需要的数据。启动加载器会查询该分区的数据，以判断该从哪个 OTA 应用程序分区加载程序。如果 “otadata” 分区为空，则会执行出厂程序。

创建自定义分区表
----------------

如果在 ``menuconfig`` 中选择了 “Custom partition table CSV”，则还需要输入该分区表的 CSV 文件在项目中的路径。CSV 文件可以根据需要，描述任意数量的分区信息。

CSV 文件的格式与上面摘要中打印的格式相同，但是在 CSV 文件中并非所有字段都是必需的。例如下面是一个自定义的 OTA 分区表的 CSV 文件::

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

Name 字段可以是任何有意义的名称，但不能超过 16 个字节，其中包括一个空字节（之后的内容将被截断）。该字段对 {IDF_TARGET_NAME} 并不是特别重要。

Type 字段
~~~~~~~~~

Type 字段可以指定为 app (0x00) 或者 data (0x01)，也可以直接使用数字 0-254（或者十六进制 0x00-0xFE）。注意，0x00-0x3F 不得使用（预留给 esp-idf 的核心功能）。

如果你的应用程序需要以 ESP-IDF 尚未支持的格式存储数据，请在 0x40-0xFE 内添加一个自定义分区类型。

参考 :cpp:type:`esp_partition_type_t` 关于 ``app`` 和 ``data`` 分区的枚举定义。

如果用 C++ 编写，那么指定一个应用程序定义的分区类型，需要在 :cpp:type:`esp_partition_type_t` 中使用整数，从而与 :ref:`分区 API<api-reference-partition-table>` 一起使用。例如::

    static const esp_partition_type_t APP_PARTITION_TYPE_A = (esp_partition_type_t)0x40;

注意，启动加载器将忽略 ``app`` (0x00) 和 ``data`` (0x01) 以外的其他分区类型。

SubType 字段
~~~~~~~~~~~~
{IDF_TARGET_ESP_PHY_REF:default = ":ref:`CONFIG_ESP_PHY_INIT_DATA_IN_PARTITION`", esp32p4, esp32c5 = "NOT UPDATED YET"}

SubType 字段长度为 8 bit，内容与具体分区 Type 有关。目前，esp-idf 仅仅规定了 “app” 和 “data” 两种分区类型的子类型含义。

参考 :cpp:type:`esp_partition_subtype_t`，以了解 ESP-IDF 定义的全部子类型列表，包括：

* 当 Type 定义为 ``app`` 时，SubType 字段可以指定为 ``factory`` (0x00)、 ``ota_0`` (0x10) … ``ota_15`` (0x1F) 或者 ``test`` (0x20)。

   -  ``factory`` (0x00) 是默认的 app 分区。启动加载器将默认加载该应用程序。但如果存在类型为 data/ota 分区，则启动加载器将加载 data/ota 分区中的数据，进而判断启动哪个 OTA 镜像文件。

      -  OTA 升级永远都不会更新 factory 分区中的内容。
      -  如果你希望在 OTA 项目中预留更多 flash，可以删除 factory 分区，转而使用 ota_0 分区。

   -  ota_0 (0x10) … ota_15 (0x1F) 为 OTA 应用程序分区，启动加载器将根据 OTA 数据分区中的数据来决定加载哪个 OTA 应用程序分区中的程序。在使用 OTA 功能时，应用程序应至少拥有 2 个 OTA 应用程序分区（``ota_0`` 和 ``ota_1``）。更多详细信息，请参考 :doc:`OTA 文档 </api-reference/system/ota>` 。
   -  ``test`` (0x20) 为预留的子类型，用于工厂测试流程。如果没有其他有效 app 分区，test 将作为备选启动分区使用。也可以配置启动加载器在每次启动时读取 GPIO，如果 GPIO 被拉低则启动该分区。详细信息请查阅 :ref:`bootloader_boot_from_test_firmware`。

* 当 Type 定义为 ``data`` 时，SubType 字段可以指定为 ``ota`` (0x00)、``phy`` (0x01)、``nvs`` (0x02)、``nvs_keys`` (0x04) 或者其他组件特定的子类型（请参考 :cpp:type:`子类型枚举 <esp_partition_subtype_t>`).

   -  ``ota`` (0) 即 :ref:`OTA 数据分区 <ota_data_partition>` ，用于存储当前所选的 OTA 应用程序的信息。这个分区的大小需要设定为 0x2000。更多详细信息，请参考 :doc:`OTA 文档 <../api-reference/system/ota>` 。
   -  ``phy`` (1) 分区用于存放 PHY 初始化数据，从而保证可以为每个设备单独配置 PHY，而非必须采用固件中的统一 PHY 初始化数据。

      -  默认配置下，phy 分区并不启用，而是直接将 phy 初始化数据编译至应用程序中，从而节省分区表空间（直接将此分区删掉）。
      -  如果需要从此分区加载 phy 初始化数据，请打开项目配置菜单（``idf.py menuconfig``），并且使能 {IDF_TARGET_ESP_PHY_REF} 选项。此时，还需要手动将 phy 初始化数据烧至设备 flash（esp-idf 编译系统并不会自动完成该操作）。
   -  ``nvs`` (2) 是专门给 :doc:`非易失性存储 (NVS) API <../api-reference/storage/nvs_flash>` 使用的分区。

      -  用于存储每台设备的 PHY 校准数据（注意，并不是 PHY 初始化数据）。

      .. only:: SOC_WIFI_SUPPORTED

          -  用于存储 Wi-Fi 数据（如果使用了 :doc:`esp_wifi_set_storage(WIFI_STORAGE_FLASH) <../api-reference/network/esp_wifi>` 初始化函数）。

      -  NVS API 还可以用于其他应用程序数据。
      -  强烈建议为 NVS 分区分配至少 0x3000 字节空间。
      -  如果使用 NVS API 存储大量数据，请增加 NVS 分区的大小（默认是 0x6000 字节）。
   -  ``nvs_keys`` (4) 是 NVS 秘钥分区。详细信息，请参考 :doc:`非易失性存储 (NVS) API <../api-reference/storage/nvs_flash>` 文档。

      -  用于存储加密密钥（如果启用了 `NVS 加密` 功能）。
      -  此分区应至少设定为 4096 字节。

  - ESP-IDF 还支持其他用于数据存储的预定义子类型，包括：

    - ``coredump`` (0x03) 用于在使用自定义分区表 CSV 文件时存储核心转储，详情请参阅 :doc:`/api-guides/core_dump`。
    - ``efuse`` (0x05) 使用 :ref:`虚拟 eFuse <virtual-efuses>` 模拟 eFuse 位。
    - ``undefined`` (0x06) 隐式用于未指定子类型（即子类型为空）的数据分区，但也可显式将其标记为未定义。
    - ``fat`` (0x81) 用于 :doc:`/api-reference/storage/fatfs`。
    - ``spiffs`` (0x82) 用于 :doc:`/api-reference/storage/spiffs`。
    - ``littlefs`` (0x83) 用于 `LittleFS 文件系统 <https://github.com/littlefs-project/littlefs>`_，详情可参阅 :example:`storage/littlefs` 示例。

.. Comment: ``esphttpd`` (0x80) was not added to the list because there is no docs section for it and it is not clear whether user should use it explicitly.

  其它数据子类型已预留给 ESP-IDF 未来使用。

* 如果分区类型是由应用程序定义的任意值 (0x40-0xFE)，那么 ``subtype`` 字段可以是由应用程序选择的任何值 (0x00-0xFE)。

  请注意，如果用 C++ 编写，应用程序定义的子类型值需要转换为 :cpp:type:`esp_partition_type_t`，从而与 :ref:`分区 API <api-reference-partition-table>` 一起使用。

额外分区 SubType 字段
~~~~~~~~~~~~~~~~~~~~~~~~

组件可以通过设置 ``EXTRA_PARTITION_SUBTYPES`` 属性来定义额外的分区子类型。 ``EXTRA_PARTITION_SUBTYPES`` 是一个 CMake 列表，其中的每个条目由字符串组成，以逗号为分隔，格式为 ``<type>, <subtype>, <value>``。构建系统通过该属性会自动添加额外的子类型，并在 :cpp:type:`esp_partition_subtype_t` 中插入名为 ``ESP_PARTITION_SUBTYPE_<type>_<subtype>`` 的字段。项目可以使用这个子类型来定义分区表 CSV 文件中的分区，并使用 :cpp:type:`esp_partition_subtype_t` 中的新字段。

.. _partition-offset-and-size:

偏移地址 (Offset) 和 大小 (Size) 字段
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. list::

   - 偏移地址表示 SPI flash 中的分区地址，扇区大小为 0x1000 (4 KB)。因此，偏移地址必须是 4 KB 的倍数。
   - 若 CSV 文件中的分区偏移地址为空，则该分区会接在前一个分区之后；若为首个分区，则将接在分区表之后。
   - ``app`` 分区的偏移地址必须与 0x10000 (64 KB) 对齐。如果偏移字段留空，则 ``gen_esp32part.py`` 工具会自动计算得到一个满足对齐要求的偏移地址。如果 ``app`` 分区的偏移地址没有与 0x10000 (64 KB) 对齐，则该工具会报错。
   - ``app`` 分区的大小必须与 flash 扇区大小对齐。为 ``app`` 分区指定未对齐的大小将返回错误。
   :SOC_SECURE_BOOT_V1: - 若启用了安全启动 V1，则 ``app`` 分区的大小需与 0x10000 (64 KB) 对齐。
   - ``app`` 分区的大小和偏移地址可以采用十进制数或是以 0x 为前缀的十六进制数，且支持 K 或 M 的倍数单位（K 和 M 分别代表 1024 和 1024*1024 字节）。

如果你希望允许分区表中的分区采用任意起始偏移量 (:ref:`CONFIG_PARTITION_TABLE_OFFSET`)，请将分区表（CSV 文件）中所有分区的偏移字段都留空。注意，此时，如果你更改了分区表中任意分区的偏移地址，则其他分区的偏移地址也会跟着改变。这种情况下，如果你之前还曾设定某个分区采用固定偏移地址，则可能造成分区表冲突，从而导致报错。

Flags 字段
~~~~~~~~~~

目前支持 ``encrypted`` 和 ``readonly`` 标记：

  - 如果 Flags 字段设置为 ``encrypted``，且已启用 :doc:`/security/flash-encryption` 功能，则该分区将会被加密。

  .. note::

      无论是否设置 Flags 字段，``app`` 分区都将保持加密。

  - 如果 Flags 字段设置为 ``readonly``，则该分区为只读分区。``readonly`` 标记仅支持除 ``ota`` 和 ``coredump`` 子类型外的 ``data`` 分区。使用该标记，防止意外写入如出厂数据分区等包含关键设备特定配置数据的分区。

  .. note::

      在任何写入模式下 (``w``、``w+``、``a``、``a+``、``r+``)，尝试通过 C 文件 I/O API 打开文件 (``fopen```) 的操作都将失败并返回 ``NULL``。除 ``O_RDONLY`` 外，``open`` 与任何标志一同使用都将失败并返回 ``-1``，全局变量 ``errno`` 也将设置为 ``EROFS``。上述情况同样适用于通过其他 POSIX 系统调用函数执行写入或擦除的操作。在只读分区上，以读写模式打开 NVS 的句柄将失败并返回 :c:macro:`ESP_ERR_NOT_ALLOWED` 错误代码，使用 ``esp_partition`` 或 ``spi_flash`` 等较低级别的 API 进行写入操作也将返回 :c:macro:`ESP_ERR_NOT_ALLOWED` 错误代码。

可以使用冒号连接不同的标记，来同时指定多个标记，如 ``encrypted:readonly``。

生成二进制分区表
----------------

烧写到 {IDF_TARGET_NAME} 中的分区表采用二进制格式，而不是 CSV 文件本身。此时，:component_file:`partition_table/gen_esp32part.py` 工具可以实现 CSV 和二进制文件之间的转换。

如果你在项目配置菜单（``idf.py menuconfig``）中设置了分区表 CSV 文件的名称，然后构建项目或执行 ``idf.py partition-table``。这时，转换将在编译过程中自动完成。

手动将 CSV 文件转换为二进制文件::

   python gen_esp32part.py input_partitions.csv binary_partitions.bin

手动将二进制文件转换为 CSV 文件::

   python gen_esp32part.py binary_partitions.bin input_partitions.csv

在标准输出 (stdout) 上，打印二进制分区表的内容（运行  ``idf.py partition-table`` 时展示的信息摘要也是这样生成的）::

   python gen_esp32part.py binary_partitions.bin

分区大小检查
---------------------

ESP-IDF 构建系统将自动检查生成的二进制文件大小与可用的分区大小是否匹配，如果二进制文件太大，则会构建失败并报错。

目前会对以下二进制文件进行检查：

* 引导加载程序的二进制文件的大小要适合分区表前的区域大小（分区表前的区域都分配给了引导加载程序），具体请参考 :ref:`bootloader-size`。
* 应用程序二进制文件应至少适合一个 “app" 类型的分区。如果不适合任何应用程序分区，则会构建失败。如果只适合某些应用程序分区，则会打印相关警告。

.. note::

   即使分区大小检查返回错误并导致构建失败，仍然会生成可以烧录的二进制文件（它们对于可用空间来说过大，因此无法正常工作）。

MD5 校验和
~~~~~~~~~~

二进制格式的分区表中含有一个 MD5 校验和。这个 MD5 校验和是根据分区表内容计算的，可在设备启动阶段，用于验证分区表的完整性。

.. only:: esp32

   用户可通过 ``gen_esp32part.py`` 的 ``--disable-md5sum`` 选项或者 :ref:`CONFIG_PARTITION_TABLE_MD5` 选项关闭 MD5 校验。对于 :ref:`ESP-IDF v3.1 版本前的引导加载程序 <CONFIG_APP_COMPATIBLE_PRE_V3_1_BOOTLOADERS>`，因为它不支持 MD5 校验，所以无法正常启动并报错 ``invalid magic number 0xebeb``，此时用户可以使用此选项关闭 MD5 校验。

.. only:: not esp32

    用户可通过 ``gen_esp32part.py`` 的 ``--disable-md5sum`` 选项或者 :ref:`CONFIG_PARTITION_TABLE_MD5` 选项关闭 MD5 校验。


烧写分区表
----------

-  ``idf.py partition-table-flash`` ：使用 esptool.py 工具烧写分区表。
-  ``idf.py flash`` ：会烧写所有内容，包括分区表。

在执行 ``idf.py partition-table`` 命令时，手动烧写分区表的命令也将打印在终端上。

.. note::

   分区表的更新并不会擦除根据旧分区表存储的数据。此时，可以使用 ``idf.py erase-flash`` 命令或者 ``esptool.py erase_flash`` 命令来擦除 flash 中的所有内容。


分区工具 (``parttool.py``)
---------------------------

`partition_table` 组件中有分区工具 :component_file:`parttool.py<partition_table/parttool.py>`，可以在目标设备上完成分区相关操作。该工具有如下用途：

  - 读取分区，将内容存储到文件中 (read_partition)
  - 将文件中的内容写至分区 (write_partition)
  - 擦除分区 (erase_partition)
  - 检索特定分区的名称、偏移、大小和 flag（“加密”）标志等信息 (get_partition_info)

用户若想通过编程方式完成相关操作，可从另一个 Python 脚本导入并使用分区工具，或者从 Shell 脚本调用分区工具。前者可使用工具的 Python API，后者可使用命令行界面。

Python API
~~~~~~~~~~~

首先请确保已导入 `parttool` 模块。

.. code-block:: python

  import sys
  import os

  idf_path = os.environ["IDF_PATH"]  # 从环境中获取 IDF_PATH 的值
  parttool_dir = os.path.join(idf_path, "components", "partition_table")  # parttool.py 位于 $IDF_PATH/components/partition_table 下

  sys.path.append(parttool_dir)  # 使能 Python 寻找 parttool 模块
  from parttool import *  # 导入 parttool 模块内的所有名称

要使用分区工具的 Python API，第一步是创建 `ParttoolTarget`：

.. code-block:: python

  # 创建 parttool.py 的目标设备，并将目标设备连接到串行端口 /dev/ttyUSB1
  target = ParttoolTarget("/dev/ttyUSB1")

现在，可使用创建的 `ParttoolTarget` 在目标设备上完成操作：

.. code-block:: python

  # 擦除名为 'storage' 的分区
  target.erase_partition(PartitionName("storage"))

  # 读取类型为 'data'、子类型为 'spiffs' 的分区，保存至文件 'spiffs.bin'
  target.read_partition(PartitionType("data", "spiffs"), "spiffs.bin")

  # 将 'factory.bin' 文件的内容写至 'factory' 分区
  target.write_partition(PartitionName("factory"), "factory.bin")

  # 打印默认启动分区的大小
  storage = target.get_partition_info(PARTITION_BOOT_DEFAULT)
  print(storage.size)

使用 `PartitionName`、`PartitionType` 或 PARTITION_BOOT_DEFAULT 指定要操作的分区。顾名思义，这三个参数可以指向拥有特定名称的分区、特定类型和子类型的分区或默认启动分区。

更多关于 Python API 的信息，请查看分区工具的代码注释。

命令行界面
~~~~~~~~~~

`parttool.py` 的命令行界面具有如下结构：

.. code-block:: bash

  parttool.py [command-args] [subcommand] [subcommand-args]

  - command-args - 执行主命令 (parttool.py) 所需的实际参数，多与目标设备有关
  - subcommand - 要执行的操作
  - subcommand-args - 所选操作的实际参数

.. code-block:: bash

  # 擦除名为 'storage' 的分区
  parttool.py --port "/dev/ttyUSB1" erase_partition --partition-name=storage

  # 读取类型为 'data'、子类型为 'spiffs' 的分区，保存到 'spiffs.bin' 文件
  parttool.py --port "/dev/ttyUSB1" read_partition --partition-type=data --partition-subtype=spiffs --output "spiffs.bin"

  # 将 'factory.bin' 文件中的内容写入到 'factory' 分区
  parttool.py --port "/dev/ttyUSB1" write_partition --partition-name=factory --input "factory.bin"

  # 打印默认启动分区的大小
  parttool.py --port "/dev/ttyUSB1" get_partition_info --partition-boot-default --info size

更多信息可用 `--help` 指令查看：

.. code-block:: bash

  # 显示可用的子命令和主命令描述
  parttool.py --help

  # 显示子命令的描述
  parttool.py [subcommand] --help

.. _secure boot: security/secure-boot-v1.rst
