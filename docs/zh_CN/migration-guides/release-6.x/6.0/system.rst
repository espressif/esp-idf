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

已弃用的头文件 ``{IDF_TARGET_NAME}/rtc.h`` 已被移除，请改用替代头文件 ``esp_rtc_time.h``。

硬件支持
--------

已弃用的头文件 ``soc_memory_types.h`` 已被移除，请改用替代头文件 ``esp_memory_utils.h``。

App Trace
----------

已移除额外数据缓冲选项。不再支持 `CONFIG_APPTRACE_PENDING_DATA_SIZE_MAX` 配置项。

FreeRTOS
--------

**已移除的函数**

以下已弃用的 FreeRTOS 函数已在 ESP-IDF v6.0 中移除：

- :cpp:func:`xTaskGetAffinity` – 请使用 :cpp:func:`xTaskGetCoreID` 替代。
- :cpp:func:`xTaskGetIdleTaskHandleForCPU` – 请使用 :cpp:func:`xTaskGetIdleTaskHandleForCore` 替代。
- :cpp:func:`xTaskGetCurrentTaskHandleForCPU` – 请使用 :cpp:func:`xTaskGetCurrentTaskHandleForCore` 替代。

**已弃用的函数**

函数 :cpp:func:`pxTaskGetStackStart` 已弃用。请使用 :cpp:func:`xTaskGetStackStart` 替代以提高类型安全性。
