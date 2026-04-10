/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
// This file is auto-generated
#pragma once
#include "esp_err.h"
#include "esp_wifi.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t esp_wifi_remote_init(const wifi_init_config_t *config);
esp_err_t esp_wifi_remote_deinit(void);
esp_err_t esp_wifi_remote_set_mode(wifi_mode_t mode);
esp_err_t esp_wifi_remote_get_mode(wifi_mode_t *mode);
esp_err_t esp_wifi_remote_start(void);
esp_err_t esp_wifi_remote_stop(void);
esp_err_t esp_wifi_remote_restore(void);
esp_err_t esp_wifi_remote_connect(void);
esp_err_t esp_wifi_remote_disconnect(void);
esp_err_t esp_wifi_remote_clear_fast_connect(void);
esp_err_t esp_wifi_remote_deauth_sta(uint16_t aid);
esp_err_t esp_wifi_remote_scan_start(const wifi_scan_config_t *config, _Bool block);
esp_err_t esp_wifi_remote_set_scan_parameters(const wifi_scan_default_params_t *config);
esp_err_t esp_wifi_remote_get_scan_parameters(wifi_scan_default_params_t *config);
esp_err_t esp_wifi_remote_scan_stop(void);
esp_err_t esp_wifi_remote_scan_get_ap_num(uint16_t *number);
esp_err_t esp_wifi_remote_scan_get_ap_records(uint16_t *number, wifi_ap_record_t *ap_records);
esp_err_t esp_wifi_remote_scan_get_ap_record(wifi_ap_record_t *ap_record);
esp_err_t esp_wifi_remote_clear_ap_list(void);
esp_err_t esp_wifi_remote_sta_get_ap_info(wifi_ap_record_t *ap_info);
esp_err_t esp_wifi_remote_set_ps(wifi_ps_type_t type);
esp_err_t esp_wifi_remote_get_ps(wifi_ps_type_t *type);
esp_err_t esp_wifi_remote_set_protocol(wifi_interface_t ifx, uint8_t protocol_bitmap);
esp_err_t esp_wifi_remote_get_protocol(wifi_interface_t ifx, uint8_t *protocol_bitmap);
esp_err_t esp_wifi_remote_set_bandwidth(wifi_interface_t ifx, wifi_bandwidth_t bw);
esp_err_t esp_wifi_remote_get_bandwidth(wifi_interface_t ifx, wifi_bandwidth_t *bw);
esp_err_t esp_wifi_remote_set_channel(uint8_t primary, wifi_second_chan_t second);
esp_err_t esp_wifi_remote_get_channel(uint8_t *primary, wifi_second_chan_t *second);
esp_err_t esp_wifi_remote_get_home_channel(uint8_t *primary, wifi_second_chan_t *second);
esp_err_t esp_wifi_remote_set_country(const wifi_country_t *country);
esp_err_t esp_wifi_remote_get_country(wifi_country_t *country);
esp_err_t esp_wifi_remote_set_mac(wifi_interface_t ifx, const uint8_t mac[6]);
esp_err_t esp_wifi_remote_get_mac(wifi_interface_t ifx, uint8_t mac[6]);
esp_err_t esp_wifi_remote_set_promiscuous_rx_cb(wifi_promiscuous_cb_t cb);
esp_err_t esp_wifi_remote_set_promiscuous(_Bool en);
esp_err_t esp_wifi_remote_get_promiscuous(_Bool *en);
esp_err_t esp_wifi_remote_set_promiscuous_filter(const wifi_promiscuous_filter_t *filter);
esp_err_t esp_wifi_remote_get_promiscuous_filter(wifi_promiscuous_filter_t *filter);
esp_err_t esp_wifi_remote_set_promiscuous_ctrl_filter(const wifi_promiscuous_filter_t *filter);
esp_err_t esp_wifi_remote_get_promiscuous_ctrl_filter(wifi_promiscuous_filter_t *filter);
esp_err_t esp_wifi_remote_set_config(wifi_interface_t interface, wifi_config_t *conf);
esp_err_t esp_wifi_remote_get_config(wifi_interface_t interface, wifi_config_t *conf);
esp_err_t esp_wifi_remote_ap_get_sta_list(wifi_sta_list_t *sta);
esp_err_t esp_wifi_remote_ap_get_sta_aid(const uint8_t mac[6], uint16_t *aid);
esp_err_t esp_wifi_remote_set_storage(wifi_storage_t storage);
esp_err_t esp_wifi_remote_set_vendor_ie(_Bool enable, wifi_vendor_ie_type_t type, wifi_vendor_ie_id_t idx, const void *vnd_ie);
esp_err_t esp_wifi_remote_set_vendor_ie_cb(esp_vendor_ie_cb_t cb, void *ctx);
esp_err_t esp_wifi_remote_set_max_tx_power(int8_t power);
esp_err_t esp_wifi_remote_get_max_tx_power(int8_t *power);
esp_err_t esp_wifi_remote_set_event_mask(uint32_t mask);
esp_err_t esp_wifi_remote_get_event_mask(uint32_t *mask);
esp_err_t esp_wifi_remote_80211_tx(wifi_interface_t ifx, const void *buffer, int len, _Bool en_sys_seq);
esp_err_t esp_wifi_remote_register_80211_tx_cb(esp_wifi_80211_tx_done_cb_t cb);
esp_err_t esp_wifi_remote_set_csi_rx_cb(wifi_csi_cb_t cb, void *ctx);
esp_err_t esp_wifi_remote_set_csi_config(const wifi_csi_config_t *config);
esp_err_t esp_wifi_remote_get_csi_config(wifi_csi_config_t *config);
esp_err_t esp_wifi_remote_set_csi(_Bool en);
int64_t esp_wifi_remote_get_tsf_time(wifi_interface_t interface);
esp_err_t esp_wifi_remote_set_inactive_time(wifi_interface_t ifx, uint16_t sec);
esp_err_t esp_wifi_remote_get_inactive_time(wifi_interface_t ifx, uint16_t *sec);
esp_err_t esp_wifi_remote_statis_dump(uint32_t modules);
esp_err_t esp_wifi_remote_set_rssi_threshold(int32_t rssi);
esp_err_t esp_wifi_remote_ftm_initiate_session(wifi_ftm_initiator_cfg_t *cfg);
esp_err_t esp_wifi_remote_ftm_end_session(void);
esp_err_t esp_wifi_remote_ftm_resp_set_offset(int16_t offset_cm);
esp_err_t esp_wifi_remote_ftm_get_report(wifi_ftm_report_entry_t *report, uint8_t num_entries);
esp_err_t esp_wifi_remote_config_11b_rate(wifi_interface_t ifx, _Bool disable);
esp_err_t esp_wifi_remote_connectionless_module_set_wake_interval(uint16_t wake_interval);
esp_err_t esp_wifi_remote_force_wakeup_acquire(void);
esp_err_t esp_wifi_remote_force_wakeup_release(void);
esp_err_t esp_wifi_remote_set_country_code(const char *country, _Bool ieee80211d_enabled);
esp_err_t esp_wifi_remote_get_country_code(char *country);
esp_err_t esp_wifi_remote_config_80211_tx_rate(wifi_interface_t ifx, wifi_phy_rate_t rate);
esp_err_t esp_wifi_remote_config_80211_tx(wifi_interface_t ifx, wifi_tx_rate_config_t *config);
esp_err_t esp_wifi_remote_disable_pmf_config(wifi_interface_t ifx);
esp_err_t esp_wifi_remote_sta_get_aid(uint16_t *aid);
esp_err_t esp_wifi_remote_sta_get_negotiated_phymode(wifi_phy_mode_t *phymode);
esp_err_t esp_wifi_remote_set_dynamic_cs(_Bool enabled);
esp_err_t esp_wifi_remote_sta_get_rssi(int *rssi);
esp_err_t esp_wifi_remote_set_band(wifi_band_t band);
esp_err_t esp_wifi_remote_get_band(wifi_band_t *band);
esp_err_t esp_wifi_remote_set_band_mode(wifi_band_mode_t band_mode);
esp_err_t esp_wifi_remote_get_band_mode(wifi_band_mode_t *band_mode);
esp_err_t esp_wifi_remote_set_protocols(wifi_interface_t ifx, wifi_protocols_t *protocols);
esp_err_t esp_wifi_remote_get_protocols(wifi_interface_t ifx, wifi_protocols_t *protocols);
esp_err_t esp_wifi_remote_set_bandwidths(wifi_interface_t ifx, wifi_bandwidths_t *bw);
esp_err_t esp_wifi_remote_get_bandwidths(wifi_interface_t ifx, wifi_bandwidths_t *bw);
esp_err_t esp_wifi_remote_action_tx_req(wifi_action_tx_req_t *req);
esp_err_t esp_wifi_remote_remain_on_channel(wifi_roc_req_t *req);

#ifdef __cplusplus
}
#endif
