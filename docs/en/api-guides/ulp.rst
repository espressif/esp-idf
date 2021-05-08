ULP Coprocessor programming
=============================

:link_to_translation:`zh_CN:[中文]`

.. toctree::
   :maxdepth: 1

   :esp32: Instruction set reference for ESP32 ULP <ulp_instruction_set>
   :esp32s2: Instruction set reference for ESP32-S2 ULP <ulps2_instruction_set>
   Programming using macros (legacy) <ulp_macros>


The ULP (Ultra Low Power) coprocessor is a simple FSM (Finite State Machine) which is designed to perform measurements using the ADC, temperature sensor, and external I2C sensors, while the main processors are in deep sleep mode. The ULP coprocessor can access the RTC_SLOW_MEM memory region, and registers in RTC_CNTL, RTC_IO, and SARADC peripherals. The ULP coprocessor uses fixed-width 32-bit instructions, 32-bit memory addressing, and has 4 general-purpose 16-bit registers.

Installing the Toolchain
------------------------

The ULP coprocessor code is written in assembly and compiled using the `binutils-esp32ulp toolchain`_.

If you have already set up ESP-IDF with CMake build system according to the :doc:`Getting Started Guide <../../get-started/index>`, then the ULP toolchain will already be installed.

.. only:: esp32

    If you are using ESP-IDF with the legacy GNU Make based build system, refer to the instructions on this page: :doc:`ulp-legacy`.

Compiling the ULP Code
-----------------------

To compile the ULP code as part of the component, the following steps must be taken:

1. The ULP code, written in assembly, must be added to one or more files with `.S` extension. These files must be placed into a separate directory inside the component directory, for instance `ulp/`.

.. note: When registering the component (via ``idf_component_register``), this directory should not be added to the ``SRC_DIRS`` argument. The logic behind this is that the ESP-IDF build system will compile files found in ``SRC_DIRS`` based on their extensions. For ``.S`` files, ``xtensa-{IDF_TARGET_NAME}-elf-as`` assembler is used. This is not desirable for ULP assembly files, so the easiest way to achieve the distinction is by placing ULP assembly files into a separate directory. The ULP assembly source files should also **not** be added to ``SRCS`` for the same reason. See the step below for how to properly add ULP assembly source files.

2. Call ``ulp_embed_binary`` from the component CMakeLists.txt after registration. For example::

    ...
    idf_component_register()

    set(ulp_app_name ulp_${COMPONENT_NAME})
    set(ulp_s_sources ulp/ulp_assembly_source_file.S)
    set(ulp_exp_dep_srcs "ulp_c_source_file.c")

    ulp_embed_binary(${ulp_app_name} "${ulp_s_sources}" "${ulp_exp_dep_srcs}")

 The first argument to ``ulp_embed_binary`` specifies the ULP binary name. The name specified here will also be used by other generated artifacts such as the ELF file, map file, header file and linker export file. The second argument specifies the ULP assembly source files. Finally, the third argument specifies the list of component source files which include the header file to be generated. This list is needed to build the dependencies correctly and ensure that the generated header file will be created before any of these files are compiled. See section below for the concept of generated header files for ULP applications.

3. Build the application as usual (e.g. `idf.py app`)

   Inside, the build system will take the following steps to build ULP program:

   1. **Run each assembly file (foo.S) through the C preprocessor.** This step generates the preprocessed assembly files (foo.ulp.S) in the component build directory. This step also generates dependency files (foo.ulp.d).

   2. **Run preprocessed assembly sources through the assembler.** This produces object (foo.ulp.o) and listing (foo.ulp.lst) files. Listing files are generated for debugging purposes and are not used at later stages of the build process.

   3. **Run the linker script template through the C preprocessor.** The template is located in ``components/ulp/ld`` directory.

   4. **Link the object files into an output ELF file** (``ulp_app_name.elf``). The Map file (``ulp_app_name.map``) generated at this stage may be useful for debugging purposes.

   5. **Dump the contents of the ELF file into a binary** (``ulp_app_name.bin``) which can then be embedded into the application.

   6. **Generate a list of global symbols** (``ulp_app_name.sym``) in the ELF file using ``esp32ulp-elf-nm``.

   7. **Create an LD export script and header file** (``ulp_app_name.ld`` and ``ulp_app_name.h``) containing the symbols from ``ulp_app_name.sym``. This is done using the ``esp32ulp_mapgen.py`` utility.

   8. **Add the generated binary to the list of binary files** to be embedded into the application.

Accessing the ULP Program Variables
-------------------------------------

Global symbols defined in the ULP program may be used inside the main program.

For example, the ULP program may define a variable ``measurement_count`` which will define the number of ADC measurements the program needs to make before waking up the chip from deep sleep::

                            .global measurement_count
    measurement_count:      .long 0

                            /* later, use measurement_count */
                            move r3, measurement_count
                            ld r3, r3, 0

The main program needs to initialize this variable before the ULP program is started. The build system makes this possible by generating the ``${ULP_APP_NAME}.h`` and ``${ULP_APP_NAME}.ld`` files which define the global symbols present in the ULP program. Each global symbol defined in the ULP program is included in these files and are prefixed with ``ulp_``.

The header file contains the declaration of the symbol::

    extern uint32_t ulp_measurement_count;

Note that all symbols (variables, arrays, functions) are declared as ``uint32_t``. For functions and arrays, take the address of the symbol and cast it to the appropriate type.

The generated linker script file defines the locations of symbols in RTC_SLOW_MEM::

    PROVIDE ( ulp_measurement_count = 0x50000060 );

To access the ULP program variables from the main program, the generated header file should be included using an ``include`` statement. This will allow the ULP program variables to be accessed as regular variables::

    #include "ulp_app_name.h"

    // later
    void init_ulp_vars() {
        ulp_measurement_count = 64;
    }

Note that the ULP program can only use lower 16 bits of each 32-bit word in RTC memory, because the registers are 16-bit, and there is no instruction to load from the high part of the word.

Likewise, the ULP store instruction writes register value into the lower 16 bits part of the 32-bit word. The upper 16 bits are written with a value which depends on the address of the store instruction, thus when reading variables written by the ULP, the main application needs to mask the upper 16 bits, e.g.::

    printf("Last measurement value: %d\n", ulp_last_measurement & UINT16_MAX);

Starting the ULP Program
------------------------

To run a ULP program, the main application needs to load the ULP program into RTC memory using the ``ulp_load_binary`` function, and then start it using the ``ulp_run`` function.

Note that "Enable Ultra Low Power (ULP) Coprocessor" option must be enabled in menuconfig to reserve memory for the ULP. "RTC slow memory reserved for coprocessor" option must be set to a value sufficient to store ULP code and data. If the application components contain multiple ULP programs, then the size of the RTC memory must be sufficient to hold the largest one.

Each ULP program is embedded into the ESP-IDF application as a binary blob. The application can reference this blob and load it in the following way (suppose ULP_APP_NAME was defined to ``ulp_app_name``)::

    extern const uint8_t bin_start[] asm("_binary_ulp_app_name_bin_start");
    extern const uint8_t bin_end[]   asm("_binary_ulp_app_name_bin_end");

    void start_ulp_program() {
        ESP_ERROR_CHECK( ulp_load_binary(
            0 /* load address, set to 0 when using default linker scripts */,
            bin_start,
            (bin_end - bin_start) / sizeof(uint32_t)) );
    }

.. doxygenfunction:: ulp_load_binary

Once the program is loaded into RTC memory, the application can start it, passing the address of the entry point to the ``ulp_run`` function::

    ESP_ERROR_CHECK( ulp_run(&ulp_entry - RTC_SLOW_MEM) );

.. doxygenfunction:: ulp_run

Declaration of the entry point symbol comes from the generated header file mentioned above, ``${ULP_APP_NAME}.h``. In the assembly source of the ULP application, this symbol must be marked as ``.global``::


            .global entry
    entry:
            /* code starts here */


ULP Program Flow
----------------

The ULP coprocessor is started by a timer. The timer is started once ``ulp_run`` is called. The timer counts a number of RTC_SLOW_CLK ticks (by default, produced by an internal 150 kHz RC oscillator). The number of ticks is set using ``SENS_ULP_CP_SLEEP_CYCx_REG`` registers (x = 0..4). When starting the ULP for the first time, ``SENS_ULP_CP_SLEEP_CYC0_REG`` will be used to set the number of timer ticks. Later the ULP program can select another ``SENS_ULP_CP_SLEEP_CYCx_REG`` register using the ``sleep`` instruction.

The application can set ULP timer period values (SENS_ULP_CP_SLEEP_CYCx_REG, x = 0..4) using the ``ulp_set_wakeup_period`` function.

.. doxygenfunction:: ulp_set_wakeup_period

Once the timer counts the number of ticks set in the selected ``SENS_ULP_CP_SLEEP_CYCx_REG`` register, the ULP coprocessor will power up and start running the program from the entry point set in the call to ``ulp_run``.

The program runs until it encounters a ``halt`` instruction or an illegal instruction. Once the program halts, the ULP coprocessor will power down, and the timer will be started again.

To disable the timer (effectively preventing the ULP program from running again), please clear the ``RTC_CNTL_ULP_CP_SLP_TIMER_EN`` bit in the ``RTC_CNTL_STATE0_REG`` register. This can be done both from the ULP code and from the main program.


.. _binutils-esp32ulp toolchain: https://github.com/espressif/binutils-esp32ulp
