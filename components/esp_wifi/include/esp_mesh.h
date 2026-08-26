/*
 * SPDX-FileCopyrightText: 2017-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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
 *  |            |                       Wi-Fi Driver               |              |
 *  |            |--------------------------------------------------|              |
 *  |            |                                                                 |
 *  |            |                       Platform HAL                              |
 *  |------------------------------------------------------------------------------|
 *
 *   System Events delivery:
 *
 *  |---------------|
 *  |               |                    default handler
 *  |  Wi-Fi stack  |     events     |---------------------|
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
 *  Mesh event contains Wi-Fi connection states on station interface, children connection states on softAP interface and etc..
 *  Applications need to register a mesh event callback handler by API esp_mesh_set_config() firstly.
 *  This handler is to receive events posted from mesh stack and LwIP stack.
 *  Applications could add relative handler for each event.
 *  Examples:
 *  (1) Applications could use Wi-Fi station connect states to decide when to send data to its parent, to the root or to external IP network;
 *  (2) Applications could use Wi-Fi softAP states to decide when to send data to its children.
 *
 *  In present implementation, applications are able to access mesh stack directly without having to go through LwIP stack.
 *  Applications use esp_mesh_send() and esp_mesh_recv() to send and receive messages over the mesh network.
 *  In mesh stack design, normal devices don't require LwIP stack, but if any of these devices could be promoted to a root node in runtime,
 *  (due to automatic or manual topology reconfiguration) the TCP/IP stack should be initialized (for the root code to access external IP network)
 *
 *  Over the mesh network, only the root is able to access external IP network.
 *  In application mesh event handler, once a device becomes a root, start DHCP client immediately whether DHCP is chosen.
 */

#ifndef __ESP_MESH_H__
#define __ESP_MESH_H__

#include "esp_err.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_mesh_internal.h"
#include "lwip/ip_addr.h"

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
 * @brief Mesh error code definition
 */
#define ESP_ERR_MESH_WIFI_NOT_START       (ESP_ERR_MESH_BASE + 1)    /**< Wi-Fi isn't started */
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
#define ESP_ERR_MESH_INTERFACE            (ESP_ERR_MESH_BASE + 19)   /**< low-level Wi-Fi interface error */
#define ESP_ERR_MESH_DISCARD_DUPLICATE    (ESP_ERR_MESH_BASE + 20)   /**< discard the packet due to the duplicate sequence number */
#define ESP_ERR_MESH_DISCARD              (ESP_ERR_MESH_BASE + 21)   /**< discard the packet */
#define ESP_ERR_MESH_VOTING               (ESP_ERR_MESH_BASE + 22)   /**< vote in progress */
#define ESP_ERR_MESH_XMIT                 (ESP_ERR_MESH_BASE + 23)   /**< TX fail, the tx state is a value other than timeout and disconnect */
#define ESP_ERR_MESH_QUEUE_READ           (ESP_ERR_MESH_BASE + 24)   /**< error in reading queue */
#define ESP_ERR_MESH_PS                   (ESP_ERR_MESH_BASE + 25)   /**< mesh PS is not specified as enable or disable */
#define ESP_ERR_MESH_RECV_RELEASE         (ESP_ERR_MESH_BASE + 26)   /**< release esp_mesh_recv_toDS */

/**
 * @brief Flags bitmap for esp_mesh_send() and esp_mesh_recv()
 */
#define MESH_DATA_ENC           (0x01)  /**< data encrypted (Unimplemented) */
#define MESH_DATA_P2P           (0x02)  /**< point-to-point delivery over the mesh network */
#define MESH_DATA_FROMDS        (0x04)  /**< receive from external IP network */
#define MESH_DATA_TODS          (0x08)  /**< identify this packet is target to external IP network */
#define MESH_DATA_NONBLOCK      (0x10)  /**< esp_mesh_send() non-block */
#define MESH_DATA_DROP          (0x20)  /**< in the situation of the root having been changed, identify this packet can be dropped by new root */
#define MESH_DATA_GROUP         (0x40)  /**< identify this packet is target to a group address */

/**
 * @brief Option definitions for esp_mesh_send() and esp_mesh_recv()
 */
#define MESH_OPT_SEND_GROUP     (7)     /**< data transmission by group; used with esp_mesh_send() and shall have payload */
#define MESH_OPT_RECV_DS_ADDR   (8)     /**< return a remote IP address; used with esp_mesh_send() and esp_mesh_recv() */

/**
 * @brief Flag of mesh networking IE
 */
#define MESH_ASSOC_FLAG_MAP_ASSOC           (0x01)     /**< Mesh AP doesn't detect children leave yet */
#define MESH_ASSOC_FLAG_VOTE_IN_PROGRESS    (0x02)     /**< station in vote, set when root vote start, clear when connect to router or when root switch*/
#define MESH_ASSOC_FLAG_STA_VOTED           (0x04)     /**< station vote done, set when connect to router */
#define MESH_ASSOC_FLAG_NETWORK_FREE        (0x08)     /**< no root in current network */
#define MESH_ASSOC_FLAG_STA_VOTE_EXPIRE     (0x10)     /**< the voted address is expired, means the voted device lose the chance to be root */
#define MESH_ASSOC_FLAG_ROOTS_FOUND         (0x20)     /**< roots conflict is found, means that there are at least two roots in the mesh network */
#define MESH_ASSOC_FLAG_ROOT_FIXED          (0x40)     /**< the root is fixed in the mesh network */

/**
 * @brief Mesh PS (Power Save) duty cycle type
 */
#define MESH_PS_DEVICE_DUTY_REQUEST         (0x01)    /**< requests to join a network PS without specifying a device duty cycle. After the
                                                           device joins the network, a network duty cycle will be provided by the network */
#define MESH_PS_DEVICE_DUTY_DEMAND          (0x04)    /**< requests to join a network PS and specifies a demanded device duty cycle */
#define MESH_PS_NETWORK_DUTY_MASTER         (0x80)    /**< indicates the device is the NWK-DUTY-MASTER (network duty cycle master) */

/**
 * @brief Mesh PS (Power Save) duty cycle applied rule
 */
#define MESH_PS_NETWORK_DUTY_APPLIED_ENTIRE         (0)    /** the specified network duty is applied to the entire network <*/
#define MESH_PS_NETWORK_DUTY_APPLIED_UPLINK         (1)    /** the specified network duty is applied to only the up-link path <*/

/*******************************************************
 *                Enumerations
 *******************************************************/
/**
 * @brief Enumerated list of mesh event id
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
    MESH_EVENT_NO_PARENT_FOUND,         /**< no parent found */
    MESH_EVENT_LAYER_CHANGE,            /**< layer changes over the mesh network */
    MESH_EVENT_TODS_STATE,              /**< state represents whether the root is able to access external IP network.
                                             This state is a manual event that needs to be triggered with esp_mesh_post_toDS_state(). */
    MESH_EVENT_VOTE_STARTED,            /**< the process of voting a new root is started either by children or by the root */
    MESH_EVENT_VOTE_STOPPED,            /**< the process of voting a new root is stopped */
    MESH_EVENT_ROOT_ADDRESS,            /**< the root address is obtained. It is posted by mesh stack automatically. */
    MESH_EVENT_ROOT_SWITCH_REQ,         /**< root switch request sent from a new voted root candidate */
    MESH_EVENT_ROOT_SWITCH_ACK,         /**< root switch acknowledgment responds the above request sent from current root */
    MESH_EVENT_ROOT_ASKED_YIELD,        /**< the root is asked yield by a more powerful existing root. If self organized is disabled
                                             and this device is specified to be a root by users, users should set a new parent
                                             for this device. if self organized is enabled, this device will find a new parent
                                             by itself, users could ignore this event. */
    MESH_EVENT_ROOT_FIXED,              /**< when devices join a network, if the setting of Fixed Root for one device is different
                                             from that of its parent, the device will update the setting the same as its parent's.
                                             Fixed Root Setting of each device is variable as that setting changes of the root. */
    MESH_EVENT_SCAN_DONE,               /**< if self-organized networking is disabled, user can call esp_wifi_scan_start() to trigger
                                             this event, and add the corresponding scan done handler in this event. */
    MESH_EVENT_NETWORK_STATE,           /**< network state, such as whether current mesh network has a root. */
    MESH_EVENT_STOP_RECONNECTION,       /**< the root stops reconnecting to the router and non-root devices stop reconnecting to their parents. */
    MESH_EVENT_FIND_NETWORK,            /**< when the channel field in mesh configuration is set to zero, mesh stack will perform a
                                             full channel scan to find a mesh network that can join, and return the channel value
                                             after finding it. */
    MESH_EVENT_ROUTER_SWITCH,           /**< if users specify BSSID of the router in mesh configuration, when the root connects to another
                                             router with the same SSID, this event will be posted and the new router information is attached. */
    MESH_EVENT_PS_PARENT_DUTY,          /**< parent duty */
    MESH_EVENT_PS_CHILD_DUTY,           /**< child duty */
    MESH_EVENT_PS_DEVICE_DUTY,          /**< device duty */
    MESH_EVENT_MAX,
} mesh_event_id_t;

/** @brief ESP-MESH event base declaration */
ESP_EVENT_DECLARE_BASE(MESH_EVENT);

/**
 * @brief Device type
 */
typedef enum {
    MESH_IDLE,    /**< hasn't joined the mesh network yet */
    MESH_ROOT,    /**< the only sink of the mesh network. Has the ability to access external IP network */
    MESH_NODE,    /**< intermediate device. Has the ability to forward packets over the mesh network */
    MESH_LEAF,    /**< has no forwarding ability */
    MESH_STA,     /**< connect to router with a standalone Wi-Fi station mode, no network expansion capability */
} mesh_type_t;

/**
 * @brief Protocol of transmitted application data
 */
typedef enum {
    MESH_PROTO_BIN,     /**< binary */
    MESH_PROTO_HTTP,    /**< HTTP protocol */
    MESH_PROTO_JSON,    /**< JSON format */
    MESH_PROTO_MQTT,    /**< MQTT protocol */
    MESH_PROTO_AP,      /**< IP network mesh communication of node's AP interface */
    MESH_PROTO_STA,     /**< IP network mesh communication of node's STA interface */
} mesh_proto_t;

/**
 * @brief For reliable transmission, mesh stack provides three type of services
 */
typedef enum {
    MESH_TOS_P2P,    /**< provide P2P (point-to-point) retransmission on mesh stack by default */
    MESH_TOS_E2E,    /**< provide E2E (end-to-end) retransmission on mesh stack (Unimplemented) */
    MESH_TOS_DEF,    /**< no retransmission on mesh stack */
} mesh_tos_t;

/**
 * @brief Vote reason
 */
typedef enum {
    MESH_VOTE_REASON_ROOT_INITIATED = 1,    /**< vote is initiated by the root */
    MESH_VOTE_REASON_CHILD_INITIATED,       /**< vote is initiated by children */
} mesh_vote_reason_t;

/**
 * @brief Mesh disconnect reason code
 */
typedef enum {
    MESH_REASON_CYCLIC = 100,               /**< cyclic is detected */
    MESH_REASON_PARENT_IDLE,                /**< parent is idle */
    MESH_REASON_LEAF,                       /**< the connected device is changed to a leaf */
    MESH_REASON_DIFF_ID,                    /**< in different mesh ID */
    MESH_REASON_ROOTS,                      /**< root conflict is detected */
    MESH_REASON_PARENT_STOPPED,             /**< parent has stopped the mesh */
    MESH_REASON_SCAN_FAIL,                  /**< scan fail */
    MESH_REASON_IE_UNKNOWN,                 /**< unknown IE */
    MESH_REASON_WAIVE_ROOT,                 /**< waive root */
    MESH_REASON_PARENT_WORSE,               /**< parent with very poor RSSI */
    MESH_REASON_EMPTY_PASSWORD,             /**< use an empty password to connect to an encrypted parent */
    MESH_REASON_PARENT_UNENCRYPTED,         /**< connect to an unencrypted parent/router */
} mesh_disconnect_reason_t;

/**
 * @brief Mesh topology
 */
typedef enum {
    MESH_TOPO_TREE,                         /**< tree topology */
    MESH_TOPO_CHAIN,                        /**< chain topology */
} esp_mesh_topology_t;

/*******************************************************
 *                Structures
 *******************************************************/
/**
 * @brief IP address and port
 */
typedef struct {
    esp_ip4_addr_t ip4;    /**< IP address */
    uint16_t port;     /**< port */
} __attribute__((packed)) mip_t;

/**
 * @brief Mesh address
 */
typedef union {
    uint8_t addr[6];    /**< mac address */
    mip_t mip;          /**< mip address */
} mesh_addr_t;

/**
 * @brief Channel switch information
 */
typedef struct {
    uint8_t channel;    /**< new channel */
} mesh_event_channel_switch_t;

/**
 * @brief Parent connected information
 */
typedef struct {
    wifi_event_sta_connected_t connected; /**< parent information, same as Wi-Fi event SYSTEM_EVENT_STA_CONNECTED does */
    uint16_t self_layer;                  /**< layer */
    uint8_t duty;                         /**< parent duty */
} mesh_event_connected_t;

/**
 * @brief No parent found information
 */
typedef struct {
    int scan_times;    /**< scan times being through */
} mesh_event_no_parent_found_t;

/**
 * @brief Layer change information
 */
typedef struct {
    uint16_t new_layer; /**< new layer */
} mesh_event_layer_change_t;

/**
 * @brief The reachability of the root to a DS (distribute system)
 */
typedef enum {
    MESH_TODS_UNREACHABLE,    /**< the root isn't able to access external IP network */
    MESH_TODS_REACHABLE,      /**< the root is able to access external IP network */
} mesh_event_toDS_state_t;

/**
 * @brief vote started information
 */
typedef struct {
    int reason;             /**< vote reason, vote could be initiated by children or by the root itself */
    int attempts;           /**< max vote attempts before stopped */
    mesh_addr_t rc_addr;    /**< root address specified by users via API esp_mesh_waive_root() */
} mesh_event_vote_started_t;

/**
 * @brief find a mesh network that this device can join
 */
typedef struct {
    uint8_t channel;            /**< channel number of the new found network */
    uint8_t router_bssid[6];    /**< router BSSID */
} mesh_event_find_network_t;

/**
 * @brief Root address
 */
typedef mesh_addr_t mesh_event_root_address_t;

/**
 * @brief Parent disconnected information
 */
typedef wifi_event_sta_disconnected_t mesh_event_disconnected_t;

/**
 * @brief Child connected information
 */
typedef wifi_event_ap_staconnected_t mesh_event_child_connected_t;

/**
 * @brief Child disconnected information
 */
typedef wifi_event_ap_stadisconnected_t mesh_event_child_disconnected_t;

/**
 * @brief Root switch request information
 */
typedef struct {
    int reason;             /**< root switch reason, generally root switch is initialized by users via API esp_mesh_waive_root() */
    mesh_addr_t rc_addr;    /**< the address of root switch requester */
} mesh_event_root_switch_req_t;

/**
 * @brief Other powerful root address
 */
typedef struct {
    int8_t rssi;           /**< rssi with router */
    uint16_t capacity;     /**< the number of devices in current network */
    uint8_t addr[6];       /**< other powerful root address */
} mesh_event_root_conflict_t;

/**
 * @brief Routing table change
 */
typedef struct {
    uint16_t rt_size_new;      /**< the new value */
    uint16_t rt_size_change;   /**< the changed value */
} mesh_event_routing_table_change_t;

/**
 * @brief Root fixed
 */
typedef struct {
    bool is_fixed;     /**< status */
} mesh_event_root_fixed_t;

/**
 * @brief Scan done　event information
 */
typedef struct {
    uint8_t  number;     /**< the number of APs scanned */
} mesh_event_scan_done_t;

/**
 * @brief Network state information
 */
typedef struct {
    bool is_rootless;     /**< whether current mesh network has a root */
} mesh_event_network_state_t;

/**
 * @brief New router information
 */
typedef wifi_event_sta_connected_t mesh_event_router_switch_t;

/**
 * @brief PS duty information
 */
typedef struct {
    uint8_t duty;                                 /**< parent or child duty */
    mesh_event_child_connected_t child_connected; /**< child info */
} mesh_event_ps_duty_t;

/**
 * @brief Mesh event information
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
                                                                external IP network. This state indicates right now whether the root is capable of sending
                                                                packets out. If not, devices had better to wait until this state changes to be
                                                                MESH_TODS_REACHABLE. */
    mesh_event_vote_started_t vote_started;                /**< vote started */
    mesh_event_root_address_t root_addr;                   /**< root address */
    mesh_event_root_switch_req_t switch_req;               /**< root switch request */
    mesh_event_root_conflict_t root_conflict;              /**< other powerful root */
    mesh_event_root_fixed_t root_fixed;                    /**< fixed root */
    mesh_event_scan_done_t scan_done;                      /**< scan done */
    mesh_event_network_state_t network_state;              /**< network state, such as whether current mesh network has a root. */
    mesh_event_find_network_t find_network;                /**< network found that can join */
    mesh_event_router_switch_t router_switch;              /**< new router information */
    mesh_event_ps_duty_t ps_duty;                          /**< PS duty information */
} mesh_event_info_t;

/**
 * @brief Mesh option
 */
typedef struct {
    uint8_t type;    /**< option type */
    uint16_t len;    /**< option length */
    uint8_t *val;    /**< option value */
} __attribute__((packed)) mesh_opt_t;

/**
 * @brief Mesh data for esp_mesh_send() and esp_mesh_recv()
 */
typedef struct {
    uint8_t *data;         /**< data */
    uint16_t size;         /**< data size */
    mesh_proto_t proto;    /**< data protocol */
    mesh_tos_t tos;        /**< data type of service */
} mesh_data_t;

/**
 * @brief Router configuration
 */
typedef struct {
    uint8_t ssid[32];             /**< SSID */
    uint8_t ssid_len;             /**< length of SSID */
    uint8_t bssid[6];             /**< BSSID, if this value is specified, users should also specify "allow_router_switch". */
    uint8_t password[64];         /**< password */
    bool allow_router_switch;     /**< if the BSSID is specified and this value is also set, when the router of this specified BSSID
                                       fails to be found after "fail" (mesh_attempts_t) times, the whole network is allowed to switch
                                       to another router with the same SSID. The new router might also be on a different channel.
                                       The default value is false.
                                       There is a risk that if the password is different between the new switched router and the previous
                                       one, the mesh network could be established but the root will never connect to the new switched router. */
} mesh_router_t;

/**
 * @brief Mesh softAP configuration
 */
typedef struct {
    uint8_t password[64];              /**< mesh softAP password */
    /**
     * max number of stations allowed to connect in, default 6, max 10
     * = max_connection + nonmesh_max_connection
     */
    uint8_t max_connection;            /**< max mesh connections */
    uint8_t nonmesh_max_connection;    /**< max non-mesh connections */
} mesh_ap_cfg_t;

/**
 * @brief Mesh initialization configuration
 */
typedef struct {
    uint8_t channel;                            /**< channel, the mesh network on */
    bool allow_channel_switch;                  /**< if this value is set, when "fail" (mesh_attempts_t) times is reached, device will change to
                                                     a full channel scan for a network that could join. The default value is false. */
    mesh_addr_t mesh_id;                        /**< mesh network identification */
    mesh_router_t router;                       /**< router configuration */
    mesh_ap_cfg_t mesh_ap;                      /**< mesh softAP configuration */
    const mesh_crypto_funcs_t *crypto_funcs;    /**< crypto functions */
} mesh_cfg_t;

/**
 * @brief Vote address configuration
 */
typedef union {
    int attempts;           /**< max vote attempts before a new root is elected automatically by mesh network. (min:15, 15 by default) */
    mesh_addr_t rc_addr;    /**< a new root address specified by users for API esp_mesh_waive_root() */
} mesh_rc_config_t;

/**
 * @brief Vote
 */
typedef struct {
    float percentage;           /**< vote percentage threshold for approval of being a root */
    bool is_rc_specified;       /**< if true, rc_addr shall be specified (Unimplemented).
                                     if false, attempts value shall be specified to make network start root election. */
    mesh_rc_config_t config;    /**< vote address configuration */
} mesh_vote_t;

/**
 * @brief The number of packets pending in the queue waiting to be sent by the mesh stack
 */
typedef struct {
    int to_parent;        /**< to parent queue */
    int to_parent_p2p;    /**< to parent (P2P) queue */
    int to_child;         /**< to child queue */
    int to_child_p2p;     /**< to child (P2P) queue */
    int mgmt;             /**< management queue */
    int broadcast;        /**< broadcast and multicast queue */
} mesh_tx_pending_t;

/**
 * @brief The number of packets available in the queue waiting to be received by applications
 */
typedef struct {
    int toDS;      /**< to external DS */
    int toSelf;    /**< to self */
} mesh_rx_pending_t;

/*******************************************************
 *                Variable Declaration
 *******************************************************/
/* mesh IE crypto callback function */
extern const mesh_crypto_funcs_t g_wifi_default_mesh_crypto_funcs;

#define MESH_INIT_CONFIG_DEFAULT() { \
    .crypto_funcs = &g_wifi_default_mesh_crypto_funcs, \
}

/*******************************************************
 *                Function Definitions
 *******************************************************/
/**
 * @brief      Mesh initialization
 *             - Check whether Wi-Fi is started.
 *             - Initialize mesh global variables with default values.
 *
 * @attention  This API shall be called after Wi-Fi is started.
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 */
esp_err_t esp_mesh_init(void);

/**
 * @brief      Mesh de-initialization
 *
 *             - Release resources and stop the mesh
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 */
esp_err_t esp_mesh_deinit(void);

/**
 * @brief      Start mesh
 *             - Initialize mesh IE.
 *             - Start mesh network management service.
 *             - Create TX and RX queues according to the configuration.
 *             - Register mesh packets receive callback.
 *
 * @attention　　This API shall be called after mesh initialization and configuration.
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
 * @brief      Stop mesh
 *             - Deinitialize mesh IE.
 *             - Disconnect with current parent.
 *             - Disassociate all currently associated children.
 *             - Stop mesh network management service.
 *             - Unregister mesh packets receive callback.
 *             - Delete TX and RX queues.
 *             - Release resources.
 *             - Restore Wi-Fi softAP to default settings if Wi-Fi dual mode is enabled.
 *             - Set Wi-Fi Power Save type to WIFI_PS_NONE.
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 */
esp_err_t esp_mesh_stop(void);

/**
 * @brief      Send a packet over the mesh network
 *             - Send a packet to any device in the mesh network.
 *             - Send a packet to external IP network.
 *
 * @attention  This API is not reentrant.
 *
 * @param[in]  to  the address of the final destination of the packet
 *             - If the packet is to the root, set this parameter to NULL.
 *             - If the packet is to an external IP network, set this parameter to the IPv4:PORT combination.
 *               This packet will be delivered to the root firstly, then users need to call esp_mesh_recv_toDS() on the root node to forward this
 *               packet to the final IP server address.
 * @param[in]  data  pointer to a sending mesh packet
 *             - Field size should not exceed MESH_MPS. Note that the size of one mesh packet should not exceed MESH_MTU.
 *             - Field proto should be set to data protocol in use (default is MESH_PROTO_BIN for binary).
 *             - Field tos should be set to transmission tos (type of service) in use (default is MESH_TOS_P2P for point-to-point reliable).
 *               - If the packet is to the root, MESH_TOS_P2P must be set to ensure reliable transmission.
 *               - As long as the MESH_TOS_P2P is set, the API is blocking, even if the flag is set with MESH_DATA_NONBLOCK.
 *               - As long as the MESH_TOS_DEF is set, the API is non-blocking.
 * @param[in]  flag  bitmap for data sent
 *             - Flag is at least one of the three MESH_DATA_P2P/MESH_DATA_FROMDS/MESH_DATA_TODS, which represents the direction of packet sending.
 *             -  Speed up the route search
 *               - If the packet is to an internal device, MESH_DATA_P2P should be set.
 *               - If the packet is to the root ("to" parameter isn't NULL) or to external IP network, MESH_DATA_TODS should be set.
 *               - If the packet is from the root to an internal device, MESH_DATA_FROMDS should be set.
 *             - Specify whether this API is blocking or non-blocking, blocking by default.
 *             - In the situation of the root change, MESH_DATA_DROP identifies this packet can be dropped by the new root
 *               for upstream data to external IP network, we try our best to avoid data loss caused by the root change, but
 *               there is a risk that the new root is running out of memory because most of memory is occupied by the pending data which
 *               isn't read out in time by esp_mesh_recv_toDS().
 *
 *               Generally, we suggest esp_mesh_recv_toDS() is called after a connection with IP network is created. Thus data outgoing
 *               to external IP network via socket is just from reading esp_mesh_recv_toDS() which avoids unnecessary memory copy.
 *
 * @param[in]  opt  options
 *             - In case of sending a packet to a certain group, MESH_OPT_SEND_GROUP is a good choice.
 *               In this option, the value field should be set to the target receiver addresses in this group.
 *             - Root sends a packet to an internal device, this packet is from external IP network in case the receiver device responds
 *             this packet, MESH_OPT_RECV_DS_ADDR is required to attach the target DS address.
 * @param[in]  opt_count  option count
 *             - Currently, this API only takes one option, so opt_count is only supported to be 1.
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
 *    - ESP_ERR_MESH_NOT_SUPPORT
 *    - ESP_ERR_MESH_XMIT
 */
esp_err_t esp_mesh_send(const mesh_addr_t *to, const mesh_data_t *data,
                        int flag, const mesh_opt_t opt[],  int opt_count);
/**
 * @brief      Set blocking time of esp_mesh_send()
 *             - Suggest to set the blocking time to at least 5s when the environment is poor. Otherwise, esp_mesh_send() may timeout frequently.
 *
 * @attention  This API shall be called before mesh is started.
 *
 * @param[in]  time_ms  blocking time of esp_mesh_send(), unit:ms
 *
 * @return
 *    - ESP_OK
 */
esp_err_t esp_mesh_send_block_time(uint32_t time_ms);

/**
 * @brief      Receive a packet targeted to self over the mesh network
 *
 * @attention  Mesh RX queue should be checked regularly to avoid running out of memory.
 *             - Use esp_mesh_get_rx_pending() to check the number of packets available in the queue waiting
 *             to be received by applications.
 *
 * @param[out] from  the address of the original source of the packet
 * @param[out] data  pointer to the received mesh packet
 *             - Field proto is the data protocol in use. Should follow it to parse the received data.
 *             - Field tos is the transmission tos (type of service) in use.
 * @param[in]  timeout_ms  wait time if a packet isn't immediately available (0:no wait, portMAX_DELAY:wait forever)
 * @param[out] flag  bitmap for data received
 *             - MESH_DATA_FROMDS represents data from external IP network
 *             - MESH_DATA_TODS represents data directed upward within the mesh network
 *
 *             flag could be MESH_DATA_FROMDS or MESH_DATA_TODS.
 * @param[out] opt  options desired to receive
 *             - MESH_OPT_RECV_DS_ADDR attaches the DS address
 * @param[in]  opt_count  option count desired to receive
 *             - Currently, this API only takes one option, so opt_count is only supported to be 1.
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
 * @brief      Receive a packet targeted to external IP network
 *             - Root uses this API to receive packets destined to external IP network
 *             - Root forwards the received packets to the final destination via socket.
 *             - If no socket connection is ready to send out the received packets and this esp_mesh_recv_toDS()
 *             hasn't been called by applications, packets from the whole mesh network will be pending in toDS queue.
 *
 *             Use esp_mesh_get_rx_pending() to check the number of packets available in the queue waiting
 *             to be received by applications in case of running out of memory in the root.
 *
 *             Using esp_mesh_set_xon_qsize() users may configure the RX queue size, default:32. If this size is too large,
 *             and esp_mesh_recv_toDS() isn't called in time, there is a risk that a great deal of memory is occupied
 *             by the pending packets. If this size is too small, it will impact the efficiency on upstream. How to
 *             decide this value depends on the specific application scenarios.
 *
 * @attention  This API is only called by the root.
 *
 * @param[out] from  the address of the original source of the packet
 * @param[out] to  the address contains remote IP address and port (IPv4:PORT)
 * @param[out] data  pointer to the received packet
 *             - Contain the protocol and applications should follow it to parse the data.
 * @param[in]  timeout_ms  wait time if a packet isn't immediately available (0:no wait, portMAX_DELAY:wait forever)
 * @param[out] flag  bitmap for data received
 *             - MESH_DATA_TODS represents the received data target to external IP network. Root shall forward this data to external IP network via the association with router.
 *
 *             flag could be MESH_DATA_TODS.
 * @param[out] opt  options desired to receive
 * @param[in]  opt_count  option count desired to receive
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_ARGUMENT
 *    - ESP_ERR_MESH_NOT_START
 *    - ESP_ERR_MESH_TIMEOUT
 *    - ESP_ERR_MESH_DISCARD
 *    - ESP_ERR_MESH_RECV_RELEASE
 */
esp_err_t esp_mesh_recv_toDS(mesh_addr_t *from, mesh_addr_t *to,
                             mesh_data_t *data, int timeout_ms, int *flag, mesh_opt_t opt[],
                             int opt_count);

/**
 * @brief      Set mesh stack configuration
 *             - Use MESH_INIT_CONFIG_DEFAULT() to initialize the default values, mesh IE is encrypted by default.
 *             - Mesh network is established on a fixed channel (1-14).
 *             - Mesh event callback is mandatory.
 *             - Mesh ID is an identifier of an MBSS. Nodes with the same mesh ID can communicate with each other.
 *             - Regarding to the router configuration, if the router is hidden, BSSID field is mandatory.
 *
 *             If BSSID field isn't set and there exists more than one router with same SSID, there is a risk that more
 *             roots than one connected with different BSSID will appear. It means more than one mesh network is established
 *             with the same mesh ID.
 *
 *             Root conflict function could eliminate redundant roots connected with the same BSSID, but couldn't handle roots
 *             connected with different BSSID. Because users might have such requirements of setting up routers with same SSID
 *             for the future replacement. But in that case, if the above situations happen, please make sure applications
 *             implement forward functions on the root to guarantee devices in different mesh networks can communicate with each other.
 *             max_connection of mesh softAP is limited by the max number of Wi-Fi softAP supported (max:10).
 *
 * @attention  This API shall be called before mesh is started after mesh is initialized.
 *
 * @param[in]  config  pointer to mesh stack configuration
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_ARGUMENT
 *    - ESP_ERR_MESH_NOT_ALLOWED
 */
esp_err_t esp_mesh_set_config(const mesh_cfg_t *config);

/**
 * @brief      Get mesh stack configuration
 *
 * @param[out] config  pointer to mesh stack configuration
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_ARGUMENT
 */
esp_err_t esp_mesh_get_config(mesh_cfg_t *config);

/**
 * @brief      Get router configuration
 *
 * @attention  This API is used to dynamically modify the router configuration after mesh is configured.
 *
 * @param[in]  router  pointer to router configuration
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_ARGUMENT
 */
esp_err_t esp_mesh_set_router(const mesh_router_t *router);

/**
 * @brief      Get router configuration
 *
 * @param[out] router  pointer to router configuration
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_ARGUMENT
 */
esp_err_t esp_mesh_get_router(mesh_router_t *router);

/**
 * @brief      Set mesh network ID
 *
 * @attention  This API is used to dynamically modify the mesh network ID.
 *
 * @param[in]  id  pointer to mesh network ID
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_ARGUMENT: invalid argument
 */
esp_err_t esp_mesh_set_id(const mesh_addr_t *id);

/**
 * @brief      Get mesh network ID
 *
 * @param[out] id  pointer to mesh network ID
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_ARGUMENT
 */
esp_err_t esp_mesh_get_id(mesh_addr_t *id);

/**
 * @brief      Designate device type over the mesh network
 *            - MESH_IDLE: designates a device as a self-organized node for a mesh network
 *            - MESH_ROOT: designates the root node for a mesh network
 *            - MESH_LEAF: designates a device as a standalone Wi-Fi station that connects to a parent
 *            - MESH_STA: designates a device as a standalone Wi-Fi station that connects to a router
 *
 * @param[in]  type  device type
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_NOT_ALLOWED
 */
esp_err_t esp_mesh_set_type(mesh_type_t type);

/**
 * @brief      Get device type over mesh network
 *
 * @attention  This API shall be called after having received the event MESH_EVENT_PARENT_CONNECTED.
 *
 * @return     mesh type
 *
 */
mesh_type_t esp_mesh_get_type(void);

/**
 * @brief      Set network max layer value
 *             - for tree topology, the max is 25.
 *             - for chain topology, the max is 1000.
 *             - Network max layer limits the max hop count.
 *
 * @attention  This API shall be called before mesh is started.
 *
 * @param[in]  max_layer  max layer value
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_ARGUMENT
 *    - ESP_ERR_MESH_NOT_ALLOWED
 */
esp_err_t esp_mesh_set_max_layer(int max_layer);

/**
 * @brief      Get max layer value
 *
 * @return     max layer value
 */
int esp_mesh_get_max_layer(void);

/**
 * @brief      Set mesh softAP password
 *
 * @attention  This API shall be called before mesh is started.
 *
 * @param[in]  pwd  pointer to the password
 * @param[in]  len  password length
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_ARGUMENT
 *    - ESP_ERR_MESH_NOT_ALLOWED
 */
esp_err_t esp_mesh_set_ap_password(const uint8_t *pwd, int len);

/**
 * @brief      Set mesh softAP authentication mode
 *
 * @attention  This API shall be called before mesh is started.
 *
 * @param[in]  authmode  authentication mode
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_ARGUMENT
 *    - ESP_ERR_MESH_NOT_ALLOWED
 */
esp_err_t esp_mesh_set_ap_authmode(wifi_auth_mode_t authmode);

/**
 * @brief      Get mesh softAP authentication mode
 *
 * @return     authentication mode
 */
wifi_auth_mode_t esp_mesh_get_ap_authmode(void);

/**
 * @brief      Set mesh max connection value
 *             - Set mesh softAP max connection = mesh max connection + non-mesh max connection
 *
 * @attention  This API shall be called before mesh is started.
 *
 * @param[in]  connections  the number of max connections
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_ARGUMENT
 */
esp_err_t esp_mesh_set_ap_connections(int connections);

/**
 * @brief      Get mesh max connection configuration
 *
 * @return     the number of mesh max connections
 */
int esp_mesh_get_ap_connections(void);

/**
 * @brief      Get non-mesh max connection configuration
 *
 * @return     the number of non-mesh max connections
 */
int esp_mesh_get_non_mesh_connections(void);

/**
 * @brief      Get current layer value over the mesh network
 *
 * @attention  This API shall be called after having received the event MESH_EVENT_PARENT_CONNECTED.
 *
 * @return     layer value
 *
 */
int esp_mesh_get_layer(void);

/**
 * @brief      Get the parent BSSID
 *
 * @attention  This API shall be called after having received the event MESH_EVENT_PARENT_CONNECTED.
 *
 * @param[out] bssid  pointer to parent BSSID
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 */
esp_err_t esp_mesh_get_parent_bssid(mesh_addr_t *bssid);

/**
 * @brief      Return whether the device is the root node of the network
 *
 * @return     true/false
 */
bool esp_mesh_is_root(void);

/**
 * @brief      Enable/disable self-organized networking
 *             - Self-organized networking has three main functions:
 *             select the root node;
 *             find a preferred parent;
 *             initiate reconnection if a disconnection is detected.
 *             - Self-organized networking is enabled by default.
 *             - If self-organized is disabled, users should set a parent for the device via esp_mesh_set_parent().
 *
 * @attention  This API is used to dynamically modify whether to enable the self organizing.
 *
 * @param[in]  enable  enable or disable self-organized networking
 * @param[in]  select_parent  Only valid when self-organized networking is enabled.
 *             - if select_parent is set to true, the root will give up its mesh root status and search for a new parent
 *             like other non-root devices.
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 */
esp_err_t esp_mesh_set_self_organized(bool enable, bool select_parent);

/**
 * @brief      Return whether enable self-organized networking or not
 *
 * @return     true/false
 */
bool esp_mesh_get_self_organized(void);

/**
 * @brief      Cause the root device to give up (waive) its mesh root status
 *             - A device is elected root primarily based on RSSI from the external router.
 *             - If external router conditions change, users can call this API to perform a root switch.
 *             - In this API, users could specify a desired root address to replace itself or specify an attempts value
 *             to ask current root to initiate a new round of voting. During the voting, a better root candidate would
 *             be expected to find to replace the current one.
 *             - If no desired root candidate, the vote will try a specified number of attempts (at least 15). If no better
 *             root candidate is found, keep the current one. If a better candidate is found, the new better one will
 *             send a root switch request to the current root, current root will respond with a root switch acknowledgment.
 *             - After that, the new candidate will connect to the router to be a new root, the previous root will disconnect
 *             with the router and choose another parent instead.
 *
 *             Root switch is completed with minimal disruption to the whole mesh network.
 *
 * @attention  This API is only called by the root.
 *
 * @param[in]  vote  vote configuration
 *             - If this parameter is set NULL, the vote will perform the default 15 times.
 *
 *             - Field percentage threshold is 0.9 by default.
 *             - Field is_rc_specified shall be false.
 *             - Field attempts shall be at least 15 times.
 * @param[in]  reason  only accept MESH_VOTE_REASON_ROOT_INITIATED for now
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_QUEUE_FULL
 *    - ESP_ERR_MESH_DISCARD
 *    - ESP_FAIL
 */
esp_err_t esp_mesh_waive_root(const mesh_vote_t *vote, int reason);

/**
 * @brief      Set vote percentage threshold for approval of being a root (default:0.9)
 *             - During the networking, only obtaining vote percentage reaches this threshold,
 *             the device could be a root.
 *
 * @attention  This API shall be called before mesh is started.
 *
 * @param[in]  percentage  vote percentage threshold
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 */
esp_err_t esp_mesh_set_vote_percentage(float percentage);

/**
 * @brief      Get vote percentage threshold for approval of being a root
 *
 * @return     percentage threshold
 */
float esp_mesh_get_vote_percentage(void);

/**
 * @brief      Set mesh softAP associate expired time (default:10 seconds)
 *             - If mesh softAP hasn't received any data from an associated child within this time,
 *             mesh softAP will take this child inactive and disassociate it.
 *             - If mesh softAP is encrypted, this value should be set a greater value, such as 30 seconds.
 *
 * @param[in]  seconds  the expired time
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 */
esp_err_t esp_mesh_set_ap_assoc_expire(int seconds);

/**
 * @brief      Get mesh softAP associate expired time
 *
 * @return     seconds
 */
int esp_mesh_get_ap_assoc_expire(void);

/**
 * @brief      Get total number of devices in current network (including the root)
 *
 * @attention  The returned value might be incorrect when the network is changing.
 **
 * @return     total number of devices (including the root)
 */
int esp_mesh_get_total_node_num(void);

/**
 * @brief      Get the number of devices in this device's sub-network (including self)
 *
 * @return     the number of devices over this device's sub-network (including self)
 */
int esp_mesh_get_routing_table_size(void);

/**
 * @brief      Get routing table of this device's sub-network (including itself)
 *
 * @param[out] mac  pointer to routing table
 * @param[in]  len  routing table size(in bytes)
 * @param[out] size  pointer to the number of devices in routing table (including itself)
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_ARGUMENT
 */
esp_err_t esp_mesh_get_routing_table(mesh_addr_t *mac, int len, int *size);

/**
 * @brief      Post the toDS state to the mesh stack
 *
 * @attention  This API is only for the root.
 *
 * @param[in]  reachable  this state represents whether the root is able to access external IP network
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 */
esp_err_t esp_mesh_post_toDS_state(bool reachable);

/**
 * @brief      Return the number of packets pending in the queue waiting to be sent by the mesh stack
 *
 * @param[out] pending  pointer to the TX pending
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 */
esp_err_t esp_mesh_get_tx_pending(mesh_tx_pending_t *pending);

/**
 * @brief      Return the number of packets available in the queue waiting to be received by applications
 *
 * @param[out] pending  pointer to the RX pending
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 */
esp_err_t esp_mesh_get_rx_pending(mesh_rx_pending_t *pending);

/**
 * @brief      Return the number of packets could be accepted from the specified address
 *
 * @param[in]  addr  self address or an associate children address
 * @param[out] xseqno_in  sequence number of the last received packet from the specified address
 *
 * @return    the number of upQ for a certain address
 */
int esp_mesh_available_txupQ_num(const mesh_addr_t *addr, uint32_t *xseqno_in);

/**
 * @brief      Set the number of RX queue for the node, the average number of window allocated to one of
 *             its child node is: wnd = xon_qsize / (2 * max_connection + 1).
 *             However, the window of each child node is not strictly equal to the average value,
 *             it is affected by the traffic also.
 *
 * @attention  This API shall be called before mesh is started.
 *
 * @param[in]  qsize  default:32 (min:16)
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 */
esp_err_t esp_mesh_set_xon_qsize(int qsize);

/**
 * @brief      Get queue size
 *
 * @return     the number of queue
 */
int esp_mesh_get_xon_qsize(void);

/**
 * @brief      Set whether allow more than one root existing in one network
 *             - The default value is true, that is, multiple roots are allowed.
 *
 * @param[in]  allowed  allow or not
 *
 * @return
 *    - ESP_OK
 *    - ESP_WIFI_ERR_NOT_INIT
 *    - ESP_WIFI_ERR_NOT_START
 */
esp_err_t esp_mesh_allow_root_conflicts(bool allowed);

/**
 * @brief      Check whether allow more than one root to exist in one network
 *
 * @return     true/false
 */
bool esp_mesh_is_root_conflicts_allowed(void);

/**
 * @brief      Set group ID addresses
 *
 * @param[in]  addr  pointer to new group ID addresses
 * @param[in]  num  the number of group ID addresses
 *
 * @return
 *    - ESP_OK
 *    - ESP_MESH_ERR_ARGUMENT
 */
esp_err_t esp_mesh_set_group_id(const mesh_addr_t *addr, int num);

/**
 * @brief      Delete group ID addresses
 *
 * @param[in]  addr  pointer to deleted group ID address
 * @param[in]  num  the number of group ID addresses
 *
 * @return
 *    - ESP_OK
 *    - ESP_MESH_ERR_ARGUMENT
 */
esp_err_t esp_mesh_delete_group_id(const mesh_addr_t *addr, int num);

/**
 * @brief      Get the number of group ID addresses
 *
 * @return     the number of group ID addresses
 */
int esp_mesh_get_group_num(void);

/**
 * @brief      Get group ID addresses
 *
 * @param[out] addr  pointer to group ID addresses
 * @param[in]  num  the number of group ID addresses
 *
 * @return
 *    - ESP_OK
 *    - ESP_MESH_ERR_ARGUMENT
 */
esp_err_t esp_mesh_get_group_list(mesh_addr_t *addr, int num);

/**
 * @brief      Check whether the specified group address is my group
 *
 * @return     true/false
 */
bool esp_mesh_is_my_group(const mesh_addr_t *addr);

/**
 * @brief      Set mesh network capacity (max:1000, default:300)
 *
 * @attention  This API shall be called before mesh is started.
 *
 * @param[in]  num  mesh network capacity
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_NOT_ALLOWED
 *    - ESP_MESH_ERR_ARGUMENT
 */
esp_err_t esp_mesh_set_capacity_num(int num);

/**
 * @brief      Get mesh network capacity
 *
 * @return     mesh network capacity
 */
int esp_mesh_get_capacity_num(void);

/**
 * @brief      Set mesh IE crypto functions
 *
 * @attention  This API can be called at any time after mesh is configured.
 *
 * @param[in]  crypto_funcs  crypto functions for mesh IE
 *           - If crypto_funcs is set to NULL, mesh IE is no longer encrypted.
 * @return
 *    - ESP_OK
 */
esp_err_t esp_mesh_set_ie_crypto_funcs(const mesh_crypto_funcs_t *crypto_funcs);

/**
 * @brief      Set mesh IE crypto key
 *
 * @attention  This API can be called at any time after mesh is configured.
 *
 * @param[in]  key  ASCII crypto key
 * @param[in]  len  length in bytes, range:8~64
 *
 * @return
 *    - ESP_OK
 *    - ESP_MESH_ERR_ARGUMENT
 */
esp_err_t esp_mesh_set_ie_crypto_key(const char *key, int len);

/**
 * @brief      Get mesh IE crypto key
 *
 * @param[out] key  ASCII crypto key
 * @param[in]  len  length in bytes, range:8~64
 *
 * @return
 *    - ESP_OK
 *    - ESP_MESH_ERR_ARGUMENT
 */
esp_err_t esp_mesh_get_ie_crypto_key(char *key, int len);

/**
 * @brief      Set delay time before starting root healing
 *
 * @param[in]  delay_ms  delay time in milliseconds
 *
 * @return
 *    - ESP_OK
 */
esp_err_t esp_mesh_set_root_healing_delay(int delay_ms);

/**
 * @brief      Get delay time before network starts root healing
 *
 * @return     delay time in milliseconds
 */
int esp_mesh_get_root_healing_delay(void);

/**
 * @brief      Enable network Fixed Root Setting
 *             - Enabling fixed root disables automatic election of the root node via voting.
 *             - All devices in the network shall use the same Fixed Root Setting (enabled or disabled).
 *             - If Fixed Root is enabled, users should make sure a root node is designated for the network.
 *
 * @param[in]  enable  enable or not
 *
 * @return
 *    - ESP_OK
 */
esp_err_t esp_mesh_fix_root(bool enable);

/**
 * @brief      Check whether network Fixed Root Setting is enabled
 *             - Enable/disable network Fixed Root Setting by API esp_mesh_fix_root().
 *             - Network Fixed Root Setting also changes with the "flag" value in parent networking IE.
 *
 * @return     true/false
 */
bool esp_mesh_is_root_fixed(void);

/**
 * @brief      Set a specified parent for the device
 *
 * @attention  This API can be called at any time after mesh is configured.
 *
 * @param[in]  parent  parent configuration, the SSID and the channel of the parent are mandatory.
 *             - If the BSSID is set, make sure that the SSID and BSSID represent the same parent,
 *             otherwise the device will never find this specified parent.
 * @param[in]  parent_mesh_id  parent mesh ID,
 *             - If this value is not set, the original mesh ID is used.
 * @param[in]  my_type  mesh type
 *             - MESH_STA is not supported.
 *             - If the parent set for the device is the same as the router in the network configuration,
 *            then my_type shall set MESH_ROOT and my_layer shall set MESH_ROOT_LAYER.
 * @param[in]  my_layer  mesh layer
 *             - my_layer of the device may change after joining the network.
 *             - If my_type is set MESH_NODE, my_layer shall be greater than MESH_ROOT_LAYER.
 *             - If my_type is set MESH_LEAF, the device becomes a standalone Wi-Fi station and no longer
 *             has the ability to extend the network.
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_ARGUMENT
 *    - ESP_ERR_MESH_NOT_CONFIG
 */
esp_err_t esp_mesh_set_parent(const wifi_config_t *parent, const mesh_addr_t *parent_mesh_id, mesh_type_t my_type, int my_layer);

/**
 * @brief      Get mesh networking IE length of one AP
 *
 * @param[out] len  mesh networking IE length
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_WIFI_NOT_INIT
 *    - ESP_ERR_INVALID_ARG
 *    - ESP_ERR_WIFI_FAIL
 */
esp_err_t esp_mesh_scan_get_ap_ie_len(int *len);

/**
 * @brief      Get AP record
 *
 * @attention  Different from esp_wifi_scan_get_ap_records(), this API only gets one of APs scanned each time.
 *             See "manual_networking" example.
 *
 * @param[out] ap_record  pointer to one AP record
 * @param[out] buffer  pointer to the mesh networking IE of this AP
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_WIFI_NOT_INIT
 *    - ESP_ERR_INVALID_ARG
 *    - ESP_ERR_WIFI_FAIL
 */
esp_err_t esp_mesh_scan_get_ap_record(wifi_ap_record_t *ap_record, void *buffer);

/**
 * @brief      Flush upstream packets pending in to_parent queue and to_parent_p2p queue
 *
 * @return
 *    - ESP_OK
 */
esp_err_t esp_mesh_flush_upstream_packets(void);

/**
 * @brief      Get the number of nodes in the subnet of a specific child
 *
 * @param[in]  child_mac  an associated child address of this device
 * @param[out] nodes_num  pointer to the number of nodes in the subnet of a specific child
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_NOT_START
 *    - ESP_ERR_MESH_ARGUMENT
 */
esp_err_t esp_mesh_get_subnet_nodes_num(const mesh_addr_t *child_mac, int *nodes_num);

/**
 * @brief      Get nodes in the subnet of a specific child
 *
 * @param[in]  child_mac  an associated child address of this device
 * @param[out] nodes  pointer to nodes in the subnet of a specific child
 * @param[in]  nodes_num  the number of nodes in the subnet of a specific child
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_MESH_NOT_START
 *    - ESP_ERR_MESH_ARGUMENT
 */
esp_err_t esp_mesh_get_subnet_nodes_list(const mesh_addr_t *child_mac, mesh_addr_t *nodes, int nodes_num);

/**
 * @brief      Disconnect from current parent
 *
 * @return
 *    - ESP_OK
 */
esp_err_t esp_mesh_disconnect(void);

/**
 * @brief      Connect to current parent
 *
 * @return
 *    - ESP_OK
 */
esp_err_t esp_mesh_connect(void);

/**
 * @brief      Flush scan result
 *
 * @return
 *    - ESP_OK
 */
esp_err_t esp_mesh_flush_scan_result(void);

/**
 * @brief      Cause the root device to add Channel Switch Announcement Element (CSA IE) to beacon
 *             - Set the new channel
 *             - Set how many beacons with CSA IE will be sent before changing a new channel
 *             - Enable the channel switch function
 *
 * @attention  This API is only called by the root.
 *
 * @param[in]  new_bssid  the new router BSSID if the router changes
 * @param[in]  csa_newchan  the new channel number to which the whole network is moving
 * @param[in]  csa_count  channel switch period(beacon count), unit is based on beacon interval of its softAP, the default value is 15.
 *
 * @return
 *    - ESP_OK
 */
esp_err_t esp_mesh_switch_channel(const uint8_t *new_bssid, int csa_newchan, int csa_count);

/**
 * @brief      Get the router BSSID
 *
 * @param[out] router_bssid  pointer to the router BSSID
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_WIFI_NOT_INIT
 *    - ESP_ERR_INVALID_ARG
 */
esp_err_t esp_mesh_get_router_bssid(uint8_t *router_bssid);

/**
 * @brief      Get the TSF time
 *
 * @return     the TSF time
 */
int64_t esp_mesh_get_tsf_time(void);

/**
 * @brief      Set mesh topology. The default value is MESH_TOPO_TREE
 *             - MESH_TOPO_CHAIN supports up to 1000 layers
 *
 * @attention  This API shall be called before mesh is started.
 *
 * @param[in]  topo  MESH_TOPO_TREE or MESH_TOPO_CHAIN
 *
 * @return
 *    - ESP_OK
 *    - ESP_MESH_ERR_ARGUMENT
 *    - ESP_ERR_MESH_NOT_ALLOWED
 */
esp_err_t esp_mesh_set_topology(esp_mesh_topology_t topo);

/**
 * @brief      Get mesh topology
 *
 * @return     MESH_TOPO_TREE or MESH_TOPO_CHAIN
 */
esp_mesh_topology_t esp_mesh_get_topology(void);

/**
 * @brief      Enable mesh Power Save function
 *
 * @attention  This API shall be called before mesh is started.
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_WIFI_NOT_INIT
 *    - ESP_ERR_MESH_NOT_ALLOWED
 */
esp_err_t esp_mesh_enable_ps(void);

/**
 * @brief      Disable mesh Power Save function
 *
 * @attention  This API shall be called before mesh is started.
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_WIFI_NOT_INIT
 *    - ESP_ERR_MESH_NOT_ALLOWED
 */
esp_err_t esp_mesh_disable_ps(void);

/**
 * @brief      Check whether the mesh Power Save function is enabled
 *
 * @return     true/false
 */
bool esp_mesh_is_ps_enabled(void);

/**
 * @brief      Check whether the device is in active state
 *             - If the device is not in active state, it will neither transmit nor receive frames.
 *
 * @return     true/false
 */
bool esp_mesh_is_device_active(void);

/**
 * @brief      Set the device duty cycle and type
 *             - The range of dev_duty values is 1 to 100. The default value is 10.
 *             - dev_duty = 100, the PS will be stopped.
 *             - dev_duty is better to not less than 5.
 *             - dev_duty_type could be MESH_PS_DEVICE_DUTY_REQUEST or MESH_PS_DEVICE_DUTY_DEMAND.
 *             - If dev_duty_type is set to MESH_PS_DEVICE_DUTY_REQUEST, the device will use a nwk_duty provided by the network.
 *             - If dev_duty_type is set to MESH_PS_DEVICE_DUTY_DEMAND, the device will use the specified dev_duty.
 *
 * @attention  This API can be called at any time after mesh is started.
 *
 * @param[in]  dev_duty  device duty cycle
 * @param[in]  dev_duty_type  device PS duty cycle type, not accept MESH_PS_NETWORK_DUTY_MASTER
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 */
esp_err_t esp_mesh_set_active_duty_cycle(int dev_duty, int dev_duty_type);

/**
 * @brief      Get device duty cycle and type
 *
 * @param[out] dev_duty  device duty cycle
 * @param[out] dev_duty_type  device PS duty cycle type
 *
 * @return
 *    - ESP_OK
 */
esp_err_t esp_mesh_get_active_duty_cycle(int* dev_duty, int* dev_duty_type);

/**
 * @brief      Set the network duty cycle, duration and rule
 *             - The range of nwk_duty values is 1 to 100. The default value is 10.
 *             - nwk_duty is the network duty cycle the entire network or the up-link path will use. A device that successfully
 *             sets the nwk_duty is known as a NWK-DUTY-MASTER.
 *             - duration_mins specifies how long the specified nwk_duty will be used. Once duration_mins expires, the root will take
 *             over as the NWK-DUTY-MASTER. If an existing NWK-DUTY-MASTER leaves the network, the root will take over as the
 *             NWK-DUTY-MASTER again.
 *             - duration_mins = (-1) represents nwk_duty will be used until a new NWK-DUTY-MASTER with a different nwk_duty appears.
 *             - Only the root can set duration_mins to (-1).
 *             - If applied_rule is set to MESH_PS_NETWORK_DUTY_APPLIED_ENTIRE, the nwk_duty will be used by the entire network.
 *             - If applied_rule is set to MESH_PS_NETWORK_DUTY_APPLIED_UPLINK, the nwk_duty will only be used by the up-link path nodes.
 *             - The root does not accept MESH_PS_NETWORK_DUTY_APPLIED_UPLINK.
 *             - A nwk_duty with duration_mins(-1) set by the root is the default network duty cycle used by the entire network.
 *
 * @attention  This API can be called at any time after mesh is started.
 *             - In self-organized network, if this API is called before mesh is started in all devices, (1)nwk_duty shall be set to the
 *             same value for all devices; (2)duration_mins shall be set to (-1); (3)applied_rule shall be set to
 *             MESH_PS_NETWORK_DUTY_APPLIED_ENTIRE; after the voted root appears, the root will become the NWK-DUTY-MASTER and broadcast
 *             the nwk_duty and its identity of NWK-DUTY-MASTER.
 *             - If the root is specified (FIXED-ROOT), call this API in the root to provide a default nwk_duty for the entire network.
 *             - After joins the network, any device can call this API to change the nwk_duty, duration_mins or applied_rule.
 *
 * @param[in]  nwk_duty  network duty cycle
 * @param[in]  duration_mins  duration (unit: minutes)
 * @param[in]  applied_rule  only support MESH_PS_NETWORK_DUTY_APPLIED_ENTIRE
 *
 * @return
 *    - ESP_OK
 *    - ESP_FAIL
 */
esp_err_t esp_mesh_set_network_duty_cycle(int nwk_duty, int duration_mins, int applied_rule);

/**
 * @brief      Get the network duty cycle, duration, type and rule
 *
 * @param[out] nwk_duty  current network duty cycle
 * @param[out] duration_mins  the duration of current nwk_duty
 * @param[out] dev_duty_type  if it includes MESH_PS_DEVICE_DUTY_MASTER, this device is the current NWK-DUTY-MASTER.
 * @param[out] applied_rule  MESH_PS_NETWORK_DUTY_APPLIED_ENTIRE
 *
 * @return
 *    - ESP_OK
 */
esp_err_t esp_mesh_get_network_duty_cycle(int* nwk_duty, int* duration_mins, int* dev_duty_type, int* applied_rule);

/**
 * @brief      Get the running active duty cycle
 *          - The running active duty cycle of the root is 100.
 *          - If duty type is set to MESH_PS_DEVICE_DUTY_REQUEST, the running active duty cycle is nwk_duty provided by the network.
 *          - If duty type is set to MESH_PS_DEVICE_DUTY_DEMAND, the running active duty cycle is dev_duty specified by the users.
 *          - In a mesh network, devices are typically working with a certain duty-cycle (transmitting, receiving and sleep) to
 *            reduce the power consumption. The running active duty cycle decides the amount of awake time within a beacon interval.
 *            At each start of beacon interval, all devices wake up, broadcast beacons, and transmit packets if they do have pending
 *            packets for their parents or for their children. Note that Low-duty-cycle means devices may not be active in most of
 *            the time, the latency of data transmission might be greater.
 *
 * @return     the running active duty cycle
 */
int esp_mesh_get_running_active_duty_cycle(void);

/**
 * @brief      Duty signaling
 *
 * @param[in]  fwd_times  the times of forwarding duty signaling packets
 *
 * @return
 *    - ESP_OK
 */
esp_err_t esp_mesh_ps_duty_signaling(int fwd_times);
#ifdef __cplusplus
}
#endif
#endif /* __ESP_MESH_H__ */
