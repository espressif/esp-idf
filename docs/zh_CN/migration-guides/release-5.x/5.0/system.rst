系统
======

:link_to_translation:`en:[English]`

跨核执行
----------

跨核执行 (Inter-Processor Call, IPC) 不再是一个独立组件，现已包含至 ``esp_system``。

因此，若项目的 ``CMakeLists.txt`` 文件中出现 ``PRIV_REQUIRES esp_ipc`` 或 ``REQUIRES esp_ipc``，应删除这些选项，因为项目中已默认包含 ``esp_system`` 组件。

ESP 时钟
---------

ESP 时钟 API（即以 ``esp_clk`` 为前缀的函数、类型或宏）已更新为私有 API。因此，原先的包含路径 ``#include "{IDF_TARGET_NAME}/clk.h"`` 和 ``#include "esp_clk.h"`` 已移除。如仍需使用 ESP 时钟 API（并不推荐），请使用 ``#include "esp_private/esp_clk.h"`` 来包含。

.. 注意::

    私有 API 不属于稳定的 API，不会遵循 ESP-IDF 的版本演进规则，因此不推荐用户在应用中使用。

缓存错误中断
--------------

缓存错误中断 API（即以 ``esp_cache_err`` 为前缀的函数、类型或宏）已更新为私有 API。因此，原先的包含路径 ``#include "{IDF_TARGET_NAME}/cache_err_int.h"`` 已移除。如仍需使用缓存错误中断 API（并不推荐），请使用 ``#include "esp_private/cache_err_int.h"`` 来包含。

``bootloader_support``
------------------------

* 函数 ``bootloader_common_get_reset_reason()`` 已移除。请使用 ROM 组件中的函数 ``esp_rom_get_reset_reason()``。
* 函数 ``esp_secure_boot_verify_sbv2_signature_block()`` 和 ``esp_secure_boot_verify_rsa_signature_block()`` 已移除，无新的替换函数。不推荐用户直接使用以上函数。如确需要，请在 `GitHub <https://github.com/espressif/esp-idf/issues/new/choose>`_ 上对该功能提交请求，并解释需要此函数的原因。

断电
--------

断电 API（即以 ``esp_brownout`` 为前缀的函数、类型或宏）已更新为私有 API。因此，原先的包含路径 ``#include "brownout.h"`` 已移除。如仍需使用断电 API（并不推荐），请使用 ``#include "esp_private/brownout.h"`` 来包含。

Trax
----

Trax API（即以 ``trax_`` 为前缀的函数、类型或宏）已更新为私有 API。因此，原先的包含路径 ``#include "trax.h"`` 已移除。如仍需使用 Trax API（并不推荐），请使用 ``#include "esp_private/trax.h"`` 来包含。

ROM
---

``components/esp32/rom/``  路径下存放的已弃用的 ROM 相关头文件已移除（原包含路径为 ``rom/*.h``）。请使用新的特定目标的路径 ``components/esp_rom/include/{IDF_TARGET_NAME}/`` （新的包含路径为 ``{IDF_TARGET_NAME}/rom/*.h``）。

``esp_hw_support``
---------------------

- 头文件 ``soc/cpu.h`` 及弃用的 CPU util 函数都已移除。请包含 ``esp_cpu.h`` 来代替相同功能的函数。
- 头文件 ``hal/cpu_ll.h``、``hal/cpu_hal.h``、``hal/soc_ll.h``、``hal/soc_hal.h`` 和 ``interrupt_controller_hal.h`` 的 CPU API 函数已弃用。请包含 ``esp_cpu.h`` 来代替相同功能的函数。
- 头文件 ``compare_set.h`` 已移除。请使用 ``esp_cpu.h`` 中提供的 ``esp_cpu_compare_and_set()`` 函数来代替。
- ``esp_cpu_get_ccount()``、``esp_cpu_set_ccount()`` 和 ``esp_cpu_in_ocd_debug_mode()`` 已从 ``esp_cpu.h`` 中移除。请分别使用 ``esp_cpu_get_cycle_count()``、``esp_cpu_set_cycle_count()`` 和 ``esp_cpu_dbgr_is_attached()`` 代替。
- 头文件 ``esp_intr.h`` 已移除。请包含 ``esp_intr_alloc.h`` 以分配和操作中断。
- Panic API（即以 ``esp_panic`` 为前缀的函数、类型或宏）已更新为私有 API。因此，原先的包含路径 ``#include "esp_panic.h"`` 已移除。如仍需使用 Panic API（并不推荐），请使用 ``#include "esp_private/panic_reason.h"`` 来包含。此外，请包含 ``esp_debug_helpers.h`` 以使用与调试有关的任意辅助函数，如打印回溯。
- 头文件 ``soc_log.h`` 现更名为 ``esp_hw_log.h``，并已更新为私有。建议用户使用 ``esp_log.h`` 头文件下的日志 API。
- 包含头文件 ``spinlock.h``、``clk_ctrl_os.h`` 和 ``rtc_wdt.h`` 时不应当使用 ``soc`` 前缀，如 ``#include "spinlock.h"``。
- ``esp_chip_info()`` 命令返回芯片版本，格式为 = 100 * ``主要 eFuse 版本`` + ``次要 eFuse 版本``。因此，为适应新格式， ``esp_chip_info_t`` 结构体中的 ``revision`` 扩展为 ``uint16_t``。

PSRAM
^^^^^

- 针对特定目标的头文件 ``spiram.h`` 及头文件 ``esp_spiram.h`` 已移除，创建新组件 ``esp_psram``。对于与 PSRAM 或 SPIRAM 相关的函数，请包含 ``esp_psram.h``，并在 CMakeLists.txt 项目文件中将 ``esp_psram`` 设置为必需组件。
- ``esp_spiram_get_chip_size`` 和 ``esp_spiram_get_size`` 已移除，请使用 ``esp_psram_get_size``。

eFuse
-------

- 函数 ``esp_secure_boot_read_key_digests()`` 的参数类型从 ``ets_secure_boot_key_digests_t*`` 更新为 ``esp_secure_boot_key_digests_t*``。新类型与旧类型相同，仅移除了 ``allow_key_revoke`` 标志。在当前代码中，后者总是设置为 ``true``，并未提供额外信息。
- 针对 eFuse 晶圆增加主要修订版本和次要修订版本。API ``esp_efuse_get_chip_ver()`` 与新修订不兼容，因此已移除。请使用 API ``efuse_hal_get_major_chip_version()``、``efuse_hal_get_minor_chip_version()`` 或  ``efuse_hal_chip_revision()`` 来代替原有 API。

``esp_common``
----------------

``EXT_RAM_ATTR`` 已弃用。请使用新的宏 ``EXT_RAM_BSS_ATTR`` 以将 ``.bss`` 放在 PSRAM 上。

``esp_system``
----------------

- 头文件 ``esp_random.h``、``esp_mac.h`` 和 ``esp_chip_info.h`` 以往都是通过头文件 ``esp_system.h`` 间接包含，更新后必须直接包含。已移除从 ``esp_system.h`` 中的间接包含功能。
- 回溯解析器 API（即以 ``esp_eh_frame_`` 为前缀的函数、类型或宏）已更新为私有 API。因此，原先的包含路径 ``#include "eh_frame_parser.h"`` 已移除。如仍需使用回溯解析器 API（并不推荐），请使用 ``#include "esp_private/eh_frame_parser.h"`` 来包含。
- 中断看门狗定时器 API（即以 ``esp_int_wdt_`` 为前缀的函数、类型或宏）已更新为私有 API。因此，原先的包含路径 ``#include "esp_int_wdt.h"`` 已移除。如仍需使用中断看门狗定时器 API（并不推荐），请使用 ``#include "esp_private/esp_int_wdt.h"`` 来包含。

SOC 依赖性
--------------

- Doxyfiles 中列出的公共 API 头文件中不会显示不稳定和非必要的 SOC 头文件，如 ``soc/soc.h`` 和 ``soc/rtc.h``。这意味着，如果用户仍然需要这些“缺失”的头文件，就必须在代码中明确包含这些文件。
- Kconfig 选项 ``LEGACY_INCLUDE_COMMON_HEADERS`` 也已移除。
- 头文件 ``soc/soc_memory_types.h`` 已弃用。请使用 ``esp_memory_utils.h``。包含 ``soc/soc_memory_types.h`` 将触发构建警告，如 ``soc_memory_types.h is deprecated, please migrate to esp_memory_utils.h``。

应用跟踪
--------

其中一个时间戳源已从定时器组驱动改为新的 :doc:`GPTimer <../../../api-reference/peripherals/gptimer>`。Kconfig 选项已重新命名，例如 ``APPTRACE_SV_TS_SOURCE_TIMER00`` 已更改为 ``APPTRACE_SV_TS_SOURCE_GPTIMER``。用户已无需选择组和定时器 ID。

``esp_timer``
---------------

基于 FRC2 的 ``esp_timer`` 过去可用于 ESP32，现在已移除，更新后仅可使用更简单有效的 LAC 定时器。

ESP 镜像
---------

ESP 镜像中关于 SPI 速度的枚举成员已重新更名：

- ``ESP_IMAGE_SPI_SPEED_80M`` 已重新命名为 ``ESP_IMAGE_SPI_SPEED_DIV_1``。
- ``ESP_IMAGE_SPI_SPEED_40M`` 已重新命名为 ``ESP_IMAGE_SPI_SPEED_DIV_2``。
- ``ESP_IMAGE_SPI_SPEED_26M`` 已重新命名为 ``ESP_IMAGE_SPI_SPEED_DIV_3``。
- ``ESP_IMAGE_SPI_SPEED_20M`` 已重新命名为 ``ESP_IMAGE_SPI_SPEED_DIV_4``。

任务看门狗定时器
--------------------

- API ``esp_task_wdt_init()`` 更新后有如下变化：

    - 以结构体的形式传递配置。
    - 可将该函数配置为订阅空闲任务。

- 原先的配置选项 ``CONFIG_ESP_TASK_WDT`` 重新命名为 :ref:`CONFIG_ESP_TASK_WDT_INIT` 并引入了一个新选项 :ref:`CONFIG_ESP_TASK_WDT_EN`。

FreeRTOS
--------

遗留 API 及数据类型
^^^^^^^^^^^^^^^^^^^^^^^^^

在以往版本中，ESP-IDF 默认设置 ``configENABLE_BACKWARD_COMPATIBILITY`` 选项，因此可使用 FreeRTOS v8.0.0 之前的函数名称和数据类型。该选项现在已默认禁用，因此默认情况下不再支持以往的 FreeRTOS 名称或类型。用户可以选择以下一种解决方式：

- 更新代码，删除以往的 FreeRTOS 名称或类型。
- 启用 :ref:`CONFIG_FREERTOS_ENABLE_BACKWARD_COMPATIBILITY` 以显式调用这些名称或类型。

任务快照
^^^^^^^^^^

头文件 ``task_snapshot.h`` 已从 ``freertos/task.h`` 中移除。如需使用任务快照 API，请包含 ``freertos/task_snapshot.h``。

函数 :cpp:func:`vTaskGetSnapshot` 现返回 ``BaseType_t``，成功时返回值为 ``pdTRUE``，失败则返回 ``pdFALSE``。

FreeRTOS 断言
^^^^^^^^^^^^^^^^

在以往版本中，FreeRTOS 断言通过 ``FREERTOS_ASSERT`` kconfig 选项独立配置，不同于系统的其他部分。该选项已移除，现在需要通过 ``COMPILER_OPTIMIZATION_ASSERTION_LEVEL`` 来完成配置。

FreeRTOS 移植相关的宏
^^^^^^^^^^^^^^^^^^^^^^^^^^

已移除用以保证弃用 API 向后兼容性的 ``portmacro_deprecated.h`` 文件。建议使用下列函数来代替弃用 API。

- ``portENTER_CRITICAL_NESTED()`` 已移除，请使用 ``portSET_INTERRUPT_MASK_FROM_ISR()`` 宏。
- ``portEXIT_CRITICAL_NESTED()`` 已移除，请使用 ``portCLEAR_INTERRUPT_MASK_FROM_ISR()`` 宏。
- ``vPortCPUInitializeMutex()`` 已移除，请使用  ``spinlock_initialize()`` 函数。
- ``vPortCPUAcquireMutex()`` 已移除，请使用 ``spinlock_acquire()`` 函数。
- ``vPortCPUAcquireMutexTimeout()`` 已移除，请使用 ``spinlock_acquire()`` 函数。
- ``vPortCPUReleaseMutex()`` 已移除，请使用 ``spinlock_release()`` 函数。

应用程序更新
------------

- 函数 :cpp:func:`esp_ota_get_app_description` 和 :cpp:func:`esp_ota_get_app_elf_sha256` 已弃用，请分别使用 :cpp:func:`esp_app_get_description` 和 :cpp:func:`esp_app_get_elf_sha256` 函数来代替。这些函数已移至新组件 :component:`esp_app_format`。请参考头文件 :component_file:`esp_app_desc.h <esp_app_format/include/esp_app_desc.h>`。

引导加载程序支持
----------------

- :cpp:type:`esp_app_desc_t` 结构体此前在 :component_file:`esp_app_format.h <bootloader_support/include/esp_app_format.h>` 中声明，现在在 :component_file:`esp_app_desc.h <esp_app_format/include/esp_app_desc.h>` 中声明。

- 函数 :cpp:func:`bootloader_common_get_partition_description` 已更新为私有函数，请使用代替函数 :cpp:func:`esp_ota_get_partition_description`。注意，此函数的第一个参数为 :cpp:type:`esp_partition_t`，而非 :cpp:type:`esp_partition_pos_t`。

芯片版本
^^^^^^^^^^^^^

在应用程序开始加载时，引导加载程序会检查芯片版本。只有当版本为 ``>=`` :ref:`CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MIN` 和 ``<`` ``CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MAX_FULL`` 时，应用程序才能成功加载。

在 OTA 升级时，会检查应用程序头部中的版本需求和芯片版本是否符合条件。只有当版本为 ``>=`` :ref:`CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MIN` 和 ``<`` ``CONFIG_{IDF_TARGET_CFG_PREFIX}_REV_MAX_FULL`` 时，应用程序才能成功更新。
