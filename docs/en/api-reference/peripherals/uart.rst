UART
====

{IDF_TARGET_UART_NUM:default = "UART_NUM_1", esp32 = "UART_NUM_2", esp32s2 = "UART_NUM_1"}

Overview
--------

A Universal Asynchronous Receiver/Transmitter (UART) is a hardware feature that handles communication (i.e., timing requirements and data framing) using widely-adapted asynchronous serial communication interfaces, such as RS232, RS422, RS485. A UART provides a widely adopted and cheap method to realize full-duplex or half-duplex data exchange among different devices.

.. only:: esp32

    The ESP32 chip has three UART controllers (UART0, UART1, and UART2) that feature an identical set of registers for ease of programming and flexibility.

.. only:: esp32s2

    The ESP32-S2 chip has two UART controllers (UART0 and UART1) that feature an identical set of registers for ease of programming and flexibility.

Each UART controller is independently configurable with parameters such as baud rate, data bit length, bit ordering, number of stop bits, parity bit etc. All the controllers are compatible with UART-enabled devices from various manufacturers and can also support Infrared Data Association protocols (IrDA).

Functional Overview
-------------------

The following overview describes how to establish communication between an {IDF_TARGET_NAME} and other UART devices using the functions and data types of the UART driver. The overview reflects a typical programming workflow and is broken down into the sections provided below:

1. :ref:`uart-api-setting-communication-parameters` - Setting baud rate, data bits, stop bits, etc.
2. :ref:`uart-api-setting-communication-pins` - Assigning pins for connection to a device.
3. :ref:`uart-api-driver-installation` - Allocating {IDF_TARGET_NAME}'s resources for the UART driver.
4. :ref:`uart-api-running-uart-communication` - Sending / receiving data
5. :ref:`uart-api-using-interrupts` - Triggering interrupts on specific communication events
6. :ref:`uart-api-deleting-driver` - Freeing allocated resources if a UART communication is no longer required

Steps 1 to 3 comprise the configuration stage. Step 4 is where the UART starts operating. Steps 5 and 6 are optional.

The UART driver's functions identify each of the UART controllers using :cpp:type:`uart_port_t`. This identification is needed for all the following function calls.


.. _uart-api-setting-communication-parameters:

Setting Communication Parameters
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

UART communication parameters can be configured all in a single step or individually in multiple steps.


Single Step
"""""""""""

Call the function :cpp:func:`uart_param_config` and pass to it a :cpp:type:`uart_config_t` structure. The :cpp:type:`uart_config_t` structure should contain all the required parameters. See the example below.

.. code-block:: c

    const int uart_num = {IDF_TARGET_UART_NUM};
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


Multiple Steps
""""""""""""""

Configure specific parameters individually by calling a dedicated function from the table given below. These functions are also useful if re-configuring a single parameter.

.. list-table:: Functions for Configuring specific parameters individually
   :widths: 30 70
   :header-rows: 1

   * - Parameter to Configure
     - Function
   * - Baud rate
     - :cpp:func:`uart_set_baudrate`
   * - Number of transmitted bits
     - :cpp:func:`uart_set_word_length` selected out of :cpp:type:`uart_word_length_t`
   * - Parity control
     - :cpp:func:`uart_set_parity` selected out of :cpp:type:`uart_parity_t`
   * - Number of stop bits
     - :cpp:func:`uart_set_stop_bits` selected out of :cpp:type:`uart_stop_bits_t`
   * - Hardware flow control mode
     - :cpp:func:`uart_set_hw_flow_ctrl` selected out of :cpp:type:`uart_hw_flowcontrol_t`
   * - Communication mode
     - :cpp:func:`uart_set_mode` selected out of :cpp:type:`uart_mode_t`

Each of the above functions has a ``_get_`` counterpart to check the currently set value. For example, to check the current baud rate value, call :cpp:func:`uart_get_baudrate`.


.. _uart-api-setting-communication-pins:

Setting Communication Pins
^^^^^^^^^^^^^^^^^^^^^^^^^^

After setting communication parameters, configure the physical GPIO pins to which the other UART device will be connected. For this, call the function :cpp:func:`uart_set_pin` and specify the GPIO pin numbers to which the driver should route the Tx, Rx, RTS, and CTS signals. If you want to keep a currently allocated pin number for a specific signal, pass the macro :c:macro:`UART_PIN_NO_CHANGE`.

The same macro should be specified for pins that will not be used.

.. only:: esp32

  .. code-block:: c

    // Set UART pins(TX: IO17 (UART2 default), RX: IO16 (UART2 default), RTS: IO18, CTS: IO19)
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_2, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, 18, 19));

.. only:: esp32s2

  .. code-block:: c

    // Set UART pins(TX: IO17 (UART1 default), RX: IO18 (UART1 default), RTS: IO19, CTS: IO20)
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_1, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, 19, 20));


.. _uart-api-driver-installation:

Driver Installation
^^^^^^^^^^^^^^^^^^^

Once the communication pins are set, install the driver by calling :cpp:func:`uart_driver_install` and specify the following parameters:

- Size of Tx ring buffer
- Size of Rx ring buffer
- Event queue handle and size
- Flags to allocate an interrupt

The function will allocate the required internal resources for the UART driver.

.. code-block:: c

    // Setup UART buffered IO with event queue
    const int uart_buffer_size = (1024 * 2);
    QueueHandle_t uart_queue;
    // Install UART driver using an event queue here
    ESP_ERROR_CHECK(uart_driver_install({IDF_TARGET_UART_NUM}, uart_buffer_size, \
                                            uart_buffer_size, 10, &uart_queue, 0));

Once this step is complete, you can connect the external UART device and check the communication.


.. _uart-api-running-uart-communication:

Running UART Communication
^^^^^^^^^^^^^^^^^^^^^^^^^^

Serial communication is controlled by each UART controller's finite state machine (FSM).

The process of sending data involves the following steps:

1. Write data into Tx FIFO buffer
2. FSM serializes the data
3. FSM sends the data out

The process of receiving data is similar, but the steps are reversed:

1. FSM processes an incoming serial stream and parallelizes it
2. FSM writes the data into Rx FIFO buffer
3. Read the data from Rx FIFO buffer

Therefore, an application will be limited to writing and reading data from a respective buffer using :cpp:func:`uart_write_bytes` and :cpp:func:`uart_read_bytes` respectively, and the FSM will do the rest.


Transmitting
""""""""""""

After preparing the data for transmission, call the function :cpp:func:`uart_write_bytes` and pass the data buffer's address and data length to it. The function will copy the data to the Tx ring buffer (either immediately or after enough space is available), and then exit. When there is free space in the Tx FIFO buffer, an interrupt service routine (ISR) moves the data from the Tx ring buffer to the Tx FIFO buffer in the background. The code below demonstrates the use of this function.

.. code-block:: c

    // Write data to UART.
    char* test_str = "This is a test string.\n";
    uart_write_bytes(uart_num, (const char*)test_str, strlen(test_str));

The function :cpp:func:`uart_write_bytes_with_break` is similar to :cpp:func:`uart_write_bytes` but adds a serial break signal at the end of the transmission. A 'serial break signal' means holding the Tx line low for a period longer than one data frame.

.. code-block:: c

    // Write data to UART, end with a break signal.
    uart_write_bytes_with_break(uart_num, "test break\n",strlen("test break\n"), 100);

Another function for writing data to the Tx FIFO buffer is :cpp:func:`uart_tx_chars`. Unlike :cpp:func:`uart_write_bytes`, this function will not block until space is available. Instead, it will write all data which can immediately fit into the hardware Tx FIFO, and then return the number of bytes that were written.

There is a 'companion' function :cpp:func:`uart_wait_tx_done` that monitors the status of the Tx FIFO buffer and returns once it is empty.

.. code-block:: c

    // Wait for packet to be sent
    const int uart_num = {IDF_TARGET_UART_NUM};
    ESP_ERROR_CHECK(uart_wait_tx_done(uart_num, 100)); // wait timeout is 100 RTOS ticks (TickType_t)


Receiving
"""""""""

Once the data is received by the UART and saved in the Rx FIFO buffer, it needs to be retrieved using the function :cpp:func:`uart_read_bytes`. Before reading data, you can check the number of bytes available in the Rx FIFO buffer by calling :cpp:func:`uart_get_buffered_data_len`. An example of using these functions is given below.

.. code-block:: c

    // Read data from UART.
    const int uart_num = {IDF_TARGET_UART_NUM};
    uint8_t data[128];
    int length = 0;
    ESP_ERROR_CHECK(uart_get_buffered_data_len(uart_num, (size_t*)&length));
    length = uart_read_bytes(uart_num, data, length, 100);

If the data in the Rx FIFO buffer is no longer needed, you can clear the buffer by calling :cpp:func:`uart_flush`.


Software Flow Control
"""""""""""""""""""""

If the hardware flow control is disabled, you can manually set the RTS and DTR signal levels by using the functions :cpp:func:`uart_set_rts` and :cpp:func:`uart_set_dtr` respectively.


Communication Mode Selection
""""""""""""""""""""""""""""

The UART controller supports a number of communication modes. A mode can be selected using the function :cpp:func:`uart_set_mode`. Once a specific mode is selected, the UART driver will handle the behavior of a connected UART device accordingly. As an example, it can control the RS485 driver chip using the RTS line to allow half-duplex RS485 communication.

.. code-block:: bash

    // Setup UART in rs485 half duplex mode
    ESP_ERROR_CHECK(uart_set_mode(uart_num, UART_MODE_RS485_HALF_DUPLEX));


.. _uart-api-using-interrupts:

Using Interrupts
^^^^^^^^^^^^^^^^

There are many interrupts that can be generated following specific UART states or detected errors. The full list of available interrupts is provided  `{IDF_TARGET_NAME} Technical Reference Manual <{IDF_TARGET_TRM_EN_URL}>`_ (PDF).. You can enable or disable specific interrupts by calling :cpp:func:`uart_enable_intr_mask` or :cpp:func:`uart_disable_intr_mask` respectively. The mask of all interrupts is available as :c:macro:`UART_INTR_MASK`.

By default, the :cpp:func:`uart_driver_install` function installs the driver's internal interrupt handler to manage the Tx and Rx ring buffers and provides high-level API functions like events (see below). It is also possible to register a lower level interrupt handler instead using :cpp:func:`uart_isr_register`, and to free it again using :cpp:func:`uart_isr_free`. Some UART driver functions which use the Tx and Rx ring buffers, events, etc. will not automatically work in this case - it is necessary to handle the interrupts directly in the ISR. Inside the custom handler implementation, clear the interrupt status bits using :cpp:func:`uart_clear_intr_status`.

The API provides a convenient way to handle specific interrupts discussed in this document by wrapping them into dedicated functions:

- **Event detection**: There are several events defined in :cpp:type:`uart_event_type_t` that may be reported to a user application using the FreeRTOS queue functionality. You can enable this functionality when calling :cpp:func:`uart_driver_install` described in :ref:`uart-api-driver-installation`. An example of using Event detection can be found in :example:`peripherals/uart/uart_events`.

- **FIFO space threshold or transmission timeout reached**: The Tx and Rx FIFO buffers can trigger an interrupt when they are filled with a specific number of characters, or on a timeout of sending or receiving data. To use these interrupts, do the following:

    - Configure respective threshold values of the buffer length and timeout by entering them in the structure :cpp:type:`uart_intr_config_t` and calling :cpp:func:`uart_intr_config`
    - Enable the interrupts using the functions :cpp:func:`uart_enable_tx_intr` and :cpp:func:`uart_enable_rx_intr`
    - Disable these interrupts using the corresponding functions :cpp:func:`uart_disable_tx_intr` or :cpp:func:`uart_disable_rx_intr`

- **Pattern detection**: An interrupt triggered on detecting a 'pattern' of the same character being received/sent repeatedly for a number of times. This functionality is demonstrated in the example :example:`peripherals/uart/uart_events`. It can be used, e.g., to detect a command string followed by a specific number of identical characters (the 'pattern') added at the end of the command string. The following functions are available:

    - Configure and enable this interrupt using :cpp:func:`uart_enable_pattern_det_intr`
    - Disable the interrupt using :cpp:func:`uart_disable_pattern_det_intr`


Macros
^^^^^^

The API also defines several macros. For example, :c:macro:`UART_FIFO_LEN` defines the length of hardware FIFO buffers; :c:macro:`UART_BITRATE_MAX` gives the maximum baud rate supported by the UART controllers, etc.


.. _uart-api-deleting-driver:

Deleting a Driver
^^^^^^^^^^^^^^^^^

If the communication established with :cpp:func:`uart_driver_install` is no longer required, the driver can be removed to free allocated resources by calling :cpp:func:`uart_driver_delete`.


Overview of RS485 specific communication options
------------------------------------------------

.. note::

    The following section will use ``[UART_REGISTER_NAME].[UART_FIELD_BIT]`` to refer to UART register fields/bits. To find more information on a specific option bit, open the Register Summary section of the SoC Technical Reference Manual. Use the register name to navigate to the register description and then find the field/bit.

- ``UART_RS485_CONF_REG.UART_RS485_EN``: setting this bit enables RS485 communication mode support.
- ``UART_RS485_CONF_REG.UART_RS485TX_RX_EN``: if this bit is set, the transmitter's output signal loops back to the receiver's input signal.
- ``UART_RS485_CONF_REG.UART_RS485RXBY_TX_EN``: if this bit is set, the transmitter will still be sending data if the receiver is busy (remove collisions automatically by hardware).

The {IDF_TARGET_NAME}'s RS485 UART hardware can detect signal collisions during transmission of a datagram and generate the interrupt ``UART_RS485_CLASH_INT`` if this interrupt is enabled. The term collision means that a transmitted datagram is not equal to the one received on the other end. Data collisions are usually associated with the presence of other active devices on the bus or might occur due to bus errors.

The collision detection feature allows handling collisions when their interrupts are activated and triggered. The interrupts ``UART_RS485_FRM_ERR_INT`` and ``UART_RS485_PARITY_ERR_INT`` can be used with the collision detection feature to control frame errors and parity bit errors accordingly in RS485 mode. This functionality is supported in the UART driver and can be used by selecting the :cpp:enumerator:`UART_MODE_RS485_APP_CTRL` mode (see the function :cpp:func:`uart_set_mode`).

The collision detection feature can work with circuit A and circuit C (see Section `Interface Connection Options`_). In the case of using circuit A or B, the RTS pin connected to the DE pin of the bus driver should be controlled by the user application. Use the function :cpp:func:`uart_get_collision_flag` to check if the collision detection flag has been raised.

The {IDF_TARGET_NAME} UART controllers themselves do not support half-duplex communication as they cannot provide automatic control of the RTS pin connected to the ~RE/DE input of RS485 bus driver. However, half-duplex communication can be achieved via software control of the RTS pin by the UART driver. This can be enabled by selecting the :cpp:enumerator:`UART_MODE_RS485_HALF_DUPLEX` mode when calling :cpp:func:`uart_set_mode`.

Once the host starts writing data to the Tx FIFO buffer, the UART driver automatically asserts the RTS pin (logic 1); once the last bit of the data has been transmitted, the driver de-asserts the RTS pin (logic 0). To use this mode, the software would have to disable the hardware flow control function. This mode works with all the used circuits shown below.


Interface Connection Options
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This section provides example schematics to demonstrate the basic aspects of {IDF_TARGET_NAME}'s RS485 interface connection.

.. note::

    - The schematics below do **not** necessarily contain **all required elements**.

    - The **analog devices** ADM483 & ADM2483 are examples of common RS485 transceivers and **can be replaced** with other similar transceivers.


Circuit A: Collision Detection Circuit
""""""""""""""""""""""""""""""""""""""

.. code-block:: none

         VCC ---------------+
                            |
                    +-------x-------+
         RXD <------| R             |
                    |              B|----------<> B
         TXD ------>| D    ADM483   |
 ESP                |               |     RS485 bus side
         RTS ------>| DE            |
                    |              A|----------<> A
               +----| /RE           |
               |    +-------x-------+
               |            |
              GND          GND

This circuit is preferable because it allows for collision detection and is quite simple at the same time. The receiver in the line driver is constantly enabled, which allows the UART to monitor the RS485 bus. Echo suppression is performed by the UART peripheral when the bit ``UART_RS485_CONF_REG.UART_RS485TX_RX_EN`` is enabled.


Circuit B: Manual Switching Transmitter/Receiver Without Collision Detection
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""


.. code-block:: none

         VCC ---------------+
                            |
                    +-------x-------+
         RXD <------| R             |
                    |              B|-----------<> B
         TXD ------>| D    ADM483   |
 ESP                |               |     RS485 bus side
         RTS --+--->| DE            |
               |    |              A|-----------<> A
               +----| /RE           |
                    +-------x-------+
                            |
                           GND

This circuit does not allow for collision detection. It suppresses the null bytes that the hardware receives when the bit ``UART_RS485_CONF_REG.UART_RS485TX_RX_EN`` is set. The bit ``UART_RS485_CONF_REG.UART_RS485RXBY_TX_EN`` is not applicable in this case.


Circuit C: Auto Switching Transmitter/Receiver
""""""""""""""""""""""""""""""""""""""""""""""

.. code-block:: none

   VCC1 <-------------------+-----------+           +-------------------+----> VCC2
                 10K ____   |           |           |                   |
                +---|____|--+       +---x-----------x---+    10K ____   |
                |                   |                   |   +---|____|--+
  RX <----------+-------------------| RXD               |   |
                     10K ____       |                  A|---+---------------<> A (+)
                +-------|____|------| PV    ADM2483     |   |    ____  120
                |   ____            |                   |   +---|____|---+  RS485 bus side
        VCC1 <--+--|____|--+------->| DE                |                |
                10K        |        |                  B|---+------------+--<> B (-)
                        ---+    +-->| /RE               |   |    ____
           10K          |       |   |                   |   +---|____|---+
          ____       | /-C      +---| TXD               |    10K         |
  TX >---|____|--+_B_|/   NPN   |   |                   |                |
                     |\         |   +---x-----------x---+                |
                     | \-E      |       |           |                    |
                        |       |       |           |                    |
                       GND1    GND1    GND1        GND2                 GND2

This galvanically isolated circuit does not require RTS pin control by a software application or driver because it controls the transceiver direction automatically. However, it requires suppressing null bytes during transmission by setting ``UART_RS485_CONF_REG.UART_RS485RXBY_TX_EN`` to 1 and ``UART_RS485_CONF_REG.UART_RS485TX_RX_EN`` to 0. This setup can work in any RS485 UART mode or even in :cpp:enumerator:`UART_MODE_UART`.


Application Examples
--------------------

The table below describes the code examples available in the directory :example:`peripherals/uart/`.

.. list-table::
   :widths: 35 65
   :header-rows: 1

   * - Code Example
     - Description
   * - :example:`peripherals/uart/uart_echo`
     - Configuring UART settings, installing the UART driver, and reading/writing over the UART1 interface.
   * - :example:`peripherals/uart/uart_events`
     - Reporting various communication events, using pattern detection interrupts.
   * - :example:`peripherals/uart/uart_async_rxtxtasks`
     - Transmitting and receiving data in two separate FreeRTOS tasks over the same UART.
   * - :example:`peripherals/uart/uart_select`
     - Using synchronous I/O multiplexing for UART file descriptors.
   * - :example:`peripherals/uart/uart_echo_rs485`
     - Setting up UART driver to communicate over RS485 interface in half-duplex mode. This example is similar to :example:`peripherals/uart/uart_echo` but allows communication through an RS485 interface chip connected to {IDF_TARGET_NAME} pins.
   * - :example:`peripherals/uart/nmea0183_parser`
     - Obtaining GPS information by parsing NMEA0183 statements received from GPS via the UART peripheral.


API Reference
-------------

.. include-build-file:: inc/uart.inc
.. include-build-file:: inc/uart_types.inc


GPIO Lookup Macros
^^^^^^^^^^^^^^^^^^

The UART peripherals have dedicated IO_MUX pins to which they are connected directly. However, signals can also be routed to other pins using the less direct GPIO matrix. To use direct routes, you need to know which pin is a dedicated IO_MUX pin for a UART channel. GPIO Lookup Macros simplify the process of finding and assigning IO_MUX pins. You choose a macro based on either the IO_MUX pin number, or a required UART channel name, and the macro will return the matching counterpart for you. See some examples below.

.. note::

    These macros are useful if you need very high UART baud rates (over 40 MHz), which means you will have to use IO_MUX pins only. In other cases, these macros can be ignored, and you can use the GPIO Matrix as it allows you to configure any GPIO pin for any UART function.

1. :c:macro:`UART_NUM_2_TXD_DIRECT_GPIO_NUM` returns the IO_MUX pin number of UART channel 2 TXD pin (pin 17)
2. :c:macro:`UART_GPIO19_DIRECT_CHANNEL` returns the UART number of GPIO 19 when connected to the UART peripheral via IO_MUX (this is UART_NUM_0)
3. :c:macro:`UART_CTS_GPIO19_DIRECT_CHANNEL` returns the UART number of GPIO 19 when used as the UART CTS pin via IO_MUX (this is UART_NUM_0). Similar to the above macro but specifies the pin function which is also part of the IO_MUX assignment.

.. include-build-file:: inc/uart_channel.inc

