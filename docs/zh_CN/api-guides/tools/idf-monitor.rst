*******************
IDF 监视器
*******************

:link_to_translation:`en:[English]`

IDF 监视器是一个串行终端程序，用于收发目标设备串口的串行数据，IDF 监视器同时还兼具 IDF 的其他特性。

在 IDF 中调用以下目标函数可以启用此监视器：

- **若使用 CMake 编译系统，则请调用**：``idf.py monitor``
- **若使用传统 GNU Make 编译系统，请调用**：``make monitor``

操作快捷键
==================

为了方便与 IDF 监视器进行交互，请使用表中给出的快捷键。

.. list-table::                                        
   :header-rows: 1                                     
   :widths: 15 25 55                                      
                                        
   * - 快捷键
     - 操作
     - 描述                                        
   * - Ctrl+]
     - 退出监视器程序
     -                                      
   * - Ctrl+T
     - 菜单退出键
     - 按下如下给出的任意键之一，并按指示操作。                                       
   * - * Ctrl+T
     - 将菜单字符发送至远程
     -                                      
   * - * Ctrl+]
     - 将 exit 字符发送至远程
     -                                      
   * - * Ctrl+P
     - 重置目标设备，进入 Bootloader，通过 RTS 线暂停应用程序
     - 重置目标设备，通过 RTS 线（如已连接）进入 Bootloader，此时开发板不运行任何程序。等待其他设备启动时可以使用此操作。                                     
   * - * Ctrl+R
     - 通过 RTS 线重置目标设备
     - 重置设备，并通过 RTS 线（如已连接）重新启动应用程序。                                      
   * - * Ctrl+F
     - 编译并烧录此项目
     - 暂停 idf_monitor，运行 ``flash`` 目标，然后恢复 idf_monitor。任何改动的源文件都会被重新编译，然后重新烧录。如果 idf_monitor 是以参数 ``-E`` 启动的，则会运行目标 ``encrypted-flash``。                                    
   * - * Ctrl+A (或者 A)
     - 仅编译及烧录应用程序
     - 暂停 idf_monitor，运行 ``app-flash`` 目标，然后恢复 idf_monitor。 这与 ``flash`` 类似，但只有主应用程序被编译并被重新烧录。如果 idf_monitor 是以参数 ``-E`` 启动的，则会运行目标 ``encrypted-flash``。                                    
   * - * Ctrl+Y
     - 停止/恢复在屏幕上打印日志输出
     - 激活时，会丢弃所有传入的串行数据。允许在不退出监视器的情况下快速暂停和检查日志输出。                   
   * - * Ctrl+L
     - 停止/恢复向文件写入日志输出
     - 在工程目录下创建一个文件，用于写入日志输出。可使用快捷键停止/恢复该功能（退出 IDF 监视器也会终止该功能）                                     
   * - * Ctrl+H (或者 H)
     - 显示所有快捷键
     -                                      
   * - * Ctrl+X (或者 X)
     - 退出监视器程序
     -  

除了 ``Ctrl-]`` 和 ``Ctrl-T``，其他快捷键信号会通过串口发送到目标设备。


兼具 IDF 特性
=====================

自动解码地址
~~~~~~~~~~~~~~~~

ESP-IDF 输出形式为 ``0x4_______`` 的十六进制代码地址后，IDF 监视器将使用 addr2line_ 查找该地址在源代码中的位置和对应的函数名。

.. highlight:: none

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

IDF 监视器在后台运行以下命令，解码各地址::

  xtensa-{IDF_TARGET_TOOLCHAIN_NAME}-elf-addr2line -pfiaC -e build/PROJECT.elf ADDRESS


配置 GDBStub 以启用 GDB
~~~~~~~~~~~~~~~~~~~~~~~~~

默认情况下，如果 ESP-IDF 应用程序发生 crash 事件，panic 处理器将在串口上打印相关寄存器和堆栈转储（类似上述情况），然后重置开发板。

或者选择配置 panic 处理器以运行 GDBStub，GDBStub 工具可以与 GDB_ 项目调试器进行通信，允许读取内存、检查调用堆栈帧和变量等。GDBStub 虽然没有 JTAG 通用，但不需要使用特殊硬件。

如需启用 GDBStub，请运行 ``idf.py menuconfig`` （适用于 CMake 编译系统），并将 :ref:`CONFIG_ESP_SYSTEM_PANIC` 选项设置为 ``Invoke GDBStub``。

在这种情况下，如果 panic 处理器被触发，只要 IDF 监视器监控到 GDBStub 已经加载，panic 处理器就会自动暂停串行监控并使用必要的参数运行 GDB。GDB 退出后，通过 RTS 串口线复位开发板。如果未连接 RTS 串口线，请按复位键，手动复位开发板。

IDF 监控器在后台运行如下命令::

  xtensa-{IDF_TARGET_TOOLCHAIN_NAME}-elf-gdb -ex "set serial baud BAUD" -ex "target remote PORT" -ex interrupt build/PROJECT.elf :idf_target:`Hello NAME chip`


输出筛选
~~~~~~~~~~~~~~~~

可以调用 ``idf.py monitor --print-filter="xyz"`` 启动 IDF 监视器，其中，``--print-filter`` 是输出筛选的参数。参数默认值为空字符串，可打印任何内容。

若需对打印内容设置限制，可指定 ``<tag>:<log_level>`` 等选项，其中 ``<tag>`` 是标签字符串，``<log_level>`` 是 ``{N, E, W, I, D, V, *}`` 集合中的一个字母，指的是 :doc:`日志 <../../api-reference/system/log>` 级别。

例如，``PRINT_FILTER="tag1:W"`` 只匹配并打印 ``ESP_LOGW("tag1", ...)`` 所写的输出，或者写在较低日志详细度级别的输出，即 ``ESP_LOGE("tag1", ...)``。请勿指定 ``<log_level>`` 或使用详细级别默认值 ``*``。

.. note::
   编译时，可以使用主日志在 :doc:`日志库 <../../api-reference/system/log>` 中禁用不需要的输出。也可以使用 IDF 监视器筛选输出来调整筛选设置，且无需重新编译应用程序。

应用程序标签不能包含空格、星号 ``*``、冒号 ``:``，以便兼容输出筛选功能。

如果应用程序输出的最后一行后面没有回车，可能会影响输出筛选功能，即，监视器开始打印该行，但后来发现该行不应该被写入。这是一个已知问题，可以通过添加回车来避免此问题（特别是在没有输出紧跟其后的情况下）。

筛选规则示例
~~~~~~~~~~~~~~~~

- ``*`` 可用于匹配任何类型标签。但 ``PRINT_FILTER="*:I tag1:E"`` 打印关于 ``tag1`` 的输出时会报错，这是因为 ``tag1`` 规则比 ``*`` 规则的优先级高。
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

``PRINT_FILTER="wifi esp_image:E light_driver:I"`` 筛选选项捕获的输出如下所示::

    E (31) esp_image: image at 0x30000 has invalid magic byte
    I (328) wifi: wifi driver task: 3ffdbf84, prio:23, stack:4096, core=0

``PRINT_FILTER="light_driver:D esp_image:N boot:N cpu_start:N vfs:N wifi:N *:V"`` 选项的输出如下::

    load:0x40078000,len:13564
    entry 0x40078d4c
    I (569) heap_init: Initializing. RAM available for dynamic allocation:
    D (309) light_driver: [light_init, 74]:status: 1, mode: 2


IDF 监视器已知问题
=============================

Windows 环境下已知问题
~~~~~~~~~~~~~~~~~~~~~~~~~~

- 若在 Windows 环境下，出现 "winpty: command not found" 错误，请运行 ``pacman -S winpty`` 进行修复。
- 由于 Windows 控制台限制，有些箭头键及其他一些特殊键无法在 GDB 中使用。
- 偶然情况下，``idf.py`` 或 ``make`` 退出时，可能会在 IDF 监视器恢复之前暂停 30 秒。
- GDB 运行时，可能会暂停一段时间，然后才开始与 GDBStub 进行通信。


.. _addr2line: https://sourceware.org/binutils/docs/binutils/addr2line.html
.. _gdb: https://sourceware.org/gdb/download/onlinedocs/
.. _pySerial: https://github.com/pyserial/pyserial
.. _miniterm: https://pyserial.readthedocs.org/en/latest/tools.html#module-serial.tools.miniterm