/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#define ETH_ADDR_LEN        (6)  /* MAC Address length */
#define ETH_HEADER_LEN      (14) /* Ethernet frame header size: Dest addr(6 Bytes) + Src addr(6 Bytes) + length/type(2 Bytes) */
#define ETH_VLAN_TAG_LEN    (4)  /* Optional 802.1q VLAN Tag length */
#define ETH_CRC_LEN         (4)  /* Ethernet frame CRC length */

#define ETH_MAX_PAYLOAD_LEN (1500) /* Maximum Ethernet payload size */
#define ETH_MIN_PAYLOAD_LEN (46)   /* Minimum Ethernet payload size */
#define ETH_JUMBO_FRAME_PAYLOAD_LEN (9000) /* Jumbo frame payload size */
#define ETH_MAX_PACKET_SIZE (ETH_HEADER_LEN + ETH_VLAN_TAG_LEN + ETH_MAX_PAYLOAD_LEN + ETH_CRC_LEN) /* Maximum frame size (1522 Bytes) */
#define ETH_MIN_PACKET_SIZE (ETH_HEADER_LEN + ETH_MIN_PAYLOAD_LEN + ETH_CRC_LEN) /* Minimum frame size (64 Bytes) */

#define ETH_IEEE802_3_MAX_LEN 0x05DC  /* Maximum length of IEEE802.3 frame stored in Length/Ethtype field */

/* EtherTypes */
#define ETH_T_8021Q	          0x8100  /* 802.1Q VLAN tag */
#define ETH_T_8021AD          0x88A8  /* 802.1ad Service VLAN (double vlan tag) */
