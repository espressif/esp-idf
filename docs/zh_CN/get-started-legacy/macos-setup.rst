************************************************
Mac OS 平台工具链的标准设置（传统 GNU Make）
************************************************
:link_to_translation:`en:[English]`

.. include:: ../gnu-make-legacy.rst

安装准备
========

- 安装 pip::

    sudo easy_install pip

.. note::

    ``pip`` 稍后将用于安装 :ref:`所需 Python 包 <get-started-get-packages-legacy>`。

工具链设置
==========

.. include:: /_build/inc/download-links.inc

Mac OS 版本的 ESP32 工具链可以从以下地址下载：

|download_link_osx|

下载压缩文件之后，解压到 ``~/esp`` 目录中:

.. include:: /_build/inc/unpack-code-osx.inc

.. _setup-macos-toolchain-add-it-to-path-legacy:

工具链将被解压到 ``~/esp/xtensa-esp32-elf/`` 路径下。

为了正常使用工具链，您必须更新 ``~/.profile`` 文件中的 ``PATH`` 环境变量。此外，您还可以在 ``~/.profile`` 文件中增加以下代码，这样一来，所有终端窗口均可以使用 ``xtensa-esp32-elf``::

    export PATH=$HOME/esp/xtensa-esp32-elf/bin:$PATH

或者，您可以为上述命令创建一个别名。这样，您只有在需要时才可以使用工具链。如需设置别名，请将以下代码增加至您的 ``〜/ .profile`` 文件中::

    alias get_esp32="export PATH=$HOME/esp/xtensa-esp32-elf/bin:$PATH"

这样，您可以在终端输入 ``get_esp32`` 命令将工具链添加至您的 ``PATH``，从而使用工具链。


后续步骤
========

继续设置开发环境，请前往 :ref:`get-started-get-esp-idf-legacy` 章节。


相关文档
========

.. toctree::
    :maxdepth: 1

    macos-setup-scratch

