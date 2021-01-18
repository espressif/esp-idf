// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_modem.h"
#include "esp_log.h"
#include "sdkconfig.h"

#define ESP_MODEM_EVENT_QUEUE_SIZE (16)

#define MIN_PATTERN_INTERVAL (9)
#define MIN_POST_IDLE (0)
#define MIN_PRE_IDLE (0)

/**
 * @brief Macro defined for error checking
 *
 */
static const char *MODEM_TAG = "esp-modem";
#define MODEM_CHECK(a, str, goto_tag, ...)                                              \
    do                                                                                  \
    {                                                                                   \
        if (!(a))                                                                       \
        {                                                                               \
            ESP_LOGE(MODEM_TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            goto goto_tag;                                                              \
        }                                                                               \
    } while (0)

ESP_EVENT_DEFINE_BASE(ESP_MODEM_EVENT);

/**
 * @brief ESP32 Modem DTE
 *
 */
typedef struct {
    uart_port_t uart_port;                  /*!< UART port */
    uint8_t *buffer;                        /*!< Internal buffer to store response lines/data from DCE */
    QueueHandle_t event_queue;              /*!< UART event queue handle */
    esp_event_loop_handle_t event_loop_hdl; /*!< Event loop handle */
    TaskHandle_t uart_event_task_hdl;       /*!< UART event task handle */
    SemaphoreHandle_t process_sem;          /*!< Semaphore used for indicating processing status */
    SemaphoreHandle_t   exit_sem;           /*!< Semaphore used for indicating PPP mode has stopped */
    modem_dte_t parent;                     /*!< DTE interface that should extend */
    esp_modem_on_receive receive_cb;        /*!< ptr to data reception */
    void *receive_cb_ctx;                   /*!< ptr to rx fn context data */
    int line_buffer_size;                   /*!< line buffer size in commnad mode */
    int pattern_queue_size;                 /*!< UART pattern queue size */
} esp_modem_dte_t;

/**
 * @brief Returns true if the supplied string contains only CR or LF
 *
 * @param str string to check
 * @param len length of string
 */
static inline bool is_only_cr_lf(const char *str, uint32_t len)
{
    for (int i=0; i<len; ++i) {
        if (str[i] != '\r' && str[i] != '\n') {
            return false;
        }
    }
    return true;
}

esp_err_t esp_modem_set_rx_cb(modem_dte_t *dte, esp_modem_on_receive receive_cb, void *receive_cb_ctx)
{
    esp_modem_dte_t *esp_dte = __containerof(dte, esp_modem_dte_t, parent);
    esp_dte->receive_cb_ctx = receive_cb_ctx;
    esp_dte->receive_cb = receive_cb;
    return ESP_OK;
}


/**
 * @brief Handle one line in DTE
 *
 * @param esp_dte ESP modem DTE object
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
static esp_err_t esp_dte_handle_line(esp_modem_dte_t *esp_dte)
{
    esp_err_t err = ESP_FAIL;
    modem_dce_t *dce = esp_dte->parent.dce;
    MODEM_CHECK(dce, "DTE has not yet bind with DCE", err);
    const char *line = (const char *)(esp_dte->buffer);
    size_t len = strlen(line);
    /* Skip pure "\r\n" lines */
    if (len > 2 && !is_only_cr_lf(line, len)) {
        if (dce->handle_line == NULL) {
            /* Received an asynchronous line, but no handler waiting this this */
            ESP_LOGD(MODEM_TAG, "No handler for line: %s", line);
            err = ESP_OK; /* Not an error, just propagate the line to user handler */
            goto post_event_unknown;
        }
        MODEM_CHECK(dce->handle_line(dce, line) == ESP_OK, "handle line failed", post_event_unknown);
    }
    return ESP_OK;
post_event_unknown:
    /* Send ESP_MODEM_EVENT_UNKNOWN signal to event loop */
    esp_event_post_to(esp_dte->event_loop_hdl, ESP_MODEM_EVENT, ESP_MODEM_EVENT_UNKNOWN,
                      (void *)line, strlen(line) + 1, pdMS_TO_TICKS(100));
err:
    return err;
}

/**
 * @brief Handle when a pattern has been detected by UART
 *
 * @param esp_dte ESP32 Modem DTE object
 */
static void esp_handle_uart_pattern(esp_modem_dte_t *esp_dte)
{
    int pos = uart_pattern_pop_pos(esp_dte->uart_port);
    int read_len = 0;

    if (esp_dte->parent.dce->mode == MODEM_PPP_MODE) {
        ESP_LOGD(MODEM_TAG, "Pattern event in PPP mode ignored");
        // Ignore potential pattern detection events in PPP mode
        // Note 1: the interrupt is disabled, but some events might still be pending
        // Note 2: checking the mode *after* uart_pattern_pop_pos() to consume the event
        return;
    }

    if (pos != -1) {
        if (pos < esp_dte->line_buffer_size - 1) {
            /* read one line(include '\n') */
            read_len = pos + 1;
        } else {
            ESP_LOGW(MODEM_TAG, "ESP Modem Line buffer too small");
            read_len = esp_dte->line_buffer_size - 1;
        }
        read_len = uart_read_bytes(esp_dte->uart_port, esp_dte->buffer, read_len, pdMS_TO_TICKS(100));
        if (read_len) {
            /* make sure the line is a standard string */
            esp_dte->buffer[read_len] = '\0';
            /* Send new line to handle */
            esp_dte_handle_line(esp_dte);
        } else {
            ESP_LOGE(MODEM_TAG, "uart read bytes failed");
        }
    } else {
        size_t length = 0;
        uart_get_buffered_data_len(esp_dte->uart_port, &length);
        if (length) {
            ESP_LOGD(MODEM_TAG, "Pattern not found in the pattern queue, uart data length = %d", length);
            length = MIN(esp_dte->line_buffer_size-1, length);
            length = uart_read_bytes(esp_dte->uart_port, esp_dte->buffer, length, portMAX_DELAY);
            ESP_LOG_BUFFER_HEXDUMP("esp-modem-pattern: debug_data", esp_dte->buffer, length, ESP_LOG_DEBUG);
        }
        uart_flush(esp_dte->uart_port);
    }
}

/**
 * @brief Handle when new data received by UART
 *
 * @param esp_dte ESP32 Modem DTE object
 */
static void esp_handle_uart_data(esp_modem_dte_t *esp_dte)
{
    size_t length = 0;
    uart_get_buffered_data_len(esp_dte->uart_port, &length);
    if (esp_dte->parent.dce->mode != MODEM_PPP_MODE && length) {
        // Check if matches the pattern to process the data as pattern
        int pos = uart_pattern_get_pos(esp_dte->uart_port);
        if (pos > -1) {
            esp_handle_uart_pattern(esp_dte);
            return;
        }
        // Read the data and process it using `handle_line` logic
        length = MIN(esp_dte->line_buffer_size-1, length);
        length = uart_read_bytes(esp_dte->uart_port, esp_dte->buffer, length, portMAX_DELAY);
        esp_dte->buffer[length] = '\0';
        if (strchr((char*)esp_dte->buffer, '\n') == NULL) {
            size_t max = esp_dte->line_buffer_size-1;
            size_t bytes;
            // if pattern not found in the data,
            // continue reading as long as the modem is in MODEM_STATE_PROCESSING, checking for the pattern
            while (length < max && esp_dte->buffer[length-1] != '\n' &&
                   esp_dte->parent.dce->state == MODEM_STATE_PROCESSING) {
                bytes = uart_read_bytes(esp_dte->uart_port,
                                        esp_dte->buffer + length, 1, pdMS_TO_TICKS(100));
                length += bytes;
                ESP_LOGV("esp-modem: debug_data", "Continuous read in non-data mode: length: %d char: %x", length, esp_dte->buffer[length-1]);
            }
            esp_dte->buffer[length] = '\0';
        }
        ESP_LOG_BUFFER_HEXDUMP("esp-modem: debug_data", esp_dte->buffer, length, ESP_LOG_DEBUG);
        if (esp_dte->parent.dce->handle_line) {
            /* Send new line to handle if handler registered */
            esp_dte_handle_line(esp_dte);
        }
        return;
    }

    length = MIN(esp_dte->line_buffer_size, length);
    length = uart_read_bytes(esp_dte->uart_port, esp_dte->buffer, length, portMAX_DELAY);
    /* pass the input data to configured callback */
    if (length) {
        esp_dte->receive_cb(esp_dte->buffer, length, esp_dte->receive_cb_ctx);
    }
}

/**
 * @brief UART Event Task Entry
 *
 * @param param task parameter
 */
static void uart_event_task_entry(void *param)
{
    esp_modem_dte_t *esp_dte = (esp_modem_dte_t *)param;
    uart_event_t event;
    while (1) {
        /* Drive the event loop */
        esp_event_loop_run(esp_dte->event_loop_hdl, pdMS_TO_TICKS(0));

        /* Process UART events */
        if (xQueueReceive(esp_dte->event_queue, &event, pdMS_TO_TICKS(100))) {
            if (esp_dte->parent.dce == NULL) {
                ESP_LOGD(MODEM_TAG, "Ignore UART event for DTE with no DCE attached");
                // No action on any uart event with null DCE.
                // This might happen before DCE gets initialized and attached to running DTE,
                // or after destroying the DCE when DTE is up and gets a data event.
                uart_flush(esp_dte->uart_port);
                continue;
            }

            switch (event.type) {
            case UART_DATA:
                esp_handle_uart_data(esp_dte);
                break;
            case UART_FIFO_OVF:
                ESP_LOGW(MODEM_TAG, "HW FIFO Overflow");
                uart_flush_input(esp_dte->uart_port);
                xQueueReset(esp_dte->event_queue);
                break;
            case UART_BUFFER_FULL:
                ESP_LOGW(MODEM_TAG, "Ring Buffer Full");
                uart_flush_input(esp_dte->uart_port);
                xQueueReset(esp_dte->event_queue);
                break;
            case UART_BREAK:
                ESP_LOGW(MODEM_TAG, "Rx Break");
                break;
            case UART_PARITY_ERR:
                ESP_LOGE(MODEM_TAG, "Parity Error");
                break;
            case UART_FRAME_ERR:
                ESP_LOGE(MODEM_TAG, "Frame Error");
                break;
            case UART_PATTERN_DET:
                esp_handle_uart_pattern(esp_dte);
                break;
            default:
                ESP_LOGW(MODEM_TAG, "unknown uart event type: %d", event.type);
                break;
            }
        }
    }
    vTaskDelete(NULL);
}

/**
 * @brief Send command to DCE
 *
 * @param dte Modem DTE object
 * @param command command string
 * @param timeout timeout value, unit: ms
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
static esp_err_t esp_modem_dte_send_cmd(modem_dte_t *dte, const char *command, uint32_t timeout)
{
    esp_err_t ret = ESP_FAIL;
    modem_dce_t *dce = dte->dce;
    MODEM_CHECK(dce, "DTE has not yet bind with DCE", err);
    MODEM_CHECK(command, "command is NULL", err);
    esp_modem_dte_t *esp_dte = __containerof(dte, esp_modem_dte_t, parent);
    /* Calculate timeout clock tick */
    /* Reset runtime information */
    dce->state = MODEM_STATE_PROCESSING;
    /* Send command via UART */
    uart_write_bytes(esp_dte->uart_port, command, strlen(command));
    /* Check timeout */
    MODEM_CHECK(xSemaphoreTake(esp_dte->process_sem, pdMS_TO_TICKS(timeout)) == pdTRUE, "process command timeout", err);
    ret = ESP_OK;
err:
    dce->handle_line = NULL;
    return ret;
}

/**
 * @brief Send data to DCE
 *
 * @param dte Modem DTE object
 * @param data data buffer
 * @param length length of data to send
 * @return int actual length of data that has been send out
 */
static int esp_modem_dte_send_data(modem_dte_t *dte, const char *data, uint32_t length)
{
    MODEM_CHECK(data, "data is NULL", err);
    esp_modem_dte_t *esp_dte = __containerof(dte, esp_modem_dte_t, parent);
    if (esp_dte->parent.dce->mode == MODEM_TRANSITION_MODE) {
        ESP_LOGD(MODEM_TAG, "Not sending data in transition mode");
        return -1;
    }
    return uart_write_bytes(esp_dte->uart_port, data, length);
err:
    return -1;
}



/**
 * @brief Send data and wait for prompt from DCE
 *
 * @param dte Modem DTE object
 * @param data data buffer
 * @param length length of data to send
 * @param prompt pointer of specific prompt
 * @param timeout timeout value (unit: ms)
 * @return esp_err_t
 *      ESP_OK on success
 *      ESP_FAIL on error
 */
static esp_err_t esp_modem_dte_send_wait(modem_dte_t *dte, const char *data, uint32_t length,
        const char *prompt, uint32_t timeout)
{
    MODEM_CHECK(data, "data is NULL", err_param);
    MODEM_CHECK(prompt, "prompt is NULL", err_param);
    esp_modem_dte_t *esp_dte = __containerof(dte, esp_modem_dte_t, parent);
    // We'd better disable pattern detection here for a moment in case prompt string contains the pattern character
    uart_disable_pattern_det_intr(esp_dte->uart_port);
    // uart_disable_rx_intr(esp_dte->uart_port);
    MODEM_CHECK(uart_write_bytes(esp_dte->uart_port, data, length) >= 0, "uart write bytes failed", err_write);
    uint32_t len = strlen(prompt);
    uint8_t *buffer = calloc(len + 1, sizeof(uint8_t));
    int res = uart_read_bytes(esp_dte->uart_port, buffer, len, pdMS_TO_TICKS(timeout));
    MODEM_CHECK(res >= len, "wait prompt [%s] timeout", err, prompt);
    MODEM_CHECK(!strncmp(prompt, (const char *)buffer, len), "get wrong prompt: %s", err, buffer);
    free(buffer);
    uart_enable_pattern_det_baud_intr(esp_dte->uart_port, '\n', 1, MIN_PATTERN_INTERVAL, MIN_POST_IDLE, MIN_PRE_IDLE);
    return ESP_OK;
err:
    free(buffer);
err_write:
    uart_enable_pattern_det_baud_intr(esp_dte->uart_port, '\n', 1, MIN_PATTERN_INTERVAL, MIN_POST_IDLE, MIN_PRE_IDLE);
err_param:
    return ESP_FAIL;
}

/**
 * @brief Change Modem's working mode
 *
 * @param dte Modem DTE object
 * @param new_mode new working mode
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
static esp_err_t esp_modem_dte_change_mode(modem_dte_t *dte, modem_mode_t new_mode)
{
    modem_dce_t *dce = dte->dce;
    MODEM_CHECK(dce, "DTE has not yet bind with DCE", err);
    esp_modem_dte_t *esp_dte = __containerof(dte, esp_modem_dte_t, parent);
    modem_mode_t current_mode = dce->mode;
    MODEM_CHECK(current_mode != new_mode, "already in mode: %d", err, new_mode);
    dce->mode = MODEM_TRANSITION_MODE;  // mode switching will be finished in set_working_mode() on success
                                        // (or restored on failure)
    switch (new_mode) {
    case MODEM_PPP_MODE:
        MODEM_CHECK(dce->set_working_mode(dce, new_mode) == ESP_OK, "set new working mode:%d failed", err_restore_mode, new_mode);
        uart_disable_pattern_det_intr(esp_dte->uart_port);
        uart_enable_rx_intr(esp_dte->uart_port);
        break;
    case MODEM_COMMAND_MODE:
        MODEM_CHECK(dce->set_working_mode(dce, new_mode) == ESP_OK, "set new working mode:%d failed", err_restore_mode, new_mode);
        uart_disable_rx_intr(esp_dte->uart_port);
        uart_flush(esp_dte->uart_port);
        uart_enable_pattern_det_baud_intr(esp_dte->uart_port, '\n', 1, MIN_PATTERN_INTERVAL, MIN_POST_IDLE, MIN_PRE_IDLE);
        uart_pattern_queue_reset(esp_dte->uart_port, esp_dte->pattern_queue_size);
        break;
    default:
        break;
    }
    return ESP_OK;
err_restore_mode:
    dce->mode = current_mode;
err:
    return ESP_FAIL;
}

static esp_err_t esp_modem_dte_process_cmd_done(modem_dte_t *dte)
{
    esp_modem_dte_t *esp_dte = __containerof(dte, esp_modem_dte_t, parent);
    return xSemaphoreGive(esp_dte->process_sem) == pdTRUE ? ESP_OK : ESP_FAIL;
}

/**
 * @brief Deinitialize a Modem DTE object
 *
 * @param dte Modem DTE object
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
static esp_err_t esp_modem_dte_deinit(modem_dte_t *dte)
{
    esp_modem_dte_t *esp_dte = __containerof(dte, esp_modem_dte_t, parent);
    /* Delete UART event task */
    vTaskDelete(esp_dte->uart_event_task_hdl);
    /* Delete semaphores */
    vSemaphoreDelete(esp_dte->process_sem);
    vSemaphoreDelete(esp_dte->exit_sem);
    /* Delete event loop */
    esp_event_loop_delete(esp_dte->event_loop_hdl);
    /* Uninstall UART Driver */
    uart_driver_delete(esp_dte->uart_port);
    /* Free memory */
    free(esp_dte->buffer);
    if (dte->dce) {
        dte->dce->dte = NULL;
    }
    free(esp_dte);
    return ESP_OK;
}



modem_dte_t *esp_modem_dte_init(const esp_modem_dte_config_t *config)
{
    esp_err_t res;
    /* malloc memory for esp_dte object */
    esp_modem_dte_t *esp_dte = calloc(1, sizeof(esp_modem_dte_t));
    MODEM_CHECK(esp_dte, "calloc esp_dte failed", err_dte_mem);
    /* malloc memory to storing lines from modem dce */
    esp_dte->line_buffer_size = config->line_buffer_size;
    esp_dte->buffer = calloc(1, config->line_buffer_size);
    MODEM_CHECK(esp_dte->buffer, "calloc line memory failed", err_line_mem);
    /* Set attributes */
    esp_dte->uart_port = config->port_num;
    esp_dte->parent.flow_ctrl = config->flow_control;
    /* Bind methods */
    esp_dte->parent.send_cmd = esp_modem_dte_send_cmd;
    esp_dte->parent.send_data = esp_modem_dte_send_data;
    esp_dte->parent.send_wait = esp_modem_dte_send_wait;
    esp_dte->parent.change_mode = esp_modem_dte_change_mode;
    esp_dte->parent.process_cmd_done = esp_modem_dte_process_cmd_done;
    esp_dte->parent.deinit = esp_modem_dte_deinit;

    /* Config UART */
    uart_config_t uart_config = {
        .baud_rate = config->baud_rate,
        .data_bits = config->data_bits,
        .parity = config->parity,
        .stop_bits = config->stop_bits,
        .source_clk = UART_SCLK_REF_TICK,
        .flow_ctrl = (config->flow_control == MODEM_FLOW_CONTROL_HW) ? UART_HW_FLOWCTRL_CTS_RTS : UART_HW_FLOWCTRL_DISABLE
    };
    MODEM_CHECK(uart_param_config(esp_dte->uart_port, &uart_config) == ESP_OK, "config uart parameter failed", err_uart_config);
    if (config->flow_control == MODEM_FLOW_CONTROL_HW) {
        res = uart_set_pin(esp_dte->uart_port, config->tx_io_num, config->rx_io_num,
                           config->rts_io_num, config->cts_io_num);
    } else {
        res = uart_set_pin(esp_dte->uart_port, config->tx_io_num, config->rx_io_num,
                           UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    }
    MODEM_CHECK(res == ESP_OK, "config uart gpio failed", err_uart_config);
    /* Set flow control threshold */
    if (config->flow_control == MODEM_FLOW_CONTROL_HW) {
        res = uart_set_hw_flow_ctrl(esp_dte->uart_port, UART_HW_FLOWCTRL_CTS_RTS, UART_FIFO_LEN - 8);
    } else if (config->flow_control == MODEM_FLOW_CONTROL_SW) {
        res = uart_set_sw_flow_ctrl(esp_dte->uart_port, true, 8, UART_FIFO_LEN - 8);
    }
    MODEM_CHECK(res == ESP_OK, "config uart flow control failed", err_uart_config);
    /* Install UART driver and get event queue used inside driver */
    res = uart_driver_install(esp_dte->uart_port, config->rx_buffer_size, config->tx_buffer_size,
                              config->event_queue_size, &(esp_dte->event_queue), 0);
    MODEM_CHECK(res == ESP_OK, "install uart driver failed", err_uart_config);
    res = uart_set_rx_timeout(esp_dte->uart_port, 1);
    MODEM_CHECK(res == ESP_OK, "set rx timeout failed", err_uart_config);

    /* Set pattern interrupt, used to detect the end of a line. */
    res = uart_enable_pattern_det_baud_intr(esp_dte->uart_port, '\n', 1, MIN_PATTERN_INTERVAL, MIN_POST_IDLE, MIN_PRE_IDLE);
    /* Set pattern queue size */
    esp_dte->pattern_queue_size = config->pattern_queue_size;
    res |= uart_pattern_queue_reset(esp_dte->uart_port, config->pattern_queue_size);
    /* Starting in command mode -> explicitly disable RX interrupt */
    uart_disable_rx_intr(esp_dte->uart_port);

    MODEM_CHECK(res == ESP_OK, "config uart pattern failed", err_uart_pattern);
    /* Create Event loop */
    esp_event_loop_args_t loop_args = {
        .queue_size = ESP_MODEM_EVENT_QUEUE_SIZE,
        .task_name = NULL
    };
    MODEM_CHECK(esp_event_loop_create(&loop_args, &esp_dte->event_loop_hdl) == ESP_OK, "create event loop failed", err_eloop);
    /* Create semaphore */
    esp_dte->process_sem = xSemaphoreCreateBinary();
    MODEM_CHECK(esp_dte->process_sem, "create process semaphore failed", err_sem1);
    esp_dte->exit_sem = xSemaphoreCreateBinary();
    MODEM_CHECK(esp_dte->exit_sem, "create exit semaphore failed", err_sem);

    /* Create UART Event task */
    BaseType_t ret = xTaskCreate(uart_event_task_entry,             //Task Entry
                                 "uart_event",              //Task Name
                                 config->event_task_stack_size,           //Task Stack Size(Bytes)
                                 esp_dte,                           //Task Parameter
                                 config->event_task_priority,             //Task Priority
                                 & (esp_dte->uart_event_task_hdl)   //Task Handler
                                );
    MODEM_CHECK(ret == pdTRUE, "create uart event task failed", err_tsk_create);
    return &(esp_dte->parent);
    /* Error handling */
err_tsk_create:
    vSemaphoreDelete(esp_dte->exit_sem);
err_sem:
    vSemaphoreDelete(esp_dte->process_sem);
err_sem1:
    esp_event_loop_delete(esp_dte->event_loop_hdl);
err_eloop:
    uart_disable_pattern_det_intr(esp_dte->uart_port);
err_uart_pattern:
    uart_driver_delete(esp_dte->uart_port);
err_uart_config:
    free(esp_dte->buffer);
err_line_mem:
    free(esp_dte);
err_dte_mem:
    return NULL;
}

esp_err_t esp_modem_set_event_handler(modem_dte_t *dte, esp_event_handler_t handler, int32_t event_id, void *handler_args)
{
    esp_modem_dte_t *esp_dte = __containerof(dte, esp_modem_dte_t, parent);
    return esp_event_handler_register_with(esp_dte->event_loop_hdl, ESP_MODEM_EVENT, event_id, handler, handler_args);
}

esp_err_t esp_modem_remove_event_handler(modem_dte_t *dte, esp_event_handler_t handler)
{
    esp_modem_dte_t *esp_dte = __containerof(dte, esp_modem_dte_t, parent);
    return esp_event_handler_unregister_with(esp_dte->event_loop_hdl, ESP_MODEM_EVENT, ESP_EVENT_ANY_ID, handler);
}

esp_err_t esp_modem_start_ppp(modem_dte_t *dte)
{
    modem_dce_t *dce = dte->dce;
    MODEM_CHECK(dce, "DTE has not yet bind with DCE", err);
    esp_modem_dte_t *esp_dte = __containerof(dte, esp_modem_dte_t, parent);
    /* Set PDP Context */
    MODEM_CHECK(dce->define_pdp_context(dce, 1, "IP", CONFIG_EXAMPLE_COMPONENT_MODEM_APN) == ESP_OK, "set MODEM APN failed", err);
    /* Enter PPP mode */
    MODEM_CHECK(dte->change_mode(dte, MODEM_PPP_MODE) == ESP_OK, "enter ppp mode failed", err);

    /* post PPP mode started event */
    esp_event_post_to(esp_dte->event_loop_hdl, ESP_MODEM_EVENT, ESP_MODEM_EVENT_PPP_START, NULL, 0, 0);
    return ESP_OK;
err:
    return ESP_FAIL;
}

esp_err_t esp_modem_stop_ppp(modem_dte_t *dte)
{
    modem_dce_t *dce = dte->dce;
    MODEM_CHECK(dce, "DTE has not yet bind with DCE", err);
    esp_modem_dte_t *esp_dte = __containerof(dte, esp_modem_dte_t, parent);

    /* Enter command mode */
    MODEM_CHECK(dte->change_mode(dte, MODEM_COMMAND_MODE) == ESP_OK, "enter command mode failed", err);
    /* post PPP mode stopped event */
    esp_event_post_to(esp_dte->event_loop_hdl, ESP_MODEM_EVENT, ESP_MODEM_EVENT_PPP_STOP, NULL, 0, 0);
    /* Hang up */
    MODEM_CHECK(dce->hang_up(dce) == ESP_OK, "hang up failed", err);
    /* wait for the PPP mode to exit gracefully */
    if (xSemaphoreTake(esp_dte->exit_sem, pdMS_TO_TICKS(20000)) != pdTRUE) {
        ESP_LOGW(MODEM_TAG, "Failed to exit the PPP mode gracefully");
    }
    return ESP_OK;
err:
    return ESP_FAIL;
}

esp_err_t esp_modem_notify_ppp_netif_closed(modem_dte_t *dte)
{
    esp_modem_dte_t *esp_dte = __containerof(dte, esp_modem_dte_t, parent);
    return xSemaphoreGive(esp_dte->exit_sem) == pdTRUE ? ESP_OK : ESP_FAIL;
}
