ULP RISC-V 协处理器编程
==================================

:link_to_translation:`en:[English]`

ULP RISC-V 协处理器是 ULP 的一种变体，用于 {IDF_TARGET_NAME}。与 ULP FSM 类似，ULP RISC-V 协处理器可以在主 CPU 处于低功耗模式时执行传感器读数等任务。其与 ULP FSM 的主要区别在于，ULP RISC-V 可以通过标准 GNU 工具使用 C 语言进行编程。ULP RISC-V 可以访问 RTC_SLOW_MEM 内存区域及 ``RTC_CNTL``、``RTC_IO``、``SARADC`` 等外设的寄存器。RISC-V 处理器是一种 32 位定点处理器，指令集基于 RV32IMC，包括硬件乘除法和压缩指令。

安装 ULP RISC-V 工具链
-----------------------------------

ULP RISC-V 协处理器代码以 C 语言（或汇编语言）编写，使用基于 GCC 的 RISC-V 工具链进行编译。

如果依照 :doc:`快速入门指南 <../../../get-started/index>` 中的介绍安装好了 ESP-IDF 及其 CMake 构建系统，那么 ULP RISC-V 工具链已经被默认安装到了你的开发环境中。

.. note::

    在早期版本的 ESP-IDF 中，RISC-V 工具链具有不同的名称：``riscv-none-embed-gcc``。

编译 ULP RISC-V 代码
-----------------------------

要将 ULP RISC-V 代码编译为某组件的一部分，必须执行以下步骤：

1. ULP RISC-V 代码以 C 语言或汇编语言编写（必须使用 ``.S`` 扩展名），必须放在组件目录中一个独立的目录中，例如 ``ulp/``。

.. note::

    当注册组件时（通过 ``idf_component_register``），该目录不应被添加至 ``SRC_DIRS`` 参数，因为目前该步骤需用于 ULP FSM。如何正确添加 ULP 源文件，请见以下步骤。

2. 注册后从组件 CMakeLists.txt 中调用 ``ulp_embed_binary`` 示例如下::

    ...
    idf_component_register()

    set(ulp_app_name ulp_${COMPONENT_NAME})
    set(ulp_sources "ulp/ulp_c_source_file.c" "ulp/ulp_assembly_source_file.S")
    set(ulp_exp_dep_srcs "ulp_c_source_file.c")

    ulp_embed_binary(${ulp_app_name} "${ulp_sources}" "${ulp_exp_dep_srcs}")

 ``ulp_embed_binary`` 的第一个参数指定生成的 ULP 二进制文件名。生成的其他文件，如 ELF 文件、.map 文件、头文件和链接器导出文件等也可使用此名称。第二个参数指定 ULP 源文件。最后，第三个参数指定组件源文件列表，其中包括生成的头文件。此列表用以正确构建依赖，并确保在构建过程中先生成后编译包含头文件的源文件。请参考下文，查看为 ULP 应用程序生成的头文件等相关概念。

3. 使用常规方法（例如 ``idf.py app``）编译应用程序。

   在内部，构建系统将按照以下步骤编译 ULP 程序：

   1. **通过 C 编译器和汇编器运行每个源文件。** 此步骤在组件编译目录中生成目标文件（ ``.obj.c`` 或 ``.obj.S``，取决于处理的源文件）。

   2. **通过 C 预处理器运行链接器脚本模版。** 模版位于 ``components/ulp/ld`` 目录中。

   3. **将目标文件链接到 ELF 输出文件** (``ulp_app_name.elf``)。此步骤生成的 .map 文件默认用于调试 (``ulp_app_name.map``)。

   4. **将 ELF 文件中的内容转储为二进制文件** (``ulp_app_name.bin``)，以便嵌入到应用程序中。

   5. 使用 ``riscv32-esp-elf-nm`` 在 ELF 文件中 **生成全局符号列表** (``ulp_app_name.sym``)。

   6. **创建 LD 导出脚本和头文件** （``ulp_app_name.ld`` 和 ``ulp_app_name.h``），包含来自 ``ulp_app_name.sym`` 的符号。此步骤可借助 ``esp32ulp_mapgen.py`` 工具来完成。

   7. **将生成的二进制文件添加到要嵌入应用程序的二进制文件列表中。**

.. _ulp-riscv-access-variables:

访问 ULP RISC-V 程序变量
----------------------------

在 ULP RISC-V 程序中定义的全局符号也可以在主程序中使用。

例如，ULP RISC-V 程序可以定义 ``measurement_count`` 变量，此变量可以定义程序从深度睡眠中唤醒芯片之前需要进行的 ADC 测量的次数。

.. code-block:: c

    volatile int measurement_count;

    int some_function()
    {
        //读取测量计数，后续需使用
        int temp = measurement_count;

        ...do something.
    }

构建系统生成定义 ULP 编程中全局符号的 ``${ULP_APP_NAME}.h`` 和 ``${ULP_APP_NAME}.ld`` 文件，使主程序能够访问全局 ULP RISC-V 程序变量。上述两个文件包含 ULP RISC-V 程序中定义的所有全局符号，且这些符号均以 ``ulp_`` 开头。

头文件包含对此类符号的声明：

.. code-block:: c

    extern uint32_t ulp_measurement_count;

注意，所有符号（包括变量、数组、函数）均被声明为 ``uint32_t``。函数和数组需要先获取符号地址，再转换为适当的类型。

生成的链接器文本定义了符号在 RTC_SLOW_MEM 中的位置::

    PROVIDE ( ulp_measurement_count = 0x50000060 );

要从主程序访问 ULP RISC-V 程序变量，需使用 ``include`` 语句包含生成的头文件。这样，就可以像访问常规变量一样访问 ULP RISC-V 程序变量。

.. code-block:: c

    #include "ulp_app_name.h"

    void init_ulp_vars() {
        ulp_measurement_count = 64;
    }

互斥
^^^^^^^

如果想要互斥地访问被主程序和 ULP 程序共享的变量，则可以通过 ULP RISC-V Lock API 来实现：

 * :cpp:func:`ulp_riscv_lock_acquire`
 * :cpp:func:`ulp_riscv_lock_release`

ULP 中的所有硬件指令都不支持互斥，所以 Lock API 需通过一种软件算法（`Peterson 算法 <https://zh.wikipedia.org/wiki/Peterson%E7%AE%97%E6%B3%95>`_ ）来实现互斥。

注意，只能从主程序的单个线程中调用这些锁，如果多个线程同时调用，将无法启用互斥功能。

启动 ULP RISC-V 程序
-------------------------------

要运行 ULP RISC-V 程序，主程序需要调用 :cpp:func:`ulp_riscv_load_binary` 函数，将 ULP 程序加载到 RTC 内存中，然后调用 :cpp:func:`ulp_riscv_run` 函数，启动 ULP RISC-V 程序。

注意，必须在 menuconfig 中启用 ``CONFIG_ULP_COPROC_ENABLED`` 和 ``CONFIG_ULP_COPROC_TYPE_RISCV`` 选项，以便正常运行 ULP RISC-V 程序。``RTC slow memory reserved for coprocessor`` 选项设置的值必须足够存储 ULP RISC-V 代码和数据。如果应用程序组件包含多个 ULP 程序，RTC 内存必须足以容纳最大的程序。

每个 ULP RISC-V 程序均以二进制 BLOB 的形式嵌入到 ESP-IDF 应用程序中。应用程序可以引用此 BLOB，并以下面的方式加载此 BLOB（假设 ULP_APP_NAME 已被定义为 ``ulp_app_name``）：

.. code-block:: c

    extern const uint8_t bin_start[] asm("_binary_ulp_app_name_bin_start");
    extern const uint8_t bin_end[]   asm("_binary_ulp_app_name_bin_end");

    void start_ulp_program() {
        ESP_ERROR_CHECK( ulp_riscv_load_binary( bin_start,
            (bin_end - bin_start)) );
    }

一旦上述程序加载到 RTC 内存后，应用程序即可调用 :cpp:func:`ulp_riscv_run` 函数启动此程序：

.. code-block:: c

    ESP_ERROR_CHECK( ulp_riscv_run() );

ULP RISC-V 程序流
-----------------------

{IDF_TARGET_RTC_CLK_FRE:default="150 kHz", esp32s2="90 kHz", esp32s3="136 kHz"}

ULP RISC-V 协处理器由定时器启动，调用 :cpp:func:`ulp_riscv_run` 即可启动定时器。定时器为 RTC_SLOW_CLK 的 Tick 事件计数（默认情况下，Tick 由内部 90 kHz RC 振荡器产生）。Tick 数值使用 ``RTC_CNTL_ULP_CP_TIMER_1_REG`` 寄存器设置。启用 ULP 时，使用 ``RTC_CNTL_ULP_CP_TIMER_1_REG`` 设置定时器 Tick 数值。

此应用程序可以调用 :cpp:func:`ulp_set_wakeup_period` 函数来设置 ULP 定时器周期值 (RTC_CNTL_ULP_CP_TIMER_1_REG)。

一旦定时器数到 ``RTC_CNTL_ULP_CP_TIMER_1_REG`` 寄存器中设置的 Tick 数，ULP RISC-V 协处理器就会启动，并调用 :cpp:func:`ulp_riscv_run` 的入口点开始运行程序。

程序保持运行，直至 ``RTC_CNTL_COCPU_CTRL_REG`` 寄存器中的 ``RTC_CNTL_COCPU_DONE`` 字段被置位或因非法处理器状态出现陷阱。一旦程序停止，ULP RISC-V 协处理器会关闭电源，定时器再次启动。

如需禁用定时器（有效防止 ULP 程序再次运行），请清除 ``RTC_CNTL_STATE0_REG`` 寄存器中的 ``RTC_CNTL_ULP_CP_SLP_TIMER_EN`` 位，此项操作可在 ULP 代码或主程序中进行。

ULP RISC-V 外设支持
-------------------

为了增强性能，ULP RISC-V 协处理器可以访问在低功耗 (RTC) 电源域中运行的外设。当主 CPU 处于睡眠模式时，ULP RISC-V 协处理器可与这些外设进行交互，并在满足唤醒条件时唤醒主 CPU。以下为所支持的外设类型。

RTC I2C
^^^^^^^^

RTC I2C 控制器提供了在 RTC 电源域中作为 I2C 主机的功能。ULP RISC-V 协处理器可以使用该控制器对 I2C 从机设备进行读写操作。如要使用 RTC I2C 外设，需在初始化 ULP RISC-V 内核并在其进入睡眠模式之前，先在主内核上运行的应用程序中调用 :cpp:func:`ulp_riscv_i2c_master_init` 函数。

初始化 RTC I2C 控制器之后，请务必先用 :cpp:func:`ulp_riscv_i2c_master_set_slave_addr` API 将 I2C 从机设备地址编入程序，再执行读写操作。

.. note::

    RTC I2C 外设首先将检查 :cpp:func:`ulp_riscv_i2c_master_set_slave_reg_addr` API 是否将从机子寄存器地址编入程序。如未编入，I2C 外设将以 ``SENS_SAR_I2C_CTRL_REG[18:11]`` 作为后续读写操作的子寄存器地址。这可能会导致 RTC I2C 外设与某些无需对子寄存器进行配置的 I2C 设备或传感器不兼容。

.. note::

    在主 CPU 访问 RTC I2C 外设和 ULP RISC-V 内核访问 RTC I2C 外设之间，未提供硬件原子操作的正确性保护，因此请勿让两个内核同时访问外设。

如果基于 RTC I2C 的 ULP RISC-V 程序未按预期运行，可以进行以下完整性检查排查问题：

 * SDA/SCL 管脚选择问题：SDA 管脚只能配置为 GPIO1 或 GPIO3，SCL 管脚只能配置为 GPIO0 或 GPIO2。请确保管脚配置正确。

 * I2C 时序参数问题：RTC I2C 总线时序配置受到 I2C 标准总线规范限制，任何违反标准 I2C 总线规范的时序参数都会导致错误。了解有关时序参数的详细信息，请阅读 `标准 I2C 总线规范 <https://en.wikipedia.org/wiki/I%C2%B2C>`_。

 * 如果 I2C 从机设备或传感器不需要子寄存器地址进行配置，它可能与 RTC I2C 外设不兼容。请参考前文注意事项。

 * 如果 RTC 驱动程序在主 CPU 上运行时出现 ``Write Failed!`` 或 ``Read Failed!`` 的错误日志，检查是否出现以下情况：

        * I2C 从机设备或传感器与乐鑫 SoC 上的标准 I2C 主机设备一起正常工作，说明 I2C 从机设备本身没有问题。
        * 如果 RTC I2C 中断状态日志报告 ``TIMEOUT`` 错误或 ``ACK`` 错误，则通常表示 I2C 设备未响应 RTC I2C 控制器发出的 ``START`` 条件。如果 I2C 从机设备未正确连接到控制器管脚或处于异常状态，则可能会发生这种情况。在进行后续操作之前，请确保 I2C 从机设备状态良好且连接正确。
        * 如果 RTC I2C 中断日志没有报告任何错误状态，则可能表示驱动程序接收 I2C 从机设备数据时速度较慢。这可能是由于 RTC I2C 控制器没有 TX/RX FIFO 来存储多字节数据，而是依赖于使用中断状态轮询机制来进行单字节传输。通过在外设的初始化配置参数中设置 SCL 低周期和 SCL 高周期，可以尽量提高外设 SCL 时钟的运行速度，在一定程度上缓解这一问题。

* 调试问题的方法还包括确保 RTC I2C 控制器 **仅** 在主 CPU 上运行， **没有** ULP RISC-V 代码干扰，并且没有激活 **任何** 睡眠模式。这是确保 RTC I2C 外设正常工作的基本配置。通过这种方式，可以排除由 ULP 或睡眠模式可能引起的任何潜在问题。

ULP RISC-V 中断处理
------------------------------

ULP RISC-V 内核支持来自特定内部和外部事件的中断处理。设计上，ULP RISC-V 内核可以处理以下来源的中断：

.. list-table:: ULP RISC-V 中断源
    :widths: 10 5 5
    :header-rows: 1

    * - 中断源
      - 类型
      - IRQ
    * - 内部定时器中断
      - 内部中断
      - 0
    * - EBREAK、ECALL 或非法指令
      - 内部中断
      - 1
    * - 非对齐内存访问
      - 内部中断
      - 2
    * - RTC 外设中断源
      - 外部中断
      - 31

可通过特殊的 32 位寄存器 Q0-Q3 和自定义的 R-type 指令启用中断处理。更多信息，请参阅 *{IDF_TARGET_NAME} 技术参考手册* > *超低功耗协处理器* > *ULP-RISC-V* > *ULP-RISC-V 中断* [`PDF <{IDF_TARGET_TRM_CN_URL}>`__]。

系统启动时，默认启用所有中断。触发中断时，处理器将跳转到 IRQ 向量。IRQ 向量随即保存寄存器上下文，并调用全局中断分发器。ULP RISC-V 驱动程序实现了一个 *弱* 中断分发器 :cpp:func:`_ulp_riscv_interrupt_handler`，充当处理所有中断的中心点。该全局分发器用于调用由 :cpp:func:`ulp_riscv_intr_alloc` 分配的相应中断处理程序。

ULP RISC-V 的中断处理尚在开发中，还不支持针对内部中断源的中断处理。目前支持两个 RTC 外设中断源，即软件触发的中断和 RTC IO 触发的中断，不支持嵌套中断。如果需要自定义中断处理，可以通过定义 :cpp:func:`_ulp_riscv_interrupt_handler` 来覆盖默认的全局中断调度器。

调试 ULP RISC-V 程序
----------------------------------

在对 ULP RISC-V 进行配置时，若程序未按预期运行，有时很难找出的原因。因为其内核的简单性，许多标准的调试方法如 JTAG 或 ``printf`` 无法使用。

以下方法可以调试 ULP RISC-V 程序：

 * 通过共享变量查看程序状态：如 :ref:`ulp-riscv-access-variables` 中所述，主 CPU 以及 ULP 内核都可以轻松访问 RTC 内存中的全局变量。通过 ULP 向该变量中写入状态信息，然后通过主 CPU 读取状态信息，有助于了解 ULP 内核的状态。该方法的缺点在于它要求主 CPU 一直处于唤醒状态，但现实情况可能并非如此。有时，保持主 CPU 处于唤醒状态还可能会掩盖一些问题，因为某些问题可能仅在特定电源域断电时才会出现。

 * 使用 bit-banged UART 驱动程序打印：ULP RISC-V 组件中有一个低速 bit-banged UART TX 驱动程序，可用于打印独立于主 CPU 状态的信息。有关如何使用此驱动程序的示例，请参阅 :example:`system/ulp/ulp_riscv/uart_print`。

 * 陷阱信号：ULP RISC-V 有一个硬件陷阱，将在特定条件下触发，例如非法指令。这将导致主 CPU 被 :cpp:enumerator:`ESP_SLEEP_WAKEUP_COCPU_TRAP_TRIG` 唤醒。

应用示例
--------------------

* 主 CPU 处于 Deep-sleep 状态时，ULP RISC-V 协处理器轮询 GPIO：:example:`system/ulp/ulp_riscv/gpio`。
* ULP RISC-V 协处理器使用 bit-banged UART 驱动程序打印：:example:`system/ulp/ulp_riscv/uart_print`.
* 主 CPU 处于 Deep-sleep 状态时，ULP RISC-V 协处理器读取外部温度传感器：:example:`system/ulp/ulp_riscv/ds18b20_onewire`。
* 主 CPU 处于 Deep-sleep 状态时，ULP RISC-V 协处理器读取外部 I2C 温度和湿度传感器 (BMP180)，达到阈值时唤醒主 CPU：:example:`system/ulp/ulp_riscv/i2c`.
* 使用 ULP RISC-V 协处理器处理软件中断和 RTC IO 中断：:example:`system/ulp/ulp_riscv/interrupts`.

API 参考
-------------

.. include-build-file:: inc/ulp_riscv.inc
.. include-build-file:: inc/ulp_riscv_lock_shared.inc
.. include-build-file:: inc/ulp_riscv_lock.inc
.. include-build-file:: inc/ulp_riscv_i2c.inc
