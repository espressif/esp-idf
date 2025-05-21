严重错误
========

:link_to_translation:`en:[English]`

.. _Overview:

概述
----

在某些情况下，程序并不会按照我们的预期运行，在 ESP-IDF 中，这些情况包括：

- CPU 异常：|CPU_EXCEPTIONS_LIST|
- 系统级检查错误：

    .. list::

        - :doc:`中断看门狗 <../api-reference/system/wdts>` 超时
        - :doc:`任务看门狗 <../api-reference/system/wdts>` 超时（只有开启 :ref:`CONFIG_ESP_TASK_WDT_PANIC` 后才会触发严重错误）
        - 高速缓存访问错误
        :SOC_MEMPROT_SUPPORTED: - 内存保护故障
        - 掉电检测事件
        - 堆栈溢出
        - 堆栈粉碎保护检查
        - 堆完整性检查
        - 未定义行为清理器 (UBSAN) 检查

- 使用 ``assert``、``configASSERT`` 等 :doc:`类似的宏 </api-guides/error-handling>` 断言失败。

本指南会介绍 ESP-IDF 中这类错误的处理流程，并给出对应的解决建议。

.. _Panic-Handler:

紧急处理程序
------------

:ref:`Overview` 中列举的所有出错原因都会由 *紧急处理程序 (Panic Handler)* 处理。

紧急处理程序首先会将出错原因打印到控制台，例如 CPU 异常的错误信息通常会类似于

.. parsed-literal::

    Guru Meditation Error: Core 0 panic'ed (|ILLEGAL_INSTR_MSG|). Exception was unhandled.

对于一些系统级检查错误（如中断看门狗超时，高速缓存访问错误等），错误信息会类似于

.. parsed-literal::

    Guru Meditation Error: Core 0 panic'ed (|CACHE_ERR_MSG|). Exception was unhandled.

不管哪种情况，出错原因都会以括号形式打印出来。请参阅 :ref:`Guru-Meditation-Errors` 以查看所有可能的出错原因。

紧急处理程序接下来的行为将取决于 :ref:`CONFIG_ESP_SYSTEM_PANIC` 的设置，支持的选项包括：

- 打印 CPU 寄存器，然后重启 (``CONFIG_ESP_SYSTEM_PANIC_PRINT_REBOOT``) - 默认选项

    打印系统发生异常时 CPU 寄存器的值，打印回溯，最后重启芯片。

- 打印 CPU 寄存器，然后暂停 (``CONFIG_ESP_SYSTEM_PANIC_PRINT_HALT``)

    与上一个选项类似，但不会重启，而是选择暂停程序的运行。重启程序需要外部执行复位操作。

- 静默重启 (``CONFIG_ESP_SYSTEM_PANIC_SILENT_REBOOT``)

    不打印 CPU 寄存器的值，也不打印回溯，立即重启芯片。

- 调用 GDB Stub (``CONFIG_ESP_SYSTEM_PANIC_GDBSTUB``)

    启动 GDB 服务器，通过控制台 UART 接口与 GDB 进行通信。该选项只提供只读调试或者事后调试，详细信息请参阅 `GDB Stub`_。

.. note::

    仅当构建中包含组件 ``esp_gdbstub`` 时，配置选项 :ref:`CONFIG_ESP_SYSTEM_PANIC` 中的 ``CONFIG_ESP_SYSTEM_PANIC_GDBSTUB`` 选项可用。

紧急处理程序的行为还受到另外两个配置项的影响：

- 如果使能了 :ref:`CONFIG_ESP_DEBUG_OCDAWARE` （默认），紧急处理程序会检测 {IDF_TARGET_NAME} 是否已经连接 JTAG 调试器。如果检测成功，程序会暂停运行，并将控制权交给调试器。在这种情况下，寄存器和回溯不会被打印到控制台，并且也不会使用 GDB Stub 和 Core Dump 的功能。

- 如果使能了 :doc:`内核转储 <core_dump>` 功能，系统状态（任务堆栈和寄存器）会被转储到 flash 或者 UART 以供后续分析。

- 如果 :ref:`CONFIG_ESP_PANIC_HANDLER_IRAM` 被禁用（默认情况下禁用），紧急处理程序的代码会放置在 flash 而不是 IRAM 中。这意味着，如果 ESP-IDF 在 flash 高速缓存禁用时崩溃，在运行 GDB Stub 和内核转储之前紧急处理程序会自动重新使能 flash 高速缓存。如果 flash 高速缓存也崩溃了，这样做会增加一些小风险。

    如果使能了该选项，紧急处理程序的代码（包括所需的 UART 函数）会放置在 IRAM 中，导致 SRAM 中的可用内存空间变小。当禁用 flash 高速缓存（如写入 SPI flash）时或触发异常导致 flash 高速缓存崩溃时，可用此选项调试一些复杂的崩溃问题。

- 如果启用 :ref:`CONFIG_ESP_SYSTEM_PANIC_REBOOT_DELAY_SECONDS` （默认为禁用）并将其配置为大于 0 的数字，紧急处理程序将基于该数字延迟重启的时间，单位为秒。如果用于监测串行输出的工具不支持停止和检查串行输出，可启用该选项。在这种情况下，借助延迟重启，用户可以在延迟期间检查和调试紧急处理程序的输出（例如回溯）。延迟结束后，设备将重新启动，并记录重置原因。

下图展示了紧急处理程序的行为：

.. blockdiag::
    :scale: 100%
    :caption: 紧急处理程序流程图（点击放大）
    :align: center

    blockdiag panic-handler {
        orientation = portrait;
        edge_layout = flowchart;
        default_group_color = white;
        node_width = 160;
        node_height = 60;

        cpu_exception [label = "CPU 异常", shape=roundedbox];
        sys_check [label = "Cache 错误,\nInterrupt WDT,\nabort()", shape=roundedbox];
        check_ocd [label = "JTAG 调试器\n已连接?", shape=diamond, height=80];
        print_error_cause [label = "打印出错原因"];
        use_jtag [label = "发送信号给 JTAG 调试器", shape=roundedbox];
        dump_registers [label = "打印寄存器\n和回溯"];
        check_coredump [label = "Core dump\n使能?", shape=diamond, height=80];
        do_coredump [label = "Core dump 至 UART 或者 Flash"];
        check_gdbstub [label = "GDB Stub\n使能?", shape=diamond, height=80];
        do_gdbstub [label = "启动 GDB Stub", shape=roundedbox];
        halt [label = "暂停", shape=roundedbox];
        reboot [label = "重启", shape=roundedbox];
        check_halt [label = "暂停?", shape=diamond, height=80];

        group {cpu_exception, sys_check};

        cpu_exception -> print_error_cause;
        sys_check -> print_error_cause;
        print_error_cause -> check_ocd;
        check_ocd -> use_jtag [label = "Yes"];
        check_ocd -> dump_registers [label = "No"];
        dump_registers -> check_coredump
        check_coredump -> do_coredump [label = "Yes"];
        do_coredump -> check_gdbstub;
        check_coredump -> check_gdbstub [label = "No"];
        check_gdbstub -> check_halt [label = "No"];
        check_gdbstub -> do_gdbstub [label = "Yes"];
        check_halt -> halt [label = "Yes"];
        check_halt -> reboot [label = "No"];
    }

寄存器转储与回溯
----------------

除非启用了 ``CONFIG_ESP_SYSTEM_PANIC_SILENT_REBOOT`` 否则紧急处理程序会将 CPU 寄存器和回溯打印到控制台。

.. only:: CONFIG_IDF_TARGET_ARCH_XTENSA

    .. code-block:: none

        Core 0 register dump:
        PC      : 0x400e14ed  PS      : 0x00060030  A0      : 0x800d0805  A1      : 0x3ffb5030
        A2      : 0x00000000  A3      : 0x00000001  A4      : 0x00000001  A5      : 0x3ffb50dc
        A6      : 0x00000000  A7      : 0x00000001  A8      : 0x00000000  A9      : 0x3ffb5000
        A10     : 0x00000000  A11     : 0x3ffb2bac  A12     : 0x40082d1c  A13     : 0x06ff1ff8
        A14     : 0x3ffb7078  A15     : 0x00000000  SAR     : 0x00000014  EXCCAUSE: 0x0000001d
        EXCVADDR: 0x00000000  LBEG    : 0x4000c46c  LEND    : 0x4000c477  LCOUNT  : 0xffffffff

        Backtrace: 0x400e14ed:0x3ffb5030 0x400d0802:0x3ffb5050

.. only:: CONFIG_IDF_TARGET_ARCH_RISCV

    .. code-block:: none

        Core  0 register dump:
        MEPC    : 0x420048b4  RA      : 0x420048b4  SP      : 0x3fc8f2f0  GP      : 0x3fc8a600
        TP      : 0x3fc8a2ac  T0      : 0x40057fa6  T1      : 0x0000000f  T2      : 0x00000000
        S0/FP   : 0x00000000  S1      : 0x00000000  A0      : 0x00000001  A1      : 0x00000001
        A2      : 0x00000064  A3      : 0x00000004  A4      : 0x00000001  A5      : 0x00000000
        A6      : 0x42001fd6  A7      : 0x00000000  S2      : 0x00000000  S3      : 0x00000000
        S4      : 0x00000000  S5      : 0x00000000  S6      : 0x00000000  S7      : 0x00000000
        S8      : 0x00000000  S9      : 0x00000000  S10     : 0x00000000  S11     : 0x00000000
        T3      : 0x00000000  T4      : 0x00000000  T5      : 0x00000000  T6      : 0x00000000
        MSTATUS : 0x00001881  MTVEC   : 0x40380001  MCAUSE  : 0x00000007  MTVAL   : 0x00000000
        MHARTID : 0x00000000

仅会打印异常帧中 CPU 寄存器的值，即引发 CPU 异常或者其它严重错误时刻的值。

紧急处理程序如果是因 ``abort()`` 而调用，则不会打印寄存器转储。

.. only:: CONFIG_IDF_TARGET_ARCH_XTENSA

    在某些情况下，例如中断看门狗超时，紧急处理程序会额外打印 CPU 寄存器 (EPC1-EPC4) 的值，以及另一个 CPU 的寄存器值和代码回溯。

    回溯行包含了当前任务中每个堆栈帧的 PC:SP 对（PC 是程序计数器，SP 是堆栈指针）。如果在 ISR 中发生了严重错误，回溯会同时包括被中断任务的 PC:SP 对，以及 ISR 中的 PC:SP 对。

如果使用了 :doc:`IDF 监视器 <tools/idf-monitor>`，该工具会将程序计数器的值转换为对应的代码位置（函数名，文件名，行号），并加以注释：

.. only:: CONFIG_IDF_TARGET_ARCH_XTENSA

    .. code-block:: none

        Core 0 register dump:
        PC      : 0x400e14ed  PS      : 0x00060030  A0      : 0x800d0805  A1      : 0x3ffb5030
        0x400e14ed: app_main at /Users/user/esp/example/main/main.cpp:36

        A2      : 0x00000000  A3      : 0x00000001  A4      : 0x00000001  A5      : 0x3ffb50dc
        A6      : 0x00000000  A7      : 0x00000001  A8      : 0x00000000  A9      : 0x3ffb5000
        A10     : 0x00000000  A11     : 0x3ffb2bac  A12     : 0x40082d1c  A13     : 0x06ff1ff8
        0x40082d1c: _calloc_r at /Users/user/esp/esp-idf/components/newlib/syscalls.c:51

        A14     : 0x3ffb7078  A15     : 0x00000000  SAR     : 0x00000014  EXCCAUSE: 0x0000001d
        EXCVADDR: 0x00000000  LBEG    : 0x4000c46c  LEND    : 0x4000c477  LCOUNT  : 0xffffffff

        Backtrace: 0x400e14ed:0x3ffb5030 0x400d0802:0x3ffb5050
        0x400e14ed: app_main at /Users/user/esp/example/main/main.cpp:36

        0x400d0802: main_task at /Users/user/esp/esp-idf/components/{IDF_TARGET_PATH_NAME}/cpu_start.c:470

.. only:: CONFIG_IDF_TARGET_ARCH_RISCV

    .. code-block:: none

        Core  0 register dump:
        MEPC    : 0x420048b4  RA      : 0x420048b4  SP      : 0x3fc8f2f0  GP      : 0x3fc8a600
        0x420048b4: app_main at /Users/user/esp/example/main/hello_world_main.c:20

        0x420048b4: app_main at /Users/user/esp/example/main/hello_world_main.c:20

        TP      : 0x3fc8a2ac  T0      : 0x40057fa6  T1      : 0x0000000f  T2      : 0x00000000
        S0/FP   : 0x00000000  S1      : 0x00000000  A0      : 0x00000001  A1      : 0x00000001
        A2      : 0x00000064  A3      : 0x00000004  A4      : 0x00000001  A5      : 0x00000000
        A6      : 0x42001fd6  A7      : 0x00000000  S2      : 0x00000000  S3      : 0x00000000
        0x42001fd6: uart_write at /Users/user/esp/esp-idf/components/vfs/vfs_uart.c:201

        S4      : 0x00000000  S5      : 0x00000000  S6      : 0x00000000  S7      : 0x00000000
        S8      : 0x00000000  S9      : 0x00000000  S10     : 0x00000000  S11     : 0x00000000
        T3      : 0x00000000  T4      : 0x00000000  T5      : 0x00000000  T6      : 0x00000000
        MSTATUS : 0x00001881  MTVEC   : 0x40380001  MCAUSE  : 0x00000007  MTVAL   : 0x00000000
        MHARTID : 0x00000000

    此外，由于紧急处理程序中提供了堆栈转储，因此 :doc:`IDF 监视器 <tools/idf-monitor>` 也可以生成并打印回溯。
    输出结果如下：

    .. code-block:: none

        Backtrace:

        0x42006686 in bar (ptr=ptr@entry=0x0) at ../main/hello_world_main.c:18
        18      *ptr = 0x42424242;
        #0  0x42006686 in bar (ptr=ptr@entry=0x0) at ../main/hello_world_main.c:18
        #1  0x42006692 in foo () at ../main/hello_world_main.c:22
        #2  0x420066ac in app_main () at ../main/hello_world_main.c:28
        #3  0x42015ece in main_task (args=<optimized out>) at /Users/user/esp/components/freertos/port/port_common.c:142
        #4  0x403859b8 in vPortEnterCritical () at /Users/user/esp/components/freertos/port/riscv/port.c:130
        #5  0x00000000 in ?? ()
        Backtrace stopped: frame did not save the PC

    虽然以上的回溯信息非常方便，但要求用户使用 :doc:`IDF 监视器 <tools/idf-monitor>`。因此，如果用户希望使用其它的串口监控软件也能显示堆栈回溯信息，则需要在 menuconfig 中的 “Backtracing method” 菜单下启用 ``CONFIG_ESP_SYSTEM_USE_EH_FRAME`` 选项。

    该选项会让编译器为项目的每个函数生成 DWARF 信息。然后，当 CPU 异常发生时，紧急处理程序将解析这些数据并生成出错任务的堆栈回溯信息。输出结果如下：

    .. code-block:: none

        Backtrace: 0x42009e9a:0x3fc92120 0x42009ea6:0x3fc92120 0x42009ec2:0x3fc92130 0x42024620:0x3fc92150 0x40387d7c:0x3fc92160 0xfffffffe:0x3fc92170

    这些 ``PC:SP`` 对代表当前任务每一个栈帧的程序计数器值 (Program Counter) 和栈顶地址 (Stack Pointer)。


    ``CONFIG_ESP_SYSTEM_USE_EH_FRAME`` 选项的主要优点是，回溯信息可以由程序自己解析生成并打印（而不依靠 :doc:`tools/idf-monitor`）。但是该选项会导致编译后的二进制文件更大（增幅可达 20% 甚至 100%）。此外，该选项会将调试信息也保存在二进制文件里。因此，强烈建议不要在量产版本中启用该选项。

    另一种在设备上生成此类回溯信息的是，在 menuconfig 中的 “Backtracing method” 菜单下启用 ``CONFIG_ESP_SYSTEM_USE_FRAME_POINTER`` 选项。

    启用该选项后，编译器会保留一个 CPU 寄存器，用于跟踪程序中每个函数的栈帧。这样，异常处理程序能够在任何给定时间（尤其是在发生 CPU 异常时）展开调用栈。

    启用 ``CONFIG_ESP_SYSTEM_USE_FRAME_POINTER`` 选项会导致编译后的二进制文件大小增加约 5-6%，性能下降约 1%。与 ``CONFIG_ESP_SYSTEM_USE_EH_FRAME`` 选项不同，编译器不会在生成的二进制文件中生成调试信息，因此可以在量产版本中使用此功能。

若要查找发生严重错误的代码位置，请查看 "Backtrace" 的后面几行，发生严重错误的代码显示在顶行，后续几行显示的是调用堆栈。

.. _GDB-Stub:

GDB Stub
--------

如果启用了 ``CONFIG_ESP_SYSTEM_PANIC_GDBSTUB`` 选项，在发生严重错误时，紧急处理程序不会复位芯片，相反，它将启动 GDB 远程协议服务器，通常称为 GDB Stub。发生这种情况时，可以让主机上运行的 GDB 实例通过 UART 端口连接到 ESP32。

如果使用了 :doc:`IDF 监视器 <tools/idf-monitor>`，该工具会在 UART 端口检测到 GDB Stub 提示符后自动启动 GDB，输出会类似于：

.. code-block:: none

    Entering gdb stub now.
    $T0b#e6GNU gdb (crosstool-NG crosstool-ng-1.22.0-80-gff1f415) 7.10
    Copyright (C) 2015 Free Software Foundation, Inc.
    License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
    This is free software: you are free to change and redistribute it.
    There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
    and "show warranty" for details.
    This GDB was configured as "--host=x86_64-build_apple-darwin16.3.0 --target={IDF_TARGET_TOOLCHAIN_PREFIX}".
    Type "show configuration" for configuration details.
    For bug reporting instructions, please see:
    <http://www.gnu.org/software/gdb/bugs/>.
    Find the GDB manual and other documentation resources online at:
    <http://www.gnu.org/software/gdb/documentation/>.
    For help, type "help".
    Type "apropos word" to search for commands related to "word"...
    Reading symbols from /Users/user/esp/example/build/example.elf...done.
    Remote debugging using /dev/cu.usbserial-31301
    0x400e1b41 in app_main ()
        at /Users/user/esp/example/main/main.cpp:36
    36      *((int*) 0) = 0;
    (gdb)

在 GDB 会话中，我们可以检查 CPU 寄存器，本地和静态变量以及内存中任意位置的值。但是不支持设置断点，改变 PC 值或者恢复程序的运行。若要复位程序，请退出 GDB 会话，在 IDF 监视器 中连续输入 Ctrl-T Ctrl-R，或者按下开发板上的复位按键也可以重新运行程序。

.. _RTC-Watchdog-Timeout:

RTC 看门狗超时
----------------

{IDF_TARGET_RTCWDT_RTC_RESET:default="未更新", esp32="RTCWDT_RTC_RESET", esp32s2="RTCWDT_RTC_RST", esp32s3="RTCWDT_RTC_RST", esp32c3="RTCWDT_RTC_RST", esp32c2="RTCWDT_RTC_RST", esp32c6="LP_WDT_SYS", esp32h2="LP_WDT_SYS", esp32p4="LP_WDT_SYS"}

RTC 看门狗在启动代码中用于跟踪执行时间，也有助于防止由于电源不稳定引起的锁定。RTC 看门狗默认启用，参见 :ref:`CONFIG_BOOTLOADER_WDT_ENABLE`。如果执行时间超时，RTC 看门狗将自动重启系统。此时，一级 (ROM) 引导加载程序将打印消息 ``RTC Watchdog Timeout`` 说明重启原因。

.. code-block:: none

    rst:0x10 ({IDF_TARGET_RTCWDT_RTC_RESET})


RTC 看门狗涵盖了从一级 (ROM) 引导加载程序到应用程序启动的执行时间，最初在一级 (ROM) 引导加载程序中设置，而后在引导加载程序中使用 :ref:`CONFIG_BOOTLOADER_WDT_TIME_MS` 选项进行配置（默认 9000 ms）。在应用初始化阶段，由于慢速时钟源可能已更改，RTC 看门狗将被重新配置，最后在调用 ``app_main()`` 之前被禁用。可以使用选项 :ref:`CONFIG_BOOTLOADER_WDT_DISABLE_IN_USER_CODE` 以保证 RTC 看门狗在调用 ``app_main`` 之前不被禁用，而是保持运行状态，用户需要在应用代码中定期“喂狗”。

:ref:`紧急处理程序 <Panic-Handler>` 通过 RTC 看门狗保护机制，确保系统在遇到严重错误时不会陷入死循环。紧急处理程序会重新配置 RTC 看门狗的超时时间为 10 秒。如果 10 秒内紧急处理程序没有完成，RTC 看门狗将会强制复位系统。

.. _Guru-Meditation-Errors:

Guru Meditation 错误
--------------------

.. Note to editor: titles of the following section need to match exception causes printed by the panic handler. Do not change the titles (insert spaces, reword, etc.) unless panic handler messages are also changed.

.. Note to translator: When translating this section, avoid translating the following section titles. "Guru Meditation" in the title of this section should also not be translated. Keep these two notes when translating.

本节将对打印在 ``Guru Meditation Error: Core panic'ed`` 后面括号中的致错原因进行逐一解释。

.. note::

    想要了解 "Guru Meditation" 的历史渊源，请参阅 `维基百科 <https://en.wikipedia.org/wiki/Guru_Meditation>`_ 。


|ILLEGAL_INSTR_MSG|
^^^^^^^^^^^^^^^^^^^

此 CPU 异常表示当前执行的指令不是有效指令，引起此错误的常见原因包括：

- FreeRTOS 中的任务函数已返回。在 FreeRTOS 中，如果想终止任务函数，需要调用 :cpp:func:`vTaskDelete` 函数释放当前任务的资源，而不是直接返回。

- 无法从 SPI flash 中读取下一条指令，这通常发生在：

    - 应用程序将 SPI flash 的管脚重新配置为其它功能（如 GPIO、UART 等）。有关 SPI flash 管脚的详细信息，请参阅硬件设计指南和芯片/模组的数据手册。

    - 某些外部设备意外连接到 SPI flash 的管脚上，干扰了 {IDF_TARGET_NAME} 和 SPI flash 之间的通信。

- 在 C++ 代码中，退出 non-void 函数而无返回值被认为是未定义的行为。启用优化后，编译器通常会忽略此类函数的结尾，导致 |ILLEGAL_INSTR_MSG| 异常。默认情况下，ESP-IDF 构建系统启用 ``-Werror=return-type``，这意味着缺少返回语句会被视为编译时错误。但是，如果应用程序项目禁用了编译器警告，可能就无法检测到该问题，在运行时就会出现 |ILLEGAL_INSTR_MSG| 异常。

.. only:: CONFIG_IDF_TARGET_ARCH_XTENSA

    InstrFetchProhibited
    ^^^^^^^^^^^^^^^^^^^^

    此 CPU 异常表示 CPU 无法读取指令，因为指令的地址不在 IRAM 或者 IROM 中的有效区域中。

    通常这意味着代码中调用了并不指向有效代码块的函数指针。这种情况下，可以查看 ``PC`` （程序计数器）寄存器的值并做进一步判断：若为 0 或者其它非法值（即只要不是 ``0x4xxxxxxx`` 的情况），则证实确实是该原因。

    LoadProhibited, StoreProhibited
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    当应用程序尝试读取或写入无效的内存位置时，会发生此类 CPU 异常。此类无效内存地址可以在寄存器转储的 ``EXCVADDR`` 中找到。如果该地址为零，通常意味着应用程序正尝试解引用一个 NULL 指针。如果该地址接近于零，则通常意味着应用程序尝试访问某个结构体的成员，但是该结构体的指针为 NULL。如果该地址是其它非法值（不在 ``0x3fxxxxxx`` - ``0x6xxxxxxx`` 的范围内），则可能意味着用于访问数据的指针未初始化或者已经损坏。

    IntegerDivideByZero
    ^^^^^^^^^^^^^^^^^^^

    应用程序尝试将整数除以零。

    LoadStoreAlignment
    ^^^^^^^^^^^^^^^^^^

    应用程序尝试读取/写入的内存位置不符合加载/存储指令对字节对齐大小的要求，例如，32 位读取指令只能访问 4 字节对齐的内存地址，而 16 位写入指令只能访问 2 字节对齐的内存地址。

    LoadStoreError
    ^^^^^^^^^^^^^^

    这类异常通常发生于以下几种场合:

    - 应用程序尝试从仅支持 32 位读取/写入的内存区域执行 8 位或 16 位加载/存储操作，例如，解引用一个指向指令内存区域（比如 IRAM 或者 IROM）的 char* 指针就会触发这个错误。

    - 应用程序尝试写入数据到只读的内存区域（比如 IROM 或者 DROM）也会触发这个错误。

    Unhandled debug exception
    ^^^^^^^^^^^^^^^^^^^^^^^^^

    执行指令 ``BREAK`` 时，会发生此 CPU 异常。

.. only:: CONFIG_IDF_TARGET_ARCH_RISCV

    Instruction address misaligned
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    此 CPU 异常表示要执行的指令地址非 2 字节对齐。

    Instruction access fault, Load access fault, Store access fault
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    当应用程序尝试读取或写入无效的内存位置时，会发生此类 CPU 异常。此类无效内存地址可以在寄存器转储的 ``MTVAL`` 中找到。如果该地址为零，通常意味着应用程序正尝试解引用一个 NULL 指针。如果该地址接近于零，则通常意味着应用程序尝试访问某个结构体的成员，但是该结构体的指针为 NULL。如果该地址是其它非法值（不在 ``0x3fxxxxxx`` - ``0x6xxxxxxx`` 的范围内），则可能意味着用于访问数据的指针未初始化或者已经损坏。

    Breakpoint
    ^^^^^^^^^^

    执行 ``EBREAK`` 指令时，会发生此 CPU 异常。请参见 :ref:`FreeRTOS-End-Of-Stack-Watchpoint`。

    Load address misaligned, Store address misaligned
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    应用程序尝试读取/写入的内存位置不符合加载/存储指令对字节对齐大小的要求，例如，32 位加载指令只能访问 4 字节对齐的内存地址，而 16 位加载指令只能访问 2 字节对齐的内存地址。

Interrupt wdt timeout on CPU0/CPU1
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

这表示发生了中断看门狗超时，详细信息请查阅 :doc:`看门狗 <../api-reference/system/wdts>` 文档。

.. _cache_error:

|CACHE_ERR_MSG|
^^^^^^^^^^^^^^^

某些情况下，ESP-IDF 会暂时禁止通过 cache 访问外部 SPI flash 和 SPI RAM，例如在使用 spi_flash API 读取/写入/擦除/映射 SPI flash 的时候。在这些情况下，任务会被挂起，并且未使用 ``ESP_INTR_FLAG_IRAM`` 注册的中断处理程序会被禁用。请确保任何使用此标志注册的中断处理程序所访问的代码和数据分别位于 IRAM 和 DRAM 中。更多详细信息请参阅 :ref:`SPI flash API 文档 <iram-safe-interrupt-handlers>` 和 :ref:`IRAM 安全中断处理程序 <iram_safe_interrupts_handlers>`。

.. only:: SOC_MEMPROT_SUPPORTED

    Memory Protection Fault
    ^^^^^^^^^^^^^^^^^^^^^^^

    ESP-IDF 中使用 {IDF_TARGET_NAME} 的权限控制功能来防止以下类型的内存访问：

    * 程序加载后向指令 RAM 写入代码
    * 从数据 RAM（用于堆、静态 .data 和 .bss 区域）执行代码

    该类操作对于大多数程序来说并不必要，禁止此类操作往往使软件漏洞更难被利用。依赖动态加载或自修改代码的应用程序可以使用 :ref:`CONFIG_ESP_SYSTEM_MEMPROT_FEATURE` 选项来禁用此项保护。

    发生故障时，紧急处理程序会报告故障的地址和引起故障的内存访问的类型。

其他严重错误
------------

.. only:: SOC_BOD_SUPPORTED

    掉电
    ^^^^

    {IDF_TARGET_NAME} 内部集成掉电检测电路，并且会默认启用。如果电源电压低于安全值，掉电检测器可以触发系统复位。掉电检测器可以使用 :ref:`CONFIG_ESP_BROWNOUT_DET` 和 :ref:`CONFIG_ESP_BROWNOUT_DET_LVL_SEL` 这两个选项进行设置。

    当掉电检测器被触发时，会打印如下信息：

    .. code-block:: none

        Brownout detector was triggered

    芯片会在该打印信息结束后复位。

    请注意，如果电源电压快速下降，则只能在控制台上看到部分打印信息。


堆不完整
^^^^^^^^

ESP-IDF 堆的实现包含许多运行时的堆结构检查，可以在 menuconfig 中开启额外的检查（“Heap Poisoning”）。如果其中的某项检查失败，则会打印类似如下信息：

.. code-block:: none

    CORRUPT HEAP: Bad tail at 0x3ffe270a. Expected 0xbaad5678 got 0xbaac5678
    assertion "head != NULL" failed: file "/Users/user/esp/esp-idf/components/heap/multi_heap_poisoning.c", line 201, function: multi_heap_free
    abort() was called at PC 0x400dca43 on core 0

更多详细信息，请查阅 :doc:`堆内存调试 <../api-reference/system/heap_debug>` 文档。

|STACK_OVERFLOW|
^^^^^^^^^^^^^^^^

.. only:: SOC_ASSIST_DEBUG_SUPPORTED

    .. _Hardware-Stack-Guard:

    硬件堆栈保护
    """"""""""""

    {IDF_TARGET_NAME} 集成了辅助调试模块，支持监测堆栈指针 (SP) 寄存器，确保其值位于已分配给堆栈的内存范围内。发生中断处理或 FreeRTOS 切换上下文时，辅助调试模块都会设置新的堆栈监测范围。注意，该操作会对性能产生一定影响。

    以下为辅助调试模块的部分相关特性：

    - 采用硬件实现
    - 支持监测堆栈指针寄存器的值
    - 无需占用额外 CPU 时间或内存，即可监测堆栈内存范围

    当辅助调试模块检测到堆栈溢出时，将触发紧急处理程序并打印类似如下信息：

    .. parsed-literal::

        Guru Meditation Error: Core 0 panic'ed (Stack protection fault).

    可以通过 :ref:`CONFIG_ESP_SYSTEM_HW_STACK_GUARD` 选项，禁用硬件堆栈保护。

.. _FreeRTOS-End-Of-Stack-Watchpoint:

FreeRTOS 任务堆栈末尾监视点
"""""""""""""""""""""""""""

ESP-IDF 支持基于监视点的 FreeRTOS 堆栈溢出检测机制。每次 FreeRTOS 切换任务上下文时，都会设置一个监视点，用于监视堆栈的最后 32 字节。

通常，该设置会提前触发监视点，触发点可能会比预期提前多达 28 字节。基于 FreeRTOS 中堆栈金丝雀的大小为 20 字节，故将观察范围设置为 32 字节，确保可以在堆栈金丝雀遭到破坏前及时触发监测点。

.. note::

    并非每次堆栈溢出都能触发监视点。如果任务绕过堆栈金丝雀的位置访问堆栈，则无法触发监视点。

监视点触发后，将打印类似如下信息：

.. only:: CONFIG_IDF_TARGET_ARCH_XTENSA

    .. code-block:: none

        Debug exception reason: Stack canary watchpoint triggered (task_name)

.. only:: CONFIG_IDF_TARGET_ARCH_RISCV

    .. code-block:: none

        Guru Meditation Error: Core  0 panic'ed (Breakpoint). Exception was unhandled.

可以通过 :ref:`CONFIG_FREERTOS_WATCHPOINT_END_OF_STACK` 选项启用该功能。


FreeRTOS 堆栈检查
"""""""""""""""""

请参见 :ref:`CONFIG_FREERTOS_CHECK_STACKOVERFLOW`。

堆栈粉碎
^^^^^^^^

堆栈粉碎保护（基于 GCC ``-fstack-protector*`` 标志）可以通过 ESP-IDF 中的 :ref:`CONFIG_COMPILER_STACK_CHECK_MODE` 选项来开启。如果检测到堆栈粉碎，则会打印类似如下的信息：

.. code-block:: none

    Stack smashing protect failure!

    abort() was called at PC 0x400d2138 on core 0

    Backtrace: 0x4008e6c0:0x3ffc1780 0x4008e8b7:0x3ffc17a0 0x400d2138:0x3ffc17c0 0x400e79d5:0x3ffc17e0 0x400e79a7:0x3ffc1840 0x400e79df:0x3ffc18a0 0x400e2235:0x3ffc18c0 0x400e1916:0x3ffc18f0 0x400e19cd:0x3ffc1910 0x400e1a11:0x3ffc1930 0x400e1bb2:0x3ffc1950 0x400d2c44:0x3ffc1a80
    0

回溯信息会指明发生堆栈粉碎的函数，建议检查函数中是否有代码访问局部数组时发生了越界。

.. only:: CONFIG_IDF_TARGET_ARCH_XTENSA

    .. |CPU_EXCEPTIONS_LIST| replace:: 非法指令，加载/存储时的内存对齐错误，加载/存储时的访问权限错误，双重异常。
    .. |ILLEGAL_INSTR_MSG| replace:: IllegalInstruction
    .. |CACHE_ERR_MSG| replace:: Cache error
    .. |STACK_OVERFLOW| replace:: 堆栈溢出

.. only:: CONFIG_IDF_TARGET_ARCH_RISCV

    .. |CPU_EXCEPTIONS_LIST| replace:: 非法指令，加载/存储时的内存对齐错误，加载/存储时的访问权限错误。
    .. |ILLEGAL_INSTR_MSG| replace:: IllegalInstruction
    .. |CACHE_ERR_MSG| replace:: Cache error
    .. |STACK_OVERFLOW| replace:: 堆栈溢出


.. only:: SOC_CPU_HAS_LOCKUP_RESET

    CPU 锁死
    ^^^^^^^^

    若出现双重异常，即当 CPU 已经在异常处理程序中时，又出现了其他异常，则 CPU 会锁死，并触发系统复位。当 cache 出现问题，导致 CPU 无法访问外部存储时，CPU 就会进入锁死状态。此时，紧急处理程序也会因为无法获取指令或读取数据而崩溃。

    通过配置选项 :ref:`CONFIG_ESP_PANIC_HANDLER_IRAM`，可以将紧急处理程序的代码放在 IRAM 中，这样即使 cache 被禁用也可以访问 IRAM，从而获取更多关于锁死原因的信息。


未定义行为清理器 (UBSAN) 检查
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

未定义行为清理器 (UBSAN) 是一种编译器功能，它会为可能不正确的操作添加运行时检查，例如：

- 溢出（乘法溢出、有符号整数溢出）
- 移位基数或指数错误（如移位超过 32 位）
- 整数转换错误

请参考 `GCC 文档 <https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html>`_ 中的 ``-fsanitize=undefined`` 选项，查看支持检查的完整列表。

使能 UBSAN
""""""""""

默认情况下未启用 UBSAN。可以通过在构建系统中添加编译器选项 ``-fsanitize=undefined`` 在文件、组件或项目级别上使能 UBSAN。

在对使用 SoC 硬件寄存器头文件（``soc/xxx_reg.h``）的代码使能 UBSAN 时，建议使用 ``-fno-sanitize=shift-base`` 选项禁用移位基数清理器。这是由于 ESP-IDF 寄存器头文件目前包含的模式会对这个特定的清理器选项造成误报。

要在项目级使能 UBSAN，请在项目 CMakeLists.txt 文件的末尾添加以下内容：

.. code-block:: none

    idf_build_set_property(COMPILE_OPTIONS "-fsanitize=undefined" "-fno-sanitize=shift-base" APPEND)

或者，通过 ``EXTRA_CFLAGS`` 和 ``EXTRA_CXXFLAGS`` 环境变量来传递这些选项。

使能 UBSAN 会明显增加代码量和数据大小。当为整个应用程序使能 UBSAN 时，微控制器的可用 RAM 无法容纳大多数应用程序（除了一些微小程序）。因此，建议为特定的待测组件使能 UBSAN。

要为项目 CMakeLists.txt 文件中的特定组件（``component_name``）启用 UBSAN，请在文件末尾添加以下内容：

.. code-block:: none

    idf_component_get_property(lib component_name COMPONENT_LIB)
    target_compile_options(${lib} PRIVATE "-fsanitize=undefined" "-fno-sanitize=shift-base")

.. note::

    关于 :ref:`构建属性 <cmake-build-properties>` 和 :ref:`组件属性 <cmake-component-properties>` 的更多信息，请查看构建系统文档。

要为同一组件的 CMakeLists.txt 中的特定组件（``component_name``）使能 UBSAN，在文件末尾添加以下内容：

.. code-block:: none

    target_compile_options(${COMPONENT_LIB} PRIVATE "-fsanitize=undefined" "-fno-sanitize=shift-base")

UBSAN 输出
""""""""""

当 UBSAN 检测到一个错误时，会打印一个信息和回溯，例如：

.. code-block:: none

    Undefined behavior of type out_of_bounds

    Backtrace:0x4008b383:0x3ffcd8b0 0x4008c791:0x3ffcd8d0 0x4008c587:0x3ffcd8f0 0x4008c6be:0x3ffcd950 0x400db74f:0x3ffcd970 0x400db99c:0x3ffcd9a0

当使用 :doc:`IDF 监视器 <tools/idf-monitor>` 时，回溯会被解码为函数名以及源代码位置，并指向问题发生的位置（这里是 ``main.c:128``）：

.. code-block:: none

    0x4008b383: panic_abort at /path/to/esp-idf/components/esp_system/panic.c:367

    0x4008c791: esp_system_abort at /path/to/esp-idf/components/esp_system/system_api.c:106

    0x4008c587: __ubsan_default_handler at /path/to/esp-idf/components/esp_system/ubsan.c:152

    0x4008c6be: __ubsan_handle_out_of_bounds at /path/to/esp-idf/components/esp_system/ubsan.c:223

    0x400db74f: test_ub at main.c:128

    0x400db99c: app_main at main.c:56 (discriminator 1)

UBSAN 报告的错误类型为以下几种：


.. list-table::
    :widths: 40 60
    :header-rows: 1

    * - 名称
      - 含义
    * - ``type_mismatch``、``type_mismatch_v1``
      - 指针值不正确：空、未对齐、或与给定类型不兼容
    * - ``add_overflow``、``sub_overflow``、``mul_overflow``、``negate_overflow``
      - 加法、减法、乘法、求反过程中的整数溢出
    * - ``divrem_overflow``
      - 整数除以 0 或 ``INT_MIN``
    * - ``shift_out_of_bounds``
      - 左移或右移运算符导致的溢出
    * - ``out_of_bounds``
      - 访问超出数组范围
    * - ``unreachable``
      - 执行无法访问的代码
    * - ``missing_return``
      - Non-void 函数已结束而没有返回值（仅限 C++）
    * - ``vla_bound_not_positive``
      - 可变长度数组的大小不是正数
    * - ``load_invalid_value``
      - bool 或 enum（仅 C++）变量的值无效（超出范围）
    * - ``nonnull_arg``
      - 对于 ``nonnull`` 属性的函数，传递给函数的参数为空
    * - ``nonnull_return``
      - 对于 ``returns_nonnull`` 属性的函数，函数返回值为空
    * - ``builtin_unreachable``
      - 调用 ``__builtin_unreachable`` 函数
    * - ``pointer_overflow``
      - 指针运算过程中的溢出
