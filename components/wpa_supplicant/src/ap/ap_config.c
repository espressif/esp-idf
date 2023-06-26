/*
 * hostapd / Configuration helper functions
 * Copyright (c) 2003-2012, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"

#include "utils/common.h"
#include "crypto/sha1.h"
#include "common/ieee802_11_defs.h"
#include "common/eapol_common.h"
#include "common/sae.h"
#include "ap/wpa_auth.h"
#include "ap/ap_config.h"
#include "utils/wpa_debug.h"
#include "ap/hostapd.h"
#include "ap/wpa_auth_i.h"
#include "esp_wifi_driver.h"
#include "esp_wifi_types.h"

void hostapd_config_defaults_bss(struct hostapd_bss_config *bss)
{
    bss->auth_algs = WPA_AUTH_ALG_OPEN | WPA_AUTH_ALG_SHARED;

    bss->wep_rekeying_period = 300;
    /* use key0 in individual key and key1 in broadcast key */
    bss->broadcast_key_idx_min = 1;
    bss->broadcast_key_idx_max = 2;

    bss->wpa_group_rekey = 600;
    bss->wpa_gmk_rekey = 86400;
    bss->wpa_key_mgmt = WPA_KEY_MGMT_PSK;
    bss->wpa_pairwise = WPA_CIPHER_TKIP;
    bss->wpa_group = WPA_CIPHER_TKIP;
    bss->rsn_pairwise = 0;

    bss->max_num_sta = MAX_STA_COUNT;

    bss->dtim_period = 2;

    bss->ap_max_inactivity = 5*60;  //AP_MAX_INACTIVITY;
    bss->eapol_version = EAPOL_VERSION;

    bss->max_listen_interval = 65535;

#ifdef CONFIG_IEEE80211W
    bss->assoc_sa_query_max_timeout = 1000;
    bss->assoc_sa_query_retry_timeout = 201;
#endif /* CONFIG_IEEE80211W */
#ifdef EAP_SERVER_FAST
     /* both anonymous and authenticated provisioning */
    bss->eap_fast_prov = 3;
    bss->pac_key_lifetime = 7 * 24 * 60 * 60;
    bss->pac_key_refresh_time = 1 * 24 * 60 * 60;
#endif /* EAP_SERVER_FAST */

    /* Set to -1 as defaults depends on HT in setup */
    bss->wmm_enabled = -1;

#ifdef CONFIG_IEEE80211R
    bss->ft_over_ds = 1;
#endif /* CONFIG_IEEE80211R */

}


struct hostapd_config * hostapd_config_defaults(void)
{
#define ecw2cw(ecw) ((1 << (ecw)) - 1)

    struct hostapd_config *conf;
    struct hostapd_bss_config *bss;
#undef ecw2cw

    conf = (struct hostapd_config *)os_zalloc(sizeof(*conf));
    bss = (struct hostapd_bss_config *)os_zalloc(sizeof(*bss));
    if (conf == NULL || bss == NULL) {
    	wpa_printf(MSG_DEBUG, "Failed to allocate memory for "
               "configuration data.");
        os_free(conf);
        os_free(bss);
        return NULL;
    }

    hostapd_config_defaults_bss(bss);

    conf->num_bss = 1;
    conf->bss = bss;

    conf->beacon_int = 100;
    conf->rts_threshold = -1; /* use driver default: 2347 */
    conf->fragm_threshold = -1; /* user driver default: 2346 */
    conf->send_probe_response = 1;

    conf->ht_capab = HT_CAP_INFO_SMPS_DISABLED;

    conf->ap_table_max_size = 255;
    conf->ap_table_expiration_time = 60;

    return conf;
}


int hostapd_mac_comp(const void *a, const void *b)
{
    return memcmp(a, b, sizeof(macaddr));
}


int hostapd_mac_comp_empty(const void *a)
{
    u8 empty[ETH_ALEN];

    os_bzero(empty, ETH_ALEN);

    return memcmp(a, empty, ETH_ALEN);
}

static int hostapd_derive_psk(struct hostapd_ssid *ssid)
{
    ssid->wpa_psk = (struct hostapd_wpa_psk *)os_zalloc(sizeof(struct hostapd_wpa_psk));
    if (ssid->wpa_psk == NULL) {
    	wpa_printf(MSG_ERROR, "Unable to alloc space for PSK");
        return -1;
    }
    wpa_hexdump_ascii(MSG_DEBUG, "SSID",
              (u8 *) ssid->ssid, ssid->ssid_len);
    wpa_hexdump_ascii_key(MSG_DEBUG, "PSK (ASCII passphrase)",
                  (u8 *) ssid->wpa_passphrase,
                  strlen(ssid->wpa_passphrase));
#ifdef ESP_SUPPLICANT
    memcpy(ssid->wpa_psk->psk, esp_wifi_ap_get_prof_pmk_internal(), PMK_LEN);
#else
    /* It's too SLOW */
    pbkdf2_sha1(ssid->wpa_passphrase,
            ssid->ssid, ssid->ssid_len,
            4096, ssid->wpa_psk->psk, PMK_LEN);
#endif
    wpa_hexdump_key(MSG_DEBUG, "PSK (from passphrase)",
            ssid->wpa_psk->psk, PMK_LEN);
    return 0;
}


int hostapd_setup_sae_pt(struct hostapd_bss_config *conf)
{
#ifdef CONFIG_SAE
    struct hostapd_ssid *ssid = &conf->ssid;
    if ((conf->sae_pwe == SAE_PWE_HUNT_AND_PECK ||
        !wpa_key_mgmt_sae(conf->wpa_key_mgmt)))
        return 0; /* PT not needed */

    sae_deinit_pt(ssid->pt);
    ssid->pt = NULL;
    if (ssid->wpa_passphrase) {
        ssid->pt = sae_derive_pt(conf->sae_groups, ssid->ssid,
                     ssid->ssid_len,
                     (const u8 *) ssid->wpa_passphrase,
                     os_strlen(ssid->wpa_passphrase),
                     NULL);
        if (!ssid->pt)
            return -1;
    }

#endif /* CONFIG_SAE */

    return 0;
}


int hostapd_setup_wpa_psk(struct hostapd_bss_config *conf)
{
    struct hostapd_ssid *ssid = &conf->ssid;

    if (hostapd_setup_sae_pt(conf) < 0)
        return -1;

    if (ssid->wpa_passphrase != NULL) {
        if (ssid->wpa_psk != NULL) {
        	wpa_printf(MSG_DEBUG, "Using pre-configured WPA PSK "
                   "instead of passphrase");
        } else {
        	wpa_printf(MSG_DEBUG, "Deriving WPA PSK based on "
                   "passphrase\n");
            if (hostapd_derive_psk(ssid) < 0)
                return -1;
        }
        ssid->wpa_psk->group = 1;
    }

    return 0;
}


int hostapd_wep_key_cmp(struct hostapd_wep_keys *a, struct hostapd_wep_keys *b)
{
    int i;

    if (a->idx != b->idx || a->default_len != b->default_len)
        return 1;
    for (i = 0; i < NUM_WEP_KEYS; i++)
        if (a->len[i] != b->len[i] ||
        	memcmp(a->key[i], b->key[i], a->len[i]) != 0)
            return 1;
    return 0;
}

/**
 * hostapd_maclist_found - Find a MAC address from a list
 * @list: MAC address list
 * @num_entries: Number of addresses in the list
 * @addr: Address to search for
 * @vlan_id: Buffer for returning VLAN ID or %NULL if not needed
 * Returns: 1 if address is in the list or 0 if not.
 *
 * Perform a binary search for given MAC address from a pre-sorted list.
 */
int hostapd_maclist_found(struct mac_acl_entry *list, int num_entries,
              const u8 *addr, int *vlan_id)
{
    int start, end, middle, res;

    start = 0;
    end = num_entries - 1;

    while (start <= end) {
        middle = (start + end) / 2;
        res = memcmp(list[middle].addr, addr, ETH_ALEN);
        if (res == 0) {
            if (vlan_id)
                *vlan_id = list[middle].vlan_id;
            return 1;
        }
        if (res < 0)
            start = middle + 1;
        else
            end = middle - 1;
    }

    return 0;
}


int hostapd_rate_found(int *list, int rate)
{
    int i;

    if (list == NULL)
        return 0;

    for (i = 0; list[i] >= 0; i++)
        if (list[i] == rate)
            return 1;

    return 0;
}

const u8 * hostapd_get_psk(const struct hostapd_bss_config *conf,
			   const u8 *addr, const u8 *prev_psk)
{
    struct hostapd_wpa_psk *psk;
    int next_ok = prev_psk == NULL;

    for (psk = conf->ssid.wpa_psk; psk != NULL; psk = psk->next) {
        if (next_ok &&
            (psk->group || memcmp(psk->addr, addr, ETH_ALEN) == 0))
            return psk->psk;

        if (psk->psk == prev_psk)
            next_ok = 1;
    }

    return NULL;
}

void hostapd_config_clear_wpa_psk(struct hostapd_wpa_psk **l)
{
    struct hostapd_wpa_psk *psk, *tmp;

    for (psk = *l; psk;) {
        tmp = psk;
        psk = psk->next;
        bin_clear_free(tmp, sizeof(*tmp));
    }
    *l = NULL;
}

void hostapd_config_free_bss(struct hostapd_bss_config *conf)
{
    hostapd_config_clear_wpa_psk(&conf->ssid.wpa_psk);
#ifdef CONFIG_SAE
    sae_deinit_pt(conf->ssid.pt);
#endif /* CONFIG_SAE */
    os_free(conf);
}
