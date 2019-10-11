*********************************************************************
从零开始设置 Mac OS 环境下的工具链
*********************************************************************

:link_to_translation:`en:[英文]`

软件包管理器
======================

从零开始设置工具链，你需要安装 MacPorts_ 或 homebrew_ 包管理器。或者，你也可以直接 :doc:`下载预编译的工具链 <macos-setup>`。

MacPorts_ 需要安装完整的 XCode 软件，而 homebrew_ 只需要安装 XCode 命令行工具即可。

    .. _homebrew: https://brew.sh/
    .. _MacPorts: https://www.macports.org/install.php

请参考 :ref:`工具链自定义设置 <get-started-customized-setup>` 章节，查看在哪些情景下需要从头开始设置工具链。

准备工作
============================

- 安装 pip::

    sudo easy_install pip

- 安装 pyserial::

    pip install --user pyserial

- 安装 CMake 和 Ninja 编译工具：

  - 若使用 HomeBrew，你可以运行::

      brew install cmake ninja

  - 若使用 MacPorts，你可以运行::

      sudo port install cmake ninja

从源代码编译工具链
========================================

- 相关安装：

  - 对于 MacPorts::

        sudo port install gsed gawk binutils gperf grep gettext wget libtool autoconf automake make

  - 对于 homebrew::

        brew install gnu-sed gawk binutils gperftools gettext wget help2man libtool autoconf automake make

创建一个文件系统镜像（区分大小写）::

    hdiutil create ~/esp/crosstool.dmg -volname "ctng" -size 10g -fs "Case-sensitive HFS+"

挂载::

    hdiutil mount ~/esp/crosstool.dmg

创建指向你工作目录的符号链接::

    mkdir -p ~/esp
    ln -s /Volumes/ctng ~/esp/ctng-volume

前往新创建的目录：::

    cd ~/esp/ctng-volume

下载 ``crosstool-NG``，并开始编译：

.. include:: /_build/inc/scratch-build-code.inc

编译工具链：::

    ./ct-ng xtensa-esp32-elf
    ./ct-ng build
    chmod -R u+w builds/xtensa-esp32-elf

编译后的工具链将保存在 ``~/esp/ctng-volume/crosstool-NG/builds/xtensa-esp32-elf``。根据 :ref:`Mac OS 下设置环境变量的标准方法 <setup-macos-toolchain-add-it-to-path>` 中的介绍，将工具链添加到 ``PATH`` 中。


后续步骤
=================

继续设置开发环境，请前往 :ref:`获取 ESP-IDF <get-started-get-esp-idf>` 章节。
