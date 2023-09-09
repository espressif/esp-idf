/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include "mesh.h"
#include "rpl.h"
#include "access.h"
#include "beacon.h"
#include "transport.h"
#include "mesh/common.h"
#include "foundation.h"
#include "proxy_common.h"
#include "proxy_client.h"
#include "prov_pvnr.h"
#include "prov_node.h"
#include "pvnr_mgmt.h"
#include "mesh/adapter.h"

#include "mesh_v1.1/utils.h"

#if (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) || \
     CONFIG_BLE_MESH_GATT_PROXY_CLIENT

static struct bt_mesh_proxy_server {
    struct bt_mesh_conn *conn;

    enum __attribute__((packed)) {
        CLI_NONE,
        CLI_PROV,
        CLI_PROXY,
    } conn_type;

#if CONFIG_BLE_MESH_GATT_PROXY_CLIENT
    uint16_t net_idx;
#endif
    uint8_t msg_type;

    struct k_delayed_work sar_timer;

    struct net_buf_simple buf;
} servers[BLE_MESH_MAX_CONN];

#if CONFIG_BLE_MESH_RPR_SRV && CONFIG_BLE_MESH_PB_GATT
static struct {
    struct bt_mesh_prov_link *link;
    bt_mesh_addr_t addr;
} waiting_conn_link[BLE_MESH_MAX_CONN];
#endif

static uint8_t server_buf_data[BLE_MESH_PROXY_BUF_SIZE * BLE_MESH_MAX_CONN];

static struct bt_mesh_proxy_server *find_server(struct bt_mesh_conn *conn)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(servers); i++) {
        if (servers[i].conn == conn) {
            return &servers[i];
        }
    }

    return NULL;
}

static void proxy_sar_timeout(struct k_work *work)
{
    struct bt_mesh_proxy_server *server = NULL;

    BT_WARN("%s", __func__);

    server = CONTAINER_OF(work, struct bt_mesh_proxy_server, sar_timer.work);
    if (!server || !server->conn) {
        BT_ERR("Invalid proxy server parameter");
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

#if (CONFIG_BLE_MESH_RPR_SRV && CONFIG_BLE_MESH_PB_GATT)
int bt_mesh_rpr_srv_set_waiting_prov_link(struct bt_mesh_prov_link *link,
                                          bt_mesh_addr_t *addr)
{
    for (size_t i = 0; i < ARRAY_SIZE(waiting_conn_link);i++) {
        if (waiting_conn_link[i].link == NULL) {
            waiting_conn_link[i].link = link;
            memcpy(&waiting_conn_link[i].addr, addr, sizeof(bt_mesh_addr_t));
            return 0;
        }
    }

    return -ENOBUFS;
}
#endif /* CONFIG_BLE_MESH_RPR_SRV && CONFIG_BLE_MESH_PB_GATT */

static void filter_status(struct bt_mesh_proxy_server *server,
                          struct bt_mesh_net_rx *rx,
                          struct net_buf_simple *buf)
{
    uint8_t filter_type = 0U;
    uint16_t list_size = 0U;

    if (buf->len != 3) {
        BT_ERR("Invalid Proxy Filter Status length %d", buf->len);
        return;
    }

    filter_type = net_buf_simple_pull_u8(buf);
    if (filter_type > 0x01) {
        BT_ERR("Invalid proxy filter type 0x%02x", filter_type);
        return;
    }

    list_size = net_buf_simple_pull_be16(buf);

    BT_INFO("filter_type 0x%02x, list_size %d", filter_type, list_size);

    if (proxy_client_filter_status_recv_cb) {
        proxy_client_filter_status_recv_cb(server - servers, rx->ctx.addr, server->net_idx, filter_type, list_size);
    }
}

#if CONFIG_BLE_MESH_DF_SRV
static void recv_directed_proxy_caps_status(struct bt_mesh_proxy_server *server,
                                            struct bt_mesh_net_rx *rx,
                                            struct net_buf_simple *buf)
{
    uint8_t directed_proxy = net_buf_simple_pull_u8(buf);
    uint8_t use_directed = net_buf_simple_pull_u8(buf);

    BT_INFO("Directed Proxy 0x%02x, Use Directed 0x%02x", directed_proxy, use_directed);

    ARG_UNUSED(directed_proxy);
    ARG_UNUSED(use_directed);
}
#endif /* CONFIG_BLE_MESH_DF_SRV */

static void proxy_cfg(struct bt_mesh_proxy_server *server)
{
    NET_BUF_SIMPLE_DEFINE(buf, 29);
    struct bt_mesh_net_rx rx = {0};
    uint8_t opcode = 0U;
    int err = 0;

    if (server->buf.len > 29) {
        BT_ERR("Too large proxy cfg pdu (len %d)", server->buf.len);
        return;
    }

    err = bt_mesh_net_decode(&server->buf, BLE_MESH_NET_IF_PROXY_CFG,
                             &rx, &buf);
    if (err) {
        BT_ERR("Failed to decode Proxy Configuration (err %d)", err);
        return;
    }

    if (!BLE_MESH_ADDR_IS_UNICAST(rx.ctx.addr)) {
        BT_ERR("Proxy Configuration from non-unicast addr 0x%04x", rx.ctx.addr);
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

    if (buf.len < 3) {
        BT_WARN("Too short proxy configuration PDU");
        return;
    }

    opcode = net_buf_simple_pull_u8(&buf);

    switch (opcode) {
    case BLE_MESH_PROXY_CFG_FILTER_STATUS:
        filter_status(server, &rx, &buf);
        break;

#if CONFIG_BLE_MESH_DF_SRV
    case BLE_MESH_PROXY_CFG_DIRECTED_PROXY_CAPS_STATUS:
        if (buf.len != 2) {
            BT_WARN("Invalid Directed Proxy Caps Status (len %d)", buf.len);
            break;
        }

        recv_directed_proxy_caps_status(server, &rx, &buf);
        break;
#endif /* CONFIG_BLE_MESH_DF_SRV */

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
        bt_mesh_beacon_recv(&server->buf, 0);
        break;
    case BLE_MESH_PROXY_CONFIG:
        BT_DBG("Mesh Configuration PDU");
        proxy_cfg(server);
        break;
#endif /* CONFIG_BLE_MESH_GATT_PROXY_CLIENT */
#if CONFIG_BLE_MESH_PB_GATT && (CONFIG_BLE_MESH_PROVISIONER || CONFIG_BLE_MESH_RPR_SRV)
    case BLE_MESH_PROXY_PROV:
        BT_DBG("Mesh Provisioning PDU");
#if CONFIG_BLE_MESH_RPR_SRV
        if (server->conn == bt_mesh_prov_node_get_link()->conn) {
            bt_mesh_pb_gatt_recv(server->conn, &server->buf);
        } else
#endif
        {
            bt_mesh_provisioner_pb_gatt_recv(server->conn, &server->buf);
        }
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
                          uint16_t len, uint16_t offset, uint8_t flags)
{
    struct bt_mesh_proxy_server *server = find_server(conn);
    const uint8_t *data = buf;
    uint16_t srvc_uuid = 0U;

    if (!server) {
        BT_ERR("No Proxy Server object found");
        return -ENOTCONN;
    }

    if (len < 1) {
        BT_WARN("Too small Proxy PDU");
        return -EINVAL;
    }

    srvc_uuid = bt_mesh_gattc_get_service_uuid(conn);
    if (!srvc_uuid) {
        BT_ERR("No service uuid found");
        return -ENOTCONN;
    }

    if (ATTR_IS_PROV(srvc_uuid) != (BLE_MESH_PROXY_PDU_TYPE(data) == BLE_MESH_PROXY_PROV)) {
        BT_WARN("Proxy PDU type doesn't match GATT service uuid");
        return -EINVAL;
    }

    if (len - 1 > net_buf_simple_tailroom(&server->buf)) {
        BT_WARN("Too big proxy PDU");
        return -EINVAL;
    }

    switch (BLE_MESH_PROXY_PDU_SAR(data)) {
    case BLE_MESH_PROXY_SAR_COMP:
        if (server->buf.len) {
            BT_WARN("Complete PDU while a pending incomplete one");
            return -EINVAL;
        }

        server->msg_type = BLE_MESH_PROXY_PDU_TYPE(data);
        net_buf_simple_add_mem(&server->buf, data + 1, len - 1);
        proxy_complete_pdu(server);
        break;

    case BLE_MESH_PROXY_SAR_FIRST:
        if (server->buf.len) {
            BT_WARN("First PDU while a pending incomplete one");
            return -EINVAL;
        }

        k_delayed_work_submit(&server->sar_timer, BLE_MESH_PROXY_SAR_TIMEOUT);
        server->msg_type = BLE_MESH_PROXY_PDU_TYPE(data);
        net_buf_simple_add_mem(&server->buf, data + 1, len - 1);
        break;

    case BLE_MESH_PROXY_SAR_CONT:
        if (!server->buf.len) {
            BT_WARN("Continuation with no prior data");
            return -EINVAL;
        }

        if (server->msg_type != BLE_MESH_PROXY_PDU_TYPE(data)) {
            BT_WARN("Unexpected message type in continuation");
            return -EINVAL;
        }

        k_delayed_work_submit(&server->sar_timer, BLE_MESH_PROXY_SAR_TIMEOUT);
        net_buf_simple_add_mem(&server->buf, data + 1, len - 1);
        break;

    case BLE_MESH_PROXY_SAR_LAST:
        if (!server->buf.len) {
            BT_WARN("Last SAR PDU with no prior data");
            return -EINVAL;
        }

        if (server->msg_type != BLE_MESH_PROXY_PDU_TYPE(data)) {
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

static int proxy_send(struct bt_mesh_conn *conn, const void *data, uint16_t len)
{
    BT_DBG("%u bytes: %s", len, bt_hex(data, len));

    return bt_mesh_gattc_write_no_rsp(conn, NULL, data, len);
}

int bt_mesh_proxy_client_segment_send(struct bt_mesh_conn *conn, uint8_t type,
                                      struct net_buf_simple *msg)
{
    uint16_t mtu = 0U;
    int err = 0;

    if (conn == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    BT_DBG("conn %p type 0x%02x len %u: %s", conn, type, msg->len,
           bt_hex(msg->data, msg->len));

    mtu = bt_mesh_gattc_get_mtu_info(conn);
    if (!mtu) {
        BT_ERR("Conn %p used to get mtu not exists", conn);
        return -ENOTCONN;
    }

    /* ATT_MTU - OpCode (1 byte) - Handle (2 bytes) */
    mtu -= 3;
    if (mtu > msg->len) {
        net_buf_simple_push_u8(msg, BLE_MESH_PROXY_PDU_HDR(BLE_MESH_PROXY_SAR_COMP, type));
        return proxy_send(conn, msg->data, msg->len);
    }

    net_buf_simple_push_u8(msg, BLE_MESH_PROXY_PDU_HDR(BLE_MESH_PROXY_SAR_FIRST, type));
    err = proxy_send(conn, msg->data, mtu);
    net_buf_simple_pull(msg, mtu);

    while (msg->len) {
        if (msg->len + 1 < mtu) {
            net_buf_simple_push_u8(msg, BLE_MESH_PROXY_PDU_HDR(BLE_MESH_PROXY_SAR_LAST, type));
            err = proxy_send(conn, msg->data, msg->len);
            break;
        }

        net_buf_simple_push_u8(msg, BLE_MESH_PROXY_PDU_HDR(BLE_MESH_PROXY_SAR_CONT, type));
        err = proxy_send(conn, msg->data, mtu);
        net_buf_simple_pull(msg, mtu);
    }

    return err;
}

int bt_mesh_proxy_client_send(struct bt_mesh_conn *conn, uint8_t type,
                              struct net_buf_simple *msg)
{
    struct bt_mesh_proxy_server *server = find_server(conn);

    if (!server) {
        BT_ERR("No Proxy Server object found");
        return -ENOTCONN;
    }

    if ((server->conn_type == CLI_PROV) != (type == BLE_MESH_PROXY_PROV)) {
        BT_ERR("Invalid PDU type for Proxy Server");
        return -EINVAL;
    }

    return bt_mesh_proxy_client_segment_send(conn, type, msg);
}

static void proxy_connected(bt_mesh_addr_t *addr, struct bt_mesh_conn *conn, int id)
{
    struct bt_mesh_proxy_server *server = NULL;

    if (!servers[id].conn) {
        server = &servers[id];
    }

    if (!server) {
        BT_ERR("No free Proxy Server objects");
        /* Disconnect current connection, clear part of prov_link
         * information, like uuid, dev_addr, linking flag, etc.
         */
        bt_mesh_gattc_disconnect(conn);
        return;
    }

    server->conn = bt_mesh_conn_ref(conn);
    server->conn_type = CLI_NONE;
    net_buf_simple_reset(&server->buf);

#if CONFIG_BLE_MESH_RPR_SRV && CONFIG_BLE_MESH_PB_GATT
    for (size_t i = 0; i < ARRAY_SIZE(waiting_conn_link); i++) {
        if (!memcmp(addr, &waiting_conn_link[i].addr, sizeof(bt_mesh_addr_t))) {
            waiting_conn_link[i].link->conn = conn;
            break;
        }
    }
#endif

    bt_mesh_gattc_exchange_mtu(id);
}

static void proxy_disconnected(bt_mesh_addr_t *addr, struct bt_mesh_conn *conn, uint8_t reason)
{
    struct bt_mesh_proxy_server *server = find_server(conn);

    BT_DBG("conn %p, handle is %d, reason 0x%02x", conn, conn->handle, reason);

    if (!server) {
        BT_ERR("No Proxy Server object found");
        return;
    }

#if CONFIG_BLE_MESH_PB_GATT && \
    (CONFIG_BLE_MESH_PROVISIONER || CONFIG_BLE_MESH_RPR_SRV)
    if (server->conn_type == CLI_PROV) {
#if CONFIG_BLE_MESH_RPR_SRV
        if (bt_mesh_prov_node_get_link()->conn == conn) {
            for (size_t i = 0; i < ARRAY_SIZE(waiting_conn_link); i++) {
                if (waiting_conn_link[i].link->conn == conn) {
                    memset(&waiting_conn_link[i].addr, 0, sizeof(bt_mesh_addr_t));
                    break;
                }
            }

            bt_mesh_pb_gatt_close(conn, reason);
        } else
#endif /* CONFIG_BLE_MESH_RPR_SRV */
        {
            bt_mesh_provisioner_pb_gatt_close(conn, reason);
        }
    }
#endif /* CONFIG_BLE_MESH_PB_GATT && (CONFIG_BLE_MESH_PROVISIONER || CONFIG_BLE_MESH_RPR_SRV) */

#if CONFIG_BLE_MESH_GATT_PROXY_CLIENT
    if (server->conn_type == CLI_PROXY) {
        if (proxy_client_disconnect_cb) {
            proxy_client_disconnect_cb(addr, server - servers, server->net_idx, reason);
        }
    }
#endif

    k_delayed_work_cancel(&server->sar_timer);
    server->conn = NULL;
    server->conn_type = CLI_NONE;
#if CONFIG_BLE_MESH_GATT_PROXY_CLIENT
    server->net_idx = BLE_MESH_KEY_UNUSED;
#endif
}

#if CONFIG_BLE_MESH_PB_GATT && \
    (CONFIG_BLE_MESH_PROVISIONER || CONFIG_BLE_MESH_RPR_SRV)
static ssize_t prov_write_ccc(bt_mesh_addr_t *addr, struct bt_mesh_conn *conn)
{
    struct bt_mesh_proxy_server *server = find_server(conn);

    if (!server) {
        BT_ERR("No Proxy Server object found");
        return -ENOTCONN;
    }

    if (server->conn_type == CLI_NONE) {
        server->conn_type = CLI_PROV;

#if CONFIG_BLE_MESH_RPR_SRV
        if (bt_mesh_prov_node_get_link()->conn == conn) {
            int err = bt_mesh_pb_gatt_open(conn);
            if (err) {
                BT_ERR("proxy write ccc error %d", err);
                return err;
            }

            return bt_mesh_rpr_srv_recv_link_ack(addr->val, false);
        }
#endif

        return bt_mesh_provisioner_pb_gatt_open(conn, addr->val);
    }

    return -ENOMEM;
}

static ssize_t prov_recv_ntf(struct bt_mesh_conn *conn, uint8_t *data, uint16_t len)
{
    struct bt_mesh_proxy_server *server = find_server(conn);

    if (!server) {
        BT_ERR("No Proxy Server object found");
        return -ENOTCONN;
    }

    if (server->conn_type == CLI_PROV) {
        return proxy_recv(conn, NULL, data, len, 0, 0);
    }

    return -EINVAL;
}

int bt_mesh_proxy_client_prov_enable(void)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(servers); i++) {
        if (servers[i].conn) {
            servers[i].conn_type = CLI_PROV;
        }
    }

    return 0;
}

int bt_mesh_proxy_client_prov_disable(void)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(servers); i++) {
        struct bt_mesh_proxy_server *server = &servers[i];

        if (server->conn && server->conn_type == CLI_PROV) {
            bt_mesh_gattc_disconnect(server->conn);
            server->conn_type = CLI_NONE;
        }
    }

    return 0;
}
#endif /* CONFIG_BLE_MESH_PB_GATT */

#if CONFIG_BLE_MESH_GATT_PROXY_CLIENT
static ssize_t proxy_write_ccc(bt_mesh_addr_t *addr, struct bt_mesh_conn *conn)
{
    struct bt_mesh_proxy_server *server = find_server(conn);

    if (!server) {
        BT_ERR("No Proxy Server object found");
        return -ENOTCONN;
    }

    if (server->conn_type == CLI_NONE) {
        server->conn_type = CLI_PROXY;

        if (proxy_client_connect_cb) {
            proxy_client_connect_cb(addr, server - servers, server->net_idx);
        }
        return 0;
    }

    return -EINVAL;
}

static ssize_t proxy_recv_ntf(struct bt_mesh_conn *conn, uint8_t *data, uint16_t len)
{
    struct bt_mesh_proxy_server *server = find_server(conn);

    if (!server) {
        BT_ERR("No Proxy Server object found");
        return -ENOTCONN;
    }

    if (server->conn_type == CLI_PROXY) {
        return proxy_recv(conn, NULL, data, len, 0, 0);
    }

    return -EINVAL;
}

/**
 * Currently proxy client doesn't need bt_mesh_proxy_client_gatt_enable()
 * and bt_mesh_proxy_client_gatt_disable() functions, and once they are
 * used, proxy client can be enabled to parse node_id_adv and net_id_adv
 * in order to support proxy client role.
 * And if gatt proxy is disabled, proxy client can stop handling these
 * two kinds of connectable advertising packets.
 */
int bt_mesh_proxy_client_gatt_enable(void)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(servers); i++) {
        if (servers[i].conn) {
            servers[i].conn_type = CLI_PROXY;
        }
    }

    /* TODO:
     * Once at least one device has been provisioned, proxy client can be
     * set to allow receiving and parsing node_id & net_id adv packets,
     * and we may use a global flag to indicate this.
     */

    return 0;
}

int bt_mesh_proxy_client_gatt_disable(void)
{
    int i;

    /**
     * TODO:
     * Once this function is invoked, proxy client shall stop handling
     * node_id & net_id adv packets, and if proxy connection exists,
     * it should be disconnected.
     */

    for (i = 0; i < ARRAY_SIZE(servers); i++) {
        struct bt_mesh_proxy_server *server = &servers[i];

        if (server->conn && server->conn_type == CLI_PROXY) {
            bt_mesh_gattc_disconnect(server->conn);
            server->conn_type = CLI_NONE;
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
#if CONFIG_BLE_MESH_GATT_PROXY_CLIENT
    .proxy_write_descr = proxy_write_ccc,
    .proxy_notify = proxy_recv_ntf,
#endif /* CONFIG_BLE_MESH_GATT_PROXY_CLIENT */
};

#if CONFIG_BLE_MESH_GATT_PROXY_CLIENT
static struct bt_mesh_subnet *bt_mesh_is_net_id_exist(const uint8_t net_id[8])
{
    struct bt_mesh_subnet *sub = NULL;
    size_t size = 0U, i = 0U;

    size = bt_mesh_rx_netkey_size();

    for (i = 0U; i < size; i++) {
        sub = bt_mesh_rx_netkey_get(i);
        if (sub && !memcmp(sub->keys[sub->kr_flag].net_id, net_id, 8)) {
            return sub;
        }
    }

    return NULL;
}

void bt_mesh_proxy_client_gatt_adv_recv(struct net_buf_simple *buf,
                                        const bt_mesh_addr_t *addr, int8_t rssi)
{
    bt_mesh_proxy_adv_ctx_t ctx = {0};
    uint8_t type = 0U;

    /* Check if connection reaches the maximum limitation */
    if (bt_mesh_gattc_get_free_conn_count() == 0) {
        BT_INFO("BLE connections for mesh reach max limit");
        return;
    }

    type = net_buf_simple_pull_u8(buf);

    switch (type) {
    case BLE_MESH_PROXY_ADV_NET_ID: {
        if (buf->len != sizeof(ctx.net_id.net_id)) {
            BT_WARN("Malformed Network ID");
            return;
        }

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
    case BLE_MESH_PROXY_ADV_PRIVATE_NET_ID:
        BT_DBG("Mesh Proxy Private Network ID adv 0x%02x", type);
        return;
    case BLE_MESH_PROXY_ADV_PRIVATE_NODE_ID:
        BT_DBG("Mesh Proxy Private Node Identity adv 0x%02x", type);
        return;
    default:
        BT_DBG("Unknown Mesh Proxy adv type 0x%02x", type);
        return;
    }

    if (proxy_client_adv_recv_cb) {
        proxy_client_adv_recv_cb(addr, type, &ctx, rssi);
    }
}

int bt_mesh_proxy_client_connect(const uint8_t addr[6], uint8_t addr_type, uint16_t net_idx)
{
    bt_mesh_addr_t remote_addr = {0};
    int result = 0;

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

int bt_mesh_proxy_client_disconnect(uint8_t conn_handle)
{
    struct bt_mesh_conn *conn = NULL;

    if (conn_handle >= BLE_MESH_MAX_CONN) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    BT_DBG("conn_handle %d", conn_handle);

    conn = servers[conn_handle].conn;
    if (!conn) {
        BT_ERR("Not connected, conn handle %d", conn_handle);
        return -ENOTCONN;
    }

    bt_mesh_gattc_disconnect(conn);
    return 0;
}

bool bt_mesh_proxy_client_relay(struct net_buf_simple *buf, uint16_t dst)
{
    bool send = false;
    int err = 0;
    int i;

    for (i = 0; i < ARRAY_SIZE(servers); i++) {
        struct bt_mesh_proxy_server *server = &servers[i];
        NET_BUF_SIMPLE_DEFINE(msg, 32);

        if (!server->conn || server->conn_type != CLI_PROXY) {
            continue;
        }

        /* Proxy PDU sending modifies the original buffer,
         * so we need to make a copy.
         */
        net_buf_simple_reserve(&msg, 1);
        net_buf_simple_add_mem(&msg, buf->data, buf->len);

        err = bt_mesh_proxy_client_send(server->conn, BLE_MESH_PROXY_NET_PDU, &msg);
        if (err) {
            BT_ERR("Failed to send proxy network message (err %d)", err);
        } else {
            BT_INFO("%u bytes to dst 0x%04x", buf->len, dst);
            send = true;
        }
    }

    return send;
}

static int beacon_send(struct bt_mesh_conn *conn, struct bt_mesh_subnet *sub, bool private)
{
    NET_BUF_SIMPLE_DEFINE(buf, 28);

    net_buf_simple_reserve(&buf, 1);
#if CONFIG_BLE_MESH_PRB_SRV
    if (private) {
        bt_mesh_private_beacon_create(sub, &buf);
    } else
#endif
    {
        bt_mesh_secure_beacon_create(sub, &buf);
    }

    return bt_mesh_proxy_client_send(conn, BLE_MESH_PROXY_BEACON, &buf);
}

bool bt_mesh_proxy_client_beacon_send(struct bt_mesh_subnet *sub, bool private)
{
    bool send = false;
    int err = 0;
    int i;

    /* NULL means we send Secure Network Beacon or Mesh Private Beacon on all subnets */
    if (!sub) {
#if CONFIG_BLE_MESH_NODE
        if (bt_mesh_is_provisioned()) {
            for (i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
                if (bt_mesh.sub[i].net_idx != BLE_MESH_KEY_UNUSED) {
                    send = bt_mesh_proxy_client_beacon_send(&bt_mesh.sub[i], private);
                }
            }
            return send;
        }
#endif /* CONFIG_BLE_MESH_NODE */
#if CONFIG_BLE_MESH_PROVISIONER
        if (bt_mesh_is_provisioner_en()) {
            for (i = 0; i < ARRAY_SIZE(bt_mesh.p_sub); i++) {
                if (bt_mesh.p_sub[i] && bt_mesh.p_sub[i]->net_idx != BLE_MESH_KEY_UNUSED) {
                    send = bt_mesh_proxy_client_beacon_send(bt_mesh.p_sub[i], private);
                }
            }
            return send;
        }
#endif /* CONFIG_BLE_MESH_PROVISIONER */
        return send;
    }

    for (i = 0; i < ARRAY_SIZE(servers); i++) {
        if (servers[i].conn && servers[i].conn_type == CLI_PROXY) {
            err = beacon_send(servers[i].conn, sub, private);
            if (err) {
                BT_ERR("Failed to send proxy beacon message (err %d)", err);
            } else {
                send = true;
            }
        }
    }

    return send;
}

static int send_proxy_cfg(struct bt_mesh_conn *conn, uint16_t net_idx, struct bt_mesh_proxy_cfg_pdu *cfg)
{
    struct bt_mesh_msg_ctx ctx = {
        .net_idx   = net_idx,
        .app_idx   = BLE_MESH_KEY_UNUSED,        /* CTL shall be set to 1 */
        .addr      = BLE_MESH_ADDR_UNASSIGNED,   /* DST shall be set to the unassigned address */
        .send_ttl  = 0U,                         /* TTL shall be set to 0 */
        .send_cred = BLE_MESH_FLOODING_CRED,
    };
    struct bt_mesh_net_tx tx = {
        .ctx = &ctx,
        .src = bt_mesh_primary_addr(),
    };
    struct net_buf_simple *buf = NULL;
    uint16_t alloc_len = 0U;
    int err = 0;

    tx.sub = bt_mesh_subnet_get(net_idx);
    if (!tx.sub) {
        BT_ERR("NetKey 0x%04x not found", net_idx);
        return -EIO;
    }

    switch (cfg->opcode) {
    case BLE_MESH_PROXY_CFG_FILTER_SET:
        if (cfg->set.filter_type > 0x01) {
            BT_ERR("Invalid proxy filter type 0x%02x", cfg->set.filter_type);
            return -EINVAL;
        }

        alloc_len = sizeof(cfg->opcode) + sizeof(cfg->set.filter_type);
        break;

    case BLE_MESH_PROXY_CFG_FILTER_ADD:
        if (cfg->add.addr == NULL || cfg->add.addr_num == 0) {
            BT_ERR("Empty proxy addr list to add");
            return -EINVAL;
        }

        alloc_len = sizeof(cfg->opcode) + (cfg->add.addr_num << 1);
        break;

    case BLE_MESH_PROXY_CFG_FILTER_REMOVE:
        if (cfg->remove.addr == NULL || cfg->remove.addr_num == 0) {
            BT_ERR("Empty proxy addr list to remove");
            return -EINVAL;
        }

        alloc_len = sizeof(cfg->opcode) + (cfg->remove.addr_num << 1);
        break;

#if CONFIG_BLE_MESH_DF_SRV
    case BLE_MESH_PROXY_CFG_DIRECTED_PROXY_CONTROL:
        if (cfg->direct_proxy_ctrl.use_directed > BLE_MESH_PROXY_USE_DIRECTED_ENABLED) {
            BT_ERR("Invalid Use Directed 0x%02x", cfg->direct_proxy_ctrl.use_directed);
            return -EINVAL;
        }

        if (cfg->direct_proxy_ctrl.use_directed == BLE_MESH_PROXY_USE_DIRECTED_ENABLED &&
            !bt_mesh_uar_valid(&cfg->direct_proxy_ctrl.proxy_client_uar)) {
            BT_ERR("Invalid Proxy Client Unicast Addr Range (0x%04x, %d)",
                    cfg->direct_proxy_ctrl.proxy_client_uar.range_start,
                    cfg->direct_proxy_ctrl.proxy_client_uar.range_length);
            return -EINVAL;
        }

        alloc_len = sizeof(cfg->opcode) + 4;
        break;
#endif /* CONFIG_BLE_MESH_DF_SRV */

    default:
        BT_ERR("Unknown Proxy Configuration opcode 0x%02x", cfg->opcode);
        return -EINVAL;
    }

    /* For Proxy Configuration PDU:
     * 1 octet Proxy PDU type + 9 octets network pdu header + Transport PDU + 8 octets NetMIC
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
        for (uint16_t i = 0U; i < cfg->add.addr_num; i++) {
            net_buf_simple_add_le16(buf, cfg->add.addr[i]);
        }
        break;

    case BLE_MESH_PROXY_CFG_FILTER_REMOVE:
        for (uint16_t i = 0U; i < cfg->remove.addr_num; i++) {
            net_buf_simple_add_le16(buf, cfg->remove.addr[i]);
        }
        break;

#if CONFIG_BLE_MESH_DF_SRV
    case BLE_MESH_PROXY_CFG_DIRECTED_PROXY_CONTROL:
        net_buf_simple_add_u8(buf, cfg->direct_proxy_ctrl.use_directed);
        if (cfg->direct_proxy_ctrl.use_directed == BLE_MESH_PROXY_USE_DIRECTED_ENABLED) {
            bt_mesh_add_uar_be(buf, &cfg->direct_proxy_ctrl.proxy_client_uar);
        }
        break;
#endif /* CONFIG_BLE_MESH_DF_SRV */
    }

    BT_DBG("len %u bytes: %s", buf->len, bt_hex(buf->data, buf->len));

    err = bt_mesh_net_encode(&tx, buf, true);
    if (err) {
        BT_ERR("Encoding proxy cfg message failed (err %d)", err);
        goto end;
    }

    err = bt_mesh_proxy_client_send(conn, BLE_MESH_PROXY_CONFIG, buf);
    if (err) {
        BT_ERR("Failed to send proxy cfg message 0x%02x (err %d)", cfg->opcode, err);
    }

end:
    bt_mesh_free_buf(buf);
    return err;
}

int bt_mesh_proxy_client_cfg_send(uint8_t conn_handle, uint16_t net_idx,
                                  struct bt_mesh_proxy_cfg_pdu *pdu)
{
    struct bt_mesh_conn *conn = NULL;

    if (conn_handle >= BLE_MESH_MAX_CONN || !pdu || pdu->opcode > BLE_MESH_PROXY_CFG_DIRECTED_PROXY_CONTROL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    BT_DBG("conn_handle %d, net_idx 0x%04x", conn_handle, net_idx);

    conn = servers[conn_handle].conn;
    if (!conn) {
        BT_ERR("Not connected, conn handle %d", conn_handle);
        return -ENOTCONN;
    }

    /* Check if net_idx used to encrypt Proxy Configuration are the same
     * with the one added when creating proxy connection.
     */
    if (servers[conn_handle].net_idx != net_idx) {
        BT_ERR("NetKeyIndex 0x%04x mismatch, expect 0x%04x",
                net_idx, servers[conn_handle].net_idx);
        return -EIO;
    }

    return send_proxy_cfg(conn, net_idx, pdu);
}
#endif /* CONFIG_BLE_MESH_GATT_PROXY_CLIENT */

int bt_mesh_proxy_client_init(void)
{
    int i;

    /* Initialize the server receive buffers */
    for (i = 0; i < ARRAY_SIZE(servers); i++) {
        struct bt_mesh_proxy_server *server = &servers[i];

        k_delayed_work_init(&server->sar_timer, proxy_sar_timeout);
        server->buf.size = BLE_MESH_PROXY_BUF_SIZE;
        server->buf.__buf = server_buf_data + (i * BLE_MESH_PROXY_BUF_SIZE);
#if CONFIG_BLE_MESH_GATT_PROXY_CLIENT
        server->net_idx = BLE_MESH_KEY_UNUSED;
#endif
    }

    bt_mesh_gattc_conn_cb_register(&conn_callbacks);

#if CONFIG_BLE_MESH_USE_DUPLICATE_SCAN && CONFIG_BLE_MESH_GATT_PROXY_CLIENT
    bt_mesh_update_exceptional_list(BLE_MESH_EXCEP_LIST_SUB_CODE_ADD,
                                    BLE_MESH_EXCEP_LIST_TYPE_MESH_PROXY_ADV,
                                    NULL);
#endif

    return 0;
}

#if CONFIG_BLE_MESH_DEINIT
int bt_mesh_proxy_client_deinit(void)
{
    int i;

    /* Initialize the server receive buffers */
    for (i = 0; i < ARRAY_SIZE(servers); i++) {
        struct bt_mesh_proxy_server *server = &servers[i];
        k_delayed_work_free(&server->sar_timer);
        memset(server, 0, sizeof(struct bt_mesh_proxy_server));
    }

    memset(server_buf_data, 0, sizeof(server_buf_data));

    bt_mesh_gattc_conn_cb_deregister();

    return 0;
}
#endif /* CONFIG_BLE_MESH_DEINIT */

#endif /* (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) || \
           CONFIG_BLE_MESH_GATT_PROXY_CLIENT */
