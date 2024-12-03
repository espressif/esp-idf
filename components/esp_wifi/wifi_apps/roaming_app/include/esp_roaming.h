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

void roam_init_app(void);
void roam_deinit_app(void);
void roam_disable_reconnect(void);
void roam_enable_reconnect(void);
void roam_sta_connected(void);
void roam_sta_disconnected(void *disconn);
esp_err_t roam_get_config_params(struct roam_config *config);
esp_err_t roam_set_config_params(struct roam_config *config);

#ifdef __cplusplus
}
#endif
