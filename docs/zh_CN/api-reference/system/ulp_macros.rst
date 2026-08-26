使用 C 语言宏编程 ULP FSM 协处理器（遗留）
===========================================

:link_to_translation:`en:[English]`

想要为 ULP FSM 协处理器生成程序，除了用 {IDF_TARGET_NAME} ULP 协处理器目前的 binutils 端口外，还可以将类似汇编的宏嵌入到 {IDF_TARGET_NAME} 应用程序中。以下是一个示例::

    const ulp_insn_t program[] = {
        I_MOVI(R3, 16),         // R3 <- 16
        I_LD(R0, R3, 0),        // R0 <- RTC_SLOW_MEM[R3 + 0]
        I_LD(R1, R3, 1),        // R1 <- RTC_SLOW_MEM[R3 + 1]
        I_ADDR(R2, R0, R1),     // R2 <- R0 + R1
        I_ST(R2, R3, 2),        // R2 -> RTC_SLOW_MEM[R2 + 2]
        I_HALT()
    };
    size_t load_addr = 0;
    size_t size = sizeof(program)/sizeof(ulp_insn_t);
    ulp_process_macros_and_load(load_addr, program, &size);
    ulp_run(load_addr);

``program`` 数组是一个 ``ulp_insn_t`` 数组，即 ULP 协处理器指令。每个 ``I_XXX`` 预处理宏都会转换为单个 32 位指令。这些预处理宏的参数可以是寄存器号 (``R0 — R3``) 和字面常量。有关指令及其参数的详细信息，请参阅本指南末尾的 API 参考部分。

.. note::

    由于一些宏指令会展开为内联函数调用，将这样的数组定义为全局数组会导致编译器报错："initializer element is not constant"。要修复此错误，请定义指令数组为局部数组。

.. note::

    加载指令 (LD)、存储指令 (ST) 和移动指令 (MOV) 均使用 **32 位字地址**。地址 0 对应 ``RTC_SLOW_MEM`` 的第一个字。
    这与汇编代码中相同指令的地址参数处理方式不同。详情请参阅 :ref:`ulp-fsm-addressing`。

要生成分支指令，使用特殊的 ``M_`` 预处理宏定义。 ``M_LABEL`` 宏定义可用于定义分支目标。标签标识符是一个 16 位整数。 ``M_Bxxx`` 宏定义可用于生成目标设置为特定标签的分支指令。

实现注意事项：这些 ``M_`` 预处理宏将被转换为两个 ulp_insn_t 值：一个是包含标签号的令牌值，另一个是实际指令。 ``ulp_process_macros_and_load`` 函数将标签号解析为地址，修改分支指令以使用正确的地址，并移除包含标签号的额外 ``ulp_insn_t`` 令牌。

以下是使用标签和分支的示例::

    const ulp_insn_t program[] = {
        I_MOVI(R0, 34),         // R0 <- 34
        M_LABEL(1),             // label_1
        I_MOVI(R1, 32),         // R1 <- 32
        I_LD(R1, R1, 0),        // R1 <- RTC_SLOW_MEM[R1]
        I_MOVI(R2, 33),         // R2 <- 33
        I_LD(R2, R2, 0),        // R2 <- RTC_SLOW_MEM[R2]
        I_SUBR(R3, R1, R2),     // R3 <- R1 - R2
        I_ST(R3, R0, 0),        // R3 -> RTC_SLOW_MEM[R0 + 0]
        I_ADDI(R0, R0, 1),      // R0++
        M_BL(1, 64),            // if (R0 < 64) goto label_1
        I_HALT(),
    };
    RTC_SLOW_MEM[32] = 42;
    RTC_SLOW_MEM[33] = 18;
    size_t load_addr = 0;
    size_t size = sizeof(program)/sizeof(ulp_insn_t);
    ulp_process_macros_and_load(load_addr, program, &size);
    ulp_run(load_addr);

API 参考
--------

.. include-build-file:: inc/ulp.inc
