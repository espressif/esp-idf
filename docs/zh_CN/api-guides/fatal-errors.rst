严重错误
========
:link_to_translation:`en:[English]`

.. _Overview:

概述
----

在某些情况下，程序并不会按照我们的预期运行，在 ESP-IDF 中，这些情况包括：

- CPU 异常：非法指令，加载/存储时的内存对齐错误，加载/存储时的访问权限错误，双重异常。
- 系统级检查错误：

  - :doc:`中断看门狗 <../api-reference/system/wdts>` 超时
  - :doc:`任务看门狗 <../api-reference/system/wdts>` 超时（只有开启 :ref:`CONFIG_ESP_TASK_WDT_PANIC` 后才会触发严重错误）
  - 高速缓存访问错误
  - 掉电检测事件
  - 堆栈溢出
  - Stack 粉碎保护检查
  - Heap 完整性检查

- 使用 ``assert``、``configASSERT`` 等类似的宏断言失败。

本指南会介绍 ESP-IDF 中这类错误的处理流程，并给出对应的解决建议。

紧急处理程序
------------

:ref:`Overview` 中列举的所有错误都会由 *紧急处理程序（Panic Handler）* 负责处理。

紧急处理程序首先会将出错原因打印到控制台，例如 CPU 异常的错误信息通常会类似于::

    Guru Meditation Error: Core 0 panic'ed (IllegalInstruction). Exception was unhandled.

对于一些系统级检查错误（如中断看门狗超时，高速缓存访问错误等），错误信息会类似于::

    Guru Meditation Error: Core 0 panic'ed (Cache disabled but cached memory region accessed)

不管哪种情况，错误原因都会被打印在括号中。请参阅 :ref:`Guru-Meditation-Errors` 以查看所有可能的出错原因。

紧急处理程序接下来的行为将取决于 :ref:`CONFIG_ESP32_PANIC` 的设置，支持的选项包括：

- 打印 CPU 寄存器，然后重启（``CONFIG_ESP32_PANIC_PRINT_REBOOT``）- 默认选项

  打印系统发生异常时 CPU 寄存器的值，打印回溯，最后重启芯片。

- 打印 CPU 寄存器，然后暂停（``CONFIG_ESP32_PANIC_PRINT_HALT``）

  与上一个选项类似，但不会重启，而是选择暂停程序的运行。重启程序需要外部执行复位操作。

- 静默重启（``CONFIG_ESP32_PANIC_SILENT_REBOOT``）

  不打印 CPU 寄存器的值，也不打印回溯，立即重启芯片。

- 调用 GDB Stub（``CONFIG_ESP32_PANIC_GDBSTUB``）

  启动 GDB 服务器，通过控制台 UART 接口与 GDB 进行通信。详细信息请参阅 :ref:`GDB-Stub`。

紧急处理程序的行为还受到另外两个配置项的影响：

- 如果 :ref:`CONFIG_ESP32_DEBUG_OCDAWARE` 被使能了（默认），紧急处理程序会检测 ESP32 是否已经连接 JTAG 调试器。如果检测成功，程序会暂停运行，并将控制权交给调试器。在这种情况下，寄存器和回溯不会被打印到控制台，并且也不会使用 GDB Stub 和 Core Dump 的功能。

- 如果使能了 :doc:`Core Dump <core_dump>` 功能（``CONFIG_ESP32_ENABLE_COREDUMP_TO_FLASH`` 或者 ``CONFIG_ESP32_ENABLE_COREDUMP_TO_UART`` 选项），系统状态（任务堆栈和寄存器）会被转储到 Flash 或者 UART 以供后续分析。

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

除非启用了 ``CONFIG_ESP32_PANIC_SILENT_REBOOT`` 否则紧急处理程序会将 CPU 寄存器和回溯打印到控制台::

    Core 0 register dump:
    PC      : 0x400e14ed  PS      : 0x00060030  A0      : 0x800d0805  A1      : 0x3ffb5030  
    A2      : 0x00000000  A3      : 0x00000001  A4      : 0x00000001  A5      : 0x3ffb50dc  
    A6      : 0x00000000  A7      : 0x00000001  A8      : 0x00000000  A9      : 0x3ffb5000  
    A10     : 0x00000000  A11     : 0x3ffb2bac  A12     : 0x40082d1c  A13     : 0x06ff1ff8  
    A14     : 0x3ffb7078  A15     : 0x00000000  SAR     : 0x00000014  EXCCAUSE: 0x0000001d  
    EXCVADDR: 0x00000000  LBEG    : 0x4000c46c  LEND    : 0x4000c477  LCOUNT  : 0xffffffff  

    Backtrace: 0x400e14ed:0x3ffb5030 0x400d0802:0x3ffb5050

仅会打印异常帧中 CPU 寄存器的值，即引发 CPU 异常或者其它严重错误时刻的值。

紧急处理程序如果是因 abort() 而调用，则不会打印寄存器转储。

在某些情况下，例如中断看门狗超时，紧急处理程序会额外打印 CPU 寄存器（EPC1-EPC4）的值，以及另一个 CPU 的寄存器值和代码回溯。

回溯行包含了当前任务中每个堆栈帧的 PC:SP 对（PC 是程序计数器，SP 是堆栈指针）。如果在 ISR 中发生了严重错误，回溯会同时包括被中断任务的 PC:SP 对，以及 ISR 中的 PC:SP 对。

如果使用了 :doc:`IDF 监视器 <tools/idf-monitor>`，该工具会将程序计数器的值转换为对应的代码位置（函数名，文件名，行号），并加以注释::

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

    0x400d0802: main_task at /Users/user/esp/esp-idf/components/esp32/cpu_start.c:470

若要查找发生严重错误的代码位置，请查看 "Backtrace" 的后面几行，发生严重错误的代码显示在顶行，后续几行显示的是调用堆栈。

.. _GDB-Stub:

GDB Stub
--------

如果启用了 ``CONFIG_ESP32_PANIC_GDBSTUB`` 选项，在发生严重错误时，紧急处理程序不会复位芯片，相反，它将启动 GDB 远程协议服务器，通常称为 GDB Stub。发生这种情况时，可以让主机上运行的 GDB 实例通过 UART 端口连接到 ESP32。

如果使用了 :doc:`IDF 监视器 <tools/idf-monitor>`，该工具会在 UART 端口检测到 GDB Stub 提示符后自动启动 GDB，输出会类似于::

    Entering gdb stub now.
    $T0b#e6GNU gdb (crosstool-NG crosstool-ng-1.22.0-80-gff1f415) 7.10
    Copyright (C) 2015 Free Software Foundation, Inc.
    License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
    This is free software: you are free to change and redistribute it.
    There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
    and "show warranty" for details.
    This GDB was configured as "--host=x86_64-build_apple-darwin16.3.0 --target=xtensa-esp32-elf".
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

.. _Guru-Meditation-Errors:

Guru Meditation 错误
--------------------

.. Note to editor: titles of the following section need to match exception causes printed by the panic handler. Do not change the titles (insert spaces, reword, etc.) unless panic handler messages are also changed.

.. Note to translator: When translating this section, avoid translating the following section titles. "Guru Meditation" in the title of this section should also not be translated. Keep these two notes when translating.

本节将对打印在 ``Guru Meditation Error: Core panic'ed`` 后面括号中的致错原因进行逐一解释。

.. note:: 想要了解 "Guru Meditation" 的历史渊源，请参阅 `维基百科 <https://en.wikipedia.org/wiki/Guru_Meditation>`_ 。


IllegalInstruction
^^^^^^^^^^^^^^^^^^

此 CPU 异常表示当前执行的指令不是有效指令，引起此错误的常见原因包括：

- FreeRTOS 中的任务函数已返回。在 FreeRTOS 中，如果想终止任务函数，需要调用 :cpp:func:`vTaskDelete` 函数释放当前任务的资源，而不是直接返回。

- 无法从 SPI Flash 中加载下一条指令，这通常发生在：
  
  - 应用程序将 SPI Flash 的引脚重新配置为其它功能（如 GPIO，UART 等等）。有关 SPI Flash 引脚的详细信息，请参阅硬件设计指南和芯片/模组的数据手册。
  
  - 某些外部设备意外连接到 SPI Flash 的引脚上，干扰了 ESP32 和 SPI Flash 之间的通信。


InstrFetchProhibited
^^^^^^^^^^^^^^^^^^^^

此 CPU 异常表示 CPU 无法加载指令，因为指令的地址不在 IRAM 或者 IROM 中的有效区域中。

通常这意味着代码中调用了并不指向有效代码块的函数指针。这种情况下，可以查看 ``PC`` （程序计数器）寄存器的值并做进一步判断：若为 0 或者其它非法值（即只要不是 ``0x4xxxxxxx`` 的情况），则证实确实是该原因。

LoadProhibited, StoreProhibited
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

当应用程序尝试读取或写入无效的内存位置时，会发生此类 CPU 异常。此类无效内存地址可以在寄存器转储的 ``EXCVADDR`` 中找到。如果该地址为零，通常意味着应用程序正尝试解引用一个 NULL 指针。如果该地址接近于零，则通常意味着应用程序尝试访问某个结构体的成员，但是该结构体的指针为 NULL。如果该地址是其它非法值（不在 ``0x3fxxxxxx`` - ``0x6xxxxxxx`` 的范围内），则可能意味着用于访问数据的指针未初始化或者已经损坏。

IntegerDivideByZero
^^^^^^^^^^^^^^^^^^^

应用程序尝试将整数除以零。

LoadStoreAlignment
^^^^^^^^^^^^^^^^^^

应用程序尝试读取/写入的内存位置不符合加载/存储指令对字节对齐大小的要求，例如，32 位加载指令只能访问 4 字节对齐的内存地址，而 16 位加载指令只能访问 2 字节对齐的内存地址。

LoadStoreError
^^^^^^^^^^^^^^

应用程序尝试从仅支持 32 位加载/存储的内存区域执行 8 位或 16 位加载/存储操作，例如，解引用一个指向指令内存区域的 ``char*`` 指针就会导致这样的错误。

Unhandled debug exception
^^^^^^^^^^^^^^^^^^^^^^^^^

这后面通常会再跟一条消息::

    Debug exception reason: Stack canary watchpoint triggered (task_name)

此错误表示应用程序写入的位置越过了 ``task_name`` 任务堆栈的末尾，请注意，并非每次堆栈溢出都会触发此错误。任务有可能会绕过堆栈金丝雀（stack canary）的位置访问堆栈，在这种情况下，监视点就不会被触发。

Interrupt wdt timeout on CPU0 / CPU1
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

这表示发生了中断看门狗超时，详细信息请查阅 :doc:`看门狗 <../api-reference/system/wdts>` 文档。

Cache disabled but cached memory region accessed
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

在某些情况下，ESP-IDF 会暂时禁止通过高速缓存访问外部 SPI Flash 和 SPI RAM，例如在使用 spi_flash API 读取/写入/擦除/映射 SPI Flash 的时候。在这些情况下，任务会被挂起，并且未使用 ``ESP_INTR_FLAG_IRAM`` 注册的中断处理程序会被禁用。请确保任何使用此标志注册的中断处理程序所访问的代码和数据分别位于 IRAM 和 DRAM 中。更多详细信息请参阅 :ref:`SPI Flash API 文档 <iram-safe-interrupt-handlers>`。

其它严重错误
------------

欠压
^^^^

ESP32 内部集成掉电检测电路，并且会默认启用。如果电源电压低于安全值，掉电检测器可以触发系统复位。掉电检测器可以使用 :ref:`CONFIG_ESP32_BROWNOUT_DET` 和 :ref:`CONFIG_ESP32_BROWNOUT_DET_LVL_SEL` 这两个选项进行设置。
当掉电检测器被触发时，会打印如下信息::

    Brownout detector was triggered

芯片会在该打印信息结束后复位。

请注意，如果电源电压快速下降，则只能在控制台上看到部分打印信息。

Heap 不完整
^^^^^^^^^^^

ESP-IDF 堆的实现包含许多运行时的堆结构检查，可以在 menuconfig 中开启额外的检查（“Heap Poisoning”）。如果其中的某项检查失败，则会打印类似如下信息::

    CORRUPT HEAP: Bad tail at 0x3ffe270a. Expected 0xbaad5678 got 0xbaac5678
    assertion "head != NULL" failed: file "/Users/user/esp/esp-idf/components/heap/multi_heap_poisoning.c", line 201, function: multi_heap_free
    abort() was called at PC 0x400dca43 on core 0

更多详细信息，请查阅 :doc:`堆内存调试 <../api-reference/system/heap_debug>` 文档。

Stack 粉碎
^^^^^^^^^^

Stack 粉碎保护（基于 GCC ``-fstack-protector*`` 标志）可以通过 ESP-IDF 中的 :ref:`CONFIG_COMPILER_STACK_CHECK_MODE` 选项来开启。如果检测到 Stack 粉碎，则会打印类似如下的信息::

    Stack smashing protect failure!

    abort() was called at PC 0x400d2138 on core 0

    Backtrace: 0x4008e6c0:0x3ffc1780 0x4008e8b7:0x3ffc17a0 0x400d2138:0x3ffc17c0 0x400e79d5:0x3ffc17e0 0x400e79a7:0x3ffc1840 0x400e79df:0x3ffc18a0 0x400e2235:0x3ffc18c0 0x400e1916:0x3ffc18f0 0x400e19cd:0x3ffc1910 0x400e1a11:0x3ffc1930 0x400e1bb2:0x3ffc1950 0x400d2c44:0x3ffc1a80
    0

回溯信息会指明发生 Stack 粉碎的函数，建议检查函数中是否有代码访问本地数组时发生了越界。

