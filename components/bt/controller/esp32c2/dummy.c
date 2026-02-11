/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_bt_cfg.h"

#define BLE_ERR_UNKNOWN_HCI_CMD         (0x01)
/* LL Features */
#define BLE_LL_FEAT_LE_ENCRYPTION       (0x0000000001)
#define BLE_LL_FEAT_CONN_PARM_REQ       (0x0000000002)
#define BLE_LL_FEAT_EXTENDED_REJ        (0x0000000004)
#define BLE_LL_FEAT_PERIPH_INIT         (0x0000000008)
#define BLE_LL_FEAT_LE_PING             (0x0000000010)
#define BLE_LL_FEAT_DATA_LEN_EXT        (0x0000000020)
#define BLE_LL_FEAT_LL_PRIVACY          (0x0000000040)
#define BLE_LL_FEAT_EXT_SCAN_FILT       (0x0000000080)
#define BLE_LL_FEAT_LE_2M_PHY           (0x0000000100)
#define BLE_LL_FEAT_STABLE_MOD_ID_TX    (0x0000000200)
#define BLE_LL_FEAT_STABLE_MOD_ID_RX    (0x0000000400)
#define BLE_LL_FEAT_LE_CODED_PHY        (0x0000000800)
#define BLE_LL_FEAT_EXT_ADV             (0x0000001000)
#define BLE_LL_FEAT_PERIODIC_ADV        (0x0000002000)
#define BLE_LL_FEAT_CSA2                (0x0000004000)
#define BLE_LL_FEAT_LE_POWER_CLASS_1    (0x0000008000)
#define BLE_LL_FEAT_MIN_USED_CHAN       (0x0000010000)
#define BLE_LL_FEAT_CTE_REQ             (0x0000020000)
#define BLE_LL_FEAT_CTE_RSP             (0x0000040000)
#define BLE_LL_FEAT_CTE_TX              (0x0000080000)
#define BLE_LL_FEAT_CTE_RX              (0x0000100000)
#define BLE_LL_FEAT_CTE_AOD             (0x0000200000)
#define BLE_LL_FEAT_CTE_AOA             (0x0000400000)
#define BLE_LL_FEAT_CTE_RECV            (0x0000800000)
#define BLE_LL_FEAT_SYNC_TRANS_SEND     (0x0001000000)
#define BLE_LL_FEAT_SYNC_TRANS_RECV     (0x0002000000)
#define BLE_LL_FEAT_SCA_UPDATE          (0x0004000000)
#define BLE_LL_FEAT_REM_PKEY            (0x0008000000)
#define BLE_LL_FEAT_CIS_CENTRAL         (0x0010000000)
#define BLE_LL_FEAT_CIS_PERIPH          (0x0020000000)
#define BLE_LL_FEAT_ISO_BROADCASTER     (0x0040000000)
#define BLE_LL_FEAT_SYNC_RECV           (0x0080000000)
#define BLE_LL_FEAT_CIS_HOST            (0x0100000000)
#define BLE_LL_FEAT_POWER_CTRL_REQ      (0x0200000000)
#define BLE_LL_FEAT_POWER_CHANGE_IND    (0x0400000000)
#define BLE_LL_FEAT_PATH_LOSS_MON       (0x0800000000)
#define BLE_LL_FEAT_PERIODIC_ADV_ADI    (0x1000000000)
#define BLE_LL_FEAT_CONN_SUBRATING      (0x2000000000)
#define BLE_LL_FEAT_CONN_SUBRATING_HOST (0x4000000000)
#define BLE_LL_FEAT_CHANNEL_CLASS       (0x8000000000)

uint64_t ble_ll_supported_features;

void
ble_ll_supported_features_init(void)
{
    ble_ll_supported_features = BLE_LL_FEAT_EXTENDED_REJ;
    ble_ll_supported_features |= BLE_LL_FEAT_DATA_LEN_EXT;

#if DEFAULT_BT_LE_ROLE_CENTROL || DEFAULT_BT_LE_ROLE_PERIPHERAL
    ble_ll_supported_features |= BLE_LL_FEAT_PERIPH_INIT;
    ble_ll_supported_features |= BLE_LL_FEAT_CONN_PARM_REQ;
#endif

#if CONFIG_BT_LE_FEAT_LL_ENCRYPTION
    ble_ll_supported_features |= BLE_LL_FEAT_LE_ENCRYPTION;
#endif

    ble_ll_supported_features |= (BLE_LL_FEAT_LL_PRIVACY | BLE_LL_FEAT_EXT_SCAN_FILT);
    ble_ll_supported_features |= BLE_LL_FEAT_LE_PING;

#if DEFAULT_BT_LE_EXT_ADV
    ble_ll_supported_features |= BLE_LL_FEAT_EXT_ADV;
#endif

#if DEFAULT_BT_LE_PERIODIC_ADV
    ble_ll_supported_features |= BLE_LL_FEAT_PERIODIC_ADV;
    ble_ll_supported_features |= BLE_LL_FEAT_PERIODIC_ADV_ADI;
#endif

#if DEFAULT_BT_LE_PAST
    ble_ll_supported_features |= BLE_LL_FEAT_SYNC_TRANS_RECV;
    ble_ll_supported_features |= BLE_LL_FEAT_SYNC_TRANS_SEND;
#endif

#if DEFAULT_BT_LE_2M_PHY
    ble_ll_supported_features |= BLE_LL_FEAT_LE_2M_PHY;
#endif

#if DEFAULT_BT_LE_CODED_PHY
    ble_ll_supported_features |= BLE_LL_FEAT_LE_CODED_PHY;
#endif

#if DEFAULT_BT_LE_50_FEATURE_SUPPORT
    ble_ll_supported_features |= BLE_LL_FEAT_CSA2;
    ble_ll_supported_features |= BLE_LL_FEAT_SCA_UPDATE;
    ble_ll_supported_features |= BLE_LL_FEAT_REM_PKEY;
    ble_ll_supported_features |= BLE_LL_FEAT_CHANNEL_CLASS;
#endif
}

#if !DEFAULT_BT_LE_ROLE_BROADCASTER
void r_ble_ll_adv_rpa_timeout(void) { }
void r_ble_lll_adv_halt(void) { }
void r_ble_lll_adv_event_rmvd_from_sched(void) { }
void r_ble_lll_adv_ext_event_rmvd_from_sched(void) { }
int  r_ble_ll_adv_enabled(void) { return 0; }
int  r_ble_ll_adv_can_chg_whitelist(void) { return 1; }
int  r_ble_ll_adv_set_random_addr(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
void r_ble_ll_adv_reset(void) { }
void r_ble_ll_adv_init(void) { }
void r_ble_ll_adv_deinit(void) { }
int  r_ble_ll_adv_env_init(void) { return 0; }
void r_ble_ll_adv_env_deinit(void) { }
int  r_ble_lll_adv_rx_pkt_isr(void) { return -1; }
void r_ble_ll_adv_rx_pkt_in(void) { }
int  r_ble_ll_adv_set_adv_params(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_adv_read_txpwr(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_hci_set_adv_data(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_hci_set_scan_rsp_data(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_hci_adv_set_enable(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_adv_vendor_hci_legacy_adv_clear(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_adv_set_data_related_addr_change(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
#endif // !DEFAULT_BT_LE_ROLE_BROADCASTER

#if !DEFAULT_BT_LE_EXT_ADV
bool r_ble_ll_adv_ext_check_data_itvl(void) { return true; }
void r_ble_lll_adv_coex_dpc_update_on_aux_scheduled(void) { }
void r_ble_lll_adv_coex_dpc_calc_pti_update_itvl(void) { }
void r_ble_lll_adv_sec_done(void) { }
int r_ble_lll_adv_sec_schedule_next_aux(void) { return 0; }
void r_ble_lll_adv_sec_event_done(void) { }
int  r_ble_lll_adv_secondary_tx_start_cb(void) { return 0; }
void r_ble_lll_adv_aux_schedule(void) { }
void r_ble_lll_adv_update_rsp_offset(void) { }
int  r_ble_ll_adv_hci_set_random_addr(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_adv_ext_set_param(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_adv_ext_set_adv_data(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_adv_ext_set_scan_rsp(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_adv_ext_set_enable(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_adv_rd_max_adv_data_len(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_adv_rd_sup_adv_sets(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_adv_remove(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_adv_clear_all(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
#endif //!DEFAULT_BT_LE_EXT_ADV

#if !DEFAULT_BT_LE_PERIODIC_ADV
void r_ble_ll_adv_sm_stop_periodic(void) { }
void r_ble_lll_adv_periodic_event_done(void) { }
int  r_ble_lll_adv_sync_tx_start_cb(void) { return 0; }
void r_ble_lll_adv_sync_tx_end(void) { }
int  r_ble_lll_adv_periodic_start(void) { return 0; }
void r_ble_lll_adv_periodic_rmvd_from_sched(void) { }
int  r_ble_ll_adv_periodic_set_param(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_adv_periodic_set_data(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_adv_periodic_enable(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
#endif //!DEFAULT_BT_LE_PERIODIC_ADV

#if !DEFAULT_BT_LE_ROLE_OBSERVER
void r_ble_lll_scan_halt(void) { }
void r_ble_ll_scan_end_adv_evt(void) { }
void r_ble_ll_scan_rx_pkt_in(void) { }
int  r_ble_lll_scan_rx_pkt_isr(void) { return -1; }
int  r_ble_ll_scan_env_init(void) { return 0; }
void r_ble_ll_scan_env_deinit(void) {  }
void r_ble_ll_scan_init(void) { }
void r_ble_ll_scan_deinit(void) { }
void r_ble_ll_scan_reset(void) { }
int  r_ble_ll_scan_can_chg_whitelist(void) { return 1; }
int  r_ble_ll_scan_enabled(void) { return false; }
int  r_ble_lll_scan_chk_resume(void) { return -1; }
int  r_ble_ll_scan_set_scan_params(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_hci_scan_set_enable(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_scan_hci_update_adv_report_flow_ctrl(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_scan_hci_set_adv_report_flow_ctrl(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
#endif //!DEFAULT_BT_LE_ROLE_OBSERVER

#if !DEFAULT_BT_LE_EXT_SCAN
void r_ble_lll_scan_duration_period_timers_restart(void) { }
void r_ble_lll_scan_duration_period_timers_stop(void) { }
int r_ble_ll_hci_send_legacy_ext_adv_report(void) { return -1; }
void r_ble_lll_sched_rmv_elem_type(void) { }
void r_ble_ll_scan_send_truncated(void) { }
void r_ble_ll_scan_aux_data_unref(void) { }
void r_ble_lll_scan_sched_remove(void) { }
void r_ble_lll_scan_aux_data_free(void) { }
void r_ble_lll_aux_scan_drop(void) { }
int r_ble_lll_sched_aux_scan(void) { return -1; }
int r_ble_lll_scan_rx_isr_on_aux(void) { return -1; }
void r_ble_lll_scan_period_timer_cb(void) { }
void r_ble_lll_scan_duration_timer_cb(void) { }
void r_ble_ll_scan_rx_pkt_in_on_aux(void) { }
int  r_ble_ll_set_ext_scan_params(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_hci_ext_scan_set_enable(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
#endif //!DEFAULT_BT_LE_EXT_SCAN

#if !DEFAULT_BT_LE_ROLE_CENTROL
void r_ble_ll_init_rx_pkt_in(void) { }
int  r_ble_lll_init_rx_pkt_isr(void) { return -1; }
int  r_ble_ll_conn_create(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_conn_create_cancel(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
#endif //!DEFAULT_BT_LE_ROLE_CENTROL

#if !DEFAULT_BT_LE_ROLE_CENTROL || !DEFAULT_BT_LE_EXT_SCAN
int  r_ble_ll_ext_conn_create(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
#endif //!DEFAULT_BT_LE_ROLE_CENTROL || !DEFAULT_BT_LE_EXT_SCAN

#if !DEFAULT_BT_LE_ROLE_PERIPHERAL
int r_ble_ll_conn_slave_start(void) { return 0; }
#endif //!DEFAULT_BT_LE_ROLE_PERIPHERAL

#if !DEFAULT_BT_LE_ROLE_CENTROL && !DEFAULT_BT_LE_ROLE_PERIPHERAL
void r_ble_ll_conn_rx_data_pdu(void) { }
int  r_ble_lll_conn_rx_pkt_isr(void) { return -1; }
int  r_ble_ll_hci_disconnect(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_conn_hci_rd_rem_ver_cmd(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_conn_hci_update(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_conn_hci_rd_chan_map(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_conn_hci_read_rem_features(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_conn_hci_param_rr(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_conn_hci_param_nrr(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
#endif //!DEFAULT_BT_LE_ROLE_CENTROL && !DEFAULT_BT_LE_ROLE_PERIPHERAL

#if !CONFIG_BT_LE_FEAT_LL_ENCRYPTION
int r_ble_ll_conn_chk_phy_upd_start(void) { return -1; }
void r_ble_ll_hci_ev_encrypt_chg(void) { }
int r_ble_ll_ctrl_enc_allowed_pdu_rx(void) { return 1; }
int r_ble_ll_ctrl_enc_allowed_pdu_tx(void) { return 1; }
uint8_t r_ble_ll_ctrl_rx_start_enc_rsp(void) { return 0x07; }
uint8_t r_ble_ll_ctrl_rx_pause_enc_rsp(void) { return 0x07; }
int  r_ble_ll_hci_le_encrypt(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
#endif //!CONFIG_BT_LE_FEAT_LL_ENCRYPTION

#if !DEFAULT_BT_LE_ROLE_PERIPHERAL || !CONFIG_BT_LE_FEAT_LL_ENCRYPTION
uint8_t r_ble_ll_ctrl_rx_pause_enc_req(void) { return 0x07; }
uint8_t r_ble_ll_ctrl_rx_enc_req(void) { return 0x07; }
int r_ble_ll_conn_hci_le_ltk_reply(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int r_ble_ll_conn_hci_le_ltk_neg_reply(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
#endif //!DEFAULT_BT_LE_ROLE_PERIPHERAL || !CONFIG_BT_LE_FEAT_LL_ENCRYPTION

#if !DEFAULT_BT_LE_ROLE_CENTROL || !CONFIG_BT_LE_FEAT_LL_ENCRYPTION
uint8_t r_ble_ll_ctrl_rx_start_enc_req(void) { return 0x07; }
void r_ble_ll_ctrl_rx_enc_rsp(void) { }
void r_ble_ll_ctrl_enc_req_make(void) { }
int r_ble_ll_conn_hci_le_start_encrypt(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
#endif //!DEFAULT_BT_LE_ROLE_CENTROL || !CONFIG_BT_LE_FEAT_LL_ENCRYPTION

#if !DEFAULT_BT_LE_2M_PHY && !DEFAULT_BT_LE_CODED_PHY
void r_ble_ll_ctrl_phy_update_proc_complete(void) { }
void r_ble_ll_ctrl_phy_update_cancel(void) { }
uint8_t r_ble_ll_ctrl_rx_phy_update_ind(void) { return 0x07; }
uint8_t r_ble_ll_ctrl_rx_phy_rsp(void) { return 0x07; }
uint8_t r_ble_ll_ctrl_rx_phy_req(void) { return 0x07; }
void r_ble_ll_ctrl_phy_req_rsp_make(void) { }
#endif //DEFAULT_BT_LE_2M_PHY && DEFAULT_BT_LE_CODED_PHY

#if !DEFAULT_BT_LE_PERIODIC_SYNC
void r_ble_lll_sync_halt(void) { }
void r_ble_lll_sync_rmvd_from_sched(void) { }
int  r_ble_ll_sync_list_search(void) { return -1; }
uint8_t r_ble_ll_ctrl_rx_periodic_sync_ind(void) { return 0x07; }
void r_ble_ll_sync_rx_pkt_in(void) {  }
int  r_ble_lll_sync_rx_pkt_isr(void) { return -1; }
int  r_ble_ll_sync_env_init(void) { return 0; }
void r_ble_ll_sync_env_deinit(void) {  }
void r_ble_ll_sync_init(void) {  }
void r_ble_ll_sync_deinit(void) {  }
void r_ble_ll_sync_reset(void) {  }
bool r_ble_ll_sync_enabled(void) { return false; }
int  r_ble_ll_sync_create(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_sync_cancel(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_sync_terminate(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_sync_list_add(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_sync_list_remove(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_sync_list_clear(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_sync_list_size(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_sync_receive_enable(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
#endif //!DEFAULT_BT_LE_PERIODIC_SYNC

#if !DEFAULT_BT_LE_PAST || !DEFAULT_BT_LE_PERIODIC_ADV
int  r_ble_ll_adv_periodic_set_info_transfer(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
#endif //!DEFAULT_BT_LE_PAST || !DEFAULT_BT_LE_PERIODIC_ADV

#if !DEFAULT_BT_LE_PAST || !DEFAULT_BT_LE_PERIODIC_SYNC
int  r_ble_ll_sync_transfer(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_set_sync_transfer_params(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_set_default_sync_transfer_params(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
#endif //!DEFAULT_BT_LE_PAST || !DEFAULT_BT_LE_PERIODIC_SYNC

#if !DEFAULT_BT_LE_50_FEATURE_SUPPORT
uint8_t r_ble_ll_ctrl_rx_channel_reporting_ind(void) { return 0x07; }
uint8_t r_ble_ll_ctrl_rx_channel_status_ind(void) { return 0x07; }
uint8_t r_ble_ll_ctrl_rx_sca_req(void) { return 0x07; }
uint8_t r_ble_ll_ctrl_rx_sca_rsp(void) { return 0x07; }
void r_ble_ll_ctrl_channel_class_reporting_make(void) { }
void r_ble_ll_ctrl_channel_class_enable_make(void) { }
void r_ble_ll_ctrl_sca_req_rsp_make(void) { }
int  r_ble_ll_modify_sca(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_conn_req_peer_sca(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
#endif //!DEFAULT_BT_LE_50_FEATURE_SUPPORT

#if !DEFAULT_BT_LE_50_FEATURE_SUPPORT
int  r_ble_ll_conn_hci_le_rd_phy(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_hci_le_set_def_phy(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_ll_conn_hci_le_set_phy(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
#endif //!DEFAULT_BT_LE_50_FEATURE_SUPPORT

#if !CONFIG_BT_LE_DTM_ENABLED
void r_ble_lll_dtm_rx_pkt_in(void) {  }
int  r_ble_lll_dtm_rx_isr_end(void) { return -1; }
void r_ble_lll_dtm_reset(void) { }
void r_ble_lll_dtm_init(void) { }
void r_ble_lll_dtm_deinit(void) { }
int  r_ble_lll_dtm_env_init(void) { return 0; }
void r_ble_lll_dtm_env_deinit(void) { }
int  r_ble_lll_hci_dtm_tx_test(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_lll_hci_dtm_rx_test(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_lll_dtm_end_test(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_lll_hci_dtm_rx_test_v2(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
int  r_ble_lll_hci_dtm_tx_test_v2(void) { return BLE_ERR_UNKNOWN_HCI_CMD; }
#endif //!CONFIG_BT_LE_DTM_ENABLED
