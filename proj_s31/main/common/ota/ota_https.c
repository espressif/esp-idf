#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/unistd.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_http_client.h"
#include "ota.h"

static const char *TAG = "OTA_HTTPS";
#define HTTPS_OTA_BUF_SIZE 4096

typedef struct _T_OTA_HTTPS_CTX
{
    ota_start_cb    start;
    ota_write_cb    write;
    ota_stop_cb     stop;
    void           *param;
    char           *url;
    bool            is_running;
    TaskHandle_t    task_handle;
}T_OTA_HTTPS_CTX;

static void https_task(void *arg)
{
    T_OTA_HTTPS_CTX *pCtx = (T_OTA_HTTPS_CTX *)arg;
    esp_http_client_handle_t client = NULL;
    esp_err_t err = ESP_OK;
    char *buf = NULL;
    bool ota_begun = false;

    if(pCtx == NULL)
    {
        vTaskDelete(NULL);
        ESP_LOGE(TAG, "Invalid ota https context");
        return;
    }

    err = pCtx->start(pCtx->param);
    if(err != ESP_OK)
    {
        err = ESP_FAIL;
        ESP_LOGE(TAG, "Failed to start ota https");
        goto exit;
    }

    ota_begun = true;

    esp_http_client_config_t config = 
    {
        .url = pCtx->url,
        .timeout_ms = 15000,
    };

    client = esp_http_client_init(&config); // init http client
    if(client == NULL)
    {
        err = ESP_FAIL;
        ESP_LOGE(TAG, "Failed to init http client");
        goto exit;
    }

    err = esp_http_client_open(client, 0); // open http connection
    if(err != ESP_OK)
    {
        goto exit;
    }

    int content_length = esp_http_client_fetch_headers(client);
    int status = esp_http_client_get_status_code(client);
    if(status != 200)
    {
        ESP_LOGE(TAG, "HTTP status %d, len=%d", status, content_length);
        err = ESP_FAIL;
        goto exit;
    }

    buf = malloc(HTTPS_OTA_BUF_SIZE);
    if(buf == NULL)
    {
        err = ESP_ERR_NO_MEM;
        goto exit;
    }

    for(;;)
    {
        int n = esp_http_client_read(client, buf, HTTPS_OTA_BUF_SIZE);
        if (n < 0) 
        {
            err = ESP_FAIL;
            break;
        }
        if (n == 0) 
        {          // EOF
            err = ESP_OK;
            break;
        }
        err = pCtx->write(pCtx->param, (const uint8_t *)buf, (size_t)n);
        if (err != ESP_OK) 
        {
            break;
        }
    }
exit:
    if(buf != NULL)
    {
        free(buf);
        buf = NULL;
    }
    if(client != NULL)
    {
        esp_http_client_close(client);
        esp_http_client_cleanup(client);
    }

    // start fail 
    if(pCtx->stop != NULL)
    {
        pCtx->stop(pCtx->param, (err == ESP_OK) && ota_begun);
    }

    pCtx->is_running = false;
    pCtx->task_handle = NULL;
    vTaskDelete(NULL);
    return;
}

static esp_err_t __start(void *ctx, ota_start_cb start_cb, ota_write_cb write_cb, ota_stop_cb stop_cb, void *param)
{
    T_OTA_HTTPS_CTX *pCtx = (T_OTA_HTTPS_CTX *)ctx;
    if(pCtx == NULL)
    {
        ESP_LOGE(TAG, "Invalid ota https context");
        return ESP_FAIL;
    }

    T_HTTPS_OTA_PARAM *pParam = (T_HTTPS_OTA_PARAM *)param;
    if(pParam == NULL)
    {
        ESP_LOGE(TAG, "Invalid ota https parameter");
        return ESP_FAIL;
    }
    pCtx->url  = calloc(1, strlen(pParam->url) + 1);
    if(pCtx->url == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate memory for ota https url");
        return ESP_FAIL;
    }
    strncpy(pCtx->url, pParam->url, strlen(pParam->url));

    pCtx->start = start_cb;
    pCtx->write = write_cb;
    pCtx->stop = stop_cb;
    pCtx->param = pParam->func_param;
    pCtx->is_running = true;
    pCtx->task_handle = NULL;

    if(xTaskCreate(https_task, "https_task", 8192, pCtx, 5, &pCtx->task_handle) != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to create https task");
        free(pCtx->url);
        pCtx->is_running = false;
        pCtx->task_handle = NULL;
        pCtx->url = NULL;
        return ESP_FAIL;
    }

    return ESP_OK;
}

static esp_err_t __stop(void *ctx)
{
    T_OTA_HTTPS_CTX *pCtx = (T_OTA_HTTPS_CTX *)ctx;
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
    T_OTA_HTTPS_CTX *pCtx = (T_OTA_HTTPS_CTX *)ctx;
    if(pCtx == NULL)
    {
        ESP_LOGE(TAG, "Invalid ota https context");
        return ESP_FAIL;
    }

    if(pCtx->is_running)
    {
        pCtx->stop(pCtx->param, false);
    }

    if(pCtx->url != NULL)
    {
        free(pCtx->url);
        pCtx->url = NULL;
    }
    free(pCtx);
    pCtx = NULL;
    return ESP_OK;
}

T_OTA_MODE_CTX *ota_https_init(void)
{
    T_OTA_MODE_CTX *pMode = (T_OTA_MODE_CTX *)calloc(1, sizeof(T_OTA_MODE_CTX));
    if(pMode == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate memory for ota https context");
        return NULL;
    }

    pMode->start = __start;
    pMode->stop = __stop;
    pMode->deinit = __deinit;
    
    T_OTA_HTTPS_CTX *pCtx = (T_OTA_HTTPS_CTX *)calloc(1, sizeof(T_OTA_HTTPS_CTX));
    if(pCtx == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate memory for ota https context");
        goto fail;
    }
    
    pMode->ctx = (void *)pCtx;
    return pMode;

fail:
    if(pMode != NULL)
    {
        if(pMode->ctx != NULL)
        {
            if(pCtx->url != NULL)
            {
                free(pCtx->url);
                pCtx->url = NULL;
            }
            free(pMode->ctx);
            pMode->ctx = NULL;
        }
        free(pMode);
        pMode = NULL;
    }
    return NULL;
}
