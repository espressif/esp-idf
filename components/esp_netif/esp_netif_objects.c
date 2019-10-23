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
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
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
static xSemaphoreHandle  s_list_lock = NULL;

ESP_EVENT_DEFINE_BASE(IP_EVENT);

esp_err_t esp_netif_list_lock(void)
{
    if (s_list_lock == NULL) {
        s_list_lock = xSemaphoreCreateMutex();
        if (s_list_lock == NULL) {
            return ESP_ERR_NO_MEM;
        }
    }
    xSemaphoreTake(s_list_lock, portMAX_DELAY);
    return ESP_OK;
}

void esp_netif_list_unlock(void)
{
    assert(s_list_lock);
    xSemaphoreGive(s_list_lock);
    if (s_esp_netif_counter == 0) {
        vQueueDelete(s_list_lock);
        s_list_lock = NULL;
    }
}

//
// List manipulation functions
//
esp_err_t esp_netif_add_to_list(esp_netif_t *netif)
{
    esp_err_t ret;
    struct slist_netifs_s *item = calloc(1, sizeof(struct slist_netifs_s));
    ESP_LOGD(TAG, "%s %p", __func__, netif);
    if (item == NULL) {
        return ESP_ERR_NO_MEM;
    }
    item->netif = netif;

    if ((ret = esp_netif_list_lock()) != ESP_OK) {
        return ret;
    }

    SLIST_INSERT_HEAD(&s_head, item, next);
    ++s_esp_netif_counter;
    ESP_LOGD(TAG, "%s netif added successfully (total netifs: %d)", __func__, s_esp_netif_counter);
    esp_netif_list_unlock();
    return ESP_OK;
}


esp_err_t esp_netif_remove_from_list(esp_netif_t *netif)
{
    struct slist_netifs_s *item;
    esp_err_t ret;
    if ((ret = esp_netif_list_lock()) != ESP_OK) {
        return ret;
    }
    ESP_LOGV(TAG, "%s %p", __func__, netif);

    SLIST_FOREACH(item, &s_head, next) {
        if (item->netif == netif) {
            SLIST_REMOVE(&s_head, item, slist_netifs_s, next);
            assert(s_esp_netif_counter > 0);
            --s_esp_netif_counter;
            ESP_LOGD(TAG, "%s netif successfully removed (total netifs: %d)", __func__, s_esp_netif_counter);
            free(item);
            esp_netif_list_unlock();
            return ESP_OK;
        }
    }
    esp_netif_list_unlock();
    return ESP_ERR_NOT_FOUND;
}

size_t esp_netif_get_nr_of_ifs(void)
{
    return s_esp_netif_counter;
}

esp_netif_t* esp_netif_next(esp_netif_t* netif)
{
    esp_err_t ret;
    esp_netif_t* result;
    if ((ret = esp_netif_list_lock()) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to lock esp-netif list with %d", ret);
        return NULL;
    }
    result = esp_netif_next_unsafe(netif);
    esp_netif_list_unlock();
    return result;
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
    esp_err_t ret;
    if ((ret = esp_netif_list_lock()) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to lock esp-netif list with %d", ret);
        return NULL;
    }

    // looking for the netif in the list of registered interfaces
    esp_netif_t *it = esp_netif_next_unsafe(NULL);
    do {
        if (it && it == esp_netif) {
            esp_netif_list_unlock();
            return true;
        }
    } while (NULL != (it = esp_netif_next_unsafe(it)));
    esp_netif_list_unlock();
    return false;
}

esp_netif_t *esp_netif_get_handle_from_ifkey(const char *if_key)
{
    esp_err_t ret;
    if ((ret = esp_netif_list_lock()) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to lock esp-netif list with %d", ret);
        return NULL;
    }

    esp_netif_t *esp_netif = esp_netif_next_unsafe(NULL);
    do {
        if (esp_netif && strcmp(if_key, esp_netif_get_ifkey(esp_netif))==0) {
            esp_netif_list_unlock();
            return esp_netif;
        }
    } while (NULL != (esp_netif = esp_netif_next_unsafe(esp_netif)));
    esp_netif_list_unlock();
    return NULL;
}