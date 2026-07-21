/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file ble_esl_int.h
 * @brief ESL component internal header — shared interfaces across source files
 *
 * Not part of the public API. Groups internal declarations by source file.
 */

#ifndef BLE_ESL_INT_H
#define BLE_ESL_INT_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "ble_esl.h"
#include "ble_esl_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================== Constants ========================== */

/** @brief Sync/Unsync state timeout: 60 minutes in microseconds (for esp_timer) */
#define ESL_STATE_TIMEOUT_US            (60ULL * 60ULL * 1000000ULL)

/** @brief ECP Procedure Timeout in microseconds */
#define ESL_ECP_TIMEOUT_US              (30ULL * 1000000ULL)

/** @brief Maximum number of TLVs in a single ESL Payload */
#define ESL_MAX_TLVS_PER_PAYLOAD        24

/** @brief Sensor transport context: ECP */
#define ESL_TRANSPORT_ECP               0

/** @brief Sensor transport context: PAwR */
#define ESL_TRANSPORT_PAWR              1

/* ---- esl_state.c ---- */

/**
 * @brief Return the active ACL connection handle
 * @return Connection handle, or BLE_HS_CONN_HANDLE_NONE if not connected
 */
uint16_t esl_state_get_conn_handle(void);

/**
 * @brief Check whether the ESL is synchronized to the AP's PAwR train
 *
 * Reflects the actual synchronization condition rather than the state machine
 * state: the sync is established on entry to Synchronized and retained while in
 * the Updating state (where PAwR data is ignored but sync is not lost). It is
 * cleared on Unsynchronized, Configuring and Unassociated.
 *
 * @return true if synchronized to the AP
 */
bool esl_is_pawr_synced(void);

/**
 * @brief Execute a state transition with associated side-effects
 *
 * Validates legality, changes GAP mode, arms/disarms timeouts, deletes data
 * on unassociate/factory-reset, and fires BLE_ESL_EVT_STATE_CHANGED callback.
 *
 * @param[in] new_state Target state
 * @return ESP_OK on success; ESP_ERR_INVALID_STATE if transition not permitted
 */
esp_err_t esl_state_transition(ble_esl_state_t new_state);

/**
 * @brief Get the current ESL absolute time in milliseconds
 *
 * The time counter is set via the ESL Current Absolute Time characteristic
 * and increments by 1 every millisecond, wrapping at 0xFFFFFFFF.
 *
 * @return Current absolute time in milliseconds
 */
uint32_t esl_get_abs_time(void);

/**
 * @brief Fire an ESL event to the application callback
 *
 * @param[in] event Event type
 * @param[in] param Event data (may be NULL)
 */
void esl_notify_app(ble_esl_event_t event, ble_esl_cb_param_t *param);

/**
 * @brief Get the ESL's configured address
 *
 * @param[out] esl_id  ESL_ID (0x00–0xFE)
 * @param[out] group_id Group_ID (7-bit)
 * @return ESP_OK if address is configured; ESP_ERR_INVALID_STATE if not
 */
esp_err_t esl_get_address(uint8_t *esl_id, uint8_t *group_id);

/**
 * @brief Get the ESL's stored configuration
 *
 * @return Pointer to the stored ble_esl_config_t (valid while initialized)
 */
const ble_esl_config_t *esl_get_config(void);

/**
 * @brief Get the stored AP Sync Key Material
 *
 * @return Pointer to 24-byte key material, or NULL if not configured
 */
const ble_esl_key_material_t *esl_get_ap_sync_key(void);

/**
 * @brief Get the stored ESL Response Key Material
 *
 * @return Pointer to 24-byte key material, or NULL if not configured
 */
const ble_esl_key_material_t *esl_get_resp_key(void);

/**
 * @brief Clear all stored configuration data (keys, address, timed commands)
 *
 * Called during unassociate, factory reset, and unsync timeout transitions.
 *
 * @param[in] clear_images If true, also clear stored image data (factory reset only)
 */
void esl_clear_stored_data(bool clear_images);

/**
 * @brief Notify the state machine that the Update Complete command was received
 *
 * In the Updating state, both PAST and Update Complete are required before
 * transitioning to Synchronized. This function records that Update Complete
 * has been received and triggers the transition if PAST was already received.
 */
void esl_notify_update_complete(void);

/* ---- esl_gatts.c ---- */
/**
 * @brief Send one response TLV as an ECP characteristic notification
 *
 * @param[in] response_tlv Pointer to the encoded response TLV buffer
 * @param[in] response_len Length of the response TLV in bytes
 * @return ESP_OK on success; ESP_FAIL if notification could not be sent
 */
esp_err_t esl_send_ecp_response(const uint8_t *response_tlv, uint8_t response_len);

/**
 * @brief Handle completion of an ECP notification transmission
 *
 * Used by the GAP event handler to run any command cleanup that must happen
 * after the ECP response notification has actually been transmitted.
 */
void esl_handle_ecp_notify_tx(uint16_t conn_handle, uint16_t attr_handle,
                              int status, uint8_t indication);

/* ---- esl_cmd_lifecycle.c ---- */

/**
 * @brief Result of command dispatch
 */
typedef struct {
    bool has_response;                          /*!< True if command produces a response TLV */
    uint8_t resp_opcode;                        /*!< Response opcode byte */
    uint8_t resp_params[BLE_ESL_TLV_MAX_SIZE];  /*!< Response parameter bytes */
    uint8_t resp_params_len;                    /*!< Length of resp_params in octets */
} ble_esl_cmd_result_t;

/**
 * @brief Central command dispatch function
 *
 * Validates ESL_ID, checks for unrecognized opcodes, and routes to the
 * appropriate per-opcode handler. Used by both ECP and PAwR paths.
 *
 * @param[in]  opcode     Command opcode byte
 * @param[in]  params     Pointer to command parameter bytes (first byte is ESL_ID)
 * @param[in]  params_len Length of params in octets
 * @param[out] result     Output: populated with the response
 * @param[in]  transport  Transport context: ESL_TRANSPORT_ECP or ESL_TRANSPORT_PAWR
 * @return ESP_OK if the command was processed — including rejections that carry
 *         an Error response in @p result; ESP_ERR_INVALID_STATE if the module is
 *         not initialized
 */
esp_err_t esl_cmd_dispatch(uint8_t opcode, const uint8_t *params,
                           uint8_t params_len, ble_esl_cmd_result_t *result,
                           uint8_t transport);

/**
 * @brief Assemble the 16-bit Basic State response bitmap
 *
 * Queries Service Needed, Synchronized, Active LED, Pending LED Update,
 * and Pending Display Update flags from respective modules.
 *
 * @param[out] bitmap_out The 16-bit Basic State bitmap
 * @return ESP_OK on success
 */
esp_err_t esl_build_basic_state(uint16_t *bitmap_out);

/**
 * @brief Check if factory reset is pending
 * @return true if factory reset guard flag is set
 */
bool esl_is_factory_reset_pending(void);

/**
 * @brief Clear the factory reset pending flag
 *
 * Called after factory reset cleanup is complete (e.g., after clearing stored
 * data and transitioning to Unassociated state in the disconnect handler).
 */
void esl_clear_factory_reset_pending(void);

/**
 * @brief Initialize the lifecycle command module
 * @return ESP_OK on success
 */
esp_err_t esl_cmd_lifecycle_init(void);

/**
 * @brief Deinitialize the lifecycle command module
 */
void esl_cmd_lifecycle_deinit(void);

/* ---- esl_cmd_display.c ---- */

/**
 * @brief Handle Read Sensor Data command (opcode 0x10)
 *
 * @param[in]  params     Command parameters (after opcode)
 * @param[in]  params_len Length of params
 * @param[out] result     Command result
 * @param[in]  transport  ESL_TRANSPORT_ECP or ESL_TRANSPORT_PAWR
 * @return ESP_OK on success
 */
esp_err_t esl_cmd_read_sensor(const uint8_t *params, uint8_t params_len,
                              ble_esl_cmd_result_t *result, uint8_t transport);

/**
 * @brief Handle Refresh Display command (opcode 0x11)
 *
 * @param[in]  params     Command parameters
 * @param[in]  params_len Length of params
 * @param[out] result     Command result
 * @return ESP_OK on success
 */
esp_err_t esl_cmd_refresh_display(const uint8_t *params, uint8_t params_len,
                                  ble_esl_cmd_result_t *result);

/**
 * @brief Handle Display Image command (opcode 0x20)
 *
 * @param[in]  params     Command parameters
 * @param[in]  params_len Length of params
 * @param[out] result     Command result
 * @return ESP_OK on success
 */
esp_err_t esl_cmd_display_image(const uint8_t *params, uint8_t params_len,
                                ble_esl_cmd_result_t *result);

/**
 * @brief Handle Display Timed Image command (opcode 0x60)
 *
 * @param[in]  params     Command parameters
 * @param[in]  params_len Length of params
 * @param[out] result     Command result
 * @return ESP_OK on success
 */
esp_err_t esl_cmd_display_timed_image(const uint8_t *params, uint8_t params_len,
                                      ble_esl_cmd_result_t *result);

/**
 * @brief Get the Pending Display Update flag for Basic State bitmap
 * @return true if at least one display has a pending timed image command
 */
bool esl_get_display_pending(void);

/**
 * @brief Initialize the display/sensor command module
 * @return ESP_OK on success
 */
esp_err_t esl_cmd_display_init(void);

/**
 * @brief Deinitialize the display/sensor command module
 */
void esl_cmd_display_deinit(void);

/**
 * @brief Cancel all pending timed display commands
 *
 * Called during unassociate / factory reset.
 */
void esl_cmd_display_cancel_all(void);

/* ---- esl_cmd_led.c ---- */

/**
 * @brief LED status flags for Basic State bitmap
 */
typedef struct {
    bool active_led;                    /*!< True if any LED is currently active */
    bool pending_led_update;            /*!< True if any LED timed command is pending */
} esl_led_status_t;

/**
 * @brief Handle LED Control command (opcode 0xB0)
 *
 * @param[in]  params     Command parameters
 * @param[in]  params_len Length of params
 * @param[out] result     Command result
 * @return ESP_OK on success
 */
esp_err_t esl_cmd_led_control(const uint8_t *params, uint8_t params_len,
                              ble_esl_cmd_result_t *result);

/**
 * @brief Handle LED Timed Control command (opcode 0xF0)
 *
 * @param[in]  params     Command parameters
 * @param[in]  params_len Length of params
 * @param[out] result     Command result
 * @return ESP_OK on success
 */
esp_err_t esl_cmd_led_timed_control(const uint8_t *params, uint8_t params_len,
                                    ble_esl_cmd_result_t *result);

/**
 * @brief Handle Vendor-specific command (opcodes 0x_F)
 *
 * @param[in]  opcode     Full opcode byte
 * @param[in]  params     Command parameters
 * @param[in]  params_len Length of params
 * @param[out] result     Command result
 * @return ESP_OK on success
 */
esp_err_t esl_cmd_vendor_specific(uint8_t opcode, const uint8_t *params,
                                  uint8_t params_len, ble_esl_cmd_result_t *result);

/**
 * @brief Get Active LED and Pending LED Update flags
 * @return esl_led_status_t with current flags
 */
esl_led_status_t esl_get_led_status(void);

/**
 * @brief Initialize the LED command module
 * @return ESP_OK on success
 */
esp_err_t esl_cmd_led_init(void);

/**
 * @brief Deinitialize the LED command module
 */
void esl_cmd_led_deinit(void);

/**
 * @brief Cancel all pending timed LED commands
 *
 * Called during unassociate / factory reset.
 */
void esl_cmd_led_cancel_all(void);

/* ---- esl_pawr.c ---- */

/**
 * @brief Slot info tracked during PAwR packet processing
 */
typedef struct {
    uint8_t slot_number;    /*!< PAwR response slot */
    uint8_t tlv_count;      /*!< Number of individually-addressed TLVs */
    uint8_t broadcast_count; /*!< Number of broadcast TLVs */
} esl_pawr_slot_info_t;

/**
 * @brief Process a received PAwR sync packet
 *
 * Strips the inner AD wrapper ([len][0x34]), decodes the ESL Payload,
 * validates Group_ID, dispatches commands, calculates response slot,
 * assembles and encrypts response, and transmits.
 *
 * @param[in] subevent PAwR subevent number
 * @param[in] data     Pointer to the decrypted plaintext, i.e. the inner AD
 *                     structure [inner_len][ESL Tag 0x34][ESL Payload]
 * @param[in] data_len Length of the decrypted plaintext
 * @return ESP_OK on success, error code on failure
 */
esp_err_t esl_pawr_process_sync_packet(uint8_t subevent, const uint8_t *data,
                                       uint8_t data_len);

/**
 * @brief Calculate the response slot for this ESL
 *
 * @param[in] data        Pointer to the decrypted ESL Payload buffer
 * @param[in] tlv_offsets Array of offsets for each command TLV
 * @param[in] tlv_lens    Array of lengths for each command TLV
 * @param[in] tlv_count   Total number of command TLVs
 * @param[in] esl_id      This ESL's ESL_ID
 * @return Response slot number (>= 0), or -1 if not individually addressed
 */
int16_t esl_pawr_calc_response_slot(const uint8_t *data,
                                    const uint8_t *tlv_offsets,
                                    const uint8_t *tlv_lens,
                                    uint8_t tlv_count, uint8_t esl_id);

/**
 * @brief Assemble response TLVs into a single ESL Payload
 *
 * @param[in]  resp_tlvs     Array of pointers to encoded response TLV buffers
 * @param[in]  resp_tlv_lens Array of response TLV lengths
 * @param[in]  resp_count    Number of response TLVs
 * @param[out] out_buf       Output buffer (min 48 octets)
 * @param[out] out_len       Total payload size written
 * @return ESP_OK on success; ESP_ERR_INVALID_SIZE if payload exceeds 48 octets
 */
esp_err_t esl_pawr_assemble_response(const uint8_t *resp_tlvs[],
                                     const uint8_t resp_tlv_lens[],
                                     uint8_t resp_count,
                                     uint8_t *out_buf, uint8_t *out_len);

/**
 * @brief Encrypt and send response in the assigned PAwR response slot
 *
 * @param[in] slot        Response slot number
 * @param[in] payload     Assembled response ESL Payload
 * @param[in] payload_len Length of the payload
 * @return ESP_OK on success, error code on failure
 */
esp_err_t esl_pawr_encrypt_and_send(uint8_t slot, const uint8_t *payload,
                                    uint8_t payload_len);

/**
 * @brief Check if an Unassociate post-response cleanup is pending
 * @return true if Unassociate cleanup needs to run after ECP response sent
 */
bool esl_is_unassociate_pending(void);

/**
 * @brief Execute deferred Unassociate cleanup after ECP response is sent
 *
 * Clears stored data, transitions to Unassociated, fires UNASSOCIATE event.
 */
void esl_execute_unassociate_cleanup(void);

/**
 * @brief Set PAwR sync context before processing a sync packet
 *
 * Must be called from the PAwR sync-received event handler before
 * calling esl_pawr_process_sync_packet().
 *
 * @param[in] sync_handle PAwR sync handle
 * @param[in] event_counter PAwR event counter
 */
void esl_pawr_set_sync_context(uint16_t sync_handle, uint16_t event_counter);

/**
 * @brief Initialize the PAwR module
 * @return ESP_OK on success
 */
esp_err_t esl_pawr_init(void);

/**
 * @brief Deinitialize the PAwR module
 */
void esl_pawr_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* BLE_ESL_INT_H */
