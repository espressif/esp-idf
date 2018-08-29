**********************************
从零开始设置 Mac OS 环境下的工具链
**********************************

安装必要的工具
=====================

- 安装 pip::

    sudo easy_install pip

.. note::

    ``pip`` 稍后将用于安装 :ref:`必要的 Python 软件包 <get-started-get-packages>`。

从源代码编译工具链
==================

- 安装依赖:

  - 安装 MacPorts_ 或者 homebrew_ 包管理器。MacPorts 需要安装完整的 XCode 软件，但是 homebrew 只需要安装 XCode 命令行工具即可。
    
    .. _homebrew: https://brew.sh/
    .. _MacPorts: https://www.macports.org/install.php

  - 对于 MacPorts::

        sudo port install gsed gawk binutils gperf grep gettext wget libtool autoconf automake

  - 对于 homebrew::

        brew install gnu-sed gawk binutils gperftools gettext wget help2man libtool autoconf automake

创建大小写敏感的文件系统镜像::

    hdiutil create ~/esp/crosstool.dmg -volname "ctng" -size 10g -fs "Case-sensitive HFS+"

挂载::

    hdiutil mount ~/esp/crosstool.dmg

创建指向你工作目录的符号链接::

    cd ~/esp
    ln -s /Volumes/ctng crosstool-NG

下载 ``crosstool-NG`` 然后编译:

.. include:: /_build/inc/scratch-build-code.inc

编译工具链::

    ./ct-ng xtensa-esp32-elf
    ./ct-ng build
    chmod -R u+w builds/xtensa-esp32-elf

编译得到的工具链会被保存到 ``~/esp/crosstool-NG/builds/xtensa-esp32-elf``。根据 :ref:`Mac OS 下设置环境变量的标准方法 <setup-macos-toolchain-add-it-to-path>` 中的介绍，将工具链添加到 ``PATH`` 中。


下一步
======

继续设置开发环境，请前往 :ref:`获取 ESP-IDF <get-started-get-esp-idf>` 章节。
