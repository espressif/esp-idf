/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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
#include "os.h"
#include "esp_nan.h"

/* NAN States */
#define NAN_STARTED_BIT     BIT0
#define NAN_STOPPED_BIT     BIT1

/* NAN Events */
#define NDP_INDICATION      BIT2
#define NDP_ACCEPTED        BIT3
#define NDP_TERMINATED      BIT4
#define NDP_REJECTED        BIT5

/* Macros */
#define MACADDR_LEN 6
#define MACADDR_EQUAL(a1, a2)   (memcmp(a1, a2, MACADDR_LEN))
#define MACADDR_COPY(dst, src)  (memcpy(dst, src, MACADDR_LEN))
#define NAN_DW_INTVL_MS 524     /* NAN DW interval (512 TU's ~= 524 mSec) */
#define NAN_NDP_RESP_TIMEOUT_DW 4
#define NAN_NDP_RESP_TIMEOUT    NAN_NDP_RESP_TIMEOUT_DW*NAN_DW_INTVL_MS

/* Global Variables */
static const char *TAG = "nan_app";
static EventGroupHandle_t nan_event_group;
static bool s_app_default_handlers_set = false;
static uint8_t null_mac[MACADDR_LEN] = {0};
static void *s_nan_data_lock = NULL;

#define NAN_DATA_LOCK() os_mutex_lock(s_nan_data_lock)
#define NAN_DATA_UNLOCK() os_mutex_unlock(s_nan_data_lock)

struct peer_svc_info {
    SLIST_ENTRY(peer_svc_info) next;
    uint8_t peer_svc_info[ESP_WIFI_MAX_SVC_INFO_LEN];   /**< Information for followup message */
    uint8_t svc_id;                                     /**< Identifier of peer's service */
    uint8_t own_svc_id;                                 /**< Identifier for own service  */
    uint8_t type;                                       /**< Service type (Publish/Subscribe) */
    uint8_t peer_nmi[MACADDR_LEN];                      /**< Peer's NAN Management Interface address */
};

struct own_svc_info {
    char svc_name[ESP_WIFI_MAX_SVC_NAME_LEN];           /**< Name identifying a service */
    uint8_t svc_id;                                     /**< Identifier for a service */
    uint8_t type;                                       /**< Service type (Publish/Subscribe) */
    bool ndp_resp_needed;                               /**< If enabled, NDP response is required */
    uint8_t num_peer_records;                           /**< Count of peer records associated with svc_id */
    SLIST_HEAD(peer_list_t, peer_svc_info) peer_list;   /**< List of peers matched for specific service */
};

struct ndl_info {
    uint8_t ndp_id;                                     /**< Identifier for instance of NDP */
    uint8_t peer_ndi[MACADDR_LEN];                      /**< Peer's NAN Data Interface address */
    uint8_t peer_nmi[MACADDR_LEN];                      /**< Peer's NAN Management Interface address */
    uint8_t publisher_id;                               /**< Publisher's service identifier */
    uint8_t own_role;                                   /**< Own role (Publisher/Subscriber) */
};

typedef struct {
    uint8_t state;
    uint8_t event;
    struct ndl_info ndl[ESP_WIFI_NAN_DATAPATH_MAX_PEERS];        /**< Record of NDL of all peers */
    struct own_svc_info own_svc[ESP_WIFI_NAN_MAX_SVC_SUPPORTED]; /**< Record of own service(s) */
    esp_netif_t *nan_netif;
} nan_ctx_t;

static nan_ctx_t s_nan_ctx;

void esp_wifi_nan_get_ipv6_linklocal_from_mac(ip6_addr_t *ip6, uint8_t *mac_addr)
{
    if (ip6 == NULL || mac_addr == NULL)
        return;
    /* Link-local prefix. */
    ip6->addr[0] = htonl(0xfe800000ul);
    ip6->addr[1] = 0;

    /* Assume hwaddr is a 48-bit IEEE 802 MAC. Convert to EUI-64 address. Complement Group bit. */
    ip6->addr[2] = htonl((((uint32_t)(mac_addr[0] ^ 0x02)) << 24) |
                         ((uint32_t)(mac_addr[1]) << 16) |
                         ((uint32_t)(mac_addr[2]) << 8) |
                         (0xff));
    ip6->addr[3] = htonl((uint32_t)(0xfeul << 24) |
                         ((uint32_t)(mac_addr[3]) << 16) |
                         ((uint32_t)(mac_addr[4]) << 8) |
                         (mac_addr[5]));

    ip6->zone = IP6_NO_ZONE;
}

static struct own_svc_info *nan_find_own_svc(uint8_t svc_id)
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

static struct own_svc_info *nan_find_own_svc_by_name(const char *svc_name)
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

static struct peer_svc_info *nan_find_peer_svc(uint8_t own_svc_id, uint8_t peer_svc_id, uint8_t peer_nmi[])
{
    struct peer_svc_info *p_peer_svc = NULL, *temp;
    struct own_svc_info *p_own_svc = NULL;
    uint8_t *peer_nmi_valid = NULL;
    int idx = 0;

    if (MACADDR_EQUAL(peer_nmi, null_mac)) {
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
                if (temp->svc_id == peer_svc_id && !MACADDR_EQUAL(temp->peer_nmi, peer_nmi_valid)) {
                    p_peer_svc = temp;
                    break;
                }
            } else if (peer_svc_id != 0) {
                if (temp->svc_id == peer_svc_id) {
                    p_peer_svc = temp;
                    break;
                }
            } else {
                if (peer_nmi_valid && !MACADDR_EQUAL(temp->peer_nmi, peer_nmi_valid)) {
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

static bool nan_record_peer_svc(uint8_t own_svc_id, uint8_t peer_svc_id, uint8_t peer_nmi[])
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

static void nan_record_own_svc(uint8_t id, uint8_t type, const char svc_name[], bool ndp_resp_needed)
{
    struct own_svc_info *p_svc = NULL;

    for (int i = 0; i < ESP_WIFI_NAN_MAX_SVC_SUPPORTED; i++) {
        if (s_nan_ctx.own_svc[i].svc_id == 0) {
            p_svc = &s_nan_ctx.own_svc[i];
            break;
        }
    }

    if (!p_svc) {
        return;
    }

    p_svc->svc_id = id;
    p_svc->type = type;
    strlcpy(p_svc->svc_name, svc_name, ESP_WIFI_MAX_SVC_NAME_LEN);
    SLIST_INIT(&(p_svc->peer_list));
    if (type == ESP_NAN_PUBLISH) {
        p_svc->ndp_resp_needed = ndp_resp_needed;
    }
}

static bool ndl_limit_reached(void)
{
    for (int i = 0; i < ESP_WIFI_NAN_DATAPATH_MAX_PEERS; i++) {
        if (s_nan_ctx.ndl[i].ndp_id == 0) {
            return false;
        }
    }
    return true;
}

static void nan_record_new_ndl(uint8_t ndp_id, uint8_t publish_id, uint8_t peer_nmi[], uint8_t own_role)
{
    struct ndl_info *ndl = NULL;

    for (int i = 0; i < ESP_WIFI_NAN_DATAPATH_MAX_PEERS; i++) {
        if (s_nan_ctx.ndl[i].ndp_id == 0) {
            ndl = &s_nan_ctx.ndl[i];
            break;
        }
    }
    if (!ndl) {
        return;
    }
    ndl->ndp_id = ndp_id;
    if (peer_nmi) {
        MACADDR_COPY(ndl->peer_nmi, peer_nmi);
    }
    ndl->publisher_id = publish_id;
    ndl->own_role = own_role;
}

static struct ndl_info *nan_find_ndl(uint8_t ndp_id, uint8_t peer_nmi[])
{
    struct ndl_info *ndl = NULL;

    for (int i = 0; i < ESP_WIFI_NAN_DATAPATH_MAX_PEERS; i++) {
        ndl = &s_nan_ctx.ndl[i];
        if (ndp_id != 0 && peer_nmi) {
            if (ndl->ndp_id == ndp_id && !MACADDR_EQUAL(ndl->peer_nmi, peer_nmi)) {
                return ndl;
            }
        } else if (ndp_id != 0) {
            if (ndl->ndp_id == ndp_id) {
                return ndl;
            }
        } else if (peer_nmi) {
            if (!MACADDR_EQUAL(ndl->peer_nmi, peer_nmi)) {
                return ndl;
            }
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

static void nan_update_peer_svc(uint8_t own_svc_id, uint8_t peer_svc_id, uint8_t peer_nmi[])
{
    struct peer_svc_info *peer_info = nan_find_peer_svc(own_svc_id, 0, peer_nmi);
    if (peer_info) {
        peer_info->svc_id = peer_svc_id;
    }

    struct ndl_info *ndl = nan_find_ndl(0, peer_nmi);
    if (ndl) {
        ndl->publisher_id = peer_svc_id;
    }
}

static void nan_fill_params_from_event(void *evt_data, uint8_t event)
{
    switch (event) {
    case WIFI_EVENT_NDP_INDICATION: {
        wifi_event_ndp_indication_t *evt = (wifi_event_ndp_indication_t *)evt_data;
        nan_record_new_ndl(evt->ndp_id, evt->publish_id, evt->peer_nmi, ESP_WIFI_NDP_ROLE_RESPONDER);
        break;
    }
    case WIFI_EVENT_NDP_CONFIRM: {
        wifi_event_ndp_confirm_t *evt = (wifi_event_ndp_confirm_t *)evt_data;
        struct ndl_info *ndl = NULL;

        if ((ndl = nan_find_ndl(evt->ndp_id, evt->peer_nmi)) == NULL) {
            ESP_LOGE(TAG, "No NDL with ndp id %d", evt->ndp_id);
            return;
        }
        MACADDR_COPY(ndl->peer_ndi, evt->peer_ndi);
        break;
    }
    case WIFI_EVENT_NAN_REPLIED: {
        wifi_event_nan_replied_t *evt = (wifi_event_nan_replied_t *)evt_data;

        if (!nan_find_peer_svc(evt->publish_id, evt->subscribe_id, evt->sub_if_mac)) {
            nan_record_peer_svc(evt->publish_id, evt->subscribe_id, evt->sub_if_mac);
        }
        break;
    }
    case WIFI_EVENT_NAN_RECEIVE: {
        wifi_event_nan_receive_t *evt = (wifi_event_nan_receive_t *)evt_data;

        if (!nan_find_peer_svc(evt->inst_id, evt->peer_inst_id, evt->peer_if_mac)) {
            nan_record_peer_svc(evt->inst_id, evt->peer_inst_id, evt->peer_if_mac);
        }
        break;
    }
    case WIFI_EVENT_NAN_SVC_MATCH: {
        wifi_event_nan_svc_match_t *evt = (wifi_event_nan_svc_match_t *)evt_data;

        if (evt->update_pub_id) {
            nan_update_peer_svc(evt->subscribe_id, evt->publish_id, evt->pub_if_mac);
        }

        if (!nan_find_peer_svc(evt->subscribe_id, evt->publish_id, evt->pub_if_mac)) {
            nan_record_peer_svc(evt->subscribe_id, evt->publish_id, evt->pub_if_mac);
        }
        break;
    }
    default:
        break;
    }
}

static void nan_app_action_service_match(void *arg, esp_event_base_t event_base, int32_t event_id, void *data)
{
    if (data == NULL) {
        return;
    }
    wifi_event_nan_svc_match_t *evt = (wifi_event_nan_svc_match_t *)data;

    ESP_LOGI(TAG, "Service matched with "MACSTR" [Peer Publish id - %d]",
             MAC2STR(evt->pub_if_mac), evt->publish_id);

    NAN_DATA_LOCK();
    nan_fill_params_from_event(evt, WIFI_EVENT_NAN_SVC_MATCH);
    NAN_DATA_UNLOCK();
}

static void nan_app_action_replied(void *arg, esp_event_base_t event_base, int32_t event_id, void *data)
{
    if (data == NULL) {
        return;
    }
    wifi_event_nan_replied_t *evt = (wifi_event_nan_replied_t *)data;

    ESP_LOGD(TAG, "Sent Publish to Peer "MACSTR" [Peer Subscribe id - %d]",
             MAC2STR(evt->sub_if_mac), evt->subscribe_id);

    NAN_DATA_LOCK();
    nan_fill_params_from_event(evt, WIFI_EVENT_NAN_REPLIED);
    NAN_DATA_UNLOCK();
}

static void nan_app_action_receive(void *arg, esp_event_base_t event_base, int32_t event_id, void *data)
{
    if (data == NULL) {
        return;
    }
    wifi_event_nan_receive_t *evt = (wifi_event_nan_receive_t *)data;

    ESP_LOGI(TAG, "Received message '%s' from Peer "MACSTR" [Peer Service id - %d]",
             evt->peer_svc_info, MAC2STR(evt->peer_if_mac), evt->peer_inst_id);

    NAN_DATA_LOCK();
    nan_fill_params_from_event(evt, WIFI_EVENT_NAN_RECEIVE);
    NAN_DATA_UNLOCK();
}

static void nan_app_action_ndp_indication(void *arg, esp_event_base_t event_base, int32_t event_id, void *data)
{
    if (data == NULL) {
        return;
    }
    wifi_event_ndp_indication_t *evt = (wifi_event_ndp_indication_t *)data;

    NAN_DATA_LOCK();
    struct own_svc_info *p_own_svc = nan_find_own_svc(evt->publish_id);

    if (!p_own_svc) {
        ESP_LOGE(TAG, "No Publish found with id %d", evt->publish_id);
        goto done;
    }
    if (ndl_limit_reached()) {
        ESP_LOGE(TAG, "NDP limit reached");
        goto done;
    }

    nan_fill_params_from_event(evt, WIFI_EVENT_NDP_INDICATION);
    if (p_own_svc->ndp_resp_needed) {
        ESP_LOGI(TAG, "NDP Req from "MACSTR" [NDP Id: %d], Accept OR Deny using NDP command",
                 MAC2STR(evt->peer_nmi), evt->ndp_id);
        s_nan_ctx.event |= NDP_INDICATION;
    } else {
        wifi_nan_datapath_resp_t ndp_resp = {0};
        ndp_resp.accept = true;
        ndp_resp.ndp_id = evt->ndp_id;
        MACADDR_COPY(ndp_resp.peer_mac, evt->peer_nmi);

        esp_nan_internal_datapath_resp(&ndp_resp);
    }

done:
    NAN_DATA_UNLOCK();
}

static void nan_app_action_ndp_confirm(void *arg, esp_event_base_t event_base, int32_t event_id, void *data)
{
    if (data == NULL) {
        return;
    }
    wifi_event_ndp_confirm_t *evt = (wifi_event_ndp_confirm_t *)data;

    NAN_DATA_LOCK();
    wifi_netif_driver_t driver = esp_netif_get_io_driver(s_nan_ctx.nan_netif);
    ip_addr_t target_addr = {0};

    if (!s_nan_ctx.nan_netif) {
        ESP_LOGE(TAG, "%s: NAN netif is NULL", __func__);
        goto done;
    }

    if (nan_find_ndl(evt->ndp_id, NULL) == NULL) {
        /* As ndl isn't found, timeout has occured for NDP response and datapath request is rejected */
        goto done;
    }
    if (evt->status == NDP_STATUS_REJECTED) {
        ESP_LOGE(TAG, "NDP request to Peer "MACSTR" rejected [NDP ID - %d]", MAC2STR(evt->peer_nmi), evt->ndp_id);
        nan_reset_ndl(evt->ndp_id, false);
        os_event_group_set_bits(nan_event_group, NDP_REJECTED);
        goto done;
    }

    /* If interface not ready when started, rxcb to be registered on connection */
    if (esp_wifi_register_if_rxcb(driver,  esp_netif_receive, s_nan_ctx.nan_netif) != ESP_OK) {
        ESP_LOGE(TAG, "%s: esp_wifi_register_if_rxcb failed", __func__);
        goto done;
    }

    nan_fill_params_from_event(evt, WIFI_EVENT_NDP_CONFIRM);

    esp_netif_action_connected(s_nan_ctx.nan_netif, event_base, event_id, data);

    esp_netif_create_ip6_linklocal(s_nan_ctx.nan_netif);
    NAN_DATA_UNLOCK();
    esp_wifi_nan_get_ipv6_linklocal_from_mac(&target_addr.u_addr.ip6, evt->peer_ndi);
    target_addr.type = IPADDR_TYPE_V6;
    ESP_LOGI(TAG, "NDP confirmed with Peer "MACSTR" [NDP ID - %d, Peer IPv6 - %s]",
             MAC2STR(evt->peer_nmi), evt->ndp_id, inet6_ntoa(*ip_2_ip6(&target_addr)));
    os_event_group_set_bits(nan_event_group, NDP_ACCEPTED);
    return;

done:
    NAN_DATA_UNLOCK();
    return;
}

static void nan_app_action_ndp_terminated(void *arg, esp_event_base_t event_base, int32_t event_id, void *data)
{
    if (data == NULL) {
        return;
    }
    wifi_event_ndp_terminated_t *evt = (wifi_event_ndp_terminated_t *)data;

    NAN_DATA_LOCK();
    if (s_nan_ctx.nan_netif && !nan_is_datapath_active()) {
        esp_netif_action_disconnected(s_nan_ctx.nan_netif, event_base, event_id, data);
    }
    ESP_LOGI(TAG, "NDP terminated with Peer "MACSTR" [NDP ID - %d]", MAC2STR(evt->init_ndi), evt->ndp_id);
    nan_reset_ndl(evt->ndp_id, false);

    s_nan_ctx.event &= ~(NDP_INDICATION);
    NAN_DATA_UNLOCK();
    os_event_group_set_bits(nan_event_group, NDP_TERMINATED);
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
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_NAN_SVC_MATCH, nan_app_action_service_match);
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_NAN_REPLIED, nan_app_action_replied);
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_NAN_RECEIVE, nan_app_action_receive);
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_NDP_INDICATION, nan_app_action_ndp_indication);
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_NDP_CONFIRM, nan_app_action_ndp_confirm);
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_NDP_TERMINATED, nan_app_action_ndp_terminated);
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
    ESP_GOTO_ON_ERROR(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_NAN_SVC_MATCH,
                                                 nan_app_action_service_match, NULL), fail, TAG, "Registering event handler failed");

    ESP_GOTO_ON_ERROR(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_NAN_REPLIED,
                                             nan_app_action_replied, NULL), fail, TAG, "Registering event handler failed");

    ESP_GOTO_ON_ERROR(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_NAN_RECEIVE,
                                             nan_app_action_receive, NULL), fail, TAG, "Registering event handler failed");

    ESP_GOTO_ON_ERROR(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_NDP_INDICATION,
                                             nan_app_action_ndp_indication, NULL), fail, TAG, "Registering event handler failed");

    ESP_GOTO_ON_ERROR(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_NDP_CONFIRM,
                                             nan_app_action_ndp_confirm, NULL), fail, TAG, "Registering event handler failed");

    ESP_GOTO_ON_ERROR(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_NDP_TERMINATED,
                                             nan_app_action_ndp_terminated, NULL), fail, TAG, "Registering event handler failed");

    ESP_GOTO_ON_ERROR(esp_event_handler_register(IP_EVENT, IP_EVENT_GOT_IP6,
                                             nan_app_action_got_ipv6, NULL), fail, TAG, "Registering event handler failed");

    s_app_default_handlers_set = true;
    return ESP_OK;

fail:
    nan_clear_app_default_handlers();
    return ESP_FAIL;
}

void esp_nan_app_deinit(void)
{
    if (nan_event_group) {
        os_event_group_delete(nan_event_group);
        nan_event_group = NULL;
    }

    if (s_nan_data_lock) {
        os_semphr_delete(s_nan_data_lock);
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
    }
}

void esp_nan_action_start(esp_netif_t *nan_netif)
{
    if (nan_set_app_default_handlers() != ESP_OK) {
        ESP_LOGE(TAG, "Registering NAN handlers failed");
        return;
    }

    NAN_DATA_LOCK();
    s_nan_ctx.nan_netif = nan_netif;
    s_nan_ctx.state = NAN_STARTED_BIT;
    NAN_DATA_UNLOCK();

    ESP_LOGI(TAG, "NAN Discovery started.");
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

    os_event_group_set_bits(nan_event_group, NAN_STOPPED_BIT);
}

esp_err_t esp_wifi_nan_start(const wifi_nan_config_t *nan_cfg)
{
    wifi_mode_t mode;
    esp_err_t ret;
    wifi_config_t config = {0};

    ret = esp_wifi_get_mode(&mode);
    if (ret == ESP_ERR_WIFI_NOT_INIT) {
        ESP_LOGE(TAG, "WiFi not initialised!");
        return ret;
    } else if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Unable to get mode");
        return ret;
    }

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
    NAN_DATA_UNLOCK();

    ESP_RETURN_ON_ERROR(esp_wifi_set_mode(WIFI_MODE_NAN), TAG, "Set mode NAN failed");

    memcpy(&config.nan, nan_cfg, sizeof(wifi_nan_config_t));
    ESP_RETURN_ON_ERROR(esp_wifi_set_config(WIFI_IF_NAN, &config), TAG, "Setting NAN config failed");

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

esp_err_t esp_wifi_nan_stop(void)
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
        os_event_group_clear_bits(nan_event_group, NDP_TERMINATED);
        os_event_group_wait_bits(nan_event_group, NDP_TERMINATED, pdFALSE, pdFALSE, portMAX_DELAY);
        os_event_group_clear_bits(nan_event_group, NDP_TERMINATED);
        /* Wait for 1 NAN DW interval (512 TU's ~= 524 mSec) for successful termination */
        g_wifi_osi_funcs._task_delay(NAN_DW_INTVL_MS/portTICK_PERIOD_MS);
    } else {
        NAN_DATA_UNLOCK();
    }

    ESP_RETURN_ON_ERROR(esp_wifi_stop(), TAG, "Stopping NAN failed");

    EventBits_t bits = os_event_group_wait_bits(nan_event_group, NAN_STOPPED_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
    if (!(bits & NAN_STOPPED_BIT)) {
        return ESP_FAIL;
    }

    NAN_DATA_LOCK();
    memset(&s_nan_ctx, 0, sizeof(nan_ctx_t));
    NAN_DATA_UNLOCK();
    return ESP_OK;
}

uint8_t esp_wifi_nan_publish_service(const wifi_nan_publish_cfg_t *publish_cfg, bool ndp_resp_needed)
{
    uint8_t pub_id;

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
        ESP_LOGE(TAG, "Service name %s already used!", publish_cfg->service_name);
        goto fail;
    }

    if (esp_nan_internal_publish_service(publish_cfg, &pub_id, false) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to publish service '%s'", publish_cfg->service_name);
        goto fail;
    }

    ESP_LOGI(TAG, "Started Publishing %s [Service ID - %u]", publish_cfg->service_name, pub_id);
    nan_record_own_svc(pub_id, ESP_NAN_PUBLISH, publish_cfg->service_name, ndp_resp_needed);
    NAN_DATA_UNLOCK();

    return pub_id;
fail:
    NAN_DATA_UNLOCK();
    return 0;
}

uint8_t esp_wifi_nan_subscribe_service(const wifi_nan_subscribe_cfg_t *subscribe_cfg)
{
    uint8_t sub_id;

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

    if (esp_nan_internal_subscribe_service(subscribe_cfg, &sub_id, false) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to subscribe to service '%s'", subscribe_cfg->service_name);
        goto fail;
    }

    ESP_LOGI(TAG, "Started Subscribing to %s [Service ID - %u]", subscribe_cfg->service_name, sub_id);
    nan_record_own_svc(sub_id, ESP_NAN_SUBSCRIBE, subscribe_cfg->service_name, false);
    NAN_DATA_UNLOCK();

    return sub_id;
fail:
    NAN_DATA_UNLOCK();
    return 0;
}

esp_err_t esp_wifi_nan_send_message(wifi_nan_followup_params_t *fup_params)
{
    struct peer_svc_info *p_peer_svc;

    NAN_DATA_LOCK();
    p_peer_svc = nan_find_peer_svc(fup_params->inst_id, fup_params->peer_inst_id,
                                        fup_params->peer_mac);
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
    if (!MACADDR_EQUAL(fup_params->peer_mac, null_mac)) {
        MACADDR_COPY(fup_params->peer_mac, p_peer_svc->peer_nmi);
    }

    NAN_DATA_UNLOCK();
    if (esp_nan_internal_send_followup(fup_params) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to send Follow-up message!");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Sent message '%s' to Peer "MACSTR" with Service ID %d", fup_params->svc_info,
                  MAC2STR(fup_params->peer_mac), fup_params->peer_inst_id);
    return ESP_OK;
}

esp_err_t esp_wifi_nan_cancel_service(uint8_t service_id)
{
    NAN_DATA_LOCK();
    struct own_svc_info *p_own_svc = nan_find_own_svc(service_id);

    if (!p_own_svc) {
        ESP_LOGE(TAG, "Cannot find own service with id %d!", service_id);
        goto fail;
    }

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
    return ESP_OK;
}

uint8_t esp_wifi_nan_datapath_req(wifi_nan_datapath_req_t *req)
{
    uint8_t ndp_id = 0;

    NAN_DATA_LOCK();
    struct peer_svc_info *p_peer_svc = nan_find_peer_svc(0, req->pub_id, req->peer_mac);

    if (!p_peer_svc) {
        ESP_LOGE(TAG, "Cannot send NDP Req, peer not found!");
        goto fail;
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

    if (!MACADDR_EQUAL(req->peer_mac, null_mac)) {
        MACADDR_COPY(req->peer_mac, p_peer_svc->peer_nmi);
    }

    if (esp_nan_internal_datapath_req(req, &ndp_id) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initiate NDP req");
        goto fail;
    }

    nan_record_new_ndl(ndp_id, req->pub_id, req->peer_mac, ESP_WIFI_NDP_ROLE_INITIATOR);
    NAN_DATA_UNLOCK();

    ESP_LOGD(TAG, "Requested NDP with "MACSTR" [NDP ID - %d]", MAC2STR(req->peer_mac), ndp_id);

    EventBits_t bits = os_event_group_wait_bits(nan_event_group, NDP_ACCEPTED | NDP_REJECTED, pdFALSE, pdFALSE, pdMS_TO_TICKS(NAN_NDP_RESP_TIMEOUT));
    if (bits & NDP_ACCEPTED) {
        os_event_group_clear_bits(nan_event_group, NDP_ACCEPTED);
        return ndp_id;
    } else if (bits & NDP_REJECTED) {
        os_event_group_clear_bits(nan_event_group, NDP_REJECTED);
        return 0;
    } else {
        NAN_DATA_LOCK();
        nan_reset_ndl(ndp_id, false);
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

    if (!ndl) {
        ESP_LOGE(TAG, "No NDL with ndp id %d", resp->ndp_id);
        goto fail;
    }
    if (!(s_nan_ctx.event & NDP_INDICATION)) { //INDICATION of specific peer
        ESP_LOGE(TAG, "Need NDP Indication before NDP Response can be sent");
        goto fail;
    }

    if (!MACADDR_EQUAL(resp->peer_mac, null_mac)) {
        MACADDR_COPY(resp->peer_mac, ndl->peer_nmi);
    }

    if (esp_nan_internal_datapath_resp(resp) == ESP_OK) {
        s_nan_ctx.event &= ~NDP_INDICATION;
        NAN_DATA_UNLOCK();
        return ESP_OK;
    }

fail:
    NAN_DATA_UNLOCK();
    return ESP_FAIL;
}

esp_err_t esp_wifi_nan_datapath_end(wifi_nan_datapath_end_req_t *req)
{
    struct ndl_info *ndl = NULL;

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

    if (!MACADDR_EQUAL(req->peer_mac, null_mac)) {
        MACADDR_COPY(req->peer_mac, ndl->peer_nmi);
    }

    NAN_DATA_UNLOCK();
    if (esp_nan_internal_datapath_end(req) == ESP_OK) {
        return ESP_OK;
    }

    return ESP_FAIL;
}

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
