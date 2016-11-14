.. include:: ../../components/nvs_flash/README.rst

Application Example
-------------------

`Instructions <http://esp-idf.readthedocs.io/en/latest/api/template.html>`_

API Reference
-------------

Header Files
^^^^^^^^^^^^

  * `nvs_flash/include/nvs_flash.h <https://github.com/espressif/esp-idf/blob/master/components/nvs_flash/include/nvs_flash.h>`_
  * `nvs_flash/include/nvs.h <https://github.com/espressif/esp-idf/blob/master/components/nvs_flash/include/nvs.h>`_

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

Type Definitions
^^^^^^^^^^^^^^^^

.. doxygentypedef:: nvs_handle

Enumerations
^^^^^^^^^^^^

.. doxygenenum:: nvs_open_mode

Functions
^^^^^^^^^
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
.. doxygenfunction:: nvs_flash_init
.. doxygenfunction:: nvs_flash_init_custom

