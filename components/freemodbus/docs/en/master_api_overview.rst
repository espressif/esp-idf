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

.. note:: Refer to `esp_netif component <https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_netif.html>`__ for more information about network interface initialization.

The slave IP addresses in the table can be assigned automatically using mDNS service as described in the example.
Refer to :ref:`example TCP master <example_mb_tcp_master>` for more information.

.. note:: RS485 communication requires call to UART specific APIs to setup communication mode and pins. Refer to the `UART communication section <https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/uart.html#uart-api-running-uart-communication>`__ in documentation.


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
