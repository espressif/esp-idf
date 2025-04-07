/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "utils/includes.h"
#include "utils/common.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_wifi_driver.h"
#include "drivers/driver.h"
#include "common/bss.h"
#include "common/rrm.h"
#include "common/wnm_sta.h"
#include "common/wpa_supplicant_i.h"
#include "esp_scan_i.h"
#include "esp_common_i.h"
#include "common/ieee802_11_common.h"
#include "esp_rrm.h"
#include "esp_wnm.h"
#include "rsn_supp/wpa_i.h"
#include "rsn_supp/wpa.h"
#include "esp_private/wifi.h"

/* Utility Functions */
esp_err_t esp_supplicant_str_to_mac(const char *str, uint8_t dest[6])
{
    if (hwaddr_aton2(str, dest) < 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

struct wpa_supplicant g_wpa_supp;

#ifdef CONFIG_RRM
static void handle_rrm_frame(struct wpa_supplicant *wpa_s, u8 *sender,
			     u8 *payload, size_t len, int8_t rssi)
{
	if (payload[0] == WLAN_RRM_NEIGHBOR_REPORT_RESPONSE) {
		/* neighbor report parsing */
		wpas_rrm_process_neighbor_rep(wpa_s, payload + 1, len - 1);
	} else if (payload[0] == WLAN_RRM_RADIO_MEASUREMENT_REQUEST) {
		/* Beacon measurement */
		wpas_rrm_handle_radio_measurement_request(wpa_s, NULL,
				sender, payload + 1, len - 1);
	} else if (payload[0] == WLAN_RRM_LINK_MEASUREMENT_REQUEST) {
		/* Link measurement */
		wpas_rrm_handle_link_measurement_request(wpa_s, NULL,
				payload + 1, len - 1, rssi);
	}
}
#endif /* CONFIG_RRM */

#if defined(CONFIG_IEEE80211KV)
static int mgmt_rx_action(u8 *frame, size_t len, u8 *sender, int8_t rssi, u8 channel)
{
	#if defined(CONFIG_RRM) || defined(CONFIG_WNM)
	u8 category;
	u8 bssid[ETH_ALEN];
	struct wpa_supplicant *wpa_s = &g_wpa_supp;
	int ret = esp_wifi_get_assoc_bssid_internal(bssid);

	if (ret < 0) {
		wpa_printf(MSG_INFO, "STA not associated");
		return -1;
	}

	category = *frame++;
	len--;
#if defined(CONFIG_WNM)
	if (category == WLAN_ACTION_WNM) {
		ieee802_11_rx_wnm_action(wpa_s, sender, frame, len);
	}
#endif /* CONFIG_WNM */
#if defined(CONFIG_RRM)
	if (category == WLAN_ACTION_RADIO_MEASUREMENT) {
		handle_rrm_frame(wpa_s, sender, frame, len, rssi);
	}
#endif /* CONFIG_RRM */
#endif /* defined(CONFIG_RRM) || defined(CONFIG_WNM) */
	return 0;
}

#if defined(CONFIG_WNM)
static void clear_bssid_flag_and_channel(struct wpa_supplicant *wpa_s)
{
	wifi_config_t *config;

	/* Reset only if btm is enabled */
	if (esp_wifi_is_btm_enabled_internal(WIFI_IF_STA) == false) {
		return;
	}

    config = os_zalloc(sizeof(wifi_config_t));

    if (!config) {
        wpa_printf(MSG_ERROR, "failed to allocate memory");
        return;
    }

    esp_wifi_get_config(WIFI_IF_STA, config);
    if (config->sta.bssid_set || config->sta.channel) {
        config->sta.channel = 0;
        config->sta.bssid_set = 0;
        esp_wifi_set_config(WIFI_IF_STA, config);
    }
    os_free(config);
    wpa_printf(MSG_DEBUG, "cleared bssid flag");
}
#endif /* CONFIG_WNM */
#endif /* CONFIG_IEEE80211KV */

#if defined(CONFIG_IEEE80211KV) || defined(CONFIG_IEEE80211R)
static void register_mgmt_frames(struct wpa_supplicant *wpa_s)
{
	wpa_s->type &= ~(1 << WLAN_FC_STYPE_ACTION);
	/* subtype is defined only for action frame */
	wpa_s->subtype = 0;

#ifdef CONFIG_RRM
	/* current supported features in supplicant: rrm and btm */
	if (esp_wifi_is_rm_enabled_internal(WIFI_IF_STA)) {
		wpa_s->subtype = 1 << WLAN_ACTION_RADIO_MEASUREMENT;
	}
#endif /* CONFIG_RRM */
#ifdef CONFIG_WNM
	if (esp_wifi_is_btm_enabled_internal(WIFI_IF_STA)) {
		wpa_s->subtype |= 1 << WLAN_ACTION_WNM;
	}
#endif /* CONFIG_WNM */
	if (wpa_s->subtype) {
		wpa_s->type |= 1 << WLAN_FC_STYPE_ACTION;
	}

#ifdef CONFIG_IEEE80211R
	/* register auth/assoc frames if FT is enabled */
	if (esp_wifi_is_ft_enabled_internal(ESP_IF_WIFI_STA))
		wpa_s->type |= (1 << WLAN_FC_STYPE_AUTH) |
				(1 << WLAN_FC_STYPE_ASSOC_RESP) |
				(1 << WLAN_FC_STYPE_REASSOC_RESP);
#endif /* CONFIG_IEEE80211R */
	esp_wifi_register_mgmt_frame_internal(wpa_s->type, wpa_s->subtype);
}
#endif /* defined(CONFIG_IEEE80211KV) || defined(CONFIG_IEEE80211R) */

#ifdef CONFIG_IEEE80211R
static int handle_auth_frame(u8 *frame, size_t len,
			     u8 *sender, int8_t rssi, u8 channel)
{
	if (gWpaSm.key_mgmt == WPA_KEY_MGMT_FT_PSK) {
		if (gWpaSm.ft_protocol) {
			if (wpa_ft_process_response(&gWpaSm, frame + 6,
						    len - 6, 0, sender, NULL, 0) < 0) {
				wpa_sm_set_ft_params(&gWpaSm, NULL, 0);
				return -1;
			}
		}
	}
	return 0;
}

static int handle_assoc_frame(u8 *frame, size_t len,
			      u8 *sender, int8_t rssi, u8 channel)
{
	if (gWpaSm.key_mgmt == WPA_KEY_MGMT_FT_PSK) {
		if (gWpaSm.ft_protocol) {
			if (wpa_ft_validate_reassoc_resp(&gWpaSm, frame + 6, len - 6, sender)) {
				wpa_sm_set_ft_params(&gWpaSm, NULL, 0);
				return -1;
			}
		}
		wpa_sm_set_ft_params(&gWpaSm, frame + 6, len - 6);
	}
	return 0;
}

void wpa_sta_clear_ft_auth_ie(void)
{
    struct wpa_sm *sm = &gWpaSm;
    wpa_printf(MSG_DEBUG, "Clearing all FT IE parameters and keys");
    /* Reset FT parameters */
    wpa_sm_set_ft_params(sm, NULL, 0);
    esp_wifi_unset_appie_internal(WIFI_APPIE_RAM_STA_AUTH);
    sm->ft_reassoc_completed = 0;
    sm->ft_protocol = 0;
    /* Clear pmk and ptk */
    wpa_sm_drop_sa(sm);
}
#endif /* CONFIG_IEEE80211R */

void esp_supplicant_unset_all_appie(void)
{
   uint8_t appie;
   for (appie = WIFI_APPIE_PROBEREQ; appie < WIFI_APPIE_RAM_MAX; appie++) {
        esp_wifi_unset_appie_internal(appie);
   }
}

static int ieee80211_handle_rx_frm(u8 type, u8 *frame, size_t len, u8 *sender,
				   int8_t rssi, u8 channel, u64 current_tsf)
{
	int ret = 0;

	switch (type) {
#if defined(CONFIG_IEEE80211KV) || defined(CONFIG_IEEE80211R)
	case WLAN_FC_STYPE_BEACON:
	case WLAN_FC_STYPE_PROBE_RESP:
		ret = esp_handle_beacon_probe(type, frame, len, sender, rssi, channel, current_tsf);
		break;
#endif /* defined(CONFIG_IEEE80211KV) || defined(CONFIG_IEEE80211R) */
#ifdef CONFIG_IEEE80211R
	case WLAN_FC_STYPE_AUTH:
		ret = handle_auth_frame(frame, len, sender, rssi, channel);
		break;
#endif /* CONFIG_IEEE80211R */
	case WLAN_FC_STYPE_ASSOC_RESP:
	case WLAN_FC_STYPE_REASSOC_RESP:
#ifdef CONFIG_IEEE80211R
		ret = handle_assoc_frame(frame, len, sender, rssi, channel);
#endif /* CONFIG_IEEE80211R */
		wpa_sm_notify_assoc(&gWpaSm, sender);
		break;
#if defined(CONFIG_IEEE80211KV)
	case WLAN_FC_STYPE_ACTION:
		mgmt_rx_action(frame, len, sender, rssi, channel);
		break;
#endif /* defined(CONFIG_IEEE80211KV) */
	default:
		ret = -1;
		break;
	}

	return ret;
}

#if defined(CONFIG_IEEE80211KV) || defined(CONFIG_IEEE80211R)
#ifdef CONFIG_MBO
bool mbo_bss_profile_match(u8 *bssid)
{
	/* Incase supplicant wants drivers to skip this BSS, return false */
	struct wpa_bss *bss = wpa_bss_get_bssid(&g_wpa_supp, bssid);
	if (!bss) {
		return true;
	}
	const u8 *assoc_disallow = wpas_mbo_get_bss_attr(bss, MBO_ATTR_ID_ASSOC_DISALLOW);
	if (assoc_disallow && assoc_disallow[1] >= 1) {
		wpa_printf(MSG_DEBUG,
				"skip - MBO association disallowed (reason %u)", assoc_disallow[2]);
		return false;
	}

	if (wpa_is_bss_tmp_disallowed(&g_wpa_supp, bss)) {
		wpa_printf(MSG_DEBUG,
				"skip - BSS is temporary disallowed");
		return false;
	}

	return true;
}
#endif /* CONFIG_MBO */

#endif /* defined(CONFIG_IEEE80211KV) || defined(CONFIG_IEEE80211R) */

int esp_supplicant_common_init(struct wpa_funcs *wpa_cb)
{
	struct wpa_supplicant *wpa_s = &g_wpa_supp;
	int ret = 0;

#if defined(CONFIG_IEEE80211KV) || defined(CONFIG_IEEE80211R)
#if defined(CONFIG_RRM)
	wpas_rrm_reset(wpa_s);
	wpas_clear_beacon_rep_data(wpa_s);
#endif /* defined(CONFIG_RRM) */
	esp_scan_init(wpa_s);

#endif /* defined(CONFIG_IEEE80211KV) || defined(CONFIG_IEEE80211R) */
	wpa_s->type = 0;
	wpa_s->subtype = 0;
	wpa_s->type |= (1 << WLAN_FC_STYPE_ASSOC_RESP) | (1 << WLAN_FC_STYPE_REASSOC_RESP) | (1 << WLAN_FC_STYPE_AUTH);
	if (esp_wifi_register_mgmt_frame_internal(wpa_s->type, wpa_s->subtype) != ESP_OK) {
		ret = -1;
		goto err;
	}
	wpa_cb->wpa_sta_rx_mgmt = ieee80211_handle_rx_frm;

#if defined(CONFIG_IEEE80211KV) || defined(CONFIG_IEEE80211R)
#ifdef CONFIG_MBO
	dl_list_init(&wpa_s->bss_tmp_disallowed);
#endif /* CONFIG_MBO */
#endif /* defined(CONFIG_IEEE80211KV) || defined(CONFIG_IEEE80211R) */
	return 0;
err:
	esp_supplicant_common_deinit();
	return ret;
}

void esp_supplicant_common_deinit(void)
{
	struct wpa_supplicant *wpa_s = &g_wpa_supp;

#if defined(CONFIG_IEEE80211KV) || defined(CONFIG_IEEE80211R)
	esp_scan_deinit(wpa_s);
#if defined(CONFIG_RRM)
	wpas_rrm_reset(wpa_s);
	wpas_clear_beacon_rep_data(wpa_s);
#endif /* defined(CONFIG_RRM) */
#endif /* defined(CONFIG_IEEE80211KV) || defined(CONFIG_IEEE80211R) */
	if (wpa_s->type) {
		wpa_s->type = 0;
		esp_wifi_register_mgmt_frame_internal(wpa_s->type, wpa_s->subtype);
	}
}

void supplicant_sta_conn_handler(uint8_t *bssid)
{
#if defined(CONFIG_IEEE80211KV) || defined(CONFIG_IEEE80211R)
	u8 *ie;
	struct wpa_supplicant *wpa_s = &g_wpa_supp;
	struct wpa_bss *bss = wpa_bss_get_bssid(wpa_s, bssid);
	if (!bss) {
		wpa_printf(MSG_INFO, "connected bss entry not present in scan cache");
		return;
	}
	wpa_s->current_bss = bss;
	ie = (u8 *)bss;
	ie += sizeof(struct wpa_bss);
	ieee802_11_parse_elems(wpa_s, ie, bss->ie_len);
	wpa_bss_flush(wpa_s);
	/* Register for mgmt frames */
	register_mgmt_frames(wpa_s);
#if defined(CONFIG_WNM)
	/* clear set bssid flag */
	clear_bssid_flag_and_channel(wpa_s);
#endif /* CONFIG_WNM */
#endif /* defined(CONFIG_IEEE80211KV) || defined(CONFIG_IEEE80211R) */
}

void supplicant_sta_disconn_handler(uint8_t reason_code)
{
#if defined(CONFIG_IEEE80211KV)
	struct wpa_supplicant *wpa_s = &g_wpa_supp;

#if defined(CONFIG_RRM)
	wpas_rrm_reset(wpa_s);
	wpas_clear_beacon_rep_data(wpa_s);
#endif /* defined(CONFIG_RRM) */
#if defined(CONFIG_WNM)
	/* Not clearing in case of roaming disconnect as BTM induced connection
	* itself sets a specific bssid and channel to connect to before disconnection.
	* Subsequent connections or disconnections will clear this flag */
	if (reason_code != WIFI_REASON_ROAMING) {
		clear_bssid_flag_and_channel(wpa_s);
	}
#endif /* CONFIG_WNM */
	if (wpa_s->current_bss) {
		wpa_s->current_bss = NULL;
	}
#endif /* defined(CONFIG_IEEE80211KV) */
}

#if defined(CONFIG_RRM)
bool esp_rrm_is_rrm_supported_connection(void)
{
	struct wpa_supplicant *wpa_s = &g_wpa_supp;

	if (!wpa_s->current_bss) {
		wpa_printf(MSG_DEBUG, "STA not associated, return");
		return false;
	}

	if (!(wpa_s->rrm_ie[0] & WLAN_RRM_CAPS_NEIGHBOR_REPORT)) {
		wpa_printf(MSG_DEBUG,
			"RRM: No network support for Neighbor Report.");
		return false;
	}

	return true;
}
int esp_rrm_send_neighbor_rep_request(neighbor_rep_request_cb cb,
				      void *cb_ctx)
{
	struct wpa_supplicant *wpa_s = &g_wpa_supp;
	struct wpa_ssid_value wpa_ssid = {0};
	struct wifi_ssid *ssid;

	if (!wpa_s->current_bss) {
		wpa_printf(MSG_ERROR, "STA not associated, return");
		return -2;
	}

	if (!(wpa_s->rrm_ie[0] & WLAN_RRM_CAPS_NEIGHBOR_REPORT)) {
		wpa_printf(MSG_ERROR,
			"RRM: No network support for Neighbor Report.");
		return -1;
	}

	ssid = esp_wifi_sta_get_prof_ssid_internal();

	os_memcpy(wpa_ssid.ssid, ssid->ssid, ssid->len);
	wpa_ssid.ssid_len = ssid->len;

	return wpas_rrm_send_neighbor_rep_request(wpa_s, &wpa_ssid, 0, 0, cb, cb_ctx);
}

static size_t get_rm_enabled_ie(uint8_t *ie, size_t len)
{
    uint8_t rrm_ie[7] = {0};
    uint8_t rrm_ie_len = 5;
    uint8_t *pos = rrm_ie;

    if (!esp_wifi_is_rm_enabled_internal(WIFI_IF_STA)) {
        return 0;
    }

    *pos++ = WLAN_EID_RRM_ENABLED_CAPABILITIES;
    *pos++ = rrm_ie_len;
    *pos |= WLAN_RRM_CAPS_LINK_MEASUREMENT;

    *pos |= WLAN_RRM_CAPS_BEACON_REPORT_PASSIVE |
#ifdef SCAN_CACHE_SUPPORTED
            WLAN_RRM_CAPS_BEACON_REPORT_TABLE |
#endif /* SCAN_CACHE_SUPPORTED */
            WLAN_RRM_CAPS_BEACON_REPORT_ACTIVE;

    os_memcpy(ie, rrm_ie, sizeof(rrm_ie));

    return rrm_ie_len + 2;
}
#endif /* defined(CONFIG_RRM) */

#if defined(CONFIG_WNM)
bool esp_wnm_is_btm_supported_connection(void)
{
	struct wpa_supplicant *wpa_s = &g_wpa_supp;

	if (!wpa_s->current_bss) {
		wpa_printf(MSG_DEBUG, "STA not associated, return");
		return false;
	}

	if (!wpa_bss_ext_capab(wpa_s->current_bss, WLAN_EXT_CAPAB_BSS_TRANSITION)) {
		wpa_printf(MSG_DEBUG, "AP doesn't support BTM, return");
		return false;
	}

	return true;
}

int esp_wnm_send_bss_transition_mgmt_query(enum btm_query_reason query_reason,
					   const char *btm_candidates,
					   int cand_list)
{
	struct wpa_supplicant *wpa_s = &g_wpa_supp;

	if (!wpa_s->current_bss) {
		wpa_printf(MSG_ERROR, "STA not associated, return");
		return -2;
	}

	if (!wpa_bss_ext_capab(wpa_s->current_bss, WLAN_EXT_CAPAB_BSS_TRANSITION)) {
		wpa_printf(MSG_ERROR, "AP doesn't support BTM, return");
		return -1;
	}
	return wnm_send_bss_transition_mgmt_query(wpa_s, query_reason, btm_candidates, cand_list);
}

static uint8_t get_extended_caps_ie(uint8_t *ie, size_t len)
{
	uint8_t ext_caps_ie[5] = {0};
	uint8_t ext_caps_ie_len = 3;
	uint8_t *pos = ext_caps_ie;
	wifi_ioctl_config_t cfg = {0};
	esp_err_t err = 0;

	if (!esp_wifi_is_btm_enabled_internal(WIFI_IF_STA)) {
		return 0;
	}

	*pos++ = WLAN_EID_EXT_CAPAB;
	*pos++ = ext_caps_ie_len;

	err = esp_wifi_internal_ioctl(WIFI_IOCTL_GET_STA_HT2040_COEX, &cfg);
	if (err == ESP_OK && cfg.data.ht2040_coex.enable) {
		*pos++ |= BIT(WLAN_EXT_CAPAB_20_40_COEX);
	} else {
		*pos++ = 0;
	}
	*pos ++ = 0;
#define CAPAB_BSS_TRANSITION BIT(3)
	*pos |= CAPAB_BSS_TRANSITION;
#undef CAPAB_BSS_TRANSITION
	os_memcpy(ie, ext_caps_ie, sizeof(ext_caps_ie));

	return ext_caps_ie_len + 2;
}
#endif /* defined(CONFIG_WNM) */

#ifdef CONFIG_MBO
int esp_mbo_update_non_pref_chan(struct non_pref_chan_s *non_pref_chan)
{
    int ret = wpas_mbo_update_non_pref_chan(&g_wpa_supp, non_pref_chan);

    return ret;
}

static size_t get_mbo_oce_scan_ie(uint8_t *ie, size_t len)
{
	uint8_t mbo_ie[32] = {0};
	uint8_t mbo_ie_len = 32;

	/* Return if MBO IE is not enabled in driver */
	if (!esp_wifi_is_mbo_enabled_internal(WIFI_IF_STA)) {
		return 0;
	}

	struct wpabuf *default_ies = NULL;
	if (wpabuf_resize(&default_ies, 18) == 0) {
		wpas_mbo_scan_ie(&g_wpa_supp, default_ies);
		os_memcpy(mbo_ie, wpabuf_head_u8(default_ies), wpabuf_len(default_ies));
		mbo_ie_len = wpabuf_len(default_ies);
		wpabuf_free(default_ies);
	}
	os_memcpy(ie, mbo_ie, mbo_ie_len);
	return mbo_ie_len;
}

static size_t get_mbo_oce_assoc_ie(uint8_t *ie, size_t len)
{
	uint8_t mbo_ie[32] = {0};
	uint8_t mbo_ie_len = 32;

	/* Return if MBO IE is not enabled in driver */
	if (!esp_wifi_is_mbo_enabled_internal(WIFI_IF_STA)) {
		return 0;
	}

	mbo_ie_len = wpas_mbo_ie(&g_wpa_supp, mbo_ie, mbo_ie_len, 0);
	os_memcpy(ie, mbo_ie, mbo_ie_len);

	return mbo_ie_len;
}

static uint8_t get_operating_class_ie(uint8_t *ie, size_t len)
{
	uint8_t op_class_ie[4] = {0};
	uint8_t op_class_ie_len = 2;
	uint8_t *pos = op_class_ie;

	*pos++ = WLAN_EID_SUPPORTED_OPERATING_CLASSES;
	*pos++ = op_class_ie_len;
#define OPER_CLASS 0x51
        /* Current Operating Class */
        *pos++ = OPER_CLASS;
#undef OPER_CLASS
        *pos = 0;
	os_memcpy(ie, op_class_ie, sizeof(op_class_ie));

	return op_class_ie_len + 2;
}
#endif /* CONFIG_MBO */

void wpa_supplicant_connect(struct wpa_supplicant *wpa_s,
                            struct wpa_bss *bss, char *ssid)
{
	wifi_config_t *config = os_zalloc(sizeof(wifi_config_t));

	if (!config) {
		wpa_printf(MSG_ERROR, "failed to allocate memory");
		return;
	}

	esp_wifi_get_config(WIFI_IF_STA, config);
	/* We only support roaming in same ESS, therefore only bssid setting is needed */
	os_memcpy(config->sta.bssid, bss->bssid, ETH_ALEN);
	config->sta.bssid_set = 1;
	config->sta.channel = bss->channel;
	/* supplicant connect will only be called in case of bss transition(roaming) */
	esp_wifi_internal_issue_disconnect(WIFI_REASON_BSS_TRANSITION_DISASSOC);
	esp_wifi_set_config(WIFI_IF_STA, config);
	os_free(config);
	esp_wifi_connect();
}

#if defined(CONFIG_WNM) || defined(CONFIG_MBO)
void esp_set_scan_ie(void)
{
#define SCAN_IE_LEN 64
	uint8_t *ie, *pos;
	size_t len = SCAN_IE_LEN, ie_len;

	ie = os_malloc(SCAN_IE_LEN);
	if (!ie) {
		wpa_printf(MSG_ERROR, "failed to allocate ie");
		return;
	}
	pos = ie;
#ifdef CONFIG_WNM
	ie_len = get_extended_caps_ie(pos, len);
	pos += ie_len;
	len -= ie_len;
#endif /* defined(CONFIG_WNM) */
#ifdef CONFIG_MBO
	ie_len = get_mbo_oce_scan_ie(pos, len);
	pos += ie_len;
	len -= ie_len;
#endif /* CONFIG_MBO */
	if (SCAN_IE_LEN - len) {
		esp_wifi_unset_appie_internal(WIFI_APPIE_PROBEREQ);
		esp_wifi_set_appie_internal(WIFI_APPIE_PROBEREQ, ie, SCAN_IE_LEN - len, 0);
	}
	os_free(ie);
#undef SCAN_IE_LEN
}
#endif /* defined(CONFIG_WNM) || defined(CONFIG_MBO) */

#ifdef CONFIG_IEEE80211R
static size_t add_mdie(uint8_t *bssid, uint8_t *ie, size_t len)
{
	size_t mdie_len = 0;
	struct wpa_sm *sm = &gWpaSm;

	/* Return if MBO IE is not enabled in driver */
	if (!esp_wifi_is_ft_enabled_internal(WIFI_IF_STA)) {
		return 0;
	}

	struct wpa_bss *bss = wpa_bss_get_bssid(&g_wpa_supp, bssid);
	if (bss && wpa_key_mgmt_ft(sm->key_mgmt)) {
		const u8 *mdie = wpa_bss_get_ie(bss, WLAN_EID_MOBILITY_DOMAIN);

		if (mdie && mdie[1] >= MOBILITY_DOMAIN_ID_LEN) {
			const u8 *md = mdie + 2;
			const u8 *wpa_md = wpa_sm_get_ft_md(sm);

			if (os_memcmp(md, wpa_md,
				      MOBILITY_DOMAIN_ID_LEN) == 0) {
				/* Add mobility domain IE */
				mdie_len = wpa_ft_add_mdie(
						sm, ie,
						len, mdie);
			}
		}
	}

	return mdie_len;
}

int wpa_sm_update_ft_ies(struct wpa_sm *sm, const u8 *md,
			 const u8 *ies, size_t ies_len, bool auth_ie)
{
	wpa_printf(MSG_INFO, "Updating FT IEs (len=%d), auth_ie %d ", ies_len, auth_ie);
	if (os_memcmp(sm->mobility_domain, md, MOBILITY_DOMAIN_ID_LEN) != 0) {
		return 0;
	}
	/* Update auth IEs to be used in FT association */
	if (auth_ie) {
		esp_wifi_set_appie_internal(WIFI_APPIE_RAM_STA_AUTH, (u8 *)ies, ies_len, 0);
	} else {
		esp_set_assoc_ie(sm->bssid, ies, ies_len, false);
	}
	wpa_printf(MSG_INFO, "Updated FT IEs (len=%d) auth_ie=%d", ies_len, auth_ie);

	return 0;
}
#endif /* CONFIG_IEEE80211R */

void esp_get_tx_power(uint8_t *tx_power)
{
#define DEFAULT_MAX_TX_POWER 19 /* max tx power is 19.5 dbm */
	s8 power;
	/* esp sends management frames at max tx power configured */
	int ret = esp_wifi_get_max_tx_power(&power);
	if (ret != 0) {
		wpa_printf(MSG_ERROR, "failed to get tx power");
		*tx_power = DEFAULT_MAX_TX_POWER;
		return;
	}
	*tx_power = power/4;
#undef DEFAULT_MAX_TX_POWER
}

int wpa_drv_send_action(struct wpa_supplicant *wpa_s,
			unsigned int channel,
			unsigned int wait,
			const u8 *data, size_t data_len,
			int no_cck)
{
	int ret = -1;
	wifi_mgmt_frm_req_t *req;

	if (!wpa_s->current_bss) {
		return ret;
	}

	req = os_zalloc(sizeof(*req) + data_len);
	if (!req) {
		return ret;
	}

	req->ifx = WIFI_IF_STA;
	req->subtype = (WLAN_FC_STYPE_ACTION << 4);
	req->data_len = data_len;
	os_memcpy(req->data, data, req->data_len);

	ret = esp_wifi_send_mgmt_frm_internal(req);

	if (ret != 0) {
		wpa_printf(MSG_ERROR, "action frame sending failed");
	} else {
		wpa_printf(MSG_INFO, "action frame sent");
	}

	os_free(req);
	return ret;
}

#ifndef CONFIG_WNM
void esp_set_scan_ie(void) { }
bool esp_wnm_is_btm_supported_connection(void)
{
    return false;
}
int esp_wnm_send_bss_transition_mgmt_query(enum btm_query_reason query_reason,
                                           const char *btm_candidates,
                                           int cand_list)
{
    return -1;
}
#endif

#ifndef CONFIG_RRM
bool esp_rrm_is_rrm_supported_connection(void)
{
    return false;
}

int esp_rrm_send_neighbor_report_request(void)
{
    return -1;
}

int esp_rrm_send_neighbor_rep_request(neighbor_rep_request_cb cb,
				      void *cb_ctx)
{
	return -1;
}
#endif

#if defined(CONFIG_RRM) || defined(CONFIG_IEEE80211R) || defined(CONFIG_WPA3_SAE)
void esp_set_assoc_ie(uint8_t *bssid, const u8 *ies, size_t ies_len, bool mdie)
{
#define ASSOC_IE_LEN 128
	uint8_t *ie, *pos;
	size_t len = ASSOC_IE_LEN;
#if defined(CONFIG_RRM) || defined(CONFIG_IEEE80211R)
    size_t ie_len;
#endif /* defined(CONFIG_IEEE80211KV) || defined(CONFIG_IEEE80211R) */
	ie = os_malloc(ASSOC_IE_LEN + ies_len);
	if (!ie) {
		wpa_printf(MSG_ERROR, "failed to allocate ie");
	return;
    }
	pos = ie;
#if defined(CONFIG_RRM)
	ie_len = get_rm_enabled_ie(pos, len);
	pos += ie_len;
	len -= ie_len;
#endif /* defined(CONFIG_RRM) */
#ifdef CONFIG_MBO
	ie_len = get_operating_class_ie(pos, len);
	pos += ie_len;
	len -= ie_len;
	ie_len = get_mbo_oce_assoc_ie(pos, len);
	pos += ie_len;
	len -= ie_len;
#endif /* CONFIG_MBO */
#ifdef CONFIG_IEEE80211R
	if (mdie) {
		ie_len = add_mdie(bssid, pos, len);
		pos += ie_len;
		len -= ie_len;
	}
#endif /* CONFIG_IEEE80211R */
	if (ies_len) {
		os_memcpy(pos, ies, ies_len);
		pos += ies_len;
		len -= ies_len;
	}
	esp_wifi_set_appie_internal(WIFI_APPIE_ASSOC_REQ, ie, ASSOC_IE_LEN - len, 0);
	os_free(ie);
#undef ASSOC_IE_LEN
}
#else
void esp_set_assoc_ie(uint8_t *bssid, const u8 *ies, size_t ies_len, bool mdie) { }
#endif  /* defined(CONFIG_RRM) || defined(CONFIG_IEEE80211R) || defined(CONFIG_WPA3_SAE) */
