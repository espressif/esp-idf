/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_check.h"
#include "esp_async_memcpy.h"
#include "esp_async_memcpy_priv.h"

ESP_LOG_ATTR_TAG(TAG, "async_mcp");

esp_err_t esp_async_memcpy_uninstall(async_memcpy_handle_t asmcp)
{
    ESP_RETURN_ON_FALSE(asmcp, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    return asmcp->del(asmcp);
}

esp_err_t esp_async_memcpy(async_memcpy_handle_t asmcp, void *dst, void *src, size_t n, async_memcpy_isr_cb_t cb_isr, void *cb_args)
{
    ESP_RETURN_ON_FALSE(asmcp && dst && src && n, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    return asmcp->memcpy(asmcp, dst, src, n, cb_isr, cb_args);
}

#if SOC_ETM_SUPPORTED
esp_err_t esp_async_memcpy_new_etm_event(async_memcpy_handle_t asmcp, async_memcpy_etm_event_t event_type, esp_etm_event_handle_t *out_event)
{
    ESP_RETURN_ON_FALSE(asmcp && out_event, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(asmcp->new_etm_event, ESP_ERR_NOT_SUPPORTED, TAG, "ETM is not supported");
    return asmcp->new_etm_event(asmcp, event_type, out_event);
}
#endif
