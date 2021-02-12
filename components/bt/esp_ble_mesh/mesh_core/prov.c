/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <errno.h>
#include <string.h>

#include "crypto.h"
#include "adv.h"
#include "mesh.h"
#include "access.h"
#include "foundation.h"
#include "mesh_common.h"
#include "mesh_proxy.h"
#include "proxy_server.h"
#include "prov.h"

#if CONFIG_BLE_MESH_NODE

/* 3 transmissions, 20ms interval */
#define PROV_XMIT              BLE_MESH_TRANSMIT(2, 20)

#define AUTH_METHOD_NO_OOB     0x00
#define AUTH_METHOD_STATIC     0x01
#define AUTH_METHOD_OUTPUT     0x02
#define AUTH_METHOD_INPUT      0x03

#define OUTPUT_OOB_BLINK       0x00
#define OUTPUT_OOB_BEEP        0x01
#define OUTPUT_OOB_VIBRATE     0x02
#define OUTPUT_OOB_NUMBER      0x03
#define OUTPUT_OOB_STRING      0x04

#define INPUT_OOB_PUSH         0x00
#define INPUT_OOB_TWIST        0x01
#define INPUT_OOB_NUMBER       0x02
#define INPUT_OOB_STRING       0x03

#define PUB_KEY_NO_OOB         0x00
#define PUB_KEY_OOB            0x01

#define PROV_ERR_NONE          0x00
#define PROV_ERR_NVAL_PDU      0x01
#define PROV_ERR_NVAL_FMT      0x02
#define PROV_ERR_UNEXP_PDU     0x03
#define PROV_ERR_CFM_FAILED    0x04
#define PROV_ERR_RESOURCES     0x05
#define PROV_ERR_DECRYPT       0x06
#define PROV_ERR_UNEXP_ERR     0x07
#define PROV_ERR_ADDR          0x08

#define PROV_INVITE            0x00
#define PROV_CAPABILITIES      0x01
#define PROV_START             0x02
#define PROV_PUB_KEY           0x03
#define PROV_INPUT_COMPLETE    0x04
#define PROV_CONFIRM           0x05
#define PROV_RANDOM            0x06
#define PROV_DATA              0x07
#define PROV_COMPLETE          0x08
#define PROV_FAILED            0x09

#define PROV_ALG_P256          0x00

#define GPCF(gpc)              (gpc & 0x03)
#define GPC_START(last_seg)    (((last_seg) << 2) | 0x00)
#define GPC_ACK                0x01
#define GPC_CONT(seg_id)       (((seg_id) << 2) | 0x02)
#define GPC_CTL(op)            (((op) << 2) | 0x03)

#define START_PAYLOAD_MAX      20
#define CONT_PAYLOAD_MAX       23

#define START_LAST_SEG(gpc)    (gpc >> 2)
#define CONT_SEG_INDEX(gpc)    (gpc >> 2)

#define BEARER_CTL(gpc)        (gpc >> 2)
#define LINK_OPEN              0x00
#define LINK_ACK               0x01
#define LINK_CLOSE             0x02

#define CLOSE_REASON_SUCCESS   0x00
#define CLOSE_REASON_TIMEOUT   0x01
#define CLOSE_REASON_FAILED    0x02

#define XACT_SEG_DATA(_seg) (&link.rx.buf->data[20 + ((_seg - 1) * 23)])
#define XACT_SEG_RECV(_seg) (link.rx.seg &= ~(1 << (_seg)))

#define XACT_NVAL              0xff

enum {
    REMOTE_PUB_KEY,        /* Remote key has been received */
    OOB_PUB_KEY,           /* OOB public key is available */
    LINK_ACTIVE,           /* Link has been opened */
    HAVE_DHKEY,            /* DHKey has been calculated */
    SEND_CONFIRM,          /* Waiting to send Confirm value */
    WAIT_NUMBER,           /* Waiting for number input from user */
    WAIT_STRING,           /* Waiting for string input from user */
    LINK_INVALID,          /* Error occurred during provisioning */

    NUM_FLAGS,
};

struct prov_link {
    BLE_MESH_ATOMIC_DEFINE(flags, NUM_FLAGS);
#if defined(CONFIG_BLE_MESH_PB_GATT)
    struct bt_mesh_conn *conn;  /* GATT connection */
#endif
    uint8_t  dhkey[32];         /* Calculated DHKey */
    uint8_t  expect;            /* Next expected PDU */

    bool     oob_pk_flag;       /* Flag indicates whether using OOB public key */

    uint8_t  oob_method;
    uint8_t  oob_action;
    uint8_t  oob_size;

    uint8_t  conf[16];          /* Remote Confirmation */
    uint8_t  rand[16];          /* Local Random */
    uint8_t  auth[16];          /* Authentication Value */

    uint8_t  conf_salt[16];     /* ConfirmationSalt */
    uint8_t  conf_key[16];      /* ConfirmationKey */
    uint8_t  conf_inputs[145];  /* ConfirmationInputs */
    uint8_t  prov_salt[16];     /* Provisioning Salt */

#if defined(CONFIG_BLE_MESH_PB_ADV)
    uint32_t id;                /* Link ID */
    uint8_t  tx_pdu_type;       /* The previously transmitted Provisioning PDU type */

    struct {
        uint8_t id;         /* Transaction ID */
        uint8_t prev_id;    /* Previous Transaction ID */
        uint8_t seg;        /* Bit-field of unreceived segments */
        uint8_t last_seg;   /* Last segment (to check length) */
        uint8_t fcs;        /* Expected FCS value */
        struct net_buf_simple *buf;
    } rx;

    struct {
        /* Start timestamp of the transaction */
        int64_t start;

        /* Transaction id*/
        uint8_t id;

        /* Pending outgoing buffer(s) */
        struct net_buf *buf[3];

        /* Retransmit timer */
        struct k_delayed_work retransmit;
    } tx;
#endif

    struct k_delayed_work prot_timer;
};

struct prov_rx {
    uint32_t link_id;
    uint8_t  xact_id;
    uint8_t  gpc;
};

#define BUF_TIMEOUT          K_MSEC(400)

#if defined(CONFIG_BLE_MESH_FAST_PROV)
#define RETRANSMIT_TIMEOUT   K_MSEC(360)
#define TRANSACTION_TIMEOUT  K_SECONDS(3)
#define PROTOCOL_TIMEOUT     K_SECONDS(6)
#else
#define RETRANSMIT_TIMEOUT   K_MSEC(500)
#define TRANSACTION_TIMEOUT  K_SECONDS(30)
#define PROTOCOL_TIMEOUT     K_SECONDS(60)
#endif /* CONFIG_BLE_MESH_FAST_PROV */

#if defined(CONFIG_BLE_MESH_PB_GATT)
#define PROV_BUF_HEADROOM 5
#else
#define PROV_BUF_HEADROOM 0
NET_BUF_SIMPLE_DEFINE_STATIC(rx_buf, 65);
#endif

#define PROV_BUF(name, len) \
    NET_BUF_SIMPLE_DEFINE(name, PROV_BUF_HEADROOM + len)

static struct prov_link link;

static const struct bt_mesh_prov *prov;

#if defined(CONFIG_BLE_MESH_PB_ADV)
static bt_mesh_mutex_t pb_buf_lock;

static inline void bt_mesh_pb_buf_mutex_new(void)
{
    if (!pb_buf_lock.mutex) {
        bt_mesh_mutex_create(&pb_buf_lock);
    }
}

#if CONFIG_BLE_MESH_DEINIT
static inline void bt_mesh_pb_buf_mutex_free(void)
{
    bt_mesh_mutex_free(&pb_buf_lock);
}
#endif /* CONFIG_BLE_MESH_DEINIT */

static inline void bt_mesh_pb_buf_lock(void)
{
    bt_mesh_mutex_lock(&pb_buf_lock);
}

static inline void bt_mesh_pb_buf_unlock(void)
{
    bt_mesh_mutex_unlock(&pb_buf_lock);
}
#endif /* CONFIG_BLE_MESH_PB_ADV */

static void reset_state(void)
{
    k_delayed_work_cancel(&link.prot_timer);

    /* Disable Attention Timer if it was set */
    if (link.conf_inputs[0]) {
        bt_mesh_attention(NULL, 0);
    }

#if defined(CONFIG_BLE_MESH_PB_GATT)
    if (link.conn) {
        bt_mesh_conn_unref(link.conn);
    }
#endif

#if defined(CONFIG_BLE_MESH_PB_ADV)
    /* Clear everything except the retransmit and protocol timer
     * delayed work objects.
     */
    (void)memset(&link, 0, offsetof(struct prov_link, tx.retransmit));
    link.rx.prev_id = XACT_NVAL;

#if defined(CONFIG_BLE_MESH_PB_GATT)
    link.rx.buf = bt_mesh_proxy_server_get_buf();
#else
    net_buf_simple_reset(&rx_buf);
    link.rx.buf = &rx_buf;
#endif /* PB_GATT */

#else /* !PB_ADV */
    /* Clear everything except the protocol timer (k_delayed_work) */
    (void)memset(&link, 0, offsetof(struct prov_link, prot_timer));
#endif /* PB_ADV */
}

#if defined(CONFIG_BLE_MESH_PB_ADV)
static void buf_sent(int err, void *user_data)
{
    if (!link.tx.buf[0]) {
        return;
    }

    k_delayed_work_submit(&link.tx.retransmit, RETRANSMIT_TIMEOUT);
}

static struct bt_mesh_send_cb buf_sent_cb = {
    .end = buf_sent,
};

static void free_segments(void)
{
    int i;

    bt_mesh_pb_buf_lock();

    for (i = 0; i < ARRAY_SIZE(link.tx.buf); i++) {
        struct net_buf *buf = link.tx.buf[i];

        if (!buf) {
            break;
        }

        link.tx.buf[i] = NULL;
        bt_mesh_adv_buf_ref_debug(__func__, buf, 3U, BLE_MESH_BUF_REF_SMALL);
        /* Mark as canceled */
        BLE_MESH_ADV(buf)->busy = 0U;
        net_buf_unref(buf);
    }

    bt_mesh_pb_buf_unlock();
}

static void prov_clear_tx(void)
{
    BT_DBG("%s", __func__);

    k_delayed_work_cancel(&link.tx.retransmit);

    free_segments();
}

static void reset_adv_link(void)
{
    prov_clear_tx();

    if (prov->link_close) {
        prov->link_close(BLE_MESH_PROV_ADV);
    }

#if defined(CONFIG_BLE_MESH_USE_DUPLICATE_SCAN)
    /* Remove the link id from exceptional list */
    bt_mesh_update_exceptional_list(BLE_MESH_EXCEP_LIST_REMOVE,
                                    BLE_MESH_EXCEP_INFO_MESH_LINK_ID, &link.id);
#endif

    reset_state();
}

static struct net_buf *adv_buf_create(void)
{
    struct net_buf *buf = NULL;

    buf = bt_mesh_adv_create(BLE_MESH_ADV_PROV, PROV_XMIT, BUF_TIMEOUT);
    if (!buf) {
        BT_ERR("Out of provisioning buffers");
        return NULL;
    }

    return buf;
}

static uint8_t pending_ack = XACT_NVAL;

static void ack_complete(uint16_t duration, int err, void *user_data)
{
    BT_DBG("xact %u complete", (uint8_t)pending_ack);
    pending_ack = XACT_NVAL;
}

static void gen_prov_ack_send(uint8_t xact_id)
{
    static const struct bt_mesh_send_cb cb = {
        .start = ack_complete,
    };
    const struct bt_mesh_send_cb *complete = NULL;
    struct net_buf *buf = NULL;

    BT_DBG("xact_id %u", xact_id);

    if (pending_ack == xact_id) {
        BT_DBG("Not sending duplicate ack");
        return;
    }

    buf = adv_buf_create();
    if (!buf) {
        return;
    }

    if (pending_ack == XACT_NVAL) {
        pending_ack = xact_id;
        complete = &cb;
    } else {
        complete = NULL;
    }

    net_buf_add_be32(buf, link.id);
    net_buf_add_u8(buf, xact_id);
    net_buf_add_u8(buf, GPC_ACK);

    bt_mesh_adv_send(buf, complete, NULL);
    net_buf_unref(buf);
}

static void send_reliable(void)
{
    int i;

    link.tx.start = k_uptime_get();

    for (i = 0; i < ARRAY_SIZE(link.tx.buf); i++) {
        struct net_buf *buf = link.tx.buf[i];

        if (!buf) {
            break;
        }

        if (i + 1 < ARRAY_SIZE(link.tx.buf) && link.tx.buf[i + 1]) {
            bt_mesh_adv_send(buf, NULL, NULL);
        } else {
            bt_mesh_adv_send(buf, &buf_sent_cb, NULL);
        }
    }
}

static int bearer_ctl_send(uint8_t op, void *data, uint8_t data_len)
{
    struct net_buf *buf = NULL;

    BT_DBG("op 0x%02x data_len %u", op, data_len);

    prov_clear_tx();

    buf = adv_buf_create();
    if (!buf) {
        return -ENOBUFS;
    }

    net_buf_add_be32(buf, link.id);
    /* Transaction ID, always 0 for Bearer messages */
    net_buf_add_u8(buf, 0x00);
    net_buf_add_u8(buf, GPC_CTL(op));
    net_buf_add_mem(buf, data, data_len);

    link.tx.buf[0] = buf;
    send_reliable();

    return 0;
}

static uint8_t last_seg(uint8_t len)
{
    if (len <= START_PAYLOAD_MAX) {
        return 0;
    }

    len -= START_PAYLOAD_MAX;

    return 1 + (len / CONT_PAYLOAD_MAX);
}

static inline uint8_t next_transaction_id(void)
{
    if (link.tx.id != 0U && link.tx.id != 0xFF) {
        return ++link.tx.id;
    }

    link.tx.id = 0x80;
    return link.tx.id;
}

static int prov_send_adv(struct net_buf_simple *msg)
{
    struct net_buf *start = NULL, *buf = NULL;
    uint8_t seg_len = 0U, seg_id = 0U;
    uint8_t xact_id = 0U;
    int32_t timeout = PROTOCOL_TIMEOUT;

    BT_DBG("len %u: %s", msg->len, bt_hex(msg->data, msg->len));

    prov_clear_tx();

    start = adv_buf_create();
    if (!start) {
        return -ENOBUFS;
    }

    xact_id = next_transaction_id();
    net_buf_add_be32(start, link.id);
    net_buf_add_u8(start, xact_id);

    net_buf_add_u8(start, GPC_START(last_seg(msg->len)));
    net_buf_add_be16(start, msg->len);
    net_buf_add_u8(start, bt_mesh_fcs_calc(msg->data, msg->len));

    link.tx.buf[0] = start;
    /* Changed by Espressif, get message type */
    link.tx_pdu_type = msg->data[0];

    seg_len = MIN(msg->len, START_PAYLOAD_MAX);
    BT_DBG("seg 0 len %u: %s", seg_len, bt_hex(msg->data, seg_len));
    net_buf_add_mem(start, msg->data, seg_len);
    net_buf_simple_pull(msg, seg_len);

    buf = start;
    for (seg_id = 1U; msg->len > 0; seg_id++) {
        if (seg_id >= ARRAY_SIZE(link.tx.buf)) {
            BT_ERR("Too big message (seg_id %d)", seg_id);
            free_segments();
            return -E2BIG;
        }

        buf = adv_buf_create();
        if (!buf) {
            free_segments();
            return -ENOBUFS;
        }

        link.tx.buf[seg_id] = buf;

        seg_len = MIN(msg->len, CONT_PAYLOAD_MAX);

        BT_DBG("seg_id %u len %u: %s", seg_id, seg_len,
               bt_hex(msg->data, seg_len));

        net_buf_add_be32(buf, link.id);
        net_buf_add_u8(buf, xact_id);
        net_buf_add_u8(buf, GPC_CONT(seg_id));
        net_buf_add_mem(buf, msg->data, seg_len);
        net_buf_simple_pull(msg, seg_len);
    }

    send_reliable();

    /* Changed by Espressif, add provisioning timeout timer operations.
     * When sending a provisioning PDU successfully, restart the 60s timer.
     */
#if defined(CONFIG_BLE_MESH_FAST_PROV)
    if (link.tx_pdu_type >= PROV_COMPLETE) {
        timeout = K_SECONDS(60);
    }
#endif
    k_delayed_work_submit(&link.prot_timer, timeout);

    return 0;
}

#endif /* CONFIG_BLE_MESH_PB_ADV */

#if defined(CONFIG_BLE_MESH_PB_GATT)
static int prov_send_gatt(struct net_buf_simple *msg)
{
    int err = 0;

    if (!link.conn) {
        return -ENOTCONN;
    }

    /* Changed by Espressif, add provisioning timeout timer operations.
     * When sending a provisioning PDU successfully, restart the 60s timer.
     */
    err = bt_mesh_proxy_server_send(link.conn, BLE_MESH_PROXY_PROV, msg);
    if (err) {
        BT_ERR("Failed to send provisioning PDU");
        return err;
    }

    k_delayed_work_submit(&link.prot_timer, PROTOCOL_TIMEOUT);

    return 0;
}
#endif /* CONFIG_BLE_MESH_PB_GATT */

static inline int prov_send(struct net_buf_simple *buf)
{
#if defined(CONFIG_BLE_MESH_PB_GATT)
    if (link.conn) {
        return prov_send_gatt(buf);
    }
#endif
#if defined(CONFIG_BLE_MESH_PB_ADV)
    return prov_send_adv(buf);
#else
    return 0;
#endif
}

static void prov_buf_init(struct net_buf_simple *buf, uint8_t type)
{
    net_buf_simple_reserve(buf, PROV_BUF_HEADROOM);
    net_buf_simple_add_u8(buf, type);
}

static void prov_send_fail_msg(uint8_t err)
{
    PROV_BUF(buf, 2);

    prov_buf_init(&buf, PROV_FAILED);
    net_buf_simple_add_u8(&buf, err);

    if (prov_send(&buf)) {
        BT_ERR("Failed to send Provisioning Failed message");
    }

    bt_mesh_atomic_set_bit(link.flags, LINK_INVALID);
}

static void prov_invite(const uint8_t *data)
{
    PROV_BUF(buf, 12);

    BT_DBG("Attention Duration: %u seconds", data[0]);

    if (data[0]) {
        bt_mesh_attention(NULL, data[0]);
    }

    link.conf_inputs[0] = data[0];

    prov_buf_init(&buf, PROV_CAPABILITIES);

    /* Number of Elements supported */
    net_buf_simple_add_u8(&buf, bt_mesh_elem_count());

    /* Supported algorithms - FIPS P-256 Eliptic Curve */
    net_buf_simple_add_be16(&buf, BIT(PROV_ALG_P256));

    /* Public Key Type */
    net_buf_simple_add_u8(&buf, prov->oob_pub_key);

    /* Static OOB Type */
    net_buf_simple_add_u8(&buf, prov->static_val ? BIT(0) : 0x00);

    /* Output OOB Size */
    net_buf_simple_add_u8(&buf, prov->output_size);

    /* Output OOB Action */
    net_buf_simple_add_be16(&buf, prov->output_actions);

    /* Input OOB Size */
    net_buf_simple_add_u8(&buf, prov->input_size);

    /* Input OOB Action */
    net_buf_simple_add_be16(&buf, prov->input_actions);

    memcpy(&link.conf_inputs[1], &buf.data[1], 11);

    if (prov_send(&buf)) {
        BT_ERR("Failed to send capabilities");
        return;
    }

    link.expect = PROV_START;
}

static void prov_capabilities(const uint8_t *data)
{
    uint16_t algorithms = 0U, output_action = 0U, input_action = 0U;

    BT_DBG("Elements: %u", data[0]);

    algorithms = sys_get_be16(&data[1]);
    BT_DBG("Algorithms:        %u", algorithms);

    BT_DBG("Public Key Type:   0x%02x", data[3]);
    BT_DBG("Static OOB Type:   0x%02x", data[4]);
    BT_DBG("Output OOB Size:   %u", data[5]);

    output_action = sys_get_be16(&data[6]);
    BT_DBG("Output OOB Action: 0x%04x", output_action);

    BT_DBG("Input OOB Size:    %u", data[8]);

    input_action = sys_get_be16(&data[9]);
    BT_DBG("Input OOB Action:  0x%04x", input_action);

    ((void) algorithms);
    ((void) output_action);
    ((void) input_action);
}

static bt_mesh_output_action_t output_action(uint8_t action)
{
    switch (action) {
    case OUTPUT_OOB_BLINK:
        return BLE_MESH_BLINK;
    case OUTPUT_OOB_BEEP:
        return BLE_MESH_BEEP;
    case OUTPUT_OOB_VIBRATE:
        return BLE_MESH_VIBRATE;
    case OUTPUT_OOB_NUMBER:
        return BLE_MESH_DISPLAY_NUMBER;
    case OUTPUT_OOB_STRING:
        return BLE_MESH_DISPLAY_STRING;
    default:
        return BLE_MESH_NO_OUTPUT;
    }
}

static bt_mesh_input_action_t input_action(uint8_t action)
{
    switch (action) {
    case INPUT_OOB_PUSH:
        return BLE_MESH_PUSH;
    case INPUT_OOB_TWIST:
        return BLE_MESH_TWIST;
    case INPUT_OOB_NUMBER:
        return BLE_MESH_ENTER_NUMBER;
    case INPUT_OOB_STRING:
        return BLE_MESH_ENTER_STRING;
    default:
        return BLE_MESH_NO_INPUT;
    }
}

static int prov_auth(uint8_t method, uint8_t action, uint8_t size)
{
    bt_mesh_output_action_t output = 0U;
    bt_mesh_input_action_t input = 0U;

    switch (method) {
    case AUTH_METHOD_NO_OOB:
        if (action || size) {
            return -EINVAL;
        }

        (void)memset(link.auth, 0, sizeof(link.auth));
        return 0;
    case AUTH_METHOD_STATIC:
        if (action || size) {
            return -EINVAL;
        }

        memcpy(link.auth + 16 - prov->static_val_len,
               prov->static_val, prov->static_val_len);
        (void)memset(link.auth, 0,
                     sizeof(link.auth) - prov->static_val_len);
        return 0;

    case AUTH_METHOD_OUTPUT:
        output = output_action(action);
        if (!output) {
            return -EINVAL;
        }

        if (!(prov->output_actions & output)) {
            return -EINVAL;
        }

        if (size > prov->output_size) {
            return -EINVAL;
        }

        if (output == BLE_MESH_DISPLAY_STRING) {
            unsigned char str[9] = {'\0'};
            uint8_t i = 0U;

            bt_mesh_rand(str, size);

            /* Normalize to '0' .. '9' & 'A' .. 'Z' */
            for (i = 0U; i < size; i++) {
                str[i] %= 36;
                if (str[i] < 10) {
                    str[i] += '0';
                } else {
                    str[i] += 'A' - 10;
                }
            }
            str[size] = '\0';

            memcpy(link.auth, str, size);
            (void)memset(link.auth + size, 0,
                         sizeof(link.auth) - size);

            return prov->output_string((char *)str);
        } else {
            uint32_t div[8] = { 10, 100, 1000, 10000, 100000,
                                1000000, 10000000, 100000000
                              };
            uint32_t num = 0U;

            bt_mesh_rand(&num, sizeof(num));
            num %= div[size - 1];

            sys_put_be32(num, &link.auth[12]);
            (void)memset(link.auth, 0, 12);

            return prov->output_number(output, num);
        }

    case AUTH_METHOD_INPUT:
        input = input_action(action);
        if (!input) {
            return -EINVAL;
        }

        if (!(prov->input_actions & input)) {
            return -EINVAL;
        }

        if (size > prov->input_size) {
            return -EINVAL;
        }

        if (input == BLE_MESH_ENTER_STRING) {
            bt_mesh_atomic_set_bit(link.flags, WAIT_STRING);
        } else {
            bt_mesh_atomic_set_bit(link.flags, WAIT_NUMBER);
        }

        return prov->input(input, size);

    default:
        return -EINVAL;
    }
}

static void prov_start(const uint8_t *data)
{
    BT_INFO("Algorithm:   0x%02x", data[0]);
    BT_INFO("Public Key:  0x%02x", data[1]);
    BT_INFO("Auth Method: 0x%02x", data[2]);
    BT_INFO("Auth Action: 0x%02x", data[3]);
    BT_INFO("Auth Size:   0x%02x", data[4]);

    if (data[0] != PROV_ALG_P256) {
        BT_ERR("Unknown algorithm 0x%02x", data[0]);
        prov_send_fail_msg(PROV_ERR_NVAL_FMT);
        return;
    }

    if (data[1] != prov->oob_pub_key) {
        BT_ERR("Invalid public key type: 0x%02x", data[1]);
        prov_send_fail_msg(PROV_ERR_NVAL_FMT);
        return;
    }

    memcpy(&link.conf_inputs[12], data, 5);

    link.expect = PROV_PUB_KEY;

    /* If Provisioning Start PDU indicates that provisioner chooses
     * OOB public key, then callback to the application layer to let
     * users input public & private key pair.
     */
    link.oob_pk_flag = data[1] ? true : false;
    if (link.oob_pk_flag) {
        prov->oob_pub_key_cb();
    }

    if (prov_auth(data[2], data[3], data[4]) < 0) {
        BT_ERR("Invalid authentication method: 0x%02x; "
               "action: 0x%02x; size: 0x%02x",
               data[2], data[3], data[4]);
        prov_send_fail_msg(PROV_ERR_NVAL_FMT);
    }
}

static void send_confirm(void)
{
    PROV_BUF(cfm, 17);

    BT_DBG("ConfInputs[0]   %s", bt_hex(link.conf_inputs, 64));
    BT_DBG("ConfInputs[64]  %s", bt_hex(&link.conf_inputs[64], 64));
    BT_DBG("ConfInputs[128] %s", bt_hex(&link.conf_inputs[128], 17));

    if (bt_mesh_prov_conf_salt(link.conf_inputs, link.conf_salt)) {
        BT_ERR("Unable to generate confirmation salt");
        prov_send_fail_msg(PROV_ERR_UNEXP_ERR);
        return;
    }

    BT_DBG("ConfirmationSalt: %s", bt_hex(link.conf_salt, 16));

    if (bt_mesh_prov_conf_key(link.dhkey, link.conf_salt, link.conf_key)) {
        BT_ERR("Unable to generate confirmation key");
        prov_send_fail_msg(PROV_ERR_UNEXP_ERR);
        return;
    }

    BT_DBG("ConfirmationKey: %s", bt_hex(link.conf_key, 16));

    if (bt_mesh_rand(link.rand, 16)) {
        BT_ERR("Unable to generate random number");
        prov_send_fail_msg(PROV_ERR_UNEXP_ERR);
        return;
    }

    BT_DBG("LocalRandom: %s", bt_hex(link.rand, 16));

    prov_buf_init(&cfm, PROV_CONFIRM);

    if (bt_mesh_prov_conf(link.conf_key, link.rand, link.auth,
                          net_buf_simple_add(&cfm, 16))) {
        BT_ERR("Unable to generate confirmation value");
        prov_send_fail_msg(PROV_ERR_UNEXP_ERR);
        return;
    }

    if (prov_send(&cfm)) {
        BT_ERR("Unable to send Provisioning Confirm");
        return;
    }

    link.expect = PROV_RANDOM;
}

static void send_input_complete(void)
{
    PROV_BUF(buf, 1);

    prov_buf_init(&buf, PROV_INPUT_COMPLETE);
    if (prov_send(&buf)) {
        BT_ERR("Failed to send Provisioning Input Complete");
    }
}

int bt_mesh_input_number(uint32_t num)
{
    BT_INFO("%u", num);

    if (!bt_mesh_atomic_test_and_clear_bit(link.flags, WAIT_NUMBER)) {
        return -EINVAL;
    }

    sys_put_be32(num, &link.auth[12]);

    send_input_complete();

    if (!bt_mesh_atomic_test_bit(link.flags, HAVE_DHKEY)) {
        return 0;
    }

    if (bt_mesh_atomic_test_and_clear_bit(link.flags, SEND_CONFIRM)) {
        send_confirm();
    }

    return 0;
}

int bt_mesh_input_string(const char *str)
{
    BT_INFO("%s", str);

    if (!bt_mesh_atomic_test_and_clear_bit(link.flags, WAIT_STRING)) {
        return -EINVAL;
    }

    (void)memcpy(link.auth, str, prov->input_size);

    send_input_complete();

    if (!bt_mesh_atomic_test_bit(link.flags, HAVE_DHKEY)) {
        return 0;
    }

    if (bt_mesh_atomic_test_and_clear_bit(link.flags, SEND_CONFIRM)) {
        send_confirm();
    }

    return 0;
}

static void prov_dh_key_cb(const uint8_t key[32], const uint8_t idx)
{
    BT_DBG("%p", key);

    if (!key) {
        BT_ERR("DHKey generation failed");
        prov_send_fail_msg(PROV_ERR_UNEXP_ERR);
        return;
    }

    sys_memcpy_swap(link.dhkey, key, 32);

    BT_DBG("DHkey: %s", bt_hex(link.dhkey, 32));

    bt_mesh_atomic_set_bit(link.flags, HAVE_DHKEY);

    if (bt_mesh_atomic_test_bit(link.flags, WAIT_NUMBER) ||
            bt_mesh_atomic_test_bit(link.flags, WAIT_STRING)) {
        return;
    }

    if (bt_mesh_atomic_test_and_clear_bit(link.flags, SEND_CONFIRM)) {
        send_confirm();
    }
}

static void send_pub_key(void)
{
    PROV_BUF(buf, 65);
    const uint8_t *key = NULL;

    /* Copy remote key in little-endian for bt_mesh_dh_key_gen().
     * X and Y halves are swapped independently. Use response
     * buffer as a temporary storage location. The validating of
     * the remote public key is finished when it is received.
     */
    sys_memcpy_swap(buf.data, &link.conf_inputs[17], 32);
    sys_memcpy_swap(&buf.data[32], &link.conf_inputs[49], 32);

    if (bt_mesh_dh_key_gen(buf.data, prov_dh_key_cb, 0)) {
        BT_ERR("Unable to generate DHKey");
        prov_send_fail_msg(PROV_ERR_UNEXP_ERR);
        return;
    }

    key = bt_mesh_pub_key_get();
    if (!key) {
        BT_ERR("No public key available");
        prov_send_fail_msg(PROV_ERR_UNEXP_ERR);
        return;
    }

    BT_DBG("Local Public Key: %s", bt_hex(key, 64));

    prov_buf_init(&buf, PROV_PUB_KEY);

    /* Swap X and Y halves independently to big-endian */
    sys_memcpy_swap(net_buf_simple_add(&buf, 32), key, 32);
    sys_memcpy_swap(net_buf_simple_add(&buf, 32), &key[32], 32);

    memcpy(&link.conf_inputs[81], &buf.data[1], 64);

    if (prov_send(&buf)) {
        BT_ERR("Failed to send Public Key");
        return;
    }

    link.expect = PROV_CONFIRM;
}

static int bt_mesh_calc_dh_key(void)
{
    NET_BUF_SIMPLE_DEFINE(buf, 64);

    /* Copy remote key in little-endian for bt_mesh_dh_key_gen().
     * X and Y halves are swapped independently.
     */
    net_buf_simple_reset(&buf);
    sys_memcpy_swap(buf.data, &link.conf_inputs[17], 32);
    sys_memcpy_swap(&buf.data[32], &link.conf_inputs[49], 32);

    if (bt_mesh_dh_key_gen(buf.data, prov_dh_key_cb, 0)) {
        BT_ERR("Unable to generate DHKey");
        prov_send_fail_msg(PROV_ERR_UNEXP_ERR);
        return -EIO;
    }

    return 0;
}

int bt_mesh_set_oob_pub_key(const uint8_t pub_key_x[32], const uint8_t pub_key_y[32],
                            const uint8_t pri_key[32])
{
    if (!pub_key_x || !pub_key_y || !pri_key) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    /* Copy OOB public key in big-endian to Provisioning ConfirmationInputs,
     * X and Y halves are swapped independently.
     * And set input private key to mesh_bearer_adapt.c
     */
    sys_memcpy_swap(&link.conf_inputs[81], pub_key_x, 32);
    sys_memcpy_swap(&link.conf_inputs[81] + 32, pub_key_y, 32);
    bt_mesh_set_private_key(pri_key);

    bt_mesh_atomic_set_bit(link.flags, OOB_PUB_KEY);

    /* If remote public key is not got, just return */
    if (!bt_mesh_atomic_test_bit(link.flags, REMOTE_PUB_KEY)) {
        return 0;
    }

    return bt_mesh_calc_dh_key();
}

static void prov_pub_key(const uint8_t *data)
{
    BT_DBG("Remote Public Key: %s", bt_hex(data, 64));

    /* BLE Mesh BQB test case MESH/NODE/PROV/UPD/BI-13-C needs to
     * check the public key using the following rules:
     * (1) X > 0, Y > 0
     * (2) X > 0, Y = 0
     * (3) X = 0, Y = 0
     */
    if (!bt_mesh_check_public_key(data)) {
        BT_ERR("Invalid public key");
        prov_send_fail_msg(PROV_ERR_UNEXP_PDU);
        return;
    }

    memcpy(&link.conf_inputs[17], data, 64);
    bt_mesh_atomic_set_bit(link.flags, REMOTE_PUB_KEY);

    if (!bt_mesh_pub_key_get()) {
        /* Clear retransmit timer */
#if defined(CONFIG_BLE_MESH_PB_ADV)
        prov_clear_tx();
#endif
        BT_WARN("Waiting for a local public key");
        return;
    }

    if (!link.oob_pk_flag) {
        send_pub_key();
    } else {
        link.expect = PROV_CONFIRM;
    }
}

static void prov_input_complete(const uint8_t *data)
{
    BT_DBG("%s", __func__);
}

static void prov_confirm(const uint8_t *data)
{
    BT_DBG("Remote Confirm: %s", bt_hex(data, 16));

    memcpy(link.conf, data, 16);

    if (!bt_mesh_atomic_test_bit(link.flags, HAVE_DHKEY)) {
#if defined(CONFIG_BLE_MESH_PB_ADV)
        prov_clear_tx();
#endif
        bt_mesh_atomic_set_bit(link.flags, SEND_CONFIRM);
        /* If using OOB public key and it has already got, calculates dhkey */
        if (link.oob_pk_flag && bt_mesh_atomic_test_bit(link.flags, OOB_PUB_KEY)) {
            bt_mesh_calc_dh_key();
        }
    } else {
        send_confirm();
    }
}

static void prov_random(const uint8_t *data)
{
    PROV_BUF(rnd, 17);
    uint8_t conf_verify[16] = {0};

    BT_DBG("Remote Random: %s", bt_hex(data, 16));

    if (bt_mesh_prov_conf(link.conf_key, data, link.auth, conf_verify)) {
        BT_ERR("Unable to calculate confirmation verification");
        prov_send_fail_msg(PROV_ERR_UNEXP_ERR);
        return;
    }

    if (memcmp(conf_verify, link.conf, 16)) {
        BT_ERR("Invalid confirmation value");
        BT_DBG("Received:   %s", bt_hex(link.conf, 16));
        BT_DBG("Calculated: %s",  bt_hex(conf_verify, 16));
        prov_send_fail_msg(PROV_ERR_CFM_FAILED);
        return;
    }

    prov_buf_init(&rnd, PROV_RANDOM);
    net_buf_simple_add_mem(&rnd, link.rand, 16);

    if (prov_send(&rnd)) {
        BT_ERR("Failed to send Provisioning Random");
        return;
    }

    if (bt_mesh_prov_salt(link.conf_salt, data, link.rand,
                          link.prov_salt)) {
        BT_ERR("Failed to generate provisioning salt");
        prov_send_fail_msg(PROV_ERR_UNEXP_ERR);
        return;
    }

    BT_DBG("ProvisioningSalt: %s", bt_hex(link.prov_salt, 16));

    link.expect = PROV_DATA;
}

static inline bool is_pb_gatt(void)
{
#if defined(CONFIG_BLE_MESH_PB_GATT)
    return !!link.conn;
#else
    return false;
#endif
}

static void prov_data(const uint8_t *data)
{
    PROV_BUF(msg, 1);
    uint8_t session_key[16] = {0};
    uint8_t nonce[13] = {0};
    uint8_t dev_key[16] = {0};
    uint8_t pdu[25] = {0};
    uint8_t flags = 0U;
    uint32_t iv_index = 0U;
    uint16_t addr = 0U;
    uint16_t net_idx = 0U;
    int err = 0;
    bool identity_enable = false;

    BT_DBG("%s", __func__);

    err = bt_mesh_session_key(link.dhkey, link.prov_salt, session_key);
    if (err) {
        BT_ERR("Unable to generate session key");
        prov_send_fail_msg(PROV_ERR_UNEXP_ERR);
        return;
    }

    BT_DBG("SessionKey: %s", bt_hex(session_key, 16));

    err = bt_mesh_prov_nonce(link.dhkey, link.prov_salt, nonce);
    if (err) {
        BT_ERR("Unable to generate session nonce");
        prov_send_fail_msg(PROV_ERR_UNEXP_ERR);
        return;
    }

    BT_DBG("Nonce: %s", bt_hex(nonce, 13));

    err = bt_mesh_prov_decrypt(session_key, nonce, data, pdu);
    if (err) {
        BT_ERR("Unable to decrypt provisioning data");
        prov_send_fail_msg(PROV_ERR_DECRYPT);
        return;
    }

    err = bt_mesh_dev_key(link.dhkey, link.prov_salt, dev_key);
    if (err) {
        BT_ERR("Unable to generate device key");
        prov_send_fail_msg(PROV_ERR_UNEXP_ERR);
        return;
    }

    BT_DBG("DevKey: %s", bt_hex(dev_key, 16));

    net_idx = sys_get_be16(&pdu[16]);
    flags = pdu[18];
    iv_index = sys_get_be32(&pdu[19]);
    addr = sys_get_be16(&pdu[23]);

    BT_DBG("net_idx %u iv_index 0x%08x, addr 0x%04x",
           net_idx, iv_index, addr);

    prov_buf_init(&msg, PROV_COMPLETE);
    if (prov_send(&msg)) {
        BT_ERR("Failed to send Provisioning Complete");
        return;
    }

    /* Ignore any further PDUs on this link */
    link.expect = 0U;

    /* Store info, since bt_mesh_provision() will end up clearing it */
    if (IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_SERVER)) {
        identity_enable = is_pb_gatt();
    } else {
        identity_enable = false;
    }

    err = bt_mesh_provision(pdu, net_idx, flags, iv_index, addr, dev_key);
    if (err) {
        BT_ERR("Failed to provision (err %d)", err);
        return;
    }

    /* After PB-GATT provisioning we should start advertising
     * using Node Identity.
     */
    if (IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_SERVER) && identity_enable) {
        bt_mesh_proxy_identity_enable();
    }
}

static void prov_complete(const uint8_t *data)
{
    BT_DBG("%s", __func__);
}

static void prov_failed(const uint8_t *data)
{
    BT_WARN("Error: 0x%02x", data[0]);
}

static const struct {
    void (*func)(const uint8_t *data);
    uint16_t len;
} prov_handlers[] = {
    { prov_invite, 1 },
    { prov_capabilities, 11 },
    { prov_start, 5, },
    { prov_pub_key, 64 },
    { prov_input_complete, 0 },
    { prov_confirm, 16 },
    { prov_random, 16 },
    { prov_data, 33 },
    { prov_complete, 0 },
    { prov_failed, 1 },
};

#if defined(CONFIG_BLE_MESH_PB_ADV)
static void prov_retransmit(struct k_work *work)
{
    int64_t timeout = TRANSACTION_TIMEOUT;
    int i;

    BT_DBG("%s", __func__);

    if (!bt_mesh_atomic_test_bit(link.flags, LINK_ACTIVE)) {
        BT_WARN("Link not active");
        return;
    }

#if defined(CONFIG_BLE_MESH_FAST_PROV)
    /* When Provisioning Failed PDU is sent, 3s may be used here. */
    if (link.tx_pdu_type >= PROV_COMPLETE) {
        timeout = K_SECONDS(30);
    }
#endif
    if (k_uptime_get() - link.tx.start > timeout) {
        BT_WARN("Node timeout, giving up transaction");
        reset_adv_link();
        return;
    }

    bt_mesh_pb_buf_lock();

    for (i = 0; i < ARRAY_SIZE(link.tx.buf); i++) {
        struct net_buf *buf = link.tx.buf[i];

        if (!buf) {
            break;
        }

        if (BLE_MESH_ADV(buf)->busy) {
            continue;
        }

        BT_DBG("%u bytes: %s", buf->len, bt_hex(buf->data, buf->len));

        if (i + 1 < ARRAY_SIZE(link.tx.buf) && link.tx.buf[i + 1]) {
            bt_mesh_adv_send(buf, NULL, NULL);
        } else {
            bt_mesh_adv_send(buf, &buf_sent_cb, NULL);
        }

    }

    bt_mesh_pb_buf_unlock();
}

static void link_open(struct prov_rx *rx, struct net_buf_simple *buf)
{
    BT_DBG("len %u", buf->len);

    if (buf->len < 16) {
        BT_ERR("Too short bearer open message (len %u)", buf->len);
        return;
    }

    if (bt_mesh_atomic_test_bit(link.flags, LINK_ACTIVE)) {
        /* Send another link ack if the provisioner missed the last */
        if (link.id == rx->link_id && link.expect == PROV_INVITE) {
            BT_DBG("Resending link ack");
            bearer_ctl_send(LINK_ACK, NULL, 0);
        } else {
            BT_INFO("Ignoring bearer open: link already active");
        }

        return;
    }

    if (memcmp(buf->data, prov->uuid, 16)) {
        BT_DBG("Bearer open message not for us");
        return;
    }

    if (prov->link_open) {
        prov->link_open(BLE_MESH_PROV_ADV);
    }

    link.id = rx->link_id;
    bt_mesh_atomic_set_bit(link.flags, LINK_ACTIVE);
    net_buf_simple_reset(link.rx.buf);

#if defined(CONFIG_BLE_MESH_USE_DUPLICATE_SCAN)
    /* Add the link id into exceptional list */
    bt_mesh_update_exceptional_list(BLE_MESH_EXCEP_LIST_ADD,
                                    BLE_MESH_EXCEP_INFO_MESH_LINK_ID, &link.id);
#endif

    bearer_ctl_send(LINK_ACK, NULL, 0);

    link.expect = PROV_INVITE;
}

static void link_ack(struct prov_rx *rx, struct net_buf_simple *buf)
{
    BT_DBG("len %u", buf->len);
}

static void link_close(struct prov_rx *rx, struct net_buf_simple *buf)
{
    BT_DBG("len %u", buf->len);

    reset_adv_link();
}

static void gen_prov_ctl(struct prov_rx *rx, struct net_buf_simple *buf)
{
    BT_DBG("op 0x%02x len %u", BEARER_CTL(rx->gpc), buf->len);

    switch (BEARER_CTL(rx->gpc)) {
    case LINK_OPEN:
        link_open(rx, buf);
        break;
    case LINK_ACK:
        if (!bt_mesh_atomic_test_bit(link.flags, LINK_ACTIVE)) {
            return;
        }

        link_ack(rx, buf);
        break;
    case LINK_CLOSE:
        if (!bt_mesh_atomic_test_bit(link.flags, LINK_ACTIVE)) {
            return;
        }

        link_close(rx, buf);
        break;
    default:
        BT_ERR("Unknown bearer opcode: 0x%02x", BEARER_CTL(rx->gpc));
        return;
    }
}

static void prov_msg_recv(void)
{
    uint8_t type = link.rx.buf->data[0];

    BT_DBG("type 0x%02x len %u", type, link.rx.buf->len);

    if (!bt_mesh_fcs_check(link.rx.buf, link.rx.fcs)) {
        BT_ERR("Incorrect FCS");
        return;
    }

    gen_prov_ack_send(link.rx.id);
    link.rx.prev_id = link.rx.id;
    link.rx.id = 0U;

    if (bt_mesh_atomic_test_bit(link.flags, LINK_INVALID)) {
        BT_WARN("Unexpected msg 0x%02x on invalidated link", type);
        prov_send_fail_msg(PROV_ERR_UNEXP_PDU);
        return;
    }

    if (type != PROV_FAILED && type != link.expect) {
        BT_WARN("Unexpected msg 0x%02x != 0x%02x", type, link.expect);
        prov_send_fail_msg(PROV_ERR_UNEXP_PDU);
        return;
    }

    if (type >= ARRAY_SIZE(prov_handlers)) {
        BT_ERR("Unknown provisioning PDU type 0x%02x", type);
        prov_send_fail_msg(PROV_ERR_NVAL_PDU);
        return;
    }

    if (1 + prov_handlers[type].len != link.rx.buf->len) {
        BT_ERR("Invalid length %u for type 0x%02x",
                link.rx.buf->len, type);
        prov_send_fail_msg(PROV_ERR_NVAL_FMT);
        return;
    }

    /* Changed by Espressif, add provisioning timeout timer operations.
     * When received a provisioning PDU, restart the 60s timer.
     */
    k_delayed_work_submit(&link.prot_timer, PROTOCOL_TIMEOUT);

    prov_handlers[type].func(&link.rx.buf->data[1]);
}

static void gen_prov_cont(struct prov_rx *rx, struct net_buf_simple *buf)
{
    uint8_t seg = CONT_SEG_INDEX(rx->gpc);

    BT_DBG("len %u, seg_index %u", buf->len, seg);

    if (!link.rx.seg && link.rx.prev_id == rx->xact_id) {
        BT_INFO("Resending ack");
        gen_prov_ack_send(rx->xact_id);
        return;
    }

    if (rx->xact_id != link.rx.id) {
        BT_WARN("Data for unknown transaction (%u != %u)",
                rx->xact_id, link.rx.id);
        return;
    }

    if (seg > link.rx.last_seg) {
        BT_ERR("Invalid segment index %u", seg);
        prov_send_fail_msg(PROV_ERR_NVAL_FMT);
        return;
    } else if (seg == link.rx.last_seg) {
        uint8_t expect_len = 0U;

        expect_len = (link.rx.buf->len - 20U -
                      ((link.rx.last_seg - 1) * 23U));
        if (expect_len != buf->len) {
            BT_ERR("Incorrect last seg len: %u != %u",
                    expect_len, buf->len);
            prov_send_fail_msg(PROV_ERR_NVAL_FMT);
            return;
        }
    }

    if (!(link.rx.seg & BIT(seg))) {
        BT_INFO("Ignoring already received segment");
        return;
    }

    memcpy(XACT_SEG_DATA(seg), buf->data, buf->len);
    XACT_SEG_RECV(seg);

    if (!link.rx.seg) {
        prov_msg_recv();
    }
}

static void gen_prov_ack(struct prov_rx *rx, struct net_buf_simple *buf)
{
    BT_DBG("len %u", buf->len);

    if (!link.tx.buf[0]) {
        return;
    }

    if (rx->xact_id == link.tx.id) {
        prov_clear_tx();
    }
}

static void gen_prov_start(struct prov_rx *rx, struct net_buf_simple *buf)
{
    if (link.rx.seg) {
        BT_INFO("Got Start while there are unreceived segments");
        return;
    }

    if (link.rx.prev_id == rx->xact_id) {
        BT_INFO("Resending ack");
        gen_prov_ack_send(rx->xact_id);
        return;
    }

    link.rx.buf->len = net_buf_simple_pull_be16(buf);
    link.rx.id  = rx->xact_id;
    link.rx.fcs = net_buf_simple_pull_u8(buf);

    BT_DBG("len %u last_seg %u total_len %u fcs 0x%02x", buf->len,
           START_LAST_SEG(rx->gpc), link.rx.buf->len, link.rx.fcs);

    if (link.rx.buf->len < 1) {
        BT_ERR("Ignoring zero-length provisioning PDU");
        prov_send_fail_msg(PROV_ERR_NVAL_FMT);
        return;
    }

    if (link.rx.buf->len > link.rx.buf->size) {
        BT_ERR("Too large provisioning PDU (%u bytes)",
                link.rx.buf->len);
        prov_send_fail_msg(PROV_ERR_NVAL_FMT);
        return;
    }

    if (START_LAST_SEG(rx->gpc) > 0 && link.rx.buf->len <= 20U) {
        BT_ERR("Too small total length for multi-segment PDU");
        prov_send_fail_msg(PROV_ERR_NVAL_FMT);
        return;
    }

    link.rx.seg = (1 << (START_LAST_SEG(rx->gpc) + 1)) - 1;
    link.rx.last_seg = START_LAST_SEG(rx->gpc);
    memcpy(link.rx.buf->data, buf->data, buf->len);
    XACT_SEG_RECV(0);

    if (!link.rx.seg) {
        prov_msg_recv();
    }
}

static const struct {
    void (*func)(struct prov_rx *rx, struct net_buf_simple *buf);
    bool require_link;
    uint8_t min_len;
} gen_prov[] = {
    { gen_prov_start, true, 3 },
    { gen_prov_ack, true, 0 },
    { gen_prov_cont, true, 0 },
    { gen_prov_ctl, false, 0 },
};

static void gen_prov_recv(struct prov_rx *rx, struct net_buf_simple *buf)
{
    if (buf->len < gen_prov[GPCF(rx->gpc)].min_len) {
        BT_ERR("Too short GPC message type %u", GPCF(rx->gpc));
        return;
    }

    if (!bt_mesh_atomic_test_bit(link.flags, LINK_ACTIVE) &&
            gen_prov[GPCF(rx->gpc)].require_link) {
        BT_DBG("Ignoring message that requires active link");
        return;
    }

    gen_prov[GPCF(rx->gpc)].func(rx, buf);
}

void bt_mesh_pb_adv_recv(struct net_buf_simple *buf)
{
    struct prov_rx rx = {0};

    if (!bt_prov_active() && bt_mesh_is_provisioned()) {
        BT_DBG("Ignoring provisioning PDU - already provisioned");
        return;
    }

    if (buf->len < 6) {
        BT_WARN("Too short provisioning packet (len %u)", buf->len);
        return;
    }

    rx.link_id = net_buf_simple_pull_be32(buf);
    rx.xact_id = net_buf_simple_pull_u8(buf);
    rx.gpc = net_buf_simple_pull_u8(buf);

    BT_DBG("link_id 0x%08x xact_id %u", rx.link_id, rx.xact_id);

    if (bt_mesh_atomic_test_bit(link.flags, LINK_ACTIVE) && link.id != rx.link_id) {
        BT_DBG("Ignoring mesh beacon for unknown link");
        return;
    }

    gen_prov_recv(&rx, buf);
}
#endif /* CONFIG_BLE_MESH_PB_ADV */

#if defined(CONFIG_BLE_MESH_PB_GATT)
int bt_mesh_pb_gatt_recv(struct bt_mesh_conn *conn, struct net_buf_simple *buf)
{
    uint8_t type = 0U;

    BT_DBG("%u bytes: %s", buf->len, bt_hex(buf->data, buf->len));

    if (link.conn != conn) {
        BT_WARN("Data for unexpected connection");
        return -ENOTCONN;
    }

    if (buf->len < 1) {
        BT_WARN("Too short provisioning packet (len %u)", buf->len);
        return -EINVAL;
    }

    type = net_buf_simple_pull_u8(buf);
    if (type != PROV_FAILED && type != link.expect) {
        BT_WARN("Unexpected msg 0x%02x != 0x%02x", type, link.expect);
        prov_send_fail_msg(PROV_ERR_UNEXP_PDU);
        return -EINVAL;
    }

    if (type >= ARRAY_SIZE(prov_handlers)) {
        BT_ERR("Unknown provisioning PDU type 0x%02x", type);
        return -EINVAL;
    }

    if (prov_handlers[type].len != buf->len) {
        BT_ERR("Invalid length %u for type 0x%02x", buf->len, type);
        return -EINVAL;
    }

    /* Changed by Espressif, add provisioning timeout timer operations.
     * When received a provisioning PDU, restart the 60s timer.
     */
    k_delayed_work_submit(&link.prot_timer, PROTOCOL_TIMEOUT);

    prov_handlers[type].func(buf->data);

    return 0;
}

int bt_mesh_pb_gatt_open(struct bt_mesh_conn *conn)
{
    BT_DBG("conn %p", conn);

    if (bt_mesh_atomic_test_and_set_bit(link.flags, LINK_ACTIVE)) {
        return -EBUSY;
    }

    link.conn = bt_mesh_conn_ref(conn);
    link.expect = PROV_INVITE;

    if (prov->link_open) {
        prov->link_open(BLE_MESH_PROV_GATT);
    }

    return 0;
}

int bt_mesh_pb_gatt_close(struct bt_mesh_conn *conn)
{
    BT_DBG("conn %p", conn);

    if (link.conn != conn) {
        BT_ERR("Not connected");
        return -ENOTCONN;
    }

    if (prov->link_close) {
        prov->link_close(BLE_MESH_PROV_GATT);
    }

    reset_state();

    return 0;
}
#endif /* CONFIG_BLE_MESH_PB_GATT */

const struct bt_mesh_prov *bt_mesh_prov_get(void)
{
    return prov;
}

bool bt_prov_active(void)
{
    return bt_mesh_atomic_test_bit(link.flags, LINK_ACTIVE);
}

static void protocol_timeout(struct k_work *work)
{
    BT_WARN("Protocol timeout");

#if defined(CONFIG_BLE_MESH_PB_GATT)
    if (link.conn) {
        bt_mesh_pb_gatt_close(link.conn);
        return;
    }
#endif

#if defined(CONFIG_BLE_MESH_PB_ADV)
    uint8_t reason = CLOSE_REASON_TIMEOUT;

    link.rx.seg = 0U;
    bearer_ctl_send(LINK_CLOSE, &reason, sizeof(reason));

    reset_state();
#endif
}

int bt_mesh_prov_init(const struct bt_mesh_prov *prov_info)
{
    const uint8_t *key = NULL;

    if (!prov_info) {
        BT_ERR("No provisioning context provided");
        return -EINVAL;
    }

    if (prov_info->static_val_len > BLE_MESH_PROV_STATIC_OOB_MAX_LEN ||
        prov_info->output_size > BLE_MESH_PROV_OUTPUT_OOB_MAX_LEN ||
        prov_info->input_size > BLE_MESH_PROV_INPUT_OOB_MAX_LEN) {
        BT_ERR("Invalid authentication oob length");
        return -EINVAL;
    }

    __ASSERT(prov_info->uuid, "Device UUID not initialized");

    /* Changed by Espressif. Use micro-ecc to generate public key now. */
    key = bt_mesh_pub_key_get();
    if (!key) {
        BT_ERR("Failed to generate public key");
        return -EIO;
    }

    k_delayed_work_init(&link.prot_timer, protocol_timeout);

    prov = prov_info;

#if defined(CONFIG_BLE_MESH_PB_ADV)
    k_delayed_work_init(&link.tx.retransmit, prov_retransmit);
#endif

    reset_state();

#if defined(CONFIG_BLE_MESH_PB_ADV)
    bt_mesh_pb_buf_mutex_new();
#endif

    return 0;
}

#if CONFIG_BLE_MESH_DEINIT
int bt_mesh_prov_deinit(void)
{
    if (prov == NULL) {
        BT_ERR("No provisioning context provided");
        return -EINVAL;
    }

    k_delayed_work_free(&link.prot_timer);

#if defined(CONFIG_BLE_MESH_PB_ADV)
    prov_clear_tx();
    k_delayed_work_free(&link.tx.retransmit);
#if defined(CONFIG_BLE_MESH_USE_DUPLICATE_SCAN)
    /* Remove the link id from exceptional list */
    bt_mesh_update_exceptional_list(BLE_MESH_EXCEP_LIST_REMOVE,
                                    BLE_MESH_EXCEP_INFO_MESH_LINK_ID, &link.id);
#endif /* CONFIG_BLE_MESH_USE_DUPLICATE_SCAN */
#endif /* CONFIG_BLE_MESH_PB_ADV */

    (void)memset(&link, 0, sizeof(link));

#if defined(CONFIG_BLE_MESH_PB_ADV)
    bt_mesh_pb_buf_mutex_free();
#endif

    prov = NULL;

    return 0;
}
#endif /* CONFIG_BLE_MESH_DEINIT */

void bt_mesh_prov_complete(uint16_t net_idx, const uint8_t net_key[16],
                           uint16_t addr, uint8_t flags, uint32_t iv_index)
{
    if (prov->complete) {
        prov->complete(net_idx, net_key, addr, flags, iv_index);
    }
}

void bt_mesh_prov_reset(void)
{
    if (prov->reset) {
        prov->reset();
    }
}

#endif /* CONFIG_BLE_MESH_NODE */
