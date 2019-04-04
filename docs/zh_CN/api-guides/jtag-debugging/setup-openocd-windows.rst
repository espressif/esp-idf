*****************************
在 Windows 环境下安装 OpenOCD
*****************************
:link_to_translation:`en:[English]`

IDF 工具安装程序
================

如果您正在使用 CMake 构建系统，并遵循 :doc:`/get-started-cmake/windows-setup` 章节的指导使用了 ``ESP-IDF Tools Installer`` 的 V1.2 及其以上版本，那么默认情况下您已经安装好了 ``OpenOCD`` 软件。

``ESP-IDF Tools Installer`` 会将 ``OpenOCD`` 添加到环境变量 ``PATH`` 中，这样你就可以在任何目录运行它。

安装 OpenOCD
============

Windows 系统版本的 OpenOCD 可以直接从以下 Github 链接中下载：

https://github.com/espressif/openocd-esp32/releases

下载文件名包含 `win32` 字样的最新发布的归档文件，例如 `openocd-esp32-macos-0.10.0-win32-20180418.zip`。

将该文件解压缩到 ``~/esp/`` 目录下::

    cd ~/esp
    unzip /c/Users/<user>/Downloads/openocd-esp32-win32-<version>.zip

下一步
======

进一下配置调试环境，请前往 :ref:`jtag-debugging-configuring-esp32-target` 章节。


相关文档
========

.. toctree::
    :maxdepth: 1

    building-openocd-windows
