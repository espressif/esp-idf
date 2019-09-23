ULP coprocessor programming
===================================

:link_to_translation:`zh_CN:[中文]`

.. toctree::
   :maxdepth: 1

   Instruction set reference <ulp_instruction_set>
   Programming using macros (legacy) <ulp_macros>


ULP (Ultra Low Power) coprocessor is a simple FSM which is designed to perform measurements using ADC, temperature sensor, and external I2C sensors, while main processors are in deep sleep mode. ULP coprocessor can access RTC_SLOW_MEM memory region, and registers in RTC_CNTL, RTC_IO, and SARADC peripherals. ULP coprocessor uses fixed-width 32-bit instructions, 32-bit memory addressing, and has 4 general purpose 16-bit registers.

Installing the toolchain
------------------------

ULP coprocessor code is written in assembly and compiled using the `binutils-esp32ulp toolchain`_.

If you have already set up ESP-IDF with CMake build system according to the :doc:`Getting Started Guide <../../get-started/index>`, then the ULP toolchain is already installed.

If you are using ESP-IDF with the legacy GNU Make based build system, refer to the instructions on this page: :doc:`ulp-legacy`.

Compiling ULP code
------------------

To compile ULP code as part of a component, the following steps must be taken:

1. ULP code, written in assembly, must be added to one or more files with `.S` extension. These files must be placed into a separate directory inside component directory, for instance `ulp/`.

.. note: This directory should not be added to the ``COMPONENT_SRCDIRS`` environment variable. The logic behind this is that the ESP-IDF build system will compile files found in ``COMPONENT_SRCDIRS`` based on their extensions. For ``.S`` files, ``xtensa-esp32-elf-as`` assembler is used. This is not desirable for ULP assembly files, so the easiest way to achieve the distinction is by placing ULP assembly files into a separate directory. The ULP assembly source files should also **not** be added to ``COMPONENT_SRCS`` for the same reason. See the step below for how to properly add ULP assembly source files.

2. Call ``ulp_embed_binary`` from the component CMakeLists.txt after registration. For example::

    ...
    register_component()

    set(ulp_app_name ulp_${COMPONENT_NAME})
    set(ulp_s_sources ulp/ulp_assembly_source_file.S)
    set(ulp_exp_dep_srcs "ulp_c_source_file.c")

    ulp_embed_binary(${ulp_app_name} ${ulp_s_sources} ${ulp_exp_dep_srcs})

 The first argument to ``ulp_embed_binary`` specifies the ULP binary name. The name specified here will also be used other generated artifacts 
 such as the ELF file, map file, header file and linker export file. The second argument specifies the ULP assembly source files. 
 Finally, the third argument specifies the list of component source files which include the header file to be generated. 
 This list is needed to build the dependencies correctly and ensure that the generated header file is created before any of these files are compiled. 
 See section below explaining the concept of generated header files for ULP applications. 

3. Build the application as usual (e.g. `idf.py app`)

   Inside, the build system will take the following steps to build ULP program:

   1. **Run each assembly file (foo.S) through C preprocessor.** This step generates the preprocessed assembly files (foo.ulp.S) in the component build directory. This step also generates dependency files (foo.ulp.d).

   2. **Run preprocessed assembly sources through assembler.** This produces objects (foo.ulp.o) and listing (foo.ulp.lst) files. Listing files are generated for debugging purposes and are not used at later stages of build process.

   3. **Run linker script template through C preprocessor.** The template is located in components/ulp/ld directory.

   4. **Link object files into an output ELF file** (ulp_app_name.elf). Map file (ulp_app_name.map) generated at this stage may be useful for debugging purposes.

   5. **Dump contents of the ELF file into binary** (ulp_app_name.bin) for embedding into the application.

   6. **Generate list of global symbols** (ulp_app_name.sym) in the ELF file using esp32ulp-elf-nm.

   7. **Create LD export script and header file** (ulp_app_name.ld and ulp_app_name.h) containing the symbols from ulp_app_name.sym. This is done using esp32ulp_mapgen.py utility.

   8. **Add the generated binary to the list of binary files** to be emedded into the application.

Accessing ULP program variables
-------------------------------

Global symbols defined in the ULP program may be used inside the main program.

For example, ULP program may define a variable ``measurement_count`` which will define the number of ADC measurements the program needs to make before waking up the chip from deep sleep::

                            .global measurement_count
    measurement_count:      .long 0

                            /* later, use measurement_count */
                            move r3, measurement_count
                            ld r3, r3, 0

Main program needs to initialize this variable before ULP program is started. Build system makes this possible by generating a ``${ULP_APP_NAME}.h`` and ``${ULP_APP_NAME}.ld`` files which define global symbols present in the ULP program. This files include each global symbol defined in the ULP program, prefixed with ``ulp_``.

The header file contains declaration of the symbol::

    extern uint32_t ulp_measurement_count;

Note that all symbols (variables, arrays, functions) are declared as ``uint32_t``. For functions and arrays, take address of the symbol and cast to the appropriate type.

The generated linker script file defines locations of symbols in RTC_SLOW_MEM::

    PROVIDE ( ulp_measurement_count = 0x50000060 );

To access ULP program variables from the main program, include the generated header file and use variables as one normally would::

    #include "ulp_app_name.h"

    // later
    void init_ulp_vars() {
        ulp_measurement_count = 64;
    }

Note that ULP program can only use lower 16 bits of each 32-bit word in RTC memory, because the registers are 16-bit, and there is no instruction to load from high part of the word.

Likewise, ULP store instruction writes register value into the lower 16 bit part of the 32-bit word. Upper 16 bits are written with a value which depends on the address of the store instruction, so when reading variables written by the ULP, main application needs to mask upper 16 bits, e.g.::

    printf("Last measurement value: %d\n", ulp_last_measurement & UINT16_MAX);

Starting the ULP program
------------------------

To run a ULP program, main application needs to load the ULP program into RTC memory using ``ulp_load_binary`` function, and then start it using ``ulp_run`` function.

Note that "Enable Ultra Low Power (ULP) Coprocessor" option must be enabled in menuconfig in order to reserve memory for the ULP. "RTC slow memory reserved for coprocessor" option must be set to a value sufficient to store ULP code and data. If the application components contain multiple ULP programs, then the size of the RTC memory must be sufficient to hold the largest one.

Each ULP program is embedded into the ESP-IDF application as a binary blob. Application can reference this blob and load it in the following way (suppose ULP_APP_NAME was defined to ``ulp_app_name``::

    extern const uint8_t bin_start[] asm("_binary_ulp_app_name_bin_start");
    extern const uint8_t bin_end[]   asm("_binary_ulp_app_name_bin_end");

    void start_ulp_program() {
        ESP_ERROR_CHECK( ulp_load_binary(
            0 /* load address, set to 0 when using default linker scripts */,
            bin_start,
            (bin_end - bin_start) / sizeof(uint32_t)) );
    }

.. doxygenfunction:: ulp_load_binary

Once the program is loaded into RTC memory, application can start it, passing the address of the entry point to ``ulp_run`` function::

    ESP_ERROR_CHECK( ulp_run(&ulp_entry - RTC_SLOW_MEM) );

.. doxygenfunction:: ulp_run

Declaration of the entry point symbol comes from the above mentioned generated header file, ``${ULP_APP_NAME}.h``. In assembly source of the ULP application, this symbol must be marked as ``.global``::


            .global entry
    entry:
            /* code starts here */


ULP program flow
----------------

ULP coprocessor is started by a timer. The timer is started once ``ulp_run`` is called. The timer counts a number of RTC_SLOW_CLK ticks (by default, produced by an internal 150kHz RC oscillator). The number of ticks is set using ``SENS_ULP_CP_SLEEP_CYCx_REG`` registers (x = 0..4). When starting the ULP for the first time, ``SENS_ULP_CP_SLEEP_CYC0_REG`` will be used to set the number of timer ticks. Later the ULP program can select another ``SENS_ULP_CP_SLEEP_CYCx_REG`` register using ``sleep`` instruction.

The application can set ULP timer period values (SENS_ULP_CP_SLEEP_CYCx_REG, x = 0..4) using ``ulp_set_wakeup_period`` function.

.. doxygenfunction:: ulp_set_wakeup_period

Once the timer counts the number of ticks set in the selected ``SENS_ULP_CP_SLEEP_CYCx_REG`` register, ULP coprocessor powers up and starts running the program from the entry point set in the call to ``ulp_run``.

The program runs until it encounters a ``halt`` instruction or an illegal instruction. Once the program halts, ULP coprocessor powers down, and the timer is started again.

To disable the timer (effectively preventing the ULP program from running again), clear the ``RTC_CNTL_ULP_CP_SLP_TIMER_EN`` bit in the ``RTC_CNTL_STATE0_REG`` register. This can be done both from ULP code and from the main program.


.. _binutils-esp32ulp toolchain: https://github.com/espressif/binutils-esp32ulp
