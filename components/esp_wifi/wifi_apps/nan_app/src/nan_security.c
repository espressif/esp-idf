/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * NAN encrypted-datapath (NDP) security: PMK/PMKID derivation, 4-way
 * handshake key descriptor build/parse, MIC computation, and CSIA/SCIA
 * attribute construction. Whole file is gated on CONFIG_ESP_WIFI_NAN_SECURITY.
 */

#include "sdkconfig.h"

#ifdef CONFIG_ESP_WIFI_NAN_SECURITY

#include <ctype.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_private/wifi.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_mac.h"
#include "os.h"
#include "esp_nan.h"
#include "utils/common.h"
#include "crypto/sha256.h"
#include "nan_i.h"

static const char *TAG = "nan_sec";

#define NAN_PMK_NAME_LABEL      "NAN PMK Name"
#define NAN_PMK_NAME_LABEL_LEN  12

/* Attribute IDs used in this file (mirrors values used by the Wi-Fi blob) */
#define NAN_ATTR_ID_CIPHER_SUITE_INFO   0x22
#define NAN_ATTR_ID_SECURITY_CONTEXT    0x23
#define NAN_ATTR_ID_SHARED_KEY_DESC     0x24

/*-------------------------------------------------------------------------
 * Pending state captured by the NDP key-desc parser before the indication
 * callback fires. Consumed by nan_security_apply_pending().
 *-----------------------------------------------------------------------*/

/* Pending M1 (ANonce + replay counter) */
static struct {
    bool valid;
    uint8_t pub_id;
    uint8_t anonce[NAN_NONCE_LEN];
    uint8_t rx_replay_counter[NAN_REPLAY_COUNTER_LEN];
    uint8_t key_rsc[NAN_KEY_RSC_LEN];
} s_pending_m1;

/* Pending PMKID and CSID from CSIA/SCIA. csid_bitmap accumulates every
 * spec-valid CSID advertised in the CSIA (a publisher may list more than
 * one cipher suite). */
static struct {
    bool has_pmkid;
    bool has_csid;
    uint8_t pub_id;
    uint8_t pmkid[ESP_WIFI_NAN_NDP_PMKID_LEN];
    uint16_t csid_bitmap;
} s_pending_scia;

/* Spec Table 121: valid CSIDs are 1..8. Bit 0 and bits 9..15 are not assigned. */
#define NAN_CSID_VALID_BITMAP   ((uint16_t)0x01FE)

/* Sentinel for peer_svc->matched_cred_idx: no PMKID match remembered yet. */
#define NAN_NO_MATCHED_CRED  0xFF

/*-------------------------------------------------------------------------
 * Static helpers
 *-----------------------------------------------------------------------*/

static const uint8_t *nan_find_attr(const uint8_t *attrs, size_t attrs_len,
                                    uint8_t attr_id, uint16_t *out_len)
{
    size_t offset = 0;
    while (offset + 3 <= attrs_len) {
        uint8_t id = attrs[offset];
        uint16_t len = attrs[offset + 1] | (attrs[offset + 2] << 8);

        if (offset + 3 + len > attrs_len) {
            break;
        }

        if (id == attr_id) {
            if (out_len) {
                *out_len = len;
            }
            return &attrs[offset + 3];
        }

        offset += 3 + len;
    }
    return NULL;
}

static bool nan_csid_bitmap_has_pasn(uint16_t csid_bitmap)
{
    return (csid_bitmap & WIFI_NAN_CSID_BIT_NCS_PK_PASN_128) != 0;
}

/*
 * Service ID = first 6 bytes of SHA256(lowercase(service_name))
 * per Wi-Fi Aware v4.0 §5.1.5 (Service Name and Service ID).
 */
static bool nan_compute_service_id(const char *service_name, uint8_t service_id[6])
{
    if (!service_name || !g_wifi_default_wpa_crypto_funcs.sha256_vector) {
        return false;
    }
    size_t name_len = strlen(service_name);
    char *lower = os_malloc(name_len + 1);
    if (!lower) {
        return false;
    }
    strlcpy(lower, service_name, name_len + 1);
    for (char *p = lower; *p; p++) {
        *p = tolower((unsigned char) * p);
    }
    uint8_t hash[32];
    const uint8_t *addr[1] = {(const uint8_t *)lower};
    size_t len[1] = {name_len};
    int ret = g_wifi_default_wpa_crypto_funcs.sha256_vector(1, addr, len, hash);
    os_free(lower);
    if (ret != 0) {
        return false;
    }
    memcpy(service_id, hash, 6);
    return true;
}

#define NAN_SERVICE_ID_LEN 6

/*
 * ND-PMK from passphrase: salt = 0 || CSID || Service_ID || peer NMI (publisher NMI on both sides).
 * PBKDF2 uses HMAC-SHA256 for all supported cipher suite IDs (no SHA-384 path).
 */
static int nan_derive_nd_pmk_from_passphrase(const char *pwd, uint8_t csid,
                                             const uint8_t *service_id,
                                             const uint8_t *peer_nmi,
                                             uint8_t *nd_pmk)
{
    uint8_t salt[14];

    if (!pwd || !service_id || !peer_nmi || !nd_pmk || csid == 0) {
        return -1;
    }

    salt[0] = 0;
    salt[1] = csid;
    os_memcpy(salt + 2, service_id, NAN_SERVICE_ID_LEN);
    os_memcpy(salt + 2 + NAN_SERVICE_ID_LEN, peer_nmi, ETH_ALEN);

    /* NCS-SK-128 is the only suite supported by the rest of the handshake
     * (PTK / MIC / KCK / KEK lengths in nan_i.h are 128-bit-specific).
     * NCS-PK-PASN-128 reuses the same passphrase-based PMK derivation per
     * Wi-Fi Aware v4.0 §7.1.3.6. Anything else fails negotiation here. */
    switch (csid) {
    case WIFI_NAN_CSID_NCS_SK_128:
    case WIFI_NAN_CSID_NCS_PK_PASN_128:
        return pbkdf2_sha256(pwd, salt, sizeof(salt), 4096, nd_pmk, ESP_WIFI_NAN_NDP_PMK_LEN);
    default:
        return -1;
    }
}

/*
 * Spec: ND-PMKID = L(HMAC-Hash(ND-PMK,
 *                              "NAN PMK Name" || NDP Initiator NMI || NDP Responder NMI || Service ID),
 *                    0, 128)
 * Uses NMI addresses (not NDI) for both initiator and responder.
 */
static bool nan_derive_ndp_request_pmkid(const uint8_t pmk[ESP_WIFI_NAN_NDP_PMK_LEN],
                                         const uint8_t initiator_nmi[6],
                                         const uint8_t responder_nmi[6],
                                         const uint8_t service_id[6],
                                         uint8_t pmkid_out[ESP_WIFI_NAN_NDP_PMKID_LEN])
{
    uint8_t hash[32];

    if (!service_id || !initiator_nmi || !responder_nmi ||
            !g_wifi_default_wpa_crypto_funcs.hmac_sha256_vector) {
        return false;
    }

    const unsigned char *addr_pmkid[4] = {(const unsigned char *)NAN_PMK_NAME_LABEL,
                                          initiator_nmi, responder_nmi, service_id
                                         };
    int len_pmkid[4] = {NAN_PMK_NAME_LABEL_LEN, 6, 6, 6};
    if (g_wifi_default_wpa_crypto_funcs.hmac_sha256_vector(pmk, ESP_WIFI_NAN_NDP_PMK_LEN,
                                                           4, addr_pmkid, len_pmkid, hash) != 0) {
        return false;
    }
    memcpy(pmkid_out, hash, ESP_WIFI_NAN_NDP_PMKID_LEN);
    return true;
}

/*
 * Match peer's ND-PMKID (from NDP Request/SCIA) against our per-credential
 * PMK cache. Returns the index in p_svc->derived_security[] that matched, or
 * -1 if no credential matches.
 *
 * Rule: Do NOT compare NDP Request PMKID to Publish-PMKID — they differ by design.
 * - Publish SCIA PMKID uses IAddr = FF:FF:FF:FF:FF:FF (initiator-independent).
 * - NDP Request SCIA PMKID uses Initiator NMI and Responder NMI (pair-specific).
 */
static int nan_match_pmkid(struct own_svc_info *p_svc,
                           const uint8_t peer_pmkid[ESP_WIFI_NAN_NDP_PMKID_LEN],
                           const uint8_t *peer_nmi,
                           const uint8_t *peer_ndi)
{
    (void)peer_ndi;

    if (!p_svc || !peer_pmkid || !peer_nmi || !p_svc->svc_name[0]) {
        goto no_match;
    }

    uint8_t our_nmi[6];
    if (esp_wifi_get_mac(WIFI_IF_NAN, our_nmi) != ESP_OK) {
        goto no_match;
    }

    uint8_t service_id[6];
    if (!nan_compute_service_id(p_svc->svc_name, service_id)) {
        goto no_match;
    }

    ESP_LOGD(TAG, "  Service Name: %s", p_svc->svc_name);
    ESP_LOG_BUFFER_HEXDUMP(TAG, service_id, 6, ESP_LOG_DEBUG);
    ESP_LOGD(TAG, "  Initiator NMI (peer): "MACSTR, MAC2STR(peer_nmi));
    ESP_LOGD(TAG, "  Responder NMI (us):   "MACSTR, MAC2STR(our_nmi));

    for (uint8_t i = 0; i < p_svc->user_cfg.num_credentials; i++) {
        uint8_t expected_pmkid[ESP_WIFI_NAN_NDP_PMKID_LEN];
        const uint8_t *own_pmk = p_svc->derived_security[i].nd_pmk;

        /* Spec order: Initiator NMI, Responder NMI */
        if (nan_derive_ndp_request_pmkid(own_pmk, peer_nmi, our_nmi,
                                         service_id, expected_pmkid) &&
                memcmp(expected_pmkid, peer_pmkid, ESP_WIFI_NAN_NDP_PMKID_LEN) == 0) {
            ESP_LOGD(TAG, "PMKID matched at credential slot %u", i);
            return (int)i;
        }

        /* Fallback: try reversed order in case peer implementation differs */
        if (nan_derive_ndp_request_pmkid(own_pmk, our_nmi, peer_nmi,
                                         service_id, expected_pmkid) &&
                memcmp(expected_pmkid, peer_pmkid, ESP_WIFI_NAN_NDP_PMKID_LEN) == 0) {
            ESP_LOGD(TAG, "PMKID matched at credential slot %u (reversed)", i);
            return (int)i;
        }
    }

no_match:
    ESP_LOGW(TAG, "PMKID not found in our cache (no match)");
    return -1;
}

/*
 * RSNA NCS-SK-KDF (Wi-Fi Aware v4.0 §7.1.3.5) — alias of PRF-Length in
 * IEEE 802.11-2020 §12.7.1.7.2 with HMAC-SHA-256 as the underlying
 * pseudo-random function. Used to derive NAN PTK (KCK || KEK || TK).
 * PRF-Length(Key, Label, Data) -> buf_len bytes.
 */
static int nan_sha256_prf(const uint8_t *key, size_t key_len, const char *label,
                          const uint8_t *data, size_t data_len,
                          uint8_t *buf, size_t buf_len)
{
    const unsigned char *addr[4];
    int len_arr[4];
    uint8_t counter_le[2];
    uint8_t length_le[2];
    uint16_t counter = 1;
    size_t pos = 0;
    const size_t hash_len = 32;
    uint8_t hash[32];

    if (!g_wifi_default_wpa_crypto_funcs.hmac_sha256_vector || !key || !buf) {
        return -1;
    }

    length_le[0] = (uint8_t)((buf_len * 8) & 0xFF);
    length_le[1] = (uint8_t)((buf_len * 8) >> 8);

    addr[0] = counter_le;
    len_arr[0] = 2;
    addr[1] = (const unsigned char *)label;
    len_arr[1] = (int)strlen(label);
    addr[2] = data;
    len_arr[2] = (int)data_len;
    addr[3] = length_le;
    len_arr[3] = 2;

    while (pos < buf_len) {
        counter_le[0] = (uint8_t)(counter & 0xFF);
        counter_le[1] = (uint8_t)((counter >> 8) & 0xFF);
        if (pos + hash_len <= buf_len) {
            if (g_wifi_default_wpa_crypto_funcs.hmac_sha256_vector(key, (int)key_len, 4, addr, len_arr, &buf[pos]) != 0) {
                return -1;
            }
            pos += hash_len;
        } else {
            if (g_wifi_default_wpa_crypto_funcs.hmac_sha256_vector(key, (int)key_len, 4, addr, len_arr, hash) != 0) {
                return -1;
            }
            memcpy(&buf[pos], hash, buf_len - pos);
            pos = buf_len;
        }
        counter++;
    }
    return 0;
}

/*
 * Derive ND-PTK (KCK, KEK, TK) for NCS-SK-128 (Wi-Fi Aware v4.0 §7.1.3.5):
 *   PRF-Length(ND-PMK, "NAN Pairwise key expansion",
 *              IAddr || RAddr || INonce || RNonce)
 * Per spec, IAddr/RAddr are the NDP Initiator/Responder Data Interface
 * addresses (NDI) — caller must pass NDIs, not NMIs. INonce=ANonce,
 * RNonce=SNonce.
 */
static int nan_ndp_ptk_derive(const uint8_t *pmk, const uint8_t *i_addr, const uint8_t *r_addr,
                              const uint8_t *anonce, const uint8_t *snonce,
                              uint8_t *kck_out, uint8_t *kek_out, uint8_t *tk_out)
{
    uint8_t seed[6 + 6 + NAN_NONCE_LEN + NAN_NONCE_LEN];
    uint8_t ptk[NAN_NCS_SK_128_PTK_LEN];
    size_t off = 0;

    memcpy(seed + off, i_addr, 6);
    off += 6;
    memcpy(seed + off, r_addr, 6);
    off += 6;
    memcpy(seed + off, anonce, NAN_NONCE_LEN);
    off += NAN_NONCE_LEN;
    memcpy(seed + off, snonce, NAN_NONCE_LEN);

    if (nan_sha256_prf(pmk, ESP_WIFI_NAN_NDP_PMK_LEN, "NAN Pairwise key expansion",
                       seed, sizeof(seed), ptk, sizeof(ptk)) != 0) {
        return -1;
    }
    memcpy(kck_out, ptk, NAN_NCS_SK_128_KCK_LEN);
    memcpy(kek_out, ptk + NAN_NCS_SK_128_KCK_LEN, NAN_NCS_SK_128_KEK_LEN);
    memcpy(tk_out, ptk + NAN_NCS_SK_128_KCK_LEN + NAN_NCS_SK_128_KEK_LEN, NAN_NCS_SK_128_TK_LEN);
    return 0;
}

/*
 * Lazy initiator PTK derivation. No-op if ndl->ptk_set is already 1.
 * Caller MUST hold NAN_DATA_LOCK. On success, ndl->nd_kck/kek/tk and
 * the corresponding *_len fields are populated and ptk_set=1.
 *
 * Spec §7.1.3.5: PTK PRF takes Data Interface addresses. Local NDI is
 * obtained via esp_wifi_get_mac(WIFI_IF_NAN); peer NDI lives in
 * ndl->peer_ndi, populated by ndp_response_indication on M2 RX.
 *
 * Returns 0 on success, -1 on failure.
 */
static int ndl_ensure_ptk(struct ndl_info *ndl)
{
    if (ndl->ptk_set) {
        return 0;
    }
    uint8_t our_mac[6];
    if (esp_wifi_get_mac(WIFI_IF_NAN, our_mac) != ESP_OK) {
        ESP_LOGE(TAG, "ndl_ensure_ptk: get our MAC failed");
        return -1;
    }
    if (nan_ndp_ptk_derive(ndl->security_ctx.nd_pmk,
                           our_mac,         /* IAddr = Initiator NDI (us) */
                           ndl->peer_ndi,   /* RAddr = Responder NDI (peer) */
                           ndl->anonce, ndl->snonce,
                           ndl->nd_kck, ndl->nd_kek, ndl->nd_tk) != 0) {
        ESP_LOGE(TAG, "ndl_ensure_ptk: PTK derivation failed");
        return -1;
    }
    ndl->kck_len = NAN_NCS_SK_128_KCK_LEN;
    ndl->kek_len = NAN_NCS_SK_128_KEK_LEN;
    ndl->tk_len  = NAN_NCS_SK_128_TK_LEN;
    ndl->ptk_set = 1;
    return 0;
}

/* Find NDL by publisher_id (e.g. when parsing key desc and ndp_id/peer_nmi unknown). */
static struct ndl_info *nan_find_ndl_by_pub_id(uint8_t pub_id)
{
    struct ndl_info *ndl_in_progress = NULL;
    struct ndl_info *ndl_any = NULL;

    for (int i = 0; i < ESP_WIFI_NAN_DATAPATH_MAX_PEERS; i++) {
        struct ndl_info *ndl = &s_nan_ctx.ndl[i];
        if (ndl->ndp_id == 0) {
            continue;
        }
        if (ndl->publisher_id != pub_id) {
            continue;
        }
        ndl_any = ndl;
        if (ndl->handshake_state != NAN_HANDSHAKE_IDLE) {
            ndl_in_progress = ndl;
            break;
        }
    }
    return ndl_in_progress ? ndl_in_progress : ndl_any;
}

/*
 * Claim a free NDL slot for (pub_id, peer_nmi) when parsing CSIA/SCIA/key
 * desc before NDP/NDL. Caller runs in wifi task (no NAN_DATA_LOCK).
 * Later nan_record_new_ndl will find this by pub_id+peer and update ndp_id.
 */
static struct ndl_info *nan_ndl_claim_for_pub_peer(uint8_t pub_id, const uint8_t *peer_nmi, uint8_t ndp_id)
{
    if (!peer_nmi) {
        return NULL;
    }
    for (int i = 0; i < ESP_WIFI_NAN_DATAPATH_MAX_PEERS; i++) {
        struct ndl_info *ndl = &s_nan_ctx.ndl[i];
        if (ndl->ndp_id == 0) {
            ndl->publisher_id = pub_id;
            MACADDR_COPY(ndl->peer_nmi, peer_nmi);
            ndl->ndp_id = ndp_id;
            return ndl;
        }
    }
    return NULL;
}

/*
 * Fill NDL security context from paired-peer cache (produced at PASN pairing
 * completion). Caller must hold NAN_DATA_LOCK.
 */
#if defined(CONFIG_ESP_WIFI_NAN_PAIRING)
static bool nan_security_fill_from_paired_cache(struct ndl_info *ndl, const uint8_t *peer_nmi)
{
    const struct nan_paired_peer *paired;

    if (!ndl || !peer_nmi) {
        return false;
    }

    paired = nan_app_find_paired_peer(peer_nmi);
    if (!paired || !paired->valid) {
        return false;
    }
    if (paired->ndp_csid == 0 || paired->ndp_csid > 15) {
        return false;
    }

    ndl->security_ctx.type = WIFI_NAN_SECURITY_ENCRYPTED;
    ndl->security_ctx.csid_bitmap = (uint16_t)(1u << paired->ndp_csid);
    memcpy(ndl->security_ctx.nd_pmk, paired->nd_pmk, ESP_WIFI_NAN_NDP_PMK_LEN);

    ESP_LOGD(TAG, "Loaded ND-PMK from paired-peer cache for peer "MACSTR" (csid=%u)",
             MAC2STR(peer_nmi), paired->ndp_csid);
    return true;
}
#endif

/*
 * Build RSNA Key Descriptor payload (95-byte EAPOL-Key layout, see
 * IEEE 802.11-2020 §12.7.2). NAN carries this body inside the NAN
 * Shared Key Descriptor attribute (Wi-Fi Aware v4.0 §9.5.21).
 *
 * Mirrors hostap nan_sec_add_key_attrs argument shape (instance_id is carried
 * by the outer attribute, so this helper only populates the key descriptor
 * body): caller supplies the variable bits (MIC/SECURE/INSTALL/ACK) and an
 * optional nonce. KEY_TYPE (Pairwise) is always set; Version stays 0
 * (NCS-SK uses AES-CMAC). MIC is left as zero for the caller to fill.
 */
static int nan_build_rsna_key_descriptor(uint8_t *kd, uint16_t key_info_flags,
                                         const uint8_t *nonce,
                                         const uint8_t *replay_counter,
                                         const uint8_t *key_rsc)
{
    if (!kd || !replay_counter || !key_rsc) {
        return 0;
    }

    kd[NAN_KEY_DESC_TYPE_OFF] = NAN_KEY_DESC_TYPE_RSN;

    uint16_t key_info = key_info_flags | NAN_KEY_INFO_KEY_TYPE;
    kd[NAN_KEY_DESC_KEY_INFO_OFF]     = (key_info >> 8) & 0xFF;
    kd[NAN_KEY_DESC_KEY_INFO_OFF + 1] = (key_info >> 0) & 0xFF;

    kd[NAN_KEY_DESC_KEY_LEN_OFF]     = 0;
    kd[NAN_KEY_DESC_KEY_LEN_OFF + 1] = 0;

    memcpy(&kd[NAN_KEY_DESC_REPLAY_OFF], replay_counter, NAN_REPLAY_COUNTER_LEN);

    if (nonce) {
        memcpy(&kd[NAN_KEY_DESC_NONCE_OFF], nonce, NAN_NONCE_LEN);
    } else {
        memset(&kd[NAN_KEY_DESC_NONCE_OFF], 0, NAN_NONCE_LEN);
    }

    memset(&kd[NAN_KEY_DESC_IV_OFF], 0, 16);
    memcpy(&kd[NAN_KEY_DESC_RSC_OFF], key_rsc, NAN_KEY_RSC_LEN);
    memset(&kd[NAN_KEY_DESC_KEY_ID_OFF], 0, 8);
    memset(&kd[NAN_KEY_DESC_MIC_OFF], 0, NAN_NCS_SK_128_MIC_LEN);

    kd[NAN_KEY_DESC_DATA_LEN_OFF]     = 0;
    kd[NAN_KEY_DESC_DATA_LEN_OFF + 1] = 0;

    return NAN_KEY_DESC_MIN_LEN;
}

/*
 * Internal SCIA builder shared by Publish SDF and NDP-Response paths.
 * Per-entry layout: Len(2) + Type(1) + PubID(1) + Value(PMKID_LEN) = 20 bytes.
 */
static int nan_build_scia_attr(uint8_t *frm, uint8_t pub_id,
                               const uint8_t (*pmkids)[ESP_WIFI_NAN_NDP_PMKID_LEN],
                               uint8_t num_pmkids)
{
    if (!frm || !pmkids || !num_pmkids) {
        return 0;
    }

    uint8_t *p = frm;

    *p++ = NAN_ATTR_ID_SECURITY_CONTEXT;

    uint16_t attr_len = 20 * num_pmkids;
    *p++ = attr_len & 0xFF;
    *p++ = (attr_len >> 8) & 0xFF;

    for (uint8_t i = 0; i < num_pmkids; i++) {
        ESP_LOG_BUFFER_HEXDUMP(TAG, pmkids[i], ESP_WIFI_NAN_NDP_PMKID_LEN, ESP_LOG_DEBUG);

        uint16_t val_len = ESP_WIFI_NAN_NDP_PMKID_LEN;
        *p++ = val_len & 0xFF;
        *p++ = (val_len >> 8) & 0xFF;
        *p++ = NAN_SEC_CTX_TYPE_ND_PMKID;
        *p++ = pub_id;
        memcpy(p, pmkids[i], ESP_WIFI_NAN_NDP_PMKID_LEN);
        p += ESP_WIFI_NAN_NDP_PMKID_LEN;
    }

    return (int)(p - frm);
}

/*
 * Compute Key MIC over a NAN action frame body and patch the Shared Key
 * Descriptor attribute in place. Mirrors hostap nan_sec_pre_tx() (single
 * entry point for M2/M3/M4 MIC); separated only because the dispatch by
 * frame subtype lives in the WiFi blob, not here.
 *
 * If `include_auth_token` is true, the HMAC scope is `Auth_Token || body`
 * per Wi-Fi Aware v4.0 §7.1.3.5 M3 construction; Auth_Token is taken from
 * ndl->auth_token (captured during M1 TX/RX). Otherwise the scope is
 * `body` only (M2/M4 construction).
 */
static int nan_update_kd_mic(uint8_t *body, size_t body_len, uint8_t *key_desc_attr,
                             uint8_t ndp_id, const uint8_t *peer_nmi, const char *label,
                             bool include_auth_token)
{
    if (!body || !key_desc_attr || !peer_nmi || body_len == 0) {
        ESP_LOGE(TAG, "NDP %s Update MIC: invalid parameters", label);
        return -1;
    }

    if (!g_wifi_default_wpa_crypto_funcs.hmac_sha256_vector) {
        ESP_LOGE(TAG, "NDP %s Update MIC: hmac_sha256_vector not registered", label);
        return -1;
    }

    if (key_desc_attr < body || key_desc_attr >= (body + body_len)) {
        ESP_LOGE(TAG, "NDP %s Update MIC: key_desc_attr outside body", label);
        ESP_LOGE(TAG, "  body=%p, body_len=%zu, end=%p", (void *)body, body_len,
                 (void *)(body + body_len));
        ESP_LOGE(TAG, "  key_desc_attr=%p, offset=%td", (void *)key_desc_attr,
                 key_desc_attr - body);
        ESP_LOG_BUFFER_HEXDUMP(TAG, body, body_len, ESP_LOG_ERROR);
        return -1;
    }

    NAN_DATA_LOCK();
    struct ndl_info *ndl = nan_find_ndl(ndp_id, (uint8_t *)peer_nmi);
    if (!ndl) {
        NAN_DATA_UNLOCK();
        ESP_LOGE(TAG, "NDP %s Update MIC: no NDL for ndp_id=%d", label, ndp_id);
        return -1;
    }

    if (!ndl->ptk_set) {
        NAN_DATA_UNLOCK();
        ESP_LOGE(TAG, "NDP %s Update MIC: PTK not set for ndp_id=%d", label, ndp_id);
        return -1;
    }

    if (include_auth_token && ndl->auth_token_len == 0) {
        NAN_DATA_UNLOCK();
        ESP_LOGE(TAG, "NDP %s Update MIC: Auth_Token not captured (call esp_nan_capture_m1_auth_token after M1 assembly)", label);
        return -1;
    }

    if ((key_desc_attr + 3) > (body + body_len)) {
        NAN_DATA_UNLOCK();
        ESP_LOGE(TAG, "NDP %s Update MIC: attr header out of bounds", label);
        return -1;
    }

    /* Skip attribute header (3) + pub_id (1) to reach the 95-byte key desc */
    uint8_t *key_desc = key_desc_attr + 3 + 1;

    if ((key_desc + NAN_KEY_DESC_MIC_OFF + NAN_NCS_SK_128_MIC_LEN) > (body + body_len)) {
        NAN_DATA_UNLOCK();
        ESP_LOGE(TAG, "NDP %s Update MIC: MIC field out of bounds", label);
        return -1;
    }

    /* MIC field must be zeroed before HMAC computation
     * (Wi-Fi Aware v4.0 §7.1.3.5 / IEEE 802.11-2020 §12.7.2). */
    memset(&key_desc[NAN_KEY_DESC_MIC_OFF], 0, NAN_NCS_SK_128_MIC_LEN);

    ESP_LOGD(TAG, "NDP %s Update MIC: body (%zu bytes, MIC zeroed) for ndp_id=%d:",
             label, body_len, ndp_id);
    ESP_LOG_BUFFER_HEXDUMP(TAG, body, body_len, ESP_LOG_DEBUG);

    uint8_t mic_buf[32];
    const unsigned char *addr[2];
    int len_arr[2];
    int n_vec;
    if (include_auth_token) {
        addr[0] = ndl->auth_token;
        len_arr[0] = (int)ndl->auth_token_len;
        addr[1] = body;
        len_arr[1] = (int)body_len;
        n_vec = 2;
    } else {
        addr[0] = body;
        len_arr[0] = (int)body_len;
        n_vec = 1;
    }

    if (g_wifi_default_wpa_crypto_funcs.hmac_sha256_vector(ndl->nd_kck,
                                                           NAN_NCS_SK_128_KCK_LEN,
                                                           n_vec, addr, len_arr,
                                                           mic_buf) != 0) {
        NAN_DATA_UNLOCK();
        ESP_LOGE(TAG, "NDP %s Update MIC: HMAC-SHA256 failed", label);
        return -1;
    }

    memcpy(&key_desc[NAN_KEY_DESC_MIC_OFF], mic_buf, NAN_NCS_SK_128_MIC_LEN);

    ESP_LOGD(TAG, "NDP %s Update MIC: KCK:", label);
    ESP_LOG_BUFFER_HEXDUMP(TAG, ndl->nd_kck, NAN_NCS_SK_128_KCK_LEN, ESP_LOG_DEBUG);
    ESP_LOGD(TAG, "NDP %s Update MIC: computed MIC%s:",
             label, include_auth_token ? " (scope = Auth_Token || body)" : "");
    ESP_LOG_BUFFER_HEXDUMP(TAG, mic_buf, NAN_NCS_SK_128_MIC_LEN, ESP_LOG_DEBUG);

    NAN_DATA_UNLOCK();
    return 0;
}

/*
 * Map RSNA Key Info bits (IEEE 802.11-2020 §12.7.2 Table 12-7) to
 * NDP 4-way handshake message number (1..4) per Wi-Fi Aware v4.0
 * §7.1.3.5 NCS-SK construction.
 * Returns 0 for an unrecognized combination.
 *
 * Hostap dispatches by NAN Action subtype directly; we infer here because the
 * subtype isn't plumbed into this entry point. Keeping the inference in one
 * place mirrors hostap's centralized validation in nan_sec_rx().
 */
static uint8_t nan_keyinfo_to_msg_type(uint16_t key_info)
{
    bool has_mic     = (key_info & NAN_KEY_INFO_MIC) != 0;
    bool has_secure  = (key_info & NAN_KEY_INFO_SECURE) != 0;
    bool has_install = (key_info & NAN_KEY_INFO_INSTALL) != 0;
    bool has_ack     = (key_info & NAN_KEY_INFO_ACK) != 0;

    if (!has_mic && !has_secure) {
        return 1;
    }
    if (has_mic && !has_secure) {
        return 2;
    }
    if (has_mic && has_secure && !has_install) {
        return 3;
    }
    if (has_mic && has_secure && has_install) {
        /*
         * Interop mapping observed in Android captures:
         *  - M3: KeyInfo=0x03c8 (Install=1, ACK=1, MIC=1, Secure=1, Pairwise=1)
         *  - M4: KeyInfo=0x0348 (Install=1, ACK=0, MIC=1, Secure=1, Pairwise=1)
         */
        return has_ack ? 3 : 4;
    }
    return 0;
}

/*-------------------------------------------------------------------------
 * Public API: PMK/PMKID derivation for publish
 *-----------------------------------------------------------------------*/

esp_err_t nan_derive_security_params(const char *service_name,
                                     const wifi_nan_discovery_security_params_t *sec_cfg,
                                     wifi_nan_security_params_t *out_derived)
{
    uint8_t pmk[ESP_WIFI_NAN_NDP_PMK_LEN];
    uint8_t service_id[6];
    uint8_t hash[32];

    if (!service_name || !sec_cfg || !out_derived) {
        return ESP_ERR_INVALID_ARG;
    }
    if (sec_cfg->num_credentials == 0 ||
            sec_cfg->num_credentials > ESP_WIFI_NAN_MAX_CREDS_PER_SVC) {
        ESP_LOGE(TAG, "Invalid num_credentials=%u (cap %u)",
                 sec_cfg->num_credentials, ESP_WIFI_NAN_MAX_CREDS_PER_SVC);
        return ESP_ERR_INVALID_ARG;
    }

    memset(out_derived, 0,
           sizeof(*out_derived) * ESP_WIFI_NAN_MAX_CREDS_PER_SVC);

    if (!nan_compute_service_id(service_name, service_id)) {
        ESP_LOGE(TAG, "Service ID derivation failed");
        return ESP_FAIL;
    }
    if (!g_wifi_default_wpa_crypto_funcs.hmac_sha256_vector) {
        ESP_LOGE(TAG, "hmac_sha256_vector not registered");
        return ESP_FAIL;
    }

    ESP_LOGD(TAG, "Security derivation: svc='%s' num_creds=%u",
             service_name, sec_cfg->num_credentials);
    ESP_LOG_BUFFER_HEXDUMP(TAG, service_id, 6, ESP_LOG_DEBUG);

    /* Publish-side PMKID uses IAddr=ff:ff:ff:ff:ff:ff (initiator-independent);
     * pair-specific PMKID for NDP Request derived separately. */
    uint8_t i_addr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t r_addr[6];
    esp_wifi_get_mac(WIFI_IF_NAN, r_addr);

    for (uint8_t i = 0; i < sec_cfg->num_credentials; i++) {
        const wifi_nan_credential_t *cred = &sec_cfg->creds[i];

        if (cred->csid == 0) {
            ESP_LOGE(TAG, "Cred[%u] missing csid", i);
            goto fail;
        }

        if (cred->csid == WIFI_NAN_CSID_NCS_SK_128) {
            if (cred->use_pmk) {
                memcpy(pmk, cred->pmk, ESP_WIFI_NAN_NDP_PMK_LEN);
            } else {
                if (nan_derive_nd_pmk_from_passphrase(cred->passphrase, cred->csid,
                                                      service_id, r_addr, pmk) != 0) {
                    ESP_LOGE(TAG, "Cred[%u]: PBKDF2 ND-PMK derivation failed", i);
                    goto fail;
                }
            }

            const unsigned char *addr_pmkid[4] = {(const unsigned char *)NAN_PMK_NAME_LABEL,
                                                  i_addr, r_addr, service_id
                                                 };
            int len_pmkid[4] = {NAN_PMK_NAME_LABEL_LEN, 6, 6, 6};

            g_wifi_default_wpa_crypto_funcs.hmac_sha256_vector(pmk, ESP_WIFI_NAN_NDP_PMK_LEN,
                                                               4, addr_pmkid, len_pmkid, hash);

            memcpy(out_derived[i].nd_pmk,   pmk,  ESP_WIFI_NAN_NDP_PMK_LEN);
            memcpy(out_derived[i].nd_pmkid, hash, ESP_WIFI_NAN_NDP_PMKID_LEN);
            forced_memzero(pmk, sizeof(pmk));
        } else if (cred->csid == WIFI_NAN_CSID_NCS_PK_PASN_128) {
            /* Fill PMKID with 0xff before pairing, ND-PMK to be derived at the end of pairing */
            memset(out_derived[i].nd_pmkid, 0xff, ESP_WIFI_NAN_NDP_PMKID_LEN);
        }
        out_derived[i].type            = WIFI_NAN_SECURITY_ENCRYPTED;
        out_derived[i].csid_bitmap     = (uint16_t)(1u << cred->csid);
        out_derived[i].group_data_prot = sec_cfg->group_data_prot;
        out_derived[i].group_mgmt_prot = sec_cfg->group_mgmt_prot;
    }

    /* Mirror the derived material into the host's own_svc_info slot for this
     * service (claimed before the blob's publish/subscribe call). This lets
     * host paths — nan_match_pmkid (responder M1) and the NDL seeding at
     * nan_app.c — read derived ND-PMK + PMKID without a second PBKDF2 pass
     * on the app/main task. Single derive, two destinations. */
    {
        struct own_svc_info *p_svc = nan_find_own_svc_by_name(service_name);
        if (p_svc) {
            memcpy(p_svc->derived_security, out_derived,
                   sizeof(*out_derived) * sec_cfg->num_credentials);
        }
    }
    return ESP_OK;

fail:
    forced_memzero(pmk, sizeof(pmk));
    memset(out_derived, 0,
           sizeof(*out_derived) * ESP_WIFI_NAN_MAX_CREDS_PER_SVC);
    return ESP_FAIL;
}

/*
 * Blob-side gate query: blob calls this to learn whether an NDP is
 * secured (and which cipher) without touching ndl->security_ctx
 * directly. Lookup honors the initiator pre-claim convention: ndp_id=0
 * matches the slot keyed by peer_nmi alone, used between
 * esp_nan_internal_datapath_req() entry and the blob assigning the
 * real ndp_id.
 */
uint16_t nan_get_ndp_security_csid(uint8_t ndp_id, const uint8_t *peer_nmi)
{
    if (!peer_nmi) {
        return 0;
    }
    NAN_DATA_LOCK();
    struct ndl_info *ndl = nan_find_ndl(ndp_id, (uint8_t *)peer_nmi);
    uint16_t csid = ndl ? ndl->security_ctx.csid_bitmap : 0;
    NAN_DATA_UNLOCK();
    return csid;
}

bool nan_security_service_match(const struct own_svc_info *own_svc,
                                struct peer_svc_info *peer_svc,
                                const uint8_t *publisher_nmi,
                                const wifi_nan_peer_sdf_security_t *peer_sec)
{
    uint8_t pmk[ESP_WIFI_NAN_NDP_PMK_LEN];
    uint8_t service_id[6];
    uint8_t pmkid[ESP_WIFI_NAN_NDP_PMKID_LEN];
    uint8_t hash[32];
    bool matched = false;

    if (!own_svc || !peer_svc || !publisher_nmi || !peer_sec) {
        return false;
    }

    const wifi_nan_discovery_security_params_t *cfg = &own_svc->user_cfg;

    if (cfg->num_credentials == 0 ||
            cfg->num_credentials > ESP_WIFI_NAN_MAX_CREDS_PER_SVC) {
        return false;
    }

    if (nan_csid_bitmap_has_pasn(peer_sec->csid_bitmap)) {
        int c;
        for (c = 0; c < cfg->num_credentials; c++) {
            const wifi_nan_credential_t *cred = &cfg->creds[c];
            if (peer_sec->csid_bitmap & (1 << cred->csid)) {
                return true;
            }
        }
        if (c == cfg->num_credentials) {
            return false;
        }
    }

    if (peer_sec->num_pmkids == 0) {
        return false;
    }
    if (!nan_compute_service_id(own_svc->svc_name, service_id)) {
        return false;
    }
    if (!g_wifi_default_wpa_crypto_funcs.hmac_sha256_vector) {
        return false;
    }

    uint8_t i_addr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    const unsigned char *addr_pmkid[4] = {(const unsigned char *)NAN_PMK_NAME_LABEL,
                                          i_addr, publisher_nmi, service_id
                                         };
    int len_pmkid[4] = {NAN_PMK_NAME_LABEL_LEN, 6, 6, 6};

    /* Cross-product: each local credential vs each peer-advertised PMKID. First
     * match wins. Both sides honor the spec PMKID formula with IAddr broadcast,
     * RAddr = publisher_nmi, so a (local cred, peer SCID) match indicates a
     * shared ND-PMK for this service. */
    for (uint8_t c = 0; c < cfg->num_credentials; c++) {
        const wifi_nan_credential_t *cred = &cfg->creds[c];
        if (cred->csid == 0) {
            continue;
        }

        if (cred->use_pmk) {
            memcpy(pmk, cred->pmk, sizeof(pmk));
        } else {
            if (nan_derive_nd_pmk_from_passphrase(cred->passphrase, cred->csid,
                                                  service_id, publisher_nmi, pmk) != 0) {
                continue;
            }
        }

        g_wifi_default_wpa_crypto_funcs.hmac_sha256_vector(pmk, ESP_WIFI_NAN_NDP_PMK_LEN,
                                                           4, addr_pmkid, len_pmkid, hash);
        memcpy(pmkid, hash, ESP_WIFI_NAN_NDP_PMKID_LEN);

        for (uint8_t i = 0; i < peer_sec->num_pmkids && i < NAN_PEER_MAX_PMKIDS; i++) {
            if (memcmp(pmkid, peer_sec->pmkids[i], ESP_WIFI_NAN_NDP_PMKID_LEN) == 0) {
                ESP_LOGD(TAG, "ND-PMKID match: svc='%s' cred[%u] vs peer SCID[%u], publisher "MACSTR,
                         own_svc->svc_name, c, i, MAC2STR(publisher_nmi));
                ESP_LOG_BUFFER_HEXDUMP(TAG, pmkid, ESP_WIFI_NAN_NDP_PMKID_LEN, ESP_LOG_INFO);
                /* Remember which local cred matched, so the initiator NDP-req
                 * path (nan_security_populate_initiator_ndl) derives M1's
                 * pair-PMKID from the right credential. */
                peer_svc->matched_cred_idx = c;
                matched = true;
                goto done;
            }
        }
    }

done:
    forced_memzero(pmk, sizeof(pmk));
    return matched;
}

/*
 * Populate the initiator NDL's security_ctx from cached subscriber params so
 * the blob's CSIA / SCIA / Shared-Key Descriptor builder callbacks can find
 * the cipher + pair-PMKID + ND-PMK keyed by (ndp_id, peer_nmi). Writing into
 * the NDL (rather than wifi_nan_datapath_req_t) keeps key material off the
 * public API surface -- callers never see ND-PMK or PMKID.
 *
 * Computes the pair-specific PMKID using L(HMAC(PMK, "NAN PMK Name" ||
 * Initiator NMI || Responder NMI || Service_ID), 0, 128), distinct from the
 * publish-time discovery PMKID (which uses broadcast IAddr).
 */
esp_err_t nan_security_populate_initiator_ndl(struct ndl_info *ndl,
                                              const struct own_svc_info *own_svc,
                                              const struct peer_svc_info *peer_svc,
                                              const uint8_t *peer_nmi)
{
    const wifi_nan_credential_t *cred = NULL;
#if defined(CONFIG_ESP_WIFI_NAN_PAIRING)
    const struct nan_paired_peer *paired = NULL;
#endif

    if (!ndl || !own_svc || !peer_nmi) {
        return ESP_FAIL;
    }
    const wifi_nan_discovery_security_params_t *cfg = &own_svc->user_cfg;
#if defined(CONFIG_ESP_WIFI_NAN_PAIRING)
    paired = nan_app_find_paired_peer(peer_nmi);
#endif
    if (cfg->num_credentials == 0
#if defined(CONFIG_ESP_WIFI_NAN_PAIRING)
            && (!paired || !paired->valid)
#endif
       ) {
        return ESP_OK; /* subscriber didn't request encrypted datapath */
    }

    /* Use the credential that matched this publisher at SDF RX (stored on the
     * peer_svc by nan_security_service_match). Falls back to creds[0] only
     * when no SDF-time match was recorded (atypical: subscriber initiated NDP
     * without a prior service match). */
    uint8_t matched_idx = peer_svc ? peer_svc->matched_cred_idx : NAN_NO_MATCHED_CRED;
    uint8_t cred_idx = (matched_idx != NAN_NO_MATCHED_CRED) ? matched_idx : 0;
    if (cred_idx >= cfg->num_credentials) {
        cred_idx = 0;
    }
    if (cfg->num_credentials > 0) {
        cred = &cfg->creds[cred_idx];
        ESP_LOGD(TAG, "NDP req security: svc='%s' using cred[%u] for peer "MACSTR
                 " (matched_idx=%d, num_creds=%u)",
                 own_svc->svc_name, cred_idx, MAC2STR(peer_nmi),
                 (matched_idx == NAN_NO_MATCHED_CRED) ? -1 : (int)matched_idx,
                 cfg->num_credentials);
    }

    uint8_t pmk[ESP_WIFI_NAN_NDP_PMK_LEN];
    uint8_t service_id[6];
    uint8_t our_nmi[6];
    esp_err_t ret = ESP_FAIL;

    if (esp_wifi_get_mac(WIFI_IF_NAN, our_nmi) != ESP_OK) {
        ESP_LOGE(TAG, "NDP req security: get own NMI failed");
        goto cleanup;
    }
    if (!nan_compute_service_id(own_svc->svc_name, service_id)) {
        ESP_LOGE(TAG, "NDP req security: service id derivation failed");
        goto cleanup;
    }

    uint8_t ndp_csid = 0;
#if defined(CONFIG_ESP_WIFI_NAN_PAIRING)
    if (paired && paired->valid && paired->ndp_csid >= WIFI_NAN_CSID_NCS_SK_128 &&
            paired->ndp_csid <= WIFI_NAN_CSID_NCS_SK_256) {
        memcpy(pmk, paired->nd_pmk, ESP_WIFI_NAN_NDP_PMK_LEN);
        ndp_csid = paired->ndp_csid;
        ESP_LOGD(TAG, "NDP req security: using paired-peer cached ND-PMK for " MACSTR
                 " (csid=%u)", MAC2STR(peer_nmi), ndp_csid);
    } else
#endif
    {
        if (!cred) {
            ESP_LOGE(TAG, "NDP req security: no credential and no paired cache for peer " MACSTR,
                     MAC2STR(peer_nmi));
            goto cleanup;
        }
        if (cred->use_pmk) {
            memcpy(pmk, cred->pmk, ESP_WIFI_NAN_NDP_PMK_LEN);
        } else {
            if (cred->csid == 0) {
                goto cleanup;
            }
            if (nan_derive_nd_pmk_from_passphrase(cred->passphrase, cred->csid, service_id,
                                                  peer_nmi, pmk) != 0) {
                ESP_LOGE(TAG, "NDP req security: passphrase->PMK failed");
                goto cleanup;
            }
        }
        ndp_csid = cred->csid;
    }

    uint8_t pair_pmkid[ESP_WIFI_NAN_NDP_PMKID_LEN];
    if (!nan_derive_ndp_request_pmkid(pmk, our_nmi, peer_nmi, service_id, pair_pmkid)) {
        ESP_LOGE(TAG, "NDP req security: pair PMKID derivation failed");
        goto cleanup;
    }

    ndl->security_ctx.type        = WIFI_NAN_SECURITY_ENCRYPTED;
    ndl->security_ctx.csid_bitmap = (uint16_t)(1u << ndp_csid);
    memcpy(ndl->security_ctx.nd_pmk,   pmk,        ESP_WIFI_NAN_NDP_PMK_LEN);
    memcpy(ndl->security_ctx.nd_pmkid, pair_pmkid, ESP_WIFI_NAN_NDP_PMKID_LEN);

    ESP_LOGD(TAG, "NDP req security: derived ND-PMK + pair PMKID for peer "MACSTR, MAC2STR(peer_nmi));
    ESP_LOG_BUFFER_HEXDUMP(TAG, pair_pmkid, ESP_WIFI_NAN_NDP_PMKID_LEN, ESP_LOG_DEBUG);
    ret = ESP_OK;

cleanup:
    forced_memzero(pmk, sizeof(pmk));
    return ret;
}

/*-------------------------------------------------------------------------
 * Public API: CSIA / SCIA / Shared Key Descriptor construction
 *-----------------------------------------------------------------------*/

int esp_nan_construct_csia(uint8_t *frm, uint8_t pub_id, uint16_t own_csid_bitmap, uint16_t peer_csid_bitmap)
{
    uint16_t csid_bitmap = peer_csid_bitmap ? (own_csid_bitmap & peer_csid_bitmap) : own_csid_bitmap;
    csid_bitmap &= NAN_CSID_VALID_BITMAP;

    ESP_LOGD(TAG, "Constructing CSIA: pub_id=%d, own=0x%04x, peer=0x%04x, effective=0x%04x",
             pub_id, own_csid_bitmap, peer_csid_bitmap, csid_bitmap);
    if (!frm || !csid_bitmap) {
        return 0;
    }

    uint8_t num_csids = __builtin_popcount(csid_bitmap);
    uint8_t *p = frm;

    *p++ = NAN_ATTR_ID_CIPHER_SUITE_INFO;

    uint16_t attr_len = 1 + 2 * num_csids;
    *p++ = attr_len & 0xFF;
    *p++ = (attr_len >> 8) & 0xFF;

    /* Capabilities byte (0x4 set for iOS interop) */
    *p++ = 0x4;

    /* Cipher Suite ID list: [CSID(1)] [Publish ID(1)] */
    for (uint8_t csid = 1; csid <= 8; csid++) {
        if (csid_bitmap & (1 << csid)) {
            *p++ = csid;
            *p++ = pub_id;
        }
    }

    return (int)(p - frm);
}

int esp_nan_construct_scia_publish(uint8_t *frm, uint8_t pub_id,
                                   uint8_t num_pmkids,
                                   const uint8_t pmkids[][ESP_WIFI_NAN_NDP_PMKID_LEN])
{
    ESP_LOGD(TAG, "Constructing SCIA (Publish SDF): pub_id=%d, num_pmkids=%d", pub_id, num_pmkids);
    return nan_build_scia_attr(frm, pub_id, pmkids, num_pmkids);
}

int esp_nan_construct_scia_ndp_resp(uint8_t *frm, uint8_t ndp_id, const uint8_t *peer_nmi)
{
    if (!frm || !peer_nmi) {
        return 0;
    }

    NAN_DATA_LOCK();
    struct ndl_info *ndl = nan_find_ndl(ndp_id, (uint8_t *)peer_nmi);
    if (!ndl) {
        /* Initiator path: blob calls SCIA builder before the key-desc builder
         * has had a chance to patch the pre-claimed NDL's ndp_id (still 0).
         * Fall back to peer-only lookup so we find the NDL that
         * esp_wifi_nan_datapath_req() pre-claimed and seeded with security
         * context. The key-desc builder will assign the real ndp_id shortly
         * after. */
        ndl = nan_find_ndl(0, (uint8_t *)peer_nmi);
        if (!ndl) {
            NAN_DATA_UNLOCK();
            ESP_LOGW(TAG, "NDP SCIA: no NDL for ndp_id=%d peer="MACSTR, ndp_id, MAC2STR(peer_nmi));
            return 0;
        }
    }

    static const uint8_t zero_pmkid[ESP_WIFI_NAN_NDP_PMKID_LEN] = {0};
    if (memcmp(ndl->security_ctx.nd_pmkid, zero_pmkid, ESP_WIFI_NAN_NDP_PMKID_LEN) == 0) {
        NAN_DATA_UNLOCK();
        ESP_LOGW(TAG, "NDP Response SCIA: PMKID not set for ndp_id=%d", ndp_id);
        return 0;
    }

    ESP_LOGD(TAG, "Constructing SCIA (NDP Response M2): ndp_id=%d, pub_id=%d", ndp_id, ndl->publisher_id);

    uint8_t pub_id = ndl->publisher_id;
    uint8_t pmkid_one[1][ESP_WIFI_NAN_NDP_PMKID_LEN];
    memcpy(pmkid_one[0], ndl->security_ctx.nd_pmkid, ESP_WIFI_NAN_NDP_PMKID_LEN);

    NAN_DATA_UNLOCK();
    return nan_build_scia_attr(frm, pub_id,
                               (const uint8_t (*)[ESP_WIFI_NAN_NDP_PMKID_LEN])pmkid_one, 1);
}

uint32_t esp_nan_get_csia_len(uint16_t own_csid_bitmap, uint16_t peer_csid_bitmap)
{
    /* ID(1) + Len(2) + Capabilities(1) + 2 bytes per CSID entry. Empty bitmap
     * → 0 so the caller does not reserve a 4-byte hole the constructor refuses
     * to fill. Mask matches what construct_csia actually emits. */
    uint16_t csid_bitmap = peer_csid_bitmap ? (own_csid_bitmap & peer_csid_bitmap) : own_csid_bitmap;
    csid_bitmap &= NAN_CSID_VALID_BITMAP;
    if (!csid_bitmap) {
        return 0;
    }
    uint8_t num_csids = __builtin_popcount(csid_bitmap);
    uint32_t len = 3 + 1 + 2 * num_csids;
    ESP_LOGD(TAG, "GET CSIA LEN: %lu (own=0x%04x, peer=0x%04x, num_csids=%d)",
             len, own_csid_bitmap, peer_csid_bitmap, num_csids);
    return len;
}

uint32_t esp_nan_get_scia_len(uint8_t num_pmkids)
{
    /* ID(1) + Len(2) + 20 bytes per SCID entry. With zero PMKIDs the
     * constructor (nan_build_scia_attr) writes nothing, so report 0
     * here too — otherwise the caller reserves 3 header bytes that
     * never get filled and uninitialized memory ends up in the SDF. */
    if (!num_pmkids) {
        return 0;
    }
    uint32_t len = 3 + 20 * num_pmkids;
    ESP_LOGD(TAG, "GET SCIA LEN: %lu (num_pmkids=%d)", len, num_pmkids);
    return len;
}

uint32_t esp_nan_get_shared_key_desc_attr_len(uint16_t key_data_len)
{
    uint32_t body_len = 1 + NAN_KEY_DESC_MIN_LEN + key_data_len;
    uint32_t total = 3 + body_len;
    ESP_LOGD(TAG, "GET Shared Key Desc Attr LEN: %lu (key_data_len=%u)",
             (unsigned long)total, (unsigned int)key_data_len);
    return total;
}

int esp_nan_ndp_security_install_get_shared_desc_len(void)
{
    return (int)esp_nan_get_shared_key_desc_attr_len(0);
}

int esp_nan_get_ndp_resp_shared_key_desc(uint8_t *buf, size_t buf_len, uint8_t ndp_id, const uint8_t *peer_nmi)
{
    const uint32_t attr_len = esp_nan_get_shared_key_desc_attr_len(0);

    if (!buf || buf_len < attr_len || !peer_nmi) {
        return 0;
    }

    NAN_DATA_LOCK();
    struct ndl_info *ndl = nan_find_ndl(ndp_id, (uint8_t *)peer_nmi);
    if (!ndl) {
        NAN_DATA_UNLOCK();
        ESP_LOGW(TAG, "NDP Resp Key Desc: no NDL for ndp_id=%d", ndp_id);
        return 0;
    }
    if (ndl->handshake_state != NAN_HANDSHAKE_M1_RCVD) {
        NAN_DATA_UNLOCK();
        ESP_LOGW(TAG, "NDP Resp Key Desc: NDL not in M1_RCVD (state=%d)", ndl->handshake_state);
        return 0;
    }

    /* Resolve PMK from publish when: NDL not encrypted, or encrypted but nd_pmk not set */
    {
        static const uint8_t zero_pmkid[ESP_WIFI_NAN_NDP_PMKID_LEN] = {0};
        static const uint8_t zero_pmk[ESP_WIFI_NAN_NDP_PMK_LEN] = {0};
        bool have_peer_pmkid = (memcmp(ndl->security_ctx.nd_pmkid, zero_pmkid,
                                       ESP_WIFI_NAN_NDP_PMKID_LEN) != 0);
        bool need_pmk = (ndl->security_ctx.type != WIFI_NAN_SECURITY_ENCRYPTED) ||
                        (memcmp(ndl->security_ctx.nd_pmk, zero_pmk, ESP_WIFI_NAN_NDP_PMK_LEN) == 0);

        bool resolved_from_pairing_cache = false;
#if defined(CONFIG_ESP_WIFI_NAN_PAIRING)
        if (need_pmk) {
            resolved_from_pairing_cache = nan_security_fill_from_paired_cache(ndl, peer_nmi);
        }
#endif

        if (resolved_from_pairing_cache) {
            ESP_LOGD(TAG, "NDP Resp Key Desc: resolved PMK from paired-peer cache");
        } else if (need_pmk && have_peer_pmkid) {
            struct own_svc_info *p_svc = nan_find_own_svc(ndl->publisher_id);
            int matched_idx = p_svc ? nan_match_pmkid(p_svc, ndl->security_ctx.nd_pmkid,
                                                      ndl->peer_nmi, ndl->peer_ndi) : -1;
            if (matched_idx >= 0) {
                ndl->security_ctx.type = WIFI_NAN_SECURITY_ENCRYPTED;
                ndl->security_ctx.csid_bitmap = p_svc->derived_security[matched_idx].csid_bitmap;
                memcpy(ndl->security_ctx.nd_pmk,
                       p_svc->derived_security[matched_idx].nd_pmk,
                       ESP_WIFI_NAN_NDP_PMK_LEN);
                ESP_LOGD(TAG, "NDP Resp Key Desc: resolved PMK from cred slot %d", matched_idx);
            } else if (ndl->security_ctx.type != WIFI_NAN_SECURITY_ENCRYPTED) {
                NAN_DATA_UNLOCK();
                ESP_LOGW(TAG, "NDP Resp Key Desc: NDL not encrypted; send NDP Response without Shared Key Descriptor");
                return 0;
            } else {
                NAN_DATA_UNLOCK();
                ESP_LOGW(TAG, "NDP Resp Key Desc: no PMK for peer PMKID; ensure matching credential on both devices");
                return 0;
            }
        } else if (ndl->security_ctx.type != WIFI_NAN_SECURITY_ENCRYPTED) {
            NAN_DATA_UNLOCK();
            ESP_LOGW(TAG, "NDP Resp Key Desc: NDL not encrypted; send NDP Response without Shared Key Descriptor");
            return 0;
        }
    }

    /* Generate SNonce and derive PTK if not yet done */
    if (!ndl->ptk_set) {
        if (os_get_random(ndl->snonce, NAN_NONCE_LEN) != 0) {
            NAN_DATA_UNLOCK();
            ESP_LOGE(TAG, "NDP Resp Key Desc: SNonce random failed");
            return 0;
        }
        /* M2 echoes M1's replay counter unchanged (per RSNA 4-way handshake). */
        memcpy(ndl->tx_replay_counter, ndl->rx_replay_counter, NAN_REPLAY_COUNTER_LEN);

        uint8_t our_mac[6];
        if (esp_wifi_get_mac(WIFI_IF_NAN, our_mac) != ESP_OK) {
            NAN_DATA_UNLOCK();
            ESP_LOGE(TAG, "NDP Resp Key Desc: get our MAC failed");
            return 0;
        }
        if (nan_ndp_ptk_derive(ndl->security_ctx.nd_pmk,
                               ndl->peer_ndi,  /* IAddr = Initiator NDI (Wi-Fi Aware v4.0 §7.1.3.5) */
                               our_mac,        /* RAddr = Responder NDI */
                               ndl->anonce, ndl->snonce,
                               ndl->nd_kck, ndl->nd_kek, ndl->nd_tk) != 0) {
            NAN_DATA_UNLOCK();
            ESP_LOGE(TAG, "NDP Resp Key Desc: PTK derivation failed");
            return 0;
        }
        ndl->kck_len = NAN_NCS_SK_128_KCK_LEN;
        ndl->kek_len = NAN_NCS_SK_128_KEK_LEN;
        ndl->tk_len  = NAN_NCS_SK_128_TK_LEN;
        ndl->ptk_set = 1;
    }

    uint8_t *p = buf;
    *p++ = NAN_ATTR_ID_SHARED_KEY_DESC;
    {
        uint16_t body_len = 1 + NAN_KEY_DESC_MIN_LEN;
        *p++ = body_len & 0xFF;
        *p++ = (body_len >> 8) & 0xFF;
    }
    *p++ = ndl->publisher_id;

    int n = nan_build_rsna_key_descriptor(p, NAN_KEY_INFO_MIC,
                                          ndl->snonce,
                                          ndl->tx_replay_counter,
                                          ndl->key_rsc);
    if (n <= 0) {
        NAN_DATA_UNLOCK();
        return 0;
    }

    n = 3 + 1 + n;

    NAN_DATA_UNLOCK();
    ESP_LOGD(TAG, "NDP Resp Key Desc: built (MIC=0; driver must call esp_nan_update_ndp_resp_mic) for ndp_id=%d", ndp_id);
    return n;
}

int esp_nan_get_ndp_security_install_key_desc(uint8_t *buf, size_t buf_len, uint8_t ndp_id, const uint8_t *peer_nmi)
{
    const uint32_t attr_len = esp_nan_get_shared_key_desc_attr_len(0);

    if (!buf || buf_len < attr_len || !peer_nmi) {
        return 0;
    }

    NAN_DATA_LOCK();
    struct ndl_info *ndl = nan_find_ndl(ndp_id, (uint8_t *)peer_nmi);
    if (!ndl) {
        NAN_DATA_UNLOCK();
        ESP_LOGW(TAG, "NDP M4 Key Desc: no NDL for ndp_id=%d", ndp_id);
        return 0;
    }

    if (ndl->handshake_state != NAN_HANDSHAKE_M3_RCVD) {
        NAN_DATA_UNLOCK();
        ESP_LOGW(TAG, "NDP M4 Key Desc: NDL not in M3_RCVD (state=%d)", ndl->handshake_state);
        return 0;
    }

    if (!ndl->ptk_set) {
        NAN_DATA_UNLOCK();
        ESP_LOGW(TAG, "NDP M4 Key Desc: PTK not set for ndp_id=%d", ndp_id);
        return 0;
    }

    /* M4 echoes M3 replay counter unchanged */
    memcpy(ndl->tx_replay_counter, ndl->rx_replay_counter, NAN_REPLAY_COUNTER_LEN);

    uint8_t *p = buf;
    *p++ = NAN_ATTR_ID_SHARED_KEY_DESC;
    {
        uint16_t body_len = 1 + NAN_KEY_DESC_MIN_LEN;
        *p++ = body_len & 0xFF;
        *p++ = (body_len >> 8) & 0xFF;
    }
    *p++ = ndl->publisher_id;

    int n = nan_build_rsna_key_descriptor(p,
                                          NAN_KEY_INFO_MIC |
                                          NAN_KEY_INFO_SECURE |
                                          NAN_KEY_INFO_INSTALL,
                                          NULL, /* M4 nonce = 0 */
                                          ndl->tx_replay_counter,
                                          ndl->key_rsc);
    if (n <= 0) {
        NAN_DATA_UNLOCK();
        return 0;
    }

    n = 3 + 1 + n;

    NAN_DATA_UNLOCK();
    ESP_LOGD(TAG, "NDP M4 Key Desc: built for ndp_id=%d", ndp_id);
    return n;
}

int esp_nan_update_ndp_resp_mic(uint8_t *m2_body, size_t body_len, uint8_t *key_desc_attr,
                                uint8_t ndp_id, const uint8_t *peer_nmi)
{
    return nan_update_kd_mic(m2_body, body_len, key_desc_attr, ndp_id, peer_nmi, "M2", false);
}

int esp_nan_update_ndp_security_install_mic(uint8_t *m4_body, size_t body_len, uint8_t *key_desc_attr,
                                            uint8_t ndp_id, const uint8_t *peer_nmi)
{
    return nan_update_kd_mic(m4_body, body_len, key_desc_attr, ndp_id, peer_nmi, "M4", false);
}

/*-------------------------------------------------------------------------
 * Public API: parsers (CSIA / SCIA / Shared Key Descriptor / Publish SDF)
 *-----------------------------------------------------------------------*/

void esp_nan_parse_ndp_csia(void *frm, size_t buf_len, wifi_nan_security_params_t *param)
{
    if (!frm || !param || buf_len < 3) {
        return;
    }

    uint8_t *p = (uint8_t *)frm;
    if (p[0] != NAN_ATTR_ID_CIPHER_SUITE_INFO) {
        ESP_LOGW(TAG, "Invalid CSIA ID: 0x%02x (expected 0x%02x)", p[0], NAN_ATTR_ID_CIPHER_SUITE_INFO);
        return;
    }
    uint16_t len = p[1] | (p[2] << 8);
    if ((size_t)3 + len > buf_len) {
        ESP_LOGW(TAG, "CSIA: encoded len %u exceeds buf %zu", len, buf_len);
        return;
    }
    uint8_t *body = p + 3;

    /* body[0] = Capabilities; pairs of [CSID(1) PubID(1)] follow. Iterate all
     * entries so a peer advertising multiple cipher suites is fully captured. */
    if (len >= 3) {
        uint16_t accum = 0;
        uint8_t pub_id = 0;
        for (uint16_t off = 1; off + 2 <= len; off += 2) {
            uint8_t csid = body[off];
            if (csid >= WIFI_NAN_CSID_NCS_SK_128 && csid <= WIFI_NAN_CSID_NCS_PK_PASN_256) {
                accum |= (uint16_t)(1u << csid);
                pub_id = body[off + 1];
            }
        }
        if (accum) {
            param->csid_bitmap = accum;
            param->type = WIFI_NAN_SECURITY_ENCRYPTED;
            s_pending_scia.has_csid = true;
            s_pending_scia.pub_id = pub_id;
            s_pending_scia.csid_bitmap = accum;
        }
    }
}

void esp_nan_parse_ndp_scia(void *frm, size_t buf_len, wifi_nan_security_params_t *param)
{
    if (!frm || !param || buf_len < 3) {
        return;
    }

    uint8_t *p = (uint8_t *)frm;
    if (p[0] != NAN_ATTR_ID_SECURITY_CONTEXT) {
        ESP_LOGW(TAG, "Invalid SCIA ID: 0x%02x (expected 0x%02x)", p[0], NAN_ATTR_ID_SECURITY_CONTEXT);
        return;
    }
    uint16_t len = p[1] | (p[2] << 8);
    if ((size_t)3 + len > buf_len) {
        ESP_LOGW(TAG, "SCIA: encoded len %u exceeds buf %zu", len, buf_len);
        return;
    }
    uint8_t *body = p + 3;

    /* size_t offset prevents (4 + entry_len) wrapping to 0 on a peer-supplied
     * entry_len near 0xFFFF; per-entry layout is Len(2)||Type(1)||PubID(1)||Value. */
    size_t offset = 0;
    while (offset + 4 <= len) {
        uint16_t entry_len = body[offset] | (body[offset + 1] << 8);
        uint8_t type = body[offset + 2];
        uint8_t pub_id = body[offset + 3];
        uint8_t *val = &body[offset + 4];

        if (offset + 4 + entry_len > len) {
            break;
        }

        if (type == 1 && entry_len == ESP_WIFI_NAN_NDP_PMKID_LEN) {
            memcpy(param->nd_pmkid, val, ESP_WIFI_NAN_NDP_PMKID_LEN);

            s_pending_scia.has_pmkid = true;
            s_pending_scia.pub_id = pub_id;
            memcpy(s_pending_scia.pmkid, val, ESP_WIFI_NAN_NDP_PMKID_LEN);
            break;
        }
        offset += 4 + entry_len;
    }
}

void esp_nan_parse_ndp_key_desc(void *frm, size_t buf_len, uint8_t ndp_id, const uint8_t *peer_nmi)
{
    if (!frm || buf_len < 3) {
        return;
    }

    uint8_t *p = (uint8_t *)frm;
    if (p[0] != NAN_ATTR_ID_SHARED_KEY_DESC) {
        ESP_LOGW(TAG, "Invalid Key Desc ID: 0x%02x (expected 0x%02x)", p[0], NAN_ATTR_ID_SHARED_KEY_DESC);
        return;
    }
    uint16_t attr_len = p[1] | (p[2] << 8);
    if ((size_t)3 + attr_len > buf_len) {
        ESP_LOGW(TAG, "Key Desc: encoded len %u exceeds buf %zu", attr_len, buf_len);
        return;
    }
    uint8_t *body = p + 3;

    if (attr_len < 1) {
        return;
    }

    uint8_t pub_id = body[0];
    uint8_t *key_desc = body + 1;
    uint16_t key_desc_len = attr_len - 1;

    if (key_desc_len < NAN_KEY_DESC_MIN_LEN) {
        ESP_LOGW(TAG, "Key Descriptor too short (%d bytes)", key_desc_len);
        return;
    }

    /* Key Info is big-endian on the wire per IEEE 802.11-2020 §12.7.2. */
    uint16_t key_info = (key_desc[NAN_KEY_DESC_KEY_INFO_OFF] << 8) | key_desc[NAN_KEY_DESC_KEY_INFO_OFF + 1];
    uint8_t *replay_counter = &key_desc[NAN_KEY_DESC_REPLAY_OFF];
    uint8_t *key_nonce = &key_desc[NAN_KEY_DESC_NONCE_OFF];
    uint8_t *key_rsc = &key_desc[NAN_KEY_DESC_RSC_OFF];
    uint16_t key_data_len = (key_desc[NAN_KEY_DESC_DATA_LEN_OFF] << 8) | key_desc[NAN_KEY_DESC_DATA_LEN_OFF + 1];

    bool has_enc_key = (key_info & NAN_KEY_INFO_ENC_KEY) != 0;
    uint8_t msg_type = nan_keyinfo_to_msg_type(key_info);

    if ((key_info & NAN_KEY_INFO_INSTALL) && (key_info & NAN_KEY_INFO_ACK)) {
        ESP_LOGD(TAG, "NDP Key Desc: Install+ACK bits set (KeyInfo=0x%04x)", key_info);
    }

    switch (msg_type) {
    case 1:
        ESP_LOGD(TAG, "NDP Key Desc: M1 (NDP Request) - ANonce + replay counter");
        break;
    case 2:
        ESP_LOGD(TAG, "NDP Key Desc: M2 (NDP Response) - SNonce + replay counter");
        break;
    case 3:
        ESP_LOGD(TAG, "NDP Key Desc: M3 (NDP Confirm); key data len=%d", key_data_len);
        break;
    case 4:
        ESP_LOGD(TAG, "NDP Key Desc: M4 (Security Install); key data len=%d", key_data_len);
        break;
    default:
        ESP_LOGD(TAG, "NDP Key Desc: unknown message (KeyInfo=0x%04x)", key_info);
        break;
    }

    /*
     * Called from wifi task context; must not block on NAN_DATA_LOCK to avoid
     * deadlock (app may hold lock while waiting for wifi).
     */
    struct ndl_info *ndl = NULL;
    if (ndp_id != 0 || peer_nmi != NULL) {
        ndl = nan_find_ndl(ndp_id, (uint8_t *)peer_nmi);
    }
    if (ndl == NULL && peer_nmi != NULL) {
        ndl = nan_find_ndl_by_pub_id_and_peer(pub_id, peer_nmi);
    }
    if (ndl == NULL && peer_nmi != NULL) {
        ndl = nan_ndl_claim_for_pub_peer(pub_id, peer_nmi, ndp_id);
    }
    if (ndl == NULL) {
        ndl = nan_find_ndl_by_pub_id(pub_id);
    }

    if (ndl) {
        memcpy(ndl->rx_replay_counter, replay_counter, NAN_REPLAY_COUNTER_LEN);
        ndl->rx_replay_counter_set = 1;

        if (msg_type == 1) {
            memcpy(ndl->anonce, key_nonce, NAN_NONCE_LEN);
            ndl->handshake_state = NAN_HANDSHAKE_M1_RCVD;
            ESP_LOGD(TAG, "NDP Key Desc: stored ANonce + replay counter (M1)");
        } else if (msg_type == 2) {
            memcpy(ndl->snonce, key_nonce, NAN_NONCE_LEN);
            ndl->handshake_state = NAN_HANDSHAKE_M2_RCVD;
            ESP_LOGD(TAG, "NDP Key Desc: stored SNonce + replay counter (M2)");
        } else if (msg_type == 3) {
            /* Responder receives M3 — leave the MIC verification (which has
             * the Auth_Token||body scope) to esp_nan_verify_ndp_confirm_mic
             * before promoting to M3_RCVD. */
            ndl->handshake_state = NAN_HANDSHAKE_M3_PENDING_VERIFY;
            ESP_LOGD(TAG, "NDP Key Desc: handshake_state=M3_PENDING_VERIFY (responder)");
        } else if (msg_type == 4) {
            /* Initiator receives M4 — gate final COMPLETE on MIC verify in
             * esp_nan_verify_ndp_security_install_mic. */
            ndl->handshake_state = NAN_HANDSHAKE_M4_RCVD;
            ESP_LOGD(TAG, "NDP Key Desc: handshake_state=M4_RCVD (initiator)");
        }

        memcpy(ndl->key_rsc, key_rsc, NAN_KEY_RSC_LEN);

        /* Parse Key Data (KDEs) - only when not encrypted */
        if (key_data_len > 0 && (NAN_KEY_DESC_DATA_OFF + key_data_len <= key_desc_len) && !has_enc_key) {
            uint8_t *key_data = &key_desc[NAN_KEY_DESC_DATA_OFF];
            uint16_t kd_offset = 0;

            while (kd_offset + 2 <= key_data_len) {
                uint8_t kde_type = key_data[kd_offset];
                uint8_t kde_len = key_data[kd_offset + 1];

                if (kd_offset + 2 + kde_len > key_data_len) {
                    break;
                }

                if (kde_type == 0xDD && kde_len >= 4) {
                    uint32_t oui = (key_data[kd_offset + 2] << 16) | (key_data[kd_offset + 3] << 8) | key_data[kd_offset + 4];
                    uint8_t data_type = key_data[kd_offset + 5];
                    uint8_t *data = &key_data[kd_offset + 6];
                    uint8_t data_len = kde_len - 4;

                    if (oui == 0x000FAC) { /* WFA OUI */
                        if (data_type == 1 && data_len <= NAN_GTK_MAX_LEN) {
                            memcpy(ndl->gtk, data, data_len);
                            ndl->gtk_len = data_len;
                            ndl->gtk_set = 1;
                            ESP_LOGI(TAG, "KDE: GTK stored (len=%d)", data_len);
                        } else if (data_type == 3 && data_len >= 6) {
                            ESP_LOGI(TAG, "KDE: MAC Address: " MACSTR, MAC2STR(data));
                        } else if (data_type == 4 && data_len <= NAN_GTK_MAX_LEN) {
                            memcpy(ndl->igtk, data, data_len);
                            ndl->igtk_len = data_len;
                            ndl->igtk_set = 1;
                            ESP_LOGI(TAG, "KDE: IGTK stored (len=%d)", data_len);
                        } else if (data_type == 5 && data_len <= NAN_GTK_MAX_LEN) {
                            memcpy(ndl->bigtk, data, data_len);
                            ndl->bigtk_len = data_len;
                            ndl->bigtk_set = 1;
                            ESP_LOGI(TAG, "KDE: BIGTK stored (len=%d)", data_len);
                        }
                    } else if (oui == 0x506F9A) { /* NAN OUI */
                        if (data_type == 36 && data_len >= 1) {
                            ESP_LOGI(TAG, "KDE: NIK (Cipher Ver: %d)", data[0]);
                        } else if (data_type == 37 && data_len >= 6) {
                            uint16_t key_bitmap = data[0] | (data[1] << 8);
                            uint32_t lifetime = (data[2] << 24) | (data[3] << 16) | (data[4] << 8) | data[5];
                            ESP_LOGI(TAG, "KDE: NAN Key Lifetime: %lu s, Bitmap: 0x%04x",
                                     (unsigned long)lifetime, key_bitmap);
                        }
                    }
                }
                kd_offset += 2 + kde_len;
            }
        }
    } else if (msg_type == 1) {
        /* M1 received but NDL not created yet — store as pending */
        s_pending_m1.valid = true;
        s_pending_m1.pub_id = pub_id;
        memcpy(s_pending_m1.anonce, key_nonce, NAN_NONCE_LEN);
        memcpy(s_pending_m1.rx_replay_counter, replay_counter, NAN_REPLAY_COUNTER_LEN);
        memcpy(s_pending_m1.key_rsc, key_rsc, NAN_KEY_RSC_LEN);
        ESP_LOGI(TAG, "NDP Key Desc: M1 stored as pending for pub_id=%d", pub_id);
    } else {
        ESP_LOGI(TAG, "NDP Key Desc: no matching NDL (ndp_id=%d, pub_id=%d), key desc not applied",
                 ndp_id, pub_id);
    }
}

esp_err_t esp_nan_parse_publish_security(const uint8_t *attrs, size_t attrs_len,
                                         wifi_nan_peer_sdf_security_t *security)
{
    uint16_t csia_len = 0;
    const uint8_t *csia = NULL;
    uint16_t scia_len = 0;
    const uint8_t *scia = NULL;

    if (!attrs || !security) {
        return ESP_ERR_INVALID_ARG;
    }

    memset(security, 0, sizeof(*security));

    /* 1. CSIA — Cipher Suite IDs */
    csia = nan_find_attr(attrs, attrs_len, NAN_ATTR_ID_CIPHER_SUITE_INFO, &csia_len);
    if (csia && csia_len >= 1) {
        ESP_LOGD(TAG, "Found CSIA in Publish SDF, len=%d", csia_len);
        ESP_LOG_BUFFER_HEXDUMP(TAG, csia, csia_len, ESP_LOG_DEBUG);

        /* Skip Capabilities byte; iterate [CSID(1)] [Publish ID(1)] entries */
        size_t offset = 1;
        while (offset + 2 <= csia_len) {
            uint8_t csid = csia[offset];

            /* Spec Table 121: csid is 1..8. Skip out-of-range to avoid UB / truncation. */
            if (csid >= WIFI_NAN_CSID_NCS_SK_128 && csid <= WIFI_NAN_CSID_NCS_PK_PASN_256) {
                security->csid_bitmap |= (uint16_t)(1u << csid);
            }

            offset += 2;
        }
    }

    /* 2. SCIA — PMKIDs */
    scia = nan_find_attr(attrs, attrs_len, NAN_ATTR_ID_SECURITY_CONTEXT, &scia_len);
    if (scia && scia_len > 0) {
        ESP_LOGD(TAG, "Found SCIA in Publish SDF, len=%d", scia_len);

        size_t offset = 0;
        while (offset + 4 <= scia_len && security->num_pmkids < NAN_PEER_MAX_PMKIDS) {
            uint16_t val_len = scia[offset] | (scia[offset + 1] << 8);
            uint8_t type = scia[offset + 2];

            if (offset + 4 + val_len > scia_len) {
                break;
            }

            if (type == 1 && val_len == ESP_WIFI_NAN_NDP_PMKID_LEN) {
                memcpy(security->pmkids[security->num_pmkids],
                       &scia[offset + 4], ESP_WIFI_NAN_NDP_PMKID_LEN);
                ESP_LOGD(TAG, "  Parsed PMKID[%d]:", security->num_pmkids);
                ESP_LOG_BUFFER_HEXDUMP(TAG, security->pmkids[security->num_pmkids],
                                       ESP_WIFI_NAN_NDP_PMKID_LEN, ESP_LOG_DEBUG);
                security->num_pmkids++;
            }

            offset += 4 + val_len;
        }
    }

    if (security->csid_bitmap == 0 && security->num_pmkids == 0) {
        return ESP_ERR_NOT_FOUND;
    }

    return ESP_OK;
}

/*-------------------------------------------------------------------------
 * Cross-file accessor: drain pending CSIA/SCIA/M1 onto NDL.
 * Called from nan_app_ndp_indication_cb once peer info is known.
 *-----------------------------------------------------------------------*/

void nan_security_apply_pending(struct ndl_info *ndl,
                                struct own_svc_info *p_own_svc,
                                uint8_t pub_id,
                                const uint8_t *peer_nmi,
                                const uint8_t *peer_ndi)
{
    if (ndl && s_pending_scia.pub_id == pub_id &&
            (s_pending_scia.has_csid || s_pending_scia.has_pmkid)) {

        if (s_pending_scia.has_csid) {
            ndl->security_ctx.csid_bitmap = s_pending_scia.csid_bitmap;
            ndl->security_ctx.type = WIFI_NAN_SECURITY_ENCRYPTED;
        }

        if (s_pending_scia.has_pmkid) {
            memcpy(ndl->security_ctx.nd_pmkid, s_pending_scia.pmkid, ESP_WIFI_NAN_NDP_PMKID_LEN);
            int matched_idx = nan_match_pmkid(p_own_svc, s_pending_scia.pmkid, peer_nmi, peer_ndi);
            if (matched_idx >= 0) {
                ndl->security_ctx.type = WIFI_NAN_SECURITY_ENCRYPTED;
                ndl->security_ctx.csid_bitmap = p_own_svc->derived_security[matched_idx].csid_bitmap;
                memcpy(ndl->security_ctx.nd_pmk,
                       p_own_svc->derived_security[matched_idx].nd_pmk,
                       ESP_WIFI_NAN_NDP_PMK_LEN);
                ESP_LOGD(TAG, "NDP Indication: PMKID validated via cred slot %d", matched_idx);
            }
#if defined(CONFIG_ESP_WIFI_NAN_PAIRING)
            else if (nan_security_fill_from_paired_cache(ndl, peer_nmi)) {
                ESP_LOGD(TAG, "NDP Indication: PMK sourced from paired-peer cache");
            }
#endif
            else {
                ESP_LOGW(TAG, "NDP Indication: PMKID validation failed");
            }
        }

        memset(&s_pending_scia, 0, sizeof(s_pending_scia));
    }

    if (s_pending_m1.valid && s_pending_m1.pub_id == pub_id && ndl) {
        memcpy(ndl->anonce, s_pending_m1.anonce, NAN_NONCE_LEN);
        memcpy(ndl->rx_replay_counter, s_pending_m1.rx_replay_counter, NAN_REPLAY_COUNTER_LEN);
        ndl->rx_replay_counter_set = 1;
        memcpy(ndl->key_rsc, s_pending_m1.key_rsc, NAN_KEY_RSC_LEN);
        ndl->handshake_state = NAN_HANDSHAKE_M1_RCVD;
        ESP_LOGD(TAG, "NDP Indication: applied pending M1 to NDL");
        s_pending_m1.valid = false;
    }
}

/* Replay counter helper (used by M3 initiator builder). */
static void nan_replay_counter_increment(uint8_t *counter)
{
    int i;
    for (i = NAN_REPLAY_COUNTER_LEN - 1; i >= 0; i--) {
        counter[i]++;
        if (counter[i] != 0) {
            break;
        }
    }
}

/*
 * ---------------------------------------------------------------------------
 * Initiator-side NDP security (M1 TX, M2 verify, M3 TX, M4 verify) and
 * responder-side M3 MIC verification. These complete the NCS-SK 4-way
 * handshake by adding the symmetric counterparts to the responder M2/M4
 * builders above.
 * ---------------------------------------------------------------------------
 */

/**
 * @brief Verify MIC in Shared Key Descriptor of an inbound NDP message.
 *
 * Computes HMAC-SHA256(KCK, [prefix ||] body_with_MIC_zeroed) and compares
 * the first 16 bytes against the saved MIC. Restores the MIC field on
 * return so the caller buffer is left intact.
 *
 * For M2/M4 the scope is body only — pass prefix=NULL. For M3 (responder
 * side) the scope is Auth_Token || body per Wi-Fi Aware v4.0 §7.1.3.5;
 * pass prefix=Auth_Token, prefix_len=Auth_Token_len.
 *
 * @return 0 on MIC pass, -1 on parameter error or MIC mismatch.
 */
static int nan_verify_kck_mic(uint8_t *body, size_t body_len,
                              uint8_t *key_desc_attr,
                              const uint8_t *kck, size_t kck_len,
                              const uint8_t *prefix, size_t prefix_len,
                              const char *tag_label)
{
    if (!body || !key_desc_attr || body_len == 0 || !kck) {
        ESP_LOGE(TAG, "%s Verify MIC: invalid parameters", tag_label);
        return -1;
    }
    if (!g_wifi_default_wpa_crypto_funcs.hmac_sha256_vector) {
        ESP_LOGE(TAG, "%s Verify MIC: hmac_sha256_vector not registered", tag_label);
        return -1;
    }
    if (key_desc_attr < body || key_desc_attr >= (body + body_len)) {
        ESP_LOGE(TAG, "%s Verify MIC: key_desc_attr outside body", tag_label);
        return -1;
    }
    uint8_t *key_desc = key_desc_attr + 3 + 1; /* skip attr hdr (3) + pub_id (1) */
    if ((key_desc + NAN_KEY_DESC_MIC_OFF + NAN_NCS_SK_128_MIC_LEN) > (body + body_len)) {
        ESP_LOGE(TAG, "%s Verify MIC: MIC field out of bounds", tag_label);
        return -1;
    }

    uint8_t saved_mic[NAN_NCS_SK_128_MIC_LEN];
    memcpy(saved_mic, &key_desc[NAN_KEY_DESC_MIC_OFF], NAN_NCS_SK_128_MIC_LEN);
    memset(&key_desc[NAN_KEY_DESC_MIC_OFF], 0, NAN_NCS_SK_128_MIC_LEN);

    uint8_t mic_buf[32];
    const unsigned char *addr[2];
    int len_arr[2];
    int n_vec;
    if (prefix && prefix_len > 0) {
        addr[0] = prefix;
        len_arr[0] = (int)prefix_len;
        addr[1] = body;
        len_arr[1] = (int)body_len;
        n_vec = 2;
    } else {
        addr[0] = body;
        len_arr[0] = (int)body_len;
        n_vec = 1;
    }
    int rc = g_wifi_default_wpa_crypto_funcs.hmac_sha256_vector(kck, (int)kck_len,
                                                                n_vec, addr, len_arr, mic_buf);
    memcpy(&key_desc[NAN_KEY_DESC_MIC_OFF], saved_mic, NAN_NCS_SK_128_MIC_LEN);

    if (rc != 0) {
        ESP_LOGE(TAG, "%s Verify MIC: HMAC-SHA256 failed", tag_label);
        return -1;
    }
    if (memcmp(mic_buf, saved_mic, NAN_NCS_SK_128_MIC_LEN) != 0) {
        ESP_LOGE(TAG, "%s Verify MIC: mismatch", tag_label);
        ESP_LOGD(TAG, "%s expected MIC:", tag_label);
        ESP_LOG_BUFFER_HEXDUMP(TAG, mic_buf, NAN_NCS_SK_128_MIC_LEN, ESP_LOG_DEBUG);
        ESP_LOGD(TAG, "%s received MIC:", tag_label);
        ESP_LOG_BUFFER_HEXDUMP(TAG, saved_mic, NAN_NCS_SK_128_MIC_LEN, ESP_LOG_DEBUG);
        return -1;
    }
    ESP_LOGD(TAG, "%s Verify MIC: pass", tag_label);
    return 0;
}

/**
 * @brief Construct SCIA for outbound NDP Request (M1) — initiator TX path.
 *
 * Identical wire format to esp_nan_construct_scia_ndp_resp(): the SCIA body
 * carries the pair-specific PMKID stored in ndl->security_ctx.nd_pmkid. The
 * field is pre-populated by the host before M1 TX; both initiator and
 * responder paths converge on the same NDL field, so we delegate.
 */
int esp_nan_construct_scia_ndp_req(uint8_t *frm, uint8_t ndp_id, const uint8_t *peer_nmi)
{
    ESP_LOGD(TAG, "Constructing SCIA (NDP Request M1) -> delegating to scia_ndp_resp body");
    return esp_nan_construct_scia_ndp_resp(frm, ndp_id, peer_nmi);
}

/**
 * @brief Build Shared Key Descriptor attribute for outbound NDP Request (M1).
 *
 * Initiator-side analogue of esp_nan_get_ndp_resp_shared_key_desc(). Looks
 * up the pre-claimed initiator NDL by ndp_id/peer (created in
 * esp_wifi_nan_datapath_req() with own_role=INITIATOR + security_ctx.nd_pmk
 * populated). Generates a random ANonce, sets the TX replay counter to all
 * zeros (per RSNA convention for initial M1), zeroes the Key RSC, and emits
 * the attribute header + body.
 *
 * No MIC is set — M1 is unauthenticated. After the blob assembles the full
 * M1 body, it must call esp_nan_capture_m1_auth_token() so the host can
 * store Auth_Token = SHA256(M1_body)[0:16] for use during M3 MIC.
 */
int esp_nan_get_ndp_req_shared_key_desc(uint8_t *buf, size_t buf_len, uint8_t ndp_id, const uint8_t *peer_nmi)
{
    const uint32_t attr_len = esp_nan_get_shared_key_desc_attr_len(0);

    if (!buf || buf_len < attr_len || !peer_nmi) {
        return 0;
    }

    NAN_DATA_LOCK();
    struct ndl_info *ndl = nan_find_ndl(ndp_id, (uint8_t *)peer_nmi);
    if (!ndl) {
        /* Pre-claimed initiator NDL may have ndp_id=0 (assigned by blob during
         * this call). Fall back to peer-only lookup; capture the real ndp_id
         * once we have the NDL so subsequent ndp_id-keyed lookups (parser,
         * MIC verifier) find it. */
        ndl = nan_find_ndl(0, (uint8_t *)peer_nmi);
        if (ndl && ndl->ndp_id == 0 && ndp_id != 0) {
            ndl->ndp_id = ndp_id;
            ESP_LOGD(TAG, "NDP Req Key Desc: assigned ndp_id=%d to pre-claimed NDL for peer="MACSTR,
                     ndp_id, MAC2STR(peer_nmi));
        }
    }
    if (!ndl) {
        NAN_DATA_UNLOCK();
        ESP_LOGW(TAG, "NDP Req Key Desc: no NDL for ndp_id=%d peer="MACSTR, ndp_id, MAC2STR(peer_nmi));
        return 0;
    }
    if (ndl->own_role != ESP_WIFI_NDP_ROLE_INITIATOR) {
        NAN_DATA_UNLOCK();
        ESP_LOGW(TAG, "NDP Req Key Desc: NDL not initiator role (role=%d)", ndl->own_role);
        return 0;
    }
    if (ndl->handshake_state != NAN_HANDSHAKE_IDLE) {
        NAN_DATA_UNLOCK();
        ESP_LOGW(TAG, "NDP Req Key Desc: NDL not in IDLE (state=%d)", ndl->handshake_state);
        return 0;
    }
    if (ndl->security_ctx.type != WIFI_NAN_SECURITY_ENCRYPTED) {
        NAN_DATA_UNLOCK();
        ESP_LOGW(TAG, "NDP Req Key Desc: NDL not configured for encrypted datapath");
        return 0;
    }

    if (os_get_random(ndl->anonce, NAN_NONCE_LEN) != 0) {
        NAN_DATA_UNLOCK();
        ESP_LOGE(TAG, "NDP Req Key Desc: ANonce random failed");
        return 0;
    }
    memset(ndl->tx_replay_counter, 0, NAN_REPLAY_COUNTER_LEN);
    memset(ndl->key_rsc, 0, NAN_KEY_RSC_LEN);
    /* Auth token must be re-captured by esp_nan_capture_m1_auth_token() after assembly */
    ndl->auth_token_len = 0;

    uint8_t *p = buf;
    *p++ = NAN_ATTR_ID_SHARED_KEY_DESC;
    {
        uint16_t body_len = 1 + NAN_KEY_DESC_MIN_LEN;
        *p++ = body_len & 0xFF;
        *p++ = (body_len >> 8) & 0xFF;
    }
    *p++ = ndl->publisher_id;

    int n = nan_build_rsna_key_descriptor(p, NAN_KEY_INFO_ACK,
                                          ndl->anonce,
                                          ndl->tx_replay_counter,
                                          ndl->key_rsc);
    if (n <= 0) {
        NAN_DATA_UNLOCK();
        return 0;
    }

    n = 3 + 1 + n;

    NAN_DATA_UNLOCK();
    /* State transition to M1_SENT happens in the host TX-confirm hook */
    ESP_LOGD(TAG, "NDP Req Key Desc: built (M1, no MIC) for ndp_id=%d", ndp_id);
    return n;
}

/**
 * @brief Capture Auth_Token = SHA256(M1_body)[0:16] for use during M3 MIC.
 *
 * Per Wi-Fi Aware v4.0 §7.1.3.5 (M3 construction), the M3 MIC scope is
 * Auth_Token || Body_of_M3,
 * where Auth_Token is computed over the entire as-transmitted M1 body
 * (CSIA, SCIA, Shared-Key Descriptor, NDPE attributes — everything the
 * blob assembled). Driver must call after assembly, before TX.
 */
int esp_nan_capture_m1_auth_token(const uint8_t *m1_body, size_t body_len,
                                  uint8_t ndp_id, const uint8_t *peer_nmi)
{
    if (!m1_body || body_len == 0 || !peer_nmi) {
        ESP_LOGE(TAG, "Auth_Token capture: invalid parameters");
        return -1;
    }
    if (!g_wifi_default_wpa_crypto_funcs.sha256_vector) {
        ESP_LOGE(TAG, "Auth_Token capture: sha256_vector not registered");
        return -1;
    }

    uint8_t hash[32];
    const uint8_t *addr[1] = { m1_body };
    size_t len[1] = { body_len };
    if (g_wifi_default_wpa_crypto_funcs.sha256_vector(1, addr, len, hash) != 0) {
        ESP_LOGE(TAG, "Auth_Token capture: SHA-256 failed");
        return -1;
    }

    NAN_DATA_LOCK();
    struct ndl_info *ndl = nan_find_ndl(ndp_id, (uint8_t *)peer_nmi);
    if (!ndl) {
        NAN_DATA_UNLOCK();
        ESP_LOGE(TAG, "Auth_Token capture: no NDL for ndp_id=%d", ndp_id);
        return -1;
    }
    /* NCS-SK-128: Auth_Token length is 16 bytes (NCS-SK-HASH = SHA-256, L() takes 128 bits). */
    memcpy(ndl->auth_token, hash, NAN_NCS_SK_128_MIC_LEN);
    ndl->auth_token_len = NAN_NCS_SK_128_MIC_LEN;
    ESP_LOGD(TAG, "Auth_Token captured (16 bytes) for ndp_id=%d, body_len=%u", ndp_id, (unsigned)body_len);
    ESP_LOG_BUFFER_HEXDUMP(TAG, ndl->auth_token, NAN_NCS_SK_128_MIC_LEN, ESP_LOG_DEBUG);
    NAN_DATA_UNLOCK();
    return 0;
}

/**
 * @brief Build Shared Key Descriptor attribute for outbound NDP Confirm (M3).
 *
 * Initiator-side. Looks up NDL (must be in M2_RCVD with stored ANonce + SNonce
 * + nd_pmk). Derives PTK if not yet done, increments the TX replay counter,
 * and emits the attribute header + body with MIC=0.
 *
 * Caller must subsequently call esp_nan_update_ndp_confirm_mic() to
 * compute MIC = HMAC-SHA256(KCK, Auth_Token || M3_body).
 */
int esp_nan_get_ndp_confirm_shared_key_desc(uint8_t *buf, size_t buf_len, uint8_t ndp_id, const uint8_t *peer_nmi)
{
    const uint32_t attr_len = esp_nan_get_shared_key_desc_attr_len(0);

    if (!buf || buf_len < attr_len || !peer_nmi) {
        return 0;
    }

    NAN_DATA_LOCK();
    struct ndl_info *ndl = nan_find_ndl(ndp_id, (uint8_t *)peer_nmi);
    if (!ndl) {
        NAN_DATA_UNLOCK();
        ESP_LOGW(TAG, "NDP M3 Key Desc: no NDL for ndp_id=%d", ndp_id);
        return 0;
    }
    if (ndl->own_role != ESP_WIFI_NDP_ROLE_INITIATOR) {
        NAN_DATA_UNLOCK();
        ESP_LOGW(TAG, "NDP M3 Key Desc: NDL not initiator role (role=%d)", ndl->own_role);
        return 0;
    }
    if (ndl->handshake_state != NAN_HANDSHAKE_M2_RCVD) {
        NAN_DATA_UNLOCK();
        ESP_LOGW(TAG, "NDP M3 Key Desc: NDL not in M2_RCVD (state=%d)", ndl->handshake_state);
        return 0;
    }
    if (ndl->security_ctx.type != WIFI_NAN_SECURITY_ENCRYPTED) {
        NAN_DATA_UNLOCK();
        ESP_LOGW(TAG, "NDP M3 Key Desc: NDL not encrypted");
        return 0;
    }

    if (ndl_ensure_ptk(ndl) != 0) {
        NAN_DATA_UNLOCK();
        return 0;
    }

    /* Initiator increments its own TX replay counter for M3 (M1 was zero). */
    nan_replay_counter_increment(ndl->tx_replay_counter);

    uint8_t *p = buf;
    *p++ = NAN_ATTR_ID_SHARED_KEY_DESC;
    {
        uint16_t body_len = 1 + NAN_KEY_DESC_MIN_LEN;
        *p++ = body_len & 0xFF;
        *p++ = (body_len >> 8) & 0xFF;
    }
    *p++ = ndl->publisher_id;

    int n = nan_build_rsna_key_descriptor(p,
                                          NAN_KEY_INFO_MIC |
                                          NAN_KEY_INFO_SECURE |
                                          NAN_KEY_INFO_ACK,
                                          NULL, /* M3 nonce = 0 */
                                          ndl->tx_replay_counter,
                                          ndl->key_rsc);
    if (n <= 0) {
        NAN_DATA_UNLOCK();
        return 0;
    }

    n = 3 + 1 + n;

    NAN_DATA_UNLOCK();
    /* State transition to M3_SENT happens in host TX-confirm hook */
    ESP_LOGD(TAG, "NDP M3 Key Desc: built (MIC=0, driver must call esp_nan_update_ndp_confirm_mic) for ndp_id=%d", ndp_id);
    return n;
}

/**
 * @brief Verify M2 MIC on the initiator side after the parser has stored SNonce.
 *
 * Derives PTK if not yet done (using stored ANonce + just-parsed SNonce, with
 * i_addr=our_NMI for initiator), then verifies HMAC-SHA256(KCK, M2_body) against
 * the MIC field in the descriptor.
 */
int esp_nan_verify_ndp_resp_mic(uint8_t *m2_body, size_t body_len, uint8_t *key_desc_attr,
                                uint8_t ndp_id, const uint8_t *peer_nmi)
{
    if (!m2_body || !key_desc_attr || !peer_nmi || body_len == 0) {
        ESP_LOGE(TAG, "NDP M2 Verify MIC: invalid parameters");
        return -1;
    }

    NAN_DATA_LOCK();
    struct ndl_info *ndl = nan_find_ndl(ndp_id, (uint8_t *)peer_nmi);
    if (!ndl) {
        NAN_DATA_UNLOCK();
        ESP_LOGE(TAG, "NDP M2 Verify MIC: no NDL for ndp_id=%d", ndp_id);
        return -1;
    }
    if (ndl->own_role != ESP_WIFI_NDP_ROLE_INITIATOR) {
        NAN_DATA_UNLOCK();
        ESP_LOGE(TAG, "NDP M2 Verify MIC: NDL not initiator role");
        return -1;
    }
    if (ndl->handshake_state != NAN_HANDSHAKE_M2_RCVD) {
        NAN_DATA_UNLOCK();
        ESP_LOGE(TAG, "NDP M2 Verify MIC: NDL not in M2_RCVD (state=%d)", ndl->handshake_state);
        return -1;
    }

    if (ndl_ensure_ptk(ndl) != 0) {
        NAN_DATA_UNLOCK();
        return -1;
    }

    int rc = nan_verify_kck_mic(m2_body, body_len, key_desc_attr,
                                ndl->nd_kck, NAN_NCS_SK_128_KCK_LEN,
                                NULL, 0, "NDP M2");
    NAN_DATA_UNLOCK();
    return rc;
}

/**
 * @brief Compute MIC for outbound NDP Confirm (M3) — initiator TX path.
 *
 * Per Wi-Fi Aware v4.0 §7.1.3.5, M3 MIC scope is `Auth_Token || Body_of_M3` (not body
 * alone). Auth_Token must have been captured during M1 TX via
 * esp_nan_capture_m1_auth_token(). Computes
 *   HMAC-SHA256(KCK, Auth_Token || M3_body_with_MIC_zeroed)
 * and writes the first 16 bytes into the MIC field of the descriptor.
 */
int esp_nan_update_ndp_confirm_mic(uint8_t *m3_body, size_t body_len, uint8_t *key_desc_attr,
                                   uint8_t ndp_id, const uint8_t *peer_nmi)
{
    return nan_update_kd_mic(m3_body, body_len, key_desc_attr, ndp_id, peer_nmi, "M3", true);
}

/**
 * @brief Verify M3 MIC on the responder side. Promotes handshake_state from
 * M3_PENDING_VERIFY to M3_RCVD on a passing HMAC-SHA-256(KCK,
 * Auth_Token || M3_body). On -1, abort the M4 TX path.
 *
 * Auth_Token is the SHA-256 of the M1 body the responder received,
 * stored via esp_nan_capture_m1_auth_token() during M1 RX.
 */
int esp_nan_verify_ndp_confirm_mic(uint8_t *m3_body, size_t body_len, uint8_t *key_desc_attr,
                                   uint8_t ndp_id, const uint8_t *peer_nmi)
{
    if (!m3_body || !key_desc_attr || !peer_nmi || body_len == 0) {
        ESP_LOGE(TAG, "NDP M3 Verify MIC: invalid parameters");
        return -1;
    }
    if (key_desc_attr < m3_body || key_desc_attr >= (m3_body + body_len)) {
        ESP_LOGE(TAG, "NDP M3 Verify MIC: key_desc_attr outside m3_body");
        return -1;
    }

    NAN_DATA_LOCK();
    struct ndl_info *ndl = nan_find_ndl(ndp_id, (uint8_t *)peer_nmi);
    if (!ndl) {
        NAN_DATA_UNLOCK();
        ESP_LOGE(TAG, "NDP M3 Verify MIC: no NDL for ndp_id=%d", ndp_id);
        return -1;
    }
    if (ndl->own_role == ESP_WIFI_NDP_ROLE_INITIATOR) {
        NAN_DATA_UNLOCK();
        ESP_LOGE(TAG, "NDP M3 Verify MIC: NDL is initiator role; M3 verify is responder-only");
        return -1;
    }
    if (!ndl->ptk_set) {
        NAN_DATA_UNLOCK();
        ESP_LOGE(TAG, "NDP M3 Verify MIC: PTK not set for ndp_id=%d", ndp_id);
        return -1;
    }
    if (ndl->auth_token_len == 0) {
        NAN_DATA_UNLOCK();
        ESP_LOGE(TAG, "NDP M3 Verify MIC: Auth_Token not captured (call esp_nan_capture_m1_auth_token from M1 RX path)");
        return -1;
    }
    if (ndl->handshake_state != NAN_HANDSHAKE_M3_PENDING_VERIFY) {
        NAN_DATA_UNLOCK();
        ESP_LOGE(TAG, "NDP M3 Verify MIC: NDL not in M3_PENDING_VERIFY (state=%d)", ndl->handshake_state);
        return -1;
    }

    int rc = nan_verify_kck_mic(m3_body, body_len, key_desc_attr,
                                ndl->nd_kck, NAN_NCS_SK_128_KCK_LEN,
                                ndl->auth_token, ndl->auth_token_len,
                                "NDP M3");
    if (rc == 0) {
        ndl->handshake_state = NAN_HANDSHAKE_M3_RCVD;
        ESP_LOGD(TAG, "NDP M3 Verify MIC: pass; handshake_state=M3_RCVD (ready to TX M4)");
    }
    NAN_DATA_UNLOCK();
    return rc;
}

/**
 * @brief Verify M4 MIC on the initiator side. Promotes handshake_state from
 * M4_RCVD to COMPLETE on a passing HMAC-SHA256(KCK, M4_body). On -1, tear
 * the NDP down rather than installing the TK.
 */
int esp_nan_verify_ndp_security_install_mic(uint8_t *m4_body, size_t body_len, uint8_t *key_desc_attr,
                                            uint8_t ndp_id, const uint8_t *peer_nmi)
{
    if (!m4_body || !key_desc_attr || !peer_nmi || body_len == 0) {
        ESP_LOGE(TAG, "NDP M4 Verify MIC: invalid parameters");
        return -1;
    }

    NAN_DATA_LOCK();
    struct ndl_info *ndl = nan_find_ndl(ndp_id, (uint8_t *)peer_nmi);
    if (!ndl) {
        NAN_DATA_UNLOCK();
        ESP_LOGE(TAG, "NDP M4 Verify MIC: no NDL for ndp_id=%d", ndp_id);
        return -1;
    }
    if (ndl->own_role != ESP_WIFI_NDP_ROLE_INITIATOR) {
        NAN_DATA_UNLOCK();
        ESP_LOGE(TAG, "NDP M4 Verify MIC: NDL not initiator role");
        return -1;
    }
    if (!ndl->ptk_set) {
        NAN_DATA_UNLOCK();
        ESP_LOGE(TAG, "NDP M4 Verify MIC: PTK not set for ndp_id=%d", ndp_id);
        return -1;
    }
    if (ndl->handshake_state != NAN_HANDSHAKE_M4_RCVD) {
        NAN_DATA_UNLOCK();
        ESP_LOGE(TAG, "NDP M4 Verify MIC: NDL not in M4_RCVD (state=%d)", ndl->handshake_state);
        return -1;
    }

    int rc = nan_verify_kck_mic(m4_body, body_len, key_desc_attr,
                                ndl->nd_kck, NAN_NCS_SK_128_KCK_LEN,
                                NULL, 0, "NDP M4");
    if (rc == 0) {
        ndl->handshake_state = NAN_HANDSHAKE_COMPLETE;
        ESP_LOGD(TAG, "NDP M4 Verify MIC: handshake_state=COMPLETE (initiator)");
    }
    NAN_DATA_UNLOCK();
    return rc;
}

#endif /* CONFIG_ESP_WIFI_NAN_SECURITY */
