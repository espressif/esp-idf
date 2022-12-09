/*
 * Copyright (c) 2015-2016 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _BLE_MESH_HCI_H_
#define _BLE_MESH_HCI_H_

#include "mesh_atomic.h"
#include "mesh_bearer_adapt.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Porting form zephyr/subsys/bluetooth/host/hci_core.h */

#define BLE_MESH_LMP_FEAT_PAGES_COUNT   1

/* bt_mesh_dev flags: the flags defined here represent BT controller state */
enum {
    BLE_MESH_DEV_ENABLE,
    BLE_MESH_DEV_READY,
    BLE_MESH_DEV_ID_STATIC_RANDOM,
    BLE_MESH_DEV_HAS_PUB_KEY,
    BLE_MESH_DEV_PUB_KEY_BUSY,

    BLE_MESH_DEV_ADVERTISING,
    BLE_MESH_DEV_KEEP_ADVERTISING,
    BLE_MESH_DEV_SCANNING,
    BLE_MESH_DEV_EXPLICIT_SCAN,
    BLE_MESH_DEV_ACTIVE_SCAN,
    BLE_MESH_DEV_SCAN_FILTER_DUP,

    BLE_MESH_DEV_RPA_VALID,

    BLE_MESH_DEV_ID_PENDING,

    /* Total number of flags - must be at the end of the enum */
    BLE_MESH_DEV_NUM_FLAGS,
};

struct bt_mesh_dev_le {
    /* LE features */
    uint8_t  features[8];

    /* LE states */
    uint64_t states;
};

/* State tracking for the local Bluetooth controller */
struct bt_mesh_dev {
    /* Flags indicate which functionality is enabled */
    BLE_MESH_ATOMIC_DEFINE(flags, BLE_MESH_DEV_NUM_FLAGS);

    /* Controller version & manufacturer information */
    uint8_t  hci_version;
    uint8_t  lmp_version;
    uint16_t hci_revision;
    uint16_t lmp_subversion;
    uint16_t manufacturer;

    /* LMP features (pages 0, 1, 2) */
    uint8_t  features[BLE_MESH_LMP_FEAT_PAGES_COUNT][8];

    /* LE controller specific features */
    struct bt_mesh_dev_le le;
};

/*Porting from zephyr/subsys/bluetooth/host/hci_core.h */
/* HCI version from Assigned Numbers */
#define BLE_MESH_HCI_VERSION_1_0B   0
#define BLE_MESH_HCI_VERSION_1_1    1
#define BLE_MESH_HCI_VERSION_1_2    2
#define BLE_MESH_HCI_VERSION_2_0    3
#define BLE_MESH_HCI_VERSION_2_1    4
#define BLE_MESH_HCI_VERSION_3_0    5
#define BLE_MESH_HCI_VERSION_4_0    6
#define BLE_MESH_HCI_VERSION_4_1    7
#define BLE_MESH_HCI_VERSION_4_2    8
#define BLE_MESH_HCI_VERSION_5_0    9

/* OpCode Group Fields */
#define BLE_MESH_OGF_LINK_CTRL      0x01
#define BLE_MESH_OGF_BASEBAND       0x03
#define BLE_MESH_OGF_INFO           0x04
#define BLE_MESH_OGF_STATUS         0x05
#define BLE_MESH_OGF_LE             0x08
#define BLE_MESH_OGF_VS             0x3f

/* Construct OpCode from OGF and OCF */
#define BLE_MESH_OP(ogf, ocf)       ((ocf) | ((ogf) << 10))

/* Obtain OGF from OpCode */
#define BLE_MESH_OGF(opcode)        (((opcode) >> 10) & BIT_MASK(6))

/* Obtain OCF from OpCode */
#define BLE_MESH_OCF(opcode)        ((opcode) & BIT_MASK(10))

#define BLE_MESH_HCI_OP_SET_ADV_PARAM       BLE_MESH_OP(BLE_MESH_OGF_LE, 0x0006)
struct bt_mesh_hci_cp_set_adv_param {
    uint16_t       min_interval;
    uint16_t       max_interval;
    uint8_t        type;
    uint8_t        own_addr_type;
    bt_mesh_addr_t direct_addr;
    uint8_t        channel_map;
    uint8_t        filter_policy;
} __packed;

#define BLE_MESH_HCI_OP_SET_ADV_DATA        BLE_MESH_OP(BLE_MESH_OGF_LE, 0x0008)
struct bt_mesh_hci_cp_set_adv_data {
    uint8_t len;
    uint8_t data[31];
} __packed;

#define BLE_MESH_HCI_OP_SET_SCAN_RSP_DATA   BLE_MESH_OP(BLE_MESH_OGF_LE, 0x0009)
struct bt_mesh_hci_cp_set_scan_rsp_data {
    uint8_t len;
    uint8_t data[31];
} __packed;

/* Added by Espressif */
extern struct bt_mesh_dev bt_mesh_dev;

void bt_mesh_hci_init(void);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MESH_HCI_H_ */
