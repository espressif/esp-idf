GPIO & RTC GPIO
===============

:link_to_translation:`zh_CN:[中文]`

GPIO Summary
------------

.. include:: gpio/{IDF_TARGET_PATH_NAME}.inc
    :start-after: gpio-summary
    :end-before: ---

.. only:: SOC_RTCIO_INPUT_OUTPUT_SUPPORTED

    .. only:: not SOC_LP_PERIPHERALS_SUPPORTED

        There is also separate "RTC GPIO" support, which functions when GPIOs are routed to the "RTC" low-power and analog subsystem. These pin functions can be used when:

    .. only:: SOC_LP_PERIPHERALS_SUPPORTED

        There is also separate "RTC GPIO" support, which functions when GPIOs are routed to the "RTC" low-power, analog subsystem, and Low-Power(LP) peripherals. These pin functions can be used when:

    .. list::

        - In Deep-sleep mode
        :SOC_ULP_FSM_SUPPORTED: - The :doc:`Ultra Low Power FSM co-processor <../../api-reference/system/ulp>` is running
        :SOC_RISCV_COPROC_SUPPORTED: - The :doc:`Ultra Low Power RISC-V co-processor <../../api-reference/system/ulp-risc-v>` is running
        :SOC_LP_CORE_SUPPORTED: - The :doc:`Ultra Low Power LP-Core co-processor <../../api-reference/system/ulp-lp-core>` is running
        - Analog functions such as ADC/DAC/etc are in use
        :SOC_LP_PERIPHERALS_SUPPORTED: - LP peripherals, such as LP_UART, LP_I2C, are in use

Check Current Configuration of IOs
----------------------------------

GPIO driver offers a dump function :cpp:func:`gpio_dump_io_configuration` to show the current configurations of IOs, such as pull-up/pull-down, input/output enable, pin mapping, etc. Below is an example of how to dump the configuration of GPIO4, GPIO18, and GPIO26:

::

    gpio_dump_io_configuration(stdout, (1ULL << 4) | (1ULL << 18) | (1ULL << 26));

The dump will be like this:

::

    ================IO DUMP Start================
    IO[4] -
      Pullup: 1, Pulldown: 0, DriveCap: 2
      InputEn: 1, OutputEn: 0, OpenDrain: 0
      FuncSel: 1 (GPIO)
      GPIO Matrix SigIn ID: (simple GPIO input)
      SleepSelEn: 1

    IO[18] -
      Pullup: 0, Pulldown: 0, DriveCap: 2
      InputEn: 0, OutputEn: 1, OpenDrain: 0
      FuncSel: 1 (GPIO)
      GPIO Matrix SigOut ID: 256 (simple GPIO output)
      SleepSelEn: 1

    IO[26] **RESERVED** -
      Pullup: 1, Pulldown: 0, DriveCap: 2
      InputEn: 1, OutputEn: 0, OpenDrain: 0
      FuncSel: 0 (IOMUX)
      SleepSelEn: 1

    =================IO DUMP End==================

In addition, if you would like to dump the configurations of all IOs, you can use:

::

    gpio_dump_all_io_configuration(stdout, SOC_GPIO_VALID_GPIO_MASK);

If an IO pin is routed to a peripheral signal through the GPIO matrix, the signal ID printed in the dump information is defined in the :component_file:`soc/{IDF_TARGET_PATH_NAME}/include/soc/gpio_sig_map.h` header file. The word ``**RESERVED**`` indicates the IO is occupied by either SPI flash or PSRAM. It is strongly not recommended to reconfigure them for other application purposes.

Do not rely on the default configurations values in the Technical Reference Manual, because it may be changed in the bootloader or application startup code before app_main.

.. only:: esp32c3 or esp32c6 or esp32h2 or esp32p4 or esp32s2 or esp32s3 or esp32c5

    Configure USB PHY Pins to GPIO
    -------------------------------

    To configure the USB PHY pins to GPIO, you can use the function :cpp:func:`gpio_config`. Below is an example of how to configure the USB PHY pins to GPIO:

    .. code-block:: c

        gpio_config_t usb_phy_conf = {
            .pin_bit_mask = (1ULL << USB_PHY_DP_PIN) | (1ULL << USB_PHY_DM_PIN),
            .mode = GPIO_MODE_INPUT_OUTPUT,
            .pull_up_en = 0,
            .pull_down_en = 0,
            .intr_type = GPIO_INTR_DISABLE,
        };
        gpio_config(&usb_phy_conf);

.. only:: SOC_GPIO_SUPPORT_PIN_GLITCH_FILTER or SOC_GPIO_FLEX_GLITCH_FILTER_NUM

    GPIO Glitch Filter
    ------------------

    The {IDF_TARGET_NAME} chip features hardware filters to remove unwanted glitch pulses from the input GPIO, which can help reduce false triggering of the interrupt and prevent a noise being routed to the peripheral side.

    .. only:: SOC_GPIO_SUPPORT_PIN_GLITCH_FILTER

        Each GPIO can be configured with a glitch filter, which can be used to filter out pulses shorter than **two** sample clock cycles. The duration of the filter is not configurable. The sample clock is the clock source of the IO_MUX. In the driver, we call this kind of filter as ``pin glitch filter``. You can create the filter handle by calling :cpp:func:`gpio_new_pin_glitch_filter`. All the configurations for a pin glitch filter are listed in the :cpp:type:`gpio_pin_glitch_filter_config_t` structure.

        - :cpp:member:`gpio_pin_glitch_filter_config_t::gpio_num` sets the GPIO number to enable the glitch filter.

    .. only:: SOC_GPIO_FLEX_GLITCH_FILTER_NUM

        {IDF_TARGET_FLEX_GLITCH_FILTER_NUM:default="8"}

        {IDF_TARGET_NAME} provides {IDF_TARGET_FLEX_GLITCH_FILTER_NUM} flexible glitch filters, whose duration is configurable. We refer to this kind of filter as ``flex flitch filter``. Each of them can be applied to any input GPIO. However, applying multiple filters to the same GPIO does not make difference from one. You can create the filter handle by calling :cpp:func:`gpio_new_flex_glitch_filter`. All the configurations for a flexible glitch filter are listed in the :cpp:type:`gpio_flex_glitch_filter_config_t` structure.

        - :cpp:member:`gpio_flex_glitch_filter_config_t::gpio_num` sets the GPIO that will be applied to the flex glitch filter.
        - :cpp:member:`gpio_flex_glitch_filter_config_t::window_width_ns` and :cpp:member:`gpio_flex_glitch_filter_config_t::window_thres_ns` are the key parameters of the glitch filter. During :cpp:member:`gpio_flex_glitch_filter_config_t::window_width_ns`, any pulse whose width is shorter than :cpp:member:`gpio_flex_glitch_filter_config_t::window_thres_ns` will be discarded. Please note that, you can not set :cpp:member:`gpio_flex_glitch_filter_config_t::window_thres_ns` bigger than :cpp:member:`gpio_flex_glitch_filter_config_t::window_width_ns`.

    .. only:: SOC_GPIO_SUPPORT_PIN_GLITCH_FILTER and SOC_GPIO_FLEX_GLITCH_FILTER_NUM

        Please note, the ``pin glitch filter`` and ``flex glitch filter`` are independent. You can enable both of them for the same GPIO.

    The glitch filter is disabled by default, and can be enabled by calling :cpp:func:`gpio_glitch_filter_enable`. To recycle the filter, you can call :cpp:func:`gpio_del_glitch_filter`. Please note, before deleting the filter, you should disable it first by calling :cpp:func:`gpio_glitch_filter_disable`.


.. only:: SOC_GPIO_SUPPORT_PIN_HYS_FILTER

    GPIO Hysteresis Filter
    ----------------------

    {IDF_TARGET_NAME} support the hardware hysteresis of the input pin, which can reduce the GPIO interrupt shoot by accident due to unstable sampling when the input voltage is near the criteria of logic 0 and 1, especially when the input logic level conversion is slow or the voltage setup time is too long.

    .. only:: SOC_GPIO_SUPPORT_PIN_HYS_CTRL_BY_EFUSE

        Each pin can enable hysteresis function independently. By default, it controlled by eFuse and been closed, but it can also be enabled or disabled by software manually. You can select the hysteresis control mode by configuring :cpp:member:`gpio_config_t::hys_ctrl_mode`. Hysteresis control mode is set along with all the other GPIO configurations in :cpp:func:`gpio_config`.

        .. note::

            When the hysteresis function is controlled by eFuse, this feature can still be controlled independently for each pin, you need to `burn the eFuse <https://docs.espressif.com/projects/esptool/en/latest/esp32/espefuse/index.html>`_ to enable the hysteresis function on specific GPIO additionally.

    .. only:: not SOC_GPIO_SUPPORT_PIN_HYS_CTRL_BY_EFUSE

        Each pin can enable hysteresis function independently. By default, the function is not enabled. You can select the hysteresis control mode by configuring :cpp:member:`gpio_config_t::hys_ctrl_mode`. Hysteresis control mode is set along with all the other GPIO configurations in :cpp:func:`gpio_config`.


Application Example
-------------------

.. list::

    * :example:`peripherals/gpio/generic_gpio` demonstrates how to configure GPIO to generate pulses and use it with interruption.
    :esp32s2: * :example:`peripherals/gpio/matrix_keyboard` demonstrates how to drive a common matrix keyboard using the dedicated GPIO APIs, including manipulating the level on a group of GPIOs, triggering edge interrupt, and reading level on a group of GPIOs.


API Reference - Normal GPIO
---------------------------

.. include-build-file:: inc/gpio.inc
.. include-build-file:: inc/gpio_types.inc


.. only:: SOC_RTCIO_INPUT_OUTPUT_SUPPORTED

    API Reference - RTC GPIO
    ------------------------

    .. include-build-file:: inc/rtc_io.inc
    .. include-build-file:: inc/lp_io.inc
    .. include-build-file:: inc/rtc_io_types.inc

.. only:: SOC_GPIO_SUPPORT_PIN_GLITCH_FILTER or SOC_GPIO_FLEX_GLITCH_FILTER_NUM

      API Reference - GPIO Glitch Filter
      ----------------------------------

      .. include-build-file:: inc/gpio_filter.inc
