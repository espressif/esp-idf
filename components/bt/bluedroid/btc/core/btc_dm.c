#include <stdlib.h>
#include <string.h>
#include "btc_common.h"
#include "btc_dm.h"
#include "btc_main.h"
#include "bt_trace.h"
#include "bt_target.h"
#include "btc_storage.h"
#include "bta_api.h"


/******************************************************************************
**  Constants & Macros
******************************************************************************/
#define BTA_SERVICE_ID_TO_SERVICE_MASK(id)       (1 << (id))

/******************************************************************************
**  Static variables
******************************************************************************/
static tBTA_SERVICE_MASK btc_enabled_services = 0;
/******************************************************************************
**  Static functions
******************************************************************************/
/******************************************************************************
**  Externs
******************************************************************************/
extern bt_status_t btc_av_execute_service(BOOLEAN b_enable);
extern bt_status_t btc_av_sink_execute_service(BOOLEAN b_enable);

/******************************************************************************
**  Functions
******************************************************************************/
static void btc_dm_sec_arg_deep_free(btc_msg_t *msg)
{
    btc_dm_sec_args_t *arg = (btc_dm_sec_args_t *)(msg->arg);
    if (msg->act == BTA_DM_BLE_KEY_EVT) {
        osi_free(arg->sec.ble_key.p_key_value);
    }
}

void btc_dm_sec_arg_deep_copy(btc_msg_t *msg, void *dst, void *src)
{
    tBTA_DM_SEC *dst_dm_sec = (tBTA_DM_SEC *)dst;
    tBTA_DM_SEC *src_dm_sec = (tBTA_DM_SEC *)src;

    if (!src_dm_sec) {
        return;
    }

    assert(dst_dm_sec);
    memcpy(dst_dm_sec, src_dm_sec, sizeof(tBTA_DM_SEC));

    if (msg->act == BTA_DM_BLE_KEY_EVT) {
        dst_dm_sec->ble_key.p_key_value = osi_malloc(sizeof(tBTM_LE_KEY_VALUE));
        assert(src_dm_sec->ble_key.p_key_value);
        assert(dst_dm_sec->ble_key.p_key_value);
        memcpy(dst_dm_sec->ble_key.p_key_value, src_dm_sec->ble_key.p_key_value, sizeof(tBTM_LE_KEY_VALUE));
    }
}

/*******************************************************************************
**
** Function         btc_dm_evt
**
** Description      Switches context from BTE to BTC for all DM events
**
** Returns          void
**
*******************************************************************************/

void btc_dm_sec_evt(tBTA_DM_SEC_EVT event, tBTA_DM_SEC *data)
{
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_DM_SEC;
    msg.act = event;
        
    btc_transfer_context(&msg, (btc_dm_sec_args_t *)data, sizeof(btc_dm_sec_args_t), btc_dm_sec_arg_deep_copy);
}

static void btc_enable_bluetooth_evt(tBTA_STATUS status)
{
    if (status == BTA_SUCCESS) {
        future_ready(*btc_main_get_future_p(BTC_MAIN_ENABLE_FUTURE), FUTURE_SUCCESS);
    } else {
        future_ready(*btc_main_get_future_p(BTC_MAIN_ENABLE_FUTURE), FUTURE_FAIL);
    }
}

static void btc_disable_bluetooth_evt(void)
{
    LOG_DEBUG("%s", __FUNCTION__);

    future_ready(*btc_main_get_future_p(BTC_MAIN_DISABLE_FUTURE), FUTURE_SUCCESS);
}

static void btc_dm_auth_cmpl_evt (tBTA_DM_AUTH_CMPL *p_auth_cmpl)
{
    /* Save link key, if not temporary */
    bt_bdaddr_t bd_addr;
    bt_status_t status;
    LOG_DEBUG("%s: bond state success %d, present %d, type%d\n", __func__, p_auth_cmpl->success,
		p_auth_cmpl->key_present, p_auth_cmpl->key_type);

    bdcpy(bd_addr.address, p_auth_cmpl->bd_addr);
    if ( (p_auth_cmpl->success == TRUE) && (p_auth_cmpl->key_present) )
    {
	#if 0
        if ((p_auth_cmpl->key_type < HCI_LKEY_TYPE_DEBUG_COMB) ||
            (p_auth_cmpl->key_type == HCI_LKEY_TYPE_AUTH_COMB) ||
            (p_auth_cmpl->key_type == HCI_LKEY_TYPE_CHANGED_COMB) ||
            (p_auth_cmpl->key_type == HCI_LKEY_TYPE_AUTH_COMB_P_256)
            )
        #endif
	if (1)
        {
            bt_status_t ret;
            LOG_DEBUG("%s: Storing link key. key_type=0x%x",
			     __FUNCTION__, p_auth_cmpl->key_type);
            ret = btc_storage_add_bonded_device(&bd_addr,
                                p_auth_cmpl->key, p_auth_cmpl->key_type,
                                16);
            BTC_ASSERTC(ret == BT_STATUS_SUCCESS, "storing link key failed", ret);
        }
        else
        {
            LOG_DEBUG("%s: Temporary key. Not storing. key_type=0x%x",
                __FUNCTION__, p_auth_cmpl->key_type);
        }
    }

    // Skip SDP for certain  HID Devices
    if (p_auth_cmpl->success)
    {
    }
    else
    {
        // Map the HCI fail reason  to  bt status
        switch(p_auth_cmpl->fail_reason)
        {
            case HCI_ERR_PAGE_TIMEOUT:
		LOG_WARN("%s() - Pairing timeout; retrying () ...", __FUNCTION__);
		return;
                /* Fall-through */
            case HCI_ERR_CONNECTION_TOUT:
                status =  BT_STATUS_RMT_DEV_DOWN;
                break;

            case HCI_ERR_PAIRING_NOT_ALLOWED:
                status = BT_STATUS_AUTH_REJECTED;
                break;

            case HCI_ERR_LMP_RESPONSE_TIMEOUT:
                status =  BT_STATUS_AUTH_FAILURE;
                break;

            /* map the auth failure codes, so we can retry pairing if necessary */
            case HCI_ERR_AUTH_FAILURE:
            case HCI_ERR_KEY_MISSING:
                btc_storage_remove_bonded_device(&bd_addr);
            case HCI_ERR_HOST_REJECT_SECURITY:
            case HCI_ERR_ENCRY_MODE_NOT_ACCEPTABLE:
            case HCI_ERR_UNIT_KEY_USED:
            case HCI_ERR_PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED:
            case HCI_ERR_INSUFFCIENT_SECURITY:
            case HCI_ERR_PEER_USER:
            case HCI_ERR_UNSPECIFIED:
                LOG_DEBUG(" %s() Authentication fail reason %d",
                    __FUNCTION__, p_auth_cmpl->fail_reason);
                    /* if autopair attempts are more than 1, or not attempted */
                    status =  BT_STATUS_AUTH_FAILURE;
                break;
            default:
                status =  BT_STATUS_FAIL;
        }
    }
    (void) status;
}

tBTA_SERVICE_MASK btc_get_enabled_services_mask(void)
{
    return btc_enabled_services;
}

static bt_status_t btc_in_execute_service_request(tBTA_SERVICE_ID service_id,
        BOOLEAN b_enable)
{
    LOG_DEBUG("%s service_id: %d\n", __FUNCTION__, service_id);
    /* Check the service_ID and invoke the profile's BT state changed API */
    switch (service_id) {
    case BTA_A2DP_SOURCE_SERVICE_ID:
        btc_av_execute_service(b_enable);
        break;
    case BTA_A2DP_SINK_SERVICE_ID:
        btc_av_sink_execute_service(b_enable);
        break;
    default:
        LOG_ERROR("%s: Unknown service being enabled\n", __FUNCTION__);
        return BT_STATUS_FAIL;
    }
    return BT_STATUS_SUCCESS;
}

void btc_dm_execute_service_request(BOOLEAN enable, char *p_param)
{
    btc_in_execute_service_request(*((tBTA_SERVICE_ID *)p_param), enable);
}

bt_status_t btc_dm_enable_service(tBTA_SERVICE_ID service_id)
{
    tBTA_SERVICE_ID *p_id = &service_id;

    btc_enabled_services |= (1 << service_id);

    LOG_DEBUG("%s: current services:0x%x", __FUNCTION__, btc_enabled_services);

    btc_dm_execute_service_request(TRUE, (char *)p_id);

    return BT_STATUS_SUCCESS;
}

bt_status_t btc_dm_disable_service(tBTA_SERVICE_ID service_id)
{
    tBTA_SERVICE_ID *p_id = &service_id;

    btc_enabled_services &= (tBTA_SERVICE_MASK)(~(1 << service_id));

    LOG_DEBUG("%s: Current Services:0x%x", __FUNCTION__, btc_enabled_services);

    btc_dm_execute_service_request(FALSE, (char *)p_id);

    return BT_STATUS_SUCCESS;
}

void btc_dm_sec_cb_handler(btc_msg_t *msg)
{
    btc_dm_sec_args_t *arg = (btc_dm_sec_args_t *)(msg->arg);
    tBTA_DM_SEC *p_data = &(arg->sec);
    // tBTA_SERVICE_MASK service_mask;
    LOG_DEBUG("btif_dm_upstreams_cback  ev: %d\n", msg->act);

    switch (msg->act) {
    case BTA_DM_ENABLE_EVT:
#if KARL_NOT_IGNORE
        /* for each of the enabled services in the mask, trigger the profile
         * enable */
        service_mask = btif_get_enabled_services_mask();
        for (int i = 0; i <= BTA_MAX_SERVICE_ID; i++) {
            if (service_mask &
                    (tBTA_SERVICE_MASK)(BTA_SERVICE_ID_TO_SERVICE_MASK(i))) {
                btif_in_execute_service_request(i, TRUE);
            }
        }
        btif_enable_bluetooth_evt(p_data->enable.status);
#endif /* KARL_NOT_IGNORE */
	btc_storage_load_bonded_devices();
        btc_enable_bluetooth_evt(p_data->enable.status);
        break;
    case BTA_DM_DISABLE_EVT:
#if KARL_NOT_IGNORE
        /* for each of the enabled services in the mask, trigger the profile
         * disable */
        service_mask = btif_get_enabled_services_mask();
        for (int i = 0; i <= BTA_MAX_SERVICE_ID; i++) {
            if (service_mask &
                    (tBTA_SERVICE_MASK)(BTA_SERVICE_ID_TO_SERVICE_MASK(i))) {
                btif_in_execute_service_request(i, FALSE);
            }
        }
        btif_disable_bluetooth_evt();
#endif /* KARL_NOT_IGNORE */
        btc_disable_bluetooth_evt();
        break;
    case BTA_DM_PIN_REQ_EVT:
	break;
    case BTA_DM_AUTH_CMPL_EVT:
	btc_dm_auth_cmpl_evt(&p_data->auth_cmpl);
	break;
    case BTA_DM_BOND_CANCEL_CMPL_EVT:
    case BTA_DM_SP_CFM_REQ_EVT:
    case BTA_DM_SP_KEY_NOTIF_EVT:

    case BTA_DM_DEV_UNPAIRED_EVT:
    case BTA_DM_BUSY_LEVEL_EVT:
    case BTA_DM_LINK_UP_EVT:
    case BTA_DM_LINK_DOWN_EVT:
    case BTA_DM_HW_ERROR_EVT:

#if (defined(BLE_INCLUDED) && (BLE_INCLUDED == TRUE))
    case BTA_DM_BLE_KEY_EVT:
    case BTA_DM_BLE_SEC_REQ_EVT:
    case BTA_DM_BLE_PASSKEY_NOTIF_EVT:
    case BTA_DM_BLE_PASSKEY_REQ_EVT:
    case BTA_DM_BLE_NC_REQ_EVT:
    case BTA_DM_BLE_OOB_REQ_EVT:
    case BTA_DM_BLE_LOCAL_IR_EVT:
    case BTA_DM_BLE_LOCAL_ER_EVT:
    case BTA_DM_BLE_AUTH_CMPL_EVT:
    case BTA_DM_LE_FEATURES_READ:
    case BTA_DM_ENER_INFO_READ:
#endif

    case BTA_DM_AUTHORIZE_EVT:
    case BTA_DM_SIG_STRENGTH_EVT:
    case BTA_DM_SP_RMT_OOB_EVT:
    case BTA_DM_SP_KEYPRESS_EVT:
    case BTA_DM_ROLE_CHG_EVT:

    default:
        LOG_WARN( "btc_dm_sec_cback : unhandled event (%d)\n", msg->act );
        break;
    }

    btc_dm_sec_arg_deep_free(msg);
}
