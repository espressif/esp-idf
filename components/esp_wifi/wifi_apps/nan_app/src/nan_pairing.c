/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
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
#include "os.h"
#include "utils/common.h"

#if defined(CONFIG_ESP_WIFI_PASN_SUPPORT)
#include "esp_private/esp_supp_nan.h"
#include "apps_private/wifi_apps_private.h"
#endif

static const char *TAG = "nan_pairing";

/* Default NIK / pairing-record lifetime (also reused for paired-peer cache
 * entries) — matches the NIK Key Lifetime KDE we transmit in the post-pairing
 * Shared Key Descriptor (Wi-Fi Aware v4.0 §7.6.4.2). */
#define NAN_PAIRING_DEFAULT_NIK_LIFETIME_SEC 86400U

#if defined(CONFIG_ESP_WIFI_PASN_SUPPORT)
struct nan_pasn_data *esp_nan_app_get_pasn_data(void)
{
    return s_nan_ctx.nan_pasn_data;
}

void esp_nan_app_set_pasn_data(struct nan_pasn_data *pd)
{
    s_nan_ctx.nan_pasn_data = pd;
}

static void nan_pairing_key_installed_cb(const uint8_t *peer_nmi,
                                         uint8_t role,
                                         uint8_t ndp_csid,
                                         const uint8_t *nd_pmk,
                                         size_t nd_pmk_len)
{
    wifi_event_nan_pairing_complete_t evt = {0};

    if (!peer_nmi) {
        return;
    }

#if defined(CONFIG_ESP_WIFI_NAN_SECURITY)
    /* Cache ND-PMK for future paired NDPs (Wi-Fi Aware v4.0 §7.6.4.2). The
     * NDP cipher (CSID) is determined by the PASN cipher and resolved on the
     * supplicant side before this callback fires; if either is missing, the
     * pairing event still fires but the security layer will fall back to its
     * service-credential path for any subsequent NDP. */
    if (ndp_csid && nd_pmk && nd_pmk_len == ESP_WIFI_NAN_NDP_PMK_LEN) {
        (void)nan_app_register_paired_peer(peer_nmi, role, ndp_csid,
                                           nd_pmk, nd_pmk_len,
                                           NAN_PAIRING_DEFAULT_NIK_LIFETIME_SEC);
    } else {
        ESP_LOGW(TAG, "Pairing complete for " MACSTR
                 ": ND-PMK unavailable (csid=%u nd_pmk_len=%u); "
                 "paired-peer cache not updated",
                 MAC2STR(peer_nmi), ndp_csid, (unsigned)nd_pmk_len);
    }
#else
    (void)role;
    (void)ndp_csid;
    (void)nd_pmk;
    (void)nd_pmk_len;
#endif

    evt.status = WIFI_NAN_PAIRING_STATUS_ACCEPTED;
    evt.reason_code = 0;
    MACADDR_COPY(evt.peer_nmi, peer_nmi);
    nan_app_post_event(WIFI_EVENT_NAN_PAIRING_CONFIRM, &evt, sizeof(evt));
}
#endif

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
    ESP_LOGI(TAG, "Pairing Bootstrapping Request from "MACSTR" [pub_id=%d, method=0x%x]",
             MAC2STR(peer_nmi), pub_id, selected_method);

    wifi_event_nan_bootstrap_indication_t evt = {0};
    evt.peer_svc_id = peer_svc_id;
    evt.own_svc_id = pub_id;
    MACADDR_COPY(evt.peer_nmi, peer_nmi);
    evt.selected_method = selected_method;

    nan_app_post_event(WIFI_EVENT_NAN_BOOTSTRAP_INDICATION, &evt, sizeof(evt));
}

void nan_app_bootstrap_completed(uint8_t status, uint8_t peer_svc_id, uint8_t sub_id,
                                 uint8_t peer_nmi[6], uint16_t matched_method,
                                 uint8_t reason_code)
{
    ESP_LOGI(TAG, "Pairing Bootstrapping Response from "MACSTR" [sub_id=%d, status=%d, method=0x%x]",
             MAC2STR(peer_nmi), sub_id, status, matched_method);

    wifi_event_nan_bootstrap_complete_t evt = {0};
    evt.status = status;
    evt.peer_svc_id = peer_svc_id;
    evt.own_svc_id = sub_id;
    MACADDR_COPY(evt.peer_nmi, peer_nmi);
    evt.matched_method = matched_method;
    evt.reason_code = reason_code;

    nan_app_post_event(WIFI_EVENT_NAN_BOOTSTRAP_COMPLETED, &evt, sizeof(evt));
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

esp_err_t esp_wifi_nan_pairing_start(wifi_nan_pairing_config_t *cfg)
{
    if (!cfg) {
        ESP_LOGE(TAG, "Pairing config NULL");
        return ESP_ERR_INVALID_ARG;
    }

#if defined(CONFIG_ESP_WIFI_PASN_SUPPORT)
    int ret;

    switch (cfg->self_role) {
    case NAN_PAIRING_ROLE_RESPONDER:
        ret = esp_nan_supp_pasn_responder_init(cfg->peer_nmi, cfg->cred.pincode,
                                               nan_pairing_key_installed_cb);
        if (ret != 0) {
            ESP_LOGE(TAG, "NAN PASN responder init failed for "MACSTR, MAC2STR(cfg->peer_nmi));
            return ESP_FAIL;
        }
        break;
    case NAN_PAIRING_ROLE_INITIATOR:
        ret = esp_nan_supp_pasn_initiator_auth(cfg->peer_nmi, cfg->cred.pincode,
                                               nan_pairing_key_installed_cb);
        if (ret != 0) {
            ESP_LOGE(TAG, "NAN PASN initiator auth failed for "MACSTR, MAC2STR(cfg->peer_nmi));
            return ESP_FAIL;
        }
        break;
    default:
        ESP_LOGE(TAG, "Invalid pairing role %d", cfg->self_role);
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
#else
    ESP_LOGE(TAG, "NAN PASN support not enabled");
    return ESP_ERR_NOT_SUPPORTED;
#endif
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
    uint8_t nonce[NAN_NIRA_NONCE_LEN];
    uint8_t tag[NAN_NIRA_TAG_LEN];

    if (!frm) {
        return 0;
    }

    if (os_get_random(nonce, sizeof(nonce)) != 0) {
        ESP_LOGE(TAG, "NIRA: failed to generate nonce");
        return 0;
    }

#ifdef CONFIG_ESP_WIFI_NAN_SECURITY
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

    /* Tag = Truncate-64(HMAC-SHA-256(NIK, "NIR" || NMI || Nonce)) */
    addr[0] = (const unsigned char *)NAN_NIRA_STR;
    len_arr[0] = NAN_NIRA_STR_LEN;
    addr[1] = own_nmi;
    len_arr[1] = MACADDR_LEN;
    addr[2] = nonce;
    len_arr[2] = NAN_NIRA_NONCE_LEN;
    if (g_wifi_default_wpa_crypto_funcs.hmac_sha256_vector(s_nan_ctx.own_nik,
                                                           ESP_WIFI_NAN_NIK_LEN,
                                                           3, addr, len_arr,
                                                           digest) != 0) {
        ESP_LOGE(TAG, "NIRA: tag derivation failed");
        return 0;
    }
    memcpy(tag, digest, NAN_NIRA_TAG_LEN);
    memset(digest, 0, sizeof(digest));
#else
    /* NIRA requires an available NIK; skip when NAN security is not enabled. */
    return 0;
#endif

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

#if defined(CONFIG_ESP_WIFI_NAN_PAIRING) && defined(CONFIG_ESP_WIFI_PASN_SUPPORT) && defined(CONFIG_ESP_WIFI_NAN_SECURITY)

#include "crypto/sha256.h"
#include "utils/eloop.h"
#include "crypto/aes_wrap.h"
#include "common/ieee802_11_defs.h"
#include "common/wpa_common.h"

#define NAN_PAIRING_FUP_MIN_ATTR_LEN 3
#define NAN_PASN_KDE_OUI_TYPE_NIK       36
#define NAN_PASN_KDE_OUI_TYPE_LIFETIME  37
#define NAN_PASN_KEY_LIFETIME_NIK_BIT   BIT(3)
/* NAN_PAIRING_DEFAULT_NIK_LIFETIME_SEC is defined at file scope above. */
#define NAN_ATTR_ID_SHARED_KEY_DESC     0x24
#define GSP_SUBATTR_TRANSPORT_PORT      0x00
#define GSP_SUBATTR_INSTANCE_NAME       0x03
#define NAN_PAIRING_SRV_PORT            3333
#define NAN_PAIRING_SRV_HOSTNAME        "ESP-SRV-1234"
#define NAN_PAIRING_SSI_BUF_LEN         64

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
 * Append a single GSP sub-attribute (TLV: ID(1) | Length(2 LE) | Value).
 * Ported from esp-nsd @c add_gsp_subattr (wa_sd.c).
 */
static int nan_pairing_add_gsp_subattr(uint8_t *buf, uint8_t sub_attr_id,
                                       const void *payload, uint16_t len)
{
    uint8_t *p = buf;

    *p++ = sub_attr_id;
    *((uint16_t *)p) = len;
    p += sizeof(uint16_t);
    memcpy(p, payload, len);

    return 1 + 2 + len;
}

/**
 * Build a GSP SSI carrying the SRV record (Transport Port + Instance Name).
 * Ported from esp-nsd @c get_ssi_for_srv_record (wa_sd.c) with a stack
 * buffer and a fixed (hostname, port) instead of dynamic allocation.
 */
static size_t nan_pairing_build_srv_ssi(uint8_t *buf, size_t buf_len,
                                        const char *hostname, uint16_t port)
{
    uint8_t *p = buf;
    size_t hostname_len;
    size_t need;

    if (!buf || !hostname) {
        return 0;
    }
    hostname_len = strlen(hostname);
    need = sizeof(wifi_nan_wfa_ssi_t)
           + (1 + 2 + sizeof(port))
           + (1 + 2 + hostname_len);
    if (buf_len < need) {
        return 0;
    }

    p += nan_pairing_add_ssi_gsp_attr(p);
    p += nan_pairing_add_gsp_subattr(p, GSP_SUBATTR_TRANSPORT_PORT,
                                     &port, sizeof(port));
    p += nan_pairing_add_gsp_subattr(p, GSP_SUBATTR_INSTANCE_NAME,
                                     hostname, hostname_len);

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

/**
 * Build a NIRA attribute (ID 0x2B) into @a buf using the NIK and our NMI.
 * Returns total attribute length on success, 0 on failure.
 */
static size_t nan_pairing_build_nira_attr(uint8_t *buf, size_t buf_len,
                                          const uint8_t nik[NAN_PASN_NIK_LEN],
                                          const uint8_t nmi_addr[ETH_ALEN])
{
    uint8_t nonce[NAN_NIRA_NONCE_LEN];
    uint8_t tag[NAN_NIRA_TAG_LEN];
    uint8_t *p = buf;

    if (!buf || !nik || !nmi_addr || buf_len < NAN_NIRA_ATTR_LEN) {
        return 0;
    }
    if (os_get_random(nonce, sizeof(nonce)) != 0) {
        return 0;
    }
    if (nan_pairing_derive_nira_tag(nik, nmi_addr, nonce, tag) != 0) {
        return 0;
    }

    *p++ = NAN_ATTR_ID_IDENTITY_RESOLUTION;
    /* Attribute Length (LE16): CipherVersion(1) + Nonce(8) + Tag(8) = 17 */
    WPA_PUT_LE16(p, 1 + NAN_NIRA_NONCE_LEN + NAN_NIRA_TAG_LEN);
    p += 2;
    *p++ = NAN_NIRA_CIPHER_VER;
    memcpy(p, nonce, NAN_NIRA_NONCE_LEN);
    p += NAN_NIRA_NONCE_LEN;
    memcpy(p, tag, NAN_NIRA_TAG_LEN);
    p += NAN_NIRA_TAG_LEN;

    return (size_t)(p - buf);
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
    WPA_PUT_BE32(&buf[pos], NAN_PAIRING_DEFAULT_NIK_LIFETIME_SEC);
    pos += 4;

    /*
     * AES-WRAP requires plaintext length to be a multiple of 8. IEEE
     * 802.11-2020 §12.7.2 specifies Key Data padding as a single 0xDD byte
     * followed by zeros (not a sequence of well-formed vendor IEs).
     */
    {
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
    uint8_t srv_ssi[NAN_PAIRING_SSI_BUF_LEN] = {0};
    uint8_t our_nmi[ETH_ALEN] = {0};
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

    if (!peer_mac || os_get_random(nik, sizeof(nik)) != 0) {
        return ESP_ERR_INVALID_ARG;
    }

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
    if (aes_wrap(saved->kek, saved->kek_len, plain_len / 8, plain, wrapped) != 0) {
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

    /* SSI: GSP (WFA OUI + proto=Generic) carrying an SRV record so iPhone
     * has Transport Port + Instance Name to associate with this pairing. */
    srv_ssi_len = nan_pairing_build_srv_ssi(srv_ssi, sizeof(srv_ssi),
                                            NAN_PAIRING_SRV_HOSTNAME,
                                            NAN_PAIRING_SRV_PORT);
    if (srv_ssi_len == 0) {
        ESP_LOGW(TAG, "Pairing follow-up: failed to build SRV SSI");
        return ESP_FAIL;
    }

    fup.inst_id = svc_id;
    fup.peer_inst_id = peer_svc_id;
    MACADDR_COPY(fup.peer_mac, peer_mac);
    fup.ssi = srv_ssi;
    fup.ssi_len = (uint16_t)srv_ssi_len;

    sk_attr_len = (size_t)(w - shared_key_wrapped);
    params_i.pairing_npba = NULL;
    params_i.shared_key_wrapped = shared_key_wrapped;
    params_i.shared_key_wrapped_len = (uint16_t)sk_attr_len;

    /* NIRA proves possession of the NIK we just wrapped above; iPhone uses
     * it to bind the NIK to the sender and won't commit the pairing record
     * without it. */
    if (esp_wifi_get_mac(WIFI_IF_NAN, our_nmi) != ESP_OK) {
        ESP_LOGW(TAG, "Pairing follow-up: cannot read NAN NMI for NIRA");
        return ESP_FAIL;
    }
    nira_len = nan_pairing_build_nira_attr(nira_attr, sizeof(nira_attr),
                                           nik, our_nmi);
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

    const uint16_t *attr_body_len_field = (const uint16_t *)&shared_key_attr[1];

    attr_body_len = *attr_body_len_field;
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

    NAN_DATA_LOCK();
    struct peer_svc_info *p_peer_svc = nan_find_peer_svc_exact(svc_id, peer_svc_id, peer_mac);
    if (p_peer_svc) {
        memcpy(p_peer_svc->peer_nik, nik, NAN_APP_PEER_NIK_LEN);
        p_peer_svc->peer_nik_cipher_ver = cipher_ver;
        p_peer_svc->peer_nik_lifetime_sec = lifetime_sec;
        p_peer_svc->has_nik = true;
        ESP_LOGI(TAG, "Stored peer NIK from " MACSTR " (cipher_ver=%u, lifetime=%u s)",
                 MAC2STR(peer_mac), cipher_ver, lifetime_sec);
    }
    NAN_DATA_UNLOCK();

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

#endif /* CONFIG_ESP_WIFI_NAN_PAIRING && CONFIG_ESP_WIFI_PASN_SUPPORT && CONFIG_ESP_WIFI_NAN_SECURITY */

#endif /* CONFIG_ESP_WIFI_NAN_PAIRING */
