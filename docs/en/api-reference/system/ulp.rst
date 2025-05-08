Ultra Low Power (ULP) coprocessor
===================================

:link_to_translation:`zh_CN:[中文]`

The Ultra Low Power (ULP) coprocessor is designed to perform tasks while the main CPU is in deep sleep mode, allowing for power savings and extended battery life in applications. The ULP coprocessor can monitor sensors, control peripherals, and handle other tasks that do not require the main CPU's computing power. It can also wake up the main CPU when specific conditions are met, such as when a sensor reading exceeds a predefined threshold.

.. only:: not SOC_ULP_SUPPORTED

    .. note:: The ULP coprocessor is not supported on {IDF_TARGET_NAME}.

.. only:: SOC_ULP_SUPPORTED

    The ULP coprocessor on a chip may include one or more of the following types, but only one can operate at a time:

    - **ULP FSM**: A finite state machine (FSM) based ULP coprocessor. It is suitable for simple tasks, as applications for this type must be written using assembly language or C macros. This type is supported by ESP32, ESP32-S2, and ESP32-S3.
    - **ULP RISC-V**: A RISC-V based ULP coprocessor. It offers enhanced computational capabilities and flexibility, making it suitable for more complex applications written in C. This type is supported by ESP32-S2 and ESP32-S3.
    - **ULP LP Core**: A RISC-V based ULP coprocessor that combines the advantages of the **ULP RISC-V** type with additional features, such as extended memory access, broader peripheral access, a debug module, and an interrupt controller. This coprocessor is capable of operating even when the entire system is active. This type is supported by ESP32-C5, ESP32-C6, ESP32-P4, and upcoming chips.

    .. toctree::
        :maxdepth: 1

        :SOC_ULP_FSM_SUPPORTED: ulp-fsm
        :SOC_RISCV_COPROC_SUPPORTED: ulp-risc-v
        :SOC_LP_CORE_SUPPORTED: ulp-lp-core

    Code examples for this API section are provided in the :example:`system/ulp` directory of ESP-IDF examples.
