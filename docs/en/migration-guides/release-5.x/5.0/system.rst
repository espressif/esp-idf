System
======

:link_to_translation:`zh_CN:[中文]`

Inter-Processor Call
-----------------------

IPC (Inter-Processor Call) feature is no longer a stand-alone component and has been integrated into the ``esp_system`` component.

Thus, any project presenting a ``CMakeLists.txt`` file with the parameters ``PRIV_REQUIRES esp_ipc`` or ``REQUIRES esp_ipc`` should be modified to simply remove these options as the ``esp_system`` component is included by default.

ESP Clock
---------

The ESP Clock API (functions/types/macros prefixed with ``esp_clk``) has been made into a private API. Thus, the previous include paths ``#include "{IDF_TARGET_NAME}/clk.h"`` and ``#include "esp_clk.h"`` have been removed. If users still require usage of the ESP Clock API (though this is not recommended), it can be included via  ``#include "esp_private/esp_clk.h"``.

.. note::

    Private APIs are not stable and no are longer subject to the ESP-IDF versioning scheme's breaking change rules. Thus, it is not recommended for users to continue calling private APIs in their applications.

Cache Error Interrupt
---------------------

The Cache Error Interrupt API (functions/types/macros prefixed with ``esp_cache_err``) has been made into a private API. Thus, the previous include path ``#include "{IDF_TARGET_NAME}/cache_err_int.h"`` has been removed. If users still require usage of the Cache Error Interrupt API (though this is not recommended), it can be included via  ``#include "esp_private/cache_err_int.h"``.

``bootloader_support``
----------------------

* The function ``bootloader_common_get_reset_reason()`` has been removed. Please use the function ``esp_rom_get_reset_reason()`` in the ROM component.
* The functions ``esp_secure_boot_verify_sbv2_signature_block()`` and ``esp_secure_boot_verify_rsa_signature_block()`` have been removed without replacement. We do not expect users to use these directly. If they are indeed still necessary, please open a feature request on `GitHub <https://github.com/espressif/esp-idf/issues/new/choose>`_ explaining why these functions are necessary to you.

Brownout
--------

The Brownout API (functions/types/macros prefixed with ``esp_brownout``) has been made into a private API. Thus, the previous include path ``#include "brownout.h"`` has been removed. If users still require usage of the Brownout API (though this is not recommended), it can be included via  ``#include "esp_private/brownout.h"``.

Trax
----

The Trax API (functions/types/macros prefixed with ``trax_``) has been made into a private API. Thus, the previous include path ``#include "trax.h"`` has been removed. If users still require usage of the Trax API (though this is not recommended), it can be included via  ``#include "esp_private/trax.h"``.

ROM
---

The previously deprecated ROM-related header files located in ``components/esp32/rom/`` (old include path: ``rom/*.h``) have been moved. Please use the new target-specific path from ``components/esp_rom/include/{IDF_TARGET_NAME}/`` (new include path: ``{IDF_TARGET_NAME}/rom/*.h``).

``esp_hw_support``
------------------

- The header files ``soc/cpu.h`` have been deleted and deprecated CPU util functions have been removed. ESP-IDF developers should include ``esp_cpu.h`` instead for equivalent functions.
- The header files ``hal/cpu_ll.h``, ``hal/cpu_hal.h``, ``hal/soc_ll.h``, ``hal/soc_hal.h`` and ``interrupt_controller_hal.h`` CPU API functions have been deprecated. ESP-IDF developers should include ``esp_cpu.h`` instead for equivalent functions.
- The header file ``compare_set.h`` have been deleted. ESP-IDF developers should use ``esp_cpu_compare_and_set()`` function provided in ``esp_cpu.h`` instead.
- ``esp_cpu_get_ccount()``, ``esp_cpu_set_ccount()`` and ``esp_cpu_in_ocd_debug_mode()`` were removed from ``esp_cpu.h``. ESP-IDF developers should use respectively ``esp_cpu_get_cycle_count()``, ``esp_cpu_set_cycle_count()`` and ``esp_cpu_dbgr_is_attached()`` instead.
- The header file ``esp_intr.h`` has been deleted. Please include ``esp_intr_alloc.h`` to allocate and manipulate interrupts.
- The Panic API (functions/types/macros prefixed with ``esp_panic``) has been made into a private API. Thus, the previous include path ``#include "esp_panic.h"`` has been removed. If users still require usage of the Trax API (though this is not recommended), it can be included via  ``#include "esp_private/panic_reason.h"``. Besides, developers should include ``esp_debug_helpers.h`` instead to use any debug-related helper functions, e.g., print backtrace.
- The header file ``soc_log.h`` is now renamed to ``esp_hw_log.h`` and has been made private. Users are encouraged to use logging APIs provided under ``esp_log.h`` instead.
- The header files ``spinlock.h``, ``clk_ctrl_os.h``, and ``rtc_wdt.h`` must now be included without the ``soc`` prefix. For example, ``#include "spinlock.h"``.
- ``esp_chip_info()`` returns the chip version in the format  = 100 * ``major eFuse version`` + ``minor eFuse version``. Thus, the ``revision`` in the ``esp_chip_info_t`` structure is expanded to ``uint16_t`` to fit the new format.

PSRAM
^^^^^

- The target-specific header file ``spiram.h`` and the header file ``esp_spiram.h`` have been removed. A new component ``esp_psram`` is created instead. For PSRAM/SPIRAM-related functions, users now include ``esp_psram.h`` and set the ``esp_psram`` component as a component requirement in their ``CMakeLists.txt`` project files.
- ``esp_spiram_get_chip_size`` and ``esp_spiram_get_size`` have been deleted. You should use ``esp_psram_get_size`` instead.

eFuse
----------

- The parameter type of function ``esp_secure_boot_read_key_digests()`` changed from ``ets_secure_boot_key_digests_t*`` to ``esp_secure_boot_key_digests_t*``. The new type is the same as the old one, except that the ``allow_key_revoke`` flag has been removed. The latter was always set to ``true`` in current code, not providing additional information.
- Added eFuse wafer revisions: major and minor. The ``esp_efuse_get_chip_ver()`` API is not compatible with these changes, so it was removed. Instead, please use the following APIs: ``efuse_hal_get_major_chip_version()``, ``efuse_hal_get_minor_chip_version()`` or ``efuse_hal_chip_revision()``.

``esp_common``
--------------

``EXT_RAM_ATTR`` is deprecated. Use the new macro ``EXT_RAM_BSS_ATTR`` to put ``.bss`` on PSRAM.

``esp_system``
--------------

- The header files ``esp_random.h``, ``esp_mac.h``, and ``esp_chip_info.h``, which were all previously indirectly included via the header file ``esp_system.h``, must now be included directly. These indirect inclusions from ``esp_system.h`` have been removed.
- The Backtrace Parser API (functions/types/macros prefixed with ``esp_eh_frame_``) has been made into a private API. Thus, the previous include path ``#include "eh_frame_parser.h"`` has been removed. If users still require usage of the Backtrace Parser API (though this is not recommended), it can be included via ``#include "esp_private/eh_frame_parser.h"``.
- The Interrupt Watchdog API (functions/types/macros prefixed with ``esp_int_wdt_``) has been made into a private API. Thus, the previous include path ``#include "esp_int_wdt.h"`` has been removed. If users still require usage of the Interrupt Watchdog API (though this is not recommended), it can be included via  ``#include "esp_private/esp_int_wdt.h"``.

SoC Dependency
--------------

- Public API headers listed in the Doxyfiles will not expose unstable and unnecessary SoC header files, such as ``soc/soc.h`` and ``soc/rtc.h``. That means the user has to explicitly include them in their code if these "missing" header files are still wanted.
- Kconfig option ``LEGACY_INCLUDE_COMMON_HEADERS`` is also removed.
- The header file ``soc/soc_memory_types.h`` has been deprecated. Users should use the ``esp_memory_utils.h`` instead. Including ``soc/soc_memory_types.h`` will bring a build warning like ``soc_memory_types.h is deprecated, please migrate to esp_memory_utils.h``

APP Trace
---------

One of the timestamp sources has changed from the legacy timer group driver to the new :doc:`GPTimer <../../../api-reference/peripherals/gptimer>`. Kconfig choices like ``APPTRACE_SV_TS_SOURCE_TIMER00`` has been changed to ``APPTRACE_SV_TS_SOURCE_GPTIMER``. User no longer need to choose the group and timer ID.

``esp_timer``
-------------

The FRC2-based legacy implementation of ``esp_timer`` available on ESP32 has been removed. The simpler and more efficient implementation based on the LAC timer is now the only option.

ESP Image
---------

The image SPI speed enum definitions have been renamed.

- Enum ``ESP_IMAGE_SPI_SPEED_80M`` has been renamed to ``ESP_IMAGE_SPI_SPEED_DIV_1``.
- Enum ``ESP_IMAGE_SPI_SPEED_40M`` has been renamed to ``ESP_IMAGE_SPI_SPEED_DIV_2``.
- Enum ``ESP_IMAGE_SPI_SPEED_26M`` has been renamed to ``ESP_IMAGE_SPI_SPEED_DIV_3``.
- Enum ``ESP_IMAGE_SPI_SPEED_20M`` has been renamed to ``ESP_IMAGE_SPI_SPEED_DIV_4``.

Task Watchdog Timers
--------------------

- The API for ``esp_task_wdt_init()`` has changed as follows:

    - Configuration is now passed as a configuration structure.
    - The function will now handle subscribing of the idle tasks if configured to do so.

- The former ``CONFIG_ESP_TASK_WDT`` configuration option has been renamed to :ref:`CONFIG_ESP_TASK_WDT_INIT` and a new :ref:`CONFIG_ESP_TASK_WDT_EN` option has been introduced.

FreeRTOS
--------

Legacy API and Data Types
^^^^^^^^^^^^^^^^^^^^^^^^^

Previously, the ``configENABLE_BACKWARD_COMPATIBILITY`` option was set by default, thus allowing pre FreeRTOS v8.0.0 function names and data types to be used. The ``configENABLE_BACKWARD_COMPATIBILITY`` is now disabled by default, thus legacy FreeRTOS names/types are no longer supported by default. Users should do one of the following:

- Update their code to remove usage of legacy FreeRTOS names/types.
- Enable the :ref:`CONFIG_FREERTOS_ENABLE_BACKWARD_COMPATIBILITY` to explicitly allow the usage of legacy names/types.

Tasks Snapshot
^^^^^^^^^^^^^^

The header ``task_snapshot.h`` has been removed from ``freertos/task.h``. ESP-IDF developers should include ``freertos/task_snapshot.h`` if they need tasks snapshot API.

The function :cpp:func:`vTaskGetSnapshot` now returns ``BaseType_t``. Return value shall be ``pdTRUE`` on success and ``pdFALSE`` otherwise.

FreeRTOS Asserts
^^^^^^^^^^^^^^^^

Previously, FreeRTOS asserts were configured separately from the rest of the system using the ``FREERTOS_ASSERT`` kconfig option. This option has now been removed and the configuration is now done through ``COMPILER_OPTIMIZATION_ASSERTION_LEVEL``.

Port Macro API
^^^^^^^^^^^^^^^

The file ``portmacro_deprecated.h`` which was added to maintain backward compatibility for deprecated APIs is removed. Users are advised to use the alternate functions for the deprecated APIs as listed below:

- ``portENTER_CRITICAL_NESTED()`` is removed. Users should use the ``portSET_INTERRUPT_MASK_FROM_ISR()`` macro instead.
- ``portEXIT_CRITICAL_NESTED()`` is removed. Users should use the ``portCLEAR_INTERRUPT_MASK_FROM_ISR()`` macro instead.
- ``vPortCPUInitializeMutex()`` is removed. Users should use the ``spinlock_initialize()`` function instead.
- ``vPortCPUAcquireMutex()`` is removed. Users should use the ``spinlock_acquire()`` function instead.
- ``vPortCPUAcquireMutexTimeout()`` is removed. Users should use the ``spinlock_acquire()`` function instead.
- ``vPortCPUReleaseMutex()`` is removed. Users should use the ``spinlock_release()`` function instead.

App Update
----------

- The functions :cpp:func:`esp_ota_get_app_description` and :cpp:func:`esp_ota_get_app_elf_sha256` have been termed as deprecated. Please use the alternative functions :cpp:func:`esp_app_get_description` and :cpp:func:`esp_app_get_elf_sha256` respectively. These functions have now been moved to a new component :component:`esp_app_format`. Please refer to the header file :component_file:`esp_app_desc.h <esp_app_format/include/esp_app_desc.h>`.

Bootloader Support
------------------

- The :cpp:type:`esp_app_desc_t` structure, which used to be declared in :component_file:`esp_app_format.h <bootloader_support/include/esp_app_format.h>`, is now declared in :component_file:`esp_app_desc.h <esp_app_format/include/esp_app_desc.h>`.

- The function :cpp:func:`bootloader_common_get_partition_description` has now been made private. Please use the alternative function :cpp:func:`esp_ota_get_partition_description`. Note that this function takes :cpp:type:`esp_partition_t` as its first argument instead of :cpp:type:`esp_partition_pos_t`.

Chip Revision
^^^^^^^^^^^^^

The bootloader checks the chip revision at the beginning of the application loading. The application can only be loaded if the version is ``>=`` :ref:`CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MIN` and ``<`` ``CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MAX_FULL``.

During the OTA upgrade, the version requirements and chip revision in the application header are checked for compatibility. The application can only be updated if the version is ``>=`` :ref:`CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MIN` and ``<`` ``CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MAX_FULL``.
