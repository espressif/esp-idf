ULP LP-Core 协处理器编程
===================================

:link_to_translation:`en:[English]`

ULP LP-Core（低功耗内核）协处理器是 {IDF_TARGET_NAME} 中 ULP 的一个变型。它具有超低功耗，同时还能在主 CPU 处于低功耗模式时保持运行。因此，LP-Core 协处理器能够在主 CPU 处于睡眠模式时处理 GPIO 或传感器读取等任务，从而显著降低整个系统的整体功耗。

ULP LP-Core 协处理器具有以下功能：

* 利用基于 RISC-V ISA 的 32 位处理器，包括标准扩展整数 (I)、乘法/除法 (M)、原子 (A) 和压缩 (C)。
* 中断控制器。
* 包含一个调试模块，支持通过 JTAG 进行外部调试。
* 当整个系统处于 active 模式时，可以访问所有的高功耗 (HP) SRAM 和外设。
* 当 HP 系统处于睡眠模式时，可以访问低功耗 (LP) SRAM 和外设。

编译 ULP LP-Core 代码
----------------------------------

ULP LP-Core 代码会与 ESP-IDF 项目共同编译，生成一个单独的二进制文件，并自动嵌入到主项目的二进制文件中。编译操作如下：

1. 将用 C 语言或汇编语言编写的 ULP LP-Core 代码（带有 ``.S`` 扩展名）放在组件目录下的专用目录中，例如 ``ulp/``。

2. 在 CMakeLists.txt 文件中注册组件后，调用 ``ulp_embed_binary`` 函数。例如：

    idf_component_register()

    set(ulp_app_name ulp_${COMPONENT_NAME})
    set(ulp_sources "ulp/ulp_c_source_file.c" "ulp/ulp_assembly_source_file.S")
    set(ulp_exp_dep_srcs "ulp_c_source_file.c")

    ulp_embed_binary(${ulp_app_name} "${ulp_sources}" "${ulp_exp_dep_srcs}")

``ulp_embed_binary`` 的第一个参数为 ULP 二进制文件的文件名，该文件名也用于其他生成的文件，如 ELF 文件、映射文件、头文件和链接器导出文件。第二个参数为 ULP 源文件。第三个参数为组件源文件列表，用于包含要生成的头文件。要正确构建依赖关系、确保在编译这些文件前创建要生成的头文件，都需要此文件列表。有关 ULP 应用程序生成头文件的概念，请参阅本文档后续章节。

1. 在 menuconfig 中启用 :ref:`CONFIG_ULP_COPROC_ENABLED` 和 :ref:`CONFIG_ULP_COPROC_TYPE` 选项，并将 :ref:`CONFIG_ULP_COPROC_TYPE` 设置为 ``CONFIG_ULP_COPROC_TYPE_LP_CORE``。:ref:`CONFIG_ULP_COPROC_RESERVE_MEM` 选项为 ULP 保留 RTC 内存，因此必须设置为一个足够大的值，以存储 ULP LP-Core 代码和数据。如果应用程序组件包含多个 ULP 程序，那么 RTC 内存的大小必须足够容纳其中最大的程序。

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

访问 ULP LP-Core 程序变量
-------------------------------------------

在主程序中可以使用在 ULP LP-Core 程序中定义的全局符号。

例如，ULP LP-Core 程序定义了一个变量 ``measurement_count``，用来表示程序从深度睡眠中唤醒芯片前所需的 GPIO 测量次数。

.. code-block:: c

    volatile int measurement_count;

    int some_function()
    {
        //读取测量次数以便后续使用。
        int temp = measurement_count;

        ...do something.
    }

主程序可以访问 ULP LP-Core 程序全局变量，这是因为构建系统生成了 ``${ULP_APP_NAME}.h`` 和 ``${ULP_APP_NAME}.ld`` 文件，文件中定义了 ULP LP-Core 程序中现有的的全局符号。在 ULP LP-Core 程序中定义的每个全局符号都包含在这两个文件中，并具有前缀 ``ulp_``。

头文件中包含符号的声明：

.. code-block:: c

    extern uint32_t ulp_measurement_count;

注意，所有的符号（变量、数组、函数）都被声明为 ``uint32_t`` 类型。对于函数和数组，获取符号的地址并将其转换为合适的类型。

生成的链接器脚本文件定义了 LP_MEM 中符号的位置::

    PROVIDE ( ulp_measurement_count = 0x50000060 );

要从主程序访问 ULP LP-Core 程序变量，应使用 ``include`` 语句将生成的头文件包含在主程序中，这样就可以像访问常规变量一样访问 ULP LP-Core 程序变量。

.. code-block:: c

    #include "ulp_app_name.h"

    void init_ulp_vars() {
        ulp_measurement_count = 64;
    }


启动 ULP LP-Core 程序
--------------------------------

要运行 ULP LP-Core 程序，主应用程序需要先使用 :cpp:func:`ulp_lp_core_load_binary` 函数将 ULP 程序加载到 RTC 内存中，然后使用 :cpp:func:`ulp_lp_core_run` 函数进行启动。

每个 ULP LP-Core 程序以二进制 blob 的形式嵌入到 ESP-IDF 应用程序中。应用程序可以按照如下方式引用和加载该 blob（假设 ULP_APP_NAME 被定义为 ``ulp_app_name``）：

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

ULP LP-Core 程序流程
------------------------

ULP LP-Core 协处理器如何启动取决于 :cpp:type:`ulp_lp_core_cfg_t` 中选择的唤醒源。最常见的用例是 ULP 定期唤醒，在进行一些测量后唤醒主 CPU，或者再次进入睡眠状态。

ULP 有以下唤醒源：
    * :c:macro:`ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU` - LP 内核可以被 HP CPU 唤醒。
    * :c:macro:`ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER` - LP 内核可以被 LP 定时器唤醒。
    * :c:macro:`ULP_LP_CORE_WAKEUP_SOURCE_ETM` - LP 内核可以被 ETM 事件唤醒。（暂不支持）
    * :c:macro:`ULP_LP_CORE_WAKEUP_SOURCE_LP_IO` - 当 LP IO 电平变化时，LP 内核会被唤醒。（暂不支持）
    * :c:macro:`ULP_LP_CORE_WAKEUP_SOURCE_LP_UART` - LP 内核在接收到一定数量的 UART RX 脉冲后会被唤醒。（暂不支持）

ULP 被唤醒时会经历以下步骤：

.. list::

    :CONFIG_ESP_ROM_HAS_LP_ROM: #. 除非已指定 :cpp:member:`ulp_lp_core_cfg_t::skip_lp_rom_boot`，否则运行 ROM 启动代码并跳转至 LP RAM 中的入口地址。ROM 启动代码将初始化 LP UART 并打印启动信息。
    #. 初始化系统功能，如中断
    #. 调用用户代码 ``main()``
    #. 从 ``main()`` 返回
    #. 如果指定了 ``lp_timer_sleep_duration_us``，则配置下一个唤醒闹钟
    #. 调用 :cpp:func:`ulp_lp_core_halt`


ULP LP-Core 支持的外设
------------------------------

为了增强 ULP LP-Core 协处理器的功能，它可以访问在低功耗电源域运行的外设。ULP LP-Core 协处理器可以在主 CPU 处于睡眠模式时与这些外设进行交互，并在达到唤醒条件时唤醒主 CPU。以下为支持的外设：

.. list::

    * LP IO
    * LP I2C
    * LP UART

.. only:: CONFIG_ESP_ROM_HAS_LP_ROM

    ULP LP-Core ROM
    ---------------

    ULP LP-Core ROM 是位于 LP-ROM 中的一小段预编译代码，用户无法修改。与主 CPU 运行的引导加载程序 ROM 代码类似，ULP LP-Core ROM 也在 ULP LP-Core 协处理器启动时执行。该 ROM 代码会初始化 ULP LP-Core 协处理器，随后跳转到用户程序。如果已初始化 LP UART，该 ROM 代码还会打印启动信息。

    如果已将 :cpp:member:`ulp_lp_core_cfg_t::skip_lp_rom_boot` 设置为真，则不会执行 ULP LP-Core ROM 代码。如需尽快唤醒 ULP，同时避免初始化和信息打印产生额外开销，则可使用这一功能。

    除上述启动代码，ULP LP-Core ROM 代码还提供以下功能和接口：

    * :component_file:`ROM.ld 接口 <esp_rom/{IDF_TARGET_PATH_NAME}/ld/{IDF_TARGET_PATH_NAME}lp.rom.ld>`
    * :component_file:`newlib.ld 接口 <esp_rom/{IDF_TARGET_PATH_NAME}/ld/{IDF_TARGET_PATH_NAME}lp.rom.newlib.ld>`

    在任何情况下，这些函数都存在于 LP-ROM 中，因此在程序中使用这些函数可以减少 ULP 应用程序的 RAM 占用。


应用示例
--------------------

* 在示例 :example:`system/ulp/lp_core/gpio` 中，ULP LP-Core 协处理器在主 CPU 深度睡眠时轮询 GPIO。
* 在示例 :example:`system/ulp/lp_core/lp_i2c` 中，ULP LP-Core 协处理器在主 CPU 深度睡眠时读取外部 I2C 环境光传感器 (BH1750)，并在达到阈值时唤醒主 CPU。
* 在示例 :example:`system/ulp/lp_core/lp_uart/lp_uart_echo` 中，低功耗内核上运行的 LP UART 驱动程序读取并回显写入串行控制台的数据。
* :example:`system/ulp/lp_core/lp_uart/lp_uart_print` 展示了如何在低功耗内核上使用串口打印功能。

API 参考
-------------

主 CPU API 参考
~~~~~~~~~~~~~~~~~~~~~~

.. include-build-file:: inc/ulp_lp_core.inc
.. include-build-file:: inc/lp_core_i2c.inc
.. include-build-file:: inc/lp_core_uart.inc

LP 内核 API 参考
~~~~~~~~~~~~~~~~~~~~~~

.. include-build-file:: inc/ulp_lp_core_utils.inc
.. include-build-file:: inc/ulp_lp_core_gpio.inc
.. include-build-file:: inc/ulp_lp_core_i2c.inc
.. include-build-file:: inc/ulp_lp_core_uart.inc
.. include-build-file:: inc/ulp_lp_core_print.inc
