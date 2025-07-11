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

**内存布局**

为了减少 IRAM 的使用，大多数 FreeRTOS 函数的默认位置已从 IRAM 更改为 Flash。因此，``CONFIG_FREERTOS_PLACE_FUNCTIONS_INTO_FLASH`` 选项已被移除。此举可节省大量 IRAM，但可能会对性能造成轻微影响。对于性能要求严苛的应用程序，可通过启用新增的 :ref:`CONFIG_FREERTOS_IN_IRAM` 选项来恢复之前的行为。
在决定是否启用 ``CONFIG_FREERTOS_IN_IRAM`` 时，建议进行性能测试以测量对特定用例的实际影响。Flash 和 IRAM 配置之间的性能差异可能因 Flash 缓存效率、API 使用模式和系统负载而异。
在 ``components/freertos/test_apps/freertos/performance/test_freertos_api_performance.c`` 中提供了基准性能测试，用于测量常用 FreeRTOS API 的执行时间。此测试可帮助您评估内存布局对目标硬件和应用程序要求的性能影响。

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

核心转储
--------

二进制数据格式已被弃用。`CONFIG_ESP_COREDUMP_DATA_FORMAT_BIN` 表示该功能已完全删除，不再可用。现在默认的数据格式是 ELF。

CRC 数据完整性检查已被弃用。`ESP_COREDUMP_CHECKSUM_CRC32` 表示该功能已完全删除，不再可用。现在默认的校验和算法为 SHA256。

函数 :cpp:func:`esp_core_dump_partition_and_size_get()` 现在对空白（已擦除）分区返回 `ESP_ERR_NOT_FOUND`，而不是 `ESP_ERR_INVALID_SIZE`。

Gcov
----

gcov 组件已移至独立仓库。`esp_gcov <https://components.espressif.com/components/espressif/esp_gcov>`_ 现为托管组件。

**组件依赖**

使用 gcov 功能的项目现在必须在 ``idf_component.yml`` 清单文件中添加 esp_gcov 组件作为依赖项：

.. code-block:: yaml

    dependencies:
      espressif/esp_gcov: ^1

**配置更改**

gcov 配置选项现在归类在 ``GNU Code Coverage`` 菜单下。

``CONFIG_APPTRACE_GCOV_ENABLE`` 选项已重命名为 ``CONFIG_ESP_GCOV_ENABLE``。

**头文件更改**

对于 gcov 功能，请改用 ``esp_gcov.h`` 头文件替代原有的 ``esp_app_trace.h``。
