**********************************************
Windows 平台工具链的标准设置（传统 GNU Make）
**********************************************
:link_to_translation:`en:[English]`

.. include:: ../gnu-make-legacy.rst

概述
====

Windows 系统没有内置的 "make" 环境，所以除了安装工具链之外，您还需要一个兼容 GNU 的环境。这里，我们使用 MSYS2_ 环境兼容 GNU。您无需一直使用这个环境，比如您可以使用 :doc:`Eclipse <eclipse-setup>` 或其他前端，仅需在后台运行 MSYS2_ 即可。


工具链设置
==========

最简便的工具链设置方法是从下方地址下载 Windows 多合一工具链和 MSYS2 压缩包文件：

https://dl.espressif.com/dl/esp32_win32_msys2_environment_and_toolchain-20190611.zip

请将压缩包文件解压至 ``C:\``（或其他目录，但本文档中以 ``C:\`` 为例），该文件将为您创建一个带有预配置环境的 ``msys32`` 目录。


开始尝试
========

请运行 ``C:\msys32\mingw32.exe`` 文件，打开一个 MSYS2 MINGW32 终端窗口。该窗口的环境为 bash shell。请创建一个名为 ``esp`` 的文件夹，作为 ESP32 应用程序开发的默认目录。您可以使用以下命令创建文件夹::

    mkdir -p ~/esp

您还可以通过 ``cd ~/esp`` 命令，进入刚刚创建的文件夹。如无其他问题，本步骤到此结束。

.. figure:: ../../_static/msys2-terminal-window.png
    :align: center
    :alt: MSYS2 MINGW32 shell 窗口
    :figclass: align-center

    MSYS2 MINGW32 shell 窗口

请在后续步骤中，使用本窗口配置 ESP32 的开发环境。


后续步骤
==========

继续设置开发环境，请前往 :ref:`get-started-get-esp-idf-legacy` 章节。

更新环境
========

当 IDF 有更新时，有时需要安装新的工具链或为 Windows MSYS2 环境添加新的要求。如需将旧版本预编译环境中的数据迁移至新版本，您可以：

1. 复制旧的 MSYS2 环境（即 ``C:\msys32``），并将其移动/重命名到不同目录下（即 ``C:\msys32_old``）。
2. 使用上述步骤，下载新的预编译环境。
3. 将新的 MSYS2 环境解压缩至 ``C:\msys32`` （或您指定的其他位置）。
4. 找到旧的 ``C:\msys32_old\home`` 文件夹，并将其移动至 ``C:\msys32``。
5. 此时，如无其他需要，您可以删除旧的 ``C:\msys32_old\home`` 文件夹。

注意，您可以在电脑中安装多个不同的 MSYS2 环境，仅需将它们保存在不同的路径下即可。

此外，您还可以 :ref:`直接更新现有环境（无需下载新的版本）<updating-existing-windows-environment-legacy>`，但步骤更加复杂。

相关文档
========

.. toctree::
    :maxdepth: 1

    windows-setup-scratch


.. _MSYS2: https://msys2.github.io/

