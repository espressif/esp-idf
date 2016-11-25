LED Control
===========

Overview
--------

`Instructions`_

Application Example
-------------------

`Instructions`_

API Reference
-------------

`Instructions`_

.. _Instructions: template.html

Header Files
^^^^^^^^^^^^

  * `driver/include/driver/ledc.h <https://github.com/espressif/esp-idf/blob/master/components/driver/include/driver/ledc.h>`_

Data Structures
^^^^^^^^^^^^^^^

.. doxygenstruct:: ledc_channel_config_t
   :members:

.. doxygenstruct:: ledc_timer_config_t
   :members:

Macros
^^^^^^

.. doxygendefine:: LEDC_APB_CLK_HZ
.. doxygendefine:: LEDC_REF_CLK_HZ

Enumerations
^^^^^^^^^^^^

.. doxygenenum:: ledc_mode_t
.. doxygenenum:: ledc_intr_type_t
.. doxygenenum:: ledc_duty_direction_t
.. doxygenenum:: ledc_clk_src_t
.. doxygenenum:: ledc_timer_t
.. doxygenenum:: ledc_channel_t
.. doxygenenum:: ledc_timer_bit_t

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
