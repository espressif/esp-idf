JTAG 调试
=========
:link_to_translation:`en:[English]`

本文将指导安装 {IDF_TARGET_NAME} 的 OpenOCD 调试环境，并介绍如何使用 GDB 来调试 {IDF_TARGET_NAME} 的应用程序。本文的组织结构如下：

:ref:`jtag-debugging-introduction`
    介绍本指南主旨。
:ref:`jtag-debugging-how-it-works`
    介绍 {IDF_TARGET_NAME}，JTAG（Joint Test Action Group）接口，OpenOCD 和 GDB 是如何相互连接从而实现 {IDF_TARGET_NAME} 的调试功能。
:ref:`jtag-debugging-selecting-jtag-adapter`
    介绍有关 JTAG 硬件适配器的选择及参照标准。
:ref:`jtag-debugging-setup-openocd`
    介绍如何安装官方预编译好的 OpenOCD 软件包并验证是否安装成功。
:ref:`jtag-debugging-configuring-target`
    介绍如何设置 OpenOCD 软件并安装 JTAG 硬件适配器，这两者共同组成最终的调试目标。
:ref:`jtag-debugging-launching-debugger`
    介绍如何从 :ref:`Eclipse 集成开发环境 <jtag-debugging-using-debugger-eclipse>` 和 :ref:`命令行终端 <jtag-debugging-using-debugger-command-line>` 启动 GDB 调试会话。
:ref:`jtag-debugging-examples`
    如果你对 GDB 不太熟悉，本小节会分别针对 :ref:`Eclipse 集成开发环境 <jtag-debugging-examples-eclipse>` 和 :ref:`命令行终端 <jtag-debugging-examples-command-line>` 来讲解调试的范例。
:ref:`jtag-debugging-building-openocd`
    介绍如何在 :doc:`Windows <building-openocd-windows>`，:doc:`Linux <building-openocd-linux>` 和 :doc:`MacOS <building-openocd-macos>` 操作系统上从源码构建 OpenOCD。
:ref:`jtag-debugging-tips-and-quirks`
    介绍使用 OpenOCD 和 GDB 通过 JTAG 接口调试 {IDF_TARGET_NAME} 时的注意事项和补充内容。


.. include:: {IDF_TARGET_TOOLCHAIN_NAME}.inc
   :start-after: devkit-defs
   :end-before: ---


.. _jtag-debugging-introduction:

引言
----

.. only:: esp32

    ESP32 具有两个强大的 Xtensa 内核，支持多种程序架构。ESP-IDF 自带的 FreeRTOS 操作系统具有多核抢占式多线程的功能，它允许用户以更加直观的方式编写软件。

    与此相对地，简便的编程方式会给程序的调试带来困难（如果没有合适的工具），比如找出由两个线程引起的错误，并且这两个线程在单独的 CPU 核上同时运行，仅凭 ``printf`` 语句会花费很长的时间来定位到该错误。在大多数情况下，调试此类问题更快的方法是使用调试器，连接到处理器的调试端口。

乐鑫已经为 {IDF_TARGET_NAME} 处理器和多核 FreeRTOS 架构移植好了 OpenOCD，它将成为大多数 {IDF_TARGET_NAME} 应用程序的基础。此外，乐鑫还提供了一些 OpenOCD 本身并不支持的工具来进一步丰富调试的功能。

本文将指导如何在 Linux，Windows 和 MacOS 环境下为 {IDF_TARGET_NAME} 安装 OpenOCD，并使用 GDB 进行软件调试。除了个别操作系统的安装过程有所差别以外，软件用户界面和使用流程都是一样的。

.. note::
    本文使用的图片素材来自于 Ubuntu 16.04 LTS 上 Eclipse Neon 3 软件的截图，不同的操作系统（Windows， MacOS 或者 Linux）和 Eclipse 软件版本在用户界面上可能会有细微的差别。

.. _jtag-debugging-how-it-works:

工作原理
--------

通过 JTAG（Joint Test Action Group）接口使用 OpenOCD 调试 {IDF_TARGET_NAME} 时所需要的一些关键的软件和硬件包括 **xtensa-{IDF_TARGET_TOOLCHAIN_NAME}-elf-gdb
调试器**，**OpenOCD 片上调试器** 和连接到 **{IDF_TARGET_NAME}** 目标的 **JTAG 适配器**。

.. figure:: ../../../_static/jtag-debugging-overview_zh.jpg
    :align: center
    :alt: JTAG debugging - overview diagram
    :figclass: align-center

    JTAG 调试 - 概述图

在 “Application Loading and Monitoring” 下还有另外一组软件和硬件，它们用来编译、构建和烧写应用程序到 {IDF_TARGET_NAME} 上，以及监视来自 {IDF_TARGET_NAME} 的运行诊断信息。

`Eclipse <https://www.eclipse.org/>`__ 环境集成了 JTAG 调试和应用程序加载、监视的功能，它使得软件从编写、编译、加载到调试的迭代过程变得更加快速而简单。所有的软件均适用于 Windows，Linux 和 MacOS 平台。

如果你使用的是 |devkit-name-with-link|，得益于板载的 FT232H 芯片，PC 和 {IDF_TARGET_NAME} 的连接仅仅需要一根 USB 线即可完成。FT232H 提供了两路 USB 通道，一路连接到 JTAG，另一路连接到 UART。

根据用户的喜好，除了使用 Eclipse 集成开发环境，还可以直接在命令行终端运行 `debugger` 和 `idf.py build`。

.. _jtag-debugging-selecting-jtag-adapter:

选择 JTAG 适配器
----------------

上手 JTAG 最快速便捷的方式是使用 |devkit-name-with-link|，因为它板载了 JTAG 调试接口，无需使用外部的 JTAG 硬件适配器和额外的线缆来连接 JTAG 与 {IDF_TARGET_NAME}。|devkit-name| 采用 FT2232H 提供的 JTAG 接口，可以稳定运行在 20 MHz 的时钟频率，外接的适配器很难达到这个速度。

如果你想使用单独的 JTAG 适配器，请确保其与 {IDF_TARGET_NAME} 的电平电压和 OpenOCD 软件都兼容。{IDF_TARGET_NAME} 使用的是业界标准的 JTAG 接口，它省略了（实际上也并不需要）TRST 信号脚。JTAG 使用的 IO 引脚由 VDD_3P3_RTC 电源引脚供电（通常连接到外部 3.3 V 的电源轨），因此 JTAG 硬件适配器的引脚需要能够在该电压范围内正常工作。

在软件方面，OpenOCD 支持相当多数量的 JTAG 适配器，可以参阅 `OpenOCD 支持的适配器列表 <http://openocd.org/doc/html/Debug-Adapter-Hardware.html>`_ （尽管上面显示的器件不太完整），这个页面还列出了兼容 SWD 接口的适配器，但是请注意，{IDF_TARGET_NAME} 目前并不支持 SWD。此外那些被硬编码为只支持特定产品线的 JTAG 适配器也不能在 {IDF_TARGET_NAME} 上工作，比如用于 STM32 产品家族的 ST-LINK 适配器。

JTAG 正常工作至少需要连接的信号线有：TDI，TDO，TCK，TMS 和 GND。某些 JTAG 适配器还需要 {IDF_TARGET_NAME} 提供一路电源到适配器的某个引脚上（比如 Vtar）用以设置适配器的工作电压。SRST 信号线是可选的，它可以连接到 {IDF_TARGET_NAME} 的 CH_PD 引脚上，尽管目前 OpenOCD 对该信号线的支持还非常有限。


.. _jtag-debugging-setup-openocd:

安装 OpenOCD
------------

.. highlight:: bash

如果你已经按照 :doc:`快速入门 <../../get-started/index>` 一文中的介绍安装好了 ESP-IDF 及其 CMake 构建系统，那么 OpenOCD 已经被默认安装到了你的开发系统中。在 :ref:`设置开发环境 <get-started-set-up-env>` 结束后，你应该能够在终端中运行如下 OpenOCD 命令::

    openocd --version

.. highlight:: none

终端会输出以下信息（实际版本号可能会比这里列出的更新）::

    Open On-Chip Debugger  v0.10.0-esp32-20190708 (2019-07-08-11:04)
    Licensed under GNU GPL v2
    For bug reports, read
        http://openocd.org/doc/doxygen/bugs.html

你还可以检查 ``OPENOCD_SCRIPTS`` 环境变量的值来确认 OpenOCD 配置文件的路径，Linux 和 macOS 用户可以在终端输入 ``echo $OPENOCD_SCRIPTS``，Windows 用户需要输入 ``echo %OPENOCD_SCRIPTS%``。如果终端打印了有效的路径，则表明 OpenOCD 已经被正确安装。

如果上述步骤没有成功执行，请返回快速入门手册，参考其中 :ref:`设置安装工具 <get-started-set-up-tools>` 章节的说明。

.. note::

    另外，我们还可以从源代码编译 OpenOCD 工具，相关详细信息请参阅 :ref:`jtag-debugging-building-openocd` 章节。


.. _jtag-debugging-configuring-target:

配置 {IDF_TARGET_NAME} 目标板
----------------------------

安装好 OpenOCD 之后就可以配置 {IDF_TARGET_NAME} 目标（即带 JTAG 接口的 {IDF_TARGET_NAME} 板），具体可以通过以下三个步骤进行：

-  配置并连接 JTAG 接口
-  运行 OpenOCD
-  上传待调试的应用程序


配置并连接 JTAG 接口
~~~~~~~~~~~~~~~~~~~~

此步骤取决于您使用的 JTAG 和 {IDF_TARGET_NAME} 板，请参考以下两种情况。

.. toctree::
    :maxdepth: 1

    configure-ft2232h-jtag
    configure-other-jtag


.. _jtag-debugging-run-openocd:

运行 OpenOCD
~~~~~~~~~~~~

配置完目标并将其连接到电脑后，即可启动 OpenOCD。

.. highlight:: bash

打开终端，按照快速入门中的指南 :ref:`设置好开发环境 <get-started-set-up-env>` ，然后运行如下命令，启动 OpenOCD（该命令在 Windows，Linux，和 macOS 中通用）:

.. include:: {IDF_TARGET_TOOLCHAIN_NAME}.inc
    :start-after: run-openocd
    :end-before: ---

.. note::

    上述命令中 ``-f`` 选项后跟的配置文件专用于 |run-openocd-device-name|。您可能需要根据具体使用的硬件而选择或修改不同的配置文件，相关指导请参阅 :ref:`jtag-debugging-tip-openocd-configure-target`。

.. highlight:: none

现在应该可以看到如下输入（此日志来自 |run-openocd-device-name|）:

.. include:: {IDF_TARGET_TOOLCHAIN_NAME}.inc
   :start-after: run-openocd-output
   :end-before: ---

-  如果出现指示权限问题的错误，请参阅 ``~/esp/openocd-esp32`` 目录下 OpenOCD README 文件中关于 “Permissions delegation” 的说明。
-  如果发现配置文件有错误，例如 |run-openocd-cfg-file-err|，请检查 ``-s`` 后面的路径，OpenOCD 会根据此路径来查找 ``-f`` 指定的文件。此外，还需要检查配置文件是否确实位于该路径下。
-  如果看到 JTAG 错误（输出全是 1 或者全是 0），请检查硬件连接，除了 {IDF_TARGET_NAME} 的引脚之外是否还有其他信号连接到了 JTAG，并查看是否所有器件都已经上电。


.. _jtag-upload-app-debug:

上传待调试的应用程序
~~~~~~~~~~~~~~~~~~~~

您可以像往常一样构建并上传 {IDF_TARGET_NAME} 应用程序，具体请参阅 :ref:`get-started-build` 章节。

除此以外，还支持使用 OpenOCD 通过 JTAG 接口将应用程序镜像烧写到闪存中，命令如下:

.. include:: {IDF_TARGET_TOOLCHAIN_NAME}.inc
   :start-after: run-openocd-upload
   :end-before: ---

其中 OpenOCD 的烧写命令 ``program_esp`` 具有以下格式：

``program_esp <image_file> <offset> [verify] [reset] [exit]``

    -  ``image_file`` - 程序镜像文件存放的路径
    -  ``offset`` - 镜像烧写到闪存中的偏移地址
    -  ``verify`` - 烧写完成后校验闪存中的内容（可选）
    -  ``reset`` - 烧写完成后重启目标（可选）
    -  ``exit`` - 烧写完成后退出 OpenOCD（可选）

现在可以进行应用程序的调试了，请按照以下章节中讲解的步骤进行操作。


.. _jtag-debugging-launching-debugger:

启动调试器
----------

{IDF_TARGET_NAME} 的工具链中带有 GNU 调试器（简称 GDB） ``xtensa-{IDF_TARGET_TOOLCHAIN_NAME}-elf-gdb``，它和其它工具链软件存放在同一个 bin 目录下。除了直接在命令行终端中调用并操作 GDB 外，还可以在 IDE (例如 Eclipse，Visual Studio Code 等）中调用它，在图形用户界面的帮助下间接操作 GDB，无需在终端中输入任何命令。

关于以上两种调试器的使用方法，详见以下链接。

* :ref:`jtag-debugging-using-debugger-eclipse`
* :ref:`jtag-debugging-using-debugger-command-line`

建议首先检查调试器是否能在 :ref:`命令行终端 <jtag-debugging-using-debugger-command-line>` 下正常工作，然后再转到使用 Eclipse 等 :ref:`集成开发环境 <jtag-debugging-using-debugger-eclipse>` 下进行调试工作。


.. _jtag-debugging-examples:

调试范例
--------

本节适用于不熟悉 GDB 的用户，将使用 :example:`get-started/blink` 下简单的应用程序来演示 :ref:`调试会话的工作流程 <jtag-debugging-examples-eclipse>`，同时会介绍以下常用的调试操作：

1. :ref:`jtag-debugging-examples-eclipse-01`
2. :ref:`jtag-debugging-examples-eclipse-02`
3. :ref:`jtag-debugging-examples-eclipse-03`
4. :ref:`jtag-debugging-examples-eclipse-04`
5. :ref:`jtag-debugging-examples-eclipse-05`
6. :ref:`jtag-debugging-examples-eclipse-06`
7. :ref:`jtag-debugging-examples-eclipse-07`

此外还会提供 :ref:`在命令行终端进行调试 <jtag-debugging-examples-command-line>` 的案例。

在演示之前，请设置好 {IDF_TARGET_NAME} 目标板并加载 :example:`get-started/blink` 至 {IDF_TARGET_NAME} 中。


.. _jtag-debugging-building-openocd:

从源码构建 OpenOCD
------------------

请参阅以下文档，它们分别介绍了在各大操作系统平台上从源码构建 OpenOCD 的流程。

.. toctree::
    :maxdepth: 1

    Windows <building-openocd-windows>
    Linux <building-openocd-linux>
    MacOS <building-openocd-macos>


本文档演示所使用的 OpenOCD 是 :ref:`jtag-debugging-setup-openocd` 章节中介绍的预编译好的二进制发行版。

.. highlight:: bash

如果要使用本地从源代码编译的 OpenOCD 程序，需要将相应可执行文件的路径修改为 ``src/openocd``，并设置 ``OPENOCD_SCRIPTS`` 环境变量，这样 OpenOCD 才能找到配置文件。Linux 和 macOS 用户可以执行:

.. code-block:: bash

    cd ~/esp/openocd-esp32
    export OPENOCD_SCRIPTS=$PWD/tcl

Windows 用户可以执行:

.. code-block:: batch

    cd %USERPROFILE%\esp\openocd-esp32
    set "OPENOCD_SCRIPTS=%CD%\tcl"

运行本地编译的 OpenOCD 的示例如下（Linux 和 macOS 用户）:

.. include:: {IDF_TARGET_TOOLCHAIN_NAME}.inc
   :start-after: run-openocd-src-linux
   :end-before: ---

Windows 用户:

.. include:: {IDF_TARGET_TOOLCHAIN_NAME}.inc
   :start-after: run-openocd-src-win
   :end-before: ---


.. _jtag-debugging-tips-and-quirks:

注意事项和补充内容
--------------------

本节列出了本指南中提到的所有注意事项和补充内容的链接。

.. toctree::
    :maxdepth: 2

    tips-and-quirks


相关文档
--------

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