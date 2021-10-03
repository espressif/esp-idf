***********************************************
从零开始设置 macOS 环境下的工具链
***********************************************

:link_to_translation:`en:[English]`

软件包管理器
===============

从零开始设置工具链，您需要安装 MacPorts_ 或 Homebrew_ 软件包管理器。或者，您也可以直接下载 :doc:`预编译的工具链 <macos-setup>`。

MacPorts 需要完整的 XCode 软件，而 Homebrew 只需要安装 XCode 命令行工具即可。

.. _Homebrew: https://brew.sh/
.. _MacPorts: https://www.macports.org/install.php

请参考 :ref:`工具链自定义设置 <get-started-customized-setup>` 章节，查看可能需要从头开始设置工具链的情况。

安装准备
=====================

- 安装 pip::

	sudo easy_install pip

- 安装 pyserial::

	pip install --user pyserial

- 安装 CMake 和 Ninja 编译工具：

	- 若有 Homebrew，您可以运行::

		brew install cmake ninja dfu-util

	- 若有 MacPorts，您可以运行::

		sudo port install cmake ninja dfu-util

从源代码编译工具链
=================================

安装依赖项：

- 对于 MacPorts::

	sudo port install gsed gawk binutils gperf grep gettext wget libtool autoconf automake make

- 对于 Homebrew::

	brew install gnu-sed gawk binutils gperftools gettext wget help2man libtool autoconf automake make

创建一个文件系统镜像（区分大小写）::

    hdiutil create ~/esp/crosstool.dmg -volname "ctng" -size 10g -fs "Case-sensitive HFS+"

挂载::

	hdiutil mount ~/esp/crosstool.dmg

创建指向您工作目录的符号链接::

    mkdir -p ~/esp
    ln -s /Volumes/ctng ~/esp/ctng-volume

前往新创建的目录::

	cd ~/esp/ctng-volume

下载并编译 ``crosstool-NG``:

.. include-build-file:: inc/scratch-build-code.inc

编译工具链::

	./ct-ng {IDF_TARGET_TOOLCHAIN_PREFIX}
	./ct-ng build
	chmod -R u+w builds/{IDF_TARGET_TOOLCHAIN_PREFIX}

编译得到的工具链会被保存到 ``~/esp/ctng-volume/crosstool-NG/builds/{IDF_TARGET_TOOLCHAIN_PREFIX}``。使用工具链前，请将 ``~/esp/ctng-volume/crosstool-NG/builds/{IDF_TARGET_TOOLCHAIN_PREFIX}/bin`` 添加至 ``PATH`` 环境变量。

后续步骤
==========

请前往 :ref:`get-started-get-esp-idf` 章节继续设置开发环境。