# ESP-Modbus Library

## Overview

An Espressif ESP-Modbus Library (esp-modbus) is a library to support Modbus communication in the networks based on RS485, WiFi, Ethernet interfaces. The Modbus is a data communications protocol originally published by Modicon (now Schneider Electric) in 1979 for use with its programmable logic controllers (PLCs).

* [ESP-Modbus component on GitHub](https://www.github.com/espressif/esp-modbus)

This library is to be used with Espressif’s IoT Development Framework, [ESP_IDF](https://github.com/espressif/esp-idf). The packages from this repository are uploaded to Espressif’s component repository.

* [esp-modbus component in component repository](https://components.espressif.com/component/espressif/esp-modbus)

You can add the component to your project via `idf.py add-dependency`. More information about idf-component-manager can be found in [Espressif API guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/tools/idf-component-manager.html) or [PyPi registry](https://pypi.org/project/idf-component-manager).

The ESP-Modbus library can be used with ESP-IDF v4.1 and later. ESP-IDF v4.x releases include an earlier version of ESP-Modbus library inside freemodbus component. To use ESP-Modbus with these releases, users need to exclude the built-in freemodbus component from the build process, and update application components to depend on esp-modbus component instead. To exclude freemodbus component from compilation, add the following line to the project CMakeLists.txt file:

```
set(EXCLUDE_COMPONENTS freemodbus)
```

ESP-IDF v5.x and later releases do not include freemodbus component, so no extra steps are necessary when adding esp-modbus component.

## Documentation

The documentation can be found on the link below:

* [ESP-Modbus documentation (English)](https://docs.espressif.com/projects/esp-modbus)

## Application Examples

The examples below demonstrate the ESP-Modbus library of serial, TCP ports for slave and master implementations accordingly.

- [Modbus serial slave example](https://github.com/espressif/esp-idf/tree/master/examples/protocols/modbus/serial/mb_slave)

- [Modbus serial master example](https://github.com/espressif/esp-idf/tree/master/examples/protocols/modbus/serial/mb_master)

- [Modbus TCP master example](https://github.com/espressif/esp-idf/tree/master/examples/protocols/modbus/tcp/mb_tcp_master)

- [Modbus TCP slave example](https://github.com/espressif/esp-idf/tree/master/examples/protocols/modbus/tcp/mb_tcp_slave)

Please refer to the specific example README.md for details.

## Protocol References

- [Modbus Organization with protocol specifications](https://modbus.org/specs.php)

## Contributing

We welcome contributions to this project in the form of bug reports, feature requests and pull requests.

Issue reports and feature requests can be submitted using Github Issues: https://github.com/espressif/esp-modbus/issues. Please check if the issue has already been reported before opening a new one.

Contributions in the form of pull requests should follow ESP-IDF project's [contribution guidelines](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/contribute/index.html). We kindly ask developers to start a discussion on an issue before proposing large changes to the project.

## Licence

ESP-Modbus project is based on [FreeMODBUS library](https://github.com/cwalter-at/freemodbus), Copyright (c) 2006 Christian Walter and licensed under the BSD 3-clause license.

Modbus Master related code is Copyright (c) 2013 Armink and licensed under BSD 3-clause license.

All original code in this repository is Copyright (c) 2016-2022 Espressif Systems (Shanghai) Co. Ltd.

The project is distributed under Apache 2.0 license. See the accompanying [LICENSE file](https://github.com/espressif/esp-modbus/blob/master/LICENSE) for a copy.