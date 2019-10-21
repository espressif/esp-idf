// Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string.h>
#include <errno.h>

#include "sdkconfig.h"

#include "mesh.h"
#include "mesh_bearer_adapt.h"
#include "mesh_trace.h"
#include "mesh_common.h"

#include "net.h"
#include "access.h"
#include "beacon.h"
#include "foundation.h"
#include "provisioner_prov.h"
#include "proxy_client.h"
#include "provisioner_beacon.h"
#include "provisioner_main.h"

#define PDU_TYPE(data)      (data[0] & BIT_MASK(6))
#define PDU_SAR(data)       (data[0] >> 6)

#define PROXY_SAR_TIMEOUT   K_SECONDS(20)

#define SAR_COMPLETE        0x00
#define SAR_FIRST           0x01
#define SAR_CONT            0x02
#define SAR_LAST            0x03

#define PDU_HDR(sar, type)  (sar << 6 | (type & BIT_MASK(6)))

#define SERVER_BUF_SIZE     68

#if (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) || \
    CONFIG_BLE_MESH_GATT_PROXY_CLIENT

static struct bt_mesh_proxy_server {
    struct bt_mesh_conn *conn;
    enum __packed {
        NONE,
        PROV,
        PROXY,
    } conn_type;
#if CONFIG_BLE_MESH_GATT_PROXY_CLIENT
    u16_t net_idx;
#endif
    u8_t msg_type;
    struct k_delayed_work sar_timer;
    struct net_buf_simple buf;
} servers[BLE_MESH_MAX_CONN];

static u8_t server_buf_data[SERVER_BUF_SIZE * BLE_MESH_MAX_CONN];

static struct bt_mesh_proxy_server *find_server(struct bt_mesh_conn *conn)
{
    u8_t i;

    for (i = 0U; i < ARRAY_SIZE(servers); i++) {
        if (servers[i].conn == conn) {
            return &servers[i];
        }
    }

    return NULL;
}

static void proxy_sar_timeout(struct k_work *work)
{
    struct bt_mesh_proxy_server *server = NULL;

    BT_DBG("%s", __func__);

    server = CONTAINER_OF(work, struct bt_mesh_proxy_server, sar_timer.work);
    if (!server || !server->conn) {
        BT_ERR("%s, Invalid proxy server parameter", __func__);
        return;
    }

    net_buf_simple_reset(&server->buf);
    bt_mesh_gattc_disconnect(server->conn);
}

#if CONFIG_BLE_MESH_GATT_PROXY_CLIENT
/**
 * The following callbacks are used to notify proper information
 * to the application layer.
 */
static proxy_client_recv_adv_cb_t proxy_client_adv_recv_cb;
static proxy_client_connect_cb_t proxy_client_connect_cb;
static proxy_client_disconnect_cb_t proxy_client_disconnect_cb;
static proxy_client_recv_filter_status_cb_t proxy_client_filter_status_recv_cb;

void bt_mesh_proxy_client_set_adv_recv_cb(proxy_client_recv_adv_cb_t cb)
{
    proxy_client_adv_recv_cb = cb;
}

void bt_mesh_proxy_client_set_conn_cb(proxy_client_connect_cb_t cb)
{
    proxy_client_connect_cb = cb;
}

void bt_mesh_proxy_client_set_disconn_cb(proxy_client_disconnect_cb_t cb)
{
    proxy_client_disconnect_cb = cb;
}

void bt_mesh_proxy_client_set_filter_status_cb(proxy_client_recv_filter_status_cb_t cb)
{
    proxy_client_filter_status_recv_cb = cb;
}

static void filter_status(struct bt_mesh_proxy_server *server,
                          struct bt_mesh_net_rx *rx,
                          struct net_buf_simple *buf)
{
    u8_t filter_type;
    u16_t list_size;

    if (buf->len != 3) {
        BT_ERR("%s, Invalid Proxy Filter Status length %d", __func__, buf->len);
        return;
    }

    filter_type = net_buf_simple_pull_u8(buf);
    if (filter_type > 0x01) {
        BT_ERR("%s, Invalid filter type 0x%02x", __func__, filter_type);
        return;
    }

    list_size = net_buf_simple_pull_be16(buf);

    BT_DBG("%s, filter_type 0x%02x list_size %d", __func__, filter_type, list_size);

    if (proxy_client_filter_status_recv_cb) {
        proxy_client_filter_status_recv_cb(server - servers, rx->ctx.addr, server->net_idx, filter_type, list_size);
    }

    return;
}

static void proxy_cfg(struct bt_mesh_proxy_server *server)
{
    NET_BUF_SIMPLE_DEFINE(buf, 29);
    struct bt_mesh_net_rx rx = {0};
    u8_t opcode;
    int err;

    err = bt_mesh_net_decode(&server->buf, BLE_MESH_NET_IF_PROXY_CFG,
                             &rx, &buf);
    if (err) {
        BT_ERR("%s, Failed to decode Proxy Configuration (err %d)", __func__, err);
        return;
    }

    if (!BLE_MESH_ADDR_IS_UNICAST(rx.ctx.addr)) {
        BT_ERR("%s, Proxy Configuration from non-unicast addr 0x%04x", __func__, rx.ctx.addr);
        return;
    }

    /* Remove network headers */
    net_buf_simple_pull(&buf, BLE_MESH_NET_HDR_LEN);

    BT_DBG("%u bytes: %s", buf.len, bt_hex(buf.data, buf.len));

    if (buf.len < 3) {
        BT_WARN("Too short proxy configuration PDU");
        return;
    }

    opcode = net_buf_simple_pull_u8(&buf);

    switch (opcode) {
    case BLE_MESH_PROXY_CFG_FILTER_STATUS:
        filter_status(server, &rx, &buf);
        break;
    default:
        BT_WARN("Unknown Proxy Configuration OpCode 0x%02x", opcode);
        break;
    }
}
#endif /* CONFIG_BLE_MESH_GATT_PROXY_CLIENT */

static void proxy_complete_pdu(struct bt_mesh_proxy_server *server)
{
    switch (server->msg_type) {
#if CONFIG_BLE_MESH_GATT_PROXY_CLIENT
    case BLE_MESH_PROXY_NET_PDU:
        BT_DBG("Mesh Network PDU");
        bt_mesh_net_recv(&server->buf, 0, BLE_MESH_NET_IF_PROXY);
        break;
    case BLE_MESH_PROXY_BEACON:
        BT_DBG("Mesh Beacon PDU");
        if (bt_mesh_is_provisioner_en()) {
#if CONFIG_BLE_MESH_PROVISIONER
            provisioner_beacon_recv(&server->buf);
#endif
        } else {
#if CONFIG_BLE_MESH_NODE
            bt_mesh_beacon_recv(&server->buf);
#endif
        }
        break;
    case BLE_MESH_PROXY_CONFIG:
        BT_DBG("Mesh Configuration PDU");
        proxy_cfg(server);
        break;
#endif /* CONFIG_BLE_MESH_GATT_PROXY_CLIENT */
#if CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT
    case BLE_MESH_PROXY_PROV:
        BT_DBG("Mesh Provisioning PDU");
        provisioner_pb_gatt_recv(server->conn, &server->buf);
        break;
#endif
    default:
        BT_WARN("Unhandled Message Type 0x%02x", server->msg_type);
        break;
    }

    net_buf_simple_reset(&server->buf);
}

#define ATTR_IS_PROV(uuid) (uuid == BLE_MESH_UUID_MESH_PROV_VAL)

static ssize_t proxy_recv(struct bt_mesh_conn *conn,
                          const struct bt_mesh_gatt_attr *attr, const void *buf,
                          u16_t len, u16_t offset, u8_t flags)
{
    struct bt_mesh_proxy_server *server = find_server(conn);
    const u8_t *data = buf;
    u16_t srvc_uuid;

    if (!server) {
        BT_ERR("%s, No Proxy Server object found", __func__);
        return -ENOTCONN;
    }

    if (len < 1) {
        BT_WARN("Too small Proxy PDU");
        return -EINVAL;
    }

    srvc_uuid = bt_mesh_gattc_get_service_uuid(conn);
    if (!srvc_uuid) {
        BT_ERR("%s, No service uuid found", __func__);
        return -ENOTCONN;
    }

    if (ATTR_IS_PROV(srvc_uuid) != (PDU_TYPE(data) == BLE_MESH_PROXY_PROV)) {
        BT_WARN("Proxy PDU type doesn't match GATT service uuid");
        return -EINVAL;
    }

    if (len - 1 > net_buf_simple_tailroom(&server->buf)) {
        BT_WARN("Too big proxy PDU");
        return -EINVAL;
    }

    switch (PDU_SAR(data)) {
    case SAR_COMPLETE:
        if (server->buf.len) {
            BT_WARN("Complete PDU while a pending incomplete one");
            return -EINVAL;
        }

        server->msg_type = PDU_TYPE(data);
        net_buf_simple_add_mem(&server->buf, data + 1, len - 1);
        proxy_complete_pdu(server);
        break;

    case SAR_FIRST:
        if (server->buf.len) {
            BT_WARN("First PDU while a pending incomplete one");
            return -EINVAL;
        }

        k_delayed_work_submit(&server->sar_timer, PROXY_SAR_TIMEOUT);
        server->msg_type = PDU_TYPE(data);
        net_buf_simple_add_mem(&server->buf, data + 1, len - 1);
        break;

    case SAR_CONT:
        if (!server->buf.len) {
            BT_WARN("Continuation with no prior data");
            return -EINVAL;
        }

        if (server->msg_type != PDU_TYPE(data)) {
            BT_WARN("Unexpected message type in continuation");
            return -EINVAL;
        }

        k_delayed_work_submit(&server->sar_timer, PROXY_SAR_TIMEOUT);
        net_buf_simple_add_mem(&server->buf, data + 1, len - 1);
        break;

    case SAR_LAST:
        if (!server->buf.len) {
            BT_WARN("Last SAR PDU with no prior data");
            return -EINVAL;
        }

        if (server->msg_type != PDU_TYPE(data)) {
            BT_WARN("Unexpected message type in last SAR PDU");
            return -EINVAL;
        }

        k_delayed_work_cancel(&server->sar_timer);
        net_buf_simple_add_mem(&server->buf, data + 1, len - 1);
        proxy_complete_pdu(server);
        break;
    }

    return len;
}


static int proxy_send(struct bt_mesh_conn *conn, const void *data, u16_t len)
{
    BT_DBG("%u bytes: %s", len, bt_hex(data, len));

    return bt_mesh_gattc_write_no_rsp(conn, NULL, data, len);
}

static int proxy_segment_and_send(struct bt_mesh_conn *conn, u8_t type,
                                  struct net_buf_simple *msg)
{
    u16_t mtu;
    int err;

    if (conn == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    BT_DBG("conn %p type 0x%02x len %u: %s", conn, type, msg->len,
           bt_hex(msg->data, msg->len));

    mtu = bt_mesh_gattc_get_mtu_info(conn);
    if (!mtu) {
        BT_ERR("%s, Conn used to get mtu does not exist", __func__);
        return -ENOTCONN;
    }

    /* ATT_MTU - OpCode (1 byte) - Handle (2 bytes) */
    mtu -= 3;
    if (mtu > msg->len) {
        net_buf_simple_push_u8(msg, PDU_HDR(SAR_COMPLETE, type));
        return proxy_send(conn, msg->data, msg->len);
    }

    net_buf_simple_push_u8(msg, PDU_HDR(SAR_FIRST, type));
    err = proxy_send(conn, msg->data, mtu);
    net_buf_simple_pull(msg, mtu);

    while (msg->len) {
        if (msg->len + 1 < mtu) {
            net_buf_simple_push_u8(msg, PDU_HDR(SAR_LAST, type));
            err = proxy_send(conn, msg->data, msg->len);
            break;
        }

        net_buf_simple_push_u8(msg, PDU_HDR(SAR_CONT, type));
        err = proxy_send(conn, msg->data, mtu);
        net_buf_simple_pull(msg, mtu);
    }

    return err;
}

int bt_mesh_proxy_prov_client_send(struct bt_mesh_conn *conn, u8_t type,
                                   struct net_buf_simple *msg)
{
    struct bt_mesh_proxy_server *server = find_server(conn);

    if (!server) {
        BT_ERR("$%s, No Proxy Server object found", __func__);
        return -ENOTCONN;
    }

    if ((server->conn_type == PROV) != (type == BLE_MESH_PROXY_PROV)) {
        BT_ERR("%s, Invalid PDU type for Proxy Server", __func__);
        return -EINVAL;
    }

    return proxy_segment_and_send(conn, type, msg);
}

static void proxy_connected(bt_mesh_addr_t *addr, struct bt_mesh_conn *conn, int id)
{
    struct bt_mesh_proxy_server *server = NULL;

    if (!servers[id].conn) {
        server = &servers[id];
    }

    if (!server) {
        BT_ERR("%s, No free Proxy Server objects", __func__);
        /** Disconnect current connection, clear part of prov_link
         *  information, like uuid, dev_addr, linking flag, etc.
         */
        bt_mesh_gattc_disconnect(conn);
        return;
    }

    server->conn = bt_mesh_conn_ref(conn);
    server->conn_type = NONE;
    net_buf_simple_reset(&server->buf);

    bt_mesh_gattc_exchange_mtu(id);
    return;
}

static void proxy_disconnected(bt_mesh_addr_t *addr, struct bt_mesh_conn *conn, u8_t reason)
{
    struct bt_mesh_proxy_server *server = find_server(conn);

    BT_DBG("conn %p, handle is %d, reason 0x%02x", conn, conn->handle, reason);

    if (!server) {
        BT_ERR("%s, No Proxy Server object found", __func__);
        return;
    }

#if CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT
    if (server->conn_type == PROV) {
        provisioner_pb_gatt_close(conn, reason);
    }
#endif

#if CONFIG_BLE_MESH_GATT_PROXY_CLIENT
    if (server->conn_type == PROXY) {
        if (proxy_client_disconnect_cb) {
            proxy_client_disconnect_cb(addr, server - servers, server->net_idx, reason);
        }
    }
#endif

    k_delayed_work_cancel(&server->sar_timer);
    server->conn = NULL;
    server->conn_type = NONE;
#if CONFIG_BLE_MESH_GATT_PROXY_CLIENT
    server->net_idx = BLE_MESH_KEY_UNUSED;
#endif

    return;
}

#if CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT
static ssize_t prov_write_ccc(bt_mesh_addr_t *addr, struct bt_mesh_conn *conn)
{
    struct bt_mesh_proxy_server *server = find_server(conn);

    if (!server) {
        BT_ERR("%s, No Proxy Server object found", __func__);
        return -ENOTCONN;
    }

    if (server->conn_type == NONE) {
        server->conn_type = PROV;

        if (provisioner_set_prov_conn(addr->val, server->conn)) {
            BT_ERR("%s, provisioner_set_prov_conn failed", __func__);
            bt_mesh_gattc_disconnect(server->conn);
            return -EIO;
        }
        return provisioner_pb_gatt_open(conn, addr->val);
    }

    return -ENOMEM;
}

static ssize_t prov_recv_ntf(struct bt_mesh_conn *conn, u8_t *data, u16_t len)
{
    struct bt_mesh_proxy_server *server = find_server(conn);

    if (!server) {
        BT_ERR("%s, No Proxy Server object found", __func__);
        return -ENOTCONN;
    }

    if (server->conn_type == PROV) {
        return proxy_recv(conn, NULL, data, len, 0, 0);
    }

    return -EINVAL;
}

int provisioner_pb_gatt_enable(void)
{
    u8_t i;

    BT_DBG("%s", __func__);

    for (i = 0U; i < ARRAY_SIZE(servers); i++) {
        if (servers[i].conn) {
            servers[i].conn_type = PROV;
        }
    }

    return 0;
}

int provisioner_pb_gatt_disable(void)
{
    u8_t i;

    BT_DBG("%s", __func__);

    for (i = 0U; i < ARRAY_SIZE(servers); i++) {
        struct bt_mesh_proxy_server *server = &servers[i];

        if (server->conn && server->conn_type == PROV) {
            bt_mesh_gattc_disconnect(server->conn);
            server->conn_type = NONE;
        }
    }

    return 0;
}
#endif /* CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT */

#if defined(CONFIG_BLE_MESH_GATT_PROXY_CLIENT)
static ssize_t proxy_write_ccc(bt_mesh_addr_t *addr, struct bt_mesh_conn *conn)
{
    struct bt_mesh_proxy_server *server = find_server(conn);

    if (!server) {
        BT_ERR("%s, No Proxy Server object found", __func__);
        return -ENOTCONN;
    }

    if (server->conn_type == NONE) {
        server->conn_type = PROXY;

        if (proxy_client_connect_cb) {
            proxy_client_connect_cb(addr, server - servers, server->net_idx);
        }
        return 0;
    }

    return -EINVAL;
}

static ssize_t proxy_recv_ntf(struct bt_mesh_conn *conn, u8_t *data, u16_t len)
{
    struct bt_mesh_proxy_server *server = find_server(conn);

    if (!server) {
        BT_ERR("%s, No Proxy Server object found", __func__);
        return -ENOTCONN;
    }

    if (server->conn_type == PROXY) {
        return proxy_recv(conn, NULL, data, len, 0, 0);
    }

    return -EINVAL;
}

/**
 * Currently proxy client does't need bt_mesh_proxy_client_enable() and
 * bt_mesh_proxy_client_disable() functions, and once they are used,
 * proxy client can be enabled to parse node_id_adv and net_id_adv in
 * order to support proxy client role.
 * And if gatt proxy is disabled, proxy client can stop handling these
 * two kinds of connectable advertising packets.
 */
int bt_mesh_proxy_client_enable(void)
{
    u8_t i;

    BT_DBG("%s", __func__);

    for (i = 0U; i < ARRAY_SIZE(servers); i++) {
        if (servers[i].conn) {
            servers[i].conn_type = PROXY;
        }
    }

    /**
     * TODO:
     * Once at leat one device has been provisioned, proxy client can be
     * set to allow receiving and parsing node_id & net_id adv packets,
     * and we may use a global flag to indicate this.
     */

    return 0;
}

int bt_mesh_proxy_client_disable(void)
{
    u8_t i;

    BT_DBG("%s", __func__);

    /**
     * TODO:
     * Once this function is invoked, proxy client shall stop handling
     * node_id & net_id adv packets, and if proxy connection exists,
     * it should be disconnected.
     */

    for (i = 0U; i < ARRAY_SIZE(servers); i++) {
        struct bt_mesh_proxy_server *server = &servers[i];

        if (server->conn && server->conn_type == PROXY) {
            bt_mesh_gattc_disconnect(server->conn);
            server->conn_type = NONE;
        }
    }

    return 0;
}
#endif /* CONFIG_BLE_MESH_GATT_PROXY_CLIENT */

static struct bt_mesh_prov_conn_cb conn_callbacks = {
    .connected = proxy_connected,
    .disconnected = proxy_disconnected,
#if CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT
    .prov_write_descr = prov_write_ccc,
    .prov_notify = prov_recv_ntf,
#endif /* CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT */
#if defined(CONFIG_BLE_MESH_GATT_PROXY_CLIENT)
    .proxy_write_descr = proxy_write_ccc,
    .proxy_notify = proxy_recv_ntf,
#endif /* CONFIG_BLE_MESH_GATT_PROXY_CLIENT */
};

#if defined(CONFIG_BLE_MESH_GATT_PROXY_CLIENT)
static struct bt_mesh_subnet *bt_mesh_is_net_id_exist(const u8_t net_id[8])
{
    struct bt_mesh_subnet *sub = NULL;
    size_t size, i;

    size = bt_mesh_rx_netkey_size();

    for (i = 0U; i < size; i++) {
        sub = bt_mesh_rx_netkey_get(i);
        if (sub && !memcmp(sub->keys[sub->kr_flag].net_id, net_id, 8)) {
            return sub;
        }
    }

    return NULL;
}

void proxy_client_adv_ind_recv(struct net_buf_simple *buf, const bt_mesh_addr_t *addr)
{
    bt_mesh_proxy_adv_ctx_t ctx = {0};
    u8_t type;

    /* Check if connection reaches the maximum limitation */
    if (bt_mesh_gattc_get_free_conn_count() == 0) {
        BT_WARN("%s, max connections", __func__);
        return;
    }

    type = net_buf_simple_pull_u8(buf);

    switch (type) {
    case BLE_MESH_PROXY_ADV_NET_ID: {
        struct bt_mesh_subnet *sub = NULL;
        sub = bt_mesh_is_net_id_exist(buf->data);
        if (!sub) {
            return;
        }

        memcpy(ctx.net_id.net_id, buf->data, buf->len);
        ctx.net_id.net_idx = sub->net_idx;
        break;
    }
    case BLE_MESH_PROXY_ADV_NODE_ID:
        /* Gets node identity information.
         * hash = aes-ecb(identity key, 16 octets(padding + random + src)) mod 2^64,
         * If Proxy Client wants to get src, it may encrypts multiple times and compare
         * the hash value (8 octets) with the received one.
         */
        return;
    default:
        BT_DBG("%s, Unknwon Mesh Proxy adv type 0x%02x", __func__, type);
        return;
    }

    if (proxy_client_adv_recv_cb) {
        proxy_client_adv_recv_cb(addr, type, &ctx);
    }
}

int bt_mesh_proxy_client_connect(const u8_t addr[6], u8_t addr_type, u16_t net_idx)
{
    bt_mesh_addr_t remote_addr = {0};
    int result;

    if (!addr || addr_type > BLE_MESH_ADDR_RANDOM) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    memcpy(remote_addr.val, addr, BLE_MESH_ADDR_LEN);
    remote_addr.type = addr_type;

    result = bt_mesh_gattc_conn_create(&remote_addr, BLE_MESH_UUID_MESH_PROXY_VAL);
    if (result < 0) {
        return result;
    }

    /* Store corresponding net_idx which can be used for sending Proxy Configuration */
    servers[result].net_idx = net_idx;
    return 0;
}

int bt_mesh_proxy_client_disconnect(u8_t conn_handle)
{
    struct bt_mesh_conn *conn;

    if (conn_handle >= BLE_MESH_MAX_CONN) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    BT_DBG("conn_handle %d", conn_handle);

    conn = servers[conn_handle].conn;
    if (!conn) {
        BT_ERR("%s, Not connected, conn_handle %d", __func__, conn_handle);
        return -ENOTCONN;
    }

    bt_mesh_gattc_disconnect(conn);
    return 0;
}

bool bt_mesh_proxy_client_send(struct net_buf_simple *buf, u16_t dst)
{
    bool send = false;
    int err;
    u8_t i;

    BT_DBG("%u bytes to dst 0x%04x", buf->len, dst);

    for (i = 0U; i < ARRAY_SIZE(servers); i++) {
        struct bt_mesh_proxy_server *server = &servers[i];
        NET_BUF_SIMPLE_DEFINE(msg, 32);

        if (!server->conn || server->conn_type != PROXY) {
            continue;
        }

        /* Proxy PDU sending modifies the original buffer,
         * so we need to make a copy.
         */
        net_buf_simple_init(&msg, 1);
        net_buf_simple_add_mem(&msg, buf->data, buf->len);

        err = bt_mesh_proxy_prov_client_send(server->conn, BLE_MESH_PROXY_NET_PDU, &msg);
        if (err) {
            BT_ERR("%s, Failed to send proxy net message (err %d)", __func__, err);
        } else {
            send = true;
        }
    }

    return send;
}

static int beacon_send(struct bt_mesh_conn *conn, struct bt_mesh_subnet *sub)
{
    NET_BUF_SIMPLE_DEFINE(buf, 23);

    net_buf_simple_init(&buf, 1);
    bt_mesh_beacon_create(sub, &buf);

    return bt_mesh_proxy_prov_client_send(conn, BLE_MESH_PROXY_BEACON, &buf);
}

bool bt_mesh_proxy_client_beacon_send(struct bt_mesh_subnet *sub)
{
    bool send = false;
    int err;
    u8_t i;

    /* NULL means we send Secure Network Beacon on all subnets */
    if (!sub) {
        if (bt_mesh_is_provisioner_en()) {
#if CONFIG_BLE_MESH_PROVISIONER
            for (i = 0U; i < ARRAY_SIZE(bt_mesh.p_sub); i++) {
                if (bt_mesh.p_sub[i] && bt_mesh.p_sub[i]->net_idx != BLE_MESH_KEY_UNUSED) {
                    send = bt_mesh_proxy_client_beacon_send(bt_mesh.p_sub[i]);
                }
            }
#endif
        } else {
#if CONFIG_BLE_MESH_NODE
            for (i = 0U; i < ARRAY_SIZE(bt_mesh.sub); i++) {
                if (bt_mesh.sub[i].net_idx != BLE_MESH_KEY_UNUSED) {
                    send = bt_mesh_proxy_client_beacon_send(&bt_mesh.sub[i]);
                }
            }
#endif
        }

        return send;
    }

    for (i = 0U; i < ARRAY_SIZE(servers); i++) {
        if (servers[i].conn && servers[i].conn_type == PROXY) {
            err = beacon_send(servers[i].conn, sub);
            if (err) {
                BT_ERR("%s, Failed to send proxy beacon message (err %d)", __func__, err);
            } else {
                send = true;
            }
        }
    }

    return send;
}

static int send_proxy_cfg(struct bt_mesh_conn *conn, u16_t net_idx, struct bt_mesh_proxy_cfg_pdu *cfg)
{
    struct bt_mesh_msg_ctx ctx = {
        .net_idx  = net_idx,
        .app_idx  = BLE_MESH_KEY_UNUSED,        /* CTL shall be set to 1 */
        .addr     = BLE_MESH_ADDR_UNASSIGNED,   /* DST shall be set to the unassigned address */
        .send_ttl = 0U,                         /* TTL shall be set to 0 */
    };
    struct bt_mesh_net_tx tx = {
        .ctx = &ctx,
        .src = bt_mesh_primary_addr(),
    };
    struct net_buf_simple *buf = NULL;
    u16_t alloc_len;
    int err;

    if (bt_mesh_is_provisioner_en()) {
#if CONFIG_BLE_MESH_PROVISIONER
        tx.sub = provisioner_subnet_get(net_idx);
#endif
    } else {
#if CONFIG_BLE_MESH_NODE
        tx.sub = bt_mesh_subnet_get(net_idx);
#endif
    }
    if (!tx.sub) {
        BT_ERR("%s, Failed to find subnet", __func__);
        return -EIO;
    }

    switch (cfg->opcode) {
    case BLE_MESH_PROXY_CFG_FILTER_SET:
        if (cfg->set.filter_type > 0x01) {
            BT_ERR("%s, Invalid filter type 0x%02x", __func__, cfg->set.filter_type);
            return -EINVAL;
        }

        alloc_len = sizeof(cfg->opcode) + sizeof(cfg->set.filter_type);
        break;
    case BLE_MESH_PROXY_CFG_FILTER_ADD:
        if (cfg->add.addr == NULL || cfg->add.addr_num == 0) {
            BT_ERR("%s, Add address list is NULL", __func__);
            return -EINVAL;
        }

        alloc_len = sizeof(cfg->opcode) + (cfg->add.addr_num << 1);
        break;
    case BLE_MESH_PROXY_CFG_FILTER_REMOVE:
        if (cfg->remove.addr == NULL || cfg->remove.addr_num == 0) {
            BT_ERR("%s, Remove address list is NULL", __func__);
            return -EINVAL;
        }

        alloc_len = sizeof(cfg->opcode) + (cfg->remove.addr_num << 1);
        break;
    default:
        BT_ERR("%s, Unknown Proxy Configuration opcode 0x%02x", __func__, cfg->opcode);
        return -EINVAL;
    }

    /**
     * For Proxy Configuration PDU:
     * 1 octet Proxy PDU type + 9 octets network pdu header + Tranport PDU + 8 octets NetMIC
     */
    buf = bt_mesh_alloc_buf(1 + BLE_MESH_NET_HDR_LEN + alloc_len + 8);
    if (!buf) {
        return -ENOMEM;
    }

    net_buf_simple_reset(buf);
    net_buf_simple_reserve(buf, 10);

    net_buf_simple_add_u8(buf, cfg->opcode);
    switch (cfg->opcode) {
    case BLE_MESH_PROXY_CFG_FILTER_SET:
        net_buf_simple_add_u8(buf, cfg->set.filter_type);
        break;
    case BLE_MESH_PROXY_CFG_FILTER_ADD:
        for (u16_t i = 0U; i < cfg->add.addr_num; i++) {
            net_buf_simple_add_le16(buf, cfg->add.addr[i]);
        }
        break;
    case BLE_MESH_PROXY_CFG_FILTER_REMOVE:
        for (u16_t i = 0U; i < cfg->remove.addr_num; i++) {
            net_buf_simple_add_le16(buf, cfg->remove.addr[i]);
        }
        break;
    }

    BT_DBG("%s, len %u bytes: %s", __func__, buf->len, bt_hex(buf->data, buf->len));

    err = bt_mesh_net_encode(&tx, buf, true);
    if (err) {
        BT_ERR("%s, Encoding Proxy message failed (err %d)", __func__, err);
        bt_mesh_free_buf(buf);
        return err;
    }

    err = bt_mesh_proxy_prov_client_send(conn, BLE_MESH_PROXY_CONFIG, buf);
    if (err) {
        BT_ERR("%s, Failed to send proxy cfg message (err %d)", __func__, err);
    }

    bt_mesh_free_buf(buf);
    return err;
}

int bt_mesh_proxy_client_send_cfg(u8_t conn_handle, u16_t net_idx, struct bt_mesh_proxy_cfg_pdu *pdu)
{
    struct bt_mesh_conn *conn;

    if (conn_handle >= BLE_MESH_MAX_CONN || !pdu || pdu->opcode > BLE_MESH_PROXY_CFG_FILTER_REMOVE) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    BT_DBG("conn_handle %d, net_idx 0x%04x", conn_handle, net_idx);

    conn = servers[conn_handle].conn;
    if (!conn) {
        BT_ERR("%s, Not connected, conn_handle %d", __func__, conn_handle);
        return -ENOTCONN;
    }

    /**
     * Check if net_idx used to encrypt Proxy Configuration are the same
     * with the one added when creating proxy connection.
     */
    if (servers[conn_handle].net_idx != net_idx) {
        BT_ERR("%s, NetKey Index 0x%04x mismatch, expect 0x%04x",
               __func__, net_idx, servers[conn_handle].net_idx);
        return -EIO;
    }

    return send_proxy_cfg(conn, net_idx, pdu);
}
#endif /* CONFIG_BLE_MESH_GATT_PROXY_CLIENT */

int bt_mesh_proxy_prov_client_init(void)
{
    u8_t i;

    /* Initialize the server receive buffers */
    for (i = 0U; i < ARRAY_SIZE(servers); i++) {
        struct bt_mesh_proxy_server *server = &servers[i];

        k_delayed_work_init(&server->sar_timer, proxy_sar_timeout);
        server->buf.size = SERVER_BUF_SIZE;
        server->buf.__buf = server_buf_data + (i * SERVER_BUF_SIZE);
#if CONFIG_BLE_MESH_GATT_PROXY_CLIENT
        server->net_idx = BLE_MESH_KEY_UNUSED;
#endif
    }

    bt_mesh_gattc_conn_cb_register(&conn_callbacks);

#if CONFIG_BLE_MESH_USE_DUPLICATE_SCAN && CONFIG_BLE_MESH_GATT_PROXY_CLIENT
    bt_mesh_update_exceptional_list(BLE_MESH_EXCEP_LIST_ADD,
                                    BLE_MESH_EXCEP_INFO_MESH_PROXY_ADV, NULL);
#endif

    return 0;
}

#endif /* (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) || CONFIG_BLE_MESH_GATT_PROXY_CLIENT */
