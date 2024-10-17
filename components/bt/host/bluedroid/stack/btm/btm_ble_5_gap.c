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

void btm_ble_advrecod_init(void)
{
    memset(&adv_record[0], 0, sizeof(tBTM_EXT_ADV_RECORD)*MAX_BLE_ADV_INSTANCE);
}

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

    if ((err = btsnd_hcic_ble_set_ext_adv_params(instance, params->type, params->interval_min, params->interval_max,
                                      params->channel_map, params->own_addr_type, params->peer_addr_type,
                                      params->peer_addr, params->filter_policy, params->tx_power,
                                      params->primary_phy, params->max_skip,
                                      params->secondary_phy, params->sid, params->scan_req_notif)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("LE EA SetParams: cmd err=0x%x", err);
        status = BTM_HCI_ERROR | err;
        goto end;
    }

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

tBTM_STATUS BTM_BlePeriodicAdvSetParams(UINT8 instance, tBTM_BLE_Periodic_Adv_Params *params)
{
    tBTM_STATUS status = BTM_SUCCESS;
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};
    //ext_adv_flag = true;

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

    if ((err= btsnd_hcic_ble_set_periodic_adv_params(instance, params->interval_min,
                                               params->interval_max, params->properties)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("LE PA SetParams: cmd err=0x%x", err);
        status = BTM_HCI_ERROR | err;
    }

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
        #if (CONFIG_BT_BLE_FEAT_CREATE_SYNC_ENH)
        || (params->reports_disabled > 0x01)
        || (params->filter_duplicates > 0x01)
        #endif
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

    #if (CONFIG_BT_BLE_FEAT_CREATE_SYNC_ENH)
    if (params->reports_disabled) {
        SET_BIT(option, 1);
    }
    if (params->filter_duplicates) {
        SET_BIT(option, 2);
    }
    #endif

    if (!btsnd_hcic_ble_periodic_adv_create_sync(option, params->sid, params->addr_type,
                                            params->addr, params->sync_timeout, 0)) {
        BTM_TRACE_ERROR("LE PA CreateSync cmd failed");
        status = BTM_ILLEGAL_VALUE;
    }

end:
    if(status != BTM_SUCCESS) {
        cb_params.status = status;
        BTM_ExtBleCallbackTrigger(BTM_BLE_5_GAP_PERIODIC_ADV_CREATE_SYNC_COMPLETE_EVT, &cb_params);
    }

    return status;
}

void btm_set_phy_callback(UINT8 status)
{
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};
    cb_params.status = status;

    BTM_ExtBleCallbackTrigger(BTM_BLE_5_GAP_SET_PREFERED_PHY_COMPLETE_EVT, &cb_params);

}

void btm_create_sync_callback(UINT8 status)
{
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};
    cb_params.status = status;

    BTM_ExtBleCallbackTrigger(BTM_BLE_5_GAP_PERIODIC_ADV_CREATE_SYNC_COMPLETE_EVT, &cb_params);
}

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

tBTM_STATUS BTM_BlePeriodicAdvSyncCancel(void)
{
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_STATUS status = BTM_SUCCESS;
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

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

void BTM_BleSetPreferExtenedConnParams (BD_ADDR bd_addr, tBTM_EXT_CONN_PARAMS *params)
{
    tBTM_SEC_DEV_REC  *p_dev_rec = btm_find_or_alloc_dev (bd_addr);

    if (p_dev_rec) {
        if (params) {
            memcpy(&p_dev_rec->ext_conn_params, params, sizeof(tBTM_EXT_CONN_PARAMS));
        } else {
            BTM_TRACE_ERROR("Invalid Extand connection parameters");
        }
    } else {
            BTM_TRACE_ERROR("Unknown Device, setting rejected");
    }

    return;
}

void btm_ble_extended_init(void)
{

}

void btm_ble_extended_cleanup(void)
{

}

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

void btm_ble_scan_timeout_evt(void)
{
    BTM_ExtBleCallbackTrigger(BTM_BLE_5_GAP_SCAN_TIMEOUT_EVT, NULL);
}

void btm_ble_adv_set_terminated_evt(tBTM_BLE_ADV_TERMINAT *params)
{
    tBTM_BLE_5_GAP_CB_PARAMS cb_params = {0};

    if (!params) {
        BTM_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    // adv terminated due to connection, save the adv handle and connection handle
    if(params->completed_event == 0x00) {
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

    memcpy(&cb_params.sync_estab, params, sizeof(tBTM_BLE_PERIOD_ADV_SYNC_ESTAB));

    // If the user has register the callback function, should callback it to the application.
    BTM_ExtBleCallbackTrigger(BTM_BLE_5_GAP_PERIODIC_ADV_SYNC_ESTAB_EVT, &cb_params);

    return;

}

uint8_t btm_ble_ext_adv_active_count(void)
{
    uint8_t count = 0;

    for (uint8_t i = 0; i < MAX_BLE_ADV_INSTANCE; i++) {
        if (adv_record[i].enabled == true) {
            count++;
        }
    }

    return count;
}

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
