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


#ifndef __BTC_COMMON_H__
#define __BTC_COMMON_H__

#include "common/bt_trace.h"
#include "stack/bt_types.h"
#include "osi/osi.h"

#define BTC_ASSERTC(cond, msg, val) if (!(cond)) { LOG_ERROR( \
    "### ASSERT : %s line %d %s (%d) ###", __FILE__, __LINE__, msg, val);}

#define BTC_HAL_CBACK(P_CB, P_CBACK, ...)\
    if (P_CB && P_CB->P_CBACK) {            \
        LOG_INFO("HAL %s->%s", #P_CB, #P_CBACK); \
        P_CB->P_CBACK(__VA_ARGS__);         \
    }                                       \
    else {                                  \
        BTC_ASSERTC(0, "Callback is NULL", 0);  \
    }

#endif /* __BTC_COMMON_H__ */
