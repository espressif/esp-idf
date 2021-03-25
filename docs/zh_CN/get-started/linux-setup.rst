﻿*********************************************
Linux 平台工具链的标准设置
*********************************************

:link_to_translation:`en:[English]`

安装准备
=====================

编译 ESP-IDF 需要以下软件包。请根据使用的系统，选择合适的安装命令，如下所示：

- Ubuntu 和 Debian::

    sudo apt-get install git wget flex bison gperf python3 python3-pip python3-setuptools cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0

- CentOS 7 & 8::

    sudo yum -y update && sudo yum install git wget flex bison gperf python3 python3-pip python3-setuptools cmake ninja-build ccache dfu-util libusbx

目前仍然支持 CentOS 7，但为了更好的用户体验，建议使用 CentOS 8。

- Arch::

    sudo pacman -S --needed gcc git make flex bison gperf python-pip cmake ninja ccache dfu-util libusb

.. note::
    - 使用 ESP-IDF 需要 CMake 3.5 或以上版本。较早的 Linux 发行版可能需要升级自身的软件源仓库，或开启 backports 套件库，或安装 "cmake3" 软件包（不是安装 "cmake"）。
    - 如果上述列表中，并没有您使用的系统，请参考您所用系统的相关文档，查看安装软件包所用的命令。

其他提示
===============

权限问题 /dev/ttyUSB0
------------------------------------------------------------

使用某些 Linux 版本向 {IDF_TARGET_NAME} 烧录固件时，可能会出现 ``Failed to open port /dev/ttyUSB0`` 错误消息。此时可以将用户添加至 :ref:`Linux Dialout 组<linux-dialout-group>`。

修复 Ubuntu 16.04 损坏的 pip 
=================================

``python3-pip`` 包可能已损坏无法升级。需使用脚本 `get-pip.py <https://bootstrap.pypa.io/get-pip.py>`_ 手动删除并安装该包::

    apt remove python3-pip python3-virtualenv; rm -r ~/.local
    rm -r ~/.espressif/python_env && python get-pip.py

停用 Python 2 
====================

Python 2 已经 `结束生命周期 <https://www.python.org/doc/sunset-python-2/>`_，ESP-IDF 很快将不再支持 Python 2。请安装 Python 3.6 或以上版本。可参考上面列出的目前主流 Linux 发行版的安装说明。


后续步骤
==========

继续设置开发环境，请前往 :ref:`get-started-get-esp-idf` 章节。


.. _AUR: https://wiki.archlinux.org/index.php/Arch_User_Repository
