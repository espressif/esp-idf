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

/*
 * CAUTION: cb is invoked while HOSTAPD_STA_LIST_LOCK is held.
 * The mutex is non-recursive (os_mutex_create), so cb must NEVER
 * call ap_get_sta, ap_free_sta, ap_sta_add, or any other function
 * that acquires HOSTAPD_STA_LIST_LOCK — doing so will deadlock.
 * Use the lock-free variants (e.g. ap_get_sta_internal) if needed.
 */
int ap_for_each_sta(struct hostapd_data *hapd,
		    int (*cb)(struct hostapd_data *hapd, struct sta_info *sta,
			      void *ctx),
		    void *ctx)
{
	struct sta_info *sta;
	int ret;

	HOSTAPD_STA_LIST_LOCK(hapd);
	for (sta = hapd->sta_list; sta; sta = sta->next) {
		if (cb(hapd, sta, ctx)) {
			ret = 1;
			HOSTAPD_STA_LIST_UNLOCK(hapd);
			return ret;
		}
	}
	HOSTAPD_STA_LIST_UNLOCK(hapd);

	return 0;
}


/**
 * ap_get_sta - Look up a station by MAC address
 * @hapd: hostapd data structure
 * @sta: MAC address to look up
 * Returns: Pointer to sta_info structure, or NULL if not found
 *
 * This function acquires and releases HOSTAPD_STA_LIST_LOCK internally.
 * The returned pointer is NOT protected after the lock is released.
 * Use-after-free can occur if the station is freed by another task
 * (via ap_free_sta) between the unlock here and the caller's use.
 *
 * For safe access to sta fields, callers should either:
 * - Use ap_get_sta_internal() while holding HOSTAPD_STA_LIST_LOCK, OR
 * - Take sta->lock immediately after (for SAE stations), OR
 * - Copy needed data while lock is still held
 *
 * Note: This is particularly important for wpa3_task callers.
 */
struct sta_info * ap_get_sta(struct hostapd_data *hapd, const u8 *sta)
{
	struct sta_info *s;

	HOSTAPD_STA_LIST_LOCK(hapd);
	s = hapd->sta_hash[STA_HASH(sta)];
	while (s != NULL && os_memcmp(s->addr, sta, 6) != 0)
		s = s->hnext;
	HOSTAPD_STA_LIST_UNLOCK(hapd);
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

/* Caller MUST hold HOSTAPD_STA_LIST_LOCK (see sta_info.h). */
struct sta_info * ap_get_sta_internal(struct hostapd_data *hapd, const u8 *sta)
{
	struct sta_info *s;

	s = hapd->sta_hash[STA_HASH(sta)];
	while (s != NULL && os_memcmp(s->addr, sta, 6) != 0)
		s = s->hnext;
	return s;
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
	HOSTAPD_STA_LIST_LOCK(hapd);
	ap_sta_hash_del(hapd, sta);
	ap_sta_list_del(hapd, sta);

	hapd->num_sta--;
	HOSTAPD_STA_LIST_UNLOCK(hapd);

#ifdef CONFIG_SAE
	sae_clear_data(sta->sae);
	os_free(sta->sae);
	if (sta->lock) {
		os_semphr_give(sta->lock);
		os_mutex_delete(sta->lock);
		sta->lock = NULL;
	}
#ifdef ESP_SUPPLICANT
	if (sta->sae_data) {
		wpabuf_free(sta->sae_data);
		sta->sae_data = NULL;
	}
#endif /* ESP_SUPPLICANT */
#endif /* CONFIG_SAE */
	wpa_auth_sta_deinit(sta->wpa_sm);
#ifdef CONFIG_WPS_REGISTRAR
	if (ap_sta_pending_delayed_1x_auth_fail_disconnect(hapd, sta))
		eloop_cancel_timeout(ap_sta_delayed_1x_auth_fail_cb, hapd, sta);

	ieee802_1x_free_station(hapd, sta);

	wpabuf_free(sta->wps_ie);
#endif
#ifdef CONFIG_OWE_SOFTAP
       bin_clear_free(sta->owe_pmk, PMK_LEN);
       crypto_ecdh_deinit(sta->owe_ecdh);
#endif /* CONFIG_OWE_SOFTAP */

	os_free(sta);
}


/* Called during teardown after WPA3 task is stopped, so sta->lock is always available. */
void hostapd_free_stas(struct hostapd_data *hapd)
{
	struct sta_info *sta;

	while (1) {
		HOSTAPD_STA_LIST_LOCK(hapd);
		sta = hapd->sta_list;
		if (!sta) {
			HOSTAPD_STA_LIST_UNLOCK(hapd);
			break;
		}
#ifdef CONFIG_SAE
		if (sta->lock) {
			os_semphr_take(sta->lock, OS_BLOCK);
			HOSTAPD_STA_LIST_UNLOCK(hapd);
			ap_free_sta(hapd, sta);
			continue;
		}
#endif /* CONFIG_SAE */
		HOSTAPD_STA_LIST_UNLOCK(hapd);
		ap_free_sta(hapd, sta);
	}
}


struct sta_info * ap_sta_add(struct hostapd_data *hapd, const u8 *addr)
{
	struct sta_info *sta;

	HOSTAPD_STA_LIST_LOCK(hapd);
	sta = ap_get_sta_internal(hapd, addr);
	if (sta) {
		HOSTAPD_STA_LIST_UNLOCK(hapd);
		return sta;
	}

	wpa_printf(MSG_DEBUG, "  New STA");
	if (hapd->num_sta >= hapd->conf->max_num_sta) {
		wpa_printf(MSG_DEBUG, "no more room for new STAs (%d/%d)",
			   hapd->num_sta, hapd->conf->max_num_sta);
		HOSTAPD_STA_LIST_UNLOCK(hapd);
		return NULL;
	}

	sta = os_zalloc(sizeof(struct sta_info));
	if (sta == NULL) {
		wpa_printf(MSG_ERROR, "malloc failed");
		HOSTAPD_STA_LIST_UNLOCK(hapd);
		return NULL;
	}

	/* initialize STA info data */
	os_memcpy(sta->addr, addr, ETH_ALEN);
#ifdef CONFIG_SAE
	atomic_init(&sta->sae_commit_processing, false);
	atomic_init(&sta->remove_pending, false);
	sta->lock = os_semphr_create(1, 1);
	if (!sta->lock) {
		wpa_printf(MSG_ERROR, "Failed to create sta->lock for " MACSTR,
			   MAC2STR(addr));
		HOSTAPD_STA_LIST_UNLOCK(hapd);
		os_free(sta);
		return NULL;
	}
#endif /* CONFIG_SAE */
	sta->next = hapd->sta_list;
	hapd->sta_list = sta;
	hapd->num_sta++;
	ap_sta_hash_add(hapd, sta);
	HOSTAPD_STA_LIST_UNLOCK(hapd);

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
