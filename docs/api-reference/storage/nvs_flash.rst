.. include:: ../../../components/nvs_flash/README.rst

Application Example
-------------------

Two examples are provided in :example:`storage` directory of ESP-IDF examples:

:example:`storage/nvs_rw_value`

  Demonstrates how to read and write a single integer value using NVS.

  The value holds the number of ESP32 module restarts. Since it is written to NVS, the value is preserved between restarts.

  Example also shows how to check if read / write operation was successful, or certain value is not initialized in NVS. Diagnostic is provided in plain text to help track program flow and capture any issues on the way.

:example:`storage/nvs_rw_blob`

  Demonstrates how to read and write a single integer value and a blob (binary large object) using NVS to preserve them between ESP32 module restarts.

    * value - tracks number of ESP32 module soft and hard restarts.
    * blob - contains a table with module run times. The table is read from NVS to dynamically allocated RAM. New run time is added to the table on each manually triggered soft restart and written back to NVS. Triggering is done by pulling down GPIO0.

  Example also shows how to implement diagnostics if read / write operation was successful.


API Reference
-------------

.. include:: /_build/inc/nvs_flash.inc

.. include:: /_build/inc/nvs.inc


