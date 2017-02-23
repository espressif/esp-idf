OTA
===

Application Example
-------------------
 
Demonstration of OTA (over the air) firmware update workflow: :example:`system/ota`.

API Reference
-------------

Header Files
^^^^^^^^^^^^

  * :component_file:`app_update/include/esp_ota_ops.h`

Macros
^^^^^^

.. doxygendefine:: ESP_ERR_OTA_BASE
.. doxygendefine:: ESP_ERR_OTA_PARTITION_CONFLICT
.. doxygendefine:: ESP_ERR_OTA_SELECT_INFO_INVALID
.. doxygendefine:: ESP_ERR_OTA_VALIDATE_FAILED

Type Definitions
^^^^^^^^^^^^^^^^

.. doxygentypedef:: esp_ota_handle_t

Functions
^^^^^^^^^

.. doxygenfunction:: esp_ota_begin
.. doxygenfunction:: esp_ota_write
.. doxygenfunction:: esp_ota_end
.. doxygenfunction:: esp_ota_set_boot_partition
.. doxygenfunction:: esp_ota_get_boot_partition
