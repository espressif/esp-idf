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

#ifndef __BTC_UTIL_H__
#define __BTC_UTIL_H__

#include <stdbool.h>
#include "bt_types.h"
#include "bt_defs.h"

/*******************************************************************************
**  Constants & Macros
********************************************************************************/
#define CASE_RETURN_STR(const) case const: return #const;

/*******************************************************************************
**  Type definitions for callback functions
********************************************************************************/
typedef char bdstr_t[18];


/*******************************************************************************
**  Functions
********************************************************************************/
const char *dump_rc_event(UINT8 event);
const char *dump_rc_notification_event_id(UINT8 event_id);
const char *dump_rc_pdu(UINT8 pdu);

UINT32 devclass2uint(DEV_CLASS dev_class);
void uint2devclass(UINT32 dev, DEV_CLASS dev_class);
void uuid16_to_uuid128(uint16_t uuid16, bt_uuid_t *uuid128);

void uuid_to_string_legacy(bt_uuid_t *p_uuid, char *str);
void string_to_uuid(char *str, bt_uuid_t *p_uuid);

#endif /*  __BTC_UTIL_H__ */
