******************************************
从零开始设置 Linux 环境下的工具链
******************************************

:link_to_translation:`en:[English]`

除了从乐鑫官网直接下载已编译好的二进制工具链外，您还可以按照本文介绍，从头开始设置自己的工具链。如需快速使用已编译好的二进制工具链，可回到 :doc:`linux-setup` 章节。

.. 注解:: 设置自己的工具链可以解决 Y2K38 问题（time_t 从 32 位扩展到 64 位）。

安装准备
=====================

编译 ESP-IDF 需要以下软件包：

- CentOS 7::

    sudo yum -y update && sudo yum install git wget ncurses-devel flex bison gperf python3 python3-pip cmake ninja-build ccache dfu-util

目前仍然支持 CentOS 7，但为了更好的用户体验，建议使用 CentOS 8。

- Ubuntu 和 Debian::

    sudo apt-get install git wget libncurses-dev flex bison gperf python3 python3-pip python3-setuptools python3-serial python3-cryptography python3-future python3-pyparsing python3-pyelftools cmake ninja-build ccache libffi-dev libssl-dev dfu-util

- Arch::

    sudo pacman -Sy --needed gcc git make ncurses flex bison gperf python-pyserial python-cryptography python-future python-pyparsing python-pyelftools cmake ninja ccache dfu-util

.. 注解::
    使用 ESP-IDF 需要 CMake 3.5 或以上版本。较早的 Linux 发行版可能需要升级自身的软件源仓库，或开启 backports 套件库，或安装 "cmake3" 软件包（不是安装 "cmake")。

从源代码编译工具链
=================================

安装依赖项：

- CentOS 7::

    sudo yum install gawk gperf grep gettext ncurses-devel python3 python3-devel automake bison flex texinfo help2man libtool make

- Ubuntu pre-16.04::

    sudo apt-get install gawk gperf grep gettext libncurses-dev python python-dev automake bison flex texinfo help2man libtool make

- Ubuntu 16.04 或以上 ::

    sudo apt-get install gawk gperf grep gettext python python-dev automake bison flex texinfo help2man libtool libtool-bin make

- Debian 9::

    sudo apt-get install gawk gperf grep gettext libncurses-dev python python-dev automake bison flex texinfo help2man libtool libtool-bin make

- Arch::

    sudo pacman -Sy --needed python-pip

创建工作目录，并进入该目录::

    mkdir -p ~/esp
    cd ~/esp

下载并编译 ``crosstool-NG`` ：

.. include-build-file:: inc/scratch-build-code.inc

.. 注解:: 在设置支持 64 位 time_t 的工具链时，您需要将 ``crosstool-NG/samples/xtensa-esp32-elf/crosstool.config`` 文件中第 33 和 43 行的可选参数 ``--enable-newlib-long-time_t`` 删除。

编译工具链::

    ./ct-ng xtensa-{IDF_TARGET_TOOLCHAIN_NAME}-elf
    ./ct-ng build
    chmod -R u+w builds/xtensa-{IDF_TARGET_TOOLCHAIN_NAME}-elf

编译得到的工具链会被保存至 ``~/esp/crosstool-NG/builds/xtensa-{IDF_TARGET_TOOLCHAIN_NAME}-elf``。

添加工具链到 PATH 环境变量
===========================

需要将自定义工具链复制到一个二进制目录中，并将其添加到 ``PATH`` 中。例如，您可以将编译好的工具链复制到 ``~/esp/xtensa-{IDF_TARGET_TOOLCHAIN_NAME}-elf/`` 目录中。

为了正常使用工具链，您需要更新 ``~/.profile`` 文件中 ``PATH`` 环境变量。此外，您还可以在 ``~/.profile`` 文件中增加以下代码。这样，所有终端窗口均可以使用 ``xtensa-{IDF_TARGET_TOOLCHAIN_NAME}-elf``::

    export PATH="$HOME/esp/xtensa-{IDF_TARGET_TOOLCHAIN_NAME}-elf/bin:$PATH"

.. 注解::

    如果您已将 ``/bin/bash`` 设置为登录 shell，且同时存在 ``.bash_profile`` 和 ``.profile`` 两个文件，则请更新 ``.bash_profile``。在 CentOS 环境下, ``alias`` 需要添加到 ``.bashrc`` 文件中。

退出并重新登录以使 ``.profile`` 的更改生效。运行以下命令来检查 ``PATH`` 设置是否正确::

    printenv PATH

此时您需要检查输出结果的开头中是否包含类似如下的工具链路径::

    $ printenv PATH
    /home/user-name/esp/xtensa-{IDF_TARGET_TOOLCHAIN_NAME}-elf/bin:/home/user-name/bin:/home/user-name/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin

注意这里的 ``/home/user-name`` 应该替换成您安装的主路径。

停用 Python 2
====================

Python 2 已经 `结束生命周期 <https://www.python.org/doc/sunset-python-2/>`_，ESP-IDF 很快将不再支持 Python 2。请安装 Python 3.6 或以上版本。可参考上面列出的目前主流 Linux 发行版的安装说明。


后续步骤
==========

继续设置开发环境，请前往 :ref:`get-started-get-esp-idf` 章节。
