通过 make 指令创建和烧录项目（传统 GNU Make）
=================================================
:link_to_translation:`en:[English]`

.. include:: ../gnu-make-legacy.rst


寻找项目
--------

除了 `esp-idf-template <https://github.com/espressif/esp-idf-template>`_ 项目模版外，ESP-IDF 还在 GitHub 仓库中的 :idf:`examples` 目录下提供多个示例项目。

请找到并进入您想要的项目，开始配置、构建该项目。


配置项目
--------

::

    make menuconfig


编译项目
--------

::

    make all

该命令将配置 app 和 bootloader，并根据配置生成分区表。


烧录项目
--------

``make all`` 完成后将打印一行命令，提示您如何使用 esptool.py 烧录芯片。不过，您可以使用以下命令进行烧录::

    make flash

该命令可以将整个项目（包括 app、bootloader 和分区表）烧录至新芯片。此外，如果分区表中存在 ota_data，则该命令还会同时将初始 ota_data 烧录至芯片。
该命令允许您直接从 factory 分区中运行新加载的 app（如果 factory 分区不存在，则从第一个 OTA 分区开始运行）。
您可以使用 ``make menuconfig`` 配置串口的烧录。

在运行 ``make flash`` 之前无需单独运行 ``make all``。``make flash`` 命令本身就可以自动构建所需的文件。


仅编译和烧录应用程序
--------------------

在首次烧录完成后，您可以只希望烧录并编译应用程序，而不再包括 bootloader 和分区表。这种情况可以使用以下命令：

* ``make app`` -- 仅编译应用程序
* ``make app-flash`` -- 仅烧录应用程序

``make app-flash`` -- 在有需要时自动重新编译应用程序

如果没有变化，每次都重新烧录、编译 bootloader 和分区表也没有坏处。


分区表
------

项目编译完成后，"build" 目录下将出现一个名为 "my_app.bin" （例）的二进制文件。这是一个可由 bootloader 加载的 ESP32 映像文件。

ESP32 的一块 flash 可以包含多个应用程序，以及多种数据（校准数据、文件系统、参数存储等）。因此，在向 flash 烧录分区表时，通常保留 0x8000 的偏移量。

分区表中的每个条目都有名称（标签）、类型（app、数据或其他）、子类型及在 flash 中的偏移量。

使用分区表最简便的方法是：运行 ``make menuconfig``，选择一个预定义分区表：

* "Single factory app, no OTA"
* "Factory app, two OTA definitions"

上述两种情况中，factory app 在 flash 中的烧录偏移地址均为 0x10000。运行 ``make partition_table`` 命令可以打印分区表摘要。

更多有关 :doc:`分区表 <../api-guides/partition-tables>` 及自定义分区表的内容，请见 :doc:`相关文档 <../api-guides/partition-tables>`。


