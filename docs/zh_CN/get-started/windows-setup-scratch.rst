********************************************
从零开始设置 Windows 环境下的工具链
********************************************

:link_to_translation:`en:[English]`

除了使用 :doc:`ESP-IDF 工具安装器 <windows-setup>`，用户也可以手动设置 Windows 环境下的工具链，这正是本文的主要内容。手动安装工具可以更好地控制安装流程，同时也方便高阶用户进行自定义安装。

使用 ESP-IDF 工具安装器对工具链及其他工具进行快速标准设置，请参照 :doc:`windows-setup`。

.. note::
    基于 GNU Make 的构建系统要求 Windows 兼容 MSYS2_ Unix，基于 CMake 的构建系统则无此要求。

.. _get-esp-idf-windows-command-line:

获取 ESP-IDF
=================

.. 注解::

    较早版本 ESP-IDF 使用了 **MSYS2 bash 终端命令行**。目前，基于 CMake 的编译系统可使用常见的 **Windows 命令窗口**，即本指南中使用的终端。

    请注意，如果您使用基于 bash 的终端或 PowerShell 终端，一些命令语法将与下面描述有所不同。

打开命令提示符，运行以下命令：

.. include-build-file:: inc/git-clone-windows.inc

ESP-IDF 将下载至 ``%userprofile%\esp\esp-idf``。

请前往 :doc:`/versions`，查看 ESP-IDF 不同版本的具体适用场景。

.. include-build-file:: inc/git-clone-notes.inc

.. 注解::

    在克隆远程仓库时，请加上可选参数 ``--recursive`` 。如果你已经克隆了 ESP-IDF 但没有加上此参数，请运行以下命令获取所有子模块 ::

        cd esp-idf
        git submodule update --init


工具
=====

CMake 工具
^^^^^^^^^^

下载最新发布的 Windows 平台稳定版 `CMake`_，并运行安装器。

当安装器询问“安装选项”时，选择 "Add CMake to the system PATH for all users"（为所有用户的系统路径添加 CMake）或 "Add CMake to the system PATH for the current user"（为当前用户的系统路径添加 CMake）。

Ninja 编译工具
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. 注解::
    目前，Ninja 仅提供支持 64 位 Windows 版本的 bin 文件。您也可以配合其他编译工具（如 mingw-make）在 32 位 Windows 版本中使用 CMake 和 ``idf.py`` 。但是目前暂无关于此工具的说明文档。

请从 `下载页面 <ninja-dl_>`_ 下载最新发布的 Windows 平台稳定版 Ninja_。

适用于 Windows 平台的 Ninja 下载文件是一个 .zip 文件，包含一个 ``ninja.exe`` 文件。您需要将该文件解压到目录，并 :ref:`添加到 Path 环境变量 <add-directory-windows-path>` （或者选择解压到 Path 变量中已有的目录）。


Python
^^^^^^

下载并运行适用于 Windows 安装器的最新版 Python_。

Python 安装器的“自定义”菜单可为您提供一系列选项，最后一项为 "Add python.exe to Path"（添加 python.exe 到 Path 环境变量中），请将该选项更改为 "Will be installed"（将会安装）。

Python 安装完成后，从 Windows 开始菜单中打开“命令提示符”窗口，并运行以下命令::

    pip install --user pyserial

工具链设置
===============

.. include-build-file:: inc/download-links.inc

下载预编译的 Windows 工具链：

|download_link_win32|

将压缩包文件解压到 ``C:\Program Files``（或其他位置）。压缩包文件包含一个 ``{IDF_TARGET_TOOLCHAIN_PREFIX}`` 目录。

然后，请将该目录下的 ``bin`` 子目录 :ref:`添加到 Path 环境变量 <add-directory-windows-path>`。例如，将目录 ``C:\Program Files\{IDF_TARGET_TOOLCHAIN_PREFIX}\bin`` 添加到 Path 环境变量。

.. 注解::
    如果您已安装 MSYS2 环境（适用 "GNU Make" 编译系统），则可以跳过下载那一步，直接添加目录 ``C:\msys32\opt\{IDF_TARGET_TOOLCHAIN_PREFIX}\bin`` 到 Path 环境变量，因为 MSYS2 环境已包含工具链。


.. _add-directory-windows-path:

添加目录到 Path 环境变量
========================

在 Windows 环境下，向 Path 环境变量增加任何新目录，请：

打开系统“控制面板”，找到环境变量对话框（Windows 10 用户请前往“高级系统设置”）。

双击 ``Path`` 变量（选择“用户路径”或“系统路径”，具体取决于您是否希望其他用户的 Path 中也存在该目录)。最后在值的末尾增加 ``;<new value>``。


后续步骤
==========

请前往 :ref:`get-started-set-up-tools` 章节继续设置开发环境。

.. _CMake: https://cmake.org/download/
.. _Ninja: https://ninja-build.org/
.. _ninja-dl: https://github.com/ninja-build/ninja/releases
.. _Python: https://www.python.org/downloads/windows/
.. _MSYS2: https://www.msys2.org/
.. _kconfig-frontends releases page: https://github.com/espressif/kconfig-frontends/releases
.. Note: These two targets may be used from git-clone-notes.inc depending on version, don't remove
.. _Stable version: https://docs.espressif.com/projects/esp-idf/zh_CN/stable/
.. _Releases page: https://github.com/espressif/esp-idf/releases