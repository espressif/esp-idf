/*
 * SPDX-FileCopyrightText: 2023 NXP
 * SPDX-FileCopyrightText: 2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "tmap_peripheral.h"

void initiate_call(void)
{
    if (tmap_peer_is_cg() == false) {
        ESP_LOGE(TAG, "Peer not support Call Gateway");
        return;
    }

    ccp_originate_call();
}

void terminate_call(void)
{
    if (tmap_peer_is_cg() == false) {
        ESP_LOGE(TAG, "Peer not support Call Gateway");
        return;
    }

    ccp_terminate_call();
}

void play_media(void)
{
    if (tmap_peer_is_ums() == false) {
        ESP_LOGE(TAG, "Peer not support Unicast Media Sender");
        return;
    }

    mcp_send_cmd(ESP_BLE_AUDIO_MCS_OPC_PLAY);
}

void pause_media(void)
{
    if (tmap_peer_is_ums() == false) {
        ESP_LOGE(TAG, "Peer not support Unicast Media Sender");
        return;
    }

    mcp_send_cmd(ESP_BLE_AUDIO_MCS_OPC_PAUSE);
}
