/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file esl_cmd_lifecycle.c
 * @brief ESL ECP command dispatch pipeline, lifecycle commands, and Basic State
 *        bitmap construction.
 *
 * Handles command routing (ESL_ID validation, opcode recognition, factory-reset
 * guard), lifecycle commands (Ping, Unassociate, Service Reset, Factory Reset,
 * Update Complete), and the Basic State bitmap assembly.
 */

#include <string.h>
#include <stdlib.h>

#include "esp_log.h"
#include "esp_err.h"
#include "host/ble_hs.h"
#include "host/ble_gap.h"

#include "ble_esl.h"
#include "ble_esl_common.h"
#include "ble_esl_int.h"

static const char *TAG = "esl_cmd_lifecycle";

/* ========================== Internal Context ========================== */

/**
 * @brief Lifecycle module internal state
 */
typedef struct {
    bool service_needed;        /*!< Service Needed flag (Basic State bit 0) */
    bool factory_reset_pending; /*!< Factory Reset guard — reject all subsequent cmds */
    bool unassociate_pending;   /*!< Post-response cleanup flag for Unassociate */
} lifecycle_ctx_t;

static lifecycle_ctx_t *s_ctx = NULL;

/* ========================== Forward Declarations ========================== */

static void build_error_response(ble_esl_cmd_result_t *result, uint8_t error_code);
static void build_basic_state_response(ble_esl_cmd_result_t *result, uint16_t bitmap);
static bool is_valid_opcode(uint8_t opcode);

static esp_err_t handle_ping(const uint8_t *params, uint8_t params_len,
                             ble_esl_cmd_result_t *result);
static esp_err_t handle_unassociate(const uint8_t *params, uint8_t params_len,
                                    ble_esl_cmd_result_t *result);
static esp_err_t handle_service_reset(const uint8_t *params, uint8_t params_len,
                                      ble_esl_cmd_result_t *result);
static esp_err_t handle_factory_reset(const uint8_t *params, uint8_t params_len,
                                      ble_esl_cmd_result_t *result);
static esp_err_t handle_update_complete(const uint8_t *params, uint8_t params_len,
                                        ble_esl_cmd_result_t *result);

/* ========================== Helper Functions ========================== */

/**
 * @brief Build an Error response TLV into the result structure
 *
 * @param[out] result     Command result to populate
 * @param[in]  error_code ESL error code
 */
static void build_error_response(ble_esl_cmd_result_t *result, uint8_t error_code)
{
    result->has_response = true;
    /* Error Response opcode: Tag=0x0, Length=0x0 → opcode = 0x00 */
    result->resp_opcode = BLE_ESL_RESP_ERROR;
    result->resp_params[0] = error_code;
    result->resp_params_len = 1;
}

/**
 * @brief Build a Basic State response TLV into the result structure
 *
 * @param[out] result  Command result to populate
 * @param[in]  bitmap  16-bit Basic State bitmap (little-endian)
 */
static void build_basic_state_response(ble_esl_cmd_result_t *result, uint16_t bitmap)
{
    result->has_response = true;
    /* Basic State Response opcode: Tag=0x0, Length=0x1 → opcode = 0x10 */
    result->resp_opcode = BLE_ESL_RESP_BASIC_STATE;
    result->resp_params[0] = (uint8_t)(bitmap & 0xFF);
    result->resp_params[1] = (uint8_t)((bitmap >> 8) & 0xFF);
    result->resp_params_len = 2;
}

/**
 * @brief Check if an opcode is a recognized ESL command opcode
 *
 * @param[in] opcode Command opcode byte
 * @return true if recognized, false otherwise
 */
static bool is_valid_opcode(uint8_t opcode)
{
    /* Check fixed opcodes */
    switch (opcode) {
    case BLE_ESL_CMD_PING:
    case BLE_ESL_CMD_UNASSOCIATE:
    case BLE_ESL_CMD_SERVICE_RESET:
    case BLE_ESL_CMD_FACTORY_RESET:
    case BLE_ESL_CMD_UPDATE_COMPLETE:
    case BLE_ESL_CMD_READ_SENSOR:
    case BLE_ESL_CMD_REFRESH_DISPLAY:
    case BLE_ESL_CMD_DISPLAY_IMAGE:
    case BLE_ESL_CMD_DISPLAY_TIMED_IMAGE:
    case BLE_ESL_CMD_LED_CONTROL:
    case BLE_ESL_CMD_LED_TIMED_CONTROL:
        return true;
    default:
        break;
    }

    /* Check vendor-specific: Tag nibble (bits [3:0]) == 0x0F */
    if (BLE_ESL_TLV_TAG(opcode) == BLE_ESL_CMD_VENDOR_TAG) {
#if CONFIG_BLE_ESL_VENDOR_SPECIFIC
        return true;
#else
        return false;
#endif
    }

    return false;
}

/* ========================== Lifecycle Command Handlers ========================== */

/**
 * @brief Handle Ping command (opcode 0x00)
 *
 * No side effects. Always returns a Basic State response.
 */
static esp_err_t handle_ping(const uint8_t *params, uint8_t params_len,
                             ble_esl_cmd_result_t *result)
{
    uint16_t bitmap = 0;
    esp_err_t ret = esl_build_basic_state(&bitmap);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to build basic state for Ping: %d", ret);
        /* Ping shall never produce an Error response, build best-effort bitmap */
        bitmap = 0;
    }

    build_basic_state_response(result, bitmap);
    return ESP_OK;
}

/**
 * @brief Handle Unassociate from AP command (opcode 0x01)
 *
 * Builds a Basic State response. The actual cleanup (bonding removal, state
 * transition to Unassociated, event notification) happens AFTER the response
 * is sent — signaled via the unassociate_pending flag.
 */
static esp_err_t handle_unassociate(const uint8_t *params, uint8_t params_len,
                                    ble_esl_cmd_result_t *result)
{
    uint16_t bitmap = 0;
    esp_err_t ret = esl_build_basic_state(&bitmap);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to build basic state for Unassociate: %d", ret);
        build_error_response(result, BLE_ESL_ERR_UNSPECIFIED);
        return ESP_OK;
    }

    build_basic_state_response(result, bitmap);

    /* Set post-response flag: esl_state.c checks this after sending ECP response */
    s_ctx->unassociate_pending = true;

    return ESP_OK;
}

/**
 * @brief Handle Service Reset command (opcode 0x02)
 *
 * Attempts to clear the Service Needed flag. If no persistent condition remains,
 * the flag is cleared. Always returns a Basic State response.
 */
static esp_err_t handle_service_reset(const uint8_t *params, uint8_t params_len,
                                      ble_esl_cmd_result_t *result)
{
    /*
     * Attempt to clear service_needed. Per spec, the flag is only cleared if
     * the underlying condition no longer persists. Since we don't track the
     * persistent condition separately, we simply clear the flag here.
     * The application can re-set it via ble_esl_set_service_needed(true) if
     * the condition still persists.
     */
    if (s_ctx->service_needed) {
        s_ctx->service_needed = false;
        ESP_LOGI(TAG, "Service Needed flag cleared by Service Reset");
    }

    uint16_t bitmap = 0;
    esp_err_t ret = esl_build_basic_state(&bitmap);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to build basic state for Service Reset: %d", ret);
        build_error_response(result, BLE_ESL_ERR_UNSPECIFIED);
        return ESP_OK;
    }

    build_basic_state_response(result, bitmap);
    return ESP_OK;
}

/**
 * @brief Handle Factory Reset command (opcode 0x03)
 *
 * Sets the factory_reset_pending guard, initiates disconnect, and fires the
 * FACTORY_RESET event. Produces no response (has_response = false).
 */
static esp_err_t handle_factory_reset(const uint8_t *params, uint8_t params_len,
                                      ble_esl_cmd_result_t *result)
{
    /* Set guard flag — all subsequent commands will be rejected */
    s_ctx->factory_reset_pending = true;

    /* No response for Factory Reset */
    result->has_response = false;
    result->resp_params_len = 0;

    /* Initiate disconnection using the actual active connection handle.
     * We terminate with Remote User Terminated Connection reason. */
    uint16_t conn_handle = esl_state_get_conn_handle();
    if (conn_handle != BLE_HS_CONN_HANDLE_NONE) {
        int rc = ble_gap_terminate(conn_handle, BLE_ERR_REM_USER_CONN_TERM);
        if (rc != 0 && rc != BLE_HS_EALREADY) {
            ESP_LOGW(TAG, "Failed to initiate disconnect for factory reset: %d", rc);
        }
    } else {
        ESP_LOGW(TAG, "No active connection found for factory reset disconnect");
    }

    /* Fire FACTORY_RESET event to the application */
    esl_notify_app(BLE_ESL_EVT_FACTORY_RESET, NULL);

    ESP_LOGI(TAG, "Factory Reset initiated");
    return ESP_OK;
}

/**
 * @brief Handle Update Complete command (opcode 0x04)
 *
 * Records that Update Complete was received and lets the state module decide
 * when to advance to Synchronized. The transition must NOT happen until PAST
 * (PAwR sync transfer) has also completed, because entering Synchronized tears
 * down the ACL link over which PAST is delivered — advancing early would leave
 * the ESL un-synced to the PAwR train and unable to answer commands. Per ESL
 * spec Section 3.9.2.5.2, the Update Complete command has no response.
 */
static esp_err_t handle_update_complete(const uint8_t *params, uint8_t params_len,
                                        ble_esl_cmd_result_t *result)
{
    /* No response for Update Complete per spec */
    result->has_response = false;
    result->resp_params_len = 0;

    /* Defer the SYNCHRONIZED transition to the state module, which gates it on
     * PAST reception (and, in Configuring, on the mandatory configuration
     * writes). */
    esl_notify_update_complete();

    ESP_LOGI(TAG, "Update Complete processed");
    return ESP_OK;
}

/* ========================== Public / Internal API ========================== */

esp_err_t esl_cmd_lifecycle_init(void)
{
    if (s_ctx != NULL) {
        ESP_LOGW(TAG, "Lifecycle module already initialized");
        return ESP_ERR_INVALID_STATE;
    }

    s_ctx = calloc(1, sizeof(lifecycle_ctx_t));
    if (s_ctx == NULL) {
        ESP_LOGE(TAG, "Failed to allocate lifecycle context");
        return ESP_ERR_NO_MEM;
    }

    s_ctx->service_needed = false;
    s_ctx->factory_reset_pending = false;
    s_ctx->unassociate_pending = false;

    ESP_LOGI(TAG, "Lifecycle command module initialized");
    return ESP_OK;
}

void esl_cmd_lifecycle_deinit(void)
{
    if (s_ctx != NULL) {
        free(s_ctx);
        s_ctx = NULL;
    }
    ESP_LOGI(TAG, "Lifecycle command module deinitialized");
}

esp_err_t esl_cmd_dispatch(uint8_t opcode, const uint8_t *params,
                           uint8_t params_len, ble_esl_cmd_result_t *result,
                           uint8_t transport)
{
    assert(result != NULL);

    /* Initialize result to safe defaults */
    memset(result, 0, sizeof(ble_esl_cmd_result_t));
    result->has_response = false;

    if (s_ctx == NULL) {
        ESP_LOGE(TAG, "Lifecycle module not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    /* Factory Reset Pending Guard: reject all commands with Unspecified Error.
     * The rejection is a normal, spec-mandated outcome that carries an Error
     * response, so return ESP_OK — as every other rejection path below does —
     * so callers actually transmit the response instead of treating it as a
     * dispatch failure. */
    if (s_ctx->factory_reset_pending) {
        ESP_LOGW(TAG, "Factory reset pending, rejecting command opcode=0x%02X", opcode);
        build_error_response(result, BLE_ESL_ERR_UNSPECIFIED);
        return ESP_OK;
    }

    /* Validate ESL_ID (first parameter byte) */
    if (params == NULL || params_len < 1) {
        ESP_LOGE(TAG, "Command has no parameters (missing ESL_ID)");
        build_error_response(result, BLE_ESL_ERR_INVALID_PARAMS);
        return ESP_OK;
    }

    uint8_t cmd_esl_id = params[0];
    uint8_t my_esl_id = 0;
    uint8_t my_group_id = 0;
    esp_err_t ret = esl_get_address(&my_esl_id, &my_group_id);

    if (cmd_esl_id == BLE_ESL_BROADCAST_ADDRESS) {
        if (transport == ESL_TRANSPORT_ECP) {
            /* Broadcast Address (0xFF) is only valid over PAwR, not ECP. */
            ESP_LOGW(TAG, "Broadcast address 0xFF rejected on ECP");
            build_error_response(result, BLE_ESL_ERR_INVALID_PARAMS);
            return ESP_OK;
        }
        /* Over PAwR, broadcast address is valid — skip ESL_ID matching */
    } else {
        if (ret != ESP_OK) {
            /* ESL address not configured — cannot validate ESL_ID */
            ESP_LOGW(TAG, "ESL address not configured, rejecting command");
            build_error_response(result, BLE_ESL_ERR_INVALID_PARAMS);
            return ESP_OK;
        }

        if (cmd_esl_id != my_esl_id) {
            /* ESL_ID mismatch */
            ESP_LOGW(TAG, "ESL_ID mismatch: received=0x%02X, own=0x%02X", cmd_esl_id, my_esl_id);
            build_error_response(result, BLE_ESL_ERR_INVALID_PARAMS);
            return ESP_OK;
        }
    }

    /* Check for unrecognized opcode */
    if (!is_valid_opcode(opcode)) {
        ESP_LOGW(TAG, "Unrecognized opcode: 0x%02X", opcode);
        build_error_response(result, BLE_ESL_ERR_INVALID_OPCODE);
        return ESP_OK;
    }

    /* Strip ESL_ID byte before delegating to sub-handlers */
    const uint8_t *cmd_params = params + 1;
    uint8_t cmd_params_len = params_len - 1;

    /* Route to appropriate handler */
    switch (opcode) {
    /* Lifecycle commands — handled inline */
    case BLE_ESL_CMD_PING:
        return handle_ping(cmd_params, cmd_params_len, result);

    case BLE_ESL_CMD_UNASSOCIATE:
        if (transport != ESL_TRANSPORT_ECP) {
            ESP_LOGW(TAG, "Unassociate rejected: only valid over ECP");
            build_error_response(result, BLE_ESL_ERR_INVALID_STATE);
            return ESP_OK;
        }
        return handle_unassociate(cmd_params, cmd_params_len, result);

    case BLE_ESL_CMD_SERVICE_RESET:
        return handle_service_reset(cmd_params, cmd_params_len, result);

    case BLE_ESL_CMD_FACTORY_RESET:
        return handle_factory_reset(cmd_params, cmd_params_len, result);

    case BLE_ESL_CMD_UPDATE_COMPLETE:
        return handle_update_complete(cmd_params, cmd_params_len, result);

    /* Display/Sensor commands — delegated to esl_cmd_display.c */
    case BLE_ESL_CMD_READ_SENSOR:
        return esl_cmd_read_sensor(cmd_params, cmd_params_len, result, transport);

    case BLE_ESL_CMD_REFRESH_DISPLAY:
        return esl_cmd_refresh_display(cmd_params, cmd_params_len, result);

    case BLE_ESL_CMD_DISPLAY_IMAGE:
        return esl_cmd_display_image(cmd_params, cmd_params_len, result);

    case BLE_ESL_CMD_DISPLAY_TIMED_IMAGE:
        return esl_cmd_display_timed_image(cmd_params, cmd_params_len, result);

    /* LED commands — delegated to esl_cmd_led.c */
    case BLE_ESL_CMD_LED_CONTROL:
        return esl_cmd_led_control(cmd_params, cmd_params_len, result);

    case BLE_ESL_CMD_LED_TIMED_CONTROL:
        return esl_cmd_led_timed_control(cmd_params, cmd_params_len, result);

    default:
        /* Vendor-specific commands (Tag nibble == 0x0F) */
        if (BLE_ESL_TLV_TAG(opcode) == BLE_ESL_CMD_VENDOR_TAG) {
            return esl_cmd_vendor_specific(opcode, cmd_params, cmd_params_len, result);
        }
        /* Should not reach here due to is_valid_opcode check above */
        build_error_response(result, BLE_ESL_ERR_INVALID_OPCODE);
        return ESP_OK;
    }
}

esp_err_t esl_build_basic_state(uint16_t *bitmap_out)
{
    if (bitmap_out == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (s_ctx == NULL) {
        ESP_LOGE(TAG, "Lifecycle module not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    uint16_t bitmap = 0;

    /* Bit 0: Service Needed */
    if (s_ctx->service_needed) {
        bitmap |= BLE_ESL_BASIC_STATE_SERVICE_NEEDED;
    }

    /* Bit 1: Synchronized — per spec this reports whether the ESL is
     * synchronized to the AP, not whether it sits in the Synchronized state.
     * The Updating state keeps the PAwR sync (it only ignores the data), so an
     * ESL that entered Updating from Synchronized must still report bit 1. */
    ble_esl_state_t state = ble_esl_get_state();
    if (state == BLE_ESL_STATE_SYNCHRONIZED ||
        (state == BLE_ESL_STATE_UPDATING && esl_is_pawr_synced())) {
        bitmap |= BLE_ESL_BASIC_STATE_SYNCHRONIZED;
    }

    /* Bits 2-3: Active LED and Pending LED Update */
    esl_led_status_t led_status = esl_get_led_status();
    if (led_status.active_led) {
        bitmap |= BLE_ESL_BASIC_STATE_ACTIVE_LED;
    }
    if (led_status.pending_led_update) {
        bitmap |= BLE_ESL_BASIC_STATE_PENDING_LED_UPDATE;
    }

    /* Bit 4: Pending Display Update */
    if (esl_get_display_pending()) {
        bitmap |= BLE_ESL_BASIC_STATE_PENDING_DISP_UPDATE;
    }

    /* Bits 5-15: RFU, set to 0 (already 0 from initialization) */

    *bitmap_out = bitmap;
    return ESP_OK;
}

esp_err_t ble_esl_set_service_needed(bool flag)
{
    if (s_ctx == NULL) {
        ESP_LOGE(TAG, "Lifecycle module not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    s_ctx->service_needed = flag;
    ESP_LOGI(TAG, "Service Needed flag set to %s", flag ? "true" : "false");
    return ESP_OK;
}

bool esl_is_factory_reset_pending(void)
{
    if (s_ctx == NULL) {
        return false;
    }
    return s_ctx->factory_reset_pending;
}

void esl_clear_factory_reset_pending(void)
{
    if (s_ctx != NULL) {
        s_ctx->factory_reset_pending = false;
        ESP_LOGI(TAG, "Factory reset pending flag cleared");
    }
}

bool esl_is_unassociate_pending(void)
{
    if (s_ctx == NULL) {
        return false;
    }
    return s_ctx->unassociate_pending;
}

void esl_execute_unassociate_cleanup(void)
{
    if (s_ctx == NULL || !s_ctx->unassociate_pending) {
        return;
    }

    /* Clear the pending flag first */
    s_ctx->unassociate_pending = false;

    ESP_LOGI(TAG, "Executing post-response Unassociate cleanup");

    /* Transition to Unassociated state (internally clears stored data) */
    esp_err_t ret = esl_state_transition(BLE_ESL_STATE_UNASSOCIATED);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to transition to Unassociated state: %d", ret);
    }

    /* Fire UNASSOCIATE event to the application */
    esl_notify_app(BLE_ESL_EVT_UNASSOCIATE, NULL);
}
