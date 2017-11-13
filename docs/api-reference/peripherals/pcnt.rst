Pulse Counter
=============

Introduction
------------

The PCNT (Pulse Counter) module is designed to count the number of rising and/or falling edges of an input signal. Each pulse counter unit has a 16-bit signed counter register and two channels that can be configured to either increment or decrement the counter. Each channel has a signal input that accepts signal edges to be detected, as well as a control input that can be used to enable or disable the signal input. The inputs have optional filters that can be used to discard unwanted glitches in the signal.


Functionality Overview
----------------------

Description of functionality of this API has been broken down into four sections:

* :ref:`pcnt-api-configuration` - describes counter's configuration parameters and how to setup the counter.
* :ref:`pcnt-api-operating-the-counter` - provides information on control functions to pause, measure and clear the counter. 
* :ref:`pcnt-api-filtering-pulses` - describes options to filtering pulses and the counter control signals.
* :ref:`pcnt-api-using-interrupts` - presents how to trigger interrupts on specific states of the counter. 


.. _pcnt-api-configuration:

Configuration
-------------

The PCNT module has eight independent counting "units" numbered from 0 to 7. In the API they are referred to using :cpp:type:`pcnt_unit_t`. Each unit has two independent channels numbered as 0 and 1 and specified with :cpp:type:`pcnt_channel_t`.

The configuration is provided separately per unit's channel using :cpp:type:`pcnt_config_t` and covers:

    * The unit and the channel number this configuration refers to.
    * GPIO numbers of the pulse input and the pulse gate input.
    * Two pairs of parameters: :cpp:type:`pcnt_ctrl_mode_t` and :cpp:type:`pcnt_count_mode_t` to define how the counter reacts depending on the the status of control signal and how counting is done positive / negative edge of the pulses.
    * Two limit values (minimum / maximum) that are used to establish watchpoints and trigger interrupts when the pulse count is meeting particular limit.

Setting up of particular channel is then done by calling a function :cpp:func:`pcnt_unit_config` with above :cpp:type:`pcnt_config_t` as the input parameter.

To disable the pulse or the control input pin in configuration, provide :cpp:type:`PCNT_PIN_NOT_USED` instead of the GPIO number.


.. _pcnt-api-operating-the-counter:

Operating the Counter
---------------------

After doing setup with :cpp:func:`pcnt_unit_config`, the counter immediately starts to operate. The accumulated pulse count can be checked by calling :cpp:func:`pcnt_get_counter_value`.

There are couple of functions that allow to control the counter's operation: :cpp:func:`pcnt_counter_pause`,  :cpp:func:`pcnt_counter_resume` and :cpp:func:`pcnt_counter_clear`

It is also possible to dynamically change the previously set up counter modes with :cpp:func:`pcnt_unit_config` by calling :cpp:func:`pcnt_set_mode`.

If desired, the pulse input pin and the control input pin may be changed "on the fly" using :cpp:func:`pcnt_set_pin`. To disable particular input provide as a function parameter :cpp:type:`PCNT_PIN_NOT_USED` instead of the GPIO number.

.. note::

    For the counter not to miss any pulses, the pulse duration should be longer than one APB_CLK cycle (12.5 ns). The pulses are sampled on the edges of the APB_CLK clock and may be missed, if fall between the edges. This applies to counter operation with or without a :ref:`filer <pcnt-api-filtering-pulses>`.


.. _pcnt-api-filtering-pulses:

Filtering Pulses
----------------

The PCNT unit features filters on each of the pulse and control inputs, adding the option to ignore short glitches in the signals.

The length of ignored pulses is provided in APB_CLK clock cycles by calling :cpp:func:`pcnt_set_filter_value`. The current filter setting may be checked with :cpp:func:`pcnt_get_filter_value`. The APB_CLK clock is running at 80 MHz.

The filter is put into operation / suspended by calling :cpp:func:`pcnt_filter_enable` / :cpp:func:`pcnt_filter_disable`.


.. _pcnt-api-using-interrupts:

Using Interrupts
----------------

There are five counter state watch events, defined in :cpp:type:`pcnt_evt_type_t`, that are able to trigger an interrupt. The event happens on the pulse counter reaching specific values:

    * Minimum or maximum count values: :cpp:member:`counter_l_lim` or :cpp:member:`counter_h_lim` provided in :cpp:type:`pcnt_config_t` as discussed in :ref:`pcnt-api-configuration`
    * Threshold 0 or Threshold 1 values set using function :cpp:func:`pcnt_set_event_value`.
    * Pulse count = 0

To register, enable or disable an interrupt to service the above events, call :cpp:func:`pcnt_isr_register`, :cpp:func:`pcnt_intr_enable`. and :cpp:func:`pcnt_intr_disable`. To enable or disable events on reaching threshold values, you will also need to call functions :cpp:func:`pcnt_event_enable` and :cpp:func:`pcnt_event_disable`. 

In order to check what are the threshold values currently set, use function :cpp:func:`pcnt_get_event_value`.


Application Example
-------------------

Pulse counter with control signal and event interrupt example: :example:`peripherals/pcnt`.


API Reference
-------------

.. include:: /_build/inc/pcnt.inc

