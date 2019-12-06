**********************************************
Eclipse IDE 的创建和烧录指南（传统 GNU Make）
**********************************************
:link_to_translation:`en:[English]`

.. include:: ../gnu-make-legacy.rst

.. _eclipse-install-steps-legacy:

安装 Eclipse IDE
================

Eclipse IDE 是一个可视化的集成开发环境，可用于编写、编译和调试 ESP-IDF 项目。

* 首先，请在您的平台上安装相应的 ESP-IDF，具体步骤请按照您的操作系统（Windows、OS X 和 Linux）对应的指示进行安装。

* 我们建议，您可以首先使用命令行创建一个项目，大致熟悉项目的创建流程。此外，您还需要使用命令行 (``make menuconfig``) 对您的 ESP-IDF 项目进行配置。目前，Eclipse 还无法配置 ESP-IDF 项目。

* 从 eclipse.org_ 下载对应操作系统的 Eclipse 安装器。

* 运行 Eclipse Installer，选择 **"Eclipse for C/C++ Development"** （有的版本也可能显示为 CDT）。

配置 Eclipse
==============

打开安装好的 Eclipse IDE，并按照以下步骤进行操作：

导入新项目
----------

* Eclipse 需使用 ESP-IDF 的 Makefile 功能。因此，在使用 Eclipse 前，您需要先创建一个 ESP-IDF 项目。在创建 ESP-IDF 项目时，您可以使用 GitHub 中的 idf-template 项目模版，或从 ESP-IDF 子目录中复制一个 example。

* 运行 Eclipse，选择 **"File"** -> **"Import..."**。

* 在弹出的对话框中选择 **"C/C++"** -> **"Existing Code as Makefile Project"**，点击 **"Next"**。

* 进入下一界面，在 **"Existing Code Location"** 处输入您的 IDF 项目路径。注意，这里应输入 ESP-IDF 项目的路径，而非 ESP-IDF 本身的路径（这个稍后再填）。此外，您指定的目标路径中应包含名为 ``Makefile`` （项目 Makefile）的文件。

* 本界面中，找到 **"Toolchain for Indexer Settings"**，选择 **"Cross GCC"**，最后点击 **"Finish"**。


项目属性
--------

* 新项目将出现在 **"Project Explorer"** 下。右键选择该项目，并在菜单中选择 **"Properties"**。

* 点击 **"C/C++ Build"** 下的 **"Environment"** 属性页，选择 **"Add..."**，并在对应位置输入 ``BATCH_BUILD`` 和 ``1``。

* 再次点击 **"Add..."**，并在 ``IDF_PATH`` 处输入 ESP-IDF 所在的完整安装路径。Windows 用户可以从 Windows 浏览器复制 ``IDF_PATH``。

* 编辑 ``PATH`` 环境变量。保留当前值，并将该路径增加至 Xtensa 工具链安装路径后，作为 IDF 设置的一部分（如已增加至 PATH，则可忽略）。工具链的常见路径为 ``/home/user-name/esp/xtensa-esp32-elf/bin`` （例）。注意，您需要在补充的路径前增加一个冒号（`:`）。Windows 用户需要将 ``PATH`` 环境变量前增加 ``C:\msys32\mingw32\bin;C:\msys32\opt\xtensa-esp32-elf\bin;C:\msys32\usr\bin``。注意，请根据 msys32 的具体安装路径，修改 ``PATH`` 环境变量。

* MacOS 用户需要增加一个 ``PYTHONPATH`` 环境变量，并将其设置为 ``/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/site-packages``。保证系统中预先安装的 Python （需安装 pyserial 模块）可以覆盖 Eclipse 内置的任何 Python。

**特别说明**：如果您的 IDF_PATH 或项目不在 ``C:\msys32\home`` 之下，则应使用自定义编译命令：``python ${IDF_PATH}/tools/windows/eclipse_make.py`` （请注意，这种方法可能导致编译时间显著增加。）

* 前往 **"C/C++ General"** -> **"Preprocessor Include Paths"** 属性页面。点击 **"Providers"** 选项卡。

    * 从 **"Providers"** 列表中选择 **"CDT Cross GCC Built-in Compiler Settings"**，将 **"Command to get compiler specs"** 修改为 ``xtensa-esp32-elf-gcc ${FLAGS} -std=c++11 -E -P -v -dD "${INPUTS}"``。

    * 从 **"Providers"** 列表中选择 **"CDT GCC Build Output Parser"**，将 **"Compiler command pattern"** 修改为 ``xtensa-esp32-elf-(gcc|g\+\+|c\+\+|cc|cpp|clang)``。

* 前往 **“C/C++ General”** -> **“Indexer”** 属性页面。

    * 勾选 **“Enable project specific settings”**，启用本页面上的其他设置。

    * 取消勾选 **“Allow heuristic resolution of includes”**。勾选该选项有时会导致 Eclipse 无法找到正确的头文件目录。

* 前往 **“C/C++ Build”** -> **“Behavior”** 属性页面。

    * 勾选 **“Enable parallel build”**，启用“并行编译”配置。

.. _eclipse-build-project-legacy:

在 Eclipse 中构建项目
-----------------------

在首次创建项目前，Eclipse IDE 可能会显示大量有关未定义值的错误和警告，这是因为 ESP-IDF 在编译过程中会自动生成一些源文件。因此，这些错误和警告均会在项目编译完成后消失。

* 点击 **“OK”**，关闭 Eclipse IDE 中的 **“Properties”** 对话框。

* 在 Eclipse IDE 界面外，打开命令提示符窗口。进入项目目录，运行 ``make menuconfig`` 设置项目的 ESP-IDF 配置。现阶段，您还无法在 Eclipse 中完成本操作。

.. note:

	如果您尝试跳过配置步骤直接开始编译，ESP-IDF 将在命令行提示需完成配置，但 Eclipse 暂时无法处理这种情况，因此编译将挂起或失败。

* 回到 Eclipse IDE 界面，选择 **“Project”** -> **“Build”** 开始编译项目。

**小提示**：如果您的项目不是通过 Eclipse 创建的，则在选择 **“Project”** -> **“Build”** 前还需进行 **“Project”** -> **“Clean”** 操作。如此操作后，Eclipse 才能查看所有源文件的编译器参数，并借此确定头文件包含路径。

在 Eclipse 中烧录项目
------------------------------------

您可以将 ``make flash`` 目标集成在 Eclipse 项目中，这样就可以通过 Eclipse UI 调用 ``esptool.py`` 进行烧录：

* 打开 **“Project Explorer”**，并右击您的项目（请注意右击项目本身，而非项目下的子文件，否则 Eclipse 可能会找到错误的 ``Makefile``）。

* 从菜单中选择 **“Build Targets”** -> **“Create...”**。

* 目标名称输入 “flash”，其他选项使用默认值。

* 这时，您可以选择 **“Project”** -> **“Build Target”** -> **“Build”** (快捷键：Shift + F9），创建自定义烧录目标，用于编译、烧录项目。

注意，您仍需要通过 ``make menuconfig`` 设置串行端口和其他烧录选项。``make menuconfig`` 仍需通过命令行操作（具体请见对应操作系统的指南）。

如有需要，请按照相同步骤添加 ``bootloader`` 和 ``partition_table`` 目标。


.. _eclipse.org: https://www.eclipse.org/
