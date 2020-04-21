*********************************************
Linux 平台工具链的标准设置
*********************************************

:link_to_translation:`en:[English]`

安装准备
========

编译 ESP-IDF 需要以下软件包：

- CentOS 7::

	sudo yum install git wget ncurses-devel flex bison gperf python cmake ninja-build ccache

- Ubuntu and Debian::

	sudo apt-get install git wget libncurses-dev flex bison gperf python python-pip python-setuptools cmake ninja-build ccache libffi-dev libssl-dev

- Arch::

	sudo pacman -S --needed gcc git make ncurses flex bison gperf python-pip cmake ninja ccache

.. note::
	
	使用 ESP-IDF 需要 CMake 3.5 或以上版本。较早版本的 Linux 可能需要升级才能向后移植仓库，或安装 "cmake3" 软件包，而不是安装 "cmake"。

其他提示
========

权限问题 /dev/ttyUSB0
----------------------

使用某些 Linux 版本向 ESP32 烧写固件时，可能会出现 ``Failed to open port /dev/ttyUSB0`` 错误消息。此时，可以将当前用户增加至 :ref:` Linux Dialout 组 <linux-dialout-group>`。

Arch Linux 用户
----------------

在 Arch Linux 中运行预编译 gdb (xtensa-esp32-elf-gdb) 需要 ncurses 5，但 Arch 会使用 ncurses 6。

不过，AUR_ 中有针对原生和 lib32 配置的向下兼容库：

- https://aur.archlinux.org/packages/ncurses5-compat-libs/
- https://aur.archlinux.org/packages/lib32-ncurses5-compat-libs/

在安装这些软件包之前，您可能需要将作者的公钥添加到您的密钥环中，具体参考上方的“注释”部分。

此外，您也可以使用 crosstool-NG 编译一个链接到 ncurses 6 的 gdb。

设置 Ubuntu 和 Debian 默认使用 Python 3
------------------------------------------------

目前，Ubuntu 和 Debian 仍使用 Python 2.7 为默认编译器。Python3 可通过以下方式安装::

	sudo apt-get install python3 python3-pip python3-setuptools

运行以下指令，设置 Python 3 为默认编译器::

	sudo update-alternatives --install /usr/bin/python python /usr/bin/python3 10

.. note::
	
	此改动将影响系统中的所有应用。

后续步骤
========

继续设置开发环境，请前往 :ref:`get-started-get-esp-idf` 章节。


相关文档
========

.. toctree::
	:maxdepth: 1

	linux-setup-scratch


.. _AUR: https://wiki.archlinux.org/index.php/Arch_User_Repository
