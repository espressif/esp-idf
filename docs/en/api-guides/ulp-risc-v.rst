ULP-RISC-V Coprocessor programming
==================================
:link_to_translation:`zh_CN:[中文]`

.. toctree::
   :maxdepth: 1


The ULP-RISC-V coprocessor is a variant of the ULP, present in ESP32-S2. Similar to ULP, ULP RISC-V coprocessor can perform tasks such as sensor readings while the main CPU stays in low power modes. The main difference from the FSM ULP is this variant can be programmed in C using standard GNU tools. The ULP-RISC-V coprocessor can access the RTC_SLOW_MEM memory region, and registers in RTC_CNTL, RTC_IO, and SARADC peripherals. The RISC-V processor is a 32-bit, fixed point machine. Its instruction set is based on RV32IMC which includes hardware multiplication and division, and compressed code.  

Installing the ULP-RISC-V Toolchain
-----------------------------------

The ULP-RISC-V coprocessor code is written in C (assembly is also possible) and compiled using RISC-V toolchain based on GCC.

If you have already set up ESP-IDF with CMake build system according to the :doc:`Getting Started Guide <../../get-started/index>`, then the toolchain should already be installed.

.. note: In earlier versions of ESP-IDF, RISC-V toolchain had a different prefix: `riscv-none-embed-gcc`.

Compiling the ULP-RISC-V Code
-----------------------------

To compile the ULP-RISC-V code as part of the component, the following steps must be taken:

1. The ULP-RISC-V code, written in C or assembly (must use the `.S` extension), must be placed into a separate directory inside the component directory, for instance `ulp/`.

.. note: When registering the component (via ``idf_component_register``), this directory should not be added to the ``SRC_DIRS`` argument as it is currently done for the FSM ULP. See the step below for how to properly add ULP source files

2. Call ``ulp_embed_binary`` from the component CMakeLists.txt after registration. For example::

    ...
    idf_component_register()

    set(ulp_app_name ulp_${COMPONENT_NAME})
    set(ulp_sources "ulp/ulp_c_source_file.c" "ulp/ulp_assembly_source_file.S")
    set(ulp_exp_dep_srcs "ulp_c_source_file.c")

    ulp_embed_binary(${ulp_app_name} "${ulp_sources}" "${ulp_exp_dep_srcs}")

 The first argument to ``ulp_embed_binary`` specifies the ULP binary name. The name specified here will also be used by other generated artifacts
 such as the ELF file, map file, header file and linker export file. The second argument specifies the ULP source files.
 Finally, the third argument specifies the list of component source files which include the header file to be generated.
 This list is needed to build the dependencies correctly and ensure that the generated header file will be created before any of these files are compiled.
 See section below for the concept of generated header files for ULP applications.

3. Build the application as usual (e.g. `idf.py app`)

   Inside, the build system will take the following steps to build ULP program:

   1. **Run each source file through the C compiler and assembler.** This step generates the object files (.obj.c or .obj.S depending of source file processed) in the component build directory.

   2. **Run the linker script template through the C preprocessor.** The template is located in ``components/ulp/ld`` directory.

   4. **Link the object files into an output ELF file** (``ulp_app_name.elf``). The Map file (``ulp_app_name.map``) generated at this stage may be useful for debugging purposes.

   5. **Dump the contents of the ELF file into a binary** (``ulp_app_name.bin``) which can then be embedded into the application.

   6. **Generate a list of global symbols** (``ulp_app_name.sym``) in the ELF file using ``riscv32-esp-elf-nm``.

   7. **Create an LD export script and header file** (``ulp_app_name.ld`` and ``ulp_app_name.h``) containing the symbols from ``ulp_app_name.sym``. This is done using the ``esp32ulp_mapgen.py`` utility.

   8. **Add the generated binary to the list of binary files** to be embedded into the application.

Accessing the ULP-RISC-V Program Variables
------------------------------------------

Global symbols defined in the ULP-RISC-V program may be used inside the main program.

For example, the ULP-RISC-V program may define a variable ``measurement_count`` which will define the number of ADC measurements the program needs to make before waking up the chip from deep sleep

.. code-block:: c

    volatile int measurement_count; 

    int some_function()
    {
        //read the measurement count for use it later.
        int temp = measurement_count;

        ...do something.
    }

The main program can access the global ULP-RISC-V program variables, the build system makes this possible by generating the ``${ULP_APP_NAME}.h`` and ``${ULP_APP_NAME}.ld`` files which define the global symbols present in the ULP program. Each global symbol defined in the ULP program is included in these files and are prefixed with ``ulp_``.

The header file contains the declaration of the symbol

.. code-block:: c

    extern uint32_t ulp_measurement_count;

Note that all symbols (variables, arrays, functions) are declared as ``uint32_t``. For functions and arrays, take the address of the symbol and cast it to the appropriate type.

The generated linker script file defines the locations of symbols in RTC_SLOW_MEM::

    PROVIDE ( ulp_measurement_count = 0x50000060 );

To access the ULP-RISC-V program variables from the main program, the generated header file should be included using an ``include`` statement. This will allow the ULP program variables to be accessed as regular variables

.. code-block:: c

    #include "ulp_app_name.h"

    void init_ulp_vars() {
        ulp_measurement_count = 64;
    }

Starting the ULP-RISC-V Program
-------------------------------

To run a ULP-RISC-V program, the main application needs to load the ULP program into RTC memory using the :cpp:func:`ulp_riscv_load_binary` function, and then start it using the :cpp:func:`ulp_riscv_run` function.

Note that `CONFIG_ESP32S2_ULP_COPROC_ENABLED` and `CONFIG_ESP32S2_ULP_COPROC_RISCV` options must be enabled in menuconfig to reserve memory for the ULP. "RTC slow memory reserved for coprocessor" option must be set to a value sufficient to store ULP code and data. If the application components contain multiple ULP programs, then the size of the RTC memory must be sufficient to hold the largest one.

Each ULP-RISC-V program is embedded into the ESP-IDF application as a binary blob. The application can reference this blob and load it in the following way (suppose ULP_APP_NAME was defined to ``ulp_app_name``)

.. code-block:: c

    extern const uint8_t bin_start[] asm("_binary_ulp_app_name_bin_start");
    extern const uint8_t bin_end[]   asm("_binary_ulp_app_name_bin_end");

    void start_ulp_program() {
        ESP_ERROR_CHECK( ulp_riscv_load_binary( bin_start,
            (bin_end - bin_start)) );
    }

.. doxygenfunction:: ulp_riscv_load_binary()

Once the program is loaded into RTC memory, the application can start it, calling the :cpp:func:`ulp_riscv_run` function

.. code-block:: c

    ESP_ERROR_CHECK( ulp_riscv_run() );

.. doxygenfunction:: ulp_riscv_run()

ULP-RISC-V Program Flow
-----------------------

{IDF_TARGET_RTC_CLK_FRE:default="150kHz", esp32s2="90kHz"}

The ULP-RISC-V coprocessor is started by a timer. The timer is started once :cpp:func:`ulp_riscv_run` is called. The timer counts the number of RTC_SLOW_CLK ticks (by default, produced by an internal {IDF_TARGET_RTC_CLK_FRE} RC oscillator). The number of ticks is set using ``RTC_CNTL_ULP_CP_TIMER_1_REG`` register. When starting the ULP, ``RTC_CNTL_ULP_CP_TIMER_1_REG`` will be used to set the number of timer ticks.

The application can set ULP timer period values (RTC_CNTL_ULP_CP_TIMER_1_REG) using the :cpp:func:`ulp_set_wakeup_period` function.

Once the timer counts the number of ticks set in the ``RTC_CNTL_ULP_CP_TIMER_1_REG`` register, the ULP coprocessor will power up and start running the program from the entry point set in the call to :cpp:func:`ulp_riscv_run`.

The program runs until the field ``RTC_CNTL_COCPU_DONE`` in register ``RTC_CNTL_COCPU_CTRL_REG`` gets written or when a trap occurs due to illegal processor state. Once the program halts, the ULP coprocessor will power down, and the timer will be started again.

To disable the timer (effectively preventing the ULP program from running again), please clear the ``RTC_CNTL_ULP_CP_SLP_TIMER_EN`` bit in the ``RTC_CNTL_STATE0_REG`` register. This can be done both from the ULP code and from the main program.

