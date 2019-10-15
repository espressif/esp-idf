*********************************************
Linux 平台工具链的标准设置
*********************************************

:link_to_translation:`en:[English]`

安装准备
=====================

编译 ESP-IDF 需要以下软件包：

- CentOS 7::

    sudo yum install git wget ncurses-devel flex bison gperf python pyserial python-pyelftools cmake ninja-build ccache

- Ubuntu 和 Debian::

    sudo apt-get install git wget libncurses-dev flex bison gperf python python-pip python-setuptools python-serial python-click python-cryptography python-future python-pyparsing python-pyelftools cmake ninja-build ccache

- Arch::

    sudo pacman -S --needed gcc git make ncurses flex bison gperf python2-pip python2-pyserial python2-click python2-cryptography python2-future python2-pyparsing python2-pyelftools cmake ninja ccache

.. note::
    使用 ESP-IDF 需要 CMake 3.5 或以上版本。较早版本的 Linux 可能需要升级才能向后移植仓库，或安装 "cmake3" 软件包，而不是安装 "cmake"。

其他提示
===============

权限问题 /dev/ttyUSB0
------------------------------------------------------------

使用某些 Linux 版本向 ESP32 烧写固件时，可能会出现 ``Failed to open port /dev/ttyUSB0`` 错误消息。此时，可以将当前用户增加至 :ref:` Linux Dialout 组 <linux-dialout-group>`。

ncurses 5 依赖项
--------------------

在 Linux 上运行预编译的 gdb (xtensa-esp32-elf-gdb) 需要 ncurses 5，但一些较新版本默认只提供 ncurses 6。

请查看对应版本信息，确认是否存在可用的 ncurses 5。此外，您也可以使用 crosstool-NG 编译一个链接到 ncurses 6 的 gdb。

Arch Linux 用户可在 AUR_ 中获得 native 和 lib32 配置的 ncurses 5 库：

- https://aur.archlinux.org/packages/ncurses5-compat-libs/
- https://aur.archlinux.org/packages/lib32-ncurses5-compat-libs/

在安装这些软件包之前，您可能需要将作者的公钥添加到您的密钥环中，具体参考上方的“注释”部分。

后续步骤
==========

继续设置开发环境，请前往 :ref:`get-started-get-esp-idf` 章节。


相关文档
=================

.. toctree::
    :maxdepth: 1

    linux-setup-scratch


.. _AUR: https://wiki.archlinux.org/index.php/Arch_User_Repository

