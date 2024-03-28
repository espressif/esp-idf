Programming ULP FSM Coprocessor Using C Macros (Legacy)
=======================================================

In addition to the existing binutils port for the {IDF_TARGET_NAME} ULP coprocessor, it is possible to generate programs for the ULP FSM coprocessor by embedding assembly-like macros into an {IDF_TARGET_NAME} application. Here is an example how this can be done::

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

The ``program`` array is an array of ``ulp_insn_t``, i.e., ULP coprocessor instructions. Each ``I_XXX`` preprocessor define translates into a single 32-bit instruction. Arguments of these preprocessor defines can be register numbers (``R0 — R3``) and literal constants. See the API reference section at the end of this guide for descriptions of instructions and arguments they take.

.. note::

    Because some of the instruction macros expand to inline function calls, defining such array in global scope will cause the compiler to produce an "initializer element is not constant" error. To fix this error, move the definition of instructions array into local scope.

.. note::

    Load, store and move instructions use **addresses expressed in 32-bit words**. Address 0 corresponds to the first word of ``RTC_SLOW_MEM``.
    This is different to how address arguments are handled in assembly code of the same instructions. See the section :ref:`ulp-fsm-addressing` for more details for reference.

To generate branch instructions, special ``M_`` preprocessor defines are used. ``M_LABEL`` define can be used to define a branch target. Label identifier is a 16-bit integer. ``M_Bxxx`` defines can be used to generate branch instructions with target set to a particular label.

Implementation note: these ``M_`` preprocessor defines will be translated into two ulp_insn_t values: one is a token value which contains label number, and the other is the actual instruction. ``ulp_process_macros_and_load`` function resolves the label number to the address, modifies the branch instruction to use the correct address, and removes the extra ``ulp_insn_t`` token which contains the label number.

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

API Reference
-------------

.. include-build-file:: inc/ulp.inc
