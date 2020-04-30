// Copyright 2019-2020 Espressif Systems (Shanghai) PTE LTD
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

#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int chip_usb_dw_init(void);
int chip_usb_dw_did_persist(void);
void chip_usb_dw_prepare_persist(void);
uint32_t chip_usb_get_persist_flags(void);
void chip_usb_set_persist_flags(uint32_t flags);

#ifdef __cplusplus
}
#endif
