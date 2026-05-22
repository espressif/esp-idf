/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Private bridge API between the WiFi driver / NAN discovery engine
 * (components/esp_wifi/wifi_apps/nan_app) and the imported NAN
 * NDP/NDL/bootstrap/security engine living in
 * components/wpa_supplicant/src/nan/.
 *
 * Symbols declared here are implemented in
 * components/wpa_supplicant/esp_supplicant/src/esp_nan_supplicant.c.
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"
#include "esp_wifi_types_generic.h"

#ifdef __cplusplus
extern "C" {
#endif

struct nan_data;

#if CONFIG_ESP_WIFI_PASN_SUPPORT
#ifndef ETH_ALEN
#define ETH_ALEN 6
#endif
#ifndef NAN_PASN_GLOBAL_PTK_BLOB_MAX
#define NAN_PASN_GLOBAL_PTK_BLOB_MAX 128
#endif
#ifndef NAN_PASN_KEY_PMK_MAX
#define NAN_PASN_KEY_PMK_MAX 64
#endif
#ifndef NAN_PASN_KEK_MAX_LEN
#define NAN_PASN_KEK_MAX_LEN 32
#endif
#ifndef NAN_PASN_NIK_LEN
#define NAN_PASN_NIK_LEN 16
#endif

#ifndef ESP_NAN_ROLE_ENUM_DEFINED
#define ESP_NAN_ROLE_ENUM_DEFINED
enum nan_role {
    NAN_ROLE_IDLE = 0,
    NAN_ROLE_PAIRING_INITIATOR = 1,
    NAN_ROLE_PAIRING_RESPONDER = 2,
};
#endif

/**
 * Pairing-complete callback. Fires after the PASN pairwise TK is installed and
 * -- if the PASN PTK carried a KDK -- the ND-PMK has been derived
 * (Wi-Fi Aware v4.0 section 7.6.4.2: KDF-HASH-256(NM-KDK, "NDP PMK Derivation",
 * Pairing Initiator NMI || Pairing Responder NMI)).
 *
 * @param peer_nmi    Peer NMI (6 bytes).
 * @param role        enum nan_role value for the local device.
 * @param ndp_csid    NCS-SK CSID for paired-peer NDP (WIFI_NAN_CSID_NCS_SK_128
 *                    or _SK_256), 0 if no usable cipher mapping was available.
 * @param nd_pmk      ND-PMK bytes (32) or NULL if KDK was absent.
 * @param nd_pmk_len  Length of @a nd_pmk (32 when present, 0 otherwise).
 */
typedef void (*esp_nan_pairing_key_installed_cb_t)(const uint8_t *peer_nmi,
                                                   uint8_t role,
                                                   uint8_t ndp_csid,
                                                   const uint8_t *nd_pmk,
                                                   size_t nd_pmk_len);

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
    size_t kek_len;
    uint8_t kek[NAN_PASN_KEK_MAX_LEN];
};

/**
 * @brief  Schedule NAN PASN responder setup after pairing bootstrapping indication.
 *
 * Invoked from @c nan_app_pairing_indication_cb when the local device is the
 * pairing responder. Runs on the wpa_supplicant eloop thread.
 *
 * @param  peer_nmi  Peer NMI (6 bytes).
 * @param  pincode   6-digit PIN (0..999999), or @c UINT32_MAX for the default PIN.
 * @param  pairing_key_installed_cb Callback invoked after pairwise key installation with peer NMI.
 * @return 0 on success, -1 on failure.
 */
int esp_nan_supp_pasn_responder_init(const uint8_t *peer_nmi, uint32_t pincode,
                                     esp_nan_pairing_key_installed_cb_t pairing_key_installed_cb);

/**
 * @brief  Schedule NAN PASN initiator authentication after pairing bootstrapping confirm.
 *
 * Invoked from @c nan_app_pairing_confirm_cb when the local device is the
 * pairing initiator and bootstrapping completed successfully (@a status == 0).
 * Runs on the wpa_supplicant eloop thread.
 *
 * @param  peer_nmi  Peer NMI (6 bytes).
 * @param  pincode   6-digit PIN (0..999999), or @c UINT32_MAX for the default PIN.
 * @param  pairing_key_installed_cb Callback invoked after pairwise key installation with peer NMI.
 * @return 0 on success, -1 on failure.
 */
int esp_nan_supp_pasn_initiator_auth(const uint8_t *peer_nmi, uint32_t pincode,
                                     esp_nan_pairing_key_installed_cb_t pairing_key_installed_cb);

/**
 * Schedule @ref handle_auth_pasn from NAN app callback table.
 */
void handle_auth_pasn(uint8_t *buf, size_t len, uint16_t trans_seq, uint16_t status);

const struct nan_pasn_key_material *nan_pasn_get_saved_keys(void);
void nan_pasn_clear_saved_keys(void);

/**
 * Decrypt a NAN Shared Key Descriptor attribute received in a follow-up frame.
 *
 * @param[in]  shared_key_attr  Raw attribute buffer beginning with the
 *                              NAN Shared Key Descriptor ID octet.
 * @param[in]  attr_total_len   Total length including the 3-byte attribute header.
 * @param[out] nik              Buffer receiving the decrypted NIK.
 * @param[in]  nik_size         Size of @a nik. Must be at least @c NAN_PASN_NIK_LEN.
 * @param[out] cipher_ver       Optional. Receives the NIK cipher version.
 * @param[out] lifetime_sec     Optional. Receives the NIK lifetime in seconds.
 * @return 0 on success, -1 on failure.
 */
int nan_pasn_followup_decrypt_keys(const uint8_t *shared_key_attr,
                                   size_t attr_total_len,
                                   uint8_t *nik, size_t nik_size,
                                   uint8_t *cipher_ver,
                                   uint32_t *lifetime_sec);

#endif /* CONFIG_ESP_WIFI_PASN_SUPPORT */

#ifdef __cplusplus
}
#endif
