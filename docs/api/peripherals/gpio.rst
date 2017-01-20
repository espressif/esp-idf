GPIO
====

Overview
--------

The ESP32 chip features 40 physical GPIO pads. Some GPIO pads cannot be used or do not have the corresponding pin on the chip package(refer to technical reference manual ). Each pad can be used as a general purpose I/O or can be connected to an internal peripheral signal.
Note that GPIO6-11 are usually used for SPI flash. GPIO34-39 can only be set as input mode.

Application Example
-------------------

GPIO output and input interrupt example: :example:`peripherals/gpio`.

API Reference
-------------

Header Files
^^^^^^^^^^^^

  * :component_file:`driver/include/driver/gpio.h`

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

.. doxygentypedef:: gpio_isr_t
.. doxygentypedef:: gpio_isr_handle_t

Enumerations
^^^^^^^^^^^^

.. doxygenenum:: gpio_num_t
.. doxygenenum:: gpio_int_type_t
.. doxygenenum:: gpio_mode_t
.. doxygenenum:: gpio_pullup_t
.. doxygenenum:: gpio_pulldown_t
.. doxygenenum:: gpio_pull_mode_t

Structures
^^^^^^^^^^

.. doxygenstruct:: gpio_config_t
    :members:


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
.. doxygenfunction:: gpio_pullup_en
.. doxygenfunction:: gpio_pullup_dis
.. doxygenfunction:: gpio_pulldown_en
.. doxygenfunction:: gpio_pulldown_dis
.. doxygenfunction:: gpio_install_isr_service
.. doxygenfunction:: gpio_uninstall_isr_service
.. doxygenfunction:: gpio_isr_handler_add
.. doxygenfunction:: gpio_isr_handler_remove

