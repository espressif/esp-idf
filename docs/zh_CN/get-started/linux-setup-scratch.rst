******************************************
从零开始设置 Linux 环境下的工具链
******************************************

:link_to_translation:`en:[English]`

除了从乐鑫官网直接下载已编译好的二进制工具链外，您还可以按照本文介绍，从头开始设置自己的工具链。如需快速使用已编译好的二进制工具链，可回到 :doc:`linux-setup` 章节。

安装准备
=====================

编译 ESP-IDF 需要以下软件包：

- CentOS 7::

    sudo yum install git wget ncurses-devel flex bison gperf python pyserial python-pyelftools cmake ninja-build ccache

- Ubuntu 和 Debian::

    sudo apt-get install git wget libncurses-dev flex bison gperf python python-pip python-setuptools python-serial python-click python-cryptography python-future python-pyparsing python-pyelftools cmake ninja-build ccache

- Arch::

    sudo pacman -S --needed gcc git make ncurses flex bison gperf python2-pyserial python2-click python2-cryptography python2-future python2-pyparsing python2-pyelftools cmake ninja ccache

.. note::
    使用 ESP-IDF 需要 CMake 3.5 或以上版本。较早版本的 Linux 可能需要升级才能向后移植仓库，或安装 "cmake3" 软件包，而不是安装 "cmake"。

从源代码编译工具链
=================================

安装依赖项：

- CentOS 7::

    sudo yum install gawk gperf grep gettext ncurses-devel python python-devel automake bison flex texinfo help2man libtool make

- Ubuntu pre-16.04::

    sudo apt-get install gawk gperf grep gettext libncurses-dev python python-dev automake bison flex texinfo help2man libtool make

- Ubuntu 16.04 或以上 ::

    sudo apt-get install gawk gperf grep gettext python python-dev automake bison flex texinfo help2man libtool libtool-bin make

- Debian 9::

    sudo apt-get install gawk gperf grep gettext libncurses-dev python python-dev automake bison flex texinfo help2man libtool libtool-bin make

- Arch::

    TODO

创建工作目录，并进入该目录::

    mkdir -p ~/esp
    cd ~/esp

下载并编译 ``crosstool-NG`` ：

.. include:: /_build/inc/scratch-build-code.inc

编译工具链::

    ./ct-ng xtensa-esp32-elf
    ./ct-ng build
    chmod -R u+w builds/xtensa-esp32-elf

编译得到的工具链会被保存到 ``~/esp/crosstool-NG/builds/xtensa-esp32-elf``。请按照 :ref:`标准设置指南 <setup-linux-toolchain-add-it-to-path-legacy>` 的介绍，将工具链添加到 ``PATH``。


后续步骤
==========

继续设置开发环境，请前往 :ref:`get-started-get-esp-idf` 章节。

