************************************************
在 Mac OS 上安装 ESP32 工具链  (传统 GNU Make)
************************************************
:link_to_translation:`en:[English]`

.. include:: ../gnu-make-legacy.rst

安装准备
================

- 安装 pip::

    sudo easy_install pip

.. note::

    ``pip`` 稍后将用于安装 :ref:`必要的 Python 软件包 <get-started-get-packages-legacy>`。

安装工具链
===============

.. include:: /_build/inc/download-links.inc

Mac OS 版本的 ESP32 工具链可以从以下地址下载：

|download_link_osx|

下载压缩文件之后，解压到 ``~/esp`` 目录中:

.. include:: /_build/inc/unpack-code-osx.inc

.. _setup-macos-toolchain-add-it-to-path-legacy:

工具链将被解压到 ``~/esp/xtensa-esp32-elf/`` 路径下。

在 ``~/.profile`` 文件中更新 ``PATH`` 环境变量以使用工具链。为了使 ``xtensa-esp32-elf`` 在各种终端会话中都可用，在 ``~/.profile`` 文件中加上以下指令::

     export PATH=$HOME/esp/xtensa-esp32-elf/bin:$PATH

或者，您可以为上述命令创建一个别名。这样只有执行以下指令时工具链才能被使用。将下面的指令添加到您的 ``〜/ .profile`` 文件中::

    alias get_esp32="export PATH=$HOME/esp/xtensa-esp32-elf/bin:$PATH"

当需要使用工具链时，在命令行里输入 ``get_esp32``，就可以将工具链添加到 ``PATH`` 中。


下一步
==========

前往 :ref:`get-started-get-esp-idf-legacy` 继续配置开发环境。


相关文档
=================

.. toctree::
    :maxdepth: 1

    macos-setup-scratch

