/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <sys/cdefs.h>
#include "esp_types.h"
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_cam_ctlr.h"
#include "esp_cam_ctlr_interface.h"

static const char *TAG = "CAM_CTLR";

esp_err_t esp_cam_ctlr_enable(esp_cam_ctlr_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(handle->enable, ESP_ERR_NOT_SUPPORTED, TAG, "controller driver function not supported");

    return handle->enable(handle);
}

esp_err_t esp_cam_ctlr_start(esp_cam_ctlr_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(handle->start, ESP_ERR_NOT_SUPPORTED, TAG, "controller driver function not supported");

    return handle->start(handle);
}

esp_err_t esp_cam_ctlr_stop(esp_cam_ctlr_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(handle->stop, ESP_ERR_NOT_SUPPORTED, TAG, "controller driver function not supported");

    return handle->stop(handle);
}

esp_err_t esp_cam_ctlr_disable(esp_cam_ctlr_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(handle->disable, ESP_ERR_NOT_SUPPORTED, TAG, "controller driver function not supported");

    return handle->disable(handle);
}

esp_err_t esp_cam_ctlr_receive(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, uint32_t timeout_ms)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(handle->receive, ESP_ERR_NOT_SUPPORTED, TAG, "controller driver function not supported");

    return handle->receive(handle, trans, timeout_ms);
}

esp_err_t esp_cam_ctlr_register_event_callbacks(esp_cam_ctlr_handle_t handle, const esp_cam_ctlr_evt_cbs_t *cbs, void *user_data)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(handle->register_event_callbacks, ESP_ERR_NOT_SUPPORTED, TAG, "controller driver function not supported");

    return handle->register_event_callbacks(handle, cbs, user_data);
}

esp_err_t esp_cam_del_ctlr(esp_cam_ctlr_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(handle->del, ESP_ERR_NOT_SUPPORTED, TAG, "controller driver function not supported");

    return handle->del(handle);
}
