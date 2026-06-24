Universal Asynchronous Receiver/Transmitter (UART)
==================================================

:link_to_translation:`zh_CN:[中文]`

{IDF_TARGET_UART_EXAMPLE_PORT:default = "UART_NUM_1", esp32 = "UART_NUM_2", esp32s3 = "UART_NUM_2"}

Introduction
------------

A Universal Asynchronous Receiver/Transmitter (UART) is a hardware feature that handles communication (i.e., timing requirements and data framing) using widely-adopted asynchronous serial communication interfaces, such as RS232, RS422, and RS485. A UART provides a widely adopted and cheap method to realize full-duplex or half-duplex data exchange among different devices.

The {IDF_TARGET_NAME} chip has {IDF_TARGET_SOC_UART_HP_NUM} UART controllers (also referred to as port), each featuring an identical set of registers to simplify programming and for more flexibility.

Each UART controller is independently configurable with parameters such as baud rate, data bit length, bit ordering, number of stop bits, parity bit, etc. All the regular UART controllers are compatible with UART-enabled devices from various manufacturers and can also support Infrared Data Association (IrDA) protocols.

.. only:: SOC_UART_HAS_LP_UART

    Additionally, the {IDF_TARGET_NAME} chip has one low-power (LP) UART controller. It is the cut-down version of regular UART. Usually, the LP UART controller only support basic UART functionality with a much smaller RAM size, and does not support IrDA or RS485 protocols. For a full list of difference between UART and LP UART, please refer to the **{IDF_TARGET_NAME} Technical Reference Manual** > **UART Controller (UART)** > **Features** [`PDF <{IDF_TARGET_TRM_EN_URL}#uart>`__]).

.. only:: SOC_UHCI_SUPPORTED

    .. toctree::
        :hidden:

        uhci

    The {IDF_TARGET_NAME} chip also supports using DMA with UART. For details, see to :doc:`uhci`.

Functional Overview
-------------------

The overview describes how to establish communication between an {IDF_TARGET_NAME} and other UART devices using the functions and data types of the UART driver. A typical programming workflow is broken down into the sections provided below:

1. :ref:`uart-api-driver-installation` - Allocating {IDF_TARGET_NAME}'s resources for the UART driver
2. :ref:`uart-api-setting-communication-parameters` - Setting baud rate, data bits, stop bits, etc.
3. :ref:`uart-api-setting-communication-pins` - Assigning pins for connection to a device
4. :ref:`uart-api-running-uart-communication` - Sending/receiving data
5. :ref:`uart-api-using-interrupts` - Triggering interrupts on specific communication events
6. :ref:`uart-api-deleting-driver` - Freeing allocated resources if a UART communication is no longer required

Steps 1 to 3 comprise the configuration stage. Step 4 is where the UART starts operating. Steps 5 and 6 are optional.

.. only:: SOC_UART_HAS_LP_UART

    Additionally, when using the LP UART Controller you need to pay attention to :ref:`uart-api-lp-uart-driver`.

The UART driver's functions identify each of the UART controllers using :cpp:type:`uart_port_t`. This identification is needed for all the following function calls.


.. _uart-api-driver-installation:

Install Drivers
^^^^^^^^^^^^^^^^^^^

First of all, install the driver by calling :cpp:func:`uart_driver_install` and specify the following parameters:

- UART port number
- Size of RX ring buffer
- Size of TX ring buffer
- Event queue size
- Pointer to store the event queue handle
- Flags to allocate an interrupt

.. _driver-code-snippet:

The function allocates the required internal resources for the UART driver.

.. code-block:: c

    // Setup UART buffered IO with event queue
    const int uart_buffer_size = (1024 * 2);
    QueueHandle_t uart_queue;
    // Install UART driver using an event queue here
    ESP_ERROR_CHECK(uart_driver_install({IDF_TARGET_UART_EXAMPLE_PORT}, uart_buffer_size, uart_buffer_size, 10, &uart_queue, 0));


.. _uart-api-setting-communication-parameters:

Set Communication Parameters
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

As the next step, UART communication parameters can be configured all in a single step or individually in multiple steps.

Single Step
"""""""""""

Call the function :cpp:func:`uart_param_config` and pass to it a :cpp:type:`uart_config_t` structure. The :cpp:type:`uart_config_t` structure should contain all the required parameters. See the example below.

.. code-block:: c

    const uart_port_t uart_num = {IDF_TARGET_UART_EXAMPLE_PORT};
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

For more information on how to configure the hardware flow control options, please refer to :example:`peripherals/uart/uart_echo`.

.. only:: SOC_UART_SUPPORT_SLEEP_RETENTION

    Additionally, :cpp:member:`uart_config_t::allow_pd` can be set to enable the backup of the UART configuration registers before entering sleep and restore these registers after exiting sleep. This allows the UART to continue working properly after waking up even when the UART module power domain is entirely off during sleep. This option implies an balance between power consumption and memory usage. If the power consumption is not a concern, you can disable this option to save memory.

If glitches may occur on the RX signal, :cpp:member:`uart_config_t::rx_glitch_filt_thresh` can be set to filter the glitches to ensure the correct data is received (note that this feature is not supported on ESP32 and ESP32-S2). The unit of the :cpp:member:`uart_config_t::rx_glitch_filt_thresh` is nanoseconds. The default value is 0, which means no filtering.

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

Set Communication Pins
^^^^^^^^^^^^^^^^^^^^^^^^^^

After setting communication parameters, configure the physical GPIO pins to which the other UART device will be connected. For this, call the function :cpp:func:`uart_set_pin` and specify the GPIO pin numbers to which the driver should route the TX, RX, RTS, CTS, DTR, and DSR signals. If you want to keep a currently allocated pin number for a specific signal, pass the macro :c:macro:`UART_PIN_NO_CHANGE`.

The same macro :c:macro:`UART_PIN_NO_CHANGE` should be specified for pins that will not be used.

.. code-block:: c

  // Set UART pins(TX: IO4, RX: IO5, RTS: IO18, CTS: IO19, DTR: UNUSED, DSR: UNUSED)
  ESP_ERROR_CHECK(uart_set_pin({IDF_TARGET_UART_EXAMPLE_PORT}, 4, 5, 18, 19, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));


.. _uart-api-running-uart-communication:

Run UART Communication
^^^^^^^^^^^^^^^^^^^^^^^^^^

Serial communication is controlled by each UART controller's finite state machine (FSM).

The process of sending data involves the following steps:

1. Write data into TX FIFO buffer
2. FSM serializes the data
3. FSM sends the data out

The process of receiving data is similar, but the steps are reversed:

1. FSM processes an incoming serial stream and parallelizes it
2. FSM writes the data into RX FIFO buffer
3. Read the data from RX FIFO buffer

Therefore, an application only writes and reads data from a specific buffer using :cpp:func:`uart_write_bytes` and :cpp:func:`uart_read_bytes` respectively, and the FSM does the rest.


Transmit Data
"""""""""""""

After preparing the data for transmission, call the function :cpp:func:`uart_write_bytes` and pass the data buffer's address and data length to it. The function copies the data to the TX ring buffer (either immediately or after enough space is available), and then exit. When there is free space in the TX FIFO buffer, an interrupt service routine (ISR) moves the data from the TX ring buffer to the TX FIFO buffer in the background. The code below demonstrates the use of this function.

.. code-block:: c

    // Write data to UART.
    char* test_str = "This is a test string.\n";
    uart_write_bytes(uart_num, (const char*)test_str, strlen(test_str));

The function :cpp:func:`uart_write_bytes_with_break` is similar to :cpp:func:`uart_write_bytes` but adds a serial break signal at the end of the transmission. A 'serial break signal' means holding the TX line low for a period longer than one data frame.

.. code-block:: c

    // Write data to UART, end with a break signal.
    uart_write_bytes_with_break(uart_num, "test break\n",strlen("test break\n"), 100);

Another function for writing data to the TX FIFO buffer is :cpp:func:`uart_tx_chars`. Unlike :cpp:func:`uart_write_bytes`, this function does not block until space is available. Instead, it writes all data which can immediately fit into the hardware TX FIFO, and then return the number of bytes that were written.

There is a 'companion' function :cpp:func:`uart_wait_tx_done` that monitors the status of the TX FIFO buffer and returns once it is empty.

.. code-block:: c

    // Wait for packet to be sent
    const uart_port_t uart_num = {IDF_TARGET_UART_EXAMPLE_PORT};
    ESP_ERROR_CHECK(uart_wait_tx_done(uart_num, 100)); // wait timeout is 100 RTOS ticks (TickType_t)


Receive Data
""""""""""""

Once the data is received by the UART and saved in the RX FIFO buffer, it needs to be retrieved using the function :cpp:func:`uart_read_bytes`. Before reading data, you can check the number of bytes available in the RX FIFO buffer by calling :cpp:func:`uart_get_buffered_data_len`. An example of using these functions is given below.

.. code-block:: c

    // Read data from UART.
    const uart_port_t uart_num = {IDF_TARGET_UART_EXAMPLE_PORT};
    uint8_t data[128];
    int length = 0;
    ESP_ERROR_CHECK(uart_get_buffered_data_len(uart_num, (size_t*)&length));
    length = uart_read_bytes(uart_num, data, length, 100);

If the data in the RX FIFO buffer is no longer needed, you can clear the buffer by calling :cpp:func:`uart_flush`.


Software Flow Control
"""""""""""""""""""""

If the hardware flow control is disabled, you can manually set the RTS and DTR signal levels by using the functions :cpp:func:`uart_set_rts` and :cpp:func:`uart_set_dtr` respectively.


Communication Mode Selection
""""""""""""""""""""""""""""

The UART controller supports a number of communication modes. A mode can be selected using the function :cpp:func:`uart_set_mode`. Once a specific mode is selected, the UART driver handles the behavior of a connected UART device accordingly. As an example, it can control the RS485 driver chip using the RTS line to allow half-duplex RS485 communication.

.. code-block:: bash

    // Setup UART in rs485 half duplex mode
    ESP_ERROR_CHECK(uart_set_mode(uart_num, UART_MODE_RS485_HALF_DUPLEX));


.. _uart-api-using-interrupts:

Use Interrupts
^^^^^^^^^^^^^^^^

There are many interrupts that can be generated depending on specific UART states or detected errors. The full list of available interrupts is provided in *{IDF_TARGET_NAME} Technical Reference Manual* > *UART Controller (UART)* > *UART Interrupts* [`PDF <{IDF_TARGET_TRM_EN_URL}#uart>`__]. You can enable or disable specific interrupts by calling :cpp:func:`uart_enable_intr_mask` or :cpp:func:`uart_disable_intr_mask` respectively.

The UART driver provides a convenient way to handle specific interrupts by wrapping them into corresponding events. Events defined in :cpp:type:`uart_event_type_t` can be reported to a user application using the FreeRTOS queue functionality.

To receive the events that have happened, call :cpp:func:`uart_driver_install` and get the event queue handle returned from the function. Please see the above :ref:`code snippet <driver-code-snippet>` as an example.

The processed events include the following:

- **FIFO overflow** (:cpp:enumerator:`UART_FIFO_OVF`): The RX FIFO can trigger an interrupt when it receives more data than the FIFO can store.

    - (Optional) Configure the full threshold of the FIFO space by entering it in the structure :cpp:type:`uart_intr_config_t` and call :cpp:func:`uart_intr_config` to set the configuration. This can help the data stored in the RX FIFO can be processed timely in the driver to avoid FIFO overflow.
    - Enable the interrupts using the functions :cpp:func:`uart_enable_rx_intr`.
    - Disable these interrupts using the corresponding functions :cpp:func:`uart_disable_rx_intr`.

  .. code-block:: c

      const uart_port_t uart_num = {IDF_TARGET_UART_EXAMPLE_PORT};
      // Configure a UART interrupt threshold and timeout
      uart_intr_config_t uart_intr = {
          .intr_enable_mask = UART_INTR_RXFIFO_FULL | UART_INTR_RXFIFO_TOUT,
          .rxfifo_full_thresh = 100,
          .rx_timeout_thresh = 10,
      };
      ESP_ERROR_CHECK(uart_intr_config(uart_num, &uart_intr));

      // Enable UART RX FIFO full threshold and timeout interrupts
      ESP_ERROR_CHECK(uart_enable_rx_intr(uart_num));

- **Pattern detection** (:cpp:enumerator:`UART_PATTERN_DET`): An interrupt triggered on detecting a 'pattern' of the same character being received/sent repeatedly. It can be used, e.g., to detect a command string with a specific number of identical characters (the 'pattern') at the end. The following functions are available:

    - Configure and enable this interrupt using :cpp:func:`uart_enable_pattern_det_baud_intr`
    - Disable the interrupt using :cpp:func:`uart_disable_pattern_det_intr`

  .. code-block:: c

      //Set UART pattern detect function
      uart_enable_pattern_det_baud_intr(EX_UART_NUM, '+', PATTERN_CHR_NUM, 9, 0, 0);

- **Other events**: The UART driver can report other events such as data receiving (:cpp:enumerator:`UART_DATA`), ring buffer full (:cpp:enumerator:`UART_BUFFER_FULL`), detecting NULL after the stop bit (:cpp:enumerator:`UART_BREAK`), parity check error (:cpp:enumerator:`UART_PARITY_ERR`), and frame error (:cpp:enumerator:`UART_FRAME_ERR`).

The strings inside of brackets indicate corresponding event names. An example of how to handle various UART events can be found in :example:`peripherals/uart/uart_events`.

.. _uart-api-deleting-driver:

Deleting a Driver
^^^^^^^^^^^^^^^^^

If the communication established with :cpp:func:`uart_driver_install` is no longer required, the driver can be removed to free allocated resources by calling :cpp:func:`uart_driver_delete`.


Macros
^^^^^^

The API also defines several macros. For example, :c:macro:`UART_HW_FIFO_LEN` defines the length of hardware FIFO buffers; :c:macro:`UART_BITRATE_MAX` gives the maximum baud rate supported by the UART controllers, etc.

.. only:: SOC_UART_HAS_LP_UART

    .. _uart-api-lp-uart-driver:

    Use LP UART Controller with HP Core
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    The UART driver also supports to control the LP UART controller when the chip is in active mode. The configuration steps for the LP UART are the same as the steps for a normal UART controller, except:

    .. list::

        - The port number for the LP UART controller is defined by :c:macro:`LP_UART_NUM_0`.
        - The available clock sources for the LP UART controller can be found in :cpp:type:`lp_uart_sclk_t`.
        - The size of the hardware FIFO for the LP UART controller is much smaller, which is defined in :c:macro:`SOC_LP_UART_FIFO_LEN`.
        :SOC_LP_GPIO_MATRIX_SUPPORTED: - The GPIO pins for the LP UART controller can only be selected from the LP GPIO pins.
        :not SOC_LP_GPIO_MATRIX_SUPPORTED: - The GPIO pins for the LP UART controller are unalterable, because there is no LP GPIO matrix on the target. Please see **{IDF_TARGET_NAME} Technical Reference Manual** > **IO MUX and GPIO Matrix (GPIO, IO MUX)** > **LP IO MUX Functions List** [`PDF <{IDF_TARGET_TRM_EN_URL}#lp-io-mux-func-list>`__] for the specific pin numbers.


Overview of RS485 Specific Communication 0ptions
------------------------------------------------

.. note::

    The following section uses ``[UART_REGISTER_NAME].[UART_FIELD_BIT]`` to refer to UART register fields/bits. For more information on a specific option bit, see **{IDF_TARGET_NAME} Technical Reference Manual** > **UART Controller (UART)** > **Register Summary** [`PDF <{IDF_TARGET_TRM_EN_URL}#uart-reg-summ>`__]. Use the register name to navigate to the register description and then find the field/bit.

- ``UART_RS485_CONF_REG.UART_RS485_EN``: setting this bit enables RS485 communication mode support.
- ``UART_RS485_CONF_REG.UART_RS485TX_RX_EN``: if this bit is set, the transmitter's output signal loops back to the receiver's input signal.
- ``UART_RS485_CONF_REG.UART_RS485RXBY_TX_EN``: if this bit is set, the transmitter will still be sending data if the receiver is busy (remove collisions automatically by hardware).

The {IDF_TARGET_NAME}'s RS485 UART hardware can detect signal collisions during transmission of a datagram and generate the interrupt ``UART_RS485_CLASH_INT`` if this interrupt is enabled. The term collision means that a transmitted datagram is not equal to the one received on the other end. Data collisions are usually associated with the presence of other active devices on the bus or might occur due to bus errors.

The collision detection feature allows handling collisions when their interrupts are activated and triggered. The interrupts ``UART_RS485_FRM_ERR_INT`` and ``UART_RS485_PARITY_ERR_INT`` can be used with the collision detection feature to control frame errors and parity bit errors accordingly in RS485 mode. This functionality is supported in the UART driver and can be used by selecting the :cpp:enumerator:`UART_MODE_RS485_APP_CTRL` mode (see the function :cpp:func:`uart_set_mode`).

The collision detection feature can work with circuit A and circuit C (see Section `Interface Connection Options`_). Use the function :cpp:func:`uart_get_collision_flag` to check if the collision detection flag has been raised. In the case of using circuit A or B, either DTR or RTS pin can be connected to the DE/~RE pin of the transceiver module to achieve half-duplex communication.

The RS485 half-duplex communication mode is supported by the UART driver and can be activated by selecting the :cpp:enumerator:`UART_MODE_RS485_HALF_DUPLEX` mode calling :cpp:func:`uart_set_mode`. The DTR line is automatically controlled by the hardware directly under RS485 half-duplex mode, while the RTS line is software-controlled by the UART driver. Once the host starts writing data to the TX FIFO buffer, the UART driver automatically asserts the RTS pin (logic 1); once the last bit of the data has been transmitted, the driver de-asserts the RTS pin (logic 0). To use this mode, the software would have to disable the hardware flow control function. Since the switching is made in the interrupt handler, comparing to DTR line, some latency is expected on RTS line.

.. only:: esp32

    .. note::

        On {IDF_TARGET_NAME}, DTR signal is only available on UART0. For other UART ports, you can only connect RTS signal to the DE/~RE pin of the transceiver module.


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
     DTR/RTS ------>| DE            |
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
     DTR/RTS --+--->| DE            |
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

* :example:`peripherals/uart/uart_async_rxtxtasks` demonstrates how to use two asynchronous tasks for communication via the same UART interface, with one task transmitting "Hello world" periodically and the other task receiving and printing data from the UART.
* :example:`peripherals/uart/uart_echo` demonstrates how to use the UART interfaces to echo back any data received on the configured UART.
* :example:`peripherals/uart/uart_echo_rs485` demonstrates how to use the ESP32's UART software driver in RS485 half duplex transmission mode to echo any data it receives on UART port back to the sender in the RS485 network, requiring external connection of bus drivers.
* :example:`peripherals/uart/uart_events` demonstrates how to use the UART driver to handle special UART events, read data from UART0, and echo it back to the monitoring console.
* :example:`peripherals/uart/uart_repl` demonstrates how to use and connect two UARTs, allowing the UART used for stdout to send commands and receive replies from another console UART without human interaction.
* :example:`peripherals/uart/uart_select` demonstrates the use of ``select()`` for synchronous I/O multiplexing on the UART interface, allowing for non-blocking read and write from/to various sources such as UART and sockets, where a ready resource can be served without being blocked by a busy resource.
* :example:`peripherals/uart/nmea0183_parser` demonstrates how to parse NMEA-0183 data streams from GPS/BDS/GLONASS modules using the ESP UART Event driver and ESP event loop library, and output common information such as UTC time, latitude, longitude, altitude, and speed.


API Reference
-------------

.. include-build-file:: inc/uart.inc
.. include-build-file:: inc/uart_wakeup.inc
.. include-build-file:: inc/uart_types.inc


GPIO Lookup Macros
^^^^^^^^^^^^^^^^^^

Some UART ports have dedicated IO_MUX pins to which they are connected directly. These can be useful if you need very high UART baud rates, which means you will have to use IO_MUX pins only. In other cases, any GPIO pin can be used for UART communication by routing the signals through the GPIO matrix. If the UART port has dedicated IO_MUX pins, :c:macro:`UxTXD_GPIO_NUM` and :c:macro:`UxRXD_GPIO_NUM` can be used to find the corresponding IO_MUX pin numbers.

.. include-build-file:: inc/uart_pins.inc
