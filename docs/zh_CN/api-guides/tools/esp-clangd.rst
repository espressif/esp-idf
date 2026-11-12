**********************************
在 IDF 应用程序中使用 clangd
**********************************

:link_to_translation:`en:[English]`

clangd 是一款便捷的语言服务器，可在集成开发环境 (IDE) 和编辑器中导航与补全 C 和 C++ 代码。本文介绍如何在编辑器中使用 clangd 进行交叉编译与嵌入式开发。ESP-Clangd 是对 clangd 做了少量补丁的版本，该补丁增加了对自定义配置文件的支持，详见下文 `ESP-Clangd 自定义配置`_。

针对嵌入式项目的调优
============================

在 PC 上进行本地开发时，通常无需任何额外配置。然而，对于嵌入式与交叉编译项目，clangd 往往无法即装即用。通常需要根据项目、编译器和目标进行特殊配置。

下面这些设置对 IDF 应用程序代码以及一般的嵌入式项目都很重要。“重要”提示框中包含对 IDF 代码的推荐设置。

编译数据库
--------------------

clangd 通过编译数据库 ``compile_commands.json`` 获取处理项目代码所需的信息。它会从其中的 ``command`` 条目提取编译器路径、目标三元组以及其他信息。构建系统可以生成 ``compile_commands.json``。clangd 会在默认位置查找该文件。（详见 `Project setup <https://clangd.llvm.org/installation#project-setup>`__。）

否则，需要通过 ``CompileFlags: CompilationDatabase`` 设置指定包含该数据库的目录。（详见 `CompilationDatabase <https://clangd.llvm.org/config.html#compilationdatabase>`__。``None`` 与 ``Ancestors`` 为关键词。）

.. important::

    IDF 应用程序使用 CMake 构建，后者会在 ``build/`` 目录 (``<project>/build/compile_commands.json``) 中生成编译数据库。这是 clangd 的默认位置，因此无需额外配置。

多个 ``compile_commands.json`` 数据库
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

某个项目或构建系统可能会为不同的子目录使用多个 ``compile_commands.json``，例如用于协处理器代码。此时，请在项目配置文件 ``.clangd`` 中添加片段 (Fragment)，使用 ``If: PathMatch`` 来匹配不同路径，并为每个片段分别设置 ``CompileFlags: CompilationDatabase``。（详见 `Loading and combining fragments <https://clangd.llvm.org/config.html#loading-and-combining-fragments>`__。）

.. important::

    对于 IDF 应用程序中的 ULP 协处理器 C 代码，新建 ``<project>/.clangd`` 文件，并写入如下设置：

    .. code-block:: yaml

        ---
        If:
          PathMatch: main/ulp/.*
        CompileFlags:
          CompilationDatabase: build/esp-idf/main/ulp_main

    ``---`` 是片段 (Fragment) 语法的一部分。路径均相对于 ``.clangd`` 文件所在位置。``main/ulp/`` 目录包含 ULP 协处理器的 C 代码。``build/esp-idf/main/ulp_main`` 目录包含构建系统为 ULP 代码生成的 ``compile_commands.json`` 文件。

允许外部编译器
--------------------------

开发嵌入式项目时，请在启动 clangd 时请传入 ``--query-driver=...`` 命令行参数。该白名单参数需与 ``compile_commands.json`` 配合使用：它通过 glob 模式匹配 ``command`` 条目中出现的外部编译器，并告知 clangd 哪些编译器可以安全执行。clangd 会运行匹配的编译器以探测其系统包含路径与内置头文件。

.. important::

    对于 IDF 应用程序，与大多数交叉编译项目一样，向 clangd 传入一个简单的参数：

    .. code-block:: yaml

        --query-driver=**


过滤编译器选项
--------------------------

clangd 会使用每个编译数据库条目中 ``command`` 字段的选项来调用其内部编译器前端。构建系统可能会使用 clangd 无法识别的选项。可以通过 ``CompileFlags: Add[]`` 与 ``CompileFlags: Remove[]`` 来过滤这些选项。

.. important::

    默认情况下，IDF 应用程序使用 ``IDF_TOOLCHAIN=gcc``，因此应忽略部分 GCC 特定选项。为简化处理，可移除所有不影响抽象语法树 (AST) 的代码生成选项：

    .. code-block:: yaml

        CompileFlags:
          Remove: [-m*, -f*]

    如果 IDF 应用程序使用 ``IDF_TOOLCHAIN=clang`` 构建，则无需过滤。


内置头文件
----------------

内置头文件（也称资源头文件）由编译器提供，有别于标准库头文件，其中包括 ``stddef.h`` 与 ``stdbool.h``。

clangd 自带一套默认的内置头文件，但为了更好地匹配目标芯片，建议改用交叉编译工具链提供的内置头文件。请设置 ``CompileFlags: BuiltinHeaders: QueryDriver``，使 clangd 能依据编译数据库定位这些头文件。（详见 `BuiltinHeaders <https://clangd.llvm.org/config.html#builtinheaders>`__。）同时设置合适的 ``--query-driver=`` 参数。

.. important::

    若要在 IDF 应用程序中使用乐鑫工具链提供的内置头文件，请设置：

    .. code-block:: yaml

        CompileFlags:
          BuiltinHeaders: QueryDriver


clangd 配置
====================

可以使用命令行参数、环境变量和配置文件配置 clangd。这几种配置方式支持的选项并不完全相同，但有些设置是通用的。

配置文件有多个作用域。（详见 `Files <https://clangd.llvm.org/config.html#files>`__。）

项目配置文件
--------------------------

可以在源代码树中放置 ``.clangd`` 文件。它适用于同一目录及其子目录中的源文件。clangd 会在已打开文件的所有父目录中搜索 ``.clangd``。

对于需要专属设置的应用程序或库，请使用项目配置。

.. important::

    可以在 :project_file:`tools/clangd-config.yaml` 中找到适用于 IDF 及 IDF 应用程序代码的推荐设置。

用户配置文件
-----------------------

clangd 会将全局（用户）设置应用到打开的每个文件。用户配置路径为：

- ``~/.config/clangd/config.yaml`` (Linux)
- ``C:\Users\user\AppData\Local\clangd\config.yaml`` (Windows)
- ``~/Library/Preferences/clangd/config.yaml`` (macOS)

可以在用户配置中放置通用设置，并使用片段 (Fragment) 与条件 (Condition) 为匹配的路径指定设置。（详见 `If <https://clangd.llvm.org/config.html#if>`__。）

ESP-Clangd 自定义配置
-------------------------------

ESP-Clangd 增加了命令行参数 ``--custom-config-file=<path/to/yaml>``。自定义配置不受打开文件所在位置影响，因此可以按项目分组使用独立设置（例如面向特定芯片的项目），而无需将其混入用户配置。

自定义配置的优先级高于用户与项目配置。诸如 ``CompileFlags: Remove[]`` 这类集合型参数，仍会在各作用域间进行合并。

.. important::

    对于 IDF 应用程序，从编辑器工作区启动 clangd 时，请传入 :project_file:`tools/clangd-config.yaml` 的完整路径：

        <path/to/tools>/esp-clangd/bin/clangd --custom-config-file=<path/to/IDF>/tools/clangd-config.yaml ...

VS Code 示例
===============

ESP-IDF 会在常规的 ESP-IDF 安装管理器 (EIM) 或 ``idf_tools.py`` 安装过程中，连同其他工具一起自动安装 ESP-Clangd。

下面这段 ``<project>/.vscode/settings.json`` 配置示例展示了如何配置 VS Code。请按所用编辑器或 IDE 进行相应调整。

.. code-block:: json

    {
        "clangd.path": "${env:HOME}/.espressif/tools/esp-clangd/esp-22.1.4_20260825/esp-clangd/bin/clangd",
        "clangd.arguments": [
            "--custom-config-file=${env:HOME}/Path/To/IDF/tools/clangd-config.yaml",
            "--query-driver=**",
            "--log=verbose"
        ]
    }

以下为 ESP-IDF 随附的 :project_file:`tools/clangd-config.yaml` 文件：

.. literalinclude:: ../../../../tools/clangd-config.yaml
    :language: yaml
