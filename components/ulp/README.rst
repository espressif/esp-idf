ULP coprocessor programming
===========================

.. warning:: ULP coprocessor programming approach described here is experimental. It is probable that once binutils support for ULP is done, this preprocessor-based approach may be deprecated. We welcome discussion about and contributions to ULP programming tools.

ULP coprocessor is a simple FSM which is designed to perform measurements using ADC, temperature sensor, and external I2C sensors, while main processors are in deep sleep mode. ULP coprocessor can access RTC_SLOW_MEM memory region, and registers in RTC_CNTL, RTC_IO, and SARADC peripherals. ULP coprocessor uses fixed-width 32-bit instructions, 32-bit memory addressing, and has 4 general purpose 16-bit registers.

ULP coprocessor doesn't have a dedicated binutils port yet. Programming ULP coprocessor is possible by embedding assembly-like macros into an ESP32 application.
Here is an example how this can be done::

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

The ``program`` array is an array of ``ulp_insn_t``, i.e. ULP coprocessor instructions. Each ``I_XXX`` preprocessor define translates into a single 32-bit instruction. Arguments of these preprocessor defines can be register numbers (``R0 — R3``) and literal constants. See `ULP coprocessor instruction defines`_ section for descriptions of instructions and arguments they take.

Load and store instructions use addresses expressed in 32-bit words. Address 0 corresponds to the first word of ``RTC_SLOW_MEM`` (which is address 0x50000000 as seen by the main CPUs).

To generate branch instructions, special ``M_`` preprocessor defines are used. ``M_LABEL`` define can be used to define a branch target. Label identifier is a 16-bit integer. ``M_Bxxx`` defines can be used to generate branch instructions with target set to a particular label. 

Implementation note: these ``M_`` preprocessor defines will be translated into two ``ulp_insn_t`` values: one is a token value which contains label number, and the other is the actual instruction. ``ulp_process_macros_and_load`` function resolves the label number to the address, modifies the branch instruction to use the correct address, and removes the the extra ``ulp_insn_t`` token which contains the label numer.

Here is an example of using labels and branches::

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


Functions
^^^^^^^^^

.. doxygenfunction:: ulp_process_macros_and_load
.. doxygenfunction:: ulp_run

Error codes
^^^^^^^^^^^

.. doxygendefine:: ESP_ERR_ULP_BASE
.. doxygendefine:: ESP_ERR_ULP_SIZE_TOO_BIG
.. doxygendefine:: ESP_ERR_ULP_INVALID_LOAD_ADDR
.. doxygendefine:: ESP_ERR_ULP_DUPLICATE_LABEL
.. doxygendefine:: ESP_ERR_ULP_UNDEFINED_LABEL
.. doxygendefine:: ESP_ERR_ULP_BRANCH_OUT_OF_RANGE

ULP coprocessor registers
^^^^^^^^^^^^^^^^^^^^^^^^^

ULP co-processor has 4 16-bit general purpose registers. All registers have same functionality, with one exception. R0 register is used by some of the compare-and-branch instructions as a source register.
 
These definitions can be used for all instructions which require a register.

.. doxygengroup:: ulp_registers
    :content-only:
    
ULP coprocessor instruction defines
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygendefine:: I_DELAY
.. doxygendefine:: I_HALT
.. doxygendefine:: I_END
.. doxygendefine:: I_ST
.. doxygendefine:: I_LD
.. doxygendefine:: I_WR_REG
.. doxygendefine:: I_RD_REG
.. doxygendefine:: I_BL
.. doxygendefine:: I_BGE
.. doxygendefine:: I_BXR
.. doxygendefine:: I_BXI
.. doxygendefine:: I_BXZR
.. doxygendefine:: I_BXZI
.. doxygendefine:: I_BXFR
.. doxygendefine:: I_BXFI
.. doxygendefine:: I_ADDR
.. doxygendefine:: I_SUBR
.. doxygendefine:: I_ANDR
.. doxygendefine:: I_ORR
.. doxygendefine:: I_MOVR
.. doxygendefine:: I_LSHR
.. doxygendefine:: I_RSHR
.. doxygendefine:: I_ADDI
.. doxygendefine:: I_SUBI
.. doxygendefine:: I_ANDI
.. doxygendefine:: I_ORI
.. doxygendefine:: I_MOVI
.. doxygendefine:: I_LSHI
.. doxygendefine:: I_RSHI
.. doxygendefine:: M_LABEL
.. doxygendefine:: M_BL
.. doxygendefine:: M_BGE
.. doxygendefine:: M_BX
.. doxygendefine:: M_BXZ
.. doxygendefine:: M_BXF

Defines
^^^^^^^

.. doxygendefine:: RTC_SLOW_MEM

