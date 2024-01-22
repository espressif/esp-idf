***********************
IDF 监视器
***********************

:link_to_translation:`en:[English]`

IDF 监视器是一个串行终端程序，使用了 esp-idf-monitor_ 包，用于收发目标设备串口的串行数据，IDF 监视器同时还兼具 ESP-IDF 的其他特性。

在 ESP-IDF 中调用 ``idf.py monitor`` 可以启用此监视器。

操作快捷键
==================

为了方便与 IDF 监视器进行交互，请使用表中给出的快捷键。这些快捷键可以自定义，请查看 `配置文件`_ 章节了解详情。

.. list-table::
   :header-rows: 1
   :widths: 15 25 55

   * - 快捷键
     - 操作
     - 描述
   * - Ctrl + ]
     - 退出监视器程序
     -
   * - Ctrl + T
     - 菜单退出键
     - 按下如下给出的任意键之一，并按指示操作。
   * - * Ctrl + T
     - 将菜单字符发送至远程
     -
   * - * Ctrl + ]
     - 将 exit 字符发送至远程
     -
   * - * Ctrl + P
     - 重置目标设备，进入引导加载程序，通过 RTS 和 DTR 线暂停应用程序
     - 重置目标设备，通过 RTS 和 DTR 线（如已连接）进入引导加载程序。这会阻止开发板运行任何程序，在等待其他设备启动时可以使用此操作。更多详细信息，请参考 :ref:`target-reset-into-bootloader`。
   * - * Ctrl + R
     - 通过 RTS 线重置目标设备
     - 重置设备，并通过 RTS 线（如已连接）重新启动应用程序。
   * - * Ctrl + F
     - 编译并烧录此项目
     - 暂停 idf_monitor，运行 ``flash`` 目标，然后恢复 idf_monitor。任何改动的源文件都会被重新编译，然后重新烧录。如果 idf_monitor 是以参数 ``-E`` 启动的，则会运行目标 ``encrypted-flash``。
   * - * Ctrl + A (或者 A)
     - 仅编译及烧录应用程序
     - 暂停 idf_monitor，运行 ``app-flash`` 目标，然后恢复 idf_monitor。 这与 ``flash`` 类似，但只有主应用程序被编译并被重新烧录。如果 idf_monitor 是以参数 ``-E`` 启动的，则会运行目标 ``encrypted-flash``。
   * - * Ctrl + Y
     - 停止/恢复在屏幕上打印日志输出
     - 激活时，会丢弃所有传入的串行数据。允许在不退出监视器的情况下快速暂停和检查日志输出。
   * - * Ctrl + L
     - 停止/恢复向文件写入日志输出
     - 在工程目录下创建一个文件，用于写入日志输出。可使用快捷键停止/恢复该功能（退出 IDF 监视器也会终止该功能）。
   * - * Ctrl + I (或者 I)
     - 停止/恢复打印时间标记
     - IDF 监视器可以在每一行的开头打印一个时间标记。时间标记的格式可以通过 ``--timestamp-format`` 命令行参数来改变。
   * - * Ctrl + H (或者 H)
     - 显示所有快捷键
     -
   * - * Ctrl + X (或者 X)
     - 退出监视器程序
     -
   * - Ctrl + C
     - 中断正在运行的应用程序
     - 暂停 IDF 监视器并运行 GDB_ 项目调试器，从而在运行时调试应用程序。这需要启用 :ref: `CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME` 选项。

除了 ``Ctrl-]`` 和 ``Ctrl-T``，其他快捷键信号会通过串口发送到目标设备。


兼具 ESP-IDF 特性
=========================

自动解码地址
~~~~~~~~~~~~~~~~

每当芯片输出指向可执行代码的十六进制地址时，IDF 监视器将查找该地址在源代码中的位置（文件名和行号），并在下一行用黄色打印出该位置。

.. highlight:: none

.. only:: CONFIG_IDF_TARGET_ARCH_XTENSA

  ESP-IDF 应用程序发生 crash 和 panic 事件时，将产生如下的寄存器转储和回溯::

    Guru Meditation Error of type StoreProhibited occurred on core  0. Exception was unhandled.
    Register dump:
    PC      : 0x400f360d  PS      : 0x00060330  A0      : 0x800dbf56  A1      : 0x3ffb7e00
    A2      : 0x3ffb136c  A3      : 0x00000005  A4      : 0x00000000  A5      : 0x00000000
    A6      : 0x00000000  A7      : 0x00000080  A8      : 0x00000000  A9      : 0x3ffb7dd0
    A10     : 0x00000003  A11     : 0x00060f23  A12     : 0x00060f20  A13     : 0x3ffba6d0
    A14     : 0x00000047  A15     : 0x0000000f  SAR     : 0x00000019  EXCCAUSE: 0x0000001d
    EXCVADDR: 0x00000000  LBEG    : 0x4000c46c  LEND    : 0x4000c477  LCOUNT  : 0x00000000

    Backtrace: 0x400f360d:0x3ffb7e00 0x400dbf56:0x3ffb7e20 0x400dbf5e:0x3ffb7e40 0x400dbf82:0x3ffb7e60 0x400d071d:0x3ffb7e90

  IDF 监视器为寄存器转储补充如下信息::

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
    0x400d071d: main_task at /home/gus/esp/32/idf/components/{IDF_TARGET_PATH_NAME}/./cpu_start.c:254

.. only:: CONFIG_IDF_TARGET_ARCH_RISCV

  ESP-IDF 应用程序发生 crash 和 panic 事件时，将产生如下的寄存器转储和回溯::

      abort() was called at PC 0x42067cd5 on core 0

      Stack dump detected
      Core  0 register dump:
      MEPC    : 0x40386488  RA      : 0x40386b02  SP      : 0x3fc9a350  GP      : 0x3fc923c0
      TP      : 0xa5a5a5a5  T0      : 0x37363534  T1      : 0x7271706f  T2      : 0x33323130
      S0/FP   : 0x00000004  S1      : 0x3fc9a3b4  A0      : 0x3fc9a37c  A1      : 0x3fc9a3b2
      A2      : 0x00000000  A3      : 0x3fc9a3a9  A4      : 0x00000001  A5      : 0x3fc99000
      A6      : 0x7a797877  A7      : 0x76757473  S2      : 0xa5a5a5a5  S3      : 0xa5a5a5a5
      S4      : 0xa5a5a5a5  S5      : 0xa5a5a5a5  S6      : 0xa5a5a5a5  S7      : 0xa5a5a5a5
      S8      : 0xa5a5a5a5  S9      : 0xa5a5a5a5  S10     : 0xa5a5a5a5  S11     : 0xa5a5a5a5
      T3      : 0x6e6d6c6b  T4      : 0x6a696867  T5      : 0x66656463  T6      : 0x62613938
      MSTATUS : 0x00001881  MTVEC   : 0x40380001  MCAUSE  : 0x00000007  MTVAL   : 0x00000000

      MHARTID : 0x00000000

      Stack memory:
      3fc9a350: 0xa5a5a5a5 0xa5a5a5a5 0x3fc9a3b0 0x403906cc 0xa5a5a5a5 0xa5a5a5a5 0xa5a5a5a50
      3fc9a370: 0x3fc9a3b4 0x3fc9423c 0x3fc9a3b0 0x726f6261 0x20292874 0x20736177 0x6c6c61635
      3fc9a390: 0x43502074 0x34783020 0x37363032 0x20356463 0x63206e6f 0x2065726f 0x000000300
      3fc9a3b0: 0x00000030 0x36303234 0x35646337 0x3c093700 0x0000002a 0xa5a5a5a5 0x3c0937f48
      3fc9a3d0: 0x00000001 0x3c0917f8 0x3c0937d4 0x0000002a 0xa5a5a5a5 0xa5a5a5a5 0xa5a5a5a5e
      3fc9a3f0: 0x0001f24c 0x000006c8 0x00000000 0x0001c200 0xffffffff 0xffffffff 0x000000200
      3fc9a410: 0x00001000 0x00000002 0x3c093818 0x3fccb470 0xa5a5a5a5 0xa5a5a5a5 0xa5a5a5a56
      .....

  通过分析堆栈转储 IDF 监视器为寄存器转储补充如下信息::

    abort() was called at PC 0x42067cd5 on core 0
    0x42067cd5: __assert_func at /builds/idf/crosstool-NG/.build/riscv32-esp-elf/src/newlib/newlib/libc/stdlib/assert.c:62 (discriminator 8)

    Stack dump detected
    Core  0 register dump:
    MEPC    : 0x40386488  RA      : 0x40386b02  SP      : 0x3fc9a350  GP      : 0x3fc923c0
    0x40386488: panic_abort at /home/marius/esp-idf_2/components/esp_system/panic.c:367

    0x40386b02: rtos_int_enter at /home/marius/esp-idf_2/components/freertos/port/riscv/portasm.S:35

    TP      : 0xa5a5a5a5  T0      : 0x37363534  T1      : 0x7271706f  T2      : 0x33323130
    S0/FP   : 0x00000004  S1      : 0x3fc9a3b4  A0      : 0x3fc9a37c  A1      : 0x3fc9a3b2
    A2      : 0x00000000  A3      : 0x3fc9a3a9  A4      : 0x00000001  A5      : 0x3fc99000
    A6      : 0x7a797877  A7      : 0x76757473  S2      : 0xa5a5a5a5  S3      : 0xa5a5a5a5
    S4      : 0xa5a5a5a5  S5      : 0xa5a5a5a5  S6      : 0xa5a5a5a5  S7      : 0xa5a5a5a5
    S8      : 0xa5a5a5a5  S9      : 0xa5a5a5a5  S10     : 0xa5a5a5a5  S11     : 0xa5a5a5a5
    T3      : 0x6e6d6c6b  T4      : 0x6a696867  T5      : 0x66656463  T6      : 0x62613938
    MSTATUS : 0x00001881  MTVEC   : 0x40380001  MCAUSE  : 0x00000007  MTVAL   : 0x00000000

    MHARTID : 0x00000000

    Backtrace:
    panic_abort (details=details@entry=0x3fc9a37c "abort() was called at PC 0x42067cd5 on core 0") at /home/marius/esp-idf_2/components/esp_system/panic.c:367
    367     *((int *) 0) = 0; // NOLINT(clang-analyzer-core.NullDereference) should be an invalid operation on targets
    #0  panic_abort (details=details@entry=0x3fc9a37c "abort() was called at PC 0x42067cd5 on core 0") at /home/marius/esp-idf_2/components/esp_system/panic.c:367
    #1  0x40386b02 in esp_system_abort (details=details@entry=0x3fc9a37c "abort() was called at PC 0x42067cd5 on core 0") at /home/marius/esp-idf_2/components/esp_system/system_api.c:108
    #2  0x403906cc in abort () at /home/marius/esp-idf_2/components/newlib/abort.c:46
    #3  0x42067cd8 in __assert_func (file=file@entry=0x3c0937f4 "", line=line@entry=42, func=func@entry=0x3c0937d4 <__func__.8540> "", failedexpr=failedexpr@entry=0x3c0917f8 "") at /builds/idf/crosstool-NG/.build/riscv32-esp-elf/src/newlib/newlib/libc/stdlib/assert.c:62
    #4  0x4200729e in app_main () at ../main/iperf_example_main.c:42
    #5  0x42086cd6 in main_task (args=<optimized out>) at /home/marius/esp-idf_2/components/freertos/port/port_common.c:133
    #6  0x40389f3a in vPortEnterCritical () at /home/marius/esp-idf_2/components/freertos/port/riscv/port.c:129

IDF 监视器在后台运行以下命令，解码各地址::

  {IDF_TARGET_TOOLCHAIN_PREFIX}-addr2line -pfiaC -e build/PROJECT.elf ADDRESS

.. only:: CONFIG_IDF_TARGET_ARCH_XTENSA

  如果在应用程序源代码中找不到匹配的地址，IDF 监视器还会检查 ROM 代码。此时不会打印源文件名和行号，只显示 ``函数名 in ROM``::

    abort() was called at PC 0x40007c69 on core 0
    0x40007c69: ets_write_char in ROM

    Backtrace: 0x40081656:0x3ffb4ac0 0x40085729:0x3ffb4ae0 0x4008a7ce:0x3ffb4b00 0x40007c69:0x3ffb4b70 0x40008148:0x3ffb4b90 0x400d51d7:0x3ffb4c20 0x400e31bc:0x3ffb4c50 0x40087bc5:0x3ffb4c80
    0x40081656: panic_abort at /Users/espressif/esp-idf/components/esp_system/panic.c:452
    0x40085729: esp_system_abort at /Users/espressif/esp-idf/components/esp_system/port/esp_system_chip.c:90
    0x4008a7ce: abort at /Users/espressif/esp-idf/components/newlib/abort.c:38
    0x40007c69: ets_write_char in ROM
    0x40008148: ets_printf in ROM
    0x400d51d7: app_main at /Users/espressif/esp-idf/examples/get-started/hello_world/main/hello_world_main.c:49
    0x400e31bc: main_task at /Users/espressif/esp-idf/components/freertos/app_startup.c:208 (discriminator 13)
    0x40087bc5: vPortTaskWrapper at /Users/espressif/esp-idf/components/freertos/FreeRTOS-Kernel/portable/xtensa/port.c:162
    .....

.. only:: CONFIG_IDF_TARGET_ARCH_RISCV

  如果在应用程序源代码中找不到匹配的地址，IDF 监视器还会检查 ROM 代码。此时不会打印源文件名和行号，只显示 ``函数名 in ROM``::

    abort() was called at PC 0x400481c1 on core 0
    0x400481c1: ets_rsa_pss_verify in ROM

    Stack dump detected
    Core  0 register dump:
    MEPC    : 0x4038051c  RA      : 0x40383840  SP      : 0x3fc8f6b0  GP      : 0x3fc8b000
    0x4038051c: panic_abort at /Users/espressif/esp-idf/components/esp_system/panic.c:452
    0x40383840: __ubsan_include at /Users/espressif/esp-idf/components/esp_system/ubsan.c:313

    TP      : 0x3fc8721c  T0      : 0x37363534  T1      : 0x7271706f  T2      : 0x33323130
    S0/FP   : 0x00000004  S1      : 0x3fc8f714  A0      : 0x3fc8f6dc  A1      : 0x3fc8f712
    A2      : 0x00000000  A3      : 0x3fc8f709  A4      : 0x00000001  A5      : 0x3fc8c000
    A6      : 0x7a797877  A7      : 0x76757473  S2      : 0x00000000  S3      : 0x3fc8f750
    S4      : 0x3fc8f7e4  S5      : 0x00000000  S6      : 0x400481b0  S7      : 0x3c025841
    0x400481b0: ets_rsa_pss_verify in ROM
    .....

ROM ELF 文件会根据 ``IDF_PATH`` 和 ``ESP_ROM_ELF_DIR`` 环境变量的路径自动加载。如需覆盖此行为，可以通过调用 ``esp_idf_monitor`` 并指定特定的 ROM ELF 文件路径：``python -m esp_idf_monitor --rom-elf-file [ROM ELF 文件的路径]``。

.. note::

    将环境变量 ``ESP_MONITOR_DECODE`` 设置为 ``0`` 或者调用 esp_idf_monitor 的特定命令行选项 ``python -m esp_idf_monitor --disable-address-decoding`` 来禁止地址解码。

连接时复位目标芯片
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

默认情况下，IDF 监视器会在目标芯片连接时通过 DTR 和 RTS 串行线自动复位芯片。要防止 IDF 监视器在连接时自动复位，请在调用 IDF 监视器时加上选项 ``--no-reset``，如 ``idf.py monitor --no-reset``，或者将环境变量 ``ESP_IDF_MONITOR_NO_RESET`` 设置成 1。

.. note::

    ``--no-reset`` 选项在 IDF 监视器连接到特定端口时可以实现同样的效果，如 ``idf.py monitor --no-reset -p [PORT]``。


.. _target-reset-into-bootloader:

复位目标到引导加载程序
~~~~~~~~~~~~~~~~~~~~~~~~~~~

IDF 监视器可以通过预定义的复位序列将芯片复位到引导加载程序，该序列已经经过调整，可以在大多数环境中正常工作。此外，用户可以设置自定义复位序列。通过对复位序列进行微调，使其适应各种情况。

使用预定义的复位序列
--------------------------------

IDF 监视器的默认复位序列可在大多数环境中使用。使用默认序列复位芯片到引导加载程序中，无需进行额外配置。

自定义复位序列
---------------------

对于高级用户或特定用例，IDF 监视器支持使用 :ref:`configuration-file` 配置自定义复位序列。这在默认序列可能不足的极端情况下特别有用。

复位序列可通过以下格式的字符串定义：

- 各个命令由 ``|`` 分隔（例如 ``R0|D1|W0.5``）。
- 命令（例如 ``R0``）由代码（``R``）和参数（``0``）定义。

.. list-table::
    :header-rows: 1
    :widths: 15 50 35
    :align: center

    * - 代码
      - 操作
      - 参数
    * - D
      - 设置 DTR 控制线
      - ``1``/``0``
    * - R
      - 设置 RTS 控制线
      - ``1``/``0``
    * - U
      - 同时设置 DTR 和 RTS 控制线（仅适用于类 Unix 系统）
      - ``0,0``/``0,1``/``1,0``/``1,1``
    * - W
      - 等待 ``N`` 秒（其中 ``N`` 为浮点数）
      - N

示例：

.. code-block:: ini

    [esp-idf-monitor]
    custom_reset_sequence = U0,1|W0.1|D1|R0|W0.5|D0

有关更多详细信息，请参阅 Esptool 文档中 `custom reset sequence`_ 章节。请注意，IDF 监视器只使用了 Esptool 配置中的 ``custom_reset_sequence`` 值，其他值会被 IDF 监视器忽略。

IDF 监视器和 Esptool 之间共享配置
----------------------------------------------

自定义复位序列的配置可以在 IDF 监视器和 Esptool 之间的共享配置文件中指定。在这种情况下，为了使两个工具都能识别配置文件，其名称应为 ``setup.cfg`` 或 ``tox.ini``。

共享配置文件的示例：

.. code-block:: ini

    [esp-idf-monitor]
    menu_key = T
    skip_menu_key = True

    [esptool]
    custom_reset_sequence = U0,1|W0.1|D1|R0|W0.5|D0

.. note::

    当在 ``[esp-idf-monitor]`` 部分和 ``[esptool]`` 部分都使用 ``custom_reset_sequence`` 参数时，IDF 监视器会优先使用 ``[esp-idf-monitor]`` 部分的配置。``[esptool]`` 部分中任何与之冲突的配置都将被忽略。

    当配置分散在多个文件中时，此优先规则也适用。全局 esp-idf-monitor 配置将优先于本地 esptool 配置。


配置 GDBStub 以启用 GDB
~~~~~~~~~~~~~~~~~~~~~~~~~

GDBStub 支持在运行时进行调试。GDBStub 在目标上运行，并通过串口连接到主机从而接收调试命令。GDBStub 支持读取内存和变量、检查调用堆栈帧等命令。虽然没有 JTAG 调试通用，但由于 GDBStub 完全通过串行端口完成通信，故不需要使用特殊硬件（如 JTAG/USB 桥接器）。

通过设置 :ref:`CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME`，可以将目标配置为在后台运行 GDBStub。GDBStub 将保持在后台运行，直到通过串行端口发送 ``Ctrl+C`` 导致应用程序中断（即停止程序执行），从而让 GDBStub 处理调试命令。

此外，还可以通过设置 :ref:`CONFIG_ESP_SYSTEM_PANIC` 为 ``GDBStub on panic`` 来配置 panic 处理程序，使其在发生 crash 事件时运行 GDBStub。当 crash 发生时，GDBStub 将通过串口输出特殊的字符串模式，表示 GDBStub 正在运行。

无论是通过发送 ``Ctrl+C`` 还是收到特殊字符串模式，IDF 监视器都会自动启动 GDB，从而让用户发送调试命令。GDB 退出后，通过 RTS 串口线复位目标。如果未连接 RTS 串口线，请按复位键，手动复位开发板。

.. note::

    IDF 监视器在后台运行如下命令启用 GDB::

        {IDF_TARGET_TOOLCHAIN_PREFIX}-gdb -ex "set serial baud BAUD" -ex "target remote PORT" -ex interrupt build/PROJECT.elf :idf_target:`Hello NAME chip`


输出筛选
~~~~~~~~~~~~~~~~

可以调用 ``idf.py monitor --print-filter="xyz"`` 启动 IDF 监视器，其中，``--print-filter`` 是输出筛选的参数。参数默认值为空字符串，即打印所有内容。支持使用环境变量 ``ESP_IDF_MONITOR_PRINT_FILTER`` 调整筛选设置。

.. note::

   同时使用环境变量 ``ESP_IDF_MONITOR_PRINT_FILTER`` 和参数 ``--print-filter`` 时，通过命令行输入的 CLI 参数 ``--print-filter`` 优先级更高。

若需对打印内容设置限制，可指定 ``<tag>:<log_level>`` 等选项，其中 ``<tag>`` 是标签字符串，``<log_level>`` 是 ``{N, E, W, I, D, V, *}`` 集合中的一个字母，指的是 :doc:`日志 <../../api-reference/system/log>` 级别。

例如，``--print_filter="tag1:W"`` 只匹配并打印 ``ESP_LOGW("tag1", ...)`` 所写的输出，或者写在较低日志详细度级别的输出，即 ``ESP_LOGE("tag1", ...)``。请勿指定 ``<log_level>`` 或使用详细级别默认值 ``*``。

.. note::

   编译时，可以使用主日志在 :doc:`日志库 <../../api-reference/system/log>` 中禁用不需要的输出。也可以使用 IDF 监视器筛选输出来调整筛选设置，且无需重新编译应用程序。

应用程序标签不能包含空格、星号 ``*``、冒号 ``:``，以便兼容输出筛选功能。

如果应用程序输出的最后一行后面没有回车，可能会影响输出筛选功能，即，监视器开始打印该行，但后来发现该行不应该被写入。这是一个已知问题，可以通过添加回车来避免此问题（特别是在没有输出紧跟其后的情况下）。

筛选规则示例
~~~~~~~~~~~~~~~~

- ``*`` 可用于匹配任何类型标签。但 ``--print_filter="*:I tag1:E"`` 打印关于 ``tag1`` 的输出时会报错，这是因为 ``tag1`` 规则比 ``*`` 规则的优先级高。
- 默认规则（空）等价于 ``*:V``，因为在详细级别或更低级别匹配任意标签即意味匹配所有内容。
- ``"*:N"`` 不仅抑制了日志功能的输出，也抑制了 ``printf`` 的打印输出。为了避免这一问题，请使用 ``*:E`` 或更高的冗余级别。
- 规则 ``"tag1:V"``、``"tag1:v"``、``"tag1:"``、``"tag1:*"`` 和 ``"tag1"`` 等同。
- 规则 ``"tag1:W tag1:E"`` 等同于 ``"tag1:E"``，这是因为后续出现的具有相同名称的标签会覆盖掉前一个标签。
- 规则 ``"tag1:I tag2:W"`` 仅在 Info 详细度级别或更低级别打印 ``tag1``，在 Warning 详细度级别或更低级别打印 ``tag2``。
- 规则 ``"tag1:I tag2:W tag3:N"`` 在本质上等同于上一规则，这是因为 ``tag3:N`` 指定 ``tag3`` 不打印。
- ``tag3:N`` 在规则 ``"tag1:I tag2:W tag3:N *:V"`` 中更有意义，这是因为如果没有 ``tag3:N``，``tag3`` 信息就可能打印出来了；``tag1`` 和 ``tag2`` 错误信息会打印在指定的详细度级别（或更低级别），并默认打印所有内容。


高级筛选规则示例
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

如下日志是在没有设置任何筛选选项的情况下获得的::

    load:0x40078000,len:13564
    entry 0x40078d4c
    E (31) esp_image: image at 0x30000 has invalid magic byte
    W (31) esp_image: image at 0x30000 has invalid SPI mode 255
    E (39) boot: Factory app partition is not bootable
    I (568) cpu_start: Pro cpu up.
    I (569) heap_init: Initializing. RAM available for dynamic allocation:
    I (603) cpu_start: Pro cpu start user code
    D (309) light_driver: [light_init, 74]:status: 1, mode: 2
    D (318) vfs: esp_vfs_register_fd_range is successful for range <54; 64) and VFS ID 1
    I (328) wifi: wifi driver task: 3ffdbf84, prio:23, stack:4096, core=0

``--print_filter="wifi esp_image:E light_driver:I"`` 筛选选项捕获的输出如下所示::

    E (31) esp_image: image at 0x30000 has invalid magic byte
    I (328) wifi: wifi driver task: 3ffdbf84, prio:23, stack:4096, core=0

``--print_filter="light_driver:D esp_image:N boot:N cpu_start:N vfs:N wifi:N *:V"`` 选项的输出如下::

    load:0x40078000,len:13564
    entry 0x40078d4c
    I (569) heap_init: Initializing. RAM available for dynamic allocation:
    D (309) light_driver: [light_init, 74]:status: 1, mode: 2


.. _configuration-file:

配置文件
========

``esp-idf-monitor`` 使用 `C0 控制字符`_ 与控制台进行交互。配置文件中的字符会被转换为对应的 C0 控制代码。可用字符包括英文字母 (A-Z) 和特殊符号：``[``、``]``、``\``、``^``、和 ``_``.

.. warning::

    注意，一些字符可能无法在所有平台通用，或被保留作为其他用途的快捷键。请谨慎使用此功能。


文件位置
~~~~~~~~~~

配置文件的默认名称为 ``esp-idf-monitor.cfg``。首先，在 ``esp-idf-monitor`` 路径中检测配置文件并运行。

如果此目录中没有检测到配置文件，则检查当前用户操作系统的配置目录：

  - Linux: ``/home/<user>/.config/esp-idf-monitor/``
  - MacOS ``/Users/<user>/.config/esp-idf-monitor/``
  - Windows: ``c:\Users\<user>\AppData\Local\esp-idf-monitor\``

如仍未检测到配置文件，会最后再检查主目录：

  - Linux: ``/home/<user>/``
  - MacOS ``/Users/<user>/``
  - Windows: ``c:\Users\<user>\``

在 Windows 中，可以使用 ``HOME`` 或 ``USERPROFILE`` 环境变量设置主目录，因此，Windows 配置目录的位置也取决于这些变量。

还可以使用 ``ESP_IDF_MONITOR_CFGFILE`` 环境变量为配置文件指定一个不同的位置，例如 ``ESP_IDF_MONITOR_CFGFILE = ~/custom_config.cfg``。这一设置的检测优先级高于上述所有位置检测的优先级。

如果没有使用其他配置文件，``esp-idf-monitor`` 会从其他常用的配置文件中读取设置。如果存在 ``setup.cfg`` 或 ``tox.ini`` 文件，``esp-idf-monitor`` 会自动从这些文件中读取设置。

配置选项
~~~~~~~~~~

下表列出了可用的配置选项：

.. list-table::
    :header-rows: 1
    :widths: 30 50 20
    :align: center

    * - 选项名称
      - 描述
      - 默认值
    * - menu_key
      - 访问主菜单
      - ``T``
    * - exit_key
      - 退出监视器
      - ``]``
    * - chip_reset_key
      - 初始化芯片重置
      - ``R``
    * - recompile_upload_key
      - 重新编译并上传
      - ``F``
    * - recompile_upload_app_key
      - 仅重新编译并上传应用程序
      - ``A``
    * - toggle_output_key
      - 切换输出显示
      - ``Y``
    * - toggle_log_key
      - 切换日志功能
      - ``L``
    * - toggle_timestamp_key
      - 切换时间戳显示
      - ``I``
    * - chip_reset_bootloader_key
      - 将芯片重置为引导加载模式
      - ``P``
    * - exit_menu_key
      - 从菜单中退出监视器
      - ``X``
    * - skip_menu_key
      - 设置使用菜单命令时无需按下主菜单键
      - ``False``
    * - custom_reset_sequence
      - 复位目标到引导加载程序的自定义复位序列
      - 无默认值


语法
~~~~

配置文件为 .ini 文件格式，必须以 ``[esp-idf-monitor]`` 标头引入才能被识别为有效文件。以下语法以“配置名称 = 配置值”形式列出。以 ``#`` 或 ``;`` 开头的行是注释，将被忽略。

.. code-block:: ini

    # esp-idf-monitor.cfg file to configure internal settings of esp-idf-monitor
    [esp-idf-monitor]
    menu_key = T
    exit_key = ]
    chip_reset_key = R
    recompile_upload_key = F
    recompile_upload_app_key = A
    toggle_output_key = Y
    toggle_log_key = L
    toggle_timestamp_key = I
    chip_reset_bootloader_key = P
    exit_menu_key = X
    skip_menu_key = False


IDF 监视器已知问题
=================================

如果在使用 IDF 监视器过程中遇到任何问题，请查看我们的 `GitHub 仓库 <https://github.com/espressif/esp-idf-monitor/issues>`_ 以获取已知问题列表及其当前状态。如果遇到的问题没有相关记录，请创建一个新的问题报告。

.. _addr2line: https://sourceware.org/binutils/docs/binutils/addr2line.html
.. _esp-idf-monitor: https://github.com/espressif/esp-idf-monitor
.. _gdb: https://sourceware.org/gdb/download/onlinedocs/
.. _pySerial: https://github.com/pyserial/pyserial
.. _miniterm: https://pyserial.readthedocs.org/en/latest/tools.html#module-serial.tools.miniterm
.. _C0 控制字符: https://zh.wikipedia.org/wiki/C0%E4%B8%8EC1%E6%8E%A7%E5%88%B6%E5%AD%97%E7%AC%A6#C0_(ASCII%E5%8F%8A%E5%85%B6%E6%B4%BE%E7%94%9F)
.. _custom reset sequence: https://docs.espressif.com/projects/esptool/en/latest/{IDF_TARGET_PATH_NAME}/esptool/configuration-file.html#custom-reset-sequence
