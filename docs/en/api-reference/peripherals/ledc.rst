LED Control (LEDC)
==================

{IDF_TARGET_LEDC_MAX_FADE_RANGE_NUM: default="1", esp32c6="16", esp32h2="16", esp32p4="16"}

:link_to_translation:`zh_CN:[中文]`

Introduction
------------

The LED control (LEDC) peripheral is primarily designed to control the intensity of LEDs, although it can also be used to generate PWM signals for other purposes. It has {IDF_TARGET_SOC_LEDC_CHANNEL_NUM} channels which can generate independent waveforms that can be used, for example, to drive RGB LED devices.

.. only:: esp32

    LEDC channels are divided into two groups of 8 channels each. One group of LEDC channels operates in high speed mode. This mode is implemented in hardware and offers automatic and glitch-free changing of the PWM duty cycle. The other group of channels operate in low speed mode, the PWM duty cycle must be changed by the driver in software. Each group of channels is also able to use different clock sources.

The PWM controller can automatically increase or decrease the duty cycle gradually, allowing for fades without any processor interference.


Functionality Overview
----------------------

.. only:: esp32

    Setting up a channel of the LEDC in either :ref:`high or low speed mode <ledc-api-high_low_speed_mode>` is done in three steps:


.. only:: not esp32

    Setting up a channel of the LEDC is done in three steps. Note that unlike ESP32, {IDF_TARGET_NAME} only supports configuring channels in "low speed" mode.

1. :ref:`ledc-api-configure-timer` by specifying the PWM signal's frequency and duty cycle resolution.
2. :ref:`ledc-api-configure-channel` by associating it with the timer and GPIO to output the PWM signal.
3. :ref:`ledc-api-change-pwm-signal` that drives the output in order to change LED's intensity. This can be done under the full control of software or with hardware fading functions.

As an optional step, it is also possible to set up an interrupt on fade end.

.. figure:: ../../../_static/ledc-api-settings.jpg
    :align: center
    :alt: Key Settings of LED PWM Controller's API
    :figclass: align-center

    Key Settings of LED PWM Controller's API

.. note::

    For an initial setup, it is recommended to configure for the timers first (by calling :cpp:func:`ledc_timer_config`), and then for the channels (by calling :cpp:func:`ledc_channel_config`). This ensures the PWM frequency is at the desired value since the appearance of the PWM signal from the IO pad.


.. _ledc-api-configure-timer:

Timer Configuration
^^^^^^^^^^^^^^^^^^^

Setting the timer is done by calling the function :cpp:func:`ledc_timer_config` and passing the data structure :cpp:type:`ledc_timer_config_t` that contains the following configuration settings:

.. list::

    :esp32:     - Speed mode :cpp:type:`ledc_mode_t`
    :not esp32: - Speed mode (value must be ``LEDC_LOW_SPEED_MODE``)
    - Timer number :cpp:type:`ledc_timer_t`
    - PWM signal frequency in Hz
    - Resolution of PWM duty
    - Source clock :cpp:type:`ledc_clk_cfg_t`

The frequency and the duty resolution are interdependent. The higher the PWM frequency, the lower the duty resolution which is available, and vice versa. This relationship might be important if you are planning to use this API for purposes other than changing the intensity of LEDs. For more details, see Section :ref:`ledc-api-supported-range-frequency-duty-resolution`.

The source clock can also limit the PWM frequency. The higher the source clock frequency, the higher the maximum PWM frequency can be configured.

.. only:: esp32

    .. list-table:: Characteristics of {IDF_TARGET_NAME} LEDC source clocks
       :widths: 5 5 5 20
       :header-rows: 1

       * - Clock name
         - Clock freq
         - Speed mode
         - Clock capabilities
       * - APB_CLK
         - 80 MHz
         - High / Low
         - /
       * - REF_TICK
         - 1 MHz
         - High / Low
         - Dynamic Frequency Scaling compatible
       * - RC_FAST_CLK
         - ~ 8 MHz
         - Low
         - Dynamic Frequency Scaling compatible, Light sleep compatible

.. only:: esp32s2

    .. list-table:: Characteristics of {IDF_TARGET_NAME} LEDC source clocks
       :widths: 15 15 30
       :header-rows: 1

       * - Clock name
         - Clock freq
         - Clock capabilities
       * - APB_CLK
         - 80 MHz
         - /
       * - REF_TICK
         - 1 MHz
         - Dynamic Frequency Scaling compatible
       * - RC_FAST_CLK
         - ~ 8 MHz
         - Dynamic Frequency Scaling compatible, Light sleep compatible
       * - XTAL_CLK
         - 40 MHz
         - Dynamic Frequency Scaling compatible

.. only:: esp32s3 or esp32c3

    .. list-table:: Characteristics of {IDF_TARGET_NAME} LEDC source clocks
       :widths: 15 15 30
       :header-rows: 1

       * - Clock name
         - Clock freq
         - Clock capabilities
       * - APB_CLK
         - 80 MHz
         - /
       * - RC_FAST_CLK
         - ~ 20 MHz
         - Dynamic Frequency Scaling compatible, Light sleep compatible
       * - XTAL_CLK
         - 40 MHz
         - Dynamic Frequency Scaling compatible

.. only:: esp32c2

    .. list-table:: Characteristics of {IDF_TARGET_NAME} LEDC source clocks
       :widths: 15 15 30
       :header-rows: 1

       * - Clock name
         - Clock freq
         - Clock capabilities
       * - PLL_60M_CLK
         - 60 MHz
         - /
       * - RC_FAST_CLK
         - ~ 20 MHz
         - Dynamic Frequency Scaling compatible, Light sleep compatible
       * - XTAL_CLK
         - 40 MHz
         - Dynamic Frequency Scaling compatible

.. only:: esp32c6 or esp32p4

    .. list-table:: Characteristics of {IDF_TARGET_NAME} LEDC source clocks
       :widths: 15 15 30
       :header-rows: 1

       * - Clock name
         - Clock freq
         - Clock capabilities
       * - PLL_80M_CLK
         - 80 MHz
         - /
       * - RC_FAST_CLK
         - ~ 20 MHz
         - Dynamic Frequency Scaling compatible, Light sleep compatible
       * - XTAL_CLK
         - 40 MHz
         - Dynamic Frequency Scaling compatible

.. only:: esp32h2

    .. list-table:: Characteristics of {IDF_TARGET_NAME} LEDC source clocks
       :widths: 15 15 30
       :header-rows: 1

       * - Clock name
         - Clock freq
         - Clock capabilities
       * - PLL_96M_CLK
         - 96 MHz
         - /
       * - RC_FAST_CLK
         - ~ 8 MHz
         - Dynamic Frequency Scaling compatible, Light sleep compatible
       * - XTAL_CLK
         - 32 MHz
         - Dynamic Frequency Scaling compatible

.. note::

    .. only:: SOC_CLK_RC_FAST_SUPPORT_CALIBRATION

        1. On {IDF_TARGET_NAME}, if RC_FAST_CLK is chosen as the LEDC clock source, an internal calibration will be performed to get the exact frequency of the clock. This ensures the accuracy of output PWM signal frequency.

    .. only:: not SOC_CLK_RC_FAST_SUPPORT_CALIBRATION

        1. On {IDF_TARGET_NAME}, if RC_FAST_CLK is chosen as the LEDC clock source, you may see the frequency of output PWM signal is not very accurate. This is because no internal calibration is performed to get the exact frequency of the clock due to hardware limitation, a theoretic frequency value is used.

    .. only:: not SOC_LEDC_HAS_TIMER_SPECIFIC_MUX

        2. For {IDF_TARGET_NAME}, all timers share one clock source. In other words, it is impossible to use different clock sources for different timers.

The LEDC driver offers a helper function :cpp:func:`ledc_find_suitable_duty_resolution` to find the maximum possible resolution for the timer, given the source clock frequency and the desired PWM signal frequency.

When a timer is no longer needed by any channel, it can be deconfigured by calling the same function :cpp:func:`ledc_timer_config`. The configuration structure :cpp:type:`ledc_timer_config_t` passes in should be:

-  :cpp:member:`ledc_timer_config_t::speed_mode` The speed mode of the timer which wants to be deconfigured belongs to (:cpp:type:`ledc_mode_t`)

-  :cpp:member:`ledc_timer_config_t::timer_num` The ID of the timers which wants to be deconfigured (:cpp:type:`ledc_timer_t`)

-  :cpp:member:`ledc_timer_config_t::deconfigure` Set this to true so that the timer specified can be deconfigured


.. _ledc-api-configure-channel:

Channel Configuration
^^^^^^^^^^^^^^^^^^^^^

When the timer is set up, configure the desired channel (one out of :cpp:type:`ledc_channel_t`). This is done by calling the function :cpp:func:`ledc_channel_config`.

Similar to the timer configuration, the channel setup function should be passed a structure :cpp:type:`ledc_channel_config_t` that contains the channel's configuration parameters.

At this point, the channel should start operating and generating the PWM signal on the selected GPIO, as configured in :cpp:type:`ledc_channel_config_t`, with the frequency specified in the timer settings and the given duty cycle. The channel operation (signal generation) can be suspended at any time by calling the function :cpp:func:`ledc_stop`.


.. _ledc-api-change-pwm-signal:

Change PWM Signal
^^^^^^^^^^^^^^^^^

Once the channel starts operating and generating the PWM signal with the constant duty cycle and frequency, there are a couple of ways to change this signal. When driving LEDs, primarily the duty cycle is changed to vary the light intensity.

The following two sections describe how to change the duty cycle using software and hardware fading. If required, the signal's frequency can also be changed; it is covered in Section :ref:`ledc-api-change-pwm-frequency`.

.. only:: not esp32

    .. note::

        All the timers and channels in the {IDF_TARGET_NAME}'s LED PWM Controller only support low speed mode. Any change of PWM settings must be explicitly triggered by software (see below).


Change PWM Duty Cycle Using Software
""""""""""""""""""""""""""""""""""""

To set the duty cycle, use the dedicated function :cpp:func:`ledc_set_duty`. After that, call :cpp:func:`ledc_update_duty` to activate the changes. To check the currently set value, use the corresponding ``_get_`` function :cpp:func:`ledc_get_duty`.

Another way to set the duty cycle, as well as some other channel parameters, is by calling :cpp:func:`ledc_channel_config` covered in Section :ref:`ledc-api-configure-channel`.

The range of the duty cycle values passed to functions depends on selected ``duty_resolution`` and should be from ``0`` to ``(2 ** duty_resolution)``. For example, if the selected duty resolution is 10, then the duty cycle values can range from 0 to 1024. This provides the resolution of ~ 0.1%.

.. only:: esp32 or esp32s2 or esp32s3 or esp32c3 or esp32c2 or esp32c6 or esp32h2 or esp32p4

    .. warning::

        On {IDF_TARGET_NAME}, when channel's binded timer selects its maximum duty resolution, the duty cycle value cannot be set to ``(2 ** duty_resolution)``. Otherwise, the internal duty counter in the hardware will overflow and be messed up.


Change PWM Duty Cycle Using Hardware
""""""""""""""""""""""""""""""""""""

The LEDC hardware provides the means to gradually transition from one duty cycle value to another. To use this functionality, enable fading with :cpp:func:`ledc_fade_func_install` and then configure it by calling one of the available fading functions:

* :cpp:func:`ledc_set_fade_with_time`
* :cpp:func:`ledc_set_fade_with_step`
* :cpp:func:`ledc_set_fade`

.. only:: SOC_LEDC_GAMMA_CURVE_FADE_SUPPORTED

    On {IDF_TARGET_NAME}, the hardware additionally allows to perform up to {IDF_TARGET_LEDC_MAX_FADE_RANGE_NUM} consecutive linear fades without CPU intervention. This feature can be useful if you want to do a fade with gamma correction.

    The luminance perceived by human eyes does not have a linear relationship with the PWM duty cycle. In order to make human feel the LED is dimming or lightening linearly, the change in duty cycle should be non-linear, which is the so-called gamma correction. The LED controller can simulate a gamma curve fading by piecewise linear approximation. :cpp:func:`ledc_fill_multi_fade_param_list` is a function that can help to construct the parameters for the piecewise linear fades. First, you need to allocate a memory block for saving the fade parameters, then by providing start/end PWM duty cycle values, gamma correction function, and the total number of desired linear segments to the helper function, it will fill the calculation results into the allocated space. You can also construct the array of :cpp:type:`ledc_fade_param_config_t` manually. Once the fade parameter structs are prepared, a consecutive fading can be configured by passing the pointer to the prepared :cpp:type:`ledc_fade_param_config_t` list and the total number of fade ranges to :cpp:func:`ledc_set_multi_fade`.

.. only:: esp32

    Start fading with :cpp:func:`ledc_fade_start`. A fade can be operated in blocking or non-blocking mode, please check :cpp:enum:`ledc_fade_mode_t` for the difference between the two available fade modes. Note that with either fade mode, the next fade or fixed-duty update will not take effect until the last fade finishes. Due to hardware limitations, there is no way to stop a fade before it reaches its target duty.

.. only:: not esp32

    Start fading with :cpp:func:`ledc_fade_start`. A fade can be operated in blocking or non-blocking mode, please check :cpp:enum:`ledc_fade_mode_t` for the difference between the two available fade modes. Note that with either fade mode, the next fade or fixed-duty update will not take effect until the last fade finishes or is stopped. :cpp:func:`ledc_fade_stop` has to be called to stop a fade that is in progress.

To get a notification about the completion of a fade operation, a fade end callback function can be registered for each channel by calling :cpp:func:`ledc_cb_register` after the fade service being installed. The fade end callback prototype is defined in :cpp:type:`ledc_cb_t`, where you should return a boolean value from the callback function, indicating whether a high priority task is woken up by this callback function. It is worth mentioning, the callback and the function invoked by itself should be placed in IRAM, as the interrupt service routine is in IRAM. :cpp:func:`ledc_cb_register` will print a warning message if it finds the addresses of callback and user context are incorrect.

If not required anymore, fading and an associated interrupt can be disabled with :cpp:func:`ledc_fade_func_uninstall`.


.. _ledc-api-change-pwm-frequency:

Change PWM Frequency
""""""""""""""""""""

The LEDC API provides several ways to change the PWM frequency "on the fly":

    * Set the frequency by calling :cpp:func:`ledc_set_freq`. There is a corresponding function :cpp:func:`ledc_get_freq` to check the current frequency.
    * Change the frequency and the duty resolution by calling :cpp:func:`ledc_bind_channel_timer` to bind some other timer to the channel.
    * Change the channel's timer by calling :cpp:func:`ledc_channel_config`.


More Control Over PWM
"""""""""""""""""""""

There are several lower level timer-specific functions that can be used to change PWM settings:

* :cpp:func:`ledc_timer_set`
* :cpp:func:`ledc_timer_rst`
* :cpp:func:`ledc_timer_pause`
* :cpp:func:`ledc_timer_resume`

The first two functions are called "behind the scenes" by :cpp:func:`ledc_channel_config` to provide a startup of a timer after it is configured.


Use Interrupts
^^^^^^^^^^^^^^

When configuring an LEDC channel, one of the parameters selected within :cpp:type:`ledc_channel_config_t` is :cpp:type:`ledc_intr_type_t` which triggers an interrupt on fade completion.

For registration of a handler to address this interrupt, call :cpp:func:`ledc_isr_register`.


.. only:: esp32

    .. _ledc-api-high_low_speed_mode:

    LEDC High and Low Speed Mode
    ----------------------------

    High speed mode enables a glitch-free changeover of timer settings. This means that if the timer settings are modified, the changes will be applied automatically on the next overflow interrupt of the timer. In contrast, when updating the low-speed timer, the change of settings should be explicitly triggered by software. The LEDC driver handles it in the background, e.g., when :cpp:func:`ledc_timer_config` or :cpp:func:`ledc_timer_set` is called.

    For additional details regarding speed modes, see **{IDF_TARGET_NAME} Technical Reference Manual** > **LED PWM Controller (LEDC)** [`PDF <{IDF_TARGET_TRM_EN_URL}#ledpwm>`__].

    .. _ledc-api-supported-range-frequency-duty-resolution:

.. only:: not esp32

    .. _ledc-api-supported-range-frequency-duty-resolution:

Supported Range of Frequency and Duty Resolutions
-------------------------------------------------

The LED PWM Controller is designed primarily to drive LEDs. It provides a large flexibility of PWM duty cycle settings. For instance, the PWM frequency of 5 kHz can have the maximum duty resolution of 13 bits. This means that the duty can be set anywhere from 0 to 100% with a resolution of ~ 0.012% (2 ** 13 = 8192 discrete levels of the LED intensity). Note, however, that these parameters depend on the clock signal clocking the LED PWM Controller timer which in turn clocks the channel (see :ref:`timer configuration <ledc-api-configure-timer>` and the **{IDF_TARGET_NAME} Technical Reference Manual** > **LED PWM Controller (LEDC)** [`PDF <{IDF_TARGET_TRM_EN_URL}#ledpwm>`__]).

The LEDC can be used for generating signals at much higher frequencies that are sufficient enough to clock other devices, e.g., a digital camera module. In this case, the maximum available frequency is 40 MHz with duty resolution of 1 bit. This means that the duty cycle is fixed at 50% and cannot be adjusted.

The LEDC API is designed to report an error when trying to set a frequency and a duty resolution that exceed the range of LEDC's hardware. For example, an attempt to set the frequency to 20 MHz and the duty resolution to 3 bits results in the following error reported on a serial monitor:

.. highlight:: none

::

    E (196) ledc: requested frequency and duty resolution cannot be achieved, try reducing freq_hz or duty_resolution. div_param=128

In such a situation, either the duty resolution or the frequency must be reduced. For example, setting the duty resolution to 2 resolves this issue and makes it possible to set the duty cycle at 25% steps, i.e., at 25%, 50% or 75%.

The LEDC driver also captures and reports attempts to configure frequency/duty resolution combinations that are below the supported minimum, e.g.,:

::

    E (196) ledc: requested frequency and duty resolution cannot be achieved, try increasing freq_hz or duty_resolution. div_param=128000000

The duty resolution is normally set using :cpp:type:`ledc_timer_bit_t`. This enumeration covers the range from 10 to 15 bits. If a smaller duty resolution is required (from 10 down to 1), enter the equivalent numeric values directly.


Application Example
-------------------

The LEDC basic example: :example:`peripherals/ledc/ledc_basic`.

The LEDC change duty cycle and fading control example: :example:`peripherals/ledc/ledc_fade`.

.. only:: SOC_LEDC_GAMMA_CURVE_FADE_SUPPORTED

    The LEDC color control with Gamma correction on RGB LED example: :example:`peripherals/ledc/ledc_gamma_curve_fade`.

API Reference
-------------

.. include-build-file:: inc/ledc.inc
.. include-build-file:: inc/ledc_types.inc
