System
======

:link_to_translation:`zh_CN:[中文]`

Xtensa
------

The Xtensa special register header files have been updated to use a new naming convention. The old ``specreg.h`` header files are now deprecated and will be removed in a future release.

The register names have been updated to use the ``XT_REG_`` prefix. Please use the new ``xt_specreg.h`` file instead.

Power Management
----------------

In previous versions of ESP-IDF, the API :cpp:func:`esp_sleep_get_wakeup_cause` was used to retrieve the wakeup reason after the chip exited sleep. However, this function only returns one wakeup source, even if multiple sources were triggered simultaneously, which may cause users to miss other active wakeup events.

Since ESP-IDF v6.0, a new API :cpp:func:`esp_sleep_get_wakeup_causes` has been introduced. This function returns a bitmap representing all wakeup sources that caused the chip to exit sleep. Each bit corresponds to a value in the :cpp:type:`esp_sleep_wakeup_cause_t` enum (e.g., ESP_SLEEP_WAKEUP_TIMER, ESP_SLEEP_WAKEUP_EXT1, etc.). Users can check each wakeup source using bitwise operations.

The original :cpp:func:`esp_sleep_get_wakeup_cause()` function has been marked as deprecated, and it is recommended to migrate to the new interface. This legacy API may be removed in future versions. Migration can be performed as shown in the example below:

Old Version:

.. code-block:: c

    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    if (cause == ESP_SLEEP_WAKEUP_EXT1) {
        handle_ext1_wakeup();
    }

Update to:

.. code-block:: c

    uint32_t causes = esp_sleep_get_wakeup_causes();
    if (causes & BIT(ESP_SLEEP_WAKEUP_EXT1)) {
        handle_ext1_wakeup();
    }
    if (causes & BIT(ESP_SLEEP_WAKEUP_TIMER)) {
        handle_timer_wakeup();
    }

Bootloader
----------

Removed option for compiling bootloader with no optimization level (-O0, `CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION_NONE`). On most targets it was no longer possible to compile the bootloader with -O0, as IRAM sections would overflow. For debugging purposes, it is recommended to use the -Og (:ref:`CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION_DEBUG<CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION_DEBUG>`) optimization level instead. This provides a good balance between optimization and debuggability.

Time
----

The deprecated ``{IDF_TARGET_NAME}/rtc.h`` header file has been removed. Please include the replacement ``esp_rtc_time.h`` instead.

HW-Support
----------

The deprecated ``soc_memory_types.h`` header file has been removed. Please include the replacement ``esp_memory_utils.h`` instead.

App Trace
----------
Removed extra data buffering option. `CONFIG_APPTRACE_PENDING_DATA_SIZE_MAX` is no longer supported.

Removed deprecated `ESP_APPTRACE_DEST_TRAX` enum value. Use `ESP_APPTRACE_DEST_JTAG` instead.

The :cpp:func:`esp_apptrace_down_buffer_config` function now requires a destination parameter and returns an error code for proper error handling.

Old Version:

.. code-block:: c

    esp_apptrace_down_buffer_config(down_buf, sizeof(down_buf));

Update to:

.. code-block:: c

    esp_err_t res = esp_apptrace_down_buffer_config(ESP_APPTRACE_DEST_JTAG, down_buf, sizeof(down_buf));
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "Failed to config down buffer!");
        return res;
    }

FreeRTOS
--------

**Removed Functions**

The following deprecated FreeRTOS functions have been removed in ESP-IDF v6.0:

- :cpp:func:`xTaskGetAffinity` – Use :cpp:func:`xTaskGetCoreID` instead.
- :cpp:func:`xTaskGetIdleTaskHandleForCPU` – Use :cpp:func:`xTaskGetIdleTaskHandleForCore` instead.
- :cpp:func:`xTaskGetCurrentTaskHandleForCPU` – Use :cpp:func:`xTaskGetCurrentTaskHandleForCore` instead.

**Deprecated Functions**

The function :cpp:func:`pxTaskGetStackStart` has been deprecated. Use :cpp:func:`xTaskGetStackStart` instead for improved type safety.
