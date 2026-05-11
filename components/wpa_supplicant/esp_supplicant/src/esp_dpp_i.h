/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
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

#define ESP_DPP_AUTH_TIMEOUT_SECS 2
#define ESP_GAS_TIMEOUT_SECS 2
#define ESP_DPP_PMK_CACHE_DEFAULT_TIMEOUT  (86400 * 7)   /*!< 7 days */

#define BOOTSTRAP_ROC_WAIT_TIME 500
#define OFFCHAN_TX_WAIT_TIME 600

struct dpp_bootstrap_params_t {
    enum dpp_bootstrap_type type;
    uint8_t chan_list[14];
    uint8_t num_chan;
    uint8_t mac[6];
};

enum dpp_tx_frame_type {
    DPP_TX_INVALID = 0,
    DPP_TX_AUTHENTICATION_RESP,
    DPP_TX_AUTHENTICATION_CONF,
    DPP_TX_PEER_DISCOVERY_REQ,
    DPP_TX_GAS_CONFIG_REQ,
};

struct esp_dpp_context_t {
    struct dpp_bootstrap_params_t bootstrap_params;
    struct dpp_authentication *dpp_auth;
    struct dpp_global *dpp_global;
    wifi_config_t wifi_cfg;
    struct {
        uint8_t op_id;
        enum dpp_tx_frame_type type;
    } pending_tx_op;
    bool pending_tx_op_in_progress;
    unsigned int gas_query_tries;
    unsigned int listen_chan_idx;
    int id;
    bool dpp_deinit_pending;
    bool bootstrap_done;
    bool dpp_listen_ongoing;
};

#ifdef CONFIG_TESTING_OPTIONS
int dpp_test_gen_invalid_key(struct wpabuf *msg, const struct dpp_curve_params *curve);
char * dpp_corrupt_connector_signature(const char *connector);
#endif /* CONFIG_TESTING_OPTIONS */

#ifdef CONFIG_ESP_WIFI_DPP_SUPPORT
bool is_dpp_enabled(void);
#else
static inline bool is_dpp_enabled(void)
{
    return false;
}
#endif
#endif /* ESP_DPP_I_H */
