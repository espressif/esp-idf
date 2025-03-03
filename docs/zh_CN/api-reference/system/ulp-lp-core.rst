ULP LP 内核协处理器编程
=======================

:link_to_translation:`en:[English]`

ULP LP 内核 (Low-power core) 协处理器是 {IDF_TARGET_NAME} 中 ULP 的一个变型。它具有超低功耗，同时还能在主 CPU 处于低功耗模式时保持运行。因此，LP 内核协处理器能够在主 CPU 处于睡眠模式时处理 GPIO 或传感器读取等任务，从而显著降低整个系统的整体功耗。

ULP LP 内核协处理器具有以下功能：

* RV32I 处理器（32 位 RISC-V ISA），支持乘法/除法 (M)、原子 (A) 和压缩 (C) 扩展。
* 中断控制器。
* 包含一个调试模块，支持通过 JTAG 进行外部调试。
* 当整个系统处于 active 模式时，可以访问所有的高功耗 (HP) SRAM 和外设。
* 当 HP 系统处于睡眠模式时，可以访问低功耗 (LP) SRAM 和外设。

编译 ULP LP 内核代码
--------------------

ULP LP 内核代码会与 ESP-IDF 项目共同编译，生成一个单独的二进制文件，并自动嵌入到主项目的二进制文件中。编译可通过以下两种方式实现：

使用 ``ulp_embed_binary``
~~~~~~~~~~~~~~~~~~~~~~~~~

1. 将用 C 语言或汇编语言编写的 ULP LP 内核代码（带有 ``.S`` 扩展名）放在组件目录下的专用目录中，例如 ``ulp/``。

2. 在 ``CMakeLists.txt`` 文件中注册组件后，调用 ``ulp_embed_binary`` 函数。例如：

.. code-block:: cmake

    idf_component_register()

    set(ulp_app_name ulp_${COMPONENT_NAME})
    set(ulp_sources "ulp/ulp_c_source_file.c" "ulp/ulp_assembly_source_file.S")
    set(ulp_exp_dep_srcs "ulp_c_source_file.c")

    ulp_embed_binary(${ulp_app_name} "${ulp_sources}" "${ulp_exp_dep_srcs}")

``ulp_embed_binary`` 的第一个参数指定生成的 ULP 二进制文件名。该文件名也用于其他生成的文件，如 ELF 文件、映射文件、头文件和链接器导出文件。第二个参数指定 ULP 源文件。第三个参数指定组件源文件列表，其中包括生成的头文件。此列表用以正确构建依赖，并确保在编译这些文件前创建要生成的头文件。有关 ULP 应用程序生成头文件的概念，请参阅本文档后续章节。

在这个生成的头文件中，ULP 代码中的变量默认以 ``ulp_`` 作为前缀。

如果需要嵌入多个 ULP 程序，可以添加自定义前缀，以避免变量名冲突，如下所示：

.. code-block:: cmake

    idf_component_register()

    set(ulp_app_name ulp_${COMPONENT_NAME})
    set(ulp_sources "ulp/ulp_c_source_file.c" "ulp/ulp_assembly_source_file.S")
    set(ulp_exp_dep_srcs "ulp_c_source_file.c")

    ulp_embed_binary(${ulp_app_name} "${ulp_sources}" "${ulp_exp_dep_srcs}" PREFIX "ULP::")

最后的 PREFIX 参数可以是 C 语言风格命名的前缀（如 ``ulp2_``）或 C++ 风格命名的前缀（如 ``ULP::``）。

使用自定义的 CMake 项目
~~~~~~~~~~~~~~~~~~~~~~~

也可以为 LP 内核创建自定义的 CMake 项目，从而更好地控制构建过程，并实现常规 CMake 项目的操作，例如设置编译选项、链接外部库等。

请在组件的 ``CMakeLists.txt`` 文件中将 ULP 项目添加为外部项目：

.. code-block:: cmake

    ulp_add_project("ULP_APP_NAME" "${CMAKE_SOURCE_DIR}/PATH_TO_DIR_WITH_ULP_PROJECT_FILE/")

请创建一个文件夹，并在文件夹中添加 ULP 项目文件及 ``CMakeLists.txt`` 文件，该文件夹的位置应与 ``ulp_add_project`` 函数中指定的路径一致。``CMakeLists.txt`` 文件应如下所示：

.. code-block:: cmake

    cmake_minimum_required(VERSION 3.16)

    # 项目/目标名称由主项目传递，允许 IDF 依赖此目标
    # 将二进制文件嵌入到主应用程序中
    project(${ULP_APP_NAME})
    add_executable(${ULP_APP_NAME} main.c)

    # 导入 ULP 项目辅助函数
    include(IDFULPProject)

    # 应用默认的编译选项
    ulp_apply_default_options(${ULP_APP_NAME})

    # 应用 IDF ULP 组件提供的默认源文件
    ulp_apply_default_sources(${ULP_APP_NAME})

    # 添加构建二进制文件的目标，并添加链接脚本，用于将 ULP 共享变量导出到主应用程序
    ulp_add_build_binary_targets(${ULP_APP_NAME})

    # 以下内容是可选的，可以用于自定义构建过程

    # 创建自定义库
    set(lib_path "${CMAKE_CURRENT_LIST_DIR}/lib")
    add_library(custom_lib STATIC "${lib_path}/lib_src.c")
    target_include_directories(custom_lib PUBLIC "${lib_path}/")

    # 链接到库
    target_link_libraries(${ULP_APP_NAME} PRIVATE custom_lib)

    # 设置自定义编译标志
    target_compile_options(${ULP_APP_NAME} PRIVATE -msave-restore)

构建项目
~~~~~~~~

若想编译和构建项目，请执行以下操作：

1. 在 menuconfig 中启用 :ref:`CONFIG_ULP_COPROC_ENABLED` 和 :ref:`CONFIG_ULP_COPROC_TYPE` 选项，并将 :ref:`CONFIG_ULP_COPROC_TYPE` 设置为 ``CONFIG_ULP_COPROC_TYPE_LP_CORE``。:ref:`CONFIG_ULP_COPROC_RESERVE_MEM` 选项为 ULP 保留 RTC 内存，因此必须设置为一个足够大的值，以存储 ULP LP 内核代码和数据。如果应用程序组件包含多个 ULP 程序，那么 RTC 内存的大小必须足够容纳其中最大的程序。

2. 按照常规步骤构建应用程序（例如 ``idf.py app``）。

在构建过程中，采取以下步骤来构建 ULP 程序：

    1. **通过 C 编译器和汇编器运行每个源文件。** 此步骤会在组件构建目录中生成目标文件 ``.obj.c`` 或 ``.obj.S``，具体取决于处理的源文件。

    2. **通过 C 预处理器运行链接器脚本模板。** 模板位于 ``components/ulp/ld`` 目录中。

    3. **将对象文件链接到一个 ELF 输出文件中，** 即 ``ulp_app_name.elf``。在此阶段生成的映射文件 ``ulp_app_name.map`` 可用于调试。

    4. **将 ELF 文件的内容转储到一个二进制文件中，** 即 ``ulp_app_name.bin``。此二进制文件接下来可以嵌入到应用程序中。

    5. 使用 ``riscv32-esp-elf-nm`` 在 ELF 文件中 **生成全局符号列表，** 即 ``ulp_app_name.sym``。

    6. **创建一个 LD 导出脚本和一个头文件，** 即 ``ulp_app_name.ld`` 和 ``ulp_app_name.h``，并在其中包含 ``ulp_app_name.sym`` 中的符号。此步骤可以通过 ``esp32ulp_mapgen.py`` 实现。

    7. **将生成的二进制文件添加到要嵌入到应用程序中的二进制文件列表。**


.. _ulp-lp-core-access-variables:

访问 ULP LP 内核程序变量
------------------------

在主程序中可以使用在 ULP LP 内核程序中定义的全局符号。

例如，ULP LP 内核程序定义了一个变量 ``measurement_count``，用来表示程序从深度睡眠中唤醒芯片前所需的 GPIO 测量次数。

.. code-block:: c

    volatile int measurement_count;

    int some_function()
    {
        //读取测量次数以便后续使用。
        int temp = measurement_count;

        ...do something.
    }

主程序可以访问 ULP LP 内核程序全局变量，这是因为构建系统生成了 ``${ULP_APP_NAME}.h`` 和 ``${ULP_APP_NAME}.ld`` 文件，文件中定义了 ULP LP 内核程序中现有的的全局符号。在 ULP LP 内核程序中定义的每个全局符号都包含在这两个文件中，并具有前缀 ``ulp_``。

头文件中包含符号的声明：

.. code-block:: c

    extern uint32_t ulp_measurement_count;

注意，所有的符号（变量、函数）都被声明为 ``uint32_t`` 类型。数组被声明为 ``uint32_t [SIZE]`` 类型。函数需要先获取符号地址，再转换为适当的类型。

要从主程序访问 ULP LP 内核程序变量，应使用 ``include`` 语句将生成的头文件包含在主程序中，这样就可以像访问常规变量一样访问 ULP LP 内核程序变量。

.. code-block:: c

    #include "ulp_app_name.h"

    void init_ulp_vars() {
        ulp_measurement_count = 64;
    }

.. note::

    - LP 内核程序全局变量存储在二进制文件的 ``.bss`` 或者 ``.data`` 部分。这些部分在加载和执行 LP 内核二进制文件时被初始化。在首次运行 LP 内核之前，从 HP-Core 主程序访问这些变量可能会导致未定义行为。

    - 默认以 ``ulp_`` 作为前缀。你可以在使用 ``ulp_embed_binary`` 时指定前缀，以避免多个 ULP 程序之间的命名冲突。


启动 ULP LP 内核程序
--------------------

要运行 ULP LP 内核程序，主应用程序需要先使用 :cpp:func:`ulp_lp_core_load_binary` 函数将 ULP 程序加载到 RTC 内存中，然后使用 :cpp:func:`ulp_lp_core_run` 函数进行启动。

每个 ULP LP 内核程序以二进制 blob 的形式嵌入到 ESP-IDF 应用程序中。应用程序可以按照如下方式引用和加载该 blob（假设 ULP_APP_NAME 被定义为 ``ulp_app_name``）：

.. code-block:: c

    extern const uint8_t bin_start[] asm("_binary_ulp_app_name_bin_start");
    extern const uint8_t bin_end[]   asm("_binary_ulp_app_name_bin_end");

    void start_ulp_program() {
        ESP_ERROR_CHECK( ulp_lp_core_load_binary( bin_start,
            (bin_end - bin_start)) );
    }

将程序加载到 LP 内存后，就可以调用 :cpp:func:`ulp_lp_core_run` 配置和启动应用程序：

.. code-block:: c

    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER, // LP 内核会定期被 LP 定时器唤醒
        .lp_timer_sleep_duration_us = 10000,
    };

    ESP_ERROR_CHECK( ulp_lp_core_run(&cfg) );

ULP LP 内核程序流程
-------------------

ULP LP 内核协处理器如何启动取决于 :cpp:type:`ulp_lp_core_cfg_t` 中选择的唤醒源。最常见的用例是 ULP 定期唤醒，在进行一些测量后唤醒主 CPU，或者再次进入睡眠状态。

ULP 有以下唤醒源：
    * :c:macro:`ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU` - LP 内核可以被 HP CPU 唤醒。
    * :c:macro:`ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER` - LP 内核可以被 LP 定时器唤醒。
    * :c:macro:`ULP_LP_CORE_WAKEUP_SOURCE_ETM` - LP 内核可以被 ETM 事件唤醒。（暂不支持）
    * :c:macro:`ULP_LP_CORE_WAKEUP_SOURCE_LP_IO` - 当 LP IO 电平变化时，LP 内核会被唤醒。（暂不支持）
    * :c:macro:`ULP_LP_CORE_WAKEUP_SOURCE_LP_UART` - 当 LP UART 在不同模式下接收到唤醒数据时，LP 内核会被唤醒。

ULP 被唤醒时会经历以下步骤：

.. list::

    :CONFIG_ESP_ROM_HAS_LP_ROM: #. 除非已指定 :cpp:member:`ulp_lp_core_cfg_t::skip_lp_rom_boot`，否则运行 ROM 启动代码并跳转至 LP RAM 中的入口地址。ROM 启动代码将初始化 LP UART 并打印启动信息。
    #. 初始化系统功能，如中断
    #. 调用用户代码 ``main()``
    #. 从 ``main()`` 返回
    #. 如果指定了 ``lp_timer_sleep_duration_us``，则配置下一个唤醒闹钟
    #. 调用 :cpp:func:`ulp_lp_core_halt`


ULP LP 内核支持的外设
---------------------

为了增强 ULP LP 内核协处理器的功能，它可以访问在低功耗电源域运行的外设。ULP LP 内核协处理器可以在主 CPU 处于睡眠模式时与这些外设进行交互，并在达到唤醒条件时唤醒主 CPU。以下为支持的外设：

.. list::

    * LP IO
    * LP I2C
    * LP UART
    :SOC_LP_SPI_SUPPORTED: * LP SPI

.. only:: CONFIG_ESP_ROM_HAS_LP_ROM

    ULP LP 内核 ROM
    ---------------

    ULP LP 内核 ROM 是位于 LP-ROM 中的一小段预编译代码，用户无法修改。与主 CPU 运行的引导加载程序 ROM 代码类似，ULP LP 内核 ROM 也在 ULP LP 内核协处理器启动时执行。该 ROM 代码会初始化 ULP LP 内核协处理器，随后跳转到用户程序。如果已初始化 LP UART，该 ROM 代码还会打印启动信息。

    如果已将 :cpp:member:`ulp_lp_core_cfg_t::skip_lp_rom_boot` 设置为真，则不会执行 ULP LP 内核 ROM 代码。如需尽快唤醒 ULP，同时避免初始化和信息打印产生额外开销，则可使用这一功能。

    除上述启动代码，ULP LP 内核 ROM 代码还提供以下功能和接口：

    * :component_file:`ROM.ld 接口 <esp_rom/{IDF_TARGET_PATH_NAME}/ld/{IDF_TARGET_PATH_NAME}lp.rom.ld>`
    * :component_file:`newlib.ld 接口 <esp_rom/{IDF_TARGET_PATH_NAME}/ld/{IDF_TARGET_PATH_NAME}lp.rom.newlib.ld>`

    在任何情况下，这些函数都存在于 LP-ROM 中，因此在程序中使用这些函数可以减少 ULP 应用程序的 RAM 占用。


ULP LP 内核中断
---------------

配置 LP 内核协处理器，可以处理各种类型的中断，例如 LP IO 低/高电平中断或是 LP 定时器中断。只需重写 IDF 提供的任何一个弱处理函数，就可以注册一个中断处理程序。所有处理程序可见 :component_file:`ulp_lp_core_interrupts.h <ulp/lp_core/lp_core/include/ulp_lp_core_interrupts.h>`。有关特定目标可使用的中断的详细信息，请参阅 **{IDF_TARGET_NAME} 技术参考手册** [`PDF <{IDF_TARGET_TRM_CN_URL}#ulp>`__]。

例如，要重写 LP IO 中断的处理程序，可以在 ULP LP 内核代码中定义以下函数：

.. code-block:: c

    void LP_CORE_ISR_ATTR ulp_lp_core_lp_io_intr_handler(void)
    {
        // 处理中断，清除中断源
    }

:c:macro:`LP_CORE_ISR_ATTR` 宏用于定义中断处理函数，可确保调用中断处理程序时妥善保存并恢复寄存器。

除了为需要处理的中断源配置相关的中断寄存器外，还要调用 :cpp:func:`ulp_lp_core_intr_enable` 函数，在 LP 内核中断控制器中使能全局中断。

ULP LP 内核时钟配置
-------------------

{IDF_TARGET_XTAL_FREQ:default="未更新", esp32c5="48 MHz", esp32p4="40 MHz"}

ULP LP 内核的时钟源来自系统时钟 ``LP_FAST_CLK``，详情请参见 `技术参考手册 <{IDF_TARGET_TRM_CN_URL}>`__ > ``复位和时钟``。

.. only:: SOC_CLK_LP_FAST_SUPPORT_XTAL

    在 {IDF_TARGET_NAME} 上，``LP_FAST_CLK`` 支持使用外部 {IDF_TARGET_XTAL_FREQ} 晶振 (XTAL) 作为其时钟源。默认时钟源 ``RTC_FAST_CLOCK`` 的运行频率约为 20 MHz，使用外部晶振时钟后，ULP LP 内核将以更高的频率运行。缺点在于，``LP_FAST_CLK`` 在休眠期间通常会断电以减少功耗，而选择 XTAL 作为时钟源后，休眠期间时钟仍将保持通电，造成功耗增加。因此，如果仅希望在 HP 内核活动时将 LP 内核用作协处理器，则可以使用 XTAL 以提高 LP 内核的性能和频率稳定性。

    要启用此功能，请将 :ref:`CONFIG_RTC_FAST_CLK_SRC` 设置为 ``CONFIG_RTC_FAST_CLK_SRC_XTAL``。


调试 ULP LP 内核应用程序
------------------------

在编程 LP 内核时，有时很难弄清楚程序未按预期运行的原因。请参考以下策略，调试 LP 内核程序：

* 使用 LP UART 打印：LP 内核可以访问 LP UART 外设，在主 CPU 处于睡眠状态时独立打印信息。有关使用此驱动程序的示例，请参阅 :example:`system/ulp/lp_core/lp_uart/lp_uart_print`。

* 通过 :ref:`CONFIG_ULP_HP_UART_CONSOLE_PRINT`，将 :cpp:func:`lp_core_printf` 路由到 HP-Core 控制台 UART，可以轻松地将 LP 内核信息打印到已经连接的 HP-Core 控制台 UART。此方法的缺点是需要主 CPU 处于唤醒状态，并且由于 LP 内核与 HP 内未同步，输出可能会交错。

* 通过共享变量共享程序状态：如 :ref:`ulp-lp-core-access-variables` 所述，主 CPU 和 ULP 内核都可以轻松访问 RTC 内存中的全局变量。若想了解 ULP 内核的运行状态，可以将状态信息从 ULP 写入变量中，并通过主 CPU 读取信息。这种方法的缺点在于它需要主 CPU 一直处于唤醒状态，而这通常很难实现。另外，若主 CPU 一直处于唤醒状态，可能会掩盖某些问题，因为部分问题只会在特定电源域断电时发生。

* 紧急处理程序：当检测到异常时，LP 内核的紧急处理程序会把 LP 内核寄存器的状态通过 LP UART 发送出去。将 :ref:`CONFIG_ULP_PANIC_OUTPUT_ENABLE` 选项设置为 ``y``，可以启用紧急处理程序。禁用此选项将减少 LP 内核应用程序的 LP-RAM 使用量。若想从紧急转储中解析栈回溯，可以使用 ``idf.py monitor``。

.. warning::

    如果在单个项目中使用多个 ULP 应用程序，栈回溯解码可能无法正常工作。此时建议直接使用 esp-idf-monitor_ 工具，并指定正确的 ULP ELF 文件：

    .. code-block:: bash

        python -m esp_idf_monitor --toolchain-prefix riscv32-esp-elf- --target {IDF_TARGET_PATH_NAME} --decode-panic backtrace PATH_TO_ULP_ELF_FILE


调试 ULP LP 内核应用程序：使用 GDB 和 OpenOCD
----------------------------------------------

与调试 HP 内核类似，也可以用 GDB 和 OpenOCD 来调试 LP 内核上的代码，但要注意其特殊之处和限制条件。

调试会话
~~~~~~~~

使用支持 LP 内核调试的特殊配置文件来运行 OpenOCD，然后用特殊的 ``gdbinit`` 文件运行 GDB。

.. code-block:: bash

    openocd -f board/{IDF_TARGET_PATH_NAME}-lpcore-builtin.cfg
    riscv32-esp-elf-gdb -x gdbinit <path to main program ELF>

以下是带有内联注释的 ``gdbinit`` 文件内容，详细信息请参考下一章节。

.. code-block:: bash

    # 连接到目标
    target extended-remote :3333
    # 重置芯片
    mon reset halt
    maintenance flush register-cache
    # 添加 ULP 程序的符号和调试信息
    add-symbol <ULP 程序 ELF 文件路径>
    # 设置临时硬件断点
    # 如果需要的断点数量超过硬件支持的数量
    thb main
    commands
    # 在这里设置断点
    # 此时 ULP 程序已加载到 RAM 中
    # 若无可用的硬件断点插槽，GDB 将设置软件断点
    b func1
    b func2
    b func3
    # 恢复执行
    c
    end
    # 重置后启动主程序
    c

LP 内核调试特性
~~~~~~~~~~~~~~~

.. list::

    #. 为了方便调试，请在 ULP 应用的 ``CMakeLists.txt`` 文件中添加 ``-O0`` 编译选项。具体操作步骤请参见 :example:`system/ulp/lp_core/debugging/`。
    :not esp32p4: #. LP 内核支持的硬件异常类型有限，例如，写入地址 `0x0` 不会像在 HP 内核上一样造成系统崩溃。启用 LP 内核应用程序的未定义行为检测器 (`ubsan`) 可以捕捉一些错误，从而在一定程度上弥补这一限制。但请注意，这将显著增加代码量，可能会导致应用程序超出 RTC RAM 的容量限制。要启用 `ubsan`，请在 ``CMakeLists.txt`` 文件中添加 ``-fsanitize=undefined -fno-sanitize=shift-base`` 编译选项。具体操作步骤请参见 :example:`system/ulp/lp_core/debugging/`。
    #. 为了调试运行在 LP 内核上的程序，需要先将调试信息和符号加载到 GDB 中。这可以通过 GDB 命令行或在 ``gdbinit`` 文件中完成。具体操作步骤请参见上文。
    #. LP 内核应用程序会在启时会加载到 RAM 中，在此之前设置的所有软件断点都会被覆盖。设置 LP 内核应用断点的最佳时机是在 LP 内核程序运行至 ``main`` 函数之时。
    #. 使用 IDE 时，可能无法配置上述 ``gdbinit`` 文件中的断点操作或命令。因此，请在调试会话开始前预设并禁用所有断点，只保留 ``main`` 函数处的断点。当程序在 ``main`` 处停止时，手动启用其余断点并恢复执行。

限制
~~~~

#. 调试场景有限制：目前，当 HP 内核或 LP 内核进入睡眠模式时，将无法调试。
#. 调试内核时，OpenOCD 不支持 FreeRTOS，因此无法看到系统中正在运行的任务，但会有几个线程代表 HP 和 LP 内核：

.. code-block:: bash

    (gdb) info thread
        Id   Target Id                                                          Frame
        1    Thread 1 "{IDF_TARGET_PATH_NAME}.cpu0" (Name: {IDF_TARGET_PATH_NAME}.cpu0, state: debug-request) 0x40803772 in esp_cpu_wait_for_intr ()
            at /home/user/projects/esp/esp-idf/components/esp_hw_support/cpu.c:64
      * 2    Thread 2 "{IDF_TARGET_PATH_NAME}.cpu1" (Name: {IDF_TARGET_PATH_NAME}.cpu1, state: breakpoint)    do_things (max=1000000000)
            at /home/user/projects/esp/esp-idf/examples/system/ulp/lp_core/debugging/main/lp_core/main.c:21

#. 在 GDB 中设置硬件断点时，这些断点会同时应用到两个内核上，因此可用的硬件断点数量受 LP 内核支持数量（{IDF_TARGET_NAME} 有 {IDF_TARGET_SOC_CPU_BREAKPOINTS_NUM} 个）所限。
#. OpenOCD 的 flash 支持被禁用。LP 内核应用程序完全在 RAM 中运行，且 GDB 可以为其使用软件断点，因而该限制无关紧要。但若想在 HP 内核运行的代码中调用的 flash 函数（例如 `app_main`）上设置断点，则需要通过 ``hb`` 和 ``thb`` GDB 命令显式请求设置硬件断点。
#. 由于主程序和 ULP 程序被链接为独立的二进制文件，它们可能会拥有相同名称的全局符号（如函数或变量）。若通过函数名称设置断点，则 GDB 将为所有同名函数设置断点。在调试 LP 内核时，OpenOCD 不支持 flash，因此如果上述函数位于 flash 中，可能会引发问题。此时建议通过源代码行号或函数的内存地址来设置断点。

应用示例
--------

.. list::

    - :example:`system/ulp/lp_core/gpio` 展示了 ULP LP 内核协处理器在主 CPU 深度睡眠时轮询 GPIO。
    :esp32c6: - :example:`system/ulp/lp_core/lp_i2c` 展示了 ULP LP 内核协处理器在主 CPU 深度睡眠时读取外部 I2C 环境光传感器 (BH1750)，并在达到阈值时唤醒主 CPU。
    - :example:`system/ulp/lp_core/lp_uart/lp_uart_echo` 展示了低功耗内核上运行的 LP UART 驱动程序如何读取并回显写入串行控制台的数据。
    - :example:`system/ulp/lp_core/lp_uart/lp_uart_print` 展示了如何在低功耗内核上使用串口打印功能。
    - :example:`system/ulp/lp_core/lp_uart/lp_uart_char_seq_wakeup` 展示了如何使用 LP UART 特定字符序列唤醒模式触发唤醒。
    - :example:`system/ulp/lp_core/interrupt` 展示了如何在 LP 内核上注册中断处理程序，接收由主 CPU 触发的中断。
    - :example:`system/ulp/lp_core/gpio_intr_pulse_counter` 展示了如何在主 CPU 处于 Deep-sleep 模式时，使用 GPIO 中断为脉冲计数。
    - :example:`system/ulp/lp_core/build_system/` 演示了如何为 ULP 应用程序添加自定义的 ``CMakeLists.txt`` 文件。
    - :example:`system/ulp/lp_core/debugging` 演示了如何使用 GDB 和 OpenOCD 来调试运行在 LP 内核上的代码。

API 参考
--------

主 CPU API 参考
~~~~~~~~~~~~~~~

.. include-build-file:: inc/ulp_lp_core.inc
.. include-build-file:: inc/lp_core_i2c.inc
.. include-build-file:: inc/lp_core_uart.inc

.. only:: SOC_LP_SPI_SUPPORTED

    .. include-build-file:: inc/lp_core_spi.inc

.. only:: SOC_LP_CORE_SUPPORT_ETM

    .. include-build-file:: inc/lp_core_etm.inc

.. include-build-file:: inc/lp_core_types.inc

LP 内核 API 参考
~~~~~~~~~~~~~~~~

.. include-build-file:: inc/ulp_lp_core_utils.inc
.. include-build-file:: inc/ulp_lp_core_gpio.inc
.. include-build-file:: inc/ulp_lp_core_i2c.inc
.. include-build-file:: inc/ulp_lp_core_uart.inc
.. include-build-file:: inc/ulp_lp_core_print.inc
.. include-build-file:: inc/ulp_lp_core_interrupts.inc

.. only:: SOC_LP_SPI_SUPPORTED

    .. include-build-file:: inc/ulp_lp_core_spi.inc

.. _esp-idf-monitor: https://github.com/espressif/esp-idf-monitor

.. only:: SOC_UART_HAS_LP_UART

    .. include-build-file:: inc/ulp_lp_core_lp_uart_shared.inc
