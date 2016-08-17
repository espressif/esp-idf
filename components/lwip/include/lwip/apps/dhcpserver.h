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
#ifndef __DHCPS_H__
#define __DHCPS_H__

#include "lwip/ip_addr.h"
//#include "esp_common.h"
#define USE_DNS
/* Here for now until needed in other places in lwIP */
#ifndef isprint
#define in_range(c, lo, up)  ((u8_t)c >= lo && (u8_t)c <= up)
#define isprint(c)           in_range(c, 0x20, 0x7f)
#define isdigit(c)           in_range(c, '0', '9')
#define isxdigit(c)          (isdigit(c) || in_range(c, 'a', 'f') || in_range(c, 'A', 'F'))
#define islower(c)           in_range(c, 'a', 'z')
#define isspace(c)           (c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v')
#endif

#ifdef LWIP_ESP8266

typedef struct dhcps_state{
        s16_t state;
} dhcps_state;

typedef struct dhcps_msg {
        u8_t op, htype, hlen, hops;
        u8_t xid[4];
        u16_t secs, flags;
        u8_t ciaddr[4];
        u8_t yiaddr[4];
        u8_t siaddr[4];
        u8_t giaddr[4];
        u8_t chaddr[16];
        u8_t sname[64];
        u8_t file[128];
        u8_t options[312];
}dhcps_msg;

struct dhcps_lease {
	bool enable;
	ip4_addr_t start_ip;
	ip4_addr_t end_ip;
};

enum dhcps_offer_option{
	OFFER_START = 0x00,
	OFFER_ROUTER = 0x01,
	OFFER_END
};

struct dhcps_pool{
	ip4_addr_t ip;
	u8_t mac[6];
	u32_t lease_timer;
};

typedef struct _list_node{
	void *pnode;
	struct _list_node *pnext;
}list_node;

extern u32_t dhcps_lease_time;
#define DHCPS_LEASE_TIMER  dhcps_lease_time  //0x05A0

#define   dhcps_router_enabled(offer)	((offer & OFFER_ROUTER) != 0)
void dhcps_start(struct netif *netif);
void dhcps_stop(struct netif *netif);

#else
#include "lwip/opt.h"

#include "lwip/netif.h"
#include "lwip/udp.h"

/** DHCP DEBUG INFO **/
#define DHCPS_DEBUG 1
#if DHCPS_DEBUG    	
#define log_info(message, ...)   do { \
		os_printf((message), ##__VA_ARGS__);   \
		os_printf("\n");		  \
	  } while(0);
	
#else
#define log_info(message, ...)
#endif

#if (!defined(unlikely))
#define unlikely(Expression)	!!(Expression)
#endif

#define REQUIRE_ASSERT(Expression) do{if (!(Expression)) log_info("%d\n", __LINE__);}while(0)

#define REQUIRE_ACTION(Expression,Label,Action) \
	do{\
		if (unlikely(!(Expression))) \
		{\
			log_info("%d\n", __LINE__);\
			{Action;}\
			goto Label;\
		}\
	}while(0)

#define REQUIRE_NOERROR(Expression,Label) \
	do{\
		int LocalError;\
		LocalError = (int)Expression;\
		if (unlikely(LocalError != 0)) \
		{\
			log_info("%d 0x%x\n", __LINE__, LocalError);\
			goto Label;\
		}\
	}while(0)

#define REQUIRE_NOERROR_ACTION(Expression,Label,Action) \
	do{\
		int LocalError;\
		LocalError = (int)Expression;\
		if (unlikely(LocalError != 0)) \
		{\
			log_info("%d\n", __LINE__);\
			{Action;}\
			goto Label;\
		}\
	}while(0)

#define DHCP_OK	0
#define DHCP_FAIL -1
#define DHCP_SERVER_OPENDNS    0xd043dede /* OpenDNS DNS server 208.67.222.222 */

/* DHCP message */
/*
 * Code ID of DHCP and BOOTP options
 * as defined in RFC 2132
 */
typedef enum
{
	DHCP_NONE	= 0,
    DHCPS_DISCOVER = 1,
    DHCPS_OFFER    = 2,
    DHCPS_REQUEST  = 3,
    DHCPS_DECLINE  = 4,
    DHCPS_ACK      = 5,
    DHCPS_NAK      = 6,
    DHCPS_RELEASE  = 7,
    DHCPS_INFORM   = 8,
    DHCP_FORCE_RENEW = 9,
    DHCP_LEASE_QUERY = 10,
    DHCP_LEASE_UNASSIGNED = 11,
    DHCP_LEASE_UNKNOWN = 12,
    DHCP_LEASE_ACTIVE =  13,
} dhcp_msg_type;

typedef enum
{
    BOOTPS = 67,
    BOOTPC = 68
} ports;

typedef enum
{
    BOOTREQUEST = 1,
    BOOTREPLY   = 2,
} op_types;

typedef enum
{
    ETHERNET     = 0x01,
    ETHERNET_LAN = 0x06,
    ETHEFDDI = 0x08,
    ETHEIEEE1394 = 0x18
} hardware_types;

typedef enum
{
    DHCPS_CHADDR_LEN = 16U,
    DHCPS_SNAME_LEN = 64U,
    DHCPS_FILE_LEN = 128U,
    DHCP_HEADER_SIZE = 236U // without size of options
} head_size;

typedef struct dhcps
{
    /** transaction identifier of last sent request */
    u32_t xid;
    /** incoming msg */
    struct dhcps_msg *msg_in;
    /** track PCB allocation state */
    u8_t pcb_allocated;
    /** current DHCP state machine state */
    u8_t state;
    /** retries of current request */
    u8_t tries;
#if LWIP_DHCP_AUTOIP_COOP
    u8_t autoip_coop_state;
#endif
    u8_t subnet_mask_given;

    struct pbuf *p_out; /* pbuf of outcoming msg */
    struct dhcp_msg *msg_out; /* outgoing msg */
    u16_t options_out_len; /* outgoing msg options length */
    u16_t request_timeout; /* #ticks with period DHCP_FINE_TIMER_SECS for request timeout */
    u16_t t1_timeout;  /* #ticks with period DHCP_COARSE_TIMER_SECS for renewal time */
    u16_t t2_timeout;  /* #ticks with period DHCP_COARSE_TIMER_SECS for rebind time */
    u16_t t1_renew_time;  /* #ticks with period DHCP_COARSE_TIMER_SECS until next renew try */
    u16_t t2_rebind_time; /* #ticks with period DHCP_COARSE_TIMER_SECS until next rebind try */
    u16_t lease_used; /* #ticks with period DHCP_COARSE_TIMER_SECS since last received DHCP ack */
    u16_t t0_timeout; /* #ticks with period DHCP_COARSE_TIMER_SECS for lease time */
    ip_addr_t server_ip_addr; /* dhcp server address that offered this lease (ip_addr_t because passed to UDP) */
    ip4_addr_t offered_ip_addr;
    ip4_addr_t offered_sn_mask;
    ip4_addr_t offered_gw_addr;

    u32_t offered_t0_lease; /* lease period (in seconds) */
    u32_t offered_t1_renew; /* recommended renew time (usually 50% of lease period) */
    u32_t offered_t2_rebind; /* recommended rebind time (usually 87.5 of lease period)  */
#if LWIP_DHCP_BOOTP_FILE
    ip_addr_t offered_si_addr;
    char boot_file_name[DHCP_FILE_LEN];
#endif /* LWIP_DHCP_BOOTPFILE */

} dhcps;

typedef struct dhcp_message
{
    PACK_STRUCT_FLD_8(u8_t op);
    PACK_STRUCT_FLD_8(u8_t htype);
    PACK_STRUCT_FLD_8(u8_t hlen);
    PACK_STRUCT_FLD_8(u8_t hops);
    PACK_STRUCT_FIELD(u32_t xid);
    PACK_STRUCT_FIELD(u16_t secs);
    PACK_STRUCT_FIELD(u16_t flags);
    PACK_STRUCT_FLD_S(ip4_addr_p_t ciaddr);
    PACK_STRUCT_FLD_S(ip4_addr_p_t yiaddr);
    PACK_STRUCT_FLD_S(ip4_addr_p_t siaddr);
    PACK_STRUCT_FLD_S(ip4_addr_p_t giaddr);
    PACK_STRUCT_FLD_8(u8_t chaddr[DHCPS_CHADDR_LEN]);
    PACK_STRUCT_FLD_8(u8_t sname[DHCPS_SNAME_LEN]);
    PACK_STRUCT_FLD_8(u8_t file[DHCPS_FILE_LEN]);
//    PACK_STRUCT_FIELD(u32_t cookie);
#define DHCPS_MIN_OPTIONS_LEN 312U
    /** make sure user does not configure this too small */
#if ((defined(DHCPS_OPTIONS_LEN)) && (DHCPS_OPTIONS_LEN < DHCPS_MIN_OPTIONS_LEN))
#undef DHCPS_OPTIONS_LEN
#endif
    /** allow this to be configured in lwipopts.h, but not too small */
#if (!defined(DHCPS_OPTIONS_LEN))
    /** set this to be sufficient for your options in outgoing DHCP msgs */
#define DHCPS_OPTIONS_LEN DHCPS_MIN_OPTIONS_LEN
#endif
    PACK_STRUCT_FLD_8(u8_t options[DHCPS_OPTIONS_LEN]);
} dhcp_message;

/** DHCP OPTIONS CODE **/
typedef enum
{
    /* RFC 1497 Vendor Extensions */

    PAD = 0,
    END = 255,

    SUBNET_MASK = 1,
    TIME_OFFSET = 2,
    ROUTER = 3,
    TIME_SERVER = 4,
    NAME_SERVER = 5,
    DOMAIN_NAME_SERVER = 6,
    LOG_SERVER = 7,
    COOKIE_SERVER = 8,
    LPR_SERVER = 9,
    IMPRESS_SERVER = 10,
    RESOURCE_LOCATION_SERVER = 11,
    HOST_NAME = 12,
    BOOT_FILE_SIZE = 13,
    MERIT_DUMP_FILE = 14,
    DOMAIN_NAME = 15,
    SWAP_SERVER = 16,
    ROOT_PATH = 17,
    EXTENSIONS_PATH = 18,

    /* IP Layer Parameters per Host */

    IP_FORWARDING = 19,
    NON_LOCAL_SOURCE_ROUTING = 20,
    POLICY_FILTER = 21,
    MAXIMUM_DATAGRAM_REASSEMBLY_SIZE = 22,
    DEFAULT_IP_TIME_TO_LIVE = 23,
    PATH_MTU_AGING_TIMEOUT = 24,
    PATH_MTU_PLATEAU_TABLE = 25,

    /* IP Layer Parameters per Interface */

    INTERFACE_MTU = 26,
    ALL_SUBNETS_ARE_LOCAL = 27,
    BROADCAST_ADDRESS = 28,
    PERFORM_MASK_DISCOVERY = 29,
    MASK_SUPPLIER = 30,
    PERFORM_ROUTER_DISCOVERY = 31,
    ROUTER_SOLICITATION_ADDRESS = 32,
    STATIC_ROUTE = 33,

    /* Link Layer Parameters per Interface */

    TRAILER_ENCAPSULATION = 34,
    ARP_CACHE_TIMEOUT = 35,
    ETHERNET_ENCAPSULATION = 36,

    /* TCP Parameters */

    TCP_DEFAULT_TTL = 37,
    TCP_KEEPALIVE_INTERVAL = 38,
    TCP_KEEPALIVE_GARBAGE = 39,

    /* Application and Service Parameters */

    NETWORK_INFORMATION_SERVICE_DOMAIN = 40,
    NETWORK_INFORMATION_SERVERS = 41,
    NETWORK_TIME_PROTOCOL_SERVERS = 42,
    VENDOR_SPECIFIC_INFORMATION = 43,
    NETBIOS_OVER_TCP_IP_NAME_SERVER = 44,
    NETBIOS_OVER_TCP_IP_DATAGRAM_DISTRIBUTION_SERVER = 45,
    NETBIOS_OVER_TCP_IP_NODE_TYPE = 46,
    NETBIOS_OVER_TCP_IP_SCOPE = 47,
    X_WINDOW_SYSTEM_FONT_SERVER = 48,
    X_WINDOW_SYSTEM_DISPLAY_MANAGER = 49,
    NETWORK_INFORMATION_SERVICE_PLUS_DOMAIN = 64,
    NETWORK_INFORMATION_SERVICE_PLUS_SERVERS = 65,
    MOBILE_IP_HOME_AGENT = 68,
    SMTP_SERVER = 69,
    POP3_SERVER = 70,
    NNTP_SERVER = 71,
    DEFAULT_WWW_SERVER = 72,
    DEFAULT_FINGER_SERVER = 73,
    DEFAULT_IRC_SERVER = 74,
    STREETTALK_SERVER = 75,
    STREETTALK_DIRECTORY_ASSISTANCE_SERVER = 76,

    /* DHCP Extensions */

    REQUESTED_IP_ADDRESS = 50,
    IP_ADDRESS_LEASE_TIME = 51,
    OPTION_OVERLOAD = 52,
    TFTP_SERVER_NAME = 66,
    BOOTFILE_NAME = 67,
    DHCP_MESSAGE_TYPE = 53,
    SERVER_IDENTIFIER = 54,
    PARAMETER_REQUEST_LIST = 55,
    MESSAGE = 56,
    MAXIMUM_DHCP_MESSAGE_SIZE = 57,
    RENEWAL_T1_TIME_VALUE = 58,
    REBINDING_T2_TIME_VALUE = 59,
    VENDOR_CLASS_IDENTIFIER = 60,
    CLIENT_IDENTIFIER = 61,

    USER_CLASS = 77,
    FQDN = 81,
    DHCP_AGENT_OPTIONS = 82,
    NDS_SERVERS = 85,
    NDS_TREE_NAME = 86,
    NDS_CONTEXT = 87,
    CLIENT_LAST_TRANSACTION_TIME = 91,
    ASSOCIATED_IP = 92,
    USER_AUTHENTICATION_PROTOCOL = 98,
    AUTO_CONFIGURE = 116,
    NAME_SERVICE_SEARCH = 117,
    SUBNET_SELECTION = 118,
    DOMAIN_SEARCH = 119,
    CLASSLESS_ROUTE = 121,
} dhcp_msg_option;

typedef struct dhcp_option
{
    uint8_t id;        // option id
    uint8_t len;       // option length
    uint8_t data[256]; // option data

    STAILQ_ENTRY(dhcp_option) pointers; // pointers, see queue(3)
} dhcp_option;

typedef STAILQ_HEAD(dhcp_option_list_, dhcp_option) DHCP_OPTION_LIST;
typedef struct dhcp_option_list_ dhcp_option_list;

/*
 * Header to manage the database of address bindings.
 */

// static association or dynamic
enum
{
    DYNAMIC = 0,
    STATIC  = 1,
    STATIC_OR_DYNAMIC = 2
};

// binding status
enum
{
    EMPTY = 0,
    ASSOCIATED,
    PENDINGD,
    EXPIRED,
    RELEASED
};

/*
 * IP address used to delimitate an address pool.
 */
typedef struct pool_indexes
{
    uint32_t first;    // first address of the pool
    uint32_t last;     // last address of the pool
    uint32_t current;  // current available address
}pool_indexes;

/*
 * The bindings are organized as a double linked list
 * using the standard queue(3) library
 */
typedef struct address_binding
{
    uint32_t address;     // address
    uint8_t cident_len;   // client identifier len
    uint8_t cident[256];  // client identifier

    time_t binding_time;  // time of binding
    time_t lease_time;    // duration of lease

    int status;           // binding status
    int is_static;        // check if it is a static binding

    STAILQ_ENTRY(address_binding) pointers; // list pointers, see queue(3)
}address_binding;

typedef STAILQ_HEAD(binding_list_, address_binding) BINDING_LIST_HEAD;
typedef struct binding_list_ binding_list;

/*
 * Global association pool.
 *
 * The (static or dynamic) associations tables of the DHCP server,
 * are maintained in this global structure.
 *
 * Note: all the IP addresses are in host order,
 *       to allow an easy manipulation.
 */

typedef struct address_pool
{
    uint32_t server_id; // this server id (IP address)
    uint32_t netmask;   // network mask
    uint32_t gateway;   // network gateway

    char device[16];    // network device to use

    pool_indexes indexes;  // used to delimitate a pool of available addresses

    time_t lease_time;   // default lease time
    time_t pending_time; // duration of a binding in the pending state
    struct udp_pcb *socket;	   // 
    bool   flags;

    dhcp_option_list options; // options for this pool, see queue

    binding_list bindings; // associated addresses, see queue(3)
}address_pool;

/*
 * Internal representation of a DHCP message,
 * with options parsed into a list...
 */
typedef struct dhcps_msg
{
    dhcp_message     hdr;
    dhcp_option_list opts;
} dhcps_msg;

bool dhcps_option_set(u8_t opt_id, void* optarg);
void dhcps_start(struct netif *netif, struct dhcps_lease *lease_pool);
void dhcps_stop(struct netif *netif);
bool dhcps_lease_set(struct dhcps_lease *please);
bool dhcps_lease_get(struct dhcps_lease *please);

#endif
#endif
