**********************************
从零开始设置 Linux 环境下的工具链
**********************************

除了直接从 Espressif 官网下载二进制格式的工具链，下面将再介绍一种可替代的办法。如果想要快速设置二进制工具链而不是手动从源码编译，请做好备份，并前往 :doc:`Linux 环境下的设置 <linux-setup>` 章节。


安装必要的工具
==============

要想使用 ESP-IDF 进行编译，您需要获取以下软件包：

- Ubuntu 和 Debian::

    sudo apt-get install git wget make libncurses-dev flex bison gperf python python-pip python-setuptools python-serial

- Arch::

    sudo pacman -S --needed gcc git make ncurses flex bison gperf python2-pyserial

.. note::

    一些旧的（2014年之前）Linux 发行版中使用的 ``pyserial`` 版本可能是 2.x ， ESP-IDF并不支持。
    在这种情况下，请参考 :ref:`安装依赖的 Python 软件包 <get-started-get-packages>` 章节，通过 ``pip`` 工具来安装支持的版本。

从源代码编译工具链
==================

- 安装依赖:

  - CentOS 7::

        sudo yum install gawk gperf grep gettext ncurses-devel python python-devel automake bison flex texinfo help2man libtool

  - Ubuntu pre-16.04::

        sudo apt-get install gawk gperf grep gettext libncurses-dev python python-dev automake bison flex texinfo help2man libtool

  - Ubuntu 16.04::

        sudo apt-get install gawk gperf grep gettext python python-dev automake bison flex texinfo help2man libtool libtool-bin

  - Debian 9::

        sudo apt-get install gawk gperf grep gettext libncurses-dev python python-dev automake bison flex texinfo help2man libtool libtool-bin

  - Arch::

        TODO

下载 ``crosstool-NG`` 然后编译:

.. include:: /_build/inc/scratch-build-code.inc

编译工具链::

    ./ct-ng xtensa-esp32-elf
    ./ct-ng build
    chmod -R u+w builds/xtensa-esp32-elf

编译得到的工具链会被保存到 ``~/esp/crosstool-NG/builds/xtensa-esp32-elf``。根据 :ref:`Linux 下设置环境变量的标准方法 <setup-linux-toolchain-add-it-to-path>` 中的介绍，将工具链添加到 ``PATH`` 中。


下一步
======

继续设置开发环境，请前往 :ref:`获取 ESP-IDF <get-started-get-esp-idf>` 章节。
