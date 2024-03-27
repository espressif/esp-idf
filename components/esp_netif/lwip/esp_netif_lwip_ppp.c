/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include "esp_netif.h"

#include "lwip/dns.h"
#include "netif/ppp/pppos.h"
#include "esp_log.h"
#include "esp_netif_net_stack.h"
#include "esp_event.h"
#include "esp_netif_ppp.h"
#include "esp_netif_lwip_internal.h"
#include <string.h>
#include "lwip/ip6_addr.h"
#include "netif/pppif.h"

ESP_EVENT_DEFINE_BASE(NETIF_PPP_STATUS);

static const char *TAG = "esp-netif_lwip-ppp";

/**
 * @brief internal lwip_ppp context struct extends the netif related data
 *        used to hold PPP netif related parameters
 */
typedef struct lwip_peer2peer_ctx {
    netif_related_data_t base;      // Generic portion of netif-related data
    // PPP specific fields follow
    bool ppp_phase_event_enabled;
    bool ppp_error_event_enabled;
#ifdef CONFIG_LWIP_ENABLE_LCP_ECHO
    bool ppp_lcp_echo_disabled;
#endif
#ifdef CONFIG_LWIP_PPP_SERVER_SUPPORT
    esp_ip4_addr_t ppp_our_ip4_addr;      // our desired IP (IPADDR_ANY if no preference)
    esp_ip4_addr_t ppp_their_ip4_addr;    // their desired IP (IPADDR_ANY if no preference)
#endif
    ppp_pcb *ppp;
} lwip_peer2peer_ctx_t;

/**
 * @brief lwip callback from PPP client used here to produce PPP error related events,
 * as well as some IP events
 */
static void on_ppp_status_changed(ppp_pcb *pcb, int err_code, void *ctx)
{
    esp_netif_t *netif = ctx;
    ip_event_got_ip_t evt = {
            .esp_netif = netif,
    };
    esp_err_t err;
    struct lwip_peer2peer_ctx *obj =  (struct lwip_peer2peer_ctx*)netif->related_data;
    assert(obj->base.netif_type == PPP_LWIP_NETIF);
    switch (err_code) {
        case PPPERR_NONE:
            ESP_LOGI(TAG, "Connected");
            break;
        case PPPERR_PARAM:
            ESP_LOGE(TAG, "Invalid parameter");
            break;
        case PPPERR_OPEN:
            ESP_LOGE(TAG, "Unable to open PPP session");
            break;
        case PPPERR_DEVICE:
            ESP_LOGE(TAG, "Invalid I/O device for PPP");
            break;
        case PPPERR_ALLOC:
            ESP_LOGE(TAG, "Unable to allocate resources");
            break;
        case PPPERR_USER: /* User interrupt */
            ESP_LOGI(TAG, "User interrupt");
            break;
        case PPPERR_CONNECT: /* Connection lost */
            ESP_LOGI(TAG, "Connection lost");
            err = esp_event_post(IP_EVENT, netif->lost_ip_event, &evt, sizeof(evt), 0);

            if (ESP_OK != err) {
                ESP_LOGE(TAG, "esp_event_post failed with code %d", err);
            }
            return;

        case PPPERR_AUTHFAIL:
            ESP_LOGE(TAG, "Failed authentication challenge");
            break;
        case PPPERR_PROTOCOL:
            ESP_LOGE(TAG, "Failed to meet protocol");
            break;
        case PPPERR_PEERDEAD:
            ESP_LOGE(TAG, "Connection timeout");
            break;
        case PPPERR_IDLETIMEOUT:
            ESP_LOGE(TAG, "Idle Timeout");
            break;
        case PPPERR_CONNECTTIME:
            ESP_LOGE(TAG, "Max connect time reached");
            break;
        case PPPERR_LOOPBACK:
            ESP_LOGE(TAG, "Loopback detected");
            break;
        default:
            ESP_LOGE(TAG, "Unknown error code %d", err_code);
            break;
    }
    if (obj->ppp_error_event_enabled) {
        err = esp_event_post(NETIF_PPP_STATUS, err_code, &netif, sizeof(netif), 0);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "esp_event_post failed with code %d", err);
        }

    }
}

#if PPP_NOTIFY_PHASE
/**
 * @brief Notify phase callback which is called on each PPP internal state change
 *
 * @param pcb PPP control block
 * @param phase Phase ID
 * @param ctx Context of callback
 */
static void on_ppp_notify_phase(ppp_pcb *pcb, u8_t phase, void *ctx)
{
    switch (phase) {
        case PPP_PHASE_DEAD:
            ESP_LOGD(TAG, "Phase Dead");
            break;
        case PPP_PHASE_INITIALIZE:
            ESP_LOGD(TAG, "Phase Start");
            break;
        case PPP_PHASE_ESTABLISH:
            ESP_LOGD(TAG, "Phase Establish");
            break;
        case PPP_PHASE_AUTHENTICATE:
            ESP_LOGD(TAG, "Phase Authenticate");
            break;
        case PPP_PHASE_NETWORK:
            ESP_LOGD(TAG, "Phase Network");
            break;
        case PPP_PHASE_RUNNING:
            ESP_LOGD(TAG, "Phase Running");
            break;
        case PPP_PHASE_TERMINATE:
            ESP_LOGD(TAG, "Phase Terminate");
            break;
        case PPP_PHASE_DISCONNECT:
            ESP_LOGD(TAG, "Phase Disconnect");
            break;
        default:
            ESP_LOGW(TAG, "Phase Unknown: %d", phase);
            break;
    }
    esp_netif_t *netif = ctx;
    lwip_peer2peer_ctx_t *obj = (lwip_peer2peer_ctx_t *)netif->related_data;
    assert(obj->base.netif_type == PPP_LWIP_NETIF);
    if (obj && obj->ppp_phase_event_enabled) {
        esp_err_t err = esp_event_post(NETIF_PPP_STATUS, NETIF_PP_PHASE_OFFSET + phase, &netif, sizeof(netif), 0);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "esp_event_post failed with code %d", err);
        }
    }
}
#endif // PPP_NOTIFY_PHASE

/**
 * @brief PPP low level output callback used to transmit data using standard esp-netif interafce
 *
 * @param pcb PPP control block
 * @param data Buffer to write to serial port
 * @param len Length of the data buffer
 * @param ctx Context of callback
 *
 * @return uint32_t Length of data successfully sent
 */
static uint32_t pppos_low_level_output(ppp_pcb *pcb, uint8_t *data, uint32_t len, void *netif)
{
    esp_err_t ret = esp_netif_transmit(netif, data, len);
    if (ret == ESP_OK) {
        return len;
    }
    return 0;
}

esp_err_t esp_netif_ppp_set_auth_internal(esp_netif_t *netif, esp_netif_auth_type_t authtype, const char *user, const char *passwd)
{
    if (!ESP_NETIF_IS_POINT2POINT_TYPE(netif, PPP_LWIP_NETIF)) {
        return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
    }
#if PPP_AUTH_SUPPORT
    lwip_peer2peer_ctx_t *ppp_ctx = (lwip_peer2peer_ctx_t *)netif->related_data;
    ppp_set_auth(ppp_ctx->ppp, authtype, user, passwd);
    return ESP_OK;
#else
    ESP_LOGE(TAG, "%s failed: No authorisation enabled in menuconfig", __func__);
    return ESP_ERR_ESP_NETIF_IF_NOT_READY;
#endif
}

void esp_netif_ppp_set_default_netif(netif_related_data_t *netif_related)
{
    lwip_peer2peer_ctx_t *ppp_ctx = (lwip_peer2peer_ctx_t *)netif_related;
    assert(ppp_ctx->base.netif_type == PPP_LWIP_NETIF);

    ppp_set_default(ppp_ctx->ppp);
}

netif_related_data_t * esp_netif_new_ppp(esp_netif_t *esp_netif, const esp_netif_netstack_config_t *esp_netif_stack_config)
{
    struct netif * netif_impl = esp_netif->lwip_netif;
    struct lwip_peer2peer_ctx * ppp_obj = calloc(1, sizeof(struct lwip_peer2peer_ctx));
    if (ppp_obj == NULL) {
        ESP_LOGE(TAG, "%s: cannot allocate lwip_ppp_ctx", __func__);
        return NULL;
    }
    // Setup the generic esp-netif fields
    ppp_obj->base.is_point2point = true;
    ppp_obj->base.netif_type = PPP_LWIP_NETIF;

    ppp_obj->ppp = pppos_create(netif_impl, pppos_low_level_output, on_ppp_status_changed, esp_netif);
    ESP_LOGD(TAG, "%s: PPP connection created: %p", __func__, ppp_obj->ppp);
    if (!ppp_obj->ppp) {
        ESP_LOGE(TAG, "%s: lwIP PPP connection cannot be created", __func__);
        return NULL;
    }

    // Set the related data here, since the phase callback could be triggered before this function exits
    esp_netif->related_data = (netif_related_data_t *)ppp_obj;
#if PPP_NOTIFY_PHASE
    ppp_set_notify_phase_callback(ppp_obj->ppp, on_ppp_notify_phase);
#endif
    ppp_set_usepeerdns(ppp_obj->ppp, 1);

    return (netif_related_data_t *)ppp_obj;
}

esp_err_t esp_netif_start_ppp(esp_netif_t *esp_netif)
{
    netif_related_data_t *netif_related = esp_netif->related_data;
    lwip_peer2peer_ctx_t *ppp_ctx = (lwip_peer2peer_ctx_t *)netif_related;
    assert(ppp_ctx->base.netif_type == PPP_LWIP_NETIF);

#ifdef CONFIG_LWIP_ENABLE_LCP_ECHO
    if (ppp_ctx->ppp_lcp_echo_disabled) {
        ppp_ctx->ppp->settings.lcp_echo_interval = 0;
        ppp_ctx->ppp->settings.lcp_echo_fails = 0;
    } else {
        ppp_ctx->ppp->settings.lcp_echo_interval = LCP_ECHOINTERVAL;
        ppp_ctx->ppp->settings.lcp_echo_fails = LCP_MAXECHOFAILS;
    }
#endif
#ifdef CONFIG_LWIP_PPP_SERVER_SUPPORT
    if (ppp_ctx->ppp_our_ip4_addr.addr != IPADDR_ANY) {
        // Set our preferred address, and accept the remote
        ppp_ctx->ppp->ipcp_wantoptions.ouraddr = ppp_ctx->ppp_our_ip4_addr.addr;
        ppp_ctx->ppp->ipcp_wantoptions.accept_remote = 1;
    }
    if (ppp_ctx->ppp_their_ip4_addr.addr != IPADDR_ANY) {
        // Set their preferred address, and accept the local
        ppp_ctx->ppp->ipcp_wantoptions.hisaddr = ppp_ctx->ppp_their_ip4_addr.addr;
        ppp_ctx->ppp->ipcp_wantoptions.accept_local = 1;
    }
#endif // CONFIG_LWIP_PPP_SERVER_SUPPORT

    ESP_LOGD(TAG, "%s: Starting PPP connection: %p", __func__, ppp_ctx->ppp);
#ifdef CONFIG_LWIP_PPP_SERVER_SUPPORT
    esp_err_t err = ppp_listen(ppp_ctx->ppp);
#else
    err_t err = ppp_connect(ppp_ctx->ppp, 0);
#endif
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: PPP connection cannot be started", __func__);
        if (ppp_ctx->ppp_error_event_enabled) {
            esp_event_post(NETIF_PPP_STATUS, NETIF_PPP_CONNECT_FAILED, esp_netif, sizeof(esp_netif), 0);
        }
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_netif_recv_ret_t esp_netif_lwip_ppp_input(void *ppp_ctx, void *buffer, size_t len, void *eb)
{
    struct lwip_peer2peer_ctx * obj = ppp_ctx;
    err_t ret = pppos_input_tcpip_as_ram_pbuf(obj->ppp, buffer, len);
    if (ret != ERR_OK) {
        ESP_LOGE(TAG, "pppos_input_tcpip failed with %d", ret);
        return ESP_NETIF_OPTIONAL_RETURN_CODE(ESP_FAIL);
    }
    return ESP_NETIF_OPTIONAL_RETURN_CODE(ESP_OK);
}

esp_err_t esp_netif_stop_ppp(netif_related_data_t *netif_related)
{
    lwip_peer2peer_ctx_t *ppp_ctx = (lwip_peer2peer_ctx_t *)netif_related;
    assert(ppp_ctx->base.netif_type == PPP_LWIP_NETIF);
    ESP_LOGD(TAG, "%s: Stopped PPP connection: %p", __func__, ppp_ctx->ppp);
    err_t ret = ppp_close(ppp_ctx->ppp, 0);
    if (ret != ERR_OK) {
        ESP_LOGE(TAG, "ppp_close failed with %d", ret);
        return ESP_FAIL;
    }
    return ESP_OK;
}

void esp_netif_destroy_ppp(netif_related_data_t *netif_related)
{
    lwip_peer2peer_ctx_t *ppp_ctx = (lwip_peer2peer_ctx_t *)netif_related;
    assert(ppp_ctx->base.netif_type == PPP_LWIP_NETIF);

    ppp_free(ppp_ctx->ppp);
    free(netif_related);
}

esp_err_t esp_netif_ppp_set_params(esp_netif_t *netif, const esp_netif_ppp_config_t *config)
{
    if (netif == NULL || netif->related_data == NULL || config == NULL ||
        ((struct lwip_peer2peer_ctx *)netif->related_data)->base.netif_type != PPP_LWIP_NETIF) {
        return ESP_ERR_INVALID_ARG;
    }
    struct lwip_peer2peer_ctx *obj =  (struct lwip_peer2peer_ctx *)netif->related_data;
    obj->ppp_phase_event_enabled = config->ppp_phase_event_enabled;
    obj->ppp_error_event_enabled = config->ppp_error_event_enabled;
#ifdef CONFIG_LWIP_ENABLE_LCP_ECHO
    obj->ppp_lcp_echo_disabled = config->ppp_lcp_echo_disabled;
#endif
#ifdef CONFIG_LWIP_PPP_SERVER_SUPPORT
    obj->ppp_our_ip4_addr = config->ppp_our_ip4_addr;
    obj->ppp_their_ip4_addr = config->ppp_their_ip4_addr;
#endif
    return ESP_OK;
}

esp_err_t esp_netif_ppp_get_params(esp_netif_t *netif, esp_netif_ppp_config_t *config)
{
    if (netif == NULL || netif->related_data == NULL || config == NULL ||
        ((struct lwip_peer2peer_ctx *)netif->related_data)->base.netif_type != PPP_LWIP_NETIF) {
        return ESP_ERR_INVALID_ARG;
    }
    struct lwip_peer2peer_ctx *obj =  (struct lwip_peer2peer_ctx *)netif->related_data;
    config->ppp_phase_event_enabled = obj->ppp_phase_event_enabled;
    config->ppp_error_event_enabled = obj->ppp_error_event_enabled;
#ifdef CONFIG_LWIP_ENABLE_LCP_ECHO
    config->ppp_lcp_echo_disabled = obj->ppp_lcp_echo_disabled;
#endif
#ifdef CONFIG_LWIP_PPP_SERVER_SUPPORT
    config->ppp_our_ip4_addr = obj->ppp_our_ip4_addr;
    config->ppp_their_ip4_addr = obj->ppp_their_ip4_addr;
#endif

    return ESP_OK;
}
