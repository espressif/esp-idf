*****************************
Linux 平台工具链的标准设置
*****************************

安装前提
=====================

编译 ESP-IDF 需要以下软件包：

- CentOS 7::

    sudo yum install git wget make ncurses-devel flex bison gperf python pyserial

- Ubuntu and Debian::

    sudo apt-get install git wget make libncurses-dev flex bison gperf python python-serial

- Arch::

    sudo pacman -S --needed gcc git make ncurses flex bison gperf python2-pyserial

工具链的设置
===============

Linux 版的 ESP32 工具链可以从 Espressif 的网站下载：

- 64-bit Linux：

  https://dl.espressif.com/dl/xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz

- 32-bit Linux：

  https://dl.espressif.com/dl/xtensa-esp32-elf-linux32-1.22.0-80-g6c4433a-5.2.0.tar.gz

1. 下载完成后，将它解压到 ``~/esp`` 目录： ::

        mkdir -p ~/esp
        cd ~/esp
        tar -xzf ~/Downloads/xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz

.. _setup-linux-toolchain-add-it-to-path-cn:  

2. 工具链将会被解压到 ``~/esp/xtensa-esp32-elf/`` 目录。

   要使用工具链，你还需要在 ``~/.profile`` 文件中更新环境变量 ``PATH``。要使 ``xtensa-esp32-elf`` 在所有的终端会话中都有效，需要将下面这一行代码添加到你的 ``~/.profile`` 文件中： ::

    export PATH="$PATH:$HOME/esp/xtensa-esp32-elf/bin"

   或者你也可以给上面的命令创建一个别名。这样做的好处是，你只在需要使用它的时候才获取工具链。将下面这行代码添加到 ``~/.profile`` 文件中即可： ::

    alias get_esp32='export PATH="$PATH:$HOME/esp/xtensa-esp32-elf/bin"'

   然后，当你需要使用工具链时，在命令行输入 ``get_esp32``，然后工具链会自动添加到你的 ``PATH`` 中。

    .. note::
    
        如果将 ``/bin/bash`` 设置为登录 shell，且同时存在 ``.bash_profile`` 和 ``.profile``，则更新 ``.bash_profile`` 。
    
3. 退出并重新登录以使 ``.profile`` 更改生效。 运行以下命令来检查 ``PATH`` 设置是否正确： ::

        printenv PATH

    检查一下字符串的末尾是否包含类似的工具链的路径： ::

        $ printenv PATH
        /home/user-name/bin:/home/user-name/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin:/home/user-name/esp/xtensa-esp32-elf/bin

     除了 ``/home/user-name``，应该有具体的安装的主路径。

权限问题 /dev/ttyUSB0
------------------------------

某些 Linux 版本可能在烧写 ESP32 时会出现 ``Failed to open port /dev/ttyUSB0`` 错误消息。 :ref:`可以通过将当前用户添加到拨出组来解决<linux-dialout-group>`。

Arch Linux 用户
----------------

在 Arch 中运行预编译的 gdb (xtensa-esp32-elf-gdb) 需要 ncurses 5，但是 Arch 使用的是 ncurses 6。在 AUR_ 中向下兼容的库文件，可用于本地和 lib32 的配置：

- https://aur.archlinux.org/packages/ncurses5-compat-libs/
- https://aur.archlinux.org/packages/lib32-ncurses5-compat-libs/

在安装这些软件包之前，你可能需要将作者的公钥添加到你的钥匙圈中，上面链接中的“Comments”部分有所叙述。

或者，你也可以使用 crosstool-NG 编译一个链接 ncurses 6 的 gdb。

后续步骤
==========

要继续设置开发环境，请参考 :ref:`get-started-get-esp-idf-cn` 一节。


.. _AUR: https://wiki.archlinux.org/index.php/Arch_User_Repository

