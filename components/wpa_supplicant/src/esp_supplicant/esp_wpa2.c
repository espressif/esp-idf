// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "esp_err.h"

#include "utils/includes.h"
#include "utils/common.h"
#include "utils/wpa_debug.h"
#include "common/wpa_ctrl.h"
#include "common/eapol_common.h"
#include "common/ieee802_11_defs.h"
#include "utils/state_machine.h"
#include "rsn_supp/wpa.h"

#include "crypto/crypto.h"

#include "utils/ext_password.h"
#include "tls/tls.h"
#include "eap_peer/eap_i.h"
#include "eap_peer/eap_config.h"
#include "eap_peer/eap.h"
#include "eap_peer/eap_tls.h"
#ifdef EAP_PEER_METHOD
#include "eap_peer/eap_methods.h"
#endif

#include "esp_wifi_driver.h"
#include "esp_private/wifi.h"
#include "esp_wpa_err.h"

#define WPA2_VERSION    "v2.0"

#define DATA_MUTEX_TAKE() xSemaphoreTakeRecursive(s_wpa2_data_lock,portMAX_DELAY)
#define DATA_MUTEX_GIVE() xSemaphoreGiveRecursive(s_wpa2_data_lock)

//length of the string "fast_provisioning={0/1/2} "
#define FAST_PROVISIONING_CONFIG_STR_LEN 20
//length of the string "fast_max_pac_list_len=(int < 100) "
#define FAST_MAX_PAC_LIST_CONFIG_STR_LEN 25
//length of the string "fast_pac_format=binary"
#define FAST_PAC_FORMAT_STR_LEN 22
//Total
#define PHASE1_PARAM_STRING_LEN FAST_PROVISIONING_CONFIG_STR_LEN + FAST_MAX_PAC_LIST_CONFIG_STR_LEN + FAST_PAC_FORMAT_STR_LEN

static void *s_wpa2_data_lock = NULL;

static struct eap_sm *gEapSm = NULL;

static int eap_peer_sm_init(void);
static void eap_peer_sm_deinit(void);

static int eap_sm_rx_eapol_internal(u8 *src_addr, u8 *buf, u32 len, uint8_t *bssid);
static int wpa2_start_eapol_internal(void);
int wpa2_post(uint32_t sig, uint32_t par);

#ifdef USE_WPA2_TASK
static TaskHandle_t s_wpa2_task_hdl = NULL;
static void *s_wpa2_queue = NULL;
static wpa2_state_t s_wpa2_state = WPA2_STATE_DISABLED;
static void *s_wpa2_api_lock = NULL;
static void *s_wifi_wpa2_sync_sem = NULL;
static bool s_disable_time_check = true;

static void wpa2_api_lock(void)
{
    if (s_wpa2_api_lock == NULL) {
        s_wpa2_api_lock = xSemaphoreCreateRecursiveMutex();
        if (!s_wpa2_api_lock) {
            wpa_printf(MSG_ERROR, "WPA2: failed to create wpa2 api lock");
            return;
        }
    }

    xSemaphoreTakeRecursive(s_wpa2_api_lock, portMAX_DELAY);
}

static void wpa2_api_unlock(void)
{
    if (s_wpa2_api_lock) {
        xSemaphoreGiveRecursive(s_wpa2_api_lock);
    }
}

static bool inline wpa2_is_enabled(void)
{
    return (s_wpa2_state == WPA2_STATE_ENABLED);
}

static bool inline wpa2_is_disabled(void)
{
    return (s_wpa2_state == WPA2_STATE_DISABLED);
}

static void inline wpa2_set_state(wpa2_state_t state)
{
    s_wpa2_state = state;
}

static void wpa2_set_eap_state(wpa2_ent_eap_state_t state)
{
    if (!gEapSm) {
        return;
    }

    gEapSm->finish_state = state;
    esp_wifi_set_wpa2_ent_state_internal(state);
}

static inline void wpa2_task_delete(void *arg)
{
    void *my_task_hdl = xTaskGetCurrentTaskHandle();
    int ret = ESP_OK;

    if (my_task_hdl == s_wpa2_task_hdl) {
        wpa_printf(MSG_ERROR, "WPA2: should never call task delete api in wpa2 task context");
        return;
    }

    ret = wpa2_post(SIG_WPA2_TASK_DEL, 0);

    if (ESP_OK != ret) {
        wpa_printf(MSG_ERROR, "WPA2: failed to post task delete event, ret=%d", ret);
        return;
    }
}

#define WPA_ADDR_LEN 6
struct wpa2_rx_param {
    uint8_t *bssid;
    u8 sa[WPA_ADDR_LEN];
    u8 *buf;
    int len;
    STAILQ_ENTRY(wpa2_rx_param) bqentry;
};
static STAILQ_HEAD(, wpa2_rx_param) s_wpa2_rxq;

static void wpa2_rxq_init(void)
{
    DATA_MUTEX_TAKE();
    STAILQ_INIT(&s_wpa2_rxq);
    DATA_MUTEX_GIVE();
}

static void wpa2_rxq_enqueue(struct wpa2_rx_param *param)
{
    DATA_MUTEX_TAKE();
    STAILQ_INSERT_TAIL(&s_wpa2_rxq,param, bqentry);
    DATA_MUTEX_GIVE();
}

static struct wpa2_rx_param * wpa2_rxq_dequeue(void)
{
    struct wpa2_rx_param *param = NULL;
    DATA_MUTEX_TAKE();
    if ((param = STAILQ_FIRST(&s_wpa2_rxq)) != NULL) {
        STAILQ_REMOVE_HEAD(&s_wpa2_rxq, bqentry);
        STAILQ_NEXT(param,bqentry) = NULL;
    }
    DATA_MUTEX_GIVE();
    return param;
}

static void wpa2_rxq_deinit(void)
{
    struct wpa2_rx_param *param = NULL;
    DATA_MUTEX_TAKE();
    while ((param = STAILQ_FIRST(&s_wpa2_rxq)) != NULL) {
        STAILQ_REMOVE_HEAD(&s_wpa2_rxq, bqentry);
        STAILQ_NEXT(param,bqentry) = NULL;
        os_free(param->buf);
        os_free(param);
    }
    DATA_MUTEX_GIVE();
}

void wpa2_task(void *pvParameters )
{
    ETSEvent *e;
    struct eap_sm *sm = gEapSm;
    bool task_del = false;

    if (!sm) {
        return;
    }

    for (;;) {
        if ( pdPASS == xQueueReceive(s_wpa2_queue, &e, portMAX_DELAY) ) {
            if (e->sig < SIG_WPA2_MAX) {
                DATA_MUTEX_TAKE();
                if(sm->wpa2_sig_cnt[e->sig]) {
                    sm->wpa2_sig_cnt[e->sig]--;
                } else {
                    wpa_printf(MSG_ERROR, "wpa2_task: invalid sig cnt, sig=%d cnt=%d", e->sig, sm->wpa2_sig_cnt[e->sig]);
                }
                DATA_MUTEX_GIVE();
            }
            switch (e->sig) {
            case SIG_WPA2_TASK_DEL:
                task_del = true;
                break;
            case SIG_WPA2_START:
                wpa2_start_eapol_internal();
                break;
            case SIG_WPA2_RX: {
                struct wpa2_rx_param *param = NULL;

                while ((param = wpa2_rxq_dequeue()) != NULL){
                    eap_sm_rx_eapol_internal(param->sa, param->buf, param->len, param->bssid);
                    os_free(param->buf);
                    os_free(param);
                }
                break;
            }
            default:
                break;
            }
            os_free(e);
        }

        if (task_del) {
            break;
        } else {
            if (s_wifi_wpa2_sync_sem) {
                wpa_printf(MSG_DEBUG, "WPA2: wifi->wpa2 api completed sig(%d)", e->sig);
                xSemaphoreGive(s_wifi_wpa2_sync_sem);
            } else {
                wpa_printf(MSG_ERROR, "WPA2: null wifi->wpa2 sync sem");
            }
        }
    }

    wpa_printf(MSG_DEBUG, "WPA2: queue deleted");
    vQueueDelete(s_wpa2_queue);
    wpa_printf(MSG_DEBUG, "WPA2: task deleted");
    s_wpa2_queue = NULL;
    if (s_wifi_wpa2_sync_sem) {
        wpa_printf(MSG_DEBUG, "WPA2: wifi->wpa2 api completed sig(%d)", e->sig);
        xSemaphoreGive(s_wifi_wpa2_sync_sem);
    } else {
        wpa_printf(MSG_ERROR, "WPA2: null wifi->wpa2 sync sem");
    }

    /* At this point, we completed */
    vTaskDelete(NULL);
}

int wpa2_post(uint32_t sig, uint32_t par)
{
    struct eap_sm *sm = gEapSm;

    if (!sm) {
        return ESP_FAIL;
    }

    DATA_MUTEX_TAKE();
    if (sm->wpa2_sig_cnt[sig]) {
        DATA_MUTEX_GIVE();
        return ESP_OK;
    } else {
        ETSEvent *evt = (ETSEvent *)os_malloc(sizeof(ETSEvent));
        if (evt == NULL) {
            wpa_printf(MSG_ERROR, "WPA2: E N M\n");
            DATA_MUTEX_GIVE();
            return ESP_FAIL;
        }
        sm->wpa2_sig_cnt[sig]++;
        DATA_MUTEX_GIVE();
        evt->sig = sig;
        evt->par = par;
        if ( xQueueSend(s_wpa2_queue, &evt, 10 / portTICK_PERIOD_MS ) != pdPASS) {
            wpa_printf(MSG_ERROR, "WPA2: Q S E");
            return ESP_FAIL;
        } else {
            if (s_wifi_wpa2_sync_sem) {
                xSemaphoreTake(s_wifi_wpa2_sync_sem, portMAX_DELAY);
                wpa_printf(MSG_DEBUG, "WPA2: wpa2 api return, sm->state(%d)", sm->finish_state);
            } else {
                wpa_printf(MSG_ERROR, "WPA2: null wifi->wpa2 sync sem");
            }
        }
    }
    return ESP_OK;
}

#endif /* USE_WPA2_TASK */

static void wpa2_sendto_wrapper(void *buffer, uint16_t len)
{
    esp_wifi_internal_tx(WIFI_IF_STA, buffer, len);
}

static inline int wpa2_sm_ether_send(struct eap_sm *sm, const u8 *dest, u16 proto,
                                     const u8 *data, size_t data_len)
{
    void *buffer = (void *)(data - sizeof(struct l2_ethhdr));
    struct l2_ethhdr *eth = NULL;

    if (!buffer) {
        wpa_printf(MSG_ERROR, "wpa2: invalid data");
        return ESP_FAIL;
    } else {
        eth = (struct l2_ethhdr *)buffer;
        memcpy(eth->h_dest, dest, ETH_ALEN);
        memcpy(eth->h_source, sm->ownaddr, ETH_ALEN);
        eth->h_proto = host_to_be16(proto);
        wpa2_sendto_wrapper(buffer, sizeof(struct l2_ethhdr) + data_len);
    }

    return ESP_OK;
}

u8 *wpa2_sm_alloc_eapol(struct eap_sm *sm, u8 type,
                        const void *data, u16 data_len,
                        size_t *msg_len, void **data_pos)
{
    void *buffer;
    struct ieee802_1x_hdr *hdr;

    *msg_len = sizeof(struct ieee802_1x_hdr) + data_len;
    /* XXX: reserve l2_ethhdr is enough */
    buffer = os_malloc(*msg_len + sizeof(struct l2_ethhdr));

    if (buffer == NULL) {
        return NULL;
    }

    hdr = (struct ieee802_1x_hdr *)((char *)buffer + sizeof(struct l2_ethhdr));
    hdr->version = 0x01;
    hdr->type = type;
    hdr->length = host_to_be16(data_len);

    if (data) {
        memcpy(hdr + 1, data, data_len);
    } else {
        memset(hdr + 1, 0, data_len);
    }

    if (data_pos) {
        *data_pos = hdr + 1;
    }

    return (u8 *) hdr;
}


void wpa2_sm_free_eapol(u8 *buffer)
{
    if (buffer != NULL) {
        buffer = buffer - sizeof(struct l2_ethhdr);
        os_free(buffer);
    }

}

int eap_sm_send_eapol(struct eap_sm *sm, struct wpabuf *resp)
{
    size_t outlen;
    int ret;
    u8 *outbuf = NULL;

    u8 bssid[6];
    ret = esp_wifi_get_assoc_bssid_internal(bssid);

    if (ret != 0) {
        wpa_printf(MSG_DEBUG, "bssid is empty \n");
        return WPA_ERR_INVALID_BSSID;
    }

    outbuf = wpa2_sm_alloc_eapol(sm, IEEE802_1X_TYPE_EAP_PACKET,
                                 wpabuf_head_u8(resp), wpabuf_len(resp),
                                 &outlen, NULL);
    if (!outbuf) {
        return ESP_ERR_NO_MEM;
    }

    ret = wpa2_sm_ether_send(sm, bssid, ETH_P_EAPOL, outbuf, outlen);
    wpa2_sm_free_eapol(outbuf);
    if (ret) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

int eap_sm_process_request(struct eap_sm *sm, struct wpabuf *reqData)
{
    size_t plen;
    u32 reqVendor, reqVendorMethod;
    u8 type, *pos;
    struct eap_hdr *ehdr;
    const struct eap_method *m = NULL;
    struct wpabuf *resp = NULL;
    struct eap_method_ret m_res;
    int ret = 0;

    if (reqData == NULL || wpabuf_len(reqData) < sizeof(*ehdr)) {
        return ESP_ERR_INVALID_ARG;
    }

    ehdr = (struct eap_hdr *)wpabuf_head(reqData);
    plen = be_to_host16(ehdr->length);
    if (plen > wpabuf_len(reqData)) {
        return ESP_FAIL;
    }

    if (ehdr->identifier == sm->current_identifier &&
        sm->lastRespData != NULL) {
        /*Retransmit*/
        resp = sm->lastRespData;
        goto send_resp;
    }

    sm->current_identifier = ehdr->identifier;

    pos = (u8 *)(ehdr + 1);
    type = *pos++;
    if (type == EAP_TYPE_IDENTITY) {
        resp = (struct wpabuf *)eap_sm_build_identity_resp(sm, ehdr->identifier, 0);
        goto send_resp;
    } else if (type == EAP_TYPE_NOTIFICATION) {
        /*Ignore*/
        goto out;
    } else if (type == EAP_TYPE_EXPANDED) {
        if (plen < sizeof(*ehdr) + 8) {
            return ESP_FAIL;
        }
        reqVendor = WPA_GET_BE24(pos);
        pos += 3;
        reqVendorMethod = WPA_GET_BE32(pos);
    } else {
        reqVendor = EAP_VENDOR_IETF;
        reqVendorMethod = type;
    }

    if (sm->m && sm->m->process && sm->eap_method_priv &&
            reqVendor == sm->m->vendor &&
            reqVendorMethod == sm->m->method) {
        resp = sm->m->process(sm, sm->eap_method_priv,
                              &m_res, reqData);
    } else {
        m = eap_peer_get_eap_method(reqVendor, reqVendorMethod);
        if (m == NULL) {
            goto build_nak;
        }

        if (!eap_sm_allowMethod(sm, reqVendor, reqVendorMethod)) {
            wpa_printf(MSG_DEBUG, "EAP: vendor %u method %u not allowed",
                    reqVendor, reqVendorMethod);
            wpa_msg(sm->msg_ctx, MSG_INFO, WPA_EVENT_EAP_PROPOSED_METHOD
                    "vendor=%u method=%u -> NAK",
                    reqVendor, reqVendorMethod);
            goto build_nak;
        }
        if (sm->m) {
            eap_deinit_prev_method(sm, "GET_METHOD");
        }
        sm->m = m;
        sm->eap_method_priv = sm->m->init(sm);
        if (sm->eap_method_priv == NULL) {
            wpa_printf(MSG_ERROR, "Method private structure allocated failure\n");
            sm->m = NULL;
            goto build_nak;
        }

        if (sm->m->process) {
            resp = sm->m->process(sm, sm->eap_method_priv, &m_res, reqData);
        }
    }

    if (sm->m->isKeyAvailable && sm->m->getKey &&
            sm->m->isKeyAvailable(sm, sm->eap_method_priv)) {
        if (sm->eapKeyData) {
            os_free(sm->eapKeyData);
        }
        sm->eapKeyData = sm->m->getKey(sm, sm->eap_method_priv,
                                       &sm->eapKeyDataLen);
    }
    goto send_resp;

build_nak:
    resp = (struct wpabuf *)eap_sm_build_nak(sm, type, ehdr->identifier);
    if (resp == NULL) {
        return ESP_FAIL;
    }
send_resp:
    if (resp == NULL) {
        wpa_printf(MSG_ERROR, "Response build fail, return.");
        return ESP_FAIL;
    }
    ret = eap_sm_send_eapol(sm, resp);
    if (resp != sm->lastRespData) {
        wpabuf_free(sm->lastRespData);
    }
    if (ret != ESP_OK) {
        wpabuf_free(resp);
        resp = NULL;
        if (ret == WPA_ERR_INVALID_BSSID) {
            ret = WPA2_ENT_EAP_STATE_FAIL;
            wpa2_set_eap_state(WPA2_ENT_EAP_STATE_FAIL);
        }
    }
    sm->lastRespData = resp;
out:
    return ret;
}

static int eap_sm_rx_eapol(u8 *src_addr, u8 *buf, u32 len, uint8_t *bssid)
{
    struct eap_sm *sm = gEapSm;

    if (!sm) {
        return ESP_FAIL;
    }
#ifdef USE_WPA2_TASK
    {
        struct wpa2_rx_param *param = (struct wpa2_rx_param *)os_zalloc(sizeof(struct wpa2_rx_param));   /* free in task */

        if (!param) {
            return ESP_ERR_NO_MEM;
        }

        param->buf = (u8 *)os_zalloc(len);   /* free in task */
        if (!param->buf) {
            os_free(param);
            return ESP_ERR_NO_MEM;
        }
        param->bssid = bssid;
        memcpy(param->buf, buf, len);
        param->len = len;
        memcpy(param->sa, src_addr, WPA_ADDR_LEN);

        wpa2_rxq_enqueue(param);
        return wpa2_post(SIG_WPA2_RX, 0);
    }
#else

    return eap_sm_rx_eapol_internal(src_addr, buf, len, bssid);
#endif
}

static int wpa2_ent_rx_eapol(u8 *src_addr, u8 *buf, u32 len, uint8_t *bssid)
{
    struct ieee802_1x_hdr *hdr;
    int ret = ESP_OK;

    hdr = (struct ieee802_1x_hdr *) buf;

    switch (hdr->type) {
	    case IEEE802_1X_TYPE_EAPOL_START:
	    case IEEE802_1X_TYPE_EAP_PACKET:
	    case IEEE802_1X_TYPE_EAPOL_LOGOFF:
		    ret = eap_sm_rx_eapol(src_addr, buf, len, bssid);
		    break;
	    case IEEE802_1X_TYPE_EAPOL_KEY:
            ret = wpa_sm_rx_eapol(src_addr, buf, len);
		    break;
	    default:
		wpa_printf(MSG_ERROR, "Unknown EAPOL packet type - %d\n", hdr->type);
		    break;
    }

	return ret;
}

static int eap_sm_rx_eapol_internal(u8 *src_addr, u8 *buf, u32 len, uint8_t *bssid)
{
    struct eap_sm *sm = gEapSm;
    u32 plen, data_len;
    struct ieee802_1x_hdr *hdr;
    struct eap_hdr *ehdr;
    struct wpabuf *req = NULL;
    u8 *tmp;
    int ret = ESP_FAIL;

    if (!sm) {
        return ESP_FAIL;
    }

    if (len < sizeof(*hdr) + sizeof(*ehdr)) {
#ifdef DEBUG_PRINT
        wpa_printf(MSG_DEBUG, "WPA: EAPOL frame too short to be a WPA "
                   "EAPOL-Key (len %lu, expecting at least %lu)",
                   (unsigned long) len,
                   (unsigned long) sizeof(*hdr) + sizeof(*ehdr));
#endif
        return ESP_FAIL;
    }

    tmp = buf;

    hdr = (struct ieee802_1x_hdr *) tmp;
    ehdr = (struct eap_hdr *) (hdr + 1);
    plen = be_to_host16(hdr->length);
    data_len = plen + sizeof(*hdr);

#ifdef DEBUG_PRINT
    wpa_printf(MSG_DEBUG, "IEEE 802.1X RX: version=%d type=%d length=%d\n",
               hdr->version, hdr->type, plen);
#endif
    if (hdr->version < EAPOL_VERSION) {
        /* TODO: backwards compatibility */
    }
    if (hdr->type != IEEE802_1X_TYPE_EAP_PACKET) {
#ifdef DEBUG_PRINT
        wpa_printf(MSG_DEBUG, "WPA2: EAP frame (type %u) discarded, "
                   "not a EAP PACKET frame", hdr->type);
#endif
        ret = -2;
        goto _out;
    }
    if (plen > len - sizeof(*hdr) || plen < sizeof(*ehdr)) {
#ifdef DEBUG_PRINT
        wpa_printf(MSG_DEBUG, "WPA2: EAPOL frame payload size %lu "
                   "invalid (frame size %lu)",
                   (unsigned long) plen, (unsigned long) len);
#endif
        ret = -2;
        goto _out;
    }

    wpa_hexdump(MSG_MSGDUMP, "WPA2: RX EAPOL-EAP PACKET", tmp, len);

    if (data_len < len) {
#ifdef DEBUG_PRINT
        wpa_printf(MSG_DEBUG, "WPA: ignoring %lu bytes after the IEEE "
                   "802.1X data\n", (unsigned long) len - data_len);
#endif
    }

#ifdef EAP_PEER_METHOD
    switch (ehdr->code) {
    case EAP_CODE_REQUEST:
        /* Handle EAP-reauthentication case */
        if (sm->finish_state == WPA2_ENT_EAP_STATE_SUCCESS) {
                wpa_printf(MSG_INFO, ">>>>>wpa2 EAP Re-authentication in progress\n");
		wpa2_set_eap_state(WPA2_ENT_EAP_STATE_IN_PROGRESS);
	}

        req = wpabuf_alloc_copy((u8 *)ehdr, len - sizeof(*hdr));
        ret = eap_sm_process_request(sm, req);
        break;
    case EAP_CODE_RESPONSE:
        /*Ignore*/
        break;
    case EAP_CODE_SUCCESS:
        if (sm->eapKeyData) {
            wpa_set_pmk(sm->eapKeyData, NULL, false);
            os_free(sm->eapKeyData);
            sm->eapKeyData = NULL;
            wpa_printf(MSG_INFO, ">>>>>wpa2 FINISH\n");
            ret = WPA2_ENT_EAP_STATE_SUCCESS;
            wpa2_set_eap_state(WPA2_ENT_EAP_STATE_SUCCESS);
	    eap_deinit_prev_method(sm, "EAP Success");
        } else {
            wpa_printf(MSG_INFO, ">>>>>wpa2 FAILED, receive EAP_SUCCESS but pmk is empty, potential attack!\n");
            ret = WPA2_ENT_EAP_STATE_FAIL;
            wpa2_set_eap_state(WPA2_ENT_EAP_STATE_FAIL);
        }
        break;
    case EAP_CODE_FAILURE:
        wpa_printf(MSG_INFO, ">>>>>wpa2 FAILED\n");
        ret = WPA2_ENT_EAP_STATE_FAIL;
        wpa2_set_eap_state(WPA2_ENT_EAP_STATE_FAIL);
        break;
    }
_out:
    wpabuf_free(req);
#endif
    return ret;
}

static int wpa2_start_eapol(void)
{
#ifdef USE_WPA2_TASK
    return wpa2_post(SIG_WPA2_START, 0);
#else
    return wpa2_start_eapol_internal();
#endif
}

static int wpa2_start_eapol_internal(void)
{
    struct eap_sm *sm = gEapSm;
    int ret = 0;
    u8 bssid[6];
    u8 *buf;
    size_t len;

    if (!sm) {
        return ESP_FAIL;
    }

    if (wpa_sta_cur_pmksa_matches_akm()) {
        wpa_printf(MSG_DEBUG,
                "RSN: PMKSA caching - do not send EAPOL-Start");
        return ESP_FAIL;
    }

    ret = esp_wifi_get_assoc_bssid_internal(bssid);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "bssid is empty!");
        return WPA_ERR_INVALID_BSSID;
    }

    buf = wpa2_sm_alloc_eapol(sm, IEEE802_1X_TYPE_EAPOL_START, (u8 *)"", 0, &len, NULL);
    if (!buf) {
        return ESP_FAIL;
    }

    wpa2_set_eap_state(WPA2_ENT_EAP_STATE_IN_PROGRESS);
    wpa2_sm_ether_send(sm, bssid, ETH_P_EAPOL, buf, len);
    wpa2_sm_free_eapol(buf);
    return ESP_OK;
}

/**
 * eap_peer_sm_init - Allocate and initialize EAP peer state machine
 * @eapol_ctx: Context data to be used with eapol_cb calls
 * @eapol_cb: Pointer to EAPOL callback functions
 * @msg_ctx: Context data for wpa_msg() calls
 * @conf: EAP configuration
 * Returns: Pointer to the allocated EAP state machine or %NULL on failure
 *
 * This function allocates and initializes an EAP state machine. In addition,
 * this initializes TLS library for the new EAP state machine. eapol_cb pointer
 * will be in use until eap_peer_sm_deinit() is used to deinitialize this EAP
 * state machine. Consequently, the caller must make sure that this data
 * structure remains alive while the EAP state machine is active.
 */
static int eap_peer_sm_init(void)
{
    int ret = 0;
    struct eap_sm *sm;

    if (gEapSm) {
        wpa_printf(MSG_ERROR, "WPA2: wpa2 sm not null, deinit it");
        eap_peer_sm_deinit();
    }

    sm = (struct eap_sm *)os_zalloc(sizeof(*sm));
    if (sm == NULL) {
        ret = ESP_ERR_NO_MEM;
        return ret;
    }

    gEapSm = sm;
    s_wpa2_data_lock = xSemaphoreCreateRecursiveMutex();
    if (!s_wpa2_data_lock) {
        wpa_printf(MSG_ERROR, "wpa2 eap_peer_sm_init: failed to alloc data lock");
        ret = ESP_ERR_NO_MEM;
	goto _err;
    }

    wpa2_set_eap_state(WPA2_ENT_EAP_STATE_NOT_START);
    sm->current_identifier = 0xff;
    esp_wifi_get_macaddr_internal(WIFI_IF_STA, sm->ownaddr);
    ret = eap_peer_blob_init(sm);
    if (ret) {
        wpa_printf(MSG_ERROR, "eap_peer_blob_init failed\n");
        ret = ESP_FAIL;
        goto _err;
    }

    ret = eap_peer_config_init(sm, g_wpa_private_key_passwd, g_wpa_private_key_passwd_len);
    if (ret) {
        wpa_printf(MSG_ERROR, "eap_peer_config_init failed\n");
        ret = ESP_FAIL;
        goto _err;
    }

    sm->ssl_ctx = tls_init();
    if (sm->ssl_ctx == NULL) {
        wpa_printf(MSG_WARNING, "SSL: Failed to initialize TLS "
                   "context.");
        ret = ESP_FAIL;
        goto _err;
    }

    wpa2_rxq_init();

    gEapSm = sm;
#ifdef USE_WPA2_TASK
    s_wpa2_queue = xQueueCreate(SIG_WPA2_MAX, sizeof( s_wpa2_queue ) );
    ret = xTaskCreate(wpa2_task, "wpa2T", WPA2_TASK_STACK_SIZE, NULL, 2, &s_wpa2_task_hdl);
    if (ret != pdPASS) {
        wpa_printf(MSG_ERROR, "wps enable: failed to create task");
        ret = ESP_FAIL;
        goto _err;
    }
    s_wifi_wpa2_sync_sem = xSemaphoreCreateCounting(1, 0);
    if (!s_wifi_wpa2_sync_sem) {
        wpa_printf(MSG_ERROR, "WPA2: failed create wifi wpa2 task sync sem");
        ret = ESP_FAIL;
        goto _err;
    }

    wpa_printf(MSG_INFO, "wpa2_task prio:%d, stack:%d\n", 2, WPA2_TASK_STACK_SIZE);

#endif
    return ESP_OK;

_err:
    eap_peer_sm_deinit();
    return ret;
}

/**
 * eap_peer_sm_deinit - Deinitialize and free an EAP peer state machine
 * @sm: Pointer to EAP state machine allocated with eap_peer_sm_init()
 *
 * This function deinitializes EAP state machine and frees all allocated
 * resources.
 */
static void eap_peer_sm_deinit(void)
{
    struct eap_sm *sm = gEapSm;

    if (sm == NULL) {
        return;
    }

    eap_peer_config_deinit(sm);
    eap_peer_blob_deinit(sm);
    eap_deinit_prev_method(sm, "EAP deinit");
    eap_sm_abort(sm);
    tls_deinit(sm->ssl_ctx);
#ifdef USE_WPA2_TASK
    wpa2_task_delete(0);
#endif

    if (STAILQ_FIRST((&s_wpa2_rxq)) != NULL) {
        wpa2_rxq_deinit();
    }

    if (s_wifi_wpa2_sync_sem) {
        vSemaphoreDelete(s_wifi_wpa2_sync_sem);
        s_wifi_wpa2_sync_sem = NULL;
    }

    if (s_wpa2_data_lock) {
        vSemaphoreDelete(s_wpa2_data_lock);
        s_wpa2_data_lock = NULL;
        wpa_printf(MSG_DEBUG, "wpa2 eap_peer_sm_deinit: free data lock");
    }

    if (s_wpa2_queue) {
        vQueueDelete(s_wpa2_queue);
        s_wpa2_queue = NULL;
    }
    os_free(sm);
    gEapSm = NULL;
}

esp_err_t esp_wifi_sta_wpa2_ent_enable_fn(void *arg)
{
    struct wpa2_funcs *wpa2_cb;

    wpa_printf(MSG_INFO, "WPA2 ENTERPRISE VERSION: [%s] enable\n",
               WPA2_VERSION);

    wpa2_cb = (struct wpa2_funcs *)os_zalloc(sizeof(struct wpa2_funcs));
    if (wpa2_cb == NULL) {
        wpa_printf(MSG_ERROR, "WPA2: no mem for wpa2 cb\n");
        return ESP_ERR_NO_MEM;
    }

    wpa2_cb->wpa2_sm_rx_eapol = wpa2_ent_rx_eapol;
    wpa2_cb->wpa2_start = wpa2_start_eapol;
    wpa2_cb->wpa2_init = eap_peer_sm_init;
    wpa2_cb->wpa2_deinit = eap_peer_sm_deinit;

    esp_wifi_register_wpa2_cb_internal(wpa2_cb);

    wpa_printf(MSG_DEBUG, "WPA2 ENTERPRISE CRYPTO INIT.\r\n");

#ifdef EAP_PEER_METHOD
    if (eap_peer_register_methods()) {
        wpa_printf(MSG_ERROR, "Register EAP Peer methods Failure\n");
    }
#endif
    return ESP_OK;
}

esp_err_t esp_wifi_sta_wpa2_ent_enable(void)
{
    wifi_wpa2_param_t param;
    esp_err_t ret;

    wpa2_api_lock();

    if (wpa2_is_enabled()) {
        wpa_printf(MSG_INFO, "WPA2: already enabled");
        wpa2_api_unlock();
        return ESP_OK;
    }

    param.fn = (wifi_wpa2_fn_t)esp_wifi_sta_wpa2_ent_enable_fn;
    param.param = NULL;

    ret = esp_wifi_sta_wpa2_ent_enable_internal(&param);

    if (ESP_OK == ret) {
        wpa2_set_state(WPA2_STATE_ENABLED);
    } else {
        wpa_printf(MSG_ERROR, "failed to enable wpa2 ret=%d", ret);
    }

    wpa2_api_unlock();

    return ret;
}

esp_err_t esp_wifi_sta_wpa2_ent_disable_fn(void *param)
{
    wpa_printf(MSG_INFO, "WPA2 ENTERPRISE VERSION: [%s] disable\n", WPA2_VERSION);
    esp_wifi_unregister_wpa2_cb_internal();

    if (gEapSm) {
        eap_peer_sm_deinit();
    }

#ifdef USE_WPA2_TASK
#endif

#ifdef EAP_PEER_METHOD
    eap_peer_unregister_methods();
#endif

    return ESP_OK;
}

esp_err_t esp_wifi_sta_wpa2_ent_disable(void)
{
    wifi_wpa2_param_t param;
    esp_err_t ret;

    wpa2_api_lock();

    if (wpa2_is_disabled()) {
        wpa_printf(MSG_INFO, "WPA2: already disabled");
        wpa2_api_unlock();
        return ESP_OK;
    }

    param.fn = (wifi_wpa2_fn_t)esp_wifi_sta_wpa2_ent_disable_fn;
    param.param = 0;
    ret = esp_wifi_sta_wpa2_ent_disable_internal(&param);

    if (ESP_OK == ret) {
        wpa2_set_state(WPA2_STATE_DISABLED);
    } else {
        wpa_printf(MSG_ERROR, "failed to disable wpa2 ret=%d", ret);
    }

    wpa2_api_unlock();

    return ret;
}

esp_err_t esp_wifi_sta_wpa2_ent_set_cert_key(const unsigned char *client_cert, int client_cert_len, const unsigned char *private_key, int private_key_len, const unsigned char *private_key_passwd, int private_key_passwd_len)
{
    if (client_cert && client_cert_len > 0) {
        g_wpa_client_cert = client_cert;
        g_wpa_client_cert_len = client_cert_len;
    }
    if (private_key && private_key_len > 0) {
        g_wpa_private_key = private_key;
        g_wpa_private_key_len = private_key_len;
    }
    if (private_key_passwd && private_key_passwd_len > 0) {
        g_wpa_private_key_passwd = private_key_passwd;
        g_wpa_private_key_passwd_len = private_key_passwd_len;
    }

    return ESP_OK;
}

void esp_wifi_sta_wpa2_ent_clear_cert_key(void)
{
    esp_wifi_unregister_wpa2_cb_internal();

    g_wpa_client_cert = NULL;
    g_wpa_client_cert_len = 0;
    g_wpa_private_key = NULL;
    g_wpa_private_key_len = 0;
    g_wpa_private_key_passwd = NULL;
    g_wpa_private_key_passwd_len = 0;
}

esp_err_t esp_wifi_sta_wpa2_ent_set_ca_cert(const unsigned char *ca_cert, int ca_cert_len)
{
    if (ca_cert && ca_cert_len > 0) {
        g_wpa_ca_cert = ca_cert;
        g_wpa_ca_cert_len = ca_cert_len;
    }

    return ESP_OK;
}

void esp_wifi_sta_wpa2_ent_clear_ca_cert(void)
{
    g_wpa_ca_cert = NULL;
    g_wpa_ca_cert_len = 0;
}

#define ANONYMOUS_ID_LEN_MAX 128
esp_err_t esp_wifi_sta_wpa2_ent_set_identity(const unsigned char *identity, int len)
{
    if (len <= 0 || len > ANONYMOUS_ID_LEN_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    if (g_wpa_anonymous_identity) {
        os_free(g_wpa_anonymous_identity);
        g_wpa_anonymous_identity = NULL;
    }

    g_wpa_anonymous_identity = (u8 *)os_zalloc(len);
    if (g_wpa_anonymous_identity == NULL) {
        return ESP_ERR_NO_MEM;
    }

    os_memcpy(g_wpa_anonymous_identity, identity, len);
    g_wpa_anonymous_identity_len = len;

    return ESP_OK;
}

void esp_wifi_sta_wpa2_ent_clear_identity(void)
{
    if (g_wpa_anonymous_identity) {
        os_free(g_wpa_anonymous_identity);
    }

    g_wpa_anonymous_identity = NULL;
    g_wpa_anonymous_identity_len = 0;
}

#define USERNAME_LEN_MAX 128
esp_err_t esp_wifi_sta_wpa2_ent_set_username(const unsigned char *username, int len)
{
    if (len <= 0 || len > USERNAME_LEN_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    if (g_wpa_username) {
        os_free(g_wpa_username);
        g_wpa_username = NULL;
    }

    g_wpa_username = (u8 *)os_zalloc(len);
    if (g_wpa_username == NULL) {
        return ESP_ERR_NO_MEM;
    }

    os_memcpy(g_wpa_username, username, len);
    g_wpa_username_len = len;

    return ESP_OK;
}

void esp_wifi_sta_wpa2_ent_clear_username(void)
{
    if (g_wpa_username) {
        os_free(g_wpa_username);
    }

    g_wpa_username = NULL;
    g_wpa_username_len = 0;
}

esp_err_t esp_wifi_sta_wpa2_ent_set_password(const unsigned char *password, int len)
{
    if (len <= 0) {
        return ESP_ERR_INVALID_ARG;
    }

    if (g_wpa_password) {
        os_free(g_wpa_password);
        g_wpa_password = NULL;
    }

    g_wpa_password = (u8 *)os_zalloc(len);
    if (g_wpa_password == NULL) {
        return ESP_ERR_NO_MEM;
    }

    os_memcpy(g_wpa_password, password, len);
    g_wpa_password_len = len;

    return ESP_OK;
}

void esp_wifi_sta_wpa2_ent_clear_password(void)
{
    if (g_wpa_password) {
        os_free(g_wpa_password);
    }
    g_wpa_password = NULL;
    g_wpa_password_len = 0;
}

esp_err_t esp_wifi_sta_wpa2_ent_set_new_password(const unsigned char *new_password, int len)
{
    if (len <= 0) {
        return ESP_ERR_INVALID_ARG;
    }

    if (g_wpa_new_password) {
        os_free(g_wpa_new_password);
        g_wpa_new_password = NULL;
    }

    g_wpa_new_password = (u8 *)os_zalloc(len);
    if (g_wpa_new_password == NULL) {
        return ESP_ERR_NO_MEM;
    }

    os_memcpy(g_wpa_new_password, new_password, len);
    g_wpa_password_len = len;

    return ESP_OK;
}

void esp_wifi_sta_wpa2_ent_clear_new_password(void)
{
    if (g_wpa_new_password) {
        os_free(g_wpa_new_password);
    }
    g_wpa_new_password = NULL;
    g_wpa_new_password_len = 0;
}

esp_err_t esp_wifi_sta_wpa2_ent_set_disable_time_check(bool disable)
{
    s_disable_time_check = disable;
    return ESP_OK;
}

bool wifi_sta_get_enterprise_disable_time_check(void)
{
    return s_disable_time_check;
}

esp_err_t esp_wifi_sta_wpa2_ent_get_disable_time_check(bool *disable)
{
    *disable = wifi_sta_get_enterprise_disable_time_check();
    return ESP_OK;
}

esp_err_t esp_wifi_sta_wpa2_ent_set_pac_file(const unsigned char *pac_file, int pac_file_len)
{
    if (pac_file && pac_file_len > -1) {
        if (pac_file_len < 512) { // The file contains less than 1 pac and is to be rewritten later
            g_wpa_pac_file = (u8 *)os_zalloc(512);
            if (g_wpa_pac_file == NULL) {
                return ESP_ERR_NO_MEM;
            }
            g_wpa_pac_file_len = 0;
        } else { // The file contains pac data
            g_wpa_pac_file = (u8 *)os_zalloc(pac_file_len);
            if (g_wpa_pac_file == NULL) {
                return ESP_ERR_NO_MEM;
            }
            os_memcpy(g_wpa_pac_file, pac_file, pac_file_len);
            g_wpa_pac_file_len = pac_file_len;
        }
    } else {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_wifi_sta_wpa2_ent_set_fast_phase1_params(esp_eap_fast_config config)
{
    char config_for_supplicant[PHASE1_PARAM_STRING_LEN] = "";
    if ((config.fast_provisioning > -1) && (config.fast_provisioning <= 2)) {
        os_sprintf((char *) &config_for_supplicant, "fast_provisioning=%d ", config.fast_provisioning);
    } else {
        return ESP_ERR_INVALID_ARG;
    }
    if (config.fast_max_pac_list_len && config.fast_max_pac_list_len < 100) {
        os_sprintf((char *) &config_for_supplicant + strlen(config_for_supplicant), "fast_max_pac_list_len=%d ", config.fast_max_pac_list_len);
    } else if (config.fast_max_pac_list_len >= 100) {
        return ESP_ERR_INVALID_ARG;
    }
    if (config.fast_pac_format_binary) {
        os_strcat((char *) &config_for_supplicant, (const char *) "fast_pac_format=binary");
    }

    // Free the old buffer if it already exists
    if (g_wpa_phase1_options != NULL) {
        os_free(g_wpa_phase1_options);
    }
    g_wpa_phase1_options = (char *)os_zalloc(sizeof(config_for_supplicant));
    if (g_wpa_phase1_options == NULL) {
        return ESP_ERR_NO_MEM;
    }
    os_memcpy(g_wpa_phase1_options, &config_for_supplicant, sizeof(config_for_supplicant));
    return ESP_OK;

}
