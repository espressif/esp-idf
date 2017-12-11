UART
====

Overview
--------

An Universal Asynchronous Receiver/Transmitter (UART) is a component known to handle the timing requirements for a variety of widely-adapted protocols (RS232, RS485, RS422, ...). An UART provides a widely adopted and cheap method to realize full-duplex data exchange among different devices.

There are three UART controllers available on the ESP32 chip. They are compatible with UART-enabled devices from various manufacturers. All UART controllers integrated in the ESP32 feature an identical set of registers for ease of programming and flexibility. In this documentation, these controllers are referred to as UART0, UART1, and UART2.


Functional Overview
-------------------

The following overview describes functions and data types used to establish communication between ESP32 and some other UART device. The overview reflects a typical workflow when programming ESP32's UART driver and is broken down into the following sections:

1. :ref:`uart-api-setting-communication-parameters` - baud rate, data bits, stop bits, etc,
2. :ref:`uart-api-setting-communication-pins` - pins the other UART is connected to
3. :ref:`uart-api-driver-installation` - allocate ESP32's resources for the UART driver
4. :ref:`uart-api-running-uart-communication` - send / receive the data
5. :ref:`uart-api-using-interrupts` - trigger interrupts on specific communication events 
6. :ref:`uart-api-deleting-driver` - release ESP32's resources, if UART communication is not required anymore

The minimum to make the UART working is to complete the first four steps, the last two steps are optional.

The driver is identified by :cpp:type:`uart_port_t`, that corresponds to one of the tree UART controllers. Such identification is present in all the following function calls.


.. _uart-api-setting-communication-parameters:

Setting Communication Parameters
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

There are two ways to set the communications parameters for UART. One is to do it in one shot by calling :cpp:func:`uart_param_config` provided with configuration parameters in :cpp:type:`uart_config_t` structure.

The alternate way is to configure specific parameters individually by calling dedicated functions:

    * Baud rate - :cpp:func:`uart_set_baudrate`
    * Number of transmitted bits - :cpp:func:`uart_set_word_length` selected out of :cpp:type:`uart_word_length_t`
    * Parity control - :cpp:func:`uart_set_parity` selected out of :cpp:type:`uart_parity_t`
    * Number of stop bits - :cpp:func:`uart_set_stop_bits` selected out of :cpp:type:`uart_stop_bits_t`
    * Hardware flow control mode - :cpp:func:`uart_set_hw_flow_ctrl` selected out of `uart_hw_flowcontrol_t`

All the above functions have a ``_get_`` equivalent to retrieve the current setting, e.g. :cpp:func:`uart_get_baudrate`.


.. _uart-api-setting-communication-pins:

Setting Communication Pins
^^^^^^^^^^^^^^^^^^^^^^^^^^

In next step, after configuring communication parameters, we are setting physical GPIO pin numbers the other UART will be connected to. This is done in a single step by calling function :cpp:func:`uart_set_pin` and providing it with GPIO numbers, that driver should use for the Tx, Rx, RTS and CTS signals.

Instead of GPIO pin number we can enter a macro :cpp:type:`UART_PIN_NO_CHANGE` and the currently allocated pin will not be changed. The same macro should be entered if certain pin will not be used.


.. _uart-api-driver-installation:

Driver Installation
^^^^^^^^^^^^^^^^^^^

Once configuration of driver is complete, we can install it by calling :cpp:func:`uart_driver_install`. As result several resources required by the UART will be allocated. The type / size of resources are specified as function call parameters and concern:

    * size of the send buffer
    * size of the receive buffer 
    * the event queue handle and size
    * flags to allocate an interrupt

If all above steps have been complete, we are ready to connect the other UART device and check the communication.


.. _uart-api-running-uart-communication:

Running UART Communication
^^^^^^^^^^^^^^^^^^^^^^^^^^

The processes of serial communication are under control of UART's hardware FSM. The data to be sent should be put into Tx FIFO buffer, FSM will serialize them and sent out. A similar process, but in reverse order, is done to receive the data. Incoming serial stream is processed by FSM and moved to the Rx FIFO buffer. Therefore the task of API's communication functions is limited to writing and reading the data to / from the respective buffer. This is reflected in some function names, e.g.: :cpp:func:`uart_write_bytes` to transmit the data out, or :cpp:func:`uart_read_bytes` to read the incoming data.


Transmitting
""""""""""""

The basic API function to write the data to Tx FIFO buffer is :cpp:func:`uart_tx_chars`. If the buffer contains not sent characters, this function will write what fits into the empty space and exit reporting the number of bytes actually written.

There is a 'companion' function :cpp:func:`uart_wait_tx_done` that waits until all the data are transmitted out and the Tx FIFO is empty.

An easier to work with function is :cpp:func:`uart_write_bytes`. It sets up an intermediate ring buffer and exits after copying the data to this buffer. When there is an empty space in the FIFO, the data are moved from the ring buffer to the FIFO in the background by an ISR.

There is a similar function as above that adds a serial break signal after sending the data - :cpp:func:`uart_write_bytes_with_break`. The 'serial break signal' means holding TX line low for period longer than one data frame.


Receiving
"""""""""

To retrieve the data received by UART and saved in Rx FIFO, use function :cpp:func:`uart_read_bytes`. You can check in advance what is the number of bytes available in Rx FIFO by calling :cpp:func:`uart_get_buffered_data_len`.

If the data in Rx FIFO is not required and should be discarded, call :cpp:func:`uart_flush`.


Software Flow Control
"""""""""""""""""""""

When the hardware flow control is disabled, then use :cpp:func:`uart_set_rts` and :cpp:func:`uart_set_dtr` to manually set the levels of the RTS and DTR signals.


.. _uart-api-using-interrupts:

Using Interrupts
^^^^^^^^^^^^^^^^

There are nineteen interrupts reported on specific states of UART or on detected errors. The full list of available interrupts is described in `ESP32 Technical Reference Manual <https://espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf>`_ (PDF). To enable specific interrupts call :cpp:func:`uart_enable_intr_mask`, to disable call :cpp:func:`uart_disable_intr_mask`. The mask of all interrupts is available as :cpp:type:`UART_INTR_MASK`. Registration of an handler to service interrupts is done with :cpp:func:`uart_isr_register`, freeing the handler with :cpp:func:`uart_isr_free`. To clear the interrupt status bits once the handler is called use :cpp:func:`uart_clear_intr_status`. 

The API provides a convenient way to handle specific interrupts discussed above by wrapping them into dedicated functions:

* **Event detection** - there are several events defined in :cpp:type:`uart_event_type_t` that may be reported to user application using FreeRTOS queue functionality. You can enable this functionality when calling :cpp:func:`uart_driver_install` described in :ref:`uart-api-driver-installation`. Example how to use it is covered in :example:`peripherals/uart_events`.

* **FIFO space threshold or transmission timeout reached** - the interrupts on TX or Rx FIFO buffer being filled with specific number of characters or on a timeout of sending or receiving data. To use these interrupts, first configure respective threshold values of the buffer length and the timeout by entering them in :cpp:type:`uart_intr_config_t` structure and calling :cpp:func:`uart_intr_config`. Then enable interrupts with functions :cpp:func:`uart_enable_rx_intr` and :cpp:func:`uart_enable_tx_intr`. To disable these interrupts there are corresponding functions :cpp:func:`uart_disable_rx_intr` or :cpp:func:`uart_disable_tx_intr`.

* **Pattern detection** - an interrupt triggered on detecting a 'pattern' of the same character being sent number of times. The functions that allow to configure, enable and disable this interrupt are :cpp:func:`uart_enable_pattern_det_intr` and uart_disable_pattern_det_intr`.


Macros
^^^^^^

The API provides several macros to define configuration parameters, e.g. :cpp:type:`UART_FIFO_LEN` to define the length of the hardware FIFO buffers, :cpp:type:`UART_BITRATE_MAX` that gives the maximum baud rate supported by UART, etc.


.. _uart-api-deleting-driver:

Deleting Driver
^^^^^^^^^^^^^^^

If communication is established with :cpp:func:`uart_driver_install` for some specific period of time and then not required, the driver may be removed to free allocated resources by calling :cpp:func:`uart_driver_delete`.


Application Examples
--------------------

Configure UART settings and install UART driver to read/write using UART1 interface: :example:`peripherals/uart_echo`.

Demonstration of how to report various communication events and how to use patern detection interrupts: :example:`peripherals/uart_events`.

Transmitting and receiveing with the same UART in two separate FreeRTOS tasks: :example:`peripherals/uart_async_rxtxtasks`.


API Reference
-------------

.. include:: /_build/inc/uart.inc


GPIO Lookup Macros
^^^^^^^^^^^^^^^^^^

You can use macros to specify the **direct** GPIO (UART module connected to pads through direct IO mux without the GPIO mux) number of a UART channel, or vice versa. The pin name can be omitted if the channel of a GPIO number is specified, e.g.:

1. ``UART_NUM_2_TXD_DIRECT_GPIO_NUM`` is the GPIO number of UART channel 2 TXD pin (17);
2. ``UART_GPIO19_DIRECT_CHANNEL`` is the UART channel number of GPIO 19 (channel 0);
3. ``UART_CTS_GPIO19_DIRECT_CHANNEL`` is the UART channel number of GPIO 19, and GPIO 19 must be a CTS pin (channel 0).

.. include:: /_build/inc/uart_channel.inc

