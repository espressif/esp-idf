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
#include "mdns.h"

#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "lwip/ip_addr.h"
#include "lwip/pbuf.h"
#include "lwip/igmp.h"
#include "lwip/udp.h"
#include "lwip/mld6.h"
#include "lwip/priv/tcpip_priv.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "esp_event_loop.h"

#include "mdns_private.h"

#ifdef MDNS_ENABLE_DEBUG
void mdns_debug_packet(const uint8_t * data, size_t len);
#endif

static const char * MDNS_DEFAULT_DOMAIN = "local";
static const char * MDNS_SUB_STR = "_sub";

static mdns_server_t * _mdns_server = NULL;

static volatile TaskHandle_t _mdns_service_task_handle = NULL;
static SemaphoreHandle_t _mdns_service_semaphore = NULL;

static void _mdns_search_finish_done();
static mdns_search_once_t * _mdns_search_find_from(mdns_search_once_t * search, mdns_name_t * name, uint16_t type, tcpip_adapter_if_t tcpip_if, mdns_ip_protocol_t ip_protocol);
static void _mdns_search_result_add_ip(mdns_search_once_t * search, const char * hostname, ip_addr_t * ip, tcpip_adapter_if_t tcpip_if, mdns_ip_protocol_t ip_protocol);
static void _mdns_search_result_add_srv(mdns_search_once_t * search, const char * hostname, uint16_t port, tcpip_adapter_if_t tcpip_if, mdns_ip_protocol_t ip_protocol);
static void _mdns_search_result_add_txt(mdns_search_once_t * search, mdns_txt_item_t * txt, size_t txt_count, tcpip_adapter_if_t tcpip_if, mdns_ip_protocol_t ip_protocol);
static mdns_result_t * _mdns_search_result_add_ptr(mdns_search_once_t * search, const char * instance, tcpip_adapter_if_t tcpip_if, mdns_ip_protocol_t ip_protocol);

/*
 * @brief  Appends/increments a number to name/instance in case of collision
 * */
static char * _mdns_mangle_name(char* in) {
    char *p = strrchr(in, '-');
    int suffix = 0;
    if (p == NULL) {
        //No - in ``in``
        suffix = 2;
    } else {
        char *endp = NULL;
        suffix = strtol(p + 1, &endp, 10);
        if (*endp != 0) {
            //suffix is not numerical
            suffix = 2;
            p = NULL; //so we append -suffix to the entire string
        }
    }
    char *ret;
    if (p == NULL) {
        //need to add -2 to string
        ret = malloc(strlen(in) + 3);
        if (ret == NULL) {
            return NULL;
        }
        sprintf(ret, "%s-2", in);
    } else {
        ret = malloc(strlen(in) + 2); //one extra byte in case 9-10 or 99-100 etc
        strcpy(ret, in);
        int baseLen = p - in; //length of 'bla' in 'bla-123'
        //overwrite suffix with new suffix
        sprintf(ret + baseLen, "-%d", suffix + 1);
    }
    return ret;
}

/**
 * @brief  finds service from given service type
 * @param  server       the server
 * @param  service      service type to match
 * @param  proto        proto to match
 *
 * @return the service item if found or NULL on error
 */
static mdns_srv_item_t * _mdns_get_service_item(const char * service, const char * proto)
{
    mdns_srv_item_t * s = _mdns_server->services;
    while (s) {
        if (!strcasecmp(s->service->service, service) && !strcasecmp(s->service->proto, proto)) {
            return s;
        }
        s = s->next;
    }
    return NULL;
}

/*
 * MDNS Server Networking
 * */

/**
 * @brief  Queue RX packet action
 */
static esp_err_t _mdns_send_rx_action(mdns_rx_packet_t * packet)
{
    mdns_action_t * action = NULL;

    action = (mdns_action_t *)malloc(sizeof(mdns_action_t));
    if (!action) {
        return ESP_ERR_NO_MEM;
    }

    action->type = ACTION_RX_HANDLE;
    action->data.rx_handle.packet = packet;
    if (xQueueSend(_mdns_server->action_queue, &action, (portTickType)0) != pdPASS) {
        free(action);
        return ESP_ERR_NO_MEM;
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
 * @brief  Start PCB
 */
static esp_err_t _mdns_pcb_init(tcpip_adapter_if_t tcpip_if, mdns_ip_protocol_t ip_protocol)
{
    mdns_api_call_t msg = {
        .tcpip_if = tcpip_if,
        .ip_protocol = ip_protocol
    };
    tcpip_api_call(_mdns_pcb_init_api, (struct tcpip_api_call*)&msg);
    return msg.err;
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

/**
 * @brief  Stop PCB
 */
static esp_err_t _mdns_pcb_deinit(tcpip_adapter_if_t tcpip_if, mdns_ip_protocol_t ip_protocol)
{
    mdns_api_call_t msg = {
        .tcpip_if = tcpip_if,
        .ip_protocol = ip_protocol
    };
    tcpip_api_call(_mdns_pcb_deinit_api, (struct tcpip_api_call*)&msg);
    return msg.err;
}


/**
 * @brief  send packet over UDP
 *
 * @param  server       The server
 * @param  data         byte array containing the packet data
 * @param  len          length of the packet data
 *
 * @return length of sent packet or 0 on error
 */
static size_t _udp_pcb_write(tcpip_adapter_if_t tcpip_if, mdns_ip_protocol_t ip_protocol, const ip_addr_t *ip, uint16_t port, uint8_t * data, size_t len)
{
#ifndef MDNS_TEST_MODE
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
#endif
    return len;
}

/**
 * @brief  reads MDNS FQDN into mdns_name_t structure
 *         FQDN is in format: [hostname.|[instance.]_service._proto.]local.
 *
 * @param  packet       MDNS packet
 * @param  start        Starting point of FQDN
 * @param  name         mdns_name_t structure to populate
 * @param  buf          temporary char buffer
 *
 * @return the address after the parsed FQDN in the packet or NULL on error
 */
static const uint8_t * _mdns_read_fqdn(const uint8_t * packet, const uint8_t * start, mdns_name_t * name, char * buf)
{
    size_t index = 0;
    while (start[index]) {
        if (name->parts == 4) {
            return NULL;
        }
        uint8_t len = start[index++];
        if (len < 0xC0) {
            if (len > 63) {
                //length can not be more than 63
                return NULL;
            }
            uint8_t i;
            for (i=0; i<len; i++) {
                buf[i] = start[index++];
            }
            buf[len] = '\0';
            if (name->parts == 1 && buf[0] != '_'
                    && (strcasecmp(buf, MDNS_DEFAULT_DOMAIN) != 0)
                    && (strcasecmp(buf, "ip6") != 0)
                    && (strcasecmp(buf, "in-addr") != 0)) {
                strlcat(name->host, ".", sizeof(name->host));
                strlcat(name->host, buf, sizeof(name->host));
            } else if (strcasecmp(buf, MDNS_SUB_STR) == 0) {
                name->sub = 1;
            } else {
                char* mdns_name_ptrs[]={name->host, name->service, name->proto, name->domain};
                memcpy(mdns_name_ptrs[name->parts++], buf, len+1);
            }
        } else {
            size_t address = (((uint16_t)len & 0x3F) << 8) | start[index++];
            if ((packet + address) >= start) {
                //reference address can not be after where we are
                return NULL;
            }
            if (_mdns_read_fqdn(packet, packet + address, name, buf)) {
                return start + index;
            }
            return NULL;
        }
    }
    return start + index + 1;
}

/**
 * @brief  sets uint16_t value in a packet
 *
 * @param  packet       MDNS packet
 * @param  index        offset of uint16_t value
 * @param  value        the value to set
 */
static inline void _mdns_set_u16(uint8_t * packet, uint16_t index, uint16_t value)
{
    if ((index + 1) >= MDNS_MAX_PACKET_SIZE) {
        return;
    }
    packet[index] = (value >> 8) & 0xFF;
    packet[index+1] = value & 0xFF;
}

/**
 * @brief  appends byte in a packet, incrementing the index
 *
 * @param  packet       MDNS packet
 * @param  index        offset in the packet
 * @param  value        the value to set
 *
 * @return length of added data: 0 on error or 1 on success
 */
static inline uint8_t _mdns_append_u8(uint8_t * packet, uint16_t * index, uint8_t value)
{
    if (*index >= MDNS_MAX_PACKET_SIZE) {
        return 0;
    }
    packet[*index] = value;
    *index += 1;
    return 1;
}

/**
 * @brief  appends uint16_t in a packet, incrementing the index
 *
 * @param  packet       MDNS packet
 * @param  index        offset in the packet
 * @param  value        the value to set
 *
 * @return length of added data: 0 on error or 2 on success
 */
static inline uint8_t _mdns_append_u16(uint8_t * packet, uint16_t * index, uint16_t value)
{
    if ((*index + 1) >= MDNS_MAX_PACKET_SIZE) {
        return 0;
    }
    _mdns_append_u8(packet, index, (value >> 8) & 0xFF);
    _mdns_append_u8(packet, index, value & 0xFF);
    return 2;
}

/**
 * @brief  appends uint32_t in a packet, incrementing the index
 *
 * @param  packet       MDNS packet
 * @param  index        offset in the packet
 * @param  value        the value to set
 *
 * @return length of added data: 0 on error or 4 on success
 */
static inline uint8_t _mdns_append_u32(uint8_t * packet, uint16_t * index, uint32_t value)
{
    if ((*index + 3) >= MDNS_MAX_PACKET_SIZE) {
        return 0;
    }
    _mdns_append_u8(packet, index, (value >> 24) & 0xFF);
    _mdns_append_u8(packet, index, (value >> 16) & 0xFF);
    _mdns_append_u8(packet, index, (value >> 8) & 0xFF);
    _mdns_append_u8(packet, index, value & 0xFF);
    return 4;
}

/**
 * @brief  appends answer type, class, ttl and data length to a packet, incrementing the index
 *
 * @param  packet       MDNS packet
 * @param  index        offset in the packet
 * @param  type         answer type
 * @param  ttl          answer ttl
 *
 * @return length of added data: 0 on error or 10 on success
 */
static inline uint8_t _mdns_append_type(uint8_t * packet, uint16_t * index, uint8_t type, bool flush, uint32_t ttl)
{
    if ((*index + 10) >= MDNS_MAX_PACKET_SIZE) {
        return 0;
    }
    uint16_t clas = MDNS_CLASS_IN;
    if (flush) {
        clas = MDNS_CLASS_IN_FLUSH_CACHE;
    }
    if (type == MDNS_ANSWER_PTR) {
        _mdns_append_u16(packet, index, MDNS_TYPE_PTR);
        _mdns_append_u16(packet, index, clas);
    } else if (type == MDNS_ANSWER_TXT) {
        _mdns_append_u16(packet, index, MDNS_TYPE_TXT);
        _mdns_append_u16(packet, index, clas);
    } else if (type == MDNS_ANSWER_SRV) {
        _mdns_append_u16(packet, index, MDNS_TYPE_SRV);
        _mdns_append_u16(packet, index, clas);
    } else if (type == MDNS_ANSWER_A) {
        _mdns_append_u16(packet, index, MDNS_TYPE_A);
        _mdns_append_u16(packet, index, clas);
    } else if (type == MDNS_ANSWER_AAAA) {
        _mdns_append_u16(packet, index, MDNS_TYPE_AAAA);
        _mdns_append_u16(packet, index, clas);
    } else {
        return 0;
    }
    _mdns_append_u32(packet, index, ttl);
    _mdns_append_u16(packet, index, 0);
    return 10;
}

/**
 * @brief  appends single string to a packet, incrementing the index
 *
 * @param  packet       MDNS packet
 * @param  index        offset in the packet
 * @param  string       the string to append
 *
 * @return length of added data: 0 on error or length of the string + 1 on success
 */
static inline uint8_t _mdns_append_string(uint8_t * packet, uint16_t * index, const char * string)
{
    uint8_t len = strlen(string);
    if ((*index + len + 1) >= MDNS_MAX_PACKET_SIZE) {
        return 0;
    }
    _mdns_append_u8(packet, index, len);
    memcpy(packet + *index, string, len);
    *index += len;
    return len + 1;
}

/**
 * @brief  appends FQDN to a packet, incrementing the index and
 *         compressing the output if previous occurrence of the string (or part of it) has been found
 *
 * @param  packet       MDNS packet
 * @param  index        offset in the packet
 * @param  strings      string array containing the parts of the FQDN
 * @param  count        number of strings in the array
 *
 * @return length of added data: 0 on error or length on success
 */
static uint16_t _mdns_append_fqdn(uint8_t * packet, uint16_t * index, const char * strings[], uint8_t count)
{
    if (!count) {
        //empty string so terminate
        return _mdns_append_u8(packet, index, 0);
    }
    mdns_name_t name;
    static char buf[MDNS_NAME_BUF_LEN];
    uint8_t len = strlen(strings[0]);
    //try to find first the string length in the packet (if it exists)
    uint8_t * len_location = (uint8_t *)memchr(packet, (char)len, *index);
    while (len_location) {
        //check if the string after len_location is the string that we are looking for
        if (memcmp(len_location+1, strings[0], len)) { //not continuing with our string
search_next:
            //try and find the length byte further in the packet
            len_location = (uint8_t *)memchr(len_location+1, (char)len, *index - (len_location+1 - packet));
            continue;
        }
        //seems that we might have found the string that we are looking for
        //read the destination into name and compare
        name.parts = 0;
        name.sub = 0;
        name.host[0] = 0;
        name.service[0] = 0;
        name.proto[0] = 0;
        name.domain[0] = 0;
        const uint8_t * content = _mdns_read_fqdn(packet, len_location, &name, buf);
        if (!content) {
            //not a readable fqdn?
            return 0;
        }
        if (name.parts == count) {
            uint8_t i;
            for (i=0; i<count; i++) {
                if (strcasecmp(strings[i], (const char *)&name + (i * (MDNS_NAME_BUF_LEN)))) {
                    //not our string! let's search more
                    goto search_next;
                }
            }
            //we actually have found the string
            break;
        } else {
            goto search_next;
        }
    }
    //string is not yet in the packet, so let's add it
    if (!len_location) {
        uint8_t written = _mdns_append_string(packet, index, strings[0]);
        if (!written) {
            return 0;
        }
        //run the same for the other strings in the name
        return written + _mdns_append_fqdn(packet, index, &strings[1], count - 1);
    }

    //we have found the string so let's insert a pointer to it instead
    uint16_t offset = len_location - packet;
    offset |= MDNS_NAME_REF;
    return _mdns_append_u16(packet, index, offset);
}

/**
 * @brief  appends PTR record for service to a packet, incrementing the index
 *
 * @param  packet       MDNS packet
 * @param  index        offset in the packet
 * @param  server       the server that is hosting the service
 * @param  service      the service to add record for
 *
 * @return length of added data: 0 on error or length on success
 */
static uint16_t _mdns_append_ptr_record(uint8_t * packet, uint16_t * index, const char * instance, const char * service, const char * proto, bool flush, bool bye)
{
    const char * str[4];
    uint16_t record_length = 0;
    uint8_t part_length;

    str[0] = instance;
    str[1] = service;
    str[2] = proto;
    str[3] = MDNS_DEFAULT_DOMAIN;

    part_length = _mdns_append_fqdn(packet, index, str + 1, 3);
    if (!part_length) {
        return 0;
    }
    record_length += part_length;

    part_length = _mdns_append_type(packet, index, MDNS_ANSWER_PTR, false, bye?0:MDNS_ANSWER_PTR_TTL);
    if (!part_length) {
        return 0;
    }
    record_length += part_length;

    uint16_t data_len_location = *index - 2;
    part_length = _mdns_append_fqdn(packet, index, str, 4);
    if (!part_length) {
        return 0;
    }
    _mdns_set_u16(packet, data_len_location, part_length);
    record_length += part_length;
    return record_length;
}

/**
 * @brief  appends DNS-SD PTR record for service to a packet, incrementing the index
 *
 * @param  packet       MDNS packet
 * @param  index        offset in the packet
 * @param  server       the server that is hosting the service
 * @param  service      the service to add record for
 *
 * @return length of added data: 0 on error or length on success
 */
static uint16_t _mdns_append_sdptr_record(uint8_t * packet, uint16_t * index, mdns_service_t * service, bool flush, bool bye)
{
    const char * str[3];
    const char * sd_str[4];
    uint16_t record_length = 0;
    uint8_t part_length;

    sd_str[0] = (char*)"_services";
    sd_str[1] = (char*)"_dns-sd";
    sd_str[2] = (char*)"_udp";
    sd_str[3] = MDNS_DEFAULT_DOMAIN;

    str[0] = service->service;
    str[1] = service->proto;
    str[2] = MDNS_DEFAULT_DOMAIN;

    part_length = _mdns_append_fqdn(packet, index, sd_str, 4);

    record_length += part_length;

    part_length = _mdns_append_type(packet, index, MDNS_ANSWER_PTR, flush, MDNS_ANSWER_PTR_TTL);
    if (!part_length) {
        return 0;
    }
    record_length += part_length;

    uint16_t data_len_location = *index - 2;
    part_length = _mdns_append_fqdn(packet, index, str, 3);
    if (!part_length) {
        return 0;
    }
    _mdns_set_u16(packet, data_len_location, part_length);
    record_length += part_length;
    return record_length;
}

/**
 * @brief  appends TXT record for service to a packet, incrementing the index
 *
 * @param  packet       MDNS packet
 * @param  index        offset in the packet
 * @param  server       the server that is hosting the service
 * @param  service      the service to add record for
 *
 * @return length of added data: 0 on error or length on success
 */
static uint16_t _mdns_append_txt_record(uint8_t * packet, uint16_t * index, mdns_service_t * service, bool flush, bool bye)
{
    const char * str[4];
    uint16_t record_length = 0;
    uint8_t part_length;

    str[0] = (service->instance)?service->instance
             :(_mdns_server->instance)?_mdns_server->instance
             :_mdns_server->hostname;
    str[1] = service->service;
    str[2] = service->proto;
    str[3] = MDNS_DEFAULT_DOMAIN;

    part_length = _mdns_append_fqdn(packet, index, str, 4);
    if (!part_length) {
        return 0;
    }
    record_length += part_length;

    part_length = _mdns_append_type(packet, index, MDNS_ANSWER_TXT, flush, bye?0:MDNS_ANSWER_TXT_TTL);
    if (!part_length) {
        return 0;
    }
    record_length += part_length;

    uint16_t data_len_location = *index - 2;
    uint16_t data_len = 0;

    char * tmp;
    mdns_txt_linked_item_t * txt = service->txt;
    while (txt) {
        tmp = (char *)malloc(2 + strlen(txt->key) + strlen(txt->value));
        if (tmp) {
            sprintf(tmp, "%s=%s", txt->key, txt->value);
            uint8_t l = _mdns_append_string(packet, index, tmp);
            free(tmp);
            if (!l) {
                return 0;
            }
            data_len += l;
        }
        txt = txt->next;
    }
    if (!data_len) {
        data_len = 1;
        packet[*index] = 0;
        *index = *index + 1;
    }
    _mdns_set_u16(packet, data_len_location, data_len);
    record_length += data_len;
    return record_length;
}

/**
 * @brief  appends SRV record for service to a packet, incrementing the index
 *
 * @param  packet       MDNS packet
 * @param  index        offset in the packet
 * @param  server       the server that is hosting the service
 * @param  service      the service to add record for
 *
 * @return length of added data: 0 on error or length on success
 */
static uint16_t _mdns_append_srv_record(uint8_t * packet, uint16_t * index, mdns_service_t * service, bool flush, bool bye)
{
    const char * str[4];
    uint16_t record_length = 0;
    uint8_t part_length;

    str[0] = (service->instance)?service->instance
             :(_mdns_server->instance)?_mdns_server->instance
             :_mdns_server->hostname;
    str[1] = service->service;
    str[2] = service->proto;
    str[3] = MDNS_DEFAULT_DOMAIN;

    part_length = _mdns_append_fqdn(packet, index, str, 4);
    if (!part_length) {
        return 0;
    }
    record_length += part_length;

    part_length = _mdns_append_type(packet, index, MDNS_ANSWER_SRV, flush, bye?0:MDNS_ANSWER_SRV_TTL);
    if (!part_length) {
        return 0;
    }
    record_length += part_length;

    uint16_t data_len_location = *index - 2;

    part_length = 0;
    part_length += _mdns_append_u16(packet, index, service->priority);
    part_length += _mdns_append_u16(packet, index, service->weight);
    part_length += _mdns_append_u16(packet, index, service->port);
    if (part_length != 6) {
        return 0;
    }

    str[0] = _mdns_server->hostname;
    str[1] = MDNS_DEFAULT_DOMAIN;
    part_length = _mdns_append_fqdn(packet, index, str, 2);
    if (!part_length) {
        return 0;
    }
    _mdns_set_u16(packet, data_len_location, part_length + 6);

    record_length += part_length + 6;
    return record_length;
}

/**
 * @brief  appends A record to a packet, incrementing the index
 *
 * @param  packet       MDNS packet
 * @param  index        offset in the packet
 * @param  server       the server
 * @param  ip           the IP address to add
 *
 * @return length of added data: 0 on error or length on success
 */
static uint16_t _mdns_append_a_record(uint8_t * packet, uint16_t * index, uint32_t ip, bool flush, bool bye)
{
    const char * str[2];
    uint16_t record_length = 0;
    uint8_t part_length;

    str[0] = _mdns_server->hostname;
    str[1] = MDNS_DEFAULT_DOMAIN;

    part_length = _mdns_append_fqdn(packet, index, str, 2);
    if (!part_length) {
        return 0;
    }
    record_length += part_length;

    part_length = _mdns_append_type(packet, index, MDNS_ANSWER_A, flush, bye?0:MDNS_ANSWER_A_TTL);
    if (!part_length) {
        return 0;
    }
    record_length += part_length;

    uint16_t data_len_location = *index - 2;

    if ((*index + 3) >= MDNS_MAX_PACKET_SIZE) {
        return 0;
    }
    _mdns_append_u8(packet, index, ip & 0xFF);
    _mdns_append_u8(packet, index, (ip >> 8) & 0xFF);
    _mdns_append_u8(packet, index, (ip >> 16) & 0xFF);
    _mdns_append_u8(packet, index, (ip >> 24) & 0xFF);
    _mdns_set_u16(packet, data_len_location, 4);

    record_length += 4;
    return record_length;
}

/**
 * @brief  appends AAAA record to a packet, incrementing the index
 *
 * @param  packet       MDNS packet
 * @param  index        offset in the packet
 * @param  ipv6         the IPv6 address to add
 *
 * @return length of added data: 0 on error or length on success
 */
static uint16_t _mdns_append_aaaa_record(uint8_t * packet, uint16_t * index, uint8_t * ipv6, bool flush, bool bye)
{
    const char * str[2];
    uint16_t record_length = 0;
    uint8_t part_length;

    str[0] = _mdns_server->hostname;
    str[1] = MDNS_DEFAULT_DOMAIN;

    part_length = _mdns_append_fqdn(packet, index, str, 2);
    if (!part_length) {
        return 0;
    }
    record_length += part_length;

    part_length = _mdns_append_type(packet, index, MDNS_ANSWER_AAAA, flush, bye?0:MDNS_ANSWER_AAAA_TTL);
    if (!part_length) {
        return 0;
    }
    record_length += part_length;

    uint16_t data_len_location = *index - 2;

    if ((*index + 15) >= MDNS_MAX_PACKET_SIZE) {
        return 0;
    }

    part_length = sizeof(ip6_addr_t);
    memcpy(packet + *index, ipv6, part_length);
    *index += part_length;
    _mdns_set_u16(packet, data_len_location, part_length);
    record_length += part_length;
    return record_length;
}

/**
 * @brief  Append question to packet
 */
static uint16_t _mdns_append_question(uint8_t * packet, uint16_t * index, mdns_out_question_t * q)
{
    const char * str[4];
    uint8_t str_index = 0;
    uint8_t part_length;
    if (q->host) {
        str[str_index++] = q->host;
    }
    if (q->service) {
        str[str_index++] = q->service;
    }
    if (q->proto) {
        str[str_index++] = q->proto;
    }
    if (q->domain) {
        str[str_index++] = q->domain;
    }

    part_length = _mdns_append_fqdn(packet, index, str, str_index);
    if (!part_length) {
        return 0;
    }

    part_length += _mdns_append_u16(packet, index, q->type);
    part_length += _mdns_append_u16(packet, index, q->unicast?0x8001:0x0001);
    return part_length;
}

/**
 * @brief  Helper to get either ETH or STA if the other is provided
 *          Used when two interfaces are on the same subnet
 */
static tcpip_adapter_if_t _mdns_get_other_if (tcpip_adapter_if_t tcpip_if)
{
    if (tcpip_if == TCPIP_ADAPTER_IF_STA) {
        return TCPIP_ADAPTER_IF_ETH;
    } else if (tcpip_if == TCPIP_ADAPTER_IF_ETH) {
        return TCPIP_ADAPTER_IF_STA;
    }
    return TCPIP_ADAPTER_IF_MAX;
}

/**
 * @brief  Check if interface is duplicate (two interfaces on the same subnet)
 */
static bool _mdns_if_is_dup(tcpip_adapter_if_t tcpip_if)
{
    tcpip_adapter_if_t other_if = _mdns_get_other_if (tcpip_if);
    if (other_if == TCPIP_ADAPTER_IF_MAX) {
        return false;
    }
    if (_mdns_server->interfaces[tcpip_if].pcbs[MDNS_IP_PROTOCOL_V4].state == PCB_DUP
        || _mdns_server->interfaces[tcpip_if].pcbs[MDNS_IP_PROTOCOL_V6].state == PCB_DUP
        || _mdns_server->interfaces[other_if].pcbs[MDNS_IP_PROTOCOL_V4].state == PCB_DUP
        || _mdns_server->interfaces[other_if].pcbs[MDNS_IP_PROTOCOL_V6].state == PCB_DUP
    ) {
        return true;
    }
    return false;
}

/**
 * @brief  Check if IPv6 address is NULL
 */
static bool _ipv6_address_is_zero(ip6_addr_t ip6)
{
    uint8_t i;
    uint8_t * data = (uint8_t *)ip6.addr;
    for (i=0; i<16; i++) {
        if (data[i]) {
            return false;
        }
    }
    return true;
}

/**
 * @brief  Append answer to packet
 *
 *  @return number of answers added to the packet
 */
static uint8_t _mdns_append_answer(uint8_t * packet, uint16_t * index, mdns_out_answer_t * answer, tcpip_adapter_if_t tcpip_if)
{
    if (answer->type == MDNS_TYPE_PTR) {

        if (answer->service) {
            return _mdns_append_ptr_record(packet, index,
                (answer->service->instance)?answer->service->instance
                    :(_mdns_server->instance)?_mdns_server->instance
                    :_mdns_server->hostname,
                answer->service->service, answer->service->proto,
                answer->flush, answer->bye) > 0;
        } else {
            return _mdns_append_ptr_record(packet, index,
                answer->custom_instance, answer->custom_service, answer->custom_proto,
                answer->flush, answer->bye) > 0;
        }
    } else if (answer->type == MDNS_TYPE_SRV) {
        return _mdns_append_srv_record(packet, index, answer->service, answer->flush, answer->bye) > 0;
    } else if (answer->type == MDNS_TYPE_TXT) {
        return _mdns_append_txt_record(packet, index, answer->service, answer->flush, answer->bye) > 0;
    } else if (answer->type == MDNS_TYPE_SDPTR) {
        return _mdns_append_sdptr_record(packet, index, answer->service, answer->flush, answer->bye) > 0;
    } else if (answer->type == MDNS_TYPE_A) {
        tcpip_adapter_ip_info_t if_ip_info;
        if (!_mdns_server->interfaces[tcpip_if].pcbs[MDNS_IP_PROTOCOL_V4].pcb && _mdns_server->interfaces[tcpip_if].pcbs[MDNS_IP_PROTOCOL_V4].state != PCB_DUP) {
            return 0;
        }
        if (tcpip_adapter_get_ip_info(tcpip_if, &if_ip_info)) {
            return 0;
        }
        if (_mdns_append_a_record(packet, index, if_ip_info.ip.addr, answer->flush, answer->bye) <= 0) {
            return 0;
        }
        if (!_mdns_if_is_dup(tcpip_if)) {
            return 1;
        }
        tcpip_adapter_if_t other_if = _mdns_get_other_if (tcpip_if);
        if (tcpip_adapter_get_ip_info(other_if, &if_ip_info)) {
            return 1;
        }
        if (_mdns_append_a_record(packet, index, if_ip_info.ip.addr, answer->flush, answer->bye) > 0) {
            return 2;
        }
        return 1;
    } else if (answer->type == MDNS_TYPE_AAAA) {
        struct ip6_addr if_ip6;
        if (!_mdns_server->interfaces[tcpip_if].pcbs[MDNS_IP_PROTOCOL_V6].pcb && _mdns_server->interfaces[tcpip_if].pcbs[MDNS_IP_PROTOCOL_V6].state != PCB_DUP) {
            return 0;
        }
        if (tcpip_adapter_get_ip6_linklocal(tcpip_if, &if_ip6)) {
            return 0;
        }
        if (_ipv6_address_is_zero(if_ip6)) {
            return 0;
        }
        if (_mdns_append_aaaa_record(packet, index, (uint8_t*)if_ip6.addr, answer->flush, answer->bye) <= 0) {
            return 0;
        }
        if (!_mdns_if_is_dup(tcpip_if)) {
            return 1;
        }
        tcpip_adapter_if_t other_if = _mdns_get_other_if (tcpip_if);
        if (tcpip_adapter_get_ip6_linklocal(other_if, &if_ip6)) {
            return 1;
        }
        if (_mdns_append_aaaa_record(packet, index, (uint8_t*)if_ip6.addr, answer->flush, answer->bye) > 0) {
            return 2;
        }
        return 1;
    }
    return 0;
}

/**
 * @brief  sends a packet
 *
 * @param  p       the packet
 */
static void _mdns_dispatch_tx_packet(mdns_tx_packet_t * p)
{
    static uint8_t packet[MDNS_MAX_PACKET_SIZE];
    uint16_t index = MDNS_HEAD_LEN;
    memset(packet, 0, MDNS_HEAD_LEN);
    mdns_out_question_t * q;
    mdns_out_answer_t * a;
    uint8_t count;

    _mdns_set_u16(packet, MDNS_HEAD_FLAGS_OFFSET, p->flags);

    count = 0;
    q = p->questions;
    while (q) {
        if (_mdns_append_question(packet, &index, q)) {
            count++;
        }
        q = q->next;
    }
    _mdns_set_u16(packet, MDNS_HEAD_QUESTIONS_OFFSET, count);

    count = 0;
    a = p->answers;
    while (a) {
        count += _mdns_append_answer(packet, &index, a, p->tcpip_if);
        a = a->next;
    }
    _mdns_set_u16(packet, MDNS_HEAD_ANSWERS_OFFSET, count);

    count = 0;
    a = p->servers;
    while (a) {
        count += _mdns_append_answer(packet, &index, a, p->tcpip_if);
        a = a->next;
    }
    _mdns_set_u16(packet, MDNS_HEAD_SERVERS_OFFSET, count);

    count = 0;
    a = p->additional;
    while (a) {
        count += _mdns_append_answer(packet, &index, a, p->tcpip_if);
        a = a->next;
    }
    _mdns_set_u16(packet, MDNS_HEAD_ADDITIONAL_OFFSET, count);

#ifdef MDNS_ENABLE_DEBUG
    _mdns_dbg_printf("\nTX[%u][%u]: ", p->tcpip_if, p->ip_protocol);
    if (p->dst.type == IPADDR_TYPE_V4) {
        _mdns_dbg_printf("To: " IPSTR ":%u, ", IP2STR(&p->dst.u_addr.ip4), p->port);
    } else {
        _mdns_dbg_printf("To: " IPV6STR ":%u, ", IPV62STR(p->dst.u_addr.ip6), p->port);
    }
    mdns_debug_packet(packet, index);
#endif

    _udp_pcb_write(p->tcpip_if, p->ip_protocol, &p->dst, p->port, packet, index);
}

/**
 * @brief  frees a packet
 *
 * @param  packet       the packet
 */
static void _mdns_free_tx_packet(mdns_tx_packet_t * packet)
{
    if (!packet) {
        return;
    }
    queueFree(mdns_out_question_t, packet->questions);
    queueFree(mdns_out_answer_t, packet->answers);
    queueFree(mdns_out_answer_t, packet->servers);
    queueFree(mdns_out_answer_t, packet->additional);
    free(packet);
}

/**
 * @brief  schedules a packet to be sent after given milliseconds
 *
 * @param  packet       the packet
 * @param  ms_after     number of milliseconds after which the packet should be dispatched
 */
static void _mdns_schedule_tx_packet(mdns_tx_packet_t * packet, uint32_t ms_after)
{
    if (!packet) {
        return;
    }
    packet->send_at = (xTaskGetTickCount() * portTICK_PERIOD_MS) + ms_after;
    packet->next = NULL;
    if (!_mdns_server->tx_queue_head || _mdns_server->tx_queue_head->send_at > packet->send_at) {
        packet->next = _mdns_server->tx_queue_head;
        _mdns_server->tx_queue_head = packet;
        return;
    }
    mdns_tx_packet_t * q = _mdns_server->tx_queue_head;
    while (q->next && q->next->send_at <= packet->send_at) {
        q = q->next;
    }
    packet->next = q->next;
    q->next = packet;
}

/**
 * @brief  free all packets scheduled for sending
 */
static void _mdns_clear_tx_queue_head()
{
    mdns_tx_packet_t * q;
    while (_mdns_server->tx_queue_head) {
        q = _mdns_server->tx_queue_head;
        _mdns_server->tx_queue_head = _mdns_server->tx_queue_head->next;
        _mdns_free_tx_packet(q);
    }
}

/**
 * @brief  clear packets scheduled for sending on a specific interface
 *
 * @param  tcpip_if     the interface
 * @param  ip_protocol     pcb type V4/V6
 */
static void _mdns_clear_pcb_tx_queue_head(tcpip_adapter_if_t tcpip_if, mdns_ip_protocol_t ip_protocol)
{
    mdns_tx_packet_t * q, * p;
    while (_mdns_server->tx_queue_head && _mdns_server->tx_queue_head->tcpip_if == tcpip_if && _mdns_server->tx_queue_head->ip_protocol == ip_protocol) {
        q = _mdns_server->tx_queue_head;
        _mdns_server->tx_queue_head = _mdns_server->tx_queue_head->next;
        _mdns_free_tx_packet(q);
    }
    if (_mdns_server->tx_queue_head) {
        q = _mdns_server->tx_queue_head;
        while (q->next) {
            if (q->next->tcpip_if == tcpip_if && q->next->ip_protocol == ip_protocol) {
                p = q->next;
                q->next = p->next;
                _mdns_free_tx_packet(p);
            } else {
                q = q->next;
            }
        }
    }
}

/**
 * @brief  get the next packet scheduled for sending on a specific interface
 *
 * @param  tcpip_if     the interface
 * @param  ip_protocol     pcb type V4/V6
 */
static mdns_tx_packet_t * _mdns_get_next_pcb_packet(tcpip_adapter_if_t tcpip_if, mdns_ip_protocol_t ip_protocol)
{
    mdns_tx_packet_t * q = _mdns_server->tx_queue_head;
    while (q) {
        if (q->tcpip_if == tcpip_if && q->ip_protocol == ip_protocol) {
            return q;
        }
        q = q->next;
    }
    return NULL;
}

/**
 * @brief  Find, remove and free answer from the scheduled packets
 */
static void _mdns_remove_scheduled_answer(tcpip_adapter_if_t tcpip_if, mdns_ip_protocol_t ip_protocol, uint16_t type, mdns_srv_item_t * service)
{
    mdns_srv_item_t s = {NULL, NULL};
    if (!service) {
        service = &s;
    }
    mdns_tx_packet_t * q = _mdns_server->tx_queue_head;
    while (q) {
        if (q->tcpip_if == tcpip_if && q->ip_protocol == ip_protocol && q->distributed) {
            mdns_out_answer_t * a = q->answers;
            if (a->type == type && a->service == service->service) {
                q->answers = q->answers->next;
                free(a);
            } else {
                while (a->next) {
                    if (a->next->type == type && a->next->service == service->service) {
                        mdns_out_answer_t * b = a->next;
                        a->next = b->next;
                        free(b);
                        break;
                    }
                    a = a->next;
                }
            }
        }
        q = q->next;
    }
}

/**
 * @brief  Remove and free answer from answer list (destination)
 */
static void _mdns_dealloc_answer(mdns_out_answer_t ** destnation, uint16_t type, mdns_srv_item_t * service)
{
    mdns_out_answer_t * d = *destnation;
    if (!d) {
        return;
    }
    mdns_srv_item_t s = {NULL, NULL};
    if (!service) {
        service = &s;
    }
    if (d->type == type && d->service == service->service) {
        *destnation = d->next;
        free(d);
        return;
    }
    while (d->next) {
        mdns_out_answer_t * a = d->next;
        if (a->type == type && a->service == service->service) {
            d->next = a->next;
            free(a);
            return;
        }
        d = d->next;
    }
}

/**
 * @brief  Allocate new answer and add it to answer list (destination)
 */
static bool _mdns_alloc_answer(mdns_out_answer_t ** destnation, uint16_t type, mdns_service_t * service, bool flush, bool bye)
{
    mdns_out_answer_t * d = *destnation;
    while (d) {
        if (d->type == type && d->service == service) {
            return true;
        }
        d = d->next;
    }

    mdns_out_answer_t * a = (mdns_out_answer_t *)malloc(sizeof(mdns_out_answer_t));
    if (!a) {
        return false;
    }
    a->type = type;
    a->service = service;
    a->bye = bye;
    a->flush = flush;
    a->next = NULL;
    queueToEnd(mdns_out_answer_t, *destnation, a);
    return true;
}

/**
 * @brief  Allocate new packet for sending
 */
static mdns_tx_packet_t * _mdns_alloc_packet_default(tcpip_adapter_if_t tcpip_if, mdns_ip_protocol_t ip_protocol)
{
    mdns_tx_packet_t * packet = (mdns_tx_packet_t*)malloc(sizeof(mdns_tx_packet_t));
    if (!packet) {
        return NULL;
    }
    memset((uint8_t*)packet, 0, sizeof(mdns_tx_packet_t));
    packet->tcpip_if = tcpip_if;
    packet->ip_protocol = ip_protocol;
    packet->port = MDNS_SERVICE_PORT;
    if (ip_protocol == MDNS_IP_PROTOCOL_V4) {
        IP_ADDR4(&packet->dst, 224, 0, 0, 251);
    } else {
        ip_addr_t addr = IPADDR6_INIT(0x000002ff, 0, 0, 0xfb000000);
        memcpy(&packet->dst, &addr, sizeof(ip_addr_t));
    }
    return packet;
}

/**
 * @brief  Create answer packet to questions from parsed packet
 */
static void _mdns_create_answer_from_parsed_packet(mdns_parsed_packet_t * parsed_packet)
{
    if (!parsed_packet->questions) {
        return;
    }
    bool send_flush = parsed_packet->src_port == MDNS_SERVICE_PORT;
    bool unicast = false;
    bool shared = false;
    mdns_tx_packet_t * packet = _mdns_alloc_packet_default(parsed_packet->tcpip_if, parsed_packet->ip_protocol);
    if (!packet) {
        return;
    }
    packet->flags = MDNS_FLAGS_AUTHORITATIVE;
    packet->distributed = parsed_packet->distributed;

    mdns_parsed_question_t * q = parsed_packet->questions;
    while (q) {
        mdns_srv_item_t * service = NULL;
        if (q->service && q->proto) {
            service = _mdns_get_service_item(q->service, q->proto);
            if (!service) {
                continue;
            }
        }
        if (q->unicast) {
            unicast = true;
        }
        if (service) {
            if (q->type == MDNS_TYPE_PTR || q->type == MDNS_TYPE_ANY) {
                if (q->type == MDNS_TYPE_PTR) {
                    shared = true;
                }
                if (!_mdns_alloc_answer(&packet->answers, MDNS_TYPE_PTR, service->service, false, false)
                        || !_mdns_alloc_answer(&packet->answers, MDNS_TYPE_SRV, service->service, send_flush, false)
                        || !_mdns_alloc_answer(&packet->answers, MDNS_TYPE_TXT, service->service, send_flush, false)
                        || !_mdns_alloc_answer(shared?&packet->additional:&packet->answers, MDNS_TYPE_A, NULL, send_flush, false)
                        || !_mdns_alloc_answer(shared?&packet->additional:&packet->answers, MDNS_TYPE_AAAA, NULL, send_flush, false)) {
                    _mdns_free_tx_packet(packet);
                    return;
                }
            } else if (q->type == MDNS_TYPE_SRV) {
                if (!_mdns_alloc_answer(&packet->answers, MDNS_TYPE_SRV, service->service, send_flush, false)
                        || !_mdns_alloc_answer(&packet->additional, MDNS_TYPE_A, NULL, send_flush, false)
                        || !_mdns_alloc_answer(&packet->additional, MDNS_TYPE_AAAA, NULL, send_flush, false)) {
                    _mdns_free_tx_packet(packet);
                    return;
                }
            } else if (q->type == MDNS_TYPE_TXT) {
                if (!_mdns_alloc_answer(&packet->answers, MDNS_TYPE_TXT, service->service, send_flush, false)) {
                    _mdns_free_tx_packet(packet);
                    return;
                }
            } else if (q->type == MDNS_TYPE_SDPTR) {
                shared = true;
                if (!_mdns_alloc_answer(&packet->answers, MDNS_TYPE_PTR, service->service, false, false)) {
                    _mdns_free_tx_packet(packet);
                    return;
                }
            }
        } else {
            if (q->type == MDNS_TYPE_ANY || q->type == MDNS_TYPE_A || q->type == MDNS_TYPE_AAAA) {
                if (!_mdns_alloc_answer(&packet->answers, MDNS_TYPE_A, NULL, send_flush, false)
                        || !_mdns_alloc_answer(&packet->answers, MDNS_TYPE_AAAA, NULL, send_flush, false)) {
                    _mdns_free_tx_packet(packet);
                    return;
                }
            } else if (!_mdns_alloc_answer(&packet->answers, q->type, NULL, send_flush, false)) {
                _mdns_free_tx_packet(packet);
                return;
            }
        }
        q = q->next;
    }
    if (unicast || !send_flush) {
        memcpy(&packet->dst, &parsed_packet->src, sizeof(ip_addr_t));
        packet->port = parsed_packet->src_port;
    }

    static uint8_t share_step = 0;
    if (shared) {
        _mdns_schedule_tx_packet(packet, 25 + (share_step * 25));
        share_step = (share_step + 1) & 0x03;
    } else {
        _mdns_dispatch_tx_packet(packet);
        _mdns_free_tx_packet(packet);
    }
}

/**
 * @brief  Check if question is already in the list
 */
static bool _mdns_question_exists(mdns_out_question_t * needle, mdns_out_question_t * haystack)
{
    while (haystack) {
        if (haystack->type == needle->type
            && haystack->host == needle->host
            && haystack->service == needle->service
            && haystack->proto == needle->proto) {
            return true;
        }
        haystack = haystack->next;
    }
    return false;
}

/**
 * @brief  Create probe packet for particular services on particular PCB
 */
static mdns_tx_packet_t * _mdns_create_probe_packet(tcpip_adapter_if_t tcpip_if, mdns_ip_protocol_t ip_protocol, mdns_srv_item_t * services[], size_t len, bool first, bool include_ip)
{
    mdns_tx_packet_t * packet = _mdns_alloc_packet_default(tcpip_if, ip_protocol);
    if (!packet) {
        return NULL;
    }

    size_t i;
    for (i=0; i<len; i++) {
        mdns_out_question_t * q = (mdns_out_question_t *)malloc(sizeof(mdns_out_question_t));
        if (!q) {
            _mdns_free_tx_packet(packet);
            return NULL;
        }
        q->next = NULL;
        q->unicast = first;
        q->type = MDNS_TYPE_ANY;
        q->host = (services[i]->service->instance)?services[i]->service->instance
                  :(_mdns_server->instance)?_mdns_server->instance
                  :_mdns_server->hostname;
        q->service = services[i]->service->service;
        q->proto = services[i]->service->proto;
        q->domain = MDNS_DEFAULT_DOMAIN;
        if (_mdns_question_exists(q, packet->questions)) {
            free(q);
        } else {
            queueToEnd(mdns_out_question_t, packet->questions, q);
        }

        if (!_mdns_alloc_answer(&packet->servers, MDNS_TYPE_SRV, services[i]->service, false, false)) {
            _mdns_free_tx_packet(packet);
            return NULL;
        }
    }

    if (include_ip) {
        mdns_out_question_t * q = (mdns_out_question_t *)malloc(sizeof(mdns_out_question_t));
        if (!q) {
            _mdns_free_tx_packet(packet);
            return NULL;
        }
        q->next = NULL;
        q->unicast = first;
        q->type = MDNS_TYPE_ANY;
        q->host = _mdns_server->hostname;
        q->service = NULL;
        q->proto = NULL;
        q->domain = MDNS_DEFAULT_DOMAIN;
        if (_mdns_question_exists(q, packet->questions)) {
            free(q);
        } else {
            queueToEnd(mdns_out_question_t, packet->questions, q);
        }

        if (_mdns_server->interfaces[tcpip_if].pcbs[MDNS_IP_PROTOCOL_V4].pcb) {
            if (!_mdns_alloc_answer(&packet->servers, MDNS_TYPE_A, NULL, false, false)) {
                _mdns_free_tx_packet(packet);
                return NULL;
            }
        }

        if (_mdns_server->interfaces[tcpip_if].pcbs[MDNS_IP_PROTOCOL_V6].pcb) {
            if (!_mdns_alloc_answer(&packet->servers, MDNS_TYPE_AAAA, NULL, false, false)) {
                _mdns_free_tx_packet(packet);
                return NULL;
            }
        }
    }

    return packet;
}

/**
 * @brief  Create announce packet for particular services on particular PCB
 */
static mdns_tx_packet_t * _mdns_create_announce_packet(tcpip_adapter_if_t tcpip_if, mdns_ip_protocol_t ip_protocol, mdns_srv_item_t * services[], size_t len, bool include_ip)
{
    mdns_tx_packet_t * packet = _mdns_alloc_packet_default(tcpip_if, ip_protocol);
    if (!packet) {
        return NULL;
    }
    packet->flags = MDNS_FLAGS_AUTHORITATIVE;

    uint8_t i;
    for (i=0; i<len; i++) {
        if (!_mdns_alloc_answer(&packet->answers, MDNS_TYPE_SDPTR, services[i]->service, false, false)
                || !_mdns_alloc_answer(&packet->answers, MDNS_TYPE_PTR, services[i]->service, false, false)
                || !_mdns_alloc_answer(&packet->answers, MDNS_TYPE_SRV, services[i]->service, true, false)
                || !_mdns_alloc_answer(&packet->answers, MDNS_TYPE_TXT, services[i]->service, true, false)) {
            _mdns_free_tx_packet(packet);
            return NULL;
        }
    }
    if (include_ip) {
        if (!_mdns_alloc_answer(&packet->answers, MDNS_TYPE_A, NULL, true, false)
                || !_mdns_alloc_answer(&packet->answers, MDNS_TYPE_AAAA, NULL, true, false)) {
            _mdns_free_tx_packet(packet);
            return NULL;
        }
    }
    return packet;
}

/**
 * @brief  Convert probe packet to announce
 */
static mdns_tx_packet_t * _mdns_create_announce_from_probe(mdns_tx_packet_t * probe)
{

    mdns_tx_packet_t * packet = _mdns_alloc_packet_default(probe->tcpip_if, probe->ip_protocol);
    if (!packet) {
        return NULL;
    }
    packet->flags = MDNS_FLAGS_AUTHORITATIVE;

    mdns_out_answer_t * s = probe->servers;
    while (s) {
        if (s->type == MDNS_TYPE_SRV) {
            if (!_mdns_alloc_answer(&packet->answers, MDNS_TYPE_SDPTR, s->service, false, false)
                    || !_mdns_alloc_answer(&packet->answers, MDNS_TYPE_PTR, s->service, false, false)
                    || !_mdns_alloc_answer(&packet->answers, MDNS_TYPE_SRV, s->service, true, false)
                    || !_mdns_alloc_answer(&packet->answers, MDNS_TYPE_TXT, s->service, true, false)) {
                _mdns_free_tx_packet(packet);
                return NULL;
            }

        } else if (s->type == MDNS_TYPE_A || s->type == MDNS_TYPE_AAAA) {
            if (!_mdns_alloc_answer(&packet->answers, s->type, NULL, true, false)) {
                _mdns_free_tx_packet(packet);
                return NULL;
            }
        }

        s = s->next;
    }
    return packet;
}

/**
 * @brief  Send by for particular services on particular PCB
 */
static void _mdns_pcb_send_bye(tcpip_adapter_if_t tcpip_if, mdns_ip_protocol_t ip_protocol, mdns_srv_item_t ** services, size_t len, bool include_ip)
{
    mdns_tx_packet_t * packet = _mdns_alloc_packet_default(tcpip_if, ip_protocol);
    if (!packet) {
        return;
    }
    size_t i;
    for (i=0; i<len; i++) {
        if (!_mdns_alloc_answer(&packet->answers, MDNS_TYPE_PTR, services[i]->service, true, true)) {
            _mdns_free_tx_packet(packet);
            return;
        }
    }
    if (include_ip && (!_mdns_alloc_answer(&packet->answers, MDNS_TYPE_A, NULL, true, true) || !_mdns_alloc_answer(&packet->answers, MDNS_TYPE_AAAA, NULL, true, true))) {
        _mdns_free_tx_packet(packet);
        return;
    }
    _mdns_dispatch_tx_packet(packet);
    _mdns_free_tx_packet(packet);
}

/**
 * @brief  Send probe for particular services on particular PCB
 */
static void _mdns_init_pcb_probe(tcpip_adapter_if_t tcpip_if, mdns_ip_protocol_t ip_protocol, mdns_srv_item_t ** services, size_t len, bool probe_ip)
{
    mdns_pcb_t * pcb = &_mdns_server->interfaces[tcpip_if].pcbs[ip_protocol];
    size_t services_final_len = len;

    _mdns_clear_pcb_tx_queue_head(tcpip_if, ip_protocol);

    if (!_mdns_server->hostname || !_mdns_server->hostname[0]) {
        pcb->state = PCB_RUNNING;
        return;
    }

    if (PCB_STATE_IS_PROBING(pcb)) {
        services_final_len += pcb->probe_services_len;
    }
    mdns_srv_item_t ** _services = NULL;
    if (services_final_len) {
        _services = (mdns_srv_item_t **)malloc(sizeof(mdns_srv_item_t *) * services_final_len);
        if (!_services) {
            return;
        }

        size_t i;
        for (i=0; i<len; i++) {
            _services[i] = services[i];
        }
        if (pcb->probe_services) {
            for (i=0; i<pcb->probe_services_len; i++) {
                _services[len+i] = pcb->probe_services[i];
            }
            free(pcb->probe_services);
        }
    }

    probe_ip = pcb->probe_ip || probe_ip;

    pcb->probe_ip = false;
    pcb->probe_services = NULL;
    pcb->probe_services_len = 0;
    pcb->probe_running = false;

    mdns_tx_packet_t * packet = _mdns_create_probe_packet(tcpip_if, ip_protocol, _services, services_final_len, true, probe_ip);
    if (!packet) {
        free(_services);
        return;
    }

    pcb->probe_ip = probe_ip;
    pcb->probe_services = _services;
    pcb->probe_services_len = services_final_len;
    pcb->probe_running = true;
    _mdns_schedule_tx_packet(packet, ((pcb->failed_probes > 5)?1000:120) + (esp_random() & 0x7F));
    pcb->state = PCB_PROBE_1;
}

/**
 * @brief  Restart the responder on particular PCB
 */
static void _mdns_restart_pcb(tcpip_adapter_if_t tcpip_if, mdns_ip_protocol_t ip_protocol)
{
    size_t srv_count = 0;
    mdns_srv_item_t * a = _mdns_server->services;
    while (a) {
        srv_count++;
        a = a->next;
    }
    mdns_srv_item_t * services[srv_count];
    size_t i = 0;
    a = _mdns_server->services;
    while (a) {
        services[i++] = a;
        a = a->next;
    }
    _mdns_init_pcb_probe(tcpip_if, ip_protocol, services, srv_count, true);
}

/**
 * @brief  Send by for particular services
 */
static void _mdns_send_bye(mdns_srv_item_t ** services, size_t len, bool include_ip)
{
    uint8_t i, j;
    if (!_mdns_server->hostname || !_mdns_server->hostname[0]) {
        return;
    }

    for (i=0; i<TCPIP_ADAPTER_IF_MAX; i++) {
        for (j=0; j<MDNS_IP_PROTOCOL_MAX; j++) {
            if (_mdns_server->interfaces[i].pcbs[j].pcb && _mdns_server->interfaces[i].pcbs[j].state == PCB_RUNNING) {
                _mdns_pcb_send_bye((tcpip_adapter_if_t)i, (mdns_ip_protocol_t)j, services, len, include_ip);
            }
        }
    }
}

/**
 * @brief  Send announcement on particular PCB
 */
static void _mdns_announce_pcb(tcpip_adapter_if_t tcpip_if, mdns_ip_protocol_t ip_protocol, mdns_srv_item_t ** services, size_t len, bool include_ip)
{
    mdns_pcb_t * _pcb = &_mdns_server->interfaces[tcpip_if].pcbs[ip_protocol];
    size_t i;
    if (_pcb->pcb) {
        if (PCB_STATE_IS_PROBING(_pcb)) {
            _mdns_init_pcb_probe(tcpip_if, ip_protocol, services, len, include_ip);
        } else if (PCB_STATE_IS_ANNOUNCING(_pcb)) {
            mdns_tx_packet_t *  p = _mdns_get_next_pcb_packet(tcpip_if, ip_protocol);
            if (p) {
                for (i=0; i<len; i++) {
                    if (!_mdns_alloc_answer(&p->answers, MDNS_TYPE_SDPTR, services[i]->service, false, false)
                            || !_mdns_alloc_answer(&p->answers, MDNS_TYPE_PTR, services[i]->service, false, false)
                            || !_mdns_alloc_answer(&p->answers, MDNS_TYPE_SRV, services[i]->service, true, false)
                            || !_mdns_alloc_answer(&p->answers, MDNS_TYPE_TXT, services[i]->service, true, false)) {
                        break;
                    }
                }
                if (include_ip) {
                    _mdns_dealloc_answer(&p->additional, MDNS_TYPE_A, NULL);
                    _mdns_dealloc_answer(&p->additional, MDNS_TYPE_AAAA, NULL);
                    _mdns_alloc_answer(&p->answers, MDNS_TYPE_A, NULL, true, false);
                    _mdns_alloc_answer(&p->answers, MDNS_TYPE_AAAA, NULL, true, false);
                }
                _pcb->state = PCB_ANNOUNCE_1;
            }
        } else if (_pcb->state == PCB_RUNNING) {

            if (!_mdns_server->hostname || !_mdns_server->hostname[0]) {
                return;
            }

            _pcb->state = PCB_ANNOUNCE_1;
            mdns_tx_packet_t *  p = _mdns_create_announce_packet(tcpip_if, ip_protocol, services, len, include_ip);
            if (p) {
                _mdns_schedule_tx_packet(p, 0);
            }
        }
    }
}

/**
 * @brief  Send probe on all active PCBs
 */
static void _mdns_probe_all_pcbs(mdns_srv_item_t ** services, size_t len, bool probe_ip, bool clear_old_probe)
{
    uint8_t i, j;
    for (i=0; i<TCPIP_ADAPTER_IF_MAX; i++) {
        for (j=0; j<MDNS_IP_PROTOCOL_MAX; j++) {
            if (_mdns_server->interfaces[i].pcbs[j].pcb) {
                mdns_pcb_t * _pcb = &_mdns_server->interfaces[i].pcbs[j];
                if (clear_old_probe) {
                    free(_pcb->probe_services);
                    _pcb->probe_services = NULL;
                    _pcb->probe_services_len = 0;
                    _pcb->probe_running = false;
                }
                _mdns_init_pcb_probe((tcpip_adapter_if_t)i, (mdns_ip_protocol_t)j, services, len, probe_ip);
            }
        }
    }
}

/**
 * @brief  Send announcement on all active PCBs
 */
static void _mdns_announce_all_pcbs(mdns_srv_item_t ** services, size_t len, bool include_ip)
{
    uint8_t i, j;
    for (i=0; i<TCPIP_ADAPTER_IF_MAX; i++) {
        for (j=0; j<MDNS_IP_PROTOCOL_MAX; j++) {
            _mdns_announce_pcb((tcpip_adapter_if_t)i, (mdns_ip_protocol_t)j, services, len, include_ip);
        }
    }
}

/**
 * @brief  Restart the responder on all active PCBs
 */
static void _mdns_send_final_bye(bool include_ip)
{
    //collect all services and start probe
    size_t srv_count = 0;
    mdns_srv_item_t * a = _mdns_server->services;
    while (a) {
        if (!a->service->instance) {
            srv_count++;
        }
        a = a->next;
    }
    if (!srv_count) {
        return;
    }
    mdns_srv_item_t * services[srv_count];
    size_t i = 0;
    a = _mdns_server->services;
    while (a) {
        if (!a->service->instance) {
            services[i++] = a;
        }
        a = a->next;
    }
    _mdns_send_bye(services, srv_count, include_ip);
}

/**
 * @brief  Stop the responder on all services without instance
 */
static void _mdns_send_bye_all_pcbs_no_instance()
{
    size_t srv_count = 0;
    mdns_srv_item_t * a = _mdns_server->services;
    while (a) {
        if (!a->service->instance) {
            srv_count++;
        }
        a = a->next;
    }
    if (!srv_count) {
        return;
    }
    mdns_srv_item_t * services[srv_count];
    size_t i = 0;
    a = _mdns_server->services;
    while (a) {
        if (!a->service->instance) {
            services[i++] = a;
        }
        a = a->next;
    }
    _mdns_send_bye(services, srv_count, false);
}

/**
 * @brief  Restart the responder on all services without instance
 */
static void _mdns_restart_all_pcbs_no_instance()
{
    size_t srv_count = 0;
    mdns_srv_item_t * a = _mdns_server->services;
    while (a) {
        if (!a->service->instance) {
            srv_count++;
        }
        a = a->next;
    }
    if (!srv_count) {
        return;
    }
    mdns_srv_item_t * services[srv_count];
    size_t i = 0;
    a = _mdns_server->services;
    while (a) {
        if (!a->service->instance) {
            services[i++] = a;
        }
        a = a->next;
    }
    _mdns_probe_all_pcbs(services, srv_count, false, true);
}

/**
 * @brief  Restart the responder on all active PCBs
 */
static void _mdns_restart_all_pcbs()
{
    _mdns_clear_tx_queue_head();
    size_t srv_count = 0;
    mdns_srv_item_t * a = _mdns_server->services;
    while (a) {
        srv_count++;
        a = a->next;
    }
    mdns_srv_item_t * services[srv_count];
    size_t l = 0;
    a = _mdns_server->services;
    while (a) {
        services[l++] = a;
        a = a->next;
    }

    _mdns_probe_all_pcbs(services, srv_count, true, true);
}



/**
 * @brief  creates/allocates new text item list
 * @param  num_items     service number of txt items or 0
 * @param  txt           service txt items array or NULL
 *
 * @return pointer to the linked txt item list or NULL
 */
static mdns_txt_linked_item_t * _mdns_allocate_txt(size_t num_items, mdns_txt_item_t txt[])
{
    mdns_txt_linked_item_t * new_txt = NULL;
    size_t i = 0;
    if (num_items) {
        for (i=0; i<num_items; i++) {
            mdns_txt_linked_item_t * new_item = (mdns_txt_linked_item_t *)malloc(sizeof(mdns_txt_linked_item_t));
            if (!new_item) {
                break;
            }
            new_item->key = strdup(txt[i].key);
            if (!new_item->key) {
                free(new_item);
                break;
            }
            new_item->value = strdup(txt[i].value);
            if (!new_item->value) {
                free((char *)new_item->key);
                free(new_item);
                break;
            }
            new_item->next = new_txt;
            new_txt = new_item;
        }
    }
    return new_txt;
}

/**
 * @brief  creates/allocates new service
 * @param  service       service type
 * @param  proto         service proto
 * @param  port          service port
 * @param  instance      service instance
 * @param  num_items     service number of txt items or 0
 * @param  txt           service txt items array or NULL
 *
 * @return pointer to the service or NULL on error
 */
static mdns_service_t * _mdns_create_service(const char * service, const char * proto, uint16_t port, const char * instance, size_t num_items, mdns_txt_item_t txt[])
{
    mdns_service_t * s = (mdns_service_t *)malloc(sizeof(mdns_service_t));
    if (!s) {
        return NULL;
    }

    mdns_txt_linked_item_t * new_txt = _mdns_allocate_txt(num_items, txt);
    if (num_items && new_txt == NULL) {
        free(s);
        return NULL;
    }

    s->priority = 0;
    s->weight = 0;
    s->instance = instance?strndup(instance, MDNS_NAME_BUF_LEN - 1):NULL;
    s->txt = new_txt;
    s->port = port;

    s->service = strndup(service, MDNS_NAME_BUF_LEN - 1);
    if (!s->service) {
        free(s);
        return NULL;
    }

    s->proto = strndup(proto, MDNS_NAME_BUF_LEN - 1);
    if (!s->proto) {
        free((char *)s->service);
        free(s);
        return NULL;
    }

    return s;
}

/**
 * @brief  free service memory
 *
 * @param  service      the service
 */
static void _mdns_free_service(mdns_service_t * service)
{
    if (!service) {
        return;
    }
    free((char *)service->instance);
    free((char *)service->service);
    free((char *)service->proto);
    while (service->txt) {
        mdns_txt_linked_item_t * s = service->txt;
        service->txt = service->txt->next;
        free((char *)s->key);
        free((char *)s->value);
        free(s);
    }
    free(service->txt);
    free(service);
}


/*
 * Received Packet Handling
 * */

/**
 * @brief  Detect SRV collision
 */
static int _mdns_check_srv_collision(mdns_service_t * service, uint16_t priority, uint16_t weight, uint16_t port, const char * host, const char * domain)
{
    size_t our_host_len = strlen(_mdns_server->hostname);
    size_t our_len = 14 + our_host_len;

    size_t their_host_len = strlen(host);
    size_t their_domain_len = strlen(domain);
    size_t their_len = 9 + their_host_len + their_domain_len;

    if (their_len > our_len) {
        return 1;//they win
    } else if (their_len < our_len) {
        return -1;//we win
    }

    uint16_t our_index = 0;
    uint8_t our_data[our_len];
    _mdns_append_u16(our_data, &our_index, service->priority);
    _mdns_append_u16(our_data, &our_index, service->weight);
    _mdns_append_u16(our_data, &our_index, service->port);
    our_data[our_index++] = our_host_len;
    memcpy(our_data + our_index, _mdns_server->hostname, our_host_len);
    our_index += our_host_len;
    our_data[our_index++] = 5;
    memcpy(our_data + our_index, MDNS_DEFAULT_DOMAIN, 5);
    our_index += 5;
    our_data[our_index++] = 0;

    uint16_t their_index = 0;
    uint8_t their_data[their_len];
    _mdns_append_u16(their_data, &their_index, priority);
    _mdns_append_u16(their_data, &their_index, weight);
    _mdns_append_u16(their_data, &their_index, port);
    their_data[their_index++] = their_host_len;
    memcpy(their_data + their_index, host, their_host_len);
    their_index += their_host_len;
    their_data[their_index++] = their_domain_len;
    memcpy(their_data + their_index, domain, their_domain_len);
    their_index += their_domain_len;
    their_data[their_index++] = 0;

    int ret = memcmp(our_data, their_data, our_len);
    if (ret > 0) {
        return -1;//we win
    } else if (ret < 0) {
        return 1;//they win
    }
    return 0;//same
}

/**
 * @brief  Detect TXT collision
 */
static int _mdns_check_txt_collision(mdns_service_t * service, const uint8_t * data, size_t len)
{
    size_t data_len = 1;
    if (len == 1 && service->txt) {
        return -1;//we win
    } else if (len > 1 && !service->txt) {
        return 1;//they win
    } else if (len == 1 && !service->txt) {
        return 0;//same
    }

    mdns_txt_linked_item_t * txt = service->txt;
    while (txt) {
        data_len += 2 + strlen(service->txt->key) + strlen(service->txt->value);
        txt = txt->next;
    }

    if (len > data_len) {
        return 1;//they win
    } else if (len < data_len) {
        return -1;//we win
    }

    uint8_t ours[len];
    uint16_t index = 0;
    char * tmp;

    txt = service->txt;
    while (txt) {
        tmp = (char *)malloc(2 + strlen(txt->key) + strlen(txt->value));
        if (tmp) {
            sprintf(tmp, "%s=%s", txt->key, txt->value);
            _mdns_append_string(ours, &index, tmp);
            free(tmp);
        }
        txt = txt->next;
    }

    int ret = memcmp(ours, data, len);
    if (ret > 0) {
        return -1;//we win
    } else if (ret < 0) {
        return 1;//they win
    }
    return 0;//same
}

/**
 * @brief  Set interface as duplicate if another is found on the same subnet
 */
static void _mdns_dup_interface(tcpip_adapter_if_t tcpip_if)
{
    uint8_t i;
    tcpip_adapter_if_t other_if = _mdns_get_other_if (tcpip_if);
    for (i=0; i<MDNS_IP_PROTOCOL_MAX; i++) {
        if (_mdns_server->interfaces[other_if].pcbs[i].pcb) {
            //stop this interface and mark as dup
            if (_mdns_server->interfaces[tcpip_if].pcbs[i].pcb) {
                _mdns_clear_pcb_tx_queue_head(tcpip_if, i);
                _mdns_pcb_deinit(tcpip_if, i);
            }
            _mdns_server->interfaces[tcpip_if].pcbs[i].state = PCB_DUP;
            _mdns_announce_pcb(other_if, i, NULL, 0, true);
        }
    }
}

/**
 * @brief  Detect IPv4 address collision
 */
static int _mdns_check_a_collision(ip4_addr_t * ip, tcpip_adapter_if_t tcpip_if)
{
    tcpip_adapter_ip_info_t if_ip_info;
    tcpip_adapter_ip_info_t other_ip_info;
    if (!ip->addr) {
        return 1;//denial! they win
    }
    if (tcpip_adapter_get_ip_info(tcpip_if, &if_ip_info)) {
        return 1;//they win
    }

    int ret = memcmp((uint8_t*)&if_ip_info.ip.addr, (uint8_t*)&ip->addr, sizeof(ip4_addr_t));
    if (ret > 0) {
        return -1;//we win
    } else if (ret < 0) {
        //is it the other interface?
        tcpip_adapter_if_t other_if = _mdns_get_other_if (tcpip_if);
        if (other_if == TCPIP_ADAPTER_IF_MAX) {
            return 1;//AP interface! They win
        }
        if (tcpip_adapter_get_ip_info(other_if, &other_ip_info)) {
            return 1;//IPv4 not active! They win
        }
        if (ip->addr != other_ip_info.ip.addr) {
            return 1;//IPv4 not ours! They win
        }
        _mdns_dup_interface(tcpip_if);
        return 2;//they win
    }
    return 0;//same
}

/**
 * @brief  Detect IPv6 address collision
 */
static int _mdns_check_aaaa_collision(ip6_addr_t * ip, tcpip_adapter_if_t tcpip_if)
{
    struct ip6_addr if_ip6;
    struct ip6_addr other_ip6;
    if (_ipv6_address_is_zero(*ip)) {
        return 1;//denial! they win
    }
    if (tcpip_adapter_get_ip6_linklocal(tcpip_if, &if_ip6)) {
        return 1;//they win
    }
    int ret = memcmp((uint8_t*)&if_ip6.addr, (uint8_t*)ip->addr, sizeof(ip6_addr_t));
    if (ret > 0) {
        return -1;//we win
    } else if (ret < 0) {
        //is it the other interface?
        tcpip_adapter_if_t other_if = _mdns_get_other_if (tcpip_if);
        if (other_if == TCPIP_ADAPTER_IF_MAX) {
            return 1;//AP interface! They win
        }
        if (tcpip_adapter_get_ip6_linklocal(other_if, &other_ip6)) {
            return 1;//IPv6 not active! They win
        }
        if (memcmp((uint8_t*)&other_ip6.addr, (uint8_t*)ip->addr, sizeof(ip6_addr_t))) {
            return 1;//IPv6 not ours! They win
        }
        _mdns_dup_interface(tcpip_if);
        return 2;//they win
    }
    return 0;//same
}

/**
 * @brief  Check if parsed name is discovery
 */
static bool _mdns_name_is_discovery(mdns_name_t * name, uint16_t type)
{
    return (
               (name->host && name->host[0] && !strcasecmp(name->host, "_services"))
               && (name->service && name->service[0] && !strcasecmp(name->service, "_dns-sd"))
               && (name->proto && name->proto[0] && !strcasecmp(name->proto, "_udp"))
               && (name->domain && name->domain[0] && !strcasecmp(name->domain, MDNS_DEFAULT_DOMAIN))
               && type == MDNS_TYPE_PTR
           );
}

/**
 * @brief  Check if the parsed name is ours (matches service or host name)
 */
static bool _mdns_name_is_ours(mdns_name_t * name)
{
    //domain have to be "local"
    if (!name->domain || !name->domain[0] || strcasecmp(name->domain, MDNS_DEFAULT_DOMAIN)) {
        return false;
    }

    //if service and proto are empty, host must match out hostname
    if ((!name->service || !name->service[0]) && (!name->proto || !name->proto[0])) {
        if (name->host && name->host[0] && strcasecmp(name->host, _mdns_server->hostname) == 0) {
            return true;
        }
        return false;
    }

    //if service or proto is empty, name is invalid
    if ((!name->service || !name->service[0]) || (!name->proto || !name->proto[0])) {
        return false;
    }

    //find the service
    mdns_srv_item_t * service = _mdns_get_service_item(name->service, name->proto);
    if (!service) {
        return false;
    }

    //if host is empty and we have service, we have success
    if (!name->host || !name->host[0]) {
        return true;
    }

    //OK we have host in the name. find what is the instance of the service
    const char * instance = service->service->instance;
    if (instance == NULL) {
        if (_mdns_server->instance && _mdns_server->instance[0]) {
            instance = _mdns_server->instance;
        } else if (_mdns_server->hostname && _mdns_server->hostname[0]) {
            instance = _mdns_server->hostname;
        } else {
            return false;
        }
    }

    //compare the instance against the name
    if (strcasecmp(name->host, instance) == 0) {
        return true;
    }

    return false;
}

/**
 * @brief  read uint16_t from a packet
 * @param  packet       the packet
 * @param  index        index in the packet where the value starts
 *
 * @return the value
 */
static inline uint16_t _mdns_read_u16(const uint8_t * packet, uint16_t index)
{
    return (uint16_t)(packet[index]) << 8 | packet[index+1];
}

/**
 * @brief  read uint32_t from a packet
 * @param  packet       the packet
 * @param  index        index in the packet where the value starts
 *
 * @return the value
 */
static inline uint32_t _mdns_read_u32(const uint8_t * packet, uint16_t index)
{
    return (uint32_t)(packet[index]) << 24 | (uint32_t)(packet[index+1]) << 16 | (uint32_t)(packet[index+2]) << 8 | packet[index+3];
}

/**
 * @brief  reads and formats MDNS FQDN into mdns_name_t structure
 *
 * @param  packet       MDNS packet
 * @param  start        Starting point of FQDN
 * @param  name         mdns_name_t structure to populate
 *
 * @return the address after the parsed FQDN in the packet or NULL on error
 */
static const uint8_t * _mdns_parse_fqdn(const uint8_t * packet, const uint8_t * start, mdns_name_t * name)
{
    name->parts = 0;
    name->sub = 0;
    name->host[0] = 0;
    name->service[0] = 0;
    name->proto[0] = 0;
    name->domain[0] = 0;

    static char buf[MDNS_NAME_BUF_LEN];

    const uint8_t * next_data = (uint8_t*)_mdns_read_fqdn(packet, start, name, buf);
    if (!next_data) {
        return 0;
    }
    if (!name->parts) {
        return next_data;
    }
    if (name->parts == 3) {
        memmove((uint8_t*)name + (MDNS_NAME_BUF_LEN), (uint8_t*)name, 3*(MDNS_NAME_BUF_LEN));
        name->host[0] = 0;
    } else if (name->parts == 2) {
        memmove((uint8_t*)(name->domain), (uint8_t*)(name->service), (MDNS_NAME_BUF_LEN));
        name->service[0] = 0;
        name->proto[0] = 0;
    }
    if (strcasecmp(name->domain, MDNS_DEFAULT_DOMAIN) == 0 || strcasecmp(name->domain, "arpa") == 0) {
        return next_data;
    }
    return 0;
}

/**
 * @brief  Called from parser to check if question matches particular service
 */
static bool _mdns_question_matches(mdns_parsed_question_t * question, uint16_t type, mdns_srv_item_t * service)
{
    if (question->type != type) {
        return false;
    }
    if (type == MDNS_TYPE_A || type == MDNS_TYPE_AAAA) {
        return true;
    } else if (type == MDNS_TYPE_PTR || type == MDNS_TYPE_SDPTR) {
        if (!strcasecmp(service->service->service, question->service)
            && !strcasecmp(service->service->proto, question->proto)
            && !strcasecmp(MDNS_DEFAULT_DOMAIN, question->domain)) {
            return true;
        }
    } else if (type == MDNS_TYPE_SRV || type == MDNS_TYPE_TXT) {
        const char * name = service->service->instance;
        if (!name) {
            name = _mdns_server->instance;
        }
        if (!name) {
            name = _mdns_server->hostname;
        }
        if (!strcasecmp(name, question->host)
            && !strcasecmp(service->service->service, question->service)
            && !strcasecmp(service->service->proto, question->proto)
            && !strcasecmp(MDNS_DEFAULT_DOMAIN, question->domain)) {
            return true;
        }
    }

    return false;
}

/**
 * @brief  Removes saved question from parsed data
 */
static void _mdns_remove_parsed_question(mdns_parsed_packet_t * parsed_packet, uint16_t type, mdns_srv_item_t * service)
{
    mdns_parsed_question_t * q = parsed_packet->questions;

    if (_mdns_question_matches(q, type, service)) {
        parsed_packet->questions = q->next;
        free(q->host);
        free(q->service);
        free(q->proto);
        free(q->domain);
        free(q);
        return;
    }

    while (q->next) {
        mdns_parsed_question_t * p = q->next;
        if (_mdns_question_matches(p, type, service)) {
            q->next = p->next;
            free(p->host);
            free(p->service);
            free(p->proto);
            free(p->domain);
            free(p);
            return;
        }
        q = q->next;
    }
}

/**
 * @brief  Get number of items in TXT parsed data
 */
static int _mdns_txt_items_count_get(const uint8_t * data, size_t len)
{
    if (len == 1) {
        return 0;
    }

    int num_items = 0;
    uint16_t i=0;
    size_t partLen = 0;

    while (i < len) {
        partLen = data[i++];
        if (!partLen) {
            break;
        }
        if ((i+partLen) > len) {
            return -1;//error
        }
        i+=partLen;
        num_items++;
    }
    return num_items;
}

/**
 * @brief  Get the length of TXT item's key name
 */
static int _mdns_txt_item_name_get_len(const uint8_t * data, size_t len)
{
    int i;
    if (*data == '=') {
        return -1;
    }
    for (i = 0; i < len; i++) {
        if (data[i] == '=') {
            return i;
        }
    }
    return len;
}

/**
 * @brief  Create TXT result array from parsed TXT data
 */
static void _mdns_result_txt_create(const uint8_t * data, size_t len, mdns_txt_item_t ** out_txt, size_t * out_count)
{
    *out_txt = NULL;
    *out_count = 0;
    uint16_t i=0, y;
    size_t partLen = 0;
    int num_items = _mdns_txt_items_count_get(data, len);
    if (num_items < 0) {
        return;//error
    }

    if (!num_items) {
        return;
    }

    mdns_txt_item_t * txt = (mdns_txt_item_t *)malloc(sizeof(mdns_txt_item_t) * num_items);
    memset(txt, 0, sizeof(mdns_txt_item_t) * num_items);
    size_t txt_num = 0;

    while (i < len) {
        partLen = data[i++];
        if (!partLen) {
            break;
        }

        if ((i+partLen) > len) {
            goto handle_error;//error
        }

        int name_len = _mdns_txt_item_name_get_len(data+i, partLen);
        if (name_len < 0) {//invalid item (no name)
            i += partLen;
            continue;
        }
        char * key = (char *)malloc(name_len + 1);
        if (!key) {
            goto handle_error;//error
        }

        mdns_txt_item_t * t = &txt[txt_num++];

        memcpy(key, data + i, name_len);
        key[name_len] = 0;
        i += name_len + 1;
        t->key = key;

        int value_len = partLen - name_len - 1;
        if (value_len > 0) {
            char * value = (char *)malloc(value_len + 1);
            memcpy(value, data + i, value_len);
            value[value_len] = 0;
            i += value_len;
            t->value = value;
        }
    }

    *out_txt = txt;
    *out_count = txt_num;
    return;

handle_error :
    for (y=0; y<txt_num; y++) {
        mdns_txt_item_t * t = &txt[y];
        free((char *)t->key);
        free((char *)t->value);
    }
    free(txt);
}

/**
 * @brief  Duplicate string or return error
 */
static esp_err_t _mdns_strdup_check(char ** out, char * in)
{
    if (in && in[0]) {
        *out = strdup(in);
        if (!*out) {
            return ESP_FAIL;
        }
        return ESP_OK;
    }
    *out = NULL;
    return ESP_OK;
}

/**
 * @brief  main packet parser
 *
 * @param  packet       the packet
 */
void mdns_parse_packet(mdns_rx_packet_t * packet)
{
    static mdns_name_t n;
    mdns_header_t header;
    const uint8_t * data = (const uint8_t*)packet->pb->payload;
    size_t len = packet->pb->len;
    const uint8_t * content = data + MDNS_HEAD_LEN;
    bool do_not_reply = false;
    mdns_search_once_t * search_result = NULL;

#ifdef MDNS_ENABLE_DEBUG
    _mdns_dbg_printf("\nRX[%u][%u]: ", packet->tcpip_if, (uint32_t)packet->ip_protocol);
    if (packet->src.type == IPADDR_TYPE_V4) {
        _mdns_dbg_printf("From: " IPSTR ":%u, To: " IPSTR ", ", IP2STR(&packet->src.u_addr.ip4), packet->src_port, IP2STR(&packet->dest.u_addr.ip4));
    } else {
        _mdns_dbg_printf("From: " IPV6STR ":%u, To: " IPV6STR ", ", IPV62STR(packet->src.u_addr.ip6), packet->src_port, IPV62STR(packet->dest.u_addr.ip6));
    }
    mdns_debug_packet(data, len);
#endif

    mdns_parsed_packet_t * parsed_packet = (mdns_parsed_packet_t *)malloc(sizeof(mdns_parsed_packet_t));
    if (!parsed_packet) {
        return;
    }
    memset(parsed_packet, 0, sizeof(mdns_parsed_packet_t));

    mdns_name_t * name = &n;
    memset(name, 0, sizeof(mdns_name_t));

    header.id = _mdns_read_u16(data, MDNS_HEAD_ID_OFFSET);
    header.flags.value = _mdns_read_u16(data, MDNS_HEAD_FLAGS_OFFSET);
    header.questions = _mdns_read_u16(data, MDNS_HEAD_QUESTIONS_OFFSET);
    header.answers = _mdns_read_u16(data, MDNS_HEAD_ANSWERS_OFFSET);
    header.servers = _mdns_read_u16(data, MDNS_HEAD_SERVERS_OFFSET);
    header.additional = _mdns_read_u16(data, MDNS_HEAD_ADDITIONAL_OFFSET);

    if (header.flags.value == MDNS_FLAGS_AUTHORITATIVE && packet->src_port != MDNS_SERVICE_PORT) {
        free(parsed_packet);
        return;
    }

    parsed_packet->tcpip_if = packet->tcpip_if;
    parsed_packet->ip_protocol = packet->ip_protocol;
    parsed_packet->multicast = packet->multicast;
    parsed_packet->authoritative = header.flags.value == MDNS_FLAGS_AUTHORITATIVE;
    parsed_packet->distributed = header.flags.value == MDNS_FLAGS_DISTRIBUTED;
    ip_addr_copy(parsed_packet->src, packet->src);
    parsed_packet->src_port = packet->src_port;

    if (header.questions) {
        uint8_t qs = header.questions;

        while (qs--) {
            content = _mdns_parse_fqdn(data, content, name);
            if (!content) {
                header.answers = 0;
                header.additional = 0;
                header.servers = 0;
                goto clear_rx_packet;//error
            }

            uint16_t type = _mdns_read_u16(content, MDNS_TYPE_OFFSET);
            uint16_t clas = _mdns_read_u16(content, MDNS_CLASS_OFFSET);
            bool unicast = !!(clas & 0x8000);
            clas &= 0x7FFF;
            content = content + 4;

            if (clas != 0x0001) {//bad class
                continue;
            }

            if (_mdns_name_is_discovery(name, type)) {
                //service discovery
                parsed_packet->discovery = true;
                mdns_srv_item_t * a = _mdns_server->services;
                while (a) {
                    mdns_parsed_question_t * question = (mdns_parsed_question_t *)malloc(sizeof(mdns_parsed_question_t));
                    if (!question) {
                        goto clear_rx_packet;
                    }
                    question->next = parsed_packet->questions;
                    parsed_packet->questions = question;

                    question->unicast = unicast;
                    question->type = MDNS_TYPE_SDPTR;
                    question->host = NULL;
                    question->service = strdup(a->service->service);
                    question->proto = strdup(a->service->proto);
                    question->domain = strdup(MDNS_DEFAULT_DOMAIN);
                    if (!question->service || !question->proto || !question->domain) {
                        goto clear_rx_packet;
                    }
                    a = a->next;
                }
                continue;
            } else if (name->sub || !_mdns_name_is_ours(name)) {
                continue;
            }

            if (type == MDNS_TYPE_ANY) {
                parsed_packet->probe = true;
            }

            mdns_parsed_question_t * question = (mdns_parsed_question_t *)malloc(sizeof(mdns_parsed_question_t));
            if (!question) {
                goto clear_rx_packet;
            }
            question->next = parsed_packet->questions;
            parsed_packet->questions = question;

            question->unicast = unicast;
            question->type = type;
            if (_mdns_strdup_check(&(question->host), name->host)
              || _mdns_strdup_check(&(question->service), name->service)
              || _mdns_strdup_check(&(question->proto), name->proto)
              || _mdns_strdup_check(&(question->domain), name->domain)) {
                goto clear_rx_packet;
            }
        }
    }

    if (header.questions && !parsed_packet->questions && !parsed_packet->discovery) {
        goto clear_rx_packet;
    } else if (header.answers || header.servers || header.additional) {
        uint16_t recordIndex = 0;

        while (content < (data + len)) {

            content = _mdns_parse_fqdn(data, content, name);
            if (!content) {
                goto clear_rx_packet;//error
            }

            uint16_t type = _mdns_read_u16(content, MDNS_TYPE_OFFSET);
            uint16_t clas = _mdns_read_u16(content, MDNS_CLASS_OFFSET);
            uint32_t ttl = _mdns_read_u32(content, MDNS_TTL_OFFSET);
            uint16_t data_len = _mdns_read_u16(content, MDNS_LEN_OFFSET);
            const uint8_t * data_ptr = content + MDNS_DATA_OFFSET;
            clas &= 0x7FFF;

            content = data_ptr + data_len;
            if (content > (data + len)) {
                goto clear_rx_packet;
            }

            bool discovery = false;
            bool ours = false;
            mdns_srv_item_t * service = NULL;
            mdns_parsed_recort_type_t record_type = MDNS_ANSWER;

            if (recordIndex >= (header.answers + header.servers)) {
                record_type = MDNS_EXTRA;
            } else if (recordIndex >= (header.answers)) {
                record_type = MDNS_NS;
            }
            recordIndex++;

            if (type == MDNS_TYPE_NSEC || type == MDNS_TYPE_OPT) {
                //skip NSEC and OPT
                continue;
            }

            if (parsed_packet->discovery && _mdns_name_is_discovery(name, type)) {
                discovery = true;
            } else if (!name->sub && _mdns_name_is_ours(name)) {
                ours = true;
                if (name->service && name->service[0] && name->proto && name->proto[0]) {
                    service = _mdns_get_service_item(name->service, name->proto);
                }
            } else {
                if (header.questions || !parsed_packet->authoritative || record_type == MDNS_NS) {
                    //skip this record
                    continue;
                }
                search_result = _mdns_search_find_from(_mdns_server->search_once, name, type, packet->tcpip_if, packet->ip_protocol);
            }

            if (type == MDNS_TYPE_PTR) {
                if (!_mdns_parse_fqdn(data, data_ptr, name)) {
                    continue;//error
                }
                if (search_result) {
                    _mdns_search_result_add_ptr(search_result, name->host, packet->tcpip_if, packet->ip_protocol);
                } else if ((discovery || ours) && !name->sub && _mdns_name_is_ours(name)) {
                    if (discovery) {
                        service = _mdns_get_service_item(name->service, name->proto);
                        _mdns_remove_parsed_question(parsed_packet, MDNS_TYPE_SDPTR, service);
                    } else if (parsed_packet->questions && !parsed_packet->probe) {
                        _mdns_remove_parsed_question(parsed_packet, type, service);
                    } else {
                        //check if TTL is more than half of the full TTL value (4500)
                        if (ttl > 2250) {
                            _mdns_remove_scheduled_answer(packet->tcpip_if, packet->ip_protocol, type, service);
                        }
                    }
                }
            } else if (type == MDNS_TYPE_SRV) {
                mdns_result_t * result = NULL;
                if (search_result && search_result->type == MDNS_TYPE_PTR) {
                    result = search_result->result;
                    while (result) {
                        if (packet->tcpip_if == result->tcpip_if
                            && packet->ip_protocol == result->ip_protocol
                            && result->instance_name && !strcmp(name->host, result->instance_name)) {
                            break;
                        }
                        result = result->next;
                    }
                    if (!result) {
                        result = _mdns_search_result_add_ptr(search_result, name->host, packet->tcpip_if, packet->ip_protocol);
                        if (!result) {
                            continue;//error
                        }
                    }
                }

                if (!_mdns_parse_fqdn(data, data_ptr + MDNS_SRV_FQDN_OFFSET, name)) {
                    continue;//error
                }
                uint16_t priority = _mdns_read_u16(data_ptr, MDNS_SRV_PRIORITY_OFFSET);
                uint16_t weight = _mdns_read_u16(data_ptr, MDNS_SRV_WEIGHT_OFFSET);
                uint16_t port = _mdns_read_u16(data_ptr, MDNS_SRV_PORT_OFFSET);

                if (search_result) {
                    if (search_result->type == MDNS_TYPE_PTR) {
                        result->port = port;
                        result->hostname = strdup(name->host);
                    } else {
                        _mdns_search_result_add_srv(search_result, name->host, port, packet->tcpip_if, packet->ip_protocol);
                    }
                } else if (ours) {
                    if (parsed_packet->questions && !parsed_packet->probe) {
                        _mdns_remove_parsed_question(parsed_packet, type, service);
                        continue;
                    } else if (parsed_packet->distributed) {
                        _mdns_remove_scheduled_answer(packet->tcpip_if, packet->ip_protocol, type, service);
                        continue;
                    }
                    //detect collision (-1=won, 0=none, 1=lost)
                    int col = 0;
                    if (clas > 1) {
                        col = 1;
                    } else if (!clas) {
                        col = -1;
                    } else {
                        col = _mdns_check_srv_collision(service->service, priority, weight, port, name->host, name->domain);
                    }
                    if (col && (parsed_packet->probe || parsed_packet->authoritative)) {
                        if (col > 0 || !port) {
                            do_not_reply = true;
                            if (_mdns_server->interfaces[packet->tcpip_if].pcbs[packet->ip_protocol].probe_running) {
                                _mdns_server->interfaces[packet->tcpip_if].pcbs[packet->ip_protocol].failed_probes++;
                                if (service->service->instance) {
                                    char * new_instance = _mdns_mangle_name((char *)service->service->instance);
                                    if (new_instance) {
                                        free((char *)service->service->instance);
                                        service->service->instance = new_instance;
                                    }
                                    _mdns_probe_all_pcbs(&service, 1, false, false);
                                } else if (_mdns_server->instance) {
                                    char * new_instance = _mdns_mangle_name((char *)_mdns_server->instance);
                                    if (new_instance) {
                                        free((char *)_mdns_server->instance);
                                        _mdns_server->instance = new_instance;
                                    }
                                    _mdns_restart_all_pcbs_no_instance();
                                } else {
                                    char * new_host = _mdns_mangle_name((char *)_mdns_server->hostname);
                                    if (new_host) {
                                        free((char *)_mdns_server->hostname);
                                        _mdns_server->hostname = new_host;
                                    }
                                    _mdns_restart_all_pcbs();
                                }
                            } else {
                                _mdns_pcb_send_bye(packet->tcpip_if, packet->ip_protocol, &service, 1, false);
                                _mdns_init_pcb_probe(packet->tcpip_if, packet->ip_protocol, &service, 1, false);
                            }
                        }
                    } else if (ttl > 60 && !col && !parsed_packet->authoritative && !parsed_packet->probe && !parsed_packet->questions) {
                        _mdns_remove_scheduled_answer(packet->tcpip_if, packet->ip_protocol, type, service);
                    }
                }
            } else if (type == MDNS_TYPE_TXT) {
                if (search_result) {
                    mdns_txt_item_t * txt = NULL;
                    size_t txt_count = 0;
                    _mdns_result_txt_create(data_ptr, data_len, &txt, &txt_count);
                    if (!txt_count) {
                        continue;
                    }

                    mdns_result_t * result = NULL;
                    if (search_result->type == MDNS_TYPE_PTR) {
                        result = search_result->result;
                        while (result) {
                            if (packet->tcpip_if == result->tcpip_if
                                && packet->ip_protocol == result->ip_protocol
                                && result->instance_name && !strcmp(name->host, result->instance_name)) {
                                break;
                            }
                            result = result->next;
                        }
                        if (!result) {
                            result = _mdns_search_result_add_ptr(search_result, name->host, packet->tcpip_if, packet->ip_protocol);
                            if (!result) {
                                continue;//error
                            }
                        }
                        if (!result->txt) {
                            result->txt = txt;
                            result->txt_count = txt_count;
                        }
                    } else {
                        _mdns_search_result_add_txt(search_result, txt, txt_count, packet->tcpip_if, packet->ip_protocol);
                    }
                } else if (ours) {
                    if (parsed_packet->questions && !parsed_packet->probe) {
                        _mdns_remove_parsed_question(parsed_packet, type, service);
                        continue;
                    }
                    //detect collision (-1=won, 0=none, 1=lost)
                    int col = 0;
                    if (clas > 1) {
                        col = 1;
                    } else if (!clas) {
                        col = -1;
                    } else {
                        col = _mdns_check_txt_collision(service->service, data_ptr, data_len);
                    }
                    if (col && !_mdns_server->interfaces[packet->tcpip_if].pcbs[packet->ip_protocol].probe_running) {
                        do_not_reply = true;
                        _mdns_init_pcb_probe(packet->tcpip_if, packet->ip_protocol, &service, 1, true);
                    } else if (ttl > 2250 && !col && !parsed_packet->authoritative && !parsed_packet->probe && !parsed_packet->questions && !_mdns_server->interfaces[packet->tcpip_if].pcbs[packet->ip_protocol].probe_running) {
                        _mdns_remove_scheduled_answer(packet->tcpip_if, packet->ip_protocol, type, service);
                    }
                }

            } else if (type == MDNS_TYPE_AAAA) {//ipv6
                ip_addr_t ip6;
                ip6.type = IPADDR_TYPE_V6;
                memcpy(ip6.u_addr.ip6.addr, data_ptr, 16);
                if (search_result) {
                    //check for more applicable searches (PTR & A/AAAA at the same time)
                    while (search_result) {
                        _mdns_search_result_add_ip(search_result, name->host, &ip6, packet->tcpip_if, packet->ip_protocol);
                        search_result = _mdns_search_find_from(search_result->next, name, type, packet->tcpip_if, packet->ip_protocol);
                    }
                } else if (ours) {
                    if (parsed_packet->questions && !parsed_packet->probe) {
                        _mdns_remove_parsed_question(parsed_packet, type, NULL);
                        continue;
                    }
                    //detect collision (-1=won, 0=none, 1=lost)
                    int col = 0;
                    if (clas > 1) {
                        col = 1;
                    } else if (!clas) {
                        col = -1;
                    } else {
                        col = _mdns_check_aaaa_collision(&(ip6.u_addr.ip6), packet->tcpip_if);
                    }
                    if (col == 2) {
                        goto clear_rx_packet;
                    } else if (col == 1) {
                        do_not_reply = true;
                        if (_mdns_server->interfaces[packet->tcpip_if].pcbs[packet->ip_protocol].probe_running) {
                            if (col && (parsed_packet->probe || parsed_packet->authoritative)) {
                                _mdns_server->interfaces[packet->tcpip_if].pcbs[packet->ip_protocol].failed_probes++;
                                char * new_host = _mdns_mangle_name((char *)_mdns_server->hostname);
                                if (new_host) {
                                    free((char *)_mdns_server->hostname);
                                    _mdns_server->hostname = new_host;
                                }
                                _mdns_restart_all_pcbs();
                            }
                        } else {
                            _mdns_init_pcb_probe(packet->tcpip_if, packet->ip_protocol, NULL, 0, true);
                        }
                    } else if (ttl > 60 && !col && !parsed_packet->authoritative && !parsed_packet->probe && !parsed_packet->questions && !_mdns_server->interfaces[packet->tcpip_if].pcbs[packet->ip_protocol].probe_running) {
                        _mdns_remove_scheduled_answer(packet->tcpip_if, packet->ip_protocol, type, NULL);
                    }
                }

            } else if (type == MDNS_TYPE_A) {
                ip_addr_t ip;
                ip.type = IPADDR_TYPE_V4;
                memcpy(&(ip.u_addr.ip4.addr), data_ptr, 4);
                if (search_result) {
                    //check for more applicable searches (PTR & A/AAAA at the same time)
                    while (search_result) {
                        _mdns_search_result_add_ip(search_result, name->host, &ip, packet->tcpip_if, packet->ip_protocol);
                        search_result = _mdns_search_find_from(search_result->next, name, type, packet->tcpip_if, packet->ip_protocol);
                    }
                } else if (ours) {
                    if (parsed_packet->questions && !parsed_packet->probe) {
                        _mdns_remove_parsed_question(parsed_packet, type, NULL);
                        continue;
                    }
                    //detect collision (-1=won, 0=none, 1=lost)
                    int col = 0;
                    if (clas > 1) {
                        col = 1;
                    } else if (!clas) {
                        col = -1;
                    } else {
                        col = _mdns_check_a_collision(&(ip.u_addr.ip4), packet->tcpip_if);
                    }
                    if (col == 2) {
                        goto clear_rx_packet;
                    } else if (col == 1) {
                        do_not_reply = true;
                        if (_mdns_server->interfaces[packet->tcpip_if].pcbs[packet->ip_protocol].probe_running) {
                            if (col && (parsed_packet->probe || parsed_packet->authoritative)) {
                                _mdns_server->interfaces[packet->tcpip_if].pcbs[packet->ip_protocol].failed_probes++;
                                char * new_host = _mdns_mangle_name((char *)_mdns_server->hostname);
                                if (new_host) {
                                    free((char *)_mdns_server->hostname);
                                    _mdns_server->hostname = new_host;
                                }
                                _mdns_restart_all_pcbs();
                            }
                        } else {
                            _mdns_init_pcb_probe(packet->tcpip_if, packet->ip_protocol, NULL, 0, true);
                        }
                    } else if (ttl > 60 && !col && !parsed_packet->authoritative && !parsed_packet->probe && !parsed_packet->questions && !_mdns_server->interfaces[packet->tcpip_if].pcbs[packet->ip_protocol].probe_running) {
                        _mdns_remove_scheduled_answer(packet->tcpip_if, packet->ip_protocol, type, NULL);
                    }
                }

            }
        }
        //end while
        if (parsed_packet->authoritative) {
            _mdns_search_finish_done();
        }
    }

    if (!do_not_reply && _mdns_server->interfaces[packet->tcpip_if].pcbs[packet->ip_protocol].state > PCB_PROBE_3 && (parsed_packet->questions || parsed_packet->discovery)) {
        _mdns_create_answer_from_parsed_packet(parsed_packet);
    }


clear_rx_packet:
    while (parsed_packet->questions) {
        mdns_parsed_question_t * question = parsed_packet->questions;
        parsed_packet->questions = parsed_packet->questions->next;
        free(question->host);
        free(question->service);
        free(question->proto);
        free(question->domain);
        free(question);
    }
    free(parsed_packet);
}

/**
 * @brief  Enable mDNS interface
 */
void _mdns_enable_pcb(tcpip_adapter_if_t tcpip_if, mdns_ip_protocol_t ip_protocol)
{
    if (!_mdns_server->interfaces[tcpip_if].pcbs[ip_protocol].pcb) {
        if (_mdns_pcb_init(tcpip_if, ip_protocol)) {
            return;
        }
    }
    _mdns_restart_pcb(tcpip_if, ip_protocol);
}

/**
 * @brief  Disable mDNS interface
 */
void _mdns_disable_pcb(tcpip_adapter_if_t tcpip_if, mdns_ip_protocol_t ip_protocol)
{
    if (_mdns_server->interfaces[tcpip_if].pcbs[ip_protocol].pcb) {
        _mdns_clear_pcb_tx_queue_head(tcpip_if, ip_protocol);
        _mdns_pcb_deinit(tcpip_if, ip_protocol);
        tcpip_adapter_if_t other_if = _mdns_get_other_if (tcpip_if);
        if (other_if != TCPIP_ADAPTER_IF_MAX && _mdns_server->interfaces[other_if].pcbs[ip_protocol].state == PCB_DUP) {
            _mdns_server->interfaces[other_if].pcbs[ip_protocol].state = PCB_OFF;
            _mdns_enable_pcb(other_if, ip_protocol);
        }
    }
    _mdns_server->interfaces[tcpip_if].pcbs[ip_protocol].state = PCB_OFF;
}

/**
 * @brief  Dispatch interface changes based on system events
 */
void _mdns_handle_system_event(system_event_id_t event, tcpip_adapter_if_t interface)
{
    if (!_mdns_server) {
        return;
    }

    tcpip_adapter_dhcp_status_t dcst;
    switch(event) {
    case SYSTEM_EVENT_STA_CONNECTED:
        if (!tcpip_adapter_dhcpc_get_status(TCPIP_ADAPTER_IF_STA, &dcst)) {
            if (dcst != TCPIP_ADAPTER_DHCP_STARTED) {
                _mdns_enable_pcb(TCPIP_ADAPTER_IF_STA, MDNS_IP_PROTOCOL_V4);
            }
        }
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        _mdns_enable_pcb(TCPIP_ADAPTER_IF_STA, MDNS_IP_PROTOCOL_V4);
        _mdns_announce_pcb(TCPIP_ADAPTER_IF_STA, MDNS_IP_PROTOCOL_V6, NULL, 0, true);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        _mdns_disable_pcb(TCPIP_ADAPTER_IF_STA, MDNS_IP_PROTOCOL_V4);
        _mdns_disable_pcb(TCPIP_ADAPTER_IF_STA, MDNS_IP_PROTOCOL_V6);
        break;
    case SYSTEM_EVENT_AP_START:
        _mdns_enable_pcb(TCPIP_ADAPTER_IF_AP, MDNS_IP_PROTOCOL_V4);
        break;
    case SYSTEM_EVENT_AP_STOP:
        _mdns_disable_pcb(TCPIP_ADAPTER_IF_AP, MDNS_IP_PROTOCOL_V4);
        _mdns_disable_pcb(TCPIP_ADAPTER_IF_AP, MDNS_IP_PROTOCOL_V6);
        break;
    case SYSTEM_EVENT_GOT_IP6:
        _mdns_enable_pcb(interface, MDNS_IP_PROTOCOL_V6);
        _mdns_announce_pcb(interface, MDNS_IP_PROTOCOL_V4, NULL, 0, true);
        break;
    case SYSTEM_EVENT_ETH_CONNECTED:
        if (!tcpip_adapter_dhcpc_get_status(TCPIP_ADAPTER_IF_ETH, &dcst)) {
            if (dcst != TCPIP_ADAPTER_DHCP_STARTED) {
                _mdns_enable_pcb(TCPIP_ADAPTER_IF_ETH, MDNS_IP_PROTOCOL_V4);
            }
        }
        break;
    case SYSTEM_EVENT_ETH_GOT_IP:
        _mdns_enable_pcb(TCPIP_ADAPTER_IF_ETH, MDNS_IP_PROTOCOL_V4);
        break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
        _mdns_disable_pcb(TCPIP_ADAPTER_IF_ETH, MDNS_IP_PROTOCOL_V4);
        _mdns_disable_pcb(TCPIP_ADAPTER_IF_ETH, MDNS_IP_PROTOCOL_V6);
        break;
    default:
        break;
    }
}

/*
 * MDNS Search
 * */

/**
 * @brief  Free search structure (except the results)
 */
static void _mdns_search_free(mdns_search_once_t * search)
{
    free(search->instance);
    free(search->service);
    free(search->proto);
    vSemaphoreDelete(search->lock);
    free(search);
}

/**
 * @brief  Allocate new search structure
 */
static mdns_search_once_t * _mdns_search_init(const char * name, const char * service, const char * proto, uint16_t type, uint32_t timeout, uint8_t max_results)
{
    mdns_search_once_t * search = (mdns_search_once_t *)malloc(sizeof(mdns_search_once_t));
    if (!search) {
        return NULL;
    }
    memset(search, 0, sizeof(mdns_search_once_t));

    search->lock = xSemaphoreCreateMutex();
    if (!search->lock) {
        free(search);
        return NULL;
    }

    if (name) {
        search->instance = strndup(name, MDNS_NAME_BUF_LEN-1);
        if (!search->instance) {
            _mdns_search_free(search);
            return NULL;
        }
    }

    if (service) {
        search->service = strndup(service, MDNS_NAME_BUF_LEN-1);
        if (!search->service) {
            _mdns_search_free(search);
            return NULL;
        }
    }

    if (proto) {
        search->proto = strndup(proto, MDNS_NAME_BUF_LEN-1);
        if (!search->proto) {
            _mdns_search_free(search);
            return NULL;
        }
    }

    search->type = type;
    search->timeout = timeout;
    search->num_results = 0;
    search->max_results = max_results;
    search->result = NULL;
    search->state = SEARCH_INIT;
    search->sent_at = 0;
    search->started_at = xTaskGetTickCount() * portTICK_PERIOD_MS;
    search->next = NULL;

    xSemaphoreTake(search->lock, 0);

    return search;
}

/**
 * @brief  Mark search as finished and remove it from search chain
 */
static void _mdns_search_finish(mdns_search_once_t * search)
{
    search->state = SEARCH_OFF;
    queueDetach(mdns_search_once_t, _mdns_server->search_once, search);
    xSemaphoreGive(search->lock);
}

/**
 * @brief  Add new search to the search chain
 */
static void _mdns_search_add(mdns_search_once_t * search)
{
    search->next = _mdns_server->search_once;
    _mdns_server->search_once = search;
}

/**
 * @brief  Called from parser to finish any searches that have reached maximum results
 */
static void _mdns_search_finish_done()
{
    mdns_search_once_t * search = _mdns_server->search_once;
    mdns_search_once_t * s = NULL;
    while (search) {
        s = search;
        search = search->next;
        if (s->max_results && s->num_results >= s->max_results) {
            _mdns_search_finish(s);
        }
    }
}

/**
 * @brief  Create linked IP (copy) from parsed one
 */
static mdns_ip_addr_t * _mdns_result_addr_create_ip(ip_addr_t * ip)
{
    mdns_ip_addr_t * a = (mdns_ip_addr_t *)malloc(sizeof(mdns_ip_addr_t));
    if (!a) {
        return NULL;
    }
    memset(a, 0 , sizeof(mdns_ip_addr_t));
    a->addr.type = ip->type;
    if (ip->type == IPADDR_TYPE_V6) {
        memcpy(a->addr.u_addr.ip6.addr, ip->u_addr.ip6.addr, 16);
    } else {
        a->addr.u_addr.ip4.addr = ip->u_addr.ip4.addr;
    }
    return a;
}

/**
 * @brief  Chain new IP to search result
 */
static void _mdns_result_add_ip(mdns_result_t * r, ip_addr_t * ip)
{
    mdns_ip_addr_t * a = r->addr;
    while (a) {
        if (a->addr.type == ip->type) {
            if (a->addr.type == IPADDR_TYPE_V4 && a->addr.u_addr.ip4.addr == ip->u_addr.ip4.addr) {
                return;
            }
            if (a->addr.type == IPADDR_TYPE_V6 && !memcmp(a->addr.u_addr.ip6.addr, ip->u_addr.ip6.addr, 16)) {
                return;
            }
        }
        a = a->next;
    }
    a = _mdns_result_addr_create_ip(ip);
    if (!a) {
        return;
    }
    a->next = r->addr;
    r->addr = a;
}

/**
 * @brief  Called from parser to add A/AAAA data to search result
 */
static void _mdns_search_result_add_ip(mdns_search_once_t * search, const char * hostname, ip_addr_t * ip, tcpip_adapter_if_t tcpip_if, mdns_ip_protocol_t ip_protocol)
{
    mdns_result_t * r = NULL;
    mdns_ip_addr_t * a = NULL;

    if ((search->type == MDNS_TYPE_A && ip->type == IPADDR_TYPE_V4)
      || (search->type == MDNS_TYPE_AAAA && ip->type == IPADDR_TYPE_V6)
      || search->type == MDNS_TYPE_ANY) {
        r = search->result;
        while (r) {
            if (r->tcpip_if == tcpip_if && r->ip_protocol == ip_protocol) {
                _mdns_result_add_ip(r, ip);
                return;
            }
            r = r->next;
        }
        if (!search->max_results || search->num_results < search->max_results) {
            r = (mdns_result_t *)malloc(sizeof(mdns_result_t));
            if (!r) {
                return;
            }

            memset(r, 0 , sizeof(mdns_result_t));

            a = _mdns_result_addr_create_ip(ip);
            if (!a) {
                free(r);
                return;
            }
            a->next = r->addr;
            r->addr = a;
            r->tcpip_if = tcpip_if;
            r->ip_protocol = ip_protocol;
            r->next = search->result;
            search->result = r;
            search->num_results++;
        }
    } else if (search->type == MDNS_TYPE_PTR) {
        r = search->result;
        while (r) {
            if (r->tcpip_if == tcpip_if && r->ip_protocol == ip_protocol && r->hostname && !strcasecmp(hostname, r->hostname)) {
                _mdns_result_add_ip(r, ip);
                break;
            }
            r = r->next;
        }
    }
}

/**
 * @brief  Called from parser to add PTR data to search result
 */
static mdns_result_t * _mdns_search_result_add_ptr(mdns_search_once_t * search, const char * instance, tcpip_adapter_if_t tcpip_if, mdns_ip_protocol_t ip_protocol)
{
    mdns_result_t * r = search->result;
    while (r) {
        if (r->tcpip_if == tcpip_if && r->ip_protocol == ip_protocol && r->instance_name && !strcasecmp(instance, r->instance_name)) {
            return r;
        }
        r = r->next;
    }
    if (!search->max_results || search->num_results < search->max_results) {
        r = (mdns_result_t *)malloc(sizeof(mdns_result_t));
        if (!r) {
            return NULL;
        }

        memset(r, 0 , sizeof(mdns_result_t));
        r->instance_name = strdup(instance);
        if (!r->instance_name) {
            free(r);
            return NULL;
        }

        r->tcpip_if = tcpip_if;
        r->ip_protocol = ip_protocol;
        r->next = search->result;
        search->result = r;
        search->num_results++;
        return r;
    }
    return NULL;
}

/**
 * @brief  Called from parser to add SRV data to search result
 */
static void _mdns_search_result_add_srv(mdns_search_once_t * search, const char * hostname, uint16_t port, tcpip_adapter_if_t tcpip_if, mdns_ip_protocol_t ip_protocol)
{
    mdns_result_t * r = search->result;
    while (r) {
        if (r->tcpip_if == tcpip_if && r->ip_protocol == ip_protocol && r->hostname && !strcasecmp(hostname, r->hostname)) {
            return;
        }
        r = r->next;
    }
    if (!search->max_results || search->num_results < search->max_results) {
        r = (mdns_result_t *)malloc(sizeof(mdns_result_t));
        if (!r) {
            return;
        }

        memset(r, 0 , sizeof(mdns_result_t));
        r->hostname = strdup(hostname);
        if (!r->hostname) {
            free(r);
            return;
        }
        r->port = port;
        r->tcpip_if = tcpip_if;
        r->ip_protocol = ip_protocol;
        r->next = search->result;
        search->result = r;
        search->num_results++;
    }
}

/**
 * @brief  Called from parser to add TXT data to search result
 */
static void _mdns_search_result_add_txt(mdns_search_once_t * search, mdns_txt_item_t * txt, size_t txt_count, tcpip_adapter_if_t tcpip_if, mdns_ip_protocol_t ip_protocol)
{
    int i;
    mdns_result_t * r = search->result;
    while (r) {
        if (r->tcpip_if == tcpip_if && r->ip_protocol == ip_protocol) {
            if (r->txt) {
                return;
            }
            r->txt = txt;
            r->txt_count = txt_count;
            return;
        }
        r = r->next;
    }
    if (!search->max_results || search->num_results < search->max_results) {
        r = (mdns_result_t *)malloc(sizeof(mdns_result_t));
        if (!r) {
            for (i=0; i<txt_count; i++) {
                free((char *)(txt[i].key));
                free((char *)(txt[i].value));
            }
            free(txt);
            return;
        }

        memset(r, 0 , sizeof(mdns_result_t));
        r->txt = txt;
        r->txt_count = txt_count;
        r->tcpip_if = tcpip_if;
        r->ip_protocol = ip_protocol;
        r->next = search->result;
        search->result = r;
        search->num_results++;
    }
}

/**
 * @brief  Called from packet parser to find matching running search
 */
static mdns_search_once_t * _mdns_search_find_from(mdns_search_once_t * s, mdns_name_t * name, uint16_t type, tcpip_adapter_if_t tcpip_if, mdns_ip_protocol_t ip_protocol)
{
    mdns_result_t * r = NULL;
    while (s) {
        if (s->state == SEARCH_OFF) {
            s = s->next;
            continue;
        }

        if (type == MDNS_TYPE_A || type == MDNS_TYPE_AAAA) {
            if ((s->type == MDNS_TYPE_ANY && s->service != NULL)
                || (s->type != MDNS_TYPE_ANY && s->type != type && s->type != MDNS_TYPE_PTR))
            {
                s = s->next;
                continue;
            }
            if (s->type != MDNS_TYPE_PTR) {
                if (!strcasecmp(name->host, s->instance)) {
                    return s;
                }
                s = s->next;
                continue;
            }
            r = s->result;
            while (r) {
                if (r->tcpip_if == tcpip_if && r->ip_protocol == ip_protocol && r->hostname && !strcasecmp(name->host, r->hostname)) {
                    return s;
                }
                r = r->next;
            }
            s = s->next;
            continue;
        }

        if (type == MDNS_TYPE_SRV || type == MDNS_TYPE_TXT) {
            if ((s->type == MDNS_TYPE_ANY && s->service == NULL)
                || (s->type != MDNS_TYPE_ANY && s->type != type && s->type != MDNS_TYPE_PTR))
            {
                s = s->next;
                continue;
            }
            if (strcasecmp(name->service, s->service)
                || strcasecmp(name->proto, s->proto))
            {
                s = s->next;
                continue;
            }
            if (s->type != MDNS_TYPE_PTR) {
                if (!strcasecmp(name->host, s->instance)) {
                    return s;
                }
                s = s->next;
                continue;
            }
            return s;
        }

        if (type == MDNS_TYPE_PTR && type == s->type && !strcasecmp(name->service, s->service) && !strcasecmp(name->proto, s->proto)) {
            return s;
        }

        s = s->next;
    }

    return NULL;
}

/**
 * @brief  Create search packet for partidular interface
 */
static mdns_tx_packet_t * _mdns_create_search_packet(mdns_search_once_t * search, tcpip_adapter_if_t tcpip_if, mdns_ip_protocol_t ip_protocol)
{
    mdns_result_t * r = NULL;
    mdns_tx_packet_t * packet = _mdns_alloc_packet_default(tcpip_if, ip_protocol);
    if (!packet) {
        return NULL;
    }

    mdns_out_question_t * q = (mdns_out_question_t *)malloc(sizeof(mdns_out_question_t));
    if (!q) {
        _mdns_free_tx_packet(packet);
        return NULL;
    }
    q->next = NULL;
    q->unicast = search->type != MDNS_TYPE_PTR;
    q->type = search->type;
    q->host = search->instance;
    q->service = search->service;
    q->proto = search->proto;
    q->domain = MDNS_DEFAULT_DOMAIN;
    queueToEnd(mdns_out_question_t, packet->questions, q);

    if (search->type == MDNS_TYPE_PTR) {
        r = search->result;
        while (r) {
            //full record on the same interface is available
            if (r->tcpip_if != tcpip_if || r->ip_protocol != ip_protocol || r->instance_name == NULL || r->hostname == NULL || r->addr == NULL) {
                r = r->next;
                continue;
            }
            mdns_out_answer_t * a = (mdns_out_answer_t *)malloc(sizeof(mdns_out_answer_t));
            if (!a) {
                _mdns_free_tx_packet(packet);
                return NULL;
            }
            a->type = MDNS_TYPE_PTR;
            a->service = NULL;
            a->custom_instance = r->instance_name;
            a->custom_service = search->service;
            a->custom_proto = search->proto;
            a->bye = false;
            a->flush = false;
            a->next = NULL;
            queueToEnd(mdns_out_answer_t, packet->answers, a);
            r = r->next;
        }
    }

    return packet;
}

/**
 * @brief  Send search packet to particular interface
 */
static void _mdns_search_send_pcb(mdns_search_once_t * search, tcpip_adapter_if_t tcpip_if, mdns_ip_protocol_t ip_protocol)
{
    mdns_tx_packet_t * packet = NULL;
    if (_mdns_server->interfaces[tcpip_if].pcbs[ip_protocol].pcb && _mdns_server->interfaces[tcpip_if].pcbs[ip_protocol].state == PCB_RUNNING) {
        packet = _mdns_create_search_packet(search, tcpip_if, ip_protocol);
        if (!packet) {
            return;
        }
        _mdns_dispatch_tx_packet(packet);
        _mdns_free_tx_packet(packet);
    }
}

/**
 * @brief  Send search packet to all available interfaces
 */
static void _mdns_search_send(mdns_search_once_t * search)
{
    uint8_t i, j;
    for (i=0; i<TCPIP_ADAPTER_IF_MAX; i++) {
        for (j=0; j<MDNS_IP_PROTOCOL_MAX; j++) {
            _mdns_search_send_pcb(search, (tcpip_adapter_if_t)i, (mdns_ip_protocol_t)j);
        }
    }
}

static void _mdns_tx_handle_packet(mdns_tx_packet_t * p)
{
    mdns_tx_packet_t * a = NULL;
    mdns_out_question_t * q = NULL;
    mdns_pcb_t * pcb = &_mdns_server->interfaces[p->tcpip_if].pcbs[p->ip_protocol];
    uint32_t send_after = 1000;

    if (pcb->state == PCB_OFF) {
        _mdns_free_tx_packet(p);
        return;
    }
    _mdns_dispatch_tx_packet(p);

    switch(pcb->state) {
    case PCB_PROBE_1:
        q = p->questions;
        while (q) {
            q->unicast = false;
            q = q->next;
        }
        //fallthrough
    case PCB_PROBE_2:
        _mdns_schedule_tx_packet(p, 250);
        pcb->state = (mdns_pcb_state_t)((uint8_t)(pcb->state) + 1);
        break;
    case PCB_PROBE_3:
        a = _mdns_create_announce_from_probe(p);
        if (!a) {
            _mdns_schedule_tx_packet(p, 250);
            break;
        }
        pcb->probe_running = false;
        pcb->probe_ip = false;
        pcb->probe_services_len = 0;
        pcb->failed_probes = 0;
        free(pcb->probe_services);
        pcb->probe_services = NULL;
        _mdns_free_tx_packet(p);
        p = a;
        send_after = 250;
        //fallthrough
    case PCB_ANNOUNCE_1:
        //fallthrough
    case PCB_ANNOUNCE_2:
        _mdns_schedule_tx_packet(p, send_after);
        pcb->state = (mdns_pcb_state_t)((uint8_t)(pcb->state) + 1);
        break;
    case PCB_ANNOUNCE_3:
        pcb->state = PCB_RUNNING;
        _mdns_free_tx_packet(p);
        break;
    default:
        _mdns_free_tx_packet(p);
        break;
    }
}

/**
 * @brief  Free action data
 */
static void _mdns_free_action(mdns_action_t * action)
{
    switch(action->type) {
    case ACTION_HOSTNAME_SET:
        free(action->data.hostname);
        break;
    case ACTION_INSTANCE_SET:
        free(action->data.instance);
        break;
    case ACTION_SERVICE_ADD:
        _mdns_free_service(action->data.srv_add.service->service);
        free(action->data.srv_add.service);
        break;
    case ACTION_SERVICE_INSTANCE_SET:
        free(action->data.srv_instance.instance);
        break;
    case ACTION_SERVICE_TXT_REPLACE:
        free(action->data.srv_txt_replace.txt);
        break;
    case ACTION_SERVICE_TXT_SET:
        free(action->data.srv_txt_set.key);
        free(action->data.srv_txt_set.value);
        break;
    case ACTION_SERVICE_TXT_DEL:
        free(action->data.srv_txt_del.key);
        break;
    case ACTION_SEARCH_ADD:
        //fallthrough
    case ACTION_SEARCH_SEND:
        //fallthrough
    case ACTION_SEARCH_END:
        _mdns_search_free(action->data.search_add.search);
        break;
    case ACTION_TX_HANDLE:
        _mdns_free_tx_packet(action->data.tx_handle.packet);
        break;
    case ACTION_RX_HANDLE:
        pbuf_free(action->data.rx_handle.packet->pb);
        free(action->data.rx_handle.packet);
        break;
    default:
        break;
    }
    free(action);
}

/**
 * @brief  Called from service thread to execute given action
 */
static void _mdns_execute_action(mdns_action_t * action)
{
    mdns_srv_item_t * a = NULL;
    mdns_service_t * service;
    char * key;
    char * value;
    mdns_txt_linked_item_t * txt, * t;

    switch(action->type) {
    case ACTION_SYSTEM_EVENT:
        _mdns_handle_system_event(action->data.sys_event.event_id, action->data.sys_event.interface);
        break;
    case ACTION_HOSTNAME_SET:
        _mdns_send_final_bye(true);
        free((char*)_mdns_server->hostname);
        _mdns_server->hostname = action->data.hostname;
        _mdns_restart_all_pcbs();

        break;
    case ACTION_INSTANCE_SET:
        _mdns_send_bye_all_pcbs_no_instance();
        free((char*)_mdns_server->instance);
        _mdns_server->instance = action->data.instance;
        _mdns_restart_all_pcbs_no_instance();

        break;
    case ACTION_SERVICE_ADD:
        action->data.srv_add.service->next = _mdns_server->services;
        _mdns_server->services = action->data.srv_add.service;
        _mdns_probe_all_pcbs(&action->data.srv_add.service, 1, false, false);

        break;
    case ACTION_SERVICE_INSTANCE_SET:
        if (action->data.srv_port.service->service->instance) {
            _mdns_send_bye(&action->data.srv_instance.service, 1, false);
            free((char*)action->data.srv_instance.service->service->instance);
        }
        action->data.srv_instance.service->service->instance = action->data.srv_instance.instance;
        _mdns_probe_all_pcbs(&action->data.srv_instance.service, 1, false, false);

        break;
    case ACTION_SERVICE_PORT_SET:
        action->data.srv_port.service->service->port = action->data.srv_port.port;
        _mdns_announce_all_pcbs(&action->data.srv_port.service, 1, true);

        break;
    case ACTION_SERVICE_TXT_REPLACE:
        service = action->data.srv_txt_replace.service->service;
        txt = service->txt;
        service->txt = NULL;
        while (txt) {
            t = txt;
            txt = txt->next;
            free((char *)t->value);
            free((char *)t->key);
            free(t);
        }
        service->txt = _mdns_allocate_txt(action->data.srv_txt_replace.num_items, action->data.srv_txt_replace.txt);
        _mdns_announce_all_pcbs(&action->data.srv_txt_replace.service, 1, false);

        break;
    case ACTION_SERVICE_TXT_SET:
        service = action->data.srv_txt_set.service->service;
        key = action->data.srv_txt_set.key;
        value = action->data.srv_txt_set.value;
        txt = service->txt;
        while (txt) {
            if (strcmp(txt->key, key) == 0) {
                free((char *)txt->value);
                free(key);
                txt->value = value;
                break;
            }
            txt = txt->next;
        }
        if (!txt) {
            txt = (mdns_txt_linked_item_t *)malloc(sizeof(mdns_txt_linked_item_t));
            if (!txt) {
                _mdns_free_action(action);
                return;
            }
            txt->key = key;
            txt->value = value;
            txt->next = service->txt;
            service->txt = txt;
        }

        _mdns_announce_all_pcbs(&action->data.srv_txt_set.service, 1, false);

        break;
    case ACTION_SERVICE_TXT_DEL:
        service = action->data.srv_txt_del.service->service;
        key = action->data.srv_txt_del.key;
        txt = service->txt;
        if (!txt) {
            break;
        }
        if (strcmp(txt->key, key) == 0) {
            service->txt = txt->next;
            free((char *)txt->key);
            free((char *)txt->value);
            free(txt);
        } else {
            while (txt->next) {
                if (strcmp(txt->next->key, key) == 0) {
                    t = txt->next;
                    txt->next = t->next;
                    free((char *)t->key);
                    free((char *)t->value);
                    free(t);
                    break;
                } else {
                    txt = txt->next;
                }
            }
        }
        free(key);

        _mdns_announce_all_pcbs(&action->data.srv_txt_set.service, 1, false);

        break;
    case ACTION_SERVICE_DEL:
        a = _mdns_server->services;
        if (_mdns_server->services == action->data.srv_del.service) {
            _mdns_server->services = a->next;
            _mdns_send_bye(&a, 1, false);
            _mdns_free_service(a->service);
            free(a);
        } else {
            while (a->next && a->next != action->data.srv_del.service) {
                a = a->next;
            }
            if (a->next == action->data.srv_del.service) {
                mdns_srv_item_t * b = a;
                a->next = a->next->next;
                _mdns_send_bye(&b, 1, false);
                _mdns_free_service(b->service);
                free(b);
            }
        }

        break;
    case ACTION_SERVICES_CLEAR:
        _mdns_send_final_bye(false);
        a = _mdns_server->services;
        _mdns_server->services = NULL;
        while (a) {
            mdns_srv_item_t * s = a;
            a = a->next;
            _mdns_free_service(s->service);
            free(s);
        }

        break;
    case ACTION_SEARCH_ADD:
        _mdns_search_add(action->data.search_add.search);
        break;
    case ACTION_SEARCH_SEND:
        _mdns_search_send(action->data.search_add.search);
        break;
    case ACTION_SEARCH_END:
        _mdns_search_finish(action->data.search_add.search);
        break;
    case ACTION_TX_HANDLE:
        _mdns_tx_handle_packet(action->data.tx_handle.packet);
        break;
    case ACTION_RX_HANDLE:
        mdns_parse_packet(action->data.rx_handle.packet);
        pbuf_free(action->data.rx_handle.packet->pb);
        free(action->data.rx_handle.packet);
        break;
    default:
        break;
    }
    free(action);
}

/**
 * @brief  Queue search action
 */
static esp_err_t _mdns_send_search_action(mdns_action_type_t type, mdns_search_once_t * search)
{
    mdns_action_t * action = NULL;

    action = (mdns_action_t *)malloc(sizeof(mdns_action_t));
    if (!action) {
        return ESP_ERR_NO_MEM;
    }

    action->type = type;
    action->data.search_add.search = search;
    if (xQueueSend(_mdns_server->action_queue, &action, (portTickType)0) != pdPASS) {
        free(action);
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

/**
 * @brief  Called from timer task to run mDNS responder
 */
static void _mdns_scheduler_run()
{
    mdns_tx_packet_t * p = _mdns_server->tx_queue_head;
    mdns_action_t * action = NULL;

    if (!p) {
        return;
    }
    MDNS_SERVICE_LOCK();
    if ((int32_t)(p->send_at - (xTaskGetTickCount() * portTICK_PERIOD_MS)) < 0) {
        action = (mdns_action_t *)malloc(sizeof(mdns_action_t));
        if (action) {
            _mdns_server->tx_queue_head = p->next;
            action->type = ACTION_TX_HANDLE;
            action->data.tx_handle.packet = p;
            if (xQueueSend(_mdns_server->action_queue, &action, (portTickType)0) != pdPASS) {
                free(action);
                _mdns_server->tx_queue_head = p;
            }
        }
    }
    MDNS_SERVICE_UNLOCK();
}

/**
 * @brief  Called from timer task to run active searches
 */
static void _mdns_search_run()
{
    mdns_search_once_t * s = _mdns_server->search_once;
    uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
    if (!s) {
        return;
    }
    MDNS_SERVICE_LOCK();
    while (s) {
        if (s->state != SEARCH_OFF) {
            if (now > (s->started_at + s->timeout)) {
                s->state = SEARCH_OFF;
                if (_mdns_send_search_action(ACTION_SEARCH_END, s) != ESP_OK) {
                    s->state = SEARCH_RUNNING;
                }
            } else if (s->state == SEARCH_INIT || (now - s->sent_at) > 1000) {
                s->state = SEARCH_RUNNING;
                s->sent_at = now;
                if (_mdns_send_search_action(ACTION_SEARCH_SEND, s) != ESP_OK) {
                    s->sent_at -= 1000;
                }
            }
        }
        s = s->next;
    }
    MDNS_SERVICE_UNLOCK();
}

/**
 * @brief  the main MDNS service task. Packets are received and parsed here
 */
static void _mdns_service_task(void *pvParameters)
{
    mdns_action_t * a = NULL;
    for (;;) {
        if (_mdns_server && _mdns_server->action_queue) {
            if (xQueueReceive(_mdns_server->action_queue, &a, portMAX_DELAY) == pdTRUE) {
                if (a->type == ACTION_TASK_STOP) {
                    break;
                }
                MDNS_SERVICE_LOCK();
                _mdns_execute_action(a);
                MDNS_SERVICE_UNLOCK();
            }
        } else {
            vTaskDelay(500 * portTICK_PERIOD_MS);
        }
    }
    _mdns_service_task_handle = NULL;
    vTaskDelete(NULL);
}

static void _mdns_timer_cb(void * arg)
{
    _mdns_scheduler_run();
    _mdns_search_run();
}

static esp_err_t _mdns_start_timer(){
    esp_timer_create_args_t timer_conf = {
        .callback = _mdns_timer_cb,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "mdns_timer"
    };
    esp_err_t err = esp_timer_create(&timer_conf, &(_mdns_server->timer_handle));
    if (err) {
        return err;
    }
    return esp_timer_start_periodic(_mdns_server->timer_handle, MDNS_TIMER_PERIOD_US);
}

static esp_err_t _mdns_stop_timer(){
    esp_err_t err = ESP_OK;
    if (_mdns_server->timer_handle) {
        err = esp_timer_stop(_mdns_server->timer_handle);
        if (err) {
            return err;
        }
        err = esp_timer_delete(_mdns_server->timer_handle);
    }
    return err;
}

/**
 * @brief  Start the service thread if not running
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
static esp_err_t _mdns_service_task_start()
{
#ifndef MDNS_TEST_MODE
    if (!_mdns_service_semaphore) {
        _mdns_service_semaphore = xSemaphoreCreateMutex();
        if (!_mdns_service_semaphore) {
            return ESP_FAIL;
        }
    }
    MDNS_SERVICE_LOCK();
    if (_mdns_start_timer()) {
        MDNS_SERVICE_UNLOCK();
        return ESP_FAIL;
    }
    if (!_mdns_service_task_handle) {
        xTaskCreatePinnedToCore(_mdns_service_task, "mdns", MDNS_SERVICE_STACK_DEPTH, NULL, 1, (TaskHandle_t * const)(&_mdns_service_task_handle), 0);
        if (!_mdns_service_task_handle) {
            MDNS_SERVICE_UNLOCK();
            return ESP_FAIL;
        }
    }
    MDNS_SERVICE_UNLOCK();
#endif

    return ESP_OK;
}

/**
 * @brief  Stop the service thread
 *
 * @return
 *      - ESP_OK
 */
static esp_err_t _mdns_service_task_stop()
{
#ifndef MDNS_TEST_MODE
    MDNS_SERVICE_LOCK();
    _mdns_stop_timer();
    if (_mdns_service_task_handle) {
        mdns_action_t action;
        mdns_action_t * a = &action;
        action.type = ACTION_TASK_STOP;
        if (xQueueSend(_mdns_server->action_queue, &a, (portTickType)0) != pdPASS) {
            vTaskDelete(_mdns_service_task_handle);
            _mdns_service_task_handle = NULL;
        }
        while (_mdns_service_task_handle) {
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    }
    MDNS_SERVICE_UNLOCK();
#endif
    return ESP_OK;
}

/*
 * Public Methods
 * */

esp_err_t mdns_handle_system_event(void *ctx, system_event_t *event)
{
    if (!_mdns_server) {
        return ESP_OK;
    }

    mdns_action_t * action = (mdns_action_t *)malloc(sizeof(mdns_action_t));
    if (!action) {
        return ESP_OK;
    }
    action->type = ACTION_SYSTEM_EVENT;
    action->data.sys_event.event_id = event->event_id;
    action->data.sys_event.interface = (event->event_id == SYSTEM_EVENT_GOT_IP6)?event->event_info.got_ip6.if_index:0;
    if (xQueueSend(_mdns_server->action_queue, &action, (portTickType)0) != pdPASS) {
        free(action);
    }
    return ESP_OK;
}

esp_err_t mdns_init()
{
    esp_err_t err = ESP_OK;

    if (_mdns_server) {
        return err;
    }

    _mdns_server = (mdns_server_t *)malloc(sizeof(mdns_server_t));
    if (!_mdns_server) {
        return ESP_ERR_NO_MEM;
    }
    memset((uint8_t*)_mdns_server, 0, sizeof(mdns_server_t));

    _mdns_server->lock = xSemaphoreCreateMutex();
    if (!_mdns_server->lock) {
        err = ESP_ERR_NO_MEM;
        goto free_server;
    }

    _mdns_server->action_queue = xQueueCreate(MDNS_ACTION_QUEUE_LEN, sizeof(mdns_action_t *));
    if (!_mdns_server->action_queue) {
        err = ESP_ERR_NO_MEM;
        goto free_lock;
    }

    if (_mdns_service_task_start()) {
        //service start failed!
        err = ESP_FAIL;
        goto free_all;
    }

    uint8_t i;
    ip6_addr_t tmp_addr6;
    tcpip_adapter_ip_info_t if_ip_info;

    for (i=0; i<TCPIP_ADAPTER_IF_MAX; i++) {
        if (!tcpip_adapter_get_ip6_linklocal(i, &tmp_addr6) && !_ipv6_address_is_zero(tmp_addr6)) {
            _mdns_enable_pcb(i, MDNS_IP_PROTOCOL_V6);
        }
        if (!tcpip_adapter_get_ip_info(i, &if_ip_info) && if_ip_info.ip.addr) {
            _mdns_enable_pcb(i, MDNS_IP_PROTOCOL_V4);
        }
    }

    return ESP_OK;

free_all:
    vQueueDelete(_mdns_server->action_queue);
free_lock:
    vSemaphoreDelete(_mdns_server->lock);
free_server:
    free(_mdns_server);
    _mdns_server = NULL;
    return err;
}

void mdns_free()
{
    uint8_t i, j;
    if (!_mdns_server) {
        return;
    }
    _mdns_service_task_stop();
    mdns_service_remove_all(_mdns_server);
    for (i=0; i<TCPIP_ADAPTER_IF_MAX; i++) {
        for (j=0; j<MDNS_IP_PROTOCOL_MAX; j++) {
            _mdns_pcb_deinit(i, j);
        }
    }
    free((char*)_mdns_server->hostname);
    free((char*)_mdns_server->instance);
    if (_mdns_server->action_queue) {
        mdns_action_t * c;
        while (xQueueReceive(_mdns_server->action_queue, &c, 0) == pdTRUE) {
            _mdns_free_action(c);
        }
        vQueueDelete(_mdns_server->action_queue);
    }
    _mdns_clear_tx_queue_head();
    while (_mdns_server->search_once) {
        mdns_search_once_t * h = _mdns_server->search_once;
        _mdns_server->search_once = h->next;
        free(h->instance);
        free(h->service);
        free(h->proto);
        vSemaphoreDelete(h->lock);
        if (h->result) {
            mdns_query_results_free(h->result);
        }
        free(h);
    }
    vSemaphoreDelete(_mdns_server->lock);
    free(_mdns_server);
    _mdns_server = NULL;
}

esp_err_t mdns_hostname_set(const char * hostname)
{
    if (!_mdns_server) {
        return ESP_ERR_INVALID_ARG;
    }
    if (strlen(hostname) > (MDNS_NAME_BUF_LEN - 1)) {
        return ESP_ERR_INVALID_ARG;
    }
    char * new_hostname = strndup(hostname, MDNS_NAME_BUF_LEN - 1);
    if (!new_hostname) {
        return ESP_ERR_NO_MEM;
    }

    mdns_action_t * action = (mdns_action_t *)malloc(sizeof(mdns_action_t));
    if (!action) {
        free(new_hostname);
        return ESP_ERR_NO_MEM;
    }
    action->type = ACTION_HOSTNAME_SET;
    action->data.hostname = new_hostname;
    if (xQueueSend(_mdns_server->action_queue, &action, (portTickType)0) != pdPASS) {
        free(new_hostname);
        free(action);
        return ESP_ERR_NO_MEM;
    }
    return ERR_OK;
}

esp_err_t mdns_instance_name_set(const char * instance)
{
    if (!_mdns_server) {
        return ESP_ERR_INVALID_ARG;
    }
    if (strlen(instance) > (MDNS_NAME_BUF_LEN - 1)) {
        return ESP_ERR_INVALID_ARG;
    }
    char * new_instance = strndup(instance, MDNS_NAME_BUF_LEN - 1);
    if (!new_instance) {
        return ESP_ERR_NO_MEM;
    }

    mdns_action_t * action = (mdns_action_t *)malloc(sizeof(mdns_action_t));
    if (!action) {
        free(new_instance);
        return ESP_ERR_NO_MEM;
    }
    action->type = ACTION_INSTANCE_SET;
    action->data.instance = new_instance;
    if (xQueueSend(_mdns_server->action_queue, &action, (portTickType)0) != pdPASS) {
        free(new_instance);
        free(action);
        return ESP_ERR_NO_MEM;
    }
    return ERR_OK;
}

/*
 * MDNS SERVICES
 * */

esp_err_t mdns_service_add(const char * instance, const char * service, const char * proto, uint16_t port, mdns_txt_item_t txt[], size_t num_items)
{
    if (!_mdns_server || !service || !proto || !port) {
        return ESP_ERR_INVALID_ARG;
    }
    mdns_srv_item_t * item = _mdns_get_service_item(service, proto);
    if (item) {
        return ESP_ERR_INVALID_ARG;
    }

    mdns_service_t * s = _mdns_create_service(service, proto, port, instance, num_items, txt);
    if (!s) {
        return ESP_ERR_NO_MEM;
    }

    item = (mdns_srv_item_t *)malloc(sizeof(mdns_srv_item_t));
    if (!item) {
        _mdns_free_service(s);
        return ESP_ERR_NO_MEM;
    }

    item->service = s;
    item->next = NULL;

    mdns_action_t * action = (mdns_action_t *)malloc(sizeof(mdns_action_t));
    if (!action) {
        _mdns_free_service(s);
        free(item);
        return ESP_ERR_NO_MEM;
    }
    action->type = ACTION_SERVICE_ADD;
    action->data.srv_add.service = item;
    if (xQueueSend(_mdns_server->action_queue, &action, (portTickType)0) != pdPASS) {
        _mdns_free_service(s);
        free(item);
        free(action);
        return ESP_ERR_NO_MEM;
    }

    uint8_t i = 0;
    while (_mdns_get_service_item(service, proto) == NULL && i++ < 200) {
        vTaskDelay(1);
    }
    if (i >= 200) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t mdns_service_port_set(const char * service, const char * proto, uint16_t port)
{
    if (!_mdns_server || !_mdns_server->services || !service || !proto || !port) {
        return ESP_ERR_INVALID_ARG;
    }
    mdns_srv_item_t * s = _mdns_get_service_item(service, proto);
    if (!s) {
        return ESP_ERR_NOT_FOUND;
    }

    mdns_action_t * action = (mdns_action_t *)malloc(sizeof(mdns_action_t));
    if (!action) {
        return ESP_ERR_NO_MEM;
    }
    action->type = ACTION_SERVICE_PORT_SET;
    action->data.srv_port.service = s;
    action->data.srv_port.port = port;
    if (xQueueSend(_mdns_server->action_queue, &action, (portTickType)0) != pdPASS) {
        free(action);
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

esp_err_t mdns_service_txt_set(const char * service, const char * proto, mdns_txt_item_t txt[], uint8_t num_items)
{
    if (!_mdns_server || !_mdns_server->services || !service || !proto || (num_items && txt == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }
    mdns_srv_item_t * s = _mdns_get_service_item(service, proto);
    if (!s) {
        return ESP_ERR_NOT_FOUND;
    }

    mdns_txt_item_t * txt_copy = NULL;
    if (num_items){
        txt_copy = (mdns_txt_item_t *)malloc(num_items * sizeof(mdns_txt_item_t));
        if (!txt_copy) {
            return ESP_ERR_NO_MEM;
        }
        memcpy(txt_copy, txt, num_items * sizeof(mdns_txt_item_t));
    }

    mdns_action_t * action = (mdns_action_t *)malloc(sizeof(mdns_action_t));
    if (!action) {
        free(txt_copy);
        return ESP_ERR_NO_MEM;
    }
    action->type = ACTION_SERVICE_TXT_REPLACE;
    action->data.srv_txt_replace.service = s;
    action->data.srv_txt_replace.num_items = num_items;
    action->data.srv_txt_replace.txt = txt_copy;

    if (xQueueSend(_mdns_server->action_queue, &action, (portTickType)0) != pdPASS) {
        free(txt_copy);
        free(action);
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}


esp_err_t mdns_service_txt_item_set(const char * service, const char * proto, const char * key, const char * value)
{
    if (!_mdns_server || !_mdns_server->services || !service || !proto) {
        return ESP_ERR_INVALID_ARG;
    }
    mdns_srv_item_t * s = _mdns_get_service_item(service, proto);
    if (!s) {
        return ESP_ERR_NOT_FOUND;
    }
    mdns_action_t * action = (mdns_action_t *)malloc(sizeof(mdns_action_t));
    if (!action) {
        return ESP_ERR_NO_MEM;
    }

    action->type = ACTION_SERVICE_TXT_SET;
    action->data.srv_txt_set.service = s;
    action->data.srv_txt_set.key = strdup(key);
    if (!action->data.srv_txt_set.key) {
        free(action);
        return ESP_ERR_NO_MEM;
    }
    action->data.srv_txt_set.value = strdup(value);
    if (!action->data.srv_txt_set.value) {
        free(action->data.srv_txt_set.key);
        free(action);
        return ESP_ERR_NO_MEM;
    }
    if (xQueueSend(_mdns_server->action_queue, &action, (portTickType)0) != pdPASS) {
        free(action->data.srv_txt_set.key);
        free(action->data.srv_txt_set.value);
        free(action);
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

esp_err_t mdns_service_txt_item_remove(const char * service, const char * proto, const char * key)
{
    if (!_mdns_server || !_mdns_server->services || !service || !proto) {
        return ESP_ERR_INVALID_ARG;
    }
    mdns_srv_item_t * s = _mdns_get_service_item(service, proto);
    if (!s) {
        return ESP_ERR_NOT_FOUND;
    }
    mdns_action_t * action = (mdns_action_t *)malloc(sizeof(mdns_action_t));
    if (!action) {
        return ESP_ERR_NO_MEM;
    }

    action->type = ACTION_SERVICE_TXT_DEL;
    action->data.srv_txt_del.service = s;
    action->data.srv_txt_del.key = strdup(key);
    if (!action->data.srv_txt_del.key) {
        free(action);
        return ESP_ERR_NO_MEM;
    }
    if (xQueueSend(_mdns_server->action_queue, &action, (portTickType)0) != pdPASS) {
        free(action->data.srv_txt_del.key);
        free(action);
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

esp_err_t mdns_service_instance_name_set(const char * service, const char * proto, const char * instance)
{
    if (!_mdns_server || !_mdns_server->services || !service || !proto) {
        return ESP_ERR_INVALID_ARG;
    }
    if (strlen(instance) > (MDNS_NAME_BUF_LEN - 1)) {
        return ESP_ERR_INVALID_ARG;
    }
    mdns_srv_item_t * s = _mdns_get_service_item(service, proto);
    if (!s) {
        return ESP_ERR_NOT_FOUND;
    }
    char * new_instance = strndup(instance, MDNS_NAME_BUF_LEN - 1);
    if (!new_instance) {
        return ESP_ERR_NO_MEM;
    }

    mdns_action_t * action = (mdns_action_t *)malloc(sizeof(mdns_action_t));
    if (!action) {
        free(new_instance);
        return ESP_ERR_NO_MEM;
    }
    action->type = ACTION_SERVICE_INSTANCE_SET;
    action->data.srv_instance.service = s;
    action->data.srv_instance.instance = new_instance;
    if (xQueueSend(_mdns_server->action_queue, &action, (portTickType)0) != pdPASS) {
        free(new_instance);
        free(action);
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

esp_err_t mdns_service_remove(const char * service, const char * proto)
{
    if (!_mdns_server || !_mdns_server->services || !service || !proto) {
        return ESP_ERR_INVALID_ARG;
    }
    mdns_srv_item_t * s = _mdns_get_service_item(service, proto);
    if (!s) {
        return ESP_ERR_NOT_FOUND;
    }

    mdns_action_t * action = (mdns_action_t *)malloc(sizeof(mdns_action_t));
    if (!action) {
        return ESP_ERR_NO_MEM;
    }
    action->type = ACTION_SERVICE_DEL;
    action->data.srv_del.service = s;
    if (xQueueSend(_mdns_server->action_queue, &action, (portTickType)0) != pdPASS) {
        free(action);
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

esp_err_t mdns_service_remove_all()
{
    if (!_mdns_server) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!_mdns_server->services) {
        return ESP_OK;
    }

    mdns_action_t * action = (mdns_action_t *)malloc(sizeof(mdns_action_t));
    if (!action) {
        return ESP_ERR_NO_MEM;
    }
    action->type = ACTION_SERVICES_CLEAR;
    if (xQueueSend(_mdns_server->action_queue, &action, (portTickType)0) != pdPASS) {
        free(action);
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

/*
 * MDNS QUERY
 * */

void mdns_query_results_free(mdns_result_t * results)
{
    mdns_result_t * r;
    mdns_ip_addr_t * a;
    int i;

    while (results) {
        r = results;

        free((char *)(r->hostname));
        free((char *)(r->instance_name));

        for (i=0; i<r->txt_count; i++) {
            free((char *)(r->txt[i].key));
            free((char *)(r->txt[i].value));
        }
        free(r->txt);

        while (r->addr) {
            a = r->addr;
            r->addr = r->addr->next;
            free(a);
        }

        results = results->next;
        free(r);
    }
}

esp_err_t mdns_query(const char * name, const char * service, const char * proto, uint16_t type, uint32_t timeout, size_t max_results, mdns_result_t ** results)
{
    mdns_search_once_t * search = NULL;

    *results = NULL;

    if (!_mdns_server) {
        return ESP_ERR_INVALID_STATE;
    }

    if (!timeout) {
        return ESP_ERR_INVALID_ARG;
    }

    search = _mdns_search_init(name, service, proto, type, timeout, max_results);
    if (!search) {
        return ESP_ERR_NO_MEM;
    }

    if (_mdns_send_search_action(ACTION_SEARCH_ADD, search)) {
        _mdns_search_free(search);
        return ESP_ERR_NO_MEM;
    }
    xSemaphoreTake(search->lock, portMAX_DELAY);

    *results = search->result;
    _mdns_search_free(search);

    return ESP_OK;
}

esp_err_t mdns_query_ptr(const char * service, const char * proto, uint32_t timeout, size_t max_results, mdns_result_t ** results)
{
    if (!service || !proto) {
        return ESP_ERR_INVALID_ARG;
    }

    return mdns_query(NULL, service, proto, MDNS_TYPE_PTR, timeout, max_results, results);
}

esp_err_t mdns_query_srv(const char * instance, const char * service, const char * proto, uint32_t timeout, mdns_result_t ** result)
{
    if (!instance || !service || !proto) {
        return ESP_ERR_INVALID_ARG;
    }

    return mdns_query(instance, service, proto, MDNS_TYPE_SRV, timeout, 1, result);
}

esp_err_t mdns_query_txt(const char * instance, const char * service, const char * proto, uint32_t timeout, mdns_result_t ** result)
{
    if (!instance || !service || !proto) {
        return ESP_ERR_INVALID_ARG;
    }

    return mdns_query(instance, service, proto, MDNS_TYPE_TXT, timeout, 1, result);
}

esp_err_t mdns_query_a(const char * name, uint32_t timeout, ip4_addr_t * addr)
{
    mdns_result_t * result = NULL;
    esp_err_t err;

    if (!name) {
        return ESP_ERR_INVALID_ARG;
    }

    err = mdns_query(name, NULL, NULL, MDNS_TYPE_A, timeout, 1, &result);

    if (err) {
        return err;
    }

    if (!result) {
        return ESP_ERR_NOT_FOUND;
    }

    mdns_ip_addr_t * a = result->addr;
    while (a) {
        if (a->addr.type == IPADDR_TYPE_V4) {
            addr->addr = a->addr.u_addr.ip4.addr;
            mdns_query_results_free(result);
            return ESP_OK;
        }
        a = a->next;
    }

    mdns_query_results_free(result);
    return ESP_ERR_NOT_FOUND;
}

esp_err_t mdns_query_aaaa(const char * name, uint32_t timeout, ip6_addr_t * addr)
{
    mdns_result_t * result = NULL;
    esp_err_t err;

    if (!name) {
        return ESP_ERR_INVALID_ARG;
    }

    err = mdns_query(name, NULL, NULL, MDNS_TYPE_AAAA, timeout, 1, &result);

    if (err) {
        return err;
    }

    if (!result) {
        return ESP_ERR_NOT_FOUND;
    }

    mdns_ip_addr_t * a = result->addr;
    while (a) {
        if (a->addr.type == IPADDR_TYPE_V6) {
            memcpy(addr->addr, a->addr.u_addr.ip6.addr, 16);
            mdns_query_results_free(result);
            return ESP_OK;
        }
        a = a->next;
    }

    mdns_query_results_free(result);
    return ESP_ERR_NOT_FOUND;
}

#ifdef MDNS_ENABLE_DEBUG

void mdns_debug_packet(const uint8_t * data, size_t len)
{
    static mdns_name_t n;
    mdns_header_t header;
    const uint8_t * content = data + MDNS_HEAD_LEN;
    uint32_t t = xTaskGetTickCount() * portTICK_PERIOD_MS;
    mdns_name_t * name = &n;
    memset(name, 0, sizeof(mdns_name_t));

    _mdns_dbg_printf("Packet[%u]: ", t);

    header.id = _mdns_read_u16(data, MDNS_HEAD_ID_OFFSET);
    header.flags.value = _mdns_read_u16(data, MDNS_HEAD_FLAGS_OFFSET);
    header.questions = _mdns_read_u16(data, MDNS_HEAD_QUESTIONS_OFFSET);
    header.answers = _mdns_read_u16(data, MDNS_HEAD_ANSWERS_OFFSET);
    header.servers = _mdns_read_u16(data, MDNS_HEAD_SERVERS_OFFSET);
    header.additional = _mdns_read_u16(data, MDNS_HEAD_ADDITIONAL_OFFSET);

    _mdns_dbg_printf("%s",
        (header.flags.value == MDNS_FLAGS_AUTHORITATIVE)?"AUTHORITATIVE\n":
        (header.flags.value == MDNS_FLAGS_DISTRIBUTED)?"DISTRIBUTED\n":
        (header.flags.value == 0)?"\n":" "
    );
    if (header.flags.value && header.flags.value != MDNS_FLAGS_AUTHORITATIVE) {
        _mdns_dbg_printf("0x%04X\n", header.flags.value);
    }

    if (header.questions) {
        uint8_t qs = header.questions;

        while (qs--) {
            content = _mdns_parse_fqdn(data, content, name);
            if (!content) {
                header.answers = 0;
                header.additional = 0;
                header.servers = 0;
                _mdns_dbg_printf("ERROR: %s:%u\n", __FILE__, __LINE__);
                break;
            }

            uint16_t type = _mdns_read_u16(content, MDNS_TYPE_OFFSET);
            uint16_t clas = _mdns_read_u16(content, MDNS_CLASS_OFFSET);
            bool unicast = !!(clas & 0x8000);
            clas &= 0x7FFF;
            content = content + 4;

            _mdns_dbg_printf("    Q: ");
            if (unicast) {
                _mdns_dbg_printf("*U* ");
            }
            if (type == MDNS_TYPE_PTR) {
                _mdns_dbg_printf("%s.%s%s.%s.%s. PTR ", name->host, name->sub?"_sub.":"", name->service, name->proto, name->domain);
            } else if (type == MDNS_TYPE_SRV) {
                _mdns_dbg_printf("%s.%s%s.%s.%s. SRV ", name->host, name->sub?"_sub.":"", name->service, name->proto, name->domain);
            } else if (type == MDNS_TYPE_TXT) {
                _mdns_dbg_printf("%s.%s%s.%s.%s. TXT ", name->host, name->sub?"_sub.":"", name->service, name->proto, name->domain);
            } else if (type == MDNS_TYPE_A) {
                _mdns_dbg_printf("%s.%s. A ", name->host, name->domain);
            } else if (type == MDNS_TYPE_AAAA) {
                _mdns_dbg_printf("%s.%s. AAAA ", name->host, name->domain);
            } else if (type == MDNS_TYPE_NSEC) {
                _mdns_dbg_printf("%s.%s%s.%s.%s. NSEC ", name->host, name->sub?"_sub.":"", name->service, name->proto, name->domain);
            } else if (type == MDNS_TYPE_ANY) {
                _mdns_dbg_printf("%s.%s%s.%s.%s. ANY ", name->host, name->sub?"_sub.":"", name->service, name->proto, name->domain);
            } else {
                _mdns_dbg_printf("%s.%s%s.%s.%s. %04X ", name->host, name->sub?"_sub.":"", name->service, name->proto, name->domain, type);
            }

            if (clas == 0x0001) {
                _mdns_dbg_printf("IN");
            } else {
                _mdns_dbg_printf("%04X", clas);
            }
            _mdns_dbg_printf("\n");
        }
    }

    if (header.answers || header.servers || header.additional) {
        uint16_t recordIndex = 0;

        while (content < (data + len)) {

            content = _mdns_parse_fqdn(data, content, name);
            if (!content) {
                _mdns_dbg_printf("ERROR: %s:%u\n", __FILE__, __LINE__);
                break;
            }

            uint16_t type = _mdns_read_u16(content, MDNS_TYPE_OFFSET);
            uint16_t clas = _mdns_read_u16(content, MDNS_CLASS_OFFSET);
            uint32_t ttl = _mdns_read_u32(content, MDNS_TTL_OFFSET);
            uint16_t data_len = _mdns_read_u16(content, MDNS_LEN_OFFSET);
            const uint8_t * data_ptr = content + MDNS_DATA_OFFSET;
            bool flush = !!(clas & 0x8000);
            clas &= 0x7FFF;

            content = data_ptr + data_len;
            if (content > (data + len)) {
                _mdns_dbg_printf("ERROR: %s:%u\n", __FILE__, __LINE__);
                break;
            }

            mdns_parsed_recort_type_t record_type = MDNS_ANSWER;

            if (recordIndex >= (header.answers + header.servers)) {
                record_type = MDNS_EXTRA;
            } else if (recordIndex >= (header.answers)) {
                record_type = MDNS_NS;
            }
            recordIndex++;

            if (record_type == MDNS_EXTRA) {
                _mdns_dbg_printf("    X");
            } else if (record_type == MDNS_NS) {
                _mdns_dbg_printf("    S");
            } else {
                _mdns_dbg_printf("    A");
            }

            if (type == MDNS_TYPE_PTR) {
                _mdns_dbg_printf(": %s%s%s.%s.%s. PTR ", name->host, name->host[0]?".":"", name->service, name->proto, name->domain);
            } else if (type == MDNS_TYPE_SRV) {
                _mdns_dbg_printf(": %s.%s.%s.%s. SRV ", name->host, name->service, name->proto, name->domain);
            } else if (type == MDNS_TYPE_TXT) {
                _mdns_dbg_printf(": %s.%s.%s.%s. TXT ", name->host, name->service, name->proto, name->domain);
            } else if (type == MDNS_TYPE_A) {
                _mdns_dbg_printf(": %s.%s. A ", name->host, name->domain);
            } else if (type == MDNS_TYPE_AAAA) {
                _mdns_dbg_printf(": %s.%s. AAAA ", name->host, name->domain);
            } else if (type == MDNS_TYPE_NSEC) {
                _mdns_dbg_printf(": %s.%s.%s.%s. NSEC ", name->host, name->service, name->proto, name->domain);
            } else if (type == MDNS_TYPE_ANY) {
                _mdns_dbg_printf(": %s.%s.%s.%s. ANY ", name->host, name->service, name->proto, name->domain);
            } else if (type == MDNS_TYPE_OPT) {
                _mdns_dbg_printf(": . OPT ");
            } else {
                _mdns_dbg_printf(": %s.%s.%s.%s. %04X ", name->host, name->service, name->proto, name->domain, type);
            }

            if (clas == 0x0001) {
                _mdns_dbg_printf("IN ");
            } else {
                _mdns_dbg_printf("%04X ", clas);
            }
            if (flush) {
                _mdns_dbg_printf("FLUSH ");
            }
            _mdns_dbg_printf("%u ", ttl);
            _mdns_dbg_printf("[%u] ", data_len);
            if (type == MDNS_TYPE_PTR) {
                if (!_mdns_parse_fqdn(data, data_ptr, name)) {
                    _mdns_dbg_printf("ERROR: %s:%u\n", __FILE__, __LINE__);
                    continue;
                }
                _mdns_dbg_printf("%s.%s.%s.%s.\n", name->host, name->service, name->proto, name->domain);
            } else if (type == MDNS_TYPE_SRV) {
                if (!_mdns_parse_fqdn(data, data_ptr + MDNS_SRV_FQDN_OFFSET, name)) {
                    _mdns_dbg_printf("ERROR: %s:%u\n", __FILE__, __LINE__);
                    continue;
                }
                uint16_t priority = _mdns_read_u16(data_ptr, MDNS_SRV_PRIORITY_OFFSET);
                uint16_t weight = _mdns_read_u16(data_ptr, MDNS_SRV_WEIGHT_OFFSET);
                uint16_t port = _mdns_read_u16(data_ptr, MDNS_SRV_PORT_OFFSET);
                _mdns_dbg_printf("%u %u %u %s.%s.\n", priority, weight, port, name->host, name->domain);
            } else if (type == MDNS_TYPE_TXT) {
                uint16_t i=0, y;
                while (i < data_len) {
                    uint8_t partLen = data_ptr[i++];
                    if ((i+partLen) > data_len) {
                        _mdns_dbg_printf("ERROR: %s:%u\n", __FILE__, __LINE__);
                        break;
                    }
                    char txt[partLen+1];
                    for (y=0; y<partLen; y++) {
                        char d = data_ptr[i++];
                        txt[y] = d;
                    }
                    txt[partLen] = 0;
                    _mdns_dbg_printf("%s", txt);
                    if (i<data_len) {
                        _mdns_dbg_printf("; ");
                    }
                }
                _mdns_dbg_printf("\n");
            } else if (type == MDNS_TYPE_AAAA) {
                ip6_addr_t ip6;
                memcpy(&ip6, data_ptr, sizeof(ip6_addr_t));
                _mdns_dbg_printf(IPV6STR "\n", IPV62STR(ip6));
            } else if (type == MDNS_TYPE_A) {
                ip4_addr_t ip;
                memcpy(&ip, data_ptr, sizeof(ip4_addr_t));
                _mdns_dbg_printf(IPSTR "\n", IP2STR(&ip));
            } else if (type == MDNS_TYPE_NSEC) {
                const uint8_t * old_ptr = data_ptr;
                const uint8_t * new_ptr = _mdns_parse_fqdn(data, data_ptr, name);
                if (new_ptr) {
                    _mdns_dbg_printf("%s.%s.%s.%s. ", name->host, name->service, name->proto, name->domain);
                    size_t diff = new_ptr - old_ptr;
                    data_len -= diff;
                    data_ptr = new_ptr;
                }
                size_t i;
                for (i=0; i<data_len; i++) {
                    _mdns_dbg_printf(" %02x", data_ptr[i]);
                }
                _mdns_dbg_printf("\n");
            } else if (type == MDNS_TYPE_OPT) {
                uint16_t opCode = _mdns_read_u16(data_ptr, 0);
                uint16_t opLen = _mdns_read_u16(data_ptr, 2);
                _mdns_dbg_printf(" Code: %04x Data[%u]:", opCode, opLen);
                size_t i;
                for (i=4; i<data_len; i++) {
                    _mdns_dbg_printf(" %02x", data_ptr[i]);
                }
                _mdns_dbg_printf("\n");
            } else {
                size_t i;
                for (i=0; i<data_len; i++) {
                    _mdns_dbg_printf(" %02x", data_ptr[i]);
                }
                _mdns_dbg_printf("\n");
            }
        }
    }
}
#endif
