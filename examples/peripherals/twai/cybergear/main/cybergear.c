/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_log.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_twai.h"
#include "esp_twai_onchip.h"
#include "cybergear.h"

#define TAG "Cybergear"

typedef struct {
    union {
        uint8_t byte[8];
        uint16_t half_word[4];
        uint32_t word[2];
        float float_data[2];
    };
} __attribute__((aligned(4))) cybergear_data_t;

typedef struct cybergear_t {
    twai_node_handle_t node_hdl;
    uint8_t motor_id;
    uint8_t esp_node_id;
    uint8_t mode;
    uint16_t raw_location;
    uint16_t raw_speed;
    uint16_t raw_torque;
    uint16_t raw_temperature;
    union {
        struct {
            uint8_t under_voltage: 1;
            uint8_t over_current: 1;
            uint8_t over_heating: 1;
            uint8_t magnetic_coding_fault: 1;
            uint8_t hall_coding_fault: 1;
            uint8_t uncalibrated: 1;
            uint8_t state: 2;
        } bit;
        uint8_t raw;
    } status;
    SemaphoreHandle_t receive_sem;
    SemaphoreHandle_t broadcast_sem;
} cybergear_t;

static inline uint32_t map_float_to_uint(float x, float x_min, float x_max, uint32_t bits)
{
    if (x_max == x_min) {
        return 0;
    }
    x = x < x_min ? x_min : x;
    x = x > x_max ? x_max : x;
    float span = x_max - x_min;
    float normalized = (x - x_min) / span;
    uint32_t max_val = (bits >= 32) ? UINT32_MAX : ((1UL << bits) - 1);
    return (uint32_t)(normalized * max_val + 0.5f);
}

static inline float map_uint_to_float(uint32_t x, float x_min, float x_max, uint32_t bits)
{
    if (x_max == x_min) {
        return x_min;
    }
    if (bits == 0 || bits > 32) {
        return x_min;
    }
    uint32_t in_max = (1U << bits) - 1;
    return ((float)x / (float)in_max) * (x_max - x_min) + x_min;
}

static IRAM_ATTR bool cybergear_rx_done_cb(twai_node_handle_t handle, const twai_rx_done_event_data_t *edata, void *user_ctx)
{
    cybergear_t *motor = (cybergear_t *)user_ctx;
    uint8_t recv_buff[TWAI_FRAME_MAX_LEN];
    twai_frame_t rx_frame = {
        .buffer = recv_buff,
        .buffer_len = sizeof(recv_buff),
    };
    BaseType_t high_task_woken = pdFALSE;

    twai_node_receive_from_isr(handle, &rx_frame);

    ESP_EARLY_LOGV(TAG, "rx_frame.header.id: 0x%x, rx_frame.buffer: %02x %02x %02x %02x %02x %02x %02x %02x", rx_frame.header.id,
                   rx_frame.buffer[0], rx_frame.buffer[1], rx_frame.buffer[2], rx_frame.buffer[3],
                   rx_frame.buffer[4], rx_frame.buffer[5], rx_frame.buffer[6], rx_frame.buffer[7]);

    if (rx_frame.header.id >> 24 == CYBERGEAR_CMD_FEEDBACK_STATUS) {
        motor->status.raw = (rx_frame.header.id >> 16) & 0xFF;
        motor->raw_location = rx_frame.buffer[0] << 8 | rx_frame.buffer[1];
        motor->raw_speed = rx_frame.buffer[2] << 8 | rx_frame.buffer[3];
        motor->raw_torque = rx_frame.buffer[4] << 8 | rx_frame.buffer[5];
        motor->raw_temperature = rx_frame.buffer[6] << 8 | rx_frame.buffer[7];
        xSemaphoreGiveFromISR(motor->receive_sem, &high_task_woken);
    } else if (rx_frame.header.id >> 24 == CYBERGEAR_CMD_BROADCAST) {
        motor->motor_id = rx_frame.header.id >> 8 & 0xFF;
        xSemaphoreGiveFromISR(motor->broadcast_sem, &high_task_woken);
    }

    return high_task_woken;
}

static esp_err_t cybergear_send_twai_package(cybergear_t *motor, uint32_t message_id, cybergear_data_t *message_data)
{
    // Clear the semaphore
    xSemaphoreTake(motor->receive_sem, 0);
    twai_frame_t frame = {
        .header = {
            .id = message_id,
            .dlc = sizeof(cybergear_data_t),
            .ide = 1,
        },
        .buffer = (uint8_t *)message_data,
        .buffer_len = sizeof(cybergear_data_t),
    };

    ESP_RETURN_ON_ERROR(twai_node_transmit(motor->node_hdl, &frame, pdMS_TO_TICKS(CYBERGEAR_MOTOR_TIMEOUT)), TAG, "failed to send twai package");
    // Wait for the status update
    if (xSemaphoreTake(motor->receive_sem, pdMS_TO_TICKS(CYBERGEAR_MOTOR_TIMEOUT)) != pdTRUE) {
        ESP_LOGW(TAG, "timeout waiting for response");
        return ESP_ERR_TIMEOUT;
    }

    return ESP_OK;
}

esp_err_t cybergear_enable(cybergear_t *motor)
{
    ESP_RETURN_ON_FALSE(motor, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    uint32_t message_id = CYBERGEAR_CMD_ENABLE << 24 | motor->esp_node_id << 8 | motor->motor_id;
    cybergear_data_t message_data = {0};

    return cybergear_send_twai_package(motor, message_id, &message_data);
}

esp_err_t cybergear_disable(cybergear_t *motor, bool clear_fault)
{
    ESP_RETURN_ON_FALSE(motor, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    uint32_t message_id = CYBERGEAR_CMD_DISABLE << 24 | motor->esp_node_id << 8 | motor->motor_id;
    cybergear_data_t message_data = {0};
    message_data.byte[0] = clear_fault;

    return cybergear_send_twai_package(motor, message_id, &message_data);
}

esp_err_t cybergear_control_motion(cybergear_t *motor, const cybergear_motion_control_t *motion_control)
{
    ESP_RETURN_ON_FALSE(motor && motion_control, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    uint16_t torque = map_float_to_uint(motion_control->torque, CYBERGEAR_TORQUE_MIN, CYBERGEAR_TORQUE_MAX, 16);
    uint16_t location = map_float_to_uint(motion_control->location, CYBERGEAR_LOCATION_MIN, CYBERGEAR_LOCATION_MAX, 16);
    uint16_t speed = map_float_to_uint(motion_control->speed, CYBERGEAR_SPEED_MIN, CYBERGEAR_SPEED_MAX, 16);
    uint16_t kp = map_float_to_uint(motion_control->kp, CYBERGEAR_KP_MIN, CYBERGEAR_KP_MAX, 16);
    uint16_t kd = map_float_to_uint(motion_control->kd, CYBERGEAR_KD_MIN, CYBERGEAR_KD_MAX, 16);

    uint32_t message_id = CYBERGEAR_CMD_MOTION_CONTROL << 24 | torque << 8 | motor->motor_id;
    cybergear_data_t message_data = {0};

    // cybergear use big-endian here
    message_data.byte[0] = location >> 8;
    message_data.byte[1] = location & 0xFF;
    message_data.byte[2] = speed >> 8;
    message_data.byte[3] = speed & 0xFF;
    message_data.byte[4] = kp >> 8;
    message_data.byte[5] = kp & 0xFF;
    message_data.byte[6] = kd >> 8;
    message_data.byte[7] = kd & 0xFF;

    return cybergear_send_twai_package(motor, message_id, &message_data);
}

esp_err_t cybergear_set_mode(cybergear_t *motor, cybergear_mode_t mode)
{
    ESP_RETURN_ON_FALSE(motor, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(mode < MODE_MAX, ESP_ERR_INVALID_ARG, TAG, "mode must be 0-3, 0:Motion, 1:Location, 2:Speed, 3:Current");

    uint32_t message_id = CYBERGEAR_CMD_SET_PARA << 24 | motor->esp_node_id << 8 | motor->motor_id;
    cybergear_data_t message_data = {0};
    message_data.half_word[0] = CYBERGEAR_ADDR_RUN_MODE;
    message_data.byte[4] = mode;
    return cybergear_send_twai_package(motor, message_id, &message_data);
}

esp_err_t cybergear_set_location(cybergear_t *motor, float location)
{
    ESP_RETURN_ON_FALSE(motor, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    uint32_t message_id = CYBERGEAR_CMD_SET_PARA << 24 | motor->esp_node_id << 8 | motor->motor_id;
    cybergear_data_t message_data = {0};
    message_data.half_word[0] = CYBERGEAR_ADDR_LOCATION_REF;
    message_data.float_data[1] = location;
    return cybergear_send_twai_package(motor, message_id, &message_data);
}

esp_err_t cybergear_set_speed(cybergear_t *motor, float speed)
{
    ESP_RETURN_ON_FALSE(motor, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    uint32_t message_id = CYBERGEAR_CMD_SET_PARA << 24 | motor->esp_node_id << 8 | motor->motor_id;
    cybergear_data_t message_data = {0};
    message_data.half_word[0] = CYBERGEAR_ADDR_SPEED_REF;
    message_data.float_data[1] = speed;
    return cybergear_send_twai_package(motor, message_id, &message_data);
}

esp_err_t cybergear_set_speed_limit(cybergear_t *motor, float speed_limit)
{
    ESP_RETURN_ON_FALSE(motor, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    uint32_t message_id = CYBERGEAR_CMD_SET_PARA << 24 | motor->esp_node_id << 8 | motor->motor_id;
    cybergear_data_t message_data = {0};
    message_data.half_word[0] = CYBERGEAR_ADDR_LIMIT_SPEED;
    message_data.float_data[1] = speed_limit;
    return cybergear_send_twai_package(motor, message_id, &message_data);
}

esp_err_t cybergear_set_current(cybergear_t *motor, float current)
{
    ESP_RETURN_ON_FALSE(motor, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    uint32_t message_id = CYBERGEAR_CMD_SET_PARA << 24 | motor->esp_node_id << 8 | motor->motor_id;
    cybergear_data_t message_data = {0};
    message_data.half_word[0] = CYBERGEAR_ADDR_CURRENT_REF;
    message_data.float_data[1] = current;
    return cybergear_send_twai_package(motor, message_id, &message_data);
}

esp_err_t cybergear_set_current_limit(cybergear_t *motor, float current_limit)
{
    ESP_RETURN_ON_FALSE(motor, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    uint32_t message_id = CYBERGEAR_CMD_SET_PARA << 24 | motor->esp_node_id << 8 | motor->motor_id;
    cybergear_data_t message_data = {0};
    message_data.half_word[0] = CYBERGEAR_ADDR_LIMIT_CURRENT;
    message_data.float_data[1] = current_limit;
    return cybergear_send_twai_package(motor, message_id, &message_data);
}

esp_err_t cybergear_reset_zero_location(cybergear_t *motor)
{
    ESP_RETURN_ON_FALSE(motor, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    uint32_t message_id = CYBERGEAR_CMD_SET_LOCATION_TO_ZERO << 24 | motor->esp_node_id << 8 | motor->motor_id;
    cybergear_data_t message_data = {0};
    message_data.byte[0] = 1;
    return cybergear_send_twai_package(motor, message_id, &message_data);
}

esp_err_t cybergear_set_motor_id(cybergear_t *motor, uint8_t new_motor_id)
{
    ESP_RETURN_ON_FALSE(motor, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    uint32_t message_id = CYBERGEAR_CMD_SET_MOTOR_ID << 24 | new_motor_id << 16 | motor->esp_node_id << 8 | motor->motor_id;
    cybergear_data_t message_data = {0};
    esp_err_t ret = ESP_OK;

    // re-configure the filter
    ESP_RETURN_ON_ERROR(twai_node_disable(motor->node_hdl), TAG, "failed to disable node");
    twai_mask_filter_config_t mask_filter = {
        .id = (new_motor_id << 8 | motor->esp_node_id),
        .mask = 0xFF,
        .is_ext = 1,
    };
    ESP_RETURN_ON_ERROR(twai_node_config_mask_filter(motor->node_hdl, 0, &mask_filter), TAG, "failed to configure filter");
    ESP_RETURN_ON_ERROR(twai_node_enable(motor->node_hdl), TAG, "failed to enable node");
    ESP_GOTO_ON_ERROR(cybergear_send_twai_package(motor, message_id, &message_data), err, TAG, "failed to send twai package");
    motor->motor_id = new_motor_id;
    return ret;
err:
    // resume the origin filter
    twai_node_disable(motor->node_hdl);
    mask_filter.id = motor->motor_id << 8 | motor->esp_node_id;
    twai_node_config_mask_filter(motor->node_hdl, 0, &mask_filter);
    twai_node_enable(motor->node_hdl);
    return ret;
}

esp_err_t cybergear_get_motor_id(cybergear_t *motor, uint8_t *ret_motor_id)
{
    ESP_RETURN_ON_FALSE(motor, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    // Clear the semaphore
    xSemaphoreTake(motor->broadcast_sem, 0);
    ESP_LOGI(TAG, "Please power off the motor and power it on again, then it will broadcast its motor ID");
    // Wait for the status update, first broadcast is default motor ID, and the second broadcast is the real motor ID set by user
    xSemaphoreTake(motor->broadcast_sem, portMAX_DELAY);
    xSemaphoreTake(motor->broadcast_sem, portMAX_DELAY);

    if (ret_motor_id) {
        *ret_motor_id = motor->motor_id;
    }
    return ESP_OK;
}

esp_err_t cybergear_get_parameter(cybergear_t *motor, uint16_t para_index)
{
    ESP_RETURN_ON_FALSE(motor, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    uint32_t message_id = CYBERGEAR_CMD_READ_PARA << 24 | motor->esp_node_id << 8 | motor->motor_id;
    cybergear_data_t message_data = {0};
    message_data.half_word[0] = para_index;
    return cybergear_send_twai_package(motor, message_id, &message_data);
}

esp_err_t cybergear_get_status(cybergear_t *motor, cybergear_status_t *status)
{
    ESP_RETURN_ON_FALSE(motor && status, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    uint32_t message_id = CYBERGEAR_CMD_FEEDBACK_STATUS << 24 | motor->esp_node_id << 8 | motor->motor_id;
    cybergear_data_t message_data = {0};
    ESP_RETURN_ON_ERROR(cybergear_send_twai_package(motor, message_id, &message_data), TAG, "failed to send twai package");

    status->motor_id = motor->motor_id;
    status->fault = motor->status.raw & 0x3F;
    status->state = motor->status.raw >> 6;
    status->location = map_uint_to_float(motor->raw_location, CYBERGEAR_LOCATION_MIN, CYBERGEAR_LOCATION_MAX, 16);
    status->speed = map_uint_to_float(motor->raw_speed, CYBERGEAR_SPEED_MIN, CYBERGEAR_SPEED_MAX, 16);
    status->torque = map_uint_to_float(motor->raw_torque, CYBERGEAR_TORQUE_MIN, CYBERGEAR_TORQUE_MAX, 16);
    status->temperature = motor->raw_temperature / 10.0f;
    return ESP_OK;
}

esp_err_t cybergear_del(cybergear_t *motor)
{
    ESP_RETURN_ON_FALSE(motor, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    if (motor->receive_sem) {
        vSemaphoreDelete(motor->receive_sem);
    }
    if (motor->broadcast_sem) {
        vSemaphoreDelete(motor->broadcast_sem);
    }
    if (motor->node_hdl) {
        ESP_RETURN_ON_ERROR(twai_node_disable(motor->node_hdl), TAG, "failed to disable node");
        ESP_RETURN_ON_ERROR(twai_node_delete(motor->node_hdl), TAG, "failed to delete node");
    }
    free(motor);
    return ESP_OK;
}

esp_err_t cybergear_new_twai_motor(twai_node_handle_t twai_node, const cybergear_config_t *motor_config, cybergear_handle_t *ret_motor)
{
    esp_err_t ret = ESP_OK;
    cybergear_t *motor = NULL;
    ESP_GOTO_ON_FALSE(twai_node && motor_config && ret_motor, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    motor = calloc(1, sizeof(cybergear_t));
    ESP_GOTO_ON_FALSE(motor, ESP_ERR_NO_MEM, err, TAG, "no memory for motor");

    motor->receive_sem = xSemaphoreCreateBinary();
    ESP_GOTO_ON_FALSE(motor->receive_sem, ESP_ERR_NO_MEM, err, TAG, "no memory for status update semaphore");
    motor->broadcast_sem = xSemaphoreCreateBinary();
    ESP_GOTO_ON_FALSE(motor->broadcast_sem, ESP_ERR_NO_MEM, err, TAG, "no memory for broadcast semaphore");
    motor->node_hdl = twai_node;
    // Register RX done callbacks
    twai_event_callbacks_t cbs = {
        .on_rx_done = cybergear_rx_done_cb,
    };
    ESP_GOTO_ON_ERROR(twai_node_register_event_callbacks(motor->node_hdl, &cbs, motor), err, TAG, "failed to register callbacks");

    // configure the filter, cybergear only support ID-extended frame
    twai_mask_filter_config_t mask_filter = {
        .id = (motor_config->motor_id << 8 | motor_config->esp_node_id),
        .mask = 0xFF,
        .is_ext = 1,
    };
    ESP_GOTO_ON_ERROR(twai_node_config_mask_filter(motor->node_hdl, 0, &mask_filter), err, TAG, "failed to configure filter");

    ESP_GOTO_ON_ERROR(twai_node_enable(motor->node_hdl), err, TAG, "failed to enable node");

    motor->motor_id = motor_config->motor_id;
    motor->esp_node_id = motor_config->esp_node_id;

    *ret_motor = motor;
    return ESP_OK;

err:
    if (motor) {
        if (motor->node_hdl) {
            twai_node_delete(motor->node_hdl);
        }
        if (motor->receive_sem) {
            vSemaphoreDelete(motor->receive_sem);
        }
        if (motor->broadcast_sem) {
            vSemaphoreDelete(motor->broadcast_sem);
        }
        free(motor);
    }

    return ret;
}
