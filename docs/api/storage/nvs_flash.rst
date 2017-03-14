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

Header Files
^^^^^^^^^^^^

  * :component_file:`nvs_flash/include/nvs_flash.h`
  * :component_file:`nvs_flash/include/nvs.h`

Macros
^^^^^^

.. doxygendefine:: ESP_ERR_NVS_BASE
.. doxygendefine:: ESP_ERR_NVS_NOT_INITIALIZED
.. doxygendefine:: ESP_ERR_NVS_NOT_FOUND
.. doxygendefine:: ESP_ERR_NVS_TYPE_MISMATCH
.. doxygendefine:: ESP_ERR_NVS_READ_ONLY
.. doxygendefine:: ESP_ERR_NVS_NOT_ENOUGH_SPACE
.. doxygendefine:: ESP_ERR_NVS_INVALID_NAME
.. doxygendefine:: ESP_ERR_NVS_INVALID_HANDLE
.. doxygendefine:: ESP_ERR_NVS_REMOVE_FAILED
.. doxygendefine:: ESP_ERR_NVS_KEY_TOO_LONG
.. doxygendefine:: ESP_ERR_NVS_PAGE_FULL
.. doxygendefine:: ESP_ERR_NVS_INVALID_STATE
.. doxygendefine:: ESP_ERR_NVS_INVALID_LENGTH
.. doxygendefine:: ESP_ERR_NVS_NO_FREE_PAGES

Type Definitions
^^^^^^^^^^^^^^^^

.. doxygentypedef:: nvs_handle

Enumerations
^^^^^^^^^^^^

.. doxygenenum:: nvs_open_mode

Functions
^^^^^^^^^
.. doxygenfunction:: nvs_flash_init
.. doxygenfunction:: nvs_open
.. doxygenfunction:: nvs_set_i8
.. doxygenfunction:: nvs_set_u8
.. doxygenfunction:: nvs_set_i16
.. doxygenfunction:: nvs_set_u16
.. doxygenfunction:: nvs_set_i32
.. doxygenfunction:: nvs_set_u32
.. doxygenfunction:: nvs_set_i64
.. doxygenfunction:: nvs_set_u64
.. doxygenfunction:: nvs_set_str
.. doxygenfunction:: nvs_set_blob
.. doxygenfunction:: nvs_get_i8
.. doxygenfunction:: nvs_get_u8
.. doxygenfunction:: nvs_get_i16
.. doxygenfunction:: nvs_get_u16
.. doxygenfunction:: nvs_get_i32
.. doxygenfunction:: nvs_get_u32
.. doxygenfunction:: nvs_get_i64
.. doxygenfunction:: nvs_get_u64
.. doxygenfunction:: nvs_get_str
.. doxygenfunction:: nvs_get_blob
.. doxygenfunction:: nvs_erase_key
.. doxygenfunction:: nvs_erase_all
.. doxygenfunction:: nvs_commit
.. doxygenfunction:: nvs_close


