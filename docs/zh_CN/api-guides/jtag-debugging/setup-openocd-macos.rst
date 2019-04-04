***************************
在 MacOS 环境下安装 OpenOCD
***************************
:link_to_translation:`en:[English]`

安装 libusb
===========

使用 `Homebrew <https://brew.sh/>`_ 或者 `Macports <https://www.macports.org/>`_ 来安装 `libusb` 软件包。

安装 OpenOCD
============

MacOS 系统版本的 OpenOCD 可以直接从以下 Github 链接中下载：

https://github.com/espressif/openocd-esp32/releases

下载文件名包含 `macos` 字样的最新发布的归档文件，例如 `openocd-esp32-macos-0.10.0-esp32-20180418.tar.gz`。

将该文件解压缩到 ``~/esp/`` 目录下::

    cd ~/esp
    tar -xzf ~/Downloads/openocd-esp32-macos-<version>.tar.gz 


下一步
======

进一下配置调试环境，请前往 :ref:`jtag-debugging-configuring-esp32-target` 章节。


相关文档
========

.. toctree::
    :maxdepth: 1

    building-openocd-macos

