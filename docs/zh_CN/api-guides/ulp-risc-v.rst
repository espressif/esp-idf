ULP-RISC-V 协处理器编程
==================================
:link_to_translation:`en:[English]`

.. only:: esp32s3

    .. warning::

        此功能不适用于 v4.4 版本。


.. toctree::
   :maxdepth: 1


ULP-RISC-V 协处理器是 ULP 的一种变体，用于 ESP32-S2。与 ULP 类似，ULP-RISC-V 协处理器可以在主处理器处于低功耗模式时执行传感器读数等任务。与 ULP-FSM 不同，ULP-RISC-V 可以通过标准 GNU 工具使用 C 语言进行编程。ULP-RISC-V 可以访问 RTC_SLOW_MEM 内存区域及 RTC_CNTL、RTC_IO、SARADC 等外设的寄存器。RISC-V 处理器是一种 32 位定点处理器，指令集基于 RV32IMC，包括硬件乘除法和压缩指令。

安装 ULP-RISC-V 工具链
-----------------------------------

ULP-RISC-V 协处理器代码以 C 语言编写（也可能是汇编语言），使用基于 GCC 的 RISC-V 工具链进行编译。

如果你已依照 :doc:`快速入门指南 <../../get-started/index>` 中的介绍安装好了 ESP-IDF 及其 CMake 构建系统，那么 ULP-RISC-V 工具链已经被默认安装到了你的开发环境中。

.. note: 在早期版本的ESP-IDF中，RISC-V工具链具有不同的名称：`riscv-none-embed-gcc`。

编译 ULP-RISC-V 代码
-----------------------------

要将 ULP-RISC-V 代码编译为某组件的一部分，必须执行以下步骤：

1. ULP-RISC-V 代码以 C 语言或汇编语言编写（必须使用 `.S` 扩展名)，必须放在组件目录中一个独立的目录中，例如 `ulp/`。

.. note: 当注册组件时（通过 ``idf_component_register``），该目录不应被添加至 ``SRC_DIRS`` 参数，因为目前 ULP-FSM 需要进行此步骤。如何正确添加 ULP 源文件，请见以下步骤。

2. 注册后从组件 CMakeLists.txt 中调用 ``ulp_embed_binary`` 示例如下::

    ...
    idf_component_register()

    set(ulp_app_name ulp_${COMPONENT_NAME})
    set(ulp_sources "ulp/ulp_c_source_file.c" "ulp/ulp_assembly_source_file.S")
    set(ulp_exp_dep_srcs "ulp_c_source_file.c")

    ulp_embed_binary(${ulp_app_name} "${ulp_sources}" "${ulp_exp_dep_srcs}")

 ``ulp_embed_binary`` 的第一个参数指定生成的 ULP 二进制文件名。生成的其他文件，
 如 ELF 文件、.map 文件、头文件和链接器导出文件等也可使用此名称。第二个参数指定 ULP 源文件。
 最后，第三个参数指定组件源文件列表，其中包括生成的头文件。
 此列表用以正确构建依赖，并确保在构建过程中先生成后编译包含头文件的源文件。
 请参考下文，查看为 ULP 应用程序生成的头文件等相关概念。

3. 使用常规方法（例如 `idf.py app`）编译应用程序

   在内部，构建系统将按照以下步骤编译 ULP 程序：

   1. **通过 C 编译器和汇编器运行每个源文件。** 此步骤在组件编译目录中生成目标文件（.obj.c 或 .obj.S，取决于处理的源文件）。

   2. **通过 C 预处理器运行链接器脚本模版。** 模版位于 ``components/ulp/ld`` 目录中。

   4. **将目标文件链接到 ELF 输出文件** (``ulp_app_name.elf``)。此步骤生成的 .Map 文件默认用于调试 (``ulp_app_name.map``)。

   5. **将 ELF 文件中的内容转储为二进制文件** (``ulp_app_name.bin``)，以便嵌入到应用程序中。

   6. 使用 ``riscv32-esp-elf-nm`` 在 ELF 文件中 **生成全局符号列表** (``ulp_app_name.sym``)。

   7. **创建 LD 导出脚本和头文件** （``ulp_app_name.ld`` 和 ``ulp_app_name.h``），包含来自 ``ulp_app_name.sym`` 的符号。此步骤可借助 ``esp32ulp_mapgen.py`` 工具来完成。

   8. **将生成的二进制文件添加到要嵌入应用程序的二进制文件列表中。** 

访问 ULP-RISC-V 程序变量
----------------------------

在 ULP-RISC-V 程序中定义的全局符号也可以在主程序中使用。 

例如，ULP-RISC-V 程序可以定义 ``measurement_count`` 变量，此变量可以定义程序从深度睡眠中唤醒芯片之前需要进行的 ADC 测量的次数。

.. code-block:: c

    volatile int measurement_count;

    int some_function()
    {
        //read the measurement count for use it later.
        int temp = measurement_count;

        ...do something.
    }

构建系统生成定义 ULP 编程中全局符号的 ``${ULP_APP_NAME}.h`` 和 ``${ULP_APP_NAME}.ld`` 文件，使主程序能够访问全局 ULP-RISC-V 程序变量。上述两个文件包含 ULP 程序中定义的所有全局符号，且这些符号均以 ``ulp_`` 开头，。

头文件包含对此类符号的声明

.. code-block:: c

    extern uint32_t ulp_measurement_count;

注意，所有符号（包括变量、数组、函数）均被声明为 ``uint32_t``。 函数和数组需要先获取符号地址、再转换为适当的类型。

生成的链接器文本定义了符号在 RTC_SLOW_MEM 中的位置::

    PROVIDE ( ulp_measurement_count = 0x50000060 );

要从主程序访问 ULP-RISC-V 程序变量，需使用 ``include`` 语句包含生成的头文件。这样，就可以像访问常规变量一样访问 ULP 程序变量。

.. code-block:: c

    #include "ulp_app_name.h"

    void init_ulp_vars() {
        ulp_measurement_count = 64;
    }

启动 ULP-RISC-V 程序
-------------------------------

要运行 ULP-RISC-V 程序，主程序需要调用 :cpp:func:`ulp_riscv_load_binary` 函数，将 ULP 程序加载到 RTC 内存中，然后调用 :cpp:func:`ulp_riscv_run` 函数，启动 ULP-RISC-V 程序。

注意，必须在 menuconfig 中启用 `CONFIG_ESP32S2_ULP_COPROC_ENABLED` 和 `CONFIG_ESP32S2_ULP_COPROC_RISCV` 选项，以便为 ULP 预留内存。"RTC slow memory reserved for coprocessor" 选项设置的值必须足够存储 ULP 代码和数据。如果应用程序组件包含多个 ULP 程序，RTC 内存必须足以容纳最大的程序。

每个 ULP-RISC-V 程序均以二进制 BLOB 的形式嵌入到 ESP-IDF 应用程序中。应用程序可以引用此 BLOB，并以下面的方式加载此 BLOB（假设 ULP_APP_NAME 已被定义为 ``ulp_app_name``）：

.. code-block:: c

    extern const uint8_t bin_start[] asm("_binary_ulp_app_name_bin_start");
    extern const uint8_t bin_end[]   asm("_binary_ulp_app_name_bin_end");

    void start_ulp_program() {
        ESP_ERROR_CHECK( ulp_riscv_load_binary( bin_start,
            (bin_end - bin_start)) );
    }

.. doxygenfunction:: ulp_riscv_load_binary()

一旦上述程序加载到 RTC 内存后，应用程序即可调用 :cpp:func:`ulp_riscv_run` 函数启动此程序：

.. code-block:: c

    ESP_ERROR_CHECK( ulp_riscv_run() );

.. doxygenfunction:: ulp_riscv_run()

ULP-RISC-V 程序流
-----------------------

ULP-RISC-V 协处理器由定时器启动，调用 :cpp:func:`ulp_riscv_run` 即可启动定时器。定时器为 RTC_SLOW_CLK 的 Tick 事件计数（默认情况下，Tick 由内部 90 kHz RC 振荡器产生）。Tick 数值使用 ``RTC_CNTL_ULP_CP_TIMER_1_REG`` 寄存器设置。启用 ULP 时，使用 ``RTC_CNTL_ULP_CP_TIMER_1_REG`` 设置定时器 Tick 数值。

此应用程序可以调用 :cpp:func:`ulp_set_wakeup_period` 函数来设置 ULP 定时器周期值 (RTC_CNTL_ULP_CP_TIMER_1_REG)。

一旦定时器数到 ``RTC_CNTL_ULP_CP_TIMER_1_REG`` 寄存器中设置的 Tick 数，ULP 协处理器就会启动，并调用 :cpp:func:`ulp_riscv_run` 的入口点开始运行程序。

程序保持运行，直至 ``RTC_CNTL_COCPU_CTRL_REG`` 寄存器中的 ``RTC_CNTL_COCPU_DONE`` 字段被置位或因非法处理器状态出现陷阱。一旦程序停止，ULP 协处理器会关闭电源，定时器再次启动。

如需禁用定时器（有效防止 ULP 程序再次运行），请清除 ``RTC_CNTL_STATE0_REG`` 寄存器中的 ``RTC_CNTL_ULP_CP_SLP_TIMER_EN`` 位，此项操作可在 ULP 代码或主程序中进行。

