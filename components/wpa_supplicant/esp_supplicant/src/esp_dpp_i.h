/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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
    SIG_DPP_LISTEN_NEXT_CHANNEL,
    SIG_DPP_DEL_TASK,
    SIG_DPP_MAX,
};

typedef struct {
    uint32_t id;
    uint32_t data;
} dpp_event_t;

#define BOOTSTRAP_ROC_WAIT_TIME 500
#define OFFCHAN_TX_WAIT_TIME 500

struct dpp_bootstrap_params_t {
    enum dpp_bootstrap_type type;
    uint8_t chan_list[14];
    uint8_t num_chan;
    uint8_t mac[6];
    uint32_t info_len;
    char *info;
};

struct esp_dpp_context_t {
    struct dpp_bootstrap_params_t bootstrap_params;
    struct dpp_authentication *dpp_auth;
    int gas_dialog_token;
    esp_supp_dpp_event_cb_t dpp_event_cb;
    struct dpp_global *dpp_global;
    wifi_config_t wifi_cfg;
    int id;
};

int esp_supp_rx_action(uint8_t *hdr, uint8_t *payload, size_t len, uint8_t channel);

#endif /* ESP_DPP_I_H */
