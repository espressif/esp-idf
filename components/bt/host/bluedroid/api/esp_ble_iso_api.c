/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_bt_device.h"
#include "esp_bt_main.h"
#include "esp_ble_iso_api.h"
#include "btc_iso_ble.h"
#include "btc/btc_manage.h"


esp_err_t esp_ble_iso_register_callback(esp_ble_iso_cb_t callback)
{
    if (callback == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    return (btc_profile_cb_set(BTC_PID_ISO_BLE, callback) == 0 ? ESP_OK : ESP_FAIL);
}

esp_ble_iso_cb_t esp_ble_iso_get_callback(void)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return NULL;
    }

    return (esp_ble_iso_cb_t) btc_profile_cb_get(BTC_PID_ISO_BLE);
}

#if (BLE_FEAT_ISO_BIG_BROADCASTER_EN == TRUE)
esp_err_t esp_ble_iso_create_big(esp_ble_iso_big_creat_params_t *big_creat_param)
{

    btc_msg_t msg = {0};
    btc_ble_iso_args_t arg;
    memset(&arg, 0, sizeof(arg));

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (big_creat_param == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (big_creat_param->num_bis == 0 || big_creat_param->num_bis > BLE_ISO_BIS_MAX_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }
    if (big_creat_param->num_bis > ESP_BLE_MAX_BIS_NUM) {
        return ESP_ERR_INVALID_ARG;
    }
    if (big_creat_param->sdu_interval < BLE_ISO_SDU_INT_MIN || big_creat_param->sdu_interval > BLE_ISO_SDU_INT_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    if (big_creat_param->max_sdu < 0x0001 || big_creat_param->max_sdu > 0x0FFF) {
        return ESP_ERR_INVALID_ARG;
    }
    if (big_creat_param->max_transport_latency < BLE_ISO_MAX_TRANSPORT_LATENCY_MIN ||
            big_creat_param->max_transport_latency > BLE_ISO_MAX_TRANSPORT_LATENCY_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    if (big_creat_param->rtn > 0x1E) {
        return ESP_ERR_INVALID_ARG;
    }
    if ((big_creat_param->phy != 0x01) && (big_creat_param->phy != 0x02) && (big_creat_param->phy != 0x04)) {
        return ESP_ERR_INVALID_ARG;
    }
    if (big_creat_param->packing > 0x01) {
        return ESP_ERR_INVALID_ARG;
    }
    if (big_creat_param->framing > BLE_ISO_FRAMING_FRAMED_PDU_UNSEGMENTABLE_MODE) {
        return ESP_ERR_INVALID_ARG;
    }
    if (big_creat_param->encryption > 0x01) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_ISO_BLE;
    msg.act = BTC_ISO_ACT_BIG_CREATE;

    memcpy(&arg.iso_big_creat_params, big_creat_param, sizeof(esp_ble_iso_big_creat_params_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_iso_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_iso_create_big_test(esp_ble_iso_big_creat_test_params_t *big_creat_test_param)
{
    btc_msg_t msg = {0};
    btc_ble_iso_args_t arg;
    memset(&arg, 0, sizeof(arg));

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (big_creat_test_param == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (big_creat_test_param->num_bis == 0 || big_creat_test_param->num_bis > BLE_ISO_BIS_MAX_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }
    if (big_creat_test_param->num_bis > ESP_BLE_MAX_BIS_NUM) {
        return ESP_ERR_INVALID_ARG;
    }
    if (big_creat_test_param->sdu_interval < BLE_ISO_SDU_INT_MIN || big_creat_test_param->sdu_interval > BLE_ISO_SDU_INT_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    if (big_creat_test_param->iso_interval < 0x0004 || big_creat_test_param->iso_interval > 0x0C80) {
        return ESP_ERR_INVALID_ARG;
    }
    if (big_creat_test_param->nse == 0 || big_creat_test_param->nse > 0x1F) {
        return ESP_ERR_INVALID_ARG;
    }
    if (big_creat_test_param->max_sdu < 0x0001 || big_creat_test_param->max_sdu > 0x0FFF) {
        return ESP_ERR_INVALID_ARG;
    }
    if (big_creat_test_param->max_pdu < 0x0001 || big_creat_test_param->max_pdu > 0x00FB) {
        return ESP_ERR_INVALID_ARG;
    }
    if ((big_creat_test_param->phy != 0x01) && (big_creat_test_param->phy != 0x02) && (big_creat_test_param->phy != 0x04)) {
        return ESP_ERR_INVALID_ARG;
    }
    if (big_creat_test_param->framing > BLE_ISO_FRAMING_FRAMED_PDU_UNSEGMENTABLE_MODE) {
        return ESP_ERR_INVALID_ARG;
    }
    if (big_creat_test_param->bn == 0 || big_creat_test_param->bn > 0x07) {
        return ESP_ERR_INVALID_ARG;
    }
    if (big_creat_test_param->irc == 0 || big_creat_test_param->irc > 0x0F) {
        return ESP_ERR_INVALID_ARG;
    }
    if (big_creat_test_param->encryption > 0x01) {
        return ESP_ERR_INVALID_ARG;
    }
    if (big_creat_test_param->packing > 0x01) {
        return ESP_ERR_INVALID_ARG;
    }
    if (big_creat_test_param->pto > 0x0F) {
        return ESP_ERR_INVALID_ARG;
    }


    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_ISO_BLE;
    msg.act = BTC_ISO_ACT_BIG_CREATE_TEST;

    memcpy(&arg.iso_big_creat_test_params, big_creat_test_param, sizeof(esp_ble_iso_big_creat_test_params_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_iso_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_iso_terminate_big(uint8_t big_handle, uint8_t reason)
{
    btc_msg_t msg = {0};
    btc_ble_iso_args_t arg;
    memset(&arg, 0, sizeof(arg));

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    /* big_handle: 0x00 to 0xEF */
    if (big_handle > 0xEF) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_ISO_BLE;
    msg.act = BTC_ISO_ACT_BIG_TERMINATE;

    arg.iso_big_terminate_params.big_handle = big_handle;
    arg.iso_big_terminate_params.reason = reason;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_iso_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}
#endif // #if (BLE_FEAT_ISO_BIG_BROADCASTER_EN == TRUE)

#if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
esp_err_t esp_ble_iso_big_create_sync(esp_ble_iso_big_sync_creat_params_t *big_sync_create_param)
{
    btc_msg_t msg = {0};
    btc_ble_iso_args_t arg;
    memset(&arg, 0, sizeof(arg));

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (big_sync_create_param == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    /* num_bis: 0x01 to 0x1F, and not more than BLE_ISO_BIS_MAX_COUNT */
    if (big_sync_create_param->num_bis == 0 || big_sync_create_param->num_bis > BLE_ISO_BIS_MAX_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }
    if (big_sync_create_param->num_bis > ESP_BLE_MAX_BIS_NUM) {
        return ESP_ERR_INVALID_ARG;
    }
    if (big_sync_create_param->big_handle > 0xEF) {
        return ESP_ERR_INVALID_ARG;
    }
    /* sync_handle: 0x0000 to 0x0EFF */
    if (big_sync_create_param->sync_handle > 0x0EFF) {
        return ESP_ERR_INVALID_ARG;
    }
    if (big_sync_create_param->encryption > 0x01) {
        return ESP_ERR_INVALID_ARG;
    }
    /* mse: 0x00 to 0x1F */
    if (big_sync_create_param->mse > 0x1F) {
        return ESP_ERR_INVALID_ARG;
    }
    /* big_sync_timeout: 0x000A to 0x4000 */
    if (big_sync_create_param->big_sync_timeout < 0x000A || big_sync_create_param->big_sync_timeout > 0x4000) {
        return ESP_ERR_INVALID_ARG;
    }
    for (uint8_t k = 0; k < big_sync_create_param->num_bis; k++) {
        /* BIS index: 0x01 to 0x1F */
        if (big_sync_create_param->bis[k] == 0 || big_sync_create_param->bis[k] > 0x1F) {
            return ESP_ERR_INVALID_ARG;
        }
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_ISO_BLE;
    msg.act = BTC_ISO_ACT_BIG_SYNC_CREATE;

    arg.iso_big_sync_creat_params.big_handle = big_sync_create_param->big_handle;
    arg.iso_big_sync_creat_params.sync_handle = big_sync_create_param->sync_handle;
    arg.iso_big_sync_creat_params.encryption = big_sync_create_param->encryption;
    memcpy(&arg.iso_big_sync_creat_params.bc_code[0], &big_sync_create_param->bc_code[0], 16);
    arg.iso_big_sync_creat_params.mse = big_sync_create_param->mse;
    arg.iso_big_sync_creat_params.big_sync_timeout = big_sync_create_param->big_sync_timeout;
    arg.iso_big_sync_creat_params.num_bis = big_sync_create_param->num_bis;
    for (uint8_t i = 0; i < big_sync_create_param->num_bis; i++)
    {
        arg.iso_big_sync_creat_params.bis[i] = big_sync_create_param->bis[i];
    }

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_iso_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_iso_big_terminate_sync(uint8_t big_handle)
{
    btc_msg_t msg = {0};
    btc_ble_iso_args_t arg;
    memset(&arg, 0, sizeof(arg));

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    /* big_handle: 0x00 to 0xEF */
    if (big_handle > 0xEF) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_ISO_BLE;
    msg.act = BTC_ISO_ACT_BIG_SYNC_TERMINATE;

    arg.iso_big_sync_terminate_params.big_handle = big_handle;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_iso_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}
#endif // #if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)

esp_err_t esp_ble_iso_set_iso_data_path(esp_ble_iso_set_data_path_params_t *data_path_params)
{
    btc_msg_t msg = {0};
    btc_ble_iso_args_t arg;
    memset(&arg, 0, sizeof(arg));

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (data_path_params == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (data_path_params->codec_cfg_len && (!data_path_params->codec_cfg)) {
        return ESP_ERR_INVALID_ARG;
    }

    if (data_path_params->codec_cfg && (!data_path_params->codec_cfg_len)) {
        return ESP_ERR_INVALID_ARG;
    }

    if (data_path_params->data_path_dir > ESP_BLE_ISO_DATA_PATH_DIR_OUTPUT) {
        return ESP_ERR_INVALID_ARG;
    }
    /* controller_delay: 0x000000 to 0x3D0900 (0 to 4 s in microseconds) */
    if (data_path_params->controller_delay > 0x3D0900) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_ISO_BLE;
    msg.act = BTC_ISO_ACT_SET_DATA_PATH;

    arg.iso_set_data_path_params.conn_handle = data_path_params->iso_handle;
    arg.iso_set_data_path_params.data_path_dir = data_path_params->data_path_dir;
    arg.iso_set_data_path_params.data_path_id = data_path_params->data_path_id;
    arg.iso_set_data_path_params.coding_fmt = data_path_params->coding_fmt;
    arg.iso_set_data_path_params.company_id = data_path_params->company_id;
    arg.iso_set_data_path_params.vs_codec_id = data_path_params->vs_codec_id;
    arg.iso_set_data_path_params.controller_delay = data_path_params->controller_delay;
    arg.iso_set_data_path_params.codec_cfg_len = data_path_params->codec_cfg_len;
    arg.iso_set_data_path_params.codec_cfg = data_path_params->codec_cfg;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_iso_args_t), btc_iso_ble_arg_deep_copy, btc_iso_ble_arg_deep_free)
                == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_iso_remove_iso_data_path(esp_ble_iso_remove_data_path_params_t *data_path_params)
{
    btc_msg_t msg = {0};
    btc_ble_iso_args_t arg;
    memset(&arg, 0, sizeof(arg));

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (data_path_params == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (data_path_params->data_path_dir == 0 || data_path_params->data_path_dir > ESP_BLE_ISO_DATA_PATH_DIR_ALL_BIT_MASK) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_ISO_BLE;
    msg.act = BTC_ISO_ACT_REMOVE_DATA_PATH;

    arg.iso_remove_data_path_params.conn_handle = data_path_params->iso_handle;
    arg.iso_remove_data_path_params.data_path_dir = data_path_params->data_path_dir;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_iso_args_t), NULL, NULL)
                == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_iso_read_iso_tx_sync(uint16_t iso_handle)
{
    btc_msg_t msg = {0};
    btc_ble_iso_args_t arg;
    memset(&arg, 0, sizeof(arg));

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_ISO_BLE;
    msg.act = BTC_ISO_ACT_READ_ISO_TX_SYNC;

    arg.iso_read_tx_sync_params.iso_handle = iso_handle;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_iso_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_iso_read_link_quality(uint16_t iso_handle)
{
    btc_msg_t msg = {0};
    btc_ble_iso_args_t arg;
    memset(&arg, 0, sizeof(arg));

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_ISO_BLE;
    msg.act = BTC_ISO_ACT_READ_ISO_LINK_QUALITY;

    arg.iso_read_link_quality_params.iso_handle = iso_handle;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_iso_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

#if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)
esp_err_t esp_ble_iso_set_cig_parameters(struct esp_ble_iso_set_cig_params *cig_params)
{
    btc_msg_t msg = {0};
    btc_ble_iso_args_t arg;
    memset(&arg, 0, sizeof(arg));

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (cig_params == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (cig_params->cis_cnt == 0 || cig_params->cis_cnt > BLE_ISO_CIS_MAX_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }
    if (cig_params->sdu_int_c_to_p < BLE_ISO_SDU_INT_MIN || cig_params->sdu_int_c_to_p > BLE_ISO_SDU_INT_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    if (cig_params->sdu_int_p_to_c < BLE_ISO_SDU_INT_MIN || cig_params->sdu_int_p_to_c > BLE_ISO_SDU_INT_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    if (cig_params->worse_case_SCA > BLE_ISO_WORST_CASE_SCA_LEVEL_20_PPM) {
        return ESP_ERR_INVALID_ARG;
    }
    if (cig_params->framing > BLE_ISO_FRAMING_FRAMED_PDU_UNSEGMENTABLE_MODE) {
        return ESP_ERR_INVALID_ARG;
    }
    if (cig_params->mtl_c_to_p < BLE_ISO_MAX_TRANSPORT_LATENCY_MIN || cig_params->mtl_c_to_p > BLE_ISO_MAX_TRANSPORT_LATENCY_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    if (cig_params->mtl_p_to_c < BLE_ISO_MAX_TRANSPORT_LATENCY_MIN || cig_params->mtl_p_to_c > BLE_ISO_MAX_TRANSPORT_LATENCY_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_ISO_BLE;
    msg.act = BTC_ISO_ACT_SET_CIG_PARAMS;

    arg.set_cig_params.cig_id = cig_params->cig_id;
    arg.set_cig_params.sdu_int_c_to_p = cig_params->sdu_int_c_to_p;
    arg.set_cig_params.sdu_int_p_to_c = cig_params->sdu_int_p_to_c;
    arg.set_cig_params.worse_case_SCA = cig_params->worse_case_SCA;
    arg.set_cig_params.packing = cig_params->packing;
    arg.set_cig_params.framing = cig_params->framing;
    arg.set_cig_params.mtl_c_to_p = cig_params->mtl_c_to_p;
    arg.set_cig_params.mtl_p_to_c = cig_params->mtl_p_to_c;
    arg.set_cig_params.cis_cnt = cig_params->cis_cnt;
    memcpy(&arg.set_cig_params.cis_params[0], &cig_params->cis_params[0], sizeof(struct ble_iso_cis_params) * cig_params->cis_cnt);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_iso_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);

}

esp_err_t esp_ble_iso_set_cig_parameters_test(struct esp_ble_iso_set_cig_params_test *cig_params_test)
{
    btc_msg_t msg = {0};
    btc_ble_iso_args_t arg;
    memset(&arg, 0, sizeof(arg));

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (cig_params_test == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (cig_params_test->cis_cnt == 0 || cig_params_test->cis_cnt > BLE_ISO_CIS_MAX_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }
    if (cig_params_test->sdu_int_c_to_p < BLE_ISO_SDU_INT_MIN || cig_params_test->sdu_int_c_to_p > BLE_ISO_SDU_INT_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    if (cig_params_test->sdu_int_p_to_c < BLE_ISO_SDU_INT_MIN || cig_params_test->sdu_int_p_to_c > BLE_ISO_SDU_INT_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    if (cig_params_test->iso_interval < 0x0004 || cig_params_test->iso_interval > 0x0C80) {
        return ESP_ERR_INVALID_ARG;
    }
    if (cig_params_test->worse_case_SCA > BLE_ISO_WORST_CASE_SCA_LEVEL_20_PPM) {
        return ESP_ERR_INVALID_ARG;
    }
    if (cig_params_test->framing > BLE_ISO_FRAMING_FRAMED_PDU_UNSEGMENTABLE_MODE) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_ISO_BLE;
    msg.act = BTC_ISO_ACT_SET_CIG_PARAMS_TEST;

    arg.set_cig_params_test.cig_id = cig_params_test->cig_id;
    arg.set_cig_params_test.sdu_int_c_to_p = cig_params_test->sdu_int_c_to_p;
    arg.set_cig_params_test.sdu_int_p_to_c = cig_params_test->sdu_int_p_to_c;
    arg.set_cig_params_test.ft_c_to_p = cig_params_test->ft_c_to_p;
    arg.set_cig_params_test.ft_p_to_c = cig_params_test->ft_p_to_c;
    arg.set_cig_params_test.iso_interval = cig_params_test->iso_interval;
    arg.set_cig_params_test.worse_case_SCA = cig_params_test->worse_case_SCA;
    arg.set_cig_params_test.packing = cig_params_test->packing;
    arg.set_cig_params_test.framing = cig_params_test->framing;
    arg.set_cig_params_test.cis_cnt = cig_params_test->cis_cnt;
    memcpy(&arg.set_cig_params_test.cis_params_test[0], &cig_params_test->cis_params_test[0], sizeof(struct ble_iso_cis_params_test) * cig_params_test->cis_cnt);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_iso_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);

}

esp_err_t esp_ble_iso_create_cis(struct esp_ble_iso_create_cis_params *creat_cis_params)
{
    btc_msg_t msg = {0};
    btc_ble_iso_args_t arg;
    memset(&arg, 0, sizeof(arg));

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (creat_cis_params == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (creat_cis_params->cis_count == 0 || creat_cis_params->cis_count > BLE_ISO_CIS_MAX_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_ISO_BLE;
    msg.act = BTC_ISO_ACT_CREATE_CIS;

    arg.creat_cis_params.cis_count = creat_cis_params->cis_count;
    for (uint8_t i = 0; i < creat_cis_params->cis_count; i++)
    {
        arg.creat_cis_params.cis_hdls[i].cis_hdl = creat_cis_params->cis_hdl_info[i].cis_hdl;
        arg.creat_cis_params.cis_hdls[i].acl_hdl = creat_cis_params->cis_hdl_info[i].acl_hdl;
    }

    //memcpy(&arg.creat_cis_params.cis_hdl_info[0], &creat_cis_params->cis_hdl_info[0], creat_cis_params->cis_count * 4);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_iso_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_iso_remove_cig(uint8_t cig_id)
{
    btc_msg_t msg = {0};
    btc_ble_iso_args_t arg;
    memset(&arg, 0, sizeof(arg));

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    /* cig_id: 0x00 to 0xEF */
    if (cig_id > 0xEF) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_ISO_BLE;
    msg.act = BTC_ISO_ACT_REMOVE_CIG;

    arg.remove_cig_params.cig_id = cig_id;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_iso_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}
#endif // #if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)

#if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)
esp_err_t esp_ble_iso_accept_cis_request(uint16_t cis_handle)
{
    btc_msg_t msg = {0};
    btc_ble_iso_args_t arg;
    memset(&arg, 0, sizeof(arg));

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    /* Connection handle range: 0x0000 to 0x0EFF */
    if (cis_handle > 0x0EFF) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_ISO_BLE;
    msg.act = BTC_ISO_ACT_ACCEPT_CIS_REQ;

    arg.accept_cis_req_params.cis_handle = cis_handle;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_iso_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_iso_reject_cis_request(uint16_t cis_handle, uint8_t reason)
{
    btc_msg_t msg = {0};
    btc_ble_iso_args_t arg;
    memset(&arg, 0, sizeof(arg));

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    if (cis_handle > 0x0EFF) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_ISO_BLE;
    msg.act = BTC_ISO_ACT_REJECT_CIS_REQ;

    arg.reject_cis_req_params.cis_handle = cis_handle;
    arg.reject_cis_req_params.reason     = reason;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_iso_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}
#endif // #if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)

#if (BLE_FEAT_ISO_CIG_EN == TRUE)
esp_err_t esp_ble_iso_disconnect_cis(uint16_t cis_handle, uint8_t reason)
{
    btc_msg_t msg = {0};
    btc_ble_iso_args_t arg;
    memset(&arg, 0, sizeof(arg));

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    if (cis_handle > 0x0EFF) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_ISO_BLE;
    msg.act = BTC_ISO_ACT_DISCON_CIS;

    arg.discon_cis_params.cis_handle = cis_handle;
    arg.discon_cis_params.reason     = reason;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_iso_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}
#endif // #if (BLE_FEAT_ISO_CIG_EN == TRUE)
