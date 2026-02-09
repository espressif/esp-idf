/*
 * SPDX-FileCopyrightText: 2019-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
    This file implements the WPS Enrollee logic, which allows an ESP device to connect
    to a WPS-enabled Access Point using either the Push-Button Configuration (PBC) or
    PIN method. It handles the WPS state machine, EAPOL frame exchange, and credential
    reception.

    The implementation relies on a global state machine (gWpsSm) and interacts with the
    main Wi-Fi task via an event loop (eloop). Public API functions are provided to
    enable, disable, and start the WPS process, with mutexes ensuring thread safety
    at the API boundary.
*/

#include <string.h>
#include <inttypes.h>

#include "utils/includes.h"
#include "common.h"
#include "eloop.h"
#include "rsn_supp/wpa.h"
#include "utils/common.h"
#include "common/eapol_common.h"
#include "utils/wpa_debug.h"
#include "common/ieee802_11_defs.h"
#include "crypto/dh_group5.h"
#include "wps/wps_i.h"
#include "wps/wps_dev_attr.h"
#include "eap_peer/eap_defs.h"
#include "eap_peer/eap_common.h"
#include "esp_wifi_driver.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_err.h"
#include "esp_private/wifi.h"
#include "esp_wps_i.h"
#include "esp_dpp_i.h"
#include "esp_wps.h"
#include "eap_common/eap_wsc_common.h"
#include "esp_wpas_glue.h"

#ifdef CONFIG_WPS_RECONNECT_ON_FAIL
static wifi_config_t *s_previous_wifi_config = NULL;
#endif

// WPS Timeout Constants (in seconds)
static const int WPS_TOTAL_TIMEOUT_SECS = 120;
static const int WPS_CONNECT_TIMEOUT_SECS = 20;
static const int EAPOL_START_HANDLE_TIMEOUT_SECS = 3;
static const int WPS_MSG_TIMEOUT_M2_SECS = 20;
static const int WPS_MSG_TIMEOUT_FINISH_SECS = 5;
static const int WPS_POST_M8_WORKAROUND_SECS = 2;
static const int WPS_SUCCESS_EVENT_TIMEOUT_SECS = 1;
static const int WPS_SCAN_RETRY_TIMEOUT_SECS = 2;

// Callback structure for handling WPS events within the Wi-Fi task
static struct wps_sm_funcs *s_wps_sm_cb = NULL;

const char *wps_model_number = CONFIG_IDF_TARGET;

/* API lock to ensure thread safety for public WPS functions */
void *s_wps_api_lock = NULL;  /* Used in WPS/WPS-REG public API only, never be freed */
void *s_wps_api_sem = NULL;   /* Sync semaphore used between WPS/WPS-REG public API caller task and WPS task, never be freed */
/* Synchronous flag to indicate if the WPS feature is enabled by the user. Protected by s_wps_api_lock. */
bool s_wps_enabled = false;
static void wifi_wps_scan_done(void *arg, ETS_STATUS status);

static void wifi_wps_scan(void *data, void *user_ctx);
static int wifi_station_wps_start(void *data, void *user_ctx);
static int wifi_wps_enable_internal(void *ctx, void *data);
static int wifi_wps_disable_internal(void *ctx, void *data);
static void wifi_station_wps_msg_timeout(void *data, void *user_ctx);
void wifi_station_wps_eapol_start_handle(void *data, void *user_ctx);
static void wifi_station_wps_success(void *data, void *user_ctx);
static void wifi_station_wps_timeout(void *data, void *user_ctx);
static int wps_delete_timer(void);
static int wps_finish(void);

struct wps_sm *gWpsSm = NULL;
static wifi_event_sta_wps_er_success_t s_wps_success_evt;
static wps_factory_information_t *s_factory_info = NULL;

/*
* wps_sm_ether_send - Send Ethernet frame
* @wpa_s: Pointer to wpa_supplicant data
* @proto: Ethertype in host byte order
* @buf: Frame payload starting from IEEE 802.1X header
* @len: Frame payload length
* Returns: >=0 on success, <0 on failure
*/
static inline int wps_sm_ether_send(struct wps_sm *sm, u16 proto,
                                    const u8 *data, size_t data_len)
{
    u8 bssid[ETH_ALEN];
    int ret = esp_wifi_get_assoc_bssid_internal(bssid);

    if (ret != 0) {
        wpa_printf(MSG_ERROR, "WPS: BSSID is empty, cannot send EAPOL frame");
        return -1;
    }

    return wpa_ether_send(sm, bssid, proto, data, data_len);
}

static inline u8 *wps_sm_alloc_eapol(struct wps_sm *sm, u8 type,
                                     const void *data, u16 data_len,
                                     size_t *msg_len, void **data_pos)
{
    return wpa_alloc_eapol(sm, type, data, data_len, msg_len, data_pos);
}

static inline void wps_sm_free_eapol(u8 *buffer)
{
    return wpa_free_eapol(buffer);
}

static void wps_build_ic_appie_wps_pr(void)
{
    struct wps_sm *sm = wps_sm_get();
    struct wpabuf *extra_ie = NULL;
    struct wpabuf *wps_ie;
    u16 pw_id;

    if (!sm) {
        return;
    }

    wpa_printf(MSG_DEBUG, "WPS: Building Probe Request IE");

    if (wps_get_type() == WPS_TYPE_PBC) {
        pw_id = DEV_PW_PUSHBUTTON;
    } else {
        pw_id = DEV_PW_DEFAULT;
    }
    wps_ie = wps_build_probe_req_ie(pw_id, sm->dev, sm->uuid,
                                    WPS_REQ_ENROLLEE, 0, NULL);

    if (!wps_ie) {
        return;
    }

    if (wpabuf_resize(&extra_ie, wpabuf_len(wps_ie)) == 0) {
        wpabuf_put_buf(extra_ie, wps_ie);
    } else {
        wpabuf_free(wps_ie);
        return;
    }
    wpabuf_free(wps_ie);

    esp_wifi_set_appie_internal(WIFI_APPIE_WPS_PR, (uint8_t *)wpabuf_head(extra_ie), extra_ie->used, 0);
    wpabuf_free(extra_ie);
}

static void wps_build_ic_appie_wps_ar(void)
{
    struct wpabuf *buf = wps_build_assoc_req_ie(WPS_REQ_ENROLLEE);

    wpa_printf(MSG_DEBUG, "WPS: Building Association Request IE");

    if (buf) {
        esp_wifi_set_appie_internal(WIFI_APPIE_WPS_AR, (uint8_t *)wpabuf_head(buf), buf->used, 0);
        wpabuf_free(buf);
    }
}

static int wps_send_eapol_frame(u8 eapol_type, const void *payload, size_t payload_len)
{
    struct wps_sm *sm = gWpsSm;
    u8 *buf;
    int len;
    int ret;

    if (!sm) {
        return ESP_FAIL;
    }

    buf = wps_sm_alloc_eapol(sm, eapol_type, payload, payload_len, (size_t *)&len, NULL);
    if (!buf) {
        wpa_printf(MSG_ERROR, "WPS: EAPOL buffer allocation failed");
        return ESP_ERR_NO_MEM;
    }

    ret = wps_sm_ether_send(sm, ETH_P_EAPOL, buf, len);
    wps_sm_free_eapol(buf);

    if (ret) {
        wpa_printf(MSG_ERROR, "WPS: EAPOL send failed (ret=%d)", ret);
        return ESP_FAIL;
    }
    return ESP_OK;
}

static bool is_ap_supports_sae(struct wps_scan_ie *scan)
{
    struct wpa_ie_data rsn_info;

    if (!scan->rsn) {
        return false;
    }

    wpa_parse_wpa_ie_rsn(scan->rsn, scan->rsn[1] + 2, &rsn_info);

    if (rsn_info.key_mgmt & WPA_KEY_MGMT_SAE) {
        return true;
    }

    return false;
}

static bool is_wps_pbc_overlap(struct wps_sm *sm, const u8 *sel_uuid)
{
    if (!sel_uuid) {
        wpa_printf(MSG_DEBUG, "WPS: null uuid field");
        return true;
    }

    if (os_memcmp(sel_uuid, sm->uuid_r, WPS_UUID_LEN) != 0) {
        wpa_printf(MSG_DEBUG, "WPS: Scanned BSS UUID does not match");
        wpa_hexdump(MSG_DEBUG, "WPS: UUID of scanned BSS is",
                    sel_uuid, WPS_UUID_LEN);
        wpa_hexdump(MSG_DEBUG, "WPS: UUID of sm BSS is",
                    sm->uuid_r, WPS_UUID_LEN);
        return true;
    }

    return false;
}

static bool wps_parse_scan_result(struct wps_scan_ie *scan)
{
    struct wps_sm *sm = wps_sm_get();
    wifi_mode_t op_mode = 0;

    if (!sm) {
        return false;
    }

    if (sm->wps_pbc_overlap) {
        return false;
    }

    if (!sm->is_wps_scan || !scan->bssid) {
        return false;
    }

    if (wps_get_type() == WPS_TYPE_DISABLE
            || (wps_get_status() != WPS_STATUS_DISABLE
                && wps_get_status() != WPS_STATUS_SCANNING)
       ) {
        return false;
    }

    esp_wifi_get_mode(&op_mode);
    if ((op_mode != WIFI_MODE_STA)
#ifdef CONFIG_ESP_WIFI_SOFTAP_SUPPORT
            && (op_mode != WIFI_MODE_APSTA)
#endif
       ) {
        return false;
    }

    if (!scan->rsn && !scan->wpa && (scan->capinfo & WLAN_CAPABILITY_PRIVACY)) {
        wpa_printf(MSG_DEBUG, "WPS: WEP not supported, skipping AP");
        return false;
    }

    if (scan->wps) {
        bool ap_found = false;
        struct wpabuf *buf = wpabuf_alloc_copy(scan->wps + 6, scan->wps[1] - 4);
        const u8 *scan_uuid;

        if ((wps_get_type() == WPS_TYPE_PBC && wps_is_selected_pbc_registrar(buf)) ||
                (wps_get_type() == WPS_TYPE_PIN && wps_is_addr_authorized(buf, sm->ownaddr, 1))) {
            /* Found one AP with selected registrar true */
            ap_found = true;
        }

        if (ap_found) {
            if (scan->ssid[1] > SSID_MAX_LEN) {
                wpabuf_free(buf);
                return false;
            }
            esp_wifi_enable_sta_privacy_internal();
            os_memset(sm->creds[0].ssid, 0, SSID_MAX_LEN);
            os_memcpy(sm->creds[0].ssid, (char *)&scan->ssid[2], (int)scan->ssid[1]);
            sm->creds[0].ssid_len = scan->ssid[1];
            if (scan->bssid && memcmp(sm->bssid, scan->bssid, ETH_ALEN) != 0) {
                wpa_printf(MSG_DEBUG, "WPS: BSSID changed from " MACSTR " to " MACSTR,
                           MAC2STR(sm->bssid), MAC2STR(scan->bssid));
                sm->discover_ssid_cnt++;
                wpa_printf(MSG_DEBUG, "WPS: AP discovery count: %d, channel: %d ", sm->discover_ssid_cnt, scan->chan);
                os_memcpy(sm->bssid, scan->bssid, ETH_ALEN);

                scan_uuid = wps_get_uuid_e(buf);
                if (sm->discover_ssid_cnt > 1 && wps_get_type() == WPS_TYPE_PBC && is_wps_pbc_overlap(sm, scan_uuid) == true) {
                    wpa_printf(MSG_WARNING, "WPS: PBC overlap detected");
                    sm->wps_pbc_overlap = true;
                    wpabuf_free(buf);
                    return false;
                }
                if (scan_uuid) {
                    os_memcpy(sm->uuid_r, scan_uuid, WPS_UUID_LEN);
                }

                if (is_ap_supports_sae(scan)) {
                    wpa_printf(MSG_INFO, "WPS: AP supports SAE");
                    sm->dev->config_methods |= WPS_CONFIG_DISPLAY | WPS_CONFIG_VIRT_DISPLAY;
                    sm->wps->wps->config_methods |= WPS_CONFIG_DISPLAY | WPS_CONFIG_VIRT_DISPLAY;
                    /* Reset assoc req, probe reset not needed */
                    wps_build_ic_appie_wps_ar();
                }
            }
            wpabuf_free(buf);
            wpa_printf(MSG_INFO, "WPS AP discovered: %s", (char *)sm->creds[0].ssid);
            sm->channel = scan->chan;

            return true;
        }
        wpabuf_free(buf);
    }

    return false;
}

static int wps_send_eap_identity_rsp(u8 id)
{
    struct wps_sm *sm = wps_sm_get();
    struct wpabuf *eap_buf = NULL;
    int ret = ESP_OK;

    if (!sm) {
        return ESP_FAIL;
    }

    wpa_printf(MSG_DEBUG, "WPS: Sending EAP-Identity Response");
    eap_buf = eap_msg_alloc(EAP_VENDOR_IETF, EAP_TYPE_IDENTITY, sm->identity_len,
                            EAP_CODE_RESPONSE, id);
    if (!eap_buf) {
        wpa_printf(MSG_ERROR, "WPS: EAP buffer allocation failed");
        return ESP_ERR_NO_MEM;
    }

    wpabuf_put_data(eap_buf, sm->identity, sm->identity_len);

    ret = wps_send_eapol_frame(IEEE802_1X_TYPE_EAP_PACKET, wpabuf_head(eap_buf), wpabuf_len(eap_buf));

    wpabuf_free(eap_buf);
    return ret;
}

static int wps_send_frag_ack(u8 id)
{
    struct wps_sm *sm = wps_sm_get();
    struct wpabuf *eap_buf = NULL;
    int ret = ESP_OK;

    wpa_printf(MSG_DEBUG, "WPS: Sending EAP-WSC Frag-ACK (id:%d)", id);

    if (!sm) {
        return ESP_FAIL;
    }

    eap_buf = eap_wsc_build_frag_ack(id, EAP_CODE_RESPONSE);
    if (!eap_buf) {
        return ESP_ERR_NO_MEM;
    }

    ret = wps_send_eapol_frame(IEEE802_1X_TYPE_EAP_PACKET, wpabuf_head(eap_buf), wpabuf_len(eap_buf));

    wpabuf_free(eap_buf);

    return ret;
}

static int wps_enrollee_process_msg_frag(struct wpabuf **buf, int tot_len, u8 *frag_data, int frag_len, u8 flag)
{
    struct wps_sm *sm = wps_sm_get();
    u8 identifier;

    if (!sm || !buf || !frag_data) {
        wpa_printf(MSG_ERROR, "WPS: %s: Invalid arguments", __func__);
        return ESP_FAIL;
    }

    identifier = sm->current_identifier;

    if (*buf == NULL) {
        if (frag_len < 0 || tot_len < frag_len) {
            wpa_printf(MSG_ERROR, "WPS: Invalid first fragment length");
            return ESP_FAIL;
        }

        *buf = wpabuf_alloc(tot_len);
        if (*buf == NULL) {
            return ESP_ERR_NO_MEM;
        }

        wpabuf_put_data(*buf, frag_data, frag_len);
        return wps_send_frag_ack(identifier);
    }

    if (flag & WPS_MSG_FLAG_LEN) {
        wpa_printf(MSG_ERROR, "WPS: %s: Invalid fragment flag: 0x%02x", __func__, flag);
        wpabuf_free(*buf);
        *buf = NULL;
        return ESP_FAIL;
    }

    if (frag_len < 0 || wpabuf_len(*buf) + frag_len > tot_len) {
        wpa_printf(MSG_ERROR, "WPS: Invalid subsequent fragment length");
        wpabuf_free(*buf);
        *buf = NULL;
        return ESP_FAIL;
    }
    wpabuf_put_data(*buf, frag_data, frag_len);

    if (flag & WPS_MSG_FLAG_MORE) {
        return wps_send_frag_ack(identifier);
    }

    return ESP_OK;
}

static void wifi_station_wps_post_m8_timeout(void *data, void *user_ctx)
{
    struct wps_sm *sm = wps_sm_get();

    if (sm && sm->post_m8_recv) {
        wpa_printf(MSG_DEBUG, "WPS: M8 workaround timeout, success!");
        sm->wps->state = WPS_FINISHED;
        /* Reset it here so that deauth won't call wps finish again */
        sm->post_m8_recv = false;
        wps_finish();
    }
};

static int wps_process_wps_mX_req(u8 *ubuf, int len, enum wps_process_res *res)
{
    struct wps_sm *sm = wps_sm_get();
    static struct wpabuf *wps_buf = NULL;
    struct eap_expand *expd;
    int tlen = 0;
    u8 *tbuf;
    u8 flag;
    int frag_len;
    u16 be_tot_len = 0;

    if (!sm || !res) {
        return ESP_FAIL;
    }

    if (len < (int)(sizeof(struct eap_expand) + 1)) {
        wpa_printf(MSG_ERROR, "WPS: Truncated EAP-Expanded header");
        return ESP_FAIL;
    }
    expd = (struct eap_expand *) ubuf;
    wpa_printf(MSG_DEBUG, "WPS: Processing WSC message (len=%d, total_len=%d)", len, tlen);

    if (sm->state == WAIT_START) {
        if (expd->opcode != WSC_Start) {
            wpa_printf(MSG_WARNING, "WPS: Unexpected Op-Code %d in WAIT_START state", expd->opcode);
            return ESP_ERR_INVALID_STATE;
        }
        wpa_printf(MSG_DEBUG, "WPS: Received EAP-WSC Start");
        sm->state = WPA_MESG;
    } else if (expd->opcode == WSC_Start) {
        wpa_printf(MSG_WARNING, "WPS: Unexpected Op-Code %d",
                   expd->opcode);
        return ESP_ERR_INVALID_STATE;
    }

    flag = *(u8 *)(ubuf + sizeof(struct eap_expand));
    if (flag & WPS_MSG_FLAG_LEN) {
        if (len < (int)(sizeof(struct eap_expand) + 1 + 2)) {
            wpa_printf(MSG_ERROR, "WPS: Missing total length field");
            return ESP_FAIL;
        }
        tbuf = ubuf + sizeof(struct eap_expand) + 1 + 2; // includes 2-byte total length
        frag_len = len - (sizeof(struct eap_expand) + 1 + 2);
        be_tot_len = *(u16 *)(ubuf + sizeof(struct eap_expand) + 1);
        tlen = ((be_tot_len & 0xff) << 8) | ((be_tot_len >> 8) & 0xff);
    } else {
        tbuf = ubuf + sizeof(struct eap_expand) + 1;
        frag_len = len - (sizeof(struct eap_expand) + 1);
        tlen = frag_len;
    }

    if (frag_len < 0 || tlen < 0 || ((flag & WPS_MSG_FLAG_LEN) && tlen < frag_len)) {
        wpa_printf(MSG_ERROR, "WPS: Invalid fragment sizes");
        if (wps_buf) {
            wpabuf_free(wps_buf);
            wps_buf = NULL;
        }
        return ESP_FAIL;
    }

    if (tlen > 50000) {
        wpa_printf(MSG_ERROR, "WPS: Invalid EAP-WSC message length");
        return ESP_FAIL;
    }
    if ((flag & WPS_MSG_FLAG_MORE) || wps_buf != NULL) {//frag msg
        wpa_printf(MSG_DEBUG, "WPS: Received fragment (id=%d, flag=0x%x, frag_len=%d, total_len=%d)", sm->current_identifier, flag, frag_len, tlen);
        if (ESP_OK != wps_enrollee_process_msg_frag(&wps_buf, tlen, tbuf, frag_len, flag)) {
            if (wps_buf) {
                wpabuf_free(wps_buf);
                wps_buf = NULL;
            }
            return ESP_FAIL;
        }
        if (flag & WPS_MSG_FLAG_MORE) {
            *res = WPS_FRAGMENT;
            return ESP_OK;
        }
    } else { //not frag msg
        if (wps_buf) {//if something wrong, frag msg buf is not freed, free first
            wpa_printf(MSG_ERROR, "WPS: Fragment buffer not freed before receiving new message");
            wpabuf_free(wps_buf);
            wps_buf = NULL;
        }
        wps_buf = wpabuf_alloc_copy(tbuf, tlen);
    }

    if (!wps_buf) {
        return ESP_FAIL;
    }

    eloop_cancel_timeout(wifi_station_wps_msg_timeout, NULL, NULL);

    *res = wps_enrollee_process_msg(sm->wps, expd->opcode, wps_buf);

    if (*res == WPS_CONTINUE && sm->wps->state == WPS_MSG_DONE) {
        wpa_printf(MSG_DEBUG, "WPS: M8 processed, starting workaround timer");
        sm->post_m8_recv = true;
        eloop_cancel_timeout(wifi_station_wps_post_m8_timeout, NULL, NULL);
        eloop_register_timeout(WPS_POST_M8_WORKAROUND_SECS, 0, wifi_station_wps_post_m8_timeout, NULL, NULL);
    }

    if (*res == WPS_FAILURE) {
        sm->state = WPA_FAIL;
    }

    if (wps_buf) {
        wpabuf_free(wps_buf);
        wps_buf = NULL;
    }
    return ESP_OK;
}

static int wps_send_wps_mX_rsp(u8 id)
{
    struct wps_sm *sm = wps_sm_get();
    struct wpabuf *eap_buf = NULL;
    struct wpabuf *wps_buf = NULL;
    int ret = ESP_OK;
    enum wsc_op_code opcode;

    wpa_printf(MSG_DEBUG, "WPS: Sending WSC Message");

    if (!sm) {
        return ESP_FAIL;
    }

    wps_buf = wps_enrollee_get_msg(sm->wps, &opcode);
    if (!wps_buf) {
        return ESP_FAIL;
    }

    eap_buf = eap_msg_alloc(EAP_VENDOR_WFA, 0x00000001, wpabuf_len(wps_buf) + 2, EAP_CODE_RESPONSE, id);
    if (!eap_buf) {
        wpabuf_free(wps_buf);
        return ESP_ERR_NO_MEM;
    }

    wpabuf_put_u8(eap_buf, opcode);
    wpabuf_put_u8(eap_buf, 0x00); /* flags */
    wpabuf_put_data(eap_buf, wpabuf_head(wps_buf), wpabuf_len(wps_buf));
    wpabuf_free(wps_buf);

    ret = wps_send_eapol_frame(IEEE802_1X_TYPE_EAP_PACKET, wpabuf_head(eap_buf), wpabuf_len(eap_buf));

    wpabuf_free(eap_buf);
    return ret;
}

static int wps_tx_start(void)
{
    wpa_printf(MSG_DEBUG,  "WPS: Sending EAPOL-Start");

    if (wps_send_eapol_frame(IEEE802_1X_TYPE_EAPOL_START, (u8 *)"", 0) != ESP_OK) {
        return ESP_FAIL;
    }

    eloop_cancel_timeout(wifi_station_wps_eapol_start_handle, NULL, NULL);
    eloop_register_timeout(EAPOL_START_HANDLE_TIMEOUT_SECS, 0, wifi_station_wps_eapol_start_handle, NULL, NULL);

    return ESP_OK;
}

static int wps_start_pending(void)
{
    if (!wps_sm_get()) {
        return ESP_FAIL;
    }

    wpa_printf(MSG_DEBUG, "WPS: Pending start, sending EAPOL-Start");
    return wps_tx_start();
}

static int wps_send_event_and_disable(int32_t event_id, void* event_data, size_t data_len)
{
    if (gWpsSm == NULL) {
        return 0;
    }

#ifdef CONFIG_WPS_RECONNECT_ON_FAIL
    /* On failure, reconnect to the previous AP if one was saved */
    if (s_previous_wifi_config) {
        wpa_printf(MSG_INFO, "WPS failed, reconnecting to previous AP: %s", (char *)s_previous_wifi_config->sta.ssid);
        esp_wifi_set_config(WIFI_IF_STA, s_previous_wifi_config);
        esp_wifi_connect();
        os_free(s_previous_wifi_config);
        s_previous_wifi_config = NULL;
    }
#endif

    esp_event_post(WIFI_EVENT, event_id, event_data, data_len, OS_BLOCK);

    wps_set_type(WPS_TYPE_DISABLE);
    wifi_wps_disable_internal(NULL, NULL);

    API_MUTEX_TAKE();
    s_wps_enabled = false;
    API_MUTEX_GIVE();

    return 0;
}

static int wps_handle_failure(wifi_event_sta_wps_fail_reason_t reason_code)
{
    struct wps_sm *sm = wps_sm_get();

    if (!sm) {
        return ESP_FAIL;
    }

    wpa_printf(MSG_INFO, "WPS process stopping.");

    /* Disconnect from AP before disabling WPS */
    esp_wifi_disconnect();

    wps_send_event_and_disable(WIFI_EVENT_STA_WPS_ER_FAILED, &reason_code, sizeof(reason_code));

    return ESP_OK;
}

static void wifi_station_fill_event_info(struct wps_sm *sm, wifi_event_sta_wps_er_success_t *evt)
{
    int i;

    os_memset(evt, 0, sizeof(*evt));
    if (sm && sm->ap_cred_cnt > 1) {
        evt->ap_cred_cnt = sm->ap_cred_cnt < MAX_WPS_AP_CRED ? sm->ap_cred_cnt : MAX_WPS_AP_CRED;
        for (i = 0; i < evt->ap_cred_cnt; i++) {
            os_memcpy(evt->ap_cred[i].ssid, sm->creds[i].ssid, sm->creds[i].ssid_len);
            os_memcpy(evt->ap_cred[i].passphrase, sm->creds[i].key, sm->creds[i].key_len);
        }
    }
}

static int wps_finish(void)
{
    struct wps_sm *sm = wps_sm_get();
    int ret = ESP_FAIL;

    if (!sm) {
        return ESP_FAIL;
    }

    if (sm->wps->state == WPS_FINISHED) {
        if (sm->state == WPA_FINISH_PROCESS) {
            return ESP_OK;
        }
        sm->state = WPA_FINISH_PROCESS;
        bool connect = (sm->ap_cred_cnt == 1);

#ifdef CONFIG_WPS_RECONNECT_ON_FAIL
        if (s_previous_wifi_config) {
            os_free(s_previous_wifi_config);
            s_previous_wifi_config = NULL;
        }
#endif

        wpa_printf(MSG_INFO, "WPS: Succeeded");
        wps_set_status(WPS_STATUS_SUCCESS);
        /* WPS finished, dequeue all timers */
        wps_delete_timer();

        esp_wifi_unset_appie_internal(WIFI_APPIE_WPS_PR);
        esp_wifi_unset_appie_internal(WIFI_APPIE_WPS_AR);
        esp_wifi_set_wps_cb_internal(NULL);

        if (sm->ap_cred_cnt == 1) {
            wifi_config_t *config = os_zalloc(sizeof(wifi_config_t));

            if (!config) {
                return ESP_FAIL;
            }

            esp_wifi_get_config(WIFI_IF_STA, config);
            os_memcpy(config->sta.ssid, sm->creds[0].ssid, sm->creds[0].ssid_len);
            os_memcpy(config->sta.password, sm->creds[0].key, sm->creds[0].key_len);
#ifndef CONFIG_WPS_STRICT
            /* Some APs support AES in WPA IE, enable connection with them */
            if (sm->creds[0].auth_type == WPS_AUTH_WPAPSK &&
                    (sm->creds[0].encr_type & WPS_ENCR_AES)) {
                config->sta.threshold.authmode = WIFI_AUTH_WPA_PSK;
            }
#endif
            config->sta.bssid_set = 0;
            config->sta.sae_pwe_h2e = 0;
            esp_wifi_disconnect();
            esp_wifi_set_config(WIFI_IF_STA, config);
            os_free(config);
        }

        /* fill event info */
        wifi_station_fill_event_info(sm, &s_wps_success_evt);

        /* Disable WPS when success */
        wps_set_type(WPS_TYPE_DISABLE);
        wifi_wps_disable_internal(NULL, NULL);

        if (connect) {
            esp_wifi_connect();
        }

        /* wait for 1 sec before sending WPS success event to give connection time */
        /* TODO: Remove disconnection event during WPS session to make it cleaner */
        eloop_cancel_timeout(wifi_station_wps_success, NULL, NULL);
        eloop_register_timeout(WPS_SUCCESS_EVENT_TIMEOUT_SECS, 0, wifi_station_wps_success, NULL, NULL);

        ret = 0;
    } else {
        wpa_printf(MSG_ERROR, "WPS failed with state %d", sm->wps->state);
        ret = wps_handle_failure(WPS_FAIL_REASON_NORMAL);
    }

    API_MUTEX_TAKE();
    s_wps_enabled = false;
    API_MUTEX_GIVE();

    return ret;
}

/* This will get executed in the wifi task's context */
static void wps_sm_notify_deauth(void)
{
    struct wps_sm *sm = wps_sm_get();

    if (sm && sm->post_m8_recv) {
        wpa_printf(MSG_DEBUG, "WPS: deauth after M8, considering success");
        eloop_cancel_timeout(wifi_station_wps_post_m8_timeout, NULL, NULL);
        sm->wps->state = WPS_FINISHED;
        wps_finish();
        return;
    }
    if (sm && sm->wps && sm->wps->state != WPS_FINISHED) {
        wpa_printf(MSG_ERROR, "WPS: Deauthenticated during handshake");
        wps_handle_failure(WPS_FAIL_REASON_RECV_DEAUTH);
    }
}

int wps_start_msg_timer(void)
{
    struct wps_sm *sm = wps_sm_get();
    uint32_t msg_timeout;
    int ret = ESP_FAIL;

    if (!sm) {
        return ESP_FAIL;
    }

    if (sm->wps->state == WPS_FINISHED) {
        msg_timeout = WPS_MSG_TIMEOUT_FINISH_SECS;
        wpa_printf(MSG_DEBUG, "WPS: Starting M-Done timer (%u s)", msg_timeout);
        eloop_cancel_timeout(wifi_station_wps_msg_timeout, NULL, NULL);
        eloop_register_timeout(msg_timeout, 0, wifi_station_wps_msg_timeout, NULL, NULL);
        ret = 0;
    } else if (sm->wps->state == RECV_M2) {
        msg_timeout = WPS_MSG_TIMEOUT_M2_SECS;
        wpa_printf(MSG_DEBUG, "WPS: Starting M2 timer (%u s)", msg_timeout);
        eloop_cancel_timeout(wifi_station_wps_msg_timeout, NULL, NULL);
        eloop_register_timeout(msg_timeout, 0, wifi_station_wps_msg_timeout, NULL, NULL);
        ret = 0;
    }
    return ret;
}

/**
 * wps_sm_rx_eapol - Process received WPA EAPOL frames
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @src_addr: Source MAC address of the EAPOL packet
 * @buf: Pointer to the beginning of the EAPOL data (EAPOL header)
 * @len: Length of the EAPOL frame
 * Returns: 1 = WPA EAPOL-Key processed, ESP_OK = not a WPA EAPOL-Key, ESP_FAIL failure
 *
 * This function is called for each received EAPOL frame. Other than EAPOL-Key
 * frames can be skipped if filtering is done elsewhere. wpa_sm_rx_eapol() is
 * only processing WPA and WPA2 EAPOL-Key frames.
 *
 * The received EAPOL-Key packets are validated and valid packets are replied
 * to. In addition, key material (PTK, GTK) is configured at the end of a
 * successful key handshake.
 * buf begin from version, so remove mac header ,snap header and ether_type
 */
static int wps_sm_rx_eapol(u8 *src_addr, u8 *buf, u32 len)
{
    struct wps_sm *sm = wps_sm_get();
    u32 plen, data_len, eap_len;
    struct ieee802_1x_hdr *hdr;
    struct eap_hdr *ehdr;
    u8 *tmp;
    u8 eap_code;
    u8 eap_type;
    int ret = ESP_FAIL;
    enum wps_process_res res = WPS_DONE;

    if (!sm) {
        return ESP_FAIL;
    }

    if (len < sizeof(*hdr) + sizeof(*ehdr)) {
        wpa_printf(MSG_DEBUG,  "WPS: EAPOL frame too short (len=%lu)",
                   (unsigned long) len);
        return ESP_OK;
    }

    tmp = buf;

    hdr = (struct ieee802_1x_hdr *) tmp;
    ehdr = (struct eap_hdr *)(hdr + 1);
    plen = be_to_host16(hdr->length);
    data_len = plen + sizeof(*hdr);
    eap_len = be_to_host16(ehdr->length);

    wpa_printf(MSG_DEBUG, "WPS: RX IEEE 802.1X: version=%d type=%d length=%u", hdr->version, hdr->type, plen);

    if (hdr->version < EAPOL_VERSION) {
        /* TODO: backwards compatibility */
    }
    if (hdr->type != IEEE802_1X_TYPE_EAP_PACKET) {
        wpa_printf(MSG_DEBUG, "WPS: Discarding non-EAP-PACKET frame (type %u)", hdr->type);
        ret = 0;
        goto out;
    }
    if (plen > len - sizeof(*hdr) || plen < sizeof(*ehdr)) {
        wpa_printf(MSG_DEBUG, "WPS: Invalid EAPOL payload size %lu (frame size %lu)",
                   (unsigned long) plen, (unsigned long) len);
        ret = 0;
        goto out;
    }

    wpa_hexdump(MSG_MSGDUMP, "WPS: RX EAPOL-EAP Packet", tmp, len);

    if (data_len < len) {
        wpa_printf(MSG_DEBUG, "WPS: Ignoring %lu extra bytes in EAPOL frame", (unsigned long) len - data_len);
    }

    if (eap_len != plen) {
        wpa_printf(MSG_DEBUG, "WPS: Invalid EAP length %lu in EAPOL frame (total payload %lu)",
                   (unsigned long) eap_len, (unsigned long) plen);
        ret = 0;
        goto out;
    }

    eap_code = ehdr->code;
    switch (eap_code) {
    case EAP_CODE_SUCCESS:
        wpa_printf(MSG_WARNING, "WPS: Unexpected EAP-Success frame received");
        ret = 0;
        break;
    case EAP_CODE_FAILURE:
        wpa_printf(MSG_INFO, "WPS: Received EAP-Failure frame.");
        ret = wps_finish();
        break;
    case EAP_CODE_RESPONSE:
        wpa_printf(MSG_WARNING, "WPS: Unexpected EAP-Response frame received");
        ret = 0;
        break;
    case EAP_CODE_REQUEST:
        eap_type = ((u8 *)ehdr)[sizeof(*ehdr)];
        switch (eap_type) {
        case EAP_TYPE_IDENTITY:
            wpa_printf(MSG_DEBUG, "WPS: Received EAP-Request-Identity");
            sm->current_identifier = ehdr->identifier;
            eloop_cancel_timeout(wifi_station_wps_eapol_start_handle, NULL, NULL);
            wpa_printf(MSG_DEBUG,  "WPS: Building EAP-Identity Response.");
            ret = wps_send_eap_identity_rsp(ehdr->identifier);
            eloop_register_timeout(EAPOL_START_HANDLE_TIMEOUT_SECS, 0, wifi_station_wps_eapol_start_handle, NULL, NULL);
            break;
        case EAP_TYPE_EXPANDED:
            wpa_printf(MSG_DEBUG, "WPS: Received EAP-WSC message (len=%u)", plen);
            sm->current_identifier = ehdr->identifier;

            tmp = (u8 *)(ehdr + 1) + 1;
            ret = wps_process_wps_mX_req(tmp, plen - sizeof(*ehdr) - 1, &res);
            if (res == WPS_FRAGMENT) {
                wpa_printf(MSG_DEBUG, "WPS fragment received, waiting for more.");
                ret = ESP_OK;
                break;
            }
            if (ret == ESP_OK && res != WPS_FAILURE) {
                ret = wps_send_wps_mX_rsp(ehdr->identifier);

                if (ret == ESP_OK) {
                    wpa_printf(MSG_DEBUG, "WPS: State updated to %d", sm->wps->state);
                    wps_start_msg_timer();
                }
            } else if (ret == ESP_ERR_INVALID_STATE) {
                ret = ESP_OK;
            } else {
                ret = ESP_FAIL;
            }
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
out:
    if ((ret != 0 || res == WPS_FAILURE)) {
        wpa_printf(MSG_ERROR, "WPS: EAPOL processing failed (ret=%d)", ret);
        wps_handle_failure(WPS_FAIL_REASON_NORMAL);

        return ESP_FAIL;
    }

    return ret;
}

static int wps_set_default_factory(void)
{
    if (!s_factory_info) {
        s_factory_info = os_zalloc(sizeof(wps_factory_information_t));
        if (!s_factory_info) {
            wpa_printf(MSG_ERROR, "WPS: Failed to allocate memory for factory info");
            return ESP_ERR_NO_MEM;
        }
    }

    os_snprintf(s_factory_info->manufacturer, WPS_MAX_MANUFACTURER_LEN, "ESPRESSIF");
    os_snprintf(s_factory_info->model_name, WPS_MAX_MODEL_NUMBER_LEN, "ESPRESSIF IOT");
    os_snprintf(s_factory_info->model_number, WPS_MAX_MODEL_NAME_LEN, wps_model_number);
    os_snprintf(s_factory_info->device_name, WPS_MAX_DEVICE_NAME_LEN, "%s STATION", wps_model_number);

    return ESP_OK;
}

int wps_set_factory_info(const esp_wps_config_t *config)
{
    int ret;

    ret = wps_set_default_factory();
    if (ret != 0) {
        return ret;
    }

    if (config->factory_info.manufacturer[0] != 0) {
        os_memcpy(s_factory_info->manufacturer, config->factory_info.manufacturer, WPS_MAX_MANUFACTURER_LEN - 1);
    }

    if (config->factory_info.model_number[0] != 0) {
        os_memcpy(s_factory_info->model_number, config->factory_info.model_number, WPS_MAX_MODEL_NUMBER_LEN - 1);
    }

    if (config->factory_info.model_name[0] != 0) {
        os_memcpy(s_factory_info->model_name, config->factory_info.model_name, WPS_MAX_MODEL_NAME_LEN - 1);
    }

    if (config->factory_info.device_name[0] != 0) {
        os_memcpy(s_factory_info->device_name, config->factory_info.device_name, WPS_MAX_DEVICE_NAME_LEN - 1);
    }

    wpa_printf(MSG_INFO, "WPS: Manufacturer: %s, Model: %s, Model-Num: %s, Device: %s", s_factory_info->manufacturer,
               s_factory_info->model_number, s_factory_info->model_name, s_factory_info->device_name);

    return ESP_OK;
}

int wps_dev_init(void)
{
    int ret = 0;
    struct wps_sm *sm = wps_sm_get();
    struct wps_device_data *dev = NULL;

    if (!sm) {
        ret = ESP_FAIL;
        goto _out;
    }

    dev = &sm->wps_ctx->dev;
    sm->dev = dev;

    if (!dev) {
        ret = ESP_FAIL;
        goto _out;
    }
    dev->config_methods = WPS_CONFIG_VIRT_PUSHBUTTON;
#ifdef CONFIG_ESP_WIFI_WPS_PASSPHRASE
    dev->config_methods |= WPS_CONFIG_DISPLAY | WPS_CONFIG_VIRT_DISPLAY;
#endif
    dev->rf_bands = WPS_RF_24GHZ;

    WPA_PUT_BE16(dev->pri_dev_type, WPS_DEV_PHONE);
    WPA_PUT_BE32(dev->pri_dev_type + 2, WPS_DEV_OUI_WFA);
    WPA_PUT_BE16(dev->pri_dev_type + 6, WPS_DEV_PHONE_SINGLE_MODE);

    if (!s_factory_info) {
        ret = wps_set_default_factory();
        if (ret != 0) {
            goto _out;
        }
    }

    dev->manufacturer = os_zalloc(WPS_MAX_MANUFACTURER_LEN);
    if (!dev->manufacturer) {
        ret = ESP_FAIL;
        goto _out;
    }
    os_snprintf(dev->manufacturer, WPS_MAX_MANUFACTURER_LEN, s_factory_info->manufacturer);

    dev->model_name = os_zalloc(WPS_MAX_MODEL_NAME_LEN);
    if (!dev->model_name) {
        ret = ESP_FAIL;
        goto _out;
    }
    os_snprintf(dev->model_name, WPS_MAX_MODEL_NAME_LEN, s_factory_info->model_name);

    dev->model_number = os_zalloc(WPS_MAX_MODEL_NAME_LEN);
    if (!dev->model_number) {
        ret = ESP_FAIL;
        goto _out;
    }
    os_snprintf(dev->model_number, WPS_MAX_MODEL_NAME_LEN, s_factory_info->model_number);

    dev->device_name = os_zalloc(WPS_MAX_DEVICE_NAME_LEN);
    if (!dev->device_name) {
        ret = ESP_FAIL;
        goto _out;
    }
    os_snprintf(dev->device_name, WPS_MAX_DEVICE_NAME_LEN, s_factory_info->device_name);

    dev->serial_number = os_zalloc(16);
    if (!dev->serial_number) {
        ret = ESP_FAIL;
        goto _out;
    }
    os_snprintf(dev->serial_number, 16, "%02x%02x%02x%02x%02x%02x",
                sm->ownaddr[0], sm->ownaddr[1], sm->ownaddr[2],
                sm->ownaddr[3], sm->ownaddr[4], sm->ownaddr[5]);

    uuid_gen_mac_addr(sm->ownaddr, sm->uuid);
    os_memcpy(dev->mac_addr, sm->ownaddr, ETH_ALEN);

    return ESP_OK;

_out:
    wps_dev_deinit(dev);
    return ret;
}

int wps_dev_deinit(struct wps_device_data *dev)
{
    if (!dev) {
        return ESP_FAIL;
    }
    wps_device_data_free(dev);

    if (s_factory_info) {
        os_free(s_factory_info);
        s_factory_info = NULL;
    }

    return ESP_OK;
}

static void wifi_station_wps_timeout(void *data, void *user_ctx)
{
    struct wps_sm *sm = wps_sm_get();
    if (!sm) {
        return;
    }
    wpa_printf(MSG_ERROR, "WPS overall timeout!");

    wps_send_event_and_disable(WIFI_EVENT_STA_WPS_ER_TIMEOUT, 0, 0);
}

static void wifi_station_wps_msg_timeout(void *data, void *user_ctx)
{
    struct wps_sm *sm = wps_sm_get();
    if (!sm) {
        return;
    }

    if (sm->wps->state == WPS_FINISHED) {
        wpa_printf(MSG_DEBUG, "WPS: M-Done timer expired, finishing.");
        wps_finish();
        return;
    } else if (sm->wps->state == RECV_M2) {
        wpa_printf(MSG_ERROR, "WPS: Timed out waiting for M2/M2D");
        wps_handle_failure(WPS_FAIL_REASON_RECV_M2D);
    }
}

static void wifi_station_wps_success(void *data, void *user_ctx)
{
    /*
     * For only one AP credential don't send event data, wps_finish() has already set
     * the config. This is for backward compatibility.
     */
    if (s_wps_success_evt.ap_cred_cnt > 1) {
        esp_event_post(WIFI_EVENT, WIFI_EVENT_STA_WPS_ER_SUCCESS, &s_wps_success_evt,
                       sizeof(s_wps_success_evt), OS_BLOCK);
    } else {
        esp_event_post(WIFI_EVENT, WIFI_EVENT_STA_WPS_ER_SUCCESS,
                       0, 0, OS_BLOCK);
    }
    os_memset(&s_wps_success_evt, 0, sizeof(s_wps_success_evt));
}

void wifi_station_wps_eapol_start_handle(void *data, void *user_ctx)
{
    wpa_printf(MSG_DEBUG, "WPS: Resending EAPOL-Start");
    wps_tx_start();
}

static int save_credentials_cb(void *ctx, const struct wps_credential *cred)
{
    struct wps_sm *sm = wps_sm_get();
    struct wps_credential *creds;
    if (!sm || !cred || sm->ap_cred_cnt >= MAX_CRED_COUNT) {
        return ESP_FAIL;
    }

    creds = &sm->creds[sm->ap_cred_cnt];
    memcpy(creds, cred, sizeof(*creds));

    sm->ap_cred_cnt++;

    wpa_hexdump_ascii(MSG_DEBUG, "WPS: Received credential - SSID ", cred->ssid, cred->ssid_len);
    wpa_hexdump_ascii(MSG_DEBUG, "WPS: Received credential - Key ", cred->key, cred->key_len);

    return ESP_OK;
}

int wps_init_cfg_pin(struct wps_config *cfg)
{
    if (wps_get_type() != WPS_TYPE_PIN) {
        cfg->pbc = 1;
        return 0;
    }

    cfg->pbc = 0;

    if ((os_strncmp((char *)cfg->pin, "00000000", 8) == 0) || !wps_pin_str_valid((char *)cfg->pin)) {
        unsigned int spin = 0;

        cfg->dev_pw_id = DEV_PW_DEFAULT;
        cfg->pin_len = 8;
        if (wps_generate_pin(&spin) < 0) {
            return -1;
        }
        wpa_printf(MSG_INFO, "WPS: Invalid PIN provided, generated new PIN: %08d", spin);
        os_snprintf((char *)cfg->pin, 9, "%08d", spin);
    }

    return 0;
}

struct wps_sm_funcs* wps_get_wps_sm_cb(void)
{
    return s_wps_sm_cb;
}

static int wifi_station_wps_init(const esp_wps_config_t *config)
{
    struct wps_funcs *wps_cb;
    struct wps_sm *sm = NULL;
    struct wps_config cfg = {0};

    if (gWpsSm) {
        goto _out;
    }

    wpa_printf(MSG_DEBUG, "WPS: Initializing");

    gWpsSm = os_zalloc(sizeof(struct wps_sm));   /* alloc Wps_sm */
    if (!gWpsSm) {
        goto _out;
    }

    sm = gWpsSm;
    esp_wifi_get_macaddr_internal(WIFI_IF_STA, sm->ownaddr);
    os_memcpy(gWpaSm.own_addr, sm->ownaddr, ETH_ALEN);

    sm->identity_len = WSC_ID_ENROLLEE_LEN;
    os_memcpy(sm->identity, WSC_ID_ENROLLEE, sm->identity_len);

    sm->wps_ctx = os_zalloc(sizeof(struct wps_context)); /* alloc wps_ctx */
    if (!sm->wps_ctx) {
        goto _err;
    }

    if (wps_dev_init() != 0) {
        goto _err;
    }

    cfg.wps = sm->wps_ctx;

    os_memcpy((void *)cfg.pin, config->pin, 8);
    if (wps_init_cfg_pin(&cfg) < 0) {
        goto _err;
    }

    os_memcpy(cfg.wps->uuid, sm->uuid, WPS_UUID_LEN);
    cfg.wps->config_methods = sm->wps_ctx->dev.config_methods;
    if ((sm->wps = wps_init(&cfg)) == NULL) {         /* alloc wps_data */
        goto _err;
    }

    /* Report PIN */
    if (wps_get_type() == WPS_TYPE_PIN) {
        wifi_event_sta_wps_er_pin_t evt;
        os_memcpy(evt.pin_code, sm->wps->dev_password, 8);
        esp_event_post(WIFI_EVENT, WIFI_EVENT_STA_WPS_ER_PIN, &evt, sizeof(evt), OS_BLOCK);
    }

    sm->wps->wps->cred_cb = save_credentials_cb;

    if (esp_wifi_get_appie_internal(WIFI_APPIE_WPS_PR) == NULL) {
        wps_build_ic_appie_wps_pr();
    }

    if (esp_wifi_get_appie_internal(WIFI_APPIE_WPS_AR) == NULL) {
        wps_build_ic_appie_wps_ar();
    }

    /* Cancel any pending timers from previous WPS attempts */
    wps_delete_timer();

    wps_cb = os_malloc(sizeof(struct wps_funcs));
    if (wps_cb == NULL) {
        goto _err;
    }
    wps_cb->wps_parse_scan_result = wps_parse_scan_result;
    wps_cb->wifi_station_wps_start = NULL;
    wps_cb->wps_sm_rx_eapol = wps_sm_rx_eapol;
    wps_cb->wps_start_pending = wps_start_pending;
    esp_wifi_set_wps_cb_internal(wps_cb);

    s_wps_sm_cb = os_zalloc(sizeof(struct wps_sm_funcs));
    if (!s_wps_sm_cb) {
        goto _err;
    }
    os_memset(&s_wps_success_evt, 0, sizeof(s_wps_success_evt));

    return ESP_OK;

_err:
    esp_wifi_unset_appie_internal(WIFI_APPIE_WPS_PR);
    esp_wifi_unset_appie_internal(WIFI_APPIE_WPS_AR);

    if (sm->dev) {
        wps_dev_deinit(sm->dev);
        sm->dev = NULL;
    }
    if (sm->wps_ctx) {
        if (sm->wps_ctx->dh_privkey) {
            wpabuf_free(sm->wps_ctx->dh_privkey);
        }
        os_free(sm->wps_ctx);
        sm->wps_ctx = NULL;
    }
    if (sm->wps) {
        wps_deinit(sm->wps);
        sm->wps = NULL;
    }
    os_free(gWpsSm);
    gWpsSm = NULL;
    return ESP_FAIL;
_out:
    return ESP_FAIL;
}

static int wps_delete_timer(void)
{
    struct wps_sm *sm = wps_sm_get();

    if (!sm) {
        return ESP_OK;
    }

    eloop_cancel_timeout(wifi_station_wps_success, NULL, NULL);
    eloop_cancel_timeout(wifi_station_wps_timeout, NULL, NULL);
    eloop_cancel_timeout(wifi_station_wps_msg_timeout, NULL, NULL);
    eloop_cancel_timeout(wifi_wps_scan, NULL, NULL);
    eloop_cancel_timeout(wifi_station_wps_eapol_start_handle, NULL, NULL);
    eloop_cancel_timeout(wifi_station_wps_post_m8_timeout, NULL, NULL);
    esp_wifi_disarm_sta_connection_timer_internal();
    return ESP_OK;
}

static int wifi_station_wps_deinit(void)
{
    struct wps_sm *sm = gWpsSm;

    if (gWpsSm == NULL) {
        return ESP_FAIL;
    }

    esp_wifi_unset_appie_internal(WIFI_APPIE_WPS_PR);
    esp_wifi_unset_appie_internal(WIFI_APPIE_WPS_AR);
    esp_wifi_set_wps_cb_internal(NULL);

    if (sm->dev) {
        wps_dev_deinit(sm->dev);
        sm->dev = NULL;
    }
    if (sm->wps_ctx) {
        if (sm->wps_ctx->dh_privkey) {
            wpabuf_free(sm->wps_ctx->dh_privkey);
        }
        os_free(sm->wps_ctx);
        sm->wps_ctx = NULL;
    }
    if (sm->wps) {
        wps_deinit(sm->wps);
        sm->wps = NULL;
    }
    if (s_wps_sm_cb) {
        s_wps_sm_cb->wps_sm_notify_deauth = NULL;
        os_free(s_wps_sm_cb);
        s_wps_sm_cb = NULL;
    }
    os_free(gWpsSm);
    gWpsSm = NULL;

    return ESP_OK;
}

struct wps_sm *wps_sm_get(void)
{
    return gWpsSm;
}

static void wifi_wps_scan_done(void *arg, ETS_STATUS status)
{
    struct wps_sm *sm = wps_sm_get();
    wifi_config_t wifi_config = {0};

    wpa_printf(MSG_INFO, "WPS: scan done");
    if (wps_get_type() == WPS_TYPE_DISABLE) {
        return;
    }

    if (!sm) {
        return;
    }

    if (sm->discover_ssid_cnt == 1) {
        wps_set_status(WPS_STATUS_PENDING);
    } else if (sm->discover_ssid_cnt == 0)  {
        wps_set_status(WPS_STATUS_SCANNING);
    } else {
        if (sm->wps_pbc_overlap) {
            sm->wps_pbc_overlap = false;
            wpa_printf(MSG_ERROR, "WPS: PBC session overlap detected!");
            wps_send_event_and_disable(WIFI_EVENT_STA_WPS_ER_PBC_OVERLAP, 0, 0);
            return; /* WPS is disabled, no further processing */
        } else {
            wps_set_status(WPS_STATUS_PENDING);
        }
    }

    wpa_printf(MSG_DEBUG, "WPS: Scan done, %d APs discovered.", sm->discover_ssid_cnt);

    sm->discover_ssid_cnt = 0;

    if (wps_get_status() == WPS_STATUS_PENDING) {

        os_memcpy(wifi_config.sta.bssid, sm->bssid, ETH_ALEN);
        os_memcpy(wifi_config.sta.ssid, (char *)sm->creds[0].ssid, sm->creds[0].ssid_len);
        wifi_config.sta.bssid_set = 1;
        wifi_config.sta.channel = sm->channel;
        wpa_printf(MSG_INFO, "WPS: connecting to %s, bssid=" MACSTR,
                   (char *)sm->creds[0].ssid, MAC2STR(wifi_config.sta.bssid));
        esp_wifi_set_config(0, &wifi_config);

        wpa_printf(MSG_DEBUG, "WPS: Starting negotiation");
        wifi_config.sta.failure_retry_cnt = 2;
        esp_wifi_connect();
        sm->state = WAIT_START;
        eloop_cancel_timeout(wifi_station_wps_msg_timeout, NULL, NULL);
        eloop_register_timeout(WPS_CONNECT_TIMEOUT_SECS, 0, wifi_station_wps_msg_timeout, NULL, NULL);
    } else if (wps_get_status() == WPS_STATUS_SCANNING) {
        eloop_cancel_timeout(wifi_wps_scan, NULL, NULL);
        eloop_register_timeout(WPS_SCAN_RETRY_TIMEOUT_SECS, 0, wifi_wps_scan, NULL, NULL);
    } else {
        return;
    }
}

static void wifi_wps_scan(void *data, void *user_ctx)
{
    struct wps_sm *sm = wps_sm_get();

    if (!sm) {
        return;
    }

    wpa_printf(MSG_DEBUG, "WPS: Starting scan");

    typedef void (* scan_done_cb_t)(void *arg, ETS_STATUS status);
    extern int esp_wifi_promiscuous_scan_start(wifi_scan_config_t *config, scan_done_cb_t cb);
    esp_wifi_promiscuous_scan_start(NULL, wifi_wps_scan_done);
}

static int wps_rf_band_cb(void *ctx)
{
    wifi_band_mode_t band_mode;
    esp_err_t ret = esp_wifi_get_band_mode(&band_mode);

    if (ret != ESP_OK) {
        wpa_printf(MSG_ERROR, "WPS: failed to get band mode");
    }

    switch (band_mode) {
    case WIFI_BAND_MODE_2G_ONLY:
        return WPS_RF_24GHZ;
    case WIFI_BAND_MODE_5G_ONLY:
        return WPS_RF_50GHZ;
    case WIFI_BAND_MODE_AUTO:
        return WPS_RF_24GHZ | WPS_RF_50GHZ;
    default:
        return WPS_RF_24GHZ;
    }
    return WPS_RF_24GHZ;
}

static int wifi_station_wps_start(void *data, void *user_ctx)
{
    struct wps_sm *sm = wps_sm_get();

    if (!sm) {
        wpa_printf(MSG_ERROR, "WPS: Not initialized");
        return ESP_FAIL;
    }

    eloop_cancel_timeout(wifi_station_wps_timeout, NULL, NULL);
    eloop_register_timeout(WPS_TOTAL_TIMEOUT_SECS, 0, wifi_station_wps_timeout, NULL, NULL);

    switch (wps_get_status()) {
    case WPS_STATUS_DISABLE:
        sm->is_wps_scan = true;

        wps_build_public_key(sm->wps, NULL);
        if (sm->wps->wps->dh_privkey) {
            wpabuf_free(sm->wps->wps->dh_privkey);
        }
        sm->wps->wps->dh_privkey = sm->wps->dh_privkey;
        sm->wps->wps->dh_ctx = sm->wps->dh_ctx;
        sm->wps->wps->dh_pubkey = sm->wps->dh_pubkey_e;
        sm->wps->wps->rf_band_cb = wps_rf_band_cb;
        sm->wps->dh_privkey = NULL;
#ifdef CONFIG_WPS_RECONNECT_ON_FAIL
        if (s_previous_wifi_config) {
            os_free(s_previous_wifi_config);
            s_previous_wifi_config = NULL;
        }
        wifi_ap_record_t ap_info;
        /* Check if we are connected to an AP */
        if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
            s_previous_wifi_config = (wifi_config_t *)os_zalloc(sizeof(wifi_config_t));
            if (s_previous_wifi_config) {
                if (esp_wifi_get_config(WIFI_IF_STA, s_previous_wifi_config) != ESP_OK ||
                        s_previous_wifi_config->sta.ssid[0] == 0) {
                    os_free(s_previous_wifi_config);
                    s_previous_wifi_config = NULL;
                } else {
                    wpa_printf(MSG_INFO, "WPS: Stored previous AP to reconnect on failure: %s",
                               (char *)s_previous_wifi_config->sta.ssid);
                }
            }
        }
#endif
        esp_wifi_disconnect();
        wifi_wps_scan(NULL, NULL);
        if (s_wps_sm_cb) {
            s_wps_sm_cb->wps_sm_notify_deauth = wps_sm_notify_deauth;
        }
        break;
    case WPS_STATUS_SCANNING:
        eloop_cancel_timeout(wifi_station_wps_timeout, NULL, NULL);
        eloop_register_timeout(WPS_TOTAL_TIMEOUT_SECS, 0, wifi_station_wps_timeout, NULL, NULL);
        break;
    default:
        break;
    }
    os_memset(sm->uuid_r, 0, sizeof(sm->uuid_r));
    sm->wps_pbc_overlap = false;
    esp_wifi_set_wps_start_flag_internal(true);
    return ESP_OK;
}

static int wps_check_wifi_mode(void)
{
    wifi_mode_t mode;
    int ret;

    ret = esp_wifi_get_mode(&mode);
    if (ESP_OK != ret) {
        wpa_printf(MSG_ERROR, "WPS: Failed to get Wi-Fi mode (ret=%d)", ret);
        return ESP_FAIL;
    }

    if (
#ifdef CONFIG_ESP_WIFI_SOFTAP_SUPPORT
        mode == WIFI_MODE_AP ||
#endif
        mode == WIFI_MODE_NULL) {
        wpa_printf(MSG_ERROR, "WPS: Invalid Wi-Fi mode for WPS: %d ", mode);
        return ESP_ERR_WIFI_MODE;
    }

    return ESP_OK;
}

int esp_wifi_wps_enable(const esp_wps_config_t *config)
{
    int ret = ESP_OK;
    struct wps_sm *sm = gWpsSm;

    if (esp_wifi_get_user_init_flag_internal() == 0) {
        wpa_printf(MSG_ERROR, "WPS: Wi-Fi not started, cannot enable");
        return ESP_ERR_WIFI_STATE;
    }

    if (ESP_OK != wps_check_wifi_mode()) {
        return ESP_ERR_WIFI_MODE;
    }

    if (is_dpp_enabled()) {
        wpa_printf(MSG_ERROR, "WPS: Cannot enable, DPP is already initialized");
        return ESP_FAIL;
    }

    API_MUTEX_TAKE();
    if (s_wps_enabled) {
        if (sm && os_memcmp(sm->identity, WSC_ID_REGISTRAR, sm->identity_len) == 0) {
            wpa_printf(MSG_ERROR, "WPS: Cannot enable Enrollee, Registrar is already enabled");
            ret = ESP_ERR_WIFI_MODE;
        } else {
            wpa_printf(MSG_DEBUG, "WPS: Already enabled");
        }
        API_MUTEX_GIVE();
        return ret;
    }

    ret = eloop_register_timeout_blocking(wifi_wps_enable_internal, NULL, (void *)config);
    s_wps_enabled = true;
    API_MUTEX_GIVE();
    return ret;
}

bool is_wps_enabled(void)
{
    bool enabled;

    API_MUTEX_TAKE();
    enabled = s_wps_enabled;
    API_MUTEX_GIVE();

    return enabled;
}

static int wifi_wps_disable(void)
{
    wps_set_type(WPS_TYPE_DISABLE);
    wifi_wps_disable_internal(NULL, NULL);

    return 0;
}

static int wifi_wps_enable_internal(void *ctx, void *data)
{
    const esp_wps_config_t *config = (const esp_wps_config_t *)data;
    int ret = 0;
    struct wpa_sm *wpa_sm = &gWpaSm;

    if (config->wps_type == WPS_TYPE_DISABLE) {
        wpa_printf(MSG_ERROR, "WPS: Invalid WPS type for enabling");
        return ESP_ERR_WIFI_WPS_TYPE;
    }
    wpa_printf(MSG_DEBUG, "WPS: Setting factory information");
    ret = wps_set_factory_info(config);
    if (ret != 0) {
        return ret;
    }

    wpa_printf(MSG_INFO, "WPS: Enabling");

    wps_set_type(config->wps_type);
    wps_set_status(WPS_STATUS_DISABLE);

    ret = wifi_station_wps_init(config);

    if (ret != 0) {
        wps_set_type(WPS_TYPE_DISABLE);
        wps_set_status(WPS_STATUS_DISABLE);
        return ESP_FAIL;
    }
    wpa_sm->wpa_sm_wps_disable = wifi_wps_disable;
    return ESP_OK;
}

static int wifi_wps_disable_internal(void *ctx, void *data)
{
    /* Only disconnect in case of WPS pending */
    if (wps_get_status() == WPS_STATUS_PENDING) {
        esp_wifi_disconnect();
    }
    wps_set_status(WPS_STATUS_DISABLE);

    /* Call wps_delete_timer to delete all WPS timer, no timer will call wps_post()
     * to post message to wps_task once this function returns.
     */
    wps_delete_timer();
    wifi_station_wps_deinit();
#ifdef CONFIG_WPS_RECONNECT_ON_FAIL
    /* If the config exists, it means WPS was cancelled by the user */
    if (s_previous_wifi_config) {
        os_free(s_previous_wifi_config);
        s_previous_wifi_config = NULL;
    }
#endif
    return ESP_OK;
}

int esp_wifi_wps_disable(void)
{
    int ret = 0;
    struct wps_sm *wps_sm = gWpsSm;
    struct wpa_sm *wpa_sm = &gWpaSm;

    if (wps_sm && os_memcmp(wps_sm->identity, WSC_ID_REGISTRAR, wps_sm->identity_len) == 0) {
        return ESP_ERR_WIFI_MODE;
    }

    API_MUTEX_TAKE();

    if (!s_wps_enabled) {
        wpa_printf(MSG_DEBUG, "WPS: Already disabled");
        API_MUTEX_GIVE();
        return ESP_OK;
    }

    wpa_printf(MSG_INFO, "WPS: Disabling");
    wps_set_type(WPS_TYPE_DISABLE); /* Notify WiFi task */

    ret = eloop_register_timeout_blocking(wifi_wps_disable_internal, NULL, NULL);

    if (ESP_OK != ret) {
        wpa_printf(MSG_ERROR, "WPS: Failed to disable (ret=%d)", ret);
    }
    esp_wifi_set_wps_start_flag_internal(false);
    s_wps_enabled = false;
    API_MUTEX_GIVE();
    wpa_sm->wpa_sm_wps_disable = NULL;
    return ESP_OK;
}

int esp_wifi_wps_start(void)
{
    int ret;

    if (ESP_OK != wps_check_wifi_mode()) {
        return ESP_ERR_WIFI_MODE;
    }

    API_MUTEX_TAKE();

    if (!s_wps_enabled) {
        wpa_printf(MSG_ERROR, "WPS: Cannot start, not enabled");
        API_MUTEX_GIVE();
        return ESP_ERR_WIFI_WPS_SM;
    }

    if (esp_wifi_get_user_init_flag_internal() == 0) {
        API_MUTEX_GIVE();
        return ESP_ERR_WIFI_STATE;
    }

    ret = eloop_register_timeout_blocking(wifi_station_wps_start, NULL, NULL);
    API_MUTEX_GIVE();
    return ret;
}
