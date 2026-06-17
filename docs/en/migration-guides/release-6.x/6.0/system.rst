System
======

:link_to_translation:`zh_CN:[中文]`

Default LibC changed from Newlib to PicolibC
--------------------------------------------

Since ESP-IDF v6.0, the default LibC used in builds has switched to the PicolibC implementation.

.. note::

    PicolibC is a Newlib fork with a rewritten stdio implementation whose goal is to consume less memory.

In most cases, no application behavior changes are expected, except for reduced binary size and less stack consumption on I/O operations.

.. warning::

    **Breaking change:** It is not possible to redefine stdin, stdout, and stderr for specific tasks as was possible with Newlib. These streams are global and shared between all tasks. This is POSIX-standardized behavior.

:ref:`CONFIG_LIBC_PICOLIBC_NEWLIB_COMPATIBILITY`, which is enabled by default, provides limited compatibility with Newlib by providing thread-local copies of ``global stdin``, ``stdout``, ``stderr``, and the ``getreent()`` implementation. If a library built with Newlib headers operates with "internal" fields of "struct reent", there may be task stack corruption. Note that manipulating ``struct reent`` fields is expected only by the Newlib library itself.

If you are not linking against external libraries built against Newlib headers, you may disable :ref:`CONFIG_LIBC_PICOLIBC_NEWLIB_COMPATIBILITY` to save a small amount of memory.

Newlib is still maintained in ESP-IDF toolchains. To switch to using it, select Newlib in menuconfig via the option LIBC_NEWLIB in :ref:`CONFIG_LIBC`.

Comparison of Newlib vs Picolibc
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

There are a small example that shows the motivation of switching to Picolibc:

.. code-block:: c

    FILE *f = fopen("/dev/console", "w");
    for (int i = 0; i < 10; i++)
    {
        fprintf(f, "hello world %s\n", "🤖");
        fprintf(f, "%.1000f\n", 3.141592653589793);
        fprintf(f, "%1000d\n", 42);
    }

The test code was compiled with both Newlib and Picolibc, and the results were compared on ESP32-C3:

.. list-table:: Comparison of Newlib vs Picolibc
   :header-rows: 1
   :widths: 30 20 20 20

   * - Metric
     - Newlib
     - Picolibc
     - Difference
   * - Binary size (bytes)
     - 280,128
     - 224,656
     - 19.80%
   * - Stack usage (bytes)
     - 1,748
     - 802
     - 54.12%
   * - Heap usage (bytes)
     - 1,652
     - 376
     - 77.24%
   * - Performance (CPU cycles)
     - 278,232,026
     - 279,823,800
     - 0.59%

.. note::

    Even when :ref:`CONFIG_LIBC_NEWLIB_NANO_FORMAT` is enabled, which disables float formatting, applications with Picolibc are still smaller by 6% (224,592 vs 239,888 bytes).

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

.. _gpio_wakeup_api_changes:

GPIO Wakeup API Changes
^^^^^^^^^^^^^^^^^^^^^^^^

The following APIs and types have been removed and replaced with new ones that support both Deep Sleep and Light Sleep (when peripheral power domain is powered down):

**Removed APIs:**

- :func:`esp_deep_sleep_enable_gpio_wakeup` - Use :func:`esp_sleep_enable_gpio_wakeup_on_hp_periph_powerdown` instead
- :func:`gpio_deep_sleep_wakeup_enable` - Use :func:`gpio_wakeup_enable_on_hp_periph_powerdown_sleep` instead
- :func:`gpio_deep_sleep_wakeup_disable` - Use :func:`gpio_wakeup_disable_on_hp_periph_powerdown_sleep` instead

**Removed Types:**

- :cpp:type:`esp_deepsleep_gpio_wake_up_mode_t` - Use :cpp:type:`esp_sleep_gpio_wake_up_mode_t` instead

**Removed Macros:**

- ``GPIO_IS_DEEP_SLEEP_WAKEUP_VALID_GPIO()`` - Use ``GPIO_IS_HP_PERIPH_PD_WAKEUP_VALID_IO()`` instead

**Migration Example:**

Old code:

.. code-block:: c

    #include "esp_sleep.h"
    #include "driver/gpio.h"

    // Enable GPIO wakeup for deep sleep
    esp_deep_sleep_enable_gpio_wakeup(BIT(GPIO_NUM_0), ESP_GPIO_WAKEUP_GPIO_LOW);

    // Or using GPIO driver API
    gpio_deep_sleep_wakeup_enable(GPIO_NUM_0, GPIO_INTR_LOW_LEVEL);

    // Check if GPIO is valid for deep sleep wakeup
    if (GPIO_IS_DEEP_SLEEP_WAKEUP_VALID_GPIO(GPIO_NUM_0)) {
        // ...
    }

New code:

.. code-block:: c

    #include "esp_sleep.h"
    #include "driver/gpio.h"

    // Enable GPIO wakeup for deep sleep or light sleep (when peripheral power domain is powered down)
    esp_sleep_enable_gpio_wakeup_on_hp_periph_powerdown(BIT(GPIO_NUM_0), ESP_GPIO_WAKEUP_GPIO_LOW);

    // Or using GPIO driver API
    gpio_wakeup_enable_on_hp_periph_powerdown_sleep(GPIO_NUM_0, GPIO_INTR_LOW_LEVEL);

    // Check if GPIO is valid for wakeup on peripheral powerdown sleep
    if (GPIO_IS_HP_PERIPH_PD_WAKEUP_VALID_IO(GPIO_NUM_0)) {
        // ...
    }

**Note:** The new APIs work for both Deep Sleep and Light Sleep modes when the peripheral power domain is powered down (``PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP`` enabled in menuconfig).

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

The deprecated ``intr_types.h`` header file has been removed. Please include the replacement ``esp_intr_types.h`` instead.

The deprecated ``esp_private/interrupt_deprecated.h`` header file, previously accessible through ``riscv/interrupt.h`` header, has been removed. The deprecated functions are no longer available; please use the non-deprecated versions instead.

The ``esp_fault.h`` header file has been moved from the ``esp_hw_support`` component to the ``esp_common`` component. If your application encounters build errors after this change, add ``esp_common`` to your component's ``REQUIRES`` or ``PRIV_REQUIRES`` list in ``CMakeLists.txt``.

ROM Headers
-----------

The deprecated ``STATUS`` type has been removed from ``ets_sys.h`` ROM header files. Please use ``ETS_STATUS`` instead.

App Trace
----------

Configuration Changes
^^^^^^^^^^^^^^^^^^^^^

The application tracing configuration menu has been moved. Previously located at ``Component config`` > ``Application Level Tracing``, it is now under ``Component config`` > ``ESP Trace Configuration``.

Previously, application tracing was automatically enabled when a destination was configured. Now you must explicitly enable application tracing by selecting the trace transport before configuring any destination.

To enable application tracing, go to ``Component config`` > ``ESP Trace Configuration`` > ``Trace transport`` and select ``ESP-IDF apptrace`` in menuconfig. After that, configuration can be done at ``Component config`` > ``ESP Trace Configuration`` > ``Application Level Tracing``.

If apptrace will be used without a library (for example when SEGGER SystemView is disabled) in standalone mode, the following configurations need to be set in your sdkconfig file:

.. code-block:: none

    CONFIG_ESP_TRACE_ENABLE=y
    CONFIG_ESP_TRACE_LIB_NONE=y
    CONFIG_ESP_TRACE_TRANSPORT_APPTRACE=y

These can also be configured through menuconfig as described above.

Removed extra data buffering option. ``CONFIG_APPTRACE_PENDING_DATA_SIZE_MAX`` is no longer supported.

Removed deprecated ``ESP_APPTRACE_DEST_TRAX`` enum value. Use ``ESP_APPTRACE_DEST_JTAG`` instead.

Component Dependency Changes
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Applications now require the ``esp_trace`` component instead of ``app_trace``. Update your component's ``CMakeLists.txt`` file to reflect this change.

The ``app_trace`` component is now a sub-component of ``esp_trace`` and will be included automatically when needed.

Initialization Flow Changes
^^^^^^^^^^^^^^^^^^^^^^^^^^^

For runtime configuration override, a new callback system is available. See the :doc:`Application Tracing documentation <../../../api-guides/app_trace>` for details on ``esp_apptrace_get_user_params()`` and ``esp_trace_get_user_params()``.

API Changes
^^^^^^^^^^^

The destination parameter has been removed from all apptrace APIs.

Default destination is now configured in menuconfig under ``Component config`` > ``ESP Trace Configuration`` > ``Application Level Tracing`` and can be altered at runtime by providing callback with custom tracing configuration.

The UART destination configuration has been simplified:

- Removed: Individual UART selection via ``CONFIG_APPTRACE_DEST_UARTx=y``
- Added: Single UART port selection via ``CONFIG_APPTRACE_DEST_UART_NUM``

To migrate, update your sdkconfig:

Old configuration:

.. code-block:: none

    CONFIG_APPTRACE_DEST_UART0=y
    # or
    CONFIG_APPTRACE_DEST_UART1=y

New configuration:

.. code-block:: none

    CONFIG_APPTRACE_DEST_UART=y
    CONFIG_APPTRACE_DEST_UART_NUM=0  # or 1, 2 depending on target

SEGGER SystemView
^^^^^^^^^^^^^^^^^

The SystemView component has been moved to a separate repository. `esp_sysview <https://components.espressif.com/components/espressif/esp_sysview>`_  is now a managed component.

Configuration
^^^^^^^^^^^^^^

The SystemView configuration has been moved to a new location in the menuconfig. It becomes visible only when the project adds the ``esp_sysview`` component dependency in ``idf_component.yml`` and the external library is selected in menuconfig.

Add the dependency in your component manifest:

.. code-block:: yaml

    dependencies:
      espressif/esp_sysview: ^1

Then, in menuconfig, select: ``Component config`` > ``ESP Trace Configuration`` > ``Trace library`` > ``External library from component registry``.

After that, the SystemView configuration can be shown by selecting ``Component config`` > ``SEGGER SystemView Configuration``.

The SystemView no longer has its own separate destination configuration. It shares the configuration with the application tracing transport (JTAG or UART).

FreeRTOS
--------

Removed Functions
^^^^^^^^^^^^^^^^^

The following deprecated FreeRTOS functions have been removed in ESP-IDF v6.0:

- :cpp:func:`xTaskGetAffinity` – Use :cpp:func:`xTaskGetCoreID` instead.
- :cpp:func:`xTaskGetIdleTaskHandleForCPU` – Use :cpp:func:`xTaskGetIdleTaskHandleForCore` instead.
- :cpp:func:`xTaskGetCurrentTaskHandleForCPU` – Use :cpp:func:`xTaskGetCurrentTaskHandleForCore` instead.

The following compatibility functions have been removed in ESP-IDF v6.0. These functions were maintained for backward compatibility with previous ESP-IDF versions as they were changed to either macros or separate functions in FreeRTOS. This compatibility has been removed.

- :cpp:func:`xQueueGenericReceive` - Use :cpp:func:`xQueueReceive`, :cpp:func:`xQueuePeek`, or :cpp:func:`xQueueSemaphoreTake` instead, depending on your use case.
- :cpp:func:`vTaskDelayUntil` - Use :cpp:func:`xTaskDelayUntil` instead.
- :cpp:func:`ulTaskNotifyTake` - Use the macro ``ulTaskNotifyTake`` instead.
- :cpp:func:`xTaskNotifyWait` - Use the macro ``xTaskNotifyWait`` instead.

Deprecated Functions
^^^^^^^^^^^^^^^^^^^^

The function :cpp:func:`pxTaskGetStackStart` has been deprecated. Use :cpp:func:`xTaskGetStackStart` instead for improved type safety.

API Added
^^^^^^^^^

Task snapshot APIs have been made public to support external frameworks like ESP Insights. These APIs are now provided through ``freertos/freertos_debug.h`` instead of the deprecated ``freertos/task_snapshot.h``.

For safe use while the scheduler is running, use ``vTaskSuspendAll()`` before calling snapshot functions, and ``xTaskResumeAll()`` afterward.

Memory Placement
^^^^^^^^^^^^^^^^

- To reduce IRAM usage, the default placement for most FreeRTOS functions has been changed from IRAM to flash. Consequently, the ``CONFIG_FREERTOS_PLACE_FUNCTIONS_INTO_FLASH`` option has been removed. This change saves a significant amount of IRAM but may have a slight performance impact. For performance-critical applications, you can restore the previous behavior by enabling the new :ref:`CONFIG_FREERTOS_IN_IRAM` option.
- Before enabling ``CONFIG_FREERTOS_IN_IRAM``, it is recommended to run performance tests to measure the actual impact on your specific use case. The performance difference between flash and IRAM configurations depends on factors such as flash cache efficiency, API usage patterns, and system load.
- A baseline performance test is provided in ``components/freertos/test_apps/freertos/performance/test_freertos_api_performance.c``. This test measures the execution time of commonly used FreeRTOS APIs and can help you evaluate the effect of memory placement for your target hardware and application requirements.
- Task snapshot functions are automatically placed in IRAM when ``CONFIG_ESP_PANIC_HANDLER_IRAM`` is enabled, ensuring they remain accessible during panic handling.
- ``vTaskGetSnapshot`` is kept in IRAM unless ``CONFIG_FREERTOS_PLACE_ISR_FUNCTIONS_INTO_FLASH`` is enabled, as it is used by the Task Watchdog interrupt handler.

Removed Configuration Options
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The following hidden (and always true) configuration options have been removed:

- ``CONFIG_FREERTOS_ENABLE_TASK_SNAPSHOT``
- ``CONFIG_FREERTOS_PLACE_SNAPSHOT_FUNS_INTO_FLASH``

Ring Buffer
-----------

Memory Placement
^^^^^^^^^^^^^^^^

To reduce IRAM usage, the default placement for `esp_ringbuf` functions has been changed from IRAM to Flash. Consequently, the ``CONFIG_RINGBUF_PLACE_FUNCTIONS_INTO_FLASH`` option has been removed. This change saves a significant amount of IRAM but may have a slight performance impact. For performance-critical applications, the previous behavior can be restored by enabling the new :ref:`CONFIG_RINGBUF_IN_IRAM` option.

Log
---

Removed Functions
^^^^^^^^^^^^^^^^^

The following deprecated Log functions have been removed in ESP-IDF v6.0:

- :cpp:func:`esp_log_buffer_hex` - Use :cpp:func:`ESP_LOG_BUFFER_HEX` instead.
- :cpp:func:`esp_log_buffer_char` - Use :cpp:func:`ESP_LOG_BUFFER_CHAR` instead.

Removed Headers
^^^^^^^^^^^^^^^

- ``esp_log_internal.h`` - Use ``esp_log_buffer.h`` instead.

ESP-Event
---------

Unnecessary FreeRTOS headers have been removed from ``esp_event.h``. Code that previously depended on these implicit includes must now include the headers explicitly: ``#include "freertos/queue.h"`` and ``#include "freertos/semphr.h"`` to your files.

Core Dump
---------

Binary data format has been dropped. `CONFIG_ESP_COREDUMP_DATA_FORMAT_BIN` is no longer supported. ELF is now the default data format.

CRC data integrity check has been dropped. `ESP_COREDUMP_CHECKSUM_CRC32` is no longer supported. SHA256 is now the default checksum algorithm.

The function :cpp:func:`esp_core_dump_partition_and_size_get()` now returns `ESP_ERR_NOT_FOUND` for blank (erased) partitions instead of `ESP_ERR_INVALID_SIZE`.

OTA Updates
-----------

The partial download functionality in ESP HTTPS OTA has been moved under a configuration option in order to reduce the memory footprint if partial download is not used.

To use partial download features in your OTA applications, you need to enable the component-level configuration :ref:`CONFIG_ESP_HTTPS_OTA_ENABLE_PARTIAL_DOWNLOAD` in menuconfig (``Component config`` > ``ESP HTTPS OTA`` > ``Enable partial HTTP download for OTA``).

Removed Deprecated APIs
^^^^^^^^^^^^^^^^^^^^^^^

The following deprecated functions have been removed from the ``app_update`` component:

- :cpp:func:`esp_ota_get_app_description` – Use :cpp:func:`esp_app_get_description` instead.
- :cpp:func:`esp_ota_get_app_elf_sha256` – Use :cpp:func:`esp_app_get_elf_sha256` instead.

These functions have moved to the ``esp_app_format`` component. Update your include from ``esp_ota_ops.h`` to ``esp_app_desc.h`` and add ``esp_app_format`` to your component dependencies if needed.

Gcov
----

The gcov component has been moved to a separate repository. `esp_gcov <https://components.espressif.com/components/espressif/esp_gcov>`_  is now a managed component.

Component Dependency
^^^^^^^^^^^^^^^^^^^^

Projects using gcov functionality must now add the esp_gcov component as a dependency in their ``idf_component.yml`` manifest file:

.. code-block:: yaml

    dependencies:
      espressif/esp_gcov: ^1

Configuration Changes
^^^^^^^^^^^^^^^^^^^^^

The gcov configuration options have moved from the Application Level Tracing menu to a dedicated ``GNU Code Coverage`` menu section.

The ``CONFIG_APPTRACE_GCOV_ENABLE`` option has been renamed to ``CONFIG_ESP_GCOV_ENABLE``.

Header File Changes
^^^^^^^^^^^^^^^^^^^

For the gcov functionality, include the ``esp_gcov.h`` header file instead of ``esp_app_trace.h``.

System Console (STDIO)
----------------------

``esp_vfs_cdcacm.h`` has been moved to the new component ``esp_usb_cdc_romconsole``, you will now have to add an explicit ``REQUIRES`` for ``esp_usb_cdc_rom_console`` if using any functions from this header.

LibC
------

:ref:`CONFIG_COMPILER_ASSERT_NDEBUG_EVALUATE` default value is changed to `n`. This means asserts will no longer evaluate the expression inside the assert when ``NDEBUG`` is set. This reverts the default behavior to be in line with the C standard.

ULP
---

The LP-Core will now wake up the main CPU when it encounters an exception during deep sleep. This feature is enabled by default but can be disabled via the :ref:`CONFIG_ULP_TRAP_WAKEUP` Kconfig option is this behavior is not desired.

Heap
----

In previous versions of ESP-IDF, the capability ``MALLOC_CAP_EXEC`` would be available regardless of the memory protection configuration state. This implied that a call to e.g., :cpp:func:`heap_caps_malloc` with ``MALLOC_CAP_EXEC`` would return NULL when ``CONFIG_ESP_SYSTEM_MEMPROT_FEATURE`` or ``CONFIG_ESP_SYSTEM_PMP_IDRAM_SPLIT`` are enabled.

Since ESP-IDF v6.0, the definition of ``MALLOC_CAP_EXEC`` is conditional, meaning that if CONFIG_ESP_SYSTEM_MEMPROT is enabled, ``MALLOC_CAP_EXEC`` will not be defined. Therefore, using it will generate a compile time error.

``esp_common``
--------------

- ``EXT_RAM_ATTR`` was deprecated since v5.0, Now it has been removed. Please use the macro ``EXT_RAM_BSS_ATTR`` to put ``.bss`` on PSRAM.
- RTC related memory attributes (``RTC_x_ATTR``) have been removed on those chips without RTC memory.
