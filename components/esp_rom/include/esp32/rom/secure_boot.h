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

#ifndef _ROM_SECURE_BOOT_H_
#define _ROM_SECURE_BOOT_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void ets_secure_boot_start(void);

void ets_secure_boot_finish(void);

void ets_secure_boot_hash(const uint32_t *buf);

void ets_secure_boot_obtain(void);

int ets_secure_boot_check(uint32_t *buf);

void ets_secure_boot_rd_iv(uint32_t *buf);

void ets_secure_boot_rd_abstract(uint32_t *buf);

bool ets_secure_boot_check_start(uint8_t abs_index, uint32_t iv_addr);

int ets_secure_boot_check_finish(uint32_t *abstract);

#ifdef __cplusplus
}
#endif

#endif /* _ROM_SECURE_BOOT_H_ */
