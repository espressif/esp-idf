/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#ifndef _LWIP_DEBUG_H
#define _LWIP_DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

void dbg_lwip_tcp_pcb_show(void);
void dbg_lwip_udp_pcb_show(void);
void dbg_lwip_tcp_rxtx_show(void);
void dbg_lwip_udp_rxtx_show(void);
void dbg_lwip_mem_cnt_show(void);

#ifdef __cplusplus
}
#endif

#endif // _LWIP_DEBUG_H
