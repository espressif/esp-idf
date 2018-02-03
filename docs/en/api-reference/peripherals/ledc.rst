LED Control
===========

Introduction
------------

The LED control (LEDC) module is primarily designed to control the intensity of LEDs, although it can be used to generate PWM signals for other purposes as well. It has 16 channels which can generate independent waveforms, that can be used to drive e.g. RGB LED devices. 

Half of all LEDC's channels provide high speed mode of operation. This mode offers implemented in hardware, automatic and glitch free change of PWM duty cycle. The other half of channels operate in a low speed mode, where the moment of change depends on the application software. Each group of channels is also able to use different clock sources but this feature is not implemented in the API.

The PWM controller also has the ability to automatically increase or decrease the duty cycle gradually, allowing for fades without any processor interference.


Functionality Overview
----------------------

Getting LEDC to work on specific channel in either :ref:`high or low speed mode <ledc-api-high_low_speed_mode>` is done in three steps:

1. :ref:`ledc-api-configure-timer` to determine PWM signal's frequency and the a number (resolution of duty range).
2. :ref:`ledc-api-configure-channel` by associating it with the timer and GPIO to output the PWM signal.
3. :ref:`ledc-api-change-pwm-signal` that drives the output to change LED's intensity. This may be done under full control by software or with help of hardware fading functions.

In an optional step it is also possible to set up an interrupt on the fade end.

.. figure:: ../../_static/ledc-api-settings.jpg
    :align: center
    :alt: Key Settings of LED PWM Controller's API
    :figclass: align-center

    Key Settings of LED PWM Controller's API


.. _ledc-api-configure-timer:

Configure Timer
^^^^^^^^^^^^^^^

Setting of the timer is done by calling function :cpp:func:`ledc_timer_config`. This function should be provided with a data structure :cpp:type:`ledc_timer_config_t` that contains the following configuration settings:

    * The timer number :cpp:type:`ledc_timer_t` and a speed mode :cpp:type:`ledc_mode_t`. 
    * The PWM signal's frequency and resolution of PWM's duty value changes.

The frequency and the duty resolution are interdependent. The higher the PWM frequency, the lower duty resolution is available and vice versa. This relationship may became important, if you are planning to use this API for purposes other that changing intensity of LEDs. Check section :ref:`ledc-api-supported-range-frequency-duty-resolution` for more details. 


.. _ledc-api-configure-channel:

Configure Channel
^^^^^^^^^^^^^^^^^

Having set up the timer, the next step is to configure selected channel (one out of :cpp:type:`ledc_channel_t`). This is done by calling function :cpp:func:`ledc_channel_config`. 

In similar way, like with the timer configuration, the channel setup function should be provided with specific structure :cpp:type:`ledc_channel_config_t`, that contains channel's configuration parameters.

At this point channel should became operational and start generating PWM signal of frequency determined by the timer settings and the duty on selected GPIO, as configured in :cpp:type:`ledc_channel_config_t`. The channel operation / the signal generation may be suspended at any time by calling function :cpp:func:`ledc_stop`.


.. _ledc-api-change-pwm-signal:

Change PWM Signal
^^^^^^^^^^^^^^^^^

Once the channel is operational and generating the PWM signal of constant duty and frequency, there are couple of ways to change this signal. When driving LEDs we are changing primarily the duty to vary the light intensity. See the two section below how to change the duty by software or with hardware fading. If required, we can change signal's frequency as well and this is covered in section :ref:`ledc-api-change-pwm-frequency`.


Change PWM Duty by Software
"""""""""""""""""""""""""""

Setting of the duty is done by first calling dedicated function :cpp:func:`ledc_set_duty` and then calling :cpp:func:`ledc_update_duty` to make the change effective. To check the value currently set, there is a corresponding ``_get_`` function :cpp:func:`ledc_get_duty`.

Another way to set the duty, and some other channel parameters as well, is by calling :cpp:func:`ledc_channel_config` discussed in the previous section.

The range of the duty value entered into functions depends on selected ``duty_resolution`` and should be from 0 to (2 ** duty_resolution) - 1. For example, if selected duty resolution is 10, then the duty range is from 0 to 1023. This provides the resolution of ~0.1%.


Change PWM Duty with Hardware Fading
""""""""""""""""""""""""""""""""""""

The LEDC hardware provides the means to gradually fade from one duty value to another. To use this functionality first enable fading with :cpp:func:`ledc_fade_func_install`. Then configure it by calling one of available fading functions:

* :cpp:func:`ledc_set_fade_with_time`
* :cpp:func:`ledc_set_fade_with_step`
* :cpp:func:`ledc_set_fade`

Finally start fading with :cpp:func:`ledc_fade_start`.

If not required anymore, fading and associated interrupt may be disabled with :cpp:func:`ledc_fade_func_uninstall`.


.. _ledc-api-change-pwm-frequency:

Change PWM Frequency
""""""""""""""""""""

The LEDC API provides several means to change the PWM frequency "on the fly". 

    * One of options is to call :cpp:func:`ledc_set_freq`. There is a corresponding function :cpp:func:`ledc_get_freq` to check what frequency is currently set. 

    * Another option to change the frequency, and the duty resolution as well, is by calling :cpp:func:`ledc_bind_channel_timer` to bind other timer to the channel. 

    * Finally the channel's timer may be changed by calling :cpp:func:`ledc_channel_config`.


More Control Over PWM
"""""""""""""""""""""

There are couple of lower level timer specific functions, that may be used to provide additional means to change the PWM settings:

* :cpp:func:`ledc_timer_set`
* :cpp:func:`ledc_timer_rst`
* :cpp:func:`ledc_timer_pause`
* :cpp:func:`ledc_timer_resume`

The first two functions are called "behind the scenes" by :cpp:func:`ledc_channel_config` to provide "clean" start up of a timer after is it configured.


Use Interrupts
^^^^^^^^^^^^^^

When configuring a LEDC channel, one of parameters selected within :cpp:type:`ledc_channel_config_t` is :cpp:type:`ledc_intr_type_t` and allows to enable an interrupt on fade completion. 

Registration of a handler to service this interrupt is done by calling :cpp:func:`ledc_isr_register`.


.. _ledc-api-high_low_speed_mode:

LEDC High and Low Speed Mode
----------------------------

Out of the total 8 timers and 16 channels available in the LED PWM Controller, half of them are dedicated to operate in the high speed mode and the other half in the low speed mode. Selection of the low or high speed "capable" timer or the channel is done with parameter :cpp:type:`ledc_mode_t` that is present in applicable function calls. 

The advantage of the high speed mode is h/w supported, glitch-free changeover of the timer settings. This means that if the timer settings are modified, the changes will be applied automatically after the next overflow interrupt of the timer. In contrast, when updating the low-speed timer, the change of settings should be specifically triggered by software. The LEDC API is doing it "behind the scenes", e.g. when :cpp:func:`ledc_timer_config` or :cpp:func:`ledc_timer_set` is called. 

For additional details regarding speed modes please refer to `ESP32 Technical Reference Manual <https://espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf>`_ (PDF). Note that support for ``SLOW_CLOCK`` mentioned in this manual is not implemented in the LEDC API.


.. _ledc-api-supported-range-frequency-duty-resolution:

Supported Range of Frequency and Duty Resolution
------------------------------------------------

The LED PWM Controller is designed primarily to drive LEDs and provides wide resolution of PWM duty settings. For instance for the PWM frequency at 5 kHz, the maximum duty resolution is 13 bits. It means that the duty may be set anywhere from 0 to 100% with resolution of ~0.012% (13 ** 2 = 8192 discrete levels of the LED intensity).

The LEDC may be used for providing signals at much higher frequencies to clock other devices, e.g. a digital camera module. In such a case the maximum available frequency is 40 MHz with duty resolution of 1 bit. This means that duty is fixed at 50% and cannot be adjusted.

The API is designed to report an error when trying to set a frequency and a duty resolution that is out of the range of LEDC's hardware. For example, an attempt to set the frequency at 20 MHz and the duty resolution of 3 bits will result in the following error reported on a serial monitor:

.. highlight:: none

::

    E (196) ledc: requested frequency and duty resolution can not be achieved, try reducing freq_hz or duty_resolution. div_param=128

In such a case either the duty resolution or the frequency should be reduced. For example setting the duty resolution at 2 will resolve this issue and provide possibility to set the duty with 25% steps, i.e. at 25%, 50% or 75%.

The LEDC API will also capture and report an attempt to configure frequency / duty resolution combination that is below the supported minimum, e.g.:

::

    E (196) ledc: requested frequency and duty resolution can not be achieved, try increasing freq_hz or duty_resolution. div_param=128000000

Setting of the duty resolution is normally done using :cpp:type:`ledc_timer_bit_t`. This enumeration covers the range from 10 to 15 bits. If a smaller duty resolution is required (below 10 down to 1), enter the equivalent numeric values directly. 


Application Example
-------------------

The LEDC change duty cycle and fading control example: :example:`peripherals/ledc`.


API Reference
-------------

.. include:: /_build/inc/ledc.inc

