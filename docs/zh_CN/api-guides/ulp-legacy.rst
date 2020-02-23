ULP 协处理器 (传统的 GNU Make)
=================================

:link_to_translation:`en:[English]`

.. toctree::
   :maxdepth: 1

   指令集参考 <ulp_instruction_set>
   使用宏进行编程（遗留）<ulp_macros>

.. include:: ../gnu-make-legacy.rst

ULP（Ultra Low Power 超低功耗）协处理器是一种简单的有限状态机 (FSM)，可以在主处理器处于深度睡眠模式时，使用 ADC、温度传感器和外部 I2C 传感器执行测量操作。ULP 协处理器可以访问 RTC_SLOW_MEM 内存区域及 RTC_CNTL、RTC_IO、SARADC 等外设寄存器。ULP 协处理器使用 32 位固定宽度的指令，32 位内存寻址，配备 4 个 16 位通用寄存器。

安装工具链
-----------

ULP 协处理器代码是用汇编语言编写的，并使用 `binutils-esp32ulp toolchain` 进行编译。

1. 从 https://github.com/espressif/binutils-esp32ulp/releases 网站下载预编译的最新版本工具链二进制文件。

2. 将工具链提取至目录中，向工具链的 ``bin/`` 目录中添加路径至 ``PATH`` 环境变量中。

编译 ULP 代码
--------------

若需要将 ULP 代码编译为某组件的一部分，则必须执行以下步骤：

1. 用汇编语言编写的 ULP 代码必须导入到一个或多个 `.S` 扩展文件中，且这些文件必须放在组件目录中一个独立的目录中，例如 `ulp/`。 

.. note: 该目录不要添加到 ``COMPONENT_SRCDIRS`` 环境变量中。因为 ESP-IDF 构建系统将基于文件扩展名编译在 ``COMPONENT_SRCDIRS`` 中搜索到的文件。对于 ``.S`` 文件，使用的是 ``xtensa-esp32-elf-as`` 汇编器。但这并不适用于 ULP 程序集文件，因此体现这种区别的最简单方法就是将 ULP 程序集文件放到单独的目录中。

2. 修改组件 makefile，添加下列参数::

    ULP_APP_NAME ?= ulp_$(COMPONENT_NAME)
    ULP_S_SOURCES = $(COMPONENT_PATH)/ulp/ulp_source_file.S
    ULP_EXP_DEP_OBJECTS := main.o
    include $(IDF_PATH)/components/ulp/component_ulp_common.mk
   
   代码解释如下：

   ULP_APP_NAME
    为生成的 ULP 应用程序命名，不带扩展名。此名称用于 ULP 应用程序的构建输出：ELF 文件、map 文件、二进制文件、生成的头文件和链接器导出文件。

   ULP_S_SOURCES
    设置要传递给 ULP 汇编器的程序集文件列表。列表中的路径须是绝对路径，即以 ``$(COMPONENT_PATH)`` 开头的路径。如果列表中需列出多个文件，可使用函数 ``$(addprefix)``。因为路径与组件构建目录相关，因而不需前置。

   ULP_EXP_DEP_OBJECTS
    设置组件中目标文件名称的列表，其中包含生成的头文件。此列表用以建立正确的依赖项，并确保在此类文件被编译前生成头文件。ULP 应用程序头文件的定义请见下文。

   include $(IDF_PATH)/components/ulp/component_ulp_common.mk
    包含常见的 ULP 编译步骤。为 ULP 目标文件、ELF 文件、二进制文件等定义编译目标。

3. 使用常规方法（例如 ``idf.py build`` 或 ``idf.py app``）编译应用程序
   
  在编译过程中，构建系统将按照以下步骤编译 ULP 程序：

  1. **通过 C 预处理器运行每个程序集文件 (foo.S)。** 此步骤在组件编译目录中生成预处理的程序集文件 (foo.ulp.pS)，同时生成依赖文件 (foo.ulp.d)。

  2. **通过汇编器运行预处理过的汇编源码。** 此步骤会生成目标文件 (foo.ulp.o) 和清单文件 (foo.ulp.lst)。生成的清单文件用于调试，不用于构建过程的后续步骤。
 
  3. **通过 C 预处理器运行链接器脚本模板。** 模版位于 components/ulp/ld 目录中。

  4. **将目标文件链接到 ELF 输出文件** (ulp_app_name.elf)。此步骤生成的 .map 文件 (ulp_app_name.map) 可能会用于调试。

  5. **将 ELF 文件中的内容转储为二进制文件** (ulp_app_name.bin)，以便嵌入到应用程序中。

  6. **使用 esp32ulp-elf-nm 在 ELF 文件中生成全局符号列表** (ulp_app_name.sym)。

  7. **创建 LD 导出脚本和头文件** (ulp_app_name.ld 和 ulp_app_name.h)，包含来自 ulp_app_name.sym 的符号。此步骤通过 esp32ulp_mapgen.py 工具来完成。

  8. **将生成的二进制文件添加到要嵌入应用程序的二进制文件列表中。** 

访问 ULP 程序变量
------------------

在 ULP 程序中定义的全局符号也可以在主程序中使用。 

例如，ULP 程序可以定义 ``measurement_count`` 变量，此变量规定了芯片从深度睡眠模式唤醒之前，程序需要进行 ADC 测量的次数::

                            .global measurement_count
    measurement_count:      .long 0

                            /* later, use measurement_count */
                            move r3, measurement_count
                            ld r3, r3, 0

主程序需要在启动 ULP 程序之前初始化 ``measurement_count`` 变量，构建系统通过生成定义 ULP 程序中全局符号的 ``$(ULP_APP_NAME).h`` 和 ``$(ULP_APP_NAME).ld`` 文件，可以实现上述操作；这些文件包含了 ULP 程序中所有全局符号，文件名以 ``ulp_`` 开头。

头文件包含对此类符号的声明::
    
    extern uint32_t ulp_measurement_count;

注意，所有符号（包括变量、数组、函数）均被声明为 ``uint32_t``。对于函数和数组，先获取符号地址，然后转换为适当的类型。

生成的链接器脚本文件定义了符号在 RTC_SLOW_MEM 中的位置::

    PROVIDE ( ulp_measurement_count = 0x50000060 );

如果要从主程序访问 ULP 程序变量，应先使用 ``include`` 语句包含生成的头文件，这样，就可以像访问常规变量一样访问 ulp 程序变量。操作如下::

    #include "ulp_app_name.h"

    // later
    void init_ulp_vars() {
        ulp_measurement_count = 64;
    }

注意，ULP 程序只能使用 RTC 内存中 32 位字的低 16 位，因为寄存器是 16 位的，并且不存在从字的高位加载的指令。

同样，ULP 储存指令将寄存器值写入 32 位字的低 16 位中。写入高 16 位的值取决于存储指令的地址，因此在读取 ULP 写的变量时，主应用程序需要屏蔽高 16 位，例如::

    printf("Last measurement value: %d\n", ulp_last_measurement & UINT16_MAX);

启动 ULP 程序
---------------

要运行 ULP 程序，主应用程序需要调用 ``ulp_load_binary`` 函数将 ULP 程序加载到 RTC 内存中，然后调用 ``ulp_run`` 函数，启动 ULP 程序。

注意，在 menuconfig 中必须启用 "Enable Ultra Low Power (ULP) Coprocessor" 选项，以便为 ULP 预留内存。"RTC slow memory reserved for coprocessor" 选项设置的值必须足够储存 ULP 代码和数据。如果应用程序组件包含多个 ULP 程序，则 RTC 内存必须足以容纳最大的程序。

每个 ULP 程序均以二进制 BLOB 的形式嵌入到 ESP-IDF 应用程序中。应用程序可以引用此 BLOB，并以下面的方式加载此 BLOB （假设 ULP_APP_NAME 已被定义为 ``ulp_app_name``）::

    extern const uint8_t bin_start[] asm("_binary_ulp_app_name_bin_start");
    extern const uint8_t bin_end[]   asm("_binary_ulp_app_name_bin_end");

    void start_ulp_program() {
        ESP_ERROR_CHECK( ulp_load_binary(
            0 /* load address, set to 0 when using default linker scripts */,
            bin_start,
            (bin_end - bin_start) / sizeof(uint32_t)) );
    }

.. doxygenfunction:: ulp_load_binary

一旦上述程序加载到 RTC 内存后，应用程序即可启动此程序，并将入口点的地址传递给 ``ulp_run`` 函数::

    ESP_ERROR_CHECK( ulp_run(&ulp_entry - RTC_SLOW_MEM) );

.. doxygenfunction:: ulp_run

上述生成的头文件 ``$(ULP_APP_NAME).h`` 声明了入口点符号。在 ULP 应用程序的汇编源代码中，此符号必须标记为 ``.global``::

            .global entry
    entry:
            /* code starts here */

ULP 程序流
-----------

ULP 协处理器由定时器启动，而调用 ``ulp_run`` 则可启动此定时器。定时器为 RTC_SLOW_CLK 的 Tick 事件计数（默认情况下，Tick 由内部 150 kHz 晶振器生成）。使用 ``SENS_ULP_CP_SLEEP_CYCx_REG`` 寄存器 (x = 0..4) 设置 Tick 数值。第一次启动 ULP 时，使用 ``SENS_ULP_CP_SLEEP_CYC0_REG`` 设置定时器 Tick 数值，之后，ULP 程序可以使用 ``sleep`` 指令来另外选择 ``SENS_ULP_CP_SLEEP_CYCx_REG`` 寄存器。

此应用程序可以调用 ``ulp_set_wakeup_period`` 函数来设置 ULP 定时器周期值 (SENS_ULP_CP_SLEEP_CYCx_REG, x = 0..4)。

.. doxygenfunction:: ulp_set_wakeup_period

一旦定时器数值达到在所选的 ``SENS_ULP_CP_SLEEP_CYCx_REG`` 寄存器中设置的数值，ULP 协处理器就会启动，并调用 ``ulp_run`` 的入口点，开始运行程序。

程序保持运行，直到遇到 ``halt`` 指令或非法指令才停止。一旦程序停止，ULP 协处理器电源关闭，定时器再次启动。

如果想禁用定时器（禁用定时器可有效防止 ULP 程序再次运行），请清除 ``RTC_CNTL_STATE0_REG`` 寄存器中的 ``RTC_CNTL_ULP_CP_SLP_TIMER_EN`` 位，可在 ULP 代码或主程序中进行以上操作。


.. _binutils-esp32ulp toolchain: https://github.com/espressif/binutils-esp32ulp
