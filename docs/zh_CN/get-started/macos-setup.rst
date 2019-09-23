******************************************************************
在 Mac OS 上安装 ESP32 工具链
******************************************************************

:link_to_translation:`en:[英文]`

安装准备
=====================

ESP-IDF 将使用 Mac OS 上默认安装的 Python 版本。

- 安装 pip::

    sudo easy_install pip

- 安装 pyserial::

    pip install --user pyserial

- 安装 CMake 和 Ninja 编译工具：

  - 若有 HomeBrew_，你可以运行::

      brew install cmake ninja

  - 若有 MacPorts_，你可以运行::

      sudo port install cmake ninja

  - 若以上均不适用，请访问 CMake_ 和 Ninja_ 主页，查询有关 Mac OS 平台的下载安装问题。 

- 强烈建议同时安装 ccache_ 以达到更快的编译速度。如有 HomeBrew_，可通过 MacPorts_ 上的 ``brew install ccache`` 或 ``sudo port install ccache`` 完成安装。

.. note::

   如在任一步骤中出现以下报错信息::

   ``xcrun: error: invalid active developer path (/Library/Developer/CommandLineTools), missing xcrun at: /Library/Developer/CommandLineTools/usr/bin/xcrun``

   你需要安装 XCode 命令行工具才能继续，具体可运行 ``xcode-select --install`` 进行安装。

安装工具链
======================

.. include:: /_build/inc/download-links.inc

下载 MacOS 版本的 ESP32 工具链，请前往乐鑫官网：

|download_link_osx|

完成下载后，请在 ``~/esp`` 目录下进行解压：

.. include:: /_build/inc/unpack-code-osx.inc

.. _setup-macos-toolchain-add-it-to-path:

此后，该工具链将解压至 ``~/esp/xtensa-esp32-elf/`` 目录。

为了开始使用工具链，你必须更新 ``~/.profile`` 文件中的 ``PATH`` 环境变量。为了让所有终端都可以使用 ``xtensa-esp32-elf``，请将下方命令增加至你的 ``~/.profile`` 文件：::

    export PATH=$HOME/esp/xtensa-esp32-elf/bin:$PATH

此外，你可以为以上命令增加一个别名。这样，你就可以仅在有需要时获取工具链。具体方式是在 ``~/.profile`` 文件中增加下方命令::

    alias get_esp32="export PATH=$HOME/esp/xtensa-esp32-elf/bin:$PATH"

此时，你可以直接输入 ``get_esp32`` 命令，即可将工具链添加至你的 ``PATH``。

注意，这里需要退出并重新登陆，``.profile`` 更改才会生效。

此外，你可以使用以下命令，验证 ``PATH`` 是否设置正确::

        printenv PATH


后续步骤
=================

前往 :ref:`get-started-get-esp-idf`，完成接下来的开发环境配置。

相关文档
=================

.. toctree::
    :maxdepth: 1

    macos-setup-scratch

.. _cmake: https://cmake.org/
.. _ninja: https://ninja-build.org/
.. _ccache: https://ccache.samba.org/
.. _homebrew: https://brew.sh/
.. _MacPorts: https://www.macports.org/install.php
