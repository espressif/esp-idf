/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * NAN PASN: PASN pairing and authentication for Wi-Fi NAN.
 */

#include "sdkconfig.h"
#include "esp_private/esp_supp_nan.h"

#if CONFIG_ESP_WIFI_NAN_PAIRING

#include "esp_nan_supp_i.h"

#include "utils/includes.h"
#include "utils/common.h"
#include "common/ieee802_11_defs.h"
#include "common/nan.h"
#include "esp_wifi_driver.h"
#include "crypto/crypto.h"
#include "crypto/aes_wrap.h"
#include "crypto/sha256.h"
#include "crypto/sha384.h"

#include "pasn/pasn_common.h"
#include "common/wpa_common.h"
#include "apps_private/wifi_apps_private.h"

#include "includes.h"
#include "wpa_supplicant_i.h"
#include "utils/eloop.h"
#include "esp_err.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_private/wifi.h"

#define IEEE80211_MGMT_HDRLEN 24

/* Default NAN pairing PIN used when caller does not configure one. */
#define NAN_DEFAULT_PAIRING_PIN "000000"
#define NAN_PASN_AUTH_TIMEOUT_SECS 10

/* Match nan_app pairwise key install for NAN data path (esp_wifi_set_nan_key_internal). */
#define NAN_PASN_WIFI_ALG_CCMP    3
#define NAN_PASN_KEY_FLAG_RX      BIT(2)
#define NAN_PASN_KEY_FLAG_TX      BIT(3)
#define NAN_PASN_KEY_FLAG_PAIRWISE BIT(5)

static struct nan_pasn_key_material g_nan_pasn_saved_keys;

/* Key index for esp_wifi_set_nan_key_internal (NAN PASN pairwise TK). */
int temp = 1;

#define NAN_PASN_AES_WRAP_OVERHEAD      8
#define NAN_PASN_AES_WRAP_MIN_CIPHERTEXT (NAN_PASN_AES_WRAP_OVERHEAD + 8)

/**
 * Map PASN pairwise cipher to the NCS-SK CSID used for paired-peer NDPs
 * (Wi-Fi Aware v4.0 §7.6.4.2: paired NDP runs NCS-SK style M1-M4, key length
 * inherits from the PASN cipher).
 *
 * Returns 0 when the PASN cipher has no SK equivalent.
 */
static uint8_t nan_pasn_pasn_cipher_to_ndp_csid(int pasn_cipher)
{
    switch (pasn_cipher) {
    case WPA_CIPHER_CCMP:       return WIFI_NAN_CSID_NCS_SK_128;
    case WPA_CIPHER_GCMP_256:   return WIFI_NAN_CSID_NCS_SK_256;
    default:                    return 0;
    }
}

/**
 * Key lengths for NCS-PK-PASN-128 / NCS-PK-PASN-256 (Wi-Fi Aware pairing).
 * Mirrors hostap @c nan_crypto_cipher_*_len in src/nan/nan_crypto.c.
 */
static size_t __attribute__((unused)) nan_pasn_cipher_kck_len(int cipher)
{
    switch (cipher) {
    case WPA_CIPHER_CCMP:
        return 16;
    case WPA_CIPHER_GCMP_256:
        return 24;
    default:
        return 0;
    }
}

static size_t nan_pasn_cipher_kek_len(int cipher)
{
    switch (cipher) {
    case WPA_CIPHER_CCMP:
        return 16;
    case WPA_CIPHER_GCMP_256:
        return 32;
    default:
        return 0;
    }
}

static size_t nan_pasn_cipher_tk_len(int cipher)
{
    switch (cipher) {
    case WPA_CIPHER_CCMP:
        return 16;
    case WPA_CIPHER_GCMP_256:
        return 32;
    default:
        return 0;
    }
}

static size_t nan_pasn_cipher_mic_len(int cipher)
{
    switch (cipher) {
    case WPA_CIPHER_CCMP:
        return 16;
    case WPA_CIPHER_GCMP_256:
        return 24;
    default:
        return 0;
    }
}

static size_t nan_pasn_cipher_eapol_key_hdrlen(int cipher)
{
    size_t mic_len = nan_pasn_cipher_mic_len(cipher);

    if (mic_len == 24) {
        return sizeof(struct wpa_eapol_key_192);
    }
    if (mic_len == 16) {
        return sizeof(struct wpa_eapol_key);
    }
    return 0;
}

static bool nan_pasn_cipher_is_supported(int cipher)
{
    return cipher == WPA_CIPHER_CCMP || cipher == WPA_CIPHER_GCMP_256;
}

/**
 * nan_crypto_derive_from_kdk - Derive a key from KDK using KDF-HASH-NNN
 *
 * KEY = KDF-HASH-NNN(KDK, label, Pairing Initiator NMI || Pairing Responder NMI)
 *
 * Mirrors hostap @c nan_crypto_derive_from_kdk (src/nan/nan_crypto.c).
 * @a cipher is @c WPA_CIPHER_CCMP (NCS-PK-PASN-128) or @c WPA_CIPHER_GCMP_256
 * (NCS-PK-PASN-256).
 */
static int nan_crypto_derive_from_kdk(const u8 *kdk, size_t kdk_len, int cipher,
                                      const char *label,
                                      const u8 *initiator_nmi,
                                      const u8 *responder_nmi,
                                      u8 *key, size_t key_len)
{
    u8 data[ETH_ALEN * 2];
    int ret;

    if (!kdk || !kdk_len || !label || !initiator_nmi || !responder_nmi ||
            !key || !key_len) {
        wpa_printf(MSG_INFO,
                   "NAN: Invalid parameters for NPK/KEK derivation");
        return -1;
    }

    os_memcpy(data, initiator_nmi, ETH_ALEN);
    os_memcpy(data + ETH_ALEN, responder_nmi, ETH_ALEN);

    if (cipher == WPA_CIPHER_CCMP) {
        ret = sha256_prf(kdk, kdk_len, label, data, sizeof(data), key, key_len);
    } else if (cipher == WPA_CIPHER_GCMP_256) {
        ret = sha384_prf(kdk, kdk_len, label, data, sizeof(data), key, key_len);
    } else {
        wpa_printf(MSG_INFO,
                   "NAN: Unsupported cipher suite for key derivation: %d",
                   cipher);
        return -1;
    }

    if (ret) {
        wpa_printf(MSG_INFO,
                   "NAN: NPK/KEK derivation failed (ret=%d)", ret);
        return ret;
    }

    wpa_hexdump_key(MSG_DEBUG, "NAN: KDK", kdk, kdk_len);
    wpa_printf(MSG_DEBUG, "NAN: Label: %s", label);
    wpa_printf(MSG_DEBUG, "NAN: Initiator NMI " MACSTR,
               MAC2STR(initiator_nmi));
    wpa_printf(MSG_DEBUG, "NAN: Responder NMI " MACSTR,
               MAC2STR(responder_nmi));
    wpa_hexdump_key(MSG_DEBUG, "NAN: Derived key", key, key_len);

    return 0;
}

/** Same layout as @ref nan_pasn_store_ptk (KCK|KEK|TK|KDK). */
static int nan_pasn_flatten_ptk_blob(struct wpa_ptk *ptk, u8 *dst, size_t dst_sz,
                                     size_t *out_len)
{
    u8 *pos;

    if (!ptk || !dst || !out_len) {
        return -1;
    }
    if (ptk->ptk_len > dst_sz) {
        return -1;
    }
    pos = dst;
    *out_len = ptk->ptk_len;
    if (ptk->kck_len) {
        os_memcpy(pos, ptk->kck, ptk->kck_len);
        pos += ptk->kck_len;
    }
    if (ptk->kek_len) {
        os_memcpy(pos, ptk->kek, ptk->kek_len);
        pos += ptk->kek_len;
    }
    if (ptk->tk_len) {
        os_memcpy(pos, ptk->tk, ptk->tk_len);
        pos += ptk->tk_len;
    }
    if (ptk->kdk_len) {
        os_memcpy(pos, ptk->kdk, ptk->kdk_len);
        pos += ptk->kdk_len;
    }
    return 0;
}

/**
 * Install PASN pairwise TK into the NAN interface key table (firmware).
 * Call while @a pasn still holds a valid PTK (before forced_memzero).
 */
static int nan_pasn_install_nan_pairwise_tk(struct nan_pasn_data *nan, struct pasn_data *pasn)
{
    struct wpa_ptk *ptk;
    uint8_t key_rsc[8] = {0};
    int kret;
    (void)nan;

    if (!pasn) {
        return -1;
    }

    if (!nan_pasn_cipher_is_supported(pasn->cipher)) {
        wpa_printf(MSG_INFO, "NAN PASN: skip NAN TK install (cipher=%d)", pasn->cipher);
        return -1;
    }

    ptk = pasn_get_ptk(pasn);
    if (!ptk || !ptk->tk_len || ptk->tk_len != nan_pasn_cipher_tk_len(pasn->cipher) ||
            ptk->tk_len > sizeof(ptk->tk)) {
        return -1;
    }

    wpa_hexdump_key(MSG_DEBUG, "NAN PASN: NM-TK", ptk->tk, ptk->tk_len);
    kret = esp_wifi_set_nan_key_internal(
               NAN_PASN_WIFI_ALG_CCMP, pasn->peer_addr, temp, 1, key_rsc, sizeof(key_rsc),
               ptk->tk, ptk->tk_len,
               NAN_KEY_NM_TK);
    if (kret != 0) {
        wpa_printf(MSG_WARNING, "NAN PASN: esp_wifi_set_nan_key_internal failed (%d)",
                   kret);
        return -1;
    }
    return 0;
}

/**
 * Common path for @ref esp_nan_app_post_pasn_pairing_indication (initiator and responder).
 *
 * @param initiator_nmi  Initiator NMI (6 octets).
 * @param responder_nmi  Responder NMI (6 octets).
 */
static void nan_pasn_post_pasn_pairing_indication_evt(struct nan_pasn_data *nan,
                                                      struct pasn_data *pasn,
                                                      const u8 initiator_nmi[ETH_ALEN],
                                                      const u8 responder_nmi[ETH_ALEN])
{
#if 0
    wifi_event_nan_pasn_pairing_indication_t ind;

    if (!nan || !pasn) {
        return;
    }

    os_memset(&ind, 0, sizeof(ind));
    ind.type = WIFI_NAN_PASN_PAIRING_IND_TYPE_SETUP;
    ind.self_handle = 0;
    os_memcpy(ind.initiator_nan_address, initiator_nmi, ETH_ALEN);
    os_memcpy(ind.responder_nan_address, responder_nmi, ETH_ALEN);
    ind.paired_peer_handle_valid = 0;
    ind.auth_password = nan->dev_sae_pin_len > 0 ? 1 : 0;
    ind.auth_opportunistic =
        (pasn->akmp == WPA_KEY_MGMT_PASN && nan->dev_sae_pin_len == 0) ? 1 : 0;
    ind.npk_nik_caching = 0;
    esp_nan_app_post_pasn_pairing_indication(&ind);
#endif
}

/**
 * nan_crypto_derive_kek - Derive KEK from NM-KDK after PASN pairing
 *
 * NM-KEK = KDF-HASH-MMM(NM-KDK, "NAN Management KEK Derivation",
 *                       Pairing Initiator NMI || Pairing Responder NMI)
 *
 * Mirrors hostap @c nan_crypto_derive_kek (src/nan/nan_crypto.c). Called from
 * @ref nan_pasn_copy_keys_from_pasn when @c ptk->kdk_len is set.
 */
static int nan_crypto_derive_kek(const u8 *kdk, size_t kdk_len, int cipher,
                                 const u8 *initiator_nmi,
                                 const u8 *responder_nmi, struct wpa_ptk *ptk)
{
    const char *label = "NAN Management KEK Derivation";
    size_t kek_len;

    wpa_printf(MSG_DEBUG, "NAN: Deriving KEK from NM-KDK");

    if (!kdk || !kdk_len || !initiator_nmi || !responder_nmi || !ptk) {
        wpa_printf(MSG_INFO,
                   "NAN: Invalid parameters for KEK derivation");
        return -1;
    }

    if (!nan_pasn_cipher_is_supported(cipher)) {
        wpa_printf(MSG_INFO,
                   "NAN: Unsupported cipher suite for KEK derivation: %d",
                   cipher);
        return -1;
    }

    kek_len = nan_pasn_cipher_kek_len(cipher);
    if (kek_len > sizeof(ptk->kek)) {
        wpa_printf(MSG_INFO,
                   "NAN: KEK length %zu exceeds wpa_ptk buffer", kek_len);
        return -1;
    }

    ptk->kek_len = kek_len;

    return nan_crypto_derive_from_kdk(kdk, kdk_len, cipher, label,
                                      initiator_nmi, responder_nmi,
                                      ptk->kek, ptk->kek_len);
}

static void nan_pasn_copy_keys_from_pasn(struct nan_pasn_data *nan, struct pasn_data *pasn)
{
    struct wpa_ptk *ptk;
    size_t pmk_len;
    const u8 *pmk_ptr;

    if (!nan || !pasn) {
        return;
    }

    forced_memzero(&g_nan_pasn_saved_keys, sizeof(g_nan_pasn_saved_keys));

    ptk = pasn_get_ptk(pasn);
    if (!ptk) {
        return;
    }

    os_memcpy(g_nan_pasn_saved_keys.peer_addr, pasn->peer_addr, ETH_ALEN);
    g_nan_pasn_saved_keys.role = nan->dev_role;
    g_nan_pasn_saved_keys.akmp = pasn->akmp;
    g_nan_pasn_saved_keys.cipher = pasn->cipher;

    pmk_len = pasn_get_pmk_len(pasn);
    pmk_ptr = pasn_get_pmk(pasn);
    if (pmk_len && pmk_ptr && pmk_len <= sizeof(g_nan_pasn_saved_keys.pmk)) {
        os_memcpy(g_nan_pasn_saved_keys.pmk, pmk_ptr, pmk_len);
        g_nan_pasn_saved_keys.pmk_len = pmk_len;
    }

    /*
     * NAN Management KEK is derived from KDK using pairing initiator/responder
     * NMI addresses (nan_crypto_derive_kek), same pattern as hostap. ND-PMK is
     * derived earlier via pasn_nd_pmk_derive_from_kdk_store() (hostap
     * nan_crypto_derive_nd_pmk_from_kdk) after pasn_pmk_to_ptk.
     */
    if (ptk->kdk_len) {
        const u8 *initiator_nmi;
        const u8 *responder_nmi;

        if (nan->dev_role == NAN_ROLE_PAIRING_INITIATOR) {
            initiator_nmi = pasn->own_addr;
            responder_nmi = pasn->peer_addr;
        } else {
            initiator_nmi = pasn->peer_addr;
            responder_nmi = pasn->own_addr;
        }

        if (nan_crypto_derive_kek(ptk->kdk, ptk->kdk_len, pasn->cipher,
                                  initiator_nmi, responder_nmi, ptk) != 0) {
            wpa_printf(MSG_INFO, "NAN PASN: KEK derivation failed");
            forced_memzero(&g_nan_pasn_saved_keys, sizeof(g_nan_pasn_saved_keys));
            return;
        }

        ptk->ptk_len = ptk->kck_len + ptk->kek_len + ptk->tk_len + ptk->kdk_len;
    }

    if (nan_pasn_flatten_ptk_blob(ptk, g_nan_pasn_saved_keys.ptk_blob,
                                  sizeof(g_nan_pasn_saved_keys.ptk_blob),
                                  &g_nan_pasn_saved_keys.ptk_blob_len) != 0) {
        forced_memzero(&g_nan_pasn_saved_keys, sizeof(g_nan_pasn_saved_keys));
        return;
    }

    if (ptk->kek_len && ptk->kek_len <= sizeof(g_nan_pasn_saved_keys.kek)) {
        os_memcpy(g_nan_pasn_saved_keys.kek, ptk->kek, ptk->kek_len);
        g_nan_pasn_saved_keys.kek_len = ptk->kek_len;
    }

    g_nan_pasn_saved_keys.valid = 1;
}

const struct nan_pasn_key_material *nan_pasn_get_saved_keys(void)
{
    return g_nan_pasn_saved_keys.valid ? &g_nan_pasn_saved_keys : NULL;
}

void nan_pasn_clear_saved_keys(void)
{
    forced_memzero(&g_nan_pasn_saved_keys, sizeof(g_nan_pasn_saved_keys));
}

/* NAN KDE OUI Type values from Wi-Fi Aware spec v4.0, Table 126. */
#define NAN_PASN_KDE_OUI_TYPE_NIK       36
#define NAN_PASN_KDE_OUI_TYPE_LIFETIME  37
#define NAN_PASN_KEY_LIFETIME_NIK_BIT   BIT(3)

/**
 * Decrypt NAN key data using AES Key Unwrap (RFC 3394).
 *
 * Ported from hostap @c nan_crypto_decrypt_key_data (src/nan/nan_crypto.c).
 * Caller is responsible for freeing the returned wpabuf using @c wpabuf_free.
 *
 */

static struct wpabuf *
nan_crypto_decrypt_key_data(const u8 *kek, size_t kek_len,
                            const u8 *encrypted_data, size_t encrypted_len)
{
    struct wpabuf *decrypted;
    size_t plain_len;
    u8 *buf;

    if (!encrypted_data || !encrypted_len) {
        wpa_printf(MSG_INFO, "NAN: Invalid encrypted key data");
        return NULL;
    }

    wpa_hexdump_key(MSG_DEBUG, "NAN: Encrypted key data",
                    encrypted_data, encrypted_len);

    if (!kek || !kek_len) {
        wpa_printf(MSG_INFO,
                   "NAN: No KEK available for key data decryption");
        return NULL;
    }

    wpa_hexdump_key(MSG_DEBUG, "NAN: KEK for decryption", kek, kek_len);

    /* AES-WRAP adds 8 bytes overhead and requires 8-byte aligned input. */
    if (encrypted_len < NAN_PASN_AES_WRAP_MIN_CIPHERTEXT ||
            encrypted_len % 8 != 0) {
        wpa_printf(MSG_INFO,
                   "NAN: Invalid encrypted key data length %zu",
                   encrypted_len);
        return NULL;
    }

    plain_len = encrypted_len - 8;
    decrypted = wpabuf_alloc(plain_len);
    if (!decrypted) {
        wpa_printf(MSG_INFO,
                   "NAN: Failed to allocate decryption buffer");
        return NULL;
    }

    buf = wpabuf_put(decrypted, plain_len);
    if (aes_unwrap(kek, kek_len, plain_len / 8, encrypted_data, buf)) {
        wpa_printf(MSG_INFO,
                   "NAN: AES unwrap failed - could not decrypt key data");
        wpabuf_free(decrypted);
        return NULL;
    }

    wpa_hexdump_key(MSG_DEBUG, "NAN: Decrypted key data",
                    wpabuf_head(decrypted), wpabuf_len(decrypted));

    return decrypted;
}

/**
 * Walk a sequence of NAN KDEs (Vendor Specific elements with WFA OUI) in the
 * decrypted Key Data and copy the NIK and lifetime fields into output args.
 */
static int nan_pasn_parse_nik_kdes(const u8 *data, size_t len, int cipher,
                                   u8 *nik, u8 *cipher_ver,
                                   u32 *lifetime_sec, u16 *lifetime_bitmap)
{
    size_t gtk_key_len = nan_pasn_cipher_tk_len(cipher);
    size_t gtk_kde_min = 2 + 6 + gtk_key_len;
    /* RSN KDE selectors. Defined locally so the parser stays usable even when
     * the upstream macros are gated by CONFIG_IEEE80211W or are absent (BIGTK).
     */
    static const u32 sel_igtk  = RSN_SELECTOR(0x00, 0x0f, 0xac, 9);
    static const u32 sel_bigtk = RSN_SELECTOR(0x00, 0x0f, 0xac, 14);
    static const u8 wfa_oui[3] = { 0x50, 0x6f, 0x9a };
    bool nik_found = false;
    const u8 *pos = data;
    const u8 *end = data + len;

    while (pos + 2 <= end) {
        u8 id = pos[0];
        u8 elen = pos[1];

        if (pos + 2 + elen > end) {
            return -1;
        }
        if (id != WLAN_EID_VENDOR_SPECIFIC || elen < 4) {
            pos += 2 + elen;
            continue;
        }

        const u32 selector = RSN_SELECTOR_GET(pos + 2);
        const u8 oui_type = pos[5];
        const u8 *kde_body = pos + 6;
        size_t kde_body_len = elen - 4;

        if (os_memcmp(pos + 2, wfa_oui, sizeof(wfa_oui)) == 0) {
            if (oui_type == NAN_PASN_KDE_OUI_TYPE_NIK &&
                    kde_body_len >= 1 + NAN_PASN_NIK_LEN) {
                if (cipher_ver) {
                    *cipher_ver = kde_body[0];
                }
                os_memcpy(nik, kde_body + 1, NAN_PASN_NIK_LEN);
                nik_found = true;
                wpa_printf(MSG_DEBUG, "NAN: NIK KDE cipher_ver=%u",
                           kde_body[0]);
                wpa_hexdump_key(MSG_DEBUG, "NAN: NIK",
                                kde_body + 1, NAN_PASN_NIK_LEN);
            } else if (oui_type == NAN_PASN_KDE_OUI_TYPE_LIFETIME &&
                       kde_body_len >= 6) {
                if (lifetime_bitmap) {
                    *lifetime_bitmap = WPA_GET_LE16(kde_body);
                }
                if (lifetime_sec) {
                    *lifetime_sec = WPA_GET_BE32(kde_body + 2);
                }
            }
        } else if (gtk_key_len && selector == sel_igtk &&
                   kde_body_len >= gtk_kde_min) {
            /* IGTK KDE: KeyID(2 LE) | IPN(6) | IGTK */
            wpa_printf(MSG_DEBUG, "NAN: IGTK KDE KeyID=%u igtk_len=%zu",
                       WPA_GET_LE16(kde_body), kde_body_len - 8);
            wpa_hexdump(MSG_DEBUG, "NAN: IGTK IPN", kde_body + 2, 6);
            wpa_hexdump_key(MSG_DEBUG, "NAN: IGTK", kde_body + 8, kde_body_len - 8);
        } else if (gtk_key_len && selector == sel_bigtk &&
                   kde_body_len >= gtk_kde_min) {
            /* BIGTK KDE: KeyID(2 LE) | BIPN(6) | BIGTK */
            wpa_printf(MSG_DEBUG, "NAN: BIGTK KDE KeyID=%u bigtk_len=%zu",
                       WPA_GET_LE16(kde_body), kde_body_len - 8);
            wpa_hexdump(MSG_DEBUG, "NAN: BIPN", kde_body + 2, 6);
            wpa_hexdump_key(MSG_DEBUG, "NAN: BIGTK", kde_body + 8, kde_body_len - 8);
        }

        pos += 2 + elen;
    }

    /* Lifetime KDE is optional in practice (e.g. iPhone omits it), so only
     * the NIK is required here.
     */
    return nik_found ? 0 : -1;
}

/**
 * Expected format of shared_key_attr (NCS-PK-PASN-128, MIC=16):
 *
 * Offset  Size  Field                                  Source / spec ref
 * ─────────────────────────────────────────────────────────────────────
 * NAN attribute header (Wi-Fi Aware v4.0, Table 125)
 *   0x00   1   Attribute ID = 0x24                    NAN_ATTR_SHARED_KEY_DESCR
 *   0x01   2   Attribute Length (LE)                  body length, not incl. header
 *   0x03   1   Publish ID                             struct nan_shared_key.publish_id
 *
 * IEEE 802.11 RSNA Key Descriptor (EAPOL-Key body, IEEE 802.11-2020 §12.7.2)
 *   0x04   1   Descriptor Type = 0x02                 NAN_KEY_DESC (Wi-Fi Aware fixed)
 *   0x05   2   Key Information (BE)
 *   0x07   2   Key Length (BE) = 0x0000               not carrying a pairwise cipher key
 *   0x09   8   Key Replay Counter                     unused for PASN one-shot
 *   0x11  32   Key Nonce                              unused (no 4-way handshake)
 *   0x31  16   EAPOL-Key IV                           unused
 *   0x41   8   Key RSC                                unused
 *   0x49   8   Reserved (Key ID)
 *   0x51  16   Key MIC                                HMAC over body w/ MIC zeroed
 *   0x61   2   Key Data Length (BE)                   length of the wrapped blob
 *   0x63   N   Key Data                               AES-WRAP(KEK, KDEs || pad)
 *
 *  Total = 4 + 95 + N bytes.
 *
 * NCS-PK-PASN-128 (CCMP) and NCS-PK-PASN-256 (GCMP-256) use cipher-dependent
 * KEK/MIC lengths (see @c nan_pasn_cipher_*_len).
 */
int nan_pasn_followup_decrypt_keys(const uint8_t *shared_key_attr,
                                   size_t attr_total_len,
                                   uint8_t *nik, size_t nik_size,
                                   uint8_t *cipher_ver,
                                   uint32_t *lifetime_sec)
{
    const struct nan_pasn_key_material *saved;
    const struct wpa_eapol_key *key_desc;
    const u8 *body;
    size_t body_len;
    u16 attr_body_len;
    u16 key_info;
    u16 key_data_len;
    u8 found_cipher_ver = 0;
    u32 found_lifetime = 0;
    u16 lifetime_bitmap = 0;
    struct wpabuf *key_data = NULL;
    size_t eapol_hdrlen;
    size_t mic_len;
    int ret = -1;

    if (!shared_key_attr || !nik || nik_size < NAN_PASN_NIK_LEN) {
        return -1;
    }

    /* Attribute header: ID(1) + Length(2 LE). */
    if (attr_total_len < 3 || shared_key_attr[0] != NAN_ATTR_SHARED_KEY_DESCR) {
        wpa_printf(MSG_INFO, "NAN: Invalid Shared Key Descriptor attribute");
        return -1;
    }

    attr_body_len = WPA_GET_LE16(&shared_key_attr[1]);
    if ((size_t)attr_body_len + 3 > attr_total_len) {
        wpa_printf(MSG_INFO,
                   "NAN: Truncated Shared Key Descriptor attribute (len=%u, total=%zu)",
                   attr_body_len, attr_total_len);
        return -1;
    }

    saved = nan_pasn_get_saved_keys();
    if (!saved || !saved->kek_len) {
        wpa_printf(MSG_INFO,
                   "NAN: No saved KEK available to decrypt Shared Key Descriptor");
        return -1;
    }
    if (!nan_pasn_cipher_is_supported(saved->cipher)) {
        wpa_printf(MSG_INFO,
                   "NAN: Unsupported cipher 0x%x for Shared Key Descriptor",
                   saved->cipher);
        return -1;
    }

    eapol_hdrlen = nan_pasn_cipher_eapol_key_hdrlen(saved->cipher);
    mic_len = nan_pasn_cipher_mic_len(saved->cipher);
    if (!eapol_hdrlen || !mic_len) {
        return -1;
    }

    /*
     * Body layout (Wi-Fi Aware spec v4.0 + IEEE 802.11 EAPOL-Key):
     *   publish_id(1) + EAPOL-Key descriptor (MIC length per cipher) +
     *   key_data.
     */
    if (attr_body_len < 1 + eapol_hdrlen) {
        wpa_printf(MSG_INFO,
                   "NAN: Shared Key Descriptor body too short (%u, need %zu)",
                   attr_body_len, (size_t)(1 + eapol_hdrlen));
        return -1;
    }

    body = &shared_key_attr[3];
    body_len = attr_body_len;

    /* Skip the 1-byte Publish ID; key descriptor starts at offset 1. */
    key_desc = (const struct wpa_eapol_key *)(body + 1);
    key_info = WPA_GET_BE16(key_desc->key_info);

    if (!(key_info & WPA_KEY_INFO_KEY_TYPE)) {
        wpa_printf(MSG_INFO,
                   "NAN: Follow-up frame does not contain pairwise key");
        return -1;
    }
    if (!(key_info & WPA_KEY_INFO_ENCR_KEY_DATA)) {
        wpa_printf(MSG_INFO,
                   "NAN: Follow-up frame does not contain encrypted key data");
        return -1;
    }

    key_data_len = WPA_GET_BE16(key_desc->key_data_length);
    if ((size_t)1 + eapol_hdrlen + key_data_len > body_len) {
        wpa_printf(MSG_INFO,
                   "NAN: Shared Key Descriptor key data overruns attribute (key_data_len=%u, body_len=%zu, eapol_hdrlen=%zu)",
                   key_data_len, body_len, eapol_hdrlen);
        return -1;
    }

    wpa_printf(MSG_DEBUG,
               "NAN: Shared Key Descr cipher=%d mic_len=%zu key_data_len=%u body_len=%zu",
               saved->cipher, mic_len, key_data_len, body_len);

    key_data = nan_crypto_decrypt_key_data(saved->kek, saved->kek_len,
                                           body + 1 + eapol_hdrlen,
                                           key_data_len);
    if (!key_data) {
        wpa_printf(MSG_INFO,
                   "NAN: Failed to decrypt Shared Key Descriptor key data");
        return -1;
    }

    if (nan_pasn_parse_nik_kdes(wpabuf_head(key_data), wpabuf_len(key_data),
                                saved->cipher, nik, &found_cipher_ver,
                                &found_lifetime, &lifetime_bitmap) != 0) {
        wpa_printf(MSG_INFO,
                   "NAN: NIK KDE missing in decrypted key data");
        goto out;
    }

    /* Lifetime KDE is optional; if present, its bitmap must mark NIK. */
    if (found_lifetime &&
            !(lifetime_bitmap & NAN_PASN_KEY_LIFETIME_NIK_BIT)) {
        wpa_printf(MSG_INFO,
                   "NAN: Unexpected key bitmap in Key Lifetime KDE: 0x%04x",
                   lifetime_bitmap);
        goto out;
    }

    if (cipher_ver) {
        *cipher_ver = found_cipher_ver;
    }
    if (lifetime_sec) {
        *lifetime_sec = found_lifetime;
    }

    wpa_hexdump_key(MSG_DEBUG, "NAN: Peer NIK from follow-up", nik,
                    NAN_PASN_NIK_LEN);
    ret = 0;

out:
    wpabuf_clear_free(key_data);
    return ret;
}

static int nan_chan_to_freq_mhz(uint8_t chan)
{
    if (chan >= 1 && chan <= 13) {
        return 2407 + (int)chan * 5;
    }
    if (chan == 14) {
        return 2484;
    }
    return 0;
}

static int nan_pasn_get_current_freq_mhz(void)
{
    uint8_t primary = 0;
    wifi_second_chan_t second = WIFI_SECOND_CHAN_NONE;

    if (esp_wifi_get_channel(&primary, &second) != ESP_OK || primary == 0) {
        return 0;
    }
    return nan_chan_to_freq_mhz(primary);
}

static void nan_pasn_auth_timeout_cancel(struct nan_pasn_data *nan);

static void nan_pasn_auth_timeout_cb(void *eloop_ctx, void *user_data)
{
    struct nan_pasn_data *nan = eloop_ctx;

    if (!nan) {
        return;
    }

    wpa_printf(MSG_INFO,
               "NAN PASN: %s auth timed out after %u seconds",
               ((enum nan_role)(uintptr_t)user_data == NAN_ROLE_PAIRING_INITIATOR) ?
               "initiator" :
               "responder",
               NAN_PASN_AUTH_TIMEOUT_SECS);
    nan_pasn_data_deinit(nan);
}

static int nan_pasn_auth_timeout_arm(struct nan_pasn_data *nan, enum nan_role role)
{
    if (!nan) {
        return -1;
    }

    nan_pasn_auth_timeout_cancel(nan);
    if (eloop_register_timeout(NAN_PASN_AUTH_TIMEOUT_SECS, 0,
                               nan_pasn_auth_timeout_cb, nan,
                               (void *)(uintptr_t)role) != 0) {
        return -1;
    }

    return 0;
}

static void nan_pasn_auth_timeout_cancel(struct nan_pasn_data *nan)
{
    if (!nan) {
        return;
    }

    eloop_cancel_timeout(nan_pasn_auth_timeout_cb, nan, ELOOP_ALL_CTX);
}

/* Store decimal PIN digits in @a nan->dev_sae_pin (NUL-terminated ASCII) for SAE. */
static int nan_set_dev_sae_pin(struct nan_pasn_data *nan, const char *digits)
{
    size_t i;
    size_t len;

    if (!nan || !digits) {
        return -1;
    }

    os_memset(nan->dev_sae_pin, 0, sizeof(nan->dev_sae_pin));
    nan->dev_sae_pin_len = 0;

    len = os_strlen(digits);
    if (len == 0 || len >= sizeof(nan->dev_sae_pin)) {
        return -1;
    }

    for (i = 0; i < len; i++) {
        if (digits[i] < '0' || digits[i] > '9') {
            return -1;
        }
        nan->dev_sae_pin[i] = digits[i];
    }
    nan->dev_sae_pin[len] = '\0';
    nan->dev_sae_pin_len = len;

    return 0;
}

/*
 * Build Wi-Fi Alliance NAN vendor IE (EID 221) with DCEA / BPBA / CSIA for PASN
 * and pass to @a pasn via pasn_set_extra_ies() so wpa_pasn_add_extra_ies() appends
 * it after PASN Parameters on Auth 1/3 (and after prepare_data_element on Auth 2).
 */
static int nan_prepare_pasn_extra_ie(struct nan_pasn_data *nan, struct pasn_data *pasn,
                                     const struct wpabuf *frame, bool add_dira)
{
    static const u8 nan_pasn_attr_payload[] = {
        NAN_ATTR_DCEA, 0x02, 0x00, 0x00, 0x03,
        NAN_ATTR_BPBA, 0x05, 0x00, 0x90, 0x02, 0x00, 0x02, 0x00,
        NAN_ATTR_CSIA, 0x03, 0x00, 0x00, 0x07, 0x05,
    };
    u8 fixed[2 + 3 + 1 + sizeof(nan_pasn_attr_payload)];
    u8 *buf = NULL;
    size_t fr_len = 0;
    size_t total_len;
    int ret;

    (void)nan;
    (void)add_dira;

    fixed[0] = WLAN_EID_VENDOR_SPECIFIC;
    fixed[1] = 3 + 1 + sizeof(nan_pasn_attr_payload);
    WPA_PUT_BE24(&fixed[2], OUI_WFA);
    fixed[5] = NAN_OUI_TYPE;
    os_memcpy(&fixed[6], nan_pasn_attr_payload, sizeof(nan_pasn_attr_payload));

    if (frame) {
        fr_len = wpabuf_len(frame);
    }

    if (!fr_len) {
        return pasn_set_extra_ies(pasn, fixed, sizeof(fixed));
    }

    total_len = sizeof(fixed) + fr_len;
    buf = os_malloc(total_len);
    if (!buf) {
        return -1;
    }
    os_memcpy(buf, fixed, sizeof(fixed));
    os_memcpy(buf + sizeof(fixed), wpabuf_head_u8(frame), fr_len);
    ret = pasn_set_extra_ies(pasn, buf, total_len);
    os_free(buf);
    return ret;
}

static struct wpabuf * nan_pairing_generate_rsnxe(struct nan_pasn_data *nan,
                                                  int akmp)
{
    u32 capab;
    size_t flen = 0;
    struct wpabuf *buf;

    capab = BIT(WLAN_RSNX_CAPAB_KEK_IN_PASN);

    if (wpa_key_mgmt_sae(akmp)) {
        capab |= BIT(WLAN_RSNX_CAPAB_SAE_H2E);
    }

    while (capab >> flen * 8) {
        flen++;
    }
    if (!flen) {
        flen = 1;
    }

    buf = wpabuf_alloc(2 + 1);
    if (!buf) {
        return NULL;
    }

    if (wpabuf_tailroom(buf) < 2 + 1) {
        wpa_printf(MSG_INFO, "wpabuf tail room too small");
        wpabuf_free(buf);
        return NULL;
    }
    capab |= flen - 1; /* merge full-IE length code for cap bits */
    capab &= ~0x0000000fU; /* RSNX Length (bits 0-3): n-1 = 0 */

    wpa_printf(MSG_INFO, "RSNXE capabilities: %04x", capab);
    wpabuf_put_u8(buf, WLAN_EID_RSNX);
    /* PASN/NAN: only first RSNX octet on the wire; omit 2nd + reserved tail */
    wpabuf_put_u8(buf, 1);
    wpabuf_put_u8(buf, (u8)(capab & 0xff));
    return buf;
}

/* Fixed NAN pairing SSID (salt for SAE HKDF / H2E), not the user PIN. */
#define NAN_PAIRING_SSID "516F9A010000"

static void nan_pairing_apply_sae_pin(struct pasn_data *pasn, u8 pasn_type,
                                      struct nan_pasn_data *nan)
{
    int pasn_groups[4] = { 0 };
    size_t ssid_len = os_strlen(NAN_PAIRING_SSID);

    if (!pasn || !nan || nan->dev_sae_pin_len == 0) {
        return;
    }

    if (pasn_type & 0xc && pasn_type & 0x3) {
        pasn_groups[0] = 20;
        pasn_groups[1] = 19;
    } else if (pasn_type & 0xc) {
        pasn_groups[0] = 20;
    } else {
        pasn_groups[0] = 19;
    }

    pasn->pt = sae_derive_pt(pasn_groups, (const u8 *) NAN_PAIRING_SSID,
                             ssid_len, (const u8 *) nan->dev_sae_pin,
                             nan->dev_sae_pin_len, NULL);
    pasn->password = nan->dev_sae_pin;
}

void nan_pasn_initialize(struct nan_pasn_data *nan, const u8 *addr, int freq, bool verify, bool derive_kek)
{
    struct pasn_data *pasn;
    struct wpabuf *rsnxe;

    if (!nan) {
        return;
    }

    if (nan->pasn) {
        wpa_pasn_reset(nan->pasn);
    } else {
        nan->pasn = pasn_data_init();
        if (!nan->pasn) {
            return;
        }
    }

    pasn = nan->pasn;

    os_memcpy(pasn->own_addr, nan->cfg->dev_addr, ETH_ALEN);

    /* PASN Auth1 DA = pasn->peer_addr (see wpa_pasn_build_auth_header). Avoid broadcast DA. */
    if (addr) {
        const u8 *peer = addr;

        if (is_broadcast_ether_addr(peer)) {
            /* Responder may not know peer MAC until Auth1; allow broadcast placeholder. */
            if (nan->dev_role == NAN_ROLE_PAIRING_RESPONDER) {
                os_memcpy(pasn->peer_addr, peer, ETH_ALEN);
            } else if (is_zero_ether_addr(nan->pasn_unicast_peer) ||
                       is_broadcast_ether_addr(nan->pasn_unicast_peer)) {
                wpa_printf(MSG_INFO,
                           "NAN PASN: peer is broadcast and no cached unicast peer");
                return;
            } else {
                peer = nan->pasn_unicast_peer;
                wpa_printf(MSG_DEBUG,
                           "NAN PASN: using cached unicast peer for Auth1 DA");
            }
        } else {
            os_memcpy(nan->pasn_unicast_peer, peer, ETH_ALEN);
        }
        os_memcpy(pasn->peer_addr, peer, ETH_ALEN);
    }

    os_memcpy(pasn->bssid, nan->dev_role == NAN_ROLE_PAIRING_INITIATOR ?
              pasn->peer_addr : pasn->own_addr, ETH_ALEN);

    pasn->group = 19;
    pasn->cipher = WPA_CIPHER_CCMP;
    pasn_enable_kdk_derivation(pasn);

    /*
     * NAN PASN uses kek_len 0 for in-frame PASN-PTK; NAN Management KEK comes
     * from KDK via nan_crypto_derive_kek in nan_pasn_copy_keys_from_pasn.
     * ND-PMK is filled by pasn_nd_pmk_derive_from_kdk_store (hostap
     * nan_crypto_derive_nd_pmk_from_kdk). Matches hostap nan_pairing.c.
     */
    (void)derive_kek;
    pasn->derive_kek = false;
    pasn->kek_len = 0;

    if (nan->dev_sae_pin_len > 0) {
        pasn->akmp = WPA_KEY_MGMT_SAE;
        nan_pairing_apply_sae_pin(pasn, nan->cfg->pasn_type, nan);
    } else if (!verify) {
        pasn->akmp = WPA_KEY_MGMT_PASN;
    }

    pasn->rsn_pairwise = pasn->cipher;
    pasn->wpa_key_mgmt = pasn->akmp;

    rsnxe = nan_pairing_generate_rsnxe(nan, pasn->akmp);
    if (rsnxe) {
        os_free(pasn->rsnxe_ie);
        pasn->rsnxe_ie = os_memdup(wpabuf_head_u8(rsnxe),
                                   wpabuf_len(rsnxe));
        if (!pasn->rsnxe_ie) {
            wpabuf_free(rsnxe);
            return;
        }
        wpabuf_free(rsnxe);
    }

    if (nan->dev_role == NAN_ROLE_PAIRING_INITIATOR) {
        pasn->pmksa = nan->initiator_pmksa;
    } else {
        pasn->pmksa = nan->responder_pmksa;
    }

    pasn->cb_ctx = nan->cfg->cb_ctx;
    pasn->send_mgmt = nan->cfg->pasn_send_mgmt;
    pasn->prepare_data_element = nan->cfg->prepare_data_element;
    pasn->parse_data_element = nan->cfg->parse_data_element;
    pasn->validate_custom_pmkid = nan->cfg->pasn_validate_pmkid;

    pasn->freq = freq;

}

int nan_initiate_pasn_verify(struct nan_pasn_data *pd, const u8 *peer_addr,
                             int freq, int role,
                             const u8 *bssid, const u8 *ssid, size_t ssid_len)
{
    struct nan_pasn_data *nan;
    struct pasn_data *pasn;
    int ret = 0;

    (void)role;
    (void)bssid;
    (void)ssid;
    (void)ssid_len;

    if (!pd) {
        return -1;
    }
    nan = pd;

    if (!peer_addr) {
        wpa_printf(MSG_INFO, "Peer address NULL");
        return -1;
    }

    nan->dev_role = NAN_ROLE_PAIRING_INITIATOR;
    nan_pasn_initialize(nan, peer_addr, freq, true, true);
    pasn = nan->pasn;

    if (nan_prepare_pasn_extra_ie(nan, pasn, NULL, false) != 0) {
        wpa_printf(MSG_INFO, "NAN PASN: extra IE failed");
        return -1;
    }

    if (wpa_pasn_verify(pasn, pasn->own_addr, pasn->peer_addr, pasn->bssid,
                        pasn->akmp, pasn->cipher, pasn->group, pasn->freq,
                        NULL, 0, NULL, 0, NULL)) {
        wpa_printf(MSG_INFO, "PASN verify failed");
        ret = -1;
    }
    if (pasn->extra_ies) {
        os_free((u8 *) pasn->extra_ies);
        pasn->extra_ies = NULL;
        pasn->extra_ies_len = 0;
    }
    return ret;
}

int nan_initiate_pasn_auth(struct nan_pasn_data *pd, const u8 *addr, int freq)
{
    struct nan_pasn_data *nan;
    struct pasn_data *pasn;
    int ret = 0;

    if (!pd) {
        return -1;
    }
    nan = pd;

    if (!addr) {
        wpa_printf(MSG_INFO, "Peer address NULL");
        return -1;
    }

    nan->dev_role = NAN_ROLE_PAIRING_INITIATOR;

    nan_pasn_initialize(nan, addr, freq, false, true);
    pasn = nan->pasn;

    pasn_initiator_pmksa_cache_remove(pasn->pmksa, (u8 *)addr);

    if (nan_prepare_pasn_extra_ie(nan, pasn, NULL, false) != 0) {
        wpa_printf(MSG_INFO, "NAN PASN: extra IE failed");
        return -1;
    }

    if (wpas_pasn_start(pasn, pasn->own_addr, pasn->peer_addr, pasn->bssid,
                        pasn->akmp, pasn->cipher, pasn->group, pasn->freq,
                        NULL, 0, NULL, 0, NULL)) {
        wpa_printf(MSG_INFO, "Failed to start PASN");
        ret = -1;
    } else {
        nan_pasn_post_pasn_pairing_indication_evt(nan, pasn, pasn->own_addr,
                                                  pasn->peer_addr);
        if (nan_pasn_auth_timeout_arm(nan, NAN_ROLE_PAIRING_INITIATOR) != 0) {
            wpa_printf(MSG_INFO, "Failed to arm initiator PASN auth timeout");
            ret = -1;
        }
    }
    if (pasn->extra_ies) {
        os_free((u8 *) pasn->extra_ies);
        pasn->extra_ies = NULL;
        pasn->extra_ies_len = 0;
    }
    return ret;
}

int * int_array_dup(const int *a)
{
    if (!a) {
        return NULL;
    }
    return os_memdup(a, (int_array_len(a) + 1) * sizeof(int));
}

static int nan_handle_pasn_auth(struct nan_pasn_data *nan,
                                const struct ieee80211_auth *mgmt, size_t len,
                                int freq)
{
    struct pasn_data *pasn;
    u8 pasn_type;
    int pasn_groups[4] = { 0 };
    u16 auth_alg, auth_transaction, status_code;

    (void)freq;
    if (!nan || !nan->pasn) {
        return -1;
    }

    if (os_memcmp(mgmt->da, nan->cfg->dev_addr, ETH_ALEN) != 0) {
        wpa_printf(MSG_INFO, "PASN Responder: Not our frame");
        return -1;
    }

    if (len < offsetof(struct ieee80211_auth, auth.variable)) {
        return -1;
    }

    pasn = nan->pasn;
    auth_alg = le_to_host16(mgmt->auth.auth_alg);
    status_code = le_to_host16(mgmt->auth.status_code);
    auth_transaction = le_to_host16(mgmt->auth.auth_transaction);

    if (status_code != WLAN_STATUS_SUCCESS &&
            status_code != WLAN_STATUS_ASSOC_REJECTED_TEMPORARILY) {
        wpa_printf(MSG_INFO, "PASN: Authentication rejected - status=%u",
                   status_code);
        return -1;
    }

    if (auth_alg != WLAN_AUTH_PASN ||
            auth_transaction == WLAN_AUTH_TR_SEQ_PASN_AUTH2) {
        wpa_printf(MSG_INFO,
                   "PASN Responder: Not a PASN frame or unexpected Authentication frame, auth_alg=%d",
                   auth_alg);
        return -1;
    }
    if (auth_transaction == WLAN_AUTH_TR_SEQ_PASN_AUTH1) {
        if (nan_pasn_auth_timeout_arm(nan, NAN_ROLE_PAIRING_RESPONDER) != 0) {
            wpa_printf(MSG_INFO, "PASN Responder: Failed to arm auth timeout");
            return -1;
        }
        pasn_type = nan->cfg->pasn_type;
        if (pasn_type & 0xc && pasn_type & 0x3) {
            pasn_groups[0] = 20;
            pasn_groups[1] = 19;
        } else if (pasn_type & 0xc) {
            pasn_groups[0] = 20;
        } else {
            pasn_groups[0] = 19;
        }
        os_free(pasn->pasn_groups);
        pasn->pasn_groups = int_array_dup(pasn_groups);

        if (handle_auth_pasn_1(pasn, nan->cfg->dev_addr, mgmt->sa, mgmt,
                               len, false) < 0) {
            nan_pasn_auth_timeout_cancel(nan);
            wpa_printf(MSG_INFO,
                       "PASN Responder: Handle Auth 1 failed");
            return -1;
        }
        nan_pasn_post_pasn_pairing_indication_evt(nan, pasn, mgmt->sa,
                                                  nan->cfg->dev_addr);
    } else if (auth_transaction == WLAN_AUTH_TR_SEQ_PASN_AUTH3) {
        if (handle_auth_pasn_3(pasn, nan->cfg->dev_addr, mgmt->sa, mgmt,
                               len) < 0) {
            wpa_printf(MSG_INFO,
                       "PASN Responder: Handle Auth 3 failed");
            return -1;
        }
        nan_pasn_auth_timeout_cancel(nan);
#ifdef CONFIG_TESTING_OPTIONS
        nan_pasn_store_ptk(nan, &pasn->ptk);
#endif /* CONFIG_TESTING_OPTIONS */
        nan_pasn_copy_keys_from_pasn(nan, pasn);
        if (nan_pasn_install_nan_pairwise_tk(nan, pasn) == 0 &&
                nan->pairing_key_installed_cb) {
            const uint8_t *nd_pmk = pasn_nd_pmk_global.valid ?
                                    pasn_nd_pmk_global.nd_pmk : NULL;
            size_t nd_pmk_len = pasn_nd_pmk_global.valid ? PMK_LEN : 0;
            nan->pairing_key_installed_cb(pasn->peer_addr,
                                          (uint8_t)nan->dev_role,
                                          nan_pasn_pasn_cipher_to_ndp_csid(pasn->cipher),
                                          nd_pmk, nd_pmk_len);
        }
        forced_memzero(pasn_get_ptk(pasn), sizeof(pasn->ptk));
        nan_pasn_data_deinit(nan);
    }
    return 0;
}

int nan_pasn_auth_rx(struct nan_pasn_data *nan, const struct ieee80211_auth *mgmt,
                     size_t len, int freq)
{
    int ret = 0;
    u16 auth_transaction;
    struct pasn_data *pasn;
    struct wpa_pasn_params_data pasn_data;

    if (!nan || !mgmt) {
        return -1;
    }
    if (!nan->pasn) {
        nan->pasn = pasn_data_init();
        if (!nan->pasn) {
            wpa_printf(MSG_INFO, "PASN: Uninitialized");
            return -1;
        }
    }

    pasn = nan->pasn;

    wpabuf_free(pasn->frame);
    pasn->frame = NULL;
    pasn_register_callbacks(pasn, nan->cfg->cb_ctx,
                            nan->cfg->pasn_send_mgmt,
                            nan->cfg->pasn_validate_pmkid);
    auth_transaction = le_to_host16(mgmt->auth.auth_transaction);

    if (nan->dev_role == NAN_ROLE_PAIRING_INITIATOR &&
            auth_transaction == WLAN_AUTH_TR_SEQ_PASN_AUTH2) {
        ret = wpa_pasn_auth_rx(pasn, (const u8 *) mgmt, len, &pasn_data);
        if (ret < 0) {
            wpa_printf(MSG_INFO, "PASN: wpa_pasn_auth_rx() failed");
            nan->dev_role = NAN_ROLE_IDLE;
        } else if (ret == 0 && pasn->status == WLAN_STATUS_SUCCESS) {
            /*
             * Pairing setup confirm maps to PASN completion. For initiator,
             * this is only after Auth2 has been validated and Auth3 has been
             * successfully built/transmitted by wpa_pasn_auth_rx().
             */
            nan_pasn_copy_keys_from_pasn(nan, pasn);
            if (nan_pasn_install_nan_pairwise_tk(nan, pasn) == 0 &&
                    nan->pairing_key_installed_cb) {
                const uint8_t *nd_pmk = pasn_nd_pmk_global.valid ?
                                        pasn_nd_pmk_global.nd_pmk : NULL;
                size_t nd_pmk_len = pasn_nd_pmk_global.valid ? PMK_LEN : 0;
                nan->pairing_key_installed_cb(pasn->peer_addr,
                                              (uint8_t)nan->dev_role,
                                              nan_pasn_pasn_cipher_to_ndp_csid(pasn->cipher),
                                              nd_pmk, nd_pmk_len);
            }
        }
#ifdef CONFIG_TESTING_OPTIONS
        nan_pasn_store_ptk(nan, &pasn->ptk);
#endif /* CONFIG_TESTING_OPTIONS */
        forced_memzero(pasn_get_ptk(pasn), sizeof(pasn->ptk));
    } else {
        ret = nan_handle_pasn_auth(nan, mgmt, len, freq);
    }
    return ret;
}

void handle_auth_pasn(uint8_t *buf, size_t len, uint16_t trans_seq, uint16_t status)
{
    const struct ieee80211_auth *mgmt;
    struct nan_pasn_data *nan;
    int rx_freq;

    (void)trans_seq;
    (void)status;

    if (!buf || len < IEEE80211_MGMT_HDRLEN + 6) {
        return;
    }
    mgmt = (const struct ieee80211_auth *)buf;
    nan = esp_nan_app_get_pasn_data();
    rx_freq = nan_pasn_get_current_freq_mhz();
    if (rx_freq <= 0) {
        rx_freq = 2412;
    }
    if (!nan) {
        wpa_printf(MSG_DEBUG, "NAN PASN: receive_pasn: no context");
        return;
    }
    nan_pasn_auth_rx(nan, mgmt, len, rx_freq);
}

void nan_pasn_pmksa_set_pmk(struct nan_pasn_data *nan, const u8 *src, const u8 *dst,
                            const u8 *pmk, size_t pmk_len, const u8 *pmkid)
{
    if (!nan || !pmk || !pmk_len) {
        return;
    }

    wpa_hexdump_key(MSG_DEBUG, "NAN PASN: PMK", pmk, pmk_len);
    if (pmkid) {
        wpa_hexdump(MSG_DEBUG, "NAN PASN: PMKID", pmkid, PMKID_LEN);
    }

    pasn_initiator_pmksa_cache_add(nan->initiator_pmksa, src, dst, pmk,
                                   pmk_len, pmkid);
    pasn_responder_pmksa_cache_add(nan->responder_pmksa, src, dst, pmk,
                                   pmk_len, pmkid);
}

#ifdef CONFIG_TESTING_OPTIONS

void nan_pasn_store_ptk(struct nan_pasn_data *nan, struct wpa_ptk *ptk)
{
    u8 *pos;

    if (ptk->ptk_len > sizeof(nan->pasn_ptk)) {
        wpa_printf(MSG_INFO, "NAN PASN PTK exceeds: (len=%ld)", ptk->ptk_len);
        return;
    }

    pos = nan->pasn_ptk;
    nan->pasn_ptk_len = ptk->ptk_len;
    if (ptk->kck_len) {
        os_memcpy(pos, ptk->kck, ptk->kck_len);
        pos += ptk->kck_len;
    }
    if (ptk->kek_len) {
        os_memcpy(pos, ptk->kek, ptk->kek_len);
        pos += ptk->kek_len;
    }
    if (ptk->tk_len) {
        os_memcpy(pos, ptk->tk, ptk->tk_len);
        pos += ptk->tk_len;
    }
    if (ptk->kdk_len) {
        os_memcpy(pos, ptk->kdk, ptk->kdk_len);
        pos += ptk->kdk_len;
    }

    wpa_hexdump_key(MSG_DEBUG, "NAN PASN: stored PTK blob (KCK|KEK|TK|KDK)",
                    nan->pasn_ptk, nan->pasn_ptk_len);
}

int nan_pasn_get_ptk(struct nan_pasn_data *nan, const u8 **buf, size_t *buf_len)
{
    if (!nan || !nan->pasn_ptk_len) {
        return -1;
    }

    *buf_len = nan->pasn_ptk_len;
    *buf = nan->pasn_ptk;
    return 0;
}

#endif /* CONFIG_TESTING_OPTIONS */

static int nan_pasn_esp_send_mgmt(void *ctx, const u8 *data, size_t data_len, int noack,
                                  unsigned int freq, unsigned int wait_ms)
{
    wifi_mgmt_frm_req_t *req;
    size_t body_len;
    const u8 *body;
    const u8 *da = data + 4;
    struct nan_pasn_data *nan = ctx;
    int ret;

    (void)noack;
    (void)freq;
    (void)wait_ms;

    if (!data || data_len <= IEEE80211_MGMT_HDRLEN) {
        return -1;
    }

    /* If stack built Auth1 with broadcast DA, use cached unicast peer for the driver. */
    if (nan && is_broadcast_ether_addr(da) &&
            !is_zero_ether_addr(nan->pasn_unicast_peer) &&
            !is_broadcast_ether_addr(nan->pasn_unicast_peer)) {
        da = nan->pasn_unicast_peer;
        wpa_printf(MSG_DEBUG,
                   "NAN PASN: overriding broadcast DA with cached unicast peer");
    }

    body = data + IEEE80211_MGMT_HDRLEN;
    body_len = data_len - IEEE80211_MGMT_HDRLEN;

    req = os_zalloc(sizeof(*req) + body_len);
    if (!req) {
        return -1;
    }

    req->ifx = WIFI_IF_NAN;
    req->subtype = (WLAN_FC_STYPE_AUTH << 4);
    req->data_len = body_len;
    os_memcpy(req->da, da, ETH_ALEN);
    os_memcpy(req->data, body, body_len);

    ret = esp_wifi_send_mgmt_frm_internal(req);

    os_free(req);

    return (ret == 0) ? 0 : -1;
}

struct nan_pasn_data *nan_pasn_data_init(void)
{
    struct nan_pasn_data *pd;
    struct nan_config *cfg;

    pd = os_zalloc(sizeof(*pd));
    if (!pd) {
        return NULL;
    }

    cfg = os_zalloc(sizeof(*cfg));
    if (!cfg) {
        os_free(pd);
        return NULL;
    }

    esp_wifi_get_mac(WIFI_IF_NAN, cfg->dev_addr);

    cfg->pasn_type = 0x03;
    cfg->cb_ctx = pd;
    cfg->set_pmksa = NULL;
    cfg->pasn_send_mgmt = nan_pasn_esp_send_mgmt;
    cfg->prepare_data_element = NULL;
    cfg->parse_data_element = NULL;
    cfg->pasn_validate_pmkid = NULL;

    pd->cfg = cfg;
    pd->initiator_pmksa = pasn_initiator_pmksa_cache_init();
    pd->responder_pmksa = pasn_responder_pmksa_cache_init();
    pd->pasn = NULL;

    /* Keep a deterministic decimal PIN unless caller overrides it later. */
    if (nan_set_dev_sae_pin(pd, NAN_DEFAULT_PAIRING_PIN) != 0) {
        os_free(cfg);
        os_free(pd);
        return NULL;
    }

    return pd;
}

void nan_pasn_data_deinit(struct nan_pasn_data *pd)
{
    if (!pd) {
        return;
    }

    nan_pasn_auth_timeout_cancel(pd);

    forced_memzero(pd->dev_sae_pin, sizeof(pd->dev_sae_pin));
    pd->dev_sae_pin_len = 0;

    if (pd == esp_nan_app_get_pasn_data()) {
        esp_nan_app_set_pasn_data(NULL);
    }

    if (pd->pasn) {
        /* Reset must run before pasn_data_deinit: deinit frees pasn (UAF if reversed). */
        wpa_pasn_reset(pd->pasn);
        pasn_data_deinit(pd->pasn);
        pd->pasn = NULL;
    }
    if (pd->initiator_pmksa) {
        pasn_initiator_pmksa_cache_deinit(pd->initiator_pmksa);
        pd->initiator_pmksa = NULL;
    }
    if (pd->responder_pmksa) {
        pasn_responder_pmksa_cache_deinit(pd->responder_pmksa);
        pd->responder_pmksa = NULL;
    }
    os_free(pd->cfg);
    pd->cfg = NULL;
    os_free(pd);
}

int nan_pasn_auth_initiate(struct nan_pasn_data *pd, const uint8_t *peer_addr, int freq)
{
    if (!pd || !peer_addr) {
        return -1;
    }
    return nan_initiate_pasn_auth(pd, peer_addr, freq);
}

#define NAN_PASN_VERIFY_ELOOP_SSID_MAX 32

struct nan_pasn_eloop_ctx {
    uint8_t peer_addr[ETH_ALEN];
    uint32_t pincode;
    esp_nan_pairing_key_installed_cb_t pairing_key_installed_cb;
};

static void nan_pasn_auth_eloop_cb(void *eloop_ctx, void *user_data)
{
    struct nan_pasn_eloop_ctx *ctx = user_data;
    struct nan_pasn_data *old;
    struct nan_pasn_data *pd;
    char pin_digits[16];
    int n;
    const char *pin_to_apply = NULL;

    (void)eloop_ctx;
    if (!ctx) {
        return;
    }

    old = esp_nan_app_get_pasn_data();
    if (old) {
        nan_pasn_data_deinit(old);
    }

    pd = nan_pasn_data_init();
    if (!pd) {
        os_free(ctx);
        return;
    }

    esp_nan_app_set_pasn_data(pd);
    pd->pairing_key_installed_cb = ctx->pairing_key_installed_cb;

    if (ctx->pincode != UINT32_MAX) {
        n = os_snprintf(pin_digits, sizeof(pin_digits), "%06u",
                        (unsigned)(ctx->pincode % 1000000U));
        if (os_snprintf_error(sizeof(pin_digits), n)) {
            nan_pasn_data_deinit(pd);
            os_free(ctx);
            return;
        }
        pin_to_apply = pin_digits;
    }

    if (pin_to_apply && nan_set_dev_sae_pin(pd, pin_to_apply) != 0) {
        nan_pasn_data_deinit(pd);
        os_free(ctx);
        return;
    }

    {
        int freq = nan_pasn_get_current_freq_mhz();

        if (freq <= 0) {
            freq = 2412;
        }
        nan_pasn_auth_initiate(pd, ctx->peer_addr, freq);
    }
    os_free(ctx);
}

int esp_nan_supp_pasn_initiator_auth(const uint8_t *peer_nmi, uint32_t pincode,
                                     esp_nan_pairing_key_installed_cb_t pairing_key_installed_cb)
{
    struct nan_pasn_eloop_ctx *ctx;

    ctx = os_zalloc(sizeof(*ctx));
    if (!ctx || !peer_nmi) {
        return -1;
    }

    os_memcpy(ctx->peer_addr, peer_nmi, ETH_ALEN);
    ctx->pincode = pincode;
    ctx->pairing_key_installed_cb = pairing_key_installed_cb;

    if (eloop_register_timeout(0, 0, nan_pasn_auth_eloop_cb, NULL, ctx) != 0) {
        os_free(ctx);
        return -1;
    }

    return 0;
}

struct nan_pasn_verify_eloop_ctx {
    uint8_t peer_addr[ETH_ALEN];
    int freq;
    int role;
    uint8_t bssid[ETH_ALEN];
    uint8_t ssid[NAN_PASN_VERIFY_ELOOP_SSID_MAX];
    size_t ssid_len;
};

static void nan_pasn_verify_eloop_cb(void *eloop_ctx, void *user_data)
{
    struct nan_pasn_verify_eloop_ctx *ctx = user_data;
    struct nan_pasn_data *pd;
    const uint8_t *ssid_arg;

    (void)eloop_ctx;
    if (!ctx) {
        return;
    }

    pd = esp_nan_app_get_pasn_data();
    if (!pd) {
        os_free(ctx);
        return;
    }

    ssid_arg = ctx->ssid_len ? ctx->ssid : NULL;
    nan_initiate_pasn_verify(pd, ctx->peer_addr, ctx->freq, ctx->role,
                             ctx->bssid, ssid_arg, ctx->ssid_len);
    os_free(ctx);
}

int nan_pasn_verify_eloop(unsigned int secs, unsigned int usecs,
                          const uint8_t *peer_addr, int freq, int role,
                          const uint8_t *bssid,
                          const uint8_t *ssid, size_t ssid_len)
{
    struct nan_pasn_verify_eloop_ctx *ctx;

    if (!peer_addr) {
        return -1;
    }
    if (ssid_len > NAN_PASN_VERIFY_ELOOP_SSID_MAX) {
        return -1;
    }

    ctx = os_zalloc(sizeof(*ctx));
    if (!ctx) {
        return -1;
    }

    os_memcpy(ctx->peer_addr, peer_addr, ETH_ALEN);
    ctx->freq = freq;
    ctx->role = role;
    if (bssid) {
        os_memcpy(ctx->bssid, bssid, ETH_ALEN);
    } else {
        os_memcpy(ctx->bssid, peer_addr, ETH_ALEN);
    }
    if (ssid && ssid_len) {
        os_memcpy(ctx->ssid, ssid, ssid_len);
        ctx->ssid_len = ssid_len;
    }

    if (eloop_register_timeout(secs, usecs, nan_pasn_verify_eloop_cb, NULL, ctx) != 0) {
        os_free(ctx);
        return -1;
    }

    return 0;
}

/**
 * pasn_responder_init - Set up NAN PASN responder state for incoming Auth frames.
 *
 * Calls nan_pasn_data_init(), esp_nan_app_set_pasn_data(), optional PIN override,
 * role @c NAN_ROLE_PAIRING_RESPONDER, and nan_pasn_initialize().
 *
 * @param peer_addr Peer NAN address, or NULL to use a broadcast placeholder until Auth1.
 * @param pincode 6-digit value 0..999999, or @c UINT32_MAX to keep default PIN from @ref nan_pasn_data_init.
 * Returns 0 on success, -1 on failure.
 */
int pasn_responder_init(const uint8_t *peer_addr, uint32_t pincode)
{
    struct nan_pasn_data *pd;
    struct nan_pasn_data *old;
    char pin_digits[16];
    static const u8 bcast[ETH_ALEN] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    const u8 *peer = peer_addr ? peer_addr : bcast;
    const char *pin_to_apply = NULL;
    int n;
    int freq;

    os_memset(pin_digits, 0, sizeof(pin_digits));

    old = esp_nan_app_get_pasn_data();
    if (old) {
        nan_pasn_data_deinit(old);
    }

    pd = nan_pasn_data_init();
    if (!pd) {
        return -1;
    }

    esp_nan_app_set_pasn_data(pd);

    if (pincode != UINT32_MAX) {
        n = os_snprintf(pin_digits, sizeof(pin_digits), "%06u",
                        (unsigned)(pincode % 1000000U));
        if (os_snprintf_error(sizeof(pin_digits), n)) {
            goto fail;
        }
        pin_to_apply = pin_digits;
    }

    if (pin_to_apply && nan_set_dev_sae_pin(pd, pin_to_apply) != 0) {
        goto fail;
    }

    freq = nan_pasn_get_current_freq_mhz();
    if (freq <= 0) {
        freq = 2412;
    }

    pd->dev_role = NAN_ROLE_PAIRING_RESPONDER;
    nan_pasn_initialize(pd, peer, freq, false, true);

    if (!pd->pasn || nan_prepare_pasn_extra_ie(pd, pd->pasn, NULL, false) != 0) {
        goto fail;
    }

    return 0;

fail:
    nan_pasn_data_deinit(pd);
    return -1;
}

struct pasn_responder_eloop_ctx {
    uint8_t peer_addr[ETH_ALEN];
    uint32_t pincode;
    esp_nan_pairing_key_installed_cb_t pairing_key_installed_cb;
};

static void pasn_responder_init_eloop_cb(void *eloop_ctx, void *user_data)
{
    struct pasn_responder_eloop_ctx *ctx = user_data;
    struct nan_pasn_data *pd;

    (void)eloop_ctx;
    if (!ctx) {
        return;
    }
    if (pasn_responder_init(ctx->peer_addr, ctx->pincode) == 0) {
        pd = esp_nan_app_get_pasn_data();
        if (pd) {
            pd->pairing_key_installed_cb = ctx->pairing_key_installed_cb;
        }
    }
    os_free(ctx);
}

int esp_nan_supp_pasn_responder_init(const uint8_t *peer_nmi, uint32_t pincode,
                                     esp_nan_pairing_key_installed_cb_t pairing_key_installed_cb)
{
    struct pasn_responder_eloop_ctx *ctx;

    ctx = os_zalloc(sizeof(*ctx));
    if (!ctx || !peer_nmi) {
        return -1;
    }

    ctx->pincode = pincode;
    os_memcpy(ctx->peer_addr, peer_nmi, ETH_ALEN);
    ctx->pairing_key_installed_cb = pairing_key_installed_cb;

    if (eloop_register_timeout(0, 0, pasn_responder_init_eloop_cb, NULL, ctx) != 0) {
        os_free(ctx);
        return -1;
    }

    return 0;
}

#endif /* CONFIG_ESP_WIFI_NAN_PAIRING */
