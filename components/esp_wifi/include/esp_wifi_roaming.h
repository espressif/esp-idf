/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2026 Philip Kearney, Nightwhistle Labs
 */

#pragma once

#include <stdbool.h>
#include "sdkconfig.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief What the application has outstanding, for the roaming app's check after a connection.
 *
 * Right after a connection gets its IP address, an application reconnects what it had: it opens sessions
 * again and answers whoever was waiting for it. A roam then drops the connection briefly, and a reply due
 * in that time is lost; the check's scan takes the radio off channel and delays replies. So the check
 * waits while either member is true, and then for 500 ms in which both are false.
 */
typedef struct {
    bool awaiting_reply; /**< A message is out whose reply or acknowledgement has not come (for Matter: an exchange is open) */
    bool settling;       /**< Still restoring, after this connection, what it had before it (for Matter: a stored subscription not yet resumed) */
} wifi_roaming_app_hold_t;

#if CONFIG_ESP_WIFI_ROAMING_JOIN_CHECK || __DOXYGEN__
/**
 * @brief Tell the roaming app what the application has outstanding, for its check after a connection.
 *
 * The check (CONFIG_ESP_WIFI_ROAMING_JOIN_CHECK) scans once after each connection, and roams only to an AP
 * much stronger than the one connected to. It waits while the application reports a reply outstanding or
 * that it is settling, and never roams while a reply is outstanding. Call this from any task whenever
 * either member changes; the roaming app keeps the last report. An application that never calls it is
 * waited for only the 500 ms quiet time, before the scan and before a roam.
 *
 * @note Declared only when CONFIG_ESP_WIFI_ROAMING_JOIN_CHECK is enabled.
 *
 * @param hold What the application has outstanding (copied)
 *
 * @return
 *    - ESP_OK: the report is queued for the Wi-Fi task
 *    - ESP_ERR_INVALID_ARG: hold is NULL
 *    - others: the Wi-Fi task's queue refused it
 */
esp_err_t esp_wifi_roaming_set_app_hold(const wifi_roaming_app_hold_t *hold);
#endif

#ifdef __cplusplus
}
#endif
