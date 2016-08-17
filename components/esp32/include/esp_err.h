// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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
#ifndef __ESP_ERR_H__
#define __ESP_ERR_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int32_t esp_err_t;

/* Definitions for error constants. */

#define ESP_OK          0
#define ESP_FAIL        -1

#define ESP_ERR_NO_MEM  0x101

#ifdef __cplusplus
}
#endif

#endif /* __ESP_ERR_H__ */
