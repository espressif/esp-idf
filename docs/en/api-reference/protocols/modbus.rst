ESP-Modbus
==========

Overview
--------


The Modbus serial communication protocol is de facto standard protocol widely used to connect industrial electronic devices. Modbus allows communication among many devices connected to the same network, for example, a system that measures temperature and humidity and communicates the results to a computer. The Modbus protocol uses several types of data: Holding Registers, Input Registers, Coils (single bit output), Discrete Inputs. Versions of the Modbus protocol exist for serial port and for Ethernet and other protocols that support the Internet protocol suite. 
There are many variants of Modbus protocols, some of them are:


    * ``Modbus RTU`` — This is used in serial communication and makes use of a compact, binary representation of the data for protocol communication. The RTU format follows the commands/data with a cyclic redundancy check checksum as an error check mechanism to ensure the reliability of data. Modbus RTU is the most common implementation available for Modbus. A Modbus RTU message must be transmitted continuously without inter-character hesitations. Modbus messages are framed (separated) by idle (silent) periods. The RS-485 interface communication is usually used for this type.
    * ``Modbus ASCII`` — This is used in serial communication and makes use of ASCII characters for protocol communication. The ASCII format uses a longitudinal redundancy check checksum. Modbus ASCII messages are framed by leading colon (":") and trailing newline (CR/LF).
    * ``Modbus TCP/IP or Modbus TCP`` — This is a Modbus variant used for communications over TCP/IP networks, connecting over port 502. It does not require a checksum calculation, as lower layers already provide checksum protection.


Modbus slave interface API overview
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


ESP-IDF supports Modbus slave protocol and provides modbus_controller interface API to interact with user application. The interface API functions below are used to setup and use Modbus slave stack from application and could be executed in next order:


The files deviceparams.c/h contain the user structures which represent Modbus parameters accessed by stack. These parameters should be prepared by user and be assigned to the modbus_controller interface using :cpp:func:`mbcontroller_set_descriptor()` API call before start of communication.  

.. doxygenfunction:: mbcontroller_init

The function initializes the Modbus controller interface and its active context (tasks, RTOS objects and other resources).

.. doxygenfunction:: mbcontroller_setup

The function is used to setup communication parameters of the Modbus stack. See the Modbus controller API documentation for more information.

.. doxygenfunction:: mbcontroller_set_descriptor

The function initializes Modbus communication descriptors for each type of Modbus register area (Holding Registers, Input Registers, Coils (single bit output), Discrete Inputs). Once areas are initialized and the :cpp:func:`mbcontroller_start()` API is called the Modbus stack can access the data in user data structures by request from master. See the :cpp:type:`mb_register_area_descriptor_t` for more information.

.. doxygenfunction:: mbcontroller_start

Modbus controller start function. Starts stack and interface and allows communication.

.. doxygenfunction:: mbcontroller_check_event

The blocking call to function waits for event specified in the input parameter as event mask. Once master access the parameter and event mask matches the parameter the application task will be unblocked and function will return ESP_OK. See the :cpp:type:`mb_event_group_t` for more information about Modbus event masks.

.. doxygenfunction:: mbcontroller_get_param_info

The function gets information about accessed parameters from modbus controller event queue. The KConfig 'CONFIG_MB_CONTROLLER_NOTIFY_QUEUE_SIZE' key can be used to configure the notification queue size. The timeout parameter allows to specify timeout for waiting notification. The :cpp:type:`mb_param_info_t` structure contain information about accessed parameter.

.. doxygenfunction:: mbcontroller_destroy

This function stops Modbus communication stack and destroys controller interface.

There are some configuration parameters modbus_controller interface and Modbus stack can be configured using KConfig values in "Modbus configuration" menu. See the example application for more information about how to use these API functions.


Application Example
-------------------
The example uses the FreeModbus library port for slave implementation: 

:example:`protocols/modbus_slave`

