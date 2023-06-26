/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_netif.h"
#include "lwip/netif.h"
#include "esp_netif_ppp.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_ESP_NETIF_RECEIVE_REPORT_ERRORS
typedef esp_err_t esp_netif_recv_ret_t;
#define ESP_NETIF_OPTIONAL_RETURN_CODE(expr) expr
#else
typedef void esp_netif_recv_ret_t;
#define ESP_NETIF_OPTIONAL_RETURN_CODE(expr)
#endif // CONFIG_ESP_NETIF_RECEIVE_REPORT_ERRORS

typedef err_t (*init_fn_t)(struct netif*);
typedef esp_netif_recv_ret_t (*input_fn_t)(void *netif, void *buffer, size_t len, void *eb);

struct esp_netif_netstack_lwip_vanilla_config {
    init_fn_t init_fn;
    input_fn_t input_fn;
};

struct esp_netif_netstack_lwip_ppp_config {
    input_fn_t input_fn;
    esp_netif_ppp_config_t ppp_events;
};

// LWIP netif specific network stack configuration
struct esp_netif_netstack_config {
    union {
        struct esp_netif_netstack_lwip_vanilla_config lwip;
        struct esp_netif_netstack_lwip_ppp_config lwip_ppp;
    };
};

/**
 * @brief   LWIP's network stack init function for Ethernet
 * @param netif LWIP's network interface handle
 * @return ERR_OK on success
 */
err_t ethernetif_init(struct netif *netif);

/**
 * @brief   LWIP's network stack input packet function for Ethernet
 * @param h LWIP's network interface handle
 * @param buffer Input buffer pointer
 * @param len Input buffer size
 * @param l2_buff External buffer pointer (to be passed to custom input-buffer free)
 */
esp_netif_recv_ret_t ethernetif_input(void *h, void *buffer, size_t len, void *l2_buff);

/**
 * @brief   LWIP's network stack init function for WiFi (AP)
 * @param netif LWIP's network interface handle
 * @return ERR_OK on success
 */
err_t wlanif_init_ap(struct netif *netif);

/**
 * @brief   LWIP's network stack init function for WiFi (Station)
 * @param netif LWIP's network interface handle
 * @return ERR_OK on success
 */
err_t wlanif_init_sta(struct netif *netif);

/**
 * @brief   LWIP's network stack init function for WiFi Aware interface (NAN)
 * @param netif LWIP's network interface handle
 * @return ERR_OK on success
 */
err_t wlanif_init_nan(struct netif *netif);

/**
 * @brief   LWIP's network stack input packet function for WiFi (both STA/AP)
 * @param h LWIP's network interface handle
 * @param buffer Input buffer pointer
 * @param len Input buffer size
 * @param l2_buff External buffer pointer (to be passed to custom input-buffer free)
 */
esp_netif_recv_ret_t wlanif_input(void *h, void *buffer, size_t len, void* l2_buff);

#ifdef __cplusplus
}
#endif
