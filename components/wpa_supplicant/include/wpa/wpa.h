/*
 * wpa_supplicant - WPA definitions
 * Copyright (c) 2003-2007, Jouni Malinen <j@w1.fi>
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

#ifndef WPA_H
#define WPA_H

#include "c_types.h"
#include "os_type.h"
#include "common.h"
#include "ets_sys.h"
#include "wpa/defs.h"
#include "wpa/wpa_common.h"

//#include "net80211/ieee80211_var.h"
//#include "net80211/ieee80211_node.h"

#define WPA_SM_STATE(_sm) ((_sm)->wpa_state)

struct wpa_sm;

int wpa_sm_rx_eapol(u8 *src_addr, u8 *buf, u32 len);

#define WPA_ASSERT  ASSERT

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

//    char *passphrase;  //wlan password
//    u8 *ssid;               //wlan network name
//    size_t ssid_len;

    struct wpa_ptk ptk, tptk;
    int ptk_set, tptk_set;
    u8 snonce[WPA_NONCE_LEN];
    u8 anonce[WPA_NONCE_LEN]; /* ANonce from the last 1/4 msg */
    int renew_snonce;
    u8 rx_replay_counter[WPA_REPLAY_COUNTER_LEN];
    int rx_replay_counter_set;
    u8 request_counter[WPA_REPLAY_COUNTER_LEN];

//    void *network_ctx;

    unsigned int pairwise_cipher;
    unsigned int group_cipher;
    unsigned int key_mgmt;
    unsigned int mgmt_group_cipher;

    int rsn_enabled; /* Whether RSN is enabled in configuration */

    int countermeasures; /*TKIP countermeasures state flag, 1:in countermeasures state*/
    os_timer_t  cm_timer;

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

    struct install_key install_ptk;
    struct install_key install_gtk;
    int  key_entry_valid;   //present current avaliable entry for bssid, for pairkey:0,5,10,15,20, gtk: pairkey_no+i (i:1~4)
	
//    char *msg;   //send eapol msg buff
//    size_t msg_len;  //msg length:6 + sizeof(eth) + data_len

//    struct netif *ifp;
    struct pbuf *pb;

    void (* sendto) (struct pbuf *pb);
    void (*config_assoc_ie) (uint8 proto, u8 *assoc_buf, u32 assoc_wpa_ie_len);
    void (*install_ppkey) (enum wpa_alg alg, uint8 *addr, int key_idx, int set_tx,
               uint8 *seq, size_t seq_len, uint8 *key, size_t key_len, int key_entry_valid);
    void (*wpa_deauthenticate)(uint8 reason_code);
    void (*wpa_neg_complete)();
    struct wpa_gtk_data gd; //used for calllback save param
    uint16 key_info; 	//used for txcallback param    
};

struct l2_ethhdr {
    u8 h_dest[ETH_ALEN];
    u8 h_source[ETH_ALEN];
    be16 h_proto;
} STRUCT_PACKED;

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


/**
 * send_eapol - Optional function for sending EAPOL packets
 * @priv: private driver interface data
 * @dest: Destination MAC address
 * @proto: Ethertype
 * @data: EAPOL packet starting with IEEE 802.1X header
 * @data_len: Size of the EAPOL packet
 *
 * Returns: 0 on success, -1 on failure
 *
 * This optional function can be used to override l2_packet operations
 * with driver specific functionality. If this function pointer is set,
 * l2_packet module is not used at all and the driver interface code is
 * responsible for receiving and sending all EAPOL packets. The
 * received EAPOL packets are sent to core code with EVENT_EAPOL_RX
 * event. The driver interface is required to implement get_mac_addr()
 * handler if send_eapol() is used.
 */

#define KEYENTRY_TABLE_MAP(key_entry_valid)  ((key_entry_valid)%5) 

void pp_michael_mic_failure(uint16 isunicast);

void wpa_sm_set_state(enum wpa_states state);

#endif /* WPA_H */

