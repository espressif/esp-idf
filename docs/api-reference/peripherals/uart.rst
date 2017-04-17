UART
====

Overview
--------

`Instructions`_

.. _Instructions: ../template.html

Application Example
-------------------

Configure uart settings and install uart driver to read/write using UART0 and UART1 interfaces: :example:`peripherals/uart`.

API Reference
-------------

Header Files
^^^^^^^^^^^^

  * :component_file:`driver/include/driver/uart.h`

Data Structures
^^^^^^^^^^^^^^^

.. doxygenstruct:: uart_config_t
   :members:

.. doxygenstruct:: uart_intr_config_t
   :members:

.. doxygenstruct:: uart_event_t
   :members:

Macros
^^^^^^

.. doxygendefine:: UART_FIFO_LEN
.. doxygendefine:: UART_INTR_MASK
.. doxygendefine:: UART_LINE_INV_MASK
.. doxygendefine:: UART_BITRATE_MAX
.. doxygendefine:: UART_PIN_NO_CHANGE
.. doxygendefine:: UART_INVERSE_DISABLE
.. doxygendefine:: UART_INVERSE_RXD
.. doxygendefine:: UART_INVERSE_CTS
.. doxygendefine:: UART_INVERSE_TXD
.. doxygendefine:: UART_INVERSE_RTS

Enumerations
^^^^^^^^^^^^

.. doxygenenum:: uart_word_length_t
.. doxygenenum:: uart_stop_bits_t
.. doxygenenum:: uart_port_t
.. doxygenenum:: uart_parity_t
.. doxygenenum:: uart_hw_flowcontrol_t
.. doxygenenum:: uart_event_type_t

Functions
^^^^^^^^^

.. doxygenfunction:: uart_set_word_length
.. doxygenfunction:: uart_get_word_length
.. doxygenfunction:: uart_set_stop_bits
.. doxygenfunction:: uart_get_stop_bits
.. doxygenfunction:: uart_set_parity
.. doxygenfunction:: uart_get_parity
.. doxygenfunction:: uart_set_baudrate
.. doxygenfunction:: uart_get_baudrate
.. doxygenfunction:: uart_set_line_inverse
.. doxygenfunction:: uart_set_hw_flow_ctrl
.. doxygenfunction:: uart_get_hw_flow_ctrl
.. doxygenfunction:: uart_clear_intr_status
.. doxygenfunction:: uart_enable_intr_mask
.. doxygenfunction:: uart_disable_intr_mask
.. doxygenfunction:: uart_enable_rx_intr
.. doxygenfunction:: uart_disable_rx_intr
.. doxygenfunction:: uart_disable_tx_intr
.. doxygenfunction:: uart_enable_tx_intr
.. doxygenfunction:: uart_isr_register
.. doxygenfunction:: uart_set_pin
.. doxygenfunction:: uart_set_rts
.. doxygenfunction:: uart_set_dtr
.. doxygenfunction:: uart_param_config
.. doxygenfunction:: uart_intr_config
.. doxygenfunction:: uart_driver_install
.. doxygenfunction:: uart_driver_delete
.. doxygenfunction:: uart_wait_tx_done
.. doxygenfunction:: uart_tx_chars
.. doxygenfunction:: uart_write_bytes
.. doxygenfunction:: uart_write_bytes_with_break
.. doxygenfunction:: uart_read_bytes
.. doxygenfunction:: uart_flush
.. doxygenfunction:: uart_get_buffered_data_len
.. doxygenfunction:: uart_disable_pattern_det_intr
.. doxygenfunction:: uart_enable_pattern_det_intr

