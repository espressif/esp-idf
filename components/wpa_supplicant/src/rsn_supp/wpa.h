/*
 * wpa_supplicant - WPA definitions
 * Copyright (c) 2003-2015, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef WPA_H
#define WPA_H

#include "sdkconfig.h"
#include "utils/common.h"
#include "utils/includes.h"
#include "common/defs.h"
#include "common/wpa_common.h"
#include "esp_wifi_types.h"
#include "esp_wifi_crypto_types.h"
#include "wpa_i.h"

struct wpa_sm;
extern struct wpa_sm gWpaSm;

#define WPA_SM_STATE(_sm) ((_sm)->wpa_state)

bool wpa_sta_is_cur_pmksa_set(void);
bool wpa_sta_in_4way_handshake(void);
bool wpa_sta_cur_pmksa_matches_akm(void);

#define WPA_ASSERT  assert

struct l2_ethhdr {
    u8 h_dest[ETH_ALEN];
    u8 h_source[ETH_ALEN];
    be16 h_proto;
} STRUCT_PACKED;


void wpa_sm_set_state(enum wpa_states state);

void wpa_set_pmk(uint8_t *pmk, size_t pmk_length, const u8 *pmkid, bool cache_pmksa);

int wpa_michael_mic_failure(u16 isunicast);

wifi_cipher_type_t cipher_type_map_supp_to_public(unsigned cipher);

unsigned cipher_type_map_public_to_supp(wifi_cipher_type_t cipher);

void wpa_sta_clear_curr_pmksa(void);

int wpa_sm_set_key(struct install_key *sm, enum wpa_alg alg,
        u8 *addr, int key_idx, int set_tx,
        u8 *seq, size_t seq_len,
        u8 *key, size_t key_len,
        enum key_flag key_flag);

int wpa_sm_set_ap_rsnxe(const u8 *ie, size_t len);

int wpa_sm_set_assoc_rsnxe(struct wpa_sm *sm, const u8 *ie, size_t len);

void wpa_sm_drop_sa(struct wpa_sm *sm);

#ifdef CONFIG_IEEE80211R

int wpa_sm_set_ft_params(struct wpa_sm *sm, const u8 *ies, size_t ies_len);
int wpa_ft_prepare_auth_request(struct wpa_sm *sm, const u8 *mdie);
int wpa_ft_add_mdie(struct wpa_sm *sm, u8 *ies, size_t ies_len,
		    const u8 *mdie);
const u8 * wpa_sm_get_ft_md(struct wpa_sm *sm);
int wpa_ft_process_response(struct wpa_sm *sm, const u8 *ies, size_t ies_len,
			    int ft_action, const u8 *target_ap,
			    const u8 *ric_ies, size_t ric_ies_len);
int wpa_ft_is_completed(struct wpa_sm *sm);
void wpa_reset_ft_completed(struct wpa_sm *sm);
int wpa_ft_validate_reassoc_resp(struct wpa_sm *sm, const u8 *ies,
				 size_t ies_len, const u8 *src_addr);
int wpa_ft_start_over_ds(struct wpa_sm *sm, const u8 *target_ap,
			 const u8 *mdie);

#else /* CONFIG_IEEE80211R */

static inline int
wpa_sm_set_ft_params(struct wpa_sm *sm, const u8 *ies, size_t ies_len)
{
	return 0;
}

static inline int wpa_ft_prepare_auth_request(struct wpa_sm *sm,
					      const u8 *mdie)
{
	return 0;
}

static inline int wpa_ft_add_mdie(struct wpa_sm *sm, u8 *ies, size_t ies_len,
				  const u8 *mdie)
{
	return 0;
}

static inline int
wpa_ft_process_response(struct wpa_sm *sm, const u8 *ies, size_t ies_len,
			int ft_action, const u8 *target_ap)
{
	return 0;
}

static inline int wpa_ft_is_completed(struct wpa_sm *sm)
{
	return 0;
}

static inline void wpa_reset_ft_completed(struct wpa_sm *sm)
{
}

static inline int
wpa_ft_validate_reassoc_resp(struct wpa_sm *sm, const u8 *ies, size_t ies_len,
			     const u8 *src_addr)
{
	return -1;
}

#endif /* CONFIG_IEEE80211R */
struct wpa_sm * get_wpa_sm(void);

void wpa_sm_set_pmk_from_pmksa(struct wpa_sm *sm);

void wpa_sm_notify_assoc(struct wpa_sm *sm, const u8 *bssid);

void wpa_sm_notify_disassoc(struct wpa_sm *sm);

int owe_process_assoc_resp(const u8 *rsn_ie, size_t rsn_len, const uint8_t *dh_ie, size_t dh_len);

struct wpabuf *owe_build_assoc_req(struct wpa_sm *sm, u16 group);

void wpa_sm_pmksa_cache_flush(struct wpa_sm *sm, void *network_ctx);
#endif /* WPA_H */
