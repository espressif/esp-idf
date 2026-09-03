/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file esl_persisted_tag_map.h
 * @brief Pure helpers for TAG persisted snapshot field mapping (host-testable).
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "ble_esl_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_BIT_ADDRESS
#define CONFIG_BIT_ADDRESS      (1 << 0)
#define CONFIG_BIT_AP_SYNC_KEY  (1 << 1)
#define CONFIG_BIT_RESP_KEY     (1 << 2)
#define CONFIG_BIT_ABS_TIME     (1 << 3)
#endif

#define ESL_PERSISTED_CONFIG_MASK \
    (CONFIG_BIT_ADDRESS | CONFIG_BIT_AP_SYNC_KEY | CONFIG_BIT_RESP_KEY)

typedef struct {
    ble_esl_address_t esl_address;
    ble_esl_key_material_t ap_sync_key;
    ble_esl_key_material_t resp_key;
    uint8_t peer_addr_type;
    uint8_t peer_addr[6];
} esl_persisted_tag_dto_t;

typedef struct {
    bool address_valid;
    bool ap_sync_key_valid;
    bool resp_key_valid;
    bool has_bonded_peer;
    uint8_t config_complete;
    uint8_t peer_addr_type;
    uint8_t peer_addr[6];
    ble_esl_address_t esl_address;
    ble_esl_key_material_t ap_sync_key;
    ble_esl_key_material_t resp_key;
} esl_persisted_tag_ram_t;

static inline bool esl_persisted_peer_nonzero(const uint8_t addr[6])
{
    for (int i = 0; i < 6; i++) {
        if (addr[i] != 0) {
            return true;
        }
    }
    return false;
}

static inline bool esl_persisted_export_ok(const esl_persisted_tag_ram_t *ram)
{
    if (ram == NULL) {
        return false;
    }
    if (!ram->address_valid || !ram->ap_sync_key_valid || !ram->resp_key_valid ||
            !ram->has_bonded_peer) {
        return false;
    }
    if ((ram->config_complete & ESL_PERSISTED_CONFIG_MASK) != ESL_PERSISTED_CONFIG_MASK) {
        return false;
    }
    if (ram->esl_address.esl_id == BLE_ESL_BROADCAST_ADDRESS) {
        return false;
    }
    if (!esl_persisted_peer_nonzero(ram->peer_addr)) {
        return false;
    }
    return true;
}

static inline bool esl_persisted_fill_dto(const esl_persisted_tag_ram_t *ram,
                                          esl_persisted_tag_dto_t *out)
{
    if (!esl_persisted_export_ok(ram) || out == NULL) {
        return false;
    }
    out->esl_address = ram->esl_address;
    out->ap_sync_key = ram->ap_sync_key;
    out->resp_key = ram->resp_key;
    out->peer_addr_type = ram->peer_addr_type;
    memcpy(out->peer_addr, ram->peer_addr, 6);
    return true;
}

/**
 * Apply DTO into RAM fields for restore. Does not touch Absolute Time / sync handles.
 * Sets config_complete to ESL_PERSISTED_CONFIG_MASK only (ABS_TIME cleared).
 */
static inline bool esl_persisted_apply_dto(const esl_persisted_tag_dto_t *in,
                                           esl_persisted_tag_ram_t *ram)
{
    if (in == NULL || ram == NULL) {
        return false;
    }
    if (in->esl_address.esl_id == BLE_ESL_BROADCAST_ADDRESS) {
        return false;
    }
    if (!esl_persisted_peer_nonzero(in->peer_addr)) {
        return false;
    }

    ram->esl_address = in->esl_address;
    ram->ap_sync_key = in->ap_sync_key;
    ram->resp_key = in->resp_key;
    ram->peer_addr_type = in->peer_addr_type;
    memcpy(ram->peer_addr, in->peer_addr, 6);
    ram->address_valid = true;
    ram->ap_sync_key_valid = true;
    ram->resp_key_valid = true;
    ram->has_bonded_peer = true;
    ram->config_complete = (uint8_t)ESL_PERSISTED_CONFIG_MASK;
    return true;
}

#ifdef __cplusplus
}
#endif
