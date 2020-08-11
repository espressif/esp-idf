*********************************************
Linux 平台工具链的标准设置
*********************************************

:link_to_translation:`en:[English]`

安装准备
=====================

编译 ESP-IDF 需要以下软件包：

- CentOS 7::

    sudo yum install git wget flex bison gperf python cmake ninja-build ccache dfu-util

- Ubuntu 和 Debian::

    sudo apt-get install git wget flex bison gperf python python-pip python-setuptools cmake ninja-build ccache libffi-dev libssl-dev dfu-util

- Arch::

    sudo pacman -S --needed gcc git make flex bison gperf python-pip python-pyserial cmake ninja ccache dfu-util

.. note::
    使用 ESP-IDF 需要 CMake 3.5 或以上版本。较早版本的 Linux 可能需要升级才能向后移植仓库，或安装 "cmake3" 软件包，而不是安装 "cmake"。

其他提示
===============

权限问题 /dev/ttyUSB0
------------------------------------------------------------

使用某些 Linux 版本向 {IDF_TARGET_NAME} 烧写固件时，可能会出现 ``Failed to open port /dev/ttyUSB0`` 错误消息。此时，可以将当前用户增加至 :ref:` Linux Dialout 组 <linux-dialout-group>`。

设置 Python 3 为 Ubuntu 和 Debian 默认 Python 版本
----------------------------------------------------

Ubuntu v20.04 和 Debian v7 之前的版本默认解释器为 Python 2.7，但这里推荐使用 Python 3，您可以运行下方命令安装 Python 3。或者查看当前所用系统的相关文档，按照文档推荐的其它方法安装 Python 3::

    sudo apt-get install python3 python3-pip python3-setuptools

设置 Python 3 为默认 Python 版本::

    sudo update-alternatives --install /usr/bin/python python /usr/bin/python3 10

.. note::
    上述设置为全局设置，同时会影响到其它应用。

后续步骤
==========

继续设置开发环境，请前往 :ref:`get-started-get-esp-idf` 章节。


相关文档
=================

.. toctree::
    :maxdepth: 1

    :esp32: linux-setup-scratch


.. _AUR: https://wiki.archlinux.org/index.php/Arch_User_Repository

