/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
// This file is auto-generated
#include "esp_wifi_remote.h"
#include "esp_log.h"

#define WEAK __attribute__((weak))
#define LOG_UNSUPPORTED_AND_RETURN(ret) ESP_LOGW("esp_wifi_remote_weak", "%s unsupported", __func__); \
         return ret;

WEAK esp_err_t esp_wifi_remote_init(const wifi_init_config_t *config)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_deinit(void)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_set_mode(wifi_mode_t mode)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_get_mode(wifi_mode_t *mode)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_start(void)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_stop(void)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_restore(void)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_connect(void)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_disconnect(void)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_clear_fast_connect(void)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_deauth_sta(uint16_t aid)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_scan_start(const wifi_scan_config_t *config, _Bool block)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_set_scan_parameters(const wifi_scan_default_params_t *config)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_get_scan_parameters(wifi_scan_default_params_t *config)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_scan_stop(void)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_scan_get_ap_num(uint16_t *number)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_scan_get_ap_records(uint16_t *number, wifi_ap_record_t *ap_records)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_scan_get_ap_record(wifi_ap_record_t *ap_record)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_clear_ap_list(void)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_sta_get_ap_info(wifi_ap_record_t *ap_info)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_set_ps(wifi_ps_type_t type)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_get_ps(wifi_ps_type_t *type)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_set_protocol(wifi_interface_t ifx, uint8_t protocol_bitmap)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_get_protocol(wifi_interface_t ifx, uint8_t *protocol_bitmap)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_set_bandwidth(wifi_interface_t ifx, wifi_bandwidth_t bw)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_get_bandwidth(wifi_interface_t ifx, wifi_bandwidth_t *bw)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_set_channel(uint8_t primary, wifi_second_chan_t second)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_get_channel(uint8_t *primary, wifi_second_chan_t *second)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_set_country(const wifi_country_t *country)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_get_country(wifi_country_t *country)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_set_mac(wifi_interface_t ifx, const uint8_t mac[6])
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_get_mac(wifi_interface_t ifx, uint8_t mac[6])
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_set_promiscuous_rx_cb(wifi_promiscuous_cb_t cb)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_set_promiscuous(_Bool en)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_get_promiscuous(_Bool *en)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_set_promiscuous_filter(const wifi_promiscuous_filter_t *filter)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_get_promiscuous_filter(wifi_promiscuous_filter_t *filter)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_set_promiscuous_ctrl_filter(const wifi_promiscuous_filter_t *filter)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_get_promiscuous_ctrl_filter(wifi_promiscuous_filter_t *filter)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_set_config(wifi_interface_t interface, wifi_config_t *conf)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_get_config(wifi_interface_t interface, wifi_config_t *conf)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_ap_get_sta_list(wifi_sta_list_t *sta)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_ap_get_sta_aid(const uint8_t mac[6], uint16_t *aid)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_set_storage(wifi_storage_t storage)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_set_vendor_ie(_Bool enable, wifi_vendor_ie_type_t type, wifi_vendor_ie_id_t idx, const void *vnd_ie)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_set_vendor_ie_cb(esp_vendor_ie_cb_t cb, void *ctx)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_set_max_tx_power(int8_t power)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_get_max_tx_power(int8_t *power)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_set_event_mask(uint32_t mask)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_get_event_mask(uint32_t *mask)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_80211_tx(wifi_interface_t ifx, const void *buffer, int len, _Bool en_sys_seq)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_register_80211_tx_cb(esp_wifi_80211_tx_done_cb_t cb)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_set_csi_rx_cb(wifi_csi_cb_t cb, void *ctx)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_set_csi_config(const wifi_csi_config_t *config)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_get_csi_config(wifi_csi_config_t *config)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_set_csi(_Bool en)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK int64_t esp_wifi_remote_get_tsf_time(wifi_interface_t interface)
{
    LOG_UNSUPPORTED_AND_RETURN(-1);
}

WEAK esp_err_t esp_wifi_remote_set_inactive_time(wifi_interface_t ifx, uint16_t sec)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_get_inactive_time(wifi_interface_t ifx, uint16_t *sec)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_statis_dump(uint32_t modules)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_set_rssi_threshold(int32_t rssi)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_ftm_initiate_session(wifi_ftm_initiator_cfg_t *cfg)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_ftm_end_session(void)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_ftm_resp_set_offset(int16_t offset_cm)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_ftm_get_report(wifi_ftm_report_entry_t *report, uint8_t num_entries)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_config_11b_rate(wifi_interface_t ifx, _Bool disable)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_connectionless_module_set_wake_interval(uint16_t wake_interval)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_force_wakeup_acquire(void)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_force_wakeup_release(void)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_set_country_code(const char *country, _Bool ieee80211d_enabled)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_get_country_code(char *country)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_config_80211_tx_rate(wifi_interface_t ifx, wifi_phy_rate_t rate)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_config_80211_tx(wifi_interface_t ifx, wifi_tx_rate_config_t *config)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_disable_pmf_config(wifi_interface_t ifx)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_sta_get_aid(uint16_t *aid)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_sta_get_negotiated_phymode(wifi_phy_mode_t *phymode)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_set_dynamic_cs(_Bool enabled)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_sta_get_rssi(int *rssi)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_set_band(wifi_band_t band)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_get_band(wifi_band_t *band)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_set_band_mode(wifi_band_mode_t band_mode)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_get_band_mode(wifi_band_mode_t *band_mode)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_set_protocols(wifi_interface_t ifx, wifi_protocols_t *protocols)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_get_protocols(wifi_interface_t ifx, wifi_protocols_t *protocols)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_set_bandwidths(wifi_interface_t ifx, wifi_bandwidths_t *bw)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_get_bandwidths(wifi_interface_t ifx, wifi_bandwidths_t *bw)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_action_tx_req(wifi_action_tx_req_t *req)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_remain_on_channel(wifi_roc_req_t *req)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}
