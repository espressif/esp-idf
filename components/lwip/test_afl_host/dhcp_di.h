#include "no_warn_host.h"
#include <stdlib.h>     // For abort()
#include <stdio.h>      // For printf()
#include "lwip/opt.h"
#include "lwip/stats.h"
#include "lwip/mem.h"
#include "lwip/udp.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "lwip/def.h"
#include "lwip/dhcp.h"
#include "lwip/autoip.h"
#include "lwip/dns.h"
#include "netif/etharp.h"

#ifndef BUILDING_DEF

void __assert_func(const char *file, int line, const char *func, const char *expr)
{
    printf("Assert failed in %s, %s:%d (%s)", func, file, line, expr);
    abort();
}

static void dhcp_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);

void (*dhcp_test_static_dhcp_recv)(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port) = NULL;

void dhcp_test_init_di(void)
{
    dhcp_test_static_dhcp_recv = dhcp_recv;
}

void dhcp_test_dhcp_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    dhcp_test_static_dhcp_recv(arg, pcb, p, addr, port);
}

#endif /* BUILDING_DEF */
