
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

static struct udp_pcb * _pcb_main = NULL;

static void _udp_recv(void *arg, struct udp_pcb *upcb, struct pbuf *pb, const ip_addr_t *raddr, uint16_t rport);

/**
 * @brief  Low level UDP PCB Initialize
 */
static esp_err_t _udp_pcb_main_init()
{
    if(_pcb_main) {
        return ESP_OK;
    }
    _pcb_main = udp_new();
    if (!_pcb_main) {
        return ESP_ERR_NO_MEM;
    }
    if (udp_bind(_pcb_main, IP_ANY_TYPE, MDNS_SERVICE_PORT) != 0) {
        udp_remove(_pcb_main);
        _pcb_main = NULL;
        return ESP_ERR_INVALID_STATE;
    }
    _pcb_main->mcast_ttl = 1;
    _pcb_main->remote_port = MDNS_SERVICE_PORT;
    ip_addr_copy(_pcb_main->remote_ip, ip_addr_any_type);
    udp_recv(_pcb_main, &_udp_recv, _mdns_server);
    return ESP_OK;
}

/**
 * @brief  Low level UDP PCB Free
 */
static void _udp_pcb_main_deinit()
{
    if(_pcb_main){
        udp_recv(_pcb_main, NULL, NULL);
        udp_disconnect(_pcb_main);
        udp_remove(_pcb_main);
        _pcb_main = NULL;
    }
}

/**
 * @brief  Low level UDP Multicast membership control
 */
static esp_err_t _udp_join_group(tcpip_adapter_if_t tcpip_if, mdns_ip_protocol_t ip_protocol, bool join)
{
    struct netif * netif = NULL;
    void * nif = NULL;
    esp_err_t err = tcpip_adapter_get_netif(tcpip_if, &nif);
    if (err) {
        return ESP_ERR_INVALID_ARG;
    }
    netif = (struct netif *)nif;

    if (ip_protocol == MDNS_IP_PROTOCOL_V4) {
        ip_addr_t multicast_addr;
        IP_ADDR4(&multicast_addr, 224, 0, 0, 251);

        if(join){
            if (igmp_joingroup_netif(netif, (const struct ip4_addr *)&multicast_addr.u_addr.ip4)) {
                return ESP_ERR_INVALID_STATE;
            }
        } else {
            if (igmp_leavegroup_netif(netif, (const struct ip4_addr *)&multicast_addr.u_addr.ip4)) {
                return ESP_ERR_INVALID_STATE;
            }
        }
    } else {
        ip_addr_t multicast_addr = IPADDR6_INIT(0x000002ff, 0, 0, 0xfb000000);

        if(join){
            if (mld6_joingroup_netif(netif, &(multicast_addr.u_addr.ip6))) {
                return ESP_ERR_INVALID_STATE;
            }
        } else {
            if (mld6_leavegroup_netif(netif, &(multicast_addr.u_addr.ip6))) {
                return ESP_ERR_INVALID_STATE;
            }
        }
    }
    return ESP_OK;
}

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
 * @brief  Check if any of the interfaces is up
 */
static bool _udp_pcb_is_in_use(){
    int i, p;
    for (i=0; i<TCPIP_ADAPTER_IF_MAX; i++) {
        for (p=0; p<MDNS_IP_PROTOCOL_MAX; p++) {
            if(_mdns_server->interfaces[i].pcbs[p].pcb){
                return true;
            }
        }
    }
    return false;
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
        free(_pcb->probe_services);
        _pcb->state = PCB_OFF;
        _pcb->pcb = NULL;
        _pcb->probe_ip = false;
        _pcb->probe_services = NULL;
        _pcb->probe_services_len = 0;
        _pcb->probe_running = false;
        _pcb->failed_probes = 0;
        _udp_join_group(tcpip_if, ip_protocol, false);
        if(!_udp_pcb_is_in_use()) {
            _udp_pcb_main_deinit();
        }
    }
}

/**
 * @brief  Start PCB Main code
 */
static esp_err_t _udp_pcb_init(tcpip_adapter_if_t tcpip_if, mdns_ip_protocol_t ip_protocol)
{
    if (!_mdns_server || _mdns_server->interfaces[tcpip_if].pcbs[ip_protocol].pcb) {
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t err = _udp_join_group(tcpip_if, ip_protocol, true);
    if(err){
        return err;
    }

    err = _udp_pcb_main_init();
    if(err){
        return err;
    }

    _mdns_server->interfaces[tcpip_if].pcbs[ip_protocol].pcb = _pcb_main;
    _mdns_server->interfaces[tcpip_if].pcbs[ip_protocol].failed_probes = 0;
    return ESP_OK;
}

typedef struct {
    struct tcpip_api_call_data call;
    tcpip_adapter_if_t tcpip_if;
    mdns_ip_protocol_t ip_protocol;
    esp_err_t err;
} mdns_api_call_t;

/**
 * @brief  Start PCB from LwIP thread
 */
static err_t _mdns_pcb_init_api(struct tcpip_api_call_data *api_call_msg)
{
    mdns_api_call_t * msg = (mdns_api_call_t *)api_call_msg;
    msg->err = _udp_pcb_init(msg->tcpip_if, msg->ip_protocol);
    return msg->err;
}

/**
 * @brief  Stop PCB from LwIP thread
 */
static err_t _mdns_pcb_deinit_api(struct tcpip_api_call_data *api_call_msg)
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
    tcpip_api_call(_mdns_pcb_init_api, (struct tcpip_api_call_data*)&msg);
    return msg.err;
}

esp_err_t _mdns_pcb_deinit(tcpip_adapter_if_t tcpip_if, mdns_ip_protocol_t ip_protocol)
{
    mdns_api_call_t msg = {
        .tcpip_if = tcpip_if,
        .ip_protocol = ip_protocol
    };
    tcpip_api_call(_mdns_pcb_deinit_api, (struct tcpip_api_call_data*)&msg);
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
