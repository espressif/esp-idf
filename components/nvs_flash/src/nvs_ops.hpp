// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
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
#ifndef nvs_ops_hpp
#define nvs_ops_hpp

#include "esp_err.h"

namespace nvs
{
    esp_err_t nvs_flash_write(size_t destAddr, const void *srcAddr, size_t size);
    esp_err_t nvs_flash_read(size_t srcAddr, void *destAddr, size_t size);

} // namespace nvs


#endif /* nvs_ops_hpp */
