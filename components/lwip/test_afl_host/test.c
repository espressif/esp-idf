#include <stdio.h>
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "tcpip_adapter.h"
#include <string.h>

const ip_addr_t ip_addr_any;
ip4_addr_t server_ip;
struct netif mynetif;

// Dependency injected static function to pass the packet into parser
void dhcp_test_handle_dhcp(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);
void dhcp_test_init_di();

// Starting the test
int main()
{
    uint8_t *buf;
    struct pbuf *p;
    FILE *file;
    size_t len = 1460;

    dhcp_test_init_di();

    p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
    buf = p->payload;

    IP4_ADDR(&server_ip, 192,168,4,1);
    dhcps_start(&mynetif, server_ip);

#ifdef SIM
    memset(buf, 0, 1460);

    file = fopen("in/data1.bin", "r");
    if (file) {
    len = fread(buf, 1, 1460, file);
    }
    fclose(file);
    int i;
    for (i=0; i<1; i++) {
#else
    while (__AFL_LOOP(1000)) {
        memset(buf, 0, 1460);
        size_t len = read(0, buf, 1460);
#endif
        p->len = len;
        p->tot_len = len;
        p->next = NULL;

        dhcp_test_handle_dhcp(NULL, NULL, p, &ip_addr_any, 0);
    }
    return 0;
}
