核心转储
=========

:link_to_translation:`en:[English]`

概述
--------

核心转储是软件发生致命错误时，由紧急处理程序自动保存的一组软件状态信息。核心转储有助于对故障进行事后分析，了解软件状态。ESP-IDF 支持生成核心转储。

核心转储包含了系统中所有任务在发生故障时的快照，每个快照都包括任务的控制块 (TCB) 和栈信息。通过分析任务快照，可以确定是哪个任务、在哪个指令（代码行），以及该任务的哪个调用栈导致了系统崩溃。如果将某些变量赋予特殊的核心转储属性，还可以转储这些变量的内容。

核心转储数据会按照特定格式保存在核心转储文件中，详情请参阅 :doc:`core_dump_internals`。然而，ESP-IDF 的 ``idf.py`` 命令提供了专门的子命令，用于解码和分析核心转储文件。


配置
--------------

目标
^^^^^^^^^^^

选项 :ref:`CONFIG_ESP_COREDUMP_TO_FLASH_OR_UART` 可以启用或禁用核心转储，并在启用时选择核心转储的目标。发生崩溃时，生成的核心转储文件可以保存到 flash 中，也可以通过 UART 输出到连接的主机上。


格式和大小
^^^^^^^^^^^^^

选项 :ref:`CONFIG_ESP_COREDUMP_DATA_FORMAT` 控制核心转储文件格式，即 ELF 格式或二进制格式。

ELF 格式具备扩展特性，支持在发生崩溃时保存更多关于错误任务和崩溃软件的信息，但使用 ELF 格式会使核心转储文件变大。建议在新的软件设计中使用此格式，该格式足够灵活，可以在未来的修订版本中进行扩展，保存更多信息。

出于兼容性考虑，核心转储文件保留二进制格式。二进制格式的核心转储文件更小，性能更优。

选项 :ref:`CONFIG_ESP_COREDUMP_MAX_TASKS_NUM` 配置核心转储保存的任务快照数量。

通过 ``Components`` > ``Core dump`` > ``Core dump data integrity check`` 选项可进行核心转储数据完整性检查。

.. only:: esp32

    数据完整性检查
    ^^^^^^^^^^^^^^^^^^^^

    核心转储文件包含一个校验和，用于验证核心转储文件在保存时是否完整（即未损坏）。选项 :ref:`CONFIG_ESP_COREDUMP_CHECKSUM` 控制校验和的类型，即 CRC32 或 SHA256（仅在 ELF 格式中支持）。

    CRC32 选项能够提供更好的计算性能，在存储时占用内存较少。

    SHA256 哈希算法检测到损坏的概率高于具有多位错误的 CRC32。


保留栈大小
^^^^^^^^^^^^^^^^^^^

核心转储例程需要解析并保存所有其他任务的栈，因此会从单独的栈中运行。选项 :ref:`CONFIG_ESP_COREDUMP_STACK_SIZE` 控制核心转储栈大小，以字节数表示。

将此选项设置为 0 字节将使核心转储例程从 ISR 栈中运行，从而节省内存。将选项设置为大于零的值将创建一个独立的栈。

.. note::

   如果使用了独立的栈，建议栈大小应大于 1300 字节，确保核心转储例程本身不会导致栈溢出。

.. only:: not esp32c5

    核心转储内存区域
    ^^^^^^^^^^^^^^^^

    核心转储默认保存 CPU 寄存器、任务数据和崩溃原因。选择 :ref:`CONFIG_ESP_COREDUMP_CAPTURE_DRAM` 选项后，``.bss`` 段和 ``.data`` 段以及 ``heap`` 数据也将保存到转储中。

    推荐将上面提到的几个数据段都保存到核心转储中，以方便调试。但这会导致核心转储文件变大，具体所需的额外存储空间取决于应用程序使用的 DRAM 大小。

    .. only:: SOC_SPIRAM_SUPPORTED

        .. note::

            除了崩溃任务的 TCB 和栈外，位于外部 RAM 中的数据不会保存到核心转储文件中，包括使用 ``EXT_RAM_BSS_ATTR`` 或 ``EXT_RAM_NOINIT_ATTR`` 属性定义的变量，以及存储在 ``extram_bss`` 段中的任何数据。

    .. note::

        该功能仅在使用 ELF 文件格式时可用。

将核心转储保存到 flash
-----------------------

将核心转储文件保存至 flash 时，这些文件会保存到 flash 上的特殊分区。指定核心转储分区可以在 flash 芯片上预留空间来存储核心转储文件。

使用 ESP-IDF 提供的默认分区表时，核心转储分区会自动声明。但使用自定义分区表时，请按如下示例进行核心转储分区声明：

.. code-block:: none

   # 名称，   类型，子类型，   偏移量，   大小
   # 注意：如果增加了引导加载程序大小，请及时更新偏移量，避免产生重叠
   nvs,      data, nvs,     0x9000,  0x6000
   phy_init, data, phy,     0xf000,  0x1000
   factory,  app,  factory, 0x10000, 1M
   coredump, data, coredump,,        64K

.. important::

    如果设备启用了 :doc:`../security/flash-encryption`，请在核心转储分区中添加 ``encrypted`` 标志。请注意，使用 ``idf.py coredump-info`` 或 ``idf.py coredump-debug`` 命令无法从加密分区读取核心转储。
    建议使用 ``idf.py coredump-info -c <path-to-core-dump>`` 命令从 ESP 设备侧读取核心转储，ESP 设备会自动解密分区并发送到相应位置用于分析。

    .. code-block:: none

        coredump, data, coredump,,       64K, encrypted

分区命名没有特殊要求，可以根据应用程序的需要选择。但分区类型应为 ``data``，子类型应为 ``coredump``。此外，在选择分区大小时需注意，核心转储的数据结构会产生 20 字节的固定开销和 12 字节的单任务开销，此开销不包括每个任务的 TCB 和栈的大小。因此，分区大小应至少为 ``20 + 最大任务数 x（12 + TCB 大小 + 最大任务栈大小）`` 字节。

用于分析 flash 中核心转储的常用命令，可参考以下示例：

.. code-block:: bash

    idf.py coredump-info

或

.. code-block:: bash

    idf.py coredump-debug


.. note::

    ``idf.py coredump-info`` 命令和 ``idf.py coredump-debug`` 命令对 `esp-coredump` 工具进行了封装，可以在 ESP-IDF 环境中轻松使用。更多信息，请参考 :ref:`core_dump_commands`。


将核心转储保存到 UART
-----------------------

当核心转储文件输出到 UART 时，输出文件会以 Base64 编码方式呈现。通过 :ref:`CONFIG_ESP_COREDUMP_DECODE` 选项，可以选择 ESP-IDF 监视器对输出文件自动解码，或保持编码状态等待手动解码。


自动解码
^^^^^^^^^^^^^^^^^^

如果设置 :ref:`CONFIG_ESP_COREDUMP_DECODE`，使其自动解码 UART 核心转储文件，ESP-IDF 监视器会自动解码数据，将所有函数地址转换为源代码行，并在监视器中显示相应信息。ESP-IDF 监视器会输出类似以下内容：

此外，选项 :ref:`CONFIG_ESP_COREDUMP_UART_DELAY` 支持在将核心转储文件输出到 UART 前添加延迟。

.. code-block:: none

    ===============================================================
    ==================== ESP32 CORE DUMP START ====================

    Crashed task handle: 0x3ffafba0, name: 'main', GDB name: 'process 1073413024'
    Crashed task is not in the interrupt context
    Panic reason: abort() was called at PC 0x400d66b9 on core 0

    ================== CURRENT THREAD REGISTERS ===================
    exccause       0x1d (StoreProhibitedCause)
    excvaddr       0x0
    epc1           0x40084013
    epc2           0x0
    ...
    ==================== CURRENT THREAD STACK =====================
    #0  0x4008110d in panic_abort (details=0x3ffb4f0b "abort() was called at PC 0x400d66b9 on core 0") at /builds/espressif/esp-idf/components/esp_system/panic.c:472
    #1  0x4008510c in esp_system_abort (details=0x3ffb4f0b "abort() was called at PC 0x400d66b9 on core 0") at /builds/espressif/esp-idf/components/esp_system/port/esp_system_chip.c:93
    ...
    ======================== THREADS INFO =========================
      Id   Target Id          Frame
    * 1    process 1073413024 0x4008110d in panic_abort (details=0x3ffb4f0b "abort() was called at PC 0x400d66b9 on core 0") at /builds/espressif/esp-idf/components/esp_system/panic.c:472
      2    process 1073413368 vPortTaskWrapper (pxCode=0x0, pvParameters=0x0) at /builds/espressif/esp-idf/components/freertos/FreeRTOS-Kernel/portable/xtensa/port.c:133
    ...
           TCB             NAME PRIO C/B  STACK USED/FREE
    ---------- ---------------- -------- ----------------
    0x3ffafba0             main      1/1         368/3724
    0x3ffafcf8            IDLE0      0/0         288/1240
    0x3ffafe50            IDLE1      0/0         416/1108
    ...
    ==================== THREAD 1 (TCB: 0x3ffafba0, name: 'main') =====================
    #0  0x4008110d in panic_abort (details=0x3ffb4f0b "abort() was called at PC 0x400d66b9 on core 0") at /builds/espressif/esp-idf/components/esp_system/panic.c:472
    #1  0x4008510c in esp_system_abort (details=0x3ffb4f0b "abort() was called at PC 0x400d66b9 on core 0") at /builds/espressif/esp-idf/components/esp_system/port/esp_system_chip.c:93
    ...
    ==================== THREAD 2 (TCB: 0x3ffafcf8, name: 'IDLE0') =====================
    #0  vPortTaskWrapper (pxCode=0x0, pvParameters=0x0) at /builds/espressif/esp-idf/components/freertos/FreeRTOS-Kernel/portable/xtensa/port.c:133
    #1  0x40000000 in ?? ()
    ...
    ======================= ALL MEMORY REGIONS ========================
    Name   Address   Size   Attrs
    ...
    .iram0.vectors 0x40080000 0x403 R XA
    .iram0.text 0x40080404 0xb8ab R XA
    .dram0.data 0x3ffb0000 0x2114 RW A
    ...
    ===================== ESP32 CORE DUMP END =====================
    ===============================================================


手动解码
^^^^^^^^^^^^^^^

如果设置 :ref:`CONFIG_ESP_COREDUMP_DECODE` 为不解码，则在以下 UART 输出的页眉和页脚之间，将输出核心转储的原始 Base64 编码正文：

.. code-block:: none

    ================= CORE DUMP START =================
    <将 Base64 编码的核心转储内容解码，并将其保存到磁盘文件中>
    ================= CORE DUMP END ===================

建议将核心转储文本主体手动保存到文件，``CORE DUMP START`` 和 ``CORE DUMP END`` 行不应包含在核心转储文本文件中。随后，可以使用以下命令解码保存的文本：

.. code-block:: bash

    idf.py coredump-info -c </path/to/saved/base64/text>

或

.. code-block:: bash

    idf.py coredump-debug -c </path/to/saved/base64/text>


.. _core_dump_commands:

核心转储命令
------------------

ESP-IDF 提供了一些特殊命令，有助于检索和分析核心转储：

* ``idf.py coredump-info`` - 打印崩溃任务的寄存器、调用栈、系统可用任务列表、内存区域以及核心转储中存储的内存内容（包括 TCB 和栈）。
* ``idf.py coredump-debug`` - 创建核心转储 ELF 文件，并使用该文件运行 GDB 调试会话。你可以手动检查内存、变量和任务状态。请注意，由于并未将所有内存保存在核心转储中，因此只有在栈上分配的变量的值才有意义。

高阶用户如果需要传递额外参数或使用自定义 ELF 文件，可直接使用 `esp-coredump <https://github.com/espressif/esp-coredump>`_ 工具。如果在 ESP-IDF 环境中使用该工具，可运行如下命令查询更多信息：

.. code-block:: bash

    esp-coredump --help


回溯中的 ROM 函数
---------------------------

程序崩溃时，某些任务和/或崩溃任务本身的调用栈中可能包含一或多个 ROM 函数。由于 ROM 不是程序 ELF 的一部分，而 GDB 需要分析函数序言来解码回溯，因此 GDB 无法解析这些调用栈。因此，在遇到第一个 ROM 函数时，调用栈解析将中断并报错。

为解决这一问题，ESP-IDF 监视器会根据目标芯片及其修订版本自动加载乐鑫提供的 `ROM ELF <https://github.com/espressif/esp-rom-elfs/releases>`_。有关 ROM ELF 的详细信息，请参阅 `esp-rom-elfs <https://github.com/espressif/esp-rom-elfs/blob/master/README.md>`_。


按需转储变量
---------------------------

通过读取变量的最后一个值，可以了解崩溃发生的根本原因。核心转储支持通过为已声明的变量添加特殊标记，在 GDB 上检索变量数据。


支持的标记和 RAM 区域
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. list::

   * ``COREDUMP_DRAM_ATTR`` 将变量放置在 DRAM 区域，该区域包含在转储中。
   :SOC_RTC_FAST_MEM_SUPPORTED or SOC_RTC_SLOW_MEM_SUPPORTED: * ``COREDUMP_RTC_ATTR`` 将变量放置在 RTC 区域，该区域包含在转储中。
   :SOC_RTC_FAST_MEM_SUPPORTED: * ``COREDUMP_RTC_FAST_ATTR`` 将变量放置在 RTC_FAST 区域，该区域包含在转储中。


示例
^^^^^^^

1. 在 :ref:`project-configuration-menu` 中启用 :ref:`COREDUMP TO FLASH <CONFIG_ESP_COREDUMP_TO_FLASH_OR_UART>`，随后保存并退出。

2. 在项目中，创建如下全局变量，放置在 DRAM 区域：

.. code-block:: bash

   // uint8_t global_var;
   COREDUMP_DRAM_ATTR uint8_t global_var;

3. 在主应用程序中，将该变量设置为任意值，并以 ``assert(0)`` 引发崩溃。

.. code-block:: bash

   global_var = 25;
   assert(0);

4. 在目标设备上构建、烧写并运行应用程序，等待转储信息。

5. 运行以下命令，在 GDB 中开始核心转储，其中 ``PORT`` 是设备的 USB 端口：

.. code-block:: bash

   idf.py coredump-debug

6. 在 GDB shell 中，输入 ``p global_var`` 获取变量内容：

.. code-block:: bash

   (gdb) p global_var
   $1 = 25 '\031'


运行 ``idf.py coredump-info`` 和 ``idf.py coredump-debug``
--------------------------------------------------------------

要获取更多有关使用方法的详情，请运行 ``idf.py coredump-info --help`` 和 ``idf.py coredump-debug --help`` 命令。


相关文档
^^^^^^^^^^^^^^^^^

.. toctree::
    :maxdepth: 1

    core_dump_internals
