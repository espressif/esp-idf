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
#include "netif/ppp/pppapi.h"
#include "netif/ppp/pppos.h"
#include "lwip/dns.h"
#include "esp_netif.h"
#include "esp_modem.h"
#include "esp_log.h"
#include "sdkconfig.h"

#define ESP_MODEM_LINE_BUFFER_SIZE (CONFIG_EXAMPLE_UART_RX_BUFFER_SIZE / 2)
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
    struct netif pppif;                     /*!< PPP network interface */
    ppp_pcb *ppp;                           /*!< PPP control block */
    modem_dte_t parent;                     /*!< DTE interface that should extend */
} esp_modem_dte_t;

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
    modem_dce_t *dce = esp_dte->parent.dce;
    MODEM_CHECK(dce, "DTE has not yet bind with DCE", err);
    const char *line = (const char *)(esp_dte->buffer);
    /* Skip pure "\r\n" lines */
    if (strlen(line) > 2) {
        MODEM_CHECK(dce->handle_line, "no handler for line", err_handle);
        MODEM_CHECK(dce->handle_line(dce, line) == ESP_OK, "handle line failed", err_handle);
    }
    return ESP_OK;
err_handle:
    /* Send MODEM_EVENT_UNKNOWN signal to event loop */
    esp_event_post_to(esp_dte->event_loop_hdl, ESP_MODEM_EVENT, MODEM_EVENT_UNKNOWN,
                      (void *)line, strlen(line) + 1, pdMS_TO_TICKS(100));
err:
    return ESP_FAIL;
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
    if (pos != -1) {
        if (pos < ESP_MODEM_LINE_BUFFER_SIZE - 1) {
            /* read one line(include '\n') */
            read_len = pos + 1;
        } else {
            ESP_LOGW(MODEM_TAG, "ESP Modem Line buffer too small");
            read_len = ESP_MODEM_LINE_BUFFER_SIZE - 1;
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
        ESP_LOGW(MODEM_TAG, "Pattern Queue Size too small");
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
    length = MIN(ESP_MODEM_LINE_BUFFER_SIZE, length);
    length = uart_read_bytes(esp_dte->uart_port, esp_dte->buffer, length, portMAX_DELAY);
    /* pass input data to the lwIP core thread */
    if (length) {
        pppos_input_tcpip(esp_dte->ppp, esp_dte->buffer, length);
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
        if (xQueueReceive(esp_dte->event_queue, &event, pdMS_TO_TICKS(100))) {
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
        /* Drive the event loop */
        esp_event_loop_run(esp_dte->event_loop_hdl, pdMS_TO_TICKS(50));
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
    MODEM_CHECK(dce->mode != new_mode, "already in mode: %d", err, new_mode);
    switch (new_mode) {
    case MODEM_PPP_MODE:
        MODEM_CHECK(dce->set_working_mode(dce, new_mode) == ESP_OK, "set new working mode:%d failed", err, new_mode);
        uart_disable_pattern_det_intr(esp_dte->uart_port);
        uart_enable_rx_intr(esp_dte->uart_port);
        break;
    case MODEM_COMMAND_MODE:
        uart_disable_rx_intr(esp_dte->uart_port);
        uart_flush(esp_dte->uart_port);
        uart_enable_pattern_det_baud_intr(esp_dte->uart_port, '\n', 1, MIN_PATTERN_INTERVAL, MIN_POST_IDLE, MIN_PRE_IDLE);
        uart_pattern_queue_reset(esp_dte->uart_port, CONFIG_EXAMPLE_UART_PATTERN_QUEUE_SIZE);
        MODEM_CHECK(dce->set_working_mode(dce, new_mode) == ESP_OK, "set new working mode:%d failed", err, new_mode);
        break;
    default:
        break;
    }
    return ESP_OK;
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
    /* Delete semaphore */
    vSemaphoreDelete(esp_dte->process_sem);
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
    esp_dte->buffer = calloc(1, ESP_MODEM_LINE_BUFFER_SIZE);
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
        .flow_ctrl = (config->flow_control == MODEM_FLOW_CONTROL_HW) ? UART_HW_FLOWCTRL_CTS_RTS : UART_HW_FLOWCTRL_DISABLE
    };
    MODEM_CHECK(uart_param_config(esp_dte->uart_port, &uart_config) == ESP_OK, "config uart parameter failed", err_uart_config);
    if (config->flow_control == MODEM_FLOW_CONTROL_HW) {
        res = uart_set_pin(esp_dte->uart_port, CONFIG_EXAMPLE_UART_MODEM_TX_PIN, CONFIG_EXAMPLE_UART_MODEM_RX_PIN,
                           CONFIG_EXAMPLE_UART_MODEM_RTS_PIN, CONFIG_EXAMPLE_UART_MODEM_CTS_PIN);
    } else {
        res = uart_set_pin(esp_dte->uart_port, CONFIG_EXAMPLE_UART_MODEM_TX_PIN, CONFIG_EXAMPLE_UART_MODEM_RX_PIN,
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
    res = uart_driver_install(esp_dte->uart_port, CONFIG_EXAMPLE_UART_RX_BUFFER_SIZE, CONFIG_EXAMPLE_UART_TX_BUFFER_SIZE,
                              CONFIG_EXAMPLE_UART_EVENT_QUEUE_SIZE, &(esp_dte->event_queue), 0);
    MODEM_CHECK(res == ESP_OK, "install uart driver failed", err_uart_config);
    /* Set pattern interrupt, used to detect the end of a line. */
    res = uart_enable_pattern_det_baud_intr(esp_dte->uart_port, '\n', 1, MIN_PATTERN_INTERVAL, MIN_POST_IDLE, MIN_PRE_IDLE);
    /* Set pattern queue size */
    res |= uart_pattern_queue_reset(esp_dte->uart_port, CONFIG_EXAMPLE_UART_PATTERN_QUEUE_SIZE);
    MODEM_CHECK(res == ESP_OK, "config uart pattern failed", err_uart_pattern);
    /* Create Event loop */
    esp_event_loop_args_t loop_args = {
        .queue_size = ESP_MODEM_EVENT_QUEUE_SIZE,
        .task_name = NULL
    };
    MODEM_CHECK(esp_event_loop_create(&loop_args, &esp_dte->event_loop_hdl) == ESP_OK, "create event loop failed", err_eloop);
    /* Create semaphore */
    esp_dte->process_sem = xSemaphoreCreateBinary();
    MODEM_CHECK(esp_dte->process_sem, "create process semaphore failed", err_sem);
    /* Create UART Event task */
    BaseType_t ret = xTaskCreate(uart_event_task_entry,             //Task Entry
                                 "uart_event",                      //Task Name
                                 CONFIG_EXAMPLE_UART_EVENT_TASK_STACK_SIZE, //Task Stack Size(Bytes)
                                 esp_dte,                           //Task Parameter
                                 CONFIG_EXAMPLE_UART_EVENT_TASK_PRIORITY,   //Task Priority
                                 & (esp_dte->uart_event_task_hdl)   //Task Handler
                                );
    MODEM_CHECK(ret == pdTRUE, "create uart event task failed", err_tsk_create);
    return &(esp_dte->parent);
    /* Error handling */
err_tsk_create:
    vSemaphoreDelete(esp_dte->process_sem);
err_sem:
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

esp_err_t esp_modem_add_event_handler(modem_dte_t *dte, esp_event_handler_t handler, void *handler_args)
{
    esp_modem_dte_t *esp_dte = __containerof(dte, esp_modem_dte_t, parent);
    return esp_event_handler_register_with(esp_dte->event_loop_hdl, ESP_MODEM_EVENT, ESP_EVENT_ANY_ID, handler, handler_args);
}

esp_err_t esp_modem_remove_event_handler(modem_dte_t *dte, esp_event_handler_t handler)
{
    esp_modem_dte_t *esp_dte = __containerof(dte, esp_modem_dte_t, parent);
    return esp_event_handler_unregister_with(esp_dte->event_loop_hdl, ESP_MODEM_EVENT, ESP_EVENT_ANY_ID, handler);
}

/**
 * @brief PPP status callback which is called on PPP status change (up, down, …) by lwIP core thread
 *
 * @param pcb PPP control block
 * @param err_code Error code
 * @param ctx Context of callback
 */
static void on_ppp_status_changed(ppp_pcb *pcb, int err_code, void *ctx)
{
    struct netif *pppif = ppp_netif(pcb);
    const ip_addr_t *dest_ip = NULL;
    modem_dte_t *dte = (modem_dte_t *)(ctx);
    esp_modem_dte_t *esp_dte = __containerof(dte, esp_modem_dte_t, parent);
    ppp_client_ip_info_t ipinfo = {0};
    switch (err_code) {
    case PPPERR_NONE: /* Connected */
        ipinfo.ip = pppif->ip_addr.u_addr.ip4;
        ipinfo.gw = pppif->gw.u_addr.ip4;
        ipinfo.netmask = pppif->netmask.u_addr.ip4;
        dest_ip = dns_getserver(0);
        if(dest_ip != NULL){
          ipinfo.ns1 = (*dest_ip).u_addr.ip4;
        }
        dest_ip = dns_getserver(1);
        if(dest_ip != NULL){
          ipinfo.ns2 = (*dest_ip).u_addr.ip4;
        }
        esp_event_post_to(esp_dte->event_loop_hdl, ESP_MODEM_EVENT, MODEM_EVENT_PPP_CONNECT, &ipinfo, sizeof(ipinfo), 0);
        break;
    case PPPERR_PARAM:
        ESP_LOGE(MODEM_TAG, "Invalid parameter");
        break;
    case PPPERR_OPEN:
        ESP_LOGE(MODEM_TAG, "Unable to open PPP session");
        break;
    case PPPERR_DEVICE:
        ESP_LOGE(MODEM_TAG, "Invalid I/O device for PPP");
        break;
    case PPPERR_ALLOC:
        ESP_LOGE(MODEM_TAG, "Unable to allocate resources");
        break;
    case PPPERR_USER: /* User interrupt */
        esp_event_post_to(esp_dte->event_loop_hdl, ESP_MODEM_EVENT, MODEM_EVENT_PPP_STOP, NULL, 0, 0);
        /* Free the PPP control block */
        pppapi_free(esp_dte->ppp);
        break;
    case PPPERR_CONNECT: /* Connection lost */
        esp_event_post_to(esp_dte->event_loop_hdl, ESP_MODEM_EVENT, MODEM_EVENT_PPP_DISCONNECT, NULL, 0, 0);
        break;
    case PPPERR_AUTHFAIL:
        ESP_LOGE(MODEM_TAG, "Failed authentication challenge");
        break;
    case PPPERR_PROTOCOL:
        ESP_LOGE(MODEM_TAG, "Failed to meet protocol");
        break;
    case PPPERR_PEERDEAD:
        ESP_LOGE(MODEM_TAG, "Connection timeout");
        break;
    case PPPERR_IDLETIMEOUT:
        ESP_LOGE(MODEM_TAG, "Idle Timeout");
        break;
    case PPPERR_CONNECTTIME:
        ESP_LOGE(MODEM_TAG, "Max connect time reached");
        break;
    case PPPERR_LOOPBACK:
        ESP_LOGE(MODEM_TAG, "Loopback detected");
        break;
    default:
        ESP_LOGE(MODEM_TAG, "Unknown error code %d", err_code);
        break;
    }
}

#if PPP_NOTIFY_PHASE
/**
 * @brief Notify phase callback which is called on each PPP internal state change
 *
 * @param pcb PPP control block
 * @param phase Phase ID
 * @param ctx Context of callback
 */
static void on_ppp_notify_phase(ppp_pcb *pcb, u8_t phase, void *ctx)
{
    switch (phase) {
    case PPP_PHASE_DEAD:
        ESP_LOGD(MODEM_TAG, "Phase Dead");
        break;
    case PPP_PHASE_INITIALIZE:
        ESP_LOGD(MODEM_TAG, "Phase Start");
        break;
    case PPP_PHASE_ESTABLISH:
        ESP_LOGD(MODEM_TAG, "Phase Establish");
        break;
    case PPP_PHASE_AUTHENTICATE:
        ESP_LOGD(MODEM_TAG, "Phase Authenticate");
        break;
    case PPP_PHASE_NETWORK:
        ESP_LOGD(MODEM_TAG, "Phase Network");
        break;
    case PPP_PHASE_RUNNING:
        ESP_LOGD(MODEM_TAG, "Phase Running");
        break;
    case PPP_PHASE_TERMINATE:
        ESP_LOGD(MODEM_TAG, "Phase Terminate");
        break;
    case PPP_PHASE_DISCONNECT:
        ESP_LOGD(MODEM_TAG, "Phase Disconnect");
        break;
    default:
        ESP_LOGW(MODEM_TAG, "Phase Unknown: %d", phase);
        break;
    }
}
#endif

/**
 * @brief PPPoS serial output callback
 *
 * @param pcb PPP control block
 * @param data Buffer to write to serial port
 * @param len Length of the data buffer
 * @param ctx Context of callback
 * @return uint32_t Length of data successfully sent
 */
static uint32_t pppos_low_level_output(ppp_pcb *pcb, uint8_t *data, uint32_t len, void *ctx)
{
    modem_dte_t *dte = (modem_dte_t *)ctx;
    return dte->send_data(dte, (const char *)data, len);
}

esp_err_t esp_modem_setup_ppp(modem_dte_t *dte)
{
    modem_dce_t *dce = dte->dce;
    MODEM_CHECK(dce, "DTE has not yet bind with DCE", err);
    esp_modem_dte_t *esp_dte = __containerof(dte, esp_modem_dte_t, parent);
    /* Set PDP Context */
    MODEM_CHECK(dce->define_pdp_context(dce, 1, "IP", CONFIG_EXAMPLE_MODEM_APN) == ESP_OK, "set MODEM APN failed", err);
    /* Enter PPP mode */
    MODEM_CHECK(dte->change_mode(dte, MODEM_PPP_MODE) == ESP_OK, "enter ppp mode failed", err);
    /* Create PPPoS interface */
    esp_dte->ppp = pppapi_pppos_create(&(esp_dte->pppif), pppos_low_level_output, on_ppp_status_changed, dte);
    MODEM_CHECK(esp_dte->ppp, "create pppos interface failed", err);
#if PPP_NOTIFY_PHASE
    ppp_set_notify_phase_callback(esp_dte->ppp, on_ppp_notify_phase);
#endif
    /* Initiate PPP client connection */
    /* Set default route */
    MODEM_CHECK(pppapi_set_default(esp_dte->ppp) == ERR_OK, "set default route failed", err);
    /* Ask the peer for up to 2 DNS server addresses */
    ppp_set_usepeerdns(esp_dte->ppp, 1);
    /* Auth configuration */
#if PAP_SUPPORT
    pppapi_set_auth(esp_dte->ppp, PPPAUTHTYPE_PAP, CONFIG_EXAMPLE_MODEM_PPP_AUTH_USERNAME, CONFIG_EXAMPLE_MODEM_PPP_AUTH_PASSWORD);
#elif CHAP_SUPPORT
    pppapi_set_auth(esp_dte->ppp, PPPAUTHTYPE_CHAP, CONFIG_EXAMPLE_MODEM_PPP_AUTH_USERNAME, CONFIG_EXAMPLE_MODEM_PPP_AUTH_PASSWORD);
#else
#error "Unsupported AUTH Negotiation"
#endif
    /* Initiate PPP negotiation, without waiting */
    MODEM_CHECK(pppapi_connect(esp_dte->ppp, 0) == ERR_OK, "initiate ppp negotiation failed", err);
    esp_event_post_to(esp_dte->event_loop_hdl, ESP_MODEM_EVENT, MODEM_EVENT_PPP_START, NULL, 0, 0);
    return ESP_OK;
err:
    return ESP_FAIL;
}

esp_err_t esp_modem_exit_ppp(modem_dte_t *dte)
{
    modem_dce_t *dce = dte->dce;
    MODEM_CHECK(dce, "DTE has not yet bind with DCE", err);
    esp_modem_dte_t *esp_dte = __containerof(dte, esp_modem_dte_t, parent);
    /* Shutdown of PPP protocols */
    MODEM_CHECK(pppapi_close(esp_dte->ppp, 0) == ERR_OK, "close ppp connection failed", err);
    /* Enter command mode */
    MODEM_CHECK(dte->change_mode(dte, MODEM_COMMAND_MODE) == ESP_OK, "enter command mode failed", err);
    /* Hang up */
    MODEM_CHECK(dce->hang_up(dce) == ESP_OK, "hang up failed", err);
    return ESP_OK;
err:
    return ESP_FAIL;
}
