ESP-Modbus
==========

Overview
--------


The Modbus serial communication protocol is de facto standard protocol widely used to connect industrial electronic devices. Modbus allows communication among many devices connected to the same network, for example, a system that measures temperature and humidity and communicates the results to a computer. The Modbus protocol uses several types of data: Holding Registers, Input Registers, Coils (single bit output), Discrete Inputs. Versions of the Modbus protocol exist for serial port and for Ethernet and other protocols that support the Internet protocol suite. 
There are many variants of Modbus protocols, some of them are:


    * ``Modbus RTU`` — This is used in serial communication and makes use of a compact, binary representation of the data for protocol communication. The RTU format follows the commands/data with a cyclic redundancy check checksum as an error check mechanism to ensure the reliability of data. Modbus RTU is the most common implementation available for Modbus. A Modbus RTU message must be transmitted continuously without inter-character hesitations. Modbus messages are framed (separated) by idle (silent) periods. The RS-485 interface communication is usually used for this type.
    * ``Modbus ASCII`` — This is used in serial communication and makes use of ASCII characters for protocol communication. The ASCII format uses a longitudinal redundancy check checksum. Modbus ASCII messages are framed by leading colon (":") and trailing newline (CR/LF).
    * ``Modbus TCP/IP or Modbus TCP`` — This is a Modbus variant used for communications over TCP/IP networks, connecting over port 502. It does not require a checksum calculation, as lower layers already provide checksum protection.

    
Modbus common interface API overview
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


The API functions below provide common functionality to setup Modbus stack for slave and master implementation accordingly. ISP-IDF supports Modbus serial slave and master protocol stacks and provides modbus_controller interface API to interact with user application.

.. doxygenfunction:: mbc_slave_init
.. doxygenfunction:: mbc_master_init

The function initializes the Modbus controller interface and its active context (tasks, RTOS objects and other resources).

.. doxygenfunction:: mbc_slave_setup
.. doxygenfunction:: mbc_master_setup

The function is used to setup communication parameters of the Modbus stack. See the Modbus controller API documentation for more information.

:cpp:func:`mbc_slave_set_descriptor`: Initialization of slave descriptor.

:cpp:func:`mbc_master_set_descriptor`: Initialization of master descriptor.

The Modbus stack uses parameter description tables (descriptors) for communication. These are different for master and slave implementation of stack and should be assigned by the API call before start of communication.

.. doxygenfunction:: mbc_slave_start
.. doxygenfunction:: mbc_master_start

Modbus controller start function. Starts stack and interface and allows communication.  

.. doxygenfunction:: mbc_slave_destroy
.. doxygenfunction:: mbc_master_destroy

This function stops Modbus communication stack and destroys controller interface.  

There are some configurable parameters of modbus_controller interface and Modbus stack that can be configured using KConfig values in "Modbus configuration" menu. The most important option in KConfig menu is "Selection of Modbus stack support mode" that allows to select master or slave stack for implementation. See the examples for more information about how to use these API functions.
    

Modbus serial slave interface API overview
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


The slave stack requires the user defined structures which represent Modbus parameters accessed by stack. These structures should be prepared by user and be assigned to the modbus_controller interface using :cpp:func:`mbc_slave_set_descriptor()` API call before start of communication.
The interface API functions below are used for Modbus slave application:

.. doxygenfunction:: mbc_slave_set_descriptor

The function initializes Modbus communication descriptors for each type of Modbus register area (Holding Registers, Input Registers, Coils (single bit output), Discrete Inputs). Once areas are initialized and the :cpp:func:`mbc_slave_start()` API is called the Modbus stack can access the data in user data structures by request from master. See the :cpp:type:`mb_register_area_descriptor_t` and example for more information.

.. doxygenfunction:: mbc_slave_check_event

The blocking call to function waits for event specified in the input parameter as event mask. Once master access the parameter and event mask matches the parameter the application task will be unblocked and function will return ESP_OK. See the :cpp:type:`mb_event_group_t` for more information about Modbus event masks.

.. doxygenfunction:: mbc_slave_get_param_info

The function gets information about accessed parameters from modbus controller event queue. The KConfig 'CONFIG_FMB_CONTROLLER_NOTIFY_QUEUE_SIZE' key can be used to configure the notification queue size. The timeout parameter allows to specify timeout for waiting notification. The :cpp:type:`mb_param_info_t` structure contain information about accessed parameter.


Modbus serial master interface API overview
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


The Modbus master implementation requires parameter description table be defined before start of stack. This table describes characteristics (physical parameters like temperature, humidity, etc.) and links them to Modbus registers in specific slave device in the Modbus segment. The table has to be assigned to the modbus_controller interface using :cpp:func:`mbc_master_set_descriptor()` API call before start of communication.

Below are the interface API functions that are used to setup and use Modbus master stack from user application and can be executed in next order:

.. doxygenfunction:: mbc_master_set_descriptor

Assigns parameter description table for Modbus controller interface. The table has to be prepared by user according to particular 

.. doxygenfunction:: mbc_master_send_request

This function sends data request as defined in parameter request, waits response from corresponded slave and returns status of command execution. This function provides a standard way for read/write access to Modbus devices in the network.

.. doxygenfunction:: mbc_master_get_cid_info

The function gets information about supported characteristic defined as cid. It will check if characteristic is supported and returns its description.

.. doxygenfunction:: mbc_master_get_parameter

The function reads data of characteristic defined in parameters from Modbus slave device and returns its data. The additional data for request is taken from parameter description table.

.. doxygenfunction:: mbc_master_set_parameter

The function writes characteristic's value defined as a name and cid parameter in corresponded slave device. The additional data for parameter request is taken from master parameter description table.

Application Example
-------------------


The examples below use the FreeModbus library port for slave and master implementation accordingly. The selection of stack is performed through KConfig menu "Selection of Modbus stack support mode" and related configuration keys.

:example:`protocols/modbus_slave`

:example:`protocols/modbus_master`

Please refer to the specific example README.md for details.

