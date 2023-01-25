ESP-Modbus
==========

The Espressif ESP-Modbus Library (esp-modbus) supports Modbus communication in the networks based on RS485, Wi-Fi, Ethernet interfaces.

Options Of Usage
----------------

There are two options for using the ``ESP-Modbus`` library in your projects:

   1. Using the ``freemodbus`` component included in the current release of ESP-IDF.
     
      The ESP-IDF component ``freemodbus`` included in this release reflects the ``esp-modbus`` component of version v1.0.7. This release of the component is used by default in the existing examples mentioned below on this page.

      The downside of this option is that the ``freemodbus`` component may not contain the latest fixes from esp-modbus repository. Because of this, it is recommended to use the original ``esp-modbus`` component for new projects (the option 2).

   2. Using the ``esp-modbus`` component via the component manager instead of ``freemodbus``.
      
      Additional information for the ``ESP-Modbus`` component can be found in the separate repository:

      * `ESP-Modbus component on GitHub <https://www.github.com/espressif/esp-modbus>`__

      In order to use ``esp-modbus`` component, your application must include the statement below in its ``CMakeLists.txt`` file to exclude the ``freemodbus`` component from the build. The excluding of the ``freemodbus`` component is required to avoid compilation issues when ``esp-modbus`` is used in conjunction with the component manager.

      .. highlight:: none

      ::

         set(EXCLUDE_COMPONENTS freemodbus)
      
      If your project uses the make build system, the statement below needs to be added into project Makefile:

      .. highlight:: none

      ::

         EXCLUDE_COMPONENTS := freemodbus

      The ``main`` component folder of the new application shall include the component manager manifest file ``idf_component.yml`` as below:

      .. highlight:: none

      ::

         dependencies:
            idf: ">=4.1"
            espressif/esp-modbus:
               version: "^1.0"

      This option allows to use the latest ``esp-modbus`` component updates in any projects based on ESP-IDF versions from v4.1 up to v5.0.
      Refer to :doc:`component manager documentation <../../api-guides/tools/idf-component-manager>` for more information on how to set up the component manager. The ``esp-modbus`` component can be found in `component manager registry <https://components.espressif.com/component/espressif/esp-modbus>`__.

Managed Documentation
---------------------

The component documentation can be found on the link below:

* `ESP-Modbus documentation (English) <https://docs.espressif.com/projects/esp-modbus>`__

Application Example
-------------------

The examples below demonstrate the ESP-Modbus library of serial, TCP ports for slave and master implementations accordingly.

- :example:`protocols/modbus/serial/mb_slave`
- :example:`protocols/modbus/serial/mb_master`
- :example:`protocols/modbus/tcp/mb_tcp_slave`
- :example:`protocols/modbus/tcp/mb_tcp_master`

Please refer to the specific example README.md for details.

Protocol References
-------------------

    - `Modbus Organization with protocol specifications <https://modbus.org/specs.php>`__

