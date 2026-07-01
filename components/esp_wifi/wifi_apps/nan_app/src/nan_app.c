/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ctype.h>
#include <stdio.h>
#include "esp_wifi.h"
#include "esp_private/wifi.h"
#include "esp_wifi_netif.h"
#include "freertos/event_groups.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_mac.h"
#include "esp_timer.h"
#include "os.h"
#include "esp_nan.h"
#include "utils/common.h"
#include "nan_i.h"
#ifdef CONFIG_ESP_WIFI_NAN_USD_ENABLE
#include "esp_private/esp_nan_usd.h"
#endif /* CONFIG_ESP_WIFI_NAN_USD_ENABLE */

#if !CONFIG_ESP_WIFI_NAN_PAIRING
uint32_t esp_nan_get_nira_len(void)
{
    return 0;
}

int esp_nan_construct_nira(uint8_t *frm)
{
    (void)frm;
    return 0;
}

bool esp_nan_verify_nira(uint8_t *peer_mac, uint8_t *nira_attr, uint16_t nira_attr_len)
{
    (void)peer_mac;
    (void)nira_attr;
    (void)nira_attr_len;
    return false;
}

bool esp_nan_verify_nira_get_own_svc(uint8_t *peer_mac, uint8_t *nira_attr,
                                     uint16_t nira_attr_len, uint8_t *own_inst_id)
{
    (void)peer_mac;
    (void)nira_attr;
    (void)nira_attr_len;
    if (own_inst_id) {
        *own_inst_id = 0;
    }
    return false;
}
#endif

#if defined(CONFIG_ESP_WIFI_NAN_SYNC_ENABLE) && defined(CONFIG_ESP_WIFI_PASN_SUPPORT)
#include "esp_private/esp_supp_nan.h"
#include "apps_private/wifi_apps_private.h"
#endif

/* NAN States */
#define NAN_STARTED_BIT     BIT0
#define NAN_STOPPED_BIT     BIT1

/* NAN Events (NAN_TX_SUCCESS / NAN_TX_FAILURE in nan_i.h) */
#define NDP_INDICATION      BIT4
#define NDP_ACCEPTED        BIT5
#define NDP_TERMINATED      BIT6
#define NDP_REJECTED        BIT7

/* NAN Caps */
#define NAN_CAPS_NDPE_ATTR  BIT3

/* Macros */
#define MACADDR_LEN             6
#define MACADDR_EQUAL(a1, a2)   (memcmp(a1, a2, MACADDR_LEN) == 0)
#define MACADDR_COPY(dst, src)  (memcpy(dst, src, MACADDR_LEN))

/* Global Variables */
static const char *TAG = "nan_app";
#ifdef CONFIG_ESP_WIFI_NAN_SYNC_ENABLE
static EventGroupHandle_t nan_event_group;
static bool s_app_default_handlers_set = false;
static uint8_t null_mac[MACADDR_LEN] = {0};
void *s_nan_data_lock = NULL; /* extern in nan_i.h */
static uint32_t s_fup_context;
#endif /* CONFIG_ESP_WIFI_NAN_SYNC_ENABLE */
#ifdef CONFIG_ESP_WIFI_NAN_USD_ENABLE
#endif /* CONFIG_ESP_WIFI_NAN_USD_ENABLE */
static bool s_usd_in_progress = false;
static const uint8_t s_wfa_oui[3] = {0x50, 0x6f, 0x9a};

/* Definitions */
#define NAN_SDEA_CTRL_FSD_REQD      BIT(0)
#define NAN_SDEA_CTRL_FSD_GAS       BIT(1)
#define NAN_SDEA_CTRL_DATAPATH_REQD BIT(2)
#define NAN_SDEA_CTRL_SECURITY_REQD BIT(6)

#ifdef CONFIG_ESP_WIFI_NAN_SYNC_ENABLE

/* Definition of nan_ctx_t storage shared via nan_i.h. */
nan_ctx_t s_nan_ctx;

#ifdef CONFIG_ESP_WIFI_NAN_PAIRING
EventGroupHandle_t nan_pairing_get_event_group(void)
{
    return nan_event_group;
}

uint32_t *nan_pairing_get_fup_context(void)
{
    return &s_fup_context;
}

const uint8_t *nan_pairing_get_null_mac(void)
{
    return null_mac;
}

#ifdef CONFIG_ESP_WIFI_NAN_SECURITY
/* ---- Paired-peer cache ---------------------------------------------------
 *
 * Tracks ND-PMK + cipher per peer NMI after PASN/Pairing completes. The
 * security layer (nan_security.c) consumes these on inbound SDF match and on
 * NDP request/response to source ND-PMK without an app-provided service
 * credential. Lifetime accounting is timestamp-based; auto-purge is left to
 * the consumer (current path: explicit remove or stop).
 */

static struct nan_paired_peer *nan_app_find_paired_slot_locked(const uint8_t *peer_nmi)
{
    for (int i = 0; i < ESP_WIFI_NAN_DATAPATH_MAX_PEERS; i++) {
        struct nan_paired_peer *p = &s_nan_ctx.paired_peers[i];
        if (p->valid && memcmp(p->peer_nmi, peer_nmi, MACADDR_LEN) == 0) {
            return p;
        }
    }
    return NULL;
}

static struct nan_paired_peer *nan_app_alloc_paired_slot_locked(const uint8_t *peer_nmi)
{
    struct nan_paired_peer *p = nan_app_find_paired_slot_locked(peer_nmi);
    if (p) {
        return p;
    }
    for (int i = 0; i < ESP_WIFI_NAN_DATAPATH_MAX_PEERS; i++) {
        if (!s_nan_ctx.paired_peers[i].valid) {
            return &s_nan_ctx.paired_peers[i];
        }
    }
    /* Evict oldest entry by established_us. */
    int oldest = 0;
    int64_t oldest_ts = s_nan_ctx.paired_peers[0].established_us;
    for (int i = 1; i < ESP_WIFI_NAN_DATAPATH_MAX_PEERS; i++) {
        if (s_nan_ctx.paired_peers[i].established_us < oldest_ts) {
            oldest_ts = s_nan_ctx.paired_peers[i].established_us;
            oldest = i;
        }
    }
    return &s_nan_ctx.paired_peers[oldest];
}

esp_err_t nan_app_register_paired_peer(const uint8_t *peer_nmi,
                                       uint8_t role,
                                       uint8_t ndp_csid,
                                       const uint8_t *nd_pmk,
                                       size_t nd_pmk_len,
                                       uint32_t lifetime_sec)
{
    if (!peer_nmi || !nd_pmk || nd_pmk_len != ESP_WIFI_NAN_NDP_PMK_LEN) {
        return ESP_ERR_INVALID_ARG;
    }
    if (ndp_csid != WIFI_NAN_CSID_NCS_SK_128 && ndp_csid != WIFI_NAN_CSID_NCS_SK_256) {
        return ESP_ERR_INVALID_ARG;
    }

    NAN_DATA_LOCK();
    struct nan_paired_peer *p = nan_app_alloc_paired_slot_locked(peer_nmi);
    forced_memzero(p->nd_pmk, sizeof(p->nd_pmk));
    memset(p, 0, sizeof(*p));
    p->valid = true;
    MACADDR_COPY(p->peer_nmi, peer_nmi);
    p->role = role;
    p->ndp_csid = ndp_csid;
    memcpy(p->nd_pmk, nd_pmk, nd_pmk_len);
    p->lifetime_sec = lifetime_sec;
    p->established_us = esp_timer_get_time();
    NAN_DATA_UNLOCK();

    ESP_LOGI(TAG, "Paired peer cached: " MACSTR " role=%u csid=%u lifetime=%us",
             MAC2STR(peer_nmi), role, ndp_csid, lifetime_sec);
    return ESP_OK;
}

const struct nan_paired_peer *nan_app_find_paired_peer(const uint8_t *peer_nmi)
{
    if (!peer_nmi) {
        return NULL;
    }
    /* Caller holds NAN_DATA_LOCK (see nan_i.h). */
    return nan_app_find_paired_slot_locked(peer_nmi);
}

void nan_app_remove_paired_peer(const uint8_t *peer_nmi)
{
    if (!peer_nmi) {
        return;
    }
    NAN_DATA_LOCK();
    struct nan_paired_peer *p = nan_app_find_paired_slot_locked(peer_nmi);
    if (p) {
        forced_memzero(p->nd_pmk, sizeof(p->nd_pmk));
        memset(p, 0, sizeof(*p));
    }
    NAN_DATA_UNLOCK();
}

void nan_app_clear_paired_peers(void)
{
    NAN_DATA_LOCK();
    for (int i = 0; i < ESP_WIFI_NAN_DATAPATH_MAX_PEERS; i++) {
        forced_memzero(s_nan_ctx.paired_peers[i].nd_pmk,
                       sizeof(s_nan_ctx.paired_peers[i].nd_pmk));
    }
    memset(s_nan_ctx.paired_peers, 0, sizeof(s_nan_ctx.paired_peers));
    NAN_DATA_UNLOCK();
}
#endif /* CONFIG_ESP_WIFI_NAN_SECURITY */

static void nan_app_clear_one_peer_tks(const uint8_t *peer_nmi)
{
    uint8_t key_rsc[8] = {0};
    static const uint8_t zero_mac[6] = {0};

    if (!peer_nmi || memcmp(peer_nmi, zero_mac, 6) == 0) {
        return;
    }

    NAN_DATA_LOCK();

    /* NM-TK is bound to peer NMI (see nan_pasn_install_nan_pairwise_tk). */
    esp_wifi_set_nan_key_internal(NAN_WIFI_WPA_ALG_CCMP,
                                  (uint8_t *)peer_nmi, 1, 1,
                                  key_rsc, sizeof(key_rsc),
                                  NULL, 0, NAN_KEY_NM_TK);

#ifdef CONFIG_ESP_WIFI_NAN_SECURITY
    struct ndl_info *ndl = nan_find_ndl(0, (uint8_t *)peer_nmi);
    uint8_t *key_addr = (uint8_t *)peer_nmi;

    if (ndl) {
        if (memcmp(ndl->peer_ndi, zero_mac, 6) != 0) {
            key_addr = ndl->peer_ndi;
        }

        esp_wifi_set_nan_key_internal(NAN_WIFI_WPA_ALG_CCMP,
                                      key_addr, 0, 1,
                                      key_rsc, sizeof(key_rsc),
                                      NULL, 0, NAN_KEY_ND_TK);

        /* Drop the peer's RX GTK (bound to the peer NDI) and wipe local GTK
         * state. The TX GTK keyed on the local NDI is released by the blob
         * when the NDI/interface is torn down. */
        esp_wifi_set_nan_key_internal(NAN_WIFI_WPA_ALG_CCMP,
                                      key_addr, ndl->gtk_keyid, 0,
                                      key_rsc, sizeof(key_rsc),
                                      NULL, 0, NAN_KEY_ND_GTK);

        /* Drop the peer's RX IGTK/BIGTK (BIP-CMAC-128, installed against the
         * peer NMI at NDP confirm) so no stale BIP keys linger in the blob. */
        if (ndl->igtk_set) {
            esp_wifi_set_nan_key_internal(NAN_WIFI_WPA_ALG_BIP_CMAC_128,
                                          (uint8_t *)peer_nmi, ndl->igtk_keyid, 0,
                                          key_rsc, 6,
                                          NULL, 0, NAN_KEY_ND_IGTK);
        }
        if (ndl->bigtk_set) {
            esp_wifi_set_nan_key_internal(NAN_WIFI_WPA_ALG_BIP_CMAC_128,
                                          (uint8_t *)peer_nmi, ndl->bigtk_keyid, 0,
                                          key_rsc, 6,
                                          NULL, 0, NAN_KEY_ND_BIGTK);
        }

        forced_memzero(ndl->nd_tk, sizeof(ndl->nd_tk));
        forced_memzero(ndl->nd_kck, sizeof(ndl->nd_kck));
        forced_memzero(ndl->nd_kek, sizeof(ndl->nd_kek));
        forced_memzero(ndl->gtk, sizeof(ndl->gtk));
        forced_memzero(ndl->own_gtk, sizeof(ndl->own_gtk));
        forced_memzero(ndl->igtk, sizeof(ndl->igtk));
        forced_memzero(ndl->bigtk, sizeof(ndl->bigtk));
        ndl->ptk_set = 0;
        ndl->tk_len = 0;
        ndl->kck_len = 0;
        ndl->kek_len = 0;
        ndl->gtk_set = 0;
        ndl->own_gtk_set = 0;
        ndl->gtk_len = 0;
        ndl->own_gtk_len = 0;
        ndl->igtk_set = 0;
        ndl->bigtk_set = 0;
        ndl->igtk_len = 0;
        ndl->bigtk_len = 0;
    }

    /* Fallback when no NDL slot tracks peer_ndi yet. */
    esp_wifi_set_nan_key_internal(NAN_WIFI_WPA_ALG_CCMP,
                                  (uint8_t *)peer_nmi, 0, 1,
                                  key_rsc, sizeof(key_rsc),
                                  NULL, 0, NAN_KEY_ND_TK);
#endif /* CONFIG_ESP_WIFI_NAN_SECURITY */

    NAN_DATA_UNLOCK();
}

void esp_nan_app_clear_peer_tks(const uint8_t *peer_nmi, uint8_t service_id)
{
    if (peer_nmi) {
        nan_app_clear_one_peer_tks(peer_nmi);
        return;
    }

    if (service_id == 0) {
        return;
    }

#ifdef CONFIG_ESP_WIFI_NAN_SYNC_ENABLE
    uint8_t peer_nmis[NAN_MAX_PEERS_RECORD][MACADDR_LEN];
    int peer_count = 0;
    struct own_svc_info *p_own_svc;
    struct peer_svc_info *temp;

    NAN_DATA_LOCK();
    p_own_svc = nan_find_own_svc(service_id);
    if (!p_own_svc) {
        NAN_DATA_UNLOCK();
        return;
    }
    SLIST_FOREACH(temp, &(p_own_svc->peer_list), next) {
        bool dup = false;

        for (int i = 0; i < peer_count; i++) {
            if (MACADDR_EQUAL(peer_nmis[i], temp->peer_nmi)) {
                dup = true;
                break;
            }
        }
        if (!dup && peer_count < NAN_MAX_PEERS_RECORD) {
            MACADDR_COPY(peer_nmis[peer_count], temp->peer_nmi);
            peer_count++;
        }
    }
    NAN_DATA_UNLOCK();

    for (int i = 0; i < peer_count; i++) {
        nan_app_clear_one_peer_tks(peer_nmis[i]);
    }
#else
    (void)service_id;
#endif /* CONFIG_ESP_WIFI_NAN_SYNC_ENABLE */
}

#endif /* CONFIG_ESP_WIFI_NAN_PAIRING */

void esp_wifi_nan_get_ipv6_linklocal_from_mac(ip6_addr_t *ip6, uint8_t *mac_addr)
{
    if (ip6 == NULL || mac_addr == NULL) {
        return;
    }
    /* Reuse the interface-agnostic derivation in the esp_wifi netif layer, then
     * copy the address words into the lwIP ip6_addr_t. The two structures share
     * the same layout, which is how esp_netif converts between them. */
    esp_ip6_addr_t esp_ip6;
    esp_wifi_netif_get_ip6_linklocal_from_mac(&esp_ip6, mac_addr);
    memcpy(ip6->addr, esp_ip6.addr, sizeof(ip6->addr));
    ip6->zone = IP6_NO_ZONE;
}

struct own_svc_info *nan_find_own_svc(uint8_t svc_id)
{
    struct own_svc_info *p_svc = NULL;

    if (svc_id == 0) {
        ESP_LOGE(TAG, "Service id cannot be 0!");
        return NULL;
    }

    for (int i = 0; i < ESP_WIFI_NAN_MAX_SVC_SUPPORTED; i++) {
        if (s_nan_ctx.own_svc[i].svc_id == svc_id) {
            p_svc = &s_nan_ctx.own_svc[i];
            break;
        }
    }

    return p_svc;
}

struct own_svc_info *nan_find_own_svc_by_name(const char *svc_name)
{
    struct own_svc_info *p_svc = NULL;

    if (!svc_name) {
        ESP_LOGE(TAG, "Service name not given!");
        return NULL;
    }

    for (int i = 0; i < ESP_WIFI_NAN_MAX_SVC_SUPPORTED; i++) {
        if (s_nan_ctx.own_svc[i].svc_id && !strcmp(s_nan_ctx.own_svc[i].svc_name, svc_name)) {
            p_svc = &s_nan_ctx.own_svc[i];
            break;
        }
    }

    return p_svc;
}

struct peer_svc_info *nan_find_peer_svc(uint8_t own_svc_id, uint8_t peer_svc_id, uint8_t peer_nmi[])
{
    struct peer_svc_info *p_peer_svc = NULL, *temp;
    struct own_svc_info *p_own_svc = NULL;
    uint8_t *peer_nmi_valid = NULL;
    int idx = 0;

    if (!MACADDR_EQUAL(peer_nmi, null_mac)) {
        /* non-zero Peer NMI given, use it */
        peer_nmi_valid = peer_nmi;
    }
    while (idx < ESP_WIFI_NAN_MAX_SVC_SUPPORTED) {
        if (own_svc_id) {
            p_own_svc = nan_find_own_svc(own_svc_id);
            if (!p_own_svc) {
                ESP_LOGE(TAG, "Cannot find own service with id %d!", own_svc_id);
                return NULL;
            }
        } else {
            p_own_svc = &s_nan_ctx.own_svc[idx++];
        }
        SLIST_FOREACH(temp, &(p_own_svc->peer_list), next) {
            if (peer_svc_id != 0 && peer_nmi_valid) {
                if (temp->svc_id == peer_svc_id && MACADDR_EQUAL(temp->peer_nmi, peer_nmi_valid)) {
                    p_peer_svc = temp;
                    break;
                }
            } else if (peer_svc_id != 0) {
                if (temp->svc_id == peer_svc_id) {
                    p_peer_svc = temp;
                    break;
                }
            } else {
                if (peer_nmi_valid && MACADDR_EQUAL(temp->peer_nmi, peer_nmi_valid)) {
                    p_peer_svc = temp;
                    break;
                }
            }
        }
        if (p_peer_svc || own_svc_id) { /* If no peer found with given own_svc_id, don't search in other services */
            break;
        }
    }
    return p_peer_svc;
}

static bool nan_record_peer_svc(uint8_t own_svc_id, uint8_t peer_svc_id, uint8_t peer_nmi[], uint32_t device_caps)
{
    struct own_svc_info *p_own_svc;
    struct peer_svc_info *p_peer_svc;

    p_own_svc = nan_find_own_svc(own_svc_id);
    if (!p_own_svc) {
        ESP_LOGE(TAG, "Unable to find own service with id %d", own_svc_id);
        return false;
    }
    p_peer_svc = (struct peer_svc_info *)os_zalloc(sizeof(struct peer_svc_info));
    if (!p_peer_svc) {
        ESP_LOGE(TAG, "Unable to allocate for Peer Service");
        return false;
    }
    p_peer_svc->svc_id = peer_svc_id;
    p_peer_svc->own_svc_id = own_svc_id;
    p_peer_svc->type = (p_own_svc->type == ESP_NAN_SUBSCRIBE) ? ESP_NAN_PUBLISH : ESP_NAN_SUBSCRIBE;
    MACADDR_COPY(p_peer_svc->peer_nmi, peer_nmi);
    p_peer_svc->device_caps = device_caps;
#ifdef CONFIG_ESP_WIFI_NAN_SECURITY
    p_peer_svc->matched_cred_idx = 0xFF; /* NAN_NO_MATCHED_CRED — set by service_match if PMKID hits */
#endif
    if (p_own_svc->num_peer_records < NAN_MAX_PEERS_RECORD) {
        SLIST_INSERT_HEAD(&(p_own_svc->peer_list), p_peer_svc, next);
        p_own_svc->num_peer_records++;
    } else {
        /* Remove the oldest peer service entry */
        struct peer_svc_info *prev_ele = NULL, *cur_ele = NULL;

        SLIST_FOREACH(cur_ele, &(p_own_svc->peer_list), next) {
            if (SLIST_NEXT(cur_ele, next) == NULL) {
                if (SLIST_FIRST(&(p_own_svc->peer_list)) == cur_ele) {
                    SLIST_REMOVE_HEAD(&(p_own_svc->peer_list), next);
                } else {
                    SLIST_REMOVE_AFTER(prev_ele, next);
                }
                break;
            }
            prev_ele = cur_ele;
        }
        /* Insert new peer service entry */
        SLIST_INSERT_HEAD(&(p_own_svc->peer_list), p_peer_svc, next);
        os_free(cur_ele);
    }

    return true;
}

static void nan_reset_service(uint8_t svc_id, bool reset_all)
{
    struct own_svc_info *p_own_svc = NULL;
    struct peer_svc_info *p_peer_svc = NULL, *temp;
    int idx = 0;

    if (svc_id == 0 && !reset_all) {
        return;
    }

    while (idx < ESP_WIFI_NAN_MAX_SVC_SUPPORTED) {
        p_own_svc = &s_nan_ctx.own_svc[idx++];
        if (reset_all || (svc_id && p_own_svc->svc_id == svc_id)) {
#ifdef CONFIG_ESP_WIFI_NAN_PAIRING
            nan_pairing_cancel_svc_pending(p_own_svc);
#endif
            SLIST_FOREACH_SAFE(p_peer_svc, &(p_own_svc->peer_list), next, temp) {
                SLIST_REMOVE(&(p_own_svc->peer_list), p_peer_svc, peer_svc_info, next);
                os_free(p_peer_svc);
            }
            memset(p_own_svc, 0, sizeof(struct own_svc_info));
        }
    }
}

static void nan_reset_ndl(uint8_t ndp_id, bool reset_all)
{
    struct ndl_info *ndl = NULL;

    if (reset_all) {
        memset(s_nan_ctx.ndl, 0, sizeof(struct ndl_info) * ESP_WIFI_NAN_DATAPATH_MAX_PEERS);
        return;
    }
    for (int i = 0; i < ESP_WIFI_NAN_DATAPATH_MAX_PEERS; i++) {
        ndl = &s_nan_ctx.ndl[i];
        if (ndl->ndp_id == ndp_id) {
            memset(ndl, 0, sizeof(struct ndl_info));
            break;
        }
    }
}

static bool nan_services_limit_reached(void)
{
    for (int i = 0; i < ESP_WIFI_NAN_MAX_SVC_SUPPORTED; i++) {
        if (s_nan_ctx.own_svc[i].svc_id == 0) {
            return false;
        }
    }
    return true;
}

/*
 * Service ID = first 6 bytes of SHA256(lowercase(service_name))
 * per Wi-Fi Aware v4.0 §5.1.5 (Service Name and Service ID).
 */
bool nan_compute_service_id(const char *service_name, uint8_t service_id[6])
{
    if (!service_name || !g_wifi_default_wpa_crypto_funcs.sha256_vector) {
        return false;
    }
    size_t name_len = strlen(service_name);
    char *lower = os_malloc(name_len + 1);
    if (!lower) {
        return false;
    }
    strlcpy(lower, service_name, name_len + 1);
    for (char *p = lower; *p; p++) {
        *p = tolower((unsigned char) * p);
    }
    uint8_t hash[32];
    const uint8_t *addr[1] = {(const uint8_t *)lower};
    size_t len[1] = {name_len};
    int ret = g_wifi_default_wpa_crypto_funcs.sha256_vector(1, addr, len, hash);
    os_free(lower);
    if (ret != 0) {
        return false;
    }
    memcpy(service_id, hash, 6);
    return true;
}

/* Pre-claim a slot for an upcoming publish/subscribe service. The slot is
 * marked with a pending sentinel svc_id (0xFF) so nan_find_own_svc_by_name()
 * can find it from the derive callback running on the WiFi task — that's how
 * the host's per-credential derived_security[] mirror gets populated without
 * a duplicate PBKDF2 pass on the app/main task (which would block IDLE0 and
 * trip the watchdog). Real svc_id is stamped later by nan_finalize_own_svc(). */
#define NAN_SVC_ID_PENDING  0xFF

static struct own_svc_info *nan_claim_own_svc_slot(uint8_t type, const char svc_name[],
                                                   const wifi_nan_discovery_security_params_t *security_cfg, wifi_nan_pairing_cfg_t *pairing)
{
    struct own_svc_info *p_svc = NULL;
    for (int i = 0; i < ESP_WIFI_NAN_MAX_SVC_SUPPORTED; i++) {
        if (s_nan_ctx.own_svc[i].svc_id == 0) {
            p_svc = &s_nan_ctx.own_svc[i];
            break;
        }
    }
    if (!p_svc) {
        return NULL;
    }

    p_svc->svc_id = NAN_SVC_ID_PENDING;
    p_svc->type = type;
    strlcpy(p_svc->svc_name, svc_name, ESP_WIFI_MAX_SVC_NAME_LEN);
    SLIST_INIT(&p_svc->peer_list);

#ifdef CONFIG_ESP_WIFI_NAN_SECURITY
    forced_memzero(&p_svc->user_cfg, sizeof(p_svc->user_cfg));
    forced_memzero(&p_svc->derived_security, sizeof(p_svc->derived_security));
    if (security_cfg) {
        memcpy(&p_svc->user_cfg, security_cfg, sizeof(*security_cfg));
        /* Device-global group_mgmt_prot (IGTKSA/BIGTKSA) forces GTKSA on every
         * secured service: the CSIA capability field has no "IGTK/BIGTK without
         * GTKSA" encoding (§9.5.21.2 Table 122), so advertising group-management
         * protection mandates advertising GTKSA. Warn and force it on if the app
         * requested group_data_prot=0. Forcing support never blocks a peer that
         * lacks protection — keys activate only after capability negotiation. */
        if (s_nan_ctx.group_mgmt_prot && !p_svc->user_cfg.group_data_prot) {
            ESP_LOGW(TAG, "group_data_prot forced ON for '%s': group_mgmt_prot is "
                     "enabled device-wide; GTKSA cannot be advertised without it",
                     svc_name);
            p_svc->user_cfg.group_data_prot = 1;
        }
    }
#ifdef CONFIG_ESP_WIFI_NAN_PAIRING
    if (pairing) {
        memcpy(&p_svc->pairing, pairing, sizeof(*pairing));
    }
#else
    (void)pairing;
#endif
#else
    (void)security_cfg;
    (void)pairing;
#endif
    return p_svc;
}

/* Stamp the real svc_id and per-publish flags after the blob accepts the
 * service. Looked up by name since the WiFi-task derive callback may have
 * already populated derived_security[] before this runs. */
static void nan_finalize_own_svc(const char *svc_name, uint8_t id, bool ndp_resp_needed, uint8_t service_hash[6])
{
    struct own_svc_info *p_svc = nan_find_own_svc_by_name(svc_name);
    if (!p_svc) {
        return;
    }
    p_svc->svc_id = id;
    memcpy(p_svc->svc_hash, service_hash, 6);
    if (p_svc->type == ESP_NAN_PUBLISH) {
        p_svc->ndp_resp_needed = ndp_resp_needed;
    }
}

/* Release a pending slot when the blob's publish/subscribe call fails. */
static void nan_abort_own_svc(const char *svc_name)
{
    struct own_svc_info *p_svc = nan_find_own_svc_by_name(svc_name);
    if (!p_svc) {
        return;
    }
    forced_memzero(p_svc, sizeof(*p_svc));
    /* svc_id back to 0 -> slot free */
}

/* A slot is in use once nan_record_new_ndl/preclaim has stamped peer_nmi,
 * even if ndp_id is still 0 (initiator pre-claim window between M1 build
 * and the WiFi library returning the real ndp_id). Treating ndp_id==0 alone as
 * "free" lets a concurrent claim for a different peer overwrite a
 * pre-claimed slot's security context. */
static inline bool nan_ndl_slot_in_use(const struct ndl_info *ndl)
{
    return (ndl->ndp_id != 0) || !MACADDR_EQUAL(ndl->peer_nmi, null_mac);
}

static bool ndl_limit_reached(void)
{
    for (int i = 0; i < ESP_WIFI_NAN_DATAPATH_MAX_PEERS; i++) {
        if (!nan_ndl_slot_in_use(&s_nan_ctx.ndl[i])) {
            return false;
        }
    }
    return true;
}

static void nan_record_new_ndl(uint8_t ndp_id, uint8_t publish_id, uint8_t peer_nmi[], uint8_t own_role, uint32_t device_caps)
{
    struct ndl_info *ndl = nan_find_ndl_by_pub_id_and_peer(publish_id, peer_nmi);
    /* Reuse the slot when either:
     *  - it is an IDLE pre-claim from the initiator security path, or
     *  - it already holds an active NDP with the same ndp_id. */
    bool reuse_slot = false;
#ifdef CONFIG_ESP_WIFI_NAN_SECURITY
    if (ndl && ndl->handshake_state == NAN_HANDSHAKE_IDLE) {
        reuse_slot = true;
    }
#endif
    if (ndl && ndp_id != 0 && ndl->ndp_id == ndp_id) {
        reuse_slot = true;
    }
    if (ndl && reuse_slot) {
        ndl->ndp_id = ndp_id;
        ndl->own_role = own_role;
        ndl->device_caps = device_caps;
        return;
    }
    if (ndl) {
        /* Stale slot from a prior session; wipe PMK/PTK/handshake state before re-binding. */
        forced_memzero(ndl, sizeof(*ndl));
    } else {
        for (int i = 0; i < ESP_WIFI_NAN_DATAPATH_MAX_PEERS; i++) {
            if (!nan_ndl_slot_in_use(&s_nan_ctx.ndl[i])) {
                ndl = &s_nan_ctx.ndl[i];
                break;
            }
        }
        if (!ndl) {
            ESP_LOGE(TAG, "No free NDL slot for ndp_id=%u pub_id=%u peer="MACSTR,
                     ndp_id, publish_id, MAC2STR(peer_nmi));
            return;
        }
    }
    ndl->ndp_id = ndp_id;
    ndl->device_caps = device_caps;
    if (peer_nmi) {
        MACADDR_COPY(ndl->peer_nmi, peer_nmi);
    }
    /* peer_ndi is populated by WiFi-library callbacks (responder: M1 RX; initiator: M2 RX before MIC verify). */
    ndl->publisher_id = publish_id;
    ndl->own_role = own_role;
}

struct ndl_info *nan_find_ndl(uint8_t ndp_id, uint8_t peer_nmi[])
{
    for (int i = 0; i < ESP_WIFI_NAN_DATAPATH_MAX_PEERS; i++) {
        struct ndl_info *ndl = &s_nan_ctx.ndl[i];
        if (ndp_id != 0 && peer_nmi) {
            if (ndl->ndp_id == ndp_id && MACADDR_EQUAL(ndl->peer_nmi, peer_nmi)) {
                return ndl;
            }
        } else if (ndp_id != 0) {
            if (ndl->ndp_id == ndp_id) {
                return ndl;
            }
        } else if (peer_nmi) {
            if (MACADDR_EQUAL(ndl->peer_nmi, peer_nmi)) {
                return ndl;
            }
        }
    }
    return NULL;
}

/** Find NDL by publisher_id + peer_nmi (e.g. when CSIA/SCIA/key desc parsed before NDP/NDL attribute) */
struct ndl_info *nan_find_ndl_by_pub_id_and_peer(uint8_t pub_id, const uint8_t *peer_nmi)
{
    if (!peer_nmi) {
        return NULL;
    }
    for (int i = 0; i < ESP_WIFI_NAN_DATAPATH_MAX_PEERS; i++) {
        struct ndl_info *ndl = &s_nan_ctx.ndl[i];
        if (ndl->publisher_id == pub_id && MACADDR_EQUAL(ndl->peer_nmi, peer_nmi)) {
            return ndl;
        }
    }
    return NULL;
}

static bool nan_is_datapath_active(void)
{
    for (int i = 0; i < ESP_WIFI_NAN_DATAPATH_MAX_PEERS; i++) {
        if (s_nan_ctx.ndl[i].ndp_id != 0) {
            return true;
        }
    }
    return false;
}

/*
 * Initiator NDL pre-claim / finalize.
 *
 * Problem:
 *   The M1 Shared-Key Descriptor + SCIA security callbacks
 *   (Wi-Fi Aware v4.0 §7.1.3.5) run inside
 *   esp_nan_internal_datapath_req() and need an NDL to look up by
 *   ndp_id -- but the real ndp_id is only known after that call
 *   returns.
 *
 * Workaround:
 *   Pre-claim a slot at ndp_id=0 with security_ctx already populated,
 *   so the security callbacks find it. Once datapath_req returns the
 *   real ndp_id, stamp it onto the pre-claimed slot.
 *
 * Notes:
 *   - Pre-claim is SECURITY-only. With CONFIG_ESP_WIFI_NAN_SECURITY=n
 *     the security callbacks are NULL in nan_secure_dp_funcs and never
 *     run, so no pre-lookup is needed.
 *   - Finalize stays unconditional: nan_record_new_ndl() reuses a
 *     pre-claimed slot when one exists, otherwise allocates fresh.
 *   - Other finalize call sites:
 *       * nan_app_ndp_response_indication_cb (M2 RX, if datapath_req
 *         return races M2 indication)
 *       * key-desc parser claim path in nan_security.c
 */
#ifdef CONFIG_ESP_WIFI_NAN_SECURITY
static struct ndl_info *nan_ndl_preclaim_initiator(uint8_t pub_id,
                                                   uint8_t peer_nmi[],
                                                   uint32_t device_caps)
{
    nan_record_new_ndl(0, pub_id, peer_nmi, ESP_WIFI_NDP_ROLE_INITIATOR, device_caps);
    return nan_find_ndl_by_pub_id_and_peer(pub_id, peer_nmi);
}
#endif

static void nan_ndl_finalize_ndp_id(uint8_t real_ndp_id, uint8_t pub_id,
                                    uint8_t peer_nmi[], uint32_t device_caps)
{
    /* nan_record_new_ndl() stamps real_ndp_id onto the pre-claimed
     * slot found by (pub_id, peer_nmi), or allocates a fresh slot if
     * no pre-claim exists (SECURITY=n path). */
    nan_record_new_ndl(real_ndp_id, pub_id, peer_nmi, ESP_WIFI_NDP_ROLE_INITIATOR, device_caps);
}

static void nan_ndl_release(uint8_t ndp_id_or_zero)
{
    nan_reset_ndl(ndp_id_or_zero, false);
}

/* types of ipv6 addresses to be displayed on ipv6 events */
static const char *s_ipv6_addr_types[] = {
    "UNKNOWN",
    "GLOBAL",
    "LINK_LOCAL",
    "SITE_LOCAL",
    "UNIQUE_LOCAL",
    "IPV4_MAPPED_IPV6"
};

static void nan_app_action_got_ipv6(void *arg, esp_event_base_t event_base, int32_t event_id, void *data)
{
    if (data == NULL) {
        return;
    }
    ip_event_got_ip6_t *event = (ip_event_got_ip6_t *)data;

    NAN_DATA_LOCK();
    if (event->esp_netif == s_nan_ctx.nan_netif) {
        esp_ip6_addr_type_t ipv6_type = esp_netif_ip6_get_addr_type(&event->ip6_info.ip);
        ESP_LOGD(TAG, "NAN Data Interface ready [IPv6 - "IPV6STR", type - %s]",
                 IPV62STR(event->ip6_info.ip), s_ipv6_addr_types[ipv6_type]);
    }
    NAN_DATA_UNLOCK();
}

static esp_err_t nan_clear_app_default_handlers(void)
{
    esp_event_handler_unregister(IP_EVENT, IP_EVENT_GOT_IP6, nan_app_action_got_ipv6);
    s_app_default_handlers_set = false;

    return ESP_OK;
}

static esp_err_t nan_set_app_default_handlers(void)
{
    if (s_app_default_handlers_set) {
        return ESP_OK;
    }

    int ret;
    (void) ret;
    ESP_GOTO_ON_ERROR(esp_event_handler_register(IP_EVENT, IP_EVENT_GOT_IP6,
                                                 nan_app_action_got_ipv6, NULL), fail, TAG, "Registering event handler failed");

    s_app_default_handlers_set = true;
    return ESP_OK;

fail:
    nan_clear_app_default_handlers();
    return ESP_FAIL;
}

void nan_app_post_event(int32_t event_id, void* event_data, size_t event_data_size)
{
    g_wifi_osi_funcs._event_post(WIFI_EVENT, event_id, event_data, event_data_size, OSI_FUNCS_TIME_BLOCKING);
}

static void nan_app_service_match_cb(uint8_t sub_id, struct nan_cb_peer_info *peer_info,
                                     struct nan_cb_npba_t *npba)
{
    if (!peer_info) {
        return;
    }
    uint8_t pub_id = peer_info->peer_svc_id;
    uint8_t *pub_mac = peer_info->peer_mac;
    uint16_t capab = peer_info->sdea;
    uint8_t ssi_ver = peer_info->ssi_ver;
    uint8_t *ssi = peer_info->ssi;
    uint16_t ssi_len = peer_info->ssi_len;
    uint32_t device_caps = peer_info->device_caps;

    NAN_DATA_LOCK();
    struct peer_svc_info *p_peer_svc = nan_find_peer_svc(sub_id, 0, pub_mac);

    if (p_peer_svc && p_peer_svc->svc_id != pub_id) {
        struct ndl_info *ndl = nan_find_ndl(0, pub_mac);

        p_peer_svc->svc_id = pub_id;
        p_peer_svc->device_caps = device_caps;
        if (ndl) {
            ndl->publisher_id = pub_id;
            ndl->device_caps = device_caps;
        }
    } else {
        nan_record_peer_svc(sub_id, pub_id, pub_mac, device_caps);
    }
    NAN_DATA_UNLOCK();

    ESP_LOGI(TAG, "Service matched with capabilities: 0x%04x", capab);

#ifdef CONFIG_ESP_WIFI_NAN_SECURITY
    /* Service-match security gate, keyed by the local subscribe (sub_id):
     *   1. This subscribe was created without credentials (open subscribe) ->
     *      accept regardless of what the peer advertised. NDP will be open.
     *   2. This subscribe was created with credentials (secure subscribe) ->
     *      peer must have advertised PMKIDs AND at least one of them must
     *      match one of this subscribe's creds, else drop.
     * Reading per-subscribe state from own_svc (instead of a singleton cache)
     * disambiguates concurrent secure subscribes — each service uses its own
     * service_name + creds[] in the PMKID derivation. */
    NAN_DATA_LOCK();
    struct own_svc_info *p_own_svc = nan_find_own_svc(sub_id);
    struct peer_svc_info *p_peer_svc_sec = nan_find_peer_svc(sub_id, pub_id, pub_mac);
    bool has_creds = p_own_svc && p_own_svc->user_cfg.num_credentials > 0;
    NAN_DATA_UNLOCK();

    if (has_creds) {
        if (!peer_info->peer_security_params ||
                peer_info->peer_security_params->num_pmkids == 0) {
            ESP_LOGW(TAG, "Secure subscribe '%s': peer "MACSTR" advertises no PMKIDs",
                     p_own_svc ? p_own_svc->svc_name : "?", MAC2STR(pub_mac));
            return;
        }
        if (!p_peer_svc_sec ||
                !nan_security_service_match(p_own_svc, p_peer_svc_sec,
                                            pub_mac, peer_info->peer_security_params)) {
            ESP_LOGW(TAG, "PMKID mismatch: svc='%s' peer="MACSTR,
                     p_own_svc ? p_own_svc->svc_name : "?", MAC2STR(pub_mac));
            return;
        }
    }
#endif

    size_t evt_data_len = sizeof(wifi_event_nan_svc_match_t) + ssi_len;
    wifi_event_nan_svc_match_t *evt = (wifi_event_nan_svc_match_t *)os_zalloc(evt_data_len);
    if (!evt) {
        ESP_LOGE(TAG, "Failed to allocate for event");
        return;
    }

    evt->subscribe_id = sub_id;
    evt->publish_id = pub_id;
    MACADDR_COPY(evt->pub_if_mac, pub_mac);
    evt->update_pub_id = false;
    evt->fsd_reqd = (capab & NAN_SDEA_CTRL_FSD_REQD) ? 1 : 0;
    evt->fsd_gas = (capab & NAN_SDEA_CTRL_FSD_GAS) ? 1 : 0;
    evt->datapath_reqd = (capab & NAN_SDEA_CTRL_DATAPATH_REQD) ? 1 : 0;
    evt->ndpe_support = (device_caps & NAN_CAPS_NDPE_ATTR) ? 1 : 0;
    evt->security_reqd = (capab & NAN_SDEA_CTRL_SECURITY_REQD) ? 1 : 0;

    if (peer_info->peer_security_params) {
        const wifi_nan_peer_sdf_security_t *peer_sec = peer_info->peer_security_params;

        evt->csid_bitmap = peer_sec->csid_bitmap;
        evt->pairing_setup = peer_sec->pairing_setup;
        evt->npk_nik_caching = peer_sec->npk_nik_caching;
    }

#ifdef CONFIG_ESP_WIFI_NAN_PAIRING
    evt->bootstrapping_methods = nan_app_parse_npba_from_publish(npba);
    if (peer_info->nira_verified) {
        evt->already_paired = 1;
    }
#endif

    evt->ssi_version = ssi_ver;
    if (ssi && ssi_len) {
        if (ssi_ver) {
            evt->ssi_version = ssi_ver;
        }
        memcpy(evt->ssi, ssi, ssi_len);
        evt->ssi_len = ssi_len;
        ESP_LOG_BUFFER_HEXDUMP(TAG, ssi, ssi_len, ESP_LOG_DEBUG);
    }

    ESP_LOGI(TAG, "Service matched with "MACSTR" [Peer Publish id - %d]",
             MAC2STR(pub_mac), pub_id);

    nan_app_post_event(WIFI_EVENT_NAN_SVC_MATCH, evt, evt_data_len);
    os_free(evt);
}

static void nan_app_replied_cb(uint8_t pub_id, struct nan_cb_peer_info *peer_info)
{
    if (!peer_info) {
        return;
    }
    uint8_t sub_id = peer_info->peer_svc_id;
    uint8_t *sub_nmi = peer_info->peer_mac;

    uint8_t *ssi = peer_info->ssi;
    uint16_t ssi_len = peer_info->ssi_len;
    uint32_t device_caps = peer_info->device_caps;

    NAN_DATA_LOCK();
    struct peer_svc_info *p_peer_svc = nan_find_peer_svc(pub_id, sub_id, sub_nmi);
    if (!p_peer_svc) {
        p_peer_svc = nan_find_peer_svc(pub_id, 0, sub_nmi);
    }
    if (!p_peer_svc) {
        nan_record_peer_svc(pub_id, sub_id, sub_nmi, device_caps);
    } else {
        if (p_peer_svc->svc_id != sub_id) {
            p_peer_svc->svc_id = sub_id;
        }
        if (p_peer_svc->own_svc_id != pub_id) {
            p_peer_svc->own_svc_id = pub_id;
        }
        if (p_peer_svc->device_caps != device_caps) {
            p_peer_svc->device_caps = device_caps;
        }
        if (!MACADDR_EQUAL(p_peer_svc->peer_nmi, sub_nmi)) {
            MACADDR_COPY(p_peer_svc->peer_nmi, sub_nmi);
        }
    }
    NAN_DATA_UNLOCK();

    size_t evt_data_len = sizeof(wifi_event_nan_replied_t) + ssi_len;

    wifi_event_nan_replied_t *evt = (wifi_event_nan_replied_t *)os_zalloc(evt_data_len);
    if (!evt) {
        ESP_LOGE(TAG, "Failed to allocate for event");
        return;
    }
    evt->publish_id = pub_id;
    evt->subscribe_id = sub_id;
    MACADDR_COPY(evt->sub_if_mac, sub_nmi);

    if (ssi && ssi_len) {
        memcpy(evt->ssi, ssi, ssi_len);
        evt->ssi_len = ssi_len;
        ESP_LOG_BUFFER_HEXDUMP(TAG, ssi, ssi_len, ESP_LOG_DEBUG);
    }

    nan_app_post_event(WIFI_EVENT_NAN_REPLIED, evt, evt_data_len);
    os_free(evt);
}

static void nan_app_receive_cb(uint8_t svc_id, struct nan_cb_peer_info *peer_info,
                               uint8_t *shared_key_attr, uint16_t shared_key_attr_buf_len,
                               struct nan_cb_npba_t *npba)
{
    if (!peer_info) {
        return;
    }
    uint8_t peer_svc_id = peer_info->peer_svc_id;
    uint8_t *peer_mac = peer_info->peer_mac;
    uint8_t *ssi = peer_info->ssi;
    uint16_t ssi_len = peer_info->ssi_len;
    uint32_t device_caps = peer_info->device_caps;

    NAN_DATA_LOCK();
    struct peer_svc_info *p_peer_svc = nan_find_peer_svc(svc_id, peer_svc_id, peer_mac);
    if (!p_peer_svc) {
        p_peer_svc = nan_find_peer_svc(svc_id, 0, peer_mac);
    }
    if (!p_peer_svc) {
        nan_record_peer_svc(svc_id, peer_svc_id, peer_mac, device_caps);
    } else {
        if (p_peer_svc->svc_id != peer_svc_id) {
            p_peer_svc->svc_id = peer_svc_id;
        }
        if (p_peer_svc->own_svc_id != svc_id) {
            p_peer_svc->own_svc_id = svc_id;
        }
        if (p_peer_svc->device_caps != device_caps) {
            p_peer_svc->device_caps = device_caps;
        }
        if (!MACADDR_EQUAL(p_peer_svc->peer_nmi, peer_mac)) {
            MACADDR_COPY(p_peer_svc->peer_nmi, peer_mac);
        }
    }
    NAN_DATA_UNLOCK();

#if defined(CONFIG_ESP_WIFI_NAN_SECURITY) && defined(CONFIG_ESP_WIFI_NAN_PAIRING) && defined(CONFIG_ESP_WIFI_PASN_SUPPORT)
    if (shared_key_attr) {
        nan_app_receive_pairing_followup(svc_id, peer_svc_id, peer_mac, shared_key_attr,
                                         shared_key_attr_buf_len);
        return;
    }
#endif

#if defined(CONFIG_ESP_WIFI_NAN_PAIRING)
    if (npba) {
        nan_app_parse_npba_from_receive(svc_id, peer_svc_id, peer_mac, npba);
        return;
    }
#endif

    size_t evt_data_len = sizeof(wifi_event_nan_receive_t) + ssi_len;
    wifi_event_nan_receive_t *evt = (wifi_event_nan_receive_t *)os_zalloc(evt_data_len);
    if (!evt) {
        ESP_LOGE(TAG, "Failed to allocate for event");
        return;
    }

    evt->inst_id = svc_id;
    evt->peer_inst_id = peer_svc_id;
    MACADDR_COPY(evt->peer_if_mac, peer_mac);
    if (ssi && ssi_len) {
        memcpy(evt->ssi, ssi, ssi_len);
        evt->ssi_len = ssi_len;
        ESP_LOGD(TAG, "Received payload from Peer "MACSTR" [Peer Service id - %d] - ", MAC2STR(peer_mac), peer_svc_id);
    }

    nan_app_post_event(WIFI_EVENT_NAN_RECEIVE, evt, evt_data_len);
    os_free(evt);
}

static void nan_app_ndp_indication_cb(uint8_t pub_id, struct ndp_cb_peer_info *peer_info, uint32_t device_caps)
{
    /*
     * Responder-side NDP indication. Security parsers (CSIA/SCIA/
     * Shared-Key) have already run and stashed the peer's security
     * inputs in static pending caches; nan_security_apply_pending()
     * below promotes them onto the NDL. NAN_DATA_LOCK guards
     * s_nan_ctx mutation only and is released before any
     * esp_nan_internal_* call (see lock contract in nan_i.h).
     */
    if (!peer_info) {
        return;
    }
    uint8_t ndp_id = peer_info->ndp_id;
    uint8_t *peer_nmi = peer_info->peer_nmi;
    uint8_t *peer_ndi = peer_info->peer_ndi;
    uint8_t *ssi = peer_info->ssi;
    uint16_t ssi_len = peer_info->ssi_len;
    bool ndp_resp_needed = false;

    bool send_auto_resp = false;
    wifi_nan_datapath_resp_t ndp_resp = {0};
    ip_addr_t own_ipv6 = {0};

    NAN_DATA_LOCK();
    struct own_svc_info *p_own_svc = nan_find_own_svc(pub_id);

    if (!p_own_svc) {
        NAN_DATA_UNLOCK();
        ESP_LOGE(TAG, "No Publish found with id %d", pub_id);
        return;
    }
    ndp_resp_needed = p_own_svc->ndp_resp_needed;
    if (ndl_limit_reached()) {
        NAN_DATA_UNLOCK();
        ESP_LOGE(TAG, "NDP limit reached");
        return;
    }

    nan_record_new_ndl(ndp_id, pub_id, peer_nmi, ESP_WIFI_NDP_ROLE_RESPONDER, device_caps);

    struct peer_svc_info *p_peer_svc = nan_find_peer_svc(pub_id, 0, peer_nmi);
    if (!p_peer_svc) {
        nan_record_peer_svc(pub_id, 0, peer_nmi, device_caps);
    } else {
        p_peer_svc->device_caps = device_caps;
    }

    struct ndl_info *ndl = nan_find_ndl(ndp_id, peer_nmi);
    if (ndl) {
        MACADDR_COPY(ndl->peer_ndi, peer_ndi);
    }

#ifdef CONFIG_ESP_WIFI_NAN_SECURITY
    /* Apply pending CSIA/SCIA/M1 (captured before this indication) to the NDL. */
    nan_security_apply_pending(ndl, p_own_svc, pub_id, peer_nmi, peer_ndi);
#endif

    if (p_own_svc->ndp_resp_needed) {
        ESP_LOGD(TAG, "NDP Req from "MACSTR" [NDP Id: %d], Accept OR Deny using NDP command",
                 MAC2STR(peer_nmi), ndp_id);
        s_nan_ctx.event |= NDP_INDICATION;
    } else {
        /* Build auto-response from NDL state under lock; dispatch the WiFi-library
         * call after release (esp_nan_internal_* must not be called with
         * NAN_DATA_LOCK held -- see nan_i.h). */
        ndp_resp.accept = true;
        ndp_resp.ndp_id = ndp_id;
        MACADDR_COPY(ndp_resp.peer_mac, peer_nmi);
        if (device_caps & NAN_CAPS_NDPE_ATTR) {
            uint8_t own_bssid[6];
            esp_err_t err = esp_wifi_get_mac(WIFI_IF_NAN, own_bssid);
            if (err != ESP_OK) {
                /* Cannot build the auto-response: free the NDL slot and deny the
                 * peer so it does not wait indefinitely. Send outside the lock. */
                ESP_LOGE(TAG, "get own NAN MAC failed, rc=0x%x; denying NDP ndp_id=%d", err, ndp_id);
                nan_reset_ndl(ndp_id, false);
                NAN_DATA_UNLOCK();
                ndp_resp.accept = false;
                esp_nan_internal_datapath_resp(&ndp_resp, (uint8_t *)&own_ipv6.u_addr.ip6.addr[2]);
                return;
            }
            esp_wifi_nan_get_ipv6_linklocal_from_mac(&own_ipv6.u_addr.ip6, own_bssid);
        }

        /* Datapath security on the auto-respond path is sourced from the
         * NDL's security_ctx (populated by nan_security_apply_pending above
         * from publish-time cfg). The WiFi library's M2 builder callbacks look it
         * up by (ndp_id, peer_nmi); no per-resp security field needed. */
        send_auto_resp = true;
    }
    NAN_DATA_UNLOCK();

    if (send_auto_resp) {
        esp_err_t resp_err = esp_nan_internal_datapath_resp(&ndp_resp, (uint8_t *)&own_ipv6.u_addr.ip6.addr[2]);
        if (resp_err != ESP_OK) {
            ESP_LOGE(TAG, "Auto NDP response failed for ndp_id=%u peer="MACSTR" err=%d",
                     ndp_id, MAC2STR(peer_nmi), resp_err);
        }
    }

    /* Event-post path reads only peer_info / ssi (WiFi-library-owned, not s_nan_ctx),
     * so it runs outside the lock. */
    size_t evt_data_len = sizeof(wifi_event_ndp_indication_t) + ssi_len;
    wifi_event_ndp_indication_t *evt = (wifi_event_ndp_indication_t *)os_zalloc(evt_data_len);
    if (!evt) {
        ESP_LOGE(TAG, "Failed to allocate for event");
        if (ndp_resp_needed) {
            uint8_t ipv6_identifier[8] = {0};
            wifi_nan_datapath_resp_t ndp_resp = {0};

            ndp_resp.accept = false;
            ndp_resp.ndp_id = ndp_id;
            MACADDR_COPY(ndp_resp.peer_mac, peer_nmi);

            NAN_DATA_LOCK();
            nan_reset_ndl(ndp_id, false);
            s_nan_ctx.event &= ~NDP_INDICATION;
            NAN_DATA_UNLOCK();

            esp_nan_internal_datapath_resp(&ndp_resp, ipv6_identifier);
        }
        return;
    }

    evt->publish_id = pub_id;
    evt->ndp_id = ndp_id;
    MACADDR_COPY(evt->peer_nmi, peer_nmi);
    MACADDR_COPY(evt->peer_ndi, peer_ndi);
    if (ssi && ssi_len) {
        memcpy(evt->ssi, ssi, ssi_len);
        evt->ssi_len = ssi_len;
        ESP_LOG_BUFFER_HEXDUMP(TAG, ssi, ssi_len, ESP_LOG_DEBUG);
    }

    nan_app_post_event(WIFI_EVENT_NDP_INDICATION, evt, evt_data_len);
    os_free(evt);
}

static void nan_app_ndp_response_indication_cb(struct ndp_cb_peer_info *peer_info)
{
    if (!peer_info) {
        return;
    }
    uint8_t ndp_id = peer_info->ndp_id;
    uint8_t *peer_nmi = peer_info->peer_nmi;
    uint8_t *peer_ndi = peer_info->peer_ndi;

    NAN_DATA_LOCK();
    struct ndl_info *ndl = nan_find_ndl(ndp_id, peer_nmi);
    if (!ndl) {
        /* Initiator pre-claim path may still have ndp_id=0 if the M1 builder
         * did not run through the SCIA / key-desc helpers (unsecured path).
         * Fall back to peer-only lookup and stamp ndp_id. */
        ndl = nan_find_ndl(0, peer_nmi);
        if (ndl && ndl->ndp_id == 0 && ndp_id != 0) {
            ndl->ndp_id = ndp_id;
        }
    }
    if (ndl && peer_ndi) {
        MACADDR_COPY(ndl->peer_ndi, peer_ndi);
        ESP_LOGD(TAG, "NDP M2 RX: stored peer NDI "MACSTR" (ndp_id=%d)",
                 MAC2STR(peer_ndi), ndp_id);
    } else if (!ndl) {
        ESP_LOGW(TAG, "NDP M2 RX: no NDL for ndp_id=%d peer="MACSTR,
                 ndp_id, MAC2STR(peer_nmi));
    }
    NAN_DATA_UNLOCK();
}

/* Tear down an NDP whose confirm callback fired but cannot be completed
 * (TK not ready, initiator handshake not COMPLETE, key install failed,
 * event alloc failed). Notifies the peer via datapath_end, wipes the NDL
 * (including TK / KCK / KEK material), and unblocks any waiting app. */
static void nan_ndp_confirm_teardown(const uint8_t peer_nmi[6], uint8_t ndp_id)
{
    wifi_nan_datapath_end_req_t ndp_end = {0};

    MACADDR_COPY(ndp_end.peer_mac, peer_nmi);
    ndp_end.ndp_id = ndp_id;

    NAN_DATA_UNLOCK();
    esp_nan_internal_datapath_end(&ndp_end);
    NAN_DATA_LOCK();

    nan_reset_ndl(ndp_id, false);
    os_event_group_set_bits(nan_event_group, NDP_REJECTED);
}

static void nan_app_ndp_confirm_cb(uint8_t status, struct ndp_cb_peer_info *peer_info,
                            uint8_t own_ndi[6], uint8_t ipv6_identifier[8])
{
    if (!peer_info) {
        return;
    }
    uint8_t ndp_id = peer_info->ndp_id;
    uint8_t *peer_nmi = peer_info->peer_nmi;
    uint8_t *peer_ndi = peer_info->peer_ndi;
    uint8_t *ssi = peer_info->ssi;
    uint16_t ssi_len = peer_info->ssi_len;
    NAN_DATA_LOCK();
    struct ndl_info *ndl = nan_find_ndl(ndp_id, peer_nmi);
    if (!ndl) {
        ESP_LOGE(TAG, "No NDL with ndp id %d", ndp_id);
        NAN_DATA_UNLOCK();
        return;
    }
    MACADDR_COPY(ndl->peer_ndi, peer_ndi);

    wifi_netif_driver_t driver = esp_netif_get_io_driver(s_nan_ctx.nan_netif);

    if (!s_nan_ctx.nan_netif) {
        ESP_LOGE(TAG, "%s: NAN netif is NULL", __func__);
        goto done;
    }

    if (status == NDP_STATUS_REJECTED) {
        ESP_LOGE(TAG, "NDP request to Peer "MACSTR" rejected [NDP ID - %d]", MAC2STR(peer_nmi), ndp_id);
        nan_reset_ndl(ndp_id, false);
        os_event_group_set_bits(nan_event_group, NDP_REJECTED);
        goto done;
    }

    /* If interface not ready when started, rxcb to be registered on connection */
    if (esp_wifi_register_if_rxcb(driver,  esp_netif_receive, s_nan_ctx.nan_netif) != ESP_OK) {
        ESP_LOGE(TAG, "%s: esp_wifi_register_if_rxcb failed", __func__);
        goto done;
    }

#ifdef CONFIG_ESP_WIFI_NAN_SECURITY
    if (ndl->security_ctx.type == WIFI_NAN_SECURITY_ENCRYPTED) {
        if (!ndl->ptk_set || ndl->tk_len < NAN_NCS_SK_128_TK_LEN) {
            ESP_LOGE(TAG, "NDP confirm: encrypted datapath but TK is not ready (ndp_id=%d)", ndp_id);
            nan_ndp_confirm_teardown(peer_nmi, ndp_id);
            goto done;
        }
        /* Initiator: refuse TK install unless M4 MIC verifier promoted state to COMPLETE.
         * Parser sets M4_RCVD on receipt; verifier transitions to COMPLETE on a passing
         * HMAC-SHA256(KCK, M4_body). If we're still at M4_RCVD here, MIC verify failed
         * or was skipped — do not install the TK on a possibly tampered handshake. */
        if (ndl->own_role == ESP_WIFI_NDP_ROLE_INITIATOR &&
            ndl->handshake_state != NAN_HANDSHAKE_COMPLETE) {
            ESP_LOGE(TAG, "NDP confirm (initiator): handshake_state=%d (not COMPLETE); skipping TK install",
                     ndl->handshake_state);
            nan_ndp_confirm_teardown(peer_nmi, ndp_id);
            goto done;
        }
    }
#endif /* CONFIG_ESP_WIFI_NAN_SECURITY */

    /* Allocate the confirm event before installing the pairwise key, so an
     * allocation failure can tear the NDP down without leaving a stale key
     * bound to peer_ndi in the MAC's key store. */
    size_t evt_data_len = sizeof(wifi_event_ndp_confirm_t) + ssi_len;
    wifi_event_ndp_confirm_t *evt = (wifi_event_ndp_confirm_t *)os_zalloc(evt_data_len);
    if (!evt) {
        ESP_LOGE(TAG, "Failed to allocate for event, terminate NDP");
        nan_ndp_confirm_teardown(peer_nmi, ndp_id);
        goto done;
    }

#ifdef CONFIG_ESP_WIFI_NAN_SECURITY
    if (ndl->security_ctx.type == WIFI_NAN_SECURITY_ENCRYPTED) {
        uint8_t key_rsc[8] = {0};
        int ret = esp_wifi_set_nan_key_internal(NAN_WIFI_WPA_ALG_CCMP,
                                                peer_ndi,
                                                0,
                                                1,
                                                key_rsc,
                                                sizeof(key_rsc),
                                                ndl->nd_tk,
                                                NAN_NCS_SK_128_TK_LEN,
                                                NAN_KEY_ND_TK);
        ESP_LOG_BUFFER_HEXDUMP("## ND-TK ", ndl->nd_tk, NAN_NCS_SK_128_TK_LEN, ESP_LOG_DEBUG);
        if (ret != 0) {
            ESP_LOGE(TAG, "NDP confirm: failed to install ND-TK (ndp_id=%d, ret=%d)", ndp_id, ret);
            os_free(evt);
            nan_ndp_confirm_teardown(peer_nmi, ndp_id);
            goto done;
        }
        ret = esp_wifi_set_nan_key_internal(NAN_WIFI_WPA_ALG_CCMP,
                                                peer_nmi,
                                                0,
                                                1,
                                                key_rsc,
                                                sizeof(key_rsc),
                                                ndl->nd_tk,
                                                NAN_NCS_SK_128_TK_LEN,
                                                NAN_KEY_NM_TK);
        if (ret != 0) {
            ESP_LOGE(TAG, "NDP confirm: failed to install NM-TK (ndp_id=%d, ret=%d)", ndp_id, ret);
            os_free(evt);
            nan_ndp_confirm_teardown(peer_nmi, ndp_id);
            goto done;
        }

        /* Group keys (ND-GTK) exchanged during NDP setup (§7.1.3.2): our GTK
         * is the TX key bound to the local NDI; the peer's GTK is the RX key
         * bound to the peer NDI. Best-effort — a GTK install failure must not
         * tear down the working unicast datapath. */
        if (ndl->own_gtk_set && ndl->own_gtk_len) {
            int gret = esp_wifi_set_nan_key_internal(NAN_WIFI_WPA_ALG_CCMP,
                                                     own_ndi, ndl->own_gtk_keyid, 1,
                                                     ndl->own_gtk_rsc, NAN_KEY_RSC_LEN,
                                                     ndl->own_gtk, ndl->own_gtk_len,
                                                     NAN_KEY_ND_GTK);
            if (gret != 0) {
                ESP_LOGW(TAG, "NDP confirm: own GTK (TX) install failed (ndp_id=%d, ret=%d)", ndp_id, gret);
            } else {
                ESP_LOGI(TAG, "NDP confirm: own GTK (TX) installed (keyid=%d)", ndl->own_gtk_keyid);
            }
            ESP_LOG_BUFFER_HEXDUMP("## ND-GTK ", ndl->own_gtk, ndl->own_gtk_len, ESP_LOG_DEBUG);
        }
        if (ndl->gtk_set && ndl->gtk_len) {
            int gret = esp_wifi_set_nan_key_internal(NAN_WIFI_WPA_ALG_CCMP,
                                                     peer_ndi, ndl->gtk_keyid, 0,
                                                     ndl->gtk_rsc, NAN_KEY_RSC_LEN,
                                                     ndl->gtk, ndl->gtk_len,
                                                     NAN_KEY_ND_GTK);
            if (gret != 0) {
                ESP_LOGW(TAG, "NDP confirm: peer GTK (RX) install failed (ndp_id=%d, ret=%d)", ndp_id, gret);
            } else {
                ESP_LOGI(TAG, "NDP confirm: peer GTK (RX) installed (keyid=%d)", ndl->gtk_keyid);
            }
        }

        /* Own IGTK/BIGTK (TX) are installed once at NAN start (see
         * nan_security_install_own_group_integrity_keys); not re-installed here,
         * to preserve the blob's monotonic BIPN/IPN across the session. Only the
         * peer RX keys are bound at NDP confirm. §7.1.3.3/§7.1.3.4; NMI==NDI today.
         * Peer IGTK/BIGTK install RX-only against the peer NMI, seeding the BIP
         * RX replay counter with the peer's advertised IPN/BIPN from the KDE. */
        if (ndl->igtk_set && ndl->igtk_len) {
            if (ndl->igtk_len != NAN_ND_GTK_LEN) {
                ESP_LOGW(TAG, "NDP confirm: peer IGTK len=%d unsupported (BIP-CMAC-128 only); skipping",
                         ndl->igtk_len);
            } else {
                int r = esp_wifi_set_nan_key_internal(NAN_WIFI_WPA_ALG_BIP_CMAC_128,
                                                      peer_nmi, ndl->igtk_keyid, 0,
                                                      ndl->igtk_ipn, 6,
                                                      ndl->igtk, ndl->igtk_len,
                                                      NAN_KEY_ND_IGTK);
                if (r != 0) {
                    ESP_LOGW(TAG, "NDP confirm: peer IGTK (RX) install failed, rc=0x%x (ndp_id=%d)", r, ndp_id);
                } else {
                    ESP_LOGI(TAG, "NDP confirm: peer IGTK (RX) installed (keyid=%d)", ndl->igtk_keyid);
                }
                /* Peer IGTK bytes for sniffer MIC cross-check vs the peer's multicast SDFs. */
                ESP_LOG_BUFFER_HEXDUMP("## PEER ND-IGTK ", ndl->igtk, ndl->igtk_len, ESP_LOG_DEBUG);
            }
        }
        if (ndl->bigtk_set && ndl->bigtk_len) {
            if (ndl->bigtk_len != NAN_ND_GTK_LEN) {
                ESP_LOGW(TAG, "NDP confirm: peer BIGTK len=%d unsupported (BIP-CMAC-128 only); skipping",
                         ndl->bigtk_len);
            } else {
                int r = esp_wifi_set_nan_key_internal(NAN_WIFI_WPA_ALG_BIP_CMAC_128,
                                                      peer_nmi, ndl->bigtk_keyid, 0,
                                                      ndl->bigtk_ipn, 6,
                                                      ndl->bigtk, ndl->bigtk_len,
                                                      NAN_KEY_ND_BIGTK);
                if (r != 0) {
                    ESP_LOGW(TAG, "NDP confirm: peer BIGTK (RX) install failed, rc=0x%x (ndp_id=%d)", r, ndp_id);
                } else {
                    ESP_LOGI(TAG, "NDP confirm: peer BIGTK (RX) installed (keyid=%d)", ndl->bigtk_keyid);
                }
                /* Peer BIGTK bytes for sniffer MIC cross-check vs the peer's protected Beacons. */
                ESP_LOG_BUFFER_HEXDUMP("## PEER ND-BIGTK ", ndl->bigtk, ndl->bigtk_len, ESP_LOG_DEBUG);
            }
        }
    }
#endif /* CONFIG_ESP_WIFI_NAN_SECURITY */
    evt->status = status;
    evt->ndp_id = ndp_id;
    MACADDR_COPY(evt->peer_nmi, peer_nmi);
    MACADDR_COPY(evt->peer_ndi, peer_ndi);
    MACADDR_COPY(evt->own_ndi, own_ndi);

    if (IS_ZERO_NAN_IPV6_IDENTIFIER(ipv6_identifier)) {
        ip6_addr_t linklocal;
        esp_wifi_nan_get_ipv6_linklocal_from_mac(&linklocal, peer_ndi);
        memcpy(evt->ipv6_identifier, &linklocal.addr[2], NAN_IPV6_IDENTIFIER_LEN);
    } else {
        memcpy(evt->ipv6_identifier, ipv6_identifier, NAN_IPV6_IDENTIFIER_LEN);
    }
    if (ssi && ssi_len) {
        memcpy(evt->ssi, ssi, ssi_len);
        evt->ssi_len = ssi_len;
        ESP_LOG_BUFFER_HEXDUMP(TAG, ssi, ssi_len, ESP_LOG_DEBUG);
    }

    NAN_DATA_UNLOCK();

    ip6_addr_t peer_ip6 = {0};
    peer_ip6.addr[0] = PP_HTONL(0xfe800000);
    peer_ip6.addr[1] = 0;
    memcpy(&peer_ip6.addr[2], evt->ipv6_identifier, 8);

    ESP_LOGI(TAG, "NDP confirmed with Peer "MACSTR" [NDP ID - %d, Peer IPv6 - %s]",
             MAC2STR(peer_nmi), ndp_id, inet6_ntoa(peer_ip6));

#if CONFIG_LWIP_ND6_SUPPORT_STATIC_ENTRIES
    /* Pin the peer's link-local -> NDI mapping so traffic to the peer skips
     * Neighbor Discovery (no NS/NA) on the NAN link. The esp_wifi netif layer
     * owns the netif lookup and derives the peer's link-local from its NDI
     * (the address the peer actually sources from). */
    esp_err_t nbr_err = esp_wifi_netif_set_static_neighbor(WIFI_IF_NAN, peer_ndi, true);
    if (nbr_err != ESP_OK) {
        ESP_LOGW(TAG, "static nbr ADD failed: %s", esp_err_to_name(nbr_err));
    }
#else
    esp_netif_action_connected(s_nan_ctx.nan_netif, WIFI_EVENT, WIFI_EVENT_NDP_CONFIRM, evt);
    esp_netif_create_ip6_linklocal(s_nan_ctx.nan_netif);
#endif

    os_event_group_set_bits(nan_event_group, NDP_ACCEPTED);
    nan_app_post_event(WIFI_EVENT_NDP_CONFIRM, evt, evt_data_len);
    os_free(evt);
    return;

done:
    NAN_DATA_UNLOCK();
    return;
}

static void nan_app_ndp_terminated_cb(uint8_t reason, uint8_t ndp_id, uint8_t init_ndi[6])
{
    NAN_DATA_LOCK();
    if (s_nan_ctx.nan_netif && !nan_is_datapath_active()) {
        esp_netif_action_disconnected(s_nan_ctx.nan_netif, WIFI_EVENT, WIFI_EVENT_NDP_TERMINATED, NULL);
    }
    ESP_LOGI(TAG, "NDP terminated with Peer "MACSTR" [NDP ID - %d]", MAC2STR(init_ndi), ndp_id);
    nan_reset_ndl(ndp_id, false);

    s_nan_ctx.event &= ~(NDP_INDICATION);
    NAN_DATA_UNLOCK();

#if CONFIG_LWIP_ND6_SUPPORT_STATIC_ENTRIES
    /* Drop the peer's static neighbor mapping added on NDP confirm. (It is also
     * cleared automatically if the NAN netif goes down on the last datapath.) */
    esp_err_t nbr_err = esp_wifi_netif_set_static_neighbor(WIFI_IF_NAN, init_ndi, false);
    if (nbr_err != ESP_OK) {
        ESP_LOGW(TAG, "static nbr DEL failed: %s", esp_err_to_name(nbr_err));
    }
#endif

    wifi_event_ndp_terminated_t *evt = (wifi_event_ndp_terminated_t *)os_zalloc(sizeof(wifi_event_ndp_terminated_t));
    if (!evt) {
        ESP_LOGE(TAG, "Failed to allocate for event");
        return;
    }
    evt->reason = reason;
    evt->ndp_id = ndp_id;
    MACADDR_COPY(evt->init_ndi, init_ndi);

    nan_app_post_event(WIFI_EVENT_NDP_TERMINATED, evt, sizeof(wifi_event_ndp_terminated_t));
    os_free(evt);
    os_event_group_set_bits(nan_event_group, NDP_TERMINATED);
}

static void nan_action_txdone_cb(uint32_t context, bool tx_status)
{
    if (nan_event_group && s_fup_context == context) {
        if (tx_status) {
            os_event_group_set_bits(nan_event_group, NAN_TX_SUCCESS);
        } else {
            os_event_group_set_bits(nan_event_group, NAN_TX_FAILURE);
        }
    }
}

static void esp_nan_ndp_tx_done_cb(uint8_t ndp_id, const uint8_t *peer_nmi, uint8_t msg_type, bool tx_status)
{

    NAN_DATA_LOCK();

    struct ndl_info *ndl = nan_find_ndl(ndp_id, (uint8_t *)peer_nmi);
    if (!ndl) {
        ESP_LOGE(TAG, "NDP TX Confirm: No NDL found for ndp_id=%d", ndp_id);
        NAN_DATA_UNLOCK();
        return;
    }

    if (!tx_status) {
        ESP_LOGE(TAG, "NDP TX Confirm: msg_type=%d transmission failed for ndp_id=%d", msg_type, ndp_id);
        NAN_DATA_UNLOCK();
        return;
    }

    ESP_LOGD(TAG, "NDP TX Confirm: msg_type=%d sent successfully, ndp_id=%d", msg_type, ndp_id);

#ifdef CONFIG_ESP_WIFI_NAN_SECURITY
    /* Update security handshake state if encrypted datapath is active */
    if (msg_type == 2 && ndl->handshake_state == NAN_HANDSHAKE_M1_RCVD) {
        ndl->handshake_state = NAN_HANDSHAKE_M2_SENT;
    } else if (msg_type == 4 && ndl->handshake_state == NAN_HANDSHAKE_M3_RCVD) {
        ndl->handshake_state = NAN_HANDSHAKE_COMPLETE;
    }
#endif /* CONFIG_ESP_WIFI_NAN_SECURITY */

    NAN_DATA_UNLOCK();
}

/* NAN secure-datapath helpers registered with the WiFi libraries. */
static struct nan_secure_dp_funcs s_nan_secure_dp_funcs = {
    /* Always-present helpers */
    .ndp_tx_done_cb                            = esp_nan_ndp_tx_done_cb,

#if CONFIG_ESP_WIFI_NAN_SECURITY
    /* Length getters */
    .get_csia_len                              = esp_nan_get_csia_len,
    .get_scia_len                              = esp_nan_get_scia_len,
    .get_shared_key_desc_attr_len              = esp_nan_get_shared_key_desc_attr_len,
    .ndp_security_install_get_shared_desc_len  = esp_nan_ndp_security_install_get_shared_desc_len,
    .get_ndp_resp_num_pmkids                   = esp_nan_get_ndp_resp_num_pmkids,
    .get_ndp_resp_shared_key_desc_len          = esp_nan_get_ndp_resp_shared_key_desc_len,
    .ndp_confirm_get_shared_desc_len           = esp_nan_ndp_confirm_get_shared_desc_len,

    /* CSIA / SCIA construction */
    .construct_csia                            = esp_nan_construct_csia,
    .construct_scia_publish                    = esp_nan_construct_scia_publish,
    .construct_scia_ndp_req                    = esp_nan_construct_scia_ndp_req,
    .construct_scia_ndp_resp                   = esp_nan_construct_scia_ndp_resp,

    /* Shared Key Descriptor builders */
    .get_ndp_req_shared_key_desc               = esp_nan_get_ndp_req_shared_key_desc,
    .get_ndp_resp_shared_key_desc              = esp_nan_get_ndp_resp_shared_key_desc,
    .get_ndp_confirm_shared_key_desc           = esp_nan_get_ndp_confirm_shared_key_desc,
    .get_ndp_security_install_key_desc         = esp_nan_get_ndp_security_install_key_desc,

    /* M1 Auth_Token capture */
    .capture_m1_auth_token                     = esp_nan_capture_m1_auth_token,

    /* MIC compute (TX path) */
    .update_ndp_resp_mic                       = esp_nan_update_ndp_resp_mic,
    .update_ndp_confirm_mic                    = esp_nan_update_ndp_confirm_mic,
    .update_ndp_security_install_mic           = esp_nan_update_ndp_security_install_mic,

    /* MIC verify (RX path) */
    .verify_ndp_resp_mic                       = esp_nan_verify_ndp_resp_mic,
    .verify_ndp_confirm_mic                    = esp_nan_verify_ndp_confirm_mic,
    .verify_ndp_security_install_mic           = esp_nan_verify_ndp_security_install_mic,

    /* RX-path attribute parsers */
    .parse_ndp_csia                            = esp_nan_parse_ndp_csia,
    .parse_ndp_scia                            = esp_nan_parse_ndp_scia,
    .parse_ndp_key_desc                        = esp_nan_parse_ndp_key_desc,

    /* Publish-side security parser */
    .parse_publish_security                    = esp_nan_parse_publish_security,

    /* Publish-init helper */
    .derive_security_params                    = nan_derive_security_params,

    /* NDP security gate query */
    .get_ndp_security_csid                     = nan_get_ndp_security_csid,
#endif /* CONFIG_ESP_WIFI_NAN_SECURITY */
};

void esp_nan_app_deinit(void)
{
    esp_nan_internal_register_secure_dp_funcs(NULL);

    /* Free per-peer/NDL state in case Wi-Fi is being deinit'd without a
     * prior esp_wifi_nan_sync_stop. Not SECURITY-gated: reset helpers
     * touch only fields that exist in both configs; conditional key
     * material lives inside the #ifdef'd region of struct ndl_info. */
    if (s_nan_data_lock) {
        NAN_DATA_LOCK();
        nan_reset_service(0, true);
        nan_reset_ndl(0, true);
        memset(&s_nan_ctx, 0, sizeof(s_nan_ctx));
        NAN_DATA_UNLOCK();
    }

    if (nan_event_group) {
        os_event_group_delete(nan_event_group);
        nan_event_group = NULL;
    }

    if (s_nan_data_lock) {
        os_mutex_delete(s_nan_data_lock);
        s_nan_data_lock = NULL;
    }
}

void esp_nan_app_init(void)
{
    if (nan_event_group) {
        os_event_group_delete(nan_event_group);
        nan_event_group = NULL;
    }
    nan_event_group = os_event_group_create();

    s_nan_data_lock = os_recursive_mutex_create();
    if (!s_nan_data_lock) {
        ESP_LOGE(TAG, "Failed to create NAN data lock");
        esp_nan_app_deinit();
        return;
    }

    esp_nan_internal_register_secure_dp_funcs(&s_nan_secure_dp_funcs);
}

#ifdef CONFIG_ESP_WIFI_NAN_PAIRING
#if defined(CONFIG_ESP_WIFI_NAN_SECURITY)
static bool nan_peer_cred_npk_present(const wifi_nan_peer_creds_t *c)
{
    static const uint8_t zero_npk[ESP_WIFI_NAN_NPK_LEN] = {0};

    if (!c || !c->is_valid) {
        return false;
    }
    return memcmp(c->npk, zero_npk, ESP_WIFI_NAN_NPK_LEN) != 0;
}
#endif

static bool nan_peer_nik_cached_cb(uint8_t *peer_mac)
{
    bool cached = false;
    struct peer_svc_info *peer;

    if (!peer_mac) {
        return false;
    }

    NAN_DATA_LOCK();
    peer = nan_find_peer_svc(0, 0, peer_mac);
#if defined(CONFIG_ESP_WIFI_NAN_SECURITY)
    if (peer) {
        struct own_svc_info *own = nan_find_own_svc(peer->own_svc_id);

        for (uint8_t i = 0; i < s_nan_ctx.num_peer_creds; i++) {
            if (!s_nan_ctx.peer_creds[i].is_valid) {
                continue;
            }
            if (!nan_peer_cred_npk_present(&s_nan_ctx.peer_creds[i])) {
                continue;
            }
            if (own && memcmp(s_nan_ctx.peer_creds[i].service_hash, own->svc_hash, 6) == 0) {
                cached = true;
                break;
            }
        }
    }
    if (!cached) {
        uint8_t npk_slots = 0;

        for (uint8_t i = 0; i < s_nan_ctx.num_peer_creds; i++) {
            if (s_nan_ctx.peer_creds[i].is_valid &&
                    nan_peer_cred_npk_present(&s_nan_ctx.peer_creds[i])) {
                npk_slots++;
            }
        }
        if (npk_slots == 1) {
            cached = true;
        }
    }
#else
    if (peer && peer->has_nik) {
        cached = true;
    }
#endif
    NAN_DATA_UNLOCK();

    return cached;
}
#endif

void esp_nan_action_start(esp_netif_t *nan_netif)
{
    nan_set_app_default_handlers();

    NAN_DATA_LOCK();
    s_nan_ctx.nan_netif = nan_netif;
    s_nan_ctx.state = NAN_STARTED_BIT;
    NAN_DATA_UNLOCK();

    struct nan_sync_callbacks nan_cb = {
        .service_match = nan_app_service_match_cb,
        .replied = nan_app_replied_cb,
        .receive = nan_app_receive_cb,
        .ndp_indication = nan_app_ndp_indication_cb,
        .ndp_confirm = nan_app_ndp_confirm_cb,
        .ndp_terminated = nan_app_ndp_terminated_cb,
        .action_txdone = nan_action_txdone_cb,
        .ndp_response_indication = nan_app_ndp_response_indication_cb,
#ifdef CONFIG_ESP_WIFI_NAN_PAIRING
        .get_nira_len = esp_nan_get_nira_len,
        .construct_nira = esp_nan_construct_nira,
        .verify_nira = esp_nan_verify_nira,
        .peer_nik_cached = nan_peer_nik_cached_cb,
        .receive_pasn = handle_auth_pasn,
#endif
    };
    esp_nan_internal_register_callbacks(&nan_cb);

#ifdef CONFIG_ESP_WIFI_NAN_SECURITY
    /* s_nan_ctx.group_mgmt_prot (device-global IGTKSA/BIGTKSA, one per NMI) was
     * captured from the user's start config in esp_wifi_nan_sync_start().
     * Install the device-global IGTK/BIGTK for TX now (when enabled) so Beacons
     * (BIGTK) and group-addressed SDFs (IGTK) are BIP-protected from the first
     * frame, like iOS. The blob gates beacon BIP-TX on an active BIGTK index
     * only (no NDP state), so installing here is sufficient. */
    nan_security_install_own_group_integrity_keys();
#endif

    ESP_LOGI(TAG, "NAN Discovery started.");
    os_event_group_clear_bits(nan_event_group, NAN_STOPPED_BIT);
    os_event_group_set_bits(nan_event_group, NAN_STARTED_BIT);
}

void esp_nan_action_stop(void)
{
	nan_clear_app_default_handlers();

    NAN_DATA_LOCK();
    if (nan_is_datapath_active()) {
        nan_reset_ndl(0, true);
        esp_wifi_internal_reg_rxcb(WIFI_IF_NAN, NULL);
    }

    nan_reset_service(0, true);
    s_nan_ctx.state &= ~NAN_STARTED_BIT;
    s_nan_ctx.state |= NAN_STOPPED_BIT;
    NAN_DATA_UNLOCK();

#if defined(CONFIG_ESP_WIFI_NAN_PAIRING) && defined(CONFIG_ESP_WIFI_NAN_SECURITY)
    /* ND-PMKs are bound to the local NMI used at PASN time; a later
     * sync_start may use a different (randomized) NMI, which would
     * invalidate every cached derivation. */
    nan_app_clear_paired_peers();
#endif

#ifdef CONFIG_ESP_WIFI_NAN_SECURITY
    /* Drop the device-global IGTK/BIGTK so the next start regenerates fresh
     * keys instead of re-installing a stale key with IPN/BIPN=0 (which would
     * reset the blob's replay counter) — see nan_security_reset_own_group_keys. */
    nan_security_reset_own_group_keys();
#endif

    esp_nan_internal_register_callbacks(NULL);
    os_event_group_clear_bits(nan_event_group, NAN_STARTED_BIT);
    os_event_group_set_bits(nan_event_group, NAN_STOPPED_BIT);
}

esp_err_t esp_wifi_nan_sync_start(const wifi_nan_sync_config_t *nan_cfg)
{
    wifi_mode_t mode;
    esp_err_t ret;

    if (!nan_cfg) {
        ESP_LOGE(TAG, "NAN start config is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    ret = esp_wifi_get_mode(&mode);
    if (ret == ESP_ERR_WIFI_NOT_INIT) {
        ESP_LOGE(TAG, "WiFi not initialised!");
        return ret;
    } else if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Unable to get mode");
        return ret;
    }

    /* XXX: For now, NAN-USD and NAN-Sync can not coexist. */
    /* NAN-Synchronization Only */
    wifi_config_t config = {0};
    if (!s_nan_data_lock) {
        ESP_LOGE(TAG, "NAN Data lock doesn't exist");
        return ESP_FAIL;
    }

    NAN_DATA_LOCK();
    if (s_nan_ctx.state & NAN_STARTED_BIT) {
        ESP_LOGI(TAG, "NAN already started");
        NAN_DATA_UNLOCK();
        return ESP_OK;
    }
#ifdef CONFIG_ESP_WIFI_NAN_SECURITY
    s_nan_ctx.own_nik_valid = false;
    s_nan_ctx.num_peer_creds = 0;
    memset(s_nan_ctx.peer_creds, 0, sizeof(s_nan_ctx.peer_creds));
    s_nan_ctx.use_nvs_for_caching = nan_cfg->use_nvs_for_caching;
    s_nan_ctx.group_mgmt_prot = nan_cfg->group_mgmt_prot;
    s_nan_ctx.nik_lifetime = 0;

    if (nan_cfg->reset_current_nvs_creds) {
        /* Start from a clean slate: drop every credential persisted in NVS. */
        ret = esp_wifi_nan_erase_all_creds();
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to erase NAN credentials from NVS");
            NAN_DATA_UNLOCK();
            return ret;
        }
    } else if (esp_wifi_nan_load_saved_creds(s_nan_ctx.own_nik, &s_nan_ctx.own_nik_valid,
                                             s_nan_ctx.peer_creds, &s_nan_ctx.num_peer_creds) != ESP_OK) {
        ESP_LOGW(TAG, "Failed to load saved NAN credentials");
        s_nan_ctx.own_nik_valid = false;
        s_nan_ctx.num_peer_creds = 0;
    }

    if (!s_nan_ctx.own_nik_valid) {
        if (os_get_random(s_nan_ctx.own_nik, ESP_WIFI_NAN_NIK_LEN) != 0) {
            NAN_DATA_UNLOCK();
            ESP_LOGE(TAG, "Failed to generate NAN NIK");
            return ESP_FAIL;
        }
        s_nan_ctx.own_nik_valid = true;
        /* Persist the freshly generated NIK only when NVS caching is enabled;
         * otherwise the identity stays ephemeral for this session. */
        if (s_nan_ctx.use_nvs_for_caching) {
            ret = esp_wifi_nan_save_own_nik(s_nan_ctx.own_nik);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Failed to persist own NIK to NVS");
                NAN_DATA_UNLOCK();
                return ret;
            }
        }
    }
    /* Drop the cached NIRA tag; it was derived from the previous NIK. */
    s_nan_ctx.nira_cached = false;
#endif
    NAN_DATA_UNLOCK();

    ESP_RETURN_ON_ERROR(esp_wifi_set_mode(WIFI_MODE_NAN), TAG, "Set mode NAN failed");

    memcpy(&config.nan, nan_cfg, sizeof(wifi_nan_sync_config_t));
    ESP_RETURN_ON_ERROR(esp_wifi_set_config(WIFI_IF_NAN, &config), TAG, "Setting NAN config failed");

    os_event_group_clear_bits(nan_event_group, NAN_STARTED_BIT);
    if (esp_wifi_start() != ESP_OK) {
        ESP_LOGE(TAG, "Starting wifi failed");
        NAN_DATA_LOCK();
        s_nan_ctx.nan_netif = NULL;
        NAN_DATA_UNLOCK();
        return ESP_FAIL;
    }

    EventBits_t bits = os_event_group_wait_bits(nan_event_group, NAN_STARTED_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
    if (!(bits & NAN_STARTED_BIT)) {
        NAN_DATA_LOCK();
        s_nan_ctx.nan_netif = NULL;
        NAN_DATA_UNLOCK();
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t esp_wifi_nan_sync_stop(void)
{
    NAN_DATA_LOCK();
    if (!(s_nan_ctx.state & NAN_STARTED_BIT)) {
        ESP_LOGE(TAG, "NAN isn't started");
        NAN_DATA_UNLOCK();
        return ESP_FAIL;
    }

    if (nan_is_datapath_active()) {
        /* Terminate all NDP's */
        wifi_nan_datapath_end_req_t ndp_end = {0};
        for (int i=0; i < ESP_WIFI_NAN_DATAPATH_MAX_PEERS; i++) {
             if (s_nan_ctx.ndl[i].ndp_id) {
                 MACADDR_COPY(ndp_end.peer_mac, s_nan_ctx.ndl[i].peer_nmi);
                 ndp_end.ndp_id = s_nan_ctx.ndl[i].ndp_id;
                 esp_nan_internal_datapath_end(&ndp_end);
             }
        }
        nan_reset_ndl(0, true);

        NAN_DATA_UNLOCK();
        /* Wait for 1 NAN DW interval (512 TU's ~= 524 mSec) for successful termination */
        g_wifi_osi_funcs._task_delay(NAN_DW_INTVL_MS/portTICK_PERIOD_MS);
    } else {
        NAN_DATA_UNLOCK();
    }

    /* Wait for a fresh stop event, not a stale bit from prior run. */
    os_event_group_clear_bits(nan_event_group, NAN_STOPPED_BIT);
    ESP_RETURN_ON_ERROR(esp_wifi_stop(), TAG, "Stopping NAN failed");

    EventBits_t bits = os_event_group_wait_bits(nan_event_group, NAN_STOPPED_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
    if (!(bits & NAN_STOPPED_BIT)) {
        return ESP_FAIL;
    }

    NAN_DATA_LOCK();
    /* Free per-peer linked lists before zeroing own_svc[] heads, else the
     * peer_svc_info heap allocations leak. */
    nan_reset_service(0, true);
    nan_reset_ndl(0, true);
    memset(&s_nan_ctx, 0, sizeof(nan_ctx_t));
    NAN_DATA_UNLOCK();
    return ESP_OK;
}
#endif /* CONFIG_ESP_WIFI_NAN_SYNC_ENABLE */

#ifdef CONFIG_ESP_WIFI_NAN_PAIRING
static bool nan_check_paired_service_hash(uint8_t service_hash[6])
{
    for (uint8_t i = 0; i < s_nan_ctx.num_peer_creds; i++) {
        if (s_nan_ctx.peer_creds[i].is_valid &&
                os_memcmp(s_nan_ctx.peer_creds[i].service_hash, service_hash, 6) == 0) {
            return true;
        }
    }
    return false;
}
#endif

uint8_t esp_wifi_nan_publish_service(const wifi_nan_publish_cfg_t *publish_cfg)
{
    int pub_id = 0;

    if (publish_cfg->usd_discovery_flag && !s_usd_in_progress) {
        ESP_LOGE(TAG, "Can not start Publish function with USD Discovery "
                "when USD is not enabled");
        return 0;
    }

    if (!publish_cfg->usd_discovery_flag && s_usd_in_progress) {
        // XXX Add support for NAN-Sync and NAN-USD concurrent discovery
        ESP_LOGE(TAG, "Can not start Publish function with "
                "NAN-Synchronization Discovery when USD is enabled");
        return 0;
    }

    if ((publish_cfg->ssi_len && publish_cfg->ssi == NULL) ||
        (publish_cfg->ssi &&
         (!publish_cfg->ssi_len ||
          publish_cfg->ssi_len > ESP_WIFI_MAX_SVC_SSI_LEN))) {
        ESP_LOGE(TAG, "Configured ssi and ssi_len(%d) incorrect",
                publish_cfg->ssi_len);
        return 0;
    }

    if (publish_cfg->ssi &&
            !memcmp(publish_cfg->ssi, s_wfa_oui, sizeof(s_wfa_oui))) {
        /* WFA defined Service Specific Info */
        wifi_nan_wfa_ssi_t *wfa_ssi = (wifi_nan_wfa_ssi_t *)publish_cfg->ssi;
        if (wfa_ssi->proto >= WIFI_SVC_PROTO_MAX) {
            ESP_LOGI(TAG, "Unrecognized WFA Defined SSI protocol (%d)",
                    wfa_ssi->proto);
        }
    }

#ifdef CONFIG_ESP_WIFI_NAN_PAIRING
    {
        uint16_t bootstrap_methods = publish_cfg->pairing ?
                                     publish_cfg->pairing->bootstrapping_methods : 0;

        if (!nan_pairing_validate_publish_bootstrapping(bootstrap_methods)) {
            ESP_LOGE(TAG, "Invalid bootstrapping methods for Publisher. Only OPPORTUNISTIC and PIN_CODE_DISPLAY allowed");
            return 0;
        }
    }
#endif /* CONFIG_ESP_WIFI_NAN_PAIRING */

#ifdef CONFIG_ESP_WIFI_NAN_USD_ENABLE
    if (s_usd_in_progress) {
        if ((pub_id = esp_nan_usd_publish(publish_cfg)) == -1) {
            ESP_LOGE(TAG, "Failed to publish service '%s'",
                    publish_cfg->service_name);
            return 0;
        }
        ESP_LOGI(TAG, "Started Publishing %s [Service ID - %u]",
                publish_cfg->service_name, pub_id);
        return pub_id;
    }
#endif /* CONFIG_ESP_WIFI_NAN_USD_ENABLE */

#ifdef CONFIG_ESP_WIFI_NAN_SYNC_ENABLE
    wifi_nan_publish_cfg_t *cfg = NULL;
    uint8_t service_id[6] = {0};

    NAN_DATA_LOCK();
    if (!(s_nan_ctx.state & NAN_STARTED_BIT)) {
        ESP_LOGE(TAG, "NAN not started!");
        goto fail;
    }

    if (nan_services_limit_reached()) {
        ESP_LOGE(TAG, "Maximum services limit reached");
        goto fail;
    }

    if (nan_find_own_svc_by_name(publish_cfg->service_name)) {
        ESP_LOGE(TAG, "Service name %s already used!",
                publish_cfg->service_name);
        goto fail;
    }

    if (publish_cfg->security_reqd) {
#ifndef CONFIG_ESP_WIFI_NAN_SECURITY
        ESP_LOGE(TAG, "Encrypted datapath not enabled (CONFIG_ESP_WIFI_NAN_SECURITY)");
        goto fail;
#else
        if (!publish_cfg->security_cfg) {
            ESP_LOGE(TAG, "security_reqd set but security_cfg is NULL");
            goto fail;
        }
        if (publish_cfg->security_cfg->num_credentials == 0 ||
            publish_cfg->security_cfg->num_credentials > ESP_WIFI_NAN_MAX_CREDS_PER_SVC) {
            ESP_LOGE(TAG, "security_cfg.num_credentials=%u must be 1..%u",
                     publish_cfg->security_cfg->num_credentials,
                     ESP_WIFI_NAN_MAX_CREDS_PER_SVC);
            goto fail;
        }
        for (uint8_t i = 0; i < publish_cfg->security_cfg->num_credentials; i++) {
            uint8_t csid = publish_cfg->security_cfg->creds[i].csid;
            if (csid != WIFI_NAN_CSID_NCS_SK_128
#if CONFIG_ESP_WIFI_NAN_PAIRING
                    && csid != WIFI_NAN_CSID_NCS_PK_PASN_128
#endif
                    ) {
                ESP_LOGE(TAG, "creds[%u].csid=%u unsupported cipher", i, csid);
                goto fail;
            }
        }
#endif
    }

    /* Heap-allocate config copy to avoid ~700 bytes on stack */
    cfg = os_zalloc(sizeof(*cfg));
    if (!cfg) {
        ESP_LOGE(TAG, "Failed to allocate publish config");
        goto fail;
    }
    memcpy(cfg, publish_cfg, sizeof(*cfg));
    cfg->pairing = NULL;
    if (publish_cfg->pairing) {
        cfg->pairing = os_malloc(sizeof(*cfg->pairing));
        if (!cfg->pairing) {
            ESP_LOGE(TAG, "Failed to copy pairing config");
            goto fail;
        }
        memcpy(cfg->pairing, publish_cfg->pairing, sizeof(*cfg->pairing));
    }

    /* Pre-claim host slot BEFORE calling into the blob. Reason: the blob
     * invokes our derive_security_params callback on the WiFi task and looks
     * up the host's slot by service name to mirror derived ND-PMK/ND-PMKID
     * into p_svc->derived_security[]. Doing the derive on WiFi task (not the
     * app/main task) keeps PBKDF2's hardware-SHA polling off IDLE0 and avoids
     * tripping the task watchdog when num_credentials > 1. */
    if (!nan_compute_service_id(publish_cfg->service_name, service_id)) {
        ESP_LOGE(TAG, "Failed to compute Service ID for %s", publish_cfg->service_name);
        goto fail;
    }

#ifdef CONFIG_ESP_WIFI_NAN_PAIRING
    if (cfg->pairing && nan_check_paired_service_hash(service_id)) {
        cfg->pairing->pairing_setup = false;
    }
#endif

    if (!nan_claim_own_svc_slot(ESP_NAN_PUBLISH, publish_cfg->service_name,
#ifdef CONFIG_ESP_WIFI_NAN_SECURITY
                                cfg->security_cfg,
#ifdef CONFIG_ESP_WIFI_NAN_PAIRING
                                cfg->pairing
#else
                                NULL
#endif
#else
                                NULL, NULL
#endif
                               )) {
        ESP_LOGE(TAG, "No free service slot");
        goto fail;
    }

    if (esp_nan_internal_publish_service(cfg, (uint8_t *) &pub_id, false) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to publish service '%s'", publish_cfg->service_name);
        nan_abort_own_svc(publish_cfg->service_name);
        goto fail;
    }

    ESP_LOGI(TAG, "Started Publishing %s [Service ID - %u]", publish_cfg->service_name, pub_id);
    nan_finalize_own_svc(publish_cfg->service_name, pub_id, publish_cfg->ndp_resp_needed, service_id);
    if (cfg->pairing) {
        os_free(cfg->pairing);
    }
    os_free(cfg);
    NAN_DATA_UNLOCK();

    return pub_id;
fail:
    if (cfg) {
        if (cfg->pairing) {
            os_free(cfg->pairing);
        }
        os_free(cfg);
    }
    NAN_DATA_UNLOCK();
    return 0;
#endif /* CONFIG_ESP_WIFI_NAN_SYNC_ENABLE */
    return 0;
}

uint8_t esp_wifi_nan_subscribe_service(const wifi_nan_subscribe_cfg_t *subscribe_cfg)
{
    int sub_id = 0;

    if (subscribe_cfg->usd_discovery_flag && !s_usd_in_progress) {
        ESP_LOGE(TAG, "Can not start Subscribe function with USD Discovery "
                "when USD is not enabled");
        return 0;
    }

    if (!subscribe_cfg->usd_discovery_flag && s_usd_in_progress) {
        // XXX Add support for NAN-Sync and NAN-USD concurrent discovery
        ESP_LOGE(TAG, "Can not start Subscribe function with "
                "NAN-Synchronization Discovery when USD is enabled");
        return 0;
    }

    if ((subscribe_cfg->ssi_len && subscribe_cfg->ssi == NULL) ||
        (subscribe_cfg->ssi && (!subscribe_cfg->ssi_len || subscribe_cfg->ssi_len > ESP_WIFI_MAX_SVC_SSI_LEN))) {
        ESP_LOGE(TAG, "Configured ssi and ssi_len(%d) incorrect", subscribe_cfg->ssi_len);
        return 0;
    }

    if (subscribe_cfg->ssi && !memcmp(subscribe_cfg->ssi, s_wfa_oui, sizeof(s_wfa_oui))) {
        /* WFA defined Service Specific Info */
        wifi_nan_wfa_ssi_t *wfa_ssi = (wifi_nan_wfa_ssi_t *)subscribe_cfg->ssi;
        if (wfa_ssi->proto >= WIFI_SVC_PROTO_MAX) {
            ESP_LOGI(TAG, "Unrecognized WFA Defined SSI protocol (%d)", wfa_ssi->proto);
        }
    }

#ifdef CONFIG_ESP_WIFI_NAN_PAIRING
    {
        uint16_t bootstrap_methods = subscribe_cfg->pairing ?
                                     subscribe_cfg->pairing->bootstrapping_methods : 0;

        if (!nan_pairing_validate_subscribe_bootstrapping(bootstrap_methods)) {
            ESP_LOGE(TAG, "Invalid bootstrapping methods for Subscriber. Only OPPORTUNISTIC and PIN_CODE_KEYPAD allowed");
            return 0;
        }
    }
#endif /* CONFIG_ESP_WIFI_NAN_PAIRING */

#ifdef CONFIG_ESP_WIFI_NAN_USD_ENABLE
    if (s_usd_in_progress) {
        if ((sub_id = esp_nan_usd_subscribe(subscribe_cfg)) == -1) {
            ESP_LOGE(TAG, "Failed to subscribe to service '%s'",
                    subscribe_cfg->service_name);
            return 0;
        }
        ESP_LOGI(TAG, "Started Subscribing to %s [Service ID - %u]",
                subscribe_cfg->service_name, sub_id);
        return sub_id;
    }
#endif /* CONFIG_ESP_WIFI_NAN_USD_ENABLE */

#ifdef CONFIG_ESP_WIFI_NAN_SYNC_ENABLE
    wifi_nan_subscribe_cfg_t *cfg = NULL;
    uint8_t service_id[6] = {0};

    if (subscribe_cfg->security_reqd) {
#ifndef CONFIG_ESP_WIFI_NAN_SECURITY
        ESP_LOGE(TAG, "Encrypted datapath not enabled (CONFIG_ESP_WIFI_NAN_SECURITY)");
        return 0;
#else
        if (!subscribe_cfg->security_cfg) {
            ESP_LOGE(TAG, "security_reqd set but security_cfg is NULL");
            return 0;
        }
        if (subscribe_cfg->security_cfg->num_credentials == 0 ||
            subscribe_cfg->security_cfg->num_credentials > ESP_WIFI_NAN_MAX_CREDS_PER_SVC) {
            ESP_LOGE(TAG, "security_cfg.num_credentials=%u must be 1..%u",
                     subscribe_cfg->security_cfg->num_credentials,
                     ESP_WIFI_NAN_MAX_CREDS_PER_SVC);
            return 0;
        }
        for (uint8_t i = 0; i < subscribe_cfg->security_cfg->num_credentials; i++) {
            uint8_t csid = subscribe_cfg->security_cfg->creds[i].csid;
            if (csid != WIFI_NAN_CSID_NCS_SK_128
#if CONFIG_ESP_WIFI_NAN_PAIRING
                    && csid != WIFI_NAN_CSID_NCS_PK_PASN_128
#endif
                    ) {
                ESP_LOGE(TAG, "creds[%u].csid=%u unsupported cipher", i, csid);
                return 0;
            }
        }
#endif
    }
    NAN_DATA_LOCK();
    if (!(s_nan_ctx.state & NAN_STARTED_BIT)) {
        ESP_LOGE(TAG, "NAN not started!");
        goto fail;
    }
    if (nan_services_limit_reached()) {
        ESP_LOGE(TAG, "Maximum services limit reached");
        goto fail;
    }

    if (nan_find_own_svc_by_name(subscribe_cfg->service_name)) {
        ESP_LOGE(TAG, "Service name already used!");
        goto fail;
    }

    cfg = os_zalloc(sizeof(*cfg));
    if (!cfg) {
        ESP_LOGE(TAG, "Failed to allocate subscribe config");
        goto fail;
    }
    memcpy(cfg, subscribe_cfg, sizeof(*cfg));
    cfg->pairing = NULL;
    if (subscribe_cfg->pairing) {
        cfg->pairing = os_malloc(sizeof(*cfg->pairing));
        if (!cfg->pairing) {
            ESP_LOGE(TAG, "Failed to copy pairing config");
            goto fail;
        }
        memcpy(cfg->pairing, subscribe_cfg->pairing, sizeof(*cfg->pairing));
    }

    /* Pre-claim host slot BEFORE the blob's subscribe call; see comment on
     * the publish path for the watchdog rationale. */

    if (!nan_compute_service_id(cfg->service_name, service_id)) {
        ESP_LOGE(TAG, "Failed to compute Service ID for %s", cfg->service_name);
        goto fail;
    }

#ifdef CONFIG_ESP_WIFI_NAN_PAIRING
    if (cfg->pairing && nan_check_paired_service_hash(service_id)) {
        cfg->pairing->pairing_setup = false;
    }
#endif

    if (!nan_claim_own_svc_slot(ESP_NAN_SUBSCRIBE, cfg->service_name,
                                cfg->security_cfg, cfg->pairing)) {
        ESP_LOGE(TAG, "No free service slot");
        goto fail;
    }

    if (esp_nan_internal_subscribe_service(cfg, (uint8_t *) &sub_id, false) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to subscribe to service '%s'", cfg->service_name);
        nan_abort_own_svc(cfg->service_name);
        goto fail;
    }

    ESP_LOGI(TAG, "Started Subscribing to %s [Service ID - %u]", cfg->service_name, sub_id);
    nan_finalize_own_svc(cfg->service_name, (uint8_t) sub_id, false, service_id);
    if (cfg->pairing) {
        os_free(cfg->pairing);
    }
    os_free(cfg);
    NAN_DATA_UNLOCK();

    return sub_id;
fail:
    if (cfg) {
        if (cfg->pairing) {
            os_free(cfg->pairing);
        }
        os_free(cfg);
    }
    NAN_DATA_UNLOCK();
    return 0;
#endif /* CONFIG_ESP_WIFI_NAN_SYNC_ENABLE */
    return 0;
}

esp_err_t esp_wifi_nan_send_message(wifi_nan_followup_params_t *fup_params)
{
    esp_err_t ret = ESP_OK;

    if ((fup_params->ssi_len && fup_params->ssi == NULL) ||
        (fup_params->ssi &&
         (!fup_params->ssi_len ||
          fup_params->ssi_len > ESP_WIFI_MAX_FUP_SSI_LEN))) {
        ESP_LOGE(TAG, "Configured ssi and ssi_len(%d) incorrect",
                fup_params->ssi_len);
        return ESP_FAIL;
    }

    if (fup_params->ssi &&
            !memcmp(fup_params->ssi, s_wfa_oui, sizeof(s_wfa_oui))) {
        /* WFA defined Service Specific Info */
        wifi_nan_wfa_ssi_t *wfa_ssi = (wifi_nan_wfa_ssi_t *)fup_params->ssi;
        if (wfa_ssi->proto >= WIFI_SVC_PROTO_MAX) {
            ESP_LOGI(TAG, "Unrecognized WFA Defined SSI protocol (%d)",
                    wfa_ssi->proto);
        }
    }

#ifdef CONFIG_ESP_WIFI_NAN_USD_ENABLE
    if (s_usd_in_progress) {
        if (esp_nan_usd_transmit(fup_params->inst_id, fup_params->ssi,
                    fup_params->ssi_len, fup_params->peer_mac,
                    fup_params->peer_inst_id) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to send Follow-up message!");
            return ESP_FAIL;
        }
        return ESP_OK;
    }
#endif /* CONFIG_ESP_WIFI_NAN_USD_ENABLE */

#ifdef CONFIG_ESP_WIFI_NAN_SYNC_ENABLE
    struct peer_svc_info *p_peer_svc;
    NAN_DATA_LOCK();
    p_peer_svc = nan_find_peer_svc(fup_params->inst_id,
            fup_params->peer_inst_id, fup_params->peer_mac);
    if (!p_peer_svc) {
        ESP_LOGE(TAG, "Cannot send Follow-up, peer not found!");
        NAN_DATA_UNLOCK();
        return ESP_FAIL;
    }

    if (!fup_params->inst_id) {
        fup_params->inst_id = p_peer_svc->own_svc_id;
    }
    if (!fup_params->peer_inst_id) {
        fup_params->peer_inst_id = p_peer_svc->svc_id;
    }
    if (MACADDR_EQUAL(fup_params->peer_mac, null_mac)) {
        MACADDR_COPY(fup_params->peer_mac, p_peer_svc->peer_nmi);
    }

    NAN_DATA_UNLOCK();
    os_event_group_clear_bits(nan_event_group, NAN_TX_SUCCESS | NAN_TX_FAILURE);
    if ((ret = esp_nan_internal_send_followup(fup_params, &s_fup_context, NULL)) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to send Follow-up message!");
        return ret;
    }

    EventBits_t bits = os_event_group_wait_bits(nan_event_group, NAN_TX_SUCCESS | NAN_TX_FAILURE, pdFALSE, pdFALSE, pdMS_TO_TICKS(NAN_ACTION_TIMEOUT));
    if (bits & NAN_TX_SUCCESS) {
        if (fup_params->ssi) {
            ESP_LOGD(TAG, "Sent below payload in Follow-up message to Peer "MACSTR" with Service ID %d",
                          MAC2STR(fup_params->peer_mac), fup_params->peer_inst_id);
            ESP_LOG_BUFFER_HEXDUMP(TAG, fup_params->ssi, fup_params->ssi_len, ESP_LOG_DEBUG);
        } else {
            ESP_LOGI(TAG, "Sent message to Peer "MACSTR" with Service ID %d",
                    MAC2STR(fup_params->peer_mac), fup_params->peer_inst_id);
        }
        ret = ESP_OK;
    } else if (bits & NAN_TX_FAILURE) {
        ESP_LOGE(TAG, "Failed to send Follow-up message!");
        ret = ESP_FAIL;
    } else {
        ESP_LOGE(TAG, "Timeout, failed to send Follow-up message!");
        ret = ESP_FAIL;
    }
#endif /* CONFIG_ESP_WIFI_NAN_SYNC_ENABLE */

    return ret;
}

esp_err_t esp_wifi_nan_cancel_service(uint8_t service_id)
{
#ifdef CONFIG_ESP_WIFI_NAN_USD_ENABLE
    if (s_usd_in_progress) {
        return esp_nan_usd_cancel_service(service_id);
    }
#endif /* CONFIG_ESP_WIFI_NAN_USD_ENABLE */
#ifdef CONFIG_ESP_WIFI_NAN_SYNC_ENABLE
#if CONFIG_ESP_WIFI_NAN_PAIRING
    /* Snapshot peer NMIs before cancel; clear TKs only after a successful cancel
     * so a failed attempt does not leave an active service without keys. */
    uint8_t peer_nmis[NAN_MAX_PEERS_RECORD][MACADDR_LEN];
    int peer_count = 0;
#endif
    NAN_DATA_LOCK();
    struct own_svc_info *p_own_svc = nan_find_own_svc(service_id);

    if (!p_own_svc) {
        ESP_LOGE(TAG, "Cannot find own service with id %d!", service_id);
        goto fail;
    }

#if CONFIG_ESP_WIFI_NAN_PAIRING
    {
        struct peer_svc_info *temp;

        SLIST_FOREACH(temp, &(p_own_svc->peer_list), next) {
            bool dup = false;

            for (int i = 0; i < peer_count; i++) {
                if (MACADDR_EQUAL(peer_nmis[i], temp->peer_nmi)) {
                    dup = true;
                    break;
                }
            }
            if (!dup && peer_count < NAN_MAX_PEERS_RECORD) {
                MACADDR_COPY(peer_nmis[peer_count], temp->peer_nmi);
                peer_count++;
            }
        }
    }
#endif

    if (p_own_svc->type == ESP_NAN_PUBLISH) {
        if (esp_nan_internal_publish_service(NULL, &service_id, true) == ESP_OK) {
            nan_reset_service(service_id, false);
            ESP_LOGI(TAG, "Cancelled Publish with Service ID %d", service_id);
            goto done;
        }
    }

    if (p_own_svc->type == ESP_NAN_SUBSCRIBE) {
        if (esp_nan_internal_subscribe_service(NULL, &service_id, true) == ESP_OK) {
            nan_reset_service(service_id, false);
            ESP_LOGI(TAG, "Cancelled Subscribe with Service ID %d", service_id);
            goto done;
        }
    }

fail:
    NAN_DATA_UNLOCK();
    return ESP_FAIL;

done:
    NAN_DATA_UNLOCK();
#if CONFIG_ESP_WIFI_NAN_PAIRING
    /* Cancel succeeded; now safe to wipe pairwise keys for the collected peers. */
    for (int i = 0; i < peer_count; i++) {
        nan_app_clear_one_peer_tks(peer_nmis[i]);
    }
#endif
    return ESP_OK;
#endif /* CONFIG_ESP_WIFI_NAN_SYNC_ENABLE */
    return ESP_FAIL;
}

#ifdef CONFIG_ESP_WIFI_NAN_SYNC_ENABLE
uint8_t esp_wifi_nan_datapath_req(wifi_nan_datapath_req_t *req)
{
    uint8_t ndp_id = 0;
    uint8_t own_bssid[6];
    ip_addr_t own_ipv6 = {0};

    NAN_DATA_LOCK();
    struct peer_svc_info *p_peer_svc = nan_find_peer_svc(0, req->pub_id, req->peer_mac);

    if (!p_peer_svc) {
        ESP_LOGE(TAG, "Cannot send NDP Req, peer not found!");
        goto fail;
    }

    if (p_peer_svc->device_caps & NAN_CAPS_NDPE_ATTR) {
        esp_err_t err = esp_wifi_get_mac(WIFI_IF_NAN, own_bssid);

        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Cannot get own BSSID!");
            goto fail;
        }
        esp_wifi_nan_get_ipv6_linklocal_from_mac(&own_ipv6.u_addr.ip6, own_bssid);
    }

    if (req->pub_id == 0)
        req->pub_id = p_peer_svc->svc_id;

    if (p_peer_svc->type != ESP_NAN_PUBLISH) {
        ESP_LOGE(TAG, "Only subscriber can send an NDP Req to a Publisher");
        goto fail;
    }
    if (ndl_limit_reached()) {
        ESP_LOGE(TAG, "Cannot establish new datapath, limit reached!");
        goto fail;
    }

    if (MACADDR_EQUAL(req->peer_mac, null_mac)) {
        MACADDR_COPY(req->peer_mac, p_peer_svc->peer_nmi);
    }

    os_event_group_clear_bits(nan_event_group, NDP_ACCEPTED | NDP_REJECTED);

    uint32_t saved_pub_id      = req->pub_id;
    uint32_t saved_device_caps = p_peer_svc->device_caps;
#ifdef CONFIG_ESP_WIFI_NAN_SECURITY
    /* Encrypted datapath: pre-claim an NDL at ndp_id=0 with security_ctx
     * populated from the subscribe-time security_cfg. The WiFi library's M1 builder
     * callbacks (construct_scia_ndp_req / get_ndp_req_shared_key_desc /
     * capture_m1_auth_token) fire inside esp_nan_internal_datapath_req()
     * before it returns the real ndp_id, and they look up the NDL by
     * (ndp_id=0, peer_nmi). Open datapath skips the pre-claim entirely --
     * no security callbacks fire, so there is nothing to look up. */
    struct ndl_info *preclaimed = nan_ndl_preclaim_initiator(saved_pub_id,
                                                             req->peer_mac,
                                                             saved_device_caps);
    if (preclaimed) {
        /* p_peer_svc carries the per-(svc, peer) cred index remembered at SDF
         * RX; own_svc supplies svc_name + creds[] for this specific subscribe. */
        struct own_svc_info *p_own_svc_init = nan_find_own_svc(p_peer_svc->own_svc_id);
        nan_security_populate_initiator_ndl(preclaimed, p_own_svc_init, p_peer_svc,
                                            p_peer_svc->peer_nmi);
    }
#endif

    /* Release lock before internal call: the WiFi library may invoke
     * esp_nan_get_ndp_req_shared_key_desc / esp_nan_construct_scia_ndp_req /
     * esp_nan_capture_m1_auth_token which all take NAN_DATA_LOCK. Holding
     * it here would deadlock those callbacks. */
    NAN_DATA_UNLOCK();

    if (esp_nan_internal_datapath_req(req, &ndp_id, (uint8_t *)&own_ipv6.u_addr.ip6.addr[2]) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initiate NDP req");
#ifdef CONFIG_ESP_WIFI_NAN_SECURITY
        NAN_DATA_LOCK();
        nan_ndl_release(0);  /* clean up pre-claimed NDL */
        NAN_DATA_UNLOCK();
#endif
        return 0;
    }

    NAN_DATA_LOCK();
    nan_ndl_finalize_ndp_id(ndp_id, saved_pub_id, req->peer_mac, saved_device_caps);
    NAN_DATA_UNLOCK();

    ESP_LOGD(TAG, "Requested NDP with "MACSTR" [NDP ID - %d]", MAC2STR(req->peer_mac), ndp_id);

    /* At worst, Secured NDP (M1..M4) needs ~2.1s; 4*DW races the confirm.
     * Pick 6 (~3.1s). Harmless for open NDP - success unblocks instantly;
     * only failure path waits longer. */
    EventBits_t bits = os_event_group_wait_bits(nan_event_group, NDP_ACCEPTED | NDP_REJECTED,
                                                pdFALSE, pdFALSE,
                                                pdMS_TO_TICKS(6 * NAN_DW_INTVL_MS));
    if (bits & NDP_ACCEPTED) {
        return ndp_id;
    } else if (bits & NDP_REJECTED) {
        return 0;
    } else {
        ESP_LOGE(TAG, "NDP request timed out");
        NAN_DATA_LOCK();
        nan_ndl_release(ndp_id);
        NAN_DATA_UNLOCK();
        return 0;
    }
fail:
    NAN_DATA_UNLOCK();
    return 0;
}

esp_err_t esp_wifi_nan_datapath_resp(wifi_nan_datapath_resp_t *resp)
{
    NAN_DATA_LOCK();
    struct ndl_info *ndl = nan_find_ndl(resp->ndp_id, NULL);
    uint8_t own_bssid[6];
    ip_addr_t own_ipv6 = {0};

    if (!ndl) {
        ESP_LOGE(TAG, "No NDL with ndp id %d", resp->ndp_id);
        goto fail;
    }
    if (!(s_nan_ctx.event & NDP_INDICATION)) { //INDICATION of specific peer
        ESP_LOGE(TAG, "Need NDP Indication before NDP Response can be sent");
        goto fail;
    }
    if (MACADDR_EQUAL(resp->peer_mac, null_mac)) {
        MACADDR_COPY(resp->peer_mac, ndl->peer_nmi);
    }

    if (ndl->device_caps & NAN_CAPS_NDPE_ATTR) {
        esp_err_t err = esp_wifi_get_mac(WIFI_IF_NAN, own_bssid);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Cannot get own BSSID!");
            NAN_DATA_UNLOCK();
            goto fail;
        }
        esp_wifi_nan_get_ipv6_linklocal_from_mac(&own_ipv6.u_addr.ip6, own_bssid);
    }

    /* Release lock before internal call: esp_nan_internal_datapath_resp() may call
     * esp_nan_get_ndp_resp_shared_key_desc() which takes NAN_DATA_LOCK again → deadlock if we keep the lock. */
    NAN_DATA_UNLOCK();

    if (esp_nan_internal_datapath_resp(resp, (uint8_t *)&own_ipv6.u_addr.ip6.addr[2]) == ESP_OK) {
        NAN_DATA_LOCK();
        s_nan_ctx.event &= ~NDP_INDICATION;
        NAN_DATA_UNLOCK();
        return ESP_OK;
    }

    return ESP_FAIL;

fail:
    NAN_DATA_UNLOCK();
    return ESP_FAIL;
}

esp_err_t esp_wifi_nan_datapath_end(wifi_nan_datapath_end_req_t *req)
{
    struct ndl_info *ndl = NULL;
    esp_err_t ret;

    NAN_DATA_LOCK();
    if (!nan_is_datapath_active()) {
        ESP_LOGE(TAG, "No Datapath active");
        NAN_DATA_UNLOCK();
        return ESP_FAIL;
    }

    ndl = nan_find_ndl(req->ndp_id, NULL);
    if (!ndl) {
        ESP_LOGE(TAG, "No NDL with ndp id %d", req->ndp_id);
        NAN_DATA_UNLOCK();
        return ESP_FAIL;
    }

    if (MACADDR_EQUAL(req->peer_mac, null_mac)) {
        MACADDR_COPY(req->peer_mac, ndl->peer_nmi);
    }

    NAN_DATA_UNLOCK();
    os_event_group_clear_bits(nan_event_group, NDP_TERMINATED);
    ret = esp_nan_internal_datapath_end(req);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to end NDP with id %d (ret %d)", req->ndp_id, ret);
        return ret;
    }
    EventBits_t bits = os_event_group_wait_bits(nan_event_group, NDP_TERMINATED, pdTRUE, pdFALSE, pdMS_TO_TICKS(NAN_ACTION_TIMEOUT));
    if (!(bits & NDP_TERMINATED)) {
        ESP_LOGE(TAG, "Failed to end NDP with id %d", req->ndp_id);
        return ESP_FAIL;
    }

    return ESP_OK;
}

#ifdef CONFIG_ESP_WIFI_NAN_PAIRING
esp_err_t esp_nan_app_end_peer_datapaths(uint8_t publish_id)
{
    wifi_nan_datapath_end_req_t ndp_end;
    int i;

    NAN_DATA_LOCK();
    for (i = 0; i < ESP_WIFI_NAN_DATAPATH_MAX_PEERS; i++) {
        struct ndl_info *ndl = &s_nan_ctx.ndl[i];

        if (ndl->publisher_id == publish_id) {
            ndp_end.ndp_id = ndl->ndp_id;
            MACADDR_COPY(ndp_end.peer_mac, ndl->peer_nmi);
            break;
        }
    }
    NAN_DATA_UNLOCK();
    if (i == ESP_WIFI_NAN_DATAPATH_MAX_PEERS)
        return ESP_FAIL;

    return  esp_wifi_nan_datapath_end(&ndp_end);
}
#endif /* CONFIG_ESP_WIFI_NAN_PAIRING */

esp_err_t esp_wifi_nan_get_own_svc_info(uint8_t *own_svc_id, char *svc_name, int *num_peer_records)
{
    struct own_svc_info *own_svc = NULL;

    if (!own_svc_id || !num_peer_records || !svc_name) {
        ESP_LOGE(TAG, "NULL memory address for input parameters");
        return ESP_FAIL;
    }

    NAN_DATA_LOCK();
    if (*own_svc_id == 0) {
        own_svc = nan_find_own_svc_by_name(svc_name);
        if (!own_svc) {
            ESP_LOGE(TAG, "No record found for given service name %s", svc_name);
            goto fail;
        }
        *own_svc_id = own_svc->svc_id;
    } else {
        own_svc = nan_find_own_svc(*own_svc_id);
        if (!own_svc) {
            ESP_LOGE(TAG, "No record found for given service ID %d", *own_svc_id);
            goto fail;
        }
        strlcpy(svc_name, own_svc->svc_name, ESP_WIFI_MAX_SVC_NAME_LEN);
    }

    *num_peer_records = own_svc->num_peer_records;
    NAN_DATA_UNLOCK();
    return ESP_OK;

fail:
    NAN_DATA_UNLOCK();
    return ESP_FAIL;
}

esp_err_t esp_wifi_nan_get_peer_records(int *num_peer_records, uint8_t own_svc_id, struct nan_peer_record *peer_record)
{
    struct own_svc_info *own_record = NULL;
    struct peer_svc_info *temp = NULL;
    int peer_num = 0;

    if (!peer_record || !num_peer_records) {
        ESP_LOGE(TAG, "NULL memory address for input parameters");
        return ESP_FAIL;
    }
    if (own_svc_id == 0) {
        ESP_LOGE(TAG, "Invalid service ID");
        return ESP_FAIL;
    }
    if (*num_peer_records == 0) {
        ESP_LOGE(TAG, "Number of peer records provided is 0");
        return ESP_FAIL;
    }

    NAN_DATA_LOCK();
    own_record = nan_find_own_svc(own_svc_id);
    if (own_record) {
        SLIST_FOREACH(temp, &(own_record->peer_list), next) {
            struct ndl_info *p_ndl;
            peer_record[peer_num].peer_svc_id = temp->svc_id;
            peer_record[peer_num].own_svc_id = own_svc_id;
            peer_record[peer_num].peer_svc_type = temp->type;
            MACADDR_COPY(peer_record[peer_num].peer_nmi, temp->peer_nmi);
            p_ndl = nan_find_ndl(0, temp->peer_nmi);
            if (p_ndl) {
                if (p_ndl->own_role == ESP_NAN_PUBLISH) {
                    if (p_ndl->publisher_id == own_svc_id) {
                        peer_record[peer_num].ndp_id = p_ndl->ndp_id;
                        MACADDR_COPY(peer_record[peer_num].peer_ndi, p_ndl->peer_ndi);
                    }
                } else if (p_ndl->own_role == ESP_NAN_SUBSCRIBE) {
                    struct peer_svc_info *peer_info = NULL;
                    peer_info = nan_find_peer_svc(own_svc_id, temp->svc_id, temp->peer_nmi);
                    if (peer_info && peer_info->svc_id == p_ndl->publisher_id) {
                        peer_record[peer_num].ndp_id = p_ndl->ndp_id;
                        MACADDR_COPY(peer_record[peer_num].peer_ndi, p_ndl->peer_ndi);
                    }
                }
            } else {
                peer_record[peer_num].ndp_id = 0;
                MACADDR_COPY(peer_record[peer_num].peer_ndi, null_mac);
            }
            peer_num ++;
            if (peer_num == *num_peer_records) {
                break;
            }
        }
        if (*num_peer_records > peer_num) {
            *num_peer_records = peer_num;
        }

        NAN_DATA_UNLOCK();
        return ESP_OK;
    } else {
        *num_peer_records = 0;
        ESP_LOGD(TAG, "No record found for own service id %d", own_svc_id);
        NAN_DATA_UNLOCK();
        return ESP_FAIL;
    }
}

esp_err_t esp_wifi_nan_get_peer_info(char *svc_name, uint8_t *peer_mac, struct nan_peer_record *peer_info)
{
    struct peer_svc_info *peer_svc = NULL;
    uint8_t own_svc_id = 0;

    if (!peer_mac || !peer_info) {
        ESP_LOGE(TAG, "Invalid memory address for input parameters");
        return ESP_FAIL;
    }

    NAN_DATA_LOCK();
    if (svc_name) {
        struct own_svc_info *own_svc = nan_find_own_svc_by_name(svc_name);
        if (!own_svc) {
            ESP_LOGE(TAG, "No record found for given service name %s", svc_name);
            NAN_DATA_UNLOCK();
            return ESP_FAIL;
        }
        own_svc_id = own_svc->svc_id;
    }

    peer_svc = nan_find_peer_svc(own_svc_id, 0, peer_mac);
    if (peer_svc) {
        struct ndl_info *p_ndl;
        peer_info->peer_svc_id = peer_svc->svc_id;
        peer_info->own_svc_id = peer_svc->own_svc_id;
        peer_info->peer_svc_type = peer_svc->type;
        MACADDR_COPY(peer_info->peer_nmi, peer_mac);
        p_ndl = nan_find_ndl(0, peer_mac);
        if (p_ndl) {
            peer_info->ndp_id = p_ndl->ndp_id;
            MACADDR_COPY(peer_info->peer_ndi, p_ndl->peer_ndi);
        } else {
            peer_info->ndp_id = 0;
            MACADDR_COPY(peer_info->peer_ndi, null_mac);
        }
        NAN_DATA_UNLOCK();
        return ESP_OK;
    } else {
        ESP_LOGD(TAG, "No record found for Peer "MACSTR, MAC2STR(peer_mac));
        NAN_DATA_UNLOCK();
        return ESP_FAIL;
    }
}
#endif /* CONFIG_ESP_WIFI_NAN_SYNC_ENABLE */

#ifdef CONFIG_ESP_WIFI_NAN_USD_ENABLE
esp_err_t esp_wifi_nan_usd_start(void)
{
    if (!s_usd_in_progress) {
        ESP_RETURN_ON_ERROR(esp_wifi_set_mode(WIFI_MODE_STA), "NAN-USD", "Setting STA mode failed");
        ESP_RETURN_ON_ERROR(esp_nan_usd_init(), "NAN-USD", "Failed to initialise NAN USD engine");
        s_usd_in_progress = true;
        ESP_LOGI(TAG, "NaN-USD Started");
        return ESP_OK;
    }
    return ESP_OK;
}

esp_err_t esp_wifi_nan_usd_stop(void)
{
    if (s_usd_in_progress) {
        s_usd_in_progress = false;
        esp_nan_usd_deinit();
        ESP_LOGI(TAG, "NaN-USD Stopped");
        return ESP_OK;
    }
    return ESP_OK;
}

wifi_nan_usd_config_t esp_wifi_usd_get_default_publish_cfg(void)
{
    wifi_country_t country;
    uint8_t start_chan, end_chan;

    wifi_nan_usd_config_t cfg = {
        .usd_default_channel = 6,
        .n_min = 5,
        .n_max = 10,
        .m_min = 5,
        .m_max = 10,
    };

    esp_wifi_get_country(&country);
    start_chan = country.schan;
    end_chan = country.schan + country.nchan - 1;
    for (uint8_t chan = start_chan; chan <= end_chan; chan++) {
        cfg.usd_chan_bitmap.ghz_2_channels |= CHANNEL_TO_BIT(chan);
    }
#if CONFIG_SOC_WIFI_SUPPORT_5G
    cfg.usd_chan_bitmap.ghz_5_channels = country.wifi_5g_channel_mask;
#endif

    return cfg;
}

wifi_nan_usd_config_t esp_wifi_usd_get_default_subscribe_cfg(void)
{
    wifi_country_t country;
    uint8_t start_chan, end_chan;

    wifi_nan_usd_config_t cfg =  {
        .usd_default_channel = 6,
        .n_min = 5,
        .n_max = 10,
        .m_min = 5,
        .m_max = 10
    };

    esp_wifi_get_country(&country);
    start_chan = country.schan;
    end_chan = country.schan + country.nchan - 1;
    for (uint8_t chan = start_chan; chan <= end_chan; chan++) {
        cfg.usd_chan_bitmap.ghz_2_channels |= CHANNEL_TO_BIT(chan);
    }
#if CONFIG_SOC_WIFI_SUPPORT_5G
    cfg.usd_chan_bitmap.ghz_5_channels = country.wifi_5g_channel_mask;
#endif

    return cfg;
}
#endif /* CONFIG_ESP_WIFI_NAN_USD_ENABLE */
