Dedicated GPIO
==============

Overview
--------

The dedicated GPIO is designed for CPU interaction with GPIO matrix and IO MUX. Any GPIO that is configured as "dedicated" can be access by CPU instructions directly, which makes it easy to achieve a high GPIO flip speed, and simulate serial/parallel interface in a bit-banging way.


Create/Destroy GPIO Bundle
--------------------------

A GPIO bundle is a group of GPIOs, which can be manipulated at the same time in one CPU cycle. The maximal number of GPIOs that a bundle can contain is limited by each CPU. What's more, the GPIO bundle has a strong relevance to the CPU which it derives from. **Any operations on the GPIO bundle should be put inside a task which is running on the same CPU core to the GPIO bundle belongs to.** Likewise, only those ISRs who are installed on the same CPU core are allowed to do operations on that GPIO bundle.

.. note::
    Dedicated GPIO is more of a CPU peripheral, so it has a strong relationship with CPU core. It's highly recommended to install and operate GPIO bundle in a pin-to-core task. For example, if GPIOA is connected to CPU0, and the dedicated GPIO instruction is issued from CPU1, then it's impossible to control GPIOA.

To install a GPIO bundle, one needs to call :cpp:func:`dedic_gpio_new_bundle` to allocate the software resources and connect the dedicated channels to user selected GPIOs. Configurations for a GPIO bundle are covered in :cpp:type:`dedic_gpio_bundle_config_t` structure:

- :cpp:member:`gpio_array`: An array that contains GPIO number.
- :cpp:member:`array_size`: Element number of :cpp:member:`gpio_array`.
- :cpp:member:`flags`: Extra flags to control the behavior of GPIO Bundle.

  - :cpp:member:`in_en` and :cpp:member:`out_en` are used to select whether to enable the input and output function (note, they can be enabled together).
  - :cpp:member:`in_invert` and :cpp:member:`out_invert` are used to select whether to invert the GPIO signal.

The following code shows how to install a output only GPIO bundle:

.. highlight:: c

::

    // configure GPIO
    const int bundleA_gpios[] = {0, 1};
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_OUTPUT,
    };
    for (int i = 0; i < sizeof(bundleA_gpios) / sizeof(bundleA_gpios[0]); i++) {
        io_conf.pin_bit_mask = 1ULL << bundleA_gpios[i];
        gpio_config(&io_conf);
    }
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

To uninstall the GPIO bundle, one needs to call :cpp:func:`dedic_gpio_del_bundle`.

.. note::
    :cpp:func:`dedic_gpio_new_bundle` doesn't cover any GPIO pad configuration (e.g. pull up/down, drive ability, output/input enable), so before installing a dedicated GPIO bundle, you have to configure the GPIO separately using GPIO driver API (e.g. :cpp:func:`gpio_config`). For more information about GPIO driver, please refer to :doc:`GPIO API Reference <gpio>`.


GPIO Bundle Operations
----------------------

.. list-table::
   :widths: 50 50
   :header-rows: 1

   * - Operations
     - Functions
   * - Write to GPIOs in the bundle by mask
     - :cpp:func:`dedic_gpio_bundle_write`
   * - Read the value that input to bundle
     - :cpp:func:`dedic_gpio_bundle_read_out`
   * - Read the value that output from bundle
     - :cpp:func:`dedic_gpio_bundle_read_in`

.. note::
    The functions above just wrap the customized instructions defined for {IDF_TARGET_NAME}, for the details of those instructions, please refer to {IDF_TARGET_NAME} TRM.

.. only:: esp32s2

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

        // enable positive edge interrupt on the second GPIO in the bundle (i.e. index 1)
        ESP_ERROR_CHECK(dedic_gpio_bundle_set_interrupt_and_callback(bundle, BIT(1), DEDIC_GPIO_INTR_POS_EDGE, dedic_gpio_isr_callback, sem));

        // wait for done semaphore
        xSemaphoreTake(sem, portMAX_DELAY);


Manipulate GPIOs by Writing Assembly Code
------------------------------------------

For advanced users, they can always manipulate the GPIOs by writing assembly code or invoking CPU Low Level APIs. The usual procedure could be:

1. Allocate a GPIO bundle: :cpp:func:`dedic_gpio_new_bundle`
2. Query the mask occupied by that bundle: :cpp:func:`dedic_gpio_get_out_mask` or/and :cpp:func:`dedic_gpio_get_in_mask`
3. Call CPU LL apis (e.g. `cpu_ll_write_dedic_gpio_mask`) or write assembly code with that mask

For details of supported dedicated GPIO instructions, please refer to {IDF_TARGET_NAME} TRM.

.. note::
    Writing assembly code in application could make your code hard to port between targets, because those customized instructions are not guaranteed to remain the same format in different targets.


Application Example
-------------------

Matrix keyboard example based on dedicated GPIO: :example:`peripherals/gpio/matrix_keyboard`.


API Reference
-------------

.. include-build-file:: inc/dedic_gpio.inc
