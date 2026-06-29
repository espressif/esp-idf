/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * NAN pairing bootstrapping, PASN pairing follow-up, and related frame helpers.
 */

#include "sdkconfig.h"

#if defined(CONFIG_ESP_WIFI_NAN_PAIRING)

#include <stdint.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_private/wifi.h"
#include "esp_private/wifi_types.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_nan.h"
#include "nan_i.h"
#include "apps_private/wifi_apps_private.h"
#include "os.h"
#include "utils/common.h"
#include "utils/eloop.h"

#include "esp_wifi_driver.h"

#if defined(CONFIG_ESP_WIFI_PASN_SUPPORT)
#include "esp_private/esp_supp_nan.h"
#include "common/defs.h"
#endif

static const char *TAG = "nan_pairing";

static esp_nan_app_bootstrap_cb_t s_bootstrap_cb;

/* Consume (clear and return) any pending verify-session for @a peer_nmi.
 * The flag lives on the own service that the peer was verified against, so it
 * survives even when no peer_svc_info exists for a MAC-randomised peer. */
static bool nan_pairing_take_verify_session(const uint8_t *peer_nmi)
{
    bool pending = false;

    if (!peer_nmi) {
        return false;
    }

    NAN_DATA_LOCK();
    for (int i = 0; i < ESP_WIFI_NAN_MAX_SVC_SUPPORTED; i++) {
        struct own_svc_info *own = &s_nan_ctx.own_svc[i];

        if (own->verify_session_pending &&
                MACADDR_EQUAL(own->verify_session_peer_nmi, peer_nmi)) {
            own->verify_session_pending = false;
            pending = true;
            break;
        }
    }
    NAN_DATA_UNLOCK();

    return pending;
}

void esp_nan_pairing_mark_verify_session(uint8_t own_inst_id, const uint8_t *peer_nmi)
{
    struct own_svc_info *own;

    if (!own_inst_id || !peer_nmi) {
        return;
    }

    NAN_DATA_LOCK();
    own = nan_find_own_svc(own_inst_id);
    if (own) {
        own->verify_session_pending = true;
        MACADDR_COPY(own->verify_session_peer_nmi, peer_nmi);
    }
    NAN_DATA_UNLOCK();
}

void esp_nan_pairing_clear_verify_session(const uint8_t *peer_nmi)
{
    if (!peer_nmi) {
        return;
    }

    NAN_DATA_LOCK();
    for (int i = 0; i < ESP_WIFI_NAN_MAX_SVC_SUPPORTED; i++) {
        struct own_svc_info *own = &s_nan_ctx.own_svc[i];

        if (own->verify_session_pending &&
                MACADDR_EQUAL(own->verify_session_peer_nmi, peer_nmi)) {
            own->verify_session_pending = false;
        }
    }
    NAN_DATA_UNLOCK();
}

/* Resolve the own service instance id a peer is associated with, used to anchor
 * a proactive verify-session flag. Returns 0 when no peer record exists yet. */
static uint8_t nan_pairing_resolve_own_inst(uint8_t peer_svc_id, const uint8_t *peer_nmi)
{
    struct peer_svc_info *peer;
    uint8_t own_inst_id = 0;

    if (!peer_nmi) {
        return 0;
    }

    NAN_DATA_LOCK();
    peer = nan_find_peer_svc(0, peer_svc_id, (uint8_t *)peer_nmi);
    if (peer) {
        own_inst_id = peer->own_svc_id;
    }
    NAN_DATA_UNLOCK();

    return own_inst_id;
}

esp_err_t esp_nan_app_set_bootstrap_cb(esp_nan_app_bootstrap_cb_t cb)
{
    s_bootstrap_cb = cb;
    return ESP_OK;
}

static void nan_app_bootstrap_notify(const wifi_nan_bootstrap_event_t *evt)
{
    if (!s_bootstrap_cb) {
        ESP_LOGW(TAG, "Bootstrap frame received but no callback registered");
        return;
    }
    s_bootstrap_cb(evt);
}

static bool nan_peer_cred_npk_present(const wifi_nan_peer_creds_t *c)
{
    static const uint8_t zero_npk[ESP_WIFI_NAN_NPK_LEN] = {0};

    if (!c || !c->is_valid) {
        return false;
    }
    return memcmp(c->npk, zero_npk, ESP_WIFI_NAN_NPK_LEN) != 0;
}

/* Resolve NPK from s_nan_ctx.peer_creds. Caller holds NAN_DATA_LOCK. */
static const wifi_nan_peer_creds_t *nan_peer_cred_lookup(void)
{
    const wifi_nan_peer_creds_t *only = NULL;
    uint8_t npk_slots = 0;

    for (int idx = 0; idx < ESP_WIFI_NAN_MAX_SVC_SUPPORTED; idx++) {
        const struct own_svc_info *own = &s_nan_ctx.own_svc[idx];

        if (own->svc_id == 0) {
            continue;
        }
        for (uint8_t i = 0; i < s_nan_ctx.num_peer_creds; i++) {
            if (s_nan_ctx.peer_creds[i].is_valid &&
                    memcmp(s_nan_ctx.peer_creds[i].service_hash, own->svc_hash, 6) == 0 &&
                    nan_peer_cred_npk_present(&s_nan_ctx.peer_creds[i])) {
                return &s_nan_ctx.peer_creds[i];
            }
        }
    }

    for (uint8_t i = 0; i < s_nan_ctx.num_peer_creds; i++) {
        if (!nan_peer_cred_npk_present(&s_nan_ctx.peer_creds[i])) {
            continue;
        }
        only = &s_nan_ctx.peer_creds[i];
        npk_slots++;
    }
    if (npk_slots == 1) {
        return only;
    }

    return NULL;
}

int nan_global_peer_npk_lookup(uint8_t *npk, size_t *npk_len, int *akmp)
{
    const wifi_nan_peer_creds_t *slot = NULL;

    if (!npk || !npk_len || !akmp) {
        return -1;
    }

    NAN_DATA_LOCK();
    slot = nan_peer_cred_lookup();
    if (slot) {
        memcpy(npk, slot->npk, ESP_WIFI_NAN_NPK_LEN);
        *npk_len = ESP_WIFI_NAN_NPK_LEN;
        /* Wi-Fi Aware pairing: PASN frames use SAE as the single base AKM. */
        *akmp = WPA_KEY_MGMT_SAE;
        NAN_DATA_UNLOCK();
        return 0;
    }
    NAN_DATA_UNLOCK();

    return -1;
}

struct nan_pasn_data *esp_nan_app_get_pasn_data(void)
{
    return s_nan_ctx.nan_pasn_data;
}

void esp_nan_app_set_pasn_data(struct nan_pasn_data *pd)
{
    s_nan_ctx.nan_pasn_data = pd;
}

bool nan_pairing_validate_publish_bootstrapping(uint16_t bootstrapping_methods)
{
    if (!bootstrapping_methods) {
        return true;
    }
    uint16_t valid_methods = WIFI_NAN_BOOTSTRAP_OPPORTUNISTIC | WIFI_NAN_BOOTSTRAP_PIN_CODE_DISPLAY;
    return (bootstrapping_methods & ~valid_methods) == 0;
}

bool nan_pairing_validate_subscribe_bootstrapping(uint16_t bootstrapping_methods)
{
    if (!bootstrapping_methods) {
        return true;
    }
    uint16_t valid_methods = WIFI_NAN_BOOTSTRAP_OPPORTUNISTIC | WIFI_NAN_BOOTSTRAP_PIN_CODE_KEYPAD;
    return (bootstrapping_methods & ~valid_methods) == 0;
}

uint16_t nan_app_parse_npba_from_publish(const struct nan_cb_npba_t *npba)
{
    if (!npba || npba->type != WIFI_NAN_NPBA_TYPE_ADVERTISE) {
        return 0;
    }
    return npba->methods;
}

void nan_app_bootstrap_indication(uint8_t peer_svc_id, uint8_t pub_id,
                                  uint8_t peer_nmi[6], uint16_t selected_method)
{
    wifi_nan_bootstrap_event_t evt = {0};

    ESP_LOGI(TAG, "Pairing Bootstrapping Request from "MACSTR" [pub_id=%d, method=0x%x]",
             MAC2STR(peer_nmi), pub_id, selected_method);

    evt.type = WIFI_NAN_NPBA_TYPE_REQUEST;
    evt.peer_svc_id = peer_svc_id;
    evt.own_svc_id = pub_id;
    MACADDR_COPY(evt.peer_nmi, peer_nmi);
    evt.methods = selected_method;

    nan_app_bootstrap_notify(&evt);
}

void nan_app_bootstrap_completed(uint8_t status, uint8_t peer_svc_id, uint8_t sub_id,
                                 uint8_t peer_nmi[6], uint16_t matched_method,
                                 uint8_t reason_code)
{
    wifi_nan_bootstrap_event_t evt = {0};

    ESP_LOGI(TAG, "Pairing Bootstrapping Response from "MACSTR" [sub_id=%d, status=%d, method=0x%x]",
             MAC2STR(peer_nmi), sub_id, status, matched_method);

    if (peer_nmi == NULL) {
        return;
    }

    evt.type = WIFI_NAN_NPBA_TYPE_RESPONSE;
    evt.status = status;
    evt.peer_svc_id = peer_svc_id;
    evt.own_svc_id = sub_id;
    MACADDR_COPY(evt.peer_nmi, peer_nmi);
    evt.methods = matched_method;
    evt.reason_code = reason_code;

    nan_app_bootstrap_notify(&evt);
}

bool nan_app_parse_npba_from_receive(uint8_t own_svc_id, uint8_t peer_svc_id,
                                     uint8_t peer_nmi[6], const struct nan_cb_npba_t *npba)
{
    if (!npba || !peer_nmi) {
        return false;
    }

    switch (npba->type) {
    case WIFI_NAN_NPBA_TYPE_REQUEST:
        nan_app_bootstrap_indication(peer_svc_id, own_svc_id, peer_nmi, npba->methods);
        return true;
    case WIFI_NAN_NPBA_TYPE_RESPONSE:
        nan_app_bootstrap_completed(npba->status, peer_svc_id, own_svc_id, peer_nmi,
                                    npba->methods, 0);
        return true;
    default:
        return false;
    }
}

static esp_err_t nan_send_bootstrap_followup(uint8_t inst_id, uint8_t peer_inst_id,
                                             uint8_t *peer_mac,
                                             wifi_nan_pairing_npba_params_t *pairing_npba)
{
    struct peer_svc_info *p_peer_svc;
    NAN_DATA_LOCK();
    p_peer_svc = nan_find_peer_svc(inst_id, peer_inst_id, peer_mac);
    if (!p_peer_svc) {
        ESP_LOGE(TAG, "Cannot send Pairing follow-up, peer not found!");
        NAN_DATA_UNLOCK();
        return ESP_FAIL;
    }

    wifi_nan_followup_params_t fup_params = {0};
    fup_params.inst_id = inst_id ? inst_id : p_peer_svc->own_svc_id;
    fup_params.peer_inst_id = peer_inst_id ? peer_inst_id : p_peer_svc->svc_id;
    if (!MACADDR_EQUAL(peer_mac, nan_pairing_get_null_mac())) {
        MACADDR_COPY(fup_params.peer_mac, p_peer_svc->peer_nmi);
    } else {
        MACADDR_COPY(fup_params.peer_mac, peer_mac);
    }
    NAN_DATA_UNLOCK();

    extra_params_internal_t params_i = { .pairing_npba = pairing_npba };

    os_event_group_clear_bits(nan_pairing_get_event_group(), NAN_TX_SUCCESS | NAN_TX_FAILURE);
    esp_err_t ret = esp_nan_internal_send_followup(&fup_params, nan_pairing_get_fup_context(), &params_i);
    if (ret != ESP_OK) {
        return ret;
    }

    EventBits_t bits = os_event_group_wait_bits(nan_pairing_get_event_group(),
                                                NAN_TX_SUCCESS | NAN_TX_FAILURE, pdFALSE, pdFALSE,
                                                pdMS_TO_TICKS(NAN_ACTION_TIMEOUT));
    if (bits & NAN_TX_SUCCESS) {
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t esp_wifi_nan_bootstrap_request(wifi_nan_pairing_bootstrap_req_t *req)
{
    if (!req) {
        ESP_LOGE(TAG, "Pairing request params NULL");
        return ESP_ERR_INVALID_ARG;
    }

    if (!req->selected_method ||
            (req->selected_method & (req->selected_method - 1))) {
        ESP_LOGE(TAG, "Exactly one bootstrapping method must be selected");
        return ESP_ERR_INVALID_ARG;
    }

    uint16_t valid_methods = WIFI_NAN_BOOTSTRAP_OPPORTUNISTIC | WIFI_NAN_BOOTSTRAP_PIN_CODE_KEYPAD;
    if (req->selected_method & ~valid_methods) {
        ESP_LOGE(TAG, "Invalid bootstrapping method for initiator (subscriber)");
        return ESP_ERR_INVALID_ARG;
    }

    wifi_nan_pairing_npba_params_t pairing_npba = {
        .type = WIFI_NAN_NPBA_TYPE_REQUEST,
        .status = req->status,
        .method = req->selected_method,
        .reason_code = 0,
        .comeback_after = req->comeback_after,
        .cookie = req->cookie,
    };

    esp_err_t ret = nan_send_bootstrap_followup(req->inst_id, req->peer_inst_id,
                                                req->peer_mac, &pairing_npba);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Sent Pairing Bootstrapping request to Peer "MACSTR" [method=0x%x]",
                 MAC2STR(req->peer_mac), req->selected_method);
    } else {
        ESP_LOGE(TAG, "Failed to send Pairing Bootstrapping request!");
    }
    return ret;
}

esp_err_t esp_wifi_nan_bootstrap_response(wifi_nan_pairing_bootstrapping_resp_t *resp)
{
    if (!resp) {
        ESP_LOGE(TAG, "Pairing response params NULL");
        return ESP_ERR_INVALID_ARG;
    }

    if (resp->status == WIFI_NAN_PAIRING_STATUS_ACCEPTED) {
        uint16_t valid_methods = WIFI_NAN_BOOTSTRAP_OPPORTUNISTIC | WIFI_NAN_BOOTSTRAP_PIN_CODE_DISPLAY;
        if (!resp->matched_method || (resp->matched_method & ~valid_methods)) {
            ESP_LOGE(TAG, "Invalid matched bootstrapping method for responder (publisher)");
            return ESP_ERR_INVALID_ARG;
        }
    }

    wifi_nan_pairing_npba_params_t pairing_npba = {
        .type = WIFI_NAN_NPBA_TYPE_RESPONSE,
        .status = resp->status,
        .method = resp->matched_method,
        .reason_code = resp->reason_code,
        .comeback_after = resp->comeback_after,
        .cookie = resp->cookie,
    };

    esp_err_t ret = nan_send_bootstrap_followup(resp->inst_id, resp->peer_inst_id,
                                                resp->peer_mac, &pairing_npba);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Sent Pairing Bootstrapping response to Peer "MACSTR" [status=%d]",
                 MAC2STR(resp->peer_mac), resp->status);
    } else {
        ESP_LOGE(TAG, "Failed to send Pairing Bootstrapping response!");
    }
    return ret;
}

/* NIRA: ID(1) + Len(2) + CipherVersion(1) + Nonce(8) + Tag(8) = 20 */
#define NAN_ATTR_ID_IDENTITY_RESOLUTION     0x2B
#define NAN_NIRA_NONCE_LEN                  8
#define NAN_NIRA_TAG_LEN                    8
#define NAN_NIRA_CIPHER_VER                 0
#define NAN_NIRA_ATTR_LEN                   (3 + 1 + NAN_NIRA_NONCE_LEN + NAN_NIRA_TAG_LEN)
#define NAN_NIRA_STR                        "NIR"
#define NAN_NIRA_STR_LEN                    3

uint32_t esp_nan_get_nira_len(void)
{
    return NAN_NIRA_ATTR_LEN;
}

int esp_nan_construct_nira(uint8_t *frm)
{
    const uint8_t *nonce;
    const uint8_t *tag;
    uint8_t fresh_nonce[NAN_NIRA_NONCE_LEN];
    uint8_t fresh_tag[NAN_NIRA_TAG_LEN];

    if (!frm) {
        return 0;
    }

    if (s_nan_ctx.nira_cached) {
        nonce = s_nan_ctx.cached_nira_nonce;
        tag = s_nan_ctx.cached_nira_tag;
    } else {
        uint8_t own_nmi[MACADDR_LEN];
        const unsigned char *addr[3];
        int len_arr[3];
        uint8_t digest[32];

        if (!s_nan_ctx.own_nik_valid) {
            ESP_LOGW(TAG, "NIRA: own NIK is not available");
            return 0;
        }
        if (!g_wifi_default_wpa_crypto_funcs.hmac_sha256_vector) {
            ESP_LOGE(TAG, "NIRA: hmac_sha256_vector not registered");
            return 0;
        }
        if (esp_wifi_get_mac(WIFI_IF_NAN, own_nmi) != ESP_OK) {
            ESP_LOGE(TAG, "NIRA: failed to read NAN NMI");
            return 0;
        }
        if (os_get_random(fresh_nonce, sizeof(fresh_nonce)) != 0) {
            ESP_LOGE(TAG, "NIRA: failed to generate nonce");
            return 0;
        }

        /* Tag = Truncate-64(HMAC-SHA-256(NIK, "NIR" || NMI || Nonce)) */
        addr[0] = (const unsigned char *)NAN_NIRA_STR;
        len_arr[0] = NAN_NIRA_STR_LEN;
        addr[1] = own_nmi;
        len_arr[1] = MACADDR_LEN;
        addr[2] = fresh_nonce;
        len_arr[2] = NAN_NIRA_NONCE_LEN;
        if (g_wifi_default_wpa_crypto_funcs.hmac_sha256_vector(s_nan_ctx.own_nik,
                                                               ESP_WIFI_NAN_NIK_LEN,
                                                               3, addr, len_arr,
                                                               digest) != 0) {
            ESP_LOGE(TAG, "NIRA: tag derivation failed");
            return 0;
        }
        memcpy(fresh_tag, digest, NAN_NIRA_TAG_LEN);
        memset(digest, 0, sizeof(digest));

        memcpy(s_nan_ctx.cached_nira_nonce, fresh_nonce, NAN_NIRA_NONCE_LEN);
        memcpy(s_nan_ctx.cached_nira_tag, fresh_tag, NAN_NIRA_TAG_LEN);
        s_nan_ctx.nira_cached = true;

        nonce = fresh_nonce;
        tag = fresh_tag;
    }

    uint8_t *p = frm;
    *p++ = NAN_ATTR_ID_IDENTITY_RESOLUTION;
    /* Attribute Length: CipherVersion(1) + Nonce(8) + Tag(8) = 17 */
    *p++ = 17 & 0xFF;
    *p++ = (17 >> 8) & 0xFF;
    *p++ = NAN_NIRA_CIPHER_VER;
    memcpy(p, nonce, NAN_NIRA_NONCE_LEN);
    p += NAN_NIRA_NONCE_LEN;
    memcpy(p, tag, NAN_NIRA_TAG_LEN);
    p += NAN_NIRA_TAG_LEN;

    return (int)(p - frm);
}

#include "common/ieee802_11_defs.h"
#include "common/wpa_common.h"

#define NAN_PAIRING_FUP_MIN_ATTR_LEN 3
#define NAN_PASN_KDE_OUI_TYPE_NIK       36
#define NAN_PASN_KDE_OUI_TYPE_LIFETIME  37
#define NAN_PASN_KEY_LIFETIME_NIK_BIT   BIT(3)
#define NAN_ATTR_ID_SHARED_KEY_DESC     0x24
#define NAN_PAIRING_NIK_FUP_TIMEOUT_SEC 5

struct nan_pairing_fup_ctx {
    uint8_t svc_id;
    uint8_t peer_svc_id;
    uint8_t peer_mac[MACADDR_LEN];
    size_t shared_key_attr_len;
    uint8_t shared_key_attr[];
};

static struct peer_svc_info *nan_find_peer_svc_exact(uint8_t own_svc_id, uint8_t peer_svc_id,
                                                     const uint8_t *peer_mac)
{
    struct peer_svc_info *temp;

    if (!own_svc_id || !peer_svc_id || !peer_mac) {
        return NULL;
    }

    for (int i = 0; i < ESP_WIFI_NAN_MAX_SVC_SUPPORTED; i++) {
        struct own_svc_info *own = &s_nan_ctx.own_svc[i];

        if (own->svc_id != own_svc_id) {
            continue;
        }

        SLIST_FOREACH(temp, &(own->peer_list), next) {
            if (temp->svc_id == peer_svc_id &&
                    memcmp(temp->peer_nmi, peer_mac, MACADDR_LEN) == 0) {
                return temp;
            }
        }
        break;
    }

    return NULL;
}

static void nan_pairing_nik_fup_timeout_cb(void *eloop_data, void *user_ctx)
{
    struct own_svc_info *own = user_ctx;
    wifi_event_nan_pairing_complete_t evt = {0};

    (void)eloop_data;

    if (!own || !own->nik_fup_pending) {
        return;
    }

    own->nik_fup_pending = false;
    evt.status = WIFI_NAN_PAIRING_STATUS_ACCEPTED;
    evt.reason_code = WIFI_NAN_PAIRING_REASON_NIK_FUP_TIMEOUT;
    MACADDR_COPY(evt.peer_nmi, own->nik_fup_pending_peer_nmi);
    nan_app_remove_paired_peer(own->nik_fup_pending_peer_nmi);
    esp_nan_set_pairing_status(own->svc_id, 0, own->nik_fup_pending_peer_nmi, false);
    nan_app_post_event(WIFI_EVENT_NAN_PAIRING_CONFIRM, &evt, sizeof(evt));
    ESP_LOGW(TAG, "Failed to complete NIK exchange with peer " MACSTR
             " (reason=%u)", MAC2STR(own->nik_fup_pending_peer_nmi), evt.reason_code);
}

void nan_pairing_cancel_svc_pending(struct own_svc_info *own)
{
    if (!own || !own->nik_fup_pending) {
        return;
    }

    eloop_cancel_timeout(nan_pairing_nik_fup_timeout_cb, NULL, own);
    own->nik_fup_pending = false;
}

static void nan_pairing_arm_pending(struct own_svc_info *own, const uint8_t *peer_mac)
{
    if (!own || !peer_mac) {
        return;
    }

    nan_pairing_cancel_svc_pending(own);
    MACADDR_COPY(own->nik_fup_pending_peer_nmi, peer_mac);
    own->nik_fup_pending = true;

    if (eloop_register_timeout(NAN_PAIRING_NIK_FUP_TIMEOUT_SEC, 0,
                               nan_pairing_nik_fup_timeout_cb, NULL,
                               own) != 0) {
        own->nik_fup_pending = false;
    }
}

/**
 * Build the WFA OUI + GSP protocol header that prefixes a Generic Service
 * Protocol SSI (Wi-Fi Aware v4.0 §4.2.7). Ported from esp-nsd
 * @c add_ssi_gsp_attr (wa_sd.c).
 */
static int nan_pairing_add_ssi_gsp_attr(uint8_t *buf)
{
    wifi_nan_wfa_ssi_t *wfa_ssi = (wifi_nan_wfa_ssi_t *)buf;
    static const uint8_t oui_wfa[WIFI_OUI_LEN] = { 0x50, 0x6F, 0x9A };

    memcpy(wfa_ssi->wfa_oui, oui_wfa, WIFI_OUI_LEN);
    wfa_ssi->proto = WIFI_SVC_PROTO_GENERIC;

    return sizeof(wifi_nan_wfa_ssi_t);
}

/**
 * Build a GSP SSI (WFA OUI + proto=Generic header only).
 */
static size_t nan_pairing_build_srv_ssi(uint8_t *buf, size_t buf_len)
{
    uint8_t *p = buf;

    if (!buf || buf_len < sizeof(wifi_nan_wfa_ssi_t)) {
        return 0;
    }

    p += nan_pairing_add_ssi_gsp_attr(p);

    return (size_t)(p - buf);
}

/**
 * Derive a NIRA tag for cipher version 0 (Wi-Fi Aware v4.0):
 *   Tag = Truncate-64(HMAC-SHA-256(NIK, "NIR" || NMI || Nonce))
 *
 * Ported from hostap @c nan_crypto_derive_nira_tag (src/nan/nan_crypto.c),
 * adapted to the ESP-IDF crypto trampoline.
 */
static int nan_pairing_derive_nira_tag(const uint8_t nik[NAN_PASN_NIK_LEN],
                                       const uint8_t nmi_addr[ETH_ALEN],
                                       const uint8_t nira_nonce[NAN_NIRA_NONCE_LEN],
                                       uint8_t tag_out[NAN_NIRA_TAG_LEN])
{
    const unsigned char *addr[3];
    int len_arr[3];
    uint8_t digest[32];

    if (!nik || !nmi_addr || !nira_nonce || !tag_out) {
        return -1;
    }
    if (!g_wifi_default_wpa_crypto_funcs.hmac_sha256_vector) {
        return -1;
    }

    addr[0] = (const unsigned char *)NAN_NIRA_STR;
    len_arr[0] = NAN_NIRA_STR_LEN;
    addr[1] = nmi_addr;
    len_arr[1] = ETH_ALEN;
    addr[2] = nira_nonce;
    len_arr[2] = NAN_NIRA_NONCE_LEN;

    if (g_wifi_default_wpa_crypto_funcs.hmac_sha256_vector(nik, NAN_PASN_NIK_LEN,
                                                           3, addr, len_arr,
                                                           digest) != 0) {
        return -1;
    }

    memcpy(tag_out, digest, NAN_NIRA_TAG_LEN);
    memset(digest, 0, sizeof(digest));
    return 0;
}

static size_t nan_pairing_build_plain_key_data(uint8_t *buf, size_t buf_len,
                                               const uint8_t nik[NAN_PASN_NIK_LEN])
{
    size_t pos = 0;

    if (!buf || !nik || buf_len < 40) {
        return 0;
    }

    /* NIK KDE: EID + Len + OUI + OUI Type + Cipher Ver + NIK. */
    buf[pos++] = WLAN_EID_VENDOR_SPECIFIC;
    buf[pos++] = 4 + 1 + NAN_PASN_NIK_LEN;
    buf[pos++] = 0x50;
    buf[pos++] = 0x6f;
    buf[pos++] = 0x9a;
    buf[pos++] = NAN_PASN_KDE_OUI_TYPE_NIK;
    buf[pos++] = 0; /* NAN_NIRA_CIPHER_VER_128 — matches hostap and iPhone */
    memcpy(&buf[pos], nik, NAN_PASN_NIK_LEN);
    pos += NAN_PASN_NIK_LEN;

    /* Key Lifetime KDE: hostap's RX path rejects the frame when this KDE is
     * missing (nan_pairing_followup_rx, "Key Lifetime KDE missing"). iPhone
     * is likely the same. Layout: bitmap(LE16) | lifetime(BE32). */
    buf[pos++] = WLAN_EID_VENDOR_SPECIFIC;
    buf[pos++] = 4 + 2 + 4;
    buf[pos++] = 0x50;
    buf[pos++] = 0x6f;
    buf[pos++] = 0x9a;
    buf[pos++] = NAN_PASN_KDE_OUI_TYPE_LIFETIME;
    WPA_PUT_LE16(&buf[pos], NAN_PASN_KEY_LIFETIME_NIK_BIT);
    pos += 2;
    WPA_PUT_BE32(&buf[pos], s_nan_ctx.nik_lifetime);
    pos += 4;

    /*
     * AES-WRAP requires plaintext length to be a multiple of 8. IEEE
     * 802.11-2020 §12.7.2 specifies Key Data padding as a single 0xDD byte
     * followed by zeros (not a sequence of well-formed vendor IEs).
     */
    size_t pad = (8 - (pos % 8)) % 8;
    if (pad) {
        if (buf_len - pos < pad) {
            return 0;
        }
        buf[pos++] = 0xDD;
        while (--pad) {
            buf[pos++] = 0x00;
        }
    }

    return pos;
}

static esp_err_t nan_app_send_pairing_followup(uint8_t svc_id, uint8_t peer_svc_id,
                                               const uint8_t *peer_mac,
                                               const uint8_t *shared_key_attr,
                                               size_t shared_key_attr_len)
{
    const struct nan_pasn_key_material *saved;
    uint8_t plain[64] = {0};
    uint8_t wrapped[80] = {0};
    uint8_t key_desc[sizeof(struct wpa_eapol_key)] = {0};
    uint8_t shared_key_wrapped[sizeof(struct wpa_eapol_key) + sizeof(wrapped)] = {0};
    uint8_t nira_attr[NAN_NIRA_ATTR_LEN] = {0};
    wifi_nan_wfa_ssi_t srv_ssi = {0};
    uint8_t nik[NAN_PASN_NIK_LEN];
    size_t plain_len;
    size_t wrapped_len;
    size_t sk_attr_len;
    size_t nira_len;
    size_t srv_ssi_len;
    uint16_t key_info;
    uint8_t *w = shared_key_wrapped;
    uint32_t tx_ctx = 0;
    wifi_nan_followup_params_t fup = {0};
    extra_params_internal_t params_i = {0};

    (void)shared_key_attr;
    (void)shared_key_attr_len;

    if (!peer_mac) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!s_nan_ctx.own_nik_valid) {
        ESP_LOGW(TAG, "Pairing follow-up: own NIK is not available");
        return ESP_ERR_INVALID_STATE;
    }
    memcpy(nik, s_nan_ctx.own_nik, sizeof(nik));

    saved = nan_pasn_get_saved_keys();
    if (!saved || !saved->kek_len) {
        ESP_LOGW(TAG, "Pairing follow-up: missing saved KEK");
        return ESP_ERR_INVALID_STATE;
    }

    plain_len = nan_pairing_build_plain_key_data(plain, sizeof(plain), nik);
    if (plain_len == 0 || plain_len % 8) {
        return ESP_FAIL;
    }

    if (plain_len / 8 > sizeof(wrapped) / 8 - 1) {
        return ESP_ERR_INVALID_SIZE;
    }
    wrapped_len = plain_len + 8;
    if (!g_wifi_default_wpa_crypto_funcs.aes_wrap) {
        ESP_LOGE(TAG, "Pairing follow-up: aes_wrap not registered");
        return ESP_FAIL;
    }
    if (g_wifi_default_wpa_crypto_funcs.aes_wrap(saved->kek, saved->kek_len, plain_len / 8, plain, wrapped) != 0) {
        return ESP_FAIL;
    }

    key_desc[NAN_KEY_DESC_TYPE_OFF] = NAN_KEY_DESC_TYPE_RSN;
    /* Key Info: AKM-defined | Pairwise | Install | ACK | Secure | Encrypted
     * Key Data = 0x12C8. MIC bit deliberately *not* set — MIC computation is
     * skipped here because we can only sign the SKDA bytes we own, while
     * hostap (and iPhone) sign the entire assembled NAF body. Without that
     * full-body assembly hook, an incorrect MIC is worse than none. */
    key_info = (uint16_t)(WPA_KEY_INFO_TYPE_AKM_DEFINED |
                          WPA_KEY_INFO_KEY_TYPE |
                          WPA_KEY_INFO_INSTALL |
                          WPA_KEY_INFO_ACK |
                          WPA_KEY_INFO_SECURE |
                          WPA_KEY_INFO_ENCR_KEY_DATA);
    key_desc[NAN_KEY_DESC_KEY_INFO_OFF] = (uint8_t)((key_info >> 8) & 0xff);
    key_desc[NAN_KEY_DESC_KEY_INFO_OFF + 1] = (uint8_t)(key_info & 0xff);
    WPA_PUT_BE16(&key_desc[NAN_KEY_DESC_DATA_LEN_OFF], (uint16_t)wrapped_len);

    memcpy(w, key_desc, sizeof(key_desc));
    w += sizeof(key_desc);
    memcpy(w, wrapped, wrapped_len);
    w += wrapped_len;

    /* SSI: GSP (WFA OUI + proto=Generic). */
    srv_ssi_len = nan_pairing_build_srv_ssi((uint8_t *)&srv_ssi, sizeof(srv_ssi));
    if (srv_ssi_len == 0) {
        ESP_LOGW(TAG, "Pairing follow-up: failed to build GSP SSI");
        return ESP_FAIL;
    }

    fup.inst_id = svc_id;
    fup.peer_inst_id = peer_svc_id;
    MACADDR_COPY(fup.peer_mac, peer_mac);
    fup.ssi = (uint8_t *)&srv_ssi;
    fup.ssi_len = (uint16_t)srv_ssi_len;

    sk_attr_len = (size_t)(w - shared_key_wrapped);
    params_i.pairing_npba = NULL;
    params_i.shared_key_wrapped = shared_key_wrapped;
    params_i.shared_key_wrapped_len = (uint16_t)sk_attr_len;

    /* NIRA proves possession of the NIK we just wrapped above; iPhone uses
     * it to bind the NIK to the sender and won't commit the pairing record
     * without it. */
    nira_len = esp_nan_construct_nira(nira_attr);
    if (nira_len == 0) {
        ESP_LOGW(TAG, "Pairing follow-up: NIRA attribute build failed");
        return ESP_FAIL;
    }
    params_i.nira_attr = nira_attr;
    params_i.nira_attr_len = (uint16_t)nira_len;

    ESP_LOGI(TAG, "Pairing follow-up: sending key-desc payload to " MACSTR, MAC2STR(peer_mac));
    return esp_nan_internal_send_followup(&fup, &tx_ctx, &params_i);
}

static void nan_app_send_pairing_followup_eloop(void *eloop_data, void *user_data)
{
    struct nan_pairing_fup_ctx *ctx = (struct nan_pairing_fup_ctx *)user_data;
    (void)eloop_data;

    if (!ctx) {
        return;
    }

    (void) nan_app_send_pairing_followup(ctx->svc_id, ctx->peer_svc_id,
                                         ctx->peer_mac,
                                         ctx->shared_key_attr,
                                         ctx->shared_key_attr_len);
    os_free(ctx);
}

/* Insert or refresh a (peer NIK, NPK) entry in the in-RAM credential cache used
 * for NIRA identity resolution. Caller holds NAN_DATA_LOCK. A @a npk of NULL
 * stores a zeroed key; NULL @a service_hash stores zeros. When the cache is
 * full the oldest entry (slot 0) is reused. */
static void nan_app_update_peer_creds(const uint8_t *peer_nik, const uint8_t *npk,
                                      const uint8_t service_hash[6])
{
    wifi_nan_peer_creds_t *slot = NULL;

    /* Reuse the slot already holding this NIK, if any. */
    for (uint8_t i = 0; i < s_nan_ctx.num_peer_creds; i++) {
        if (s_nan_ctx.peer_creds[i].is_valid &&
                os_memcmp(s_nan_ctx.peer_creds[i].peer_nik, peer_nik, ESP_WIFI_NAN_NIK_LEN) == 0) {
            slot = &s_nan_ctx.peer_creds[i];
            break;
        }
    }
    if (!slot) {
        if (s_nan_ctx.num_peer_creds < ESP_WIFI_NAN_MAX_PEER_CREDS) {
            slot = &s_nan_ctx.peer_creds[s_nan_ctx.num_peer_creds++];
        } else {
            slot = &s_nan_ctx.peer_creds[0];
        }
    }

    memcpy(slot->peer_nik, peer_nik, ESP_WIFI_NAN_NIK_LEN);
    if (service_hash) {
        memcpy(slot->service_hash, service_hash, 6);
    } else {
        memset(slot->service_hash, 0, sizeof(slot->service_hash));
    }
    if (npk) {
        memcpy(slot->npk, npk, ESP_WIFI_NAN_NPK_LEN);
    } else {
        memset(slot->npk, 0, ESP_WIFI_NAN_NPK_LEN);
    }
    slot->is_valid = true;
}

static void nan_pairing_post_confirm(const uint8_t *peer_nmi)
{
    wifi_event_nan_pairing_complete_t evt = {0};

    evt.status = WIFI_NAN_PAIRING_STATUS_ACCEPTED;
    MACADDR_COPY(evt.peer_nmi, peer_nmi);
    nan_app_post_event(WIFI_EVENT_NAN_PAIRING_CONFIRM, &evt, sizeof(evt));
}

static void nan_pairing_key_installed_cb(const uint8_t *peer_nmi,
                                         uint8_t role,
                                         uint8_t pairing_verification,
                                         uint8_t ndp_csid,
                                         const uint8_t *nd_pmk,
                                         size_t nd_pmk_len,
                                         uint32_t nik_lifetime_sec)
{
    (void)pairing_verification;

    if (!peer_nmi) {
        return;
    }

    const struct nan_pasn_key_material *keys = nan_pasn_get_saved_keys();
    uint8_t peer_remote_svc_id = 0;
    bool npk_nik_caching = true;
    uint8_t own_svc_id = 0;

    NAN_DATA_LOCK();
    struct peer_svc_info *peer = nan_find_peer_svc(0, 0, (uint8_t *)peer_nmi);
    if (peer) {
        struct own_svc_info *own = nan_find_own_svc(peer->own_svc_id);

        peer_remote_svc_id = peer->svc_id;
        own_svc_id = peer->own_svc_id;
        if (own) {
            npk_nik_caching = own->pairing.npk_nik_caching;
        }
        if (keys && keys->pmk_len && keys->pmk_len <= ESP_WIFI_NAN_NPK_LEN &&
                peer->has_nik) {
            uint8_t npk_tmp[ESP_WIFI_NAN_NPK_LEN] = {0};

            memcpy(npk_tmp, keys->pmk, keys->pmk_len);
            nan_app_update_peer_creds(peer->peer_nik, npk_tmp,
                                      own ? own->svc_hash : NULL);
        }
    }
    NAN_DATA_UNLOCK();

    uint32_t lifetime_sec = nik_lifetime_sec;

    /* Cache ND-PMK for future paired NDPs (Wi-Fi Aware v4.0 §7.6.4.2). */
    if (ndp_csid && nd_pmk && nd_pmk_len == ESP_WIFI_NAN_NDP_PMK_LEN) {
        (void)nan_app_register_paired_peer(peer_nmi, role, ndp_csid,
                                           nd_pmk, nd_pmk_len,
                                           lifetime_sec);
    } else {
        ESP_LOGW(TAG, "Pairing complete for " MACSTR
                 ": ND-PMK unavailable (csid=%u nd_pmk_len=%u); "
                 "paired-peer cache not updated",
                 MAC2STR(peer_nmi), ndp_csid, (unsigned)nd_pmk_len);
    }

    /*
     * Re-pair verification has no follow-up exchange (the initiator
     * goes straight to NDP). Post PAIRING_CONFIRM here so the application
     * is notified that PASN re-pairing is complete.
     */
    if (nan_pairing_take_verify_session(peer_nmi) &&
            (role == NAN_ROLE_PAIRING_RESPONDER || role == NAN_ROLE_PAIRING_INITIATOR)) {
        /* Re-verification: no follow-up ping-pong; notify app directly. */
        esp_nan_set_pairing_status(own_svc_id, peer_remote_svc_id, (uint8_t *)peer_nmi, true);
        nan_pairing_post_confirm(peer_nmi);
        return;
    }

    if (own_svc_id && !npk_nik_caching) {
        wifi_event_nan_pairing_complete_t evt = {0};

        evt.status = WIFI_NAN_PAIRING_STATUS_ACCEPTED;
        evt.reason_code = 0;
        MACADDR_COPY(evt.peer_nmi, peer_nmi);
        esp_nan_set_pairing_status(own_svc_id, peer_remote_svc_id, (uint8_t *)peer_nmi, true);
        nan_app_post_event(WIFI_EVENT_NAN_PAIRING_CONFIRM, &evt, sizeof(evt));
        return;
    }

    if (role == NAN_ROLE_PAIRING_INITIATOR) {

        struct nan_pairing_fup_ctx *ctx = os_zalloc(sizeof(*ctx));
        if (!ctx) {
            ESP_LOGW(TAG, "Pairing key installed: failed to alloc fup ctx for " MACSTR,
                     MAC2STR(peer_nmi));
            return;
        }

        ctx->svc_id      = own_svc_id;
        ctx->peer_svc_id = peer_remote_svc_id;

        /* Without a peer service entry the follow-up would carry zero service
         * IDs; drop it rather than send an invalid frame. */
        if (!ctx->svc_id || !ctx->peer_svc_id) {
            ESP_LOGW(TAG, "Pairing key installed: peer service not found for " MACSTR
                     ", skipping initiator follow-up", MAC2STR(peer_nmi));
            os_free(ctx);
            return;
        }

        MACADDR_COPY(ctx->peer_mac, peer_nmi);
        ctx->shared_key_attr_len = 0;

        bool armed_pending = false;
        NAN_DATA_LOCK();
        struct own_svc_info *own = nan_find_own_svc(ctx->svc_id);
        if (own) {
            nan_pairing_arm_pending(own, peer_nmi);
            armed_pending = true;
        }
        NAN_DATA_UNLOCK();

        if (eloop_register_timeout(0, 0, nan_app_send_pairing_followup_eloop, NULL, ctx) != 0) {
            ESP_LOGW(TAG, "Pairing key installed: failed to schedule initiator follow-up");
            if (armed_pending) {
                NAN_DATA_LOCK();
                own = nan_find_own_svc(ctx->svc_id);
                if (own) {
                    nan_pairing_cancel_svc_pending(own);
                }
                NAN_DATA_UNLOCK();
            }
            os_free(ctx);
        }
        return;
    }

    if (role == NAN_ROLE_PAIRING_RESPONDER) {
        NAN_DATA_LOCK();
        struct own_svc_info *own = nan_find_own_svc(own_svc_id);
        if (own) {
            nan_pairing_arm_pending(own, peer_nmi);
        }
        NAN_DATA_UNLOCK();
    }
}

esp_err_t esp_wifi_nan_pairing_start(wifi_nan_pairing_config_t *cfg)
{
    if (!cfg) {
        ESP_LOGE(TAG, "Pairing config NULL");
        return ESP_ERR_INVALID_ARG;
    }

#if defined(CONFIG_ESP_WIFI_PASN_SUPPORT)
    if (!cfg->pairing_verification &&
            cfg->cred.pincode != UINT32_MAX &&
            cfg->cred.pincode > NAN_PAIRING_PINCODE_MAX) {
        ESP_LOGE(TAG, "Invalid pincode %u (valid range %u..%u or UINT32_MAX for default)",
                 cfg->cred.pincode, NAN_PAIRING_PINCODE_MIN, NAN_PAIRING_PINCODE_MAX);
        return ESP_ERR_INVALID_ARG;
    }
#endif

    int ret;

    switch (cfg->self_role) {
    case NAN_PAIRING_ROLE_RESPONDER:
        /* Verify-session is marked on Auth1 RX after NIRA OK in
         * handle_auth_pasn(); initiator-side verify marks in pairing_start(). */
        if (!cfg->pairing_verification) {
            esp_nan_pairing_clear_verify_session(cfg->peer_nmi);
        }
        ret = esp_nan_supp_pasn_responder_init(cfg->peer_nmi, cfg->cred.pincode,
                                               0,
                                               nan_pairing_key_installed_cb);
        if (ret != 0) {
            ESP_LOGE(TAG, "NAN PASN responder init failed for "MACSTR, MAC2STR(cfg->peer_nmi));
            return ESP_FAIL;
        }
        break;
    case NAN_PAIRING_ROLE_INITIATOR:
        if (cfg->pairing_verification) {
            esp_nan_pairing_mark_verify_session(
                nan_pairing_resolve_own_inst(cfg->peer_svc_id, cfg->peer_nmi),
                cfg->peer_nmi);
            ret = esp_nan_supp_pasn_initiator_verify(cfg->peer_nmi,
                                                     nan_pairing_key_installed_cb);
        } else {
            esp_nan_pairing_clear_verify_session(cfg->peer_nmi);
            ret = esp_nan_supp_pasn_initiator_auth(cfg->peer_nmi, cfg->cred.pincode,
                                                   0,
                                                   nan_pairing_key_installed_cb);
        }
        if (ret != 0) {
            ESP_LOGE(TAG, "NAN PASN initiator %s failed for "MACSTR,
                     cfg->pairing_verification ? "verify" : "auth",
                     MAC2STR(cfg->peer_nmi));
            return ESP_FAIL;
        }
        break;
    default:
        ESP_LOGE(TAG, "Invalid pairing role %d", cfg->self_role);
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

void nan_app_receive_pairing_followup(uint8_t svc_id, uint8_t peer_svc_id,
                                      const uint8_t *peer_mac,
                                      const uint8_t *shared_key_attr,
                                      size_t shared_key_attr_buf_len)
{
    size_t attr_body_len;
    size_t total_len;
    uint8_t nik[NAN_APP_PEER_NIK_LEN];
    uint8_t cipher_ver = 0;
    uint32_t lifetime_sec = 0;
    struct nan_pairing_fup_ctx *ctx;
    size_t alloc_len;
    if (!shared_key_attr || !peer_mac) {
        return;
    }
    if (shared_key_attr_buf_len < NAN_PAIRING_FUP_MIN_ATTR_LEN) {
        return;
    }
    if (shared_key_attr[0] != NAN_ATTR_ID_SHARED_KEY_DESC) {
        return;
    }
    attr_body_len = WPA_GET_LE16(&shared_key_attr[1]);
    if (attr_body_len > shared_key_attr_buf_len - 3) {
        ESP_LOGW(TAG, "Pairing follow-up: truncated Shared Key Descriptor (body=%zu, avail=%zu)",
                 attr_body_len, shared_key_attr_buf_len);
        return;
    }
    total_len = attr_body_len + 3;
    if (total_len > shared_key_attr_buf_len) {
        return;
    }
    if (nan_pasn_followup_decrypt_keys(shared_key_attr, total_len,
                                       nik, sizeof(nik),
                                       &cipher_ver,
                                       &lifetime_sec) != 0) {
        ESP_LOGW(TAG, "Pairing follow-up: failed to decrypt peer key data");
        return;
    }

    bool already_had_nik = false;
    bool pairing_completed = false;
    bool persist_creds = false;
    uint8_t persist_npk[ESP_WIFI_NAN_NPK_LEN] = {0};
    struct own_svc_info *own = NULL;

    NAN_DATA_LOCK();
    struct peer_svc_info *p_peer_svc = nan_find_peer_svc_exact(svc_id, peer_svc_id, peer_mac);
    if (!p_peer_svc) {
        /* NDP may have created a peer entry with peer_svc_id=0 before follow-up. */
        p_peer_svc = nan_find_peer_svc(svc_id, 0, (uint8_t *)peer_mac);
    }
    if (p_peer_svc) {
        already_had_nik = p_peer_svc->has_nik;
        memcpy(p_peer_svc->peer_nik, nik, NAN_APP_PEER_NIK_LEN);
        p_peer_svc->peer_nik_cipher_ver = cipher_ver;
        p_peer_svc->peer_nik_lifetime_sec = lifetime_sec;
        p_peer_svc->has_nik = true;
        s_nan_ctx.nik_lifetime = lifetime_sec;
        ESP_LOGI(TAG, "Stored peer NIK from " MACSTR " (cipher_ver=%u, lifetime=%u s)",
                 MAC2STR(peer_mac), cipher_ver, lifetime_sec);

        /* Refresh peer_creds (and NVS when enabled) with peer NIK and PASN NPK. */
        const struct nan_paired_peer *paired = nan_app_find_paired_peer(peer_mac);
        const struct nan_pasn_key_material *keys = nan_pasn_get_saved_keys();
        const uint8_t *npk_src = NULL;

        if (keys && keys->pmk_len && keys->pmk_len <= ESP_WIFI_NAN_NPK_LEN) {
            memcpy(persist_npk, keys->pmk, keys->pmk_len);
            npk_src = persist_npk;
        } else if (paired) {
            memcpy(persist_npk, paired->nd_pmk, ESP_WIFI_NAN_NPK_LEN);
            npk_src = persist_npk;
        }

        own = nan_find_own_svc(p_peer_svc->own_svc_id);

        if (!already_had_nik) {
            pairing_completed = true;
        }

        nan_app_update_peer_creds(nik, npk_src, own ? own->svc_hash : NULL);
        if (s_nan_ctx.nik_lifetime == 0 && s_nan_ctx.use_nvs_for_caching) {
            persist_creds = true;
        }
    }
    NAN_DATA_UNLOCK();

    /* Persist outside the lock; NVS writes can block. */
    if (persist_creds) {
        esp_wifi_nan_save_creds_for_peer(nik, persist_npk, own ? own->svc_hash : NULL);
    }

    /* Invoke blocking calls outside NAN_DATA_LOCK to avoid deadlock. */
    if (pairing_completed) {
        wifi_event_nan_pairing_complete_t evt = {0};

        if (own) {
            nan_pairing_cancel_svc_pending(own);
            esp_nan_set_pairing_status(own->svc_id, peer_svc_id, (uint8_t *)peer_mac, true);
        }
        evt.status = WIFI_NAN_PAIRING_STATUS_ACCEPTED;
        evt.reason_code = 0;
        MACADDR_COPY(evt.peer_nmi, peer_mac);
        nan_app_post_event(WIFI_EVENT_NAN_PAIRING_CONFIRM, &evt, sizeof(evt));
    }

    /* Only reply with our own NIK the first time we receive the peer's.
     * If has_nik was already true this is a redundant echo — don't reply
     * or we create an infinite ping-pong of follow-ups. */
    if (already_had_nik) {
        ESP_LOGD(TAG, "Pairing follow-up: NIK already known for " MACSTR ", skipping reply",
                 MAC2STR(peer_mac));
        return;
    }

    if (total_len > SIZE_MAX - sizeof(*ctx)) {
        return;
    }
    alloc_len = sizeof(*ctx) + total_len;
    ctx = os_zalloc(alloc_len);
    if (!ctx) {
        return;
    }
    ctx->svc_id = svc_id;
    ctx->peer_svc_id = peer_svc_id;
    MACADDR_COPY(ctx->peer_mac, peer_mac);
    ctx->shared_key_attr_len = total_len;
    memcpy(ctx->shared_key_attr, shared_key_attr, total_len);

    if (eloop_register_timeout(0, 0, nan_app_send_pairing_followup_eloop, NULL, ctx) != 0) {
        os_free(ctx);
        ESP_LOGW(TAG, "Pairing follow-up: failed to schedule response");
    }
}

/* Core NIRA verification. When @a own_inst_id is non-NULL it also resolves the
 * matched identity to a live own service instance id (0 if the verifying NIK
 * does not map to an active local service), used to anchor a verify-session
 * flag. The bool result reflects identity match only. */
static bool nan_verify_nira_internal(uint8_t *peer_mac, uint8_t *nira_attr,
                                     uint16_t nira_attr_len, uint8_t *own_inst_id)
{
    uint8_t expected_tag[NAN_NIRA_TAG_LEN];
    const uint8_t *nonce;
    const uint8_t *received_tag;
    bool match = false;

    if (own_inst_id) {
        *own_inst_id = 0;
    }

    if (!peer_mac || !nira_attr) {
        return false;
    }

    if (nira_attr_len < NAN_NIRA_ATTR_LEN) {
        ESP_LOGW(TAG, "NIRA verify: attribute too short (%u < %u)",
                 (unsigned)nira_attr_len, (unsigned)NAN_NIRA_ATTR_LEN);
        return false;
    }

    nonce        = nira_attr + 4;
    received_tag = nira_attr + 4 + NAN_NIRA_NONCE_LEN;

    /* The peer derives its NIRA tag from one of its NIKs; the sending MAC may be
     * randomised, so resolve the identity by trying every cached peer NIK. */
    NAN_DATA_LOCK();
    for (uint8_t i = 0; i < s_nan_ctx.num_peer_creds; i++) {
        if (!s_nan_ctx.peer_creds[i].is_valid) {
            continue;
        }
        if (nan_pairing_derive_nira_tag(s_nan_ctx.peer_creds[i].peer_nik, peer_mac,
                                        nonce, expected_tag) != 0) {
            continue;
        }
        if (os_memcmp_const(expected_tag, received_tag, NAN_NIRA_TAG_LEN) == 0) {
            match = true;
            if (own_inst_id) {
                for (int idx = 0; idx < ESP_WIFI_NAN_MAX_SVC_SUPPORTED; idx++) {
                    struct own_svc_info *own = &s_nan_ctx.own_svc[idx];

                    if (own->svc_id &&
                            memcmp(own->svc_hash,
                                   s_nan_ctx.peer_creds[i].service_hash, 6) == 0) {
                        *own_inst_id = own->svc_id;
                        break;
                    }
                }
            }
            break;
        }
    }
    NAN_DATA_UNLOCK();

    if (match) {
        ESP_LOGD(TAG, "NIRA verify: OK for "MACSTR, MAC2STR(peer_mac));
    } else {
        ESP_LOGW(TAG, "NIRA verify: no matching NIK for "MACSTR, MAC2STR(peer_mac));
    }
    return match;
}

bool esp_nan_verify_nira(uint8_t *peer_mac, uint8_t *nira_attr, uint16_t nira_attr_len)
{
    return nan_verify_nira_internal(peer_mac, nira_attr, nira_attr_len, NULL);
}

bool esp_nan_verify_nira_get_own_svc(uint8_t *peer_mac, uint8_t *nira_attr,
                                     uint16_t nira_attr_len, uint8_t *own_inst_id)
{
    return nan_verify_nira_internal(peer_mac, nira_attr, nira_attr_len, own_inst_id);
}

#if defined(CONFIG_ESP_WIFI_PASN_SUPPORT)
esp_nan_pairing_key_installed_cb_t esp_nan_pairing_get_key_installed_cb(void)
{
    return nan_pairing_key_installed_cb;
}
#endif

#endif /* CONFIG_ESP_WIFI_NAN_PAIRING */
