使用调试器
----------

:link_to_translation:`en:[English]`

本节介绍以下几种配置和运行调试器的方法：

* :ref:`jtag-debugging-using-debugger-eclipse`
* :ref:`jtag-debugging-using-debugger-command-line`
* :ref:`jtag-debugging-with-idf-py`

关于如何使用 VS Code 进行调试，请参阅文档 `使用 VS Code 调试 <https://docs.espressif.com/projects/vscode-esp-idf-extension/zh_CN/latest/debugproject.html>`__。


.. _jtag-debugging-using-debugger-eclipse:

使用 Eclipse 调试
^^^^^^^^^^^^^^^^^^^^^^^

.. note::

    建议首先通过 :ref:`idf.py <jtag-debugging-with-idf-py>` 或 :ref:`命令行 <jtag-debugging-using-debugger-command-line>` 检查调试器是否正常工作，然后再转到使用 :ref:`Eclipse <jtag-debugging-using-debugger-eclipse>` 平台。

作为一款集成开发环境 (IDE)，Eclipse 提供了一套强大的工具，用于开发和调试软件应用程序。对于 ESP-IDF 应用程序，`IDF Eclipse 插件 <https://github.com/espressif/idf-eclipse-plugin>`_ 提供了两种调试方式:

1.  `ESP-IDF GDB OpenOCD 调试 <https://github.com/espressif/idf-eclipse-plugin/blob/master/docs/OpenOCD%20Debugging.md#esp-idf-gdb-openocd-debugging>`_
2.  GDB 硬件调试

默认情况下，Eclipse 通过 GDB 硬件调试插件支持 OpenOCD 调试。该调试方式需要从命令行启动 OpenOCD 服务器，并在 Eclipse 中配置 GDB 客户端，整个过程耗时且容易出错。

为了使调试过程更加容易，`IDF Eclipse 插件 <https://github.com/espressif/idf-eclipse-plugin>`_ 提供了定制的 ESP-IDF GDB OpenOCD 调试功能，支持在 Eclipse 内部配置好 OpenOCD 服务器和 GDB 客户端。该插件已经设置好所有必需的配置参数，点击一个按钮即可开始调试。

因此，建议通过 `IDF Eclipse 插件 <https://github.com/espressif/idf-eclipse-plugin>`_ 进行 `ESP-IDF GDB OpenOCD 调试 <https://github.com/espressif/idf-eclipse-plugin/blob/master/docs/OpenOCD%20Debugging.md#esp-idf-gdb-openocd-debugging>`__。

**GDB 硬件调试**

.. note::

    只有在无法使用 `ESP-IDF GDB OpenOCD 调试 <https://github.com/espressif/idf-eclipse-plugin/blob/master/docs/OpenOCD%20Debugging.md#esp-idf-gdb-openocd-debugging>`_ 的情况下，才建议使用 GDB 硬件调试。

首先，打开 Eclipse，选择 ``Help`` > ``Install New Software`` 来安装 ``GDB Hardware Debugging`` 插件。

安装完成后，按照以下步骤配置调试会话。请注意，一些配置参数是通用的，有些则针对特定项目。我们会通过配置 "blink" 示例项目的调试环境来进行展示，请先按照 `Eclipse Plugin <https://github.com/espressif/idf-eclipse-plugin/blob/master/README_CN.md>`_ 介绍的方法将该示例项目添加到 Eclipse 的工作空间。示例项目 :example:`get-started/blink` 的源代码可以在 ESP-IDF 仓库的 :idf:`examples` 目录下找到。

1.  在 Eclipse 中，进入 ``Run`` > ``Debug Configuration``，会出现一个新的窗口。在窗口的左侧窗格中，双击 ``GDB Hardware Debugging`` （或者选择 ``GDB Hardware Debugging`` 然后按下 ``New`` 按钮）来新建一个配置。

2.  在右边显示的表单中，``Name:`` 一栏中输入配置的名称，例如： “Blink checking”。

3.  在下面的 ``Main`` 选项卡中， 点击 ``Project:`` 边上的 ``Browse`` 按钮，然后选择当前的 ``blink`` 项目。

4.  在下一行的 ``C/C++ Application:`` 中，点击 ``Browse`` 按钮，选择 ``blink.elf`` 文件。如果 ``blink.elf`` 文件不存在，那么很有可能该项目还没有编译，请参考 `Eclipse Plugin <https://github.com/espressif/idf-eclipse-plugin/blob/master/README_CN.md>`_ 指南中的介绍。

5.  最后，在 ``Build (if required) before launching`` 下面点击 ``Disable auto build``。

    上述步骤 1 - 5 的示例输入如下图所示。

    .. figure:: ../../../_static/hw-debugging-main-tab.jpg
        :align: center
        :alt: Configuration of GDB Hardware Debugging - Main tab
        :figclass: align-center

        GDB 硬件调试的配置 - Main 选项卡

6.  点击 ``Debugger`` 选项卡，在 ``GDB Command`` 栏中输入 ``{IDF_TARGET_TOOLCHAIN_PREFIX}-gdb`` 来调用调试器。

7.  更改 ``Remote host`` 的默认配置，在 ``Port number`` 下面输入 ``3333``。

    上述步骤 6 - 7 的示例输入如下图所示。

    .. figure:: ../../../_static/hw-debugging-debugger-tab.jpg
        :align: center
        :alt: Configuration of GDB Hardware Debugging - Debugger tab
        :figclass: align-center

        GDB 硬件调试的配置 - Debugger 选项卡

8.  最后一个需要更改默认配置的选项卡是 ``Startup`` 选项卡。在 ``Initialization Commands`` 下，取消选中 ``Reset and Delay (seconds)`` 和 ``Halt``，然后在下面一栏中输入以下命令：

    .. code-block:: none

        mon reset halt
        maintenance flush register-cache
        set remote hardware-watchpoint-limit 2

    .. note::

        如果想在启动新的调试会话之前自动更新闪存中的镜像，请在 ``Initialization Commands`` 文本框的开头添加以下命令行:

        .. code-block:: none

            mon reset halt
            mon program_esp ${workspace_loc:blink/build/blink.bin} 0x10000 verify

    有关 ``program_esp`` 命令的说明请参考 :ref:`jtag-upload-app-debug` 章节。

9.  在 ``Load Image and Symbols`` 下，取消选中 ``Load image`` 选项。

10. 在同一个选项卡中继续往下浏览，建立一个初始断点用来在调试器复位后暂停 CPU。插件会根据 ``Set break point at:`` 一栏中输入的函数名，在该函数的开头设置断点。选中这一选项，并在相应的字段中输入 ``app_main``。

11. 选中 ``Resume`` 选项，这会使得程序在每次调用步骤 8 中的 ``mon reset halt`` 后恢复，然后在 ``app_main`` 的断点处停止。

    上述步骤 8 - 11 的示例输入如下图所示。

    .. figure:: ../../../_static/hw-debugging-startup-tab.jpg
        :align: center
        :alt: Configuration of GDB Hardware Debugging - Startup tab
        :figclass: align-center

        GDB 硬件调试的配置 - Startup 选项卡

    上面的启动序列看起来有些复杂，如果你对其中的初始化命令不太熟悉，请查阅 :ref:`jtag-debugging-tip-debugger-startup-commands` 章节获取更多说明。

12. 如果前面已经完成 :ref:`jtag-debugging-configuring-target` 中介绍的步骤，目标正在运行并准备好与调试器进行对话，那么点击 ``Debug`` 按钮直接进行调试。如果尚未完成前面步骤，请点击 ``Apply`` 按钮保存配置，返回 :ref:`jtag-debugging-configuring-target` 章节进行配置，最后再回到这里开始调试。

    一旦所有 1-12 的配置步骤都已经完成，Eclipse 就会打开 ``Debug`` 视图，如下图所示。

    .. figure:: ../../../_static/debug-perspective.jpg
        :align: center
        :alt: Debug Perspective in Eclipse
        :figclass: align-center

        Eclipse 中的调试视图

    如果不太了解 GDB 的常用方法，请查阅 :ref:`jtag-debugging-examples-eclipse` 文章中的调试示例章节 :ref:`jtag-debugging-examples`。


.. _jtag-debugging-using-debugger-command-line:

使用命令行调试
^^^^^^^^^^^^^^^^

1.  为了能够启动调试会话，需要先启动并运行目标，如果还没有完成，请按照 :ref:`jtag-debugging-configuring-target` 中的介绍进行操作。

.. highlight:: bash

2.  打开一个新的终端会话并前往待调试的项目目录，比如：

    .. code-block:: none

        cd ~/esp/blink

.. highlight:: none

3.  启动调试器时，通常需要提供一些配置参数和命令。构建系统会生成多个 ``.gdbinit`` 文件，以便进行高效调试。这些文件的路径存储在 ``build/project_description.json`` 文件的 ``gdbinit_files`` 字段部分，具体路径如下所示进行定义：

    .. code-block:: json

        "gdbinit_files": {
            "01_symbols": "application_path/build/gdbinit/symbols",
            "02_prefix_map": "application_path/build/gdbinit/prefix_map",
            "03_py_extensions": "application_path/build/gdbinit/py_extensions",
            "04_connect": "application_path/build/gdbinit/connect"
        }

    按照 JSON 键名中的 ``XX_`` 前缀进行排序，并以这种顺序将字段信息提供给 GDB。

    生成的 ``.gdbinit`` 文件具有以下不同功能：

    * ``symbols`` - 包含用于调试的符号来源。
    * ``prefix_map`` - 配置前缀映射以修改 GDB 中的源路径。详情请参阅 :ref:`reproducible-builds-and-debugging`。
    * ``py_extensions`` - 初始化 GDB 中的 Python 扩展。请注意，应使用包含 ``libpython`` 库、且受 GDB 支持的 Python 版本。若运行以下命令时没有报错，则说明 GDB 和 Python 兼容：``{IDF_TARGET_TOOLCHAIN_PREFIX}-gdb --batch-silent --ex "python import os"``。
    * ``connect`` - 包含与目标设备建立连接时所需的命令。

    为增强调试体验，你还可以创建自定义的 ``.gdbinit`` 文件。自定义文件可以与生成的配置文件一起使用，也可以直替换它们。

.. highlight:: bash

4.  准备启动 GDB。请使用以下示例命令加载符号表并连接目标设备（命令中的 ``-q`` 选项用于减少启动输出）：

    .. code-block:: none

        {IDF_TARGET_TOOLCHAIN_PREFIX}-gdb -q -x build/gdbinit/symbols -x build/gdbinit/prefix_map -x build/gdbinit/connect build/blink.elf

.. highlight:: none

5.  如果前面的步骤已经正确完成，你会看到如下所示的输出日志，在日志的最后会出现 ``(gdb)`` 提示符：

    .. code-block:: none

        {IDF_TARGET_TOOLCHAIN_PREFIX}-gdb -q -x build/gdbinit/symbols -x build/gdbinit/prefix_map -x build/gdbinit/connect build/blink.elf

        user-name@computer-name:~/esp-idf/examples/get-started/blink$ {IDF_TARGET_TOOLCHAIN_PREFIX}-gdb -q -x build/gdbinit/symbols -x build/gdbinit/connect build/blink.elf
        Reading symbols from build/blink.elf...
        add symbol table from file "/home/user-name/esp-idf/examples/get-started/blink/build/bootloader/bootloader.elf"
        [Switching to Thread 1070141764]
        app_main () at /home/user-name/esp-idf/examples/get-started/blink/main/blink_example_main.c:95
        95          configure_led();
        add symbol table from file "/home/alex/.espressif/tools/esp-rom-elfs/20241011/{IDF_TARGET_PATH_NAME}_rev0_rom.elf"
        JTAG tap: {IDF_TARGET_PATH_NAME}.tap0 tap/device found: 0x00005c25 (mfg: 0x612 (Espressif Systems), part: 0x0005, ver: 0x0)
        [{IDF_TARGET_PATH_NAME}] Reset cause (3) - (Software core reset)
        Hardware assisted breakpoint 1 at 0x42009436: file /home/user-name/esp-idf/examples/get-started/blink/main/blink_example_main.c, line 92.
        [Switching to Thread 1070139884]

        Thread 2 "main" hit Temporary breakpoint 1, app_main () at /home/user-name/esp-idf/examples/get-started/blink/main/blink_example_main.c:92
        92      {
        (gdb)


    请注意，以上日志的倒数第三行显示调试器已在 ``build/gdbinit/connect`` 函数的断点处停止，该断点在 ``gdbinit`` 文件中设定。由于处理器已暂停，LED 将不再闪烁。如果你的 LED 也停止了闪烁，则可以开始调试。

    如果不太了解 GDB 的常用方法，请查阅 :ref:`jtag-debugging-examples-command-line` 文章中的调试示例章节 :ref:`jtag-debugging-examples`。


.. _jtag-debugging-with-idf-py:

使用 idf.py 进行调试
^^^^^^^^^^^^^^^^^^^^

你还可以使用 ``idf.py`` 更方便地执行上述提到的调试命令，可以使用以下命令：

1.  ``idf.py openocd``

    在终端中运行 OpenOCD，其配置信息来源于环境变量或者命令行。默认会使用 ``OPENOCD_SCRIPTS`` 环境变量中指定的脚本路径，它是由 ESP-IDF 项目仓库中的导出脚本（``export.sh`` or ``export.bat``）添加到系统环境变量中的。
    当然，你可以在命令行中通过  ``--openocd-scripts`` 参数来覆盖这个变量的值。

    .. include:: {IDF_TARGET_PATH_NAME}.inc
        :start-after: idf-py-openocd-default-cfg
        :end-before: ---

    至于当前开发板的 JTAG 配置，请使用环境变量 ``OPENOCD_COMMANDS`` 或命令行参数 ``--openocd-commands``。如果这两者都没有被定义，那么 OpenOCD 会使用 |idf-py-def-cfg| 参数来启动。


2.  ``idf.py gdb``

    根据当前项目的 ELF 文件自动生成 GDB 启动脚本，然后按照 :ref:`jtag-debugging-using-debugger-command-line` 中描述的步骤启动 GDB。详情请参阅 :ref:`jtag-debugging-using-debugger-command-line`。


3.  ``idf.py gdbtui``

    和步骤 2 相同，但是会在启动 GDB 的时候传递 ``tui`` 参数，这样可以方便在调试过程中查看源代码。


4.  ``idf.py gdbgui``

    启动 `gdbgui <https://www.gdbgui.com>`_，在浏览器中打开调试器的前端界面。请在运行安装脚本时添加 "--enable-gdbgui" 参数，即运行 ``install.sh --enable-gdbgui``，从而确保支持 ``gdbgui`` 选项。


    上述这些命令也可以合并到一起使用，``idf.py`` 会自动将后台进程（比如 openocd）最先运行，交互式进程（比如 GDB，monitor）最后运行。

    常用的组合命令如下所示：

    .. code-block:: none

        idf.py openocd gdbgui monitor


    上述命令会将 OpenOCD 运行至后台，然后启动 `gdbgui <https://www.gdbgui.com>`_ 打开一个浏览器窗口，显示调试器的前端界面，最后在活动终端打开串口监视器。
