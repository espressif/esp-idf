/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * @file esp_pbuf reference
 * This file handles lwip custom pbufs interfacing with esp_netif
 * and the L2 free function esp_netif_free_rx_buffer()
 */

#include "lwip/mem.h"
#include "lwip/esp_pbuf_ref.h"
#include "esp_netif_net_stack.h"

/**
 * @brief Specific pbuf structure for pbufs allocated by ESP netif
 * of PBUF_REF type
 */
typedef struct esp_custom_pbuf
{
    struct pbuf_custom p;
    esp_netif_t *esp_netif;
    void* l2_buf;
} esp_custom_pbuf_t;

/**
 * @brief Free custom pbuf containing the L2 layer buffer allocated in the driver
 *
 * @param pbuf Custom pbuf holding the packet passed to lwip input
 * @note This function called as a custom_free_function() upon pbuf_free()
 */
static void esp_pbuf_free(struct pbuf *pbuf)
{
    esp_custom_pbuf_t* esp_pbuf = (esp_custom_pbuf_t*)pbuf;
    esp_netif_free_rx_buffer(esp_pbuf->esp_netif, esp_pbuf->l2_buf);
    mem_free(pbuf);
}

/**
 * @brief Allocate custom pbuf for supplied sp_netif
 * @param esp_netif esp-netif handle
 * @param buffer Buffer to allocate
 * @param len Size of the buffer
 * @param l2_buff External l2 buffe
 * @return Custom pbuf pointer on success; NULL if no free heap
 */
struct pbuf* esp_pbuf_allocate(esp_netif_t *esp_netif, void *buffer, size_t len, void *l2_buff)
{
    struct pbuf *p;

    esp_custom_pbuf_t* esp_pbuf  = mem_malloc(sizeof(esp_custom_pbuf_t));
    if (esp_pbuf == NULL) {
        return NULL;
    }
    esp_pbuf->p.custom_free_function = esp_pbuf_free;
    esp_pbuf->esp_netif = esp_netif;
    esp_pbuf->l2_buf = l2_buff;
    p = pbuf_alloced_custom(PBUF_RAW, len, PBUF_REF, &esp_pbuf->p, buffer, len);
    if (p == NULL) {
        mem_free(esp_pbuf);
        return NULL;
    }
    return p;
}
