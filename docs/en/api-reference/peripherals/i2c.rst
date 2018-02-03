I2C
===

An I2C (Inter-Integrated Circuit) bus can be used for communication with several external devices connected to the same bus as ESP32. There are two I2C controllers on board of the ESP32, each of which can be set to master mode or slave mode.


Overview
--------

The following sections will walk you through typical steps to configure and operate the I2C driver:

1.  :ref:`i2c-api-configure-driver` - select driver's parameters like master or slave mode, set specific GPIO pins to act as SDA and SCL, set the clock speed, etc. 
2.  :ref:`i2c-api-install-driver`- activate driver in master or slave mode to operate on one of the two I2C controllers available on ESP32.
3.  :ref:`i2c-api-run-communication`:

    a)  :ref:`i2c-api-master-mode` - run communication acting as a master
    b)  :ref:`i2c-api-slave-mode` - get slave responding to messages from the master

4.  :ref:`i2c-api-interrupt-handling` - configure and service I2C interrupts.
5.  :ref:`i2c-api-going-beyond-defaults` - adjust timing, pin configuration and other parameters of the I2C communication.
6.  :ref:`i2c-api-error-handling` - how to recognize and handle driver configuration and communication errors.
7.  :ref:`i2c-api-delete-driver`- on communication end to free resources used by the I2C driver.

The top level identification of an I2C driver is one of the two port numbers selected from :cpp:type:`i2c_port_t`. The mode of operation for a given port is provided during driver configuration by selecting either "master" or "slave" from :cpp:type:`i2c_mode_t`.


.. _i2c-api-configure-driver:

Configure Driver
^^^^^^^^^^^^^^^^

The first step to establishing I2C communication is to configure the driver. This is done by setting several parameters contained in :cpp:type:`i2c_config_t` structure:

* I2C **operation mode** - select either slave or master from :cpp:type:`i2c_opmode_t`
* Settings of the **communication pins**:

    * GPIO pin numbers assigned to the SDA and SCL signals 
    * Whether to enable ESP32's internal pull up for respective pins

* I2C **clock speed**, if this configuration concerns the master mode
* If this configuration concerns the slave mode:

    * Whether **10 bit address mode** should be enabled
    * The **slave address**

Then, to initialize configuration for a given I2C port, call function :cpp:func:`i2c_param_config` with the port number and :cpp:type:`i2c_config_t` structure as the function call parameters. 

At this stage :cpp:func:`i2c_param_config` also sets "behind the scenes" couple of other I2C configuration parameters to commonly used default values. To check what are the values and how to change them, see :ref:`i2c-api-going-beyond-defaults`. 


.. _i2c-api-install-driver:

Install Driver
^^^^^^^^^^^^^^

Having the configuration initialized, the next step is to install the I2C driver by calling :cpp:func:`i2c_driver_install`. This function call requires the following parameters:

* The port number, one of the two ports available, selected from :cpp:type:`i2c_port_t`
* The operation mode, slave or master selected from :cpp:type:`i2c_opmode_t`
* Sizes of buffers that will be allocated for sending and receiving data **in the slave mode**
* Flags used to allocate the interrupt


.. _i2c-api-run-communication:

Run Communication
^^^^^^^^^^^^^^^^^

With the I2C driver installed, ESP32 is ready to communicate with other I2C devices. Programming of communication depends on whether selected I2C port operates in a master or a slave mode.


.. _i2c-api-master-mode:

Master Mode
"""""""""""

ESP32's I2C port working in the master made is responsible for establishing communication with slave I2C devices and sending commands to trigger actions by slaves, like doing a measurement and sending back a result. 

To organize this process the driver provides a container, called a "command link", that should be populated with a sequence of commands and then passed to the I2C controller for execution.

**Master Write**

An example of building a commend link for I2C master sending n bytes to slave is shown below:

.. blockdiag::
    :scale: 75
    :caption: I2C command link - master write example
    :align: center

    blockdiag i2c-command-link-master-write { 
        # global properties
        span_width = 5;
        span_height = 5;
        node_height = 25;
        default_group_color = lightgrey;
        class spacer [shape=none, width=10];
        class cmdlink [colwidth=2, width=180];
        class cjoint [shape=none, width=40];

        # all the rows
        0 -- a0 --                         f0 [style=none]; 
        1 -- a1 -- b1 -- c1 -- d1 -- e1 -- f1 -- g1 -- h1 [style=none]; 
        2 -- a2 -- b2 -- c2 -- d2 -- e2 -- f2 -- g2 [style=none]; 
        3 -- a3 --             d3 --       f3 [style=none];
        4 -- a4 [style=none];
        5 -- a5 [style=none];
        6 -- a6 --       c6 [style=none];
        7 -- a7 --       c7 -- d7 [style=none];
        8 -- a8 --       c8 --              f8 [style=none];
        9 -- a9 --       c9 --                         h9 [style=none];
        10 -- a10 [style=none];
        11 -- a11 [style=none];

        # separator row
        3, a3, d3, f3 [shape=none, height=5];

        # tuning node properties and connections
        0 [class=spacer]; a0 [shape=none, colwidth=5]; f0 [shape=note, colwidth=2];
        1 [class=spacer]; a1 [shape=none]; b1; c1 [width=40]; e1 [shape=none, width=30]; g1 [shape=none, width=30]; h1 [width=40];
        2 [class=spacer]; a2 [shape=none]; b2; c2 [class=cjoint]; d2 [shape=none]; e2 [width=30]; f2 [shape=none]; g2 [width=30];
        3 [class=spacer]; a3 [shape=none, colwidth=3]; d3 [colwidth=2]; f3 [colwidth=2];
        4 [class=spacer]; a4 [class=cmdlink]
        5 [class=spacer]; a5 [class=cmdlink];
        6 [class=spacer]; a6 [class=cmdlink]; c6 [class=cjoint]; a6 -- c6 [style=solid]; c6 -- c2 -> c1 [folded];
        7 [class=spacer]; a7 [class=cmdlink]; c7 [class=cjoint]; d7 [shape=none, colwidth=2]; a7 -- c7 -- d7 [style=solid]; d7 -> d3 [folded];
        8 [class=spacer]; a8 [class=cmdlink]; c8 [class=cjoint, colwidth=3]; f8 [shape=none, colwidth=2]; a8 -- c8 -- f8 [style=solid]; f8 -> f3 [folded];
        9 [class=spacer]; a9 [class=cmdlink]; c9 [class=cjoint, colwidth=5]; h9 [shape=none, width=40]; a9 -- c9 -- h9 [style=solid]; h9 -> h1 [folded];
        10 [class=spacer]; a10 [class=cmdlink]; 
        11 [class=spacer]; a11 [class=cmdlink]; 

        # labels
        f0 [label="Data n times", shape=note, color=yellow];
        b1 [label=Master, shape=note, color=lightyellow]; c1 [label=START]; d1 [label="Slave Address"]; f1 [label=Data]; h1 [label=STOP];
        b2 [label=Slave, shape=note, color=lightyellow]; e2 [label=ACK]; g2 [label=ACK];  
        a4 [shape=note, label=Commands, color=yellow]; 
        a5 [label="cmd = i2c_cmd_link_create()", numbered = 1]; 
        a6 [label="i2c_master_start(cmd)", numbered = 2]; 
        a7 [label="i2c_master_write_byte(cmd, Address, ACK)", numbered = 3]; 
        a8 [label="i2c_master_write(Data, n, ACK)", numbered = 4]; 
        a9 [label="i2c_master_stop(cmd)", numbered = 5]; 
        a10 [label="i2c_master_cmd_begin(I2c_port, cmd, wait)", numbered = 6]; 
        a11 [label="i2c_cmd_link_delete(cmd)", numbered = 7]; 

        # Slave Address
        group { d1; e1; }
        group { d2; e2; d3; }

        # Data x n times
        group { f1; g1;}
        group { f2; g2; f3; }
    }

The following describes how the command link for a "master write" is set up and what comes inside:

1.  The first step is to create a command link with :cpp:func:`i2c_cmd_link_create`. 

    Then the command link is populated with series of data to be sent to the slave:
    
    2.  **Start bit** - :cpp:func:`i2c_master_start`
    3.  Single byte **slave address** - :cpp:func:`i2c_master_write_byte`. The address is provided as an argument of this function call.
    4.  One or more bytes of **data** as an argument of :cpp:func:`i2c_master_write`. 
    5.  **Stop bit** - :cpp:func:`i2c_master_stop`

    Both :cpp:func:`i2c_master_write_byte` and :cpp:func:`i2c_master_write` commands have additional argument defining whether slave should **acknowledge** received data or not.

6.  Execution of command link by I2C controller is triggered by calling :cpp:func:`i2c_master_cmd_begin`.
7.  As the last step, after sending of the commands is finished, the resources used by the command link are released by calling :cpp:func:`i2c_cmd_link_delete`.

**Master Read**

There is a similar sequence of steps for the master to read the data from a slave. 

.. blockdiag::
    :scale: 100
    :caption: I2C command link - master read example
    :align: center

    blockdiag i2c-command-link-master-read { 
        # global properties
        span_width = 5;
        span_height = 5;
        node_height = 25;
        default_group_color = lightgrey;
        class spacer [shape=none, width=10];
        class cmdlink [colwidth=2, width=180];
        class cjoint [shape=none, width=40];

        # all the rows
        0 -- a0 --                         f0 [style=none]; 
        1 -- a1 -- b1 -- c1 -- d1 -- e1 -- f1 -- g1 -- h1 -- i1 -- j1 [style=none]; 
        2 -- a2 -- b2 -- c2 -- d2 -- e2 -- f2 -- g2 -- h2 -- i2  [style=none]; 
        3 -- a3 --             d3 --       f3 --       h3 [style=none];
        4 -- a4 [style=none];
        5 -- a5 [style=none];
        6 -- a6 --       c6 [style=none];
        7 -- a7 --       c7 -- d7 [style=none];
        8 -- a8 --       c8 --              f8 [style=none];
        9 -- a9 --       c9 --                         h9 [style=none];
        10 -- a10 --     c10 --                                    j10 [style=none];
        11 -- a11 [style=none];
        12 -- a12 [style=none];

        # separator row
        3, a3, d3, f3, h3 [shape=none, height=5];

        # tuning node properties and connections
        0 [class=spacer]; a0 [shape=none, colwidth=5]; f0 [shape=note, colwidth=2];
        1 [class=spacer]; a1 [shape=none]; b1; c1 [width=40]; e1 [shape=none, width=30]; f1 [shape=none]; g1 [width=30]; h1 [shape=none]; i1 [width=30]; j1 [width=40];
        2 [class=spacer]; a2 [shape=none]; b2; c2 [class=cjoint]; d2 [shape=none]; e2 [width=30]; g2 [shape=none, width=30]; i2 [shape=none, width=30];
        3 [class=spacer]; a3 [shape=none, colwidth=3]; d3 [colwidth=2]; f3 [colwidth=2]; h3 [colwidth=2];
        4 [class=spacer]; a4 [class=cmdlink]
        5 [class=spacer]; a5 [class=cmdlink];
        6 [class=spacer]; a6 [class=cmdlink]; c6 [class=cjoint]; a6 -- c6 [style=solid]; c6 -- c2 -> c1 [folded];
        7 [class=spacer]; a7 [class=cmdlink]; c7 [class=cjoint]; d7 [shape=none, colwidth=2]; a7 -- c7 -- d7 [style=solid]; d7 -> d3 [folded];
        8 [class=spacer]; a8 [class=cmdlink]; c8 [class=cjoint, colwidth=3]; f8 [shape=none, colwidth=2]; a8 -- c8 -- f8 [style=solid]; f8 -> f3 [folded];
        9 [class=spacer]; a9 [class=cmdlink]; c9 [class=cjoint, colwidth=5]; h9 [shape=none, colwidth=2]; a9 -- c9 -- h9 [style=solid]; h9 -> h3 [folded];
        10 [class=spacer]; a10 [class=cmdlink]; c10 [class=cjoint, colwidth=7]; j10 [shape=none, width=40]; a10 -- c10 -- j10 [style=solid]; j10 -> j1 [folded];
        11 [class=spacer]; a11 [class=cmdlink]; 
        12 [class=spacer]; a12 [class=cmdlink]; 

        # labels
        f0 [label="Data (n-1) times", shape=note, color=yellow];
        b1 [label=Master, shape=note, color=lightyellow]; c1 [label=START]; d1 [label="Slave Address"]; g1 [label=ACK]; i1 [label=NAK]; j1 [label=STOP];
        b2 [label=Slave, shape=note, color=lightyellow]; e2 [label=ACK]; f2 [label=Data]; h2 [label=Data];
        a4 [shape=note, label=Commands, color=yellow]; 
        a5 [label="cmd = i2c_cmd_link_create()", numbered = 1]; 
        a6 [label="i2c_master_start(cmd)", numbered = 2]; 
        a7 [label="i2c_master_write_byte(cmd, Address, ACK)", numbered = 3]; 
        a8 [label="i2c_master_read(Data, n-1, ACK)", numbered = 4]; 
        a9 [label="i2c_master_read(Data, 1, NAK)", numbered = 5]; 
        a10 [label="i2c_master_stop(cmd)", numbered = 6]; 
        a11 [label="i2c_master_cmd_begin(I2c_port, cmd, wait)", numbered = 7]; 
        a12 [label="i2c_cmd_link_delete(cmd)", numbered = 8]; 

        # Slave Address
        group { d1; e1; }
        group { d2; e2; d3; }

        # Data x (n - 1) times
        group { f1; g1;}
        group { f2; g2; f3; }

        # Data
        group { h1; i1; }
        group { h2; i2; h3; }
    }

When reading the data, instead of "i2c_master_read...", the command link is populated with :cpp:func:`i2c_master_read_byte` and / or :cpp:func:`i2c_master_read`. Also, the last read is configured for not providing an acknowledge by the master.

**Master Write or Read?**

After sending a slave's address, see step 3 on pictures above, the master either writes to or reads from the slave. The information what the master will actually do is hidden in the least significant bit of the slave's address. 

Therefore the command link instructing the slave that the master will write the data contains the address like ``(ESP_SLAVE_ADDR << 1) | I2C_MASTER_WRITE`` and looks as follows::

  i2c_master_write_byte(cmd, (ESP_SLAVE_ADDR << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN)

By similar token the command link to read from the slave looks as follows::

  i2c_master_write_byte(cmd, (ESP_SLAVE_ADDR << 1) | I2C_MASTER_READ, ACK_CHECK_EN)


.. _i2c-api-slave-mode:

Slave Mode
""""""""""

The API provides functions to read and write data by the slave - * :cpp:func:`i2c_slave_read_buffer` and :cpp:func:`i2c_slave_write_buffer`. An example of using these functions is provided in :example:`peripherals/i2c`.


.. _i2c-api-interrupt-handling:

Interrupt Handling
^^^^^^^^^^^^^^^^^^

To register an interrupt handler, call function :cpp:func:`i2c_isr_register`, to delete the handler call :cpp:func:`i2c_isr_free`. Description of interrupts triggered by I2C controller is provided in the `ESP32 Technical Reference Manual (PDF) <https://espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf>`_.


.. _i2c-api-going-beyond-defaults:

Going Beyond Defaults
^^^^^^^^^^^^^^^^^^^^^

There are couple of I2C communication parameters setup during driver configuration (when calling :cpp:func:`i2c_param_config`, see :ref:`i2c-api-configure-driver`), to some default commonly used values. Some parameters are also already configured in registers of the I2C controller. These parameters can be changed to user defined values by calling dedicated functions: 

* Period of SCL pulses being high and low - :cpp:func:`i2c_set_period`
* SCL and SDA signal timing used during generation of start / stop signals - :cpp:func:`i2c_set_start_timing` / :cpp:func:`i2c_set_stop_timing`
* Timing relationship between SCL and SDA signals when sampling by slave, as well as when transmitting by master - :cpp:func:`i2c_set_data_timing`
* I2C timeout - :cpp:func:`i2c_set_timeout`

  .. note::

    The timing values are defined in APB clock cycles. The frequency of APB is specified in :cpp:type:`I2C_APB_CLK_FREQ`.

* What bit, LSB or MSB, is transmitted / received first - :cpp:func:`i2c_set_data_mode` selectable out of modes defined in :cpp:type:`i2c_trans_mode_t`

Each one of the above functions has a *_get_* counterpart to check the currently set value.

To see the default values of parameters setup during driver configuration, please refer to file :component_file:`driver/i2c.c` looking up defines with ``_DEFAULT`` suffix.

With function :cpp:func:`i2c_set_pin` it is also possible to select different SDA and SCL pins and alter configuration of pull ups, changing what has been already entered with :cpp:func:`i2c_param_config`.

.. note::

    ESP32's internal pull ups are in the range of some tens of kOhm, and as such in most cases insufficient for use as I2C pull ups by themselves. We suggest to add external pull ups as well, with values as described in the I2C standard.


.. _i2c-api-error-handling:

Error Handling
^^^^^^^^^^^^^^

Most of driver's function return the ``ESP_OK`` on successful completion or a specific error code on a failure. It is a good practice to always check the returned values and implement the error handling. The driver is also printing out log messages, when e.g. checking the correctness of entered configuration, that contain explanation of errors. For details please refer to file :component_file:`driver/i2c.c` looking up defines with ``_ERR_STR`` suffix.

Use dedicated interrupts to capture communication failures. For instance there is ``I2C_TIME_OUT_INT`` interrupt triggered when I2C takes too long to receive data. See :ref:`i2c-api-interrupt-handling` for related information.

To reset internal hardware buffers in case of communication failure, you can use :cpp:func:`i2c_reset_tx_fifo` and :cpp:func:`i2c_reset_rx_fifo`.


.. _i2c-api-delete-driver:

Delete Driver
^^^^^^^^^^^^^

If the I2C communication is established with :cpp:func:`i2c_driver_install` for some specific period of time and then not required, the driver may be removed to free allocated resources by calling :cpp:func:`i2c_driver_delete`.


Application Example
-------------------

I2C master and slave example: :example:`peripherals/i2c`.


API Reference
-------------

.. include:: /_build/inc/i2c.inc
