ULP RISC-V Coprocessor Programming
==================================

:link_to_translation:`zh_CN:[中文]`

The ULP RISC-V coprocessor is a variant of the ULP present in {IDF_TARGET_NAME}. Similar to ULP FSM, the ULP RISC-V coprocessor can perform tasks such as sensor readings while the main CPU stays in low power modes. The main difference between ULP FSM and ULP RISC-V is that the latter can be programmed in C using standard GNU tools. The ULP RISC-V coprocessor can access the RTC_SLOW_MEM memory region, and registers in ``RTC_CNTL``, ``RTC_IO``, and ``SARADC`` peripherals. The RISC-V processor is a 32-bit fixed point machine. Its instruction set is based on RV32IMC which includes hardware multiplication and division, and compressed code.

Installing the ULP RISC-V Toolchain
-----------------------------------

The ULP RISC-V coprocessor code is written in C (assembly is also possible) and compiled using the RISC-V toolchain based on GCC.

If you have already set up ESP-IDF with CMake build system according to the :doc:`Getting Started Guide <../../../get-started/index>`, then the toolchain should already be installed.

.. note::

    In earlier versions of ESP-IDF, RISC-V toolchain had a different prefix: ``riscv-none-embed-gcc``.

Compiling Code for the ULP RISC-V
----------------------------------

The ULP RISC-V code is compiled together with your ESP-IDF project as a separate binary and automatically embedded into the main project binary. There are two ways to achieve this:

Using ``ulp_embed_binary``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. Place the ULP RISC-V code, written in C or assembly (with the ``.S`` extension), in a dedicated directory within the component directory, such as ``ulp/``.

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
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

It is also possible to create a custom CMake project for the ULP RISC-V. This gives more control over the build process and allows you to set compile options, link external libraries and all other things that are possible with a regular CMake project.

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
^^^^^^^^^^^^^^^^^^^^^

To compile and build your project:

1. Enable both :ref:`CONFIG_ULP_COPROC_ENABLED` and :ref:`CONFIG_ULP_COPROC_TYPE` in menuconfig, and set :ref:`CONFIG_ULP_COPROC_TYPE` to ``CONFIG_ULP_COPROC_TYPE_LP_CORE``. The :ref:`CONFIG_ULP_COPROC_RESERVE_MEM` option reserves RTC memory for the ULP, and must be set to a value big enough to store both the ULP LP-Core code and data. If the application components contain multiple ULP programs, then the size of the RTC memory must be sufficient to hold the largest one.

2. Build the application as usual (e.g., ``idf.py app``).

During the build process, the following steps are taken to build ULP program:

    1. **Run each source file through the C compiler and assembler.** This step generates the object files ``.obj.c`` or ``.obj.S`` in the component build directory depending on the source file processed.

    2. **Run the linker script template through the C preprocessor.** The template is located in ``components/ulp/ld`` directory.

    3. **Link the object files into an output ELF file** (``ulp_app_name.elf``). The Map file ``ulp_app_name.map`` generated at this stage may be useful for debugging purposes.

    4. **Dump the contents of the ELF file into a binary** (``ulp_app_name.bin``) which can then be embedded into the application.

    5. **Generate a list of global symbols** (``ulp_app_name.sym``) in the ELF file using ``riscv32-esp-elf-nm``.

    6. **Create an LD export script and a header file** ``ulp_app_name.ld`` and ``ulp_app_name.h`` containing the symbols from ``ulp_app_name.sym``. This is done using the ``esp32ulp_mapgen.py`` utility.

    7. **Add the generated binary to the list of binary files** to be embedded into the application.


.. _ulp-riscv-access-variables:

Accessing the ULP RISC-V Program Variables
------------------------------------------

Global symbols defined in the ULP RISC-V program may be used inside the main program.

For example, the ULP RISC-V program may define a variable ``measurement_count`` which will define the number of ADC measurements the program needs to make before waking up the chip from deep sleep.

.. code-block:: c

    volatile int measurement_count;

    int some_function()
    {
        //read the measurement count for later use.
        int temp = measurement_count;

        ...do something.
    }

The main program can access the global ULP RISC-V program variables as the build system makes this possible by generating the ``${ULP_APP_NAME}.h`` and ``${ULP_APP_NAME}.ld`` files which define the global symbols present in the ULP RISC-V program. Each global symbol defined in the ULP RISC-V program is included in these files and are prefixed with ``ulp_``.

The header file contains the declaration of the symbol:

.. code-block:: c

    extern uint32_t ulp_measurement_count;

Note that all symbols (variables, functions) are declared as ``uint32_t``. Arrays are declared as ``uint32_t [SIZE]``. For functions, take the address of the symbol and cast it to the appropriate type.

To access the ULP RISC-V program variables from the main program, the generated header file should be included using an ``include`` statement. This will allow the ULP RISC-V program variables to be accessed as regular variables.

.. code-block:: c

    #include "ulp_app_name.h"

    void init_ulp_vars() {
        ulp_measurement_count = 64;
    }

.. note::

    - Variables declared in the global scope of the ULP RISC-V program reside in either the ``.bss`` or ``.data`` section of the binary. These sections are initialized when the ULP RISC-V binary is loaded and executed. Accessing these variables from the main program on the main CPU before the first ULP RISC-V run may result in undefined behavior.

    - The ``ulp_`` prefix is the default value. You can specify the prefix to use with ``ulp_embed_binary`` to avoid name collisions for multiple ULP programs.

Mutual Exclusion
^^^^^^^^^^^^^^^^

If mutual exclusion is needed when accessing a variable shared between the main program and ULP, then this can be achieved by using the ULP RISC-V lock API:

 * :cpp:func:`ulp_riscv_lock_acquire`
 * :cpp:func:`ulp_riscv_lock_release`

The ULP does not have any hardware instructions to facilitate mutual exclusion, so the lock API achieves this through a software algorithm (`Peterson's algorithm <https://en.wikipedia.org/wiki/Peterson%27s_algorithm>`_).

The locks are intended to only be called from a single thread in the main program, and will not provide mutual exclusion if used simultaneously from multiple threads.

Starting the ULP RISC-V Program
-------------------------------

To run a ULP RISC-V program, the main application needs to load the ULP program into RTC memory using the :cpp:func:`ulp_riscv_load_binary` function, and then start it using the :cpp:func:`ulp_riscv_run` function.

Note that the ``CONFIG_ULP_COPROC_ENABLED`` and ``CONFIG_ULP_COPROC_TYPE_RISCV`` options must be enabled in menuconfig to work with ULP RISC-V. To reserve memory for the ULP, the ``RTC slow memory reserved for coprocessor`` option must be set to a value big enough to store ULP RISC-V code and data. If the application components contain multiple ULP programs, then the size of the RTC memory must be sufficient to hold the largest one.

Each ULP RISC-V program is embedded into the ESP-IDF application as a binary blob. The application can reference this blob and load it in the following way (suppose ULP_APP_NAME was defined to ``ulp_app_name``):

.. code-block:: c

    extern const uint8_t bin_start[] asm("_binary_ulp_app_name_bin_start");
    extern const uint8_t bin_end[]   asm("_binary_ulp_app_name_bin_end");

    void start_ulp_program() {
        ESP_ERROR_CHECK( ulp_riscv_load_binary( bin_start,
            (bin_end - bin_start)) );
    }

Once the program is loaded into RTC memory, the application can start it by calling the :cpp:func:`ulp_riscv_run` function:

.. code-block:: c

    ESP_ERROR_CHECK( ulp_riscv_run() );

ULP RISC-V Program Flow
-----------------------

{IDF_TARGET_RTC_CLK_FRE:default="150 kHz", esp32s2="90 kHz", esp32s3="136 kHz"}

The ULP RISC-V coprocessor is started by a timer. The timer is started once :cpp:func:`ulp_riscv_run` is called. The timer counts the number of RTC_SLOW_CLK ticks (by default, produced by an internal {IDF_TARGET_RTC_CLK_FRE} RC oscillator). The number of ticks is set using ``RTC_CNTL_ULP_CP_TIMER_1_REG`` register. When starting the ULP, ``RTC_CNTL_ULP_CP_TIMER_1_REG`` will be used to set the number of timer ticks.

The application can set ULP timer period values (RTC_CNTL_ULP_CP_TIMER_1_REG) using the :cpp:func:`ulp_set_wakeup_period` function.

Once the timer counts the number of ticks set in the ``RTC_CNTL_ULP_CP_TIMER_1_REG`` register, the ULP RISC-V coprocessor will power up and start running the program from the entry point set in the call to :cpp:func:`ulp_riscv_run`.

The program runs until the field ``RTC_CNTL_COCPU_DONE`` in register ``RTC_CNTL_COCPU_CTRL_REG`` gets written or when a trap occurs due to illegal processor state. Once the program halts, the ULP RISC-V coprocessor will power down, and the timer will be started again.

To disable the timer (effectively preventing the ULP program from running again), please clear the ``RTC_CNTL_ULP_CP_SLP_TIMER_EN`` bit in the ``RTC_CNTL_ULP_CP_TIMER_REG`` register. This can be done both from the ULP code and from the main program.

ULP RISC-V Peripheral Support
------------------------------

To enhance the capabilities of the ULP RISC-V coprocessor, it has access to peripherals which operate in the low-power (RTC) domain. The ULP RISC-V coprocessor can interact with these peripherals when the main CPU is in sleep mode, and can wake up the main CPU once a wakeup condition is reached. The following peripherals are supported.

RTC I2C
^^^^^^^^

The RTC I2C controller provides I2C master functionality in the RTC domain. The ULP RISC-V coprocessor can read from or write to I2C slave devices using this controller. To use the RTC I2C peripheral, call the :cpp:func:`ulp_riscv_i2c_master_init` from your application running on the main core before initializing the ULP RISC-V core and going to sleep.

Once the RTC I2C controller is initialized, the I2C slave device address must be programmed via the :cpp:func:`ulp_riscv_i2c_master_set_slave_addr` API before any read or write operation is performed.

.. note::

    The RTC I2C peripheral issues two kinds of I2C transactions:

    - **READ**: [start] → write device address → write device sub-register address → [repeated start] → write device address → read N bytes → [stop]
    - **WRITE**: [start] → write device address → write device sub-register address → [repeated start] → write device address → write N bytes → [stop]

    In both cases, sending the sub-register address is required and cannot be disabled. Therefore, the peripheral always expects a slave sub-register address to be set using the :cpp:func:`ulp_riscv_i2c_master_set_slave_reg_addr` API. If it is not set explicitly, the peripheral uses the value in ``SENS_SAR_I2C_CTRL_REG[18:11]`` as the sub-register address for subsequent transactions.

    This behavior makes the RTC I2C peripheral incompatible with:

    - Devices that do not expect a sub-register address write before initiating a read or write transaction.
    - Devices requiring 16-bit or wider register addresses, since only 8-bit addressing is supported.

.. note::

    There is no hardware atomicity protection in accessing the RTC I2C peripheral between the main CPU and the ULP RISC-V core. Therefore, care must be taken that both cores are not accessing the peripheral simultaneously.

In case your RTC I2C based ULP RISC-V program is not working as expected, the following sanity checks can help in debugging the issue:

 * Incorrect SDA/SCL pin selection: The SDA pin can only be set up as GPIO1 or GPIO3 and SCL pin can only be set up as GPIO0 or GPIO2. Make sure that the pin configuration is correct.

 * Incorrect I2C timing parameters: The RTC I2C bus timing configuration is limited by the I2C standard bus specification. Any timing parameters which violate the standard I2C bus specifications would result in an error. For details on the timing parameters, please read the `standard I2C bus specifications <https://en.wikipedia.org/wiki/I%C2%B2C>`_.

 * If the I2C slave device or sensor does not require a sub-register address to be programmed, it may not be compatible with the RTC I2C peripheral. Please refer the notes above.

 * If the RTC driver reports a ``Write Failed!`` or ``Read Failed!`` error log when running on the main CPU, then make sure:

        * The I2C slave device or sensor works correctly with the standard I2C master on Espressif SoCs. This would rule out any problems with the I2C slave device itself.
        * If the RTC I2C interrupt status log reports a ``TIMEOUT`` error or ``ACK`` error, it could typically mean that the I2C device did not respond to a ``START`` condition sent out by the RTC I2C controller. This could happen if the I2C slave device is not connected properly to the controller pins or if the I2C slave device is in a bad state. Make sure that the I2C slave device is in a good state and connected properly before continuing.
        * If the RTC I2C interrupt log does not report any error status, it could mean that the driver is not fast enough in receiving data from the I2C slave device. This could happen as the RTC I2C controller does not have a TX/RX FIFO to store multiple bytes of data but rather, it depends on single byte transmissions using an interrupt status polling mechanism. This could be mitigated to some extent by making sure that the SCL clock of the peripheral is running as fast as possible. This can be tweaked by configuring the SCL low period and SCL high period values in the initialization config parameters for the peripheral.

* Other methods of debugging problems would be to ensure that the RTC I2C controller is operational **only** on the main CPU **without** any ULP RISC-V code interfering and **without** any sleep mode being activated. This is the basic configuration under which the RTC I2C peripheral must work. This way you can rule out any potential issues due to the ULP or sleep modes.

ULP RISC-V Interrupt Handling
------------------------------

The ULP RISC-V core supports interrupt handling from certain internal and external events. By design, the ULP RISC-V core can handle interrupts from the following sources:

.. list-table:: ULP RISC-V interrupt sources
    :widths: 10 5 5
    :header-rows: 1

    * - Interrupt Source
      - Type
      - IRQ
    * - Internal Timer Interrupt
      - Internal
      - 0
    * - EBREAK or ECALL or Illegal Instruction
      - Internal
      - 1
    * - Unaligned Memory Access
      - Internal
      - 2
    * - RTC Peripheral Sources
      - External
      - 31

Interrupt handling is enabled via special 32-bit registers Q0-Q3 and custom R-type instructions. For more information, see *{IDF_TARGET_NAME} Technical Reference Manual* > *ULP Coprocessor* > *ULP-RISC-V* > *ULP-RISC-V Interrupts* [`PDF <{IDF_TARGET_TRM_EN_URL}>`__].

All interrupts are enabled globally during start-up. When an interrupt occurs, the processor jumps to the IRQ vector. The IRQ vector performs the task of saving the register context and then calling the global interrupt dispatcher. The ULP RISC-V driver implements a *weak* interrupt dispatcher :cpp:func:`_ulp_riscv_interrupt_handler` which serves as the central point for handling all interrupts. This global dispatcher calls respective interrupt handlers which have been allocated via the :cpp:func:`ulp_riscv_intr_alloc`.

Interrupt handling on the ULP RISC-V is not fully featured yet. At present, interrupt handling for internal interrupt sources is not supported. Support is provided for two RTC peripheral sources, viz., software-triggered interrupts and RTC IO-triggered interrupts. ULP RISC-V does not support nested interrupts. If users need custom interrupt handling then they may override the default global interrupt dispatcher by defining their own :cpp:func:`_ulp_riscv_interrupt_handler`.

Debugging Your ULP RISC-V Program
----------------------------------

When programming the ULP RISC-V, it can sometimes be challenging to figure out why the program is not behaving as expected. Due to the simplicity of the core, many of the standard methods of debugging, e.g., JTAG or ``printf``, are simply not available.

Keeping this in mind, here are some ways that may help you debug your ULP RISC-V program:

 * Share program state through shared variables: as described in :ref:`ulp-riscv-access-variables`, both the main CPU and the ULP core can easily access global variables in RTC memory. Writing state information to such a variable from the ULP and reading it from the main CPU can help you discern what is happening on the ULP core. The downside of this approach is that it requires the main CPU to be awake, which will not always be the case. Keeping the main CPU awake might even, in some cases, mask problems, as some issues may only occur when certain power domains are powered down.

 * Use the bit-banged UART driver to print: the ULP RISC-V component comes with a low-speed bit-banged UART TX driver that can be used for printing information independently of the main CPU state. See :example:`system/ulp/ulp_riscv/uart_print` for an example of how to use this driver.

 * Trap signal: the ULP RISC-V has a hardware trap that will trigger under certain conditions, e.g., illegal instruction. This will cause the main CPU to be woken up with the wake-up cause :cpp:enumerator:`ESP_SLEEP_WAKEUP_COCPU_TRAP_TRIG`.

Application Examples
--------------------

* :example:`system/ulp/ulp_riscv/gpio` demonstrates how to program the ULP-RISC-V coprocessor to monitor a GPIO pin and wake up the main CPU when its state changes.

* :example:`system/ulp/ulp_riscv/uart_print` demonstrates how to program the ULP-RISC-V coprocessor on the development board to bitbang a UART TX line, allowing for output logging directly from the ULP-RISC-V coprocessor even when the main CPU is in deep sleep.

.. only:: esp32s2

    * :example:`system/ulp/ulp_riscv/ds18b20_onewire` demonstrates how to use the ULP-RISC-V co-processor to read temperature from a DS18B20 sensor over 1-Wire, and wake up the main CPU from deep-sleep when the temperature exceeds a set limit.

* :example:`system/ulp/ulp_riscv/i2c` demonstrates how to use the RTC I2C peripheral from the ULP RISC-V coprocessor in deep sleep mode to periodically measure temperature and pressure values from the BMP180 sensor and wake up the main CPU when these values exceed a certain threshold.

* :example:`system/ulp/ulp_riscv/interrupts` demonstrates how the ULP-RISC-V coprocessor can register and handle software and RTC IO triggered interrupts, keeping a count of the software interrupts and waking up the main processor from deep sleep after a certain threshold or when a button is pressed.

* :example:`system/ulp/ulp_riscv/adc` demonstrates how to use the ULP-RISC-V coprocessor to periodically measure input voltage and wake up the system from deep sleep if the voltage exceeds a set threshold.

* :example:`system/ulp/ulp_riscv/gpio_interrupt` demonstrates how to program the ULP-RISC-V coprocessor to wake up from a RTC IO interrupt using GPIO0 as the input signal, and how to configure and run the coprocessor, putting the chip into deep sleep mode until the wakeup source pin is pulled low.

* :example:`system/ulp/ulp_riscv/touch` demonstrates how to program the ULP RISC-V coprocessor to periodically scan and read touch pad sensors, and wake up the main CPU when a touch pad is active.

API Reference
-------------

.. include-build-file:: inc/ulp_riscv.inc
.. include-build-file:: inc/ulp_riscv_lock_shared.inc
.. include-build-file:: inc/ulp_riscv_lock.inc
.. include-build-file:: inc/ulp_riscv_i2c.inc
