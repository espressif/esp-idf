/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * @file esp_pbuf reference interface file
 */

#ifndef __LWIP_ESP_PBUF_REF_H__
#define __LWIP_ESP_PBUF_REF_H__

#include <stddef.h>
#include "lwip/pbuf.h"
#include "esp_netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Allocate custom pbuf containing pointer to a private l2-free function
 *
 * @note pbuf_free() will deallocate this custom pbuf and call the driver assigned free function
 */
struct pbuf* esp_pbuf_allocate(esp_netif_t *esp_netif, void *buffer, size_t len, void *l2_buff);

#ifdef __cplusplus
}
#endif

#endif //__LWIP_ESP_PBUF_REF_H__
