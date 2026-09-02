/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file esl_cmd_display.c
 * @brief ESL Display & Sensor command handlers
 *
 * Implements Read Sensor Data, Refresh Display, Display Image, and
 * Display Timed Image command processing, including timed command
 * scheduling, sensor read deferral, and Pending Display Update flag
 * management.
 */

#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "ble_esl_int.h"

static const char *TAG = "esl_cmd_display";

/* ========================== Internal Data Structures ========================== */

/** @brief Per-display internal state */
typedef struct {
    bool     has_active_image;      /*!< Whether an image is currently displayed */
    uint8_t  current_image_index;   /*!< Image index currently displayed (valid if has_active_image) */
    bool     timed_pending;         /*!< Whether a timed display command is pending */
    uint8_t  timed_image_index;     /*!< Image index for the pending timed command */
    uint32_t timed_absolute_time;   /*!< Absolute time for the pending timed command */
    esp_timer_handle_t timer;       /*!< Timer handle for timed display commands */
} display_state_t;

/** @brief Maximum sensor data length (ESL spec: up to 15 bytes) */
#define SENSOR_DATA_CACHE_MAX_LEN   15

/** @brief Per-sensor internal state */
typedef struct {
    bool    read_pending;           /*!< Whether a sensor read is in progress */
    uint8_t transport_context;      /*!< ESL_TRANSPORT_ECP or ESL_TRANSPORT_PAWR */
    bool    cache_valid;            /*!< Whether cached sensor data is available (PAwR) */
    uint8_t cache_error_code;       /*!< Cached error code (0 = success) */
    uint8_t cache_data[SENSOR_DATA_CACHE_MAX_LEN]; /*!< Cached sensor data */
    uint8_t cache_data_len;         /*!< Length of cached sensor data */
} sensor_state_t;

/** @brief Aggregated display/sensor module context */
typedef struct {
    display_state_t *displays;      /*!< Array of per-display state [num_displays] */
    sensor_state_t  *sensors;       /*!< Array of per-sensor state [num_sensors] */
    uint8_t num_displays;           /*!< Number of displays from config */
    uint8_t num_images;             /*!< Number of images from config */
    uint8_t num_sensors;            /*!< Number of sensors from config */
} display_ctx_t;

static display_ctx_t *s_ctx = NULL;

/** @brief File-scope mutex protecting s_ctx and all shared display/sensor state.
 *  Decoupled from s_ctx lifetime so the lock remains valid even after s_ctx is freed. */
static SemaphoreHandle_t s_lock = NULL;

/* ========================== Forward Declarations ========================== */

static void timed_display_timer_cb(void *arg);
static void build_error_response(ble_esl_cmd_result_t *result, uint8_t error_code);
static void build_display_state_response(ble_esl_cmd_result_t *result,
                                         uint8_t display_index, uint8_t image_index);
static bool is_image_available(uint8_t image_index);
static void restore_sensor_pending(uint8_t sensor_index, uint8_t transport);

/* ========================== Helper Functions ========================== */

/**
 * @brief Build an Error response (opcode 0x00) into the result structure
 */
static void build_error_response(ble_esl_cmd_result_t *result, uint8_t error_code)
{
    result->has_response = true;
    result->resp_opcode = BLE_ESL_RESP_ERROR;
    result->resp_params[0] = error_code;
    result->resp_params_len = 1;
}

/**
 * @brief Build a Display State response (opcode 0x11) into the result structure
 */
static void build_display_state_response(ble_esl_cmd_result_t *result,
                                         uint8_t display_index, uint8_t image_index)
{
    result->has_response = true;
    result->resp_opcode = BLE_ESL_RESP_DISPLAY_STATE;
    result->resp_params[0] = display_index;
    result->resp_params[1] = image_index;
    result->resp_params_len = 2;
}

/**
 * @brief Check if an image slot contains valid data
 *
 * An image is considered available if:
 * - The image_writable_mask is NULL: all images are read-only (static/pre-loaded)
 *   and therefore always available.
 * - The image_writable_mask entry is false: the image is read-only (protected)
 *   and therefore always available.
 * - The image_writable_mask entry is true: the image is writable and assumed
 *   to have been written by the AP.
 *
 * In other words, any valid image index within range is considered available.
 */
static bool is_image_available(uint8_t image_index)
{
    const ble_esl_config_t *config = esl_get_config();
    if (config == NULL) {
        return false;
    }
    if (image_index >= config->num_images) {
        return false;
    }
    /* All valid image indices are considered available:
     * - Read-only images (no writable mask, or mask[i] == false) are
     *   static/pre-loaded and always available.
     * - Writable images (mask[i] == true) are assumed written by the AP. */
    return true;
}

/**
 * @brief Restore sensor pending state after a failed response send
 *
 * Re-acquires the lock, validates that the context and sensor index are
 * still valid, and restores read_pending and transport_context so the
 * application can retry calling ble_esl_report_sensor_data().
 *
 * @param[in] sensor_index Index of the sensor to restore
 * @param[in] transport    Original transport context to restore
 */
static void restore_sensor_pending(uint8_t sensor_index, uint8_t transport)
{
    xSemaphoreTake(s_lock, portMAX_DELAY);
    if (s_ctx != NULL && sensor_index < s_ctx->num_sensors) {
        s_ctx->sensors[sensor_index].read_pending = true;
        s_ctx->sensors[sensor_index].transport_context = transport;
    }
    xSemaphoreGive(s_lock);
}

/* ========================== Timer Callback ========================== */

/**
 * @brief Timer callback for Display Timed Image execution
 *
 * Runs in timer task context — keeps work minimal: updates state and
 * fires the application callback.
 */
static void timed_display_timer_cb(void *arg)
{
    xSemaphoreTake(s_lock, portMAX_DELAY);

    if (s_ctx == NULL) {
        /* Context was freed while we waited for the lock or before — nothing to do. */
        xSemaphoreGive(s_lock);
        return;
    }

    /* arg encodes the display index */
    uint8_t display_index = (uint8_t)(uintptr_t)arg;
    if (display_index >= s_ctx->num_displays) {
        xSemaphoreGive(s_lock);
        return;
    }

    display_state_t *disp = &s_ctx->displays[display_index];
    if (!disp->timed_pending) {
        xSemaphoreGive(s_lock);
        return;
    }

    /* Update per-display current image tracking */
    disp->has_active_image = true;
    disp->current_image_index = disp->timed_image_index;
    uint8_t image_index = disp->current_image_index;

    /* Clear pending state */
    disp->timed_pending = false;

    xSemaphoreGive(s_lock);

    /* Fire BLE_ESL_EVT_DISPLAY_IMAGE callback outside the lock to avoid
     * potential deadlocks in application code that may call back into ESL. */
    ble_esl_cb_param_t param;
    memset(&param, 0, sizeof(param));
    param.display_image.display_index = display_index;
    param.display_image.image_index = image_index;
    esl_notify_app(BLE_ESL_EVT_DISPLAY_IMAGE, &param);

    ESP_LOGI(TAG, "Timed display executed: display=%u image=%u",
             display_index, image_index);
}

/* ========================== Init / Deinit ========================== */

esp_err_t esl_cmd_display_init(void)
{
    if (s_ctx != NULL) {
        ESP_LOGW(TAG, "Already initialized");
        return ESP_ERR_INVALID_STATE;
    }

    const ble_esl_config_t *config = esl_get_config();
    if (config == NULL) {
        ESP_LOGE(TAG, "Config not available");
        return ESP_ERR_INVALID_STATE;
    }

    /* Create the file-scope lock once (survives init/deinit cycles) */
    if (s_lock == NULL) {
        s_lock = xSemaphoreCreateMutex();
        if (s_lock == NULL) {
            ESP_LOGE(TAG, "Failed to create display module mutex");
            return ESP_ERR_NO_MEM;
        }
    }

    s_ctx = calloc(1, sizeof(display_ctx_t));
    if (s_ctx == NULL) {
        ESP_LOGE(TAG, "Failed to allocate display context");
        return ESP_ERR_NO_MEM;
    }

    s_ctx->num_displays = config->num_displays;
    s_ctx->num_images = config->num_images;
    s_ctx->num_sensors = config->num_sensors;

    /* Allocate per-display state */
    if (s_ctx->num_displays > 0) {
        s_ctx->displays = calloc(s_ctx->num_displays, sizeof(display_state_t));
        if (s_ctx->displays == NULL) {
            ESP_LOGE(TAG, "Failed to allocate display state array");
            free(s_ctx);
            s_ctx = NULL;
            return ESP_ERR_NO_MEM;
        }

        /* Create esp_timers for each display slot */
        for (uint8_t i = 0; i < s_ctx->num_displays; i++) {
            esp_timer_create_args_t timer_args = {
                .callback = timed_display_timer_cb,
                .arg = (void *)(uintptr_t)i,
                .dispatch_method = ESP_TIMER_TASK,
                .name = "esl_disp_timed",
            };
            esp_err_t ret = esp_timer_create(&timer_args, &s_ctx->displays[i].timer);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Failed to create timer for display %u: %s", i, esp_err_to_name(ret));
                /* Clean up previously created timers */
                for (uint8_t j = 0; j < i; j++) {
                    esp_timer_delete(s_ctx->displays[j].timer);
                }
                free(s_ctx->displays);
                free(s_ctx);
                s_ctx = NULL;
                return ret;
            }
        }
    }

    /* Allocate per-sensor state */
    if (s_ctx->num_sensors > 0) {
        s_ctx->sensors = calloc(s_ctx->num_sensors, sizeof(sensor_state_t));
        if (s_ctx->sensors == NULL) {
            ESP_LOGE(TAG, "Failed to allocate sensor state array");
            /* Clean up display timers */
            for (uint8_t i = 0; i < s_ctx->num_displays; i++) {
                esp_timer_delete(s_ctx->displays[i].timer);
            }
            free(s_ctx->displays);
            free(s_ctx);
            s_ctx = NULL;
            return ESP_ERR_NO_MEM;
        }
    }

    ESP_LOGI(TAG, "Display/sensor module initialized: displays=%u images=%u sensors=%u",
             s_ctx->num_displays, s_ctx->num_images, s_ctx->num_sensors);
    return ESP_OK;
}

void esl_cmd_display_deinit(void)
{
    if (s_lock == NULL) {
        return;
    }

    /* Acquire the file-scope lock before touching any s_ctx field so that the
     * timer stop/delete operations and the teardown of the state arrays are
     * serialised against concurrent callers (e.g., esl_get_display_pending,
     * esl_cmd_display_cancel_all, esl_cmd_display_timed_image) which also
     * operate on the same timer handles under this lock.
     * Holding the lock across esp_timer_delete() is safe: for ESP_TIMER_TASK
     * dispatch the delete is processed by the timer task, so it does not wait
     * inline for timed_display_timer_cb() (which itself takes s_lock). */
    xSemaphoreTake(s_lock, portMAX_DELAY);

    if (s_ctx == NULL) {
        xSemaphoreGive(s_lock);
        return;
    }

    /* Stop all display timers first to prevent new callback invocations.
     * esp_timer_stop only removes the timer from the armed list; an
     * already-dispatched callback may still be running — it will observe
     * s_ctx == NULL once it acquires the lock we hold here. */
    if (s_ctx->displays != NULL) {
        for (uint8_t i = 0; i < s_ctx->num_displays; i++) {
            if (s_ctx->displays[i].timer != NULL) {
                esp_timer_stop(s_ctx->displays[i].timer);
            }
        }
    }

    /* Delete all display timers. */
    if (s_ctx->displays != NULL) {
        for (uint8_t i = 0; i < s_ctx->num_displays; i++) {
            if (s_ctx->displays[i].timer != NULL) {
                esp_timer_delete(s_ctx->displays[i].timer);
                s_ctx->displays[i].timer = NULL;
            }
        }
    }

    /* Free display state */
    if (s_ctx->displays != NULL) {
        free(s_ctx->displays);
    }

    /* Free sensor state */
    if (s_ctx->sensors != NULL) {
        free(s_ctx->sensors);
    }

    free(s_ctx);
    s_ctx = NULL;

    xSemaphoreGive(s_lock);

    /* s_lock is intentionally NOT deleted — it is a file-scope static that
     * survives init/deinit cycles so that late callers safely see s_ctx == NULL
     * under the lock instead of accessing a deleted semaphore. */

    ESP_LOGI(TAG, "Display/sensor module deinitialized");
}

/* ========================== Command Handlers ========================== */

/**
 * @brief Consume cached PAwR sensor data and build the ESL response.
 *
 * Caller must hold s_lock. If valid cached data is present it is consumed
 * (the cache is cleared) and either a Sensor Value or an Error response is
 * built into @p result.
 *
 * @return true if cached data was consumed and a response was built.
 */
static bool serve_cached_sensor_locked(sensor_state_t *sensor,
                                       uint8_t sensor_index,
                                       ble_esl_cmd_result_t *result)
{
    if (!sensor->cache_valid) {
        return false;
    }

    uint8_t cached_error = sensor->cache_error_code;
    uint8_t cached_data_len = sensor->cache_data_len;

    /* Clear the cache */
    sensor->cache_valid = false;

    if (cached_error == 0) {
        /* Build Sensor Value response */
        uint8_t resp_opcode = (uint8_t)((cached_data_len << 4) | BLE_ESL_RESP_SENSOR_VALUE_TAG);
        result->has_response = true;
        result->resp_opcode = resp_opcode;
        result->resp_params[0] = sensor_index;
        memcpy(&result->resp_params[1], sensor->cache_data, cached_data_len);
        result->resp_params_len = cached_data_len + 1;
    } else {
        /* Build Error response with cached error code */
        build_error_response(result, cached_error);
    }

    return true;
}

esp_err_t esl_cmd_read_sensor(const uint8_t *params, uint8_t params_len,
                              ble_esl_cmd_result_t *result, uint8_t transport)
{
    assert(params != NULL);
    assert(result != NULL);

    xSemaphoreTake(s_lock, portMAX_DELAY);

    if (s_ctx == NULL) {
        ESP_LOGE(TAG, "Module not initialized");
        xSemaphoreGive(s_lock);
        build_error_response(result, BLE_ESL_ERR_UNSPECIFIED);
        return ESP_OK;
    }

    /* params[0] = Sensor_Index */
    uint8_t sensor_index = params[0];

    /* Validate Sensor_Index */
    if (sensor_index >= s_ctx->num_sensors) {
        ESP_LOGW(TAG, "Invalid sensor index: %u (max %u)", sensor_index, s_ctx->num_sensors);
        xSemaphoreGive(s_lock);
        build_error_response(result, BLE_ESL_ERR_INVALID_PARAMS);
        return ESP_OK;
    }

    sensor_state_t *sensor = &s_ctx->sensors[sensor_index];

    /* For PAwR transport, if cached sensor data from a previous read is
     * available, return it directly instead of issuing another Retry. */
    if (transport == ESL_TRANSPORT_PAWR && sensor->cache_valid) {
        uint8_t cached_error = sensor->cache_error_code;
        serve_cached_sensor_locked(sensor, sensor_index, result);
        xSemaphoreGive(s_lock);

        ESP_LOGI(TAG, "Sensor read served from cache (PAwR): index=%u error=0x%02x",
                 sensor_index, cached_error);
        return ESP_OK;
    }

    /* Mark sensor read as pending with transport context */
    sensor->read_pending = true;
    sensor->transport_context = transport;

    xSemaphoreGive(s_lock);

    /* Fire BLE_ESL_EVT_SENSOR_READ callback outside the lock */
    ble_esl_cb_param_t cb_param;
    memset(&cb_param, 0, sizeof(cb_param));
    cb_param.sensor_read.sensor_index = sensor_index;
    esl_notify_app(BLE_ESL_EVT_SENSOR_READ, &cb_param);

    if (transport == ESL_TRANSPORT_PAWR) {
        /* If the app reported the sensor data synchronously from within the
         * callback above, the cache is now valid — serve the Sensor Value in
         * this same response and avoid forcing the AP through a Retry +
         * re-poll round trip. */
        xSemaphoreTake(s_lock, portMAX_DELAY);
        /* Re-validate sensor_index: the module may have been deinitialised and
         * re-initialised with fewer sensors while the lock was released. */
        bool served = (s_ctx != NULL) && (sensor_index < s_ctx->num_sensors) &&
                      serve_cached_sensor_locked(&s_ctx->sensors[sensor_index],
                                                 sensor_index, result);
        xSemaphoreGive(s_lock);

        if (served) {
            ESP_LOGI(TAG, "Sensor read served synchronously (PAwR): index=%u",
                     sensor_index);
        } else {
            /* The read is still pending (app will report it later). Respond
             * with Retry so the AP re-issues the command in a subsequent
             * subevent, at which point the cached value is returned. */
            build_error_response(result, BLE_ESL_ERR_RETRY);
            ESP_LOGI(TAG, "Sensor read deferred (PAwR): index=%u, responding Retry",
                     sensor_index);
        }
    } else {
        /* ECP transport: response is deferred — will be sent when app
         * calls ble_esl_report_sensor_data() */
        result->has_response = false;
        ESP_LOGI(TAG, "Sensor read requested: index=%u transport=ECP", sensor_index);
    }

    return ESP_OK;
}

esp_err_t esl_cmd_refresh_display(const uint8_t *params, uint8_t params_len,
                                  ble_esl_cmd_result_t *result)
{
    assert(params != NULL);
    assert(result != NULL);

    xSemaphoreTake(s_lock, portMAX_DELAY);

    if (s_ctx == NULL) {
        ESP_LOGE(TAG, "Module not initialized");
        xSemaphoreGive(s_lock);
        build_error_response(result, BLE_ESL_ERR_UNSPECIFIED);
        return ESP_OK;
    }

    /* params[0] = Display_Index */
    uint8_t display_index = params[0];

    /* Validate Display_Index */
    if (display_index >= s_ctx->num_displays) {
        ESP_LOGW(TAG, "Invalid display index: %u (max %u)", display_index, s_ctx->num_displays);
        xSemaphoreGive(s_lock);
        build_error_response(result, BLE_ESL_ERR_INVALID_PARAMS);
        return ESP_OK;
    }

    /* Check that an image is currently being displayed */
    display_state_t *disp = &s_ctx->displays[display_index];
    if (!disp->has_active_image) {
        ESP_LOGW(TAG, "No active image on display %u", display_index);
        xSemaphoreGive(s_lock);
        build_error_response(result, BLE_ESL_ERR_IMAGE_NOT_AVAILABLE);
        return ESP_OK;
    }

    /* Save image index to local before releasing lock to avoid data race */
    uint8_t image_index = disp->current_image_index;

    /* Build Display State response */
    build_display_state_response(result, display_index, image_index);

    xSemaphoreGive(s_lock);

    /* Fire BLE_ESL_EVT_REFRESH_DISPLAY callback outside the lock */
    ble_esl_cb_param_t cb_param;
    memset(&cb_param, 0, sizeof(cb_param));
    cb_param.refresh_display.display_index = display_index;
    esl_notify_app(BLE_ESL_EVT_REFRESH_DISPLAY, &cb_param);

    ESP_LOGI(TAG, "Refresh display: index=%u image=%u", display_index, image_index);
    return ESP_OK;
}

esp_err_t esl_cmd_display_image(const uint8_t *params, uint8_t params_len,
                                ble_esl_cmd_result_t *result)
{
    assert(params != NULL);
    assert(result != NULL);

    xSemaphoreTake(s_lock, portMAX_DELAY);

    if (s_ctx == NULL) {
        ESP_LOGE(TAG, "Module not initialized");
        xSemaphoreGive(s_lock);
        build_error_response(result, BLE_ESL_ERR_UNSPECIFIED);
        return ESP_OK;
    }

    /* params[0] = Display_Index, params[1] = Image_Index */
    uint8_t display_index = params[0];
    uint8_t image_index = params[1];

    /* Validate Display_Index */
    if (display_index >= s_ctx->num_displays) {
        ESP_LOGW(TAG, "Invalid display index: %u (max %u)", display_index, s_ctx->num_displays);
        xSemaphoreGive(s_lock);
        build_error_response(result, BLE_ESL_ERR_INVALID_PARAMS);
        return ESP_OK;
    }

    /* Validate Image_Index range */
    if (image_index >= s_ctx->num_images) {
        ESP_LOGW(TAG, "Invalid image index: %u (max %u)", image_index, s_ctx->num_images);
        xSemaphoreGive(s_lock);
        build_error_response(result, BLE_ESL_ERR_INVALID_IMAGE_INDEX);
        return ESP_OK;
    }

    /* Check image availability */
    if (!is_image_available(image_index)) {
        ESP_LOGW(TAG, "Image not available: index=%u", image_index);
        xSemaphoreGive(s_lock);
        build_error_response(result, BLE_ESL_ERR_IMAGE_NOT_AVAILABLE);
        return ESP_OK;
    }

    /* Update internal state */
    display_state_t *disp = &s_ctx->displays[display_index];
    disp->has_active_image = true;
    disp->current_image_index = image_index;

    /* Build Display State response */
    build_display_state_response(result, display_index, image_index);

    xSemaphoreGive(s_lock);

    /* Fire BLE_ESL_EVT_DISPLAY_IMAGE callback outside the lock */
    ble_esl_cb_param_t cb_param;
    memset(&cb_param, 0, sizeof(cb_param));
    cb_param.display_image.display_index = display_index;
    cb_param.display_image.image_index = image_index;
    esl_notify_app(BLE_ESL_EVT_DISPLAY_IMAGE, &cb_param);

    ESP_LOGI(TAG, "Display image: display=%u image=%u", display_index, image_index);
    return ESP_OK;
}

esp_err_t esl_cmd_display_timed_image(const uint8_t *params, uint8_t params_len,
                                      ble_esl_cmd_result_t *result)
{
    assert(params != NULL);
    assert(result != NULL);

    xSemaphoreTake(s_lock, portMAX_DELAY);

    if (s_ctx == NULL) {
        ESP_LOGE(TAG, "Module not initialized");
        xSemaphoreGive(s_lock);
        build_error_response(result, BLE_ESL_ERR_UNSPECIFIED);
        return ESP_OK;
    }

    /* params[0] = Display_Index, params[1] = Image_Index, params[2..5] = Absolute_Time (LE) */
    uint8_t display_index = params[0];
    uint8_t image_index = params[1];
    uint32_t absolute_time = (uint32_t)params[2] |
                             ((uint32_t)params[3] << 8) |
                             ((uint32_t)params[4] << 16) |
                             ((uint32_t)params[5] << 24);

    /* Validate Display_Index */
    if (display_index >= s_ctx->num_displays) {
        ESP_LOGW(TAG, "Invalid display index: %u (max %u)", display_index, s_ctx->num_displays);
        xSemaphoreGive(s_lock);
        build_error_response(result, BLE_ESL_ERR_INVALID_PARAMS);
        return ESP_OK;
    }

    /* Validate Image_Index range */
    if (image_index >= s_ctx->num_images) {
        ESP_LOGW(TAG, "Invalid image index: %u (max %u)", image_index, s_ctx->num_images);
        xSemaphoreGive(s_lock);
        build_error_response(result, BLE_ESL_ERR_INVALID_IMAGE_INDEX);
        return ESP_OK;
    }

    /* Check image availability */
    if (!is_image_available(image_index)) {
        ESP_LOGW(TAG, "Image not available: index=%u", image_index);
        xSemaphoreGive(s_lock);
        build_error_response(result, BLE_ESL_ERR_IMAGE_NOT_AVAILABLE);
        return ESP_OK;
    }

    display_state_t *disp = &s_ctx->displays[display_index];

    /* Handle Absolute_Time = 0 (cancellation) */
    if (absolute_time == 0x00000000) {
        ESP_LOGI(TAG, "Cancelling timed display for display %u", display_index);
        if (disp->timed_pending) {
            esp_timer_stop(disp->timer);
            disp->timed_pending = false;
        }
        /* Send Display State response with command's Display_Index and Image_Index */
        build_display_state_response(result, display_index, image_index);
        xSemaphoreGive(s_lock);
        return ESP_OK;
    }

    /* Validate Absolute_Time plausibility */
    uint32_t current_time = esl_get_abs_time();
    uint32_t delta;
    if (absolute_time >= current_time) {
        delta = absolute_time - current_time;
    } else {
        /* Wrap-around: time until wrap + time after wrap */
        delta = (0xFFFFFFFF - current_time) + absolute_time + 1;
    }

    if (delta > BLE_ESL_IMPLAUSIBLE_TIME_MS) {
        ESP_LOGW(TAG, "Implausible absolute time: target=%lu current=%lu delta=%lu",
                 (unsigned long)absolute_time, (unsigned long)current_time, (unsigned long)delta);
        xSemaphoreGive(s_lock);
        build_error_response(result, BLE_ESL_ERR_IMPLAUSIBLE_ABS_TIME);
        return ESP_OK;
    }

    /* Handle pending timed command conflict */
    if (disp->timed_pending) {
        if (absolute_time == disp->timed_absolute_time) {
            /* Same time → replace the pending command (update Image_Index) */
            ESP_LOGI(TAG, "Replacing timed display for display %u: new image=%u",
                     display_index, image_index);
            disp->timed_image_index = image_index;
            /* Timer is already running for the same time, no need to restart */
            build_display_state_response(result, display_index, image_index);
            xSemaphoreGive(s_lock);
            return ESP_OK;
        } else {
            /* Different time → Queue Full error */
            ESP_LOGW(TAG, "Queue full for display %u: pending_time=%lu new_time=%lu",
                     display_index, (unsigned long)disp->timed_absolute_time,
                     (unsigned long)absolute_time);
            xSemaphoreGive(s_lock);
            build_error_response(result, BLE_ESL_ERR_QUEUE_FULL);
            return ESP_OK;
        }
    }

    /* Schedule timed execution */
    disp->timed_pending = true;
    disp->timed_image_index = image_index;
    disp->timed_absolute_time = absolute_time;

    /* Convert delta from ms to us for esp_timer */
    uint64_t timeout_us = (uint64_t)delta * 1000ULL;
    if (timeout_us == 0) {
        /* If delta is 0 (target time is now), use a minimal timeout */
        timeout_us = 1;
    }

    esp_err_t ret = esp_timer_start_once(disp->timer, timeout_us);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start timed display timer: %s", esp_err_to_name(ret));
        disp->timed_pending = false;
        xSemaphoreGive(s_lock);
        build_error_response(result, BLE_ESL_ERR_UNSPECIFIED);
        return ESP_OK;
    }

    ESP_LOGI(TAG, "Timed display scheduled: display=%u image=%u time=%lu delta=%lu ms",
             display_index, image_index, (unsigned long)absolute_time, (unsigned long)delta);

    /* Send Display State response immediately */
    build_display_state_response(result, display_index, image_index);
    xSemaphoreGive(s_lock);
    return ESP_OK;
}

/* ========================== Pending Display Update Flag ========================== */

bool esl_get_display_pending(void)
{
    if (s_lock == NULL) {
        return false;
    }

    xSemaphoreTake(s_lock, portMAX_DELAY);

    if (s_ctx == NULL || s_ctx->displays == NULL) {
        xSemaphoreGive(s_lock);
        return false;
    }

    bool pending = false;
    for (uint8_t i = 0; i < s_ctx->num_displays; i++) {
        if (s_ctx->displays[i].timed_pending) {
            pending = true;
            break;
        }
    }

    xSemaphoreGive(s_lock);
    return pending;
}

/* ========================== Cancel All Timed Display Commands ========================== */

void esl_cmd_display_cancel_all(void)
{
    if (s_lock == NULL) {
        return;
    }

    xSemaphoreTake(s_lock, portMAX_DELAY);

    if (s_ctx == NULL || s_ctx->displays == NULL) {
        xSemaphoreGive(s_lock);
        return;
    }

    for (uint8_t i = 0; i < s_ctx->num_displays; i++) {
        if (s_ctx->displays[i].timed_pending) {
            esp_timer_stop(s_ctx->displays[i].timer);
            s_ctx->displays[i].timed_pending = false;
            ESP_LOGI(TAG, "Cancelled timed display for display %u", i);
        }
    }

    xSemaphoreGive(s_lock);
}

/* ========================== Sensor Data Report (Public API) ========================== */

esp_err_t ble_esl_report_sensor_data(uint8_t sensor_index, uint8_t error_code,
                                     const uint8_t *data, uint8_t data_len)
{
    xSemaphoreTake(s_lock, portMAX_DELAY);

    if (s_ctx == NULL) {
        ESP_LOGE(TAG, "Module not initialized");
        xSemaphoreGive(s_lock);
        return ESP_ERR_INVALID_STATE;
    }

    /* Validate sensor_index */
    if (sensor_index >= s_ctx->num_sensors) {
        ESP_LOGE(TAG, "Invalid sensor index: %u", sensor_index);
        xSemaphoreGive(s_lock);
        return ESP_ERR_INVALID_ARG;
    }

    /* Check that a read is pending for this sensor */
    sensor_state_t *sensor = &s_ctx->sensors[sensor_index];
    if (!sensor->read_pending) {
        ESP_LOGE(TAG, "No sensor read pending for index %u", sensor_index);
        xSemaphoreGive(s_lock);
        return ESP_ERR_INVALID_STATE;
    }

    /* Validate data parameters when error_code is 0 (success) */
    if (error_code == 0) {
        if (data == NULL) {
            ESP_LOGE(TAG, "Sensor data pointer is NULL");
            xSemaphoreGive(s_lock);
            return ESP_ERR_INVALID_ARG;
        }
        if (data_len == 0 || data_len > SENSOR_DATA_CACHE_MAX_LEN) {
            ESP_LOGE(TAG, "Invalid sensor data length: %u (must be 1-%u)",
                     data_len, SENSOR_DATA_CACHE_MAX_LEN);
            xSemaphoreGive(s_lock);
            return ESP_ERR_INVALID_ARG;
        }
    }

    uint8_t transport = sensor->transport_context;

    /* Clear pending state */
    sensor->read_pending = false;

    if (transport == ESL_TRANSPORT_PAWR) {
        /* PAwR transport: cache the sensor data so that when the AP
         * re-issues the Read Sensor Data command in a subsequent PAwR
         * subevent, the cached data can be returned directly instead
         * of another Retry. */
        sensor->cache_valid = true;
        sensor->cache_error_code = error_code;
        if (error_code == 0) {
            memcpy(sensor->cache_data, data, data_len);
            sensor->cache_data_len = data_len;
        } else {
            sensor->cache_data_len = 0;
        }

        xSemaphoreGive(s_lock);

        ESP_LOGI(TAG, "PAwR sensor data cached (index=%u error=0x%02x), awaiting AP re-poll",
                 sensor_index, error_code);
        return ESP_OK;
    }

    xSemaphoreGive(s_lock);

    /* ECP transport: build and send the response immediately */
    uint8_t tlv_buf[BLE_ESL_TLV_MAX_SIZE];
    uint8_t tlv_len = 0;

    if (error_code == 0) {
        /* Build Sensor Value response:
         * Opcode = (data_len << 4) | 0x0E
         * Params = [sensor_index, data[0], data[1], ..., data[data_len-1]]
         */
        uint8_t resp_opcode = (uint8_t)((data_len << 4) | BLE_ESL_RESP_SENSOR_VALUE_TAG);
        uint8_t resp_params[16]; /* max 1 + 15 = 16 */
        resp_params[0] = sensor_index;
        memcpy(&resp_params[1], data, data_len);
        uint8_t resp_params_len = data_len + 1;

        esp_err_t ret = ble_esl_tlv_encode(resp_opcode, resp_params, resp_params_len,
                                           tlv_buf, &tlv_len);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to encode sensor value TLV: %s", esp_err_to_name(ret));
            restore_sensor_pending(sensor_index, transport);
            return ret;
        }

        ESP_LOGI(TAG, "Sensor value response: index=%u data_len=%u", sensor_index, data_len);
    } else {
        /* Build Error response: opcode 0x00, params = [error_code] */
        uint8_t resp_params[1] = { error_code };
        esp_err_t ret = ble_esl_tlv_encode(BLE_ESL_RESP_ERROR, resp_params, 1,
                                           tlv_buf, &tlv_len);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to encode error TLV: %s", esp_err_to_name(ret));
            restore_sensor_pending(sensor_index, transport);
            return ret;
        }

        ESP_LOGW(TAG, "Sensor error response: index=%u error=0x%02x", sensor_index, error_code);
    }

    /* Send response via ECP */
    esp_err_t ret = esl_send_ecp_response(tlv_buf, tlv_len);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to send ECP sensor response: %s", esp_err_to_name(ret));
        restore_sensor_pending(sensor_index, transport);
        return ret;
    }

    return ESP_OK;
}
