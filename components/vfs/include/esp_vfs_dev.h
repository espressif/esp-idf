// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#ifndef __ESP_VFS_DEV_H__
#define __ESP_VFS_DEV_H__

#include "esp_vfs.h"

/**
 * @brief add /dev/uart virtual filesystem driver
 *
 * This function is called from startup code to enable serial output
 */
void esp_vfs_dev_uart_register();


#endif //__ESP_VFS_DEV_H__
