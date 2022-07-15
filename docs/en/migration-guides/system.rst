Migrate System to ESP-IDF 5.0
==================================

Inter-Processor Call
-----------------------

IPC (Inter-Processor Call) component has been moved to ``esp_system``.

Thus, any project presenting a ``CMakeLists.txt`` file with the parameters ``PRIV_REQUIRES esp_ipc`` or ``REQUIRES esp_ipc``, should be modified to simply remove these options as ``esp_system`` component is included by default.

ESP Clock
---------

The old headers ``{IDF_TARGET_NAME}/clk.h``, ``esp_clk.h`` have been removed.

Therefore, If you want to use the function with the prefix ``esp_clk`` please include ``esp_private/esp_clk.h`` instead.

Cache Error Interrupt
---------------------

The old headers ``{IDF_TARGET_NAME}/cache_err_int.h`` have been removed. Please include ``esp_private/cache_err_int.h`` instead.

Brownout
--------

The header ``brownout.h`` has been made private. ESP-IDF developers should include ``esp_private/brownout.h`` instead.

Trax
----

The header ``trax.h`` has been made private. ESP-IDF developers should include ``esp_private/trax.h`` instead.

ROM
---
Deprecated ROM related header files from `components/esp32/rom/` (old include path: `rom/*.h`) have been deleted. Please update to use the new target-specific path from `components/esp_rom/include/{IDF_TARGET_NAME}/` (new include path: `{IDF_TARGET_NAME}/rom/*.h`).

ESP HW Support
--------------

- The header files ``soc/cpu.h`` have been deleted and deprecated CPU util functions have been removed. ESP-IDF developers should include ``esp_cpu.h`` instead for equivalent functions.
- The header file ``esp_intr.h`` has been deleted. Please include ``esp_intr_alloc.h`` to allocate and manipulate interrupts.
- The header file ``esp_panic.h`` has been deleted. ESP-IDF developers should include ``esp_private/panic_reason.h`` to get supported panic reasons. And should include ``esp_debug_helpers.h`` to use any debug related helper functions, e.g. print backtrace.
- The header file ``soc_log.h`` is now renamed to ``esp_hw_log.h`` and all logging macros have been updated from ``SOC_LOGx`` to ``ESP_HW_LOGx``. ESP-IDF users must use the later form.
- The header files ``spinlock.h``, ``clk_ctrl_os.h`` and ``rtc_wdt.h`` must now be included without the ``soc`` prefix. Eg:- ``#include "spinlock.h"``.
- ``esp_chip_info()`` returns the chip version in the format  = 100 * ``major eFuse version`` + ``minor eFuse version``. Thus, the ``revision`` in the ``esp_chip_info_t`` structure is expanded to uint16_t to fit the new format.

PSRAM
^^^^^
- The target specific header files ``spiram.h`` have been deleted. The header file ``esp_spiram.h`` has been deleted. A new component ``esp_psram`` is created, you should include ``esp_psram.h`` instead. Besides, you might need to add ``esp_psram`` component to the list of component requirements in CMakeLists.txt.
- ``esp_spiram_get_chip_size`` and ``esp_spiram_get_size`` have been deleted. You should use ``esp_psram_get_size`` instead.

ESP Common
----------

- `EXT_RAM_ATTR` is deprecated. Use this new macro `EXT_RAM_BSS_ATTR` to put .bss on PSRAM.

ESP System
----------
- The header files ``esp_random.h``, ``esp_mac.h`` and ``esp_chip_info.h``, which were all previously indirectly included via the header file ``esp_system.h``, must now be included directly. These headers are removed from ``esp_system.h``.
- The header file ``eh_frame_parser.h`` must now be included with a ``esp_private`` prefix like ``#include "esp_private/eh_frame_parser.h"``.
- The header file ``esp_int_wdt.h`` must now be included with a ``esp_private`` prefix like ``#include "esp_private/esp_int_wdt.h"``.

SOC dependency
--------------

- Public API headers who are listed in the Doxyfiles won't expose unstable and unnecessary soc header files like ``soc/soc.h``, ``soc/rtc.h``. That means, the user has to explicitly include them in their code if these "missing" header files are still wanted.
- Kconfig option ``LEGACY_INCLUDE_COMMON_HEADERS`` is also removed.
- The header file ``soc/soc_memory_types.h`` has been deprecated. Users should use the ``esp_memory_utils.h`` instead. Including `soc/soc_memory_types.h` will bring a build warning like `soc_memory_types.h is deprecated, please migrate to esp_memory_utils.h`

APP Trace
---------

One of the timestamp sources has changed from the legacy timer group driver to the new :doc:`GPTimer <../api-reference/peripherals/gptimer>`. Kconfig choices like ``APPTRACE_SV_TS_SOURCE_TIMER00`` has been changed to ``APPTRACE_SV_TS_SOURCE_GPTIMER``. User doesn't need to choose the group and timer ID any more.

ESP Timer
---------

Removed the FRC2 based legacy implementation of esp_timer available on ESP32. The simpler and more efficient implementation based on the LAC timer is now the only option.

ESP image
---------

Rename the image SPI speed enum definition.
- Enum ``ESP_IMAGE_SPI_SPEED_80M`` has been renamed to ``ESP_IMAGE_SPI_SPEED_DIV_1``.
- Enum ``ESP_IMAGE_SPI_SPEED_40M`` has been renamed to ``ESP_IMAGE_SPI_SPEED_DIV_2``.
- Enum ``ESP_IMAGE_SPI_SPEED_26M`` has been renamed to ``ESP_IMAGE_SPI_SPEED_DIV_3``.
- Enum ``ESP_IMAGE_SPI_SPEED_20M`` has been renamed to ``ESP_IMAGE_SPI_SPEED_DIV_4``.

Task Watchdog Timers
--------------------

- The API for ``esp_task_wdt_init()`` has changed as follows

    - Configuration is now passed as a configuration structure.
    - The function will now handle subscribing of the idle tasks if configured to do so

Efuse
-----

- Added eFuse wafer revisions: major and minor. The `esp_efuse_get_chip_ver()` API is not compatible with these changes this is why it was removed. Please use instead of it the following APIs: `efuse_hal_get_major_chip_version()`, `efuse_hal_get_minor_chip_version()` or `efuse_hal_chip_revision()`.
