#include "no_warn_host.h"
#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "tcpip_adapter.h"
#include <string.h>
#include <stdio.h>

const ip_addr_t ip_addr_any;
const ip_addr_t ip_addr_broadcast;
const ip_addr_t ip_addr_any_type;
struct ip_globals ip_data;
struct netif *netif_list;
struct udp_pcb mock_pcb;
uint32_t g_random_numbers[8] = {0};
uint32_t g_random_numbers_cnt = 0;


struct pbuf* pbuf_skip(struct pbuf* in, u16_t in_offset, u16_t* out_offset)
{
    u16_t offset_left = in_offset;
    struct pbuf* q = in;

    /* get the correct pbuf */
    while ((q != NULL) && (q->len <= offset_left)) {
        offset_left -= q->len;
        q = q->next;
    }
    if (out_offset != NULL) {
        *out_offset = offset_left;
    }
    return q;
}

int pbuf_try_get_at(const struct pbuf* p, u16_t offset)
{
    u16_t q_idx;
    struct pbuf* q = pbuf_skip(p, offset, &q_idx);

    /* return requested data if pbuf is OK */
    if ((q != NULL) && (q->len > q_idx)) {
        return ((u8_t*)q->payload)[q_idx];
    }
    return -1;
}

void pbuf_put_at(struct pbuf* p, u16_t offset, u8_t data)
{
    u16_t q_idx;
    struct pbuf* q = pbuf_skip(p, offset, &q_idx);

    /* write requested data if pbuf is OK */
    if ((q != NULL) && (q->len > q_idx)) {
        ((u8_t*)q->payload)[q_idx] = data;
    }
}

u8_t pbuf_get_at(const struct pbuf* p, u16_t offset)
{
    u16_t q_idx;
    struct pbuf* q = pbuf_skip(p, offset, &q_idx);

    /* return requested data if pbuf is OK */
    if ((q != NULL) && (q->len > q_idx)) {
        return ((u8_t*)q->payload)[q_idx];
    }
    return 0;
}

err_t pbuf_take(struct pbuf *buf, const void *dataptr, u16_t len)
{
    return ERR_OK;
}

err_t pbuf_take_at(struct pbuf *buf, const void *dataptr, u16_t len, u16_t offset)
{
    return ERR_OK;
}

struct udp_pcb * udp_new_ip_type(u8_t type)
{
    return &mock_pcb;
}

esp_err_t tcpip_adapter_get_ip_info(tcpip_adapter_if_t tcpip_if, tcpip_adapter_ip_info_t *ip_info)
{
    return ESP_OK;
}

struct pbuf * pbuf_alloc(pbuf_layer layer, u16_t length, pbuf_type type)
{
    struct pbuf * p;
    p = (struct pbuf *)malloc(MEMP_PBUF_POOL);
    p->tot_len = length;
    p->next = NULL;
    p->type_internal = PBUF_POOL;
    p->len = length;
    p->payload = malloc(length);
    return p;
}

u8_t pbuf_free(struct pbuf *p)
{
    if (p) {
        if (p->payload) {
            free(p->payload);
            p->payload = NULL;
        }
        free (p);
        p = NULL;
    }
    return 1;
}

err_t udp_sendto(struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *dst_ip, u16_t dst_port)
{
    return ESP_OK;
}

void udp_remove(struct udp_pcb *pcb)
{
    if (pcb == NULL)
    {
       free(pcb);
    }
}

struct udp_pcb *udp_new(void)
{
    return malloc(sizeof(struct udp_pcb));
}

err_t udp_bind(struct udp_pcb *pcb, const ip_addr_t *ipaddr, u16_t port)
{
    return ESP_OK;
}

void udp_recv(struct udp_pcb *pcb, udp_recv_fn recv, void *recv_arg)
{
}

void udp_disconnect(struct udp_pcb *pcb)
{
}

#ifndef NOT_MOCK_DNS
void dns_setserver(u8_t numdns, const ip_addr_t *dnsserver)
{
}
#endif

uint32_t esp_random(void)
{
    // Preparation for injecting favorable random numbers 
    return g_random_numbers[g_random_numbers_cnt++ % 8];
}

err_t etharp_query(struct netif *netif, const ip4_addr_t *ipaddr, struct pbuf *q)
{
    return ESP_OK;
}

void netif_set_addr(struct netif *netif, const ip4_addr_t *ipaddr, const ip4_addr_t *netmask,
                    const ip4_addr_t *gw)
{
}

void pbuf_realloc(struct pbuf *p, u16_t size)
{
    if (p != NULL)
    {
        uint8_t *buf = malloc(size);
        free(p->payload);
        p->payload = buf;
        p->len = size;
        p->tot_len = size;
    }
}

u16_t pbuf_copy_partial(const struct pbuf *buf, void *dataptr, u16_t len, u16_t offset)
{
    struct pbuf *p;
    u16_t left;
    u16_t buf_copy_len;
    u16_t copied_total = 0;

    LWIP_ERROR("pbuf_copy_partial: invalid buf", (buf != NULL), return 0;);
    LWIP_ERROR("pbuf_copy_partial: invalid dataptr", (dataptr != NULL), return 0;);

    left = 0;

    if ((buf == NULL) || (dataptr == NULL)) {
        return 0;
    }

    /* Note some systems use byte copy if dataptr or one of the pbuf payload pointers are unaligned. */
    for (p = buf; len != 0 && p != NULL; p = p->next) {
        if ((offset != 0) && (offset >= p->len)) {
        /* don't copy from this buffer -> on to the next */
        offset -= p->len;
        } else {
        /* copy from this buffer. maybe only partially. */
        buf_copy_len = p->len - offset;
        if (buf_copy_len > len)
            buf_copy_len = len;
        /* copy the necessary parts of the buffer */
        MEMCPY(&((char*)dataptr)[left], &((char*)p->payload)[offset], buf_copy_len);
        copied_total += buf_copy_len;
        left += buf_copy_len;
        len -= buf_copy_len;
        offset = 0;
        }
    }
    return copied_total;
}

err_t udp_connect(struct udp_pcb *pcb, const ip_addr_t *ipaddr, u16_t port)
{
    return ESP_OK;
}

err_t udp_sendto_if(struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *dst_ip, u16_t dst_port, struct netif *netif)
{
    return ESP_OK;
}

err_t udp_sendto_if_src(struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *dst_ip, u16_t dst_port, struct netif *netif, const ip_addr_t *src_ip)
{
    return ESP_OK;
}

void * mem_malloc(mem_size_t size)
{
    return malloc(size);
}

void mem_free(void *rmem)
{
    free(rmem);
}