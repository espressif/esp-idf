// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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
#include <esp_log.h>
#include <string.h>
#include <esp_err.h>

#include <custom_provisioning/custom_config.h>

#include "custom_config.pb-c.h"

static const char *TAG = "custom_config";

int custom_prov_config_data_handler(uint32_t session_id, const uint8_t *inbuf, ssize_t inlen, uint8_t **outbuf, ssize_t *outlen, void *priv_data)
{
    CustomConfigRequest *req;
    CustomConfigResponse resp;
    custom_prov_config_handler_t app_handler_custom_config = (custom_prov_config_handler_t) priv_data;

    req = custom_config_request__unpack(NULL, inlen, inbuf);
    if (!req) {
        ESP_LOGE(TAG, "Unable to unpack config data");
        return ESP_ERR_INVALID_ARG;
    }

    custom_config_response__init(&resp);
    resp.status = CUSTOM_CONFIG_STATUS__ConfigFail;

    if (app_handler_custom_config) {
        custom_config_t config;
        strlcpy(config.info, req->info, sizeof(config.info));
        config.version = req->version;

        esp_err_t err = app_handler_custom_config(&config);
        resp.status = (err == ESP_OK) ? CUSTOM_CONFIG_STATUS__ConfigSuccess :
                                        CUSTOM_CONFIG_STATUS__ConfigFail;
    }
    custom_config_request__free_unpacked(req, NULL);

    resp.dummy = 47;    // Set a non zero value of dummy

    *outlen = custom_config_response__get_packed_size(&resp);
    if (*outlen <= 0) {
        ESP_LOGE(TAG, "Invalid encoding for response");
        return ESP_FAIL;
    }

    *outbuf = (uint8_t *) malloc(*outlen);
    if (*outbuf == NULL) {
        ESP_LOGE(TAG, "System out of memory");
        return ESP_ERR_NO_MEM;
    }

    custom_config_response__pack(&resp, *outbuf);
    return ESP_OK;
}
