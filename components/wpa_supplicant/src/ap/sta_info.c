/*
 * hostapd / Station table
 * Copyright (c) 2002-2017, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"

#include "utils/common.h"
#include "utils/eloop.h"
#include "common/ieee802_11_defs.h"
#include "common/sae.h"
#include "crypto/crypto.h"
#include "hostapd.h"
#include "ieee802_1x.h"
#include "wpa_auth.h"
#include "ap_config.h"
#include "sta_info.h"
#include "esp_wps_i.h"

static void ap_sta_delayed_1x_auth_fail_cb(void *eloop_ctx, void *timeout_ctx);
void hostapd_wps_eap_completed(struct hostapd_data *hapd);

int ap_for_each_sta(struct hostapd_data *hapd,
		    int (*cb)(struct hostapd_data *hapd, struct sta_info *sta,
			      void *ctx),
		    void *ctx)
{
	struct sta_info *sta;

	for (sta = hapd->sta_list; sta; sta = sta->next) {
		if (cb(hapd, sta, ctx))
			return 1;
	}

	return 0;
}


struct sta_info * ap_get_sta(struct hostapd_data *hapd, const u8 *sta)
{
	struct sta_info *s;

	s = hapd->sta_hash[STA_HASH(sta)];
	while (s != NULL && os_memcmp(s->addr, sta, 6) != 0)
		s = s->hnext;
	return s;
}


static void ap_sta_list_del(struct hostapd_data *hapd, struct sta_info *sta)
{
	struct sta_info *tmp;

	if (hapd->sta_list == sta) {
		hapd->sta_list = sta->next;
		return;
	}

	tmp = hapd->sta_list;
	while (tmp != NULL && tmp->next != sta)
		tmp = tmp->next;
	if (tmp == NULL) {
		wpa_printf(MSG_DEBUG, "Could not remove STA " MACSTR " from "
			   "list.", MAC2STR(sta->addr));
	} else
		tmp->next = sta->next;
}


void ap_sta_hash_add(struct hostapd_data *hapd, struct sta_info *sta)
{
	sta->hnext = hapd->sta_hash[STA_HASH(sta->addr)];
	hapd->sta_hash[STA_HASH(sta->addr)] = sta;
}


static void ap_sta_hash_del(struct hostapd_data *hapd, struct sta_info *sta)
{
	struct sta_info *s;

	s = hapd->sta_hash[STA_HASH(sta->addr)];
	if (s == NULL) return;
	if (os_memcmp(s->addr, sta->addr, 6) == 0) {
		hapd->sta_hash[STA_HASH(sta->addr)] = s->hnext;
		return;
	}

	while (s->hnext != NULL &&
	       os_memcmp(s->hnext->addr, sta->addr, ETH_ALEN) != 0)
		s = s->hnext;
	if (s->hnext != NULL)
		s->hnext = s->hnext->hnext;
	else
		wpa_printf(MSG_DEBUG, "AP: could not remove STA " MACSTR
			   " from hash table", MAC2STR(sta->addr));
}

void ap_free_sta(struct hostapd_data *hapd, struct sta_info *sta)
{
	ap_sta_hash_del(hapd, sta);
	ap_sta_list_del(hapd, sta);

	hapd->num_sta--;

#ifdef CONFIG_SAE
	sae_clear_data(sta->sae);
	os_free(sta->sae);
	if (sta->lock) {
		os_semphr_give(sta->lock);
		os_mutex_delete(sta->lock);
		sta->lock = NULL;
	}
#endif /* CONFIG_SAE */
	wpa_auth_sta_deinit(sta->wpa_sm);
#ifdef CONFIG_WPS_REGISTRAR
	if (ap_sta_pending_delayed_1x_auth_fail_disconnect(hapd, sta))
		eloop_cancel_timeout(ap_sta_delayed_1x_auth_fail_cb, hapd, sta);

	ieee802_1x_free_station(hapd, sta);

	wpabuf_free(sta->wps_ie);
#endif

	os_free(sta);
}


void hostapd_free_stas(struct hostapd_data *hapd)
{
	struct sta_info *sta, *prev;

	sta = hapd->sta_list;

	while (sta) {
		prev = sta;
		sta = sta->next;
		wpa_printf(MSG_DEBUG, "Removing station " MACSTR,
			   MAC2STR(prev->addr));
		ap_free_sta(hapd, prev);
	}
}


struct sta_info * ap_sta_add(struct hostapd_data *hapd, const u8 *addr)
{
	struct sta_info *sta;

	sta = ap_get_sta(hapd, addr);
	if (sta)
		return sta;

	wpa_printf(MSG_DEBUG, "  New STA");
	if (hapd->num_sta >= hapd->conf->max_num_sta) {
		/* FIX: might try to remove some old STAs first? */
		wpa_printf(MSG_DEBUG, "no more room for new STAs (%d/%d)",
			   hapd->num_sta, hapd->conf->max_num_sta);
		return NULL;
	}

	sta = os_zalloc(sizeof(struct sta_info));
	if (sta == NULL) {
		wpa_printf(MSG_ERROR, "malloc failed");
		return NULL;
	}

	/* initialize STA info data */
	os_memcpy(sta->addr, addr, ETH_ALEN);
	sta->next = hapd->sta_list;
	hapd->sta_list = sta;
	hapd->num_sta++;
	ap_sta_hash_add(hapd, sta);
#ifdef CONFIG_SAE
	sta->sae_commit_processing = false;
	sta->remove_pending = false;
#endif /* CONFIG_SAE */

	return sta;
}

static void ap_sta_delayed_1x_auth_fail_cb(void *eloop_ctx, void *timeout_ctx)
{
	struct sta_info *sta = timeout_ctx;
	u16 reason;

	wpa_dbg(hapd->msg_ctx, MSG_DEBUG,
		"IEEE 802.1X: Scheduled disconnection of " MACSTR
		" after EAP-Failure", MAC2STR(sta->addr));

	reason = WLAN_REASON_IEEE_802_1X_AUTH_FAILED;
	esp_wifi_ap_deauth_internal(sta->addr, reason);
}


void ap_sta_delayed_1x_auth_fail_disconnect(struct hostapd_data *hapd,
					    struct sta_info *sta)
{
	wpa_dbg(hapd->msg_ctx, MSG_DEBUG,
		"IEEE 802.1X: Force disconnection of " MACSTR
		" after EAP-Failure in 10 ms", MAC2STR(sta->addr));

	/*
	 * Add a small sleep to increase likelihood of previously requested
	 * EAP-Failure TX getting out before this should the driver reorder
	 * operations.
	 */
	eloop_cancel_timeout(ap_sta_delayed_1x_auth_fail_cb, hapd, sta);
	eloop_register_timeout(0, 10000, ap_sta_delayed_1x_auth_fail_cb,
			       hapd, sta);
	if (wps_get_status() == WPS_STATUS_PENDING)
		wps_set_status(WPS_STATUS_SUCCESS);
}


int ap_sta_pending_delayed_1x_auth_fail_disconnect(struct hostapd_data *hapd,
						   struct sta_info *sta)
{
	return eloop_is_timeout_registered(ap_sta_delayed_1x_auth_fail_cb,
					   hapd, sta);
}
