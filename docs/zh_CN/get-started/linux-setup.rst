*********************************************
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

兼容的 Python 版本
=================================

ESP-IDF 支持 Python 3.6 及以上版本，建议升级操作系统到最新版本从而更新 Python。也可选择从 `sources <https://www.python.org/downloads/>`_ 安装最新版 Python，或使用 Python 管理系统如 `pyenv <https://github.com/pyenv/pyenv>`_ 对版本进行升级管理。

后续步骤
==========

继续设置开发环境，请前往 :ref:`get-started-get-esp-idf` 章节。


.. _AUR: https://wiki.archlinux.org/index.php/Arch_User_Repository
