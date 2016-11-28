// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "lwip/lwip_debug.h"
#include "lwip/api.h"
#include "lwip/netbuf.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/stats.h"
#include "lwip/priv/memp_priv.h"
#include "lwip/memp.h"

#define DBG_LWIP_IP_SHOW(info, ip)  printf("%s type=%d ip=%x\n", (info), (ip).type, (ip).u_addr.ip4.addr)
#define DBG_LWIP_IP_PCB_SHOW(pcb) \
        DBG_LWIP_IP_SHOW("local ip", (pcb)->local_ip);\
        DBG_LWIP_IP_SHOW("remote ip", (pcb)->local_ip);\
        printf("so_options=%x, tos=%d ttl=%d\n", (pcb)->so_options, (pcb)->tos, (pcb)->ttl)

#define DBG_LWIP_SEG_SHOW(seg) while(seg) { printf("\tseg=%p next=%p pbuf=%p flags=%x\n", (seg), (seg)->next, (seg)->p, (seg)->flags); (seg)=(seg)->next;}
    

static void dbg_lwip_tcp_pcb_one_show(struct tcp_pcb* pcb)
{
    struct tcp_seg *seg = NULL;

    if (!pcb) {
        return;
    }

    printf("\npcb=%p next=%p cb_arg=%p\n", pcb, pcb->next, pcb->callback_arg);
    DBG_LWIP_IP_PCB_SHOW(pcb);
    printf("state=%x\n", pcb->state);
    printf("prio=%d\n", pcb->prio);
    printf("local_port=%d, remote_port=%d\n", pcb->local_port, pcb->remote_port);
    printf("flags=%x\n", pcb->flags);
    printf("pooltmr=%d pollinterval=%d, last_tmr=%d tmr=%d rtmer=%d\n", pcb->polltmr, pcb->pollinterval, pcb->last_timer, pcb->tmr, pcb->rtime);
    printf("recv_nxt=%d recv_wnd=%d recv_ann_wnd=%d recv_ann_right_edge=%d\n", pcb->rcv_nxt, pcb->rcv_wnd, pcb->rcv_ann_wnd, pcb->rcv_ann_right_edge);
    printf("mss=%d\n", pcb->mss);
    printf("rttest=%d rtseq=%d sa=%d sv=%d\n", pcb->rttest, pcb->rtseq, pcb->sa, pcb->sv);
    printf("rto=%d nrtx=%d\n", pcb->rto, pcb->nrtx);
    printf("dupacks=%d lastack=%d\n", pcb->dupacks, pcb->lastack);
#if ESP_PER_SOC_TCP_WND
    printf("per_soc_window=%d per_soc_snd_buf=%d\n", pcb->per_soc_tcp_wnd, pcb->per_soc_tcp_snd_buf);
#endif
    printf("cwnd=%d ssthreash=%d\n", pcb->cwnd, pcb->ssthresh);
    printf("snd_next=%d snd_wl1=%d snd_wl2=%d\n", pcb->snd_nxt, pcb->snd_wl1, pcb->snd_wl2);
    printf("snd_lbb=%d snd_wnd=%d snd_wnd_max=%d\n", pcb->snd_lbb, pcb->snd_wnd, pcb->snd_wnd_max);
    printf("acked=%d\n", pcb->acked);
    printf("snd_buf=%d snd_queuelen=%d\n", pcb->snd_buf, pcb->snd_queuelen);
    printf("unsent_oversize=%d\n", pcb->unsent_oversize);
    printf("keep_idle=%d keep_intvl=%d keep_cnt=%d\n", pcb->keep_idle, pcb->keep_intvl, pcb->keep_cnt);
    printf("persist_cnt=%d persist_backoff=%d\n", pcb->persist_cnt, pcb->persist_backoff);
    printf("keep_cnt_sent=%d\n", pcb->keep_cnt_sent);

    printf("unsent segments:\n");
    seg = pcb->unsent;
    DBG_LWIP_SEG_SHOW(seg) 

    printf("unacked segments:\n");
    seg = pcb->unacked;
    DBG_LWIP_SEG_SHOW(seg);

    printf("ooseg semengts:\n");
    seg = pcb->ooseq;
    DBG_LWIP_SEG_SHOW(seg);

    printf("refused data=%p\n", pcb->refused_data);
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
    printf("-------------active pcbs------------\n");
    dbg_lwip_tcp_pcb_list_show(tcp_active_pcbs);
    printf("-------------bound pcbs-------------\n");
    dbg_lwip_tcp_pcb_list_show(tcp_bound_pcbs);
    printf("-------------tw     pcbs------------\n");
    dbg_lwip_tcp_pcb_list_show(tcp_tw_pcbs);
}

void dbg_lwip_udp_pcb_one_show(struct udp_pcb *pcb)
{
    printf("pcb=%p next=%p\n", pcb, (void*)pcb->next);
    DBG_LWIP_IP_PCB_SHOW(pcb);
    printf("flags=%x\n", pcb->flags);
    printf("local_port=%d remote_port=%d\n", pcb->local_port, pcb->remote_port);
    printf("recv cb=%p recv_arg=%p\n", pcb->recv, pcb->recv_arg);
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
    printf("TBC\n");
}

void dbg_lwip_udp_rxtx_show(void)
{
    printf("TBC\n");
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
    ESP_STATS_DISPLAY();
}

#if (ESP_CNT_DEBUG == 1)

uint32_t g_lwip_mem_cnt[MEMP_MAX][2];
extern const struct memp_desc * const memp_pools[MEMP_MAX];

void dbg_lwip_cnt_show(void)
{
    int i=0;

    printf("-----lwip memory counter-----\n");
    printf("%6s %8s %8s\n", "index", "alloc", "free");
    for (i=0; i<MEMP_MAX; i++){
        printf("%6u %8u %8u\n", i, g_lwip_mem_cnt[i][0], g_lwip_mem_cnt[i][1]);
    }
}


#endif

