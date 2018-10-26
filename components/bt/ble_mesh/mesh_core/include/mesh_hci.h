/*
 * Copyright (c) 2015-2016 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __MESH_HCI_H
#define __MESH_HCI_H

#include "mesh_kernel.h"
#include "mesh_bearer_adapt.h"
#include "mesh_atomic.h"


#ifdef __cplusplus
extern "C" {
#endif


/* Porting form zephyr/subsys/bluetooth/host/hci_core.h */

#if defined(CONFIG_BT_BREDR)
#define LMP_FEAT_PAGES_COUNT    3
#else
#define LMP_FEAT_PAGES_COUNT    1
#endif

/* bt_dev flags: the flags defined here represent BT controller state */
enum {
    BT_DEV_ENABLE,
    BT_DEV_READY,
    BT_DEV_ID_STATIC_RANDOM,
    BT_DEV_HAS_PUB_KEY,
    BT_DEV_PUB_KEY_BUSY,

    BT_DEV_ADVERTISING,
    BT_DEV_KEEP_ADVERTISING,
    BT_DEV_SCANNING,
    BT_DEV_EXPLICIT_SCAN,
    BT_DEV_ACTIVE_SCAN,
    BT_DEV_SCAN_FILTER_DUP,

    BT_DEV_RPA_VALID,

    BT_DEV_ID_PENDING,

#if defined(CONFIG_BT_BREDR)
    BT_DEV_ISCAN,
    BT_DEV_PSCAN,
    BT_DEV_INQUIRY,
#endif /* CONFIG_BT_BREDR */

    /* Total number of flags - must be at the end of the enum */
    BT_DEV_NUM_FLAGS,
};

struct bt_dev_le {
    /* LE features */
    u8_t            features[8];
    /* LE states */
    u64_t           states;

#if defined(CONFIG_BT_CONN)
    /* Controller buffer information */
    u16_t           mtu;
    struct k_sem        pkts;
#endif /* CONFIG_BT_CONN */

#if defined(CONFIG_BT_SMP)
    /* Size of the the controller resolving list */
    u8_t                    rl_size;
    /* Number of entries in the resolving list. rl_entries > rl_size
     * means that host-side resolving is used.
     */
    u8_t                    rl_entries;
#endif /* CONFIG_BT_SMP */
};

#if defined(CONFIG_BT_BREDR)
struct bt_dev_br {
    /* Max controller's acceptable ACL packet length */
    u16_t         mtu;
    struct k_sem  pkts;
    u16_t         esco_pkt_type;
};
#endif

/* The theoretical max for these is 8 and 64, but there's no point
 * in allocating the full memory if we only support a small subset.
 * These values must be updated whenever the host implementation is
 * extended beyond the current values.
 */
#define BT_DEV_VS_FEAT_MAX  1
#define BT_DEV_VS_CMDS_MAX  2

/* State tracking for the local Bluetooth controller */
struct bt_dev {
    /* Local Identity Address */
    bt_addr_le_t        id_addr;

    /* Current local Random Address */
    bt_addr_le_t        random_addr;

    /* Controller version & manufacturer information */
    u8_t            hci_version;
    u8_t            lmp_version;
    u16_t           hci_revision;
    u16_t           lmp_subversion;
    u16_t           manufacturer;

    /* LMP features (pages 0, 1, 2) */
    u8_t            features[LMP_FEAT_PAGES_COUNT][8];

    /* Supported commands */
    u8_t            supported_commands[64];

#if defined(CONFIG_BT_HCI_VS_EXT)
    /* Vendor HCI support */
    u8_t                    vs_features[BT_DEV_VS_FEAT_MAX];
    u8_t                    vs_commands[BT_DEV_VS_CMDS_MAX];
#endif

#if 0
    struct k_work           init;
#endif /* if 0 */
    ATOMIC_DEFINE(flags, BT_DEV_NUM_FLAGS);

    /* LE controller specific features */
    struct bt_dev_le    le;

#if defined(CONFIG_BT_BREDR)
    /* BR/EDR controller specific features */
    struct bt_dev_br    br;
#endif

#if 0
    /* Number of commands controller can accept */
    struct k_sem        ncmd_sem;

    /* Last sent HCI command */
    struct net_buf      *sent_cmd;
#endif /* if 0 */

#if !defined(CONFIG_BT_RECV_IS_RX_THREAD)
    /* Queue for incoming HCI events & ACL data */
    struct k_fifo       rx_queue;
#endif

#if 0

    /* Queue for high priority HCI events which may unlock waiters
     * in other threads. Such events include Number of Completed
     * Packets, as well as the Command Complete/Status events.
     */
    struct k_fifo       rx_prio_queue;

    /* Queue for outgoing HCI commands */
    struct k_fifo       cmd_tx_queue;

    /* Registered HCI driver */
    const struct bt_hci_driver *drv;
#endif /* #if 0 */
#if defined(CONFIG_BT_PRIVACY)
    /* Local Identity Resolving Key */
    u8_t            irk[16];

    /* Work used for RPA rotation */
    struct k_delayed_work rpa_update;
#endif
};

/* Added by Espressif */
extern struct bt_dev bt_dev;

void mesh_hci_init(void);


/*Porting from zephyr/subsys/bluetooth/host/hci_core.h */
/* HCI version from Assigned Numbers */
#define BT_HCI_VERSION_1_0B                     0
#define BT_HCI_VERSION_1_1                      1
#define BT_HCI_VERSION_1_2                      2
#define BT_HCI_VERSION_2_0                      3
#define BT_HCI_VERSION_2_1                      4
#define BT_HCI_VERSION_3_0                      5
#define BT_HCI_VERSION_4_0                      6
#define BT_HCI_VERSION_4_1                      7
#define BT_HCI_VERSION_4_2                      8
#define BT_HCI_VERSION_5_0                      9

/* OpCode Group Fields */
#define BT_OGF_LINK_CTRL                        0x01
#define BT_OGF_BASEBAND                         0x03
#define BT_OGF_INFO                             0x04
#define BT_OGF_STATUS                           0x05
#define BT_OGF_LE                               0x08
#define BT_OGF_VS                               0x3f

/* Construct OpCode from OGF and OCF */
#define BT_OP(ogf, ocf)                         ((ocf) | ((ogf) << 10))

/* Obtain OGF from OpCode */
#define BT_OGF(opcode)                          (((opcode) >> 10) & BIT_MASK(6))
/* Obtain OCF from OpCode */
#define BT_OCF(opcode)                          ((opcode) & BIT_MASK(10))

#define BT_HCI_OP_LE_SET_ADV_PARAM              BT_OP(BT_OGF_LE, 0x0006)
struct bt_hci_cp_le_set_adv_param {
    u16_t        min_interval;
    u16_t        max_interval;
    u8_t         type;
    u8_t         own_addr_type;
    bt_addr_le_t direct_addr;
    u8_t         channel_map;
    u8_t         filter_policy;
} __packed;

#define BT_HCI_OP_LE_SET_ADV_DATA               BT_OP(BT_OGF_LE, 0x0008)
struct bt_hci_cp_le_set_adv_data {
    u8_t  len;
    u8_t  data[31];
} __packed;

#define BT_HCI_OP_LE_SET_SCAN_RSP_DATA          BT_OP(BT_OGF_LE, 0x0009)
struct bt_hci_cp_le_set_scan_rsp_data {
    u8_t  len;
    u8_t  data[31];
} __packed;


#ifdef __cplusplus
}
#endif

#endif /* __MESH_HCI_H */




