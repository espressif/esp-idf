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

.. _app_rollback:

App rollback
------------

The main purpose of the application rollback is to keep the device working after the update. This feature allows you to roll back to the previous working application in case a new application has critical errors. When the rollback process is enabled and an OTA update provides a new version of the app, one of three things can happen:

* The application works fine, :cpp:func:`esp_ota_mark_app_valid_cancel_rollback` marks the running application with the state ``ESP_OTA_IMG_VALID``. There are no restrictions on booting this application.
* The application has critical errors and further work is not possible, a rollback to the previous application is required, :cpp:func:`esp_ota_mark_app_invalid_rollback_and_reboot` marks the running application with the state ``ESP_OTA_IMG_INVALID`` and reset. This application will not be selected by the bootloader for boot and will boot the previously working application.
* If the :ref:`CONFIG_APP_ROLLBACK_ENABLE` option is set, and occur a reset without calling either function then happend and is rolled back.

Note: The state is not written to the binary image of the application it is written to the ``otadata`` partition. The partition contains a ``ota_seq`` counter  which is a pointer to the slot (ota_0, ota_1, ...) from which the application will be selected for boot.

App OTA State
^^^^^^^^^^^^^

States control the process of selecting a boot app:

+-----------------------------+--------------------------------------------------------+
|            States           | Restriction of selecting a boot app in bootloader      |
+=============================+========================================================+
| ESP_OTA_IMG_VALID           | None restriction. Will be selected.                    |
+-----------------------------+--------------------------------------------------------+
| ESP_OTA_IMG_UNDEFINED       | None restriction. Will be selected.                    |
+-----------------------------+--------------------------------------------------------+
| ESP_OTA_IMG_INVALID         | Will not be selected.                                  |
+-----------------------------+--------------------------------------------------------+
| ESP_OTA_IMG_ABORTED         | Will not be selected.                                  |
+-----------------------------+--------------------------------------------------------+
| ESP_OTA_IMG_NEW             | If :ref:`CONFIG_APP_ROLLBACK_ENABLE` option is set     |
|                             | it will be selected only once. In bootloader the state |
|                             | immediately changes to ``ESP_OTA_IMG_PENDING_VERIFY``. |
+-----------------------------+--------------------------------------------------------+
| ESP_OTA_IMG_PENDING_VERIFY  | If :ref:`CONFIG_APP_ROLLBACK_ENABLE` option is set     |                                                  
|                             | it will not be selected and the state will change to   |
|                             | ``ESP_OTA_IMG_ABORTED``.                               |
+-----------------------------+--------------------------------------------------------+

If :ref:`CONFIG_APP_ROLLBACK_ENABLE` option is not enabled (by default), then the use of the following functions :cpp:func:`esp_ota_mark_app_valid_cancel_rollback` and :cpp:func:`esp_ota_mark_app_invalid_rollback_and_reboot` are optional, and ``ESP_OTA_IMG_NEW`` and ``ESP_OTA_IMG_PENDING_VERIFY`` states are not used.

An option in Kconfig :ref:`CONFIG_APP_ROLLBACK_ENABLE` allows you to track the first boot of a new application. In this case, the application must confirm its operability by calling :cpp:func:`esp_ota_mark_app_valid_cancel_rollback` function, otherwise the application will be rolled back upon reboot. It allows you to control the operability of the application during the boot phase. Thus, a new application has only one attempt to boot successfully.

Rollback Process
^^^^^^^^^^^^^^^^

The description of the rollback process when :ref:`CONFIG_APP_ROLLBACK_ENABLE` option is enabled:

* The new application successfully downloaded and :cpp:func:`esp_ota_set_boot_partition` function makes this partition bootable and sets the state ``ESP_OTA_IMG_NEW``. This state means that the application is new and should be monitored for its first boot.
* Reboot :cpp:func:`esp_restart`.
* The bootloader checks for the ``ESP_OTA_IMG_PENDING_VERIFY`` state if it is set, then it will be written to ``ESP_OTA_IMG_ABORTED``.
* The bootloader selects a new application to boot so that the state is not set as ``ESP_OTA_IMG_INVALID`` or ``ESP_OTA_IMG_ABORTED``.
* The bootloader checks the selected application for ``ESP_OTA_IMG_NEW`` state if it is set, then it will be written to ``ESP_OTA_IMG_PENDING_VERIFY``. This state means that the application requires confirmation of its operability, if this does not happen and a reboot occurs, this state will be overwritten to ``ESP_OTA_IMG_ABORTED`` (see above) and this application will no longer be able to start, i.e. there will be a rollback to the previous work application.
* A new application has started and should make a self-test.
* If the self-test has completed successfully, then you must call the function :cpp:func:`esp_ota_mark_app_valid_cancel_rollback` because the application is awaiting confirmation of operability (``ESP_OTA_IMG_PENDING_VERIFY`` state).
* If the self-test fails then call :cpp:func:`esp_ota_mark_app_invalid_rollback_and_reboot` function to roll back to the previous working application, while the invalid application is set ``ESP_OTA_IMG_INVALID`` state.
* If the application has not been confirmed, the state remains ``ESP_OTA_IMG_PENDING_VERIFY``, and the next boot it will be changed to ``ESP_OTA_IMG_ABORTED``. That will prevent re-boot of this application. There will be a rollback to the previous working application.

Unexpected Reset
^^^^^^^^^^^^^^^^

If a power loss or an unexpected crash occurs at the time of the first boot of a new application, it will roll back the application.

Recommendation: Perform the self-test procedure as quickly as possible, to prevent rollback due to power loss.

Only ``OTA`` partitions can be rolled back. Factory partition is not rolled back.

Booting invalid/aborted apps
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Booting an application which was previously set to ``ESP_OTA_IMG_INVALID`` or ``ESP_OTA_IMG_ABORTED`` is possible:

* Get the last invalid application partition :cpp:func:`esp_ota_get_last_invalid_partition`.
* Pass the received partition to :cpp:func:`esp_ota_set_boot_partition`, this will update the ``otadata``.
* Restart :cpp:func:`esp_restart`. The bootloader will boot the specified application.

To determine if self-tests should be run during startup of an application, call the :cpp:func:`esp_ota_get_state_partition` function. If result is ``ESP_OTA_IMG_PENDING_VERIFY`` then self-testing and subsequent confirmation of operability is required.

Where the states are set
^^^^^^^^^^^^^^^^^^^^^^^^

A brief description of where the states are set:

* ``ESP_OTA_IMG_VALID`` state is set by :cpp:func:`esp_ota_mark_app_valid_cancel_rollback` function.
* ``ESP_OTA_IMG_UNDEFINED`` state is set by :cpp:func:`esp_ota_set_boot_partition` function if :ref:`CONFIG_APP_ROLLBACK_ENABLE` option is not enabled.
* ``ESP_OTA_IMG_NEW`` state is set by :cpp:func:`esp_ota_set_boot_partition` function if :ref:`CONFIG_APP_ROLLBACK_ENABLE` option is enabled.
* ``ESP_OTA_IMG_INVALID`` state is set by  :cpp:func:`esp_ota_mark_app_invalid_rollback_and_reboot` function.
* ``ESP_OTA_IMG_ABORTED`` state is set if there was no confirmation of the application operability and occurs reboots (if :ref:`CONFIG_APP_ROLLBACK_ENABLE` option is enabled).
* ``ESP_OTA_IMG_PENDING_VERIFY`` state is set in a bootloader if :ref:`CONFIG_APP_ROLLBACK_ENABLE` option is enabled and selected app has ``ESP_OTA_IMG_NEW`` state.

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



