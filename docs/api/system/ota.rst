Over The Air Updates (OTA)
==========================

OTA Process Overview
^^^^^^^^^^^^^^^^^^^^

The OTA update mechanism allows a device to update itself based on data received while the normal firmware is running
(for example, over WiFi or Bluetooth.)

OTA requires configuring the :doc:`Partition Table </partition-tables>` of the device with at least two "OTA app slot"
partitions (ie `ota_0` and `ota_1`) and an "OTA Data Partition".

The OTA operation functions write a new app firmware image to whichever OTA app slot is not currently being used for
booting. Once the image is verified, the OTA Data partition is updated to specify that this image should be used for the
next boot.

.. _ota_data_partition:

OTA Data Partition
^^^^^^^^^^^^^^^^^^

An OTA data partition (type ``data``, subtype ``ota``) must be included in the :doc:`Partition Table </partition-tables>`
of any project which uses the OTA functions.

For factory boot settings, the OTA data partition should contain no data (all bytes erased to 0xFF). In this case the
esp-idf software bootloader will boot the factory app if it is present in the the partition table. If no factory app is
included in the partition table, the first available OTA slot (usually ``ota_0``) is booted.

After the first OTA update, the OTA data partition is updated to specify which OTA app slot partition should be booted next.

The OTA data partition is two flash sectors (0x2000 bytes) in size, to prevent problems if there is a power failure
while it is being written. Sectors are independently erased and written with matching data, and if they disagree a
counter field is used to determine which sector was written more recently.

See also
--------

* :doc:`Partition Table documentation </partition-tables>`
* :doc:`Lower-Level SPI Flash/Partition API </api/storage/spi_flash>`

Application Example
-------------------

End-to-end example of OTA firmware update workflow: :example:`system/ota`.

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
.. doxygendefine:: OTA_SIZE_UNKNOWN

Type Definitions
^^^^^^^^^^^^^^^^

.. doxygentypedef:: esp_ota_handle_t

Functions
^^^^^^^^^

.. doxygenfunction:: esp_ota_begin
.. doxygenfunction:: esp_ota_write
.. doxygenfunction:: esp_ota_end
.. doxygenfunction:: esp_ota_get_running_partition
.. doxygenfunction:: esp_ota_set_boot_partition
.. doxygenfunction:: esp_ota_get_boot_partition
.. doxygenfunction:: esp_ota_get_next_update_partition
