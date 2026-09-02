/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file ble_esl_ap.h
 * @brief Electronic Shelf Label (ESL) Profile — Access Point (AP) Public API
 *
 * Implements the GATT Client / GAP Central role of the ESL Profile v1.0.1.
 * Manages ESL scanning, connection, configuration, PAwR broadcasting,
 * command dispatch, and per-ESL lifecycle state tracking.
 */

#ifndef BLE_ESL_AP_H
#define BLE_ESL_AP_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "ble_esl_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================== Kconfig Defaults ========================== */

#ifndef CONFIG_BLE_ESL_AP_MAX_CONNECTIONS
#define CONFIG_BLE_ESL_AP_MAX_CONNECTIONS    8   /*!< Max simultaneous ACL connections */
#endif

#ifndef CONFIG_BLE_ESL_AP_MAX_ESLS
#define CONFIG_BLE_ESL_AP_MAX_ESLS           100 /*!< Max tracked ESLs across all states */
#endif

/* ========================== Event Types ========================== */

/**
 * @brief AP event types delivered through the application callback
 */
typedef enum {
    /* Connection events (ap_connection.c) */
    BLE_ESL_AP_EVT_SCAN_RESULT = 0,    /*!< ESL discovered during scanning */
    BLE_ESL_AP_EVT_CONNECTED,          /*!< ACL connected + service discovery done */
    BLE_ESL_AP_EVT_DISCONNECTED,       /*!< ACL disconnected or connection failed */
    /* Lifecycle events (ap_lifecycle.c) */
    BLE_ESL_AP_EVT_ESL_INFO,          /*!< ESL info characteristics read complete */
    BLE_ESL_AP_EVT_CONFIGURED,        /*!< ESL configuration write complete */
    BLE_ESL_AP_EVT_IMAGE_TRANSFERRED, /*!< Image transfer via OTS complete */
    BLE_ESL_AP_EVT_SYNCHRONIZED,      /*!< ESL transitioned to Synchronized state */
    BLE_ESL_AP_EVT_STATE_CHANGED,     /*!< ESL tracked state changed */
    /* Command events (ap_command.c) */
    BLE_ESL_AP_EVT_RESPONSE,          /*!< Command response received from ESL */
    BLE_ESL_AP_EVT_CMD_TIMEOUT,       /*!< ECP procedure timeout (30 seconds) */
} ble_esl_ap_evt_t;

/* ========================== Response Types ========================== */

/**
 * @brief ESL response type identifiers
 */
typedef enum {
    ESL_AP_RESP_ERROR = 0,        /*!< Error Response (opcode 0x00) */
    ESL_AP_RESP_LED_STATE,        /*!< LED State Response (opcode 0x01) */
    ESL_AP_RESP_BASIC_STATE,      /*!< Basic State Response (opcode 0x10) */
    ESL_AP_RESP_DISPLAY_STATE,    /*!< Display State Response (opcode 0x11) */
    ESL_AP_RESP_SENSOR_VALUE,     /*!< Sensor Value Response (opcode 0x_E) */
    ESL_AP_RESP_VENDOR,           /*!< Vendor-specific Response (opcode 0x_F) */
} esl_ap_response_type_t;

/* ========================== Callback Type ========================== */

/**
 * @brief Application event callback type
 *
 * All AP events are delivered through a single callback. The application
 * casts @p param to the appropriate struct based on @p event.
 *
 * @param event Event type from ble_esl_ap_evt_t
 * @param param Pointer to event-specific data structure
 */
typedef void (*ble_esl_ap_cb_t)(ble_esl_ap_evt_t event, void *param);

/* ========================== Configuration Structures ========================== */

/**
 * @brief PAwR timing configuration
 */
typedef struct {
    uint16_t periodic_adv_interval_min; /*!< Min PA interval (units: 1.25 ms) */
    uint16_t periodic_adv_interval_max; /*!< Max PA interval (units: 1.25 ms) */
    uint8_t  num_subevents;             /*!< Subevents per interval (1–128) */
    uint8_t  subevent_interval;         /*!< Interval between subevents (units: 1.25 ms) */
    uint8_t  response_slot_delay;       /*!< Delay to first response slot (units: 1.25 ms) */
    uint8_t  response_slot_spacing;     /*!< Response slot spacing (units: 0.125 ms) */
    uint8_t  num_response_slots;        /*!< Response slots per subevent (1–255) */
} ble_esl_ap_pawr_config_t;

/**
 * @brief AP initialization configuration
 */
typedef struct {
    ble_esl_ap_cb_t callback;             /*!< Application event callback (must not be NULL) */
    ble_esl_ap_pawr_config_t pawr_config; /*!< PAwR timing parameters */
} ble_esl_ap_config_t;

/* ========================== Connection Event Structures ========================== */

/**
 * @brief Scan result event data (BLE_ESL_AP_EVT_SCAN_RESULT)
 */
typedef struct {
    uint8_t addr[6];            /*!< BLE address (little-endian) */
    uint8_t addr_type;          /*!< Address type (0 = public, 1 = random) */
    int8_t rssi;                /*!< Received signal strength (dBm) */
    const uint8_t *adv_data;    /*!< Raw advertising data (valid during callback) */
    uint16_t adv_data_len;      /*!< Advertising data length in octets */
    bool is_associated;         /*!< true = Unsynchronized (known), false = Unassociated */
} ble_esl_ap_scan_result_t;

/**
 * @brief Connected event data (BLE_ESL_AP_EVT_CONNECTED)
 *
 * Populated after service and characteristic discovery completes.
 */
typedef struct {
    uint16_t conn_handle;    /*!< ACL connection handle */
    uint8_t addr[6];         /*!< BLE address of connected ESL */
    uint8_t addr_type;       /*!< Address type */
    bool has_display_info;   /*!< ESL Display Information (0x2BFA) discovered */
    bool has_image_info;     /*!< ESL Image Information (0x2BFB) discovered */
    bool has_sensor_info;    /*!< ESL Sensor Information (0x2BFC) discovered */
    bool has_led_info;       /*!< ESL LED Information (0x2BFD) discovered */
    bool has_dis;            /*!< Device Information Service discovered */
    bool has_pnp_id;         /*!< PnP ID (0x2A50) discovered within DIS */
} ble_esl_ap_conn_info_t;

/**
 * @brief Disconnected event data (BLE_ESL_AP_EVT_DISCONNECTED)
 */
typedef struct {
    uint16_t conn_handle;   /*!< ACL connection handle */
    uint8_t addr[6];        /*!< BLE address of the ESL */
    uint8_t reason;         /*!< HCI disconnection reason code */
} ble_esl_ap_disconn_info_t;

/* ========================== Lifecycle Structures ========================== */

/**
 * @brief ESL configuration parameters for ble_esl_ap_configure()
 */
typedef struct {
    uint8_t esl_id;                       /*!< ESL_ID to assign (0x00–0xFE) */
    uint8_t group_id;                     /*!< Group_ID to assign (0x00–0x7F) */
    ble_esl_key_material_t ap_sync_key;   /*!< AP Sync Key Material (session key + IV) */
    ble_esl_key_material_t resp_key;      /*!< ESL Response Key Material (session key + IV) */
} ble_esl_ap_esl_config_t;

/**
 * @brief ESL info event data (BLE_ESL_AP_EVT_ESL_INFO)
 *
 * Populated by ble_esl_ap_read_info(). All pointers are valid only during the
 * callback invocation.
 */
typedef struct {
    uint16_t conn_handle;                   /*!< Connection handle */
    esp_err_t status;                       /*!< Overall read status */
    uint8_t num_displays;                   /*!< Number of displays (0 if char absent) */
    const ble_esl_display_info_t *displays; /*!< Display info array (5 octets each) */
    uint8_t max_image_index;                /*!< Max Image_Index (0xFF if char absent) */
    uint8_t num_sensors;                    /*!< Number of sensors (0 if char absent) */
    const uint8_t *sensors;                 /*!< Raw sensor info data */
    uint16_t sensors_len;                   /*!< Sensor info buffer length */
    uint8_t num_leds;                       /*!< Number of LEDs (0 if char absent) */
    const uint8_t *leds;                    /*!< LED info octets (1 per LED) */
    bool has_pnp_id;                        /*!< PnP ID was successfully read */
    uint8_t pnp_id[7];                     /*!< PnP ID value (valid if has_pnp_id) */
} ble_esl_ap_esl_info_t;

/**
 * @brief Image transfer parameters for ble_esl_ap_transfer_image()
 */
typedef struct {
    uint16_t conn_handle;       /*!< Connection handle */
    uint8_t image_index;        /*!< Target Image_Index on the ESL */
    const uint8_t *data;        /*!< Pointer to image data buffer */
    uint32_t data_len;          /*!< Image data length in octets */
    bool truncate;              /*!< If true, set Truncate bit in OTS Mode */
} ble_esl_ap_image_transfer_params_t;

/**
 * @brief Configured event data (BLE_ESL_AP_EVT_CONFIGURED)
 */
typedef struct {
    uint16_t conn_handle;   /*!< Connection handle */
    esp_err_t status;       /*!< ESP_OK if all mandatory chars written */
} ble_esl_ap_configured_t;

/**
 * @brief Image transferred event data (BLE_ESL_AP_EVT_IMAGE_TRANSFERRED)
 */
typedef struct {
    uint16_t conn_handle;   /*!< Connection handle */
    uint8_t image_index;    /*!< Image index that was transferred */
    esp_err_t status;       /*!< ESP_OK on success */
} ble_esl_ap_image_transferred_t;

/**
 * @brief Synchronized event data (BLE_ESL_AP_EVT_SYNCHRONIZED)
 */
typedef struct {
    ble_esl_address_t esl_addr; /*!< ESL Address (ESL_ID + Group_ID) */
    esp_err_t status;           /*!< ESP_OK on success */
} ble_esl_ap_synchronized_t;

/**
 * @brief State changed event data (BLE_ESL_AP_EVT_STATE_CHANGED)
 */
typedef struct {
    uint16_t conn_handle;               /*!< Connection handle (0xFFFF if disconnected) */
    ble_esl_address_t esl_addr;         /*!< ESL Address (ESL_ID + Group_ID) */
    ble_esl_state_t old_state;          /*!< Previous state */
    ble_esl_state_t new_state;          /*!< New state */
} ble_esl_ap_state_changed_t;

/* ========================== Response Data Structures ========================== */

/**
 * @brief Parsed 16-bit Basic State bitmap
 */
typedef struct {
    bool service_needed;          /*!< Bit 0: ESL needs service */
    bool synchronized;            /*!< Bit 1: ESL is synchronized to AP */
    bool active_led;              /*!< Bit 2: One or more LEDs active */
    bool pending_led_update;      /*!< Bit 3: Timed LED command pending */
    bool pending_display_update;  /*!< Bit 4: Timed display command pending */
} esl_ap_basic_state_t;

/**
 * @brief Error response info
 */
typedef struct {
    uint8_t error_code;   /*!< ESL error code from BLE_ESL_ERR_* */
} esl_ap_error_info_t;

/**
 * @brief LED state response info
 */
typedef struct {
    uint8_t led_index;    /*!< Echo of the commanded LED_Index */
} esl_ap_led_state_info_t;

/**
 * @brief Display state response info
 */
typedef struct {
    uint8_t display_index; /*!< Echo of commanded Display_Index */
    uint8_t image_index;   /*!< Image currently shown on the display */
} esl_ap_display_state_info_t;

/**
 * @brief Sensor value response info
 */
typedef struct {
    uint8_t sensor_index;     /*!< Echo of commanded Sensor_Index */
    const uint8_t *data;      /*!< Raw sensor data (valid during callback) */
    uint8_t data_len;         /*!< Sensor data length (0–15 octets) */
} esl_ap_sensor_value_info_t;

/**
 * @brief Vendor-specific response info
 */
typedef struct {
    const uint8_t *data;  /*!< Vendor data (valid during callback) */
    uint8_t data_len;     /*!< Data length (1–16 octets) */
} esl_ap_vendor_response_info_t;

/**
 * @brief Command response delivered via BLE_ESL_AP_EVT_RESPONSE
 */
typedef struct {
    uint8_t esl_id;                /*!< Responding ESL_ID (0x00–0xFE) */
    uint8_t group_id;              /*!< Responding Group_ID (0x00–0x7F) */
    esl_ap_response_type_t type;   /*!< Response type selector */
    union {
        esl_ap_error_info_t error;                 /*!< Valid when type == ESL_AP_RESP_ERROR */
        esl_ap_led_state_info_t led_state;         /*!< Valid when type == ESL_AP_RESP_LED_STATE */
        esl_ap_basic_state_t basic_state;          /*!< Valid when type == ESL_AP_RESP_BASIC_STATE */
        esl_ap_display_state_info_t display_state; /*!< Valid when type == ESL_AP_RESP_DISPLAY_STATE */
        esl_ap_sensor_value_info_t sensor_value;   /*!< Valid when type == ESL_AP_RESP_SENSOR_VALUE */
        esl_ap_vendor_response_info_t vendor;      /*!< Valid when type == ESL_AP_RESP_VENDOR */
    };
} esl_ap_response_t;

/**
 * @brief Command timeout event data (BLE_ESL_AP_EVT_CMD_TIMEOUT)
 */
typedef struct {
    uint8_t esl_id;    /*!< ESL_ID of the timed-out ESL */
    uint8_t group_id;  /*!< Group_ID of the timed-out ESL */
} esl_ap_cmd_timeout_t;

/* ========================== Command Parameter Structures ========================== */

/**
 * @brief LED control settings for ble_esl_ap_led_control() and ble_esl_ap_led_timed_control()
 */
typedef struct {
    uint8_t color_red;           /*!< 2-bit red component (0–3) */
    uint8_t color_green;         /*!< 2-bit green component (0–3) */
    uint8_t color_blue;          /*!< 2-bit blue component (0–3) */
    uint8_t brightness;          /*!< 2-bit brightness (0=25%, 1=50%, 2=75%, 3=100%) */
    uint8_t flashing_pattern[7]; /*!< 56-bit (7-byte) flashing pattern */
    uint8_t repeat_type;         /*!< 0 = count-based, 1 = duration-based */
    uint16_t repeats_duration;   /*!< 15-bit repeat count or duration (seconds) */
} esl_ap_led_settings_t;

/* ========================== Public APIs: Connection ========================== */
/* Implemented in ble_esl_ap_connection.c                                       */

/**
 * @brief Initialize the AP module
 *
 * Registers the application callback, stores PAwR timing parameters,
 * and allocates internal resources. Must be called before any other API.
 *
 * @param config Pointer to AP configuration (callback + PAwR timing)
 * @return ESP_OK on success; ESP_ERR_INVALID_ARG if config/callback is NULL or
 *         num_subevents is outside 1–128;
 *         ESP_ERR_INVALID_STATE if already initialized
 */
esp_err_t ble_esl_ap_init(const ble_esl_ap_config_t *config);

/**
 * @brief Deinitialize the AP module
 *
 * Disconnects all ESLs, stops scanning/PAwR, and frees all resources.
 *
 * @note If a connection cannot be terminated, scanning and PAwR are left
 *       stopped, the still connected links stay tracked and ESP_FAIL is
 *       returned; the caller may retry this function.
 *
 * @return ESP_OK on success; ESP_ERR_INVALID_STATE if not initialized;
 *         ESP_FAIL if an active connection could not be terminated
 */
esp_err_t ble_esl_ap_deinit(void);

/**
 * @brief Start AP operation (scanning + PAwR broadcasting)
 *
 * Begins GAP General Discovery for ESLs and starts PAwR broadcasting.
 * Discovered ESLs are reported via BLE_ESL_AP_EVT_SCAN_RESULT.
 *
 * @note This function does not automatically initiate connections. The caller must handle
 *       scan results and call ble_esl_ap_connect() to establish connections.
 *
 * @return ESP_OK on success; ESP_ERR_INVALID_STATE if not initialized or already started
 */
esp_err_t ble_esl_ap_start(void);

/**
 * @brief Stop AP operation (scanning + PAwR broadcasting)
 *
 * Existing ACL connections are not affected.
 *
 * @return ESP_OK on success; ESP_ERR_INVALID_STATE if not started
 */
esp_err_t ble_esl_ap_stop(void);

/**
 * @brief Initiate ACL connection to an ESL
 *
 * Asynchronous — returns immediately.
 * Result is delivered via BLE_ESL_AP_EVT_CONNECTED on success or
 * BLE_ESL_AP_EVT_DISCONNECTED on any failure along the way.
 *
 * @note This function performs the following steps automatically in sequence:
 *  1. Initiated ACL connection with peer
 *  2. Initiated SMP pairing and encryption
 *  3. Discovery service, characteristic and descriptor (ESL Service, optional DIS, OTS)
 *  4. Subscribes to indications or notifications
 *
 * @param addr     6-byte BLE address of the target ESL
 * @param addr_type Address type (0 = public, 1 = random)
 * @return ESP_OK if connection initiation accepted; ESP_ERR_INVALID_ARG if
 *         addr NULL; ESP_ERR_NO_MEM if max connections reached
 */
esp_err_t ble_esl_ap_connect(const uint8_t *addr, uint8_t addr_type);

/**
 * @brief Connect to a Synchronized ESL through the PAwR train
 *
 * Implements the Periodic Advertising Connection procedure (Core [Vol 3],
 * Part C, §9.5.5): the AP sends an AUX_CONNECT_REQ in the ESL's PAwR subevent
 * (== its Group_ID) to pull an already-Synchronized ESL — which is not
 * advertising — into an ACL connection without waiting for it to become
 * connectable. Encryption is restored from the bond's stored LTK, after which
 * the ESL enters the Updating state.
 *
 * Use this to move an ESL from Synchronized to Updating (e.g. for image
 * transfer or reconfiguration). The ESL must be tracked and currently in the
 * Synchronized state. Asynchronous — result delivered via
 * BLE_ESL_AP_EVT_CONNECTED on success (after service discovery) or
 * BLE_ESL_AP_EVT_DISCONNECTED on failure, mirroring ble_esl_ap_connect().
 *
 * @note Requires controller + host support for PAwR connection establishment
 *       (CONFIG_BT_NIMBLE_PERIODIC_ADV_WITH_RESPONSES).
 *
 * @param esl_addr ESL Address of the target ESL (see ble_esl_addr_make())
 * @return ESP_OK if the connection was initiated; ESP_ERR_NOT_FOUND if the ESL
 *         is not tracked; ESP_ERR_INVALID_STATE if it is not Synchronized;
 *         ESP_ERR_NO_MEM if the connection table is full
 */
esp_err_t ble_esl_ap_connect_synced(ble_esl_address_t esl_addr);

/**
 * @brief Disconnect from a connected ESL
 *
 * Asynchronous — BLE_ESL_AP_EVT_DISCONNECTED delivered on completion.
 *
 * @param conn_handle ACL connection handle
 * @return ESP_OK if disconnect initiated; ESP_ERR_NOT_FOUND if handle unknown
 */
esp_err_t ble_esl_ap_disconnect(uint16_t conn_handle);

/* ========================== Public APIs: Lifecycle ========================== */
/* Implemented in ble_esl_ap_lifecycle.c                                       */

/**
 * @brief Write ESL configuration characteristics
 *
 * Writes ESL Address, AP Sync Key Material, ESL Response Key Material, and
 * ESL Current Absolute Time in sequence. Async — result via
 * BLE_ESL_AP_EVT_CONFIGURED.
 *
 * @param conn_handle Connection handle of the connected ESL
 * @param config      Pointer to configuration parameters
 * @return ESP_OK if operation initiated; error code on failure
 */
esp_err_t ble_esl_ap_configure(uint16_t conn_handle,
                               const ble_esl_ap_esl_config_t *config);

/**
 * @brief Read ESL information characteristics
 *
 * Reads Display Info, Image Info, Sensor Info, LED Info, and PnP ID
 * (insofar as present). Async — result via BLE_ESL_AP_EVT_ESL_INFO.
 *
 * @param conn_handle Connection handle of the connected ESL
 * @return ESP_OK if operation initiated; error code on failure
 */
esp_err_t ble_esl_ap_read_info(uint16_t conn_handle);

/**
 * @brief Transfer image data to an ESL via OTS
 *
 * Uses OTS Go To + Write Object Contents. Object_ID is derived from
 * image_index as 0x000000000100 + image_index. Async — result via
 * BLE_ESL_AP_EVT_IMAGE_TRANSFERRED.
 *
 * @param params Pointer to image transfer parameters
 * @return ESP_OK if operation initiated; error code on failure
 */
esp_err_t ble_esl_ap_transfer_image(const ble_esl_ap_image_transfer_params_t *params);

/**
 * @brief Transition a connected ESL to the Synchronized state
 *
 * Sends Update Complete command via ECP, then initiates PAST. ESL
 * synchronizes to PAwR and disconnects. Async — result via
 * BLE_ESL_AP_EVT_SYNCHRONIZED.
 *
 * @param conn_handle Connection handle of the connected ESL
 * @return ESP_OK if operation initiated; error code on failure
 */
esp_err_t ble_esl_ap_synchronize(uint16_t conn_handle);

/**
 * @brief Get the AP's tracked state for a specific ESL
 *
 * Synchronous. Returns BLE_ESL_STATE_UNASSOCIATED if not found.
 *
 * @param esl_addr ESL Address of the ESL (see ble_esl_addr_make())
 * @return Current tracked state
 */
ble_esl_state_t ble_esl_ap_get_esl_state(ble_esl_address_t esl_addr);

/* ========================== Public APIs: Command ========================== */
/* Implemented in ble_esl_ap_command.c                                       */

/**
 * @brief Send Ping command (opcode 0x00)
 *
 * Verifies ESL reachability. Expected response: Basic State.
 *
 * @param esl_id   Target ESL_ID (0x00–0xFE, or 0xFF for broadcast)
 * @param group_id Group_ID (0x00–0x7F)
 * @return ESP_OK on successful dispatch; error code on failure
 */
esp_err_t ble_esl_ap_ping(uint8_t esl_id, uint8_t group_id);

/**
 * @brief Send Display Image command (opcode 0x20)
 *
 * Immediately display a stored image. Expected response: Display State.
 *
 * @param esl_id        Target ESL_ID (0x00–0xFE, or 0xFF for broadcast)
 * @param group_id      Group_ID (0x00–0x7F)
 * @param display_index Index of the target display
 * @param image_index   Index of the stored image to show
 * @return ESP_OK on successful dispatch; error code on failure
 */
esp_err_t ble_esl_ap_display_image(uint8_t esl_id, uint8_t group_id,
                                   uint8_t display_index, uint8_t image_index);

/**
 * @brief Send Display Timed Image command (opcode 0x60)
 *
 * Schedule an image display at a future time. 0x00000000 cancels pending.
 * Expected response: Display State.
 *
 * @param esl_id        Target ESL_ID (0x00–0xFE, or 0xFF for broadcast)
 * @param group_id      Group_ID (0x00–0x7F)
 * @param display_index Index of the target display
 * @param image_index   Index of the stored image
 * @param absolute_time Scheduled time in ms (0 = cancel)
 * @return ESP_OK on successful dispatch; error code on failure
 */
esp_err_t ble_esl_ap_display_timed_image(uint8_t esl_id, uint8_t group_id,
                                         uint8_t display_index, uint8_t image_index,
                                         uint32_t absolute_time);

/**
 * @brief Send Refresh Display command (opcode 0x11)
 *
 * Refresh current display without changing image. Expected response: Display State.
 *
 * @param esl_id        Target ESL_ID (0x00–0xFE, or 0xFF for broadcast)
 * @param group_id      Group_ID (0x00–0x7F)
 * @param display_index Index of the display to refresh
 * @return ESP_OK on successful dispatch; error code on failure
 */
esp_err_t ble_esl_ap_refresh_display(uint8_t esl_id, uint8_t group_id,
                                     uint8_t display_index);

/**
 * @brief Send LED Control command (opcode 0xB0)
 *
 * Immediately control LED color, brightness, and flashing pattern.
 * Expected response: LED State.
 *
 * @param esl_id    Target ESL_ID (0x00–0xFE, or 0xFF for broadcast)
 * @param group_id  Group_ID (0x00–0x7F)
 * @param led_index Index of the target LED
 * @param settings  Pointer to LED control settings
 * @return ESP_OK on successful dispatch; error code on failure
 */
esp_err_t ble_esl_ap_led_control(uint8_t esl_id, uint8_t group_id,
                                 uint8_t led_index,
                                 const esl_ap_led_settings_t *settings);

/**
 * @brief Send LED Timed Control command (opcode 0xF0)
 *
 * Schedule LED control at a future time. 0x00000000 cancels pending.
 * Expected response: LED State.
 *
 * @param esl_id        Target ESL_ID (0x00–0xFE, or 0xFF for broadcast)
 * @param group_id      Group_ID (0x00–0x7F)
 * @param led_index     Index of the target LED
 * @param settings      Pointer to LED control settings
 * @param absolute_time Scheduled time in ms (0 = cancel)
 * @return ESP_OK on successful dispatch; error code on failure
 */
esp_err_t ble_esl_ap_led_timed_control(uint8_t esl_id, uint8_t group_id,
                                       uint8_t led_index,
                                       const esl_ap_led_settings_t *settings,
                                       uint32_t absolute_time);

/**
 * @brief Send Read Sensor Data command (opcode 0x10)
 *
 * Request sensor data. Expected response: Sensor Value.
 *
 * @param esl_id       Target ESL_ID (0x00–0xFE, or 0xFF for broadcast)
 * @param group_id     Group_ID (0x00–0x7F)
 * @param sensor_index Index of the sensor to read
 * @return ESP_OK on successful dispatch; error code on failure
 */
esp_err_t ble_esl_ap_read_sensor(uint8_t esl_id, uint8_t group_id,
                                 uint8_t sensor_index);

/**
 * @brief Send Unassociate from AP command (opcode 0x01)
 *
 * Disassociates the ESL. Expected response: Basic State. On success,
 * the ESL's tracked state transitions to Unassociated.
 *
 * @param esl_id   Target ESL_ID (0x00–0xFE, or 0xFF for broadcast)
 * @param group_id Group_ID (0x00–0x7F)
 * @return ESP_OK on successful dispatch; error code on failure
 */
esp_err_t ble_esl_ap_unassociate(uint8_t esl_id, uint8_t group_id);

/**
 * @brief Send Service Reset command (opcode 0x02)
 *
 * Clears the Service Needed flag. Expected response: Basic State.
 *
 * @param esl_id   Target ESL_ID (0x00–0xFE, or 0xFF for broadcast)
 * @param group_id Group_ID (0x00–0x7F)
 * @return ESP_OK on successful dispatch; error code on failure
 */
esp_err_t ble_esl_ap_service_reset(uint8_t esl_id, uint8_t group_id);

/**
 * @brief Send Factory Reset command (opcode 0x03) — ECP only
 *
 * Connection-oriented only; returns ESP_ERR_INVALID_STATE if ESL is
 * in Synchronized state. No response expected; ESL disconnects after.
 *
 * @param esl_id   Target ESL_ID (0x00–0xFE)
 * @param group_id Group_ID (0x00–0x7F)
 * @return ESP_OK on successful dispatch; error code on failure
 */
esp_err_t ble_esl_ap_factory_reset(uint8_t esl_id, uint8_t group_id);

/**
 * @brief Send vendor-specific command (Tag nibble = 0xF)
 *
 * Opcode constructed as (params_len << 4) | 0x0F: the Length nibble is derived
 * internally from params_len, since ESL_ID is prepended as the first parameter
 * byte and the Length nibble encodes (parameter count - 1).
 *
 * @param esl_id     Target ESL_ID (0x00–0xFE, or 0xFF for broadcast)
 * @param group_id   Group_ID (0x00–0x7F)
 * @param params     Vendor-specific parameter bytes (ESL_ID prepended internally),
 *                   may be NULL only when params_len is 0
 * @param params_len Length of params (0x00–0x0F)
 * @return ESP_OK on successful dispatch; error code on failure
 */
esp_err_t ble_esl_ap_vendor_command(uint8_t esl_id, uint8_t group_id,
                                    const uint8_t *params, uint8_t params_len);

#ifdef __cplusplus
}
#endif

#endif /* BLE_ESL_AP_H */
