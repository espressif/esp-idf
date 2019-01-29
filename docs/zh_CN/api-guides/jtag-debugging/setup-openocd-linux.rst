***************************
在 Linux 环境下安装 OpenOCD
***************************
:link_to_translation:`en:[English]`

安装 OpenOCD
============

64 位 Linux 系统版本的 OpenOCD 可以直接从以下 Github 链接中下载：

https://github.com/espressif/openocd-esp32/releases

下载文件名称包含 `linux64` 字样的最新发布的归档文件，例如 `openocd-esp32-linux64-0.10.0-esp32-20180418.tar.gz`。

将该文件解压缩到 ``~/esp/`` 目录下::

    cd ~/esp
    tar -xzf ~/Downloads/openocd-esp32-linux64-<version>.tar.gz 


下一步
======

进一下配置调试环境，请前往 :ref:`jtag-debugging-configuring-esp32-target` 章节。


相关文档
========

.. toctree::
    :maxdepth: 1

    building-openocd-linux

