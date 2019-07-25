通过 make 指令创建和烧录项目 (传统 GNU Make)
============================================
:link_to_translation:`en:[English]`

.. include:: ../gnu-make-legacy.rst


寻找项目
-----------------

和 `esp-idf-template <https://github.com/espressif/esp-idf-template>`_ 项目一样，ESP-IDF 在 Github 上的 :idf:`examples` 目录下也有示例项目。

找到需要的项目后，切换到其目录，然后可以对其进行配置和构建。


配置项目
------------------------

::

    make menuconfig


编译项目
----------------------

::

    make all

... 该命令将配置 app 和 bootloader 并根据配置生成分区表。


烧录项目
---------------------

当 ``make all`` 结束后，系统将打印一命令行提示您如何使用 esptool.py 烧录芯片。用户也可以通过以下指令进行烧录::

    make flash

这种方法将烧录整个项目（包括 app, bootloader 和分割表）到芯片中。通过命令 `make menuconfig` 可以配置串口。

运行 ``make flash`` 之前无需运行 ``make all``。运行 ``make flash`` 将自动重建烧录所需的一切。


仅编译和烧录应用程序
---------------------------------

在最初的烧录之后，用户可以仅创建烧录 app，不烧录 bootloader 和分区表:

* ``make app`` - 仅创建应用程序。
* ``make app-flash`` - 仅烧录应用程序。

需要时 ``make app-flash`` 指令将自动重建 app。

如果 bootloader 和分区表不变的话，对他们进行重新烧录并不会有负面影响。

分区表
-------------------

编译完项目后，"build" 目录将包含一个名为 "my_app.bin" 的二进制文件。这是一个可由 bootloader 加载的 ESP32 映像二进制文件。

一个 ESP32 flash 可以包含多个应用程序，以及多种数据（校准数据，文件系统，参数存储等）。因此，分区表烧录在 flash 偏移地址 0x8000 的地方。

分区表中的每个条目都有一个名称（标签），类型（app，数据或其他），子类型和闪存中分区表被存放的偏移量。

使用分区表最简单的方法是 `make menuconfig` 并选择一个简单的预定义分区表:

* "Single factory app, no OTA"
* "Factory app, two OTA definitions"

在这两种情况下，出厂应用程序的烧录偏移为 0x10000。运行 `make partition_table`，可以打印分区表摘要。

更多关于 :doc:`分区表 <../api-guides/partition-tables>` 的信息，以及如何创建自定义分区表，可以查看 :doc:`文档 <../api-guides/partition-tables>`。
