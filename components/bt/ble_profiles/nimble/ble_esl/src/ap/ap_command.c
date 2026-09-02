/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file ble_esl_ap_command.c
 * @brief ESL AP — Command dispatch and response parsing
 *
 * Implements transport selection (ECP vs PAwR), TLV command encoding,
 * ECP procedure timeout management, and response parsing for all six
 * ESL response types.
 */

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "ble_esl_ap.h"
#include "ble_esl_ap_int.h"
#include "ble_esl_common.h"

static const char *TAG = "esl_ap_cmd";

/* ========================== Constants ========================== */

/** ECP procedure timeout in microseconds (30 seconds) */
#define ECP_TIMEOUT_US  ((uint64_t)BLE_ESL_ECP_TIMEOUT_SEC * 1000000ULL)

/* ========================== Per-Connection Command Context ========================== */

/**
 * @brief Per-connection ECP command context
 *
 * Tracks the pending ECP command, its target ESL address, the opcode
 * (for Unassociate state transition), and the 30-second timeout timer.
 */
typedef struct {
    bool in_use;                    /*!< Context is active */
    uint16_t conn_handle;           /*!< ACL connection handle */
    uint8_t esl_id;                 /*!< Target ESL_ID */
    uint8_t group_id;               /*!< Target Group_ID */
    uint8_t cmd_opcode;             /*!< Command opcode sent */
    esp_timer_handle_t timer;       /*!< 30-second ECP timeout timer */
} esl_ap_ecp_cmd_ctx_t;

/** Mutex protecting the ECP context array from concurrent access across tasks */
static SemaphoreHandle_t s_ecp_ctx_mutex;

/* ========================== PAwR Response Slot Mapping ========================== */

/** Maximum number of response slots tracked per subevent */
#define PAWR_MAX_RESPONSE_SLOTS  16

/** Invalid ESL address sentinel for slot mapping */
#define ESL_ADDR_INVALID  0xFFFF

/**
 * @brief Per-group PAwR response slot to ESL address mapping
 *
 * When the AP sends a PAwR payload containing individually-addressed TLVs,
 * it records which ESL address is expected to respond in each slot.
 * The response handler uses this mapping to look up the correct ESL entry.
 */
typedef struct {
    bool valid;                                          /*!< Mapping is active for this group */
    uint8_t group_id;                                    /*!< Group ID (subevent) */
    uint8_t num_slots;                                   /*!< Number of slots populated */
    uint16_t slot_to_esl_addr[PAWR_MAX_RESPONSE_SLOTS]; /*!< slot index → ESL address */
} pawr_slot_map_t;

/* ========================== Module Runtime Context ========================== */

/**
 * @brief Aggregate heap-allocated runtime context for the command sub-module
 *
 * Holds the RAM-heavy tables that were previously statically allocated in
 * .bss. Allocated once from the heap on init and freed on deinit, referenced
 * through a single pointer (s_ap_command).
 *
 * @note The mutex protecting @ref ecp_ctx is intentionally NOT part of this
 *       struct. It is kept as a separate static handle (s_ecp_ctx_mutex)
 *       because it is the synchronization primitive that guards the lifecycle
 *       of this very block: the module's "is-initialized" checks and the timer
 *       / notification callbacks rely on the mutex being valid independently of
 *       the heap block it protects. Embedding the handle inside the memory it
 *       guards would be fragile (a stale ctx pointer into a freed block would
 *       have to be dereferenced just to reach the lock).
 */
typedef struct {
    /** Array of per-connection ECP command contexts */
    esl_ap_ecp_cmd_ctx_t ecp_ctx[CONFIG_BLE_ESL_AP_MAX_CONNECTIONS];
    /** Per-group slot mapping table (one entry per possible concurrent group) */
    pawr_slot_map_t pawr_slot_map[BLE_ESL_GROUP_ID_MAX + 1];
} ap_command_ctx_t;

/** Single heap-allocated runtime context (NULL until init) */
static ap_command_ctx_t *s_ap_command;

/**
 * @brief Record the response slot mapping for a PAwR send
 *
 * For a single unicast TLV, the ESL's response slot is always 0
 * (TLV #1 → slot = 1 - 1 = 0).
 */
static void pawr_record_slot_mapping(uint8_t group_id, uint8_t esl_id)
{
    if (s_ap_command == NULL || group_id > BLE_ESL_GROUP_ID_MAX) {
        return;
    }
    pawr_slot_map_t *map = &s_ap_command->pawr_slot_map[group_id];
    /* Clear previous mapping for this group */
    memset(map->slot_to_esl_addr, 0xFF, sizeof(map->slot_to_esl_addr));
    map->valid = true;
    map->group_id = group_id;
    map->num_slots = 1;
    /* Single unicast TLV → slot 0 */
    map->slot_to_esl_addr[0] = BLE_ESL_AP_MAKE_ADDR(esl_id, group_id);
}

/**
 * @brief Look up the ESL address for a given PAwR response slot
 *
 * @param group_id      Group ID (subevent)
 * @param response_slot Response slot index
 * @return ESL address, or ESL_ADDR_INVALID if not found
 */
static uint16_t pawr_lookup_slot(uint8_t group_id, uint8_t response_slot)
{
    if (s_ap_command == NULL || group_id > BLE_ESL_GROUP_ID_MAX) {
        return ESL_ADDR_INVALID;
    }
    pawr_slot_map_t *map = &s_ap_command->pawr_slot_map[group_id];
    if (!map->valid || response_slot >= PAWR_MAX_RESPONSE_SLOTS) {
        return ESL_ADDR_INVALID;
    }
    return map->slot_to_esl_addr[response_slot];
}

/* ========================== Forward Declarations ========================== */

static void ecp_timeout_cb(void *arg);
static void ecp_response_cb(uint16_t conn_handle, esp_err_t status,
                            const uint8_t *data, uint16_t data_len,
                            void *user_data);
static void parse_response(uint8_t esl_id, uint8_t group_id,
                           const uint8_t *data, uint8_t data_len,
                           uint8_t cmd_opcode);
static esp_err_t dispatch_command(uint8_t esl_id, uint8_t group_id,
                                  uint8_t opcode, const uint8_t *params,
                                  uint8_t params_len, bool ecp_only);
static esp_err_t build_led_params(uint8_t esl_id, uint8_t led_index,
                                  const esl_ap_led_settings_t *settings,
                                  uint8_t *out_params, uint8_t *out_len);

/* ========================== Context Helpers ========================== */

/**
 * @brief Find a free ECP command context slot
 */
static esl_ap_ecp_cmd_ctx_t *alloc_ecp_ctx(void)
{
    if (s_ap_command == NULL) {
        return NULL;
    }
    for (int i = 0; i < CONFIG_BLE_ESL_AP_MAX_CONNECTIONS; i++) {
        if (!s_ap_command->ecp_ctx[i].in_use) {
            return &s_ap_command->ecp_ctx[i];
        }
    }
    return NULL;
}

/**
 * @brief Find an ECP command context by connection handle
 */
static esl_ap_ecp_cmd_ctx_t *find_ecp_ctx_by_conn(uint16_t conn_handle)
{
    if (s_ap_command == NULL) {
        return NULL;
    }
    for (int i = 0; i < CONFIG_BLE_ESL_AP_MAX_CONNECTIONS; i++) {
        if (s_ap_command->ecp_ctx[i].in_use && s_ap_command->ecp_ctx[i].conn_handle == conn_handle) {
            return &s_ap_command->ecp_ctx[i];
        }
    }
    return NULL;
}

/**
 * @brief Free an ECP command context, stopping and deleting its timer
 *
 * Also clears the associated connection's pending ECP state to prevent
 * late-arriving notifications from accessing stale context pointers.
 *
 * @note Caller MUST hold s_ecp_ctx_mutex.
 */
static void free_ecp_ctx(esl_ap_ecp_cmd_ctx_t *ctx)
{
    if (ctx == NULL) {
        return;
    }

    if (!ctx->in_use) {
        return;
    }

    /* Clear the connection's pending ECP state only if we own it */
    ble_esl_ap_conn_t *conn = ble_esl_ap_find_conn(ctx->conn_handle);
    if (conn != NULL && conn->ecp_user_data == ctx) {
        conn->ecp_pending = false;
        conn->ecp_cb = NULL;
        conn->ecp_user_data = NULL;
    }

    if (ctx->timer != NULL) {
        esp_timer_handle_t timer = ctx->timer;
        ctx->timer = NULL;
        esp_timer_stop(timer);  /* Ignore error if not running */
        esp_timer_delete(timer);
    }
    memset(ctx, 0, sizeof(*ctx));
}

/* ========================== Init / Deinit ========================== */

esp_err_t ble_esl_ap_command_init(void)
{
    /* Allocate the aggregate runtime context once (calloc zero-initializes,
     * replacing the previous memset of the static tables). Plain internal
     * heap is fine: these tables have no DMA / special-capability needs. */
    s_ap_command = calloc(1, sizeof(ap_command_ctx_t));
    if (s_ap_command == NULL) {
        ESP_LOGE(TAG, "Failed to allocate command context");
        return ESP_ERR_NO_MEM;
    }

    s_ecp_ctx_mutex = xSemaphoreCreateMutex();
    if (s_ecp_ctx_mutex == NULL) {
        ESP_LOGE(TAG, "Failed to create ECP context mutex");
        free(s_ap_command);
        s_ap_command = NULL;
        return ESP_ERR_NO_MEM;
    }

    ESP_LOGI(TAG, "Command sub-module initialized");
    return ESP_OK;
}

void ble_esl_ap_command_deinit(void)
{
    if (s_ap_command == NULL) {
        return;
    }

    /*
     * First pass: stop all timers with blocking wait OUTSIDE the mutex
     * to avoid deadlock (timer callbacks also take the mutex).
     * This ensures no callbacks are in-flight after this loop.
     */
    for (int i = 0; i < CONFIG_BLE_ESL_AP_MAX_CONNECTIONS; i++) {
        if (s_ap_command->ecp_ctx[i].in_use && s_ap_command->ecp_ctx[i].timer != NULL) {
            esp_timer_stop_blocking(s_ap_command->ecp_ctx[i].timer, portMAX_DELAY);
        }
    }

    /* Second pass: clean up contexts under the mutex */
    if (s_ecp_ctx_mutex != NULL) {
        xSemaphoreTake(s_ecp_ctx_mutex, portMAX_DELAY);
    }

    for (int i = 0; i < CONFIG_BLE_ESL_AP_MAX_CONNECTIONS; i++) {
        if (s_ap_command->ecp_ctx[i].in_use) {
            free_ecp_ctx(&s_ap_command->ecp_ctx[i]);
        }
    }

    if (s_ecp_ctx_mutex != NULL) {
        xSemaphoreGive(s_ecp_ctx_mutex);
        vSemaphoreDelete(s_ecp_ctx_mutex);
        s_ecp_ctx_mutex = NULL;
    }

    /* Release the aggregate context last, once no callback can reach it. */
    free(s_ap_command);
    s_ap_command = NULL;

    ESP_LOGI(TAG, "Command sub-module deinitialized");
}

/* ========================== Connection Cleanup ========================== */

void ble_esl_ap_command_cleanup_conn(uint16_t conn_handle)
{
    if (s_ecp_ctx_mutex == NULL) {
        return;
    }

    xSemaphoreTake(s_ecp_ctx_mutex, portMAX_DELAY);
    esl_ap_ecp_cmd_ctx_t *ctx = find_ecp_ctx_by_conn(conn_handle);
    if (ctx != NULL) {
        ESP_LOGW(TAG, "Cleaning up pending ECP context for conn_handle=0x%04x", conn_handle);
        free_ecp_ctx(ctx);
    }
    xSemaphoreGive(s_ecp_ctx_mutex);
}

/* ========================== ECP Timeout Callback ========================== */

/**
 * @brief ECP procedure timeout callback (30 seconds)
 *
 * Fires BLE_ESL_AP_EVT_CMD_TIMEOUT to the application. The AP shall not
 * start any new ECP procedure on this link.
 */
static void ecp_timeout_cb(void *arg)
{
    esl_ap_ecp_cmd_ctx_t *ctx = (esl_ap_ecp_cmd_ctx_t *)arg;
    assert(ctx != NULL);

    xSemaphoreTake(s_ecp_ctx_mutex, portMAX_DELAY);

    /* Check if context is still active (may have been freed by response cb) */
    if (!ctx->in_use) {
        xSemaphoreGive(s_ecp_ctx_mutex);
        return;
    }

    ESP_LOGW(TAG, "ECP timeout: esl_id=0x%02X group_id=0x%02X conn=0x%04X",
             ctx->esl_id, ctx->group_id, ctx->conn_handle);

    /* Capture event data before freeing context */
    uint8_t esl_id = ctx->esl_id;
    uint8_t group_id = ctx->group_id;

    /* Clean up the context */
    free_ecp_ctx(ctx);

    xSemaphoreGive(s_ecp_ctx_mutex);

    /* Deliver timeout event to application */
    if (g_esl_ap != NULL && g_esl_ap->app_cb != NULL) {
        esl_ap_cmd_timeout_t timeout_evt = {
            .esl_id = esl_id,
            .group_id = group_id,
        };
        g_esl_ap->app_cb(BLE_ESL_AP_EVT_CMD_TIMEOUT, &timeout_evt);
    }
}

/* ========================== ECP Response Callback ========================== */

/**
 * @brief ECP notification callback — invoked when the ESL responds via ECP
 *
 * Stops the timeout timer, parses the response, and delivers the event.
 */
static void ecp_response_cb(uint16_t conn_handle, esp_err_t status,
                            const uint8_t *data, uint16_t data_len,
                            void *user_data)
{
    esl_ap_ecp_cmd_ctx_t *ctx = (esl_ap_ecp_cmd_ctx_t *)user_data;
    if (ctx == NULL) {
        ESP_LOGE(TAG, "ECP response callback with NULL context");
        return;
    }

    xSemaphoreTake(s_ecp_ctx_mutex, portMAX_DELAY);

    /* Check if context is still active (may have been freed by timeout cb) */
    if (!ctx->in_use) {
        xSemaphoreGive(s_ecp_ctx_mutex);
        return;
    }

    /* Stop the timeout timer */
    if (ctx->timer != NULL) {
        esp_timer_stop(ctx->timer);  /* Ignore error if already stopped */
    }

    if (status != ESP_OK) {
        ESP_LOGE(TAG, "ECP write/notification failed: status=0x%X, conn=0x%04X",
                 status, conn_handle);
        free_ecp_ctx(ctx);
        xSemaphoreGive(s_ecp_ctx_mutex);
        return;
    }

    if (data == NULL || data_len == 0) {
        /* Factory Reset has no response — this is expected */
        ESP_LOGD(TAG, "ECP response with no data (conn=0x%04X)", conn_handle);
        free_ecp_ctx(ctx);
        xSemaphoreGive(s_ecp_ctx_mutex);
        return;
    }

    /* Capture context data before freeing */
    uint8_t esl_id = ctx->esl_id;
    uint8_t group_id = ctx->group_id;
    uint8_t cmd_opcode = ctx->cmd_opcode;

    /* Clean up */
    free_ecp_ctx(ctx);

    xSemaphoreGive(s_ecp_ctx_mutex);

    /* Parse and deliver the response (outside lock) */
    parse_response(esl_id, group_id, data, (uint8_t)(data_len > 255 ? 255 : data_len), cmd_opcode);
}

/* ========================== Response Parsing ========================== */

/**
 * @brief Parse a response TLV and deliver BLE_ESL_AP_EVT_RESPONSE
 *
 * Decodes the TLV, determines the response type from the tag/length nibbles,
 * populates esl_ap_response_t, and fires the event callback.
 *
 * @param esl_id     ESL_ID of the responding ESL
 * @param group_id   Group_ID of the responding ESL
 * @param data       Raw response TLV data
 * @param data_len   Length of data
 * @param cmd_opcode The command opcode that was sent (for state transitions)
 */
static void parse_response(uint8_t esl_id, uint8_t group_id,
                           const uint8_t *data, uint8_t data_len,
                           uint8_t cmd_opcode)
{
    if (data == NULL || data_len < BLE_ESL_TLV_MIN_SIZE) {
        ESP_LOGE(TAG, "Response too short: len=%u", data_len);
        return;
    }

    uint8_t opcode = 0;
    const uint8_t *params = NULL;
    uint8_t params_len = 0;

    esp_err_t ret = ble_esl_tlv_decode(data, data_len, &opcode, &params, &params_len);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "TLV decode failed: 0x%X", ret);
        return;
    }

    uint8_t tag_nibble = BLE_ESL_TLV_TAG(opcode);
    uint8_t len_nibble = BLE_ESL_TLV_LENGTH(opcode);

    esl_ap_response_t response;
    memset(&response, 0, sizeof(response));
    response.esl_id = esl_id;
    response.group_id = group_id;

    if (tag_nibble == 0x00 && len_nibble == 0x00) {
        /* Error Response (opcode 0x00) */
        response.type = ESL_AP_RESP_ERROR;
        if (params_len >= 1) {
            response.error.error_code = params[0];
        }
        ESP_LOGW(TAG, "Error response: esl_id=0x%02X error_code=0x%02X",
                 esl_id, response.error.error_code);

    } else if (tag_nibble == 0x01 && len_nibble == 0x00) {
        /* LED State Response (opcode 0x01) */
        response.type = ESL_AP_RESP_LED_STATE;
        if (params_len >= 1) {
            response.led_state.led_index = params[0];
        }
        ESP_LOGD(TAG, "LED state response: esl_id=0x%02X led_index=%u",
                 esl_id, response.led_state.led_index);

    } else if (tag_nibble == 0x00 && len_nibble == 0x01) {
        /* Basic State Response (opcode 0x10) */
        response.type = ESL_AP_RESP_BASIC_STATE;
        if (params_len >= 2) {
            uint16_t bitmap = (uint16_t)(params[0] | (params[1] << 8));
            response.basic_state.service_needed =
                (bitmap & BLE_ESL_BASIC_STATE_SERVICE_NEEDED) != 0;
            response.basic_state.synchronized =
                (bitmap & BLE_ESL_BASIC_STATE_SYNCHRONIZED) != 0;
            response.basic_state.active_led =
                (bitmap & BLE_ESL_BASIC_STATE_ACTIVE_LED) != 0;
            response.basic_state.pending_led_update =
                (bitmap & BLE_ESL_BASIC_STATE_PENDING_LED_UPDATE) != 0;
            response.basic_state.pending_display_update =
                (bitmap & BLE_ESL_BASIC_STATE_PENDING_DISP_UPDATE) != 0;
        }
        ESP_LOGD(TAG, "Basic state response: esl_id=0x%02X svc_needed=%d sync=%d",
                 esl_id, response.basic_state.service_needed,
                 response.basic_state.synchronized);

        /* For Unassociate command, transition ESL to Unassociated state */
        if (cmd_opcode == BLE_ESL_CMD_UNASSOCIATE) {
            uint16_t esl_addr = BLE_ESL_AP_MAKE_ADDR(esl_id, group_id);
            esp_err_t err = ble_esl_ap_update_esl_state(esl_addr,
                                                        BLE_ESL_STATE_UNASSOCIATED);
            if (err != ESP_OK) {
                ESP_LOGW(TAG, "Failed to update ESL state to Unassociated: 0x%X", err);
            }
        }

    } else if (tag_nibble == 0x01 && len_nibble == 0x01) {
        /* Display State Response (opcode 0x11) */
        response.type = ESL_AP_RESP_DISPLAY_STATE;
        if (params_len >= 2) {
            response.display_state.display_index = params[0];
            response.display_state.image_index = params[1];
        }
        ESP_LOGD(TAG, "Display state response: esl_id=0x%02X disp=%u img=%u",
                 esl_id, response.display_state.display_index,
                 response.display_state.image_index);

    } else if (tag_nibble == 0x0E) {
        /* Sensor Value Response (opcode 0x_E) */
        response.type = ESL_AP_RESP_SENSOR_VALUE;
        if (params_len >= 1) {
            response.sensor_value.sensor_index = params[0];
            if (params_len > 1) {
                response.sensor_value.data = &params[1];
                response.sensor_value.data_len = params_len - 1;
            } else {
                response.sensor_value.data = NULL;
                response.sensor_value.data_len = 0;
            }
        }
        ESP_LOGD(TAG, "Sensor value response: esl_id=0x%02X sensor=%u data_len=%u",
                 esl_id, response.sensor_value.sensor_index,
                 response.sensor_value.data_len);

    } else if (tag_nibble == 0x0F) {
        /* Vendor-specific Response (opcode 0x_F) */
        response.type = ESL_AP_RESP_VENDOR;
        response.vendor.data = params;
        response.vendor.data_len = params_len;
        ESP_LOGD(TAG, "Vendor response: esl_id=0x%02X data_len=%u",
                 esl_id, response.vendor.data_len);

    } else {
        ESP_LOGW(TAG, "Unknown response opcode: 0x%02X (tag=0x%X len=0x%X)",
                 opcode, tag_nibble, len_nibble);
        return;
    }

    /* Deliver the response event to the application */
    if (g_esl_ap != NULL && g_esl_ap->app_cb != NULL) {
        g_esl_ap->app_cb(BLE_ESL_AP_EVT_RESPONSE, &response);
    }
}

/* ========================== Transport Dispatch ========================== */

/**
 * @brief Core command dispatch logic — selects transport and sends the command
 *
 * @param esl_id     Target ESL_ID (0x00–0xFE, or 0xFF for broadcast)
 * @param group_id   Target Group_ID (0x00–0x7F)
 * @param opcode     Command opcode
 * @param params     Command parameter bytes (first byte is esl_id)
 * @param params_len Length of params
 * @param ecp_only   If true, command is restricted to ECP transport
 * @return ESP_OK on success; error code on failure
 */
static esp_err_t dispatch_command(uint8_t esl_id, uint8_t group_id,
                                  uint8_t opcode, const uint8_t *params,
                                  uint8_t params_len, bool ecp_only)
{
    if (g_esl_ap == NULL || !g_esl_ap->initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    if (group_id > BLE_ESL_GROUP_ID_MAX) {
        ESP_LOGE(TAG, "Invalid group_id: 0x%02X", group_id);
        return ESP_ERR_INVALID_ARG;
    }

    /* Encode the command into TLV wire format */
    uint8_t tlv_buf[BLE_ESL_TLV_MAX_SIZE];
    uint8_t tlv_len = 0;
    esp_err_t ret = ble_esl_tlv_encode(opcode, params, params_len,
                                       tlv_buf, &tlv_len);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "TLV encode failed: 0x%X", ret);
        return ret;
    }

    /* Broadcast: always use PAwR */
    if (esl_id == BLE_ESL_BROADCAST_ADDRESS) {
        if (ecp_only) {
            ESP_LOGE(TAG, "Command 0x%02X cannot be broadcast", opcode);
            return ESP_ERR_INVALID_ARG;
        }

        /* Build ESL payload and send via PAwR */
        uint8_t payload_buf[BLE_ESL_PAYLOAD_MAX_SIZE];
        uint8_t payload_len = 0;
        const uint8_t *tlv_ptrs[] = { tlv_buf };
        const uint8_t tlv_lens[] = { tlv_len };

        ret = ble_esl_payload_encode(group_id, tlv_ptrs, tlv_lens, 1,
                                     payload_buf, &payload_len);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Payload encode failed: 0x%X", ret);
            return ret;
        }

        ret = ble_esl_ap_pawr_send(group_id, payload_buf, payload_len);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "PAwR send failed: 0x%X", ret);
        }
        return ret;
    }

    /* Unicast: determine transport based on ESL tracked state */
    uint16_t esl_addr = BLE_ESL_AP_MAKE_ADDR(esl_id, group_id);
    ble_esl_state_t state = ble_esl_ap_get_esl_state(ble_esl_addr_make(esl_id, group_id));

    switch (state) {
    case BLE_ESL_STATE_CONFIGURING:
    case BLE_ESL_STATE_UPDATING: {
        /* ECP transport (connection-oriented) */
        ble_esl_ap_esl_entry_t *esl = ble_esl_ap_find_esl(esl_addr);
        if (esl == NULL || esl->conn_handle == BLE_ESL_AP_CONN_HANDLE_INVALID) {
            ESP_LOGE(TAG, "No active connection for ESL 0x%04X", esl_addr);
            return ESP_ERR_INVALID_STATE;
        }

        uint16_t conn_handle = esl->conn_handle;

        xSemaphoreTake(s_ecp_ctx_mutex, portMAX_DELAY);

        /* Check if there's already a pending ECP command on this connection */
        esl_ap_ecp_cmd_ctx_t *existing = find_ecp_ctx_by_conn(conn_handle);
        if (existing != NULL) {
            xSemaphoreGive(s_ecp_ctx_mutex);
            ESP_LOGE(TAG, "ECP already pending on conn 0x%04X", conn_handle);
            return ESP_ERR_INVALID_STATE;
        }

        /* Allocate a command context */
        esl_ap_ecp_cmd_ctx_t *ctx = alloc_ecp_ctx();
        if (ctx == NULL) {
            xSemaphoreGive(s_ecp_ctx_mutex);
            ESP_LOGE(TAG, "No free ECP command context");
            return ESP_ERR_NO_MEM;
        }

        ctx->in_use = true;
        ctx->conn_handle = conn_handle;
        ctx->esl_id = esl_id;
        ctx->group_id = group_id;
        ctx->cmd_opcode = opcode;

        /* Create the 30-second timeout timer */
        esp_timer_create_args_t timer_args = {
            .callback = ecp_timeout_cb,
            .arg = ctx,
            .dispatch_method = ESP_TIMER_TASK,
            .name = "ecp_timeout",
        };
        ret = esp_timer_create(&timer_args, &ctx->timer);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to create ECP timeout timer: 0x%X", ret);
            free_ecp_ctx(ctx);
            xSemaphoreGive(s_ecp_ctx_mutex);
            return ret;
        }

        /* Start the 30-second timeout timer BEFORE initiating the write
         * to prevent a race where a fast response frees the context
         * before the timer is started (Issue 7). */
        ret = esp_timer_start_once(ctx->timer, ECP_TIMEOUT_US);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to start ECP timeout timer: 0x%X", ret);
            free_ecp_ctx(ctx);
            xSemaphoreGive(s_ecp_ctx_mutex);
            return ret;
        }

        xSemaphoreGive(s_ecp_ctx_mutex);

        /* Write the TLV to ECP characteristic */
        ret = ble_esl_ap_ecp_write(conn_handle, tlv_buf, tlv_len,
                                   ecp_response_cb, ctx);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "ECP write failed: 0x%X", ret);
            xSemaphoreTake(s_ecp_ctx_mutex, portMAX_DELAY);
            free_ecp_ctx(ctx);
            xSemaphoreGive(s_ecp_ctx_mutex);
            return ret;
        }

        return ESP_OK;
    }

    case BLE_ESL_STATE_SYNCHRONIZED: {
        /* ECP-only commands cannot be sent via PAwR */
        if (ecp_only) {
            ESP_LOGE(TAG, "Command 0x%02X is ECP-only, cannot send in Synchronized state",
                     opcode);
            return ESP_ERR_INVALID_STATE;
        }

        /* PAwR transport (connectionless) */
        uint8_t payload_buf[BLE_ESL_PAYLOAD_MAX_SIZE];
        uint8_t payload_len = 0;
        const uint8_t *tlv_ptrs[] = { tlv_buf };
        const uint8_t tlv_lens[] = { tlv_len };

        ret = ble_esl_payload_encode(group_id, tlv_ptrs, tlv_lens, 1,
                                     payload_buf, &payload_len);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Payload encode failed: 0x%X", ret);
            return ret;
        }

        /* Store the command opcode for response correlation */
        ble_esl_ap_esl_entry_t *esl = ble_esl_ap_find_esl(esl_addr);
        if (esl != NULL) {
            esl->pending_pawr_cmd_opcode = opcode;
        }

        /* Record the response slot mapping for this group (Issue 9) */
        pawr_record_slot_mapping(group_id, esl_id);

        ret = ble_esl_ap_pawr_send(group_id, payload_buf, payload_len);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "PAwR send failed: 0x%X", ret);
        }
        return ret;
    }

    case BLE_ESL_STATE_UNASSOCIATED:
    case BLE_ESL_STATE_UNSYNCHRONIZED:
    default:
        ESP_LOGE(TAG, "ESL 0x%04X in state %d — cannot dispatch command",
                 esl_addr, state);
        return ESP_ERR_INVALID_STATE;
    }
}

/* ========================== LED Parameter Builder ========================== */

/**
 * @brief Build LED control parameter bytes from settings
 *
 * LED Control params: esl_id(1) + led_index(1) + color_brightness(1) +
 *                     flashing_pattern(7) + repeat(2) = 12 bytes
 *
 * @param esl_id    Target ESL_ID
 * @param led_index LED index
 * @param settings  LED control settings
 * @param out_params Output buffer (must be >= 12 bytes)
 * @param out_len   Output: number of bytes written
 * @return ESP_OK on success
 */
static esp_err_t build_led_params(uint8_t esl_id, uint8_t led_index,
                                  const esl_ap_led_settings_t *settings,
                                  uint8_t *out_params, uint8_t *out_len)
{
    if (settings == NULL || out_params == NULL || out_len == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t pos = 0;

    /* ESL_ID */
    out_params[pos++] = esl_id;

    /* LED_Index */
    out_params[pos++] = led_index;

    /* Color + Brightness byte */
    out_params[pos++] = BLE_ESL_LED_CTRL_COLOR_BRIGHTNESS(
        settings->color_red, settings->color_green,
        settings->color_blue, settings->brightness);

    /* Flashing pattern (7 bytes) */
    memcpy(&out_params[pos], settings->flashing_pattern, 7);
    pos += 7;

    /* Repeat field (2 bytes, little-endian) */
    uint16_t repeat = BLE_ESL_LED_CTRL_REPEAT(settings->repeat_type,
                                               settings->repeats_duration);
    out_params[pos++] = (uint8_t)(repeat & 0xFF);
    out_params[pos++] = (uint8_t)((repeat >> 8) & 0xFF);

    *out_len = pos;  /* Should be 12 */
    return ESP_OK;
}

/* ========================== Public Command APIs ========================== */

esp_err_t ble_esl_ap_ping(uint8_t esl_id, uint8_t group_id)
{
    /* Ping: opcode 0x00, params = [esl_id] (1 byte) */
    uint8_t params[1] = { esl_id };
    return dispatch_command(esl_id, group_id, BLE_ESL_CMD_PING,
                           params, sizeof(params), false);
}

esp_err_t ble_esl_ap_unassociate(uint8_t esl_id, uint8_t group_id)
{
    /* Unassociate: opcode 0x01, ECP only, no broadcast.
     * Per ESL Profile spec (Section 6.1.4), Unassociate is a connection-oriented
     * command that shall only be written to the ECP characteristic. */
    if (esl_id == BLE_ESL_BROADCAST_ADDRESS) {
        ESP_LOGE(TAG, "Unassociate cannot be broadcast");
        return ESP_ERR_INVALID_ARG;
    }
    if (group_id > BLE_ESL_GROUP_ID_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t params[1] = { esl_id };
    return dispatch_command(esl_id, group_id, BLE_ESL_CMD_UNASSOCIATE,
                           params, sizeof(params), true);
}

esp_err_t ble_esl_ap_service_reset(uint8_t esl_id, uint8_t group_id)
{
    /* Service Reset: opcode 0x02, params = [esl_id] (1 byte) */
    uint8_t params[1] = { esl_id };
    return dispatch_command(esl_id, group_id, BLE_ESL_CMD_SERVICE_RESET,
                           params, sizeof(params), false);
}

esp_err_t ble_esl_ap_factory_reset(uint8_t esl_id, uint8_t group_id)
{
    /* Factory Reset: opcode 0x03, ECP only, no broadcast */
    if (esl_id == BLE_ESL_BROADCAST_ADDRESS) {
        ESP_LOGE(TAG, "Factory Reset cannot be broadcast");
        return ESP_ERR_INVALID_ARG;
    }
    if (group_id > BLE_ESL_GROUP_ID_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t params[1] = { esl_id };
    return dispatch_command(esl_id, group_id, BLE_ESL_CMD_FACTORY_RESET,
                           params, sizeof(params), true);
}

esp_err_t ble_esl_ap_read_sensor(uint8_t esl_id, uint8_t group_id,
                                 uint8_t sensor_index)
{
    /* Read Sensor: opcode 0x10, params = [esl_id, sensor_index] (2 bytes) */
    uint8_t params[2] = { esl_id, sensor_index };
    return dispatch_command(esl_id, group_id, BLE_ESL_CMD_READ_SENSOR,
                           params, sizeof(params), false);
}

esp_err_t ble_esl_ap_refresh_display(uint8_t esl_id, uint8_t group_id,
                                     uint8_t display_index)
{
    /* Refresh Display: opcode 0x11, params = [esl_id, display_index] (2 bytes) */
    uint8_t params[2] = { esl_id, display_index };
    return dispatch_command(esl_id, group_id, BLE_ESL_CMD_REFRESH_DISPLAY,
                           params, sizeof(params), false);
}

esp_err_t ble_esl_ap_display_image(uint8_t esl_id, uint8_t group_id,
                                   uint8_t display_index, uint8_t image_index)
{
    /* Display Image: opcode 0x20, params = [esl_id, display_index, image_index] (3 bytes) */
    uint8_t params[3] = { esl_id, display_index, image_index };
    return dispatch_command(esl_id, group_id, BLE_ESL_CMD_DISPLAY_IMAGE,
                           params, sizeof(params), false);
}

esp_err_t ble_esl_ap_display_timed_image(uint8_t esl_id, uint8_t group_id,
                                         uint8_t display_index, uint8_t image_index,
                                         uint32_t absolute_time)
{
    /* Display Timed Image: opcode 0x60,
     * params = [esl_id, display_index, image_index, abs_time(4)] (7 bytes) */
    uint8_t params[7];
    params[0] = esl_id;
    params[1] = display_index;
    params[2] = image_index;
    params[3] = (uint8_t)(absolute_time & 0xFF);
    params[4] = (uint8_t)((absolute_time >> 8) & 0xFF);
    params[5] = (uint8_t)((absolute_time >> 16) & 0xFF);
    params[6] = (uint8_t)((absolute_time >> 24) & 0xFF);

    return dispatch_command(esl_id, group_id, BLE_ESL_CMD_DISPLAY_TIMED_IMAGE,
                           params, sizeof(params), false);
}

esp_err_t ble_esl_ap_led_control(uint8_t esl_id, uint8_t group_id,
                                 uint8_t led_index,
                                 const esl_ap_led_settings_t *settings)
{
    if (settings == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    /* LED Control: opcode 0xB0, params = 12 bytes */
    uint8_t params[12];
    uint8_t params_len = 0;
    esp_err_t ret = build_led_params(esl_id, led_index, settings,
                                     params, &params_len);
    if (ret != ESP_OK) {
        return ret;
    }

    return dispatch_command(esl_id, group_id, BLE_ESL_CMD_LED_CONTROL,
                           params, params_len, false);
}

esp_err_t ble_esl_ap_led_timed_control(uint8_t esl_id, uint8_t group_id,
                                       uint8_t led_index,
                                       const esl_ap_led_settings_t *settings,
                                       uint32_t absolute_time)
{
    if (settings == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    /* LED Timed Control: opcode 0xF0, params = 16 bytes (12 LED + 4 abs_time) */
    uint8_t params[16];
    uint8_t led_len = 0;
    esp_err_t ret = build_led_params(esl_id, led_index, settings,
                                     params, &led_len);
    if (ret != ESP_OK) {
        return ret;
    }

    /* Append absolute_time (4 bytes, little-endian) */
    params[led_len]     = (uint8_t)(absolute_time & 0xFF);
    params[led_len + 1] = (uint8_t)((absolute_time >> 8) & 0xFF);
    params[led_len + 2] = (uint8_t)((absolute_time >> 16) & 0xFF);
    params[led_len + 3] = (uint8_t)((absolute_time >> 24) & 0xFF);

    return dispatch_command(esl_id, group_id, BLE_ESL_CMD_LED_TIMED_CONTROL,
                           params, led_len + 4, false);
}

esp_err_t ble_esl_ap_vendor_command(uint8_t esl_id, uint8_t group_id,
                                    const uint8_t *params, uint8_t params_len)
{
    if (params == NULL && params_len > 0) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Total parameter count is 1 (ESL_ID) + params_len, and the opcode Length
     * nibble encodes (parameter count - 1), so params_len must fit a nibble */
    if (params_len > 0x0F) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Build the vendor opcode: Tag nibble = 0xF, Length nibble = params_len */
    uint8_t opcode = BLE_ESL_TLV_OPCODE(BLE_ESL_CMD_VENDOR_TAG, params_len);

    /* Build full params: esl_id + vendor params */
    uint8_t full_params[BLE_ESL_TLV_MAX_SIZE];
    uint8_t full_len = 0;

    full_params[full_len++] = esl_id;
    if (params_len > 0) {
        memcpy(&full_params[full_len], params, params_len);
        full_len += params_len;
    }

    return dispatch_command(esl_id, group_id, opcode,
                           full_params, full_len, false);
}

/* ========================== ECP Notification Handler ========================== */

void ble_esl_ap_command_handle_ecp_notification(uint16_t conn_handle,
                                                const uint8_t *data,
                                                uint16_t data_len)
{
    if (s_ecp_ctx_mutex == NULL) {
        return;
    }

    /* Find the connection context */
    ble_esl_ap_conn_t *conn = ble_esl_ap_find_conn(conn_handle);
    if (conn == NULL) {
        ESP_LOGW(TAG, "ECP notification on unknown conn 0x%04X", conn_handle);
        return;
    }

    xSemaphoreTake(s_ecp_ctx_mutex, portMAX_DELAY);

    /* Find the pending ECP command context */
    esl_ap_ecp_cmd_ctx_t *ctx = find_ecp_ctx_by_conn(conn_handle);
    if (ctx == NULL) {
        xSemaphoreGive(s_ecp_ctx_mutex);
        ESP_LOGW(TAG, "ECP notification with no pending command on conn 0x%04X",
                 conn_handle);
        return;
    }

    /* Check if context is still active */
    if (!ctx->in_use) {
        xSemaphoreGive(s_ecp_ctx_mutex);
        return;
    }

    /* Stop the timeout timer */
    if (ctx->timer != NULL) {
        esp_timer_stop(ctx->timer);  /* Ignore error if already stopped */
    }

    /* Capture context data before freeing */
    uint8_t esl_id = ctx->esl_id;
    uint8_t group_id = ctx->group_id;
    uint8_t cmd_opcode = ctx->cmd_opcode;

    /* Clean up the context */
    free_ecp_ctx(ctx);

    xSemaphoreGive(s_ecp_ctx_mutex);

    /* Parse and deliver the response (outside lock) */
    if (data != NULL && data_len > 0) {
        parse_response(esl_id, group_id, data,
                       (uint8_t)(data_len > 255 ? 255 : data_len),
                       cmd_opcode);
    }
}

/* ========================== PAwR Response Handler ========================== */

void ble_esl_ap_command_handle_pawr_response(uint8_t group_id,
                                             uint8_t response_slot,
                                             const uint8_t *data,
                                             uint8_t data_len)
{
    if (g_esl_ap == NULL) {
        return;
    }

    if (data == NULL || data_len == 0) {
        ESP_LOGD(TAG, "Empty PAwR response slot %u in group %u",
                 response_slot, group_id);
        return;
    }

    /*
     * Look up the ESL entry that was assigned to this response slot.
     * The response slot is determined by the position of the last
     * individually-addressed TLV for that ESL in the transmitted payload.
     * We use the slot-to-ESL mapping recorded during PAwR send to find
     * the correct ESL entry.
     */
    uint16_t esl_addr = pawr_lookup_slot(group_id, response_slot);
    ble_esl_ap_esl_entry_t *esl = NULL;
    if (esl_addr != ESL_ADDR_INVALID) {
        esl = ble_esl_ap_find_esl(esl_addr);
    }

    if (esl == NULL) {
        ESP_LOGW(TAG, "PAwR response from unknown ESL: group=%u slot=%u",
                 group_id, response_slot);
        return;
    }

    esl_addr = esl->esl_addr;
    uint8_t esl_id = BLE_ESL_AP_ADDR_ESL_ID(esl_addr);

    /* Retrieve and clear the stored command opcode for response correlation */
    uint8_t cmd_opcode = esl->pending_pawr_cmd_opcode;
    esl->pending_pawr_cmd_opcode = 0;

    /* Decrypt and parse the PAwR response */
    ble_esl_ap_parsed_response_t parsed;
    esp_err_t ret = ble_esl_ap_pawr_parse_response(esl_addr, response_slot,
                                                    data, data_len, &parsed);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "PAwR response parse failed for ESL 0x%04X: 0x%X",
                 esl_addr, ret);
        return;
    }

    /* Parse each response TLV from the decrypted payload */
    if (parsed.payload != NULL && parsed.payload_len > 0) {
        uint8_t offset = 0;
        while (offset < parsed.payload_len) {
            uint8_t remaining = parsed.payload_len - offset;
            if (remaining < BLE_ESL_TLV_MIN_SIZE) {
                break;
            }

            /* Determine TLV size from the opcode at current offset */
            uint8_t resp_opcode = parsed.payload[offset];
            uint8_t tlv_total = BLE_ESL_TLV_TOTAL_LEN(resp_opcode);
            if (tlv_total > remaining) {
                ESP_LOGW(TAG, "Truncated TLV in PAwR response at offset %u", offset);
                break;
            }

            /* Parse this individual response TLV */
            parse_response(esl_id, group_id, &parsed.payload[offset],
                           tlv_total, cmd_opcode);
            offset += tlv_total;
        }
    }
}
