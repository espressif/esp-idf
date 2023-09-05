可下载的 ESP-IDF 工具
===============================

:link_to_translation:`en:[English]`

构建过程中，ESP-IDF 依赖许多工具，如交叉编译工具链、CMake 构建系统等。

如 :doc:`快速入门 <../../get-started/index>` 所述，若所需工具版本可用，首选使用当前操作系统的软件包管理器（如 apt、yum、brew 等）安装相关工具。例如，在 Linux 和 macOS 系统中，建议用 Linux 和 macOS 系统的软件包管理器安装 CMake。

但部分 ESP-IDF 的特定工具在操作系统软件包存储库中不可用，且不同版本的 ESP-IDF 需相应使用不同版本的工具运行。为解决以上两个问题，ESP-IDF 提供了一组脚本，可以下载和安装正确的工具版本，并设置相应运行环境。

下文中，这类可下载的工具简称为“工具”。除此类工具外，ESP-IDF 还使用以下工具：

* ESP-IDF 捆绑的 Python 脚本，如 ``idf.py``
* 从 PyPI 安装的 Python 软件包

以下各小节介绍了可下载工具的安装方法，并提供了在不同平台上安装的工具列表。

.. note::

    本文档面向需要自定义其安装过程的高级用户、希望了解安装过程的用户以及 ESP-IDF 开发人员。

    要了解如何安装 ESP-IDF 工具，请参阅 :doc:`快速入门 <../../get-started/index>`。


工具元数据文件
-------------------

各平台所需工具及工具版本列表存放在 :idf_file:`tools/tools.json` 文件中，:idf_file:`tools/tools_schema.json` 定义了该文件的模式。

在安装工具或设置环境变量时，:idf_file:`tools/idf_tools.py` 脚本将使用上述文件。

.. _idf-tools-path:

工具安装目录
----------------------------

``IDF_TOOLS_PATH`` 环境变量指定下载及安装工具的位置。若未设置该变量，Linux 和 macOS 系统的默认下载安装位置为 ``HOME/.espressif``，Windows 系统的默认下载安装位置为 ``%USER_PROFILE%\.espressif``。

在 ``IDF_TOOLS_PATH`` 目录下，工具安装脚本会创建以下子目录和文件：

- ``dist`` — 工具存档下载位置。
- ``tools`` — 工具解压缩位置。工具会解压缩到子目录 ``tools/TOOL_NAME/VERSION/`` 中，该操作支持同时安装不同版本的工具。
- ``idf-env.json`` — “目标 (target)”和“功能 (feature)”等用户安装选项均存储在此文件中。“目标”为选择需要安装和保持更新的工具的芯片目标；“功能”则决定应安装哪些 Python 软件包。有关用户安装选项的详情，请参阅下文。
- ``python_env`` —  与工具无关；虚拟 Python 环境安装在其子目录中。注意，设置 ``IDF_PYTHON_ENV_PATH`` 环境变量可以将 Python 环境目录放置到其他位置。
- ``espidf.constraints.*.txt`` — 每个 ESP-IDF 版本都有的约束文件，包含 Python 包版本要求。

GitHub 资源镜像
-----------------------

工具下载器下载的工具大多属于 GitHub 发布的资源，即在 GitHub 上伴随软件发布的文件。

如果无法访问 GitHub 下载或访问速度较慢，可以配置一个 GitHub 资源镜像。

要使用乐鑫下载服务器，请将环境变量  ``IDF_GITHUB_ASSETS`` 设置为 ``dl.espressif.com/github_assets``，在国内下载时，也可设置为 ``dl.espressif.cn/github_assets`` 加快下载速度。安装过程中，当从 ``github.com`` 下载工具时，URL 将重写为使用乐鑫下载服务器。

只要 URL 与 ``github.com`` 的下载 URL 格式匹配，任何镜像服务器均可使用，安装过程中下载的 GitHub 资源 URL 将把 ``https://github.com`` 替换为 ``https://${IDF_GITHUB_ASSETS}``。

.. note:: 目前，乐鑫下载服务器不会镜像 GitHub 上的所有内容，只镜像部分发布版本的附件资源文件及源文件。


``idf_tools.py`` 脚本
---------------------------------------

ESP-IDF 随附的 :idf_file:`tools/idf_tools.py` 脚本具备以下功能：

* ``install``：将工具下载到 ``${IDF_TOOLS_PATH}/dist`` 目录，并解压缩到 ``${IDF_TOOLS_PATH}/tools/TOOL_NAME/VERSION``。

  ``install`` 命令接收 ``TOOL_NAME`` 或 ``TOOL_NAME@VERSION`` 格式的安装工具列表。如果给定参数 ``all``，则会安装列表上的所有工具，包括必须项和可选项。如果没有给定参数，或给定参数为 ``required``，则只安装必须项。

* ``download``：与 ``install`` 类似，但不会解压缩工具。使用可选项 ``--platform`` 可下载特定平台的工具。

* ``export``：列出使用已安装工具前应设置的环境变量。对多数工具而言，只需要设置环境变量 ``PATH``，但也有些工具需要设置额外的环境变量。

  环境变量可以以 ``shell`` 或 ``key-value`` 格式列出，使用 ``--format`` 参数设置该选项。

  - ``export`` 可选参数：

    - ``--unset``：该参数可用于创建语句，取消特定全局变量设置，使环境恢复到调用 ``export.{sh/fish}`` 前的状态。
    - ``--add_paths_extras``：该参数将 ``$PATH`` 中与 ESP-IDF 相关的额外路径添加到 ``${IDF_TOOLS_PATH}/esp-idf.json`` 中，以保证在退出当前 ESP-IDF 环境时删除全局变量。例如，在运行 ``export.{sh/fish}`` 脚本时，如果在全局变量 ``$PATH`` 中添加了新的路径，在命令中添加该参数可以将这些新路径保存到 ``${IDF_TOOLS_PATH}/esp-idf.json`` 文件中。

  - ``shell``：生成适合在 shell 中执行的输出，例如，在 Linux 和 macOS 上生成以下输出

    .. code-block:: none

        export PATH="/home/user/.espressif/tools/tool/v1.0.0/bin:$PATH"

    在 Windows 上生成以下输出

    .. code-block:: none

        set "PATH=C:\Users\user\.espressif\tools\v1.0.0\bin;%PATH%"

    .. note::

        当前不支持以 Powershell 格式导出环境变量，可以用 ``key-value`` 格式代替。

    如果 shell 支持，则该命令的输出可用于更新环境变量。例如

    .. code-block:: none

        eval $($IDF_PATH/tools/idf_tools.py export)

  - ``key-value``：以 ``VARIABLE=VALUE`` 格式生成输出，以便其他脚本解析

    .. code-block:: none

        PATH=/home/user/.espressif/tools/tool/v1.0.0:$PATH

    注意，用于处理此输出的脚本必须对输出中的 ``$VAR`` 或 ``%VAR%`` 模式进行扩展，即解析成对应变量。

* ``list``：列出已知的工具版本，并指示哪些版本已安装。

  以下选项可用于自定义输出。

  - ``--outdated``：仅列出安装在 ``IDF_TOOLS_PATH`` 中的过时版本工具。

* ``check``：检查每个工具是否在系统路径和 ``IDF_TOOLS_PATH`` 中可用。

* ``install-python-env``：在 ``${IDF_TOOLS_PATH}/python_env`` 目录或直接在 ``IDF_PYTHON_ENV_PATH`` 环境变量设置的目录中创建 Python 虚拟环境，并在其中安装所需的 Python 软件包。

  * 参数 ``--features`` 为可选项，用于指定要添加或删除的功能列表，功能之间用逗号分隔。

    #. 该参数将删除以 ``-`` 开头的功能，添加以 ``+`` 开头或无符号标记的功能。例如，要删除功能 ``XY``，示例语法为 ``--features=-XY``；要添加功能 ``XY``，示例语法为 ``--features=+XY`` 或 ``--features=XY``。如果为同一功能同时提供了删除和添加选项，则不执行任何操作。

    #. 每个功能都必须有依赖文件。例如，只有当 ``${IDF_PATH}/tools/requirements/requirements.XY.txt`` 文件已存在，并包含要安装的 Python 包列表时，功能 ``XY`` 才有效。

    #. ``core`` 功能为必须项，确保 ESP-IDF 的核心功能，如控制台中的构建、烧录、监视器、调试等。

    #. 用户可选择任意数量的可选功能，已选功能列表存储在 ``idf-env.json`` 中。

    #. 依赖文件中存储了需要安装的 Python 包以及 ``espidf.constraints.*.txt`` 文件，该约束文件从 https://dl.espressif.com 下载，并存储在 ``${IDF_TOOLS_PATH}`` 目录，包含了针对特定 ESP-IDF 版本的安装包版本要求。

  .. note::

    可以通过使用 ``--no-constraints`` 参数或将环境变量 ``IDF_PYTHON_CHECK_CONSTRAINTS`` 设置为 ``no``，禁用约束文件的下载和使用，但 **并不建议此做法**。

* ``check-python-dependencies``：检查所有必需的 Python 包是否均已安装。该命令会对比检查由 ``idf-env.json`` 功能列表从 ``${IDF_PATH}/tools/requirements/requirements.*.txt`` 所选择的软件包与 ``espidf.constraints.*.txt`` 文件指定的软件包版本是否一致。

  .. note::

    约束文件可通过 ``install-python-env`` 命令下载。与 ``install-python-env`` 命令类似，可以通过使用 ``--no-constraints`` 参数或将环境变量 ``IDF_PYTHON_CHECK_CONSTRAINTS`` 设置为 ``no``，禁止使用约束文件。

* ``uninstall``：打印并删除当前 ESP-IDF 版本未使用的工具。

  - ``--dry-run`` 打印已安装但未使用的工具。
  - ``--remove-archives`` 删除过去下载的所有旧版本软件安装包。

.. _idf-tools-install:

安装脚本
---------------

ESP-IDF 的根目录中提供了针对不同 shell 的用户安装脚本，包括：

* ``install.bat`` 适用于 Windows 命令提示符
* ``install.ps1`` 适用于 Powershell
* ``install.sh`` 适用于 Bash
* ``install.fish`` 适用于 Fish

这些脚本除了下载和安装 ``IDF_TOOLS_PATH`` 中的工具外，还会准备一个 Python 虚拟环境，并在此虚拟环境中安装所需软件包。

为启用相应功能，这些脚本可以选择性地接受一组以逗号分隔的芯片目标列表及 ``--enable-*`` 参数，这类参数会传递给 ``idf_tools.py`` 脚本，并由该脚本将这类参数存储在 ``idf-env.json`` 中，从而逐步启用芯片目标及功能。

要为所有芯片目标安装工具，请在不使用任何可选参数的情况下，运行 ``idf_tools.py install --targets=all``。要安装具备 ESP-IDF 核心功能的 Python 软件包，请运行 ``idf_tools.py install-python-env --features=core``。

也可为特定芯片安装工具，例如，运行 ``install.sh esp32`` 可以只为 ESP32 安装工具。更多相关示例，请参阅 :ref:`第三步：设置工具 <get-started-set-up-tools>`。

运行 ``idf_tools.py install-python-env --features=core,XY``，``install.sh --enable-XY`` 可以启用 ``XY`` 功能。

.. _idf-tools-export:

导出脚本
--------------

由于安装好的工具并非永久添加到用户或系统的 ``PATH`` 环境变量中，因此，要在命令行中使用这些工具，还需要额外步骤。以下脚本会修改当前 shell 的环境变量，从而使用正确版本的工具：

* ``export.bat`` 适用于 Windows 命令提示符
* ``export.ps1`` 适用于 Powershell
* ``export.sh`` 适用于 Bash
* ``export.fish`` 适用于 Fish

.. note::

    在 Bash 中修改 shell 环境时，必须使用 ``. ./export.sh`` 命令加载 ``export.sh``，注意添加前面的点和空格。

    ``export.sh`` 可以在除了 Bash 外的其他 shell（如 zsh）中使用。但在这种情况下，必须在运行脚本前设置 ``IDF_PATH`` 环境变量。在 Bash 中使用时，脚本会从当前目录猜测 ``IDF_PATH`` 的值。

除了调用 ``idf_tools.py``，这些脚本还会列出已经添加到 ``PATH`` 的目录。

其他安装方法
--------------------------

为适用于不同环境，ESP-IDF 提供了更多用户友好的 ``idf_tools.py`` 包装工具：

* :ref:`ESP-IDF 工具安装器 <get-started-windows-tools-installer>` 支持下载和安装工具，其内部使用 ``idf_tools.py`` 实现功能。
* `ESP-IDF Eclipse 插件 <https://github.com/espressif/idf-eclipse-plugin/blob/master/README.md>`_ 包括了一个用于设置工具的菜单项，该插件内部调用 ``idf_tools.py``。
* `VSCode ESP-IDF 扩展 <https://github.com/espressif/vscode-esp-idf-extension/blob/master/docs/tutorial/install.md>`_ 提供了设置工具的入门流程。尽管此扩展包不依赖 ``idf_tools.py``，但安装方法相同。

自定义安装
-------------------

推荐用户使用上述方法安装 ESP-IDF 工具，但也可以选择其他方式来构建 ESP-IDF 应用程序。自定义安装时，用户需将所有必要的工具都安装在某个位置，并在 ``PATH`` 中定义，以保证 ESP-IDF 构建系统可用。

.. _idf-tools-uninstall:

卸载 ESP-IDF
-----------------

卸载 ESP-IDF 需要删除安装过程中配置的工具和环境变量。

* 使用 :ref:`Windows ESP-IDF 工具安装器 <get-started-windows-tools-installer>` 的 Windows 用户可以直接运行卸载向导卸载 ESP-IDF。
* :ref:`工具安装目录 <idf-tools-path>` 下包含了已下载及安装的工具，删除该目录即可删除此前通过运行 :ref:`安装脚本 <idf-tools-install>` 安装的内容。通过 :ref:`导出脚本 <idf-tools-export>` 设置的环境变量不具备永久性，新环境中不会存在此类环境变量。
* 如在安装过程中进行了自定义设置，除删除上述工具外，可能还涉及手动恢复此前为适用 ESP-IDF 工具而修改的环境变量及系统路径，例如 ``IDF_PYTHON_ENV_PATH`` 或 ``IDF_TOOLS_PATH``。如存在通过手动复制安装的工具，则也需手动追踪并删除相关文件。
* 如安装了 `ESP-IDF Eclipse <https://github.com/espressif/idf-eclipse-plugin/blob/master/README.md>`_ 或 `VSCode ESP-IDF 扩展程序 <https://github.com/espressif/vscode-esp-idf-extension/blob/master/docs/tutorial/install.md>`_ 等插件，则需按照对应插件文档中的特定卸载说明进行操作。

.. note::

  卸载 ESP-IDF 工具不会删除任何项目文件或用户代码。为防止意外丢失其他文件，请在删除文件时谨慎操作。如果对某个步骤的操作有所疑问，请参考前述安装说明。

  上述卸载指南默认需卸载的工具是按本文档中的步骤进行安装的。如果使用了自定义安装，可能需要进行相应调整。

.. _idf-tools-list:

ESP-IDF 工具列表
---------------------

.. include-build-file:: idf-tools-inc.rst
