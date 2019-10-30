********************************************
Linux 平台工具链的标准设置（传统 GNU Make）
********************************************
:link_to_translation:`en:[English]`

.. include:: ../gnu-make-legacy.rst

安装准备
========

编译 ESP-IDF 需要以下软件包：

- CentOS 7::

    sudo yum install gcc git wget make flex bison gperf python python2-cryptography

- Ubuntu and Debian::

    sudo apt-get install gcc git wget make flex bison gperf python python-pip python-setuptools python-serial python-cryptography python-future python-pyparsing python-pyelftools

- Arch::

    sudo pacman -S --needed gcc git make flex bison gperf python2-pyserial python2-cryptography python2-future python2-pyparsing python2-pyelftools

.. note::

    一些旧的（2014年之前）Linux 发行版中使用的 ``pyserial`` 版本可能是 2.x ，ESP-IDF 并不支持。这种情况下，请按照 :ref:`get-started-get-packages-legacy` 章节的介绍，使用 ``pip`` 安装软件包。

工具链设置
==========

.. include:: /_build/inc/download-links.inc

Linux 版的 ESP32 工具链可以从 Espressif 的网站下载：

- 64 位 Linux：

    |download_link_linux64|

- 32 位 Linux：

    |download_link_linux32|

1. 下载压缩文件之后，解压到 ``~/esp`` 目录中:

    - 64 位 Linux：

    .. include:: /_build/inc/unpack-code-linux64.inc

    - 32 位 Linux：

    .. include:: /_build/inc/unpack-code-linux32.inc

.. _setup-linux-toolchain-add-it-to-path-legacy:

2. 工具链将被解压到 ``~/esp/xtensa-esp32-elf/`` 路径下。

   为了正常使用工具链，您必须更新 ``~/.profile`` 文件中的 ``PATH`` 环境变量。此外，您还可以在 ``~/.profile`` 文件中增加以下代码，这样一来，所有终端窗口均可以使用 ``xtensa-esp32-elf``::

        export PATH="$HOME/esp/xtensa-esp32-elf/bin:$PATH"

   或者，您可以为上述命令创建一个别名。这样，您只有在需要时才可以使用工具链。如需设置别名，请将以下代码增加至您的 ``〜/ .profile`` 文件中::

        alias get_esp32='export PATH="$HOME/esp/xtensa-esp32-elf/bin:$PATH"'

   这样，您可以在终端输入 ``get_esp32`` 命令将工具链添加至您的 ``PATH``，从而使用工具链。

   .. note::
    
    如果您已将 ``/bin/bash`` 设置为登录 shell，且同时存在 ``.bash_profile`` 和 ``.profile``，则请更新 ``.bash_profile`` 。在 CentOS 环境下, ``alias`` 需要添加到 ``.bashrc`` 文件中。

3. 退出并重新登录以使 ``.profile`` 更改生效。运行以下命令来检查 ``PATH`` 设置是否正确 ::

        printenv PATH

   检查字符串的开头是否包含类似的工具链路径::

    $ printenv PATH
    /home/user-name/esp/xtensa-esp32-elf/bin:/home/user-name/bin:/home/user-name/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin

   这里，您需要将  ``/home/user-name`` 替换为您的安装主目录。


权限问题 /dev/ttyUSB0
------------------------------

某些 Linux 版本可能在烧写 ESP32 时会出现 ``Failed to open port /dev/ttyUSB0`` 错误消息，这可以通过 :ref:`将当前用户添加到 dialout 组 <linux-dialout-group-legacy>` 来解决。


后续步骤
==========

继续设置开发环境，请前往 :ref:`get-started-get-esp-idf-legacy` 章节。


相关文档
=================

.. toctree::
    :maxdepth: 1

    linux-setup-scratch


.. _AUR: https://wiki.archlinux.org/index.php/Arch_User_Repository

