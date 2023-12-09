/*
 * SPDX-FileCopyrightText: 2001-2004 Swedish Institute of Computer Science
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * SPDX-FileContributor: 2023 Espressif Systems (Shanghai) CO LTD
 */
#include "pppif.h"
#include "lwip/tcpip.h"

/*
 * Similar to pppos_input_tcpip() from lwip's ppp netif, but instead
 * of PBUF_POOL, we use PBUF_RAM type of the pbuf we pass to the stack
 */
err_t pppos_input_tcpip_as_ram_pbuf(ppp_pcb *ppp, u8_t *s, int l)
{
    struct pbuf *p = pbuf_alloc(PBUF_RAW, l, PBUF_RAM);
    if (!p) {
        return ERR_MEM;
    }
    pbuf_take(p, s, l);

    err_t err = tcpip_inpkt(p, ppp_netif(ppp), pppos_input_sys);
    if (err != ERR_OK) {
        pbuf_free(p);
    }
    return err;
}
