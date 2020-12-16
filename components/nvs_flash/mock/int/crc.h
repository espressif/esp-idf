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
#ifndef crc_h
#define crc_h

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Mock function to replace ESP ROM function used in IDF with a Linux implementation.
 * Note: the name MUST have the prefix esp_rom_* since tools/ci/check_rom_apis.sh checks and complains otherwise.
 */
uint32_t esp_rom_crc32_le(uint32_t crc, const uint8_t* buf, size_t len);

#ifdef __cplusplus
}
#endif


#endif /* crc_h */
