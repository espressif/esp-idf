*****************************************************
Eclipse IDE 的创建和烧录指南（Windows 平台）
*****************************************************
:link_to_translation:`en:[English]`

Windows 平台上的 Eclipse 配置略有不同，具体步骤请见下文。

注意：OS X 和 Linux 平台上的 Eclipse IDE 配置，请见 :doc:`Eclipse IDE page <eclipse-setup>`。

安装 Eclipse IDE
==================

Windows 平台的 Eclipse 安装步骤与其他平台相同，请见 :ref:`Installing Eclipse IDE <eclipse-install-steps>`。

.. _eclipse-windows-setup:


Windows 平台上的 Eclipse 配置
================================

完成 Eclipse IDE 的安装后，请按照下列步骤继续操作：

导入新项目
-------------

* Eclipse IDE 需使用 ESP-IDF 的 Makefile 功能。因此，在使用 Eclipse 前，您需要先创建一个 ESP-IDF 项目。在创建 ESP-IDF 项目时，您可以使用 GitHub 中的 idf-template 项目模版，或从 esp-idf 子目录中选择一个 example。

* 运行 Eclipse，选择 “File” -> “Import...”。

* 在弹出的对话框中选择 “C/C++” -> “Existing Code as Makefile Project”，然后点击 “Next”。

* 下个界面，在 “Existing Code Location” 位置输入您的 IDF 项目的路径。注意，这里应填入 ESP-IDF 项目的路径，而非 ESP-IDF 的路径（稍后再填写）。此外，您指定的目录中应包含名为 “Makefile”（项目 Makefile）的文件。

* 在同一页面上，在 “Toolchain for Indexer Settings” 下取消选中 “Show only available toolchains that support this platform”。

* 在出现的扩展列表中，选择 “Cygwin GCC”。然后点击 “Finish”。

*注意：您可能看到有关“无法找到 Cygwin GCC 工具链”的警告。这种情况并不影响安装，我们只需重新配置 Eclipse，并找到我们的工具链即可。*

项目属性 
----------

* 新项目将出现在 “Project Explorer” 下。请右键选择该项目，并在菜单中选 “Properties”。

* 点击 “C/C++ Build” 属性页。

	* 取消选中 “Use default build command”，然后输入命令开始自定义创建：``python ${IDF_PATH}/tools/windows/eclipse_make.py``

* 点击 “C/C++ Build” 下的 “Environment” 属性页面。

	* 选择 “Add...”，并在对应位置输入 ``BATCH_BUILD`` 和 ``1``。

	* 再次点击 “Add...”，输入名称 ``IDF_PATH``，并填写 ESP-IDF 的完整安装路径。``IDF_PATH`` 目录路径应使用正斜杠，而非反斜线，例如 ``C:/Users/MyUser/Development/esp-idf``。

	* 选择 PATH 环境变量，删除默认值，并将其替换为 ``C:\msys32\usr\bin;C:\msys32\mingw32\bin;C:\msys32\opt\xtensa-esp32-elf\bin`` （如果您已经将 msys32 安装到其他目​​录，这里请自行调整）。


* 点击 “C/C++ General” -> “Preprocessor Include Paths, Macros, etc.” 属性页。

	* 点击 “Providers” 选项卡。
	
		* 从 “Providers” 列表中选择 “CDT Cross GCC Built-in Compiler Settings”，将 “Command to get compiler specs” 修改为 ``xtensa-esp32-elf-gcc ${FLAGS}  -std=c++11 -E -P -v -dD "${INPUTS}"``
		
		* 从 “Providers” 列表中选择 “CDT GCC Build Output Parser”，将 “Compiler command pattern” 修改为 ``xtensa-esp32-elf-(gcc|g\+\+|c\+\+|cc|cpp|clang)``

点击 “C/C++ General" -> "Indexer” 属性页。

    * 选择 “Enable project specific settings” 以启用本页上的其他设置。

.. note::

    取消选中 “Allow heuristic resolution of includes”。因为启用此选项时，有时会导致 Eclipse 无法找到正确的头文件目录。

在 Eclipse IDE 中创建项目
---------------------------

Windows 平台的 Eclipse 项目创建步骤与其他平台相同，请见 :ref:`Building in Eclipse <eclipse-build-project>`。

技术细节
=========

**以下内容仅供 Windows 平台专家或非常感兴趣的开发者阅读。**

Windows 平台的 Eclipse 介绍到此结束，下方将主要将介绍一些关键步骤的原理，助您了解更多 Eclipse 的背景信息。

* 首先，xtensa-esp32-elf-gcc 交叉编译器 *并非* Cygwin 工具链，但我们会在 Eclipse 中指定其为 Cygwin 工具链。主要原因在于：msys2 需要使用 Cygwin，并支持 Unix 风格的路径，即 ``/c/blah``，而非 ``c:/blah`` 或 ``c:\\blah``。特别需要说明的是，``xtensa-esp32-elf-gcc`` 会“告知” Eclipse 的 ``built-in compiler settings`` 功能，其内置 “include” 目录全部位于 ``/usr/`` 路径下，这也是 Eclipse 唯一可以解析的 ``Unix/Cygwin`` 风格路径。通过在 Eclipse 中指定 ``xtensa-esp32-elf-gcc`` 交叉编译器为 Cygwin 编译器，可以让 Eclipse 使用 cygpath 实用程序直接内部解析路径。


* 在解析 ESP-IDF 的 make 结果时也经常会出现同样的问题。Eclipse 可以解析 make 的结果，查找头文件目录，但是无法脱离 ``cygpath``，直接解析类似 ``/c/blah`` 的目录。``Eclipse Build Output Parser`` 将利用该机制确认是否调用 ``cygpath``，但由于未知原因，目前 ESP-IDF 配置并不会触发该功能。出于这个原因，我们会使用 ``eclipse_make.py`` 包装脚本调用 ``make``，然后使用 ``cygpath`` 处理 Eclipse 的结果。
