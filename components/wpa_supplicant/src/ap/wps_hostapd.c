/*
 * hostapd / WPS integration
 * Copyright (c) 2008-2016, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"

#include "utils/common.h"
#include "utils/eloop.h"
#include "utils/uuid.h"
#include "common/wpa_ctrl.h"
#include "common/ieee802_11_defs.h"
#include "common/ieee802_11_common.h"
#include "wps/wps.h"
#include "wps/wps_defs.h"
#include "wps/wps_dev_attr.h"
#include "wps/wps_attr_parse.h"
#include "hostapd.h"
#include "ap_config.h"
#include "wpa_auth.h"
#include "wpa_auth_i.h"
#include "sta_info.h"
#include "wps/wps_i.h"
#include "wps_hostapd.h"
#include "eap_server/eap_methods.h"
#include "ieee802_1x.h"
#include "esp_wps_i.h"

static void hostapd_wps_ap_pin_timeout(void *eloop_data, void *user_ctx);

static int hostapd_wps_set_ie_cb(void *ctx, struct wpabuf *beacon_ie,
				 struct wpabuf *probe_resp_ie)
{
	int ret;

	wpa_printf(MSG_DEBUG, "WPS: Updating beacon IEs(%d) to driver", beacon_ie->used);
	ret = esp_wifi_set_appie_internal(WIFI_APPIE_RAM_BEACON,
			(uint8_t *)wpabuf_head(beacon_ie), beacon_ie->used, 0);
	if (ret != ESP_OK) {
		wpa_printf(MSG_ERROR, "WPS: Failed to update beacon IEs");
		goto cleanup;
	}
	wpa_printf(MSG_DEBUG, "WPS: Updating probe IEs(%d) to driver", probe_resp_ie->used);
	ret = esp_wifi_set_appie_internal(WIFI_APPIE_RAM_PROBE_RSP,
			(uint8_t *)wpabuf_head(probe_resp_ie), probe_resp_ie->used, 0);

cleanup:
	wpabuf_free(beacon_ie);
	wpabuf_free(probe_resp_ie);

	return ret;
}

struct wps_stop_reg_data {
	struct hostapd_data *current_hapd;
	const u8 *uuid_e;
	const u8 *dev_pw;
	size_t dev_pw_len;
};

static int wps_stop_registrar(struct hostapd_data *hapd, void *ctx)
{
	struct wps_stop_reg_data *data = ctx;
	if (hapd != data->current_hapd && hapd->wps != NULL)
		wps_registrar_complete(hapd->wps->registrar, data->uuid_e,
				       data->dev_pw, data->dev_pw_len);
	return 0;
}

void hostapd_wps_eap_completed(struct hostapd_data *hapd)
{
}

static void hostapd_wps_reg_success_cb(void *ctx, const u8 *mac_addr,
				       const u8 *uuid_e, const u8 *dev_pw,
				       size_t dev_pw_len)
{
	struct hostapd_data *hapd = ctx;
	char uuid[40];
	struct wps_stop_reg_data data;
	if (uuid_bin2str(uuid_e, uuid, sizeof(uuid)))
		return;
	wpa_msg(hapd->msg_ctx, MSG_INFO, WPS_EVENT_REG_SUCCESS MACSTR " %s",
		MAC2STR(mac_addr), uuid);
	data.current_hapd = hapd;
	data.uuid_e = uuid_e;
	data.dev_pw = dev_pw;
	data.dev_pw_len = dev_pw_len;
	wps_stop_registrar(hapd, &data);

	/* TODO add callback event for freeRTOS */
}


static void hostapd_wps_reenable_ap_pin(void *eloop_data, void *user_ctx)
{
	struct hostapd_data *hapd = eloop_data;

	if (hapd->conf->ap_setup_locked)
		return;
	if (hapd->ap_pin_failures_consecutive >= 10)
		return;

	wpa_printf(MSG_DEBUG, "WPS: Re-enable AP PIN");
	wpa_msg(hapd->msg_ctx, MSG_INFO, WPS_EVENT_AP_SETUP_UNLOCKED);
	hapd->wps->ap_setup_locked = 0;
	wps_registrar_update_ie(hapd->wps->registrar);
}

static void hostapd_pwd_auth_fail(struct hostapd_data *hapd,
				  struct wps_event_pwd_auth_fail *data)
{
	/* Update WPS Status - Authentication Failure */
	wifi_event_ap_wps_rg_fail_reason_t evt = {0};

	wpa_printf(MSG_DEBUG, "WPS: Authentication failure update");
	hapd->wps_stats.status = WPS_FAILURE_STATUS;
	os_memcpy(hapd->wps_stats.peer_addr, data->peer_macaddr, ETH_ALEN);

	os_memcpy(evt.peer_macaddr, data->peer_macaddr, ETH_ALEN);
	evt.reason = WPS_AP_FAIL_REASON_AUTH;
	esp_event_post(WIFI_EVENT, WIFI_EVENT_AP_WPS_RG_FAILED, &evt,
			sizeof(evt), OS_BLOCK);
}


static int wps_ap_pin_success(struct hostapd_data *hapd, void *ctx)
{
	if (hapd->conf->ap_pin == NULL || hapd->wps == NULL)
		return 0;

	if (hapd->ap_pin_failures_consecutive == 0)
		return 0;

	wpa_printf(MSG_DEBUG, "WPS: Clear consecutive AP PIN failure counter "
		   "- total validation failures %u (%u consecutive)",
		   hapd->ap_pin_failures, hapd->ap_pin_failures_consecutive);
	hapd->ap_pin_failures_consecutive = 0;

	return 0;
}


static void hostapd_wps_event_pbc_overlap(struct hostapd_data *hapd)
{
	/* Update WPS Status - PBC Overlap */

	hapd->wps_stats.pbc_status = WPS_PBC_STATUS_OVERLAP;
	esp_event_post(WIFI_EVENT, WIFI_EVENT_AP_WPS_RG_PBC_OVERLAP, NULL, 0, OS_BLOCK);
}


static void hostapd_wps_event_pbc_timeout(struct hostapd_data *hapd)
{
	/* Update WPS PBC Status:PBC Timeout */
	hapd->wps_stats.pbc_status = WPS_PBC_STATUS_TIMEOUT;
	esp_event_post(WIFI_EVENT, WIFI_EVENT_AP_WPS_RG_TIMEOUT, NULL, 0, OS_BLOCK);
}

static void hostapd_wps_event_pin_timeout(struct hostapd_data *hapd)
{
	esp_event_post(WIFI_EVENT, WIFI_EVENT_AP_WPS_RG_TIMEOUT, NULL, 0, OS_BLOCK);
}

static void hostapd_wps_event_pbc_disable(struct hostapd_data *hapd)
{
	/* Update WPS PBC status - Active */
	hapd->wps_stats.pbc_status = WPS_PBC_STATUS_DISABLE;
}


static void hostapd_wps_event_success(struct hostapd_data *hapd,
				      struct wps_event_success *success)
{
	wifi_event_ap_wps_rg_success_t evt;

	os_memcpy(evt.peer_macaddr, success->peer_macaddr, ETH_ALEN);
	esp_event_post(WIFI_EVENT, WIFI_EVENT_AP_WPS_RG_SUCCESS, &evt,
			sizeof(evt), OS_BLOCK);
}


static void hostapd_wps_event_fail(struct hostapd_data *hapd,
				   struct wps_event_fail *fail)
{
	wifi_event_ap_wps_rg_fail_reason_t evt;
	if (fail->config_error > WPS_CFG_NO_ERROR)
		evt.reason = WPS_AP_FAIL_REASON_CONFIG;
	else
		evt.reason = WPS_AP_FAIL_REASON_NORMAL;
	os_memcpy(evt.peer_macaddr, fail->peer_macaddr, ETH_ALEN);
	esp_event_post(WIFI_EVENT, WIFI_EVENT_AP_WPS_RG_FAILED, &evt,
			sizeof(evt), OS_BLOCK);
}


static void hostapd_wps_event_cb(void *ctx, enum wps_event event,
				 union wps_event_data *data)
{
	struct hostapd_data *hapd = ctx;

	switch (event) {
	case WPS_EV_FAIL:
		hostapd_wps_event_fail(hapd, &data->fail);
		break;
	case WPS_EV_SUCCESS:
		hostapd_wps_event_success(hapd, &data->success);
		wpa_msg(hapd->msg_ctx, MSG_INFO, WPS_EVENT_SUCCESS);
		break;
	case WPS_EV_PWD_AUTH_FAIL:
		hostapd_pwd_auth_fail(hapd, &data->pwd_auth_fail);
		break;
	case WPS_EV_PBC_OVERLAP:
		hostapd_wps_event_pbc_overlap(hapd);
		wpa_msg(hapd->msg_ctx, MSG_INFO, WPS_EVENT_OVERLAP);
		break;
	case WPS_EV_PBC_TIMEOUT:
		hostapd_wps_event_pbc_timeout(hapd);
		wpa_msg(hapd->msg_ctx, MSG_INFO, WPS_EVENT_TIMEOUT);
		break;
	case WPS_EV_SELECTED_REGISTRAR_TIMEOUT:
		hostapd_wps_event_pin_timeout(hapd);
	case WPS_EV_PBC_ACTIVE:
		wpa_msg(hapd->msg_ctx, MSG_INFO, WPS_EVENT_ACTIVE);
		break;
	case WPS_EV_PBC_DISABLE:
		hostapd_wps_event_pbc_disable(hapd);
		wpa_msg(hapd->msg_ctx, MSG_INFO, WPS_EVENT_DISABLE);
		break;
	case WPS_EV_ER_AP_ADD:
		break;
	case WPS_EV_ER_AP_REMOVE:
		break;
	case WPS_EV_ER_ENROLLEE_ADD:
		break;
	case WPS_EV_ER_ENROLLEE_REMOVE:
		break;
	case WPS_EV_ER_AP_SETTINGS:
		break;
	case WPS_EV_ER_SET_SELECTED_REGISTRAR:
		break;
	case WPS_EV_AP_PIN_SUCCESS:
		wps_ap_pin_success(hapd, NULL);
		break;
	default:
		break;
	}
	if (hapd->wps_event_cb)
		hapd->wps_event_cb(hapd, event, data);
}


static int hostapd_wps_rf_band_cb(void *ctx)
{
	return WPS_RF_24GHZ;
}


static void hostapd_wps_clear_ies(struct hostapd_data *hapd, int deinit_only)
{
	esp_wifi_unset_appie_internal(WIFI_APPIE_RAM_BEACON);
	esp_wifi_unset_appie_internal(WIFI_APPIE_RAM_PROBE_RSP);
}

static void hostapd_free_wps(struct wps_context *wps)
{
	int i;

	for (i = 0; i < MAX_WPS_VENDOR_EXTENSIONS; i++)
		wpabuf_free(wps->dev.vendor_ext[i]);
	wps_device_data_free(&wps->dev);
	os_free(wps->network_key);
	wpabuf_free(wps->dh_pubkey);
	wpabuf_free(wps->dh_privkey);
	os_free(wps);
}

int hostapd_init_wps(struct hostapd_data *hapd, struct wps_data *wps_data, struct wps_context *wps)
{
	struct wps_registrar_config cfg;

	wps->event_cb = hostapd_wps_event_cb;
	wps->rf_band_cb = hostapd_wps_rf_band_cb;
	wps->cb_ctx = hapd;

	wps->wps_state = WPS_STATE_CONFIGURED;
	wps->ap_setup_locked = 0;
	wps->ap = 1;

	/* build credentials to be given */
	hostapd_wps_config_ap(hapd, wps_data);

	os_memset(&cfg, 0, sizeof(cfg));
	cfg.set_ie_cb = hostapd_wps_set_ie_cb;
	cfg.reg_success_cb = hostapd_wps_reg_success_cb;
	cfg.cb_ctx = hapd;
	cfg.disable_auto_conf = 1;

	wps->registrar = wps_registrar_init(wps, &cfg);
	if (wps->registrar == NULL) {
		wpa_printf(MSG_ERROR, "Failed to initialize WPS Registrar");
		goto fail;
	}

	hapd->wps = wps;

	eap_server_identity_register();
	if (eap_server_wsc_register() < 0) {
		wpa_printf(MSG_ERROR, "Failed to register for wsc server");
	}
	if (ieee802_1x_init(hapd) < 0) {
		return 0;
	}
	return 0;

fail:
	hostapd_free_wps(wps);
	return -1;
}

#ifdef ESP_SUPPLICANT
static int ap_sta_server_sm_deinit(struct hostapd_data *hapd,
		struct sta_info *sta, void *ctx)
{
	ieee802_1x_free_station(hapd, sta);
	return 0;
}
#endif /* ESP_SUPPLICANT */

void hostapd_deinit_wps(struct hostapd_data *hapd)
{
	eloop_cancel_timeout(hostapd_wps_reenable_ap_pin, hapd, NULL);
	eloop_cancel_timeout(hostapd_wps_ap_pin_timeout, hapd, NULL);
	if (hapd->wps == NULL) {
		hostapd_wps_clear_ies(hapd, 1);
		return;
	}
	wps_registrar_deinit(hapd->wps->registrar);
	hapd->wps->registrar = NULL;

#ifdef ESP_SUPPLICANT
	ap_for_each_sta(hapd, ap_sta_server_sm_deinit, NULL);
#endif /* ESP_SUPPLICANT */

	eap_server_unregister_methods();
	hapd->wps = NULL;
	hostapd_wps_clear_ies(hapd, 1);
}


void hostapd_update_wps(struct hostapd_data *hapd)
{
	struct wps_context *wps = hapd->wps;
	struct hostapd_bss_config *conf = hapd->conf;

	if (!wps)
		return;

	os_memcpy(wps->ssid, conf->ssid.ssid, conf->ssid.ssid_len);
	wps->ssid_len = conf->ssid.ssid_len;

	/* Clear WPS settings, then fill them again */
	hostapd_wps_config_ap(hapd, NULL);

	wps_registrar_update_ie(wps->registrar);
}

int hostapd_wps_add_pin(struct hostapd_data *hapd, const u8 *pin)
{
	int ret;

	if (hapd->wps == NULL)
		return 0;
	ret = wps_registrar_add_pin(hapd->wps->registrar, NULL,
				    NULL, pin, 8, 120);
	return ret;
}

int hostapd_wps_button_pushed(struct hostapd_data *hapd,
			      const u8 *p2p_dev_addr)
{
	if (hapd->wps) {
		return wps_registrar_button_pushed(hapd->wps->registrar,
						   p2p_dev_addr);
	}
	return 0;
}


static int wps_cancel(struct hostapd_data *hapd)
{
	if (hapd->wps) {
		wps_registrar_wps_cancel(hapd->wps->registrar);
		wpa_msg(hapd->msg_ctx, MSG_INFO, WPS_EVENT_CANCEL);
	}

	return 0;
}


int hostapd_wps_cancel(struct hostapd_data *hapd)
{
	int ret;

	ret = wps_cancel(hapd);
	return ret;
}


static void hostapd_wps_ap_pin_timeout(void *eloop_data, void *user_ctx)
{
	struct hostapd_data *hapd = eloop_data;
	wpa_printf(MSG_DEBUG, "WPS: AP PIN timed out");
	hostapd_wps_ap_pin_disable(hapd);
	wpa_msg(hapd->msg_ctx, MSG_INFO, WPS_EVENT_AP_PIN_DISABLED);
}


static void hostapd_wps_ap_pin_enable(struct hostapd_data *hapd, int timeout)
{
	wpa_printf(MSG_DEBUG, "WPS: Enabling AP PIN (timeout=%d)", timeout);
	hapd->ap_pin_failures = 0;
	hapd->ap_pin_failures_consecutive = 0;
	hapd->conf->ap_setup_locked = 0;
	if (hapd->wps->ap_setup_locked) {
		wpa_msg(hapd->msg_ctx, MSG_INFO, WPS_EVENT_AP_SETUP_UNLOCKED);
		hapd->wps->ap_setup_locked = 0;
		wps_registrar_update_ie(hapd->wps->registrar);
	}
	eloop_cancel_timeout(hostapd_wps_ap_pin_timeout, hapd, NULL);
	if (timeout > 0)
		eloop_register_timeout(timeout, 0,
				       hostapd_wps_ap_pin_timeout, hapd, NULL);
}

static int wps_ap_pin_disable(struct hostapd_data *hapd)
{
	os_free(hapd->conf->ap_pin);
	hapd->conf->ap_pin = NULL;
	eloop_cancel_timeout(hostapd_wps_ap_pin_timeout, hapd, NULL);
	return 0;
}


void hostapd_wps_ap_pin_disable(struct hostapd_data *hapd)
{
	wpa_printf(MSG_DEBUG, "WPS: Disabling AP PIN");
	wps_ap_pin_disable(hapd);
}


struct wps_ap_pin_data {
	char pin_txt[9];
	int timeout;
};


static int wps_ap_pin_set(struct hostapd_data *hapd, void *ctx)
{
	struct wps_ap_pin_data *data = ctx;

	if (!hapd->wps)
		return 0;

	os_free(hapd->conf->ap_pin);
	hapd->conf->ap_pin = os_strdup(data->pin_txt);
	hostapd_wps_ap_pin_enable(hapd, data->timeout);
	return 0;
}


const char * hostapd_wps_ap_pin_random(struct hostapd_data *hapd, int timeout)
{
	unsigned int pin;
	struct wps_ap_pin_data data;

	if (wps_generate_pin(&pin) < 0)
		return NULL;
	os_snprintf(data.pin_txt, sizeof(data.pin_txt), "%08u", pin);
	data.timeout = timeout;
	wps_ap_pin_set(hapd, &data);
	return hapd->conf->ap_pin;
}

const char * hostapd_wps_ap_pin_get(struct hostapd_data *hapd)
{
	return hapd->conf->ap_pin;
}


int hostapd_wps_ap_pin_set(struct hostapd_data *hapd, const char *pin,
			   int timeout)
{
	struct wps_ap_pin_data data;
	int ret;

	ret = os_snprintf(data.pin_txt, sizeof(data.pin_txt), "%s", pin);
	if (os_snprintf_error(sizeof(data.pin_txt), ret))
		return -1;
	data.timeout = timeout;
	return wps_ap_pin_set(hapd, &data);
}


int hostapd_wps_config_ap(struct hostapd_data *hapd, struct wps_data *wps_data)
{
	struct wps_credential cred = {0};

	os_memcpy(cred.ssid, hapd->conf->ssid.ssid, hapd->conf->ssid.ssid_len);
	cred.ssid_len = hapd->conf->ssid.ssid_len;
	cred.encr_type = WPS_ENCR_NONE;
	cred.auth_type = WPS_AUTH_OPEN;

	if (hapd->wpa_auth->conf.wpa == WPA_PROTO_WPA) {
		cred.auth_type = WPS_AUTH_WPAPSK;
		cred.encr_type = WPS_ENCR_TKIP;
	} else if (hapd->wpa_auth->conf.wpa == WPA_PROTO_RSN) {
		cred.auth_type = WPS_AUTH_WPA2PSK;
		cred.encr_type = WPS_ENCR_AES;
	} else if (hapd->wpa_auth->conf.wpa == (WPA_PROTO_RSN | WPA_PROTO_WPA)) {
		cred.auth_type = WPS_AUTH_WPA2PSK | WPS_AUTH_WPAPSK;
		cred.encr_type = WPS_ENCR_AES;
	}

	if (hapd->conf->ssid.wpa_passphrase) {
		cred.key_len = os_strlen(hapd->conf->ssid.wpa_passphrase);
		memcpy(cred.key, hapd->conf->ssid.wpa_passphrase, cred.key_len);
	}
	wps_data->use_cred = os_malloc(sizeof(struct wps_credential));
	if (!wps_data->use_cred) {
		wpa_printf(MSG_ERROR, "WPS: Disabling AP PIN");
		return -1;
	}
	os_memcpy(wps_data->use_cred, &cred, sizeof(struct wps_credential));
	return 0;
}
