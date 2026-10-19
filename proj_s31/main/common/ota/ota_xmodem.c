#include <string.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "driver/uart.h"
#include "hal/uart_types.h"
#include "xymodem.h"
#include "ota.h"
#include "sdkconfig.h"

static const char *TAG = "OTA_XMODEM";
typedef struct _T_OTA_XMODEM_CTX
{
    ota_start_cb    start;
    ota_write_cb    write;
    ota_stop_cb     stop;
    void           *param;
    uart_port_t     uart_port;
    bool            is_running;
    TaskHandle_t    task_handle;
}T_OTA_XMODEM_CTX;

static esp_err_t uart_recv(void *ctx, void *buf, size_t len, size_t *recv_len, uint32_t timeout_ms)
{
    T_OTA_XMODEM_CTX *pCtx = (T_OTA_XMODEM_CTX *)(intptr_t)ctx;
    int n = uart_read_bytes(pCtx->uart_port, buf, len, pdMS_TO_TICKS(timeout_ms));
    if(n < 0)
    {
        return ESP_FAIL;
    }
    *recv_len = (size_t)n;
    return ESP_OK;
}

static esp_err_t uart_send(void *ctx, const void *buf, size_t len)
{
    T_OTA_XMODEM_CTX *pCtx = (T_OTA_XMODEM_CTX *)(intptr_t)ctx;
    int n = uart_write_bytes(pCtx->uart_port, buf, len);
    return (n == (int)len) ? ESP_OK : ESP_FAIL;
}

static esp_err_t sink_write(void *ctx, const void *buf, size_t len)
{
    T_OTA_XMODEM_CTX *pCtx = (T_OTA_XMODEM_CTX *)ctx;
    return pCtx->write(pCtx->param, buf, len);
}

static void xmodem_task(void *arg)
{
    T_OTA_XMODEM_CTX *pCtx = (T_OTA_XMODEM_CTX *)arg;
    esp_err_t err = ESP_OK;
    if(pCtx == NULL)
    {
        ESP_LOGE(TAG, "Invalid ota xmodem context");
        vTaskDelete(NULL);
        return;
    }

    err = pCtx->start(pCtx->param);
    if(err == ESP_OK)
    {
        ESP_LOGI(TAG, "XMODEM OTA started");
    
        xymodem_transport_t transport = 
        {
            .recv = uart_recv,
            .send = uart_send,
            .ctx = pCtx
        };

        xmodem_data_sink_t sink = {
            .write = sink_write,
            .ctx   = pCtx,
        };

        xymodem_recv_config_t cfg = XYMODEM_RECV_CONFIG_DEFAULT();

        err = xmodem_recv(&transport, &cfg, &sink);  // 阻塞到传完/失败
        if(err != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to receive ota xmodem: %s", esp_err_to_name(err));
        }
    }
    pCtx->stop(pCtx->param, err == ESP_OK);
    uart_driver_delete(pCtx->uart_port);
    pCtx->is_running = false;
    pCtx->task_handle = NULL;
    vTaskDelete(NULL);
    return;
}

static esp_err_t __start(void *ctx, ota_start_cb start_cb, ota_write_cb write_cb, ota_stop_cb stop_cb, void *param)
{
    T_OTA_XMODEM_CTX *pCtx = (T_OTA_XMODEM_CTX *)ctx;
    if(pCtx == NULL)
    {
        ESP_LOGE(TAG, "Invalid ota xmodem context");
        return ESP_FAIL;
    }

    pCtx->start = start_cb;
    pCtx->write = write_cb;
    pCtx->stop = stop_cb;
    pCtx->param = param;

    // start xmodem uart
    if(uart_is_driver_installed(CONFIG_XMODEM_OTA_UART_PORT))
    {
        ESP_LOGE(TAG, "XMODEM UART driver is already used");
        return ESP_ERR_INVALID_STATE;
    }
    uart_config_t uart_config = 
    {
        .baud_rate = CONFIG_XMODEM_OTA_UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    if(uart_param_config(CONFIG_XMODEM_OTA_UART_PORT, &uart_config) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to configure uart parameters");
        return ESP_ERR_INVALID_STATE;
    }

    if(uart_set_pin(CONFIG_XMODEM_OTA_UART_PORT, CONFIG_XMODEM_OTA_UART_TX_PIN, CONFIG_XMODEM_OTA_UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set uart pins");
        return ESP_ERR_INVALID_STATE;
    }

    if(uart_driver_install(CONFIG_XMODEM_OTA_UART_PORT, 2048, 2048, 10, NULL, 0) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to install uart driver");
        return ESP_ERR_INVALID_STATE;
    }

    pCtx->uart_port = CONFIG_XMODEM_OTA_UART_PORT;
    pCtx->is_running = true;

    if(xTaskCreate(xmodem_task, "xmodem_task", 8192, pCtx, 5, &pCtx->task_handle) != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to create xmodem task");
        uart_driver_delete(pCtx->uart_port);
        pCtx->is_running = false;
        return ESP_ERR_INVALID_STATE;
    }
    return ESP_OK;
}

static esp_err_t __stop(void *ctx)
{
    T_OTA_XMODEM_CTX *pCtx = (T_OTA_XMODEM_CTX *)ctx;
    if (pCtx == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!pCtx->is_running || pCtx->task_handle == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    /* 无法安全打断 xmodem_recv；取消请停对端或复位 */
    ESP_LOGW(TAG, "OTA cancel not supported while xmodem_recv is running");
    return ESP_ERR_NOT_SUPPORTED;
}

static esp_err_t __deinit(void *ctx)
{
    T_OTA_XMODEM_CTX *pCtx = (T_OTA_XMODEM_CTX *)ctx;
    if(pCtx == NULL)
    {
        ESP_LOGE(TAG, "Invalid ota xmodem context");
        return ESP_FAIL;
    }

    if(pCtx->is_running)
    {// if ota is running, stop it first
        pCtx->stop(pCtx->param, false); // fail to stop
    }
    
    free(pCtx);
    pCtx = NULL;
    return ESP_OK;
}

T_OTA_MODE_CTX *ota_xmodem_init(void)
{
    T_OTA_MODE_CTX *pMode = (T_OTA_MODE_CTX *)calloc(1, sizeof(T_OTA_MODE_CTX));
    if(pMode == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate memory for ota mode");
        goto fail;
    }

    pMode->start  = __start;
    pMode->stop   = __stop;
    pMode->deinit = __deinit;

    T_OTA_XMODEM_CTX *pCtx = (T_OTA_XMODEM_CTX *)calloc(1, sizeof(T_OTA_XMODEM_CTX));
    if(pCtx == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate memory for ota xmodem context");
        goto fail;
    }
    pMode->ctx = (void *)pCtx;
    return pMode;

fail:
    if(pMode->ctx != NULL)
    {
        free(pMode->ctx);
        pMode->ctx = NULL;
    }

    if(pMode != NULL)
    {
        free(pMode);
        pMode = NULL;
    }

    return NULL;
}