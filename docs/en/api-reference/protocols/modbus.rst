ESP-Modbus
==========

:link_to_translation:`zh_CN:[中文]`

The Espressif ESP-Modbus Library (esp-modbus) supports Modbus communication in the networks based on RS485, Wi-Fi, and Ethernet interfaces.
Since ESP-IDF version v5.0, the component ``freemodbus`` has been moved from ESP-IDF to a separate repository:

* `ESP-Modbus component on GitHub <https://github.com/espressif/esp-modbus>`__

Hosted Documentation
--------------------

The documentation can be found through the link below:

* `ESP-Modbus documentation (English) <https://docs.espressif.com/projects/esp-modbus>`__

Application Examples
--------------------

The examples below demonstrate the ESP-Modbus library of serial and TCP ports for both slave and master implementations respectively.

- :example:`protocols/modbus/serial/mb_slave` demonstrates how to use {IDF_TARGET_NAME} as a Modbus serial slave device with the esp-modbus stack, enabling an external Modbus host to read and write device parameters using the Modbus protocol.

- :example:`protocols/modbus/serial/mb_master` demonstrates how to use the esp-modbus stack port on {IDF_TARGET_NAME} as a Modbus serial master device, capable of reading and writing values from slave devices in a Modbus segment.

- :example:`protocols/modbus/tcp/mb_tcp_slave` demonstrates the esp-modbus TCP slave stack port, allowing an external Modbus host to read and write device parameters via the Modbus protocol.

- :example:`protocols/modbus/tcp/mb_tcp_master` demonstrates how to use the esp-modbus stack port on {IDF_TARGET_NAME} as a Modbus TCP master device, capable of reading and writing values from slave devices in a Modbus network.

Please refer to the ``README.md`` documents of each specific example for details.

Protocol References
-------------------

    - For the detailed protocol specifications, see `The Modbus Organization <https://modbus.org/specs.php>`_.
