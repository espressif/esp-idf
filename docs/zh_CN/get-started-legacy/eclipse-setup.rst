********************************************
Eclipse IDE 的创建和烧录指南 (传统 GNU Make)
********************************************
:link_to_translation:`en:[English]`

.. include:: ../gnu-make-legacy.rst

.. _eclipse-install-steps-legacy:

安装 Eclipse IDE
================

Eclipse IDE 是一个可视化的集成开发环境，可用于编写、编译和调试 ESP-IDF 项目。

* 首先，请在您的平台上安装相应的 ESP-IDF，具体步骤请参考适用于 Windows、OS X 和 Linux 的相应安装步骤。

* 我们建议，您应首先使用命令行创建一个项目，大致熟悉项目的创建流程。此外，您还需要使用命令行 (``make menuconfig``) 对您的 ESP-IDF 项目进行配置。目前，Eclipse 尚不支持对 ESP-IDF 项目进行配置。

* 下载相应版本的 Eclipse Installer 至您的平台，点击 eclipse.org_。

* 运行 Eclipse Installer，选择 “Eclipse for C/C++ Development”（有的版本也可能显示为 CDT）。


配置 Eclipse IDE
=================

请打开安装好的 Eclipse IDE，并按照以下步骤进行操作：

导入新项目
----------

* Eclipse IDE 需使用 ESP-IDF 的 Makefile 功能。因此，在使用 Eclipse 前，您需要先创建一个 ESP-IDF 项目。在创建 ESP-IDF 项目时，您可以使用 GitHub 中的 idf-template 项目模版，或从 esp-idf 子目录中选择一个 example。

* 运行 Eclipse，选择 “File” -> “Import...”。

* 在弹出的对话框中选择 “C/C++” -> “Existing Code as Makefile Project”，然后点击 “Next”。

* 在下个界面中 “Existing Code Location” 位置输入您的 IDF 项目的路径。注意，这里应输入 ESP-IDF 项目的路径，而非 ESP-IDF 本身的路径（这个稍后再填）。此外，您指定的目标路径中应包含名为 ``Makefile`` （项目 Makefile）的文件。

* 在本界面，找到 “Toolchain for Indexer Settings”，选择 “Cross GCC”，最后点击 “Finish”。


项目属性
--------

* 新项目将出现在 “Project Explorer” 下。请右键选择该项目，并在菜单中选择 “Properties”。

* 点击 “C/C++ Build” 下的 “Environment” 属性页，选择 “Add...”，并在对应位置输入 ``BATCH_BUILD`` 和 ``1``。

* 再次点击 “Add...”，并在 “IDF_PATH” 中输入 ESP-IDF 所在的完整安装路径。

* 选择 “PATH” 环境变量，不要改变默认值。如果 Xtensa 工具链的路径尚不在 “PATH” 列表中，则应将该路径 (``something/xtensa-esp32-elf/bin``) 增加至列表，工具链的典型路径类似于 ``/home/user-name/esp/xtensa-esp32-elf/bin``。请注意您需要在附加路径前添加冒号 ``:``。Windows 用户需要将 ``C:\msys32\mingw32\bin;C:\msys32\opt\xtensa-esp32-elf\bin;C:\msys32\usr\bin`` 添加到 ``PATH`` 环境变量的靠前位置（如果您将 msys32 安装到了其它目录，则需要更改对应的路径以匹配您的本地环境）。

* 在 macOS 平台上，增加一个 “PYTHONPATH” 环境变量，并将其设置为 ``/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/site-packages``， 保证系统中预先安装的 Python （需安装 pyserial 模块）可以覆盖 Eclipse 内置的任何 Python。

* 前往 “C/C++ General” -> “Preprocessor Include Paths” 属性页面。

	* 点击 “Providers” 选项卡。
	
		* 从 “Providers” 列表中选择 “CDT Cross GCC Built-in Compiler Settings”，将 “Command to get compiler specs” 修改为 ``xtensa-esp32-elf-gcc ${FLAGS} -std=c++11 -E -P -v -dD "${INPUTS}"``
		
		* 从 “Providers” 列表中选择 “CDT GCC Build Output Parser”，将 “Compiler command pattern” 修改为 ``xtensa-esp32-elf-(gcc|g\+\+|c\+\+|cc|cpp|clang)``

* 前往 “C/C++ General” -> “Indexer” 属性页面。

	* 去除 "Allow heuristic resolution of includes" 勾选。启用此选项时，Eclipse 有时无法找到正确的头文件目录。

点击 “C/C++ General" -> "Indexer” 属性页。

    * 选择 “Enable project specific settings” 以启用本页上的其他设置。

.. note::

    取消选中 “Allow heuristic resolution of includes”。因为启用此选项时，有时会导致 Eclipse 无法找到正确的头文件目录。

点击 “C/C++ Build” -> “Behavior” 属性页。

* 选中 “Enable parallel build” 以启用多任务并行构建。

.. _eclipse-build-project-legacy:

在 Eclipse IDE 中创建项目
--------------------------

在首次创建项目前，Eclipse IDE 可能会显示大量有关未定义值的错误和警告，主要原因在于项目编译过程中所需的一些源文件是在 ESP-IDF 项目创建过程中自动生成的。因此，这些错误和警告将在 ESP-IDF 项目生成完成后消失。

* 点击 “OK”，关闭 Eclipse IDE 中的 “Properties” 对话框。

* 在 Eclipse IDE 界面外，打开命令管理器。进入项目目录，并通过 ``make menuconfig`` 命令对您的 ESP-IDF 项目进行配置。现阶段，您还无法在 Eclipse 中完成本操作。

*如果您未进行最开始的配置步骤，ESP-IDF 将提示在命令行中进行配置 - 但由于 Eclipse 暂时不支持相关功能，因此该项目将挂起或创建失败。*

* 返回 Eclipse IDE 界面中，选择 “Project” -> “Build” 创建您的项目。

**提示**：如果您已经在 Eclipse IDE 环境外创建了项目，则可能需要选择 “Project” -> “Clean before choosing Project” -> “Build”，允许 Eclipse 查看所有源文件的编译器参数，并借此确定头文件包含路径。

在 Eclipse IDE 中烧录项目
--------------------------

您可以将 ``make flash`` 目标放在 Eclipse 项目中，通过 Eclipse UI 调用 ``esptool.py`` 进行烧录：

* 打开 “Project Explorer”，并右击您的项目（请注意右击项目本身，而非项目下的子文件，否则 Eclipse 可能会找到错误的 ``Makefile``）。

* 从菜单中选择 “Build Targets” -> “Create”。

* 输入 “flash” 为目标名称，其他选项使用默认值。

* 选择 “Project” -> “Build Target” -> “Build (快捷键：Shift + F9）”，创建自定义烧录目标，用于编译、烧录项目。

注意，您将需要通过 ``make menuconfig``，设置串行端口和其他烧录选项。``make menuconfig`` 仍需通过命令行操作（请见平台的对应指南）。

如有需要，请按照相同步骤添加 ``bootloader`` 和 ``partition_table``。


.. _eclipse.org: https://www.eclipse.org/
