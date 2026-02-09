/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _BLE_DUMMY_H_
#define _BLE_DUMMY_H_

/* BLE LL ADV */
#if !DEFAULT_BT_LE_ROLE_BROADCASTER
int r_ble_ll_adv_env_init(void){return 0;}
int r_ble_ll_adv_env_init_eco4(void){return 0;}
void r_ble_ll_adv_scan_req_rxd(void){}
void r_ble_ll_adv_scan_req_rxd_eco4(void){}
void r_ble_ll_adv_status_check(void){}
void r_ble_ll_adv_status_check_eco4(void){}
void r_ble_ll_adv_set_adv_params(void){}
void r_ble_ll_adv_set_adv_params_eco4(void){}
void r_ble_ll_adv_set_enable(void){}
void r_ble_ll_adv_set_enable_eco4(void){}
void r_ble_ll_adv_sm_init(void){}
void r_ble_ll_adv_sm_init_eco4(void){}
void r_ble_ll_hci_set_adv_data(void){}
void r_ble_ll_hci_set_adv_data_eco4(void){}
void r_ble_ll_hci_set_scan_rsp_data(void){}
void r_ble_ll_hci_set_scan_rsp_data_eco4(void){}
void r_ble_ll_adv_scan_rsp_length_chk(void){}
void r_ble_ll_adv_scan_rsp_length_chk_eco4(void){}
void r_ble_ll_adv_data_length_chk(void){}
void r_ble_ll_adv_data_length_chk_eco4(void){}

/* Cut */
int r_ble_ll_adv_init(void){return 0;}
int r_ble_ll_adv_init_eco4(void){return 0;}
void r_ble_ll_adv_deinit(void){}
void r_ble_ll_adv_deinit_eco4(void){}
void r_ble_ll_adv_reset(void){}
void r_ble_ll_adv_reset_eco4(void){}
void r_ble_ll_adv_enabled(void){}
void r_ble_ll_adv_enabled_eco4(void){}
void r_ble_ll_adv_set_random_addr(void){}
void r_ble_ll_adv_set_random_addr_eco4(void){}
void r_ble_ll_adv_rpa_timeout(void){}
void r_ble_ll_adv_rpa_timeout_eco4(void){}
void r_ble_ll_adv_can_chg_whitelist(void){}
void r_ble_ll_adv_can_chg_whitelist_eco4(void){}
void r_ble_ll_adv_delay_get(void){}
void r_ble_ll_adv_delay_get_eco4(void){}
void r_ble_lll_adv_update_rsp_offset(void){}
void r_ble_lll_adv_update_rsp_offset_eco4(void){}
#endif // !DEFAULT_BT_LE_ROLE_BROADCASTER

#if !DEFAULT_BT_LE_EXT_ADV
void r_ble_ll_adv_ext_set_param(void){}
void r_ble_ll_adv_ext_set_param_eco4(void){}
void r_ble_ll_adv_ext_set_adv_data(void){}
void r_ble_ll_adv_ext_set_adv_data_eco4(void){}
void r_ble_ll_adv_ext_set_scan_rsp(void){}
void r_ble_ll_adv_ext_set_scan_rsp_eco4(void){}
void r_ble_ll_adv_ext_estimate_data_itvl(void){}
void r_ble_ll_adv_ext_estimate_data_itvl_eco4(void){}
void r_ble_ll_adv_ext_check_data_itvl(void){}
void r_ble_ll_adv_ext_check_data_itvl_eco4(void){}
void r_ble_ll_hci_send_ext_adv_report(void){}
void r_ble_ll_hci_send_ext_adv_report_eco4(void){}
void r_ble_ll_hci_send_legacy_ext_adv_report(void){}
void r_ble_ll_hci_send_legacy_ext_adv_report_eco4(void){}
#endif //!DEFAULT_BT_LE_EXT_ADV

#if !DEFAULT_BT_LE_ROLE_OBSERVER
int r_ble_ll_scan_env_init(void){return 0;}
int r_ble_ll_scan_env_init_eco4(void){return 0;}
void r_ble_ll_scan_dup_update_legacy(void){}
void r_ble_ll_scan_dup_update_legacy_eco4(void){}
void r_ble_ll_scan_get_next_adv_prim_chan(void){}
void r_ble_ll_scan_get_next_adv_prim_chan_eco4(void){}
void r_ble_ll_scan_move_window_to(void){}
void r_ble_ll_scan_move_window_to_eco4(void){}
void r_ble_ll_scan_send_adv_report(void){}
void r_ble_ll_scan_send_adv_report_eco4(void){}
void r_ble_ll_scan_send_adv_lost_report(void){}
void r_ble_ll_scan_send_adv_lost_report_eco4(void){}
void r_ble_ll_scan_common_init(void){}
void r_ble_ll_scan_common_init_eco4(void){}
void r_ble_ll_scan_pre_process(void){}
void r_ble_ll_scan_pre_process_eco4(void){}
void r_ble_ll_scan_env_deinit(void){}
void r_ble_ll_scan_env_deinit_eco4(void){}
void r_ble_ll_check_scan_params(void){}
void r_ble_ll_check_scan_params_eco4(void){}
void r_ble_ll_scan_hci_specify_scan_chan(void){}
void r_ble_ll_scan_hci_specify_scan_chan_eco4(void){}
void r_ble_lll_scan_coex_event_cb(void){}
void r_ble_lll_scan_coex_event_cb_eco4(void){}
void r_ble_lll_scan_req_backoff(void){}
void r_ble_lll_scan_req_backoff_eco4(void){}
void r_ble_lll_scan_common_init(void){}
void r_ble_lll_scan_common_init_eco4(void){}
void r_ble_lll_scan_restart(void){}
void r_ble_lll_scan_restart_eco4(void){}
void r_ble_lll_scan_rx_pkt_isr(void){}
void r_ble_lll_scan_rx_pkt_isr_eco4(void){}
void r_ble_lll_scan_start(void){}
void r_ble_lll_scan_start_eco4(void){}
void r_ble_lll_scan_start_rx(void){}
void r_ble_lll_scan_start_rx_eco4(void){}
void r_ble_lll_scan_stop(void){}
void r_ble_lll_scan_stop_eco4(void){}
void r_ble_lll_scan_event_proc(void){}
void r_ble_lll_scan_event_proc_eco4(void){}
void r_ble_ll_get_chan_to_scan(void){}
void r_ble_ll_get_chan_to_scan_eco4(void){}
void r_ble_phy_set_scan_mode(void){}
void r_ble_phy_set_scan_mode_eco4(void){}
/* Module cut */

void r_ble_ll_scan_init(void){}
void r_ble_ll_scan_init_eco4(void){}
void r_ble_ll_scan_deinit(void){}
void r_ble_ll_scan_deinit_eco4(void){}
void r_ble_ll_scan_reset(void){}
void r_ble_ll_scan_reset_eco4(void){}
void r_ble_ll_scan_can_chg_whitelist(void){}
void r_ble_ll_scan_can_chg_whitelist_eco4(void){}
void r_ble_ll_scan_enabled(void){}
void r_ble_ll_scan_enabled_eco4(void){}
void r_ble_ll_scan_continue_status_get_eco4(void){}
void r_ble_ll_scan_continue_status_get_eco4_eco4(void){}
void r_ble_ll_scan_rxed(void){}
void r_ble_ll_scan_rxed_eco4(void){}
void r_ble_ll_scan_set_perfer_addr(void){}
void r_ble_ll_scan_set_perfer_addr_eco4(void){}
void r_ble_lll_scan_chk_resume(void){}
void r_ble_lll_scan_chk_resume_eco4(void){}
int r_ble_ll_scan_status_check(void){return 0;}
int r_ble_ll_scan_status_check_eco4(void){return 0;}
#endif // !DEFAULT_BT_LE_ROLE_OBSERVER

#if !DEFAULT_BT_LE_EXT_SCAN
void r_ble_ll_scan_rx_pkt_in_on_aux(void){}
void r_ble_ll_scan_rx_pkt_in_on_aux_eco4(void){}
void r_ble_lll_aux_scan_cb(void){}
void r_ble_lll_aux_scan_cb_eco4(void){}
#endif  //!DEFAULT_BT_LE_EXT_SCAN

#if !DEFAULT_BT_LE_ROLE_CENTROL
void r_ble_ll_conn_ext_master_init(void){}
void r_ble_ll_conn_ext_master_init_eco4(void){}
void r_ble_ll_conn_master_init(void){}
void r_ble_ll_conn_master_init_eco4(void){}
void r_ble_lll_init_rx_pkt_isr(void){}
void r_ble_lll_init_rx_pkt_isr_eco4(void){}
void r_ble_ll_ctrl_rx_conn_param_rsp(void){}
void r_ble_ll_ctrl_rx_conn_param_rsp_eco4(void){}
#endif // !DEFAULT_BT_LE_ROLE_CENTROL

#if !DEFAULT_BT_LE_ROLE_PERIPHERAL
void r_ble_ll_conn_slave_start(void){}
void r_ble_ll_conn_slave_start_eco4(void){}
void r_ble_ll_ctrl_rx_conn_update(void){}
void r_ble_ll_ctrl_rx_conn_update_eco4(void){}
void r_ble_lll_conn_event_delete_and_reschedule(void){}
void r_ble_lll_conn_event_delete_and_reschedule_eco4(void){}
void r_ble_ll_utils_verify_aa(void){}
void r_ble_ll_utils_verify_aa_eco4(void){}
#endif // !DEFAULT_BT_LE_ROLE_PERIPHERAL

#if !DEFAULT_BT_LE_ROLE_CENTROL && !DEFAULT_BT_LE_ROLE_PERIPHERAL
void r_ble_ll_conn_rx_data_pdu(void){}
void r_ble_ll_conn_rx_data_pdu_eco4(void){}
int r_ble_ll_conn_callout_env_init(void){return 0;}
int r_ble_ll_conn_callout_env_init_eco4(void){return 0;}
void r_ble_ll_conn_callout_env_deinit(void){}
void r_ble_ll_conn_callout_env_deinit_eco4(void){}
void r_ble_ll_conn_hci_set_data_len(void){}
void r_ble_ll_conn_hci_set_data_len_eco4(void){}
void r_ble_ll_conn_module_deinit(void){}
void r_ble_ll_conn_module_deinit_eco4(void){}
void r_ble_ll_conn_module_init(void){}
void r_ble_ll_conn_module_init_eco4(void){}
void r_ble_ll_conn_next_event(void){}
void r_ble_ll_conn_next_event_eco4(void){}
void r_ble_ll_conn_set_txpwr_by_handle(void){}
void r_ble_ll_conn_set_txpwr_by_handle_eco4(void){}
void r_ble_ll_conn_sm_new(void){}
void r_ble_ll_conn_sm_new_eco4(void){}
void r_ble_ll_ctrl_rx_chanmap_req(void){}
void r_ble_ll_ctrl_rx_chanmap_req_eco4(void){}
void r_ble_ll_ctrl_conn_param_pdu_make(void){}
void r_ble_ll_ctrl_conn_param_pdu_make_eco4(void){}
void r_ble_ll_ctrl_rx_pdu(void){}
void r_ble_ll_ctrl_rx_pdu_eco4(void){}
void r_ble_ll_ctrl_stop_rsp_timer(void){}
void r_ble_ll_ctrl_stop_rsp_timer_eco4(void){}
void r_ble_ll_conn_update_new_conn_param(void){}
void r_ble_ll_conn_update_new_conn_param_eco4(void){}
void r_ble_ll_conn_update_new_phy(void){}
void r_ble_ll_conn_update_new_phy_eco4(void){}
void r_ble_ll_conn_update_new_chan_map(void){}
void r_ble_ll_conn_update_new_chan_map_eco4(void){}
void r_ble_lll_conn_module_init(void){}
void r_ble_lll_conn_module_init_eco4(void){}
void r_ble_lll_conn_pre_process(void){}
void r_ble_lll_conn_pre_process_eco4(void){}
void r_ble_lll_conn_create_scheduled(void){}
void r_ble_lll_conn_create_scheduled_eco4(void){}
void r_ble_phy_set_conn_mode(void){}
void r_ble_phy_set_conn_mode_eco4(void){}
void r_ble_phy_sequence_update_conn_ind_params(void){}
void r_ble_phy_sequence_update_conn_ind_params_eco4(void){}
void r_ble_phy_update_conn_sequence(void){}
void r_ble_phy_update_conn_sequence_eco4(void){}
void r_ble_lll_conn_event_end_timer_cb(void){}
void r_ble_lll_conn_event_end_timer_cb_eco4(void){}
void r_ble_lll_conn_rx_pkt_isr(void){}
void r_ble_lll_conn_rx_pkt_isr_eco4(void){}
/* Can't cut */

// int r_ble_ll_conn_env_init(void){return 0;}
// int_eco4 r_ble_ll_conn_env_init(void){return 0;}
#endif // !DEFAULT_BT_LE_ROLE_CENTROL && !DEFAULT_BT_LE_ROLE_PERIPHERAL

#if !DEFAULT_BT_LE_ROLE_PERIPHERAL || !DEFAULT_BT_LE_2M_PHY && !DEFAULT_BT_LE_CODED_PHY
void r_ble_ll_ctrl_rx_phy_update_ind(void){}
void r_ble_ll_ctrl_rx_phy_update_ind_eco4(void){}
#endif // !DEFAULT_BT_LE_2M_PHY && !DEFAULT_BT_LE_CODED_PHY

#if !DEFAULT_BT_LE_50_FEATURE_SUPPORT
#endif // !DEFAULT_BT_LE_50_FEATURE_SUPPORT

#if !DEFAULT_BT_LE_PERIODIC_SYNC
void r_ble_ll_sync_parse_ext_hdr(void){}
void r_ble_ll_sync_parse_ext_hdr_eco4(void){}
#endif // !DEFAULT_BT_LE_PERIODIC_ADV

#if !CONFIG_BT_LE_SLEEP_ENABLE
void r_ble_lll_rfmgmt_controller_sleep_en(void){};
void r_ble_lll_rfmgmt_controller_sleep_en_eco4(void){};
void r_ble_lll_rfmgmt_set_sleep_cb(void *s_cb, void *w_cb, void *s_arg, void *w_arg, uint32_t us_to_enabled){}
void r_ble_lll_rfmgmt_set_sleep_cb_eco4(void *s_cb, void *w_cb, void *s_arg, void *w_arg, uint32_t us_to_enabled){}
void r_ble_lll_rfmgmt_disable(void){}
void r_ble_lll_rfmgmt_disable_eco4(void){}
void r_ble_lll_rfmgmt_timer_exp(void){}
void r_ble_lll_rfmgmt_timer_exp_eco4(void){}
void r_ble_lll_rfmgmt_timer_reschedule(void){}
void r_ble_lll_rfmgmt_timer_reschedule_eco4(void){}
void r_ble_lll_rfmgmt_enable(void){}
void r_ble_lll_rfmgmt_enable_eco4(void){}
void r_ble_lll_rfmgmt_enable_now(void){}
void r_ble_lll_rfmgmt_enable_now_eco4(void){}
void r_ble_lll_rfmgmt_is_enabled(void){}
void r_ble_lll_rfmgmt_is_enabled_eco4(void){}
void r_ble_lll_rfmgmt_reset(void){}
void r_ble_lll_rfmgmt_reset_eco4(void){}
int r_ble_lll_rfmgmt_env_init(void){return 0;}
int r_ble_lll_rfmgmt_env_init_eco4(void){return 0;}
void r_ble_lll_rfmgmt_should_skip_light_sleep_check(void){}
void r_ble_lll_rfmgmt_should_skip_light_sleep_check_eco4(void){}
void r_ble_lll_rfmgmt_wake_up_overhead_set(void){}
void r_ble_lll_rfmgmt_wake_up_overhead_set_eco4(void){}
void r_ble_lll_rfmgmt_get_wakeup_delta(void){}
void r_ble_lll_rfmgmt_get_wakeup_delta_eco4(void){}
#endif // CONFIG_BT_LE_SLEEP_ENABLE

#if !CONFIG_BT_LE_DTM_ENABLED
void r_ble_lll_dtm_rx_create_ctx(void){}
void r_ble_lll_dtm_rx_create_ctx_eco4(void){}
void r_ble_lll_dtm_rx_pkt_in(void){}
void r_ble_lll_dtm_rx_pkt_in_eco4(void){}
void r_ble_lll_dtm_tx_create_ctx(void){}
void r_ble_lll_dtm_tx_create_ctx_eco4(void){}
void r_ble_lll_dtm_rx_resi_get(void){}
void r_ble_lll_dtm_rx_resi_get_eco4(void){}
#endif // !CONFIG_BT_LE_DTM_ENABLED

#if !DEFAULT_BT_LE_TX_CCA_ENABLED
void r_ble_lll_sched_offset_update(void){}
void r_ble_lll_sched_offset_update_eco4(void){}
#endif // !DEFAULT_BT_LE_TX_CCA_ENABLED

#if !DEFAULT_BT_LE_ROLE_OBSERVER && !DEFAULT_BT_LE_ROLE_CENTROL && !DEFAULT_BT_LE_ROLE_PERIPHERAL
void r_ble_phy_set_rxwin1(void){}
void r_ble_phy_set_rxwin1_eco4(void){}
#endif // !DEFAULT_BT_LE_ROLE_CENTROL && !DEFAULT_BT_LE_ROLE_PERIPHERAL

#if !DEFAULT_BT_LE_PERIODIC_SYNC && !DEFAULT_BT_LE_ROLE_PERIPHERAL
void r_ble_ll_customize_peer_sca_set(void){}
void r_ble_ll_customize_peer_sca_set_eco4(void){}
int r_ble_ll_customize_peer_sca_get(void){return 0;}
int r_ble_ll_customize_peer_sca_get_eco4(void){return 0;}
int r_ble_ll_utils_calc_window_widening(void){return 0;}
int r_ble_ll_utils_calc_window_widening_eco4(void){return 0;}
#endif // !DEFAULT_BT_LE_PERIODIC_SYNC && !DEFAULT_BT_LE_ROLE_PERIPHERAL

#if !(CONFIG_BT_NIMBLE_ENABLED || CONFIG_BT_BLUEDROID_ENABLED)
void r_ble_vendor_hci_register(void){}
void r_ble_vendor_hci_register_eco4(void){}
void r_ble_vendor_hci_unregister(void){}
void r_ble_vendor_hci_unregister_eco4(void){}
void r_ble_ll_hci_vendor_event_is_enabled(void){}
void r_ble_ll_hci_vendor_event_is_enabled_eco4(void){}
void r_ble_ll_hci_vs_cmd_proc(void){}
void r_ble_ll_hci_vs_cmd_proc_eco4(void){}
void r_ble_ll_hci_vs_csa_set(void){}
void r_ble_ll_hci_vs_csa_set_eco4(void){}
void r_ble_ll_reset_config_params(void){}
void r_ble_ll_reset_config_params_eco4(void){}
#endif // !(CONFIG_BT_NIMBLE_ENABLED || CONFIG_BT_BLUEDROID_ENABLED)


#endif // _BLE_DUMMY_H_
