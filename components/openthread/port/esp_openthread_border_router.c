// Copyright 2021 Espressif Systems (Shanghai) CO LTD
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
// limitations under the License

#include "esp_openthread_border_router.h"
#include "esp_err.h"

static esp_netif_t *s_backbone_netif = NULL;

esp_err_t esp_openthread_border_router_init(esp_netif_t *backbone_if)
{
#if CONFIG_OPENTHREAD_BORDER_ROUTER
    s_backbone_netif = backbone_if;

    return ESP_OK;
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

esp_netif_t *esp_openthread_get_backbone_netif(void)
{
    return s_backbone_netif;
}
