/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _WL_Ext_Cfg_H_
#define _WL_Ext_Cfg_H_
#include "WL_Config.h"

typedef struct WL_Ext_Cfg_s : public WL_Config_s {
    uint32_t fat_sector_size;   /*!< virtual sector size*/
} wl_ext_cfg_t;

#endif // _WL_Ext_Cfg_H_
