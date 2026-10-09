/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "common/bt_target.h"
#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_avrc_api.h"
#include "esp_bt_main.h"
#include "btc/btc_manage.h"
#include "btc_avrc.h"

#if BTC_AV_INCLUDED

#define BT_AVRC_TAG "BT_AVRC"

static esp_err_t avrc_ct_check_user_tl(uint8_t tl)
{
    if (tl <= ESP_USER_AVRC_TRANS_LABEL_MAX) {
        return ESP_OK;
    }
    if (tl <= ESP_AVRC_TRANS_LABEL_MAX) {
        ESP_LOGE(BT_AVRC_TAG, "transaction label %u is reserved (14 and 15 are used by the stack for Play/Pause); use 0-%u",
                 tl, ESP_USER_AVRC_TRANS_LABEL_MAX);
    }
    return ESP_ERR_INVALID_ARG;
}

static esp_err_t avrc_ct_transfer(uint8_t act, const btc_avrc_args_t *arg)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_AVRC_CT;
    msg.act = act;
    bt_status_t stat = btc_transfer_context(&msg, (void *)arg, sizeof(*arg), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_avrc_ct_register_callback(esp_avrc_ct_cb_t callback)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (callback == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_profile_cb_set(BTC_PID_AVRC_CT, callback);
    return ESP_OK;
}

esp_err_t esp_avrc_ct_init(void)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_AVRC_CT;
    msg.act = BTC_AVRC_CT_API_INIT_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_avrc_ct_deinit(void)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_AVRC_CT;
    msg.act = BTC_AVRC_CT_API_DEINIT_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

static esp_err_t avrc_ct_send_set_player_value(esp_a2d_conn_hdl_t conn_hdl, uint8_t tl, uint8_t attr_id, uint8_t value_id)
{
    if (tl > ESP_AVRC_TRANS_LABEL_MAX || attr_id > ESP_AVRC_PS_MAX_ATTR - 1) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_avrc_args_t arg;
    memset(&arg, 0, sizeof(arg));
    arg.ps_cmd.conn_hdl = conn_hdl;
    arg.ps_cmd.tl = tl;
    arg.ps_cmd.attr_id = attr_id;
    arg.ps_cmd.value_id = value_id;
    return avrc_ct_transfer(BTC_AVRC_CTRL_API_SND_SET_PLAYER_SETTING_EVT, &arg);
}

esp_err_t esp_avrc_ct_send_set_player_value_cmd_by_handle(esp_a2d_conn_hdl_t conn_hdl, uint8_t tl,
                                                          uint8_t attr_id, uint8_t value_id)
{
    if (conn_hdl == ESP_A2D_INVALID_CONN_HDL || avrc_ct_check_user_tl(tl) != ESP_OK) {
        return ESP_ERR_INVALID_ARG;
    }
    return avrc_ct_send_set_player_value(conn_hdl, tl, attr_id, value_id);
}

esp_err_t esp_avrc_ct_send_set_player_value_cmd(uint8_t tl, uint8_t attr_id, uint8_t value_id)
{
    return avrc_ct_send_set_player_value(ESP_A2D_INVALID_CONN_HDL, tl, attr_id, value_id);
}

static esp_err_t avrc_ct_send_get_rn_caps(esp_a2d_conn_hdl_t conn_hdl, uint8_t tl)
{
    if (tl > ESP_AVRC_TRANS_LABEL_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_avrc_args_t arg;
    memset(&arg, 0, sizeof(arg));
    arg.get_caps_cmd.conn_hdl = conn_hdl;
    arg.get_caps_cmd.tl = tl;
    return avrc_ct_transfer(BTC_AVRC_STATUS_API_SND_GET_RN_CAPS_EVT, &arg);
}

esp_err_t esp_avrc_ct_send_get_rn_capabilities_cmd_by_handle(esp_a2d_conn_hdl_t conn_hdl, uint8_t tl)
{
    if (conn_hdl == ESP_A2D_INVALID_CONN_HDL || avrc_ct_check_user_tl(tl) != ESP_OK) {
        return ESP_ERR_INVALID_ARG;
    }
    return avrc_ct_send_get_rn_caps(conn_hdl, tl);
}

esp_err_t esp_avrc_ct_send_get_rn_capabilities_cmd(uint8_t tl)
{
    return avrc_ct_send_get_rn_caps(ESP_A2D_INVALID_CONN_HDL, tl);
}

static esp_err_t avrc_ct_send_reg_notify(esp_a2d_conn_hdl_t conn_hdl, uint8_t tl, uint8_t event_id, uint32_t event_parameter)
{
    if (tl > ESP_AVRC_TRANS_LABEL_MAX || event_id > ESP_AVRC_RN_MAX_EVT - 1) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!btc_avrc_ct_rn_evt_supported(event_id)) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    btc_avrc_args_t arg;
    memset(&arg, 0, sizeof(arg));
    arg.rn_cmd.conn_hdl = conn_hdl;
    arg.rn_cmd.tl = tl;
    arg.rn_cmd.event_id = event_id;
    arg.rn_cmd.event_parameter = event_parameter;
    return avrc_ct_transfer(BTC_AVRC_NOTIFY_API_SND_REG_NOTIFY_EVT, &arg);
}

esp_err_t esp_avrc_ct_send_register_notification_cmd_by_handle(esp_a2d_conn_hdl_t conn_hdl, uint8_t tl,
                                                               uint8_t event_id, uint32_t event_parameter)
{
    if (conn_hdl == ESP_A2D_INVALID_CONN_HDL || avrc_ct_check_user_tl(tl) != ESP_OK) {
        return ESP_ERR_INVALID_ARG;
    }
    return avrc_ct_send_reg_notify(conn_hdl, tl, event_id, event_parameter);
}

esp_err_t esp_avrc_ct_send_register_notification_cmd(uint8_t tl, uint8_t event_id, uint32_t event_parameter)
{
    return avrc_ct_send_reg_notify(ESP_A2D_INVALID_CONN_HDL, tl, event_id, event_parameter);
}

static esp_err_t avrc_ct_send_abs_vol(esp_a2d_conn_hdl_t conn_hdl, uint8_t tl, uint8_t volume)
{
    if (tl > ESP_AVRC_TRANS_LABEL_MAX || volume > BTC_AVRC_MAX_VOLUME) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!btc_avrc_ct_rn_evt_supported(ESP_AVRC_RN_VOLUME_CHANGE)) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    btc_avrc_args_t arg;
    memset(&arg, 0, sizeof(arg));
    arg.set_abs_vol_cmd.conn_hdl = conn_hdl;
    arg.set_abs_vol_cmd.tl = tl;
    arg.set_abs_vol_cmd.volume = volume;
    return avrc_ct_transfer(BTC_AVRC_CTRL_API_SND_SET_ABSOLUTE_VOLUME_EVT, &arg);
}

esp_err_t esp_avrc_ct_send_set_absolute_volume_cmd_by_handle(esp_a2d_conn_hdl_t conn_hdl, uint8_t tl, uint8_t volume)
{
    if (conn_hdl == ESP_A2D_INVALID_CONN_HDL || avrc_ct_check_user_tl(tl) != ESP_OK) {
        return ESP_ERR_INVALID_ARG;
    }
    return avrc_ct_send_abs_vol(conn_hdl, tl, volume);
}

esp_err_t esp_avrc_ct_send_set_absolute_volume_cmd(uint8_t tl, uint8_t volume)
{
    return avrc_ct_send_abs_vol(ESP_A2D_INVALID_CONN_HDL, tl, volume);
}

static esp_err_t avrc_ct_send_metadata(esp_a2d_conn_hdl_t conn_hdl, uint8_t tl, uint8_t attr_mask)
{
    if (tl > ESP_AVRC_TRANS_LABEL_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_avrc_args_t arg;
    memset(&arg, 0, sizeof(arg));
    arg.md_cmd.conn_hdl = conn_hdl;
    arg.md_cmd.tl = tl;
    arg.md_cmd.attr_mask = attr_mask;
    return avrc_ct_transfer(BTC_AVRC_STATUS_API_SND_META_EVT, &arg);
}

esp_err_t esp_avrc_ct_send_metadata_cmd_by_handle(esp_a2d_conn_hdl_t conn_hdl, uint8_t tl, uint8_t attr_mask)
{
    if (conn_hdl == ESP_A2D_INVALID_CONN_HDL || avrc_ct_check_user_tl(tl) != ESP_OK) {
        return ESP_ERR_INVALID_ARG;
    }
    return avrc_ct_send_metadata(conn_hdl, tl, attr_mask);
}

esp_err_t esp_avrc_ct_send_metadata_cmd(uint8_t tl, uint8_t attr_mask)
{
    return avrc_ct_send_metadata(ESP_A2D_INVALID_CONN_HDL, tl, attr_mask);
}

static esp_err_t avrc_ct_send_pt(esp_a2d_conn_hdl_t conn_hdl, uint8_t tl, uint8_t key_code, uint8_t key_state)
{
    if (tl > ESP_AVRC_TRANS_LABEL_MAX || key_state > ESP_AVRC_PT_CMD_STATE_RELEASED) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_avrc_args_t arg;
    memset(&arg, 0, sizeof(arg));
    arg.pt_cmd.conn_hdl = conn_hdl;
    arg.pt_cmd.tl = tl;
    arg.pt_cmd.key_code = key_code;
    arg.pt_cmd.key_state = key_state;
    return avrc_ct_transfer(BTC_AVRC_CTRL_API_SND_PTCMD_EVT, &arg);
}

esp_err_t esp_avrc_ct_send_passthrough_cmd_by_handle(esp_a2d_conn_hdl_t conn_hdl, uint8_t tl, uint8_t key_code,
                                                     uint8_t key_state)
{
    if (conn_hdl == ESP_A2D_INVALID_CONN_HDL || avrc_ct_check_user_tl(tl) != ESP_OK) {
        return ESP_ERR_INVALID_ARG;
    }
    return avrc_ct_send_pt(conn_hdl, tl, key_code, key_state);
}

esp_err_t esp_avrc_ct_send_passthrough_cmd(uint8_t tl, uint8_t key_code, uint8_t key_state)
{
    return avrc_ct_send_pt(ESP_A2D_INVALID_CONN_HDL, tl, key_code, key_state);
}

static esp_err_t avrc_ct_send_play_status(esp_a2d_conn_hdl_t conn_hdl, uint8_t tl)
{
    if (tl > ESP_AVRC_TRANS_LABEL_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_avrc_args_t arg;
    memset(&arg, 0, sizeof(arg));
    arg.get_play_status_cmd.conn_hdl = conn_hdl;
    arg.get_play_status_cmd.tl = tl;
    return avrc_ct_transfer(BTC_AVRC_STATUS_API_SND_GET_PLAY_STATUS_EVT, &arg);
}

esp_err_t esp_avrc_ct_send_get_play_status_cmd_by_handle(esp_a2d_conn_hdl_t conn_hdl, uint8_t tl)
{
    if (conn_hdl == ESP_A2D_INVALID_CONN_HDL || avrc_ct_check_user_tl(tl) != ESP_OK) {
        return ESP_ERR_INVALID_ARG;
    }
    return avrc_ct_send_play_status(conn_hdl, tl);
}

esp_err_t esp_avrc_ct_send_get_play_status_cmd(uint8_t tl)
{
    return avrc_ct_send_play_status(ESP_A2D_INVALID_CONN_HDL, tl);
}

#if BTC_AV_CA_INCLUDED

esp_err_t esp_avrc_ct_cover_art_connect(uint16_t mtu)
{
    if ((esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) ||
        (!btc_avrc_ct_connected_p())) {
        return ESP_ERR_INVALID_STATE;
    }

    if (!btc_avrc_ct_check_cover_art_support()) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    if (mtu > ESP_AVRC_CA_MTU_MAX || mtu < ESP_AVRC_CA_MTU_MIN) {
        mtu = ESP_AVRC_CA_MTU_MAX;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_AVRC_CT;
    msg.act = BTC_AVRC_CT_API_COVER_ART_CONNECT_EVT;

    btc_avrc_args_t arg;
    memset(&arg, 0, sizeof(btc_avrc_args_t));
    arg.ca_conn.mtu = mtu;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_avrc_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_avrc_ct_cover_art_disconnect(void)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (!btc_avrc_ct_check_cover_art_support()) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_AVRC_CT;
    msg.act = BTC_AVRC_CT_API_COVER_ART_DISCONNECT_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_avrc_ct_cover_art_get_image_properties(uint8_t *image_handle)
{
    if ((esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) ||
        (!btc_avrc_ct_connected_p())) {
        return ESP_ERR_INVALID_STATE;
    }

    if (!btc_avrc_ct_check_cover_art_support()) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    if (image_handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_AVRC_CT;
    msg.act = BTC_AVRC_CT_API_COVER_ART_GET_IMAGE_PROPERTIES_EVT;

    btc_avrc_args_t arg;
    memset(&arg, 0, sizeof(btc_avrc_args_t));
    memcpy(arg.ca_get_img_prop.image_handle, image_handle, ESP_AVRC_CA_IMAGE_HANDLE_LEN);

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_avrc_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_avrc_ct_cover_art_get_image(uint8_t *image_handle, uint8_t *image_descriptor, uint16_t image_descriptor_len)
{
    if ((esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) ||
        (!btc_avrc_ct_connected_p())) {
        return ESP_ERR_INVALID_STATE;
    }

    if (!btc_avrc_ct_check_cover_art_support()) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    if (image_handle == NULL || image_descriptor == NULL || image_descriptor_len == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_AVRC_CT;
    msg.act = BTC_AVRC_CT_API_COVER_ART_GET_IMAGE_EVT;

    btc_avrc_args_t arg;
    memset(&arg, 0, sizeof(btc_avrc_args_t));

    memcpy(arg.ca_get_img.image_handle, image_handle, ESP_AVRC_CA_IMAGE_HANDLE_LEN);
    arg.ca_get_img.image_descriptor_len = image_descriptor_len;
    arg.ca_get_img.image_descriptor = image_descriptor;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_avrc_args_t), btc_avrc_arg_deep_copy, btc_avrc_arg_deep_free);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_avrc_ct_cover_art_get_linked_thumbnail(uint8_t *image_handle)
{
    if ((esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) ||
        (!btc_avrc_ct_connected_p())) {
        return ESP_ERR_INVALID_STATE;
    }

    if (!btc_avrc_ct_check_cover_art_support()) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    if (image_handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_AVRC_CT;
    msg.act = BTC_AVRC_CT_API_COVER_ART_GET_LINKED_THUMBNAIL_EVT;

    btc_avrc_args_t arg;
    memset(&arg, 0, sizeof(btc_avrc_args_t));
    memcpy(arg.ca_get_lk_thn.image_handle, image_handle, ESP_AVRC_CA_IMAGE_HANDLE_LEN);

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_avrc_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

#endif /* #if BTC_AV_CA_INCLUDED */

/*********************************************************************************************/
/**                  following is the API of AVRCP target role                              **/
/*********************************************************************************************/

esp_err_t esp_avrc_tg_register_callback(esp_avrc_tg_cb_t callback)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (callback == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_profile_cb_set(BTC_PID_AVRC_TG, callback);
    return ESP_OK;
}

esp_err_t esp_avrc_tg_init(void)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_AVRC_TG;
    msg.act = BTC_AVRC_TG_API_INIT_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_avrc_tg_deinit(void)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_AVRC_TG;
    msg.act = BTC_AVRC_TG_API_DEINIT_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

bool esp_avrc_psth_bit_mask_operation(esp_avrc_bit_mask_op_t op, esp_avrc_psth_bit_mask_t *psth,
                                      esp_avrc_pt_cmd_t cmd)
{
    if (!psth ||
        cmd > ESP_AVRC_PT_CMD_VENDOR) {
        return false;
    }

    uint16_t *p = &psth->bits[(uint8_t)cmd >> 4];
    uint16_t mask = (uint16_t)1 << ((uint8_t)cmd & 0x0F);
    switch (op) {
    case ESP_AVRC_BIT_MASK_OP_SET:
        *p |= mask;
        break;
    case ESP_AVRC_BIT_MASK_OP_CLEAR:
        *p &= ~mask;
        break;
    case ESP_AVRC_BIT_MASK_OP_TEST:
        return (*p & mask);
    default:
        return false;
    }

    return true;
}

esp_err_t esp_avrc_tg_get_psth_cmd_filter(esp_avrc_psth_filter_t filter, esp_avrc_psth_bit_mask_t *cmd_set)
{
    if ((esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) ||
        (! btc_avrc_tg_init_p())) {
        return ESP_ERR_INVALID_STATE;
    }
    if (filter >= ESP_AVRC_PSTH_FILTER_SUPPORT_MAX ||
        cmd_set == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (filter == ESP_AVRC_PSTH_FILTER_ALLOWED_CMD) {
        const uint16_t *allowed_cmd_set = btc_avrc_tg_get_allowed_command();
        memcpy(cmd_set, allowed_cmd_set, sizeof(esp_avrc_psth_bit_mask_t));
    } else if (filter == ESP_AVRC_PSTH_FILTER_SUPPORTED_CMD) {
        const uint16_t *supported_cmd_set = btc_avrc_tg_get_supported_command();
        memcpy(cmd_set, supported_cmd_set, sizeof(esp_avrc_psth_bit_mask_t));
    } else {
    }

    return ESP_OK;
}

esp_err_t esp_avrc_tg_set_psth_cmd_filter(esp_avrc_psth_filter_t filter, const esp_avrc_psth_bit_mask_t *cmd_set)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    if (filter >= ESP_AVRC_PSTH_FILTER_SUPPORT_MAX ||
        cmd_set == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (filter == ESP_AVRC_PSTH_FILTER_ALLOWED_CMD) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    if (filter == ESP_AVRC_PSTH_FILTER_SUPPORTED_CMD) {
        bool allowed = btc_avrc_tg_check_supported_command(cmd_set->bits);
        if (!allowed) {
            return ESP_ERR_NOT_SUPPORTED;
        }
        btc_msg_t msg;
        msg.sig = BTC_SIG_API_CALL;
        msg.pid = BTC_PID_AVRC_TG;
        msg.act = BTC_AVRC_TG_API_SET_PSTH_SUPPORTED_CMD_EVT;

        btc_avrc_tg_args_t arg;
        memset(&arg, 0, sizeof(btc_avrc_tg_args_t));
        arg.set_psth_cmd = (uint16_t *)cmd_set->bits;

        /* Switch to BTC context */
        bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_avrc_tg_args_t),
                                                btc_avrc_tg_arg_deep_copy, btc_avrc_tg_arg_deep_free);
        return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
    } else {
        return ESP_FAIL;
    }
}

esp_err_t esp_avrc_tg_get_rn_evt_cap(esp_avrc_rn_evt_cap_t cap, esp_avrc_rn_evt_cap_mask_t *evt_set)
{
    if ((esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) ||
        (! btc_avrc_tg_init_p())) {
        return ESP_ERR_INVALID_STATE;
    }
    if (cap >= ESP_AVRC_RN_CAP_MAX ||
        evt_set == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (cap == ESP_AVRC_RN_CAP_ALLOWED_EVT) {
        evt_set->bits = btc_avrc_tg_get_rn_allowed_evt();
    } else if (cap == ESP_AVRC_RN_CAP_SUPPORTED_EVT) {
        evt_set->bits = btc_avrc_tg_get_rn_supported_evt();
    } else {
    }

    return ESP_OK;
}

esp_err_t esp_avrc_tg_set_rn_evt_cap(const esp_avrc_rn_evt_cap_mask_t *evt_set)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    if (evt_set == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    bool allowed = btc_avrc_tg_check_rn_supported_evt(evt_set->bits);
    if (!allowed) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_AVRC_TG;
    msg.act = BTC_AVRC_TG_API_SET_RN_SUPPORTED_EVT;

    btc_avrc_tg_args_t arg;
    memset(&arg, 0, sizeof(btc_avrc_tg_args_t));

    arg.set_rn_evt = evt_set->bits;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_avrc_tg_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

bool esp_avrc_rn_evt_bit_mask_operation(esp_avrc_bit_mask_op_t op, esp_avrc_rn_evt_cap_mask_t *events,
                                        esp_avrc_rn_event_ids_t event_id)
{
    if (!events ||
        event_id >= ESP_AVRC_RN_MAX_EVT) {
        return false;
    }

    uint16_t *p = &events->bits;
    uint16_t mask = (uint16_t)1 << ((uint8_t)event_id & 0x0F);
    switch (op) {
    case ESP_AVRC_BIT_MASK_OP_SET:
        *p |= mask;
        break;
    case ESP_AVRC_BIT_MASK_OP_CLEAR:
        *p &= ~mask;
        break;
    case ESP_AVRC_BIT_MASK_OP_TEST:
        return (*p & mask);
    default:
        return false;
    }

    return true;
}

static esp_err_t avrc_tg_send_rn(esp_a2d_conn_hdl_t conn_hdl, esp_avrc_rn_event_ids_t event_id,
                                  esp_avrc_rn_rsp_t rsp, esp_avrc_rn_param_t *param)
{
    if ((esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) ||
        (! btc_avrc_tg_connected_p())) {
        return ESP_ERR_INVALID_STATE;
    }

    if ( ! btc_avrc_tg_rn_evt_supported((uint8_t)event_id)) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    if (param == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_AVRC_TG;
    msg.act = BTC_AVRC_TG_API_SEND_RN_RSP_EVT;

    btc_avrc_tg_args_t arg;
    memset(&arg, 0, sizeof(btc_avrc_tg_args_t));
    arg.rn_rsp.conn_hdl = conn_hdl;
    arg.rn_rsp.event_id = event_id;
    arg.rn_rsp.rsp = rsp;
    memcpy(&arg.rn_rsp.param, param, sizeof(esp_avrc_rn_param_t));

    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_avrc_tg_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_avrc_tg_send_rn_rsp_by_handle(esp_a2d_conn_hdl_t conn_hdl, esp_avrc_rn_event_ids_t event_id,
                                            esp_avrc_rn_rsp_t rsp, esp_avrc_rn_param_t *param)
{
    if (conn_hdl == ESP_A2D_INVALID_CONN_HDL) {
        return ESP_ERR_INVALID_ARG;
    }
    return avrc_tg_send_rn(conn_hdl, event_id, rsp, param);
}

esp_err_t esp_avrc_tg_send_rn_rsp(esp_avrc_rn_event_ids_t event_id, esp_avrc_rn_rsp_t rsp,
                                  esp_avrc_rn_param_t *param)
{
    return avrc_tg_send_rn(ESP_A2D_INVALID_CONN_HDL, event_id, rsp, param);
}

esp_err_t esp_avrc_get_profile_status(esp_avrc_profile_status_t *profile_status)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    if (profile_status == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    memset(profile_status, 0, sizeof(esp_avrc_profile_status_t));
    btc_avrc_get_profile_status(profile_status);

    return ESP_OK;
}

#endif /* #if BTC_AV_INCLUDED */
