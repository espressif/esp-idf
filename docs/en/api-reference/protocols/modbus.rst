ESP-Modbus
==========

Overview
--------

The Modbus serial communication protocol is de facto standard protocol widely used to connect industrial electronic devices. Modbus allows communication among many devices connected to the same network, for example, a system that measures temperature and humidity and communicates the results to a computer. The Modbus protocol uses several types of data: Holding Registers, Input Registers, Coils (single bit output), Discrete Inputs. Versions of the Modbus protocol exist for serial port and for Ethernet and other protocols that support the Internet protocol suite. 
There are many variants of Modbus protocols, some of them are:


    * ``Modbus RTU`` — This is used in serial communication and makes use of a compact, binary representation of the data for protocol communication. The RTU format follows the commands/data with a cyclic redundancy check checksum as an error check mechanism to ensure the reliability of data. Modbus RTU is the most common implementation available for Modbus. A Modbus RTU message must be transmitted continuously without inter-character hesitations. Modbus messages are framed (separated) by idle (silent) periods. The RS-485 interface communication is usually used for this type.
    * ``Modbus ASCII`` — This is used in serial communication and makes use of ASCII characters for protocol communication. The ASCII format uses a longitudinal redundancy check checksum. Modbus ASCII messages are framed by leading colon (":") and trailing newline (CR/LF).
    * ``Modbus TCP/IP or Modbus TCP`` — This is a Modbus variant used for communications over TCP/IP networks, connecting over port 502. It does not require a checksum calculation, as lower layers already provide checksum protection.

The following document (and included code snippets) requires some familiarity with the Modbus protocol. Refer to the Modbus Organization's with protocol specifications for specifics.

Messaging Model And Data Mapping
--------------------------------

Modbus is an application protocol that defines rules for messaging structure and data organization that are independent of the data transmission medium. Traditional serial Modbus is a register-based protocol that defines message transactions that occur between master(s) and slave devices (multiple masters are allowed on using Modbus TCP/IP). The slave devices listen for communication from the master and simply respond as instructed. The master(s) always controls communication and may communicate directly to one slave, or all connected slaves, but the slaves cannot communicate directly with each other.

.. figure:: ../../../_static/modbus-segment.png
    :align: center
    :scale: 80%
    :alt: Modbus segment diagram
    :figclass: align-center

    Modbus segment diagram

.. note:: It is assumed that the number of slaves and their register maps are known by the Modbus master before the start of stack.

The register map of each slave device is usually part of its device manual. A Slave device usually permits configuration of its short slave address and communication options that are used within the device's network segment.

The Modbus protocol allows devices to map data to four types of registers (Holding, Input, Discrete, Coil). The figure below illustrates an example mapping of a device's data to the four types of registers.

.. figure:: ../../../_static/modbus-data-mapping.png
    :align: center
    :scale: 80%
    :alt: Modbus data mapping
    :figclass: align-center

    Modbus data mapping

The following sections give an overview of how to use the ESP_Modbus component found under `components/freemodbus`. The sections cover initialization of a Modbus port, and the setup a master or slave device accordingly:

- :ref:`modbus_api_port_initialization`
- :ref:`modbus_api_slave_overview`
- :ref:`modbus_api_master_overview`

.. _modbus_api_port_initialization:

Modbus Port Initialization
^^^^^^^^^^^^^^^^^^^^^^^^^^

The ESP_Modbus supports Modbus SERIAL and TCP ports and a port must be initialized before calling any other Modbus API. The functions below are used to create and then initialize Modbus controller interface (either master or slave) over a particular transmission medium (either Serial or TCP/IP):

- :cpp:func:`mbc_slave_init`
- :cpp:func:`mbc_master_init`
- :cpp:func:`mbc_slave_init_tcp`
- :cpp:func:`mbc_master_init_tcp`

The API call uses the first parameter to recognize the type of port being initialized. Supported enumeration for different ports: :cpp:enumerator:`MB_PORT_SERIAL_MASTER`, :cpp:enumerator:`MB_PORT_SERIAL_SLAVE` accordingly. 
The parameters :cpp:enumerator:`MB_PORT_TCP_MASTER`, :cpp:enumerator:`MB_PORT_TCP_SLAVE` are reserved for internal usage.

.. code:: c

    void* master_handler = NULL; // Pointer to allocate interface structure
    // Initialization of Modbus master for serial port
    esp_err_t err = mbc_master_init(MB_PORT_SERIAL_MASTER, &master_handler);
    if (master_handler == NULL || err != ESP_OK) {
        ESP_LOGE(TAG, "mb controller initialization fail.");
    }

This example code to initialize slave port:

.. code:: c

    void* slave_handler = NULL; // Pointer to allocate interface structure
    // Initialization of Modbus slave for TCP 
    esp_err_t err = mbc_slave_init_tcp(&slave_handler);
    if (slave_handler == NULL || err != ESP_OK) {
        // Error handling is performed here
        ESP_LOGE(TAG, "mb controller initialization fail.");
    }

.. _modbus_api_master_overview:

Modbus Master API Overview
--------------------------

The following overview describes how to setup Modbus master communication. The overview reflects a typical programming workflow and is broken down into the sections provided below:

1. :ref:`modbus_api_port_initialization` - Initialization of Modbus controller interface for the selected port.
2. :ref:`modbus_api_master_configure_descriptor` - Configure data descriptors to access slave parameters.
3. :ref:`modbus_api_master_setup_communication_options` - Allows to setup communication options for selected port.
4. :ref:`modbus_api_master_start_communication` - Start stack and sending / receiving data.
5. :ref:`modbus_api_master_destroy` - Destroy Modbus controller and its resources.

.. _modbus_api_master_configure_descriptor:

Configuring Master Data Access
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The architectural approach of ESP_Modbus includes one level above standard Modbus IO driver.
The additional layer is called Modbus controller and its goal is to add an abstraction such as CID - characteristic identifier.
The CID is linked to a corresponding Modbus registers through the table called Data Dictionary and represents device physical parameter (such as temperature, humidity, etc.) in specific Modbus slave device.
This approach allows the upper layer (e.g., MESH or MQTT) to be isolated from Modbus specifics thus simplify Modbus integration with other protocols/networks.

The Data Dictionary is the list in the Modbus master which shall be defined by user to link each CID to its corresponding Modbus registers representation using Register Mapping table of the Modbus slave being used.
Each element in this data dictionary is of type :cpp:type:`mb_parameter_descriptor_t` and represents the description of one physical characteristic:

.. list-table:: Table 1 Modbus master Data Dictionary description 
  :widths: 8 10 82
  :header-rows: 1

  * - Field 
    - Description
    - Detailed information
  * - ``cid``
    - Characteristic ID         
    - The identifier of characteristic (must be unique).
  * - ``param_key``
    - Characteristic Name
    - String description of the characteristic.
  * - ``param_units``
    - Characteristic Units
    - Physical Units of the characteristic.
  * - ``mb_slave_addr``
    - Modbus Slave Address
    - The short address of the device with correspond parameter UID.
  * - ``mb_param_type``
    - Modbus Register Type
    - Type of Modbus register area. 
      :cpp:enumerator:`MB_PARAM_INPUT`, :cpp:enumerator:`MB_PARAM_HOLDING`, :cpp:enumerator:`MB_PARAM_COIL`, :cpp:enumerator:`MB_PARAM_DISCRETE`  - represents Input , Holding, Coil and Discrete input register area accordingly;
  * - ``mb_reg_start``
    - Modbus Register Start
    - Relative register address of the characteristic in the register area.  
  * - ``mb_size``
    - Modbus Register Size
    - Length of characteristic in registers.
  * - ``param_offset``
    - Instance Offset
    - Offset to instance of the characteristic in bytes. It is used to calculate the absolute address to the characteristic in the storage structure.
      It is optional field and can be set to zero if the parameter is not used in the application.
  * - ``param_type``
    - Data Type                 
    - Specifies type of the characteristic.
      :cpp:enumerator:`PARAM_TYPE_U8`, :cpp:enumerator:`PARAM_TYPE_U16`, :cpp:enumerator:`PARAM_TYPE_U32` - Unsigned integer 8/16/32 bit type;
      :cpp:enumerator:`PARAM_TYPE_FLOAT` - IEEE754 floating point format;
      :cpp:enumerator:`PARAM_TYPE_ASCII` - ASCII string or binary data;
  * - ``param_size``
    - Data Size
    - The storage size of the characteristic (bytes).
  * - ``param_opts``
    - Parameter Options
    - Limits, options of characteristic used during processing of alarm in user application (optional)
  * - ``access``
    - Parameter access type
    - Can be used in user application to define the behavior of the characteristic during processing of data in user application;
      :cpp:enumerator:`PAR_PERMS_READ_WRITE_TRIGGER`, :cpp:enumerator:`PAR_PERMS_READ`, :cpp:enumerator:`PAR_PERMS_READ_WRITE_TRIGGER`;

.. note:: The ``cid`` and ``param_key`` have to be unique. Please use the prefix to the parameter key if you have several similar parameters in your register map table.

.. list-table:: Table 2 Example Register mapping table of Modbus slave
  :widths: 5 5 2 10 5 5 68
  :header-rows: 1
  
  * - CID
    - Register
    - Length
    - Range
    - Type
    - Units
    - Description
  * - 0
    - 30000
    - 4
    - MAX_UINT
    - U32
    - Not defined
    - Serial number of device (4 bytes) read-only
  * - 1
    - 30002
    - 2
    - MAX_UINT
    - U16
    - Not defined
    - Software version (4 bytes) read-only
  * - 2
    - 40000
    - 4
    - -20..40
    - FLOAT
    - DegC
    - Room temperature in DegC. Writing a temperature value to this register for single point calibration.
    
.. code:: c

    // Enumeration of modbus slave addresses accessed by master device
    enum {
        MB_DEVICE_ADDR1 = 1,
        MB_DEVICE_ADDR2,
        MB_SLAVE_COUNT
    };

    // Enumeration of all supported CIDs for device
    enum {
        CID_SER_NUM1 = 0,
        CID_SW_VER1,
        CID_TEMP_DATA_1,
        CID_SER_NUM2,
        CID_SW_VER2,
        CID_TEMP_DATA_2
    };

    // Example Data Dictionary for Modbus parameters in 2 slaves in the segment
    mb_parameter_descriptor_t device_parameters[] = {
        // CID, Name, Units, Modbus addr, register type, Modbus Reg Start Addr, Modbus Reg read length, 
        // Instance offset (NA), Instance type, Instance length (bytes), Options (NA), Permissions
        { CID_SER_NUM1, STR("Serial_number_1"), STR("--"), MB_DEVICE_ADDR1, MB_PARAM_INPUT, 0, 2,
                        0, PARAM_TYPE_U32, 4, OPTS( 0,0,0 ), PAR_PERMS_READ_WRITE_TRIGGER },
        { CID_SW_VER1, STR("Software_version_1"), STR("--"), MB_DEVICE_ADDR1, MB_PARAM_INPUT, 2, 1,
                        0, PARAM_TYPE_U16, 2, OPTS( 0,0,0 ), PAR_PERMS_READ_WRITE_TRIGGER },
        { CID_TEMP_DATA_1, STR("Temperature_1"), STR("C"), MB_DEVICE_ADDR1, MB_PARAM_HOLDING, 0, 2,
                        0, PARAM_TYPE_FLOAT, 4, OPTS( 16, 30, 1 ), PAR_PERMS_READ_WRITE_TRIGGER },
        { CID_SER_NUM2, STR("Serial_number_2"), STR("--"), MB_DEVICE_ADDR2, MB_PARAM_INPUT, 0, 2,
                        0, PARAM_TYPE_U32, 4, OPTS( 0,0,0 ), PAR_PERMS_READ_WRITE_TRIGGER },
        { CID_SW_VER2, STR("Software_version_2"), STR("--"), MB_DEVICE_ADDR2, MB_PARAM_INPUT, 2, 1,
                        0, PARAM_TYPE_U16, 2, OPTS( 0,0,0 ), PAR_PERMS_READ_WRITE_TRIGGER },
        { CID_TEMP_DATA_2, STR("Temperature_2"), STR("C"), MB_DEVICE_ADDR2, MB_PARAM_HOLDING, 0, 2,
                        0, PARAM_TYPE_FLOAT, 4, OPTS( 20, 30, 1 ), PAR_PERMS_READ_WRITE_TRIGGER },
    };
    // Calculate number of parameters in the table
    uint16_t num_device_parameters = (sizeof(device_parameters) / sizeof(device_parameters[0]));

During initialization of the Modbus stack, a pointer to the Data Dictionary (called descriptor) must be provided as the parameter of the function below. 

:cpp:func:`mbc_master_set_descriptor`: Initialization of master descriptor.

.. code:: c

    ESP_ERROR_CHECK(mbc_master_set_descriptor(&device_parameters[0], num_device_parameters));
    
The Data Dictionary can be initialized from SD card, MQTT or other source before start of stack. Once the initialization and setup is done, the Modbus controller allows the reading of complex parameters from any slave included in descriptor table using its CID.

.. _modbus_api_master_setup_communication_options:

Master Communication Options
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Calling the setup function allows for specific communication options to be defined for port.

:cpp:func:`mbc_master_setup`

The communication structure provided as a parameter is different for serial and TCP communication mode.

Example setup for serial port:

.. code:: c

    mb_communication_info_t comm_info = {
        .port = MB_PORT_NUM,        // Serial port number 
        .mode = MB_MODE_RTU,        // Modbus mode of communication (MB_MODE_RTU or MB_MODE_ASCII)
        .baudrate = 9600,           // Modbus communication baud rate
        .parity = MB_PARITY_NONE    // parity option for serial port
    };

    ESP_ERROR_CHECK(mbc_master_setup((void*)&comm_info));

Modbus master TCP port requires additional definition of IP address table where number of addresses should be equal to number of unique slave addresses in master Modbus Data Dictionary:

The order of IP address string corresponds to short slave address in the Data Dictionary.

.. code:: c

    #define MB_SLAVE_COUNT 2 // Number of slaves in the segment being accessed (as defined in Data Dictionary)

    char* slave_ip_address_table[MB_SLAVE_COUNT] = {
        "192.168.1.2",     // Address corresponds to UID1 and set to predefined value by user
        "192.168.1.3",     // corresponds to UID2 in the segment
        NULL               // end of table
    };

    mb_communication_info_t comm_info = { 
        .ip_port = MB_TCP_PORT,                    // Modbus TCP port number (default = 502)
        .ip_addr_type = MB_IPV4,                   // version of IP protocol
        .ip_mode = MB_MODE_TCP,                    // Port communication mode
        .ip_addr = (void*)slave_ip_address_table,  // assign table of IP addresses
        .ip_netif_ptr = esp_netif_ptr              // esp_netif_ptr pointer to the corresponding network interface
    };

    ESP_ERROR_CHECK(mbc_master_setup((void*)&comm_info));

.. note:: Refer to :doc:`esp_netif component <../network/esp_netif>` for more information about network interface initialization.

The slave IP addresses in the table can be assigned automatically using mDNS service as described in the example.
Refer to :example:`protocols/modbus/tcp/mb_tcp_master` for more information.

.. note:: RS485 communication requires call to UART specific APIs to setup communication mode and pins. Refer to :ref:`uart-api-running-uart-communication` section of UART documentation.


.. _modbus_api_master_start_communication:

Master Communication
^^^^^^^^^^^^^^^^^^^^

The starting of the Modbus controller is the final step in enabling communication. This is performed using function below:

:cpp:func:`mbc_master_start`

.. code:: c
    
    esp_err_t err = mbc_master_start();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "mb controller start fail, err=%x.", err);
    }
    
The list of functions below are used by the Modbus master stack from a user's application:

:cpp:func:`mbc_master_send_request`: This function executes a blocking Modbus request. The master sends a data request (as defined in parameter request structure :cpp:type:`mb_param_request_t`) and then blocks until a response from corresponding slave and returns the status of command execution. This function provides a standard way for read/write access to Modbus devices in the network.

:cpp:func:`mbc_master_get_cid_info`: The function gets information about each characteristic supported in the data dictionary and returns the characteristic's description in the form of the :cpp:type:`mb_parameter_descriptor_t` structure. Each characteristic is accessed using its CID.

:cpp:func:`mbc_master_get_parameter`: The function reads the data of a characteristic defined in the parameters of a Modbus slave device. The additional data for request is taken from parameter description table.

Example: 

.. code:: c
    
    const mb_parameter_descriptor_t* param_descriptor = NULL;
    uint8_t temp_data[4] = {0}; // temporary buffer to hold maximum CID size
    uint8_t type = 0;
    ....
    
    // Get the information for characteristic cid from data dictionary
    esp_err_t err = mbc_master_get_cid_info(cid, &param_descriptor);
    if ((err != ESP_ERR_NOT_FOUND) && (param_descriptor != NULL)) {
        err = mbc_master_get_parameter(param_descriptor->cid, (char*)param_descriptor->param_key, (uint8_t*)temp_data, &type);
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "Characteristic #%d %s (%s) value = (0x%08x) read successful.",
                             param_descriptor->cid,
                             (char*)param_descriptor->param_key,
                             (char*)param_descriptor->param_units,
                             *(uint32_t*)temp_data);
        } else {
            ESP_LOGE(TAG, "Characteristic #%d (%s) read fail, err = 0x%x (%s).",
                            param_descriptor->cid,
                            (char*)param_descriptor->param_key,
                            (int)err,
                            (char*)esp_err_to_name(err));
        }
    } else {
        ESP_LOGE(TAG, "Could not get information for characteristic %d.", cid);
    }


:cpp:func:`mbc_master_set_parameter`

The function writes characteristic's value defined as a name and cid parameter in corresponded slave device. The additional data for parameter request is taken from master parameter description table.

.. code:: c

        uint8_t type = 0; // Type of parameter
        uint8_t temp_data[4] = {0}; // temporary buffer
        
        esp_err_t err = mbc_master_set_parameter(CID_TEMP_DATA_2, "Temperature_2", (uint8_t*)temp_data, &type);
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "Set parameter data successfully.");
        } else {
            ESP_LOGE(TAG, "Set data fail, err = 0x%x (%s).", (int)err, (char*)esp_err_to_name(err));
        }


.. _modbus_api_master_destroy:

Modbus Master Teardown
^^^^^^^^^^^^^^^^^^^^^^

This function stops Modbus communication stack and destroys controller interface and free all used active objects.  

:cpp:func:`mbc_master_destroy`

.. code:: c

    ESP_ERROR_CHECK(mbc_master_destroy());


.. _modbus_api_slave_overview:

Modbus Slave API Overview
-------------------------

The sections below represent typical programming workflow for the slave API which should be called in following order:

1. :ref:`modbus_api_port_initialization` - Initialization of Modbus controller interface for the selected port.
2. :ref:`modbus_api_slave_configure_descriptor` - Configure data descriptors to access slave parameters.
3. :ref:`modbus_api_slave_setup_communication_options` - Allows to setup communication options for selected port.
4. :ref:`modbus_api_slave_communication` - Start stack and sending / receiving data. Filter events when master accesses the register areas.
5. :ref:`modbus_api_slave_destroy` - Destroy Modbus controller and its resources.

.. _modbus_api_slave_configure_descriptor:

Configuring Slave Data Access
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The following functions must be called when the Modbus controller slave port is already initialized. Refer to :ref:`modbus_api_port_initialization`.

The slave stack requires the user to define structures (memory storage areas) that store the Modbus parameters accessed by stack. These structures should be prepared by the user and be assigned to the Modbus controller interface using :cpp:func:`mbc_slave_set_descriptor` API call before the start of communication. The slave task can call the :cpp:func:`mbc_slave_check_event` function which will block until the Modbus master access the slave. The slave task can then get information about the data being accessed.

.. note:: One slave can define several area descriptors per each type of Modbus register area with different start_offset.

Register area is defined by using the :cpp:type:`mb_register_area_descriptor_t` structure. 

.. list-table:: Table 3 Modbus register area descriptor 
  :widths: 8 92
  :header-rows: 1

  * - Field
    - Description
  * - ``start_offset``
    - Zero based register relative offset for defined register area. Example: register address = 40002 ( 4x register area - Function 3 - holding register ), start_offset = 2 
  * - ``type``
    - Type of the Modbus register area. Refer to :cpp:type:`mb_param_type_t` for more information.
  * - ``address``
    - A pointer to the memory area which is used to store the register data for this area descriptor.
  * - ``size``
    - The size of the memory area in bytes which is used to store register data.
    
:cpp:func:`mbc_slave_set_descriptor`

The function initializes Modbus communication descriptors for each type of Modbus register area (Holding Registers, Input Registers, Coils (single bit output), Discrete Inputs). Once areas are initialized and the :cpp:func:`mbc_slave_start()` API is called the Modbus stack can access the data in user data structures by request from master.

.. code:: c

    #define MB_REG_INPUT_START_AREA0    (0)
    #define MB_REG_HOLDING_START_AREA0  (0)
    #define MB_REG_HOLD_CNT             (100)
    #define MB_REG_INPUT_CNT            (100)

    mb_register_area_descriptor_t reg_area; // Modbus register area descriptor structure
    unit16_t holding_reg_area[MB_REG_HOLD_CNT] = {0}; // storage area for holding registers 
    unit16_t input_reg_area[MB_REG_INPUT_CNT] = {0}; // storage area for input registers 

    reg_area.type = MB_PARAM_HOLDING;                               // Set type of register area
    reg_area.start_offset = MB_REG_HOLDING_START_AREA0;             // Offset of register area in Modbus protocol
    reg_area.address = (void*)&holding_reg_area[0];                 // Set pointer to storage instance
    reg_area.size = sizeof(holding_reg_area) << 1;                  // Set the size of register storage area in bytes
    ESP_ERROR_CHECK(mbc_slave_set_descriptor(reg_area));
    
    reg_area.type = MB_PARAM_INPUT;
    reg_area.start_offset = MB_REG_INPUT_START_AREA0;
    reg_area.address = (void*)&input_reg_area[0];
    reg_area.size = sizeof(input_reg_area) << 1;
    ESP_ERROR_CHECK(mbc_slave_set_descriptor(reg_area));


At least one area descriptor per each Modbus register type must be set in order to provide register access to its area. If the master tries to access an undefined area, the stack will generate a Modbus exception.

Direct access to register area from user application must be protected by critical section:

.. code:: c

    portENTER_CRITICAL(&param_lock);
    holding_reg_area[2] += 10;
    portEXIT_CRITICAL(&param_lock);


.. _modbus_api_slave_setup_communication_options:

Slave Communication Options
^^^^^^^^^^^^^^^^^^^^^^^^^^^

The function initializes the Modbus controller interface and its active context (tasks, RTOS objects and other resources).

:cpp:func:`mbc_slave_setup`

The function is used to setup communication parameters of the Modbus stack.

Example initialization of Modbus TCP communication:

.. code:: c
    
    esp_netif_init();
    ...
    
    mb_communication_info_t comm_info = { 
        .ip_port = MB_TCP_PORT,                    // Modbus TCP port number (default = 502)
        .ip_addr_type = MB_IPV4,                   // version of IP protocol
        .ip_mode = MB_MODE_TCP,                    // Port communication mode
        .ip_addr = NULL,                           // This field keeps the client IP address to bind, NULL - bind to any client
        .ip_netif_ptr = esp_netif_ptr              // esp_netif_ptr - pointer to the corresponding network interface
    };
    
    // Setup communication parameters and start stack
    ESP_ERROR_CHECK(mbc_slave_setup((void*)&comm_info));

Example initialization of Modbus serial communication:

.. code:: c
    
    #define MB_SLAVE_DEV_SPEED 9600
    #define MB_SLAVE_ADDR 1
    #define MB_SLAVE_PORT_NUM 2
    ...
    
    // Setup communication parameters and start stack
    mb_communication_info_t comm_info = { 
        .mode = MB_MODE_RTU,                    // Communication type
        .slave_addr = MB_SLAVE_ADDR,            // Short address of the slave
        .port = MB_SLAVE_PORT_NUM,              // UART physical port number
        .baudrate = MB_SLAVE_DEV_SPEED,         // Baud rate for communication
        .parity = MB_PARITY_NONE                // Parity option
    };
    
    ESP_ERROR_CHECK(mbc_slave_setup((void*)&comm_info));

.. _modbus_api_slave_communication:

Slave Communication
^^^^^^^^^^^^^^^^^^^

The function below is used to start Modbus controller interface and allows communication.  

:cpp:func:`mbc_slave_start`

.. code:: c

    ESP_ERROR_CHECK(mbc_slave_start());

:cpp:func:`mbc_slave_check_event`

The blocking call to function waits for a event specified (represented as an event mask parameter). Once the master accesses the parameter and the event mask matches the parameter type, the application task will be unblocked and function will return the corresponding event :cpp:type:`mb_event_group_t` which describes the type of register access being done.

:cpp:func:`mbc_slave_get_param_info`

The function gets information about accessed parameters from the Modbus controller event queue. The KConfig :ref:`CONFIG_FMB_CONTROLLER_NOTIFY_QUEUE_SIZE` key can be used to configure the notification queue size. The timeout parameter allows a timeout to be specified when waiting for a notification. The :cpp:type:`mb_param_info_t` structure contains information about accessed parameter.

.. list-table:: Table 4 Description of the register info structure: :cpp:type:`mb_param_info_t`
  :widths: 10 90
  :header-rows: 1
  
  * - Field
    - Description
  * - ``time_stamp``
    - the time stamp of the event when defined parameter is accessed 
  * - ``mb_offset``
    - start Modbus register accessed by master
  * - ``type``
    - type of the Modbus register area being accessed (See the :cpp:type:`mb_event_group_t` for more information)
  * - ``address``
    - memory address that corresponds to accessed register in defined area descriptor
  * - ``size``
    - number of registers being accessed by master

Example to get event when holding or input registers accessed in the slave:

.. code:: c

    #define MB_READ_MASK            (MB_EVENT_INPUT_REG_RD | MB_EVENT_HOLDING_REG_RD)
    #define MB_WRITE_MASK           (MB_EVENT_HOLDING_REG_WR)
    #define MB_READ_WRITE_MASK      (MB_READ_MASK | MB_WRITE_MASK)
    #define MB_PAR_INFO_GET_TOUT    (10 / portTICK_RATE_MS)                           
    ....
                                                
    // The function blocks while waiting for register access
    mb_event_group_t event = mbc_slave_check_event(MB_READ_WRITE_MASK);
    
    // Get information about data accessed from master 
    ESP_ERROR_CHECK(mbc_slave_get_param_info(&reg_info, MB_PAR_INFO_GET_TOUT));
    const char* rw_str = (event & MB_READ_MASK) ? "READ" : "WRITE";
    
    // Filter events and process them accordingly
    if (event & (MB_EVENT_HOLDING_REG_WR | MB_EVENT_HOLDING_REG_RD)) {
        ESP_LOGI(TAG, "HOLDING %s (%u us), ADDR:%u, TYPE:%u, INST_ADDR:0x%.4x, SIZE:%u",
                    rw_str,
                    (uint32_t)reg_info.time_stamp,
                    (uint32_t)reg_info.mb_offset,
                    (uint32_t)reg_info.type,
                    (uint32_t)reg_info.address,
                    (uint32_t)reg_info.size);
    } else if (event & (MB_EVENT_INPUT_REG_RD)) {
        ESP_LOGI(TAG, "INPUT %s (%u us), ADDR:%u, TYPE:%u, INST_ADDR:0x%.4x, SIZE:%u",
                    rw_str,
                    (uint32_t)reg_info.time_stamp,
                    (uint32_t)reg_info.mb_offset,
                    (uint32_t)reg_info.type,
                    (uint32_t)reg_info.address,
                    (uint32_t)reg_info.size);
    }
    
.. _modbus_api_slave_destroy:

Modbus Slave Teardown
^^^^^^^^^^^^^^^^^^^^^

This function stops the Modbus communication stack, destroys the controller interface, and frees all used active objects allocated for the slave.  

:cpp:func:`mbc_slave_destroy`

.. code:: c

    ESP_ERROR_CHECK(mbc_slave_destroy());

Possible Communication Issues And Solutions
-------------------------------------------

If the examples do not work as expected and slave and master boards are not able to communicate correctly, it is possible to find the reason for errors. The most important errors are described in master example output and formatted as below:

.. highlight:: none

::

    E (1692332) MB_CONTROLLER_MASTER: mbc_master_get_parameter(111): SERIAL master get parameter failure error=(0x107) (ESP_ERR_TIMEOUT).


.. list-table:: Table 5 Modbus error codes and troubleshooting
  :widths: 5 30 65
  :header-rows: 1

  * - Error
    - Description
    - Possible solution
  * - 0x106
    - ``ESP_ERR_NOT_SUPPORTED``  - Invalid register request - slave returned an exception because the requested register is not supported.
    - Refer to slave register map. Check the master data dictionary for correctness.
  * - 0x107
    - ``ESP_ERR_TIMEOUT`` - Slave response timeout - Modbus slave did not send response during configured slave response timeout.
    - Measure and increase the maximum slave response timeout `idf.py menuconfig`, option :ref:`CONFIG_FMB_MASTER_TIMEOUT_MS_RESPOND`.
      Check physical connection or network configuration and make sure that the slave response can reach the master side.
      If the application has some high performance tasks with higher priority than :ref:`CONFIG_FMB_PORT_TASK_PRIO` it is recommended to place Modbus tasks on the other core using an option :ref:`CONFIG_FMB_PORT_TASK_AFFINITY`.
      Configure the Modbus task's priority :ref:`CONFIG_FMB_PORT_TASK_PRIO` to ensure that the task gets sufficient processing time to handle Modbus stack events.
  * - 0x108
    - ``ESP_ERR_INVALID_RESPONSE`` - Received unsupported response from slave or frame check failure. Master can not execute command handler because the command is either not supported or is incorrect.
    - Check the physical connection then refer to register map of your slave to configure the master data dictionary properly.
  * - 0x103
    - ``ESP_ERR_INVALID_STATE`` - Critical failure or FSM sequence failure or master FSM is busy processing previous request.
    - Make sure your physical connection is working properly. Increase task stack size and check Modbus initialization sequence.

Application Example
-------------------

The examples below use the FreeModbus library port for serial TCP slave and master implementations accordingly. The selection of stack is performed through KConfig menu option "Enable Modbus stack support ..." for appropriate communication mode and related configuration keys.

- :example:`protocols/modbus/serial/mb_slave`
- :example:`protocols/modbus/serial/mb_master`
- :example:`protocols/modbus/tcp/mb_tcp_slave`
- :example:`protocols/modbus/tcp/mb_tcp_master`

Please refer to the specific example README.md for details.

Protocol References
-------------------

    -  ``https://modbus.org/specs.php``: Modbus Organization with protocol specifications.

API Reference
-------------

.. include-build-file:: inc/esp_modbus_common.inc
.. include-build-file:: inc/esp_modbus_master.inc
.. include-build-file:: inc/esp_modbus_slave.inc

