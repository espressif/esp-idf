#include "no_warn_host.h"
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
#include <string.h>

const ip_addr_t ip_addr_any;
const ip_addr_t ip_addr_broadcast;
struct ip_globals ip_data;
struct netif *netif_list;
struct netif mynetif;
ip4_addr_t server_ip;

//
// Dependency injected test functions
void dhcp_test_dhcp_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);
void dhcp_test_init_di();

//
// Test starts here
//
int main(int argc, char** argv)
{
    uint8_t *buf;
    struct pbuf *p;
    FILE *file;
    size_t len = 1460;

    dhcp_test_init_di();
    
    mynetif.flags = NETIF_FLAG_UP | NETIF_FLAG_ETHARP;
    mynetif.mtu = 576;


    IP4_ADDR(&server_ip, 192,168,4,1);
    dhcp_start(&mynetif);

    ip_data.current_input_netif = &mynetif;
    ip_data.current_netif = &mynetif;

#ifdef INSTR_IS_OFF
    p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
    buf = p->payload;
    memset(buf, 0, 1460);
    if (argc != 2)
    {
        printf("Non-instrumentation mode: please supply a file name created by AFL to reproduce crash\n");
        return 1;
    }
    //
    // Note: parameter1 is a file (mangled packet) which caused the crash
    file = fopen(argv[1], "r");
    if (file) {
    len = fread(buf, 1, 1460, file);
    }
    fclose(file);
    int i;
    for (i=0; i<1; i++) {
#else
    while (__AFL_LOOP(1000)) {
        p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
        buf = p->payload;
        memset(buf, 0, 1460);
        size_t len = read(0, buf, 1460);
#endif
        p->len = len;
        p->tot_len = len;
        p->next = NULL;

        dhcp_test_dhcp_recv(NULL, NULL, p, &ip_addr_any, 0);
    }


    
    return 0;
}
