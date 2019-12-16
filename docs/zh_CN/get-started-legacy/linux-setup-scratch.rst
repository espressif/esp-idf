*****************************************************
从零开始设置 Linux 环境下的工具链  (传统 GNU Make)
*****************************************************
:link_to_translation:`en:[English]`

.. include:: ../gnu-make-legacy.rst

.. note::

    安装工具链的标准流程可以通过阅读文档 :doc:`Linux 平台工具链的标准设置 <linux-setup>` 来获得，:ref:`工具链的自定义设置 <get-started-customized-setup-legacy>` 章节会介绍哪些情况下我们必须要重新定义工具链。


安装必要的工具
==============

要想使用 ESP-IDF 进行编译，您需要获取以下软件包：

- Ubuntu 和 Debian::

    sudo apt-get install gcc git wget make libncurses-dev flex bison gperf python python-pip python-setuptools python-serial python-cryptography python-future python-pyparsing python-pyelftools libffi-dev libssl-dev

- Arch::

    sudo pacman -S --needed gcc git make ncurses flex bison gperf python2-pyserial python2-cryptography python2-future python2-pyparsing python2-pyelftools

.. note::

    一些旧的（2014年之前）Linux 发行版中使用的 ``pyserial`` 版本可能是 2.x ， ESP-IDF并不支持。
    在这种情况下，请参考 :ref:`安装依赖的 Python 软件包 <get-started-get-packages-legacy>` 章节，通过 ``pip`` 工具来安装支持的版本。

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

新建工作目录，然后进入::

  mkdir -p ~/esp
  cd ~/esp


下载 ``crosstool-NG`` 然后编译:

.. include:: /_build/inc/scratch-build-code.inc

编译工具链::

    ./ct-ng xtensa-esp32-elf
    ./ct-ng build
    chmod -R u+w builds/xtensa-esp32-elf

编译得到的工具链会被保存到 ``~/esp/crosstool-NG/builds/xtensa-esp32-elf``。根据 :ref:`Linux 下设置环境变量的标准方法 <setup-linux-toolchain-add-it-to-path-legacy>` 中的介绍，将工具链添加到 ``PATH`` 中。


下一步
======

继续设置开发环境，请前往 :ref:`获取 ESP-IDF <get-started-get-esp-idf-legacy>` 章节。
