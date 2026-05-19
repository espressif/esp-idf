/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * NAN PASN — public API and types for NSD / Wi-Fi Aware examples (e.g. esp-nsd udp_client).
 * When CONFIG_ESP_WIFI_PASN_SUPPORT is off, nan_pasn.c still links stubs (no-op / -1)
 * for this API so apps can call e.g. pasn_responder_init_eloop without a link error.
 */

#ifndef _NAN_PASN_H_
#define _NAN_PASN_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Self-contained: public consumers only get esp_supplicant/include. */
#ifndef ETH_ALEN
#define ETH_ALEN 6
#endif
#ifndef PMKID_LEN
#define PMKID_LEN 16
#endif
#ifndef NAN_PASN_GLOBAL_PTK_BLOB_MAX
#define NAN_PASN_GLOBAL_PTK_BLOB_MAX 128
#endif
#ifndef NAN_PASN_KEY_PMK_MAX
#define NAN_PASN_KEY_PMK_MAX 64
#endif

struct wpabuf;
struct pasn_data;
struct rsn_pmksa_cache;

enum nan_role {
    NAN_ROLE_IDLE = 0,
    NAN_ROLE_PAIRING_INITIATOR = 1,
    NAN_ROLE_PAIRING_RESPONDER = 2,
};

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
    /** Last known unicast peer; used when @c addr is broadcast so PASN Auth1 DA is not ff:ff:ff:ff:ff:ff. */
    uint8_t pasn_unicast_peer[ETH_ALEN];
    /**
     * NUL-terminated decimal PIN ('0'–'9' only).
     * Same buffer is SAE password material (ASCII per digit) and @c pasn->password.
     */
    char dev_sae_pin[64];
    size_t dev_sae_pin_len;
    struct nan_config *cfg;
    struct rsn_pmksa_cache *initiator_pmksa;
    struct rsn_pmksa_cache *responder_pmksa;
    uint8_t pasn_ptk[128];
    size_t pasn_ptk_len;
    struct pasn_data *pasn;
};

#ifdef __cplusplus
extern "C" {
#endif

int nan_initiate_pasn_verify(struct nan_pasn_data *pd, const uint8_t *peer_addr,
                             int freq, int role, const uint8_t *bssid,
                             const uint8_t *ssid, size_t ssid_len);

int nan_initiate_pasn_auth(struct nan_pasn_data *pd, const uint8_t *addr, int freq);

struct nan_pasn_data *nan_pasn_data_init(void);
void nan_pasn_data_deinit(struct nan_pasn_data *pd);
int nan_pasn_auth_initiate(struct nan_pasn_data *pd, const uint8_t *peer_addr, int freq);
int nan_pasn_auth(struct nan_pasn_data **pd_out, const uint8_t *peer_addr, int freq);
/**
 * Defer PASN auth initiation on eloop (same delayed pattern as @ref pasn_responder_init_eloop).
 * Replaces the global NAN PASN object: deinits any existing @c esp_nan_app_get_pasn_data(),
 * @ref nan_pasn_data_init, optional PIN override, then @ref nan_pasn_auth_initiate.
 * Operating frequency is chosen internally (current NAN channel, or 2412 MHz fallback).
 * @param pincode 6-digit PIN value (0..999999), e.g. @c 0 for @c "000000". @c UINT32_MAX to keep the default PIN from @ref nan_pasn_data_init.
 */
int nan_pasn_auth_eloop(const uint8_t *peer_addr, uint32_t pincode);

/**
 * Schedule @ref nan_initiate_pasn_verify on wpa_supplicant eloop after @a secs / @a usecs.
 * Looks up @c struct nan_pasn_data via @ref esp_nan_app_get_pasn_data in the callback.
 * @a bssid may be NULL to use @a peer_addr as BSSID. @a ssid may be NULL if @a ssid_len is 0.
 */
int nan_pasn_verify_eloop(unsigned int secs, unsigned int usecs,
                          const uint8_t *peer_addr, int freq, int role,
                          const uint8_t *bssid,
                          const uint8_t *ssid, size_t ssid_len);

/**
 * NAN PASN responder setup (nan_pasn_data_init, esp_nan_app_set_pasn_data, PIN, nan_pasn_initialize).
 * @param pincode 6-digit PIN value (e.g. @c wa_pairing_cred_t.pincode, 0..999999). Use @c UINT32_MAX to keep the default PIN from @ref nan_pasn_data_init (no override).
 * Frequency is chosen internally (current NAN channel, or 2412 MHz fallback).
 */
int pasn_responder_init(const uint8_t *peer_addr, uint32_t pincode);
/**
 * Schedule @ref pasn_responder_init on wpa_supplicant eloop (delay 0).
 * @a peer_addr may be NULL (broadcast placeholder).
 * @a pincode same as @ref pasn_responder_init (value is stored in eloop context).
 */
int pasn_responder_init_eloop(const uint8_t *peer_addr, uint32_t pincode);

void handle_auth_pasn(uint8_t *buf, size_t len, uint16_t trans_seq, uint16_t status);

/**
 * Last PASN key material after successful pairing (PMK + flattened PTK KCK|KEK|TK|KDK).
 * Written before @c pasn PTK is cleared; initiator session is torn down on Auth3 TX status.
 */
struct nan_pasn_key_material {
    uint8_t valid;
    uint8_t peer_addr[ETH_ALEN];
    enum nan_role role;
    int akmp;
    int cipher;
    size_t pmk_len;
    uint8_t pmk[NAN_PASN_KEY_PMK_MAX];
    size_t ptk_blob_len;
    uint8_t ptk_blob[NAN_PASN_GLOBAL_PTK_BLOB_MAX];
};

const struct nan_pasn_key_material *nan_pasn_get_saved_keys(void);
void nan_pasn_clear_saved_keys(void);

#ifdef __cplusplus
}
#endif

#endif /* _NAN_PASN_H_ */
