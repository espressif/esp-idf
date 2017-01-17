LED Control
===========

Overview
--------

The LED control module is primarily designed to control the intensity of LEDs, although it can be used to generate PWM signals for other purposes as well. 
It has 16 channels which can generate independent waveforms that can be used to drive e.g. RGB LED devices. For maximum flexibility, the high-speed as well 
as the low-speed channels can be driven from one of four high-speed/low-speed timers. The PWM controller also has the ability to automatically increase or 
decrease the duty cycle gradually, allowing for fades without any processor interference.

Application Example
-------------------

LEDC change duty cycle and fading control example: `examples/peripherals/ledc <https://github.com/espressif/esp-idf/tree/master/examples/peripherals/ledc>`_.

API Reference
-------------

Header Files
^^^^^^^^^^^^

  * `driver/include/driver/ledc.h <https://github.com/espressif/esp-idf/blob/master/components/driver/include/driver/ledc.h>`_

Macros
^^^^^^

.. doxygendefine:: LEDC_APB_CLK_HZ
.. doxygendefine:: LEDC_REF_CLK_HZ

Type Definitions
^^^^^^^^^^^^^^^^

.. doxygentypedef:: ledc_isr_handle_t

Enumerations
^^^^^^^^^^^^

.. doxygenenum:: ledc_mode_t
.. doxygenenum:: ledc_intr_type_t
.. doxygenenum:: ledc_duty_direction_t
.. doxygenenum:: ledc_clk_src_t
.. doxygenenum:: ledc_timer_t
.. doxygenenum:: ledc_channel_t
.. doxygenenum:: ledc_timer_bit_t

Structures
^^^^^^^^^^

.. doxygenstruct:: ledc_channel_config_t
    :members:

.. doxygenstruct:: ledc_timer_config_t
    :members:


Functions
^^^^^^^^^

.. doxygenfunction:: ledc_channel_config
.. doxygenfunction:: ledc_timer_config
.. doxygenfunction:: ledc_update_duty
.. doxygenfunction:: ledc_stop
.. doxygenfunction:: ledc_set_freq
.. doxygenfunction:: ledc_get_freq
.. doxygenfunction:: ledc_set_duty
.. doxygenfunction:: ledc_get_duty
.. doxygenfunction:: ledc_set_fade
.. doxygenfunction:: ledc_isr_register
.. doxygenfunction:: ledc_timer_set
.. doxygenfunction:: ledc_timer_rst
.. doxygenfunction:: ledc_timer_pause
.. doxygenfunction:: ledc_timer_resume
.. doxygenfunction:: ledc_bind_channel_timer
.. doxygenfunction:: ledc_set_fade_with_step
.. doxygenfunction:: ledc_set_fade_with_time
.. doxygenfunction:: ledc_fade_func_install
.. doxygenfunction:: ledc_fade_func_uninstall
.. doxygenfunction:: ledc_fade_start

