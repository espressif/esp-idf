***********
IDF Monitor
***********
:link_to_translation:`en:[English]`

IDF Monitor 工具是在 IDF 中调用 “make monitor” 目标时运行的 Python 程序。

它主要是一个串行终端程序，用于收发该端口的串行数据，IDF Monitor 同时兼具 IDF 的其他特性。

IDF Monitor 操作快捷键
===========================
- ``Ctrl-]`` 退出 monitor；
- ``Ctrl-T Ctrl-H`` 展示帮助页面和其他快捷键；
- 除了 ``Ctrl-]`` 和 ``Ctrl-T``，其他快捷键信号会通过串口发送到目标设备。

自动解码地址
=================
当 esp-idf 以 “0x4 _______” 形式打印一个十六进制的代码地址时，IDF Monitor 将使用 addr2line_ 来查找源代码的位置和函数名称。

.. highlight:: none

当某个 esp-idf 应用程序发生 crash 和 panic 事件之后，将产生如下的寄存器转储和回溯::

    Guru Meditation Error of type StoreProhibited occurred on core  0. Exception was unhandled.
    Register dump:
    PC      : 0x400f360d  PS      : 0x00060330  A0      : 0x800dbf56  A1      : 0x3ffb7e00
    A2      : 0x3ffb136c  A3      : 0x00000005  A4      : 0x00000000  A5      : 0x00000000
    A6      : 0x00000000  A7      : 0x00000080  A8      : 0x00000000  A9      : 0x3ffb7dd0
    A10     : 0x00000003  A11     : 0x00060f23  A12     : 0x00060f20  A13     : 0x3ffba6d0
    A14     : 0x00000047  A15     : 0x0000000f  SAR     : 0x00000019  EXCCAUSE: 0x0000001d
    EXCVADDR: 0x00000000  LBEG    : 0x4000c46c  LEND    : 0x4000c477  LCOUNT  : 0x00000000

    Backtrace: 0x400f360d:0x3ffb7e00 0x400dbf56:0x3ffb7e20 0x400dbf5e:0x3ffb7e40 0x400dbf82:0x3ffb7e60 0x400d071d:0x3ffb7e90

IDF Monitor 为转储补充如下信息::

    Guru Meditation Error of type StoreProhibited occurred on core  0. Exception was unhandled.
    Register dump:
    PC      : 0x400f360d  PS      : 0x00060330  A0      : 0x800dbf56  A1      : 0x3ffb7e00
    0x400f360d: do_something_to_crash at /home/gus/esp/32/idf/examples/get-started/hello_world/main/./hello_world_main.c:57
    (inlined by) inner_dont_crash at /home/gus/esp/32/idf/examples/get-started/hello_world/main/./hello_world_main.c:52
    A2      : 0x3ffb136c  A3      : 0x00000005  A4      : 0x00000000  A5      : 0x00000000
    A6      : 0x00000000  A7      : 0x00000080  A8      : 0x00000000  A9      : 0x3ffb7dd0
    A10     : 0x00000003  A11     : 0x00060f23  A12     : 0x00060f20  A13     : 0x3ffba6d0
    A14     : 0x00000047  A15     : 0x0000000f  SAR     : 0x00000019  EXCCAUSE: 0x0000001d
    EXCVADDR: 0x00000000  LBEG    : 0x4000c46c  LEND    : 0x4000c477  LCOUNT  : 0x00000000

    Backtrace: 0x400f360d:0x3ffb7e00 0x400dbf56:0x3ffb7e20 0x400dbf5e:0x3ffb7e40 0x400dbf82:0x3ffb7e60 0x400d071d:0x3ffb7e90
    0x400f360d: do_something_to_crash at /home/gus/esp/32/idf/examples/get-started/hello_world/main/./hello_world_main.c:57
    (inlined by) inner_dont_crash at /home/gus/esp/32/idf/examples/get-started/hello_world/main/./hello_world_main.c:52
    0x400dbf56: still_dont_crash at /home/gus/esp/32/idf/examples/get-started/hello_world/main/./hello_world_main.c:47
    0x400dbf5e: dont_crash at /home/gus/esp/32/idf/examples/get-started/hello_world/main/./hello_world_main.c:42
    0x400dbf82: app_main at /home/gus/esp/32/idf/examples/get-started/hello_world/main/./hello_world_main.c:33
    0x400d071d: main_task at /home/gus/esp/32/idf/components/esp32/./cpu_start.c:254

在后台，IDF Monitor 运行以下命令解码各个地址::

  xtensa-esp32-elf-addr2line -pfiaC -e build/PROJECT.elf ADDRESS


配置 GDBStub 供 GDB 调试
============================

默认情况下，如果 esp-idf 应用程序 crash, panic 处理函数打印上述的寄存器和堆栈转储，然后重启。

您可以选择配置 panic 处理函数，使其运行串行的 "gdb stub"。该程序可以与 gdb 调试器通信，提供内存，变量，栈帧读取的功能。虽然这不像 JTAG 调试那样通用，但您不需要使用特殊硬件。

要启用 gdbstub，运行 ``make menuconfig`` 并将 :ref:`CONFIG_ESP32_PANIC` 选项设置为 ``Invoke GDBStub``。

如果启用此选项并且 IDF Monitor 发现 gdbstub 已加载，它将自动暂停串口监控并使用正确的参数运行 GDB。GDB 退出后，电路板将通过 RTS 串行线路复位（如果已连接）。

IDF Monitor 在后台运行的命令是::

  xtensa-esp32-elf-gdb -ex "set serial baud BAUD" -ex "target remote PORT" -ex interrupt build/PROJECT.elf


快速编译与烧录
=================

使用快捷键 ``Ctrl-T Ctrl-A`` 暂停 IDF Monitor，并运行 ``make flash`` 目标，然后 IDF Monitor 就会恢复正常。任何更改的源文件将在烧录之前重新编译。

使用快捷键 ``Ctrl-T Ctrl-A`` 暂停 IDF Monitor，并运行 ``make app-flash`` 目标，然后 IDF Monitor 就会恢复正常。这与 ``make flash`` 类似，但只有主应用程序被编译和重新烧录。

快速重置
======================
键盘快捷键 ``Ctrl-T Ctrl-R`` 将通过 RTS 线（如果已连接）重置开发板。


暂停应用程序
=====================

通过快捷键 ``Ctrl-T Ctrl-P`` 重启进入 bootloader，开发板将不运行任何程序。等待其他设备启动时可以使用此操作。使用快捷键 ``Ctrl-T Ctrl-R`` 重新启动应用程序。

输出显示开关
================

暂停屏幕上的输出，以查看之前日志，可以使用快捷键 ``Ctrl-T Ctrl-Y`` 切换显示（当显示关闭时丢弃所有的串行数据）。这样您可以停下来查看日志，不必关闭显示器就可以快速恢复打印。

Simple Monitor
=======================

较早版本的 ESP-IDF 使用 pySerial_ 命令行程序 miniterm_ 作为串行控制台程序。

这个程序仍然可以通过 ``make simple_monitor`` 运行。

IDF Monitor 基于 miniterm 并使用相同的快捷键。

IDF Monitor 已知问题
=============================

Windows 环境下已知问题
~~~~~~~~~~~~~~~~~~~~~~~~~~

- 如果您使用支持 idf_monitor 的 Windows 环境，却收到错误 "winpty: command not found”，请运行 ``pacman -S winpty`` 进行修复。
- 由于 Windows 控制台的限制，gdb 中的箭头键和其他一些特殊键不起作用。
- 偶尔当 “make” 退出时，可能会在 idf_monitor 恢复之前暂停 30 秒。
- 偶尔当 "gdb" 运行时，它可能会暂停一段时间，然后才开始与 gdbstub 进行通信。


.. _addr2line: https://sourceware.org/binutils/docs/binutils/addr2line.html
.. _gdb: https://sourceware.org/gdb/download/onlinedocs/
.. _pySerial: https://github.com/pyserial/pyserial
.. _miniterm: https://pyserial.readthedocs.org/en/latest/tools.html#module-serial.tools.miniterm


