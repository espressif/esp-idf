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

Application Example
-------------------

The examples below demonstrate the ESP-Modbus library of serial and TCP ports for both slave and master implementations respectively.

- :example:`protocols/modbus/serial/mb_slave`
- :example:`protocols/modbus/serial/mb_master`
- :example:`protocols/modbus/tcp/mb_tcp_slave`
- :example:`protocols/modbus/tcp/mb_tcp_master`

Please refer to the ``README.md`` documents of each specific example for details.

Protocol References
-------------------

    - For the detailed protocol specifications, see `The Modbus Organization <https://modbus.org/specs.php>`_.


