// Copyright 2010-2016 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _ROM_WDT_H_
#define _ROM_WDT_H_

#include "soc.h"
#include "ets_sys.h"

#define WDT_RESET_VALUE         0x73
#define WDT_RESET_LEN           7             //real time: (1<<(WDT_RESET_LEN+1))*pclk
#define WDT_CONTROL_ENABLED     BIT0
#define WDT_MODE_SET(v)         (v)
#define WDT_TARGET_SET(v)       (v)
#define WDT_ADDRESS             0

#define SEC_TO_WDT_TICK(s) (s * WDT_CLK_FREQ)  //it's Pclk clock,44MHz

typedef enum{
    NEXT_OVERFLOW_RESET        = 0,
    NEXT_OVERFLOW_NO_RESET     = 1,
    EACH_OVERFLOW_RESET        = 2,
} WDT_RESP_MODE;

#define WDT_DEFAULT_FEED_INTERVAL       WDT_INTERVAL_SIX_SEC        /* 6 seconds */
#define WDT_DEFAULT_EXPIRE_INTERVAL     WDT_INTERVAL_TWELVE_SEC     /* 12 seconds */

#endif /* _ROM_WDT_H_ */

