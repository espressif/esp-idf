******************************************************************
从零开始设置 Windows 环境下的工具链
******************************************************************

:link_to_translation:`en:[英文]`

本文就如何运行基于 CMake 构建系统中的 :doc:`ESP-IDF 工具安装器 <windows-setup>` 进行逐步详细说明。手动安装所有工具能更好地控制整个安装流程，同时也方便高阶用户进行自定义安装。

使用 ESP-IDF 工具安装器对工具链及其他工具进行快速标准设置，请参照 :doc:`windows-setup`。


.. note::
   基于 GNU Make 的构建系统要求 Windows 兼容 `MSYS2`_ Unix。基于 CMake 的构建系统则无此要求。

工具
=====

cmake
^^^^^

下载最新发布的 Windows 平台稳定版 `CMake`_，并运行安装器。

当安装器询问安装选项时，选择 "Add CMake to the system PATH for all users"（为所有用户的系统路径添加 CMake）或 "Add CMake to the system PATH for the current user"（为当前用户的系统路径添加 CMake）。

Ninja 编译工具
^^^^^^^^^^^^^^^^^^^^

.. note::
    Ninja 目前仅为 64 位版本 Windows 提供 bin 文件。你也可以通过其他编译工具使用 CMake 和 ``idf.py``，如适用于 32 位 Windows 的 mingw-make，但是目前暂无关于此工具的说明文档。

从（`下载页面 <ninja-dl>`_）下载最新发布的 Windows 平台稳定版 `ninja`_。 

适用于 Windows 平台的 Ninja 下载文件是一个 .zip 文件，包含一个 ``ninja.exe`` 文件。将其解压到目录，并 `添加到你的路径 <add-directory-windows-path>`_ （或者选择你的路径中已有的目录）。


Python 2.x
^^^^^^^^^^

下载并运行适用于 Windows 安装器的最新版 `Python`_ 2.7。 

Python 安装的“自定义”那一步提供了一份选项列表，最后一个选项是 "Add python.exe to Path"（添加 python.exe 到路径中），更改该选项，选择 "Will be installed"（将会安装）。

Python 安装完成后，打开 Windows 开始菜单下的 Command Prompt，并运行以下命令::

  pip install --user pyserial

适用于 IDF 的 MConf
^^^^^^^^^^^^^^^^^^^^^^

从 `kconfig-frontends 发布页面 <mconf-idf>`_ 下载配置工具 mconf-idf。此为 ``mconf`` 配置工具，可针对 ESP-IDF 进行一些自定义操作。

你需将此工具解压到目录，然后 `添加到你的路径 <add-directory-windows-path>`_。

工具链设置
===============

.. include:: /_build/inc/download-links.inc

下载预编译的 Windows 平台工具链：

|download_link_win32|

解压压缩包文件到 ``C:\Program Files`` （或其他地址）。压缩包文件包含 ``xtensa-esp32-elf`` 目录。

然后，须将该目录下的子目录 ``bin`` `添加到你的路径 <add-directory-windows-path>`_。例如，``C:\Program Files\xtensa-esp32-elf\bin``。

.. note::

   如果你已安装 MSYS2 环境（适用 "GNU Make" 构建系统），你可以跳过下载那一步，直接添加目录 ``C:\msys32\opt\xtensa-esp32-elf\bin`` 到路径，因为 MSYS2 环境已包含工具链。


.. _add-directory-windows-path:

添加目录到路径
========================

添加任何新目录到你的 Windows Path 环境变量：

打开系统控制面板，找到环境变量对话框（对于 Windows 10，则在高级系统设置中查找对话框）。

双击 ``Path`` 变量（选择用户或系统路径，这取决于你是否希望其他用户路径中也存在该目录)。在最后数值那里新添 ``;<new value>``。


后续步骤
================

要继续设置开发环境，请参照 :ref:`get-started-get-esp-idf`。

.. _ninja: https://ninja-build.org/
.. _Python: https://www.python.org/downloads/windows/
.. _MSYS2: https://msys2.github.io/

