在用户配置文件中添加 IDF_PATH 和 idf.py PATH
==========================================================================================================

:link_to_translation:`en:[英文]`

使用基于 CMake 的构建系统和 idf.py 工具，用户需修改两处系统环境变量：

- ``IDF_PATH`` 需设置为含有 ESP-IDF 目录的路径
- 系统 ``PATH`` 变量需包括含有 ``idf.py`` 工具 （属于 ESP-IDF 一部分）的目录

为确保系统重启后仍保存之前的变量设置，请参照以下说明将变量设置添加到用户配置文件中。

.. note:: 使用 IDE 工具的情况下，你可以选择在 IDE 项目环境中设置环境变量，而不使用如下命令行。

.. note:: 如果你从未用过 ``idf.py`` 命令行工具，而是直接运行 cmake 或通过 IDE 工具运行 cmake，则无需设置 ``PATH`` 变量，只需设置 ``IDF_PATH`` 变量。不过，你也可以两个都设置。

.. note:: 如果你只用过 ``idf.py`` 命令行工具，从未直接运行 cmake 或通过 IDE 工具运行 cmake，则无需设置 ``IDF_PATH`` 变量。``idf.py`` 会搜索自身包含的目录，如果没有发现 ``IDF_PATH``，则会自行进行有关设置。

.. _add-paths-to-profile-windows:

Windows 操作系统
-----------------------------------

在 Windows 10 操作系统下设置环境变量，用户应在开始菜单下搜索 "Edit Environment Variables"。

在较早版本的 Windows 操作系统下设置环境变量，用户应打开系统控制面板，选择“高级”，找到环境变量按钮。

你可以为本台电脑上的“所有用户”或“当前用户”设置环境变量，这取决于其他用户是否也需要使用 ESP-IDF。

- 点击 ``New...`` （新建...） 添加名为 ``IDF_PATH`` 的新系统变量，具体设置为包含 ESP-IDF 的目录，例如，``C:\Users\user-name\esp\esp-idf``。
- 找到 ``Path`` 环境变量，双击进行编辑。在末尾添加 ``;%IDF_PATH%\tools``，这样你就可以通过 Windows 命令窗口运行 ``idf.py`` 等其他工具了。

如果你在安装 ESP32 硬件开发的软件环境时，从 :ref:`get-started-setup-path` 小节跳到了这里，请返回 :ref:`get-started-start-project` 小节开始阅读。


.. _add-idf_path-to-profile-linux-macos:

Linux 和 MacOS 操作系统
------------------------------------

要设置 ``IDF_PATH``，并在 PATH 中添加 ``idf.py``，请将以下两行代码添加至你的 ``~/.profile`` 文件中::

    export IDF_PATH=~/esp/esp-idf
    export PATH="$IDF_PATH/tools:$PATH"

.. note::

   ``~/.profile`` 表示在你的电脑用户主目录中，后缀为 ``.profile`` 的文件。(``~`` 为 shell 中的缩写)。

请退出，并重新登录使更改生效。

.. note::

    并非所有 shell 都使用 ``.profile``，但是如果同时存在 ``/bin/bash`` 和 ``.bash_profile``，请更新此配置文件。如果存在 ``zsh``，请更新 ``.zprofile``。其他 shell 可能使用其他配置文件（详询有关 shell 的文档）。

运行以下命令来检查 ``IDF_PATH`` 设置是否正确::

    printenv IDF_PATH

此处应打印出此前在 ``~/.profile`` 文件中输入（或手动设置）的路径。

为确认 ``idf.py`` 目前是否在 ``PATH`` 中，你可以运行以下命令::

  which idf.py

这里，应打印出类似 ``${IDF_PATH}/tools/idf.py`` 的路径。

如果不想修改 ``IDF_PATH`` 或 ``PATH``，你可以在每次重启或退出后在终端中手动输入::

    export IDF_PATH=~/esp/esp-idf
    export PATH="$IDF_PATH/tools:$PATH"

如果你在安装 ESP32 硬件开发的软件环境时，从 :ref:`get-started-setup-path` 小节跳到了这里，请返回 :ref:`get-started-start-project` 小节开始阅读。
