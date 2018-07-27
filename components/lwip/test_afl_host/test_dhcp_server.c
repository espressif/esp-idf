#include "no_warn_host.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "tcpip_adapter.h"
#include <string.h>
#include <stdio.h>

const ip_addr_t ip_addr_any;
ip4_addr_t server_ip;
struct netif mynetif;

// dhcps callback
void dhcp_test_dhcps_cb (u8_t client_ip[4]) {}

// Dependency injected static function to pass the packet into parser
void dhcp_test_handle_dhcp(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);
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

    IP4_ADDR(&server_ip, 192,168,4,1);
    dhcps_set_new_lease_cb(dhcp_test_dhcps_cb);
    dhcps_start(&mynetif, server_ip);

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

        dhcp_test_handle_dhcp(NULL, NULL, p, &ip_addr_any, 0);
    }
    return 0;
}
