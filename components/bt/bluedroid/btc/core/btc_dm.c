#include <stdlib.h>
#include <string.h>
#include "btc_dm.h"
#include "btc_main.h"
#include "bt_trace.h"
#include "bt_target.h"
#include "btif_storage.h" // TODO: replace with "btc"

extern void btif_dm_auth_cmpl_evt (tBTA_DM_AUTH_CMPL *p_auth_cmpl); // todo: replace with "btc_xx"

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
	btif_storage_load_bonded_devices();
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
	btif_dm_auth_cmpl_evt(&p_data->auth_cmpl);
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
