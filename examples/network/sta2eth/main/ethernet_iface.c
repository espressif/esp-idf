/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <string.h>
#include "cc.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "wired_iface.h"
#include "dhcpserver/dhcpserver_options.h"
#include "esp_mac.h"
#include "ethernet_init.h"
#include "esp_eth_netif_glue.h"

/**
 *  Disable promiscuous mode on Ethernet interface by setting this macro to 0
 *  if disabled, we'd have to rewrite MAC addressed in frames with the actual Eth interface MAC address
 *  - this results in better throughput
 *  - might cause ARP conflicts if the PC is also connected to the same AP with another NIC
 */
#define ETH_BRIDGE_PROMISCUOUS  0

/**
 * Set this to 1 to runtime update HW addresses in DHCP messages
 * (this is needed if the client uses 61 option and the DHCP server applies strict rules on assigning addresses)
 */
#define MODIFY_DHCP_MSGS        0

static const char *TAG = "example_wired_ethernet";
static esp_eth_handle_t s_eth_handle = NULL;
static bool s_ethernet_is_connected = false;
static uint8_t s_eth_mac[6];
static wired_rx_cb_t s_rx_cb = NULL;
static wired_free_cb_t s_free_cb = NULL;

/**
 * @brief Event handler for Ethernet events
 */
void eth_event_handler(void *arg, esp_event_base_t event_base,
                       int32_t event_id, void *event_data)
{
    uint8_t mac_addr[6] = {0};
    /* we can get the ethernet driver handle from event data */
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;
    esp_netif_t *netif = (esp_netif_t*)arg;

    switch (event_id) {
    case ETHERNET_EVENT_CONNECTED:
        ESP_LOGI(TAG, "Ethernet Link Up");
        esp_netif_dhcps_start(netif);
        esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr);
        ESP_LOGI(TAG, "Ethernet HW Addr %02x:%02x:%02x:%02x:%02x:%02x",
                 mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
        s_ethernet_is_connected = true;
        break;
    case ETHERNET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "Ethernet Link Down");
        esp_netif_dhcps_stop(netif);
        s_ethernet_is_connected = false;
        break;
    case ETHERNET_EVENT_START:
        ESP_LOGI(TAG, "Ethernet Started");
        break;
    case ETHERNET_EVENT_STOP:
        ESP_LOGI(TAG, "Ethernet Stopped");
        break;
    default:
        ESP_LOGI(TAG, "Default Event");
        break;
    }
}

/**
 *  In this scenario of WiFi station to Ethernet bridge mode, we have this configuration
 *
 *   (ISP) router        ESP32               PC
 *      [ AP ] <->   [ sta -- eth ] <->  [ eth-NIC ]
 *
 *  From the PC's NIC perspective the L2 forwarding should be transparent and resemble this configuration:
 *
 *   (ISP) router                           PC
 *      [ AP ]       <---------->       [ virtual wifi-NIC ]
 *
 *  In order for the ESP32 to act as L2 bridge it needs to accept all frames on the interface
 *  - For Ethernet we just enable `PROMISCUOUS` mode
 *  - For Wifi we could also enable the promiscuous mode, but in that case we'd receive encoded frames
 *    from 802.11 and we'd have to decode it and process (using wpa-supplicant).
 *    The easier option (in this scenario of only one client -- eth-NIC) we could simply "pretend"
 *    that we have the HW mac address of eth-NIC and receive only ethernet frames for "us" from esp_wifi API
 *  (we could use the same technique for Ethernet and yield better throughput, see ETH_BRIDGE_PROMISCUOUS flag)
 *
 *  This API updates Ethernet frames to swap mac addresses of ESP32 interfaces with those of eth-NIC and AP.
 *  For that we'd have to parse initial DHCP packets (manually) to record the HW addresses of the AP and eth-NIC
 *  (note, that it is possible to simply spoof the MAC addresses, but that's not recommended technique)
 */
#define IP_V4 0x40
#define IP_PROTO_UDP 0x11
#define DHCP_PORT_IN 0x43
#define DHCP_PORT_OUT 0x44
#define DHCP_MACIG_COOKIE_OFFSET (8 + 236)
#define DHCP_HW_ADDRESS_OFFSET (36)
#define MIN_DHCP_PACKET_SIZE (285)
#define IP_HEADER_SIZE (20)
#define DHCP_DISCOVER 1
#define DHCP_OFFER 2
#define DHCP_COOKIE_WITH_PKT_TYPE(type) {0x63, 0x82, 0x53, 0x63, 0x35, 1, type};

#if MODIFY_DHCP_MSGS
static void update_udp_checksum(uint16_t *udp_header, uint16_t* ip_header)
{
    uint32_t sum = 0;
    uint16_t *ptr = udp_header;
    ptr[3] = 0; // clear the current checksum
    int payload_len = htons(ip_header[1]) - IP_HEADER_SIZE;
    // add UDP payload
    for (int i = 0; i < payload_len/2; i++) {
        sum += htons(*ptr++);
    }
    // add the padding if the packet length is odd
    if (payload_len & 1) {
        sum += (*((uint8_t *)ptr) << 8);
    }
    // add some IP header data
    ptr = ip_header + 6;
    for (int i = 0; i < 4; i++) {       // IP addresses
        sum += htons(*ptr++);
    }
    sum += IP_PROTO_UDP + payload_len;  // protocol + size
    do {
        sum = (sum & 0xFFFF) + (sum >> 16);
    } while (sum & 0xFFFF0000);         //  process the carry
    ptr = udp_header;
    ptr[3] = htons(~sum);   // update the UDP header with the new checksum
}
#endif // MODIFY_DHCP_MSGS

void mac_spoof(mac_spoof_direction_t direction, uint8_t *buffer, uint16_t len, uint8_t own_mac[6])
{
    if (!s_ethernet_is_connected) {
        return;
    }
    static uint8_t eth_nic_mac[6] = {};
    static bool eth_nic_mac_found = false;
#if !ETH_BRIDGE_PROMISCUOUS
    static uint8_t ap_mac[6] = {};
    static bool ap_mac_found = false;
#endif
    uint8_t *dest_mac = buffer;
    uint8_t *src_mac = buffer + 6;
    uint8_t *eth_type = buffer + 12;
    if (eth_type[0] == 0x08) {      // support only IPv4
        // try to find NIC HW address (look for DHCP discovery packet)
        if ( (!eth_nic_mac_found || (MODIFY_DHCP_MSGS)) && direction == FROM_WIRED && eth_type[1] == 0x00) {  // ETH IP4
            uint8_t *ip_header = eth_type + 2;
            if (len > MIN_DHCP_PACKET_SIZE && (ip_header[0] & 0xF0) == IP_V4 && ip_header[9] == IP_PROTO_UDP) {
                uint8_t *udp_header = ip_header + IP_HEADER_SIZE;
                const uint8_t dhcp_ports[] = {0, DHCP_PORT_OUT, 0, DHCP_PORT_IN};
                if (memcmp(udp_header, dhcp_ports, sizeof(dhcp_ports)) == 0) {
                    uint8_t *dhcp_magic = udp_header + DHCP_MACIG_COOKIE_OFFSET;
                    const uint8_t dhcp_type[] = DHCP_COOKIE_WITH_PKT_TYPE(DHCP_DISCOVER);
                    if (!eth_nic_mac_found && memcmp(dhcp_magic, dhcp_type, sizeof(dhcp_type)) == 0) {
                        eth_nic_mac_found = true;
                        memcpy(eth_nic_mac, src_mac, 6);
                    }
#if MODIFY_DHCP_MSGS
                    if (eth_nic_mac_found) {
                        bool update_checksum = false;
                        // Replace the BOOTP HW address
                        uint8_t *dhcp_client_hw_addr = udp_header + DHCP_HW_ADDRESS_OFFSET;
                        if (memcmp(dhcp_client_hw_addr, eth_nic_mac, 6) == 0) {
                            memcpy(dhcp_client_hw_addr, own_mac, 6);
                            update_checksum = true;
                        }
                        // Replace the HW address in opt-61
                        uint8_t *dhcp_opts = dhcp_magic + 4;
                        while (*dhcp_opts != 0xFF) {
                            if (dhcp_opts[0] == 61 && dhcp_opts[1] == 7 /* size (type=1 + mac=6) */ && dhcp_opts[2] == 1 /* HW address type*/ &&
                                memcmp(dhcp_opts + 3, eth_nic_mac, 6) == 0) {
                                update_checksum = true;
                                memcpy(dhcp_opts + 3, own_mac, 6);
                                break;
                            }
                            dhcp_opts += dhcp_opts[1]+ 2;
                            if (dhcp_opts - buffer >= len) {
                                break;
                            }
                        }
                        if (update_checksum) {
                            update_udp_checksum((uint16_t *) udp_header, (uint16_t *) ip_header);
                        }
                    }
#endif // MODIFY_DHCP_MSGS
                }   // DHCP
            } // UDP/IP
#if !ETH_BRIDGE_PROMISCUOUS || MODIFY_DHCP_MSGS
            // try to find AP HW address (look for DHCP offer packet)
        } else if ( (!ap_mac_found || (MODIFY_DHCP_MSGS)) && direction == TO_WIRED && eth_type[1] == 0x00) {  // ETH IP4
            uint8_t *ip_header = eth_type + 2;
            if (len > MIN_DHCP_PACKET_SIZE && (ip_header[0] & 0xF0) == IP_V4 && ip_header[9] == IP_PROTO_UDP) {
                uint8_t *udp_header = ip_header + IP_HEADER_SIZE;
                const uint8_t dhcp_ports[] = {0, DHCP_PORT_IN, 0, DHCP_PORT_OUT};
                if (memcmp(udp_header, dhcp_ports, sizeof(dhcp_ports)) == 0) {
                    uint8_t *dhcp_magic = udp_header + DHCP_MACIG_COOKIE_OFFSET;
#if MODIFY_DHCP_MSGS
                    if (eth_nic_mac_found) {
                        uint8_t *dhcp_client_hw_addr = udp_header + DHCP_HW_ADDRESS_OFFSET;
                        // Replace BOOTP HW address
                        if (memcmp(dhcp_client_hw_addr, own_mac, 6) == 0) {
                            memcpy(dhcp_client_hw_addr, eth_nic_mac, 6);
                            update_udp_checksum((uint16_t*)udp_header, (uint16_t*)ip_header);
                        }
                    }
#endif // MODIFY_DHCP_MSGS
                    const uint8_t dhcp_type[] = DHCP_COOKIE_WITH_PKT_TYPE(DHCP_OFFER);
                    if (!ap_mac_found && memcmp(dhcp_magic, dhcp_type, sizeof(dhcp_type)) == 0) {
                        ap_mac_found = true;
                        memcpy(ap_mac, src_mac, 6);
                    }
                }   // DHCP
            } // UDP/IP
#endif // !ETH_BRIDGE_PROMISCUOUS || MODIFY_DHCP_MSGS
        }

        // swap addresses in ARP probes
        if (eth_type[1] == 0x06) { // ARP
            uint8_t *arp = eth_type + 2 + 8; // points to sender's HW address
            if (eth_nic_mac_found && direction == FROM_WIRED && memcmp(arp, eth_nic_mac, 6) == 0) {
                /* updates senders HW address to our wireless */
                memcpy(arp, own_mac, 6);
#if !ETH_BRIDGE_PROMISCUOUS
            } else if (ap_mac_found && direction == TO_WIRED && memcmp(arp, ap_mac, 6) == 0) {
                /* updates senders HW address to our wired */
                memcpy(arp, s_eth_mac, 6);
#endif // !ETH_BRIDGE_PROMISCUOUS
            }
        }
        // swap HW addresses in ETH frames
#if !ETH_BRIDGE_PROMISCUOUS
        if (ap_mac_found && direction == FROM_WIRED && memcmp(dest_mac, s_eth_mac, 6) == 0) {
            memcpy(dest_mac, ap_mac, 6);
        }
        if (ap_mac_found && direction == TO_WIRED && memcmp(src_mac, ap_mac, 6) == 0) {
            memcpy(src_mac, s_eth_mac, 6);
        }
#endif // !ETH_BRIDGE_PROMISCUOUS
        if (eth_nic_mac_found && direction == FROM_WIRED && memcmp(src_mac, eth_nic_mac, 6) == 0) {
            memcpy(src_mac, own_mac, 6);
        }
        if (eth_nic_mac_found && direction == TO_WIRED && memcmp(dest_mac, own_mac, 6) == 0) {
            memcpy(dest_mac, eth_nic_mac, 6);
        }
    }   // IP4 section of eth-type (0x08) both ETH-IP4 and ETHARP
}

static esp_err_t wired_recv(esp_eth_handle_t eth_handle, uint8_t *buffer, uint32_t len, void *priv)
{
    esp_err_t ret = s_rx_cb(buffer, len, buffer);
    free(buffer);
    return ret;
}

esp_err_t wired_bridge_init(wired_rx_cb_t rx_cb, wired_free_cb_t free_cb)
{
    uint8_t eth_port_cnt = 0;
    esp_eth_handle_t *eth_handles;
    ESP_ERROR_CHECK(example_eth_init(&eth_handles, &eth_port_cnt));

    // Check for multiple Ethernet interfaces
    if (1 < eth_port_cnt) {
        ESP_LOGW(TAG, "Multiple Ethernet Interface detected: Only the first initialized interface is going to be used.");
    }
    s_eth_handle = eth_handles[0];
    free(eth_handles);
    ESP_ERROR_CHECK(esp_eth_update_input_path(s_eth_handle, wired_recv, NULL));
#if ETH_BRIDGE_PROMISCUOUS
    bool eth_promiscuous = true;
    ESP_ERROR_CHECK(esp_eth_ioctl(s_eth_handle, ETH_CMD_S_PROMISCUOUS, &eth_promiscuous));
#endif
    ESP_ERROR_CHECK(esp_eth_ioctl(s_eth_handle, ETH_CMD_G_MAC_ADDR, &s_eth_mac));
    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, eth_event_handler, NULL));
    ESP_ERROR_CHECK(esp_eth_start(s_eth_handle));
    s_rx_cb = rx_cb;
    s_free_cb = free_cb;
    return ESP_OK;
}

esp_err_t wired_send(void *buffer, uint16_t len, void *buff_free_arg)
{
    if (s_ethernet_is_connected) {
        if (esp_eth_transmit(s_eth_handle, buffer, len) != ESP_OK) {
            ESP_LOGE(TAG, "Ethernet send packet failed");
            return ESP_FAIL;
        }
        if (s_free_cb) {
            s_free_cb(buff_free_arg, NULL);
        }
        return ESP_OK;
    }
    return ESP_ERR_INVALID_STATE;
}

/**
 *  In this scenario of configuring WiFi, we setup Ethernet to create a network and run DHCP server,
 *  so it could assign an IP address to the PC
 *
 *           ESP32               PC
 *      | <DHCP server>   eth | <->  [ eth-NIC ]
 *      | <HTTP server>       |
 *      | (wifi-provisioning) |
 *
 *  From the PC's NIC perspective the board acts as a separate network with it's own IP and MAC address
 *  (this network's MAC address is the native ESP32's Ethernet interface MAC)
 */

esp_err_t wired_netif_init(void)
{
    uint8_t eth_port_cnt = 0;
    esp_eth_handle_t *eth_handles;
    ESP_ERROR_CHECK(example_eth_init(&eth_handles, &eth_port_cnt));

    // Check or multiple ethernet interface
    if (1 < eth_port_cnt) {
        ESP_LOGW(TAG, "Multiple Ethernet Interface detected: Only the first initialized interface is going to be used.");
    }
    s_eth_handle = eth_handles[0];
    free(eth_handles);

    // 1) Derive the base config (very similar to IDF's default WiFi AP with DHCP server)
    esp_netif_inherent_config_t base_cfg =  {
            .flags = ESP_NETIF_DHCP_SERVER,                // Run DHCP server
            .ip_info = &_g_esp_netif_soft_ap_ip,           // Use the same IP ranges as IDF's soft AP
            .if_key = "wired",                             // Set mame, key, priority
            .if_desc = "ethernet config device",
            .route_prio = 10
    };

    // Config the esp-netif with:
    //   1) inherent config (behavioural settings of an interface)
    //   2) driver's config -- no need, will use the default ethernet-netif glue and attach it to this netif
    //   3) stack config -- will use the default ethernet TCP/IP settings
    esp_netif_config_t cfg = {
        .base = &base_cfg,
        .stack = ESP_NETIF_NETSTACK_DEFAULT_ETH
    };

    esp_netif_t *netif = esp_netif_new(&cfg);
    if (netif == NULL) {
        return ESP_FAIL;
    }

    // Now we attach the constructed network interface to IDF's default ethernet glue
    esp_eth_netif_glue_handle_t eth_glue = esp_eth_new_netif_glue(s_eth_handle);
    ESP_ERROR_CHECK(esp_netif_attach(netif, eth_glue));

    uint8_t mac[6];
    ESP_ERROR_CHECK(esp_eth_ioctl(s_eth_handle, ETH_CMD_G_MAC_ADDR, &mac));
    esp_netif_set_mac(netif, mac);

    // set the minimum lease time
    uint32_t  lease_opt = 1;
    esp_netif_dhcps_option(netif, ESP_NETIF_OP_SET, IP_ADDRESS_LEASE_TIME, &lease_opt, sizeof(lease_opt));
    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, eth_event_handler, netif));
    ESP_ERROR_CHECK(esp_eth_start(s_eth_handle));
    return ESP_OK;
}
