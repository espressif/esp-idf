Security
========

:link_to_translation:`zh_CN:[中文]`

Memory Protection
-----------------

Starting from **ESP-IDF v6.2**, on ESP32-C5, ESP32-C6, ESP32-C61, ESP32-H2, and ESP32-P4 the second stage bootloader no longer configures or locks any RISC-V PMP entries. The complete PMP configuration is now programmed and locked by the application during early startup.

A PMP entry locked by the bootloader cannot be reconfigured until the next CPU reset. Since the bootloader on a deployed device is generally never updated, entries locked by an older bootloader could conflict with the PMP layout expected by an application built with a newer ESP-IDF version, leading to boot failures or to memory protection being silently inactive.

Applications built with ESP-IDF are not affected, as they program and lock the full PMP configuration themselves during startup, before any application code runs.

Custom (non-ESP-IDF) applications launched by the ESP-IDF second stage bootloader must not assume that any PMP entries are pre-configured or locked at handoff. Previously, the bootloader configured and locked entries covering, e.g., the ROM and the peripheral address spaces; such applications must now program their own PMP configuration.


Image verification split out of ``bootloader_support``
------------------------------------------------------

Image verification and secure-boot signature checking moved from
``bootloader_support`` into the new ``esp_image_verify`` component. ``bootloader_support`` no longer depends on ``mbedtls``.

- Components that relied on ``bootloader_support`` transitively providing ``mbedtls`` or ``app_update`` must now declare those dependencies explicitly in their ``PRIV_REQUIRES`` / ``REQUIRES``.
- :cpp:func:`bootloader_common_get_sha256_of_partition` is deprecated. Use :cpp:func:`esp_partition_get_sha256` instead.
- In builds that do not include the ``esp_image_verify`` component (no OTA or signed-image features), calling :cpp:func:`esp_partition_get_sha256` or the deprecated function above fails at link time with an undefined reference to ``esp_image_get_metadata`` / ``bootloader_sha256_flash_contents``. Add ``esp_image_verify`` (or ``app_update``, which includes it and provides the OTA APIs) to the calling component's ``PRIV_REQUIRES`` or to the project's ``COMPONENTS`` list.
- Builds enabling :ref:`CONFIG_SECURE_SIGNED_ON_UPDATE_NO_SECURE_BOOT` must include the ``esp_image_verify`` component, which provides the startup check that verifies the running app's signature.
  Apps using OTA get it automatically through ``app_update``; apps that trim the component graph (e.g. ``MINIMAL_BUILD``) must add ``esp_image_verify`` (or ``app_update``) to a ``PRIV_REQUIRES`` list or the project ``COMPONENTS``. Such builds fail with a ``#error`` instead of silently skipping the configured check.
- The Kconfig option ``CONFIG_SECURE_SIGNED_DATA_PARTITION`` was renamed to ``CONFIG_APP_UPDATE_SECURE_SIGNED_DATA_PARTITION`` (old name still accepted via ``sdkconfig.rename``).
