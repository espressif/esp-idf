**********************************************
MacOS 平台工具链的标准设置
**********************************************

:link_to_translation:`en:[English]`

安装准备
=====================

ESP-IDF 将使用 Mac OS 上默认安装的 Python 版本。

- 安装 pip::

    sudo easy_install pip

- 安装 pyserial::

    pip install --user pyserial

- 安装 CMake 和 Ninja 编译工具：

    - 若有 HomeBrew_，您可以运行::

        brew install cmake ninja

    - 若有 MacPorts_，您可以运行::

        sudo port install cmake ninja

    - 若以上均不适用，请访问 CMake_ 和 Ninja_ 主页，查询有关 Mac OS 平台的下载安装问题。

- 强烈建议同时安装 ccache_ 以获得更快的编译速度。如有 HomeBrew_，可通过 MacPorts_ 上的 ``brew install ccache`` 或 ``sudo port install ccache`` 完成安装。

.. note::
    
    如您在上述任何步骤中遇到以下错误::

    ``xcrun: error: invalid active developer path (/Library/Developer/CommandLineTools), missing xcrun at:/Library/Developer/CommandLineTools/usr/bin/xcrun``

    则必须安装 XCode 命令行工具，具体可运行 ``xcode-select --install``。

后续步骤
==========

继续设置开发环境，请前往 :ref:`get-started-get-esp-idf` 章节。

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

