/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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
