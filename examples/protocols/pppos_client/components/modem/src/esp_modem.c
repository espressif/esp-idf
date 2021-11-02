/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
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

/**
 * @brief This sets the threshold for receiving data events when UART RX buffer reaches
 * this level. Decreasing the number causes more events and lowers changes of UART overflows,
 * but more allocations in lwIP. You can increase this number if you're using slower baudrates
 * or having the UART ISR in IRAM.
 */
#define ESP_MODEM_UART_RX_FULL_THRESHOLD (64)

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
    int buffer_size;                        /*!< internal buffer size */
    int consumed;                           /*!< index to the consumed buffer pointer */
} esp_modem_dte_t;

/**
 * @brief Returns true if the supplied string contains only <CR> characters
 *
 * @param str string to check
 * @param len length of string
 */
static inline bool is_only_cr(const char *str, uint32_t len)
{
    for (int i=0; i<len; ++i) {
        if (str[i] != '\r') {
            return false;
        }
    }
    return true;
}

static inline void report_unknown_line(esp_modem_dte_t *esp_dte, char *line, int line_len)
{
    /* Send ESP_MODEM_EVENT_UNKNOWN signal to event loop */
    esp_event_post_to(esp_dte->event_loop_hdl, ESP_MODEM_EVENT, ESP_MODEM_EVENT_UNKNOWN,
                      (void *)line, line_len + 1, pdMS_TO_TICKS(100));
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
static esp_err_t esp_dte_handle_line(esp_modem_dte_t *esp_dte, char * line, size_t len, char separator)
{
    esp_err_t err = ESP_FAIL;
    modem_dce_t *dce = esp_dte->parent.dce;
    MODEM_CHECK(dce, "DTE has not yet bind with DCE", err);
    if (separator != '\n' && dce->handle_line) {
        /* If waiting for a specific separator, just pass the entire string */
        MODEM_CHECK(dce->handle_line(dce, line) == ESP_OK, "handle line failed", post_event_unknown);
        return ESP_OK;
    }
    /* Tokenize the data to call handlers separately for each *line* */
    char *str_ptr = NULL;
    char *p = strtok_r(line, "\n", &str_ptr);
    while (p) {
        int plen = strlen(p);
        if (plen > 1 && !is_only_cr(p, plen)) {
            ESP_LOGD(MODEM_TAG, "Handling line: >>%s\n<<", p);
            if (dce->handle_line == NULL) {
                /* Received an asynchronous line, but no handler waiting this this */
                ESP_LOGD(MODEM_TAG, "No handler for line: %s", p);
                report_unknown_line(esp_dte, p, plen);
                return ESP_OK; /* Not an error, just propagate the line to user handler */
            }
            if (dce->handle_line(dce, p) != ESP_OK) {
                ESP_LOGE(MODEM_TAG, "handle line failed");
                report_unknown_line(esp_dte, p, plen);
            }
        }
        p = strtok_r(NULL, "\n", &str_ptr);
    }
    return ESP_OK;
post_event_unknown:
    report_unknown_line(esp_dte, line, strlen(line));
err:
    return err;
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
    ESP_LOGV(MODEM_TAG, "uart_get_buffered_data_len()=%d", length);
    if (esp_dte->parent.dce->mode != MODEM_PPP_MODE && length) {
        // Read the data and process it using `handle_line` logic
        length = MIN(esp_dte->buffer_size - 1, length);
        length = uart_read_bytes(esp_dte->uart_port, esp_dte->buffer + esp_dte->consumed, length - esp_dte->consumed, portMAX_DELAY);
        const char separator = esp_dte->parent.dce->prompt == NULL ? '\n' : (esp_dte->parent.dce->prompt)[strlen(esp_dte->parent.dce->prompt)-1];
        if (memchr(esp_dte->buffer + esp_dte->consumed, separator, length)) {
            esp_dte->buffer[length] = '\0';
            ESP_LOG_BUFFER_HEXDUMP("esp-modem: pattern-detection", esp_dte->buffer, length, ESP_LOG_VERBOSE);
            if (esp_dte->parent.dce->handle_line) {
                /* Send new line to handle if handler registered */
                if (esp_dte_handle_line(esp_dte, (char*)esp_dte->buffer, length, separator) == ESP_OK) {
                    esp_dte->consumed = 0;
                    return;
                }
            }
            esp_dte->consumed += length;
        }
        return;
    }

    length = MIN(esp_dte->buffer_size, length);
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
            default:
                ESP_LOGW(MODEM_TAG, "unknown uart event type: %d", event.type);
                break;
            }
        }
    }
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
    ESP_LOGD(MODEM_TAG, "Sending command:%s", command);
    MODEM_CHECK(dce, "DTE has not yet bind with DCE", errdce);
    MODEM_CHECK(command, "command is NULL", err);
    esp_modem_dte_t *esp_dte = __containerof(dte, esp_modem_dte_t, parent);
    esp_dte->consumed = 0;
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
errdce:
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
 * @brief Handle response from send data and wait from prompt.
 */
static esp_err_t esp_modem_dte_send_wait_default_handler(modem_dce_t *dce, const char *line)
{
    esp_err_t err = ESP_FAIL;
    if (!strncmp(line, dce->prompt, strlen(dce->prompt))) {
        dce->state = MODEM_STATE_SUCCESS;
        err = dce->dte->process_cmd_done(dce->dte);
    } else {
        dce->state = MODEM_STATE_FAIL;
        err = dce->dte->process_cmd_done(dce->dte);
    }
    return err;
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
    modem_dce_t *dce = dte->dce;
    MODEM_CHECK(dce, "DTE has not yet bind with DCE", err_param);
    dce->prompt = prompt;   // the last character of this prompt will be used as a separator to call the line handker
    dce->handle_line = esp_modem_dte_send_wait_default_handler;
    MODEM_CHECK(dte->send_cmd(dte, data, timeout) == ESP_OK, "wait for prompt timeout", err);
    MODEM_CHECK(dce->state == MODEM_STATE_SUCCESS, "wait for prompt failed", err);
    dce->prompt = NULL;
    return ESP_OK;
err:
    dce->prompt = NULL;
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
    modem_mode_t current_mode = dce->mode;
    MODEM_CHECK(current_mode != new_mode, "already in mode: %d", err, new_mode);
    dce->mode = MODEM_TRANSITION_MODE;  // mode switching will be finished in set_working_mode() on success
                                        // (or restored on failure)
    MODEM_CHECK(dce->set_working_mode(dce, new_mode) == ESP_OK, "set new working mode:%d failed", err_restore_mode, new_mode);
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
    esp_dte->buffer_size = config->dte_buffer_size;
    esp_dte->buffer = calloc(1, config->dte_buffer_size);
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
#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2
        .source_clk = UART_SCLK_REF_TICK,
#else
        .source_clk = UART_SCLK_XTAL,
#endif
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

    res = uart_set_rx_full_threshold(config->port_num, ESP_MODEM_UART_RX_FULL_THRESHOLD);
    MODEM_CHECK(res == ESP_OK, "config rx full threshold failed", err_uart_config);

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
