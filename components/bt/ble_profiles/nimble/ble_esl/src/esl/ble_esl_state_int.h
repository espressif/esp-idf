/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file ble_esl_state_int.h
 * @brief ESL component internal header — shared interfaces across source files
 *
 * Not part of the public API. Groups internal declarations by source file.
 */

#ifndef BLE_ESL_STATE_INT_H
#define BLE_ESL_STATE_INT_H

#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "nimble/ble.h"
#include "ble_esl.h"
#include "ble_esl_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================== Configuration Bitmask ========================== */

#define ESL_CONFIG_BIT_ADDRESS      (1 << 0)
#define ESL_CONFIG_BIT_AP_SYNC_KEY  (1 << 1)
#define ESL_CONFIG_BIT_RESP_KEY     (1 << 2)
#define ESL_CONFIG_BIT_ABS_TIME     (1 << 3)
#define ESL_CONFIG_COMPLETE_MASK    (ESL_CONFIG_BIT_ADDRESS | ESL_CONFIG_BIT_AP_SYNC_KEY | \
                                     ESL_CONFIG_BIT_RESP_KEY | ESL_CONFIG_BIT_ABS_TIME)

/* ========================== Internal State Context ========================== */
typedef struct {
    ble_esl_state_t         state;              /* Current ESL state */
    ble_esl_config_t        config;             /* Copy of init config */
    ble_esl_address_t       esl_address;        /* Stored ESL address */
    ble_esl_key_material_t  ap_sync_key;        /* AP Sync Key Material */
    ble_esl_key_material_t  resp_key;           /* ESL Response Key Material */
    uint32_t                abs_time_base;      /* Written absolute time value */
    int64_t                 abs_time_offset_us; /* esp_timer_get_time() at write */
    uint8_t                 config_complete;    /* 4-bit bitmask */
    uint16_t                conn_handle;        /* Active ACL connection handle */
    ble_esl_cb_t            app_cb;             /* Application callback */
    bool                    started;            /* Whether ble_esl_start() called */
    bool                    initialized;        /* Whether ble_esl_init() called */
    esp_timer_handle_t      sync_timer;         /* 60-min sync timeout */
    esp_timer_handle_t      unsync_timer;       /* 60-min unsync timeout */
    esp_timer_handle_t      ecp_timer;          /* 30-sec ECP procedure timeout */
    uint16_t                ecp_val_handle;     /* ECP characteristic value handle */
    ble_addr_t              bonded_peer_addr;   /* Address of bonded AP */
    bool                    has_bonded_peer;    /* Whether we have a bonded peer */
    bool                    ap_sync_key_valid;  /* AP sync key written */
    bool                    resp_key_valid;     /* Response key written */
    bool                    address_valid;      /* ESL address written */
    /* Current vs retiring PAwR sync tracking (AP reboot / PAST switch).
     * Only current_sync_handle drives SYNCHRONIZED/UPDATING sync bookkeeping.
     * retiring_* tracks a locally terminated old sync so its SYNC_LOST does not
     * look like a natural loss of the new train. */
    uint16_t                current_sync_handle;    /* Valid periodic sync; BLE_HS_CONN_HANDLE_NONE if none */
    uint16_t                retiring_sync_handle;   /* Locally terminated old sync awaiting SYNC_LOST */
    bool                    retiring_local_terminate; /* retiring SYNC_LOST is expected; do not change ESL state */
    bool                    past_received;      /* PAST completed in Updating state */
    bool                    pawr_synced;        /* Synchronized to the AP's PAwR train (retained across Updating) */
    bool                    past_pending;       /* Re-arm PAST only after retiring SYNC_LOST frees pool slot */
    bool                    update_complete_received; /* Update Complete cmd received in Updating state */
    bool                    deinit_pending;     /* ble_esl_deinit() is waiting for disconnect */
    SemaphoreHandle_t       deinit_sem;         /* Signaled when disconnect completes during deinit */
} esl_state_ctx_t;

/* ---- esl_state.c ---- */

/**
 * @brief Return the singleton ESL state context
 * @return Pointer to the global esl_state_ctx_t instance
 */
esl_state_ctx_t *esl_state_ctx_get(void);

/* ---- esl_gatts.c ---- */

/**
 * @brief Initialize the GATT server and register the ESL Service
 *
 * @param[in] config Pointer to the user-supplied ESL configuration
 * @return ESP_OK on success
 */
esp_err_t esl_gatts_init(const ble_esl_config_t *config);

/**
 * @brief Deregister the ESL Service and free GATT server resources
 * @return ESP_OK on success
 */
esp_err_t esl_gatts_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* BLE_ESL_STATE_INT_H */
