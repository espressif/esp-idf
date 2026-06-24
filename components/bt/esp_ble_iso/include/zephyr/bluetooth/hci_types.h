/* hci.h - Bluetooth Host Control Interface types */

/*
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 * SPDX-FileCopyrightText: 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ZEPHYR_INCLUDE_BLUETOOTH_HCI_TYPES_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_HCI_TYPES_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <zephyr/sys/util.h>
#include <zephyr/bluetooth/addr.h>
#include <zephyr/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Bluetooth spec v5.4 Vol 4, Part A Table 2.1: HCI packet indicators
 * The following definitions are intended for use with the UART Transport Layer and
 * may be reused with other transport layers if desired.
 */
#define BT_HCI_H4_NONE                  0x00    /* None of the known packet types */
#define BT_HCI_H4_CMD                   0x01    /* HCI Command packet */
#define BT_HCI_H4_ACL                   0x02    /* HCI ACL Data packet */
#define BT_HCI_H4_SCO                   0x03    /* HCI Synchronous Data packet */
#define BT_HCI_H4_EVT                   0x04    /* HCI Event packet */
#define BT_HCI_H4_ISO                   0x05    /* HCI ISO Data packet */

#define BT_HCI_OWN_ADDR_PUBLIC          0x00
#define BT_HCI_OWN_ADDR_RANDOM          0x01
#define BT_HCI_OWN_ADDR_RPA_OR_PUBLIC   0x02
#define BT_HCI_OWN_ADDR_RPA_OR_RANDOM   0x03
#define BT_HCI_OWN_ADDR_RPA_MASK        0x02

#define BT_HCI_PEER_ADDR_RPA_UNRESOLVED 0xfe
#define BT_HCI_PEER_ADDR_ANONYMOUS      0xff

#define BT_ENC_KEY_SIZE_MIN             0x07
#define BT_ENC_KEY_SIZE_MAX             0x10

#define BT_HCI_ADV_HANDLE_INVALID       0xff
#define BT_HCI_SYNC_HANDLE_INVALID      0xffff
#define BT_HCI_PAWR_SUBEVENT_MAX        128

/* Bluetooth spec v5.4 Vol 4, Part E - 5.4.3 HCI Synchronous Data Packets */
struct bt_hci_sco_hdr {
    uint16_t handle; /* 12 bit handle, 2 bit Packet Status Flag, 1 bit RFU */
    uint8_t  len;
} __packed;
#define BT_HCI_SCO_HDR_SIZE             3

/* Bluetooth spec v5.4 Vol 4, Part E - 5.4.4 HCI Event Packet */
struct bt_hci_evt_hdr {
    uint8_t  evt;
    uint8_t  len;
} __packed;
#define BT_HCI_EVT_HDR_SIZE             2

#define BT_ACL_START_NO_FLUSH           0x00
#define BT_ACL_CONT                     0x01
#define BT_ACL_START                    0x02
#define BT_ACL_COMPLETE                 0x03

#define BT_ACL_POINT_TO_POINT           0x00
#define BT_ACL_BROADCAST                0x01

#define BT_ACL_HANDLE_MASK              BIT_MASK(12)

#define bt_acl_handle(h)                ((h) & BT_ACL_HANDLE_MASK)
#define bt_acl_flags(h)                 ((h) >> 12)
#define bt_acl_flags_pb(f)              ((f) & BIT_MASK(2))
#define bt_acl_flags_bc(f)              ((f) >> 2)
#define bt_acl_handle_pack(h, f)        ((h) | ((f) << 12))

/* Bluetooth spec v5.4 Vol 4, Part E - 5.4.2 ACL Data Packets */
struct bt_hci_acl_hdr {
    uint16_t handle;
    uint16_t len;
} __packed;
#define BT_HCI_ACL_HDR_SIZE             4

#define BT_ISO_START                    0x00
#define BT_ISO_CONT                     0x01
#define BT_ISO_SINGLE                   0x02
#define BT_ISO_END                      0x03

#define bt_iso_handle(h)                ((h) & 0x0fff)
#define bt_iso_flags(h)                 ((h) >> 12)
#define bt_iso_flags_pb(f)              ((f) & 0x0003)
#define bt_iso_flags_ts(f)              (((f) >> 2) & 0x0001)
#define bt_iso_pack_flags(pb, ts) \
    (((pb) & 0x0003) | (((ts) & 0x0001) << 2))
#define bt_iso_handle_pack(h, pb, ts) \
    ((h) | (bt_iso_pack_flags(pb, ts) << 12))
#define bt_iso_hdr_len(h)                ((h) & BIT_MASK(14))

#define BT_ISO_DATA_VALID                0x00
#define BT_ISO_DATA_INVALID              0x01
#define BT_ISO_DATA_NOP                  0x02

#define bt_iso_pkt_len(h)                ((h) & BIT_MASK(12))
#define bt_iso_pkt_flags(h)              ((h) >> 14)
#define bt_iso_pkt_len_pack(h, f)        (((h) & BIT_MASK(12)) | ((f) << 14))

struct bt_hci_iso_sdu_hdr {
    uint16_t sn;
    uint16_t slen; /* 12 bit len, 2 bit RFU, 2 bit packet status */
} __packed;
#define BT_HCI_ISO_SDU_HDR_SIZE          4

struct bt_hci_iso_sdu_ts_hdr {
    uint32_t ts;
    struct bt_hci_iso_sdu_hdr sdu;
} __packed;
#define BT_HCI_ISO_SDU_TS_HDR_SIZE       8

/* Bluetooth spec v5.4 Vol 4, Part E - 5.4.5 HCI ISO Data Packets */
struct bt_hci_iso_hdr {
    uint16_t handle; /* 12 bit handle, 2 bit PB flags, 1 bit TS_Flag, 1 bit RFU */
    uint16_t len; /* 14 bits, 2 bits RFU */
} __packed;
#define BT_HCI_ISO_HDR_SIZE             4

/* Bluetooth spec v5.4 Vol 4, Part E - 5.4.1 HCI Command Packet */
struct bt_hci_cmd_hdr {
    uint16_t opcode;
    uint8_t  param_len;
} __packed;
#define BT_HCI_CMD_HDR_SIZE             3

/* Supported Commands */
#define BT_CMD_TEST(cmd, octet, bit)            (cmd[octet] & BIT(bit))
#define BT_CMD_LE_STATES(cmd)                   BT_CMD_TEST(cmd, 28, 3)

#define BT_FEAT_TEST(feat, page, octet, bit)    (feat[page][octet] & BIT(bit))

#define BT_FEAT_BREDR(feat)                     !BT_FEAT_TEST(feat, 0, 4, 5)
#define BT_FEAT_LE(feat)                        BT_FEAT_TEST(feat, 0, 4, 6)
#define BT_FEAT_EXT_FEATURES(feat)              BT_FEAT_TEST(feat, 0, 7, 7)
#define BT_FEAT_HOST_SSP(feat)                  BT_FEAT_TEST(feat, 1, 0, 0)
#define BT_FEAT_SC(feat)                        BT_FEAT_TEST(feat, 2, 1, 0)

#define BT_FEAT_LMP_SCO_CAPABLE(feat)           BT_FEAT_TEST(feat, 0, 1, 3)
#define BT_FEAT_LMP_ESCO_CAPABLE(feat)          BT_FEAT_TEST(feat, 0, 3, 7)
#define BT_FEAT_HV2_PKT(feat)                   BT_FEAT_TEST(feat, 0, 1, 4)
#define BT_FEAT_HV3_PKT(feat)                   BT_FEAT_TEST(feat, 0, 1, 5)
#define BT_FEAT_EV4_PKT(feat)                   BT_FEAT_TEST(feat, 0, 4, 0)
#define BT_FEAT_EV5_PKT(feat)                   BT_FEAT_TEST(feat, 0, 4, 1)
#define BT_FEAT_2EV3_PKT(feat)                  BT_FEAT_TEST(feat, 0, 5, 5)
#define BT_FEAT_3EV3_PKT(feat)                  BT_FEAT_TEST(feat, 0, 5, 6)
#define BT_FEAT_3SLOT_PKT(feat)                 BT_FEAT_TEST(feat, 0, 5, 7)

/* LE features */
#define BT_LE_FEAT_BIT_ENC                      0
#define BT_LE_FEAT_BIT_CONN_PARAM_REQ           1
#define BT_LE_FEAT_BIT_EXT_REJ_IND              2
#define BT_LE_FEAT_BIT_PER_INIT_FEAT_XCHG       3
#define BT_LE_FEAT_BIT_PING                     4
#define BT_LE_FEAT_BIT_DLE                      5
#define BT_LE_FEAT_BIT_PRIVACY                  6
#define BT_LE_FEAT_BIT_EXT_SCAN                 7
#define BT_LE_FEAT_BIT_PHY_2M                   8
#define BT_LE_FEAT_BIT_SMI_TX                   9
#define BT_LE_FEAT_BIT_SMI_RX                   10
#define BT_LE_FEAT_BIT_PHY_CODED                11
#define BT_LE_FEAT_BIT_EXT_ADV                  12
#define BT_LE_FEAT_BIT_PER_ADV                  13
#define BT_LE_FEAT_BIT_CHAN_SEL_ALGO_2          14
#define BT_LE_FEAT_BIT_PWR_CLASS_1              15
#define BT_LE_FEAT_BIT_MIN_USED_CHAN_PROC       16
#define BT_LE_FEAT_BIT_CONN_CTE_REQ             17
#define BT_LE_FEAT_BIT_CONN_CTE_RESP            18
#define BT_LE_FEAT_BIT_CONNECTIONLESS_CTE_TX    19
#define BT_LE_FEAT_BIT_CONNECTIONLESS_CTE_RX    20
#define BT_LE_FEAT_BIT_ANT_SWITCH_TX_AOD        21
#define BT_LE_FEAT_BIT_ANT_SWITCH_RX_AOA        22
#define BT_LE_FEAT_BIT_RX_CTE                   23
#define BT_LE_FEAT_BIT_PAST_SEND                24
#define BT_LE_FEAT_BIT_PAST_RECV                25
#define BT_LE_FEAT_BIT_SCA_UPDATE               26
#define BT_LE_FEAT_BIT_REMOTE_PUB_KEY_VALIDATE  27
#define BT_LE_FEAT_BIT_CIS_CENTRAL              28
#define BT_LE_FEAT_BIT_CIS_PERIPHERAL           29
#define BT_LE_FEAT_BIT_ISO_BROADCASTER          30
#define BT_LE_FEAT_BIT_SYNC_RECEIVER            31
#define BT_LE_FEAT_BIT_ISO_CHANNELS             32
#define BT_LE_FEAT_BIT_PWR_CTRL_REQ             33
#define BT_LE_FEAT_BIT_PWR_CHG_IND              34
#define BT_LE_FEAT_BIT_PATH_LOSS_MONITOR        35
#define BT_LE_FEAT_BIT_PER_ADV_ADI_SUPP         36
#define BT_LE_FEAT_BIT_CONN_SUBRATING           37
#define BT_LE_FEAT_BIT_CONN_SUBRATING_HOST_SUPP 38
#define BT_LE_FEAT_BIT_CHANNEL_CLASSIFICATION   39

#define BT_LE_FEAT_BIT_PAWR_ADVERTISER          43
#define BT_LE_FEAT_BIT_PAWR_SCANNER             44

#define BT_LE_FEAT_BIT_CHANNEL_SOUNDING         46
#define BT_LE_FEAT_BIT_CHANNEL_SOUNDING_HOST    47

#define BT_LE_FEAT_TEST(feat, n)                (feat[(n) >> 3] & \
                         BIT((n) & 7))

#define BT_FEAT_LE_ENCR(feat)                     BT_LE_FEAT_TEST(feat, \
                          BT_LE_FEAT_BIT_ENC)
#define BT_FEAT_LE_CONN_PARAM_REQ_PROC(feat)      BT_LE_FEAT_TEST(feat, \
                          BT_LE_FEAT_BIT_CONN_PARAM_REQ)
#define BT_FEAT_LE_PER_INIT_FEAT_XCHG(feat)       BT_LE_FEAT_TEST(feat, \
                          BT_LE_FEAT_BIT_PER_INIT_FEAT_XCHG)
#define BT_FEAT_LE_DLE(feat)                      BT_LE_FEAT_TEST(feat, \
                          BT_LE_FEAT_BIT_DLE)
#define BT_FEAT_LE_PHY_2M(feat)                   BT_LE_FEAT_TEST(feat, \
                          BT_LE_FEAT_BIT_PHY_2M)
#define BT_FEAT_LE_PHY_CODED(feat)                BT_LE_FEAT_TEST(feat, \
                          BT_LE_FEAT_BIT_PHY_CODED)
#define BT_FEAT_LE_PRIVACY(feat)                  BT_LE_FEAT_TEST(feat, \
                          BT_LE_FEAT_BIT_PRIVACY)
#define BT_FEAT_LE_EXT_ADV(feat)                  BT_LE_FEAT_TEST(feat, \
                          BT_LE_FEAT_BIT_EXT_ADV)
#define BT_FEAT_LE_EXT_PER_ADV(feat)              BT_LE_FEAT_TEST(feat, \
                          BT_LE_FEAT_BIT_PER_ADV)
#define BT_FEAT_LE_CONNECTION_CTE_REQ(feat)       BT_LE_FEAT_TEST(feat, \
                          BT_LE_FEAT_BIT_CONN_CTE_REQ)
#define BT_FEAT_LE_CONNECTION_CTE_RESP(feat)      BT_LE_FEAT_TEST(feat, \
                          BT_LE_FEAT_BIT_CONN_CTE_RESP)
#define BT_FEAT_LE_CONNECTIONLESS_CTE_TX(feat)    BT_LE_FEAT_TEST(feat, \
                          BT_LE_FEAT_BIT_CONNECTIONLESS_CTE_TX)
#define BT_FEAT_LE_CONNECTIONLESS_CTE_RX(feat)    BT_LE_FEAT_TEST(feat, \
                          BT_LE_FEAT_BIT_CONNECTIONLESS_CTE_RX)
#define BT_FEAT_LE_ANT_SWITCH_TX_AOD(feat)        BT_LE_FEAT_TEST(feat, \
                          BT_LE_FEAT_BIT_ANT_SWITCH_TX_AOD)
#define BT_FEAT_LE_ANT_SWITCH_RX_AOA(feat)        BT_LE_FEAT_TEST(feat, \
                          BT_LE_FEAT_BIT_ANT_SWITCH_RX_AOA)
#define BT_FEAT_LE_RX_CTE(feat)                   BT_LE_FEAT_TEST(feat, \
                          BT_LE_FEAT_BIT_RX_CTE)
#define BT_FEAT_LE_PAST_SEND(feat)                BT_LE_FEAT_TEST(feat, \
                          BT_LE_FEAT_BIT_PAST_SEND)
#define BT_FEAT_LE_PAST_RECV(feat)                BT_LE_FEAT_TEST(feat, \
                          BT_LE_FEAT_BIT_PAST_RECV)
#define BT_FEAT_LE_CIS_CENTRAL(feat)              BT_LE_FEAT_TEST(feat, \
                          BT_LE_FEAT_BIT_CIS_CENTRAL)
#define BT_FEAT_LE_CIS_PERIPHERAL(feat)           BT_LE_FEAT_TEST(feat, \
                          BT_LE_FEAT_BIT_CIS_PERIPHERAL)
#define BT_FEAT_LE_ISO_BROADCASTER(feat)          BT_LE_FEAT_TEST(feat, \
                          BT_LE_FEAT_BIT_ISO_BROADCASTER)
#define BT_FEAT_LE_SYNC_RECEIVER(feat)            BT_LE_FEAT_TEST(feat, \
                          BT_LE_FEAT_BIT_SYNC_RECEIVER)
#define BT_FEAT_LE_ISO_CHANNELS(feat)             BT_LE_FEAT_TEST(feat, \
                          BT_LE_FEAT_BIT_ISO_CHANNELS)
#define BT_FEAT_LE_PWR_CTRL_REQ(feat)             BT_LE_FEAT_TEST(feat, \
                          BT_LE_FEAT_BIT_PWR_CTRL_REQ)
#define BT_FEAT_LE_PWR_CHG_IND(feat)              BT_LE_FEAT_TEST(feat, \
                          BT_LE_FEAT_BIT_PWR_CHG_IND)
#define BT_FEAT_LE_PATH_LOSS_MONITOR(feat)        BT_LE_FEAT_TEST(feat, \
                          BT_LE_FEAT_BIT_PATH_LOSS_MONITOR)
#define BT_FEAT_LE_PER_ADV_ADI_SUPP(feat)         BT_LE_FEAT_TEST(feat, \
                          BT_LE_FEAT_BIT_PER_ADV_ADI_SUPP)
#define BT_FEAT_LE_CONN_SUBRATING(feat)           BT_LE_FEAT_TEST(feat, \
                          BT_LE_FEAT_BIT_CONN_SUBRATING)
#define BT_FEAT_LE_CONN_SUBRATING_HOST_SUPP(feat) BT_LE_FEAT_TEST(feat, \
                          BT_LE_FEAT_BIT_CONN_SUBRATING_HOST_SUPP)
#define BT_FEAT_LE_CHANNEL_CLASSIFICATION(feat)   BT_LE_FEAT_TEST(feat, \
                          BT_LE_FEAT_BIT_CHANNEL_CLASSIFICATION)
#define BT_FEAT_LE_PAWR_ADVERTISER(feat)      BT_LE_FEAT_TEST(feat, \
                          BT_LE_FEAT_BIT_PAWR_ADVERTISER)
#define BT_FEAT_LE_PAWR_SCANNER(feat)             BT_LE_FEAT_TEST(feat, \
                          BT_LE_FEAT_BIT_PAWR_SCANNER)
#define BT_FEAT_LE_CHANNEL_SOUNDING(feat)         BT_LE_FEAT_TEST(feat, \
                          BT_LE_FEAT_BIT_CHANNEL_SOUNDING)
#define BT_FEAT_LE_CHANNEL_SOUNDING_HOST(feat)    BT_LE_FEAT_TEST(feat, \
                          BT_LE_FEAT_BIT_CHANNEL_SOUNDING_HOST)

#define BT_FEAT_LE_CIS(feat)            (BT_FEAT_LE_CIS_CENTRAL(feat) | \
                    BT_FEAT_LE_CIS_PERIPHERAL(feat))
#define BT_FEAT_LE_BIS(feat)            (BT_FEAT_LE_ISO_BROADCASTER(feat) | \
                    BT_FEAT_LE_SYNC_RECEIVER(feat))
#define BT_FEAT_LE_ISO(feat)            (BT_FEAT_LE_CIS(feat) | \
                    BT_FEAT_LE_BIS(feat))

/* LE States. See Core_v5.4, Vol 4, Part E, Section 7.8.27 */
#define BT_LE_STATES_PER_CONN_ADV(states)     (states & BIT64_MASK(38))

#if defined(CONFIG_BT_SCAN_AND_INITIATE_IN_PARALLEL)
/* Both passive and active scanner can be run in parallel with initiator. */
#define BT_LE_STATES_SCAN_INIT(states) ((states) & BIT64_MASK(22) && \
                    (states) & BIT64_MASK(23))

#else
#define BT_LE_STATES_SCAN_INIT(states)  0
#endif

/* Bonding/authentication types */
#define BT_HCI_NO_BONDING                       0x00
#define BT_HCI_NO_BONDING_MITM                  0x01
#define BT_HCI_DEDICATED_BONDING                0x02
#define BT_HCI_DEDICATED_BONDING_MITM           0x03
#define BT_HCI_GENERAL_BONDING                  0x04
#define BT_HCI_GENERAL_BONDING_MITM             0x05

/*
 * MITM protection is enabled in SSP authentication requirements octet when
 * LSB bit is set.
 */
#define BT_MITM                                 0x01

/* I/O capabilities */
#define BT_IO_DISPLAY_ONLY                      0x00
#define BT_IO_DISPLAY_YESNO                     0x01
#define BT_IO_KEYBOARD_ONLY                     0x02
#define BT_IO_NO_INPUT_OUTPUT                   0x03

/* SCO packet types */
#define HCI_PKT_TYPE_HV1                        0x0020
#define HCI_PKT_TYPE_HV2                        0x0040
#define HCI_PKT_TYPE_HV3                        0x0080

/* eSCO packet types */
#define HCI_PKT_TYPE_SCO_HV1                    0x0001
#define HCI_PKT_TYPE_SCO_HV2                    0x0002
#define HCI_PKT_TYPE_SCO_HV3                    0x0004
#define HCI_PKT_TYPE_ESCO_EV3                   0x0008
#define HCI_PKT_TYPE_ESCO_EV4                   0x0010
#define HCI_PKT_TYPE_ESCO_EV5                   0x0020
#define HCI_PKT_TYPE_ESCO_2EV3                  0x0040
#define HCI_PKT_TYPE_ESCO_3EV3                  0x0080
#define HCI_PKT_TYPE_ESCO_2EV5                  0x0100
#define HCI_PKT_TYPE_ESCO_3EV5                  0x0200

#define ESCO_PKT_MASK                           (HCI_PKT_TYPE_SCO_HV1 | \
                         HCI_PKT_TYPE_SCO_HV2 | \
                         HCI_PKT_TYPE_SCO_HV3 | \
                         HCI_PKT_TYPE_ESCO_EV3 | \
                         HCI_PKT_TYPE_ESCO_EV4 | \
                         HCI_PKT_TYPE_ESCO_EV5)
#define SCO_PKT_MASK                            (HCI_PKT_TYPE_SCO_HV1 | \
                         HCI_PKT_TYPE_SCO_HV2 | \
                         HCI_PKT_TYPE_SCO_HV3)
#define EDR_ESCO_PKT_MASK                       (HCI_PKT_TYPE_ESCO_2EV3 | \
                         HCI_PKT_TYPE_ESCO_3EV3 | \
                         HCI_PKT_TYPE_ESCO_2EV5 | \
                         HCI_PKT_TYPE_ESCO_3EV5)

/* HCI BR/EDR link types */
#define BT_HCI_SCO                              0x00
#define BT_HCI_ACL                              0x01
#define BT_HCI_ESCO                             0x02

/* OpCode Group Fields */
#define BT_OGF_LINK_CTRL                        0x01
#define BT_OGF_BASEBAND                         0x03
#define BT_OGF_INFO                             0x04
#define BT_OGF_STATUS                           0x05
#define BT_OGF_LE                               0x08
#define BT_OGF_VS                               0x3f

/* Construct OpCode from OGF and OCF */
#define BT_OP(ogf, ocf)                         ((ocf) | ((ogf) << 10))

/* Invalid opcode */
#define BT_OP_NOP               0x0000

/* Obtain OGF from OpCode */
#define BT_OGF(opcode)                          (((opcode) >> 10) & BIT_MASK(6))
/* Obtain OCF from OpCode */
#define BT_OCF(opcode)                          ((opcode) & BIT_MASK(10))

#define BT_HCI_OP_INQUIRY                       BT_OP(BT_OGF_LINK_CTRL, 0x0001) /* 0x0401 */
struct bt_hci_op_inquiry {
    uint8_t lap[3];
    uint8_t length;
    uint8_t num_rsp;
} __packed;

#define BT_HCI_OP_INQUIRY_CANCEL                BT_OP(BT_OGF_LINK_CTRL, 0x0002) /* 0x0402 */

#define BT_HCI_OP_CONNECT                       BT_OP(BT_OGF_LINK_CTRL, 0x0005) /* 0x0405 */
struct bt_hci_cp_connect {
    bt_addr_t bdaddr;
    uint16_t  packet_type;
    uint8_t   pscan_rep_mode;
    uint8_t   reserved;
    uint16_t  clock_offset;
    uint8_t   allow_role_switch;
} __packed;

#define BT_HCI_OP_DISCONNECT                    BT_OP(BT_OGF_LINK_CTRL, 0x0006) /* 0x0406 */
struct bt_hci_cp_disconnect {
    uint16_t handle;
    uint8_t  reason;
} __packed;

#define BT_HCI_OP_CONNECT_CANCEL                BT_OP(BT_OGF_LINK_CTRL, 0x0008) /* 0x0408 */
struct bt_hci_cp_connect_cancel {
    bt_addr_t bdaddr;
} __packed;
struct bt_hci_rp_connect_cancel {
    uint8_t   status;
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_OP_ACCEPT_CONN_REQ               BT_OP(BT_OGF_LINK_CTRL, 0x0009) /* 0x0409 */
struct bt_hci_cp_accept_conn_req {
    bt_addr_t bdaddr;
    uint8_t   role;
} __packed;

#define BT_HCI_OP_SETUP_SYNC_CONN               BT_OP(BT_OGF_LINK_CTRL, 0x0028) /* 0x0428 */
struct bt_hci_cp_setup_sync_conn {
    uint16_t  handle;
    uint32_t  tx_bandwidth;
    uint32_t  rx_bandwidth;
    uint16_t  max_latency;
    uint16_t  content_format;
    uint8_t   retrans_effort;
    uint16_t  pkt_type;
} __packed;

#define BT_HCI_OP_ACCEPT_SYNC_CONN_REQ          BT_OP(BT_OGF_LINK_CTRL, 0x0029) /* 0x0429 */
struct bt_hci_cp_accept_sync_conn_req {
    bt_addr_t bdaddr;
    uint32_t  tx_bandwidth;
    uint32_t  rx_bandwidth;
    uint16_t  max_latency;
    uint16_t  content_format;
    uint8_t   retrans_effort;
    uint16_t  pkt_type;
} __packed;

#define BT_HCI_OP_REJECT_CONN_REQ               BT_OP(BT_OGF_LINK_CTRL, 0x000a) /* 0x040a */
struct bt_hci_cp_reject_conn_req {
    bt_addr_t bdaddr;
    uint8_t   reason;
} __packed;

#define BT_HCI_OP_LINK_KEY_REPLY                BT_OP(BT_OGF_LINK_CTRL, 0x000b) /* 0x040b */
struct bt_hci_cp_link_key_reply {
    bt_addr_t bdaddr;
    uint8_t   link_key[16];
} __packed;

#define BT_HCI_OP_LINK_KEY_NEG_REPLY            BT_OP(BT_OGF_LINK_CTRL, 0x000c) /* 0x040c */
struct bt_hci_cp_link_key_neg_reply {
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_OP_PIN_CODE_REPLY                BT_OP(BT_OGF_LINK_CTRL, 0x000d) /* 0x040d */
struct bt_hci_cp_pin_code_reply {
    bt_addr_t bdaddr;
    uint8_t   pin_len;
    uint8_t   pin_code[16];
} __packed;
struct bt_hci_rp_pin_code_reply {
    uint8_t      status;
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_OP_PIN_CODE_NEG_REPLY            BT_OP(BT_OGF_LINK_CTRL, 0x000e) /* 0x040e */
struct bt_hci_cp_pin_code_neg_reply {
    bt_addr_t bdaddr;
} __packed;
struct bt_hci_rp_pin_code_neg_reply {
    uint8_t   status;
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_OP_AUTH_REQUESTED                BT_OP(BT_OGF_LINK_CTRL, 0x0011) /* 0x0411 */
struct bt_hci_cp_auth_requested {
    uint16_t handle;
} __packed;

#define BT_HCI_OP_SET_CONN_ENCRYPT              BT_OP(BT_OGF_LINK_CTRL, 0x0013) /* 0x0413 */
struct bt_hci_cp_set_conn_encrypt {
    uint16_t handle;
    uint8_t  encrypt;
} __packed;

#define BT_HCI_OP_REMOTE_NAME_REQUEST           BT_OP(BT_OGF_LINK_CTRL, 0x0019) /* 0x0419 */
struct bt_hci_cp_remote_name_request {
    bt_addr_t bdaddr;
    uint8_t   pscan_rep_mode;
    uint8_t   reserved;
    uint16_t  clock_offset;
} __packed;

#define BT_HCI_OP_REMOTE_NAME_CANCEL            BT_OP(BT_OGF_LINK_CTRL, 0x001a) /* 0x041a */
struct bt_hci_cp_remote_name_cancel {
    bt_addr_t bdaddr;
} __packed;
struct bt_hci_rp_remote_name_cancel {
    uint8_t   status;
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_OP_READ_REMOTE_FEATURES          BT_OP(BT_OGF_LINK_CTRL, 0x001b) /* 0x041b */
struct bt_hci_cp_read_remote_features {
    uint16_t handle;
} __packed;

#define BT_HCI_OP_READ_REMOTE_EXT_FEATURES      BT_OP(BT_OGF_LINK_CTRL, 0x001c) /* 0x041c */
struct bt_hci_cp_read_remote_ext_features {
    uint16_t handle;
    uint8_t  page;
} __packed;

#define BT_HCI_OP_READ_REMOTE_VERSION_INFO      BT_OP(BT_OGF_LINK_CTRL, 0x001d) /* 0x041d */
struct bt_hci_cp_read_remote_version_info {
    uint16_t handle;
} __packed;

#define BT_HCI_OP_IO_CAPABILITY_REPLY           BT_OP(BT_OGF_LINK_CTRL, 0x002b) /* 0x042b */
struct bt_hci_cp_io_capability_reply {
    bt_addr_t bdaddr;
    uint8_t   capability;
    uint8_t   oob_data;
    uint8_t   authentication;
} __packed;

#define BT_HCI_OP_USER_CONFIRM_REPLY            BT_OP(BT_OGF_LINK_CTRL, 0x002c) /* 0x042c */
#define BT_HCI_OP_USER_CONFIRM_NEG_REPLY        BT_OP(BT_OGF_LINK_CTRL, 0x002d) /* 0x042d */
struct bt_hci_cp_user_confirm_reply {
    bt_addr_t bdaddr;
} __packed;
struct bt_hci_rp_user_confirm_reply {
    uint8_t   status;
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_OP_USER_PASSKEY_REPLY            BT_OP(BT_OGF_LINK_CTRL, 0x002e) /* 0x042e */
struct bt_hci_cp_user_passkey_reply {
    bt_addr_t bdaddr;
    uint32_t  passkey;
} __packed;

#define BT_HCI_OP_USER_PASSKEY_NEG_REPLY        BT_OP(BT_OGF_LINK_CTRL, 0x002f) /* 0x042f */
struct bt_hci_cp_user_passkey_neg_reply {
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_OP_IO_CAPABILITY_NEG_REPLY       BT_OP(BT_OGF_LINK_CTRL, 0x0034) /* 0x0434 */
struct bt_hci_cp_io_capability_neg_reply {
    bt_addr_t bdaddr;
    uint8_t   reason;
} __packed;

#define BT_HCI_OP_SET_EVENT_MASK                BT_OP(BT_OGF_BASEBAND, 0x0001) /* 0x0c01 */
struct bt_hci_cp_set_event_mask {
    uint8_t  events[8];
} __packed;

#define BT_HCI_OP_RESET                         BT_OP(BT_OGF_BASEBAND, 0x0003) /* 0x0c03 */

#define BT_HCI_OP_WRITE_LOCAL_NAME              BT_OP(BT_OGF_BASEBAND, 0x0013) /* 0x0c13 */
struct bt_hci_write_local_name {
    uint8_t local_name[248];
} __packed;

#define BT_HCI_OP_READ_CONN_ACCEPT_TIMEOUT      BT_OP(BT_OGF_BASEBAND, 0x0015) /* 0x0c15 */
struct bt_hci_rp_read_conn_accept_timeout {
    uint8_t  status;
    uint16_t conn_accept_timeout;
} __packed;

#define BT_HCI_OP_WRITE_CONN_ACCEPT_TIMEOUT     BT_OP(BT_OGF_BASEBAND, 0x0016) /* 0x0c16 */
struct bt_hci_cp_write_conn_accept_timeout {
    uint16_t conn_accept_timeout;
} __packed;

struct bt_hci_rp_write_conn_accept_timeout {
    uint8_t  status;
} __packed;

#define BT_HCI_OP_WRITE_PAGE_TIMEOUT            BT_OP(BT_OGF_BASEBAND, 0x0018) /* 0x0c18 */

#define BT_HCI_OP_WRITE_SCAN_ENABLE             BT_OP(BT_OGF_BASEBAND, 0x001a) /* 0x0c1a */
#define BT_BREDR_SCAN_DISABLED                  0x00
#define BT_BREDR_SCAN_INQUIRY                   0x01
#define BT_BREDR_SCAN_PAGE                      0x02

#define BT_COD(major_service, major_device, minor_device)                         \
    (((uint32_t)major_service << 13) | ((uint32_t)major_device << 8) |            \
     ((uint32_t)minor_device << 2))
#define BT_COD_VALID(cod) ((0 == (cod[0] & (BIT(0) | BIT(1)))) ? true : false)
#define BT_COD_MAJOR_SERVICE_CLASSES(cod)                                         \
    ((((uint32_t)cod[2] & 0xFF) >> 5) | (((uint32_t)cod[1] & 0xD0) >> 5))
#define BT_COD_MAJOR_DEVICE_CLASS(cod) ((((uint32_t)cod[1]) & 0x1FUL))
#define BT_COD_MINOR_DEVICE_CLASS(cod) (((((uint32_t)cod[0]) & 0xFF) >> 2))

#define BT_COD_MAJOR_MISC           0x00
#define BT_COD_MAJOR_COMPUTER       0x01
#define BT_COD_MAJOR_PHONE          0x02
#define BT_COD_MAJOR_LAN_NETWORK_AP 0x03
#define BT_COD_MAJOR_AUDIO_VIDEO    0x04
#define BT_COD_MAJOR_PERIPHERAL     0x05
#define BT_COD_MAJOR_IMAGING        0x06
#define BT_COD_MAJOR_WEARABLE       0x07
#define BT_COD_MAJOR_TOY            0x08
#define BT_COD_MAJOR_HEALTH         0x09
#define BT_COD_MAJOR_UNCATEGORIZED  0x1F

/* Minor Device Class field - Computer Major Class */
#define BT_COD_MAJOR_COMPUTER_MINOR_UNCATEGORIZED         0x00
#define BT_COD_MAJOR_COMPUTER_MINOR_DESKTOP               0x01
#define BT_COD_MAJOR_COMPUTER_MINOR_SERVER_CLASS_COMPUTER 0x02
#define BT_COD_MAJOR_COMPUTER_MINOR_LAPTOP                0x03
#define BT_COD_MAJOR_COMPUTER_MINOR_HANDHELD_PC_PDA       0x04
#define BT_COD_MAJOR_COMPUTER_MINOR_PALM_SIZE_PC_PDA      0x05
#define BT_COD_MAJOR_COMPUTER_MINOR_WEARABLE_COMPUTER     0x06
#define BT_COD_MAJOR_COMPUTER_MINOR_TABLET                0x07

/* Minor Device Class field - Phone Major Class */
#define BT_COD_MAJOR_PHONE_MINOR_UNCATEGORIZED             0x00
#define BT_COD_MAJOR_PHONE_MINOR_CELLULAR                  0x01
#define BT_COD_MAJOR_PHONE_MINOR_CORDLESS                  0x02
#define BT_COD_MAJOR_PHONE_MINOR_SMARTPHONE                0x03
#define BT_COD_MAJOR_PHONE_MINOR_WIRED_MODEM_VOICE_GATEWAY 0x04
#define BT_COD_MAJOR_PHONE_MINOR_ISDN                      0x05

/* Minor Device Class field - Audio/Video Major Class */
#define BT_COD_MAJOR_AUDIO_VIDEO_MINOR_UNCATEGORIZED             0x00
#define BT_COD_MAJOR_AUDIO_VIDEO_MINOR_WEARABLE_HEADSET          0x01
#define BT_COD_MAJOR_AUDIO_VIDEO_MINOR_HANDS_FREE                0x02
#define BT_COD_MAJOR_AUDIO_VIDEO_MINOR_RFU                       0x03
#define BT_COD_MAJOR_AUDIO_VIDEO_MINOR_MICROPHONE                0x04
#define BT_COD_MAJOR_AUDIO_VIDEO_MINOR_LOUDSPEAKER               0x05
#define BT_COD_MAJOR_AUDIO_VIDEO_MINOR_HEADPHONES                0x06
#define BT_COD_MAJOR_AUDIO_VIDEO_MINOR_PORTABLE_AUDIO            0x07
#define BT_COD_MAJOR_AUDIO_VIDEO_MINOR_CAR_AUDIO                 0x08
#define BT_COD_MAJOR_AUDIO_VIDEO_MINOR_SET_TOP_BOX               0x09
#define BT_COD_MAJOR_AUDIO_VIDEO_MINOR_HIFI_AUDIO                0x0A
#define BT_COD_MAJOR_AUDIO_VIDEO_MINOR_VCR                       0x0B
#define BT_COD_MAJOR_AUDIO_VIDEO_MINOR_VIDEO_CAMERA              0x0C
#define BT_COD_MAJOR_AUDIO_VIDEO_MINOR_CAMCORDER                 0x0D
#define BT_COD_MAJOR_AUDIO_VIDEO_MINOR_VIDEO_MONITOR             0x0E
#define BT_COD_MAJOR_AUDIO_VIDEO_MINOR_VIDEO_DISPLAY_LOUDSPEAKER 0x0F
#define BT_COD_MAJOR_AUDIO_VIDEO_MINOR_VIDEO_CONFERENCING        0x10
#define BT_COD_MAJOR_AUDIO_VIDEO_MINOR_RFU2                      0x11
#define BT_COD_MAJOR_AUDIO_VIDEO_MINOR_GAME_TOY                  0x12

#define BT_HCI_OP_WRITE_CLASS_OF_DEVICE         BT_OP(BT_OGF_BASEBAND, 0x0024) /* 0x0c24 */
struct bt_hci_cp_write_class_of_device {
    uint8_t  class_of_device[3];
} __packed;

#define BT_TX_POWER_LEVEL_CURRENT               0x00
#define BT_TX_POWER_LEVEL_MAX                   0x01
#define BT_HCI_OP_READ_TX_POWER_LEVEL           BT_OP(BT_OGF_BASEBAND, 0x002d) /* 0x0c2d */
struct bt_hci_cp_read_tx_power_level {
    uint16_t handle;
    uint8_t  type;
} __packed;

struct bt_hci_rp_read_tx_power_level {
    uint8_t  status;
    uint16_t handle;
    int8_t   tx_power_level;
} __packed;

#define BT_HCI_LE_TX_POWER_PHY_1M               0x01
#define BT_HCI_LE_TX_POWER_PHY_2M               0x02
#define BT_HCI_LE_TX_POWER_PHY_CODED_S8         0x03
#define BT_HCI_LE_TX_POWER_PHY_CODED_S2         0x04
#define BT_HCI_OP_LE_ENH_READ_TX_POWER_LEVEL    BT_OP(BT_OGF_LE, 0x0076) /* 0x2076 */
struct bt_hci_cp_le_read_tx_power_level {
    uint16_t handle;
    uint8_t  phy;
} __packed;

struct bt_hci_rp_le_read_tx_power_level {
    uint8_t  status;
    uint16_t handle;
    uint8_t  phy;
    int8_t   current_tx_power_level;
    int8_t   max_tx_power_level;
} __packed;

#define BT_HCI_OP_LE_READ_REMOTE_TX_POWER_LEVEL BT_OP(BT_OGF_LE, 0x0077) /* 0x2077 */

#define BT_HCI_LE_TX_POWER_REPORT_DISABLE       0x00
#define BT_HCI_LE_TX_POWER_REPORT_ENABLE        0x01
#define BT_HCI_OP_LE_SET_TX_POWER_REPORT_ENABLE BT_OP(BT_OGF_LE, 0x007A) /* 0x207A */
struct bt_hci_cp_le_set_tx_power_report_enable {
    uint16_t handle;
    uint8_t  local_enable;
    uint8_t  remote_enable;
} __packed;

struct bt_hci_cp_le_set_path_loss_reporting_parameters {
    uint16_t handle;
    uint8_t  high_threshold;
    uint8_t  high_hysteresis;
    uint8_t  low_threshold;
    uint8_t  low_hysteresis;
    uint16_t min_time_spent;
} __packed;

struct bt_hci_cp_le_set_path_loss_reporting_enable {
    uint16_t handle;
    uint8_t  enable;
} __packed;

#define BT_HCI_OP_LE_SET_PATH_LOSS_REPORTING_PARAMETERS BT_OP(BT_OGF_LE, 0x0078) /* 0x2078 */

#define BT_HCI_LE_PATH_LOSS_REPORTING_DISABLE       0x00
#define BT_HCI_LE_PATH_LOSS_REPORTING_ENABLE        0x01
#define BT_HCI_OP_LE_SET_PATH_LOSS_REPORTING_ENABLE BT_OP(BT_OGF_LE, 0x0079) /* 0x2079 */

struct bt_hci_cp_le_set_default_subrate {
    uint16_t subrate_min;
    uint16_t subrate_max;
    uint16_t max_latency;
    uint16_t continuation_number;
    uint16_t supervision_timeout;
} __packed;

struct bt_hci_cp_le_subrate_request {
    uint16_t handle;
    uint16_t subrate_min;
    uint16_t subrate_max;
    uint16_t max_latency;
    uint16_t continuation_number;
    uint16_t supervision_timeout;
} __packed;

#define BT_HCI_OP_LE_SET_DEFAULT_SUBRATE BT_OP(BT_OGF_LE, 0x007D) /* 0x207D */
#define BT_HCI_OP_LE_SUBRATE_REQUEST     BT_OP(BT_OGF_LE, 0x007E) /* 0x207E */

#define BT_HCI_CTL_TO_HOST_FLOW_DISABLE         0x00
#define BT_HCI_CTL_TO_HOST_FLOW_ENABLE          0x01
#define BT_HCI_OP_SET_CTL_TO_HOST_FLOW          BT_OP(BT_OGF_BASEBAND, 0x0031) /* 0x0c31 */
struct bt_hci_cp_set_ctl_to_host_flow {
    uint8_t  flow_enable;
} __packed;

#define BT_HCI_OP_HOST_BUFFER_SIZE              BT_OP(BT_OGF_BASEBAND, 0x0033) /* 0x0c33 */
struct bt_hci_cp_host_buffer_size {
    uint16_t acl_mtu;
    uint8_t  sco_mtu;
    uint16_t acl_pkts;
    uint16_t sco_pkts;
} __packed;

struct bt_hci_handle_count {
    uint16_t handle;
    uint16_t count;
} __packed;

#define BT_HCI_OP_HOST_NUM_COMPLETED_PACKETS    BT_OP(BT_OGF_BASEBAND, 0x0035) /* 0x0c35 */
struct bt_hci_cp_host_num_completed_packets {
    uint8_t  num_handles;
    struct bt_hci_handle_count h[0];
} __packed;

#define BT_HCI_OP_WRITE_INQUIRY_MODE            BT_OP(BT_OGF_BASEBAND, 0x0045) /* 0x0c45 */
struct bt_hci_cp_write_inquiry_mode {
    uint8_t  mode;
} __packed;

#define BT_HCI_OP_WRITE_SSP_MODE                BT_OP(BT_OGF_BASEBAND, 0x0056) /* 0x0c56 */
struct bt_hci_cp_write_ssp_mode {
    uint8_t mode;
} __packed;

#define BT_HCI_OP_SET_EVENT_MASK_PAGE_2         BT_OP(BT_OGF_BASEBAND, 0x0063) /* 0x0c63 */
struct bt_hci_cp_set_event_mask_page_2 {
    uint8_t  events_page_2[8];
} __packed;

#define BT_HCI_OP_LE_WRITE_LE_HOST_SUPP         BT_OP(BT_OGF_BASEBAND, 0x006d) /* 0x0c6d */
struct bt_hci_cp_write_le_host_supp {
    uint8_t  le;
    uint8_t  simul;
} __packed;

#define BT_HCI_OP_WRITE_SC_HOST_SUPP            BT_OP(BT_OGF_BASEBAND, 0x007a) /* 0x0c7a */
struct bt_hci_cp_write_sc_host_supp {
    uint8_t  sc_support;
} __packed;

#define BT_HCI_OP_READ_AUTH_PAYLOAD_TIMEOUT     BT_OP(BT_OGF_BASEBAND, 0x007b) /* 0x0c7b */
struct bt_hci_cp_read_auth_payload_timeout {
    uint16_t handle;
} __packed;

struct bt_hci_rp_read_auth_payload_timeout {
    uint8_t  status;
    uint16_t handle;
    uint16_t auth_payload_timeout;
} __packed;

#define BT_HCI_OP_WRITE_AUTH_PAYLOAD_TIMEOUT    BT_OP(BT_OGF_BASEBAND, 0x007c) /* 0x0c7c */
struct bt_hci_cp_write_auth_payload_timeout {
    uint16_t handle;
    uint16_t auth_payload_timeout;
} __packed;

struct bt_hci_rp_write_auth_payload_timeout {
    uint8_t  status;
    uint16_t handle;
} __packed;

#define BT_HCI_OP_CONFIGURE_DATA_PATH           BT_OP(BT_OGF_BASEBAND, 0x0083) /* 0x0c83 */
struct bt_hci_cp_configure_data_path {
    uint8_t  data_path_dir;
    uint8_t  data_path_id;
    uint8_t  vs_config_len;
    uint8_t  vs_config[0];
} __packed;

struct bt_hci_rp_configure_data_path {
    uint8_t  status;
} __packed;

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
#define BT_HCI_VERSION_5_1                      10
#define BT_HCI_VERSION_5_2                      11
#define BT_HCI_VERSION_5_3                      12
#define BT_HCI_VERSION_5_4                      13
#define BT_HCI_VERSION_6_0                      14

#define BT_HCI_OP_READ_LOCAL_VERSION_INFO       BT_OP(BT_OGF_INFO, 0x0001) /* 0x1001 */
struct bt_hci_rp_read_local_version_info {
    uint8_t  status;
    uint8_t  hci_version;
    uint16_t hci_revision;
    uint8_t  lmp_version;
    uint16_t manufacturer;
    uint16_t lmp_subversion;
} __packed;

#define BT_HCI_OP_READ_SUPPORTED_COMMANDS       BT_OP(BT_OGF_INFO, 0x0002) /* 0x1002 */
struct bt_hci_rp_read_supported_commands {
    uint8_t  status;
    uint8_t  commands[64];
} __packed;

#define BT_HCI_OP_READ_LOCAL_EXT_FEATURES       BT_OP(BT_OGF_INFO, 0x0004) /* 0x1004 */
struct bt_hci_cp_read_local_ext_features {
    uint8_t page;
};
struct bt_hci_rp_read_local_ext_features {
    uint8_t  status;
    uint8_t  page;
    uint8_t  max_page;
    uint8_t  ext_features[8];
} __packed;

#define BT_HCI_OP_READ_LOCAL_FEATURES           BT_OP(BT_OGF_INFO, 0x0003) /* 0x1003 */
struct bt_hci_rp_read_local_features {
    uint8_t  status;
    uint8_t  features[8];
} __packed;

#define BT_HCI_OP_READ_BUFFER_SIZE              BT_OP(BT_OGF_INFO, 0x0005) /* 0x1005 */
struct bt_hci_rp_read_buffer_size {
    uint8_t  status;
    uint16_t acl_max_len;
    uint8_t  sco_max_len;
    uint16_t acl_max_num;
    uint16_t sco_max_num;
} __packed;

#define BT_HCI_OP_READ_BD_ADDR                  BT_OP(BT_OGF_INFO, 0x0009) /* 0x1009 */
struct bt_hci_rp_read_bd_addr {
    uint8_t   status;
    bt_addr_t bdaddr;
} __packed;

/* logic transport type bits as returned when reading supported codecs */
#define BT_HCI_CODEC_TRANSPORT_MASK_BREDR_ACL BIT(0)
#define BT_HCI_CODEC_TRANSPORT_MASK_BREDR_SCO BIT(1)
#define BT_HCI_CODEC_TRANSPORT_MASK_LE_CIS    BIT(2)
#define BT_HCI_CODEC_TRANSPORT_MASK_LE_BIS    BIT(3)

/* logic transport types for reading codec capabilities and controller delays */
#define BT_HCI_LOGICAL_TRANSPORT_TYPE_BREDR_ACL 0x00
#define BT_HCI_LOGICAL_TRANSPORT_TYPE_BREDR_SCO 0x01
#define BT_HCI_LOGICAL_TRANSPORT_TYPE_LE_CIS    0x02
#define BT_HCI_LOGICAL_TRANSPORT_TYPE_LE_BIS    0x03

/* audio datapath directions */
#define BT_HCI_DATAPATH_DIR_HOST_TO_CTLR 0x00
#define BT_HCI_DATAPATH_DIR_CTLR_TO_HOST 0x01

/* audio datapath IDs */
#define BT_HCI_DATAPATH_ID_HCI      0x00
#define BT_HCI_DATAPATH_ID_VS       0x01
#define BT_HCI_DATAPATH_ID_VS_END   0xfe

/* coding format assigned numbers, used for codec IDs */
#define BT_HCI_CODING_FORMAT_ULAW_LOG    0x00
#define BT_HCI_CODING_FORMAT_ALAW_LOG    0x01
#define BT_HCI_CODING_FORMAT_CVSD        0x02
#define BT_HCI_CODING_FORMAT_TRANSPARENT 0x03
#define BT_HCI_CODING_FORMAT_LINEAR_PCM  0x04
#define BT_HCI_CODING_FORMAT_MSBC        0x05
#define BT_HCI_CODING_FORMAT_LC3         0x06
#define BT_HCI_CODING_FORMAT_G729A       0x07
#define BT_HCI_CODING_FORMAT_VS          0xFF

#define BT_HCI_OP_READ_CODECS                   BT_OP(BT_OGF_INFO, 0x000b) /* 0x100b */
struct bt_hci_std_codec_info {
    uint8_t codec_id;
} __packed;
struct bt_hci_std_codecs {
    uint8_t num_codecs;
    struct bt_hci_std_codec_info codec_info[0];
} __packed;
struct bt_hci_vs_codec_info {
    uint16_t company_id;
    uint16_t codec_id;
} __packed;
struct bt_hci_vs_codecs {
    uint8_t num_codecs;
    struct bt_hci_vs_codec_info codec_info[0];
} __packed;
struct bt_hci_rp_read_codecs {
    uint8_t status;
    /* other fields filled in dynamically */
    uint8_t codecs[0];
} __packed;

#define BT_HCI_OP_READ_CODECS_V2                BT_OP(BT_OGF_INFO, 0x000d) /* 0x100d */
struct bt_hci_std_codec_info_v2 {
    uint8_t codec_id;
    uint8_t transports; /* bitmap */
} __packed;
struct bt_hci_std_codecs_v2 {
    uint8_t num_codecs;
    struct bt_hci_std_codec_info_v2 codec_info[0];
} __packed;
struct bt_hci_vs_codec_info_v2 {
    uint16_t company_id;
    uint16_t codec_id;
    uint8_t transports; /* bitmap */
} __packed;
struct bt_hci_vs_codecs_v2 {
    uint8_t num_codecs;
    struct bt_hci_vs_codec_info_v2 codec_info[0];
} __packed;
struct bt_hci_rp_read_codecs_v2 {
    uint8_t status;
    /* other fields filled in dynamically */
    uint8_t codecs[0];
} __packed;

struct bt_hci_cp_codec_id {
    uint8_t coding_format;
    uint16_t company_id;
    uint16_t vs_codec_id;
} __packed;

#define BT_HCI_OP_READ_CODEC_CAPABILITIES       BT_OP(BT_OGF_INFO, 0x000e) /* 0x100e */
struct bt_hci_cp_read_codec_capabilities {
    struct bt_hci_cp_codec_id codec_id;
    uint8_t transport;
    uint8_t direction;
} __packed;
struct bt_hci_codec_capability_info {
    uint8_t length;
    uint8_t data[0];
} __packed;
struct bt_hci_rp_read_codec_capabilities {
    uint8_t status;
    uint8_t num_capabilities;
    /* other fields filled in dynamically */
    uint8_t capabilities[0];
} __packed;

#define BT_HCI_OP_READ_CTLR_DELAY               BT_OP(BT_OGF_INFO, 0x000f) /* 0x100f */
struct bt_hci_cp_read_ctlr_delay {
    struct bt_hci_cp_codec_id codec_id;
    uint8_t transport;
    uint8_t direction;
    uint8_t codec_config_len;
    uint8_t codec_config[0];
} __packed;
struct bt_hci_rp_read_ctlr_delay {
    uint8_t status;
    uint8_t min_ctlr_delay[3];
    uint8_t max_ctlr_delay[3];
} __packed;

#define BT_HCI_OP_READ_RSSI                     BT_OP(BT_OGF_STATUS, 0x0005) /* 0x1405 */
struct bt_hci_cp_read_rssi {
    uint16_t handle;
} __packed;
struct bt_hci_rp_read_rssi {
    uint8_t  status;
    uint16_t handle;
    int8_t   rssi;
} __packed;

#define BT_HCI_ENCRYPTION_KEY_SIZE_MIN          7
#define BT_HCI_ENCRYPTION_KEY_SIZE_MAX          16

#define BT_HCI_OP_READ_ENCRYPTION_KEY_SIZE      BT_OP(BT_OGF_STATUS, 0x0008) /* 0x1408 */
struct bt_hci_cp_read_encryption_key_size {
    uint16_t handle;
} __packed;
struct bt_hci_rp_read_encryption_key_size {
    uint8_t  status;
    uint16_t handle;
    uint8_t  key_size;
} __packed;

/* BLE */

#define BT_HCI_OP_LE_SET_EVENT_MASK             BT_OP(BT_OGF_LE, 0x0001) /* 0x2001 */
struct bt_hci_cp_le_set_event_mask {
    uint8_t events[8];
} __packed;

#define BT_HCI_OP_LE_READ_BUFFER_SIZE           BT_OP(BT_OGF_LE, 0x0002) /* 0x2002 */
struct bt_hci_rp_le_read_buffer_size {
    uint8_t  status;
    uint16_t le_max_len;
    uint8_t  le_max_num;
} __packed;

#define BT_HCI_OP_LE_READ_LOCAL_FEATURES        BT_OP(BT_OGF_LE, 0x0003) /* 0x2003 */
struct bt_hci_rp_le_read_local_features {
    uint8_t  status;
    uint8_t  features[8];
} __packed;

#define BT_HCI_OP_LE_SET_RANDOM_ADDRESS         BT_OP(BT_OGF_LE, 0x0005) /* 0x2005 */
struct bt_hci_cp_le_set_random_address {
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_ADV_IND                          0x00
#define BT_HCI_ADV_DIRECT_IND                   0x01
#define BT_HCI_ADV_SCAN_IND                     0x02
#define BT_HCI_ADV_NONCONN_IND                  0x03
#define BT_HCI_ADV_DIRECT_IND_LOW_DUTY          0x04
#define BT_HCI_ADV_SCAN_RSP                     0x04

#define BT_LE_ADV_INTERVAL_MIN                  0x0020
#define BT_LE_ADV_INTERVAL_MAX                  0x4000
#define BT_LE_ADV_INTERVAL_DEFAULT              0x0800

#define BT_LE_ADV_CHAN_MAP_CHAN_37              0x01
#define BT_LE_ADV_CHAN_MAP_CHAN_38              0x02
#define BT_LE_ADV_CHAN_MAP_CHAN_39              0x04
#define BT_LE_ADV_CHAN_MAP_ALL                  0x07

#define BT_LE_ADV_FP_NO_FILTER                  0x00
#define BT_LE_ADV_FP_FILTER_SCAN_REQ            0x01
#define BT_LE_ADV_FP_FILTER_CONN_IND            0x02
#define BT_LE_ADV_FP_FILTER_BOTH                0x03

#define BT_HCI_OP_LE_SET_ADV_PARAM              BT_OP(BT_OGF_LE, 0x0006) /* 0x2006 */
struct bt_hci_cp_le_set_adv_param {
    uint16_t     min_interval;
    uint16_t     max_interval;
    uint8_t      type;
    uint8_t      own_addr_type;
    bt_addr_le_t direct_addr;
    uint8_t      channel_map;
    uint8_t      filter_policy;
} __packed;

#define BT_HCI_OP_LE_READ_ADV_CHAN_TX_POWER     BT_OP(BT_OGF_LE, 0x0007) /* 0x2007 */
struct bt_hci_rp_le_read_chan_tx_power {
    uint8_t status;
    int8_t  tx_power_level;
} __packed;

#define BT_HCI_OP_LE_SET_ADV_DATA               BT_OP(BT_OGF_LE, 0x0008) /* 0x2008 */
struct bt_hci_cp_le_set_adv_data {
    uint8_t  len;
    uint8_t  data[31];
} __packed;

#define BT_HCI_OP_LE_SET_SCAN_RSP_DATA          BT_OP(BT_OGF_LE, 0x0009) /* 0x2009 */
struct bt_hci_cp_le_set_scan_rsp_data {
    uint8_t  len;
    uint8_t  data[31];
} __packed;

#define BT_HCI_LE_ADV_DISABLE                   0x00
#define BT_HCI_LE_ADV_ENABLE                    0x01

#define BT_HCI_OP_LE_SET_ADV_ENABLE             BT_OP(BT_OGF_LE, 0x000a) /* 0x200a */
struct bt_hci_cp_le_set_adv_enable {
    uint8_t  enable;
} __packed;

/* Scan types */
#define BT_HCI_OP_LE_SET_SCAN_PARAM             BT_OP(BT_OGF_LE, 0x000b) /* 0x200b */
#define BT_HCI_LE_SCAN_PASSIVE                  0x00
#define BT_HCI_LE_SCAN_ACTIVE                   0x01

#define BT_HCI_LE_SCAN_FP_BASIC_NO_FILTER       0x00
#define BT_HCI_LE_SCAN_FP_BASIC_FILTER          0x01
#define BT_HCI_LE_SCAN_FP_EXT_NO_FILTER         0x02
#define BT_HCI_LE_SCAN_FP_EXT_FILTER            0x03

struct bt_hci_cp_le_set_scan_param {
    uint8_t  scan_type;
    uint16_t interval;
    uint16_t window;
    uint8_t  addr_type;
    uint8_t  filter_policy;
} __packed;

#define BT_HCI_OP_LE_SET_SCAN_ENABLE            BT_OP(BT_OGF_LE, 0x000c) /* 0x200c */

#define BT_HCI_LE_SCAN_DISABLE                  0x00
#define BT_HCI_LE_SCAN_ENABLE                   0x01

#define BT_HCI_LE_SCAN_FILTER_DUP_DISABLE       0x00
#define BT_HCI_LE_SCAN_FILTER_DUP_ENABLE        0x01

struct bt_hci_cp_le_set_scan_enable {
    uint8_t  enable;
    uint8_t  filter_dup;
} __packed;

#define BT_HCI_OP_LE_CREATE_CONN                BT_OP(BT_OGF_LE, 0x000d) /* 0x200d */

#define BT_HCI_LE_CREATE_CONN_FP_NO_FILTER      0x00
#define BT_HCI_LE_CREATE_CONN_FP_FILTER         0x01

struct bt_hci_cp_le_create_conn {
    uint16_t     scan_interval;
    uint16_t     scan_window;
    uint8_t      filter_policy;
    bt_addr_le_t peer_addr;
    uint8_t      own_addr_type;
    uint16_t     conn_interval_min;
    uint16_t     conn_interval_max;
    uint16_t     conn_latency;
    uint16_t     supervision_timeout;
    uint16_t     min_ce_len;
    uint16_t     max_ce_len;
} __packed;

#define BT_HCI_OP_LE_CREATE_CONN_CANCEL         BT_OP(BT_OGF_LE, 0x000e) /* 0x200e */

#define BT_HCI_OP_LE_READ_FAL_SIZE               BT_OP(BT_OGF_LE, 0x000f) /* 0x200f */
struct bt_hci_rp_le_read_fal_size {
    uint8_t  status;
    uint8_t  fal_size;
} __packed;

#define BT_HCI_OP_LE_CLEAR_FAL                   BT_OP(BT_OGF_LE, 0x0010) /* 0x2010 */

#define BT_HCI_OP_LE_ADD_DEV_TO_FAL              BT_OP(BT_OGF_LE, 0x0011) /* 0x2011 */
struct bt_hci_cp_le_add_dev_to_fal {
    bt_addr_le_t  addr;
} __packed;

#define BT_HCI_OP_LE_REM_DEV_FROM_FAL            BT_OP(BT_OGF_LE, 0x0012) /* 0x2012 */
struct bt_hci_cp_le_rem_dev_from_fal {
    bt_addr_le_t  addr;
} __packed;

#define BT_HCI_OP_LE_CONN_UPDATE                BT_OP(BT_OGF_LE, 0x0013) /* 0x2013 */
struct hci_cp_le_conn_update {
    uint16_t handle;
    uint16_t conn_interval_min;
    uint16_t conn_interval_max;
    uint16_t conn_latency;
    uint16_t supervision_timeout;
    uint16_t min_ce_len;
    uint16_t max_ce_len;
} __packed;

#define BT_HCI_OP_LE_SET_HOST_CHAN_CLASSIF      BT_OP(BT_OGF_LE, 0x0014) /* 0x2014 */
struct bt_hci_cp_le_set_host_chan_classif {
    uint8_t  ch_map[5];
} __packed;

#define BT_HCI_OP_LE_READ_CHAN_MAP              BT_OP(BT_OGF_LE, 0x0015) /* 0x2015 */
struct bt_hci_cp_le_read_chan_map {
    uint16_t handle;
} __packed;
struct bt_hci_rp_le_read_chan_map {
    uint8_t  status;
    uint16_t handle;
    uint8_t  ch_map[5];
} __packed;

#define BT_HCI_OP_LE_READ_REMOTE_FEATURES       BT_OP(BT_OGF_LE, 0x0016) /* 0x2016 */
struct bt_hci_cp_le_read_remote_features {
    uint16_t  handle;
} __packed;

#define BT_HCI_OP_LE_ENCRYPT                    BT_OP(BT_OGF_LE, 0x0017) /* 0x2017 */
struct bt_hci_cp_le_encrypt {
    uint8_t  key[16];
    uint8_t  plaintext[16];
} __packed;
struct bt_hci_rp_le_encrypt {
    uint8_t  status;
    uint8_t  enc_data[16];
} __packed;

#define BT_HCI_OP_LE_RAND                       BT_OP(BT_OGF_LE, 0x0018) /* 0x2018 */
struct bt_hci_rp_le_rand {
    uint8_t  status;
    uint8_t  rand[8];
} __packed;

#define BT_HCI_OP_LE_START_ENCRYPTION           BT_OP(BT_OGF_LE, 0x0019) /* 0x2019 */
struct bt_hci_cp_le_start_encryption {
    uint16_t handle;
    uint64_t rand;
    uint16_t ediv;
    uint8_t  ltk[16];
} __packed;

#define BT_HCI_OP_LE_LTK_REQ_REPLY              BT_OP(BT_OGF_LE, 0x001a) /* 0x201a */
struct bt_hci_cp_le_ltk_req_reply {
    uint16_t handle;
    uint8_t  ltk[16];
} __packed;
struct bt_hci_rp_le_ltk_req_reply {
    uint8_t  status;
    uint16_t handle;
} __packed;

#define BT_HCI_OP_LE_LTK_REQ_NEG_REPLY          BT_OP(BT_OGF_LE, 0x001b) /* 0x201b */
struct bt_hci_cp_le_ltk_req_neg_reply {
    uint16_t handle;
} __packed;
struct bt_hci_rp_le_ltk_req_neg_reply {
    uint8_t  status;
    uint16_t handle;
} __packed;

#define BT_HCI_OP_LE_READ_SUPP_STATES           BT_OP(BT_OGF_LE, 0x001c) /* 0x201c */
struct bt_hci_rp_le_read_supp_states {
    uint8_t  status;
    uint8_t  le_states[8];
} __packed;

#define BT_HCI_OP_LE_RX_TEST                    BT_OP(BT_OGF_LE, 0x001d) /* 0x201d */
struct bt_hci_cp_le_rx_test {
    uint8_t  rx_ch;
} __packed;

#define BT_HCI_TEST_PKT_PAYLOAD_PRBS9           0x00
#define BT_HCI_TEST_PKT_PAYLOAD_11110000        0x01
#define BT_HCI_TEST_PKT_PAYLOAD_10101010        0x02
#define BT_HCI_TEST_PKT_PAYLOAD_PRBS15          0x03
#define BT_HCI_TEST_PKT_PAYLOAD_11111111        0x04
#define BT_HCI_TEST_PKT_PAYLOAD_00000000        0x05
#define BT_HCI_TEST_PKT_PAYLOAD_00001111        0x06
#define BT_HCI_TEST_PKT_PAYLOAD_01010101        0x07

#define BT_HCI_OP_LE_TX_TEST                    BT_OP(BT_OGF_LE, 0x001e) /* 0x201e */
struct bt_hci_cp_le_tx_test {
    uint8_t  tx_ch;
    uint8_t  test_data_len;
    uint8_t  pkt_payload;
} __packed;

#define BT_HCI_OP_LE_TEST_END                   BT_OP(BT_OGF_LE, 0x001f) /* 0x201f */
struct bt_hci_rp_le_test_end {
    uint8_t  status;
    uint16_t rx_pkt_count;
} __packed;

#define BT_HCI_OP_LE_CONN_PARAM_REQ_REPLY       BT_OP(BT_OGF_LE, 0x0020) /* 0x2020 */
struct bt_hci_cp_le_conn_param_req_reply {
    uint16_t handle;
    uint16_t interval_min;
    uint16_t interval_max;
    uint16_t latency;
    uint16_t timeout;
    uint16_t min_ce_len;
    uint16_t max_ce_len;
} __packed;
struct bt_hci_rp_le_conn_param_req_reply {
    uint8_t  status;
    uint16_t handle;
} __packed;

#define BT_HCI_OP_LE_CONN_PARAM_REQ_NEG_REPLY   BT_OP(BT_OGF_LE, 0x0021) /* 0x2021 */
struct bt_hci_cp_le_conn_param_req_neg_reply {
    uint16_t handle;
    uint8_t  reason;
} __packed;
struct bt_hci_rp_le_conn_param_req_neg_reply {
    uint8_t  status;
    uint16_t handle;
} __packed;

#define BT_HCI_OP_LE_SET_DATA_LEN               BT_OP(BT_OGF_LE, 0x0022) /* 0x2022 */
struct bt_hci_cp_le_set_data_len {
    uint16_t handle;
    uint16_t tx_octets;
    uint16_t tx_time;
} __packed;
struct bt_hci_rp_le_set_data_len {
    uint8_t  status;
    uint16_t handle;
} __packed;

#define BT_HCI_OP_LE_READ_DEFAULT_DATA_LEN      BT_OP(BT_OGF_LE, 0x0023) /* 0x2023 */
struct bt_hci_rp_le_read_default_data_len {
    uint8_t  status;
    uint16_t max_tx_octets;
    uint16_t max_tx_time;
} __packed;

#define BT_HCI_OP_LE_WRITE_DEFAULT_DATA_LEN     BT_OP(BT_OGF_LE, 0x0024) /* 0x2024 */
struct bt_hci_cp_le_write_default_data_len {
    uint16_t max_tx_octets;
    uint16_t max_tx_time;
} __packed;

#define BT_HCI_OP_LE_P256_PUBLIC_KEY            BT_OP(BT_OGF_LE, 0x0025) /* 0x2025 */

#define BT_HCI_OP_LE_GENERATE_DHKEY             BT_OP(BT_OGF_LE, 0x0026) /* 0x2026 */
struct bt_hci_cp_le_generate_dhkey {
    uint8_t key[64];
} __packed;

#define BT_HCI_OP_LE_GENERATE_DHKEY_V2          BT_OP(BT_OGF_LE, 0x005e) /* 0x205e */

#define BT_HCI_LE_KEY_TYPE_GENERATED            0x00
#define BT_HCI_LE_KEY_TYPE_DEBUG                0x01

struct bt_hci_cp_le_generate_dhkey_v2 {
    uint8_t key[64];
    uint8_t key_type;
} __packed;

#define BT_HCI_OP_LE_ADD_DEV_TO_RL              BT_OP(BT_OGF_LE, 0x0027) /* 0x2027 */
struct bt_hci_cp_le_add_dev_to_rl {
    bt_addr_le_t  peer_id_addr;
    uint8_t       peer_irk[16];
    uint8_t       local_irk[16];
} __packed;

#define BT_HCI_OP_LE_REM_DEV_FROM_RL            BT_OP(BT_OGF_LE, 0x0028) /* 0x2028 */
struct bt_hci_cp_le_rem_dev_from_rl {
    bt_addr_le_t  peer_id_addr;
} __packed;

#define BT_HCI_OP_LE_CLEAR_RL                   BT_OP(BT_OGF_LE, 0x0029) /* 0x2029 */

#define BT_HCI_OP_LE_READ_RL_SIZE               BT_OP(BT_OGF_LE, 0x002a) /* 0x202a */
struct bt_hci_rp_le_read_rl_size {
    uint8_t  status;
    uint8_t  rl_size;
} __packed;

#define BT_HCI_OP_LE_READ_PEER_RPA              BT_OP(BT_OGF_LE, 0x002b) /* 0x202b */
struct bt_hci_cp_le_read_peer_rpa {
    bt_addr_le_t  peer_id_addr;
} __packed;
struct bt_hci_rp_le_read_peer_rpa {
    uint8_t    status;
    bt_addr_t  peer_rpa;
} __packed;

#define BT_HCI_OP_LE_READ_LOCAL_RPA             BT_OP(BT_OGF_LE, 0x002c) /* 0x202c */
struct bt_hci_cp_le_read_local_rpa {
    bt_addr_le_t  peer_id_addr;
} __packed;
struct bt_hci_rp_le_read_local_rpa {
    uint8_t    status;
    bt_addr_t  local_rpa;
} __packed;

#define BT_HCI_ADDR_RES_DISABLE                 0x00
#define BT_HCI_ADDR_RES_ENABLE                  0x01

#define BT_HCI_OP_LE_SET_ADDR_RES_ENABLE        BT_OP(BT_OGF_LE, 0x002d) /* 0x202d */
struct bt_hci_cp_le_set_addr_res_enable {
    uint8_t  enable;
} __packed;

#define BT_HCI_OP_LE_SET_RPA_TIMEOUT            BT_OP(BT_OGF_LE, 0x002e) /* 0x202e */
struct bt_hci_cp_le_set_rpa_timeout {
    uint16_t rpa_timeout;
} __packed;

/* All limits according to BT Core spec 5.4 [Vol 4, Part E, 7.8.46] */
#define BT_HCI_LE_MAX_TX_OCTETS_MIN             0x001B
#define BT_HCI_LE_MAX_TX_OCTETS_MAX             0x00FB
#define BT_HCI_LE_MAX_RX_OCTETS_MIN             0x001B
#define BT_HCI_LE_MAX_RX_OCTETS_MAX             0x00FB

#define BT_HCI_LE_MAX_TX_TIME_MIN               0x0148
#define BT_HCI_LE_MAX_TX_TIME_MAX               0x4290
#define BT_HCI_LE_MAX_RX_TIME_MIN               0x0148
#define BT_HCI_LE_MAX_RX_TIME_MAX               0x4290

#define BT_HCI_OP_LE_READ_MAX_DATA_LEN          BT_OP(BT_OGF_LE, 0x002f) /* 0x202f */
struct bt_hci_rp_le_read_max_data_len {
    uint8_t  status;
    uint16_t max_tx_octets;
    uint16_t max_tx_time;
    uint16_t max_rx_octets;
    uint16_t max_rx_time;
} __packed;

#define BT_HCI_LE_PHY_1M                        0x01
#define BT_HCI_LE_PHY_2M                        0x02
#define BT_HCI_LE_PHY_CODED                     0x03

#define BT_HCI_OP_LE_READ_PHY                   BT_OP(BT_OGF_LE, 0x0030) /* 0x2030 */
struct bt_hci_cp_le_read_phy {
    uint16_t handle;
} __packed;
struct bt_hci_rp_le_read_phy {
    uint8_t  status;
    uint16_t handle;
    uint8_t  tx_phy;
    uint8_t  rx_phy;
} __packed;

#define BT_HCI_LE_PHY_TX_ANY                    BIT(0)
#define BT_HCI_LE_PHY_RX_ANY                    BIT(1)

#define BT_HCI_LE_PHY_PREFER_1M                 BIT(0)
#define BT_HCI_LE_PHY_PREFER_2M                 BIT(1)
#define BT_HCI_LE_PHY_PREFER_CODED              BIT(2)

#define BT_HCI_OP_LE_SET_DEFAULT_PHY            BT_OP(BT_OGF_LE, 0x0031) /* 0x2031 */
struct bt_hci_cp_le_set_default_phy {
    uint8_t all_phys;
    uint8_t tx_phys;
    uint8_t rx_phys;
} __packed;

#define BT_HCI_LE_PHY_CODED_ANY                 0x00
#define BT_HCI_LE_PHY_CODED_S2                  0x01
#define BT_HCI_LE_PHY_CODED_S8                  0x02

#define BT_HCI_OP_LE_SET_PHY                    BT_OP(BT_OGF_LE, 0x0032) /* 0x2032 */
struct bt_hci_cp_le_set_phy {
    uint16_t  handle;
    uint8_t   all_phys;
    uint8_t   tx_phys;
    uint8_t   rx_phys;
    uint16_t  phy_opts;
} __packed;

#define BT_HCI_LE_MOD_INDEX_STANDARD            0x00
#define BT_HCI_LE_MOD_INDEX_STABLE              0x01

#define BT_HCI_LE_RX_PHY_1M                     0x01
#define BT_HCI_LE_RX_PHY_2M                     0x02
#define BT_HCI_LE_RX_PHY_CODED                  0x03

#define BT_HCI_OP_LE_ENH_RX_TEST                BT_OP(BT_OGF_LE, 0x0033) /* 0x2033 */
struct bt_hci_cp_le_enh_rx_test {
    uint8_t  rx_ch;
    uint8_t  phy;
    uint8_t  mod_index;
} __packed;

#define BT_HCI_LE_TX_PHY_1M                     0x01
#define BT_HCI_LE_TX_PHY_2M                     0x02
#define BT_HCI_LE_TX_PHY_CODED_S8               0x03
#define BT_HCI_LE_TX_PHY_CODED_S2               0x04

#define BT_HCI_OP_LE_ENH_TX_TEST                BT_OP(BT_OGF_LE, 0x0034) /* 0x2034 */
struct bt_hci_cp_le_enh_tx_test {
    uint8_t  tx_ch;
    uint8_t  test_data_len;
    uint8_t  pkt_payload;
    uint8_t  phy;
} __packed;

#define BT_HCI_OP_LE_SET_ADV_SET_RANDOM_ADDR    BT_OP(BT_OGF_LE, 0x0035) /* 0x2035 */
struct bt_hci_cp_le_set_adv_set_random_addr {
    uint8_t   handle;
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_LE_ADV_PROP_CONN                 BIT(0)
#define BT_HCI_LE_ADV_PROP_SCAN                 BIT(1)
#define BT_HCI_LE_ADV_PROP_DIRECT               BIT(2)
#define BT_HCI_LE_ADV_PROP_HI_DC_CONN           BIT(3)
#define BT_HCI_LE_ADV_PROP_LEGACY               BIT(4)
#define BT_HCI_LE_ADV_PROP_ANON                 BIT(5)
#define BT_HCI_LE_ADV_PROP_TX_POWER             BIT(6)

#define BT_HCI_LE_PRIM_ADV_INTERVAL_MIN         0x000020
#define BT_HCI_LE_PRIM_ADV_INTERVAL_MAX         0xFFFFFF

#define BT_HCI_LE_ADV_SCAN_REQ_ENABLE  1
#define BT_HCI_LE_ADV_SCAN_REQ_DISABLE 0

#define BT_HCI_LE_ADV_TX_POWER_NO_PREF 0x7F

#define BT_HCI_LE_ADV_HANDLE_MAX       0xEF

#define BT_HCI_LE_EXT_ADV_SID_INVALID  0xFF

#define BT_HCI_OP_LE_SET_EXT_ADV_PARAM          BT_OP(BT_OGF_LE, 0x0036) /* 0x2036 */
struct bt_hci_cp_le_set_ext_adv_param {
    uint8_t      handle;
    uint16_t     props;
    uint8_t      prim_min_interval[3];
    uint8_t      prim_max_interval[3];
    uint8_t      prim_channel_map;
    uint8_t      own_addr_type;
    bt_addr_le_t peer_addr;
    uint8_t      filter_policy;
    int8_t       tx_power;
    uint8_t      prim_adv_phy;
    uint8_t      sec_adv_max_skip;
    uint8_t      sec_adv_phy;
    uint8_t      sid;
    uint8_t      scan_req_notify_enable;
} __packed;
struct bt_hci_rp_le_set_ext_adv_param {
    uint8_t status;
    int8_t  tx_power;
} __packed;

#define BT_HCI_LE_EXT_ADV_OP_INTERM_FRAG        0x00
#define BT_HCI_LE_EXT_ADV_OP_FIRST_FRAG         0x01
#define BT_HCI_LE_EXT_ADV_OP_LAST_FRAG          0x02
#define BT_HCI_LE_EXT_ADV_OP_COMPLETE_DATA      0x03
#define BT_HCI_LE_EXT_ADV_OP_UNCHANGED_DATA     0x04

#define BT_HCI_LE_EXT_ADV_FRAG_ENABLED          0x00
#define BT_HCI_LE_EXT_ADV_FRAG_DISABLED         0x01

#define BT_HCI_LE_EXT_ADV_FRAG_MAX_LEN          251

#define BT_HCI_OP_LE_SET_EXT_ADV_DATA           BT_OP(BT_OGF_LE, 0x0037) /* 0x2037 */
struct bt_hci_cp_le_set_ext_adv_data {
    uint8_t  handle;
    uint8_t  op;
    uint8_t  frag_pref;
    uint8_t  len;
    uint8_t  data[0];
} __packed;

#define BT_HCI_OP_LE_SET_EXT_SCAN_RSP_DATA      BT_OP(BT_OGF_LE, 0x0038) /* 0x2038 */
struct bt_hci_cp_le_set_ext_scan_rsp_data {
    uint8_t  handle;
    uint8_t  op;
    uint8_t  frag_pref;
    uint8_t  len;
    uint8_t  data[0];
} __packed;

#define BT_HCI_OP_LE_SET_EXT_ADV_ENABLE         BT_OP(BT_OGF_LE, 0x0039) /* 0x2039 */
struct bt_hci_ext_adv_set {
    uint8_t  handle;
    uint16_t duration;
    uint8_t  max_ext_adv_evts;
} __packed;

struct bt_hci_cp_le_set_ext_adv_enable {
    uint8_t  enable;
    uint8_t  set_num;
    struct bt_hci_ext_adv_set s[0];
} __packed;

#define BT_HCI_OP_LE_READ_MAX_ADV_DATA_LEN      BT_OP(BT_OGF_LE, 0x003a) /* 0x203a */
struct bt_hci_rp_le_read_max_adv_data_len {
    uint8_t  status;
    uint16_t max_adv_data_len;
} __packed;

#define BT_HCI_OP_LE_READ_NUM_ADV_SETS          BT_OP(BT_OGF_LE, 0x003b) /* 0x203b */
struct bt_hci_rp_le_read_num_adv_sets {
    uint8_t  status;
    uint8_t  num_sets;
} __packed;

#define BT_HCI_OP_LE_REMOVE_ADV_SET             BT_OP(BT_OGF_LE, 0x003c) /* 0x203c */
struct bt_hci_cp_le_remove_adv_set {
    uint8_t  handle;
} __packed;

#define BT_HCI_OP_CLEAR_ADV_SETS                BT_OP(BT_OGF_LE, 0x003d) /* 0x203d */

#define BT_HCI_LE_PER_ADV_INTERVAL_MIN          0x0006
#define BT_HCI_LE_PER_ADV_INTERVAL_MAX          0xFFFF

#define BT_HCI_OP_LE_SET_PER_ADV_PARAM          BT_OP(BT_OGF_LE, 0x003e) /* 0x203e */
struct bt_hci_cp_le_set_per_adv_param {
    uint8_t  handle;
    uint16_t min_interval;
    uint16_t max_interval;
    uint16_t props;
} __packed;

#define BT_HCI_LE_PER_ADV_OP_INTERM_FRAG        0x00
#define BT_HCI_LE_PER_ADV_OP_FIRST_FRAG         0x01
#define BT_HCI_LE_PER_ADV_OP_LAST_FRAG          0x02
#define BT_HCI_LE_PER_ADV_OP_COMPLETE_DATA      0x03

#define BT_HCI_LE_PER_ADV_FRAG_MAX_LEN          252

#define BT_HCI_OP_LE_SET_PER_ADV_DATA           BT_OP(BT_OGF_LE, 0x003f) /* 0x203f */
struct bt_hci_cp_le_set_per_adv_data {
    uint8_t  handle;
    uint8_t  op;
    uint8_t  len;
    uint8_t  data[0];
} __packed;

#define BT_HCI_LE_SET_PER_ADV_ENABLE_ENABLE     BIT(0)
#define BT_HCI_LE_SET_PER_ADV_ENABLE_ADI        BIT(1)

#define BT_HCI_OP_LE_SET_PER_ADV_ENABLE         BT_OP(BT_OGF_LE, 0x0040) /* 0x2040 */
struct bt_hci_cp_le_set_per_adv_enable {
    uint8_t  enable;
    uint8_t  handle;
} __packed;

#define BT_HCI_OP_LE_SET_EXT_SCAN_PARAM         BT_OP(BT_OGF_LE, 0x0041) /* 0x2041 */
struct bt_hci_ext_scan_phy {
    uint8_t  type;
    uint16_t interval;
    uint16_t window;
} __packed;

#define BT_HCI_LE_EXT_SCAN_PHY_1M               BIT(0)
#define BT_HCI_LE_EXT_SCAN_PHY_2M               BIT(1)
#define BT_HCI_LE_EXT_SCAN_PHY_CODED            BIT(2)

struct bt_hci_cp_le_set_ext_scan_param {
    uint8_t  own_addr_type;
    uint8_t  filter_policy;
    uint8_t  phys;
    struct bt_hci_ext_scan_phy p[0];
} __packed;

/* Extends BT_HCI_LE_SCAN_FILTER_DUP */
#define BT_HCI_LE_EXT_SCAN_FILTER_DUP_ENABLE_RESET  0x02

#define BT_HCI_OP_LE_SET_EXT_SCAN_ENABLE        BT_OP(BT_OGF_LE, 0x0042) /* 0x2042 */
struct bt_hci_cp_le_set_ext_scan_enable {
    uint8_t  enable;
    uint8_t  filter_dup;
    uint16_t duration;
    uint16_t period;
} __packed;

#define BT_HCI_OP_LE_EXT_CREATE_CONN            BT_OP(BT_OGF_LE, 0x0043) /* 0x2043 */
#define BT_HCI_OP_LE_EXT_CREATE_CONN_V2         BT_OP(BT_OGF_LE, 0x0085) /* 0x2085 */
struct bt_hci_ext_conn_phy {
    uint16_t scan_interval;
    uint16_t scan_window;
    uint16_t conn_interval_min;
    uint16_t conn_interval_max;
    uint16_t conn_latency;
    uint16_t supervision_timeout;
    uint16_t min_ce_len;
    uint16_t max_ce_len;
} __packed;

struct bt_hci_cp_le_ext_create_conn {
    uint8_t      filter_policy;
    uint8_t      own_addr_type;
    bt_addr_le_t peer_addr;
    uint8_t      phys;
    struct bt_hci_ext_conn_phy p[0];
} __packed;

struct bt_hci_cp_le_ext_create_conn_v2 {
    uint8_t      adv_handle;
    uint8_t      subevent;
    uint8_t      filter_policy;
    uint8_t      own_addr_type;
    bt_addr_le_t peer_addr;
    uint8_t      phys;
    struct bt_hci_ext_conn_phy p[0];
} __packed;

#define BT_HCI_OP_LE_SET_PER_ADV_SUBEVENT_DATA  BT_OP(BT_OGF_LE, 0x0082) /* 0x2082 */
struct bt_hci_cp_le_set_pawr_subevent_data_element {
    uint8_t subevent;
    uint8_t response_slot_start;
    uint8_t response_slot_count;
    uint8_t subevent_data_length;
    uint8_t subevent_data[0];
} __packed;

struct bt_hci_cp_le_set_pawr_subevent_data {
    uint8_t adv_handle;
    uint8_t num_subevents;
    struct bt_hci_cp_le_set_pawr_subevent_data_element subevents[0];
} __packed;

#define BT_HCI_OP_LE_SET_PER_ADV_RESPONSE_DATA  BT_OP(BT_OGF_LE, 0x0083) /* 0x2083 */
struct bt_hci_cp_le_set_pawr_response_data {
    uint16_t sync_handle;
    uint16_t request_event;
    uint8_t request_subevent;
    uint8_t response_subevent;
    uint8_t response_slot;
    uint8_t response_data_length;
    uint8_t response_data[0];
} __packed;

#define BT_HCI_OP_LE_SET_PER_ADV_SYNC_SUBEVENT  BT_OP(BT_OGF_LE, 0x0084) /* 0x2084 */
struct bt_hci_cp_le_set_pawr_sync_subevent {
    uint16_t sync_handle;
    uint16_t periodic_adv_properties;
    uint8_t num_subevents;
    uint8_t subevents[0];
} __packed;

#define BT_HCI_OP_LE_SET_PER_ADV_PARAM_V2       BT_OP(BT_OGF_LE, 0x0086) /* 0x2086 */
struct bt_hci_cp_le_set_per_adv_param_v2 {
    uint8_t  handle;
    uint16_t min_interval;
    uint16_t max_interval;
    uint16_t props;
    uint8_t num_subevents;
    uint8_t subevent_interval;
    uint8_t response_slot_delay;
    uint8_t response_slot_spacing;
    uint8_t num_response_slots;
} __packed;

#define BT_HCI_LE_PER_ADV_CREATE_SYNC_FP_USE_LIST               BIT(0)
#define BT_HCI_LE_PER_ADV_CREATE_SYNC_FP_REPORTS_DISABLED       BIT(1)
#define BT_HCI_LE_PER_ADV_CREATE_SYNC_FP_FILTER_DUPLICATE       BIT(2)

#define BT_HCI_LE_PER_ADV_CREATE_SYNC_CTE_TYPE_NO_FILTERING     0
#define BT_HCI_LE_PER_ADV_CREATE_SYNC_CTE_TYPE_NO_AOA           BIT(0)
#define BT_HCI_LE_PER_ADV_CREATE_SYNC_CTE_TYPE_NO_AOD_1US       BIT(1)
#define BT_HCI_LE_PER_ADV_CREATE_SYNC_CTE_TYPE_NO_AOD_2US       BIT(2)
#define BT_HCI_LE_PER_ADV_CREATE_SYNC_CTE_TYPE_NO_CTE           BIT(3)
#define BT_HCI_LE_PER_ADV_CREATE_SYNC_CTE_TYPE_ONLY_CTE         BIT(4)
/* Constants to check correctness of CTE type */
#define BT_HCI_LE_PER_ADV_CREATE_SYNC_CTE_TYPE_ALLOWED_BITS 5
#define BT_HCI_LE_PER_ADV_CREATE_SYNC_CTE_TYPE_INVALID_VALUE \
    (~BIT_MASK(BT_HCI_LE_PER_ADV_CREATE_SYNC_CTE_TYPE_ALLOWED_BITS))

#define BT_HCI_OP_LE_PER_ADV_CREATE_SYNC        BT_OP(BT_OGF_LE, 0x0044) /* 0x2044 */
struct bt_hci_cp_le_per_adv_create_sync {
    uint8_t options;
    uint8_t      sid;
    bt_addr_le_t addr;
    uint16_t     skip;
    uint16_t     sync_timeout;
    uint8_t      cte_type;
} __packed;

#define BT_HCI_OP_LE_PER_ADV_CREATE_SYNC_CANCEL BT_OP(BT_OGF_LE, 0x0045) /* 0x2045 */

#define BT_HCI_OP_LE_PER_ADV_TERMINATE_SYNC     BT_OP(BT_OGF_LE, 0x0046) /* 0x2046 */
struct bt_hci_cp_le_per_adv_terminate_sync {
    uint16_t handle;
} __packed;

#define BT_HCI_OP_LE_ADD_DEV_TO_PER_ADV_LIST    BT_OP(BT_OGF_LE, 0x0047) /* 0x2047 */
struct bt_hci_cp_le_add_dev_to_per_adv_list {
    bt_addr_le_t addr;
    uint8_t      sid;
} __packed;

#define BT_HCI_OP_LE_REM_DEV_FROM_PER_ADV_LIST  BT_OP(BT_OGF_LE, 0x0048) /* 0x2048 */
struct bt_hci_cp_le_rem_dev_from_per_adv_list {
    bt_addr_le_t addr;
    uint8_t      sid;
} __packed;

#define BT_HCI_OP_LE_CLEAR_PER_ADV_LIST         BT_OP(BT_OGF_LE, 0x0049) /* 0x2049 */

#define BT_HCI_OP_LE_READ_PER_ADV_LIST_SIZE     BT_OP(BT_OGF_LE, 0x004a) /* 0x204a */
struct bt_hci_rp_le_read_per_adv_list_size {
    uint8_t  status;
    uint8_t  list_size;
} __packed;

#define BT_HCI_OP_LE_READ_TX_POWER              BT_OP(BT_OGF_LE, 0x004b) /* 0x204b */
struct bt_hci_rp_le_read_tx_power {
    uint8_t status;
    int8_t  min_tx_power;
    int8_t  max_tx_power;
} __packed;

#define BT_HCI_OP_LE_READ_RF_PATH_COMP          BT_OP(BT_OGF_LE, 0x004c) /* 0x204c */
struct bt_hci_rp_le_read_rf_path_comp {
    uint8_t status;
    int16_t tx_path_comp;
    int16_t rx_path_comp;
} __packed;

#define BT_HCI_OP_LE_WRITE_RF_PATH_COMP         BT_OP(BT_OGF_LE, 0x004d) /* 0x204d */
struct bt_hci_cp_le_write_rf_path_comp {
    int16_t  tx_path_comp;
    int16_t  rx_path_comp;
} __packed;

#define BT_HCI_LE_PRIVACY_MODE_NETWORK          0x00
#define BT_HCI_LE_PRIVACY_MODE_DEVICE           0x01

#define BT_HCI_OP_LE_SET_PRIVACY_MODE           BT_OP(BT_OGF_LE, 0x004e) /* 0x204e */
struct bt_hci_cp_le_set_privacy_mode {
    bt_addr_le_t id_addr;
    uint8_t         mode;
} __packed;

#define BT_HCI_LE_TEST_CTE_DISABLED             0x00
#define BT_HCI_LE_TEST_CTE_TYPE_ANY             0x00
#define BT_HCI_LE_TEST_SLOT_DURATION_ANY        0x00
#define BT_HCI_LE_TEST_SWITCH_PATTERN_LEN_ANY   0x00

#define BT_HCI_OP_LE_RX_TEST_V3                 BT_OP(BT_OGF_LE, 0x004f) /* 0x204f */
struct bt_hci_cp_le_rx_test_v3 {
    uint8_t  rx_ch;
    uint8_t  phy;
    uint8_t  mod_index;
    uint8_t  expected_cte_len;
    uint8_t  expected_cte_type;
    uint8_t  slot_durations;
    uint8_t  switch_pattern_len;
    uint8_t  ant_ids[0];
} __packed;

#define BT_HCI_OP_LE_TX_TEST_V3                 BT_OP(BT_OGF_LE, 0x0050) /* 0x2050 */

struct bt_hci_cp_le_tx_test_v3 {
    uint8_t  tx_ch;
    uint8_t  test_data_len;
    uint8_t  pkt_payload;
    uint8_t  phy;
    uint8_t  cte_len;
    uint8_t  cte_type;
    uint8_t  switch_pattern_len;
    uint8_t  ant_ids[0];
} __packed;

/* Min and max Constant Tone Extension length in 8us units */
#define BT_HCI_LE_CTE_LEN_MIN                  0x2
#define BT_HCI_LE_CTE_LEN_MAX                  0x14

#define BT_HCI_LE_AOA_CTE                      0x0
#define BT_HCI_LE_AOD_CTE_1US                  0x1
#define BT_HCI_LE_AOD_CTE_2US                  0x2
#define BT_HCI_LE_NO_CTE                       0xFF

#define BT_HCI_LE_CTE_COUNT_MIN                0x1
#define BT_HCI_LE_CTE_COUNT_MAX                0x10

#define BT_HCI_OP_LE_SET_CL_CTE_TX_PARAMS      BT_OP(BT_OGF_LE, 0x0051) /* 0x2051 */
struct bt_hci_cp_le_set_cl_cte_tx_params {
    uint8_t handle;
    uint8_t cte_len;
    uint8_t cte_type;
    uint8_t cte_count;
    uint8_t switch_pattern_len;
    uint8_t ant_ids[0];
} __packed;

#define BT_HCI_OP_LE_SET_CL_CTE_TX_ENABLE      BT_OP(BT_OGF_LE, 0x0052) /* 0x2052 */
struct bt_hci_cp_le_set_cl_cte_tx_enable {
    uint8_t handle;
    uint8_t cte_enable;
} __packed;

#define BT_HCI_LE_ANTENNA_SWITCHING_SLOT_1US   0x1
#define BT_HCI_LE_ANTENNA_SWITCHING_SLOT_2US   0x2

#define BT_HCI_LE_SAMPLE_CTE_ALL               0x0
#define BT_HCI_LE_SAMPLE_CTE_COUNT_MIN         0x1
#define BT_HCI_LE_SAMPLE_CTE_COUNT_MAX         0x10

#define BT_HCI_OP_LE_SET_CL_CTE_SAMPLING_ENABLE BT_OP(BT_OGF_LE, 0x0053) /* 0x2053 */
struct bt_hci_cp_le_set_cl_cte_sampling_enable {
    uint16_t sync_handle;
    uint8_t  sampling_enable;
    uint8_t  slot_durations;
    uint8_t  max_sampled_cte;
    uint8_t  switch_pattern_len;
    uint8_t  ant_ids[0];
} __packed;

struct bt_hci_rp_le_set_cl_cte_sampling_enable {
    uint8_t  status;
    uint16_t sync_handle;
} __packed;

#define BT_HCI_OP_LE_SET_CONN_CTE_RX_PARAMS BT_OP(BT_OGF_LE, 0x0054) /* 0x2054 */
struct bt_hci_cp_le_set_conn_cte_rx_params {
    uint16_t handle;
    uint8_t  sampling_enable;
    uint8_t  slot_durations;
    uint8_t  switch_pattern_len;
    uint8_t  ant_ids[0];
} __packed;

struct bt_hci_rp_le_set_conn_cte_rx_params {
    uint8_t  status;
    uint16_t handle;
} __packed;

#define BT_HCI_LE_AOA_CTE_RSP                   BIT(0)
#define BT_HCI_LE_AOD_CTE_RSP_1US               BIT(1)
#define BT_HCI_LE_AOD_CTE_RSP_2US               BIT(2)

#define BT_HCI_LE_SWITCH_PATTERN_LEN_MIN        0x2
#define BT_HCI_LE_SWITCH_PATTERN_LEN_MAX        0x4B

#define BT_HCI_OP_LE_SET_CONN_CTE_TX_PARAMS     BT_OP(BT_OGF_LE, 0x0055) /* 0x2055 */
struct bt_hci_cp_le_set_conn_cte_tx_params {
    uint16_t handle;
    uint8_t  cte_types;
    uint8_t  switch_pattern_len;
    uint8_t  ant_ids[0];
} __packed;

struct bt_hci_rp_le_set_conn_cte_tx_params {
    uint8_t  status;
    uint16_t handle;
} __packed;

/* Interval between consecutive CTE request procedure starts in number of connection events. */
#define BT_HCI_REQUEST_CTE_ONCE                0x0
#define BT_HCI_REQUEST_CTE_INTERVAL_MIN        0x1
#define BT_HCI_REQUEST_CTE_INTERVAL_MAX        0xFFFF

#define BT_HCI_OP_LE_CONN_CTE_REQ_ENABLE       BT_OP(BT_OGF_LE, 0x0056) /* 0x2056 */
struct bt_hci_cp_le_conn_cte_req_enable {
    uint16_t handle;
    uint8_t  enable;
    uint16_t cte_request_interval;
    uint8_t  requested_cte_length;
    uint8_t  requested_cte_type;
} __packed;

struct bt_hci_rp_le_conn_cte_req_enable {
    uint8_t  status;
    uint16_t handle;
} __packed;

#define BT_HCI_OP_LE_CONN_CTE_RSP_ENABLE       BT_OP(BT_OGF_LE, 0x0057) /* 0x2057 */
struct bt_hci_cp_le_conn_cte_rsp_enable {
    uint16_t handle;
    uint8_t  enable;
} __packed;

struct bt_hci_rp_le_conn_cte_rsp_enable {
    uint8_t  status;
    uint16_t handle;
} __packed;

#define BT_HCI_LE_1US_AOD_TX                    BIT(0)
#define BT_HCI_LE_1US_AOD_RX                    BIT(1)
#define BT_HCI_LE_1US_AOA_RX                    BIT(2)

#define BT_HCI_LE_NUM_ANT_MIN                   0x1
#define BT_HCI_LE_NUM_ANT_MAX                   0x4B

#define BT_HCI_LE_MAX_SWITCH_PATTERN_LEN_MIN    0x2
#define BT_HCI_LE_MAX_SWITCH_PATTERN_LEN_MAX    0x4B

#define BT_HCI_LE_MAX_CTE_LEN_MIN               0x2
#define BT_HCI_LE_MAX_CTE_LEN_MAX               0x14

#define BT_HCI_OP_LE_READ_ANT_INFO              BT_OP(BT_OGF_LE, 0x0058) /* 0x2058 */
struct bt_hci_rp_le_read_ant_info {
    uint8_t status;
    uint8_t switch_sample_rates;
    uint8_t num_ant;
    uint8_t max_switch_pattern_len;
    uint8_t max_cte_len;
};

#define BT_HCI_LE_SET_PER_ADV_RECV_ENABLE_ENABLE           BIT(0)
#define BT_HCI_LE_SET_PER_ADV_RECV_ENABLE_FILTER_DUPLICATE BIT(1)

#define BT_HCI_OP_LE_SET_PER_ADV_RECV_ENABLE     BT_OP(BT_OGF_LE, 0x0059) /* 0x2059 */
struct bt_hci_cp_le_set_per_adv_recv_enable {
    uint16_t handle;
    uint8_t  enable;
} __packed;

#define BT_HCI_OP_LE_PER_ADV_SYNC_TRANSFER      BT_OP(BT_OGF_LE, 0x005a) /* 0x205a */
struct bt_hci_cp_le_per_adv_sync_transfer {
    uint16_t conn_handle;
    uint16_t service_data;
    uint16_t sync_handle;
} __packed;

struct bt_hci_rp_le_per_adv_sync_transfer {
    uint8_t  status;
    uint16_t conn_handle;
} __packed;

#define BT_HCI_OP_LE_PER_ADV_SET_INFO_TRANSFER  BT_OP(BT_OGF_LE, 0x005b) /* 0x205b */
struct bt_hci_cp_le_per_adv_set_info_transfer {
    uint16_t conn_handle;
    uint16_t service_data;
    uint8_t  adv_handle;
} __packed;

struct bt_hci_rp_le_per_adv_set_info_transfer {
    uint8_t  status;
    uint16_t conn_handle;
} __packed;

#define BT_HCI_LE_PAST_MODE_NO_SYNC                0x00
#define BT_HCI_LE_PAST_MODE_NO_REPORTS             0x01
#define BT_HCI_LE_PAST_MODE_SYNC                   0x02
#define BT_HCI_LE_PAST_MODE_SYNC_FILTER_DUPLICATES 0x03

#define BT_HCI_LE_PAST_CTE_TYPE_NO_AOA           BIT(0)
#define BT_HCI_LE_PAST_CTE_TYPE_NO_AOD_1US       BIT(1)
#define BT_HCI_LE_PAST_CTE_TYPE_NO_AOD_2US       BIT(2)
#define BT_HCI_LE_PAST_CTE_TYPE_NO_CTE           BIT(3)
#define BT_HCI_LE_PAST_CTE_TYPE_ONLY_CTE         BIT(4)

#define BT_HCI_OP_LE_PAST_PARAM                 BT_OP(BT_OGF_LE, 0x005c) /* 0x205c */
struct bt_hci_cp_le_past_param {
    uint16_t conn_handle;
    uint8_t  mode;
    uint16_t skip;
    uint16_t timeout;
    uint8_t  cte_type;
} __packed;

struct bt_hci_rp_le_past_param {
    uint8_t  status;
    uint16_t conn_handle;
} __packed;

#define BT_HCI_OP_LE_DEFAULT_PAST_PARAM         BT_OP(BT_OGF_LE, 0x005d) /* 0x205d */
struct bt_hci_cp_le_default_past_param {
    uint8_t  mode;
    uint16_t skip;
    uint16_t timeout;
    uint8_t  cte_type;
} __packed;

struct bt_hci_rp_le_default_past_param {
    uint8_t  status;
} __packed;

#define BT_HCI_OP_LE_READ_BUFFER_SIZE_V2        BT_OP(BT_OGF_LE, 0x0060) /* 0x2060 */
struct bt_hci_rp_le_read_buffer_size_v2 {
    uint8_t  status;
    uint16_t acl_max_len;
    uint8_t  acl_max_num;
    uint16_t iso_max_len;
    uint8_t  iso_max_num;
} __packed;

#define BT_HCI_OP_LE_READ_ISO_TX_SYNC           BT_OP(BT_OGF_LE, 0x0061) /* 0x2061 */
struct bt_hci_cp_le_read_iso_tx_sync {
    uint16_t handle;
} __packed;

struct bt_hci_rp_le_read_iso_tx_sync {
    uint8_t  status;
    uint16_t handle;
    uint16_t seq;
    uint32_t timestamp;
    uint8_t  offset[3];
} __packed;

#define BT_HCI_ISO_CIG_ID_MAX                   0xFE
#define BT_HCI_ISO_CIS_COUNT_MAX                0x1F
#define BT_HCI_ISO_SDU_INTERVAL_MIN             0x0000FF
#define BT_HCI_ISO_SDU_INTERVAL_MAX             0x0FFFFF
#define BT_HCI_ISO_WORST_CASE_SCA_VALID_MASK    0x07
#define BT_HCI_ISO_PACKING_VALID_MASK           0x01
#define BT_HCI_ISO_FRAMING_VALID_MASK           0x01
#define BT_HCI_ISO_MAX_TRANSPORT_LATENCY_MIN    0x0005
#define BT_HCI_ISO_MAX_TRANSPORT_LATENCY_MAX    0x0FA0
#define BT_HCI_ISO_CIS_ID_VALID_MAX             0xEF
#define BT_HCI_ISO_MAX_SDU_VALID_MASK           0x0FFF
#define BT_HCI_ISO_PHY_VALID_MASK               0x07
#define BT_HCI_ISO_INTERVAL_MIN                 0x0004
#define BT_HCI_ISO_INTERVAL_MAX                 0x0C80

#define BT_HCI_OP_LE_SET_CIG_PARAMS             BT_OP(BT_OGF_LE, 0x0062) /* 0x2062 */
struct bt_hci_cis_params {
    uint8_t  cis_id;
    uint16_t c_sdu;
    uint16_t p_sdu;
    uint8_t  c_phy;
    uint8_t  p_phy;
    uint8_t  c_rtn;
    uint8_t  p_rtn;
} __packed;

struct bt_hci_cp_le_set_cig_params {
    uint8_t  cig_id;
    uint8_t  c_interval[3];
    uint8_t  p_interval[3];
    uint8_t  sca;
    uint8_t  packing;
    uint8_t  framing;
    uint16_t c_latency;
    uint16_t p_latency;
    uint8_t  num_cis;
    struct bt_hci_cis_params cis[0];
} __packed;

struct bt_hci_rp_le_set_cig_params {
    uint8_t  status;
    uint8_t  cig_id;
    uint8_t  num_handles;
    uint16_t handle[0];
} __packed;

#define BT_HCI_OP_LE_SET_CIG_PARAMS_TEST        BT_OP(BT_OGF_LE, 0x0063) /* 0x2063 */
struct bt_hci_cis_params_test {
    uint8_t  cis_id;
    uint8_t  nse;
    uint16_t c_sdu;
    uint16_t p_sdu;
    uint16_t c_pdu;
    uint16_t p_pdu;
    uint8_t  c_phy;
    uint8_t  p_phy;
    uint8_t  c_bn;
    uint8_t  p_bn;
} __packed;

struct bt_hci_cp_le_set_cig_params_test {
    uint8_t  cig_id;
    uint8_t  c_interval[3];
    uint8_t  p_interval[3];
    uint8_t  c_ft;
    uint8_t  p_ft;
    uint16_t iso_interval;
    uint8_t  sca;
    uint8_t  packing;
    uint8_t  framing;
    uint8_t  num_cis;
    struct bt_hci_cis_params_test cis[0];
} __packed;

struct bt_hci_rp_le_set_cig_params_test {
    uint8_t  status;
    uint8_t  cig_id;
    uint8_t  num_handles;
    uint16_t handle[0];
} __packed;

#define BT_HCI_OP_LE_CREATE_CIS                 BT_OP(BT_OGF_LE, 0x0064) /* 0x2064 */
struct bt_hci_cis {
    uint16_t  cis_handle;
    uint16_t  acl_handle;
} __packed;

struct bt_hci_cp_le_create_cis {
    uint8_t  num_cis;
    struct bt_hci_cis cis[0];
} __packed;

#define BT_HCI_OP_LE_REMOVE_CIG                 BT_OP(BT_OGF_LE, 0x0065) /* 0x2065 */
struct bt_hci_cp_le_remove_cig {
    uint8_t  cig_id;
} __packed;

struct bt_hci_rp_le_remove_cig {
    uint8_t  status;
    uint8_t  cig_id;
} __packed;

#define BT_HCI_OP_LE_ACCEPT_CIS                 BT_OP(BT_OGF_LE, 0x0066) /* 0x2066 */
struct bt_hci_cp_le_accept_cis {
    uint16_t handle;
} __packed;

#define BT_HCI_OP_LE_REJECT_CIS                 BT_OP(BT_OGF_LE, 0x0067) /* 0x2067 */
struct bt_hci_cp_le_reject_cis {
    uint16_t handle;
    uint8_t  reason;
} __packed;

struct bt_hci_rp_le_reject_cis {
    uint8_t  status;
    uint16_t handle;
} __packed;

#define BT_HCI_OP_LE_CREATE_BIG                 BT_OP(BT_OGF_LE, 0x0068) /* 0x2068 */
struct bt_hci_cp_le_create_big {
    uint8_t  big_handle;
    uint8_t  adv_handle;
    uint8_t  num_bis;
    uint8_t  sdu_interval[3];
    uint16_t max_sdu;
    uint16_t max_latency;
    uint8_t  rtn;
    uint8_t  phy;
    uint8_t  packing;
    uint8_t  framing;
    uint8_t  encryption;
    uint8_t  bcode[16];
} __packed;

#define BT_HCI_OP_LE_CREATE_BIG_TEST            BT_OP(BT_OGF_LE, 0x0069) /* 0x2069 */
struct bt_hci_cp_le_create_big_test {
    uint8_t  big_handle;
    uint8_t  adv_handle;
    uint8_t  num_bis;
    uint8_t  sdu_interval[3];
    uint16_t iso_interval;
    uint8_t  nse;
    uint16_t max_sdu;
    uint16_t max_pdu;
    uint8_t  phy;
    uint8_t  packing;
    uint8_t  framing;
    uint8_t  bn;
    uint8_t  irc;
    uint8_t  pto;
    uint8_t  encryption;
    uint8_t  bcode[16];
} __packed;

#define BT_HCI_OP_LE_TERMINATE_BIG              BT_OP(BT_OGF_LE, 0x006a) /* 0x206a */
struct bt_hci_cp_le_terminate_big {
    uint8_t  big_handle;
    uint8_t  reason;
} __packed;

#define BT_HCI_OP_LE_BIG_CREATE_SYNC            BT_OP(BT_OGF_LE, 0x006b) /* 0x206b */
struct bt_hci_cp_le_big_create_sync {
    uint8_t  big_handle;
    uint16_t sync_handle;
    uint8_t  encryption;
    uint8_t  bcode[16];
    uint8_t  mse;
    uint16_t sync_timeout;
    uint8_t  num_bis;
    uint8_t  bis[0];
} __packed;

#define BT_HCI_OP_LE_BIG_TERMINATE_SYNC         BT_OP(BT_OGF_LE, 0x006c) /* 0x206c */
struct bt_hci_cp_le_big_terminate_sync {
    uint8_t  big_handle;
} __packed;

struct bt_hci_rp_le_big_terminate_sync {
    uint8_t  status;
    uint8_t  big_handle;
} __packed;

#define BT_HCI_OP_LE_REQ_PEER_SC                BT_OP(BT_OGF_LE, 0x006d) /* 0x206d */
struct bt_hci_cp_le_req_peer_sca {
    uint16_t handle;
} __packed;

#define BT_HCI_OP_LE_SETUP_ISO_PATH             BT_OP(BT_OGF_LE, 0x006e) /* 0x206e */
struct bt_hci_cp_le_setup_iso_path {
    uint16_t handle;
    uint8_t  path_dir;
    uint8_t  path_id;
    struct bt_hci_cp_codec_id codec_id;
    uint8_t  controller_delay[3];
    uint8_t  codec_config_len;
    uint8_t  codec_config[0];
} __packed;

struct bt_hci_rp_le_setup_iso_path {
    uint8_t  status;
    uint16_t handle;
} __packed;

#define BT_HCI_OP_LE_REMOVE_ISO_PATH            BT_OP(BT_OGF_LE, 0x006f) /* 0x206f */
struct bt_hci_cp_le_remove_iso_path {
    uint16_t handle;
    uint8_t  path_dir;
} __packed;

struct bt_hci_rp_le_remove_iso_path {
    uint8_t  status;
    uint16_t handle;
} __packed;

#define BT_HCI_ISO_TEST_ZERO_SIZE_SDU     0
#define BT_HCI_ISO_TEST_VARIABLE_SIZE_SDU 1
#define BT_HCI_ISO_TEST_MAX_SIZE_SDU      2

#define BT_HCI_OP_LE_ISO_TRANSMIT_TEST          BT_OP(BT_OGF_LE, 0x0070) /* 0x2070 */
struct bt_hci_cp_le_iso_transmit_test {
    uint16_t handle;
    uint8_t  payload_type;
} __packed;

struct bt_hci_rp_le_iso_transmit_test {
    uint8_t  status;
    uint16_t handle;
} __packed;

#define BT_HCI_OP_LE_ISO_RECEIVE_TEST           BT_OP(BT_OGF_LE, 0x0071) /* 0x2071 */
struct bt_hci_cp_le_iso_receive_test {
    uint16_t handle;
    uint8_t  payload_type;
} __packed;

struct bt_hci_rp_le_iso_receive_test {
    uint8_t  status;
    uint16_t handle;
} __packed;

#define BT_HCI_OP_LE_ISO_READ_TEST_COUNTERS     BT_OP(BT_OGF_LE, 0x0072) /* 0x2072 */
struct bt_hci_cp_le_read_test_counters {
    uint16_t handle;
} __packed;

struct bt_hci_rp_le_read_test_counters {
    uint8_t  status;
    uint16_t handle;
    uint32_t received_cnt;
    uint32_t missed_cnt;
    uint32_t failed_cnt;
} __packed;

#define BT_HCI_OP_LE_ISO_TEST_END               BT_OP(BT_OGF_LE, 0x0073) /* 0x2073 */
struct bt_hci_cp_le_iso_test_end {
    uint16_t handle;
} __packed;

struct bt_hci_rp_le_iso_test_end {
    uint8_t  status;
    uint16_t handle;
    uint32_t received_cnt;
    uint32_t missed_cnt;
    uint32_t failed_cnt;
} __packed;

#define BT_HCI_OP_LE_SET_HOST_FEATURE           BT_OP(BT_OGF_LE, 0x0074) /* 0x2074 */
struct bt_hci_cp_le_set_host_feature {
    uint8_t  bit_number;
    uint8_t  bit_value;
} __packed;

struct bt_hci_rp_le_set_host_feature {
    uint8_t  status;
} __packed;

#define BT_HCI_OP_LE_READ_ISO_LINK_QUALITY      BT_OP(BT_OGF_LE, 0x0075) /* 0x2075 */
struct bt_hci_cp_le_read_iso_link_quality {
    uint16_t handle;
} __packed;

struct bt_hci_rp_le_read_iso_link_quality {
    uint8_t  status;
    uint16_t handle;
    uint32_t tx_unacked_packets;
    uint32_t tx_flushed_packets;
    uint32_t tx_last_subevent_packets;
    uint32_t retransmitted_packets;
    uint32_t crc_error_packets;
    uint32_t rx_unreceived_packets;
    uint32_t duplicate_packets;
} __packed;

#define BT_HCI_OP_LE_TX_TEST_V4                 BT_OP(BT_OGF_LE, 0x007B) /* 0x207B */

struct bt_hci_cp_le_tx_test_v4 {
    uint8_t  tx_ch;
    uint8_t  test_data_len;
    uint8_t  pkt_payload;
    uint8_t  phy;
    uint8_t  cte_len;
    uint8_t  cte_type;
    uint8_t  switch_pattern_len;
    uint8_t  ant_ids[0];
} __packed;

#define BT_HCI_TX_TEST_POWER_MIN -0x7F
#define BT_HCI_TX_TEST_POWER_MAX 0x14

#define BT_HCI_TX_TEST_POWER_MIN_SET 0x7E
#define BT_HCI_TX_TEST_POWER_MAX_SET 0x7F

/* Helper structure for Tx power parameter in the HCI Tx Test v4 command.
 * Previous parameter of this command is variable size so having separated structure
 * for this parameter helps in command parameters unpacking.
 */
struct bt_hci_cp_le_tx_test_v4_tx_power {
    int8_t tx_power;
} __packed;

#define BT_HCI_OP_LE_CS_READ_LOCAL_SUPPORTED_CAPABILITIES BT_OP(BT_OGF_LE, 0x0089) /* 0x2089 */

struct bt_hci_rp_le_read_local_supported_capabilities {
    uint8_t status;
    uint8_t num_config_supported;
    uint16_t max_consecutive_procedures_supported;
    uint8_t num_antennas_supported;
    uint8_t max_antenna_paths_supported;
    uint8_t roles_supported;
    uint8_t modes_supported;
    uint8_t rtt_capability;
    uint8_t rtt_aa_only_n;
    uint8_t rtt_sounding_n;
    uint8_t rtt_random_payload_n;
    uint16_t nadm_sounding_capability;
    uint16_t nadm_random_capability;
    uint8_t cs_sync_phys_supported;
    uint16_t subfeatures_supported;
    uint16_t t_ip1_times_supported;
    uint16_t t_ip2_times_supported;
    uint16_t t_fcs_times_supported;
    uint16_t t_pm_times_supported;
    uint8_t t_sw_time_supported;
    uint8_t tx_snr_capability;
} __packed;

#define BT_HCI_OP_LE_CS_READ_REMOTE_SUPPORTED_CAPABILITIES BT_OP(BT_OGF_LE, 0x008A) /* 0x208A */

struct bt_hci_cp_le_read_remote_supported_capabilities {
    uint16_t handle;
} __packed;

#define BT_HCI_OP_LE_CS_WRITE_CACHED_REMOTE_SUPPORTED_CAPABILITIES \
    BT_OP(BT_OGF_LE, 0x008B) /* 0x208B */

struct bt_hci_cp_le_write_cached_remote_supported_capabilities {
    uint16_t handle;
    uint8_t num_config_supported;
    uint16_t max_consecutive_procedures_supported;
    uint8_t num_antennas_supported;
    uint8_t max_antenna_paths_supported;
    uint8_t roles_supported;
    uint8_t modes_supported;
    uint8_t rtt_capability;
    uint8_t rtt_aa_only_n;
    uint8_t rtt_sounding_n;
    uint8_t rtt_random_payload_n;
    uint16_t nadm_sounding_capability;
    uint16_t nadm_random_capability;
    uint8_t cs_sync_phys_supported;
    uint16_t subfeatures_supported;
    uint16_t t_ip1_times_supported;
    uint16_t t_ip2_times_supported;
    uint16_t t_fcs_times_supported;
    uint16_t t_pm_times_supported;
    uint8_t t_sw_time_supported;
    uint8_t tx_snr_capability;
} __packed;

#define BT_HCI_OP_LE_CS_SECURITY_ENABLE BT_OP(BT_OGF_LE, 0x008C) /* 0x208C */

struct bt_hci_cp_le_security_enable {
    uint16_t handle;
} __packed;

#define BT_HCI_OP_LE_CS_SET_DEFAULT_SETTINGS BT_OP(BT_OGF_LE, 0x008D) /* 0x208D */

#define BT_HCI_OP_LE_CS_INITIATOR_ROLE_MASK BIT(0)
#define BT_HCI_OP_LE_CS_REFLECTOR_ROLE_MASK BIT(1)

#define BT_HCI_OP_LE_CS_MIN_MAX_TX_POWER -127
#define BT_HCI_OP_LE_CS_MAX_MAX_TX_POWER 20

#define BT_HCI_OP_LE_CS_ANTENNA_SEL_ONE   0x01
#define BT_HCI_OP_LE_CS_ANTENNA_SEL_TWO   0x02
#define BT_HCI_OP_LE_CS_ANTENNA_SEL_THREE 0x03
#define BT_HCI_OP_LE_CS_ANTENNA_SEL_FOUR  0x04
#define BT_HCI_OP_LE_CS_ANTENNA_SEL_REP   0xFE
#define BT_HCI_OP_LE_CS_ANTENNA_SEL_NONE  0xFF

struct bt_hci_cp_le_cs_set_default_settings {
    uint16_t handle;
    uint8_t role_enable;
    uint8_t cs_sync_antenna_selection;
    int8_t max_tx_power;
} __packed;

#define BT_HCI_OP_LE_CS_READ_REMOTE_FAE_TABLE BT_OP(BT_OGF_LE, 0x008E) /* 0x208E */

struct bt_hci_cp_le_read_remote_fae_table {
    uint16_t handle;
} __packed;

#define BT_HCI_OP_LE_CS_WRITE_CACHED_REMOTE_FAE_TABLE BT_OP(BT_OGF_LE, 0x008F) /* 0x208F */

struct bt_hci_cp_le_write_cached_remote_fae_table {
    uint16_t handle;
    uint8_t remote_fae_table[72];
} __packed;

#define BT_HCI_OP_LE_CS_SET_CHANNEL_CLASSIFICATION BT_OP(BT_OGF_LE, 0x0092) /* 0x2092 */

#define BT_HCI_OP_LE_CS_SET_PROCEDURE_PARAMETERS BT_OP(BT_OGF_LE, 0x0093) /* 0x2093 */

#define BT_HCI_OP_LE_CS_PROCEDURE_PHY_1M       0x01
#define BT_HCI_OP_LE_CS_PROCEDURE_PHY_2M       0x02
#define BT_HCI_OP_LE_CS_PROCEDURE_PHY_CODED_S8 0x03
#define BT_HCI_OP_LE_CS_PROCEDURE_PHY_CODED_S2 0x04

struct bt_hci_cp_le_set_procedure_parameters {
    uint16_t handle;
    uint8_t config_id;
    uint16_t max_procedure_len;
    uint16_t min_procedure_interval;
    uint16_t max_procedure_interval;
    uint16_t max_procedure_count;
    uint8_t min_subevent_len[3];
    uint8_t max_subevent_len[3];
    uint8_t tone_antenna_config_selection;
    uint8_t phy;
    uint8_t tx_power_delta;
    uint8_t preferred_peer_antenna;
    uint8_t snr_control_initiator;
    uint8_t snr_control_reflector;
} __packed;

#define BT_HCI_OP_LE_CS_PROCEDURE_ENABLE BT_OP(BT_OGF_LE, 0x0094) /* 0x2094 */

#define BT_HCI_OP_LE_CS_PROCEDURES_DISABLED 0x00
#define BT_HCI_OP_LE_CS_PROCEDURES_ENABLED  0x01

struct bt_hci_cp_le_procedure_enable {
    uint16_t handle;
    uint8_t config_id;
    uint8_t enable;
} __packed;

#define BT_HCI_OP_LE_CS_TEST BT_OP(BT_OGF_LE, 0x0095) /* 0x2095 */

#define BT_HCI_OP_LE_CS_MAIN_MODE_1 0x1
#define BT_HCI_OP_LE_CS_MAIN_MODE_2 0x2
#define BT_HCI_OP_LE_CS_MAIN_MODE_3 0x3

#define BT_HCI_OP_LE_CS_SUB_MODE_1 0x1
#define BT_HCI_OP_LE_CS_SUB_MODE_2 0x2
#define BT_HCI_OP_LE_CS_SUB_MODE_3 0x3
#define BT_HCI_OP_LE_CS_SUB_MODE_UNUSED 0xFF

#define BT_HCI_OP_LE_CS_INITIATOR_ROLE 0x0
#define BT_HCI_OP_LE_CS_REFLECTOR_ROLE 0x1

#define BT_HCI_OP_LE_CS_RTT_TYPE_AA_ONLY     0x0
#define BT_HCI_OP_LE_CS_RTT_TYPE_32BIT_SOUND 0x1
#define BT_HCI_OP_LE_CS_RTT_TYPE_96BIT_SOUND 0x2
#define BT_HCI_OP_LE_CS_RTT_TYPE_32BIT_RAND  0x3
#define BT_HCI_OP_LE_CS_RTT_TYPE_64BIT_RAND  0x4
#define BT_HCI_OP_LE_CS_RTT_TYPE_96BIT_RAND  0x5
#define BT_HCI_OP_LE_CS_RTT_TYPE_128BIT_RAND 0x6

#define BT_HCI_OP_LE_CS_CS_SYNC_1M     0x1
#define BT_HCI_OP_LE_CS_CS_SYNC_2M     0x2
#define BT_HCI_OP_LE_CS_CS_SYNC_2M_2BT 0x3

#define BT_HCI_OP_LE_CS_TEST_MINIMIZE_TX_POWER 0x7E
#define BT_HCI_OP_LE_CS_TEST_MAXIMIZE_TX_POWER 0x7F

#define BT_HCI_OP_LE_CS_ACI_0 0x0
#define BT_HCI_OP_LE_CS_ACI_1 0x1
#define BT_HCI_OP_LE_CS_ACI_2 0x2
#define BT_HCI_OP_LE_CS_ACI_3 0x3
#define BT_HCI_OP_LE_CS_ACI_4 0x4
#define BT_HCI_OP_LE_CS_ACI_5 0x5
#define BT_HCI_OP_LE_CS_ACI_6 0x6
#define BT_HCI_OP_LE_CS_ACI_7 0x7

#define BT_HCI_OP_LE_CS_INITIATOR_SNR_18 0x0
#define BT_HCI_OP_LE_CS_INITIATOR_SNR_21 0x1
#define BT_HCI_OP_LE_CS_INITIATOR_SNR_24 0x2
#define BT_HCI_OP_LE_CS_INITIATOR_SNR_27 0x3
#define BT_HCI_OP_LE_CS_INITIATOR_SNR_30 0x4
#define BT_HCI_OP_LE_CS_INITIATOR_SNR_NOT_USED 0xFF

#define BT_HCI_OP_LE_CS_REFLECTOR_SNR_18 0x0
#define BT_HCI_OP_LE_CS_REFLECTOR_SNR_21 0x1
#define BT_HCI_OP_LE_CS_REFLECTOR_SNR_24 0x2
#define BT_HCI_OP_LE_CS_REFLECTOR_SNR_27 0x3
#define BT_HCI_OP_LE_CS_REFLECTOR_SNR_30 0x4
#define BT_HCI_OP_LE_CS_REFLECTOR_SNR_NOT_USED 0xFF

#define BT_HCI_OP_LE_CS_TEST_OVERRIDE_CONFIG_0_MASK BIT(0)
#define BT_HCI_OP_LE_CS_TEST_OVERRIDE_CONFIG_2_MASK BIT(2)
#define BT_HCI_OP_LE_CS_TEST_OVERRIDE_CONFIG_3_MASK BIT(3)
#define BT_HCI_OP_LE_CS_TEST_OVERRIDE_CONFIG_4_MASK BIT(4)
#define BT_HCI_OP_LE_CS_TEST_OVERRIDE_CONFIG_5_MASK BIT(5)
#define BT_HCI_OP_LE_CS_TEST_OVERRIDE_CONFIG_6_MASK BIT(6)
#define BT_HCI_OP_LE_CS_TEST_OVERRIDE_CONFIG_7_MASK BIT(7)
#define BT_HCI_OP_LE_CS_TEST_OVERRIDE_CONFIG_8_MASK BIT(8)
#define BT_HCI_OP_LE_CS_TEST_OVERRIDE_CONFIG_10_MASK BIT(10)

#define BT_HCI_OP_LE_CS_TEST_CHSEL_TYPE_3B 0x0
#define BT_HCI_OP_LE_CS_TEST_CHSEL_TYPE_3C 0x1

#define BT_HCI_OP_LE_CS_TEST_CH3C_SHAPE_HAT 0x0
#define BT_HCI_OP_LE_CS_TEST_CH3C_SHAPE_X   0x1

#define BT_HCI_OP_LE_CS_TEST_TONE_EXT_NONE 0x0
#define BT_HCI_OP_LE_CS_TEST_TONE_EXT_INIT 0x1
#define BT_HCI_OP_LE_CS_TEST_TONE_EXT_REFL 0x2
#define BT_HCI_OP_LE_CS_TEST_TONE_EXT_BOTH 0x3
#define BT_HCI_OP_LE_CS_TEST_TONE_EXT_REPEAT 0x4

#define BT_HCI_OP_LE_CS_TEST_AP_INDEX_00 0x0
#define BT_HCI_OP_LE_CS_TEST_AP_INDEX_01 0x1
#define BT_HCI_OP_LE_CS_TEST_AP_INDEX_02 0x2
#define BT_HCI_OP_LE_CS_TEST_AP_INDEX_03 0x3
#define BT_HCI_OP_LE_CS_TEST_AP_INDEX_04 0x4
#define BT_HCI_OP_LE_CS_TEST_AP_INDEX_05 0x5
#define BT_HCI_OP_LE_CS_TEST_AP_INDEX_06 0x6
#define BT_HCI_OP_LE_CS_TEST_AP_INDEX_07 0x7
#define BT_HCI_OP_LE_CS_TEST_AP_INDEX_08 0x8
#define BT_HCI_OP_LE_CS_TEST_AP_INDEX_09 0x9
#define BT_HCI_OP_LE_CS_TEST_AP_INDEX_10 0xA
#define BT_HCI_OP_LE_CS_TEST_AP_INDEX_11 0xB
#define BT_HCI_OP_LE_CS_TEST_AP_INDEX_12 0xC
#define BT_HCI_OP_LE_CS_TEST_AP_INDEX_13 0xD
#define BT_HCI_OP_LE_CS_TEST_AP_INDEX_14 0xE
#define BT_HCI_OP_LE_CS_TEST_AP_INDEX_15 0xF
#define BT_HCI_OP_LE_CS_TEST_AP_INDEX_16 0x10
#define BT_HCI_OP_LE_CS_TEST_AP_INDEX_17 0x11
#define BT_HCI_OP_LE_CS_TEST_AP_INDEX_18 0x12
#define BT_HCI_OP_LE_CS_TEST_AP_INDEX_19 0x13
#define BT_HCI_OP_LE_CS_TEST_AP_INDEX_20 0x14
#define BT_HCI_OP_LE_CS_TEST_AP_INDEX_21 0x15
#define BT_HCI_OP_LE_CS_TEST_AP_INDEX_22 0x16
#define BT_HCI_OP_LE_CS_TEST_AP_INDEX_23 0x17
#define BT_HCI_OP_LE_CS_TEST_AP_INDEX_LOOP 0xFF

#define BT_HCI_OP_LE_CS_TEST_SS_MARKER_2_POSITION_NOT_PRESENT 0xFF

#define BT_HCI_OP_LE_CS_TEST_SS_MARKER_VAL_0011 0x0
#define BT_HCI_OP_LE_CS_TEST_SS_MARKER_VAL_1100 0x1
#define BT_HCI_OP_LE_CS_TEST_SS_MARKER_VAL_LOOP 0x2

#define BT_HCI_OP_LE_CS_TEST_PAYLOAD_PRBS9    0x00
#define BT_HCI_OP_LE_CS_TEST_PAYLOAD_11110000 0x01
#define BT_HCI_OP_LE_CS_TEST_PAYLOAD_10101010 0x02
#define BT_HCI_OP_LE_CS_TEST_PAYLOAD_PRBS15   0x03
#define BT_HCI_OP_LE_CS_TEST_PAYLOAD_11111111 0x04
#define BT_HCI_OP_LE_CS_TEST_PAYLOAD_00000000 0x05
#define BT_HCI_OP_LE_CS_TEST_PAYLOAD_00001111 0x06
#define BT_HCI_OP_LE_CS_TEST_PAYLOAD_01010101 0x07
#define BT_HCI_OP_LE_CS_TEST_PAYLOAD_USER     0x80

struct bt_hci_op_le_cs_test {
    uint8_t main_mode_type;
    uint8_t sub_mode_type;
    uint8_t main_mode_repetition;
    uint8_t mode_0_steps;
    uint8_t role;
    uint8_t rtt_type;
    uint8_t cs_sync_phy;
    uint8_t cs_sync_antenna_selection;
    uint8_t subevent_len[3];
    uint16_t subevent_interval;
    uint8_t max_num_subevents;
    uint8_t transmit_power_level;
    uint8_t t_ip1_time;
    uint8_t t_ip2_time;
    uint8_t t_fcs_time;
    uint8_t t_pm_time;
    uint8_t t_sw_time;
    uint8_t tone_antenna_config_selection;
    uint8_t reserved;
    uint8_t snr_control_initiator;
    uint8_t snr_control_reflector;
    uint16_t drbg_nonce;
    uint8_t channel_map_repetition;
    uint16_t override_config;
    uint8_t override_parameters_length;
    uint8_t override_parameters_data[];
} __packed;

#define BT_HCI_OP_LE_CS_CREATE_CONFIG BT_OP(BT_OGF_LE, 0x0090) /* 0x2090 */

struct bt_hci_cp_le_cs_create_config {
    uint16_t handle;
    uint8_t config_id;
    uint8_t create_context;
    uint8_t main_mode_type;
    uint8_t sub_mode_type;
    uint8_t min_main_mode_steps;
    uint8_t max_main_mode_steps;
    uint8_t main_mode_repetition;
    uint8_t mode_0_steps;
    uint8_t role;
    uint8_t rtt_type;
    uint8_t cs_sync_phy;
    uint8_t channel_map[10];
    uint8_t channel_map_repetition;
    uint8_t channel_selection_type;
    uint8_t ch3c_shape;
    uint8_t ch3c_jump;
    uint8_t reserved;
} __packed;

#define BT_HCI_OP_LE_CS_REMOVE_CONFIG BT_OP(BT_OGF_LE, 0x0091) /* 0x2091 */

struct bt_hci_cp_le_cs_remove_config {
    uint16_t handle;
    uint8_t config_id;
} __packed;

#define BT_HCI_OP_LE_CS_TEST_END BT_OP(BT_OGF_LE, 0x0096) /* 0x2096 */

/* Event definitions */

#define BT_HCI_EVT_UNKNOWN                      0x00
#define BT_HCI_EVT_VENDOR                       0xff

#define BT_HCI_EVT_INQUIRY_COMPLETE             0x01
struct bt_hci_evt_inquiry_complete {
    uint8_t status;
} __packed;

#define BT_HCI_EVT_CONN_COMPLETE                0x03
struct bt_hci_evt_conn_complete {
    uint8_t   status;
    uint16_t  handle;
    bt_addr_t bdaddr;
    uint8_t   link_type;
    uint8_t   encr_enabled;
} __packed;

#define BT_HCI_EVT_CONN_REQUEST                 0x04
struct bt_hci_evt_conn_request {
    bt_addr_t bdaddr;
    uint8_t   dev_class[3];
    uint8_t   link_type;
} __packed;

#define BT_HCI_EVT_DISCONN_COMPLETE             0x05
struct bt_hci_evt_disconn_complete {
    uint8_t  status;
    uint16_t handle;
    uint8_t  reason;
} __packed;

#define BT_HCI_EVT_AUTH_COMPLETE                0x06
struct bt_hci_evt_auth_complete {
    uint8_t  status;
    uint16_t handle;
} __packed;

#define BT_HCI_EVT_REMOTE_NAME_REQ_COMPLETE     0x07
struct bt_hci_evt_remote_name_req_complete {
    uint8_t   status;
    bt_addr_t bdaddr;
    uint8_t   name[248];
} __packed;

#define BT_HCI_EVT_ENCRYPT_CHANGE               0x08
struct bt_hci_evt_encrypt_change {
    uint8_t  status;
    uint16_t handle;
    uint8_t  encrypt;
} __packed;

#define BT_HCI_EVT_REMOTE_FEATURES              0x0b
struct bt_hci_evt_remote_features {
    uint8_t  status;
    uint16_t handle;
    uint8_t  features[8];
} __packed;

#define BT_HCI_EVT_REMOTE_VERSION_INFO          0x0c
struct bt_hci_evt_remote_version_info {
    uint8_t  status;
    uint16_t handle;
    uint8_t  version;
    uint16_t manufacturer;
    uint16_t subversion;
} __packed;

#define BT_HCI_EVT_CMD_COMPLETE                 0x0e
struct bt_hci_evt_cmd_complete {
    uint8_t  ncmd;
    uint16_t opcode;
} __packed;

struct bt_hci_evt_cc_status {
    uint8_t  status;
} __packed;

#define BT_HCI_EVT_CMD_STATUS                   0x0f
struct bt_hci_evt_cmd_status {
    uint8_t  status;
    uint8_t  ncmd;
    uint16_t opcode;
} __packed;

#define BT_HCI_EVT_HARDWARE_ERROR               0x10
struct bt_hci_evt_hardware_error {
    uint8_t  hardware_code;
} __packed;

#define BT_HCI_EVT_ROLE_CHANGE                  0x12
struct bt_hci_evt_role_change {
    uint8_t   status;
    bt_addr_t bdaddr;
    uint8_t   role;
} __packed;

#define BT_HCI_EVT_NUM_COMPLETED_PACKETS        0x13
struct bt_hci_evt_num_completed_packets {
    uint8_t  num_handles;
    struct bt_hci_handle_count h[0];
} __packed;

#define BT_HCI_EVT_PIN_CODE_REQ                 0x16
struct bt_hci_evt_pin_code_req {
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_EVT_LINK_KEY_REQ                 0x17
struct bt_hci_evt_link_key_req {
    bt_addr_t bdaddr;
} __packed;

/* Link Key types */
#define BT_LK_COMBINATION                       0x00
#define BT_LK_LOCAL_UNIT                        0x01
#define BT_LK_REMOTE_UNIT                       0x02
#define BT_LK_DEBUG_COMBINATION                 0x03
#define BT_LK_UNAUTH_COMBINATION_P192           0x04
#define BT_LK_AUTH_COMBINATION_P192             0x05
#define BT_LK_CHANGED_COMBINATION               0x06
#define BT_LK_UNAUTH_COMBINATION_P256           0x07
#define BT_LK_AUTH_COMBINATION_P256             0x08

#define BT_HCI_EVT_LINK_KEY_NOTIFY              0x18
struct bt_hci_evt_link_key_notify {
    bt_addr_t bdaddr;
    uint8_t   link_key[16];
    uint8_t   key_type;
} __packed;

/* Overflow link types */
#define BT_OVERFLOW_LINK_SYNCH                  0x00
#define BT_OVERFLOW_LINK_ACL                    0x01
#define BT_OVERFLOW_LINK_ISO                    0x02

#define BT_HCI_EVT_DATA_BUF_OVERFLOW            0x1a
struct bt_hci_evt_data_buf_overflow {
    uint8_t  link_type;
} __packed;

#define BT_HCI_EVT_INQUIRY_RESULT_WITH_RSSI     0x22
struct bt_hci_evt_inquiry_result_with_rssi {
    bt_addr_t addr;
    uint8_t   pscan_rep_mode;
    uint8_t   reserved;
    uint8_t   cod[3];
    uint16_t  clock_offset;
    int8_t    rssi;
} __packed;

#define BT_HCI_EVT_REMOTE_EXT_FEATURES          0x23
struct bt_hci_evt_remote_ext_features {
    uint8_t  status;
    uint16_t handle;
    uint8_t  page;
    uint8_t  max_page;
    uint8_t  features[8];
} __packed;

#define BT_HCI_EVT_LE_PER_ADV_SYNC_ESTABLISHED_V2 0x24
struct bt_hci_evt_le_per_adv_sync_established_v2 {
    uint8_t status;
    uint16_t handle;
    uint8_t sid;
    bt_addr_le_t adv_addr;
    uint8_t phy;
    uint16_t interval;
    uint8_t clock_accuracy;
    uint8_t num_subevents;
    uint8_t subevent_interval;
    uint8_t response_slot_delay;
    uint8_t response_slot_spacing;
} __packed;

#define BT_HCI_EVT_LE_PER_ADVERTISING_REPORT_V2 0x25
struct bt_hci_evt_le_per_advertising_report_v2 {
    uint16_t handle;
    int8_t tx_power;
    int8_t rssi;
    uint8_t cte_type;
    uint16_t periodic_event_counter;
    uint8_t subevent;
    uint8_t data_status;
    uint8_t length;
    uint8_t data[0];
} __packed;

#define BT_HCI_EVT_LE_PAST_RECEIVED_V2 0x26
struct bt_hci_evt_le_past_received_v2 {
    uint8_t status;
    uint16_t conn_handle;
    uint16_t service_data;
    uint16_t sync_handle;
    uint8_t adv_sid;
    bt_addr_le_t addr;
    uint8_t phy;
    uint16_t interval;
    uint8_t clock_accuracy;
    uint8_t num_subevents;
    uint8_t subevent_interval;
    uint8_t response_slot_delay;
    uint8_t response_slot_spacing;
} __packed;

#define BT_HCI_EVT_LE_PER_ADV_SUBEVENT_DATA_REQUEST 0x27
struct bt_hci_evt_le_per_adv_subevent_data_request {
    uint8_t adv_handle;
    uint8_t subevent_start;
    uint8_t subevent_data_count;
} __packed;

#define BT_HCI_EVT_LE_PER_ADV_RESPONSE_REPORT 0x28

struct bt_hci_evt_le_per_adv_response {
    int8_t tx_power;
    int8_t rssi;
    uint8_t cte_type;
    uint8_t response_slot;
    uint8_t data_status;
    uint8_t data_length;
    uint8_t data[0];
} __packed;

struct bt_hci_evt_le_per_adv_response_report {
    uint8_t adv_handle;
    uint8_t subevent;
    uint8_t tx_status;
    uint8_t num_responses;
    struct bt_hci_evt_le_per_adv_response responses[0];
} __packed;

#define BT_HCI_EVT_LE_ENH_CONN_COMPLETE_V2 0x29
struct bt_hci_evt_le_enh_conn_complete_v2 {
    uint8_t      status;
    uint16_t     handle;
    uint8_t      role;
    bt_addr_le_t peer_addr;
    bt_addr_t    local_rpa;
    bt_addr_t    peer_rpa;
    uint16_t     interval;
    uint16_t     latency;
    uint16_t     supv_timeout;
    uint8_t      clock_accuracy;
    uint8_t adv_handle;
    uint16_t sync_handle;
} __packed;

#define BT_HCI_EVT_SYNC_CONN_COMPLETE           0x2c
struct bt_hci_evt_sync_conn_complete {
    uint8_t    status;
    uint16_t   handle;
    bt_addr_t  bdaddr;
    uint8_t    link_type;
    uint8_t    tx_interval;
    uint8_t    retansmission_window;
    uint16_t   rx_pkt_length;
    uint16_t   tx_pkt_length;
    uint8_t    air_mode;
} __packed;

#define BT_HCI_EVT_EXTENDED_INQUIRY_RESULT      0x2f
struct bt_hci_evt_extended_inquiry_result {
    uint8_t    num_reports;
    bt_addr_t  addr;
    uint8_t    pscan_rep_mode;
    uint8_t    reserved;
    uint8_t    cod[3];
    uint16_t   clock_offset;
    int8_t     rssi;
    uint8_t    eir[240];
} __packed;

#define BT_HCI_EVT_ENCRYPT_KEY_REFRESH_COMPLETE 0x30
struct bt_hci_evt_encrypt_key_refresh_complete {
    uint8_t  status;
    uint16_t handle;
} __packed;

#define BT_HCI_EVT_IO_CAPA_REQ                  0x31
struct bt_hci_evt_io_capa_req {
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_EVT_IO_CAPA_RESP                 0x32
struct bt_hci_evt_io_capa_resp {
    bt_addr_t bdaddr;
    uint8_t   capability;
    uint8_t   oob_data;
    uint8_t   authentication;
} __packed;

#define BT_HCI_EVT_USER_CONFIRM_REQ             0x33
struct bt_hci_evt_user_confirm_req {
    bt_addr_t bdaddr;
    uint32_t  passkey;
} __packed;

#define BT_HCI_EVT_USER_PASSKEY_REQ             0x34
struct bt_hci_evt_user_passkey_req {
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_EVT_SSP_COMPLETE                 0x36
struct bt_hci_evt_ssp_complete {
    uint8_t   status;
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_EVT_USER_PASSKEY_NOTIFY          0x3b
struct bt_hci_evt_user_passkey_notify {
    bt_addr_t bdaddr;
    uint32_t  passkey;
} __packed;

#define BT_HCI_EVT_LE_META_EVENT                0x3e
struct bt_hci_evt_le_meta_event {
    uint8_t  subevent;
} __packed;

#define BT_HCI_EVT_AUTH_PAYLOAD_TIMEOUT_EXP     0x57
struct bt_hci_evt_auth_payload_timeout_exp {
    uint16_t handle;
} __packed;

#define BT_HCI_ROLE_CENTRAL                     0x00
#define BT_HCI_ROLE_PERIPHERAL                  0x01

#define BT_HCI_EVT_LE_CONN_COMPLETE             0x01
struct bt_hci_evt_le_conn_complete {
    uint8_t      status;
    uint16_t     handle;
    uint8_t      role;
    bt_addr_le_t peer_addr;
    uint16_t     interval;
    uint16_t     latency;
    uint16_t     supv_timeout;
    uint8_t      clock_accuracy;
} __packed;

#define BT_HCI_LE_RSSI_NOT_AVAILABLE            0x7F

#define BT_HCI_EVT_LE_ADVERTISING_REPORT        0x02
struct bt_hci_evt_le_advertising_info {
    uint8_t      evt_type;
    bt_addr_le_t addr;
    uint8_t      length;
    uint8_t      data[0];
} __packed;
struct bt_hci_evt_le_advertising_report {
    uint8_t num_reports;
    struct bt_hci_evt_le_advertising_info adv_info[0];
} __packed;

/** All limits according to BT Core Spec v5.4 [Vol 4, Part E]. */
#define BT_HCI_LE_INTERVAL_MIN           0x0006
#define BT_HCI_LE_INTERVAL_MAX           0x0c80
#define BT_HCI_LE_PERIPHERAL_LATENCY_MAX 0x01f3
#define BT_HCI_LE_SUPERVISON_TIMEOUT_MIN 0x000a
#define BT_HCI_LE_SUPERVISON_TIMEOUT_MAX 0x0c80

#define BT_HCI_EVT_LE_CONN_UPDATE_COMPLETE      0x03
struct bt_hci_evt_le_conn_update_complete {
    uint8_t  status;
    uint16_t handle;
    uint16_t interval;
    uint16_t latency;
    uint16_t supv_timeout;
} __packed;

#define BT_HCI_EVT_LE_REMOTE_FEAT_COMPLETE      0x04
struct bt_hci_evt_le_remote_feat_complete {
    uint8_t  status;
    uint16_t handle;
    uint8_t  features[8];
} __packed;

#define BT_HCI_EVT_LE_LTK_REQUEST               0x05
struct bt_hci_evt_le_ltk_request {
    uint16_t handle;
    uint64_t rand;
    uint16_t ediv;
} __packed;

#define BT_HCI_EVT_LE_CONN_PARAM_REQ            0x06
struct bt_hci_evt_le_conn_param_req {
    uint16_t handle;
    uint16_t interval_min;
    uint16_t interval_max;
    uint16_t latency;
    uint16_t timeout;
} __packed;

#define BT_HCI_EVT_LE_DATA_LEN_CHANGE           0x07
struct bt_hci_evt_le_data_len_change {
    uint16_t handle;
    uint16_t max_tx_octets;
    uint16_t max_tx_time;
    uint16_t max_rx_octets;
    uint16_t max_rx_time;
} __packed;

#define BT_HCI_EVT_LE_P256_PUBLIC_KEY_COMPLETE  0x08
struct bt_hci_evt_le_p256_public_key_complete {
    uint8_t status;
    uint8_t key[64];
} __packed;

#define BT_HCI_EVT_LE_GENERATE_DHKEY_COMPLETE   0x09
struct bt_hci_evt_le_generate_dhkey_complete {
    uint8_t status;
    uint8_t dhkey[32];
} __packed;

#define BT_HCI_EVT_LE_ENH_CONN_COMPLETE         0x0a
struct bt_hci_evt_le_enh_conn_complete {
    uint8_t      status;
    uint16_t     handle;
    uint8_t      role;
    bt_addr_le_t peer_addr;
    bt_addr_t    local_rpa;
    bt_addr_t    peer_rpa;
    uint16_t     interval;
    uint16_t     latency;
    uint16_t     supv_timeout;
    uint8_t      clock_accuracy;
} __packed;

#define BT_HCI_EVT_LE_DIRECT_ADV_REPORT         0x0b
struct bt_hci_evt_le_direct_adv_info {
    uint8_t      evt_type;
    bt_addr_le_t addr;
    bt_addr_le_t dir_addr;
    int8_t       rssi;
} __packed;
struct bt_hci_evt_le_direct_adv_report {
    uint8_t num_reports;
    struct bt_hci_evt_le_direct_adv_info direct_adv_info[0];
} __packed;

#define BT_HCI_EVT_LE_PHY_UPDATE_COMPLETE       0x0c
struct bt_hci_evt_le_phy_update_complete {
    uint8_t  status;
    uint16_t handle;
    uint8_t  tx_phy;
    uint8_t  rx_phy;
} __packed;

#define BT_HCI_EVT_LE_EXT_ADVERTISING_REPORT    0x0d

#define BT_HCI_LE_ADV_EVT_TYPE_CONN                 BIT(0)
#define BT_HCI_LE_ADV_EVT_TYPE_SCAN                 BIT(1)
#define BT_HCI_LE_ADV_EVT_TYPE_DIRECT               BIT(2)
#define BT_HCI_LE_ADV_EVT_TYPE_SCAN_RSP             BIT(3)
#define BT_HCI_LE_ADV_EVT_TYPE_LEGACY               BIT(4)

#define BT_HCI_LE_ADV_EVT_TYPE_DATA_STATUS(ev_type) (((ev_type) >> 5) & 0x03)
#define BT_HCI_LE_ADV_EVT_TYPE_DATA_STATUS_COMPLETE   0
#define BT_HCI_LE_ADV_EVT_TYPE_DATA_STATUS_PARTIAL    1
#define BT_HCI_LE_ADV_EVT_TYPE_DATA_STATUS_INCOMPLETE 2
#define BT_HCI_LE_ADV_EVT_TYPE_DATA_STATUS_RX_FAILED  0xFF

struct bt_hci_evt_le_ext_advertising_info {
    uint16_t     evt_type;
    bt_addr_le_t addr;
    uint8_t      prim_phy;
    uint8_t      sec_phy;
    uint8_t      sid;
    int8_t       tx_power;
    int8_t       rssi;
    uint16_t     interval;
    bt_addr_le_t direct_addr;
    uint8_t      length;
    uint8_t      data[0];
} __packed;
struct bt_hci_evt_le_ext_advertising_report {
    uint8_t num_reports;
    struct bt_hci_evt_le_ext_advertising_info adv_info[0];
} __packed;

#define BT_HCI_EVT_LE_PER_ADV_SYNC_ESTABLISHED  0x0e
struct bt_hci_evt_le_per_adv_sync_established {
    uint8_t      status;
    uint16_t     handle;
    uint8_t      sid;
    bt_addr_le_t adv_addr;
    uint8_t      phy;
    uint16_t     interval;
    uint8_t      clock_accuracy;
} __packed;

#define BT_HCI_EVT_LE_PER_ADVERTISING_REPORT    0x0f
struct bt_hci_evt_le_per_advertising_report {
    uint16_t handle;
    int8_t   tx_power;
    int8_t   rssi;
    uint8_t  cte_type;
    uint8_t  data_status;
    uint8_t  length;
    uint8_t  data[0];
} __packed;

#define BT_HCI_EVT_LE_PER_ADV_SYNC_LOST         0x10
struct bt_hci_evt_le_per_adv_sync_lost {
    uint16_t handle;
} __packed;

#define BT_HCI_EVT_LE_SCAN_TIMEOUT              0x11

#define BT_HCI_EVT_LE_ADV_SET_TERMINATED        0x12
struct bt_hci_evt_le_adv_set_terminated {
    uint8_t  status;
    uint8_t  adv_handle;
    uint16_t conn_handle;
    uint8_t  num_completed_ext_adv_evts;
} __packed;

#define BT_HCI_EVT_LE_SCAN_REQ_RECEIVED         0x13
struct bt_hci_evt_le_scan_req_received {
    uint8_t      handle;
    bt_addr_le_t addr;
} __packed;

#define BT_HCI_LE_CHAN_SEL_ALGO_1               0x00
#define BT_HCI_LE_CHAN_SEL_ALGO_2               0x01

#define BT_HCI_EVT_LE_CHAN_SEL_ALGO             0x14
struct bt_hci_evt_le_chan_sel_algo {
    uint16_t handle;
    uint8_t  chan_sel_algo;
} __packed;

#define BT_HCI_LE_CTE_CRC_OK                    0x0
#define BT_HCI_LE_CTE_CRC_ERR_CTE_BASED_TIME    0x1
#define BT_HCI_LE_CTE_CRC_ERR_CTE_BASED_OTHER   0x2
#define BT_HCI_LE_CTE_INSUFFICIENT_RESOURCES    0xFF

#define B_HCI_LE_CTE_REPORT_SAMPLE_COUNT_MIN    0x9
#define B_HCI_LE_CTE_REPORT_SAMPLE_COUNT_MAX    0x52

#define BT_HCI_LE_CTE_REPORT_NO_VALID_SAMPLE    0x80

#define BT_HCI_EVT_LE_CONNECTIONLESS_IQ_REPORT  0x15
struct bt_hci_le_iq_sample {
    int8_t i;
    int8_t q;
};

struct bt_hci_evt_le_connectionless_iq_report {
    uint16_t sync_handle;
    uint8_t  chan_idx;
    int16_t  rssi;
    uint8_t  rssi_ant_id;
    uint8_t  cte_type;
    uint8_t  slot_durations;
    uint8_t  packet_status;
    uint16_t per_evt_counter;
    uint8_t  sample_count;
    struct bt_hci_le_iq_sample sample[0];
} __packed;

#define BT_HCI_EVT_LE_CONNECTION_IQ_REPORT      0x16
struct bt_hci_evt_le_connection_iq_report {
    uint16_t conn_handle;
    uint8_t  rx_phy;
    uint8_t  data_chan_idx;
    int16_t  rssi;
    uint8_t  rssi_ant_id;
    uint8_t  cte_type;
    uint8_t  slot_durations;
    uint8_t  packet_status;
    uint16_t conn_evt_counter;
    uint8_t  sample_count;
    struct bt_hci_le_iq_sample sample[0];
} __packed;

#define BT_HCI_CTE_REQ_STATUS_RSP_WITHOUT_CTE  0x0

#define BT_HCI_EVT_LE_CTE_REQUEST_FAILED       0x17
struct bt_hci_evt_le_cte_req_failed {
    /* According to BT 5.3 Core Spec the status field may have following
     * values:
     * - BT_HCI_CTE_REQ_STATUS_RSP_WITHOUT_CTE when received LL_CTE_RSP_PDU without CTE.
     * - Other Controller error code for peer rejected request.
     */
    uint8_t  status;
    uint16_t conn_handle;
} __packed;

#define BT_HCI_EVT_LE_PAST_RECEIVED                0x18
struct bt_hci_evt_le_past_received {
    uint8_t      status;
    uint16_t     conn_handle;
    uint16_t     service_data;
    uint16_t     sync_handle;
    uint8_t      adv_sid;
    bt_addr_le_t addr;
    uint8_t      phy;
    uint16_t     interval;
    uint8_t      clock_accuracy;
} __packed;

#define BT_HCI_EVT_LE_CIS_ESTABLISHED           0x19
struct bt_hci_evt_le_cis_established {
    uint8_t  status;
    uint16_t conn_handle;
    uint8_t  cig_sync_delay[3];
    uint8_t  cis_sync_delay[3];
    uint8_t  c_latency[3];
    uint8_t  p_latency[3];
    uint8_t  c_phy;
    uint8_t  p_phy;
    uint8_t  nse;
    uint8_t  c_bn;
    uint8_t  p_bn;
    uint8_t  c_ft;
    uint8_t  p_ft;
    uint16_t c_max_pdu;
    uint16_t p_max_pdu;
    uint16_t interval;
} __packed;

#define BT_HCI_EVT_LE_CIS_REQ                   0x1a
struct bt_hci_evt_le_cis_req {
    uint16_t acl_handle;
    uint16_t cis_handle;
    uint8_t  cig_id;
    uint8_t  cis_id;
} __packed;

#define BT_HCI_EVT_LE_BIG_COMPLETE              0x1b
struct bt_hci_evt_le_big_complete {
    uint8_t  status;
    uint8_t  big_handle;
    uint8_t  sync_delay[3];
    uint8_t  latency[3];
    uint8_t  phy;
    uint8_t  nse;
    uint8_t  bn;
    uint8_t  pto;
    uint8_t  irc;
    uint16_t max_pdu;
    uint16_t iso_interval;
    uint8_t  num_bis;
    uint16_t handle[0];
} __packed;

#define BT_HCI_EVT_LE_BIG_TERMINATE             0x1c
struct bt_hci_evt_le_big_terminate {
    uint8_t  big_handle;
    uint8_t  reason;
} __packed;

#define BT_HCI_EVT_LE_BIG_SYNC_ESTABLISHED      0x1d
struct bt_hci_evt_le_big_sync_established {
    uint8_t  status;
    uint8_t  big_handle;
    uint8_t  latency[3];
    uint8_t  nse;
    uint8_t  bn;
    uint8_t  pto;
    uint8_t  irc;
    uint16_t max_pdu;
    uint16_t iso_interval;
    uint8_t  num_bis;
    uint16_t handle[0];
} __packed;

#define BT_HCI_EVT_LE_BIG_SYNC_LOST             0x1e
struct bt_hci_evt_le_big_sync_lost {
    uint8_t  big_handle;
    uint8_t  reason;
} __packed;

#define BT_HCI_EVT_LE_REQ_PEER_SCA_COMPLETE     0x1f
struct bt_hci_evt_le_req_peer_sca_complete {
    uint8_t  status;
    uint16_t handle;
    uint8_t  sca;
} __packed;

#define BT_HCI_LE_ZONE_ENTERED_LOW      0x0
#define BT_HCI_LE_ZONE_ENTERED_MIDDLE   0x1
#define BT_HCI_LE_ZONE_ENTERED_HIGH     0x2
#define BT_HCI_LE_PATH_LOSS_UNAVAILABLE 0xFF

#define BT_HCI_EVT_LE_PATH_LOSS_THRESHOLD                   0x20
struct bt_hci_evt_le_path_loss_threshold {
    uint16_t handle;
    uint8_t  current_path_loss;
    uint8_t  zone_entered;
} __packed;

/** Reason for Transmit power reporting.
 */
/* Local Transmit power changed. */
#define BT_HCI_LE_TX_POWER_REPORT_REASON_LOCAL_CHANGED         0x00
/* Remote Transmit power changed. */
#define BT_HCI_LE_TX_POWER_REPORT_REASON_REMOTE_CHANGED        0x01
/* HCI_LE_Read_Remote_Transmit_Power_Level command completed. */
#define BT_HCI_LE_TX_POWER_REPORT_REASON_READ_REMOTE_COMPLETED 0x02

#define BT_HCI_EVT_LE_TRANSMIT_POWER_REPORT     0x21
struct bt_hci_evt_le_transmit_power_report {
    uint8_t  status;
    uint16_t handle;
    uint8_t  reason;
    uint8_t  phy;
    int8_t   tx_power_level;
    uint8_t  tx_power_level_flag;
    int8_t   delta;
} __packed;

#define BT_HCI_EVT_LE_BIGINFO_ADV_REPORT        0x22
struct bt_hci_evt_le_biginfo_adv_report {
    uint16_t sync_handle;
    uint8_t  num_bis;
    uint8_t  nse;
    uint16_t iso_interval;
    uint8_t  bn;
    uint8_t  pto;
    uint8_t  irc;
    uint16_t max_pdu;
    uint8_t  sdu_interval[3];
    uint16_t max_sdu;
    uint8_t  phy;
    uint8_t  framing;
    uint8_t  encryption;
} __packed;

/** All limits according to BT Core Spec v5.4 [Vol 4, Part E]. */
#define BT_HCI_LE_SUBRATE_FACTOR_MIN   0x0001
#define BT_HCI_LE_SUBRATE_FACTOR_MAX   0x01f4
#define BT_HCI_LE_CONTINUATION_NUM_MAX 0x01f3

#define BT_HCI_EVT_LE_SUBRATE_CHANGE            0x23
struct bt_hci_evt_le_subrate_change {
    uint8_t status;
    uint16_t handle;
    uint16_t subrate_factor;
    uint16_t peripheral_latency;
    uint16_t continuation_number;
    uint16_t supervision_timeout;
} __packed;

#define BT_HCI_EVT_LE_CIS_ESTABLISHED_V2 0x2a
struct bt_hci_evt_le_cis_established_v2 {
    uint8_t  status;
    uint16_t conn_handle;
    uint8_t  cig_sync_delay[3];
    uint8_t  cis_sync_delay[3];
    uint8_t  c_latency[3];
    uint8_t  p_latency[3];
    uint8_t  c_phy;
    uint8_t  p_phy;
    uint8_t  nse;
    uint8_t  c_bn;
    uint8_t  p_bn;
    uint8_t  c_ft;
    uint8_t  p_ft;
    uint16_t c_max_pdu;
    uint16_t p_max_pdu;
    uint16_t interval;
    uint8_t  sub_interval[3];
    uint16_t c_max_sdu;
    uint16_t p_max_sdu;
    uint8_t  c_sdu_interval[3];
    uint8_t  p_sdu_interval[3];
    uint8_t  framing;
} __packed;

#define BT_HCI_EVT_LE_READ_ALL_REMOTE_FEAT_COMPLETE 0x2b

#define BT_HCI_LE_FEATURE_PAGE_MAX          10
#define BT_HCI_LE_BYTES_PER_FEATURE_PAGE    24
#define BT_HCI_LE_BYTES_PAGE_0_FEATURE_PAGE 8

struct bt_hci_evt_le_read_all_remote_feat_complete {
    uint8_t  status;
    uint16_t handle;
    uint8_t  max_remote_page;
    uint8_t  max_valid_page;
    uint8_t  features[248];
} __packed;

#define BT_HCI_LE_CS_INITIATOR_ROLE_MASK BIT(0)
#define BT_HCI_LE_CS_REFLECTOR_ROLE_MASK BIT(1)

#define BT_HCI_LE_CS_MODES_SUPPORTED_MODE_3_MASK BIT(0)

#define BT_HCI_LE_CS_RTT_AA_ONLY_N_10NS_MASK        BIT(0)
#define BT_HCI_LE_CS_RTT_SOUNDING_N_10NS_MASK       BIT(1)
#define BT_HCI_LE_CS_RTT_RANDOM_PAYLOAD_N_10NS_MASK BIT(2)

#define BT_HCI_LE_CS_NADM_SOUNDING_CAPABILITY_PHASE_BASED_MASK BIT(0)
#define BT_HCI_LE_CS_NADM_RANDOM_CAPABILITY_PHASE_BASED_MASK BIT(0)

#define BT_HCI_LE_CS_SYNC_PHYS_2M_MASK BIT(1)
#define BT_HCI_LE_CS_SYNC_PHYS_2M_2BT_MASK BIT(2)

#define BT_HCI_LE_CS_SUBFEATURE_NO_TX_FAE_MASK BIT(1)
#define BT_HCI_LE_CS_SUBFEATURE_CHSEL_ALG_3C_MASK BIT(2)
#define BT_HCI_LE_CS_SUBFEATURE_PBR_FROM_RTT_SOUNDING_SEQ_MASK BIT(3)

#define BT_HCI_LE_CS_T_IP1_TIME_10US_MASK BIT(0)
#define BT_HCI_LE_CS_T_IP1_TIME_20US_MASK BIT(1)
#define BT_HCI_LE_CS_T_IP1_TIME_30US_MASK BIT(2)
#define BT_HCI_LE_CS_T_IP1_TIME_40US_MASK BIT(3)
#define BT_HCI_LE_CS_T_IP1_TIME_50US_MASK BIT(4)
#define BT_HCI_LE_CS_T_IP1_TIME_60US_MASK BIT(5)
#define BT_HCI_LE_CS_T_IP1_TIME_80US_MASK BIT(6)

#define BT_HCI_LE_CS_T_IP2_TIME_10US_MASK BIT(0)
#define BT_HCI_LE_CS_T_IP2_TIME_20US_MASK BIT(1)
#define BT_HCI_LE_CS_T_IP2_TIME_30US_MASK BIT(2)
#define BT_HCI_LE_CS_T_IP2_TIME_40US_MASK BIT(3)
#define BT_HCI_LE_CS_T_IP2_TIME_50US_MASK BIT(4)
#define BT_HCI_LE_CS_T_IP2_TIME_60US_MASK BIT(5)
#define BT_HCI_LE_CS_T_IP2_TIME_80US_MASK BIT(6)

#define BT_HCI_LE_CS_T_FCS_TIME_15US_MASK   BIT(0)
#define BT_HCI_LE_CS_T_FCS_TIME_20US_MASK   BIT(1)
#define BT_HCI_LE_CS_T_FCS_TIME_30US_MASK   BIT(2)
#define BT_HCI_LE_CS_T_FCS_TIME_40US_MASK   BIT(3)
#define BT_HCI_LE_CS_T_FCS_TIME_50US_MASK   BIT(4)
#define BT_HCI_LE_CS_T_FCS_TIME_60US_MASK   BIT(5)
#define BT_HCI_LE_CS_T_FCS_TIME_80US_MASK   BIT(6)
#define BT_HCI_LE_CS_T_FCS_TIME_100US_MASK  BIT(7)
#define BT_HCI_LE_CS_T_FCS_TIME_1200US_MASK BIT(8)

#define BT_HCI_LE_CS_T_PM_TIME_10US_MASK BIT(0)
#define BT_HCI_LE_CS_T_PM_TIME_20US_MASK BIT(1)

#define BT_HCI_LE_CS_TX_SNR_CAPABILITY_18DB_MASK BIT(0)
#define BT_HCI_LE_CS_TX_SNR_CAPABILITY_21DB_MASK BIT(1)
#define BT_HCI_LE_CS_TX_SNR_CAPABILITY_24DB_MASK BIT(2)
#define BT_HCI_LE_CS_TX_SNR_CAPABILITY_27DB_MASK BIT(3)
#define BT_HCI_LE_CS_TX_SNR_CAPABILITY_30DB_MASK BIT(4)

#define BT_HCI_EVT_LE_CS_READ_REMOTE_SUPPORTED_CAPABILITIES_COMPLETE 0x2C
struct bt_hci_evt_le_cs_read_remote_supported_capabilities_complete {
    uint8_t status;
    uint16_t conn_handle;
    uint8_t num_config_supported;
    uint16_t max_consecutive_procedures_supported;
    uint8_t num_antennas_supported;
    uint8_t max_antenna_paths_supported;
    uint8_t roles_supported;
    uint8_t modes_supported;
    uint8_t rtt_capability;
    uint8_t rtt_aa_only_n;
    uint8_t rtt_sounding_n;
    uint8_t rtt_random_payload_n;
    uint16_t nadm_sounding_capability;
    uint16_t nadm_random_capability;
    uint8_t cs_sync_phys_supported;
    uint16_t subfeatures_supported;
    uint16_t t_ip1_times_supported;
    uint16_t t_ip2_times_supported;
    uint16_t t_fcs_times_supported;
    uint16_t t_pm_times_supported;
    uint8_t t_sw_time_supported;
    uint8_t tx_snr_capability;
} __packed;

#define BT_HCI_EVT_LE_CS_READ_REMOTE_FAE_TABLE_COMPLETE 0x2D
struct bt_hci_evt_le_cs_read_remote_fae_table_complete {
    uint8_t status;
    uint16_t conn_handle;
    uint8_t remote_fae_table[72];
} __packed;

#define BT_HCI_LE_CS_CONFIG_ACTION_REMOVED 0x00
#define BT_HCI_LE_CS_CONFIG_ACTION_CREATED 0x01

#define BT_HCI_EVT_LE_CS_SECURITY_ENABLE_COMPLETE 0x2E
struct bt_hci_evt_le_cs_security_enable_complete {
    uint8_t status;
    uint16_t handle;
} __packed;

#define BT_HCI_EVT_LE_CS_CONFIG_COMPLETE 0x2F
struct bt_hci_evt_le_cs_config_complete {
    uint8_t status;
    uint16_t handle;
    uint8_t config_id;
    uint8_t action;
    uint8_t main_mode_type;
    uint8_t sub_mode_type;
    uint8_t min_main_mode_steps;
    uint8_t max_main_mode_steps;
    uint8_t main_mode_repetition;
    uint8_t mode_0_steps;
    uint8_t role;
    uint8_t rtt_type;
    uint8_t cs_sync_phy;
    uint8_t channel_map[10];
    uint8_t channel_map_repetition;
    uint8_t channel_selection_type;
    uint8_t ch3c_shape;
    uint8_t ch3c_jump;
    uint8_t reserved;
    uint8_t t_ip1_time;
    uint8_t t_ip2_time;
    uint8_t t_fcs_time;
    uint8_t t_pm_time;
} __packed;

#define BT_HCI_LE_CS_TEST_CONN_HANDLE 0x0FFF

#define BT_HCI_LE_CS_PROCEDURE_DONE_STATUS_COMPLETE 0x0
#define BT_HCI_LE_CS_PROCEDURE_DONE_STATUS_PARTIAL  0x1
#define BT_HCI_LE_CS_PROCEDURE_DONE_STATUS_ABORTED  0xF

#define BT_HCI_LE_CS_SUBEVENT_DONE_STATUS_COMPLETE 0x0
#define BT_HCI_LE_CS_SUBEVENT_DONE_STATUS_PARTIAL  0x1
#define BT_HCI_LE_CS_SUBEVENT_DONE_STATUS_ABORTED  0xF

#define BT_HCI_LE_CS_PROCEDURE_ABORT_REASON_NO_ABORT                     0x0
#define BT_HCI_LE_CS_PROCEDURE_ABORT_REASON_LOCAL_HOST_OR_REMOTE_REQUEST 0x1
#define BT_HCI_LE_CS_PROCEDURE_ABORT_REASON_TOO_FEW_CHANNELS             0x2
#define BT_HCI_LE_CS_PROCEDURE_ABORT_REASON_CHMAP_INSTANT_PASSED         0x3
#define BT_HCI_LE_CS_PROCEDURE_ABORT_REASON_UNSPECIFIED                  0xF

#define BT_HCI_LE_CS_SUBEVENT_ABORT_REASON_NO_ABORT                     0x0
#define BT_HCI_LE_CS_SUBEVENT_ABORT_REASON_LOCAL_HOST_OR_REMOTE_REQUEST 0x1
#define BT_HCI_LE_CS_SUBEVENT_ABORT_REASON_NO_CS_SYNC_RECEIVED          0x2
#define BT_HCI_LE_CS_SUBEVENT_ABORT_REASON_SCHED_CONFLICT               0x3
#define BT_HCI_LE_CS_SUBEVENT_ABORT_REASON_UNSPECIFIED                  0xF

#define BT_HCI_LE_CS_SUBEVENT_RESULT_N_AP_IGNORED 0x00
#define BT_HCI_LE_CS_SUBEVENT_RESULT_N_AP_1       0x01
#define BT_HCI_LE_CS_SUBEVENT_RESULT_N_AP_2       0x02
#define BT_HCI_LE_CS_SUBEVENT_RESULT_N_AP_3       0x03
#define BT_HCI_LE_CS_SUBEVENT_RESULT_N_AP_4       0x04

#define BT_HCI_LE_CS_SUBEVENT_RESULT_FREQ_COMPENSATION_NOT_AVAILABLE 0xC000

#define BT_HCI_LE_CS_SUBEVENT_RESULT_PCT_NOT_AVAILABLE 0xFFFFFFFF

#define BT_HCI_LE_CS_REF_POWER_LEVEL_UNAVAILABLE 0x7F

#define BT_HCI_LE_CS_PCT_I_MASK 0x000FFF
#define BT_HCI_LE_CS_PCT_Q_MASK 0xFFF000

#define BT_HCI_LE_CS_TONE_QUALITY_HIGH        0x0
#define BT_HCI_LE_CS_TONE_QUALITY_MED         0x1
#define BT_HCI_LE_CS_TONE_QUALITY_LOW         0x2
#define BT_HCI_LE_CS_TONE_QUALITY_UNAVAILABLE 0x3

#define BT_HCI_LE_CS_NOT_TONE_EXT_SLOT              0x0
#define BT_HCI_LE_CS_TONE_EXT_SLOT_EXT_NOT_EXPECTED 0x1
#define BT_HCI_LE_CS_TONE_EXT_SLOT_EXT_EXPECTED     0x2

#define BT_HCI_LE_CS_TIME_DIFFERENCE_NOT_AVAILABLE 0x8000

#define BT_HCI_LE_CS_PACKET_NADM_ATTACK_EXT_UNLIKELY  0x00
#define BT_HCI_LE_CS_PACKET_NADM_ATTACK_VERY_UNLIKELY 0x01
#define BT_HCI_LE_CS_PACKET_NADM_ATTACK_UNLIKELY      0x02
#define BT_HCI_LE_CS_PACKET_NADM_ATTACK_POSSIBLE      0x03
#define BT_HCI_LE_CS_PACKET_NADM_ATTACK_LIKELY        0x04
#define BT_HCI_LE_CS_PACKET_NADM_ATTACK_VERY_LIKELY   0x05
#define BT_HCI_LE_CS_PACKET_NADM_ATTACK_EXT_LIKELY    0x06
#define BT_HCI_LE_CS_PACKET_NADM_UNKNOWN              0xFF

#define BT_HCI_LE_CS_PACKET_QUALITY_AA_CHECK_SUCCESSFUL       0x0
#define BT_HCI_LE_CS_PACKET_QUALITY_AA_CHECK_BIT_ERRORS_FOUND 0x1
#define BT_HCI_LE_CS_PACKET_QUALITY_AA_CHECK_AA_NOT_FOUND     0x2

#define BT_HCI_LE_CS_PACKET_RSSI_NOT_AVAILABLE 0x7F

#define BT_HCI_EVT_LE_CS_SUBEVENT_RESULT 0x31
/** Subevent result step data format: Mode 0 Initiator  */
struct bt_hci_le_cs_step_data_mode_0_initiator {
#ifdef CONFIG_LITTLE_ENDIAN
    uint8_t packet_quality_aa_check: 4;
    uint8_t packet_quality_bit_errors: 4;
#else
    uint8_t packet_quality_bit_errors: 4;
    uint8_t packet_quality_aa_check: 4;
#endif /* CONFIG_LITTLE_ENDIAN */
    uint8_t packet_rssi;
    uint8_t packet_antenna;
    uint16_t measured_freq_offset;
} __packed;

/** Subevent result step data format: Mode 0 Reflector  */
struct bt_hci_le_cs_step_data_mode_0_reflector {
#ifdef CONFIG_LITTLE_ENDIAN
    uint8_t packet_quality_aa_check: 4;
    uint8_t packet_quality_bit_errors: 4;
#else
    uint8_t packet_quality_bit_errors: 4;
    uint8_t packet_quality_aa_check: 4;
#endif /* CONFIG_LITTLE_ENDIAN */
    uint8_t packet_rssi;
    uint8_t packet_antenna;
} __packed;

/** Subevent result step data format: Mode 1  */
struct bt_hci_le_cs_step_data_mode_1 {
#ifdef CONFIG_LITTLE_ENDIAN
    uint8_t packet_quality_aa_check: 4;
    uint8_t packet_quality_bit_errors: 4;
#else
    uint8_t packet_quality_bit_errors: 4;
    uint8_t packet_quality_aa_check: 4;
#endif /* CONFIG_LITTLE_ENDIAN */
    uint8_t packet_nadm;
    uint8_t packet_rssi;
    union {
        int16_t toa_tod_initiator;
        int16_t tod_toa_reflector;
    };
    uint8_t packet_antenna;
} __packed;

/** Subevent result step data format: Mode 1 with sounding sequence RTT support */
struct bt_hci_le_cs_step_data_mode_1_ss_rtt {
#ifdef CONFIG_LITTLE_ENDIAN
    uint8_t packet_quality_aa_check: 4;
    uint8_t packet_quality_bit_errors: 4;
#else
    uint8_t packet_quality_bit_errors: 4;
    uint8_t packet_quality_aa_check: 4;
#endif /* CONFIG_LITTLE_ENDIAN */
    uint8_t packet_nadm;
    uint8_t packet_rssi;
    union {
        int16_t toa_tod_initiator;
        int16_t tod_toa_reflector;
    };
    uint8_t packet_antenna;
    uint8_t packet_pct1[4];
    uint8_t packet_pct2[4];
} __packed;

/** Format for per-antenna path step data in modes 2 and 3 */
struct bt_hci_le_cs_step_data_tone_info {
    uint8_t phase_correction_term[3];
#ifdef CONFIG_LITTLE_ENDIAN
    uint8_t quality_indicator: 4;
    uint8_t extension_indicator: 4;
#else
    uint8_t extension_indicator: 4;
    uint8_t quality_indicator: 4;
#endif /* CONFIG_LITTLE_ENDIAN */
} __packed;

/** Subevent result step data format: Mode 2 */
struct bt_hci_le_cs_step_data_mode_2 {
    uint8_t antenna_permutation_index;
    struct bt_hci_le_cs_step_data_tone_info tone_info[];
} __packed;

/** Subevent result step data format: Mode 3 */
struct bt_hci_le_cs_step_data_mode_3 {
#ifdef CONFIG_LITTLE_ENDIAN
    uint8_t packet_quality_aa_check: 4;
    uint8_t packet_quality_bit_errors: 4;
#else
    uint8_t packet_quality_bit_errors: 4;
    uint8_t packet_quality_aa_check: 4;
#endif /* CONFIG_LITTLE_ENDIAN */
    uint8_t packet_nadm;
    uint8_t packet_rssi;
    union {
        int16_t toa_tod_initiator;
        int16_t tod_toa_reflector;
    };
    uint8_t packet_antenna;
    uint8_t antenna_permutation_index;
    struct bt_hci_le_cs_step_data_tone_info tone_info[];
} __packed;

/** Subevent result step data format: Mode 3 with sounding sequence RTT support */
struct bt_hci_le_cs_step_data_mode_3_ss_rtt {
#ifdef CONFIG_LITTLE_ENDIAN
    uint8_t packet_quality_aa_check: 4;
    uint8_t packet_quality_bit_errors: 4;
#else
    uint8_t packet_quality_bit_errors: 4;
    uint8_t packet_quality_aa_check: 4;
#endif /* CONFIG_LITTLE_ENDIAN */
    uint8_t packet_nadm;
    uint8_t packet_rssi;
    union {
        int16_t toa_tod_initiator;
        int16_t tod_toa_reflector;
    };
    uint8_t packet_antenna;
    uint8_t packet_pct1[4];
    uint8_t packet_pct2[4];
    uint8_t antenna_permutation_index;
    struct bt_hci_le_cs_step_data_tone_info tone_info[];
} __packed;

struct bt_hci_evt_le_cs_subevent_result_step {
    uint8_t step_mode;
    uint8_t step_channel;
    uint8_t step_data_length;
    uint8_t step_data[];
} __packed;

struct bt_hci_evt_le_cs_subevent_result {
    uint16_t conn_handle;
    uint8_t config_id;
    uint16_t start_acl_conn_event_counter;
    uint16_t procedure_counter;
    uint16_t frequency_compensation;
    uint8_t reference_power_level;
    uint8_t procedure_done_status;
    uint8_t subevent_done_status;
#ifdef CONFIG_LITTLE_ENDIAN
    uint8_t procedure_abort_reason: 4;
    uint8_t subevent_abort_reason: 4;
#else
    uint8_t subevent_abort_reason: 4;
    uint8_t procedure_abort_reason: 4;
#endif /* CONFIG_LITTLE_ENDIAN */
    uint8_t num_antenna_paths;
    uint8_t num_steps_reported;
    uint8_t steps[];
} __packed;

#define BT_HCI_EVT_LE_CS_SUBEVENT_RESULT_CONTINUE 0x32

struct bt_hci_evt_le_cs_subevent_result_continue {
    uint16_t conn_handle;
    uint8_t config_id;
    uint8_t procedure_done_status;
    uint8_t subevent_done_status;
#ifdef CONFIG_LITTLE_ENDIAN
    uint8_t procedure_abort_reason: 4;
    uint8_t subevent_abort_reason: 4;
#else
    uint8_t subevent_abort_reason: 4;
    uint8_t procedure_abort_reason: 4;
#endif /* CONFIG_LITTLE_ENDIAN */
    uint8_t num_antenna_paths;
    uint8_t num_steps_reported;
    uint8_t steps[];
} __packed;

#define BT_HCI_EVT_LE_CS_TEST_END_COMPLETE 0x33
struct bt_hci_evt_le_cs_test_end_complete {
    uint8_t status;
} __packed;

#define BT_HCI_EVT_LE_CS_PROCEDURE_ENABLE_COMPLETE 0x30
struct bt_hci_evt_le_cs_procedure_enable_complete {
    uint8_t status;
    uint16_t handle;
    uint8_t config_id;
    uint8_t state;
    uint8_t tone_antenna_config_selection;
    uint8_t selected_tx_power;
    uint8_t subevent_len[3];
    uint8_t subevents_per_event;
    uint16_t subevent_interval;
    uint16_t event_interval;
    uint16_t procedure_interval;
    uint16_t procedure_count;
    uint16_t max_procedure_len;
} __packed;

/* Event mask bits */

#define BT_EVT_BIT(n) (1ULL << (n))

#define BT_EVT_MASK_INQUIRY_COMPLETE             BT_EVT_BIT(0)
#define BT_EVT_MASK_CONN_COMPLETE                BT_EVT_BIT(2)
#define BT_EVT_MASK_CONN_REQUEST                 BT_EVT_BIT(3)
#define BT_EVT_MASK_DISCONN_COMPLETE             BT_EVT_BIT(4)
#define BT_EVT_MASK_AUTH_COMPLETE                BT_EVT_BIT(5)
#define BT_EVT_MASK_REMOTE_NAME_REQ_COMPLETE     BT_EVT_BIT(6)
#define BT_EVT_MASK_ENCRYPT_CHANGE               BT_EVT_BIT(7)
#define BT_EVT_MASK_REMOTE_FEATURES              BT_EVT_BIT(10)
#define BT_EVT_MASK_REMOTE_VERSION_INFO          BT_EVT_BIT(11)
#define BT_EVT_MASK_HARDWARE_ERROR               BT_EVT_BIT(15)
#define BT_EVT_MASK_ROLE_CHANGE                  BT_EVT_BIT(17)
#define BT_EVT_MASK_PIN_CODE_REQ                 BT_EVT_BIT(21)
#define BT_EVT_MASK_LINK_KEY_REQ                 BT_EVT_BIT(22)
#define BT_EVT_MASK_LINK_KEY_NOTIFY              BT_EVT_BIT(23)
#define BT_EVT_MASK_DATA_BUFFER_OVERFLOW         BT_EVT_BIT(25)
#define BT_EVT_MASK_INQUIRY_RESULT_WITH_RSSI     BT_EVT_BIT(33)
#define BT_EVT_MASK_REMOTE_EXT_FEATURES          BT_EVT_BIT(34)
#define BT_EVT_MASK_SYNC_CONN_COMPLETE           BT_EVT_BIT(43)
#define BT_EVT_MASK_EXTENDED_INQUIRY_RESULT      BT_EVT_BIT(46)
#define BT_EVT_MASK_ENCRYPT_KEY_REFRESH_COMPLETE BT_EVT_BIT(47)
#define BT_EVT_MASK_IO_CAPA_REQ                  BT_EVT_BIT(48)
#define BT_EVT_MASK_IO_CAPA_RESP                 BT_EVT_BIT(49)
#define BT_EVT_MASK_USER_CONFIRM_REQ             BT_EVT_BIT(50)
#define BT_EVT_MASK_USER_PASSKEY_REQ             BT_EVT_BIT(51)
#define BT_EVT_MASK_SSP_COMPLETE                 BT_EVT_BIT(53)
#define BT_EVT_MASK_USER_PASSKEY_NOTIFY          BT_EVT_BIT(58)
#define BT_EVT_MASK_LE_META_EVENT                BT_EVT_BIT(61)

/* Page 2 */
#define BT_EVT_MASK_NUM_COMPLETE_DATA_BLOCKS     BT_EVT_BIT(8)
#define BT_EVT_MASK_TRIGG_CLOCK_CAPTURE          BT_EVT_BIT(14)
#define BT_EVT_MASK_SYNCH_TRAIN_COMPLETE         BT_EVT_BIT(15)
#define BT_EVT_MASK_SYNCH_TRAIN_RX               BT_EVT_BIT(16)
#define BT_EVT_MASK_CL_PER_BC_RX                 BT_EVT_BIT(17)
#define BT_EVT_MASK_CL_PER_BC_TIMEOUT            BT_EVT_BIT(18)
#define BT_EVT_MASK_TRUNC_PAGE_COMPLETE          BT_EVT_BIT(19)
#define BT_EVT_MASK_PER_PAGE_RSP_TIMEOUT         BT_EVT_BIT(20)
#define BT_EVT_MASK_CL_PER_BC_CH_MAP_CHANGE      BT_EVT_BIT(21)
#define BT_EVT_MASK_INQUIRY_RSP_NOT              BT_EVT_BIT(22)
#define BT_EVT_MASK_AUTH_PAYLOAD_TIMEOUT_EXP     BT_EVT_BIT(23)
#define BT_EVT_MASK_SAM_STATUS_CHANGE            BT_EVT_BIT(24)

#define BT_EVT_MASK_LE_CONN_COMPLETE             BT_EVT_BIT(0)
#define BT_EVT_MASK_LE_ADVERTISING_REPORT        BT_EVT_BIT(1)
#define BT_EVT_MASK_LE_CONN_UPDATE_COMPLETE      BT_EVT_BIT(2)
#define BT_EVT_MASK_LE_REMOTE_FEAT_COMPLETE      BT_EVT_BIT(3)
#define BT_EVT_MASK_LE_LTK_REQUEST               BT_EVT_BIT(4)
#define BT_EVT_MASK_LE_CONN_PARAM_REQ            BT_EVT_BIT(5)
#define BT_EVT_MASK_LE_DATA_LEN_CHANGE           BT_EVT_BIT(6)
#define BT_EVT_MASK_LE_P256_PUBLIC_KEY_COMPLETE  BT_EVT_BIT(7)
#define BT_EVT_MASK_LE_GENERATE_DHKEY_COMPLETE   BT_EVT_BIT(8)
#define BT_EVT_MASK_LE_ENH_CONN_COMPLETE         BT_EVT_BIT(9)
#define BT_EVT_MASK_LE_DIRECT_ADV_REPORT         BT_EVT_BIT(10)
#define BT_EVT_MASK_LE_PHY_UPDATE_COMPLETE       BT_EVT_BIT(11)
#define BT_EVT_MASK_LE_EXT_ADVERTISING_REPORT    BT_EVT_BIT(12)
#define BT_EVT_MASK_LE_PER_ADV_SYNC_ESTABLISHED  BT_EVT_BIT(13)
#define BT_EVT_MASK_LE_PER_ADVERTISING_REPORT    BT_EVT_BIT(14)
#define BT_EVT_MASK_LE_PER_ADV_SYNC_LOST         BT_EVT_BIT(15)
#define BT_EVT_MASK_LE_SCAN_TIMEOUT              BT_EVT_BIT(16)
#define BT_EVT_MASK_LE_ADV_SET_TERMINATED        BT_EVT_BIT(17)
#define BT_EVT_MASK_LE_SCAN_REQ_RECEIVED         BT_EVT_BIT(18)
#define BT_EVT_MASK_LE_CHAN_SEL_ALGO             BT_EVT_BIT(19)
#define BT_EVT_MASK_LE_CONNECTIONLESS_IQ_REPORT  BT_EVT_BIT(20)
#define BT_EVT_MASK_LE_CONNECTION_IQ_REPORT      BT_EVT_BIT(21)
#define BT_EVT_MASK_LE_CTE_REQUEST_FAILED        BT_EVT_BIT(22)
#define BT_EVT_MASK_LE_PAST_RECEIVED             BT_EVT_BIT(23)
#define BT_EVT_MASK_LE_CIS_ESTABLISHED           BT_EVT_BIT(24)
#define BT_EVT_MASK_LE_CIS_REQ                   BT_EVT_BIT(25)
#define BT_EVT_MASK_LE_BIG_COMPLETE              BT_EVT_BIT(26)
#define BT_EVT_MASK_LE_BIG_TERMINATED            BT_EVT_BIT(27)
#define BT_EVT_MASK_LE_BIG_SYNC_ESTABLISHED      BT_EVT_BIT(28)
#define BT_EVT_MASK_LE_BIG_SYNC_LOST             BT_EVT_BIT(29)
#define BT_EVT_MASK_LE_REQ_PEER_SCA_COMPLETE     BT_EVT_BIT(30)
#define BT_EVT_MASK_LE_PATH_LOSS_THRESHOLD       BT_EVT_BIT(31)
#define BT_EVT_MASK_LE_TRANSMIT_POWER_REPORTING  BT_EVT_BIT(32)
#define BT_EVT_MASK_LE_BIGINFO_ADV_REPORT        BT_EVT_BIT(33)
#define BT_EVT_MASK_LE_SUBRATE_CHANGE            BT_EVT_BIT(34)

#define BT_EVT_MASK_LE_PER_ADV_SYNC_ESTABLISHED_V2 BT_EVT_BIT(35)
#define BT_EVT_MASK_LE_PER_ADVERTISING_REPORT_V2   BT_EVT_BIT(36)
#define BT_EVT_MASK_LE_PAST_RECEIVED_V2            BT_EVT_BIT(37)
#define BT_EVT_MASK_LE_PER_ADV_SUBEVENT_DATA_REQ   BT_EVT_BIT(38)
#define BT_EVT_MASK_LE_PER_ADV_RESPONSE_REPORT     BT_EVT_BIT(39)
#define BT_EVT_MASK_LE_ENH_CONN_COMPLETE_V2        BT_EVT_BIT(40)

#define BT_EVT_MASK_LE_CS_READ_REMOTE_SUPPORTED_CAPABILITIES_COMPLETE BT_EVT_BIT(43)
#define BT_EVT_MASK_LE_CS_READ_REMOTE_FAE_TABLE_COMPLETE              BT_EVT_BIT(44)
#define BT_EVT_MASK_LE_CS_SECURITY_ENABLE_COMPLETE                    BT_EVT_BIT(45)
#define BT_EVT_MASK_LE_CS_CONFIG_COMPLETE                             BT_EVT_BIT(46)
#define BT_EVT_MASK_LE_CS_PROCEDURE_ENABLE_COMPLETE                   BT_EVT_BIT(47)
#define BT_EVT_MASK_LE_CS_SUBEVENT_RESULT                             BT_EVT_BIT(48)
#define BT_EVT_MASK_LE_CS_SUBEVENT_RESULT_CONTINUE                    BT_EVT_BIT(49)
#define BT_EVT_MASK_LE_CS_TEST_END_COMPLETE                           BT_EVT_BIT(50)

/** HCI Error Codes, BT Core Spec v5.4 [Vol 1, Part F]. */
#define BT_HCI_ERR_SUCCESS                      0x00
#define BT_HCI_ERR_UNKNOWN_CMD                  0x01
#define BT_HCI_ERR_UNKNOWN_CONN_ID              0x02
#define BT_HCI_ERR_HW_FAILURE                   0x03
#define BT_HCI_ERR_PAGE_TIMEOUT                 0x04
#define BT_HCI_ERR_AUTH_FAIL                    0x05
#define BT_HCI_ERR_PIN_OR_KEY_MISSING           0x06
#define BT_HCI_ERR_MEM_CAPACITY_EXCEEDED        0x07
#define BT_HCI_ERR_CONN_TIMEOUT                 0x08
#define BT_HCI_ERR_CONN_LIMIT_EXCEEDED          0x09
#define BT_HCI_ERR_SYNC_CONN_LIMIT_EXCEEDED     0x0a
#define BT_HCI_ERR_CONN_ALREADY_EXISTS          0x0b
#define BT_HCI_ERR_CMD_DISALLOWED               0x0c
#define BT_HCI_ERR_INSUFFICIENT_RESOURCES       0x0d
#define BT_HCI_ERR_INSUFFICIENT_SECURITY        0x0e
#define BT_HCI_ERR_BD_ADDR_UNACCEPTABLE         0x0f
#define BT_HCI_ERR_CONN_ACCEPT_TIMEOUT          0x10
#define BT_HCI_ERR_UNSUPP_FEATURE_PARAM_VAL     0x11
#define BT_HCI_ERR_INVALID_PARAM                0x12
#define BT_HCI_ERR_REMOTE_USER_TERM_CONN        0x13
#define BT_HCI_ERR_REMOTE_LOW_RESOURCES         0x14
#define BT_HCI_ERR_REMOTE_POWER_OFF             0x15
#define BT_HCI_ERR_LOCALHOST_TERM_CONN          0x16
#define BT_HCI_ERR_REPEATED_ATTEMPTS            0x17
#define BT_HCI_ERR_PAIRING_NOT_ALLOWED          0x18
#define BT_HCI_ERR_UNKNOWN_LMP_PDU              0x19
#define BT_HCI_ERR_UNSUPP_REMOTE_FEATURE        0x1a
#define BT_HCI_ERR_SCO_OFFSET_REJECTED          0x1b
#define BT_HCI_ERR_SCO_INTERVAL_REJECTED        0x1c
#define BT_HCI_ERR_SCO_AIR_MODE_REJECTED        0x1d
#define BT_HCI_ERR_INVALID_LL_PARAM             0x1e
#define BT_HCI_ERR_UNSPECIFIED                  0x1f
#define BT_HCI_ERR_UNSUPP_LL_PARAM_VAL          0x20
#define BT_HCI_ERR_ROLE_CHANGE_NOT_ALLOWED      0x21
#define BT_HCI_ERR_LL_RESP_TIMEOUT              0x22
#define BT_HCI_ERR_LL_PROC_COLLISION            0x23
#define BT_HCI_ERR_LMP_PDU_NOT_ALLOWED          0x24
#define BT_HCI_ERR_ENC_MODE_NOT_ACCEPTABLE      0x25
#define BT_HCI_ERR_LINK_KEY_CANNOT_BE_CHANGED   0x26
#define BT_HCI_ERR_REQUESTED_QOS_NOT_SUPPORTED  0x27
#define BT_HCI_ERR_INSTANT_PASSED               0x28
#define BT_HCI_ERR_PAIRING_NOT_SUPPORTED        0x29
#define BT_HCI_ERR_DIFF_TRANS_COLLISION         0x2a
#define BT_HCI_ERR_QOS_UNACCEPTABLE_PARAM       0x2c
#define BT_HCI_ERR_QOS_REJECTED                 0x2d
#define BT_HCI_ERR_CHAN_ASSESS_NOT_SUPPORTED    0x2e
#define BT_HCI_ERR_INSUFF_SECURITY              0x2f
#define BT_HCI_ERR_PARAM_OUT_OF_MANDATORY_RANGE 0x30
#define BT_HCI_ERR_ROLE_SWITCH_PENDING          0x32
#define BT_HCI_ERR_RESERVED_SLOT_VIOLATION      0x34
#define BT_HCI_ERR_ROLE_SWITCH_FAILED           0x35
#define BT_HCI_ERR_EXT_INQ_RESP_TOO_LARGE       0x36
#define BT_HCI_ERR_SIMPLE_PAIR_NOT_SUPP_BY_HOST 0x37
#define BT_HCI_ERR_HOST_BUSY_PAIRING            0x38
#define BT_HCI_ERR_CONN_REJECTED_DUE_TO_NO_CHAN 0x39
#define BT_HCI_ERR_CONTROLLER_BUSY              0x3a
#define BT_HCI_ERR_UNACCEPT_CONN_PARAM          0x3b
#define BT_HCI_ERR_ADV_TIMEOUT                  0x3c
#define BT_HCI_ERR_TERM_DUE_TO_MIC_FAIL         0x3d
#define BT_HCI_ERR_CONN_FAIL_TO_ESTAB           0x3e
#define BT_HCI_ERR_MAC_CONN_FAILED              0x3f
#define BT_HCI_ERR_CLOCK_ADJUST_REJECTED        0x40
#define BT_HCI_ERR_SUBMAP_NOT_DEFINED           0x41
#define BT_HCI_ERR_UNKNOWN_ADV_IDENTIFIER       0x42
#define BT_HCI_ERR_LIMIT_REACHED                0x43
#define BT_HCI_ERR_OP_CANCELLED_BY_HOST         0x44
#define BT_HCI_ERR_PACKET_TOO_LONG              0x45
#define BT_HCI_ERR_TOO_LATE                     0x46
#define BT_HCI_ERR_TOO_EARLY                    0x47

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_HCI_TYPES_H_ */
