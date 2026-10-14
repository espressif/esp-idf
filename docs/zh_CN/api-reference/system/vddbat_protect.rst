VDDBAT 锂电池欠压保护
========================

:link_to_translation:`en:[English]`

.. contents::
    :local:
    :depth: 2

概述
----

锂电池过度放电会造成不可逆的容量损失。在 {IDF_TARGET_NAME} 上，VDDBAT 欠压保护驱动会在 VDDBAT 电压低于固定阈值时检测到欠压，并通过回调函数通知应用程序。之后由应用程序处理相关事件并进入 Deep-sleep，直到电池重新充电并恢复至 VDDBAT 阈值以上。

.. note::

    VDDBAT 欠压保护驱动只负责报告欠压，不会自行进入 Deep-sleep、关闭外设或复位芯片。

驱动介绍
--------

在使用 VDDBAT 欠压保护驱动之前，需要先了解检测是如何确认的，以及两个状态接口各自表示什么。

.. _vddbat-protect-principle:

工作原理
^^^^^^^^

欠压保护将 VDDBAT 与约 2.96 V 的阈值进行比较。比较器输出先经过硬件滤波器去抖，驱动再报告稳定欠压：

- VDDBAT 低于阈值时，滤波器向上计数；高于阈值时，向下计数。
- 计到 :cpp:member:`vddbat_protect_config_t::under_volt_filter_ticks` 时确认欠压，驱动调用已注册的回调函数。
- 回落到 :cpp:member:`vddbat_protect_config_t::up_volt_filter_ticks` 时确认恢复。该事件可以把芯片从 Deep-sleep 中唤醒。

两个数值的差值就是滤波器的迟滞。``under_volt_filter_ticks`` 越大，能忽略的跌落越长（例如 RF 突发发射），响应也越慢。没有特殊滤波需求时使用 :c:macro:`VDDBAT_PROTECT_DEFAULT_CONFIG`。

.. _vddbat-protect-state:

欠压状态
^^^^^^^^

以下两个函数都返回状态，但含义不同：

- :cpp:func:`esp_vddbat_is_under_volt` 返回硬件滤波器的实时欠压标志。从确认欠压开始到确认恢复为止，该函数返回 ``true``。可用于判断 VDDBAT 当前是否仍然偏低，例如在进入 Deep-sleep 之前。
- :cpp:func:`esp_vddbat_get_state` 以 :cpp:type:`esp_vddbat_state_t` 返回中断处理程序最近一次观察到的状态。调用回调函数时，该状态变为 :cpp:enumerator:`ESP_VDDBAT_STATE_UNDER_VOLTAGE`，调用 :cpp:func:`esp_vddbat_protect_enable` 时恢复为 :cpp:enumerator:`ESP_VDDBAT_STATE_NORMAL`。该状态不会随电压自动变化。

快速入门
--------

本节将带你快速了解如何使用 VDDBAT 欠压保护驱动。一般的使用流程如下：先在 menuconfig 中选择欠压保护，再安装驱动、注册回调并开启检测。发生欠压后，由任务进入 Deep-sleep，等 VDDBAT 恢复后唤醒。

.. _vddbat-protect-kconfig:

配置保护方式
^^^^^^^^^^^^

选择 :menuitem:`CONFIG_ESP_VDDBAT_UNDER_VOLT_PROTECT`，不要使用默认的 :menuitem:`CONFIG_ESP_BROWNOUT_DET`。此后 VDDBAT 跌落时芯片不再复位，改由本驱动报告欠压。该选项同时关闭 bootloader 中的掉电检测配置。

.. _vddbat-protect-resource:

安装驱动
^^^^^^^^

使用 :cpp:func:`esp_vddbat_protect_install` 和 :c:macro:`VDDBAT_PROTECT_DEFAULT_CONFIG` 安装保护。:cpp:type:`vddbat_protect_config_t` 中滤波字段的取值见 API 参考。

同一时间只能存在一个句柄。卸载前再次安装会返回 :c:macro:`ESP_ERR_INVALID_STATE`。

安装会配置滤波器、分配中断，并将 VDDBAT_VGOOD 从模拟复位通路中移除。此时检测尚未开启，见 :ref:`vddbat-protect-enable`。

.. code:: c

    vddbat_protect_config_t config = VDDBAT_PROTECT_DEFAULT_CONFIG();
    vddbat_protect_handle_t protect = NULL;
    ESP_ERROR_CHECK(esp_vddbat_protect_install(&config, &protect));

.. _vddbat-protect-callback:

注册事件回调
^^^^^^^^^^^^

在开启保护之前，调用 :cpp:func:`esp_vddbat_protect_register_callbacks` 注册回调函数。回调函数集中在 :cpp:type:`vddbat_protect_event_callbacks_t` 中：

- :cpp:member:`vddbat_protect_event_callbacks_t::on_under_volt`：确认欠压后调用，函数原型为 :cpp:type:`esp_vddbat_under_volt_cb_t`。

回调函数在中断处理程序中运行，因此不能阻塞，也不能直接进入 Deep-sleep。通常回调函数只负责唤醒执行关机流程的任务，并返回是否唤醒了更高优先级的任务。注册时传入的 ``user_arg`` 会传回给回调函数。

回调函数返回后，检测仍保持开启。可以在回调函数中调用 :cpp:func:`esp_vddbat_protect_disable`，在应用程序处理完本次欠压之前停止后续上报。

.. code:: c

    static IRAM_ATTR bool under_volt_isr_cb(vddbat_protect_handle_t handle, void *user_arg)
    {
        BaseType_t task_woken = pdFALSE;

        // 在任务处理完本次欠压之前停止后续上报
        esp_vddbat_protect_disable(handle);
        xSemaphoreGiveFromISR((SemaphoreHandle_t)user_arg, &task_woken);
        return task_woken == pdTRUE;
    }

    vddbat_protect_event_callbacks_t cbs = {
        .on_under_volt = under_volt_isr_cb,
    };
    ESP_ERROR_CHECK(esp_vddbat_protect_register_callbacks(protect, &cbs, under_volt_sem));

.. _vddbat-protect-enable:

开启和关闭检测
^^^^^^^^^^^^^^

:cpp:func:`esp_vddbat_protect_enable` 会清零滤波计数器和待处理事件，然后开启欠压检测。调用前必须已注册回调函数，否则返回 :c:macro:`ESP_ERR_INVALID_STATE`。

启动过程中不会自动开启保护。从芯片启动到调用 :cpp:func:`esp_vddbat_protect_enable` 之前，没有任何欠压保护，因此请在应用程序中尽早安装并开启保护。如果开启保护时 VDDBAT 已经低于阈值，滤波器确认欠压后会立即调用回调函数。

:cpp:func:`esp_vddbat_protect_disable` 停止检测。中断处理程序、句柄和回调函数仍然保留，因此之后可以再次调用 :cpp:func:`esp_vddbat_protect_enable` 开启检测，例如在 VDDBAT 恢复之后。

.. code:: c

    ESP_ERROR_CHECK(esp_vddbat_protect_enable(protect));

.. _vddbat-protect-deep-sleep:

进入 Deep-sleep
^^^^^^^^^^^^^^^

发生欠压时，芯片需要进入 Deep-sleep 状态以节省功耗保护电池，等电池重新充电到阈值以上后再唤醒。滤波器的恢复事件可用作 Deep-sleep 唤醒源 :cpp:enumerator:`ESP_SLEEP_WAKEUP_VDDBAT_UP_VOLT`。

请在任务中完成以下步骤，不要在回调函数中执行：

1. 调用 :cpp:func:`esp_vddbat_prepare_deep_sleep`。该函数会开启 VDDBAT 恢复唤醒并清除欠压事件。如果 VDDBAT 已经恢复，该函数会关闭该唤醒源并返回 :c:macro:`ESP_ERR_INVALID_STATE`。此时不要进入 Deep-sleep，因为已经不会再有恢复事件来唤醒芯片。
2. 完成其他睡眠前的工作，例如保存数据或停止 flash 写入。
3. 调用 :cpp:func:`esp_deep_sleep_try_to_start`。如果芯片在进入 Deep-sleep 的过程中收到唤醒事件（包括 VDDBAT 恢复），本次睡眠会被拒绝，函数返回。
4. 如果函数返回，调用 :cpp:func:`esp_sleep_disable_wakeup_source` 关闭 VDDBAT 唤醒源，并再次检查 :cpp:func:`esp_vddbat_is_under_volt`。如果 VDDBAT 仍然偏低，则重试。

.. code:: c

    static void under_volt_handler(vddbat_protect_handle_t protect)
    {
        while (esp_vddbat_is_under_volt(protect)) {
            if (esp_vddbat_prepare_deep_sleep(protect) != ESP_OK) {
                break;  // VDDBAT 已恢复
            }
            esp_deep_sleep_try_to_start();
            // 仅当睡眠被拒绝时才会执行到这里
            esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_VDDBAT_UP_VOLT);
            if (!esp_vddbat_is_under_volt(protect)) {
                break;
            }
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        // 回调中已关闭检测，VDDBAT 恢复后重新开启
        ESP_ERROR_CHECK(esp_vddbat_protect_enable(protect));
    }

芯片从 Deep-sleep 唤醒后会重新启动。如果是 VDDBAT 恢复导致的唤醒，:cpp:func:`esp_sleep_get_wakeup_causes` 的返回值中包含 ``BIT(ESP_SLEEP_WAKEUP_VDDBAT_UP_VOLT)``。

卸载驱动
^^^^^^^^

:cpp:func:`esp_vddbat_protect_uninstall` 释放中断和句柄，并将 VDDBAT_VGOOD 重新接回模拟复位通路。调用前必须先关闭保护，否则返回 :c:macro:`ESP_ERR_INVALID_STATE`。

.. code:: c

    ESP_ERROR_CHECK(esp_vddbat_protect_disable(protect));
    ESP_ERROR_CHECK(esp_vddbat_protect_uninstall(protect));

应用示例
^^^^^^^^

- :example:`lowpower/vddbat` 安装 VDDBAT 欠压保护，在上报欠压后由任务进入 Deep-sleep，并在 VDDBAT 恢复后唤醒芯片。

高级功能
--------

.. _vddbat-protect-thread-safety:

线程安全
^^^^^^^^

:cpp:func:`esp_vddbat_protect_disable` 可以在欠压回调函数中调用，其他函数都必须在任务上下文中调用。

驱动未做多任务并发保护，请避免在不同任务中同时调用相关函数。

.. _vddbat-protect-limitations:

使用限制
^^^^^^^^

- 锂电池保护检测器与掉电检测器（brownout detector）共用同一比较器，因此两者不能同时开启。这意味着在生产时锂电池和其它类型的供电电源不可共享一套固件代码。
- 掉电复位和 VDDBAT 欠压保护互斥。选择 VDDBAT 欠压保护后，VDDBAT 跌落时硬件不再复位芯片，需要由应用程序负责处理欠压。
- 从芯片启动到调用 :cpp:func:`esp_vddbat_protect_enable` 之前没有欠压保护。

API 参考
--------

.. include-build-file:: inc/esp_vddbat.inc
