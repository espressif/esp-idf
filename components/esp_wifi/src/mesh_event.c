// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string.h>
#include "esp_event.h"
#include "esp_mesh.h"

/* mesh event callback handler */
mesh_event_cb_t g_mesh_event_cb = NULL;

void esp_event_mesh_hook(system_event_t* event)
{
    if (event->event_id == SYSTEM_EVENT_STA_GOT_IP || event->event_id == SYSTEM_EVENT_STA_LOST_IP) {
        if (g_mesh_event_cb) {
            mesh_event_t mevent;
            if (event->event_id == SYSTEM_EVENT_STA_GOT_IP) {
                mevent.id = MESH_EVENT_ROOT_GOT_IP;
                memcpy(&mevent.info.got_ip, &event->event_info.got_ip, sizeof(system_event_sta_got_ip_t));
            } else {
                mevent.id = MESH_EVENT_ROOT_LOST_IP;
            }
            g_mesh_event_cb(mevent);
        }
    }
}
