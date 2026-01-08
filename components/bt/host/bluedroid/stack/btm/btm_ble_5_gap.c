/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "btm_int.h"
#include "stack/hcimsgs.h"
#include "osi/allocator.h"
#include "device/controller.h"
#include <string.h>
#include "l2c_int.h"
#if (BLE_50_FEATURE_SUPPORT == TRUE)
#define SET_BIT(t, n)  (t |= 1UL << (n))
tBTM_BLE_EXTENDED_CB extend_adv_cb;

tBTM_BLE_5_HCI_CBACK ble_5_hci_cb;

#define INVALID_VALUE   0XFF

extern BOOLEAN BTM_GetLocalResolvablePrivateAddr(BD_ADDR bda);
extern void BTM_UpdateAddrInfor(uint8_t addr_type, BD_ADDR bda);
extern void BTM_BleSetStaticAddr(BD_ADDR rand_addr);
extern uint32_t BTM_BleUpdateOwnType(uint8_t *own_bda_type, tBTM_START_ADV_CMPL_CBACK *cb);
#if (BLE_50_EXTEND_ADV_EN == TRUE)
static tBTM_STATUS btm_ble_ext_adv_params_validate(tBTM_BLE_GAP_EXT_ADV_PARAMS *params);
static tBTM_STATUS btm_ble_ext_adv_set_data_validate(UINT8 instance, UINT16 len, UINT8 *data);

typedef struct {
    uint16_t ter_con_handle;
    bool invalid;
    bool enabled;
    UINT8 instance;
    int duration;
    int max_events;
    uint8_t retry_count;
} tBTM_EXT_ADV_RECORD;

tBTM_EXT_ADV_RECORD adv_record[MAX_BLE_ADV_INSTANCE] = {0};
#endif // #if (BLE_50_EXTEND_ADV_EN == TRUE)

#if (BLE_50_EXTEND_SYNC_EN == TRUE)
#if (BLE_FEAT_CREATE_SYNC_RETRY_MAX > 0)
/* Create sync retry control block */
typedef struct {
    bool in_use;                                /* Whether sync creation is in progress */
    uint8_t retry_count;                        /* Current retry count */
    tBTM_BLE_Periodic_Sync_Params params;       /* Saved sync parameters for retry */
} tBTM_BLE_SYNC_RETRY_CB;

static tBTM_BLE_SYNC_RETRY_CB sync_retry_cb = {0};
#endif // #if (BLE_FEAT_CREATE_SYNC_RETRY_MAX > 0)
#endif // #if (BLE_50_EXTEND_SYNC_EN == TRUE)

extern void btm_ble_inter_set(bool extble_inter);

#if !UC_BT_STACK_NO_LOG
static const char *btm_ble_hci_status_to_str(tHCI_STATUS status)
{
    switch(status) {
    case HCI_SUCCESS:
        return "HCI_SUCCESS";
    case HCI_ERR_ILLEGAL_COMMAND:
        return "HCI_ERR_ILLEGAL_COMMAND";
    case HCI_ERR_NO_CONNECTION:
        return "HCI_ERR_NO_CONNECTION";
    case HCI_ERR_HW_FAILURE:
        return "HCI_ERR_HW_FAILURE";
    case HCI_ERR_PAGE_TIMEOUT:
        return "HCI_ERR_PAGE_TIMEOUT";
    case HCI_ERR_AUTH_FAILURE:
        return "HCI_ERR_AUTH_FAILURE";
    case HCI_ERR_KEY_MISSING:
        return "HCI_ERR_KEY_MISSING";
    case HCI_ERR_MEMORY_FULL:
        return "HCI_ERR_MEMORY_FULL";
    case HCI_ERR_CONNECTION_TOUT:
        return "HCI_ERR_CONNECTION_TOUT";
    case HCI_ERR_MAX_NUM_OF_CONNECTIONS:
        return "HCI_ERR_MAX_NUM_OF_CONNECTIONS";
    case HCI_ERR_MAX_NUM_OF_SCOS:
        return "HCI_ERR_MAX_NUM_OF_SCOS";
    case HCI_ERR_CONNECTION_EXISTS:
        return "HCI_ERR_CONNECTION_EXISTS";
    case HCI_ERR_COMMAND_DISALLOWED:
        return "HCI_ERR_COMMAND_DISALLOWED";
    case HCI_ERR_HOST_REJECT_RESOURCES:
        return "HCI_ERR_HOST_REJECT_RESOURCES";
    case HCI_ERR_HOST_REJECT_SECURITY:
        return "HCI_ERR_HOST_REJECT_SECURITY";
    case HCI_ERR_HOST_REJECT_DEVICE:
        return "HCI_ERR_HOST_REJECT_DEVICE";
    case HCI_ERR_HOST_TIMEOUT:
        return "HCI_ERR_HOST_TIMEOUT";
    case HCI_ERR_UNSUPPORTED_VALUE:
        return "HCI_ERR_UNSUPPORTED_VALUE";
    case HCI_ERR_ILLEGAL_PARAMETER_FMT:
        return "HCI_ERR_ILLEGAL_PARAMETER_FMT";
    case HCI_ERR_PEER_USER:
        return "HCI_ERR_PEER_USER";
    case HCI_ERR_PEER_LOW_RESOURCES:
        return "HCI_ERR_PEER_LOW_RESOURCES";
    case HCI_ERR_PEER_POWER_OFF:
        return "HCI_ERR_PEER_POWER_OFF";
    case HCI_ERR_CONN_CAUSE_LOCAL_HOST:
        return "HCI_ERR_CONN_CAUSE_LOCAL_HOST";
    case HCI_ERR_REPEATED_ATTEMPTS:
        return "HCI_ERR_REPEATED_ATTEMPTS";
    case HCI_ERR_PAIRING_NOT_ALLOWED:
        return "HCI_ERR_PAIRING_NOT_ALLOWED";
    case HCI_ERR_UNKNOWN_LMP_PDU:
        return "HCI_ERR_UNKNOWN_LMP_PDU";
    case HCI_ERR_UNSUPPORTED_REM_FEATURE:
        return "HCI_ERR_UNSUPPORTED_REM_FEATURE";
    case HCI_ERR_SCO_OFFSET_REJECTED:
        return "HCI_ERR_SCO_OFFSET_REJECTED";
    case HCI_ERR_SCO_INTERVAL_REJECTED:
        return "HCI_ERR_SCO_INTERVAL_REJECTED";
    case HCI_ERR_SCO_AIR_MODE:
        return "HCI_ERR_SCO_AIR_MODE";
    case HCI_ERR_INVALID_LMP_PARAM:
        return "HCI_ERR_INVALID_LMP_PARAM";
    case HCI_ERR_UNSPECIFIED:
        return "HCI_ERR_UNSPECIFIED";
    case HCI_ERR_UNSUPPORTED_LMP_PARAMETERS:
        return "HCI_ERR_UNSUPPORTED_LMP_PARAMETERS";
    case HCI_ERR_ROLE_CHANGE_NOT_ALLOWED:
        return "HCI_ERR_ROLE_CHANGE_NOT_ALLOWED";
    case HCI_ERR_LMP_RESPONSE_TIMEOUT:
        return "HCI_ERR_LMP_RESPONSE_TIMEOUT";
    case HCI_ERR_LMP_ERR_TRANS_COLLISION:
        return "HCI_ERR_LMP_ERR_TRANS_COLLISION";
    case HCI_ERR_LMP_PDU_NOT_ALLOWED:
        return "HCI_ERR_LMP_PDU_NOT_ALLOWED";
    case HCI_ERR_ENCRY_MODE_NOT_ACCEPTABLE:
        return "HCI_ERR_ENCRY_MODE_NOT_ACCEPTABLE";
    case HCI_ERR_UNIT_KEY_USED:
        return "HCI_ERR_UNIT_KEY_USED";
    case HCI_ERR_QOS_NOT_SUPPORTED:
        return "HCI_ERR_QOS_NOT_SUPPORTED";
    case HCI_ERR_INSTANT_PASSED:
        return "HCI_ERR_INSTANT_PASSED";
    case HCI_ERR_PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED:
        return "HCI_ERR_PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED";
    case HCI_ERR_DIFF_TRANSACTION_COLLISION:
        return "HCI_ERR_DIFF_TRANSACTION_COLLISION";
    case HCI_ERR_UNDEFINED_0x2B:
        return "HCI_ERR_UNDEFINED_0x2B";
    case HCI_ERR_QOS_UNACCEPTABLE_PARAM:
        return "HCI_ERR_QOS_UNACCEPTABLE_PARAM";
    case HCI_ERR_QOS_REJECTED:
        return "HCI_ERR_QOS_REJECTED";
    case HCI_ERR_CHAN_CLASSIF_NOT_SUPPORTED:
        return "HCI_ERR_CHAN_CLASSIF_NOT_SUPPORTED";
    case HCI_ERR_INSUFFCIENT_SECURITY:
        return "HCI_ERR_INSUFFCIENT_SECURITY";
    case HCI_ERR_PARAM_OUT_OF_RANGE:
        return "HCI_ERR_PARAM_OUT_OF_RANGE";
    case HCI_ERR_UNDEFINED_0x31:
        return "HCI_ERR_UNDEFINED_0x31";
    case HCI_ERR_ROLE_SWITCH_PENDING:
        return "HCI_ERR_ROLE_SWITCH_PENDING";
    case HCI_ERR_UNDEFINED_0x33:
        return "HCI_ERR_UNDEFINED_0x33";
    case HCI_ERR_RESERVED_SLOT_VIOLATION:
        return "HCI_ERR_RESERVED_SLOT_VIOLATION";
    case HCI_ERR_ROLE_SWITCH_FAILED:
        return "HCI_ERR_ROLE_SWITCH_FAILED";
    case HCI_ERR_INQ_RSP_DATA_TOO_LARGE:
        return "HCI_ERR_INQ_RSP_DATA_TOO_LARGE";
    case HCI_ERR_SIMPLE_PAIRING_NOT_SUPPORTED:
        return "HCI_ERR_SIMPLE_PAIRING_NOT_SUPPORTED";
    case HCI_ERR_HOST_BUSY_PAIRING:
        return "HCI_ERR_HOST_BUSY_PAIRING";
    case HCI_ERR_REJ_NO_SUITABLE_CHANNEL:
        return "HCI_ERR_REJ_NO_SUITABLE_CHANNEL";
    case HCI_ERR_CONTROLLER_BUSY:
        return "HCI_ERR_CONTROLLER_BUSY";
    case HCI_ERR_UNACCEPT_CONN_INTERVAL:
        return "HCI_ERR_UNACCEPT_CONN_INTERVAL";
    case HCI_ERR_DIRECTED_ADVERTISING_TIMEOUT:
        return "HCI_ERR_DIRECTED_ADVERTISING_TIMEOUT";
    case HCI_ERR_CONN_TOUT_DUE_TO_MIC_FAILURE:
        return "HCI_ERR_CONN_TOUT_DUE_TO_MIC_FAILURE";
    case HCI_ERR_CONN_FAILED_ESTABLISHMENT:
        return "HCI_ERR_CONN_FAILED_ESTABLISHMENT";
    case HCI_ERR_MAC_CONNECTION_FAILED:
        return "HCI_ERR_MAC_CONNECTION_FAILED";
    case HCI_ERR_LT_ADDR_ALREADY_IN_USE:
        return "HCI_ERR_LT_ADDR_ALREADY_IN_USE";
    case HCI_ERR_LT_ADDR_NOT_ALLOCATED:
        return "HCI_ERR_LT_ADDR_NOT_ALLOCATED";
    case HCI_ERR_CLB_NOT_ENABLED:
        return "HCI_ERR_CLB_NOT_ENABLED";
    case HCI_ERR_MAX_ERR:
        return "HCI_ERR_MAX_ERR";
    case HCI_ERR_ESP_VENDOR_FAIL:
        return "HCI_ERR_ESP_VENDOR_FAIL";
    case HCI_HINT_TO_RECREATE_AMP_PHYS_LINK:
        return "HCI_HINT_TO_RECREATE_AMP_PHYS_LINK";
    default:
        return "Invalid HCI status code.";
    }

    return NULL;
}
#endif /* !UC_BT_STACK_NO_LOG */

void btm_ble_extendadvcb_init(void)
{
    memset(&extend_adv_cb, 0, sizeof(tBTM_BLE_EXTENDED_CB));
}
#if (BLE_50_EXTEND_ADV_EN == TRUE)
void btm_ble_advrecod_init(void)
{
    memset(&adv_record[0], 0, sizeof(tBTM_EXT_ADV_RECORD)*MAX_BLE_ADV_INSTANCE);
}
#endif // #if (BLE_50_EXTEND_ADV_EN == TRUE)

void BTM_BleGapRegisterCallback(tBTM_BLE_5_HCI_CBACK cb)
{
    if (cb) {
        ble_5_hci_cb = cb;
    } else {
        BTM_TRACE_ERROR("%s, register fail, the cb function is NULL.", __func__);
    }
}

void BTM_ExtBleCallbackTrigger(tBTM_BLE_5_GAP_EVENT event, tBTM_BLE_5_GAP_CB_PARAMS *params)
{
    if(params && params->status == BTM_SUCCESS) {
        btm_ble_inter_set(true);
    }
    if (ble_5_hci_cb) {
        ble_5_hci_cb(event, params);
    }
}

tBTM_STATUS BTM_BleReadPhy(BD_ADDR bd_addr, UINT8 *tx_phy, UINT8 *rx_phy)
{
    tL2C_LCB *p_lcb = l2cu_find_lcb_by_bd_addr(bd_addr, BT_TRANSPORT_LE);
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if (!tx_phy || !rx_phy || !p_lcb) {
        cb_params.read_phy.status = BTM_ILLEGAL_VALUE;
        memcpy(cb_params.read_phy.addr, bd_addr, BD_ADDR_LEN);

        if (ble_5_hci_cb) {
            ble_5_hci_cb(BTM_BLE_5_GAP_READ_PHY_COMPLETE_EVT, &cb_params);
        }
        BTM_TRACE_ERROR("%s, invalid parameters", __func__);
        return BTM_ILLEGAL_VALUE;
    }

    btsnd_hcic_ble_read_phy(p_lcb->handle);

    return BTM_SUCCESS;
}

tBTM_STATUS BTM_BleSetPreferDefaultPhy(UINT8 tx_phy_mask, UINT8 rx_phy_mask)
{
    UINT8 all_phys = 0;
    tBTM_STATUS status = BTM_SUCCESS;
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if ((err = btsnd_hcic_ble_set_prefered_default_phy(all_phys, tx_phy_mask, rx_phy_mask)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("%s, fail to send the hci command, the error code = %s(0x%x)",
                        __func__, btm_ble_hci_status_to_str(err), err);
        status = BTM_HCI_ERROR | err;
    }

    cb_params.set_perf_def_phy.status = status;

    BTM_ExtBleCallbackTrigger(BTM_BLE_5_GAP_SET_PREFERED_DEFAULT_PHY_COMPLETE_EVT, &cb_params);

    return status;

}

tBTM_STATUS BTM_BleSetPreferPhy(BD_ADDR bd_addr, UINT8 all_phys, UINT8 tx_phy_mask,
                                           UINT8 rx_phy_mask, UINT16 phy_options)
{
    tL2C_LCB *p_lcb = l2cu_find_lcb_by_bd_addr(bd_addr, BT_TRANSPORT_LE);
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if (!p_lcb) {
        cb_params.status = BTM_ILLEGAL_VALUE;
        if (ble_5_hci_cb) {
            ble_5_hci_cb(BTM_BLE_5_GAP_SET_PREFERED_PHY_COMPLETE_EVT, &cb_params);
        }
        BTM_TRACE_ERROR("%s, invalid parameters", __func__);
        return BTM_ILLEGAL_VALUE;
    }



    if (!btsnd_hcic_ble_set_phy(p_lcb->handle, all_phys, tx_phy_mask, rx_phy_mask, phy_options)) {
        cb_params.status = BTM_ILLEGAL_VALUE;
        BTM_ExtBleCallbackTrigger(BTM_BLE_5_GAP_SET_PREFERED_PHY_COMPLETE_EVT, &cb_params);
    }


    return BTM_SUCCESS;
}

#if (BLE_50_EXTEND_ADV_EN == TRUE)
tBTM_STATUS BTM_BleSetExtendedAdvRandaddr(UINT8 instance, BD_ADDR rand_addr)
{
    tBTM_STATUS status = BTM_SUCCESS;
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if (instance >= MAX_BLE_ADV_INSTANCE || rand_addr == NULL) {
        status = BTM_ILLEGAL_VALUE;
        goto end;
    }

    /*
        A static address is a 48-bit randomly generated address and shall meet the following requirements:
        • The two most significant bits of the address shall be equal to 1
        • All bits of the random part of the address shall not be equal to 1
        • All bits of the random part of the address shall not be equal to 0
    */
    BD_ADDR invalid_rand_addr_a, invalid_rand_addr_b;
    memset(invalid_rand_addr_a, 0xff, sizeof(BD_ADDR));
    memset(invalid_rand_addr_b, 0x00, sizeof(BD_ADDR));
    if((rand_addr[0] & BT_STATIC_RAND_ADDR_MASK) == BT_STATIC_RAND_ADDR_MASK) {
        invalid_rand_addr_b[0] = invalid_rand_addr_b[0] | BT_STATIC_RAND_ADDR_MASK;
        if (memcmp(invalid_rand_addr_a, rand_addr, BD_ADDR_LEN) == 0
        || memcmp(invalid_rand_addr_b, rand_addr, BD_ADDR_LEN) == 0) {
            status = BTM_ILLEGAL_VALUE;
            goto end;
        }
    } else if ((rand_addr[0] | BT_NON_RPA_MASK) == BT_NON_RPA_MASK) {
        invalid_rand_addr_a[0] = invalid_rand_addr_a[0] & BT_NON_RPA_MASK;
        if (memcmp(invalid_rand_addr_a, rand_addr, BD_ADDR_LEN) == 0
        || memcmp(invalid_rand_addr_b, rand_addr, BD_ADDR_LEN) == 0) {
            status = BTM_ILLEGAL_VALUE;
            goto end;
        }
    } else {
        BTM_TRACE_ERROR("%s invalid random address", __func__);
        status = BTM_ILLEGAL_VALUE;
        goto end;
    }

    // set random address
    if((err = btsnd_hcic_ble_set_extend_rand_address(instance, rand_addr)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("%s, fail to send the hci command, the error code = %s(0x%x)",
                        __func__, btm_ble_hci_status_to_str(err), err);
        status = BTM_HCI_ERROR | err;
    } else {
        // set random address success, update address info
        if(extend_adv_cb.inst[instance].configured && extend_adv_cb.inst[instance].connetable) {
            BTM_BleSetStaticAddr(rand_addr);
            BTM_UpdateAddrInfor(BLE_ADDR_RANDOM, rand_addr);
        }
    }

end:
    cb_params.set_ext_rand_addr.status = status;
    cb_params.set_ext_rand_addr.instance = instance;
    BTM_ExtBleCallbackTrigger(BTM_BLE_5_GAP_EXT_ADV_SET_RAND_ADDR_COMPLETE_EVT, &cb_params);

    return status;

}
tBTM_STATUS BTM_BleSetExtendedAdvParams(UINT8 instance, tBTM_BLE_GAP_EXT_ADV_PARAMS *params)
{
    tBTM_STATUS status = BTM_SUCCESS;
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};
    bool use_rpa_addr = false;
    BD_ADDR rand_addr;

    if (instance >= MAX_BLE_ADV_INSTANCE) {
        status = BTM_ILLEGAL_VALUE;
         BTM_TRACE_ERROR("%s invalid instance %d", __func__, instance);
        goto end;
    }

    if ((status = btm_ble_ext_adv_params_validate(params)) != BTM_SUCCESS) {
        BTM_TRACE_ERROR("%s, invalid extend adv params.", __func__);
    }

    if (params->type & BTM_BLE_GAP_SET_EXT_ADV_PROP_CONNECTABLE) {
        extend_adv_cb.inst[instance].connetable = true;
    } else {
        extend_adv_cb.inst[instance].connetable = false;
    }

    if (params->type & BTM_BLE_GAP_SET_EXT_ADV_PROP_SCANNABLE) {
        extend_adv_cb.inst[instance].scannable = true;
    } else {
        extend_adv_cb.inst[instance].scannable = false;
    }

    if (params->type & BTM_BLE_GAP_SET_EXT_ADV_PROP_LEGACY) {
        extend_adv_cb.inst[instance].legacy_pdu = true;
    } else {
        extend_adv_cb.inst[instance].legacy_pdu = false;
    }

#if (CONTROLLER_RPA_LIST_ENABLE == FALSE)
    // if own_addr_type == BLE_ADDR_PUBLIC_ID or BLE_ADDR_RANDOM_ID,
    if((params->own_addr_type == BLE_ADDR_PUBLIC_ID || params->own_addr_type == BLE_ADDR_RANDOM_ID) && BTM_GetLocalResolvablePrivateAddr(rand_addr)) {
        params->own_addr_type = BLE_ADDR_RANDOM;
        use_rpa_addr = true;
    } else if(params->own_addr_type == BLE_ADDR_PUBLIC_ID){
        params->own_addr_type = BLE_ADDR_PUBLIC;
    } else if (params->own_addr_type == BLE_ADDR_RANDOM_ID) {
        params->own_addr_type = BLE_ADDR_RANDOM;
    }
#else
    btm_cb.ble_ctr_cb.addr_mgnt_cb.own_addr_type = params->own_addr_type;
#endif
#if (BT_BLE_FEAT_ADV_CODING_SELECTION == TRUE)
    if ((err = btsnd_hcic_ble_set_ext_adv_params_v2(instance, params->type, params->interval_min, params->interval_max,
                                      params->channel_map, params->own_addr_type, params->peer_addr_type,
                                      params->peer_addr, params->filter_policy, params->tx_power,
                                      params->primary_phy, params->max_skip,
                                      params->secondary_phy, params->sid, params->scan_req_notif,
                                      params->primary_adv_phy_options, params->secondary_adv_phy_options)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("LE EA SetParams: cmd err=0x%x", err);
        status = BTM_HCI_ERROR | err;
        goto end;
    }
#else
    if ((err = btsnd_hcic_ble_set_ext_adv_params(instance, params->type, params->interval_min, params->interval_max,
                                      params->channel_map, params->own_addr_type, params->peer_addr_type,
                                      params->peer_addr, params->filter_policy, params->tx_power,
                                      params->primary_phy, params->max_skip,
                                      params->secondary_phy, params->sid, params->scan_req_notif)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("LE EA SetParams: cmd err=0x%x", err);
        status = BTM_HCI_ERROR | err;
        goto end;
    }
#endif // (BT_BLE_FEAT_ADV_CODING_SELECTION == TRUE)

    extend_adv_cb.inst[instance].configured = true;

end:
    if(use_rpa_addr) {
        // update RPA address
        if((err = btsnd_hcic_ble_set_extend_rand_address(instance, rand_addr)) != HCI_SUCCESS) {
            BTM_TRACE_ERROR("LE EA SetParams: cmd err=0x%x", err);
            status = BTM_HCI_ERROR | err;
        } else {
            // set addr success, update address info
            BTM_UpdateAddrInfor(BLE_ADDR_RANDOM, rand_addr);
        }
    }
    cb_params.set_params.status = status;
    cb_params.set_params.instance = instance;
    BTM_ExtBleCallbackTrigger(BTM_BLE_5_GAP_EXT_ADV_SET_PARAMS_COMPLETE_EVT, &cb_params);

    return status;
}

tBTM_STATUS BTM_BleConfigExtendedAdvDataRaw(BOOLEAN is_scan_rsp, UINT8 instance, UINT16 len, UINT8 *data)
{
    tBTM_STATUS status = BTM_SUCCESS;
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};
    UINT16 rem_len = len;
    UINT8 operation = 0;
    UINT16 data_offset = 0;


    if ((status = btm_ble_ext_adv_set_data_validate(instance, len, data)) != BTM_SUCCESS) {
        BTM_TRACE_ERROR("%s, invalid extend adv data.", __func__);
        goto end;
    }

    do {
        UINT8 send_data_len = (rem_len > BTM_BLE_EXT_ADV_DATA_LEN_MAX) ? BTM_BLE_EXT_ADV_DATA_LEN_MAX : rem_len;
        if (len <= BTM_BLE_EXT_ADV_DATA_LEN_MAX) {
            operation = BTM_BLE_ADV_DATA_OP_COMPLETE;
        } else {
            if (rem_len == len) {
                operation = BTM_BLE_ADV_DATA_OP_FIRST_FRAG;
            } else if (rem_len <= BTM_BLE_EXT_ADV_DATA_LEN_MAX) {
                operation = BTM_BLE_ADV_DATA_OP_LAST_FRAG;
            } else {
                operation = BTM_BLE_ADV_DATA_OP_INTERMEDIATE_FRAG;
            }
        }
        if (!is_scan_rsp) {
            if ((err = btsnd_hcic_ble_set_ext_adv_data(instance, operation, 0, send_data_len, &data[data_offset])) != HCI_SUCCESS) {
                BTM_TRACE_ERROR("LE EA SetAdvData: cmd err=0x%x", err);
                status = BTM_HCI_ERROR | err;
                break;
            }
        } else {
            if ((err = btsnd_hcic_ble_set_ext_adv_scan_rsp_data(instance, operation, 0, send_data_len, &data[data_offset])) != HCI_SUCCESS) {
                BTM_TRACE_ERROR("LE EA SetScanRspData: cmd err=0x%x", err);
                status = BTM_HCI_ERROR | err;
                break;
            }
        }

        rem_len -= send_data_len;
        data_offset += send_data_len;
    } while (rem_len);

end:
    if (is_scan_rsp) {
        cb_params.scan_rsp_data_set.status = status;
        cb_params.scan_rsp_data_set.instance = instance;
    } else {
        cb_params.adv_data_set.status = status;
        cb_params.adv_data_set.instance = instance;
    }

    BTM_ExtBleCallbackTrigger(is_scan_rsp ? BTM_BLE_5_GAP_EXT_SCAN_RSP_DATA_SET_COMPLETE_EVT : BTM_BLE_5_GAP_EXT_ADV_DATA_SET_COMPLETE_EVT, &cb_params);

    return status;
}

tBTM_STATUS BTM_BleStartExtAdv(BOOLEAN enable, UINT8 num, tBTM_BLE_EXT_ADV *ext_adv)
{
    tBTM_STATUS status = BTM_SUCCESS;
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    UINT8 *instance = NULL;
    UINT16 *duration = NULL;
    UINT8 *max_events = NULL;

    // when enable = true, ext_adv = NULL or num = 0, goto end
    if ((!ext_adv || num == 0) && enable) {
        status = BTM_ILLEGAL_VALUE;
        BTM_TRACE_ERROR("%s invalid parameters", __func__);
        goto end;
    }

    if (num != 0 && ext_adv != NULL) {
        instance = osi_malloc(num);
        duration = osi_malloc(num*sizeof(UINT16));
        max_events = osi_malloc(num*sizeof(UINT8));

        if (!instance || !duration || !max_events) {
            status = BTM_NO_RESOURCES;
            BTM_TRACE_ERROR("%s invalid parameters", __func__);
            goto end;
        }

        for (int i = 0; i < num; i++) {
            instance[i] = ext_adv[i].instance;
            duration[i] = ext_adv[i].duration;
            max_events[i] = ext_adv[i].max_events;
        }

        if ((err = btsnd_hcic_ble_ext_adv_enable(enable, num, instance,
                                      duration, max_events)) != HCI_SUCCESS) {
            BTM_TRACE_ERROR("LE EA En=%d: cmd err=0x%x", enable, err);
            status = BTM_HCI_ERROR | err;
        }

        osi_free(instance);
        osi_free(duration);
        osi_free(max_events);
    } else {
        // enable = false, num == 0 or ext_adv = NULL

        if ((err = btsnd_hcic_ble_ext_adv_enable(enable, num, NULL, NULL, NULL)) != HCI_SUCCESS) {
            BTM_TRACE_ERROR("LE EA En=%d: cmd err=0x%x", enable, err);
            status = BTM_HCI_ERROR | err;
        }
        goto end;
    }



end:

    if (!enable && status == BTM_SUCCESS) {
        // disable all ext adv
        if(num == 0) {

            for (uint8_t i = 0; i < MAX_BLE_ADV_INSTANCE; i++)
            {
                adv_record[i].invalid = false;
                adv_record[i].enabled = false;
                adv_record[i].instance = INVALID_VALUE;
                adv_record[i].duration = INVALID_VALUE;
                adv_record[i].max_events = INVALID_VALUE;
                adv_record[i].retry_count = 0;
            }
        } else {
            for (uint8_t i = 0; i < num; i++)
            {
                uint8_t index = ext_adv[i].instance;
                adv_record[index].invalid = false;
                adv_record[index].enabled = false;
                adv_record[index].instance = INVALID_VALUE;
                adv_record[index].duration = INVALID_VALUE;
                adv_record[index].max_events = INVALID_VALUE;
                adv_record[index].retry_count = 0;
            }
        }
    }
    // start extend adv success, save the adv information
    if(enable && status == BTM_SUCCESS) {
        for (uint8_t i = 0; i < num; i++)
        {
            uint8_t index = ext_adv[i].instance;
            adv_record[index].invalid = true;
            adv_record[index].enabled = true;
            adv_record[index].instance = ext_adv[i].instance;
            adv_record[index].duration = ext_adv[i].duration;
            adv_record[index].max_events = ext_adv[i].max_events;
            adv_record[index].retry_count = 0;
        }
    }

    cb_params.adv_start.status = status;
    cb_params.adv_start.instance_num = num;
    for (uint8_t i = 0; i < num; i++) {
        cb_params.adv_start.instance[i] = ext_adv[i].instance;
    }

    BTM_ExtBleCallbackTrigger(enable ? BTM_BLE_5_GAP_EXT_ADV_START_COMPLETE_EVT : BTM_BLE_5_GAP_EXT_ADV_STOP_COMPLETE_EVT, &cb_params);

    return status;
}

tBTM_STATUS BTM_BleStartExtAdvRestart(uint8_t con_handle)
{
   tBTM_BLE_EXT_ADV ext_adv;
   uint8_t index = INVALID_VALUE;
   for (uint8_t i = 0; i < MAX_BLE_ADV_INSTANCE; i++)
   {
       if(adv_record[i].ter_con_handle == con_handle) {
           index = i;
           break;
       }
    }

    if((index >= MAX_BLE_ADV_INSTANCE) || (!adv_record[index].invalid)) {
        BTM_TRACE_WARNING("%s failed to find extend adv, adv_handle %u con_handle %u", __func__, index, con_handle);
        return BTM_WRONG_MODE;
    }

    adv_record[index].retry_count ++;
    BTM_TRACE_DEBUG("remote device did not receive aux connect response, retatrt the extend adv to reconnect, adv handle %d con_handle %d\n", index, con_handle);
    ext_adv.instance = adv_record[index].instance;
    ext_adv.duration = adv_record[index].duration;
    ext_adv.max_events = adv_record[index].max_events;
    return BTM_BleStartExtAdv(true, 1, &ext_adv);
}

tBTM_STATUS BTM_BleExtAdvSetRemove(UINT8 instance)
{
    tBTM_STATUS status = BTM_SUCCESS;
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if (instance >= MAX_BLE_ADV_INSTANCE) {
        status = BTM_ILLEGAL_VALUE;
        BTM_TRACE_ERROR("%s invalid instance %d", __func__, instance);
        goto end;
    }

    if ((err = btsnd_hcic_ble_remove_adv_set(instance)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("LE EAS Rm: cmd err=0x%x", err);
        status = BTM_HCI_ERROR | err;
    } else {
        extend_adv_cb.inst[instance].configured = false;
        extend_adv_cb.inst[instance].legacy_pdu = false;
        extend_adv_cb.inst[instance].directed = false;
        extend_adv_cb.inst[instance].scannable = false;
        extend_adv_cb.inst[instance].connetable = false;
    }

end:

    cb_params.adv_start.status = status;
    cb_params.adv_start.instance_num = 1;
    cb_params.adv_start.instance[0] = instance;

    BTM_ExtBleCallbackTrigger(BTM_BLE_5_GAP_EXT_ADV_SET_REMOVE_COMPLETE_EVT, &cb_params);

    return status;
}

tBTM_STATUS BTM_BleExtAdvSetClear(void)
{
    tBTM_STATUS status = BTM_SUCCESS;
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if ((err = btsnd_hcic_ble_clear_adv_set()) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("LE EAS Clr: cmd err=0x%x", err);
        status = BTM_HCI_ERROR | err;
    } else {
        for (uint8_t i = 0; i < MAX_BLE_ADV_INSTANCE; i++) {
            extend_adv_cb.inst[i].configured = false;
            extend_adv_cb.inst[i].legacy_pdu = false;
            extend_adv_cb.inst[i].directed = false;
            extend_adv_cb.inst[i].scannable = false;
            extend_adv_cb.inst[i].connetable = false;
        }
    }

    cb_params.adv_start.status = status;

    BTM_ExtBleCallbackTrigger(BTM_BLE_5_GAP_EXT_ADV_SET_CLEAR_COMPLETE_EVT, &cb_params);

    return status;
}
#endif // #if (BLE_50_EXTEND_ADV_EN == TRUE)

#if (BLE_50_PERIODIC_ADV_EN == TRUE)
tBTM_STATUS BTM_BlePeriodicAdvSetParams(UINT8 instance, tBTM_BLE_Periodic_Adv_Params *params)
{
    tBTM_STATUS status = BTM_SUCCESS;
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if (instance >= MAX_BLE_ADV_INSTANCE) {
        status = BTM_ILLEGAL_VALUE;
        BTM_TRACE_ERROR("%s invalid instance %d", __func__, instance);
        goto end;
    }

    if (!extend_adv_cb.inst[instance].configured ||
        extend_adv_cb.inst[instance].scannable ||
        extend_adv_cb.inst[instance].connetable ||
        extend_adv_cb.inst[instance].legacy_pdu) {
        BTM_TRACE_ERROR("%s, instance = %d, Before set the periodic adv parameters, please configure the the \
                extend adv to nonscannable and nonconnectable first, and it shouldn't include the legacy bit.", __func__, instance);
        status = BTM_ILLEGAL_VALUE;
        goto end;
    }

#if (BT_BLE_FEAT_PAWR_EN == TRUE)
    if ((err = btsnd_hcic_ble_set_periodic_adv_params_v2(instance, params->interval_min, params->interval_max,
                                                params->properties, params->num_subevents, params->subevent_interval,
                                                params->rsp_slot_delay, params->rsp_slot_spacing, params->num_rsp_slots)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("LE PA SetParams_V2: cmd err=0x%x", err);
        status = BTM_HCI_ERROR | err;
    }
#else
    if ((err = btsnd_hcic_ble_set_periodic_adv_params(instance, params->interval_min,
                                               params->interval_max, params->properties)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("LE PA SetParams: cmd err=0x%x", err);
        status = BTM_HCI_ERROR | err;
    }
#endif // (BT_BLE_FEAT_PAWR_EN == TRUE)

end:

    cb_params.per_adv_set_params.status = status;
    cb_params.per_adv_set_params.instance = instance;

    BTM_ExtBleCallbackTrigger(BTM_BLE_5_GAP_PERIODIC_ADV_SET_PARAMS_COMPLETE_EVT, &cb_params);

    return status;
}

tBTM_STATUS BTM_BlePeriodicAdvCfgDataRaw(UINT8 instance, UINT16 len, UINT8 *data,BOOLEAN only_update_did)
{
    tBTM_STATUS status = BTM_SUCCESS;
    tHCI_STATUS err = HCI_SUCCESS;
    uint16_t rem_len = len;
    UINT8 operation = 0;
    UINT16 data_offset = 0;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};
    if (only_update_did)
    {
        len = 0;
        data = NULL;
        rem_len = 0;
        operation = BTM_BLE_ADV_DATA_OP_UNCHANGED_DATA;
    }

    if ((status = btm_ble_ext_adv_set_data_validate(instance, len, data)) != BTM_SUCCESS) {
       BTM_TRACE_ERROR("%s, invalid extend adv data.", __func__);
       goto end;
    }

    do {
        UINT8 send_data_len = (rem_len > BTM_BLE_PERIODIC_ADV_DATA_LEN_MAX) ? BTM_BLE_PERIODIC_ADV_DATA_LEN_MAX : rem_len;

        if (len <= BTM_BLE_PERIODIC_ADV_DATA_LEN_MAX) {
            if (!only_update_did) {
                operation = BTM_BLE_ADV_DATA_OP_COMPLETE;
            }
        } else {
            if (rem_len == len) {
                operation = BTM_BLE_ADV_DATA_OP_FIRST_FRAG;
            } else if (rem_len <= BTM_BLE_PERIODIC_ADV_DATA_LEN_MAX) {
                operation = BTM_BLE_ADV_DATA_OP_LAST_FRAG;
            } else {
	        operation = BTM_BLE_ADV_DATA_OP_INTERMEDIATE_FRAG;
	    }
        }

        if ((err = btsnd_hcic_ble_set_periodic_adv_data(instance, operation, send_data_len, &data[data_offset])) != HCI_SUCCESS) {
            BTM_TRACE_ERROR("LE PA SetData: cmd err=0x%x", err);
            status = BTM_HCI_ERROR | err;
            break;
        }

        rem_len -= send_data_len;
        data_offset += send_data_len;
    } while(rem_len);

end:
    cb_params.per_adv_data_set.status = status;
    cb_params.per_adv_data_set.instance = instance;

    BTM_ExtBleCallbackTrigger(BTM_BLE_5_GAP_PERIODIC_ADV_DATA_SET_COMPLETE_EVT, &cb_params);

    return status;
}

tBTM_STATUS BTM_BlePeriodicAdvEnable(UINT8 instance, UINT8 enable)
{
    tBTM_STATUS status = BTM_SUCCESS;
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if (instance >= MAX_BLE_ADV_INSTANCE) {
        BTM_TRACE_ERROR("%s, invalid instance %d", __func__, instance);
        status = BTM_ILLEGAL_VALUE;
        goto end;
    }

    if ((err = btsnd_hcic_ble_periodic_adv_enable(enable, instance)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("LE PA En=%d: cmd err=0x%x", enable, err);
        status = BTM_HCI_ERROR | err;
    }

end:
    if (enable) {
        cb_params.per_adv_start.status = status;
        cb_params.per_adv_start.instance = instance;
    } else {
        cb_params.per_adv_stop.status = status;
        cb_params.per_adv_stop.instance = instance;
    }

    BTM_ExtBleCallbackTrigger(enable ? BTM_BLE_5_GAP_PERIODIC_ADV_START_COMPLETE_EVT : BTM_BLE_5_GAP_PERIODIC_ADV_STOP_COMPLETE_EVT, &cb_params);

    return status;

}
#endif // #if (BLE_50_PERIODIC_ADV_EN == TRUE)

#if (BLE_50_EXTEND_SYNC_EN == TRUE)
tBTM_STATUS BTM_BlePeriodicAdvCreateSync(tBTM_BLE_Periodic_Sync_Params *params)
{
    //tHCI_STATUS err = HCI_SUCCESS;
    tBTM_STATUS status = BTM_SUCCESS;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if (!params) {
        status = BTM_ILLEGAL_VALUE;
        BTM_TRACE_ERROR("%s, the parameter is NULL.", __func__);
        goto end;
    }

    if ((params->sync_timeout < 0x0a || params->sync_timeout > 0x4000)
        || (params->filter_policy > 0x01)
#if (BLE_FEAT_CREATE_SYNC_ENH == TRUE)
        || (params->reports_disabled > 0x01)
        || (params->filter_duplicates > 0x01)
#endif // (BLE_FEAT_CREATE_SYNC_ENH == TRUE)
        /*If the Periodic Advertiser List is not used,
        the Advertising_SID, Advertiser Address_Type, and Advertiser Address
        parameters specify the periodic advertising device to listen to; otherwise they
        shall be ignored.*/
        || (params->filter_policy == 0 && params->addr_type > 0x01)
        || (params->filter_policy == 0 && params->sid > 0xf)
        || (params->skip > 0x01F3)) {
            status = BTM_ILLEGAL_VALUE;
            BTM_TRACE_ERROR("%s, The sync parameters is invalid.", __func__);
            goto end;
    }
    uint8_t option = 0x00;
    if (params->filter_policy) {
        SET_BIT(option, 0);
    }

#if (BLE_FEAT_CREATE_SYNC_ENH == TRUE)
    if (params->reports_disabled) {
        SET_BIT(option, 1);
    }
    if (params->filter_duplicates) {
        SET_BIT(option, 2);
    }
#endif // (BLE_FEAT_CREATE_SYNC_ENH == TRUE)
#if (BLE_FEAT_CREATE_SYNC_RETRY_MAX > 0)
    /* Save parameters for potential retry */
    memcpy(&sync_retry_cb.params, params, sizeof(tBTM_BLE_Periodic_Sync_Params));
    sync_retry_cb.retry_count = 0;
    sync_retry_cb.in_use = true;
#endif // #if (BLE_FEAT_CREATE_SYNC_RETRY_MAX > 0)
    if (!btsnd_hcic_ble_periodic_adv_create_sync(option, params->sid, params->addr_type,
                                            params->addr, params->sync_timeout, params->sync_cte_type)) {
        BTM_TRACE_ERROR("LE PA CreateSync cmd failed");
        status = BTM_ILLEGAL_VALUE;
#if (BLE_FEAT_CREATE_SYNC_RETRY_MAX > 0)
        sync_retry_cb.in_use = false;
#endif // #if (BLE_FEAT_CREATE_SYNC_RETRY_MAX > 0)
    }

end:
    if(status != BTM_SUCCESS) {
        cb_params.status = status;
        BTM_ExtBleCallbackTrigger(BTM_BLE_5_GAP_PERIODIC_ADV_CREATE_SYNC_COMPLETE_EVT, &cb_params);
    }

    return status;
}
#endif // #if (BLE_50_EXTEND_SYNC_EN == TRUE)

void btm_set_phy_callback(UINT8 status)
{
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};
    cb_params.status = status;

    BTM_ExtBleCallbackTrigger(BTM_BLE_5_GAP_SET_PREFERED_PHY_COMPLETE_EVT, &cb_params);

}

#if (BLE_50_EXTEND_SYNC_EN == TRUE)
void btm_create_sync_callback(UINT8 status)
{
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};
    cb_params.status = status;

    BTM_ExtBleCallbackTrigger(BTM_BLE_5_GAP_PERIODIC_ADV_CREATE_SYNC_COMPLETE_EVT, &cb_params);
}
#endif // #if (BLE_50_EXTEND_SYNC_EN == TRUE)

void btm_read_phy_callback(uint8_t hci_status, uint16_t conn_handle, uint8_t tx_phy, uint8_t rx_phy)
{
    tBTM_STATUS status = BTM_SUCCESS;
    tL2C_LCB *p_lcb = l2cu_find_lcb_by_handle(conn_handle);
    if(hci_status != HCI_SUCCESS) {
        status = BTM_ILLEGAL_VALUE;
        BTM_TRACE_ERROR("%s error status %d", __func__, hci_status);
    }
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};
    if(p_lcb) {
       memcpy(cb_params.read_phy.addr, p_lcb->remote_bd_addr, BD_ADDR_LEN);
    }
    cb_params.read_phy.status = status;
    cb_params.read_phy.tx_phy = tx_phy;
    cb_params.read_phy.rx_phy = rx_phy;

    BTM_ExtBleCallbackTrigger(BTM_BLE_5_GAP_READ_PHY_COMPLETE_EVT, &cb_params);
}

#if (BLE_50_EXTEND_SYNC_EN == TRUE)
tBTM_STATUS BTM_BlePeriodicAdvSyncCancel(void)
{
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_STATUS status = BTM_SUCCESS;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

#if (BLE_FEAT_CREATE_SYNC_RETRY_MAX > 0)
    /* Clear retry state when sync is cancelled */
    sync_retry_cb.in_use = false;
    sync_retry_cb.retry_count = 0;
#endif // #if (BLE_FEAT_CREATE_SYNC_RETRY_MAX > 0)
    if ((err = btsnd_hcic_ble_periodic_adv_create_sync_cancel()) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("LE PA SyncCancel, cmd err=0x%x", err);
        status = BTM_HCI_ERROR | err;
    }

    cb_params.status = status;

    BTM_ExtBleCallbackTrigger(BTM_BLE_5_GAP_PERIODIC_ADV_SYNC_CANCEL_COMPLETE_EVT, &cb_params);

    return status;
}

tBTM_STATUS BTM_BlePeriodicAdvSyncTerm(UINT16 sync_handle)
{
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_STATUS status = BTM_SUCCESS;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if (( err = btsnd_hcic_ble_periodic_adv_term_sync(sync_handle)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("LE PA SyncTerm: cmd err=0x%x", err);
        status = BTM_HCI_ERROR | err;
    }

    cb_params.status = status;

    BTM_ExtBleCallbackTrigger(BTM_BLE_5_GAP_PERIODIC_ADV_SYNC_TERMINATE_COMPLETE_EVT, &cb_params);

    return status;
}

tBTM_STATUS BTM_BlePeriodicAdvAddDevToList(tBLE_ADDR_TYPE addr_type, BD_ADDR addr, UINT16 sid)
{
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_STATUS status = BTM_SUCCESS;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if (addr_type > BLE_ADDR_TYPE_MAX) {
        status = BTM_ILLEGAL_VALUE;
        BTM_TRACE_ERROR("%s invalid addr_type %d", __func__, addr_type);
        goto end;
    }

    if ((err = btsnd_hcic_ble_add_dev_to_periodic_adv_list(addr_type, addr, sid)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("LE PA AddDevToList: cmd err=0x%x", err);
        status = BTM_HCI_ERROR | err;
    }

end:
    cb_params.status = status;
    BTM_ExtBleCallbackTrigger(BTM_BLE_5_GAP_PERIODIC_ADV_ADD_DEV_COMPLETE_EVT, &cb_params);

    return status;
}

tBTM_STATUS BTM_BlePeriodicAdvRemoveDevFromList(tBLE_ADDR_TYPE addr_type, BD_ADDR addr, UINT16 sid)
{
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_STATUS status = BTM_SUCCESS;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if (addr_type > BLE_ADDR_TYPE_MAX) {
        status = BTM_ILLEGAL_VALUE;
        BTM_TRACE_ERROR("%s invalid addr_type %d", __func__, addr_type);
        goto end;
    }

    if ((err = btsnd_hcic_ble_rm_dev_from_periodic_adv_list(addr_type, addr, sid)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("LE PA RmDevFromList: cmd err=0x%x", err);
        status = BTM_HCI_ERROR | err;
    }

end:

    cb_params.status = status;
    BTM_ExtBleCallbackTrigger(BTM_BLE_5_GAP_PERIODIC_ADV_REMOVE_DEV_COMPLETE_EVT, &cb_params);
    return status;
}

tBTM_STATUS BTM_BlePeriodicAdvClearDev(void)
{
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_STATUS status = BTM_SUCCESS;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if ((err = btsnd_hcic_ble_clear_periodic_adv_list()) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("LE PA ClrDev: cmd err=0x%x", err);
        status = BTM_HCI_ERROR | err;
    }

    cb_params.status = status;
    BTM_ExtBleCallbackTrigger(BTM_BLE_5_GAP_PERIODIC_ADV_CLEAR_DEV_COMPLETE_EVT, &cb_params);

    return status;
}
#endif // #if (BLE_50_EXTEND_SYNC_EN == TRUE)

#if (BLE_50_EXTEND_SCAN_EN == TRUE)
tBTM_STATUS BTM_BleSetExtendedScanParams(tBTM_BLE_EXT_SCAN_PARAMS *params)
{
    UINT8 phy_mask = 0;
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_STATUS status = BTM_SUCCESS;
    tHCI_EXT_SCAN_PARAMS hci_params[2];
    int phy_count = 0;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if (!params) {
        status = BTM_ILLEGAL_VALUE;
        BTM_TRACE_ERROR("%s invalid parameters", __func__);
        goto end;
    }

    if (params->own_addr_type > BLE_ADDR_TYPE_MAX) {
        status = BTM_ILLEGAL_VALUE;
        goto end;
    }

    if (params->cfg_mask & BTM_BLE_GAP_EXT_SCAN_UNCODE_MASK) {
        phy_mask |= 0x01;
        memcpy(&hci_params[phy_count], &params->uncoded_cfg, sizeof(tHCI_EXT_SCAN_PARAMS));
        phy_count++;
    }

    if (params->cfg_mask & BTM_BLE_GAP_EXT_SCAN_CODE_MASK) {
        phy_mask |= 0x04;
        memcpy(&hci_params[phy_count], &params->coded_cfg, sizeof(tHCI_EXT_SCAN_PARAMS));
        phy_count++;
    }

    if (BTM_BleUpdateOwnType(&params->own_addr_type, NULL) != 0 ) {
        status = BTM_ILLEGAL_VALUE;
        BTM_TRACE_ERROR("LE UpdateOwnType err");
        goto end;
    }

    extend_adv_cb.scan_duplicate = params->scan_duplicate;

    if ((err = btsnd_hcic_ble_set_ext_scan_params(params->own_addr_type, params->filter_policy, phy_mask, phy_count,
                                      hci_params)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("LE ES SetParams: cmd err=0x%x", err);
        status = BTM_HCI_ERROR | err;
    }

end:

    cb_params.status = status;

    BTM_ExtBleCallbackTrigger(BTM_BLE_5_GAP_SET_EXT_SCAN_PARAMS_COMPLETE_EVT, &cb_params);

    return cb_params.status;
}

tBTM_STATUS BTM_BleExtendedScan(BOOLEAN enable, UINT16 duration, UINT16 period)
{
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};
    tBTM_STATUS status = BTM_SUCCESS;

    if (extend_adv_cb.scan_duplicate > 0x03) {
        status = BTM_ILLEGAL_VALUE;
        BTM_TRACE_ERROR("%s invalid scan_duplicate %d", __func__, extend_adv_cb.scan_duplicate);
        goto end;
    }

    if ((err = btsnd_hcic_ble_ext_scan_enable(enable, extend_adv_cb.scan_duplicate, duration, period)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("LE ES En=%d: cmd err=0x%x", enable, err);
        status = BTM_HCI_ERROR | err;
    }

end:

    cb_params.status = status;

    BTM_ExtBleCallbackTrigger(enable ? BTM_BLE_5_GAP_EXT_SCAN_START_COMPLETE_EVT : BTM_BLE_5_GAP_EXT_SCAN_STOP_COMPLETE_EVT, &cb_params);

    return status;
}
#endif // #if (BLE_50_EXTEND_SCAN_EN == TRUE)

void BTM_BleSetPreferExtenedConnParams (BD_ADDR bd_addr, tBTM_EXT_CONN_PARAMS *params)
{
    tBTM_SEC_DEV_REC  *p_dev_rec = btm_find_or_alloc_dev (bd_addr);
    tBTM_STATUS status = BTM_SUCCESS;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if (p_dev_rec) {
        if (params) {
            memcpy(&p_dev_rec->ext_conn_params, params, sizeof(tBTM_EXT_CONN_PARAMS));
        } else {
            BTM_TRACE_ERROR("Invalid Extended connection parameters");
            status = BTM_ILLEGAL_VALUE;
        }
    } else {
        BTM_TRACE_ERROR("Unknown Device, setting rejected");
        status = BTM_UNKNOWN_ADDR;
    }

    cb_params.status = status;
    BTM_ExtBleCallbackTrigger(BTM_BLE_5_GAP_PREFER_EXT_CONN_PARAMS_SET_COMPLETE_EVT, &cb_params);
}

void btm_ble_extended_init(void)
{

}

void btm_ble_extended_cleanup(void)
{

}

#if (BLE_50_EXTEND_ADV_EN == TRUE)
static tBTM_STATUS btm_ble_ext_adv_params_validate(tBTM_BLE_GAP_EXT_ADV_PARAMS *params)
{
    if (!params) {
        return BTM_ILLEGAL_VALUE;
    }

    if (params->own_addr_type > BLE_ADDR_TYPE_MAX) {
	BTM_TRACE_ERROR("%s, invalid own address type, line %d, addr type %d", __func__, __LINE__, params->own_addr_type);
        return BTM_ILLEGAL_VALUE;
    }

    if (params->type & BTM_BLE_GAP_SET_EXT_ADV_PROP_LEGACY) {
        /* Not allowed for legacy PDUs. */
        if (params->type & BTM_BLE_GAP_SET_EXT_ADV_PROP_INCLUDE_TX_PWR) {
	    BTM_TRACE_ERROR("%s, The Legacy adv can't include tx power bit, line %d", __func__, __LINE__);
            return BTM_ILLEGAL_VALUE;
        }
    }

    if (!(params->type & BTM_BLE_GAP_SET_EXT_ADV_PROP_LEGACY)) {
        /* Not allowed for extended advertising PDUs */
        if ((params->type & BTM_BLE_GAP_SET_EXT_ADV_PROP_CONNECTABLE) &&
            (params->type & BTM_BLE_GAP_SET_EXT_ADV_PROP_SCANNABLE)) {
            BTM_TRACE_ERROR("%s, For the Extend adv, the properties can't be connectable and scannable at the same time, line %d", __func__, __LINE__);
            return BTM_ILLEGAL_VALUE;
        }

        /* HD directed advertising allowed only for legacy PDUs */
        if (params->type & BTM_BLE_GAP_SET_EXT_ADV_PROP_HD_DIRECTED) {
            BTM_TRACE_ERROR("%s, HD directed advertising allowed only for legacy PDUs. line %d", __func__, __LINE__);
            return BTM_ILLEGAL_VALUE;
        }
    }

    return BTM_SUCCESS;
}

static tBTM_STATUS btm_ble_ext_adv_set_data_validate(UINT8 instance, UINT16 len, UINT8 *data)
{
    if (data == NULL && len > 0) {
        BTM_TRACE_ERROR("%s, the extend adv data is NULL. line %d", __func__, __LINE__);
        return BTM_ILLEGAL_VALUE;
    }

    if (instance >= MAX_BLE_ADV_INSTANCE) {
        BTM_TRACE_ERROR("%s, adv instance is %d, Exceeded the maximum. line %d", __func__, instance, __LINE__);
        return BTM_ILLEGAL_VALUE;
    }

    if (!extend_adv_cb.inst[instance].configured) {
        BTM_TRACE_ERROR("%s, The extend adv hasn't configured, please use the set_ext_adv_params API to set the ext adv parameters first. line %d", __func__, __LINE__);
        return BTM_ILLEGAL_VALUE;
    }

    /* Not allowed with the direted advertising for legacy */
    if (extend_adv_cb.inst[instance].legacy_pdu && extend_adv_cb.inst[instance].directed) {
	BTM_TRACE_ERROR("%s, Not allowed with the direted advertising for legacy. line %d", __func__, __LINE__);
        return BTM_ILLEGAL_VALUE;
    }

    /* Always allowed with legacy PDU but limited to legacy length */
    if (extend_adv_cb.inst[instance].legacy_pdu) {
        if (len > 31) {
            BTM_TRACE_ERROR("%s, for the legacy adv, the adv data length can't exceed 31. line %d", __func__, __LINE__);
            return BTM_ILLEGAL_VALUE;
        }
    } else {
        if (len > controller_get_interface()->ble_get_ext_adv_data_max_len()) {
            BTM_TRACE_ERROR("%s, The adv data len(%d) is longer then the controller adv max len(%d)",
                __func__, len, controller_get_interface()->ble_get_ext_adv_data_max_len());
            return BTM_ILLEGAL_VALUE;
        }
    }

    return BTM_SUCCESS;
}
#endif // #if (BLE_50_EXTEND_ADV_EN == TRUE)

void btm_ble_update_phy_evt(tBTM_BLE_UPDATE_PHY *params)
{
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if (!params) {
        BTM_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }
    tL2C_LCB *p_lcb = l2cu_find_lcb_by_handle(params->conn_idx);
    if(!p_lcb)  {
        BTM_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    cb_params.phy_update.status = params->status;
    cb_params.phy_update.tx_phy = params->tx_phy;
    cb_params.phy_update.rx_phy = params->rx_phy;
    memcpy(cb_params.phy_update.addr, p_lcb->remote_bd_addr, BD_ADDR_LEN);

    // If the user has register the callback function, should callback it to the application.

    BTM_ExtBleCallbackTrigger(BTM_BLE_5_GAP_PHY_UPDATE_COMPLETE_EVT, &cb_params);

    return;
}

#if (BLE_50_EXTEND_SCAN_EN == TRUE)
void btm_ble_scan_timeout_evt(void)
{
    BTM_ExtBleCallbackTrigger(BTM_BLE_5_GAP_SCAN_TIMEOUT_EVT, NULL);
}
#endif // #if (BLE_50_EXTEND_SCAN_EN == TRUE)

#if (BLE_50_EXTEND_ADV_EN == TRUE)
void btm_ble_adv_set_terminated_evt(tBTM_BLE_ADV_TERMINAT *params)
{
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if (!params) {
        BTM_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    if (params->adv_handle >= MAX_BLE_ADV_INSTANCE) {
        BTM_TRACE_ERROR("%s, Invalid adv_handle %d, max is %d.", __func__, params->adv_handle, MAX_BLE_ADV_INSTANCE);
        return;
    }

    // adv terminated due to connection, save the adv handle and connection handle
    if(params->status == 0x00) {
        adv_record[params->adv_handle].ter_con_handle = params->conn_handle;
    } else {
        adv_record[params->adv_handle].ter_con_handle = INVALID_VALUE;
        adv_record[params->adv_handle].invalid = false;
    }
    adv_record[params->adv_handle].enabled = false;

    memcpy(&cb_params.adv_term, params, sizeof(tBTM_BLE_ADV_TERMINAT));

    // If the user has register the callback function, should callback it to the application.
    BTM_ExtBleCallbackTrigger(BTM_BLE_5_GAP_ADV_TERMINATED_EVT, &cb_params);

    return;
}
#endif // #if (BLE_50_EXTEND_ADV_EN == TRUE)

#if (BLE_50_EXTEND_SCAN_EN == TRUE)
void btm_ble_ext_adv_report_evt(tBTM_BLE_EXT_ADV_REPORT *params)
{
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if (!params) {
        BTM_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    memcpy(&cb_params.ext_adv_report, params, sizeof(tBTM_BLE_EXT_ADV_REPORT));

    // If the user has register the callback function, should callback it to the application.
    BTM_ExtBleCallbackTrigger(BTM_BLE_5_GAP_EXT_ADV_REPORT_EVT, &cb_params);

    return;

}
#endif // #if (BLE_50_EXTEND_SCAN_EN == TRUE)

#if (BLE_50_EXTEND_ADV_EN == TRUE)
void btm_ble_scan_req_received_evt(tBTM_BLE_SCAN_REQ_RECEIVED *params)
{
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if (!params) {
        BTM_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    memcpy(&cb_params.scan_req, params, sizeof(tBTM_BLE_SCAN_REQ_RECEIVED));

    // If the user has register the callback function, should callback it to the application.
    BTM_ExtBleCallbackTrigger(BTM_BLE_5_GAP_SCAN_REQ_RECEIVED_EVT, &cb_params);

    return;
}
#endif // #if (BLE_50_EXTEND_ADV_EN == TRUE)
void btm_ble_channel_select_algorithm_evt(tBTM_BLE_CHANNEL_SEL_ALG *params)
{
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if (!params) {
        BTM_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    memcpy(&cb_params.channel_sel, params, sizeof(tBTM_BLE_CHANNEL_SEL_ALG));

    // If the user has register the callback function, should callback it to the application.
    BTM_ExtBleCallbackTrigger(BTM_BLE_5_GAP_CHANNEL_SELETE_ALGORITHM_EVT, &cb_params);

    return;
}

#if (BLE_50_EXTEND_SYNC_EN == TRUE)
void btm_ble_periodic_adv_report_evt(tBTM_PERIOD_ADV_REPORT *params)
{
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if (!params) {
        BTM_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    memcpy(&cb_params.period_adv_report, params, sizeof(tBTM_PERIOD_ADV_REPORT));

    // If the user has register the callback function, should callback it to the application.
    BTM_ExtBleCallbackTrigger(BTM_BLE_5_GAP_PERIODIC_ADV_REPORT_EVT, &cb_params);

    return;

}

void btm_ble_periodic_adv_sync_lost_evt(tBTM_BLE_PERIOD_ADV_SYNC_LOST *params)
{
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if (!params) {
        BTM_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    memcpy(&cb_params.sync_lost, params, sizeof(tBTM_BLE_PERIOD_ADV_SYNC_LOST));

    // If the user has register the callback function, should callback it to the application.
    BTM_ExtBleCallbackTrigger(BTM_BLE_5_GAP_PERIODIC_ADV_SYNC_LOST_EVT, &cb_params);

    return;

}

void btm_ble_periodic_adv_sync_establish_evt(tBTM_BLE_PERIOD_ADV_SYNC_ESTAB *params)
{
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if (!params) {
        BTM_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

#if (BLE_FEAT_CREATE_SYNC_RETRY_MAX > 0)
    /* Check if retry is needed for error 0x3E (Connection Failed to be Established) */
    if (params->status == HCI_ERR_CONN_FAILED_ESTABLISHMENT &&
        sync_retry_cb.in_use &&
        sync_retry_cb.retry_count < BLE_FEAT_CREATE_SYNC_RETRY_MAX) {

        sync_retry_cb.retry_count++;
        BTM_TRACE_WARNING("%s, Create sync failed with 0x3E, retry %d/%d",
                         __func__, sync_retry_cb.retry_count, BLE_FEAT_CREATE_SYNC_RETRY_MAX);

        /* Build option from saved parameters */
        uint8_t option = 0x00;
        if (sync_retry_cb.params.filter_policy) {
            SET_BIT(option, 0);
        }
#if (BLE_FEAT_CREATE_SYNC_ENH == TRUE)
        if (sync_retry_cb.params.reports_disabled) {
            SET_BIT(option, 1);
        }
        if (sync_retry_cb.params.filter_duplicates) {
            SET_BIT(option, 2);
        }
#endif // (BLE_FEAT_CREATE_SYNC_ENH == TRUE)

        /* Retry create sync with saved parameters */
        if (btsnd_hcic_ble_periodic_adv_create_sync(option,
                                                    sync_retry_cb.params.sid,
                                                    sync_retry_cb.params.addr_type,
                                                    sync_retry_cb.params.addr,
                                                    sync_retry_cb.params.sync_timeout,
                                                    sync_retry_cb.params.sync_cte_type)) {
            /* Retry command sent successfully, wait for next event */
            return;
        }
        /* If retry command failed, fall through to report failure */
        BTM_TRACE_ERROR("%s, Retry create sync command failed", __func__);
    }

    /* Clear retry state */
    sync_retry_cb.in_use = false;
    sync_retry_cb.retry_count = 0;
#endif // #if (BLE_FEAT_CREATE_SYNC_RETRY_MAX > 0)
    memcpy(&cb_params.sync_estab, params, sizeof(tBTM_BLE_PERIOD_ADV_SYNC_ESTAB));

    // If the user has register the callback function, should callback it to the application.
    BTM_ExtBleCallbackTrigger(BTM_BLE_5_GAP_PERIODIC_ADV_SYNC_ESTAB_EVT, &cb_params);

    return;

}
#endif // #if (BLE_50_EXTEND_SYNC_EN == TRUE)

#if (BLE_50_EXTEND_ADV_EN == TRUE)
uint8_t btm_ble_ext_adv_active_count(void)
{
    uint8_t count = 0;

    for (uint8_t i = 0; i < MAX_BLE_ADV_INSTANCE; i++) {
        if (adv_record[i].enabled == true) {
            BTM_TRACE_DEBUG("%s EXT ADV active #%d: instance=%d, duration=%d, max_events=%d",
                            __func__,
                            count,
                            adv_record[i].instance,
                            adv_record[i].duration,
                            adv_record[i].max_events);
            count++;
        }
    }

    return count;
}
#endif // #if (BLE_50_EXTEND_ADV_EN == TRUE)

#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)

#if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)
void btm_ble_periodic_adv_sync_trans_complete(UINT16 op_code, UINT8 hci_status, UINT16 conn_handle)
{
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};
    UINT8 evt = BTM_BLE_5_GAP_UNKNOWN_EVT;
    tL2C_LCB *p_lcb = l2cu_find_lcb_by_handle(conn_handle);

    switch (op_code) {
    case HCI_BLE_PERIOD_ADV_SYNC_TRANS:
        evt = BTM_BLE_GAP_PERIODIC_ADV_SYNC_TRANS_COMPLETE_EVT;
        break;
    case HCI_BLE_PERIOD_ADV_SET_INFO_TRANS:
        evt = BTM_BLE_GAP_PERIODIC_ADV_SET_INFO_TRANS_COMPLETE_EVT;
        break;
    case HCI_BLE_SET_PAST_PARAMS:
        evt = BTM_BLE_GAP_SET_PAST_PARAMS_COMPLETE_EVT;
        break;
    default:
        return;
    }

    cb_params.per_adv_sync_trans.status = BTM_SUCCESS;
    if(hci_status != HCI_SUCCESS) {
        cb_params.per_adv_sync_trans.status = BTM_ILLEGAL_VALUE;
        BTM_TRACE_ERROR("%s error status %d", __func__, hci_status);
    }

    if(p_lcb) {
       memcpy(cb_params.per_adv_sync_trans.addr, p_lcb->remote_bd_addr, BD_ADDR_LEN);
    }

    BTM_ExtBleCallbackTrigger(evt, &cb_params);
}

void BTM_BlePeriodicAdvRecvEnable(UINT16 sync_handle, UINT8 enable)
{
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_STATUS status = BTM_SUCCESS;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if ((err = btsnd_hcic_ble_set_periodic_adv_recv_enable(sync_handle, enable)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("%s cmd err=0x%x", __func__, err);
        status = BTM_HCI_ERROR | err;
    }

    cb_params.status = status;
    BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_PERIODIC_ADV_RECV_ENABLE_COMPLETE_EVT, &cb_params);
}

void BTM_BlePeriodicAdvSyncTrans(BD_ADDR bd_addr, UINT16 service_data, UINT16 sync_handle)
{
    tBTM_STATUS status = BTM_SUCCESS;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    tL2C_LCB *p_lcb = l2cu_find_lcb_by_bd_addr(bd_addr, BT_TRANSPORT_LE);
    if (!p_lcb) {
        BTM_TRACE_ERROR("%s, invalid parameters", __func__);
        status = BTM_ILLEGAL_VALUE;
    }

    if (status != BTM_SUCCESS) {
        cb_params.per_adv_sync_trans.status = status;
        memcpy(cb_params.per_adv_sync_trans.addr, bd_addr, sizeof(BD_ADDR));
        BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_PERIODIC_ADV_SYNC_TRANS_COMPLETE_EVT, &cb_params);
        return;
    }

    btsnd_hcic_ble_periodic_adv_sync_trans(p_lcb->handle, service_data, sync_handle);
}

void BTM_BlePeriodicAdvSetInfoTrans(BD_ADDR bd_addr, UINT16 service_data, UINT8 adv_handle)
{
    tBTM_STATUS status = BTM_SUCCESS;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};
    tL2C_LCB *p_lcb = l2cu_find_lcb_by_bd_addr(bd_addr, BT_TRANSPORT_LE);

    if (!p_lcb) {
        BTM_TRACE_ERROR("%s, invalid parameters", __func__);
        status = BTM_ILLEGAL_VALUE;
    }

    if (status != BTM_SUCCESS) {
        cb_params.per_adv_sync_trans.status = status;
        memcpy(cb_params.per_adv_sync_trans.addr, bd_addr, sizeof(BD_ADDR));
        BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_PERIODIC_ADV_SET_INFO_TRANS_COMPLETE_EVT, &cb_params);
        return;
    }

    btsnd_hcic_ble_periodic_adv_set_info_trans(p_lcb->handle, service_data, adv_handle);
}

void BTM_BleSetPeriodicAdvSyncTransParams(BD_ADDR bd_addr, UINT8 mode, UINT16 skip, UINT16 sync_timeout, UINT8 cte_type)
{
    tBTM_STATUS status = BTM_SUCCESS;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    // Set default past params
    if (bdaddr_is_empty((bt_bdaddr_t *)bd_addr)) {
        tHCI_STATUS err = HCI_SUCCESS;
        if ((err = btsnd_hcic_ble_set_default_periodic_adv_sync_trans_params(mode, skip, sync_timeout, cte_type)) != HCI_SUCCESS) {
            BTM_TRACE_ERROR("%s cmd err=0x%x", __func__, err);
            status = BTM_HCI_ERROR | err;
        }

        cb_params.set_past_params.status = status;
        memset(cb_params.set_past_params.addr, 0, sizeof(BD_ADDR));
        BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_SET_PAST_PARAMS_COMPLETE_EVT, &cb_params);
        return;
    }

    tL2C_LCB *p_lcb = l2cu_find_lcb_by_bd_addr(bd_addr, BT_TRANSPORT_LE);
    if (!p_lcb) {
        BTM_TRACE_ERROR("%s, invalid parameters", __func__);
        status = BTM_ILLEGAL_VALUE;
    }

    if (status != BTM_SUCCESS) {
        cb_params.set_past_params.status = status;
        memcpy(cb_params.set_past_params.addr, bd_addr, sizeof(BD_ADDR));
        BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_SET_PAST_PARAMS_COMPLETE_EVT, &cb_params);
        return;
    }

    btsnd_hcic_ble_set_periodic_adv_sync_trans_params(p_lcb->handle, mode, skip, sync_timeout, cte_type);
}

void btm_ble_periodic_adv_sync_trans_recv_evt(tBTM_BLE_PERIOD_ADV_SYNC_TRANS_RECV *params)
{
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if (!params) {
        BTM_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    memcpy(&cb_params.past_recv, params, sizeof(tBTM_BLE_PERIOD_ADV_SYNC_TRANS_RECV));

    BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_PERIODIC_ADV_SYNC_TRANS_RECV_EVT, &cb_params);
}
#endif // #if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)

#if (BLE_FEAT_POWER_CONTROL_EN == TRUE)
void BTM_BleEnhReadTransPowerLevel(uint16_t conn_handle, uint8_t phy)
{
    btsnd_hcic_ble_enh_read_trans_power_level(conn_handle, phy);
}

void BTM_BleReadRemoteTransPwrLevel(uint16_t conn_handle, uint8_t phy)
{
    btsnd_hcic_ble_read_remote_trans_power_level(conn_handle, phy);
}

void BTM_BleSetPathLossRptParams(uint16_t conn_handle, uint8_t high_threshold, uint8_t high_hysteresis,
                                        uint8_t low_threshold, uint8_t low_hysteresis, uint16_t min_time_spent)
{
    tBTM_STATUS status = BTM_SUCCESS;
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if ((err = btsnd_hcic_ble_set_path_loss_rpt_params(conn_handle, high_threshold, high_hysteresis, low_threshold, low_hysteresis, min_time_spent)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("%s cmd err=0x%x", __func__, err);
        status = BTM_HCI_ERROR | err;
    }

    cb_params.path_loss_rpting_params.status = status;
    cb_params.path_loss_rpting_params.conn_handle = conn_handle;

    BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_SET_PATH_LOSS_REPORTING_PARAMS_EVT, &cb_params);
}

void BTM_BleSetPathLossRptEnable(uint16_t conn_handle, uint8_t enable)
{
    tBTM_STATUS status = BTM_SUCCESS;
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if ((err = btsnd_hcic_ble_set_path_loss_rpt_enable(conn_handle, enable)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("%s cmd err=0x%x", __func__, err);
        status = BTM_HCI_ERROR | err;
    }

    cb_params.path_loss_rpting_enable.status = status;
    cb_params.path_loss_rpting_enable.conn_handle = conn_handle;

    BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_SET_PATH_LOSS_REPORTING_ENABLE_EVT, &cb_params);
}

void BTM_BleSetTransPwrRptEnable(uint16_t conn_handle, uint8_t local_enable, uint8_t remote_enable)
{
    tBTM_STATUS status = BTM_SUCCESS;
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if ((err = btsnd_hcic_ble_set_trans_pwr_rpt_enable(conn_handle, local_enable, remote_enable)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("%s cmd err=0x%x", __func__, err);
        status = BTM_HCI_ERROR | err;
    }

    cb_params.trans_pwr_rpting_enable.status = status;
    cb_params.trans_pwr_rpting_enable.conn_handle = conn_handle;

    BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_SET_TRANS_POWER_REPORTING_ENABLE_EVT, &cb_params);
}

void btm_enh_read_trans_pwr_level_cmpl_evt(uint8_t *p)
{
    uint8_t hci_status;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    STREAM_TO_UINT8(hci_status, p);
    STREAM_TO_UINT16(cb_params.enh_trans_pwr_level_cmpl.conn_handle, p);
    STREAM_TO_UINT8(cb_params.enh_trans_pwr_level_cmpl.phy, p);
    STREAM_TO_UINT8(cb_params.enh_trans_pwr_level_cmpl.cur_tx_pwr_level, p);
    STREAM_TO_UINT8(cb_params.enh_trans_pwr_level_cmpl.max_tx_pwr_level, p);

    if(hci_status != HCI_SUCCESS) {
        hci_status = BTM_HCI_ERROR | hci_status;
        BTM_TRACE_ERROR("%s error status %d", __func__, hci_status);
    }
    cb_params.enh_trans_pwr_level_cmpl.status = hci_status;

    BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_ENH_READ_TRANS_POWER_LEVEL_EVT, &cb_params);
}

void btm_read_remote_trans_pwr_level_cmpl(UINT8 status)
{
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if (status != HCI_SUCCESS) {
        status = (status | BTM_HCI_ERROR);
    }

    cb_params.remote_pwr_level_cmpl.status = status;

    BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_READ_REMOTE_TRANS_POWER_LEVEL_EVT, &cb_params);
}

void btm_ble_path_loss_threshold_evt(tBTM_BLE_PATH_LOSS_THRESHOLD_EVT *params)
{
    if (!params) {
        BTM_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    // If the user has register the callback function, should callback it to the application.
    BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_PATH_LOSS_THRESHOLD_EVT, (tBTM_BLE_5_GAP_CB_PARAMS *)params);
}

void btm_ble_transmit_power_report_evt(tBTM_BLE_TRANS_POWER_REPORT_EVT *params)
{
    if (!params) {
        BTM_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    if (params->status != HCI_SUCCESS) {
        params->status = (params->status | BTM_HCI_ERROR);
    }

    // If the user has register the callback function, should callback it to the application.
    BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_TRANMIT_POWER_REPORTING_EVT, (tBTM_BLE_5_GAP_CB_PARAMS *)params);
}

#endif // #if (BLE_FEAT_POWER_CONTROL_EN == TRUE)

#if (BLE_FEAT_CONN_SUBRATING == TRUE)
void BTM_BleSetDefaultSubrate(UINT16 subrate_min, UINT16 subrate_max, UINT16 max_latency, UINT16 continuation_number, UINT16 supervision_timeout)
{
    tBTM_STATUS status = BTM_SUCCESS;
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if ((err = btsnd_hcic_ble_set_default_subrate(subrate_min, subrate_max, max_latency, continuation_number, supervision_timeout)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("%s cmd err=0x%x", __func__, err);
        status = BTM_HCI_ERROR | err;
    }

    cb_params.status = status;

    BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_SET_DEFAULT_SUBRATE_EVT, &cb_params);
}

void BTM_BleSubrateRequest(UINT16 conn_handle, UINT16 subrate_min, UINT16 subrate_max, UINT16 max_latency, UINT16 continuation_number, UINT16 supervision_timeout)
{
    btsnd_hcic_ble_subrate_request(conn_handle, subrate_min, subrate_max, max_latency, continuation_number, supervision_timeout);
}

void btm_subrate_req_cmd_status(UINT8 status)
{
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if (status != HCI_SUCCESS) {
        status = (status | BTM_HCI_ERROR);
    }
    cb_params.status = status;

    BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_SUBRATE_REQUEST_EVT, &cb_params);
}

void btm_ble_subrate_change_evt(tBTM_BLE_SUBRATE_CHANGE_EVT *params)
{
    if (params->status != HCI_SUCCESS) {
        params->status = (params->status | BTM_HCI_ERROR);
    }

    BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_SUBRATE_CHANGE_EVT, (tBTM_BLE_5_GAP_CB_PARAMS *)params);
}
#endif // #if (BLE_FEAT_CONN_SUBRATING == TRUE)

#if (BLE_50_FEATURE_SUPPORT == TRUE)
tBTM_STATUS BTM_BleSetHostFeature(uint16_t bit_num, uint8_t bit_val)
{
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_STATUS status = BTM_SUCCESS;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    BTM_TRACE_DEBUG("BTM_BleSetHostFeature bit_num %d bit_value %d\n", bit_num, bit_val);

    if ((err = btsnd_hcic_ble_set_host_feature(bit_num, bit_val)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("set host feature, cmd err=0x%x", err);
        status = BTM_HCI_ERROR | err;
    }

    cb_params.status = status;
    BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_SET_HOST_FEATURE_EVT, &cb_params);

    return status;
}
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)

#if (BT_BLE_FEAT_PAWR_EN == TRUE)
void BTM_BleSetPaSubeventData(UINT8 adv_handle, UINT8 num_subevents_with_data, uint8_t *subevent_params)
{
    tBTM_STATUS status = BTM_SUCCESS;
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if ((err = btsnd_hcic_ble_set_periodic_adv_subevt_data(adv_handle, num_subevents_with_data, (ble_subevent_params *)subevent_params)) != HCI_SUCCESS) {
        BTM_TRACE_WARNING("%s cmd err = 0x%x", __func__, err);
        status = BTM_HCI_ERROR | err;
    }

    cb_params.pa_subevt_data_evt.status = status;
    cb_params.pa_subevt_data_evt.adv_handle = adv_handle;

    BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_SET_PERIODIC_ADV_SUBEVT_DATA_EVT, &cb_params);
}

void BTM_BleSetPaResponseData(UINT16 sync_handle, UINT16 req_evt, UINT8 req_subevt, UINT8 rsp_subevt, UINT8 rsp_slot, UINT8 rsp_data_len, UINT8 *rsp_data)
{
    tBTM_STATUS status = BTM_SUCCESS;
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if ((err = btsnd_hcic_ble_set_periodic_adv_rsp_data(sync_handle, req_evt, req_subevt, rsp_subevt, rsp_slot, rsp_data_len, rsp_data)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("%s cmd err = 0x%x", __func__, err);
        status = BTM_HCI_ERROR | err;
    }

    cb_params.pa_rsp_data_evt.status = status;
    cb_params.pa_rsp_data_evt.sync_handle = sync_handle;

    BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_SET_PERIODIC_ADV_RESPONSE_DATA_EVT, &cb_params);
}

void BTM_BleSetPaSyncSubevt(UINT16 sync_handle, UINT16 periodic_adv_properties, UINT8 num_subevents_to_sync, UINT8 *subevt)
{
    tBTM_STATUS status = BTM_SUCCESS;
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if ((err = btsnd_hcic_ble_set_periodic_sync_subevt(sync_handle, periodic_adv_properties, num_subevents_to_sync, subevt)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("%s cmd err = 0x%x", __func__, err);
        status = BTM_HCI_ERROR | err;
    }

    cb_params.pa_sync_subevt_evt.status = status;
    cb_params.pa_sync_subevt_evt.sync_handle = sync_handle;

    BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_SET_PERIODIC_SYNC_SUBEVT_EVT, &cb_params);
}

void btm_ble_pa_subevt_data_req_evt(tBTM_BLE_PA_SUBEVT_DATA_REQ_EVT *params)
{
    BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_PERIODIC_ADV_SUBEVT_DATA_REQUEST_EVT, (tBTM_BLE_5_GAP_CB_PARAMS *)params);
}

void btm_ble_pa_rsp_rpt_evt(tBTM_BLE_PA_RSP_REPORT_EVT *params)
{
    BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_PERIODIC_ADV_RESPONSE_REPORT_EVT, (tBTM_BLE_5_GAP_CB_PARAMS *)params);
}

#endif // #if (BT_BLE_FEAT_PAWR_EN == TRUE)

#if (BT_BLE_FEAT_CHANNEL_SOUNDING == TRUE)
void BTM_BleCSReadLocalSuppCaps(void)
{
    btsnd_hcic_ble_cs_read_local_supported_caps();
}

void btm_ble_cs_read_local_supp_caps_cmpl_evt(uint8_t *p)
{
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    STREAM_TO_UINT8(cb_params.cs_read_local_supp_caps.status, p);
    if (cb_params.cs_read_local_supp_caps.status != HCI_SUCCESS) {
        BTM_TRACE_ERROR("%s cmd err = 0x%x", __func__, cb_params.cs_read_local_supp_caps.status);
        cb_params.cs_read_local_supp_caps.status |= BTM_HCI_ERROR;
        goto _error;
    }

    STREAM_TO_UINT16(cb_params.cs_read_local_supp_caps.conn_handle, p);
    STREAM_TO_UINT8(cb_params.cs_read_local_supp_caps.num_config_supported, p);
    STREAM_TO_UINT16(cb_params.cs_read_local_supp_caps.max_consecutive_proc_supported, p);
    STREAM_TO_UINT8(cb_params.cs_read_local_supp_caps.num_ant_supported, p);
    STREAM_TO_UINT8(cb_params.cs_read_local_supp_caps.max_ant_paths_supported, p);
    STREAM_TO_UINT8(cb_params.cs_read_local_supp_caps.roles_supported, p);
    STREAM_TO_UINT8(cb_params.cs_read_local_supp_caps.modes_supported, p);
    STREAM_TO_UINT8(cb_params.cs_read_local_supp_caps.rtt_capability, p);
    STREAM_TO_UINT8(cb_params.cs_read_local_supp_caps.rtt_aa_only_n, p);
    STREAM_TO_UINT8(cb_params.cs_read_local_supp_caps.rtt_sounding_n, p);
    STREAM_TO_UINT8(cb_params.cs_read_local_supp_caps.rtt_random_payload_n, p);
    STREAM_TO_UINT16(cb_params.cs_read_local_supp_caps.NADM_sounding_capability, p);
    STREAM_TO_UINT16(cb_params.cs_read_local_supp_caps.NADM_random_capability, p);
    STREAM_TO_UINT8(cb_params.cs_read_local_supp_caps.cs_sync_phys_supported, p);
    STREAM_TO_UINT16(cb_params.cs_read_local_supp_caps.subfeatures_supported, p);
    STREAM_TO_UINT16(cb_params.cs_read_local_supp_caps.T_IP1_times_supported, p);
    STREAM_TO_UINT16(cb_params.cs_read_local_supp_caps.T_IP2_times_supported, p);
    STREAM_TO_UINT16(cb_params.cs_read_local_supp_caps.T_FCS_times_supported, p);
    STREAM_TO_UINT16(cb_params.cs_read_local_supp_caps.T_PM_times_supported, p);
    STREAM_TO_UINT8(cb_params.cs_read_local_supp_caps.T_SW_times_supported, p);
    STREAM_TO_UINT8(cb_params.cs_read_local_supp_caps.TX_SNR_capability, p);

_error:

    BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_CS_READ_LOCAL_SUPP_CAPS_EVT, &cb_params);
}

void BTM_BleCSReadRemoteSuppCaps(UINT16 conn_handle)
{
    btsnd_hcic_ble_cs_read_remote_supported_capabilities(conn_handle);
}

void btm_ble_cs_read_remote_supp_caps_cmd_status(UINT8 status)
{
    if (status != HCI_SUCCESS) {
        tBTM_BLE_CS_READ_REMOTE_SUPP_CAPS_CMPL_EVT cs_read_remote_supp_caps = {0};
        cs_read_remote_supp_caps.status = (status | BTM_HCI_ERROR);

        BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_CS_READ_REMOTE_SUPP_CAPS_CMPL_EVT, (tBTM_BLE_5_GAP_CB_PARAMS *)&cs_read_remote_supp_caps);
    }
}

void BTM_BleGapWriteCachedRemoteSupportedCaps(UINT16 conn_handle, UINT8 num_config_supported, UINT16 max_consecutive_proc_supported,
                                                UINT8 num_ant_supported, UINT8 max_ant_paths_supported, UINT8 roles_supported,
                                                UINT8 modes_supported, UINT8 rtt_capability, UINT8 rtt_aa_only_n,
                                                UINT8 rtt_sounding_n, UINT8 rtt_random_payload_n, UINT16 NADM_sounding_capability,
                                                UINT16 NADM_random_capability, UINT8  cs_sync_phys_supported, UINT16 subfeatures_supported,
                                                UINT16 T_IP1_times_supported, UINT16 T_IP2_times_supported, UINT16 T_FCS_times_supported,
                                                UINT16 T_PM_times_supported, UINT8 T_SW_times_supported, UINT8 TX_SNR_capability)
{
    tBTM_STATUS status = BTM_SUCCESS;
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if ((err = btsnd_hcic_ble_cs_write_cached_remote_supported_capabilities(conn_handle, num_config_supported, max_consecutive_proc_supported,
                                                num_ant_supported, max_ant_paths_supported, roles_supported,
                                                modes_supported, rtt_capability, rtt_aa_only_n,
                                                rtt_sounding_n, rtt_random_payload_n, NADM_sounding_capability,
                                                NADM_random_capability, cs_sync_phys_supported, subfeatures_supported,
                                                T_IP1_times_supported, T_IP2_times_supported, T_FCS_times_supported,
                                                T_PM_times_supported, T_SW_times_supported, TX_SNR_capability)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("cs write cached remote support caps, cmd err=0x%x", err);
        status = BTM_HCI_ERROR | err;
    }

    cb_params.cs_write_cached_remote_supp_caps.status = status;
    cb_params.cs_write_cached_remote_supp_caps.conn_handle = conn_handle;
    BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_CS_WRITE_CACHED_REMOTE_SUPP_CAPS_EVT, &cb_params);
}

void BTM_BleGapCsSecurityEnable(UINT16 conn_handle)
{
    btsnd_hcic_ble_cs_security_enable(conn_handle);
}

void btm_ble_cs_security_enable_cmd_status(UINT8 status)
{
    tBTM_BLE_CS_SEC_ENABLE_CMPL_EVT cs_security_enable = {0};
    if (status != HCI_SUCCESS) {
        cs_security_enable.status = (status | BTM_HCI_ERROR);
        BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_CS_SECURITY_ENABLE_CMPL_EVT, (tBTM_BLE_5_GAP_CB_PARAMS *)&cs_security_enable);
    }
}

void BTM_BleGapCsSetDefaultSetting(UINT16 conn_handle, UINT8 role_enable, UINT8 cs_sync_ant_selection, INT8 max_tx_power)
{
    tBTM_STATUS status = BTM_SUCCESS;
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if ((err = btsnd_hcic_ble_cs_set_default_settings(conn_handle, role_enable, cs_sync_ant_selection, max_tx_power)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("cs set default setting, cmd err=0x%x", err);
        status = BTM_HCI_ERROR | err;
    }

    cb_params.cs_set_default_settings.status = status;
    cb_params.cs_set_default_settings.conn_handle = conn_handle;
    BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_CS_SET_DEFAULT_SETTINGS_EVT, &cb_params);
}

void BTM_BleGapCsReadRemoteFaeTable(UINT16 conn_handle)
{
    btsnd_hcic_ble_cs_read_remote_fae_table(conn_handle);
}

void btm_ble_cs_read_remote_fae_table_cmd_status(UINT8 status)
{
    tBTM_BLE_CS_READ_REMOTE_FAE_TAB_CMPL_EVT cs_read_remote_fae_tab = {0};
    if (status != HCI_SUCCESS) {
        cs_read_remote_fae_tab.status = (status | BTM_HCI_ERROR);
        BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_CS_READ_REMOTE_FAE_TABLE_CMPL_EVT, (tBTM_BLE_5_GAP_CB_PARAMS *)&cs_read_remote_fae_tab);
    }
}

void BTM_BleGapWriteCachedRemoteFaeTable(UINT16 conn_handle, UINT8 *remote_fae_table)
{
    tBTM_STATUS status = BTM_SUCCESS;
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if ((err = btsnd_hcic_ble_cs_write_cached_remote_fae_table(conn_handle, remote_fae_table)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("cs write cached remote fae tab, cmd err=0x%x", err);
        status = BTM_HCI_ERROR | err;
    }

    cb_params.cs_write_cached_remote_fae_tab.status = status;
    cb_params.cs_write_cached_remote_fae_tab.conn_handle = conn_handle;
    BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_CS_WRITE_CACHED_REMOTE_FAE_TAB_EVT, &cb_params);
}

void BTM_BleGapCsCreateConfig(UINT16 conn_handle, UINT8 config_id, UINT8 create_context,
                                UINT8 main_mode_type, UINT8 sub_mode_type, UINT8 min_main_mode_steps,
                                UINT8 max_main_mode_steps, UINT8 main_mode_repetition, UINT8 mode_0_steps,
                                UINT8 role, UINT8 rtt_type, UINT8 cs_sync_phy, UINT8 *channel_map,
                                UINT8 channel_map_repetition, UINT8 channel_selection_type, UINT8 ch3c_shape,
                                UINT8 ch3c_jump,UINT8 reserved)
{
    btsnd_hcic_ble_cs_create_config(conn_handle, config_id, create_context,
                                main_mode_type, sub_mode_type, min_main_mode_steps,
                                max_main_mode_steps, main_mode_repetition, mode_0_steps,
                                role, rtt_type, cs_sync_phy, channel_map,
                                channel_map_repetition, channel_selection_type, ch3c_shape,
                                ch3c_jump, reserved);
}

void BTM_BleGapCsRemoveConfig(UINT16 conn_handle, UINT8 config_id)
{
    btsnd_hcic_ble_cs_remove_config(conn_handle, config_id);
}

void btm_ble_cs_update_config_cmd_status(UINT8 status, BOOLEAN create)
{
    tBTM_BLE_CS_CONFIG_CMPL_EVT config_cmpl = {0};
    if (status != HCI_SUCCESS) {
        config_cmpl.status = (status | BTM_HCI_ERROR);
        config_cmpl.action = (create? 0x01:0x00);
        BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_CS_CONFIG_CMPL_EVT, (tBTM_BLE_5_GAP_CB_PARAMS *)&config_cmpl);
    }
}

void BTM_BleGapCsSetChannelClass(UINT8 *channel_class)
{
    tBTM_STATUS status = BTM_SUCCESS;
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if ((err = btsnd_hcic_ble_cs_set_channel_classification(channel_class)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("cs set channel class, cmd err=0x%x", err);
        status = BTM_HCI_ERROR | err;
    }

    cb_params.status = status;
    BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_CS_SET_CHANNEL_CLASS_CMPL_EVT, &cb_params);
}

void BTM_BleGapCsSetProcPatams(UINT16 conn_handle, UINT8 config_id, UINT16 max_procedure_len,
                                UINT16 min_procedure_interval, UINT16 max_procedure_interval,
                                UINT16 max_procedure_count, UINT32 min_subevent_len,
                                UINT32 max_subevent_len, UINT8 tone_ant_config_selection,
                                UINT8 phy, UINT8 tx_power_delta, UINT8 preferred_peer_antenna,
                                UINT8 SNR_control_initiator, UINT8 SNR_control_reflector)
{
    tBTM_STATUS status = BTM_SUCCESS;
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if ((err = btsnd_hcic_ble_cs_set_procedure_params(conn_handle, config_id, max_procedure_len,
                                min_procedure_interval, max_procedure_interval,
                                max_procedure_count, min_subevent_len,
                                max_subevent_len, tone_ant_config_selection,
                                phy, tx_power_delta, preferred_peer_antenna,
                                SNR_control_initiator, SNR_control_reflector)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("cs set procedure params, cmd err=0x%x", err);
        status = BTM_HCI_ERROR | err;
    }

    cb_params.cs_set_proc_params.status = status;
    cb_params.cs_set_proc_params.conn_handle = conn_handle;
    BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_CS_PROC_PARAMS_CMPL_EVT, &cb_params);
}

void BTM_BleGapCsProcEnable(UINT16 conn_handle, UINT8 config_id, UINT8 enable)
{
    btsnd_hcic_ble_cs_procedure_enable(conn_handle, config_id, enable);
}

void btm_ble_cs_read_remote_supp_caps_cmpl_evt(tBTM_BLE_CS_READ_REMOTE_SUPP_CAPS_CMPL_EVT *cs_read_remote_supp_caps)
{
    if (cs_read_remote_supp_caps->status != HCI_SUCCESS) {
        BTM_TRACE_ERROR("%s cmd err = 0x%x", __func__, cs_read_remote_supp_caps->status);
        cs_read_remote_supp_caps->status |= BTM_HCI_ERROR;
    }

    BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_CS_READ_REMOTE_SUPP_CAPS_CMPL_EVT, (tBTM_BLE_5_GAP_CB_PARAMS *)cs_read_remote_supp_caps);
}

void btm_ble_cs_read_remote_fae_tab_cmpl_evt(tBTM_BLE_CS_READ_REMOTE_FAE_TAB_CMPL_EVT *cs_read_remote_fae_tab)
{
    if (cs_read_remote_fae_tab->status != HCI_SUCCESS) {
        BTM_TRACE_ERROR("%s cmd err = 0x%x", __func__, cs_read_remote_fae_tab->status);
        cs_read_remote_fae_tab->status |= BTM_HCI_ERROR;
    }

    BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_CS_READ_REMOTE_FAE_TABLE_CMPL_EVT, (tBTM_BLE_5_GAP_CB_PARAMS *)cs_read_remote_fae_tab);
}

void btm_ble_cs_securuty_enable_cmpl_evt(tBTM_BLE_CS_SEC_ENABLE_CMPL_EVT *cs_security_enable)
{
    if (cs_security_enable->status != HCI_SUCCESS) {
        BTM_TRACE_ERROR("%s cmd err = 0x%x", __func__, cs_security_enable->status);
        cs_security_enable->status |= BTM_HCI_ERROR;
    }

    BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_CS_SECURITY_ENABLE_CMPL_EVT, (tBTM_BLE_5_GAP_CB_PARAMS *)cs_security_enable);
}

void btm_ble_cs_config_cmpl_evt(tBTM_BLE_CS_CONFIG_CMPL_EVT *config_cmpl)
{
    if (config_cmpl->status != HCI_SUCCESS) {
        BTM_TRACE_ERROR("%s cmd err = 0x%x", __func__, config_cmpl->status);
        config_cmpl->status |= BTM_HCI_ERROR;
    }

    BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_CS_CONFIG_CMPL_EVT, (tBTM_BLE_5_GAP_CB_PARAMS *)config_cmpl);
}

void btm_ble_cs_proc_enable_cmpl_evt(tBTM_BLE_CS_PROC_ENABLE_CMPL_EVT *proc_en)
{
    if (proc_en->status != HCI_SUCCESS) {
        BTM_TRACE_ERROR("%s cmd err = 0x%x", __func__, proc_en->status);
        proc_en->status |= BTM_HCI_ERROR;
    }

    BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_CS_PROC_ENABLE_CMPL_EVT, (tBTM_BLE_5_GAP_CB_PARAMS *)proc_en);
}

void btm_ble_cs_subevt_result_evt(tBTM_BLE_CS_SUBEVT_RESULT_CMPL_EVT *subevt_result)
{
    BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_CS_SUBEVENT_RESULT_EVT, (tBTM_BLE_5_GAP_CB_PARAMS *)subevt_result);
}

void btm_ble_cs_subevt_continue_result_evt(tBTM_BLE_CS_SUBEVT_RESULT_CONTINUE_EVT *subevt_result_continue)
{
    BTM_ExtBleCallbackTrigger(BTM_BLE_GAP_CS_SUBEVENT_RESULT_CONTINUE_EVT, (tBTM_BLE_5_GAP_CB_PARAMS *)subevt_result_continue);
}

#endif // (BT_BLE_FEAT_CHANNEL_SOUNDING == TRUE)
