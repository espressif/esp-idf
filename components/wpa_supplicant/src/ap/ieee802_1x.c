/*
 * hostapd / IEEE 802.1X-2004 Authenticator
 * Copyright (c) 2002-2012, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"

#include "utils/common.h"
#include "utils/eloop.h"
#include "crypto/crypto.h"
#include "common/ieee802_11_defs.h"
#include "hostapd.h"
#include "ap/sta_info.h"
#include "ap/wpa_auth.h"
#include "eap_server/eap.h"
#include "ap/ap_config.h"
#include "eap_common/eap_wsc_common.h"
#include "ap/ieee802_1x.h"
#include "utils/wpa_debug.h"
#include "eapol_auth/eapol_auth_sm.h"
#include "eapol_auth/eapol_auth_sm_i.h"
#include "eap_server/eap.h"
#include "sta_info.h"
#include "ieee802_1x.h"

int hostapd_send_eapol(const u8 *source, const u8 *sta_addr,
		       const u8 *data, size_t data_len);

static void ieee802_1x_finished(struct hostapd_data *hapd,
                                struct sta_info *sta, int success,
                                int remediation);

static void ieee802_1x_send(struct hostapd_data *hapd, struct sta_info *sta,
			    u8 type, const u8 *data, size_t datalen)
{
	u8 *buf;
	struct ieee802_1x_hdr *xhdr;
	size_t len;

	len = sizeof(*xhdr) + datalen;
	buf = os_zalloc(len);
	if (!buf) {
		wpa_printf(MSG_ERROR, "malloc() failed for %s(len=%lu)",
			   __func__, (unsigned long) len);
		return;
	}

	xhdr = (struct ieee802_1x_hdr *) buf;
	xhdr->version = EAPOL_VERSION;
	xhdr->type = type;
	xhdr->length = host_to_be16(datalen);

	if (datalen > 0 && data != NULL)
		os_memcpy(xhdr + 1, data, datalen);

	hostapd_send_eapol(hapd->own_addr, sta->addr, buf, len);
	os_free(buf);
}



static void handle_eap_response(struct hostapd_data *hapd,
				struct sta_info *sta, struct eap_hdr *eap,
				size_t len)
{
	u8 type, *data;
	struct eapol_state_machine *sm = sta->eapol_sm;

	if (!sm)
		return;

	data = (u8 *) (eap + 1);

	if (len < sizeof(*eap) + 1) {
		wpa_printf(MSG_INFO, "%s: too short response data", __func__);
		return;
	}

	sm->eap_type_supp = type = data[0];

	sm->dot1xAuthEapolRespFramesRx++;

	wpabuf_free(sm->eap_if->eapRespData);
	sm->eap_if->eapRespData = wpabuf_alloc_copy(eap, len);
	sm->eapolEap = true;
}

/* Process incoming EAP packet from Supplicant */
static void handle_eap(struct hostapd_data *hapd, struct sta_info *sta,
		       u8 *buf, size_t len)
{
	struct eap_hdr *eap;
	u16 eap_len;

	if (len < sizeof(*eap)) {
		wpa_printf(MSG_INFO, "   too short EAP packet");
		return;
	}

	eap = (struct eap_hdr *) buf;

	eap_len = be_to_host16(eap->length);
	wpa_printf(MSG_DEBUG, "EAP: code=%d identifier=%d length=%d",
		   eap->code, eap->identifier,
		   eap_len);
	if (eap_len < sizeof(*eap)) {
		wpa_printf(MSG_DEBUG, "   Invalid EAP length");
		return;
	} else if (eap_len > len) {
		wpa_printf(MSG_DEBUG,
			   "   Too short frame to contain this EAP packet");
		return;
	} else if (eap_len < len) {
		wpa_printf(MSG_DEBUG,
			   "   Ignoring %lu extra bytes after EAP packet",
			   (unsigned long) len - eap_len);
	}

	switch (eap->code) {
	case EAP_CODE_RESPONSE:
		handle_eap_response(hapd, sta, eap, eap_len);
		break;
	case EAP_CODE_INITIATE:
		break;
	}
}

struct eapol_state_machine *
ieee802_1x_alloc_eapol_sm(struct hostapd_data *hapd, struct sta_info *sta)
{
	int flags = 0;

	if (sta->wpa_sm) {
		flags |= EAPOL_SM_USES_WPA;
	}
	return eapol_auth_alloc(hapd->eapol_auth, sta->addr, flags,
			sta->wps_ie, NULL, sta,
			sta->identity, NULL);
}


/**
 * ieee802_1x_receive - Process the EAPOL frames from the Supplicant
 * @hapd: hostapd BSS data
 * @sa: Source address (sender of the EAPOL frame)
 * @buf: EAPOL frame
 * @len: Length of buf in octets
 *
 * This function is called for each incoming EAPOL frame from the interface
 */
void ieee802_1x_receive(struct hostapd_data *hapd, const u8 *sa, const u8 *buf,
			size_t len)
{
	struct sta_info *sta;
	struct ieee802_1x_hdr *hdr;
	struct ieee802_1x_eapol_key *key;
	u16 datalen;

	wpa_printf(MSG_DEBUG, "IEEE 802.1X: %lu bytes from " MACSTR,
		   (unsigned long) len, MAC2STR(sa));
	sta = ap_get_sta(hapd, sa);
	if (!sta) {
		wpa_printf(MSG_DEBUG,
			   "IEEE 802.1X data frame from not associated/Pre-authenticating STA");

		return;
	}

	if (len < sizeof(*hdr)) {
		wpa_printf(MSG_INFO, "   too short IEEE 802.1X packet");
		return;
	}

	hdr = (struct ieee802_1x_hdr *) buf;
	datalen = be_to_host16(hdr->length);
	wpa_printf(MSG_DEBUG, "   IEEE 802.1X: version=%d type=%d length=%d",
		   hdr->version, hdr->type, datalen);

	if (len - sizeof(*hdr) < datalen) {
		wpa_printf(MSG_INFO,
			   "   frame too short for this IEEE 802.1X packet");
		if (sta->eapol_sm)
			sta->eapol_sm->dot1xAuthEapLengthErrorFramesRx++;
		return;
	}
	if (len - sizeof(*hdr) > datalen) {
		wpa_printf(MSG_DEBUG,
			   "   ignoring %lu extra octets after IEEE 802.1X packet",
			   (unsigned long) len - sizeof(*hdr) - datalen);
	}

	if (sta->eapol_sm) {
		sta->eapol_sm->dot1xAuthLastEapolFrameVersion = hdr->version;
		sta->eapol_sm->dot1xAuthEapolFramesRx++;
	}

	key = (struct ieee802_1x_eapol_key *) (hdr + 1);
	if (datalen >= sizeof(struct ieee802_1x_eapol_key) &&
	    hdr->type == IEEE802_1X_TYPE_EAPOL_KEY &&
	    (key->type == EAPOL_KEY_TYPE_WPA ||
	     key->type == EAPOL_KEY_TYPE_RSN)) {
		wpa_receive(hapd->wpa_auth, sta->wpa_sm, (u8 *) hdr,
			    sizeof(*hdr) + datalen);
		return;
	}

	if (!sta->eapol_sm) {
		sta->eapol_sm = ieee802_1x_alloc_eapol_sm(hapd, sta);
		if (!sta->eapol_sm)
			return;

#ifdef CONFIG_WPS
		if (!hapd->conf->ieee802_1x && hapd->conf->wps_state) {
			u32 wflags = sta->flags & (WLAN_STA_WPS |
						   WLAN_STA_WPS2 |
						   WLAN_STA_MAYBE_WPS);
			if (wflags == WLAN_STA_MAYBE_WPS ||
			    wflags == (WLAN_STA_WPS | WLAN_STA_MAYBE_WPS)) {
				/*
				 * Delay EAPOL frame transmission until a
				 * possible WPS STA initiates the handshake
				 * with EAPOL-Start. Only allow the wait to be
				 * skipped if the STA is known to support WPS
				 * 2.0.
				 */
				wpa_printf(MSG_DEBUG,
					   "WPS: Do not start EAPOL until EAPOL-Start is received");
				sta->eapol_sm->flags |= EAPOL_SM_WAIT_START;
			}
		}
#endif /* CONFIG_WPS */

		sta->eapol_sm->eap_if->portEnabled = true;
	}

	/* since we support version 1, we can ignore version field and proceed
	 * as specified in version 1 standard [IEEE Std 802.1X-2001, 7.5.5] */
	/* TODO: actually, we are not version 1 anymore.. However, Version 2
	 * does not change frame contents, so should be ok to process frames
	 * more or less identically. Some changes might be needed for
	 * verification of fields. */

	switch (hdr->type) {
	case IEEE802_1X_TYPE_EAP_PACKET:
		handle_eap(hapd, sta, (u8 *) (hdr + 1), datalen);
		break;

	case IEEE802_1X_TYPE_EAPOL_START:
		sta->eapol_sm->flags &= ~EAPOL_SM_WAIT_START;
		sta->eapol_sm->eapolStart = true;
		sta->eapol_sm->dot1xAuthEapolStartFramesRx++;
		eap_server_clear_identity(sta->eapol_sm->eap);
		wpa_auth_sm_event(sta->wpa_sm, WPA_REAUTH_EAPOL);
		break;


	case IEEE802_1X_TYPE_EAPOL_LOGOFF:
		break;

	case IEEE802_1X_TYPE_EAPOL_KEY:
		wpa_printf(MSG_DEBUG, "   EAPOL-Key");
		break;

	case IEEE802_1X_TYPE_EAPOL_ENCAPSULATED_ASF_ALERT:
		wpa_printf(MSG_DEBUG, "   EAPOL-Encapsulated-ASF-Alert");
		/* TODO: implement support for this; show data */
		break;

	default:
		wpa_printf(MSG_DEBUG, "   unknown IEEE 802.1X packet type");
		sta->eapol_sm->dot1xAuthInvalidEapolFramesRx++;
		break;
	}

	eapol_auth_step(sta->eapol_sm);
}


void ieee802_1x_free_station(struct hostapd_data *hapd, struct sta_info *sta)
{
	struct eapol_state_machine *sm = sta->eapol_sm;

	if (!sm)
		return;

	sta->eapol_sm = NULL;
	eapol_auth_free(sm);
}


static void ieee802_1x_eapol_send(void *ctx, void *sta_ctx, u8 type,
				  const u8 *data, size_t datalen)
{
#ifdef CONFIG_WPS
	struct sta_info *sta = sta_ctx;

	if ((sta->flags & (WLAN_STA_WPS | WLAN_STA_MAYBE_WPS)) ==
	    WLAN_STA_MAYBE_WPS) {
		const u8 *identity;
		size_t identity_len;
		struct eapol_state_machine *sm = sta->eapol_sm;

		identity = eap_get_identity(sm->eap, &identity_len);
		if (identity &&
		    ((identity_len == WSC_ID_ENROLLEE_LEN &&
		      os_memcmp(identity, WSC_ID_ENROLLEE,
				WSC_ID_ENROLLEE_LEN) == 0) ||
		     (identity_len == WSC_ID_REGISTRAR_LEN &&
		      os_memcmp(identity, WSC_ID_REGISTRAR,
				WSC_ID_REGISTRAR_LEN) == 0))) {
			wpa_printf(MSG_DEBUG,
				   "WPS: WLAN_STA_MAYBE_WPS -> WLAN_STA_WPS");
			sta->flags |= WLAN_STA_WPS;
		}
	}
#endif /* CONFIG_WPS */

	ieee802_1x_send(ctx, sta_ctx, type, data, datalen);
}


static void ieee802_1x_aaa_send(void *ctx, void *sta_ctx,
				const u8 *data, size_t datalen)
{
#ifndef CONFIG_NO_RADIUS
	struct hostapd_data *hapd = ctx;
	struct sta_info *sta = sta_ctx;

	ieee802_1x_encapsulate_radius(hapd, sta, data, datalen);
#endif /* CONFIG_NO_RADIUS */
}


static void _ieee802_1x_finished(void *ctx, void *sta_ctx, int success,
				 int preauth, int remediation)
{
	struct hostapd_data *hapd = ctx;
	struct sta_info *sta = sta_ctx;
	ieee802_1x_finished(hapd, sta, success, remediation);
}


static int ieee802_1x_get_eap_user(void *ctx, const u8 *identity,
				   size_t identity_len, int phase2,
				   struct eap_user *user)
{
	struct hostapd_data *hapd = ctx;
	const struct hostapd_eap_user *eap_user;
	int i;
	int rv = -1;

	eap_user = hostapd_get_eap_user(hapd, identity, identity_len, phase2);
	if (!eap_user)
		goto out;

	os_memset(user, 0, sizeof(*user));
	user->phase2 = phase2;
	for (i = 0; i < EAP_MAX_METHODS; i++) {
		user->methods[i].vendor = eap_user->methods[i].vendor;
		user->methods[i].method = eap_user->methods[i].method;
	}

	if (eap_user->password) {
		user->password = os_memdup(eap_user->password,
					   eap_user->password_len);
		if (!user->password)
			goto out;
		user->password_len = eap_user->password_len;
		user->password_hash = eap_user->password_hash;
	}
	user->force_version = eap_user->force_version;
	user->ttls_auth = eap_user->ttls_auth;
	rv = 0;

out:
	if (rv)
		wpa_printf(MSG_DEBUG, "%s: Failed to find user", __func__);

	return rv;
}


static int ieee802_1x_sta_entry_alive(void *ctx, const u8 *addr)
{
	struct hostapd_data *hapd = ctx;
	struct sta_info *sta;

	sta = ap_get_sta(hapd, addr);
	if (!sta || !sta->eapol_sm)
		return 0;
	return 1;
}


static void ieee802_1x_set_port_authorized(void *ctx, void *sta_ctx,
					   int authorized)
{
}


static void _ieee802_1x_abort_auth(void *ctx, void *sta_ctx)
{
}


static void ieee802_1x_eapol_event(void *ctx, void *sta_ctx,
				   enum eapol_event type)
{
#if 0
	/* struct hostapd_data *hapd = ctx; */
	struct sta_info *sta = sta_ctx;

	switch (type) {
	case EAPOL_AUTH_SM_CHANGE:
		wpa_auth_sm_notify(sta->wpa_sm);
		break;
	case EAPOL_AUTH_REAUTHENTICATE:
		wpa_auth_sm_event(sta->wpa_sm, WPA_REAUTH_EAPOL);
		break;
	}
#endif
}


int ieee802_1x_init(struct hostapd_data *hapd)
{
	struct eapol_auth_config conf;
	struct eapol_auth_cb cb;
	struct eap_config *eap_cfg;

	os_memset(&conf, 0, sizeof(conf));
	eap_cfg = os_zalloc(sizeof(struct eap_config));
	if (!eap_cfg)
		return -1;
	eap_cfg->max_auth_rounds = 100;
	eap_cfg->max_auth_rounds_short = 50;
	//eap_cfg->backend_auth = 1;
	eap_cfg->eap_server = 1;
	conf.eap_cfg = eap_cfg;
	conf.ctx = hapd;
	conf.wpa = hapd->conf->wpa;

	os_memset(&cb, 0, sizeof(cb));
	cb.eapol_send = ieee802_1x_eapol_send;
	cb.aaa_send = ieee802_1x_aaa_send;
	cb.finished = _ieee802_1x_finished;
	cb.get_eap_user = ieee802_1x_get_eap_user;
	cb.sta_entry_alive = ieee802_1x_sta_entry_alive;
	cb.set_port_authorized = ieee802_1x_set_port_authorized;
	cb.abort_auth = _ieee802_1x_abort_auth;
	cb.eapol_event = ieee802_1x_eapol_event;

	hapd->eapol_auth = eapol_auth_init(&conf, &cb);
	if (!hapd->eapol_auth)
		return -1;

	return 0;
}


static void ieee802_1x_finished(struct hostapd_data *hapd,
				struct sta_info *sta, int success,
				int remediation)
{
	if (!success) {
		/*
		 * Many devices require deauthentication after WPS provisioning
		 * and some may not be be able to do that themselves, so
		 * disconnect the client here. In addition, this may also
		 * benefit IEEE 802.1X/EAPOL authentication cases, too since
		 * the EAPOL PAE state machine would remain in HELD state for
		 * considerable amount of time and some EAP methods, like
		 * EAP-FAST with anonymous provisioning, may require another
		 * EAPOL authentication to be started to complete connection.
		 */
		ap_sta_delayed_1x_auth_fail_disconnect(hapd, sta);
	}
}
