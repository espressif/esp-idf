/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_netif.h"
#include "sys/queue.h"
#include "esp_log.h"
#include "esp_netif_private.h"
#include <string.h>

//
// Purpose of this module is to provide list of esp-netif structures
//  - this module has no dependency on a specific network stack (lwip)
//

static const char *TAG = "esp_netif_objects";

typedef struct slist_netifs_s slist_netifs_t;
struct slist_netifs_s {
    esp_netif_t *netif;
    SLIST_ENTRY(slist_netifs_s) next;
};

SLIST_HEAD(slisthead, slist_netifs_s) s_head = { .slh_first = NULL, };

static size_t s_esp_netif_counter = 0;

ESP_EVENT_DEFINE_BASE(IP_EVENT);

//
// List manipulation functions
//
esp_err_t esp_netif_add_to_list_unsafe(esp_netif_t *netif)
{
    struct slist_netifs_s *item = calloc(1, sizeof(struct slist_netifs_s));
    ESP_LOGV(TAG, "%s %p", __func__, netif);
    if (item == NULL) {
        return ESP_ERR_NO_MEM;
    }
    item->netif = netif;

    SLIST_INSERT_HEAD(&s_head, item, next);
    ++s_esp_netif_counter;
    ESP_LOGD(TAG, "%s netif added successfully (total netifs: %" PRIu32 ")", __func__, (uint32_t)s_esp_netif_counter);
    return ESP_OK;
}


esp_err_t esp_netif_remove_from_list_unsafe(esp_netif_t *netif)
{
    struct slist_netifs_s *item;
    ESP_LOGV(TAG, "%s %p", __func__, netif);

    SLIST_FOREACH(item, &s_head, next) {
        if (item->netif == netif) {
            SLIST_REMOVE(&s_head, item, slist_netifs_s, next);
            assert(s_esp_netif_counter > 0);
            --s_esp_netif_counter;
            ESP_LOGD(TAG, "%s netif successfully removed (total netifs: %" PRIu32 ")", __func__, (uint32_t)s_esp_netif_counter);
            free(item);
            return ESP_OK;
        }
    }
    return ESP_ERR_NOT_FOUND;
}

size_t esp_netif_get_nr_of_ifs(void)
{
    return s_esp_netif_counter;
}

// This API is inherently unsafe
// suggest that users call from esp_netif_tcpip_exec()
esp_netif_t* esp_netif_next(esp_netif_t* netif)
{
    return esp_netif_next_unsafe(netif);
}

esp_netif_t* esp_netif_next_unsafe(esp_netif_t* netif)
{
    ESP_LOGV(TAG, "%s %p", __func__, netif);
    struct slist_netifs_s *item;
    // Getting the first netif if argument is NULL
    if (netif == NULL) {
        item = SLIST_FIRST(&s_head);
        return (item == NULL) ? NULL : item->netif;
    }
    // otherwise the next one (after the supplied netif)
    SLIST_FOREACH(item, &s_head, next) {
        if (item->netif == netif) {
            item = SLIST_NEXT(item, next);
            return (item == NULL) ? NULL : item->netif;
        }
    }
    return NULL;
}

bool esp_netif_is_netif_listed(esp_netif_t *esp_netif)
{
    struct slist_netifs_s *item;
    SLIST_FOREACH(item, &s_head, next) {
        if (item->netif == esp_netif) {
            return true;
        }
    }
    return false;
}

esp_netif_t *esp_netif_get_handle_from_ifkey_unsafe(const char *if_key)
{
    struct slist_netifs_s *item;

    SLIST_FOREACH(item, &s_head, next) {
        esp_netif_t *esp_netif = item->netif;
        if (strcmp(if_key, esp_netif_get_ifkey(esp_netif)) == 0) {
            return esp_netif;
        }
    }
    return NULL;
}
