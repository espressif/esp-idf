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

#include "sdkconfig.h"

#ifdef CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/ets_sys.h"
#else
#include "esp32s2beta/rom/ets_sys.h"
#endif
#include "utils/common.h"
#include "common/defs.h"
#include "common/wpa_common.h"
#include "esp_wifi_crypto_types.h"
#include "wpa_i.h"


#define WPA_SM_STATE(_sm) ((_sm)->wpa_state)

struct wpa_sm;

int wpa_sm_rx_eapol(u8 *src_addr, u8 *buf, u32 len);
bool wpa_sta_is_cur_pmksa_set(void);
bool wpa_sta_in_4way_handshake(void);

#define WPA_ASSERT  assert

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

void pp_michael_mic_failure(u16 isunicast);

void wpa_sm_set_state(enum wpa_states state);

char * dup_binstr(const void *src, size_t len);

void wpa_set_pmk(uint8_t *pmk);

int wpa_hook_init(void);

bool wpa_hook_deinit(void);

char * dup_binstr(const void *src, size_t len);

int wpa_michael_mic_failure(u16 isunicast);

#endif /* WPA_H */

