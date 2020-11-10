/*
 * dns.c dependecy injection -- preincluded to inject interface test functions into static variables
 *
 */
#include "no_warn_host.h"

#include "lwip/opt.h"
#include "lwip/udp.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/dns.h"
#include "lwip/ip_addr.h"

#define ipaddr_aton(cp, addr)                   ip4addr_aton(cp, addr)

extern uint32_t g_random_numbers[8];
extern uint32_t g_random_numbers_cnt;

#ifndef BUILDING_DEF

void __assert_func(const char *file, int line, const char *func, const char *expr)
{
    printf("Assert failed in %s, %s:%d (%s)", func, file, line, expr);
    abort();
}

int ip4addr_aton(const char *cp, ip4_addr_t *addr)
{
    return 0;
}

static err_t dns_enqueue(const char *name, size_t hostnamelen, dns_found_callback found, void *callback_arg, u8_t dns_addrtype);
static void dns_recv(void *s, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);

void (*dns_test_static_dns_recv)(void *s, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port) = NULL;
err_t (*dns_test_static_dns_enqueue)(const char *name, size_t hostnamelen, dns_found_callback found, void *callback_arg, u8_t dns_addrtype) = NULL;

void dns_test_init_di(void)
{
    dns_test_static_dns_recv = dns_recv;
    dns_test_static_dns_enqueue = dns_enqueue;
}


err_t dns_test_dns_enqueue(const char *name, size_t hostnamelen, dns_found_callback found, void *callback_arg, u8_t dns_addrtype)
{
    return dns_test_static_dns_enqueue(name, hostnamelen, found, callback_arg, dns_addrtype);
}

void dns_test_dns_recv(void *s, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    dns_test_static_dns_recv(s, pcb, p, addr, port);
}

void dns_test_inject_port_and_txid(int port, int txid)
{
    // inject random numbers
    g_random_numbers[0] = port; //for port
    g_random_numbers[1] = txid; //for txid
    g_random_numbers_cnt = 0;   // let's start with the port
}
#endif
