TIMER
========

Overview
--------

ESP32 chip contains two hardware timer groups, each containing two general-purpose hardware timers. 

They are all 64-bit generic timers based on 16-bit prescalers and 64-bit auto-reload-capable up/down counters.


Application Example
-------------------

64-bit hardware timer example: :example:`peripherals/timer_group`.

API Reference
-------------

Header Files
^^^^^^^^^^^^

  * :component_file:`driver/include/driver/timer.h`

Macros
^^^^^^

.. doxygendefine:: TIMER_BASE_CLK

Type Definitions
^^^^^^^^^^^^^^^^


Enumerations
^^^^^^^^^^^^

.. doxygenenum:: timer_group_t
.. doxygenenum:: timer_idx_t
.. doxygenenum:: timer_count_dir_t
.. doxygenenum:: timer_start_t
.. doxygenenum:: timer_alarm_t
.. doxygenenum:: timer_intr_mode_t
.. doxygenenum:: timer_autoreload_t

Structures
^^^^^^^^^^

.. doxygenstruct:: timer_config_t
    :members:


Functions
^^^^^^^^^

.. doxygenfunction:: timer_get_counter_value
.. doxygenfunction:: timer_get_counter_time_sec
.. doxygenfunction:: timer_set_counter_value
.. doxygenfunction:: timer_start
.. doxygenfunction:: timer_pause
.. doxygenfunction:: timer_set_counter_mode
.. doxygenfunction:: timer_set_auto_reload
.. doxygenfunction:: timer_set_divider
.. doxygenfunction:: timer_set_alarm_value
.. doxygenfunction:: timer_get_alarm_value
.. doxygenfunction:: timer_set_alarm
.. doxygenfunction:: timer_isr_register
.. doxygenfunction:: timer_init
.. doxygenfunction:: timer_get_config
.. doxygenfunction:: timer_group_intr_enable
.. doxygenfunction:: timer_group_intr_disable
.. doxygenfunction:: timer_enable_intr
.. doxygenfunction:: timer_disable_intr

