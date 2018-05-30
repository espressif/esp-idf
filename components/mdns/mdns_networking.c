
/*
 * MDNS Server Networking
 * 
 */
#include <string.h>
#include "mdns_networking.h"


extern mdns_server_t * _mdns_server;

/*
 * MDNS Server Networking
 *
 */

/**
 * @brief  the receive callback of the raw udp api. Packets are received here
 *
 */
static void _udp_recv(void *arg, struct udp_pcb *upcb, struct pbuf *pb, const ip_addr_t *raddr, uint16_t rport)
{

    uint8_t i;
    while (pb != NULL) {
        struct pbuf * this_pb = pb;
        pb = pb->next;
        this_pb->next = NULL;

        mdns_rx_packet_t * packet = (mdns_rx_packet_t *)malloc(sizeof(mdns_rx_packet_t));
        if (!packet) {
            //missed packet - no memory
            pbuf_free(this_pb);
            continue;
        }

        packet->tcpip_if = TCPIP_ADAPTER_IF_MAX;
        packet->pb = this_pb;
        packet->src_port = rport;
        memcpy(&packet->src, raddr, sizeof(ip_addr_t));
        packet->dest.type = packet->src.type;

        if (packet->src.type == IPADDR_TYPE_V4) {
            packet->ip_protocol = MDNS_IP_PROTOCOL_V4;
            struct ip_hdr * iphdr = (struct ip_hdr *)(((uint8_t *)(packet->pb->payload)) - UDP_HLEN - IP_HLEN);
            packet->dest.u_addr.ip4.addr = iphdr->dest.addr;
        } else {
            packet->ip_protocol = MDNS_IP_PROTOCOL_V6;
            struct ip6_hdr * ip6hdr = (struct ip6_hdr *)(((uint8_t *)(packet->pb->payload)) - UDP_HLEN - IP6_HLEN);
            memcpy(&packet->dest.u_addr.ip6.addr, (uint8_t *)ip6hdr->dest.addr, 16);
        }
        packet->multicast = ip_addr_ismulticast(&(packet->dest));

        //lwip does not return the proper pcb if you have more than one for the same multicast address (but different interfaces)
        struct netif * netif = NULL;
        void * nif = NULL;
        struct udp_pcb * pcb = NULL;
        for (i=0; i<TCPIP_ADAPTER_IF_MAX; i++) {
            pcb = _mdns_server->interfaces[i].pcbs[packet->ip_protocol].pcb;
            tcpip_adapter_get_netif (i, &nif);
            netif = (struct netif *)nif;
            if (pcb && netif && netif == ip_current_input_netif ()) {
                if (packet->src.type == IPADDR_TYPE_V4) {
                    if ((packet->src.u_addr.ip4.addr & netif->netmask.u_addr.ip4.addr) != (netif->ip_addr.u_addr.ip4.addr & netif->netmask.u_addr.ip4.addr)) {
                        //packet source is not in the same subnet
                        pcb = NULL;
                        break;
                    }
                }
                packet->tcpip_if = i;
                break;
            }
            pcb = NULL;
        }

        if (!pcb || !_mdns_server || !_mdns_server->action_queue
          || _mdns_send_rx_action(packet) != ESP_OK) {
            pbuf_free(this_pb);
            free(packet);
        }
    }

}

/**
 * @brief  Stop PCB Main code
 */
static void _udp_pcb_deinit(tcpip_adapter_if_t tcpip_if, mdns_ip_protocol_t ip_protocol)
{
    if (!_mdns_server) {
        return;
    }
    mdns_pcb_t * _pcb = &_mdns_server->interfaces[tcpip_if].pcbs[ip_protocol];
    if (_pcb->pcb) {
        _pcb->state = PCB_OFF;
        udp_recv(_pcb->pcb, NULL, NULL);
        udp_disconnect(_pcb->pcb);
        udp_remove(_pcb->pcb);
        free(_pcb->probe_services);
        _pcb->pcb = NULL;
        _pcb->probe_ip = false;
        _pcb->probe_services = NULL;
        _pcb->probe_services_len = 0;
        _pcb->probe_running = false;
        _pcb->failed_probes = 0;
    }
}

/**
 * @brief  Start PCB V4
 */
static esp_err_t _udp_pcb_v4_init(tcpip_adapter_if_t tcpip_if)
{
    tcpip_adapter_ip_info_t if_ip_info;

    if (!_mdns_server || _mdns_server->interfaces[tcpip_if].pcbs[MDNS_IP_PROTOCOL_V4].pcb) {
        return ESP_ERR_INVALID_STATE;
    }

    if (tcpip_adapter_get_ip_info(tcpip_if, &if_ip_info) || if_ip_info.ip.addr == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    ip_addr_t interface_addr = IPADDR4_INIT(if_ip_info.ip.addr);

    ip_addr_t multicast_addr;
    IP_ADDR4(&multicast_addr, 224, 0, 0, 251);

    if (igmp_joingroup((const struct ip4_addr *)&interface_addr.u_addr.ip4, (const struct ip4_addr *)&multicast_addr.u_addr.ip4)) {
        return ESP_ERR_INVALID_STATE;
    }

    struct udp_pcb * pcb = udp_new_ip_type(IPADDR_TYPE_V4);
    if (!pcb) {
        return ESP_ERR_NO_MEM;
    }

    if (udp_bind(pcb, &interface_addr, MDNS_SERVICE_PORT) != 0) {
        udp_remove(pcb);
        return ESP_ERR_INVALID_STATE;
    }

    pcb->mcast_ttl = 1;
    pcb->remote_port = MDNS_SERVICE_PORT;
    ip_addr_copy(pcb->multicast_ip, interface_addr);
    ip_addr_copy(pcb->remote_ip, multicast_addr);

    _mdns_server->interfaces[tcpip_if].pcbs[MDNS_IP_PROTOCOL_V4].pcb = pcb;
    _mdns_server->interfaces[tcpip_if].pcbs[MDNS_IP_PROTOCOL_V4].failed_probes = 0;
    udp_recv(pcb, &_udp_recv, _mdns_server);

    return ESP_OK;
}

/**
 * @brief  Start PCB V6
 */
static esp_err_t _udp_pcb_v6_init(tcpip_adapter_if_t tcpip_if)
{
    ip_addr_t multicast_addr = IPADDR6_INIT(0x000002ff, 0, 0, 0xfb000000);
    ip_addr_t interface_addr;
    interface_addr.type = IPADDR_TYPE_V6;

    if (!_mdns_server || _mdns_server->interfaces[tcpip_if].pcbs[MDNS_IP_PROTOCOL_V6].pcb) {
        return ESP_ERR_INVALID_STATE;
    }

    if (tcpip_adapter_get_ip6_linklocal(tcpip_if, &interface_addr.u_addr.ip6)) {
        return ESP_ERR_INVALID_ARG;
    }

    if (mld6_joingroup(&(interface_addr.u_addr.ip6), &(multicast_addr.u_addr.ip6))) {
        return ESP_ERR_INVALID_STATE;
    }

    struct udp_pcb * pcb = udp_new_ip_type(IPADDR_TYPE_V6);
    if (!pcb) {
        return ESP_ERR_NO_MEM;
    }

    if (udp_bind(pcb, &interface_addr, MDNS_SERVICE_PORT) != 0) {
        udp_remove(pcb);
        return ESP_ERR_INVALID_STATE;
    }

    pcb->remote_port = MDNS_SERVICE_PORT;
    ip_addr_copy(pcb->remote_ip, multicast_addr);

    _mdns_server->interfaces[tcpip_if].pcbs[MDNS_IP_PROTOCOL_V6].pcb = pcb;
    _mdns_server->interfaces[tcpip_if].pcbs[MDNS_IP_PROTOCOL_V6].failed_probes = 0;
    udp_recv(pcb, &_udp_recv, _mdns_server);

    return ESP_OK;
}

/**
 * @brief  Start PCB Main code
 */
static esp_err_t _udp_pcb_init(tcpip_adapter_if_t tcpip_if, mdns_ip_protocol_t ip_protocol)
{
    if (ip_protocol == MDNS_IP_PROTOCOL_V4) {
        return _udp_pcb_v4_init(tcpip_if);
    } else if (ip_protocol == MDNS_IP_PROTOCOL_V6) {
        return _udp_pcb_v6_init(tcpip_if);
    }
    return ESP_ERR_INVALID_ARG;
}

typedef struct {
    struct tcpip_api_call call;
    tcpip_adapter_if_t tcpip_if;
    mdns_ip_protocol_t ip_protocol;
    esp_err_t err;
} mdns_api_call_t;

/**
 * @brief  Start PCB from LwIP thread
 */
static err_t _mdns_pcb_init_api(struct tcpip_api_call *api_call_msg)
{
    mdns_api_call_t * msg = (mdns_api_call_t *)api_call_msg;
    msg->err = _udp_pcb_init(msg->tcpip_if, msg->ip_protocol);
    return msg->err;
}

/**
 * @brief  Stop PCB from LwIP thread
 */
static err_t _mdns_pcb_deinit_api(struct tcpip_api_call *api_call_msg)
{
    mdns_api_call_t * msg = (mdns_api_call_t *)api_call_msg;
    _udp_pcb_deinit(msg->tcpip_if, msg->ip_protocol);
    msg->err = ESP_OK;
    return ESP_OK;
}

/*
 * Non-static functions below are
 *  - _mdns prefixed
 *  - commented in mdns_networking.h header
 */
esp_err_t _mdns_pcb_init(tcpip_adapter_if_t tcpip_if, mdns_ip_protocol_t ip_protocol)
{
    mdns_api_call_t msg = {
        .tcpip_if = tcpip_if,
        .ip_protocol = ip_protocol
    };
    tcpip_api_call(_mdns_pcb_init_api, (struct tcpip_api_call*)&msg);
    return msg.err;
}

esp_err_t _mdns_pcb_deinit(tcpip_adapter_if_t tcpip_if, mdns_ip_protocol_t ip_protocol)
{
    mdns_api_call_t msg = {
        .tcpip_if = tcpip_if,
        .ip_protocol = ip_protocol
    };
    tcpip_api_call(_mdns_pcb_deinit_api, (struct tcpip_api_call*)&msg);
    return msg.err;
}

size_t _mdns_udp_pcb_write(tcpip_adapter_if_t tcpip_if, mdns_ip_protocol_t ip_protocol, const ip_addr_t *ip, uint16_t port, uint8_t * data, size_t len)
{
    struct netif * netif = NULL;
    void * nif = NULL;
    esp_err_t err = tcpip_adapter_get_netif(tcpip_if, &nif);
    netif = (struct netif *)nif;
    if (err) {
        return 0;
    }

    struct pbuf* pbt = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);
    if (pbt == NULL) {
        return 0;
    }
    memcpy((uint8_t *)pbt->payload, data, len);

    err = udp_sendto_if (_mdns_server->interfaces[tcpip_if].pcbs[ip_protocol].pcb, pbt, ip, port, netif);
    pbuf_free(pbt);
    if (err) {
        return 0;
    }
    return len;
}
