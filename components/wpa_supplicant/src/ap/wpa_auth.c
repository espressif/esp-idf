/*
 * IEEE 802.11 RSN / WPA Authenticator
 * Copyright (c) 2004-2011, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"
#include "utils/common.h"
#include "utils/eloop.h"
#include "utils/state_machine.h"
#include "common/ieee802_11_defs.h"
#include "common/sae.h"
#include "ap/sta_info.h"
#include "ap/ieee802_11.h"
#include "ap/wpa_auth.h"
#include "ap/wpa_auth_i.h"
#include "ap/wpa_auth_ie.h"
#include "utils/wpa_debug.h"
#include "hostapd.h"
#include "rsn_supp/wpa.h"
#include "ap/ap_config.h"
#include "ap/sta_info.h"
#include "common/wpa_common.h"
#include "ap/pmksa_cache_auth.h"

#include "crypto/aes.h"
#include "crypto/aes_wrap.h"
#include "crypto/crypto.h"
#include "crypto/sha1.h"
#include "crypto/sha256.h"
#include "crypto/random.h"

#include "esp_wifi_driver.h"
#include "esp_wifi.h"
#include "esp_private/wifi.h"
#include "esp_wpas_glue.h"
#include "esp_wps_i.h"
#include "esp_hostap.h"

#define STATE_MACHINE_DATA struct wpa_state_machine
#define STATE_MACHINE_DEBUG_PREFIX "WPA"
#define STATE_MACHINE_ADDR sm->addr


static int wpa_sm_step(struct wpa_state_machine *sm);
static int wpa_verify_key_mic(int akmp, struct wpa_ptk *PTK, u8 *data,
			      size_t data_len);
static void wpa_group_sm_step(struct wpa_authenticator *wpa_auth,
			      struct wpa_group *group);
static void wpa_request_new_ptk(struct wpa_state_machine *sm);
static int wpa_gtk_update(struct wpa_authenticator *wpa_auth,
			  struct wpa_group *group);
static int wpa_group_config_group_keys(struct wpa_authenticator *wpa_auth,
				       struct wpa_group *group);

static const u32 dot11RSNAConfigGroupUpdateCount = 4;
static const u32 dot11RSNAConfigPairwiseUpdateCount = 4;

#define WPA_SM_MAX_INDEX 16
static void *s_sm_table[WPA_SM_MAX_INDEX];
static u32 s_sm_valid_bitmap = 0;
void resend_eapol_handle(void *data, void *user_ctx);

static struct wpa_state_machine * wpa_auth_get_sm(u32 index)
{
    if ( (index < WPA_SM_MAX_INDEX) && (BIT(index) & s_sm_valid_bitmap)){
        return s_sm_table[index];
    }

    return NULL;
}

static void wpa_auth_add_sm(struct wpa_state_machine *sm)
{
    if (sm) {
        u8 i;
        for (i=0; i<WPA_SM_MAX_INDEX; i++) {
            if (BIT(i) & s_sm_valid_bitmap) {
                if (s_sm_table[i] == sm) {
                    wpa_printf( MSG_INFO, "add sm already exist i=%d", i);
                }
                continue;
            }
            s_sm_table[i] = sm;
            s_sm_valid_bitmap |= BIT(i);
            sm->index = i;
            wpa_printf( MSG_DEBUG, "add sm, index=%d bitmap=%x", i, s_sm_valid_bitmap);
            return;
        }
    }
}

static void wpa_auth_del_sm(struct wpa_state_machine *sm)
{
    if (sm && (sm->index < WPA_SM_MAX_INDEX)) {
        if (sm != s_sm_table[sm->index]) {
            wpa_printf( MSG_INFO, "del sm error %d", sm->index);
        }
        s_sm_table[sm->index] = NULL;
        s_sm_valid_bitmap &= ~BIT(sm->index);
        wpa_printf( MSG_DEBUG, "del sm, index=%d bitmap=%x", sm->index, s_sm_valid_bitmap);
    }
}

static inline int wpa_auth_mic_failure_report(
    struct wpa_authenticator *wpa_auth, const u8 *addr)
{
    return 0;
}


static inline void wpa_auth_set_eapol(struct wpa_authenticator *wpa_auth,
                      const u8 *addr, wpa_eapol_variable var,
                      int value)
{
}


static inline int wpa_auth_get_eapol(struct wpa_authenticator *wpa_auth,
                     const u8 *addr, wpa_eapol_variable var)
{
    return -1;
}

static inline const u8 * wpa_auth_get_psk(struct wpa_authenticator *wpa_auth,
                      const u8 *addr, const u8 *prev_psk)
{
    struct hostapd_data *hapd = (struct hostapd_data *)esp_wifi_get_hostap_private_internal();

    if (!hapd){
        return NULL;
    }

#ifdef CONFIG_SAE
    struct sta_info *sta = ap_get_sta(hapd, addr);
    if (sta && sta->auth_alg == WLAN_AUTH_SAE) {
        if (!sta->sae || prev_psk)
            return NULL;
        return sta->sae->pmk;
    }
    if (sta && wpa_auth_uses_sae(sta->wpa_sm)) {
        wpa_printf(MSG_DEBUG,
               "No PSK for STA trying to use SAE with PMKSA caching");
        return NULL;
    }
#endif /*CONFIG_SAE*/

    return (u8*)hostapd_get_psk(hapd->conf, addr, prev_psk);
}

static inline int wpa_auth_get_msk(struct wpa_authenticator *wpa_auth,
                   const u8 *addr, u8 *msk, size_t *len)
{
    return -1;
}

static inline int wpa_auth_set_key(struct wpa_authenticator *wpa_auth,
                   int vlan_id,
                   enum wpa_alg alg, const u8 *addr, int idx,
                   u8 *key, size_t key_len)
{
    int ret;

    if (alg == WIFI_WPA_ALG_IGTK) {
	if (key) {
	    wpa_printf (MSG_DEBUG, "%s : igtk idx %d", __func__, idx);
	    wifi_wpa_igtk_t *igtk = os_malloc(sizeof(wifi_wpa_igtk_t));

	    if (igtk != NULL) {
		memcpy(&igtk->igtk[0], key, WPA_IGTK_LEN);
		memset((uint8_t*)&igtk->pn[0],0,6);
		igtk->keyid[0] = idx;
		igtk->keyid[1] = 0;

	    } else {
                return -1;
	    }
	    ret = esp_wifi_set_igtk_internal(ESP_IF_WIFI_AP, igtk);
	    os_free(igtk);
	    return ret;

	} else {
            wpa_printf( MSG_DEBUG, "Key is empty");
            return -1;
	}
    } else {
	wpa_printf( MSG_DEBUG, "%s : key idx  %d alg %d vlan_id %d key_len %d key", __func__, idx, alg, vlan_id, key_len);
	return esp_wifi_set_ap_key_internal(alg, addr, idx, key, key_len);
    }
    return 0;
}


static inline int wpa_auth_get_seqnum(struct wpa_authenticator *wpa_auth,
                      const u8 *addr, int idx, u8 *seq)
{
    return -1;
}

/* fix buf for tx for now */
#define WPA_TX_MSG_BUFF_MAXLEN 200

static inline int
wpa_auth_send_eapol(struct wpa_authenticator *wpa_auth, const u8 *addr,
            const u8 *data, size_t data_len, int encrypt)
{
    return hostapd_send_eapol(wpa_auth->addr, addr, data, data_len);
}

int wpa_auth_for_each_sta(struct wpa_authenticator *wpa_auth,
              int (*cb)(struct wpa_state_machine *sm, void *ctx),
              void *cb_ctx)
{
    return 0;
}

static void wpa_sta_disconnect(struct wpa_authenticator *wpa_auth,
                   const u8 *addr, u16 reason)
{
    wpa_printf(MSG_DEBUG, "wpa_sta_disconnect STA " MACSTR, MAC2STR(addr));
    esp_wifi_ap_deauth_internal((uint8_t*)addr, reason);
    return;
}

static int wpa_use_aes_cmac(struct wpa_state_machine *sm)
{
    int ret = 0;
#ifdef CONFIG_IEEE80211R_AP
    if (wpa_key_mgmt_ft(sm->wpa_key_mgmt))
        ret = 1;
#endif /* CONFIG_IEEE80211R */
#ifdef CONFIG_IEEE80211W
    if (wpa_key_mgmt_sha256(sm->wpa_key_mgmt))
        ret = 1;
#endif /* CONFIG_IEEE80211W */
    return ret;
}

static void wpa_rekey_gtk(void *eloop_ctx, void *timeout_ctx)
{
    struct wpa_authenticator *wpa_auth = eloop_ctx;
    struct wpa_group *group;

    for (group = wpa_auth->group; group; group = group->next) {
        group->GTKReKey = TRUE;
        do {
            group->changed = FALSE;
            wpa_group_sm_step(wpa_auth, group);
        } while (group->changed);
    }

    if (wpa_auth->conf.wpa_group_rekey) {
        eloop_register_timeout(wpa_auth->conf.wpa_group_rekey,
                       0, wpa_rekey_gtk, wpa_auth, NULL);
    }
}


static void wpa_rekey_ptk(void *eloop_ctx, void *timeout_ctx)
{
    struct wpa_state_machine *sm = timeout_ctx;

    wpa_request_new_ptk(sm);
    wpa_sm_step(sm);
}


static int wpa_auth_pmksa_clear_cb(struct wpa_state_machine *sm, void *ctx)
{
    if (sm->pmksa == ctx)
        sm->pmksa = NULL;
    return 0;
}


static void wpa_auth_pmksa_free_cb(struct rsn_pmksa_cache_entry *entry,
                   void *ctx)
{
    struct wpa_authenticator *wpa_auth = ctx;
    wpa_auth_for_each_sta(wpa_auth, wpa_auth_pmksa_clear_cb, entry);
}


static int wpa_group_init_gmk_and_counter(struct wpa_authenticator *wpa_auth,
                      struct wpa_group *group)
{
    u8 buf[ETH_ALEN + 8 + sizeof(group)];
    u8 rkey[32];

    if (os_get_random(group->GMK, WPA_GMK_LEN) < 0)
        return -1;
    wpa_hexdump_key(MSG_DEBUG, "GMK", group->GMK, WPA_GMK_LEN);

    /*
     * Counter = PRF-256(Random number, "Init Counter",
     *                   Local MAC Address || Time)
     */
    memcpy(buf, wpa_auth->addr, ETH_ALEN);
    wpa_get_ntp_timestamp(buf + ETH_ALEN);
    memcpy(buf + ETH_ALEN + 8, &group, sizeof(group));
    if (os_get_random(rkey, sizeof(rkey)) < 0)
        return -1;

    if (sha1_prf(rkey, sizeof(rkey), "Init Counter", buf, sizeof(buf),
            group->Counter, WPA_NONCE_LEN) < 0)
        return -1;
    wpa_hexdump_key(MSG_DEBUG, "Key Counter",
            group->Counter, WPA_NONCE_LEN);

    return 0;
}

static struct wpa_group * wpa_group_init(struct wpa_authenticator *wpa_auth,
                     int vlan_id, int delay_init)
{
    struct wpa_group *group;
    group = (struct wpa_group *)os_zalloc(sizeof(struct wpa_group));
    if (group == NULL)
        return NULL;

    group->GTKAuthenticator = TRUE;
    group->GTK_len = wpa_cipher_key_len(wpa_auth->conf.wpa_group);

    if (random_pool_ready() != 1) {
        wpa_printf( MSG_INFO, "WPA: Not enough entropy in random pool "
               "for secure operations - update keys later when "
               "the first station connects");
    }

    /*
     * Set initial GMK/Counter value here. The actual values that will be
     * used in negotiations will be set once the first station tries to
     * connect. This allows more time for collecting additional randomness
     * on embedded devices.
     */
    if (wpa_group_init_gmk_and_counter(wpa_auth, group) < 0) {
        wpa_printf( MSG_ERROR, "Failed to get random data for WPA "
               "initialization.");
        os_free(group);
        return NULL;
    }

    group->GInit = TRUE;
    if (delay_init) {
        wpa_printf( MSG_DEBUG, "WPA: Delay group state machine start "
               "until Beacon frames have been configured");
        /* Initialization is completed in wpa_init_keys(). */
    } else {
        wpa_group_sm_step(wpa_auth, group);
        group->GInit = FALSE;
        wpa_group_sm_step(wpa_auth, group);
    }

    return group;
}


/**
 * wpa_init - Initialize WPA authenticator
 * @addr: Authenticator address
 * @conf: Configuration for WPA authenticator
 * @cb: Callback functions for WPA authenticator
 * Returns: Pointer to WPA authenticator data or %NULL on failure
 */
struct wpa_authenticator * wpa_init(const u8 *addr,
                    struct wpa_auth_config *conf,
                    struct wpa_auth_callbacks *cb)
{
    struct wpa_authenticator *wpa_auth;
    wpa_auth = (struct wpa_authenticator *)os_zalloc(sizeof(struct wpa_authenticator));
    if (wpa_auth == NULL)
        return NULL;
    memcpy(wpa_auth->addr, addr, ETH_ALEN);
    memcpy(&wpa_auth->conf, conf, sizeof(*conf));

    if (wpa_auth_gen_wpa_ie(wpa_auth)) {
        wpa_printf( MSG_ERROR, "Could not generate WPA IE.");
        os_free(wpa_auth);
        return NULL;
    }

    wpa_auth->group = wpa_group_init(wpa_auth, 0, 0);
    if (wpa_auth->group == NULL) {
        os_free(wpa_auth->wpa_ie);
        os_free(wpa_auth);
        return NULL;
    }


    wpa_auth->pmksa = pmksa_cache_auth_init(wpa_auth_pmksa_free_cb,
                        wpa_auth);
    if (wpa_auth->pmksa == NULL) {
        wpa_printf(MSG_ERROR, "PMKSA cache initialization failed.");
        os_free(wpa_auth->group);
        os_free(wpa_auth->wpa_ie);
        os_free(wpa_auth);
        return NULL;
    }

#ifdef CONFIG_IEEE80211R_AP
    wpa_auth->ft_pmk_cache = wpa_ft_pmk_cache_init();
    if (wpa_auth->ft_pmk_cache == NULL) {
        wpa_printf( MSG_ERROR, "FT PMK cache initialization failed.");
        os_free(wpa_auth->wpa_ie);
        pmksa_cache_auth_deinit(wpa_auth->pmksa);
        os_free(wpa_auth);
        return NULL;
    }
#endif /* CONFIG_IEEE80211R_AP */

    return wpa_auth;
}

struct wpa_state_machine *
wpa_auth_sta_init(struct wpa_authenticator *wpa_auth, const u8 *addr)
{
    struct wpa_state_machine *sm;

    sm = (struct wpa_state_machine *)os_zalloc(sizeof(struct wpa_state_machine));
    if (sm == NULL)
        return NULL;
    memcpy(sm->addr, addr, ETH_ALEN);

    sm->wpa_auth = wpa_auth;
    sm->group = wpa_auth->group;
    wpa_auth_add_sm(sm);

    return sm;
}

int wpa_auth_sta_associated(struct wpa_authenticator *wpa_auth,
                struct wpa_state_machine *sm)
{
    if (wpa_auth == NULL || !wpa_auth->conf.wpa || sm == NULL)
        return -1;

#ifdef CONFIG_IEEE80211R_AP
    if (sm->ft_completed) {
        wpa_auth_logger(wpa_auth, sm->addr, LOGGER_DEBUG,
                "FT authentication already completed - do not "
                "start 4-way handshake");
        return 0;
    }
#endif /* CONFIG_IEEE80211R_AP */

    if (sm->started) {
        memset(&sm->key_replay, 0, sizeof(sm->key_replay));
        sm->ReAuthenticationRequest = TRUE;
        return wpa_sm_step(sm);
    }

    sm->started = 1;

    sm->Init = TRUE;
    if (wpa_sm_step(sm) == 1)
        return 1; /* should not really happen */
    sm->Init = FALSE;
    sm->AuthenticationRequest = TRUE;
    return wpa_sm_step(sm);
}


void wpa_auth_sta_no_wpa(struct wpa_state_machine *sm)
{
    /* WPA/RSN was not used - clear WPA state. This is needed if the STA
     * reassociates back to the same AP while the previous entry for the
     * STA has not yet been removed. */
    if (sm == NULL)
        return;

    sm->wpa_key_mgmt = 0;
}


static void wpa_free_sta_sm(struct wpa_state_machine *sm)
{
    wpa_auth_del_sm(sm);
    if (sm->GUpdateStationKeys) {
        sm->group->GKeyDoneStations--;
        sm->GUpdateStationKeys = FALSE;
    }
#ifdef CONFIG_IEEE80211R_AP
    os_free(sm->assoc_resp_ftie);
    wpabuf_free(sm->ft_pending_req_ies);
#endif /* CONFIG_IEEE80211R_AP */
    wpa_printf( MSG_DEBUG, "wpa_free_sta_sm: free eapol=%p\n", sm->last_rx_eapol_key);
    os_free(sm->last_rx_eapol_key);
    os_free(sm->wpa_ie);
    os_free(sm->rsnxe);
    os_free(sm);
}


void wpa_auth_sta_deinit(struct wpa_state_machine *sm)
{
#ifdef ESP_SUPPLICANT
    if (sm && esp_wifi_ap_is_sta_sae_reauth_node(sm->addr)) {
        wpa_printf( MSG_DEBUG, "deinit old sm=%p\n", sm);
    }
#else /* ESP_SUPPLICANT */
    wpa_printf( MSG_DEBUG, "deinit sm=%p\n", sm);
#endif /* ESP_SUPPLICANT */
    if (sm == NULL)
        return;

    eloop_cancel_timeout(resend_eapol_handle, (void*)(sm->index), NULL);

    if (sm->in_step_loop) {
        /* Must not free state machine while wpa_sm_step() is running.
         * Freeing will be completed in the end of wpa_sm_step(). */
        wpa_printf( MSG_DEBUG, "WPA: Registering pending STA state "
               "machine deinit for " MACSTR, MAC2STR(sm->addr));
        sm->pending_deinit = 1;
    } else
        wpa_free_sta_sm(sm);
}


static void wpa_request_new_ptk(struct wpa_state_machine *sm)
{
    if (sm == NULL)
        return;

    sm->PTKRequest = TRUE;
    sm->PTK_valid = 0;
}

static int wpa_replay_counter_valid(struct wpa_key_replay_counter *ctr,
                    const u8 *replay_counter)
{
    int i;
    for (i = 0; i < RSNA_MAX_EAPOL_RETRIES; i++) {
        if (!ctr[i].valid)
            break;
        if (memcmp(replay_counter, ctr[i].counter,
                  WPA_REPLAY_COUNTER_LEN) == 0)
            return 1;
    }
    return 0;
}

static void wpa_replay_counter_mark_invalid(struct wpa_key_replay_counter *ctr,
                        const u8 *replay_counter)
{
    int i;
    for (i = 0; i < RSNA_MAX_EAPOL_RETRIES; i++) {
        if (ctr[i].valid &&
            (replay_counter == NULL ||
             memcmp(replay_counter, ctr[i].counter,
                   WPA_REPLAY_COUNTER_LEN) == 0))
            ctr[i].valid = FALSE;
    }
}

#ifdef CONFIG_IEEE80211R_AP
static int ICACHE_FLASH_ATTR ft_check_msg_2_of_4(struct wpa_authenticator *wpa_auth,
                   struct wpa_state_machine *sm,
                   struct wpa_eapol_ie_parse *kde)
{
    struct wpa_ie_data ie;
    struct rsn_mdie *mdie;

    if (wpa_parse_wpa_ie_rsn(kde->rsn_ie, kde->rsn_ie_len, &ie) < 0 ||
        ie.num_pmkid != 1 || ie.pmkid == NULL) {
        wpa_printf( MSG_DEBUG, "FT: No PMKR1Name in "
               "FT 4-way handshake message 2/4");
        return -1;
    }

    memcpy(sm->sup_pmk_r1_name, ie.pmkid, PMKID_LEN);
    wpa_hexdump(MSG_DEBUG, "FT: PMKR1Name from Supplicant",
            sm->sup_pmk_r1_name, PMKID_LEN);

    if (!kde->mdie || !kde->ftie) {
        wpa_printf( MSG_DEBUG, "FT: No %s in FT 4-way handshake "
               "message 2/4", kde->mdie ? "FTIE" : "MDIE");
        return -1;
    }

    mdie = (struct rsn_mdie *) (kde->mdie + 2);
    if (kde->mdie[1] < sizeof(struct rsn_mdie) ||
    	memcmp(wpa_auth->conf.mobility_domain, mdie->mobility_domain,
              MOBILITY_DOMAIN_ID_LEN) != 0) {
        wpa_printf( MSG_DEBUG, "FT: MDIE mismatch");
        return -1;
    }

    if (sm->assoc_resp_ftie &&
        (kde->ftie[1] != sm->assoc_resp_ftie[1] ||
        memcmp(kde->ftie, sm->assoc_resp_ftie,
               2 + sm->assoc_resp_ftie[1]) != 0)) {
        wpa_printf( MSG_DEBUG, "FT: FTIE mismatch");
        wpa_hexdump(MSG_DEBUG, "FT: FTIE in EAPOL-Key msg 2/4",
                kde->ftie, kde->ftie_len);
        wpa_hexdump(MSG_DEBUG, "FT: FTIE in (Re)AssocResp",
                sm->assoc_resp_ftie, 2 + sm->assoc_resp_ftie[1]);
        return -1;
    }

    return 0;
}
#endif /* CONFIG_IEEE80211R_AP */

static int wpa_receive_error_report(struct wpa_authenticator *wpa_auth,
                    struct wpa_state_machine *sm, int group)
{
    if (group && wpa_auth->conf.wpa_group != WPA_CIPHER_TKIP) {
    } else if (!group && sm->pairwise != WPA_CIPHER_TKIP) {
    } else {
        if (wpa_auth_mic_failure_report(wpa_auth, sm->addr) > 0)
            return 1; /* STA entry was removed */
    }

    /*
     * Error report is not a request for a new key handshake, but since
     * Authenticator may do it, let's change the keys now anyway.
     */
    wpa_request_new_ptk(sm);
    return 0;
}

void wpa_receive(struct wpa_authenticator *wpa_auth, struct wpa_state_machine *sm, u8 *data, size_t data_len)
{
    struct ieee802_1x_hdr *hdr;
    struct wpa_eapol_key *key;
    u16 key_info, key_data_length;
    enum { PAIRWISE_2, PAIRWISE_4, GROUP_2, REQUEST } msg;
    struct wpa_eapol_ie_parse kde;
    int ft;
    const u8 *eapol_key_ie;
    size_t eapol_key_ie_len;

    if (wpa_auth == NULL || !wpa_auth->conf.wpa || sm == NULL)
        return;

    if (data_len < sizeof(*hdr) + sizeof(*key))
        return;

    hdr = (struct ieee802_1x_hdr *) data;
    key = (struct wpa_eapol_key *) (hdr + 1);
    key_info = WPA_GET_BE16(key->key_info);
    key_data_length = WPA_GET_BE16(key->key_data_length);
    wpa_printf( MSG_DEBUG, "WPA: Received EAPOL-Key from " MACSTR
           " key_info=0x%x type=%u key_data_length=%u\n",
           MAC2STR(sm->addr), key_info, key->type, key_data_length);
    if (key_data_length > data_len - sizeof(*hdr) - sizeof(*key)) {
        wpa_printf( MSG_INFO, "WPA: Invalid EAPOL-Key frame - "
               "key_data overflow (%d > %lu)\n",
               key_data_length,
               (unsigned long) (data_len - sizeof(*hdr) -
                        sizeof(*key)));
        return;
    }

    if (sm->wpa == WPA_VERSION_WPA2) {
        if (key->type == EAPOL_KEY_TYPE_WPA) {
            /*
             * Some deployed station implementations seem to send
             * msg 4/4 with incorrect type value in WPA2 mode.
             */
            wpa_printf( MSG_DEBUG, "Workaround: Allow EAPOL-Key "
                   "with unexpected WPA type in RSN mode");
        } else if (key->type != EAPOL_KEY_TYPE_RSN) {
            wpa_printf( MSG_DEBUG, "Ignore EAPOL-Key with "
                   "unexpected type %d in RSN mode",
                   key->type);
            return;
        }
    } else {
        if (key->type != EAPOL_KEY_TYPE_WPA) {
            wpa_printf( MSG_DEBUG, "Ignore EAPOL-Key with "
                   "unexpected type %d in WPA mode",
                   key->type);
            return;
        }
    }

    wpa_hexdump(MSG_DEBUG, "WPA: Received Key Nonce", key->key_nonce,
            WPA_NONCE_LEN);
    wpa_hexdump(MSG_DEBUG, "WPA: Received Replay Counter",
            key->replay_counter, WPA_REPLAY_COUNTER_LEN);

    /* FIX: verify that the EAPOL-Key frame was encrypted if pairwise keys
     * are set */

    if (key_info & WPA_KEY_INFO_SMK_MESSAGE) {
        wpa_printf(MSG_DEBUG, "WPA: Ignore SMK message");
        return;
    }

    if (key_info & WPA_KEY_INFO_REQUEST) {
        msg = REQUEST;
    } else if (!(key_info & WPA_KEY_INFO_KEY_TYPE)) {
        msg = GROUP_2;
    } else if (key_data_length == 0) {
        msg = PAIRWISE_4;
    } else {
        msg = PAIRWISE_2;
    }

    if (msg == REQUEST || msg == PAIRWISE_2 || msg == PAIRWISE_4 ||
        msg == GROUP_2) {
        u16 ver = key_info & WPA_KEY_INFO_TYPE_MASK;
        if (sm->pairwise == WPA_CIPHER_CCMP ||
            sm->pairwise == WPA_CIPHER_GCMP) {
            if (wpa_use_aes_cmac(sm) &&
                !wpa_key_mgmt_suite_b(sm->wpa_key_mgmt) &&
                !wpa_use_akm_defined(sm->wpa_key_mgmt) &&
                ver != WPA_KEY_INFO_TYPE_AES_128_CMAC) {
                return;
            }

            if (!wpa_use_aes_cmac(sm) &&
                !wpa_use_akm_defined(sm->wpa_key_mgmt) &&
                ver != WPA_KEY_INFO_TYPE_HMAC_SHA1_AES) {
                return;
            }
        }
        if (wpa_use_akm_defined(sm->wpa_key_mgmt) &&
           ver != WPA_KEY_INFO_TYPE_AKM_DEFINED){
            return;
        }
    }

    if (key_info & WPA_KEY_INFO_REQUEST) {
        if (sm->req_replay_counter_used &&
            os_memcmp(key->replay_counter, sm->req_replay_counter,
                  WPA_REPLAY_COUNTER_LEN) <= 0) {
            return;
        }
    }

    if (!(key_info & WPA_KEY_INFO_REQUEST) &&
        !wpa_replay_counter_valid(sm->key_replay, key->replay_counter)) {
        int i;

        if (msg == PAIRWISE_2 &&
            wpa_replay_counter_valid(sm->prev_key_replay,
                         key->replay_counter) &&
            sm->wpa_ptk_state == WPA_PTK_PTKINITNEGOTIATING &&
            memcmp(sm->SNonce, key->key_nonce, WPA_NONCE_LEN) != 0)
        {
            /*
             * Some supplicant implementations (e.g., Windows XP
             * WZC) update SNonce for each EAPOL-Key 2/4. This
             * breaks the workaround on accepting any of the
             * pending requests, so allow the SNonce to be updated
             * even if we have already sent out EAPOL-Key 3/4.
             */
            sm->update_snonce = 1;
            wpa_replay_counter_mark_invalid(sm->prev_key_replay,
                            key->replay_counter);
            goto continue_processing;
        }

        if (msg == PAIRWISE_2 &&
            wpa_replay_counter_valid(sm->prev_key_replay,
                         key->replay_counter) &&
            sm->wpa_ptk_state == WPA_PTK_PTKINITNEGOTIATING) {
        } else {
        }
        for (i = 0; i < RSNA_MAX_EAPOL_RETRIES; i++) {
            if (!sm->key_replay[i].valid)
                break;
            wpa_hexdump(MSG_DEBUG, "pending replay counter",
                    sm->key_replay[i].counter,
                    WPA_REPLAY_COUNTER_LEN);
        }
        wpa_hexdump(MSG_DEBUG, "received replay counter",
                key->replay_counter, WPA_REPLAY_COUNTER_LEN);
        return;
    }

continue_processing:
    switch (msg) {
    case PAIRWISE_2:
        if (sm->wpa_ptk_state != WPA_PTK_PTKSTART &&
            sm->wpa_ptk_state != WPA_PTK_PTKCALCNEGOTIATING &&
            (!sm->update_snonce ||
             sm->wpa_ptk_state != WPA_PTK_PTKINITNEGOTIATING)) {
            return;
        }
        random_add_randomness(key->key_nonce, WPA_NONCE_LEN);
        if (sm->group->reject_4way_hs_for_entropy) {
            /*
             * The system did not have enough entropy to generate
             * strong random numbers. Reject the first 4-way
             * handshake(s) and collect some entropy based on the
             * information from it. Once enough entropy is
             * available, the next attempt will trigger GMK/Key
             * Counter update and the station will be allowed to
             * continue.
             */
            wpa_printf( MSG_DEBUG, "WPA: Reject 4-way handshake to "
                   "collect more entropy for random number "
                   "generation");
            random_mark_pool_ready();
            wpa_sta_disconnect(wpa_auth, sm->addr,
                    WLAN_REASON_PREV_AUTH_NOT_VALID);
            return;
        }
        if (wpa_parse_kde_ies((u8 *) (key + 1), key_data_length,
                      &kde) < 0) {
            return;
        }
        if (kde.rsn_ie) {
            eapol_key_ie = kde.rsn_ie;
            eapol_key_ie_len = kde.rsn_ie_len;
        } else {
            eapol_key_ie = kde.wpa_ie;
            eapol_key_ie_len = kde.wpa_ie_len;
        }
        ft = sm->wpa == WPA_VERSION_WPA2 &&
            wpa_key_mgmt_ft(sm->wpa_key_mgmt);
        if (sm->wpa_ie == NULL ||
            wpa_compare_rsn_ie(ft,
                       sm->wpa_ie, sm->wpa_ie_len,
                       eapol_key_ie, eapol_key_ie_len)) {
            if (sm->wpa_ie) {
                wpa_hexdump(MSG_DEBUG, "WPA IE in AssocReq",
                        sm->wpa_ie, sm->wpa_ie_len);
            }
            wpa_hexdump(MSG_DEBUG, "WPA IE in msg 2/4",
                    eapol_key_ie, eapol_key_ie_len);
            /* MLME-DEAUTHENTICATE.request */
            wpa_sta_disconnect(wpa_auth, sm->addr,
                    WLAN_REASON_PREV_AUTH_NOT_VALID);
            return;
        }
#ifdef CONFIG_IEEE80211R_AP
        if (ft && ft_check_msg_2_of_4(wpa_auth, sm, &kde) < 0) {
            wpa_sta_disconnect(wpa_auth, sm->addr,
                    WLAN_REASON_PREV_AUTH_NOT_VALID);
            return;
        }
#endif /* CONFIG_IEEE80211R_AP */
        break;
    case PAIRWISE_4:
        if (sm->wpa_ptk_state != WPA_PTK_PTKINITNEGOTIATING ||
            !sm->PTK_valid) {
            return;
        }
        break;
    case GROUP_2:
        if (sm->wpa_ptk_group_state != WPA_PTK_GROUP_REKEYNEGOTIATING
            || !sm->PTK_valid) {
            return;
        }
        break;
    case REQUEST:
        break;
    }


    if (key_info & WPA_KEY_INFO_ACK) {
        return;
    }

    if (!(key_info & WPA_KEY_INFO_MIC)) {
        return;
    }

    sm->MICVerified = FALSE;
    if (sm->PTK_valid && !sm->update_snonce) {
        if (wpa_verify_key_mic(sm->wpa_key_mgmt, &sm->PTK, data,
                       data_len)) {
            wpa_printf(MSG_INFO,
                     "received EAPOL-Key with invalid MIC");
            return;
        }
        sm->MICVerified = TRUE;
        eloop_cancel_timeout(resend_eapol_handle, (void*)(sm->index), NULL);
        sm->pending_1_of_4_timeout = 0;
    }

    if (key_info & WPA_KEY_INFO_REQUEST) {
        if (sm->MICVerified) {
            sm->req_replay_counter_used = 1;
            memcpy(sm->req_replay_counter, key->replay_counter,
                  WPA_REPLAY_COUNTER_LEN);
        } else {
            wpa_printf(MSG_INFO,
                     "received EAPOL-Key request with invalid MIC");
            return;
        }

        /*
         * TODO: should decrypt key data field if encryption was used;
         * even though MAC address KDE is not normally encrypted,
         * supplicant is allowed to encrypt it.
         */
        if (key_info & WPA_KEY_INFO_ERROR) {
            if (wpa_receive_error_report(
                    wpa_auth, sm,
                    !(key_info & WPA_KEY_INFO_KEY_TYPE)) > 0)
                return; /* STA entry was removed */
        } else if (key_info & WPA_KEY_INFO_KEY_TYPE) {
            wpa_request_new_ptk(sm);
        } else if (key_data_length > 0 &&
               wpa_parse_kde_ies((const u8 *) (key + 1),
                         key_data_length, &kde) == 0 &&
               kde.mac_addr) {
        } else {
            eloop_cancel_timeout(wpa_rekey_gtk, wpa_auth, NULL);
            wpa_rekey_gtk(wpa_auth, NULL);
        }
    } else {
        /* Do not allow the same key replay counter to be reused. */
        wpa_replay_counter_mark_invalid(sm->key_replay,
                        key->replay_counter);

        if (msg == PAIRWISE_2) {
            /*
             * Maintain a copy of the pending EAPOL-Key frames in
             * case the EAPOL-Key frame was retransmitted. This is
             * needed to allow EAPOL-Key msg 2/4 reply to another
             * pending msg 1/4 to update the SNonce to work around
             * unexpected supplicant behavior.
             */
            memcpy(sm->prev_key_replay, sm->key_replay,
                  sizeof(sm->key_replay));
        } else {
            memset(sm->prev_key_replay, 0,
                  sizeof(sm->prev_key_replay));
        }

        /*
         * Make sure old valid counters are not accepted anymore and
         * do not get copied again.
         */
        wpa_replay_counter_mark_invalid(sm->key_replay, NULL);
    }

    wpa_printf( MSG_DEBUG, "wpa_rx: free eapol=%p", sm->last_rx_eapol_key);
    os_free(sm->last_rx_eapol_key);
    sm->last_rx_eapol_key = (u8 *)os_malloc(data_len);
    if (sm->last_rx_eapol_key == NULL)
        return;
    wpa_printf( MSG_DEBUG, "wpa_rx: new eapol=%p", sm->last_rx_eapol_key);
    memcpy(sm->last_rx_eapol_key, data, data_len);
    sm->last_rx_eapol_key_len = data_len;

    sm->rx_eapol_key_secure = !!(key_info & WPA_KEY_INFO_SECURE);
    sm->EAPOLKeyReceived = TRUE;
    sm->EAPOLKeyPairwise = !!(key_info & WPA_KEY_INFO_KEY_TYPE);
    sm->EAPOLKeyRequest = !!(key_info & WPA_KEY_INFO_REQUEST);
    memcpy(sm->SNonce, key->key_nonce, WPA_NONCE_LEN);
    wpa_sm_step(sm);
}

int wpa_auth_pmksa_add_sae(struct wpa_authenticator *wpa_auth, const u8 *addr,
               const u8 *pmk, const u8 *pmkid, bool cache_pmksa)
{
    if (cache_pmksa)
        return -1;

    wpa_hexdump_key(MSG_DEBUG, "RSN: Cache PMK from SAE", pmk, PMK_LEN);
    if (pmksa_cache_auth_add(wpa_auth->pmksa, pmk, PMK_LEN, pmkid,
                  NULL, 0,
                  wpa_auth->addr, addr, 0, NULL,
                  WPA_KEY_MGMT_SAE))
        return 0;

    return -1;
}

void wpa_auth_add_sae_pmkid(struct wpa_state_machine *sm, const u8 *pmkid)
{
    os_memcpy(sm->pmkid, pmkid, PMKID_LEN);
    sm->pmkid_set = 1;
}

static int wpa_gmk_to_gtk(const u8 *gmk, const char *label, const u8 *addr,
              const u8 *gnonce, u8 *gtk, size_t gtk_len)
{
    u8 data[ETH_ALEN + WPA_NONCE_LEN + 8 + 16];
    u8 *pos;
    int ret = 0;

    /* GTK = PRF-X(GMK, "Group key expansion",
     *    AA || GNonce || Time || random data)
     * The example described in the IEEE 802.11 standard uses only AA and
     * GNonce as inputs here. Add some more entropy since this derivation
     * is done only at the Authenticator and as such, does not need to be
     * exactly same.
     */
    memcpy(data, addr, ETH_ALEN);
    memcpy(data + ETH_ALEN, gnonce, WPA_NONCE_LEN);
    pos = data + ETH_ALEN + WPA_NONCE_LEN;
    wpa_get_ntp_timestamp(pos);
    pos += 8;
    if (os_get_random(pos, 16) < 0)
        ret = -1;

#ifdef CONFIG_IEEE80211W
    sha256_prf(gmk, WPA_GMK_LEN, label, data, sizeof(data), gtk, gtk_len);
#else /* CONFIG_IEEE80211W */
    if (sha1_prf(gmk, WPA_GMK_LEN, label, data, sizeof(data), gtk, gtk_len) < 0)
        ret = -1;
#endif /* CONFIG_IEEE80211W */

    return ret;
}


void __wpa_send_eapol(struct wpa_authenticator *wpa_auth,
              struct wpa_state_machine *sm, int key_info,
              const u8 *key_rsc, const u8 *nonce,
              const u8 *kde, size_t kde_len,
              int keyidx, int encr, int force_version)
{
    struct ieee802_1x_hdr *hdr;
    struct wpa_eapol_key *key;
    size_t len;
    int alg;
    int key_data_len, pad_len = 0;
    u8 *buf, *pos;
    int version, pairwise;
    int i;

    wpa_printf( MSG_DEBUG, "wpa_auth=%p sm=%p  kdersc=%p kde=%p nounce=%p kde_len=%u keyidx=%d encr=%d force=%d",
                   wpa_auth,sm, key_rsc, kde, nonce, kde_len, keyidx, encr, force_version);
    len = sizeof(struct ieee802_1x_hdr) + sizeof(struct wpa_eapol_key);

    if (force_version)
        version = force_version;
    else if (wpa_use_akm_defined(sm->wpa_key_mgmt))
        version = WPA_KEY_INFO_TYPE_AKM_DEFINED;
    else if (wpa_use_aes_cmac(sm))
        version = WPA_KEY_INFO_TYPE_AES_128_CMAC;
    else if (sm->pairwise != WPA_CIPHER_TKIP)
        version = WPA_KEY_INFO_TYPE_HMAC_SHA1_AES;
    else
        version = WPA_KEY_INFO_TYPE_HMAC_MD5_RC4;

    pairwise = !!(key_info & WPA_KEY_INFO_KEY_TYPE);

    wpa_printf( MSG_DEBUG, "WPA: Send EAPOL(version=%d secure=%d mic=%d "
           "ack=%d install=%d pairwise=%d kde_len=%lu keyidx=%d "
           "encr=%d)",
           version,
           (key_info & WPA_KEY_INFO_SECURE) ? 1 : 0,
           (key_info & WPA_KEY_INFO_MIC) ? 1 : 0,
           (key_info & WPA_KEY_INFO_ACK) ? 1 : 0,
           (key_info & WPA_KEY_INFO_INSTALL) ? 1 : 0,
           pairwise, (unsigned long) kde_len, keyidx, encr);

    key_data_len = kde_len;

    if ((version == WPA_KEY_INFO_TYPE_HMAC_SHA1_AES ||
         wpa_use_aes_key_wrap(sm->wpa_key_mgmt) ||
         version == WPA_KEY_INFO_TYPE_AES_128_CMAC) && encr) {
        pad_len = key_data_len % 8;
        if (pad_len)
            pad_len = 8 - pad_len;
        key_data_len += pad_len + 8;
    }

    len += key_data_len;

    hdr = (struct ieee802_1x_hdr *)os_zalloc(len);
    if (hdr == NULL)
        return;
    hdr->version = wpa_auth->conf.eapol_version;
    hdr->type = IEEE802_1X_TYPE_EAPOL_KEY;
    hdr->length = host_to_be16(len  - sizeof(*hdr));
    key = (struct wpa_eapol_key *) (hdr + 1);

    key->type = sm->wpa == WPA_VERSION_WPA2 ?
        EAPOL_KEY_TYPE_RSN : EAPOL_KEY_TYPE_WPA;
    key_info |= version;
    if (encr && sm->wpa == WPA_VERSION_WPA2)
        key_info |= WPA_KEY_INFO_ENCR_KEY_DATA;
    if (sm->wpa != WPA_VERSION_WPA2)
        key_info |= keyidx << WPA_KEY_INFO_KEY_INDEX_SHIFT;
    WPA_PUT_BE16(key->key_info, key_info);

    alg = pairwise ? sm->pairwise : wpa_auth->conf.wpa_group;
    if (sm->wpa == WPA_VERSION_WPA2 && !pairwise)
        WPA_PUT_BE16(key->key_length, 0);
    else
        WPA_PUT_BE16(key->key_length, wpa_cipher_key_len(alg));

    for (i = RSNA_MAX_EAPOL_RETRIES - 1; i > 0; i--) {
        sm->key_replay[i].valid = sm->key_replay[i - 1].valid;
        memcpy(sm->key_replay[i].counter,
              sm->key_replay[i - 1].counter,
              WPA_REPLAY_COUNTER_LEN);
    }
    inc_byte_array(sm->key_replay[0].counter, WPA_REPLAY_COUNTER_LEN);
    memcpy(key->replay_counter, sm->key_replay[0].counter,
          WPA_REPLAY_COUNTER_LEN);
    sm->key_replay[0].valid = TRUE;

    if (nonce)
        memcpy(key->key_nonce, nonce, WPA_NONCE_LEN);

    if (key_rsc)
        memcpy(key->key_rsc, key_rsc, WPA_KEY_RSC_LEN);

    if (kde && !encr) {
        memcpy(key + 1, kde, kde_len);
        WPA_PUT_BE16(key->key_data_length, kde_len);
    } else if (encr && kde) {
        buf = (u8 *)os_zalloc(key_data_len);
        if (buf == NULL) {
            os_free(hdr);
            return;
        }
        pos = buf;
        memcpy(pos, kde, kde_len);
        pos += kde_len;

        if (pad_len)
            *pos++ = 0xdd;

        wpa_hexdump_key(MSG_DEBUG, "Plaintext EAPOL-Key Key Data",
                buf, key_data_len);
        if (version == WPA_KEY_INFO_TYPE_HMAC_SHA1_AES ||
            wpa_use_aes_key_wrap(sm->wpa_key_mgmt) ||
            version == WPA_KEY_INFO_TYPE_AES_128_CMAC) {
            if (aes_wrap(sm->PTK.kek, sm->PTK.kek_len, (key_data_len - 8) / 8, buf,
                    (u8 *) (key + 1))) {
                os_free(hdr);
                os_free(buf);
                return;
            }
            WPA_PUT_BE16(key->key_data_length, key_data_len);
        } else if (sm->PTK.kek_len == 16) {
            u8 ek[32];
            memcpy(key->key_iv,
                  sm->group->Counter + WPA_NONCE_LEN - 16, 16);
            inc_byte_array(sm->group->Counter, WPA_NONCE_LEN);
            memcpy(ek, key->key_iv, 16);
            memcpy(ek + 16, sm->PTK.kek, sm->PTK.kek_len);
            memcpy(key + 1, buf, key_data_len);
            rc4_skip(ek, 32, 256, (u8 *) (key + 1), key_data_len);
            WPA_PUT_BE16(key->key_data_length, key_data_len);
        } else {
            os_free(buf);
            os_free(hdr);
            return;
        }
        os_free(buf);
    }

    if (key_info & WPA_KEY_INFO_MIC) {
        if (!sm->PTK_valid) {
            os_free(hdr);
            return;
        }
        wpa_eapol_key_mic(sm->PTK.kck, sm->PTK.kck_len,
              sm->wpa_key_mgmt, version,
              (u8 *) hdr, len, key->key_mic);
    }

    wpa_auth_set_eapol(sm->wpa_auth, sm->addr, WPA_EAPOL_inc_EapolFramesTx, 1);
    wpa_auth_send_eapol(wpa_auth, sm->addr, (u8 *) hdr, len, sm->pairwise_set);
    os_free(hdr);
}

int hostap_eapol_resend_process(void *timeout_ctx)
{
    u32 index = (u32)timeout_ctx;
    struct wpa_state_machine *sm = wpa_auth_get_sm(index);

    wpa_printf( MSG_DEBUG, "resend eapol1");

    if(sm) {
        sm->pending_1_of_4_timeout = 0;
        sm->TimeoutEvt = TRUE;
        sm->in_step_loop = 0;
        wpa_sm_step(sm);
    } else {
        wpa_printf( MSG_INFO, "Station left, stop send EAPOL frame");
    }

    return ESP_OK;
}

void resend_eapol_handle(void *data, void *user_ctx)
{
    wifi_ipc_config_t cfg;

    cfg.fn = hostap_eapol_resend_process;
    cfg.arg = data;
    cfg.arg_size = 0;
    esp_wifi_ipc_internal(&cfg, false);
}

static void wpa_send_eapol(struct wpa_authenticator *wpa_auth,
               struct wpa_state_machine *sm, int key_info,
               const u8 *key_rsc, const u8 *nonce,
               const u8 *kde, size_t kde_len,
               int keyidx, int encr)
{
    int pairwise = key_info & WPA_KEY_INFO_KEY_TYPE;
    int ctr;

    if (sm == NULL)
        return;

    __wpa_send_eapol(wpa_auth, sm, key_info, key_rsc, nonce, kde, kde_len,
             keyidx, encr, 0);

    ctr = pairwise ? sm->TimeoutCtr : sm->GTimeoutCtr;
    if (pairwise && ctr == 1 && !(key_info & WPA_KEY_INFO_MIC))
        sm->pending_1_of_4_timeout = 1;

    eloop_cancel_timeout(resend_eapol_handle, (void*)(sm->index), NULL);
    eloop_register_timeout(1, 0, resend_eapol_handle, (void*)(sm->index), NULL);
}

static int wpa_verify_key_mic(int akmp, struct wpa_ptk *PTK, u8 *data,
			      size_t data_len)
{
    struct ieee802_1x_hdr *hdr;
    struct wpa_eapol_key *key;
    u16 key_info;
    int ret = 0;
    u8 mic[WPA_EAPOL_KEY_MIC_MAX_LEN];
    size_t mic_len = 16;

    if (data_len < sizeof(*hdr) + sizeof(*key)){
        wpa_printf( MSG_DEBUG, "invalid data length, len=%u", data_len);
        return -1;
    }

    hdr = (struct ieee802_1x_hdr *) data;
    key = (struct wpa_eapol_key *) (hdr + 1);
    key_info = WPA_GET_BE16(key->key_info);
    os_memcpy(mic, key->key_mic, mic_len);
    os_memset(key->key_mic, 0, mic_len);
    if (wpa_eapol_key_mic(PTK->kck, PTK->kck_len, akmp,
                          key_info & WPA_KEY_INFO_TYPE_MASK,
                          data, data_len, key->key_mic) ||
        os_memcmp_const(mic, key->key_mic, mic_len) != 0)
        ret = -1;
    os_memcpy(key->key_mic, mic, mic_len);
    return ret;
 }


void wpa_remove_ptk(struct wpa_state_machine *sm)
{
    sm->PTK_valid = FALSE;
    memset(&sm->PTK, 0, sizeof(sm->PTK));
    wpa_auth_set_key(sm->wpa_auth, 0, WIFI_WPA_ALG_NONE, sm->addr, 0, NULL, 0);
    sm->pairwise_set = FALSE;
    eloop_cancel_timeout(wpa_rekey_ptk, sm->wpa_auth, sm);
}


int wpa_auth_sm_event(struct wpa_state_machine *sm, wpa_event event)
{
    int remove_ptk = 1;

    if (sm == NULL)
        return -1;

    switch (event) {
    case WPA_AUTH:
    case WPA_ASSOC:
        break;
    case WPA_DEAUTH:
    case WPA_DISASSOC:
        sm->DeauthenticationRequest = TRUE;
        break;
    case WPA_REAUTH:
    case WPA_REAUTH_EAPOL:
        if (!sm->started) {
            /*
             * When using WPS, we may end up here if the STA
             * manages to re-associate without the previous STA
             * entry getting removed. Consequently, we need to make
             * sure that the WPA state machines gets initialized
             * properly at this point.
             */
            wpa_printf( MSG_DEBUG, "WPA state machine had not been "
                   "started - initialize now");
            sm->started = 1;
            sm->Init = TRUE;
            if (wpa_sm_step(sm) == 1)
                return 1; /* should not really happen */
            sm->Init = FALSE;
            sm->AuthenticationRequest = TRUE;
            break;
        }
        if (sm->GUpdateStationKeys) {
            /*
             * Reauthentication cancels the pending group key
             * update for this STA.
             */
            sm->group->GKeyDoneStations--;
            sm->GUpdateStationKeys = FALSE;
            sm->PtkGroupInit = TRUE;
        }
        sm->ReAuthenticationRequest = TRUE;
        break;
    case WPA_ASSOC_FT:
#ifdef CONFIG_IEEE80211R_AP
        wpa_printf( MSG_DEBUG, "FT: Retry PTK configuration "
               "after association");
        wpa_ft_install_ptk(sm);

        /* Using FT protocol, not WPA auth state machine */
        sm->ft_completed = 1;
        return 0;
#else /* CONFIG_IEEE80211R_AP */
        break;
#endif /* CONFIG_IEEE80211R_AP */
    }

#ifdef CONFIG_IEEE80211R_AP
    sm->ft_completed = 0;
#endif /* CONFIG_IEEE80211R_AP */

#ifdef CONFIG_IEEE80211W
    if (sm->mgmt_frame_prot && event == WPA_AUTH)
        remove_ptk = 0;
#endif /* CONFIG_IEEE80211W */

    if (remove_ptk) {
        sm->PTK_valid = FALSE;
        memset(&sm->PTK, 0, sizeof(sm->PTK));

        if (event != WPA_REAUTH_EAPOL)
            wpa_remove_ptk(sm);
    }

    return wpa_sm_step(sm);
}


SM_STATE(WPA_PTK, INITIALIZE)
{
    SM_ENTRY_MA(WPA_PTK, INITIALIZE, wpa_ptk);
    if (sm->Init) {
        /* Init flag is not cleared here, so avoid busy
         * loop by claiming nothing changed. */
        sm->changed = FALSE;
    }

    sm->keycount = 0;
    if (sm->GUpdateStationKeys)
        sm->group->GKeyDoneStations--;
    sm->GUpdateStationKeys = FALSE;
    if (sm->wpa == WPA_VERSION_WPA)
        sm->PInitAKeys = FALSE;
    if (1 /* Unicast cipher supported AND (ESS OR ((IBSS or WDS) and
           * Local AA > Remote AA)) */) {
        sm->Pair = TRUE;
    }
    wpa_auth_set_eapol(sm->wpa_auth, sm->addr, WPA_EAPOL_portEnabled, 0);
    wpa_remove_ptk(sm);
    wpa_auth_set_eapol(sm->wpa_auth, sm->addr, WPA_EAPOL_portValid, 0);
    sm->TimeoutCtr = 0;
    if (wpa_key_mgmt_wpa_psk(sm->wpa_key_mgmt)) {
        wpa_auth_set_eapol(sm->wpa_auth, sm->addr,
                   WPA_EAPOL_authorized, 0);
    }
}


SM_STATE(WPA_PTK, DISCONNECT)
{
    u16 reason = sm->disconnect_reason;

    SM_ENTRY_MA(WPA_PTK, DISCONNECT, wpa_ptk);
    sm->Disconnect = FALSE;
    sm->disconnect_reason = 0;
    if (!reason)
        reason = WLAN_REASON_PREV_AUTH_NOT_VALID;
    wpa_sta_disconnect(sm->wpa_auth, sm->addr, reason);
}


SM_STATE(WPA_PTK, DISCONNECTED)
{
    SM_ENTRY_MA(WPA_PTK, DISCONNECTED, wpa_ptk);
    sm->DeauthenticationRequest = FALSE;
}


SM_STATE(WPA_PTK, AUTHENTICATION)
{
    SM_ENTRY_MA(WPA_PTK, AUTHENTICATION, wpa_ptk);
    memset(&sm->PTK, 0, sizeof(sm->PTK));
    sm->PTK_valid = FALSE;
    wpa_auth_set_eapol(sm->wpa_auth, sm->addr, WPA_EAPOL_portControl_Auto,
               1);
    wpa_auth_set_eapol(sm->wpa_auth, sm->addr, WPA_EAPOL_portEnabled, 1);
    sm->AuthenticationRequest = FALSE;
}


static void wpa_group_ensure_init(struct wpa_authenticator *wpa_auth,
                  struct wpa_group *group)
{
    if (group->first_sta_seen)
        return;
    /*
     * System has run bit further than at the time hostapd was started
     * potentially very early during boot up. This provides better chances
     * of collecting more randomness on embedded systems. Re-initialize the
     * GMK and Counter here to improve their strength if there was not
     * enough entropy available immediately after system startup.
     */
    wpa_printf( MSG_DEBUG, "WPA: Re-initialize GMK/Counter on first "
           "station");
    if (random_pool_ready() != 1) {
        wpa_printf( MSG_INFO, "WPA: Not enough entropy in random pool "
               "to proceed - reject first 4-way handshake");
        group->reject_4way_hs_for_entropy = TRUE;
    } else {
        group->first_sta_seen = TRUE;
        group->reject_4way_hs_for_entropy = FALSE;
    }

    wpa_group_init_gmk_and_counter(wpa_auth, group);
    wpa_gtk_update(wpa_auth, group);
    wpa_group_config_group_keys(wpa_auth, group);
}


SM_STATE(WPA_PTK, AUTHENTICATION2)
{
    SM_ENTRY_MA(WPA_PTK, AUTHENTICATION2, wpa_ptk);

    wpa_group_ensure_init(sm->wpa_auth, sm->group);

    /*
     * Definition of ANonce selection in IEEE Std 802.11i-2004 is somewhat
     * ambiguous. The Authenticator state machine uses a counter that is
     * incremented by one for each 4-way handshake. However, the security
     * analysis of 4-way handshake points out that unpredictable nonces
     * help in preventing precomputation attacks. Instead of the state
     * machine definition, use an unpredictable nonce value here to provide
     * stronger protection against potential precomputation attacks.
     */
    if (os_get_random(sm->ANonce, WPA_NONCE_LEN)) {
        wpa_printf( MSG_ERROR, "WPA: Failed to get random data for "
               "ANonce.");
        sm->Disconnect = true;
        return;
    }
    wpa_hexdump(MSG_DEBUG, "WPA: Assign ANonce", sm->ANonce,
            WPA_NONCE_LEN);
    sm->ReAuthenticationRequest = FALSE;
    /* IEEE 802.11i does not clear TimeoutCtr here, but this is more
     * logical place than INITIALIZE since AUTHENTICATION2 can be
     * re-entered on ReAuthenticationRequest without going through
     * INITIALIZE. */
    sm->TimeoutCtr = 0;
}


static int wpa_auth_sm_ptk_update(struct wpa_state_machine *sm)
{
   if (random_get_bytes(sm->ANonce, WPA_NONCE_LEN)) {
       wpa_printf(MSG_ERROR,
              "WPA: Failed to get random data for ANonce");
       sm->Disconnect = TRUE;
       return -1;
   }
   wpa_hexdump(MSG_DEBUG, "WPA: Assign new ANonce", sm->ANonce,
           WPA_NONCE_LEN);
   sm->TimeoutCtr = 0;
   return 0;
}


SM_STATE(WPA_PTK, INITPMK)
{
    u8 msk[2 * PMK_LEN];
    size_t len = 2 * PMK_LEN;

    SM_ENTRY_MA(WPA_PTK, INITPMK, wpa_ptk);
#ifdef CONFIG_IEEE80211R_AP
    sm->xxkey_len = 0;
#endif /* CONFIG_IEEE80211R_AP */

    if (wpa_auth_get_msk(sm->wpa_auth, sm->addr, msk, &len) == 0) {
        wpa_printf( MSG_DEBUG, "WPA: PMK from EAPOL state machine "
               "(len=%lu)", (unsigned long) len);
        memcpy(sm->PMK, msk, PMK_LEN);
#ifdef CONFIG_IEEE80211R_AP
        if (len >= 2 * PMK_LEN) {
            memcpy(sm->xxkey, msk + PMK_LEN, PMK_LEN);
            sm->xxkey_len = PMK_LEN;
        }
#endif /* CONFIG_IEEE80211R_AP */
    } else {
        wpa_printf( MSG_DEBUG, "WPA: Could not get PMK");
    }

    sm->req_replay_counter_used = 0;
    /* IEEE 802.11i does not set keyRun to FALSE, but not doing this
     * will break reauthentication since EAPOL state machines may not be
     * get into AUTHENTICATING state that clears keyRun before WPA state
     * machine enters AUTHENTICATION2 state and goes immediately to INITPMK
     * state and takes PMK from the previously used AAA Key. This will
     * eventually fail in 4-Way Handshake because Supplicant uses PMK
     * derived from the new AAA Key. Setting keyRun = FALSE here seems to
     * be good workaround for this issue. */
    wpa_auth_set_eapol(sm->wpa_auth, sm->addr, WPA_EAPOL_keyRun, 0);
}


SM_STATE(WPA_PTK, INITPSK)
{
    const u8 *psk;
    SM_ENTRY_MA(WPA_PTK, INITPSK, wpa_ptk);
    psk = wpa_auth_get_psk(sm->wpa_auth, sm->addr, NULL);
    if (psk) {
        memcpy(sm->PMK, psk, PMK_LEN);
#ifdef CONFIG_IEEE80211R_AP
        memcpy(sm->xxkey, psk, PMK_LEN);
        sm->xxkey_len = PMK_LEN;
#endif /* CONFIG_IEEE80211R_AP */
    }
#ifdef CONFIG_SAE
    if (wpa_auth_uses_sae(sm) && sm->pmksa) {
        wpa_printf(MSG_DEBUG, "SAE: PMK from PMKSA cache");
        os_memcpy(sm->PMK, sm->pmksa->pmk, sm->pmksa->pmk_len);
        sm->pmk_len = sm->pmksa->pmk_len;
    }
#endif
    sm->req_replay_counter_used = 0;
}


SM_STATE(WPA_PTK, PTKSTART)
{
    u8 buf[2 + RSN_SELECTOR_LEN + PMKID_LEN], *pmkid = NULL;
    size_t pmkid_len = 0;

    SM_ENTRY_MA(WPA_PTK, PTKSTART, wpa_ptk);
    sm->PTKRequest = FALSE;
    sm->TimeoutEvt = FALSE;

    sm->TimeoutCtr++;
    if (sm->TimeoutCtr > (int) dot11RSNAConfigPairwiseUpdateCount) {
        /* No point in sending the EAPOL-Key - we will disconnect
         * immediately following this. */
        return;
    }

    /*
     * TODO: Could add PMKID even with WPA2-PSK, but only if there is only
     * one possible PSK for this STA.
     */
    if (sm->wpa == WPA_VERSION_WPA2 &&
        (wpa_key_mgmt_wpa_ieee8021x(sm->wpa_key_mgmt) ||
        wpa_key_mgmt_sae(sm->wpa_key_mgmt))) {
        pmkid = buf;
        pmkid_len = 2 + RSN_SELECTOR_LEN + PMKID_LEN;
        pmkid[0] = WLAN_EID_VENDOR_SPECIFIC;
        pmkid[1] = RSN_SELECTOR_LEN + PMKID_LEN;
        RSN_SELECTOR_PUT(&pmkid[2], RSN_KEY_DATA_PMKID);
        if (sm->pmksa) {
            wpa_hexdump(MSG_DEBUG,
                    "RSN: Message 1/4 PMKID from PMKSA entry",
                    sm->pmksa->pmkid, PMKID_LEN);
            os_memcpy(&pmkid[2 + RSN_SELECTOR_LEN],
                    sm->pmksa->pmkid, PMKID_LEN);
#ifdef CONFIG_SAE
        } else if (wpa_key_mgmt_sae(sm->wpa_key_mgmt)) {
            if (sm->pmkid_set) {
                wpa_hexdump(MSG_DEBUG,
                        "RSN: Message 1/4 PMKID from SAE",
                        sm->pmkid, PMKID_LEN);
                os_memcpy(&pmkid[2 + RSN_SELECTOR_LEN],
                        sm->pmkid, PMKID_LEN);
        } else {
            /* No PMKID available */
            wpa_printf(MSG_DEBUG,
                    "RSN: No SAE PMKID available for message 1/4");
            pmkid = NULL;
        }
#endif /* CONFIG_SAE */
        } else {
            /*
             * Calculate PMKID since no PMKSA cache entry was
             * available with pre-calculated PMKID.
             */
            rsn_pmkid(sm->PMK, sm->pmk_len, sm->wpa_auth->addr,
                    sm->addr, &pmkid[2 + RSN_SELECTOR_LEN],
                    sm->wpa_key_mgmt);
            wpa_hexdump(MSG_DEBUG,
                    "RSN: Message 1/4 PMKID derived from PMK",
                    &pmkid[2 + RSN_SELECTOR_LEN], PMKID_LEN);
        }
    }
    wpa_send_eapol(sm->wpa_auth, sm,
               WPA_KEY_INFO_ACK | WPA_KEY_INFO_KEY_TYPE, NULL,
               sm->ANonce, pmkid, pmkid_len, 0, 0);
}


static int wpa_derive_ptk(struct wpa_state_machine *sm, const u8 *snonce,
			  const u8 *pmk, struct wpa_ptk *ptk)
{
#ifdef CONFIG_IEEE80211R_AP
    size_t ptk_len = sm->pairwise != WPA_CIPHER_TKIP ? 48 : 64;

    size_t ptk_len = sm->pairwise != WPA_CIPHER_TKIP ? 48 : 64;
    if (wpa_key_mgmt_ft(sm->wpa_key_mgmt))
        return wpa_auth_derive_ptk_ft(sm, pmk, ptk);
#endif /* CONFIG_IEEE80211R_AP */

    return wpa_pmk_to_ptk(pmk, PMK_LEN, "Pairwise key expansion",
                  sm->wpa_auth->addr, sm->addr, sm->ANonce, snonce,
                  ptk, sm->wpa_key_mgmt, sm->pairwise);
}


SM_STATE(WPA_PTK, PTKCALCNEGOTIATING)
{
    struct wpa_ptk PTK;
    int ok = 0;
    const u8 *pmk = NULL;
    u16 key_data_length;
    struct ieee802_1x_hdr *hdr;
    struct wpa_eapol_key *key;
    struct wpa_eapol_ie_parse kde;

    SM_ENTRY_MA(WPA_PTK, PTKCALCNEGOTIATING, wpa_ptk);
    sm->EAPOLKeyReceived = FALSE;
    sm->update_snonce = FALSE;

    /* WPA with IEEE 802.1X: use the derived PMK from EAP
     * WPA-PSK: iterate through possible PSKs and select the one matching
     * the packet */
    for (;;) {
        if (wpa_key_mgmt_wpa_psk(sm->wpa_key_mgmt) &&
            !wpa_key_mgmt_sae(sm->wpa_key_mgmt)) {
            wpa_printf( MSG_DEBUG, "wpa psk");
            pmk = wpa_auth_get_psk(sm->wpa_auth, sm->addr, pmk);
            if (pmk == NULL){
                wpa_printf( MSG_DEBUG, "pmk is null");
                break;
            }
        } else {
            pmk = sm->PMK;
        }

        if (!pmk && sm->pmksa) {
            wpa_printf(MSG_DEBUG, "WPA: Use PMK from PMKSA cache");
            pmk = sm->pmksa->pmk;
        }

        wpa_derive_ptk(sm, sm->SNonce, pmk, &PTK);

        if (wpa_verify_key_mic(sm->wpa_key_mgmt, &PTK,
                       sm->last_rx_eapol_key,
                       sm->last_rx_eapol_key_len) == 0) {
            ok = 1;
            break;
        }

        if (!wpa_key_mgmt_wpa_psk(sm->wpa_key_mgmt) ||
            wpa_key_mgmt_sae(sm->wpa_key_mgmt)) {
            wpa_printf( MSG_DEBUG, "wpa_key_mgmt=%x", sm->wpa_key_mgmt);
            break;
        }
    }

    if (!ok) {
        wpa_printf(MSG_INFO, "invalid MIC in msg 2/4 of 4-Way Handshake");
        return;
    }

    hdr = (struct ieee802_1x_hdr *) sm->last_rx_eapol_key;
    key = (struct wpa_eapol_key *) (hdr + 1);
    key_data_length = WPA_GET_BE16(key->key_data_length);
    if (key_data_length > sm->last_rx_eapol_key_len - sizeof(*hdr) -
       sizeof(*key))
       return;

    if (wpa_parse_kde_ies((u8 *) (key + 1), key_data_length, &kde) < 0) {
        wpa_printf(MSG_DEBUG,
                 "received EAPOL-Key msg 2/4 with invalid Key Data contents");
        return;
    }

#ifdef CONFIG_IEEE80211R_AP
    if (sm->wpa == WPA_VERSION_WPA2 && wpa_key_mgmt_ft(sm->wpa_key_mgmt)) {
        /*
         * Verify that PMKR1Name from EAPOL-Key message 2/4 matches
         * with the value we derived.
         */
        if (memcmp(sm->sup_pmk_r1_name, sm->pmk_r1_name,
                  WPA_PMK_NAME_LEN) != 0) {
            wpa_hexdump(MSG_DEBUG, "FT: PMKR1Name from "
                    "Supplicant",
                    sm->sup_pmk_r1_name, WPA_PMK_NAME_LEN);
            wpa_hexdump(MSG_DEBUG, "FT: Derived PMKR1Name",
                    sm->pmk_r1_name, WPA_PMK_NAME_LEN);
            return;
        }
    }
#endif /* CONFIG_IEEE80211R_AP */

    if ((!sm->rsnxe && kde.rsnxe) ||
        (sm->rsnxe && !kde.rsnxe) ||
        (sm->rsnxe && kde.rsnxe &&
         (sm->rsnxe_len != kde.rsnxe_len ||
          os_memcmp(sm->rsnxe, kde.rsnxe, sm->rsnxe_len) != 0))) {
        wpa_printf(MSG_DEBUG,
                "RSNXE from (Re)AssocReq did not match the one in EAPOL-Key msg 2/4");
        wpa_hexdump(MSG_DEBUG, "RSNXE in AssocReq",
                sm->rsnxe, sm->rsnxe_len);
        wpa_hexdump(MSG_DEBUG, "RSNXE in EAPOL-Key msg 2/4",
                kde.rsnxe, kde.rsnxe_len);
        wpa_sta_disconnect(sm->wpa_auth, sm->addr,
                WLAN_REASON_PREV_AUTH_NOT_VALID);
        return;
    }

    sm->pending_1_of_4_timeout = 0;
    eloop_cancel_timeout(resend_eapol_handle, (void*)(sm->index), NULL);

    if (wpa_key_mgmt_wpa_psk(sm->wpa_key_mgmt) && sm->PMK != pmk) {
        /* PSK may have changed from the previous choice, so update
         * state machine data based on whatever PSK was selected here.
         */
        memcpy(sm->PMK, pmk, PMK_LEN);
    }

    sm->MICVerified = TRUE;

    memcpy(&sm->PTK, &PTK, sizeof(PTK));
    sm->PTK_valid = TRUE;
}


SM_STATE(WPA_PTK, PTKCALCNEGOTIATING2)
{
    SM_ENTRY_MA(WPA_PTK, PTKCALCNEGOTIATING2, wpa_ptk);
    sm->TimeoutCtr = 0;
}


#ifdef CONFIG_IEEE80211W

static int ieee80211w_kde_len(struct wpa_state_machine *sm)
{
    if (sm->mgmt_frame_prot) {
        return 2 + RSN_SELECTOR_LEN + sizeof(struct wpa_igtk_kde);
    }

    return 0;
}


static u8 * ieee80211w_kde_add(struct wpa_state_machine *sm, u8 *pos)
{
    struct wpa_igtk_kde igtk;
    struct wpa_group *gsm = sm->group;

    if (!sm->mgmt_frame_prot)
        return pos;

    igtk.keyid[0] = gsm->GN_igtk;
    igtk.keyid[1] = 0;
    if (gsm->wpa_group_state != WPA_GROUP_SETKEYSDONE ||
        wpa_auth_get_seqnum(sm->wpa_auth, NULL, gsm->GN_igtk, igtk.pn) < 0)
        memset(igtk.pn, 0, sizeof(igtk.pn));
    memcpy(igtk.igtk, gsm->IGTK[gsm->GN_igtk - 4], WPA_IGTK_LEN);
    if (sm->wpa_auth->conf.disable_gtk) {
        /*
         * Provide unique random IGTK to each STA to prevent use of
         * IGTK in the BSS.
         */
        if (os_get_random(igtk.igtk, WPA_IGTK_LEN) < 0)
            return pos;
    }
    pos = wpa_add_kde(pos, RSN_KEY_DATA_IGTK,
              (const u8 *) &igtk, sizeof(igtk), NULL, 0);

    return pos;
}

#else /* CONFIG_IEEE80211W */

static int ieee80211w_kde_len(struct wpa_state_machine *sm)
{
    return 0;
}


static u8 * ieee80211w_kde_add(struct wpa_state_machine *sm, u8 *pos)
{
    return pos;
}

#endif /* CONFIG_IEEE80211W */


SM_STATE(WPA_PTK, PTKINITNEGOTIATING)
{
    u8 rsc[WPA_KEY_RSC_LEN], *_rsc, *gtk, *kde, *pos, dummy_gtk[32];
    size_t gtk_len, kde_len;
    struct wpa_group *gsm = sm->group;
    u8 *wpa_ie;
    int wpa_ie_len, secure, keyidx, encr = 0;

    SM_ENTRY_MA(WPA_PTK, PTKINITNEGOTIATING, wpa_ptk);
    sm->TimeoutEvt = FALSE;

    sm->TimeoutCtr++;
    if (sm->TimeoutCtr > (int) dot11RSNAConfigPairwiseUpdateCount) {
        /* No point in sending the EAPOL-Key - we will disconnect
         * immediately following this. */
        return;
    }

    /* Send EAPOL(1, 1, 1, Pair, P, RSC, ANonce, MIC(PTK), RSNIE, [MDIE],
       GTK[GN], IGTK, [FTIE], [TIE * 2])
     */
    memset(rsc, 0, WPA_KEY_RSC_LEN);
    wpa_auth_get_seqnum(sm->wpa_auth, NULL, gsm->GN, rsc);
    /* If FT is used, wpa_auth->wpa_ie includes both RSNIE and MDIE */
    wpa_ie = sm->wpa_auth->wpa_ie;
    wpa_ie_len = sm->wpa_auth->wpa_ie_len;
    if (sm->wpa == WPA_VERSION_WPA &&
        (sm->wpa_auth->conf.wpa & WPA_PROTO_RSN) &&
        wpa_ie_len > wpa_ie[1] + 2 && wpa_ie[0] == WLAN_EID_RSN) {
        /* WPA-only STA, remove RSN IE */
        wpa_ie = wpa_ie + wpa_ie[1] + 2;
        if (wpa_ie[0] == WLAN_EID_RSNX)
            wpa_ie = wpa_ie + wpa_ie[1] + 2;
        wpa_ie_len = wpa_ie[1] + 2;
    }
    if (sm->wpa == WPA_VERSION_WPA2) {
        /* WPA2 send GTK in the 4-way handshake */
        secure = 1;
        gtk = gsm->GTK[gsm->GN - 1];
        gtk_len = gsm->GTK_len;
        if (sm->wpa_auth->conf.disable_gtk) {
            /*
             * Provide unique random GTK to each STA to prevent use
             * of GTK in the BSS.
             */
            if (os_get_random(dummy_gtk, gtk_len) < 0)
                return;
            gtk = dummy_gtk;
        }
        keyidx = gsm->GN;
        _rsc = rsc;
        encr = 1;
    } else {
        /* WPA does not include GTK in msg 3/4 */
        secure = 0;
        gtk = NULL;
        gtk_len = 0;
        keyidx = 0;
        _rsc = NULL;
        if (sm->rx_eapol_key_secure) {
            /*
             * It looks like Windows 7 supplicant tries to use
             * Secure bit in msg 2/4 after having reported Michael
             * MIC failure and it then rejects the 4-way handshake
             * if msg 3/4 does not set Secure bit. Work around this
             * by setting the Secure bit here even in the case of
             * WPA if the supplicant used it first.
             */
            secure = 1;
        }
    }

    kde_len = wpa_ie_len + ieee80211w_kde_len(sm);
    if (gtk)
        kde_len += 2 + RSN_SELECTOR_LEN + 2 + gtk_len;
#ifdef CONFIG_IEEE80211R_AP
    if (wpa_key_mgmt_ft(sm->wpa_key_mgmt)) {
        kde_len += 2 + PMKID_LEN; /* PMKR1Name into RSN IE */
        kde_len += 300; /* FTIE + 2 * TIE */
    }
#endif /* CONFIG_IEEE80211R_AP */
    kde = (u8 *)os_malloc(kde_len);
    if (kde == NULL)
        return;

    pos = kde;
    memcpy(pos, wpa_ie, wpa_ie_len);
    pos += wpa_ie_len;
#ifdef CONFIG_IEEE80211R_AP
    if (wpa_key_mgmt_ft(sm->wpa_key_mgmt)) {
        int res = wpa_insert_pmkid(kde, pos - kde, sm->pmk_r1_name);
        if (res < 0) {
            wpa_printf( MSG_ERROR, "FT: Failed to insert "
                   "PMKR1Name into RSN IE in EAPOL-Key data");
            os_free(kde);
            return;
        }
        pos += res;
    }
#endif /* CONFIG_IEEE80211R_AP */
    if (gtk) {
        u8 hdr[2];
        hdr[0] = keyidx & 0x03;
        hdr[1] = 0;
        pos = wpa_add_kde(pos, RSN_KEY_DATA_GROUPKEY, hdr, 2,
                  gtk, gtk_len);
    }
    pos = ieee80211w_kde_add(sm, pos);

#ifdef CONFIG_IEEE80211R_AP
    if (wpa_key_mgmt_ft(sm->wpa_key_mgmt)) {
        int res;
        struct wpa_auth_config *conf;

        conf = &sm->wpa_auth->conf;
        res = wpa_write_ftie(conf, conf->r0_key_holder,
                     conf->r0_key_holder_len,
                     NULL, NULL, pos, kde + kde_len - pos,
                     NULL, 0);
        if (res < 0) {
            wpa_printf( MSG_ERROR, "FT: Failed to insert FTIE "
                   "into EAPOL-Key Key Data");
            os_free(kde);
            return;
        }
        pos += res;

        /* TIE[ReassociationDeadline] (TU) */
        *pos++ = WLAN_EID_TIMEOUT_INTERVAL;
        *pos++ = 5;
        *pos++ = WLAN_TIMEOUT_REASSOC_DEADLINE;
        WPA_PUT_LE32(pos, conf->reassociation_deadline);
        pos += 4;

        /* TIE[KeyLifetime] (seconds) */
        *pos++ = WLAN_EID_TIMEOUT_INTERVAL;
        *pos++ = 5;
        *pos++ = WLAN_TIMEOUT_KEY_LIFETIME;
        WPA_PUT_LE32(pos, conf->r0_key_lifetime * 60);
        pos += 4;
    }
#endif /* CONFIG_IEEE80211R_AP */

    wpa_send_eapol(sm->wpa_auth, sm,
               (secure ? WPA_KEY_INFO_SECURE : 0) | WPA_KEY_INFO_MIC |
               WPA_KEY_INFO_ACK | WPA_KEY_INFO_INSTALL |
               WPA_KEY_INFO_KEY_TYPE,
               _rsc, sm->ANonce, kde, pos - kde, keyidx, encr);
    os_free(kde);
}


SM_STATE(WPA_PTK, PTKINITDONE)
{
    SM_ENTRY_MA(WPA_PTK, PTKINITDONE, wpa_ptk);
    sm->EAPOLKeyReceived = FALSE;
    if (sm->Pair) {
        enum wpa_alg alg = wpa_cipher_to_alg(sm->pairwise);
        int klen = wpa_cipher_key_len(sm->pairwise);
        if (wpa_auth_set_key(sm->wpa_auth, 0, alg, sm->addr, 0,
                     sm->PTK.tk, klen)) {
            wpa_sta_disconnect(sm->wpa_auth, sm->addr,
                    WLAN_REASON_PREV_AUTH_NOT_VALID);
            return;
        }
        /* FIX: MLME-SetProtection.Request(TA, Tx_Rx) */
        sm->pairwise_set = TRUE;

        if (sm->wpa_auth->conf.wpa_ptk_rekey) {
            eloop_cancel_timeout(wpa_rekey_ptk, sm->wpa_auth, sm);
            eloop_register_timeout(sm->wpa_auth->conf.
                           wpa_ptk_rekey, 0, wpa_rekey_ptk,
                           sm->wpa_auth, sm);
        }

        if (wpa_key_mgmt_wpa_psk(sm->wpa_key_mgmt)) {
            wpa_auth_set_eapol(sm->wpa_auth, sm->addr,
                       WPA_EAPOL_authorized, 1);
        }
    }

    if (0 /* IBSS == TRUE */) {
        sm->keycount++;
        if (sm->keycount == 2) {
            wpa_auth_set_eapol(sm->wpa_auth, sm->addr,
                       WPA_EAPOL_portValid, 1);
        }
    } else {
        wpa_auth_set_eapol(sm->wpa_auth, sm->addr, WPA_EAPOL_portValid,
                   1);
    }
    wpa_auth_set_eapol(sm->wpa_auth, sm->addr, WPA_EAPOL_keyAvailable, 0);
    wpa_auth_set_eapol(sm->wpa_auth, sm->addr, WPA_EAPOL_keyDone, 1);
    if (sm->wpa == WPA_VERSION_WPA)
        sm->PInitAKeys = TRUE;
    else
        sm->has_GTK = TRUE;


{
    esp_wifi_wpa_ptk_init_done_internal(sm->addr);
}
#ifdef CONFIG_IEEE80211R_AP
    wpa_ft_push_pmk_r1(sm->wpa_auth, sm->addr);
#endif /* CONFIG_IEEE80211R_AP */
}


SM_STEP(WPA_PTK)
{

    if (sm->Init)
        SM_ENTER(WPA_PTK, INITIALIZE);
    else if (sm->Disconnect
         /* || FIX: dot11RSNAConfigSALifetime timeout */) {
        SM_ENTER(WPA_PTK, DISCONNECT);
    }
    else if (sm->DeauthenticationRequest)
        SM_ENTER(WPA_PTK, DISCONNECTED);
    else if (sm->AuthenticationRequest)
        SM_ENTER(WPA_PTK, AUTHENTICATION);
    else if (sm->ReAuthenticationRequest)
        SM_ENTER(WPA_PTK, AUTHENTICATION2);
    else if (sm->PTKRequest) {
        if (wpa_auth_sm_ptk_update(sm) < 0)
            SM_ENTER(WPA_PTK, DISCONNECTED);
        else
            SM_ENTER(WPA_PTK, PTKSTART);
    } else switch (sm->wpa_ptk_state) {
    case WPA_PTK_INITIALIZE:
        break;
    case WPA_PTK_DISCONNECT:
        SM_ENTER(WPA_PTK, DISCONNECTED);
        break;
    case WPA_PTK_DISCONNECTED:
        SM_ENTER(WPA_PTK, INITIALIZE);
        break;
    case WPA_PTK_AUTHENTICATION:
        SM_ENTER(WPA_PTK, AUTHENTICATION2);
        break;
    case WPA_PTK_AUTHENTICATION2:
        if (wpa_key_mgmt_wpa_ieee8021x(sm->wpa_key_mgmt) &&
            wpa_auth_get_eapol(sm->wpa_auth, sm->addr,
                       WPA_EAPOL_keyRun) > 0)
            SM_ENTER(WPA_PTK, INITPMK);
        else if (wpa_key_mgmt_wpa_psk(sm->wpa_key_mgmt)
             /* FIX: && 802.1X::keyRun */)
            SM_ENTER(WPA_PTK, INITPSK);
        break;
    case WPA_PTK_INITPMK:
        if (wpa_auth_get_eapol(sm->wpa_auth, sm->addr,
                       WPA_EAPOL_keyAvailable) > 0)
            SM_ENTER(WPA_PTK, PTKSTART);
        else {
            SM_ENTER(WPA_PTK, DISCONNECT);
        }
        break;
    case WPA_PTK_INITPSK:
        if (wpa_auth_get_psk(sm->wpa_auth, sm->addr, NULL)) {
            SM_ENTER(WPA_PTK, PTKSTART);
#ifdef CONFIG_SAE
        } else if (wpa_auth_uses_sae(sm) && sm->pmksa) {
            SM_ENTER(WPA_PTK, PTKSTART);
#endif /* CONFIG_SAE */
        } else {
            SM_ENTER(WPA_PTK, DISCONNECT);
        }
        break;
    case WPA_PTK_PTKSTART:
        if (sm->EAPOLKeyReceived && !sm->EAPOLKeyRequest &&
            sm->EAPOLKeyPairwise)
            SM_ENTER(WPA_PTK, PTKCALCNEGOTIATING);
        else if (sm->TimeoutCtr >
             (int) dot11RSNAConfigPairwiseUpdateCount) {
            sm->disconnect_reason =
                WLAN_REASON_4WAY_HANDSHAKE_TIMEOUT;
            SM_ENTER(WPA_PTK, DISCONNECT);
        } else if (sm->TimeoutEvt)
            SM_ENTER(WPA_PTK, PTKSTART);
        break;
    case WPA_PTK_PTKCALCNEGOTIATING:
        if (sm->MICVerified)
            SM_ENTER(WPA_PTK, PTKCALCNEGOTIATING2);
        else if (sm->EAPOLKeyReceived && !sm->EAPOLKeyRequest &&
             sm->EAPOLKeyPairwise)
            SM_ENTER(WPA_PTK, PTKCALCNEGOTIATING);
        else if (sm->TimeoutEvt)
            SM_ENTER(WPA_PTK, PTKSTART);
        break;
    case WPA_PTK_PTKCALCNEGOTIATING2:
        SM_ENTER(WPA_PTK, PTKINITNEGOTIATING);
        break;
    case WPA_PTK_PTKINITNEGOTIATING:
        if (sm->update_snonce)
            SM_ENTER(WPA_PTK, PTKCALCNEGOTIATING);
        else if (sm->EAPOLKeyReceived && !sm->EAPOLKeyRequest &&
             sm->EAPOLKeyPairwise && sm->MICVerified)
            SM_ENTER(WPA_PTK, PTKINITDONE);
        else if (sm->TimeoutCtr >
             (int) dot11RSNAConfigPairwiseUpdateCount) {
            sm->disconnect_reason =
                WLAN_REASON_4WAY_HANDSHAKE_TIMEOUT;
            SM_ENTER(WPA_PTK, DISCONNECT);
        } else if (sm->TimeoutEvt)
            SM_ENTER(WPA_PTK, PTKINITNEGOTIATING);
        break;
    case WPA_PTK_PTKINITDONE:
        break;
    }
}


SM_STATE(WPA_PTK_GROUP, IDLE)
{
    SM_ENTRY_MA(WPA_PTK_GROUP, IDLE, wpa_ptk_group);
    if (sm->Init) {
        /* Init flag is not cleared here, so avoid busy
         * loop by claiming nothing changed. */
        sm->changed = FALSE;
    }
    sm->GTimeoutCtr = 0;
}


SM_STATE(WPA_PTK_GROUP, REKEYNEGOTIATING)
{
    u8 rsc[WPA_KEY_RSC_LEN];
    struct wpa_group *gsm = sm->group;
    u8 *kde, *pos, hdr[2];
    size_t kde_len;
    u8 *gtk, dummy_gtk[32];

    SM_ENTRY_MA(WPA_PTK_GROUP, REKEYNEGOTIATING, wpa_ptk_group);

    sm->GTimeoutCtr++;
    if (sm->GTimeoutCtr > (int) dot11RSNAConfigGroupUpdateCount) {
        /* No point in sending the EAPOL-Key - we will disconnect
         * immediately following this. */
        return;
    }

    if (sm->wpa == WPA_VERSION_WPA)
        sm->PInitAKeys = FALSE;
    sm->TimeoutEvt = FALSE;
    /* Send EAPOL(1, 1, 1, !Pair, G, RSC, GNonce, MIC(PTK), GTK[GN]) */
    memset(rsc, 0, WPA_KEY_RSC_LEN);
    if (gsm->wpa_group_state == WPA_GROUP_SETKEYSDONE)
        wpa_auth_get_seqnum(sm->wpa_auth, NULL, gsm->GN, rsc);

    gtk = gsm->GTK[gsm->GN - 1];
    if (sm->wpa_auth->conf.disable_gtk) {
        /*
         * Provide unique random GTK to each STA to prevent use
         * of GTK in the BSS.
         */
        if (os_get_random(dummy_gtk, gsm->GTK_len) < 0)
            return;
        gtk = dummy_gtk;
    }
    if (sm->wpa == WPA_VERSION_WPA2) {
        kde_len = 2 + RSN_SELECTOR_LEN + 2 + gsm->GTK_len +
            ieee80211w_kde_len(sm);
        kde = (u8 *)os_malloc(kde_len);
        if (kde == NULL)
            return;

        pos = kde;
        hdr[0] = gsm->GN & 0x03;
        hdr[1] = 0;
        pos = wpa_add_kde(pos, RSN_KEY_DATA_GROUPKEY, hdr, 2,
                  gtk, gsm->GTK_len);
        pos = ieee80211w_kde_add(sm, pos);
    } else {
        kde = gtk;
        pos = kde + gsm->GTK_len;
    }

    wpa_send_eapol(sm->wpa_auth, sm,
               WPA_KEY_INFO_SECURE | WPA_KEY_INFO_MIC |
               WPA_KEY_INFO_ACK |
               (!sm->Pair ? WPA_KEY_INFO_INSTALL : 0),
               rsc, gsm->GNonce, kde, pos - kde, gsm->GN, 1);
    if (sm->wpa == WPA_VERSION_WPA2)
        os_free(kde);  // NOLINT(clang-analyzer-unix.Malloc)
}


SM_STATE(WPA_PTK_GROUP, REKEYESTABLISHED)
{
    SM_ENTRY_MA(WPA_PTK_GROUP, REKEYESTABLISHED, wpa_ptk_group);
    sm->EAPOLKeyReceived = FALSE;
    if (sm->GUpdateStationKeys)
        sm->group->GKeyDoneStations--;
    sm->GUpdateStationKeys = FALSE;
    sm->GTimeoutCtr = 0;
    /* FIX: MLME.SetProtection.Request(TA, Tx_Rx) */
    sm->has_GTK = TRUE;
}


SM_STATE(WPA_PTK_GROUP, KEYERROR)
{
    SM_ENTRY_MA(WPA_PTK_GROUP, KEYERROR, wpa_ptk_group);
    if (sm->GUpdateStationKeys)
        sm->group->GKeyDoneStations--;
    sm->GUpdateStationKeys = FALSE;
    sm->Disconnect = TRUE;
    sm->disconnect_reason = WLAN_REASON_GROUP_KEY_UPDATE_TIMEOUT;
}


SM_STEP(WPA_PTK_GROUP)
{
    if (sm->Init || sm->PtkGroupInit) {
        SM_ENTER(WPA_PTK_GROUP, IDLE);
        sm->PtkGroupInit = FALSE;
    } else switch (sm->wpa_ptk_group_state) {
    case WPA_PTK_GROUP_IDLE:
        if (sm->GUpdateStationKeys ||
            (sm->wpa == WPA_VERSION_WPA && sm->PInitAKeys))
            SM_ENTER(WPA_PTK_GROUP, REKEYNEGOTIATING);
        break;
    case WPA_PTK_GROUP_REKEYNEGOTIATING:
        if (sm->EAPOLKeyReceived && !sm->EAPOLKeyRequest &&
            !sm->EAPOLKeyPairwise && sm->MICVerified)
            SM_ENTER(WPA_PTK_GROUP, REKEYESTABLISHED);
        else if (sm->GTimeoutCtr >
             (int) dot11RSNAConfigGroupUpdateCount)
            SM_ENTER(WPA_PTK_GROUP, KEYERROR);
        else if (sm->TimeoutEvt)
            SM_ENTER(WPA_PTK_GROUP, REKEYNEGOTIATING);
        break;
    case WPA_PTK_GROUP_KEYERROR:
        SM_ENTER(WPA_PTK_GROUP, IDLE);
        break;
    case WPA_PTK_GROUP_REKEYESTABLISHED:
        SM_ENTER(WPA_PTK_GROUP, IDLE);
        break;
    }
}


static int wpa_gtk_update(struct wpa_authenticator *wpa_auth,
              struct wpa_group *group)
{
    int ret = 0;

    memcpy(group->GNonce, group->Counter, WPA_NONCE_LEN);
    inc_byte_array(group->Counter, WPA_NONCE_LEN);

    if (wpa_gmk_to_gtk(group->GMK, "Group key expansion",
               wpa_auth->addr, group->GNonce,
               group->GTK[group->GN - 1], group->GTK_len) < 0)
        ret = -1;
    wpa_hexdump_key(MSG_DEBUG, "GTK",
            group->GTK[group->GN - 1], group->GTK_len);

#ifdef CONFIG_IEEE80211W
    if (wpa_auth->conf.ieee80211w != NO_MGMT_FRAME_PROTECTION) {
        memcpy(group->GNonce, group->Counter, WPA_NONCE_LEN);
        inc_byte_array(group->Counter, WPA_NONCE_LEN);
        if (wpa_gmk_to_gtk(group->GMK, "IGTK key expansion",
                   wpa_auth->addr, group->GNonce,
                   group->IGTK[group->GN_igtk - 4],
                   WPA_IGTK_LEN) < 0)
            ret = -1;
        wpa_hexdump_key(MSG_DEBUG, "IGTK",
                group->IGTK[group->GN_igtk - 4], WPA_IGTK_LEN);
    }
#endif /* CONFIG_IEEE80211W */

    return ret;
}


static void wpa_group_gtk_init(struct wpa_authenticator *wpa_auth,
                   struct wpa_group *group)
{
    wpa_printf( MSG_DEBUG, "WPA: group state machine entering state "
           "GTK_INIT (VLAN-ID %d)", group->vlan_id);
    group->changed = FALSE; /* GInit is not cleared here; avoid loop */
    group->wpa_group_state = WPA_GROUP_GTK_INIT;

    /* GTK[0..N] = 0 */
    memset(group->GTK, 0, sizeof(group->GTK));
    group->GN = 1;
    group->GM = 2;
#ifdef CONFIG_IEEE80211W
    group->GN_igtk = 4;
    group->GM_igtk = 5;
#endif /* CONFIG_IEEE80211W */
    /* GTK[GN] = CalcGTK() */
    wpa_gtk_update(wpa_auth, group);
}


static int wpa_group_update_sta(struct wpa_state_machine *sm, void *ctx)
{
    if (ctx != NULL && ctx != sm->group)
        return 0;

    if (sm->wpa_ptk_state != WPA_PTK_PTKINITDONE) {
        sm->GUpdateStationKeys = FALSE;
        return 0;
    }
    if (sm->GUpdateStationKeys) {
        /*
         * This should not really happen, so add a debug log entry.
         * Since we clear the GKeyDoneStations before the loop, the
         * station needs to be counted here anyway.
         */
    }

    /* Do not rekey GTK/IGTK when STA is in WNM-Sleep Mode */
    if (sm->is_wnmsleep)
        return 0;

    sm->group->GKeyDoneStations++;
    sm->GUpdateStationKeys = TRUE;

    wpa_sm_step(sm);
    return 0;
}


#ifdef CONFIG_WNM_AP
/* update GTK when exiting WNM-Sleep Mode */
void wpa_wnmsleep_rekey_gtk(struct wpa_state_machine *sm)
{
    if (sm->is_wnmsleep)
        return;

    wpa_group_update_sta(sm, NULL);
}


void wpa_set_wnmsleep(struct wpa_state_machine *sm, int flag)
{
    sm->is_wnmsleep = !!flag;
}


int wpa_wnmsleep_gtk_subelem(struct wpa_state_machine *sm, u8 *pos)
{
    struct wpa_group *gsm = sm->group;
    u8 *start = pos;

    /*
     * GTK subelement:
     * Sub-elem ID[1] | Length[1] | Key Info[2] | Key Length[1] | RSC[8] |
     * Key[5..32]
     */
    *pos++ = WNM_SLEEP_SUBELEM_GTK;
    *pos++ = 11 + gsm->GTK_len;
    /* Key ID in B0-B1 of Key Info */
    WPA_PUT_LE16(pos, gsm->GN & 0x03);
    pos += 2;
    *pos++ = gsm->GTK_len;
    if (wpa_auth_get_seqnum(sm->wpa_auth, NULL, gsm->GN, pos) != 0)
        return 0;
    pos += 8;
    memcpy(pos, gsm->GTK[gsm->GN - 1], gsm->GTK_len);
    pos += gsm->GTK_len;

    wpa_printf( MSG_DEBUG, "WNM: GTK Key ID %u in WNM-Sleep Mode exit",
           gsm->GN);
    wpa_hexdump_key(MSG_DEBUG, "WNM: GTK in WNM-Sleep Mode exit",
            gsm->GTK[gsm->GN - 1], gsm->GTK_len);

    return pos - start;
}


#ifdef CONFIG_IEEE80211W
int wpa_wnmsleep_igtk_subelem(struct wpa_state_machine *sm, u8 *pos)
{
    struct wpa_group *gsm = sm->group;
    u8 *start = pos;

    /*
     * IGTK subelement:
     * Sub-elem ID[1] | Length[1] | KeyID[2] | PN[6] | Key[16]
     */
    *pos++ = WNM_SLEEP_SUBELEM_IGTK;
    *pos++ = 2 + 6 + WPA_IGTK_LEN;
    WPA_PUT_LE16(pos, gsm->GN_igtk);
    pos += 2;
    if (wpa_auth_get_seqnum(sm->wpa_auth, NULL, gsm->GN_igtk, pos) != 0)
        return 0;
    pos += 6;

    memcpy(pos, gsm->IGTK[gsm->GN_igtk - 4], WPA_IGTK_LEN);
    pos += WPA_IGTK_LEN;

    wpa_printf( MSG_DEBUG, "WNM: IGTK Key ID %u in WNM-Sleep Mode exit",
           gsm->GN_igtk);
    wpa_hexdump_key(MSG_DEBUG, "WNM: IGTK in WNM-Sleep Mode exit",
            gsm->IGTK[gsm->GN_igtk - 4], WPA_IGTK_LEN);

    return pos - start;
}
#endif /* CONFIG_IEEE80211W */
#endif /* CONFIG_WNM_AP */


static void wpa_group_setkeys(struct wpa_authenticator *wpa_auth,
                  struct wpa_group *group)
{
    int tmp;

    wpa_printf( MSG_DEBUG, "WPA: group state machine entering state "
           "SETKEYS (VLAN-ID %d)", group->vlan_id);
    group->changed = TRUE;
    group->wpa_group_state = WPA_GROUP_SETKEYS;
    group->GTKReKey = FALSE;
    tmp = group->GM;
    group->GM = group->GN;
    group->GN = tmp;
#ifdef CONFIG_IEEE80211W
    tmp = group->GM_igtk;
    group->GM_igtk = group->GN_igtk;
    group->GN_igtk = tmp;
#endif /* CONFIG_IEEE80211W */
    /* "GKeyDoneStations = GNoStations" is done in more robust way by
     * counting the STAs that are marked with GUpdateStationKeys instead of
     * including all STAs that could be in not-yet-completed state. */
    wpa_gtk_update(wpa_auth, group);

    if (group->GKeyDoneStations) {
        wpa_printf( MSG_DEBUG, "wpa_group_setkeys: Unexpected "
               "GKeyDoneStations=%d when starting new GTK rekey",
               group->GKeyDoneStations);
        group->GKeyDoneStations = 0;
    }
    wpa_auth_for_each_sta(wpa_auth, wpa_group_update_sta, group);
    wpa_printf( MSG_DEBUG, "wpa_group_setkeys: GKeyDoneStations=%d",
           group->GKeyDoneStations);
}


static int wpa_group_config_group_keys(struct wpa_authenticator *wpa_auth,
                       struct wpa_group *group)
{
    int ret = 0;
    if (wpa_auth_set_key(wpa_auth, group->vlan_id,
                 wpa_cipher_to_alg(wpa_auth->conf.wpa_group),
                 (uint8_t *)broadcast_ether_addr, group->GN,
                 group->GTK[group->GN - 1], group->GTK_len) < 0)
        ret = -1;

#ifdef CONFIG_IEEE80211W
    if (wpa_auth->conf.ieee80211w != NO_MGMT_FRAME_PROTECTION &&
        wpa_auth_set_key(wpa_auth, group->vlan_id, WIFI_WPA_ALG_IGTK,
                 broadcast_ether_addr, group->GN_igtk,
                 group->IGTK[group->GN_igtk - 4],
                 WPA_IGTK_LEN) < 0)
        ret = -1;

#endif /* CONFIG_IEEE80211W */
    return ret;
}


static int wpa_group_setkeysdone(struct wpa_authenticator *wpa_auth,
                 struct wpa_group *group)
{
    wpa_printf( MSG_DEBUG, "WPA: group state machine entering state "
           "SETKEYSDONE (VLAN-ID %d)", group->vlan_id);
    group->changed = TRUE;
    group->wpa_group_state = WPA_GROUP_SETKEYSDONE;

    if (wpa_group_config_group_keys(wpa_auth, group) < 0)
        return -1;

    return 0;
}


static void wpa_group_sm_step(struct wpa_authenticator *wpa_auth,
                  struct wpa_group *group)
{
    if (group->GInit) {
        wpa_group_gtk_init(wpa_auth, group);
    } else if (group->wpa_group_state == WPA_GROUP_GTK_INIT &&
           group->GTKAuthenticator) {
        wpa_group_setkeysdone(wpa_auth, group);
    } else if (group->wpa_group_state == WPA_GROUP_SETKEYSDONE &&
           group->GTKReKey) {
        wpa_group_setkeys(wpa_auth, group);
    } else if (group->wpa_group_state == WPA_GROUP_SETKEYS) {
        if (group->GKeyDoneStations == 0)
            wpa_group_setkeysdone(wpa_auth, group);
        else if (group->GTKReKey)
            wpa_group_setkeys(wpa_auth, group);
    }
}


static int wpa_sm_step(struct wpa_state_machine *sm)
{
    if (sm == NULL)
        return 0;

    if (sm->in_step_loop) {
        /* This should not happen, but if it does, make sure we do not
         * end up freeing the state machine too early by exiting the
         * recursive call. */
        wpa_printf( MSG_ERROR, "WPA: wpa_sm_step() called recursively");
        return 0;
    }

    sm->in_step_loop = 1;
    do {
        if (sm->pending_deinit)
            break;

        sm->changed = FALSE;
        sm->wpa_auth->group->changed = FALSE;

        SM_STEP_RUN(WPA_PTK);
        if (sm->pending_deinit)
            break;
        SM_STEP_RUN(WPA_PTK_GROUP);
        if (sm->pending_deinit)
            break;
        wpa_group_sm_step(sm->wpa_auth, sm->group);
    } while (sm->changed || sm->wpa_auth->group->changed);
    sm->in_step_loop = 0;

    if (sm->pending_deinit) {
        wpa_printf( MSG_DEBUG, "WPA: Completing pending STA state "
               "machine deinit for " MACSTR, MAC2STR(sm->addr));
        wpa_free_sta_sm(sm);
        return 1;
    }
    return 0;
}

void wpa_deinit(struct wpa_authenticator *wpa_auth)
{
    struct wpa_group *group, *prev;
	pmksa_cache_auth_deinit(wpa_auth->pmksa);
    if (wpa_auth->wpa_ie != NULL) {
        os_free(wpa_auth->wpa_ie);
    }
    if (wpa_auth->group != NULL) {
        group = wpa_auth->group;
        while (group) {
            prev = group;
            group = group->next;
            bin_clear_free(prev, sizeof(*prev));
        }
    }
    os_free(wpa_auth);

}

#ifdef CONFIG_ESP_WIFI_SOFTAP_SUPPORT
bool wpa_ap_join(struct sta_info *sta, uint8_t *bssid, uint8_t *wpa_ie,
                uint8_t wpa_ie_len, uint8_t *rsnxe, uint8_t rsnxe_len,
                bool *pmf_enable, int subtype, uint8_t *pairwise_cipher)
{
    struct hostapd_data *hapd = (struct hostapd_data*)esp_wifi_get_hostap_private_internal();
    enum wpa_validate_result status_code = WPA_IE_OK;
    int resp = WLAN_STATUS_SUCCESS;
    bool omit_rsnxe = false;

    if (!sta || !bssid || !wpa_ie) {
        return false;
    }

    if (hapd) {
        if (hapd->wpa_auth->conf.wpa) {
            if (sta->wpa_sm){
                wpa_auth_sta_deinit(sta->wpa_sm);
            }

            sta->wpa_sm = wpa_auth_sta_init(hapd->wpa_auth, bssid);
            wpa_printf( MSG_DEBUG, "init wpa sm=%p", sta->wpa_sm);

            if (sta->wpa_sm == NULL) {
                resp = WLAN_STATUS_AP_UNABLE_TO_HANDLE_NEW_STA;
                goto send_resp;
            }

            status_code = wpa_validate_wpa_ie(hapd->wpa_auth, sta->wpa_sm, wpa_ie, wpa_ie_len, rsnxe, rsnxe_len);

#ifdef CONFIG_SAE
            if (wpa_auth_uses_sae(sta->wpa_sm) && sta->sae &&
                sta->sae->state == SAE_ACCEPTED) {
                wpa_auth_add_sae_pmkid(sta->wpa_sm, sta->sae->pmkid);
            }
#endif /* CONFIG_SAE */

            resp = wpa_res_to_status_code(status_code);

send_resp:
            if (!rsnxe) {
                omit_rsnxe = true;
            }

            if (esp_send_assoc_resp(hapd, bssid, resp, omit_rsnxe, subtype) != WLAN_STATUS_SUCCESS) {
                resp = WLAN_STATUS_AP_UNABLE_TO_HANDLE_NEW_STA;
            }

            if (resp != WLAN_STATUS_SUCCESS) {
                return false;
            }

            //Check whether AP uses Management Frame Protection for this connection
            *pmf_enable = wpa_auth_uses_mfp(sta->wpa_sm);
            *pairwise_cipher = GET_BIT_POSITION(sta->wpa_sm->pairwise);
        }

        wpa_auth_sta_associated(hapd->wpa_auth, sta->wpa_sm);
    }

    return true;
}

#ifdef CONFIG_WPS_REGISTRAR
static void ap_free_sta_timeout(void *ctx, void *data)
{
    struct hostapd_data *hapd = (struct hostapd_data *) ctx;
    u8 *addr = (u8 *) data;
    struct sta_info *sta = ap_get_sta(hapd, addr);

    if (sta) {
        ap_free_sta(hapd, sta);
    }

    os_free(addr);
}
#endif

bool wpa_ap_remove(u8* bssid)
{
    struct hostapd_data *hapd = hostapd_get_hapd_data();

    if (!hapd) {
        return false;
    }
    struct sta_info *sta = ap_get_sta(hapd, bssid);
    if (!sta) {
        return false;
    }

#ifdef CONFIG_SAE
    if (sta->lock) {
        if (os_semphr_take(sta->lock, 0)) {
            ap_free_sta(hapd, sta);
        } else {
            sta->remove_pending = true;
        }
        return true;
    }
#endif /* CONFIG_SAE */

#ifdef CONFIG_WPS_REGISTRAR
    wpa_printf(MSG_DEBUG, "wps_status=%d", wps_get_status());
    if (wps_get_status() == WPS_STATUS_PENDING) {
        u8 *addr = os_malloc(ETH_ALEN);

        if (!addr) {
            return false;
        }
        os_memcpy(addr, sta->addr, ETH_ALEN);
        eloop_register_timeout(0, 10000, ap_free_sta_timeout, hapd, addr);
    } else
#endif
    ap_free_sta(hapd, sta);

    return true;
}
#endif /* CONFIG_ESP_WIFI_SOFTAP_SUPPORT */

void wpa_auth_pmksa_remove(struct wpa_authenticator *wpa_auth,
               const u8 *sta_addr)
{
    struct rsn_pmksa_cache_entry *pmksa;

    if (wpa_auth == NULL || wpa_auth->pmksa == NULL)
        return;
    pmksa = pmksa_cache_auth_get(wpa_auth->pmksa, sta_addr, NULL);
    if (pmksa) {
        wpa_printf(MSG_DEBUG, "WPA: Remove PMKSA cache entry for "
               MACSTR " based on request", MAC2STR(sta_addr));
        pmksa_cache_free_entry(wpa_auth->pmksa, pmksa);
    }
}

int wpa_auth_uses_sae(struct wpa_state_machine *sm)
{
    if (sm == NULL)
        return 0;
    return wpa_key_mgmt_sae(sm->wpa_key_mgmt);
}
