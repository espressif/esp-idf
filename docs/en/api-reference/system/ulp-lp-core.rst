ULP LP Core Coprocessor Programming
===================================

:link_to_translation:`zh_CN:[中文]`

The ULP LP core (Low-power core) coprocessor is a variant of the ULP present in {IDF_TARGET_NAME}. It features ultra-low power consumption while also being able to stay powered on while the main CPU stays in low-power modes. This enables the LP core coprocessor to handle tasks like GPIO or sensor readings while the main CPU is in sleep mode, resulting in significant overall power savings for the entire system.

The ULP LP core coprocessor has the following features:

* An RV32I (32-bit RISC-V ISA) processor, with the multiplication/division (M), atomic (A), and compressed (C) extensions.
* Interrupt controller.
* Includes a debug module that supports external debugging via JTAG.
* Can access all of the High-power (HP) SRAM and peripherals when the entire system is active.
* Can access the Low-power (LP) SRAM and peripherals when the HP system is in sleep mode.

Compiling Code for the ULP LP Core
----------------------------------

The ULP LP core code is compiled together with your ESP-IDF project as a separate binary and automatically embedded into the main project binary. There are two ways to achieve this:

Using ``ulp_embed_binary``
~~~~~~~~~~~~~~~~~~~~~~~~~~~

1. Place the ULP LP core code, written in C or assembly (with the ``.S`` extension), in a dedicated directory within the component directory, such as ``ulp/``.

2. After registering the component in the ``CMakeLists.txt`` file, call the ``ulp_embed_binary`` function. Here is an example:

.. code-block:: cmake

    idf_component_register()

    set(ulp_app_name ulp_${COMPONENT_NAME})
    set(ulp_sources "ulp/ulp_c_source_file.c" "ulp/ulp_assembly_source_file.S")
    set(ulp_exp_dep_srcs "ulp_c_source_file.c")

    ulp_embed_binary(${ulp_app_name} "${ulp_sources}" "${ulp_exp_dep_srcs}")

The first argument to ``ulp_embed_binary`` specifies the ULP binary name. The name specified here is also used by other generated artifacts such as the ELF file, map file, header file, and linker export file. The second argument specifies the ULP source files. Finally, the third argument specifies the list of component source files which include the header file to be generated. This list is needed to build the dependencies correctly and ensure that the generated header file is created before any of these files are compiled. See the section below for the concept of generated header files for ULP applications.

Variables in the ULP code will be prefixed with ``ulp_`` (default value) in this generated header file.

If you need to embed multiple ULP programs, you may add a custom prefix in order to avoid conflicting variable names like this:

.. code-block:: cmake

    idf_component_register()

    set(ulp_app_name ulp_${COMPONENT_NAME})
    set(ulp_sources "ulp/ulp_c_source_file.c" "ulp/ulp_assembly_source_file.S")
    set(ulp_exp_dep_srcs "ulp_c_source_file.c")

    ulp_embed_binary(${ulp_app_name} "${ulp_sources}" "${ulp_exp_dep_srcs}" PREFIX "ULP::")

The additional PREFIX argument can be a C style prefix (like ``ulp2_``) or a C++ style prefix (like ``ULP::``).

Using a Custom CMake Project
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

It is also possible to create a custom CMake project for the LP core. This gives more control over the build process and allows you to set compile options, link external libraries and all other things that are possible with a regular CMake project.

To do this, add the ULP project as an external project in your component ``CMakeLists.txt`` file:

.. code-block:: cmake

    ulp_add_project("ULP_APP_NAME" "${CMAKE_SOURCE_DIR}/PATH_TO_DIR_WITH_ULP_PROJECT_FILE/")

Create a folder which contains your ULP project files and a ``CMakeLists.txt`` file, located at the path given to ``ulp_add_project``. The ``CMakeLists.txt`` file should look like this:

.. code-block:: cmake

    cmake_minimum_required(VERSION 3.16)

    # Project/target name is passed from the main project to allow IDF to have a dependency on this target
    # as well as embed the binary into the main app
    project(${ULP_APP_NAME})
    add_executable(${ULP_APP_NAME} main.c)

    # Import the ULP project helper functions
    include(IDFULPProject)

    # Apply default compile options
    ulp_apply_default_options(${ULP_APP_NAME})

    # Apply default sources provided by the IDF ULP component
    ulp_apply_default_sources(${ULP_APP_NAME})

    # Add targets for building the binary, as well as the linkerscript which exports ULP shared variables to the main app
    ulp_add_build_binary_targets(${ULP_APP_NAME})

    # Everything below this line is optional and can be used to customize the build process

    # Create a custom library
    set(lib_path "${CMAKE_CURRENT_LIST_DIR}/lib")
    add_library(custom_lib STATIC "${lib_path}/lib_src.c")
    target_include_directories(custom_lib PUBLIC "${lib_path}/")

    # Link the library
    target_link_libraries(${ULP_APP_NAME} PRIVATE custom_lib)

    # Set custom compile flags
    target_compile_options(${ULP_APP_NAME} PRIVATE -msave-restore)

Building Your Project
~~~~~~~~~~~~~~~~~~~~~~

To compile and build your project:

1. Enable both :ref:`CONFIG_ULP_COPROC_ENABLED` and :ref:`CONFIG_ULP_COPROC_TYPE` in menuconfig, and set :ref:`CONFIG_ULP_COPROC_TYPE` to ``CONFIG_ULP_COPROC_TYPE_LP_CORE``. The :ref:`CONFIG_ULP_COPROC_RESERVE_MEM` option reserves RTC memory for the ULP, and must be set to a value big enough to store both the ULP LP core code and data. If the application components contain multiple ULP programs, then the size of the RTC memory must be sufficient to hold the largest one.

2. Build the application as usual (e.g., ``idf.py app``).

During the build process, the following steps are taken to build ULP program:

    1. **Run each source file through the C compiler and assembler.** This step generates the object files ``.obj.c`` or ``.obj.S`` in the component build directory depending on the source file processed.

    2. **Run the linker script template through the C preprocessor.** The template is located in ``components/ulp/ld`` directory.

    3. **Link the object files into an output ELF file** (``ulp_app_name.elf``). The Map file ``ulp_app_name.map`` generated at this stage may be useful for debugging purposes.

    4. **Dump the contents of the ELF file into a binary** (``ulp_app_name.bin``) which can then be embedded into the application.

    5. **Generate a list of global symbols** (``ulp_app_name.sym``) in the ELF file using ``riscv32-esp-elf-nm``.

    6. **Create an LD export script and a header file** ``ulp_app_name.ld`` and ``ulp_app_name.h`` containing the symbols from ``ulp_app_name.sym``. This is done using the ``esp32ulp_mapgen.py`` utility.

    7. **Add the generated binary to the list of binary files** to be embedded into the application.


.. _ulp-lp-core-access-variables:

Accessing the ULP LP Core Program Variables
-------------------------------------------

Global symbols defined in the ULP LP core program may be used inside the main program.

For example, the ULP LP core program may define a variable ``measurement_count`` which defines the number of GPIO measurements the program needs to make before waking up the chip from Deep-sleep.

.. code-block:: c

    volatile int measurement_count;

    int some_function()
    {
        //read the measurement count for later use.
        int temp = measurement_count;

        ...do something.
    }

The main program can access the global ULP LP core program variables as the build system makes this possible by generating the ``${ULP_APP_NAME}.h`` and ``${ULP_APP_NAME}.ld`` files which define the global symbols present in the ULP LP core program. Each global symbol defined in the ULP LP core program is included in these files and are prefixed with ``ulp_``.

The header file contains the declaration of the symbol:

.. code-block:: c

    extern uint32_t ulp_measurement_count;

Note that all symbols (variables, functions) are declared as ``uint32_t``. Arrays are declared as ``uint32_t [SIZE]``. For functions, take the address of the symbol and cast it to the appropriate type.

To access the ULP LP core program variables from the main program, the generated header file should be included using an ``include`` statement. This allows the ULP LP core program variables to be accessed as regular variables.

.. code-block:: c

    #include "ulp_app_name.h"

    void init_ulp_vars() {
        ulp_measurement_count = 64;
    }

.. note::

    - Variables declared in the global scope of the LP core program reside in either the ``.bss`` or ``.data`` section of the binary. These sections are initialized when the LP core binary is loaded and executed. Accessing these variables from the main program on the HP-Core before the first LP core run may result in undefined behavior.

    - The ``ulp_`` prefix is the default value. You can specify the prefix to use with ``ulp_embed_binary`` to avoid name collisions for multiple ULP programs.


Starting the ULP LP Core Program
--------------------------------

To run a ULP LP core program, the main application needs to load the ULP program into RTC memory using the :cpp:func:`ulp_lp_core_load_binary` function, and then start it using the :cpp:func:`ulp_lp_core_run` function.

Each ULP LP core program is embedded into the ESP-IDF application as a binary blob. The application can reference this blob and load it in the following way (supposed ULP_APP_NAME was defined to ``ulp_app_name``):

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

ULP LP Core Program Flow
------------------------

How the ULP LP core coprocessor is started depends on the wake-up source selected in :cpp:type:`ulp_lp_core_cfg_t`. The most common use-case is for the ULP to periodically wake up, do some measurements before either waking up the main CPU or going back to sleep again.

The ULP has the following wake-up sources:
    * :c:macro:`ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU` - LP core can be woken up by the HP CPU.
    * :c:macro:`ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER` - LP core can be woken up by the LP timer.
    * :c:macro:`ULP_LP_CORE_WAKEUP_SOURCE_ETM` - LP core can be woken up by a ETM event. (Not yet supported)
    * :c:macro:`ULP_LP_CORE_WAKEUP_SOURCE_LP_IO` - LP core can be woken up when LP IO level changes. (Not yet supported)
    * :c:macro:`ULP_LP_CORE_WAKEUP_SOURCE_LP_UART` - LP core can be woken up when LP UART receives wakeup data based on different modes.

When the ULP is woken up, it will go through the following steps:

.. list::

    :CONFIG_ESP_ROM_HAS_LP_ROM: #. Unless :cpp:member:`ulp_lp_core_cfg_t::skip_lp_rom_boot` is specified, run ROM start-up code and jump to the entry point in LP RAM. ROM start-up code will initialize LP UART as well as print boot messages.
    #. Initialize system feature, e.g., interrupts
    #. Call user code ``main()``
    #. Return from ``main()``
    #. If ``lp_timer_sleep_duration_us`` is specified, then configure the next wake-up alarm
    #. Call :cpp:func:`ulp_lp_core_halt`


ULP LP Core Peripheral Support
------------------------------

To enhance the capabilities of the ULP LP core coprocessor, it has access to peripherals that operate in the low-power domain. The ULP LP core coprocessor can interact with these peripherals when the main CPU is in sleep mode, and can wake up the main CPU once a wake-up condition is reached. The following peripherals are supported:

.. list::

    * LP IO
    * LP I2C
    * LP UART
    :SOC_LP_SPI_SUPPORTED: * LP SPI

.. only:: CONFIG_ESP_ROM_HAS_LP_ROM

    ULP LP Core ROM
    ---------------

    The ULP LP core ROM is a small pre-built piece of code located in LP-ROM, which can't be modified. Similar to the bootloader ROM code ran by the main CPU, this code is executed when the ULP LP core coprocessor is started. The ROM code initializes the ULP LP core coprocessor and then jumps to the user program. The ROM code also prints boot messages if the LP UART has been initialized.

    The ROM code is not executed if :cpp:member:`ulp_lp_core_cfg_t::skip_lp_rom_boot` is set to true. This is useful when you need the ULP to wake-up as quickly as possible and the extra overhead of initializing and printing is unwanted.

    In addition to the boot-up code mentioned above, the ROM code also provides the following functions and interfaces:

    * :component_file:`ROM.ld Interface <esp_rom/{IDF_TARGET_PATH_NAME}/ld/{IDF_TARGET_PATH_NAME}lp.rom.ld>`
    * :component_file:`newlib.ld Interface <esp_rom/{IDF_TARGET_PATH_NAME}/ld/{IDF_TARGET_PATH_NAME}lp.rom.newlib.ld>`

    Since these functions are already present in LP-ROM no matter what, using these in your program allows you to reduce the RAM footprint of your ULP application.


ULP LP Core Interrupts
----------------------

The LP core coprocessor can be configured to handle interrupts from various sources. Examples of such interrupts could be LP IO low/high or LP timer interrupts. To register a handler for an interrupt, simply override any of the weak handlers provided by IDF. A complete list of handlers can be found in :component_file:`ulp_lp_core_interrupts.h <ulp/lp_core/lp_core/include/ulp_lp_core_interrupts.h>`. For details on which interrupts are available on a specific target, please consult **{IDF_TARGET_NAME} Technical Reference Manual** [`PDF <{IDF_TARGET_TRM_EN_URL}#ulp>`__].

For example, to override the handler for the LP IO interrupt, you can define the following function in your ULP LP core code:

.. code-block:: c

    void LP_CORE_ISR_ATTR ulp_lp_core_lp_io_intr_handler(void)
    {
        // Handle the interrupt and clear the interrupt source
    }

:c:macro:`LP_CORE_ISR_ATTR` is a macro that is used to define the interrupt handler function. This macro ensures that registers are saved and restored correctly when the interrupt handler is called.

In addition to configuring the interrupt related registers for the interrupt source you want to handle, you also need to enable the interrupts globally in the LP core interrupt controller. This can be done using the :cpp:func:`ulp_lp_core_intr_enable` function.

ULP LP Core Clock Configuration
-------------------------------

{IDF_TARGET_XTAL_FREQ:default="Not updated", esp32c5="48 MHz", esp32p4="40 MHz"}

The ULP LP Core clock source is based on the system clock ``LP_FAST_CLK``, see `TRM <{IDF_TARGET_TRM_EN_URL}>`__ > ``Reset and Clock`` for more details.

.. only:: SOC_CLK_LP_FAST_SUPPORT_XTAL

    On {IDF_TARGET_NAME}, ``LP_FAST_CLK`` supports using the external {IDF_TARGET_XTAL_FREQ} crystal (XTAL) as its clock source. This allows the ULP LP Core to run at a higher frequency than with the default ``RTC_FAST_CLOCK``, which runs at around 20 MHz. However, there is a trade-off: this clock is normally powered down during sleep to reduce power consumption, but if XTAL is selected as the source, it will remain powered up during sleep, which increases power consumption. If you only plan to use the LP Core as a coprocessor while the HP Core is active, then selecting XTAL can enhance both the performance and frequency stability of the LP Core.

    To enable this feature, set :ref:`CONFIG_RTC_FAST_CLK_SRC` to ``CONFIG_RTC_FAST_CLK_SRC_XTAL``.


Debugging ULP LP-Core Applications
----------------------------------

When programming the LP core, it can sometimes be challenging to figure out why the program is not behaving as expected. Here are some strategies to help you debug your LP core program:

* Use the LP UART to print: the LP core has access to the LP UART peripheral, which can be used for printing information independently of the main CPU sleep state. See :example:`system/ulp/lp_core/lp_uart/lp_uart_print` for an example of how to use this driver.

* Routing :cpp:func:`lp_core_printf` to the HP-Core console UART with :ref:`CONFIG_ULP_HP_UART_CONSOLE_PRINT`. This allows you to easily print LP core information to the already connected HP-Core console UART. The drawback of this approach is that it requires the main CPU to be awake and since there is no synchronization between the LP and HP cores, the output may be interleaved.

* Share program state through shared variables: as described in :ref:`ulp-lp-core-access-variables`, both the main CPU and the ULP core can easily access global variables in RTC memory. Writing state information to such a variable from the ULP and reading it from the main CPU can help you discern what is happening on the ULP core. The downside of this approach is that it requires the main CPU to be awake, which will not always be the case. Keeping the main CPU awake might even, in some cases, mask problems, as some issues may only occur when certain power domains are powered down.

* Panic handler: the LP core has a panic handler that can dump the state of the LP core registers by the LP UART when an exception is detected. To enable the panic handler, set the :ref:`CONFIG_ULP_PANIC_OUTPUT_ENABLE` option to ``y``. This option can be kept disabled to reduce LP-RAM usage by the LP core application. To recover a backtrace from the panic dump, it is possible to use ``idf.py monitor``.

.. warning::

    If multiple ULP applications are used in a single project, backtrace decoding might not work correctly. In such cases, it is recommended to use the esp-idf-monitor_ tool directly with the correct ULP ELF file:

    .. code-block:: bash

        python -m esp_idf_monitor --toolchain-prefix riscv32-esp-elf- --target {IDF_TARGET_PATH_NAME} --decode-panic backtrace PATH_TO_ULP_ELF_FILE


Debugging ULP LP Core Applications with GDB and OpenOCD
-------------------------------------------------------

It is also possible to debug code running on LP core using GDB and OpenOCD as you usually do for HP cores, but it has some specifics and limitations.

Debugging Session
~~~~~~~~~~~~~~~~~

Run OpenOCD with special config file for LP core debugging support. And then run GDB with special ``gdbinit`` file.

.. code-block:: bash

    openocd -f board/{IDF_TARGET_PATH_NAME}-lpcore-builtin.cfg
    riscv32-esp-elf-gdb -x gdbinit <path to main program ELF>

Below is the ``gdbinit`` file content with inline comments. For more details, see the next section.

.. code-block:: bash

    # connect to target
    target extended-remote :3333
    # reset chip
    mon reset halt
    maintenance flush register-cache
    # add symbols and debugging info for ULP program
    add-symbol <path to ULP program ELF>
    # temporary HW breakpoint to setup breakpoints
    # if you need more than HW supports
    thb main
    commands
    # set breakpoints here
    # At this moment ULP program is loaded into RAM and when there are
    # no free HW breakpoints slots available GDB will set SW ones
    b func1
    b func2
    b func3
    # resume execution
    c
    end
    # start main program after reset
    c

LP Core Debugging Specifics
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. list::

    #. For convenient debugging, you may need to add ``-O0`` compile option for ULP app in its ``CMakeLists.txt``. See :example:`system/ulp/lp_core/debugging/` on how to do this.
    :not esp32p4: #. LP core supports limited set of HW exceptions, so, for example, writing at address `0x0` will not cause a panic as it would be for the code running on HP core. This can be overcome to some extent by enabling undefined behavior sanitizer for LP core application, so `ubsan` can help to catch some errors. But note that it will increase code size significantly and it can happen that application won't fit into RTC RAM. To enable `ubsan` for ULP app, add ``-fsanitize=undefined -fno-sanitize=shift-base`` compile option to its ``CMakeLists.txt``. See :example:`system/ulp/lp_core/debugging/` on how to do this.
    #. To be able to debug program running on LP core, debugging information and symbols need to be loaded to GDB. It can be done via GDB command line or in ``gdbinit`` file. See section above.
    #. Upon startup, LP core application is loaded into RAM, so all SW breakpoints set before that moment will get overwritten. The best moment to set breakpoints for LP core application is to do this when LP core program reaches `main` function.
    #. When using IDEs, it may lack support for configuring breakpoint actions or commands shown in ``gdbinit`` above. Consequently, you have to preset all breakpoints before debug session start and disable all of them except for ``main``. When program stops at ``main``, enable the remaining breakpoints and resume execution manually.

Limitations
~~~~~~~~~~~

#. Currently, debugging is not supported when either HP or LP core enters any sleep mode. So it limits available debugging scenarios.
#. FreeRTOS support in OpenOCD is disabled when debugging LP core, so you won't be able to see tasks running in the system. Instead, there will be several threads representing HP and LP cores:

.. code-block:: bash

    (gdb) info thread
        Id   Target Id                                                          Frame
        1    Thread 1 "{IDF_TARGET_PATH_NAME}.cpu0" (Name: {IDF_TARGET_PATH_NAME}.cpu0, state: debug-request) 0x40803772 in esp_cpu_wait_for_intr ()
            at /home/user/projects/esp/esp-idf/components/esp_hw_support/cpu.c:64
      * 2    Thread 2 "{IDF_TARGET_PATH_NAME}.cpu1" (Name: {IDF_TARGET_PATH_NAME}.cpu1, state: breakpoint)    do_things (max=1000000000)
            at /home/user/projects/esp/esp-idf/examples/system/ulp/lp_core/debugging/main/lp_core/main.c:21

#. When setting HW breakpoint in GDB, it is set on both cores, so the number of available HW breakpoints is limited to the number of them supported by LP core ({IDF_TARGET_SOC_CPU_BREAKPOINTS_NUM} for {IDF_TARGET_NAME}).
#. OpenOCD flash support is disabled. It does not matter for LP core application because it is run completely from RAM and GDB can use SW breakpoints for it. But if you want to set a breakpoint on function from flash used by the code running on HP core (e.g., `app_main`), you should request to set HW breakpoint explicitly via ``hb`` and ``thb`` GDB commands.
#. Since the main and ULP programs are linked as separate binaries, it is possible for them to have global symbols (such as functions or variables) with the same name. If you set a breakpoint using the function name, GDB will apply it to all instances of that function. This can cause issues if one of the functions is located in the flash, as OpenOCD currently doesn't support flash when debugging the LP core. In such cases, you can set breakpoints using the source line or the function's memory address instead.

Application Examples
--------------------

.. list::

    - :example:`system/ulp/lp_core/gpio` polls GPIO while main CPU is in Deep-sleep.
    :esp32c6: - :example:`system/ulp/lp_core/lp_i2c` reads external I2C ambient light sensor (BH1750) while the main CPU is in Deep-sleep and wakes up the main CPU once a threshold is met.
    - :example:`system/ulp/lp_core/lp_uart/lp_uart_echo` reads data written to a serial console and echoes it back. This example demonstrates the usage of the LP UART driver running on the LP core.
    - :example:`system/ulp/lp_core/lp_uart/lp_uart_print` shows how to print various statements from a program running on the LP core.
    - :example:`system/ulp/lp_core/lp_uart/lp_uart_char_seq_wakeup` shows how to trigger a wakeup using the LP UART specific character sequence wakeup mode.
    - :example:`system/ulp/lp_core/interrupt` shows how to register an interrupt handler on the LP core to receive an interrupt triggered by the main CPU.
    - :example:`system/ulp/lp_core/gpio_intr_pulse_counter` shows how to use GPIO interrupts to count pulses while the main CPU is in Deep-sleep mode.
    - :example:`system/ulp/lp_core/build_system/` demonstrates how to include custom ``CMakeLists.txt`` file for the ULP app.
    - :example:`system/ulp/lp_core/debugging` shows how to debug code running on LP core using GDB and OpenOCD.

API Reference
-------------

Main CPU API Reference
~~~~~~~~~~~~~~~~~~~~~~

.. include-build-file:: inc/ulp_lp_core.inc
.. include-build-file:: inc/lp_core_i2c.inc
.. include-build-file:: inc/lp_core_uart.inc

.. only:: SOC_LP_SPI_SUPPORTED

    .. include-build-file:: inc/lp_core_spi.inc

.. only:: SOC_LP_CORE_SUPPORT_ETM

    .. include-build-file:: inc/lp_core_etm.inc

.. include-build-file:: inc/lp_core_types.inc

LP Core API Reference
~~~~~~~~~~~~~~~~~~~~~~

.. include-build-file:: inc/ulp_lp_core_utils.inc
.. include-build-file:: inc/ulp_lp_core_gpio.inc
.. include-build-file:: inc/ulp_lp_core_i2c.inc
.. include-build-file:: inc/ulp_lp_core_uart.inc
.. include-build-file:: inc/ulp_lp_core_print.inc
.. include-build-file:: inc/ulp_lp_core_interrupts.inc

.. only:: SOC_LP_SPI_SUPPORTED

    .. include-build-file:: inc/ulp_lp_core_spi.inc

.. _esp-idf-monitor: https://github.com/espressif/esp-idf-monitor

.. only:: SOC_UART_HAS_LP_UART

    .. include-build-file:: inc/ulp_lp_core_lp_uart_shared.inc
