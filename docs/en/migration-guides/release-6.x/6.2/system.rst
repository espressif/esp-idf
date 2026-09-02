System
======

:link_to_translation:`zh_CN:[中文]`

OTA Updates
-----------

Application Rollback Enabled by Default
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

:ref:`app_rollback` is now enabled by default, using automatic confirmation (:ref:`CONFIG_BOOTLOADER_APP_ROLLBACK_CONFIRM_ON_STARTUP <CONFIG_BOOTLOADER_APP_ROLLBACK_CONFIRM_ON_STARTUP>`). Previously, rollback (``CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE``) was disabled by default and required the application to call :cpp:func:`esp_ota_mark_app_valid_cancel_rollback` itself.

The new default applies to new projects and to any project whose configuration is derived from ``sdkconfig.defaults``. It also applies to projects with a fixed, checked-in ``sdkconfig`` file, unless that file already recorded an explicit rollback setting:

- ``CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE=y`` is automatically migrated to :ref:`CONFIG_BOOTLOADER_APP_ROLLBACK_CONFIRM_BY_APP <CONFIG_BOOTLOADER_APP_ROLLBACK_CONFIRM_BY_APP>`, keeping the previous explicit-confirmation behavior unchanged.
- A project that left rollback at its previous default (disabled) has no deprecated option to migrate from, so it picks up the new default and gets rollback with automatic confirmation, the same as a new project.

Projects that must keep application rollback disabled need to explicitly disable :ref:`CONFIG_BOOTLOADER_APP_ROLLBACK` (*Bootloader config* > *Application Rollback* > *Enable application rollback*).

Impact on Devices with an Existing Bootloader
"""""""""""""""""""""""""""""""""""""""""""""

Application rollback is implemented jointly by the second stage bootloader and the application. Enabling it in the application is not sufficient on devices where the bootloader was already deployed without rollback support.

On such devices, updating to an application built with rollback enabled does not restore the safety net: the bootloader never transitions the OTA state from ``ESP_OTA_IMG_NEW`` to ``ESP_OTA_IMG_PENDING_VERIFY``, so a crash on the first boot after the update is not detected, and the device does not roll back to the previous working application. The application detects this situation at startup and marks the application valid to keep the OTA state consistent, but this does not provide rollback protection by itself.

To get the safety net on devices already in the field, the bootloader itself must be rebuilt with rollback support and reflashed.

Code Size Impact
"""""""""""""""""

Enabling application rollback with automatic confirmation by default adds a small amount of code compared to the previous default (rollback disabled). Measured on :example:`system/ota/native_ota_example` for ESP32, the bootloader grows by approximately 80 bytes, and the application by approximately 350 bytes. The exact numbers depend on the target and application configuration.
