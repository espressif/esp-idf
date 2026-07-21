/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file esl_cmd_led.c
 * @brief ESL LED Control, LED Timed Control, and Vendor-Specific command handlers
 *
 * Implements: LED Control (opcode 0xB0), LED Timed Control (opcode 0xF0),
 * Vendor-specific tag (opcodes 0x_F), LED status query for Basic State bitmap,
 * pattern expiry via esp_timer, and timed LED scheduling.
 */

#include <string.h>
#include <inttypes.h>
#include "esp_log.h"
#include "esp_timer.h"
#include "ble_esl.h"
#include "ble_esl_common.h"
#include "ble_esl_int.h"

static const char *TAG = "esl_cmd_led";

/* ========================================================================== */
/*  Per-LED Internal State                                                    */
/* ========================================================================== */

/** @brief Stored LED control settings for timed commands */
typedef struct {
    ble_esl_led_control_t settings;     /*!< Raw LED control settings from command */
    uint32_t absolute_time;             /*!< Target absolute time for execution */
} esl_led_timed_params_t;

/** @brief Per-LED state tracking */
typedef struct {
    bool active;                        /*!< LED is currently executing a command */
    bool timed_pending;                 /*!< A timed LED command is pending */
    esl_led_timed_params_t timed_params;/*!< Stored timed command parameters */
    ble_esl_led_control_evt_param_t current_params; /*!< Current active LED parameters */
    esp_timer_handle_t pattern_timer;   /*!< Timer for pattern expiry */
    esp_timer_handle_t timed_timer;     /*!< Timer for timed command execution */
} esl_led_state_t;

/* ========================================================================== */
/*  Module Context                                                            */
/* ========================================================================== */

/** @brief Aggregate internal state for the LED command module */
typedef struct {
    esl_led_state_t leds[CONFIG_BLE_ESL_MAX_LEDS]; /*!< Per-LED state array */
    bool initialized;                               /*!< Module initialized flag */
} esl_cmd_led_ctx_t;

static esl_cmd_led_ctx_t *s_ctx = NULL;

/* ========================================================================== */
/*  Forward Declarations                                                      */
/* ========================================================================== */

static void pattern_expiry_timer_cb(void *arg);
static void timed_led_timer_cb(void *arg);
static void build_led_state_response(uint8_t led_index, ble_esl_cmd_result_t *result);
static void build_error_response(uint8_t error_code, ble_esl_cmd_result_t *result);
static void parse_led_control_settings(uint8_t led_index,
                                       const ble_esl_led_control_t *settings,
                                       ble_esl_led_control_evt_param_t *out_params);
static void apply_led_command(uint8_t led_index, const ble_esl_led_control_evt_param_t *params);
static uint64_t compute_pattern_duration_us(const ble_esl_led_control_evt_param_t *params);

/* ========================================================================== */
/*  Helper: Build LED State Response                                          */
/* ========================================================================== */

/**
 * @brief Build an LED State response (opcode 0x01) into the result struct
 *
 * @param led_index LED index to include in the response
 * @param result    Output result
 */
static void build_led_state_response(uint8_t led_index, ble_esl_cmd_result_t *result)
{
    result->has_response = true;
    result->resp_opcode = BLE_ESL_RESP_LED_STATE;
    result->resp_params[0] = led_index;
    result->resp_params_len = 1;
}

/* ========================================================================== */
/*  Helper: Build Error Response                                              */
/* ========================================================================== */

/**
 * @brief Build an Error response (opcode 0x00) into the result struct
 *
 * @param error_code ESL error code
 * @param result     Output result
 */
static void build_error_response(uint8_t error_code, ble_esl_cmd_result_t *result)
{
    result->has_response = true;
    result->resp_opcode = BLE_ESL_RESP_ERROR;
    result->resp_params[0] = error_code;
    result->resp_params_len = 1;
}

/* ========================================================================== */
/*  Helper: Parse LED Control Settings                                        */
/* ========================================================================== */

/**
 * @brief Parse raw LED control settings into the application-facing params struct
 *
 * @param led_index LED index
 * @param settings  Raw LED control settings from the command
 * @param out_params Output parsed parameters
 */
static void parse_led_control_settings(uint8_t led_index,
                                       const ble_esl_led_control_t *settings,
                                       ble_esl_led_control_evt_param_t *out_params)
{
    memset(out_params, 0, sizeof(*out_params));

    out_params->led_index = led_index;

    /* Parse color and brightness from color_brightness byte */
    uint8_t cb = settings->color_brightness;
    out_params->color_red = BLE_ESL_LED_CTRL_RED(cb);
    out_params->color_green = BLE_ESL_LED_CTRL_GREEN(cb);
    out_params->color_blue = BLE_ESL_LED_CTRL_BLUE(cb);
    out_params->brightness = BLE_ESL_LED_CTRL_BRIGHTNESS(cb);

    /* Parse flashing pattern: 40-bit pattern stored in 5 bytes (little-endian) */
    uint64_t pattern = 0;
    for (int i = 0; i < 5; i++) {
        pattern |= (uint64_t)settings->flashing.pattern[i] << (8 * i);
    }
    out_params->pattern = pattern;

    out_params->bit_off_period = settings->flashing.bit_off_period;
    out_params->bit_on_period = settings->flashing.bit_on_period;

    /* Parse repeat field */
    uint16_t repeat = settings->repeat;
    out_params->repeat_type = BLE_ESL_LED_CTRL_REPEAT_TYPE(repeat);
    out_params->repeats_duration = BLE_ESL_LED_CTRL_REPEATS_DURATION(repeat);

    out_params->is_off = false;
}

/* ========================================================================== */
/*  Helper: Compute Pattern Duration in Microseconds                          */
/* ========================================================================== */

/**
 * @brief Compute the total duration of the LED pattern for timer arming
 *
 * For repeat_type=0 (count): estimates total time based on pattern bits and repeat count.
 * For repeat_type=1 (duration): uses the duration in seconds directly.
 *
 * @param params Parsed LED control parameters
 * @return Duration in microseconds, or 0 if continuous/off
 */
static uint64_t compute_pattern_duration_us(const ble_esl_led_control_evt_param_t *params)
{
    if (params->repeats_duration == 0) {
        /* Special cases: continuous on or off — no finite duration */
        return 0;
    }

    if (params->repeat_type == 1) {
        /* Duration mode: repeats_duration is in 1-second increments */
        return (uint64_t)params->repeats_duration * 1000000ULL;
    }

    /* Count mode: estimate total time from pattern bits and periods */
    /* Find the meaningful pattern length (from first '1' bit to last bit) */
    uint64_t pat = params->pattern;
    if (pat == 0) {
        /* No pattern bits set — treat as immediate off after count */
        return 0;
    }

    /* Count meaningful bits: from MSB first '1' to LSB */
    int first_one = -1;
    for (int i = 39; i >= 0; i--) {
        if (pat & (1ULL << i)) {
            first_one = i;
            break;
        }
    }

    if (first_one < 0) {
        return 0;
    }

    /* Count on-bits and off-bits in the meaningful range */
    int on_bits = 0;
    int off_bits = 0;
    for (int i = first_one; i >= 0; i--) {
        if (pat & (1ULL << i)) {
            on_bits++;
        } else {
            off_bits++;
        }
    }

    /* Time per pattern cycle in ms */
    uint32_t cycle_ms = (uint32_t)on_bits * (uint32_t)params->bit_on_period * 2 +
                        (uint32_t)off_bits * (uint32_t)params->bit_off_period * 2;

    /* Total time = cycle_ms * repeats_duration (count) */
    uint64_t total_ms = (uint64_t)cycle_ms * (uint64_t)params->repeats_duration;

    return total_ms * 1000ULL; /* Convert to microseconds */
}

/* ========================================================================== */
/*  Helper: Apply LED Command                                                 */
/* ========================================================================== */

/**
 * @brief Apply an LED command: update state, fire callback, arm pattern timer
 *
 * @param led_index LED index
 * @param params    Parsed LED control parameters
 */
static void apply_led_command(uint8_t led_index, const ble_esl_led_control_evt_param_t *params)
{
    assert(led_index < CONFIG_BLE_ESL_MAX_LEDS);
    esl_led_state_t *led = &s_ctx->leds[led_index];

    /* Stop any running pattern expiry timer (superseding) */
    esp_timer_stop(led->pattern_timer);

    /* Store current parameters */
    memcpy(&led->current_params, params, sizeof(ble_esl_led_control_evt_param_t));

    /* Determine special cases based on repeat_type and repeats_duration */
    if (params->repeat_type == 0 && params->repeats_duration == 0) {
        /* Continuous off: LED should be turned off */
        led->active = false;
        led->current_params.is_off = true;

        /* Fire callback with is_off = true */
        ble_esl_cb_param_t cb_param;
        memset(&cb_param, 0, sizeof(cb_param));
        memcpy(&cb_param.led_control, &led->current_params, sizeof(ble_esl_led_control_evt_param_t));
        cb_param.led_control.is_off = true;
        esl_notify_app(BLE_ESL_EVT_LED_CONTROL, &cb_param);

        ESP_LOGI(TAG, "LED %u: turned off (repeat_type=0, duration=0)", led_index);
        return;
    }

    if (params->repeat_type == 1 && params->repeats_duration == 0) {
        /* Continuous on: LED illuminated steadily, no pattern timer */
        led->active = true;

        /* Fire callback — pattern is ignored, steady illumination */
        ble_esl_cb_param_t cb_param;
        memset(&cb_param, 0, sizeof(cb_param));
        memcpy(&cb_param.led_control, &led->current_params, sizeof(ble_esl_led_control_evt_param_t));
        esl_notify_app(BLE_ESL_EVT_LED_CONTROL, &cb_param);

        ESP_LOGI(TAG, "LED %u: continuous on (repeat_type=1, duration=0)", led_index);
        return;
    }

    /* Normal case: finite pattern with count or duration */

    /* Compute pattern duration before committing state */
    uint64_t duration_us = compute_pattern_duration_us(params);

    if (duration_us == 0) {
        /* Zero duration (e.g. all-zero pattern): treat as immediate off */
        led->active = false;
        led->current_params.is_off = true;

        ble_esl_cb_param_t cb_param;
        memset(&cb_param, 0, sizeof(cb_param));
        memcpy(&cb_param.led_control, &led->current_params, sizeof(ble_esl_led_control_evt_param_t));
        cb_param.led_control.is_off = true;
        esl_notify_app(BLE_ESL_EVT_LED_CONTROL, &cb_param);

        ESP_LOGI(TAG, "LED %u: zero-duration pattern, immediately off", led_index);
        return;
    }

    led->active = true;

    /* Fire callback with LED parameters */
    ble_esl_cb_param_t cb_param;
    memset(&cb_param, 0, sizeof(cb_param));
    memcpy(&cb_param.led_control, &led->current_params, sizeof(ble_esl_led_control_evt_param_t));
    esl_notify_app(BLE_ESL_EVT_LED_CONTROL, &cb_param);

    /* Arm pattern expiry timer */
    {
        esp_err_t ret = esp_timer_start_once(led->pattern_timer, duration_us);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "LED %u: failed to start pattern timer: %s",
                     led_index, esp_err_to_name(ret));

            /* Roll back: mark LED inactive and notify app to turn off */
            led->active = false;
            led->current_params.is_off = true;

            ble_esl_cb_param_t off_param;
            memset(&off_param, 0, sizeof(off_param));
            memcpy(&off_param.led_control, &led->current_params, sizeof(ble_esl_led_control_evt_param_t));
            off_param.led_control.is_off = true;
            esl_notify_app(BLE_ESL_EVT_LED_CONTROL, &off_param);
        } else {
            ESP_LOGI(TAG, "LED %u: pattern timer armed for %" PRIu64 " us",
                     led_index, duration_us);
        }
    }
}

/* ========================================================================== */
/*  Pattern Expiry Timer Callback                                             */
/* ========================================================================== */

/**
 * @brief esp_timer callback for pattern expiry
 *
 * Sets the LED inactive, fires BLE_ESL_EVT_LED_CONTROL with is_off=true,
 * and updates the active flag.
 *
 * @param arg LED index (cast from uintptr_t)
 */
static void pattern_expiry_timer_cb(void *arg)
{
    uint8_t led_index = (uint8_t)(uintptr_t)arg;

    if (s_ctx == NULL) {
        return;
    }

    if (led_index >= CONFIG_BLE_ESL_MAX_LEDS) {
        ESP_LOGE(TAG, "Pattern expiry callback: invalid led_index %u", led_index);
        return;
    }

    esl_led_state_t *led = &s_ctx->leds[led_index];

    /* Mark LED as inactive */
    led->active = false;

    ESP_LOGI(TAG, "LED %u: pattern expired, turning off", led_index);

    /* Fire callback with is_off = true */
    ble_esl_cb_param_t cb_param;
    memset(&cb_param, 0, sizeof(cb_param));
    memcpy(&cb_param.led_control, &led->current_params, sizeof(ble_esl_led_control_evt_param_t));
    cb_param.led_control.is_off = true;
    esl_notify_app(BLE_ESL_EVT_LED_CONTROL, &cb_param);
}

/* ========================================================================== */
/*  Timed LED Timer Callback                                                  */
/* ========================================================================== */

/**
 * @brief esp_timer callback for timed LED command execution
 *
 * When the scheduled Absolute_Time arrives, parses the stored parameters,
 * applies the LED state, fires the callback, and arms the pattern expiry
 * timer if the duration is finite.
 *
 * @param arg LED index (cast from uintptr_t)
 */
static void timed_led_timer_cb(void *arg)
{
    uint8_t led_index = (uint8_t)(uintptr_t)arg;

    if (s_ctx == NULL) {
        return;
    }

    if (led_index >= CONFIG_BLE_ESL_MAX_LEDS) {
        ESP_LOGE(TAG, "Timed LED callback: invalid led_index %u", led_index);
        return;
    }

    esl_led_state_t *led = &s_ctx->leds[led_index];

    if (!led->timed_pending) {
        ESP_LOGW(TAG, "Timed LED callback: no pending command for LED %u", led_index);
        return;
    }

    /* Clear pending state */
    led->timed_pending = false;

    ESP_LOGI(TAG, "Timed LED executed: LED %u", led_index);

    /* Parse the stored settings */
    ble_esl_led_control_evt_param_t params;
    parse_led_control_settings(led_index, &led->timed_params.settings, &params);

    /* Apply the LED command (handles special cases, fires callback, arms pattern timer) */
    apply_led_command(led_index, &params);
}

/* ========================================================================== */
/*  Command Handler: LED Control (opcode 0xB0)                                */
/* ========================================================================== */

esp_err_t esl_cmd_led_control(const uint8_t *params, uint8_t params_len,
                              ble_esl_cmd_result_t *result)
{
    assert(s_ctx != NULL);
    assert(params != NULL);
    assert(result != NULL);

    memset(result, 0, sizeof(*result));

    /*
     * LED Control command parameters (after ESL_ID, which is already stripped
     * by the dispatch layer):
     *   [LED_Index(1), LED_Control_Settings(10)]
     * LED_Control_Settings = color_brightness(1) + flashing_pattern(7) + repeat(2)
     * Total params_len expected: 1 + 10 = 11
     */
    if (params_len < 1 + (uint8_t)sizeof(ble_esl_led_control_t)) {
        ESP_LOGE(TAG, "LED Control: insufficient params (len=%u, need %u)",
                 params_len, (unsigned)(1 + sizeof(ble_esl_led_control_t)));
        build_error_response(BLE_ESL_ERR_INVALID_PARAMS, result);
        return ESP_OK;
    }

    uint8_t led_index = params[0];

    /* Validate LED_Index */
    const ble_esl_config_t *config = esl_get_config();
    if (config == NULL) {
        ESP_LOGE(TAG, "LED Control: config not available");
        build_error_response(BLE_ESL_ERR_UNSPECIFIED, result);
        return ESP_OK;
    }

    if (led_index >= CONFIG_BLE_ESL_MAX_LEDS || led_index >= config->num_leds) {
        ESP_LOGW(TAG, "LED Control: invalid led_index %u (max=%d, num_leds=%u)",
                 led_index, CONFIG_BLE_ESL_MAX_LEDS, config->num_leds);
        build_error_response(BLE_ESL_ERR_INVALID_PARAMS, result);
        return ESP_OK;
    }

    /* Parse LED Control Settings */
    const ble_esl_led_control_t *settings =
        (const ble_esl_led_control_t *)&params[1];

    /* Parse into application-facing params */
    ble_esl_led_control_evt_param_t led_params;
    parse_led_control_settings(led_index, settings, &led_params);

    /* Validate Bit_On_Period / Bit_Off_Period when flashing is required */
    if (led_params.repeats_duration > 0) {
        if (led_params.bit_on_period == 0 || led_params.bit_off_period == 0) {
            ESP_LOGW(TAG, "LED Control: invalid bit_on/off_period (on=%u, off=%u) "
                     "with repeats_duration=%u",
                     led_params.bit_on_period, led_params.bit_off_period,
                     led_params.repeats_duration);
            build_error_response(BLE_ESL_ERR_INVALID_PARAMS, result);
            return ESP_OK;
        }
    }

    /* Apply the LED command */
    apply_led_command(led_index, &led_params);

    /* Build LED State response */
    build_led_state_response(led_index, result);

    ESP_LOGI(TAG, "LED Control: led=%u, R=%u G=%u B=%u, bright=%u, "
             "repeat_type=%u, duration=%u",
             led_index, led_params.color_red, led_params.color_green,
             led_params.color_blue, led_params.brightness,
             led_params.repeat_type, led_params.repeats_duration);

    return ESP_OK;
}

/* ========================================================================== */
/*  Command Handler: LED Timed Control (opcode 0xF0)                          */
/* ========================================================================== */

esp_err_t esl_cmd_led_timed_control(const uint8_t *params, uint8_t params_len,
                                    ble_esl_cmd_result_t *result)
{
    assert(s_ctx != NULL);
    assert(params != NULL);
    assert(result != NULL);

    memset(result, 0, sizeof(*result));

    /*
     * LED Timed Control command parameters (after ESL_ID):
     *   [LED_Index(1), LED_Control_Settings(10), Absolute_Time(4 LE)]
     * Total params_len expected: 1 + 10 + 4 = 15
     */
    uint8_t expected_len = 1 + (uint8_t)sizeof(ble_esl_led_control_t) + 4;
    if (params_len < expected_len) {
        ESP_LOGE(TAG, "LED Timed Control: insufficient params (len=%u, need %u)",
                 params_len, expected_len);
        build_error_response(BLE_ESL_ERR_INVALID_PARAMS, result);
        return ESP_OK;
    }

    uint8_t led_index = params[0];

    /* Validate LED_Index */
    const ble_esl_config_t *config = esl_get_config();
    if (config == NULL) {
        ESP_LOGE(TAG, "LED Timed Control: config not available");
        build_error_response(BLE_ESL_ERR_UNSPECIFIED, result);
        return ESP_OK;
    }

    if (led_index >= CONFIG_BLE_ESL_MAX_LEDS || led_index >= config->num_leds) {
        ESP_LOGW(TAG, "LED Timed Control: invalid led_index %u (max=%d, num_leds=%u)",
                 led_index, CONFIG_BLE_ESL_MAX_LEDS, config->num_leds);
        build_error_response(BLE_ESL_ERR_INVALID_PARAMS, result);
        return ESP_OK;
    }

    /* Parse LED Control Settings */
    const ble_esl_led_control_t *settings =
        (const ble_esl_led_control_t *)&params[1];

    /* Validate Bit_On_Period / Bit_Off_Period when flashing is required */
    {
        ble_esl_led_control_evt_param_t tmp_params;
        parse_led_control_settings(led_index, settings, &tmp_params);
        if (tmp_params.repeats_duration > 0) {
            if (tmp_params.bit_on_period == 0 || tmp_params.bit_off_period == 0) {
                ESP_LOGW(TAG, "LED Timed Control: invalid bit_on/off_period (on=%u, off=%u) "
                         "with repeats_duration=%u",
                         tmp_params.bit_on_period, tmp_params.bit_off_period,
                         tmp_params.repeats_duration);
                build_error_response(BLE_ESL_ERR_INVALID_PARAMS, result);
                return ESP_OK;
            }
        }
    }

    /* Parse Absolute_Time (little-endian, 4 bytes after LED_Control_Settings) */
    uint8_t abs_time_offset = 1 + (uint8_t)sizeof(ble_esl_led_control_t);
    uint32_t absolute_time = (uint32_t)params[abs_time_offset] |
                             ((uint32_t)params[abs_time_offset + 1] << 8) |
                             ((uint32_t)params[abs_time_offset + 2] << 16) |
                             ((uint32_t)params[abs_time_offset + 3] << 24);

    esl_led_state_t *led = &s_ctx->leds[led_index];

    /* Handle Absolute_Time = 0 (cancellation) */
    if (absolute_time == 0x00000000) {
        if (led->timed_pending) {
            /* Stop the timed timer */
            esp_err_t ret = esp_timer_stop(led->timed_timer);
            if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
                ESP_LOGW(TAG, "Failed to stop timed LED timer: %s", esp_err_to_name(ret));
            }
            led->timed_pending = false;
            ESP_LOGI(TAG, "LED Timed Control: cancelled for LED %u", led_index);
        }
        /* Respond with LED State response confirming deletion */
        build_led_state_response(led_index, result);
        return ESP_OK;
    }

    /* Validate Absolute_Time plausibility */
    uint32_t current_time = esl_get_abs_time();
    uint32_t delta;
    if (absolute_time >= current_time) {
        delta = absolute_time - current_time;
    } else {
        /* Time wraps around */
        delta = (0xFFFFFFFFUL - current_time) + absolute_time + 1;
    }

    if (delta > BLE_ESL_IMPLAUSIBLE_TIME_MS) {
        ESP_LOGW(TAG, "LED Timed Control: implausible abs_time=0x%08" PRIx32
                 " (delta=%" PRIu32 " ms)", absolute_time, delta);
        build_error_response(BLE_ESL_ERR_IMPLAUSIBLE_ABS_TIME, result);
        return ESP_OK;
    }

    /* Handle pending timed command conflict */
    if (led->timed_pending) {
        if (led->timed_params.absolute_time == absolute_time) {
            /* Same time: replace the pending command */
            memcpy(&led->timed_params.settings, settings, sizeof(ble_esl_led_control_t));
            ESP_LOGI(TAG, "LED Timed Control: replaced pending for LED %u", led_index);
        } else {
            /* Different time and non-zero: reject with Queue Full */
            ESP_LOGW(TAG, "LED Timed Control: queue full for LED %u", led_index);
            build_error_response(BLE_ESL_ERR_QUEUE_FULL, result);
            return ESP_OK;
        }
    } else {
        /* No pending command: accept and schedule */
        led->timed_pending = true;
        memcpy(&led->timed_params.settings, settings, sizeof(ble_esl_led_control_t));
        led->timed_params.absolute_time = absolute_time;

        /* Schedule the timed timer */
        uint64_t timeout_us = (uint64_t)delta * 1000ULL;

        /* Stop timer if it happens to be running (safety) */
        esp_timer_stop(led->timed_timer);

        esp_err_t ret = esp_timer_start_once(led->timed_timer, timeout_us);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to start timed LED timer: %s", esp_err_to_name(ret));
            led->timed_pending = false;
            build_error_response(BLE_ESL_ERR_INSUFFICIENT_RESOURCES, result);
            return ESP_OK;
        }

        ESP_LOGI(TAG, "LED Timed Control: scheduled LED=%u, abs_time=0x%08" PRIx32
                 " (delta=%" PRIu32 " ms)", led_index, absolute_time, delta);
    }

    /* Respond immediately with LED State response */
    build_led_state_response(led_index, result);

    return ESP_OK;
}

/* ========================================================================== */
/*  Command Handler: Vendor-Specific (opcodes 0x_F)                           */
/* ========================================================================== */

esp_err_t esl_cmd_vendor_specific(uint8_t opcode, const uint8_t *params,
                                  uint8_t params_len, ble_esl_cmd_result_t *result)
{
    assert(result != NULL);

    memset(result, 0, sizeof(*result));

    (void)params;
    (void)params_len;

#if CONFIG_BLE_ESL_VENDOR_SPECIFIC
    /*
     * Vendor-specific support is enabled, but no vendor handlers are registered
     * in this base implementation. Return Invalid Opcode.
     */
    ESP_LOGW(TAG, "Vendor-specific opcode 0x%02X: no handler registered", opcode);
    build_error_response(BLE_ESL_ERR_INVALID_OPCODE, result);
#else
    /* Vendor-specific support disabled: return Invalid Opcode */
    ESP_LOGW(TAG, "Vendor-specific opcode 0x%02X: vendor support disabled", opcode);
    build_error_response(BLE_ESL_ERR_INVALID_OPCODE, result);
#endif /* CONFIG_BLE_ESL_VENDOR_SPECIFIC */

    return ESP_OK;
}

/* ========================================================================== */
/*  Internal Interface: esl_get_led_status()                                  */
/* ========================================================================== */

esl_led_status_t esl_get_led_status(void)
{
    esl_led_status_t status = {
        .active_led = false,
        .pending_led_update = false,
    };

    if (s_ctx == NULL) {
        return status;
    }

    for (int i = 0; i < CONFIG_BLE_ESL_MAX_LEDS; i++) {
        if (s_ctx->leds[i].active) {
            status.active_led = true;
        }
        if (s_ctx->leds[i].timed_pending) {
            status.pending_led_update = true;
        }
        /* Early exit if both flags are already set */
        if (status.active_led && status.pending_led_update) {
            break;
        }
    }

    return status;
}

/* ========================================================================== */
/*  Internal Interface: esl_cmd_led_cancel_all()                              */
/* ========================================================================== */

void esl_cmd_led_cancel_all(void)
{
    if (s_ctx == NULL) {
        return;
    }

    for (int i = 0; i < CONFIG_BLE_ESL_MAX_LEDS; i++) {
        esl_led_state_t *led = &s_ctx->leds[i];

        /* Stop pattern expiry timer */
        if (led->pattern_timer != NULL) {
            esp_timer_stop(led->pattern_timer);
        }

        /* Stop timed command timer */
        if (led->timed_timer != NULL) {
            esp_timer_stop(led->timed_timer);
        }

        /* Notify application to turn off active LEDs */
        if (led->active) {
            ble_esl_cb_param_t cb_param;
            memset(&cb_param, 0, sizeof(cb_param));
            memcpy(&cb_param.led_control, &led->current_params, sizeof(ble_esl_led_control_evt_param_t));
            cb_param.led_control.is_off = true;
            esl_notify_app(BLE_ESL_EVT_LED_CONTROL, &cb_param);
        }

        /* Clear all state */
        led->active = false;
        led->timed_pending = false;
        memset(&led->timed_params, 0, sizeof(led->timed_params));
        memset(&led->current_params, 0, sizeof(led->current_params));
    }

    ESP_LOGI(TAG, "All LED commands cancelled");
}

/* ========================================================================== */
/*  Init / Deinit                                                             */
/* ========================================================================== */

esp_err_t esl_cmd_led_init(void)
{
    if (s_ctx != NULL) {
        ESP_LOGW(TAG, "LED command module already initialized");
        return ESP_ERR_INVALID_STATE;
    }

    s_ctx = calloc(1, sizeof(esl_cmd_led_ctx_t));
    if (s_ctx == NULL) {
        ESP_LOGE(TAG, "Failed to allocate LED command context");
        return ESP_ERR_NO_MEM;
    }

    /* Create esp_timers for each LED slot (pattern expiry + timed command) */
    esp_err_t ret = ESP_OK;
    for (int i = 0; i < CONFIG_BLE_ESL_MAX_LEDS; i++) {
        /* Pattern expiry timer */
        esp_timer_create_args_t pattern_timer_args = {
            .callback = pattern_expiry_timer_cb,
            .arg = (void *)(uintptr_t)i,
            .dispatch_method = ESP_TIMER_TASK,
            .name = "esl_led_pattern",
        };

        ret = esp_timer_create(&pattern_timer_args,
                                         &s_ctx->leds[i].pattern_timer);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to create pattern timer for LED %d: %s",
                     i, esp_err_to_name(ret));
            goto cleanup;
        }

        /* Timed command timer */
        esp_timer_create_args_t timed_timer_args = {
            .callback = timed_led_timer_cb,
            .arg = (void *)(uintptr_t)i,
            .dispatch_method = ESP_TIMER_TASK,
            .name = "esl_led_timed",
        };

        ret = esp_timer_create(&timed_timer_args, &s_ctx->leds[i].timed_timer);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to create timed timer for LED %d: %s",
                     i, esp_err_to_name(ret));
            /* Delete the pattern timer we just created for this LED */
            esp_timer_delete(s_ctx->leds[i].pattern_timer);
            s_ctx->leds[i].pattern_timer = NULL;
            goto cleanup;
        }
    }

    s_ctx->initialized = true;
    ESP_LOGI(TAG, "LED command module initialized (%d LEDs)", CONFIG_BLE_ESL_MAX_LEDS);

    return ESP_OK;

cleanup:
    /* Clean up already created timers */
    for (int j = 0; j < CONFIG_BLE_ESL_MAX_LEDS; j++) {
        if (s_ctx->leds[j].pattern_timer != NULL) {
            esp_timer_delete(s_ctx->leds[j].pattern_timer);
            s_ctx->leds[j].pattern_timer = NULL;
        }
        if (s_ctx->leds[j].timed_timer != NULL) {
            esp_timer_delete(s_ctx->leds[j].timed_timer);
            s_ctx->leds[j].timed_timer = NULL;
        }
    }
    free(s_ctx);
    s_ctx = NULL;
    return ret;
}

void esl_cmd_led_deinit(void)
{
    if (s_ctx == NULL) {
        return;
    }

    /* Notify application to turn off all active LEDs before teardown */
    esl_cmd_led_cancel_all();

    /* Stop and delete all timers */
    for (int i = 0; i < CONFIG_BLE_ESL_MAX_LEDS; i++) {
        if (s_ctx->leds[i].pattern_timer != NULL) {
            esp_timer_stop(s_ctx->leds[i].pattern_timer);
            esp_err_t ret = esp_timer_delete(s_ctx->leds[i].pattern_timer);
            if (ret != ESP_OK) {
                ESP_LOGW(TAG, "Failed to delete pattern timer for LED %d: %s",
                         i, esp_err_to_name(ret));
            }
            s_ctx->leds[i].pattern_timer = NULL;
        }

        if (s_ctx->leds[i].timed_timer != NULL) {
            esp_timer_stop(s_ctx->leds[i].timed_timer);
            esp_err_t ret = esp_timer_delete(s_ctx->leds[i].timed_timer);
            if (ret != ESP_OK) {
                ESP_LOGW(TAG, "Failed to delete timed timer for LED %d: %s",
                         i, esp_err_to_name(ret));
            }
            s_ctx->leds[i].timed_timer = NULL;
        }
    }

    free(s_ctx);
    s_ctx = NULL;

    ESP_LOGI(TAG, "LED command module deinitialized");
}
