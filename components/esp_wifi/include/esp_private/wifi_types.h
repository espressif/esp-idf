// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _WIFI_TYPES_H
#define _WIFI_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief WiFi ioctl command type
  *
  */
typedef enum {
    WIFI_IOCTL_SET_STA_HT2040_COEX = 1, /**< Set the configuration of STA's HT2040 coexist management */
    WIFI_IOCTL_GET_STA_HT2040_COEX,     /**< Get the configuration of STA's HT2040 coexist management */
    WIFI_IOCTL_MAX,
} wifi_ioctl_cmd_t;

/**
 * @brief Configuration for STA's HT2040 coexist management
 *
 */
typedef struct {
    int enable;                         /**< Indicate whether STA's HT2040 coexist management is enabled or not */
} wifi_ht2040_coex_t;

/**
  * @brief Configuration for WiFi ioctl
  *
  */
typedef struct {
    union {
        wifi_ht2040_coex_t ht2040_coex; /**< Configuration of STA's HT2040 coexist management */
    } data;                             /**< Configuration of ioctl command */
} wifi_ioctl_config_t;

#ifdef __cplusplus
}
#endif

#endif
