GPIO & RTC GPIO
===============

:link_to_translation:`en:[English]`

GPIO 汇总
---------

.. include:: gpio/{IDF_TARGET_PATH_NAME}.inc
    :start-after: gpio-summary
    :end-before: ---


.. only:: SOC_RTCIO_INPUT_OUTPUT_SUPPORTED

    当 GPIO 连接到 "RTC" 低功耗和模拟子系统时，{IDF_TARGET_NAME} 芯片还单独支持 "RTC GPIO"。可在以下情况时使用这些管脚功能：

    .. list::

        - 处于 Deep-sleep 模式时
        :SOC_ULP_SUPPORTED: - :doc:`超低功耗协处理器 (ULP) <../../api-reference/system/ulp>` 运行时
        - 使用 ADC/DAC 等模拟功能时

.. only:: SOC_GPIO_SUPPORT_PIN_GLITCH_FILTER or SOC_GPIO_FLEX_GLITCH_FILTER_NUM

    GPIO 毛刺过滤器
    ---------------

	{IDF_TARGET_NAME} 内置硬件的过滤器可以帮助过滤掉 GPIO 输入端口上的毛刺信号, 这可以一定程度上避免错误地触发中断或者是错把噪声当成有效的外设信号。

	.. only:: SOC_GPIO_SUPPORT_PIN_GLITCH_FILTER

		每个 GPIO 都可以使用独立的毛刺过滤器，该过滤器可以将那些脉冲宽度窄于 **2** 个采样时钟的信号剔除掉， 并且这个宽度是无法配置的。GPIO 对输入信号的采样时钟通常是 IO-MUX 的时钟源。在驱动中， 我们将此类过滤器称为 “pin glitch filter“。 你可以调用 :cpp:func:`gpio_new_pin_glitch_filter` 函数来创建一个过滤器句柄。过滤器的相关配置保存在 :cpp:type:`gpio_pin_glitch_filter_config_t` 结构体中。

		- :cpp:member:`gpio_pin_glitch_filter_config_t::gpio_num` 设置哪个 GPIO 需要开启毛刺过滤器。

	.. only:: SOC_GPIO_FLEX_GLITCH_FILTER_NUM

		{IDF_TARGET_FLEX_GLITCH_FILTER_NUM:default="8"}

		{IDF_TARGET_NAME} 提供了 {IDF_TARGET_FLEX_GLITCH_FILTER_NUM} 个灵活的毛刺过滤器， 被过滤信号的脉冲宽度可以由软件进行配置。我们称此类过滤器为 "flex flitch filter"。每个过滤器可以分配给任意一个 GPIO, 但是将多个过滤器应用在同一个 GPIO 上并不能带来更好的效果。你可以调用 :cpp:func:`gpio_new_flex_glitch_filter` 函数来创建一个过滤器句柄。过滤器的相关配置保存在 :cpp:type:`gpio_flex_glitch_filter_config_t` 结构体中。

		- :cpp:member:`gpio_flex_glitch_filter_config_t::gpio_num` 设置哪个 GPIO 需要使用毛刺过滤器。
		- :cpp:member:`gpio_flex_glitch_filter_config_t::window_width_ns` 和 :cpp:member:`gpio_flex_glitch_filter_config_t::window_thres_ns` 是毛刺过滤器的关键参数。在:cpp:member:`gpio_flex_glitch_filter_config_t::window_width_ns` 时间内，任何脉冲信号，如果它的宽度小于 :cpp:member:`gpio_flex_glitch_filter_config_t::window_thres_ns`, 那么该脉冲信号就会被滤除掉。:cpp:member:`gpio_flex_glitch_filter_config_t::window_thres_ns` 的值不能大于 :cpp:member:`gpio_flex_glitch_filter_config_t::window_width_ns`。

	.. only:: SOC_GPIO_SUPPORT_PIN_GLITCH_FILTER and SOC_GPIO_FLEX_GLITCH_FILTER_NUM

		请注意， "pin glitch filter" 和 "flex glitch filter" 是各自独立的，你可以给同一个 GPIO 同时启用他们两种滤波器。

	毛刺滤波器默认情况下是处于关闭状态的， 你需要调用 :cpp:func:`gpio_glitch_filter_enable` 去使能它。如果要回收这个过滤器，你可以调用 :cpp:func:`gpio_del_glitch_filter` 函数。注意， 在你回收过滤器句柄之前， 请确保它是处于关闭状态的，如果不是，你需要调用 :cpp:func:`gpio_glitch_filter_disable`。


应用示例
-------------------

* GPIO 输出和输入中断示例：:example:`peripherals/gpio/generic_gpio`。

API 参考 - 普通 GPIO
---------------------------

.. include-build-file:: inc/gpio.inc
.. include-build-file:: inc/gpio_types.inc


.. only:: SOC_RTCIO_INPUT_OUTPUT_SUPPORTED

    API 参考 - RTC GPIO
    ------------------------

    .. include-build-file:: inc/rtc_io.inc
    .. include-build-file:: inc/rtc_io_types.inc

.. only:: SOC_GPIO_SUPPORT_PIN_GLITCH_FILTER or SOC_GPIO_FLEX_GLITCH_FILTER_NUM

      API 参考 - GPIO 毛刺过滤器
      --------------------------

      .. include-build-file:: inc/gpio_filter.inc
