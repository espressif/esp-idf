#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include "esp_err.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_partition.h"
#include "ota.h"

static const char *TAG = "OTA";

typedef enum _E_OTA_STATE
{
    OTA_S_INIT = 0,
    OTA_S_DOWNLOADING,
    OTA_S_VERIFYING,
    OTA_S_APPLYING,
    OTA_S_DONE,
    OTA_S_ERROR,
}E_OTA_STATE;

typedef struct _T_OTA_CTRL
{
    T_OTA_MODE_CTX         *mode;
    E_OTA_STATE             state;
    const esp_partition_t  *partition;
    esp_ota_handle_t        ota_handle;
    bool                    handle_valid;
    size_t                  bytes_written;
}T_OTA_CTRL;

static T_OTA_CTRL *p_ctrl = NULL;

/* external ota module functions */
extern T_OTA_MODE_CTX *ota_https_init(void);
extern T_OTA_MODE_CTX *ota_xmodem_init(void);

// start callback function
static esp_err_t __start_cb(void *param)
{
    T_OTA_CTRL *pCtrl = (T_OTA_CTRL *)param;
    if(pCtrl == NULL)
    {
        ESP_LOGE(TAG, "Invalid ota controller");
        return ESP_FAIL;
    }

    pCtrl->partition = esp_ota_get_next_update_partition(NULL);
    if(pCtrl->partition == NULL)
    {
        ESP_LOGE(TAG, "Failed to get next update partition");
        return ESP_FAIL;
    }

    esp_err_t err = esp_ota_begin(pCtrl->partition, OTA_WITH_SEQUENTIAL_WRITES, &pCtrl->ota_handle);
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to begin ota: %d", err);
        return err;
    }

    pCtrl->handle_valid = true;
    pCtrl->state = OTA_S_DOWNLOADING;

    return ESP_OK;
}

// write data to flash
static esp_err_t __write_cb(void *param, const uint8_t *data, size_t size)
{
    T_OTA_CTRL *pCtrl = (T_OTA_CTRL *)param;
    if(pCtrl == NULL)
    {
        ESP_LOGE(TAG, "Invalid ota controller");
        return ESP_FAIL;
    }

    if(!pCtrl->handle_valid)
    {
        ESP_LOGE(TAG, "Ota handle not valid");
        return ESP_FAIL;
    }

    esp_err_t err = esp_ota_write(pCtrl->ota_handle, data, size);
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to write ota data: %d", err);
        return err;
    }

    pCtrl->bytes_written += size;
    return ESP_OK;
}

// stop callback function
static esp_err_t __stop_cb(void *param, bool success)
{
    T_OTA_CTRL *pCtrl = (T_OTA_CTRL *)param;
    esp_err_t err = ESP_FAIL;
    if(pCtrl == NULL)
    {
        ESP_LOGE(TAG, "Invalid ota controller");
        return ESP_FAIL;
    }

    if(success && pCtrl->handle_valid)
    {
        pCtrl->state = OTA_S_VERIFYING;
        if((err = esp_ota_end(pCtrl->ota_handle)) != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to end ota:%s", esp_err_to_name(err));
            goto fail;
        }
        pCtrl->state = OTA_S_APPLYING;
        if((err = esp_ota_set_boot_partition(pCtrl->partition)) != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to set boot partition:%s", esp_err_to_name(err));
            goto fail;
        }
        esp_restart();
    }
    else if(!success && pCtrl->handle_valid)
    {
        ESP_LOGE(TAG, "Ota failed");
        esp_ota_abort(pCtrl->ota_handle);
    }
    else
    {
        ESP_LOGE(TAG, "Ota failed");
    }

fail:
    // finish ota
    pCtrl->state = OTA_S_DONE;
    free(pCtrl);
    pCtrl = NULL;
    p_ctrl = NULL;

    return ESP_FAIL;
}

esp_err_t ota_mark_valid(void)
{
    esp_err_t err = esp_ota_mark_app_valid_cancel_rollback();
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to mark ota as valid: %d", err);
        return err;
    }

    return ESP_OK;
}

void ota_print_info(void)
{
    ESP_LOGI(TAG, "Running partition: %s", esp_ota_get_running_partition()->label);
    ESP_LOGI(TAG, "Boot partition: %s", esp_ota_get_boot_partition()->label);
    ESP_LOGI(TAG, "Next partition: %s", esp_ota_get_next_update_partition(NULL)->label);
}

esp_err_t ota_start(E_OTA_TYPE type)
{
    esp_err_t err = ESP_FAIL;
    if(p_ctrl != NULL)
    {
        ESP_LOGE(TAG, "Ota controller already initialized");
        return ESP_FAIL;
    }

    p_ctrl = (T_OTA_CTRL *)calloc(1, sizeof(T_OTA_CTRL));
    if(p_ctrl == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate memory for ota controller");
        return ESP_FAIL;
    }
    switch(type)
    {
        case OTA_TYPE_HTTPS:
            p_ctrl->mode = ota_https_init();
            break;
        case OTA_TYPE_XMODEM:
            p_ctrl->mode = ota_xmodem_init();
            break;
        default:
            ESP_LOGE(TAG, "Invalid ota type:%d", type);
            err = ESP_FAIL;
            goto fail;
            break;
    }

    if(p_ctrl->mode == NULL)
    {
        ESP_LOGE(TAG, "Failed to initialize ota mode");
        goto fail;
    }

    //init p_ctrl
    p_ctrl->state = OTA_S_INIT;
    p_ctrl->partition = NULL;
    p_ctrl->ota_handle = 0;
    p_ctrl->handle_valid = false;
    p_ctrl->bytes_written = 0;

    err = p_ctrl->mode->start(p_ctrl->mode->ctx, __start_cb, __write_cb, __stop_cb, p_ctrl);
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to start ota");
        goto fail;
    }
    
    return ESP_OK;

fail:
    if(p_ctrl->mode != NULL)
    {
        p_ctrl->mode->deinit(p_ctrl->mode->ctx);
        p_ctrl->mode = NULL;
    }

    if(p_ctrl != NULL)
    {
        free(p_ctrl);
        p_ctrl = NULL;
    }

    return err;
}

esp_err_t ota_stop(void)
{
    if(p_ctrl == NULL)
    {
        ESP_LOGE(TAG, "Ota controller not initialized");
        return ESP_FAIL;
    }
    
    T_OTA_CTRL *pCtrl = p_ctrl;
    esp_err_t err = pCtrl->mode->stop(pCtrl->mode->ctx);
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to stop ota: %d", err);
        return err;
    }
    
    return ESP_OK;
}

esp_err_t ota_init(void)
{// initialize the ota system

    return ESP_OK;
}
