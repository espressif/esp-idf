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

- The deprecated ``{IDF_TARGET_NAME}/rtc.h`` header file has been removed. Please include the replacement ``esp_rtc_time.h`` instead.
- The deprecated ``RTC_CLK_SRC_INT_RC32K`` menuconfig option has been removed. Internal RC32K clock is unstable at extreme temperatures, please don't use this clock as the RTC slow clock source.

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

The following compatibility functions have been removed in ESP-IDF v6.0. These functions were maintained for backward compatibility with previous ESP-IDF versions as they were changed to either macros or separate functions in FreeRTOS. This compatibility has been removed.

- :cpp:func:`xQueueGenericReceive` - Use :cpp:func:`xQueueReceive`, :cpp:func:`xQueuePeek`, or :cpp:func:`xQueueSemaphoreTake` instead, depending on your use case.
- :cpp:func:`vTaskDelayUntil` - Use :cpp:func:`xTaskDelayUntil` instead
- :cpp:func:`ulTaskNotifyTake` - Use the macro ``ulTaskNotifyTake`` instead
- :cpp:func:`xTaskNotifyWait` - Use the macro ``xTaskNotifyWait`` instead

**Deprecated Functions**

The function :cpp:func:`pxTaskGetStackStart` has been deprecated. Use :cpp:func:`xTaskGetStackStart` instead for improved type safety.

**Memory Placement**

To reduce IRAM usage, the default placement for most FreeRTOS functions has been changed from IRAM to Flash. Consequently, the ``CONFIG_FREERTOS_PLACE_FUNCTIONS_INTO_FLASH`` option has been removed. This change saves a significant amount of IRAM but may have a slight performance impact. For performance-critical applications, the previous behavior can be restored by enabling the new :ref:`CONFIG_FREERTOS_IN_IRAM` option.

When deciding whether to enable ``CONFIG_FREERTOS_IN_IRAM``, consider conducting performance testing to measure the actual impact on your specific use case. Performance differences between Flash and IRAM configurations can vary depending on flash cache efficiency, API usage patterns, and system load.

A baseline performance test is provided in ``components/freertos/test_apps/freertos/performance/test_freertos_api_performance.c`` that measures the execution time of commonly used FreeRTOS APIs. This test can help you evaluate the performance impact of memory placement for your target hardware and application requirements.

Ring Buffer
-----------

**Memory Placement**

To reduce IRAM usage, the default placement for `esp_ringbuf` functions has been changed from IRAM to Flash. Consequently, the ``CONFIG_RINGBUF_PLACE_FUNCTIONS_INTO_FLASH`` option has been removed. This change saves a significant amount of IRAM but may have a slight performance impact. For performance-critical applications, the previous behavior can be restored by enabling the new :ref:`CONFIG_RINGBUF_IN_IRAM` option.

Log
---

**Removed Functions**

The following deprecated Log functions have been removed in ESP-IDF v6.0:

- :cpp:func:`esp_log_buffer_hex` - Use :cpp:func:`ESP_LOG_BUFFER_HEX` instead.
- :cpp:func:`esp_log_buffer_char` - Use :cpp:func:`ESP_LOG_BUFFER_CHAR` instead.

**Removed Headers**

- ``esp_log_internal.h`` - Use ``esp_log_buffer.h`` instead.

Core Dump
---------

Binary data format has been dropped. `CONFIG_ESP_COREDUMP_DATA_FORMAT_BIN` is no longer supported. ELF is now the default data format.

CRC data integrity check has been dropped. `ESP_COREDUMP_CHECKSUM_CRC32` is no longer supported. SHA256 is now the default checksum algorithm.

The function :cpp:func:`esp_core_dump_partition_and_size_get()` now returns `ESP_ERR_NOT_FOUND` for blank (erased) partitions instead of `ESP_ERR_INVALID_SIZE`.

OTA Updates
-----------

The partial download functionality in ESP HTTPS OTA has been moved under a configuration option in order to reduce the memory footprint if partial download is not used.

To use partial download features in your OTA applications, you need to enable the component-level configuration :ref:`CONFIG_ESP_HTTPS_OTA_ENABLE_PARTIAL_DOWNLOAD` in menuconfig (``Component config`` → ``ESP HTTPS OTA`` → ``Enable partial HTTP download for OTA``).

**Removed Deprecated APIs**

The following deprecated functions have been removed from the ``app_update`` component:

- :cpp:func:`esp_ota_get_app_description` – Use :cpp:func:`esp_app_get_description` instead.
- :cpp:func:`esp_ota_get_app_elf_sha256` – Use :cpp:func:`esp_app_get_elf_sha256` instead.

These functions have moved to the ``esp_app_format`` component. Update your include from ``esp_ota_ops.h`` to ``esp_app_desc.h`` and add ``esp_app_format`` to your component dependencies if needed.

Gcov
----

The gcov component has been moved to a separate repository. `esp_gcov <https://components.espressif.com/components/espressif/esp_gcov>`_  is now a managed component.

**Component Dependency**

Projects using gcov functionality must now add the esp_gcov component as a dependency in their ``idf_component.yml`` manifest file:

.. code-block:: yaml

    dependencies:
      espressif/esp_gcov: ^1

**Configuration Changes**

The gcov configuration options have moved from the Application Level Tracing menu to a dedicated ``GNU Code Coverage`` menu section.

The ``CONFIG_APPTRACE_GCOV_ENABLE`` option has been renamed to ``CONFIG_ESP_GCOV_ENABLE``.

**Header File Changes**

For the gcov functionality, include the ``esp_gcov.h`` header file instead of ``esp_app_trace.h``.

System Console (STDIO)
----------------------

``esp_vfs_cdcacm.h`` has been moved to the new component ``esp_usb_cdc_rom_console``, you will now have to add an explicit ``REQUIRES`` for ``esp_usb_cdc_rom_console`` if using any functions from this header.

