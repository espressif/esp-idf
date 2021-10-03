// Copyright 2021 Espressif Systems (Shanghai) CO LTD
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
// limitations under the License

#ifndef _OPENTHREAD_LWIP_IF_H_
#define _OPENTHREAD_LWIP_IF_H_

#include "lwip/netif.h"
#include "lwip/err.h"
#include "lwip/ip6.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This function initializes the OpenThread lwIP network interface.
 *
 * @param[in] netif     The lwIP interface to initialize
 *
 * @return
 *      - ERR_OK
 *
 */
err_t openthread_netif_init(struct netif *netif);

/**
 * @brief This function sends the buffer to the lwIP network interface
 *
 * @param[in] netif     The lwIP interface to send to.
 * @param[in] buffer    The packet to send.
 * @param[in] len       The length of the buffer.
 * @param[in] eb        Unused.
 *
 */
void openthread_netif_input(void *netif, void *buffer, size_t len, void *eb);

#ifdef __cplusplus
}
#endif

#endif /* _OPENTHREAD_LWIP_IF_H_ */
