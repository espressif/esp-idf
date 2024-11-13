/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_wifi_types.h"
#include "utils/common.h"
#include <sys/time.h>
#ifdef __cplusplus
extern "C" {
#endif

struct roam_config {
    uint8_t backoff_time;
    bool low_rssi_roam_trigger;
    uint8_t low_rssi_threshold;
    uint8_t rssi_threshold_reduction_offset;
    bool scan_monitor;
    uint8_t scan_interval;
    uint8_t scan_rssi_threshold;
    uint8_t scan_rssi_diff;
    bool legacy_roam_enabled;
    uint8_t btm_retry_cnt;
    bool btm_roaming_enabled;
    bool rrm_monitor;
    uint8_t rrm_monitor_time;
    uint8_t rrm_monitor_rssi_threshold;
    wifi_scan_config_t scan_config;
};

void init_roaming_app(void);
void deinit_roaming_app(void);
void roaming_app_disable_reconnect(void);
void roaming_app_enable_reconnect(void);

#ifdef __cplusplus
}
#endif
