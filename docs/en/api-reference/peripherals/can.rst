Controller Area Network (CAN)
=============================

.. -------------------------------- Overview -----------------------------------

Overview
--------

The ESP32's peripherals contains a CAN Controller that supports Standard Frame Format (11-bit ID) and Extended Frame Format (29-bit ID) of the CAN2.0B specification.

.. warning::
    The ESP32 CAN controller is not compatible with CAN FD frames and will interpret such frames as errors.

This programming guide is split into the following sections:

    1. :ref:`basic-can-concepts`

    2. :ref:`signals-lines-and-transceiver`

    3. :ref:`configuration`

    4. :ref:`driver-operation`

    5. :ref:`examples`


.. --------------------------- Basic CAN Concepts ------------------------------

.. _basic-can-concepts:

Basic CAN Concepts
------------------

.. note::
    The following section only covers the basic aspects of CAN. For full details, see the CAN2.0B specification

The CAN protocol is a multi-master, multi-cast communication protocol with error detection/signalling and inbuilt message prioritization. The CAN protocol is commonly used as a communication bus in automotive applications.

**Multi-master:** Any node in a CAN bus is allowed initiate the transfer of data.

**Multi-cast:** When a node transmits a message, all nodes are able to receive the message (broadcast). However some nodes can selective choose which messages to accept via the use of acceptance filtering (multi-cast).

**Error Detection and Signalling:** Every CAN node will constantly monitor the CAN bus. When any node detects an error, it will signal the error by transmitting an error frame. Other nodes will receive the error frame and transmit their own error frames in response. This will result in an error detection being propagated to all nodes on the bus.

**Message Priorities:** If two nodes attempt to transmit simultaneously, the node transmitting the message with the lower ID will win arbitration. All other nodes will become receivers ensuring there is at most one transmitter at any time.

CAN Message Frames
^^^^^^^^^^^^^^^^^^

The CAN2.0B specification contains two frame formats known as **Extended Frame** and **Standard Frame** which contain 29-bit IDs and 11-bit IDs respectively. A CAN message consists of the following components

    - 29-bit or 11-bit ID
    - Data Length Code (DLC) between 0 to 8
    - Up to 8 bytes of data (should match DLC)

Error States and Counters
^^^^^^^^^^^^^^^^^^^^^^^^^

The CAN2.0B specification implements fault confinement by requiring every CAN node to maintain two internal error counters known as the **Transmit Error Counter (TEC)** and the **Receive Error Counter (REC)**. The two error counters are used to determine a CAN node's **error state**, and the counters are incremented and decremented following a set of rules (see CAN2.0B specification). These error states are known as **Error Active**, **Error Passive**, and **Bus-Off**.

**Error Active:** A CAN node is Error Active when **both TEC and REC are less than 128** and indicates a CAN node is operating normally. Error Active nodes are allowed to participate in CAN bus activities, and will actively signal any error conditions it detects by transmitting an **Active Error Flag** over the CAN bus.

**Error Passive:** A CAN node is Error Passive when **either the TEC or REC becomes greater than or equal to 128**. Error Passive nodes are still able to take part in CAN bus activities, but will instead transmit a **Passive Error Flag** upon detection of an error.

**Bus-Off:** A CAN node becomes Bus-Off when the **TEC becomes greater than or equal to 256**. A Bus-Off node is unable take part in CAN bus activity and will remain so until it undergoes bus recovery.


.. ---------------------- Signal Lines and Transceiver -------------------------

.. _signals-lines-and-transceiver:

Signals Lines and Transceiver
-----------------------------

The CAN controller does not contain a internal transceiver and therefore **requires an external transceiver** to operate. The type of external transceiver will depend on the application's physical layer specification (e.g. using SN65HVD23X transceivers for ISO 11898-2 compatibility).

The CAN controller's interface consists of 4 signal lines known as **TX, RX, BUS-OFF, and CLKOUT**. These four signal lines can be routed through the GPIO Matrix to GPIOs.

.. blockdiag:: ../../../_static/diagrams/can/can_controller_signals.diag
    :caption: Signal lines of the CAN controller
    :align: center

**TX and RX:** The TX and RX signal lines are required to interface with an external CAN transceiver. Both signal lines represent/interpret a dominant bit as a low logic level (0V), and a recessive bit as a high logic level (3.3V).

**BUS-OFF:** The BUS-OFF signal line is **optional** and is set to a low logic level (0V) whenever the CAN controller reaches a bus-off state. The BUS-OFF signal line is set to a high logic level (3.3V) otherwise.

**CLKOUT:** The CLKOUT signal line is **optional** and outputs a prescaled version of the CAN controller's source clock (APB Clock).

.. note::
    An external transceiver **must internally tie the TX input and the RX output** such that a change in logic level to the TX signal line can be observed on the RX line. Failing to do so will cause the CAN controller to interpret differences in logic levels between the two signal lines as a lost in arbitration or a bit error.


.. ------------------------------ Configuration --------------------------------

.. _configuration:

Configuration
-------------

Operating Modes
^^^^^^^^^^^^^^^

The CAN driver supports the following modes of operations:

**Normal Mode:** The normal operating mode allows the CAN controller to take part in bus activities such as transmitting and receiving messages/error frames. Acknowledgement from another CAN node is required when transmitting message frames.

**No Ack Mode:** The No Acknowledgement mode is similar to normal mode, however acknowledgements are not required when transmitting message frames. This mode is useful when self testing the CAN controller.

**Listen Only Mode:** This mode will prevent the CAN controller from taking part in bus activities. Therefore transmissions of messages/acknowledgement/error frames will be disabled. However the the CAN controller will still be able to receive messages (without acknowledging). This mode is suited for applications such as CAN bus monitoring.

Alerts
^^^^^^

The CAN driver contains an alert feature which is used to notify the application level of certain CAN driver events. Alerts are selectively enabled when the CAN driver is installed, but can be reconfigured during runtime by calling :cpp:func:`can_reconfigure_alerts`. The application can then wait for any enabled alerts to occur by calling :cpp:func:`can_read_alerts`. The CAN driver supports the following alerts:

+------------------------------------+-----------------------------------------+
| Alert                              | Description                             |
+====================================+=========================================+
| ``CAN_ALERT_TX_IDLE``              | No more messages queued for             |
|                                    | transmission                            |
+------------------------------------+-----------------------------------------+
| ``CAN_ALERT_TX_SUCCESS``           | The previous transmission was           |
|                                    | successful                              |
+------------------------------------+-----------------------------------------+
| ``CAN_ALERT_BELOW_ERR_WARN``       | Both error counters have dropped below  |
|                                    | error warning limit                     |
+------------------------------------+-----------------------------------------+
| ``CAN_ALERT_ERR_ACTIVE``           | CAN controller has become error active  |
+------------------------------------+-----------------------------------------+
| ``CAN_ALERT_RECOVERY_IN_PROGRESS`` | CAN controller is undergoing bus        |
|                                    | recovery                                |
+------------------------------------+-----------------------------------------+
| ``CAN_ALERT_BUS_RECOVERED``        | CAN controller has successfully         |
|                                    | completed bus recovery                  |
+------------------------------------+-----------------------------------------+
| ``CAN_ALERT_ARB_LOST``             | The previous transmission lost          |
|                                    | arbitration                             |
+------------------------------------+-----------------------------------------+
| ``CAN_ALERT_ABOVE_ERR_WARN``       | One of the error counters have exceeded |
|                                    | the error warning limit                 |
+------------------------------------+-----------------------------------------+
| ``CAN_ALERT_BUS_ERROR``            | A (Bit, Stuff, CRC, Form, ACK) error    |
|                                    | has occurred on the bus                 |
+------------------------------------+-----------------------------------------+
| ``CAN_ALERT_TX_FAILED``            | The previous transmission has failed    |
+------------------------------------+-----------------------------------------+
| ``CAN_ALERT_RX_QUEUE_FULL``        | The RX queue is full causing a received |
|                                    | frame to be lost                        |
+------------------------------------+-----------------------------------------+
| ``CAN_ALERT_ERR_PASS``             | CAN controller has become error passive |
+------------------------------------+-----------------------------------------+
| ``CAN_ALERT_BUS_OFF``              | Bus-off condition occurred. CAN         |
|                                    | controller can no longer influence bus  |
+------------------------------------+-----------------------------------------+

.. note::
    The **error warning limit** can be used to preemptively warn the application of bus errors before the error passive state is reached. By default the CAN driver sets the **error warning limit** to **96**. The ``CAN_ALERT_ABOVE_ERR_WARN`` is raised when the TEC or REC becomes larger then or equal to the error warning limit. The ``CAN_ALERT_BELOW_ERR_WARN`` is raised when both TEC and REC return back to values below **96**.

.. note::
    When enabling alerts, the ``CAN_ALERT_AND_LOG`` flag can be used to cause the CAN driver to log any raised alerts to UART. The ``CAN_ALERT_ALL`` and ``CAN_ALERT_NONE`` macros can also be used to enable/disable all alerts during configuration/reconfiguration.

Bit Timing
^^^^^^^^^^

The operating bit rate of the CAN controller is configured using the :cpp:type:`can_timing_config_t` structure. The period of each bit is made up of multiple **time quanta**, and the period of a **time quanta** is determined by a prescaled version of the CAN controller's source clock. A single bit contains the following segments in the following order:

    1. The **Synchronization Segment** consists of a single time quanta
    2. **Timing Segment 1** consists of 1 to 16 time quanta before sample point
    3. **Timing Segment 2** consists of 1 to 8 time quanta after sample point

The **Baudrate Prescaler** is used to determine the period of each time quanta by dividing the CAN controller's source clock (80 MHz APB clock). The ``brp`` can be **any even number from 2 to 128**. If the ESP32 is a revision 2 or later chip, the ``brp`` will also support **any multiple of 4 from 132 to 256**, and can be enabled by setting the :ref:`CONFIG_ESP32_REV_MIN` to revision 2 or higher.

.. packetdiag:: ../../../_static/diagrams/can/can_bit_timing.diag
    :caption: Bit timing configuration for 500kbit/s given BRP = 8
    :align: center

The sample point of a bit is located on the intersection of Timing Segment 1 and 2. Enabling **Triple Sampling** will cause 3 time quanta to be sampled per bit instead of 1 (extra samples are located at the tail end of Timing Segment 1).

The **Synchronization Jump Width** is used to determined the maximum number of time quanta a single bit time can be lengthened/shortened for synchronization purposes. ``sjw`` can **range from 1 to 4**.

.. note::
    Multiple combinations of ``brp``, ``tseg_1``, ``tseg_2``, and ``sjw`` can achieve the same bit rate. Users should tune these values to the physical characteristics of their CAN bus by taking into account factors such as **propagation delay, node information processing time, and phase errors**.

Bit timing **macro initializers** are also available for commonly used CAN bus bit rates. The following macro initializers are provided by the CAN driver.

    - ``CAN_TIMING_CONFIG_12_5KBITS()``
    - ``CAN_TIMING_CONFIG_16KBITS()``
    - ``CAN_TIMING_CONFIG_20KBITS()``
    - ``CAN_TIMING_CONFIG_25KBITS()``
    - ``CAN_TIMING_CONFIG_50KBITS()``
    - ``CAN_TIMING_CONFIG_100KBITS()``
    - ``CAN_TIMING_CONFIG_125KBITS()``
    - ``CAN_TIMING_CONFIG_250KBITS()``
    - ``CAN_TIMING_CONFIG_500KBITS()``
    - ``CAN_TIMING_CONFIG_800KBITS()``
    - ``CAN_TIMING_CONFIG_1MBITS()``

.. note::
    The macro initializers for 12.5K, 16K, and 20K bit rates are only available
    for ESP32 revision 2 or later.

Acceptance Filter
^^^^^^^^^^^^^^^^^

The CAN controller contains a hardware acceptance filter which can be used to filter CAN messages of a particular ID. A node that filters out a message **will not receive the message, but will still acknowledge it**. Acceptances filters can make a node more efficient by filtering out messages sent over the CAN bus that are irrelevant to the CAN node in question. The CAN controller's acceptance filter is configured using two 32-bit values within :cpp:type:`can_filter_config_t` known as the **acceptance code** and the **acceptance mask**.

The **acceptance code** specifies the bit sequence which a message's ID, RTR, and data bytes must match in order for the message to be received by the CAN controller. The **acceptance mask** is a bit sequence specifying which bits of the acceptance code can be ignored. This allows for a messages of different IDs to be accepted by a single acceptance code.

The acceptance filter can be used under **Single or Dual Filter Mode**. Single Filter Mode will use the acceptance code and mask to define a single filter. This allows for the first two data bytes of a standard frame to be filtered, or the entirety of an extended frame's 29-bit ID. The following diagram illustrates how the 32-bit acceptance code and mask will be interpreted under Single Filter Mode (Note: The yellow and blue fields represent standard and extended CAN frames respectively).

.. packetdiag:: ../../../_static/diagrams/can/can_acceptance_filter_single.diag
    :caption: Bit layout of single filter mode (Right side MSBit)
    :align: center

**Dual Filter Mode** will use the acceptance code and mask to define two separate filters allowing for increased flexibility of ID's to accept, but does not allow for all 29-bits of an extended ID to be filtered. The following diagram illustrates how the 32-bit acceptance code and mask will be interpreted under **Dual Filter Mode** (Note: The yellow and blue fields represent standard and extended CAN frames respectively).

.. packetdiag:: ../../../_static/diagrams/can/can_acceptance_filter_dual.diag
    :caption: Bit layout of dual filter mode (Right side MSBit)
    :align: center

Disabling TX Queue
^^^^^^^^^^^^^^^^^^

The TX queue can be disabled during configuration by setting the ``tx_queue_len`` member of :cpp:type:`can_general_config_t` to ``0``. This will allow applications that do not require message transmission to save a small amount of memory when using the CAN driver.


.. -------------------------------- CAN Driver ---------------------------------

.. _driver-operation:

Driver Operation
----------------

The CAN driver is designed with distinct states and strict rules regarding the functions or conditions that trigger a state transition. The following diagram illustrates the various states and their transitions.

.. blockdiag:: ../../../_static/diagrams/can/can_state_transition.diag
    :caption: State transition diagram of the CAN driver (see table below)
    :align: center

+-------+------------------------+------------------------------------+
| Label | Transition             | Action/Condition                   |
+=======+========================+====================================+
| A     | Uninstalled -> Stopped | :cpp:func:`can_driver_install`     |
+-------+------------------------+------------------------------------+
| B     | Stopped -> Uninstalled | :cpp:func:`can_driver_uninstall`   |
+-------+------------------------+------------------------------------+
| C     | Stopped -> Running     | :cpp:func:`can_start`              |
+-------+------------------------+------------------------------------+
| D     | Running -> Stopped     | :cpp:func:`can_stop`               |
+-------+------------------------+------------------------------------+
| E     | Running -> Bus-Off     | Transmit Error Counter >= 256      |
+-------+------------------------+------------------------------------+
| F     | Bus-Off -> Uninstalled | :cpp:func:`can_driver_uninstall`   |
+-------+------------------------+------------------------------------+
| G     | Bus-Off -> Recovering  | :cpp:func:`can_initiate_recovery`  |
+-------+------------------------+------------------------------------+
| H     | Recovering -> Stopped  | 128 occurrences of bus-free signal |
+-------+------------------------+------------------------------------+

Driver States
^^^^^^^^^^^^^

**Uninstalled**: In the uninstalled state, no memory is allocated for the driver and the CAN controller is powered OFF.

**Stopped**: In this state, the CAN controller is powered ON and the CAN driver has been installed. However the CAN controller will be unable to take part in any CAN bus activities such as transmitting, receiving, or acknowledging messages.

**Running**: In the running state, the CAN controller is able to take part in bus activities. Therefore messages can be transmitted/received/acknowledged. Furthermore the CAN controller will be able to transmit error frames upon detection of errors on the CAN bus.

**Bus-Off**: The bus-off state is automatically entered when the CAN controller's Transmit Error Counter becomes greater than or equal to 256 (see CAN2.0B specification regarding error counter rules). The bus-off state indicates the occurrence of severe errors on the CAN bus or in the CAN controller. Whilst in the bus-off state, the CAN controller will be unable to take part in any CAN bus activities. To exit the bus-off state, the CAN controller must undergo the bus recovery process.

**Recovering**: The recovering state is entered when the CAN driver undergoes bus recovery. The CAN driver/controller will remain in the recovering state until the 128 occurrences of the bus-free signal (see CAN2.0B specification) is observed on the CAN bus.

Message Flags
^^^^^^^^^^^^^

The CAN driver distinguishes different types of CAN messages by using the message flags in the ``flags`` field of :cpp:type:`can_message_t`. These flags help distinguish whether a message is in standard or extended format, an RTR, and the type of transmission to use when transmitting such a message. The CAN driver supports the following flags:

+-------------------------------+----------------------------------------------+
| Flag                          |  Description                                 |
+===============================+==============================================+
| ``CAN_MSG_FLAG_EXTD``         | Message is in Extended Frame Format          |
|                               | (29bit ID)                                   |
+-------------------------------+----------------------------------------------+
| ``CAN_MSG_FLAG_RTR``          | Message is a Remote Transmit Request         |
+-------------------------------+----------------------------------------------+
| ``CAN_MSG_FLAG_SS``           | Transmit message using Single Shot           |
|                               | Transmission (Message will not be            |
|                               | retransmitted upon error or loss of          |
|                               | arbitration)                                 |
+-------------------------------+----------------------------------------------+
| ``CAN_MSG_FLAG_SELF``         | Transmit message using Self Reception        |
|                               | Request (Transmitted message will also       |
|                               | received by the same node)                   |
+-------------------------------+----------------------------------------------+
| ``CAN_MSG_FLAG_DLC_NON_COMP`` | Message's Data length code is larger than 8. |
|                               | This will break compliance with CAN2.0B      |
+-------------------------------+----------------------------------------------+

.. note::
    The ``CAN_MSG_FLAG_NONE`` flag can be used for Standard Frame Format messages


.. -------------------------------- Examples -----------------------------------

.. _examples:

Examples
--------

Configuration & Installation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The following code snippet demonstrates how to configure, install, and start the CAN driver via the use of the various configuration structures, macro initializers, the :cpp:func:`can_driver_install` function, and the :cpp:func:`can_start` function.

.. code-block:: c

    #include "driver/gpio.h"
    #include "driver/can.h"

    void app_main()
    {
        //Initialize configuration structures using macro initializers
        can_general_config_t g_config = CAN_GENERAL_CONFIG_DEFAULT(GPIO_NUM_21, GPIO_NUM_22, CAN_MODE_NORMAL);
        can_timing_config_t t_config = CAN_TIMING_CONFIG_500KBITS();
        can_filter_config_t f_config = CAN_FILTER_CONFIG_ACCEPT_ALL();

        //Install CAN driver
        if (can_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
            printf("Driver installed\n");
        } else {
            printf("Failed to install driver\n");
            return;
        }

        //Start CAN driver
        if (can_start() == ESP_OK) {
            printf("Driver started\n");
        } else {
            printf("Failed to start driver\n");
            return;
        }

        ...

    }

The usage of macro initializers are not mandatory and each of the configuration structures can be manually.

Message Transmission
^^^^^^^^^^^^^^^^^^^^

The following code snippet demonstrates how to transmit a message via the usage of the :cpp:type:`can_message_t` type and :cpp:func:`can_transmit` function.

.. code-block:: c

    #include "driver/can.h"

    ...

    //Configure message to transmit
    can_message_t message;
    message.identifier = 0xAAAA;
    message.flags = CAN_MSG_FLAG_EXTD;
    message.data_length_code = 4;
    for (int i = 0; i < 4; i++) {
        message.data[i] = 0;
    }

    //Queue message for transmission
    if (can_transmit(&message, pdMS_TO_TICKS(1000)) == ESP_OK) {
        printf("Message queued for transmission\n");
    } else {
        printf("Failed to queue message for transmission\n");
    }

Message Reception
^^^^^^^^^^^^^^^^^

The following code snippet demonstrates how to receive a message via the usage of the :cpp:type:`can_message_t` type and :cpp:func:`can_receive` function.

.. code-block:: c

    #include "driver/can.h"

    ...

    //Wait for message to be received
    can_message_t message;
    if (can_receive(&message, pdMS_TO_TICKS(10000)) == ESP_OK) {
        printf("Message received\n");
    } else {
        printf("Failed to receive message\n");
        return;
    }

    //Process received message
    if (message.flags & CAN_MSG_FLAG_EXTD) {
        printf("Message is in Extended Format\n");
    } else {
        printf("Message is in Standard Format\n");
    }
    printf("ID is %d\n", message.identifier);
    if (!(message.flags & CAN_MSG_FLAG_RTR)) {
        for (int i = 0; i < message.data_length_code; i++) {
            printf("Data byte %d = %d\n", i, message.data[i]);
        }
    }

Reconfiguring and Reading Alerts
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The following code snippet demonstrates how to reconfigure and read CAN driver alerts via the use of the :cpp:func:`can_reconfigure_alerts` and :cpp:func:`can_read_alerts` functions.

.. code-block:: c

    #include "driver/can.h"

    ...

    //Reconfigure alerts to detect Error Passive and Bus-Off error states
    uint32_t alerts_to_enable = CAN_ALERT_ERR_PASS | CAN_ALERT_BUS_OFF;
    if (can_reconfigure_alerts(alerts_to_enable, NULL) == ESP_OK) {
        printf("Alerts reconfigured\n");
    } else {
        printf("Failed to reconfigure alerts");
    }

    //Block indefinitely until an alert occurs
    uint32_t alerts_triggered;
    can_read_alerts(&alerts_triggered, portMAX_DELAY);

Stop and Uninstall
^^^^^^^^^^^^^^^^^^

The following code demonstrates how to stop and uninstall the CAN driver via the use of the :cpp:func:`can_stop` and :cpp:func:`can_driver_uninstall` functions.

.. code-block:: c

    #include "driver/can.h"

    ...

    //Stop the CAN driver
    if (can_stop() == ESP_OK) {
        printf("Driver stopped\n");
    } else {
        printf("Failed to stop driver\n");
        return;
    }

    //Uninstall the CAN driver
    if (can_driver_uninstall() == ESP_OK) {
        printf("Driver uninstalled\n");
    } else {
        printf("Failed to uninstall driver\n");
        return;
    }

Multiple ID Filter Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The acceptance mask in :cpp:type:`can_filter_config_t` can be configured such that two or more IDs will be accepted for a single filter. For a particular filter to accept multiple IDs, the conflicting bit positions amongst the IDs must be set in the acceptance mask. The acceptance code can be set to any one of the IDs.

The following example shows how the calculate the acceptance mask given multiple IDs::

    ID1 =  11'b101 1010 0000
    ID2 =  11'b101 1010 0001
    ID3 =  11'b101 1010 0100
    ID4 =  11'b101 1010 1000
    //Acceptance Mask
    MASK = 11'b000 0000 1101

Application Examples
^^^^^^^^^^^^^^^^^^^^

**Network Example:** The CAN Network example demonstrates communication between two ESP32s using the CAN driver API. One CAN node acts as a network master initiate and ceasing the transfer of a data from another CAN node acting as a network slave. The example can be found via :example:`peripherals/can/can_network`.

**Alert and Recovery Example:** This example demonstrates how to use the CAN driver's alert and bus recovery API. The example purposely introduces errors on the CAN bus to put the CAN controller into the Bus-Off state. An alert is used to detect the Bus-Off state and trigger the bus recovery process. The example can be found via :example:`peripherals/can/can_alert_and_recovery`.

**Self Test Example:** This example uses the No Acknowledge Mode and Self Reception Request to cause the CAN controller to send and simultaneously receive a series of messages. This example can be used to verify if the connections between the CAN controller and the external transceiver are working correctly. The example can be found via :example:`peripherals/can/can_self_test`.


.. ---------------------------- API Reference ----------------------------------

API Reference
-------------

.. include:: /_build/inc/can.inc