Dedicated GPIO
==============

:link_to_translation:`zh_CN:[中文]`

Overview
--------

The dedicated GPIO is designed for CPU interaction with GPIO matrix and IO MUX. Any GPIO that is configured as "dedicated" can be access by CPU instructions directly, which makes it easy to achieve a high GPIO flip speed, and simulate serial/parallel interface in a bit-banging way. As toggling a GPIO in this "CPU Dedicated" way costs few overhead, it would be great for cases like performance measurement using an oscilloscope.


Create/Destroy GPIO Bundle
--------------------------

A GPIO bundle is a group of GPIOs, which can be manipulated at the same time in one CPU cycle. The maximal number of GPIOs that a bundle can contain is limited by each CPU. What's more, the GPIO bundle has a strong relevance to the CPU which it derives from. **Any operations on the GPIO bundle should be put inside a task which is running on the same CPU core to the GPIO bundle belongs to.** Likewise, only those ISRs who are installed on the same CPU core are allowed to do operations on that GPIO bundle.

.. note::

    Dedicated GPIO is more like a CPU peripheral, it has a strong relationship with CPU core. It's highly recommended to install and operate GPIO bundle in the same task, and the task should be pined to a CPU core. For example, if GPIO_A is connected to CPU_0, but the dedicated GPIO instruction is issued from CPU_1, then it's impossible to control GPIO_A.

To install a GPIO bundle, one needs to call :cpp:func:`dedic_gpio_new_bundle` to allocate the software resources and connect the dedicated channels to user selected GPIOs. Configurations for a GPIO bundle are covered in :cpp:type:`dedic_gpio_bundle_config_t` structure:

- :cpp:member:`dedic_gpio_bundle_config_t::gpio_array`: An array that contains GPIO number.
- :cpp:member:`dedic_gpio_bundle_config_t::array_size`: Element number of :cpp:member:`dedic_gpio_bundle_config_t::gpio_array`.
- :cpp:member:`dedic_gpio_bundle_config_t::in_en` and :cpp:member:`dedic_gpio_bundle_config_t::out_en` are used to configure whether to enable the input and output ability of the GPIO(s).
- :cpp:member:`dedic_gpio_bundle_config_t::in_invert` and :cpp:member:`dedic_gpio_bundle_config_t::out_invert` are used to configure whether to invert the GPIO signal.

The following code shows how to install an output only GPIO bundle:

.. highlight:: c

::

    // Create bundleA, output only
    dedic_gpio_bundle_handle_t bundleA = NULL;
    dedic_gpio_bundle_config_t bundleA_config = {
        .gpio_array = bundleA_gpios,
        .array_size = sizeof(bundleA_gpios) / sizeof(bundleA_gpios[0]),
        .flags = {
            .out_en = 1,
        },
    };
    ESP_ERROR_CHECK(dedic_gpio_new_bundle(&bundleA_config, &bundleA));

To uninstall the GPIO bundle, you should call :cpp:func:`dedic_gpio_del_bundle`.


GPIO Bundle Operations
----------------------

.. list-table::
   :widths: 50 50
   :header-rows: 1

   * - Operations
     - Functions
   * - Write to GPIOs in the bundle by mask
     - :cpp:func:`dedic_gpio_bundle_write`
   * - Read the value that output from the given GPIO bundle
     - :cpp:func:`dedic_gpio_bundle_read_out`
   * - Read the value that input to the given GPIO bundle
     - :cpp:func:`dedic_gpio_bundle_read_in`

.. note::

    Using the above functions might not get a high GPIO flip speed because of the overhead of function calls and the bit operations involved inside. Users can try :ref:`manipulate_gpios_by_writing_assembly_code` instead to reduce the overhead but should take care of the thread safety by themselves.

.. _manipulate_gpios_by_writing_assembly_code:

Manipulate GPIOs by Writing Assembly Code
------------------------------------------

For advanced users, they can always manipulate the GPIOs by writing assembly code or invoking CPU Low Level APIs. The usual procedure could be:

1. Allocate a GPIO bundle: :cpp:func:`dedic_gpio_new_bundle`
2. Query the mask occupied by that bundle: :cpp:func:`dedic_gpio_get_out_mask` or/and :cpp:func:`dedic_gpio_get_in_mask`
3. Call CPU LL apis (e.g., `dedic_gpio_cpu_ll_write_mask`) or write assembly code with that mask
4. The fastest way of toggling IO is to use the dedicated "set/clear" instructions:

    .. only:: CONFIG_IDF_TARGET_ARCH_XTENSA

        - Set bits of GPIO: ``set_bit_gpio_out imm[7:0]``
        - Clear bits of GPIO: ``clr_bit_gpio_out imm[7:0]``
        - Note: Immediate value width depends on the number of dedicated GPIO channels

    .. only:: CONFIG_IDF_TARGET_ARCH_RISCV

        - Set bits of GPIO: ``csrrsi rd, csr, imm[4:0]``
        - Clear bits of GPIO: ``csrrci rd, csr, imm[4:0]``
        - Note: Can only control the lowest 4 GPIO channels

.. only:: esp32s2

    For details of supported dedicated GPIO instructions, please refer to **{IDF_TARGET_NAME} Technical Reference Manual** > **IO MUX and GPIO Matrix (GPIO, IO_MUX)** [`PDF <{IDF_TARGET_TRM_EN_URL}#iomuxgpio>`__].

.. only:: esp32s3

    For details of supported dedicated GPIO instructions, please refer to **{IDF_TARGET_NAME} Technical Reference Manual** > **Processor Instruction Extensions (PIE) (to be added later)** [`PDF <{IDF_TARGET_TRM_EN_URL}#pie>`__].

.. only:: not (esp32s2 or esp32s3)

    For details of supported dedicated GPIO instructions, please refer to **{IDF_TARGET_NAME} Technical Reference Manual** > **ESP-RISC-V CPU** [`PDF <{IDF_TARGET_TRM_EN_URL}#riscvcpu>`__].

Some of the dedicated CPU instructions are also wrapped inside ``hal/dedic_gpio_cpu_ll.h`` as helper inline functions.

.. note::

    Writing assembly code in application could make your code hard to port between targets, because those customized instructions are not guaranteed to remain the same format on different targets.

.. only:: SOC_DEDIC_GPIO_HAS_INTERRUPT

    Interrupt Handling
    ------------------

    Dedicated GPIO can also trigger interrupt on specific input event. All supported events are defined in :cpp:type:`dedic_gpio_intr_type_t`.

    One can enable and register interrupt callback by calling :cpp:func:`dedic_gpio_bundle_set_interrupt_and_callback`. The prototype of the callback function is defined in :cpp:type:`dedic_gpio_isr_callback_t`. Keep in mind, the callback should return true if there's some high priority task woken up.

    .. highlight:: c

    ::

        // user defined ISR callback
        IRAM_ATTR bool dedic_gpio_isr_callback(dedic_gpio_bundle_handle_t bundle, uint32_t index, void *args)
        {
            SemaphoreHandle_t sem = (SemaphoreHandle_t)args;
            BaseType_t high_task_wakeup = pdFALSE;
            xSemaphoreGiveFromISR(sem, &high_task_wakeup);
            return high_task_wakeup == pdTRUE;
        }

        // enable positive edge interrupt on the second GPIO in the bundle (i.e., index 1)
        ESP_ERROR_CHECK(dedic_gpio_bundle_set_interrupt_and_callback(bundle, BIT(1), DEDIC_GPIO_INTR_POS_EDGE, dedic_gpio_isr_callback, sem));

        // wait for done semaphore
        xSemaphoreTake(sem, portMAX_DELAY);


Application Example
-------------------

.. list::

    * Software emulation (bit banging) of the UART/I2C/SPI protocols in assembly using the dedicated GPIOs and their associated CPU instructions: :example:`peripherals/dedicated_gpio`.
    :SOC_DEDIC_GPIO_HAS_INTERRUPT: * :example:`peripherals/gpio/matrix_keyboard` demonstrates how to drive a matrix keyboard using the dedicated GPIO APIs, including manipulating the level on a group of GPIOs, triggering edge interrupt, and reading level on a group of GPIOs.
    * :example:`peripherals/dedicated_gpio/soft_i2c` demonstrates how to configure and use dedicated/fast GPIOs to emulate an I2C master, perform write-read transactions on the bus, and handle strict timing requirements by placing certain functions in IRAM.
    * :example:`peripherals/dedicated_gpio/soft_uart` demonstrates how to emulate a UART bus using dedicated/fast GPIOs on {IDF_TARGET_NAME}, which can send and receive characters on the UART bus using a TX pin and an RX pin, with the baud rate and other configurations adjustable via `menuconfig`.

    .. only:: esp32c2 or esp32c3 or esp32c6 or esp32h2 or esp32p4

        * :example:`peripherals/dedicated_gpio/soft_spi` demonstrates how to configure and use dedicated/fast GPIOs to emulate a full-duplex SPI bus on {IDF_TARGET_NAME}.


API Reference
-------------

.. include-build-file:: inc/dedic_gpio.inc
