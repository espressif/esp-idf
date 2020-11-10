#include "no_warn_host.h"

#include "lwip/opt.h"
#include "lwip/udp.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/dns.h"
#include "lwip/ip_addr.h"

#include <string.h>

const ip_addr_t ip_addr_any;
const ip_addr_t ip_addr_broadcast;
struct ip_globals ip_data;
struct netif *netif_list;
struct netif mynetif;
ip4_addr_t server_ip;

//
// Dependency injected test functions
void dns_test_dns_recv(void *s, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);
void dns_test_inject_port_and_txid(int port, int txid);

void dns_test_init_di(void);
err_t dns_test_dns_enqueue(const char *name, size_t hostnamelen, dns_found_callback found, void *callback_arg, u8_t dns_addrtype);

//
// Test starts here
//
int main(int argc, char** argv)
{
    uint8_t *buf;
    struct pbuf *p;
    FILE *file;
    size_t len = 1460;

    dns_test_init_di();

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

        // Pretend that the response is from our pending querries
        IP4_ADDR(&server_ip, 8, 8, 8, 8);
        dns_setserver(0, &server_ip);
        dns_test_inject_port_and_txid(1024, (buf[0]<<8) + buf[1]);
        dns_test_dns_enqueue("test", 4, NULL, NULL, 0);

        // Process the packet
        dns_test_dns_recv(NULL, NULL, p, &ip_addr_any, 0);
    }



    return 0;
}
