**********************************************
MacOS 平台工具链的标准设置
**********************************************

:link_to_translation:`en:[English]`

安装准备
=====================

ESP-IDF 将使用 macOS 上默认安装的 Python 版本。

- 安装 pip::

    sudo easy_install pip

- 安装 CMake 和 Ninja 编译工具：

    - 若有 HomeBrew_，您可以运行::

        brew install cmake ninja dfu-util

    - 若有 MacPorts_，您可以运行::

        sudo port install cmake ninja dfu-util

    - 若以上均不适用，请访问 CMake_ 和 Ninja_ 主页，查询有关 macOS 平台的下载安装问题。

- 强烈建议同时安装 ccache_ 以获得更快的编译速度。如有 HomeBrew_，可通过 MacPorts_ 上的 ``brew install ccache`` 或 ``sudo port install ccache`` 完成安装。

.. note::
    如您在上述任何步骤中遇到以下错误::

    ``xcrun: error: invalid active developer path (/Library/Developer/CommandLineTools), missing xcrun at:/Library/Developer/CommandLineTools/usr/bin/xcrun``

    则必须安装 XCode 命令行工具，具体可运行 ``xcode-select --install``。

安装 Python 3
---------------------------------------------

`Catalina 10.15 发布说明`_ 中表示不推荐使用 Python 2.7 版本，在未来的 macOS 版本中也不会默认包含 Python 2.7。执行以下命令来检查您当前使用的 Python 版本::

  python --version

如果输出结果是 ``Python 2.7.17``，则代表您的默认解析器是 Python 2.7。这时需要您运行以下命令检查电脑上是否已经安装过 Python 3::

  python3 --version

如果运行上述命令出现错误，则代表电脑上没有安装 Python 3。

请根据以下步骤安装 Python 3：

  - 使用 HomeBrew_ 进行安装的方法如下::

      brew install python3

  - 使用 MacPorts_ 进行安装的方法如下::

      sudo port install python38

停用 Python 2
====================

Python 2 已经 `结束生命周期 <https://www.python.org/doc/sunset-python-2/>`_，ESP-IDF 很快将不再支持 Python 2。请安装 Python 3.6 或以上版本。可参考上面列出的 macOS 安装说明。

后续步骤
==========

继续设置开发环境，请前往 :ref:`get-started-get-esp-idf` 章节。


.. _cmake: https://cmake.org/
.. _ninja: https://ninja-build.org/
.. _ccache: https://ccache.samba.org/
.. _homebrew: https://brew.sh/
.. _MacPorts: https://www.macports.org/install.php
.. _Catalina 10.15 发布说明: https://developer.apple.com/documentation/macos-release-notes/macos-catalina-10_15-release-notes
