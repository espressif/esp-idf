NVS 分区生成程序
================

:link_to_translation:`en:[English]`

介绍
----

NVS 分区生成程序 (:component_file:`nvs_flash/nvs_partition_generator/nvs_partition_gen.py`) 根据 CSV 文件中的键值对生成二进制文件。该二进制文件与 :doc:`nvs_flash` 中定义的 NVS 结构兼容。

NVS 分区生成程序适合用于生成二进制数据 (blob)，其中包括设备生产时可从外部烧录的 ODM/OEM 数据。这也使得生产制造商在使用同一个应用固件的基础上，通过自定义参数，如序列号，为每个设备生成不同配置的二进制 NVS 分区。

准备工作
--------

在加密模式下使用该程序，需安装下列软件包：
    - ``cryptography``

根目录下的 `requirements.txt` 包含必需 python 包，请预先安装。

.. _nvs-csv-file-format:

CSV 文件格式
~~~~~~~~~~~~

CSV 文件每行需包含四个参数，以逗号隔开。具体参数描述见下表：

.. list-table::
    :widths: 10 20 35 35
    :header-rows: 1

    * - 序号
      - 参数
      - 描述
      - 说明
    * - 1
      - Key
      - 主键，应用程序可通过查询此键来获取数据。
      -
    * - 2
      - Type
      - 支持 ``file``、``data`` 和 ``namespace``。
      -
    * - 3
      - Encoding
      - 支持 ``u8``、``i8``、``u16``、``i16``、``u32``、``i32``、``u64``、``i64``、``string``、``hex2bin``、``base64`` 和 ``binary``。决定二进制 ``bin`` 文件中 value 被编码成的类型。``string`` 和 ``binary`` 编码的区别在于，``string`` 数据以 NULL 字符结尾，``binary`` 数据则不是。
      - ``file`` 类型当前仅支持 ``hex2bin``、``base64``、 ``string`` 和 ``binary`` 编码。
    * - 4
      - Value
      - Data value
      - ``namespace`` 字段的 ``encoding`` 和 ``value`` 应为空。 ``namespace`` 的 ``encoding`` 和 ``value`` 为固定值，不可设置。这些单元格中的所有值都会被忽视。

.. note:: CSV 文件的第一行应始终为列标题，不可设置。

此类 CSV 文件的 Dump 示例如下::

    key,type,encoding,value     <-- 列标题
    namespace_name,namespace,,  <-- 第一个条目为 "namespace"
    key1,data,u8,1
    key2,file,string,/path/to/file

.. note::

    请确保：
        - 逗号 ',' 前后无空格；
        - CSV 文件每行末尾无空格。


NVS 条目和命名空间 (namespace) 的关联
-------------------------------------

如 CSV 文件中出现命名空间条目，后续条目均会被视为该命名空间的一部分，直至找到下一个命名空间条目。找到新命名空间条目后，后续所有条目都会被视为新命名空间的一部分。

.. note:: CSV 文件中第一个条目应始终为 ``namespace``。


支持多页 blob
-------------

默认情况下，二进制 blob 可跨多页，格式参考 :ref:`structure_of_entry` 章节。如需使用旧版格式，可在程序中禁用该功能。


支持加解密
----------

NVS 分区生成程序还可使用 XTS-AES 加密生成二进制加密文件或对此类文件进行解密。更多信息详见 :ref:`nvs_encryption`。


运行程序
--------

**使用方法**::

    python nvs_partition_gen.py [-h] {generate,generate-key,encrypt,decrypt} ...

**可选参数**：

.. list-table::
    :widths: 20 40 40
    :header-rows: 1

    * - 序号
      - 参数
      - 描述
    * - 1
      - ``-h`` / ``--help``
      - 显示帮助信息并退出 

**命令**:

    运行 ``nvs_partition_gen.py {command} -h`` 查看更多帮助信息

.. list-table::
    :widths: 20 40 40
    :header-rows: 1

    * - 序号
      - 参数
      - 描述
    * - 1
      - ``generate`` 
      - 生成 NVS 分区
    * - 2
      - ``generate-key``
      - 生成加密密钥 
    * - 3
      - ``encrypt``  
      - 加密 NVS 分区
    * - 4
      - ``decrypt`` 
      - 解密 NVS 分区 

生成 NVS 分区（默认模式）
~~~~~~~~~~~~~~~~~~~~~~~~~

**使用方法**::

    python nvs_partition_gen.py generate [-h] [--version {1,2}] [--outdir OUTDIR] input output size

**位置参数**：

.. list-table::
    :widths: 30 70
    :header-rows: 1

    * - 参数
      - 描述
    * - ``input``
      - 待解析的 CSV 文件路径
    * - ``output``
      - NVS 二进制文件的输出路径
    * - ``size`` 
      - NVS 分区大小（以字节为单位，且为 4096 的整数倍）

**可选参数**：

+--------------------------+------------------------------------------------+
|       参数               |                      描述                      |
+--------------------------+------------------------------------------------+
| ``-h`` / ``--help``      | 显示帮助信息并退出                             |
+--------------------------+------------------------------------------------+
| ``--version {1,2}``      | - 设置多页 blob 版本，默认为版本 2。           |
|                          |                                                |
|                          | - 版本 1：禁用多页 blob；                      |
|                          |                                                |
|                          | - 版本 2：启用多页 blob。                      |
+--------------------------+------------------------------------------------+
| ``--outdir OUTDIR``      | 输出目录，用于存储创建的文件。（默认当前目录） |
+--------------------------+------------------------------------------------+

运行如下命令创建 NVS 分区，该程序同时会提供 CSV 示例文件::

    python nvs_partition_gen.py generate sample_singlepage_blob.csv sample.bin 0x3000


生成加密密钥分区
~~~~~~~~~~~~~~~~

.. only:: SOC_HMAC_SUPPORTED

  **使用方法**::

      python nvs_partition_gen.py generate-key [-h] [--key_protect_hmac] [--kp_hmac_keygen]
                                                    [--kp_hmac_keyfile KP_HMAC_KEYFILE] [--kp_hmac_inputkey KP_HMAC_INPUTKEY]
                                                    [--keyfile KEYFILE] [--outdir OUTDIR]

.. only:: not SOC_HMAC_SUPPORTED

    **使用方法**::

        python nvs_partition_gen.py generate-key [-h] [--keyfile KEYFILE] [--outdir OUTDIR]

**可选参数**:

.. list-table::
    :widths: 30 70
    :header-rows: 1

    * - 参数
      - 描述
    * - ``-h`` / ``--help``
      - 显示帮助信息并退出
    * - ``--keyfile KEYFILE``
      - 加密密钥分区文件的输出路径
    * - ``--outdir OUTDIR``
      - 输出目录，用于存储创建的文件（默认当前目录）

.. only:: SOC_HMAC_SUPPORTED

    **可选参数（仅适用于 HMAC 方案）**:

    .. list-table::
        :widths: 30 70
        :header-rows: 1

        * - 参数
          - 描述
        * - ``--key_protect_hmac`` 
          - 设置后使用基于 HMAC 的 NVS 加密密钥保护方案，否则使用基于 flash 加密的默认方案 
        * - ``--kp_hmac_keygen``
          - 为基于 HMAC 的加密方案生成 HMAC 密钥 
        * - ``--kp_hmac_keyfile KP_HMAC_KEYFILE`` 
          - HMAC 密钥文件的输出路径
        * - ``--kp_hmac_inputkey KP_HMAC_INPUTKEY``
          - 包含 HMAC 密钥的文件，用于生成 NVS 加密密钥

运行以下命令仅生成加密密钥分区::

    python nvs_partition_gen.py generate-key

.. only:: SOC_HMAC_SUPPORTED

    运行以下命令，为基于 HMAC 的方案生成加密密钥：

    - 生成 HMAC 密钥和 NVS 加密密钥::

        python nvs_partition_gen.py generate-key --key_protect_hmac --kp_hmac_keygen

    .. note:: 上述命令生成 ``<outdir>/keys/keys-<timestamp>.bin`` 格式的加密密钥和 ``<outdir>/keys/hmac-keys-<timestamp>.bin`` 格式的 HMAC 密钥。

    - 基于 HMAC 密钥生成 NVS 加密密钥::

        python nvs_partition_gen.py generate-key --key_protect_hmac --kp_hmac_inputkey testdata/sample_hmac_key.bin

    .. note:: 可将自定义文件名作为参数提供给 HMAC 密钥和加密密钥。

.. _generate-encrypted-nvs-partition:

生成 NVS 加密分区
~~~~~~~~~~~~~~~~~

.. only:: SOC_HMAC_SUPPORTED

    **使用方法**::

            python nvs_partition_gen.py encrypt [-h] [--version {1,2}] [--keygen]
                                                [--keyfile KEYFILE] [--inputkey INPUTKEY] [--outdir OUTDIR]
                                                [--key_protect_hmac] [--kp_hmac_keygen]
                                                [--kp_hmac_keyfile KP_HMAC_KEYFILE] [--kp_hmac_inputkey KP_HMAC_INPUTKEY]
                                                input output size

.. only:: not SOC_HMAC_SUPPORTED

    **使用方法**::

            python nvs_partition_gen.py encrypt [-h] [--version {1,2}] [--keygen]
                                                [--keyfile KEYFILE] [--inputkey INPUTKEY] [--outdir OUTDIR]
                                                input output size


**位置参数**：

.. list-table::
    :widths: 30 70
    :header-rows: 1

    * - 参数
      - 描述
    * - ``input``
      - 待解析的 CSV 文件路径
    * - ``output``
      - NVS 二进制文件的输出路径
    * - ``size`` 
      - NVS 分区大小（以字节为单位，且为 4096 的整数倍）

**可选参数**：

+--------------------------+------------------------------------------------+
|       参数               |                      描述                      |
+--------------------------+------------------------------------------------+
| ``-h`` / ``--help``      | 显示帮助信息并退出                             |
+--------------------------+------------------------------------------------+
| ``--version {1,2}``      | - 设置多页 blob 版本，默认为版本 2。           |
|                          |                                                |
|                          | - 版本 1：禁用多页 blob；                      |
|                          |                                                |
|                          | - 版本 2：启用多页 blob。                      |
+--------------------------+------------------------------------------------+
| ``--keygen``             | 生成 NVS 分区加密密钥                          |
+--------------------------+------------------------------------------------+
| ``--keyfile KEYFILE``    | 密钥文件的输出路径                             |
+--------------------------+------------------------------------------------+
| ``--inputkey INPUTKEY``  | 内含 NVS 分区加密密钥的文件                    |
+--------------------------+------------------------------------------------+
| ``--outdir OUTDIR``      | 输出目录，用于存储创建的文件。（默认当前目录） |
+--------------------------+------------------------------------------------+

.. only:: SOC_HMAC_SUPPORTED

    **可选参数（仅适用于 HMAC 方案）**:

    .. list-table::
        :widths: 30 70
        :header-rows: 1

        * - 参数
          - 描述
        * - ``--key_protect_hmac`` 
          - 设置后使用基于 HMAC 的 NVS 加密密钥保护方案，否则使用基于 flash 加密的默认方案 
        * - ``--kp_hmac_keygen``
          - 为基于 HMAC 的加密方案生成 HMAC 密钥 
        * - ``--kp_hmac_keyfile KP_HMAC_KEYFILE`` 
          - HMAC 密钥文件的输出路径
        * - ``--kp_hmac_inputkey KP_HMAC_INPUTKEY``
          - 包含 HMAC 密钥的文件，用于生成 NVS 加密密钥

运行以下命令加密 NVS 分区，该程序同时会提供一个 CSV 示例文件。

- 通过 NVS 分区生成程序生成加密密钥来加密::

    python nvs_partition_gen.py encrypt sample_singlepage_blob.csv sample_encr.bin 0x3000 --keygen

  .. note:: 创建的加密密钥格式为 ``<outdir>/keys/keys-<timestamp>.bin``。

.. only:: SOC_HMAC_SUPPORTED

  - 要使用基于 HMAC 的方案生成加密分区，可将上述命令与附加参数搭配使用。

    - 通过 NVS 分区生成程序生成加密密钥和 HMAC 密钥，从而进行加密::

        python nvs_partition_gen.py encrypt sample_singlepage_blob.csv sample_encr.bin 0x3000 --keygen --key_protect_hmac --kp_hmac_keygen

    .. note:: 上述命令生成 ``<outdir>/keys/keys-<timestamp>.bin`` 格式的加密密钥和 ``<outdir>/keys/hmac-keys-<timestamp>.bin`` 格式的 HMAC 密钥。

    - 通过 NVS 分区生成程序使用用户提供的 HMAC 密钥生成加密密钥，从而进行加密::

        python nvs_partition_gen.py encrypt sample_singlepage_blob.csv sample_encr.bin 0x3000 --keygen --key_protect_hmac --kp_hmac_inputkey testdata/sample_hmac_key.bin

  .. note:: 可将自定义文件名作为参数提供给 HMAC 密钥和加密密钥。

- 通过 NVS 分区生成程序生成加密密钥，并将密钥存储于自定义的文件中::

    python nvs_partition_gen.py encrypt sample_singlepage_blob.csv sample_encr.bin 0x3000 --keygen --keyfile sample_keys.bin

.. note:: 

    - 创建的加密密钥格式为 ``<outdir>/keys/sample_keys.bin``。

    - 加密密钥存储于新建文件的 ``keys/`` 目录下，与 NVS 密钥分区结构兼容。更多信息请参考 :ref:`nvs_encr_key_partition`。

- 将加密密钥用作二进制输入文件来进行加密::

    python nvs_partition_gen.py encrypt sample_singlepage_blob.csv sample_encr.bin 0x3000 --inputkey sample_keys.bin

解密 NVS 分区
~~~~~~~~~~~~~

**使用方法**::

    python nvs_partition_gen.py decrypt [-h] [--outdir OUTDIR] input key output

**位置参数**：

.. list-table::
    :widths: 30 70
    :header-rows: 1

    * - 参数
      - 描述
    * - ``input``
      - 待解析的 NVS 加密分区文件路径
    * - ``key``
      - 含有解密密钥的文件路径
    * - ``output``
      - 已解密的二进制文件输出路径

**可选参数**：

.. list-table::
    :widths: 30 70
    :header-rows: 1

    * - 参数
      - 描述
    * - ``-h`` / ``--help``
      - 显示帮助信息并退出
    * - ``--outdir OUTDIR``
      - 输出目录，用于存储创建的文件（默认当前目录）

运行以下命令解密已加密的 NVS 分区::

    python nvs_partition_gen.py decrypt sample_encr.bin sample_keys.bin sample_decr.bin

可以在命令中提供版本参数，选择格式版本号：
    - 版本 1：禁用多页 blob
    - 版本 2：启用多页 blob


版本 1：禁用多页 blob
~~~~~~~~~~~~~~~~~~~~~

如需禁用多页 blob，请按照如下命令将版本参数设置为 1，以此格式运行分区生成程序。该程序同时会提供一个 CSV 示例文件::

   python nvs_partition_gen.py generate sample_singlepage_blob.csv sample.bin 0x3000 --version 1


版本 2：启用多页 blob
~~~~~~~~~~~~~~~~~~~~~

如需启用多页 blob，请按照如下命令将版本参数设置为 2，以此格式运行分区生成程序。该程序同时会提供一个 CSV 示例文件::

   python nvs_partition_gen.py generate sample_multipage_blob.csv sample.bin 0x4000 --version 2

.. note::

    - NVS 分区最小为 0x3000 字节。

    - 将二进制文件烧录至设备时，请确保与应用的 sdkconfig 设置一致。


说明
----

-  分区生成程序不会对重复键进行检查，而将数据同时写入这两个重复键中。请注意不要使用同名的键；
-  新页面创建后，前一页的空白处不会再写入数据。CSV 文件中的字段须按次序排列以优化内存；
-  暂不支持 64 位数据类型。
