Bluetooth\ :sup:`®` Common
===============================

:link_to_translation:`zh_CN:[中文]`

**Bluetooth Common** provides shared definitions and APIs used by both Bluetooth Classic and Bluetooth Low Energy (Bluetooth LE). It serves as a foundation for shared functionality across Bluetooth components, offering unified interfaces for Bluetooth initialization, configuration, and device management.

The Bluetooth Common API is organized into the following parts:

.. toctree::
   :maxdepth: 1

   Bluetooth Define <esp_bt_defs>

Provides shared definitions and data structures used by both Bluetooth Classic and Bluetooth LE.

.. toctree::
   :maxdepth: 1

   Bluetooth Main <esp_bt_main>

Provides core APIs for initializing, enabling/disabling, and managing the Bluetooth stack.

.. toctree::
   :maxdepth: 1

   Bluetooth Device <esp_bt_device>

Provides APIs for managing device-level properties such as address, name, visibility, and coexistence settings.

Each part typically includes the following sections:

- **Overview**: Summary of purpose, key functionality, and main interfaces
- **Application Examples**: Example projects demonstrating typical usage
- **API Reference**: Detailed API documentation, including header files, functions, structures, macros, type definitions, and enumerations
