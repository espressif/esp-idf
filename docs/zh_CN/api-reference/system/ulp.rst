ULP 协处理器
====================

:link_to_translation:`en:[English]`

ULP（Ultra Low Power，超低功耗）协处理器可在主 CPU 处于深度睡眠模式时执行任务，从而实现功耗优化并延长电池使用寿命。ULP 协处理器能监测传感器、控制外设，并处理其他无需主 CPU 算力的任务。当满足特定条件（如传感器读数超过预设阈值）时，它还能唤醒主 CPU。

.. only:: not SOC_ULP_SUPPORTED

    .. note:: {IDF_TARGET_NAME} 不支持 ULP 协处理器。

.. only:: SOC_ULP_SUPPORTED

    芯片上的 ULP 协处理器可能包含以下一种或多种类型，但同一时间只能运行一种：

    - **ULP FSM**：基于有限状态机 (FSM) 的协处理器。适用于简单任务，相关应用需使用汇编语言或 C 语言宏编写。ESP32、ESP32-S2 和 ESP32-S3 支持此类型。
    - **ULP RISC-V**：基于 RISC-V 架构的协处理器。提供更强的计算能力和灵活性，适合运行用 C 语言编写的复杂应用。ESP32-S2 和 ESP32-S3 支持此类型。
    - **ULP LP Core**：基于 RISC-V 架构的增强型协处理器，兼具 **ULP RISC-V** 的优势，并新增扩展内存访问、更广泛的外设访问、调试模块和中断控制器等功能。该处理器甚至可在全系统运行时工作。ESP32-C5、ESP32-C6、ESP32-P4 及后续芯片支持此类型。

    .. toctree::
        :maxdepth: 1

        :SOC_ULP_FSM_SUPPORTED: ulp-fsm
        :SOC_RISCV_COPROC_SUPPORTED: ulp-risc-v
        :SOC_LP_CORE_SUPPORTED: ulp-lp-core

    本 API 章节的代码示例位于 ESP-IDF 示例项目的 :example:`system/ulp` 目录下。
