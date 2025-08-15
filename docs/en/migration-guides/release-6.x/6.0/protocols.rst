ESP-Modbus
==========

:link_to_translation:`zh_CN:[中文]`

The Espressif ESP-Modbus Library (esp-modbus) supports Modbus communication in the networks based on RS485, Wi-Fi, and Ethernet interfaces.

The component ``esp-modbus v2 (v2.x.x)`` is the current supported component version:

* `ESP-Modbus component on GitHub <https://github.com/espressif/esp-modbus/tree/main>`__

Documentation
~~~~~~~~~~~~~

* `ESP-MODBUS stable documentation v2.x.x <https://docs.espressif.com/projects/esp-modbus/en/stable>`__
* `Documentation for legacy version v1.x.x <https://docs.espressif.com/projects/esp-modbus/en/v1>`__

Application Examples
~~~~~~~~~~~~~~~~~~~~

Since ESP-IDF version v6.0, the examples for component ``esp-modbus v1`` which is obsolete have been removed from ESP-IDF.

- `legacy esp-modbus v1.x.x examples (esp-idf v5.5) <https://github.com/espressif/esp-idf/tree/release/v5.5/examples/protocols/modbus>`__

The examples below demonstrate the ESP-Modbus library of serial and TCP ports for both slave and master implementations respectively.

- `mb_serial_slave - demonstrates how to use {IDF_TARGET_NAME} as a Modbus serial slave device with the esp-modbus stack, enabling an external Modbus host to read and write device parameters using the Modbus protocol. <https://github.com/espressif/esp-modbus/tree/main/examples/serial/mb_serial_slave>`__

- `mb_serial_master - demonstrates how to use the esp-modbus stack port on {IDF_TARGET_NAME} as a Modbus serial master device, capable of reading and writing values from slave devices in a Modbus segment. <https://github.com/espressif/esp-modbus/tree/main/examples/serial/mb_serial_master>`__

- `mb_tcp_slave - demonstrates the esp-modbus TCP slave stack port, allowing an external Modbus host to read and write device parameters via the Modbus protocol. <https://github.com/espressif/esp-modbus/tree/main/examples/tcp/mb_tcp_slave>`__

- `mb_tcp_master` - demonstrates how to use the esp-modbus stack port on {IDF_TARGET_NAME} as a Modbus TCP master device, capable of reading and writing values from slave devices in a Modbus network. <https://github.com/espressif/esp-modbus/tree/main/examples/tcp/mb_tcp_master>`__

Please refer to the ``README.md`` documents of each specific example for details.

Discussions
~~~~~~~~~~~

* `Discussions for version v2 <https://github.com/espressif/esp-modbus/discussions>`__
