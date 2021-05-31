// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

#include <stdio.h>
#include <string.h>
#include <esp_err.h>
#include <esp_log.h>

#include <protocomm.h>
#include <protocomm_security0.h>
#include <protocomm_security1.h>

#include <esp_local_ctrl.h>
#include "esp_local_ctrl_priv.h"
#include "esp_local_ctrl.pb-c.h"

#define ESP_LOCAL_CTRL_VERSION "v1.0"

struct inst_ctx {
    protocomm_t *pc;
    esp_local_ctrl_config_t config;
    esp_local_ctrl_prop_t **props;
    size_t props_count;
};

struct inst_ctx *local_ctrl_inst_ctx;

static const char *TAG = "esp_local_ctrl";

esp_err_t esp_local_ctrl_start(const esp_local_ctrl_config_t *config)
{
    esp_err_t ret;

    if (!config) {
        ESP_LOGE(TAG, "NULL configuration provided");
        return ESP_ERR_INVALID_ARG;
    }

    if (!config->transport) {
        ESP_LOGE(TAG, "No transport provided");
        return ESP_ERR_INVALID_ARG;
    }

    if (config->max_properties == 0) {
        ESP_LOGE(TAG, "max_properties must be greater than 0");
        return ESP_ERR_INVALID_ARG;
    }

    if (!config->handlers.get_prop_values ||
        !config->handlers.set_prop_values) {
        ESP_LOGE(TAG, "Handlers cannot be null");
        return ESP_ERR_INVALID_ARG;
    }

    if (local_ctrl_inst_ctx) {
        ESP_LOGW(TAG, "Service already active");
        return ESP_ERR_INVALID_STATE;
    }

    local_ctrl_inst_ctx = calloc(1, sizeof(struct inst_ctx));
    if (!local_ctrl_inst_ctx) {
        ESP_LOGE(TAG, "Failed to allocate memory for instance");
        return ESP_ERR_NO_MEM;
    }
    memcpy(&local_ctrl_inst_ctx->config, config, sizeof(local_ctrl_inst_ctx->config));

    local_ctrl_inst_ctx->props = calloc(local_ctrl_inst_ctx->config.max_properties,
                                        sizeof(esp_local_ctrl_prop_t *));
    if (!local_ctrl_inst_ctx->props) {
        ESP_LOGE(TAG, "Failed to allocate memory for properties");
        free(local_ctrl_inst_ctx);
        local_ctrl_inst_ctx = NULL;
        return ESP_ERR_NO_MEM;
    }

    /* Since the config structure will be different for different transport modes, each transport may
     * implement a `copy_config()` function, which accepts a configuration structure as input and
     * creates a copy of that, which can be kept in the context structure of the `esp_local_ctrl` instance.
     * This copy can be later be freed using `free_config()` */
    if (config->transport->copy_config) {
        ret = config->transport->copy_config(&local_ctrl_inst_ctx->config.transport_config,
                                             &config->transport_config);
        if (ret != ESP_OK) {
            esp_local_ctrl_stop();
            return ret;
        }
    }

    /* For a selected transport mode, endpoints may need to be declared prior to starting the
     * `esp_local_ctrl` service, e.g. in case of BLE. By declaration it means that the transport layer
     * allocates some resources for an endpoint, and later, after service has started, a handler
     * is assigned for that endpoint */
    if (config->transport->declare_ep) {
        /* UUIDs are 16bit unique IDs for each endpoint. This may or may not be relevant for
         * a chosen transport. We reserve all values from FF50 to FFFF for the internal endpoints.
         * The remaining endpoints can be used by the application for its own custom endpoints */
        uint16_t start_uuid = 0xFF50;
        ret = config->transport->declare_ep(&local_ctrl_inst_ctx->config.transport_config,
                                            "esp_local_ctrl/version", start_uuid++);
        if (ret != ESP_OK) {
            esp_local_ctrl_stop();
            return ret;
        }
        ret = config->transport->declare_ep(&local_ctrl_inst_ctx->config.transport_config,
                                            "esp_local_ctrl/session", start_uuid++);
        if (ret != ESP_OK) {
            esp_local_ctrl_stop();
            return ret;
        }
        ret = config->transport->declare_ep(&local_ctrl_inst_ctx->config.transport_config,
                                            "esp_local_ctrl/control", start_uuid++);
        if (ret != ESP_OK) {
            esp_local_ctrl_stop();
            return ret;
        }
    }

    local_ctrl_inst_ctx->pc = protocomm_new();
    if (!local_ctrl_inst_ctx->pc) {
        ESP_LOGE(TAG, "Failed to create new protocomm instance");
        esp_local_ctrl_stop();
        return ESP_FAIL;
    }

    if (config->transport->start_service) {
        ret = config->transport->start_service(local_ctrl_inst_ctx->pc,
                                               &local_ctrl_inst_ctx->config.transport_config);
        if (ret != ESP_OK) {
            esp_local_ctrl_stop();
            return ret;
        }
    }

    ret = protocomm_set_version(local_ctrl_inst_ctx->pc, "esp_local_ctrl/version",
                                ESP_LOCAL_CTRL_VERSION);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set version endpoint");
        esp_local_ctrl_stop();
        return ret;
    }

    protocomm_security_t *proto_sec_handle;
    switch (local_ctrl_inst_ctx->config.proto_sec.version) {
        case PROTOCOM_SEC_CUSTOM:
            proto_sec_handle = local_ctrl_inst_ctx->config.proto_sec.custom_handle;
            break;
        case PROTOCOM_SEC1:
            proto_sec_handle = (protocomm_security_t *) &protocomm_security1;
            break;
        case PROTOCOM_SEC0:
        default:
            proto_sec_handle = (protocomm_security_t *) &protocomm_security0;
            break;
    }
    ret = protocomm_set_security(local_ctrl_inst_ctx->pc, "esp_local_ctrl/session",
                                 proto_sec_handle, local_ctrl_inst_ctx->config.proto_sec.pop);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set session endpoint");
        esp_local_ctrl_stop();
        return ret;
    }

    ret = protocomm_add_endpoint(local_ctrl_inst_ctx->pc, "esp_local_ctrl/control",
                                 esp_local_ctrl_data_handler, NULL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set control endpoint");
        esp_local_ctrl_stop();
        return ret;
    }
    return ESP_OK;
}

esp_err_t esp_local_ctrl_stop(void)
{
    if (local_ctrl_inst_ctx) {
        if (local_ctrl_inst_ctx->config.transport->free_config) {
            local_ctrl_inst_ctx->config.transport->free_config(&local_ctrl_inst_ctx->config.transport_config);
        }
        if (local_ctrl_inst_ctx->pc) {
            if (local_ctrl_inst_ctx->config.transport->stop_service) {
                local_ctrl_inst_ctx->config.transport->stop_service(local_ctrl_inst_ctx->pc);
            }
            protocomm_delete(local_ctrl_inst_ctx->pc);
        }
        if (local_ctrl_inst_ctx->config.handlers.usr_ctx_free_fn) {
            local_ctrl_inst_ctx->config.handlers.usr_ctx_free_fn(
                local_ctrl_inst_ctx->config.handlers.usr_ctx);
        }

        /* Iterate through all properties one by one and free them */
        for (uint32_t i = 0; i < local_ctrl_inst_ctx->config.max_properties; i++) {
            if (local_ctrl_inst_ctx->props[i] == NULL) {
                continue;
            }
            /* Release memory allocated for property data */
            free(local_ctrl_inst_ctx->props[i]->name);
            if (local_ctrl_inst_ctx->props[i]->ctx_free_fn) {
                local_ctrl_inst_ctx->props[i]->ctx_free_fn(local_ctrl_inst_ctx->props[i]->ctx);
            }
            free(local_ctrl_inst_ctx->props[i]);
        }
        free(local_ctrl_inst_ctx->props);
        free(local_ctrl_inst_ctx);
        local_ctrl_inst_ctx = NULL;
    }
    return ESP_OK;
}

static int esp_local_ctrl_get_property_index(const char *name)
{
    if (!local_ctrl_inst_ctx || !name) {
        return -1;
    }

    /* Iterate through all properties one by one
     * and find the one with matching name */
    for (uint32_t i = 0; i < local_ctrl_inst_ctx->props_count; i++) {
        if (strcmp(local_ctrl_inst_ctx->props[i]->name, name) == 0) {
            return i;
        }
    }
    return -1;
}

esp_err_t esp_local_ctrl_add_property(const esp_local_ctrl_prop_t *prop)
{
    if (!local_ctrl_inst_ctx) {
        ESP_LOGE(TAG, "Service not running");
        return ESP_ERR_INVALID_STATE;
    }
    if (!prop || !prop->name) {
        return ESP_ERR_INVALID_ARG;
    }
    if (esp_local_ctrl_get_property_index(prop->name) >= 0) {
        ESP_LOGE(TAG, "Property with name %s exists", prop->name);
        return ESP_ERR_INVALID_STATE;
    }

    if (local_ctrl_inst_ctx->config.max_properties
        == local_ctrl_inst_ctx->props_count) {
        ESP_LOGE(TAG, "Max properties limit reached. Cannot add property %s", prop->name);
        return ESP_ERR_NO_MEM;
    }

    uint32_t i = local_ctrl_inst_ctx->props_count;
    local_ctrl_inst_ctx->props[i] = calloc(1, sizeof(esp_local_ctrl_prop_t));
    if (!local_ctrl_inst_ctx->props[i]) {
        ESP_LOGE(TAG, "Failed to allocate memory for new property %s", prop->name);
        return ESP_ERR_NO_MEM;
    }
    local_ctrl_inst_ctx->props[i]->name = strdup(prop->name);
    if (!local_ctrl_inst_ctx->props[i]->name) {
        ESP_LOGE(TAG, "Failed to allocate memory for property data %s", prop->name);
        free(local_ctrl_inst_ctx->props[i]);
        local_ctrl_inst_ctx->props[i] = NULL;
        return ESP_ERR_NO_MEM;
    }
    local_ctrl_inst_ctx->props[i]->type  = prop->type;
    local_ctrl_inst_ctx->props[i]->size  = prop->size;
    local_ctrl_inst_ctx->props[i]->flags = prop->flags;
    local_ctrl_inst_ctx->props[i]->ctx   = prop->ctx;
    local_ctrl_inst_ctx->props[i]->ctx_free_fn = prop->ctx_free_fn;
    local_ctrl_inst_ctx->props_count++;
    return ESP_OK;
}


esp_err_t esp_local_ctrl_remove_property(const char *name)
{
    int idx = esp_local_ctrl_get_property_index(name);
    if (idx < 0) {
        ESP_LOGE(TAG, "Property %s not found", name);
        return ESP_ERR_NOT_FOUND;
    }

    /* Release memory allocated for property data */
    if (local_ctrl_inst_ctx->props[idx]->ctx_free_fn) {
        local_ctrl_inst_ctx->props[idx]->ctx_free_fn(
            local_ctrl_inst_ctx->props[idx]->ctx);
    }
    free(local_ctrl_inst_ctx->props[idx]->name);
    free(local_ctrl_inst_ctx->props[idx]);
    local_ctrl_inst_ctx->props[idx++] = NULL;

    /* Move the following properties forward, so that there is
     * no empty space between two properties */
    for (uint32_t i = idx; i < local_ctrl_inst_ctx->props_count; i++) {
        if (local_ctrl_inst_ctx->props[i] == NULL) {
            break;
        }
        local_ctrl_inst_ctx->props[i-1] = local_ctrl_inst_ctx->props[i];
    }
    local_ctrl_inst_ctx->props_count--;
    return ESP_OK;
}

const esp_local_ctrl_prop_t *esp_local_ctrl_get_property(const char *name)
{
    int idx = esp_local_ctrl_get_property_index(name);
    if (idx < 0) {
        ESP_LOGE(TAG, "Property %s not found", name);
        return NULL;
    }

    return local_ctrl_inst_ctx->props[idx];
}

esp_err_t esp_local_ctrl_get_prop_count(size_t *count)
{
    if (!local_ctrl_inst_ctx) {
        ESP_LOGE(TAG, "Service not running");
        return ESP_ERR_INVALID_STATE;
    }
    if (!count) {
        return ESP_ERR_INVALID_ARG;
    }
    *count = local_ctrl_inst_ctx->props_count;
    return ESP_OK;
}

esp_err_t esp_local_ctrl_get_prop_values(size_t total_indices, uint32_t *indices,
                                         esp_local_ctrl_prop_t *props,
                                         esp_local_ctrl_prop_val_t *values)
{
    if (!local_ctrl_inst_ctx) {
        ESP_LOGE(TAG, "Service not running");
        return ESP_ERR_INVALID_STATE;
    }
    if (!indices || !props || !values) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Convert indices to names */
    for (size_t i = 0; i < total_indices; i++) {
        if (indices[i] >= local_ctrl_inst_ctx->props_count) {
            ESP_LOGE(TAG, "Invalid property index %d", indices[i]);
            return ESP_ERR_INVALID_ARG;
        }
        props[i].name  = local_ctrl_inst_ctx->props[indices[i]]->name;
        props[i].type  = local_ctrl_inst_ctx->props[indices[i]]->type;
        props[i].flags = local_ctrl_inst_ctx->props[indices[i]]->flags;
        props[i].size  = local_ctrl_inst_ctx->props[indices[i]]->size;
        props[i].ctx   = local_ctrl_inst_ctx->props[indices[i]]->ctx;
    }

    esp_local_ctrl_handlers_t *h = &local_ctrl_inst_ctx->config.handlers;
    esp_err_t ret = h->get_prop_values(total_indices, props, values, h->usr_ctx);

    /* Properties with fixed sizes need to be checked */
    for (size_t i = 0; i < total_indices; i++) {
        if (local_ctrl_inst_ctx->props[indices[i]]->size != 0) {
            values[i].size = local_ctrl_inst_ctx->props[indices[i]]->size;
        }
    }
    return ret;
}

esp_err_t esp_local_ctrl_set_prop_values(size_t total_indices, uint32_t *indices,
                                         const esp_local_ctrl_prop_val_t *values)
{
   if (!local_ctrl_inst_ctx) {
        ESP_LOGE(TAG, "Service not running");
        return ESP_ERR_INVALID_STATE;
    }
    if (!indices || !values) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_local_ctrl_prop_t *props = calloc(total_indices,
                                          sizeof(esp_local_ctrl_prop_t));
    if (!props) {
        ESP_LOGE(TAG, "Unable to allocate memory for properties array");
        return ESP_ERR_NO_MEM;
    }
    for (size_t i = 0; i < total_indices; i++) {
        if (indices[i] >= local_ctrl_inst_ctx->props_count) {
            ESP_LOGE(TAG, "Invalid property index %d", indices[i]);
            free(props);
            return ESP_ERR_INVALID_ARG;
        }

        /* Properties with fixed sizes need to be checked */
        if ((local_ctrl_inst_ctx->props[indices[i]]->size != values[i].size) &&
            (local_ctrl_inst_ctx->props[indices[i]]->size != 0)) {
            ESP_LOGE(TAG, "Invalid property size %d. Expected %d",
                     values[i].size, local_ctrl_inst_ctx->props[indices[i]]->size);
            free(props);
            return ESP_ERR_INVALID_ARG;
        }

        props[i].name  = local_ctrl_inst_ctx->props[indices[i]]->name;
        props[i].type  = local_ctrl_inst_ctx->props[indices[i]]->type;
        props[i].flags = local_ctrl_inst_ctx->props[indices[i]]->flags;
        props[i].size  = local_ctrl_inst_ctx->props[indices[i]]->size;
        props[i].ctx   = local_ctrl_inst_ctx->props[indices[i]]->ctx;
    }

    esp_local_ctrl_handlers_t *h = &local_ctrl_inst_ctx->config.handlers;
    esp_err_t ret = h->set_prop_values(total_indices, props, values, h->usr_ctx);

    free(props);
    return ret;
}

esp_err_t esp_local_ctrl_set_handler(const char *ep_name,
                                     protocomm_req_handler_t handler,
                                     void *priv_data)
{
    esp_err_t ret = ESP_ERR_INVALID_STATE;

    if (local_ctrl_inst_ctx) {
        ret = protocomm_add_endpoint(local_ctrl_inst_ctx->pc, ep_name,
                                     handler, priv_data);
    }

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register endpoint handler");
    }
    return ret;
}
