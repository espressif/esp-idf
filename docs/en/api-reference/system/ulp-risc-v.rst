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

Compiling the ULP RISC-V Code
-----------------------------

To compile the ULP RISC-V code as part of the component, the following steps must be taken:

1. The ULP RISC-V code, written in C or assembly (must use the ``.S`` extension), must be placed in a separate directory inside the component directory, for instance, ``ulp/``.

.. note::

    When registering the component (via ``idf_component_register``), this directory should not be added to the ``SRC_DIRS`` argument as it is currently done for the ULP FSM. See the step below for how to properly add ULP source files.

2. Call ``ulp_embed_binary`` from the component CMakeLists.txt after registration. For example::

    ...
    idf_component_register()

    set(ulp_app_name ulp_${COMPONENT_NAME})
    set(ulp_sources "ulp/ulp_c_source_file.c" "ulp/ulp_assembly_source_file.S")
    set(ulp_exp_dep_srcs "ulp_c_source_file.c")

    ulp_embed_binary(${ulp_app_name} "${ulp_sources}" "${ulp_exp_dep_srcs}")

 The first argument to ``ulp_embed_binary`` specifies the ULP binary name. The name specified here will also be used by other generated artifacts such as the ELF file, map file, header file, and linker export file. The second argument specifies the ULP source files. Finally, the third argument specifies the list of component source files which include the header file to be generated. This list is needed to build the dependencies correctly and ensure that the generated header file will be created before any of these files are compiled. See the section below for the concept of generated header files for ULP applications.

3. Build the application as usual (e.g., ``idf.py app``).

   Inside, the build system will take the following steps to build ULP program:

   1. **Run each source file through the C compiler and assembler.** This step generates the object files (``.obj.c`` or ``.obj.S`` depending of source file processed) in the component build directory.

   2. **Run the linker script template through the C preprocessor.** The template is located in ``components/ulp/ld`` directory.

   3. **Link the object files into an output ELF file** (``ulp_app_name.elf``). The Map file (``ulp_app_name.map``) generated at this stage may be useful for debugging purposes.

   4. **Dump the contents of the ELF file into a binary** (``ulp_app_name.bin``) which can then be embedded into the application.

   5. **Generate a list of global symbols** (``ulp_app_name.sym``) in the ELF file using ``riscv32-esp-elf-nm``.

   6. **Create an LD export script and a header file** (``ulp_app_name.ld`` and ``ulp_app_name.h``) containing the symbols from ``ulp_app_name.sym``. This is done using the ``esp32ulp_mapgen.py`` utility.

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

Note that all symbols (variables, arrays, functions) are declared as ``uint32_t``. For functions and arrays, take the address of the symbol and cast it to the appropriate type.

The generated linker script file defines the locations of symbols in RTC_SLOW_MEM::

    PROVIDE ( ulp_measurement_count = 0x50000060 );

To access the ULP RISC-V program variables from the main program, the generated header file should be included using an ``include`` statement. This will allow the ULP RISC-V program variables to be accessed as regular variables.

.. code-block:: c

    #include "ulp_app_name.h"

    void init_ulp_vars() {
        ulp_measurement_count = 64;
    }

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

    The RTC I2C peripheral always expects a slave sub-register address to be programmed via the :cpp:func:`ulp_riscv_i2c_master_set_slave_reg_addr` API. If it is not, the I2C peripheral uses the ``SENS_SAR_I2C_CTRL_REG[18:11]`` as the sub-register address for the subsequent read or write operations. This could make the RTC I2C peripheral incompatible with certain I2C devices or sensors which do not need any sub-register to be programmed.

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

* ULP RISC-V Coprocessor polls GPIO while main CPU is in deep sleep: :example:`system/ulp/ulp_riscv/gpio`.
* ULP RISC-V Coprocessor uses bit-banged UART driver to print: :example:`system/ulp/ulp_riscv/uart_print`.
* ULP RISC-V Coprocessor reads external temperature sensor while main CPU is in deep sleep: :example:`system/ulp/ulp_riscv/ds18b20_onewire`.
* ULP RISC-V Coprocessor reads external I2C temperature and humidity sensor (BMP180) while the main CPU is in Deep-sleep and wakes up the main CPU once a threshold is met: :example:`system/ulp/ulp_riscv/i2c`.
* ULP RISC-V Coprocessor handles software interrupt and RTC IO interrupt: :example:`system/ulp/ulp_riscv/interrupts`.

API Reference
-------------

.. include-build-file:: inc/ulp_riscv.inc
.. include-build-file:: inc/ulp_riscv_lock_shared.inc
.. include-build-file:: inc/ulp_riscv_lock.inc
.. include-build-file:: inc/ulp_riscv_i2c.inc
