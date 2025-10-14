/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "ble_multiconn_cent_demo.h"
#define PEER_MANAGER_TAG "PEER_MANAGER"

static Peer remote_peer_lst[MAX_CONN_NUM];

void peer_manager_init(void)
{
    for (int i = 0; i < MAX_CONN_NUM; i++) {
        remote_peer_lst[i].conn_id = 0xFFFF;
        remote_peer_lst[i].conn_handle = 0xFFFF;
    }

    ESP_LOGI(PEER_MANAGER_TAG, "peer manager init complete");
}

esp_err_t peer_add(Peer *peer)
{
    for (int i = 0; i < MAX_CONN_NUM; i++) {
        if (remote_peer_lst[i].conn_id == 0xFFFF) {
            remote_peer_lst[i].char_handle = 0xFFFF;
            remote_peer_lst[i].conn_id = peer->conn_id;
            remote_peer_lst[i].conn_handle = peer->conn_handle;
            remote_peer_lst[i].gattc_if = peer->gattc_if;
            memcpy(&remote_peer_lst[i].peer_addr, &peer->peer_addr, sizeof(esp_bd_addr_t));
            ESP_LOGI(PEER_MANAGER_TAG, "peer added to list");
            return ESP_OK;
        }
    }

    ESP_LOGE(PEER_MANAGER_TAG, "peer list is full");
    return ESP_FAIL;
}

esp_err_t peer_remove(uint16_t conn_id)
{
    for (int i = 0; i < MAX_CONN_NUM; i++) {
        if (remote_peer_lst[i].conn_id == conn_id) {
            remote_peer_lst[i].conn_id = 0xFFFF;
            return ESP_OK;
        }
    }

    ESP_LOGE(PEER_MANAGER_TAG, "peer not found in list");
    return ESP_FAIL;
}

Peer *find_peer(uint16_t conn_id)
{
    for (int i = 0; i < MAX_CONN_NUM; i++) {
        if (remote_peer_lst[i].conn_id == conn_id) {
            return &remote_peer_lst[i];
        }
    }

    ESP_LOGE(PEER_MANAGER_TAG, "peer not found in list, conn_id %d", conn_id);
    return NULL;
}

void traverse_send_peer(uint16_t len, uint8_t *value)
{
    for (int i = 0; i < MAX_CONN_NUM; i++) {
        if (remote_peer_lst[i].conn_id != 0xFFFF && remote_peer_lst[i].char_handle != 0xFFFF) {
            esp_err_t ret = esp_ble_gattc_write_char(remote_peer_lst[i].gattc_if, remote_peer_lst[i].conn_id,
                remote_peer_lst[i].char_handle, len, value,ESP_GATT_WRITE_TYPE_RSP, ESP_GATT_AUTH_REQ_NONE);
            if (ret != ESP_OK) {
                ESP_LOGE(PEER_MANAGER_TAG, "traver peer write char failed!");
            }
        }
    }
}
