/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * This file defines basic PHY registers in compliance to IEEE 802.3, 22.2.4 Management functions section.
 *
 */

/**
 * @brief BMCR(Basic Mode Control Register)
 *
 */
typedef union {
    struct {
        uint32_t reserved : 7;          /*!< Reserved */
        uint32_t collision_test : 1;    /*!< Collision test */
        uint32_t duplex_mode : 1;       /*!< Duplex mode:Full Duplex(1) and Half Duplex(0) */
        uint32_t restart_auto_nego : 1; /*!< Restart auto-negotiation */
        uint32_t isolate : 1;           /*!< Isolate the PHY from MII except the SMI interface */
        uint32_t power_down : 1;        /*!< Power off PHY except SMI interface */
        uint32_t en_auto_nego : 1;      /*!< Enable auto negotiation */
        uint32_t speed_select : 1;      /*!< Select speed: 100Mbps(1) and 10Mbps(0) */
        uint32_t en_loopback : 1;       /*!< Enables transmit data to be routed to the receive path */
        uint32_t reset : 1;             /*!< Reset PHY registers. This bit is self-clearing. */
    };
    uint32_t val;
} bmcr_reg_t;
#define ETH_PHY_BMCR_REG_ADDR (0x00)

/**
 * @brief BMSR(Basic Mode Status Register)
 *
 */
typedef union {
    struct {
        uint32_t ext_capability : 1;       /*!< Extended register capability */
        uint32_t jabber_detect : 1;        /*!< Jabber condition detected */
        uint32_t link_status : 1;          /*!< Link status */
        uint32_t auto_nego_ability : 1;    /*!< Auto negotiation ability */
        uint32_t remote_fault : 1;         /*!< Remote fault detected */
        uint32_t auto_nego_complete : 1;   /*!< Auto negotiation completed */
        uint32_t mf_preamble_suppress : 1; /*!< Preamble suppression capability for management frame */
        uint32_t reserved : 1;             /*!< Reserved */
        uint32_t ext_status : 1;           /*!< Extended Status */
        uint32_t base100_t2_hdx : 1;       /*!< 100Base-T2 Half Duplex capability */
        uint32_t base100_t2_fdx : 1;       /*!< 100Base-T2 Full Duplex capability */
        uint32_t base10_t_hdx : 1;         /*!< 10Base-T Half Duplex capability */
        uint32_t base10_t_fdx : 1;         /*!< 10Base-T Full Duplex capability */
        uint32_t base100_tx_hdx : 1;       /*!< 100Base-Tx Half Duplex capability */
        uint32_t base100_tx_fdx : 1;       /*!< 100Base-Tx Full Duplex capability */
        uint32_t based100_t4 : 1;          /*!< 100Base-T4 capability */
    };
    uint32_t val;
} bmsr_reg_t;
#define ETH_PHY_BMSR_REG_ADDR (0x01)

/**
 * @brief PHYIDR1(PHY Identifier Register 1)
 *
 */
typedef union {
    struct {
        uint32_t oui_msb : 16; /*!< Organizationally Unique Identifier(OUI) most significant bits */
    };
    uint32_t val;
} phyidr1_reg_t;
#define ETH_PHY_IDR1_REG_ADDR (0x02)

/**
 * @brief PHYIDR2(PHY Identifier Register 2)
 *
 */
typedef union {
    struct {
        uint32_t model_revision : 4; /*!< Model revision number */
        uint32_t vendor_model : 6;   /*!< Vendor model number */
        uint32_t oui_lsb : 6;        /*!< Organizationally Unique Identifier(OUI) least significant bits */
    };
    uint32_t val;
} phyidr2_reg_t;
#define ETH_PHY_IDR2_REG_ADDR (0x03)

/**
 * @brief ANAR(Auto-Negotiation Advertisement Register)
 *
 */
typedef union {
    struct {
        uint32_t protocol_select : 5;  /*!< Binary encoded selector supported by this PHY */
        uint32_t base10_t : 1;         /*!< 10Base-T support */
        uint32_t base10_t_fd : 1;      /*!< 10Base-T full duplex support */
        uint32_t base100_tx : 1;       /*!< 100Base-TX support */
        uint32_t base100_tx_fd : 1;    /*!< 100Base-TX full duplex support */
        uint32_t base100_t4 : 1;       /*!< 100Base-T4 support */
        uint32_t symmetric_pause : 1;  /*!< Symmetric pause support for full duplex links */
        uint32_t asymmetric_pause : 1; /*!< Asymmetric pause support for full duplex links */
        uint32_t reserved1 : 1;        /*!< Reserved */
        uint32_t remote_fault : 1;     /*!< Advertise remote fault detection capability */
        uint32_t acknowledge : 1;      /*!< Link partner ability data reception acknowledged */
        uint32_t next_page : 1;        /*!< Next page indication, if set, next page transfer is desired */
    };
    uint32_t val;
} anar_reg_t;
#define ETH_PHY_ANAR_REG_ADDR (0x04)

/**
 * @brief ANLPAR(Auto-Negotiation Link Partner Ability Register)
 *
 */
typedef union {
    struct {
        uint32_t protocol_select : 5;  /*!< Link Partner’s binary encoded node selector */
        uint32_t base10_t : 1;         /*!< 10Base-T support */
        uint32_t base10_t_fd : 1;      /*!< 10Base-T full duplex support */
        uint32_t base100_tx : 1;       /*!< 100Base-TX support */
        uint32_t base100_tx_fd : 1;    /*!< 100Base-TX full duplex support */
        uint32_t base100_t4 : 1;       /*!< 100Base-T4 support */
        uint32_t symmetric_pause : 1;  /*!< Symmetric pause supported by Link Partner */
        uint32_t asymmetric_pause : 1; /*!< Asymmetric pause supported by Link Partner */
        uint32_t reserved : 1;         /*!< Reserved */
        uint32_t remote_fault : 1;     /*!< Link partner is indicating a remote fault */
        uint32_t acknowledge : 1;      /*!< Acknowledges from link partner */
        uint32_t next_page : 1;        /*!< Next page indication */
    };
    uint32_t val;
} anlpar_reg_t;
#define ETH_PHY_ANLPAR_REG_ADDR (0x05)

/**
 * @brief ANER(Auto-Negotiate Expansion Register)
 *
 */
typedef union {
    struct {
        uint32_t link_partner_auto_nego_able : 1; /*!< Link partner auto-negotiation ability */
        uint32_t link_page_received : 1;          /*!< Link code word page has received */
        uint32_t next_page_able : 1;              /*!< Next page ablility */
        uint32_t link_partner_next_page_able : 1; /*!< Link partner next page ability */
        uint32_t parallel_detection_fault : 1;    /*!< Parallel detection fault */
        uint32_t reserved : 11;                   /*!< Reserved */
    };
    uint32_t val;
} aner_reg_t;
#define ETH_PHY_ANER_REG_ADDR (0x06)

#ifdef __cplusplus
}
#endif
