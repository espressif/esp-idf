MCPWM
=====

Overview
--------

ESP32 has two MCPWM units which can be used to control different motors.

Block Diagram
-------------

The block diagram of MCPWM unit is as shown.

::

                __________________________________________________________________________
                |     SYNCSIG              FAULT SIG		CAPTURE SIG               |
                |    0   1   2	           0   1   2        0   1   2                     |
                |___________________________________________________________________   G  |
  INTERRUPTS<-----+ |   |   |             |   |   |         |   |   |               |  P  |
                  | |   |   |             |   |   |         |   |   |               |  I  |
          ________|_|___|___|_____________|___|___|_________|___|___|_________      |  O  |
          |         |   |   |             |   |   |         |   |   |         |     |     |
          |         |   |   |             |   |   |         |   |   |         |     |  M  |
          |         |   |   |           __v___v___v__     __v___v___v__       |     |  A  |
          |         |   |   |           |           |     |           |       |     |  T  |
          |         |   |   |           |   FAULT   |     |  CAPTURE  |       |	    |  R  |
          |         |   |   |           |  HANDLER  |     |           |       |     |  I  |
          |         |   |   |           |           |     |___________|       |     |  X  |
          |         |   |   |           |___________|                         |     |     |
          |         |   |   |                                                 |     |     |
          |	 ____v___v___v____                    ____________________    |     |     |
          |	 |  +---------+  |                    |  +------------+  |--------->|PWM0A|
          |	 |  | Timer 0 |  |                    |  | Operator 0 |  |    |     |     |
          |	 |  +---------+  |                    |  +------------+  |--------->|PWM0B|
          |	 |               |                    |                  |    |     |     |
          |	 |  +---------+  |	      	      |  +------------+  |--------->|PWM1A|
          |	 |  | Timer 1 |  |------------------->|  | Operator 1 |  |    |     |     |
          |	 |  +---------+  |                    |  +------------+  |--------->|PWM1B|
          |	 |               |                    |                  |    |     |     |
          |	 |  +---------+  |                    |  +------------+  |--------->|PWM2A|
          |	 |  | Timer 2 |  |                    |  | Operator 2 |  |    |     |     |
          |	 |  +---------+  |                    |  +------------+  |--------->|PWM2B|
          |	 |_______________|                    |__________________|    |     |_____|
          |                                                                   |
          | MCPWM-UNIT 0/1                                                    |
          |___________________________________________________________________|

			  
Application Example
-------------------

MCPWM different motor example: :example:`peripherals/mcpwm`.

API Reference
-------------

Header Files
^^^^^^^^^^^^

  * :component_file:`driver/include/driver/mcpwm.h`
    
Type Definitions
^^^^^^^^^^^^^^^^


Enumerations
^^^^^^^^^^^^

.. doxygenenum:: mcpwm_io_signals_t
.. doxygenenum:: mcpwm_unit_t
.. doxygenenum:: mcpwm_timer_t
.. doxygenenum:: mcpwm_operator_t
.. doxygenenum:: mcpwm_counter_type_t
.. doxygenenum:: mcpwm_duty_type_t
.. doxygenenum:: mcpwm_carrier_os_t
.. doxygenenum:: mcpwm_carrier_out_ivt_t
.. doxygenenum:: mcpwm_sync_signal_t
.. doxygenenum:: mcpwm_fault_signal_t
.. doxygenenum:: mcpwm_fault_input_level_t
.. doxygenenum:: mcpwm_action_on_pwmxa_t
.. doxygenenum:: mcpwm_action_on_pwmxb_t
.. doxygenenum:: mcpwm_capture_signal_t
.. doxygenenum:: mcpwm_capture_on_edge_t
.. doxygenenum:: mcpwm_deadtime_type_t

Structures
^^^^^^^^^^

.. doxygenstruct:: mcpwm_config_t
    :members:

.. doxygenstruct:: mcpwm_carrier_config_t
    :members:


Functions
^^^^^^^^^

.. doxygenfunction:: mcpwm_gpio_init
.. doxygenfunction:: mcpwm_init
.. doxygenfunction:: mcpwm_set_frequency
.. doxygenfunction:: mcpwm_set_duty
.. doxygenfunction:: mcpwm_set_duty_in_us
.. doxygenfunction:: mcpwm_set_duty_type
.. doxygenfunction:: mcpwm_get_frequency
.. doxygenfunction:: mcpwm_get_duty
.. doxygenfunction:: mcpwm_set_signal_high
.. doxygenfunction:: mcpwm_set_signal_low
.. doxygenfunction:: mcpwm_start
.. doxygenfunction:: mcpwm_stop
.. doxygenfunction:: mcpwm_carrier_init
.. doxygenfunction:: mcpwm_carrier_enable
.. doxygenfunction:: mcpwm_carrier_disable
.. doxygenfunction:: mcpwm_carrier_set_period
.. doxygenfunction:: mcpwm_carrier_set_duty_cycle
.. doxygenfunction:: mcpwm_carrier_oneshot_mode_enable
.. doxygenfunction:: mcpwm_carrier_oneshot_mode_disable
.. doxygenfunction:: mcpwm_carrier_output_invert
.. doxygenfunction:: mcpwm_deadtime_enable
.. doxygenfunction:: mcpwm_deadtime_disable
.. doxygenfunction:: mcpwm_fault_init
.. doxygenfunction:: mcpwm_fault_set_oneshot_mode
.. doxygenfunction:: mcpwm_fault_set_cyc_mode
.. doxygenfunction:: mcpwm_fault_deinit
.. doxygenfunction:: mcpwm_capture_enable
.. doxygenfunction:: mcpwm_capture_disable
.. doxygenfunction:: mcpwm_capture_signal_get_value
.. doxygenfunction:: mcpwm_capture_signal_get_edge
.. doxygenfunction:: mcpwm_sync_enable
.. doxygenfunction:: mcpwm_sync_disable
.. doxygenfunction:: mcpwm_isr_register

