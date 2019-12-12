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

#include "utils/includes.h"
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

#define API_MUTEX_TAKE() do {\
    if (!s_wps_api_lock) {\
        s_wps_api_lock = xSemaphoreCreateRecursiveMutex();\
        if (!s_wps_api_lock) {\
            wpa_printf(MSG_ERROR, "wps api lock create failed");\
            return ESP_ERR_NO_MEM;\
        }\
    }\
    xSemaphoreTakeRecursive(s_wps_api_lock, portMAX_DELAY);\
} while(0)

#define API_MUTEX_GIVE() xSemaphoreGiveRecursive(s_wps_api_lock)
#define DATA_MUTEX_TAKE() xSemaphoreTakeRecursive(s_wps_data_lock, portMAX_DELAY)
#define DATA_MUTEX_GIVE() xSemaphoreGiveRecursive(s_wps_data_lock)

#define WPS_ADDR_LEN 6
#ifdef USE_WPS_TASK
struct wps_rx_param {
    u8 sa[WPS_ADDR_LEN];
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
static void *s_wps_api_lock = NULL;  /* Used in WPS public API only, never be freed */
static void *s_wps_api_sem = NULL;   /* Sync semaphore used between WPS publi API caller task and WPS task */
static void *s_wps_data_lock = NULL;
static void *s_wps_task_create_sem = NULL;
static bool s_wps_enabled = false;
static uint8_t s_wps_sig_cnt[SIG_WPS_NUM] = {0};

#endif

void wifi_wps_scan_done(void *arg, STATUS status);
void wifi_wps_scan(void);
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

struct wps_sm *gWpsSm = NULL;
static wps_factory_information_t *s_factory_info = NULL;

#ifdef CONFIG_WPS_TESTING
int wps_version_number = 0x20;
int wps_testing_dummy_cred = 0;
#endif /* CONFIG_WPS_TESTING */

int wps_get_type(void)
{
    return esp_wifi_get_wps_type_internal();
}

int wps_set_type(uint32_t type)
{
    return esp_wifi_set_wps_type_internal(type);
}

int wps_get_status(void)
{
    return esp_wifi_get_wps_status_internal();
}

int wps_set_status(uint32_t status)
{
    return esp_wifi_set_wps_status_internal(status);
}

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

    xSemaphoreGive(s_wps_task_create_sem);

    wpa_printf(MSG_DEBUG, "wps_Task enter");
    for (;;) {
        if ( pdPASS == xQueueReceive(s_wps_queue, &e, portMAX_DELAY) ) {

            if ( (e->sig >= SIG_WPS_ENABLE) && (e->sig < SIG_WPS_NUM) ) {
                DATA_MUTEX_TAKE();
                if (s_wps_sig_cnt[e->sig]) {
                    s_wps_sig_cnt[e->sig]--;
                } else {
                    wpa_printf(MSG_ERROR, "wpsT: invalid sig cnt, sig=%d cnt=%d", e->sig, s_wps_sig_cnt[e->sig]);
                }
                DATA_MUTEX_GIVE();
            }

            wpa_printf(MSG_DEBUG, "wpsT: rx sig=%d", e->sig);

            switch (e->sig) {
            case SIG_WPS_ENABLE:
            case SIG_WPS_DISABLE:
            case SIG_WPS_START:
                param = (wps_ioctl_param_t *)e->par;
                if (!param) {
                    wpa_printf(MSG_ERROR, "wpsT: invalid param sig=%d", e->sig);
                    xSemaphoreGive(s_wps_api_sem);
                    break;
                }

                if (e->sig == SIG_WPS_ENABLE) {
                    param->ret = wifi_wps_enable_internal((esp_wps_config_t *)(param->arg));
                } else if (e->sig == SIG_WPS_DISABLE) {
                    param->ret = wifi_wps_disable_internal();
                    del_task = true;
                    s_wps_task_hdl = NULL;
                } else {
                    param->ret = wifi_station_wps_start();
                }

                xSemaphoreGive(s_wps_api_sem);
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
                wpa_printf(MSG_ERROR, "wpsT: invalid sig=%d", e->sig);
                break;
            }
            os_free(e);

            if (del_task) {
                wpa_printf(MSG_DEBUG, "wpsT: delete task");
                break;
            }
        }
    }
    vTaskDelete(NULL);
}

/* wps_post() is thread-safe
 *
 */
int wps_post(uint32_t sig, uint32_t par)
{
    wpa_printf(MSG_DEBUG, "wps post: sig=%d cnt=%d", sig, s_wps_sig_cnt[sig]);

    DATA_MUTEX_TAKE();
    if (s_wps_sig_cnt[sig]) {
        wpa_printf(MSG_DEBUG, "wps post: sig=%d processing", sig);
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

        if ( xQueueSend(s_wps_queue, &evt, 10 / portTICK_PERIOD_MS) != pdPASS) {
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

static void wps_sendto_wrapper(void *buffer, uint16_t len)
{
    esp_wifi_internal_tx(WIFI_IF_STA, buffer, len);
}

/*
* wps_sm_ether_send - Send Ethernet frame
* @wpa_s: Pointer to wpa_supplicant data
* @dest: Destination MAC address
* @proto: Ethertype in host byte order
* @buf: Frame payload starting from IEEE 802.1X header
* @len: Frame payload length
* Returns: >=0 on success, <0 on failure
*/
static inline int wps_sm_ether_send(struct wps_sm *sm, const u8 *dest, u16 proto,
                                    const u8 *data, size_t data_len)
{
    void *buffer = (void *)(data - sizeof(struct l2_ethhdr));
    struct l2_ethhdr *eth = (struct l2_ethhdr *)buffer;

    memcpy(eth->h_dest, dest, ETH_ALEN);
    memcpy(eth->h_source, sm->ownaddr, ETH_ALEN);
    eth->h_proto = host_to_be16(proto);

    wps_sendto_wrapper(buffer, sizeof(struct l2_ethhdr) + data_len);

    return ESP_OK;
}


u8 *wps_sm_alloc_eapol(struct wps_sm *sm, u8 type,
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

    hdr->version = sm->eapol_version;
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


void wps_sm_free_eapol(u8 *buffer)
{
    if (buffer != NULL) {
        buffer = buffer - sizeof(struct l2_ethhdr);
        os_free(buffer);
    }
}


/**
 * wps_init - Initialize WPS Registration protocol data
 * @cfg: WPS configuration
 * Returns: Pointer to allocated data or %NULL on failure
 *
 * This function is used to initialize WPS data for a registration protocol
 * instance (i.e., each run of registration protocol as a Registrar of
 * Enrollee. The caller is responsible for freeing this data after the
 * registration run has been completed by calling wps_deinit().
 */
struct wps_data *wps_init(void)
{
    struct wps_sm *sm = gWpsSm;
    struct wps_data *data = (struct wps_data *)os_zalloc(sizeof(*data));
    const char *all_zero_pin = "00000000";

    if (data == NULL) {
        return NULL;
    }

    data->wps = sm->wps_ctx;

    if (IS_WPS_REGISTRAR(wps_get_type())) {
        data->registrar = 1;
    } else {
        data->registrar = 0;
    }

    data->registrar = 0; /* currently, we force to support enrollee only */

    if (data->registrar) {
        memcpy(data->uuid_r, sm->uuid, WPS_UUID_LEN);
    } else {
        memcpy(data->mac_addr_e, sm->dev->mac_addr, ETH_ALEN);
        memcpy(data->uuid_e, sm->uuid, WPS_UUID_LEN);
    }

    if (wps_get_type() == WPS_TYPE_PIN) {
        u32 spin = 0;
        data->dev_pw_id = DEV_PW_DEFAULT;
        data->dev_password_len = 8;
        data->dev_password = (u8 *) os_zalloc(data->dev_password_len + 1);
        if (data->dev_password == NULL) {
            os_free(data);
            return NULL;
        }

        spin = wps_generate_pin();
        sprintf((char *)data->dev_password, "%08d", spin);
        wpa_hexdump_key(MSG_DEBUG, "WPS: AP PIN dev_password",
                        data->dev_password, data->dev_password_len);
        do {
            char tmpp[9];
            os_bzero(tmpp, 9);
            memcpy(tmpp, data->dev_password, 8);
            wpa_printf(MSG_DEBUG, "WPS PIN [%s]", tmpp);
            wifi_event_sta_wps_er_pin_t evt;
            memcpy(evt.pin_code, data->dev_password, 8);
            esp_event_send_internal(WIFI_EVENT, WIFI_EVENT_STA_WPS_ER_PIN, &evt, sizeof(evt), portMAX_DELAY);
        } while (0);
    } else if (wps_get_type() == WPS_TYPE_PBC) {
        data->pbc = 1;
        /* Use special PIN '00000000' for PBC */
        data->dev_pw_id = DEV_PW_PUSHBUTTON;
        if (data->dev_password) {
            os_free(data->dev_password);
        }
        data->dev_password = (u8 *) os_zalloc(9);
        if (data->dev_password == NULL) {
            os_free(data);
            return NULL;
        } else {
            strncpy((char *)data->dev_password, all_zero_pin, 9);
        }
        data->dev_password_len = 8;
    }

#ifdef CONFIG_WPS_NFC
    if (cfg->wps->ap && !cfg->registrar && cfg->wps->ap_nfc_dev_pw_id) {
        data->dev_pw_id = cfg->wps->ap_nfc_dev_pw_id;
        os_free(data->dev_password);
        data->dev_password =
            os_malloc(wpabuf_len(cfg->wps->ap_nfc_dev_pw));
        if (data->dev_password == NULL) {
            os_free(data);
            return NULL;
        }
        memcpy(data->dev_password,
               wpabuf_head(cfg->wps->ap_nfc_dev_pw),
               wpabuf_len(cfg->wps->ap_nfc_dev_pw));
        data->dev_password_len = wpabuf_len(cfg->wps->ap_nfc_dev_pw);
    }
#endif /* CONFIG_WPS_NFC */
    data->wps->config_methods = WPS_CONFIG_PUSHBUTTON | WPS_CONFIG_DISPLAY;
#ifdef CONFIG_WPS2
    data->wps->config_methods |= (WPS_CONFIG_VIRT_PUSHBUTTON | WPS_CONFIG_PHY_DISPLAY);
#endif

    data->state = data->registrar ? RECV_M1 : SEND_M1;

    return data;
}


/**
 * wps_deinit - Deinitialize WPS Registration protocol data
 * @data: WPS Registration protocol data from wps_init()
 */
void wps_deinit(void)
{
    struct wps_data *data = gWpsSm->wps;

#ifdef CONFIG_WPS_NFC
    if (data->registrar && data->nfc_pw_token)
        wps_registrar_remove_nfc_pw_token(data->wps->registrar,
                                          data->nfc_pw_token);
#endif /* CONFIG_WPS_NFC */

    if (data->wps_pin_revealed) {
        wpa_printf(MSG_DEBUG,  "WPS: Full PIN information revealed and "
                   "negotiation failed");
    } else if (data->registrar)
        wpa_printf(MSG_DEBUG,  "WPS: register information revealed and "
                   "negotiation failed");
    wpabuf_free(data->dh_privkey);

#ifdef DESP32_WORKAROUND
    /*
     * due to the public key calculated when wps start, it will not calculate anymore even when we build M1 message, also calculate the key need take a long time
     * which would cause WPS fail, so we clean the key after WPS finished .
     */
    data->dh_privkey = NULL;
#endif //DESP32_WORKAROUND

    wpabuf_free(data->dh_pubkey_e);
    wpabuf_free(data->dh_pubkey_r);
    wpabuf_free(data->last_msg);
    os_free(data->dev_password);
    dh5_free(data->dh_ctx);
    wps_dev_deinit(&data->peer_dev);
#ifdef CONFIG_WPS_NFC
    os_free(data->nfc_pw_token);
#endif
    os_free(data);
}

static void
wps_build_ic_appie_wps_pr(void)
{
    struct wpabuf *extra_ie = NULL;
    struct wpabuf *wps_ie;
    struct wps_sm *sm = gWpsSm;

    wpa_printf(MSG_DEBUG, "wps build: wps pr");

    if (wps_get_type() == WPS_TYPE_PBC) {
        wps_ie = (struct wpabuf *)wps_build_probe_req_ie(DEV_PW_PUSHBUTTON,
                 sm->dev,
                 sm->uuid, WPS_REQ_ENROLLEE,
                 0, NULL);
    } else {
        wps_ie = (struct wpabuf *)wps_build_probe_req_ie(DEV_PW_DEFAULT,
                 sm->dev,
                 sm->uuid, WPS_REQ_ENROLLEE,
                 0, NULL);
    }

    if (wps_ie) {
        if (wpabuf_resize(&extra_ie, wpabuf_len(wps_ie)) == 0) {
            wpabuf_put_buf(extra_ie, wps_ie);
        } else {
            wpabuf_free(wps_ie);
            return;
        }
        wpabuf_free(wps_ie);
    }

    esp_wifi_set_appie_internal(WIFI_APPIE_WPS_PR, (uint8_t *)wpabuf_head(extra_ie), extra_ie->used, 0);
    wpabuf_free(extra_ie);
}

static void
wps_build_ic_appie_wps_ar(void)
{
    struct wpabuf *buf = (struct wpabuf *)wps_build_assoc_req_ie(WPS_REQ_ENROLLEE);

    wpa_printf(MSG_DEBUG, "wps build: wps ar");

    if (buf) {
        esp_wifi_set_appie_internal(WIFI_APPIE_WPS_AR, (uint8_t *)wpabuf_head(buf), buf->used, 0);
        wpabuf_free(buf);
    }
}

static bool
wps_parse_scan_result(struct wps_scan_ie *scan)
{
    struct wps_sm *sm = gWpsSm;
    wifi_mode_t op_mode = 0;
#ifdef WPS_DEBUG
    char tmp[32];

    os_bzero(tmp, 32);
    strncpy(tmp, (char *)&scan->ssid[2], (int)scan->ssid[1]);
    wpa_printf(MSG_DEBUG, "wps parse scan: %s", tmp);
#endif

    if (wps_get_type() == WPS_TYPE_DISABLE
            || (wps_get_status() != WPS_STATUS_DISABLE
                && wps_get_status() != WPS_STATUS_SCANNING)
       ) {
        return false;
    }

    esp_wifi_get_mode(&op_mode);
    if ((op_mode == WIFI_MODE_STA || op_mode == WIFI_MODE_APSTA) && scan->wps) {
        struct wpabuf *buf = wpabuf_alloc_copy(scan->wps + 6, scan->wps[1] - 4);

        if (wps_is_selected_pbc_registrar(buf, scan->bssid)
                || wps_is_selected_pin_registrar(buf, scan->bssid)) {
            wpabuf_free(buf);

            if (sm->is_wps_scan == false) {
                return false;
            }
            if (memcmp(sm->config.bssid, scan->bssid, ETH_ALEN) != 0 ) {
                sm->discover_ssid_cnt++;
            }

            if (!scan->rsn && !scan->wpa && (scan->capinfo & WIFI_CAPINFO_PRIVACY)) {
                wpa_printf(MSG_ERROR, "WEP not suppported in WPS");

                return false;
            }

            esp_wifi_enable_sta_privacy_internal();
            os_bzero(sm->ssid, sizeof(sm->ssid));
            strncpy((char *)sm->ssid, (char *)&scan->ssid[2], (int)scan->ssid[1]);
            sm->ssid_len = scan->ssid[1];
            if (scan->bssid) {
                memcpy(gWpsSm->bssid, scan->bssid, ETH_ALEN);
                memcpy(sm->config.bssid, scan->bssid, ETH_ALEN);
                sm->config.bssid_set = 1;
            } else {
            }
            wpa_printf(MSG_DEBUG, "wps discover [%s]", sm->ssid);
            sm->scan_cnt = 0;

            memcpy(sm->config.ssid, sm->ssid, sm->ssid_len);
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
    u8 bssid[6];
    u8 *buf = NULL;
    int len;
    int ret = ESP_OK;

    wpa_printf(MSG_DEBUG, "wps send eapol id rsp");
    eap_buf = eap_msg_alloc(EAP_VENDOR_IETF, EAP_TYPE_IDENTITY, sm->identity_len,
                            EAP_CODE_RESPONSE, id);
    if (!eap_buf) {
        ret = ESP_FAIL;
        goto _err;
    }

    ret = esp_wifi_get_assoc_bssid_internal(bssid);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "bssid is empty!");
        return ESP_FAIL;
    }

    wpabuf_put_data(eap_buf, sm->identity, sm->identity_len);

    buf = wps_sm_alloc_eapol(sm, IEEE802_1X_TYPE_EAP_PACKET, wpabuf_head_u8(eap_buf), wpabuf_len(eap_buf), (size_t *)&len, NULL);
    if (!buf) {
        ret = ESP_ERR_NO_MEM;
        goto _err;
    }

    ret = wps_sm_ether_send(sm, bssid, ETH_P_EAPOL, buf, len);
    if (ret) {
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
    u8 bssid[6];
    u8 *buf;
    int len;
    int ret = 0;
    enum wsc_op_code opcode = WSC_FRAG_ACK;

    wpa_printf(MSG_DEBUG, "send frag ack id:%d", id);

    if (!sm) {
        return ESP_FAIL;
    }

    ret = esp_wifi_get_assoc_bssid_internal(bssid);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "bssid is empty!");
        return ret;
    }

    eap_buf = eap_msg_alloc(EAP_VENDOR_WFA, 0x00000001, 2, EAP_CODE_RESPONSE, id);
    if (!eap_buf) {
        ret = ESP_ERR_NO_MEM;
        goto _err;
    }

    wpabuf_put_u8(eap_buf, opcode);
    wpabuf_put_u8(eap_buf, 0x00); /* flags */

    buf = wps_sm_alloc_eapol(sm, IEEE802_1X_TYPE_EAP_PACKET, wpabuf_head_u8(eap_buf), wpabuf_len(eap_buf), (size_t *)&len, NULL);
    if (!buf) {
        ret = ESP_ERR_NO_MEM;
        goto _err;
    }

    ret = wps_sm_ether_send(sm, bssid, ETH_P_EAPOL, buf, len);
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

    if (!sm) {
        return ESP_FAIL;
    }

    expd = (struct eap_expand *) ubuf;
    wpa_printf(MSG_DEBUG, "wps process mX req: len %d, tlen %d", len, tlen);

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
        wpa_printf(MSG_DEBUG, "rx frag msg id:%d, flag:%d, frag_len: %d, tot_len: %d, be_tot_len:%d", sm->current_identifier, flag, frag_len, tlen, be_tot_len);
        if (ESP_OK != wps_enrollee_process_msg_frag(&wps_buf, tlen, tbuf, frag_len, flag)) {
            if (wps_buf) {
                wpabuf_free(wps_buf);
                wps_buf = NULL;
            }
            return ESP_FAIL;
        }
        if (flag & WPS_MSG_FLAG_MORE) {
            if (res) {
                *res = WPS_FRAGMENT;
            }
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

    ets_timer_disarm(&sm->wps_msg_timeout_timer);

    if (res) {
        *res = wps_enrollee_process_msg(sm->wps, expd->opcode, wps_buf);
    } else {
        wps_enrollee_process_msg(sm->wps, expd->opcode, wps_buf);
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
    u8 bssid[6];
    u8 *buf;
    int len;
    int ret = 0;
    enum wsc_op_code opcode;

    wpa_printf(MSG_DEBUG, "wps send wps mX rsp");

    if (!sm) {
        return ESP_FAIL;
    }

    ret = esp_wifi_get_assoc_bssid_internal(bssid);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "bssid is empty!");
        return ret;
    }

    wps_buf = (struct wpabuf *)wps_enrollee_get_msg(sm->wps, &opcode);
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

    ret = wps_sm_ether_send(sm, bssid, ETH_P_EAPOL, buf, len);
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
    u8 bssid[6];
    u8 *buf;
    int len;
    int ret = 0;

    ret = esp_wifi_get_assoc_bssid_internal(bssid);
    if (ret != 0) {
        wpa_printf(MSG_ERROR, "bssid is empty!");
        return ret;
    }

    if (!sm) {
        return ESP_FAIL;
    }

    wpa_printf(MSG_DEBUG,  "WPS: Send EAPOL START.");
    buf = wps_sm_alloc_eapol(sm, IEEE802_1X_TYPE_EAPOL_START, (u8 *)"", 0, (size_t *)&len, NULL);
    if (!buf) {
        return ESP_ERR_NO_MEM;
    }

    wps_sm_ether_send(sm, bssid, ETH_P_EAPOL, buf, len);
    wps_sm_free_eapol(buf);

    ets_timer_arm(&sm->wps_eapol_start_timer, 3000, 0);

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

int wps_stop_process(wifi_event_sta_wps_fail_reason_t reason_code)
{
    struct wps_sm *sm = gWpsSm;

    if (!gWpsSm) {
        return ESP_FAIL;
    }

    wps_set_status(WPS_STATUS_DISABLE);
    sm->scan_cnt = 0;
    sm->discover_ssid_cnt = 0;
    sm->wps->state = SEND_M1;
    os_bzero(sm->bssid, ETH_ALEN);
    os_bzero(sm->ssid, 32);
    sm->ssid_len = 0;
    os_bzero((u8 *)&sm->config, sizeof(wifi_sta_config_t));

    esp_wifi_disarm_sta_connection_timer_internal();
    ets_timer_disarm(&sm->wps_msg_timeout_timer);
    ets_timer_disarm(&sm->wps_success_cb_timer);

    esp_wifi_disconnect();

    wpa_printf(MSG_DEBUG, "Write wps_fail_information");
    
    esp_event_send_internal(WIFI_EVENT, WIFI_EVENT_STA_WPS_ER_FAILED, &reason_code, sizeof(reason_code), portMAX_DELAY);

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
        wifi_config_t *config = (wifi_config_t *)os_zalloc(sizeof(wifi_config_t));

        if (config == NULL) {
            esp_event_send_internal(WIFI_EVENT, WIFI_EVENT_STA_WPS_ER_FAILED, 0, 0, portMAX_DELAY);
            return ESP_FAIL;
        }

        wpa_printf(MSG_DEBUG, "wps finished------>");
        wps_set_status(WPS_STATUS_SUCCESS);
        esp_wifi_disarm_sta_connection_timer_internal();
        ets_timer_disarm(&sm->wps_timeout_timer);
        ets_timer_disarm(&sm->wps_msg_timeout_timer);

        memset(config, 0x00, sizeof(wifi_sta_config_t));
        memcpy(config->sta.ssid, sm->ssid, sm->ssid_len);
        memcpy(config->sta.password, sm->key, sm->key_len);
        memcpy(config->sta.bssid, sm->bssid, ETH_ALEN);
        config->sta.bssid_set = 0;
        esp_wifi_set_config(0, config);
        os_free(config);
        config = NULL;

        ets_timer_disarm(&sm->wps_success_cb_timer);
        ets_timer_arm(&sm->wps_success_cb_timer, 1000, 0);

        ret = 0;
    } else {
        wpa_printf(MSG_ERROR, "wps failed----->");

        ret = wps_stop_process(WPS_FAIL_REASON_NORMAL);
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
            memcpy(sm->dis_ap_list[cnt].bssid, sm->dis_ap_list[cnt + 1].bssid, 6);
        }
        sm->discard_ap_cnt = WPS_MAX_DIS_AP_NUM;
    }
    memcpy(sm->dis_ap_list[cnt].bssid, bssid, 6);
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
        wpa_printf(MSG_DEBUG, "start msg timer WPS_FINISHED %d ms", msg_timeout);
        ets_timer_disarm(&sm->wps_msg_timeout_timer);
        ets_timer_arm(&sm->wps_msg_timeout_timer, msg_timeout, 0);
        ret = 0;
    } else if (sm->wps->state == RECV_M2) {
        msg_timeout = 5000;
        wpa_printf(MSG_DEBUG, "start msg timer RECV_M2 %d ms", msg_timeout);
        ets_timer_disarm(&sm->wps_msg_timeout_timer);
        ets_timer_arm(&sm->wps_msg_timeout_timer, msg_timeout, 0);
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
        struct wps_rx_param *param = (struct wps_rx_param *)os_zalloc(sizeof(struct wps_rx_param));   /* free in task */

        if (!param) {
            return ESP_ERR_NO_MEM;
        }

        param->buf = (u8 *)os_zalloc(len);   /* free in task */
        if (!param->buf) {
            os_free(param);
            return ESP_ERR_NO_MEM;
        }
        memcpy(param->buf, buf, len);
        param->len = len;
        memcpy(param->sa, src_addr, WPS_ADDR_LEN);

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
#ifdef DEBUG_PRINT
        wpa_printf(MSG_DEBUG,  "WPA: EAPOL frame too short to be a WPA "
                   "EAPOL-Key (len %lu, expecting at least %lu)",
                   (unsigned long) len,
                   (unsigned long) sizeof(*hdr) + sizeof(*ehdr));
#endif
        return ESP_OK;
    }

    tmp = buf;

    hdr = (struct ieee802_1x_hdr *) tmp;
    ehdr = (struct eap_hdr *) (hdr + 1);
    plen = be_to_host16(hdr->length);
    data_len = plen + sizeof(*hdr);
    eap_len = be_to_host16(ehdr->length);

#ifdef DEBUG_PRINT
    wpa_printf(MSG_DEBUG, "IEEE 802.1X RX: version=%d type=%d length=%d",
               hdr->version, hdr->type, plen);
#endif

    if (hdr->version < EAPOL_VERSION) {
        /* TODO: backwards compatibility */
    }
    if (hdr->type != IEEE802_1X_TYPE_EAP_PACKET) {
#ifdef DEBUG_PRINT
        wpa_printf(MSG_DEBUG, "WPS: EAP frame (type %u) discarded, "
                   "not a EAP PACKET frame", hdr->type);
#endif
        ret = 0;
        goto out;
    }
    if (plen > len - sizeof(*hdr) || plen < sizeof(*ehdr)) {
#ifdef DEBUG_PRINT
        wpa_printf(MSG_DEBUG, "WPA: EAPOL frame payload size %lu "
                   "invalid (frame size %lu)",
                   (unsigned long) plen, (unsigned long) len);
#endif
        ret = 0;
        goto out;
    }

    wpa_hexdump(MSG_MSGDUMP, "WPA: RX EAPOL-EAP PACKET", tmp, len);

    if (data_len < len) {
#ifdef DEBUG_PRINT
        wpa_printf(MSG_DEBUG, "WPA: ignoring %lu bytes after the IEEE "
                   "802.1X data", (unsigned long) len - data_len);
#endif
    }

    if (eap_len != plen) {
#ifdef DEBUG_PRINT
        wpa_printf(MSG_DEBUG, "WPA: EAPOL length %lu "
                   "invalid (eapol length %lu)",
                   (unsigned long) eap_len, (unsigned long) plen);
#endif
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
            ets_timer_disarm(&sm->wps_eapol_start_timer);
            wpa_printf(MSG_DEBUG,  "WPS: Build EAP Identity.");
            ret = wps_send_eap_identity_rsp(ehdr->identifier);
            ets_timer_arm(&sm->wps_eapol_start_timer, 3000, 0);
            break;
        case EAP_TYPE_EXPANDED:
            wpa_printf(MSG_DEBUG, "=========expanded plen[%d], %d===========", plen, sizeof(*ehdr));
            if (ehdr->identifier == sm->current_identifier) {
                ret = 0;
                wpa_printf(MSG_DEBUG, "wps: ignore overlap identifier");
                goto out;
            }
            sm->current_identifier = ehdr->identifier;

            tmp = (u8 *)(ehdr + 1) + 1;
            ret = wps_process_wps_mX_req(tmp, plen - sizeof(*ehdr) - 1, &res);
            if (ret == 0 && res != WPS_FAILURE && res != WPS_IGNORE && res != WPS_FRAGMENT) {
                ret = wps_send_wps_mX_rsp(ehdr->identifier);
                if (ret == 0) {
                    wpa_printf(MSG_DEBUG, "sm->wps->state = %d", sm->wps->state);
                    wps_start_msg_timer();
                }
            } else if (ret == 0 && res == WPS_FRAGMENT) {
                wpa_printf(MSG_DEBUG, "wps frag, continue...");
                ret = ESP_OK;
            } else if (res == WPS_IGNORE) {
                wpa_printf(MSG_DEBUG, "IGNORE overlap Mx");
                ret = ESP_OK; /* IGNORE the overlap */
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
    if (ret != 0 || res == WPS_FAILURE) {
        wpa_printf(MSG_DEBUG, "wpa rx eapol internal: fail ret=%d", ret);
        wps_set_status(WPS_STATUS_DISABLE);
        esp_wifi_disarm_sta_connection_timer_internal();
        ets_timer_disarm(&sm->wps_timeout_timer);

        esp_event_send_internal(WIFI_EVENT, WIFI_EVENT_STA_WPS_ER_FAILED, 0, 0, portMAX_DELAY);

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

    sprintf(s_factory_info->manufacturer, "ESPRESSIF");
    sprintf(s_factory_info->model_name, "ESPRESSIF IOT");
    sprintf(s_factory_info->model_number, "ESP32");
    sprintf(s_factory_info->device_name, "ESP32 STATION");

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
        memcpy(s_factory_info->manufacturer, config->factory_info.manufacturer, WPS_MAX_MANUFACTURER_LEN - 1);
    }

    if (config->factory_info.model_number[0] != 0) {
        memcpy(s_factory_info->model_number, config->factory_info.model_number, WPS_MAX_MODEL_NUMBER_LEN - 1);
    }

    if (config->factory_info.model_name[0] != 0) {
        memcpy(s_factory_info->model_name, config->factory_info.model_name, WPS_MAX_MODEL_NAME_LEN - 1);
    }

    if (config->factory_info.device_name[0] != 0) {
        memcpy(s_factory_info->device_name, config->factory_info.device_name, WPS_MAX_DEVICE_NAME_LEN - 1);
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
    dev->config_methods = WPS_CONFIG_VIRT_PUSHBUTTON | WPS_CONFIG_PHY_DISPLAY;
    dev->rf_bands = WPS_RF_24GHZ;

    WPA_PUT_BE16(dev->pri_dev_type, WPS_DEV_COMPUTER);
    WPA_PUT_BE32(dev->pri_dev_type + 2, WPS_DEV_OUI_WFA);
    WPA_PUT_BE16(dev->pri_dev_type + 6, WPS_DEV_COMPUTER_PC);

    if (!s_factory_info) {
        ret = wps_set_default_factory();
        if (ret != 0) {
            goto _out;
        }
    }

    dev->manufacturer = (char *)os_zalloc(WPS_MAX_MANUFACTURER_LEN);
    if (!dev->manufacturer) {
        ret = ESP_FAIL;
        goto _out;
    }
    sprintf(dev->manufacturer, s_factory_info->manufacturer);

    dev->model_name = (char *)os_zalloc(WPS_MAX_MODEL_NAME_LEN);
    if (!dev->model_name) {
        ret = ESP_FAIL;
        goto _out;
    }
    sprintf(dev->model_name, s_factory_info->model_name);

    dev->model_number = (char *)os_zalloc(WPS_MAX_MODEL_NAME_LEN);
    if (!dev->model_number) {
        ret = ESP_FAIL;
        goto _out;
    }
    sprintf(dev->model_number, s_factory_info->model_number);

    dev->device_name = (char *)os_zalloc(WPS_MAX_DEVICE_NAME_LEN);
    if (!dev->device_name) {
        ret = ESP_FAIL;
        goto _out;
    }
    sprintf(dev->device_name, s_factory_info->device_name);

    dev->serial_number = (char *)os_zalloc(16);
    if (!dev->serial_number) {
        ret = ESP_FAIL;
        goto _out;
    }
    sprintf(dev->serial_number, "%02x%02x%02x%02x%02x%02x",
            sm->ownaddr[0], sm->ownaddr[1], sm->ownaddr[2],
            sm->ownaddr[3], sm->ownaddr[4], sm->ownaddr[5]);

    uuid_gen_mac_addr(sm->ownaddr, sm->uuid);
    memcpy(dev->mac_addr, sm->ownaddr, ETH_ALEN);

    return ESP_OK;

_out:
    if (dev->manufacturer) {
        os_free(dev->manufacturer);
    }
    if (dev->model_name) {
        os_free(dev->model_name);
    }
    if (dev->model_number) {
        os_free(dev->model_number);
    }
    if (dev->device_name) {
        os_free(dev->device_name);
    }
    if (dev->serial_number) {
        os_free(dev->serial_number);
    }

    if (s_factory_info) {
        os_free(s_factory_info);
        s_factory_info = NULL;
    }

    return ret;
}


int wps_dev_deinit(struct wps_device_data *dev)
{
    int ret = 0;

    if (!dev) {
        return ESP_FAIL;
    }

    if (dev->manufacturer) {
        os_free(dev->manufacturer);
    }
    if (dev->model_name) {
        os_free(dev->model_name);
    }
    if (dev->model_number) {
        os_free(dev->model_number);
    }
    if (dev->device_name) {
        os_free(dev->device_name);
    }
    if (dev->serial_number) {
        os_free(dev->serial_number);
    }

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

    esp_event_send_internal(WIFI_EVENT, WIFI_EVENT_STA_WPS_ER_TIMEOUT, 0, 0, portMAX_DELAY);
}

void wifi_station_wps_timeout(void)
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
    } else if (sm->wps->state == RECV_M2) {
        wpa_printf(MSG_DEBUG, "wps msg timeout RECV_M2");
        wpa_printf(MSG_DEBUG, "wps recev m2/m2d timeout------>");
        wps_add_discard_ap(sm->config.bssid);
        wps_stop_process(WPS_FAIL_REASON_RECV_M2D);
    }
}

void wifi_station_wps_msg_timeout(void)
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
    esp_event_send_internal(WIFI_EVENT, WIFI_EVENT_STA_WPS_ER_SUCCESS, 0, 0, portMAX_DELAY);
}

void wifi_station_wps_success(void)
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

void wifi_station_wps_eapol_start_handle(void)
{
#ifdef USE_WPS_TASK
    wps_post(SIG_WPS_TIMER_EAPOL_START, 0);
    return;
#else
    wifi_station_wps_eapol_start_handle_internal();
#endif
}

int
wifi_station_wps_init(void)
{
    struct wps_funcs *wps_cb;
    struct wps_sm *sm = NULL;
    uint8_t mac[6];

    if (gWpsSm) {
        goto _out;
    }

    wpa_printf(MSG_DEBUG, "wifi sta wps init");

    gWpsSm = (struct wps_sm *)os_zalloc(sizeof(struct wps_sm));   /* alloc Wps_sm */
    if (!gWpsSm) {
        goto _err;
    }

    sm = gWpsSm;
    memset(sm, 0x00, sizeof(struct wps_sm));

    esp_wifi_get_macaddr_internal(WIFI_IF_STA, mac);
    memcpy(sm->ownaddr, mac, ETH_ALEN);

    sm->discover_ssid_cnt = 0;
    sm->ignore_sel_reg = false;
    sm->discard_ap_cnt = 0;
    memset(&sm->dis_ap_list, 0, WPS_MAX_DIS_AP_NUM * sizeof(struct discard_ap_list_t));
    memset(&sm->config, 0x00, sizeof(wifi_sta_config_t));
    sm->eapol_version = 0x1;
    sm->identity_len = 29;
    memcpy(sm->identity, WPS_EAP_EXT_VENDOR_TYPE, sm->identity_len);

    sm->is_wps_scan = false;

    sm->wps_ctx = (struct wps_context *)os_zalloc(sizeof(struct wps_context)); /* alloc wps_ctx */
    if (!sm->wps_ctx) {
        goto _err;
    }

    if (wps_dev_init() != 0) {
        goto _err;
    }

    if ((sm->wps = wps_init()) == NULL) {         /* alloc wps_data */
        goto _err;
    }

    /**************80211 reference***************/

    if (esp_wifi_get_appie_internal(WIFI_APPIE_WPS_PR) == NULL) {            /* alloc probe req wps ie */
        wps_build_ic_appie_wps_pr();
    }

    if (esp_wifi_get_appie_internal(WIFI_APPIE_WPS_AR) == NULL) {           /* alloc assoc req wps ie */
        wps_build_ic_appie_wps_ar();
    }

    ets_timer_disarm(&sm->wps_timeout_timer);
    ets_timer_setfn(&sm->wps_timeout_timer, (ETSTimerFunc *)wifi_station_wps_timeout, NULL);
    ets_timer_disarm(&sm->wps_msg_timeout_timer);
    ets_timer_setfn(&sm->wps_msg_timeout_timer, (ETSTimerFunc *)wifi_station_wps_msg_timeout, NULL);
    ets_timer_disarm(&sm->wps_success_cb_timer);
    ets_timer_setfn(&sm->wps_success_cb_timer, (ETSTimerFunc *)wifi_station_wps_success, NULL);
    ets_timer_disarm(&sm->wps_scan_timer);
    ets_timer_setfn(&sm->wps_scan_timer, (ETSTimerFunc *)wifi_wps_scan, NULL);
    ets_timer_disarm(&sm->wps_eapol_start_timer);
    ets_timer_setfn(&sm->wps_eapol_start_timer, (ETSTimerFunc *)wifi_station_wps_eapol_start_handle, NULL);

    sm->scan_cnt = 0;

    wps_cb = os_malloc(sizeof(struct wps_funcs));
    if (wps_cb == NULL) {
        goto _err;
    } else {
        wps_cb->wps_parse_scan_result = wps_parse_scan_result;
        wps_cb->wifi_station_wps_start = wifi_station_wps_start;
        wps_cb->wps_sm_rx_eapol = wps_sm_rx_eapol;
        wps_cb->wps_start_pending = wps_start_pending;
        esp_wifi_set_wps_cb_internal(wps_cb);
    }

    return ESP_OK;

_err:
    esp_wifi_unset_appie_internal(WIFI_APPIE_WPS_PR);
    esp_wifi_unset_appie_internal(WIFI_APPIE_WPS_AR);

    if (sm->dev) {
        wps_dev_deinit(sm->dev);
        sm->dev = NULL;
    }
    if (sm->wps_ctx) {
        os_free(sm->wps_ctx);
        sm->wps_ctx = NULL;
    }
    if (sm->wps) {
        wps_deinit();
        sm->wps = NULL;
    }
    if (sm) {
        os_free(gWpsSm);
        gWpsSm = NULL;
    }
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

    ets_timer_disarm(&sm->wps_success_cb_timer);
    ets_timer_disarm(&sm->wps_timeout_timer);
    ets_timer_disarm(&sm->wps_msg_timeout_timer);
    ets_timer_disarm(&sm->wps_scan_timer);
    ets_timer_disarm(&sm->wps_eapol_start_timer);
    ets_timer_done(&sm->wps_success_cb_timer);
    ets_timer_done(&sm->wps_timeout_timer);
    ets_timer_done(&sm->wps_msg_timeout_timer);
    ets_timer_done(&sm->wps_scan_timer);
    ets_timer_done(&sm->wps_eapol_start_timer);
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
        os_free(sm->wps_ctx);
        sm->wps_ctx = NULL;
    }
    if (sm->wps) {
        wps_deinit();
        sm->wps = NULL;
    }
    if (sm) {
        os_free(gWpsSm);
        gWpsSm = NULL;
    }

    return ESP_OK;
}

int
wps_station_wps_register_cb(wps_st_cb_t cb)
{
    if (!gWpsSm) {
        return ESP_FAIL;
    }

    gWpsSm->st_cb = cb;
    return ESP_OK;
}

struct wps_sm *
wps_sm_get(void)
{
    return gWpsSm;
}

int
wps_ssid_save(u8 *ssid, u8 ssid_len)
{
    u8 *tmpssid;

    if (!ssid || !gWpsSm) {
        return ESP_FAIL;
    }

    memset(gWpsSm->ssid, 0x00, sizeof(gWpsSm->ssid));
    memcpy(gWpsSm->ssid, ssid, ssid_len);
    gWpsSm->ssid_len = ssid_len;

    tmpssid = (u8 *)os_zalloc(ssid_len + 1);
    if (tmpssid) {
        memcpy(tmpssid, ssid, ssid_len);
        wpa_printf(MSG_DEBUG, "WPS: ssid[%s]", tmpssid);
        os_free(tmpssid);
    }
    return ESP_OK;
}

int
wps_key_save(char *key, u8 key_len)
{
    u8 *tmpkey;

    if (!key || !gWpsSm) {
        return ESP_FAIL;
    }

    memset(gWpsSm->key, 0x00, sizeof(gWpsSm->key));
    memcpy(gWpsSm->key, key, key_len);
    gWpsSm->key_len = key_len;

    tmpkey = (u8 *)os_zalloc(key_len + 1);
    if (tmpkey) {
        memcpy(tmpkey, key, key_len);
        wpa_printf(MSG_DEBUG, "WPS: key[%s]", tmpkey);
        os_free(tmpkey);
    }
    return ESP_OK;
}

void
wifi_wps_scan_done(void *arg, STATUS status)
{
    struct wps_sm *sm = gWpsSm;
    wifi_config_t wifi_config;

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
        wpa_printf(MSG_INFO, "PBC session overlap!");
        wps_set_status(WPS_STATUS_DISABLE);
        esp_event_send_internal(WIFI_EVENT, WIFI_EVENT_STA_WPS_ER_PBC_OVERLAP, 0, 0, portMAX_DELAY);
    }

    wpa_printf(MSG_DEBUG, "wps scan_done discover_ssid_cnt = %d", sm->discover_ssid_cnt);

    sm->discover_ssid_cnt = 0;

    if (wps_get_status() == WPS_STATUS_PENDING) {
        esp_wifi_disconnect();

        memcpy(&wifi_config.sta, &sm->config, sizeof(wifi_sta_config_t));
        esp_wifi_set_config(0, &wifi_config);

        wpa_printf(MSG_DEBUG, "WPS: neg start");
        esp_wifi_connect();
    } else if (wps_get_status() == WPS_STATUS_SCANNING) {
        if (sm->scan_cnt < WPS_IGNORE_SEL_REG_MAX_CNT) {
            sm->ignore_sel_reg = true;
        }
        ets_timer_arm(&sm->wps_scan_timer, 100, 0);
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

    typedef void (* scan_done_cb_t)(void *arg, STATUS status);
    extern int esp_wifi_promiscuous_scan_start(wifi_scan_config_t *config, scan_done_cb_t cb);
    esp_wifi_promiscuous_scan_start(NULL, wifi_wps_scan_done);
}

void wifi_wps_scan(void)
{
#ifdef USE_WPS_TASK
    wps_post(SIG_WPS_TIMER_SCAN, 0);
    return;
#else
    wifi_wps_scan_internal();
#endif
}

int wifi_station_wps_start(void)
{
    struct wps_sm *sm = wps_sm_get();

    if (!sm) {
        wpa_printf(MSG_ERROR, "WPS: wps not initial");
        return ESP_FAIL;
    }

    ets_timer_arm(&sm->wps_timeout_timer, 120000, 0); /* 120s total */

    switch (wps_get_status()) {
    case WPS_STATUS_DISABLE: {
        sm->is_wps_scan = true;

        wps_build_public_key(sm->wps, NULL, WPS_CALC_KEY_PRE_CALC);

        wifi_wps_scan();


        break;
    }
    case WPS_STATUS_SCANNING:
        sm->scan_cnt = 0;
        ets_timer_disarm(&sm->wps_timeout_timer);
        ets_timer_arm(&sm->wps_timeout_timer, 120000, 0); /* 120s total */
        break;
    default:
        break;
    }
    esp_wifi_set_wps_start_flag_internal(true);
    return ESP_OK;
}

int wps_task_deinit(void)
{
    wpa_printf(MSG_DEBUG, "wps task deinit");

    if (s_wps_api_sem) {
        vSemaphoreDelete(s_wps_api_sem);
        s_wps_api_sem = NULL;
        wpa_printf(MSG_DEBUG, "wps task deinit: free api sem");
    }

    if (s_wps_task_create_sem) {
        vSemaphoreDelete(s_wps_task_create_sem);
        s_wps_task_create_sem = NULL;
        wpa_printf(MSG_DEBUG, "wps task deinit: free task create sem");
    }

    if (s_wps_queue) {
        vQueueDelete(s_wps_queue);
        s_wps_queue = NULL;
        wpa_printf(MSG_DEBUG, "wps task deinit: free queue");
    }

    if (s_wps_task_hdl) {
        vTaskDelete(s_wps_task_hdl);
        s_wps_task_hdl = NULL;
        wpa_printf(MSG_DEBUG, "wps task deinit: free task");
    }

    if (STAILQ_FIRST(&s_wps_rxq) != NULL){
        wps_rxq_deinit();
    }

    if (s_wps_data_lock) {
        vSemaphoreDelete(s_wps_data_lock);
        s_wps_data_lock = NULL;
        wpa_printf(MSG_DEBUG, "wps task deinit: free data lock");
    }

    return ESP_OK;
}

int wps_task_init(void)
{
    int ret = 0;

    /* Call wps_task_deinit() first in case esp_wifi_wps_disable() fails
     */
    wps_task_deinit();

    s_wps_data_lock = xSemaphoreCreateRecursiveMutex();
    if (!s_wps_data_lock) {
        wpa_printf(MSG_ERROR, "wps task init: failed to alloc data lock");
        goto _wps_no_mem;
    }

    s_wps_api_sem = xSemaphoreCreateCounting(1, 0);
    if (!s_wps_api_sem) {
        wpa_printf(MSG_ERROR, "wps task init: failed to create api sem");
        goto _wps_no_mem;
    }

    s_wps_task_create_sem = xSemaphoreCreateCounting(1, 0);
    if (!s_wps_task_create_sem) {
        wpa_printf(MSG_ERROR, "wps task init: failed to create task sem");
        goto _wps_no_mem;
    }

    os_bzero(s_wps_sig_cnt, SIG_WPS_NUM);
    s_wps_queue = xQueueCreate(SIG_WPS_NUM, sizeof( void * ) );
    if (!s_wps_queue) {
        wpa_printf(MSG_ERROR, "wps task init: failed to alloc queue");
        goto _wps_no_mem;
    }

    wps_rxq_init();

    ret = xTaskCreate(wps_task, "wpsT", WPS_TASK_STACK_SIZE, NULL, 2, &s_wps_task_hdl);
    if (pdPASS != ret) {
        wpa_printf(MSG_ERROR, "wps enable: failed to create task");
        goto _wps_no_mem;
    }

    xSemaphoreTake(s_wps_task_create_sem, portMAX_DELAY);
    vSemaphoreDelete(s_wps_task_create_sem);
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

    if (pdPASS == xSemaphoreTake(s_wps_api_sem, portMAX_DELAY)) {
        return param.ret;
    } else {
        return ESP_FAIL;
    }
}

int wps_check_wifi_mode(void)
{
    bool sniffer = false;
    wifi_mode_t mode;
    int ret;

    ret = esp_wifi_get_mode(&mode);
    if (ESP_OK != ret) {
        wpa_printf(MSG_ERROR, "wps check wifi mode: failed to get wifi mode ret=%d", ret);
        return ESP_FAIL;
    }

    ret = esp_wifi_get_promiscuous(&sniffer);
    if (ESP_OK != ret) {
        wpa_printf(MSG_ERROR, "wps check wifi mode: failed to get sniffer mode ret=%d", ret);
        return ESP_FAIL;
    }

    if (mode == WIFI_MODE_AP || mode == WIFI_MODE_NULL || sniffer == true) {
        wpa_printf(MSG_ERROR, "wps check wifi mode: wrong wifi mode=%d sniffer=%d", mode, sniffer);
        return ESP_ERR_WIFI_MODE;
    }

    return ESP_OK;
}

int esp_wifi_wps_enable(const esp_wps_config_t *config)
{
    int ret;

    if (ESP_OK != wps_check_wifi_mode()) {
        return ESP_ERR_WIFI_MODE;
    }

    API_MUTEX_TAKE();
    if (s_wps_enabled) {
        API_MUTEX_GIVE();
        wpa_printf(MSG_DEBUG, "wps enable: already enabled");
        return ESP_OK;
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
    wpa_printf(MSG_DEBUG, "wifi wps task: prio:%d, stack:%d\n", 2, WPS_TASK_STACK_SIZE);
    API_MUTEX_GIVE();
    return ret;
#else
    ret = wifi_wps_enable_internal(config);
    API_MUTEX_GIVE();
    return ret;
#endif
}

int wifi_wps_enable_internal(const esp_wps_config_t *config)
{
    int ret = 0;

    wpa_printf(MSG_DEBUG, "ESP WPS crypto initialize!");
    if (config->wps_type == WPS_TYPE_DISABLE) {
        wpa_printf(MSG_ERROR, "wps enable: invalid wps type");
        return ESP_ERR_WIFI_WPS_TYPE;
    }

    /* currently , we don't support REGISTRAR */
    if (IS_WPS_REGISTRAR(config->wps_type)) {
        wpa_printf(MSG_ERROR, "wps enable: not support registrar");
        return ESP_ERR_WIFI_WPS_TYPE;
    }

    wpa_printf(MSG_DEBUG, "Set factory information.");
    ret = wps_set_factory_info(config);
    if (ret != 0) {
        return ret;
    }

    wpa_printf(MSG_INFO, "wifi_wps_enable\n");

    wps_set_type(config->wps_type);
    wps_set_status(WPS_STATUS_DISABLE);

    ret = wifi_station_wps_init();

    if (ret != 0) {
        wps_set_type(WPS_STATUS_DISABLE);
        wps_set_status(WPS_STATUS_DISABLE);
        return ESP_FAIL;
    }

    return ESP_OK;
}

int wifi_wps_disable_internal(void)
{
    wps_set_status(WPS_STATUS_DISABLE);
    wifi_station_wps_deinit();
    return ESP_OK;
}

int esp_wifi_wps_disable(void)
{
    int ret = 0;

    if (ESP_OK != wps_check_wifi_mode()) {
        return ESP_ERR_WIFI_MODE;
    }

    API_MUTEX_TAKE();

    if (!s_wps_enabled) {
        wpa_printf(MSG_DEBUG, "wps disable: already disabled");
        API_MUTEX_GIVE();
        return ESP_OK;
    }

    wpa_printf(MSG_INFO, "wifi_wps_disable\n");
    wps_set_type(WPS_TYPE_DISABLE); /* Notify WiFi task */

    /* Call wps_delete_timer to delete all WPS timer, no timer will call wps_post()
     * to post message to wps_task once this function returns.
     */
    wps_delete_timer();

#ifdef USE_WPS_TASK
    ret = wps_post_block(SIG_WPS_DISABLE, 0);
#else
    ret = wifi_wps_disable_internal();
#endif

    if (ESP_OK != ret) {
        wpa_printf(MSG_ERROR, "wps disable: failed to disable wps, ret=%d", ret);
    }

    esp_wifi_disconnect();
    esp_wifi_set_wps_start_flag_internal(false);
    wps_task_deinit();
    s_wps_enabled = false;
    API_MUTEX_GIVE();
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

bool
wifi_set_wps_cb(wps_st_cb_t cb)
{
    wifi_mode_t mode;

    esp_wifi_get_mode(&mode);
    if (mode == WIFI_MODE_AP || mode == WIFI_MODE_NULL) {
        return false;
    }

    if (wps_station_wps_register_cb(cb) == 0) {
        return true;
    }

    return false;
}

