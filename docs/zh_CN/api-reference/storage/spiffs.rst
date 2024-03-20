SPIFFS 文件系统
=================

:link_to_translation:`en:[English]`

概述
--------

SPIFFS 是一个用于 SPI NOR flash 设备的嵌入式文件系统，支持磨损均衡、文件系统一致性检查等功能。

说明
-----

 - 目前，SPIFFS 尚不支持目录，但可以生成扁平结构。如果 SPIFFS 挂载在 ``/spiffs`` 下，在 ``/spiffs/tmp/myfile.txt`` 路径下创建一个文件则会在 SPIFFS 中生成一个名为 ``/tmp/myfile.txt`` 的文件，而不是在 ``/spiffs/tmp`` 下生成名为 ``myfile.txt`` 的文件；
 - SPIFFS 并非实时栈，每次写操作耗时不等；
 - 目前，SPIFFS 尚不支持检测或处理已损坏的块。
 - SPIFFS 只能稳定地使用约 75% 的指定分区容量。
 - 当文件系统空间不足时，垃圾收集器会尝试多次扫描文件系统来寻找可用空间。根据所需空间的不同，写操作会被调用多次，每次函数调用将花费几秒。同一操作可能会花费不同时长的问题缘于 SPIFFS 的设计，且已在官方的 `SPIFFS github 仓库 <https://github.com/pellepl/spiffs/issues/>`_ 或是 `<https://github.com/espressif/esp-idf/issues/1737>`_ 中被多次报告。这个问题可以通过 `SPIFFS 配置 <https://github.com/pellepl/spiffs/wiki/Configure-spiffs>`_ 部分缓解。
 - 当垃圾收集器尝试多次（默认为 10 次）扫描整个文件系统以回收空间时，在每次扫描期间，如果有可用的数据块，则垃圾收集器会释放一个数据块。因此，如果为垃圾收集器设置的最大运行次数为 n（可通过 SPIFFS_GC_MAX_RUNS 选项配置，该选项位于 `SPIFFS 配置 <https://github.com/pellepl/spiffs/wiki/Configure-spiffs>`_ 中），那么 n 倍数据块大小的空间将可用于写入数据。如果尝试写入超过 n 倍数据块大小的数据，写入操作可能会失败并返回错误。
 - 如果 {IDF_TARGET_NAME} 在文件系统操作期间断电，可能会导致 SPIFFS 损坏。但是仍可通过 ``esp_spiffs_check`` 函数恢复文件系统。详情请参阅官方 SPIFFS `FAQ <https://github.com/pellepl/spiffs/wiki/FAQ>`_。

工具
-----

``spiffsgen.py``
^^^^^^^^^^^^^^^^

:component_file:`spiffsgen.py<spiffs/spiffsgen.py>` （只写）是 SPIFFS 的一种 Python 实现，可用于从主机文件夹内容生成文件系统镜像。打开终端并运行以下命令即可使用 ``spiffsgen.py``::

    python spiffsgen.py <image_size> <base_dir> <output_file>

参数（必选）说明如下：

- **image_size**：分区大小，用于烧录生成的 SPIFFS 镜像；
- **base_dir**：创建 SPIFFS 镜像的目录；
- **output_file**：SPIFFS 镜像输出文件。

其他参数（可选）也参与控制镜像的生成，用户可以运行以下帮助命令，查看这些参数的具体信息::

    python spiffsgen.py --help

上述可选参数对应 SPIFFS 构建配置选项。若想顺利生成可用的镜像，请确保使用的参数或配置与构建 SPIFFS 时所用的参数或配置相同。运行帮助命令将显示参数所对应的 SPIFFS 构建配置。如未指定参数，将使用帮助信息中的默认值。

镜像生成后，用户可以使用 ``esptool.py`` 或 ``parttool.py`` 烧录镜像。

用户可以在命令行或脚本中手动单独调用 ``spiffsgen.py``，也可以直接从构建系统调用 ``spiffs_create_partition_image`` 来使用 ``spiffsgen.py``::

    spiffs_create_partition_image(<partition> <base_dir> [FLASH_IN_PROJECT] [DEPENDS dep dep dep...])

在构建系统中使用 ``spiffsgen.py`` 更为方便，构建配置会自动传递给 ``spiffsgen.py`` 工具，确保生成的镜像可用于构建。比如，单独调用 ``spiffsgen.py`` 时需要用到 **image_size** 参数，但在构建系统中调用 ``spiffs_create_partition_image`` 时，仅需要 **partition** 参数，镜像大小将直接从工程分区表中获取。

使用 ``spiffs_create_partition_image``，必须从组件 ``CMakeLists.txt`` 文件调用。

用户也可以指定 ``FLASH_IN_PROJECT``，然后使用 ``idf.py flash`` 将镜像与应用程序二进制文件、分区表等一起自动烧录至设备，例如::

    spiffs_create_partition_image(my_spiffs_partition my_folder FLASH_IN_PROJECT)

不指定 FLASH_IN_PROJECT/SPIFFS_IMAGE_FLASH_IN_PROJECT 也可以生成镜像，但须使用 ``esptool.py``、``parttool.py`` 或自定义构建系统目标手动烧录。

有时基本目录中的内容是在构建时生成的，用户可以使用 DEPENDS/SPIFFS_IMAGE_DEPENDS 指定目标，因此可以在生成镜像之前执行此目标::

    add_custom_target(dep COMMAND ...)

    spiffs_create_partition_image(my_spiffs_partition my_folder DEPENDS dep)

请参考 :example:`storage/spiffsgen`，查看示例。

``mkspiffs``
^^^^^^^^^^^^

用户也可以使用 `mkspiffs <https://github.com/igrr/mkspiffs>`_ 工具创建 SPIFFS 分区镜像。与 ``spiffsgen.py`` 相似，`mkspiffs <https://github.com/igrr/mkspiffs>`_ 也可以用于从指定文件夹中生成镜像，然后使用 ``esptool.py`` 烧录镜像。

该工具需要获取以下参数：

- **Block Size**：4096（SPI flash 标准）
- **Page Size**：256（SPI flash 标准）
- **Image Size**：分区大小（以字节为单位，可从分区表中获取）
- **Partition Offset**：分区起始地址（可从分区表中获取）

运行以下命令，将文件夹打包成 1 MB 大小的镜像::

    mkspiffs -c [src_folder] -b 4096 -p 256 -s 0x100000 spiffs.bin

运行以下命令，将镜像烧录到 {IDF_TARGET_NAME}（偏移量：0x110000）::

    python esptool.py --chip {IDF_TARGET_PATH_NAME} --port [port] --baud [baud] write_flash -z 0x110000 spiffs.bin

选择合适的 SPIFFS 工具
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

上面介绍的两款 SPIFFS 工具功能相似，需根据实际情况，选择合适的一款。

以下情况优先选用 ``spiffsgen.py`` 工具：

1. 仅需在构建时简单生成 SPIFFS 镜像，请选择使用 ``spiffsgen.py``，因为 ``spiffsgen.py`` 可以直接在构建系统中使用函数或命令生成 SPIFFS 镜像。
2. 主机没有可用的 C/C++ 编译器时，可以选择使用 ``spiffsgen.py`` 工具，因为 ``spiffsgen.py`` 不需要编译。

以下情况优先选用 ``mkspiffs`` 工具：

1. 如果用户除了需要生成镜像外，还需要拆包 SPIFFS 镜像，请选择使用 ``mkspiffs`` 工具，因为 ``spiffsgen.py`` 目前尚不支持此功能。
2. 如果用户当前环境中 Python 解释器不可用，但主机编译器可用，或者有预编译的 ``mkspiffs`` 二进制文件，此时请选择使用 ``mkspiffs`` 工具。但是，``mkspiffs`` 没有集成到构建系统，用户必须自己完成以下工作：在构建期间编译 ``mkspiffs`` （如果未使用预编译的二进制文件），为输出文件创建构建规则或目标，将适当的参数传递给工具等。

另请参阅
--------

- :doc:`分区表 <../../api-guides/partition-tables>`


应用示例
-------------------

:example:`storage/spiffs` 目录下提供了 SPIFFS 应用示例。该示例初始化并挂载了一个 SPIFFS 分区，然后使用 POSIX 和 C 库 API 写入和读取数据。请参考 ``example`` 目录下的 README.md 文件，获取详细信息。

高级 API 参考
------------------------

.. include-build-file:: inc/esp_spiffs.inc
