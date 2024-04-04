JTAG 调试
=========

:link_to_translation:`en:[English]`

本文将介绍如何安装 {IDF_TARGET_NAME} 的 OpenOCD 调试环境，以及如何使用 GDB 来调试 {IDF_TARGET_NAME} 的应用程序。

.. note::

    也可以使用 ``idf.py monitor`` 来调试 {IDF_TARGET_NAME}，免于设置 JTAG 或 OpenOCD。请参阅 :doc:`../../api-guides/tools/idf-monitor` 和 :ref:`CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME`。

.. highlight:: none

本文档结构如下：

:ref:`jtag-debugging-introduction`
    介绍本指南主旨。
:ref:`jtag-debugging-how-it-works`
    介绍 {IDF_TARGET_NAME}、JTAG (Joint Test Action Group) 接口、OpenOCD 和 GDB 如何相互连接，从而实现 {IDF_TARGET_NAME} 的调试功能。
:ref:`jtag-debugging-selecting-jtag-adapter`
    介绍有关 JTAG 硬件适配器的选择及参照标准。
:ref:`jtag-debugging-setup-openocd`
    介绍如何安装官方预编译好的 OpenOCD 软件包并验证是否安装成功。
:ref:`jtag-debugging-configuring-target`
    介绍如何设置 OpenOCD 软件并安装 JTAG 硬件，两项共同构成调试目标.
:ref:`jtag-debugging-launching-debugger`
    介绍如何从 :ref:`Eclipse 集成开发环境 <jtag-debugging-using-debugger-eclipse>` 和 :ref:`命令行终端 <jtag-debugging-using-debugger-command-line>` 启动 GDB 调试会话。
:ref:`jtag-debugging-examples`
    如果你不熟悉 GDB，请查看此小节以获取 :ref:`Eclipse 集成开发环境 <jtag-debugging-examples-eclipse>` 以及 :ref:`命令行终端 <jtag-debugging-examples-command-line>` 提供的调试示例。
:ref:`jtag-debugging-building-openocd`
    介绍如何在 :doc:`Windows <building-openocd-windows>`，:doc:`Linux <building-openocd-linux>` 和 :doc:`macOS <building-openocd-macos>` 操作系统上从源码构建 OpenOCD。
:ref:`jtag-debugging-tips-and-quirks`
    介绍使用 OpenOCD 和 GDB 通过 JTAG 接口调试 {IDF_TARGET_NAME} 时的注意事项和补充内容。


.. include:: {IDF_TARGET_PATH_NAME}.inc
   :start-after: devkit-defs
   :end-before: ---


.. _jtag-debugging-introduction:

引言
----

.. only:: esp32

    ESP32 具有两个强大的 Xtensa 内核，支持多种程序架构。ESP-IDF 自带的 FreeRTOS 操作系统支持多核抢占式调度，允许用户以更加直观的方式编写软件。

    与此相对地，由于缺乏合适的工具，简便的编程方式也会给程序的调试带来困难，比如找出由两个线程引起的错误，并且这两个线程在单独的 CPU 核上同时运行，那么仅凭 ``printf`` 语句会花费很长时间来定位该错误。调试此类问题更好（往往也更快）的方法是使用调试器，将其连接到处理器的调试端口。

乐鑫已完成 OpenOCD 移植，以支持 {IDF_TARGET_NAME} 处理器和多核 FreeRTOS 架构（大多数 {IDF_TARGET_NAME} 应用程序的基础）。此外，乐鑫还提供了一些 OpenOCD 本身并不支持的工具，以进一步丰富调试功能。

本文将介绍如何在 Linux、Windows 和 macOS 环境下为 {IDF_TARGET_NAME} 安装 OpenOCD，并使用 GDB 进行软件调试。除部分安装流程有所不同外，所有操作系统的软件用户界面和使用流程都是相同的。

.. note::

    本文使用的图片素材来自于 Ubuntu 16.04 LTS 上 Eclipse Neon 3 软件的截图，不同的操作系统（Windows、macOS 或 Linux）或不同的 Eclipse 软件版本在用户界面上可能会有细微差别。

.. _jtag-debugging-how-it-works:

工作原理
--------

通过 JTAG (Joint Test Action Group) 接口使用 OpenOCD 调试 {IDF_TARGET_NAME} 时所需要的关键软件和硬件包括 **{IDF_TARGET_TOOLCHAIN_PREFIX}-gdb 调试器**、**OpenOCD 片上调试器** 和连接到 **{IDF_TARGET_NAME}** 目标的 **JTAG 适配器**，如下图 “Application Loading and Monitoring” 标志所示。

.. figure:: ../../../_static/jtag-debugging-overview_zh.jpg
    :align: center
    :alt: JTAG debugging - overview diagram
    :figclass: align-center

    JTAG 调试 - 概述图

“Application Loading and Monitoring” 标志显示一组关键的软件和硬件组件，可用于编译、构建和烧写应用程序到 {IDF_TARGET_NAME} 上，以及监视来自 {IDF_TARGET_NAME} 的运行诊断信息。

`Eclipse <https://www.eclipse.org/>`__ 环境集成了 JTAG 调试和应用程序加载、监视的功能，使得软件从编写、编译、加载到调试的迭代过程变得更加快速简单。Eclipse IDE 及其集成的调试软件均适用于 Windows、Linux 和 macOS 平台。根据用户喜好，除了使用 Eclipse 集成开发环境，还可以直接在命令行终端运行 `debugger` 和 `idf.py build`。

.. only:: esp32 or esp32s2

    若使用 |devkit-name-with-link|，由于其板载 FT232H 芯片，仅需一根 USB 线即可连接 PC 与 {IDF_TARGET_NAME}。FT232H 提供了两路 USB 通道，一路连接到 JTAG，另一路连接到 UART。

.. only:: SOC_USB_SERIAL_JTAG_SUPPORTED

    仅需一根 USB 线即可高效连接 PC 与 {IDF_TARGET_NAME}，因为 {IDF_TARGET_NAME} 芯片本身提供了两路 USB 通道，一路连接到 JTAG，另一路连接到 USB 终端。应将 USB 线连接到 {IDF_TARGET_NAME} 的 D+/D- USB 管脚，而非通过 USB-UART 芯片连接到串行 RxD/TxD。后文中 :ref:`jtag-debugging-configuring-target` 小节将对此进行解释。

.. only:: esp32c3

    .. note::

        {IDF_TARGET_NAME} 的 USB 接口仅适用于调试版本为 0.3 或更高版本的芯片，对于版本为 0.1 或 0.2 的芯片，请使用其他调试方法（例如 ESP-Prog）。要确定芯片版本，可以在 boot log 开头查看类似 ``boot: chip revision: v0.3`` 的信息。更多详情，请参考 ESP32-C3 系列芯片勘误表 <https://www.espressif.com/sites/default/files/documentation/esp32-c3_errata_cn.pdf>`_。

.. _jtag-debugging-selecting-jtag-adapter:

选择 JTAG 适配器
----------------

.. only:: esp32 or esp32s2

    上手 JTAG 最快速便捷的方式是使用 |devkit-name-with-link|，因为它板载了 JTAG 调试接口，无需使用外部 JTAG 硬件适配器和额外线缆来连接 JTAG 与 {IDF_TARGET_NAME}。|devkit-name| 采用 FT2232H 提供的 JTAG 接口，可以稳定运行在 20 MHz 的时钟频率，外接的适配器很难达到这个速度。

.. only:: SOC_USB_SERIAL_JTAG_SUPPORTED

    上手 JTAG 最快速便捷的方式是将一根 USB 线连接到 {IDF_TARGET_NAME} 的 D+/D- USB 管脚，无需使用外部 JTAG 适配器和额外线缆。

如果想使用单独的 JTAG 适配器，请确保其与 {IDF_TARGET_NAME} 的电平电压和 OpenOCD 软件都兼容。{IDF_TARGET_NAME} 使用的是业界标准的 JTAG 接口，它未使用（实际上也并不需要）TRST 信号脚。JTAG 使用的 IO 管脚由 VDD_3P3_RTC 电源管脚供电（通常连接到外部 3.3 V 的电源轨），因此 JTAG 硬件适配器的管脚需要能够在该电压范围内正常工作。

在软件方面，OpenOCD 支持相当多数量的 JTAG 适配器，请参阅 `OpenOCD 支持的适配器列表 <https://openocd.org/doc/html/Debug-Adapter-Hardware.html>`_ （请注意这一列表并不完整），其中还列出了兼容 SWD 接口的适配器，但请注意，{IDF_TARGET_NAME} 目前并不支持 SWD。此外，硬编码为只支持特定产品线的 JTAG 适配器也无法在 {IDF_TARGET_NAME} 上工作，例如仅针对 STM32 系列产品的 ST-LINK 适配器。

保证 JTAG 正常工作需要连接的信号线包括：TDI、TDO、TCK、TMS 和 GND。一些 JTAG 适配器还需要 {IDF_TARGET_NAME} 提供一路电源到适配器的某个管脚上（比如 Vtar），用于设置适配器的工作电压。也可以选择将 SRST 信号线连接到 {IDF_TARGET_NAME} 的 CH_PD 管脚上，但请注意，目前 OpenOCD 对该信号线提供的支持相当有限。

`ESP-Prog <https://docs.espressif.com/projects/espressif-esp-iot-solution/en/latest/hw-reference/ESP-Prog_guide.html>`_ 中展示了使用外部电路板进行调试的实例，方法是将其连接到 {IDF_TARGET_NAME} 的 JTAG 管脚上。

.. _jtag-debugging-setup-openocd:

安装 OpenOCD
------------

.. highlight:: bash

如果已经按照 :doc:`快速入门 <../../get-started/index>` 完成了 ESP-IDF 及其 CMake 构建系统的安装，那么 OpenOCD 已经被默认安装到了你的开发系统中。在 :ref:`设置开发环境 <get-started-set-up-env>` 结束后，应该能够在终端中运行如下 OpenOCD 命令::

    openocd --version

.. highlight:: none

终端会输出以下信息（实际版本号可能会更新）::

    Open On-Chip Debugger v0.12.0-esp32-20240318 (2024-03-18-18:25)
    Licensed under GNU GPL v2
    For bug reports, read
            http://openocd.org/doc/doxygen/bugs.html

你还可以检查 ``OPENOCD_SCRIPTS`` 环境变量的值，以确认 OpenOCD 配置文件的路径，Linux 和 macOS 用户可以在终端输入 ``echo $OPENOCD_SCRIPTS``，Windows 用户需要输入 ``echo %OPENOCD_SCRIPTS%``。如果终端输出了有效路径，则表明已经正确安装 OpenOCD。

如果无法执行上述步骤，请再次阅读快速入门手册，Linux 和 macOS 用户请参考 :ref:`设置安装工具 <get-started-set-up-tools>` 章节，Windows 用户请参考 :ref:`ESP-IDF 工具安装器 <get-started-windows-tools-installer>`。

.. note::

    另外也可以从源代码编译 OpenOCD 工具，详细信息请参阅 :ref:`jtag-debugging-building-openocd` 章节。

.. _jtag-debugging-configuring-target:

配置 {IDF_TARGET_NAME} 目标板
----------------------------

OpenOCD 安装完成后就可以配置 {IDF_TARGET_NAME} 目标（即带 JTAG 接口的 {IDF_TARGET_NAME} 板），具体分为以下三个步骤：

* :ref:`jtag-debugging-configure-and-connect-JTAG-interface`
* :ref:`jtag-debugging-run-openocd`
* :ref:`jtag-upload-app-debug`

.. _jtag-debugging-configure-and-connect-JTAG-interface:

配置并连接 JTAG 接口
~~~~~~~~~~~~~~~~~~~~

此步骤取决于使用的 JTAG 和 {IDF_TARGET_NAME} 板，请参考以下两种情况。

.. toctree::
    :maxdepth: 1

    :esp32: configure-ft2232h-jtag
    :esp32s2: configure-ft2232h-jtag
    :SOC_USB_SERIAL_JTAG_SUPPORTED: configure-builtin-jtag
    configure-other-jtag


.. _jtag-debugging-run-openocd:

运行 OpenOCD
~~~~~~~~~~~~

配置完目标并将其连接到电脑后，即可启动 OpenOCD。

.. highlight:: bash

打开终端，按照快速入门指南中的 :ref:`设置好开发环境 <get-started-set-up-env>` 章节进行操作，然后运行如下命令，以启动 OpenOCD（该命令适用于 Windows、Linux 和 macOS）:

.. include:: {IDF_TARGET_PATH_NAME}.inc
    :start-after: run-openocd
    :end-before: ---

{IDF_TARGET_FTDI_CONFIG:default="Not Updated!", esp32s3="board/esp32s3-ftdi.cfg", esp32c3="board/esp32c3-ftdi.cfg", esp32c6="board/esp32c6-ftdi.cfg", esp32h2="board/esp32h2-ftdi.cfg"}

.. note::

    上述命令中 ``-f`` 选项后跟的配置文件专用于 |run-openocd-device-name|。基于具体使用的硬件，你可能需要选择不同的配置文件，具体内容请参阅 :ref:`jtag-debugging-tip-openocd-configure-target`。

    .. only:: SOC_USB_SERIAL_JTAG_SUPPORTED

        例如，对于带有用于 JTAG 连接的 FT2232H 或 FT232H 芯片的定制板，或带有 ESP-Prog 的定制板，可使用 ``{IDF_TARGET_FTDI_CONFIG}``。

.. highlight:: none

现在应该可以看到如下输出（此日志来自 |run-openocd-device-name|）:

.. include:: {IDF_TARGET_PATH_NAME}.inc
   :start-after: run-openocd-output
   :end-before: ---

*  如果出现指示权限问题的错误，请打开 ``~/esp/openocd-esp32`` 目录，参阅 OpenOCD README 文件中关于 “Permissions delegation” 的说明。
*  如果遇到无法找到配置文件的错误，例如 |run-openocd-cfg-file-err|，请检查 ``OPENOCD_SCRIPTS`` 环境变量是否设置正确，OpenOCD 根据此变量来查找 ``-f`` 指定的文件，详见 :ref:`jtag-debugging-setup-openocd`。此外，还需要检查配置文件是否确实位于该路径下。
*  如果出现 JTAG 错误（例如输出为 ``...all ones`` 或 ``...all zeroes``），请检查硬件连接是否正确，除了 {IDF_TARGET_NAME} 的管脚之外是否还有其他信号连接到了 JTAG，并查看是否所有器件都已经上电。

.. _jtag-upload-app-debug:

上传待调试的应用程序
~~~~~~~~~~~~~~~~~~~~

按照正常步骤构建并上传 {IDF_TARGET_NAME} 应用程序，具体请参阅 :ref:`get-started-build` 章节。

除此以外，还可以使用 OpenOCD 通过 JTAG 接口将应用程序镜像烧写到 flash 中，命令如下:

.. include:: {IDF_TARGET_PATH_NAME}.inc
   :start-after: run-openocd-upload
   :end-before: ---

其中 OpenOCD 的烧写命令 ``program_esp`` 格式如下：

``program_esp <image_file> <offset> [verify] [reset] [exit] [compress] [encrypt]``

-  ``image_file`` - 程序镜像文件存放的路径
-  ``offset`` - 镜像烧写到 flash 中的偏移地址
-  ``verify`` - 烧写完成后校验 flash 中的内容（可选）
-  ``reset`` - 烧写完成后重启目标（可选）
-  ``exit`` - 烧写完成后退出 OpenOCD（可选）
- ``compress`` - 烧写开始前压缩镜像文件（可选）
- ``encrypt`` - 烧写到 flash 前加密二进制文件，与 ``idf.py encrypted-flash`` 功能相同（可选）

现在可以调试应用程序了，请按照以下章节中的步骤进行操作。


.. _jtag-debugging-launching-debugger:

启动调试器
----------

{IDF_TARGET_NAME} 的工具链中带有 GNU 调试器（简称 GDB），它和其它工具链软件共同存放于 {IDF_TARGET_TOOLCHAIN_PREFIX}-gdb 中。除了直接在命令行终端中调用并操作 GDB 外，也可以在 IDE（例如 Eclipse、Visual Studio Code 等）中进行调用，使用图形用户界面间接操作 GDB，这一方法无需在终端中输入任何命令。

关于调试器的使用方法，详见以下链接。

* :ref:`jtag-debugging-using-debugger-eclipse`
* :ref:`jtag-debugging-using-debugger-command-line`
* `使用 VS Code 调试 <https://github.com/espressif/vscode-esp-idf-extension/blob/master/docs/DEBUGGING.md>`__

建议首先检查调试器能否在 :ref:`命令行终端 <jtag-debugging-using-debugger-command-line>` 下正常工作，然后再使用 Eclipse :ref:`集成开发环境 <jtag-debugging-using-debugger-eclipse>` 进行调试工作。


.. _jtag-debugging-examples:

调试范例
--------

本节适用于不熟悉 GDB 的用户，下文将使用 :example:`get-started/blink` 下简单的应用程序来演示 :ref:`调试会话的工作流程 <jtag-debugging-examples-eclipse>`，同时会介绍以下常用的调试操作：

1. :ref:`jtag-debugging-examples-eclipse-01`
2. :ref:`jtag-debugging-examples-eclipse-02`
3. :ref:`jtag-debugging-examples-eclipse-03`
4. :ref:`jtag-debugging-examples-eclipse-04`
5. :ref:`jtag-debugging-examples-eclipse-05`
6. :ref:`jtag-debugging-examples-eclipse-06`
7. :ref:`jtag-debugging-examples-eclipse-07`

此外还会提供在 :ref:`在命令行终端进行调试 <jtag-debugging-examples-command-line>` 下使用 GDB 调试的案例。

.. note::

    :ref:`jtag-debugging-examples-command-line-08` 目前仅适用于命令行调试。


在演示之前，请完成 {IDF_TARGET_NAME} 目标板设置并加载 :example:`get-started/blink` 至 {IDF_TARGET_NAME} 中。


.. _jtag-debugging-building-openocd:

从源码构建 OpenOCD
------------------

以下文档分别介绍了如何在各操作系统平台上从源码构建 OpenOCD。

.. toctree::
    :maxdepth: 1

    Windows <building-openocd-windows>
    Linux <building-openocd-linux>
    macOS <building-openocd-macos>

本文档在演示中所使用的 OpenOCD 是预编译好的二进制发行版，在 :ref:`jtag-debugging-setup-openocd` 章节中有所介绍。

如果要使用本地从源代码编译的 OpenOCD 程序，需要将相应可执行文件的路径修改为 ``src/openocd``，并设置 ``OPENOCD_SCRIPTS`` 环境变量，使得 OpenOCD 能够找到配置文件。Linux 和 macOS 用户可以执行:

.. code-block:: bash

    cd ~/esp/openocd-esp32
    export OPENOCD_SCRIPTS=$PWD/tcl

Windows 用户可以执行:

.. code-block:: batch

    cd %USERPROFILE%\esp\openocd-esp32
    set "OPENOCD_SCRIPTS=%CD%\tcl"

针对 Linux 和 macOS 用户，运行本地编译的 OpenOCD 的示例:

.. include:: {IDF_TARGET_PATH_NAME}.inc
   :start-after: run-openocd-src-linux
   :end-before: ---

Windows 用户的示例如下:

.. include:: {IDF_TARGET_PATH_NAME}.inc
   :start-after: run-openocd-src-win
   :end-before: ---

.. _jtag-debugging-tips-and-quirks:

注意事项和补充内容
--------------------

本节列出了上文中提到的所有注意事项和补充内容的链接。

.. toctree::
    :maxdepth: 2

    tips-and-quirks


相关文档
------------

.. toctree::
    :hidden:

    :maxdepth: 1

    using-debugger
    debugging-examples
    tips-and-quirks
    ../app_trace

- :doc:`using-debugger`
- :doc:`debugging-examples`
- :doc:`tips-and-quirks`
- :doc:`../app_trace`
- `ESP-Prog 调试板介绍 <https://docs.espressif.com/projects/espressif-esp-iot-solution/zh_CN/latest/hw-reference/ESP-Prog_guide.html>`__
