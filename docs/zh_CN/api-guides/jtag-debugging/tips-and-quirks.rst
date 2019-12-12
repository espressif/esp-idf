注意事项和补充内容
------------------
:link_to_translation:`en:[English]`

本节提供了本指南中各部分提到的一些注意事项和补充内容。

.. _jtag-debugging-tip-breakpoints:

可用的断点和观察点
^^^^^^^^^^^^^^^^^^

ESP32 调试器支持 2 个硬件断点和 64 个软件断点。硬件断点是由 ESP32 芯片内部的逻辑电路实现的，能够设置在代码的任何位置：闪存或者 IRAM 的代码区域。除此以外，OpenOCD 实现了两种软件断点：闪存断点（最多 32 个）和 IRAM 断点（最多 32 个）。目前 GDB 无法在闪存中设置软件断点，因此除非解决此限制，否则这些断点只能由 OpenOCD 模拟为硬件断点。（详细信息可以参阅 :ref:`下面 <jtag-debugging-tip-where-breakpoints>`）。ESP32 还支持 2 个观察点，所以可以观察两个变量的变化或者通过 GDB 命令 ``watch myVariable`` 来读取变量的值。请注意 menuconfig 中的 :ref:`CONFIG_FREERTOS_WATCHPOINT_END_OF_STACK` 选项会使用第二个观察点，如果你想在 OpenOCD 或者 GDB 中再次尝试使用这个观察点，可能不会得到预期的结果。详情请查看 menuconfig 中的帮助文档。


.. _jtag-debugging-tip-where-breakpoints:

关于断点的补充知识
^^^^^^^^^^^^^^^^^^

使用软件闪存模拟部分硬件断点的意思就是当使用 GDB 命令 ``hb myFunction`` 给某个函数设置硬件断点时，如果该函数位于闪存中，并且此时还有可用的硬件断点，那调试器就会使用硬件断点，否则就使用 32 个软件闪存断点中的一个来模拟。这个规则同样适用于 ``b myFunction`` 之类的命令，在这种情况下，GDB 会自己决定该使用哪种类型的断点。如果 ``myFunction`` 位于可写区域（IRAM），那就会使用软件 IRAM 断点，否则就会像处理 ``hb`` 命令一样使用硬件断点或者软件闪存断点。


.. _jtag-debugging-tip-flash-mappings:

闪存映射 vs 软件闪存断点
^^^^^^^^^^^^^^^^^^^^^^^^

为了在闪存中设置或者清除软件断点，OpenOCD 需要知道它们在闪存中的地址。为了完成从 ESP32 的地址空间到闪存地址的转换，OpenOCD 使用闪存中程序代码区域的映射。这些映射被保存在程序映像的头部，位于二进制数据（代码段和数据段）之前，并且特定于写入闪存的每一个应用程序的映像。因此，为了支持软件闪存断点，OpenOCD 需要知道待调试的应用程序映像在闪存中的位置。默认情况下，OpenOCD 会在 0x8000 处读取分区表并使用第一个找到的应用程序映像的映射，但是也可能会存在无法工作的情况，比如分区表不在标准的闪存位置，甚至可能有多个映像：一个出厂映像和两个 OTA 映像，你可能想要调试其中的任意一个。为了涵盖所有可能的调试情况，OpenOCD 支持特殊的命令，用于指定待调试的应用程序映像在闪存中的具体位置。该命令具有以下格式： 

``esp32 appimage_offset <offset>`` 

偏移量应为十六进制格式，如果要恢复默认行为，可以将偏移地址设置为 ``-1`` 。

.. note::

    由于 GDB 在连接 OpenOCD 时仅仅请求一次内存映射，所以可以在 TCL 配置文件中指定该命令，或者通过命令行传递给 OpenOCD。对于后者，命令行示例如下：

    ``openocd -f interface/ftdi/esp32_devkitj_v1.cfg -f board/esp-wroom-32.cfg -c "init; halt; esp32 appimage_offset 0x210000"``

    另外还可以通过 OpenOCD 的 telnet 会话执行该命令，然后再连接 GDB， 不过这种方式似乎没有那么便捷。

.. _jtag-debugging-tip-why-next-works-as-step:

“next” 命令无法跳过子程序的原因
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

当使用 ``next`` 命令单步执行代码时， GDB 会在子程序的前面设置一个断点（两个中可用的一个），这样就可以跳过进入子程序内部的细节。如果这两个断点已经用在代码的其它位置，那么 ``next`` 命令将不起作用。在这种情况下，请删掉一个断点以使其中一个变得可用。当两个断点都已经被使用时，``next`` 命令会像 ``step`` 命令一样工作，调试器就会进入子程序内部。


.. _jtag-debugging-tip-code-options:

OpenOCD 支持的编译时的选项
^^^^^^^^^^^^^^^^^^^^^^^^^^

ESP-IDF 有一些针对 OpenOCD 调试功能的选项可以在编译时进行设置：

* :ref:`CONFIG_ESP32_DEBUG_OCDAWARE` 默认会被使能。如果程序抛出了不可修复或者未处理的异常，并且此时已经连接上了 JTAG 调试器（即 OpenOCD 正在运行），那么 ESP-IDF 将会进入调试器工作模式。
* :ref:`CONFIG_FREERTOS_WATCHPOINT_END_OF_STACK` 默认没有使能。在所有任务堆栈的末尾设置观察点，从 1 号开始索引。这是调试任务堆栈溢出的最准确的方式。

更多有关设置编译时的选项的信息，请参阅 :ref:`idf.py menuconfig <get-started-configure>`。

.. _jtag-debugging-tip-freertos-support:

支持FreeRTOS
^^^^^^^^^^^^

OpenOCD 完全支持 ESP-IDF 自带的 FreeRTOS 操作系统，GDB 会将 FreeRTOS 中的任务当做线程。使用 GDB 命令 ``i threads`` 可以查看所有的线程，使用命令 ``thread n`` 可以切换到某个具体任务的堆栈，其中 ``n`` 是线程的编号。检测 FreeRTOS 的功能可以在配置目标时被禁用。更多详细信息，请参阅 :ref:`jtag-debugging-tip-openocd-configure-target`.


.. _jtag-debugging-tip-code-flash-voltage:

在 OpenOCD 的配置文件中设置 SPI 闪存的工作电压
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ESP32 的 MTDI 引脚是用于 JTAG 通信的四个引脚之一，同时也是 ESP32 的 bootstrapping 引脚。上电时，ESP32 会在 MTDI 引脚上采样二进制电平，据此来设置内部的稳压器，用于给外部的 SPI 闪存芯片供电。如果上电时 MTDI 引脚上的二进制电平为低电平，则稳压器会被设置为 3.3 V；如果 MTDI 引脚为高电平，则稳压器会被设置为 1.8 V。MTDI 引脚通常需要一个上拉电阻或者直接使能内部的弱下拉电阻（详见 `ESP32 系列芯片技术规格书 <https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_cn.pdf>`_ ），具体取决于所使用的 SPI 芯片的类型。但是一旦连接上 JTAG 后，原来用于实现 bootstrapping 功能的上拉或者下拉电阻都会被覆盖掉。 

为了解决这个问题，OpenOCD 的板级配置文件（例如 ESP32-WROOM-32 模组的 ``boards\esp-wroom-32.cfg``）提供了 ``ESP32_FLASH_VOLTAGE`` 参数来设置 ``TDO`` 信号线在空闲状态下的二进制电平，这样就可以减少由于闪存电压不正确而导致的应用程序启动不良的几率。

查看 JTAG 连接的 ESP32 模组的规格书，检查其 SPI 闪存芯片的供电电压值，然后再相应的设置 ``ESP32_FLASH_VOLTAGE``。大多数的 WROOM 模组使用 3.3 V 的闪存芯片，但是 WROVER 模组使用 1.8 V 的闪存芯片。 


.. _jtag-debugging-tip-optimize-jtag-speed:

优化 JTAG 的速度
^^^^^^^^^^^^^^^^

为了实现更高的数据通信速率同时最小化丢包数，建议优化 JTAG 时钟频率的设置，使其达到 JTAG 能稳定运行的最大值。为此，请参考以下建议。

1.  如果 CPU 以 80 MHz 运行，则 JTAG 时钟频率的上限为 20 MHz；如果 CPU 以 160 MHz 或者 240 MHz 运行，则上限为 26 MHz。
2.  根据特定的 JTAG 适配器和连接线缆的长度，你可能需要将 JTAG 的工作频率降低至 20 / 26 MHz 以下。
3.  在某些特殊情况下，如果你看到 DSR/DIR 错误（并且它并不是由 OpenOCD 试图从一个没有物理存储器映射的地址空间读取数据而导致的），请降低 JTAG 的工作频率。
4.  ESP-WROVER-KIT 能够稳定运行在 20 / 26 MHz 频率下。


.. _jtag-debugging-tip-debugger-startup-commands:

调试器的启动命令的含义
^^^^^^^^^^^^^^^^^^^^^^

在启动时，调试器发出一系列命令来复位芯片并使其在特定的代码行停止运行。这个命令序列（如下所示）支持自定义，用户可以选择在最方便合适的代码行开始调试工作。

* ``set remote hardware-watchpoint-limit 2`` — 限制 GDB 仅使用 ESP32 支持的两个硬件观察点。更多详细信息，请查阅 `GDB 配置远程目标 <https://sourceware.org/gdb/onlinedocs/gdb/Remote-Configuration.html>`_ 。
* ``mon reset halt`` — 复位芯片并使 CPU 停止运行。
* ``flushregs`` — monitor (``mon``) 命令无法通知 GDB 目标状态已经更改，GDB 会假设在 ``mon reset halt`` 之前所有的任务堆栈仍然有效。实际上，复位后目标状态将发生变化。执行 ``flushregs`` 是一种强制 GDB 从目标获取最新状态的方法。
* ``thb app_main`` — 在 ``app_main`` 处插入一个临时的硬件断点，如果有需要，可以将其替换为其他函数名。
* ``c`` — 恢复程序运行，它将会在 ``app_main`` 的断点处停止运行。


.. _jtag-debugging-tip-openocd-configure-target:

针对特定目标的 OpenOCD 配置
^^^^^^^^^^^^^^^^^^^^^^^^^^^

OpenOCD 需要知道当前使用的 JTAG 适配器的类型，以及其连接的目标板和处理器的类型。为此，请使用位于 OpenOCD 安装目录下 ``share/openocd/scripts/interface`` 和 ``share/openocd/scripts/board`` 文件夹中现有的配置文件。

例如，如果使用板载 ESP-WROOM-32 模组的 ESP-WROVER-KIT 开发板（详见 :ref:`esp-modules-and-boards-esp-wrover-kit-v1`），请使用以下配置文件：

* ``interface/ftdi/esp32_devkitj_v1.cfg``
* ``board/esp-wroom-32.cfg``

当然也可以使用自定义的配置文件，建议在已有配置文件的基础上进行修改，以匹配你的硬件。下面列举一些常用的板级配置参数。


.. highlight:: none

适配器的时钟速度
""""""""""""""""

::

    adapter_khz 20000

请参阅 :ref:`jtag-debugging-tip-optimize-jtag-speed` 以获取有关如何设置此值的指导。


单核调试
""""""""

::

    set ESP32_ONLYCPU 1

如果是双核调试，请注释掉这一行。


禁用 RTOS 支持
""""""""""""""

::

    set ESP32_RTOS none

如果要支持 RTOS， 请注释掉这一行。


ESP32 的 SPI 闪存芯片的电源电压
"""""""""""""""""""""""""""""""

::

    set ESP32_FLASH_VOLTAGE 1.8

如果 SPI 闪存芯片的电源电压为 3.3 V， 请注释掉这一行，更多信息请参阅： :ref:`jtag-debugging-tip-code-flash-voltage`。


ESP32 的目标配置文件
""""""""""""""""""""

::

    source [find target/esp32.cfg]

.. note::

    除非你熟悉 OpenOCD 内部的工作原理，否则请不要更改 ``source [find target/esp32.cfg]`` 这一行。

目前 ``target/esp32.cfg`` 仍然是 ESP32 目标（esp108 和 esp32）的唯一配置文件。支持的配置矩阵如下所示：

    +---------------+---------------+---------------+
    | Dual/single   | RTOS          | Target used   |
    +===============+===============+===============+
    | dual          | FreeRTOS      | esp32         |
    +---------------+---------------+---------------+
    | single        | FreeRTOS      | esp108 (*)    |
    +---------------+---------------+---------------+
    | dual          | none          | esp108        |
    +---------------+---------------+---------------+
    | single        | none          | esp108        |
    +---------------+---------------+---------------+

    (*) — 我们计划修复此问题，并在后续提交中添加对 esp32 目标的单核调试的支持。

更多信息，请查看 ``board/esp-wroom-32.cfg`` 配置文件的注释部分。


.. _jtag-debugging-tip-reset-by-debugger:

复位 ESP32
^^^^^^^^^^

通过在 GDB 中输入 ``mon reset`` 或者 ``mon reset halt`` 来复位板子。


.. _jtag-debugging-tip-jtag-pins-reconfigured:

不要将 JTAG 引脚用于其他功能
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

如果除了 ESP32 模组和 JTAG 适配器之外的其他硬件也连接到了 JTAG 引脚，那么 JTAG 的操作可能会受到干扰。ESP32 JTAG 使用以下引脚：

    +---+----------------+-------------+
    |   | ESP32 JTAG Pin | JTAG Signal |
    +===+================+=============+
    | 1 | MTDO / GPIO15  | TDO         |
    +---+----------------+-------------+
    | 2 | MTDI / GPIO12  | TDI         |
    +---+----------------+-------------+
    | 3 | MTCK / GPIO13  | TCK         |
    +---+----------------+-------------+
    | 4 | MTMS / GPIO14  | TMS         |
    +---+----------------+-------------+

如果用户应用程序更改了 JTAG 引脚的配置，JTAG 通信可能会失败。如果 OpenOCD 正确初始化（检测到两个 Tensilica 内核），但在程序运行期间失去了同步并报出大量 DTR/DIR 错误，则应用程序可能将 JTAG 引脚重新配置为其他功能或者用户忘记将 Vtar 连接到 JTAG 适配器。 

.. highlight:: none

下面是 GDB 在应用程序进入重新配置 MTDO/GPIO15 作为输入代码后报告的一系列错误摘录::

    cpu0: xtensa_resume (line 431): DSR (FFFFFFFF) indicates target still busy!
    cpu0: xtensa_resume (line 431): DSR (FFFFFFFF) indicates DIR instruction generated an exception!
    cpu0: xtensa_resume (line 431): DSR (FFFFFFFF) indicates DIR instruction generated an overrun!
    cpu1: xtensa_resume (line 431): DSR (FFFFFFFF) indicates target still busy!
    cpu1: xtensa_resume (line 431): DSR (FFFFFFFF) indicates DIR instruction generated an exception!
    cpu1: xtensa_resume (line 431): DSR (FFFFFFFF) indicates DIR instruction generated an overrun!


.. _jtag-debugging-tip-reporting-issues:

报告 OpenOCD / GDB 的问题
^^^^^^^^^^^^^^^^^^^^^^^^^

如果你遇到 OpenOCD 或者 GDB 程序本身的问题，并且在网上没有找到可用的解决方案，请前往 https://github.com/espressif/openocd-esp32/issues 新建一个议题。

1.  请在问题报告中提供你使用的配置的详细信息：

    a. JTAG 适配器类型。
    b. 用于编译和加载正在调试的应用程序的 ESP-IDF 版本号。
    c. 用于调试的操作系统的详细信息。
    d. 操作系统是在本地计算机运行还是在虚拟机上运行？

2.  创建一个能够演示问题的简单示例工程，描述复现该问题的步骤。且这个调试示例不能受到 Wi-Fi 协议栈引入的非确定性行为的影响，因而再次遇到同样问题时，更容易复现。

.. highlight:: bash

3.  在启动命令中添加额外的参数来输出调试日志。

    OpenOCD 端：

        ::

            openocd -l openocd_log.txt -d 3 -f interface/ftdi/esp32_devkitj_v1.cfg -f board/esp-wroom-32.cfg

        这种方式会将日志输出到文件，但是它会阻止调试信息打印在终端上。当有大量信息需要输出的时候（比如调试等级提高到 ``-d 3``）这是个不错的选择。如果你仍然希望在屏幕上看到调试日志，请改用以下命令：

        ::

            openocd -d 3 -f interface/ftdi/esp32_devkitj_v1.cfg -f board/esp-wroom-32.cfg 2>&1 | tee openocd.log

    Debugger 端：

        ::

           xtensa-esp32-elf-gdb -ex "set remotelogfile gdb_log.txt" <all other options>

        也可以将命令 ``remotelogfile gdb_log.txt`` 添加到 ``gdbinit`` 文件中。


4.  请将 ``openocd_log.txt`` 和 ``gdb_log.txt`` 文件附在你的问题报告中。
