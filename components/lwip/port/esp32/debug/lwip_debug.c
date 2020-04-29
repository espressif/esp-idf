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
#include "freertos/task.h"
#include "string.h"
#include "lwip/tcpip.h"
#include "lwip/priv/sockets_priv.h"

#define TAG "lwipd"

extern struct lwip_sock* lwip_socket_debug_get_socket(int fd);
extern void lwip_socket_debug_done_socket(struct lwip_sock *sock);
extern struct tcp_pcb *tcp_bound_pcbs;
extern struct tcp_pcb *tcp_active_pcbs;
extern struct tcp_pcb *tcp_tw_pcbs;
extern struct udp_pcb *udp_pcbs;
extern sys_mbox_t mbox;
extern sys_thread_t g_lwip_task;

#if LWIP_STATS
//extern struct stats lwip_stats;
#endif

static sys_sem_t s_lwip_debug_api_sem = NULL;
esp_lwip_stats_t g_esp_lwip_stats = {0};

typedef struct {
    struct lwip_sock sock;
    bool valid;
    uint32_t recvmbox_cnt;
    uint32_t acceptmbox_cnt;
} lwip_debug_sock_t;

typedef struct lwip_debug_tcp_pcb_s {
    struct tcp_pcb pcb;
    uint16_t unsent_cnt;
    uint16_t unack_cnt;
    uint16_t oos_cnt;
    uint16_t refused_cnt;
    struct lwip_debug_tcp_pcb_s *next;
} lwip_debug_tcp_pcb_t;

typedef struct lwip_debug_udp_pcb_s {
    struct udp_pcb pcb;
    struct lwip_debug_udp_pcb_s *next;
} lwip_debug_udp_pcb_t;

typedef struct {
    void *sockets;
    lwip_debug_tcp_pcb_t *active_pcbs;
    lwip_debug_tcp_pcb_t *bound_pcbs;
    lwip_debug_tcp_pcb_t *tw_pcbs;
    lwip_debug_udp_pcb_t *udp_pcbs;
} lwip_debug_info_t;

struct lwip_debug_api_msg_s;
typedef int (*lwip_debug_api_fn_t)(struct lwip_debug_api_msg_s *msg);
typedef struct lwip_debug_api_msg_s {
    lwip_debug_api_fn_t api_fn;
    int ret;
    void *arg;
} lwip_debug_api_msg_t;

err_t tcpip_send_msg_wait_sem(tcpip_callback_fn fn, void *apimsg, sys_sem_t* sem);

void dbg_lwip_task_show(void)
{
    if (mbox && mbox->os_mbox) {
        ESP_LOGI(TAG, "lwip mbox=%p msg=%u", mbox->os_mbox, uxQueueMessagesWaiting(mbox->os_mbox));
    }

    if (g_lwip_task) {
        ESP_LOGI(TAG, "lwip task=%p state=%d",  g_lwip_task, eTaskGetState(g_lwip_task));
    }

}

static uint32_t lwip_debug_pbuf_len(struct pbuf *buf)
{
    uint32_t cnt = 0;

    while (buf) {
        cnt ++;
        buf = buf->next;
    }

    return cnt;
}

static uint32_t lwip_debug_tcp_seg_len(struct tcp_seg *seg)
{
    uint32_t cnt = 0;

    while (seg) {
        cnt ++;
        seg = seg->next;
    }

    return cnt;
}

static int lwip_debug_deep_copy_tcp_pcb_list(struct tcp_pcb *pcb, lwip_debug_tcp_pcb_t **pdbgpcb)
{
    lwip_debug_tcp_pcb_t *dbgpcb = NULL;
    lwip_debug_tcp_pcb_t *pre = NULL;

    while(pcb){
        dbgpcb = (lwip_debug_tcp_pcb_t*)mem_malloc(sizeof(lwip_debug_tcp_pcb_t));

        if (!dbgpcb) {
            ESP_LOGI(TAG, "Failed to allocate TCP PCB");
            return -1;
        }

        memset(dbgpcb, 0, sizeof(lwip_debug_tcp_pcb_t));
        memcpy((void*)dbgpcb, (void*)pcb, sizeof(struct tcp_pcb));

        dbgpcb->unsent_cnt = lwip_debug_tcp_seg_len(pcb->unsent);
        dbgpcb->unack_cnt = lwip_debug_tcp_seg_len(pcb->unacked);
        dbgpcb->oos_cnt = lwip_debug_tcp_seg_len(pcb->ooseq);
        dbgpcb->refused_cnt = lwip_debug_pbuf_len(pcb->refused_data);

        if ( (*pdbgpcb) == NULL) {
            *pdbgpcb = dbgpcb;
            pre = dbgpcb;
        } else {
            pre->next = dbgpcb;
            pre = pre->next;
        }
        pcb = pcb->next;
    }

    return 0;
}

static int lwip_debug_get_tcp_pcbs(lwip_debug_info_t *info)
{
    lwip_debug_deep_copy_tcp_pcb_list(tcp_active_pcbs, &info->active_pcbs);
    lwip_debug_deep_copy_tcp_pcb_list(tcp_bound_pcbs, &info->bound_pcbs);
    lwip_debug_deep_copy_tcp_pcb_list(tcp_tw_pcbs, &info->tw_pcbs);
    return 0;
}

static int lwip_debug_deep_copy_udp_pcb_list(struct udp_pcb *pcb, lwip_debug_udp_pcb_t **pdbgpcb)
{
    lwip_debug_udp_pcb_t *dbgpcb = NULL;
    lwip_debug_udp_pcb_t *pre = NULL;

    while (pcb) {
        dbgpcb = (lwip_debug_udp_pcb_t*)mem_malloc(sizeof(lwip_debug_udp_pcb_t));
        
        if (!dbgpcb) {
            ESP_LOGI(TAG, "Failed to allocate UDP PCB");
            return -1;
        }
        
        memset(dbgpcb, 0, sizeof(lwip_debug_udp_pcb_t));
        memcpy((void*)dbgpcb, (void*)pcb, sizeof(struct udp_pcb));

        if ( (*pdbgpcb) == NULL) {
            *pdbgpcb = dbgpcb;
            pre = dbgpcb;
        } else {
            pre->next = dbgpcb;
            pre = pre->next;
        }

        pcb = pcb->next;
    }

    return 0;
}

static int lwip_debug_get_udp_pcbs(lwip_debug_info_t *info)
{
    lwip_debug_deep_copy_udp_pcb_list(udp_pcbs, &info->udp_pcbs);
    return 0;
}

static void lwip_socket_debug_free_sockets(lwip_debug_sock_t *dbgsock)
{   
    struct lwip_sock *sock = NULL;
    int i=0;
    
    for (i=0; i<NUM_SOCKETS; i++) {
        sock = (struct lwip_sock*)&dbgsock[i];
        if (sock->conn) {
            if (sock->conn->pcb.tcp) {
                free(sock->conn->pcb.tcp);
            }
            free(sock->conn);
        }
    }

    free(dbgsock);
}

int lwip_socket_debug_deep_copy_socket(struct lwip_sock *sock, lwip_debug_sock_t *dbgsock)
{
    struct lwip_sock *dbgsock_base = (struct lwip_sock*)dbgsock;
    struct tcp_pcb *tcp_pcb_copy = NULL;
    struct udp_pcb *udp_pcb_copy = NULL;
    struct netconn *conn_copy = NULL;
    struct netconn *conn = NULL;

    conn = sock->conn;
    if (!conn) {
        return 0;
    }

    conn_copy = (struct netconn*)mem_malloc(sizeof(struct netconn));
    if (!conn_copy) {
        return -1;
    }

    memcpy(conn_copy, conn, sizeof(struct netconn));
    dbgsock_base->conn = conn_copy;
    conn_copy->pcb.tcp = NULL;

    if (conn->recvmbox && conn->recvmbox->os_mbox) {
        dbgsock->recvmbox_cnt = uxQueueMessagesWaiting(conn->recvmbox->os_mbox);
    }

#if LWIP_TCP
    if (conn->acceptmbox && conn->acceptmbox->os_mbox) {
        dbgsock->acceptmbox_cnt = uxQueueMessagesWaiting(conn->acceptmbox->os_mbox);
    }
#endif

    if (conn->pcb.tcp) {
        switch (conn->type) {
        case NETCONN_TCP:
            tcp_pcb_copy = (struct tcp_pcb*) mem_malloc(sizeof(struct tcp_pcb));
            if (!tcp_pcb_copy) {
                goto _exit;
            }

            memcpy(tcp_pcb_copy, conn->pcb.tcp, sizeof(struct tcp_pcb));
            conn_copy->pcb.tcp = tcp_pcb_copy;
            break;

        case NETCONN_UDP:
            udp_pcb_copy = (struct udp_pcb*) mem_malloc(sizeof(struct udp_pcb));
            if (!udp_pcb_copy) {
                goto _exit;
            }

            memcpy(udp_pcb_copy, sock->conn->pcb.udp, sizeof(struct udp_pcb));
            conn_copy->pcb.udp = udp_pcb_copy;
            break;

        default:
            break;
        }
    }

    return 0;

_exit:
    if (tcp_pcb_copy) {
        free(tcp_pcb_copy);
    }

    if (udp_pcb_copy) {
        free(udp_pcb_copy);
    }

    if (conn_copy) {
        free(conn_copy);
    }

    dbgsock_base->conn = NULL;
    return -1;
}

static int lwip_socket_debug_get_all_sockets(void **arg)
{
    lwip_debug_sock_t **pdbgsocks = (lwip_debug_sock_t**)arg;
    lwip_debug_sock_t *dbgsock = NULL;
    struct lwip_sock *sock;
    int i;

    if (!pdbgsocks) {
        return -1;
    }

    *pdbgsocks = NULL;
    dbgsock = (lwip_debug_sock_t*) mem_malloc(sizeof(lwip_debug_sock_t) * NUM_SOCKETS);
    if (!dbgsock) {
        return -1;
    }

    memset(dbgsock, 0, sizeof(lwip_debug_sock_t) * NUM_SOCKETS);
    *pdbgsocks = dbgsock;

    for (i=0; i<NUM_SOCKETS; i++) {
        sock = lwip_socket_debug_get_socket(i+LWIP_SOCKET_OFFSET);

        if (sock) {
            memcpy (&dbgsock[i], sock, sizeof(struct lwip_sock));
            if (0 != lwip_socket_debug_deep_copy_socket(sock, &dbgsock[i])) {
                lwip_socket_debug_done_socket(sock);
                goto _exit;
            }
            dbgsock[i].valid = true;
            lwip_socket_debug_done_socket(sock);
        } else {
            dbgsock[i].valid = false;
        }
    }

    return 0;

_exit:
    lwip_socket_debug_free_sockets(dbgsock);
    *pdbgsocks = NULL;
    return -1;
}

void lwip_debug_dump_open_sockets(void *arg)
{
    lwip_debug_sock_t *sockets = (lwip_debug_sock_t*)arg;
    struct lwip_sock *sock = NULL;
    struct netconn *conn = NULL;
    int recv_bufsize = 0;
    int recv_avail = 0;
    s32_t send_timeout = 0;
    u32_t recv_timeout = 0;
    int socket_num = 0;
    s16_t linger = 0;
    ip_addr_t empty_ip;
    ip_addr_t *local_ip;
    ip_addr_t *remote_ip;
    uint16_t local_port;
    uint16_t remote_port;
    int i=0;

    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "========> socket list");

    if (!sockets) {
        return;
    }

    for (i=0; i<NUM_SOCKETS; i++) {
        if (true == sockets[i].valid) {
            socket_num ++;
            break;
        }
    }

    if (socket_num == 0) {
        return;
    }

    ESP_LOGI(TAG, "%2s %2s %13s %13s %3s %3s %3s %2s %2s %2s %4s %3s %4s %2s %4s %4s %4s %3s %3s %5s %3s %3s",
                  "id", "46", "src ip:port", "dst ip:port", "re", "se", "ee", "sw", "st", "rf",
                  "ctye", "cst", "cerr", "cw", "ctto", "crto", "crbs", "cra", "cli", "cflag",
                  "rmb", "amb");

    memset(&empty_ip, 0, sizeof(empty_ip));

    for (i=0; i<NUM_SOCKETS; i++) {
        sock = (struct lwip_sock*) &sockets[i];

        if (false == sockets[i].valid) {
            continue;
        }

        conn = sock->conn;
        recv_bufsize = recv_avail = send_timeout = recv_timeout = linger = 0;
        if (conn) {
#if LWIP_SO_SNDTIMEO
        send_timeout = conn->send_timeout;
#endif

#if LWIP_SO_RCVTIMEO
        recv_timeout = conn->recv_timeout;
#endif

#if LWIP_SO_LINGER
        linger = conn->linger;
#endif

#if LWIP_SO_RCVBUF
        recv_bufsize = conn->recv_bufsize;
        recv_avail = conn->recv_avail;
#endif

            if ( (conn->type == NETCONN_TCP) && conn->pcb.tcp) {
                local_ip = &conn->pcb.tcp->local_ip;
                remote_ip = &conn->pcb.tcp->remote_ip;
                local_port = conn->pcb.tcp->local_port;
                remote_port = conn->pcb.tcp->remote_port;
            } else if ( (conn->type == NETCONN_UDP) && conn->pcb.udp) {
                local_ip = &conn->pcb.udp->local_ip;
                remote_ip = &conn->pcb.udp->remote_ip;
                local_port = conn->pcb.udp->local_port;
                remote_port = conn->pcb.udp->remote_port;
            } else {
                local_ip = remote_ip = &empty_ip;
                local_port = remote_port = 0;
            }

            ESP_LOGI(TAG, "%2x %2x %8x:%4x %8x:%4x %3x %3x %3x %2x %2x %2x %4x %3x %4x %2x %4x %4x %4x %3x %3x %5x %3x %3x",
                           i+LWIP_SOCKET_OFFSET, local_ip->type, local_ip->u_addr.ip4.addr, local_port, remote_ip->u_addr.ip4.addr, remote_port, 
                           sock->rcvevent, sock->sendevent, sock->errevent, sock->select_waiting, sock->state, sock->ref,
                           conn->type, conn->state, conn->last_err, 0, send_timeout, recv_timeout, recv_bufsize,
                           recv_avail, linger, conn->flags, sockets[i].recvmbox_cnt, sockets[i].acceptmbox_cnt);
        } else {
            ESP_LOGI(TAG, "%2x %21s %21s %3x %3x %3x %2x %2x %2x %4s %3s %4s %2s %4s %4s %4s %3s %3s %5s %3s %3s",
                           i+LWIP_SOCKET_OFFSET, "-:-", "-:-", sock->rcvevent, sock->sendevent, sock->errevent, sock->select_waiting, sock->state, sock->ref,
                           "-", "-", "-", "-", "-", "-", "-",
                           "-", "-", "-", "-", "-");
        }
    }
}

static void lwip_debug_dump_tcp_pcb_list_part1(lwip_debug_tcp_pcb_t* dbgpcb, uint32_t type, uint32_t *id)
{
    char t[] = {'A', 'B', 'T'};
    struct tcp_pcb *pcb;
    uint8_t i = *id;

    while(dbgpcb){
        pcb = (struct tcp_pcb *)&dbgpcb->pcb;
        ESP_LOGI(TAG, "%2d %c %2x %8x:%4x %8x:%4x %2x %2x %5x %5x %6x %5x %5x %4x %4s %4s %4s %4s %4s", 
            i++, t[type], pcb->local_ip.type, pcb->local_ip.u_addr.ip4.addr, pcb->local_port, pcb->remote_ip.u_addr.ip4.addr, pcb->remote_port, pcb->state, pcb->prio,
            pcb->flags, pcb->polltmr, pcb->pollinterval, pcb->last_timer, pcb->tmr, pcb->mss, pcb->sent?"x":"0", pcb->recv?"x":"0", pcb->connected?"x":"0",
            pcb->poll?"x":"0", pcb->errf?"x":"0");
        dbgpcb = dbgpcb->next;
    }
    *id = *id + i;
}

static void lwip_debug_dump_tcp_pcb_list_part2(lwip_debug_tcp_pcb_t* dbgpcb, uint32_t type, uint32_t *id)
{
    struct tcp_pcb *pcb;
    uint8_t i = *id;

    while(dbgpcb){
        pcb = (struct tcp_pcb *)&dbgpcb->pcb;
        ESP_LOGI(TAG, "%2x %8x %8x %8x %8x %8x %3x %6x",
            i++, pcb->rcv_nxt, pcb->rcv_wnd, pcb->rcv_ann_wnd, pcb->rcv_ann_right_edge, 0, dbgpcb->oos_cnt, dbgpcb->refused_cnt);
        dbgpcb = dbgpcb->next;
    }
    *id = *id + i;
}

static void lwip_debug_dump_tcp_pcb_list_part3(lwip_debug_tcp_pcb_t* dbgpcb, uint32_t type, uint32_t *id)
{
    struct tcp_pcb *pcb;
    uint8_t i = *id;

    while(dbgpcb){
        pcb = (struct tcp_pcb *)&dbgpcb->pcb;

        ESP_LOGI(TAG, "%2x %6x %8x %4x %4x %4x %4x %4x %8x %8x %8x %8x %4x %4x %4x",
            i++, pcb->rttest, pcb->rtseq, pcb->sa, pcb->sv, pcb->rto, pcb->nrtx, pcb->dupacks, pcb->lastack, pcb->cwnd,
            pcb->ssthresh, 0, pcb->persist_cnt, pcb->persist_backoff, 0);

        dbgpcb = dbgpcb->next;
    }
    *id = *id + i;
}

static void lwip_debug_dump_tcp_pcb_list_part4(lwip_debug_tcp_pcb_t* dbgpcb, uint32_t type, uint32_t *id)
{
    uint32_t unsent_oversize = 0;
    struct tcp_pcb *pcb;
    uint8_t i = *id;

    while(dbgpcb){
        pcb = (struct tcp_pcb *)&dbgpcb->pcb;

#if TCP_OVERSIZE
        unsent_oversize = pcb->unsent_oversize;
#endif

        ESP_LOGI(TAG, "%2x %8x %8x %8x %8x %8x %8x %5x %5x %6x %3x %3x", 
            i++, pcb->snd_nxt, pcb->snd_wl1, pcb->snd_wl2, pcb->snd_lbb, pcb->snd_wnd, pcb->snd_wnd_max, pcb->snd_buf,
            pcb->snd_queuelen, unsent_oversize, dbgpcb->unsent_cnt, dbgpcb->unack_cnt);
        dbgpcb = dbgpcb->next;
    }
    *id = *id + i;
}



static void lwip_debug_dump_tcp_pcbs_part1(lwip_debug_info_t *info)
{
    uint32_t id = 0;

    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "%2s %1s %2s %13s %13s %2s %2s %5s %5s %6s %5s %5s %4s %4s %4s %4s %4s %4s", 
                  "id", "t", "46", "src ip:port", "dst ip:port", "st", "pr", "flags", "p_tmr", "p_itvl",
                  "l_tmr", "tmr", "mss", "sent", "recv", "conn", "poll", "errf");
    lwip_debug_dump_tcp_pcb_list_part1(info->active_pcbs, 0, &id);
    lwip_debug_dump_tcp_pcb_list_part1(info->bound_pcbs, 1, &id);
    lwip_debug_dump_tcp_pcb_list_part1(info->tw_pcbs, 2, &id);
}

static void lwip_debug_dump_tcp_pcbs_part2(lwip_debug_info_t *info)
{
    uint32_t id = 0;

    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "%2s %8s %8s %8s %8s %8s %3s %6s", "id", "r_next", "r_wnd", "r_a_wnd", "r_a_r_e", "b_acked", "oos", "refuse");
    lwip_debug_dump_tcp_pcb_list_part2(info->active_pcbs, 0, &id);
    lwip_debug_dump_tcp_pcb_list_part2(info->bound_pcbs, 1, &id);
    lwip_debug_dump_tcp_pcb_list_part2(info->tw_pcbs, 2, &id);
}

static void lwip_debug_dump_tcp_pcbs_part3(lwip_debug_info_t *info)
{
    uint32_t id = 0;

    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "%2s %6s %8s %4s %4s %4s %4s %4s %8s %8s %8s %8s %4s %4s %4s", 
                  "id", "rttest", "rtseq", "sa", "sv", "rto", "nrtx", "dup", "last", "cwnd", "ssthresh", "rto_end", "pist", "p_bo", "p_p");

    lwip_debug_dump_tcp_pcb_list_part3(info->active_pcbs, 0, &id);
    lwip_debug_dump_tcp_pcb_list_part3(info->bound_pcbs, 1, &id);
    lwip_debug_dump_tcp_pcb_list_part3(info->tw_pcbs, 2, &id);
}

static void lwip_debug_dump_tcp_pcbs_part4(lwip_debug_info_t *info)
{
    uint32_t id = 0;

    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "%2s %8s %8s %8s %8s %8s %8s %5s %5s %6s %3s %3s", 
                  "id", "snd_next", "snd_wl1", "snd_wl2", "wnd_lbb", "s_wnd", "s_wm", "s_buf", "s_qlen", "uns_os", "uns", "una");

    lwip_debug_dump_tcp_pcb_list_part4(info->active_pcbs, 0, &id);
    lwip_debug_dump_tcp_pcb_list_part4(info->bound_pcbs, 1, &id);
    lwip_debug_dump_tcp_pcb_list_part4(info->tw_pcbs, 2, &id);
}

static void lwip_debug_dump_tcp_pcbs(lwip_debug_info_t *info)
{
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "========> tcp pcb list");

    if ( !(info->active_pcbs) && !(info->bound_pcbs) && !(info->tw_pcbs) ) {
        return;
    }

    lwip_debug_dump_tcp_pcbs_part1(info);
    lwip_debug_dump_tcp_pcbs_part3(info);
    lwip_debug_dump_tcp_pcbs_part4(info);
    lwip_debug_dump_tcp_pcbs_part2(info);
}

static void lwip_debug_dump_udp_pcbs(lwip_debug_info_t *info)
{
    lwip_debug_udp_pcb_t *dbgpcb = info->udp_pcbs;
    struct udp_pcb *pcb;
    uint32_t i = 0;
    uint32_t mcast_ip = 0;
    uint8_t mcast_ttl = 0;

    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "========> udp pcb list");

    if ( !dbgpcb ) {
        return;
    }

    ESP_LOGI(TAG, "%2s %2s %13s %13s %5s %4s %4s %5s %4s %3s",
                  "id", "46", "src ip:port", "dst ip:port", "flags", "m_ip", "m_if", "m_ttl", "recv", "arg");
    while (dbgpcb) {
        pcb = (struct udp_pcb *)dbgpcb;

#if LWIP_MULTICAST_TX_OPTIONS
        mcast_ip = pcb->multicast_ip.u_addr.ip4.addr;
        mcast_ttl = pcb->mcast_ttl;
#endif

        ESP_LOGI(TAG, "%2x %2x %8x:%4x %8x:%4x %5x %4x %4x %5x %4s %3s",
            i++, pcb->local_ip.type, pcb->local_ip.u_addr.ip4.addr, pcb->local_port, pcb->remote_ip.u_addr.ip4.addr, pcb->remote_port,
            pcb->flags, mcast_ip, mcast_ttl, pcb->mcast_ttl, pcb->recv?"x":"0", pcb->recv_arg?"x":"0");
        dbgpcb = dbgpcb->next;
    }
}

static void lwip_debug_dump_sockets(lwip_debug_info_t *info)
{
    lwip_debug_dump_open_sockets(info->sockets);
    lwip_debug_dump_tcp_pcbs(info);
    lwip_debug_dump_udp_pcbs(info);
}

static void lwip_debug_api_cb(void *api_msg)
{
    lwip_debug_api_msg_t *msg = (lwip_debug_api_msg_t *)api_msg;

    if (!msg || !msg->api_fn) {
        return;
    }

    msg->ret = msg->api_fn(msg);
    sys_sem_signal(&s_lwip_debug_api_sem);
    return;
}

static int lwip_debug_api_call(lwip_debug_api_msg_t *msg)
{
    int ret;

    if (!msg || (!msg->api_fn)) {
        ESP_LOGI(TAG, "null msg/fn");
        return ESP_ERR_INVALID_ARG;
    }

    msg->ret = ESP_FAIL;

    if (!s_lwip_debug_api_sem) {
        ret = sys_sem_new(&s_lwip_debug_api_sem, 1);
        if (ESP_OK != ret) {
            ESP_LOGI(TAG, "failed to create lwip sync sem");
            return ESP_ERR_NO_MEM;
        }
    }
    
    sys_arch_sem_wait(&s_lwip_debug_api_sem, 0);
    tcpip_send_msg_wait_sem((tcpip_callback_fn)lwip_debug_api_cb, msg, &s_lwip_debug_api_sem);
    sys_sem_signal(&s_lwip_debug_api_sem);

    return msg->ret;
}

int lwip_debug_get_sockets_local(lwip_debug_api_msg_t *msg)
{
    lwip_debug_info_t *info = (lwip_debug_info_t*) msg->arg;

    if (0 != lwip_socket_debug_get_all_sockets(&info->sockets)) {
        return -1;
    }

    if (0 != lwip_debug_get_tcp_pcbs(info)) {
        return -1;
    }

    if (0 != lwip_debug_get_udp_pcbs(info)) {
        return -1;
    }

    return 0;
}

static int lwip_debug_get_sockets(lwip_debug_info_t *info)
{
    lwip_debug_api_msg_t msg;

    msg.api_fn = lwip_debug_get_sockets_local; 
    msg.arg = (void*)info;
    msg.ret = ESP_FAIL;

    lwip_debug_api_call(&msg);
    return msg.ret;
}

static void lwip_debug_free_tcp_pcb_list(lwip_debug_tcp_pcb_t *pcb)
{
    lwip_debug_tcp_pcb_t *tmp;

    while (pcb) {
        tmp = pcb;
        pcb = pcb->next;
        free(tmp);
    }
}

static void lwip_debug_free_tcp_pcbs(lwip_debug_info_t *info)
{
    if (!info) {
        return;
    }

    if (info->active_pcbs) {
        lwip_debug_free_tcp_pcb_list(info->active_pcbs);
        info->active_pcbs = 0;
    }


    if (info->bound_pcbs) {
        lwip_debug_free_tcp_pcb_list(info->bound_pcbs);
        info->bound_pcbs = 0;
    }

    if (info->tw_pcbs) {
        lwip_debug_free_tcp_pcb_list(info->tw_pcbs);
        info->tw_pcbs = 0;
    }
}

static void lwip_debug_free_udp_pcb_list(lwip_debug_udp_pcb_t *pcb)
{
    lwip_debug_udp_pcb_t *tmp;

    while (pcb) {
        tmp = pcb;
        pcb = pcb->next;
        free(tmp);
    }
}

static void lwip_debug_free_udp_pcbs(lwip_debug_info_t *info)
{
    if (!info) {
        return;
    }

    if (info->udp_pcbs) {
        lwip_debug_free_udp_pcb_list(info->udp_pcbs);
    }
}

static void lwip_debug_free_sockets(lwip_debug_info_t *info)
{
    if (!info) {
        return;
    }

    if (info->sockets) {
        lwip_socket_debug_free_sockets(info->sockets);
        info->sockets = NULL;
    }

    lwip_debug_free_tcp_pcbs(info);
    lwip_debug_free_udp_pcbs(info);
}

#if LWIP_STATS
static void lwip_debug_dump_igmp_statistics(struct stats_igmp *proto, const char *name)
{
    ESP_LOGI(TAG, "%8s %8x %8x %4x %4x %4x %4x %4x %5x %5x %5x %5x %5x %5x %5x",
        name, proto->xmit, proto->recv, proto->drop, proto->chkerr, proto->lenerr,
        proto->memerr, proto->proterr, proto->rx_v1, proto->rx_group, proto->rx_general,
        proto->rx_report, proto->tx_join, proto->tx_leave, proto->tx_report);
}


static void lwip_debug_dump_multicast_statistics(void)
{
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "========> LWIP multicast statistics");
    ESP_LOGI(TAG, "%8s %8s %8s %4s %4s %4s %4s %4s %5s %5s %5s %5s %5s %5s %5s",
        "name", "xmit", "recv", "drop", "chk", "len", "mem", "prot", "rxv1", "rx_gr", "rx_gl", "rx_rp", "tx_jn", "tx_lv", "tx_rp");

#if IGMP_STATS
    lwip_debug_dump_igmp_statistics(&lwip_stats.igmp, "IGMP");
#endif

#if MLD6_STATS
    lwip_debug_dump_igmp_statistics(&lwip_stats.mld6, "MLDv1");
#endif
}

static void lwip_debug_dump_protocol_statistics(struct stats_proto *proto, const char *name)
{
    ESP_LOGI(TAG, "%8s %8x %8x %8x %4x %4x %4x %4x %4x %4x %4x %4x %8x",
        name, proto->xmit, proto->recv, proto->fw, proto->drop, proto->chkerr, proto->lenerr,
        proto->memerr, proto->rterr, proto->proterr, proto->opterr, proto->err, proto->cachehit);
}

static void lwip_debug_dump_unicast_statistics(void)
{
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "========> LWIP counters");
    ESP_LOGI(TAG, "%8s %8s %8s %8s %4s %4s %4s %4s %4s %4s %4s %4s %8s",
        "name", "xmit", "recv", "forward", "drop", "chk", "len", "mem", "rt", "prot", "opt", "err", "hit");
#if TCP_STATS
    lwip_debug_dump_protocol_statistics(&lwip_stats.tcp, "TCP");
#endif
#if UDP_STATS
    lwip_debug_dump_protocol_statistics(&lwip_stats.udp, "UDP");
#endif
#if ICMP_STATS
    lwip_debug_dump_protocol_statistics(&lwip_stats.icmp, "ICMP");
#endif
#if IP_STATS
    lwip_debug_dump_protocol_statistics(&lwip_stats.ip, "IP");
#endif
#if IPFRAG_STATS
    lwip_debug_dump_protocol_statistics(&lwip_stats.ip_frag, "IPFRAG");
#endif
#if ETHARP_STATS
    lwip_debug_dump_protocol_statistics(&lwip_stats.etharp, "ETHARP");
#endif
#if LINK_STATS
    lwip_debug_dump_protocol_statistics(&lwip_stats.link, "LINK");
#endif
#if IP6_STATS
    lwip_debug_dump_protocol_statistics(&lwip_stats.ip6, "IPV6");
#endif
#if ICMP6_STATS
    lwip_debug_dump_protocol_statistics(&lwip_stats.icmp6, "ICMP6");
#endif
#if IP6_FRAG_STATS
    lwip_debug_dump_protocol_statistics(&lwip_stats.ip6_frag, "IPV6FRAG");
#endif
#if ND6_FRAG_STATS
    lwip_debug_dump_protocol_statistics(&lwip_stats.nd6, "ND");
#endif

}
#endif

void esp_lwip_debug_dump_sockets(void)
{
    lwip_debug_info_t info;

    memset(&info, 0, sizeof(info));

    if (ESP_OK == lwip_debug_get_sockets(&info)) {
        lwip_debug_dump_sockets(&info);
        lwip_debug_free_sockets(&info);
    }
}

static void lwip_debug_dump_sys_statistics(void)
{
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "========> LWIP system counters");
    ESP_LOGI(TAG, "%8s %8s %8s", "tpost_f", "fetch_f", "tfetch_f");
    ESP_LOGI(TAG, "%8x %8x %8x", g_esp_lwip_stats.trypost_fail, g_esp_lwip_stats.fetch_fail, g_esp_lwip_stats.tryfetch_fail);
}

void esp_lwip_debug_dump_stats(void)
{
#if LWIP_STATS
    lwip_debug_dump_sys_statistics();
    lwip_debug_dump_unicast_statistics();
    lwip_debug_dump_multicast_statistics();
#endif

    //TODO
    // 1. mbox post fail/recv timeout
    // 2. tcpip mbox counter
    // 3. tcp abort
    // 4. ARP drop
    // 5. DHCP drop
    // 6. DNS fail
    // 7. DHCP renew time, renew fail time, rebind timer
    // 8. 
}

void esp_lwip_dump(uint64_t modules)
{
    if (modules & LWIP_MODULE_SOCKETS) {
        esp_lwip_debug_dump_sockets();
    }
        
    if (modules & LWIP_MODULE_STATS) {
        esp_lwip_debug_dump_stats();
    }
}

