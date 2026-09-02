/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file ble_esl.h
 * @brief Electronic Shelf Label (ESL) Profile — ESL Role (Server / Peripheral)
 *
 * Public API for the ESL component. Manages the five-state ESL state machine,
 * ESL Service GATT database, command processing (ECP and PAwR), display/LED/sensor
 * element management, OTP Object Server for image storage, and LE Secure Connections
 * security requirements.
 */

#ifndef BLE_ESL_H
#define BLE_ESL_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "ble_esl_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================== Kconfig Defaults ========================== */

#ifndef CONFIG_BLE_ESL_MAX_DISPLAYS
#define CONFIG_BLE_ESL_MAX_DISPLAYS     1   /*!< Maximum number of displays */
#endif

#ifndef CONFIG_BLE_ESL_MAX_IMAGES
#define CONFIG_BLE_ESL_MAX_IMAGES       4   /*!< Maximum number of stored images */
#endif

#ifndef CONFIG_BLE_ESL_MAX_LEDS
#define CONFIG_BLE_ESL_MAX_LEDS         1   /*!< Maximum number of LEDs */
#endif

#ifndef CONFIG_BLE_ESL_MAX_SENSORS
#define CONFIG_BLE_ESL_MAX_SENSORS      1   /*!< Maximum number of sensors */
#endif

#ifndef CONFIG_BLE_ESL_OTS_SUPPORT
#define CONFIG_BLE_ESL_OTS_SUPPORT      1   /*!< Enable OTS Object Server for image storage */
#endif

#ifndef CONFIG_BLE_ESL_MAX_IMAGE_SIZE
#define CONFIG_BLE_ESL_MAX_IMAGE_SIZE   4096 /*!< Allocated Size reserved per image object (bytes) */
#endif

#ifndef CONFIG_BLE_ESL_VENDOR_SPECIFIC
#define CONFIG_BLE_ESL_VENDOR_SPECIFIC  0   /*!< Enable vendor-specific opcode support */
#endif

/* ========================== Configuration ========================== */

/**
 * @brief ESL component configuration
 *
 * Passed to ble_esl_init() to configure element counts, display/LED/sensor
 * information, image storage, and vendor-specific support.
 *
 * @note The array members carry no explicit length field (except @c sensor_info,
 *       which is paired with @c sensor_info_len). Their length is implied by the
 *       matching count member and CANNOT be validated by ble_esl_init() — the
 *       caller MUST size each array as documented below:
 *       - @c display_info         : num_displays entries
 *       - @c image_writable_mask  : num_images entries
 *       - @c led_info             : num_leds bytes
 *       All arrays must stay valid for the lifetime of the ESL component, as
 *       only the pointers are stored.
 */
typedef struct {
    uint8_t num_displays;               /*!< Number of displays supported (0 to CONFIG_BLE_ESL_MAX_DISPLAYS) */
    uint8_t num_images;                 /*!< Max storable images, i.e. Max_Image_Index + 1 (0 to CONFIG_BLE_ESL_MAX_IMAGES) */
    uint8_t num_leds;                   /*!< Number of LEDs supported (0 to CONFIG_BLE_ESL_MAX_LEDS) */
    uint8_t num_sensors;                /*!< Number of sensors supported (0 to CONFIG_BLE_ESL_MAX_SENSORS) */
    const ble_esl_display_info_t *display_info; /*!< Display Data array (num_displays entries), NULL if 0 displays */
    const bool *image_writable_mask;    /*!< Per-image writable flag array (num_images entries); NULL means every image is read-only (static/pre-loaded) */
    const uint8_t *led_info;            /*!< Pre-built LED Information array (1 byte each), NULL if 0 leds */
    const uint8_t *sensor_info;         /*!< Concatenated Sensor Information structures, NULL if 0 sensors */
    uint16_t sensor_info_len;           /*!< Total byte length of sensor_info array */
    bool vendor_specific;               /*!< True to register DIS PnP ID characteristic */
    const uint8_t *pnp_id;             /*!< PnP ID value (7 octets), required when vendor_specific */
} ble_esl_config_t;

/* ========================== Event Types ========================== */

/**
 * @brief ESL event types dispatched through the application callback
 */
typedef enum {
    BLE_ESL_EVT_STATE_CHANGED = 0,      /*!< State transition occurred */
    BLE_ESL_EVT_IMAGE_WRITE,            /*!< Image data received via OTS */
    BLE_ESL_EVT_DISPLAY_IMAGE,          /*!< Display a stored image */
    BLE_ESL_EVT_REFRESH_DISPLAY,        /*!< Refresh the current display image */
    BLE_ESL_EVT_SENSOR_READ,            /*!< Read sensor data request */
    BLE_ESL_EVT_LED_CONTROL,            /*!< LED control request */
    BLE_ESL_EVT_FACTORY_RESET,          /*!< Factory reset initiated */
    BLE_ESL_EVT_UNASSOCIATE,            /*!< Unassociation initiated */
} ble_esl_event_t;

/* ========================== Event Data Structures ========================== */

/**
 * @brief Event data for BLE_ESL_EVT_STATE_CHANGED
 */
typedef struct {
    ble_esl_state_t prev_state;         /*!< Previous ESL state */
    ble_esl_state_t new_state;          /*!< New ESL state */
} ble_esl_state_changed_evt_param_t;

/**
 * @brief Event data for BLE_ESL_EVT_IMAGE_WRITE
 */
typedef struct {
    uint8_t image_index;                /*!< Image storage index (0 to Max_Image_Index) */
    const uint8_t *data;                /*!< Pointer to received image data */
    uint32_t length;                    /*!< Length of image data in bytes */
    uint32_t offset;                    /*!< Write offset within the image object */
} ble_esl_image_write_evt_param_t;

/**
 * @brief Event data for BLE_ESL_EVT_DISPLAY_IMAGE
 */
typedef struct {
    uint8_t display_index;              /*!< Index of the target display (0-based) */
    uint8_t image_index;                /*!< Index of the stored image to display (0-based) */
} ble_esl_display_image_evt_param_t;

/**
 * @brief Event data for BLE_ESL_EVT_REFRESH_DISPLAY
 */
typedef struct {
    uint8_t display_index;              /*!< Index of the display to refresh (0-based) */
} ble_esl_refresh_display_evt_param_t;

/**
 * @brief Event data for BLE_ESL_EVT_SENSOR_READ
 */
typedef struct {
    uint8_t sensor_index;               /*!< Index of the sensor to read (0-based) */
} ble_esl_sensor_read_evt_param_t;

/**
 * @brief Event data for BLE_ESL_EVT_LED_CONTROL
 *
 * Carries all LED control parameters to the application callback.
 */
typedef struct {
    uint8_t  led_index;                 /*!< Target LED index (0-based) */
    uint8_t  color_red;                 /*!< 2-bit red component (0–3) */
    uint8_t  color_green;               /*!< 2-bit green component (0–3) */
    uint8_t  color_blue;                /*!< 2-bit blue component (0–3) */
    uint8_t  brightness;                /*!< 2-bit brightness (0=25%, 1=50%, 2=75%, 3=100%) */
    uint64_t pattern;                   /*!< 40-bit flashing pattern (lower 40 bits) */
    uint8_t  bit_off_period;            /*!< Off-time per bit = value × 2 ms */
    uint8_t  bit_on_period;             /*!< On-time per bit = value × 2 ms */
    uint8_t  repeat_type;               /*!< 0 = count, 1 = time duration */
    uint16_t repeats_duration;          /*!< 15-bit repetition count or duration (seconds) */
    bool     is_off;                    /*!< True = LED should be turned off */
} ble_esl_led_control_evt_param_t;

/**
 * @brief Union of all ESL event data structures
 *
 * Discriminated by the ble_esl_event_t argument in the callback.
 * FACTORY_RESET and UNASSOCIATE events carry no data (param is NULL).
 */
typedef union {
    ble_esl_state_changed_evt_param_t    state_changed;   /*!< BLE_ESL_EVT_STATE_CHANGED */
    ble_esl_image_write_evt_param_t      image_write;     /*!< BLE_ESL_EVT_IMAGE_WRITE */
    ble_esl_display_image_evt_param_t    display_image;   /*!< BLE_ESL_EVT_DISPLAY_IMAGE */
    ble_esl_refresh_display_evt_param_t  refresh_display; /*!< BLE_ESL_EVT_REFRESH_DISPLAY */
    ble_esl_sensor_read_evt_param_t      sensor_read;     /*!< BLE_ESL_EVT_SENSOR_READ */
    ble_esl_led_control_evt_param_t      led_control;     /*!< BLE_ESL_EVT_LED_CONTROL */
} ble_esl_cb_param_t;

/**
 * @brief ESL application event callback type
 *
 * @param event Event type identifier
 * @param param Pointer to event-specific data (may be NULL for events with no data)
 */
typedef void (*ble_esl_cb_t)(ble_esl_event_t event, ble_esl_cb_param_t *param);

/* ========================== Public APIs ========================== */

/**
 * @brief Initialize the ESL component
 *
 * Registers the ESL Service GATT database, initializes the OTS Object Server
 * (if enabled and images > 0), registers DIS PnP ID if vendor_specific is true,
 * and sets the state machine to Unassociated. Does not start advertising.
 *
 * @note This function modifies the GLOBAL NimBLE Security Manager configuration
 *       (@c ble_hs_cfg.sm_io_cap, @c sm_bonding, @c sm_mitm, @c sm_sc,
 *       @c sm_our_key_dist, @c sm_their_key_dist) to the settings the ESL Profile
 *       mandates: LE Secure Connections, bonding, No Input No Output, no MITM.
 *       These settings are host-wide and therefore affect all other profiles and
 *       connections. The previous values are not saved and ble_esl_deinit() does
 *       not restore them. An application requiring different SM settings must
 *       apply them after this function returns.
 *
 * @param[in] config Pointer to ESL configuration structure
 *
 * @return
 *  - ESP_OK on success
 *  - ESP_ERR_INVALID_ARG if config is invalid
 *  - ESP_ERR_NO_MEM on allocation failure
 *  - ESP_ERR_INVALID_STATE if already initialized
 */
esp_err_t ble_esl_init(const ble_esl_config_t *config);

/**
 * @brief Deinitialize the ESL component
 *
 * Stops advertising, disconnects, removes the GATT database, tears down OTS
 * objects, releases all allocated resources, and resets internal state.
 *
 * @note The global NimBLE Security Manager settings applied by ble_esl_init()
 *       are left in place; reconfigure @c ble_hs_cfg explicitly if needed.
 *
 * @return
 *  - ESP_OK on success
 *  - ESP_ERR_INVALID_STATE if not initialized
 */
esp_err_t ble_esl_deinit(void);

/**
 * @brief Start ESL operation
 *
 * Enters the GAP advertising mode appropriate for the current state.
 * Must be called after ble_esl_init().
 *
 * @return
 *  - ESP_OK on success
 *  - ESP_ERR_INVALID_STATE if not initialized or already started
 */
esp_err_t ble_esl_start(void);

/**
 * @brief Stop ESL operation
 *
 * Stops advertising, disconnects, and stops PAwR synchronization if active.
 * The ESL state is preserved.
 *
 * @return
 *  - ESP_OK on success
 *  - ESP_ERR_INVALID_STATE if not started
 *  - ESP_FAIL if the PAwR sync could not be terminated; the ESL is still
 *    stopped (advertising off, timers disarmed) and the sync is torn down on
 *    the next state transition or on ble_esl_deinit()
 */
esp_err_t ble_esl_stop(void);

/**
 * @brief Register the application event callback
 *
 * All ESL events are dispatched through this single callback.
 * Must be called after ble_esl_init() and before ble_esl_start().
 *
 * @param[in] callback Application event callback function
 *
 * @return
 *  - ESP_OK on success
 *  - ESP_ERR_INVALID_ARG if callback is NULL
 *  - ESP_ERR_INVALID_STATE if not initialized
 */
esp_err_t ble_esl_register_cb(ble_esl_cb_t callback);

/**
 * @brief Get the current ESL state
 *
 * Synchronous. Returns a snapshot of the state machine; the state is only
 * changed from the NimBLE host task, so the value may already be stale when
 * read from another task. Use BLE_ESL_EVT_STATE_CHANGED to react to changes.
 *
 * @return Current ESL state; BLE_ESL_STATE_UNASSOCIATED if not initialized
 */
ble_esl_state_t ble_esl_get_state(void);

/**
 * @brief Set or clear the Service Needed flag
 *
 * Controls bit 0 of the Basic State bitmap. Set when the application detects
 * a condition requiring AP attention (e.g., low battery, display fault).
 *
 * @param[in] flag True to set Service Needed, false to clear it
 *
 * @return
 *  - ESP_OK on success
 *  - ESP_ERR_INVALID_STATE if not initialized
 */
esp_err_t ble_esl_set_service_needed(bool flag);

/**
 * @brief Report sensor data in response to a BLE_ESL_EVT_SENSOR_READ event
 *
 * Must be called once per BLE_ESL_EVT_SENSOR_READ event. On success
 * (error_code = 0), a Sensor Value response is sent. On failure, an Error
 * response with the given error code is sent.
 *
 * @param[in] sensor_index Index of the sensor (must match the event's sensor_index)
 * @param[in] error_code   0 = success; non-zero = ESL error code
 * @param[in] data         Pointer to sensor data (ignored when error_code != 0)
 * @param[in] data_len     Length of sensor data (1–15, ignored when error_code != 0)
 *
 * @return
 *  - ESP_OK on success
 *  - ESP_ERR_INVALID_ARG if data is NULL or data_len invalid when error_code = 0
 *  - ESP_ERR_INVALID_STATE if no sensor read is pending for sensor_index
 */
esp_err_t ble_esl_report_sensor_data(uint8_t sensor_index, uint8_t error_code,
                                     const uint8_t *data, uint8_t data_len);

#ifdef __cplusplus
}
#endif

#endif /* BLE_ESL_H */
