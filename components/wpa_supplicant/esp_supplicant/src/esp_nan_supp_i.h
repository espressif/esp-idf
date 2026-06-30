/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"

#if CONFIG_ESP_WIFI_NAN_PAIRING

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "esp_private/esp_supp_nan.h"

struct wpabuf;
struct pasn_data;
struct rsn_pmksa_cache;

#define NAN_PASN_PMKID_LEN 16

typedef esp_nan_pairing_key_installed_cb_t nan_pasn_pairing_key_installed_cb_t;

struct nan_config {
    uint8_t dev_addr[ETH_ALEN];
    uint8_t pasn_type;
    void *cb_ctx;
    int (*set_pmksa)(void *ctx, const uint8_t *peer_addr, const uint8_t *pmkid);
    int (*pasn_send_mgmt)(void *ctx, const uint8_t *data, size_t data_len, int noack,
                          unsigned int freq, unsigned int wait_ms);
    int (*prepare_data_element)(void *ctx, const uint8_t *peer_addr);
    int (*parse_data_element)(void *ctx, const uint8_t *data, size_t len);
    int (*pasn_validate_pmkid)(void *ctx, const uint8_t *addr, const uint8_t *pmkid);
};

struct nan_pasn_data {
    enum nan_role dev_role;
    uint8_t pasn_unicast_peer[ETH_ALEN];
    char dev_sae_pin[64];
    size_t dev_sae_pin_len;
    struct nan_config *cfg;
    struct rsn_pmksa_cache *initiator_pmksa;
    struct rsn_pmksa_cache *responder_pmksa;
    uint8_t pasn_ptk[128];
    size_t pasn_ptk_len;
    struct pasn_data *pasn;
    nan_pasn_pairing_key_installed_cb_t pairing_key_installed_cb;
    uint8_t pairing_verification;
    uint32_t nik_lifetime_sec;
    bool pasn_auth2_done;     /**< Initiator: Auth2 already processed (dedup) */
    uint8_t peer_npkid[NAN_PASN_PMKID_LEN]; /**< Nonce||Tag from peer NIRA; for PMKID cross-check */
    bool peer_npkid_valid;
};

int nan_initiate_pasn_verify(struct nan_pasn_data *pd, const uint8_t *peer_addr,
                             int role, const uint8_t *bssid,
                             const uint8_t *ssid, size_t ssid_len);
int nan_initiate_pasn_auth(struct nan_pasn_data *pd, const uint8_t *addr);
struct nan_pasn_data *nan_pasn_data_init(void);
void nan_pasn_data_deinit(struct nan_pasn_data *pd);
int nan_pasn_auth_initiate(struct nan_pasn_data *pd, const uint8_t *peer_addr);
int nan_pasn_auth(struct nan_pasn_data **pd_out, const uint8_t *peer_addr);

#endif /* CONFIG_ESP_WIFI_NAN_PAIRING */
