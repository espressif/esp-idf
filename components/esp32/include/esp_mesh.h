// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
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

/*
 *   Software Stack demonstrated:
 *  |------------------------------------------------------------------------------|
 *  |            |                                                                 |
 *  |            |                          Application                            |
 *  |            |-----------------------------------------------------------------|
 *  |            |               | Protocols: |      |      |               |      |
 *  |            |   Mesh Stack  | HTTP, DNS, |      |      |     Other     |      |
 *  |   RTOS:    | (Networking,  | DHCP, ...  |      |      |   Components  |      |
 *  | (freeRTOS) | self-healing, |------------|      |      |               |      |
 *  |            | flow control, |  Network Stack:   |      |               |      |
 *  |            | ...)          |      (LwIP)       |      |               |      |
 *  |            |-----------------------------------|      |---------------|      |
 *  |            |                                                  |              |
 *  |            |                       WiFi Driver                |              |
 *  |            |--------------------------------------------------|              |
 *  |            |                                                                 |
 *  |            |                       Platform HAL                              |
 *  |------------------------------------------------------------------------------|
 *
 *   System Events delivery:
 *
 *  |---------------|
 *  |               |                    default handler
 *  |  WiFi stack   |     events     |---------------------|
 *  |               | -------------> |                     |
 *  |---------------|                |                     |
 *                                   |      event task     |
 *  |---------------|     events     |                     |
 *  |               | -------------> |                     |
 *  |  LwIP stack   |                |---------------------|
 *  |               |--------|
 *  |---------------|        |
 *                           |         mesh event callback handler
 *                           |       |----------------------------|
 *                           |-----> |                            |
 *  |---------------|                |        application         |
 *  |               |     events     |            task            |
 *  |  mesh stack   | -------------> |                            |
 *  |               |                |----------------------------|
 *  |---------------|
 *
 *
 *  Mesh Stack
 *
 *  Mesh event defines almost all system events applications tasks need.
 *  Mesh event contains WiFi connection states on station interface, children connection states on softAP interface and etc..
 *  Applications need to register a mesh event callback handler by API esp_mesh_set_config() firstly.
 *  This handler is to receive events posted from mesh stack and LwIP stack.
 *  Applications could add relative handler for each event.
 *  Examples:
 *  (1)Applications could use WiFi station connect states to decide when to send data to its parent, to root or to external IP network;
 *  (2)Applications could use WiFi softAP states to decide when to send data to its children.
 *
 *  In present implementation, applications are able to access mesh stack directly without having to go through LwIP stack.
 *  Applications use esp_mesh_send() and esp_mesh_recv() to send and receive messages over the mesh network.
 *  In mesh stack design, normal devices don't require LwIP stack. But since IDF hasn't supported system without initializing LwIP stack yet,
 *  applications still need to do LwIP initialization and two more things are required to be done
 *  (1)stop DHCP server on softAP interface by default
 *  (2)stop DHCP client on station interface by default.
 *  Examples:
 *  tcpip_adapter_init();
 *  tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP)；
 *  tcpip_adapter_dhcpc_stop(TCPIP_ADAPTER_IF_STA)；
 *
 *  Over the mesh network, only root is able to access external IP network.
 *  In application mesh event handler, once a device becomes a root, start DHCP client immediately if DHCP is chosen.
 */

#ifndef __ESP_MESH_H__
#define __ESP_MESH_H__

#include "esp_err.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_mesh_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************
 *                Constants
 *******************************************************/
#define MESH_ROOT_LAYER                   (1)       /**< root layer value */
#define MESH_MTU                          (1500)    /**< max transmit unit(in bytes) */
#define MESH_MPS                          (1472)    /**< max payload size(in bytes) */
/**
 * @brief mesh error code definition
 */
#define ESP_ERR_MESH_WIFI_NOT_START       (ESP_ERR_MESH_BASE + 1)    /**< WiFi isn't started */
#define ESP_ERR_MESH_NOT_INIT             (ESP_ERR_MESH_BASE + 2)    /**< mesh isn't initialized */
#define ESP_ERR_MESH_NOT_CONFIG           (ESP_ERR_MESH_BASE + 3)    /**< mesh isn't configured */
#define ESP_ERR_MESH_NOT_START            (ESP_ERR_MESH_BASE + 4)    /**< mesh isn't started */
#define ESP_ERR_MESH_NOT_SUPPORT          (ESP_ERR_MESH_BASE + 5)    /**< not supported yet */
#define ESP_ERR_MESH_NOT_ALLOWED          (ESP_ERR_MESH_BASE + 6)    /**< operation is not allowed */
#define ESP_ERR_MESH_NO_MEMORY            (ESP_ERR_MESH_BASE + 7)    /**< out of memory */
#define ESP_ERR_MESH_ARGUMENT             (ESP_ERR_MESH_BASE + 8)    /**< illegal argument */
#define ESP_ERR_MESH_EXCEED_MTU           (ESP_ERR_MESH_BASE + 9)    /**< packet size exceeds MTU */
#define ESP_ERR_MESH_TIMEOUT              (ESP_ERR_MESH_BASE + 10)   /**< timeout */
#define ESP_ERR_MESH_DISCONNECTED         (ESP_ERR_MESH_BASE + 11)   /**< disconnected with parent on station interface */
#define ESP_ERR_MESH_QUEUE_FAIL           (ESP_ERR_MESH_BASE + 12)   /**< queue fail */
#define ESP_ERR_MESH_QUEUE_FULL           (ESP_ERR_MESH_BASE + 13)   /**< queue full */
#define ESP_ERR_MESH_NO_PARENT_FOUND      (ESP_ERR_MESH_BASE + 14)   /**< no parent found to join the mesh network */
#define ESP_ERR_MESH_NO_ROUTE_FOUND       (ESP_ERR_MESH_BASE + 15)   /**< no route found to forward the packet */
#define ESP_ERR_MESH_OPTION_NULL          (ESP_ERR_MESH_BASE + 16)   /**< no option found */
#define ESP_ERR_MESH_OPTION_UNKNOWN       (ESP_ERR_MESH_BASE + 17)   /**< unknown option */
#define ESP_ERR_MESH_XON_NO_WINDOW        (ESP_ERR_MESH_BASE + 18)   /**< no window for software flow control on upstream */
#define ESP_ERR_MESH_INTERFACE            (ESP_ERR_MESH_BASE + 19)   /**< low-level WiFi interface error */
#define ESP_ERR_MESH_DISCARD_DUPLICATE    (ESP_ERR_MESH_BASE + 20)   /**< discard the packet due to the duplicate sequence number */
#define ESP_ERR_MESH_DISCARD              (ESP_ERR_MESH_BASE + 21)   /**< discard the packet */
#define ESP_ERR_MESH_VOTING               (ESP_ERR_MESH_BASE + 22)   /**< vote in progress */

/**
 * @brief flags used with esp_mesh_send() and esp_mesh_recv()
 */
#define MESH_DATA_ENC           (0x01)  /**< data encrypted(Unimplemented) */
#define MESH_DATA_P2P           (0x02)  /**< point-to-point delivery over the mesh network */
#define MESH_DATA_FROMDS        (0x04)  /**< receive from external IP network */
#define MESH_DATA_TODS          (0x08)  /**< identify this packet is target to external IP network */
#define MESH_DATA_NONBLOCK      (0x10)  /**< esp_mesh_send() non-block */
#define MESH_DATA_DROP          (0x20)  /**< in the situation of root having been changed, identify this packet can be dropped by new root */
#define MESH_DATA_GROUP         (0x40)  /**< identify this packet is target to a group address */

/**
 * @brief option definitions for esp_mesh_send() and esp_mesh_recv()
 */
#define MESH_OPT_SEND_GROUP     (7)     /**< data transmission by group; used with esp_mesh_send() and shall have payload */
#define MESH_OPT_RECV_DS_ADDR   (8)     /**< return a remote IP address; used with esp_mesh_send() and esp_mesh_recv() */

/**
 * @brief flag of mesh networking IE
 */
#define MESH_ASSOC_FLAG_VOTE_IN_PROGRESS    (0x02)     /**< vote in progress */
#define MESH_ASSOC_FLAG_NETWORK_FREE        (0x08)     /**< no root in current network */
#define MESH_ASSOC_FLAG_ROOTS_FOUND         (0x20)     /**< root conflict is found */
#define MESH_ASSOC_FLAG_ROOT_FIXED          (0x40)     /**< root is fixed */

/*******************************************************
 *                Enumerations
 *******************************************************/
/**
 * @brief enumerated list of mesh event id
 */
typedef enum {
    MESH_EVENT_STARTED,                 /**< mesh is started */
    MESH_EVENT_STOPPED,                 /**< mesh is stopped */
    MESH_EVENT_CHANNEL_SWITCH,          /**< channel switch */
    MESH_EVENT_CHILD_CONNECTED,         /**< a child is connected on softAP interface */
    MESH_EVENT_CHILD_DISCONNECTED,      /**< a child is disconnected on softAP interface */
    MESH_EVENT_ROUTING_TABLE_ADD,       /**< routing table is changed by adding newly joined children */
    MESH_EVENT_ROUTING_TABLE_REMOVE,    /**< routing table is changed by removing leave children */
    MESH_EVENT_PARENT_CONNECTED,        /**< parent is connected on station interface */
    MESH_EVENT_PARENT_DISCONNECTED,     /**< parent is disconnected on station interface */
    MESH_EVENT_NO_PARNET_FOUND,         /**< no parent found */
    MESH_EVENT_LAYER_CHANGE,            /**< layer changes over the mesh network */
    MESH_EVENT_TODS_STATE,              /**< state represents if root is able to access external IP network */
    MESH_EVENT_VOTE_STARTED,            /**< the process of voting a new root is started either by children or by root */
    MESH_EVENT_VOTE_STOPPED,            /**< the process of voting a new root is stopped */
    MESH_EVENT_ROOT_ADDRESS,            /**< the root address is obtained. It is posted by mesh stack automatically. */
    MESH_EVENT_ROOT_SWITCH_REQ,         /**< root switch request sent from a new voted root candidate */
    MESH_EVENT_ROOT_SWITCH_ACK,         /**< root switch acknowledgment responds the above request sent from current root */
    MESH_EVENT_ROOT_GOT_IP,             /**< root obtains the IP address. It is posted by LwIP stack automatically */
    MESH_EVENT_ROOT_LOST_IP,            /**< root loses the IP address. It is posted by LwIP stack automatically */
    MESH_EVENT_ROOT_ASKED_YIELD,        /**< root is asked yield by a more powerful existing root. If self organized is disabled
                                             and this device is specified to be a root by users, users should set a new parent
                                             for this device. if self organized is enabled, this device will find a new parent
                                             by itself, users could ignore this event. */
    MESH_EVENT_ROOT_FIXED,              /**< when devices join a network, if the setting of Fixed Root for one device is different
                                             from that of its parent, the device will update the setting the same as its parent's.
                                             Fixed Root setting of each device is variable as that setting changes of root. */
    MESH_EVENT_SCAN_DONE,               /**< if self-organized networking is disabled, user can call esp_wifi_scan_start() to trigger
                                             this event, and add the corresponding scan done handler in this event. */
    MESH_EVENT_MAX,
} mesh_event_id_t;

/**
 * @brief device type
 */
typedef enum {
    MESH_IDLE,    /**< hasn't joined the mesh network yet */
    MESH_ROOT,    /**< the only sink of the mesh network. Has the ability to access external IP network */
    MESH_NODE,    /**< intermediate device. Has the ability to forward packets over the mesh network */
    MESH_LEAF,    /**< has no forwarding ability */
} mesh_type_t;

/**
 * @brief protocol of transmitted application data
 */
typedef enum {
    MESH_PROTO_BIN,     /**< binary */
    MESH_PROTO_HTTP,    /**< HTTP protocol */
    MESH_PROTO_JSON,    /**< JSON format */
    MESH_PROTO_MQTT,    /**< MQTT protocol */
} mesh_proto_t;

/**
 * @brief for reliable transmission, mesh stack provides three type of services
 */
typedef enum {
    MESH_TOS_P2P,    /**< provide P2P(point-to-point) retransmission on mesh stack by default */
    MESH_TOS_E2E,    /**< provide E2E(end-to-end) retransmission on mesh stack (Unimplemented) */
    MESH_TOS_DEF,    /**< no retransmission on mesh stack */
} mesh_tos_t;

/**
 * @brief vote reason
 */
typedef enum {
    MESH_VOTE_REASON_ROOT_INITIATED = 1,    /**< vote is initiated by root */
    MESH_VOTE_REASON_CHILD_INITIATED,       /**< vote is initiated by children */
} mesh_vote_reason_t;

/**
 * @brief mesh disconnect reason code
 */
typedef enum {
    MESH_REASON_CYCLIC = 100,      /**< cyclic is detected */
    MESH_REASON_PARENT_IDLE,       /**< parent is idle */
    MESH_REASON_LEAF,              /**< the connected device is changed to a leaf */
    MESH_REASON_DIFF_ID,           /**< in different mesh ID */
    MESH_REASON_ROOTS,             /**< root conflict is detected */
    MESH_REASON_PARENT_STOPPED,    /**< parent has stopped the mesh */
    MESH_REASON_SCAN_FAIL,         /**< scan fail */
} mesh_disconnect_reason_t;

/*******************************************************
 *                Structures
 *******************************************************/
/**
 * @brief IP address and port
 */
typedef struct {
    ip4_addr_t ip4;    /**< IP address */
    uint16_t port;     /**< port */
} __attribute__((packed)) mip_t;

/**
 * @brief mesh address
 */
typedef union {
    uint8_t addr[6];    /**< mac address */
    mip_t mip;          /**< mip address */
} mesh_addr_t;

/**
 * @brief channel switch information
 */
typedef struct {
    uint8_t channel;    /**< new channel */
} mesh_event_channel_switch_t;

/**
 * @brief parent connected information
 */
typedef struct {
    system_event_sta_connected_t connected; /**< parent information, same as WiFi event SYSTEM_EVENT_STA_CONNECTED does */
    uint8_t self_layer;                     /**< layer */
} mesh_event_connected_t;

/**
 * @brief no parent found information
 */
typedef struct {
    int scan_times;    /**< scan times being through */
} mesh_event_no_parent_found_t;

/**
 * @brief layer change information
 */
typedef struct {
    uint8_t new_layer; /**< new layer */
} mesh_event_layer_change_t;

/**
 * @brief the reachability of root to a DS(distribute system)
 */
typedef enum {
    MESH_TODS_UNREACHABLE,    /**< root isn't able to access external IP network */
    MESH_TODS_REACHABLE,      /**< root is able to access external IP network */
} mesh_event_toDS_state_t;

/**
 * @brief vote started information
 */
typedef struct {
    int reason;             /**< vote reason, vote could be initiated by children or by root itself */
    int attempts;           /**< max vote attempts before stopped */
    mesh_addr_t rc_addr;    /**< root address specified by users via API esp_mesh_waive_root() */
} mesh_event_vote_started_t;

/**
 * @brief IP settings from LwIP stack
 */
typedef system_event_sta_got_ip_t mesh_event_root_got_ip_t;

/**
 * @brief root address
 */
typedef mesh_addr_t mesh_event_root_address_t;

/**
 * @brief parent disconnected information
 */
typedef system_event_sta_disconnected_t mesh_event_disconnected_t;

/**
 * @brief child connected information
 */
typedef system_event_ap_staconnected_t mesh_event_child_connected_t;

/**
 * @brief child disconnected information
 */
typedef system_event_ap_stadisconnected_t mesh_event_child_disconnected_t;

/**
 * @brief root switch request information
 */
typedef struct {
    int reason;             /**< root switch reason, generally root switch is initialized by users via API esp_mesh_waive_root() */
    mesh_addr_t rc_addr;    /**< the address of root switch requester */
} mesh_event_root_switch_req_t;

/**
 * @brief other powerful root address
 */
typedef struct {
    int8_t rssi;           /**< rssi with router */
    uint16_t capacity;     /**< the number of devices in current network */
    uint8_t addr[6];       /**< other powerful root address */
} mesh_event_root_conflict_t;

/**
 * @brief routing table change
 */
typedef struct {
    uint16_t rt_size_new;      /**< the new value */
    uint16_t rt_size_change;   /**< the changed value */
} mesh_event_routing_table_change_t;

/**
 * @brief root fixed
 */
typedef struct {
    bool is_fixed;     /**< status */
} mesh_event_root_fixed_t;

/**
 * @brief scan done　event information
 */
typedef struct {
    uint8_t  number;     /**< the number of scanned APs */
} mesh_event_scan_done_t;

/**
 * @brief mesh event information
 */
typedef union {
    mesh_event_channel_switch_t channel_switch;            /**< channel switch */
    mesh_event_child_connected_t child_connected;          /**< child connected */
    mesh_event_child_disconnected_t child_disconnected;    /**< child disconnected */
    mesh_event_routing_table_change_t routing_table;       /**< routing table change */
    mesh_event_connected_t connected;                      /**< parent connected */
    mesh_event_disconnected_t disconnected;                /**< parent disconnected */
    mesh_event_no_parent_found_t no_parent;                /**< no parent found */
    mesh_event_layer_change_t layer_change;                /**< layer change */
    mesh_event_toDS_state_t toDS_state;                    /**< toDS state, devices shall check this state firstly before trying to send packets to
                                                                external IP network. This state indicates right now if root is capable of sending
                                                                packets out. If not, devices had better to wait until this state changes to be
                                                                MESH_TODS_REACHABLE. */
    mesh_event_vote_started_t vote_started;                /**< vote started */
    mesh_event_root_got_ip_t got_ip;                       /**< root obtains IP address */
    mesh_event_root_address_t root_addr;                   /**< root address */
    mesh_event_root_switch_req_t switch_req;               /**< root switch request */
    mesh_event_root_conflict_t root_conflict;              /**< other powerful root */
    mesh_event_root_fixed_t root_fixed;                    /**< root fixed */
    mesh_event_scan_done_t scan_done;                      /**< scan done */
} mesh_event_info_t;

/**
 * @brief mesh event
 */
typedef struct {
    mesh_event_id_t id;        /**< mesh event id */
    mesh_event_info_t info;    /**< mesh event info */
} mesh_event_t;

/**
 * @brief  mesh event callback handler prototype definition
 *
 * @param  event  mesh_event_t
 */
typedef void (*mesh_event_cb_t)(mesh_event_t event);

/**
 * @brief mesh option
 */
typedef struct {
    uint8_t type;    /**< option type */
    uint16_t len;    /**< option length */
    uint8_t *val;    /**< option value */
} __attribute__((packed)) mesh_opt_t;

/**
 * @brief mesh data for esp_mesh_send() and esp_mesh_recv()
 */
typedef struct {
    uint8_t *data;         /**< data */
    uint16_t size;         /**< data size */
    mesh_proto_t proto;    /**< data protocol */
    mesh_tos_t tos;        /**< data type of service */
} mesh_data_t;

/**
 * @brief router configuration
 */
typedef struct {
    uint8_t ssid[32];        /**< SSID */
    uint8_t ssid_len;        /**< length of SSID */
    uint8_t bssid[6];        /**< BSSID, if router is hidden, this value is mandatory */
    uint8_t password[64];    /**< password */
} mesh_router_t;

/**
 * @brief mesh softAP configuration
 */
typedef struct {
    uint8_t password[64];      /**< mesh softAP password */
    uint8_t max_connection;    /**< max number of stations allowed to connect in, max 10 */
} mesh_ap_cfg_t;

/**
 * @brief mesh initialization configuration
 */
typedef struct {
    uint8_t channel;                            /**< channel, the mesh network on */
    mesh_event_cb_t event_cb;                   /**< mesh event callback */
    mesh_addr_t mesh_id;                        /**< mesh network identification */
    mesh_router_t router;                       /**< router configuration */
    mesh_ap_cfg_t mesh_ap;                      /**< mesh softAP configuration */
    const mesh_crypto_funcs_t *crypto_funcs;    /**< crypto functions */
} mesh_cfg_t;

/**
 * @brief vote address configuration
 */
typedef union {
    int attempts;           /**< max vote attempts before a new root is elected automatically by mesh network. (min:15, 15 by default) */
    mesh_addr_t rc_addr;    /**< a new root address specified by users for API esp_mesh_waive_root() */
} mesh_rc_config_t;

/**
 * @brief vote
 */
typedef struct {
    float percentage;           /**< vote percentage threshold for approval of being a root */
    bool is_rc_specified;       /**< if true, rc_addr shall be specified(Unimplemented).
                                     if false, attempts value shall be specified to make network start root election. */
    mesh_rc_config_t config;    /**< vote address configuration */
} mesh_vote_t;

/**
 * @brief the number of packets pending in the queue waiting to be sent by the mesh stack
 */
typedef struct {
    int to_parent;        /**< to parent queue */
    int to_parent_p2p;    /**< to parent(P2P) queue */
    int to_child;         /**< to child queue */
    int to_child_p2p;     /**< to child(P2P) queue */
    int mgmt;             /**< management queue */
    int broadcast;        /**< broadcast and multicast queue */
} mesh_tx_pending_t;

/**
 * @brief the number of packets available in the queue waiting to be received by applications
 */
typedef struct {
    int toDS;      /**< to external DS */
    int toSelf;    /**< to self */
} mesh_rx_pending_t;

/*******************************************************
 *                Variable Declaration
 *******************************************************/
/* mesh vendor IE crypto callback function */
extern const mesh_crypto_funcs_t g_wifi_default_mesh_crypto_funcs;

/* mesh event callback handler */
extern mesh_event_cb_t g_mesh_event_cb;

#define MESH_INIT_CONFIG_DEFAULT() { \
    .crypto_funcs = &g_wifi_default_mesh_crypto_funcs, \
}

/*******************************************************
 *                Function Definitions
 *******************************************************/
/**
 * @brief     mesh initialization
 *            Check if WiFi is started.
 *            Initialize mesh global variables with default values.
 *
 * @attention This API shall be called after WiFi is started.
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 */
esp_err_t esp_mesh_init(void);

/**
 * @brief     mesh de-initialization
 *            Release resources and stop the mesh
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 */
esp_err_t esp_mesh_deinit(void);

/**
 * @brief     start mesh
 *            Initialize mesh vendor IE
 *            Start mesh network management service
 *            Create TX and RX queues according to the configuration
 *            Register mesh packets receive callback
 *
 * @attention This API shall be called after esp_mesh_init() and esp_mesh_set_config().
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 *    - ESP_ERR_MESH_NOT_INIT
 *    - ESP_ERR_MESH_NOT_CONFIG
 *    - ESP_ERR_MESH_NO_MEMORY
 */
esp_err_t esp_mesh_start(void);

/**
 * @brief     stop mesh
 *            Deinitialize mesh vendor IE
 *            Disconnect with current parent
 *            Disassociate all currently associated children
 *            Stop mesh network management service
 *            Unregister mesh packets receive callback
 *            Delete TX and RX queues
 *            Release resources
 *            Restore WiFi softAP to default settings if WiFi dual mode is enabled
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 */
esp_err_t esp_mesh_stop(void);

/**
 * @brief     send a packet over the mesh network
 *            Send a packet to any device in the mesh network.
 *            Send a packet to external IP network.
 *
 * @attention This API is not reentrant.
 *
 * @param     to  the address of the final destination of the packet
 *            (1)if the packet is to root, just set "to" to NULL and set flag to zero.
 *            (2)if the packet is outgoing to external IP network such as an IP server address, translate IPv4:PORT known as "to".
 *               This packet will be delivered to root firstly, then root will forward this packet to the final IP server address.
 * @param     data  pointer to a sending mesh packet
 *            Should specify the data protocol applications used, binary by default.
 *            Should specify the transmission tos(type of service), P2P reliable by default.
 * @param     flag
 *            (1)used to speed up the route selection
 *              if the packet is target to an internal device, MESH_DATA_P2P should be set.
 *              if the packet is outgoing to root or to external IP network, MESH_DATA_TODS should be set.
 *              if the packet is from root to an internal device, MESH_DATA_FROMDS should be set.
 *            (2)specify if this API is block or non-block, block by default
 *              if needs non-block, MESH_DATA_NONBLOCK should be set.
 *            (3)in the situation of root having been changed, MESH_DATA_DROP identifies this packet can be dropped by new root
 *              for upstream data to external IP network, we try our best to avoid data loss caused by root having been changed, but
 *              there is a risk that new root is running out of memory because most of memory is occupied by the pending data which
 *              isn't read out in time by esp_mesh_recv_toDS().
 *              Generally, we suggest esp_mesh_recv_toDS() is called after a connection with IP network is created. Thus data outgoing
 *              to external IP network via socket is just from reading esp_mesh_recv_toDS() which avoids unnecessary memory copy.
 *
 * @param     opt  options
 *            (1)in case of sending a packet to a specified group, MESH_OPT_SEND_GROUP is a good choice.
 *               In this option, the value field should specify the target receiver addresses in this group.
 *            (2)root sends a packet to an internal device, this packet is from external IP network in case the receiver device responds
 *            this packet, MESH_OPT_RECV_DS_ADDR is required to attach the target DS address.
 * @param     opt_count  option count
 *            Currently, this API only takes one option, so opt_count is only supported to be 1.
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 *    - ESP_ERR_MESH_ARGUMENT
 *    - ESP_ERR_MESH_NOT_START
 *    - ESP_ERR_MESH_DISCONNECTED
 *    - ESP_ERR_MESH_OPT_UNKNOWN
 *    - ESP_ERR_MESH_EXCEED_MTU
 *    - ESP_ERR_MESH_NO_MEMORY
 *    - ESP_ERR_MESH_TIMEOUT
 *    - ESP_ERR_MESH_QUEUE_FULL
 *    - ESP_ERR_MESH_NO_ROUTE_FOUND
 *    - ESP_ERR_MESH_DISCARD
 */
esp_err_t esp_mesh_send(const mesh_addr_t *to, const mesh_data_t *data,
                        int flag, const mesh_opt_t opt[],  int opt_count);

/**
 * @brief     receive a packet targeted to self over the mesh network
 *            Use esp_mesh_get_rx_pending() to check the number of packets available in the queue waiting
 *            to be received by applications in case of running out of memory.
 *
 * @param     from  the address of the original source of the packet
 * @param     data  pointer to the received mesh packet
 *            Contain the protocol and applications should follow it to parse the data.
 * @param     timeout_ms  wait time if a packet isn't immediately available(0:no wait, portMAX_DELAY:wait forever)
 * @param     flag
 *            MESH_DATA_FROMDS represents data from external IP network
 *            MESH_DATA_TODS represents data directed upward within the mesh network
 * @param     opt  options desired to receive
 *            MESH_OPT_RECV_DS_ADDR attaches the DS address
 * @param     opt_count  option count desired to receive
 *            Currently, this API only takes one option, so opt_count is only supported to be 1.
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_ARGUMENT
 *    - ESP_ERR_MESH_NOT_START
 *    - ESP_ERR_MESH_TIMEOUT
 *    - ESP_ERR_MESH_DISCARD
 */
esp_err_t esp_mesh_recv(mesh_addr_t *from, mesh_data_t *data, int timeout_ms,
                        int *flag, mesh_opt_t opt[], int opt_count);

/**
 * @brief     receive a packet targeted to external IP network
 *            root uses this API to receive packets destined to external IP network
 *            root forwards the received packets to the final destination via socket.
 *            if no socket connection is ready to send out the received packets and this esp_mesh_recv_toDS()
 *            hasn't been called by applications, packets from the whole mesh network will be pending in toDS queue.
 *            Use esp_mesh_get_rx_pending() to check the number of packets available in the queue waiting
 *            to be received by applications in case of running out of memory in root.
 *            Use esp_mesh_set_xon_qsize() could configure the RX queue size, default:72. If this size is too large,
 *            and esp_mesh_recv_toDS() isn't called in time, there is a risk that a great deal of memory is occupied
 *            by the pending packets. If this size is too small, it will impact the efficiency on upstream. How to
 *            decide this value depends on the specific application scenarios.
 *
 * @attention This API is only called by root.
 *
 * @param     from  the address of the original source of the packet
 * @param     to  the address contains remote IP address and port(IPv4:PORT)
 * @param     data  pointer to the received packet
 *            Contain the protocol and applications should follow it to parse the data.
 * @param     timeout_ms  wait time if a packet isn't immediately available(0:no wait, portMAX_DELAY:wait forever)
 * @param     flag
 *            MESH_DATA_TODS represents data to external IP network
 * @param     opt  options desired to receive
 * @param     opt_count  option count desired to receive
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_ARGUMENT
 *    - ESP_ERR_MESH_NOT_START
 *    - ESP_ERR_MESH_TIMEOUT
 *    - ESP_ERR_MESH_DISCARD
 */
esp_err_t esp_mesh_recv_toDS(mesh_addr_t *from, mesh_addr_t *to,
                             mesh_data_t *data, int timeout_ms, int *flag, mesh_opt_t opt[],
                             int opt_count);

/**
 * @brief     set mesh stack configuration
 *            Use MESH_INIT_CONFIG_DEFAULT() to initialize the default values, mesh vendor IE is encrypted by default.
 *            mesh network is established on a fixed channel(1-14).
 *            mesh event callback is mandatory.
 *            mesh ID is an identifier of an MBSS. Nodes with the same mesh ID can communicate with each other.
 *            Regarding to the router configuration, if the router is hidden, BSSID field is mandatory.
 *            If BSSID field isn't set and there exists more than one router with same SSID, there is a risk that more
 *            roots than one connected with different BSSID will appear. It means more than one mesh network is established
 *            with the same mesh ID.
 *            Root conflict function could eliminate redundant roots connected with the same BSSID, but couldn't handle roots
 *            connected with different BSSID. Because users might have such requirements of setting up routers with same SSID
 *            for the future replacement. But in that case, if the above situations happen, please make sure applications
 *            implement forward functions on root to guarantee devices in different mesh network can communicate with each other.
 *            max_connection of mesh softAP is limited by the max number of WiFi softAP supported(max:10).
 *
 * @attention This API shall be called between esp_mesh_init() and esp_mesh_start().
 *
 * @param     config  pointer to mesh stack configuration
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_ARGUMENT
 *    - ESP_ERR_MESH_NOT_ALLOWED
 */
esp_err_t esp_mesh_set_config(const mesh_cfg_t *config);

/**
 * @brief     get mesh stack configuration
 *
 * @param     config  pointer to mesh stack configuration
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_ARGUMENT
 */
esp_err_t esp_mesh_get_config(mesh_cfg_t *config);

/**
 * @brief     set router configuration
 *
 * @attention This API shall be called between esp_mesh_init() and esp_mesh_start().
 *
 * @param     router  pointer to router configuration
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_ARGUMENT
 */
esp_err_t esp_mesh_set_router(const mesh_router_t *router);

/**
 * @brief     get router configuration
 *
 * @param     router  pointer to router configuration
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_ARGUMENT
 */
esp_err_t esp_mesh_get_router(mesh_router_t *router);

/**
 * @brief     set mesh network ID
 *
 * @attention This API could be called either before esp_mesh_start() or after esp_mesh_start().
 *
 * @param     id  pointer to mesh network ID
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_ARGUMENT: invalid argument
 */
esp_err_t esp_mesh_set_id(const mesh_addr_t *id);

/**
 * @brief     get mesh network ID
 *
 * @param     id  pointer to mesh network ID
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_ARGUMENT
 */
esp_err_t esp_mesh_get_id(mesh_addr_t *id);

/**
 * @brief     set device type over the mesh network(Unimplemented)
 *
 * @param     type  device type
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_NOT_ALLOWED
 */
esp_err_t esp_mesh_set_type(mesh_type_t type);

/**
 * @brief     get device type over mesh network
 *
 * @attention This API shall be called after having received the event MESH_EVENT_PARENT_CONNECTED.
 *
 * @return    mesh type
 *
 */
mesh_type_t esp_mesh_get_type(void);

/**
 * @brief     set max layer configuration(max:15, default:15)
 *
 * @attention This API shall be called before esp_mesh_start().
 *
 * @param     max_layer  max layer value
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_ARGUMENT
 *    - ESP_ERR_MESH_NOT_ALLOWED
 */
esp_err_t esp_mesh_set_max_layer(int max_layer);

/**
 * @brief     get max layer configuration
 *
 * @return    max layer value
 */
int esp_mesh_get_max_layer(void);

/**
 * @brief     set mesh softAP password
 *
 * @attention This API shall be called before esp_mesh_start().
 *
 * @param     pwd  pointer to the password
 * @param     len  password length
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_ARGUMENT
 *    - ESP_ERR_MESH_NOT_ALLOWED
 */
esp_err_t esp_mesh_set_ap_password(const uint8_t *pwd, int len);

/**
 * @brief     set mesh softAP authentication mode value
 *
 * @attention This API shall be called before esp_mesh_start().
 *
 * @param     authmode  authentication mode
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_ARGUMENT
 *    - ESP_ERR_MESH_NOT_ALLOWED
 */
esp_err_t esp_mesh_set_ap_authmode(wifi_auth_mode_t authmode);

/**
 * @brief     get mesh softAP authentication mode
 *
 * @return    authentication mode
 *
 */
wifi_auth_mode_t esp_mesh_get_ap_authmode(void);

/**
 * @brief     set mesh softAP max connection value
 *
 * @attention This API shall be called before esp_mesh_start().
 *
 * @param     connections  the number of max connections
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_ARGUMENT
 */
esp_err_t esp_mesh_set_ap_connections(int connections);

/**
 * @brief     get mesh softAP max connection configuration
 *
 * @return    the number of max connections
 *
 */
int esp_mesh_get_ap_connections(void);

/**
 * @brief     get current layer value over the mesh network
 *
 * @attention This API shall be called after having received the event MESH_EVENT_PARENT_CONNECTED.
 *
 * @return    layer value
 *
 */
int esp_mesh_get_layer(void);

/**
 * @brief     get parent BSSID
 *
 * @attention This API shall be called after having received the event MESH_EVENT_PARENT_CONNECTED.
 *
 * @param     bssid  pointer to parent BSSID
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 */
esp_err_t esp_mesh_get_parent_bssid(mesh_addr_t *bssid);

/**
 * @brief     return if the device is root
 *
 * @return    true/false
 *
 */
bool esp_mesh_is_root(void);

/**
 * @brief     enable/disable mesh networking self-organized, self-organized by default
 *            if self-organized is disabled, users should set a parent for this device via
 *            esp_mesh_set_parent();
 *
 * @attention This API could be called either before esp_mesh_start() or after esp_mesh_start().
 *
 * @param     enable  enable or disable self-organized networking
 * @param     select_parent  if enable self-organized networking, let the device select a new parent or
 *            keep connecting to the previous parent.
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 */
esp_err_t esp_mesh_set_self_organized(bool enable, bool select_parent);

/**
 * @brief     return if mesh networking is self-organized or not
 *
 * @return    true/false
 *
 */
bool esp_mesh_get_self_organized(void);

/**
 * @brief     root waive itself
 *            A device is elected to be a root during the networking mostly because it has a strong RSSI with router.
 *            If such superior conditions change, users could call this API to perform a root switch.
 *
 *            In this API, users could specify a desired root address to replace itself or specify an attempts value
 *            to ask current root to initiate a new round of voting. During the voting, a better root candidate would
 *            be expected to find to replace the current one.
 *            If no desired root candidate, the vote will try a specified attempts(at least 10 times), if no better
 *            root candidate is found, keep the current one. If a better candidate is found, the new better one will
 *            send a root switch request to the current root, current root will respond with a root switch acknowledgment.
 *            After that, the new candidate will connect to the router to be a new root, the previous root will disconnect
 *            with the router and choose another parent instead.
 *            So far, root switch is completed with minimal disruption to the whole mesh network.
 *
 * @attention This API is only called by root.
 *
 * @param     vote  vote configuration
 *            Specify a desired root address(Unimplemented)
 *            Attempts should be at least 10 times.
 *            if "vote" is set NULL, the vote will perform the default 10 times.
 * @param     reason  only accept MESH_VOTE_REASON_ROOT_INITIATED for now
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_QUEUE_FULL
 *    - ESP_ERR_MESH_DISCARD
 *    - ESP_FAIL
 */
esp_err_t esp_mesh_waive_root(const mesh_vote_t *vote, int reason);

/**
 * @brief     set vote percentage threshold for approval of being a root
 *            During the networking, only obtaining vote percentage reaches this threshold,
 *            the device could be a root.
 *
 * @attention This API shall be called before esp_mesh_start().
 *
 * @param     percentage  vote percentage threshold
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 */
esp_err_t esp_mesh_set_vote_percentage(float percentage);

/**
 * @brief     get vote percentage threshold for approval of being a root
 *
 * @return    percentage threshold
 */
float esp_mesh_get_vote_percentage(void);

/**
 * @brief     set mesh softAP associate expired time
 *            If mesh softAP hasn't received any data from an associated child within this time,
 *            mesh softAP will take this child inactive and disassociate it.
 *
 * @param     seconds
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 */
esp_err_t esp_mesh_set_ap_assoc_expire(int seconds);

/**
 * @brief     get mesh softAP associate expired time
 *
 * @return    seconds
 */
int esp_mesh_get_ap_assoc_expire(void);

/**
 * @brief     get total number of devices in current network(including root)
 *
 * @attention The returned value might be incorrect when the network is changing.
 **
 * @return    total number of devices(including root)
 */
int esp_mesh_get_total_node_num(void);

/**
 * @brief     get the number of devices in this device's sub-network(including self)
 *
 * @return    the number of devices over this device's sub-network(including self)
 */
int esp_mesh_get_routing_table_size(void);

/**
 * @brief     get routing table of this device's sub-network(including itself)
 *
 * @param     mac  pointer to routing table
 * @param     len  routing table size(in bytes)
 * @param     size  pointer to the number of devices in routing table(including itself)
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_ARGUMENT
 */
esp_err_t esp_mesh_get_routing_table(mesh_addr_t *mac, int len, int *size);

/**
 * @brief     post the toDS state to the mesh stack
 *
 * @attention This API is only for root.
 *
 * @param     reachable  this state represents if root is able to access external IP network
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 */
esp_err_t esp_mesh_post_toDS_state(bool reachable);

/**
 * @brief     return the number of packets pending in the queue waiting to be sent by the mesh stack
 *
 * @param     pending  pointer to the TX pending
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 */
esp_err_t esp_mesh_get_tx_pending(mesh_tx_pending_t *pending);

/**
 * @brief     return the number of packets available in the queue waiting to be received by applications
 *
 * @param     pending  pointer to the RX pending
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 */
esp_err_t esp_mesh_get_rx_pending(mesh_rx_pending_t *pending);

/**
 * @brief     return the number of packets could be accepted from the specified address
 *
 * @param     addr  self address or an associate children address
 * @param     xseqno_in  sequence number of the last received packet from the specified address
 *
 * @return    the number of upQ for a specified address
 */
int esp_mesh_available_txupQ_num(const mesh_addr_t *addr, uint32_t *xseqno_in);

/**
 * @brief     set queue size
 *
 * @attention This API shall be called before esp_mesh_start().
 *
 * @param     qsize  default:32(min:16)
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 */
esp_err_t esp_mesh_set_xon_qsize(int qsize);

/**
 * @brief     get queue size
 *
 * @return    qsize
 */
int esp_mesh_get_xon_qsize(void);

/**
 * @brief     set if allow more than one root existing in one network
 *
 * @param     allowed  allow or not
 *
 * @return
 *    - ESP_OK
 *    - ESP_WIFI_ERR_NOT_INIT
 *    - ESP_WIFI_ERR_NOT_START
 */
esp_err_t esp_mesh_allow_root_conflicts(bool allowed);

/**
 * @brief     check if allow more than one root to exist in one network
 *
 * @return    true/false
 */
bool esp_mesh_is_root_conflicts_allowed(void);

/**
 * @brief     set group ID addresses
 *
 * @param     addr  pointer to new group ID addresses
 * @param     num  the number of group ID addresses
 *
 * @return
 *    - ESP_OK
 *    - ESP_MESH_ERR_ARGUMENT
 */
esp_err_t esp_mesh_set_group_id(const mesh_addr_t *addr, int num);

/**
 * @brief     delete group ID addresses
 *
 * @param     addr  pointer to deleted group ID address
 * @param     num  the number of group ID addresses
 *
 * @return
 *    - ESP_OK
 *    - ESP_MESH_ERR_ARGUMENT
 */
esp_err_t esp_mesh_delete_group_id(const mesh_addr_t *addr, int num);

/**
 * @brief     get the number of group ID addresses
 *
 * @return    the number of group ID addresses
 */
int esp_mesh_get_group_num(void);

/**
 * @brief     get group ID addresses
 *
 * @param     addr  pointer to group ID addresses
 * @param     num  the number of group ID addresses
 *
 * @return
 *    - ESP_OK
 *    - ESP_MESH_ERR_ARGUMENT
 */
esp_err_t esp_mesh_get_group_list(mesh_addr_t *addr, int num);

/**
 * @brief     check if the specified group address is my group
 *
 * @return    true/false
 */
bool esp_mesh_is_my_group(const mesh_addr_t *addr);

/**
 * @brief     set mesh network capacity
 *
 * @attention This API shall be called before esp_mesh_start().
 *
 * @param     num  mesh network capacity
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_NOT_ALLOWED
 *    - ESP_MESH_ERR_ARGUMENT
 */
esp_err_t esp_mesh_set_capacity_num(int num);

/**
 * @brief    get mesh network capacity
 *
 * @return   mesh network capacity
 */
int esp_mesh_get_capacity_num(void);

/**
 * @brief    set mesh ie crypto functions
 *
 * @param     crypto_funcs  crypto functions for mesh ie
 *
 * @return
 *    - ESP_OK
 */
esp_err_t esp_mesh_set_ie_crypto_funcs(const mesh_crypto_funcs_t *crypto_funcs);

/**
 * @brief    set mesh ie crypto key
 *
 * @attention This API shall be called after esp_mesh_set_config() and before esp_mesh_start().
 *
 * @param     key  ASCII crypto key
 * @param     len  length in bytes, range:8~64
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_NOT_ALLOWED
 *    - ESP_ERR_MESH_NOT_CONFIG
 *    - ESP_MESH_ERR_ARGUMENT
 */
esp_err_t esp_mesh_set_ie_crypto_key(const char *key, int len);

/**
 * @brief    get mesh ie crypto key
 *
 * @param     key  ASCII crypto key
 * @param     len  length in bytes, range:8~64
 *
 * @return
 *    - ESP_OK
 *    - ESP_MESH_ERR_ARGUMENT
 */
esp_err_t esp_mesh_get_ie_crypto_key(char *key, int len);

/**
 * @brief    set delay time before starting root healing
 *
 * @param     delay_ms  delay time in milliseconds
 *
 * @return
 *    - ESP_OK
 */
esp_err_t esp_mesh_set_root_healing_delay(int delay_ms);

/**
 * @brief    get delay time before starting root healing
 *
 * @return    delay time in milliseconds
 */
int esp_mesh_get_root_healing_delay(void);

/**
 * @brief    set mesh event callback
 *
 * @param     event_cb  mesh event call back
 *
 * @return
 *    - ESP_OK
 */
esp_err_t esp_mesh_set_event_cb(const mesh_event_cb_t event_cb);

/**
 * @brief     set Fixed Root setting for the device
 *            If Fixed Root setting of the device is enabled, it won't compete to be a root.
 *            If a scenario needs a fixed root, all devices in this network shall enable this setting.
 *
 * @param     enable  enable or not
 *
 * @return
 *    - ESP_OK
 */
esp_err_t esp_mesh_fix_root(bool enable);

/**
 * @brief     check if Fixed Root setting is enabled
 *            Fixed Root setting can be changed by API esp_mesh_fix_root().
 *            Fixed Root setting can also be changed by event MESH_EVENT_ROOT_FIXED.
 *
 * @return    true/false
 */
bool esp_mesh_is_root_fixed(void);

/**
 * @brief     set a specified parent
 *
 * @param     parent  parent configuration, the ssid and the channel of the parent are mandatory.
 * @param     parent_mesh_id  parent mesh ID, if not set, use the device default one.
 * @param     my_type  my mesh type
 * @param     my_layer  my mesh layer
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_ARGUMENT
 *    - ESP_ERR_MESH_NOT_CONFIG
 */
esp_err_t esp_mesh_set_parent(const wifi_config_t *parent, const mesh_addr_t *parent_mesh_id, mesh_type_t my_type, int my_layer);

/**
 * @brief     get mesh networking IE length of one AP
 *
 * @param     len  mesh networking IE length
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_WIFI_NOT_INIT
 *    - ESP_ERR_WIFI_ARG
 *    - ESP_ERR_WIFI_FAIL
 */
esp_err_t esp_mesh_scan_get_ap_ie_len(int *len);

/**
 * @brief     get AP record
 *            Different from esp_wifi_scan_get_ap_records(), this API only gets one of scanned APs each time.
 *
 * @param     ap_record  pointer to the AP record
 * @param     buffer  pointer to the mesh networking IE of this AP
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_WIFI_NOT_INIT
 *    - ESP_ERR_WIFI_ARG
 *    - ESP_ERR_WIFI_FAIL
 */
esp_err_t esp_mesh_scan_get_ap_record(wifi_ap_record_t *ap_record, void *buffer);

/**
 * @brief     flush upstream packets pending in to_parent queue and to_parent_p2p queue
 *
 * @return
 *    - ESP_OK
 */
esp_err_t esp_mesh_flush_upstream_packets(void);

#ifdef __cplusplus
}
#endif
#endif /* __ESP_MESH_H__ */
