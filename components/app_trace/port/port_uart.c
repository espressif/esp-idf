/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc.h"
#include "esp_log.h"
#include "esp_cpu.h"
#include "esp_app_trace_port.h"

#include "driver/uart.h"
#include "hal/uart_ll.h"
#include "string.h"
#include "driver/gpio.h"


#define APPTRACE_DEST_UART (CONFIG_APPTRACE_DEST_UART0 | CONFIG_APPTRACE_DEST_UART1 | CONFIG_APPTRACE_DEST_UART2)

#define APP_TRACE_MAX_TX_BUFF_UART          CONFIG_APPTRACE_UART_TX_BUFF_SIZE
#define APP_TRACE_MAX_TX_MSG_UART           CONFIG_APPTRACE_UART_TX_MSG_SIZE

/** UART HW transport data */
typedef struct {
    uint8_t                             inited;
#if CONFIG_APPTRACE_LOCK_ENABLE
    esp_apptrace_lock_t                 lock;   // sync lock
#endif
    uart_port_t port_num;
// TX data ring buffer
    uint8_t *tx_data_buff;
    int32_t tx_data_buff_in;
    int32_t tx_data_buff_out;
// TX message buffer
    uint8_t *tx_msg_buff;
    uint32_t tx_msg_buff_size;

// RX message buffer
    uint8_t *down_buffer;
    uint32_t down_buffer_size;
// Buffer overflow flags
    bool message_buff_overflow;
    bool circular_buff_overflow;
} esp_apptrace_uart_data_t;

#if APPTRACE_DEST_UART
static esp_err_t esp_apptrace_uart_init(esp_apptrace_uart_data_t *hw_data);
static esp_err_t esp_apptrace_uart_flush(esp_apptrace_uart_data_t *hw_data, esp_apptrace_tmo_t *tmo);
static esp_err_t esp_apptrace_uart_flush_nolock(esp_apptrace_uart_data_t *hw_data, uint32_t min_sz, esp_apptrace_tmo_t *tmo);
static uint8_t *esp_apptrace_uart_up_buffer_get(esp_apptrace_uart_data_t *hw_data, uint32_t size, esp_apptrace_tmo_t *tmo);
static esp_err_t esp_apptrace_uart_up_buffer_put(esp_apptrace_uart_data_t *hw_data, uint8_t *ptr, esp_apptrace_tmo_t *tmo);
static void esp_apptrace_uart_down_buffer_config(esp_apptrace_uart_data_t *hw_data, uint8_t *buf, uint32_t size);
static uint8_t *esp_apptrace_uart_down_buffer_get(esp_apptrace_uart_data_t *hw_data, uint32_t *size, esp_apptrace_tmo_t *tmo);
static esp_err_t esp_apptrace_uart_down_buffer_put(esp_apptrace_uart_data_t *hw_data, uint8_t *ptr, esp_apptrace_tmo_t *tmo);
static bool esp_apptrace_uart_host_is_connected(esp_apptrace_uart_data_t *hw_data);

#endif // APPTRACE_DEST_UART
const static char *TAG = "esp_apptrace_uart";

esp_apptrace_hw_t *esp_apptrace_uart_hw_get(int num, void **data)
{
    ESP_LOGD(TAG,"esp_apptrace_uart_hw_get - %i", num);
#if APPTRACE_DEST_UART
    static esp_apptrace_uart_data_t s_uart_hw_data = {
    };
    static esp_apptrace_hw_t s_uart_hw = {
        .init = (esp_err_t (*)(void *))esp_apptrace_uart_init,
        .get_up_buffer = (uint8_t *(*)(void *, uint32_t, esp_apptrace_tmo_t *))esp_apptrace_uart_up_buffer_get,
        .put_up_buffer = (esp_err_t (*)(void *, uint8_t *, esp_apptrace_tmo_t *))esp_apptrace_uart_up_buffer_put,
        .flush_up_buffer_nolock = (esp_err_t (*)(void *, uint32_t, esp_apptrace_tmo_t *))esp_apptrace_uart_flush_nolock,
        .flush_up_buffer = (esp_err_t (*)(void *, esp_apptrace_tmo_t *))esp_apptrace_uart_flush,
        .down_buffer_config = (void (*)(void *, uint8_t *, uint32_t ))esp_apptrace_uart_down_buffer_config,
        .get_down_buffer = (uint8_t *(*)(void *, uint32_t *, esp_apptrace_tmo_t *))esp_apptrace_uart_down_buffer_get,
        .put_down_buffer = (esp_err_t (*)(void *, uint8_t *, esp_apptrace_tmo_t *))esp_apptrace_uart_down_buffer_put,
        .host_is_connected = (bool (*)(void *))esp_apptrace_uart_host_is_connected,
    };
    s_uart_hw_data.port_num = num;
    *data = &s_uart_hw_data;
    return &s_uart_hw;
#else
    return NULL;
#endif
}

#if APPTRACE_DEST_UART

static esp_err_t esp_apptrace_uart_lock(esp_apptrace_uart_data_t *hw_data, esp_apptrace_tmo_t *tmo)
{
#if CONFIG_APPTRACE_LOCK_ENABLE
    esp_err_t ret = esp_apptrace_lock_take(&hw_data->lock, tmo);
    if (ret != ESP_OK) {
        return ESP_FAIL;
    }
#endif
    return ESP_OK;
}

static esp_err_t esp_apptrace_uart_unlock(esp_apptrace_uart_data_t *hw_data)
{
    esp_err_t ret = ESP_OK;
#if CONFIG_APPTRACE_LOCK_ENABLE
    ret = esp_apptrace_lock_give(&hw_data->lock);
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

static esp_err_t esp_apptrace_send_uart_data(esp_apptrace_uart_data_t *hw_data, const char *data, uint32_t size, esp_apptrace_tmo_t *tmo)
{
    esp_err_t res = esp_apptrace_uart_lock(hw_data, tmo);
    if (res != ESP_OK) {
        return res;
    }
    // We store current out position to handle it without lock
    volatile int32_t out_position = hw_data->tx_data_buff_out;

    int len_free = APP_TRACE_MAX_TX_BUFF_UART - (hw_data->tx_data_buff_in - out_position);
    if (out_position > hw_data->tx_data_buff_in) {
        len_free = out_position - hw_data->tx_data_buff_in;
    }
    int check_len = APP_TRACE_MAX_TX_BUFF_UART - hw_data->tx_data_buff_in;
    if (size <= len_free)
    {
        if ( check_len >= size) {
            memcpy(&hw_data->tx_data_buff[hw_data->tx_data_buff_in], data, size);
            hw_data->tx_data_buff_in += size;
        } else {
            memcpy(&hw_data->tx_data_buff[hw_data->tx_data_buff_in], data, APP_TRACE_MAX_TX_BUFF_UART - hw_data->tx_data_buff_in);
            memcpy(&hw_data->tx_data_buff[0], &data[APP_TRACE_MAX_TX_BUFF_UART - hw_data->tx_data_buff_in], size - (APP_TRACE_MAX_TX_BUFF_UART - hw_data->tx_data_buff_in));
            hw_data->tx_data_buff_in = size - (APP_TRACE_MAX_TX_BUFF_UART - hw_data->tx_data_buff_in);
        }
        if (hw_data->tx_data_buff_in >= APP_TRACE_MAX_TX_BUFF_UART) {
            hw_data->tx_data_buff_in = 0;
        }
    } else {
        hw_data->circular_buff_overflow = true;
    }

    if (esp_apptrace_uart_unlock(hw_data) != ESP_OK) {
        assert(false && "Failed to unlock apptrace data!");
    }

    return ESP_OK;
}

static void send_buff_data(esp_apptrace_uart_data_t *hw_data, esp_apptrace_tmo_t *tmo)
{
    if (hw_data->tx_data_buff_in == hw_data->tx_data_buff_out) {
        return;
    }
    // We store current in position to handle it without lock
    volatile int32_t in_position = hw_data->tx_data_buff_in;
    if (in_position > hw_data->tx_data_buff_out) {
        int bytes_sent = uart_write_bytes(hw_data->port_num, &hw_data->tx_data_buff[hw_data->tx_data_buff_out], in_position - hw_data->tx_data_buff_out);
        hw_data->tx_data_buff_out += bytes_sent;
    } else {
        int bytes_sent = uart_write_bytes(hw_data->port_num, &hw_data->tx_data_buff[hw_data->tx_data_buff_out], APP_TRACE_MAX_TX_BUFF_UART - hw_data->tx_data_buff_out);
        hw_data->tx_data_buff_out += bytes_sent;
        if (hw_data->tx_data_buff_out >= APP_TRACE_MAX_TX_BUFF_UART) {
            hw_data->tx_data_buff_out = 0;
        }
    }
}

#define APP_TRACE_UART_STOP_WAIT_TMO    1000000 //us

static void esp_apptrace_send_uart_tx_task(void *arg)
{
    esp_apptrace_uart_data_t *hw_data = (esp_apptrace_uart_data_t *)arg;
    esp_apptrace_tmo_t tmo;
    esp_apptrace_tmo_init(&tmo, APP_TRACE_UART_STOP_WAIT_TMO);

    vTaskDelay(10);
    while (1) {
        send_buff_data(hw_data, &tmo);
        vTaskDelay(10);
        if (hw_data->circular_buff_overflow == true)
        {
            hw_data->circular_buff_overflow = false;
            ESP_LOGE(TAG, "Buffer overflow. Please increase UART baudrate, or increase UART TX ring buffer size in menuconfig.");
        }
        if (hw_data->message_buff_overflow == true)
        {
            hw_data->message_buff_overflow = false;
            ESP_LOGE(TAG, "Message size more then message buffer!");
        }
    }
}

static const int APP_TRACE_UART_RX_BUF_SIZE = 4024;

static esp_err_t esp_apptrace_uart_init(esp_apptrace_uart_data_t *hw_data)
{
    int core_id = esp_cpu_get_core_id();
    if (core_id == 0) {
        hw_data->tx_data_buff = (uint8_t *)heap_caps_malloc(APP_TRACE_MAX_TX_BUFF_UART, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
        if (hw_data->tx_data_buff == NULL){
            return ESP_ERR_NO_MEM;
        }
        hw_data->tx_data_buff_in = 0;
        hw_data->tx_data_buff_out = 0;
        hw_data->tx_msg_buff =  (uint8_t *)heap_caps_malloc(APP_TRACE_MAX_TX_MSG_UART, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
        if (hw_data->tx_msg_buff == NULL)
        {
            return ESP_ERR_NO_MEM;
        }
        hw_data->tx_msg_buff_size = 0;
        hw_data->down_buffer_size = 0;
        hw_data->message_buff_overflow = false;
        hw_data->circular_buff_overflow = false;


        const uart_config_t uart_config = {
            .baud_rate = CONFIG_APPTRACE_UART_BAUDRATE,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
            .source_clk = UART_SCLK_DEFAULT,
        };
        ESP_LOGI(TAG, "UART baud rate: %i", CONFIG_APPTRACE_UART_BAUDRATE);
        // We won't use a buffer for sending data.
        esp_err_t err = uart_driver_install(hw_data->port_num, APP_TRACE_UART_RX_BUF_SIZE, APP_TRACE_UART_RX_BUF_SIZE, 0, NULL, 0);
        assert((err == ESP_OK) && "Not possible to install UART. Please check and change menuconfig parameters!");
        err = uart_param_config(hw_data->port_num, &uart_config);
        assert((err == ESP_OK) && "Not possible to configure UART. Please check and change menuconfig parameters!");
        err = uart_set_pin(hw_data->port_num, CONFIG_APPTRACE_UART_TX_GPIO, CONFIG_APPTRACE_UART_RX_GPIO, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
        assert((err == ESP_OK) && "Not possible to configure UART RX/TX pins. Please check and change menuconfig parameters!");

        int uart_prio = CONFIG_APPTRACE_UART_TASK_PRIO;
        if (uart_prio >= (configMAX_PRIORITIES-1)) uart_prio = configMAX_PRIORITIES - 1;
        err = xTaskCreate(esp_apptrace_send_uart_tx_task, "app_trace_uart_tx_task", 2500, hw_data, uart_prio, NULL);
        assert((err == pdPASS) && "Not possible to configure UART. Not possible to create task!");

#if CONFIG_APPTRACE_LOCK_ENABLE
        esp_apptrace_lock_init(&hw_data->lock);
#endif
    }
    // init UART on this CPU
    esp_apptrace_uart_hw_init();
    hw_data->inited |= 1 << core_id;

    return ESP_OK;
}

static uint8_t *esp_apptrace_uart_up_buffer_get(esp_apptrace_uart_data_t *hw_data, uint32_t size, esp_apptrace_tmo_t *tmo)
{
    uint8_t *ptr;
    if (size > APP_TRACE_MAX_TX_MSG_UART) {
        hw_data->message_buff_overflow = true;
        return NULL;
    }
    if (hw_data->tx_msg_buff_size != 0)
    {
        // A previous message was not sent.
        return NULL;
    }
    esp_err_t res = esp_apptrace_uart_lock(hw_data, tmo);
    if (res != ESP_OK) {
        return NULL;
    }
    ptr = hw_data->tx_msg_buff;
    hw_data->tx_msg_buff_size = size;

    // now we can safely unlock apptrace to allow other tasks/ISRs to get other buffers and write their data
    if (esp_apptrace_uart_unlock(hw_data) != ESP_OK) {
        assert(false && "Failed to unlock apptrace data!");
    }
    return ptr;
}

static esp_err_t esp_apptrace_uart_up_buffer_put(esp_apptrace_uart_data_t *hw_data, uint8_t *ptr, esp_apptrace_tmo_t *tmo)
{
    esp_err_t res = esp_apptrace_send_uart_data(hw_data, (const char *)ptr, hw_data->tx_msg_buff_size, tmo);
    // Clear size to indicate that we've sent data
    hw_data->tx_msg_buff_size = 0;
    return res;
}

static void esp_apptrace_uart_down_buffer_config(esp_apptrace_uart_data_t *hw_data, uint8_t *buf, uint32_t size)
{
    hw_data->down_buffer = (uint8_t *)malloc(size);
    if (hw_data->down_buffer == NULL){
        assert(false && "Failed to allocate apptrace uart down buffer!");
    }
    hw_data->down_buffer_size = size;
}

static uint8_t *esp_apptrace_uart_down_buffer_get(esp_apptrace_uart_data_t *hw_data, uint32_t *size, esp_apptrace_tmo_t *tmo)
{
    uint8_t *ptr = NULL;

    if (*size > hw_data->down_buffer_size) {
        return NULL;
    }

    esp_err_t res = esp_apptrace_uart_lock(hw_data, tmo);
    if (res != ESP_OK) {
        return NULL;
    }

    size_t uart_fifolen = 0;
    uart_get_buffered_data_len(hw_data->port_num, &uart_fifolen);
    if (uart_fifolen > 0) {
        if (*size < uart_fifolen) {
            uart_fifolen = *size;
        }
        *size = uart_fifolen;
        ptr = hw_data->down_buffer;
        *size =uart_read_bytes(hw_data->port_num, ptr, uart_fifolen, 0);
    }

    if (esp_apptrace_uart_unlock(hw_data) != ESP_OK) {
        assert(false && "Failed to unlock apptrace data!");
    }
    return ptr;
}

static esp_err_t esp_apptrace_uart_down_buffer_put(esp_apptrace_uart_data_t *hw_data, uint8_t *ptr, esp_apptrace_tmo_t *tmo)
{
    return ESP_OK;
}

static bool esp_apptrace_uart_host_is_connected(esp_apptrace_uart_data_t *hw_data)
{
    return hw_data->inited & 1;
}

static esp_err_t esp_apptrace_uart_flush_nolock(esp_apptrace_uart_data_t *hw_data, uint32_t min_sz, esp_apptrace_tmo_t *tmo)
{
    return ESP_OK;
}

static esp_err_t esp_apptrace_uart_flush(esp_apptrace_uart_data_t *hw_data, esp_apptrace_tmo_t *tmo)
{
    return ESP_OK;
}

#endif // APPTRACE_DEST_UART
