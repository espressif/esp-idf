LED Control
===========

Introduction
------------

The LED control (LEDC) peripheral is primarily designed to control the intensity of LEDs, although it can also be used to generate PWM signals for other purposes as well. It has 16 channels which can generate independent waveforms that can be used, for example, to drive RGB LED devices. 

A half of LEDC's channels operate in high speed mode. This mode is implemented in hardware and offers automatic and glitch-free changing of the PWM duty cycle. The other half of channels operate in low speed mode, where the moment of change depends on the application software. Each group of channels is also able to use different clock sources, but this feature is not yet supported in the LEDC driver.

The PWM controller can automatically increase or decrease the duty cycle gradually, allowing for fades without any processor interference.


Functionality Overview
----------------------

Getting LEDC to work on a specific channel in either :ref:`high or low speed mode <ledc-api-high_low_speed_mode>` is done in three steps:

1. :ref:`ledc-api-configure-timer` by specifying the PWM signal's frequency and duty cycle resolution.
2. :ref:`ledc-api-configure-channel` by associating it with the timer and GPIO to output the PWM signal.
3. :ref:`ledc-api-change-pwm-signal` that drives the output in order to change LED's intensity. This can be done under the full control of software or with hardware fading functions.

As an optional step, it is also possible to set up an interrupt on the fade end.

.. figure:: ../../../_static/ledc-api-settings.jpg
    :align: center
    :alt: Key Settings of LED PWM Controller's API
    :figclass: align-center

    Key Settings of LED PWM Controller's API


.. _ledc-api-configure-timer:

Configure Timer
^^^^^^^^^^^^^^^

Setting the timer is done by calling the function :cpp:func:`ledc_timer_config` and passing to it a data structure :cpp:type:`ledc_timer_config_t` that contains the following configuration settings:

    - Timer number :cpp:type:`ledc_timer_t`
    - Speed mode :cpp:type:`ledc_mode_t`
    - PWM signal frequency
    - Resolution of PWM duty

The frequency and the duty resolution are interdependent. The higher the PWM frequency, the lower duty resolution is available, and vice versa. This relationship might be important if you are planning to use this API for purposes other than changing the intensity of LEDs. For more details, see Section :ref:`ledc-api-supported-range-frequency-duty-resolution`.


.. _ledc-api-configure-channel:

Configure Channel
^^^^^^^^^^^^^^^^^

When the timer is set up, configure a selected channel (one out of :cpp:type:`ledc_channel_t`). This is done by calling the function :cpp:func:`ledc_channel_config`.

Similar to the timer configuration, the channel setup function should be passed a structure :cpp:type:`ledc_channel_config_t` that contains the channel's configuration parameters.

At this point, the channel should start operating and generating the PWM signal on the selected GPIO, as configured in :cpp:type:`ledc_channel_config_t`, with the frequency specified in the timer settings and the given duty cycle. The channel operation (signal generation) can be suspended at any time by calling the function :cpp:func:`ledc_stop`.


.. _ledc-api-change-pwm-signal:

Change PWM Signal
^^^^^^^^^^^^^^^^^

Once the channel starts operating and generating the PWM signal with the constant duty cycle and frequency, there are a couple of ways to change this signal. When driving LEDs, primarily the duty cycle is changed to vary the light intensity.

The following two sections describe how to change the duty cycle using software and hardware fading. If required, the signal's frequency can also be changed; it is covered in Section :ref:`ledc-api-change-pwm-frequency`.


Change PWM Duty Cycle Using Software
""""""""""""""""""""""""""""""""""""

To set the duty cycle, use the dedicated function :cpp:func:`ledc_set_duty`. After that, call :cpp:func:`ledc_update_duty` to activeate the changes. To check the currently set value, use the corresponding ``_get_`` function :cpp:func:`ledc_get_duty`.

Another way to set the duty cycle, as well as some other channel parameters, is by calling :cpp:func:`ledc_channel_config` covered in Section :ref:`ledc-api-configure-channel`.

The range of the duty cycle values passed to functions depends on selected ``duty_resolution`` and should be from ``0`` to ``(2 ** duty_resolution) - 1``. For example, if the selected duty resolution is 10, then the duty cycle values can range from 0 to 1023. This provides the resolution of ~0.1%.


Change PWM Duty Cycle using Hardware
""""""""""""""""""""""""""""""""""""

The LEDC hardware provides the means to gradually transition from one duty cycle value to another. To use this functionality, enable fading with :cpp:func:`ledc_fade_func_install` and then configure it by calling one of the available fading functions:

* :cpp:func:`ledc_set_fade_with_time`
* :cpp:func:`ledc_set_fade_with_step`
* :cpp:func:`ledc_set_fade`

Finally start fading with :cpp:func:`ledc_fade_start`.

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

The first two functions are called "behind the scenes" by :cpp:func:`ledc_channel_config` to provide a "clean" startup of a timer after it is configured.


Use Interrupts
^^^^^^^^^^^^^^

When configuring an LEDC channel, one of the parameters selected within :cpp:type:`ledc_channel_config_t` is :cpp:type:`ledc_intr_type_t` which triggers an interrupt on fade completion. 

For registration of a handler to address this interrupt, call :cpp:func:`ledc_isr_register`.


.. _ledc-api-high_low_speed_mode:

LEDC High and Low Speed Mode
----------------------------

Of the total 8 timers and 16 channels available in the LED PWM Controller, half of them are dedicated to operation in high speed mode and the other half in low speed mode. Selection of a low or high speed timer or channel is done with the parameter :cpp:type:`ledc_mode_t` that can be found in applicable function calls.

The advantage of high speed mode is hardware-supported, glitch-free changeover of the timer settings. This means that if the timer settings are modified, the changes will be applied automatically on the next overflow interrupt of the timer. In contrast, when updating the low-speed timer, the change of settings should be explicitly triggered by software. The LEDC driver handles it in the background, e.g., when :cpp:func:`ledc_timer_config` or :cpp:func:`ledc_timer_set` is called. 

For additional details regarding speed modes, refer to `ESP32 Technical Reference Manual <https://espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf>`_ (PDF). Please note that the support for ``SLOW_CLOCK`` mentioned in this manual is not yet supported in the LEDC driver.


.. _ledc-api-supported-range-frequency-duty-resolution:

Supported Range of Frequency and Duty Resolutions
-------------------------------------------------

The LED PWM Controller is designed primarily to drive LEDs. It provides a wide resolution for PWM duty cycle settings. For instance, the PWM frequency of 5 kHz can have the maximum duty resolution of 13 bits. It means that the duty can be set anywhere from 0 to 100% with a resolution of ~0.012% (2 ** 13 = 8192 discrete levels of the LED intensity).

The LEDC can be used for generating signals at much higher frequencies that are sufficient enough to clock other devices, e.g., a digital camera module. In this case, the maximum available frequency is 40 MHz with duty resolution of 1 bit. This means that the duty cycle is fixed at 50% and cannot be adjusted.

The LEDC API is designed to report an error when trying to set a frequency and a duty resolution that exceed the range of LEDC's hardware. For example, an attempt to set the frequency to 20 MHz and the duty resolution to 3 bits will result in the following error reported on a serial monitor:

.. highlight:: none

::

    E (196) ledc: requested frequency and duty resolution cannot be achieved, try reducing freq_hz or duty_resolution. div_param=128

In such a situation, either the duty resolution or the frequency must be reduced. For example, setting the duty resolution to 2 will resolve this issue and will make it possible to set the duty cycle at 25% steps, i.e., at 25%, 50% or 75%.

The LEDC driver will also capture and report attempts to configure frequency / duty resolution combinations that are below the supported minimum, e.g.:

::

    E (196) ledc: requested frequency and duty resolution cannot be achieved, try increasing freq_hz or duty_resolution. div_param=128000000

The duty resolution is normally set using :cpp:type:`ledc_timer_bit_t`. This enumeration covers the range from 10 to 15 bits. If a smaller duty resolution is required (from 10 down to 1), enter the equivalent numeric values directly. 


Application Example
-------------------

The LEDC change duty cycle and fading control example: :example:`peripherals/ledc`.


API Reference
-------------

.. include:: /_build/inc/ledc.inc

