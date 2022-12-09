The ULP Coprocessor (Legacy GNU Make)
======================================

:link_to_translation:`zh_CN:[中文]`

.. toctree::
   :maxdepth: 1

   Instruction set reference <ulp_instruction_set>
   Programming using macros (legacy) <ulp_macros>

.. include:: ../gnu-make-legacy.rst

The ULP (Ultra Low Power) coprocessor is a simple FSM (Finite State Machine) which is designed to perform measurements using the ADC, temperature sensor, and external I2C sensors, while the main processors are in deep sleep mode. The ULP coprocessor can access the RTC_SLOW_MEM memory region, and registers in RTC_CNTL, RTC_IO, and SARADC peripherals. The ULP coprocessor uses fixed-width 32-bit instructions, 32-bit memory addressing, and has 4 general-purpose 16-bit registers.

Installing the Toolchain
------------------------

The ULP coprocessor code is written in assembly and compiled using the `binutils-esp32ulp toolchain`.

1. Download pre-built binaries of the latest toolchain release from:
https://github.com/espressif/binutils-esp32ulp/releases.

2. Extract the toolchain into a directory, and add the path to the ``bin/`` directory of the toolchain to the ``PATH`` environment variable.

Compiling the ULP Code
------------------------

To compile the ULP code as part of the component, the following steps must be taken:

1. The ULP code, written in assembly, must be added to one or more files with `.S` extension. These files must be placed into a separate directory inside the component directory, for instance `ulp/`. 

.. note: This directory should not be added to the ``COMPONENT_SRCDIRS`` environment variable. The logic behind this is that the ESP-IDF build system will compile files found in ``COMPONENT_SRCDIRS`` based on their extensions. For ``.S`` files, ``xtensa-esp32-elf-as`` assembler is used. This is not desirable for ULP assembly files, thus the easiest way to achieve this distinction is by placing ULP assembly files into a separate directory.

2. Modify the component makefile, adding the following::

    ULP_APP_NAME ?= ulp_$(COMPONENT_NAME)
    ULP_S_SOURCES = $(COMPONENT_PATH)/ulp/ulp_source_file.S
    ULP_EXP_DEP_OBJECTS := main.o
    include $(IDF_PATH)/components/ulp/component_ulp_common.mk
   
   Here is each line explained:

   ULP_APP_NAME
    Name of the generated ULP application, without an extension. This name is used for build products of the ULP application: ELF file, map file, binary file, generated header file, and generated linker export file.

   ULP_S_SOURCES
    List of assembly files to be passed to the ULP assembler. These must be absolute paths, i.e. start with ``$(COMPONENT_PATH)``. Consider using ``$(addprefix)`` function if more than one file needs to be listed. Paths are relative to component build directory, so prefixing them is not necessary.

   ULP_EXP_DEP_OBJECTS
    List of object files names within the component which include the generated header file. This list is needed to build the dependencies correctly and ensure that the generated header file is created before any of these files are compiled. See section below explaining the concept of generated header files for ULP applications.

   include $(IDF_PATH)/components/ulp/component_ulp_common.mk
    Includes common definitions of ULP build steps. Defines build targets for ULP object files, ELF file, binary file, etc.

3. Build the application as usual (e.g. ``idf.py build`` or ``idf.py app``)
   
  Inside, the build system will take the following steps to build ULP program:

  1. **Run each assembly file** (``foo.S``) **through the C preprocessor.** This step generates the preprocessed assembly files (``foo.ulp.pS``) in the component build directory. This step also generates dependency files (``foo.ulp.d``).

  2. **Run the preprocessed assembly sources through the assembler.** This produces object (``foo.ulp.o``) and listing (``foo.ulp.lst``) files. Listing files are generated for debugging purposes and are not used at later stages of the build process.
 
  3. **Run the linker script template through the C preprocessor.** The template is located in ``components/ulp/ld`` directory.

  4. **Link the object files into an output ELF file** (``ulp_app_name.elf``). The Map file (``ulp_app_name.map``) generated at this stage may be useful for debugging purposes.

  5. **Dump the contents of the ELF file into a binary** (``ulp_app_name.bin``) which can then be embedded into the application.

  6. **Generate a list of global symbols** (``ulp_app_name.sym``) in the ELF file using ``esp32ulp-elf-nm``.

  7. **Create an LD export script and header file** (``ulp_app_name.ld`` and ``ulp_app_name.h``) containing the symbols from ``ulp_app_name.sym``. This is done using the ``esp32ulp_mapgen.py`` utility.

  8. **Add the generated binary to the list of binary files** to be embedded into the application.

Accessing the ULP Program Variables
------------------------------------

Global symbols defined in the ULP program may be used inside the main program. 

For example, the ULP program may define a variable ``measurement_count`` which will define the number of the ADC measurements the program needs to make before waking up the chip from deep sleep::

                            .global measurement_count
    measurement_count:      .long 0

                            /* later, use measurement_count */
                            move r3, measurement_count
                            ld r3, r3, 0

The main program needs to initialize this variable before the ULP program is started. The build system makes this possible by generating ``$(ULP_APP_NAME).h`` and ``$(ULP_APP_NAME).ld`` files which define global symbols present in the ULP program. Each global symbol defined in the ULP program is included in these files and are prefixed with ``ulp_``.

The header file contains the declaration of the symbol::
    
    extern uint32_t ulp_measurement_count;

Note that all symbols (variables, arrays, functions) are declared as ``uint32_t``. For functions and arrays, take the address of the symbol and cast it to the appropriate type.

The generated linker script file defines locations of symbols in RTC_SLOW_MEM::

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

Declaration of the entry point symbol comes from the generated header file mentioned above, ``$(ULP_APP_NAME).h``. In the assembly source of the ULP application, this symbol must be marked as ``.global``::

            .global entry
    entry:
            /* code starts here */

ULP Program Flow
----------------

The ULP coprocessor is started by a timer. The timer is started once ``ulp_run`` is called. The timer counts the number of RTC_SLOW_CLK ticks (by default, produced by an internal 150 kHz RC oscillator). The number of ticks is set using ``SENS_ULP_CP_SLEEP_CYCx_REG`` registers (x = 0..4). When starting the ULP for the first time, ``SENS_ULP_CP_SLEEP_CYC0_REG`` will be used to set the number of timer ticks. Later the ULP program can select another ``SENS_ULP_CP_SLEEP_CYCx_REG`` register using the ``sleep`` instruction.

The application can set ULP timer period values (SENS_ULP_CP_SLEEP_CYCx_REG, x = 0..4) using the ``ulp_set_wakeup_period`` function.

.. doxygenfunction:: ulp_set_wakeup_period

Once the timer counts the number of ticks set in the selected ``SENS_ULP_CP_SLEEP_CYCx_REG`` register, the ULP coprocessor will power up and start running the program from the entry point set in the call to ``ulp_run``.

The program runs until it encounters a ``halt`` instruction or an illegal instruction. Once the program halts, ULP coprocessor will power down, and the timer will be started again.

To disable the timer (effectively preventing the ULP program from running again), please clear the ``RTC_CNTL_ULP_CP_SLP_TIMER_EN`` bit in the ``RTC_CNTL_STATE0_REG`` register. This can be done both from the ULP code and from the main program.


.. _binutils-esp32ulp toolchain: https://github.com/espressif/binutils-esp32ulp
