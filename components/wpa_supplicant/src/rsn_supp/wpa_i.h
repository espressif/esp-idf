/*
 * Internal WPA/RSN supplicant state machine definitions
 * Copyright (c) 2004-2018, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef WPA_I_H
#define WPA_I_H

extern struct wpa_sm gWpaSm;
#define DEFAULT_EAPOL_VERSION 1

struct install_key {
    int keys_cleared;
    enum wpa_alg alg;
    u8 addr[ETH_ALEN];
    int key_idx;
    int set_tx;
    u8 seq[10];
    u8 key[32];
};

/**
 * struct wpa_sm - Internal WPA state machine data
 */
struct wpa_sm {
    u8 pmk[PMK_LEN_MAX];
    size_t pmk_len;
    struct wpa_ptk ptk, tptk;
    int ptk_set, tptk_set;
    u8 snonce[WPA_NONCE_LEN];
    u8 anonce[WPA_NONCE_LEN]; /* ANonce from the last 1/4 msg */
    int renew_snonce;
    u8 rx_replay_counter[WPA_REPLAY_COUNTER_LEN];
    int rx_replay_counter_set;
    u8 request_counter[WPA_REPLAY_COUNTER_LEN];
    struct wpa_gtk gtk;
#ifdef CONFIG_IEEE80211W
    struct wpa_igtk igtk;
#endif /* CONFIG_IEEE80211W */

    struct rsn_pmksa_cache *pmksa; /* PMKSA cache */
    struct rsn_pmksa_cache_entry *cur_pmksa; /* current PMKSA entry */
    u8 ssid[32];
    size_t ssid_len;

    unsigned int pairwise_cipher;
    unsigned int group_cipher;
    unsigned int key_mgmt;
    unsigned int mgmt_group_cipher;
    void *network_ctx;

    int rsn_enabled; /* Whether RSN is enabled in configuration */
    enum sae_pwe sae_pwe; /* SAE PWE generation options */

    bool sae_pk; /* whether SAE-PK is used */

    int countermeasures; /*TKIP countermeasures state flag, 1:in countermeasures state*/

    u8 *assoc_wpa_ie; /* Own WPA/RSN IE from (Re)AssocReq */
    size_t assoc_wpa_ie_len;
    u8 *assoc_rsnxe; /* Own RSNXE from (Re)AssocReq */
    size_t assoc_rsnxe_len;

    u8 eapol_version;

    int wpa_ptk_rekey;
    u8 own_addr[ETH_ALEN];

    u8 bssid[ETH_ALEN];

    unsigned int proto;
    enum wpa_states wpa_state;

    u8 *ap_wpa_ie, *ap_rsn_ie, *ap_rsnxe;
    size_t ap_wpa_ie_len, ap_rsn_ie_len, ap_rsnxe_len;

    bool key_install;

    struct install_key install_ptk;
    struct install_key install_gtk;
    int mic_errors_seen; /* Michael MIC errors with the current PTK */
    int use_ext_key_id; /* Enabled only for WPA PSK first key exchange */
    struct wpa_gtk_data gd; //used for calllback save param
    u16 key_info;       //used for txcallback param
    u16 txcb_flags;
    bool   ap_notify_completed_rsne;
    wifi_pmf_config_t pmf_cfg;
    u8 eapol1_count;
    struct rsn_sppamsdu_sup spp_sup;
#ifdef CONFIG_IEEE80211R
    u8 xxkey[PMK_LEN]; /* PSK or the second 256 bits of MSK */
    size_t xxkey_len;
    u8 pmk_r0[PMK_LEN];
    u8 pmk_r0_name[WPA_PMK_NAME_LEN];
    u8 pmk_r1[PMK_LEN];
    u8 pmk_r1_name[WPA_PMK_NAME_LEN];
    u8 mobility_domain[MOBILITY_DOMAIN_ID_LEN];
    u8 r0kh_id[FT_R0KH_ID_MAX_LEN];
    size_t r0kh_id_len;
    u8 r1kh_id[FT_R1KH_ID_LEN];
    unsigned int ft_completed:1;
    unsigned int ft_reassoc_completed:1;
    unsigned int ft_protocol:1;
    int over_the_ds_in_progress;
    u8 target_ap[ETH_ALEN]; /* over-the-DS target AP */
    int set_ptk_after_assoc;
    u8 mdie_ft_capab; /* FT Capability and Policy from target AP MDIE */
    u8 *assoc_resp_ies; /* MDIE and FTIE from (Re)Association Response */
    size_t assoc_resp_ies_len;
#endif /* CONFIG_IEEE80211R */
#ifdef CONFIG_OWE_STA
    struct crypto_ecdh *owe_ecdh;
    u16 owe_group;
    struct wpabuf *owe_ie;
#endif /* CONFIG_OWE_STA */
    int (*wpa_sm_wps_disable)(void);
    esp_err_t (*wpa_sm_eap_disable)(void);
};

/**
 * set_key - Configure encryption key
 * @ifname: Interface name (for multi-SSID/VLAN support)
 * @priv: private driver interface data
 * @alg: encryption algorithm (%WPA_ALG_NONE, %WPA_ALG_WEP,
 *	%WPA_ALG_TKIP, %WPA_ALG_CCMP, %WPA_ALG_IGTK, %WPA_ALG_PMK);
 *	%WPA_ALG_NONE clears the key.
 * @addr: address of the peer STA or ff:ff:ff:ff:ff:ff for
 *	broadcast/default keys
 * @key_idx: key index (0..3), usually 0 for unicast keys; 0..4095 for
 *	IGTK
 * @set_tx: configure this key as the default Tx key (only used when
 *	driver does not support separate unicast/individual key
 * @seq: sequence number/packet number, seq_len octets, the next
 *	packet number to be used for in replay protection; configured
 *	for Rx keys (in most cases, this is only used with broadcast
 *	keys and set to zero for unicast keys)
 * @seq_len: length of the seq, depends on the algorithm:
 *	TKIP: 6 octets, CCMP: 6 octets, IGTK: 6 octets
 * @key: key buffer; TKIP: 16-byte temporal key, 8-byte Tx Mic key,
 *	8-byte Rx Mic Key
 * @key_len: length of the key buffer in octets (WEP: 5 or 13,
 *	TKIP: 32, CCMP: 16, IGTK: 16)
 *
 * Returns: 0 on success, -1 on failure
 *
 * Configure the given key for the kernel driver. If the driver
 * supports separate individual keys (4 default keys + 1 individual),
 * addr can be used to determine whether the key is default or
 * individual. If only 4 keys are supported, the default key with key
 * index 0 is used as the individual key. STA must be configured to use
 * it as the default Tx key (set_tx is set) and accept Rx for all the
 * key indexes. In most cases, WPA uses only key indexes 1 and 2 for
 * broadcast keys, so key index 0 is available for this kind of
 * configuration.
 *
 * Please note that TKIP keys include separate TX and RX MIC keys and
 * some drivers may expect them in different order than wpa_supplicant
 * is using. If the TX/RX keys are swapped, all TKIP encrypted packets
 * will tricker Michael MIC errors. This can be fixed by changing the
 * order of MIC keys by swapping te bytes 16..23 and 24..31 of the key
 * in driver_*.c set_key() implementation, see driver_ndis.c for an
 * example on how this can be done.
 */


int wpa_sm_update_ft_ies(struct wpa_sm *sm, const u8 *md,
			 const u8 *ies, size_t ies_len, bool auth_ie);

static inline int wpa_sm_send_ft_action(struct wpa_sm *sm, u8 action,
					const u8 *target_ap,
					const u8 *ies, size_t ies_len)
{
	return -1;
}

static inline int wpa_sm_mark_authenticated(struct wpa_sm *sm,
					    const u8 *target_ap)
{
	return 0;
}

void  wpa_config_assoc_ie(u8 proto, u8 *assoc_buf, u32 assoc_wpa_ie_len);

void  wpa_install_key(enum wpa_alg alg, u8 *addr, int key_idx, int set_tx,
                      u8 *seq, size_t seq_len, u8 *key, size_t key_len, enum key_flag key_flag);

int  wpa_get_key(uint8_t *ifx, int *alg, u8 *addr, int *key_idx,
                 u8 *key, size_t key_len, enum key_flag key_flag);

void  wpa_deauthenticate(u8 reason_code);

void  wpa_neg_complete(void);


bool wpa_sm_init(void);

void wpa_sm_deinit(void);

void eapol_txcb(uint8_t *eapol_payload, size_t len, bool tx_failure);

void wpa_set_profile(u32 wpa_proto, u8 auth_mode);

int wpa_set_bss(char *macddr, char * bssid, u8 pairwise_cipher, u8 group_cipher, char *passphrase, u8 *ssid, size_t ssid_len);

int wpa_sm_rx_eapol(u8 *src_addr, u8 *buf, u32 len);

int wpa_derive_ptk_ft(struct wpa_sm *sm, const unsigned char *src_addr,
                      const struct wpa_eapol_key *key, struct wpa_ptk *ptk);
#endif /* WPA_I_H */
