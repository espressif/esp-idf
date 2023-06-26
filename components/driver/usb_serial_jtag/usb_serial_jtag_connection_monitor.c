/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include "driver/usb_serial_jtag.h"
#include "hal/usb_serial_jtag_ll.h"
#include "esp_private/startup_internal.h"
#include "esp_freertos_hooks.h"
#include "esp_pm.h"
#include "esp_private/rtc_clk.h"
#include "esp_check.h"
#include "sdkconfig.h"

static volatile bool s_usb_serial_jtag_conn_status;
#if CONFIG_USJ_NO_AUTO_LS_ON_CONNECTION
static esp_pm_lock_handle_t s_usb_serial_jtag_pm_lock;
#endif

// FreeRTOS tick interrupt may get delayed in handling, which could lead to no SOF coming in within the period from
// a delayed tick to the following tick. Therefore, when FREERTOS_HZ is comparable to SOF packet frequency, a NO_SOF
// tolerance needs to be given to avoid a false disconnection detection.
#define USJ_DISCONNECT_CONFIRM_PERIOD_MS    3
#define ALLOWED_NO_SOF_TICKS                pdMS_TO_TICKS(USJ_DISCONNECT_CONFIRM_PERIOD_MS)

static uint32_t remaining_allowed_no_sof_ticks;

static __attribute__((unused)) const char *USB_SERIAL_JTAG_CONN_MONITOR_TAG = "usb_serial_jtag";


bool usb_serial_jtag_is_connected(void)
{
    return s_usb_serial_jtag_conn_status;
}

static void IRAM_ATTR usb_serial_jtag_sof_tick_hook(void)
{
    // SOF packet is sent by the HOST every 1ms on a full speed bus
    // Between two consecutive tick hooks, there will be at least 1ms (selectable tick rate range is 1 - 1000Hz)
    // Therefore, SOF intr bit must have be raised at every tick hook if it is connected to a HOST
    // Here, the strategy is: Always assume USB Serial/JTAG is connected until we are sure it is not connected
    // Consider it is disconnected only if SOF intr bit is not raised within (ALLOWED_NO_SOF_TICKS + 1) tick periods
    bool sof_received = (usb_serial_jtag_ll_get_intraw_mask() & USB_SERIAL_JTAG_INTR_SOF);
    usb_serial_jtag_ll_clr_intsts_mask(USB_SERIAL_JTAG_INTR_SOF);
    if (s_usb_serial_jtag_conn_status != sof_received) {
        if (!sof_received) {
            if (remaining_allowed_no_sof_ticks > 0) {
                remaining_allowed_no_sof_ticks--;
            } else {
                // Tolerance credit used up, considered as USJ disconnected
#if CONFIG_USJ_NO_AUTO_LS_ON_CONNECTION
                esp_pm_lock_release(s_usb_serial_jtag_pm_lock);
#endif
                rtc_clk_bbpll_remove_consumer();
                s_usb_serial_jtag_conn_status = false;
            }
        } else {
            // USJ re-connected
#if CONFIG_USJ_NO_AUTO_LS_ON_CONNECTION
            esp_pm_lock_acquire(s_usb_serial_jtag_pm_lock);
#endif
            rtc_clk_bbpll_add_consumer();
            s_usb_serial_jtag_conn_status = true;
            remaining_allowed_no_sof_ticks = ALLOWED_NO_SOF_TICKS;
        }
    }
}

ESP_SYSTEM_INIT_FN(usb_serial_jtag_conn_status_init, BIT(0), 230)
{
#if CONFIG_USJ_NO_AUTO_LS_ON_CONNECTION
    ESP_RETURN_ON_ERROR(esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "usb_serial_jtag", &s_usb_serial_jtag_pm_lock),
                            USB_SERIAL_JTAG_CONN_MONITOR_TAG, "create NO_LIGHT_SLEEP lock failed");
    // We always assume it is connected at first, so acquires the lock to avoid auto light sleep
    esp_pm_lock_acquire(s_usb_serial_jtag_pm_lock);
#endif
    rtc_clk_bbpll_add_consumer();
    s_usb_serial_jtag_conn_status = true;
    remaining_allowed_no_sof_ticks = ALLOWED_NO_SOF_TICKS;

    return esp_register_freertos_tick_hook(usb_serial_jtag_sof_tick_hook);
}

void usb_serial_jtag_connection_monitor_include(void)
{
    // Linker hook function, exists to make the linker examine this file
}
