*******************************************************************
Linux 平台工具链的标准设置 (CMake)
*******************************************************************

:link_to_translation:`en:[英文]`

.. include:: ../cmake-warning.rst

安装前提
=====================

编译 ESP-IDF 需要以下软件包：

- CentOS 7::

    sudo yum install git wget ncurses-devel flex bison gperf python pyserial cmake ninja-build ccache

- Ubuntu 和 Debian::

    sudo apt-get install git wget libncurses-dev flex bison gperf python python-pip python-setuptools python-serial python-cryptography python-future python-pyparsing cmake ninja-build ccache

- Arch::

    sudo pacman -S --needed gcc git make ncurses flex bison gperf python2-pyserial python2-cryptography python2-future python2-pyparsing cmake ninja ccache

.. note::
    使用 ESP-IDF 需要 CMake 3.5 或以上版本。较早版本的 Linux 可能需要升级才能向后移植仓库，或安装 "cmake3" 软件包，而不是安装 "cmake"。

工具链的设置
=========================

.. include:: /_build/inc/download-links.inc

Linux 版的 ESP32 工具链可以从 Espressif 的网站下载：

- 64 位 Linux：

  |download_link_linux64|

- 32 位 Linux：

  |download_link_linux32|

1.  下载完成后，将它解压到 ``~/esp`` 目录：

    - for 64-bit Linux:

      .. include:: /_build/inc/unpack-code-linux64.inc

    - for 32-bit Linux:

      .. include:: /_build/inc/unpack-code-linux32.inc

.. _setup-linux-toolchain-add-it-to-path-cmake:

2.  工具链将会被解压到 ``~/esp/xtensa-esp32-elf/`` 目录。

    要使用工具链，你还需要在 ``~/.profile`` 文件中更新环境变量 ``PATH``。要使 ``xtensa-esp32-elf`` 在所有的终端会话中都有效，需要将下面这一行代码添加到你的 ``~/.profile`` 文件中：::

      export PATH="$HOME/esp/xtensa-esp32-elf/bin:$PATH"

    或者，你也可以给上面的命令创建一个别名。这样做的好处是，你仅在需要时才获取工具链，将下面这行代码添加到 ``~/.profile`` 文件中即可::

      alias get_esp32='export PATH="$HOME/esp/xtensa-esp32-elf/bin:$PATH"'

    然后，当你需要使用工具链时，在命令行输入 ``get_esp32``，然后工具链会自动添加到你的 ``PATH`` 中。

    .. note::

        如果将 ``/bin/bash`` 设置为登录 shell，且同时存在  ``.bash_profile`` 和 ``.profile``，则更新 ``.bash_profile``。

3.  退出并重新登录以使 ``.profile`` 更改生效。运行以下命令来检查 ``PATH`` 设置是否正确：::

        printenv PATH

    检查字符串的开头是否包含类似的工具链路径::

        $ printenv PATH
        /home/user-name/esp/xtensa-esp32-elf/bin:/home/user-name/bin:/home/user-name/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin

     除了 ``/home/user-name``，应该有具体的安装的主路径。


权限问题 /dev/ttyUSB0
----------------------------------------------

使用某些 Linux 版本向 ESP32 烧写固件时，可能会出现 ``Failed to open port /dev/ttyUSB0`` 错误消息。此时，可以将当前用户增加至 :ref:` Linux Dialout 组 <linux-dialout-group-cmake>`

Arch Linux 用户
--------------------------------

在 Arch Linux 中运行预编译的 gdb (xtensa-esp32-elf-gdb) 需要 ncurses 5，但是 Arch 使用的是 ncurses 6。

`AUR`_ 中存在向下兼容的库文件，可用于本地和 lib32 的配置：

- https://aur.archlinux.org/packages/ncurses5-compat-libs/
- https://aur.archlinux.org/packages/lib32-ncurses5-compat-libs/

在安装这些软件包之前，你可能需要将作者的公钥添加到你的密钥环中，具体见上方链接中的 "Comments" 部分的介绍。

或者，你也可以使用 crosstool-NG 编译一个链接到 ncurses 6 的 gdb。


后续步骤
================

后续开发环境设置，请参考 :ref:`get-started-get-esp-idf-cmake` 一节。


相关文档
=================

.. toctree::
    :maxdepth: 1

    linux-setup-scratch


.. _AUR: https://wiki.archlinux.org/index.php/Arch_User_Repository
