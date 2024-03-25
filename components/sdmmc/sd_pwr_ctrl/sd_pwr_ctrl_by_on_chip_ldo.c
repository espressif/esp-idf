/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "esp_types.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "esp_private/esp_ldo.h"
#include "soc/soc_caps.h"
#include "sd_pwr_ctrl.h"
#include "sd_pwr_ctrl_by_on_chip_ldo.h"
#include "sd_pwr_ctrl_interface.h"

typedef struct {
    esp_ldo_unit_handle_t ldo_unit;
    int voltage_mv;
} sd_pwr_ctrl_ldo_ctx_t;

const static char *TAG = "sd_ldo";
static esp_err_t s_ldo_set_voltage(void *arg, int voltage_mv);

esp_err_t sd_pwr_ctrl_new_on_chip_ldo(const sd_pwr_ctrl_ldo_config_t *configs, sd_pwr_ctrl_handle_t *ret_drv)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(configs && ret_drv, ESP_ERR_INVALID_ARG, TAG, "invalid arg: null pointer");

    sd_pwr_ctrl_drv_t *driver = (sd_pwr_ctrl_drv_t *)heap_caps_calloc(1, sizeof(sd_pwr_ctrl_drv_t), MALLOC_CAP_DEFAULT | MALLOC_CAP_8BIT);
    ESP_RETURN_ON_FALSE(driver, ESP_ERR_NO_MEM, TAG, "no mem for on-chip ldo control driver");

    sd_pwr_ctrl_ldo_ctx_t *ctx = (sd_pwr_ctrl_ldo_ctx_t *)heap_caps_calloc(1, sizeof(sd_pwr_ctrl_ldo_ctx_t), MALLOC_CAP_DEFAULT | MALLOC_CAP_8BIT);
    ESP_GOTO_ON_FALSE(ctx, ESP_ERR_NO_MEM, err, TAG, "no mem for on-chip ldo control driver context");

    esp_ldo_unit_init_cfg_t unit_cfg = {
        .unit_id = configs->ldo_unit_id,
        .cfg = {
            .voltage_mv = 0,    //will be adjusted dynamically by sdmmc driver later
        },
    };
    esp_ldo_unit_handle_t ldo_unit = NULL;
    ESP_GOTO_ON_ERROR(esp_ldo_init_unit(&unit_cfg, &ldo_unit), err, TAG, "failed to create an on-chip LDO unit handle");
    ESP_GOTO_ON_ERROR(esp_ldo_enable_unit(ldo_unit), err, TAG, "failed to enable the on-chip LDO unit");

    driver->set_io_voltage = s_ldo_set_voltage;
    driver->ctx = ctx;
    ctx->ldo_unit = ldo_unit;
    ctx->voltage_mv = 0;
    *ret_drv = driver;

    return ESP_OK;

err:
    free(ctx);
    free(driver);
    return ret;
}

esp_err_t sd_pwr_ctrl_del_on_chip_ldo(sd_pwr_ctrl_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid arg: null pointer");

    sd_pwr_ctrl_ldo_ctx_t *ctx = handle->ctx;
    ESP_RETURN_ON_ERROR(esp_ldo_disable_unit(ctx->ldo_unit), TAG, "failed to disable the on-chip LDO unit");
    ESP_RETURN_ON_ERROR(esp_ldo_deinit_unit(ctx->ldo_unit), TAG, "failed to deinit the LDO unit handle");
    free(handle->ctx);
    handle->ctx = NULL;
    free(handle);
    handle = NULL;

    return ESP_OK;
}

static esp_err_t s_ldo_set_voltage(void *arg, int voltage_mv)
{
    //API checks done by caller
    sd_pwr_ctrl_ldo_ctx_t *ctx = arg;
    ESP_RETURN_ON_ERROR(esp_ldo_set_voltage(ctx->ldo_unit, voltage_mv), TAG, "failed to set LDO unit output voltage");
    ctx->voltage_mv = voltage_mv;
    return ESP_OK;
}
