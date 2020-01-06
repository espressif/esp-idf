// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _NET_SUITE_STDINOUT_H
#define _NET_SUITE_STDINOUT_H

/**
 * @brief Gets one packet from stdin
 *
 * @param packet ptr to the packet buffer
 * @param max_len maximum size of allocated data buffer
 *
 * @return actual size of received packet (-1 in case of error)
 */

ssize_t netsuite_io_get_packet(char *packet, size_t max_len);

/**
 * @brief Initializes the I/O object handle to be attached to esp-netif instance
 *
 * @return I/O object pointer
 */
void * netsuite_io_new(void);

#endif //_NET_SUITE_STDINOUT_H
