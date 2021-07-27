// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

#ifndef WPAS_GLUE_H
#define WPAS_GLUE_H

u8 *wpa_sm_alloc_eapol(struct wpa_sm *sm, u8 type,
                       const void *data, u16 data_len,
                       size_t *msg_len, void **data_pos);

int wpa_sm_mlme_setprotection(struct wpa_sm *sm, const u8 *addr,
                              int protect_type, int key_type);

void wpa_sm_deauthenticate(struct wpa_sm *sm, u8 reason_code);

void wpa_sm_disassociate(struct wpa_sm *sm, int reason_code);

int wpa_sm_get_beacon_ie(struct wpa_sm *sm);

void wpa_sm_free_eapol(u8 *buffer);

#endif /* WPAS_GLUE_H */
