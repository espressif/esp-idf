/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file ble_esl_ap_int.h
 * @brief ESL AP module — Internal header shared across source files
 *
 * Defines internal data structures, per-connection context, per-ESL tracking
 * entry, and cross-module function declarations used by:
 *   - ap_connection.c
 *   - ap_gattc.c
 *   - ap_lifecycle.c
 *   - ap_command.c
 *   - ap_pawr.c
 */

#ifndef BLE_ESL_AP_INT_H
#define BLE_ESL_AP_INT_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_timer.h"
#include "ble_esl_ap.h"
#include "ble_esl_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================== PAwR Response Structures ========================== */

/**
 * @brief Decrypted and parsed PAwR response from a single ESL
 */
typedef struct {
    uint16_t esl_addr;       /*!< Full ESL Address (ESL_ID + Group_ID) */
    uint8_t response_slot;   /*!< Response slot number in which received */
    uint8_t tlv_count;       /*!< Number of response TLVs parsed */
    uint8_t payload_buf[BLE_ESL_PAYLOAD_MAX_SIZE + 2]; /*!< Owned buffer for decrypted inner AD (len + tag + payload); payload points into this at offset 2 */
    uint8_t *payload;        /*!< Pointer to decrypted response payload (TLVs) */
    uint8_t payload_len;     /*!< Length of decrypted response payload */
    bool auth_success;       /*!< True if MIC authenticated successfully */
} ble_esl_ap_parsed_response_t;

/* ========================== Constants ========================== */

/** Invalid/unassigned connection handle sentinel */
#define BLE_ESL_AP_CONN_HANDLE_INVALID  0xFFFF

/** Maximum number of PAwR subevents a periodic advertising train may use
 *  (Core Spec: Num_Subevents range 0x01-0x80) */
#define BLE_ESL_AP_PAWR_MAX_SUBEVENTS   128

/** DIS UUID */
#define BLE_ESL_AP_DIS_UUID             0x180A

/** PnP ID UUID */
#define BLE_ESL_AP_PNP_ID_UUID          0x2A50

/** PnP ID characteristic value length */
#define BLE_ESL_AP_PNP_ID_LEN           7

/** Maximum characteristic value for Read Long reassembly buffer */
#define BLE_ESL_AP_READ_BUF_MAX         256

/*
 * Internally the AP stores an ESL Address packed into a single 16-bit key
 * (ESL_ID | Group_ID << 8) so that tracking entries can be compared and looked
 * up with a plain integer comparison. It carries exactly the same information
 * as the public ble_esl_address_t; use the helpers below to convert between
 * both forms at the public API boundary.
 */

/** ESL Address helper: build 16-bit ESL Address key from esl_id and group_id */
#define BLE_ESL_AP_MAKE_ADDR(esl_id, group_id) \
    ((uint16_t)((uint8_t)(esl_id) | (((uint8_t)(group_id) & 0x7F) << 8)))

/** Extract ESL_ID from 16-bit ESL Address key */
#define BLE_ESL_AP_ADDR_ESL_ID(addr)    ((uint8_t)((addr) & 0xFF))

/** Extract Group_ID from 16-bit ESL Address key */
#define BLE_ESL_AP_ADDR_GROUP_ID(addr)  ((uint8_t)(((addr) >> 8) & 0x7F))

/** Pack a public ble_esl_address_t into the internal 16-bit ESL Address key */
#define BLE_ESL_AP_ADDR_PACK(addr) \
    BLE_ESL_AP_MAKE_ADDR((addr).esl_id, BLE_ESL_ADDR_GROUP_ID(addr))

/**
 * @brief Unpack an internal 16-bit ESL Address key into a ble_esl_address_t
 *
 * @param esl_addr 16-bit ESL Address key (ESL_ID | Group_ID << 8)
 * @return Equivalent public ESL Address structure
 */
static inline ble_esl_address_t ble_esl_ap_addr_unpack(uint16_t esl_addr)
{
    return ble_esl_addr_make(BLE_ESL_AP_ADDR_ESL_ID(esl_addr),
                             BLE_ESL_AP_ADDR_GROUP_ID(esl_addr));
}

/* ========================== Internal Callback Type ========================== */

/**
 * @brief GATT operation completion callback (internal)
 *
 * @param conn_handle ACL connection handle
 * @param status      ESP_OK on success
 * @param data        Read value / ECP notification payload (NULL for writes)
 * @param data_len    Length of data (0 for writes)
 * @param user_data   Opaque pointer from the caller
 */
typedef void (*ble_esl_ap_gatt_cb_t)(uint16_t conn_handle, esp_err_t status,
                                     const uint8_t *data, uint16_t data_len,
                                     void *user_data);

/* ========================== Per-Connection Characteristic Handles ========================== */

/**
 * @brief Discovered characteristic handle cache for one ESL connection
 */
typedef struct {
    /* ESL Service characteristics */
    uint16_t esl_svc_start;      /*!< ESL Service start handle */
    uint16_t esl_svc_end;        /*!< ESL Service end handle */
    uint16_t esl_addr_handle;    /*!< ESL Address (0x2BF6) value handle */
    uint16_t ap_sync_key_handle; /*!< AP Sync Key Material (0x2BF7) value handle */
    uint16_t resp_key_handle;    /*!< ESL Response Key Material (0x2BF8) value handle */
    uint16_t abs_time_handle;    /*!< ESL Current Absolute Time (0x2BF9) value handle */
    uint16_t display_info_handle;/*!< ESL Display Information (0x2BFA) value handle, 0 if absent */
    uint16_t image_info_handle;  /*!< ESL Image Information (0x2BFB) value handle, 0 if absent */
    uint16_t sensor_info_handle; /*!< ESL Sensor Information (0x2BFC) value handle, 0 if absent */
    uint16_t led_info_handle;    /*!< ESL LED Information (0x2BFD) value handle, 0 if absent */
    uint16_t ecp_handle;         /*!< ESL Control Point (0x2BFE) value handle */
    uint16_t ecp_cccd_handle;    /*!< ECP CCCD handle */
    /* DIS characteristics */
    uint16_t dis_svc_start;      /*!< DIS start handle, 0 if absent */
    uint16_t dis_svc_end;        /*!< DIS end handle, 0 if absent */
    uint16_t pnp_id_handle;      /*!< PnP ID (0x2A50) value handle, 0 if absent */
} ble_esl_ap_char_handles_t;

/* ========================== Per-Connection Context ========================== */

/**
 * @brief Per-connection context for a connected ESL
 */
typedef struct {
    bool in_use;                         /*!< Entry is active */
    uint16_t conn_handle;                /*!< ACL connection handle */
    uint8_t addr[6];                     /*!< BLE device address */
    uint8_t addr_type;                   /*!< Address type (0=public, 1=random) */
    ble_esl_ap_char_handles_t handles;   /*!< Discovered characteristic handles */

    /* ECP state */
    bool ecp_pending;                    /*!< ECP write in progress (waiting for notification) */
    ble_esl_ap_gatt_cb_t ecp_cb;         /*!< ECP notification callback */
    void *ecp_user_data;                 /*!< ECP callback user data */

    /* GATT read/write state */
    bool read_pending;                   /*!< A GATT read is in progress */
    ble_esl_ap_gatt_cb_t gatt_cb;        /*!< Active GATT read/write callback */
    void *gatt_user_data;                /*!< GATT callback user data */
    uint8_t read_buf[BLE_ESL_AP_READ_BUF_MAX]; /*!< Read Long reassembly buffer */
    uint16_t read_offset;                /*!< Current offset in read_buf */

    /* OTS capability flag (set during service discovery) */
    bool has_ots;                         /*!< OTS service was found on this connection */
    bool disc_done;                       /*!< Service discovery completed successfully */

    /* ESL tracking cross-reference */
    uint16_t esl_addr;                   /*!< Assigned ESL Address (valid after configure) */
} ble_esl_ap_conn_t;

/* ========================== Per-ESL Tracking Entry ========================== */

/**
 * @brief Per-ESL tracking entry in the AP's ESL tracking table
 */
typedef struct {
    bool in_use;                         /*!< Entry is active */
    uint16_t esl_addr;                   /*!< Assigned ESL Address (ESL_ID | Group_ID << 8) */
    uint8_t ble_addr[6];                 /*!< BLE device address */
    uint8_t ble_addr_type;               /*!< BLE address type */
    ble_esl_state_t state;               /*!< Current tracked state */
    bool config_complete;                /*!< All 4 mandatory chars written successfully */
    int64_t last_sync_time_us;           /*!< Last successful PAwR exchange timestamp (us) */
    int64_t unsync_entry_time_us;        /*!< Time ESL entered Unsynchronized state (us) */
    uint16_t conn_handle;                /*!< Active connection handle (INVALID if not connected) */
    uint8_t pending_pawr_cmd_opcode;     /*!< Last command opcode sent to this ESL via PAwR, for response correlation */

    /* Key material for PAwR response decryption */
    ble_esl_key_material_t resp_key;     /*!< ESL Response Key Material */
} ble_esl_ap_esl_entry_t;

/* ========================== PAwR Pending TX Buffer ========================== */

/**
 * Maximum outer AD structure size for one PAwR subevent:
 *   1 (len) + 1 (ED tag 0x31) + 5 (randomizer) + (2 + 48) (inner AD) + 4 (MIC) = 61
 * Rounded up to a generous buffer.
 */
#define BLE_ESL_AP_PAWR_MAX_AD_BUF_SIZE 80

/**
 * @brief Pending subevent data for the PAwR pull model.
 *
 * PAwR subevent data may only be handed to the controller in response to a
 * BLE_GAP_EVENT_PER_SUBEV_DATA_REQ event. ble_esl_ap_pawr_send() therefore does
 * not push data directly; it encrypts the packet and stores it here, and the
 * data-request callback drains it into the requested subevent. One entry per
 * subevent (indexed by group_id); allocated by ble_esl_ap_pawr_start().
 */
typedef struct {
    uint8_t data[BLE_ESL_AP_PAWR_MAX_AD_BUF_SIZE]; /*!< Encrypted outer AD bytes */
    uint8_t len;                                   /*!< Valid byte count in data (0 = none) */
    uint8_t repeats_left;                          /*!< Remaining PAwR events to transmit */
    bool    valid;                                 /*!< Slot holds a packet awaiting TX */
} ble_esl_ap_pawr_pending_t;

/* ========================== Module Global State ========================== */

/**
 * @brief Top-level AP module state (allocated dynamically by init, freed by deinit)
 */
typedef struct {
    bool initialized;    /*!< Module has been initialized */
    bool started;        /*!< Scanning + PAwR broadcasting active */

    /* Application callback */
    ble_esl_ap_cb_t app_cb;

    /* PAwR configuration */
    ble_esl_ap_pawr_config_t pawr_config;

    /* Connection table */
    ble_esl_ap_conn_t conns[CONFIG_BLE_ESL_AP_MAX_CONNECTIONS];

    /* ESL tracking table */
    ble_esl_ap_esl_entry_t esls[CONFIG_BLE_ESL_AP_MAX_ESLS];

    /* PAwR broadcaster state */
    ble_esl_key_material_t ap_sync_key;            /*!< AP Sync Key Material */
    uint8_t randomizer[BLE_ESL_RANDOMIZER_SIZE];   /*!< Current AP Randomizer (5 octets, LE) */
    bool pawr_active;                              /*!< PAwR broadcaster is running */
    ble_esl_ap_pawr_pending_t *pawr_pending;      /*!< Per-subevent pending TX buffers (num_subevents entries) */

    /* Timeout monitoring timer */
    esp_timer_handle_t timeout_timer;              /*!< Periodic timer for 60-min checks */
} ble_esl_ap_state_t;

/** Global pointer to the AP module state (allocated by init, freed by deinit) */
extern ble_esl_ap_state_t *g_esl_ap;

/* ========================== Helpers ========================== */

/**
 * @brief Find a connection context by connection handle
 *
 * @param conn_handle ACL connection handle
 * @return Pointer to connection context, or NULL if not found
 */
ble_esl_ap_conn_t *ble_esl_ap_find_conn(uint16_t conn_handle);

/**
 * @brief Find an ESL tracking entry by ESL Address
 *
 * @param esl_addr ESL Address (ESL_ID | Group_ID << 8)
 * @return Pointer to ESL entry, or NULL if not found
 */
ble_esl_ap_esl_entry_t *ble_esl_ap_find_esl(uint16_t esl_addr);

/**
 * @brief Find an ESL tracking entry by BLE address
 *
 * @param addr     6-byte BLE address
 * @param addr_type Address type
 * @return Pointer to ESL entry, or NULL if not found
 */
ble_esl_ap_esl_entry_t *ble_esl_ap_find_esl_by_ble_addr(const uint8_t *addr,
                                                         uint8_t addr_type);

/**
 * @brief Allocate a free connection context slot
 *
 * @return Pointer to free slot, or NULL if all slots in use
 */
ble_esl_ap_conn_t *ble_esl_ap_alloc_conn(void);

/**
 * @brief Free a connection context slot
 *
 * @param conn Pointer to connection context
 */
void ble_esl_ap_free_conn(ble_esl_ap_conn_t *conn);

/**
 * @brief Allocate a free ESL tracking entry
 *
 * @return Pointer to free entry, or NULL if table is full
 */
ble_esl_ap_esl_entry_t *ble_esl_ap_alloc_esl(void);

/**
 * @brief Check if a BLE address is in the associated-address list
 *
 * Used during scanning to determine Unassociated vs Unsynchronized.
 *
 * @param addr      6-byte BLE address
 * @param addr_type Address type
 * @return true if the address is tracked (associated), false otherwise
 */
bool ble_esl_ap_is_associated(const uint8_t *addr, uint8_t addr_type);

/* ---- ap_gattc.c ---- */

/**
 * @brief Initialize the GATT client sub-module (OTS client registration)
 *
 * Called from ble_esl_ap_init().
 *
 * @return ESP_OK on success
 */
esp_err_t ble_esl_ap_gattc_init(void);

/**
 * @brief Deinitialize the GATT client sub-module
 *
 * Called from ble_esl_ap_deinit().
 */
void ble_esl_ap_gattc_deinit(void);

/**
 * @brief Start primary service discovery on a newly encrypted connection
 *
 * Triggers the full discovery state machine: services → ESL characteristics →
 * DIS characteristics → ECP CCCD → OTS → disc_complete.
 *
 * @param conn_handle ACL connection handle
 */
void ble_esl_ap_start_svc_discovery(uint16_t conn_handle);

/**
 * @brief Read a GATT characteristic value from a connected ESL
 *
 * Uses Read Long when value > (ATT_MTU − 1). Callback receives full value.
 *
 * @param conn_handle ACL connection handle
 * @param char_uuid   16-bit UUID of the characteristic
 * @param cb          Completion callback
 * @param user_data   Opaque pointer forwarded to cb
 * @return ESP_OK if read initiated; ESP_ERR_NOT_FOUND if char not discovered
 */
esp_err_t ble_esl_ap_gatt_read(uint16_t conn_handle, uint16_t char_uuid,
                               ble_esl_ap_gatt_cb_t cb, void *user_data);

/**
 * @brief Write a GATT characteristic value to a connected ESL
 *
 * Uses GATT Write Characteristic Value sub-procedure.
 *
 * @param conn_handle ACL connection handle
 * @param char_uuid   16-bit UUID of the characteristic
 * @param data        Value to write
 * @param data_len    Length in octets
 * @param cb          Completion callback (data/data_len = NULL/0)
 * @param user_data   Opaque pointer forwarded to cb
 * @return ESP_OK if write initiated; ESP_ERR_NOT_FOUND if char not discovered
 */
esp_err_t ble_esl_ap_gatt_write(uint16_t conn_handle, uint16_t char_uuid,
                                const uint8_t *data, uint16_t data_len,
                                ble_esl_ap_gatt_cb_t cb, void *user_data);

/**
 * @brief Write a TLV command to the ECP characteristic and wait for notification
 *
 * Only one outstanding ECP write per connection. Notification payload
 * delivered via cb(data, data_len). 30s timeout managed by caller.
 *
 * @param conn_handle ACL connection handle
 * @param tlv_data    TLV-encoded command
 * @param tlv_len     TLV data length
 * @param cb          Notification callback
 * @param user_data   Opaque pointer forwarded to cb
 * @return ESP_OK if write initiated; ESP_ERR_INVALID_STATE if ECP busy
 */
esp_err_t ble_esl_ap_ecp_write(uint16_t conn_handle, const uint8_t *tlv_data,
                               uint8_t tlv_len, ble_esl_ap_gatt_cb_t cb,
                               void *user_data);

/* ---- ble_esl_ap_lifecycle.c ---- */

/**
 * @brief Update per-ESL tracked state and fire BLE_ESL_AP_EVT_STATE_CHANGED
 *
 * @param esl_addr  ESL Address (ESL_ID | Group_ID << 8)
 * @param new_state New state to record
 * @return ESP_OK on success; ESP_ERR_NOT_FOUND if ESL not tracked
 */
esp_err_t ble_esl_ap_update_esl_state(uint16_t esl_addr,
                                      ble_esl_state_t new_state);

/**
 * @brief Initialize the lifecycle sub-module (timers, etc.)
 *
 * Called from ble_esl_ap_init().
 *
 * @return ESP_OK on success
 */
esp_err_t ble_esl_ap_lifecycle_init(void);

/**
 * @brief Deinitialize the lifecycle sub-module
 *
 * Called from ble_esl_ap_deinit().
 */
void ble_esl_ap_lifecycle_deinit(void);

/**
 * @brief Handle disconnect event for lifecycle tracking
 *
 * Called by connection module when an ESL disconnects, to complete
 * any pending synchronize procedure and update tracked state.
 *
 * @param conn_handle Connection handle of the disconnected ESL
 */
void ble_esl_ap_lifecycle_handle_disconnect(uint16_t conn_handle);

/**
 * @brief Route OTS client events to lifecycle for image transfer tracking
 *
 * Must only be called from the OTS client callback context. Internally
 * casts @p event to ble_ots_client_event_t.
 *
 * @param conn_id  Connection identifier
 * @param event    OTS client event code (ble_ots_client_event_t)
 * @param param    Event-specific parameter
 */
void ble_esl_ap_lifecycle_handle_ots_event(uint16_t conn_id,
                                           int event,
                                           const void *param);

/* ---- ap_pawr.c ---- */

/**
 * @brief Initialize the PAwR sub-module
 *
 * Called from ble_esl_ap_init().
 *
 * @return ESP_OK on success
 */
esp_err_t ble_esl_ap_pawr_init(void);

/**
 * @brief Deinitialize the PAwR sub-module
 *
 * Called from ble_esl_ap_deinit().
 */
void ble_esl_ap_pawr_deinit(void);

/**
 * @brief Start PAwR broadcasting
 *
 * Configures and enables the PAwR broadcaster using stored pawr_config.
 *
 * @return ESP_OK on success
 */
esp_err_t ble_esl_ap_pawr_start(void);

/**
 * @brief Stop PAwR broadcasting
 *
 * @return ESP_OK on success
 */
esp_err_t ble_esl_ap_pawr_stop(void);

/**
 * @brief Set the AP Sync Key Material used for encrypting sync packets
 *
 * @param key_mat AP Sync Key Material (session key + IV)
 */
void ble_esl_ap_pawr_set_sync_key(const ble_esl_key_material_t *key_mat);

/**
 * @brief Store per-ESL Response Key Material for decrypting responses
 *
 * @param esl_addr ESL Address
 * @param key_mat  ESL Response Key Material (session key + IV)
 * @return ESP_OK on success; ESP_ERR_NOT_FOUND if ESL not tracked
 */
esp_err_t ble_esl_ap_pawr_set_response_key(uint16_t esl_addr,
                                           const ble_esl_key_material_t *key_mat);

/**
 * @brief Assemble and transmit an encrypted PAwR sync packet
 *
 * @param group_id    Target Group_ID (0x00–0x7F)
 * @param payload     Pre-assembled ESL Payload (from ble_esl_payload_encode())
 * @param payload_len ESL Payload length (1–48)
 * @return ESP_OK on success; error code on failure
 */
esp_err_t ble_esl_ap_pawr_send(uint8_t group_id, const uint8_t *payload,
                               uint8_t payload_len);

/**
 * @brief Decrypt and parse a PAwR response packet
 *
 * @param esl_addr     ESL Address of the responding ESL
 * @param response_slot Response slot number
 * @param enc_data     Raw Encrypted Data AD payload
 * @param enc_data_len Length of enc_data
 * @param out_response Output parsed response
 * @return ESP_OK on success; ESP_ERR_INVALID_RESPONSE on MIC failure
 */
esp_err_t ble_esl_ap_pawr_parse_response(uint16_t esl_addr,
                                         uint8_t response_slot,
                                         const uint8_t *enc_data,
                                         uint8_t enc_data_len,
                                         ble_esl_ap_parsed_response_t *out_response);

/* ---- ap_command.c ---- */

/**
 * @brief Initialize the command sub-module
 *
 * Called from ble_esl_ap_init().
 *
 * @return ESP_OK on success
 */
esp_err_t ble_esl_ap_command_init(void);

/**
 * @brief Deinitialize the command sub-module
 *
 * Called from ble_esl_ap_deinit().
 */
void ble_esl_ap_command_deinit(void);

/**
 * @brief Handle an ECP notification received on a connection
 *
 * Called by the connection module when an ECP notification arrives.
 * Routes to the pending ECP callback.
 *
 * @param conn_handle  Connection handle
 * @param data         Notification payload (response TLV)
 * @param data_len     Payload length
 */
void ble_esl_ap_command_handle_ecp_notification(uint16_t conn_handle,
                                                const uint8_t *data,
                                                uint16_t data_len);

/**
 * @brief Clean up any pending ECP command context for a disconnected connection
 *
 * Finds the ECP command context associated with the given connection handle,
 * stops its timer, and frees it. Called from the disconnect handler to prevent
 * stale contexts and timer leaks.
 *
 * @param conn_handle  Connection handle of the disconnected connection
 */
void ble_esl_ap_command_cleanup_conn(uint16_t conn_handle);

/**
 * @brief Handle a PAwR response slot data reception
 *
 * Called by the PAwR module when response data is received in a slot.
 *
 * @param group_id      Group_ID (subevent number)
 * @param response_slot Response slot number
 * @param data          Raw received data (Encrypted Data AD)
 * @param data_len      Data length
 */
void ble_esl_ap_command_handle_pawr_response(uint8_t group_id,
                                             uint8_t response_slot,
                                             const uint8_t *data,
                                             uint8_t data_len);

#ifdef __cplusplus
}
#endif

#endif /* BLE_ESL_AP_INT_H */
