******************************
Linux 环境下从源码编译 OpenOCD
******************************
:link_to_translation:`en:[English]`

除了从 `Espressif 官方 <https://github.com/espressif/openocd-esp32/releases>`_ 直接下载 OpenOCD 可执行文件，你还可以选择从源码编译得到 OpenOCD。如果想要快速设置 OpenOCD 而不是自行编译，请备份好当前文件，前往 :ref:`jtag-debugging-setup-openocd` 章节查阅。


.. highlight:: bash

下载 OpenOCD 源码
=================

支持 ESP32 的 OpenOCD 源代码可以从乐鑫官方的 GitHub 获得，网址为 https://github.com/espressif/openocd-esp32。请使用以下命令来下载源代码::

    cd ~/esp
    git clone --recursive https://github.com/espressif/openocd-esp32.git

克隆后的源代码被保存在 ``~/esp/openocd-esp32`` 目录中。


安装依赖的软件包
================

安装编译 OpenOCD 所需的软件包。

.. note::

    依次安装以下软件包，检查安装是否成功，然后继续下一个软件包的安装。在进行下一步操作之前，要先解决当前报告的问题。

::

    sudo apt-get install make
    sudo apt-get install libtool
    sudo apt-get install pkg-config
    sudo apt-get install autoconf
    sudo apt-get install automake
    sudo apt-get install texinfo
    sudo apt-get install libusb-1.0

.. note::

    * pkg-config 应为 0.2.3 或以上的版本。
    * autoconf 应为 2.6.4 或以上的版本。
    * automake 应为 1.9 或以上的版本。
    * 当使用 USB-Blaster，ASIX Presto，OpenJTAG 和 FT2232 作为适配器时，需要下载安装 libFTDI 和 FTD2XX 的驱动。
    * 当使用 CMSIS-DAP 时，需要安装 HIDAPI。


构建 OpenOCD
============

配置和构建 OpenOCD 的流程如下::

    cd ~/esp/openocd-esp32
    ./bootstrap
    ./configure
    make

你可以选择最后再执行 ``sudo make install`` ，如果你已经安装过别的开发平台的 OpenOCD，请跳过这个步骤，因为它可能会覆盖掉原来的 OpenOCD。

.. note::

    * 如果发生错误，请解决后再次尝试编译，直到 ``make`` 成功为止。
    * 如果 OpenOCD 存在子模块问题，请 ``cd`` 到 ``openocd-esp32`` 目录，并输入 ``git submodule update --init`` 命令。
    * 如果 ``./configure`` 成功运行，JTAG 被使能的信息会被打印在 ``OpenOCD configuration summary`` 下面。
    * 如果您的设备信息未显示在日志中，请根据 ``../openocd-esp32/doc/INSTALL.txt`` 文中的描述使用 ``./configure`` 启用它。
    * 有关编译 OpenOCD 的详细信息，请参阅 ``openocd-esp32/README``。

一旦 ``make`` 过程成功结束，OpenOCD 的可执行文件会被保存到 ``~/openocd-esp32/bin`` 目录中。


下一步
======

想要进一步配置调试环境，请前往 :ref:`jtag-debugging-configuring-esp32-target` 章节。
