VDDBAT Under-Voltage Protection
===============================

:link_to_translation:`zh_CN:[中文]`

.. contents::
    :local:
    :depth: 2

Overview
--------

Over-discharging a lithium battery causes irreversible capacity loss. On {IDF_TARGET_NAME}, the VDDBAT under-voltage protection driver detects when the VDDBAT voltage drops below a fixed threshold and notifies the application through a callback. The application then handles the event and enters Deep-sleep until the battery is recharged and recovers above the VDDBAT threshold.

.. note::

    The VDDBAT under-voltage protection driver only reports the under-voltage. It does not enter Deep-sleep, stop peripherals, or reset the chip by itself.

Driver Introduction
-------------------

Before using the VDDBAT under-voltage protection driver, you need to understand how the detection is confirmed and what each of the two state functions represents.

.. _vddbat-protect-principle:

Working Principle
^^^^^^^^^^^^^^^^^

Under-voltage protection compares VDDBAT with a threshold of about 2.96 V. A hardware filter debounces the comparator before the driver reports a stable under-voltage:

- While VDDBAT is below the threshold, the filter counts up. While it is above the threshold, the filter counts down.
- Reaching :cpp:member:`vddbat_protect_config_t::under_volt_filter_ticks` confirms the under-voltage, and the driver calls the registered callback.
- Falling back to :cpp:member:`vddbat_protect_config_t::up_volt_filter_ticks` confirms the recovery. That event can wake the chip from Deep-sleep.

The gap between the two values is the filter hysteresis. A larger ``under_volt_filter_ticks`` ignores longer dips, such as those caused by a burst of RF transmission, and reacts more slowly. Use :c:macro:`VDDBAT_PROTECT_DEFAULT_CONFIG` unless the application needs a different filter.

.. _vddbat-protect-state:

Under-Voltage State
^^^^^^^^^^^^^^^^^^^

Two functions return the state, with different meanings:

- :cpp:func:`esp_vddbat_is_under_volt` returns the live under-voltage flag of the hardware filter. It returns ``true`` from the moment an under-voltage is confirmed until the recovery is confirmed. Use it to check whether VDDBAT is currently still low, for example before entering Deep-sleep.
- :cpp:func:`esp_vddbat_get_state` returns the state last observed by the interrupt handler as an :cpp:type:`esp_vddbat_state_t`. It becomes :cpp:enumerator:`ESP_VDDBAT_STATE_UNDER_VOLTAGE` when the callback is called, and returns to :cpp:enumerator:`ESP_VDDBAT_STATE_NORMAL` when :cpp:func:`esp_vddbat_protect_enable` is called. It does not change automatically with the voltage.

Quick Start
-----------

This section shows how to use the VDDBAT under-voltage protection driver. Typical usage flow: select the under-voltage protection in menuconfig, then install the driver, register the callback, and enable detection. When an under-voltage occurs, enter Deep-sleep from a task and wake up once VDDBAT recovers.

.. _vddbat-protect-kconfig:

Configure Protection Mode
^^^^^^^^^^^^^^^^^^^^^^^^^

Select :menuitem:`CONFIG_ESP_VDDBAT_UNDER_VOLT_PROTECT` instead of the default :menuitem:`CONFIG_ESP_BROWNOUT_DET`. The chip is then not reset when VDDBAT drops, and this driver reports the under-voltage. This also disables the brownout configuration done by the bootloader.

.. _vddbat-protect-resource:

Install Driver
^^^^^^^^^^^^^^

Install the protection with :cpp:func:`esp_vddbat_protect_install` and :c:macro:`VDDBAT_PROTECT_DEFAULT_CONFIG`. The filter fields of :cpp:type:`vddbat_protect_config_t` are described in the API reference.

Only one handle can exist at a time. Installing again before uninstalling returns :c:macro:`ESP_ERR_INVALID_STATE`.

Installing configures the filter, allocates the interrupt, and removes VDDBAT_VGOOD from the analog reset path. Detection is not armed yet, see :ref:`vddbat-protect-enable`.

.. code:: c

    vddbat_protect_config_t config = VDDBAT_PROTECT_DEFAULT_CONFIG();
    vddbat_protect_handle_t protect = NULL;
    ESP_ERROR_CHECK(esp_vddbat_protect_install(&config, &protect));

.. _vddbat-protect-callback:

Register Event Callbacks
^^^^^^^^^^^^^^^^^^^^^^^^

Before enabling the protection, call :cpp:func:`esp_vddbat_protect_register_callbacks` to register the callback. The callbacks are grouped in :cpp:type:`vddbat_protect_event_callbacks_t`:

- :cpp:member:`vddbat_protect_event_callbacks_t::on_under_volt`: called once an under-voltage is confirmed. The prototype is :cpp:type:`esp_vddbat_under_volt_cb_t`.

The callback runs in the interrupt handler, so it must not block, and it must not enter Deep-sleep directly. A typical callback only wakes up a task that performs the shutdown flow, and returns whether a higher priority task was woken. The ``user_arg`` passed at registration is passed back to the callback.

Detection stays enabled after the callback returns. Call :cpp:func:`esp_vddbat_protect_disable` from the callback to stop further reports until the application has handled this one.

.. code:: c

    static IRAM_ATTR bool under_volt_isr_cb(vddbat_protect_handle_t handle, void *user_arg)
    {
        BaseType_t task_woken = pdFALSE;

        // Stop further reports until the task has handled this one
        esp_vddbat_protect_disable(handle);
        xSemaphoreGiveFromISR((SemaphoreHandle_t)user_arg, &task_woken);
        return task_woken == pdTRUE;
    }

    vddbat_protect_event_callbacks_t cbs = {
        .on_under_volt = under_volt_isr_cb,
    };
    ESP_ERROR_CHECK(esp_vddbat_protect_register_callbacks(protect, &cbs, under_volt_sem));

.. _vddbat-protect-enable:

Enable and Disable Detection
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

:cpp:func:`esp_vddbat_protect_enable` clears the filter counter and pending events, and then arms the under-voltage detection. A callback must be registered first, otherwise :c:macro:`ESP_ERR_INVALID_STATE` is returned.

The protection is not enabled automatically during startup. Because there is no under-voltage protection from chip boot until :cpp:func:`esp_vddbat_protect_enable` is called, install and enable the protection early in the application. If VDDBAT is already below the threshold when the protection is enabled, the callback is called as soon as the filter confirms the under-voltage.

:cpp:func:`esp_vddbat_protect_disable` stops the detection. The interrupt handler, the handle, and the callback stay in place, so :cpp:func:`esp_vddbat_protect_enable` can be called again later to re-enable detection, for example after VDDBAT has recovered.

.. code:: c

    ESP_ERROR_CHECK(esp_vddbat_protect_enable(protect));

.. _vddbat-protect-deep-sleep:

Enter Deep-sleep
^^^^^^^^^^^^^^^^

When an under-voltage occurs, the chip should enter Deep-sleep to save power and protect the battery, and wake up once the battery is recharged above the threshold. The recovery event of the filter is available as the Deep-sleep wakeup source :cpp:enumerator:`ESP_SLEEP_WAKEUP_VDDBAT_UP_VOLT`.

Perform the following steps in a task, not in the callback:

1. Call :cpp:func:`esp_vddbat_prepare_deep_sleep`. This function enables the VDDBAT recovery wakeup and clears the under-voltage event. If VDDBAT has already recovered, it disables the wakeup source and returns :c:macro:`ESP_ERR_INVALID_STATE`. In that case, do not enter Deep-sleep, because there will be no recovery event left to wake the chip.
2. Finish any other pre-sleep work, such as saving data or stopping flash writes.
3. Call :cpp:func:`esp_deep_sleep_try_to_start`. If a wakeup event (including VDDBAT recovery) arrives while the chip is entering Deep-sleep, the sleep is rejected and the function returns.
4. If the function returns, disable the VDDBAT wakeup source with :cpp:func:`esp_sleep_disable_wakeup_source` and check :cpp:func:`esp_vddbat_is_under_volt` again. Retry if VDDBAT is still low.

.. code:: c

    static void under_volt_handler(vddbat_protect_handle_t protect)
    {
        while (esp_vddbat_is_under_volt(protect)) {
            if (esp_vddbat_prepare_deep_sleep(protect) != ESP_OK) {
                break;  // VDDBAT already recovered
            }
            esp_deep_sleep_try_to_start();
            // Only reached if the sleep was rejected
            esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_VDDBAT_UP_VOLT);
            if (!esp_vddbat_is_under_volt(protect)) {
                break;
            }
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        // Disabled in the callback, arm it again now that VDDBAT recovered
        ESP_ERROR_CHECK(esp_vddbat_protect_enable(protect));
    }

When the chip wakes up from Deep-sleep, it reboots. If the wakeup was caused by VDDBAT recovery, the return value of :cpp:func:`esp_sleep_get_wakeup_causes` contains ``BIT(ESP_SLEEP_WAKEUP_VDDBAT_UP_VOLT)``.

Uninstall Driver
^^^^^^^^^^^^^^^^

:cpp:func:`esp_vddbat_protect_uninstall` frees the interrupt and the handle, and reconnects VDDBAT_VGOOD to the analog reset path. The protection must be disabled first, otherwise :c:macro:`ESP_ERR_INVALID_STATE` is returned.

.. code:: c

    ESP_ERROR_CHECK(esp_vddbat_protect_disable(protect));
    ESP_ERROR_CHECK(esp_vddbat_protect_uninstall(protect));

Application Example
^^^^^^^^^^^^^^^^^^^

- :example:`lowpower/vddbat` installs the VDDBAT under-voltage protection, enters Deep-sleep from a task once an under-voltage is reported, and wakes up the chip when VDDBAT recovers.

Advanced
--------

.. _vddbat-protect-thread-safety:

Thread Safety
^^^^^^^^^^^^^

:cpp:func:`esp_vddbat_protect_disable` can be called from the under-voltage callback. All other functions must be called from task context.

The driver does not provide concurrency protection across tasks; avoid calling its functions concurrently from different tasks.

.. _vddbat-protect-limitations:

Limitations
^^^^^^^^^^^

- The lithium battery protection detector shares the same comparator with the brownout detector, so the two cannot be enabled at the same time. This means lithium battery and other power supply types cannot share a single firmware binary in production.
- The brownout reset and the VDDBAT under-voltage protection are mutually exclusive. With the VDDBAT protection selected, the chip is no longer reset by hardware when VDDBAT drops, so the application is responsible for handling the under-voltage.
- There is no under-voltage protection between chip boot and :cpp:func:`esp_vddbat_protect_enable`.

API Reference
-------------

.. include-build-file:: inc/esp_vddbat.inc
