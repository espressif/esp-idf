eFuse 管理器
=============

:link_to_translation:`en:[English]`

{IDF_TARGET_CODING_SCHEMES:default="Reed-Solomon", esp32="3/4 或 Repeat"}


简介
------------

eFuse 是一种微型的一次性可编程保险丝，可以通过“烧录”（即编程）将数据存储到 {IDF_TARGET_NAME} 中。eFuse 位组成不同的数据字段，用于系统参数（即 {IDF_TARGET_NAME} 的 ESP-IDF 使用的数据参数）或用户自定义参数。

eFuse 管理器组件中集合了多种工具和 API，可帮助定义、烧录和访问 eFuse 参数。常用的工具和 API 包括：

* 表格格式，用于在 CSV 文件中定义 eFuse 数据字段
* ``efuse_table_gen.py`` 工具，用于生成 CSV 文件指定的 eFuse 数据字段对应的 C 结构体
* 用于读/写 eFuse 数据字段的 C API 集合

eFuse Manager 与 ``idf.py``
---------------------------

``idf.py`` 通过 ``idf.py efuse-<subcommand>`` 命令为 eFuse 管理器提供了部分功能。本文档主要使用基于 ``idf.py`` 的命令，只有在涉及高级功能或罕见情况时，会使用基于 ``espefuse.py`` 的命令。要查看所有可用的命令，请运行 ``idf.py --help`` 并搜索以 ``efuse-`` 为前缀的命令。

硬件描述
--------------------

{IDF_TARGET_NAME} 有多个 eFuse，可用于存储系统参数和用户参数。每个 eFuse 都是一个一位字段，可以烧写为 1，之后就不能再恢复为 0。eFuse 位被分成了多个 256 位的块，每个块又被分成 8 个 32 位寄存器。部分块保留用于系统参数，其它块可用于用户参数。

如需了解更多内容，请参阅 *{IDF_TARGET_NAME} 技术参考手册* > *eFuse 控制器 (eFuse)* [`PDF <{IDF_TARGET_TRM_CN_URL}#efuse>`__]。

.. only:: esp32

    {IDF_TARGET_NAME} 有 4 个 eFuse 块，每个块的大小为 256 位（并非所有位都可用于用户参数）：

    * EFUSE_BLK0 完全用于系统用途；
    * EFUSE_BLK1 用于 flash 加密密钥。如果不使用 flash 加密功能，此块也可以用于用户参数；
    * EFUSE_BLK2 用于安全启动密钥。如果不使用安全启动功能，此块也可以用于用户参数；
    * EFUSE_BLK3 可以部分保留，以存储自定义 MAC 地址，或者完全用于用户参数。请注意，一些位已经用于 ESP-IDF。

.. only:: not esp32 and not esp32c2

    {IDF_TARGET_NAME} 有 11 个 eFuse 块，每个块的大小为 256 位（并非所有位都可用于用户参数）：

    .. list::

        * EFUSE_BLK0 完全用于系统参数；
        * EFUSE_BLK1 完全用于系统参数；
        * EFUSE_BLK2 完全用于系统参数；
        * EFUSE_BLK3（也称为 EFUSE_BLK_USER_DATA）可用于用户参数；
        * EFUSE_BLK4 至 EFUSE_BLK8（即 EFUSE_BLK_KEY0 至 EFUSE_BLK_KEY4）可以存储安全启动或 flash 加密的密钥。不使用这两个功能时，可以用于用户参数。
        :SOC_EFUSE_BLOCK9_KEY_PURPOSE_QUIRK and SOC_ECDSA_SUPPORTED: * EFUSE_BLK9（即 EFUSE_BLK_KEY5）可用于任何用途，但由于硬件问题，不能用于 flash 加密或 ECDSA；
        :SOC_EFUSE_BLOCK9_KEY_PURPOSE_QUIRK and not SOC_ECDSA_SUPPORTED: * EFUSE_BLK9（即 EFUSE_BLK_KEY5）可用于任何用途，但由于硬件问题，不能用于 flash 加密；
        :not SOC_EFUSE_BLOCK9_KEY_PURPOSE_QUIRK: * EFUSE_BLK9（即 EFUSE_BLK_KEY5）可用于存储安全启动或 flash 加密的密钥。不使用这两个功能时，可用于用户参数。
        * EFUSE_BLK10（即 EFUSE_BLK_SYS_DATA_PART2）保留用于系统参数。

.. only:: esp32c2

    {IDF_TARGET_NAME} 有 4 个 eFuse 块，每个块的大小为 256 位（并非所有位都可用于用户参数）：

    * EFUSE_BLK0 完全用于系统参数
    * EFUSE_BLK1 完全用于系统参数
    * EFUSE_BLK2 完全用于系统参数
    * EFUSE_BLK3（即 EFUSE_BLK_KEY0）可用于存储安全启动或 flash 加密的密钥。不使用这两个功能时，可用于用户参数。

定义 eFuse 字段
-----------------------

eFuse 字段通过 CSV 文件中特定格式的表格进行定义。通过这种格式，可定义任意长度和任意位数的 eFuse 字段。

另外，通过这种格式，可以结构化地定义由子字段组成的 eFuse 字段，这意味着一个父 eFuse 字段可能由占用相同 eFuse 位的多个子 eFuse 字段组成。

定义格式
^^^^^^^^^^^^^^^^^^

一般情况下，每个记录在定义表格中占据一行，每行包含以下值（也就是列）：

{IDF_TARGET_MAX_EFUSE_BLK:default = "EFUSE_BLK10", esp32 = "EFUSE_BLK3", esp32c2 = "EFUSE_BLK3"}

.. code-block:: none

    # field_name, efuse_block(EFUSE_BLK0..{IDF_TARGET_MAX_EFUSE_BLK}), bit_start(0..255), bit_count(1..256), comment

- ``field_name``

    - eFuse 字段的名称。
    - 字段名称前会自动添加 ``ESP_EFUSE_`` 前缀，在 C 代码中，可通过此名称来访问 eFuse 字段。
    - 对于每个 eFuse 字段，``field_name`` 必须唯一。
    - 如果此值为空，说明该行与前一行合并。这样就可以定义任意位排序的 eFuse 字段（例如通用表中的 ``MAC_FACTORY`` 字段）。
    - 使用 ``.`` 来定义一个子 eFuse 字段。详情请参考 :ref:`structured-efuse-fields`。

- ``efuse_block``

    - eFuse 字段的块号。例如 EFUSE_BLK0 到 {IDF_TARGET_MAX_EFUSE_BLK}。
    - 这一字段决定了 eFuse 字段在哪个块中。

- ``bit_start``

    - eFuse 字段在块内的位置偏移（0 至 255 位）。
    - ``bit_start`` 是可选项，可省略。

        - 当省略时，如果上一条记录位于同一个 eFuse 块中，``bit_start`` 会被设置为上一条记录的 ``bit_start + bit_count``。
        - 如果上一条记录位于不同的 eFuse 块中，则会报错。

- ``bit_count``

    - eFuse 字段的大小，以位为单位（1 至 N）。
    - ``bit_count`` 不能省略
    - 如将其设置为 ``MAX_BLK_LEN``，则此 eFuse 字段为块中允许的最大 eFuse 字段大小。

        .. only:: esp32

            - ``MAX_BLK_LEN`` 考虑了 eFuse 的编码方案。
            - 根据 :ref:`CONFIG_EFUSE_CODE_SCHEME_SELECTOR` 选择的编码方案，``MAX_BLK_LEN`` 可能是 256("None")、192 ("3/4") 或 128 ("REPEAT") 位。

- ``comment``

    - 描述 eFuse 字段的注释。
    - 此注释会逐字复制到 C 头文件中。

如果一个 eFuse 字段需要非顺序位序，那么该 eFuse 字段将占用多行。应在第一行的 ``field_name`` 处定该 eFuse 字段的名称，并且将其他行的 ``field_name`` 留空。这样就表明这些行属于同一个 eFuse 字段。

以下示例中定义了两个 eFuse 字段。首先定义非连续的 eFuse 字段 ``MAC_FACTORY``，然后定义了常规 eFuse 字段 ``MAC_FACTORY_CRC``：

.. code-block:: none

    # Factory MAC address #
    #######################
    MAC_FACTORY,            EFUSE_BLK0,    72,    8,    Factory MAC addr [0]
    ,                       EFUSE_BLK0,    64,    8,    Factory MAC addr [1]
    ,                       EFUSE_BLK0,    56,    8,    Factory MAC addr [2]
    ,                       EFUSE_BLK0,    48,    8,    Factory MAC addr [3]
    ,                       EFUSE_BLK0,    40,    8,    Factory MAC addr [4]
    ,                       EFUSE_BLK0,    32,    8,    Factory MAC addr [5]
    MAC_FACTORY_CRC,        EFUSE_BLK0,    80,    8,    CRC8 for factory MAC address

在 C 代码中，可以通过 ``ESP_EFUSE_MAC_FACTORY`` 和 ``ESP_EFUSE_MAC_FACTORY_CRC`` 使用这两个字段。

.. _structured-efuse-fields:

结构化 eFuse 字段
-----------------------

通常情况下，一个 eFuse 字段代表一个特定的参数。不过，在某些情况下，一个 eFuse 字段可能由多个子字段组成，因此有必要隔离访问这些子字段。例如，如果一个 eFuse 字段包含一个浮点参数，则可以将浮点的符号、指数和尾数字段作为单独的 eFuse 字段进行访问。

因此，可以在 ``field_name`` 中使用 ``.`` 操作符，以结构化的方式定义 eFuse 字段。例如，``XX.YY.ZZ`` 定义了一个 eFuse 字段 ``ZZ``，它是 eFuse 字段 ``YY`` 的子字段，而 ``YY`` 又是 eFuse 字段 ``XX`` 的子字段。

以下示例展示了如何以定义结构化 eFuse 字段：

.. code-block:: none

    WR_DIS,                           EFUSE_BLK0,   0,    32,     Write protection
    WR_DIS.RD_DIS,                    EFUSE_BLK0,   0,    1,      Write protection for RD_DIS
    WR_DIS.FIELD_1,                   EFUSE_BLK0,   1,    1,      Write protection for FIELD_1
    WR_DIS.FIELD_2,                   EFUSE_BLK0,   2,    4,      Write protection for FIELD_2 (includes B1 and B2)
    WR_DIS.FIELD_2.B1,                EFUSE_BLK0,   2,    2,      Write protection for FIELD_2.B1
    WR_DIS.FIELD_2.B2,                EFUSE_BLK0,   4,    2,      Write protection for FIELD_2.B2
    WR_DIS.FIELD_3,                   EFUSE_BLK0,   5,    1,      Write protection for FIELD_3
    WR_DIS.FIELD_3.ALIAS,             EFUSE_BLK0,   5,    1,      Write protection for FIELD_3 (just a alias for WR_DIS.FIELD_3)
    WR_DIS.FIELD_4,                   EFUSE_BLK0,   7,    1,      Write protection for FIELD_4

在上例中可以看出：

* ``WR_DIS`` 为父 eFuse 字段。其他所有行的 ``field_name`` 都具有 ``WR_DIS.`` 前缀，因此都是 ``WR_DIS`` 的子字段。
* 子字段必须与父字段使用相同的位。注意子字段和父字段的 ``bit_start`` 和 ``bit_count``：

    * 子字段的位总是在其父字段范围内。例如，``WR_DIS.RD_DIS`` 和 ``WR_DIS.RD_DIS`` 占用了 ``WR_DIS`` 的第一位和第二位。
    * 子字段使用的位不能重叠（子字段有别名时除外）。

* 可以将别名创建为子字段。例如，``WR_DIS.FIELD_3.ALIAS`` 既是 ``WR_DIS.FIELD_3`` 的子字段，又是别名，因为它们占用的位相同。

所有 eFuse 字段最终都会通过 ``efuse_table_gen.py`` 工具转换为 C 结构体。每个 C 结构体从 eFuse 字段的 ``field_name`` 中衍生出标识符，并用 ``_`` 替换所有的 ``.`` 符号。以 ``WR_DIS.RD_DIS`` 和 ``WR_DIS.FIELD_2.B1`` 为例，这两个 eFuse 字段用 C 语言分别表示为 ``ESP_EFUSE_WR_DIS_RD_DIS`` 和 ``ESP_EFUSE_WR_DIS_FIELD_2_B1``。

``efuse_table_gen.py`` 工具还会检查字段是否相互重叠并在字段范围内。如有违反，会生成以下错误：

.. code-block:: none

    Field at USER_DATA, EFUSE_BLK3, 0, 256 intersected with SERIAL_NUMBER, EFUSE_BLK3, 0, 32

要解决此问题，可使用 ``USER_DATA.SERIAL_NUMBER``，让 ``SERIAL_NUMBER`` 成为 ``USER_DATA`` 的子字段。

.. code-block:: none

    Field at FIELD, EFUSE_BLK3, 0, 50 out of range  FIELD.MAJOR_NUMBER, EFUSE_BLK3, 60, 32

要解决此问题，可将 ``FIELD.MAJOR_NUMBER`` 的 ``bit_start`` 从 ``60`` 改为 ``0``，使 ``MAJOR_NUMBER`` 与 ``FIELD`` 重叠。

``efuse_table_gen.py`` 工具
---------------------------

``efuse_table_gen.py`` 工具能够从 CSV 文件生成 C 源文件，其中包含 CSV 文件中定义的 eFuse 字段的对应 C 结构体（类型为 :cpp:type:`esp_efuse_desc_t`）。此外，该工具还会在生成 C 源文件前对 CSV 文件进行检查，以确保：

- eFuse 字段的名称唯一
- eFuse 字段使用的位不重叠

如前所述，eFuse 字段可用于存储系统参数或用户参数。由于系统参数 eFuse 字段是 ESP-IDF 和 {IDF_TARGET_NAME} 的内在要求，这些 eFuse 字段被定义在 **通用** CSV 文件中，即 ``esp_efuse_table.csv`` 中，是 ESP-IDF 的一部分。对于用户参数 eFuse 字段，用户应在 **自定义** CSV 文件（如 ``esp_efuse_custom_table.csv``）中进行定义。

要从 **通用** CSV 文件生成 C 源文件，运行 ``idf.py efuse-common-table`` 或以下命令：

.. code-block:: bash

    cd $IDF_PATH/components/efuse/
    ./efuse_table_gen.py --idf_target {IDF_TARGET_PATH_NAME} {IDF_TARGET_PATH_NAME}/esp_efuse_table.csv

然后，就会在路径 ``$IDF_PATH/components/efuse/{IDF_TARGET_PATH_NAME}`` 中生成以下 C 源文件/头文件：

*  ``esp_efuse_table.c`` 文件，包含系统参数 eFuse 字段的 C 结构体。
* ``esp_efuse_table.h`` 文件，位于 ``include`` 文件夹。应用程序可包含该头文件，以使用上述 C 结构体。

要使用 **自定义** CSV 文件生成 C 源文件，请运行 ``idf.py efuse-custom-table`` 或以下命令：

.. code-block:: bash

    cd $IDF_PATH/components/efuse/
    ./efuse_table_gen.py --idf_target {IDF_TARGET_PATH_NAME} {IDF_TARGET_PATH_NAME}/esp_efuse_table.csv PROJECT_PATH/main/esp_efuse_custom_table.csv

然后在 ``PROJECT_PATH/main`` 路径下生成 C 源/头文件：

* ``esp_efuse_custom_table.c`` 文件，包含用户参数 eFuse 字段的 C 结构体。
* ``esp_efuse_custom_table.h`` 文件，位于 ``include`` 文件夹。应用程序可包含该头文件，以使用上述 C 结构体。

要使用生成的字段，需添加以下头文件：

.. code-block:: c

    #include "esp_efuse.h"
    #include "esp_efuse_table.h" // 或 "esp_efuse_custom_table.h"


支持的编码方式
-----------------------

eFuse 支持各种编码方式，能够检测或纠正错误，保护 eFuse 数据不受损坏。

.. only:: esp32

    {IDF_TARGET_NAME} 支持以下 eFuse 编码方式：

    * ``非编码`` 方式（值为 0），即不采用编码方式。
    * ``3/4 编码`` 方式（值为 1）。
    * ``重复编码`` 方式（值为 2）。不完全受到 IDF 支持，不推荐使用。

    以上编码方案对每个 eFuse 块进行单独编码。此外，只有 EFUSE_BLK1、EFUSE_BLK2 和 EFUSE_BLK3 将被编码，这意味着 EUSE_BLK0 始终采用 ``非编码`` 方式。

    编码方案要求将 eFuse 块中的某些位用作开销。因此，采用编码方案后，每个 eFuse 块的 256 位中只有一部分可用于 eFuse 字段。

    * ``非编码``：256 位
    * ``3/4 编码``：192 位
    * ``重复编码``：128 位

    当使用一种编码方式时，可以写入的有效载荷长度是有限的。如需了解更多内容，请参考 *{IDF_TARGET_NAME} 技术参考手册* > *章节 20 eFuse 控制器 (eFuse)* [`PDF <{IDF_TARGET_TRM_CN_URL}#efuse>`__] > *章节 20.3.1.3 系统参数 coding_scheme*。

    通过以下步骤查看芯片的编码方式：

    * 运行 ``idf.py efuse-summary`` 命令。
    * 在烧录期间从 ``esptool`` 应用程序日志中查看。
    * 在应用程序中调用 :cpp:func:`esp_efuse_get_coding_scheme` 函数查看 EFUSE_BLK3 块的编码方式。

    CSV 文件中指定的 eFuse 字段必须始终符合芯片使用的 eFuse 编码方案。可以通过 :ref:`CONFIG_EFUSE_CODE_SCHEME_SELECTOR` 选择 CSV 文件使用的编码方案。生成源文件时，如果 CSV 文件中的内容不符合编码方案，则会显示错误信息。在这种情况下，必须调整错误行的 ``bit_start`` 和 ``bit_count``，以满足所选编码方案的限制。

    .. note::

        更改编码方式后，运行 ``efuse_common_table`` 和 ``efuse_custom_table`` 命令以查看采用新编码方式的 CSV 表格。

    如果程序是用 ``非编码`` 方式编译的，而芯片中使用的是 ``3/4 编码`` 方式，那么调用 eFuse API 时可能会出现 ``ESP_ERR_CODING`` 错误（字段超出块边界）。如果字段符合新的块边界，则 API 会正常运行。

    ``非编码`` 方式
    ^^^^^^^^^^^^^^^^^

    ``非编码`` 方式表示不使用编码方案，因此 eFuse 数据块的 256 位都可以使用。不过，这中方式无法防止 eFuse 位的数据损坏。

    ``3/4 编码`` 方式
    ^^^^^^^^^^^^^^^^^^^^^

    ``3/4 编码`` 方式会限制写入一个编码单元的位数。长度为 256 位的块被划分为 4 个编码单元，每个编码单元包含 6 字节的有用数据和 2 字节的服务数据。这 2 字节的服务数据中存储了前 6 个数据字节的校验和。

    由于要计算每个编码单元的校验和，写入过程必须根据不同的编码单元进行划分。在这种情况下，通过多个写入操作分别烧录各 eFuse 位（ ``非编码`` 方式的烧录方法）的常规做法将不再适用，必须一次性同时烧写该编码单元的 eFuse 字段数据和校验和。这就是所谓的批量写入模式。

    由于采用批量写入模式，一个编码单元只能写入一次，禁止在同一编码单元重复写入。这意味着，在运行时写入的编码单元中只能包含一个 eFuse 字段。但是，如果事先通过 CSV 文件指定了编码单元的 eFuse 字段，或通过 :cpp:func:`esp_efuse_write_block` 写入编码单元的 eFuse 字段，那么一个编码单元中仍可包含多个 eFuse 字段。


    ``重复编码`` 方式
    ^^^^^^^^^^^^^^^^^^^^^^^^
    ``重复编码`` 方式只是简单重复每个 eFuse 位，不会像 ``3/4 编码`` 方式那样受到批量写入模式的限制。不过，这样做会产生很大的开销，每个 eFuse 块中只有 128 个位可用。

.. only:: not esp32

    {IDF_TARGET_NAME} 不支持选择编码方式，会自动将以下编码方案应用于各 eFuse 块：

    * ``非编码`` 方式，应用于 EFUSE_BLK0。
    * ``RS 编码`` 方式。应用于 EFUSE_BLK1 - {IDF_TARGET_MAX_EFUSE_BLK}。

    ``非编码`` 方式
    ^^^^^^^^^^^^^^^^^^^^^^

    ``非编码`` 方式会自动应用于 EFUSE_BLK0。此方式不涉及任何编码，只是在硬件中维护 EFUSE_BLK0 的四个备份，因此，每个位实际存储四次，EFUSE_BLK0 也可以多次写入。

    该方案由硬件自动应用，软件中不可见。

    ``RS 编码`` 方式
    ^^^^^^^^^^^^^^^^^^^^^^

    * ``RS 编码`` 方式，即 Reed-Solomon 编码方式，会自动应用于 EFUSE_BLK1 至 {IDF_TARGET_MAX_EFUSE_BLK}。该编码方式支持至多 6 个字节的自动纠错。

    软件使用 ``RS(44, 32)`` 对 32 字节的 EFUSE_BLKx 进行编码，生成一个 12 字节的校验码，然后将 EFUSE_BLKx 和校验码同时烧录到 eFuse 中。

    在回读 eFuse 块时，eFuse 控制器会对 ``RS 编码`` 进行自动解码和纠错。因为 ``RS`` 校验码是根据整个 256 位 eFuse 块生成的，所以每个块只能写入一次，且必须采用批量写入模式。

批量写入模式
^^^^^^^^^^^^

如需在运行时写入 eFuse 字段，可能要采用批量写入模式，具体取决于 eFuse 块使用的编码方案。批量写入模式的使用步骤如下：

#. 调用 :cpp:func:`esp_efuse_batch_write_begin` 启用批量写入模式。
#. 使用各 ``esp_efuse_write_...`` 函数，按照常规方法写入 eFuse 字段。
#. 完成所有写入后，调用 :cpp:func:`esp_efuse_batch_write_commit` 将准备好的数据烧录到 eFuse 块中。

.. warning::

    如果 eFuse 块中已经存在通过 ``{IDF_TARGET_CODING_SCHEMES}`` 编码方案预先写入的数据，则无法在不破坏先前数据校验和/校验符号的情况下，写入额外的内容（即使需要写入的位为空）。

    先前的校验和/校验符号会被新的校验和/校验符号覆盖，并完全销毁。（但不会损坏有效负载 eFuse）。

    如发现在 CUSTOM_MAC、SPI_PAD_CONFIG_HD、SPI_PAD_CONFIG_CS 等块中存在预写入数据，请联系乐鑫获取所需的预烧录 eFuse。

    仅供测试（不推荐）：可以忽略或抑制违反编码方式数据的错误，从而在 eFuse 块中烧录必要的位。

.. _efuse_API:

eFuse API
---------

可以通过指向描述结构的指针来访问 eFuse 字段。API 函数可以实现一些基本操作：

* :cpp:func:`esp_efuse_read_field_blob` - 返回读取的 eFuse 位的数组。
* :cpp:func:`esp_efuse_read_field_cnt` - 返回烧写为 “1” 的位的数量。
* :cpp:func:`esp_efuse_write_field_blob` - 写入一个数组。
* :cpp:func:`esp_efuse_write_field_cnt` - 将所需数量的位写为 “1”。
* :cpp:func:`esp_efuse_get_field_size` - 返回字段的位数。
* :cpp:func:`esp_efuse_read_reg` - 返回 eFuse 寄存器的值。
* :cpp:func:`esp_efuse_write_reg` - 将值写入 eFuse 寄存器。
* :cpp:func:`esp_efuse_get_coding_scheme` - 返回块的 eFuse 编码方式。
* :cpp:func:`esp_efuse_read_block` - 从指定偏移位置开始读取指定大小的 eFuse 块中的密钥。
* :cpp:func:`esp_efuse_write_block` - 从指定偏移位置开始将密钥写入指定大小的 eFuse 块中。
* :cpp:func:`esp_efuse_batch_write_begin` - 设置字段写入的批处理模式。
* :cpp:func:`esp_efuse_batch_write_commit` - 写入所有为批处理写入模式准备的数据，并重置批处理写入模式。
* :cpp:func:`esp_efuse_batch_write_cancel` - 重置批处理写入模式和准备的数据。
* :cpp:func:`esp_efuse_get_key_dis_read` - 返回密钥块的读保护状态。
* :cpp:func:`esp_efuse_set_key_dis_read` - 设置密钥块的读保护状态。
* :cpp:func:`esp_efuse_get_key_dis_write` - 返回密钥块的写保护状态。
* :cpp:func:`esp_efuse_set_key_dis_write` - 设置密钥块的写保护状态。
* :cpp:func:`esp_efuse_get_key_purpose` - 返回 eFuse 密钥块当前设置的用途。
* :cpp:func:`esp_efuse_write_key` - 将一块密钥数据烧写到一个 eFuse 块。
* :cpp:func:`esp_efuse_write_keys` - 将密钥烧写到未使用的 eFuse 块。
* :cpp:func:`esp_efuse_find_purpose` - 查找设置为特定用途的密钥块。
* :cpp:func:`esp_efuse_get_keypurpose_dis_write` - 返回 eFuse 密钥块的密钥用途字段的写保护状态（对于 esp32 始终为 true）。
* :cpp:func:`esp_efuse_key_block_unused` - 如果密钥块未使用，则返回 true，否则返回 false。
* :cpp:func:`esp_efuse_destroy_block` - 销毁此 eFuse 块中的数据。该函数有两个作用：(1) 如果未开启写保护，则将不为 1 的位都烧写为 1；(2) 如果未开启读保护，则开启读保护。

经常使用的字段有专门的函数可供使用，例如 :cpp:func:`esp_efuse_get_pkg_ver`。

.. only:: SOC_EFUSE_KEY_PURPOSE_FIELD or SOC_SUPPORT_SECURE_BOOT_REVOKE_KEY

    eFuse 密钥 API
    ------------------

    .. only:: SOC_EFUSE_KEY_PURPOSE_FIELD

        EFUSE_BLK_KEY0 - EFUSE_BLK_KEY5 可以保存 6 个长度为 256 位的密钥。每个密钥都有一个 ``ESP_EFUSE_KEY_PURPOSE_x`` 字段说明密钥用途。用途字段描述见 :cpp:type:`esp_efuse_purpose_t`。

        类似 ``ESP_EFUSE_KEY_PURPOSE_XTS_AES_...`` 的用途用于 flash 加密。

        类似 ``ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_DIGEST...`` 的用途用于安全启动。

        一些 eFuse API 可用于处理密钥状态：

        * :cpp:func:`esp_efuse_get_purpose_field` - 返回一个指向 eFuse 密钥块的密钥用途的指针。
        * :cpp:func:`esp_efuse_get_key` - 返回一个指向密钥块的指针。
        * :cpp:func:`esp_efuse_set_key_purpose` - 为一个 eFuse 密钥块设置密钥用途。
        * :cpp:func:`esp_efuse_set_keypurpose_dis_write` - 为 eFuse 密钥块的密钥用途字段设置写保护。
        * :cpp:func:`esp_efuse_find_unused_key_block` - 搜索未使用的密钥块并返回找到的第一个结果。
        * :cpp:func:`esp_efuse_count_unused_key_blocks` - 返回 EFUSE_BLK_KEY0..EFUSE_BLK_KEY_MAX 范围中未使用的 eFuse 密钥块数量。

    .. only:: SOC_SUPPORT_SECURE_BOOT_REVOKE_KEY

        * :cpp:func:`esp_efuse_get_digest_revoke` - 返回安全启动公钥摘要撤销位的状态。
        * :cpp:func:`esp_efuse_set_digest_revoke` - 设置安全启动公钥摘要撤销位。
        * :cpp:func:`esp_efuse_get_write_protect_of_digest_revoke` - 返回安全启动公钥摘要撤销位的写保护。
        * :cpp:func:`esp_efuse_set_write_protect_of_digest_revoke` - 设置安全启动公钥摘要撤销位的写保护。


如何添加新字段
----------------------

1. 为新字段查找空闲位。运行 ``idf.py show-efuse-table`` 查看 ``esp_efuse_table.csv`` 文件，或运行以下命令：

.. include:: inc/show-efuse-table_{IDF_TARGET_NAME}.rst

不包含在方括号中的位是空闲的（一些位是乐鑫的保留位）。已检查所有字段的位重叠情况。

要在现有字段中添加子字段，请参考 :ref:`structured-efuse-fields`。例如，可使用 ``.`` 操作符将字段 ``SERIAL_NUMBER``、``MODEL_NUMBER`` 和 ``HARDWARE_REV`` 添加到现有的 ``USER_DATA`` 字段中，如下所示：

.. code-block:: none

    USER_DATA.SERIAL_NUMBER,                  EFUSE_BLK3,    0,  32,
    USER_DATA.MODEL_NUMBER,                   EFUSE_BLK3,    32, 10,
    USER_DATA.HARDWARE_REV,                   EFUSE_BLK3,    42, 10,

通常按照如下步骤添加新的 eFuse 字段：

#. 在 CSV 文件中为每个 eFuse 字段添加一行记录。
#. 运行 ``show_efuse_table`` 命令检查 eFuse 表。
#. 如要生成源文件，运行 ``efuse_common_table`` 或 ``efuse_custom_table`` 命令。

如果遇到 ``intersects with`` 或 ``out of range`` 等错误，请参阅 :ref:`structured-efuse-fields` 中的解决办法。

位序
----

eFuse 位序采取小字节序（参见下方示例），这说明 eFuse 位按照从 LSB 到 MSB 的顺序进行读写：

.. code-block:: none

    $ idf.py efuse-dump

    USER_DATA      (BLOCK3          ) [3 ] read_regs: 03020100 07060504 0B0A0908 0F0E0D0C 13121111 17161514 1B1A1918 1F1E1D1C
    BLOCK4         (BLOCK4          ) [4 ] read_regs: 03020100 07060504 0B0A0908 0F0E0D0C 13121111 17161514 1B1A1918 1F1E1D1C

    where is the register representation:

    EFUSE_RD_USR_DATA0_REG = 0x03020100
    EFUSE_RD_USR_DATA1_REG = 0x07060504
    EFUSE_RD_USR_DATA2_REG = 0x0B0A0908
    EFUSE_RD_USR_DATA3_REG = 0x0F0E0D0C
    EFUSE_RD_USR_DATA4_REG = 0x13121111
    EFUSE_RD_USR_DATA5_REG = 0x17161514
    EFUSE_RD_USR_DATA6_REG = 0x1B1A1918
    EFUSE_RD_USR_DATA7_REG = 0x1F1E1D1C

    where is the byte representation:

    byte[0] = 0x00, byte[1] = 0x01, ... byte[3] = 0x03, byte[4] = 0x04, ..., byte[31] = 0x1F

例如，CSV 文件描述了 ``USER_DATA`` 字段，该字段占用 256 位，即一个完整的块。

.. code-block:: none

    USER_DATA,          EFUSE_BLK3,    0,  256,     User data
    USER_DATA.FIELD1,   EFUSE_BLK3,    16,  16,     Field1

    ID,                 EFUSE_BLK4,    8,  3,      ID bit[0..2]
    ,                   EFUSE_BLK4,    16, 2,      ID bit[3..4]
    ,                   EFUSE_BLK4,    32, 3,      ID bit[5..7]

因此，读取如上 eFuse ``USER_DATA`` 块会得到以下结果：

.. code-block:: c

    uint8_t buf[32] = { 0 };
    esp_efuse_read_field_blob(ESP_EFUSE_USER_DATA, &buf, sizeof(buf) * 8);
    // buf[0] = 0x00, buf[1] = 0x01, ... buf[31] = 0x1F

    uint32_t field1 = 0;
    size_t field1_size = ESP_EFUSE_USER_DATA[0]->bit_count; // 可以用于这种情况，因为它只包含一个条目
    esp_efuse_read_field_blob(ESP_EFUSE_USER_DATA, &field1, field1_size);
    // field1 = 0x0302

    uint32_t field1_1 = 0;
    esp_efuse_read_field_blob(ESP_EFUSE_USER_DATA, &field1_1, 2); // 只读取前两位
    // field1 = 0x0002

    uint8_t id = 0;
    size_t id_size = esp_efuse_get_field_size(ESP_EFUSE_ID); // 返回 6
    // size_t id_size = ESP_EFUSE_USER_DATA[0]->bit_count; // 不能用于这种情况，因为其中包含 3 个条目，会返回 3 而不是 6
    esp_efuse_read_field_blob(ESP_EFUSE_ID, &id, id_size);
    // id = 0x91
    // b'100 10  001
    //   [3] [2] [3]

    uint8_t id_1 = 0;
    esp_efuse_read_field_blob(ESP_EFUSE_ID, &id_1, 3);
    // id = 0x01
    // b'001


在构建阶段获取 eFuse 状态
-------------------------

要在项目的构建阶段获取 eFuse 状态，可以使用以下两个 CMake 函数：

* ``espefuse_get_json_summary()`` - 调用 ``espefuse.py summary --format json`` 命令并返回一个 JSON 字符串（该字符串不存储在文件中）。
* ``espefuse_get_efuse()`` - 在此 JSON 字符串中找到给定的 eFuse 名称并返回其属性。

该 JSON 字符串具有以下属性：

.. code-block:: json

    {
        "MAC": {
            "bit_len": 48,
            "block": 0,
            "category": "identity",
            "description": "Factory MAC Address",
            "efuse_type": "bytes:6",
            "name": "MAC",
            "pos": 0,
            "readable": true,
            "value": "94:b9:7e:5a:6e:58 (CRC 0xe2 OK)",
            "word": 1,
            "writeable": true
        },
    }

可以通过项目顶层目录下的 ``CMakeLists.txt`` (:example_file:`system/efuse/CMakeLists.txt`) 来使用这些函数：

.. code-block:: cmake

    # ...
    project(hello_world)

    espefuse_get_json_summary(efuse_json)
    espefuse_get_efuse(ret_data ${efuse_json} "MAC" "value")
    message("MAC:" ${ret_data})

``value`` 属性的格式与 ``espefuse.py summary`` 或 ``idf.py efuse-summary`` 中显示的格式相同。

.. code-block:: none

    MAC:94:b9:7e:5a:6e:58 (CRC 0xe2 OK)

在示例测试 :example_file:`system/efuse/CMakeLists.txt` 中，添加了一个自定义目标 ``efuse-filter``。这样，不仅在项目构建阶段，而在任何时候都可以运行 ``idf.py efuse-filter`` 命令读取所需的 eFuse（在 ``efuse_names`` 列表中指定）。

调试 eFuse & 单元测试
------------------------

.. _virtual-efuses:

虚拟 eFuse
^^^^^^^^^^^^^^

Kconfig 选项 :ref:`CONFIG_EFUSE_VIRTUAL` 在 eFuse 管理器中虚拟了 eFuse 值，因此写入操作是仿真操作，不会永久更改 eFuse 值。这对于应用程序调试和单元测试很有用处。

在启动时，eFuses 被复制到 RAM 中。此时，所有的 eFuse 操作（读和写）都是通过 RAM 执行，而不是通过实际的 eFuse 寄存器执行的。

除了 :ref:`CONFIG_EFUSE_VIRTUAL` 选项外，还有 :ref:`CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH` 选项，该选项可将 eFuse 保留在 flash 内存中。要使用此模式，partition_table 在  ``partition.csv`` 中包含名为 ``efuse`` 的分区：

.. code-block:: none

    efuse_em, data, efuse,   ,   0x2000,

在启动阶段，eFuse 会从 flash 中复制到 RAM 中，在 flash 为空的情况下，则从实际的 eFuse 复制到 RAM 中，然后更新 flash。此选项能够在重启后仍然保留 eFuse，用于测试安全启动和 flash 加密功能。

flash 加密测试
""""""""""""""

flash 加密是一项硬件功能，需要物理烧录 eFuse ``key`` 和 ``FLASH_CRYPT_CNT``。如果 flash 加密实际未启用，那么启用 :ref:`CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH` 选项只是提供了测试的可能性，而不会加密 flash 中的任何内容，即使日志中显示了加密操作。

为此，可使用 :cpp:func:`bootloader_flash_write` 函数。但是，如果运行应用程序时芯片已启用 flash 加密，或者以 :ref:`CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH` 选项创建了启动加载程序，则 flash 加密/解密操作会正常进行。这意味着数据写入加密 flash 分区时被加密，从加密分区读取时被解密。

``espefuse.py``
^^^^^^^^^^^^^^^

esptool 中包含一个用于读取/写入 {IDF_TARGET_NAME} eFuse 位的有用工具： `espefuse.py <https://docs.espressif.com/projects/esptool/en/latest/{IDF_TARGET_PATH_NAME}/espefuse/index.html>`_。

``idf.py`` 命令也可以直接提供上述工具的部分功能。例如，运行 ``idf.py efuse-summary`` 命令，效果等同于 ``espefuse.py summary``。

.. include:: inc/espefuse_summary_{IDF_TARGET_NAME}.rst

获取所有 eFuse 寄存器的转储数据。

.. include:: inc/espefuse_summary_{IDF_TARGET_NAME}_dump.rst

应用示例
-----------------

- :example:`system/efuse` 演示了如何在 {IDF_TARGET_NAME} 上使用 eFuse API，展示了从通用和自定义 eFuse 表中读取和写入字段的操作，并解释了虚拟 eFuse 在调试中的用途。

API 参考
----------------

.. include-build-file:: inc/esp_efuse_chip.inc
.. include-build-file:: inc/esp_efuse.inc
