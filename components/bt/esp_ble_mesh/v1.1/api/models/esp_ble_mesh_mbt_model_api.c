/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>

#include "btc/btc_manage.h"

#include "btc_ble_mesh_mbt_model.h"
#include "esp_ble_mesh_mbt_model_api.h"

#if CONFIG_BLE_MESH_MBT_CLI

extern const void *bt_mesh_get_blob_receiver(void *model, uint16_t unicast_addr);
extern const void **bt_mesh_get_active_blob_receiver(void *model);
extern int bt_mesh_get_transfer_progress(void *model, uint16_t unicast_addr,
                                         uint8_t *block_percent, uint8_t *chunk_percent);
extern int bt_mesh_get_blob_reception_progress(void *model, uint8_t *reception_progress);

esp_err_t esp_ble_mesh_register_mbt_client_callback(esp_ble_mesh_mbt_client_cb_t callback)
{
    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    return (btc_profile_cb_set(BTC_PID_MBT_CLIENT, callback) == 0 ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_mbt_client_retrieve_capabilities(esp_ble_mesh_retrieve_capabilities_t *input)
{
    btc_ble_mesh_mbt_client_args_t arg = {0};
    btc_msg_t msg = {0};

    if (input == NULL || input->model == NULL ||
        (input->unicast_addr_count && input->unicast_addr == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_MBT_CLIENT;
    msg.act = BTC_BLE_MESH_ACT_MBT_CLIENT_RETRIEVE_CAPABILITIES;

    memcpy(&arg.retrieve_capabilities, input, sizeof(esp_ble_mesh_retrieve_capabilities_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_mbt_client_args_t),
                                 btc_ble_mesh_mbt_client_arg_deep_copy,
                                 btc_ble_mesh_mbt_client_arg_deep_free)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_mbt_client_transfer_blob(esp_ble_mesh_transfer_blob_t *input)
{
    btc_ble_mesh_mbt_client_args_t arg = {0};
    btc_msg_t msg = {0};

    if (input == NULL || input->model == NULL ||
        (input->unicast_addr_count && input->unicast_addr == NULL) ||
        input->blob_data == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_MBT_CLIENT;
    msg.act = BTC_BLE_MESH_ACT_MBT_CLIENT_TRANSFER_BLOB;

    memcpy(&arg.transfer_blob, input, sizeof(esp_ble_mesh_transfer_blob_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_mbt_client_args_t),
                                 btc_ble_mesh_mbt_client_arg_deep_copy,
                                 btc_ble_mesh_mbt_client_arg_deep_free)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_mbt_client_send_block(esp_ble_mesh_send_block_t *input)
{
    btc_ble_mesh_mbt_client_args_t arg = {0};
    btc_msg_t msg = {0};

    if (input == NULL || input->model == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_MBT_CLIENT;
    msg.act = BTC_BLE_MESH_ACT_MBT_CLIENT_SEND_BLOCK;

    memcpy(&arg.send_block, input, sizeof(esp_ble_mesh_send_block_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_mbt_client_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_mbt_client_send_data(esp_ble_mesh_send_data_t *input)
{
    btc_ble_mesh_mbt_client_args_t arg = {0};
    btc_msg_t msg = {0};

    if (input == NULL || input->model == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_MBT_CLIENT;
    msg.act = BTC_BLE_MESH_ACT_MBT_CLIENT_SEND_DATA;

    memcpy(&arg.send_data, input, sizeof(esp_ble_mesh_send_data_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_mbt_client_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_mbt_client_determine_block_status(esp_ble_mesh_determine_block_status_t *input)
{
    btc_ble_mesh_mbt_client_args_t arg = {0};
    btc_msg_t msg = {0};

    if (input == NULL || input->model == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_MBT_CLIENT;
    msg.act = BTC_BLE_MESH_ACT_MBT_CLIENT_DETERMINE_BLOCK_STATUS;

    memcpy(&arg.determine_block_status, input, sizeof(esp_ble_mesh_determine_block_status_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_mbt_client_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_mbt_client_determine_transfer_status(esp_ble_mesh_determine_transfer_status_t *input)
{
    btc_ble_mesh_mbt_client_args_t arg = {0};
    btc_msg_t msg = {0};

    if (input == NULL || input->model == NULL ||
        (input->unicast_addr_count && input->unicast_addr == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_MBT_CLIENT;
    msg.act = BTC_BLE_MESH_ACT_MBT_CLIENT_DETERMINE_TRANSFER_STATUS;

    memcpy(&arg.determine_transfer_status, input, sizeof(esp_ble_mesh_determine_transfer_status_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_mbt_client_args_t),
                                 btc_ble_mesh_mbt_client_arg_deep_copy,
                                 btc_ble_mesh_mbt_client_arg_deep_free)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_mbt_client_cancel_transfer(esp_ble_mesh_cancel_transfer_t *input)
{
    btc_ble_mesh_mbt_client_args_t arg = {0};
    btc_msg_t msg = {0};

    if (input == NULL || input->model == NULL ||
        (input->unicast_addr_count && input->unicast_addr == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_MBT_CLIENT;
    msg.act = BTC_BLE_MESH_ACT_MBT_CLIENT_CANCEL_TRANSFER;

    memcpy(&arg.cancel_transfer, input, sizeof(esp_ble_mesh_cancel_transfer_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_mbt_client_args_t),
                                 btc_ble_mesh_mbt_client_arg_deep_copy,
                                 btc_ble_mesh_mbt_client_arg_deep_free)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

const esp_ble_mesh_blob_receiver_t *esp_ble_mesh_mbt_client_get_blob_receiver(esp_ble_mesh_model_t *model,
                                                                                        uint16_t unicast_addr)
{
    return (const esp_ble_mesh_blob_receiver_t *)bt_mesh_get_blob_receiver((struct bt_mesh_model *)model,
                                                                           unicast_addr);
}

const esp_ble_mesh_blob_receiver_t **esp_ble_mesh_mbt_client_get_active_blob_receiver(esp_ble_mesh_model_t *model)
{
    return (const esp_ble_mesh_blob_receiver_t **)bt_mesh_get_active_blob_receiver((struct bt_mesh_model *)model);
}

esp_err_t esp_ble_mesh_mbt_client_get_transfer_progress(esp_ble_mesh_model_t *model,
                                                        uint16_t unicast_addr,
                                                        uint8_t *block_percent,
                                                        uint8_t *chunk_percent)
{
    return (bt_mesh_get_transfer_progress((struct bt_mesh_model *)model, unicast_addr,
                                          block_percent, chunk_percent) == 0 ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_mbt_client_set_transfer_ttl(esp_ble_mesh_model_t *model,
                                                   uint8_t transfer_ttl)
{
    btc_ble_mesh_mbt_client_args_t arg = {0};
    btc_msg_t msg = {0};

    if (model == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_MBT_CLIENT;
    msg.act = BTC_BLE_MESH_ACT_MBT_CLIENT_SET_TRANSFER_TTL;

    arg.set_transfer_ttl.model = model;
    arg.set_transfer_ttl.transfer_ttl = transfer_ttl;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_mbt_client_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_mbt_client_clear_transfer_ttl(esp_ble_mesh_model_t *model)
{
    btc_ble_mesh_mbt_client_args_t arg = {0};
    btc_msg_t msg = {0};

    if (model == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_MBT_CLIENT;
    msg.act = BTC_BLE_MESH_ACT_MBT_CLIENT_CLEAR_TRANSFER_TTL;

    arg.clear_transfer_ttl.model = model;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_mbt_client_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_mbt_client_set_app_idx(esp_ble_mesh_model_t *model,
                                              uint16_t app_idx)
{
    btc_ble_mesh_mbt_client_args_t arg = {0};
    btc_msg_t msg = {0};

    if (model == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_MBT_CLIENT;
    msg.act = BTC_BLE_MESH_ACT_MBT_CLIENT_SET_APP_IDX;

    arg.set_app_idx.model = model;
    arg.set_app_idx.app_idx = app_idx;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_mbt_client_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_mbt_client_clear_app_idx(esp_ble_mesh_model_t *model)
{
    btc_ble_mesh_mbt_client_args_t arg = {0};
    btc_msg_t msg = {0};

    if (model == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_MBT_CLIENT;
    msg.act = BTC_BLE_MESH_ACT_MBT_CLIENT_CLEAR_APP_IDX;

    arg.clear_app_idx.model = model;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_mbt_client_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_mbt_client_set_multicast_addr(esp_ble_mesh_model_t *model,
                                                     uint16_t multicast_addr)
{
    btc_ble_mesh_mbt_client_args_t arg = {0};
    btc_msg_t msg = {0};

    if (model == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_MBT_CLIENT;
    msg.act = BTC_BLE_MESH_ACT_MBT_CLIENT_SET_MULTICAST_ADDR;

    arg.set_multicast_addr.model = model;
    arg.set_multicast_addr.multicast_addr = multicast_addr;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_mbt_client_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_mbt_client_clear_multicast_addr(esp_ble_mesh_model_t *model)
{
    btc_ble_mesh_mbt_client_args_t arg = {0};
    btc_msg_t msg = {0};

    if (model == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_MBT_CLIENT;
    msg.act = BTC_BLE_MESH_ACT_MBT_CLIENT_CLEAR_MULTICAST_ADDR;

    arg.clear_multicast_addr.model = model;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_mbt_client_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}
#endif /* CONFIG_BLE_MESH_MBT_CLI */

#if CONFIG_BLE_MESH_MBT_SRV
esp_err_t esp_ble_mesh_register_mbt_server_callback(esp_ble_mesh_mbt_server_cb_t callback)
{
    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    return (btc_profile_cb_set(BTC_PID_MBT_SERVER, callback) == 0 ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_mbt_server_initialize_blob_receive(esp_ble_mesh_initialize_blob_receive_t *input)
{
    btc_ble_mesh_mbt_server_args_t arg = {0};
    btc_msg_t msg = {0};

    if (input == NULL || input->model == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_MBT_SERVER;
    msg.act = BTC_BLE_MESH_ACT_MBT_SERVER_INITIALIZE_BLOB_RECEIVE;

    memcpy(&arg.initialize_blob_receive, input, sizeof(esp_ble_mesh_initialize_blob_receive_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_mbt_server_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_mbt_server_cancel_blob_receive(esp_ble_mesh_cancel_blob_receive_t *input)
{
    btc_ble_mesh_mbt_server_args_t arg = {0};
    btc_msg_t msg = {0};

    if (input == NULL || input->model == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_MBT_SERVER;
    msg.act = BTC_BLE_MESH_ACT_MBT_SERVER_CANCEL_BLOB_RECEIVE;

    memcpy(&arg.cancel_blob_receive, input, sizeof(esp_ble_mesh_cancel_blob_receive_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_mbt_server_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_mbt_server_set_blob_capabilities(esp_ble_mesh_set_blob_capabilities_t *input)
{
    btc_ble_mesh_mbt_server_args_t arg = {0};
    btc_msg_t msg = {0};

    if (input == NULL || input->model == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_MBT_SERVER;
    msg.act = BTC_BLE_MESH_ACT_MBT_SERVER_SET_BLOB_CAPABILITIES;

    memcpy(&arg.set_blob_capabilities, input, sizeof(esp_ble_mesh_set_blob_capabilities_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_mbt_server_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_mbt_server_get_blob_reception_progress(esp_ble_mesh_model_t *model,
                                                              uint8_t *reception_progress)
{
    return (bt_mesh_get_blob_reception_progress((struct bt_mesh_model *)model,
                                                reception_progress) == 0 ? ESP_OK : ESP_FAIL);
}
#endif /* CONFIG_BLE_MESH_MBT_SRV */
