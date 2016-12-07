/******************************************************************************
 *
 *  Copyright (C) 2015 Google, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

#ifndef _INTEROP_DATABASE_H_
#define _INTEROP_DATABASE_H_

#include "interop.h"

typedef struct {
    bt_bdaddr_t addr;
    uint8_t len;
    interop_feature_t feature;
} interop_entry_t;

static const interop_entry_t interop_database[] = {
    // Nexus Remote (Spike)
    // Note: May affect other Asus brand devices
    {{{0x08, 0x62, 0x66,    0,    0,    0}}, 3, INTEROP_DISABLE_LE_SECURE_CONNECTIONS},
    {{{0x38, 0x2c, 0x4a, 0xc9,    0,    0}}, 4, INTEROP_DISABLE_LE_SECURE_CONNECTIONS},
    {{{0x38, 0x2c, 0x4a, 0xe6,    0,    0}}, 4, INTEROP_DISABLE_LE_SECURE_CONNECTIONS},
    {{{0x54, 0xa0, 0x50, 0xd9,    0,    0}}, 4, INTEROP_DISABLE_LE_SECURE_CONNECTIONS},
    {{{0xac, 0x9e, 0x17,    0,    0,    0}}, 3, INTEROP_DISABLE_LE_SECURE_CONNECTIONS},
    {{{0xf0, 0x79, 0x59,    0,    0,    0}}, 3, INTEROP_DISABLE_LE_SECURE_CONNECTIONS},

    // Motorola Key Link
    {{{0x1c, 0x96, 0x5a,    0,    0,    0}}, 3, INTEROP_DISABLE_LE_SECURE_CONNECTIONS},

    // Flic smart button
    {{{0x80, 0xe4, 0xda, 0x70,    0,    0}}, 4, INTEROP_DISABLE_LE_SECURE_CONNECTIONS},

    // BMW car kits (Harman/Becker)
    {{{0x9c, 0xdf, 0x03,    0,    0,    0}}, 3, INTEROP_AUTO_RETRY_PAIRING}
};

#endif /*_INTEROP_DATABASE_H_*/
