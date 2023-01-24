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
    - Measure and increase the maximum slave response timeout `idf.py menuconfig`, option ``CONFIG_FMB_MASTER_TIMEOUT_MS_RESPOND``.
      Check physical connection or network configuration and make sure that the slave response can reach the master side.
      If the application has some high performance tasks with higher priority than ``CONFIG_FMB_PORT_TASK_PRIO`` it is recommended to place Modbus tasks on the other core using an option ``CONFIG_FMB_PORT_TASK_AFFINITY``.
      Configure the Modbus task's priority ``CONFIG_FMB_PORT_TASK_PRIO`` to ensure that the task gets sufficient processing time to handle Modbus stack events.
  * - 0x108
    - ``ESP_ERR_INVALID_RESPONSE`` - Received unsupported response from slave or frame check failure. Master can not execute command handler because the command is either not supported or is incorrect.
    - Check the physical connection then refer to register map of your slave to configure the master data dictionary properly.
  * - 0x103
    - ``ESP_ERR_INVALID_STATE`` - Critical failure or FSM sequence failure or master FSM is busy processing previous request.
    - Make sure your physical connection is working properly. Increase task stack size and check Modbus initialization sequence.

Application Example
-------------------

The examples below use the FreeModbus library port for serial TCP slave and master implementations accordingly. The selection of stack is performed through KConfig menu option "Enable Modbus stack support ..." for appropriate communication mode and related configuration keys.

.. _example_mb_slave:

- `Modbus serial slave example <https://github.com/espressif/esp-idf/tree/master/examples/protocols/modbus/serial/mb_slave>`__

.. _example_mb_master:

- `Modbus serial master example <https://github.com/espressif/esp-idf/tree/master/examples/protocols/modbus/serial/mb_master>`__

.. _example_mb_tcp_master:

- `Modbus TCP master example <https://github.com/espressif/esp-idf/tree/master/examples/protocols/modbus/tcp/mb_tcp_master>`__

.. _example_mb_tcp_slave:

- `Modbus TCP slave example <https://github.com/espressif/esp-idf/tree/master/examples/protocols/modbus/tcp/mb_tcp_slave>`__

Please refer to the specific example README.md for details.

.. _modbus_organization:

Protocol References
-------------------

    - `Modbus Organization with protocol specifications <https://modbus.org/specs.php>`__

API Reference
-------------

.. include-build-file:: inc/esp_modbus_common.inc
.. include-build-file:: inc/esp_modbus_master.inc
.. include-build-file:: inc/esp_modbus_slave.inc

