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
//#include "esp_common.h"
#include <stdlib.h>
#include <string.h>
#include "lwip/inet.h"
#include "lwip/err.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/mem.h"
#include "lwip/ip_addr.h"
#include "apps/dhcpserver.h"

#include "tcpip_adapter.h"

#ifdef LWIP_ESP8266

enum dyc_dhcps_flags{
	DHCPS_STARTED = 0x00,
	DHCPS_STOP = 0x01,
	_END
} DhcpsFlags = DHCPS_STOP;

#define DHCPS_MAX_LEASE 0x64
#define BOOTP_BROADCAST 0x8000

#define DHCP_REQUEST        1
#define DHCP_REPLY          2
#define DHCP_HTYPE_ETHERNET 1
#define DHCP_HLEN_ETHERNET  6
#define DHCP_MSG_LEN      236

#define DHCPS_SERVER_PORT  67
#define DHCPS_CLIENT_PORT  68

#define DHCPDISCOVER  1
#define DHCPOFFER     2
#define DHCPREQUEST   3
#define DHCPDECLINE   4
#define DHCPACK       5
#define DHCPNAK       6
#define DHCPRELEASE   7

#define DHCP_OPTION_SUBNET_MASK   1
#define DHCP_OPTION_ROUTER        3
#define DHCP_OPTION_DNS_SERVER    6
#define DHCP_OPTION_REQ_IPADDR   50
#define DHCP_OPTION_LEASE_TIME   51
#define DHCP_OPTION_MSG_TYPE     53
#define DHCP_OPTION_SERVER_ID    54
#define DHCP_OPTION_INTERFACE_MTU 26
#define DHCP_OPTION_PERFORM_ROUTER_DISCOVERY 31
#define DHCP_OPTION_BROADCAST_ADDRESS 28
#define DHCP_OPTION_REQ_LIST     55
#define DHCP_OPTION_END         255

//#define USE_CLASS_B_NET 1
#define DHCPS_DEBUG          0
#define DHCPS_LOG printf

#define DYC_DHCP_CRASH //os_printf


#define MAX_STATION_NUM      8

#define DHCPS_STATE_OFFER 1
#define DHCPS_STATE_DECLINE 2
#define DHCPS_STATE_ACK 3
#define DHCPS_STATE_NAK 4
#define DHCPS_STATE_IDLE 5
#define DHCPS_STATE_RELEASE 6



#ifdef MEMLEAK_DEBUG
static const char mem_debug_file[] ICACHE_RODATA_ATTR STORE_ATTR = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////////
//static const uint8_t xid[4] = {0xad, 0xde, 0x12, 0x23};
//static u8_t old_xid[4] = {0};
static const u32_t magic_cookie  = 0x63538263;
//static const u32_t magic_cookie = 0x63538263;
//static const char magic_cookie[] = {0x63,0x82,0x53,0x63};
//static const u32_t magic_cookie = 0x63538263;
//static const u32_t magic_cookie_temp  = 0x63538263;

/*
ip_2_ip4(ipaddr)
IP_IS_V6(dest)
IP_SET_TYPE(ipaddr, IPADDR_TYPE_V4)    
*/


static struct udp_pcb *pcb_dhcps = NULL;
static ip4_addr_t  broadcast_dhcps;
static ip4_addr_t server_address;
static ip4_addr_t client_address;        //added
static ip4_addr_t client_address_plus;

static struct dhcps_lease dhcps_lease;
//static bool dhcps_lease_flag = true;
static list_node *plist = NULL;
static u8_t offer = 0xFF;
static bool renew = false;
#define DHCPS_LEASE_TIME_DEF	(120)
u32_t dhcps_lease_time = DHCPS_LEASE_TIME_DEF;  //minute



static enum dyc_dhcps_flags get_dhcps_status(void )
{
    return DhcpsFlags;
}
/******************************************************************************
 * FunctionName : node_insert_to_list
 * Description  : insert the node to the list
 * Parameters   : arg -- Additional argument to pass to the callback function
 * Returns      : none
*******************************************************************************/
void node_insert_to_list(list_node **phead, list_node* pinsert)
{
	list_node *plist = NULL;
	struct dhcps_pool *pdhcps_pool = NULL;
	struct dhcps_pool *pdhcps_node = NULL;
	if (*phead == NULL)
		*phead = pinsert;
	else {
		plist = *phead;
		pdhcps_node = pinsert->pnode;
		pdhcps_pool = plist->pnode;

		if(pdhcps_node->ip.addr < pdhcps_pool->ip.addr) {
		    pinsert->pnext = plist;
		    *phead = pinsert;
		} else {
            while (plist->pnext != NULL) {
                pdhcps_pool = plist->pnext->pnode;
                if (pdhcps_node->ip.addr < pdhcps_pool->ip.addr) {
                    pinsert->pnext = plist->pnext;
                    plist->pnext = pinsert;
                    break;
                }
                plist = plist->pnext;
            }

            if(plist->pnext == NULL) {
                plist->pnext = pinsert;
            }
		}
	}
//	pinsert->pnext = NULL;
}

/******************************************************************************
 * FunctionName : node_delete_from_list
 * Description  : remove the node from list
 * Parameters   : arg -- Additional argument to pass to the callback function
 * Returns      : none
*******************************************************************************/
void node_remove_from_list(list_node **phead, list_node* pdelete)
{
	list_node *plist = NULL;

	plist = *phead;
	if (plist == NULL){
		*phead = NULL;
	} else {
		if (plist == pdelete){
			*phead = plist->pnext;
			pdelete->pnext = NULL;
		} else {
			while (plist != NULL) {
				if (plist->pnext == pdelete){
					plist->pnext = pdelete->pnext;
					pdelete->pnext = NULL;
				}
				plist = plist->pnext;
			}
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////
/*
 * ��DHCP msg��Ϣ�ṹ����������
 *
 * @param optptr -- DHCP msg��Ϣλ��
 * @param type -- Ҫ��ӵ�����option
 *
 * @return uint8_t* ����DHCP msgƫ�Ƶ�ַ
 */
///////////////////////////////////////////////////////////////////////////////////
static u8_t* add_msg_type(u8_t *optptr, u8_t type)
{
        *optptr++ = DHCP_OPTION_MSG_TYPE;
        *optptr++ = 1;
        *optptr++ = type;
        return optptr;
}
///////////////////////////////////////////////////////////////////////////////////
/*
 * ��DHCP msg�ṹ������offerӦ������
 *
 * @param optptr -- DHCP msg��Ϣλ��
 *
 * @return uint8_t* ����DHCP msgƫ�Ƶ�ַ
 */
///////////////////////////////////////////////////////////////////////////////////
static u8_t* add_offer_options(u8_t *optptr)
{
        ip4_addr_t ipadd;

        ipadd.addr = *( (u32_t *) &server_address);

#ifdef USE_CLASS_B_NET
        *optptr++ = DHCP_OPTION_SUBNET_MASK;
        *optptr++ = 4;  //length
        *optptr++ = 255;
        *optptr++ = 240;	
        *optptr++ = 0;
        *optptr++ = 0;
#else
        *optptr++ = DHCP_OPTION_SUBNET_MASK;
        *optptr++ = 4;  
        *optptr++ = 255;
        *optptr++ = 255;	
        *optptr++ = 255;
        *optptr++ = 0;
#endif

        *optptr++ = DHCP_OPTION_LEASE_TIME;
        *optptr++ = 4;  
        *optptr++ = ((DHCPS_LEASE_TIMER * 60) >> 24) & 0xFF;
        *optptr++ = ((DHCPS_LEASE_TIMER * 60) >> 16) & 0xFF;
        *optptr++ = ((DHCPS_LEASE_TIMER * 60) >> 8) & 0xFF;
        *optptr++ = ((DHCPS_LEASE_TIMER * 60) >> 0) & 0xFF;

        *optptr++ = DHCP_OPTION_SERVER_ID;
        *optptr++ = 4;  
        *optptr++ = ip4_addr1( &ipadd);
        *optptr++ = ip4_addr2( &ipadd);
        *optptr++ = ip4_addr3( &ipadd);
        *optptr++ = ip4_addr4( &ipadd);

        if (dhcps_router_enabled(offer)){
        	struct ip_info if_ip;
		//bzero(&if_ip, sizeof(struct ip_info));
              memset(&if_ip ,0x00, sizeof(struct ip_info));
        
		tcpip_adapter_get_ip_info(WIFI_IF_AP, &if_ip);

		*optptr++ = DHCP_OPTION_ROUTER;
		*optptr++ = 4;
		*optptr++ = ip4_addr1( &if_ip.gw);
		*optptr++ = ip4_addr2( &if_ip.gw);
		*optptr++ = ip4_addr3( &if_ip.gw);
		*optptr++ = ip4_addr4( &if_ip.gw);
        }

#ifdef USE_DNS
	    *optptr++ = DHCP_OPTION_DNS_SERVER;
	    *optptr++ = 4;
	    *optptr++ = ip4_addr1( &ipadd);
		*optptr++ = ip4_addr2( &ipadd);
		*optptr++ = ip4_addr3( &ipadd);
		*optptr++ = ip4_addr4( &ipadd);
#endif

#ifdef CLASS_B_NET
        *optptr++ = DHCP_OPTION_BROADCAST_ADDRESS;
        *optptr++ = 4;  
        *optptr++ = ip4_addr1( &ipadd);
        *optptr++ = 255;
        *optptr++ = 255;
        *optptr++ = 255;
#else
        *optptr++ = DHCP_OPTION_BROADCAST_ADDRESS;
        *optptr++ = 4;  
        *optptr++ = ip4_addr1( &ipadd);
        *optptr++ = ip4_addr2( &ipadd);
        *optptr++ = ip4_addr3( &ipadd);
        *optptr++ = 255;
#endif

        *optptr++ = DHCP_OPTION_INTERFACE_MTU;
        *optptr++ = 2;  
#ifdef CLASS_B_NET
        *optptr++ = 0x05;	
        *optptr++ = 0xdc;
#else
        *optptr++ = 0x02;	
        *optptr++ = 0x40;
#endif

        *optptr++ = DHCP_OPTION_PERFORM_ROUTER_DISCOVERY;
        *optptr++ = 1;  
        *optptr++ = 0x00; 

        *optptr++ = 43;	
        *optptr++ = 6;	

        *optptr++ = 0x01;	
        *optptr++ = 4;  
        *optptr++ = 0x00;
        *optptr++ = 0x00;
        *optptr++ = 0x00;
        *optptr++ = 0x02; 	

        return optptr;
}
///////////////////////////////////////////////////////////////////////////////////
/*
 * ��DHCP msg�ṹ����ӽ����־����
 *
 * @param optptr -- DHCP msg��Ϣλ��
 *
 * @return uint8_t* ����DHCP msgƫ�Ƶ�ַ
 */
///////////////////////////////////////////////////////////////////////////////////
static u8_t* add_end(u8_t *optptr)
{

        *optptr++ = DHCP_OPTION_END;
        return optptr;
}
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
static void create_msg(struct dhcps_msg *m)
{
        ip4_addr_t client;


        client.addr = *( (uint32_t *) &client_address);

        m->op = DHCP_REPLY;

        m->htype = DHCP_HTYPE_ETHERNET;

        m->hlen = 6;  

        m->hops = 0;
//        os_memcpy((char *) xid, (char *) m->xid, sizeof(m->xid));
        m->secs = 0;
        m->flags = htons(BOOTP_BROADCAST); 

        memcpy((char *) m->yiaddr, (char *) &client.addr, sizeof(m->yiaddr));

        memset((char *) m->ciaddr, 0, sizeof(m->ciaddr));

        memset((char *) m->siaddr, 0, sizeof(m->siaddr));

        memset((char *) m->giaddr, 0, sizeof(m->giaddr));

        memset((char *) m->sname, 0, sizeof(m->sname));

        memset((char *) m->file, 0, sizeof(m->file));

        memset((char *) m->options, 0, sizeof(m->options));
        
//        u32_t temp  = 0x63538263;
 //       u8 *log_temp = (u8 *)&temp;
//DYC_DHCP_CRASH("l:%0x,%0x,%0x,%0x,",log_temp[0],log_temp[1],log_temp[2],log_temp[3]);

u32_t magic_cookie_temp = magic_cookie;

//extern bool system_get_string_from_flash(void *flash_str, void* ram_str,uint8 ram_str_len);
//system_get_string_from_flash((void *)(&magic_cookie), (void *)(&magic_cookie_temp),4);
//os_printf("ck_tmp3:%08X\n",magic_cookie_temp);

        //memcpy((char *) m->options, &magic_cookie, sizeof(magic_cookie));
        memcpy((char *) m->options, &magic_cookie_temp, sizeof(magic_cookie_temp));
//        DYC_DHCP_CRASH("m,");
}
///////////////////////////////////////////////////////////////////////////////////
/*
 * ����һ��OFFER
 *
 * @param -- m ָ����Ҫ���͵�DHCP msg����
 */
///////////////////////////////////////////////////////////////////////////////////
static void send_offer(struct dhcps_msg *m)
{
        u8_t *end;
	    struct pbuf *p, *q;
	    u8_t *data;
	    u16_t cnt=0;
	    u16_t i;
		err_t SendOffer_err_t;
        create_msg(m);

        end = add_msg_type(&m->options[4], DHCPOFFER);
        end = add_offer_options(end);
        end = add_end(end);

	    p = pbuf_alloc(PBUF_TRANSPORT, sizeof(struct dhcps_msg), PBUF_RAM);
#if DHCPS_DEBUG
		DHCPS_LOG("udhcp: send_offer>>p->ref = %d\n", p->ref);
#endif
	    if(p != NULL){
	       
#if DHCPS_DEBUG
	        DHCPS_LOG("dhcps: send_offer>>pbuf_alloc succeed\n");
	        DHCPS_LOG("dhcps: send_offer>>p->tot_len = %d\n", p->tot_len);
	        DHCPS_LOG("dhcps: send_offer>>p->len = %d\n", p->len);
#endif
	        q = p;
	        while(q != NULL){
	            data = (u8_t *)q->payload;
	            for(i=0; i<q->len; i++)
	            {
	                data[i] = ((u8_t *) m)[cnt++];
#if DHCPS_DEBUG
					DHCPS_LOG("%02x ",data[i]);
					if((i+1)%16 == 0){
						DHCPS_LOG("\n");
					}
#endif
	            }

	            q = q->next;
	        }
	    }else{
	        
#if DHCPS_DEBUG
	        DHCPS_LOG("dhcps: send_offer>>pbuf_alloc failed\n");
#endif
	        return;
	    }
        
        ip_addr_t ip_temp = IPADDR4_INIT(0x0);
        ip4_addr_set(ip_2_ip4(&ip_temp), &broadcast_dhcps);
                SendOffer_err_t = udp_sendto( pcb_dhcps, p, &ip_temp, DHCPS_CLIENT_PORT );
#if DHCPS_DEBUG
	        DHCPS_LOG("dhcps: send_offer>>udp_sendto result %x\n",SendOffer_err_t);
#endif
	    if(p->ref != 0){	
#if DHCPS_DEBUG
	        DHCPS_LOG("udhcp: send_offer>>free pbuf\n");
#endif
	        pbuf_free(p);
	    }
}
///////////////////////////////////////////////////////////////////////////////////
/*
 * ����һ��NAK��Ϣ
 *
 * @param m ָ����Ҫ���͵�DHCP msg����
 */
///////////////////////////////////////////////////////////////////////////////////
static void send_nak(struct dhcps_msg *m)
{
        u8_t *end;
        struct pbuf *p, *q;
        u8_t *data;
        u16_t cnt=0;
        u16_t i;
        err_t SendNak_err_t;
        create_msg(m);

        end = add_msg_type(&m->options[4], DHCPNAK);
        end = add_end(end);

	    p = pbuf_alloc(PBUF_TRANSPORT, sizeof(struct dhcps_msg), PBUF_RAM);
#if DHCPS_DEBUG
		DHCPS_LOG("udhcp: send_nak>>p->ref = %d\n", p->ref);
#endif
	    if(p != NULL){
	        
#if DHCPS_DEBUG
	        DHCPS_LOG("dhcps: send_nak>>pbuf_alloc succeed\n");
	        DHCPS_LOG("dhcps: send_nak>>p->tot_len = %d\n", p->tot_len);
	        DHCPS_LOG("dhcps: send_nak>>p->len = %d\n", p->len);
#endif
	        q = p;
	        while(q != NULL){
	            data = (u8_t *)q->payload;
	            for(i=0; i<q->len; i++)
	            {
	                data[i] = ((u8_t *) m)[cnt++];
#if DHCPS_DEBUG					
					DHCPS_LOG("%02x ",data[i]);
					if((i+1)%16 == 0){
						DHCPS_LOG("\n");
					}
#endif
	            }

	            q = q->next;
	        }
	    }else{
	        
#if DHCPS_DEBUG
	        DHCPS_LOG("dhcps: send_nak>>pbuf_alloc failed\n");
#endif
	        return;
    	}

        ip_addr_t ip_temp = IPADDR4_INIT(0x0);
        ip4_addr_set(ip_2_ip4(&ip_temp), &broadcast_dhcps);       
        SendNak_err_t = udp_sendto( pcb_dhcps, p, &ip_temp, DHCPS_CLIENT_PORT );
#if DHCPS_DEBUG
	        DHCPS_LOG("dhcps: send_nak>>udp_sendto result %x\n",SendNak_err_t);
#endif
 	    if(p->ref != 0){
#if DHCPS_DEBUG			
	        DHCPS_LOG("udhcp: send_nak>>free pbuf\n");
#endif
	        pbuf_free(p);
	    }
}
///////////////////////////////////////////////////////////////////////////////////
/*
 * ����һ��ACK��DHCP�ͻ���
 *
 * @param m ָ����Ҫ���͵�DHCP msg����
 */
///////////////////////////////////////////////////////////////////////////////////
static void send_ack(struct dhcps_msg *m)
{
        u8_t *end;
        struct pbuf *p, *q;
        u8_t *data;
        u16_t cnt=0;
        u16_t i;
        err_t SendAck_err_t;
        create_msg(m);

        end = add_msg_type(&m->options[4], DHCPACK);
        end = add_offer_options(end);
        end = add_end(end);
	    
	    p = pbuf_alloc(PBUF_TRANSPORT, sizeof(struct dhcps_msg), PBUF_RAM);
#if DHCPS_DEBUG
		DHCPS_LOG("udhcp: send_ack>>p->ref = %d\n", p->ref);
#endif
	    if(p != NULL){
	        
#if DHCPS_DEBUG
	        DHCPS_LOG("dhcps: send_ack>>pbuf_alloc succeed\n");
	        DHCPS_LOG("dhcps: send_ack>>p->tot_len = %d\n", p->tot_len);
	        DHCPS_LOG("dhcps: send_ack>>p->len = %d\n", p->len);
#endif
	        q = p;
	        while(q != NULL){
	            data = (u8_t *)q->payload;
	            for(i=0; i<q->len; i++)
	            {
	                data[i] = ((u8_t *) m)[cnt++];
#if DHCPS_DEBUG					
					DHCPS_LOG("%02x ",data[i]);
					if((i+1)%16 == 0){
						DHCPS_LOG("\n");
					}
#endif
	            }

	            q = q->next;
	        }
	    }else{
	    
#if DHCPS_DEBUG
	        DHCPS_LOG("dhcps: send_ack>>pbuf_alloc failed\n");
#endif
	        return;
	    }

        ip_addr_t ip_temp = IPADDR4_INIT(0x0);
        ip4_addr_set(ip_2_ip4(&ip_temp), &broadcast_dhcps);   
        SendAck_err_t = udp_sendto( pcb_dhcps, p, &ip_temp, DHCPS_CLIENT_PORT );
#if DHCPS_DEBUG
	        DHCPS_LOG("dhcps: send_ack>>udp_sendto result %x\n",SendAck_err_t);
#endif
	    
	    if(p->ref != 0){
#if DHCPS_DEBUG
	        DHCPS_LOG("udhcp: send_ack>>free pbuf\n");
#endif
	        pbuf_free(p);
	    }
}
///////////////////////////////////////////////////////////////////////////////////
/*
 * ����DHCP�ͻ��˷�����DHCP����������Ϣ�����Բ�ͬ��DHCP��������������Ӧ��Ӧ��
 *
 * @param optptr DHCP msg�е���������
 * @param len ��������Ĵ��?(byte)
 *
 * @return uint8_t ���ش�����DHCP Server״ֵ̬
 */
///////////////////////////////////////////////////////////////////////////////////
static u8_t parse_options(u8_t *optptr, s16_t len)
{
       ip4_addr_t client;
    	bool is_dhcp_parse_end = false;
    	struct dhcps_state s;

        client.addr = *( (uint32_t *) &client_address);// Ҫ�����DHCP�ͻ��˵�IP

        u8_t *end = optptr + len;
        u16_t type = 0;

        s.state = DHCPS_STATE_IDLE;

        while (optptr < end) {
#if DHCPS_DEBUG
        	DHCPS_LOG("dhcps: (s16_t)*optptr = %d\n", (s16_t)*optptr);
#endif
        	switch ((s16_t) *optptr) {

                case DHCP_OPTION_MSG_TYPE:	//53
                        type = *(optptr + 2);
                        break;

                case DHCP_OPTION_REQ_IPADDR://50
                        if( memcmp( (char *) &client.addr, (char *) optptr+2,4)==0 ) {
#if DHCPS_DEBUG
                    		DHCPS_LOG("dhcps: DHCP_OPTION_REQ_IPADDR = 0 ok\n");
#endif
                            s.state = DHCPS_STATE_ACK;
                        }else {
#if DHCPS_DEBUG
                    		DHCPS_LOG("dhcps: DHCP_OPTION_REQ_IPADDR != 0 err\n");
#endif
                            s.state = DHCPS_STATE_NAK;
                        }
                        break;
                case DHCP_OPTION_END:
			            {
			                is_dhcp_parse_end = true;
			            }
                        break;
            }

		    if(is_dhcp_parse_end){
		            break;
		    }

            optptr += optptr[1] + 2;
        }

        switch (type){
        
        	case DHCPDISCOVER://1
                s.state = DHCPS_STATE_OFFER;
#if DHCPS_DEBUG
            	DHCPS_LOG("dhcps: DHCPD_STATE_OFFER\n");
#endif
                break;

        	case DHCPREQUEST://3
                if ( !(s.state == DHCPS_STATE_ACK || s.state == DHCPS_STATE_NAK) ) {
                    if(renew == true) {
                        s.state = DHCPS_STATE_ACK;
                    } else {
                        s.state = DHCPS_STATE_NAK;
                    }
#if DHCPS_DEBUG
                		DHCPS_LOG("dhcps: DHCPD_STATE_NAK\n");
#endif
                }
                break;

			case DHCPDECLINE://4
                s.state = DHCPS_STATE_IDLE;
#if DHCPS_DEBUG
            	DHCPS_LOG("dhcps: DHCPD_STATE_IDLE\n");
#endif
                break;

        	case DHCPRELEASE://7
                s.state = DHCPS_STATE_RELEASE;
#if DHCPS_DEBUG
            	DHCPS_LOG("dhcps: DHCPD_STATE_IDLE\n");
#endif
                break;
        }
#if DHCPS_DEBUG
    	DHCPS_LOG("dhcps: return s.state = %d\n", s.state);
#endif
        return s.state;
}
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
static s16_t parse_msg(struct dhcps_msg *m, u16_t len)
{

//u32 magic_cookie_temp = magic_cookie;
//extern bool system_get_string_from_flash(void *flash_str, void* ram_str,uint8 ram_str_len);
//system_get_string_from_flash((void *)(&magic_cookie), (void *)(&magic_cookie_temp),4);
//os_printf("ck_tmp4:%08X\n",magic_cookie_temp);

		if(memcmp((char *)m->options,
                    &magic_cookie,
                    sizeof(magic_cookie)) == 0){
#if DHCPS_DEBUG
        	        DHCPS_LOG("dhcps: len = %d\n", len);
#endif

	              ip4_addr_t addr_tmp;    
//	                memcpy((char *)old_xid, (char *)m->xid, sizeof(m->xid));

			struct dhcps_pool *pdhcps_pool = NULL;
			list_node *pnode = NULL;
			list_node *pback_node = NULL;
			ip4_addr_t first_address;
			bool flag = false;

//						POOL_START:
			first_address.addr = dhcps_lease.start_ip.addr;
			client_address.addr = client_address_plus.addr;
			renew = false;
//							addr_tmp.addr =  htonl(client_address_plus.addr);
//							addr_tmp.addr++;
//							client_address_plus.addr = htonl(addr_tmp.addr);
			for (pback_node = plist; pback_node != NULL;pback_node = pback_node->pnext) {
				pdhcps_pool = pback_node->pnode;
				if (memcmp(pdhcps_pool->mac, m->chaddr, sizeof(pdhcps_pool->mac)) == 0){
//									os_printf("the same device request ip\n");
					if (memcmp(&pdhcps_pool->ip.addr, m->ciaddr, sizeof(pdhcps_pool->ip.addr)) == 0) {
					    renew = true;
					}
					client_address.addr = pdhcps_pool->ip.addr;
					pdhcps_pool->lease_timer = DHCPS_LEASE_TIMER;
					pnode = pback_node;
					goto POOL_CHECK;
				} else if (pdhcps_pool->ip.addr == client_address_plus.addr){
//									client_address.addr = client_address_plus.addr;
//									os_printf("the ip addr has been request\n");
					addr_tmp.addr = htonl(client_address_plus.addr);
					addr_tmp.addr++;
					client_address_plus.addr = htonl(addr_tmp.addr);
					client_address.addr = client_address_plus.addr;
				}

				if(flag == false) { // search the fisrt unused ip
                                    if(first_address.addr < pdhcps_pool->ip.addr) {
                                        flag = true;
                                    } else {
                                        addr_tmp.addr = htonl(first_address.addr);
                                        addr_tmp.addr++;
                                        first_address.addr = htonl(addr_tmp.addr);
                                    }
				}
			}
			if (client_address_plus.addr > dhcps_lease.end_ip.addr) {
			    client_address.addr = first_address.addr;
			}
			if (client_address.addr > dhcps_lease.end_ip.addr) {
			    client_address_plus.addr = dhcps_lease.start_ip.addr;
			    pdhcps_pool = NULL;
			    pnode = NULL;
			} else {
        			pdhcps_pool = (struct dhcps_pool *)malloc(sizeof(struct dhcps_pool));
                            memset(pdhcps_pool ,0x00 ,sizeof(struct dhcps_pool));
                    
        			pdhcps_pool->ip.addr = client_address.addr;
        			memcpy(pdhcps_pool->mac, m->chaddr, sizeof(pdhcps_pool->mac));
        			pdhcps_pool->lease_timer = DHCPS_LEASE_TIMER;
        			pnode = (list_node *)malloc(sizeof(list_node ));
                            memset(pnode ,0x00 ,sizeof(list_node));
                    
        			pnode->pnode = pdhcps_pool;
                            pnode->pnext = NULL;
                            node_insert_to_list(&plist,pnode);
                            if (client_address.addr == dhcps_lease.end_ip.addr) {
                                    client_address_plus.addr = dhcps_lease.start_ip.addr;
                            } else {
                                    addr_tmp.addr = htonl(client_address.addr);
                                    addr_tmp.addr++;
                                    client_address_plus.addr = htonl(addr_tmp.addr);
                            }
			}

			POOL_CHECK:
			if ((client_address.addr > dhcps_lease.end_ip.addr) || (ip4_addr_isany(&client_address))){
			    if(pnode != NULL) {
			        node_remove_from_list(&plist,pnode);
			        free(pnode);
			        pnode = NULL;
			    }

			    if (pdhcps_pool != NULL) {
			        free(pdhcps_pool);
			        pdhcps_pool = NULL;
			    }
// client_address_plus.addr = dhcps_lease.start_ip.addr;
				return 4;
			}

			s16_t ret = parse_options(&m->options[4], len);;

			if(ret == DHCPS_STATE_RELEASE) {
			    if(pnode != NULL) {
			        node_remove_from_list(&plist,pnode);
			        free(pnode);
			        pnode = NULL;
			    }

			    if (pdhcps_pool != NULL) {
			        free(pdhcps_pool);
			        pdhcps_pool = NULL;
			    }
			    memset(&client_address,0x0,sizeof(client_address));
			}

//TO_DO ,  set (ap-> sta)   ip_addr   no use.
//if ( tcpip_dep_set_ip_info(STATION_IF, struct ip_info *if_ip) == false )
    //return 0;
//if (wifi_softap_set_station_info(m->chaddr, &client_address) == false) {
   // return 0;

#if DHCPS_DEBUG
	                DHCPS_LOG("dhcps: xid changed\n");
	                DHCPS_LOG("dhcps: client_address.addr = %x\n", client_address.addr);
#endif
	        return ret;
	    }
        return 0;
}


static void handle_dhcp(void *arg,
					struct udp_pcb *pcb, 
					struct pbuf *p,  
					const ip_addr_t *addr,
					u16_t port)
{
        struct dhcps_msg *pmsg_dhcps = NULL;
        s16_t tlen;
        u16_t i;
        u16_t dhcps_msg_cnt=0;
        u8_t *p_dhcps_msg = NULL;
        u8_t *data;

#if DHCPS_DEBUG
    	DHCPS_LOG("dhcps: handle_dhcp-> receive a packet\n");
#endif
	    if (p==NULL) return;

	    pmsg_dhcps = (struct dhcps_msg *)malloc(sizeof(struct dhcps_msg));
           memset(pmsg_dhcps ,0x00 ,sizeof(struct dhcps_msg));
        
	    if (NULL == pmsg_dhcps){
	    	pbuf_free(p);
	    	return;
	    }
	    p_dhcps_msg = (u8_t *)pmsg_dhcps;
		tlen = p->tot_len;
	    data = p->payload;

#if DHCPS_DEBUG
	    DHCPS_LOG("dhcps: handle_dhcp-> p->tot_len = %d\n", tlen);
	    DHCPS_LOG("dhcps: handle_dhcp-> p->len = %d\n", p->len);
#endif		

	    for(i=0; i<p->len; i++){
	        p_dhcps_msg[dhcps_msg_cnt++] = data[i];
#if DHCPS_DEBUG					
			DHCPS_LOG("%02x ",data[i]);
			if((i+1)%16 == 0){
				DHCPS_LOG("\n");
			}
#endif
	    }
		
		if(p->next != NULL) {
#if DHCPS_DEBUG
	        DHCPS_LOG("dhcps: handle_dhcp-> p->next != NULL\n");
	        DHCPS_LOG("dhcps: handle_dhcp-> p->next->tot_len = %d\n",p->next->tot_len);
	        DHCPS_LOG("dhcps: handle_dhcp-> p->next->len = %d\n",p->next->len);
#endif
			
	        data = p->next->payload;
	        for(i=0; i<p->next->len; i++){
	            p_dhcps_msg[dhcps_msg_cnt++] = data[i];
#if DHCPS_DEBUG					
				DHCPS_LOG("%02x ",data[i]);
				if((i+1)%16 == 0){
					DHCPS_LOG("\n");
				}
#endif
			}
		}

		/*
	     * DHCP �ͻ���������Ϣ����
	    */
#if DHCPS_DEBUG
    	DHCPS_LOG("dhcps: handle_dhcp-> parse_msg(p)\n");
#endif
		
        switch(parse_msg(pmsg_dhcps, tlen - 240)) {
	        case DHCPS_STATE_OFFER://1 
#if DHCPS_DEBUG            
            	 DHCPS_LOG("dhcps: handle_dhcp-> DHCPD_STATE_OFFER\n");
#endif			
	             send_offer(pmsg_dhcps);
	             break;
	        case DHCPS_STATE_ACK://3
#if DHCPS_DEBUG
            	 DHCPS_LOG("dhcps: handle_dhcp-> DHCPD_STATE_ACK\n");
#endif			
	             send_ack(pmsg_dhcps);
	             break;
	        case DHCPS_STATE_NAK://4
#if DHCPS_DEBUG            
            	 DHCPS_LOG("dhcps: handle_dhcp-> DHCPD_STATE_NAK\n");
#endif
	             send_nak(pmsg_dhcps);
	             break;
		default :
				 break;
        }
#if DHCPS_DEBUG
    	DHCPS_LOG("dhcps: handle_dhcp-> pbuf_free(p)\n");
#endif
        pbuf_free(p);
        free(pmsg_dhcps);
        pmsg_dhcps = NULL;
}
///////////////////////////////////////////////////////////////////////////////////
static void wifi_softap_init_dhcps_lease(u32_t ip)
{
	u32_t softap_ip = 0,local_ip = 0;
	u32_t start_ip = 0;
	u32_t end_ip = 0;
	if (dhcps_lease.enable == true) {
		softap_ip = htonl(ip);
		start_ip = htonl(dhcps_lease.start_ip.addr);
		end_ip = htonl(dhcps_lease.end_ip.addr);
		/*config ip information can't contain local ip*/
		if ((start_ip <= softap_ip) && (softap_ip <= end_ip)) {
			dhcps_lease.enable = false;
		} else {
			/*config ip information must be in the same segment as the local ip*/
			softap_ip >>= 8;
			if (((start_ip >> 8 != softap_ip) || (end_ip >> 8 != softap_ip))
					|| (end_ip - start_ip > DHCPS_MAX_LEASE)) {
				dhcps_lease.enable = false;
			}
		}
	}

	if (dhcps_lease.enable == false) {
		local_ip = softap_ip = htonl(ip);
		softap_ip &= 0xFFFFFF00;
		local_ip &= 0xFF;
		if (local_ip >= 0x80)
			local_ip -= DHCPS_MAX_LEASE;
		else
			local_ip ++;

		bzero(&dhcps_lease, sizeof(dhcps_lease));
		dhcps_lease.start_ip.addr = softap_ip | local_ip;
		dhcps_lease.end_ip.addr = softap_ip | (local_ip + DHCPS_MAX_LEASE - 1);
		dhcps_lease.start_ip.addr = htonl(dhcps_lease.start_ip.addr);
		dhcps_lease.end_ip.addr= htonl(dhcps_lease.end_ip.addr);
	}
//	os_printf("start_ip = 0x%x, end_ip = 0x%x\n",dhcps_lease.start_ip, dhcps_lease.end_ip);
}


///////////////////////////////////////////////////////////////////////////////////
void dhcps_start(struct netif *netif)
{
	struct netif * apnetif =netif;


	if(apnetif->dhcps_pcb != NULL) {
            udp_remove(apnetif->dhcps_pcb);
       }
	pcb_dhcps = udp_new();
	if (pcb_dhcps == NULL) {
		printf("dhcps_start(): could not obtain pcb\n");
	}
	apnetif->dhcps_pcb = pcb_dhcps;

	IP4_ADDR(&broadcast_dhcps, 255, 255, 255, 255);

	server_address = netif->ip_addr.u_addr.ip4;
	wifi_softap_init_dhcps_lease( server_address.addr );
    
        client_address_plus.addr = dhcps_lease.start_ip.addr;
    
	udp_bind(pcb_dhcps, IP_ADDR_ANY, DHCPS_SERVER_PORT);
	udp_recv(pcb_dhcps, handle_dhcp, NULL);
#if DHCPS_DEBUG
	DHCPS_LOG("dhcps:dhcps_start->udp_recv function Set a receive callback handle_dhcp for UDP_PCB pcb_dhcps\n");
#endif
        DhcpsFlags = DHCPS_STARTED;

}

void dhcps_stop(struct netif *netif )
{
    struct netif * apnetif = netif;
    if(apnetif == NULL)
    {
        printf("dhcps_stop: apnetif == NULL\n");
        return;
    }
    udp_disconnect(pcb_dhcps);
//	dhcps_lease_flag = true;
    if(apnetif->dhcps_pcb != NULL) {
        udp_remove(apnetif->dhcps_pcb);
        apnetif->dhcps_pcb = NULL;
    }

    list_node *pnode = NULL;
    list_node *pback_node = NULL;
    pnode = plist;
    while (pnode != NULL) {
        pback_node = pnode;
        pnode = pback_node->pnext;
        node_remove_from_list(&plist, pback_node);
        free(pback_node->pnode);
        pback_node->pnode = NULL;
        free(pback_node);
        pback_node = NULL;
    }
    DhcpsFlags = DHCPS_STOP;
}

bool wifi_softap_set_dhcps_lease(struct dhcps_lease *please)
{

//  NOT USE
	struct ip_info info;
	u32_t softap_ip = 0;
	u32_t start_ip = 0;
	u32_t end_ip = 0;
    
	//uint8 opmode = wifi_get_opmode();
       //uint8 opmode = 0;
	//if (opmode == STATION_MODE || opmode == NULL_MODE) {
		//return false;
	//}
 	if (please == NULL || get_dhcps_status() == DHCPS_STARTED)
		return false;

	if(please->enable) {
              struct ip_info ip_info;
        	memset(&ip_info, 0x00, sizeof(struct ip_info));
              tcpip_adapter_get_ip_info(WIFI_IF_AP, &info);

              softap_ip = htonl(info.ip.addr);
        	start_ip = htonl(please->start_ip.addr);
        	end_ip = htonl(please->end_ip.addr);

        	/*config ip information can't contain local ip*/
        	if ((start_ip <= softap_ip) && (softap_ip <= end_ip))
        		return false;

        	/*config ip information must be in the same segment as the local ip*/
        	softap_ip >>= 8;
        	if ((start_ip >> 8 != softap_ip)
        			|| (end_ip >> 8 != softap_ip)) {
        		return false;
        	}

        	if (end_ip - start_ip > DHCPS_MAX_LEASE)
        		return false;

    	       memset(&dhcps_lease, 0x00, sizeof(dhcps_lease));
    //		dhcps_lease.start_ip.addr = start_ip;
    //		dhcps_lease.end_ip.addr = end_ip;
    		dhcps_lease.start_ip.addr = please->start_ip.addr;
    		dhcps_lease.end_ip.addr = please->end_ip.addr;
	}
	dhcps_lease.enable = please->enable;
//	dhcps_lease_flag = false;
	return true;
}

/******************************************************************************
 * FunctionName : wifi_softap_get_dhcps_lease
 * Description  : get the lease information of DHCP server
 * Parameters   : please -- Additional argument to get the lease information,
 * 							Little-Endian.
 * Returns      : true or false
*******************************************************************************/
bool wifi_softap_get_dhcps_lease(struct dhcps_lease *please)
{

	if (NULL == please)
		return false;
	if (dhcps_lease.enable == false){
		if (get_dhcps_status() == DHCPS_STOP)
			return false;
	} else {
	    ;
	}
	please->start_ip.addr = dhcps_lease.start_ip.addr;
	please->end_ip.addr = dhcps_lease.end_ip.addr;
	return true;
}

static void kill_oldest_dhcps_pool(void)
{
	list_node *pre = NULL, *p = NULL;
	list_node *minpre = NULL, *minp = NULL;
	struct dhcps_pool *pdhcps_pool = NULL, *pmin_pool = NULL;
	pre = plist;
	p = pre->pnext;
	minpre = pre;
	minp = p;
	while (p != NULL){
		pdhcps_pool = p->pnode;
		pmin_pool = minp->pnode;
		if (pdhcps_pool->lease_timer < pmin_pool->lease_timer){
			minp = p;
			minpre = pre;
		}
		pre = p;
		p = p->pnext;
	}
	minpre->pnext = minp->pnext;
	free(minp->pnode);
	minp->pnode = NULL;
	free(minp);
	minp = NULL;
}

void dhcps_coarse_tmr(void)
{
	u8_t num_dhcps_pool = 0;
	list_node *pback_node = NULL;
	list_node *pnode = NULL;
	struct dhcps_pool *pdhcps_pool = NULL;
	pnode = plist;
	while (pnode != NULL) {
		pdhcps_pool = pnode->pnode;
		pdhcps_pool->lease_timer --;
		if (pdhcps_pool->lease_timer == 0){
			pback_node = pnode;
			pnode = pback_node->pnext;
			node_remove_from_list(&plist,pback_node);
			free(pback_node->pnode);
			pback_node->pnode = NULL;
			free(pback_node);
			pback_node = NULL;
		} else {
			pnode = pnode ->pnext;
			num_dhcps_pool ++;
		}
	}

	if (num_dhcps_pool >= MAX_STATION_NUM)
		kill_oldest_dhcps_pool();
}

bool wifi_softap_set_dhcps_offer_option(u8_t level, void* optarg)
{
	bool offer_flag = true;
	u8_t option = 0;

	if (optarg == NULL && get_dhcps_status() == false)
		return false;

	if (level <= OFFER_START || level >= OFFER_END)
		return false;

	switch (level){
		case OFFER_ROUTER:
			offer = (*(u8_t *)optarg) & 0x01;
			offer_flag = true;
			break;
		default :
			offer_flag = false;
			break;
	}
	return offer_flag;
}

bool wifi_softap_set_dhcps_lease_time(u32_t minute)
{

    if (get_dhcps_status() == DHCPS_STARTED) {
        return false;
    }

    if(minute == 0) {
        return false;
    }
    dhcps_lease_time = minute;
    return true;
}

bool wifi_softap_reset_dhcps_lease_time(void)
{

    if (get_dhcps_status() == DHCPS_STARTED) {
        return false;
    }
    dhcps_lease_time = DHCPS_LEASE_TIME_DEF;
    return true;
}

u32_t wifi_softap_get_dhcps_lease_time(void) // minute
{
    return dhcps_lease_time;
}
#else
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "lwip/sys.h"
#include <netdb.h>
#include <errno.h>
static os_timer_t micros_overflow_timer;
static uint32_t micros_at_last_overflow_tick = 0;
static uint32_t micros_overflow_count = 0;

void micros_overflow_tick(void* arg)
{
    uint32_t m = system_get_time();
    if(m < micros_at_last_overflow_tick)
        ++micros_overflow_count;
    micros_at_last_overflow_tick = m;
}

unsigned long  millis()
{
    uint32_t m = system_get_time();
    uint32_t c = micros_overflow_count + ((m < micros_at_last_overflow_tick) ? 1 : 0);
    return c * 4294967 + m / 1000;
}

unsigned long  micros()
{
    return system_get_time();
}

void dhcps_set_default_time(void)
{
	os_timer_disarm(&micros_overflow_timer);
    os_timer_setfn(&micros_overflow_timer, (os_timer_func_t*) &micros_overflow_tick, 0);
    os_timer_arm(&micros_overflow_timer, 60000, 1);
}

time_t time(time_t * t)
{
    time_t seconds = millis();
    if (t)
    {
        *t = seconds;
    }
    return seconds;
}

/*
 * Initialize the binding list.
 */

void
dhcp_binding_list_init (binding_list *list)
{
    STAILQ_INIT(list);
}

/*
 * Create a new binding
 *
 * The binding is added to the binding list,
 * and a pointer to the binding is returned for further manipulations.
 */

address_binding *
dhcp_binding_add (binding_list *list, uint32_t address, uint8_t *cident, uint8_t cident_len, int is_static)
{
    // fill binding
    address_binding *binding = calloc(1, sizeof(*binding));

    binding->address = address;
    binding->cident_len = cident_len;
    memcpy(binding->cident, cident, cident_len);

    binding->is_static = is_static;

    // add to binding list
    STAILQ_INSERT_HEAD(list, binding, pointers);

    return binding;
}

/*
 * Updated bindings status, i.e. set to EXPIRED the status of the
 * expired bindings.
 */

void
dhcp_binding_statuses_update (binding_list *list)
{
    address_binding *binding, *binding_temp;

    STAILQ_FOREACH_SAFE(binding, list, pointers, binding_temp)
    {
        if(binding->binding_time + binding->lease_time < time(NULL))
        {
            binding->status = EXPIRED;
        }
    }
}

/*
 * Search a static or dynamic binding having the given client identifier.
 *
 * If the is_static option is true a static binding will be searched,
 * otherwise a dynamic one. If status is not zero, an binding with that
 * status will be searched.
 */

address_binding *
dhcp_binding_search (binding_list *list, uint8_t *cident, uint8_t cident_len, int is_static, int status)
{
    address_binding *binding, *binding_temp;

    STAILQ_FOREACH_SAFE(binding, list, pointers, binding_temp)
    {
        if((binding->is_static == is_static || is_static == STATIC_OR_DYNAMIC) &&
           binding->cident_len == cident_len &&
           memcmp(binding->cident, cident, cident_len) == 0)
        {
            if(status == 0)
                return binding;
            else if(status == binding->status)
                return binding;
        }
    }

    return NULL;
}

/*
 * Get an available free address
 *
 * If a zero address is returned, no more address are available.
 */

static uint32_t
dhcp_binding_take_free_address (pool_indexes *indexes)
{
    if(indexes->current <= indexes->last)
    {
        uint32_t address = indexes->current;
        indexes->current = htonl(ntohl(indexes->current) + 1);
        return address;

    }
    else
        return 0;
}

/*
 * Create a new dynamic binding or reuse an expired one.
 *
 * An attemp will be made to assign to the client the requested IP address
 * contained in the address option. An address equals to zero means that no
 * specific address has been requested.
 *
 * If the dynamic pool of addresses is full a NULL pointer will be returned.
 */

address_binding *
dhcp_binding_new_dynamic (binding_list *list, pool_indexes *indexes, uint32_t address, uint8_t *cident, uint8_t cident_len)
{
    address_binding *binding, *binding_temp;
    address_binding *found_binding = NULL;

    if (address != 0)
    {

        STAILQ_FOREACH_SAFE(binding, list, pointers, binding_temp)
        {
            // search a previous binding using the requested IP address

            if(binding->address == address)
            {
                found_binding = binding;
                break;
            }
        }
    }

    if(found_binding != NULL &&
       !found_binding->is_static &&
       found_binding->status != PENDING &&
       found_binding->status != ASSOCIATED)
    {

        // the requested IP address is available (reuse an expired association)
        return found_binding;

    }
    else
    {

        /* the requested IP address is already in use, or no address has been
               requested, or the address requested has never been allocated
               (we do not support this last case and just return the next
               available address!). */

        uint32_t address = dhcp_binding_take_free_address(indexes);

        if(address != 0)
            return dhcp_binding_add(list, address, cident, cident_len, 0);

        else   // search any previously assigned address which is expired
        {

            STAILQ_FOREACH_SAFE(binding, list, pointers, binding_temp)
            {
                if(!binding->is_static &&
                   found_binding->status != PENDING &&
                   found_binding->status != ASSOCIATED)
                    return binding;
            }

            // if executions reach here no more addresses are available
            return NULL;
        }
    }
}

/*
 * Delete an binding list and deallocate its memory.
 * Deallocate even the list elements.
 */

static void
dhcp_binding_list_delete (binding_list *list)
{
    address_binding *opt = STAILQ_FIRST(list);
    address_binding *tmp;

    while (opt != NULL)
    {
        tmp = STAILQ_NEXT(opt, pointers);
        free(opt);
        opt = tmp;
    }

    STAILQ_INIT(list);
}

/* Value parsing functions:
 *
 * Parse the string pointed by s, and allocate the
 * pointer p to contain the parsed data.
 *
 * On success return the size of the parsed data,
 * on error return zero.
 */

static  int dhcp_option_byte (char *s, void **p);
static  int dhcp_option_byte_list (char *s, void **p);
static  int dhcp_option_short (char *s, void **p);
static  int dhcp_option_short_list (char *s, void **p);
static  int dhcp_option_long (char *s, void **p);
static  int dhcp_option_string (char *s, void **p);
static  int dhcp_option_ip (char *s, void **p);
static  int dhcp_option_ip_list (char *s, void **p);

/* Global pool */
static  const uint8_t dhcp_option_magic[4] = {0x63, 0x82, 0x53, 0x63};
static address_pool dhcp_address_pool = {0};

/*
 * Mapping table between DHCP options and
 * functions that parse their value.
 */
static struct
{
    char *name;
    int (*f) (char *, void **);
} dhcp_option_info [256] =
{
    [PAD] { "PAD", NULL },
    [END] { "END", NULL },
    [SUBNET_MASK] { "SUBNET_MASK", dhcp_option_ip },
    [TIME_OFFSET] { "TIME_OFFSET", dhcp_option_long },
    [ROUTER] { "ROUTER", dhcp_option_ip_list },
    [TIME_SERVER] { "TIME_SERVER", dhcp_option_ip_list },
    [NAME_SERVER] { "NAME_SERVER", dhcp_option_ip_list },
    [DOMAIN_NAME_SERVER] { "DOMAIN_NAME_SERVER", dhcp_option_ip_list },
    [LOG_SERVER] { "LOG_SERVER", dhcp_option_ip_list },
    [COOKIE_SERVER] { "COOKIE_SERVER", dhcp_option_ip_list },
    [LPR_SERVER] { "LPR_SERVER", dhcp_option_ip_list },
    [IMPRESS_SERVER] { "IMPRESS_SERVER", dhcp_option_ip_list },
    [RESOURCE_LOCATION_SERVER] { "RESOURCE_LOCATION_SERVER", dhcp_option_ip_list },
    [HOST_NAME] { "HOST_NAME", dhcp_option_string },
    [BOOT_FILE_SIZE] { "BOOT_FILE_SIZE", dhcp_option_short },
    [MERIT_DUMP_FILE] { "MERIT_DUMP_FILE", dhcp_option_string },
    [DOMAIN_NAME] { "DOMAIN_NAME", dhcp_option_string },
    [SWAP_SERVER] { "SWAP_SERVER", dhcp_option_ip },
    [ROOT_PATH] { "ROOT_PATH", dhcp_option_string },
    [EXTENSIONS_PATH] { "EXTENSIONS_PATH", dhcp_option_string },
    [IP_FORWARDING] { "IP_FORWARDING", dhcp_option_byte },
    [NON_LOCAL_SOURCE_ROUTING] { "NON_LOCAL_SOURCE_ROUTING", dhcp_option_byte },
    [POLICY_FILTER] { "POLICY_FILTER", dhcp_option_ip_list },
    [MAXIMUM_DATAGRAM_REASSEMBLY_SIZE] { "MAXIMUM_DATAGRAM_REASSEMBLY_SIZE", dhcp_option_short },
    [DEFAULT_IP_TIME_TO_LIVE] { "DEFAULT_IP_TIME_TO_LIVE", dhcp_option_byte },
    [PATH_MTU_AGING_TIMEOUT] { "PATH_MTU_AGING_TIMEOUT", dhcp_option_long },
    [PATH_MTU_PLATEAU_TABLE] { "PATH_MTU_PLATEAU_TABLE", dhcp_option_short_list },
    [INTERFACE_MTU] { "INTERFACE_MTU", dhcp_option_short },
    [ALL_SUBNETS_ARE_LOCAL] { "ALL_SUBNETS_ARE_LOCAL", dhcp_option_byte },
    [BROADCAST_ADDRESS] { "BROADCAST_ADDRESS", dhcp_option_ip },
    [PERFORM_MASK_DISCOVERY] { "PERFORM_MASK_DISCOVERY", dhcp_option_byte },
    [MASK_SUPPLIER] { "MASK_SUPPLIER", dhcp_option_byte },
    [PERFORM_ROUTER_DISCOVERY] { "PERFORM_ROUTER_DISCOVERY", dhcp_option_byte },
    [ROUTER_SOLICITATION_ADDRESS] { "ROUTER_SOLICITATION_ADDRESS", dhcp_option_ip },
    [STATIC_ROUTE] { "STATIC_ROUTE", dhcp_option_ip_list },
    [TRAILER_ENCAPSULATION] { "TRAILER_ENCAPSULATION", dhcp_option_byte },
    [ARP_CACHE_TIMEOUT] { "ARP_CACHE_TIMEOUT", dhcp_option_long },
    [ETHERNET_ENCAPSULATION] { "ETHERNET_ENCAPSULATION", dhcp_option_byte },
    [TCP_DEFAULT_TTL] { "TCP_DEFAULT_TTL", dhcp_option_byte },
    [TCP_KEEPALIVE_INTERVAL] { "TCP_KEEPALIVE_INTERVAL", dhcp_option_long },
    [TCP_KEEPALIVE_GARBAGE] { "TCP_KEEPALIVE_GARBAGE", dhcp_option_byte },
    [NETWORK_INFORMATION_SERVICE_DOMAIN] { "NETWORK_INFORMATION_SERVICE_DOMAIN", dhcp_option_string },
    [NETWORK_INFORMATION_SERVERS] { "NETWORK_INFORMATION_SERVERS", dhcp_option_ip_list },
    [NETWORK_TIME_PROTOCOL_SERVERS] { "NETWORK_TIME_PROTOCOL_SERVERS", dhcp_option_ip_list },
    [VENDOR_SPECIFIC_INFORMATION] { "VENDOR_SPECIFIC_INFORMATION", dhcp_option_byte_list },
    [NETBIOS_OVER_TCP_IP_NAME_SERVER] { "NETBIOS_OVER_TCP_IP_NAME_SERVER", dhcp_option_ip_list },
    [NETBIOS_OVER_TCP_IP_DATAGRAM_DISTRIBUTION_SERVER] { "NETBIOS_OVER_TCP_IP_DATAGRAM_DISTRIBUTION_SERVER", dhcp_option_ip_list },
    [NETBIOS_OVER_TCP_IP_NODE_TYPE] { "NETBIOS_OVER_TCP_IP_NODE_TYPE", dhcp_option_byte },
    [NETBIOS_OVER_TCP_IP_SCOPE] { "NETBIOS_OVER_TCP_IP_SCOPE", dhcp_option_string },
    [X_WINDOW_SYSTEM_FONT_SERVER] { "X_WINDOW_SYSTEM_FONT_SERVER", dhcp_option_ip_list },
    [X_WINDOW_SYSTEM_DISPLAY_MANAGER] { "X_WINDOW_SYSTEM_DISPLAY_MANAGER", dhcp_option_ip_list },
    [NETWORK_INFORMATION_SERVICE_PLUS_DOMAIN] { "NETWORK_INFORMATION_SERVICE_PLUS_DOMAIN", dhcp_option_string },
    [NETWORK_INFORMATION_SERVICE_PLUS_SERVERS] { "NETWORK_INFORMATION_SERVICE_PLUS_SERVERS", dhcp_option_ip_list },
    [MOBILE_IP_HOME_AGENT] { "MOBILE_IP_HOME_AGENT", dhcp_option_ip_list },
    [SMTP_SERVER] { "SMTP_SERVER", dhcp_option_ip_list },
    [POP3_SERVER] { "POP3_SERVER", dhcp_option_ip_list },
    [NNTP_SERVER] { "NNTP_SERVER", dhcp_option_ip_list },
    [DEFAULT_WWW_SERVER] { "DEFAULT_WWW_SERVER", dhcp_option_ip_list },
    [DEFAULT_FINGER_SERVER] { "DEFAULT_FINGER_SERVER", dhcp_option_ip_list },
    [DEFAULT_IRC_SERVER] { "DEFAULT_IRC_SERVER", dhcp_option_ip_list },
    [STREETTALK_SERVER] { "STREETTALK_SERVER", dhcp_option_ip_list },
    [STREETTALK_DIRECTORY_ASSISTANCE_SERVER] { "STREETTALK_DIRECTORY_ASSISTANCE_SERVER",  dhcp_option_ip_list },
    [REQUESTED_IP_ADDRESS] { "REQUESTED_IP_ADDRESS", NULL },
    [IP_ADDRESS_LEASE_TIME] { "IP_ADDRESS_LEASE_TIME", dhcp_option_long },
    [OPTION_OVERLOAD] { "OPTION_OVERLOAD", dhcp_option_byte },
    [TFTP_SERVER_NAME] { "TFTP_SERVER_NAME", dhcp_option_string },
    [BOOTFILE_NAME] { "BOOTFILE_NAME", dhcp_option_string },
    [DHCP_MESSAGE_TYPE] { "DHCP_MESSAGE_TYPE", NULL },
    [SERVER_IDENTIFIER] { "SERVER_IDENTIFIER", dhcp_option_ip },
    [PARAMETER_REQUEST_LIST] { "PARAMETER_REQUEST_LIST", NULL },
    [MESSAGE] { "MESSAGE", NULL },
    [MAXIMUM_DHCP_MESSAGE_SIZE] { "MAXIMUM_DHCP_MESSAGE_SIZE", NULL },
    [RENEWAL_T1_TIME_VALUE] { "RENEWAL_T1_TIME_VALUE", dhcp_option_long },
    [REBINDING_T2_TIME_VALUE] { "REBINDING_T2_TIME_VALUE", dhcp_option_long },
    [VENDOR_CLASS_IDENTIFIER] { "VENDOR_CLASS_IDENTIFIER", NULL },
    [CLIENT_IDENTIFIER] { "CLIENT_IDENTIFIER", NULL },

    [USER_CLASS] { "USER_CLASS", NULL },
    [FQDN] { "FQDN", NULL },
    [DHCP_AGENT_OPTIONS] { "DHCP_AGENT_OPTIONS", NULL },
    [NDS_SERVERS] { "NDS_SERVERS", NULL },
    [NDS_TREE_NAME] { "NDS_TREE_NAME", NULL },
    [NDS_CONTEXT] { "NDS_CONTEXT", NULL },
    [CLIENT_LAST_TRANSACTION_TIME] { "CLIENT_LAST_TRANSACTION_TIME", NULL },
    [ASSOCIATED_IP] { "ASSOCIATED_IP", NULL },
    [USER_AUTHENTICATION_PROTOCOL] { "USER_AUTHENTICATION_PROTOCOL", NULL },
    [AUTO_CONFIGURE] { "AUTO_CONFIGURE", NULL },
    [NAME_SERVICE_SEARCH] { "NAME_SERVICE_SEARCH", dhcp_option_string },
    [SUBNET_SELECTION] { "SUBNET_SELECTION", NULL },
    [DOMAIN_SEARCH] { "DOMAIN_SEARCH", dhcp_option_string },
    [CLASSLESS_ROUTE] { "CLASSLESS_ROUTE", dhcp_option_string },
};

/* Value parsing functions */

static int
dhcp_option_byte (char *s, void **p)
{
    *p = malloc(sizeof(uint8_t));
    uint8_t n = ((uint8_t) strtol(s, NULL, 0));
    memcpy(*p, &n, sizeof(n));

    return sizeof(uint8_t);
}

static int
dhcp_option_byte_list (char *s, void **p)
{
    *p = malloc(strlen(s) * sizeof(uint8_t)); // slightly over the strictly requested size

    int count = 0;

    char *s2 = strdup(s);
    char *s3 = strtok(s2, ", ");

    while(s3 != NULL)
    {

        uint8_t n = ((uint8_t) strtol(s3, NULL, 0));

        memcpy(((uint8_t *) *p) + count, &n, sizeof(uint8_t));

        count += sizeof(uint8_t);
        s3 = strtok(NULL, " ");
    }

    free(s2);

    return count;
}

static int
dhcp_option_short (char *s, void **p)
{
    *p = malloc(sizeof(uint16_t));
    uint16_t n = ((uint16_t) strtol(s, NULL, 0));
    memcpy(*p, &n, sizeof(n));

    return sizeof(uint16_t);
}

static int
dhcp_option_short_list (char *s, void **p)
{
    *p = malloc(strlen(s) * sizeof(uint16_t)); // slightly over the strictly requested size

    int count = 0;

    char *s2 = strdup(s);
    char *s3 = strtok(s2, ", ");

    while(s3 != NULL)
    {

        uint16_t n = ((uint16_t) strtol(s3, NULL, 0));

        memcpy(((uint8_t *) *p) + count, &n, sizeof(uint16_t));

        count += sizeof(uint16_t);
        s3 = strtok(NULL, " ");
    }

    free(s2);

    return count;
}

static int
dhcp_option_long (char *s, void **p)
{
    *p = malloc(sizeof(uint32_t));
    uint32_t n = strtol(s, NULL, 0);
    memcpy(*p, &n, sizeof(n));

    return sizeof(uint32_t);
}

static int
dhcp_option_string (char *s, void **p)
{
    *p = strdup(s);

    return strlen(s);
}

static int
dhcp_option_ip (char *s, void **p)
{
    struct sockaddr_in ip;

    *p = malloc(sizeof(uint32_t));

    if (inet_aton(s, &ip.sin_addr) == 0)   // error: invalid IP address
    {
        free(*p);
        return 0;
    }

    memcpy(*p, &ip.sin_addr, sizeof(uint32_t));

    return sizeof(uint32_t);
}

static int
dhcp_option_ip_list (char *s, void **p)
{
    *p = malloc(strlen(s) * sizeof(uint32_t) / 4); // slightly over the strictly required size

    int count = 0;

    char *s2 = strdup(s);
    char *s3 = strtok(s2, ", ");

    while(s3 != NULL)
    {
        struct sockaddr_in ip;

        if (inet_aton(s3, &ip.sin_addr) == 0)   // error: invalid IP address
        {
            free(*p);
            return 0;
        }

        memcpy(((uint8_t *) *p) + count, &ip.sin_addr, sizeof(uint32_t));

        count += sizeof(uint32_t);
        s3 = strtok(NULL, " ");
    }

    free(s2);

    return count;
}

/* Option-related functions */

/*
 * Given the name of the option and its value as strings,
 * fill the dhcp_option structure pointed by opt.
 *
 * On success return the parsed option id,
 * otherwise return zero.
 */
static uint8_t
dhcp_option_parse (dhcp_option *opt, char *name, char *value)
{
    int (*f) (char *, void **);
    int id;

    uint8_t len;
    uint8_t *p;

    for (id = 0; id < 256; id++)   // search the option by name
    {
        if (dhcp_option_info[id].name &&
            strcmp(dhcp_option_info[id].name, name) == 0) break;
    }

    if (id == 256)   // not found
    {
        log_info("Unsupported DHCP option '%s'", name);
        return 0;
    }

    f = dhcp_option_info[id].f;

    if (f == NULL)   // no parsing function available
    {
        log_info("Unsupported DHCP option '%s'", name);
        return 0;
    }

    len = f(value, (void **)&p); // parse the value

    if(len == 0) // error parsing the value
        return 0;

    // structure filling
    opt->id = id;
    opt->len = len;
    memcpy(opt->data, p, len);

    free(p);

    return opt->id;
}

/*
 * Initialize an option list.
 */

static void
dhcp_option_list_init (dhcp_option_list *list)
{
    STAILQ_INIT(list);
}

/*
 * Given a list of options search an option having
 * the passed option id, and returns a pointer to it.
 *
 * If the option is not present the function returns NULL.
 */

static dhcp_option *
dhcp_option_search (dhcp_option_list *list, uint8_t id)
{
    dhcp_option *opt, *opt_temp;

    STAILQ_FOREACH_SAFE(opt, list, pointers, opt_temp)
    {

        if(opt->id == id)
            return opt;

    }

    return NULL;
}

/*
 * Print options in list.
 */

static void
dhcp_option_print (dhcp_option_list *list)
{
    dhcp_option *opt, *opt_temp;
    int i=0;

    STAILQ_FOREACH_SAFE(opt, list, pointers, opt_temp)
    {

        printf("options[%d]=%d (%s)\n", i++, opt->id,
               dhcp_option_info[opt->id].name);

    }
}


/*
 * Append the provided option to the list.
 *
 * Always allocate new memory, that must be freed later...
 */

static void
dhcp_option_append (dhcp_option_list *list, dhcp_option *opt)
{
    dhcp_option *nopt = calloc(1, sizeof(*nopt));
    memcpy(nopt, opt, 2 + opt->len);

    STAILQ_INSERT_TAIL(list, nopt, pointers);
}

/*
 * Parse the options contained in a DHCP message into a list.
 *
 * Return 1 on success, 0 if the options are malformed.
 */

static int
dhcp_option_parse_to_list (dhcp_option_list *list, dhcp_option *opts, size_t len)
{
    dhcp_option *opt, *end;

    opt = opts;
    end = (dhcp_option *)(((uint8_t *)opts) + len);

    if (len < 4 ||
        memcmp(opt, dhcp_option_magic, sizeof(dhcp_option_magic)) != 0)
        return 0;

    opt = (dhcp_option *)(((uint8_t *) opt) + 4);

    while (opt < end  &&
           opt->id != END)   // TODO: check also valid option sizes
    {

        if ((dhcp_option *)(((uint8_t *) opt) + 2 + opt->len) >= end)
            return 0; // the len field is too long

        dhcp_option_append(list, opt);

        opt = (dhcp_option *)(((uint8_t *) opt) + 2 + opt->len);
    }

    if (opt < end && opt->id == END)
        return 1;

    return 0;
}

/*
 * Serialize a list of options, to be inserted directly inside
 * the options section of a DHCP message.
 *
 * Return 0 on error, the total serialized len on success.
 */

static size_t
dhcp_option_list_serialize (dhcp_option_list *list, uint8_t *buf, size_t len)
{
    uint8_t *p = buf;

    if (len < 4)
        return 0;

    memcpy(p, dhcp_option_magic, sizeof(dhcp_option_magic));
    p += 4;
    len -= 4;

    dhcp_option *opt, *opt_temp;

    STAILQ_FOREACH_SAFE(opt, list, pointers, opt_temp)
    {

        if (len <= 2 + opt->len)
            return 0;

        memcpy(p, opt, 2 + opt->len);
        p   += 2 + opt->len;
        len -= 2 + opt->len;

    }

    if (len < 1)
        return 0;

    *p = END;

    p++;
    len--;

    return p - buf;
}

/*
 * Delete an option list and deallocate its memory.
 * Deallocate even the list elements.
 */

static void
dhcp_option_list_delete (dhcp_option_list *list)
{
    dhcp_option *opt = STAILQ_FIRST(list);
    dhcp_option *tmp;

    while (opt != NULL)
    {
        tmp = STAILQ_NEXT(opt, pointers);
        free(opt);
        opt = tmp;
    }

    STAILQ_INIT(list);
}

static void dhcp_options_default_fill(dhcp_option_list *list, dhcp_option_list *reply_opts)
{
    dhcp_option *opt, *opt_temp;
    int i = 0;

    STAILQ_FOREACH_SAFE(opt, list, pointers, opt_temp)
    {
        log_info("options[%d]=%d (%s)\n", i++, opt->id, dhcp_option_info[opt->id].name);

        if (opt != NULL)
            dhcp_option_append(reply_opts, opt);
    }
}

static void
dhcp_options_requested_fill (dhcp_option *requested_opts, dhcp_option_list *reply_opts)
{
    uint8_t len = requested_opts->len;
    uint8_t *id = requested_opts->data;

    int i = 0;
    for (i = 0; i < len; i++)
    {
        if(id[i] != 0)
        {
            dhcp_option *opt = dhcp_option_search(&dhcp_address_pool.options, id[i]);

            if(opt != NULL)
                dhcp_option_append(reply_opts, opt);
        }
    }
    dhcp_option_print(reply_opts);
}

static bool dhcp_options_add(char* name, char* value)
{
    uint8_t id = 0;
    bool flags = true;
    REQUIRE_ACTION(name, add_error, flags = false);
    REQUIRE_ACTION(value, add_error, flags = false);
    dhcp_option *option = calloc(1, sizeof(*option));
    REQUIRE_ACTION(option, add_error, flags = false);
    id = dhcp_option_parse(option, name, value);
    if (id == 0)
    {
        log_info( "error: invalid dhcp option specified: %s,%s",name, value);
        REQUIRE_ACTION(option, add_error, flags = false);
    }

    dhcp_option_append(&dhcp_address_pool.options, option);

    if(option->id == IP_ADDRESS_LEASE_TIME)
        dhcp_address_pool.lease_time = ntohl(*((uint32_t *)option->data));

add_error:
    free(option);
    return flags;
}

/*
 * Message handling routines.
 */

static uint8_t dhcp_request_expand (dhcps_msg *request, size_t len)
{
    dhcp_option_list_init(&request->opts);

    if (request->hdr.hlen < 1 || request->hdr.hlen > 16)
        return 0;

    if (dhcp_option_parse_to_list(&request->opts, (dhcp_option *)request->hdr.options, len - DHCP_HEADER_SIZE) == 0)
        return 0;

    dhcp_option *type_opt = dhcp_option_search(&request->opts, DHCP_MESSAGE_TYPE);

    if (type_opt == NULL)
        return 0;

    uint8_t type = type_opt->data[0];

    return type;
}

static int
dhcp_reply_init (dhcps_msg *request, dhcps_msg *reply)
{
    memset(&reply->hdr, 0, sizeof(reply->hdr));

    dhcp_option_list_init(&reply->opts);

    reply->hdr.op = BOOTREPLY;

    reply->hdr.htype = request->hdr.htype;
    reply->hdr.hlen  = request->hdr.hlen;

    reply->hdr.xid   = request->hdr.xid;
    reply->hdr.flags = request->hdr.flags;

    reply->hdr.giaddr = request->hdr.giaddr;

    memcpy(reply->hdr.chaddr, request->hdr.chaddr, request->hdr.hlen);

    return 1;
}

static int dhcp_reply_send(struct udp_pcb *pcb, ip_addr_t *addr, dhcps_msg *reply)
{
    size_t len = 0, ret = 0;
    struct pbuf *p = NULL, *q = NULL;
    u8_t *data = NULL;
    u16_t cnt = 0;
    u16_t i = 0;

    len = dhcp_option_list_serialize(&reply->opts, reply->hdr.options, sizeof(reply->hdr) - DHCP_HEADER_SIZE);
    len += DHCP_HEADER_SIZE;

    dhcp_option *type_opt = dhcp_option_search(&reply->opts, DHCP_MESSAGE_TYPE);
    if (type_opt == NULL)
        return -1;

//    if (type_opt->data[0] == DHCP_OFFER)
    addr->u_addr.ip4.addr = INADDR_BROADCAST;
//    else
//        ip4_addr_set(ip_2_ip4(addr), &reply->hdr.yiaddr); // use the address assigned by us

    if (reply->hdr.yiaddr.addr != 0)
    {
        log_info("send_dhcp_reply %s\n", inet_ntoa(*addr));
    }

    p = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);

    if (p != NULL)
    {
        q = p;
        while (q != NULL)
        {
            data = (u8_t *)q->payload;
            for (i = 0; i< q->len; i++)
            {
                data[i] = ((u8_t *) reply)[cnt++];
            }

            q = q->next;
        }
    }
    else
    {
        return 0;
    }

    ret = udp_sendto(pcb, p, addr, BOOTPC);
    log_info("dhcp_send %d %d\n", ret, p->ref);
    if(p->ref != 0)
    {
        pbuf_free(p);
    }

    return ret;
}

static int
dhcp_reply_fill (dhcps_msg *request, dhcps_msg *reply,
                 address_binding *binding, uint8_t type)
{
    static dhcp_option type_opt, server_id_opt;

    type_opt.id = DHCP_MESSAGE_TYPE;
    type_opt.len = 1;
    type_opt.data[0] = type;
    dhcp_option_append(&reply->opts, &type_opt);

    server_id_opt.id = SERVER_IDENTIFIER;
    server_id_opt.len = 4;
    memcpy(server_id_opt.data, &dhcp_address_pool.server_id, sizeof(dhcp_address_pool.server_id));
    dhcp_option_append(&reply->opts, &server_id_opt);

    if(binding != NULL)
    {
        reply->hdr.yiaddr.addr = binding->address;
    }

    if (type != DHCPS_NAK)
    {
        dhcp_option *requested_opts = dhcp_option_search(&request->opts, PARAMETER_REQUEST_LIST);

        if (requested_opts)
            dhcp_options_default_fill(&dhcp_address_pool.options, &reply->opts);
    }

    return type;
}

static int
dhcp_discover (dhcps_msg *request, dhcps_msg *reply)
{
    address_binding *binding = NULL;
    binding = dhcp_binding_search(&dhcp_address_pool.bindings, request->hdr.chaddr, request->hdr.hlen, STATIC, EMPTY);

    if (binding)
    {
        /* a static binding has been configured for this client */
        log_info("%s %d %p",__FILE__, __LINE__, binding);
    }
    else
    {
        /* use dynamic pool */
        /* If an address is available, the new address SHOULD be chosen as follows: */
        binding = dhcp_binding_search(&dhcp_address_pool.bindings, request->hdr.chaddr,request->hdr.hlen, DYNAMIC, EMPTY);

        if (binding)
        {
            /* The client's current address as recorded in the client's current
               binding, ELSE */

            /* The client's previous address as recorded in the client's (now
               expired or released) binding, if that address is in the server's
               pool of available addresses and not already allocated, ELSE */
            log_info("%s %d %p",__FILE__, __LINE__, binding);
        }
        else
        {
            /* The address requested in the 'Requested IP Address' option, if that
               address is valid and not already allocated, ELSE */

            /* A new address allocated from the server's pool of available
               addresses; the address is selected based on the subnet from which
               the message was received (if 'giaddr' is 0) or on the address of
               the relay agent that forwarded the message ('giaddr' when not 0). */

            /* extract requested IP address */
            uint32_t address = 0;
            dhcp_option *address_opt = dhcp_option_search(&request->opts, REQUESTED_IP_ADDRESS);

            if (address_opt != NULL)
                memcpy(&address, address_opt->data, sizeof(address));

            binding = dhcp_binding_new_dynamic(&dhcp_address_pool.bindings, &dhcp_address_pool.indexes, address, request->hdr.chaddr, request->hdr.hlen);

            if (binding == NULL)
            {
                log_info("Can not offer an address, no address available.");
                return 0;
            }
        }
    }

    if (binding->binding_time + binding->lease_time < time(NULL))
    {
        log_info("%s %d %p",__FILE__, __LINE__, binding);
        binding->status = PENDING;
        binding->binding_time = time(NULL);
        binding->lease_time = dhcp_address_pool.pending_time;
    }

    return dhcp_reply_fill(request, reply, binding, DHCPS_OFFER);
}

static int
dhcp_request (dhcps_msg *request, dhcps_msg *reply)
{
    address_binding *binding = dhcp_binding_search(&dhcp_address_pool.bindings, request->hdr.chaddr, request->hdr.hlen, STATIC_OR_DYNAMIC, PENDING);

    uint32_t server_id = 0;
    dhcp_option *server_id_opt = dhcp_option_search(&request->opts, SERVER_IDENTIFIER);

    if (server_id_opt != NULL)
        memcpy(&server_id, server_id_opt->data, sizeof(server_id));

    if (server_id == dhcp_address_pool.server_id)
    {
        /* this request is an answer to our offer */
        if (binding != NULL)
        {
            log_info("Ack, associated");

            binding->status = ASSOCIATED;
            binding->lease_time = dhcp_address_pool.lease_time;

            return dhcp_reply_fill(request, reply, binding, DHCPS_ACK);
        }
        else
        {
            log_info("Nak, not associated");

            return dhcp_reply_fill(request, reply, NULL, DHCPS_NAK);
        }

    }
    else if (server_id != 0)
    {
        /* this request is an answer to another offer */
        
        binding->status = EMPTY;
        binding->lease_time = 0;

        return 0;
    }

    return 0;
}

static int
dhcp_decline (dhcps_msg *request, dhcps_msg *reply)
{
    address_binding *binding = NULL;
    binding = dhcp_binding_search(&dhcp_address_pool.bindings, request->hdr.chaddr, request->hdr.hlen, STATIC_OR_DYNAMIC, PENDING);
    if(binding != NULL)
    {
        binding->status = EMPTY;
    }

    return 0;
}

static int
dhcp_release (dhcps_msg *request, dhcps_msg *reply)
{
    address_binding *binding = NULL;
    binding = dhcp_binding_search(&dhcp_address_pool.bindings, request->hdr.chaddr, request->hdr.hlen, STATIC_OR_DYNAMIC, ASSOCIATED);
    if(binding != NULL)
    {
        binding->status = RELEASED;
    }

    return 0;
}

static int
dhcp_inform (dhcps_msg *request, dhcps_msg *reply)
{
    return dhcp_reply_fill(request, reply, NULL, DHCPS_ACK);
}

/**
 * If an incoming DHCP message is in response to us, then trigger the state machine.
 *
 * Dispatch client DHCP messages to the correct handling routines
 *
 */
static void dhcp_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    struct netif *netif = ip_current_input_netif();

    struct pbuf *pthis = NULL;

    LWIP_UNUSED_ARG(arg);
    LWIP_ASSERT("invalid server address type", !IP_IS_V6(addr));
    /* prevent warnings about unused arguments */
    LWIP_UNUSED_ARG(pcb);
    LWIP_UNUSED_ARG(addr);
    LWIP_UNUSED_ARG(port);
    uint8_t type = 0;
    dhcps_msg *request = calloc(1, sizeof(dhcps_msg));
    REQUIRE_ACTION(request, free_pbuf_and_return, 0);
    dhcps_msg *reply = calloc(1, sizeof(dhcps_msg));
    REQUIRE_ACTION(reply, free_pbuf_and_return, 0);
    size_t len = 0;
    len = pbuf_copy_partial(p, &request->hdr, p->tot_len, 0);
    if (len < DHCP_HEADER_SIZE + 5)
    {
        goto free_pbuf_and_return;
    }
    else
    {
        if (request->hdr.op != BOOTREQUEST)
        {
            goto free_pbuf_and_return;
        }
        else
        {
            type = dhcp_request_expand(request, len);
            dhcp_reply_init(request, reply);
            switch (type)
            {
                case DHCPS_DISCOVER:
                    type = dhcp_discover(request, reply);
                    break;

                case DHCPS_REQUEST:
                    type = dhcp_request(request, reply);
                    break;

                case DHCPS_DECLINE:
                    type = dhcp_decline(request, reply);
                    break;

                case DHCPS_RELEASE:
                    type = dhcp_release(request, reply);
                    break;

                case DHCPS_INFORM:
                    type = dhcp_inform(request, reply);
                    break;

                default:
                    log_info("%s.%u: request with invalid DHCP message type option",inet_ntoa(addr), port);
                    break;
            }

            if (type != DHCP_NONE)
                dhcp_reply_send(pcb, (ip_addr_t *)addr, reply);

            dhcp_option_list_delete(&request->opts);
            dhcp_option_list_delete(&reply->opts);
        }
    }

free_pbuf_and_return:
    if (request != NULL)
        free(request);

    if (reply != NULL)
        free(reply);
    pbuf_free(p);
}

void dhcps_set_default_option(struct dhcps_lease *pool_addr)
{
    ip4_addr_t server_ip, broadcast, dns;
    REQUIRE_ASSERT(pool_addr);
	address_pool *dhcps_addr_pool = &dhcp_address_pool; 

	if (dhcps_addr_pool->flags){
	
	} else{
		dhcp_option_list_init(&dhcps_addr_pool->options);
		dhcps_addr_pool->flags = true;
	}

	/* Load configuration */
    dhcps_option_set(IP_ADDRESS_LEASE_TIME, "36000");
    dhcps_option_set(SUBNET_MASK, inet_ntoa(pool_addr->net_mask));

    server_ip.addr = dhcps_addr_pool->server_id;
    dhcps_option_set(ROUTER, inet_ntoa(server_ip));

    broadcast.addr = server_ip.addr | ~(pool_addr->net_mask.addr);
    dhcps_option_set(BROADCAST_ADDRESS,inet_ntoa(broadcast));

    dns.addr = DHCP_SERVER_OPENDNS;
    dhcps_option_set(DOMAIN_NAME_SERVER, inet_ntoa(dns));
}

static void dhcps_set_default_binding(address_pool *pool_addr)
{
	REQUIRE_ASSERT(pool_addr);
	dhcp_binding_list_init(&pool_addr->bindings);
}

static address_pool* dhcps_try_open_socket(address_pool *pool_addr)
{
    REQUIRE_ASSERT(pool_addr);
    pool_addr->socket = udp_new();
    REQUIRE_ASSERT(pool_addr->socket);
    udp_bind(pool_addr->socket, IP_ADDR_ANY, BOOTPS);
    udp_recv(pool_addr->socket, dhcp_recv, pool_addr);
    return pool_addr;
}

void dhcps_start(struct netif *netif, struct dhcps_lease *lease_pool)
{
    REQUIRE_ASSERT(netif);
    REQUIRE_ASSERT(lease_pool);
	
	dhcp_address_pool.server_id = netif->ip_addr.u_addr.ip4.addr;    	
    dhcps_set_default_time();
	dhcps_set_default_binding(&dhcp_address_pool);
    dhcps_set_default_option(lease_pool);    
    dhcps_try_open_socket(&dhcp_address_pool);
}

void dhcps_stop(struct netif *netif )
{
    dhcp_binding_list_delete(&dhcp_address_pool.bindings);
    dhcp_option_list_delete(&dhcp_address_pool.options);
    udp_remove(dhcp_address_pool.socket);
	dhcp_address_pool.flags = false;
//	memset(&dhcp_address_pool, 0, sizeof(address_pool));
}

bool dhcps_lease_set(struct dhcps_lease *please)
{    
	REQUIRE_ASSERT(please);
	    	
    dhcp_address_pool.indexes.first   = please->start_ip.addr;
    dhcp_address_pool.indexes.last    = please->end_ip.addr;
    dhcp_address_pool.indexes.current = please->start_ip.addr;
    
    return true;
}

bool dhcps_lease_get(struct dhcps_lease *please)
{
	REQUIRE_ASSERT(please);
	please->start_ip.addr = dhcp_address_pool.indexes.first;
    please->end_ip.addr = dhcp_address_pool.indexes.last;
    please->net_mask.addr = inet_addr("255.255.255.0");
	return true;
}

bool dhcps_option_set(u8_t opt_id, void* optarg)
{
	if (dhcp_address_pool.flags){
		dhcp_option *opt = dhcp_option_search(&dhcp_address_pool.options, opt_id);
	    if (opt)
	    {
	        opt->len = strlen(optarg);
	        memset(opt->data, 0, sizeof(opt->data));
	        memcpy(opt->data, optarg, opt->len);
	        return true;
	    }
	    else
	    {
	        return dhcp_options_add(dhcp_option_info[opt_id].name, optarg);
	    }
	} else{
		dhcp_option_list_init(&dhcp_address_pool.options);
		dhcp_address_pool.flags = true;
		return dhcp_options_add(dhcp_option_info[opt_id].name, optarg);
	}   
}

bool wifi_softap_set_dhcps_lease(struct dhcps_lease *please)
{
    return false;
}

bool wifi_softap_set_dhcps_lease_time(u32_t minute)
{
    return false;
}

#endif   /*ifdef  LWIP_ESP8266*/





