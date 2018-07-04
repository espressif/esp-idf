// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _WL_Ext_Cfg_H_
#define _WL_Ext_Cfg_H_
#include "WL_Config.h"

typedef struct WL_Ext_Cfg_s : public WL_Config_s {
    uint32_t fat_sector_size;   /*!< virtual sector size*/
} wl_ext_cfg_t;

#endif // _WL_Ext_Cfg_H_