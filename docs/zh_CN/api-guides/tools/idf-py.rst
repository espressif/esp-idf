IDF 前端工具 - ``idf.py``
**************************************

:link_to_translation:`en:[English]`

``idf.py`` 命令行工具提供了一个前端界面，管理工程构建、工程部署及工程调试等操作。该前端界面使用多项工具，如：

- CMake_ 用于配置要构建的工程。
- Ninja_ 用于构建工程。
- `esptool`_ 用于烧录目标芯片。

:ref:`Windows <get-started-configure>`、 :ref:`Linux 或 macOS 配置项目 <get-started-configure-linux-macos>` 中简要介绍了设置 ``idf.py`` 以配置、构建及烧录工程的操作流程。

.. important::

  ``idf.py`` 应在 ESP-IDF 工程目录下运行，即包含 ``CMakeLists.txt`` 文件的目录。旧版本工程，即包含 ``Makefile`` 的目录，与 ``idf.py`` 不兼容。

常用命令
=============

创建新工程：``create-project``
------------------------------------

.. code-block:: bash

    idf.py create-project <project name>

此命令将创建一个新的 ESP-IDF 工程。此外，使用 ``--path`` 选项可指定工程创建路径。

创建新组件：``create-component``
----------------------------------------

.. code-block:: bash

    idf.py create-component <component name>

此命令将创建一个新的组件，包含构建所需的最基本文件集。使用 ``-C`` 选项可指定组件创建目录。有关组件的更多信息，请参阅 :ref:`组件 CMakeLists 文件 <component-directories>`。

.. _selecting-idf-target:

选择目标芯片：``set-target``
----------------------------------

ESP-IDF 支持多个目标芯片，运行 ``idf.py --list-targets`` 查看当前 ESP-IDF 版本支持的所有目标芯片。

.. code-block:: bash

    idf.py set-target <target>

此命令将设置当前工程的目标芯片。

.. important::

    ``idf.py set-target`` 将清除 build 目录，并重新生成 ``sdkconfig`` 文件，原来的 ``sdkconfig`` 文件保存为 ``sdkconfig.old``。

.. note::

    ``idf.py set-target`` 命令与以下操作效果相同：

    1. 清除 build 目录 (``idf.py fullclean``)
    2. 删除 sdkconfig 文件 (``mv sdkconfig sdkconfig.old``)
    3. 使用新的目标芯片重新配置工程 (``idf.py -DIDF_TARGET=esp32 reconfigure``)

所需的 ``IDF_TARGET`` 还可以作为环境变量（如 ``export IDF_TARGET=esp32s2``）或 CMake 变量（如将 ``-DIDF_TARGET=esp32s2`` 作为 CMake 或 ``idf.py`` 的参数）传递。在经常使用同类芯片的情况下，设置环境变量将使操作更加便利。

要给指定工程设定 ``IDF_TARGET`` 的默认值，请将 ``CONFIG_IDF_TARGET`` 选项添加到该工程的 ``sdkconfig.defaults`` 文件（如 ``CONFIG_IDF_TARGET="esp32s2"``）。若未通过使用环境变量、CMake 变量或 ``idf.py set-target`` 命令等方法指定 ``IDF_TARGET``，则默认使用该选项的值。

若未通过以上任一方法设置目标芯片，构建系统将默认使用 ``esp32``。

启动图形配置工具：``menuconfig``
--------------------------------------------------

.. code-block:: bash

  idf.py menuconfig

构建工程：``build``
------------------------

.. code-block:: bash

  idf.py build

此命令将构建当前目录下的工程，具体步骤如下：

  - 若有需要，创建 build 目录。"build" 子目录可以保存构建输出文件，使用 ``-B`` 选项可改变子目录的路径。
  - 必要时运行 CMake_ 配置工程，并为主要构建工具生成构建文件。
  - 运行主要构建工具（Ninja_ 或 ``GNU Make``）。默认情况下，构建工具会完成自动检测，也可通过将 ``-G`` 选项传递给 ``idf.py`` 来显式设置构建工具。

构建是增量行为，因此若上次构建结束后，源文件或配置并未发生更改，则不会执行任何操作。

此外，使用 ``app``、``bootloader`` 或 ``partition-table`` 参数运行此命令，可选择仅构建应用程序、引导加载程序或分区表。

清除构建输出：``clean``
------------------------------

.. code-block:: bash

  idf.py clean

此命令可清除 build 目录中的构建输出文件，下次构建时，工程将完全重新构建。注意，使用此选项不会删除 build 文件夹内的 CMake 配置输出。

删除所有构建内容：``fullclean``
-------------------------------------------

.. code-block:: bash

  idf.py fullclean

此命令将删除所有 build 目录下的内容，包括 CMake 配置输出。下次构建时，CMake 将重新配置其输出。注意，此命令将递归删除 build 目录下的 *所有* 文件（工程配置将保留），请谨慎使用。

烧录工程：``flash``
------------------------

.. code-block:: bash

  idf.py flash

此命令将在需要时自动构建工程，随后将其烧录到目标芯片。使用 ``-p`` 和 ``-b`` 选项可分别设置串口名称和烧录程序的波特率。

.. note:: 环境变量 ``ESPPORT`` 和 ``ESPBAUD`` 可分别设置 ``-p`` 和 ``-b`` 选项的默认值，在命令行上设置这些选项的参数可覆盖默认值。

``idf.py`` 在内部使用 ``esptool`` 的 ``write-flash`` 命令来烧录目标设备。通过 ``--extra-args`` 选项传递额外的参数，并配置烧录过程。例如，要 `写入到外部 SPI flash 芯片 <https://docs.espressif.com/projects/esptool/en/latest/esptool/advanced-options.html#custom-spi-pin-configuration>`_，请使用以下命令： ``idf.py flash --extra-args="--spi-connection <CLK>,<Q>,<D>,<HD>,<CS>"``。要查看所有可用参数，请运行 ``esptool write-flash --help`` 或查看 `esptool 文档 <https://docs.espressif.com/projects/esptool/en/latest/esptool/index.html>`_。

与 ``build`` 命令类似，使用 ``app``、``bootloader`` 或 ``partition-table`` 参数运行此命令，可选择仅烧录应用程序、引导加载程序或分区表。

.. _merging-binaries:

合并二进制文件：``merge-bin``
-----------------------------

.. code-block:: bash

  idf.py merge-bin [-o output-file] [-f format] [<format-specific-options>]

在某些情况下（例如将文件传输到另一台机器，且不借助 ESP-IDF 对其进行烧录），只烧录一个文件比烧录 ``idf.py build`` 生成的多个文件更为便捷。

``idf.py merge-bin`` 命令会根据项目配置合并引导加载程序、分区表、应用程序本身以及其他分区（如果有的话），并在 build 文件夹中创建一个二进制文件 ``merged-binary.[bin|hex]``，之后可对其进行烧录。

合并后的文件的输出格式可以是二进制 (raw)，IntelHex (hex) 以及 UF2 (uf2)。

uf2 二进制文件也可以通过 :ref:`idf.py uf2 <generate-uf2-binary>` 生成。``idf.py uf2`` 命令在功能上与 ``idf.py merge-bin -f uf2`` 命令等效，而将二进制文件合并成上述各种格式时，``idf.py merge-bin`` 命令更具灵活性与可选性。

用法示例:

.. code-block:: bash

  idf.py merge-bin -o my-merged-binary.bin -f raw

还有一些特定格式的选项，如下所示:

- 仅针对 raw 格式：

  - ``--flash-offset``：此选项创建的合并二进制文件应在指定偏移处进行烧录，而不是在标准偏移地址 0x0 处。
  - ``--pad-to-size``：设置此选项，系统将在最终的二进制文件中添加 FF 字节直至文件大小与 flash 大小等同，从而确保烧录范围能够完整地覆盖整个 flash 芯片，且在烧录时整个 flash 芯片都被重写。

- 仅针对 uf2 格式：

  - ``--md5-disable``：该选项会在每个数据块的末尾禁用 MD5 校验和。在与 `tinyuf2 <https://github.com/adafruit/tinyuf2>`__ 等工具进行集成时，可以启用此选项。

错误处理提示
==============================

``idf.py`` 使用存储在 :idf_file:`tools/idf_py_actions/hints.yml` 中的提示数据库，当找到与给定错误相匹配的提示时，``idf.py`` 会打印该提示以尝试提供解决方案。目前，错误处理提示不支持 menuconfig 对象。

若无需该功能，可以通过 ``idf.py`` 的 ``--no-hints`` 参数关闭提示。

重要提示
===============

多个 ``idf.py`` 命令可以在同一行命令中组合使用。例如，``idf.py -p COM4 clean flash monitor`` 可以清除源代码树、编译工程、并将其烧录到目标芯片，随后运行串行监视器。

在同一调用中，多个 ``idf.py`` 命令的顺序并不重要，它们将自动以正确的程序执行，以使全部操作生效（例如先构建后烧录、先擦除后烧录）。

``idf.py`` 会尝试将未知命令作为构建系统目标执行。

命令 ``idf.py`` 支持 bash、zsh 和 fish shell 的 `shell 自动补全 <https://click.palletsprojects.com/shell-completion/>`_。

调用命令 ``export`` 为 ``idf.py`` 启用自动补全（:ref:`Windows <get-started-set-up-env>`、:ref:`Linux 或 macOS 设置环境<get-started-set-up-env-linux-macos>`），按 TAB 键启动自动补全。输入 ``idf.py -`` 并按 TAB 键以自动补全选项。

预计未来版本将支持 PowerShell 自动补全。

高级命令
=================

打开文档：``docs``
----------------------------

.. code-block:: bash

  idf.py docs

此命令将在浏览器中打开工程目标芯片和 ESP-IDF 版本对应的文档。

显示大小：``Size``
----------------------

.. code-block:: bash

  idf.py size

此命令将显示应用程序大小，包括占用的 RAM 和 flash 及各部分（如 .bss）的大小。

.. code-block:: bash

  idf.py size-components

此命令将显示工程中各个组件的应用程序大小。

.. code-block:: bash

  idf.py size-files

该命令将显示工程中每个源文件的大小。

选项
^^^^^^^

- ``--format`` 指定输出格式，可输出 ``text``、``csv``、``json2``、``tree``、``raw`` 格式，默认格式为 ``text``。
- ``--output-file`` 可选参数，可以指定命令输出文件的文件名，而非标准输出。

重新配置工程：``reconfigure``
------------------------------------

.. code-block:: bash

  idf.py reconfigure

此命令将重新运行 CMake_。正常情况下并不会用到该命令，因为一般无需重新运行 CMake，但如果从源代码树中添加或删除了文件，或需要修改 CMake 缓存变量时，将有必要使用该命令。例如，``idf.py -DNAME='VALUE' reconfigure`` 可将变量 ``NAME`` 在 CMake 缓存中设置为值 ``VALUE``。

清除 Python 字节码：``python-clean``
----------------------------------------

.. code-block:: bash

  idf.py python-clean

此命令将从 ESP-IDF 目录中删除生成的 Python 字节码。字节码在切换 ESP-IDF 和 Python 版本时可能会引起问题，建议在切换 Python 版本后运行此命令。

.. _generate-uf2-binary:

生成 UF2 二进制文件：``uf2``
---------------------------------

.. code-block:: bash

  idf.py uf2

此命令将在 build 目录中生成一个 UF2（`USB 烧录格式 <https://github.com/microsoft/uf2>`_) 二进制文件 ``uf2.bin``，该文件包含所有烧录目标芯片所必需的二进制文件，即引导加载程序、应用程序和分区表。

在 ESP 芯片上运行 `ESP USB Bridge <https://github.com/espressif/esp-usb-bridge>`_ 项目将创建一个 USB 大容量存储设备，用户可以将生成的 UF2 文件复制到该 USB 设备中，桥接 MCU 将使用该文件来烧录目标 MCU。这一操作十分简单，只需将文件复制（或“拖放”）到文件资源管理器访问的公开磁盘中即可。

如需仅为应用程序生成 UF2 二进制文件，即不包含引导加载程序和分区表，请使用 ``uf2-app`` 命令。

``idf.py uf2`` 命令在功能上与 :ref:`上述 <merging-binaries>` ``idf.py merge-bin -f uf2`` 命令等效。而将二进制文件合并为除 uf2 以外的各种格式时，``idf.py merge-bin`` 命令更具灵活性和可选性。

.. code-block:: bash

  idf.py uf2-app

读取 Otadata 分区：``read-otadata``
--------------------------------------

.. code-block:: bash

  idf.py read-otadata

此命令将打印 ``otadata`` 分区的内容，该分区存储当前所选 OTA 应用程序分区的信息。有关 ``otadata`` 分区的更多信息，请参阅 :doc:`/api-reference/system/ota`。

启动 MCP 服务器：``mcp-server``
---------------------------------

.. code-block:: bash

  idf.py mcp-server

此命令将启动 MCP（模型上下文协议）服务器，实现 AI 与 ESP-IDF 项目的集成。该服务器通过标准化协议提供工具和资源，使 AI 助手能够与 ESP-IDF 项目进行交互。

MCP 服务器提供以下工具：

- ``build_project``：使用指定目标芯片构建 ESP-IDF 项目
- ``set_target``：设置 ESP-IDF 目标芯片（esp32、esp32s3、esp32c6 等）
- ``flash_project``：将构建好的项目烧录至已连接设备
- ``monitor_serial``：启动串行监视器（在后台运行）
- ``clean_project``：清理构建产物
- ``menuconfig``：打开 menuconfig 界面（基于终端）

同时提供以下资源：

- ``project://config``：获取当前项目配置
- ``project://status``：获取当前项目构建状态
- ``project://devices``：获取已连接的 ESP 设备列表

.. note::

    运行 MCP 服务器需提前安装 ``mcp`` Python 包。可通过以下命令安装：``./install.sh --enable-mcp``。

将 ESP-IDF MCP 服务器添加到 IDE
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Claude Desktop：**

使用 Claude CLI 添加 ESP-IDF MCP 服务器：

.. code-block:: bash

  claude mcp add esp-idf python /path/to/esp-idf/tools/idf.py mcp-server --env IDF_PATH=/path/to/esp-idf

配置预设：``--preset``
========================

ESP-IDF 支持 `CMake presets`_ 以简化多个构建配置的管理。此功能允许定义可重用的配置配置文件，这些文件指定构建目录、缓存变量和其他 CMake 设置。

.. code-block:: bash

  idf.py --preset <preset-name> build

此命令使用指定的配置预设来构建项目。该预设定义了诸如构建目录位置、CMake 缓存变量（包括 ``SDKCONFIG`` 路径）和生成器首选项等设置。

预设定义文件
--------------

在项目根目录下创建一个 ``CMakePresets.json`` 或 ``CMakeUserPresets.json`` 文件来定义 **配置预设**。例如：

.. code-block:: json

  {
      "version": 3,
      "configurePresets": [
          {
              "name": "default",
              "binaryDir": "build/default",
              "displayName": "Default Configuration",
              "cacheVariables": {
                  "SDKCONFIG": "./build/default/sdkconfig"
              }
          },
          {
              "name": "production",
              "binaryDir": "build/production",
              "displayName": "Production Build",
              "cacheVariables": {
                  "SDKCONFIG_DEFAULTS": "sdkconfig.defaults.prod_common;sdkconfig.defaults.production",
                  "SDKCONFIG": "./build/production/sdkconfig"
              }
          }
      ]
  }

.. note::

    字段 ``version`` 代表 CMake Presets 的 JSON 模式版本。在本例中，其值设为 ``3``，以匹配 ESP-IDF 支持的最低 CMake 版本所支持的模式。如果你使用的是更高版本的 CMake，可以相应地增加 ``version`` 字段的值。请参阅 `CMake Presets`_。

**当前限制**

- ESP-IDF 目前不支持用于预设继承的 ``inherits`` 字段。包含继承的预设将显示警告。

自动预设选择
----------------

如果未指定预设但存在 ``CMakePresets.json`` 文件，``idf.py`` 将自动选择一个预设：

1.  如果存在名为 ``default`` 的预设，则将使用它。
2.  否则，将选择文件中的第一个预设。

.. note::

    环境变量 ``IDF_PRESET`` 可用于设置默认预设名称，例如 ``export IDF_PRESET=production``。命令行参数会覆盖环境变量。

**SDKCONFIG 文件位置**

默认情况下，``sdkconfig`` 文件在项目根目录中创建。但是，在使用 CMake 预设时，可以使用 ``SDKCONFIG`` 缓存变量指定 ``sdkconfig`` 文件的自定义位置。

完整示例请参阅 :example_file:`Multiple Build Configurations Example <build_system/cmake/multi_config/README.md>`。

全局选项
==============

运行 ``idf.py --help`` 列出所有可用的根级别选项。要列出特定子命令的选项，请运行 ``idf.py <command> --help``，如 ``idf.py monitor --help``。部分常用选项如下：

- ``-C <dir>`` 支持从默认的当前工作目录覆盖工程目录。
- ``-B <dir>`` 支持从工程目录的默认 ``build`` 子目录覆盖 build 目录。
- ``--ccache`` 可以在安装了 CCache_ 工具的前提下，在构建源文件时启用 CCache_，减少部分构建耗时。

.. important::

    注意，某些旧版本 CCache_ 在某些平台上存在 bug，因此如果文件没有按预期重新构建，可禁用 CCache_ 并重新构建。可以通过将环境变量 ``IDF_CCACHE_ENABLE`` 设置为非零值来默认启用 CCache_。

- ``-v`` 会使 ``idf.py`` 和构建系统生成详细的构建输出，有助于调试构建错误。
- ``--cmake-warn-uninitialized`` （或 ``-w``）将使 CMake 只显示在工程目录中发现的变量未初始化的警告，该选项仅控制 CMake 内部的 CMake 变量警告，不控制其他类型的构建警告。将环境变量 ``IDF_CMAKE_WARN_UNINITIALIZED`` 设置为非零值，可永久启用该选项。
- ``--no-hints`` 用于禁用有关错误处理的提示并禁用捕获输出。

通过 ``@file`` 传递参数
---------------------------------

可以通过文件向 ``idf.py`` 传递多个参数。该文件或文件路径须在开头使用 ``@`` 进行标注。文件中的参数支持通过换行或空格分隔，并按其在 idf.py 命令行中的顺序扩展。

例如，当前有文件 `custom_flash.txt`：

.. code-block:: bash

  flash --baud 115200

运行命令：``idf.py "@custom_flash.txt" monitor``

文件中的参数可以与额外的命令行参数结合使用，也支持同时使用带有 ``@`` 标注的多个文件。例如，另有一个文件 ``another_config.txt``，此时，可以通过指定 ``idf.py "@custom_flash.txt" "@another_config.txt" monitor`` 同时使用两个文件。

关于参数文件的更多示例，如通过 @filename 创建配置文件概要，请参阅多个构建配置示例文档中的 `创建配置文件概要 <profile_file_>`_ 小节。

扩展 ``idf.py``
====================

``idf.py`` 支持扩展功能。通过项目中的扩展文件以及参与构建的组件中的扩展文件，可以增加额外的子命令、全局选项和回调函数；通过暴露入口点的外部 Python 包，可以提供新的扩展功能。

- **参与构建的组件**：在项目根目录，或注册在项目 ``CMakeLists.txt`` 中的组件根目录，放置名为 ``idf_ext.py`` 的文件，该文件会在项目配置完成后得到识别。运行 ``idf.py build`` 或 ``idf.py reconfigure``，新添加的命令即可生效。
- **Python 入口点**：对于任何已安装的 Python 包，在 ``idf_extension`` 组中定义入口点后，就可以提供扩展功能。只要安装了 Python 包就可以使用扩展功能，无需重新构建项目。

.. important::

   扩展不能定义与 ``idf.py`` 命令同名的子命令或选项。系统会检查自定义的动作和选项名称是否存在冲突，不允许覆盖默认命令，如有冲突会打印警告。对于 Python 入口点，必须使用唯一标识符，否则会忽略重复的入口点名称并发出警告。

扩展文件示例
----------------------

扩展文件需要定义一个 ``action_extensions`` 函数，用于返回扩展的动作或选项。组件扩展 ``idf_ext.py`` 和基于包的扩展（例如 ``<package_name>_ext.py``）使用相同的结构，如下所示：

.. code-block:: python

  from typing import Any
  import click

  def action_extensions(base_actions: dict, project_path: str) -> dict:
      def hello_test(subcommand_name: str, ctx: click.Context, global_args: dict, **action_args: Any) -> None:
          message = action_args.get('message')
          print(f"Running action: {subcommand_name}. Message: {message}")

      def global_callback_detail(ctx: click.Context, global_args: dict, tasks: list) -> None:
          if getattr(global_args, 'detail', False):
              print(f"About to execute {len(tasks)} task(s): {[t.name for t in tasks]}")

      return {
          "version": "1",
          "global_options": [
              {
                  "names": ["--detail", "-d"],
                  "is_flag": True,
                  "help": "Enable detailed output",
              }
          ],
          "global_action_callbacks": [global_callback_detail],
          "actions": {
              "hello": {
                  "callback": hello_test,
                  "short_help": "Hello from component",
                  "help": "Test command from component extension",
                  "options": [
                      {
                          "names": ["--message", "-m"],
                          "help":  "Custom message to display",
                          "default": "Hi there!",
                          "type": str,
                      }
                  ]
              },
          },
      }


扩展 API 参考
-----------------------

``action_extensions`` 函数接收两个参数： ``base_actions`` 表示当前已注册的所有命令， ``project_path`` 表示项目的绝对路径。该函数返回一个包含最多四个键的字典：

- ``version``：表示扩展接口版本。当前 API 版本为 ``1``。此键为必填项。
- ``global_options``：一组全局选项，适用于所有命令。每个选项都是一个字典，包含 ``names``、 ``help``、 ``type``、 ``is_flag``、 ``scope`` 等字段。
- ``global_action_callbacks``：表示一组全局回调函数，在执行任何任务之前都会调用一次。每个全局回调函数接受三个参数：

   - ``ctx``：即 `click context`_
   - ``global_args``：所有可用的全局参数
   - ``tasks``：将要执行的任务列表。任务指的是运行 ``idf.py`` 时所调用的具体动作或子命令

- ``actions``：子命令字典，用于定义新的子命令。每个子命令都有一个 ``callback`` 函数，并且可以包含 ``options``、 ``arguments``、 ``dependencies`` 等。每个回调函数接受三到四个参数：

   - ``subcommand_name``：命令的名称（在多个命令共享同一回调时很有用）
   - ``ctx``：即 `click context`_
   - ``global_args``：所有可用的全局参数
   - ``**action_args``：传递给该子命令的具体参数，可选

基本用法示例
--------------------

1) **通过项目组件提供扩展**

  在项目根目录或某个已注册的组件目录下创建 ``idf_ext.py`` （例如 ``components/my_component/idf_ext.py`` ）。实现内容可参考上面的扩展文件示例。

  运行 ``idf.py build`` 或 ``idf.py reconfigure`` 加载新命令，然后执行 ``idf.py --help`` 即可看到新扩展。

2) **通过 Python 包入口点提供扩展**

  使用上述扩展文件示例，在名为 ``<package_name>_ext.py`` 的模块中实现扩展，并通过 ``idf_extension`` 入口点组暴露 ``action_extensions`` 函数。例如，在 ``pyproject.toml`` 中配置：

  .. code-block:: TOML

    [project]
    name = "my_comp"
    version = "0.1.0"

    [project.entry-points.idf_extension]
    my_pkg_ext = "my_component.my_ext:action_extensions"


  将该包安装到与 ``idf.py`` 相同的 Python 环境中（例如在包目录下执行 ``pip install -e .``）。建议使用唯一的模块名（例如 ``<package_name>_ext.py``）避免命名冲突。安装成功后，运行 ``idf.py --help`` 就可以看到新扩展命令。

.. _cmake: https://cmake.org
.. _ninja: https://ninja-build.org
.. _esptool: https://github.com/espressif/esptool/#readme
.. _CCache: https://ccache.dev/
.. _click context: https://click.palletsprojects.com/en/stable/api/#context
.. _CMake presets: https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html
.. _profile_file: https://github.com/espressif/esp-idf/tree/release/v5.5/examples/build_system/cmake/multi_config#create-configuration-profile-files-via-filename
