/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "esp_bt_main.h"
#include "btc/btc_manage.h"

#include "btc_spp.h"
#include "esp_spp_api.h"
#include "common/bt_target.h"

#if (defined BTC_SPP_INCLUDED && BTC_SPP_INCLUDED == TRUE)

static const uint8_t UUID_SPP[16] = {0x00, 0x00, 0x11, 0x01, 0x00, 0x00, 0x10, 0x00,
                                    0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB
                                    };
static tSDP_UUID sdp_uuid;

esp_err_t esp_spp_register_callback(esp_spp_cb_t callback)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (callback == NULL) {
        return ESP_FAIL;
    }

    btc_profile_cb_set(BTC_PID_SPP, callback);
    return ESP_OK;
}


esp_err_t esp_spp_init(esp_spp_mode_t mode)
{
    esp_spp_cfg_t bt_spp_cfg = {
        .mode = mode,
        .enable_l2cap_ertm = true,
        .tx_buffer_size = ESP_SPP_MAX_TX_BUFFER_SIZE,
    };

    return esp_spp_enhanced_init(&bt_spp_cfg);
}

esp_err_t esp_spp_enhanced_init(const esp_spp_cfg_t *cfg)
{
    btc_msg_t msg;
    btc_spp_args_t arg;
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (cfg->mode == ESP_SPP_MODE_VFS && (cfg->tx_buffer_size < ESP_SPP_MIN_TX_BUFFER_SIZE ||
            cfg->tx_buffer_size > ESP_SPP_MAX_TX_BUFFER_SIZE)) {
        LOG_WARN("Invalid tx buffer size");
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_SPP;
    msg.act = BTC_SPP_ACT_INIT;

    arg.init.mode = cfg->mode;
    arg.init.enable_l2cap_ertm = cfg->enable_l2cap_ertm;
    arg.init.tx_buffer_size = cfg->tx_buffer_size;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_spp_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_spp_deinit(void)
{
    btc_msg_t msg;
    btc_spp_args_t arg;
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_SPP;
    msg.act = BTC_SPP_ACT_UNINIT;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_spp_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}


esp_err_t esp_spp_start_discovery(esp_bd_addr_t bd_addr)
{
    sdp_uuid.len = 16;
    memcpy(sdp_uuid.uu.uuid128, UUID_SPP, sizeof(sdp_uuid.uu.uuid128));

    btc_msg_t msg;
    btc_spp_args_t arg;
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_SPP;
    msg.act = BTC_SPP_ACT_START_DISCOVERY;

    memcpy(arg.start_discovery.bd_addr, bd_addr, ESP_BD_ADDR_LEN);
    arg.start_discovery.num_uuid = 1;
    arg.start_discovery.p_uuid_list = &sdp_uuid;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_spp_args_t),
                btc_spp_arg_deep_copy, btc_spp_arg_deep_free) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_spp_connect(esp_spp_sec_t sec_mask,
                          esp_spp_role_t role, uint8_t remote_scn, esp_bd_addr_t peer_bd_addr)
{
    btc_msg_t msg;
    btc_spp_args_t arg;
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (sec_mask != ESP_SPP_SEC_NONE &&
        sec_mask != ESP_SPP_SEC_AUTHENTICATE &&
        sec_mask != (ESP_SPP_SEC_AUTHENTICATE | ESP_SPP_SEC_ENCRYPT)) {
        LOG_WARN("Suggest to use ESP_SPP_SEC_NONE, ESP_SPP_SEC_AUTHENTICATE"
                 "or (ESP_SPP_SEC_AUTHENTICATE | ESP_SPP_SEC_ENCRYPT) only\n");
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_SPP;
    msg.act = BTC_SPP_ACT_CONNECT;

    arg.connect.sec_mask = sec_mask;
    arg.connect.role = role;
    arg.connect.remote_scn = remote_scn;
    memcpy(arg.connect.peer_bd_addr, peer_bd_addr, ESP_BD_ADDR_LEN);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_spp_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_spp_disconnect(uint32_t handle)
{
    btc_msg_t msg;
    btc_spp_args_t arg;
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_SPP;
    msg.act = BTC_SPP_ACT_DISCONNECT;

    arg.disconnect.handle = handle;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_spp_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_spp_start_srv(esp_spp_sec_t sec_mask,
                            esp_spp_role_t role, uint8_t local_scn, const char *name)
{
    btc_msg_t msg;
    btc_spp_args_t arg;
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (name == NULL || strlen(name) > ESP_SPP_SERVER_NAME_MAX) {
        LOG_ERROR("Invalid server name!\n");
        return ESP_ERR_INVALID_ARG;
    }

    if (sec_mask != ESP_SPP_SEC_NONE &&
        sec_mask != ESP_SPP_SEC_AUTHENTICATE &&
        sec_mask != (ESP_SPP_SEC_AUTHENTICATE | ESP_SPP_SEC_ENCRYPT) &&
        sec_mask != ESP_SPP_SEC_IN_16_DIGITS &&
        sec_mask != (ESP_SPP_SEC_IN_16_DIGITS | ESP_SPP_SEC_AUTHENTICATE) &&
        sec_mask != (ESP_SPP_SEC_IN_16_DIGITS | ESP_SPP_SEC_AUTHENTICATE | ESP_SPP_SEC_ENCRYPT)) {
        LOG_WARN("Suggest to use ESP_SPP_SEC_NONE, ESP_SPP_SEC_AUTHENTICATE,"
                 "(ESP_SPP_SEC_AUTHENTICATE | ESP_SPP_SEC_ENCRYPT),"
                 "ESP_SPP_SEC_IN_16_DIGITS, (ESP_SPP_SEC_IN_16_DIGITS | ESP_SPP_SEC_AUTHENTICATE), or"
                 "(ESP_SPP_SEC_IN_16_DIGITS | ESP_SPP_SEC_AUTHENTICATE | ESP_SPP_SEC_ENCRYPT) only\n");
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_SPP;
    msg.act = BTC_SPP_ACT_START_SRV;

    arg.start_srv.sec_mask = sec_mask;
    arg.start_srv.role = role;
    arg.start_srv.local_scn = local_scn;
    arg.start_srv.max_session = ESP_SPP_MAX_SESSION;
    strcpy(arg.start_srv.name, name);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_spp_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_spp_stop_srv(void)
{
    btc_msg_t msg;
    btc_spp_args_t arg;
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_SPP;
    msg.act = BTC_SPP_ACT_STOP_SRV;
    arg.stop_srv.scn = BTC_SPP_INVALID_SCN;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_spp_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_spp_stop_srv_scn(uint8_t scn)
{
    btc_msg_t msg;
    btc_spp_args_t arg;
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if ((scn == 0) || (scn >= PORT_MAX_RFC_PORTS)) {
        LOG_ERROR("Invalid SCN!\n");
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_SPP;
    msg.act = BTC_SPP_ACT_STOP_SRV;
    arg.stop_srv.scn = scn;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_spp_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}


esp_err_t esp_spp_write(uint32_t handle, int len, uint8_t *p_data)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (len <= 0 || p_data == NULL) {
        LOG_ERROR("Invalid data or len!\n");
        return ESP_ERR_INVALID_ARG;
    }

    return spp_send_data_to_btc(handle, len, p_data, ESP_SPP_MODE_CB);
}

esp_err_t esp_spp_vfs_register(void)
{
    btc_msg_t msg;
    btc_spp_args_t arg;
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_SPP;
    msg.act = BTC_SPP_ACT_VFS_REGISTER;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_spp_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_spp_vfs_unregister(void)
{
    btc_msg_t msg;
    btc_spp_args_t arg;
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_SPP;
    msg.act = BTC_SPP_ACT_VFS_UNREGISTER;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_spp_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

#endif ///defined BTC_SPP_INCLUDED && BTC_SPP_INCLUDED == TRUE
