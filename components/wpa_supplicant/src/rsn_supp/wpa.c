/*
 * WPA Supplicant - WPA state machine and EAPOL-Key processing
 * Copyright (c) 2003-2010, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#include "utils/includes.h"

#include "utils/common.h"
#include "utils/eloop.h"
#include "rsn_supp/wpa.h"
#include "rsn_supp/pmksa_cache.h"
#include "rsn_supp/wpa_i.h"
#include "common/eapol_common.h"
#include "common/ieee802_11_defs.h"
#include "rsn_supp/wpa_ie.h"
#include "esp_wpas_glue.h"
#include "esp_wifi_driver.h"

#include "crypto/crypto.h"
#include "crypto/sha1.h"
#include "crypto/aes_wrap.h"
#include "crypto/ccmp.h"
#include "crypto/sha256.h"
#include "esp_rom_sys.h"
#include "common/bss.h"
#include "esp_common_i.h"
#include "esp_owe_i.h"
#include "common/sae.h"
#include "esp_eap_client_i.h"

/**
 * eapol_sm_notify_eap_success - Notification of external EAP success trigger
 * @sm: Pointer to EAPOL state machine allocated with eapol_sm_init()
 * @success: %TRUE = set success, %FALSE = clear success
 *
 * Notify the EAPOL state machine that external event has forced EAP state to
 * success (success = %TRUE). This can be cleared by setting success = %FALSE.
 *
 * This function is called to update EAP state when WPA-PSK key handshake has
 * been completed successfully since WPA-PSK does not use EAP state machine.
 */

#define WPA_4_4_HANDSHAKE_BIT   (1<<13)
#define WPA_GROUP_HANDSHAKE_BIT (1<<14)
struct wpa_sm gWpaSm;
/* fix buf for tx for now */
#define WPA_TX_MSG_BUFF_MAXLEN 200
#define MIN_DH_LEN(len) (len < 4)

#define ASSOC_IE_LEN 24 + 2 + PMKID_LEN + RSN_SELECTOR_LEN
#define MAX_EAPOL_RETRIES 3
u8 assoc_ie_buf[ASSOC_IE_LEN+2];

void set_assoc_ie(u8 * assoc_buf);

static int wpa_sm_get_key(uint8_t *ifx, int *alg, u8 *addr, int *key_idx, u8 *key, size_t key_len, enum key_flag key_flag);

void wpa_set_passphrase(char * passphrase, u8 *ssid, size_t ssid_len);

void wpa_sm_set_pmk_from_pmksa(struct wpa_sm *sm);
static bool wpa_supplicant_gtk_in_use(struct wpa_sm *sm, struct wpa_gtk_data *gd);
void wpa_supplicant_stop_countermeasures(void *data, void *user_ctx);
static inline enum wpa_states   wpa_sm_get_state(struct wpa_sm *sm)
{
    return sm->wpa_state;;
}

static inline void   wpa_sm_cancel_auth_timeout(struct wpa_sm *sm)
{

}

void   eapol_sm_notify_eap_success(Boolean success)
{

}

wifi_cipher_type_t cipher_type_map_supp_to_public(unsigned wpa_cipher)
{
    switch (wpa_cipher) {
    case WPA_CIPHER_NONE:
        return WIFI_CIPHER_TYPE_NONE;

    case WPA_CIPHER_WEP40:
        return WIFI_CIPHER_TYPE_WEP40;

    case WPA_CIPHER_WEP104:
        return WIFI_CIPHER_TYPE_WEP104;

    case WPA_CIPHER_TKIP:
        return WIFI_CIPHER_TYPE_TKIP;

    case WPA_CIPHER_CCMP:
        return WIFI_CIPHER_TYPE_CCMP;

    case WPA_CIPHER_CCMP|WPA_CIPHER_TKIP:
        return WIFI_CIPHER_TYPE_TKIP_CCMP;

    case WPA_CIPHER_AES_128_CMAC:
        return WIFI_CIPHER_TYPE_AES_CMAC128;

    case WPA_CIPHER_SMS4:
        return WIFI_CIPHER_TYPE_SMS4;

    case WPA_CIPHER_GCMP:
        return WIFI_CIPHER_TYPE_GCMP;

    case WPA_CIPHER_GCMP_256:
        return WIFI_CIPHER_TYPE_GCMP256;

    default:
        return WIFI_CIPHER_TYPE_UNKNOWN;
    }
}

unsigned cipher_type_map_public_to_supp(wifi_cipher_type_t cipher)
{
    switch (cipher) {
    case WIFI_CIPHER_TYPE_NONE:
        return WPA_CIPHER_NONE;

    case WIFI_CIPHER_TYPE_WEP40:
        return WPA_CIPHER_WEP40;

    case WIFI_CIPHER_TYPE_WEP104:
        return WPA_CIPHER_WEP104;

    case WIFI_CIPHER_TYPE_TKIP:
        return WPA_CIPHER_TKIP;

    case WIFI_CIPHER_TYPE_CCMP:
        return WPA_CIPHER_CCMP;

#ifdef CONFIG_GCMP
    case WIFI_CIPHER_TYPE_GCMP:
        return WPA_CIPHER_GCMP;

    case WIFI_CIPHER_TYPE_GCMP256:
        return WPA_CIPHER_GCMP_256;
#endif

    case WIFI_CIPHER_TYPE_TKIP_CCMP:
        return WPA_CIPHER_CCMP|WPA_CIPHER_TKIP;

    case WIFI_CIPHER_TYPE_AES_CMAC128:
        return WPA_CIPHER_AES_128_CMAC;

#ifdef CONFIG_GMAC
    case WIFI_CIPHER_TYPE_AES_GMAC128:
        return WPA_CIPHER_BIP_GMAC_128;

    case WIFI_CIPHER_TYPE_AES_GMAC256:
        return WPA_CIPHER_BIP_GMAC_256;
#endif

    case WIFI_CIPHER_TYPE_SMS4:
        return WPA_CIPHER_SMS4;

    default:
        return WPA_CIPHER_NONE;
    }
}

#ifdef CONFIG_ESP_WIFI_ENTERPRISE_SUPPORT
static bool is_wpa2_enterprise_connection(void)
{
    uint8_t authmode;

    if (esp_wifi_sta_prof_is_wpa2_internal()) {
        authmode = esp_wifi_sta_get_prof_authmode_internal();
        if ((authmode == WPA2_AUTH_ENT) ||
            (authmode == WPA2_AUTH_ENT_SHA256) ||
            (authmode == WPA2_AUTH_ENT_SHA384_SUITE_B)) {
            return true;
        }
    }

    return false;
}
#endif

const u8 * wpa_sm_get_auth_addr(struct wpa_sm *sm)
{
    return sm->bssid;
}

 /*
 * wpa_sm_ether_send - Send Ethernet frame
 * @wpa_s: Pointer to wpa_supplicant data
 * @dest: Destination MAC address
 * @proto: Ethertype in host byte order
 * @buf: Frame payload starting from IEEE 802.1X header
 * @len: Frame payload length
 * Returns: >=0 on success, <0 on failure
 */
static inline int wpa_sm_ether_send(struct wpa_sm *sm, const u8 *dest, u16 proto,
                                    const u8 *data, size_t data_len)
{
    return wpa_ether_send(sm, dest, proto, data, data_len);
}

/**
 * wpa_eapol_key_send - Send WPA/RSN EAPOL-Key message
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @kck: Key Confirmation Key (KCK, part of PTK)
 * @kck_len: KCK length in octets
 * @ver: Version field from Key Info
 * @dest: Destination address for the frame
 * @proto: Ethertype (usually ETH_P_EAPOL)
 * @msg: EAPOL-Key message
 * @msg_len: Length of message
 * @key_mic: Pointer to the buffer to which the EAPOL-Key MIC is written
 */
int  wpa_eapol_key_send(struct wpa_sm *sm, const u8 *kck, size_t kck_len,
                        int ver, const u8 *dest, u16 proto,
                        u8 *msg, size_t msg_len, u8 *key_mic)
{
    int ret = -1;
    if (is_zero_ether_addr(dest) && is_zero_ether_addr(sm->bssid)) {
#ifndef ESP_SUPPLICANT
        /*
         * Association event was not yet received; try to fetch
         * BSSID from the driver.
         */
        if (wpa_sm_get_bssid(sm, sm->bssid) < 0) {
            wpa_printf(MSG_DEBUG, "WPA: Failed to read BSSID for "
                   "EAPOL-Key destination address");
        } else {
            dest = sm->bssid;
            wpa_printf(MSG_DEBUG, "WPA: Use BSSID (" MACSTR
                   ") as the destination for EAPOL-Key",
                   MAC2STR(dest));
        }
#else
        return ret;
#endif
    }
    if (key_mic &&
        wpa_eapol_key_mic(kck, kck_len, sm->key_mgmt, ver, msg, msg_len,
                  key_mic)) {
        wpa_msg(NULL, MSG_ERROR,
            "WPA: Failed to generate EAPOL-Key version %d key_mgmt 0x%x MIC",
            ver, sm->key_mgmt);
        goto out;
    }
    wpa_hexdump_key(MSG_DEBUG, "WPA: KCK", kck, kck_len);
    wpa_hexdump(MSG_DEBUG, "WPA: Derived Key MIC", key_mic, wpa_mic_len(sm->key_mgmt, sm->pmk_len));
    wpa_hexdump(MSG_MSGDUMP, "WPA: TX EAPOL-Key", msg, msg_len);
    return wpa_sm_ether_send(sm, dest, proto, msg, msg_len);
out:
    return ret;
}

/**
 * wpa_sm_key_request - Send EAPOL-Key Request
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @error: Indicate whether this is an Michael MIC error report
 * @pairwise: 1 = error report for pairwise packet, 0 = for group packet
 *
 * Send an EAPOL-Key Request to the current authenticator. This function is
 * used to request rekeying and it is usually called when a local Michael MIC
 * failure is detected.
 */
static void wpa_sm_key_request(struct wpa_sm *sm, int error, int pairwise)
{
    size_t mic_len, hdrlen, rlen;
    struct wpa_eapol_key *reply;
    struct wpa_eapol_key_192 *reply192;
    int key_info, ver;
    u8 *rbuf, *key_mic;

    if (sm->key_mgmt == WPA_KEY_MGMT_OSEN || wpa_key_mgmt_suite_b(sm->key_mgmt))
        ver = WPA_KEY_INFO_TYPE_AKM_DEFINED;
    else if (wpa_key_mgmt_ft(sm->key_mgmt) || wpa_key_mgmt_sha256(sm->key_mgmt))
        ver = WPA_KEY_INFO_TYPE_AES_128_CMAC;
    else if (sm->pairwise_cipher != WPA_CIPHER_TKIP)
        ver = WPA_KEY_INFO_TYPE_HMAC_SHA1_AES;
    else if (sm->key_mgmt == WPA_KEY_MGMT_SAE || sm->key_mgmt == WPA_KEY_MGMT_OWE)
        ver = 0;
    else
        ver = WPA_KEY_INFO_TYPE_HMAC_MD5_RC4;

    mic_len = wpa_mic_len(sm->key_mgmt, sm->pmk_len);
    hdrlen = mic_len == 24 ? sizeof(*reply192) : sizeof(*reply);
    rbuf = wpa_sm_alloc_eapol(sm, IEEE802_1X_TYPE_EAPOL_KEY, NULL,
                  hdrlen, &rlen, (void *) &reply);
    if (rbuf == NULL)
        return;

    reply192 = (struct wpa_eapol_key_192 *) reply;

    reply->type = sm->proto == WPA_PROTO_RSN ?
        EAPOL_KEY_TYPE_RSN : EAPOL_KEY_TYPE_WPA;
    key_info = WPA_KEY_INFO_REQUEST | ver;
    if (sm->ptk_set)
        key_info |= WPA_KEY_INFO_SECURE;
    if (sm->ptk_set && mic_len)
        key_info |= WPA_KEY_INFO_MIC;
    if (error)
        key_info |= WPA_KEY_INFO_ERROR;
    if (pairwise)
        key_info |= WPA_KEY_INFO_KEY_TYPE;

    WPA_PUT_BE16(reply->key_info, key_info);
    WPA_PUT_BE16(reply->key_length, 0);

    os_memcpy(reply->replay_counter, sm->request_counter,
          WPA_REPLAY_COUNTER_LEN);
    inc_byte_array(sm->request_counter, WPA_REPLAY_COUNTER_LEN);
    if (mic_len == 24)
        WPA_PUT_BE16(reply192->key_data_length, 0);
    else
        WPA_PUT_BE16(reply->key_data_length, 0);

    if (!(key_info & WPA_KEY_INFO_MIC))
        key_mic = NULL;
    else
        key_mic = reply192->key_mic; /* same offset in reply */


    wpa_printf(MSG_DEBUG, "WPA: Sending EAPOL-Key Request (error=%d "
           "pairwise=%d ptk_set=%d len=%lu)",
           error, pairwise, sm->ptk_set, (unsigned long) rlen);
    wpa_eapol_key_send(sm, sm->ptk.kck, sm->ptk.kck_len, ver, wpa_sm_get_auth_addr(sm),
                       ETH_P_EAPOL, rbuf, rlen, key_mic);
    wpa_sm_free_eapol(rbuf);
}

static void wpa_sm_pmksa_free_cb(struct rsn_pmksa_cache_entry *entry,
        void *ctx, enum pmksa_free_reason reason)
{
    struct wpa_sm *sm = ctx;
    int deauth = 0;

    wpa_printf( MSG_DEBUG, "RSN: PMKSA cache entry free_cb: "
            MACSTR " reason=%d", MAC2STR(entry->aa), reason);

    if (sm->cur_pmksa == entry) {
        wpa_printf( MSG_DEBUG,
                "RSN: %s current PMKSA entry",
                reason == PMKSA_REPLACE ? "replaced" : "removed");
        pmksa_cache_clear_current(sm);

        /*
         * If an entry is simply being replaced, there's no need to
         * deauthenticate because it will be immediately re-added.
         * This happens when EAP authentication is completed again
         * (reauth or failed PMKSA caching attempt).
         * */
        if (reason != PMKSA_REPLACE)
            deauth = 1;
    }

    if (reason == PMKSA_EXPIRE &&
            (sm->pmk_len == entry->pmk_len &&
             os_memcmp(sm->pmk, entry->pmk, sm->pmk_len) == 0)) {
        wpa_printf( MSG_DEBUG,
                "RSN: deauthenticating due to expired PMK");
        pmksa_cache_clear_current(sm);
        deauth = 1;
    }

    if (deauth) {
        os_memset(sm->pmk, 0, sizeof(sm->pmk));
        wpa_sm_deauthenticate(sm, WLAN_REASON_UNSPECIFIED);
    }
}




static int wpa_supplicant_get_pmk(struct wpa_sm *sm,
        const unsigned char *src_addr,
        const u8 *pmkid)
{
    int abort_cached = 0;

    if (pmkid && !sm->cur_pmksa) {
        /* When using drivers that generate RSN IE, wpa_supplicant may
         * not have enough time to get the association information
         * event before receiving this 1/4 message, so try to find a
         * matching PMKSA cache entry here. */
        sm->cur_pmksa = pmksa_cache_get(sm->pmksa, src_addr, pmkid,
                NULL);
        if (sm->cur_pmksa) {
            wpa_printf(MSG_DEBUG,
                    "RSN: found matching PMKID from PMKSA cache");
        } else {
            wpa_printf( MSG_DEBUG,
                    "RSN: no matching PMKID found");
            abort_cached = 1;
        }
    }

    if (pmkid && sm->cur_pmksa &&
            os_memcmp_const(pmkid, sm->cur_pmksa->pmkid, PMKID_LEN) == 0) {

        wpa_hexdump(MSG_DEBUG, "RSN: matched PMKID", pmkid, PMKID_LEN);
        wpa_sm_set_pmk_from_pmksa(sm);
        wpa_hexdump_key(MSG_DEBUG, "RSN: PMK from PMKSA cache",
                sm->pmk, sm->pmk_len);
        //eapol_sm_notify_cached(sm->eapol);
#ifdef CONFIG_IEEE80211R
        sm->xxkey_len = 0;
#ifdef CONFIG_WPA3_SAE
        if ((sm->key_mgmt == WPA_KEY_MGMT_FT_SAE) &&
            sm->pmk_len == PMK_LEN) {
            /* Need to allow FT key derivation to proceed with
             * PMK from SAE being used as the XXKey in cases where
             * the PMKID in msg 1/4 matches the PMKSA entry that was
             * just added based on SAE authentication for the
             * initial mobility domain association. */
            os_memcpy(sm->xxkey, sm->pmk, sm->pmk_len);
            sm->xxkey_len = sm->pmk_len;
        }
#endif /* CONFIG_WPA3_SAE */

#endif /* CONFIG_IEEE80211R */
    } else if (wpa_key_mgmt_wpa_ieee8021x(sm->key_mgmt)) {
        int res = 0, pmk_len;
        /* For ESP_SUPPLICANT this is already set using wpa_set_pmk*/
        //res = eapol_sm_get_key(sm->eapol, 0, sm->pmk, PMK_LEN);
        if (wpa_key_mgmt_sha384(sm->key_mgmt))
            pmk_len = PMK_LEN_SUITE_B_192;
        else
            pmk_len = PMK_LEN;

        if(!sm->pmk_len) {
            res = -1;
        }

        if (res == 0) {
            struct rsn_pmksa_cache_entry *sa = NULL;
            wpa_hexdump_key(MSG_DEBUG, "WPA: PMK from EAPOL state "
                    "machines", sm->pmk, pmk_len);
            sm->pmk_len = pmk_len;
            //wpa_supplicant_key_mgmt_set_pmk(sm);
            if (sm->proto == WPA_PROTO_RSN &&
                    !wpa_key_mgmt_suite_b(sm->key_mgmt) &&
                    !wpa_key_mgmt_ft(sm->key_mgmt)) {
                sa = pmksa_cache_add(sm->pmksa, sm->pmk, pmk_len,
                                     NULL, NULL, 0, src_addr, sm->own_addr,
                                     sm->network_ctx, sm->key_mgmt);
            }
            if (!sm->cur_pmksa && pmkid &&
                pmksa_cache_get(sm->pmksa, src_addr, pmkid, NULL))
            {
                wpa_printf( MSG_DEBUG,
                    "RSN: the new PMK matches with the "
                    "PMKID");
                abort_cached = 0;
            } else if (sa && !sm->cur_pmksa && pmkid) {
                /*
                 * It looks like the authentication server
                 * derived mismatching MSK. This should not
                 * really happen, but bugs happen.. There is not
                 * much we can do here without knowing what
                 * exactly caused the server to misbehave.
                 */
                wpa_printf( MSG_INFO,
                    "RSN: PMKID mismatch - authentication server may have derived different MSK?!");
                return -1;
            }

            if (!sm->cur_pmksa)
                sm->cur_pmksa = sa;
        } else {
            wpa_printf( MSG_WARNING,
                "WPA: Failed to get master session key from "
                "EAPOL state machines - key handshake "
                "aborted");
            if (sm->cur_pmksa) {
                wpa_printf( MSG_DEBUG,
                    "RSN: Cancelled PMKSA caching "
                    "attempt");
                sm->cur_pmksa = NULL;
                abort_cached = 1;
            } else if (!abort_cached) {
                return -1;
            }
        }
    }

    if (abort_cached && wpa_key_mgmt_wpa_ieee8021x(sm->key_mgmt) &&
        !wpa_key_mgmt_suite_b(sm->key_mgmt) &&
        !wpa_key_mgmt_ft(sm->key_mgmt) && sm->key_mgmt != WPA_KEY_MGMT_OSEN)
    {
        /* Send EAPOL-Start to trigger full EAP authentication. */
        u8 *buf;
        size_t buflen;

        wpa_printf( MSG_DEBUG,
            "RSN: no PMKSA entry found - trigger "
            "full EAP authentication");
        buf = wpa_sm_alloc_eapol(sm, IEEE802_1X_TYPE_EAPOL_START,
                     NULL, 0, &buflen, NULL);
        if (buf) {
            wpa_sm_ether_send(sm, sm->bssid, ETH_P_EAPOL,
                      buf, buflen);
            wpa_sm_free_eapol(buf);
            return -2;
        }

        return -1;
    }

    return 0;
}


/**
 * wpa_supplicant_send_2_of_4 - Send message 2 of WPA/RSN 4-Way Handshake
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @dst: Destination address for the frame
 * @key: Pointer to the EAPOL-Key frame header
 * @ver: Version bits from EAPOL-Key Key Info
 * @nonce: Nonce value for the EAPOL-Key frame
 * @wpa_ie: WPA/RSN IE
 * @wpa_ie_len: Length of the WPA/RSN IE
 * @ptk: PTK to use for keyed hash and encryption
 * Returns: 0 on success, -1 on failure
 */
int   wpa_supplicant_send_2_of_4(struct wpa_sm *sm, const unsigned char *dst,
                   const struct wpa_eapol_key *key,
                   int ver, const u8 *nonce,
                   const u8 *wpa_ie, size_t wpa_ie_len,
                   struct wpa_ptk *ptk)
{
    size_t mic_len, hdrlen, rlen;
    struct wpa_eapol_key *reply;
    struct wpa_eapol_key_192 *reply192;
    u8 *rsn_ie_buf = NULL;
    u8 *rbuf, *key_mic;

    if (wpa_ie == NULL) {
        wpa_printf(MSG_ERROR, "WPA: No wpa_ie set - cannot "
               "generate msg 2/4");
        return -1;
    }

#ifdef CONFIG_IEEE80211R
    if (wpa_key_mgmt_ft(sm->key_mgmt)) {
        int res;

        wpa_hexdump(MSG_DEBUG, "WPA: WPA IE before FT processing",
                    wpa_ie, wpa_ie_len);
        /*
         * Add PMKR1Name into RSN IE (PMKID-List) and add MDIE and
         * FTIE from (Re)Association Response.
         */
        rsn_ie_buf = os_malloc(wpa_ie_len + 2 + 2 + PMKID_LEN +
                               sm->assoc_resp_ies_len);
        if (rsn_ie_buf == NULL)
                return -1;
        os_memcpy(rsn_ie_buf, wpa_ie, wpa_ie_len);
        res = wpa_insert_pmkid(rsn_ie_buf, &wpa_ie_len,
                               sm->pmk_r1_name);
        if (res < 0) {
            os_free(rsn_ie_buf);
            return -1;
        }
        wpa_hexdump(MSG_DEBUG,
                    "WPA: WPA IE after PMKID[PMKR1Name] addition into RSNE",
                    rsn_ie_buf, wpa_ie_len);

        if (sm->assoc_resp_ies) {
            wpa_hexdump(MSG_DEBUG, "WPA: Add assoc_resp_ies",
                        sm->assoc_resp_ies,
                        sm->assoc_resp_ies_len);
            os_memcpy(rsn_ie_buf + wpa_ie_len, sm->assoc_resp_ies,
                      sm->assoc_resp_ies_len);
            wpa_ie_len += sm->assoc_resp_ies_len;
        }

        wpa_ie = rsn_ie_buf;
    }
#endif /* CONFIG_IEEE80211R */
    wpa_hexdump(MSG_MSGDUMP, "WPA: WPA IE for msg 2/4\n", wpa_ie, wpa_ie_len);

    mic_len = wpa_mic_len(sm->key_mgmt, sm->pmk_len);
    hdrlen = mic_len == 24 ? sizeof(*reply192) : sizeof(*reply);
    rbuf = wpa_sm_alloc_eapol(sm, IEEE802_1X_TYPE_EAPOL_KEY,
                  NULL, hdrlen + wpa_ie_len,
                  &rlen, (void *) &reply);
    if (rbuf == NULL) {
        os_free(rsn_ie_buf);
        return -1;
    }
    reply192 = (struct wpa_eapol_key_192 *) reply;

    reply->type = sm->proto == WPA_PROTO_RSN ?
        EAPOL_KEY_TYPE_RSN : EAPOL_KEY_TYPE_WPA;
    WPA_PUT_BE16(reply->key_info,
             ver | WPA_KEY_INFO_KEY_TYPE | WPA_KEY_INFO_MIC);
    if (sm->proto == WPA_PROTO_RSN)
        WPA_PUT_BE16(reply->key_length, 0);
    else
        memcpy(reply->key_length, key->key_length, 2);

    memcpy(reply->replay_counter, key->replay_counter,
          WPA_REPLAY_COUNTER_LEN);

    key_mic = reply192->key_mic; /* same offset for reply and reply192 */
    if (mic_len == 24) {
        WPA_PUT_BE16(reply192->key_data_length, wpa_ie_len);
        os_memcpy(reply192 + 1, wpa_ie, wpa_ie_len);
    } else {
        WPA_PUT_BE16(reply->key_data_length, wpa_ie_len);
        os_memcpy(reply + 1, wpa_ie, wpa_ie_len);
    }

    os_free(rsn_ie_buf);
    os_memcpy(reply->key_nonce, nonce, WPA_NONCE_LEN);

    wpa_printf(MSG_DEBUG, "WPA Send EAPOL-Key 2/4");

    wpa_eapol_key_send(sm, ptk->kck, ptk->kck_len, ver, dst, ETH_P_EAPOL,
               rbuf, rlen, key_mic);
    wpa_sm_free_eapol(rbuf);

    return 0;
}

static int wpa_derive_ptk(struct wpa_sm *sm, const unsigned char *src_addr,
              const struct wpa_eapol_key *key, struct wpa_ptk *ptk)
{
#ifdef CONFIG_IEEE80211R
    if (wpa_key_mgmt_ft(sm->key_mgmt))
        return wpa_derive_ptk_ft(sm, src_addr, key, ptk);
#endif /* CONFIG_IEEE80211R */
    return wpa_pmk_to_ptk(sm->pmk, sm->pmk_len, "Pairwise key expansion",
                  sm->own_addr, sm->bssid, sm->snonce,
                  key->key_nonce, ptk, sm->key_mgmt,
                  sm->pairwise_cipher);
}

void wpa_supplicant_process_1_of_4(struct wpa_sm *sm,
                      const unsigned char *src_addr,
                      const struct wpa_eapol_key *key,
                      u16 ver, const u8 *key_data,
                      size_t key_data_len)
{
    struct wpa_eapol_ie_parse ie;
    struct wpa_ptk *ptk;
    int res;
    u8 *kde, *kde_buf = NULL;
    size_t kde_len;

#ifdef CONFIG_ESP_WIFI_ENTERPRISE_SUPPORT
    if (is_wpa2_enterprise_connection()) {
        wpa2_ent_eap_state_t state = eap_client_get_eap_state();
        if (state == WPA2_ENT_EAP_STATE_IN_PROGRESS) {
            wpa_printf(MSG_INFO, "EAP Success has not been processed yet."
               " Drop EAPOL message.");
            return;
        }
    }
#endif

    wpa_sm_set_state(WPA_FIRST_HALF_4WAY_HANDSHAKE);

    wpa_printf(MSG_DEBUG, "WPA 1/4-Way Handshake");

    memset(&ie, 0, sizeof(ie));

    if (sm->proto == WPA_PROTO_RSN) {
        /* RSN: msg 1/4 should contain PMKID for the selected PMK */
        wpa_hexdump(MSG_DEBUG, "RSN: msg 1/4 key data",
                key_data, key_data_len);
        if (wpa_supplicant_parse_ies(key_data, key_data_len, &ie) < 0)
            goto failed;
        if (ie.pmkid) {
            wpa_hexdump(MSG_DEBUG, "RSN: PMKID from "
                    "Authenticator", ie.pmkid, PMKID_LEN);
        }
    }
    res = wpa_supplicant_get_pmk(sm, src_addr, ie.pmkid);

    if (res == -2) {
         wpa_printf(MSG_DEBUG, "RSN: Do not reply to msg 1/4 - "
               "requesting full EAP authentication");
        return;
    }
    if (res)
        goto failed;

#ifdef CONFIG_ESP_WIFI_ENTERPRISE_SUPPORT
    if (is_wpa2_enterprise_connection()) {
        pmksa_cache_set_current(sm, NULL, sm->bssid, 0, 0);
    }
#endif

    if (sm->renew_snonce) {
        if (os_get_random(sm->snonce, WPA_NONCE_LEN)) {
            wpa_printf(MSG_DEBUG, "WPA: Failed to get random data for SNonce");
            goto failed;
        }

        sm->renew_snonce = 0;
        wpa_hexdump(MSG_DEBUG, "WPA: Renewed SNonce",
                sm->snonce, WPA_NONCE_LEN);
    }

    /* Calculate PTK which will be stored as a temporary PTK until it has
     * been verified when processing message 3/4. */
    ptk = &sm->tptk;
    wpa_derive_ptk(sm, src_addr, key, ptk);
    /* Supplicant: swap tx/rx Mic keys */
    sm->tptk_set = 1;
    sm->ptk_set = 0;
    sm->key_install = true;
    kde = sm->assoc_wpa_ie;
    kde_len = sm->assoc_wpa_ie_len;
    kde_buf = os_malloc(kde_len +
                sm->assoc_rsnxe_len);
    if (!kde_buf)
        goto failed;
    os_memcpy(kde_buf, kde, kde_len);
    kde = kde_buf;

    if (sm->assoc_rsnxe && sm->assoc_rsnxe_len) {
        os_memcpy(kde + kde_len, sm->assoc_rsnxe, sm->assoc_rsnxe_len);
        kde_len += sm->assoc_rsnxe_len;
    }

    if (wpa_supplicant_send_2_of_4(sm, sm->bssid, key, ver, sm->snonce,
                       kde, kde_len, ptk))
        goto failed;

    os_free(kde_buf);
    memcpy(sm->anonce, key->key_nonce, WPA_NONCE_LEN);
    return;

failed:
    os_free(kde_buf);
    wpa_sm_deauthenticate(sm, WLAN_REASON_UNSPECIFIED);
}

static void wpa_sm_rekey_ptk(void *eloop_ctx, void *timeout_ctx)
{
    struct wpa_sm *sm = eloop_ctx;

    wpa_printf(MSG_DEBUG, "WPA: Request PTK rekeying");
    wpa_sm_key_request(sm, 0, 1);
}


static int wpa_supplicant_install_ptk(struct wpa_sm *sm, enum key_flag key_flag)
{
    int keylen;
    enum wpa_alg alg;

    if (sm->ptk.installed) {
        wpa_printf(MSG_DEBUG, "WPA: Do not re-install same PTK to the driver");
        return 0;
    }
    wpa_printf(MSG_DEBUG, "WPA: Installing PTK to the driver.");

    if (sm->pairwise_cipher == WPA_CIPHER_NONE) {
        wpa_printf(MSG_DEBUG, "WPA: Pairwise Cipher Suite: NONE - do not use pairwise keys");
        return 0;
    }

    if (!wpa_cipher_valid_pairwise(sm->pairwise_cipher)) {
        wpa_printf(MSG_DEBUG, "WPA: Unsupported pairwise cipher %d", sm->pairwise_cipher);
        return -1;
    }

    alg = wpa_cipher_to_alg(sm->pairwise_cipher);
    keylen = wpa_cipher_key_len(sm->pairwise_cipher);

    if (alg == WIFI_WPA_ALG_NONE) {
        wpa_printf(MSG_DEBUG, "WPA: Pairwise Cipher Suite: "
               "NONE - do not use pairwise keys");
        return 0;
    }
    if (wpa_sm_set_key(&(sm->install_ptk), alg, sm->bssid, 0, 1, (sm->install_ptk).seq, WPA_KEY_RSC_LEN,
                       sm->ptk.tk, keylen, KEY_FLAG_PAIRWISE | key_flag) < 0) {
        wpa_printf(MSG_DEBUG, "WPA: Failed to set PTK to the "
               "driver (alg=%d keylen=%d bssid=" MACSTR ")",
               alg, keylen, MAC2STR(sm->bssid));
        return -1;
    }

    sm->ptk.installed = 1;

    if (sm->wpa_ptk_rekey) {
        eloop_cancel_timeout(wpa_sm_rekey_ptk, sm, NULL);
        eloop_register_timeout(sm->wpa_ptk_rekey, 0, wpa_sm_rekey_ptk,
                       sm, NULL);
    }

    return 0;
}

static int wpa_supplicant_check_group_cipher(struct wpa_sm *sm,
                         int group_cipher,
                         int keylen, int maxkeylen,
                         int *key_rsc_len,
                         enum wpa_alg *alg)
{
    int klen;

    *alg = wpa_cipher_to_alg(group_cipher);
    if (*alg == WIFI_WPA_ALG_NONE) {
            wpa_printf(MSG_WARNING,
                    "WPA: Unsupported Group Cipher %d",
                    group_cipher);
            return -1;
    }
    *key_rsc_len = 6;

    klen = wpa_cipher_key_len(group_cipher);
    if (keylen != klen || maxkeylen < klen) {
            wpa_printf(MSG_WARNING,
                    "WPA: Unsupported %s Group Cipher key length %d (%d)",
                    wpa_cipher_txt(group_cipher), keylen, maxkeylen);
            return -1;
    }
    return 0;
}

void   wpa_supplicant_key_neg_complete(struct wpa_sm *sm,
                        const u8 *addr, int secure)
{
    wpa_printf(MSG_DEBUG, "WPA: Key negotiation completed with "
          MACSTR " [PTK=%s GTK=%s]\n", MAC2STR(addr),
          wpa_cipher_txt(sm->pairwise_cipher),
          wpa_cipher_txt(sm->group_cipher));
    wpa_sm_cancel_auth_timeout(sm);
    wpa_sm_set_state(WPA_COMPLETED);

    wpa_neg_complete();
    sm->eapol1_count = 0;
    sm->use_ext_key_id = 0;

    if (secure) {
        wpa_sm_mlme_setprotection(
            sm, addr, MLME_SETPROTECTION_PROTECT_TYPE_RX_TX,
            MLME_SETPROTECTION_KEY_TYPE_PAIRWISE);

        if (wpa_key_mgmt_wpa_psk(sm->key_mgmt) || sm->key_mgmt == WPA_KEY_MGMT_OWE)
            eapol_sm_notify_eap_success(TRUE);
        /*
         * Start preauthentication after a short wait to avoid a
         * possible race condition between the data receive and key
         * configuration after the 4-Way Handshake. This increases the
         * likelyhood of the first preauth EAPOL-Start frame getting to
         * the target AP.
         */
    }
#ifdef CONFIG_IEEE80211R
    if (wpa_key_mgmt_ft(sm->key_mgmt)) {
        /* Prepare for the next transition */
        wpa_ft_prepare_auth_request(sm, NULL);
        sm->ft_protocol = 1;
    }
#endif /* CONFIG_IEEE80211R */
}

static int wpa_supplicant_install_gtk(struct wpa_sm *sm,
                      struct wpa_gtk_data *gd)
{
    u8 *_gtk = gd->gtk;
    u8 gtk_buf[32];
    u8 *key_rsc=(sm->install_gtk).seq;

    wpa_hexdump(MSG_DEBUG, "WPA: Group Key", gd->gtk, gd->gtk_len);

    /* Detect possible key reinstallation */
    if ((sm->gtk.gtk_len == (size_t) gd->gtk_len &&
            os_memcmp(sm->gtk.gtk, gd->gtk, sm->gtk.gtk_len) == 0) ||
            wpa_supplicant_gtk_in_use(sm, &(sm->gd))) {
            wpa_printf(MSG_DEBUG,
                    "WPA: Not reinstalling already in-use GTK to the driver (keyidx=%d tx=%d len=%d)",
                    gd->keyidx, gd->tx, gd->gtk_len);
            return 0;
    }
    wpa_printf(MSG_DEBUG, "WPA: Installing GTK to the driver "
           "(keyidx=%d tx=%d len=%d).", gd->keyidx, gd->tx,
           gd->gtk_len);

    wpa_hexdump(MSG_DEBUG, "WPA: RSC", key_rsc, gd->key_rsc_len);
    if (sm->group_cipher == WPA_CIPHER_TKIP) {
        /* Swap Tx/Rx keys for Michael MIC */
        memcpy(gtk_buf, gd->gtk, 16);
        memcpy(gtk_buf + 16, gd->gtk + 16, 8);
        memcpy(gtk_buf + 24, gd->gtk + 24, 8);
        _gtk = gtk_buf;
    }
    if (sm->pairwise_cipher == WPA_CIPHER_NONE) {
        if (wpa_sm_set_key(&(sm->install_gtk), gd->alg,
                   sm->bssid, //(u8 *) "\xff\xff\xff\xff\xff\xff",
                   gd->keyidx, 1, key_rsc, gd->key_rsc_len,
                   _gtk, gd->gtk_len, (KEY_FLAG_GROUP | KEY_FLAG_RX | KEY_FLAG_TX)) < 0) {
            wpa_printf(MSG_DEBUG, "WPA: Failed to set "
                   "GTK to the driver (Group only).");
            return -1;
        }
    } else if (wpa_sm_set_key(&(sm->install_gtk), gd->alg,
                  sm->bssid, //(u8 *) "\xff\xff\xff\xff\xff\xff",
                  gd->keyidx, gd->tx, key_rsc, gd->key_rsc_len,
                  _gtk, gd->gtk_len, KEY_FLAG_GROUP | KEY_FLAG_RX) < 0) {
        wpa_printf(MSG_DEBUG, "WPA: Failed to set GTK to "
               "the driver (alg=%d keylen=%d keyidx=%d)",
               gd->alg, gd->gtk_len, gd->keyidx);
        return -1;
    }
    sm->gtk.gtk_len = gd->gtk_len;
    os_memcpy(sm->gtk.gtk, gd->gtk, sm->gtk.gtk_len);

    return 0;
}

static bool wpa_supplicant_gtk_in_use(struct wpa_sm *sm, struct wpa_gtk_data *gd)
{
    u8 *_gtk = gd->gtk;
    u8 gtk_buf[32];
    u8 gtk_get[32] = {0};
    u8 ifx;
    int alg;
    u8 bssid[6];
    int keyidx = gd->keyidx;

    wpa_printf(MSG_DEBUG, "WPA: Judge GTK: (keyidx=%d len=%d).", gd->keyidx, gd->gtk_len);

    if (sm->group_cipher == WPA_CIPHER_TKIP) {
        /* Swap Tx/Rx keys for Michael MIC */
        memcpy(gtk_buf, gd->gtk, 16);
        memcpy(gtk_buf + 16, gd->gtk + 16, 8);
        memcpy(gtk_buf + 24, gd->gtk + 24, 8);
        _gtk = gtk_buf;
    }

    if (wpa_sm_get_key(&ifx, &alg, bssid, &keyidx, gtk_get, gd->gtk_len, KEY_FLAG_GROUP) == 0) {
        if (ifx == 0 && alg == gd->alg && memcmp(bssid, sm->bssid, ETH_ALEN) == 0 &&
                memcmp(_gtk, gtk_get, gd->gtk_len) == 0) {
            wpa_printf(MSG_DEBUG, "GTK %d is already in use, it may be an attack, ignore it.", gd->keyidx);
            return true;
        }
    }

    return false;
}

int wpa_supplicant_gtk_tx_bit_workaround(const struct wpa_sm *sm,
                        int tx)
{
    if (tx && sm->pairwise_cipher != WPA_CIPHER_NONE) {
        /* Ignore Tx bit for GTK if a pairwise key is used. One AP
         * seemed to set this bit (incorrectly, since Tx is only when
         * doing Group Key only APs) and without this workaround, the
         * data connection does not work because wpa_supplicant
         * configured non-zero keyidx to be used for unicast. */
        wpa_printf(MSG_DEBUG, "WPA: Tx bit set for GTK, but pairwise "
               "keys are used - ignore Tx bit");
        return 0;
    }
    return tx;
}

int wpa_supplicant_pairwise_gtk(struct wpa_sm *sm,
                       const u8 *gtk, size_t gtk_len,
                       int key_info)
{
    struct wpa_gtk_data *gd=&(sm->gd);

    /*
     * IEEE Std 802.11i-2004 - 8.5.2 EAPOL-Key frames - Figure 43x
     * GTK KDE format:
     * KeyID[bits 0-1], Tx [bit 2], Reserved [bits 3-7]
     * Reserved [bits 0-7]
     * GTK
     */

    memset(gd, 0, sizeof(struct wpa_gtk_data));
    wpa_hexdump(MSG_DEBUG, "RSN: received GTK in pairwise handshake",
            gtk, gtk_len);

    if (gtk_len < 2 || gtk_len - 2 > sizeof(gd->gtk))
        return -1;

    gd->keyidx = gtk[0] & 0x3;
    gd->tx = wpa_supplicant_gtk_tx_bit_workaround(sm,
                             !!(gtk[0] & BIT(2)));
    gtk += 2;
    gtk_len -= 2;

    memcpy(gd->gtk, gtk, gtk_len);
    gd->gtk_len = gtk_len;

    if (wpa_supplicant_check_group_cipher(sm, sm->group_cipher,
                          gtk_len, gtk_len,
                          &(gd->key_rsc_len), &(gd->alg))) {
        wpa_printf(MSG_DEBUG, "RSN: Failed to install GTK");
        return -1;
    }
    return 0;
}

#ifdef CONFIG_IEEE80211W
static int wpa_supplicant_install_igtk(struct wpa_sm *sm,
                      const wifi_wpa_igtk_t *igtk)
{
    size_t len = wpa_cipher_key_len(sm->mgmt_group_cipher);
    u16 keyidx = WPA_GET_LE16(igtk->keyid);

    /* Detect possible key reinstallation */
    if (sm->igtk.igtk_len == len &&
        os_memcmp(sm->igtk.igtk, igtk->igtk, sm->igtk.igtk_len) == 0) {
        wpa_printf(MSG_DEBUG,
            "WPA: Not reinstalling already in-use IGTK to the driver (keyidx=%d)",
            keyidx);
        return  0;
    }

    wpa_printf(MSG_DEBUG,
        "WPA: IGTK keyid %d pn %02x%02x%02x%02x%02x%02x",
        keyidx, MAC2STR(igtk->pn));
    wpa_hexdump_key(MSG_DEBUG, "WPA: IGTK", igtk->igtk, len);

    if (esp_wifi_set_igtk_internal(WIFI_IF_STA, igtk) < 0) {
        if (keyidx > 4095) {
            wpa_printf(MSG_WARNING,
                "WPA: Invalid IGTK KeyID %d", keyidx);
        }
        wpa_printf(MSG_WARNING,
            "WPA: Failed to configure IGTK to the driver");
        return -1;
    }

    sm->igtk.igtk_len = len;
    os_memcpy(sm->igtk.igtk, igtk->igtk, sm->igtk.igtk_len);

    return 0;
}
#endif /* CONFIG_IEEE80211W */

#ifdef DEBUG_PRINT
void wpa_report_ie_mismatch(struct wpa_sm *sm,
                   const char *reason, const u8 *src_addr,
                   const u8 *wpa_ie, size_t wpa_ie_len,
                   const u8 *rsn_ie, size_t rsn_ie_len)
#else
void wpa_report_ie_mismatch(struct wpa_sm *sm, const u8 *src_addr,
                   const u8 *wpa_ie, size_t wpa_ie_len,
                   const u8 *rsn_ie, size_t rsn_ie_len)
#endif
{
    wpa_printf(MSG_DEBUG, "WPA: %s (src=" MACSTR ")",
        reason, MAC2STR(src_addr));
    if (sm->ap_wpa_ie) {
        wpa_hexdump(MSG_INFO, "WPA: WPA IE in Beacon/ProbeResp",
                sm->ap_wpa_ie, sm->ap_wpa_ie_len);
    }
    if (wpa_ie) {
        if (!sm->ap_wpa_ie) {
            wpa_printf(MSG_DEBUG, "WPA: No WPA IE in "
                   "Beacon/ProbeResp");
        }
        wpa_hexdump(MSG_INFO, "WPA: WPA IE in 3/4 msg",
                wpa_ie, wpa_ie_len);
    }

    if (sm->ap_rsn_ie) {
        wpa_hexdump(MSG_INFO, "WPA: RSN IE in Beacon/ProbeResp",
                sm->ap_rsn_ie, sm->ap_rsn_ie_len);
    }
    if (rsn_ie) {
        if (!sm->ap_rsn_ie) {
            wpa_printf(MSG_DEBUG, "WPA: No RSN IE in "
                   "Beacon/ProbeResp");
        }
        wpa_hexdump(MSG_INFO, "WPA: RSN IE in 3/4 msg",
                rsn_ie, rsn_ie_len);
    }

    wpa_sm_disassociate(sm, WLAN_REASON_IE_IN_4WAY_DIFFERS);
}

static int ieee80211w_set_keys(struct wpa_sm *sm,
                   struct wpa_eapol_ie_parse *ie)
{
#ifdef CONFIG_IEEE80211W
    size_t len;

    if (!wpa_cipher_valid_mgmt_group(sm->mgmt_group_cipher))
        return 0;

    if (ie->igtk) {
        const wifi_wpa_igtk_t *igtk;
#define WPA_IGTK_KDE_PREFIX_LEN (2 + 6)
        len = wpa_cipher_key_len(sm->mgmt_group_cipher);
        if (ie->igtk_len != WPA_IGTK_KDE_PREFIX_LEN + len) {
            return -1;
        }

        igtk = (const wifi_wpa_igtk_t*)ie->igtk;
        if (wpa_supplicant_install_igtk(sm, igtk) < 0) {
            return -1;
        }
    }
#endif
    return 0;
}

static int wpa_supplicant_validate_ie(struct wpa_sm *sm,
                      const unsigned char *src_addr,
                      struct wpa_eapol_ie_parse *ie)
{
    if (ie->wpa_ie == NULL && ie->rsn_ie == NULL &&
        (sm->ap_wpa_ie || sm->ap_rsn_ie)) {
#ifdef DEBUG_PRINT
        wpa_report_ie_mismatch(sm, "IE in 3/4 msg does not match "
                       "with IE in Beacon/ProbeResp (no IE?)",
                       src_addr, ie->wpa_ie, ie->wpa_ie_len,
                       ie->rsn_ie, ie->rsn_ie_len);
#else
        wpa_report_ie_mismatch(sm,
                       src_addr, ie->wpa_ie, ie->wpa_ie_len,
                       ie->rsn_ie, ie->rsn_ie_len);
#endif
        return -1;
    }

    if ((ie->wpa_ie && sm->ap_wpa_ie &&
         (ie->wpa_ie_len != sm->ap_wpa_ie_len ||
         memcmp(ie->wpa_ie, sm->ap_wpa_ie, ie->wpa_ie_len) != 0)) ||
        (ie->rsn_ie && sm->ap_rsn_ie &&
         wpa_compare_rsn_ie(wpa_key_mgmt_ft(sm->key_mgmt),
                sm->ap_rsn_ie, sm->ap_rsn_ie_len,
                ie->rsn_ie, ie->rsn_ie_len))) {
#ifdef DEBUG_PRINT
        wpa_report_ie_mismatch(sm, "IE in 3/4 msg does not match "
                       "with IE in Beacon/ProbeResp",
                       src_addr, ie->wpa_ie, ie->wpa_ie_len,
                       ie->rsn_ie, ie->rsn_ie_len);
#else
        wpa_report_ie_mismatch(sm,
                       src_addr, ie->wpa_ie, ie->wpa_ie_len,
                       ie->rsn_ie, ie->rsn_ie_len);
#endif
        return -1;
    }

    if (sm->proto == WPA_PROTO_WPA &&
        ie->rsn_ie && sm->ap_rsn_ie == NULL && sm->rsn_enabled) {
#ifdef DEBUG_PRINT
        wpa_report_ie_mismatch(sm, "Possible downgrade attack "
                       "detected - RSN was enabled and RSN IE "
                       "was in msg 3/4, but not in "
                       "Beacon/ProbeResp",
                       src_addr, ie->wpa_ie, ie->wpa_ie_len,
                       ie->rsn_ie, ie->rsn_ie_len);
#else
        wpa_report_ie_mismatch(sm,
                               src_addr, ie->wpa_ie, ie->wpa_ie_len,
                               ie->rsn_ie, ie->rsn_ie_len);
#endif
        return -1;
    }

    if (sm->proto == WPA_PROTO_RSN &&
        ((sm->ap_rsnxe && !ie->rsnxe) ||
         (!sm->ap_rsnxe && ie->rsnxe) ||
         (sm->ap_rsnxe && ie->rsnxe &&
          (sm->ap_rsnxe_len != ie->rsnxe_len ||
           os_memcmp(sm->ap_rsnxe, ie->rsnxe, sm->ap_rsnxe_len) != 0)))) {
        wpa_printf(MSG_INFO,
            "WPA: RSNXE mismatch between Beacon/ProbeResp and EAPOL-Key msg 3/4");
        wpa_hexdump(MSG_INFO, "RSNXE in Beacon/ProbeResp",
                sm->ap_rsnxe, sm->ap_rsnxe_len);
        wpa_hexdump(MSG_INFO, "RSNXE in EAPOL-Key msg 3/4",
                ie->rsnxe, ie->rsnxe_len);
        wpa_sm_deauthenticate(sm, WLAN_REASON_IE_IN_4WAY_DIFFERS);
        return -1;
    }

    return 0;
}

/**
 * wpa_supplicant_send_4_of_4 - Send message 4 of WPA/RSN 4-Way Handshake
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @dst: Destination address for the frame
 * @key: Pointer to the EAPOL-Key frame header
 * @ver: Version bits from EAPOL-Key Key Info
 * @key_info: Key Info
 * @ptk: PTK to use for keyed hash and encryption
 * Returns: 0 on success, -1 on failure
 */
static int wpa_supplicant_send_4_of_4(struct wpa_sm *sm, const unsigned char *dst,
                   const struct wpa_eapol_key *key,
                   u16 ver, u16 key_info,
                   struct wpa_ptk *ptk)
{
    size_t mic_len, hdrlen, rlen;
    struct wpa_eapol_key *reply;
    struct wpa_eapol_key_192 *reply192;
    u8 *rbuf, *key_mic;

    mic_len = wpa_mic_len(sm->key_mgmt, sm->pmk_len);
    hdrlen = mic_len == 24 ? sizeof(*reply192) : sizeof(*reply);

    rbuf = wpa_sm_alloc_eapol(sm, IEEE802_1X_TYPE_EAPOL_KEY, NULL,
                  hdrlen,
                  &rlen, (void *) &reply);
    if (rbuf == NULL)
        return -1;

    sm->txcb_flags |= WPA_4_4_HANDSHAKE_BIT;
    wpa_printf(MSG_DEBUG, "tx 4/4 txcb_flags=%d", sm->txcb_flags);

    reply192 = (struct wpa_eapol_key_192 *) reply;
    reply->type = sm->proto == WPA_PROTO_RSN ?
        EAPOL_KEY_TYPE_RSN : EAPOL_KEY_TYPE_WPA;
    key_info &= WPA_KEY_INFO_SECURE;
    key_info |= ver | WPA_KEY_INFO_KEY_TYPE | WPA_KEY_INFO_MIC;
    WPA_PUT_BE16(reply->key_info, key_info);
    if (sm->proto == WPA_PROTO_RSN)
        WPA_PUT_BE16(reply->key_length, 0);
    else
        memcpy(reply->key_length, key->key_length, 2);
    memcpy(reply->replay_counter, key->replay_counter,
          WPA_REPLAY_COUNTER_LEN);

    key_mic = reply192->key_mic; /* same offset for reply and reply192 */
    if (mic_len == 24)
        WPA_PUT_BE16(reply192->key_data_length, 0);
    else
        WPA_PUT_BE16(reply->key_data_length, 0);

    wpa_printf(MSG_DEBUG, "WPA Send EAPOL-Key 4/4");
    wpa_eapol_key_send(sm, ptk->kck, ptk->kck_len, ver, dst, ETH_P_EAPOL,
               rbuf, rlen, key_mic);
    wpa_sm_free_eapol(rbuf);

    return 0;
}

static void wpa_sm_set_seq(struct wpa_sm *sm, struct wpa_eapol_key *key, u8 isptk)
{
    u8 *key_rsc, *seq;
    u8 null_rsc[WPA_KEY_RSC_LEN];

    os_bzero(null_rsc, WPA_KEY_RSC_LEN);

    if (sm->proto == WPA_PROTO_RSN && isptk) {
        key_rsc = null_rsc;
    } else {
        key_rsc = key->key_rsc;
        wpa_hexdump(MSG_DEBUG, "WPA: RSC", key_rsc, WPA_KEY_RSC_LEN);
    }

    seq=(isptk) ? (sm->install_ptk).seq : (sm->install_gtk).seq;
    memcpy(seq, key_rsc, WPA_KEY_RSC_LEN);
}

static void wpa_supplicant_process_3_of_4(struct wpa_sm *sm,
                      struct wpa_eapol_key *key,
                      u16 ver, const u8 *key_data,
                      size_t key_data_len)
{
    u16 key_info, keylen;
    struct wpa_eapol_ie_parse ie;

    wpa_sm_set_state(WPA_LAST_HALF_4WAY_HANDSHAKE);
    wpa_printf(MSG_DEBUG, "WPA 3/4-Way Handshake");

    key_info = WPA_GET_BE16(key->key_info);

    if (wpa_supplicant_parse_ies(key_data, key_data_len, &ie) < 0)
        goto failed;
    if (wpa_supplicant_validate_ie(sm, sm->bssid, &ie) < 0)
        goto failed;

    if (ie.gtk && !(key_info & WPA_KEY_INFO_ENCR_KEY_DATA)) {
        wpa_printf(MSG_DEBUG, "WPA: GTK IE in unencrypted key data");
        goto failed;
    }

    wpa_hexdump(MSG_DEBUG, "WPA: IE KeyData", key_data, key_data_len);
    if (memcmp(sm->anonce, key->key_nonce, WPA_NONCE_LEN) != 0) {
        wpa_printf(MSG_DEBUG, "WPA: ANonce from message 1 of 4-Way "
               "Handshake differs from 3 of 4-Way Handshake - drop"
               " packet (src=" MACSTR ")", MAC2STR(sm->bssid));
        goto failed;
    }

    keylen = WPA_GET_BE16(key->key_length);
    switch (sm->pairwise_cipher) {
    case WPA_CIPHER_CCMP:
        if (keylen != 16) {
            wpa_printf(MSG_DEBUG, "WPA: Invalid CCMP key length "
                   "%d (src=" MACSTR ")",
                   keylen, MAC2STR(sm->bssid));
            goto failed;
        }
        break;
    case WPA_CIPHER_TKIP:
        if (keylen != 32) {
            wpa_printf(MSG_DEBUG, "WPA: Invalid TKIP key length "
                   "%d (src=" MACSTR ")",
                   keylen, MAC2STR(sm->bssid));
            goto failed;
        }
        break;
    }


    /* SNonce was successfully used in msg 3/4, so mark it to be renewed
     * for the next 4-Way Handshake. If msg 3 is received again, the old
     * SNonce will still be used to avoid changing PTK. */
    sm->renew_snonce = 1;

    /*ready for txcallback , set seq and set txcallback param*/
    wpa_sm_set_seq(sm, key, 1);
    sm->key_info=key_info;
    (sm->gd).gtk_len=0; //used as flag if gtk is installed in callback
    if (ie.gtk) {
        wpa_sm_set_seq(sm, key, 0);
           if (wpa_supplicant_pairwise_gtk(sm,
                    ie.gtk, ie.gtk_len, key_info) < 0) {
        wpa_printf(MSG_DEBUG, "RSN: Failed to configure GTK");
        goto failed;
        }
    }

    if (sm->pmf_cfg.capable && ieee80211w_set_keys(sm, &ie) < 0) {
        wpa_printf(MSG_DEBUG, "RSN: Failed to configure IGTK");
        goto failed;
    }

    if (ie.transition_disable) {
        wpa_supplicant_transition_disable(sm, ie.transition_disable[0]);
    }

    if (sm->key_install && sm->key_info & WPA_KEY_INFO_INSTALL && sm->use_ext_key_id) {
        wpa_supplicant_install_ptk(sm, KEY_FLAG_RX);
    }
    /*after txover, callback will continue run remain task*/
    if (wpa_supplicant_send_4_of_4(sm, sm->bssid, key, ver, key_info,
                       &sm->ptk)) {
        goto failed;
    }

    return;

failed:
    wpa_sm_deauthenticate(sm, WLAN_REASON_UNSPECIFIED);
}

static int wpa_supplicant_activate_ptk(struct wpa_sm *sm)
{
    int keylen;
    enum wpa_alg alg;

    alg = wpa_cipher_to_alg(sm->pairwise_cipher);
    keylen = wpa_cipher_key_len(sm->pairwise_cipher);

    if (alg == WIFI_WPA_ALG_NONE) {
        wpa_printf(MSG_DEBUG, "WPA: Pairwise Cipher Suite: "
               "NONE - do not use pairwise keys");
        return 0;
    }

    wpa_printf(MSG_DEBUG,
            "WPA: Activate PTK bssid=" MACSTR ")",
            MAC2STR(sm->bssid));

    if (wpa_sm_set_key(&(sm->install_ptk), alg, sm->bssid, 0, 1, (sm->install_ptk).seq,
                       WPA_KEY_RSC_LEN, sm->ptk.tk, keylen,
                       (KEY_FLAG_MODIFY | KEY_FLAG_PAIRWISE | KEY_FLAG_RX | KEY_FLAG_TX)) < 0) {
            wpa_printf(MSG_WARNING,
                    "WPA: Failed to activate PTK for TX (idx=%d bssid="
                    MACSTR ")", 0, MAC2STR(sm->bssid));
            return -1;
    }
    return 0;
}

static int wpa_supplicant_send_4_of_4_txcallback(struct wpa_sm *sm)
{
       u16 key_info=sm->key_info;

    if (sm->key_install && key_info & WPA_KEY_INFO_INSTALL) {
        if (sm->use_ext_key_id) {
            if (wpa_supplicant_activate_ptk(sm))
                goto failed;
        } else {
            if (wpa_supplicant_install_ptk(sm, KEY_FLAG_TX | KEY_FLAG_RX))
                goto failed;
        }
    }
    else if (sm->key_install == false) {
        wpa_printf(MSG_DEBUG, "PTK has been installed, it may be an attack, ignor it.");
    }

    wpa_sm_set_state(WPA_GROUP_HANDSHAKE);

    if ((sm->gd).gtk_len) {
        if (sm->key_install) {
            if (wpa_supplicant_install_gtk(sm, &(sm->gd)))
                goto failed;
        }
        else {
            wpa_printf(MSG_DEBUG, "GTK has been installed, it may be an attack, ignor it.");
        }
        wpa_supplicant_key_neg_complete(sm, sm->bssid,
                    key_info & WPA_KEY_INFO_SECURE);
    }

    if (key_info & WPA_KEY_INFO_SECURE) {
        wpa_sm_mlme_setprotection(
            sm, sm->bssid, MLME_SETPROTECTION_PROTECT_TYPE_RX,
            MLME_SETPROTECTION_KEY_TYPE_PAIRWISE);
    }

    sm->key_install = false;

#if defined(CONFIG_SUITEB192) || defined (CONFIG_SUITEB)
    /* Add PMKSA cache entry for Suite B AKMs here since PMKID can be
     * calculated only after KCK has been derived. Though, do not replace an
     * existing PMKSA entry after each 4-way handshake (i.e., new KCK/PMKID)
     * to avoid unnecessary changes of PMKID while continuing to use the
     * same PMK. */
    if (sm->proto == WPA_PROTO_RSN && wpa_key_mgmt_suite_b(sm->key_mgmt) &&
        !sm->cur_pmksa) {
        struct rsn_pmksa_cache_entry *sa;

        sa = pmksa_cache_add(sm->pmksa, sm->pmk, sm->pmk_len, NULL,
                             sm->ptk.kck, sm->ptk.kck_len,
                             sm->bssid, sm->own_addr,
                             sm->network_ctx, sm->key_mgmt);
        if (!sm->cur_pmksa)
            sm->cur_pmksa = sa;
    }
#endif

    return 0;

failed:
       return WLAN_REASON_UNSPECIFIED;
}

static int wpa_supplicant_process_1_of_2_rsn(struct wpa_sm *sm,
                         const u8 *keydata,
                         size_t keydatalen,
                         u16 key_info,
                         struct wpa_gtk_data *gd)
{
    int maxkeylen;
    struct wpa_eapol_ie_parse ie;

    wpa_hexdump(MSG_DEBUG, "RSN: msg 1/2 key data", keydata, keydatalen);
    wpa_supplicant_parse_ies(keydata, keydatalen, &ie);
    if (ie.gtk && !(key_info & WPA_KEY_INFO_ENCR_KEY_DATA)) {
        wpa_printf(MSG_DEBUG, "WPA: GTK IE in unencrypted key data");
        return -1;
    }
    if (ie.gtk == NULL) {
        wpa_printf(MSG_DEBUG, "WPA: No GTK IE in Group Key msg 1/2");
        return -1;
    }
    maxkeylen = gd->gtk_len = ie.gtk_len - 2;

    if (wpa_supplicant_check_group_cipher(sm, sm->group_cipher,
                          gd->gtk_len, maxkeylen,
                          &gd->key_rsc_len, &gd->alg))
        return -1;

    wpa_hexdump(MSG_DEBUG, "RSN: received GTK in group key handshake",
            ie.gtk, ie.gtk_len);
    gd->keyidx = ie.gtk[0] & 0x3;
    gd->tx = wpa_supplicant_gtk_tx_bit_workaround(sm,
                              !!(ie.gtk[0] & BIT(2)));
    if (ie.gtk_len - 2 > sizeof(gd->gtk)) {
        wpa_printf(MSG_DEBUG, "RSN: Too long GTK in GTK IE "
               "(len=%lu)", (unsigned long) ie.gtk_len - 2);
        return -1;
    }
    memcpy(gd->gtk, ie.gtk + 2, ie.gtk_len - 2);

    if (ieee80211w_set_keys(sm, &ie) < 0)
    {
        wpa_printf(MSG_DEBUG, "RSN: Failed to configure IGTK");
    }
    return 0;
}

static int wpa_supplicant_process_1_of_2_wpa(struct wpa_sm *sm,
                         const struct wpa_eapol_key *key,
                         const u8 *key_data,
                         size_t key_data_len, u16 key_info,
                         u16 ver, struct wpa_gtk_data *gd)
{
    size_t maxkeylen;
    u8 ek[32];

    gd->gtk_len = WPA_GET_BE16(key->key_length);
    maxkeylen = key_data_len;
    if (ver == WPA_KEY_INFO_TYPE_HMAC_SHA1_AES) {
        if (maxkeylen < 8) {
            wpa_printf(MSG_DEBUG, "WPA: Too short maxkeylen (%lu)",
                   (unsigned long) maxkeylen);
            return -1;
        }
        maxkeylen -= 8;
    }

    if (wpa_supplicant_check_group_cipher(sm, sm->group_cipher,
                          gd->gtk_len, maxkeylen,
                          &gd->key_rsc_len, &gd->alg))
        return -1;

    gd->keyidx = (key_info & WPA_KEY_INFO_KEY_INDEX_MASK) >>
        WPA_KEY_INFO_KEY_INDEX_SHIFT;
    if (ver == WPA_KEY_INFO_TYPE_HMAC_MD5_RC4 && sm->ptk.kek_len == 16) {
        os_memcpy(ek, key->key_iv, 16);
        os_memcpy(ek + 16, sm->ptk.kek, sm->ptk.kek_len);
        if (key_data_len > sizeof(gd->gtk)) {
            wpa_printf(MSG_DEBUG, "WPA: RC4 key data "
                   "too long (%lu)",
                (unsigned long) key_data_len);
            return -1;
        }
        os_memcpy(gd->gtk, key_data, key_data_len);
        if (rc4_skip(ek, 32, 256, gd->gtk, key_data_len)) {
            wpa_printf(MSG_DEBUG, "WPA: RC4 failed");
            return -1;
        }
    } else if (ver == WPA_KEY_INFO_TYPE_HMAC_SHA1_AES) {
        if (maxkeylen % 8) {
            wpa_printf(MSG_DEBUG,
                "WPA: Unsupported AES-WRAP len %lu",
                (unsigned long) maxkeylen);
            return -1;
        }
        if (maxkeylen > sizeof(gd->gtk)) {
            wpa_printf(MSG_DEBUG, "WPA: AES-WRAP key data "
                   "too long (keydatalen=%lu maxkeylen=%lu)",
                (unsigned long) key_data_len,
                   (unsigned long) maxkeylen);
            return -1;
        }
        if (aes_unwrap(sm->ptk.kek, sm->ptk.kek_len, maxkeylen / 8, key_data, gd->gtk)) {
            wpa_printf(MSG_DEBUG, "WPA: AES unwrap "
                "failed - could not decrypt GTK");
            return -1;
        }
    } else {
        wpa_printf(MSG_DEBUG, "WPA: Unsupported key_info type %d",
               ver);
        return -1;
    }
    gd->tx = wpa_supplicant_gtk_tx_bit_workaround(
        sm, !!(key_info & WPA_KEY_INFO_TXRX));

    return 0;
}

static int wpa_supplicant_send_2_of_2(struct wpa_sm *sm,
                      const struct wpa_eapol_key *key,
                      int ver, u16 key_info)
{
    size_t mic_len, hdrlen, rlen;
    struct wpa_eapol_key *reply;
    struct wpa_eapol_key_192 *reply192;
    u8 *rbuf, *key_mic;

    mic_len = wpa_mic_len(sm->key_mgmt, sm->pmk_len);
    hdrlen = mic_len == 24 ? sizeof(*reply192) : sizeof(*reply);

    rbuf = wpa_sm_alloc_eapol(sm, IEEE802_1X_TYPE_EAPOL_KEY, NULL,
                              hdrlen, &rlen, (void *) &reply);
    if (rbuf == NULL)
        return -1;

    sm->txcb_flags |= WPA_GROUP_HANDSHAKE_BIT;
    wpa_printf(MSG_DEBUG, "2/2 txcb_flags=%d", sm->txcb_flags);

    reply192 = (struct wpa_eapol_key_192 *) reply;
    reply->type = sm->proto == WPA_PROTO_RSN ?
        EAPOL_KEY_TYPE_RSN : EAPOL_KEY_TYPE_WPA;
    key_info &= WPA_KEY_INFO_KEY_INDEX_MASK;
    key_info |= ver | WPA_KEY_INFO_MIC | WPA_KEY_INFO_SECURE;
    WPA_PUT_BE16(reply->key_info, key_info);
    if (sm->proto == WPA_PROTO_RSN)
        WPA_PUT_BE16(reply->key_length, 0);
    else
        memcpy(reply->key_length, key->key_length, 2);
    memcpy(reply->replay_counter, key->replay_counter,
          WPA_REPLAY_COUNTER_LEN);

    key_mic = reply192->key_mic; /* same offset for reply and reply192 */
    if (mic_len == 24)
        WPA_PUT_BE16(reply192->key_data_length, 0);
    else
        WPA_PUT_BE16(reply->key_data_length, 0);

    wpa_printf(MSG_DEBUG, "WPA Send 2/2 Group key");

    wpa_eapol_key_send(sm, sm->ptk.kck, sm->ptk.kck_len, ver, sm->bssid, ETH_P_EAPOL,
               rbuf, rlen, key_mic);
    wpa_sm_free_eapol(rbuf);

    return 0;
}

static void wpa_supplicant_process_1_of_2(struct wpa_sm *sm,
                      const unsigned char *src_addr,
                      struct wpa_eapol_key *key,
                      const u8 *key_data,
                      size_t key_data_len, u16 ver)
{
    u16 key_info;
    int  ret;
    struct wpa_gtk_data *gd=&(sm->gd);

    memset(gd, 0, sizeof(struct wpa_gtk_data));

    wpa_printf(MSG_DEBUG, "WPA 1/2 Group Key Handshake");

    key_info = WPA_GET_BE16(key->key_info);

    if (sm->proto == WPA_PROTO_RSN) {
        ret = wpa_supplicant_process_1_of_2_rsn(sm, key_data,
                                                key_data_len, key_info, gd);
    } else {
        ret = wpa_supplicant_process_1_of_2_wpa(sm, key, key_data,
                                                key_data_len, key_info, ver, gd);
    }

    wpa_sm_set_state(WPA_GROUP_HANDSHAKE);

    if (ret)
        goto failed;

    /*before callback, set seq for add param difficult in callback*/
    wpa_sm_set_seq(sm, key, 0);
    sm->key_info=key_info;

    /*install gtk before send 2 of 2*/
    if((sm->gd).gtk_len) {
        if (wpa_supplicant_install_gtk(sm, &(sm->gd)))
            goto failed;
    } else {
        goto failed;
    }

    if (wpa_supplicant_send_2_of_2(sm, key, ver, key_info))
        goto failed;

    if (WPA_SM_STATE(sm) == WPA_COMPLETED) {
#ifdef MSG_PRINT
        wpa_printf(MSG_DEBUG, "WPA: Group rekeying "
            "completed with " MACSTR " [GTK=%s]",
            MAC2STR(sm->bssid), wpa_cipher_txt(sm->group_cipher));
#endif
        wpa_sm_cancel_auth_timeout(sm);
        wpa_sm_set_state(WPA_COMPLETED);
    } else {
        wpa_supplicant_key_neg_complete(sm, sm->bssid,
                        sm->key_info &WPA_KEY_INFO_SECURE);
    }

    return;

failed:
    wpa_sm_deauthenticate(sm, WLAN_REASON_UNSPECIFIED);
}

static int wpa_supplicant_verify_eapol_key_mic(struct wpa_sm *sm,
                           struct wpa_eapol_key_192 *key,
                           u16 ver,
                           const u8 *buf, size_t len)
{
    u8 mic[WPA_EAPOL_KEY_MIC_MAX_LEN];
    int ok = 0;
    size_t mic_len = wpa_mic_len(sm->key_mgmt, sm->pmk_len);

    os_memcpy(mic, key->key_mic, mic_len);
    if (sm->tptk_set) {
        os_memset(key->key_mic, 0, mic_len);
        wpa_eapol_key_mic(sm->tptk.kck, sm->tptk.kck_len, sm->key_mgmt,
                          ver, buf, len, key->key_mic);
        if (os_memcmp_const(mic, key->key_mic, mic_len) != 0) {
            wpa_printf(MSG_DEBUG, "WPA: Invalid EAPOL-Key MIC "
                   "when using TPTK - ignoring TPTK");
        } else {
            ok = 1;
            sm->tptk_set = 0;
            sm->ptk_set = 1;
            memcpy(&sm->ptk, &sm->tptk, sizeof(sm->ptk));
        }
    }

    if (!ok && sm->ptk_set) {
        os_memset(key->key_mic, 0, mic_len);
        wpa_eapol_key_mic(sm->ptk.kck, sm->ptk.kck_len, sm->key_mgmt,
                          ver, buf, len, key->key_mic);
        if (os_memcmp_const(mic, key->key_mic, mic_len) != 0) {
            wpa_printf(MSG_DEBUG, "WPA: Invalid EAPOL-Key MIC "
                   "- dropping packet");
            return -1;
        }
        ok = 1;
    }

    if (!ok) {
        wpa_printf(MSG_DEBUG, "WPA: Could not verify EAPOL-Key MIC "
               "- dropping packet");
        return -1;
    }

    memcpy(sm->rx_replay_counter, key->replay_counter,
          WPA_REPLAY_COUNTER_LEN);
    sm->rx_replay_counter_set = 1;
    /*update request_counter for mic failure report*/
    memcpy(sm->request_counter, key->replay_counter,
          WPA_REPLAY_COUNTER_LEN);
    return 0;
}


/* Decrypt RSN EAPOL-Key key data (RC4 or AES-WRAP) */
static int wpa_supplicant_decrypt_key_data(struct wpa_sm *sm,
                                           struct wpa_eapol_key *key, u16 ver,
                                           u8 *key_data, size_t *key_data_len)
{
    wpa_hexdump(MSG_DEBUG, "RSN: encrypted key data",
                key_data, *key_data_len);
    if (!sm->ptk_set) {
        wpa_printf(MSG_DEBUG, "WPA: PTK not available, "
               "cannot decrypt EAPOL-Key key data.");
        return -1;
    }

    /* Decrypt key data here so that this operation does not need
     * to be implemented separately for each message type. */
    if (ver == WPA_KEY_INFO_TYPE_HMAC_MD5_RC4 && sm->ptk.kek_len == 16) {
        u8 ek[32];
        os_memcpy(ek, key->key_iv, 16);
        os_memcpy(ek + 16, sm->ptk.kek, sm->ptk.kek_len);
        if (rc4_skip(ek, 32, 256, key_data, *key_data_len)) {
            wpa_printf(MSG_DEBUG, "WPA: RC4 failed");
            return -1;
        }
    } else if (ver == WPA_KEY_INFO_TYPE_HMAC_SHA1_AES ||
               ver == WPA_KEY_INFO_TYPE_AES_128_CMAC ||
               wpa_use_aes_key_wrap(sm->key_mgmt)) {
        u8 *buf;
        if (*key_data_len < 8 || *key_data_len % 8) {
            wpa_printf(MSG_DEBUG, "WPA: Unsupported "
                   "AES-WRAP len %u", (unsigned int) *key_data_len);
            return -1;
        }

        *key_data_len -= 8; /* AES-WRAP adds 8 bytes */
        /*replaced by xxx to remove malloc*/
        buf = ((u8 *) (key+1))+ 8;
             /*
        buf = os_wifi_malloc(keydatalen);
        if (buf == NULL) {
            wpa_printf(MSG_DEBUG, "WPA: No memory for "
                   "AES-UNWRAP buffer");
            return -1;
        }
        */
        if (aes_unwrap(sm->ptk.kek, sm->ptk.kek_len, *key_data_len / 8,
                       key_data, buf)) {
            wpa_printf(MSG_DEBUG, "WPA: AES unwrap failed - "
                       "could not decrypt EAPOL-Key key data");
            return -1;
        }
        os_memcpy(key_data, buf, *key_data_len);
        WPA_PUT_BE16(key->key_data_length, *key_data_len);
    } else {
        wpa_printf(MSG_DEBUG, "WPA: Unsupported key_info type %d",
               ver);
        return -1;
    }
    wpa_hexdump(MSG_DEBUG, "WPA: decrypted EAPOL-Key key data",
                key_data, *key_data_len);
    return 0;
}


static void wpa_eapol_key_dump(struct wpa_sm *sm,
                               const struct wpa_eapol_key *key,
                               unsigned int key_data_len,
                               const u8 *mic, unsigned int mic_len)
{
#ifdef DEBUG_PRINT
    u16 key_info = WPA_GET_BE16(key->key_info);

    wpa_printf(MSG_DEBUG, "  EAPOL-Key type=%d", key->type);
    wpa_printf(MSG_DEBUG, "  key_info 0x%x (ver=%d keyidx=%d rsvd=%d %s"
           "%s%s%s%s%s%s%s)",
           key_info, (u32)(key_info & WPA_KEY_INFO_TYPE_MASK),
           (u32)((key_info & WPA_KEY_INFO_KEY_INDEX_MASK) >>
           WPA_KEY_INFO_KEY_INDEX_SHIFT),
           (u32)((key_info & (BIT(13) | BIT(14) | BIT(15))) >> 13),
           key_info & WPA_KEY_INFO_KEY_TYPE ? "Pairwise" : "Group",
           key_info & WPA_KEY_INFO_INSTALL ? " Install" : "",
           key_info & WPA_KEY_INFO_ACK ? " Ack" : "",
           key_info & WPA_KEY_INFO_MIC ? " MIC" : "",
           key_info & WPA_KEY_INFO_SECURE ? " Secure" : "",
           key_info & WPA_KEY_INFO_ERROR ? " Error" : "",
           key_info & WPA_KEY_INFO_REQUEST ? " Request" : "",
           key_info & WPA_KEY_INFO_ENCR_KEY_DATA ? " Encr" : "");
    wpa_printf(MSG_DEBUG, "  key_length=%u key_data_length=%u",
               WPA_GET_BE16(key->key_length), key_data_len);
    wpa_hexdump(MSG_DEBUG, "  replay_counter",
                key->replay_counter, WPA_REPLAY_COUNTER_LEN);
    wpa_hexdump(MSG_DEBUG, "  key_nonce", key->key_nonce, WPA_NONCE_LEN);
    wpa_hexdump(MSG_DEBUG, "  key_iv", key->key_iv, 16);
    wpa_hexdump(MSG_DEBUG, "  key_rsc", key->key_rsc, 8);
    wpa_hexdump(MSG_DEBUG, "  key_id (reserved)", key->key_id, 8);
    wpa_hexdump(MSG_DEBUG, "  key_mic", mic, mic_len);
#endif
}

/**
 * wpa_sm_rx_eapol - Process received WPA EAPOL frames
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @src_addr: Source MAC address of the EAPOL packet
 * @buf: Pointer to the beginning of the EAPOL data (EAPOL header)
 * @len: Length of the EAPOL frame
 * Returns: 1 = WPA EAPOL-Key processed, 0 = not a WPA EAPOL-Key, -1 failure
 *
 * This function is called for each received EAPOL frame. Other than EAPOL-Key
 * frames can be skipped if filtering is done elsewhere. wpa_sm_rx_eapol() is
 * only processing WPA and WPA2 EAPOL-Key frames.
 *
 * The received EAPOL-Key packets are validated and valid packets are replied
 * to. In addition, key material (PTK, GTK) is configured at the end of a
 * successful key handshake.
 * buf begin from version, so remove mac header ,snap header and ether_type
 */
int wpa_sm_rx_eapol(u8 *src_addr, u8 *buf, u32 len)
{
    struct wpa_sm *sm = &gWpaSm;
    size_t plen, data_len, key_data_len;
    struct ieee802_1x_hdr *hdr;
    struct wpa_eapol_key *key;
    struct wpa_eapol_key_192 *key192;
    u16 key_info, ver;
    u8 *tmp;
    int ret = -1;
    size_t mic_len, keyhdrlen;
    u8 *key_data;

    mic_len = wpa_mic_len(sm->key_mgmt, sm->pmk_len);
    keyhdrlen = mic_len == 24 ? sizeof(*key192) : sizeof(*key);

    if (len < sizeof(*hdr) + keyhdrlen) {
        wpa_printf(MSG_DEBUG, "WPA: EAPOL frame too short to be a WPA "
                            "EAPOL-Key (len %lu, expecting at least %lu)",
                            (unsigned long) len,
                            (unsigned long) sizeof(*hdr) + sizeof(*key));
        return 0;
    }

    tmp = buf;

    hdr = (struct ieee802_1x_hdr *) tmp;
    key = (struct wpa_eapol_key *) (hdr + 1);
    key192 = (struct wpa_eapol_key_192 *)
            (tmp + sizeof(struct ieee802_1x_hdr));
    if (mic_len == 24)
        key_data = (u8 *) (key192 + 1);
    else
        key_data = (u8 *) (key + 1);

    plen = be_to_host16(hdr->length);
    data_len = plen + sizeof(*hdr);

    wpa_printf(MSG_DEBUG, "IEEE 802.1X RX: version=%d type=%d length=%d",
           hdr->version, hdr->type, plen);

    if (hdr->version < EAPOL_VERSION) {
        /* TODO: backwards compatibility */
    }
    if (hdr->type != IEEE802_1X_TYPE_EAPOL_KEY) {
        wpa_printf(MSG_DEBUG, "WPA: EAPOL frame (type %u) discarded, "
            "not a Key frame", hdr->type);
        ret = 0;
        goto out;
    }
    if (plen > len - sizeof(*hdr) || plen < keyhdrlen) {
        wpa_printf(MSG_DEBUG, "WPA: EAPOL frame payload size %lu "
               "invalid (frame size %lu)",
               (unsigned long) plen, (unsigned long) len);
        ret = 0;
        goto out;
    }

    if (key->type != EAPOL_KEY_TYPE_WPA && key->type != EAPOL_KEY_TYPE_RSN) {
        wpa_printf(MSG_DEBUG, "WPA: EAPOL-Key type (%d) unknown, "
               "discarded", key->type);
        ret = 0;
        goto out;
    }

    wpa_hexdump(MSG_MSGDUMP, "WPA: RX EAPOL-Key", tmp, len);

    if (data_len < len) {
        wpa_printf(MSG_DEBUG, "WPA: ignoring %lu bytes after the IEEE "
               "802.1X data", (unsigned long) len - data_len);
    }
    key_info = WPA_GET_BE16(key->key_info);
    ver = key_info & WPA_KEY_INFO_TYPE_MASK;

    if (ver != WPA_KEY_INFO_TYPE_HMAC_MD5_RC4 &&
#ifdef CONFIG_IEEE80211W
        ver != WPA_KEY_INFO_TYPE_AES_128_CMAC &&
#endif
        ver != WPA_KEY_INFO_TYPE_HMAC_SHA1_AES &&
        !wpa_use_akm_defined(sm->key_mgmt)) {
        wpa_printf(MSG_DEBUG, "WPA: Unsupported EAPOL-Key descriptor "
               "version %d.", ver);
        goto out;
    }
    if (wpa_use_akm_defined(sm->key_mgmt) &&
        ver != WPA_KEY_INFO_TYPE_AKM_DEFINED) {
        wpa_msg(NULL, MSG_INFO,
                "RSN: Unsupported EAPOL-Key descriptor version %d (expected AKM defined = 0)",
                ver);
        goto out;
    }

#ifdef CONFIG_IEEE80211W
    if (wpa_key_mgmt_sha256(sm->key_mgmt)) {
        if (ver != WPA_KEY_INFO_TYPE_AES_128_CMAC &&
            !wpa_use_akm_defined(sm->key_mgmt)) {
            goto out;
        }
    } else
#endif

    if (sm->pairwise_cipher == WPA_CIPHER_CCMP &&
        !wpa_use_akm_defined(sm->key_mgmt) &&
        ver != WPA_KEY_INFO_TYPE_HMAC_SHA1_AES) {
        wpa_printf(MSG_DEBUG, "WPA: CCMP is used, but EAPOL-Key "
               "descriptor version (%d) is not 2.", ver);
        if (sm->group_cipher != WPA_CIPHER_CCMP &&
            !(key_info & WPA_KEY_INFO_KEY_TYPE)) {
            /* Earlier versions of IEEE 802.11i did not explicitly
             * require version 2 descriptor for all EAPOL-Key
             * packets, so allow group keys to use version 1 if
             * CCMP is not used for them. */
              wpa_printf(MSG_DEBUG, "WPA: Backwards compatibility: "
                   "allow invalid version for non-CCMP group "
                   "keys");
        } else if (ver == WPA_KEY_INFO_TYPE_AES_128_CMAC) {
              wpa_printf(MSG_DEBUG,
                        "WPA: Interoperability workaround: allow incorrect (should have been HMAC-SHA1), but stronger      (is AES-128-CMAC), descriptor version to be used");
        } else
            goto out;
    }

#ifdef CONFIG_GCMP
    if (sm->pairwise_cipher == WPA_CIPHER_GCMP &&
        !wpa_use_akm_defined(sm->key_mgmt) &&
        ver != WPA_KEY_INFO_TYPE_HMAC_SHA1_AES) {
        wpa_msg(NULL, MSG_INFO,
                "WPA: GCMP is used, but EAPOL-Key "
                "descriptor version (%d) is not 2", ver);
        goto out;
    }
#endif

    if (sm->rx_replay_counter_set &&
        os_memcmp(key->replay_counter, sm->rx_replay_counter,
        WPA_REPLAY_COUNTER_LEN) <= 0) {
        wpa_printf(MSG_DEBUG, "WPA: EAPOL-Key Replay Counter did not"
               " increase - dropping packet");
        goto out;
    }

    if (!(key_info & (WPA_KEY_INFO_ACK | WPA_KEY_INFO_SMK_MESSAGE))) {
        wpa_printf(MSG_DEBUG, "WPA: No Ack bit in key_info");
        goto out;
    }

    if (key_info & WPA_KEY_INFO_REQUEST) {
        wpa_printf(MSG_DEBUG, "WPA: EAPOL-Key with Request bit - dropped");
        goto out;
    }

    if ((key_info & WPA_KEY_INFO_MIC) &&
        wpa_supplicant_verify_eapol_key_mic(sm, key192, ver, tmp, data_len))
        goto out;


    if (mic_len == 24)
        key_data_len = WPA_GET_BE16(key192->key_data_length);
    else
        key_data_len = WPA_GET_BE16(key->key_data_length);

    wpa_eapol_key_dump(sm, key, key_data_len, key192->key_mic, mic_len);


    if (key_data_len > plen - keyhdrlen) {
        wpa_printf(MSG_DEBUG, "WPA: Invalid EAPOL-Key "
                              "frame - key_data overflow (%d > %u)",
                              (unsigned int) key_data_len,
                              (unsigned int) (plen - keyhdrlen));
        goto out;
    }

    if (sm->proto == WPA_PROTO_RSN &&
        (key_info & WPA_KEY_INFO_ENCR_KEY_DATA) && mic_len) {
        /*
         * Only decrypt the Key Data field if the frame's authenticity
         * was verified. When using AES-SIV (FILS), the MIC flag is not
         * set, so this check should only be performed if mic_len != 0
         * which is the case in this code branch.
         */
        if (!(key_info & WPA_KEY_INFO_MIC)) {
            wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                    "WPA: Ignore EAPOL-Key with encrypted but unauthenticated data");
            goto out;
        }
        if (wpa_supplicant_decrypt_key_data(sm, key, ver, key_data,
                                            &key_data_len))
            goto out;
    }

    if (key_info & WPA_KEY_INFO_KEY_TYPE) {
        if (key_info & WPA_KEY_INFO_KEY_INDEX_MASK) {
            wpa_printf(MSG_DEBUG, "WPA: Ignored EAPOL-Key "
                   "(Pairwise) with non-zero key index");
            goto out;
        }

        if (key_info & WPA_KEY_INFO_MIC) {
            /* 3/4 4-Way Handshake */
           wpa_supplicant_process_3_of_4(sm, key, ver, key_data,
                                         key_data_len);
        } else {
            /* 1/4 4-Way Handshake */
            sm->eapol1_count++;
            if (sm->eapol1_count > MAX_EAPOL_RETRIES) {
                wpa_printf(MSG_INFO, "EAPOL1 received for %d times, sending deauth", sm->eapol1_count);
                esp_wifi_internal_issue_disconnect(WLAN_REASON_4WAY_HANDSHAKE_TIMEOUT);
                goto out;
            }
            wpa_supplicant_process_1_of_4(sm, src_addr, key,
                                          ver, key_data,
                                          key_data_len);
        }
    } else {
        if (key_info & WPA_KEY_INFO_MIC) {
            /* 1/2 Group Key Handshake */
            wpa_supplicant_process_1_of_2(sm, src_addr, key,
                                          key_data, key_data_len,
                                          ver);
        } else {
            wpa_printf(MSG_DEBUG, "WPA: EAPOL-Key (Group) "
                   "without Mic bit - dropped");
        }
    }

    ret = 1;

out:

    return ret;
}

/**
 * wpa_supplicant_set_state - Set current connection state
 * @wpa_s: Pointer to wpa_supplicant data
 * @state: The new connection state
 *
 * This function is called whenever the connection state changes, e.g.,
 * association is completed for WPA/WPA2 4-Way Handshake is started.
 */
void wpa_sm_set_state(enum wpa_states state)
{
    struct wpa_sm *sm = &gWpaSm;
    if(WPA_MIC_FAILURE==WPA_SM_STATE(sm))
    eloop_cancel_timeout(wpa_supplicant_stop_countermeasures, NULL, NULL);
    sm->wpa_state= state;
}


/**
 * wpa_sm_set_pmk - Set PMK
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @pmk: The new PMK
 * @pmk_len: The length of the new PMK in bytes
 * @bssid: AA to add into PMKSA cache or %NULL to not cache the PMK
 *
 * Configure the PMK for WPA state machine.
 */
void wpa_sm_set_pmk(struct wpa_sm *sm, const u8 *pmk, size_t pmk_len,
                    const u8 *pmkid, const u8 *bssid)
{
    if (sm == NULL)
        return;

    sm->pmk_len = pmk_len;
    os_memcpy(sm->pmk, pmk, pmk_len);

#ifdef CONFIG_IEEE80211R
    /* Set XXKey to be PSK for FT key derivation */
    sm->xxkey_len = pmk_len;
    os_memcpy(sm->xxkey, pmk, pmk_len);
#endif /* CONFIG_IEEE80211R */

    if (bssid) {
        pmksa_cache_add(sm->pmksa, pmk, pmk_len, pmkid, NULL, 0,
                        bssid, sm->own_addr,
                        sm->network_ctx, sm->key_mgmt);
    }
}


/**
 * wpa_sm_set_pmk_from_pmksa - Set PMK based on the current PMKSA
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 *
 * Take the PMK from the current PMKSA into use. If no PMKSA is active, the PMK
 * will be cleared.
 */
void wpa_sm_set_pmk_from_pmksa(struct wpa_sm *sm)
{
    if (sm == NULL)
        return;

    if (sm->cur_pmksa) {
        sm->pmk_len = sm->cur_pmksa->pmk_len;
        os_memcpy(sm->pmk, sm->cur_pmksa->pmk, sm->pmk_len);
    } else {
        sm->pmk_len = PMK_LEN_MAX;
        os_memset(sm->pmk, 0, PMK_LEN_MAX);
    }
}


bool wpa_sm_init(void)
{
    struct wpa_sm *sm = &gWpaSm;
    u16 spp_attrubute = 0;

    os_memset(sm, 0, sizeof(struct wpa_sm));

    sm->eapol_version = DEFAULT_EAPOL_VERSION;   /* DEFAULT_EAPOL_VERSION */

    spp_attrubute = esp_wifi_get_spp_attrubute_internal(WIFI_IF_STA);
    sm->spp_sup.capable = ((spp_attrubute & WPA_CAPABILITY_SPP_CAPABLE) ? SPP_AMSDU_CAP_ENABLE : SPP_AMSDU_CAP_DISABLE);
    sm->spp_sup.require = ((spp_attrubute & WPA_CAPABILITY_SPP_REQUIRED) ? SPP_AMSDU_REQ_ENABLE : SPP_AMSDU_REQ_DISABLE);

    wpa_sm_set_state(WPA_INACTIVE);

    sm->pmksa = pmksa_cache_init(wpa_sm_pmksa_free_cb, sm, sm);
    if (sm->pmksa == NULL) {
        wpa_printf(MSG_ERROR,
                "RSN: PMKSA cache initialization failed");
        return false;
    }
    return true;
}

/**
 *  * wpa_sm_deinit - Deinitialize WPA state machine
 *    */
void wpa_sm_deinit(void)
{
    struct wpa_sm *sm = &gWpaSm;
    pmksa_cache_deinit(sm->pmksa);
    sm->pmksa = NULL;
    os_free(sm->ap_rsnxe);
    sm->ap_rsnxe = NULL;
    os_free(sm->assoc_rsnxe);
    wpa_sm_drop_sa(sm);
    sm->assoc_rsnxe = NULL;
    memset(sm, 0, sizeof(*sm));
}


#ifdef ESP_SUPPLICANT
/**
 * wpa_sm_notify_assoc - Notify WPA state machine about association
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @bssid: The BSSID of the new association
 *
 * This function is called to let WPA state machine know that the connection
 * was established.
 */
void wpa_sm_notify_assoc(struct wpa_sm *sm, const u8 *bssid)
{
    int clear_keys = 1;

    if (sm == NULL)
        return;

    wpa_printf(MSG_DEBUG,
        "WPA: Association event - clear replay counter");
    os_memcpy(sm->bssid, bssid, ETH_ALEN);
    os_memset(sm->rx_replay_counter, 0, WPA_REPLAY_COUNTER_LEN);
    sm->rx_replay_counter_set = 0;
    sm->renew_snonce = 1;

#ifdef CONFIG_IEEE80211R
    if (wpa_ft_is_completed(sm)) {
        /*
         * Clear portValid to kick EAPOL state machine to re-enter
         * AUTHENTICATED state to get the EAPOL port Authorized.
         */
        wpa_supplicant_key_neg_complete(sm, sm->bssid, 1);

        /* Prepare for the next transition */
        wpa_ft_prepare_auth_request(sm, NULL);

        clear_keys = 0;
        sm->ft_protocol = 1;
    } else {
        sm->ft_protocol = 0;
    }
#endif /* CONFIG_IEEE80211R */
    if (clear_keys) {
        /*
         * IEEE 802.11, 8.4.10: Delete PTK SA on (re)association if
         * this is not part of a Fast BSS Transition.
         */
        wpa_printf(MSG_DEBUG, "WPA: Clear old PTK");
        sm->ptk_set = 0;
        os_memset(&sm->ptk, 0, sizeof(sm->ptk));
        sm->tptk_set = 0;
        os_memset(&sm->tptk, 0, sizeof(sm->tptk));
        os_memset(&sm->gtk, 0, sizeof(sm->gtk));
        os_memset(&sm->igtk, 0, sizeof(sm->igtk));
    }
}


/**
 * wpa_sm_notify_disassoc - Notify WPA state machine about disassociation
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 *
 * This function is called to let WPA state machine know that the connection
 * was lost. This will abort any existing pre-authentication session.
 */
void wpa_sm_notify_disassoc(struct wpa_sm *sm)
{
    eloop_cancel_timeout(wpa_sm_rekey_ptk, sm, NULL);
    pmksa_cache_clear_current(sm);
#ifdef CONFIG_IEEE80211R
    sm->ft_reassoc_completed = 0;
    sm->ft_protocol = 0;
#endif /* CONFIG_IEEE80211R */

    /* Keys are not needed in the WPA state machine anymore */
    wpa_sm_drop_sa(sm);

    os_memset(sm->bssid, 0, ETH_ALEN);
}


void wpa_set_profile(u32 wpa_proto, u8 auth_mode)
{
    struct wpa_sm *sm = &gWpaSm;

    sm->proto = wpa_proto;
    if (auth_mode == WPA2_AUTH_ENT) {
        sm->key_mgmt = WPA_KEY_MGMT_IEEE8021X; /* for wpa2 enterprise */
    } else if (auth_mode == WPA2_AUTH_ENT_SHA256) {
        sm->key_mgmt = WPA_KEY_MGMT_IEEE8021X_SHA256; /* for wpa2 enterprise sha256 */
    } else if (auth_mode == WPA2_AUTH_PSK_SHA256) {
        sm->key_mgmt = WPA_KEY_MGMT_PSK_SHA256;
    } else if (auth_mode == WPA3_AUTH_PSK) {
         sm->key_mgmt = WPA_KEY_MGMT_SAE; /* for WPA3 PSK */
    } else if (auth_mode == WAPI_AUTH_PSK) {
         sm->key_mgmt = WPA_KEY_MGMT_WAPI_PSK; /* for WAPI PSK */
    } else if (auth_mode == WPA2_AUTH_ENT_SHA384_SUITE_B) {
         sm->key_mgmt = WPA_KEY_MGMT_IEEE8021X_SUITE_B_192;
    } else if (auth_mode == WPA2_AUTH_FT_PSK) {
         sm->key_mgmt = WPA_KEY_MGMT_FT_PSK;
    } else if (auth_mode == WPA3_AUTH_OWE) {
         sm->key_mgmt = WPA_KEY_MGMT_OWE;
    } else {
        sm->key_mgmt = WPA_KEY_MGMT_PSK;  /* fixed to PSK for now */
    }
}

void wpa_set_pmk(uint8_t *pmk, size_t pmk_length, const u8 *pmkid, bool cache_pmksa)
{
    struct wpa_sm *sm = &gWpaSm;
    int pmk_len;

    if (wpa_key_mgmt_sha384(sm->key_mgmt))
        pmk_len = PMK_LEN_SUITE_B_192;
    else if (wpa_key_mgmt_sae(sm->key_mgmt))
        pmk_len = pmk_length;
    else
        pmk_len = PMK_LEN;

    memcpy(sm->pmk, pmk, pmk_len);
    sm->pmk_len = pmk_len;

    if (cache_pmksa) {
        pmksa_cache_add(sm->pmksa, pmk, PMK_LEN, pmkid, NULL, 0,
                        sm->bssid, sm->own_addr,
                        sm->network_ctx, sm->key_mgmt);
    }
}

int wpa_set_bss(char *macddr, char * bssid, u8 pairwise_cipher, u8 group_cipher, char *passphrase, u8 *ssid, size_t ssid_len)
{
    int res = 0;
    struct wpa_sm *sm = &gWpaSm;
    bool use_pmk_cache = !esp_wifi_skip_supp_pmkcaching();
    u8 assoc_rsnxe[20];
    size_t assoc_rsnxe_len = sizeof(assoc_rsnxe);

    /* Incase AP has changed it's SSID, don't try with PMK caching for SAE connection */
    /* Ideally we should use network_ctx for this purpose however currently network profile block
     * is part of libraries,
     * TODO Correct this in future during NVS restructuring */
    if ((sm->key_mgmt == WPA_KEY_MGMT_SAE) &&
        (os_memcmp(sm->bssid, bssid, ETH_ALEN) == 0) &&
        (os_memcmp(sm->ssid, ssid, ssid_len) != 0)) {
        use_pmk_cache = false;
    }
    sm->pairwise_cipher = BIT(pairwise_cipher);
    sm->group_cipher = BIT(group_cipher);
    sm->rx_replay_counter_set = 0;  //init state not intall replay counter value
    memset(sm->rx_replay_counter, 0, WPA_REPLAY_COUNTER_LEN);
    sm->wpa_ptk_rekey = 0;
    sm->renew_snonce = 1;
    memcpy(sm->own_addr, macddr, ETH_ALEN);
    memcpy(sm->bssid, bssid, ETH_ALEN);
    sm->ap_notify_completed_rsne = esp_wifi_sta_is_ap_notify_completed_rsne_internal();
    sm->use_ext_key_id = (sm->proto == WPA_PROTO_WPA);
    pmksa_cache_clear_current(sm);
    sm->sae_pwe = esp_wifi_get_config_sae_pwe_h2e_internal(WIFI_IF_STA);

    struct rsn_pmksa_cache_entry *pmksa = NULL;
    if (use_pmk_cache) {
        pmksa = pmksa_cache_get(sm->pmksa, (const u8 *)bssid, NULL, NULL);
        if (pmksa && (pmksa->akmp != sm->key_mgmt)) {
            use_pmk_cache = false;
        }
    }
    if (wpa_key_mgmt_supports_caching(sm->key_mgmt) && use_pmk_cache) {
        pmksa_cache_set_current(sm, NULL, (const u8*) bssid, 0, 0);
        wpa_sm_set_pmk_from_pmksa(sm);
    } else {
        if (pmksa) {
            pmksa_cache_flush(sm->pmksa, NULL, pmksa->pmk, pmksa->pmk_len);
        }
    }

    sm->eapol1_count = 0;
#ifdef CONFIG_IEEE80211W
    if (esp_wifi_sta_pmf_enabled()) {
        wifi_config_t wifi_cfg;
        wifi_cipher_type_t mgmt_cipher = esp_wifi_sta_get_mgmt_group_cipher();

        esp_wifi_get_config(WIFI_IF_STA, &wifi_cfg);
        sm->pmf_cfg = wifi_cfg.sta.pmf_cfg;
        sm->mgmt_group_cipher = cipher_type_map_public_to_supp(mgmt_cipher);
        if (sm->mgmt_group_cipher == WPA_CIPHER_NONE) {
                wpa_printf(MSG_ERROR, "mgmt_cipher %d not supported", mgmt_cipher);
                return -1;
	}
#ifdef CONFIG_SUITEB192
        extern bool g_wpa_suiteb_certification;
        if (g_wpa_suiteb_certification) {
            if (sm->mgmt_group_cipher != WPA_CIPHER_BIP_GMAC_256) {
                wpa_printf(MSG_ERROR, "suite-b 192bit certification, only GMAC256 is supported");
                return -1;
            }
            if (sm->group_cipher != WPA_CIPHER_GCMP_256) {
                wpa_printf(MSG_ERROR, "suite-b 192bit certification, only group GCMP256 is supported for group data cipher.");
                return -1;
            }
            if (sm->pairwise_cipher != WPA_CIPHER_GCMP_256) {
               wpa_printf(MSG_ERROR,"suite-b 192bit certification, only group GCMP256 is supported for pairwise cipher");
               return -1;
            }
            if (sm->key_mgmt != WPA_KEY_MGMT_IEEE8021X_SUITE_B_192) {
                wpa_printf(MSG_ERROR, "suite-b 192bit certification, 192bit akm supported");
                return -1;
            }
        }
#endif
    } else {
        memset(&sm->pmf_cfg, 0, sizeof(sm->pmf_cfg));
        sm->mgmt_group_cipher = WPA_CIPHER_NONE;
    }
#endif
#ifdef CONFIG_IEEE80211R
    if (sm->key_mgmt == WPA_KEY_MGMT_FT_PSK) {
        const u8 *ie, *md = NULL;
        struct wpa_bss *bss = wpa_bss_get_bssid(&g_wpa_supp, (uint8_t *)bssid);
        if (!bss) {
            return -1;
        }
        ie = wpa_bss_get_ie(bss, WLAN_EID_MOBILITY_DOMAIN);
        if (ie && ie[1] >= MOBILITY_DOMAIN_ID_LEN)
                md = ie + 2;
        if (os_memcmp(md, sm->mobility_domain, MOBILITY_DOMAIN_ID_LEN) != 0) {
            /* Reset Auth IE here */
            esp_wifi_unset_appie_internal(WIFI_APPIE_RAM_STA_AUTH);
            esp_wifi_unset_appie_internal(WIFI_APPIE_ASSOC_REQ);
            sm->ft_protocol = 0;
        }
        wpa_sm_set_ft_params(sm, ie, ie ? 2 + ie[1] : 0);
    } else {
        /* Reset FT parameters */
        wpa_sm_set_ft_params(sm, NULL, 0);
        esp_wifi_unset_appie_internal(WIFI_APPIE_RAM_STA_AUTH);
        esp_wifi_unset_appie_internal(WIFI_APPIE_ASSOC_REQ);
    }
#endif
    set_assoc_ie(assoc_ie_buf); /* use static buffer */
    res = wpa_gen_wpa_ie(sm, sm->assoc_wpa_ie, sm->assoc_wpa_ie_len);
    if (res < 0)
        return -1;
    sm->assoc_wpa_ie_len = res;

    const u8 *rsnxe;
    rsnxe = esp_wifi_sta_get_rsnxe((u8*)bssid);
    wpa_sm_set_ap_rsnxe(rsnxe, rsnxe ? (rsnxe[1] + 2) : 0);
    res = wpa_gen_rsnxe(sm, assoc_rsnxe, assoc_rsnxe_len);
    if (res < 0)
        return -1;
    assoc_rsnxe_len = res;
    res = wpa_sm_set_assoc_rsnxe(sm, assoc_rsnxe, assoc_rsnxe_len);
    if (res < 0)
        return -1;
    esp_set_assoc_ie((uint8_t *)bssid, assoc_rsnxe, assoc_rsnxe_len, true);
    os_memset(sm->ssid, 0, sizeof(sm->ssid));
    os_memcpy(sm->ssid, ssid, ssid_len);
    sm->ssid_len = ssid_len;
    wpa_set_passphrase(passphrase, ssid, ssid_len);
#ifdef CONFIG_MBO
    if (!mbo_bss_profile_match((u8 *)bssid))
        return -1;
#endif

#ifndef CONFIG_SAE_PK
    esp_wifi_sta_disable_sae_pk_internal();
#endif /* CONFIG_SAE_PK */
    return 0;
}

/*
 *  Call after set ssid since we calc pmk inside this routine directly
 */
  void
wpa_set_passphrase(char * passphrase, u8 *ssid, size_t ssid_len)
{
    struct wifi_ssid *sta_ssid = esp_wifi_sta_get_prof_ssid_internal();
    struct wpa_sm *sm = &gWpaSm;

    if (passphrase == NULL) return;

    /*
     *  Here only handle passphrase string.  Need extra step to handle 32B, 64Hex raw
     *    PMK.
     */
    if (sm->key_mgmt == WPA_KEY_MGMT_SAE || sm->key_mgmt == WPA_KEY_MGMT_OWE)
        return;

    /* This is really SLOW, so just re cacl while reset param */
    if (esp_wifi_sta_get_reset_nvs_pmk_internal() != 0) {
        // check it's psk
        if (strlen((char *)esp_wifi_sta_get_prof_password_internal()) == 64) {
            if (hexstr2bin((char *)esp_wifi_sta_get_prof_password_internal(),
                           esp_wifi_sta_get_ap_info_prof_pmk_internal(), PMK_LEN) != 0)
                return;
        } else {
            pbkdf2_sha1((char *)esp_wifi_sta_get_prof_password_internal(), sta_ssid->ssid, (size_t)sta_ssid->len,
                        4096, esp_wifi_sta_get_ap_info_prof_pmk_internal(), PMK_LEN);
        }
        esp_wifi_sta_update_ap_info_internal();
        esp_wifi_sta_set_reset_nvs_pmk_internal(0);
    }

    if (sm->key_mgmt == WPA_KEY_MGMT_IEEE8021X) {
    /* TODO nothing */
    } else {
        memcpy(sm->pmk, esp_wifi_sta_get_ap_info_prof_pmk_internal(), PMK_LEN);
        sm->pmk_len = PMK_LEN;
    }
#ifdef CONFIG_IEEE80211R
    /* Set XXKey to be PSK for FT key derivation */
    sm->xxkey_len = PMK_LEN;
    os_memcpy(sm->xxkey, sm->pmk, PMK_LEN);
#endif /* CONFIG_IEEE80211R */
}

  void
set_assoc_ie(u8 * assoc_buf)
{
    struct wpa_sm *sm = &gWpaSm;

    sm->assoc_wpa_ie = assoc_buf + 2;
    //wpa_ie insert OUI 4 byte before ver, but RSN have 2 bytes of RSN capability,
    // so wpa_ie have two more bytes than rsn_ie
    if ( sm->proto == WPA_PROTO_WPA)
         sm->assoc_wpa_ie_len = ASSOC_IE_LEN;
    else
         sm->assoc_wpa_ie_len = ASSOC_IE_LEN - 2;

    wpa_config_assoc_ie(sm->proto, assoc_buf, sm->assoc_wpa_ie_len);
}

int wpa_sm_set_key(struct install_key *key_sm, enum wpa_alg alg,
        u8 *addr, int key_idx, int set_tx,
        u8 *seq, size_t seq_len,
        u8 *key, size_t key_len,
        enum key_flag key_flag)
{
    struct wpa_sm *sm = &gWpaSm;

    /*gtk or ptk both need check countermeasures*/
    if (alg == WIFI_WPA_ALG_TKIP && key_idx == 0 && key_len == 32) {
        /* Clear the MIC error counter when setting a new PTK. */
        sm->mic_errors_seen = 0;
    }

    key_sm->keys_cleared = 0;
    key_sm->alg = alg;
    memcpy(key_sm->addr, addr, ETH_ALEN);
    key_sm->key_idx = key_idx;
    key_sm->set_tx = set_tx;
    memcpy(key_sm->key, key, key_len);

    wpa_install_key(alg, addr, key_idx, set_tx, seq, seq_len, key, key_len, key_flag);
    return 0;
}

static int
wpa_sm_get_key(uint8_t *ifx, int *alg, u8 *addr, int *key_idx, u8 *key, size_t key_len, enum key_flag key_flag)
{
    return wpa_get_key(ifx, alg, addr, key_idx, key, key_len, key_flag);
}

void wpa_supplicant_clr_countermeasures(u16 *pisunicast)
{
    struct wpa_sm *sm = &gWpaSm;
    sm->mic_errors_seen = 0;
    wpa_printf(MSG_DEBUG, "WPA: TKIP countermeasures clean");
}

/*recovery from countermeasures state, countermeasures state is period that stop connection with ap
  also used in wpa_init after connecting with ap
*/
void wpa_supplicant_stop_countermeasures(void *data, void *user_ctx)
{
    struct wpa_sm *sm = &gWpaSm;

    if (sm->countermeasures) {
        sm->countermeasures = 0;
        wpa_supplicant_clr_countermeasures(NULL);
        eloop_cancel_timeout(wpa_supplicant_stop_countermeasures, NULL, NULL);

        wpa_printf(MSG_DEBUG, "WPA: TKIP countermeasures stopped");
        /*renew scan preocess, this isn't done now*/
    }
    wpa_sm_set_state(WPA_DISCONNECTED);
}

int wpa_michael_mic_failure(u16 isunicast)
{
    struct wpa_sm *sm = &gWpaSm;

    wpa_printf(MSG_DEBUG, "TKIP MIC failure occur");

    if (sm->mic_errors_seen) {
        /* Send the new MIC error report immediately since we are going
         * to start countermeasures and AP better do the same.
         */
        wpa_sm_set_state(WPA_TKIP_COUNTERMEASURES);
        wpa_sm_key_request(sm, 1, isunicast);

        /* initialize countermeasures */
        sm->countermeasures = 1;
        wpa_printf(MSG_DEBUG, "TKIP countermeasures started");

        /*
         * Need to wait for completion of request frame. We do not get
         * any callback for the message completion, so just wait a
         * short while and hope for the best. */
         os_sleep(0, 10000);

        /*deauthenticate AP*/

        /*stop monitor next mic_failure timer,disconnect for 60sec, then stop contermeasures*/
        eloop_cancel_timeout(wpa_supplicant_stop_countermeasures, NULL, NULL);
        eloop_register_timeout(60, 0, wpa_supplicant_stop_countermeasures, NULL, NULL);

        /* TODO: mark the AP rejected for 60 second. STA is
         * allowed to associate with another AP.. */
    } else {
        sm->mic_errors_seen++;
        wpa_sm_set_state(WPA_MIC_FAILURE);
        wpa_sm_key_request(sm, 1, isunicast);
        /*start 60sec counter to monitor whether next mic_failure occur in this period, or clear mic_errors_seen*/
        eloop_cancel_timeout(wpa_supplicant_stop_countermeasures, NULL, NULL);
        eloop_register_timeout(60, 0, wpa_supplicant_stop_countermeasures, NULL, NULL);
    }

    return 0;
}

/*
   eapol tx callback function to make sure new key
    install after 4-way handoff
*/
void eapol_txcb(uint8_t *eapol_payload, size_t len, bool tx_failure)
{
    struct ieee802_1x_hdr *hdr;
    struct wpa_eapol_key *key;
    struct wpa_sm *sm = &gWpaSm;
    u8 isdeauth = 0;  //no_zero value is the reason for deauth

    if (len < sizeof(struct ieee802_1x_hdr)) {
        /* Invalid 802.1X header, ignore */
        return;
    }
    hdr = (struct ieee802_1x_hdr *) eapol_payload;
    if (hdr->type != IEEE802_1X_TYPE_EAPOL_KEY) {
        /* Ignore EAPOL non-key frames */
        return;
    }
    if (len < (sizeof(struct ieee802_1x_hdr) + sizeof(struct wpa_eapol_key))) {
        wpa_printf(MSG_ERROR, "EAPOL TxDone with invalid payload len! (len - %zu)", len);
        return;
    }
    key = (struct wpa_eapol_key *) (hdr + 1);

    switch(WPA_SM_STATE(sm)) {
        case WPA_FIRST_HALF_4WAY_HANDSHAKE:
        case WPA_LAST_HALF_4WAY_HANDSHAKE:
            if (WPA_GET_BE16(key->key_data_length) == 0 ||
                    (WPA_GET_BE16(key->key_info) & WPA_KEY_INFO_SECURE)) {
                /* msg 4/4 Tx Done */
                if (tx_failure) {
                    wpa_printf(MSG_ERROR, "Eapol message 4/4 tx failure, not installing keys");
                    return;
                }

                if (sm->txcb_flags & WPA_4_4_HANDSHAKE_BIT) {
                    sm->txcb_flags &= ~WPA_4_4_HANDSHAKE_BIT;
                    isdeauth = wpa_supplicant_send_4_of_4_txcallback(sm);
                } else {
                    wpa_printf(MSG_DEBUG, "4/4 txcb, flags=%d", sm->txcb_flags);
                }
            } else {
                /* msg 2/4 Tx Done */
                wpa_printf(MSG_DEBUG, "2/4 txcb, flags=%d, txfail %d", sm->txcb_flags, tx_failure);
            }
            break;
        case WPA_GROUP_HANDSHAKE:
            if (sm->txcb_flags & WPA_GROUP_HANDSHAKE_BIT) {
                sm->txcb_flags &= ~WPA_GROUP_HANDSHAKE_BIT;
            } else {
                wpa_printf(MSG_DEBUG, "2/2 txcb, flags=%d", sm->txcb_flags);
            }
            break;
        case WPA_TKIP_COUNTERMEASURES: isdeauth=WLAN_REASON_MICHAEL_MIC_FAILURE;
            break;
        default: break;
    }

    if(isdeauth) {
        wpa_sm_deauthenticate(sm, isdeauth);
    }
}

bool wpa_sta_in_4way_handshake(void)
{
    struct wpa_sm *sm = &gWpaSm;
    if ( WPA_SM_STATE(sm) == WPA_MIC_FAILURE || WPA_SM_STATE(sm) == WPA_FIRST_HALF_4WAY_HANDSHAKE
        || WPA_SM_STATE(sm) == WPA_LAST_HALF_4WAY_HANDSHAKE) {
        return true;
    }
    return false;
}

bool wpa_sta_is_cur_pmksa_set(void) {
    struct wpa_sm *sm = &gWpaSm;
    return (pmksa_cache_get_current(sm) != NULL);
}

bool wpa_sta_cur_pmksa_matches_akm(void) {
    struct wpa_sm *sm = &gWpaSm;
    struct rsn_pmksa_cache_entry *pmksa;

    pmksa = pmksa_cache_get_current(sm);
    return (pmksa != NULL  &&
            sm->key_mgmt == pmksa->akmp);
}

void wpa_sta_clear_curr_pmksa(void) {
    struct wpa_sm *sm = &gWpaSm;

    if (sm->pmksa)
        pmksa_cache_flush(sm->pmksa, NULL, sm->pmk, sm->pmk_len);
    pmksa_cache_clear_current(sm);
}

struct wpa_sm * get_wpa_sm(void)
{
    return &gWpaSm;
}

int wpa_sm_set_ap_rsnxe(const u8 *ie, size_t len)
{
    struct wpa_sm *sm = &gWpaSm;

    os_free(sm->ap_rsnxe);
    if (!ie || len == 0) {
        wpa_hexdump(MSG_DEBUG, "WPA: set AP RSNXE", ie, len);
        sm->ap_rsnxe = NULL;
        sm->ap_rsnxe_len = 0;
    } else {
        wpa_hexdump(MSG_DEBUG, "WPA: set AP RSNXE", ie, len);
        sm->ap_rsnxe = os_memdup(ie, len);
        if (!sm->ap_rsnxe)
            return -1;

        sm->ap_rsnxe_len = len;
    }

    if (sm->ap_rsnxe != NULL) {
        sm->sae_pwe = esp_wifi_get_config_sae_pwe_h2e_internal(WIFI_IF_STA);
#ifdef CONFIG_SAE_PK
        const u8 *pw = (const u8 *)esp_wifi_sta_get_prof_password_internal();
        if (esp_wifi_sta_get_config_sae_pk_internal() != WPA3_SAE_PK_MODE_DISABLED &&
                sae_pk_valid_password((const char*)pw)) {
            sm->sae_pk = true;
        }
#endif /* CONFIG_SAE_PK */
    }
    return 0;
}


int wpa_sm_set_assoc_rsnxe(struct wpa_sm *sm, const u8 *ie, size_t len)
{
    if (!sm)
        return -1;

    os_free(sm->assoc_rsnxe);
    if (!ie || len == 0) {
        sm->assoc_rsnxe = NULL;
        sm->assoc_rsnxe_len = 0;
    } else {
        wpa_hexdump(MSG_DEBUG, "RSN: set own RSNXE", ie, len);
        sm->assoc_rsnxe = os_memdup(ie, len);
        if (!sm->assoc_rsnxe)
            return -1;

        sm->assoc_rsnxe_len = len;
    }

    return 0;
}

void wpa_sm_drop_sa(struct wpa_sm *sm)
{
    wpa_printf(MSG_DEBUG, "WPA: Clear old PMK and PTK");
    sm->ptk_set = 0;
    sm->tptk_set = 0;
    sm->pmk_len = 0;
    os_memset(&sm->ptk, 0, sizeof(sm->ptk));
    os_memset(&sm->tptk, 0, sizeof(sm->tptk));
    os_memset(&sm->gtk, 0, sizeof(sm->gtk));
    os_memset(&sm->igtk, 0, sizeof(sm->igtk));
}

#ifdef CONFIG_OWE_STA
struct wpabuf *owe_build_assoc_req(struct wpa_sm *sm, u16 group)
{
    struct wpabuf *pub = NULL;
    size_t prime_len;

    if (group == OWE_DH_GRP19) {
        prime_len = OWE_PRIME_LEN;
    } else {
        wpa_printf(MSG_ERROR, "OWE: Unsupported Diffie-Hellman group");
        return NULL;
    }

    sm->owe_ecdh = crypto_ecdh_init(group);

    if (!sm->owe_ecdh) {
        wpa_printf(MSG_ERROR, "Initialization of ecdh failed");
        return NULL;
    }

    sm->owe_group = group;
    /* Get own public key */
    pub = crypto_ecdh_get_pubkey(sm->owe_ecdh, 0);
    pub = wpabuf_zeropad(pub, prime_len);

    if (!pub) {
        wpa_printf(MSG_ERROR, "Own public key is NULL");
        goto fail;
    }

    wpa_hexdump_buf(MSG_DEBUG, "Own public key", pub);

    if (sm->owe_ie) {
        wpabuf_free(sm->owe_ie);
    }
    sm->owe_ie = wpabuf_alloc(5 + wpabuf_len(pub));

    if (!sm->owe_ie) {
        wpa_printf(MSG_ERROR, "OWE IE allocation failed");
        goto fail;
    }

    /* Constructing the DH IE */
    wpabuf_put_u8(sm->owe_ie, WLAN_EID_EXTENSION);
    wpabuf_put_u8(sm->owe_ie, 1 + 2 + wpabuf_len(pub));
    wpabuf_put_u8(sm->owe_ie, WLAN_EID_EXT_OWE_DH_PARAM);
    wpabuf_put_le16(sm->owe_ie, group);
    wpabuf_put_buf(sm->owe_ie, pub);
    wpabuf_free(pub);

    wpa_hexdump_buf(MSG_DEBUG, "OWE: Diffie-Hellman Parameter element", sm->owe_ie);

    return (struct wpabuf *)wpabuf_head(sm->owe_ie);

fail:
    wpabuf_free(pub);
    crypto_ecdh_deinit(sm->owe_ecdh);
    return NULL;
}

int owe_process_assoc_resp(const u8 *rsn_ie, size_t rsn_len, const uint8_t *dh_ie, size_t dh_len)
{
    size_t prime_len=0,hash_len=0;
    struct wpabuf * sh_secret = NULL, *pub = NULL, *hkey = NULL;
    const char *info = "OWE Key Generation";
    u8 pmkid[SHA256_MAC_LEN], prk[SHA256_MAC_LEN], pmk[SHA256_MAC_LEN];
    const u8 *addr[2];
    size_t len[2];
    u16 group;

    struct wpa_sm *sm;
    sm = get_wpa_sm();

    /* Deallocate the dh ie buffer constructed in owe_build_assoc_req */
    wpabuf_free(sm->owe_ie);
    sm->owe_ie = NULL;

    struct wpa_ie_data *parsed_rsn_data;
    parsed_rsn_data = os_zalloc(sizeof(struct wpa_ie_data));
    if (!parsed_rsn_data) {
        wpa_printf(MSG_ERROR, "Memory allocation failed");
        return -1;
    }

    if (rsn_ie && rsn_len && wpa_parse_wpa_ie_rsn(rsn_ie, rsn_len + 2, parsed_rsn_data) != 0) {
        goto fail;
    }

    if (!dh_ie && parsed_rsn_data->num_pmkid == 0) {
        wpa_printf(MSG_ERROR, "OWE: Assoc response should either have pmkid or DH IE");
        goto fail;
    }

    /* Check for PMK caching */
    if (sm->cur_pmksa && parsed_rsn_data && parsed_rsn_data->num_pmkid == 1 && parsed_rsn_data->pmkid) {
        if (os_memcmp(parsed_rsn_data->pmkid, sm->cur_pmksa->pmkid, OWE_PMKID_LEN) == 0) {
            wpa_printf(MSG_DEBUG, "OWE: Using PMK caching");
            wpa_sm_set_pmk_from_pmksa(sm);
            goto done;
        } else {
            /* If PMKID mismatches, derive keys again */
            wpa_printf(MSG_DEBUG, "OWE : Invalid PMKID in response");
        }
    }

    if (dh_ie == NULL) {
        wpa_printf(MSG_ERROR, "OWE: No Diffie Hellman IE in association response");
        goto fail;
    }
    if (dh_ie && MIN_DH_LEN(dh_len)) {
        wpa_printf(MSG_ERROR, "OWE: Invalid Diffie Hellman IE");
        goto fail;
    }

    /* If STA or AP does not have PMKID, or PMKID mismatches, proceed with normal association */
    dh_len += 2;

    dh_ie += 3;
    dh_len -=3;
    group = WPA_GET_LE16(dh_ie);

    /* Only group 19 is supported */
    if ((group != sm->owe_group) || (group != OWE_DH_GRP19)) {
        wpa_printf(MSG_ERROR, "OWE: Unexpected Diffie-Hellman group in response");
        goto fail;
    }

    prime_len = OWE_PRIME_LEN;

    /* Set peer's public key point and calculate shared secret */
    sh_secret = crypto_ecdh_set_peerkey(sm->owe_ecdh, 0, dh_ie+2, dh_len-2);
    sh_secret = wpabuf_zeropad(sh_secret, prime_len);
    if (!sh_secret) {
        wpa_printf(MSG_ERROR, "OWE: Invalid peer DH public key");
        goto fail;
    }

    wpa_hexdump_buf_key(MSG_DEBUG, "OWE: DH shared secret", sh_secret);
    pub = crypto_ecdh_get_pubkey(sm->owe_ecdh, 0);
    if (!pub) {
        wpa_printf(MSG_ERROR, "No own public key");
        goto fail;
    }

    /* PMKID = Truncate-128(Hash(C | A)) */
    addr[0] = wpabuf_head(pub);
    len[0] = wpabuf_len(pub);
    addr[1] = dh_ie + 2;
    len[1] = dh_len - 2;

    int res = sha256_vector(2, addr, len, pmkid);
    if (res < 0 ) {
        goto fail;
    }

    hash_len = SHA256_MAC_LEN;

    pub = wpabuf_zeropad(pub, prime_len);
    if (!pub) {
        goto fail;
    }

    /* prk = HKDF-extract(C | A | group, z) */
    hkey = wpabuf_alloc(wpabuf_len(pub) + dh_len - 2 + 2);
    if (!hkey) {
        goto fail;
    }

    wpabuf_put_buf(hkey, pub); /* C */
    wpabuf_free(pub);

    wpabuf_put_data(hkey, dh_ie + 2, dh_len - 2); /* A */
    wpabuf_put_le16(hkey, sm->owe_group); /* group */

    res = hmac_sha256(wpabuf_head(hkey), wpabuf_len(hkey), wpabuf_head(sh_secret), wpabuf_len(sh_secret), prk);
    if (res < 0 ) {
        goto fail;
    }

    hash_len = SHA256_MAC_LEN;

    wpabuf_free(hkey);
    wpabuf_clear_free(sh_secret);

    wpa_hexdump_key(MSG_DEBUG, "OWE: prk", prk, hash_len);

    /* PMK = HKDF-expand(prk, "OWE Key Generation", n) */
    res = hmac_sha256_kdf(prk, hash_len, NULL, (const u8 *)info,
    os_strlen(info), pmk, hash_len);
    if (res < 0 ) {
        goto fail;
    }

    forced_memzero(prk, SHA256_MAC_LEN);
    wpa_hexdump(MSG_DEBUG, "OWE: PMKID", pmkid, OWE_PMKID_LEN);

    os_memcpy(sm->pmk,pmk,hash_len);
    sm->pmk_len = hash_len;
    wpa_hexdump_key(MSG_DEBUG, "OWE: PMK", sm->pmk, sm->pmk_len);

    pmksa_cache_add(sm->pmksa, sm->pmk, sm->pmk_len, pmkid, NULL, 0,
                        sm->bssid, sm->own_addr, sm->network_ctx, sm->key_mgmt);

done:
    os_free(parsed_rsn_data);
    return 0;
fail:
    os_free(parsed_rsn_data);
    wpabuf_free(pub);
    wpabuf_free(hkey);
    wpabuf_clear_free(sh_secret);
    return -1;
}
#endif // CONFIG_OWE_STA


void wpa_sm_pmksa_cache_flush(struct wpa_sm *sm, void *network_ctx)
{
    if (sm->pmksa) {
        pmksa_cache_flush(sm->pmksa, network_ctx, NULL, 0);
    }
}
#endif // ESP_SUPPLICANT
