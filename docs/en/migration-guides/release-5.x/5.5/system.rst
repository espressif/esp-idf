System
======

:link_to_translation:`en:[English]`

Time
----

* ``{IDF_TARGET_NAME}/rtc.h`` is deprecated, include the replacement ``esp_rtc_time.h`` instead.

Log
---

**Log V2** is introduced in this ESP-IDF version as an enhanced and optional logging implementation. It is fully compatible with **Log V1**, allowing projects to continue using **Log V1** without changes. Developers can enable **Log V2** via the Kconfig option :ref:`CONFIG_LOG_VERSION`. In future ESP-IDF versions, **Log V2** may become the default.

**Key Points**

- Centralized log handling, dynamic formatting, and greater flexibility.
- Slightly higher stack and IRAM usage, with a reduced binary size (refer to measurements in the `Logging Library <../../../system/log.rst>`_ document).
- **Log V2**-specific features (e.g., dynamic formatting, detecting the execution context) are not backward-compatible with **Log V1**.

If you use the ``esp_log_write`` (or ``esp_log_writev``) function, replace it with ``esp_log`` (or ``esp_log_va``) to reduce one nested call.

The log handler (``esp_log``) supports internal formatting, so there is no need to construct formatting manually. The code below can be simplified by using ``ESP_LOG_LEVEL_LOCAL``.

.. code-block:: c

    #if ESP_LOG_VERSION == 2
        ESP_LOG_LEVEL_LOCAL(ESP_LOG_ERROR, OT_PLAT_LOG_TAG, format, args);
    #else
        if (LOG_LOCAL_LEVEL >= ESP_LOG_ERROR) {
            esp_log(ESP_LOG_CONFIG_INIT(ESP_LOG_ERROR), OT_PLAT_LOG_TAG, LOG_COLOR_E "E(%lu) %s:", esp_log_timestamp(), OT_PLAT_LOG_TAG);
            esp_log_va(ESP_LOG_CONFIG_INIT(ESP_LOG_ERROR), OT_PLAT_LOG_TAG, format, args);
            esp_log(ESP_LOG_CONFIG_INIT(ESP_LOG_ERROR), OT_PLAT_LOG_TAG, LOG_RESET_COLOR "\n");
        }
    #endif

Pre-Encrypted OTA
-----------------

The ``pre_encrypted_ota`` example has been moved to the `idf-extra-components <https://github.com/espressif/idf-extra-components/tree/master/esp_encrypted_img/examples/pre_encrypted_ota>`__ repository.
