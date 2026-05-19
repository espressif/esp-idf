/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * NAN PASN: PASN pairing and authentication for Wi-Fi NAN.
 */

#include "sdkconfig.h"
#include "nan_pasn.h"

#if CONFIG_ESP_WIFI_PASN_SUPPORT

#include "utils/includes.h"
#include "utils/common.h"
#include "common/ieee802_11_defs.h"
#include "common/nan.h"
#include "esp_wifi_driver.h"
#include "crypto/crypto.h"

#include "pasn/pasn_common.h"
#include "common/wpa_common.h"
#include "apps_private/wifi_apps_private.h"

#include "includes.h"
#include "wpa_supplicant_i.h"
#include "utils/eloop.h"
#include "esp_err.h"
#include "esp_wifi.h"
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
static void nan_pasn_install_nan_pairwise_tk(struct pasn_data *pasn)
{
    struct wpa_ptk *ptk;
    uint8_t key_rsc[8] = {0};
    uint8_t peer[ETH_ALEN];
    int kret;

    if (!pasn) {
        return;
    }

    if (pasn->cipher != WPA_CIPHER_CCMP) {
        wpa_printf(MSG_INFO, "NAN PASN: skip NAN TK install (cipher=%d)", pasn->cipher);
        return;
    }

    ptk = pasn_get_ptk(pasn);
    if (!ptk || !ptk->tk_len || ptk->tk_len > sizeof(ptk->tk)) {
        return;
    }

    os_memcpy(peer, pasn->peer_addr, ETH_ALEN);
    wpa_hexdump_key(MSG_INFO, "NAN PASN: TK before esp_wifi_set_nan_key_internal",
                    ptk->tk, ptk->tk_len);
    kret = esp_wifi_set_nan_key_internal(
               NAN_PASN_WIFI_ALG_CCMP, peer, temp, 1, key_rsc, sizeof(key_rsc),
               ptk->tk, ptk->tk_len,
               NAN_PASN_KEY_FLAG_PAIRWISE | NAN_PASN_KEY_FLAG_RX | NAN_PASN_KEY_FLAG_TX);
    if (kret != 0) {
        wpa_printf(MSG_WARNING, "NAN PASN: esp_wifi_set_nan_key_internal failed (%d)",
                   kret);
    }
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
 * Common path for @ref esp_nan_app_post_pasn_pairing_confirm (initiator and responder).
 *
 * @param auth_frame_status_code  IEEE 802.11 Authentication @c status_code from the RX frame (host endian).
 * @param initiator_nmi           Initiator NMI (6 octets).
 * @param responder_nmi           Responder NMI (6 octets).
 * @param require_pasn_internal_success  If true, post only when @c pasn->status is @c WLAN_STATUS_SUCCESS
 *                                       (initiator after Auth2). Responder uses false.
 */
static void nan_pasn_post_pasn_pairing_confirm_evt(struct nan_pasn_data *nan,
                                                   struct pasn_data *pasn,
                                                   u16 auth_frame_status_code,
                                                   const u8 initiator_nmi[ETH_ALEN],
                                                   const u8 responder_nmi[ETH_ALEN],
                                                   bool require_pasn_internal_success)
{
#if 0
    wifi_event_nan_pasn_pairing_confirm_t conf;

    if (!nan || !pasn) {
        return;
    }
    if (require_pasn_internal_success && pasn->status != WLAN_STATUS_SUCCESS) {
        return;
    }

    os_memset(&conf, 0, sizeof(conf));
    conf.type = WIFI_NAN_PASN_PAIRING_IND_TYPE_SETUP;
    conf.status = WIFI_NAN_PASN_PAIRING_CONFIRM_STATUS_ACCEPTED;
    conf.self_handle = 0;
    conf.reason_code =
        (auth_frame_status_code == WLAN_STATUS_SUCCESS) ? 0
        : (uint8_t)auth_frame_status_code;
    os_memcpy(conf.initiator_nan_address, initiator_nmi, ETH_ALEN);
    os_memcpy(conf.responder_nan_address, responder_nmi, ETH_ALEN);
    conf.paired_peer_handle_valid = 0;
    conf.auth_password = nan->dev_sae_pin_len > 0 ? 1 : 0;
    conf.auth_opportunistic =
        (pasn->akmp == WPA_KEY_MGMT_PASN && nan->dev_sae_pin_len == 0) ? 1 : 0;
    conf.npk_nik_caching = 0;
    esp_nan_app_post_pasn_pairing_confirm(&conf);
#endif
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

    if (nan_pasn_flatten_ptk_blob(ptk, g_nan_pasn_saved_keys.ptk_blob,
                                  sizeof(g_nan_pasn_saved_keys.ptk_blob),
                                  &g_nan_pasn_saved_keys.ptk_blob_len) != 0) {
        forced_memzero(&g_nan_pasn_saved_keys, sizeof(g_nan_pasn_saved_keys));
        return;
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

static const char * __attribute__((unused))
nan_pasn_role_to_str(enum nan_role role)
{
    switch (role) {
    case NAN_ROLE_PAIRING_INITIATOR:
        return "initiator";
    case NAN_ROLE_PAIRING_RESPONDER:
        return "responder";
    case NAN_ROLE_IDLE:
    default:
        return "idle";
    }
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
               nan_pasn_role_to_str((enum nan_role)(uintptr_t)user_data),
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

    if (!derive_kek) {
        pasn->derive_kek = false;
        pasn->kek_len = 0;
    }

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
        nan_pasn_post_pasn_pairing_confirm_evt(
            nan, pasn, le_to_host16(mgmt->auth.status_code),
            mgmt->sa, nan->cfg->dev_addr, false);
#ifdef CONFIG_TESTING_OPTIONS
        nan_pasn_store_ptk(nan, &pasn->ptk);
#endif /* CONFIG_TESTING_OPTIONS */
        nan_pasn_copy_keys_from_pasn(nan, pasn);
        nan_pasn_install_nan_pairwise_tk(pasn);
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
        } else {
            nan_pasn_post_pasn_pairing_confirm_evt(
                nan, pasn, le_to_host16(mgmt->auth.status_code),
                pasn->own_addr, pasn->peer_addr, true);
            nan_pasn_copy_keys_from_pasn(nan, pasn);
            nan_pasn_install_nan_pairwise_tk(pasn);
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

int nan_pasn_auth_eloop(const uint8_t *peer_addr, uint32_t pincode)
{
    struct nan_pasn_eloop_ctx *ctx;

    if (!peer_addr) {
        return -1;
    }

    ctx = os_zalloc(sizeof(*ctx));
    if (!ctx) {
        return -1;
    }

    os_memcpy(ctx->peer_addr, peer_addr, ETH_ALEN);
    ctx->pincode = pincode;

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
    unsigned int has_peer;
    uint32_t pincode;
};

static void pasn_responder_init_eloop_cb(void *eloop_ctx, void *user_data)
{
    struct pasn_responder_eloop_ctx *ctx = user_data;

    (void)eloop_ctx;
    if (!ctx) {
        return;
    }
    pasn_responder_init(ctx->has_peer ? ctx->peer_addr : NULL, ctx->pincode);
    os_free(ctx);
}

int pasn_responder_init_eloop(const uint8_t *peer_addr, uint32_t pincode)
{
    struct pasn_responder_eloop_ctx *ctx;

    ctx = os_zalloc(sizeof(*ctx));
    if (!ctx) {
        return -1;
    }

    ctx->pincode = pincode;
    if (peer_addr) {
        ctx->has_peer = 1;
        os_memcpy(ctx->peer_addr, peer_addr, ETH_ALEN);
    }

    if (eloop_register_timeout(0, 0, pasn_responder_init_eloop_cb, NULL, ctx) != 0) {
        os_free(ctx);
        return -1;
    }

    return 0;
}

#else /* !CONFIG_ESP_WIFI_PASN_SUPPORT */

void handle_auth_pasn(uint8_t *buf, size_t len, uint16_t trans_seq, uint16_t status)
{
    (void)buf;
    (void)len;
    (void)trans_seq;
    (void)status;
}

int nan_initiate_pasn_verify(struct nan_pasn_data *pd, const uint8_t *peer_addr,
                             int freq, int role, const uint8_t *bssid,
                             const uint8_t *ssid, size_t ssid_len)
{
    (void)pd;
    (void)peer_addr;
    (void)freq;
    (void)role;
    (void)bssid;
    (void)ssid;
    (void)ssid_len;
    return -1;
}

int nan_initiate_pasn_auth(struct nan_pasn_data *pd, const uint8_t *addr, int freq)
{
    (void)pd;
    (void)addr;
    (void)freq;
    return -1;
}

struct nan_pasn_data *nan_pasn_data_init(void)
{
    return NULL;
}

void nan_pasn_data_deinit(struct nan_pasn_data *pd)
{
    (void)pd;
}

int nan_pasn_auth_initiate(struct nan_pasn_data *pd, const uint8_t *peer_addr, int freq)
{
    (void)pd;
    (void)peer_addr;
    (void)freq;
    return -1;
}

int nan_pasn_auth(struct nan_pasn_data **pd_out, const uint8_t *peer_addr, int freq)
{
    (void)pd_out;
    (void)peer_addr;
    (void)freq;
    return -1;
}

int nan_pasn_auth_eloop(const uint8_t *peer_addr, uint32_t pincode)
{
    (void)peer_addr;
    (void)pincode;
    return -1;
}

int nan_pasn_verify_eloop(unsigned int secs, unsigned int usecs,
                          const uint8_t *peer_addr, int freq, int role,
                          const uint8_t *bssid,
                          const uint8_t *ssid, size_t ssid_len)
{
    (void)secs;
    (void)usecs;
    (void)peer_addr;
    (void)freq;
    (void)role;
    (void)bssid;
    (void)ssid;
    (void)ssid_len;
    return -1;
}

int pasn_responder_init(const uint8_t *peer_addr, uint32_t pincode)
{
    (void)peer_addr;
    (void)pincode;
    return -1;
}

int pasn_responder_init_eloop(const uint8_t *peer_addr, uint32_t pincode)
{
    (void)peer_addr;
    (void)pincode;
    return -1;
}

const struct nan_pasn_key_material *nan_pasn_get_saved_keys(void)
{
    return NULL;
}

void nan_pasn_clear_saved_keys(void)
{
}

#endif /* CONFIG_ESP_WIFI_PASN_SUPPORT */
