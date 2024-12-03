/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

struct roam_config {
    uint8_t backoff_time;
    bool low_rssi_roam_trigger;
    int8_t low_rssi_threshold;
    uint8_t rssi_threshold_reduction_offset;
    bool scan_monitor;
    uint8_t scan_interval;
    int8_t scan_rssi_threshold;
    uint8_t scan_rssi_diff;
    bool legacy_roam_enabled;
    uint8_t btm_retry_cnt;
    bool btm_roaming_enabled;
    bool rrm_monitor;
    uint8_t rrm_monitor_time;
    int8_t rrm_monitor_rssi_threshold;
    wifi_scan_config_t scan_config;
};

void init_roaming_app(void);
void deinit_roaming_app(void);
void roaming_app_disable_reconnect(void);
void roaming_app_enable_reconnect(void);
esp_err_t roam_get_config_params(struct roam_config *config);
esp_err_t roam_set_config_params(struct roam_config *config);

#ifdef __cplusplus
}
#endif
