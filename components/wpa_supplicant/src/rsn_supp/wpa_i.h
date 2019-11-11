/*
 * Internal WPA/RSN supplicant state machine definitions
 * Copyright (c) 2004-2010, Jouni Malinen <j@w1.fi>
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

#ifndef WPA_I_H
#define WPA_I_H

struct install_key {
    int mic_errors_seen; /* Michael MIC errors with the current PTK */
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
    u8 pmk[PMK_LEN];
    size_t pmk_len;

    struct wpa_ptk ptk, tptk;
    int ptk_set, tptk_set;
    u8 snonce[WPA_NONCE_LEN];
    u8 anonce[WPA_NONCE_LEN]; /* ANonce from the last 1/4 msg */
    int renew_snonce;
    u8 rx_replay_counter[WPA_REPLAY_COUNTER_LEN];
    int rx_replay_counter_set;
    u8 request_counter[WPA_REPLAY_COUNTER_LEN];
    struct rsn_pmksa_cache *pmksa; /* PMKSA cache */
    struct rsn_pmksa_cache_entry *cur_pmksa; /* current PMKSA entry */

    unsigned int pairwise_cipher;
    unsigned int group_cipher;
    unsigned int key_mgmt;
    unsigned int mgmt_group_cipher;
    void *network_ctx;

    int rsn_enabled; /* Whether RSN is enabled in configuration */

    int countermeasures; /*TKIP countermeasures state flag, 1:in countermeasures state*/
    ETSTimer  cm_timer;

    u8 *assoc_wpa_ie; /* Own WPA/RSN IE from (Re)AssocReq */
    size_t assoc_wpa_ie_len;

    u8 eapol_version;

    int wpa_ptk_rekey;
    u8 own_addr[ETH_ALEN];

    u8 bssid[ETH_ALEN];

    unsigned int proto;
    enum wpa_states wpa_state;

    u8 *ap_wpa_ie, *ap_rsn_ie;
    size_t ap_wpa_ie_len, ap_rsn_ie_len;

    bool key_install;

    struct install_key install_ptk;
    struct install_key install_gtk;
    int  key_entry_valid;   //present current avaliable entry for bssid, for pairkey:0,5,10,15,20, gtk: pairkey_no+i (i:1~4)

    void (* sendto) (void *buffer, uint16_t len);
    void (*config_assoc_ie) (u8 proto, u8 *assoc_buf, u32 assoc_wpa_ie_len);
    void (*install_ppkey) (enum wpa_alg alg, u8 *addr, int key_idx, int set_tx,
               u8 *seq, unsigned int seq_len, u8 *key, unsigned int key_len, int key_entry_valid);
    int (*get_ppkey) (uint8_t *ifx, int *alg, uint8_t *addr, int *key_idx,
               uint8_t *key, size_t key_len, int key_entry_valid);
    void (*wpa_deauthenticate)(u8 reason_code);
    void (*wpa_neg_complete)(void);
    struct wpa_gtk_data gd; //used for calllback save param
    u16 key_info;       //used for txcallback param
    u16 txcb_flags;
    bool   ap_notify_completed_rsne;
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


typedef void (* WPA_SEND_FUNC)(void *buffer, u16 len);

typedef void (* WPA_SET_ASSOC_IE)(u8 proto, u8 *assoc_buf, u32 assoc_wpa_ie_len);

typedef void (*WPA_INSTALL_KEY) (enum wpa_alg alg, u8 *addr, int key_idx, int set_tx,
               u8 *seq, size_t seq_len, u8 *key, size_t key_len, int key_entry_valid);

typedef int (*WPA_GET_KEY) (u8 *ifx, int *alg, u8 *addt, int *keyidx, u8 *key, size_t key_len, int key_entry_valid);

typedef void (*WPA_DEAUTH_FUNC)(u8 reason_code);

typedef void (*WPA_NEG_COMPLETE)(void);

bool wpa_sm_init(char * payload, WPA_SEND_FUNC snd_func, \
        WPA_SET_ASSOC_IE set_assoc_ie_func, \
        WPA_INSTALL_KEY ppinstallkey, \
        WPA_GET_KEY ppgetkey, \
        WPA_DEAUTH_FUNC wpa_deauth, \
        WPA_NEG_COMPLETE wpa_neg_complete);

void wpa_sm_deinit(void);

void eapol_txcb(void *eb);

void wpa_set_profile(u32 wpa_proto, u8 auth_mode);

int wpa_set_bss(char *macddr, char * bssid, u8 pairwise_cipher, u8 group_cipher, char *passphrase, u8 *ssid, size_t ssid_len);

int wpa_sm_rx_eapol(u8 *src_addr, u8 *buf, u32 len);
#endif /* WPA_I_H */

