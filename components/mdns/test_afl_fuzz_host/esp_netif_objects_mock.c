// Copyright 2020 Espressif Systems (Shanghai) CO LTD
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
#include "esp_netif_private.h"
#include <string.h>

//
// Purpose of this module is to provide list of esp-netif structures
//  - this module has no dependency on a specific network stack (lwip)
//

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
    return ESP_OK;
}


esp_err_t esp_netif_remove_from_list(esp_netif_t *netif)
{
    return ESP_ERR_NOT_FOUND;
}

size_t esp_netif_get_nr_of_ifs(void)
{
    return s_esp_netif_counter;
}

esp_netif_t* esp_netif_next(esp_netif_t* netif)
{
    return NULL;
}

esp_netif_t* esp_netif_next_unsafe(esp_netif_t* netif)
{
    return NULL;
}

esp_netif_t *esp_netif_get_handle_from_ifkey(const char *if_key)
{
    return NULL;
}
