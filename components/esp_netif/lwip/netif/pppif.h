/*
 * SPDX-FileCopyrightText: 2001-2004 Swedish Institute of Computer Science
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * SPDX-FileContributor: 2023 Espressif Systems (Shanghai) CO LTD
 */
#include "lwip/pbuf.h"
#include "netif/ppp/pppos.h"

/** Pass received raw characters to PPPoS to be decoded through lwIP TCPIP thread.
 *
 * This function uses allocated packet buffers of exact size for input to TCP/IP stack
 * to consume less memory in download mode, compared to the pppos_input_tcpip() from lwIP.
 * (original implementation uses `PBUF_POOL`, which on ESP port uses standard malloc
 * to allocate a fixed size pbuf)
 *
 * @param ppp PPP descriptor index, returned by pppos_create()
 * @param s received data
 * @param l length of received data
 */
 err_t pppos_input_tcpip_as_ram_pbuf(ppp_pcb *ppp, u8_t *s, int l);
