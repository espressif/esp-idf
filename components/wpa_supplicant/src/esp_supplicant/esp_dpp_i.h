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

#ifndef ESP_DPP_I_H
#define ESP_DPP_I_H

#include "esp_err.h"
#include "utils/includes.h"
#include "utils/common.h"

#include "common/dpp.h"
#include "esp_dpp.h"
#include "esp_wifi_driver.h"

#define DPP_TASK_STACK_SIZE  (6144 + TASK_STACK_SIZE_ADD)

enum SIG_DPP {
    SIG_DPP_RESET = 0,
    SIG_DPP_BOOTSTRAP_GEN,
    SIG_DPP_RX_ACTION,
    SIG_DPP_DEL_TASK,
    SIG_DPP_MAX,
};

typedef struct {
    uint32_t id;
    uint32_t data;
} dpp_event_t;

#define BOOTSTRAP_ROC_WAIT_TIME 5000
#define OFFCHAN_TX_WAIT_TIME 500

#define BOOTSTRAP_ROC_COOKIE    0xABABABAB

struct dpp_bootstrap_params_t {
    enum dpp_bootstrap_type type;
    uint8_t channel;
    uint8_t mac[6];
    uint32_t key_len;
    char *key;
    uint32_t info_len;
    char *info;
};

struct esp_dpp_context_t {
    struct dpp_bootstrap_params_t bootstrap_params;
    struct dpp_authentication *dpp_auth;
    int gas_dialog_token;
    wifi_config_t wifi_config;
    wifi_dpp_event_cb_t dpp_event_cb;
    struct dpp_global *dpp_global;
    wifi_config_t wifi_cfg;
    int id;
};

int esp_dpp_rx_mgmt(uint8_t *hdr, uint8_t *payload, size_t len, uint8_t channel);

#endif /* ESP_DPP_I_H */
