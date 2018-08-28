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


#ifndef _NETTEST_LWIP_IF_H_
#define _NETTEST_LWIP_IF_H_

#include "lwip/err.h"

#ifdef __cplusplus
extern "C" {
#endif

err_t nettestif_init(struct netif *netif);

void nettestif_input(void *buffer, u16_t len);

#ifdef __cplusplus
}
#endif

#endif /*  _NETTEST_LWIP_IF_H_ */