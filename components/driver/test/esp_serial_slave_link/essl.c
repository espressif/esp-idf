/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "essl.h"
#include "essl_internal.h"
#include "esp_log.h"
#include "freertos/task.h"

#define TIME_EXPIRED_SINCE_CORE(start, end, timeout, max) (bool)((end)>=(start)? \
         ((end)-(start)>(timeout)) :\
        ((max)-(timeout)>(start)-(end)))

#define TIME_EXPIRED_SINCE(start, end, timeout) TIME_EXPIRED_SINCE_CORE(start, end, timeout, UINT32_MAX)

#define MINUS_UNTIL_ZERO(a, b) ( ((a) > (b)) ? ((a)-(b)): 0)

#define TIME_REMAIN_CORE(start, end, timeout, max) ((end)>=(start)?\
        MINUS_UNTIL_ZERO(timeout, (end)-(start)):\
        MINUS_UNTIL_ZERO((start)-(end), (max)-(timeout)))

#define TIME_REMAIN(start, end, timeout)    TIME_REMAIN_CORE(start, end, timeout, UINT32_MAX)


#define ESSL_MIN(a, b)   ((a) < (b) ? (a) : (b))

__attribute__((unused)) static const char TAG[] = "esp_serial_slave_link";

#define _CHECK_EXECUTE_CMD(DEV, CMD, STR, ...) do{ \
    if ((DEV) == NULL) { \
        return ESP_ERR_INVALID_ARG; \
    } \
    if ((DEV)->CMD) { \
        return (DEV)->CMD((DEV)->args,##__VA_ARGS__); \
    } else { \
        ESP_LOGE(TAG, STR); \
        return ESP_ERR_NOT_SUPPORTED; \
    } } while(0)

#define CHECK_EXECUTE_CMD(DEV, CMD, ...) _CHECK_EXECUTE_CMD(DEV, CMD, #CMD" not supported for the current device.",##__VA_ARGS__)


esp_err_t essl_init(essl_handle_t handle, uint32_t wait_ms)
{
    CHECK_EXECUTE_CMD(handle, init, wait_ms);
}

esp_err_t essl_wait_for_ready(essl_handle_t handle, uint32_t wait_ms)
{
    CHECK_EXECUTE_CMD(handle, wait_for_ready, wait_ms);
}

esp_err_t essl_send_packet(essl_handle_t handle, const void *start, size_t length, uint32_t wait_ms)
{
    if (handle == NULL || start == NULL || length == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    if (handle->send_packet == NULL) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    esp_err_t err;
    const uint32_t timeout_ticks = pdMS_TO_TICKS(wait_ms);

    uint32_t pre = xTaskGetTickCount();
    uint32_t now;
    uint32_t remain_wait_ms = 0;

    do {
        now = xTaskGetTickCount();
        remain_wait_ms = pdTICKS_TO_MS(TIME_REMAIN(pre, now, timeout_ticks));
        err = handle->send_packet(handle->args, start, length, remain_wait_ms);
        if (err == ESP_OK) {
            break;
        } else if (err != ESP_ERR_NOT_FOUND) {
            return err;
        } // else ESP_ERR_NOT_FOUND
        //the slave is not ready, retry
    } while (remain_wait_ms > 0);
    return err;
}

esp_err_t essl_get_packet(essl_handle_t handle, void *out_data, size_t size, size_t *out_length, uint32_t wait_ms)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (out_data == NULL || size == 0 || out_length == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (handle->get_packet == NULL || handle->update_rx_data_size == NULL || handle->get_rx_data_size == NULL) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    esp_err_t err;
    const uint32_t timeout_ticks = pdMS_TO_TICKS(wait_ms);

    uint32_t pre = xTaskGetTickCount();
    uint32_t now = 3;
    uint32_t wait_remain_ms = 0;
    int data_available = handle->get_rx_data_size(handle->args);

    // if there is already enough data to read, skip the length update.
    if (data_available < size) {
        //loop until timeout, or there is at least one byte
        do {
            now = xTaskGetTickCount();
            wait_remain_ms = pdTICKS_TO_MS(TIME_REMAIN(pre, now, timeout_ticks));
            err = handle->update_rx_data_size(handle->args, wait_remain_ms);
            if (err != ESP_OK) {
                return err;
            }
            data_available = handle->get_rx_data_size(handle->args);
            if (data_available > 0) {
                break;
            }
        } while (wait_remain_ms > 0);
    }

    if (data_available == 0) {
        //the slave has no data to send
        return ESP_ERR_NOT_FOUND;
    }

    int len = ESSL_MIN(data_available, size);
    now = xTaskGetTickCount();
    wait_remain_ms = pdTICKS_TO_MS(TIME_REMAIN(pre, now, timeout_ticks));
    err = handle->get_packet(handle->args, out_data, len, wait_remain_ms);
    if (err != ESP_OK) {
        return err;
    }

    *out_length = len;
    if (len < data_available) {
        return ESP_ERR_NOT_FINISHED;
    }
    return ESP_OK;
}

esp_err_t essl_get_tx_buffer_num(essl_handle_t handle, uint32_t *out_tx_num, uint32_t wait_ms)
{
    if (handle == NULL || out_tx_num == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (handle->update_tx_buffer_num == NULL|| handle->get_tx_buffer_num == NULL) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    esp_err_t err = handle->update_tx_buffer_num(handle->args, wait_ms);
    if (err != ESP_OK) {
        return err;
    }

    *out_tx_num = handle->get_tx_buffer_num(handle->args);
    return ESP_OK;
}

esp_err_t essl_get_rx_data_size(essl_handle_t handle, uint32_t *out_rx_size, uint32_t wait_ms)
{
    if (handle == NULL || out_rx_size == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (handle->update_rx_data_size == NULL|| handle->get_rx_data_size == NULL) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    esp_err_t err = handle->update_rx_data_size(handle->args, wait_ms);
    if (err != ESP_OK) {
        return err;
    }

    *out_rx_size = handle->get_rx_data_size(handle->args);
    return ESP_OK;
}

esp_err_t essl_write_reg(essl_handle_t handle, uint8_t addr, uint8_t value, uint8_t *value_o, uint32_t wait_ms)
{
    CHECK_EXECUTE_CMD(handle, write_reg, addr, value, value_o, wait_ms);
}

esp_err_t essl_read_reg(essl_handle_t handle, uint8_t add, uint8_t *value_o, uint32_t wait_ms)
{
    CHECK_EXECUTE_CMD(handle, read_reg, add, value_o, wait_ms);
}

esp_err_t essl_wait_int(essl_handle_t handle, TickType_t wait_ms)
{
    CHECK_EXECUTE_CMD(handle, wait_int, wait_ms);
}

esp_err_t essl_reset_cnt(essl_handle_t handle)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (handle->reset_cnt == NULL) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    handle->reset_cnt(handle->args);
    return ESP_OK;
}

esp_err_t essl_clear_intr(essl_handle_t handle, uint32_t intr_mask, uint32_t wait_ms)
{
    CHECK_EXECUTE_CMD(handle, clear_intr, intr_mask, wait_ms);
}

esp_err_t essl_get_intr(essl_handle_t handle, uint32_t *intr_raw, uint32_t *intr_st, uint32_t wait_ms)
{
    if (intr_raw == NULL && intr_st == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    CHECK_EXECUTE_CMD(handle, get_intr, intr_raw, intr_st, wait_ms);
}

esp_err_t essl_set_intr_ena(essl_handle_t handle, uint32_t ena_mask, uint32_t wait_ms)
{
    CHECK_EXECUTE_CMD(handle, set_intr_ena, ena_mask, wait_ms);
}

esp_err_t essl_get_intr_ena(essl_handle_t handle, uint32_t *ena_mask_o, uint32_t wait_ms)
{
    if (ena_mask_o == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    CHECK_EXECUTE_CMD(handle, get_intr_ena, ena_mask_o, wait_ms);
}

esp_err_t essl_send_slave_intr(essl_handle_t handle, uint32_t intr_mask, uint32_t wait_ms)
{
    CHECK_EXECUTE_CMD(handle, send_slave_intr, intr_mask, wait_ms);
}
