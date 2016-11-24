/******************************************************************************
 *
 *  Copyright (c) 2014 The Android Open Source Project
 *  Copyright (C) 2009-2012 Broadcom Corporation
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

#ifndef BTIF_UTIL_H
#define BTIF_UTIL_H

// #include <hardware/bluetooth.h>
// #include <hardware/bt_hf.h>
#include <stdbool.h>
// #include <sys/time.h>

#include "bt_types.h"
// #include "bt_utils.h"
#include "bt_defs.h"

/*******************************************************************************
**  Constants & Macros
********************************************************************************/
/*******************************************************************************
**  Type definitions for callback functions
********************************************************************************/

typedef char bdstr_t[18];


/*******************************************************************************
**  Functions
********************************************************************************/
UINT32 devclass2uint(DEV_CLASS dev_class);
void uint2devclass(UINT32 dev, DEV_CLASS dev_class);
void uuid16_to_uuid128(uint16_t uuid16, bt_uuid_t* uuid128);

void uuid_to_string_legacy(bt_uuid_t *p_uuid, char *str);
void string_to_uuid(char *str, bt_uuid_t *p_uuid);

#endif /* BTIF_UTIL_H */
