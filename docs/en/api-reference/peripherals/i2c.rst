I2C Driver
==========

Overview
--------

I2C is a serial, synchronous, half-duplex communication protocol that allows co-existence of multiple masters and slaves on the same bus. The I2C bus consists of two lines: serial data line (SDA) and serial clock (SCL). Both lines require pull-up resistors.

With such advantages as simplicity and low manufacturing cost, I2C is mostly used for communication of low-speed peripheral devices over short distances (within one foot).

{IDF_TARGET_NAME} has two I2C controllers (also referred to as ports) which are responsible for handling communications on two I2C buses. Each I2C controller can operate as master or slave. As an example, one controller can act as a master and the other as a slave at the same time.


Driver Features
---------------

I2C driver governs communications of devices over the I2C bus. The driver supports the following features:

- Reading and writing bytes in Master mode
- Slave mode
- Reading and writing to registers which are in turn read/written by the master


Driver Usage
------------

The following sections describe typical steps of configuring and operating the I2C driver:

1. :ref:`i2c-api-configure-driver` - set the initialization parameters (master or slave mode, GPIO pins for SDA and SCL, clock speed, etc.)
2. :ref:`i2c-api-install-driver`- activate the driver on one of the two I2C controllers as a master or slave
3. Depending on whether you configure the driver for a master or slave, choose the appropriate item

   a) :ref:`i2c-api-master-mode` - handle communications (master)
   b) :ref:`i2c-api-slave-mode` - respond to messages from the master (slave)

4. :ref:`i2c-api-interrupt-handling` - configure and service I2C interrupts
5. :ref:`i2c-api-customized-configuration` - adjust default I2C communication parameters (timings, bit order, etc.)
6. :ref:`i2c-api-error-handling` - how to recognize and handle driver configuration and communication errors
7. :ref:`i2c-api-delete-driver`- release resources used by the I2C driver when communication ends


.. _i2c-api-configure-driver:

Configuration
^^^^^^^^^^^^^

To establish I2C communication, start by configuring the driver. This is done by setting the parameters of the structure :cpp:type:`i2c_config_t`:

- Set I2C **mode of operation** - slave or master from :cpp:type:`i2c_mode_t`
- Configure **communication pins**

    - Assign GPIO pins for SDA and SCL signals
    - Set whether to enable {IDF_TARGET_NAME}'s internal pull-ups

- (Master only) Set I2C **clock speed**
- (Slave only) Configure the following

    * Whether to enable **10 bit address mode**
    * Define **slave address**

After that, initialize the configuration for a given I2C port. For this, call the function :cpp:func:`i2c_param_config` and pass to it the port number and the structure :cpp:type:`i2c_config_t`.

At this stage, :cpp:func:`i2c_param_config` also sets a few other I2C configuration parameters to default values that are defined by the I2C specification. For more details on the values and how to modify them, see :ref:`i2c-api-customized-configuration`.


.. _i2c-api-install-driver:

Install Driver
^^^^^^^^^^^^^^

After the I2C driver is configured, install it by calling the function :cpp:func:`i2c_driver_install` with the following parameters:

- Port number, one of the two port numbers from :cpp:type:`i2c_port_t`
- Master or slave, selected from :cpp:type:`i2c_mode_t`
- (Slave only) Size of buffers to allocate for sending and receiving data. As I2C is a master-centric bus, data can only go from the slave to the master at the master's request. Therefore, the slave will usually have a send buffer where the slave application writes data. The data remains in the send buffer to be read by the master at the master's own discretion.
- Flags for allocating the interrupt (see ESP_INTR_FLAG_* values in :component_file:`esp32/include/esp_intr_alloc.h`)


.. _i2c-api-master-mode:

Communication as Master
^^^^^^^^^^^^^^^^^^^^^^^

After installing the I2C driver, {IDF_TARGET_NAME} is ready to communicate with other I2C devices.

{IDF_TARGET_NAME}'s I2C controller operating as master is responsible for establishing communication with I2C slave devices and sending commands to trigger a slave to action, for example, to take a measurement and send the readings back to the master.

For better process organization, the driver provides a container, called a "command link", that should be populated with a sequence of commands and then passed to the I2C controller for execution.


Master Write
""""""""""""

The example below shows how to build a command link for an I2C master to send *n* bytes to a slave.

.. blockdiag:: ../../../_static/diagrams/i2c-command-link-master-write-blockdiag.diag
    :scale: 100
    :caption: I2C command link - master write example
    :align: center


The following describes how a command link for a "master write" is set up and what comes inside:

1. Create a command link with :cpp:func:`i2c_cmd_link_create`.

    Then, populate it with the series of data to be sent to the slave:

   a) **Start bit** - :cpp:func:`i2c_master_start`
   b) **Slave address** - :cpp:func:`i2c_master_write_byte`. The single byte address is provided as an argument of this function call.
   c) **Data** - One or more bytes as an argument of :cpp:func:`i2c_master_write`
   d) **Stop bit** - :cpp:func:`i2c_master_stop`

    Both functions :cpp:func:`i2c_master_write_byte` and :cpp:func:`i2c_master_write` have an additional argument specifying whether the master should ensure that it has received the ACK bit.

2. Trigger the execution of the command link by I2C controller by calling :cpp:func:`i2c_master_cmd_begin`. Once the execution is triggered, the command link cannot be modified.
3. After the commands are transmitted, release the resources used by the command link by calling :cpp:func:`i2c_cmd_link_delete`.


Master Read
"""""""""""

The example below shows how to build a command link for an I2C master to read *n* bytes from a slave.

.. blockdiag:: ../../../_static/diagrams/i2c-command-link-master-read-blockdiag.diag
    :scale: 100
    :caption: I2C command link - master read example
    :align: center


Compared to writing data, the command link is populated in Step 4 not with ``i2c_master_write...`` functions but with :cpp:func:`i2c_master_read_byte` and / or :cpp:func:`i2c_master_read`. Also, the last read in Step 5 is configured so that the master does not provide the ACK bit.


Indicating Write or Read
""""""""""""""""""""""""

After sending a slave address (see Step 3 on both diagrams above), the master either writes or reads from the slave.

The information on what the master will actually do is hidden in the least significant bit of the slave's address.

For this reason, the command link sent by the master to write data to the slave contains the address ``(ESP_SLAVE_ADDR << 1) | I2C_MASTER_WRITE`` and looks as follows:

.. code-block:: c

    i2c_master_write_byte(cmd, (ESP_SLAVE_ADDR << 1) | I2C_MASTER_WRITE, ACK_EN);

Likewise, the command link to read from the slave looks as follows:

.. code-block:: c

    i2c_master_write_byte(cmd, (ESP_SLAVE_ADDR << 1) | I2C_MASTER_READ, ACK_EN);


.. _i2c-api-slave-mode:

Communication as Slave
^^^^^^^^^^^^^^^^^^^^^^

After installing the I2C driver, {IDF_TARGET_NAME} is ready to communicate with other I2C devices.

The API provides the following functions for slaves

- :cpp:func:`i2c_slave_read_buffer`

    Whenever the master writes data to the slave, the slave will automatically store it in the receive buffer. This allows the slave application to call the function :cpp:func:`i2c_slave_read_buffer` at its own discretion. This function also has a parameter to specify block time if no data is in the receive buffer. This will allow the slave application to wait with a specified timeout for data to arrive to the buffer.

- :cpp:func:`i2c_slave_write_buffer`

    The send buffer is used to store all the data that the slave wants to send to the master in FIFO order. The data stays there until the master requests for it. The function :cpp:func:`i2c_slave_write_buffer` has a parameter to specify block time if the send buffer is full. This will allow the slave application to wait with a specified timeout for the adequate amount of space to become available in the send buffer.

A code example showing how to use these functions can be found in :example:`peripherals/i2c`.


.. _i2c-api-interrupt-handling:

Interrupt Handling
^^^^^^^^^^^^^^^^^^

During driver installation, an interrupt handler is installed by default. However, you can register your own interrupt handler instead of the default one by calling the function :cpp:func:`i2c_isr_register`. When implementing your own interrupt handler, refer to the `{IDF_TARGET_NAME} Technical Reference Manual (PDF) <{IDF_TARGET_TRM_EN_URL}>`_ for the description of interrupts triggered by the I2C controller.

To delete an interrupt handler, call :cpp:func:`i2c_isr_free`.

.. _i2c-api-customized-configuration:

Customized Configuration
^^^^^^^^^^^^^^^^^^^^^^^^

As mentioned at the end of Section :ref:`i2c-api-configure-driver`, when the function :cpp:func:`i2c_param_config` initializes the driver configuration for an I2C port, it also sets several I2C communication parameters to default values defined in the `I2C specification <https://www.nxp.com/docs/en/user-guide/UM10204.pdf>`_. Some other related parameters are pre-configured in registers of the I2C controller.

All these parameters can be changed to user-defined values by calling dedicated functions given in the table below. Please note that the timing values are defined in APB clock cycles. The frequency of APB is specified in :cpp:type:`I2C_APB_CLK_FREQ`.

.. list-table:: Other Configurable I2C Communication Parameters
   :widths: 65 35
   :header-rows: 1

   * - Parameters to Change
     - Function
   * - High time and low time for SCL pulses
     - :cpp:func:`i2c_set_period`
   * - SCL and SDA signal timing used during generation of **start** signals
     - :cpp:func:`i2c_set_start_timing`
   * - SCL and SDA signal timing used during generation of **stop** signals
     - :cpp:func:`i2c_set_stop_timing`
   * - Timing relationship between SCL and SDA signals when slave samples, as well as when master toggles
     - :cpp:func:`i2c_set_data_timing`
   * - I2C timeout
     - :cpp:func:`i2c_set_timeout`
   * - Choice between transmitting / receiving the LSB or MSB first, choose one of the modes defined in :cpp:type:`i2c_trans_mode_t`
     - :cpp:func:`i2c_set_data_mode`


Each of the above functions has a *_get_* counterpart to check the currently set value. For example, to check the I2C timeout value, call :cpp:func:`i2c_get_timeout`.

To check the default parameter values which are set during the driver configuration process, please refer to the file :component_file:`driver/i2c.c` and look for defines with the suffix ``_DEFAULT``.

You can also select different pins for SDA and SCL signals and alter the configuration of pull-ups with the function :cpp:func:`i2c_set_pin`. If you want to modify already entered values, use the function :cpp:func:`i2c_param_config`.

.. note::

    {IDF_TARGET_NAME}'s internal pull-ups are in the range of tens of kOhm, which is, in most cases, insufficient for use as I2C pull-ups. Users are advised to use external pull-ups with values described in the `I2C specification <https://www.nxp.com/docs/en/user-guide/UM10204.pdf>`_.


.. _i2c-api-error-handling:

Error Handling
^^^^^^^^^^^^^^

The majority of I2C driver functions either return ``ESP_OK`` on successful completion or a specific error code on failure. It is a good practice to always check the returned values and implement error handling. The driver also prints out log messages that contain error details, e.g., when checking the validity of entered configuration. For details please refer to the file :component_file:`driver/i2c.c` and look for defines with the suffix ``_ERR_STR``.

Use dedicated interrupts to capture communication failures. For instance, if a slave stretches the clock for too long while preparing the data to send back to master, the interrupt ``I2C_TIME_OUT_INT`` will be triggered. For detailed information, see :ref:`i2c-api-interrupt-handling`.

In case of a communication failure, you can reset the internal hardware buffers by calling the functions :cpp:func:`i2c_reset_tx_fifo` and :cpp:func:`i2c_reset_rx_fifo` for the send and receive buffers respectively.


.. _i2c-api-delete-driver:

Delete Driver
^^^^^^^^^^^^^

When the I2C communication is established with the function :cpp:func:`i2c_driver_install` and is not required for some substantial amount of time, the driver may be deinitialized to release allocated resources by calling :cpp:func:`i2c_driver_delete`.

Before calling :cpp:func:`i2c_driver_delete` to remove i2c driver, please make sure that all threads have stopped using the driver in any way, because this function does not guarantee thread safety.

Application Example
-------------------

I2C master and slave example: :example:`peripherals/i2c`.


API Reference
-------------

.. include-build-file:: inc/i2c.inc
.. include-build-file:: inc/i2c_types.inc
