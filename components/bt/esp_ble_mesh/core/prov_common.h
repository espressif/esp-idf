/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _PROV_COMMON_H_
#define _PROV_COMMON_H_

#include "mesh/config.h"
#include "mesh/types.h"
#include "mesh/main.h"
#include "mesh/mutex.h"
#include "mesh/timer.h"
#include "mesh/adapter.h"

#include "mesh_v1.1/utils.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AUTH_METHOD_NO_OOB              0x00
#define AUTH_METHOD_STATIC              0x01
#define AUTH_METHOD_OUTPUT              0x02
#define AUTH_METHOD_INPUT               0x03

#define PROV_ERR_NONE                   0x00
#define PROV_ERR_NVAL_PDU               0x01
#define PROV_ERR_NVAL_FMT               0x02
#define PROV_ERR_UNEXP_PDU              0x03
#define PROV_ERR_CFM_FAILED             0x04
#define PROV_ERR_RESOURCES              0x05
#define PROV_ERR_DECRYPT                0x06
#define PROV_ERR_UNEXP_ERR              0x07
#define PROV_ERR_ADDR                   0x08
#define PROV_ERR_NVAL_DATA              0x09

#define PROV_INVITE                     0x00
#define PROV_CAPABILITIES               0x01
#define PROV_START                      0x02
#define PROV_PUB_KEY                    0x03
#define PROV_INPUT_COMPLETE             0x04
#define PROV_CONFIRM                    0x05
#define PROV_RANDOM                     0x06
#define PROV_DATA                       0x07
#define PROV_COMPLETE                   0x08
#define PROV_FAILED                     0x09
#define PROV_REC_REQ                    0x0A
#define PROV_REC_RSP                    0x0B
#define PROV_REC_GET                    0x0C
#define PROV_REC_LIST                   0x0D
/* NOTE: PROV_REC_EXP is specifically defined by Espressif.
 * It indicates that the expected PDU may be PROV_INVITE,
 * PROV_REC_REQ or PROV_REC_GET. */
#define PROV_REC_EXP                    0xFF

#define REC_RSP_SUCCESS                 0x00
#define REC_RSP_REC_NOT_PRESENT         0x01
#define REC_RSP_OFFSET_OUT_OF_BOUND     0x02

#define CERT_BASED_PROV_SUPPORT(oob)    ((oob) & BIT(7))
#define PROV_REC_SUPPORT(oob)           ((oob) & BIT(8))

#if CONFIG_BLE_MESH_PROV_EPA
#define PROV_ENC_SIZE(link)             ((link)->algorithm == PROV_ALG_P256_HMAC_SHA256 ? 32 : 16)
#define PROV_AUTH_SIZE(link)            ((link)->algorithm == PROV_ALG_P256_HMAC_SHA256 ? 32 : 16)
#define PROV_CONF_SIZE(link)            ((link)->algorithm == PROV_ALG_P256_HMAC_SHA256 ? 32 : 16)
#define PROV_RAND_SIZE(link)            ((link)->algorithm == PROV_ALG_P256_HMAC_SHA256 ? 32 : 16)
#define PROV_CONF_SALT_SIZE(link)       ((link)->algorithm == PROV_ALG_P256_HMAC_SHA256 ? 32 : 16)
#define PROV_CONF_KEY_SIZE(link)        ((link)->algorithm == PROV_ALG_P256_HMAC_SHA256 ? 32 : 16)
#define PROV_AUTH_MAX_SIZE              32
#define PROV_CONF_MAX_SIZE              32
#define PROV_RAND_MAX_SIZE              32
#else /* CONFIG_BLE_MESH_PROV_EPA */
#define PROV_ENC_SIZE(link)             16
#define PROV_AUTH_SIZE(link)            16
#define PROV_CONF_SIZE(link)            16
#define PROV_RAND_SIZE(link)            16
#define PROV_CONF_SALT_SIZE(link)       16
#define PROV_CONF_KEY_SIZE(link)        16
#define PROV_AUTH_MAX_SIZE              16
#define PROV_CONF_MAX_SIZE              16
#define PROV_RAND_MAX_SIZE              16
#endif /* CONFIG_BLE_MESH_PROV_EPA */

#define PROV_STATIC_OOB_AVAILABLE       0x00
#define PROV_ONLY_OOB_AUTH_SUPPORT      0x01

#define PROV_NO_OOB_PUB_KEY             0x00
#define PROV_OOB_PUB_KEY                0x01

#define GPCF(gpc)                       ((gpc) & 0x03)
#define GPC_START(last_seg)             (((last_seg) << 2) | 0x00)
#define GPC_ACK                         0x01
#define GPC_CONT(seg_id)                (((seg_id) << 2) | 0x02)
#define GPC_CTL(op)                     (((op) << 2) | 0x03)

#define START_PAYLOAD_MAX               20
#define CONT_PAYLOAD_MAX                23
#define START_LAST_SEG_MAX              2

#define START_LAST_SEG(gpc)             ((gpc) >> 2)
#define CONT_SEG_INDEX(gpc)             ((gpc) >> 2)

#define BEARER_CTL(gpc)                 ((gpc) >> 2)
#define LINK_OPEN                       0x00
#define LINK_ACK                        0x01
#define LINK_CLOSE                      0x02

#define CLOSE_REASON_SUCCESS            0x00
#define CLOSE_REASON_TIMEOUT            0x01
#define CLOSE_REASON_FAILED             0x02

#define PROV_DH_KEY_SIZE                32

#define PROV_CONF_INPUTS_SIZE           145

#define PROV_XACT_NVAL                  0xFF

#define NPPI_DEV_KEY_REFRESH            0x00
#define NPPI_NODE_ADDR_REFRESH          0x01
#define NPPI_NODE_COMP_REFRESH          0x02
#define NPPI_UNKNOWN                    0x03

#if CONFIG_BLE_MESH_FAST_PROV
#define RETRANSMIT_TIMEOUT              K_MSEC(360)
#define TRANSACTION_TIMEOUT             K_SECONDS(3)
#define PROTOCOL_TIMEOUT                K_SECONDS(6)
#else
#define RETRANSMIT_TIMEOUT              K_MSEC(500)
#define TRANSACTION_TIMEOUT             K_SECONDS(30)
#define PROTOCOL_TIMEOUT                K_SECONDS(60)
#endif /* CONFIG_BLE_MESH_FAST_PROV */

#if CONFIG_BLE_MESH_PB_GATT
#define PROV_BUF_HEADROOM               5
#else
#define PROV_BUF_HEADROOM               0
#endif

#define PROV_BUF(name, len) \
        NET_BUF_SIMPLE_DEFINE(name, PROV_BUF_HEADROOM + len)

#define PROV_RX_BUF_SIZE                65

#define BLE_MESH_REC_MAX_ID             0x0013

enum {
    PROV_ALG_P256_CMAC_AES128,
#if CONFIG_BLE_MESH_PROV_EPA
    PROV_ALG_P256_HMAC_SHA256,
#endif
    PROV_ALG_METHOD_MAX_NUM,
};

struct prov_rx {
    uint32_t link_id;
    uint8_t  xact_id;
    uint8_t  gpc;
};

enum {
    CONNECTING,     /* Indicate if PB-GATT connection is in progress (Provisioner) */
    REMOTE_PUB_KEY, /* Remote key has been received (Node & Provisioner) */
    OOB_PUB_KEY,    /* OOB public key is available (Node) */
    LINK_ACTIVE,    /* Link has been opened (Node & Provisioner) */
    WAIT_GEN_DHKEY, /* Waiting for remote public key to generate DHKey (Provisioner) */
    HAVE_DHKEY,     /* DHKey has been calculated (Node & Provisioner) */
    SEND_CONFIRM,   /* Waiting to send Confirm value (Node & Provisioner) */
    WAIT_NUMBER,    /* Waiting for number input from user (Node & Provisioner) */
    WAIT_STRING,    /* Waiting for string input from user (Node & Provisioner) */
    LINK_INVALID,   /* Error occurred during provisioning (Node) */
    LINK_CLOSING,   /* Indicate Link Close is being sent (Provisioner) */
    PB_REMOTE,      /* Indicate if the link is used by PB-Remote */
    PB_NPPI,        /* Indicate if the link is used by NPPI */
    PBR_OPENING,    /* Indicate if the PB-Remote Open Link Procedure is ongoing*/
    WAIT_PK_OBR,    /* Waiting for Remote Provisioning Outbound Report for Public Key */
    PROV_NUM_FLAGS,
};

struct bt_mesh_prov_link {
    BLE_MESH_ATOMIC_DEFINE(flags, PROV_NUM_FLAGS);

    uint8_t expect;             /* Next expected PDU */

    uint8_t public_key;         /* Public Key type */

    uint8_t auth_method;        /* Authentication method */
    uint8_t auth_action;        /* Authentication action */
    uint8_t auth_size;          /* Authentication size */

    uint8_t auth[PROV_AUTH_MAX_SIZE];       /* Authentication Value */
    uint8_t rand[PROV_RAND_MAX_SIZE];       /* Local Random */
    uint8_t conf[PROV_CONF_MAX_SIZE];       /* Remote Confirmation */
    uint8_t local_conf[PROV_CONF_MAX_SIZE]; /* Local Confirmation */

    uint8_t dhkey[32];          /* Calculated DHKey */

    uint8_t algorithm;          /* Provisioning Algorithm */

    uint8_t conf_salt[PROV_CONF_MAX_SIZE];  /* ConfirmationSalt */
    uint8_t conf_key[PROV_CONF_MAX_SIZE];   /* ConfirmationKey */
    uint8_t conf_inputs[145];   /* ConfirmationInputs */
    uint8_t prov_salt[16];      /* Provisioning Salt */

    bt_mesh_addr_t addr;        /* Device address */

#if CONFIG_BLE_MESH_NODE
    bool invite_recv;           /* Indicate if Provisioning Invite is received or not */
#endif /* CONFIG_BLE_MESH_NODE */

#if CONFIG_BLE_MESH_PROVISIONER
    uint8_t  uuid[16];          /* Check if device is being provisioned */
    uint16_t oob_info;          /* OOB info of this device */
    uint8_t  element_num;       /* Element num of device */
    uint8_t  kri_flags;         /* Key refresh flag and iv update flag */
    uint16_t assign_addr;       /* Application assigned address for the device */
    uint16_t unicast_addr;      /* Unicast address allocated for device */

#if CONFIG_BLE_MESH_CERT_BASED_PROV
    uint16_t record_id_expect;  /* The record id field of expected record response PDU */
    uint16_t offset_expect;     /* The offset field of expected record response PDU */
    uint16_t max_size;          /* The maximum size of provisioning record fragment can receive*/

    uint8_t *records[BLE_MESH_REC_MAX_ID];  /* Used to store provisioning records */
#endif /* CONFIG_BLE_MESH_CERT_BASED_PROV */

#if CONFIG_BLE_MESH_PB_ADV
    uint8_t  expect_ack_for;    /* Transaction ACK expected for provisioning pdu */
#endif /* CONFIG_BLE_MESH_PB_ADV */
#endif /* CONFIG_BLE_MESH_PROVISIONER */

#if CONFIG_BLE_MESH_FAST_PROV
    uint8_t  tx_pdu_type;       /* Current transmitted Provisioning PDU type */
    uint8_t  last_tx_pdu;       /* Type of last sent Provisioning PDU */
#endif /* CONFIG_BLE_MESH_FAST_PROV */

#if CONFIG_BLE_MESH_PB_GATT
    struct bt_mesh_conn *conn;  /* GATT connection */
    int    (*pb_gatt_send)(struct bt_mesh_prov_link *link, struct net_buf_simple *msg);
#endif /* CONFIG_BLE_MESH_PB_GATT */

#if CONFIG_BLE_MESH_PB_ADV
    uint32_t link_id;           /* Link ID */
    uint8_t  pending_ack;       /* Decide which transaction id ack is pending */
    uint8_t  reason;            /* Reason of closing provisioning link */

    /* Callback used to get next transaction id */
    uint8_t (*next_xact_id)(struct bt_mesh_prov_link *link);

    /* Callback used to reset PB-ADV link */
    void    (*reset_adv_link)(struct bt_mesh_prov_link *link, uint8_t reason);

    /* Callback used to handle PB-ADV 30s timeout */
    void    (*retrans_timeout)(struct bt_mesh_prov_link *link, uint8_t reason);

    struct {
        uint8_t id;             /* Transaction ID */
        uint8_t prev_id;        /* Previous Transaction ID */
        uint8_t seg;            /* Bit-field of unreceived segments */
        uint8_t last_seg;       /* Last segment (to check length) */
        uint8_t fcs;            /* Expected FCS value */
        struct net_buf_simple *buf; /* Incoming buffer */
    } rx;

    struct {
        int64_t start;          /* Start timestamp of the transaction */
        uint8_t id;             /* Transaction id*/
        struct net_buf *buf[3]; /* Pending outgoing buffer(s) */
        struct k_delayed_work retransmit;   /* Retransmit timer */
    } tx;

    bt_mesh_mutex_t buf_lock;   /* Mutex used to protect PB-ADV buffer */
#endif /* CONFIG_BLE_MESH_PB_ADV */

    uint8_t pb_remote_uuid[16]; /* Device UUID used by PB-Remote */
    uint8_t pb_remote_timeout;  /* Timeout value used by PB-Remote Open Link procedure */
    uint8_t pb_remote_nppi;     /* NPPI Procedure */
    uint8_t pb_remote_pub_key;  /* Public Key used by PB-Remote */
    uint8_t pb_remote_cbd:1,    /* Indicate if the link is closed by the unprovisioned device */
            pb_remote_csp:1,    /* Indicate if the link is close as server can not send pdu */
            pb_remote_reset:1;  /* Indicate if the link is reset */
    void   *pb_remote_data;     /* Remote Provisioning Server/Client PB-Remote information */
    /* Callback used to send Remote Provisioning messages */
    int     (*pb_remote_send)(struct bt_mesh_prov_link *link, struct net_buf_simple *buf);
    /* Callback used to notify Remote Provisioning Server that link is closed */
    void    (*pb_remote_close)(struct bt_mesh_prov_link *link, uint8_t reason);

    struct k_delayed_work prot_timer;   /* Protocol timer */
};

const struct bt_mesh_prov *bt_mesh_prov_get(void);

int bt_mesh_prov_set(const struct bt_mesh_prov *val);

void bt_mesh_prov_buf_init(struct net_buf_simple *buf, uint8_t type);

bt_mesh_output_action_t bt_mesh_prov_output_action(uint8_t action);

bt_mesh_input_action_t bt_mesh_prov_input_action(uint8_t action);

bool bt_mesh_prov_pdu_check(uint8_t type, uint16_t length, uint8_t *reason);

int bt_mesh_prov_send_adv(struct bt_mesh_prov_link *link, struct net_buf_simple *msg);

bool bt_mesh_gen_prov_start(struct bt_mesh_prov_link *link,
                            struct net_buf_simple *buf,
                            struct prov_rx *rx, bool *close);

bool bt_mesh_gen_prov_cont(struct bt_mesh_prov_link *link,
                           struct net_buf_simple *buf,
                           struct prov_rx *rx, bool *close);

void bt_mesh_gen_prov_ack_send(struct bt_mesh_prov_link *link, uint8_t xact_id);

void bt_mesh_prov_clear_tx(struct bt_mesh_prov_link *link, bool cancel);

int bt_mesh_prov_retransmit_init(struct bt_mesh_prov_link *link);

int bt_mesh_prov_bearer_ctl_send(struct bt_mesh_prov_link *link, uint8_t op,
                                 void *data, uint8_t data_len);

int bt_mesh_prov_send(struct bt_mesh_prov_link *link, struct net_buf_simple *buf);

#ifdef __cplusplus
}
#endif

#endif /* _PROV_COMMON_H_ */
