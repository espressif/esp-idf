/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
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

const char *wps_model_number = CONFIG_IDF_TARGET;

void *s_wps_api_lock = NULL;  /* Used in WPS public API only, never be freed */
void *s_wps_api_sem = NULL;   /* Sync semaphore used between WPS publi API caller task and WPS task */
bool s_wps_enabled = false;
#ifdef USE_WPS_TASK
struct wps_rx_param {
    u8 sa[ETH_ALEN];
    u8 *buf;
    int len;
    STAILQ_ENTRY(wps_rx_param) bqentry;
};
static STAILQ_HEAD(,wps_rx_param) s_wps_rxq;

typedef struct {
    void *arg;
    int ret; /* return value */
} wps_ioctl_param_t;

static void *s_wps_task_hdl = NULL;
static void *s_wps_queue = NULL;
static void *s_wps_data_lock = NULL;
static void *s_wps_task_create_sem = NULL;
static uint8_t s_wps_sig_cnt[SIG_WPS_NUM] = {0};

#endif

void wifi_wps_scan_done(void *arg, ETS_STATUS status);
void wifi_wps_scan(void *data, void *user_ctx);
int wifi_station_wps_start(void);
int wps_sm_rx_eapol_internal(u8 *src_addr, u8 *buf, u32 len);
void wifi_wps_start_internal(void);
int wifi_wps_enable_internal(const esp_wps_config_t *config);
int wifi_wps_disable_internal(void);
void wifi_station_wps_timeout_internal(void);
void wifi_station_wps_msg_timeout_internal(void);
void wifi_station_wps_success_internal(void);
void wifi_wps_scan_internal(void);
void wifi_station_wps_eapol_start_handle_internal(void);
void wps_add_discard_ap(u8 *bssid);

void wifi_station_wps_msg_timeout(void *data, void *user_ctx);
void wifi_station_wps_eapol_start_handle(void *data, void *user_ctx);
void wifi_station_wps_success(void *data, void *user_ctx);
void wifi_station_wps_timeout(void *data, void *user_ctx);

struct wps_sm *gWpsSm = NULL;
static wps_factory_information_t *s_factory_info = NULL;

static void wps_rxq_init(void)
{
    DATA_MUTEX_TAKE();
    STAILQ_INIT(&s_wps_rxq);
    DATA_MUTEX_GIVE();
}

static void wps_rxq_enqueue(struct wps_rx_param *param)
{
    DATA_MUTEX_TAKE();
    STAILQ_INSERT_TAIL(&s_wps_rxq,param, bqentry);
    DATA_MUTEX_GIVE();
}

static struct wps_rx_param * wps_rxq_dequeue(void)
{
    struct wps_rx_param *param = NULL;
    DATA_MUTEX_TAKE();
    if ((param = STAILQ_FIRST(&s_wps_rxq)) != NULL) {
        STAILQ_REMOVE_HEAD(&s_wps_rxq, bqentry);
        STAILQ_NEXT(param,bqentry) = NULL;
    }
    DATA_MUTEX_GIVE();
    return param;
}

static void wps_rxq_deinit(void)
{
    struct wps_rx_param *param = NULL;
    DATA_MUTEX_TAKE();
    while ((param = STAILQ_FIRST(&s_wps_rxq)) != NULL) {
        STAILQ_REMOVE_HEAD(&s_wps_rxq, bqentry);
        STAILQ_NEXT(param,bqentry) = NULL;
        os_free(param->buf);
        os_free(param);
    }
    DATA_MUTEX_GIVE();
}

#ifdef USE_WPS_TASK
void wps_task(void *pvParameters )
{
    ETSEvent *e;
    wps_ioctl_param_t *param;
    bool del_task = false;

    os_semphr_give(s_wps_task_create_sem);

    wpa_printf(MSG_DEBUG, "wps_Task enter");
    for (;;) {
        if ( TRUE == os_queue_recv(s_wps_queue, &e, OS_BLOCK) ) {

            if ( (e->sig >= SIG_WPS_ENABLE) && (e->sig < SIG_WPS_NUM) ) {
                DATA_MUTEX_TAKE();
                if (s_wps_sig_cnt[e->sig]) {
                    s_wps_sig_cnt[e->sig]--;
                } else {
                    wpa_printf(MSG_ERROR, "wpsT: invalid sig cnt, sig=%" PRId32 " cnt=%d", e->sig, s_wps_sig_cnt[e->sig]);
                }
                DATA_MUTEX_GIVE();
            }

            wpa_printf(MSG_DEBUG, "wpsT: rx sig=%" PRId32 "", e->sig);

            switch (e->sig) {
            case SIG_WPS_ENABLE:
            case SIG_WPS_DISABLE:
            case SIG_WPS_START:
                param = (wps_ioctl_param_t *)e->par;
                if (!param) {
                    wpa_printf(MSG_ERROR, "wpsT: invalid param sig=%" PRId32 "", e->sig);
                    os_semphr_give(s_wps_api_sem);
                    break;
                }

                if (e->sig == SIG_WPS_ENABLE) {
                    param->ret = wifi_wps_enable_internal((esp_wps_config_t *)(param->arg));
                } else if (e->sig == SIG_WPS_DISABLE) {
                    DATA_MUTEX_TAKE();
                    param->ret = wifi_wps_disable_internal();
                    del_task = true;
                    s_wps_task_hdl = NULL;
                    DATA_MUTEX_GIVE();
                } else {
                    param->ret = wifi_station_wps_start();
                }

                os_semphr_give(s_wps_api_sem);
                break;

            case SIG_WPS_RX: {
                struct wps_rx_param *param = NULL;
                while ((param = wps_rxq_dequeue()) != NULL) {
                    wps_sm_rx_eapol_internal(param->sa, param->buf, param->len);
                    os_free(param->buf);
                    os_free(param);
                }
                break;
            }

            case SIG_WPS_TIMER_TIMEOUT:
                wifi_station_wps_timeout_internal();
                break;

            case SIG_WPS_TIMER_MSG_TIMEOUT:
                wifi_station_wps_msg_timeout_internal();
                break;

            case SIG_WPS_TIMER_SUCCESS_CB:
                wifi_station_wps_success_internal();
                break;

            case SIG_WPS_TIMER_SCAN:
                wifi_wps_scan_internal();
                break;

            case SIG_WPS_TIMER_EAPOL_START:
                wifi_station_wps_eapol_start_handle_internal();
                break;

            default:
                wpa_printf(MSG_ERROR, "wpsT: invalid sig=%" PRId32 "", e->sig);
                break;
            }
            os_free(e);

            if (del_task) {
                wpa_printf(MSG_DEBUG, "wpsT: delete task");
                break;
            }
        }
    }
    os_task_delete(NULL);
}

/* wps_post() is thread-safe
 *
 */
int wps_post(uint32_t sig, uint32_t par)
{
    wpa_printf(MSG_DEBUG, "wps post: sig=%" PRId32 " cnt=%d", sig, s_wps_sig_cnt[sig]);

    DATA_MUTEX_TAKE();

    if (!s_wps_task_hdl) {
        wpa_printf(MSG_DEBUG, "wps post: sig=%" PRId32 " failed as wps task has been deinited", sig);
        DATA_MUTEX_GIVE();
        return ESP_FAIL;
    }
    if (s_wps_sig_cnt[sig]) {
        wpa_printf(MSG_DEBUG, "wps post: sig=%" PRId32 " processing", sig);
        DATA_MUTEX_GIVE();
        return ESP_OK;
    } else {
        ETSEvent *evt = (ETSEvent *)os_malloc(sizeof(ETSEvent));

        if (evt == NULL) {
            wpa_printf(MSG_ERROR, "WPS: E N M");
            DATA_MUTEX_GIVE();
            return ESP_FAIL;
        }

        s_wps_sig_cnt[sig]++;
        evt->sig = sig;
        evt->par = par;
        DATA_MUTEX_GIVE();

        if (os_queue_send(s_wps_queue, &evt, os_task_ms_to_tick(10)) != TRUE) {
            wpa_printf(MSG_ERROR, "WPS: Q S E");
            DATA_MUTEX_TAKE();
            s_wps_sig_cnt[sig]--;
            DATA_MUTEX_GIVE();
            return ESP_FAIL;
        }
    }
    return ESP_OK;
}
#endif

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
        wpa_printf(MSG_ERROR, "bssid is empty!");
        return -1;
    }

    return wpa_ether_send(sm, bssid, proto, data, data_len);
}


u8 *wps_sm_alloc_eapol(struct wps_sm *sm, u8 type,
                       const void *data, u16 data_len,
                       size_t *msg_len, void **data_pos)
{
    return wpa_alloc_eapol(sm, type, data, data_len, msg_len, data_pos);
}


void wps_sm_free_eapol(u8 *buffer)
{
    return wpa_free_eapol(buffer);
}

static void
wps_build_ic_appie_wps_pr(void)
{
    struct wpabuf *extra_ie = NULL;
    struct wpabuf *wps_ie;
    struct wps_sm *sm = gWpsSm;
    u16 pw_id;

    wpa_printf(MSG_DEBUG, "wps build: wps pr");

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

static void
wps_build_ic_appie_wps_ar(void)
{
    struct wpabuf *buf = wps_build_assoc_req_ie(WPS_REQ_ENROLLEE);

    wpa_printf(MSG_DEBUG, "wps build: wps ar");

    if (buf) {
        esp_wifi_set_appie_internal(WIFI_APPIE_WPS_AR, (uint8_t *)wpabuf_head(buf), buf->used, 0);
        wpabuf_free(buf);
    }
}

static bool ap_supports_sae(struct wps_scan_ie *scan)
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

static bool
wps_parse_scan_result(struct wps_scan_ie *scan)
{
    struct wps_sm *sm = gWpsSm;
    wifi_mode_t op_mode = 0;

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
        wpa_printf(MSG_DEBUG, "WEP not suppported in WPS");
        return false;
    }

    if (sm->ignore_sel_reg && !is_zero_ether_addr(sm->bssid)) {
        /* We have selected candidate for this scan */
        return false;
    }

    if (scan->wps) {
        bool ap_found = false;
        struct wpabuf *buf = wpabuf_alloc_copy(scan->wps + 6, scan->wps[1] - 4);
        int count;

        if ((wps_get_type() == WPS_TYPE_PBC && wps_is_selected_pbc_registrar(buf)) ||
            (wps_get_type() == WPS_TYPE_PIN && wps_is_addr_authorized(buf, sm->ownaddr, 1))) {
            /* Found one AP with selected registrar true */
            sm->ignore_sel_reg = false;
            sm->discard_ap_cnt = 0;
            ap_found = true;
        }
        if (wps_get_type() == WPS_TYPE_PIN && sm->ignore_sel_reg) {
            /* AP is in discard list? */
            for (count = 0; count < WPS_MAX_DIS_AP_NUM; count++) {
                if (os_memcmp(sm->dis_ap_list[count].bssid, scan->bssid, ETH_ALEN) == 0) {
                    wpa_printf(MSG_INFO, "discard ap bssid "MACSTR, MAC2STR(scan->bssid));
                    wpabuf_free(buf);
                    return false;
                }
            }
        }

        if (ap_found || sm->ignore_sel_reg) {
            wpabuf_free(buf);
            if (scan->ssid[1] > SSID_MAX_LEN) {
                return false;
	    }
            esp_wifi_enable_sta_privacy_internal();
            os_memset(sm->ssid[0], 0, SSID_MAX_LEN);
            os_memcpy(sm->ssid[0], (char *)&scan->ssid[2], (int)scan->ssid[1]);
            sm->ssid_len[0] = scan->ssid[1];
            if (scan->bssid && memcmp(sm->bssid, scan->bssid, ETH_ALEN) != 0) {
                wpa_printf(MSG_INFO, "sm BSSid: "MACSTR " scan BSSID " MACSTR,
                           MAC2STR(sm->bssid), MAC2STR(scan->bssid));
                sm->discover_ssid_cnt++;
                os_memcpy(sm->bssid, scan->bssid, ETH_ALEN);
                if (ap_supports_sae(scan)) {
                    wpa_printf(MSG_INFO, "AP supports SAE, get password in passphrase");
                    sm->dev->config_methods |= WPS_CONFIG_DISPLAY | WPS_CONFIG_VIRT_DISPLAY;
                    sm->wps->wps->config_methods |= WPS_CONFIG_DISPLAY | WPS_CONFIG_VIRT_DISPLAY;
                    /* Reset assoc req, probe reset not needed */
                    wps_build_ic_appie_wps_ar();
                }
            }
            wpa_printf(MSG_DEBUG, "wps discover [%s]", (char *)sm->ssid);
            sm->channel = scan->chan;

            return true;
        }
        wpabuf_free(buf);
    }

    return false;
}

int wps_send_eap_identity_rsp(u8 id)
{
    struct wps_sm *sm = gWpsSm;
    struct wpabuf *eap_buf = NULL;
    u8 *buf = NULL;
    int len;
    int ret = ESP_OK;

    wpa_printf(MSG_DEBUG, "wps send eapol id rsp");
    eap_buf = eap_msg_alloc(EAP_VENDOR_IETF, EAP_TYPE_IDENTITY, sm->identity_len,
                            EAP_CODE_RESPONSE, id);
    if (!eap_buf) {
        wpa_printf(MSG_ERROR, "eap buf allocation failed");
        ret = ESP_FAIL;
        goto _err;
    }


    wpabuf_put_data(eap_buf, sm->identity, sm->identity_len);

    buf = wps_sm_alloc_eapol(sm, IEEE802_1X_TYPE_EAP_PACKET, wpabuf_head_u8(eap_buf), wpabuf_len(eap_buf), (size_t *)&len, NULL);
    if (!buf) {
        wpa_printf(MSG_ERROR, "buf allocation failed");
        ret = ESP_ERR_NO_MEM;
        goto _err;
    }

    ret = wps_sm_ether_send(sm, ETH_P_EAPOL, buf, len);
    if (ret) {
        wpa_printf(MSG_ERROR, "wps sm ether send failed ret=%d", ret);
        ret = ESP_FAIL;
        goto _err;
    }

_err:
    wps_sm_free_eapol(buf);
    wpabuf_free(eap_buf);
    return ret;
}

int wps_send_frag_ack(u8 id)
{
    struct wps_sm *sm = gWpsSm;
    struct wpabuf *eap_buf = NULL;
    u8 *buf;
    int len;
    int ret = 0;

    wpa_printf(MSG_DEBUG, "send frag ack id:%d", id);

    if (!sm) {
        return ESP_FAIL;
    }

    eap_buf = eap_wsc_build_frag_ack(id, EAP_CODE_RESPONSE);
    if (!eap_buf) {
        ret = ESP_ERR_NO_MEM;
        goto _err;
    }

    buf = wps_sm_alloc_eapol(sm, IEEE802_1X_TYPE_EAP_PACKET, wpabuf_head_u8(eap_buf), wpabuf_len(eap_buf), (size_t *)&len, NULL);
    if (!buf) {
        ret = ESP_ERR_NO_MEM;
        goto _err;
    }

    ret = wps_sm_ether_send(sm, ETH_P_EAPOL, buf, len);
    wps_sm_free_eapol(buf);
    if (ret) {
        ret = ESP_ERR_NO_MEM;
        goto _err;
    }

_err:
    wpabuf_free(eap_buf);
    return ret;
}

int wps_enrollee_process_msg_frag(struct wpabuf **buf, int tot_len, u8 *frag_data, int frag_len, u8 flag)
{
    struct wps_sm *sm = gWpsSm;
    u8 identifier;

    if (!sm) {
        return ESP_FAIL;
    }

    identifier = sm->current_identifier;

    if (buf == NULL || frag_data == NULL) {
        wpa_printf(MSG_ERROR, "fun:%s. line:%d, frag buf or frag data is null", __FUNCTION__, __LINE__);
        return ESP_FAIL;
    }

    if (*buf == NULL) {
        if (0 == (flag & WPS_MSG_FLAG_LEN) || tot_len < frag_len) {
            wpa_printf(MSG_ERROR, "fun:%s. line:%d, flag error:%02x", __FUNCTION__, __LINE__, flag);
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
        wpa_printf(MSG_ERROR, "fun:%s. line:%d, flag error:%02x", __FUNCTION__, __LINE__, flag);
        return ESP_FAIL;
    }

    wpabuf_put_data(*buf, frag_data, frag_len);

    if (flag & WPS_MSG_FLAG_MORE) {
        return wps_send_frag_ack(identifier);
    }

    return ESP_OK;
}

int wps_process_wps_mX_req(u8 *ubuf, int len, enum wps_process_res *res)
{
    struct wps_sm *sm = gWpsSm;
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

    expd = (struct eap_expand *) ubuf;
    wpa_printf(MSG_DEBUG, "wps process mX req: len %d, tlen %d", len, tlen);

    if (sm->state == WAIT_START) {
        if (expd->opcode != WSC_Start) {
            wpa_printf(MSG_DEBUG, "EAP-WSC: Unexpected Op-Code %d "
                   "in WAIT_START state", expd->opcode);
            return ESP_FAIL;
        }
        wpa_printf(MSG_DEBUG, "EAP-WSC: Received start");
        sm->state = WPA_MESG;
    } else if (expd->opcode == WSC_Start){
        wpa_printf(MSG_DEBUG, "EAP-WSC: Unexpected Op-Code %d",
                expd->opcode);
        return ESP_FAIL;
    }

    flag = *(u8 *)(ubuf + sizeof(struct eap_expand));
    if (flag & WPS_MSG_FLAG_LEN) {
        tbuf = ubuf + sizeof(struct eap_expand) + 1 + 2;//two bytes total length
        frag_len = len - (sizeof(struct eap_expand) + 1 + 2);
        be_tot_len = *(u16 *)(ubuf + sizeof(struct eap_expand) + 1);
        tlen = ((be_tot_len & 0xff) << 8) | ((be_tot_len >> 8) & 0xff);
    } else {
        tbuf = ubuf + sizeof(struct eap_expand) + 1;
        frag_len = len - (sizeof(struct eap_expand) + 1);
        tlen = frag_len;
    }

    if ((flag & WPS_MSG_FLAG_MORE) || wps_buf != NULL) {//frag msg
        if (tlen > 50000) {
            wpa_printf(MSG_ERROR, "EAP-WSC: Invalid Message Length");
            return ESP_FAIL;
	}
        wpa_printf(MSG_DEBUG, "rx frag msg id:%d, flag:%d, frag_len: %d, tot_len: %d, be_tot_len:%d", sm->current_identifier, flag, frag_len, tlen, be_tot_len);
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
            wpa_printf(MSG_ERROR, "something is wrong, frag buf is not freed");
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

    if (*res == WPS_FAILURE) {
        sm->state = WPA_FAIL;
    }

    if (wps_buf) {
        wpabuf_free(wps_buf);
        wps_buf = NULL;
    }
    return ESP_OK;
}

int wps_send_wps_mX_rsp(u8 id)
{
    struct wps_sm *sm = gWpsSm;
    struct wpabuf *eap_buf = NULL;
    struct wpabuf *wps_buf = NULL;
    u8 *buf;
    int len;
    int ret = 0;
    enum wsc_op_code opcode;

    wpa_printf(MSG_DEBUG, "wps send wps mX rsp");

    if (!sm) {
        return ESP_FAIL;
    }

    wps_buf = wps_enrollee_get_msg(sm->wps, &opcode);
    if (!wps_buf) {
        ret = ESP_FAIL;
        goto _err;
    }

    eap_buf = eap_msg_alloc(EAP_VENDOR_WFA, 0x00000001, wpabuf_len(wps_buf) + 2, EAP_CODE_RESPONSE, id);
    if (!eap_buf) {
        ret = ESP_FAIL;
        goto _err;
    }

    wpabuf_put_u8(eap_buf, opcode);
    wpabuf_put_u8(eap_buf, 0x00); /* flags */
    wpabuf_put_data(eap_buf, wpabuf_head_u8(wps_buf), wpabuf_len(wps_buf));

    wpabuf_free(wps_buf);

    buf = wps_sm_alloc_eapol(sm, IEEE802_1X_TYPE_EAP_PACKET, wpabuf_head_u8(eap_buf), wpabuf_len(eap_buf), (size_t *)&len, NULL);
    if (!buf) {
        ret = ESP_FAIL;
        goto _err;
    }

    ret = wps_sm_ether_send(sm, ETH_P_EAPOL, buf, len);
    wps_sm_free_eapol(buf);
    if (ret) {
        ret = ESP_FAIL;
        goto _err;
    }

_err:
    wpabuf_free(eap_buf);
    return ret;
}



int wps_tx_start(void)
{
    struct wps_sm *sm = gWpsSm;
    u8 *buf;
    int len;

    if (!sm) {
        return ESP_FAIL;
    }

    wpa_printf(MSG_DEBUG,  "WPS: Send EAPOL START.");
    buf = wps_sm_alloc_eapol(sm, IEEE802_1X_TYPE_EAPOL_START, (u8 *)"", 0, (size_t *)&len, NULL);
    if (!buf) {
        return ESP_ERR_NO_MEM;
    }

    wps_sm_ether_send(sm, ETH_P_EAPOL, buf, len);
    wps_sm_free_eapol(buf);

    eloop_register_timeout(3, 0, wifi_station_wps_eapol_start_handle, NULL, NULL);

    return ESP_OK;
}

int wps_start_pending(void)
{
    if (!gWpsSm) {
        return ESP_FAIL;
    }

    wpa_printf(MSG_DEBUG, "wps start pending");
    return wps_tx_start();
}

static void wps_stop_connection_timers(struct wps_sm *sm)
{
    esp_wifi_disarm_sta_connection_timer_internal();
    eloop_cancel_timeout(wifi_station_wps_msg_timeout, NULL, NULL);
    eloop_cancel_timeout(wifi_station_wps_success, NULL, NULL);
}

static int wps_sm_init(struct wps_sm *sm)
{
    if (!sm) {
        return -1;
    }
    sm->ignore_sel_reg = false;
    sm->discard_ap_cnt = 0;
    sm->scan_cnt = 0;
    sm->discover_ssid_cnt = 0;
    os_bzero(sm->bssid, ETH_ALEN);
    os_bzero(sm->ssid, sizeof(sm->ssid));
    os_bzero(sm->ssid_len, sizeof(sm->ssid_len));
    sm->ap_cred_cnt = 0;

    return 0;
}

int wps_stop_process(wifi_event_sta_wps_fail_reason_t reason_code)
{
    struct wps_sm *sm = gWpsSm;

    if (!gWpsSm) {
        return ESP_FAIL;
    }

    wps_set_status(WPS_STATUS_DISABLE);
    wps_sm_init(sm);

    sm->wps->state = SEND_M1;
    wps_stop_connection_timers(sm);
    esp_wifi_disconnect();

    sm->state = WPA_FAIL;
    wpa_printf(MSG_DEBUG, "Write wps_fail_information");

    esp_event_post(WIFI_EVENT, WIFI_EVENT_STA_WPS_ER_FAILED, &reason_code, sizeof(reason_code), OS_BLOCK);

    return ESP_OK;
}

int wps_finish(void)
{
    struct wps_sm *sm = gWpsSm;
    int ret = ESP_FAIL;

    if (!gWpsSm) {
        return ESP_FAIL;
    }

    if (sm->wps->state == WPS_FINISHED) {
        wpa_printf(MSG_DEBUG, "wps finished------>");
        wps_set_status(WPS_STATUS_SUCCESS);
        wps_stop_connection_timers(sm);

        if (sm->ap_cred_cnt == 1) {
            wifi_config_t *config = os_zalloc(sizeof(wifi_config_t));

            if (config == NULL) {
                return ESP_FAIL;
            }

            esp_wifi_get_config(WIFI_IF_STA, config);
            os_memcpy(config->sta.ssid, sm->ssid[0], sm->ssid_len[0]);
            os_memcpy(config->sta.password, sm->key[0], sm->key_len[0]);
            os_memcpy(config->sta.bssid, sm->bssid, ETH_ALEN);
            config->sta.bssid_set = 0;
            config->sta.sae_pwe_h2e = 0;
            esp_wifi_set_config(WIFI_IF_STA, config);

            os_free(config);
        }
        eloop_cancel_timeout(wifi_station_wps_success, NULL, NULL);
	eloop_register_timeout(1, 0, wifi_station_wps_success, NULL, NULL);

        ret = 0;
    } else {
        wpa_printf(MSG_ERROR, "wps failed-----> ignore_sel_reg=%d", sm->ignore_sel_reg);
        if (sm->ignore_sel_reg) {
            sm->discover_ssid_cnt = 0;
            esp_wifi_disconnect();
            os_bzero(sm->ssid, sizeof(sm->ssid));
            os_bzero(sm->ssid_len, sizeof(sm->ssid_len));
            wps_add_discard_ap(sm->bssid);
        } else {
            ret = wps_stop_process(WPS_FAIL_REASON_NORMAL);
        }
    }

    return ret;
}

/* Add current ap to discard ap list */
void wps_add_discard_ap(u8 *bssid)
{
    struct wps_sm *sm = gWpsSm;
    u8 cnt = sm->discard_ap_cnt;

    if (!gWpsSm || !bssid) {
        return;
    }

    if (sm->discard_ap_cnt < WPS_MAX_DIS_AP_NUM) {
        sm->discard_ap_cnt++;
    } else {
        for (cnt = 0; cnt < WPS_MAX_DIS_AP_NUM - 2; cnt++) {
            os_memcpy(sm->dis_ap_list[cnt].bssid, sm->dis_ap_list[cnt + 1].bssid, 6);
        }
        sm->discard_ap_cnt = WPS_MAX_DIS_AP_NUM;
    }
    os_memcpy(sm->dis_ap_list[cnt].bssid, bssid, ETH_ALEN);
    wpa_printf(MSG_INFO, "Added BSSID:"MACSTR" to discard list cnt=%d" , MAC2STR(bssid), sm->discard_ap_cnt);
}

int wps_start_msg_timer(void)
{
    struct wps_sm *sm = gWpsSm;
    uint32_t msg_timeout;
    int ret = ESP_FAIL;

    if (!gWpsSm) {
        return ESP_FAIL;
    }

    if (sm->wps->state == WPS_FINISHED) {
        msg_timeout = 100;
        wpa_printf(MSG_DEBUG, "start msg timer WPS_FINISHED %" PRId32 " ms", msg_timeout);
	eloop_cancel_timeout(wifi_station_wps_msg_timeout, NULL, NULL);
	eloop_register_timeout(0, msg_timeout*1000, wifi_station_wps_msg_timeout, NULL, NULL);
        ret = 0;
    } else if (sm->wps->state == RECV_M2) {
        msg_timeout = 5;
        wpa_printf(MSG_DEBUG, "start msg timer RECV_M2 %" PRId32 " ms", msg_timeout);
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
int wps_sm_rx_eapol(u8 *src_addr, u8 *buf, u32 len)
{
    if (!gWpsSm) {
        return ESP_FAIL;
    }

#ifdef USE_WPS_TASK
    {
        struct wps_rx_param *param = os_zalloc(sizeof(struct wps_rx_param));   /* free in task */

        if (!param) {
            return ESP_ERR_NO_MEM;
        }

        param->buf = os_zalloc(len);   /* free in task */
        if (!param->buf) {
            os_free(param);
            return ESP_ERR_NO_MEM;
        }
        os_memcpy(param->buf, buf, len);
        param->len = len;
        os_memcpy(param->sa, src_addr, ETH_ALEN);

        wps_rxq_enqueue(param);
        return wps_post(SIG_WPS_RX, 0);
    }
#else
    return wps_sm_rx_eapol_internal(src_addr, buf, len);
#endif
}

int wps_sm_rx_eapol_internal(u8 *src_addr, u8 *buf, u32 len)
{
    struct wps_sm *sm = gWpsSm;
    u32 plen, data_len, eap_len;
    struct ieee802_1x_hdr *hdr;
    struct eap_hdr *ehdr;
    u8 *tmp;
    u8 eap_code;
    u8 eap_type;
    int ret = ESP_FAIL;
    enum wps_process_res res = WPS_DONE;

    if (!gWpsSm) {
        return ESP_FAIL;
    }

    if (len < sizeof(*hdr) + sizeof(*ehdr)) {
        wpa_printf(MSG_DEBUG,  "WPA: EAPOL frame too short to be a WPA "
                   "EAPOL-Key (len %lu, expecting at least %lu)",
                   (unsigned long) len,
                   (unsigned long) sizeof(*hdr) + sizeof(*ehdr));
        return ESP_OK;
    }

    tmp = buf;

    hdr = (struct ieee802_1x_hdr *) tmp;
    ehdr = (struct eap_hdr *) (hdr + 1);
    plen = be_to_host16(hdr->length);
    data_len = plen + sizeof(*hdr);
    eap_len = be_to_host16(ehdr->length);

    wpa_printf(MSG_DEBUG, "IEEE 802.1X RX: version=%d type=%d length=%" PRId32 "",
               hdr->version, hdr->type, plen);

    if (hdr->version < EAPOL_VERSION) {
        /* TODO: backwards compatibility */
    }
    if (hdr->type != IEEE802_1X_TYPE_EAP_PACKET) {
        wpa_printf(MSG_DEBUG, "WPS: EAP frame (type %u) discarded, "
                   "not a EAP PACKET frame", hdr->type);
        ret = 0;
        goto out;
    }
    if (plen > len - sizeof(*hdr) || plen < sizeof(*ehdr)) {
        wpa_printf(MSG_DEBUG, "WPA: EAPOL frame payload size %lu "
                   "invalid (frame size %lu)",
                   (unsigned long) plen, (unsigned long) len);
        ret = 0;
        goto out;
    }

    wpa_hexdump(MSG_MSGDUMP, "WPA: RX EAPOL-EAP PACKET", tmp, len);

    if (data_len < len) {
        wpa_printf(MSG_DEBUG, "WPA: ignoring %lu bytes after the IEEE "
                   "802.1X data", (unsigned long) len - data_len);
    }

    if (eap_len != plen) {
        wpa_printf(MSG_DEBUG, "WPA: EAPOL length %lu "
                   "invalid (eapol length %lu)",
                   (unsigned long) eap_len, (unsigned long) plen);
        ret = 0;
        goto out;
    }

    eap_code = ehdr->code;
    switch (eap_code) {
    case EAP_CODE_SUCCESS:
        wpa_printf(MSG_DEBUG, "error: receive eapol success frame!");
        ret = 0;
        break;
    case EAP_CODE_FAILURE:
        wpa_printf(MSG_DEBUG, "receive eap code failure!");
        ret = wps_finish();
        break;
    case EAP_CODE_RESPONSE:
        wpa_printf(MSG_DEBUG, "error: receive eapol response frame!");
        ret = 0;
        break;
    case EAP_CODE_REQUEST: {
        eap_type = ((u8 *)ehdr)[sizeof(*ehdr)];
        switch (eap_type) {
        case EAP_TYPE_IDENTITY:
            wpa_printf(MSG_DEBUG, "=========identity===========");
            sm->current_identifier = ehdr->identifier;
            eloop_cancel_timeout(wifi_station_wps_eapol_start_handle, NULL, NULL);
            wpa_printf(MSG_DEBUG,  "WPS: Build EAP Identity.");
            ret = wps_send_eap_identity_rsp(ehdr->identifier);
            eloop_register_timeout(3, 0, wifi_station_wps_eapol_start_handle, NULL, NULL);
            break;
        case EAP_TYPE_EXPANDED:
            wpa_printf(MSG_DEBUG, "=========expanded plen[%" PRId32 "], %d===========", plen, sizeof(*ehdr));
            sm->current_identifier = ehdr->identifier;

            tmp = (u8 *)(ehdr + 1) + 1;
            ret = wps_process_wps_mX_req(tmp, plen - sizeof(*ehdr) - 1, &res);
            if (res == WPS_FRAGMENT) {
                wpa_printf(MSG_DEBUG, "wps frag, silently exit", res);
                ret = ESP_OK;
                break;
            }
            if (ret == ESP_OK && res != WPS_FAILURE) {
                ret = wps_send_wps_mX_rsp(ehdr->identifier);

                if (ret == ESP_OK) {
                    wpa_printf(MSG_DEBUG, "sm->wps->state = %d", sm->wps->state);
                    wps_start_msg_timer();
                }
            } else {
                ret = ESP_FAIL;
            }
            break;
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
out:
    if (ret != 0 && sm->ignore_sel_reg) {
        wifi_wps_scan(NULL, NULL);
    } else if ((ret != 0 || res == WPS_FAILURE)) {
        wifi_event_sta_wps_fail_reason_t reason_code = WPS_FAIL_REASON_NORMAL;
        wpa_printf(MSG_DEBUG, "wpa rx eapol internal: fail ret=%d", ret);
        wps_set_status(WPS_STATUS_DISABLE);
        esp_wifi_disarm_sta_connection_timer_internal();
        eloop_cancel_timeout(wifi_station_wps_timeout, NULL, NULL);

        esp_event_post(WIFI_EVENT, WIFI_EVENT_STA_WPS_ER_FAILED, &reason_code, sizeof(reason_code), OS_BLOCK);

        return ret;
    }

    return ret;
}

int wps_set_default_factory(void)
{
    if (!s_factory_info) {
        s_factory_info = os_zalloc(sizeof(wps_factory_information_t));
        if (!s_factory_info) {
            wpa_printf(MSG_ERROR, "wps factory info malloc failed");
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

    wpa_printf(MSG_INFO, "manufacturer: %s, model number: %s, model name: %s, device name: %s", s_factory_info->manufacturer,
               s_factory_info->model_number, s_factory_info->model_name, s_factory_info->device_name);

    return ESP_OK;
}


int wps_dev_init(void)
{
    int ret = 0;
    struct wps_sm *sm = gWpsSm;
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
    int ret = 0;

    if (!dev) {
        return ESP_FAIL;
    }
    wps_device_data_free(dev);

    if (s_factory_info) {
        os_free(s_factory_info);
        s_factory_info = NULL;
    }

    return ret;
}

void
wifi_station_wps_timeout_internal(void)
{
    struct wps_sm *sm = gWpsSm;

    if (!sm) {
        return;
    }

    esp_wifi_disarm_sta_connection_timer_internal();

    wps_set_status(WPS_STATUS_DISABLE);

    esp_event_post(WIFI_EVENT, WIFI_EVENT_STA_WPS_ER_TIMEOUT, 0, 0, OS_BLOCK);
}

void wifi_station_wps_timeout(void *data, void *user_ctx)
{
#ifdef USE_WPS_TASK
    wps_post(SIG_WPS_TIMER_TIMEOUT, 0);
    return;
#else
    wifi_station_wps_timeout_internal();
#endif
}

void
wifi_station_wps_msg_timeout_internal(void)
{
    struct wps_sm *sm = gWpsSm;
    if (!sm) {
        return;
    }

    if (sm->wps->state == WPS_FINISHED) {
        wpa_printf(MSG_DEBUG, "wps msg timeout WPS_FINISHED");
        wps_finish();
        return;
    } else if (sm->wps->state == RECV_M2) {
        wpa_printf(MSG_DEBUG, "wps msg timeout RECV_M2");
        if (!sm->ignore_sel_reg) {
            wps_stop_process(WPS_FAIL_REASON_RECV_M2D);
        }
    }
    if (sm->ignore_sel_reg) {
        esp_wifi_disconnect();
        wps_add_discard_ap(sm->bssid);
        os_bzero(sm->ssid, sizeof(sm->ssid));
        os_bzero(sm->ssid_len, sizeof(sm->ssid_len));
        os_bzero(sm->bssid, ETH_ALEN);
        sm->discover_ssid_cnt = 0;
        wifi_wps_scan(NULL, NULL);
    }
}

void wifi_station_wps_msg_timeout(void *data, void *user_ctx)
{
#ifdef USE_WPS_TASK
    wps_post(SIG_WPS_TIMER_MSG_TIMEOUT, 0);
    return;
#else
    wifi_station_wps_msg_timeout_internal();
#endif
}

void wifi_station_wps_success_internal(void)
{
    wifi_event_sta_wps_er_success_t evt = {0};
    struct wps_sm *sm = gWpsSm;
    int i;

    /*
     * For only one AP credential don't send event data, wps_finish() has already set
     * the config. This is for backward compatibility.
     */
    if (sm->ap_cred_cnt > 1) {
        evt.ap_cred_cnt = sm->ap_cred_cnt;
        for (i = 0; i < MAX_WPS_AP_CRED; i++) {
            os_memcpy(evt.ap_cred[i].ssid, sm->ssid[i], sm->ssid_len[i]);
            os_memcpy(evt.ap_cred[i].passphrase, sm->key[i], sm->key_len[i]);
        }
        esp_event_post(WIFI_EVENT, WIFI_EVENT_STA_WPS_ER_SUCCESS, &evt,
                                sizeof(evt), OS_BLOCK);
    } else {
        esp_event_post(WIFI_EVENT, WIFI_EVENT_STA_WPS_ER_SUCCESS,
                                0, 0, OS_BLOCK);
    }
}

void wifi_station_wps_success(void *data, void *user_ctx)
{
#ifdef USE_WPS_TASK
    wps_post(SIG_WPS_TIMER_SUCCESS_CB, 0);
    return;
#else
    wifi_station_wps_success_internal();
#endif
}

void wifi_station_wps_eapol_start_handle_internal(void)
{
    wpa_printf(MSG_DEBUG, "Resend EAPOL-Start.");
    wps_tx_start();
}

void wifi_station_wps_eapol_start_handle(void *data, void *user_ctx)
{
#ifdef USE_WPS_TASK
    wps_post(SIG_WPS_TIMER_EAPOL_START, 0);
    return;
#else
    wifi_station_wps_eapol_start_handle_internal();
#endif
}

static int save_credentials_cb(void *ctx, const struct wps_credential *cred)
{
    if (!gWpsSm || !cred || gWpsSm->ap_cred_cnt > 2) {
        return ESP_FAIL;
    }

    os_memset(gWpsSm->ssid[gWpsSm->ap_cred_cnt], 0x00, sizeof(gWpsSm->ssid[gWpsSm->ap_cred_cnt]));
    os_memset(gWpsSm->key[gWpsSm->ap_cred_cnt], 0x00, sizeof(gWpsSm->key[gWpsSm->ap_cred_cnt]));

    os_memcpy(gWpsSm->ssid[gWpsSm->ap_cred_cnt], cred->ssid, cred->ssid_len);
    gWpsSm->ssid_len[gWpsSm->ap_cred_cnt] = cred->ssid_len;
    os_memcpy(gWpsSm->key[gWpsSm->ap_cred_cnt], cred->key, cred->key_len);
    gWpsSm->key_len[gWpsSm->ap_cred_cnt] = cred->key_len;

    gWpsSm->ap_cred_cnt++;

    wpa_hexdump_ascii(MSG_DEBUG, "ssid ", cred->ssid, cred->ssid_len);
    wpa_hexdump_ascii(MSG_DEBUG, "key ", cred->key, cred->key_len);

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
        wpa_printf(MSG_INFO, "Provided PIN %s is not valid, generated a new PIN %08d", (char *)cfg->pin, spin);
        os_snprintf((char *)cfg->pin, 9, "%08d", spin);
    }

    return 0;
}

static int wifi_station_wps_init(const esp_wps_config_t *config)
{
    struct wps_funcs *wps_cb;
    struct wps_sm *sm = NULL;
    struct wps_config cfg = {0};

    if (gWpsSm) {
        goto _out;
    }

    wpa_printf(MSG_DEBUG, "wifi sta wps init");

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
    /**************80211 reference***************/

    if (esp_wifi_get_appie_internal(WIFI_APPIE_WPS_PR) == NULL) {            /* alloc probe req wps ie */
        wps_build_ic_appie_wps_pr();
    }

    if (esp_wifi_get_appie_internal(WIFI_APPIE_WPS_AR) == NULL) {           /* alloc assoc req wps ie */
        wps_build_ic_appie_wps_ar();
    }

    eloop_cancel_timeout(wifi_station_wps_timeout, NULL, NULL);
    eloop_cancel_timeout(wifi_station_wps_msg_timeout, NULL, NULL);
    eloop_cancel_timeout(wifi_station_wps_success, NULL, NULL);
    eloop_cancel_timeout(wifi_wps_scan, NULL, NULL);
    eloop_cancel_timeout(wifi_station_wps_eapol_start_handle, NULL, NULL);

    wps_cb = os_malloc(sizeof(struct wps_funcs));
    if (wps_cb == NULL) {
        goto _err;
    }
    wps_cb->wps_parse_scan_result = wps_parse_scan_result;
    wps_cb->wifi_station_wps_start = wifi_station_wps_start;
    wps_cb->wps_sm_rx_eapol = wps_sm_rx_eapol;
    wps_cb->wps_start_pending = wps_start_pending;
    esp_wifi_set_wps_cb_internal(wps_cb);

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

int wps_delete_timer(void)
{
    struct wps_sm *sm = gWpsSm;

    if (!sm) {
        return ESP_OK;
    }

    eloop_cancel_timeout(wifi_station_wps_success, NULL, NULL);
    eloop_cancel_timeout(wifi_station_wps_timeout, NULL, NULL);
    eloop_cancel_timeout(wifi_station_wps_msg_timeout, NULL, NULL);
    eloop_cancel_timeout(wifi_wps_scan, NULL, NULL);
    eloop_cancel_timeout(wifi_station_wps_eapol_start_handle, NULL, NULL);
    esp_wifi_disarm_sta_connection_timer_internal();
    return ESP_OK;
}

int
wifi_station_wps_deinit(void)
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
    os_free(gWpsSm);
    gWpsSm = NULL;

    return ESP_OK;
}

struct wps_sm *
wps_sm_get(void)
{
    return gWpsSm;
}

void
wifi_wps_scan_done(void *arg, ETS_STATUS status)
{
    struct wps_sm *sm = gWpsSm;
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
        if (wps_get_type() == WPS_TYPE_PBC) {
            wpa_printf(MSG_INFO, "PBC session overlap!");
            wps_set_status(WPS_STATUS_DISABLE);
            esp_event_post(WIFI_EVENT, WIFI_EVENT_STA_WPS_ER_PBC_OVERLAP, 0, 0, OS_BLOCK);
        } else {
            wps_set_status(WPS_STATUS_PENDING);
        }
    }

    wpa_printf(MSG_DEBUG, "wps scan_done discover_ssid_cnt = %d", sm->discover_ssid_cnt);

    sm->discover_ssid_cnt = 0;

    if (wps_get_status() == WPS_STATUS_PENDING) {
        esp_wifi_disconnect();

        os_memcpy(wifi_config.sta.bssid, sm->bssid, ETH_ALEN);
        os_memcpy(wifi_config.sta.ssid, (char *)sm->ssid[0], sm->ssid_len[0]);
        wifi_config.sta.bssid_set = 1;
        wifi_config.sta.channel = sm->channel;
        wpa_printf(MSG_INFO, "WPS: connecting to %s, bssid=" MACSTR,
                   (char *)sm->ssid[0], MAC2STR(wifi_config.sta.bssid));
        esp_wifi_set_config(0, &wifi_config);

        wpa_printf(MSG_DEBUG, "WPS: neg start");
        esp_wifi_connect();
        sm->state = WAIT_START;
        eloop_cancel_timeout(wifi_station_wps_msg_timeout, NULL, NULL);
        eloop_register_timeout(2, 0, wifi_station_wps_msg_timeout, NULL, NULL);
    } else if (wps_get_status() == WPS_STATUS_SCANNING) {
        if (wps_get_type() == WPS_TYPE_PIN && sm->scan_cnt > WPS_IGNORE_SEL_REG_MAX_CNT) {
            wpa_printf(MSG_INFO, "WPS: ignore selected registrar after %d scans", sm->scan_cnt);
            sm->ignore_sel_reg = true;
        }
        eloop_cancel_timeout(wifi_wps_scan, NULL, NULL);
        eloop_register_timeout(0, 100*1000, wifi_wps_scan, NULL, NULL);
    } else {
        return;
    }
}

void
wifi_wps_scan_internal(void)
{
    struct wps_sm *sm = gWpsSm;

    sm->scan_cnt++;
    wpa_printf(MSG_DEBUG, "wifi_wps_scan : %d", sm->scan_cnt);

    typedef void (* scan_done_cb_t)(void *arg, ETS_STATUS status);
    extern int esp_wifi_promiscuous_scan_start(wifi_scan_config_t *config, scan_done_cb_t cb);
    esp_wifi_promiscuous_scan_start(NULL, wifi_wps_scan_done);
}

void wifi_wps_scan(void *data, void *user_ctx)
{
#ifdef USE_WPS_TASK
    wps_post(SIG_WPS_TIMER_SCAN, 0);
    return;
#else
    wifi_wps_scan_internal();
#endif
}

static int wps_rf_band_cb(void *ctx)
{
	return WPS_RF_24GHZ;
}

int wifi_station_wps_start(void)
{
    struct wps_sm *sm = wps_sm_get();

    if (!sm) {
        wpa_printf(MSG_ERROR, "WPS: wps is not initialized");
        return ESP_FAIL;
    }

    eloop_register_timeout(120, 0, wifi_station_wps_timeout, NULL, NULL);

    switch (wps_get_status()) {
    case WPS_STATUS_DISABLE: {
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
        wifi_wps_scan(NULL, NULL);
        break;
    }
    case WPS_STATUS_SCANNING:
        sm->scan_cnt = 0;
        eloop_cancel_timeout(wifi_station_wps_timeout, NULL, NULL);
        eloop_register_timeout(120, 0, wifi_station_wps_timeout, NULL, NULL);
        break;
    default:
        break;
    }
    sm->discard_ap_cnt = 0;
    os_memset(&sm->dis_ap_list, 0, WPS_MAX_DIS_AP_NUM * sizeof(struct discard_ap_list_t));
    esp_wifi_set_wps_start_flag_internal(true);
    return ESP_OK;
}

int wps_task_deinit(void)
{
    wpa_printf(MSG_DEBUG, "wps task deinit");

    if (s_wps_api_sem) {
        os_semphr_delete(s_wps_api_sem);
        s_wps_api_sem = NULL;
        wpa_printf(MSG_DEBUG, "wps task deinit: free api sem");
    }

    if (s_wps_task_create_sem) {
        os_semphr_delete(s_wps_task_create_sem);
        s_wps_task_create_sem = NULL;
        wpa_printf(MSG_DEBUG, "wps task deinit: free task create sem");
    }

    if (s_wps_queue) {
        os_queue_delete(s_wps_queue);
        s_wps_queue = NULL;
        wpa_printf(MSG_DEBUG, "wps task deinit: free queue");
    }

    if (STAILQ_FIRST(&s_wps_rxq) != NULL){
        wps_rxq_deinit();
    }

    return ESP_OK;
}

int wps_task_init(void)
{
    int ret = 0;

    /* Call wps_task_deinit() first in case esp_wifi_wps_disable() fails
     */
    wps_task_deinit();

    if (!s_wps_data_lock) {
        s_wps_data_lock = os_recursive_mutex_create();
        if (!s_wps_data_lock) {
            wpa_printf(MSG_ERROR, "wps task init: failed to alloc data lock");
            goto _wps_no_mem;
        }
    }

    s_wps_api_sem = os_semphr_create(1, 0);
    if (!s_wps_api_sem) {
        wpa_printf(MSG_ERROR, "wps task init: failed to create api sem");
        goto _wps_no_mem;
    }

    s_wps_task_create_sem = os_semphr_create(1, 0);
    if (!s_wps_task_create_sem) {
        wpa_printf(MSG_ERROR, "wps task init: failed to create task sem");
        goto _wps_no_mem;
    }

    os_bzero(s_wps_sig_cnt, SIG_WPS_NUM);
    s_wps_queue = os_queue_create(SIG_WPS_NUM, sizeof(s_wps_queue));
    if (!s_wps_queue) {
        wpa_printf(MSG_ERROR, "wps task init: failed to alloc queue");
        goto _wps_no_mem;
    }

    wps_rxq_init();

    ret = os_task_create(wps_task, "wpsT", WPS_TASK_STACK_SIZE, NULL, 2, &s_wps_task_hdl);
    if (TRUE != ret) {
        wpa_printf(MSG_ERROR, "wps enable: failed to create task");
        goto _wps_no_mem;
    }

    os_semphr_take(s_wps_task_create_sem, OS_BLOCK);
    os_semphr_delete(s_wps_task_create_sem);
    s_wps_task_create_sem = NULL;

    wpa_printf(MSG_DEBUG, "wifi wps enable: task prio:%d, stack:%d", 2, WPS_TASK_STACK_SIZE);
    return ESP_OK;

_wps_no_mem:
    wps_task_deinit();
    return ESP_ERR_NO_MEM;
}

int wps_post_block(uint32_t sig, void *arg)
{
    wps_ioctl_param_t param;

    param.ret = ESP_FAIL;
    param.arg = arg;

    if (ESP_OK != wps_post(sig, (uint32_t)&param)) {
        return ESP_FAIL;
    }

    if (TRUE == os_semphr_take(s_wps_api_sem, OS_BLOCK)) {
        return param.ret;
    } else {
        return ESP_FAIL;
    }
}

int wps_check_wifi_mode(void)
{
    wifi_mode_t mode;
    int ret;

    ret = esp_wifi_get_mode(&mode);
    if (ESP_OK != ret) {
        wpa_printf(MSG_ERROR, "wps check wifi mode: failed to get wifi mode ret=%d", ret);
        return ESP_FAIL;
    }

    if (
#ifdef CONFIG_ESP_WIFI_SOFTAP_SUPPORT
        mode == WIFI_MODE_AP ||
#endif
        mode == WIFI_MODE_NULL) {
        wpa_printf(MSG_ERROR, "wps check wifi mode: wrong wifi mode=%d ", mode);
        return ESP_ERR_WIFI_MODE;
    }

    return ESP_OK;
}

int esp_wifi_wps_enable(const esp_wps_config_t *config)
{
    int ret = ESP_OK;
    struct wps_sm *sm = gWpsSm;

    if (esp_wifi_get_user_init_flag_internal() == 0) {
        wpa_printf(MSG_ERROR, "wps enable: wifi not started cannot disable wpsreg");
        return ESP_ERR_WIFI_STATE;
    }

    if (ESP_OK != wps_check_wifi_mode()) {
        return ESP_ERR_WIFI_MODE;
    }

    if (is_dpp_enabled()) {
        wpa_printf(MSG_ERROR, "wps enabled failed since DPP is initialized");
        return ESP_FAIL;
    }

    API_MUTEX_TAKE();
    if (s_wps_enabled) {
        if (sm && os_memcmp(sm->identity, WSC_ID_REGISTRAR, sm->identity_len) == 0) {
            wpa_printf(MSG_ERROR, "wps enable: wpsreg already enabled cannot enable wps enrollee");
            ret = ESP_ERR_WIFI_MODE;
        } else {
            wpa_printf(MSG_DEBUG, "wps enable: already enabled");
        }
        API_MUTEX_GIVE();
        return ret;
    }

#ifdef USE_WPS_TASK
    ret = wps_task_init();
    if (ESP_OK != ret) {
        API_MUTEX_GIVE();
        return ret;
    }

    ret = wps_post_block(SIG_WPS_ENABLE, (esp_wps_config_t *)config);
    if (ESP_OK != ret) {
        wps_task_deinit();
        API_MUTEX_GIVE();
        return ret;
    }

    s_wps_enabled = true;
    wpa_printf(MSG_DEBUG, "wifi wps task: prio:%d, stack:%d", 2, WPS_TASK_STACK_SIZE);
    API_MUTEX_GIVE();
    return ret;
#else
    ret = wifi_wps_enable_internal(config);
    API_MUTEX_GIVE();
    return ret;
#endif
}

bool is_wps_enabled(void)
{
    return s_wps_enabled;
}

int wifi_wps_enable_internal(const esp_wps_config_t *config)
{
    int ret = 0;
    struct wpa_sm *wpa_sm = &gWpaSm;

    wpa_printf(MSG_DEBUG, "ESP WPS crypto initialize!");
    if (config->wps_type == WPS_TYPE_DISABLE) {
        wpa_printf(MSG_ERROR, "wps enable: invalid wps type");
        return ESP_ERR_WIFI_WPS_TYPE;
    }
    wpa_printf(MSG_DEBUG, "Set factory information.");
    ret = wps_set_factory_info(config);
    if (ret != 0) {
        return ret;
    }

    wpa_printf(MSG_INFO, "wifi_wps_enable");

    wps_set_type(config->wps_type);
    wps_set_status(WPS_STATUS_DISABLE);

    ret = wifi_station_wps_init(config);

    if (ret != 0) {
        wps_set_type(WPS_TYPE_DISABLE);
        wps_set_status(WPS_STATUS_DISABLE);
        return ESP_FAIL;
    }
    wpa_sm->wpa_sm_wps_disable = esp_wifi_wps_disable;
    return ESP_OK;
}

int wifi_wps_disable_internal(void)
{
    wps_set_status(WPS_STATUS_DISABLE);

    /* Call wps_delete_timer to delete all WPS timer, no timer will call wps_post()
     * to post message to wps_task once this function returns.
     */
    wps_delete_timer();
    wifi_station_wps_deinit();
    return ESP_OK;
}

int esp_wifi_wps_disable(void)
{
    int ret = 0;
    int wps_status;
    struct wps_sm *wps_sm = gWpsSm;
    struct wpa_sm *wpa_sm = &gWpaSm;

    if (wps_sm && os_memcmp(wps_sm->identity, WSC_ID_REGISTRAR, wps_sm->identity_len) == 0) {
        return ESP_ERR_WIFI_MODE;
    }

    API_MUTEX_TAKE();

    if (!s_wps_enabled) {
        wpa_printf(MSG_DEBUG, "wps disable: already disabled");
        API_MUTEX_GIVE();
        return ESP_OK;
    }

    wps_status = wps_get_status();
    wpa_printf(MSG_INFO, "wifi_wps_disable");
    wps_set_type(WPS_TYPE_DISABLE); /* Notify WiFi task */

#ifdef USE_WPS_TASK
    ret = wps_post_block(SIG_WPS_DISABLE, 0);
#else
    ret = wifi_wps_disable_internal();
#endif

    if (ESP_OK != ret) {
        wpa_printf(MSG_ERROR, "wps disable: failed to disable wps, ret=%d", ret);
    }

    /* Only disconnect in case of WPS pending/done */
    if ((wps_status == WPS_STATUS_PENDING) || (wps_status == WPS_STATUS_SUCCESS)) {
        esp_wifi_disconnect();
    }
    esp_wifi_set_wps_start_flag_internal(false);
    wps_task_deinit();
    s_wps_enabled = false;
    API_MUTEX_GIVE();
    wpa_sm->wpa_sm_wps_disable = NULL;
    return ESP_OK;
}

int esp_wifi_wps_start(int timeout_ms)
{
    if (ESP_OK != wps_check_wifi_mode()) {
        return ESP_ERR_WIFI_MODE;
    }

    API_MUTEX_TAKE();

    if (!s_wps_enabled) {
        wpa_printf(MSG_ERROR, "wps start: wps not enabled");
        API_MUTEX_GIVE();
        return ESP_ERR_WIFI_WPS_SM;
    }

    if (wps_get_type() == WPS_TYPE_DISABLE || (wps_get_status() != WPS_STATUS_DISABLE && wps_get_status() != WPS_STATUS_SCANNING)) {
        API_MUTEX_GIVE();
        return ESP_ERR_WIFI_WPS_TYPE;
    }

    if (esp_wifi_get_user_init_flag_internal() == 0) {
        API_MUTEX_GIVE();
        return ESP_ERR_WIFI_STATE;
    }

    wpa_printf(MSG_DEBUG, "wps scan");
#ifdef USE_WPS_TASK
    wps_post_block(SIG_WPS_START, 0);
#else
    ic_pp_post(SIG_PP_WPS, 0);
#endif
    API_MUTEX_GIVE();
    return ESP_OK;
}
