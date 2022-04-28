#ifndef ESP_MDNS_NETWORKING_H_
#define ESP_MDNS_NETWORKING_H_

/*
 * MDNS Server Networking -- private include
 *
 */
#include "mdns.h"
#include "mdns_private.h"


/**
 * @brief  Queue RX packet action
 */
esp_err_t _mdns_send_rx_action(mdns_rx_packet_t * packet);

/**
 * @brief  Start PCB
 */
esp_err_t _mdns_pcb_init(mdns_if_t tcpip_if, mdns_ip_protocol_t ip_protocol);

/**
 * @brief  Stop PCB
 */
esp_err_t _mdns_pcb_deinit(mdns_if_t tcpip_if, mdns_ip_protocol_t ip_protocol);

/**
 * @brief  send packet over UDP
 *
 * @param  server       The server
 * @param  data         byte array containing the packet data
 * @param  len          length of the packet data
 *
 * @return length of sent packet or 0 on error
 */
size_t _mdns_udp_pcb_write(mdns_if_t tcpip_if, mdns_ip_protocol_t ip_protocol, const esp_ip_addr_t *ip, uint16_t port, uint8_t * data, size_t len);

/**
 * @brief  Gets data pointer to the mDNS packet
 */
void* _mdns_get_packet_data(mdns_rx_packet_t *packet);

/**
 * @brief  Gets data length of c
 */
size_t _mdns_get_packet_len(mdns_rx_packet_t *packet);

/**
 * @brief  Free the  mDNS packet
 */
void _mdns_packet_free(mdns_rx_packet_t *packet);

#endif /* ESP_MDNS_NETWORKING_H_ */
