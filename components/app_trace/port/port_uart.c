/*
 * SPDX-FileCopyrightText: 2017-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include "soc/soc.h"
#include "esp_log.h"
#include "esp_cpu.h"
#include "esp_app_trace_config.h"
#include "esp_app_trace_port.h"

#include "driver/uart.h"
#include "hal/uart_ll.h"

/** UART HW transport data */
typedef struct {
    uint8_t                             inited;
#if CONFIG_APPTRACE_LOCK_ENABLE
    esp_apptrace_lock_t                 lock;   // sync lock
#endif
    uart_port_t port_num;

    /* TX data ring buffer */
    uint8_t *tx_data_buff;
    uint32_t tx_data_buff_size;
    int32_t tx_data_buff_in;
    int32_t tx_data_buff_out;

    /* TX message buffer */
    uint8_t *tx_msg_buff;
    uint32_t tx_msg_buff_size;
    uint32_t tx_pending_msg_size;

    /* RX message buffer */
    uint8_t *down_buffer;
    uint32_t down_buffer_size;

    /* Buffer overflow flags */
    bool message_buff_overflow;
    bool circular_buff_overflow;
} esp_apptrace_uart_data_t;

const static char *TAG = "esp_apptrace_uart";

static esp_err_t esp_apptrace_uart_lock(void *hw_data, esp_apptrace_tmo_t *tmo)
{
#if CONFIG_APPTRACE_LOCK_ENABLE
    esp_apptrace_uart_data_t *uart_data = hw_data;
    esp_err_t ret = esp_apptrace_lock_take(&uart_data->lock, tmo);
    if (ret != ESP_OK) {
        return ESP_FAIL;
    }
#endif
    return ESP_OK;
}

static esp_err_t esp_apptrace_uart_unlock(void *hw_data)
{
    esp_err_t ret = ESP_OK;
#if CONFIG_APPTRACE_LOCK_ENABLE
    esp_apptrace_uart_data_t *uart_data = hw_data;
    ret = esp_apptrace_lock_give(&uart_data->lock);
#endif
    return ret;
}

static inline void esp_apptrace_uart_hw_init(void)
{
    ESP_APPTRACE_LOGI("Initialized UART on CPU%d", esp_cpu_get_core_id());
}

/*****************************************************************************************/
/***************************** Apptrace HW iface *****************************************/
/*****************************************************************************************/

static esp_err_t esp_apptrace_send_uart_data(void *hw_data, const char *data, uint32_t size, esp_apptrace_tmo_t *tmo)
{
    esp_apptrace_uart_data_t *uart_data = hw_data;
    esp_err_t res = esp_apptrace_uart_lock(uart_data, tmo);
    if (res != ESP_OK) {
        return res;
    }
    // We store current out position to handle it without lock
    volatile int32_t out_position = uart_data->tx_data_buff_out;

    int len_free = uart_data->tx_data_buff_size - (uart_data->tx_data_buff_in - out_position);
    if (out_position > uart_data->tx_data_buff_in) {
        len_free = out_position - uart_data->tx_data_buff_in;
    }
    int check_len = uart_data->tx_data_buff_size - uart_data->tx_data_buff_in;
    if (size <= len_free) {
        if (check_len >= size) {
            memcpy(&uart_data->tx_data_buff[uart_data->tx_data_buff_in], data, size);
            uart_data->tx_data_buff_in += size;
        } else {
            memcpy(&uart_data->tx_data_buff[uart_data->tx_data_buff_in],
                   data,
                   uart_data->tx_data_buff_size - uart_data->tx_data_buff_in);
            memcpy(&uart_data->tx_data_buff[0],
                   &data[uart_data->tx_data_buff_size - uart_data->tx_data_buff_in],
                   size - (uart_data->tx_data_buff_size - uart_data->tx_data_buff_in));
            uart_data->tx_data_buff_in = size - (uart_data->tx_data_buff_size - uart_data->tx_data_buff_in);
        }
        if (uart_data->tx_data_buff_in >= uart_data->tx_data_buff_size) {
            uart_data->tx_data_buff_in = 0;
        }
    } else {
        uart_data->circular_buff_overflow = true;
    }

    if (esp_apptrace_uart_unlock(uart_data) != ESP_OK) {
        assert(false && "Failed to unlock apptrace data!");
    }

    return ESP_OK;
}

static void send_buff_data(void *hw_data, esp_apptrace_tmo_t *tmo)
{
    esp_apptrace_uart_data_t *uart_data = hw_data;

    if (uart_data->tx_data_buff_in == uart_data->tx_data_buff_out) {
        return;
    }
    // We store current in position to handle it without lock
    volatile int32_t in_position = uart_data->tx_data_buff_in;
    if (in_position > uart_data->tx_data_buff_out) {
        int bytes_sent = uart_write_bytes(uart_data->port_num,
                                          &uart_data->tx_data_buff[uart_data->tx_data_buff_out],
                                          in_position - uart_data->tx_data_buff_out);
        uart_data->tx_data_buff_out += bytes_sent;
    } else {
        int bytes_sent = uart_write_bytes(uart_data->port_num,
                                          &uart_data->tx_data_buff[uart_data->tx_data_buff_out],
                                          uart_data->tx_data_buff_size - uart_data->tx_data_buff_out);
        uart_data->tx_data_buff_out += bytes_sent;
        if (uart_data->tx_data_buff_out >= uart_data->tx_data_buff_size) {
            uart_data->tx_data_buff_out = 0;
        }
    }
}

#define APP_TRACE_UART_STOP_WAIT_TMO    1000000 //us

static void esp_apptrace_send_uart_tx_task(void *arg)
{
    esp_apptrace_uart_data_t *uart_data = arg;
    esp_apptrace_tmo_t tmo;

    esp_apptrace_tmo_init(&tmo, APP_TRACE_UART_STOP_WAIT_TMO);

    vTaskDelay(10);
    while (1) {
        send_buff_data(uart_data, &tmo);
        vTaskDelay(10);
        if (uart_data->circular_buff_overflow == true) {
            uart_data->circular_buff_overflow = false;
            ESP_LOGE(TAG, "Buffer overflow. Please increase UART baudrate, or increase UART TX ring buffer size in menuconfig.");
        }
        if (uart_data->message_buff_overflow == true) {
            uart_data->message_buff_overflow = false;
            ESP_LOGE(TAG, "Message size more then message buffer!");
        }
    }
}

static const int APP_TRACE_UART_RX_BUF_SIZE = 4024;

static esp_err_t esp_apptrace_uart_init(void *hw_data, const esp_apptrace_config_t *config)
{
    esp_apptrace_uart_data_t *uart_data = hw_data;
    const esp_apptrace_uart_config_t *apptrace_uart_config = &config->dest_cfg.uart;

    /* esp_apptrace_uart_init() is called on every core, so ensure to do main initialization only once */
    int core_id = esp_cpu_get_core_id();
    if (core_id == 0) {
        uart_data->tx_data_buff_size = apptrace_uart_config->tx_buff_size;
        uart_data->tx_data_buff = heap_caps_malloc(uart_data->tx_data_buff_size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        if (uart_data->tx_data_buff == NULL) {
            return ESP_ERR_NO_MEM;
        }
        uart_data->tx_data_buff_in = 0;
        uart_data->tx_data_buff_out = 0;
        uart_data->tx_msg_buff_size = apptrace_uart_config->tx_msg_size;
        uart_data->tx_msg_buff = heap_caps_malloc(uart_data->tx_msg_buff_size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        if (uart_data->tx_msg_buff == NULL) {
            return ESP_ERR_NO_MEM;
        }
        uart_data->tx_pending_msg_size = 0;
        uart_data->down_buffer_size = 0;
        uart_data->message_buff_overflow = false;
        uart_data->circular_buff_overflow = false;

        assert((uart_data->port_num <= SOC_UART_NUM) && "Not possible to configure UART. Please check selected UART port");

        int source_clk = UART_SCLK_DEFAULT;
#if SOC_UART_LP_NUM > 0
        if (uart_data->port_num >= SOC_UART_HP_NUM) {
            source_clk = LP_UART_SCLK_DEFAULT;
        }
#endif

        const uart_config_t uart_config = {
            .baud_rate = apptrace_uart_config->baud_rate,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
            .source_clk = source_clk,
        };
        ESP_LOGI(TAG, "UART baud rate: %i", apptrace_uart_config->baud_rate);
        // We won't use a buffer for sending data.
        esp_err_t __attribute__((unused)) err = uart_driver_install(uart_data->port_num,
                                                                    APP_TRACE_UART_RX_BUF_SIZE,
                                                                    APP_TRACE_UART_RX_BUF_SIZE,
                                                                    0,
                                                                    NULL,
                                                                    0);
        assert((err == ESP_OK) && "Not possible to install UART. Please check and change uart parameters!");
        err = uart_param_config(uart_data->port_num, &uart_config);
        assert((err == ESP_OK) && "Not possible to configure UART. Please check and change uart parameters!");
        err = uart_set_pin(uart_data->port_num,
                           apptrace_uart_config->tx_pin_num,
                           apptrace_uart_config->rx_pin_num,
                           UART_PIN_NO_CHANGE,
                           UART_PIN_NO_CHANGE);
        assert((err == ESP_OK) && "Not possible to configure UART RX/TX pins. Please check and change the uart pins!");

        int uart_prio = apptrace_uart_config->task_prio;
        if (uart_prio >= (configMAX_PRIORITIES - 1)) {
            uart_prio = configMAX_PRIORITIES - 1;
        }
        err = xTaskCreate(esp_apptrace_send_uart_tx_task, "app_trace_uart_tx_task", 2500, uart_data, uart_prio, NULL);
        assert((err == pdPASS) && "Not possible to configure UART. Not possible to create task!");

#if CONFIG_APPTRACE_LOCK_ENABLE
        esp_apptrace_lock_init(&uart_data->lock);
#endif
    }
    // init UART on this CPU
    esp_apptrace_uart_hw_init();
    uart_data->inited |= 1 << core_id;

    return ESP_OK;
}

static uint8_t *esp_apptrace_uart_up_buffer_get(void *hw_data, uint32_t size, esp_apptrace_tmo_t *tmo)
{
    esp_apptrace_uart_data_t *uart_data = hw_data;

    if (size > uart_data->tx_msg_buff_size) {
        uart_data->message_buff_overflow = true;
        return NULL;
    }
    if (uart_data->tx_pending_msg_size != 0) {
        // A previous message was not sent.
        return NULL;
    }
    esp_err_t res = esp_apptrace_uart_lock(uart_data, tmo);
    if (res != ESP_OK) {
        return NULL;
    }
    uint8_t *ptr = uart_data->tx_msg_buff;

    // Set the amount of data to send
    uart_data->tx_pending_msg_size = size;

    // now we can safely unlock apptrace to allow other tasks/ISRs to get other buffers and write their data
    if (esp_apptrace_uart_unlock(uart_data) != ESP_OK) {
        assert(false && "Failed to unlock apptrace data!");
    }
    return ptr;
}

static esp_err_t esp_apptrace_uart_up_buffer_put(void *hw_data, uint8_t *ptr, esp_apptrace_tmo_t *tmo)
{
    esp_apptrace_uart_data_t *uart_data = hw_data;
    esp_err_t res = esp_apptrace_send_uart_data(uart_data, (const char *)ptr, uart_data->tx_pending_msg_size, tmo);
    // Clear size to indicate that we've sent data
    uart_data->tx_pending_msg_size = 0;
    return res;
}

static void esp_apptrace_uart_down_buffer_config(void *hw_data, uint8_t *buf, uint32_t size)
{
    esp_apptrace_uart_data_t *uart_data = hw_data;

    assert(buf != NULL && "Down buffer cannot be NULL");

    uart_data->down_buffer = buf;
    uart_data->down_buffer_size = size;
}

static uint8_t *esp_apptrace_uart_down_buffer_get(void *hw_data, uint32_t *size, esp_apptrace_tmo_t *tmo)
{
    esp_apptrace_uart_data_t *uart_data = hw_data;
    uint8_t *ptr = NULL;

    if (*size > uart_data->down_buffer_size) {
        return NULL;
    }

    esp_err_t res = esp_apptrace_uart_lock(uart_data, tmo);
    if (res != ESP_OK) {
        return NULL;
    }

    size_t uart_fifolen = 0;
    uart_get_buffered_data_len(uart_data->port_num, &uart_fifolen);
    if (uart_fifolen > 0) {
        if (*size < uart_fifolen) {
            uart_fifolen = *size;
        }
        *size = uart_fifolen;
        ptr = uart_data->down_buffer;
        *size = uart_read_bytes(uart_data->port_num, ptr, uart_fifolen, 0);
    }

    if (esp_apptrace_uart_unlock(uart_data) != ESP_OK) {
        assert(false && "Failed to unlock apptrace data!");
    }
    return ptr;
}

static esp_err_t esp_apptrace_uart_down_buffer_put(void *hw_data, uint8_t *ptr, esp_apptrace_tmo_t *tmo)
{
    return ESP_OK;
}

static bool esp_apptrace_uart_host_is_connected(void *hw_data)
{
    esp_apptrace_uart_data_t *uart_data = hw_data;

    return uart_data->inited & 1;
}

static esp_err_t esp_apptrace_uart_flush_nolock(void *hw_data, uint32_t min_sz, esp_apptrace_tmo_t *tmo)
{
    return ESP_OK;
}

static esp_err_t esp_apptrace_uart_flush(void *hw_data, esp_apptrace_tmo_t *tmo)
{
    return ESP_OK;
}

esp_apptrace_hw_t *esp_apptrace_uart_hw_get(int num, void **data)
{
    ESP_LOGD(TAG, "esp_apptrace_uart_hw_get - %i", num);

    static esp_apptrace_uart_data_t s_uart_hw_data;
    static esp_apptrace_hw_t s_uart_hw = {
        .init = esp_apptrace_uart_init,
        .get_up_buffer = esp_apptrace_uart_up_buffer_get,
        .put_up_buffer = esp_apptrace_uart_up_buffer_put,
        .flush_up_buffer_nolock = esp_apptrace_uart_flush_nolock,
        .flush_up_buffer = esp_apptrace_uart_flush,
        .down_buffer_config = esp_apptrace_uart_down_buffer_config,
        .get_down_buffer = esp_apptrace_uart_down_buffer_get,
        .put_down_buffer = esp_apptrace_uart_down_buffer_put,
        .host_is_connected = esp_apptrace_uart_host_is_connected,
    };
    s_uart_hw_data.port_num = num;
    *data = &s_uart_hw_data;
    return &s_uart_hw;
}
