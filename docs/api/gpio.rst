GPIO API
========

Overview
--------

`Instructions <http://esp-idf.readthedocs.io/en/latest/api/template.html>`_

Application Example
-------------------

`Instructions <http://esp-idf.readthedocs.io/en/latest/api/template.html>`_

Reference
---------

`Instructions <http://esp-idf.readthedocs.io/en/latest/api/template.html>`_

Enumerations
^^^^^^^^^^^^

.. doxygenenum:: gpio_int_type_t
.. doxygenenum:: gpio_mode_t
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

*Example code:* Configuration of GPIO as an output

.. code-block:: c

    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;             //disable interrupt
    io_conf.mode = GPIO_MODE_OUTPUT;                   //set as output mode
    io_conf.pin_bit_mask = GPIO_SEL_18 | GPIO_SEL_19;  //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pull_down_en = 0;                          //disable pull-down mode
    io_conf.pull_up_en = 0;                            //disable pull-up mode
    gpio_config(&io_conf);                             //configure GPIO with the given settings

*Example code:* Configuration of GPIO as an input

.. code-block:: c

    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_POSEDGE;             //set posedge interrupt
    io_conf.mode = GPIO_MODE_INPUT;                    //set as input
    io_conf.pin_bit_mask = GPIO_SEL_4 | GPIO_SEL_5;    //bit mask of the pins that you want to set, e.g.,GPIO4/5
    io_conf.pull_down_en = 0;                          //disable pull-down mode
    io_conf.pull_up_en = 1;                            //enable pull-up mode
    gpio_config(&io_conf);                             //configure GPIO with the given settings


ROM GPIO functions
^^^^^^^^^^^^^^^^^^

.. doxygenfunction:: gpio_init
.. doxygenfunction:: gpio_output_set
.. doxygenfunction:: gpio_output_set_high
.. doxygenfunction:: gpio_input_get
.. doxygenfunction:: gpio_input_get_high
.. doxygenfunction:: gpio_intr_handler_register
.. doxygenfunction:: gpio_intr_pending
.. doxygenfunction:: gpio_intr_pending_high
.. doxygenfunction:: gpio_intr_ack
.. doxygenfunction:: gpio_intr_ack_high
.. doxygenfunction:: gpio_pin_wakeup_enable
.. doxygenfunction:: gpio_pin_wakeup_disable
.. doxygenfunction:: gpio_matrix_in
.. doxygenfunction:: gpio_matrix_out
.. doxygenfunction:: gpio_pad_select_gpio
.. doxygenfunction:: gpio_pad_set_drv
.. doxygenfunction:: gpio_pad_pullup
.. doxygenfunction:: gpio_pad_pulldown
.. doxygenfunction:: gpio_pad_unhold
.. doxygenfunction:: gpio_pad_hold


