/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <errno.h>

#include "sdkconfig.h"
#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BLE_MESH_DEBUG_PROXY)

#include "mesh_buf.h"
#include "mesh_util.h"
#include "mesh_bearer_adapt.h"
#include "mesh_trace.h"

#include "mesh.h"
#include "adv.h"
#include "net.h"
#include "prov.h"
#include "beacon.h"
#include "foundation.h"
#include "access.h"
#include "proxy_server.h"

#if CONFIG_BLE_MESH_NODE

/* Not support enabling Proxy Client and Proxy Server simultaneously */
_Static_assert(!(IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_SERVER) &&IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_CLIENT)),
               "Not support Proxy Server and Proxy Client simultaneously");

#define PDU_TYPE(data)     (data[0] & BIT_MASK(6))
#define PDU_SAR(data)      (data[0] >> 6)

/* Mesh Profile 1.0 Section 6.6:
 * "The timeout for the SAR transfer is 20 seconds. When the timeout
 *  expires, the Proxy Server shall disconnect."
 */
#define PROXY_SAR_TIMEOUT  K_SECONDS(20)

#define SAR_COMPLETE       0x00
#define SAR_FIRST          0x01
#define SAR_CONT           0x02
#define SAR_LAST           0x03

#define CFG_FILTER_SET     0x00
#define CFG_FILTER_ADD     0x01
#define CFG_FILTER_REMOVE  0x02
#define CFG_FILTER_STATUS  0x03

#define PDU_HDR(sar, type) (sar << 6 | (type & BIT_MASK(6)))

#define CLIENT_BUF_SIZE 68

#define ADV_OPT (BLE_MESH_ADV_OPT_CONNECTABLE | BLE_MESH_ADV_OPT_ONE_TIME)

static const struct bt_mesh_adv_param slow_adv_param = {
    .options = ADV_OPT,
    .interval_min = BLE_MESH_GAP_ADV_SLOW_INT_MIN,
    .interval_max = BLE_MESH_GAP_ADV_SLOW_INT_MAX,
};

static const struct bt_mesh_adv_param fast_adv_param = {
    .options = ADV_OPT,
    .interval_min = BLE_MESH_GAP_ADV_FAST_INT_MIN_0,
    .interval_max = BLE_MESH_GAP_ADV_FAST_INT_MAX_0,
};

static bool proxy_adv_enabled;

#if defined(CONFIG_BLE_MESH_GATT_PROXY_SERVER)
static void proxy_send_beacons(struct k_work *work);
static u16_t proxy_ccc_val;
#endif

#if defined(CONFIG_BLE_MESH_PB_GATT)
static u16_t prov_ccc_val;
static bool prov_fast_adv;
#endif

static struct bt_mesh_proxy_client {
    struct bt_mesh_conn *conn;
    u16_t filter[CONFIG_BLE_MESH_PROXY_FILTER_SIZE];
    enum __packed {
        NONE,
        WHITELIST,
        BLACKLIST,
        PROV,
    } filter_type;
    u8_t msg_type;
#if defined(CONFIG_BLE_MESH_GATT_PROXY_SERVER)
    struct k_work send_beacons;
#endif
    struct k_delayed_work    sar_timer;
    struct net_buf_simple    buf;
} clients[BLE_MESH_MAX_CONN] = {
    [0 ... (BLE_MESH_MAX_CONN - 1)] = {
#if defined(CONFIG_BLE_MESH_GATT_PROXY_SERVER)
        .send_beacons = _K_WORK_INITIALIZER(proxy_send_beacons),
#endif
    },
};

static u8_t client_buf_data[CLIENT_BUF_SIZE * BLE_MESH_MAX_CONN];

/* Track which service is enabled */
static enum {
    MESH_GATT_NONE,
    MESH_GATT_PROV,
    MESH_GATT_PROXY,
} gatt_svc = MESH_GATT_NONE;

static char device_name[DEVICE_NAME_SIZE] = "ESP-BLE-MESH";

int bt_mesh_set_device_name(const char *name)
{
    if (!name) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    if (strlen(name) > DEVICE_NAME_SIZE) {
        BT_ERR("%s, Too long device name", __func__);
        return -EINVAL;
    }

    memset(device_name, 0x0, sizeof(device_name));
    memcpy(device_name, name, strlen(name));

    return 0;
}

static struct bt_mesh_proxy_client *find_client(struct bt_mesh_conn *conn)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(clients); i++) {
        if (clients[i].conn == conn) {
            return &clients[i];
        }
    }

    return NULL;
}

static void proxy_sar_timeout(struct k_work *work)
{
    struct bt_mesh_proxy_client *client = NULL;

    BT_WARN("Proxy SAR timeout");

    client = CONTAINER_OF(work, struct bt_mesh_proxy_client, sar_timer.work);
    if (!client || !client->conn) {
        BT_ERR("%s, Invalid proxy client parameter", __func__);
        return;
    }

    net_buf_simple_reset(&client->buf);
    bt_mesh_gatts_disconnect(client->conn, 0x13);
}

#if defined(CONFIG_BLE_MESH_GATT_PROXY_SERVER)
/* Next subnet in queue to be advertised */
static int next_idx;

static int proxy_segment_and_send(struct bt_mesh_conn *conn, u8_t type,
                                  struct net_buf_simple *msg);

static int filter_set(struct bt_mesh_proxy_client *client,
                      struct net_buf_simple *buf)
{
    u8_t type;

    if (buf->len < 1) {
        BT_WARN("Too short Filter Set message");
        return -EINVAL;
    }

    type = net_buf_simple_pull_u8(buf);
    BT_DBG("type 0x%02x", type);

    switch (type) {
    case 0x00:
        (void)memset(client->filter, 0, sizeof(client->filter));
        client->filter_type = WHITELIST;
        break;
    case 0x01:
        (void)memset(client->filter, 0, sizeof(client->filter));
        client->filter_type = BLACKLIST;
        break;
    default:
        BT_WARN("Prohibited Filter Type 0x%02x", type);
        return -EINVAL;
    }

    return 0;
}

static void filter_add(struct bt_mesh_proxy_client *client, u16_t addr)
{
    int i;

    BT_DBG("addr 0x%04x", addr);

    if (addr == BLE_MESH_ADDR_UNASSIGNED) {
        return;
    }

    for (i = 0; i < ARRAY_SIZE(client->filter); i++) {
        if (client->filter[i] == addr) {
            return;
        }
    }

    for (i = 0; i < ARRAY_SIZE(client->filter); i++) {
        if (client->filter[i] == BLE_MESH_ADDR_UNASSIGNED) {
            client->filter[i] = addr;
            return;
        }
    }
}

static void filter_remove(struct bt_mesh_proxy_client *client, u16_t addr)
{
    int i;

    BT_DBG("addr 0x%04x", addr);

    if (addr == BLE_MESH_ADDR_UNASSIGNED) {
        return;
    }

    for (i = 0; i < ARRAY_SIZE(client->filter); i++) {
        if (client->filter[i] == addr) {
            client->filter[i] = BLE_MESH_ADDR_UNASSIGNED;
            return;
        }
    }
}

static void send_filter_status(struct bt_mesh_proxy_client *client,
                               struct bt_mesh_net_rx *rx,
                               struct net_buf_simple *buf)
{
    struct bt_mesh_net_tx tx = {
        .sub = rx->sub,
        .ctx = &rx->ctx,
        .src = bt_mesh_primary_addr(),
    };
    u16_t filter_size;
    int i, err;

    /* Configuration messages always have dst unassigned */
    tx.ctx->addr = BLE_MESH_ADDR_UNASSIGNED;

    net_buf_simple_reset(buf);
    net_buf_simple_reserve(buf, 10);

    net_buf_simple_add_u8(buf, CFG_FILTER_STATUS);

    if (client->filter_type == WHITELIST) {
        net_buf_simple_add_u8(buf, 0x00);
    } else {
        net_buf_simple_add_u8(buf, 0x01);
    }

    for (filter_size = 0U, i = 0; i < ARRAY_SIZE(client->filter); i++) {
        if (client->filter[i] != BLE_MESH_ADDR_UNASSIGNED) {
            filter_size++;
        }
    }

    net_buf_simple_add_be16(buf, filter_size);

    BT_DBG("%u bytes: %s", buf->len, bt_hex(buf->data, buf->len));

    err = bt_mesh_net_encode(&tx, buf, true);
    if (err) {
        BT_ERR("%s, Encoding Proxy cfg message failed (err %d)", __func__, err);
        return;
    }

    err = proxy_segment_and_send(client->conn, BLE_MESH_PROXY_CONFIG, buf);
    if (err) {
        BT_ERR("%s, Failed to send proxy cfg message (err %d)", __func__, err);
    }
}

static void proxy_cfg(struct bt_mesh_proxy_client *client)
{
    NET_BUF_SIMPLE_DEFINE(buf, 29);
    struct bt_mesh_net_rx rx;
    u8_t opcode;
    int err;

    err = bt_mesh_net_decode(&client->buf, BLE_MESH_NET_IF_PROXY_CFG,
                             &rx, &buf);
    if (err) {
        BT_ERR("%s, Failed to decode Proxy Configuration (err %d)", __func__, err);
        return;
    }

    /* Remove network headers */
    net_buf_simple_pull(&buf, BLE_MESH_NET_HDR_LEN);

    BT_DBG("%u bytes: %s", buf.len, bt_hex(buf.data, buf.len));

    if (buf.len < 1) {
        BT_WARN("Too short proxy configuration PDU");
        return;
    }

    opcode = net_buf_simple_pull_u8(&buf);
    switch (opcode) {
    case CFG_FILTER_SET:
        filter_set(client, &buf);
        send_filter_status(client, &rx, &buf);
        break;
    case CFG_FILTER_ADD:
        while (buf.len >= 2) {
            u16_t addr;

            addr = net_buf_simple_pull_be16(&buf);
            filter_add(client, addr);
        }
        send_filter_status(client, &rx, &buf);
        break;
    case CFG_FILTER_REMOVE:
        while (buf.len >= 2) {
            u16_t addr;

            addr = net_buf_simple_pull_be16(&buf);
            filter_remove(client, addr);
        }
        send_filter_status(client, &rx, &buf);
        break;
    default:
        BT_WARN("Unhandled configuration OpCode 0x%02x", opcode);
        break;
    }
}

static int beacon_send(struct bt_mesh_conn *conn, struct bt_mesh_subnet *sub)
{
    NET_BUF_SIMPLE_DEFINE(buf, 23);

    net_buf_simple_reserve(&buf, 1);
    bt_mesh_beacon_create(sub, &buf);

    return proxy_segment_and_send(conn, BLE_MESH_PROXY_BEACON, &buf);
}

static void proxy_send_beacons(struct k_work *work)
{
    struct bt_mesh_proxy_client *client;
    int i;

    client = CONTAINER_OF(work, struct bt_mesh_proxy_client, send_beacons);

    for (i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
        struct bt_mesh_subnet *sub = &bt_mesh.sub[i];

        if (sub->net_idx != BLE_MESH_KEY_UNUSED) {
            beacon_send(client->conn, sub);
        }
    }
}

void bt_mesh_proxy_beacon_send(struct bt_mesh_subnet *sub)
{
    int i;

    if (!sub) {
        /* NULL means we send on all subnets */
        for (i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
            if (bt_mesh.sub[i].net_idx != BLE_MESH_KEY_UNUSED) {
                bt_mesh_proxy_beacon_send(&bt_mesh.sub[i]);
            }
        }

        return;
    }

    for (i = 0; i < ARRAY_SIZE(clients); i++) {
        if (clients[i].conn) {
            beacon_send(clients[i].conn, sub);
        }
    }
}

void bt_mesh_proxy_identity_start(struct bt_mesh_subnet *sub)
{
    sub->node_id = BLE_MESH_NODE_IDENTITY_RUNNING;
    sub->node_id_start = k_uptime_get_32();

    /* Prioritize the recently enabled subnet */
    next_idx = sub - bt_mesh.sub;
}

void bt_mesh_proxy_identity_stop(struct bt_mesh_subnet *sub)
{
    sub->node_id = BLE_MESH_NODE_IDENTITY_STOPPED;
    sub->node_id_start = 0U;
}

int bt_mesh_proxy_identity_enable(void)
{
    int i, count = 0;

    BT_DBG("%s", __func__);

    if (!bt_mesh_is_provisioned()) {
        return -EAGAIN;
    }

    for (i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
        struct bt_mesh_subnet *sub = &bt_mesh.sub[i];

        if (sub->net_idx == BLE_MESH_KEY_UNUSED) {
            continue;
        }

        if (sub->node_id == BLE_MESH_NODE_IDENTITY_NOT_SUPPORTED) {
            continue;
        }

        bt_mesh_proxy_identity_start(sub);
        count++;
    }

    if (count) {
        bt_mesh_adv_update();
    }

    return 0;
}

#endif /* GATT_PROXY */

static void proxy_complete_pdu(struct bt_mesh_proxy_client *client)
{
    switch (client->msg_type) {
#if defined(CONFIG_BLE_MESH_GATT_PROXY_SERVER)
    case BLE_MESH_PROXY_NET_PDU:
        BT_DBG("Mesh Network PDU");
        bt_mesh_net_recv(&client->buf, 0, BLE_MESH_NET_IF_PROXY);
        break;
    case BLE_MESH_PROXY_BEACON:
        BT_DBG("Mesh Beacon PDU");
        bt_mesh_beacon_recv(&client->buf);
        break;
    case BLE_MESH_PROXY_CONFIG:
        BT_DBG("Mesh Configuration PDU");
        proxy_cfg(client);
        break;
#endif
#if defined(CONFIG_BLE_MESH_PB_GATT)
    case BLE_MESH_PROXY_PROV:
        BT_DBG("Mesh Provisioning PDU");
        bt_mesh_pb_gatt_recv(client->conn, &client->buf);
        break;
#endif
    default:
        BT_WARN("Unhandled Message Type 0x%02x", client->msg_type);
        break;
    }

    net_buf_simple_reset(&client->buf);
}

#define ATTR_IS_PROV(attr) (attr->user_data != NULL)

static ssize_t proxy_recv(struct bt_mesh_conn *conn,
                          const struct bt_mesh_gatt_attr *attr, const void *buf,
                          u16_t len, u16_t offset, u8_t flags)
{
    struct bt_mesh_proxy_client *client = find_client(conn);
    const u8_t *data = buf;

    if (!client) {
        return -ENOTCONN;
    }

    if (len < 1) {
        BT_WARN("Too small Proxy PDU");
        return -EINVAL;
    }

    if (ATTR_IS_PROV(attr) != (PDU_TYPE(data) == BLE_MESH_PROXY_PROV)) {
        BT_WARN("Proxy PDU type doesn't match GATT service");
        return -EINVAL;
    }

    if (len - 1 > net_buf_simple_tailroom(&client->buf)) {
        BT_WARN("Too big proxy PDU");
        return -EINVAL;
    }

    switch (PDU_SAR(data)) {
    case SAR_COMPLETE:
        if (client->buf.len) {
            BT_WARN("Complete PDU while a pending incomplete one");
            return -EINVAL;
        }

        client->msg_type = PDU_TYPE(data);
        net_buf_simple_add_mem(&client->buf, data + 1, len - 1);
        proxy_complete_pdu(client);
        break;

    case SAR_FIRST:
        if (client->buf.len) {
            BT_WARN("First PDU while a pending incomplete one");
            return -EINVAL;
        }

        k_delayed_work_submit(&client->sar_timer, PROXY_SAR_TIMEOUT);
        client->msg_type = PDU_TYPE(data);
        net_buf_simple_add_mem(&client->buf, data + 1, len - 1);
        break;

    case SAR_CONT:
        if (!client->buf.len) {
            BT_WARN("Continuation with no prior data");
            return -EINVAL;
        }

        if (client->msg_type != PDU_TYPE(data)) {
            BT_WARN("Unexpected message type in continuation");
            return -EINVAL;
        }

        k_delayed_work_submit(&client->sar_timer, PROXY_SAR_TIMEOUT);
        net_buf_simple_add_mem(&client->buf, data + 1, len - 1);
        break;

    case SAR_LAST:
        if (!client->buf.len) {
            BT_WARN("Last SAR PDU with no prior data");
            return -EINVAL;
        }

        if (client->msg_type != PDU_TYPE(data)) {
            BT_WARN("Unexpected message type in last SAR PDU");
            return -EINVAL;
        }

        k_delayed_work_cancel(&client->sar_timer);
        net_buf_simple_add_mem(&client->buf, data + 1, len - 1);
        proxy_complete_pdu(client);
        break;
    }

    return len;
}

static int conn_count;

static void proxy_connected(struct bt_mesh_conn *conn, u8_t err)
{
    struct bt_mesh_proxy_client *client;
    int i;

    BT_DBG("conn %p err 0x%02x", conn, err);

    conn_count++;

    /* Since we use ADV_OPT_ONE_TIME */
    proxy_adv_enabled = false;

    /* Try to re-enable advertising in case it's possible */
    if (conn_count < BLE_MESH_MAX_CONN) {
        bt_mesh_adv_update();
    }

    for (client = NULL, i = 0; i < ARRAY_SIZE(clients); i++) {
        if (!clients[i].conn) {
            client = &clients[i];
            break;
        }
    }

    if (!client) {
        BT_ERR("%s, No free Proxy Client objects", __func__);
        return;
    }

    client->conn = bt_mesh_conn_ref(conn);
    client->filter_type = NONE;
    (void)memset(client->filter, 0, sizeof(client->filter));
    net_buf_simple_reset(&client->buf);
}

static void proxy_disconnected(struct bt_mesh_conn *conn, u8_t reason)
{
    int i;

    BT_DBG("conn %p reason 0x%02x", conn, reason);

    conn_count--;

    for (i = 0; i < ARRAY_SIZE(clients); i++) {
        struct bt_mesh_proxy_client *client = &clients[i];

        if (client->conn == conn) {
            if (IS_ENABLED(CONFIG_BLE_MESH_PB_GATT) &&
                    client->filter_type == PROV) {
                bt_mesh_pb_gatt_close(conn);
            }

            k_delayed_work_cancel(&client->sar_timer);
            bt_mesh_conn_unref(client->conn);
            client->conn = NULL;
            break;
        }
    }

    bt_mesh_adv_update();
}

struct net_buf_simple *bt_mesh_proxy_get_buf(void)
{
    struct net_buf_simple *buf = &clients[0].buf;

    net_buf_simple_reset(buf);

    return buf;
}

#if defined(CONFIG_BLE_MESH_PB_GATT)
static ssize_t prov_ccc_write(struct bt_mesh_conn *conn,
                              const struct bt_mesh_gatt_attr *attr,
                              const void *buf, u16_t len,
                              u16_t offset, u8_t flags)
{
    struct bt_mesh_proxy_client *client;
    u16_t *value = attr->user_data;

    BT_DBG("len %u: %s", len, bt_hex(buf, len));

    if (len != sizeof(*value)) {
        return BLE_MESH_GATT_ERR(BLE_MESH_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    }

    *value = sys_get_le16(buf);
    if (*value != BLE_MESH_GATT_CCC_NOTIFY) {
        BT_WARN("Client wrote 0x%04x instead enabling notify", *value);
        return len;
    }

    /* If a connection exists there must be a client */
    client = find_client(conn);
    __ASSERT(client, "No client for connection");

    if (client->filter_type == NONE) {
        client->filter_type = PROV;
        bt_mesh_pb_gatt_open(conn);
    }

    return len;
}

static ssize_t prov_ccc_read(struct bt_mesh_conn *conn,
                             const struct bt_mesh_gatt_attr *attr,
                             void *buf, u16_t len, u16_t offset)
{
    u16_t *value = attr->user_data;

    return bt_mesh_gatts_attr_read(conn, attr, buf, len, offset, value,
                                   sizeof(*value));
}

/* Mesh Provisioning Service Declaration */
static struct bt_mesh_gatt_attr prov_attrs[] = {
    BLE_MESH_GATT_PRIMARY_SERVICE(BLE_MESH_UUID_MESH_PROV),

    BLE_MESH_GATT_CHARACTERISTIC(BLE_MESH_UUID_MESH_PROV_DATA_IN,
    BLE_MESH_GATT_CHRC_WRITE_WITHOUT_RESP),
    BLE_MESH_GATT_DESCRIPTOR(BLE_MESH_UUID_MESH_PROV_DATA_IN, BLE_MESH_GATT_PERM_WRITE,
    NULL, proxy_recv, (void *)1),

    BLE_MESH_GATT_CHARACTERISTIC(BLE_MESH_UUID_MESH_PROV_DATA_OUT,
    BLE_MESH_GATT_CHRC_NOTIFY),
    BLE_MESH_GATT_DESCRIPTOR(BLE_MESH_UUID_MESH_PROV_DATA_OUT, BLE_MESH_GATT_PERM_NONE,
    NULL, NULL, NULL),
    /* Add custom CCC as clients need to be tracked individually */
    BLE_MESH_GATT_DESCRIPTOR(BLE_MESH_UUID_GATT_CCC,
    BLE_MESH_GATT_PERM_WRITE | BLE_MESH_GATT_PERM_READ,
    prov_ccc_read, prov_ccc_write, &prov_ccc_val),
};

struct bt_mesh_gatt_service prov_svc = BLE_MESH_GATT_SERVICE(prov_attrs);

int bt_mesh_proxy_prov_enable(void)
{
    int i;

    BT_DBG("%s", __func__);

    if (gatt_svc == MESH_GATT_PROV) {
        BT_WARN("%s, Already", __func__);
        return -EALREADY;
    }

    if (gatt_svc != MESH_GATT_NONE) {
        BT_WARN("%s, Busy", __func__);
        return -EBUSY;
    }

    bt_mesh_gatts_service_start(&prov_svc);
    gatt_svc = MESH_GATT_PROV;
    prov_fast_adv = true;

    for (i = 0; i < ARRAY_SIZE(clients); i++) {
        if (clients[i].conn) {
            clients[i].filter_type = PROV;
        }
    }


    return 0;
}

int bt_mesh_proxy_prov_disable(bool disconnect)
{
    int i;

    BT_DBG("%s", __func__);

    if (gatt_svc == MESH_GATT_NONE) {
        BT_WARN("%s, Already", __func__);
        return -EALREADY;
    }

    if (gatt_svc != MESH_GATT_PROV) {
        BT_WARN("%s, Busy", __func__);
        return -EBUSY;
    }

    bt_mesh_gatts_service_stop(&prov_svc);
    gatt_svc = MESH_GATT_NONE;

    for (i = 0; i < ARRAY_SIZE(clients); i++) {
        struct bt_mesh_proxy_client *client = &clients[i];

        if (!client->conn || client->filter_type != PROV) {
            continue;
        }

        if (disconnect) {
            bt_mesh_gatts_disconnect(client->conn, 0x13);
        } else {
            bt_mesh_pb_gatt_close(client->conn);
            client->filter_type = NONE;
        }
    }

    bt_mesh_adv_update();

    return 0;
}

#endif /* CONFIG_BLE_MESH_PB_GATT */

#if defined(CONFIG_BLE_MESH_GATT_PROXY_SERVER)
static ssize_t proxy_ccc_write(struct bt_mesh_conn *conn,
                               const struct bt_mesh_gatt_attr *attr,
                               const void *buf, u16_t len,
                               u16_t offset, u8_t flags)
{
    struct bt_mesh_proxy_client *client;
    u16_t value;

    BT_DBG("len %u: %s", len, bt_hex(buf, len));

    if (len != sizeof(value)) {
        return BLE_MESH_GATT_ERR(BLE_MESH_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    }

    value = sys_get_le16(buf);
    if (value != BLE_MESH_GATT_CCC_NOTIFY) {
        BT_WARN("Client wrote 0x%04x instead enabling notify", value);
        return len;
    }

    /* If a connection exists there must be a client */
    client = find_client(conn);
    __ASSERT(client, "No client for connection");

    if (client->filter_type == NONE) {
        client->filter_type = WHITELIST;
        k_work_submit(&client->send_beacons);
    }

    return len;
}

static ssize_t proxy_ccc_read(struct bt_mesh_conn *conn,
                              const struct bt_mesh_gatt_attr *attr,
                              void *buf, u16_t len, u16_t offset)
{
    u16_t *value = attr->user_data;

    return bt_mesh_gatts_attr_read(conn, attr, buf, len, offset, value,
                                   sizeof(*value));
}

/* Mesh Proxy Service Declaration */
static struct bt_mesh_gatt_attr proxy_attrs[] = {
    BLE_MESH_GATT_PRIMARY_SERVICE(BLE_MESH_UUID_MESH_PROXY),

    BLE_MESH_GATT_CHARACTERISTIC(BLE_MESH_UUID_MESH_PROXY_DATA_IN,
    BLE_MESH_GATT_CHRC_WRITE_WITHOUT_RESP),
    BLE_MESH_GATT_DESCRIPTOR(BLE_MESH_UUID_MESH_PROXY_DATA_IN, BLE_MESH_GATT_PERM_WRITE,
    NULL, proxy_recv, NULL),

    BLE_MESH_GATT_CHARACTERISTIC(BLE_MESH_UUID_MESH_PROXY_DATA_OUT,
    BLE_MESH_GATT_CHRC_NOTIFY),
    BLE_MESH_GATT_DESCRIPTOR(BLE_MESH_UUID_MESH_PROXY_DATA_OUT, BLE_MESH_GATT_PERM_NONE,
    NULL, NULL, NULL),
    /* Add custom CCC as clients need to be tracked individually */
    BLE_MESH_GATT_DESCRIPTOR(BLE_MESH_UUID_GATT_CCC,
    BLE_MESH_GATT_PERM_READ | BLE_MESH_GATT_PERM_WRITE,
    proxy_ccc_read, proxy_ccc_write, &proxy_ccc_val),
};

struct bt_mesh_gatt_service proxy_svc = BLE_MESH_GATT_SERVICE(proxy_attrs);

int bt_mesh_proxy_gatt_enable(void)
{
    int i;

    BT_DBG("%s", __func__);

    if (gatt_svc == MESH_GATT_PROXY) {
        BT_WARN("%s, Already", __func__);
        return -EALREADY;
    }

    if (gatt_svc != MESH_GATT_NONE) {
        BT_WARN("%s, Busy", __func__);
        return -EBUSY;
    }

    bt_mesh_gatts_service_start(&proxy_svc);
    gatt_svc = MESH_GATT_PROXY;

    for (i = 0; i < ARRAY_SIZE(clients); i++) {
        if (clients[i].conn) {
            clients[i].filter_type = WHITELIST;
        }
    }

    return 0;
}

void bt_mesh_proxy_gatt_disconnect(void)
{
    int i;

    BT_DBG("%s", __func__);

    for (i = 0; i < ARRAY_SIZE(clients); i++) {
        struct bt_mesh_proxy_client *client = &clients[i];

        if (client->conn && (client->filter_type == WHITELIST ||
                             client->filter_type == BLACKLIST)) {
            client->filter_type = NONE;
            bt_mesh_gatts_disconnect(client->conn, 0x13);
        }
    }
}

int bt_mesh_proxy_gatt_disable(void)
{
    BT_DBG("%s", __func__);

    if (gatt_svc == MESH_GATT_NONE) {
        BT_WARN("%s, Already", __func__);
        return -EALREADY;
    }

    if (gatt_svc != MESH_GATT_PROXY) {
        BT_WARN("%s, Busy", __func__);
        return -EBUSY;
    }

    bt_mesh_proxy_gatt_disconnect();

    bt_mesh_gatts_service_stop(&proxy_svc);
    gatt_svc = MESH_GATT_NONE;

    return 0;
}

void bt_mesh_proxy_addr_add(struct net_buf_simple *buf, u16_t addr)
{
    struct bt_mesh_proxy_client *client =
        CONTAINER_OF(buf, struct bt_mesh_proxy_client, buf);

    BT_DBG("filter_type %u addr 0x%04x", client->filter_type, addr);

    if (client->filter_type == WHITELIST) {
        filter_add(client, addr);
    } else if (client->filter_type == BLACKLIST) {
        filter_remove(client, addr);
    }
}

static bool client_filter_match(struct bt_mesh_proxy_client *client,
                                u16_t addr)
{
    int i;

    BT_DBG("filter_type %u addr 0x%04x", client->filter_type, addr);

    if (client->filter_type == WHITELIST) {
        for (i = 0; i < ARRAY_SIZE(client->filter); i++) {
            if (client->filter[i] == addr) {
                return true;
            }
        }

        return false;
    }

    if (client->filter_type == BLACKLIST) {
        for (i = 0; i < ARRAY_SIZE(client->filter); i++) {
            if (client->filter[i] == addr) {
                return false;
            }
        }

        return true;
    }

    return false;
}

bool bt_mesh_proxy_relay(struct net_buf_simple *buf, u16_t dst)
{
    bool relayed = false;
    int i;

    BT_DBG("%u bytes to dst 0x%04x", buf->len, dst);

    for (i = 0; i < ARRAY_SIZE(clients); i++) {
        struct bt_mesh_proxy_client *client = &clients[i];
        NET_BUF_SIMPLE_DEFINE(msg, 32);

        if (!client->conn) {
            continue;
        }

        if (!client_filter_match(client, dst)) {
            continue;
        }

        /* Proxy PDU sending modifies the original buffer,
         * so we need to make a copy.
         */
        net_buf_simple_reserve(&msg, 1);
        net_buf_simple_add_mem(&msg, buf->data, buf->len);

        bt_mesh_proxy_send(client->conn, BLE_MESH_PROXY_NET_PDU, &msg);
        relayed = true;
    }

    return relayed;
}

#endif /* CONFIG_BLE_MESH_GATT_PROXY_SERVER */

static int proxy_send(struct bt_mesh_conn *conn, const void *data, u16_t len)
{
    BT_DBG("%u bytes: %s", len, bt_hex(data, len));

#if defined(CONFIG_BLE_MESH_GATT_PROXY_SERVER)
    if (gatt_svc == MESH_GATT_PROXY) {
        return bt_mesh_gatts_notify(conn, &proxy_attrs[4], data, len);
    }
#endif

#if defined(CONFIG_BLE_MESH_PB_GATT)
    if (gatt_svc == MESH_GATT_PROV) {
        return bt_mesh_gatts_notify(conn, &prov_attrs[4], data, len);
    }
#endif

    return 0;
}

static int proxy_segment_and_send(struct bt_mesh_conn *conn, u8_t type,
                                  struct net_buf_simple *msg)
{
    u16_t mtu;

    BT_DBG("conn %p type 0x%02x len %u: %s", conn, type, msg->len,
           bt_hex(msg->data, msg->len));

    /* ATT_MTU - OpCode (1 byte) - Handle (2 bytes) */
    mtu = bt_mesh_gatt_get_mtu(conn) - 3;
    if (mtu > msg->len) {
        net_buf_simple_push_u8(msg, PDU_HDR(SAR_COMPLETE, type));
        return proxy_send(conn, msg->data, msg->len);
    }

    net_buf_simple_push_u8(msg, PDU_HDR(SAR_FIRST, type));
    proxy_send(conn, msg->data, mtu);
    net_buf_simple_pull(msg, mtu);

    while (msg->len) {
        if (msg->len + 1 < mtu) {
            net_buf_simple_push_u8(msg, PDU_HDR(SAR_LAST, type));
            proxy_send(conn, msg->data, msg->len);
            break;
        }

        net_buf_simple_push_u8(msg, PDU_HDR(SAR_CONT, type));
        proxy_send(conn, msg->data, mtu);
        net_buf_simple_pull(msg, mtu);
    }

    return 0;
}

int bt_mesh_proxy_send(struct bt_mesh_conn *conn, u8_t type,
                       struct net_buf_simple *msg)
{
    struct bt_mesh_proxy_client *client = find_client(conn);

    if (!client) {
        BT_ERR("%s, No Proxy Client found", __func__);
        return -ENOTCONN;
    }

    if ((client->filter_type == PROV) != (type == BLE_MESH_PROXY_PROV)) {
        BT_ERR("%s, Invalid PDU type for Proxy Client", __func__);
        return -EINVAL;
    }

    return proxy_segment_and_send(conn, type, msg);
}

#if defined(CONFIG_BLE_MESH_PB_GATT)
static u8_t prov_svc_data[20] = { 0x27, 0x18, };

static const struct bt_mesh_adv_data prov_ad[] = {
    BLE_MESH_ADV_DATA_BYTES(BLE_MESH_DATA_FLAGS, (BLE_MESH_AD_GENERAL | BLE_MESH_AD_NO_BREDR)),
    BLE_MESH_ADV_DATA_BYTES(BLE_MESH_DATA_UUID16_ALL, 0x27, 0x18),
    BLE_MESH_ADV_DATA(BLE_MESH_DATA_SVC_DATA16, prov_svc_data, sizeof(prov_svc_data)),
};
#endif /* PB_GATT */

#if defined(CONFIG_BLE_MESH_GATT_PROXY_SERVER)

#define ID_TYPE_NET  0x00
#define ID_TYPE_NODE 0x01

#define NODE_ID_LEN  19
#define NET_ID_LEN   11

#define NODE_ID_TIMEOUT K_SECONDS(CONFIG_BLE_MESH_NODE_ID_TIMEOUT)

static u8_t proxy_svc_data[NODE_ID_LEN] = { 0x28, 0x18, };

static const struct bt_mesh_adv_data node_id_ad[] = {
    BLE_MESH_ADV_DATA_BYTES(BLE_MESH_DATA_FLAGS, (BLE_MESH_AD_GENERAL | BLE_MESH_AD_NO_BREDR)),
    BLE_MESH_ADV_DATA_BYTES(BLE_MESH_DATA_UUID16_ALL, 0x28, 0x18),
    BLE_MESH_ADV_DATA(BLE_MESH_DATA_SVC_DATA16, proxy_svc_data, NODE_ID_LEN),
};

static const struct bt_mesh_adv_data net_id_ad[] = {
    BLE_MESH_ADV_DATA_BYTES(BLE_MESH_DATA_FLAGS, (BLE_MESH_AD_GENERAL | BLE_MESH_AD_NO_BREDR)),
    BLE_MESH_ADV_DATA_BYTES(BLE_MESH_DATA_UUID16_ALL, 0x28, 0x18),
    BLE_MESH_ADV_DATA(BLE_MESH_DATA_SVC_DATA16, proxy_svc_data, NET_ID_LEN),
};

static size_t gatt_proxy_adv_create(struct bt_mesh_adv_data *proxy_sd)
{
    const char *name = device_name;
    size_t name_len = strlen(name);
    /* One octet for Length, and another octet for AD type */
    size_t sd_space = 29;

    if (name_len > sd_space) {
        proxy_sd->type = BLE_MESH_DATA_NAME_SHORTENED;
        proxy_sd->data_len = sd_space;
    } else {
        proxy_sd->type = BLE_MESH_DATA_NAME_COMPLETE;
        proxy_sd->data_len = name_len;
    }

    proxy_sd->data = (const u8_t *)name;

    return 1;
}

static int node_id_adv(struct bt_mesh_subnet *sub)
{
    struct bt_mesh_adv_data proxy_sd = {0};
    size_t proxy_sd_len;
    u8_t tmp[16];
    int err;

    BT_DBG("%s", __func__);

    proxy_svc_data[2] = ID_TYPE_NODE;

    err = bt_mesh_rand(proxy_svc_data + 11, 8);
    if (err) {
        return err;
    }

    (void)memset(tmp, 0, 6);
    memcpy(tmp + 6, proxy_svc_data + 11, 8);
    sys_put_be16(bt_mesh_primary_addr(), tmp + 14);

    err = bt_mesh_encrypt_be(sub->keys[sub->kr_flag].identity, tmp, tmp);
    if (err) {
        return err;
    }

    memcpy(proxy_svc_data + 3, tmp + 8, 8);
    proxy_sd_len = gatt_proxy_adv_create(&proxy_sd);

    err = bt_le_adv_start(&fast_adv_param, node_id_ad,
                          ARRAY_SIZE(node_id_ad), &proxy_sd, proxy_sd_len);
    if (err) {
        BT_WARN("Failed to advertise using Node ID (err %d)", err);
        return err;
    }

    proxy_adv_enabled = true;

    return 0;
}

static int net_id_adv(struct bt_mesh_subnet *sub)
{
    struct bt_mesh_adv_data proxy_sd = {0};
    size_t proxy_sd_len;
    int err;

    BT_DBG("%s", __func__);

    proxy_svc_data[2] = ID_TYPE_NET;

    BT_DBG("Advertising with NetId %s",
           bt_hex(sub->keys[sub->kr_flag].net_id, 8));

    memcpy(proxy_svc_data + 3, sub->keys[sub->kr_flag].net_id, 8);
    proxy_sd_len = gatt_proxy_adv_create(&proxy_sd);

    err = bt_le_adv_start(&slow_adv_param, net_id_ad,
                          ARRAY_SIZE(net_id_ad), &proxy_sd, proxy_sd_len);
    if (err) {
        BT_WARN("Failed to advertise using Network ID (err %d)", err);
        return err;
    }

    proxy_adv_enabled = true;

    return 0;
}

static bool advertise_subnet(struct bt_mesh_subnet *sub)
{
    if (sub->net_idx == BLE_MESH_KEY_UNUSED) {
        return false;
    }

    return (sub->node_id == BLE_MESH_NODE_IDENTITY_RUNNING ||
            bt_mesh_gatt_proxy_get() == BLE_MESH_GATT_PROXY_ENABLED);
}

static struct bt_mesh_subnet *next_sub(void)
{
    struct bt_mesh_subnet *sub = NULL;
    int i;

    for (i = next_idx; i < ARRAY_SIZE(bt_mesh.sub); i++) {
        sub = &bt_mesh.sub[i];
        if (advertise_subnet(sub)) {
            next_idx = (i + 1) % ARRAY_SIZE(bt_mesh.sub);
            return sub;
        }
    }

    /**
     * If among [next_idx, ARRAY_SIZE(bt_mesh.sub)], there is no subnet
     * to advertise, then try to start advertising from Primary subnet.
     */
    for (i = 0; i < next_idx; i++) {
        sub = &bt_mesh.sub[i];
        if (advertise_subnet(sub)) {
            next_idx = (i + 1) % ARRAY_SIZE(bt_mesh.sub);
            return sub;
        }
    }

    return NULL;
}

static int sub_count(void)
{
    int i, count = 0;

    for (i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
        struct bt_mesh_subnet *sub = &bt_mesh.sub[i];

        if (advertise_subnet(sub)) {
            count++;
        }
    }

    return count;
}

static s32_t gatt_proxy_advertise(struct bt_mesh_subnet *sub)
{
    s32_t remaining = K_FOREVER;
    int subnet_count;

    BT_DBG("%s", __func__);

    if (conn_count == BLE_MESH_MAX_CONN) {
        BT_WARN("Connectable advertising deferred (max connections)");
        return remaining;
    }

    if (!sub) {
        BT_WARN("No subnets to advertise on");
        return remaining;
    }

    if (sub->node_id == BLE_MESH_NODE_IDENTITY_RUNNING) {
        u32_t active = k_uptime_get_32() - sub->node_id_start;

        if (active < NODE_ID_TIMEOUT) {
            remaining = NODE_ID_TIMEOUT - active;
            BT_DBG("Node ID active for %u ms, %d ms remaining",
                   active, remaining);
            node_id_adv(sub);
        } else {
            bt_mesh_proxy_identity_stop(sub);
            BT_DBG("Node ID stopped");
        }
    }

    if (sub->node_id == BLE_MESH_NODE_IDENTITY_STOPPED) {
        if (bt_mesh_gatt_proxy_get() == BLE_MESH_GATT_PROXY_ENABLED) {
            net_id_adv(sub);
        } else {
            return gatt_proxy_advertise(next_sub());
        }
    }

    subnet_count = sub_count();
    BT_DBG("sub_count %u", subnet_count);
    if (subnet_count > 1) {
        s32_t max_timeout;

        /* We use NODE_ID_TIMEOUT as a starting point since it may
         * be less than 60 seconds. Divide this period into at least
         * 6 slices, but make sure that a slice is at least one
         * second long (to avoid excessive rotation).
         */
        max_timeout = NODE_ID_TIMEOUT / MAX(subnet_count, 6);
        max_timeout = MAX(max_timeout, K_SECONDS(1));

        if (remaining > max_timeout || remaining < 0) {
            remaining = max_timeout;
        }
    }

    BT_DBG("Advertising %d ms for net_idx 0x%04x", remaining, sub->net_idx);

    return remaining;
}
#endif /* GATT_PROXY */

#if defined(CONFIG_BLE_MESH_PB_GATT)
static size_t gatt_prov_adv_create(struct bt_mesh_adv_data prov_sd[2])
{
    const struct bt_mesh_prov *prov = bt_mesh_prov_get();
    const char *name = device_name;
    size_t name_len = strlen(name);
    size_t prov_sd_len = 0;
    size_t sd_space = 31;

    memcpy(prov_svc_data + 2, prov->uuid, 16);
    sys_put_be16(prov->oob_info, prov_svc_data + 18);

    if (prov->uri) {
        size_t uri_len = strlen(prov->uri);

        if (uri_len > 29) {
            /* There's no way to shorten an URI */
            BT_WARN("Too long URI to fit advertising packet");
        } else {
            prov_sd[0].type = BLE_MESH_DATA_URI;
            prov_sd[0].data_len = uri_len;
            prov_sd[0].data = (const u8_t *)prov->uri;
            sd_space -= 2 + uri_len;
            prov_sd_len++;
        }
    }

    if (sd_space > 2 && name_len > 0) {
        sd_space -= 2;

        if (sd_space < name_len) {
            prov_sd[prov_sd_len].type = BLE_MESH_DATA_NAME_SHORTENED;
            prov_sd[prov_sd_len].data_len = sd_space;
        } else {
            prov_sd[prov_sd_len].type = BLE_MESH_DATA_NAME_COMPLETE;
            prov_sd[prov_sd_len].data_len = name_len;
        }

        prov_sd[prov_sd_len].data = (const u8_t *)name;
        prov_sd_len++;
    }

    return prov_sd_len;
}
#endif /* CONFIG_BLE_MESH_PB_GATT */

s32_t bt_mesh_proxy_adv_start(void)
{
    BT_DBG("%s", __func__);

    if (gatt_svc == MESH_GATT_NONE) {
        return K_FOREVER;
    }

#if defined(CONFIG_BLE_MESH_PB_GATT)
    if (!bt_mesh_is_provisioned()) {
        const struct bt_mesh_adv_param *param;
        struct bt_mesh_adv_data prov_sd[2];
        size_t prov_sd_len;

        if (prov_fast_adv) {
            param = &fast_adv_param;
        } else {
            param = &slow_adv_param;
        }

        prov_sd_len = gatt_prov_adv_create(prov_sd);

        if (bt_le_adv_start(param, prov_ad, ARRAY_SIZE(prov_ad),
                            prov_sd, prov_sd_len) == 0) {
            proxy_adv_enabled = true;

            /* Advertise 60 seconds using fast interval */
            if (prov_fast_adv) {
                prov_fast_adv = false;
                return K_SECONDS(60);
            }
        }
    }
#endif /* PB_GATT */

#if defined(CONFIG_BLE_MESH_GATT_PROXY_SERVER)
    if (bt_mesh_is_provisioned()) {
        return gatt_proxy_advertise(next_sub());
    }
#endif /* GATT_PROXY */

    return K_FOREVER;
}

void bt_mesh_proxy_adv_stop(void)
{
    int err;

    BT_DBG("adv_enabled %u", proxy_adv_enabled);

    if (!proxy_adv_enabled) {
        return;
    }

    err = bt_le_adv_stop();
    if (err) {
        BT_ERR("%s, Failed to stop advertising (err %d)", __func__, err);
    } else {
        proxy_adv_enabled = false;
    }
}

static struct bt_mesh_conn_cb conn_callbacks = {
    .connected = proxy_connected,
    .disconnected = proxy_disconnected,
};

int bt_mesh_proxy_init(void)
{
    int i;

    /* Initialize the client receive buffers */
    for (i = 0; i < ARRAY_SIZE(clients); i++) {
        struct bt_mesh_proxy_client *client = &clients[i];

        client->buf.size = CLIENT_BUF_SIZE;
        client->buf.__buf = client_buf_data + (i * CLIENT_BUF_SIZE);

        k_delayed_work_init(&client->sar_timer, proxy_sar_timeout);
    }

    bt_mesh_gatts_conn_cb_register(&conn_callbacks);

#if defined(CONFIG_BLE_MESH_PB_GATT)
    const struct bt_mesh_prov *prov = bt_mesh_prov_get();
    __ASSERT(prov && prov->uuid, "%s, Device UUID is not initialized", __func__);
#endif

    return 0;
}

#endif /* CONFIG_BLE_MESH_NODE */
