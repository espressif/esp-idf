IDF 前端工具 - ``idf.py``
**************************************

:link_to_translation:`en:[English]`

``idf.py`` 命令行工具提供了一个前端界面，管理工程构建、工程部署及工程调试等操作。该前端界面使用多项工具，如：

- CMake_ 用于配置要构建的工程。
- Ninja_ 用于构建工程。
- `esptool.py`_ 用于烧录目标芯片。

:ref:`第五步：开始使用 ESP-IDF 吧 <get-started-configure>` 简要介绍了设置 ``idf.py`` 以配置、构建及烧录工程的操作流程。

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

    ``idf.py set-target`` 将清除构建目录，并重新生成 ``sdkconfig`` 文件，原来的 ``sdkconfig`` 文件保存为 ``sdkconfig.old``。

.. note::

    ``idf.py set-target`` 命令与以下操作效果相同：

    1. 清除构建目录 (``idf.py fullclean``)
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

  - 若有需要，创建构建子目录 ``build`` 保存构建输出文件，使用 ``-B`` 选项可改变子目录的路径。
  - 必要时运行 CMake_ 配置工程，并为主要构建工具生成构建文件。
  - 运行主要构建工具（Ninja_ 或 ``GNU Make``）。默认情况下，构建工具会完成自动检测，也可通过将 ``-G`` 选项传递给 ``idf.py`` 来显式设置构建工具。

构建是增量行为，因此若上次构建结束后，源文件或配置并未发生更改，则不会执行任何操作。

此外，使用 ``app``、``bootloader`` 或 ``partition-table`` 参数运行此命令，可选择仅构建应用程序、引导加载程序或分区表。

清除构建输出：``clean``
------------------------------

.. code-block:: bash

  idf.py clean

此命令可清除构建目录中的构建输出文件，下次构建时，工程将完全重新构建。注意，使用此选项不会删除构建文件夹内的 CMake 配置输出。

删除所有构建内容：``fullclean``
-------------------------------------------

.. code-block:: bash

  idf.py fullclean

此命令将删除所有 ``build`` 子目录内容，包括 CMake 配置输出。下次构建时，CMake 将重新配置其输出。注意，此命令将递归删除构建目录下的 *所有* 文件（工程配置将保留），请谨慎使用。

烧录工程：``flash``
------------------------

.. code-block:: bash

  idf.py flash

此命令将在需要时自动构建工程，随后将其烧录到目标芯片。使用 ``-p`` 和 ``-b`` 选项可分别设置串口名称和烧录程序的波特率。

.. note:: 环境变量 ``ESPPORT`` 和 ``ESPBAUD`` 可分别设置 ``-p`` 和 ``-b`` 选项的默认值，在命令行上设置这些选项的参数可覆盖默认值。

与 ``build`` 命令类似，使用 ``app``、``bootloader`` 或 ``partition-table`` 参数运行此命令，可选择仅烧录应用程序、引导加载程序或分区表。

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

调用命令 ``export`` 为 ``idf.py`` 启用自动补全（:ref:`第四步：设置环境变量 <get-started-set-up-env>`），按 TAB 键启动自动补全。输入 ``idf.py -`` 并按 TAB 键以自动补全选项。

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

- ``--format`` 指定输出格式，可输出 ``text``、``csv``、 ``json`` 格式，默认格式为 ``text``。
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

生成 UF2 二进制文件：``uf2``
---------------------------------

.. code-block:: bash

  idf.py uf2

此命令将在构建目录中生成一个 UF2（`USB 烧录格式 <https://github.com/microsoft/uf2>`_) 二进制文件 ``uf2.bin``，该文件包含所有烧录目标芯片所必需的二进制文件，即引导加载程序、应用程序和分区表。

在 ESP 芯片上运行 `ESP USB Bridge <https://github.com/espressif/esp-usb-bridge>`_ 项目将创建一个 USB 大容量存储设备，用户可以将生成的 UF2 文件复制到该 USB 设备中，桥接 MCU 将使用该文件来烧录目标 MCU。这一操作十分简单，只需将文件复制（或“拖放”）到文件资源管理器访问的公开磁盘中即可。

如需仅为应用程序生成 UF2 二进制文件，即不包含加载引导程序和分区表，请使用 ``uf2-app`` 命令。

.. code-block:: bash

  idf.py uf2-app

读取 Otadata 分区：``read-otadata``
--------------------------------------

.. code-block:: bash

  idf.py read-otadata

此命令将打印 ``otadata`` 分区的内容，该分区存储当前所选 OTA 应用程序分区的信息。有关 ``otadata`` 分区的更多信息，请参阅 :doc:`/api-reference/system/ota`。

全局选项
==============

运行 ``idf.py --help`` 列出所有可用的根级别选项。要列出特定子命令的选项，请运行 ``idf.py <command> --help``，如 ``idf.py monitor --help``。部分常用选项如下：

- ``-C <dir>`` 支持从默认的当前工作目录覆盖工程目录。
- ``-B <dir>`` 支持从工程目录的默认 ``build`` 子目录覆盖构建目录。
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

运行命令：``idf.py @custom_flash.txt monitor``

文件中的参数可以与额外的命令行参数结合使用，也支持同时使用带有 ``@`` 标注的多个文件。例如，另有一个文件 ``another_config.txt``，此时，可以通过指定 ``idf.py @custom_flash.txt @another_config.txt monitor`` 同时使用两个文件。

关于参数文件的更多示例，如通过 @filename 创建配置文件概要，请参阅 :example_file:`多个构建配置示例 <build_system/cmake/multi_config/README.md>`。

.. _cmake: https://cmake.org
.. _ninja: https://ninja-build.org
.. _esptool.py: https://github.com/espressif/esptool/#readme
.. _CCache: https://ccache.dev/
