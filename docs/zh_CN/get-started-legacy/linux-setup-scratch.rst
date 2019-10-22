****************************************************
从零开始设置 Linux 环境下的工具链（传统 GNU Make）
****************************************************
:link_to_translation:`en:[English]`

.. include:: ../gnu-make-legacy.rst

.. note::

    安装工具链的标准方法请见 :doc:`这里 <linux-setup>`。请参考 :ref:`工具链自定义设置 <get-started-customized-setup-legacy>` 章节，查看可能需要从头开始设置工具链的情况。

安装准备
========

编译 ESP-IDF 需要以下软件包：

- Ubuntu and Debian::

    sudo apt-get install gcc git wget make libncurses-dev flex bison gperf python python-pip python-setuptools python-serial python-cryptography python-future python-pyparsing python-pyelftools

- Arch::

    sudo pacman -S --needed gcc git make ncurses flex bison gperf python2-pyserial python2-cryptography python2-future python2-pyparsing python2-pyelftools

.. note::

    一些旧的（2014年之前）Linux 发行版中使用的 ``pyserial`` 版本可能是 2.x ，ESP-IDF 并不支持。这种情况下，请按照 :ref:`get-started-get-packages-legacy` 章节的介绍，使用 ``pip`` 安装软件包。

从源代码编译工具链
==================

- 安装依赖项：

    - CentOS 7::

        sudo yum install gawk gperf grep gettext ncurses-devel python python-devel automake bison flex texinfo help2man libtool

    - Ubuntu pre-16.04::

        sudo apt-get install gawk gperf grep gettext libncurses-dev python python-dev automake bison flex texinfo help2man libtool

    - Ubuntu 16.04 或以上 ::

        sudo apt-get install gawk gperf grep gettext python python-dev automake bison flex texinfo help2man libtool libtool-bin

    - Debian 9::

        sudo apt-get install gawk gperf grep gettext libncurses-dev python python-dev automake bison flex texinfo help2man libtool libtool-bin

    - Arch::

        TODO

- 创建工作目录，并进入该目录::

    mkdir -p ~/esp
    cd ~/esp

- 下载并编译 ``crosstool-NG`` ：

  .. include:: /_build/inc/scratch-build-code.inc

- 编译工具链::

    ./ct-ng xtensa-esp32-elf
    ./ct-ng build
    chmod -R u+w builds/xtensa-esp32-elf

  编译得到的工具链会被保存到 ``~/esp/crosstool-NG/builds/xtensa-esp32-elf``。请按照 :ref:`标准设置指南 <setup-linux-toolchain-add-it-to-path-legacy>` 的介绍，将工具链添加到 ``PATH``。


后续步骤
========

继续设置开发环境，请前往 :ref:`get-started-get-esp-idf-legacy` 章节。

