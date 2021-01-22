// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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

#pragma once

#ifndef _RSA_SIGN_ALT_H_
#define _RSA_SIGN_ALT_H_

#ifdef __cpluscplus
extern "C" {
#endif

#ifdef CONFIG_ESP_TLS_USE_DS_PERIPHERAL
#include "esp_ds/esp_rsa_sign_alt.h"
#else

#error "DS configuration flags not activated, please enable required menuconfig flags"

#endif

#ifdef __cpluscplus
}
#endif

#endif
