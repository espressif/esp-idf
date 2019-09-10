*********************************************
Linux 平台工具链的标准设置
*********************************************

:link_to_translation:`en:[English]`

安装准备
=====================

编译 ESP-IDF 需要以下软件包：

- CentOS 7::

    sudo yum install git wget flex bison gperf python pyserial python-pyelftools cmake ninja-build ccache

- Ubuntu 和 Debian::

    sudo apt-get install git wget flex bison gperf python python-pip python-setuptools python-serial python-click python-cryptography python-future python-pyparsing python-pyelftools cmake ninja-build ccache

- Arch::

    sudo pacman -S --needed gcc git make flex bison gperf python2-pip python2-pyserial python2-click python2-cryptography python2-future python2-pyparsing python2-pyelftools cmake ninja ccache

.. note::
    使用 ESP-IDF 需要 CMake 3.5 或以上版本。较早版本的 Linux 可能需要升级才能向后移植仓库，或安装 "cmake3" 软件包，而不是安装 "cmake"。

其他提示
===============

权限问题 /dev/ttyUSB0
------------------------------------------------------------

使用某些 Linux 版本向 ESP32 烧写固件时，可能会出现 ``Failed to open port /dev/ttyUSB0`` 错误消息。此时，可以将当前用户增加至 :ref:` Linux Dialout 组 <linux-dialout-group>`。

后续步骤
==========

继续设置开发环境，请前往 :ref:`get-started-get-esp-idf` 章节。


相关文档
=================

.. toctree::
    :maxdepth: 1

    linux-setup-scratch


.. _AUR: https://wiki.archlinux.org/index.php/Arch_User_Repository

