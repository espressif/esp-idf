ULP LP-Core Coprocessor Programming
===================================
:link_to_translation:`zh_CN:[中文]`

The ULP LP-Core (Low-power core) coprocessor is a variant of the ULP present in {IDF_TARGET_NAME}. It features ultra-low power consumption while also being able to stay powered on while the main CPU stays in low-power modes. This enables the LP-Core coprocessor to handle tasks like GPIO or sensor readings while the main CPU is in sleep mode, resulting in significant overall power savings for the entire system.

The ULP LP-Core coprocessor has the following features:

* Utilizes a 32-bit processor based on the RISC-V ISA, encompassing the standard extensions integer (I), multiplication/division (M), atomic (A), and compressed (C).
* Interrupt controller
* Includes a debug module that supports external debugging via JTAG.
* Can access all of the High-power (HP) SRAM and peripherals when the entire system is active.
* Can access the Low-power (LP) SRAM and peripherals when the HP system is in sleep mode.


Compiling Code for the ULP LP-Core
----------------------------------

The ULP LP-Core code is compiled together with your IDF project as a separate binary and automatically embedded into the main project binary. To acheive this do the following:

1. Place the ULP LP-Core code, written in C or assembly (with the ``.S`` extension), in a dedicated directory within the component directory, such as ``ulp/``.

2. After registering the component in the CMakeLists.txt file, call the ``ulp_embed_binary`` function. Here's an example:

    idf_component_register()

    set(ulp_app_name ulp_${COMPONENT_NAME})
    set(ulp_sources "ulp/ulp_c_source_file.c" "ulp/ulp_assembly_source_file.S")
    set(ulp_exp_dep_srcs "ulp_c_source_file.c")

    ulp_embed_binary(${ulp_app_name} "${ulp_sources}" "${ulp_exp_dep_srcs}")


 The first argument to ``ulp_embed_binary`` specifies the ULP binary name. The name specified here will also be used by other generated artifacts such as the ELF file, map file, header file, and linker export file. The second argument specifies the ULP source files. Finally, the third argument specifies the list of component source files which include the header file to be generated. This list is needed to build the dependencies correctly and ensure that the generated header file will be created before any of these files are compiled. See the section below for the concept of generated header files for ULP applications.

3. Enable both :ref:`CONFIG_ULP_COPROC_ENABLED` and :ref:`CONFIG_ULP_COPROC_TYPE` to ``CONFIG_ULP_COPROC_TYPE_LP_CORE`` options in menuconfig. The :ref:`CONFIG_ULP_COPROC_RESERVE_MEM` option will reserve RTC memory for the ULP and must be set to a value big enough to store both the ULP LP-Core code and data. If the application components contain multiple ULP programs, then the size of the RTC memory must be sufficient to hold the largest one.


4. Build the application as usual (e.g., `idf.py app`).

   During the build process, the following steps are taken to build ULP program:

   1. **Run each source file through the C compiler and assembler.** This step generates the object files (.obj.c or .obj.S depending of source file processed) in the component build directory.

   2. **Run the linker script template through the C preprocessor.** The template is located in ``components/ulp/ld`` directory.

   3. **Link the object files into an output ELF file** (``ulp_app_name.elf``). The Map file (``ulp_app_name.map``) generated at this stage may be useful for debugging purposes.

   4. **Dump the contents of the ELF file into a binary** (``ulp_app_name.bin``) which can then be embedded into the application.

   5. **Generate a list of global symbols** (``ulp_app_name.sym``) in the ELF file using ``riscv32-esp-elf-nm``.

   6. **Create an LD export script and a header file** (``ulp_app_name.ld`` and ``ulp_app_name.h``) containing the symbols from ``ulp_app_name.sym``. This is done using the ``esp32ulp_mapgen.py`` utility.

   7. **Add the generated binary to the list of binary files** to be embedded into the application.

.. _ulp-lp-core-access-variables:

Accessing the ULP LP-Core Program Variables
-------------------------------------------

Global symbols defined in the ULP LP-Core program may be used inside the main program.

For example, the ULP LP-Core program may define a variable ``measurement_count`` which will define the number of GPIO measurements the program needs to make before waking up the chip from deep sleep.

.. code-block:: c

    volatile int measurement_count;

    int some_function()
    {
        //read the measurement count for later use.
        int temp = measurement_count;

        ...do something.
    }

The main program can access the global ULP LP-Core program variables as the build system makes this possible by generating the ``${ULP_APP_NAME}.h`` and ``${ULP_APP_NAME}.ld`` files which define the global symbols present in the ULP LP-Core program. Each global symbol defined in the ULP LP-Core program is included in these files and are prefixed with ``ulp_``.

The header file contains the declaration of the symbol:

.. code-block:: c

    extern uint32_t ulp_measurement_count;

Note that all symbols (variables, arrays, functions) are declared as ``uint32_t``. For functions and arrays, take the address of the symbol and cast it to the appropriate type.

The generated linker script file defines the locations of symbols in LP_MEM::

    PROVIDE ( ulp_measurement_count = 0x50000060 );

To access the ULP LP-Core program variables from the main program, the generated header file should be included using an ``include`` statement. This will allow the ULP LP-Core program variables to be accessed as regular variables.

.. code-block:: c

    #include "ulp_app_name.h"

    void init_ulp_vars() {
        ulp_measurement_count = 64;
    }


Starting the ULP LP-Core Program
--------------------------------

To run a ULP LP-Core program, the main application needs to load the ULP program into RTC memory using the :cpp:func:`ulp_lp_core_load_binary` function, and then start it using the :cpp:func:`ulp_lp_core_run` function.

Each ULP LP-Core program is embedded into the ESP-IDF application as a binary blob. The application can reference this blob and load it in the following way (supposed ULP_APP_NAME was defined to ``ulp_app_name``):

.. code-block:: c

    extern const uint8_t bin_start[] asm("_binary_ulp_app_name_bin_start");
    extern const uint8_t bin_end[]   asm("_binary_ulp_app_name_bin_end");

    void start_ulp_program() {
        ESP_ERROR_CHECK( ulp_lp_core_load_binary( bin_start,
            (bin_end - bin_start)) );
    }

Once the program is loaded into LP memory, the application can be configured and started by calling :cpp:func:`ulp_lp_core_run`:

.. code-block:: c

    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER, // LP core will be woken up periodically by LP timer
        .lp_timer_sleep_duration_us = 10000,
    };

    ESP_ERROR_CHECK( ulp_lp_core_run(&cfg) );

ULP LP-Core Program Flow
------------------------

How the ULP LP-Core coprocessor is started depends on the wakeup source selected in :cpp:type:`ulp_lp_core_cfg_t`. The most common use-case will be for the ULP to periodically wake-up, do some measurements before either waking up the main CPU or going back to sleep again.

The ULP has the following wake-up sources:
    * :c:macro:`ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU` - LP Core can be woken up by the HP CPU.
    * :c:macro:`ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER` - LP Core can be woken up by the LP timer.
    * :c:macro:`ULP_LP_CORE_WAKEUP_SOURCE_ETM` - LP Core can be woken up by a ETM event. (Not yet supported)
    * :c:macro:`ULP_LP_CORE_WAKEUP_SOURCE_LP_IO` - LP Core can be woken up when LP IO level changes. (Not yet supported)
    * :c:macro:`ULP_LP_CORE_WAKEUP_SOURCE_LP_UART` - LP Core can be woken up after receiving a certain number of UART RX pulses. (Not yet supported)

When the ULP is woken up it will go through the following steps:

1. Initialize system feature, e.g. interrupts
2. Call user code: ``main()``
3. Return from ``main()``
4. If ``lp_timer_sleep_duration_us`` is specified then configure the next wake-up alarm
5. Call :cpp:func:`ulp_lp_core_halt`

ULP LP-Core Peripheral Support
------------------------------

To enhance the capabilities of the ULP LP-Core coprocessor, it has access to peripherals which operate in the low-power domain. The ULP LP-Core coprocessor can interact with these peripherals when the main CPU is in sleep mode, and can wake up the main CPU once a wakeup condition is reached. The following peripherals are supported:

 * LP IO
 * LP I2C

Application Examples
--------------------

* ULP LP-Core Coprocessor polls GPIO while main CPU is in deep sleep: :example:`system/ulp/lp_core/gpio`.
* ULP LP-Core Coprocessor reads external I2C ambient light sensor (BH1750) while the main CPU is in Deep-sleep and wakes up the main CPU once a threshold is met: :example:`system/ulp/lp_core/lp_i2c`.

API Reference
-------------

Main CPU API Reference
~~~~~~~~~~~~~~~~~~~~~~
.. include-build-file:: inc/ulp_lp_core.inc
.. include-build-file:: inc/lp_core_i2c.inc

LP Core API Reference
~~~~~~~~~~~~~~~~~~~~~~
.. include-build-file:: inc/ulp_lp_core_utils.inc
.. include-build-file:: inc/ulp_lp_core_gpio.inc
.. include-build-file:: inc/ulp_lp_core_i2c.inc
