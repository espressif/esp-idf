系统
====

:link_to_translation:`en:[English]`

Xtensa
------

Xtensa 特殊寄存器头文件已更新，使用新的命名约定。旧的 ``specreg.h`` 头文件现已被弃用，并将在未来版本中移除。

寄存器名称已更新为使用 ``XT_REG_`` 前缀。请使用新的 ``xt_specreg.h`` 文件。

电源管理
--------

在旧版本的 ESP-IDF 中，使用 :cpp:func:`esp_sleep_get_wakeup_cause` API 获取芯片从睡眠中唤醒的原因，然而，该函数在多个唤醒源同时激活的情况下，只会返回其中一个唤醒源，可能导致用户遗漏其他同时发生的唤醒条件。

自 v6.0 版本起，ESP-IDF 新增 :cpp:func:`esp_sleep_get_wakeup_causes` API，此函数返回一个 bitmap，表示所有触发唤醒的唤醒源。每一位对应 :cpp:type:`esp_sleep_wakeup_cause_t` 枚举中的一个值（例如 ESP_SLEEP_WAKEUP_TIMER、ESP_SLEEP_WAKEUP_EXT1 等），用户可以通过按位与操作判断是否被对应源唤醒。

原先的 :cpp:func:`esp_sleep_get_wakeup_cause()` 函数现已标记为已弃用，建议用户迁移至新接口。未来版本中，该函数可能会被移除。用户可按以下示例进行迁移：

旧代码：

.. code-block:: c

    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    if (cause == ESP_SLEEP_WAKEUP_EXT1) {
        handle_ext1_wakeup();
    }

现在需要修改成：

.. code-block:: c

    uint32_t causes = esp_sleep_get_wakeup_causes();
    if (causes & BIT(ESP_SLEEP_WAKEUP_EXT1)) {
        handle_ext1_wakeup();
    }
    if (causes & BIT(ESP_SLEEP_WAKEUP_TIMER)) {
        handle_timer_wakeup();
    }

引导加载程序
------------

已移除使用无优化等级 (-O0, `CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION_NONE`) 编译引导加载程序的选项。在大多数芯片上，使用 -O0 编译引导加载程序已无法成功，因为 IRAM 段会溢出。对于调试目的，推荐使用 -Og (:ref:`CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION_DEBUG<CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION_DEBUG>`) 优化等级，它在优化效果与可调试性之间提供了良好的平衡。

时间管理
--------

- 已弃用的头文件 ``{IDF_TARGET_NAME}/rtc.h`` 已被移除，请改用替代头文件 ``esp_rtc_time.h``。
- 已弃用的 ``RTC_CLK_SRC_INT_RC32K`` 配置项已被移除，内部 RC32K 时钟在极端温度下不稳定，请不要使用该时钟作为 RTC 低速时钟的时钟源。

硬件支持
--------

已弃用的头文件 ``soc_memory_types.h`` 已被移除，请改用替代头文件 ``esp_memory_utils.h``。

已弃用的头文件 ``intr_types.h`` 已被移除，请改用替代头文件 ``esp_intr_types.h``。

已弃用的头文件 ``esp_private/interrupt_deprecated.h`` （此前通过 ``riscv/interrupt.h`` 头文件提供）已被移除。相关已弃用的函数不再可用，请改用非弃用版本。

ROM 头文件
-----------

已从 ``ets_sys.h`` ROM 头文件中移除了已弃用的 ``STATUS`` 类型。请改用 ``ETS_STATUS``。

App 追踪
----------

已移除额外数据缓冲选项。不再支持 `CONFIG_APPTRACE_PENDING_DATA_SIZE_MAX` 配置项。

已移除弃用的 `ESP_APPTRACE_DEST_TRAX` 枚举值。请改用 `ESP_APPTRACE_DEST_JTAG`。

函数 :cpp:func:`esp_apptrace_down_buffer_config` 现在需要一个目标参数，并返回一个错误代码以便进行适当的错误处理。

旧代码：

.. code-block:: c

    esp_apptrace_down_buffer_config(down_buf, sizeof(down_buf));

现在需要修改成：

.. code-block:: c

    esp_err_t res = esp_apptrace_down_buffer_config(ESP_APPTRACE_DEST_JTAG, down_buf, sizeof(down_buf));
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "Failed to config down buffer!");
        return res;
    }

UART 目标配置已简化：

- 移除：通过 ``CONFIG_APPTRACE_DEST_UARTx=y`` 选择单个 UART
- 新增：通过 ``CONFIG_APPTRACE_DEST_UART_NUM`` 选择 UART 端口

迁移方法，更新你的 sdkconfig 配置：

旧配置：

.. code-block:: none

    CONFIG_APPTRACE_DEST_UART0=y
    # 或
    CONFIG_APPTRACE_DEST_UART1=y

新配置：

.. code-block:: none

    CONFIG_APPTRACE_DEST_UART=y
    CONFIG_APPTRACE_DEST_UART_NUM=0  # 或 1、2，具体取决于目标芯片

FreeRTOS
--------

**已移除的函数**

以下已弃用的 FreeRTOS 函数已在 ESP-IDF v6.0 中移除：

- :cpp:func:`xTaskGetAffinity` – 请使用 :cpp:func:`xTaskGetCoreID` 替代。
- :cpp:func:`xTaskGetIdleTaskHandleForCPU` – 请使用 :cpp:func:`xTaskGetIdleTaskHandleForCore` 替代。
- :cpp:func:`xTaskGetCurrentTaskHandleForCPU` – 请使用 :cpp:func:`xTaskGetCurrentTaskHandleForCore` 替代。

以下兼容性函数已在 ESP-IDF v6.0 中移除。这些函数原本是为了向后兼容旧版本 ESP-IDF 而维护的，因为它们在 FreeRTOS 中已被更改为宏或独立函数。现已移除此兼容性支持。

- :cpp:func:`xQueueGenericReceive` - 请根据具体使用场景选择 :cpp:func:`xQueueReceive`、:cpp:func:`xQueuePeek` 或 :cpp:func:`xQueueSemaphoreTake` 替代
- :cpp:func:`vTaskDelayUntil` - 请使用 :cpp:func:`xTaskDelayUntil` 替代
- :cpp:func:`ulTaskNotifyTake` - 请使用宏 ``ulTaskNotifyTake`` 替代
- :cpp:func:`xTaskNotifyWait` - 请使用宏 ``xTaskNotifyWait`` 替代

**已弃用的函数**

函数 :cpp:func:`pxTaskGetStackStart` 已弃用。请使用 :cpp:func:`xTaskGetStackStart` 替代以提高类型安全性。

**新增 API**

任务快照 API 已对外公开，以支持 ESP Insights 等外部框架。这些 API 现通过 ``freertos/freertos_debug.h`` 头文件提供，不再使用已弃用的 ``freertos/task_snapshot.h``。

在调度程序运行时安全使用的方案是：调用快照函数前先执行 ``vTaskSuspendAll()`` 暂停所有任务，完成后调用 ``xTaskResumeAll()`` 恢复运行。

**内存布局**

- 为了减少 IRAM 的使用，大多数 FreeRTOS 函数的默认存储位置已从 IRAM 更改为 flash。因此，``CONFIG_FREERTOS_PLACE_FUNCTIONS_INTO_FLASH`` 选项已被移除。这项变更可显著节省 IRAM 空间，但可能会对性能造成轻微影响。如果应用对性能有严苛要求，可通过启用新选项 :ref:`CONFIG_FREERTOS_IN_IRAM` 恢复原先配置。
- 启用 ``CONFIG_FREERTOS_IN_IRAM`` 前，建议进行性能测试以评估对具体应用场景的实际影响。flash 和 IRAM 配置的性能差异取决于 flash 缓存效率、API 调用模式和系统负载等因素。
- ``components/freertos/test_apps/freertos/performance/test_freertos_api_performance.c`` 中提供了基准性能测试。该测试测量常用 FreeRTOS API 的执行时长，可帮助开发者根据目标硬件和应用需求评估内存布局方案带来的性能影响。
- 当启用 ``CONFIG_ESP_PANIC_HANDLER_IRAM`` 时，任务快照函数会自动存入 IRAM，确保在系统崩溃处理期间仍可调用。
- 除非启用 ``CONFIG_FREERTOS_PLACE_ISR_FUNCTIONS_INTO_FLASH``，否则 ``vTaskGetSnapshot`` 将始终保留在 IRAM 中，因为该函数被任务看门狗中断处理程序所调用。

**已移除的配置选项**

以下隐藏（且始终启用）的配置选项已被移除：

- ``CONFIG_FREERTOS_ENABLE_TASK_SNAPSHOT``
- ``CONFIG_FREERTOS_PLACE_SNAPSHOT_FUNS_INTO_FLASH``

环形缓冲区
----------

**内存布局**

为了减少 IRAM 的使用，`esp_ringbuf` 函数的默认位置已从 IRAM 更改为 Flash。因此，``CONFIG_RINGBUF_PLACE_FUNCTIONS_INTO_FLASH`` 选项已被移除。此举可节省大量 IRAM，但可能会对性能造成轻微影响。对于性能要求严苛的应用程序，可通过启用新增的 :ref:`CONFIG_RINGBUF_IN_IRAM` 选项来恢复之前的行为。

Log
---

**已移除的函数**

以下已弃用的 Log 函数已在 ESP-IDF v6.0 中移除：

- :cpp:func:`esp_log_buffer_hex` – 请使用 :cpp:func:`ESP_LOG_BUFFER_HEX` 替代。
- :cpp:func:`esp_log_buffer_char` – 请使用 :cpp:func:`ESP_LOG_BUFFER_CHAR` 替代。

**已移除的头文件**

- ``esp_log_internal.h`` – 请使用 ``esp_log_buffer.h`` 替代。

ESP-Event
---------

不必要的 FreeRTOS 头文件已从 ``esp_event.h`` 中移除。此前依赖这些隐式包含的代码，现在必须显式添加以下头文件：``#include "freertos/queue.h"`` 和 ``#include "freertos/semphr.h"``。

核心转储
--------

二进制数据格式已被弃用。`CONFIG_ESP_COREDUMP_DATA_FORMAT_BIN` 表示该功能已完全删除，不再可用。现在默认的数据格式是 ELF。

CRC 数据完整性检查已被弃用。`ESP_COREDUMP_CHECKSUM_CRC32` 表示该功能已完全删除，不再可用。现在默认的校验和算法为 SHA256。

函数 :cpp:func:`esp_core_dump_partition_and_size_get()` 现在对空白（已擦除）分区返回 `ESP_ERR_NOT_FOUND`，而不是 `ESP_ERR_INVALID_SIZE`。

OTA 更新
-----------

ESP HTTPS OTA 的分段下载功能已移至配置选项下，以便在未使用分段下载时减少内存占用。

如果要在 OTA 应用中使用分段下载功能，需要在 menuconfig 中启用组件级配置 :ref:`CONFIG_ESP_HTTPS_OTA_ENABLE_PARTIAL_DOWNLOAD` (``Component config`` → ``ESP HTTPS OTA`` → ``Enable partial HTTP download for OTA``)。

**已移除的废弃 API**

以下废弃函数已从 ``app_update`` 组件中移除：

- :cpp:func:`esp_ota_get_app_description` – 请使用 :cpp:func:`esp_app_get_description` 代替。
- :cpp:func:`esp_ota_get_app_elf_sha256` – 请使用 :cpp:func:`esp_app_get_elf_sha256` 代替。

这些函数已移至 ``esp_app_format`` 组件。请将包含文件从 ``esp_ota_ops.h`` 更新为 ``esp_app_desc.h``，如有需要请将 ``esp_app_format`` 添加到您的组件依赖项中。

Gcov
----

gcov 组件已移至独立仓库。`esp_gcov <https://components.espressif.com/components/espressif/esp_gcov>`_ 现为托管组件。

**组件依赖**

使用 gcov 功能的项目现在必须在 ``idf_component.yml`` 清单文件中添加 esp_gcov 组件作为依赖项：

.. code-block:: yaml

    dependencies:
      espressif/esp_gcov: ^1

**配置更改**

gcov 配置选项已从应用程序级别追踪菜单移至专用的 ``GNU Code Coverage`` 菜单。

``CONFIG_APPTRACE_GCOV_ENABLE`` 选项已重命名为 ``CONFIG_ESP_GCOV_ENABLE``。

**头文件更改**

对于 gcov 功能，请改用 ``esp_gcov.h`` 头文件替代原有的 ``esp_app_trace.h``。

系统控制台 (STDIO)
----------------------

``esp_vfs_cdcacm.h`` 头文件已移至新组件 ``esp_usb_cdc_romconsole`` 中。若需使用该头文件中的任何函数，现在需要显式添加对 ``esp_usb_cdc_rom_console`` 的 ``REQUIRES`` 依赖项。

LibC
------

:ref:`CONFIG_COMPILER_ASSERT_NDEBUG_EVALUATE` 的默认值已改为 `n`。这意味着当设置了 ``NDEBUG`` 时，断言将不再对断言内的表达式进行求值。此更改将默认行为恢复为与 C 语言标准一致。

ULP
---

LP-Core 在深度睡眠期间遇到异常时，将唤醒主 CPU。此功能默认启用，若不需要此行为，可以通过 :ref:`CONFIG_ULP_TRAP_WAKEUP` Kconfig 配置选项禁用。
