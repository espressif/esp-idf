/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_hci_internal.h"

#if UC_BT_CTRL_BLE_IS_ENABLE
#include "ble_priv.h"
#endif /* UC_BT_CTRL_BLE_IS_ENABLE */

#if UC_BT_CTRL_BR_EDR_IS_ENABLE
#include "bredr_priv.h"
#endif /* #if UC_BT_CTRL_BR_EDR_IS_ENABLE */

#include "esp_bt_vs.h"
#include <stdint.h>

#define HCI_CMD_OGF(p) (uint8_t)(0x003f & (p >> 10))
#define HCI_CMD_OCF(p) (0x3ff & (p))

#define HCI_CMD_RET_STATUS(err)   (0x100ul | (err))
#define HCI_CMD_RET_COMPLETE(err) (err)
#define HCI_CMD_RET_ERR_CODE(err) (err & 0x00fful)

#define HCI_CMD_ERR_UNKNOWN_HCI_CMD (0x01)
#define HCI_CMD_ERR_UNKNOWN_CONN_ID (0X02)
#define HCI_CMD_ERR_INVALID_PARAMS  (0X12)

/*
 ***************************************************************************************************
 * Opcode Group Field (OGF) Definitions
 ***************************************************************************************************
 */
#define HCI_CMD_OGF_LINK_CONTROL_CMDS (0x01)
#define HCI_CMD_OGF_LINK_POLICY_CMDS (0x02)
#define HCI_CMD_OGF_HOST_CONT_BASEBAND_CMDS (0x03)
#define HCI_CMD_OGF_INFORMATIONAL_PARAMS (0x04)
#define HCI_CMD_OGF_STATUS_PARAMS (0x05)
#define HCI_CMD_OGF_TESTING_CMDS (0x06)
#define HCI_CMD_OGF_BLE_CMDS (0x08)
#define HCI_CMD_OGF_VENDOR_SPECIFIC (0x3F)

/*
 ***************************************************************************************************
 * link Control Opcode Field (OCF) Definitions
 ***************************************************************************************************
 */
#define HCI_CMD_OCF_LINK_DISCONNECT (0x0006)
#define HCI_CMD_OCF_LINK_READ_RMT_VERSION_INFO (0x001D)

/*
 ***************************************************************************************************
 * Controller Baseband Opcode Field (OCF) Definitions
 ***************************************************************************************************
 */
#define HCI_CMD_OCF_BB_SET_EVENT_MASK (0x0001)
#define HCI_CMD_OCF_BB_RESET (0x0003)
#define HCI_CMD_OCF_BB_READ_AUTO_FLUSH_TOUT (0x0027)
#define HCI_CMD_OCF_BB_WRITE_AUTO_FLUSH_TOUT (0x0028)
#define HCI_CMD_OCF_BB_SET_C2H_FLOW_CTRL (0x0031)
#define HCI_CMD_OCF_BB_HOST_BUFFER_SIZE (0x0033)
#define HCI_CMD_OCF_BB_HOST_NUM_PACKETS_DONE (0x0035)
#define HCI_CMD_OCF_BB_WRITE_AFH_CHAN_ASSESSMENT_MODE (0x0049)
#define HCI_CMD_OCF_BB_SET_EVENT_MASK_PAGE_2 (0x0063)
#define HCI_CMD_OCF_BB_READ_AUTH_PYLD_TMO (0x007B)
#define HCI_CMD_OCF_BB_WRITE_AUTH_PYLD_TMO (0x007C)
#define HCI_CMD_OCF_BB_CONFIG_DATA_PATH (0x0083)

/*
 ***************************************************************************************************
 * Informational Parameters Opcode Field (OCF) Definitions
 ***************************************************************************************************
 */
#define HCI_CMD_OCF_INFO_READ_LOCAL_VERSION_INFO (0x0001)
#define HCI_CMD_OCF_INFO_READ_LOCAL_SUPPORTED_CMDS (0x0002)
#define HCI_CMD_OCF_INFO_READ_LOCAL_FEATURES (0x0003)
#define HCI_CMD_OCF_INFO_READ_LOCAL_EXT_FEATURES (0x0004)
#define HCI_CMD_OCF_INFO_READ_BUFFER_SIZE (0x0005)
#define HCI_CMD_OCF_INFO_READ_COUNTRY_CODE (0x0007)
#define HCI_CMD_OCF_INFO_READ_BD_ADDR (0x0009)
#define HCI_CMD_OCF_INFO_READ_DATA_BLOCK_SIZE (0x000A)
#define HCI_CMD_OCF_INFO_READ_LOCAL_SUPPORTED_CODECS (0x000B)
#define HCI_CMD_OCF_INFO_READ_LOCAL_SUPPORTED_CODECS_V2 (0x000D)
#define HCI_CMD_OCF_INFO_READ_LOCAL_SUPPORTED_CODEC_CAP (0x000E)
#define HCI_CMD_OCF_INFO_READ_LOCAL_SUPPORTED_CTRL_DELAY (0x000F)

/*
 ***************************************************************************************************
 * Status Parameters Opcode Field (OCF) Definitions
 ***************************************************************************************************
 */
#define HCI_CMD_SP_OCF_READ_RSSI (0x0005)

/*
 ***************************************************************************************************
 * LE Controller Opcode Field (OCF) Definitions
 ***************************************************************************************************
 */

#define HCI_CMD_OCF_LE_SET_EVENT_MASK (0x0001)
#define HCI_CMD_OCF_LE_RD_BUF_SIZE (0x0002)
#define HCI_CMD_OCF_LE_RD_LOC_SUPP_FEAT (0x0003)
#define HCI_CMD_OCF_LE_SET_RAND_ADDR (0x0005)
#define HCI_CMD_OCF_LE_SET_ADV_PARAMS (0x0006)
#define HCI_CMD_OCF_LE_RD_ADV_CHAN_TXPWR (0x0007)
#define HCI_CMD_OCF_LE_SET_ADV_DATA (0x0008)
#define HCI_CMD_OCF_LE_SET_SCAN_RSP_DATA (0x0009)
#define HCI_CMD_OCF_LE_SET_ADV_ENABLE (0x000A)
#define HCI_CMD_OCF_LE_SET_SCAN_PARAMS (0x000B)
#define HCI_CMD_OCF_LE_SET_SCAN_ENABLE (0x000C)
#define HCI_CMD_OCF_LE_CREATE_CONN (0x000D)
#define HCI_CMD_OCF_LE_CREATE_CONN_CANCEL (0x000E)
#define HCI_CMD_OCF_LE_RD_WHITE_LIST_SIZE (0x000F)
#define HCI_CMD_OCF_LE_CLEAR_WHITE_LIST (0x0010)
#define HCI_CMD_OCF_LE_ADD_WHITE_LIST (0x0011)
#define HCI_CMD_OCF_LE_RMV_WHITE_LIST (0x0012)
#define HCI_CMD_OCF_LE_CONN_UPDATE (0x0013)
#define HCI_CMD_OCF_LE_SET_HOST_CHAN_CLASS (0x0014)
#define HCI_CMD_OCF_LE_RD_CHAN_MAP (0x0015)
#define HCI_CMD_OCF_LE_RD_REM_FEAT (0x0016)
#define HCI_CMD_OCF_LE_ENCRYPT (0x0017)
#define HCI_CMD_OCF_LE_RAND (0x0018)
#define HCI_CMD_OCF_LE_START_ENCRYPT (0x0019)
#define HCI_CMD_OCF_LE_LT_KEY_REQ_REPLY (0x001A)
#define HCI_CMD_OCF_LE_LT_KEY_REQ_NEG_REPLY (0x001B)
#define HCI_CMD_OCF_LE_RD_SUPP_STATES (0x001C)
#define HCI_CMD_OCF_LE_RX_TEST (0x001D)
#define HCI_CMD_OCF_LE_TX_TEST (0x001E)
#define HCI_CMD_OCF_LE_TEST_END (0x001F)
#define HCI_CMD_OCF_LE_REM_CONN_PARAM_RR (0x0020)
#define HCI_CMD_OCF_LE_REM_CONN_PARAM_NRR (0x0021)
#define HCI_CMD_OCF_LE_SET_DATA_LEN (0x0022)
#define HCI_CMD_OCF_LE_RD_SUGG_DEF_DATA_LEN (0x0023)
#define HCI_CMD_OCF_LE_WR_SUGG_DEF_DATA_LEN (0x0024)
#define HCI_CMD_OCF_LE_RD_P256_PUBKEY (0x0025)
#define HCI_CMD_OCF_LE_GEN_DHKEY (0x0026)
#define HCI_CMD_OCF_LE_ADD_RESOLV_LIST (0x0027)
#define HCI_CMD_OCF_LE_RMV_RESOLV_LIST (0x0028)
#define HCI_CMD_OCF_LE_CLR_RESOLV_LIST (0x0029)
#define HCI_CMD_OCF_LE_RD_RESOLV_LIST_SIZE (0x002A)
#define HCI_CMD_OCF_LE_RD_PEER_RESOLV_ADDR (0x002B)
#define HCI_CMD_OCF_LE_RD_LOCAL_RESOLV_ADDR (0x002C)
#define HCI_CMD_OCF_LE_SET_ADDR_RES_EN (0x002D)
#define HCI_CMD_OCF_LE_SET_RPA_TMO (0x002E)
#define HCI_CMD_OCF_LE_RD_MAX_DATA_LEN (0x002F)
#define HCI_CMD_OCF_LE_RD_PHY (0x0030)
#define HCI_CMD_OCF_LE_SET_DEFAULT_PHY (0x0031)
#define HCI_CMD_OCF_LE_SET_PHY (0x0032)
#define HCI_CMD_OCF_LE_RX_TEST_V2 (0x0033)
#define HCI_CMD_OCF_LE_TX_TEST_V2 (0x0034)
#define HCI_CMD_OCF_LE_SET_ADV_SET_RND_ADDR (0x0035)
#define HCI_CMD_OCF_LE_SET_EXT_ADV_PARAM (0x0036)
#define HCI_CMD_OCF_LE_SET_EXT_ADV_DATA (0x0037)
#define HCI_CMD_OCF_LE_SET_EXT_SCAN_RSP_DATA (0x0038)
#define HCI_CMD_OCF_LE_SET_EXT_ADV_ENABLE (0x0039)
#define HCI_CMD_OCF_LE_RD_MAX_ADV_DATA_LEN (0x003A)
#define HCI_CMD_OCF_LE_RD_NUM_OF_ADV_SETS (0x003B)
#define HCI_CMD_OCF_LE_REMOVE_ADV_SET (0x003C)
#define HCI_CMD_OCF_LE_CLEAR_ADV_SETS (0x003D)
#define HCI_CMD_OCF_LE_SET_PERIODIC_ADV_PARAMS (0x003E)
#define HCI_CMD_OCF_LE_SET_PERIODIC_ADV_DATA (0x003F)
#define HCI_CMD_OCF_LE_SET_PERIODIC_ADV_ENABLE (0x0040)
#define HCI_CMD_OCF_LE_SET_EXT_SCAN_PARAM (0x0041)
#define HCI_CMD_OCF_LE_SET_EXT_SCAN_ENABLE (0x0042)
#define HCI_CMD_OCF_LE_EXT_CREATE_CONN (0x0043)
#define HCI_CMD_OCF_LE_PERIODIC_ADV_CREATE_SYNC (0x0044)
#define HCI_CMD_OCF_LE_PERIODIC_ADV_CREATE_SYNC_CANCEL (0x0045)
#define HCI_CMD_OCF_LE_PERIODIC_ADV_TERM_SYNC (0x0046)
#define HCI_CMD_OCF_LE_ADD_DEV_TO_PERIODIC_ADV_LIST (0x0047)
#define HCI_CMD_OCF_LE_REM_DEV_FROM_PERIODIC_ADV_LIST (0x0048)
#define HCI_CMD_OCF_LE_CLEAR_PERIODIC_ADV_LIST (0x0049)
#define HCI_CMD_OCF_LE_RD_PERIODIC_ADV_LIST_SIZE (0x004A)
#define HCI_CMD_OCF_LE_RD_TRANSMIT_POWER (0x004B)
#define HCI_CMD_OCF_LE_RD_RF_PATH_COMPENSATION (0x004C)
#define HCI_CMD_OCF_LE_WR_RF_PATH_COMPENSATION (0x004D)
#define HCI_CMD_OCF_LE_SET_PRIVACY_MODE (0x004E)
#define HCI_CMD_OCF_LE_RX_TEST_V3 (0x004F)
#define HCI_CMD_OCF_LE_TX_TEST_V3 (0x0050)
#define HCI_CMD_OCF_LE_SET_CONNLESS_CTE_TX_PARAMS (0x0051)
#define HCI_CMD_OCF_LE_SET_CONNLESS_CTE_TX_ENABLE (0x0052)
#define HCI_CMD_OCF_LE_SET_CONNLESS_IQ_SAMPLING_ENABLE (0x0053)
#define HCI_CMD_OCF_LE_SET_CONN_CTE_RX_PARAMS (0x0054)
#define HCI_CMD_OCF_LE_SET_CONN_CTE_TX_PARAMS (0x0055)
#define HCI_CMD_OCF_LE_SET_CONN_CTE_REQ_ENABLE (0x0056)
#define HCI_CMD_OCF_LE_SET_CONN_CTE_RESP_ENABLE (0x0057)
#define HCI_CMD_OCF_LE_RD_ANTENNA_INFO (0x0058)
#define HCI_CMD_OCF_LE_PERIODIC_ADV_RECEIVE_ENABLE (0x0059)
#define HCI_CMD_OCF_LE_PERIODIC_ADV_SYNC_TRANSFER (0x005A)
#define HCI_CMD_OCF_LE_PERIODIC_ADV_SET_INFO_TRANSFER (0x005B)
#define HCI_CMD_OCF_LE_PERIODIC_ADV_SYNC_TRANSFER_PARAMS (0x005C)
#define HCI_CMD_OCF_LE_SET_DEFAULT_SYNC_TRANSFER_PARAMS (0x005D)
#define HCI_CMD_OCF_LE_GENERATE_DHKEY_V2 (0x005E)
#define HCI_CMD_OCF_LE_MODIFY_SCA (0x005F)
#define HCI_CMD_OCF_LE_RD_BUF_SIZE_V2 (0x0060)
#define HCI_CMD_OCF_LE_READ_ISO_TX_SYNC (0x0061)
#define HCI_CMD_OCF_LE_SET_CIG_PARAMS (0x0062)
#define HCI_CMD_OCF_LE_SET_CIG_PARAMS_TEST (0x0063)
#define HCI_CMD_OCF_LE_CREATE_CIS (0x0064)
#define HCI_CMD_OCF_LE_REMOVE_CIG (0x0065)
#define HCI_CMD_OCF_LE_ACCEPT_CIS_REQ (0x0066)
#define HCI_CMD_OCF_LE_REJECT_CIS_REQ (0x0067)
#define HCI_CMD_OCF_LE_CREATE_BIG (0x0068)
#define HCI_CMD_OCF_LE_CREATE_BIG_TEST (0x0069)
#define HCI_CMD_OCF_LE_TERMINATE_BIG (0x006a)
#define HCI_CMD_OCF_LE_BIG_CREATE_SYNC (0x006b)
#define HCI_CMD_OCF_LE_BIG_TERMINATE_SYNC (0x006c)
#define HCI_CMD_OCF_LE_REQ_PEER_SCA (0x006d)
#define HCI_CMD_OCF_LE_SETUP_ISO_DATA_PATH (0x006e)
#define HCI_CMD_OCF_LE_REMOVE_ISO_DATA_PATH (0x006f)
#define HCI_CMD_OCF_LE_ISO_TRANSMIT_TEST (0x0070)
#define HCI_CMD_OCF_LE_ISO_RECEIVE_TEST (0x0071)
#define HCI_CMD_OCF_LE_ISO_READ_TEST_COUNTERS (0x0072)
#define HCI_CMD_OCF_LE_ISO_TEST_END (0x0073)
#define HCI_CMD_OCF_LE_SET_HOST_FEATURE (0x0074)
#define HCI_CMD_OCF_LE_READ_ISO_LINK_QUALITY (0x0075)
#define HCI_CMD_OCF_LE_ENH_READ_TRANSMIT_POWER_LEVEL (0x0076)
#define HCI_CMD_OCF_LE_READ_REMOTE_TRANSMIT_POWER_LEVEL (0x0077)
#define HCI_CMD_OCF_LE_SET_PATH_LOSS_REPORT_PARAM (0x0078)
#define HCI_CMD_OCF_LE_SET_PATH_LOSS_REPORT_ENABLE (0x0079)
#define HCI_CMD_OCF_LE_SET_TRANS_PWR_REPORT_ENABLE (0x007A)
#define HCI_CMD_OCF_LE_SET_DATA_ADDR_CHANGE (0x007C)
#define HCI_CMD_OCF_LE_SET_DEFAULT_SUBRATE (0x007D)
#define HCI_CMD_OCF_LE_SUBRATE_REQ (0x007E)
#define HCI_CMD_OCF_LE_SET_EXT_ADV_PARAM_V2 (0x007F)
#define HCI_CMD_OCF_LE_SET_DECISION_DATA (0x0080)
#define HCI_CMD_OCF_LE_SET_DECISION_INSTRUCTIONS (0x0081)
#define HCI_CMD_OCF_LE_SET_PERIODIC_ADV_SUBEV_DATA (0x0082)
#define HCI_CMD_OCF_LE_SET_PERIODIC_ADV_RESPONSE_DATA (0x0083)
#define HCI_CMD_OCF_LE_SET_PERIODIC_ADV_SYNC_SUBEVENT (0x0084)
#define HCI_CMD_OCF_LE_EXT_CREATE_CONN_V2 (0x0085)
#define HCI_CMD_OCF_LE_SET_PERIODIC_ADV_PARAMS_V2 (0x0086)
#define HCI_CMD_OCF_LE_RD_ALL_LOCAL_SUP_FEAT (0x0087)
#define HCI_CMD_OCF_LE_RD_ALL_REMOTE_SUP_FEAT (0x0088)
#define HCI_CMD_OCF_LE_SET_HOST_FEATURE_V2 (0x0097)
#define HCI_CMD_OCF_LE_ADD_DEV_TO_MONITORED_ADV_LIST (0x0098)
#define HCI_CMD_OCF_LE_RMV_DEV_FROM_MONITORED_ADV_LIST (0x0099)
#define HCI_CMD_OCF_LE_CLEAR_MONITORED_ADV_LIST (0x009A)
#define HCI_CMD_OCF_LE_RD_MONITORED_ADV_LIST_SIZE (0x009B)
#define HCI_CMD_OCF_LE_ENABLE_MONITORING_ADV (0x009C)
#define HCI_CMD_OCF_LE_FRAME_SPACE_UPDATE (0x009D)
#define HCI_CMD_OCF_LE_SET_RESOLV_PRIV_ADDR_TIMEOUT_V2 (0x009E)
#define HCI_CMD_OCF_LE_ENABLE_UTP_OTA_MODDE (0x009F)
#define HCI_CMD_OCF_LE_UTP_SEND (0x00A0)
#define HCI_CMD_OCF_LE_CONN_RATE_REQ (0x00A1)
#define HCI_CMD_OCF_LE_SET_DEFAULT_RATE_PARAM (0x00A2)
#define HCI_CMD_OCF_LE_READ_MIN_SUPP_CONN_ITVL (0x00A3)

/*
 ***************************************************************************************************
 * External Functions Declarations (from ble_hci_external.c / btdm_hci_external.c)
 * Caller passes (const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf, uint8_t *rsplen).
 ***************************************************************************************************
 */
extern void r_btdm_hci_set_cmd_handler_func(void *func);

/* Link Control (OGF 0x01) - btdm_hci_external */
extern int r_ble_ll_hci_disconnect(const uint8_t *cmdbuf, uint8_t len);
extern int r_ble_ll_conn_hci_rd_rem_ver_cmd(const uint8_t *cmdbuf, uint8_t len);

/* Controller Baseband (OGF 0x03) - btdm_hci_external */
extern int r_btdm_hci_set_event_mask_handler(const uint8_t *cmdbuf, uint8_t len);
extern int r_btdm_hci_reset_handler(const uint8_t *cmdbuf, uint8_t len);
extern int r_btdm_hci_set_event_mask_page2_handler(const uint8_t *cmdbuf, uint8_t len);
extern int r_btdm_c2hfc_set_flow_ctrl(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                      uint8_t *rsplen);
extern int r_btdm_c2hfc_host_buffer_size(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                         uint8_t *rsplen);
extern int r_btdm_c2hfc_host_num_completed_packets(const uint8_t *cmdbuf, uint8_t len,
                                                   uint8_t *rspbuf, uint8_t *rsplen);

extern int r_ble_ll_hci_cb_host_buf_size(const uint8_t *cmdbuf, uint8_t len);
extern int r_ble_ll_conn_hci_rd_auth_pyld_tmo(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                              uint8_t *rsplen);
extern int r_ble_ll_conn_hci_wr_auth_pyld_tmo(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                              uint8_t *rsplen);
extern int r_ble_ll_hci_cb_cfg_data_path(const uint8_t *cmdbuf, uint8_t len);

#if UC_BT_CTRL_LE_FLUSHABLE_ACL_DATA
extern int flushableAclData_hci_readAutoFlushTmo(const uint8_t *cmdbuf, uint8_t len,
                                                 uint8_t *rspbuf, uint8_t *rsplen);
extern int flushableAclData_hci_writeAutoFlushTmo(const uint8_t *cmdbuf, uint8_t len,
                                                  uint8_t *rspbuf, uint8_t *rsplen);
#endif // UC_BT_CTRL_LE_FLUSHABLE_ACL_DATA

/* Informational (OGF 0x04) - btdm_hci_external / ble */
extern int r_btdm_hci_rd_local_version_info_handler(const uint8_t *cmdbuf, uint8_t len,
                                                    uint8_t *rspbuf, uint8_t *rsplen);
extern int r_btdm_hci_rd_local_supported_cmds_handler(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                                      uint8_t *rsplen);
extern int r_btdm_hci_rd_local_supp_features_handler(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                                     uint8_t *rsplen);
extern int r_btdm_hci_rd_local_ext_features_handler(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                                    uint8_t *rsplen);
extern int r_btdm_hci_rd_bdaddr_handler(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf, uint8_t *rsplen);
extern int r_btdm_hci_rd_local_supp_codecs_handler(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf, uint8_t *rsplen);
extern int r_btdm_hci_rd_local_supp_codecs_v2_handler(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf, uint8_t *rsplen);
extern int r_btdm_hci_rd_local_supp_codec_cap_handler(const uint8_t *cmdbuf, uint8_t len,
                                                      uint8_t *rspbuf, uint8_t *rsplen);
extern int r_btdm_hci_rd_local_supp_ctrl_delay_handler(const uint8_t *cmdbuf, uint8_t len,
                                                       uint8_t *rspbuf, uint8_t *rsplen);
extern int r_ble_ll_hci_rd_local_version(uint8_t *rspbuf, uint8_t *rsplen);
extern int r_ble_ll_hci_rd_local_supp_cmd(uint8_t *rspbuf, uint8_t *rsplen);
extern int r_ble_ll_hci_rd_local_supp_feat(uint8_t *rspbuf, uint8_t *rsplen);
extern int r_ble_ll_hci_rd_bd_addr(uint8_t *rspbuf, uint8_t *rsplen);
extern int r_ble_ll_hci_rd_local_supp_codecs(uint8_t *rspbuf, uint8_t *rsplen);
extern int r_ble_ll_hci_rd_local_supp_codec_caps(const uint8_t *cmdbuf, uint8_t len,
                                                 uint8_t *rspbuf, uint8_t *rsplen);
extern int r_ble_ll_hci_rd_local_supp_controller_delay(const uint8_t *cmdbuf, uint8_t len,
                                                       uint8_t *rspbuf, uint8_t *rsplen);

/* Status (OGF 0x05) - btdm_hci_external */
extern int r_ble_ll_conn_hci_rd_rssi(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                     uint8_t *rsplen);

/* LE (OGF 0x08) - ble_hci_external (r_ble_ll_* / r_ble_hci_*) */
extern int r_ble_ll_hci_set_le_event_mask(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                          uint8_t *rsplen);
extern int r_ble_ll_hci_le_read_bufsize(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                        uint8_t *rsplen);
extern int r_ble_ll_hci_le_read_local_features(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                               uint8_t *rsplen);
extern int r_ble_ll_set_random_addr(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                    uint8_t *rsplen);
extern int r_ble_ll_adv_set_adv_params(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                       uint8_t *rsplen);
extern int r_ble_ll_adv_read_txpwr(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                   uint8_t *rsplen);
extern int r_ble_ll_hci_set_adv_data(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                     uint8_t *rsplen);
extern int r_ble_ll_hci_set_scan_rsp_data(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                          uint8_t *rsplen);
extern int r_ble_ll_hci_adv_set_enable(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                       uint8_t *rsplen);
extern int r_ble_ll_scan_set_scan_params(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                         uint8_t *rsplen);
extern int r_ble_ll_hci_scan_set_enable(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                        uint8_t *rsplen);
extern int r_ble_ll_conn_create(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                uint8_t *rsplen);
extern int r_ble_ll_conn_create_cancel(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                       uint8_t *rsplen);
extern int r_ble_ll_conn_hci_update(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                    uint8_t *rsplen);
extern int r_ble_ll_conn_hci_param_rr(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                      uint8_t *rsplen);
extern int r_ble_ll_conn_hci_param_nrr(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                       uint8_t *rsplen);
extern int r_ble_ll_conn_hci_set_data_len(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                          uint8_t *rsplen);
extern int r_ble_ll_hci_le_rd_sugg_data_len(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                            uint8_t *rsplen);
extern int r_ble_ll_hci_le_wr_sugg_data_len(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                            uint8_t *rsplen);
extern int r_ble_ll_hci_le_rd_max_data_len(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                           uint8_t *rsplen);
extern int r_ble_ll_conn_hci_le_start_encrypt(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                              uint8_t *rsplen);
extern int r_ble_ll_conn_hci_le_ltk_reply(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                          uint8_t *rsplen);
extern int r_ble_ll_conn_hci_le_ltk_neg_reply(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                              uint8_t *rsplen);
extern int r_ble_ll_conn_hci_le_rd_phy(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                       uint8_t *rsplen);
extern int r_ble_ll_hci_le_set_def_phy(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                       uint8_t *rsplen);
extern int r_ble_ll_conn_hci_le_set_phy(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                        uint8_t *rsplen);
extern int r_ble_ll_conn_hci_rd_chan_map(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                         uint8_t *rsplen);
extern int r_ble_ll_conn_hci_read_rem_features(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                               uint8_t *rsplen);
extern int r_ble_ll_hci_le_encrypt(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                   uint8_t *rsplen);
extern int r_ble_ll_conn_hci_set_chan_class(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                            uint8_t *rsplen);
extern int r_ble_ll_whitelist_read_size(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                        uint8_t *rsplen);
extern int api_acceptList_hci_clearList(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                        uint8_t *rsplen);
extern int api_acceptList_hci_addDevice(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                        uint8_t *rsplen);
extern int api_acceptList_hci_removeDevice(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                           uint8_t *rsplen);
extern int r_ble_ll_hci_le_rand(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                uint8_t *rsplen);
extern int r_ble_ll_hci_le_read_supp_states(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                            uint8_t *rsplen);
extern int r_ble_lll_hci_dtm_tx_test(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                     uint8_t *rsplen);
extern int r_ble_lll_hci_dtm_rx_test(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                     uint8_t *rsplen);
extern int r_ble_lll_dtm_end_test(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                  uint8_t *rsplen);
extern int r_ble_ll_resolv_list_add(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                    uint8_t *rsplen);
extern int r_ble_ll_resolv_list_rmv(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                    uint8_t *rsplen);
extern int r_ble_ll_hci_resolv_list_clr(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                        uint8_t *rsplen);
extern int r_ble_ll_resolv_list_read_size(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                          uint8_t *rsplen);
extern int r_ble_ll_resolv_peer_addr_rd(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                        uint8_t *rsplen);
extern int r_ble_ll_resolv_local_addr_rd(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                         uint8_t *rsplen);
extern int r_ble_ll_resolv_enable_cmd(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                      uint8_t *rsplen);
extern int r_ble_ll_resolv_set_rpa_tmo(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                       uint8_t *rsplen);
extern int r_ble_lll_hci_dtm_rx_test_v2(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                        uint8_t *rsplen);
extern int r_ble_lll_hci_dtm_tx_test_v2(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                        uint8_t *rsplen);
extern int r_ble_ll_adv_hci_set_random_addr(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                            uint8_t *rsplen);
extern int r_ble_ll_adv_ext_set_param(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                      uint8_t *rsplen);
extern int r_ble_ll_adv_ext_set_adv_data(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                         uint8_t *rsplen);
extern int r_ble_ll_adv_ext_set_scan_rsp(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                         uint8_t *rsplen);
extern int r_ble_ll_adv_ext_set_enable(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                       uint8_t *rsplen);
extern int r_ble_ll_adv_rd_max_adv_data_len(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                            uint8_t *rsplen);
extern int r_ble_ll_adv_rd_sup_adv_sets(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                        uint8_t *rsplen);
extern int r_ble_ll_adv_remove(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                               uint8_t *rsplen);
extern int r_ble_ll_adv_clear_all(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                  uint8_t *rsplen);
extern int r_ble_ll_adv_periodic_set_param(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                           uint8_t *rsplen);
extern int r_ble_ll_adv_periodic_set_data(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                          uint8_t *rsplen);
extern int r_ble_ll_adv_periodic_enable(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                        uint8_t *rsplen);
extern int r_ble_ll_adv_ext_set_param_v2(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                         uint8_t *rsplen);
extern int api_pawrBcast_hci_setSubevtData(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                       uint8_t *rsplen);
extern int api_pawrBcast_hci_extConnCreateV2(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                             uint8_t *rsplen);
extern int api_pawrBcast_hci_setPerAdvParamV2(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                              uint8_t *rsplen);
extern int api_pawrSync_hci_setRspData(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                   uint8_t *rsplen);
extern int api_pawrSync_hci_syncSubevt(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                   uint8_t *rsplen);
extern int r_ble_ll_set_ext_scan_params(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                        uint8_t *rsplen);
extern int r_ble_ll_hci_ext_scan_set_enable(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                            uint8_t *rsplen);
extern int r_ble_ll_ext_conn_create(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                    uint8_t *rsplen);
extern int r_ble_ll_sync_create(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                uint8_t *rsplen);
extern int r_ble_ll_sync_cancel(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                uint8_t *rsplen);
extern int r_ble_ll_sync_terminate(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                   uint8_t *rsplen);
extern int r_ble_ll_sync_list_add(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                  uint8_t *rsplen);
extern int r_ble_ll_sync_list_remove(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                     uint8_t *rsplen);
extern int r_ble_ll_sync_list_clear(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                    uint8_t *rsplen);
extern int r_ble_ll_sync_list_size(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                   uint8_t *rsplen);
extern int r_ble_ll_sync_receive_enable(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                        uint8_t *rsplen);
extern int r_ble_ll_df_connless_cte_tx_set_param(const uint8_t *cmdbuf, uint8_t len,
                                                 uint8_t *rspbuf, uint8_t *rsplen);
extern int r_ble_ll_df_connless_cte_tx_enable(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                              uint8_t *rsplen);
extern int r_ble_ll_df_connless_cte_rx_enable(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                              uint8_t *rsplen);
extern int r_ble_ll_df_conn_cte_rx_set_param(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                             uint8_t *rsplen);
extern int r_ble_ll_df_conn_cte_tx_set_param(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                             uint8_t *rsplen);
extern int r_ble_ll_df_conn_cte_req_enable(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                           uint8_t *rsplen);
extern int r_ble_ll_df_conn_cte_rsp_enable(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                           uint8_t *rsplen);
extern int r_ble_ll_df_read_antenna_info(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                         uint8_t *rsplen);
extern int r_ble_ll_read_tx_power(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                  uint8_t *rsplen);
extern int r_ble_ll_read_rf_path_compensation(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                              uint8_t *rsplen);
extern int r_ble_ll_write_rf_path_compensation(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                               uint8_t *rsplen);
extern int r_ble_ll_resolve_set_priv_mode(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                          uint8_t *rsplen);
extern int r_ble_ll_sync_transfer(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                  uint8_t *rsplen);
extern int r_ble_ll_adv_periodic_set_info_transfer(const uint8_t *cmdbuf, uint8_t len,
                                                   uint8_t *rspbuf, uint8_t *rsplen);
extern int r_ble_ll_set_sync_transfer_params(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                             uint8_t *rsplen);
extern int r_ble_ll_set_default_sync_transfer_params(const uint8_t *cmdbuf, uint8_t len,
                                                     uint8_t *rspbuf, uint8_t *rsplen);

extern int api_iso_hci_readIsoTxSync(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                     uint8_t *rsplen);

extern int api_cis_hci_setCigParams(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                    uint8_t *rsplen);

extern int api_cis_hci_createCis(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                 uint8_t *rsplen);

extern int api_cis_hci_removeCig(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                 uint8_t *rsplen);

extern int api_cis_hci_acceptCisReq(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                    uint8_t *rsplen);

extern int api_cis_hci_rejectCisReq(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                    uint8_t *rsplen);

extern int api_cis_hci_disconnect(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                  uint8_t *rsplen);

extern int api_bisBcast_hci_createBig(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                      uint8_t *rsplen);

extern int api_bisBcast_hci_terminateBig(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                         uint8_t *rsplen);

extern int api_bisSync_hci_bigCreateSync(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                         uint8_t *rsplen);

extern int api_bisSync_hci_bigTerminateSync(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                            uint8_t *rsplen);

extern int api_iso_hci_setupIsoDataPath(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                        uint8_t *rsplen);

extern int api_iso_hci_removeIsoDataPath(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                         uint8_t *rsplen);

extern int api_iso_hci_readBufSizeV2(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                     uint8_t *rsplen);

extern int api_iso_hci_readIsoLinkQuality(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                          uint8_t *rsplen);

extern int api_cis_hci_setCigParamsTest(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                        uint8_t *rsplen);

extern int api_bisBcast_hci_createBigTest(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                          uint8_t *rsplen);

extern int api_iso_hci_transmitTest(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                    uint8_t *rsplen);

extern int api_iso_hci_receiveTest(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                   uint8_t *rsplen);

extern int api_iso_hci_readTestCounter(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                       uint8_t *rsplen);

extern int api_iso_hci_testEnd(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                               uint8_t *rsplen);

extern int api_base_hci_setHostSuppFeat(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                        uint8_t *rsplen);
extern int extFeat_hci_setHostSuppFeat(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                       uint8_t *rsplen);
extern int r_ble_ll_adv_set_data_related_addr_change(const uint8_t *cmdbuf, uint8_t len,
                                                     uint8_t *rspbuf, uint8_t *rsplen);
extern int r_ble_ll_modify_sca(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                               uint8_t *rsplen);
extern int r_ble_ll_conn_req_peer_sca(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                      uint8_t *rsplen);
extern int r_ble_ll_read_local_p256_pub_key(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                            uint8_t *rsplen);
extern int r_ble_ll_generate_dh_key_v1(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                       uint8_t *rsplen);
extern int r_ble_ll_generate_dh_key_v2(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                       uint8_t *rsplen);
extern int r_ble_ll_conn_hci_read_transmit_power_level(const uint8_t *cmdbuf, uint8_t len,
                                                       uint8_t *rspbuf, uint8_t *rsplen);
extern int r_ble_ll_conn_hci_remote_read_transmit_power_level(const uint8_t *cmdbuf, uint8_t len,
                                                              uint8_t *rspbuf, uint8_t *rsplen);
extern int r_ble_ll_conn_hci_set_tx_power_enable(const uint8_t *cmdbuf, uint8_t len,
                                                 uint8_t *rspbuf, uint8_t *rsplen);
extern int r_ble_ll_conn_hci_set_path_loss_report_param(const uint8_t *cmdbuf, uint8_t len,
                                                        uint8_t *rspbuf, uint8_t *rsplen);
extern int r_ble_ll_conn_hci_set_path_loss_en(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                              uint8_t *rsplen);
extern int ble_ll_conn_hci_set_default_subrate(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                               uint8_t *rsplen);
extern int ble_ll_conn_hci_subrate_req(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                       uint8_t *rsplen);
#if UC_BT_CTRL_LE_LL_EXT_FEAT_SET
extern int extFeat_hci_readAllLocalSuppFeat(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                            uint8_t *rsplen);
extern int extFeat_hci_readAllRemoteSuppFeat(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                             uint8_t *rsplen);
#endif // UC_BT_CTRL_LE_LL_EXT_FEAT_SET
#if UC_BT_CTRL_LE_SHORTER_CONN_ITVL
extern int sci_hci_connRateReq(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                               uint8_t *rsplen);
extern int sci_hci_readMinSuppConnItvl(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                       uint8_t *rsplen);
extern int sci_hci_setDefaultRateParam(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                       uint8_t *rsplen);
#endif // UC_BT_CTRL_LE_SHORTER_CONN_ITVL
#if UC_BT_CTRL_LE_FRAME_SPACEING_UPDATE
extern int fsu_hci_frameSpaceUpdate(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                    uint8_t *rsplen);
#endif // UC_BT_CTRL_LE_FRAME_SPACEING_UPDATE
#if UC_BT_CTRL_LE_RANDOMIZED_RPA_UPDATES
extern int resolv_hci_leSetRpaTmoV2(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                    uint8_t *rsplen);
#endif // UC_BT_CTRL_LE_RANDOMIZED_RPA_UPDATES
#if UC_BT_CTRL_LE_DECISION_ADV
extern int decAdv_hci_leSetDecisionData(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                        uint8_t *rsplen);
#endif // UC_BT_CTRL_LE_DECISION_ADV
#if UC_BT_CTRL_LE_DECISION_SCAN
extern int decScan_hci_leSetDecisionInstructions(const uint8_t *cmdbuf, uint8_t len,
                                                 uint8_t *rspbuf, uint8_t *rsplen);
#endif // UC_BT_CTRL_LE_DECISION_SCAN
#if UC_BT_CTRL_LE_MONITOR_ADV
extern int monAdv_hci_addDevToMonAdvList(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                         uint8_t *rsplen);
extern int monAdv_hci_rmvDevFromMonAdvList(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                           uint8_t *rsplen);
extern int monAdv_hci_clrMonAdvList(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                    uint8_t *rsplen);
extern int monAdv_hci_readMonAdvListSize(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                         uint8_t *rsplen);
extern int monAdv_hci_enableMonAdv(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                   uint8_t *rsplen);
#endif // UC_BT_CTRL_LE_MONITOR_ADV
#if UC_BT_CTRL_LE_DTM_ENABLED
extern int dtm_hci_endTest(const uint8_t *cmdbuf, uint8_t len, uint8_t *rsp, uint8_t *rsplen);
extern int dtm_hci_rxTest(const uint8_t *cmdbuf, uint8_t len);
extern int dtm_hci_rxTestV2(const uint8_t *cmdbuf, uint8_t len);
extern int dtm_hci_txTest(const uint8_t *cmdbuf, uint8_t len);
extern int dtm_hci_txTestV2(const uint8_t *cmdbuf, uint8_t len);
extern int dtm_hci_rxTestV3(const uint8_t *cmdbuf, uint8_t len);
extern int dtm_hci_txTestV3(const uint8_t *cmdbuf, uint8_t len);
#endif // UC_BT_CTRL_LE_DTM_ENABLED
#if UC_BT_CTRL_LE_UTP_ENABLED
extern int utp_hci_enableUtpOtaMode(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                    uint8_t *rsplen);
extern int utp_hci_utpSend(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf, uint8_t *rsplen);
#endif // UC_BT_CTRL_LE_UTP_ENABLED

#if UC_BT_CTRL_LE_VS_CMD_EVT_ENABLED
extern int api_advFilter_hci_configDuplicateExceptionList(const uint8_t *cmdbuf, uint8_t len,
                                                          uint8_t *rspbuf, uint8_t *rsplen);
extern int api_ble_ll_scan_hci_set_adv_report_flow_ctrl(const uint8_t *cmdbuf, uint8_t len,
                                                        uint8_t *rspbuf, uint8_t *rsplen);
extern int api_ble_ll_scan_hci_update_adv_report_flow_ctrl(const uint8_t *cmdbuf, uint8_t len,
                                                           uint8_t *rspbuf, uint8_t *rsplen);
extern int api_adv_hci_clearLegacyAdv(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                      uint8_t *rsplen);
extern int api_advFilter_hci_setDuplicateParam(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                               uint8_t *rsplen);
extern int api_advFilter_hci_controlDuplicate(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                              uint8_t *rsplen);
extern int api_arr_hci_enable(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf, uint8_t *rsplen);
extern int api_arr_hci_setSchedLen(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                   uint8_t *rsplen);
extern int api_pcl_hci_setRssiThresh(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                     uint8_t *rsplen);
extern int api_ble_ll_hci_vs_csa_set(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                     uint8_t *rsplen);
extern int api_internalTest_hci_procVsCmds(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                           uint8_t *rsplen);
extern int api_ble_log_hci_vs_cmd_proc(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                       uint8_t *rsplen);
extern int api_hci_stack_setVsEventMask(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                        uint8_t *rsplen);
extern int api_conn_hci_setProtTxRxThresh(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                          uint8_t *rsplen);
extern int api_ble_ll_hci_cb_customize_peer_sca(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                                uint8_t *rsplen);
extern int api_adv_hci_disableAdvDelayVsHandler(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                                uint8_t *rsplen);
extern int api_cte_hci_getHwInfoParams(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                       uint8_t *rsplen);
extern int api_cteCal_hci_setRecvParams(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                        uint8_t *rsplen);
extern int api_cteCal_hci_setTransParams(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                         uint8_t *rsplen);
extern int api_cte_hci_setCalBbParams(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                      uint8_t *rsplen);
extern int api_cte_hci_setCalLcParams(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                      uint8_t *rsplen);
extern int api_cteCal_hci_setCtrlState(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                       uint8_t *rsplen);
extern int api_cteCal_hci_setTxData(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                    uint8_t *rsplen);
extern int api_phy_hci_codedPhySwitchVsHandler(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                               uint8_t *rsplen);
extern int api_scan_hci_scanForeverVsHandler(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                             uint8_t *rsplen);
extern int api_txPower_hci_setTxPowerLevelEnhancedVsHandler(const uint8_t *cmdbuf, uint8_t len,
                                                            uint8_t *rspbuf, uint8_t *rsplen);
extern int api_txPower_hci_getTxPowerLevelEnhancedVsHandler(const uint8_t *cmdbuf, uint8_t len,
                                                            uint8_t *rspbuf, uint8_t *rsplen);
extern int api_phy_hci_enableCCAVsHandler(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                          uint8_t *rsplen);
extern int api_phy_hci_setRxSensThresVsHandler(const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                               uint8_t *rsplen);
extern int api_scan_hci_setScanBackoffUpLimitMaxVsHandler(const uint8_t *cmdbuf, uint8_t len,
                                                          uint8_t *rspbuf, uint8_t *rsplen);
extern int api_extAdv_hci_setExtAdvConstantDidVsHandler(const uint8_t *cmdbuf, uint8_t len,
                                                        uint8_t *rspbuf, uint8_t *rsplen);
extern int api_scan_hci_setScanADIOnlyFilterVsHandler(const uint8_t *cmdbuf, uint8_t len,
                                                      uint8_t *rspbuf, uint8_t *rsplen);
#endif // UC_BT_CTRL_LE_VS_CMD_EVT_ENABLED

/*
 ***************************************************************************************************
 * Local Functions Declarations
 ***************************************************************************************************
 */
static int
hci_cmd_proc_link_control_cmds(uint16_t ocf, const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                               uint8_t *rsplen)
{
    int rc;
    uint16_t conn_handle;

    (void)conn_handle;
    (void)rspbuf;
    (void)rsplen;

    rc = HCI_CMD_RET_STATUS(HCI_CMD_ERR_UNKNOWN_HCI_CMD);

    switch (ocf) {
#if UC_BT_CTRL_BLE_IS_ENABLE
    case HCI_CMD_OCF_LINK_DISCONNECT:
        conn_handle = btdm_get_le16(cmdbuf);
#if UC_BT_CTRL_LE_CIS_ENABLED
        if (HCI_INTERNAL_CONN_IS_BLE_ISO(conn_handle)) {
            rc = api_cis_hci_disconnect(cmdbuf, len, rspbuf, rsplen);
            break;
        }
#endif //  UC_BT_CTRL_LE_CIS_ENABLED
        if (HCI_INTERNAL_CONN_IS_BLE_ACL(conn_handle)) {
            rc = r_ble_ll_hci_disconnect(cmdbuf, len);
            break;
        }
        rc = HCI_CMD_RET_STATUS(HCI_CMD_ERR_UNKNOWN_CONN_ID);
        break;
    case HCI_CMD_OCF_LINK_READ_RMT_VERSION_INFO:
        conn_handle = btdm_get_le16(cmdbuf);
        if (HCI_INTERNAL_CONN_IS_BLE_ACL(conn_handle)) {
            rc = r_ble_ll_conn_hci_rd_rem_ver_cmd(cmdbuf, len);
            break;
        }
        rc = HCI_CMD_RET_STATUS(HCI_CMD_ERR_UNKNOWN_CONN_ID);
        break;
#endif // UC_BT_CTRL_BLE_IS_ENABLE
    }

    return rc;
}

static int
hci_cmd_proc_link_policy_cmds(uint16_t ocf, const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                              uint8_t *rsplen)
{
    int rc;

    rc = HCI_CMD_RET_STATUS(HCI_CMD_ERR_UNKNOWN_HCI_CMD);

    return rc;
}

static int
hci_cmd_proc_controller_baseband_cmds(uint16_t ocf, const uint8_t *cmdbuf, uint8_t len,
                                      uint8_t *rspbuf, uint8_t *rsplen)
{
    int rc;
    uint16_t conn_handle;

    (void) conn_handle;
    rc = HCI_CMD_RET_STATUS(HCI_CMD_ERR_UNKNOWN_HCI_CMD);

    switch (ocf) {
    case HCI_CMD_OCF_BB_SET_EVENT_MASK:
        rc = r_btdm_hci_set_event_mask_handler(cmdbuf, len);
        break;
    case HCI_CMD_OCF_BB_RESET:
        rc = r_btdm_hci_reset_handler(cmdbuf, len);
        if (rc == 0) {
#if UC_BT_CTRL_BLE_IS_ENABLE
            ble_stack_reset();
#endif // UC_BT_CTRL_BLE_IS_ENABLE
        }
        break;
    case HCI_CMD_OCF_BB_SET_C2H_FLOW_CTRL:
        rc = r_btdm_c2hfc_set_flow_ctrl(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_BB_HOST_BUFFER_SIZE:
        rc = r_btdm_c2hfc_host_buffer_size(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_BB_HOST_NUM_PACKETS_DONE:
        rc = r_btdm_c2hfc_host_num_completed_packets(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_BB_SET_EVENT_MASK_PAGE_2:
        rc = r_btdm_hci_set_event_mask_page2_handler(cmdbuf, len);
        break;
#if UC_BT_CTRL_BLE_IS_ENABLE
    case HCI_CMD_OCF_BB_READ_AUTH_PYLD_TMO:
        conn_handle = btdm_get_le16(cmdbuf);
        if (HCI_INTERNAL_CONN_IS_BLE_ACL(conn_handle)) {
            rc = r_ble_ll_conn_hci_rd_auth_pyld_tmo(cmdbuf, len, rspbuf, rsplen);
            break;
        }
        rc = HCI_CMD_RET_COMPLETE(HCI_CMD_ERR_UNKNOWN_CONN_ID);
        *rsplen = 4;
        break;
    case HCI_CMD_OCF_BB_WRITE_AUTH_PYLD_TMO:
        conn_handle = btdm_get_le16(cmdbuf);
        if (HCI_INTERNAL_CONN_IS_BLE_ACL(conn_handle)) {
            rc = r_ble_ll_conn_hci_wr_auth_pyld_tmo(cmdbuf, len, rspbuf, rsplen);
            break;
        }
        rc = HCI_CMD_RET_COMPLETE(HCI_CMD_ERR_UNKNOWN_CONN_ID);
        *rsplen = 2;
        break;
#if UC_BT_CTRL_LE_ISO_ENABLED
    case HCI_CMD_OCF_BB_CONFIG_DATA_PATH:
        rc = r_ble_ll_hci_cb_cfg_data_path(cmdbuf, len);
        break;
#endif // UC_BT_CTRL_LE_ISO_ENABLED
#endif // UC_BT_CTRL_BLE_IS_ENABLE
#if UC_BT_CTRL_LE_FLUSHABLE_ACL_DATA
    // TODO: Define these commands in the ble files
    case HCI_CMD_OCF_BB_READ_AUTO_FLUSH_TOUT:
        conn_handle = btdm_get_le16(cmdbuf);
        if (HCI_INTERNAL_CONN_IS_BLE_ACL(conn_handle)) {
            rc = flushableAclData_hci_readAutoFlushTmo(cmdbuf, len, rspbuf, rsplen);
            break;
        }
        rc = HCI_CMD_RET_COMPLETE(HCI_CMD_ERR_UNKNOWN_CONN_ID);
        *rsplen = 4;
        break;
    case HCI_CMD_OCF_BB_WRITE_AUTO_FLUSH_TOUT:
        conn_handle = btdm_get_le16(cmdbuf);
        if (HCI_INTERNAL_CONN_IS_BLE_ACL(conn_handle)) {
            rc = flushableAclData_hci_writeAutoFlushTmo(cmdbuf, len, rspbuf, rsplen);
            break;
        }
        rc = HCI_CMD_RET_COMPLETE(HCI_CMD_ERR_UNKNOWN_CONN_ID);
        *rsplen = 2;
        break;
#endif // UC_BT_CTRL_LE_FLUSHABLE_ACL_DATA
    }

    return rc;
}

static int
hci_cmd_proc_informational_params_cmds(uint16_t ocf, const uint8_t *cmdbuf, uint8_t len,
                                       uint8_t *rspbuf, uint8_t *rsplen)
{
    int rc;

    rc = HCI_CMD_RET_STATUS(HCI_CMD_ERR_UNKNOWN_HCI_CMD);

    switch (ocf) {
    case HCI_CMD_OCF_INFO_READ_LOCAL_VERSION_INFO:
        rc = r_btdm_hci_rd_local_version_info_handler(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_INFO_READ_LOCAL_SUPPORTED_CMDS:
        rc = r_btdm_hci_rd_local_supported_cmds_handler(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_INFO_READ_LOCAL_FEATURES:
        rc = r_btdm_hci_rd_local_supp_features_handler(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_INFO_READ_LOCAL_EXT_FEATURES:
        rc = r_btdm_hci_rd_local_ext_features_handler(cmdbuf, len, rspbuf, rsplen);
        break;
    // case HCI_CMD_OCF_INFO_READ_BUFFER_SIZE:
    //     rc = r_ble_ll_hci_le_read_bufsize(cmdbuf, len, rspbuf, rsplen);
    //     break;
    // case HCI_CMD_OCF_INFO_READ_COUNTRY_CODE:
    //     rc = HCI_CMD_ERR_UNKNOWN_HCI_CMD;
    //     break;
    case HCI_CMD_OCF_INFO_READ_BD_ADDR:
        rc = r_btdm_hci_rd_bdaddr_handler(cmdbuf, len, rspbuf, rsplen);
        break;
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
    case HCI_CMD_OCF_INFO_READ_LOCAL_SUPPORTED_CODECS:
        rc = r_btdm_hci_rd_local_supp_codecs_handler(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_INFO_READ_LOCAL_SUPPORTED_CODEC_CAP:
        rc = r_btdm_hci_rd_local_supp_codec_cap_handler(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_INFO_READ_LOCAL_SUPPORTED_CTRL_DELAY:
        rc = r_btdm_hci_rd_local_supp_ctrl_delay_handler(cmdbuf, len, rspbuf, rsplen);
        break;
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE
#if UC_BT_CTRL_BR_EDR_IS_ENABLE || UC_BT_CTRL_LE_ISO_ENABLED
        case HCI_CMD_OCF_INFO_READ_LOCAL_SUPPORTED_CODECS_V2:
            rc = r_btdm_hci_rd_local_supp_codecs_v2_handler(cmdbuf, len, rspbuf, rsplen);
            break;
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE || UC_BT_CTRL_LE_ISO_ENABLED
    }

    return rc;
}

static int
hci_cmd_proc_status_params_cmds(uint16_t ocf, const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                uint8_t *rsplen)
{
    int rc;

    rc = HCI_CMD_RET_STATUS(HCI_CMD_ERR_UNKNOWN_HCI_CMD);

    switch (ocf) {
#if UC_BT_CTRL_BLE_IS_ENABLE
    case HCI_CMD_SP_OCF_READ_RSSI:
        uint16_t conn_handle = btdm_get_le16(cmdbuf);
        if (HCI_INTERNAL_CONN_IS_BLE_ACL(conn_handle)) {
            rc = r_ble_ll_conn_hci_rd_rssi(cmdbuf, len, rspbuf, rsplen);
            break;
        }
        rc = HCI_CMD_RET_COMPLETE(HCI_CMD_ERR_UNKNOWN_CONN_ID);
        *rsplen = 3;
        break;
#endif // UC_BT_CTRL_BLE_IS_ENABLE
    }

    return rc;
}

#if UC_BT_CTRL_BLE_IS_ENABLE
static int
hci_cmd_proc_le_cmds(uint16_t ocf, const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                     uint8_t *rsplen)
{
    int rc;

    rc = HCI_CMD_RET_STATUS(HCI_CMD_ERR_UNKNOWN_HCI_CMD);
    switch (ocf) {
    case HCI_CMD_OCF_LE_SET_EVENT_MASK:
        rc = r_ble_ll_hci_set_le_event_mask(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_RD_BUF_SIZE:
        rc = r_ble_ll_hci_le_read_bufsize(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_RD_LOC_SUPP_FEAT:
        rc = r_ble_ll_hci_le_read_local_features(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_RAND_ADDR:
        rc = r_ble_ll_set_random_addr(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_ADV_PARAMS:
        rc = r_ble_ll_adv_set_adv_params(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_RD_ADV_CHAN_TXPWR:
        rc = r_ble_ll_adv_read_txpwr(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_ADV_DATA:
        rc = r_ble_ll_hci_set_adv_data(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_SCAN_RSP_DATA:
        rc = r_ble_ll_hci_set_scan_rsp_data(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_ADV_ENABLE:
        rc = r_ble_ll_hci_adv_set_enable(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_SCAN_PARAMS:
        rc = r_ble_ll_scan_set_scan_params(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_SCAN_ENABLE:
        rc = r_ble_ll_hci_scan_set_enable(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_CREATE_CONN:
        rc = r_ble_ll_conn_create(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_CREATE_CONN_CANCEL:
        rc = r_ble_ll_conn_create_cancel(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_CONN_UPDATE:
        rc = r_ble_ll_conn_hci_update(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_REM_CONN_PARAM_RR:
        rc = r_ble_ll_conn_hci_param_rr(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_REM_CONN_PARAM_NRR:
        rc = r_ble_ll_conn_hci_param_nrr(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_DATA_LEN:
        rc = r_ble_ll_conn_hci_set_data_len(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_RD_SUGG_DEF_DATA_LEN:
        rc = r_ble_ll_hci_le_rd_sugg_data_len(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_WR_SUGG_DEF_DATA_LEN:
        rc = r_ble_ll_hci_le_wr_sugg_data_len(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_RD_MAX_DATA_LEN:
        rc = r_ble_ll_hci_le_rd_max_data_len(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_START_ENCRYPT:
        rc = r_ble_ll_conn_hci_le_start_encrypt(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_LT_KEY_REQ_REPLY:
        rc = r_ble_ll_conn_hci_le_ltk_reply(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_LT_KEY_REQ_NEG_REPLY:
        rc = r_ble_ll_conn_hci_le_ltk_neg_reply(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_RD_PHY:
        rc = r_ble_ll_conn_hci_le_rd_phy(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_DEFAULT_PHY:
        rc = r_ble_ll_hci_le_set_def_phy(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_PHY:
        rc = r_ble_ll_conn_hci_le_set_phy(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_RD_CHAN_MAP:
        rc = r_ble_ll_conn_hci_rd_chan_map(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_RD_REM_FEAT:
        rc = r_ble_ll_conn_hci_read_rem_features(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_ENCRYPT:
        rc = r_ble_ll_hci_le_encrypt(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_HOST_CHAN_CLASS:
        rc = r_ble_ll_conn_hci_set_chan_class(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_RD_WHITE_LIST_SIZE:
        rc = r_ble_ll_whitelist_read_size(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_CLEAR_WHITE_LIST:
        rc = api_acceptList_hci_clearList(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_ADD_WHITE_LIST:
        rc = api_acceptList_hci_addDevice(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_RMV_WHITE_LIST:
        rc = api_acceptList_hci_removeDevice(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_RAND:
        rc = r_ble_ll_hci_le_rand(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_RD_SUPP_STATES:
        rc = r_ble_ll_hci_le_read_supp_states(cmdbuf, len, rspbuf, rsplen);
        break;
#if UC_BT_CTRL_LE_DTM_ENABLED
    case HCI_CMD_OCF_LE_TX_TEST:
        rc = dtm_hci_txTest(cmdbuf, len);
        break;
    case HCI_CMD_OCF_LE_RX_TEST:
        rc = dtm_hci_rxTest(cmdbuf, len);
        break;
    case HCI_CMD_OCF_LE_TEST_END:
        rc = dtm_hci_endTest(cmdbuf, len, rspbuf, rsplen);
        break;
#endif /* UC_BT_CTRL_LE_DTM_ENABLED */
    case HCI_CMD_OCF_LE_ADD_RESOLV_LIST:
        rc = r_ble_ll_resolv_list_add(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_RMV_RESOLV_LIST:
        rc = r_ble_ll_resolv_list_rmv(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_CLR_RESOLV_LIST:
        rc = r_ble_ll_hci_resolv_list_clr(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_RD_RESOLV_LIST_SIZE:
        rc = r_ble_ll_resolv_list_read_size(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_RD_PEER_RESOLV_ADDR:
        rc = r_ble_ll_resolv_peer_addr_rd(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_RD_LOCAL_RESOLV_ADDR:
        rc = r_ble_ll_resolv_local_addr_rd(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_ADDR_RES_EN:
        rc = r_ble_ll_resolv_enable_cmd(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_RPA_TMO:
        rc = r_ble_ll_resolv_set_rpa_tmo(cmdbuf, len, rspbuf, rsplen);
        break;
#if UC_BT_CTRL_LE_DTM_ENABLED
    case HCI_CMD_OCF_LE_RX_TEST_V2:
        rc = dtm_hci_rxTestV2(cmdbuf, len);
        break;
    case HCI_CMD_OCF_LE_TX_TEST_V2:
        rc = dtm_hci_txTestV2(cmdbuf, len);
        break;
    case HCI_CMD_OCF_LE_RX_TEST_V3:
        rc = dtm_hci_rxTestV3(cmdbuf, len);
        break;
    case HCI_CMD_OCF_LE_TX_TEST_V3:
        rc = dtm_hci_txTestV3(cmdbuf, len);
        break;
#endif /* UC_BT_CTRL_LE_DTM_ENABLED */
    case HCI_CMD_OCF_LE_SET_ADV_SET_RND_ADDR:
        rc = r_ble_ll_adv_hci_set_random_addr(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_EXT_ADV_PARAM:
        rc = r_ble_ll_adv_ext_set_param(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_EXT_ADV_DATA:
        rc = r_ble_ll_adv_ext_set_adv_data(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_EXT_SCAN_RSP_DATA:
        rc = r_ble_ll_adv_ext_set_scan_rsp(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_EXT_ADV_ENABLE:
        rc = r_ble_ll_adv_ext_set_enable(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_RD_MAX_ADV_DATA_LEN:
        rc = r_ble_ll_adv_rd_max_adv_data_len(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_RD_NUM_OF_ADV_SETS:
        rc = r_ble_ll_adv_rd_sup_adv_sets(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_REMOVE_ADV_SET:
        rc = r_ble_ll_adv_remove(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_CLEAR_ADV_SETS:
        rc = r_ble_ll_adv_clear_all(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_PERIODIC_ADV_PARAMS:
        rc = r_ble_ll_adv_periodic_set_param(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_PERIODIC_ADV_DATA:
        rc = r_ble_ll_adv_periodic_set_data(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_PERIODIC_ADV_ENABLE:
        rc = r_ble_ll_adv_periodic_enable(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_EXT_ADV_PARAM_V2:
        rc = r_ble_ll_adv_ext_set_param_v2(cmdbuf, len, rspbuf, rsplen);
        break;
#if UC_BLE_CTRL_PAWR_BCAST_SUPPORTED
    case HCI_CMD_OCF_LE_SET_PERIODIC_ADV_SUBEV_DATA:
        rc = api_pawrBcast_hci_setSubevtData(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_EXT_CREATE_CONN_V2:
        rc = api_pawrBcast_hci_extConnCreateV2(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_PERIODIC_ADV_PARAMS_V2:
        rc = api_pawrBcast_hci_setPerAdvParamV2(cmdbuf, len, rspbuf, rsplen);
        break;
#endif /* UC_BLE_CTRL_PAWR_BCAST_SUPPORTED */
#if UC_BLE_CTRL_PAWR_SYNC_SUPPORTED
    case HCI_CMD_OCF_LE_SET_PERIODIC_ADV_RESPONSE_DATA:
        rc = api_pawrSync_hci_setRspData(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_PERIODIC_ADV_SYNC_SUBEVENT:
        rc = api_pawrSync_hci_syncSubevt(cmdbuf, len, rspbuf, rsplen);
        break;
#endif /* UC_BLE_CTRL_PAWR_SYNC_SUPPORTED */
    case HCI_CMD_OCF_LE_SET_EXT_SCAN_PARAM:
        rc = r_ble_ll_set_ext_scan_params(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_EXT_SCAN_ENABLE:
        rc = r_ble_ll_hci_ext_scan_set_enable(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_EXT_CREATE_CONN:
        rc = r_ble_ll_ext_conn_create(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_PERIODIC_ADV_CREATE_SYNC:
        rc = r_ble_ll_sync_create(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_PERIODIC_ADV_CREATE_SYNC_CANCEL:
        rc = r_ble_ll_sync_cancel(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_PERIODIC_ADV_TERM_SYNC:
        rc = r_ble_ll_sync_terminate(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_ADD_DEV_TO_PERIODIC_ADV_LIST:
        rc = r_ble_ll_sync_list_add(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_REM_DEV_FROM_PERIODIC_ADV_LIST:
        rc = r_ble_ll_sync_list_remove(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_CLEAR_PERIODIC_ADV_LIST:
        rc = r_ble_ll_sync_list_clear(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_RD_PERIODIC_ADV_LIST_SIZE:
        rc = r_ble_ll_sync_list_size(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_PERIODIC_ADV_RECEIVE_ENABLE:
        rc = r_ble_ll_sync_receive_enable(cmdbuf, len, rspbuf, rsplen);
        break;
#if UC_BT_CTRL_LE_CTE_ENABLED
    case HCI_CMD_OCF_LE_SET_CONNLESS_CTE_TX_PARAMS:
        rc = r_ble_ll_df_connless_cte_tx_set_param(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_CONNLESS_CTE_TX_ENABLE:
        rc = r_ble_ll_df_connless_cte_tx_enable(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_CONNLESS_IQ_SAMPLING_ENABLE:
        rc = r_ble_ll_df_connless_cte_rx_enable(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_CONN_CTE_RX_PARAMS:
        rc = r_ble_ll_df_conn_cte_rx_set_param(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_CONN_CTE_TX_PARAMS:
        rc = r_ble_ll_df_conn_cte_tx_set_param(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_CONN_CTE_REQ_ENABLE:
        rc = r_ble_ll_df_conn_cte_req_enable(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_CONN_CTE_RESP_ENABLE:
        rc = r_ble_ll_df_conn_cte_rsp_enable(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_RD_ANTENNA_INFO:
        rc = r_ble_ll_df_read_antenna_info(cmdbuf, len, rspbuf, rsplen);
        break;
#endif /* UC_BT_CTRL_LE_CTE_ENABLED */
    case HCI_CMD_OCF_LE_RD_TRANSMIT_POWER:
        rc = r_ble_ll_read_tx_power(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_RD_RF_PATH_COMPENSATION:
        rc = r_ble_ll_read_rf_path_compensation(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_WR_RF_PATH_COMPENSATION:
        rc = r_ble_ll_write_rf_path_compensation(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_PRIVACY_MODE:
        rc = r_ble_ll_resolve_set_priv_mode(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_PERIODIC_ADV_SYNC_TRANSFER:
        rc = r_ble_ll_sync_transfer(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_PERIODIC_ADV_SET_INFO_TRANSFER:
        rc = r_ble_ll_adv_periodic_set_info_transfer(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_PERIODIC_ADV_SYNC_TRANSFER_PARAMS:
        rc = r_ble_ll_set_sync_transfer_params(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_DEFAULT_SYNC_TRANSFER_PARAMS:
        rc = r_ble_ll_set_default_sync_transfer_params(cmdbuf, len, rspbuf, rsplen);
        break;
#if UC_BT_CTRL_LE_ISO_ENABLED
    case HCI_CMD_OCF_LE_READ_ISO_TX_SYNC:
        rc = api_iso_hci_readIsoTxSync(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_CIG_PARAMS:
        rc = api_cis_hci_setCigParams(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_CREATE_CIS:
        rc = api_cis_hci_createCis(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_REMOVE_CIG:
        rc = api_cis_hci_removeCig(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_ACCEPT_CIS_REQ:
        rc = api_cis_hci_acceptCisReq(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_REJECT_CIS_REQ:
        rc = api_cis_hci_rejectCisReq(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_CREATE_BIG:
        rc = api_bisBcast_hci_createBig(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_TERMINATE_BIG:
        rc = api_bisBcast_hci_terminateBig(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_BIG_CREATE_SYNC:
        rc = api_bisSync_hci_bigCreateSync(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_BIG_TERMINATE_SYNC:
        rc = api_bisSync_hci_bigTerminateSync(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SETUP_ISO_DATA_PATH:
        rc = api_iso_hci_setupIsoDataPath(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_REMOVE_ISO_DATA_PATH:
        rc = api_iso_hci_removeIsoDataPath(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_RD_BUF_SIZE_V2:
        rc = api_iso_hci_readBufSizeV2(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_READ_ISO_LINK_QUALITY:
        rc = api_iso_hci_readIsoLinkQuality(cmdbuf, len, rspbuf, rsplen);
        break;
#if UC_BT_CTRL_LE_ISO_TEST_ENABLED
    case HCI_CMD_OCF_LE_SET_CIG_PARAMS_TEST:
        rc = api_cis_hci_setCigParamsTest(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_CREATE_BIG_TEST:
        rc = api_bisBcast_hci_createBigTest(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_ISO_TRANSMIT_TEST:
        rc = api_iso_hci_transmitTest(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_ISO_RECEIVE_TEST:
        rc = api_iso_hci_receiveTest(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_ISO_READ_TEST_COUNTERS:
        rc = api_iso_hci_readTestCounter(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_ISO_TEST_END:
        rc = api_iso_hci_testEnd(cmdbuf, len, rspbuf, rsplen);
        break;
#endif /* UC_BT_CTRL_LE_ISO_TEST_ENABLED */
#endif /* UC_BT_CTRL_LE_ISO_ENABLED */
    case HCI_CMD_OCF_LE_SET_HOST_FEATURE:
        rc = api_base_hci_setHostSuppFeat(cmdbuf, len, rspbuf, rsplen);
        break;
#if UC_BT_CTRL_LE_LL_EXT_FEAT_SET
    case HCI_CMD_OCF_LE_SET_HOST_FEATURE_V2:
        rc = extFeat_hci_setHostSuppFeat(cmdbuf, len, rspbuf, rsplen);
        break;
#endif // UC_BT_CTRL_LE_LL_EXT_FEAT_SET
    case HCI_CMD_OCF_LE_SET_DATA_ADDR_CHANGE:
        rc = r_ble_ll_adv_set_data_related_addr_change(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_MODIFY_SCA:
        rc = r_ble_ll_modify_sca(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_REQ_PEER_SCA:
        rc = r_ble_ll_conn_req_peer_sca(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_RD_P256_PUBKEY:
        rc = r_ble_ll_read_local_p256_pub_key(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_GEN_DHKEY:
        rc = r_ble_ll_generate_dh_key_v1(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_GENERATE_DHKEY_V2:
        rc = r_ble_ll_generate_dh_key_v2(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_ENH_READ_TRANSMIT_POWER_LEVEL:
        rc = r_ble_ll_conn_hci_read_transmit_power_level(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_READ_REMOTE_TRANSMIT_POWER_LEVEL:
        rc = r_ble_ll_conn_hci_remote_read_transmit_power_level(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_TRANS_PWR_REPORT_ENABLE:
        rc = r_ble_ll_conn_hci_set_tx_power_enable(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_PATH_LOSS_REPORT_PARAM:
        rc = r_ble_ll_conn_hci_set_path_loss_report_param(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_PATH_LOSS_REPORT_ENABLE:
        rc = r_ble_ll_conn_hci_set_path_loss_en(cmdbuf, len, rspbuf, rsplen);
        break;
#if UC_BLE_CTRL_SUBRATE_ENABLED
    case HCI_CMD_OCF_LE_SET_DEFAULT_SUBRATE:
        rc = ble_ll_conn_hci_set_default_subrate(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SUBRATE_REQ:
        rc = ble_ll_conn_hci_subrate_req(cmdbuf, len, rspbuf, rsplen);
        break;
#endif /* UC_BLE_CTRL_SUBRATE_ENABLED */
#if UC_BT_CTRL_LE_LL_EXT_FEAT_SET
    case HCI_CMD_OCF_LE_RD_ALL_LOCAL_SUP_FEAT:
        rc = extFeat_hci_readAllLocalSuppFeat(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_RD_ALL_REMOTE_SUP_FEAT:
        rc = extFeat_hci_readAllRemoteSuppFeat(cmdbuf, len, rspbuf, rsplen);
        break;
#endif // UC_BT_CTRL_LE_LL_EXT_FEAT_SET
#if UC_BT_CTRL_LE_SHORTER_CONN_ITVL
    case HCI_CMD_OCF_LE_CONN_RATE_REQ:
        rc = sci_hci_connRateReq(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_SET_DEFAULT_RATE_PARAM:
        rc = sci_hci_setDefaultRateParam(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_READ_MIN_SUPP_CONN_ITVL:
        rc = sci_hci_readMinSuppConnItvl(cmdbuf, len, rspbuf, rsplen);
        break;
#endif // UC_BT_CTRL_LE_SHORTER_CONN_ITVL
#if UC_BT_CTRL_LE_FRAME_SPACEING_UPDATE
    case HCI_CMD_OCF_LE_FRAME_SPACE_UPDATE:
        rc = fsu_hci_frameSpaceUpdate(cmdbuf, len, rspbuf, rsplen);
        break;
#endif // UC_BT_CTRL_LE_FRAME_SPACEING_UPDATE
#if UC_BT_CTRL_LE_RANDOMIZED_RPA_UPDATES
    case HCI_CMD_OCF_LE_SET_RESOLV_PRIV_ADDR_TIMEOUT_V2:
        rc = resolv_hci_leSetRpaTmoV2(cmdbuf, len, rspbuf, rsplen);
        break;
#endif // UC_BT_CTRL_LE_RANDOMIZED_RPA_UPDATES
#if UC_BT_CTRL_LE_DECISION_ADV
    case HCI_CMD_OCF_LE_SET_DECISION_DATA:
        rc = decAdv_hci_leSetDecisionData(cmdbuf, len, rspbuf, rsplen);
        break;
#endif // UC_BT_CTRL_LE_DECISION_ADV
#if UC_BT_CTRL_LE_DECISION_SCAN
    case HCI_CMD_OCF_LE_SET_DECISION_INSTRUCTIONS:
        rc = decScan_hci_leSetDecisionInstructions(cmdbuf, len, rspbuf, rsplen);
        break;
#endif // UC_BT_CTRL_LE_DECISION_SCAN
#if UC_BT_CTRL_LE_MONITOR_ADV
    case HCI_CMD_OCF_LE_ADD_DEV_TO_MONITORED_ADV_LIST:
        rc = monAdv_hci_addDevToMonAdvList(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_RMV_DEV_FROM_MONITORED_ADV_LIST:
        rc = monAdv_hci_rmvDevFromMonAdvList(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_CLEAR_MONITORED_ADV_LIST:
        rc = monAdv_hci_clrMonAdvList(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_RD_MONITORED_ADV_LIST_SIZE:
        rc = monAdv_hci_readMonAdvListSize(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_ENABLE_MONITORING_ADV:
        rc = monAdv_hci_enableMonAdv(cmdbuf, len, rspbuf, rsplen);
        break;
#endif // UC_BT_CTRL_LE_MONITOR_ADV
#if UC_BT_CTRL_LE_UTP_ENABLED
    case HCI_CMD_OCF_LE_ENABLE_UTP_OTA_MODDE:
        rc = utp_hci_enableUtpOtaMode(cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OCF_LE_UTP_SEND:
        rc = utp_hci_utpSend(cmdbuf, len, rspbuf, rsplen);
        break;
#endif // UC_BT_CTRL_LE_UTP_ENABLED
    }

    return rc;
}

static int
hci_cmd_proc_vendor_specific_cmds(uint16_t ocf, const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                                  uint8_t *rsplen)
{
    int rc;

    rc = HCI_CMD_RET_STATUS(HCI_CMD_ERR_UNKNOWN_HCI_CMD);

    switch (ocf) {
#if UC_BT_CTRL_LE_VS_CMD_EVT_ENABLED
    case ESP_BT_VS_CONFIG_DUP_EXC_LIST_OCF:
        rc = api_advFilter_hci_configDuplicateExceptionList(cmdbuf, len, rspbuf, rsplen);
        break;
    case ESP_BT_VS_SET_ADV_REPORT_FLOW_CTRL_OCF:
        rc = api_ble_ll_scan_hci_set_adv_report_flow_ctrl(cmdbuf, len, rspbuf, rsplen);
        break;
    case ESP_BT_VS_UPD_ADV_REPORT_FLOW_CTRL_NUM_OCF:
        rc = api_ble_ll_scan_hci_update_adv_report_flow_ctrl(cmdbuf, len, rspbuf, rsplen);
        break;
    case ESP_BT_VS_CLR_LEGACY_ADV_OCF:
        rc = api_adv_hci_clearLegacyAdv(cmdbuf, len, rspbuf, rsplen);
        break;
    case ESP_BT_VS_SET_DUP_LIST_PARAMS_OCF:
        rc = api_advFilter_hci_setDuplicateParam(cmdbuf, len, rspbuf, rsplen);
        break;
    case ESP_BT_VS_ENABLE_DUP_EXC_LIST_OCF:
        rc = api_advFilter_hci_controlDuplicate(cmdbuf, len, rspbuf, rsplen);
        break;
    case ESP_BT_VS_ENABLE_ARRANGEMENT_OCF:
        rc = api_arr_hci_enable(cmdbuf, len, rspbuf, rsplen);
        break;
    case ESP_BT_VS_SET_SCHED_ROLE_LEN_OCF:
        rc = api_arr_hci_setSchedLen(cmdbuf, len, rspbuf, rsplen);
        break;
    case ESP_BT_VS_SET_PCL_RSSI_THRESH_OCF:
        rc = api_pcl_hci_setRssiThresh(cmdbuf, len, rspbuf, rsplen);
        break;
    case ESP_BT_VS_ENABLE_CSA2_OCF:
        rc = api_ble_ll_hci_vs_csa_set(cmdbuf, len, rspbuf, rsplen);
        break;
    case ESP_BT_VS_CFG_TEST_RELATED_OCF:
        rc = api_internalTest_hci_procVsCmds(cmdbuf, len, rspbuf, rsplen);
        break;
    case ESP_BT_VS_SET_LOG_PARAMS_OCF:
        rc = api_ble_log_hci_vs_cmd_proc(cmdbuf, len, rspbuf, rsplen);
        break;
    case ESP_BT_VS_SET_LE_VENDOR_EVTS_MASK_OCF:
        rc = api_hci_stack_setVsEventMask(cmdbuf, len, rspbuf, rsplen);
        break;
    case ESP_BT_VS_SET_CONN_PROT_TX_RX_THRESH_OCF:
        rc = api_conn_hci_setProtTxRxThresh(cmdbuf, len, rspbuf, rsplen);
        break;
    case ESP_BT_VS_SET_CONST_PEER_SCA_OCF:
        rc = api_ble_ll_hci_cb_customize_peer_sca(cmdbuf, len, rspbuf, rsplen);
        break;
    case ESP_BT_VS_DISABLE_ADV_RANDOM_DELAY_OCF:
        rc = api_adv_hci_disableAdvDelayVsHandler(cmdbuf, len, rspbuf, rsplen);
        break;
#if UC_BT_CTRL_LE_CTE_CAL_ENABLED
    case ESP_BT_VS_GET_CTE_HW_INFO:
        rc = api_cte_hci_getHwInfoParams(cmdbuf, len, rspbuf, rsplen);
        break;
    case ESP_BT_VS_SET_CTE_CAL_RECV_PARAM:
        rc = api_cteCal_hci_setRecvParams(cmdbuf, len, rspbuf, rsplen);
        break;
    case ESP_BT_VS_SET_CTE_CAL_TRANS_PARAM:
        rc = api_cteCal_hci_setTransParams(cmdbuf, len, rspbuf, rsplen);
        break;
    case ESP_BT_VS_SET_CTE_CAL_BB_PARAM:
        rc = api_cte_hci_setCalBbParams(cmdbuf, len, rspbuf, rsplen);
        break;
    case ESP_BT_VS_SET_CTE_CAL_LC_PARAM:
        rc = api_cte_hci_setCalLcParams(cmdbuf, len, rspbuf, rsplen);
        break;
    case ESP_BT_VS_SET_CTE_CAL_STATE:
        rc = api_cteCal_hci_setCtrlState(cmdbuf, len, rspbuf, rsplen);
        break;
    case ESP_BT_VS_SET_CTE_CAL_TX_DATA:
        rc = api_cteCal_hci_setTxData(cmdbuf, len, rspbuf, rsplen);
        break;
#endif // UC_BT_CTRL_LE_CTE_CAL_ENABLED
    case ESP_BT_VS_SET_PREFERRED_CODED_PHY_OCF:
        rc = api_phy_hci_codedPhySwitchVsHandler(cmdbuf, len, rspbuf, rsplen);
        break;
    case ESP_BT_VS_SET_SCAN_FOREVER_OCF:
        rc = api_scan_hci_scanForeverVsHandler(cmdbuf, len, rspbuf, rsplen);
        break;
    case ESP_BT_VS_SET_TXPWR_LVL_ENH_OCF:
        rc = api_txPower_hci_setTxPowerLevelEnhancedVsHandler(cmdbuf, len, rspbuf, rsplen);
        break;
    case ESP_BT_VS_GET_TXPWR_LVL_ENH_OCF:
        rc = api_txPower_hci_getTxPowerLevelEnhancedVsHandler(cmdbuf, len, rspbuf, rsplen);
        break;
    case ESP_BT_VS_ENABLE_CCA_OCF:
        rc = api_phy_hci_enableCCAVsHandler(cmdbuf, len, rspbuf, rsplen);
        break;
    case ESP_BT_VS_SET_RX_SENS_THRESH_OCF:
        rc = api_phy_hci_setRxSensThresVsHandler(cmdbuf, len, rspbuf, rsplen);
        break;
    case ESP_BT_VS_SET_SCAN_BACKOFF_UPLIMIT_OCF:
        rc = api_scan_hci_setScanBackoffUpLimitMaxVsHandler(cmdbuf, len, rspbuf, rsplen);
        break;
#if UC_BT_CTRL_LE_VS_CONST_EXT_ADV_DID
    case ESP_BT_VS_SET_EXT_ADV_CONSTANT_DID_OCF:
        rc = api_extAdv_hci_setExtAdvConstantDidVsHandler(cmdbuf, len, rspbuf, rsplen);
        break;
#endif // UC_BT_CTRL_LE_VS_CONST_EXT_ADV_DID
#if UC_BT_CTRL_LE_VS_ADI_FILTER_ENABLED
    case ESP_BT_VS_SET_SCAN_ADI_FILTER_OCF:
        rc = api_scan_hci_setScanADIOnlyFilterVsHandler(cmdbuf, len, rspbuf, rsplen);
        break;
#endif // UC_BT_CTRL_LE_VS_ADI_FILTER_ENABLED
#endif // UC_BT_CTRL_LE_VS_CMD_EVT_ENABLED
    default:
        extern int r_ble_ll_hci_vs_cmd_proc(const uint8_t *cmdbuf, uint8_t len, uint16_t ocf,
                                            uint8_t *rspbuf, uint8_t *rsplen);
        rc = r_ble_ll_hci_vs_cmd_proc(cmdbuf, len, ocf, rspbuf, rsplen);
        break;
    }

    return rc;
}
#endif // UC_BT_CTRL_BLE_IS_ENABLE

static int
hci_cmd_proc_handler(uint16_t opcode, const uint8_t *cmdbuf, uint8_t len, uint8_t *rspbuf,
                     uint8_t *rsplen)
{
    int rc;
    uint16_t ocf;

    rc = HCI_CMD_RET_STATUS(HCI_CMD_ERR_UNKNOWN_HCI_CMD);
    ocf = HCI_CMD_OCF(opcode);

    switch (HCI_CMD_OGF(opcode)) {
    case HCI_CMD_OGF_LINK_CONTROL_CMDS:
        rc = hci_cmd_proc_link_control_cmds(ocf, cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OGF_LINK_POLICY_CMDS:
        rc = hci_cmd_proc_link_policy_cmds(ocf, cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OGF_HOST_CONT_BASEBAND_CMDS:
        rc = hci_cmd_proc_controller_baseband_cmds(ocf, cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OGF_INFORMATIONAL_PARAMS:
        rc = hci_cmd_proc_informational_params_cmds(ocf, cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OGF_STATUS_PARAMS:
        rc = hci_cmd_proc_status_params_cmds(ocf, cmdbuf, len, rspbuf, rsplen);
        break;
#if UC_BT_CTRL_BLE_IS_ENABLE
    case HCI_CMD_OGF_BLE_CMDS:
        rc = hci_cmd_proc_le_cmds(ocf, cmdbuf, len, rspbuf, rsplen);
        break;
    case HCI_CMD_OGF_VENDOR_SPECIFIC:
        rc = hci_cmd_proc_vendor_specific_cmds(ocf, cmdbuf, len, rspbuf, rsplen);
        break;
#endif // UC_BT_CTRL_BLE_IS_ENABLE
    }

#if UC_BT_CTRL_BR_EDR_IS_ENABLE
    int err = HCI_CMD_RET_ERR_CODE(rc);

    if ((err == HCI_CMD_ERR_UNKNOWN_HCI_CMD) || (err == HCI_CMD_ERR_UNKNOWN_CONN_ID)) {
        rc = bredr_hci_cmd_rx_handler(opcode, cmdbuf, len, rspbuf, rsplen);
    }
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE

    return rc;
}

/*
 ***************************************************************************************************
 * Public Functions
 ***************************************************************************************************
 */
int
hci_cmd_proc_init(void)
{
    r_btdm_hci_set_cmd_handler_func(hci_cmd_proc_handler);
    return 0;
}
