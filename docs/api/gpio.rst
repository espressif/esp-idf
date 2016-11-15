GPIO
====

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

  * `driver/include/driver/driver/gpio.h <https://github.com/espressif/esp-idf/blob/master/components/driver/include/driver/gpio.h>`_

Macros
^^^^^^

.. doxygendefine:: GPIO_SEL_0
.. doxygendefine:: GPIO_SEL_1
.. doxygendefine:: GPIO_SEL_2
.. doxygendefine:: GPIO_SEL_3
.. doxygendefine:: GPIO_SEL_4
.. doxygendefine:: GPIO_SEL_5
.. doxygendefine:: GPIO_SEL_6
.. doxygendefine:: GPIO_SEL_7
.. doxygendefine:: GPIO_SEL_8
.. doxygendefine:: GPIO_SEL_9
.. doxygendefine:: GPIO_SEL_10
.. doxygendefine:: GPIO_SEL_11
.. doxygendefine:: GPIO_SEL_12
.. doxygendefine:: GPIO_SEL_13
.. doxygendefine:: GPIO_SEL_14
.. doxygendefine:: GPIO_SEL_15
.. doxygendefine:: GPIO_SEL_16
.. doxygendefine:: GPIO_SEL_17
.. doxygendefine:: GPIO_SEL_18
.. doxygendefine:: GPIO_SEL_19
.. doxygendefine:: GPIO_SEL_21
.. doxygendefine:: GPIO_SEL_22
.. doxygendefine:: GPIO_SEL_23
.. doxygendefine:: GPIO_SEL_25
.. doxygendefine:: GPIO_SEL_26
.. doxygendefine:: GPIO_SEL_27
.. doxygendefine:: GPIO_SEL_32
.. doxygendefine:: GPIO_SEL_33
.. doxygendefine:: GPIO_SEL_34
.. doxygendefine:: GPIO_SEL_35
.. doxygendefine:: GPIO_SEL_36
.. doxygendefine:: GPIO_SEL_37
.. doxygendefine:: GPIO_SEL_38
.. doxygendefine:: GPIO_SEL_39
.. doxygendefine:: GPIO_PIN_REG_0
.. doxygendefine:: GPIO_PIN_REG_1
.. doxygendefine:: GPIO_PIN_REG_2
.. doxygendefine:: GPIO_PIN_REG_3
.. doxygendefine:: GPIO_PIN_REG_4
.. doxygendefine:: GPIO_PIN_REG_5
.. doxygendefine:: GPIO_PIN_REG_6
.. doxygendefine:: GPIO_PIN_REG_7
.. doxygendefine:: GPIO_PIN_REG_8
.. doxygendefine:: GPIO_PIN_REG_9
.. doxygendefine:: GPIO_PIN_REG_10
.. doxygendefine:: GPIO_PIN_REG_11
.. doxygendefine:: GPIO_PIN_REG_12
.. doxygendefine:: GPIO_PIN_REG_13
.. doxygendefine:: GPIO_PIN_REG_14
.. doxygendefine:: GPIO_PIN_REG_15
.. doxygendefine:: GPIO_PIN_REG_16
.. doxygendefine:: GPIO_PIN_REG_17
.. doxygendefine:: GPIO_PIN_REG_18
.. doxygendefine:: GPIO_PIN_REG_19
.. doxygendefine:: GPIO_PIN_REG_20
.. doxygendefine:: GPIO_PIN_REG_21
.. doxygendefine:: GPIO_PIN_REG_22
.. doxygendefine:: GPIO_PIN_REG_23
.. doxygendefine:: GPIO_PIN_REG_25
.. doxygendefine:: GPIO_PIN_REG_26
.. doxygendefine:: GPIO_PIN_REG_27
.. doxygendefine:: GPIO_PIN_REG_32
.. doxygendefine:: GPIO_PIN_REG_33
.. doxygendefine:: GPIO_PIN_REG_34
.. doxygendefine:: GPIO_PIN_REG_35
.. doxygendefine:: GPIO_PIN_REG_36
.. doxygendefine:: GPIO_PIN_REG_37
.. doxygendefine:: GPIO_PIN_REG_38
.. doxygendefine:: GPIO_PIN_REG_39
.. doxygendefine:: GPIO_APP_CPU_INTR_ENA
.. doxygendefine:: GPIO_APP_CPU_NMI_INTR_ENA
.. doxygendefine:: GPIO_PRO_CPU_INTR_ENA
.. doxygendefine:: GPIO_PRO_CPU_NMI_INTR_ENA
.. doxygendefine:: GPIO_SDIO_EXT_INTR_ENA
.. doxygendefine:: GPIO_MODE_DEF_INPUT
.. doxygendefine:: GPIO_MODE_DEF_OUTPUT
.. doxygendefine:: GPIO_MODE_DEF_OD
.. doxygendefine:: GPIO_PIN_COUNT
.. doxygendefine:: GPIO_IS_VALID_GPIO
.. doxygendefine:: GPIO_IS_VALID_OUTPUT_GPIO

Type Definitions
^^^^^^^^^^^^^^^^

.. doxygentypedef:: gpio_event_callback

Enumerations
^^^^^^^^^^^^

.. doxygenenum:: gpio_num_t
.. doxygenenum:: gpio_int_type_t
.. doxygenenum:: gpio_mode_t
.. doxygenenum:: gpio_pullup_t
.. doxygenenum:: gpio_pulldown_t
.. doxygenenum:: gpio_pull_mode_t

Functions
^^^^^^^^^

.. doxygenfunction:: gpio_config
.. doxygenfunction:: gpio_set_intr_type
.. doxygenfunction:: gpio_intr_enable
.. doxygenfunction:: gpio_intr_disable
.. doxygenfunction:: gpio_set_level
.. doxygenfunction:: gpio_get_level
.. doxygenfunction:: gpio_set_direction
.. doxygenfunction:: gpio_set_pull_mode
.. doxygenfunction:: gpio_wakeup_enable
.. doxygenfunction:: gpio_wakeup_disable
.. doxygenfunction:: gpio_isr_register
