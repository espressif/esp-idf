Over The Air Updates (OTA)
==========================

OTA Process Overview
--------------------

The OTA update mechanism allows a device to update itself based on data received while the normal firmware is running
(for example, over WiFi or Bluetooth.)

OTA requires configuring the :doc:`Partition Table <../../api-guides/partition-tables>` of the device with at least two "OTA app slot"
partitions (ie `ota_0` and `ota_1`) and an "OTA Data Partition".

The OTA operation functions write a new app firmware image to whichever OTA app slot is not currently being used for
booting. Once the image is verified, the OTA Data partition is updated to specify that this image should be used for the
next boot.

.. _ota_data_partition:

OTA Data Partition
------------------

An OTA data partition (type ``data``, subtype ``ota``) must be included in the :doc:`Partition Table <../../api-guides/partition-tables>`
of any project which uses the OTA functions.

For factory boot settings, the OTA data partition should contain no data (all bytes erased to 0xFF). In this case the
esp-idf software bootloader will boot the factory app if it is present in the the partition table. If no factory app is
included in the partition table, the first available OTA slot (usually ``ota_0``) is booted.

After the first OTA update, the OTA data partition is updated to specify which OTA app slot partition should be booted next.

The OTA data partition is two flash sectors (0x2000 bytes) in size, to prevent problems if there is a power failure
while it is being written. Sectors are independently erased and written with matching data, and if they disagree a
counter field is used to determine which sector was written more recently.

.. _secure-ota-updates:

Secure OTA Updates Without Secure boot
--------------------------------------

The verification of signed OTA updates can be performed even without enabling hardware secure boot. For doing so, refer :ref:`signed-app-verify`

See also
--------

* :doc:`Partition Table documentation <../../api-guides/partition-tables>`
* :doc:`Lower-Level SPI Flash/Partition API <../storage/spi_flash>`
* :doc:`ESP HTTPS OTA <esp_https_ota>`

Application Example
-------------------

End-to-end example of OTA firmware update workflow: :example:`system/ota`.

API Reference
-------------

.. include:: /_build/inc/esp_ota_ops.inc



