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

工具
-----

spiffsgen.py
^^^^^^^^^^^^^^^^

:component_file:`spiffsgen.py<spiffs/spiffsgen.py>`（只写）是 SPIFFS 的一种 Python 实现，可用于从主机文件夹内容生成文件系统映像。打开终端并运行以下命令即可使用 ``spiffsgen.py``::

    python spiffsgen.py <image_size> <base_dir> <output_file>

参数（必选）说明如下：

- **image_size**：分区大小，用于烧录生成的 SPIFFS 映像；
- **base_dir**：创建 SPIFFS 映像的目录；
- **output_file**：SPIFFS 映像输出文件。

其他参数（可选）也参与控制映像的生成，您可以运行以下帮助命令，查看这些参数的具体信息::

    python spiffsgen.py --help

上述可选参数对应 SPIFFS 构建配置选项。若想顺利生成可用的映像，请确保使用的参数或配置与构建 SPIFFS 时所用的参数或配置相同。运行帮助命令将显示参数所对应的 SPIFFS 构建配置。如未指定参数，将使用帮助信息中的默认值。 

映像生成后，您可以使用 ``esptool.py`` 或 ``parttool.py`` 烧录映像。

您可以在命令行或脚本中手动单独调用 ``spiffsgen.py``，也可以直接从构建系统调用 ``spiffs_create_partition_image`` 来使用 ``spiffsgen.py``。  

在 Make 构建系统中运行::

    SPIFFS_IMAGE_FLASH_IN_PROJECT := ...
    SPIFFS_IMAGE_DEPENDS := ...
    $(eval $(call spiffs_create_partition_image,<partition>,<base_dir>))

在 CMake 构建系统中运行::

    spiffs_create_partition_image(<partition> <base_dir> [FLASH_IN_PROJECT] [DEPENDS dep dep dep...])

在构建系统中使用 ``spiffsgen.py`` 更为方便，构建配置自动传递给 ``spiffsgen.py`` 工具，确保生成的映像可用于构建。比如，单独调用 ``spiffsgen.py`` 时需要用到 *image_size* 参数，但在构建系统中调用 ``spiffs_create_partition_image`` 时，仅需要 *partition* 参数，映像大小将直接从工程分区表中获取。   

Make 构建系统和 CMake 构建系统结构有所不同，请注意以下几点：

- 在 Make 构建系统中使用 ``spiffs_create_partition_image``，需从工程 Makefile 中调用；
- 在 CMake 构建系统中使用 ``spiffs_create_partition_image``，需从组件 CMakeLists.txt 文件调用。

您也可以指定 ``FLASH_IN_PROJECT``，然后使用 ``idf.py flash`` 或 ``make flash`` 将映像与应用程序二进制文件、分区表等一起自动烧录至设备，例如： 

在 Make 构建系统中运行::

    SPIFFS_IMAGE_FLASH_IN_PROJECT := 1
    $(eval $(call spiffs_create_partition_image,<partition>,<base_dir>))

在 CMake 构建系统中运行::

    spiffs_create_partition_image(my_spiffs_partition my_folder FLASH_IN_PROJECT)

不指定 FLASH_IN_PROJECT/SPIFFS_IMAGE_FLASH_IN_PROJECT 也可以生成映像，但须使用 ``esptool.py``、``parttool.py`` 或自定义构建系统目标手动烧录。

有时基本目录中的内容是在构建时生成的，您可以使用 DEPENDS/SPIFFS_IMAGE_DEPENDS 指定目标，因此可以在生成映像之前执行此目标。

在 Make 构建系统中运行::

    dep: 
        ...

    SPIFFS_IMAGE_DEPENDS := dep
    $(eval $(call spiffs_create_partition_image,<partition>,<base_dir>))

在 CMake 构建系统中运行::

    add_custom_target(dep COMMAND ...)

    spiffs_create_partition_image(my_spiffs_partition my_folder DEPENDS dep)

请参考 :example:`examples/storage/spiffsgen>`，查看示例。 

mkspiffs
^^^^^^^^^^^

您也可以使用 `mkspiffs <https://github.com/igrr/mkspiffs>`_ 工具创建 SPIFFS 分区映像。与 ``spiffsgen.py`` 相似，`mkspiffs <https://github.com/igrr/mkspiffs>`_ 也可以用于从指定文件夹中生成映像，然后使用 ``esptool.py`` 烧录映像。

该工具需要获取以下参数：

- **Block Size**：4096（SPI flash 标准）
- **Page Size**：256（SPI flash 标准）
- **Image Size**：分区大小（以字节为单位，可从分区表中获取）
- **Partition Offset**：分区起始地址（可从分区表内获取）

运行以下命令，将文件夹打包成 1 MB 大小的映像::

    mkspiffs -c [src_folder] -b 4096 -p 256 -s 0x100000 spiffs.bin

运行以下命令，将映像烧录到 ESP32（偏移量：0x110000）::

    python esptool.py --chip esp32 --port [port] --baud [baud] write_flash -z 0x110000 spiffs.bin


选择合适的 SPIFFS 工具
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

上面介绍的两款 SPIFFS 工具功能相似，需根据实际情况，选择合适的一款。

以下情况优先选用 ``spiffsgen.py`` 工具：

1. 仅需在构建时简单生成 SPIFFS 映像，请选择使用 ``spiffsgen.py``，``spiffsgen.py`` 可以直接在构建系统中使用函数或命令生成 SPIFFS 映像。
2. 主机没有可用的 C/C++ 编译器时，可以选择使用 ``spiffsgen.py`` 工具，``spiffsgen.py`` 不需要编译。

以下情况优先选用 ``mkspiffs`` 工具：

1. 如果您除了需要生成映像外，还需要拆包 SPIFFS 映像，请选择使用 ``mkspiffs`` 工具。``spiffsgen.py`` 目前尚不支持此功能。
2. 如果您当前环境中 Python 解释器不可用，但主机编译器可用，或者有预编译的 ``mkspiffs`` 二进制文件，此时请选择使用 ``mkspiffs`` 工具。但是，``mkspiffs`` 没有集成到构建系统，用户必须自己完成以下工作：在构建期间编译 ``mkspiffs`` （如果未使用预编译的二进制文件），为输出文件创建构建规则或目标，将适当的参数传递给工具等。

另请参阅
--------

- :doc:`分区表 <../../api-guides/partition-tables>`

应用示例
-------------------

:example:`storage/spiffs` 目录下提供了 SPIFFS 应用示例。该示例初始化并挂载了一个 SPIFFS 分区，然后使用 POSIX 和 C 库 API 写入和读取数据。请参考 ``example`` 目录下的 README.md 文件，查看详细信息。


高级 API 参考
------------------------

.. include:: /_build/inc/esp_spiffs.inc
