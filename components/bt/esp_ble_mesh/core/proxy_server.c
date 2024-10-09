/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <errno.h>

#include "mesh.h"
#include "rpl.h"
#include "adv.h"
#include "beacon.h"
#include "access.h"
#include "transport.h"
#include "foundation.h"
#include "mesh/common.h"
#include "proxy_common.h"
#include "proxy_server.h"
#include "prov_common.h"
#include "prov_node.h"

#if CONFIG_BLE_MESH_V11_SUPPORT
#include "mesh_v1.1/utils.h"
#endif

#if (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_GATT) || \
     CONFIG_BLE_MESH_GATT_PROXY_SERVER

#if !CONFIG_BLE_MESH_BQB_TEST
/* Not support enabling Proxy Client and Proxy Server simultaneously */
_Static_assert(!(IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_SERVER) && IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_CLIENT)),
               "Not support Proxy Server and Proxy Client simultaneously");
#endif

#define ADV_OPT     (BLE_MESH_ADV_OPT_CONNECTABLE | BLE_MESH_ADV_OPT_ONE_TIME)

#if CONFIG_BLE_MESH_GATT_PROXY_SERVER && \
    (CONFIG_BLE_MESH_PRB_SRV || CONFIG_BLE_MESH_PROXY_SOLIC_PDU_RX)
#define RAND_UPDATE_INTERVAL    K_MINUTES(10)

/* The Random field of Private Network Identity
 * Advertisement should be updated every 10 minutes.
 */
static struct k_delayed_work rand_upd_timer;
static uint8_t net_id_random[8] = {0};
#endif

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

#if CONFIG_BLE_MESH_GATT_PROXY_SERVER
static void proxy_send_beacons(struct k_work *work);
static uint16_t proxy_ccc_val;
#endif

#if CONFIG_BLE_MESH_PB_GATT
static uint16_t prov_ccc_val;
static bool prov_fast_adv;
static uint32_t prov_start_time;
#endif

static struct bt_mesh_proxy_client clients[BLE_MESH_MAX_CONN] = {
    [0 ... (BLE_MESH_MAX_CONN - 1)] = {
#if CONFIG_BLE_MESH_PROXY_PRIVACY
        .proxy_privacy = BLE_MESH_PROXY_PRIVACY_DISABLED,
#endif
    },
};

static uint8_t client_buf_data[BLE_MESH_PROXY_BUF_SIZE * BLE_MESH_MAX_CONN];

/* Track which service is enabled */
static enum {
    MESH_GATT_NONE,
    MESH_GATT_PROV,
    MESH_GATT_PROXY,
} gatt_svc = MESH_GATT_NONE;

static char device_name[DEVICE_NAME_SIZE + 1];

struct bt_mesh_proxy_client *bt_mesh_proxy_server_get_client(uint8_t index)
{
    return &clients[0];
}

uint8_t bt_mesh_proxy_server_get_client_count(void)
{
    return ARRAY_SIZE(clients);
}

int bt_mesh_set_device_name(const char *name)
{
    if (!name) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    if (strlen(name) > DEVICE_NAME_SIZE) {
        BT_ERR("Too long device name (len %d)", strlen(name));
        return -EINVAL;
    }

    memset(device_name, 0x0, sizeof(device_name));
    strncpy(device_name, name, DEVICE_NAME_SIZE);

    return bt_mesh_gatts_set_local_device_name(device_name);
}

const char *bt_mesh_get_device_name(void)
{
    return device_name;
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
        BT_ERR("Invalid proxy client parameter");
        return;
    }

    net_buf_simple_reset(&client->buf);
    bt_mesh_gatts_disconnect(client->conn, 0x13);
}

#if CONFIG_BLE_MESH_GATT_PROXY_SERVER
/**
 * The following callbacks are used to notify proper information
 * to the application layer.
 */
static proxy_server_connect_cb_t proxy_server_connect_cb;
static proxy_server_disconnect_cb_t proxy_server_disconnect_cb;

void bt_mesh_proxy_server_set_conn_cb(proxy_server_connect_cb_t cb)
{
    proxy_server_connect_cb = cb;
}

void bt_mesh_proxy_server_set_disconn_cb(proxy_server_disconnect_cb_t cb)
{
    proxy_server_disconnect_cb = cb;
}

/* Next subnet in queue to be advertised */
static int next_idx;

bool bt_mesh_proxy_server_find_client_by_addr(uint16_t addr)
{
    for (size_t i = 0; i < ARRAY_SIZE(clients); i++) {
        if (clients[i].conn) {
            for (size_t j = 0; j < ARRAY_SIZE(clients[i].filter); j++) {
                if (clients[i].filter[j].proxy_client && clients[i].filter[j].addr == addr) {
                    return true;
                }
            }
        }
    }

    return false;
}

uint8_t bt_mesh_proxy_server_get_all_client_type(void)
{
    uint8_t client_type = 0;

    for (size_t i = 0; i < ARRAY_SIZE(clients); i++) {
        if (clients[i].conn) {
            for (size_t j = 0; j < ARRAY_SIZE(clients[i].filter); j++) {
                if (clients[i].filter[j].proxy_client) {
                    client_type |= clients[i].proxy_client_type;
                }
            }
        }
    }

    return client_type;
}

static int filter_set(struct bt_mesh_proxy_client *client,
                      struct net_buf_simple *buf)
{
    uint8_t type = 0U;

    if (buf->len < 1) {
        BT_WARN("Too short Filter Set message");
        return -EINVAL;
    }

    type = net_buf_simple_pull_u8(buf);
    BT_INFO("Set filter type 0x%02x", type);

    switch (type) {
    case 0x00:
        (void)memset(client->filter, 0, sizeof(client->filter));
        client->filter_type = SRV_WHITELIST;
        break;
    case 0x01:
#if CONFIG_BLE_MESH_DF_SRV
        bt_mesh_directed_proxy_server_set_blacklist(client);
#endif /* CONFIG_BLE_MESH_DF_SRV */
        (void)memset(client->filter, 0, sizeof(client->filter));
        client->filter_type = SRV_BLACKLIST;
        break;
    default:
        BT_WARN("Prohibited Filter Type 0x%02x", type);
        return -EINVAL;
    }

    return 0;
}

static void filter_add(struct bt_mesh_proxy_client *client,
                       uint16_t addr, bool proxy_client)
{
    int i;

    /* The flag proxy_client is used to indicate if the added address
     * is the element address of Proxy Client.
     */

    BT_DBG("addr 0x%04x", addr);

    if (addr == BLE_MESH_ADDR_UNASSIGNED) {
        return;
    }

    if (!BLE_MESH_ADDR_IS_UNICAST(addr) && proxy_client == true) {
        BT_ERR("Invalid proxy client element addr 0x%04x", addr);
        return;
    }

    for (i = 0; i < ARRAY_SIZE(client->filter); i++) {
        if (client->filter[i].addr == addr) {
            return;
        }
    }

    for (i = 0; i < ARRAY_SIZE(client->filter); i++) {
        if (client->filter[i].addr == addr) {
            BT_INFO("client addr 0x%04x already added", addr);
            return;
        }
        if (client->filter[i].addr == BLE_MESH_ADDR_UNASSIGNED) {
            BT_INFO("Add client or filter addr 0x%04x", addr);
            client->filter[i].addr = addr;
            client->filter[i].proxy_client = proxy_client;
            return;
        }
    }

    BT_WARN("Proxy filter is full!");
}

static void filter_remove(struct bt_mesh_proxy_client *client, uint16_t addr)
{
    int i;

    BT_DBG("addr 0x%04x", addr);

    if (addr == BLE_MESH_ADDR_UNASSIGNED) {
        return;
    }

    for (i = 0; i < ARRAY_SIZE(client->filter); i++) {
        if (client->filter[i].addr == addr) {
            client->filter[i].addr = BLE_MESH_ADDR_UNASSIGNED;
            client->filter[i].proxy_client = false;
            BT_INFO("Remove filter addr 0x%04x", addr);
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
    uint16_t filter_size = 0U;
    int i, err = 0;

    /* Configuration messages always have dst unassigned */
    tx.ctx->addr = BLE_MESH_ADDR_UNASSIGNED;
    tx.ctx->send_cred = BLE_MESH_FLOODING_CRED,

    net_buf_simple_reset(buf);
    net_buf_simple_reserve(buf, 10);

    net_buf_simple_add_u8(buf, BLE_MESH_PROXY_CFG_FILTER_STATUS);

    if (client->filter_type == SRV_WHITELIST) {
        net_buf_simple_add_u8(buf, 0x00);
    } else {
        net_buf_simple_add_u8(buf, 0x01);
    }

    for (filter_size = 0U, i = 0; i < ARRAY_SIZE(client->filter); i++) {
        if (client->filter[i].addr != BLE_MESH_ADDR_UNASSIGNED) {
            filter_size++;
        }
    }

    net_buf_simple_add_be16(buf, filter_size);

    BT_DBG("%u bytes: %s", buf->len, bt_hex(buf->data, buf->len));

    err = bt_mesh_net_encode(&tx, buf, true);
    if (err) {
        BT_ERR("Encoding proxy filter status failed (err %d)", err);
        return;
    }

    err = bt_mesh_proxy_server_segment_send(client->conn, BLE_MESH_PROXY_CONFIG, buf);
    if (err) {
        BT_ERR("Failed to send proxy filter status (err %d)", err);
    }
}

static void proxy_cfg(struct bt_mesh_proxy_client *client)
{
    NET_BUF_SIMPLE_DEFINE(buf, 29);
    struct bt_mesh_net_rx rx = {0};
    uint8_t opcode = 0U;
    int err = 0;

    if (client->buf.len > 29) {
        BT_ERR("Too large proxy cfg pdu (len %d)", client->buf.len);
        return;
    }

    err = bt_mesh_net_decode(&client->buf, BLE_MESH_NET_IF_PROXY_CFG,
                             &rx, &buf);
    if (err) {
        BT_ERR("Failed to decode Proxy Configuration (err %d)", err);
        return;
    }

    rx.local_match = 1U;

    if (bt_mesh_rpl_check(&rx, NULL)) {
        BT_WARN("Replay: src 0x%04x dst 0x%04x seq 0x%06x",
                rx.ctx.addr, rx.ctx.recv_dst, rx.seq);
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
    case BLE_MESH_PROXY_CFG_FILTER_SET:
        filter_set(client, &buf);
        send_filter_status(client, &rx, &buf);
        break;

    case BLE_MESH_PROXY_CFG_FILTER_ADD:
        while (buf.len >= 2) {
            uint16_t addr = net_buf_simple_pull_be16(&buf);
            filter_add(client, addr, rx.ctx.addr == addr);
        }
        send_filter_status(client, &rx, &buf);
#if CONFIG_BLE_MESH_DF_SRV
        bt_mesh_directed_proxy_server_solicitation(client, rx.sub);
#endif /* CONFIG_BLE_MESH_DF_SRV */
        break;

    case BLE_MESH_PROXY_CFG_FILTER_REMOVE:
        while (buf.len >= 2) {
            uint16_t addr = net_buf_simple_pull_be16(&buf);
            filter_remove(client, addr);
        }
        send_filter_status(client, &rx, &buf);
        break;

#if CONFIG_BLE_MESH_DF_SRV
    case BLE_MESH_PROXY_CFG_DIRECTED_PROXY_CONTROL:
        if (buf.len < 1) {
            BT_WARN("Invalid Directed Proxy Control (len %d)", buf.len);
            break;
        }

        if (rx.sub->directed_proxy != BLE_MESH_DIRECTED_PROXY_NOT_SUPPORTED) {
            bt_mesh_directed_proxy_server_directed_proxy_ctrl_recv(client, &rx, &buf);
        }
        break;
#endif /* CONFIG_BLE_MESH_DF_SRV */

    default:
        BT_WARN("Unhandled configuration OpCode 0x%02x", opcode);
        break;
    }
}

static int beacon_send(struct bt_mesh_proxy_client *client, struct bt_mesh_subnet *sub)
{
    NET_BUF_SIMPLE_DEFINE(buf, 28);

    net_buf_simple_reserve(&buf, 1);

#if CONFIG_BLE_MESH_PROXY_PRIVACY
    if (client->proxy_privacy == BLE_MESH_PROXY_PRIVACY_ENABLED) {
        /* NOTE: Each time a Mesh Private beacon for a subnet is sent to a Proxy Client,
         * the Random field in the Mesh Private beacon shall be regenerated.
         */
        bt_mesh_private_beacon_update_random(sub);

        bt_mesh_private_beacon_create(sub, &buf);
    } else
#endif
    {
        bt_mesh_secure_beacon_create(sub, &buf);
    }

    return bt_mesh_proxy_server_segment_send(client->conn, BLE_MESH_PROXY_BEACON, &buf);
}

static void proxy_send_beacons(struct k_work *work)
{
    struct bt_mesh_proxy_client *client = NULL;
    int i;

    client = CONTAINER_OF(work, struct bt_mesh_proxy_client, send_beacons);

    /* Upon connection, the Proxy Server shall evaluate Proxy Privacy parameter
     * for the connection and the Proxy Server shall retain the value of the
     * Proxy Privacy parameter for the lifetime of the connection. The Proxy
     * Server shall send a mesh beacon for each known subnet to the Proxy Client.
     *
     * When either the GATT Proxy state or the Node Identity state is enabled,
     * the Proxy Privacy parameter for the connection shall be Disabled.
     *
     * When both the GATT Proxy state and the Node Identity state are disabled,
     * and either the Private GATT Proxy state or the Private Node Identity state
     * is enabled, the Proxy Privacy parameter for the connection shall be Enabled.
     */

    for (i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
        struct bt_mesh_subnet *sub = &bt_mesh.sub[i];

        if (sub->net_idx != BLE_MESH_KEY_UNUSED) {
            beacon_send(client, sub);

#if CONFIG_BLE_MESH_DF_SRV
            if (sub->directed_proxy != BLE_MESH_DIRECTED_PROXY_NOT_SUPPORTED) {
                bt_mesh_directed_proxy_server_directed_proxy_caps_status_send(client->conn, sub);
            }
#endif /* CONFIG_BLE_MESH_DF_SRV */
        }

#if 0
        if (sub->net_idx != BLE_MESH_KEY_UNUSED) {
            if (sub->proxy_privacy == BLE_MESH_PROXY_PRIVACY_DISABLED) {
                beacon_send(client->conn, sub);
            } else if (sub->proxy_privacy == BLE_MESH_PROXY_PRIVACY_ENABLED) {
#if CONFIG_BLE_MESH_PRB_SRV
                /* TODO: Send Mesh Private Beacon */
#endif
            }
        }
#endif
    }
}

void bt_mesh_proxy_server_beacon_send(struct bt_mesh_subnet *sub)
{
    int i;

    if (!sub) {
        /* NULL means we send on all subnets */
        for (i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
            if (bt_mesh.sub[i].net_idx != BLE_MESH_KEY_UNUSED) {
                bt_mesh_proxy_server_beacon_send(&bt_mesh.sub[i]);
            }
        }

        return;
    }

    for (i = 0; i < ARRAY_SIZE(clients); i++) {
        if (clients[i].conn) {
            beacon_send(&clients[i], sub);
        }
    }
}

void bt_mesh_proxy_server_identity_start(struct bt_mesh_subnet *sub)
{
    sub->node_id = BLE_MESH_NODE_IDENTITY_RUNNING;
    sub->node_id_start = k_uptime_get_32();

    /* Prioritize the recently enabled subnet */
    next_idx = sub - bt_mesh.sub;
}

void bt_mesh_proxy_server_identity_stop(struct bt_mesh_subnet *sub)
{
    sub->node_id = BLE_MESH_NODE_IDENTITY_STOPPED;
    sub->node_id_start = 0U;
}

int bt_mesh_proxy_identity_enable(void)
{
    int i, count = 0;

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

        bt_mesh_proxy_server_identity_start(sub);
        count++;
    }

    if (count) {
        bt_mesh_adv_update();
    }

    return 0;
}

#if CONFIG_BLE_MESH_PRB_SRV
void bt_mesh_proxy_server_private_identity_start(struct bt_mesh_subnet *sub)
{
    sub->private_node_id = BLE_MESH_PRIVATE_NODE_IDENTITY_RUNNING;
    sub->node_id_start = k_uptime_get_32();

    /* Prioritize the recently enabled subnet */
    next_idx = sub - bt_mesh.sub;
}

void bt_mesh_proxy_server_private_identity_stop(struct bt_mesh_subnet *sub)
{
    sub->private_node_id = BLE_MESH_PRIVATE_NODE_IDENTITY_STOPPED;
    sub->node_id_start = 0U;
}

bool bt_mesh_proxy_server_is_node_id_enable(void)
{
    for (size_t i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
        /* If the Node Identity state of the node for any subnet
         * is 0x01 (i.e. running), return true.
         */
        struct bt_mesh_subnet *sub = &bt_mesh.sub[i];

        if (sub->net_idx != BLE_MESH_KEY_UNUSED &&
            sub->node_id == BLE_MESH_PRIVATE_NODE_IDENTITY_RUNNING) {
            return true;
        }
    }

    return false;
}

static bool is_exist_private_node_id_enable(void)
{
    for (size_t i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
        /* If the value of the Node Identity state of the node
         * for any subnet is 0x01,If exist return true.
         */
        struct bt_mesh_subnet *sub = &bt_mesh.sub[i];

        if (sub->net_idx != BLE_MESH_KEY_UNUSED &&
            sub->private_node_id == BLE_MESH_PRIVATE_NODE_IDENTITY_RUNNING) {
            return true;
        }
    }

    return false;
}

int bt_mesh_proxy_private_identity_disable(void)
{
    if (!bt_mesh_is_provisioned()) {
        return -EAGAIN;
    }

    for (size_t i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
        /* NOTE: Set private node identity state of all valid subnets disabled */
        struct bt_mesh_subnet *sub = &bt_mesh.sub[i];

        if (sub->net_idx == BLE_MESH_KEY_UNUSED) {
            continue;
        }

        if (sub->private_node_id == BLE_MESH_PRIVATE_NODE_IDENTITY_NOT_SUPPORTED) {
            continue;
        }

        bt_mesh_proxy_server_private_identity_stop(sub);
    }

    return 0;
}

int bt_mesh_proxy_private_identity_enable(void)
{
    int count = 0;

    if (!bt_mesh_is_provisioned()) {
        return -EAGAIN;
    }

    for (size_t i = 0U; i < ARRAY_SIZE(bt_mesh.sub); i++) {
        struct bt_mesh_subnet *sub = &bt_mesh.sub[i];

        if (sub->net_idx == BLE_MESH_KEY_UNUSED) {
            continue;
        }

        if (sub->private_node_id == BLE_MESH_PRIVATE_NODE_IDENTITY_NOT_SUPPORTED) {
            continue;
        }

        bt_mesh_proxy_server_private_identity_start(sub);
        count++;
    }

    if (count) {
        bt_mesh_adv_update();
    }

    return 0;
}
#endif /* CONFIG_BLE_MESH_PRB_SRV */
#endif /* GATT_PROXY */

static void proxy_complete_pdu(struct bt_mesh_proxy_client *client)
{
    switch (client->msg_type) {
#if CONFIG_BLE_MESH_GATT_PROXY_SERVER
    case BLE_MESH_PROXY_NET_PDU:
        BT_DBG("Mesh Network PDU");
        bt_mesh_net_recv(&client->buf, 0, BLE_MESH_NET_IF_PROXY);
        break;
    case BLE_MESH_PROXY_BEACON:
        BT_DBG("Mesh Beacon PDU");
        bt_mesh_beacon_recv(&client->buf, 0);
        break;
    case BLE_MESH_PROXY_CONFIG:
        BT_DBG("Mesh Configuration PDU");
        proxy_cfg(client);
        break;
#endif
#if CONFIG_BLE_MESH_PB_GATT
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

#if CONFIG_BLE_MESH_GATT_PROXY_SERVER
    if (client->msg_type < BLE_MESH_PROXY_PROV &&
        client->proxy_msg_recv == false) {
        client->proxy_msg_recv = true;
        /**
         * @Spec: P626
         * When a new connection is established between a Proxy Client and the Directed Proxy Server, and the
         * first message received from the Proxy Client is a successfully processed DIRECTED_PROXY_CONTROL
         * message, then the Directed Proxy Server shall set the Proxy_Client_Type parameter to Directed Proxy Client,
         * shall set the Use_Directed parameter to Disable for all subnets known to the Directed Proxy Server
         * except the subnet identified by the received message;
         * otherwise, the Directed Proxy Server shall set the Proxy_Client_Type parameter to Proxy Client.
         *
         * If the first message received is DIRECTED_PROXY_CONTROL, proxy_client_type will be set to Directed Proxy Client,
         * But if device didn't receive DIRECTED_PROXY_CONTROL message and all received is normal proxy message, That
         * client type will be always in UNSET state, because we set client type in handle function of DIRECTED_PROXY_CONTROL.
         *
         * So the flowing code was used to avoid that situation.
        */
        if (client->proxy_client_type == BLE_MESH_PROXY_CLI_TYPE_UNSET) {
            client->proxy_client_type = BLE_MESH_PROXY_CLI_TYPE_PROXY_CLIENT;
        }
    }
#endif
}

#define ATTR_IS_PROV(attr) (attr->user_data != NULL)

static ssize_t proxy_recv(struct bt_mesh_conn *conn,
                          const struct bt_mesh_gatt_attr *attr, const void *buf,
                          uint16_t len, uint16_t offset, uint8_t flags)
{
    struct bt_mesh_proxy_client *client = find_client(conn);
    const uint8_t *data = buf;

    if (!client) {
        BT_ERR("No Proxy Client found");
        return -ENOTCONN;
    }

    if (len < 1) {
        BT_WARN("Too small Proxy PDU");
        return -EINVAL;
    }

    if (ATTR_IS_PROV(attr) != (BLE_MESH_PROXY_PDU_TYPE(data) == BLE_MESH_PROXY_PROV)) {
        BT_WARN("Proxy PDU type doesn't match GATT service");
        return -EINVAL;
    }

    if (len - 1 > net_buf_simple_tailroom(&client->buf)) {
        BT_WARN("Too big proxy PDU");
        return -EINVAL;
    }

    switch (BLE_MESH_PROXY_PDU_SAR(data)) {
    case BLE_MESH_PROXY_SAR_COMP:
        if (client->buf.len) {
            BT_WARN("Complete PDU while a pending incomplete one");
            return -EINVAL;
        }

        client->msg_type = BLE_MESH_PROXY_PDU_TYPE(data);
        net_buf_simple_add_mem(&client->buf, data + 1, len - 1);
        proxy_complete_pdu(client);
        break;

    case BLE_MESH_PROXY_SAR_FIRST:
        if (client->buf.len) {
            BT_WARN("First PDU while a pending incomplete one");
            return -EINVAL;
        }

        k_delayed_work_submit(&client->sar_timer, BLE_MESH_PROXY_SAR_TIMEOUT);
        client->msg_type = BLE_MESH_PROXY_PDU_TYPE(data);
        net_buf_simple_add_mem(&client->buf, data + 1, len - 1);
        break;

    case BLE_MESH_PROXY_SAR_CONT:
        if (!client->buf.len) {
            BT_WARN("Continuation with no prior data");
            return -EINVAL;
        }

        if (client->msg_type != BLE_MESH_PROXY_PDU_TYPE(data)) {
            BT_WARN("Unexpected message type in continuation");
            return -EINVAL;
        }

        k_delayed_work_submit(&client->sar_timer, BLE_MESH_PROXY_SAR_TIMEOUT);
        net_buf_simple_add_mem(&client->buf, data + 1, len - 1);
        break;

    case BLE_MESH_PROXY_SAR_LAST:
        if (!client->buf.len) {
            BT_WARN("Last SAR PDU with no prior data");
            return -EINVAL;
        }

        if (client->msg_type != BLE_MESH_PROXY_PDU_TYPE(data)) {
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

static void proxy_connected(struct bt_mesh_conn *conn, uint8_t err)
{
    struct bt_mesh_proxy_client *client = NULL;
    int i;

    BT_DBG("conn %p err 0x%02x", conn, err);

    conn_count++;

    /* Since we use ADV_OPT_ONE_TIME */
    proxy_adv_enabled = false;

#if CONFIG_BLE_MESH_PROXY_SOLIC_PDU_RX
    /* Before re-enabling advertising, stop advertising
     * the Service Data associated with the Private
     * Network Identity type.
     */
    bt_mesh_proxy_server_stop_solic_adv_priv_net_id();
#endif

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
        BT_ERR("No free Proxy Client objects");
        return;
    }

    client->conn = bt_mesh_conn_ref(conn);
    client->filter_type = SRV_NONE;
#if CONFIG_BLE_MESH_GATT_PROXY_SERVER
    (void)memset(client->filter, 0, sizeof(client->filter));

    if (proxy_server_connect_cb) {
        proxy_server_connect_cb(conn->handle);
    }
#endif
    net_buf_simple_reset(&client->buf);
}

static void proxy_disconnected(struct bt_mesh_conn *conn, uint8_t reason)
{
    int i;

    BT_DBG("conn %p reason 0x%02x", conn, reason);

    conn_count--;

    for (i = 0; i < ARRAY_SIZE(clients); i++) {
        struct bt_mesh_proxy_client *client = &clients[i];

        if (client->conn == conn) {
#if CONFIG_BLE_MESH_GATT_PROXY_SERVER
            if (proxy_server_disconnect_cb) {
                proxy_server_disconnect_cb(conn->handle, reason);
            }
#endif
            if (IS_ENABLED(CONFIG_BLE_MESH_PB_GATT) &&
                client->filter_type == SRV_PROV) {
                bt_mesh_pb_gatt_close(conn, reason);
            }

#if CONFIG_BLE_MESH_PROXY_PRIVACY
            client->proxy_privacy = BLE_MESH_PROXY_PRIVACY_DISABLED;
#endif

#if CONFIG_BLE_MESH_GATT_PROXY_SERVER && CONFIG_BLE_MESH_PRB_SRV
            k_delayed_work_cancel(&rand_upd_timer);
#endif

            k_delayed_work_cancel(&client->sar_timer);
            bt_mesh_conn_unref(client->conn);
            client->conn = NULL;
            break;
        }
    }

    bt_mesh_adv_update();

#if CONFIG_BLE_MESH_GATT_PROXY_SERVER && CONFIG_BLE_MESH_DF_SRV
    if (i == ARRAY_SIZE(clients)) {
        BT_WARN("Proxy disconnected, but no connection found");
        return;
    }

    if (bt_mesh_directed_proxy_server_update_dep_node(NULL, &clients[i], 0)) {
        BT_ERR("Proxy disconnected, failed to update dependent node");
    }
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

struct net_buf_simple *bt_mesh_proxy_server_get_buf(void)
{
    struct net_buf_simple *buf = &clients[0].buf;

    net_buf_simple_reset(buf);

    return buf;
}

#if CONFIG_BLE_MESH_PB_GATT
static ssize_t prov_ccc_write(struct bt_mesh_conn *conn,
                              const struct bt_mesh_gatt_attr *attr,
                              const void *buf, uint16_t len,
                              uint16_t offset, uint8_t flags)
{
    struct bt_mesh_proxy_client *client = NULL;
    uint16_t *value = attr->user_data;

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
    if (!client) {
        BT_ERR("No client for connection %p", conn);
        return 0;
    }

    if (client->filter_type == SRV_NONE) {
        client->filter_type = SRV_PROV;
        bt_mesh_pb_gatt_open(conn);
    }

    return len;
}

static ssize_t prov_ccc_read(struct bt_mesh_conn *conn,
                             const struct bt_mesh_gatt_attr *attr,
                             void *buf, uint16_t len, uint16_t offset)
{
    uint16_t *value = attr->user_data;

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

int bt_mesh_proxy_server_prov_enable(void)
{
    int i;

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
            clients[i].filter_type = SRV_PROV;
        }
    }


    return 0;
}

int bt_mesh_proxy_server_prov_disable(bool disconnect)
{
    int i;

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

        if (!client->conn || client->filter_type != SRV_PROV) {
            continue;
        }

        if (disconnect) {
            bt_mesh_gatts_disconnect(client->conn, 0x13);
        } else {
            bt_mesh_pb_gatt_close(client->conn, CLOSE_REASON_SUCCESS);
            client->filter_type = SRV_NONE;
        }
    }

    bt_mesh_adv_update();

    return 0;
}
#endif /* CONFIG_BLE_MESH_PB_GATT */

#if CONFIG_BLE_MESH_GATT_PROXY_SERVER
static ssize_t proxy_ccc_write(struct bt_mesh_conn *conn,
                               const struct bt_mesh_gatt_attr *attr,
                               const void *buf, uint16_t len,
                               uint16_t offset, uint8_t flags)
{
    struct bt_mesh_proxy_client *client = NULL;
    uint16_t value = 0U;

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
    if (!client) {
        BT_ERR("No client for connection %p", conn);
        return 0;
    }

    /* Spec section 6.7
     * Upon connection, the Proxy Server shall evaluate Proxy Privacy parameter (for GATT Proxy bearer see
     * Section 7.2.2.2.6) for the connection and the Proxy Server shall retain the value of the Proxy Privacy
     * parameter for the lifetime of the connection. The Proxy Server shall send a mesh beacon for each known
     * subnet to the Proxy Client
    */
#if CONFIG_BLE_MESH_PRB_SRV
    if (bt_mesh_gatt_proxy_get() == BLE_MESH_GATT_PROXY_ENABLED ||
        bt_mesh_proxy_server_is_node_id_enable()) {
        client->proxy_privacy = BLE_MESH_PROXY_PRIVACY_DISABLED;
    } else if (bt_mesh_private_gatt_proxy_state_get() == BLE_MESH_PRIVATE_GATT_PROXY_ENABLED ||
               is_exist_private_node_id_enable()) {
        client->proxy_privacy = BLE_MESH_PROXY_PRIVACY_ENABLED;
    }
#endif /* CONFIG_BLE_MESH_PRB_SRV */

    if (client->filter_type == SRV_NONE) {
        client->filter_type = SRV_WHITELIST;
        k_delayed_work_submit(&client->send_beacons, K_MSEC(15));
    }

    if (client->proxy_client_type == 0) {
        client->proxy_client_type = BLE_MESH_PROXY_CLI_TYPE_UNSET;
    }

#if CONFIG_BLE_MESH_DF_SRV
    bt_mesh_directed_proxy_server_connected(client);
#endif /* CONFIG_BLE_MESH_DF_SRV */

    return len;
}

static ssize_t proxy_ccc_read(struct bt_mesh_conn *conn,
                              const struct bt_mesh_gatt_attr *attr,
                              void *buf, uint16_t len, uint16_t offset)
{
    uint16_t *value = attr->user_data;

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

int bt_mesh_proxy_server_gatt_enable(void)
{
    int i;

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
            clients[i].filter_type = SRV_WHITELIST;
        }
    }

    return 0;
}

void bt_mesh_proxy_server_gatt_disconnect(void)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(clients); i++) {
        struct bt_mesh_proxy_client *client = &clients[i];

        if (client->conn && (client->filter_type == SRV_WHITELIST ||
                             client->filter_type == SRV_BLACKLIST)) {
            client->filter_type = SRV_NONE;
            bt_mesh_gatts_disconnect(client->conn, 0x13);
        }
    }
}

int bt_mesh_proxy_server_gatt_disable(void)
{
    if (gatt_svc == MESH_GATT_NONE) {
        BT_WARN("%s, Already", __func__);
        return -EALREADY;
    }

    if (gatt_svc != MESH_GATT_PROXY) {
        BT_WARN("%s, Busy", __func__);
        return -EBUSY;
    }

    bt_mesh_proxy_server_gatt_disconnect();

    bt_mesh_gatts_service_stop(&proxy_svc);
    gatt_svc = MESH_GATT_NONE;

    return 0;
}

void bt_mesh_proxy_server_addr_add(struct net_buf_simple *buf, uint16_t addr)
{
    struct bt_mesh_proxy_client *client = CONTAINER_OF(buf,
                                          struct bt_mesh_proxy_client,
                                          buf);

    BT_DBG("filter_type %u addr 0x%04x", client->filter_type, addr);

    if (client->filter_type == SRV_WHITELIST) {
        filter_add(client, addr, true);
    } else if (client->filter_type == SRV_BLACKLIST) {
        filter_remove(client, addr);
    }
}

static bool client_filter_match(struct bt_mesh_proxy_client *client,
                                uint16_t addr)
{
    int i;

    BT_DBG("filter_type %u addr 0x%04x", client->filter_type, addr);

    if (client->filter_type == SRV_BLACKLIST) {
        for (i = 0; i < ARRAY_SIZE(client->filter); i++) {
            if (client->filter[i].addr == addr) {
                return false;
            }
        }

        return true;
    }

    if (addr == BLE_MESH_ADDR_ALL_NODES) {
        return true;
    }

    if (client->filter_type == SRV_WHITELIST) {
        for (i = 0; i < ARRAY_SIZE(client->filter); i++) {
            if (client->filter[i].addr == addr) {
                return true;
            }
        }
    }

    return false;
}

bool bt_mesh_proxy_server_relay(struct net_buf_simple *buf, uint16_t dst)
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

        bt_mesh_proxy_server_send(client->conn, BLE_MESH_PROXY_NET_PDU, &msg);
        relayed = true;
    }

    return relayed;
}

#endif /* CONFIG_BLE_MESH_GATT_PROXY_SERVER */

static int proxy_send(struct bt_mesh_conn *conn, const void *data, uint16_t len)
{
    BT_DBG("%u bytes: %s", len, bt_hex(data, len));

#if CONFIG_BLE_MESH_GATT_PROXY_SERVER
    if (gatt_svc == MESH_GATT_PROXY) {
        return bt_mesh_gatts_notify(conn, &proxy_attrs[4], data, len);
    }
#endif

#if CONFIG_BLE_MESH_PB_GATT
    if (gatt_svc == MESH_GATT_PROV) {
        return bt_mesh_gatts_notify(conn, &prov_attrs[4], data, len);
    }
#endif

    return 0;
}

int bt_mesh_proxy_server_segment_send(struct bt_mesh_conn *conn, uint8_t type,
                                      struct net_buf_simple *msg)
{
    uint16_t mtu = 0U;

    BT_DBG("conn %p type 0x%02x len %u: %s", conn, type, msg->len,
           bt_hex(msg->data, msg->len));

    /* ATT_MTU - OpCode (1 byte) - Handle (2 bytes) */
    mtu = bt_mesh_gatt_get_mtu(conn) - 3;
    if (mtu > msg->len) {
        net_buf_simple_push_u8(msg, BLE_MESH_PROXY_PDU_HDR(BLE_MESH_PROXY_SAR_COMP, type));
        return proxy_send(conn, msg->data, msg->len);
    }

    net_buf_simple_push_u8(msg, BLE_MESH_PROXY_PDU_HDR(BLE_MESH_PROXY_SAR_FIRST, type));
    proxy_send(conn, msg->data, mtu);
    net_buf_simple_pull(msg, mtu);

    while (msg->len) {
        if (msg->len + 1 < mtu) {
            net_buf_simple_push_u8(msg, BLE_MESH_PROXY_PDU_HDR(BLE_MESH_PROXY_SAR_LAST, type));
            proxy_send(conn, msg->data, msg->len);
            break;
        }

        net_buf_simple_push_u8(msg, BLE_MESH_PROXY_PDU_HDR(BLE_MESH_PROXY_SAR_CONT, type));
        proxy_send(conn, msg->data, mtu);
        net_buf_simple_pull(msg, mtu);
    }

    return 0;
}

int bt_mesh_proxy_server_send(struct bt_mesh_conn *conn, uint8_t type,
                              struct net_buf_simple *msg)
{
    struct bt_mesh_proxy_client *client = find_client(conn);

    if (!client) {
        BT_ERR("No Proxy Client found");
        return -ENOTCONN;
    }

    if ((client->filter_type == SRV_PROV) != (type == BLE_MESH_PROXY_PROV)) {
        BT_ERR("Invalid PDU type for Proxy Client");
        return -EINVAL;
    }

    return bt_mesh_proxy_server_segment_send(conn, type, msg);
}

#if CONFIG_BLE_MESH_PB_GATT
static uint8_t prov_svc_data[20] = { 0x27, 0x18, };

static const struct bt_mesh_adv_data prov_ad[] = {
    BLE_MESH_ADV_DATA_BYTES(BLE_MESH_DATA_FLAGS, (BLE_MESH_AD_GENERAL | BLE_MESH_AD_NO_BREDR)),
    BLE_MESH_ADV_DATA_BYTES(BLE_MESH_DATA_UUID16_ALL, 0x27, 0x18),
    BLE_MESH_ADV_DATA(BLE_MESH_DATA_SVC_DATA16, prov_svc_data, sizeof(prov_svc_data)),
};
#endif /* PB_GATT */

#if CONFIG_BLE_MESH_GATT_PROXY_SERVER

#define NET_ID_LEN              11
#define NODE_ID_LEN             19
#define PRIVATE_NET_ID_LEN      19
#define PRIVATE_NODE_ID_LEN     19
#define PROXY_ADV_MAX_LEN       19

#define NODE_ID_TIMEOUT         K_SECONDS(CONFIG_BLE_MESH_NODE_ID_TIMEOUT)

/* Note:
 * The "proxy_svc_data" should be large enough to hold the maximum
 * payload of the Node ID/Net ID/Private Node ID/Private Net ID.
 */
static uint8_t proxy_svc_data[PROXY_ADV_MAX_LEN] = { 0x28, 0x18, };

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

#if CONFIG_BLE_MESH_PRB_SRV || CONFIG_BLE_MESH_PROXY_SOLIC_PDU_RX
static const struct bt_mesh_adv_data private_node_id_ad[] = {
    BLE_MESH_ADV_DATA_BYTES(BLE_MESH_DATA_FLAGS, (BLE_MESH_AD_GENERAL | BLE_MESH_AD_NO_BREDR)),
    BLE_MESH_ADV_DATA_BYTES(BLE_MESH_DATA_UUID16_ALL, 0x28, 0x18),
    BLE_MESH_ADV_DATA(BLE_MESH_DATA_SVC_DATA16, proxy_svc_data, PRIVATE_NODE_ID_LEN),
};

static const struct bt_mesh_adv_data private_net_id_ad[] = {
    BLE_MESH_ADV_DATA_BYTES(BLE_MESH_DATA_FLAGS, (BLE_MESH_AD_GENERAL | BLE_MESH_AD_NO_BREDR)),
    BLE_MESH_ADV_DATA_BYTES(BLE_MESH_DATA_UUID16_ALL, 0x28, 0x18),
    BLE_MESH_ADV_DATA(BLE_MESH_DATA_SVC_DATA16, proxy_svc_data, PRIVATE_NET_ID_LEN),
};
#endif /* CONFIG_BLE_MESH_PRB_SRV */

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

    proxy_sd->data = (const uint8_t *)name;

    return 1;
}

static int node_id_adv(struct bt_mesh_subnet *sub)
{
    struct bt_mesh_adv_data proxy_sd = {0};
    size_t proxy_sd_len = 0U;
    uint8_t tmp[16] = {0};
    int err = 0;

    proxy_svc_data[2] = BLE_MESH_PROXY_ADV_NODE_ID;

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
    size_t proxy_sd_len = 0U;
    int err = 0;

    proxy_svc_data[2] = BLE_MESH_PROXY_ADV_NET_ID;

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

#if CONFIG_BLE_MESH_PRB_SRV
void bt_mesh_proxy_server_update_net_id_rand(void)
{
    k_delayed_work_submit(&rand_upd_timer, RAND_UPDATE_INTERVAL);
}

void bt_mesh_proxy_server_update_net_id_rand_stop(void)
{
    k_delayed_work_cancel(&rand_upd_timer);
}

static void random_update_timeout(struct k_work *work)
{
    int err = 0;

    err = bt_mesh_rand(net_id_random, 8);
    if (err) {
        BT_ERR("Generate random value failed");
        return;
    }

    k_delayed_work_submit(&rand_upd_timer, RAND_UPDATE_INTERVAL);
}

static int private_node_id_adv(struct bt_mesh_subnet *sub)
{
    struct bt_mesh_adv_data proxy_sd = {0};
    size_t proxy_sd_len = 0U;
    uint8_t tmp[16] = {0};
    int err = 0;

    proxy_svc_data[2] = BLE_MESH_PROXY_ADV_PRIVATE_NODE_ID;

    err = bt_mesh_rand(proxy_svc_data + 11, 8);
    if (err) {
        return err;
    }

    memset(tmp, 0, 5);
    tmp[5] = 0x03;
    memcpy(tmp + 6, proxy_svc_data + 11, 8);
    sys_put_be16(bt_mesh_primary_addr(), tmp + 14);

    err = bt_mesh_encrypt_be(sub->keys[sub->kr_flag].identity, tmp, tmp);
    if (err) {
        return err;
    }

    memcpy(proxy_svc_data + 3, tmp + 8, 8);
    proxy_sd_len = gatt_proxy_adv_create(&proxy_sd);

    err = bt_le_adv_start(&fast_adv_param, private_node_id_ad,
                          ARRAY_SIZE(private_node_id_ad), &proxy_sd, proxy_sd_len);
    if (err) {
        BT_WARN("Failed to advertise with Private Node ID (err %d)", err);
        return err;
    }

    proxy_adv_enabled = true;

    return 0;
}
#endif /* CONFIG_BLE_MESH_PRB_SRV */

#if (CONFIG_BLE_MESH_PRB_SRV || \
     CONFIG_BLE_MESH_PROXY_SOLIC_PDU_RX)
static int private_net_id_adv(struct bt_mesh_subnet *sub)
{
    struct bt_mesh_adv_data proxy_sd = {0};
    size_t proxy_sd_len = 0U;
    uint8_t tmp[16] = {0};
    int err = 0;

    proxy_svc_data[2] = BLE_MESH_PROXY_ADV_PRIVATE_NET_ID;

    /* TODO:
     * The Private Network Identity advertisements shall use a resolvable private
     * address or a non-resolvable private address in the AdvA field of the connectable
     * advertising PDU. The address used for the AdvA field shall be regenerated
     * whenever the Random field is regenerated.
     */
    /* The Random field should be updated every 10 minutes. */
    memcpy(proxy_svc_data + 11, net_id_random, 8);
    memcpy(tmp, sub->keys[sub->kr_flag].net_id, 8);
    memcpy(tmp + 8, net_id_random, 8);

    err = bt_mesh_encrypt_be(sub->keys[sub->kr_flag].identity, tmp, tmp);
    if (err) {
        return err;
    }

    memcpy(proxy_svc_data + 3, tmp + 8, 8); /* Hash */

    proxy_sd_len = gatt_proxy_adv_create(&proxy_sd);

    err = bt_le_adv_start(&fast_adv_param, private_net_id_ad,
                          ARRAY_SIZE(private_net_id_ad), &proxy_sd, proxy_sd_len);
    if (err) {
        BT_WARN("Failed to advertise with Private Net ID (err %d)", err);
        return err;
    }

    proxy_adv_enabled = true;

    return 0;
}
#endif  /* (CONFIG_BLE_MESH_PRB_SRV || \
            CONFIG_BLE_MESH_PROXY_SOLIC_PDU_RX) */

static bool advertise_subnet(struct bt_mesh_subnet *sub)
{
    if (sub->net_idx == BLE_MESH_KEY_UNUSED) {
        return false;
    }

    return (sub->node_id == BLE_MESH_NODE_IDENTITY_RUNNING ||
            bt_mesh_gatt_proxy_get() == BLE_MESH_GATT_PROXY_ENABLED
#if CONFIG_BLE_MESH_PRB_SRV
            || sub->private_node_id == BLE_MESH_PRIVATE_NODE_IDENTITY_RUNNING ||
            bt_mesh_private_gatt_proxy_state_get() == BLE_MESH_PRIVATE_GATT_PROXY_ENABLED
#endif
            );
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

    /* If among [next_idx, ARRAY_SIZE(bt_mesh.sub)], there is no subnet
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

static int32_t gatt_proxy_advertise(struct bt_mesh_subnet *sub)
{
    int32_t remaining = K_FOREVER;
    int subnet_count = 0;
    uint32_t active = 0U;

    if (conn_count == BLE_MESH_MAX_CONN) {
        BT_WARN("Connectable advertising deferred (max connections %d)", conn_count);
        return remaining;
    }

    if (!sub) {
        BT_WARN("No subnets to advertise on");
        return remaining;
    }

    if (sub->node_id == BLE_MESH_NODE_IDENTITY_RUNNING) {
        active = k_uptime_get_32() - sub->node_id_start;

        if (active < NODE_ID_TIMEOUT) {
            remaining = NODE_ID_TIMEOUT - active;
            BT_DBG("Node ID active for %u ms, %d ms remaining", active, remaining);
            node_id_adv(sub);
        } else {
            bt_mesh_proxy_server_identity_stop(sub);
            BT_DBG("Node ID stopped");
        }
    }
#if CONFIG_BLE_MESH_PRB_SRV
    else if (sub->private_node_id == BLE_MESH_PRIVATE_NODE_IDENTITY_RUNNING) {
        active = k_uptime_get_32() - sub->node_id_start;

        if (active < NODE_ID_TIMEOUT) {
            remaining = NODE_ID_TIMEOUT - active;
            BT_DBG("Private Node ID active for %u ms, %d ms remaining", active, remaining);
            private_node_id_adv(sub);
        } else {
            bt_mesh_proxy_server_private_identity_stop(sub);
            BT_DBG("Private Node ID stopped");
        }
    }
#endif /* CONFIG_BLE_MESH_PRB_SRV */

    if (sub->node_id == BLE_MESH_NODE_IDENTITY_STOPPED
#if CONFIG_BLE_MESH_PRB_SRV
        && sub->private_node_id == BLE_MESH_PRIVATE_NODE_IDENTITY_STOPPED
#endif
        ) {
        if (bt_mesh_gatt_proxy_get() == BLE_MESH_GATT_PROXY_ENABLED) {
            net_id_adv(sub);
        }
#if CONFIG_BLE_MESH_PRB_SRV
        else if (bt_mesh_private_gatt_proxy_state_get() == BLE_MESH_PRIVATE_GATT_PROXY_ENABLED) {
            private_net_id_adv(sub);
        }
#endif
    }

    subnet_count = sub_count();
    BT_DBG("sub_count %u", subnet_count);

    if (subnet_count > 1) {
        int32_t max_timeout = 0;

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

#if CONFIG_BLE_MESH_PB_GATT
static size_t gatt_prov_adv_create(struct bt_mesh_adv_data prov_sd[2])
{
    const char *name = device_name;
    size_t name_len = strlen(name);
    size_t prov_sd_len = 0U;
    size_t sd_space = 31U;

    if (bt_mesh_prov_get() == NULL) {
        BT_ERR("No provisioning context provided");
        return 0;
    }

    memcpy(prov_svc_data + 2, bt_mesh_prov_get()->uuid, 16);
    /* According CSS, all the field within adv data shall be little-endian */
    sys_put_le16(bt_mesh_prov_get()->oob_info, prov_svc_data + 18);

    if (bt_mesh_prov_get()->uri) {
        size_t uri_len = strlen(bt_mesh_prov_get()->uri);

        if (uri_len > 29) {
            /* There's no way to shorten an URI */
            BT_WARN("Too long URI to fit advertising packet");
        } else {
            prov_sd[0].type = BLE_MESH_DATA_URI;
            prov_sd[0].data_len = uri_len;
            prov_sd[0].data = (const uint8_t *)bt_mesh_prov_get()->uri;
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

        prov_sd[prov_sd_len].data = (const uint8_t *)name;
        prov_sd_len++;
    }

    return prov_sd_len;
}
#endif /* CONFIG_BLE_MESH_PB_GATT */

#if CONFIG_BLE_MESH_PROXY_SOLIC_PDU_RX
static int32_t solic_adv_private_net_id(void)
{
    uint16_t net_idx = BLE_MESH_KEY_UNUSED;
    struct bt_mesh_subnet *sub = NULL;
    int32_t remaining = 0;

    remaining = bt_mesh_proxy_server_get_solic_adv_remaining();
    if (remaining == 0) {
        return 0;
    }

    net_idx = bt_mesh_proxy_server_get_solic_adv_net_idx();
    if (net_idx == BLE_MESH_KEY_UNUSED) {
        return 0;
    }

    sub = bt_mesh_subnet_get(net_idx);
    if (sub == NULL) {
        return 0;
    }

    private_net_id_adv(sub);
    return remaining;
}
#endif /* CONFIG_BLE_MESH_PROXY_SOLIC_PDU_RX */

int32_t bt_mesh_proxy_server_adv_start(void)
{
    if (gatt_svc == MESH_GATT_NONE) {
        return K_FOREVER;
    }

#if CONFIG_BLE_MESH_PB_GATT
    if (prov_fast_adv) {
        prov_start_time = k_uptime_get_32();
    }

    if (!bt_mesh_is_provisioned()) {
        const struct bt_mesh_adv_param *param;
        struct bt_mesh_adv_data prov_sd[2];
        size_t prov_sd_len;

        if (k_uptime_get_32() - prov_start_time < K_SECONDS(60)) {
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

#if CONFIG_BLE_MESH_GATT_PROXY_SERVER
    if (bt_mesh_is_provisioned()) {
#if CONFIG_BLE_MESH_PROXY_SOLIC_PDU_RX
        /* When the server starts advertising with Private Network Identity
         * as a result of the reception of a Solicitation PDU, the server
         * shall only advertise the subnet corresponding to the network key
         * that is used to authenticate the Solicitation PDU; otherwise, the
         * server shall interleave the advertising of each of its subnets.
         */
        int32_t remaining = solic_adv_private_net_id();
        if (remaining) {
            return remaining;
        }
#endif /* CONFIG_BLE_MESH_PROXY_SOLIC_PDU_RX */

        return gatt_proxy_advertise(next_sub());
    }
#endif /* GATT_PROXY */

    return K_FOREVER;
}

void bt_mesh_proxy_server_adv_stop(void)
{
    int err = 0;

    BT_DBG("adv_enabled %u", proxy_adv_enabled);

    if (!proxy_adv_enabled) {
        return;
    }

    err = bt_le_adv_stop();
    if (err) {
        BT_ERR("Failed to stop advertising (err %d)", err);
    } else {
        proxy_adv_enabled = false;
    }
}

static struct bt_mesh_conn_cb conn_callbacks = {
    .connected = proxy_connected,
    .disconnected = proxy_disconnected,
};

int bt_mesh_proxy_server_init(void)
{
    int i;

#if CONFIG_BLE_MESH_GATT_PROXY_SERVER
    bt_mesh_gatts_service_register(&proxy_svc);
#endif

#if CONFIG_BLE_MESH_PB_GATT
    bt_mesh_gatts_service_register(&prov_svc);
#endif

    /* Initialize the client receive buffers */
    for (i = 0; i < ARRAY_SIZE(clients); i++) {
        struct bt_mesh_proxy_client *client = &clients[i];

        client->buf.size = BLE_MESH_PROXY_BUF_SIZE;
        client->buf.__buf = client_buf_data + (i * BLE_MESH_PROXY_BUF_SIZE);
#if defined(CONFIG_BLE_MESH_GATT_PROXY_SERVER)
        k_delayed_work_init(&client->send_beacons, proxy_send_beacons);
#endif
        k_delayed_work_init(&client->sar_timer, proxy_sar_timeout);
    }

#if CONFIG_BLE_MESH_GATT_PROXY_SERVER && CONFIG_BLE_MESH_PRB_SRV
    if (k_delayed_work_init(&rand_upd_timer, random_update_timeout)) {
        BT_ERR("Failed to create a random update timer");
        return -EIO;
    }
#endif

    bt_mesh_gatts_conn_cb_register(&conn_callbacks);

    strncpy(device_name, "ESP-BLE-MESH", DEVICE_NAME_SIZE);
    return bt_mesh_gatts_set_local_device_name(device_name);
}

#if CONFIG_BLE_MESH_DEINIT
int bt_mesh_proxy_server_deinit(void)
{
    int i;

    proxy_adv_enabled = false;
    gatt_svc = MESH_GATT_NONE;

#if CONFIG_BLE_MESH_GATT_PROXY_SERVER
    bt_mesh_gatts_service_deregister(&proxy_svc);
    next_idx = 0;
#endif

#if CONFIG_BLE_MESH_PB_GATT
    bt_mesh_gatts_service_deregister(&prov_svc);
#endif

    for (i = 0; i < ARRAY_SIZE(clients); i++) {
        struct bt_mesh_proxy_client *client = &clients[i];
#if defined(CONFIG_BLE_MESH_GATT_PROXY_SERVER)
        k_delayed_work_free(&client->send_beacons);
#endif
        k_delayed_work_free(&client->sar_timer);
        memset(client, 0, sizeof(struct bt_mesh_proxy_client));
    }

#if CONFIG_BLE_MESH_GATT_PROXY_SERVER && CONFIG_BLE_MESH_PRB_SRV
    k_delayed_work_free(&rand_upd_timer);
#endif

    memset(client_buf_data, 0, sizeof(client_buf_data));
    memset(device_name, 0, sizeof(device_name));

    bt_mesh_gatts_conn_cb_deregister();
    conn_count = 0;

    return 0;
}
#endif /* CONFIG_BLE_MESH_DEINIT */

#endif /* (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_GATT) || \
           CONFIG_BLE_MESH_GATT_PROXY_SERVER */
