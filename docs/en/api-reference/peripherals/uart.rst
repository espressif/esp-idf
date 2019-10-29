UART
====

Overview
--------

A Universal Asynchronous Receiver/Transmitter (UART) is a component known to handle the timing requirements for a variety of widely-adapted interfaces (RS232, RS485, RS422, ...). A UART provides a widely adopted and cheap method to realize full-duplex or half-duplex data exchange among different devices.

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

The driver is identified by :cpp:type:`uart_port_t`, that corresponds to one of the three UART controllers. Such identification is present in all the following function calls.


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
    * Communication mode - :cpp:func:`uart_set_mode` selected out of :cpp:type:`uart_mode_t`

Configuration example: ::

    const int uart_num = UART_NUM_2;
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
        .rx_flow_ctrl_thresh = 122,
    };
    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));


All the above functions have a ``_get_`` equivalent to retrieve the current setting, e.g. :cpp:func:`uart_get_baudrate`.


.. _uart-api-setting-communication-pins:

Setting Communication Pins
^^^^^^^^^^^^^^^^^^^^^^^^^^

In next step, after configuring communication parameters, we are setting physical GPIO pin numbers the other UART will be connected to. This is done in a single step by calling function :cpp:func:`uart_set_pin` and providing it with GPIO numbers, that driver should use for the Tx, Rx, RTS and CTS signals.

Instead of GPIO pin number we can enter a macro :cpp:type:`UART_PIN_NO_CHANGE` and the currently allocated pin will not be changed. The same macro should be entered if certain pin will not be used. ::

    // Set UART pins(TX: IO16 (UART2 default), RX: IO17 (UART2 default), RTS: IO18, CTS: IO19)
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_2, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, 18, 19));

.. _uart-api-driver-installation:

Driver Installation
^^^^^^^^^^^^^^^^^^^

Once configuration of driver is complete, we can install it by calling :cpp:func:`uart_driver_install`. As result several resources required by the UART will be allocated. The type / size of resources are specified as function call parameters and concern:

    * size of the send buffer
    * size of the receive buffer 
    * the event queue handle and size
    * flags to allocate an interrupt

Example: ::

    // Setup UART buffered IO with event queue
    const int uart_buffer_size = (1024 * 2);
    QueueHandle_t uart_queue;
    // Install UART driver using an event queue here
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_2, uart_buffer_size, \
                                            uart_buffer_size, 10, &uart_queue, 0));

If all above steps have been complete, we are ready to connect the other UART device and check the communication.


.. _uart-api-running-uart-communication:

Running UART Communication
^^^^^^^^^^^^^^^^^^^^^^^^^^

The processes of serial communication are under control of UART's hardware FSM. The data to be sent should be put into Tx FIFO buffer, FSM will serialize them and sent out. A similar process, but in reverse order, is done to receive the data. Incoming serial stream is processed by FSM and moved to the Rx FIFO buffer. Therefore the task of API's communication functions is limited to writing and reading the data to / from the respective buffer. This is reflected in some function names, e.g.: :cpp:func:`uart_write_bytes` to transmit the data out, or :cpp:func:`uart_read_bytes` to read the incoming data.


Transmitting
""""""""""""

The basic API function to write the data to Tx FIFO buffer is :cpp:func:`uart_tx_chars`. If the buffer contains not sent characters, this function will write what fits into the empty space and exit reporting the number of bytes actually written.

There is a 'companion' function :cpp:func:`uart_wait_tx_done` that waits until all the data are transmitted out and the Tx FIFO is empty. ::

    // Wait for packet to be sent
    const int uart_num = UART_NUM_2;
    ESP_ERROR_CHECK(uart_wait_tx_done(uart_num, 100)); // wait timeout is 100 RTOS ticks (TickType_t)

An easier to work with function is :cpp:func:`uart_write_bytes`. It sets up an intermediate ring buffer and exits after copying the data to this buffer. When there is an empty space in the FIFO, the data are moved from the ring buffer to the FIFO in the background by an ISR. The code below demonstrates using of this function. ::

    // Write data to UART.
    char* test_str = "This is a test string.\n";
    uart_write_bytes(uart_num, (const char*)test_str, strlen(test_str));

There is a similar function as above that adds a serial break signal after sending the data - :cpp:func:`uart_write_bytes_with_break`. The 'serial break signal' means holding TX line low for period longer than one data frame ::

    // Write data to UART, end with a break signal.
    uart_write_bytes_with_break(uart_num, "test break\n",strlen("test break\n"), 100);


Receiving
"""""""""

To retrieve the data received by UART and saved in Rx FIFO, use function :cpp:func:`uart_read_bytes`. You can check in advance what is the number of bytes available in Rx FIFO by calling :cpp:func:`uart_get_buffered_data_len`. Below is the example of using this function::

    // Read data from UART.
    const int uart_num = UART_NUM_2;
    uint8_t data[128];
    int length = 0;
    ESP_ERROR_CHECK(uart_get_buffered_data_len(uart_num, (size_t*)&length));
    length = uart_read_bytes(uart_num, data, length, 100);

If the data in Rx FIFO is not required and should be discarded, call :cpp:func:`uart_flush`.


Software Flow Control
"""""""""""""""""""""

When the hardware flow control is disabled, then use :cpp:func:`uart_set_rts` and :cpp:func:`uart_set_dtr` to manually set the levels of the RTS and DTR signals.


Communication Mode Selection
""""""""""""""""""""""""""""

The UART controller supports set of communication modes. The selection of mode can be performed using function :cpp:func:`uart_set_mode`. Once the specific mode is selected the UART driver will handle behavior of external peripheral according to mode. As an example it can control RS485 driver chip over RTS line to allow half-duplex RS485 communication. ::

    // Setup UART in rs485 half duplex mode
    ESP_ERROR_CHECK(uart_set_mode(uart_num, UART_MODE_RS485_HALF_DUPLEX));


.. _uart-api-using-interrupts:

Using Interrupts
^^^^^^^^^^^^^^^^

There are nineteen interrupts reported on specific states of UART or on detected errors. The full list of available interrupts is described in `ESP32 Technical Reference Manual <https://espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf>`_ (PDF). To enable specific interrupts call :cpp:func:`uart_enable_intr_mask`, to disable call :cpp:func:`uart_disable_intr_mask`. The mask of all interrupts is available as :cpp:type:`UART_INTR_MASK`. Registration of an handler to service interrupts is done with :cpp:func:`uart_isr_register`, freeing the handler with :cpp:func:`uart_isr_free`. To clear the interrupt status bits once the handler is called use :cpp:func:`uart_clear_intr_status`. 

The API provides a convenient way to handle specific interrupts discussed above by wrapping them into dedicated functions:

* **Event detection** - there are several events defined in :cpp:type:`uart_event_type_t` that may be reported to user application using FreeRTOS queue functionality. You can enable this functionality when calling :cpp:func:`uart_driver_install` described in :ref:`uart-api-driver-installation`. Example how to use it is covered in :example:`peripherals/uart_events`.

* **FIFO space threshold or transmission timeout reached** - the interrupts on TX or Rx FIFO buffer being filled with specific number of characters or on a timeout of sending or receiving data. To use these interrupts, first configure respective threshold values of the buffer length and the timeout by entering them in :cpp:type:`uart_intr_config_t` structure and calling :cpp:func:`uart_intr_config`. Then enable interrupts with functions :cpp:func:`uart_enable_rx_intr` and :cpp:func:`uart_enable_tx_intr`. To disable these interrupts there are corresponding functions :cpp:func:`uart_disable_rx_intr` or :cpp:func:`uart_disable_tx_intr`.

* **Pattern detection** - an interrupt triggered on detecting a 'pattern' of the same character being received/sent repeatedly for a number of times. This functionality is demonstrated in :example:`peripherals/uart/uart_events` example. It may be used e.g. to detect a command string followed by specific number of identical characters (the 'pattern') added at the end of the command string. The functions that allow to configure, enable and disable this interrupt are :cpp:func:`uart_enable_pattern_det_intr` and :cpp:func:`uart_disable_pattern_det_intr`.


Macros
^^^^^^

The API provides several macros to define configuration parameters, e.g. :cpp:type:`UART_FIFO_LEN` to define the length of the hardware FIFO buffers, :cpp:type:`UART_BITRATE_MAX` that gives the maximum baud rate supported by UART, etc.


.. _uart-api-deleting-driver:

Deleting Driver
^^^^^^^^^^^^^^^

If communication is established with :cpp:func:`uart_driver_install` for some specific period of time and then not required, the driver may be removed to free allocated resources by calling :cpp:func:`uart_driver_delete`.


Overview of RS485 specific communication options
-------------------------------------------------

.. note:: Here and below the notation UART_REGISTER.UART_OPTION_BIT will be used to describe register options of UART. See the ESP32 Technical Reference Manual for more information.

- UART_RS485_CONF_REG.UART_RS485_EN = 1, enable RS485 communication mode support.
- UART_RS485_CONF_REG.UART_RS485TX_RX_EN, transmitter's output signal loop back to the receiver's input signal when this bit is set.
- UART_RS485_CONF_REG.UART_RS485RXBY_TX_EN, when bit is set the transmitter should send data when its receiver is busy (remove collisions automatically by hardware). 

The on chip RS485 UART hardware is able to detect signal collisions during transmission of datagram and generate an interrupt UART_RS485_CLASH_INT when it is enabled. The term collision means that during transmission of datagram the received data is different with what has been transmitted out or framing errors exist. Data collisions are usually associated with the presence of other active devices on the bus or due to bus errors. The collision detection feature allows suppressing the collisions when its interrupt is activated and triggered. The UART_RS485_FRM_ERR_INT and UART_RS485_PARITY_ERR_INT interrupts can be used with collision detection feature to control frame errors and parity errors accordingly in RS485 mode. This functionality is supported in the UART driver and can be used with selected UART_MODE_RS485_A mode (see :cpp:func:`uart_set_mode` function). The collision detection option can work with circuit A and circuit C (see below) which allow collision detection. In case of using circuit number A or B, control of RTS pin connected to DE pin of bus driver should be provided manually by application. The function :cpp:func:`uart_get_collision_flag` allows to get collision detection flag from driver. 

The ESP32 UART hardware is not able to control automatically the RTS pin connected to ~RE/DE input of RS485 bus driver to provide half duplex communication. This can be done by UART driver software when UART_MODE_RS485_HALF_DUPLEX mode is selected using :cpp:func:`uart_set_mode` function. The UART driver software automatically asserts the RTS pin (logic 1) once the host writes data to the transmit FIFO, and deasserts RTS pin (logic 0) once the last bit of the data has been transmitted. To use this mode the software would have to disable the hardware flow control function. This mode works with any of used circuit showed below.


Overview of RS485 interface connection options
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. note:: The example schematics below are prepared for just demonstration of basic aspects of RS485 interface connection for ESP32 and may not contain all required elements. The Analog Devices ADM483 & ADM2483 are examples of common RS485 transceivers and other similar transceivers can also be used.

The circuit A: Collision detection circuit 
""""""""""""""""""""""""""""""""""""""""""

::

         VCC ---------------+                             
                            |        
                    +-------x-------+
         RXD <------| R             |
                    |              B|----------<> B
         TXD ------>| D    ADM483   |
 ESP32              |               |     RS485 bus side
         RTS ------>| DE            |
                    |              A|----------<> A
               +----| /RE           |
               |    +-------x-------+
               |            |
              GND          GND

This circuit is preferred because it allows collision detection and is simple enough. The receiver in the line driver is constantly enabled that allows UART  to monitor the RS485 bus. Echo suppression is done by the ESP32 chip hardware when the UART_RS485_CONF_REG.UART_RS485TX_RX_EN bit is enabled.


The circuit B: manual switching of transmitter/receiver without collision detection 
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""" 

::

         VCC ---------------+                             
                            |        
                    +-------x-------+
         RXD <------| R             |
                    |              B|-----------<> B
         TXD ------>| D    ADM483   |
 ESP32              |               |     RS485 bus side
         RTS --+--->| DE            |
               |    |              A|-----------<> A
               +----| /RE           |
                    +-------x-------+
                            |
                           GND

This circuit does not allow collision detection. It suppresses the null bytes receive by hardware when UART_RS485_CONF_REG.UART_RS485TX_RX_EN is set. The bit UART_RS485_CONF_REG.UART_RS485RXBY_TX_EN is not applicable in this case. 


The circuit C: auto switching of transmitter/receiver
""""""""""""""""""""""""""""""""""""""""""""""""""""" 

::

    VCC1<-------------------+-----------+           +-------------------+----> VCC2           
                 10K ____   |           |           |                   | 
                +---|____|--+       +---x-----------x---+   10K  ____   |
                |                   |                   |   +---|____|--+ GND2
  RX <----------+-------------------| RXD               |   | 
                    10K  ____       |                  A|---+---------------<> A (+)  
                +-------|____|------| PV    ADM2483     |   |    ____  120
                |   ____            |                   |   +---|____|---+  RS485 bus side
         VCC1<--+--|____|--+------->| DE                |                |
               10K         |        |                  B|---+------------+--<> B (-)
                        ---+    +-->| /RE               |   |    ____
          10K           |       |   |                   |   +---|____|---+
          ____       | /-C      +---| TXD               |   10K          |
  TX >---|____|--B___|/   NPN   |   |                   |                |
                     |\         |   +---x-----------x---+                |
                     | \-E      |       |           |                    |
                        |       |       |           |                    |
                       GND1    GND1    GND1        GND2                 GND2

This galvanic isolated circuit does not require RTS pin control by software application or driver because it controls transceiver direction automatically. However it requires removing null bytes during transmission by setting UART_RS485_CONF_REG.UART_RS485RXBY_TX_EN = 1, UART_RS485_CONF_REG.UART_RS485TX_RX_EN = 0. This variant can work in any RS485 UART mode or even in UART_MODE_UART.

Application Examples
--------------------

Configure UART settings and install UART driver to read/write using UART1 interface: :example:`peripherals/uart/uart_echo`.

Demonstration of how to report various communication events and how to use pattern detection interrupts: :example:`peripherals/uart/uart_events`.

Transmitting and receiving with the same UART in two separate FreeRTOS tasks: :example:`peripherals/uart/uart_async_rxtxtasks`.

Using synchronous I/O multiplexing for UART file descriptors: :example:`peripherals/uart/uart_select`.

Setup of UART driver to communicate over RS485 interface in half-duplex mode: :example:`peripherals/uart/uart_echo_rs485`. This example is similar to uart_echo but provide communication through RS485 interface chip connected to ESP32 pins.

Demonstration of how to get GPS information by parsing NMEA0183 statements received from GPS via UART peripheral: :example:`peripherals/uart/nmea0183_parser`.

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

