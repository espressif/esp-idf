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

The function gets information about accessed parameters from the Modbus controller event queue. The KConfig ``CONFIG_FMB_CONTROLLER_NOTIFY_QUEUE_SIZE`` key can be used to configure the notification queue size. The timeout parameter allows a timeout to be specified when waiting for a notification. The :cpp:type:`mb_param_info_t` structure contains information about accessed parameter.

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