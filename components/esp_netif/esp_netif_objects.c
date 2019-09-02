// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "esp_netif.h"
#include "sys/queue.h"
#include "esp_log.h"

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
esp_err_t esp_netif_add_to_list(esp_netif_t *netif)
{
    struct slist_netifs_s *item = calloc(1, sizeof(struct slist_netifs_s));
    ESP_LOGD(TAG, "%s %p", __func__, netif);
    if (item == NULL) {
        return ESP_ERR_NO_MEM;
    }
    item->netif = netif;

    SLIST_INSERT_HEAD(&s_head, item, next);
    ++s_esp_netif_counter;
    ESP_LOGD(TAG, "%s netif added successfully (total netifs: %d)", __func__, s_esp_netif_counter);
    return ESP_OK;
}


esp_err_t esp_netif_remove_from_list(esp_netif_t *netif)
{
    struct slist_netifs_s *item;
    ESP_LOGV(TAG, "%s %p", __func__, netif);
    SLIST_FOREACH(item, &s_head, next) {
        if (item->netif == netif) {
            SLIST_REMOVE(&s_head, item, slist_netifs_s, next);
            assert(s_esp_netif_counter > 0);
            --s_esp_netif_counter;
            ESP_LOGD(TAG, "%s netif successfully removed (total netifs: %d)", __func__, s_esp_netif_counter);
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

esp_netif_t* esp_netif_next(esp_netif_t* netif)
{
    ESP_LOGV(TAG, "%s %p", __func__, netif);
    struct slist_netifs_s *item;
    if (netif == NULL) {
        item = SLIST_FIRST(&s_head);
        return (item == NULL) ? NULL : item->netif;
    } else {
        SLIST_FOREACH(item, &s_head, next) {
            if (item->netif == netif) {
                item = SLIST_NEXT(item, next);
                return (item == NULL) ? NULL : item->netif;
            }
        }
    }
    return NULL;
}