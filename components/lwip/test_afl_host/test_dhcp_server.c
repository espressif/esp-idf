#include "no_warn_host.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include <string.h>
#include <stdio.h>
#include "dhcpserver/dhcpserver.h"

const ip_addr_t ip_addr_any;
ip4_addr_t server_ip;
struct netif mynetif;

// dhcps callback
void dhcp_test_dhcps_cb (void* cb_arg, u8_t client_ip[4], u8_t client_mac[6]) {}

// Dependency injected static function to pass the packet into parser
void dhcp_test_handle_dhcp(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);
void dhcp_test_init_di(void);

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
    dhcps_t *dhcps = dhcps_new();
    dhcps_set_new_lease_cb(dhcps, dhcp_test_dhcps_cb, NULL);
    dhcps_start(dhcps, &mynetif, server_ip);

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

        dhcp_test_handle_dhcp(dhcps, NULL, p, &ip_addr_any, 0);
    }
    dhcps_stop(dhcps, &mynetif);
    dhcps_delete(dhcps);
    return 0;
}
