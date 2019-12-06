********************************************************
从零开始设置 Mac OS 环境下的工具链（传统 GNU Make） 
********************************************************
:link_to_translation:`en:[English]`

.. include:: ../gnu-make-legacy.rst

.. note::

    安装工具链的标准方法请见 :doc:`这里 <macos-setup>`。请参考 :ref:`工具链自定义设置 <get-started-customized-setup-legacy>` 章节，查看可能需要从头开始设置工具链的情况。

安装准备
========

- 安装 pip::

    sudo easy_install pip

.. note::

    ``pip`` 稍后将用于安装 :ref:`必要的 Python 软件包 <get-started-get-packages-legacy>`。

从源代码编译工具链
===================

- 安装依赖项：

    - 安装 MacPorts_ 或 homebrew_ 安装包管理器。MacPorts 需要完整的 XCode 软件，而 homebrew 只需要安装 XCode 命令行工具即可。

    .. _homebrew: https://brew.sh/
    .. _MacPorts: https://www.macports.org/install.php

        - 对于 MacPorts::

            sudo port install gsed gawk binutils gperf grep gettext wget libtool autoconf automake

        - 对于 homebrew::

            brew install gnu-sed gawk binutils gperftools gettext wget help2man libtool autoconf automake

- 创建一个文件系统镜像（区分大小写）::

    hdiutil create ~/esp/crosstool.dmg -volname "ctng" -size 10g -fs "Case-sensitive HFS+"

- 挂载::

    hdiutil mount ~/esp/crosstool.dmg

- 创建指向您工作目录的符号链接::

    mkdir -p ~/esp
    ln -s /Volumes/ctng ~/esp/ctng-volume

- 前往新创建的目录 ::

    cd ~/esp/ctng-volume

- 下载并编译 ``crosstool-NG`` ：

    .. include:: /_build/inc/scratch-build-code.inc

- 编译工具链::

    ./ct-ng xtensa-esp32-elf
    ./ct-ng build
    chmod -R u+w builds/xtensa-esp32-elf

编译得到的工具链会被保存到 ``~/esp/ctng-volume/crosstool-NG/builds/xtensa-esp32-elf``。请按照 :ref:`标准设置指南 <setup-macos-toolchain-add-it-to-path-legacy>` 的介绍，将工具链添加到 ``PATH``。


后续步骤
========

继续设置开发环境，请前往 :ref:`get-started-get-esp-idf-legacy` 章节。
