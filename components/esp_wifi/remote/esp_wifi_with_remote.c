/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
// This file is auto-generated
#include "esp_wifi.h"
#include "esp_wifi_remote.h"

esp_err_t esp_wifi_init(const wifi_init_config_t *config)
{
    return esp_wifi_remote_init(config);
}

esp_err_t esp_wifi_deinit(void)
{
    return esp_wifi_remote_deinit();
}

esp_err_t esp_wifi_set_mode(wifi_mode_t mode)
{
    return esp_wifi_remote_set_mode(mode);
}

esp_err_t esp_wifi_get_mode(wifi_mode_t *mode)
{
    return esp_wifi_remote_get_mode(mode);
}

esp_err_t esp_wifi_start(void)
{
    return esp_wifi_remote_start();
}

esp_err_t esp_wifi_stop(void)
{
    return esp_wifi_remote_stop();
}

esp_err_t esp_wifi_restore(void)
{
    return esp_wifi_remote_restore();
}

esp_err_t esp_wifi_connect(void)
{
    return esp_wifi_remote_connect();
}

esp_err_t esp_wifi_disconnect(void)
{
    return esp_wifi_remote_disconnect();
}

esp_err_t esp_wifi_clear_fast_connect(void)
{
    return esp_wifi_remote_clear_fast_connect();
}

esp_err_t esp_wifi_deauth_sta(uint16_t aid)
{
    return esp_wifi_remote_deauth_sta(aid);
}

esp_err_t esp_wifi_scan_start(const wifi_scan_config_t *config, _Bool block)
{
    return esp_wifi_remote_scan_start(config, block);
}

esp_err_t esp_wifi_set_scan_parameters(const wifi_scan_default_params_t *config)
{
    return esp_wifi_remote_set_scan_parameters(config);
}

esp_err_t esp_wifi_get_scan_parameters(wifi_scan_default_params_t *config)
{
    return esp_wifi_remote_get_scan_parameters(config);
}

esp_err_t esp_wifi_scan_stop(void)
{
    return esp_wifi_remote_scan_stop();
}

esp_err_t esp_wifi_scan_get_ap_num(uint16_t *number)
{
    return esp_wifi_remote_scan_get_ap_num(number);
}

esp_err_t esp_wifi_scan_get_ap_records(uint16_t *number, wifi_ap_record_t *ap_records)
{
    return esp_wifi_remote_scan_get_ap_records(number, ap_records);
}

esp_err_t esp_wifi_scan_get_ap_record(wifi_ap_record_t *ap_record)
{
    return esp_wifi_remote_scan_get_ap_record(ap_record);
}

esp_err_t esp_wifi_clear_ap_list(void)
{
    return esp_wifi_remote_clear_ap_list();
}

esp_err_t esp_wifi_sta_get_ap_info(wifi_ap_record_t *ap_info)
{
    return esp_wifi_remote_sta_get_ap_info(ap_info);
}

esp_err_t esp_wifi_set_ps(wifi_ps_type_t type)
{
    return esp_wifi_remote_set_ps(type);
}

esp_err_t esp_wifi_get_ps(wifi_ps_type_t *type)
{
    return esp_wifi_remote_get_ps(type);
}

esp_err_t esp_wifi_set_protocol(wifi_interface_t ifx, uint8_t protocol_bitmap)
{
    return esp_wifi_remote_set_protocol(ifx, protocol_bitmap);
}

esp_err_t esp_wifi_get_protocol(wifi_interface_t ifx, uint8_t *protocol_bitmap)
{
    return esp_wifi_remote_get_protocol(ifx, protocol_bitmap);
}

esp_err_t esp_wifi_set_bandwidth(wifi_interface_t ifx, wifi_bandwidth_t bw)
{
    return esp_wifi_remote_set_bandwidth(ifx, bw);
}

esp_err_t esp_wifi_get_bandwidth(wifi_interface_t ifx, wifi_bandwidth_t *bw)
{
    return esp_wifi_remote_get_bandwidth(ifx, bw);
}

esp_err_t esp_wifi_set_channel(uint8_t primary, wifi_second_chan_t second)
{
    return esp_wifi_remote_set_channel(primary, second);
}

esp_err_t esp_wifi_get_channel(uint8_t *primary, wifi_second_chan_t *second)
{
    return esp_wifi_remote_get_channel(primary, second);
}

esp_err_t esp_wifi_set_country(const wifi_country_t *country)
{
    return esp_wifi_remote_set_country(country);
}

esp_err_t esp_wifi_get_country(wifi_country_t *country)
{
    return esp_wifi_remote_get_country(country);
}

esp_err_t esp_wifi_set_mac(wifi_interface_t ifx, const uint8_t mac[6])
{
    return esp_wifi_remote_set_mac(ifx, mac);
}

esp_err_t esp_wifi_get_mac(wifi_interface_t ifx, uint8_t mac[6])
{
    return esp_wifi_remote_get_mac(ifx, mac);
}

esp_err_t esp_wifi_set_promiscuous_rx_cb(wifi_promiscuous_cb_t cb)
{
    return esp_wifi_remote_set_promiscuous_rx_cb(cb);
}

esp_err_t esp_wifi_set_promiscuous(_Bool en)
{
    return esp_wifi_remote_set_promiscuous(en);
}

esp_err_t esp_wifi_get_promiscuous(_Bool *en)
{
    return esp_wifi_remote_get_promiscuous(en);
}

esp_err_t esp_wifi_set_promiscuous_filter(const wifi_promiscuous_filter_t *filter)
{
    return esp_wifi_remote_set_promiscuous_filter(filter);
}

esp_err_t esp_wifi_get_promiscuous_filter(wifi_promiscuous_filter_t *filter)
{
    return esp_wifi_remote_get_promiscuous_filter(filter);
}

esp_err_t esp_wifi_set_promiscuous_ctrl_filter(const wifi_promiscuous_filter_t *filter)
{
    return esp_wifi_remote_set_promiscuous_ctrl_filter(filter);
}

esp_err_t esp_wifi_get_promiscuous_ctrl_filter(wifi_promiscuous_filter_t *filter)
{
    return esp_wifi_remote_get_promiscuous_ctrl_filter(filter);
}

esp_err_t esp_wifi_set_config(wifi_interface_t interface, wifi_config_t *conf)
{
    return esp_wifi_remote_set_config(interface, conf);
}

esp_err_t esp_wifi_get_config(wifi_interface_t interface, wifi_config_t *conf)
{
    return esp_wifi_remote_get_config(interface, conf);
}

esp_err_t esp_wifi_ap_get_sta_list(wifi_sta_list_t *sta)
{
    return esp_wifi_remote_ap_get_sta_list(sta);
}

esp_err_t esp_wifi_ap_get_sta_aid(const uint8_t mac[6], uint16_t *aid)
{
    return esp_wifi_remote_ap_get_sta_aid(mac, aid);
}

esp_err_t esp_wifi_set_storage(wifi_storage_t storage)
{
    return esp_wifi_remote_set_storage(storage);
}

esp_err_t esp_wifi_set_vendor_ie(_Bool enable, wifi_vendor_ie_type_t type, wifi_vendor_ie_id_t idx, const void *vnd_ie)
{
    return esp_wifi_remote_set_vendor_ie(enable, type, idx, vnd_ie);
}

esp_err_t esp_wifi_set_vendor_ie_cb(esp_vendor_ie_cb_t cb, void *ctx)
{
    return esp_wifi_remote_set_vendor_ie_cb(cb, ctx);
}

esp_err_t esp_wifi_set_max_tx_power(int8_t power)
{
    return esp_wifi_remote_set_max_tx_power(power);
}

esp_err_t esp_wifi_get_max_tx_power(int8_t *power)
{
    return esp_wifi_remote_get_max_tx_power(power);
}

esp_err_t esp_wifi_set_event_mask(uint32_t mask)
{
    return esp_wifi_remote_set_event_mask(mask);
}

esp_err_t esp_wifi_get_event_mask(uint32_t *mask)
{
    return esp_wifi_remote_get_event_mask(mask);
}

esp_err_t esp_wifi_80211_tx(wifi_interface_t ifx, const void *buffer, int len, _Bool en_sys_seq)
{
    return esp_wifi_remote_80211_tx(ifx, buffer, len, en_sys_seq);
}

esp_err_t esp_wifi_register_80211_tx_cb(esp_wifi_80211_tx_done_cb_t cb)
{
    return esp_wifi_remote_register_80211_tx_cb(cb);
}

esp_err_t esp_wifi_set_csi_rx_cb(wifi_csi_cb_t cb, void *ctx)
{
    return esp_wifi_remote_set_csi_rx_cb(cb, ctx);
}

esp_err_t esp_wifi_set_csi_config(const wifi_csi_config_t *config)
{
    return esp_wifi_remote_set_csi_config(config);
}

esp_err_t esp_wifi_get_csi_config(wifi_csi_config_t *config)
{
    return esp_wifi_remote_get_csi_config(config);
}

esp_err_t esp_wifi_set_csi(_Bool en)
{
    return esp_wifi_remote_set_csi(en);
}

int64_t esp_wifi_get_tsf_time(wifi_interface_t interface)
{
    return esp_wifi_remote_get_tsf_time(interface);
}

esp_err_t esp_wifi_set_inactive_time(wifi_interface_t ifx, uint16_t sec)
{
    return esp_wifi_remote_set_inactive_time(ifx, sec);
}

esp_err_t esp_wifi_get_inactive_time(wifi_interface_t ifx, uint16_t *sec)
{
    return esp_wifi_remote_get_inactive_time(ifx, sec);
}

esp_err_t esp_wifi_statis_dump(uint32_t modules)
{
    return esp_wifi_remote_statis_dump(modules);
}

esp_err_t esp_wifi_set_rssi_threshold(int32_t rssi)
{
    return esp_wifi_remote_set_rssi_threshold(rssi);
}

esp_err_t esp_wifi_ftm_initiate_session(wifi_ftm_initiator_cfg_t *cfg)
{
    return esp_wifi_remote_ftm_initiate_session(cfg);
}

esp_err_t esp_wifi_ftm_end_session(void)
{
    return esp_wifi_remote_ftm_end_session();
}

esp_err_t esp_wifi_ftm_resp_set_offset(int16_t offset_cm)
{
    return esp_wifi_remote_ftm_resp_set_offset(offset_cm);
}

esp_err_t esp_wifi_ftm_get_report(wifi_ftm_report_entry_t *report, uint8_t num_entries)
{
    return esp_wifi_remote_ftm_get_report(report, num_entries);
}

esp_err_t esp_wifi_config_11b_rate(wifi_interface_t ifx, _Bool disable)
{
    return esp_wifi_remote_config_11b_rate(ifx, disable);
}

esp_err_t esp_wifi_connectionless_module_set_wake_interval(uint16_t wake_interval)
{
    return esp_wifi_remote_connectionless_module_set_wake_interval(wake_interval);
}

esp_err_t esp_wifi_force_wakeup_acquire(void)
{
    return esp_wifi_remote_force_wakeup_acquire();
}

esp_err_t esp_wifi_force_wakeup_release(void)
{
    return esp_wifi_remote_force_wakeup_release();
}

esp_err_t esp_wifi_set_country_code(const char *country, _Bool ieee80211d_enabled)
{
    return esp_wifi_remote_set_country_code(country, ieee80211d_enabled);
}

esp_err_t esp_wifi_get_country_code(char *country)
{
    return esp_wifi_remote_get_country_code(country);
}

esp_err_t esp_wifi_config_80211_tx_rate(wifi_interface_t ifx, wifi_phy_rate_t rate)
{
    return esp_wifi_remote_config_80211_tx_rate(ifx, rate);
}

esp_err_t esp_wifi_config_80211_tx(wifi_interface_t ifx, wifi_tx_rate_config_t *config)
{
    return esp_wifi_remote_config_80211_tx(ifx, config);
}

esp_err_t esp_wifi_disable_pmf_config(wifi_interface_t ifx)
{
    return esp_wifi_remote_disable_pmf_config(ifx);
}

esp_err_t esp_wifi_sta_get_aid(uint16_t *aid)
{
    return esp_wifi_remote_sta_get_aid(aid);
}

esp_err_t esp_wifi_sta_get_negotiated_phymode(wifi_phy_mode_t *phymode)
{
    return esp_wifi_remote_sta_get_negotiated_phymode(phymode);
}

esp_err_t esp_wifi_set_dynamic_cs(_Bool enabled)
{
    return esp_wifi_remote_set_dynamic_cs(enabled);
}

esp_err_t esp_wifi_sta_get_rssi(int *rssi)
{
    return esp_wifi_remote_sta_get_rssi(rssi);
}

esp_err_t esp_wifi_set_band(wifi_band_t band)
{
    return esp_wifi_remote_set_band(band);
}

esp_err_t esp_wifi_get_band(wifi_band_t *band)
{
    return esp_wifi_remote_get_band(band);
}

esp_err_t esp_wifi_set_band_mode(wifi_band_mode_t band_mode)
{
    return esp_wifi_remote_set_band_mode(band_mode);
}

esp_err_t esp_wifi_get_band_mode(wifi_band_mode_t *band_mode)
{
    return esp_wifi_remote_get_band_mode(band_mode);
}

esp_err_t esp_wifi_set_protocols(wifi_interface_t ifx, wifi_protocols_t *protocols)
{
    return esp_wifi_remote_set_protocols(ifx, protocols);
}

esp_err_t esp_wifi_get_protocols(wifi_interface_t ifx, wifi_protocols_t *protocols)
{
    return esp_wifi_remote_get_protocols(ifx, protocols);
}

esp_err_t esp_wifi_set_bandwidths(wifi_interface_t ifx, wifi_bandwidths_t *bw)
{
    return esp_wifi_remote_set_bandwidths(ifx, bw);
}

esp_err_t esp_wifi_get_bandwidths(wifi_interface_t ifx, wifi_bandwidths_t *bw)
{
    return esp_wifi_remote_get_bandwidths(ifx, bw);
}

esp_err_t esp_wifi_action_tx_req(wifi_action_tx_req_t *req)
{
    return esp_wifi_remote_action_tx_req(req);
}

esp_err_t esp_wifi_remain_on_channel(wifi_roc_req_t *req)
{
    return esp_wifi_remote_remain_on_channel(req);
}
