/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "debug/lwip_debug.h"
#include "lwip/api.h"
#include "lwip/netbuf.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/stats.h"
#include "lwip/priv/memp_priv.h"
#include "lwip/memp.h"
#include "esp_log.h"

#if CONFIG_LWIP_IPV6 && CONFIG_LWIP_IPV4
#define DBG_LWIP_IP_SHOW(info, ip)  ESP_LWIP_LOGI("%s type=%" PRIu8 " ip=%" PRIX32, (info), (ip).type, (ip).u_addr.ip4.addr)
#elif CONFIG_LWIP_IPV4
#define DBG_LWIP_IP_SHOW(info, ip)  ESP_LWIP_LOGI("%s type=%" PRIu8 " ip=%" PRIX32, (info), IPADDR_TYPE_V4, (ip).addr)
#elif CONFIG_LWIP_IPV6
#define DBG_LWIP_IP_SHOW(info, ip)  ESP_LWIP_LOGI("%s type=%" PRIu8 " ip=%" PRIX32 ", %" PRIX32 ", %" PRIX32 ", %" PRIX32 , (info), IPADDR_TYPE_V6, (ip).addr[0], (ip).addr[1], (ip).addr[2], (ip).addr[3])
#endif
#define DBG_LWIP_IP_PCB_SHOW(pcb) \
        DBG_LWIP_IP_SHOW("local ip", (pcb)->local_ip);\
        DBG_LWIP_IP_SHOW("remote ip", (pcb)->remote_ip);\
        ESP_LWIP_LOGI("so_options=%x, tos=%" PRIu8 " ttl=%" PRIu8, (pcb)->so_options, (pcb)->tos, (pcb)->ttl)

#define DBG_LWIP_SEG_SHOW(seg) while(seg) { ESP_LWIP_LOGI("\tseg=%p next=%p pbuf=%p flags=%x", (seg), (seg)->next, (seg)->p, (seg)->flags); (seg)=(seg)->next;}
#define DBG_LWIP_ITEM_NUMBER_PER_LINE 9

#if ESP_STATS_TCP
static void dbg_lwip_tcp_pcb_cnt_show(struct tcp_pcb *pcb)
{
    int len = 0;
    char *buf;
    char *p;
    int i;

    buf = mem_malloc(512);
    if (!buf) {
        return;
    }

    p = buf;
    len += sprintf(p + len, "%11s", "tcp_retry: ");
    for (i=0; i<TCP_MAXRTX; i++) {
        len += sprintf(p + len, "%-2d=%-5d ", i+1, pcb->retry_cnt[i]);
    }
    ESP_LWIP_LOGI("%s", buf);
    p = buf;
    len = 0;
    len += sprintf(p + len, "%11s", "tcp_rto#0:");
    for (i=0; i<ESP_STATS_TCP_ARRAY_SIZE; i++) {
        if ((i!=0) && (i%DBG_LWIP_ITEM_NUMBER_PER_LINE==0)) {
            ESP_LWIP_LOGI("%s", buf);
            len = 0;
            p = buf;
            len += sprintf(p + len, "%11s", "tcp_rto#1:");
        }
        len += sprintf(p + len, "%-2d=%-5d ", i+1, pcb->rto_cnt[i]);
    }
    ESP_LWIP_LOGI("%s", buf);

    free(buf);
}
#endif

static void dbg_lwip_tcp_pcb_one_show(struct tcp_pcb* pcb)
{
    struct tcp_seg *seg = NULL;

    if (!pcb) {
        return;
    }

    ESP_LWIP_LOGI("pcb=%p next=%p cb_arg=%p", pcb, pcb->next, pcb->callback_arg);
    DBG_LWIP_IP_PCB_SHOW(pcb);
    ESP_LWIP_LOGI("state=%x", pcb->state);
    ESP_LWIP_LOGI("prio=%d", pcb->prio);
    ESP_LWIP_LOGI("local_port=%d, remote_port=%d", pcb->local_port, pcb->remote_port);
    ESP_LWIP_LOGI("flags=%x", pcb->flags);
    ESP_LWIP_LOGI("pooltmr=%d pollinterval=%d, last_tmr=%d tmr=%" PRIu32 " rtmer=%d", pcb->polltmr, pcb->pollinterval, pcb->last_timer, pcb->tmr, pcb->rtime);
    ESP_LWIP_LOGI("recv_nxt=%" PRIu32 " recv_wnd=%"TCPWNDSIZE_F" recv_ann_wnd=%"TCPWNDSIZE_F" recv_ann_right_edge=%" PRIu32, pcb->rcv_nxt, pcb->rcv_wnd, pcb->rcv_ann_wnd, pcb->rcv_ann_right_edge);
    ESP_LWIP_LOGI("mss=%d", pcb->mss);
    ESP_LWIP_LOGI("rttest=%" PRIu32 " rtseq=%" PRIu32 " sa=%d sv=%d", pcb->rttest, pcb->rtseq, pcb->sa, pcb->sv);
    ESP_LWIP_LOGI("rto=%d nrtx=%d", pcb->rto, pcb->nrtx);
    ESP_LWIP_LOGI("dupacks=%d lastack=%" PRIu32, pcb->dupacks, pcb->lastack);
#if ESP_PER_SOC_TCP_WND
    ESP_LWIP_LOGI("per_soc_window=%d per_soc_snd_buf=%d", pcb->per_soc_tcp_wnd, pcb->per_soc_tcp_snd_buf);
#endif
    ESP_LWIP_LOGI("cwnd=%"TCPWNDSIZE_F" ssthreash=%"TCPWNDSIZE_F, pcb->cwnd, pcb->ssthresh);
    ESP_LWIP_LOGI("snd_next=%" PRIu32 " snd_wl1=%" PRIu32 " snd_wl2=%" PRIu32, pcb->snd_nxt, pcb->snd_wl1, pcb->snd_wl2);
    ESP_LWIP_LOGI("snd_lbb=%" PRIu32 " snd_wnd=%"TCPWNDSIZE_F" snd_wnd_max=%"TCPWNDSIZE_F, pcb->snd_lbb, pcb->snd_wnd, pcb->snd_wnd_max);
    //ESP_LWIP_LOGI("acked=%d", pcb->acked);
    ESP_LWIP_LOGI("snd_buf=%"TCPWNDSIZE_F" snd_queuelen=%d", pcb->snd_buf, pcb->snd_queuelen);
    ESP_LWIP_LOGI("unsent_oversize=%d", pcb->unsent_oversize);
    ESP_LWIP_LOGI("keep_idle=%" PRIu32 " keep_intvl=%" PRIu32 " keep_cnt=%" PRIu32, pcb->keep_idle, pcb->keep_intvl, pcb->keep_cnt);
    ESP_LWIP_LOGI("persist_cnt=%d persist_backoff=%d", pcb->persist_cnt, pcb->persist_backoff);
    ESP_LWIP_LOGI("keep_cnt_sent=%d", pcb->keep_cnt_sent);

    ESP_LWIP_LOGI("unsent segments:");
    seg = pcb->unsent;
    DBG_LWIP_SEG_SHOW(seg)

    ESP_LWIP_LOGI("unacked segments:");
    seg = pcb->unacked;
    DBG_LWIP_SEG_SHOW(seg);

#if TCP_QUEUE_OOSEQ
    ESP_LWIP_LOGI("ooseq segments:");
    seg = pcb->ooseq;
    DBG_LWIP_SEG_SHOW(seg);
#endif

    ESP_LWIP_LOGI("refused data=%p", pcb->refused_data);

#if ESP_STATS_TCP
    dbg_lwip_tcp_pcb_cnt_show(pcb);
#endif
}

static void dbg_lwip_tcp_pcb_list_show(struct tcp_pcb* pcb)
{
    while(pcb){
        dbg_lwip_tcp_pcb_one_show(pcb);
        pcb = pcb->next;
    }
}

extern struct tcp_pcb *tcp_bound_pcbs;
extern struct tcp_pcb *tcp_active_pcbs;
extern struct tcp_pcb *tcp_tw_pcbs;
void dbg_lwip_tcp_pcb_show(void)
{
    ESP_LWIP_LOGI("-------------active pcbs------------");
    dbg_lwip_tcp_pcb_list_show(tcp_active_pcbs);
    ESP_LWIP_LOGI("-------------bound pcbs-------------");
    dbg_lwip_tcp_pcb_list_show(tcp_bound_pcbs);
    ESP_LWIP_LOGI("-------------tw     pcbs------------");
    dbg_lwip_tcp_pcb_list_show(tcp_tw_pcbs);
}

void dbg_lwip_udp_pcb_one_show(struct udp_pcb *pcb)
{
    ESP_LWIP_LOGI("pcb=%p next=%p", pcb, (void*)pcb->next);
    DBG_LWIP_IP_PCB_SHOW(pcb);
    ESP_LWIP_LOGI("flags=%x", pcb->flags);
    ESP_LWIP_LOGI("local_port=%d remote_port=%d", pcb->local_port, pcb->remote_port);
    ESP_LWIP_LOGI("recv cb=%p recv_arg=%p", pcb->recv, pcb->recv_arg);
}

extern struct udp_pcb *udp_pcbs;
void dbg_lwip_udp_pcb_show(void)
{
    struct udp_pcb *pcb = udp_pcbs;

    while (pcb){
        dbg_lwip_udp_pcb_one_show(pcb);
        pcb = pcb->next;
    }
}

void dbg_lwip_tcp_rxtx_show(void)
{
    ESP_LWIP_LOGI("TBC");
}

void dbg_lwip_udp_rxtx_show(void)
{
    ESP_LWIP_LOGI("TBC");
}

void dbg_lwip_stats_show(void)
{
    TCP_STATS_DISPLAY();
    UDP_STATS_DISPLAY();
    ICMP_STATS_DISPLAY();
    IGMP_STATS_DISPLAY();
    IP_STATS_DISPLAY();
    IPFRAG_STATS_DISPLAY();
    ETHARP_STATS_DISPLAY();
    LINK_STATS_DISPLAY();
    MEM_STATS_DISPLAY();
    SYS_STATS_DISPLAY();
    IP6_STATS_DISPLAY();
    ICMP6_STATS_DISPLAY();
    IP6_FRAG_STATS_DISPLAY();
    MLD6_STATS_DISPLAY();
    ND6_STATS_DISPLAY();
}

#if (ESP_STATS_MEM == 1)

uint32_t g_lwip_mem_cnt[MEMP_MAX][2];
extern const struct memp_desc * const memp_pools[MEMP_MAX];

void dbg_lwip_cnt_show(void)
{
    int i=0;

    ESP_LWIP_LOGI("-----lwip memory counter-----");
    ESP_LWIP_LOGI("%6s %8s %8s", "index", "alloc", "free");
    for (i=0; i<MEMP_MAX; i++){
        ESP_LWIP_LOGI("%6" U32_F " %8" U32_F " %8" U32_F, (u32_t)i, g_lwip_mem_cnt[i][0], g_lwip_mem_cnt[i][1]);
    }
}


#endif
