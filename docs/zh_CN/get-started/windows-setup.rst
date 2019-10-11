**********************************************************
Windows 平台工具链的标准设置
**********************************************************

:link_to_translation:`en:[英文]`

.. note::
      基于 CMake 的构建系统仅支持 64 位版本 Windows。

引言
============

ESP-IDF 需要安装必要的工具，以编译 ESP32 固件，包括：Git，交叉编译器，以及 CMake 构建工具。本文将对这些工具一一说明。

在此入门指南中，我们通过命令提示符进行有关操作。不过，安装 ESP-IDF 后你还可以使用 :doc:`Eclipse <eclipse-setup>` 或支持 CMake 的图形化工具 IDE。

https://dl.espressif.com/dl/esp32_win32_msys2_environment_and_toolchain-20190611.zip

ESP-IDF 工具安装器
=======================

安装 ESP-IDF 必备工具最简易的方式是下载 ESP-IDF 工具安装器，地址如下：

https://dl.espressif.com/dl/esp-idf-tools-setup-1.2.exe

安装器会自动安装 ESP32 Xtensa gcc 工具链，Ninja_ 编译工具，以及名为 mconf-idf_ 的配置工具。此外，如果你的电脑还未安装有关 CMake_ 和 Python_ 2.7 的安装器，它还可以下载和运行与之对应的安装器。

安装器默认更新 Windows ``Path`` 环境变量，因而上述工具也可在其他环境中运行。如果禁止该选项，则需自行设置 ESP-IDF 所使用的环境（终端或所选 IDE），并配置正确的路径。 

请注意，此安装器仅针对 ESP-IDF 工具包，并不包括 ESP-IDF。

安装 Git
==============

ESP-IDF 工具安装器并不会安装 Git，因为快速入门指南默认你将以命令行的模式使用它。你可以通过 `Git For Windows`_ 下载和安装 Windows 平台的命令行 Git 工具（包括 "Git Bash" 终端）。

如果你想使用其他图形化 Git 客户端，如 `Github Desktop`， 你可以自行安装，但需要对本《入门指南》中相应的 Git 命令进行转换，以便用于你所选的 Git 客户端。

使用终端
================

在本《入门指南》接下来的步骤说明中，我们将使用终端命令提示符进行有关操作。你也可以使用任何其他形式的命令提示符：

- 比如，Windows 开始菜单下内置的命令提示符。本文档中的所有 Windows 命令行指令均为 Windows 命令提示符中所使用的 "batch" 命令。
- 你还可以使用 `Git for Windows`_ 中的 "Git Bash" 终端，其所使用的 "bash" 命令提示符语法与 Mac OS 或 Linux 的既定语法相同。安装此终端后，你可以在开始菜单下找到命令提示符窗口。
- 如果你已安装 MSYS2_ （通过 ESP-IDF 之前版本），你还可以使用 MSYS 终端。

后续步骤
==========

要继续设置开发环境，请参照 :ref:`get-started-get-esp-idf`。

相关文档
=================

想要自定义安装流程的高阶用户可参照：

.. toctree::
    :maxdepth: 1

    windows-setup-scratch


.. _MSYS2: https://msys2.github.io/
.. _cmake: https://cmake.org/download/
.. _ninja: https://ninja-build.org/
.. _Python: https://www.python.org/downloads/windows/
.. _Git for Windows: https://gitforwindows.org/
.. _mconf-idf: https://github.com/espressif/kconfig-frontends/releases/
.. _Github Desktop: https://desktop.github.com/
