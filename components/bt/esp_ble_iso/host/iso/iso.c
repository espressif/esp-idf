/*  Bluetooth ISO */

/*
 * SPDX-FileCopyrightText: 2020 Intel Corporation
 * SPDX-FileCopyrightText: 2021-2025 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <zephyr/autoconf.h>
#include <zephyr/bluetooth/buf.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/hci_types.h>
#include <zephyr/bluetooth/iso.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/net_buf.h>
#include <zephyr/sys/__assert.h>
#include <zephyr/sys/atomic.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/check.h>
#include <zephyr/sys/slist.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/util_macro.h>
#include <zephyr/toolchain.h>

#include "host/hci_core.h"
#include "host/conn_internal.h"
#include "iso_internal.h"

#include "common/host.h"

#if CONFIG_FREERTOS_HZ < 1000
#warning "FreeRTOS with 1ms tick rate is strongly recommended for running ISO."
#endif /* CONFIG_FREERTOS_HZ < 1000 */

LOG_MODULE_REGISTER(ISO_CORE, CONFIG_BT_ISO_LOG_LEVEL);

#if defined(CONFIG_BT_DEBUG_ISO_DATA)
#define BT_ISO_DATA_DBG(fmt, ...) LOG_DBG(fmt, ##__VA_ARGS__)
#else
#define BT_ISO_DATA_DBG(fmt, ...)
#endif /* CONFIG_BT_DEBUG_ISO_DATA */

#define iso_chan(_iso) ((_iso)->iso.chan);

struct bt_conn iso_conns[CONFIG_BT_ISO_MAX_CHAN];

/* TODO: Allow more than one server? */
#if defined(CONFIG_BT_ISO_CENTRAL)
struct bt_iso_cig cigs[CONFIG_BT_ISO_MAX_CIG];

static struct bt_iso_cig *get_cig(const struct bt_iso_chan *iso_chan);
#endif /* CONFIG_BT_ISO_CENTRAL */

#if defined(CONFIG_BT_ISO_PERIPHERAL)
static struct bt_iso_server *iso_server;

static struct bt_conn *bt_conn_add_iso(struct bt_conn *acl);
#endif /* CONFIG_BT_ISO_PERIPHERAL */

#if defined(CONFIG_BT_ISO_BROADCAST)
struct bt_iso_big bigs[CONFIG_BT_ISO_MAX_BIG];

static struct bt_iso_big *lookup_big_by_handle(uint8_t big_handle);
#endif /* CONFIG_BT_ISO_BROADCAST */

static void bt_iso_sent_cb(struct bt_conn *iso, void *user_data, int err)
{
#if defined(CONFIG_BT_ISO_TX)
    struct bt_iso_chan *chan = iso->iso.chan;
    struct bt_iso_chan_ops *ops;

    assert(chan != NULL && "NUllConnForISOSentCb");

    ops = chan->ops;

    if (!err && ops != NULL && ops->sent != NULL) {
        ops->sent(chan, user_data);
    }
#endif /* CONFIG_BT_ISO_TX */
}

void hci_iso(struct net_buf *buf)
{
    struct bt_hci_iso_hdr *hdr;
    uint16_t handle, len;
    struct bt_conn *iso;
    uint8_t flags;

    if (buf->len < sizeof(*hdr)) {
        LOG_ERR("InvIsoPktSize[%u]", buf->len);
        return;
    }

    hdr = net_buf_pull_mem(buf, sizeof(*hdr));
    len = bt_iso_hdr_len(sys_le16_to_cpu(hdr->len));
    handle = sys_le16_to_cpu(hdr->handle);
    flags = bt_iso_flags(handle);

    iso(buf)->handle = bt_iso_handle(handle);
    iso(buf)->index = BT_CONN_INDEX_INVALID;

    if (buf->len != len) {
        LOG_ERR("MismatchIsoDataLen[%u][%u]", buf->len, len);
        return;
    }

    iso = bt_conn_lookup_handle(iso(buf)->handle, BT_CONN_TYPE_ISO);
    if (iso == NULL) {
        /* Expected during BIG/CIG teardown: the bt_conn is unref'd
         * synchronously when the host initiates termination, but ISO packets
         * already queued for the iso task may still arrive here. Drop them
         * silently.
         */
        LOG_INF("IsoRxConnGone[%u]", iso(buf)->handle);
        return;
    }

    iso(buf)->index = bt_conn_index(iso);

    bt_iso_recv(iso, buf, flags);
    bt_conn_unref(iso);
}

static void iso_get_and_clear_cb(struct bt_conn *conn, struct net_buf *buf, bt_conn_tx_cb_t *cb,
                                 void **ud)
{
    if (IS_ENABLED(CONFIG_BT_ISO_TX)) {
        *cb = bt_iso_sent_cb;
    } else {
        *cb = NULL;
    }

    *ud = NULL;
}

static struct bt_conn *iso_new(void)
{
    struct bt_conn *iso = bt_conn_new(iso_conns, ARRAY_SIZE(iso_conns));

    LOG_DBG("IsoNew");

    if (iso) {
        iso->type = BT_CONN_TYPE_ISO;
        iso->get_and_clear_cb = iso_get_and_clear_cb;
    }

    return iso;
}

static int hci_le_setup_iso_data_path(const struct bt_conn *iso, uint8_t dir,
                                      const struct bt_iso_chan_path *path)
{
    struct bt_hci_cp_le_setup_iso_path *cp;
    struct bt_hci_rp_le_setup_iso_path *rp;
    struct net_buf *buf, *rsp;
    uint8_t *cc;
    int err;

    LOG_DBG("SetupIsoDataPath[%u]", dir);

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_SETUP_ISO_PATH, sizeof(*cp) + path->cc_len);
    if (!buf) {
        return -ENOBUFS;
    }

    cp = net_buf_add(buf, sizeof(*cp));
    cp->handle = sys_cpu_to_le16(iso->handle);
    cp->path_dir = dir;
    cp->path_id = path->pid;
    cp->codec_id.coding_format = path->format;
    cp->codec_id.company_id = sys_cpu_to_le16(path->cid);
    cp->codec_id.vs_codec_id = sys_cpu_to_le16(path->vid);
    sys_put_le24(path->delay, cp->controller_delay);
    cp->codec_config_len = path->cc_len;
    cc = net_buf_add(buf, path->cc_len);
    if (path->cc_len) {
        memcpy(cc, path->cc, path->cc_len);
    }
    err = bt_hci_cmd_send_sync(BT_HCI_OP_LE_SETUP_ISO_PATH, buf, &rsp);
    if (err) {
        return err;
    }

    rp = (void *)rsp->data;
    if (rp->status || (sys_le16_to_cpu(rp->handle) != iso->handle)) {
        LOG_ERR("SetupIsoDataPathFail[%02x][%u][%u]",
                rp->status, rp->handle, iso->handle);
        err = -EIO;
    }

    net_buf_unref(rsp);

    return err;
}

static void bt_iso_chan_add(struct bt_conn *iso, struct bt_iso_chan *chan)
{
    /* Attach ISO channel to the connection */
    chan->iso = iso;
    iso->iso.chan = chan;

    LOG_DBG("IsoChanAdd");
}

static int validate_iso_setup_data_path_parms(const struct bt_iso_chan *chan, uint8_t dir,
                                              const struct bt_iso_chan_path *path)
{
    struct bt_conn *iso;

    CHECKIF(chan == NULL) {
        LOG_DBG("ChanNull");

        return -EINVAL;
    }

    CHECKIF(path == NULL) {
        LOG_DBG("PathNull");

        return -EINVAL;
    }

    CHECKIF(dir != BT_HCI_DATAPATH_DIR_HOST_TO_CTLR &&
            dir != BT_HCI_DATAPATH_DIR_CTLR_TO_HOST) {
        LOG_DBG("InvDir[%u]", dir);

        return -EINVAL;
    }

    iso = chan->iso;
    if (iso == NULL) {
        LOG_DBG("ChanNotAssociated[%p]", chan);

        return -ENODEV;
    }

    if (!iso->iso.info.can_recv && dir == BT_HCI_DATAPATH_DIR_CTLR_TO_HOST) {
        LOG_DBG("InvDirForChanCannotRecv[%u][%p]", dir, chan);

        return -EINVAL;
    }

    if (!iso->iso.info.can_send && dir == BT_HCI_DATAPATH_DIR_HOST_TO_CTLR) {
        LOG_DBG("InvDirForChanCannotSend[%u][%p]", dir, chan);

        return -EINVAL;
    }

    CHECKIF(path->pid != BT_ISO_DATA_PATH_HCI &&
            !IN_RANGE(path->pid, BT_ISO_DATA_PATH_VS_ID_MIN, BT_ISO_DATA_PATH_VS_ID_MAX)) {
        LOG_DBG("InvPid[%u]", path->pid);

        return -EINVAL;
    }

    CHECKIF(path->format > BT_HCI_CODING_FORMAT_G729A &&
            path->format != BT_HCI_CODING_FORMAT_VS) {
        LOG_DBG("InvFormat[%u]", path->format);

        return -EINVAL;
    }

    CHECKIF(path->delay > BT_ISO_CONTROLLER_DELAY_MAX) {
        LOG_DBG("InvDelay[%u]", path->delay);

        return -EINVAL;
    }

    CHECKIF(path->cc_len > 0U && path->cc == NULL) {
        LOG_DBG("NoCcProvidedForCcLen[%u]", path->cc_len);

        return -EINVAL;
    }

    return 0;
}

int bt_iso_setup_data_path(const struct bt_iso_chan *chan, uint8_t dir,
                           const struct bt_iso_chan_path *path)
{
    int err;

    LOG_DBG("IsoSetupDataPath");

    err = validate_iso_setup_data_path_parms(chan, dir, path);
    if (err != 0) {
        return err;
    }

    err = hci_le_setup_iso_data_path(chan->iso, dir, path);
    if (err != 0) {
        LOG_DBG("SetDataPathFail[%d]", err);

        /* Return known possible errors */
        if (err == -ENOBUFS || err == -EIO || err == -EACCES) {
            return err;
        }

        LOG_DBG("UnkErrFromSetupIsoDataPath[%d]", err);

        return -ENOEXEC;
    }

    return 0;
}

static int hci_le_remove_iso_data_path(struct bt_conn *iso, uint8_t dir)
{
    struct bt_hci_cp_le_remove_iso_path *cp;
    struct bt_hci_rp_le_remove_iso_path *rp;
    struct net_buf *buf, *rsp;
    int err;

    LOG_DBG("RemoveIsoDataPath[%u]", dir);

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_REMOVE_ISO_PATH, sizeof(*cp));
    if (!buf) {
        return -ENOBUFS;
    }

    cp = net_buf_add(buf, sizeof(*cp));
    cp->handle = sys_cpu_to_le16(iso->handle);
    /* The path_dir is a bitfield and it's technically possible to do BIT(0) | BIT(1) but for
     * simplicity our API only supports removing a single ISO data path at a time.
     * We can convert from BT_HCI_DATAPATH_DIR_HOST_TO_CTLR and BT_HCI_DATAPATH_DIR_CTLR_TO_HOST
     * to the proper values just by using `BIT`
     */
    cp->path_dir = BIT(dir);

    err = bt_hci_cmd_send_sync(BT_HCI_OP_LE_REMOVE_ISO_PATH, buf, &rsp);
    if (err) {
        return err;
    }

    rp = (void *)rsp->data;
    if (rp->status || (sys_le16_to_cpu(rp->handle) != iso->handle)) {
        LOG_ERR("RemoveIsoDataPathFail[%02x][%u][%u]",
                rp->status, rp->handle, iso->handle);
        err = -EIO;
    }

    net_buf_unref(rsp);

    return err;
}

static int validate_iso_remove_data_path(const struct bt_iso_chan *chan, uint8_t dir)
{
    struct bt_conn *iso;

    CHECKIF(chan == NULL) {
        LOG_DBG("ChanNull");

        return -EINVAL;
    }

    CHECKIF(dir != BT_HCI_DATAPATH_DIR_HOST_TO_CTLR &&
            dir != BT_HCI_DATAPATH_DIR_CTLR_TO_HOST) {
        LOG_DBG("InvDir[%u]", dir);

        return -EINVAL;
    }

    iso = chan->iso;
    if (iso == NULL) {
        LOG_DBG("ChanNotAssociated[%p]", chan);

        return -ENODEV;
    }

    return 0;
}

int bt_iso_remove_data_path(const struct bt_iso_chan *chan, uint8_t dir)
{
    int err;

    err = validate_iso_remove_data_path(chan, dir);
    if (err != 0) {
        return err;
    }

    err = hci_le_remove_iso_data_path(chan->iso, dir);
    if (err != 0) {
        LOG_DBG("RemoveDataPathFail[%d]", err);

        /* Return known possible errors */
        if (err == -ENOBUFS || err == -EIO || err == -EACCES) {
            return err;
        }

        LOG_DBG("UnkErrFromRemoveIsoDataPath[%d]", err);

        return -ENOEXEC;
    }

    return 0;
}

void bt_iso_connected(struct bt_conn *iso)
{
    struct bt_iso_chan *chan;

    if (iso == NULL || iso->type != BT_CONN_TYPE_ISO) {
        LOG_ERR("InvParams[%p][%u]", iso, iso ? iso->type : 0);
        return;
    }

    LOG_DBG("IsoConnected");

    chan = iso_chan(iso);
    if (chan == NULL) {
        LOG_ERR("ChanFromConnectedIsoNotFound");
        return;
    }

    bt_iso_chan_set_state(chan, BT_ISO_STATE_CONNECTED);

    if (chan->ops->connected) {
        chan->ops->connected(chan);
    }
}

static void bt_iso_chan_disconnected(struct bt_iso_chan *chan, uint8_t reason)
{
    assert(chan->iso != NULL && "NullConnForIsoChan");

    const uint8_t conn_type = chan->iso->iso.info.type;

    LOG_DBG("IsoChanDisconnected[%02x]", reason);

    bt_iso_chan_set_state(chan, BT_ISO_STATE_DISCONNECTED);
    bt_conn_set_state(chan->iso, BT_CONN_DISCONNECT_COMPLETE);

    /* Calling disconnected before final cleanup allows users to use bt_iso_chan_get_info in
     * the callback and to be more similar to the ACL disconnected callback. This also means
     * that the channel cannot be reused or memset in the callback
     */
    if (chan->ops->disconnected) {
        chan->ops->disconnected(chan, reason);
    }

    /* The peripheral does not have the concept of a CIG, so once a CIS
     * disconnects it is completely freed by unref'ing it
     */
    if (IS_ENABLED(CONFIG_BT_ISO_UNICAST) &&
            (conn_type == BT_ISO_CHAN_TYPE_CENTRAL || conn_type == BT_ISO_CHAN_TYPE_PERIPHERAL)) {
        bt_iso_cleanup_acl(chan->iso);

        if (conn_type == BT_ISO_CHAN_TYPE_PERIPHERAL) {
            /* Release iso conn slot back to the pool. */
            chan->iso->type = BT_CONN_TYPE_NONE;
            bt_conn_unref(chan->iso);
            chan->iso = NULL;
#if defined(CONFIG_BT_ISO_CENTRAL)
        } else {
            bool is_chan_connected;
            struct bt_iso_cig *cig;
            struct bt_iso_chan *cis_chan;

            /* Update CIG state */
            cig = get_cig(chan);
            assert(cig != NULL && "CigNull");

            is_chan_connected = false;
            SYS_SLIST_FOR_EACH_CONTAINER(&cig->cis_channels, cis_chan, node) {
                if (cis_chan->state != BT_ISO_STATE_DISCONNECTED) {
                    is_chan_connected = true;
                    break;
                }
            }

            if (!is_chan_connected) {
                cig->state = BT_ISO_CIG_STATE_INACTIVE;
            }
#endif /* CONFIG_BT_ISO_CENTRAL */
        }
    } else if (IS_ENABLED(CONFIG_BT_ISO_BROADCASTER) &&
               conn_type == BT_ISO_CHAN_TYPE_BROADCASTER) {
        /* BIS do not get a HCI Disconnected event and will not handle cleanup of pending TX
         * complete in the same way as ACL and CIS do. Call bt_conn_tx_notify directly here
         * to flush the chan->iso->tx_complete for each disconnected BIS
         */
    } else {
        /* No special handling for BT_ISO_CHAN_TYPE_SYNC_RECEIVER */
    }
}

void bt_iso_disconnected(struct bt_conn *iso)
{
    struct bt_iso_chan *chan;

    if (iso == NULL || iso->type != BT_CONN_TYPE_ISO) {
        LOG_ERR("InvParams[%p][%u]", iso, iso ? iso->type : 0);
        return;
    }

    LOG_DBG("IsoDisconnected");

    chan = iso_chan(iso);
    if (chan == NULL) {
        LOG_ERR("ChanNotFoundForDisconnectedIso");
        return;
    }

    bt_iso_chan_disconnected(chan, iso->err);
}

#if defined(CONFIG_BT_ISO_LOG_LEVEL_DBG)
const char *bt_iso_chan_state_str(uint8_t state)
{
    switch (state) {
    case BT_ISO_STATE_DISCONNECTED:
        return "disconnected";
    case BT_ISO_STATE_CONNECTING:
        return "connecting";
    case BT_ISO_STATE_ENCRYPT_PENDING:
        return "encryption pending";
    case BT_ISO_STATE_CONNECTED:
        return "connected";
    case BT_ISO_STATE_DISCONNECTING:
        return "disconnecting";
    default:
        return "unknown";
    }
}

void bt_iso_chan_set_state_debug(struct bt_iso_chan *chan, enum bt_iso_state state,
                                 const char *func, int line)
{
    LOG_DBG("IsoChanSetState[%p][%s][%s]",
            chan, bt_iso_chan_state_str(chan->state), bt_iso_chan_state_str(state));

    /* check transitions validness */
    switch (state) {
    case BT_ISO_STATE_DISCONNECTED:
        /* regardless of old state always allows this states */
        break;
    case BT_ISO_STATE_ENCRYPT_PENDING:
        __fallthrough;
    case BT_ISO_STATE_CONNECTING:
        if (chan->state != BT_ISO_STATE_DISCONNECTED) {
            LOG_WRN("InvTransition[%s][%d]", func, line);
        }
        break;
    case BT_ISO_STATE_CONNECTED:
        if (chan->state != BT_ISO_STATE_CONNECTING) {
            LOG_WRN("InvTransition[%s][%d]", func, line);
        }
        break;
    case BT_ISO_STATE_DISCONNECTING:
        if (chan->state != BT_ISO_STATE_CONNECTING &&
                chan->state != BT_ISO_STATE_CONNECTED) {
            LOG_WRN("InvTransition[%s][%d]", func, line);
        }
        break;
    default:
        LOG_ERR("UnkState[%s][%d][%u]", func, line, state);
        return;
    }

    chan->state = state;
}
#else
void bt_iso_chan_set_state(struct bt_iso_chan *chan, enum bt_iso_state state)
{
    LOG_DBG("IsoChanSetState[%p][%u]", chan, state);
    chan->state = state;
}
#endif /* CONFIG_BT_ISO_LOG_LEVEL_DBG */

int bt_iso_chan_get_info(const struct bt_iso_chan *chan, struct bt_iso_info *info)
{
    LOG_DBG("IsoChanGetInfo");

    CHECKIF(chan == NULL) {
        LOG_ERR("ChanNull");
        return -EINVAL;
    }

    CHECKIF(chan->iso == NULL) {
        LOG_ERR("ChanIsoNull");
        return -EINVAL;
    }

    CHECKIF(info == NULL) {
        LOG_ERR("InfoNull");
        return -EINVAL;
    }

    (void)memcpy(info, &chan->iso->iso.info, sizeof(*info));

    return 0;
}

#if defined(CONFIG_BT_ISO_RX)
void bt_iso_recv(struct bt_conn *iso, struct net_buf *buf, uint8_t flags)
{
    struct bt_iso_recv_info iso_info = {0};
    struct bt_hci_iso_sdu_hdr *hdr;
    struct bt_iso_chan *chan;
    uint8_t pb, ts;
    uint16_t len, pkt_seq_no;

    pb = bt_iso_flags_pb(flags);
    ts = bt_iso_flags_ts(flags);

    /* When the PB_Flag does not equal BT_ISO_START or BT_ISO_SINGLE,
     * the fields Time_Stamp, Packet_Sequence_Number, Packet_Status_Flag
     * and ISO_SDU_Length are omitted from the HCI ISO Data packet.
     */
    switch (pb) {
    case BT_ISO_SINGLE:
        iso_info.flags = 0;

        /* The ISO_Data_Load field contains either the first fragment
         * of an SDU or a complete SDU.
         */
        if (ts) {
            struct bt_hci_iso_sdu_ts_hdr *ts_hdr;

            ts_hdr = net_buf_pull_mem(buf, sizeof(*ts_hdr));
            iso_info.ts = sys_le32_to_cpu(ts_hdr->ts);

            hdr = &ts_hdr->sdu;
            iso_info.flags |= BT_ISO_FLAGS_TS;
        } else {
            hdr = net_buf_pull_mem(buf, sizeof(*hdr));
            /* TODO: Generate a timestamp? */
            iso_info.ts = 0x00000000;
        }

        len = sys_le16_to_cpu(hdr->slen);
        flags = bt_iso_pkt_flags(len);
        len = bt_iso_pkt_len(len);
        pkt_seq_no = sys_le16_to_cpu(hdr->sn);
        iso_info.seq_num = pkt_seq_no;
        if (flags == BT_ISO_DATA_VALID) {
            iso_info.flags |= BT_ISO_FLAGS_VALID;
        } else if (flags == BT_ISO_DATA_INVALID) {
            iso_info.flags |= BT_ISO_FLAGS_ERROR;
        } else if (flags == BT_ISO_DATA_NOP) {
            iso_info.flags |= BT_ISO_FLAGS_LOST;
        } else {
            LOG_WRN("InvIsoPsFlag[%02x]", flags);
            iso_info.flags = 0;
        }
        break;

    default:
        LOG_ERR("UnexpIsoPbFlag[%02x]", pb);
        return;
    }

    chan = iso_chan(iso);
    if (chan == NULL) {
        LOG_ERR("NoChanForIsoRecv");
    } else if (chan->ops->recv != NULL) {
        chan->ops->recv(chan, &iso_info, buf->data, buf->len);
    }
}
#endif /* CONFIG_BT_ISO_RX */

#if defined(CONFIG_BT_ISO_TX)
int bt_iso_chan_send(struct bt_iso_chan *chan, struct net_buf *buf, uint16_t seq_num)
{
    return bt_le_iso_tx(chan, buf, seq_num, 0, false);
}

int bt_iso_chan_send_ts(struct bt_iso_chan *chan, struct net_buf *buf, uint16_t seq_num,
                        uint32_t ts)
{
    return bt_le_iso_tx(chan, buf, seq_num, ts, true);
}

#if defined(CONFIG_BT_ISO_CENTRAL) || defined(CONFIG_BT_ISO_BROADCASTER)
static bool valid_chan_io_qos(const struct bt_iso_chan_io_qos *io_qos, bool is_tx,
                              bool is_broadcast, bool advanced)
{
    LOG_DBG("ValidChanIoQos[%u][%u][%u]", is_tx, is_broadcast, advanced);

    if (io_qos->phy != BT_GAP_LE_PHY_1M &&
        io_qos->phy != BT_GAP_LE_PHY_2M &&
        io_qos->phy != BT_GAP_LE_PHY_CODED) {
        LOG_ERR("InvPhy[%u]", io_qos->phy);

        return false;
    }

    if (IS_ENABLED(CONFIG_BT_ISO_BROADCASTER) && is_broadcast &&
            io_qos->rtn > BT_ISO_BROADCAST_RTN_MAX) {
        LOG_ERR("InvRtn[%u]", io_qos->rtn);

        return false;
    }

#if defined(CONFIG_BT_ISO_TEST_PARAMS)
    if (advanced) {
        if (IS_ENABLED(CONFIG_BT_ISO_BROADCASTER) && is_broadcast) {
            if (!IN_RANGE(io_qos->max_pdu, BT_ISO_BROADCAST_PDU_MIN, BT_ISO_PDU_MAX)) {
                LOG_ERR("InvBroadcastPdu[%u]", io_qos->max_pdu);

                return false;
            }
        } else if (IS_ENABLED(CONFIG_BT_ISO_CENTRAL)) {
            if (!IN_RANGE(io_qos->max_pdu, BT_ISO_CONNECTED_PDU_MIN, BT_ISO_PDU_MAX)) {
                LOG_ERR("InvUnicastPdu[%u]", io_qos->max_pdu);

                return false;
            }
        }

        if (!IN_RANGE(io_qos->burst_number, BT_ISO_BN_MIN, BT_ISO_BN_MAX)) {
            LOG_ERR("InvBn[%u]", io_qos->burst_number);

            return false;
        }
    }
#else
    ARG_UNUSED(advanced);
#endif /* CONFIG_BT_ISO_TEST_PARAMS */

    return true;
}
#endif /* CONFIG_BT_ISO_CENTRAL || CONFIG_BT_ISO_BROADCASTER */

int bt_iso_chan_get_tx_sync(const struct bt_iso_chan *chan, struct bt_iso_tx_info *info)
{
    struct bt_hci_cp_le_read_iso_tx_sync *cp;
    struct bt_hci_rp_le_read_iso_tx_sync *rp;
    struct net_buf *buf;
    struct net_buf *rsp = NULL;
    int err;

    LOG_DBG("IsoChanGetTxSync");

    CHECKIF(chan == NULL) {
        LOG_ERR("ChanNUll");
        return -EINVAL;
    }

    CHECKIF(chan->iso == NULL) {
        LOG_ERR("ChanIsoNull");
        return -EINVAL;
    }

    CHECKIF(info == NULL) {
        LOG_ERR("InfoNUll");
        return -EINVAL;
    }

    CHECKIF(chan->state != BT_ISO_STATE_CONNECTED) {
        LOG_ERR("ChanNotConnected[%u]", chan->state);
        return -ENOTCONN;
    }

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_READ_ISO_TX_SYNC, sizeof(*cp));
    if (!buf) {
        return -ENOMEM;
    }

    cp = net_buf_add(buf, sizeof(*cp));
    cp->handle = sys_cpu_to_le16(chan->iso->handle);

    err = bt_hci_cmd_send_sync(BT_HCI_OP_LE_READ_ISO_TX_SYNC, buf, &rsp);
    if (err) {
        return err;
    }

    if (rsp) {
        rp = (struct bt_hci_rp_le_read_iso_tx_sync *)rsp->data;

        info->ts = sys_le32_to_cpu(rp->timestamp);
        info->seq_num = sys_le16_to_cpu(rp->seq);
        info->offset = sys_get_le24(rp->offset);

        net_buf_unref(rsp);
    } else {
        return -ENOTSUP;
    }

    return 0;
}
#endif /* CONFIG_BT_ISO_TX */

#if defined(CONFIG_BT_ISO_UNICAST)
int bt_iso_chan_disconnect(struct bt_iso_chan *chan)
{
    int err;

    LOG_DBG("IsoChanDisconnect");

    CHECKIF(!chan) {
        LOG_ERR("ChanNull");
        return -EINVAL;
    }

    CHECKIF(chan->iso == NULL) {
        LOG_ERR("ChanIsoNull");
        return -EINVAL;
    }

    if (chan->iso->iso.acl == NULL || chan->state == BT_ISO_STATE_DISCONNECTED) {
        LOG_ERR("ChanNotConnected[%u]", chan->state);
        return -ENOTCONN;
    }

    if (chan->state == BT_ISO_STATE_ENCRYPT_PENDING) {
        LOG_WRN("ChanAlreadyDisconnected");
        bt_iso_chan_set_state(chan, BT_ISO_STATE_DISCONNECTED);

        if (chan->ops->disconnected) {
            chan->ops->disconnected(chan, BT_HCI_ERR_LOCALHOST_TERM_CONN);
        }

        return 0;
    }

    if (chan->state == BT_ISO_STATE_DISCONNECTING) {
        LOG_WRN("AlreadyDisconnecting");

        return -EALREADY;
    }

    if (IS_ENABLED(CONFIG_BT_ISO_PERIPHERAL) && chan->iso->role == BT_HCI_ROLE_PERIPHERAL &&
            chan->state == BT_ISO_STATE_CONNECTING) {
        /* A CIS peripheral is not allowed to disconnect a CIS in the connecting state - It
         * has to wait for a CIS Established event
         */
        LOG_ERR("CisPerConnecting");
        return -EAGAIN;
    }

    err = bt_conn_disconnect(chan->iso, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
    if (err == 0) {
        bt_iso_chan_set_state(chan, BT_ISO_STATE_DISCONNECTING);
    }

    return err;
}

void bt_iso_cleanup_acl(struct bt_conn *iso)
{
    LOG_DBG("IsoCleanupAcl[%u]", iso->iso.acl ? iso->iso.acl->handle : UINT16_MAX);

    if (iso->iso.acl) {
        bt_conn_unref(iso->iso.acl);
        iso->iso.acl = NULL;
    }
}

static void store_cis_info(const struct bt_hci_evt_le_cis_established *evt, struct bt_conn *iso)
{
    struct bt_conn_iso *iso_conn = &iso->iso;
    struct bt_iso_info *info = &iso_conn->info;
    struct bt_iso_unicast_info *unicast_info = &info->unicast;
    struct bt_iso_unicast_tx_info *peripheral = &unicast_info->peripheral;
    struct bt_iso_unicast_tx_info *central = &unicast_info->central;
    const uint8_t c_phy = bt_get_phy(evt->c_phy);
    const uint8_t p_phy = bt_get_phy(evt->p_phy);
    struct bt_iso_chan_io_qos *tx;
    struct bt_iso_chan_io_qos *rx;
    struct bt_iso_chan *chan;

    iso_conn = &iso->iso;
    chan = iso_conn->chan;
    if (chan == NULL || chan->qos == NULL) {
        LOG_ERR("CisChanOrQosNull[%p]", chan);
        return;
    }

    rx = chan->qos->rx;
    tx = chan->qos->tx;

    LOG_DBG("StoreCisInfo[%p][%p][%p]", chan, tx, rx);

    if (iso->role == BT_HCI_ROLE_PERIPHERAL) {
        /* As of BT Core 6.0, we can only get the SDU size if the controller
         * supports bt_hci_evt_le_cis_established_v2. Since this is not guaranteeds,
         * we fallback to using the PDU size as the SDU size.
         */
        if (rx != NULL) {
            rx->phy = c_phy;
            rx->sdu = sys_le16_to_cpu(evt->c_max_pdu);
        }

        if (tx != NULL) {
            tx->phy = p_phy;
            tx->sdu = sys_le16_to_cpu(evt->p_max_pdu);
        }

        iso_conn->info.type = BT_ISO_CHAN_TYPE_PERIPHERAL;
    } else {
        /* values are already set for central - Verify */
        if (tx != NULL && tx->phy != c_phy) {
            LOG_WRN("UnexpC2PPhy[%u][%u]", c_phy, tx->phy);
            /* We assume that tx->phy has become invalid, and will use the event from
             * the controller as the truth
             */
            tx->phy = c_phy;
        }

        if (rx != NULL && rx->phy != p_phy) {
            LOG_WRN("UnexpP2CMaxSdu[%u][%u]", p_phy, rx->phy);
            /* We assume that rx->phy has become invalid, and will use the event from
             * the controller as the truth
             */
            rx->phy = p_phy;
        }
    }

    /* Verify if device can send */
    iso_conn->info.can_send = false;
    if (tx != NULL) {
        if (iso->role == BT_HCI_ROLE_PERIPHERAL && evt->p_bn > 0) {
            iso_conn->info.can_send = true;
        } else if (iso->role == BT_HCI_ROLE_CENTRAL && evt->c_bn > 0) {
            iso_conn->info.can_send = true;
        }
    }

    /* Verify if device can recv */
    iso_conn->info.can_recv = false;
    if (rx != NULL) {
        if (iso->role == BT_HCI_ROLE_PERIPHERAL && evt->c_bn > 0) {
            iso_conn->info.can_recv = true;
        } else if (iso->role == BT_HCI_ROLE_CENTRAL && evt->p_bn > 0) {
            iso_conn->info.can_recv = true;
        }
    }

    info->iso_interval = sys_le16_to_cpu(evt->interval);
    info->max_subevent = evt->nse;

    unicast_info->cig_sync_delay = sys_get_le24(evt->cig_sync_delay);
    unicast_info->cis_sync_delay = sys_get_le24(evt->cis_sync_delay);
    unicast_info->cig_id = iso_conn->info.unicast.cig_id;
    unicast_info->cis_id = iso_conn->info.unicast.cis_id;

    central->bn = evt->c_bn;
    central->phy = bt_get_phy(evt->c_phy);
    central->latency = sys_get_le16(evt->c_latency);
    central->max_pdu = sys_le16_to_cpu(evt->c_max_pdu);
    /* Transform to n * 1.25ms */
    central->flush_timeout = info->iso_interval * evt->c_ft;

    peripheral->bn = evt->p_bn;
    peripheral->phy = bt_get_phy(evt->p_phy);
    peripheral->latency = sys_get_le16(evt->p_latency);
    peripheral->max_pdu = sys_le16_to_cpu(evt->p_max_pdu);
    /* Transform to n * 1.25ms */
    peripheral->flush_timeout = info->iso_interval * evt->p_ft;

    /* The following values are only available with bt_hci_evt_le_cis_established_v2 so
     * initialize them to the "unknown" values
     */
    unicast_info->subinterval = BT_ISO_SUBINTERVAL_UNKNOWN;

    if (iso->role == BT_HCI_ROLE_PERIPHERAL) {
        central->max_sdu = central->max_pdu;
        central->sdu_interval = BT_ISO_SDU_INTERVAL_UNKNOWN;

        peripheral->max_sdu = peripheral->max_pdu;
        peripheral->sdu_interval = BT_ISO_SDU_INTERVAL_UNKNOWN;
    } else {
        central->max_sdu = tx == NULL ? 0 : tx->sdu;
        central->sdu_interval = BT_ISO_SDU_INTERVAL_UNKNOWN;

        peripheral->max_sdu = rx == NULL ? 0 : rx->sdu;
        peripheral->sdu_interval = BT_ISO_SDU_INTERVAL_UNKNOWN;
    }
}

/** Only store information that is not stored by store_cis_info
 * Assumes that store_cis_info has been called first
 */
static void store_cis_info_v2(const struct bt_hci_evt_le_cis_established_v2 *evt,
                              struct bt_conn *iso)
{
    struct bt_conn_iso *iso_conn = &iso->iso;
    struct bt_iso_info *info = &iso_conn->info;
    struct bt_iso_unicast_info *unicast_info = &info->unicast;
    struct bt_iso_unicast_tx_info *peripheral = &unicast_info->peripheral;
    struct bt_iso_unicast_tx_info *central = &unicast_info->central;
    const uint16_t c_max_sdu = sys_le16_to_cpu(evt->c_max_sdu);
    const uint16_t p_max_sdu = sys_le16_to_cpu(evt->p_max_sdu);
    struct bt_iso_chan_io_qos *tx;
    struct bt_iso_chan_io_qos *rx;
    struct bt_iso_chan *chan;

    /* The v1 version of the event is a subset of the v2 version - We can thus use the
     * store_cis_info function for the majority of the info
     */
    store_cis_info((const struct bt_hci_evt_le_cis_established *)evt, iso);

    LOG_DBG("StoreCisInfoV2");

    chan = iso_conn->chan;
    rx = chan->qos->rx;
    tx = chan->qos->tx;

    if (iso->role == BT_HCI_ROLE_PERIPHERAL) {
        /* Update the SDU sizes in the IO QoS fields stored by store_cis_info */
        if (rx != NULL) {
            rx->sdu = c_max_sdu;
        }

        if (tx != NULL) {
            tx->sdu = p_max_sdu;
        }
    } else {
        /* values are already set for central - Verify */
        if (tx != NULL && tx->sdu != c_max_sdu) {
            LOG_WRN("UnexpC2PMaxSdu[%u][%u]", c_max_sdu, tx->sdu);
            /* We assume that tx->sdu has become invalid, and will use the event from
             * the controller as the truth
             */
            tx->sdu = c_max_sdu;
        }

        if (rx != NULL && rx->sdu != p_max_sdu) {
            LOG_WRN("UnexpP2CMaxSdu[%u][%u]", p_max_sdu, rx->sdu);
            /* We assume that rx->sdu has become invalid, and will use the event from
             * the controller as the truth
             */
            rx->sdu = p_max_sdu;
        }
    }

    unicast_info->subinterval = sys_get_le24(evt->sub_interval);

    central->max_sdu = sys_le16_to_cpu(evt->c_max_sdu);
    central->sdu_interval = sys_get_le24(evt->c_sdu_interval);

    peripheral->max_sdu = sys_le16_to_cpu(evt->p_max_sdu);
    peripheral->sdu_interval = sys_get_le24(evt->p_sdu_interval);
}

void hci_le_cis_established(struct net_buf *buf)
{
    struct bt_hci_evt_le_cis_established *evt = (void *)buf->data;
    uint16_t handle = sys_le16_to_cpu(evt->conn_handle);
    struct bt_conn *iso;

    LOG_INF("CisEstab");
    LOG_INF("Evt[%02x][%04x][%u][%u][%u][%u][%u][%u][%u][%u][%u][%u][%u][%u][%u][%u]",
            evt->status, handle, sys_get_le24(evt->cig_sync_delay),
            sys_get_le24(evt->cis_sync_delay), sys_get_le24(evt->c_latency),
            sys_get_le24(evt->p_latency), evt->c_phy, evt->p_phy, evt->nse,
            evt->c_bn, evt->p_bn, evt->c_ft, evt->p_ft, evt->c_max_pdu,
            evt->p_max_pdu, evt->interval);

    /* ISO connection handles are already assigned at this point */
    iso = bt_conn_lookup_handle(handle, BT_CONN_TYPE_ISO);
    if (!iso) {
        /* If it is a local disconnect, then we may have received the disconnect complete
         * event before this event, and in which case we do not expect to find the CIS
         * object
         */
        if (evt->status != BT_HCI_ERR_OP_CANCELLED_BY_HOST) {
            LOG_ERR("NoConnFoundForHdl[%u]", handle);
        }

        return;
    }

    if (evt->status == BT_HCI_ERR_SUCCESS) {
        store_cis_info(evt, iso);
        bt_conn_set_state(iso, BT_CONN_CONNECTED);
    } else if (iso->role == BT_HCI_ROLE_PERIPHERAL ||
               evt->status != BT_HCI_ERR_OP_CANCELLED_BY_HOST) {
        iso->err = evt->status;
        bt_iso_disconnected(iso);
    } /* else we wait for disconnect event */

    bt_conn_unref(iso);
}

void hci_le_cis_established_v2(struct net_buf *buf)
{
    struct bt_hci_evt_le_cis_established_v2 *evt = (void *)buf->data;
    uint16_t handle = sys_le16_to_cpu(evt->conn_handle);
    struct bt_conn *iso;

    LOG_INF("CisEstabV2");
    LOG_INF("Evt[%02x][%04x][%u][%u][%u][%u][%u][%u][%u][%u][%u][%u][%u][%u][%u][%u][%u][%u][%u][%u][%u][%u]",
            evt->status, handle, sys_get_le24(evt->cig_sync_delay),
            sys_get_le24(evt->cis_sync_delay), sys_get_le24(evt->c_latency),
            sys_get_le24(evt->p_latency), evt->c_phy, evt->p_phy, evt->nse,
            evt->c_bn, evt->p_bn, evt->c_ft, evt->p_ft, evt->c_max_pdu,
            evt->p_max_pdu, evt->interval, sys_get_le24(evt->sub_interval),
            evt->c_max_sdu, evt->p_max_sdu, sys_get_le24(evt->c_sdu_interval),
            sys_get_le24(evt->p_sdu_interval), evt->framing);

    /* ISO connection handles are already assigned at this point */
    iso = bt_conn_lookup_handle(handle, BT_CONN_TYPE_ISO);
    if (!iso) {
        /* If it is a local disconnect, then we may have received the disconnect complete
         * event before this event, and in which case we do not expect to find the CIS
         * object
         */
        if (evt->status != BT_HCI_ERR_OP_CANCELLED_BY_HOST) {
            LOG_ERR("NoConnFoundForHdl[%u]", handle);
        }

        return;
    }

    if (evt->status == BT_HCI_ERR_SUCCESS) {
        store_cis_info_v2(evt, iso);
        bt_conn_set_state(iso, BT_CONN_CONNECTED);
    } else if (iso->role == BT_HCI_ROLE_PERIPHERAL ||
               evt->status != BT_HCI_ERR_OP_CANCELLED_BY_HOST) {
        iso->err = evt->status;
        bt_iso_disconnected(iso);
    } /* else we wait for disconnect event */

    bt_conn_unref(iso);
}

#if defined(CONFIG_BT_ISO_PERIPHERAL)
int bt_iso_server_register(struct bt_iso_server *server)
{
    LOG_DBG("IsoSrvReg");

    CHECKIF(!server) {
        LOG_ERR("SrvNull");
        return -EINVAL;
    }

    /* Check if controller is ISO capable */
    if (!BT_FEAT_LE_CIS_PERIPHERAL(bt_dev.le.features)) {
        LOG_ERR("CisPerNotSupp");
        return -ENOTSUP;
    }

    if (iso_server) {
        LOG_WRN("SrvExist");
        return -EADDRINUSE;
    }

    if (!server->accept) {
        LOG_ERR("SrvAcceptNull");
        return -EINVAL;
    }

#if defined(CONFIG_BT_SMP)
    if (server->sec_level > BT_SECURITY_L3) {
        LOG_ERR("InvSrvSecLevel[%u]", server->sec_level);
        return -EINVAL;
    } else if (server->sec_level < BT_SECURITY_L1) {
        LOG_DBG("SrvSecLevelUpdatedToL1");
        /* Level 0 is only applicable for BR/EDR */
        server->sec_level = BT_SECURITY_L1;
    }
#endif /* CONFIG_BT_SMP */

    iso_server = server;

    return 0;
}

int bt_iso_server_unregister(struct bt_iso_server *server)
{
    LOG_DBG("IsoSrvUnreg");

    CHECKIF(!server) {
        LOG_ERR("SrvNull");
        return -EINVAL;
    }

    if (iso_server != server) {
        LOG_ERR("SrvMismatch[%p][%p]", iso_server, server);
        return -EINVAL;
    }

    iso_server = NULL;

    return 0;
}

static int iso_accept(struct bt_conn *acl, struct bt_conn *iso)
{
    struct bt_iso_accept_info accept_info;
    struct bt_iso_chan *chan;
    int err;

    CHECKIF(!iso || iso->type != BT_CONN_TYPE_ISO) {
        LOG_ERR("InvParams[%p][%u]", iso, iso ? iso->type : 0);
        return -EINVAL;
    }

    LOG_DBG("IsoAccept");

    accept_info.acl = acl;
    accept_info.cig_id = iso->iso.info.unicast.cig_id;
    accept_info.cis_id = iso->iso.info.unicast.cis_id;

    err = iso_server->accept(&accept_info, &chan);
    if (err < 0) {
        LOG_ERR("IsoSrvAcceptFail[%d]", err);
        return err;
    }

    if (chan == NULL) {
        LOG_ERR("AcceptCbReturnedNullChan");
        return -ENOMEM;
    }

#if defined(CONFIG_BT_SMP)
    chan->required_sec_level = iso_server->sec_level;
#endif /* CONFIG_BT_SMP */

    bt_iso_chan_add(iso, chan);
    bt_iso_chan_set_state(chan, BT_ISO_STATE_CONNECTING);

    return 0;
}

static int hci_le_reject_cis(uint16_t handle, uint8_t reason)
{
    struct bt_hci_cp_le_reject_cis *cp;
    struct net_buf *buf;
    int err;

    LOG_DBG("RejectCis[%u][%02x]", handle, reason);

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_REJECT_CIS, sizeof(*cp));
    if (!buf) {
        return -ENOBUFS;
    }

    cp = net_buf_add(buf, sizeof(*cp));
    cp->handle = sys_cpu_to_le16(handle);
    cp->reason = reason;

    err = bt_hci_cmd_send_sync(BT_HCI_OP_LE_REJECT_CIS, buf, NULL);
    if (err) {
        return err;
    }

    return 0;
}

static int hci_le_accept_cis(uint16_t handle)
{
    struct bt_hci_cp_le_accept_cis *cp;
    struct net_buf *buf;
    int err;

    LOG_DBG("AcceptCis[%u]", handle);

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_ACCEPT_CIS, sizeof(*cp));
    if (!buf) {
        return -ENOBUFS;
    }

    cp = net_buf_add(buf, sizeof(*cp));
    cp->handle = sys_cpu_to_le16(handle);

    err = bt_hci_cmd_send_sync(BT_HCI_OP_LE_ACCEPT_CIS, buf, NULL);
    if (err) {
        return err;
    }

    return 0;
}

static uint8_t iso_server_check_security(struct bt_conn *conn)
{
    LOG_DBG("IsoSrvCheckSec[%u][%u]", conn->sec_level, iso_server->sec_level);

    if (IS_ENABLED(CONFIG_BT_CONN_DISABLE_SECURITY)) {
        return BT_HCI_ERR_SUCCESS;
    }

#if defined(CONFIG_BT_SMP)
    if (conn->sec_level >= iso_server->sec_level) {
        return BT_HCI_ERR_SUCCESS;
    }

    return BT_HCI_ERR_INSUFFICIENT_SECURITY;
#else
    return BT_HCI_ERR_SUCCESS;
#endif /* CONFIG_BT_SMP */
}

void hci_le_cis_req(struct net_buf *buf)
{
    struct bt_hci_evt_le_cis_req *evt = (void *)buf->data;
    uint16_t acl_handle = sys_le16_to_cpu(evt->acl_handle);
    uint16_t cis_handle = sys_le16_to_cpu(evt->cis_handle);
    struct bt_conn *acl, *iso;
    uint8_t sec_err;
    int err;

    LOG_DBG("CisReqEvt[%u][%u][%u][%u]",
            acl_handle, cis_handle, evt->cig_id, evt->cis_id);

    if (iso_server == NULL) {
        LOG_WRN("NoIsoSrvReg");
        hci_le_reject_cis(cis_handle, BT_HCI_ERR_UNSPECIFIED);
        return;
    }

    /* Lookup existing connection with same handle */
    iso = bt_conn_lookup_handle(cis_handle, BT_CONN_TYPE_ISO);
    if (iso) {
        LOG_ERR("InvCisHdl[%u]", cis_handle);
        hci_le_reject_cis(cis_handle, BT_HCI_ERR_CONN_LIMIT_EXCEEDED);
        bt_conn_unref(iso);
        return;
    }

    /* Lookup ACL connection to attach */
    acl = bt_conn_lookup_handle(acl_handle, BT_CONN_TYPE_LE);
    if (!acl) {
        LOG_ERR("InvAclHdl[%u]", acl_handle);
        hci_le_reject_cis(cis_handle, BT_HCI_ERR_UNKNOWN_CONN_ID);
        return;
    }

    sec_err = iso_server_check_security(acl);
    if (sec_err != BT_HCI_ERR_SUCCESS) {
        LOG_DBG("InsuffSec[%u]", sec_err);
        err = hci_le_reject_cis(cis_handle, sec_err);
        if (err != 0) {
            LOG_ERR("RejectCisFail[%d]", err);
        }

        bt_conn_unref(acl);
        return;
    }

    /* Add ISO connection */
    iso = bt_conn_add_iso(acl);

    bt_conn_unref(acl);

    if (!iso) {
        LOG_ERR("AddCisToAclFail[%u]", acl_handle);
        hci_le_reject_cis(cis_handle, BT_HCI_ERR_INSUFFICIENT_RESOURCES);
        return;
    }

    iso->iso.info.type = BT_ISO_CHAN_TYPE_PERIPHERAL;
    iso->iso.info.unicast.cig_id = evt->cig_id;
    iso->iso.info.unicast.cis_id = evt->cis_id;

    /* Request application to accept */
    err = iso_accept(acl, iso);
    if (err) {
        LOG_DBG("AppRejectedCis[%d]", err);
        bt_iso_cleanup_acl(iso);
        bt_conn_unref(iso);
        hci_le_reject_cis(cis_handle, BT_HCI_ERR_INSUFFICIENT_RESOURCES);
        return;
    }

    iso->handle = cis_handle;
    iso->role = BT_HCI_ROLE_PERIPHERAL;
    bt_conn_set_state(iso, BT_CONN_INITIATING);

    err = hci_le_accept_cis(cis_handle);
    if (err) {
        bt_iso_cleanup_acl(iso);
        bt_conn_unref(iso);
        hci_le_reject_cis(cis_handle, BT_HCI_ERR_INSUFFICIENT_RESOURCES);
        return;
    }
}

static struct bt_conn *bt_conn_add_iso(struct bt_conn *acl)
{
    struct bt_conn *iso = iso_new();

    LOG_DBG("ConnAddIso[%u]", acl->handle);

    if (iso == NULL) {
        LOG_ERR("NoFreeIso");
        return NULL;
    }

    iso->iso.acl = bt_conn_ref(acl);

    return iso;
}
#endif /* CONFIG_BT_ISO_PERIPHERAL */

#if defined(CONFIG_BT_ISO_CENTRAL)
static bool valid_chan_qos(const struct bt_iso_chan_qos *qos, bool advanced)
{
    LOG_DBG("ValidChanQos[%u]", advanced);

#if defined(CONFIG_BT_ISO_TEST_PARAMS)
    if (advanced && !IN_RANGE(qos->num_subevents, BT_ISO_NSE_MIN, BT_ISO_NSE_MAX)) {
        LOG_ERR("InvNse[%u]", qos->num_subevents);

        return false;
    }
#endif /* CONFIG_BT_ISO_TEST_PARAMS */

    if (qos->rx == NULL && qos->tx == NULL) {
        LOG_DBG("BothRxAndTxQosNull");
        return false;
    }

    if (qos->rx != NULL && !valid_chan_io_qos(qos->rx, false, false, advanced)) {
        LOG_DBG("InvRxQos");
        return false;
    }

    if (qos->tx != NULL && !valid_chan_io_qos(qos->tx, true, false, advanced)) {
        LOG_DBG("InvTxQos");
        return false;
    }

    return true;
}

static int hci_le_remove_cig(uint8_t cig_id)
{
    struct bt_hci_cp_le_remove_cig *req;
    struct net_buf *buf;

    LOG_DBG("RemoveCig[%u]", cig_id);

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_REMOVE_CIG, sizeof(*req));
    if (!buf) {
        return -ENOBUFS;
    }

    req = net_buf_add(buf, sizeof(*req));

    memset(req, 0, sizeof(*req));

    req->cig_id = cig_id;

    return bt_hci_cmd_send_sync(BT_HCI_OP_LE_REMOVE_CIG, buf, NULL);
}

static struct net_buf *hci_le_set_cig_params(const struct bt_iso_cig *cig,
                                             const struct bt_iso_cig_param *param)
{
    struct bt_hci_cp_le_set_cig_params *req;
    struct bt_hci_cis_params *cis_param;
    struct net_buf *buf;
    struct net_buf *rsp;
    int i, err;

    LOG_DBG("SetCigParams[%u]", cig->id);

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_SET_CIG_PARAMS,
                            sizeof(*req) + sizeof(*cis_param) * param->num_cis);
    if (!buf) {
        return NULL;
    }

    req = net_buf_add(buf, sizeof(*req));

    memset(req, 0, sizeof(*req));

    req->cig_id = cig->id;
    req->c_latency = sys_cpu_to_le16(param->c_to_p_latency);
    req->p_latency = sys_cpu_to_le16(param->p_to_c_latency);
    sys_put_le24(param->c_to_p_interval, req->c_interval);
    sys_put_le24(param->p_to_c_interval, req->p_interval);

    req->sca = param->sca;
    req->packing = param->packing;
    req->framing = param->framing;
    req->num_cis = param->num_cis;

    LOG_DBG("[%u][%u][%u][%u][%u][%u][%u][%u][%u]",
            cig->id, param->c_to_p_latency, param->p_to_c_latency,
            param->c_to_p_interval, param->p_to_c_interval, param->sca,
            param->packing, param->framing, param->num_cis);

    /* Program the cis parameters */
    for (i = 0; i < param->num_cis; i++) {
        struct bt_iso_chan *cis = param->cis_channels[i];
        struct bt_iso_chan_qos *qos = cis->qos;

        cis_param = net_buf_add(buf, sizeof(*cis_param));

        memset(cis_param, 0, sizeof(*cis_param));

        cis_param->cis_id = cis->iso->iso.info.unicast.cis_id;

        if (!qos->tx && !qos->rx) {
            LOG_ERR("BothCisTxAndRxDisabled");
            net_buf_unref(buf);
            return NULL;
        }

        if (!qos->tx) {
            /* Use RX PHY if TX is not set (disabled)
             * to avoid setting invalid values
             */
            cis_param->c_phy = qos->rx->phy;
        } else {
            cis_param->c_sdu = sys_cpu_to_le16(qos->tx->sdu);
            cis_param->c_phy = qos->tx->phy;
            cis_param->c_rtn = qos->tx->rtn;
        }

        if (!qos->rx) {
            /* Use TX PHY if RX is not set (disabled)
             * to avoid setting invalid values
             */
            cis_param->p_phy = qos->tx->phy;
        } else {
            cis_param->p_sdu = sys_cpu_to_le16(qos->rx->sdu);
            cis_param->p_phy = qos->rx->phy;
            cis_param->p_rtn = qos->rx->rtn;
        }

        LOG_DBG("[%d]:[%u][%u][%u][%u][%u][%u][%u]",
                i, cis_param->cis_id, cis_param->c_phy, cis_param->c_sdu,
                cis_param->c_rtn, cis_param->p_phy, cis_param->p_sdu,
                cis_param->p_rtn);
    }

    err = bt_hci_cmd_send_sync(BT_HCI_OP_LE_SET_CIG_PARAMS, buf, &rsp);
    if (err) {
        return NULL;
    }

    return rsp;
}

#if defined(CONFIG_BT_ISO_TEST_PARAMS)
static struct net_buf *hci_le_set_cig_test_params(const struct bt_iso_cig *cig,
                                                  const struct bt_iso_cig_param *param)
{
    struct bt_hci_cp_le_set_cig_params_test *req;
    struct bt_hci_cis_params_test *cis_param;
    struct net_buf *buf;
    struct net_buf *rsp;
    int err;

    LOG_DBG("SetCigTestParams");

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_SET_CIG_PARAMS_TEST,
                            sizeof(*req) + sizeof(*cis_param) * param->num_cis);
    if (!buf) {
        return NULL;
    }

    req = net_buf_add(buf, sizeof(*req));

    memset(req, 0, sizeof(*req));

    req->cig_id = cig->id;
    sys_put_le24(param->c_to_p_interval, req->c_interval);
    sys_put_le24(param->p_to_c_interval, req->p_interval);

    req->c_ft = param->c_to_p_ft;
    req->p_ft = param->p_to_c_ft;
    req->iso_interval = sys_cpu_to_le16(param->iso_interval);
    req->sca = param->sca;
    req->packing = param->packing;
    req->framing = param->framing;
    req->num_cis = param->num_cis;

    LOG_DBG("[%u][%u][%u][%u][%u][%u][%u][%u][%u][%u]",
            cig->id, param->c_to_p_interval, param->p_to_c_interval,
            param->c_to_p_ft, param->p_to_c_ft, param->iso_interval,
            param->sca, param->packing, param->framing, param->num_cis);

    /* Program the cis parameters */
    for (uint8_t i = 0U; i < param->num_cis; i++) {
        const struct bt_iso_chan *cis = param->cis_channels[i];
        const struct bt_iso_chan_qos *qos = cis->qos;

        cis_param = net_buf_add(buf, sizeof(*cis_param));

        memset(cis_param, 0, sizeof(*cis_param));

        cis_param->cis_id = cis->iso->iso.info.unicast.cis_id;
        cis_param->nse = qos->num_subevents;

        if (!qos->tx && !qos->rx) {
            LOG_ERR("BothCisTxAndRxDisabled");
            net_buf_unref(buf);
            return NULL;
        }

        if (!qos->tx) {
            /* Use RX PHY if TX is not set (disabled)
             * to avoid setting invalid values
             */
            cis_param->c_phy = qos->rx->phy;
        } else {
            cis_param->c_sdu = sys_cpu_to_le16(qos->tx->sdu);
            cis_param->c_pdu = sys_cpu_to_le16(qos->tx->max_pdu);
            cis_param->c_phy = qos->tx->phy;
            cis_param->c_bn = qos->tx->burst_number;
        }

        if (!qos->rx) {
            /* Use TX PHY if RX is not set (disabled)
             * to avoid setting invalid values
             */
            cis_param->p_phy = qos->tx->phy;
        } else {
            cis_param->p_sdu = sys_cpu_to_le16(qos->rx->sdu);
            cis_param->p_pdu = sys_cpu_to_le16(qos->rx->max_pdu);
            cis_param->p_phy = qos->rx->phy;
            cis_param->p_bn = qos->rx->burst_number;
        }

        LOG_DBG("[%d]:[%u][%u][%u][%u][%u][%u][%u][%u][%u][%u]",
                i, cis_param->cis_id, cis_param->nse, cis_param->c_sdu,
                cis_param->p_sdu, cis_param->c_pdu, cis_param->p_pdu,
                cis_param->c_phy, cis_param->p_phy, cis_param->c_bn,
                cis_param->p_bn);
    }

    err = bt_hci_cmd_send_sync(BT_HCI_OP_LE_SET_CIG_PARAMS_TEST, buf, &rsp);
    if (err) {
        return NULL;
    }

    return rsp;
}

static bool is_advanced_cig_param(const struct bt_iso_cig_param *param)
{
    LOG_DBG("IsAdvancedCigParam[%u][%u][%u]",
            param->c_to_p_ft, param->p_to_c_ft, param->iso_interval);

    if (param->c_to_p_ft != 0U || param->p_to_c_ft != 0U || param->iso_interval != 0U) {
        return true;
    }

    /* Check if any of the CIS contain any test-param-only values */
    for (uint8_t i = 0U; i < param->num_cis; i++) {
        const struct bt_iso_chan *cis = param->cis_channels[i];
        const struct bt_iso_chan_qos *qos = cis->qos;

        if (qos->num_subevents > 0U) {
            return true;
        }

        if (qos->tx != NULL) {
            if (qos->tx->max_pdu > 0U || qos->tx->burst_number > 0U) {
                return true;
            }
        }

        if (qos->rx != NULL) {
            if (qos->rx->max_pdu > 0U || qos->rx->burst_number > 0U) {
                return true;
            }
        }
    }

    return false;
}
#endif /* CONFIG_BT_ISO_TEST_PARAMS */

static struct bt_iso_cig *get_cig(const struct bt_iso_chan *iso_chan)
{
    LOG_DBG("GetCig[%p]", iso_chan);

    if (iso_chan == NULL || iso_chan->iso == NULL) {
        return NULL;
    }

    assert(iso_chan->iso->iso.info.unicast.cig_id < ARRAY_SIZE(cigs) && "InvCIGID");

    return &cigs[iso_chan->iso->iso.info.unicast.cig_id];
}

static struct bt_iso_cig *get_free_cig(void)
{
    /* We can use the index in the `cigs` array as CIG ID */

    LOG_DBG("GetFreeCig");

    for (size_t i = 0; i < ARRAY_SIZE(cigs); i++) {
        if (cigs[i].state == BT_ISO_CIG_STATE_IDLE) {
            cigs[i].state = BT_ISO_CIG_STATE_CONFIGURED;
            cigs[i].id = i;
            sys_slist_init(&cigs[i].cis_channels);
            LOG_DBG("CigNew[%zu]", i);
            return &cigs[i];
        }
    }

    LOG_ERR("NoFreeCig");

    return NULL;
}

static bool cis_is_in_cig(const struct bt_iso_cig *cig, const struct bt_iso_chan *cis)
{
    if (cig == NULL || cis == NULL || cis->iso == NULL) {
        return false;
    }

    return cig->id == cis->iso->iso.info.unicast.cig_id;
}

static int cig_init_cis(struct bt_iso_cig *cig, const struct bt_iso_cig_param *param)
{
    LOG_DBG("CigInitCis[%u]", cig->id);

    for (uint8_t i = 0; i < param->num_cis; i++) {
        struct bt_iso_chan *cis = param->cis_channels[i];

        if (cis->iso == NULL) {
            struct bt_conn_iso *iso_conn;

            cis->iso = iso_new();
            if (cis->iso == NULL) {
                LOG_ERR("NoFreeCis");
                return -ENOMEM;
            }
            iso_conn = &cis->iso->iso;

            iso_conn->info.unicast.cig_id = cig->id;
            iso_conn->info.type = BT_ISO_CHAN_TYPE_CENTRAL;
            iso_conn->info.unicast.cis_id = cig->num_cis++;

            bt_iso_chan_add(cis->iso, cis);

            sys_slist_append(&cig->cis_channels, &cis->node);
        } /* else already initialized */
    }

    return 0;
}

static void cleanup_cig(struct bt_iso_cig *cig)
{
    struct bt_iso_chan *cis, *tmp;

    LOG_DBG("CleanupCig");

    SYS_SLIST_FOR_EACH_CONTAINER_SAFE(&cig->cis_channels, cis, tmp, node) {
        if (cis->iso != NULL) {
            /* Release iso conn slot back to the pool. */
            cis->iso->type = BT_CONN_TYPE_NONE;
            bt_conn_unref(cis->iso);
            cis->iso = NULL;
        }

        sys_slist_remove(&cig->cis_channels, NULL, &cis->node);
    }

    memset(cig, 0, sizeof(*cig));
}

static bool valid_cig_param(const struct bt_iso_cig_param *param, bool advanced,
                            const struct bt_iso_cig *cig)
{
    bool is_c_to_p = false;
    bool is_p_to_c = false;

    if (param == NULL) {
        return false;
    }

    if (param->num_cis > BT_ISO_MAX_GROUP_ISO_COUNT ||
            param->num_cis > CONFIG_BT_ISO_MAX_CHAN) {
        LOG_ERR("TooLargeNumCis[%u][%u]",
                param->num_cis, MAX(CONFIG_BT_ISO_MAX_CHAN, BT_ISO_MAX_GROUP_ISO_COUNT));
        return false;
    }

    for (uint8_t i = 0; i < param->num_cis; i++) {
        struct bt_iso_chan *cis = param->cis_channels[i];

        if (cis == NULL) {
            LOG_ERR("CisChansNull[%u]", i);
            return false;
        }

        if (cis->qos == NULL) {
            LOG_ERR("CisChanQosNull[%u]", i);
            return false;
        }

        if (cis->iso != NULL && !cis_is_in_cig(cig, cis)) {
            LOG_WRN("CisChanExist[%u][%p]", i, get_cig(cis));
            return false;
        }

        if (!valid_chan_qos(cis->qos, advanced)) {
            LOG_ERR("InvCisChanQos[%u]", i);
            return false;
        }

        for (uint8_t j = 0; j < i; j++) {
            if (cis == param->cis_channels[j]) {
                LOG_ERR("DuplicateCis[%u][%u]", i, j);
                return false;
            }
        }

        if (cis->qos->rx != NULL && cis->qos->rx->sdu != 0U) {
            is_p_to_c = true;
        }

        if (cis->qos->tx != NULL && cis->qos->tx->sdu != 0U) {
            is_c_to_p = true;
        }
    }

    if (param->framing != BT_ISO_FRAMING_UNFRAMED && param->framing != BT_ISO_FRAMING_FRAMED) {
        LOG_ERR("InvFraming[%u]", param->framing);
        return false;
    }

    if (param->packing != BT_ISO_PACKING_SEQUENTIAL &&
            param->packing != BT_ISO_PACKING_INTERLEAVED) {
        LOG_ERR("InvPacking[%u]", param->packing);
        return false;
    }

    if (is_c_to_p &&
            !IN_RANGE(param->c_to_p_interval, BT_ISO_SDU_INTERVAL_MIN, BT_ISO_SDU_INTERVAL_MAX)) {
        LOG_ERR("InvC2PInterval[%u]", param->c_to_p_interval);
        return false;
    }

    if (is_p_to_c &&
            !IN_RANGE(param->p_to_c_interval, BT_ISO_SDU_INTERVAL_MIN, BT_ISO_SDU_INTERVAL_MAX)) {
        LOG_ERR("InvP2CInterval[%u]", param->p_to_c_interval);
        return false;
    }

    if (!advanced) {
        if (is_c_to_p &&
                !IN_RANGE(param->c_to_p_latency, BT_ISO_LATENCY_MIN, BT_ISO_LATENCY_MAX)) {
            LOG_ERR("InvC2PLatency[%u]", param->c_to_p_latency);
            return false;
        }

        if (is_p_to_c &&
                !IN_RANGE(param->p_to_c_latency, BT_ISO_LATENCY_MIN, BT_ISO_LATENCY_MAX)) {
            LOG_ERR("InvP2CLatency[%u]", param->p_to_c_latency);
            return false;
        }
    }

#if defined(CONFIG_BT_ISO_TEST_PARAMS)
    if (advanced) {
        if (!IN_RANGE(param->c_to_p_ft, BT_ISO_FT_MIN, BT_ISO_FT_MAX)) {
            LOG_ERR("InvC2PFt[%u]", param->c_to_p_ft);

            return false;
        }

        if (!IN_RANGE(param->p_to_c_ft, BT_ISO_FT_MIN, BT_ISO_FT_MAX)) {
            LOG_ERR("InvP2CFt[%u]", param->p_to_c_ft);

            return false;
        }

        if (!IN_RANGE(param->iso_interval, BT_ISO_ISO_INTERVAL_MIN,
                      BT_ISO_ISO_INTERVAL_MAX)) {
            LOG_ERR("InvIsoInterval[%u]", param->iso_interval);

            return false;
        }
    }
#endif /* CONFIG_BT_ISO_TEST_PARAMS */

    return true;
}

int bt_iso_cig_create(const struct bt_iso_cig_param *param, struct bt_iso_cig **out_cig)
{
    int err;
    struct net_buf *rsp = NULL;
    struct bt_iso_cig *cig;
    struct bt_hci_rp_le_set_cig_params *cig_rsp;
    struct bt_iso_chan *cis;
    bool advanced = false;
    int i;

    LOG_DBG("IsoCigCreate");

    CHECKIF(param == NULL) {
        LOG_ERR("ParamNull");
        return -EINVAL;
    }

    CHECKIF(out_cig == NULL) {
        LOG_ERR("OutCigNull");
        return -EINVAL;
    }

    *out_cig = NULL;

    /* Check if controller is ISO capable as a central */
    if (!BT_FEAT_LE_CIS_CENTRAL(bt_dev.le.features)) {
        LOG_ERR("CisCenNotSupp");
        return -ENOTSUP;
    }

    /* TBD: Should we allow creating empty CIGs? */
    CHECKIF(param->cis_channels == NULL) {
        LOG_ERR("CisChansNull");
        return -EINVAL;
    }

    CHECKIF(param->num_cis == 0) {
        LOG_ERR("InvNumCis[%u]", param->num_cis);
        return -EINVAL;
    }

#if defined(CONFIG_BT_ISO_TEST_PARAMS)
    advanced = is_advanced_cig_param(param);
#endif /* CONFIG_BT_ISO_TEST_PARAMS */

    CHECKIF(!valid_cig_param(param, advanced, NULL)) {
        LOG_ERR("InvCigParams[%u]", advanced);
        return -EINVAL;
    }

    cig = get_free_cig();

    if (!cig) {
        return -ENOMEM;
    }

    err = cig_init_cis(cig, param);
    if (err) {
        LOG_DBG("InitCisFail[%d]", err);
        cleanup_cig(cig);
        return err;
    }

    if (!advanced) {
        rsp = hci_le_set_cig_params(cig, param);
#if defined(CONFIG_BT_ISO_TEST_PARAMS)
    } else {
        rsp = hci_le_set_cig_test_params(cig, param);
#endif /* CONFIG_BT_ISO_TEST_PARAMS */
    }

    if (rsp == NULL) {
        LOG_WRN("SetCigParamsFail");
        err = -EIO;
        cleanup_cig(cig);
        return err;
    }

    cig_rsp = (void *)rsp->data;

    if (rsp->len < sizeof(*cig_rsp) || cig_rsp->num_handles != param->num_cis) {
        LOG_WRN("InvSetCigParamsRsp[%u][%zu][%u][%u]",
                rsp->len, sizeof(*cig_rsp), cig_rsp->num_handles, param->num_cis);
        err = -EIO;
        net_buf_unref(rsp);
        cleanup_cig(cig);
        return err;
    }

    i = 0;
    SYS_SLIST_FOR_EACH_CONTAINER(&cig->cis_channels, cis, node) {
        const uint16_t handle = cig_rsp->handle[i++];

        /* Assign the connection handle */
        cis->iso->handle = sys_le16_to_cpu(handle);
    }

    net_buf_unref(rsp);

    *out_cig = cig;

    LOG_DBG("OutCig[%u]", cig->id);

    return err;
}

static void restore_cig(struct bt_iso_cig *cig, uint8_t existing_num_cis)
{
    struct bt_iso_chan *cis, *tmp;
    sys_snode_t *prev = NULL;

    LOG_DBG("RestoreCig[%u][%u]", cig->id, existing_num_cis);

    SYS_SLIST_FOR_EACH_CONTAINER_SAFE(&cig->cis_channels, cis, tmp, node) {
        /* Remove all newly added by comparing the cis_id to the number
         * of CIS that was previously added before
         * bt_iso_cig_reconfigure was called
         */
        if (cis->iso != NULL && cis->iso->iso.info.unicast.cis_id >= existing_num_cis) {
            bt_conn_unref(cis->iso);
            cis->iso = NULL;

            sys_slist_remove(&cig->cis_channels, prev, &cis->node);
            cig->num_cis--;
            /* prev stays unchanged: the removed node is gone, the next
             * iteration will process tmp which follows prev directly */
        } else {
            prev = &cis->node;
        }
    }
}

int bt_iso_cig_reconfigure(struct bt_iso_cig *cig, const struct bt_iso_cig_param *param)
{
    struct bt_hci_rp_le_set_cig_params *cig_rsp;
    uint8_t existing_num_cis;
    bool advanced = false;
    struct net_buf *rsp = NULL;
    int err;

    LOG_DBG("IsoCigReconfigure");

    CHECKIF(cig == NULL) {
        LOG_ERR("CigNull");
        return -EINVAL;
    }

    CHECKIF(param == NULL) {
        LOG_ERR("ParamNull");
        return -EINVAL;
    }

    if (cig->state != BT_ISO_CIG_STATE_CONFIGURED) {
        LOG_ERR("InvCigState[%u]", cig->state);
        return -EINVAL;
    }

#if defined(CONFIG_BT_ISO_TEST_PARAMS)
    advanced = is_advanced_cig_param(param);
#endif /* CONFIG_BT_ISO_TEST_PARAMS */

    CHECKIF(!valid_cig_param(param, advanced, cig)) {
        LOG_DBG("InvCigParams");
        return -EINVAL;
    }

    /* Used to restore CIG in case of error */
    existing_num_cis = cig->num_cis;

    err = cig_init_cis(cig, param);
    if (err != 0) {
        LOG_DBG("InitCisFail[%d]", err);
        restore_cig(cig, existing_num_cis);
        return err;
    }

    if (!advanced) {
        rsp = hci_le_set_cig_params(cig, param);
#if defined(CONFIG_BT_ISO_TEST_PARAMS)
    } else {
        rsp = hci_le_set_cig_test_params(cig, param);
#endif /* CONFIG_BT_ISO_TEST_PARAMS */
    }

    if (rsp == NULL) {
        LOG_WRN("SetCigParamsFail");
        err = -EIO;
        restore_cig(cig, existing_num_cis);
        return err;
    }

    cig_rsp = (void *)rsp->data;

    if (rsp->len < sizeof(*cig_rsp)) {
        LOG_WRN("InvSetCigParamsRspLen[%u][%zu]",
                rsp->len, sizeof(*cig_rsp));
        err = -EIO;
        net_buf_unref(rsp);
        restore_cig(cig, existing_num_cis);
        return err;
    }

    if (cig_rsp->num_handles != param->num_cis) {
        LOG_WRN("InvSetCigParamsRspNumHdl[%u][%u]",
                cig_rsp->num_handles, param->num_cis);
        err = -EIO;
        net_buf_unref(rsp);
        restore_cig(cig, existing_num_cis);
        return err;
    }

    for (uint8_t i = 0u; i < param->num_cis; i++) {
        const uint16_t handle = cig_rsp->handle[i];
        struct bt_iso_chan *cis = param->cis_channels[i];

        /* Assign the connection handle */
        cis->iso->handle = sys_le16_to_cpu(handle);
    }

    net_buf_unref(rsp);

    return err;
}

int bt_iso_cig_terminate(struct bt_iso_cig *cig)
{
    int err;

    LOG_DBG("IsoCigTerminate");

    CHECKIF(cig == NULL) {
        LOG_ERR("CigNull");
        return -EINVAL;
    }

    if (cig->state != BT_ISO_CIG_STATE_INACTIVE && cig->state != BT_ISO_CIG_STATE_CONFIGURED) {
        LOG_ERR("InvCigState[%u]", cig->state);
        return -EINVAL;
    }

    err = hci_le_remove_cig(cig->id);
    if (err != 0) {
        LOG_ERR("TerminateCigFail[%d]", err);
        return err;
    }

    cleanup_cig(cig);

    return 0;
}

static int hci_le_create_cis(const struct bt_iso_connect_param *param, size_t count)
{
    struct bt_hci_cis *cis;
    struct bt_hci_cp_le_create_cis *req;
    struct net_buf *buf;

    LOG_DBG("CreateCis[%u]", count);

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_CREATE_CIS, sizeof(*req) + sizeof(*cis) * count);
    if (!buf) {
        return -ENOBUFS;
    }

    req = net_buf_add(buf, sizeof(*req));

    memset(req, 0, sizeof(*req));

    /* Program the cis parameters */
    for (size_t i = 0; i < count; i++) {
        struct bt_iso_chan *iso_chan = param[i].iso_chan;

        if (iso_chan->state == BT_ISO_STATE_ENCRYPT_PENDING) {
            continue;
        }

        cis = net_buf_add(buf, sizeof(*cis));

        memset(cis, 0, sizeof(*cis));

        cis->cis_handle = sys_cpu_to_le16(param[i].iso_chan->iso->handle);
        cis->acl_handle = sys_cpu_to_le16(param[i].acl->handle);
        req->num_cis++;
    }

    /* If all CIS are pending for security, do nothing,
     * but return a recognizable return value
     */
    if (req->num_cis == 0) {
        net_buf_unref(buf);

        return -ECANCELED;
    }

    return bt_hci_cmd_send_sync(BT_HCI_OP_LE_CREATE_CIS, buf, NULL);
}

#if defined(CONFIG_BT_SMP)
static int iso_chan_connect_security(const struct bt_iso_connect_param *param, size_t count)
{
    /* conn_idx_handled is an array of booleans for which conn indexes
     * already have been used to call bt_conn_set_security.
     * Using indexes avoids looping the array when checking if it has been
     * handled.
     */
    bool conn_idx_handled[CONFIG_BT_MAX_CONN];

    LOG_DBG("IsoChanConnectSec[%u]", count);

    memset(conn_idx_handled, false, sizeof(conn_idx_handled));
    for (size_t i = 0; i < count; i++) {
        struct bt_iso_chan *iso_chan = param[i].iso_chan;
        struct bt_conn *acl = param[i].acl;
        uint8_t conn_idx = bt_conn_index(acl);

        LOG_DBG("SecLevel[%zu][%u][%u]",
                i, acl->sec_level, iso_chan->required_sec_level);

        if (acl->sec_level < iso_chan->required_sec_level) {
            if (!conn_idx_handled[conn_idx]) {
                int err;

                err = bt_conn_set_security(acl, iso_chan->required_sec_level);
                if (err != 0) {
                    LOG_ERR("SetSecFail[%zu][%d]", i, err);

                    /* Restore states */
                    for (size_t j = 0; j < i; j++) {
                        iso_chan = param[j].iso_chan;

                        if (iso_chan->state == BT_ISO_STATE_ENCRYPT_PENDING) {
                            bt_iso_cleanup_acl(iso_chan->iso);
                            bt_iso_chan_set_state(iso_chan,
                                                  BT_ISO_STATE_DISCONNECTED);
                        }
                    }

                    return err;
                }

                conn_idx_handled[conn_idx] = true;
            }

            iso_chan->iso->iso.acl = bt_conn_ref(acl);
            bt_iso_chan_set_state(iso_chan, BT_ISO_STATE_ENCRYPT_PENDING);
        }
    }

    return 0;
}
#endif /* CONFIG_BT_SMP */

static bool iso_chans_connecting(void)
{
    LOG_DBG("IsoChansConnecting");

    for (size_t i = 0U; i < ARRAY_SIZE(iso_conns); i++) {
        const struct bt_conn *iso = &iso_conns[i];
        const struct bt_iso_chan *iso_chan;

        if (!(iso->iso.info.type == BT_ISO_CHAN_TYPE_CENTRAL ||
              iso->iso.info.type == BT_ISO_CHAN_TYPE_PERIPHERAL)) {
            continue;
        }

        iso_chan = iso_chan(iso);
        if (iso_chan != NULL &&
            (iso_chan->state == BT_ISO_STATE_CONNECTING ||
                iso_chan->state == BT_ISO_STATE_ENCRYPT_PENDING)) {
            return true;
        }
    }

    return false;
}

int bt_iso_chan_connect(const struct bt_iso_connect_param *param, size_t count)
{
    int err;

    LOG_DBG("IsoChanConnect[%u]", count);

    CHECKIF(param == NULL) {
        LOG_ERR("ParamNull");
        return -EINVAL;
    }

    CHECKIF(count == 0) {
        LOG_ERR("InvCount[%zu]", count);
        return -EINVAL;
    }

    CHECKIF(count > CONFIG_BT_ISO_MAX_CHAN) {
        LOG_ERR("TooLargeCount[%zu][%u]", count, CONFIG_BT_ISO_MAX_CHAN);
        return -EINVAL;
    }

    /* Validate input */
    for (size_t i = 0; i < count; i++) {
        CHECKIF(param[i].iso_chan == NULL) {
            LOG_ERR("IsoNull[%zu]", i);
            return -EINVAL;
        }

        CHECKIF(param[i].acl == NULL) {
            LOG_ERR("AclNull[%zu]", i);
            return -EINVAL;
        }

        CHECKIF((param[i].acl->type & BT_CONN_TYPE_LE) == 0) {
            LOG_ERR("InvAclType[%zu][%u]", i, param[i].acl->type);
            return -EINVAL;
        }

        if (param[i].iso_chan->iso == NULL) {
            LOG_ERR("IsoNotInit[%zu]", i);
            return -EINVAL;
        }

        if (param[i].iso_chan->state != BT_ISO_STATE_DISCONNECTED) {
            LOG_ERR("IsoNotDisconnected[%zu][%u]", i, param[i].iso_chan->state);
            return -EINVAL;
        }
    }

    if (iso_chans_connecting()) {
        LOG_ERR("PendingIsoExist");
        return -EBUSY;
    }

#if defined(CONFIG_BT_SMP)
    /* Check for and initiate security for all channels that have
     * requested encryption if the ACL link is not already secured
     */
    err = iso_chan_connect_security(param, count);
    if (err != 0) {
        LOG_ERR("InitSecForCisFail[%d]", err);
        return err;
    }
#endif /* CONFIG_BT_SMP */

    err = hci_le_create_cis(param, count);
    if (err == -ECANCELED) {
        LOG_DBG("CisPendingOnSec");

        return 0;
    } else if (err != 0) {
        LOG_ERR("ConnectCisFail[%d]", err);

        return err;
    }

    /* Set connection states */
    for (size_t i = 0; i < count; i++) {
        struct bt_iso_chan *iso_chan = param[i].iso_chan;
        struct bt_iso_cig *cig;

        if (iso_chan->state == BT_ISO_STATE_ENCRYPT_PENDING) {
            continue;
        }

        iso_chan->iso->iso.acl = bt_conn_ref(param[i].acl);
        bt_conn_set_state(iso_chan->iso, BT_CONN_INITIATING);
        bt_iso_chan_set_state(iso_chan, BT_ISO_STATE_CONNECTING);

        cig = get_cig(iso_chan);
        assert(cig && "CigNull");
        cig->state = BT_ISO_CIG_STATE_ACTIVE;
    }

    return 0;
}
#endif /* CONFIG_BT_ISO_CENTRAL */
#endif /* CONFIG_BT_ISO_UNICAST */

#if defined(CONFIG_BT_ISO_BROADCAST)
static sys_slist_t iso_big_cbs = SYS_SLIST_STATIC_INIT(&iso_big_cbs);

static struct bt_iso_big *lookup_big_by_handle(uint8_t big_handle)
{
    LOG_DBG("LookupBigByHdl[%u]", big_handle);
    return &bigs[big_handle];
}

static struct bt_iso_big *get_free_big(void)
{
    /* We can use the index in the `bigs` array as BIG handles, for both
     * broadcaster and receiver (even if the device is both!)
     */

    LOG_DBG("GetFreeBig");

    for (size_t i = 0; i < ARRAY_SIZE(bigs); i++) {
        if (!atomic_test_and_set_bit(bigs[i].flags, BT_BIG_INITIALIZED)) {
            bigs[i].handle = i;
            sys_slist_init(&bigs[i].bis_channels);
            LOG_DBG("BigNew[%u]", i);
            return &bigs[i];
        }
    }

    LOG_ERR("NoFreeBig");

    return NULL;
}

static struct bt_iso_big *big_lookup_flag(int bit)
{
    LOG_DBG("BigLookupFlag[%d]", bit);

    for (size_t i = 0; i < ARRAY_SIZE(bigs); i++) {
        if (atomic_test_bit(bigs[i].flags, bit)) {
            return &bigs[i];
        }
    }

    LOG_WRN("NoBigFound[%d]", bit);

    return NULL;
}

static void cleanup_big(struct bt_iso_big *big)
{
    struct bt_iso_chan *bis, *tmp;

    LOG_DBG("CleanupBig");

    SYS_SLIST_FOR_EACH_CONTAINER_SAFE(&big->bis_channels, bis, tmp, node) {
        if (bis->iso != NULL) {
            /* Release iso conn slot back to the pool. */
            bis->iso->type = BT_CONN_TYPE_NONE;
            bt_conn_unref(bis->iso);
            bis->iso = NULL;
        }

        sys_slist_remove(&big->bis_channels, NULL, &bis->node);
    }

    memset(big, 0, sizeof(*big));
}

static void big_disconnect(struct bt_iso_big *big, uint8_t reason)
{
    struct bt_iso_chan *bis;

    LOG_DBG("BigDisconnect[%02x]", reason);

    atomic_set_bit(big->flags, BT_BIG_BUSY);

    SYS_SLIST_FOR_EACH_CONTAINER(&big->bis_channels, bis, node) {
        bis->iso->err = reason;

        bt_iso_chan_disconnected(bis, reason);
    }

    /* Cleanup the BIG before calling the `stopped` so that the `big` pointer and the ISO
     * channels in the `big` can be reused in the callback
     */
    cleanup_big(big);

    if (!sys_slist_is_empty(&iso_big_cbs)) {
        struct bt_iso_big_cb *listener;

        SYS_SLIST_FOR_EACH_CONTAINER(&iso_big_cbs, listener, _node) {
            if (listener->stopped != NULL) {
                listener->stopped(big, reason);
            }
        }
    }
}

static int big_init_bis(struct bt_iso_big *big, struct bt_iso_chan *bis, uint8_t bis_number,
                        bool broadcaster)
{
    LOG_DBG("BigInitBis[%u][%u][%u]", big->handle, bis_number, broadcaster);

    struct bt_conn_iso *iso_conn;

    bis->iso = iso_new();
    if (bis->iso == NULL) {
        LOG_ERR("NoFreeBis");
        return -ENOMEM;
    }

    iso_conn = &bis->iso->iso;

#if defined(CONFIG_BT_ISO_BROADCASTER)
    if (broadcaster) {
        iso_conn->info.type = BT_ISO_CHAN_TYPE_BROADCASTER;
        iso_conn->info.broadcaster.big_handle = big->handle;
        iso_conn->info.broadcaster.bis_number = bis_number;
    }
#endif /* CONFIG_BT_ISO_BROADCASTER */
#if defined(CONFIG_BT_ISO_SYNC_RECEIVER)
    if (!broadcaster) {
        iso_conn->info.type = BT_ISO_CHAN_TYPE_SYNC_RECEIVER;
        iso_conn->info.sync_receiver.big_handle = big->handle;
        iso_conn->info.sync_receiver.bis_number = bis_number;
    }
#endif /* CONFIG_BT_ISO_SYNC_RECEIVER */

    bt_iso_chan_add(bis->iso, bis);

    sys_slist_append(&big->bis_channels, &bis->node);

    return 0;
}

int bt_iso_big_register_cb(struct bt_iso_big_cb *cb)
{
    CHECKIF(cb == NULL) {
        LOG_DBG("BigCbNull");

        return -EINVAL;
    }

    if (sys_slist_find(&iso_big_cbs, &cb->_node, NULL)) {
        LOG_DBG("BigCbAlreadyReg[%p]", cb);

        return -EEXIST;
    }

    sys_slist_append(&iso_big_cbs, &cb->_node);

    return 0;
}

#if defined(CONFIG_BT_ISO_BROADCASTER)
static int hci_le_create_big(struct bt_le_ext_adv *padv, struct bt_iso_big *big,
                             struct bt_iso_big_create_param *param)
{
    const struct bt_iso_chan_io_qos *qos;
    struct bt_hci_cp_le_create_big *req;
    struct net_buf *buf;
    int err;
    struct bt_iso_chan *bis;

    LOG_DBG("CreateBig");

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_CREATE_BIG, sizeof(*req));

    if (!buf) {
        return -ENOBUFS;
    }

    bis = SYS_SLIST_PEEK_HEAD_CONTAINER(&big->bis_channels, bis, node);
    assert(bis != NULL && "BisNull");

    /* All BIS will share the same QOS */
    qos = bis->qos->tx;

    req = net_buf_add(buf, sizeof(*req));
    req->big_handle = big->handle;
    req->adv_handle = padv->handle;
    req->num_bis = big->num_bis;
    sys_put_le24(param->interval, req->sdu_interval);
    req->max_sdu = sys_cpu_to_le16(qos->sdu);
    req->max_latency = sys_cpu_to_le16(param->latency);
    req->rtn = qos->rtn;
    req->phy = qos->phy;
    req->packing = param->packing;
    req->framing = param->framing;
    req->encryption = param->encryption;
    if (req->encryption) {
        memcpy(req->bcode, param->bcode, sizeof(req->bcode));
    } else {
        memset(req->bcode, 0, sizeof(req->bcode));
    }

    LOG_DBG("[%u][%u][%u][%u][%u][%u][%u][%u][%u][%u][%u]",
            req->big_handle, req->adv_handle, req->num_bis, param->interval,
            req->max_sdu, req->max_latency, req->rtn, req->phy, req->packing,
            req->framing, req->encryption);
    if (req->encryption) {
        LOG_DBG("[%s]", bt_hex(req->bcode, sizeof(req->bcode)));
    }

    err = bt_hci_cmd_send_sync(BT_HCI_OP_LE_CREATE_BIG, buf, NULL);

    if (err) {
        return err;
    }

    SYS_SLIST_FOR_EACH_CONTAINER(&big->bis_channels, bis, node) {
        bt_iso_chan_set_state(bis, BT_ISO_STATE_CONNECTING);
    }

    return err;
}

#if defined(CONFIG_BT_ISO_TEST_PARAMS)
static int hci_le_create_big_test(const struct bt_le_ext_adv *padv, struct bt_iso_big *big,
                                  const struct bt_iso_big_create_param *param)
{
    struct bt_hci_cp_le_create_big_test *req;
    const struct bt_iso_chan_qos *qos;
    struct bt_iso_chan *bis;
    struct net_buf *buf;
    int err;

    LOG_DBG("CreateBigTest");

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_CREATE_BIG_TEST, sizeof(*req));

    if (!buf) {
        return -ENOBUFS;
    }

    bis = SYS_SLIST_PEEK_HEAD_CONTAINER(&big->bis_channels, bis, node);
    assert(bis != NULL && "BisNull");

    /* All BIS will share the same QOS */
    qos = bis->qos;

    req = net_buf_add(buf, sizeof(*req));
    req->big_handle = big->handle;
    req->adv_handle = padv->handle;
    req->num_bis = big->num_bis;
    sys_put_le24(param->interval, req->sdu_interval);
    req->iso_interval = sys_cpu_to_le16(param->iso_interval);
    req->nse = qos->num_subevents;
    req->max_sdu = sys_cpu_to_le16(qos->tx->sdu);
    req->max_pdu = sys_cpu_to_le16(qos->tx->max_pdu);
    req->phy = qos->tx->phy;
    req->packing = param->packing;
    req->framing = param->framing;
    req->bn = qos->tx->burst_number;
    req->irc = param->irc;
    req->pto = param->pto;
    req->encryption = param->encryption;
    if (req->encryption) {
        memcpy(req->bcode, param->bcode, sizeof(req->bcode));
    } else {
        memset(req->bcode, 0, sizeof(req->bcode));
    }

    LOG_DBG("[%u][%u][%u][%u][%u][%u][%u][%u][%u][%u][%u][%u][%u][%u][%u]",
            req->big_handle, req->adv_handle, req->num_bis, param->interval,
            param->iso_interval, req->nse, req->max_sdu, req->max_pdu,
            req->phy, req->packing, req->framing, req->bn, req->irc,
            req->pto, req->encryption);
    if (req->encryption) {
        LOG_DBG("[%s]", bt_hex(req->bcode, sizeof(req->bcode)));
    }

    err = bt_hci_cmd_send_sync(BT_HCI_OP_LE_CREATE_BIG_TEST, buf, NULL);
    if (err) {
        return err;
    }

    SYS_SLIST_FOR_EACH_CONTAINER(&big->bis_channels, bis, node) {
        bt_iso_chan_set_state(bis, BT_ISO_STATE_CONNECTING);
    }

    return err;
}

static bool is_advanced_big_param(const struct bt_iso_big_create_param *param)
{
    LOG_DBG("IsAdvancedBigParam[%u][%u]", param->irc, param->iso_interval);

    if (param->irc != 0U || param->iso_interval != 0U) {
        return true;
    }

    /* Check if any of the CIS contain any test-param-only values */
    for (uint8_t i = 0U; i < param->num_bis; i++) {
        const struct bt_iso_chan *bis = param->bis_channels[i];
        const struct bt_iso_chan_qos *qos = bis->qos;

        if (qos->num_subevents > 0U) {
            return true;
        }

        assert(qos->tx != NULL && "BigWithNullTx");

        if (qos->tx->max_pdu > 0U || qos->tx->burst_number > 0U) {
            return true;
        }
    }

    return false;
}
#endif /* CONFIG_BT_ISO_TEST_PARAMS */

static bool valid_big_param(const struct bt_iso_big_create_param *param, bool advanced)
{
    CHECKIF(!param->bis_channels) {
        LOG_ERR("BisChansNull");

        return false;
    }

    CHECKIF(!param->num_bis) {
        LOG_ERR("InvNumBis[%u]", param->num_bis);

        return false;
    }

    for (uint8_t i = 0; i < param->num_bis; i++) {
        struct bt_iso_chan *bis = param->bis_channels[i];

        CHECKIF(bis == NULL) {
            LOG_ERR("BisChanNull[%u]", i);

            return false;
        }

        if (bis->iso) {
            LOG_WRN("BisChanExist[%u]", i);

            return false;
        }

        CHECKIF(bis->qos == NULL) {
            LOG_ERR("BisChanQosNull[%u]", i);

            return false;
        }

        CHECKIF(bis->qos->tx == NULL ||
                !valid_chan_io_qos(bis->qos->tx, true, true, advanced)) {
            LOG_ERR("InvBisChanQos[%u]", i);

            return false;
        }
    }

    CHECKIF(param->framing != BT_ISO_FRAMING_UNFRAMED &&
            param->framing != BT_ISO_FRAMING_FRAMED) {
        LOG_ERR("InvFraming[%u]", param->framing);

        return false;
    }

    CHECKIF(param->packing != BT_ISO_PACKING_SEQUENTIAL &&
            param->packing != BT_ISO_PACKING_INTERLEAVED) {
        LOG_ERR("InvPacking[%u]", param->packing);

        return false;
    }

    CHECKIF(param->num_bis > BT_ISO_MAX_GROUP_ISO_COUNT ||
            param->num_bis > CONFIG_BT_ISO_MAX_CHAN) {
        LOG_ERR("TooLargeNumBis[%u][%u]",
                param->num_bis, MAX(CONFIG_BT_ISO_MAX_CHAN, BT_ISO_MAX_GROUP_ISO_COUNT));

        return false;
    }

    CHECKIF(!IN_RANGE(param->interval, BT_ISO_SDU_INTERVAL_MIN, BT_ISO_SDU_INTERVAL_MAX)) {
        LOG_ERR("InvInterval[%u]", param->interval);

        return false;
    }

    CHECKIF(!advanced && !IN_RANGE(param->latency, BT_ISO_LATENCY_MIN, BT_ISO_LATENCY_MAX)) {
        LOG_ERR("InvLatency[%u]", param->latency);

        return false;
    }

#if defined(CONFIG_BT_ISO_TEST_PARAMS)
    if (advanced) {
        CHECKIF(!IN_RANGE(param->irc, BT_ISO_IRC_MIN, BT_ISO_IRC_MAX)) {
            LOG_ERR("InvIrc[%u]", param->irc);

            return false;
        }

        CHECKIF(!IN_RANGE(param->pto, BT_ISO_PTO_MIN, BT_ISO_PTO_MAX)) {
            LOG_ERR("InvPto[%u]", param->pto);

            return false;
        }

        CHECKIF(!IN_RANGE(param->iso_interval, BT_ISO_ISO_INTERVAL_MIN,
                          BT_ISO_ISO_INTERVAL_MAX)) {
            LOG_ERR("InvIsoInterval[%u]", param->iso_interval);

            return false;
        }
    }
#endif /* CONFIG_BT_ISO_TEST_PARAMS */

    return true;
}

int bt_iso_big_create(struct bt_le_ext_adv *padv, struct bt_iso_big_create_param *param,
                      struct bt_iso_big **out_big)
{
    struct bt_iso_chan **bis_channels;
    int err;
    struct bt_iso_big *big;
    bool advanced = false;

    LOG_DBG("IsoBigCreate");

    CHECKIF(padv == NULL) {
        LOG_ERR("PaNull");
        return -EINVAL;
    }

    CHECKIF(param == NULL) {
        LOG_ERR("ParamNull");
        return -EINVAL;
    }

    CHECKIF(out_big == NULL) {
        LOG_ERR("OutBigNull");
        return -EINVAL;
    }

    if (!atomic_test_bit(padv->flags, BT_PER_ADV_PARAMS_SET)) {
        LOG_ERR("PaParamsNotSet");
        return -EINVAL;
    }

#if defined(CONFIG_BT_ISO_TEST_PARAMS)
    advanced = is_advanced_big_param(param);
#endif /* CONFIG_BT_ISO_TEST_PARAMS */

    if (!valid_big_param(param, advanced)) {
        LOG_ERR("InvBigParams[%u]", advanced);
        return -EINVAL;
    }

    big = get_free_big();

    if (!big) {
        return -ENOMEM;
    }

    bis_channels = param->bis_channels;
    for (uint8_t i = 0; i < param->num_bis; i++) {
        /* BIS numbers start from 1, so the first BIS will get BIS Number = 1 */
        const uint8_t bis_number = i + 1U;

        err = big_init_bis(big, bis_channels[i], bis_number, true);
        if (err != 0) {
            LOG_DBG("InitBisFail[%u][%d]", i, err);
            cleanup_big(big);

            return err;
        }
    }

    big->num_bis = param->num_bis;

    if (!advanced) {
        err = hci_le_create_big(padv, big, param);
#if defined(CONFIG_BT_ISO_TEST_PARAMS)
    } else {
        err = hci_le_create_big_test(padv, big, param);
#endif /* CONFIG_BT_ISO_TEST_PARAMS */
    }

    if (err) {
        LOG_ERR("CreateBigFail[%d]", err);
        cleanup_big(big);
        return err;
    }

    *out_big = big;

    LOG_DBG("OutBig[%u]", big->handle);

    return err;
}

static void store_bis_broadcaster_info(const struct bt_hci_evt_le_big_complete *evt,
                                       struct bt_conn *iso)
{
    struct bt_conn_iso *iso_conn = &iso->iso;
    struct bt_iso_info *info = &iso_conn->info;
    struct bt_iso_broadcaster_info *broadcaster_info = &info->broadcaster;

    info->iso_interval = sys_le16_to_cpu(evt->iso_interval);
    info->max_subevent = evt->nse;

    broadcaster_info->sync_delay = sys_get_le24(evt->sync_delay);
    broadcaster_info->latency = sys_get_le24(evt->latency);
    broadcaster_info->phy = bt_get_phy(evt->phy);
    broadcaster_info->bn = evt->bn;
    broadcaster_info->irc = evt->irc;
    /* Transform to n * 1.25ms */
    broadcaster_info->pto = info->iso_interval * evt->pto;
    broadcaster_info->max_pdu = sys_le16_to_cpu(evt->max_pdu);
    broadcaster_info->big_handle = iso_conn->info.broadcaster.big_handle;
    broadcaster_info->bis_number = iso_conn->info.broadcaster.bis_number;

    info->can_send = true;
    info->can_recv = false;

    LOG_DBG("StoreBisInfo[%u]", broadcaster_info->pto);
}

void hci_le_big_complete(struct net_buf *buf)
{
    struct bt_hci_evt_le_big_complete *evt = (void *)buf->data;
    struct bt_iso_chan *bis;
    struct bt_iso_big *big;
    int i;

    LOG_DBG("BigComp");
    LOG_DBG("Evt[%02x][%u][%u][%u][%u][%u][%u][%u][%u][%u][%u][%u]",
            evt->status, evt->big_handle, sys_get_le24(evt->sync_delay),
            sys_get_le24(evt->latency), evt->phy, evt->nse, evt->bn,
            evt->pto, evt->irc, evt->max_pdu, evt->iso_interval,
            evt->num_bis);

    if (evt->big_handle >= ARRAY_SIZE(bigs)) {
        LOG_ERR("InvBigHdl[%u][%u]", evt->big_handle, ARRAY_SIZE(bigs));

        big = big_lookup_flag(BT_BIG_PENDING);
        if (big) {
            big_disconnect(big, evt->status ? evt->status : BT_HCI_ERR_UNSPECIFIED);
        }

        return;
    }

    big = lookup_big_by_handle(evt->big_handle);
    atomic_clear_bit(big->flags, BT_BIG_PENDING);

    LOG_DBG("BigComp[%u][%p][%02x][%s]", big->handle, big, evt->status,
            bt_hci_err_to_str(evt->status));

    if (evt->status || evt->num_bis != big->num_bis) {
        if (evt->status == BT_HCI_ERR_SUCCESS && evt->num_bis != big->num_bis) {
            LOG_ERR("InvNumOfBisCreated[%u][%u]", evt->num_bis, big->num_bis);
        }
        big_disconnect(big, evt->status ? evt->status : BT_HCI_ERR_UNSPECIFIED);
        return;
    }

    atomic_set_bit(big->flags, BT_BIG_BUSY);
    i = 0;
    SYS_SLIST_FOR_EACH_CONTAINER(&big->bis_channels, bis, node) {
        const uint16_t handle = evt->handle[i++];
        struct bt_conn *iso_conn = bis->iso;

        LOG_DBG("BisHdl[%04x]", handle);

        iso_conn->handle = sys_le16_to_cpu(handle);
        store_bis_broadcaster_info(evt, iso_conn);
        bt_conn_set_state(iso_conn, BT_CONN_CONNECTED);
    }

    atomic_clear_bit(big->flags, BT_BIG_BUSY);

    if (!sys_slist_is_empty(&iso_big_cbs)) {
        struct bt_iso_big_cb *listener;

        SYS_SLIST_FOR_EACH_CONTAINER(&iso_big_cbs, listener, _node) {
            if (listener->started != NULL) {
                listener->started(big);
            }
        }
    }
}

void hci_le_big_terminate(struct net_buf *buf)
{
    struct bt_hci_evt_le_big_terminate *evt = (void *)buf->data;
    struct bt_iso_big *big;

    LOG_DBG("BigTerminateEvt[%u][%02x]", evt->big_handle, evt->reason);

    if (evt->big_handle >= ARRAY_SIZE(bigs)) {
        LOG_ERR("InvBigHdl[%u][%u]", evt->big_handle, ARRAY_SIZE(bigs));
        return;
    }

    big = lookup_big_by_handle(evt->big_handle);

    if (!atomic_test_bit(big->flags, BT_BIG_INITIALIZED)) {
        LOG_ERR("BigNotInit[%u]", evt->big_handle);
        return;
    }

    LOG_DBG("BigTerminated[%u][%p]", big->handle, big);

    big_disconnect(big, evt->reason);
}
#endif /* CONFIG_BT_ISO_BROADCASTER */

static int hci_le_terminate_big(struct bt_iso_big *big)
{
    struct bt_hci_cp_le_terminate_big *req;
    struct net_buf *buf;

    LOG_DBG("TerminateBig[%u]", big->handle);

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_TERMINATE_BIG, sizeof(*req));
    if (!buf) {
        return -ENOBUFS;
    }

    req = net_buf_add(buf, sizeof(*req));
    req->big_handle = big->handle;
    req->reason = BT_HCI_ERR_REMOTE_USER_TERM_CONN;

    return bt_hci_cmd_send_sync(BT_HCI_OP_LE_TERMINATE_BIG, buf, NULL);
}

static int hci_le_big_sync_term(struct bt_iso_big *big)
{
    struct bt_hci_cp_le_big_terminate_sync *req;
    struct bt_hci_rp_le_big_terminate_sync *evt;
    struct net_buf *buf;
    struct net_buf *rsp;
    int err;

    LOG_DBG("BigSyncTerm[%u]", big->handle);

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_BIG_TERMINATE_SYNC, sizeof(*req));
    if (!buf) {
        return -ENOBUFS;
    }

    req = net_buf_add(buf, sizeof(*req));
    req->big_handle = big->handle;
    err = bt_hci_cmd_send_sync(BT_HCI_OP_LE_BIG_TERMINATE_SYNC, buf, &rsp);
    if (err) {
        return err;
    }

    evt = (struct bt_hci_rp_le_big_terminate_sync *)rsp->data;
    if (evt->status || (evt->big_handle != big->handle)) {
        LOG_ERR("BigSyncTermFail[%02x][%u][%u]",
                evt->status, evt->big_handle, big->handle);
        err = -EIO;
    }

    net_buf_unref(rsp);

    return err;
}

int bt_iso_big_terminate(struct bt_iso_big *big)
{
    struct bt_iso_chan *bis;
    int err;

    LOG_DBG("IsoBigTerminate");

    CHECKIF(big == NULL) {
        LOG_ERR("BigNull");
        return -EINVAL;
    }

    if (!atomic_test_bit(big->flags, BT_BIG_INITIALIZED) || !big->num_bis) {
        LOG_ERR("BigNotInit[%u]", big->num_bis);
        return -EINVAL;
    }

    if (atomic_test_bit(big->flags, BT_BIG_BUSY)) {
        LOG_DBG("BigBusy[%u]", big->handle);
        return -EBUSY;
    }

    bis = SYS_SLIST_PEEK_HEAD_CONTAINER(&big->bis_channels, bis, node);
    assert(bis != NULL && "BisNull");

    if (IS_ENABLED(CONFIG_BT_ISO_BROADCASTER) &&
            bis->iso->iso.info.type == BT_ISO_CHAN_TYPE_BROADCASTER) {
        err = hci_le_terminate_big(big);

        /* Wait for BT_HCI_EVT_LE_BIG_TERMINATE before cleaning up
         * the BIG in hci_le_big_terminate
         */
        if (!err) {
            SYS_SLIST_FOR_EACH_CONTAINER(&big->bis_channels, bis, node) {
                bt_iso_chan_set_state(bis, BT_ISO_STATE_DISCONNECTING);
            }
        }
    } else if (IS_ENABLED(CONFIG_BT_ISO_SYNC_RECEIVER) &&
               bis->iso->iso.info.type == BT_ISO_CHAN_TYPE_SYNC_RECEIVER) {
        err = hci_le_big_sync_term(big);

        if (!err) {
            big_disconnect(big, BT_HCI_ERR_LOCALHOST_TERM_CONN);
        }
    } else {
        err = -EINVAL;
    }

    if (err) {
        LOG_ERR("TerminateBigFail[%d]", err);
    }

    return err;
}

#if defined(CONFIG_BT_ISO_SYNC_RECEIVER)
static void store_bis_sync_receiver_info(const struct bt_hci_evt_le_big_sync_established *evt,
                                         struct bt_conn *iso)
{
    struct bt_conn_iso *iso_conn = &iso->iso;
    struct bt_iso_info *info = &iso_conn->info;
    struct bt_iso_sync_receiver_info *receiver_info = &info->sync_receiver;

    info->max_subevent = evt->nse;
    info->iso_interval = sys_le16_to_cpu(evt->iso_interval);

    receiver_info->latency = sys_get_le24(evt->latency);
    receiver_info->bn = evt->bn;
    receiver_info->irc = evt->irc;
    /* Transform to n * 1.25ms */
    receiver_info->pto = info->iso_interval * evt->pto;
    receiver_info->max_pdu = sys_le16_to_cpu(evt->max_pdu);
    receiver_info->big_handle = iso_conn->info.sync_receiver.big_handle;
    receiver_info->bis_number = iso_conn->info.sync_receiver.bis_number;

    info->can_send = false;
    info->can_recv = true;

    LOG_DBG("StoreBisInfo[%u]", receiver_info->pto);
}

void hci_le_big_sync_established(struct net_buf *buf)
{
    struct bt_hci_evt_le_big_sync_established *evt = (void *)buf->data;
    struct bt_iso_chan *bis;
    struct bt_iso_big *big;
    int i;

    LOG_DBG("BigSyncEstab");
    LOG_DBG("Evt[%02x][%u][%u][%u][%u][%u][%u][%u][%u][%u]",
            evt->status, evt->big_handle, sys_get_le24(evt->latency),
            evt->nse, evt->bn, evt->pto, evt->irc, evt->max_pdu,
            evt->iso_interval, evt->num_bis);

    if (evt->big_handle >= ARRAY_SIZE(bigs)) {
        LOG_ERR("InvBigHdl[%u][%u]", evt->big_handle, ARRAY_SIZE(bigs));

        big = big_lookup_flag(BT_BIG_SYNCING);
        if (big) {
            big_disconnect(big, evt->status ? evt->status : BT_HCI_ERR_UNSPECIFIED);
        }

        return;
    }

    big = lookup_big_by_handle(evt->big_handle);
    atomic_clear_bit(big->flags, BT_BIG_SYNCING);

    LOG_DBG("BigSyncEstab[%u][%p][%02x][%s]", big->handle, big, evt->status,
            bt_hci_err_to_str(evt->status));

    if (evt->status || evt->num_bis != big->num_bis) {
        if (evt->status == BT_HCI_ERR_SUCCESS && evt->num_bis != big->num_bis) {
            LOG_ERR("InvNumOfBisSynced[%u][%u]", evt->num_bis, big->num_bis);
        }
        big_disconnect(big, evt->status ? evt->status : BT_HCI_ERR_UNSPECIFIED);
        return;
    }

    atomic_set_bit(big->flags, BT_BIG_BUSY);
    i = 0;
    SYS_SLIST_FOR_EACH_CONTAINER(&big->bis_channels, bis, node) {
        const uint16_t handle = evt->handle[i++];
        struct bt_conn *iso_conn = bis->iso;

        LOG_DBG("BisHdl[%04x]", handle);

        iso_conn->handle = sys_le16_to_cpu(handle);
        store_bis_sync_receiver_info(evt, iso_conn);
        bt_conn_set_state(iso_conn, BT_CONN_CONNECTED);
    }

    atomic_clear_bit(big->flags, BT_BIG_BUSY);

    if (!sys_slist_is_empty(&iso_big_cbs)) {
        struct bt_iso_big_cb *listener;

        SYS_SLIST_FOR_EACH_CONTAINER(&iso_big_cbs, listener, _node) {
            if (listener->started != NULL) {
                listener->started(big);
            }
        }
    }
}

void hci_le_big_sync_lost(struct net_buf *buf)
{
    struct bt_hci_evt_le_big_sync_lost *evt = (void *)buf->data;
    struct bt_iso_big *big;

    LOG_DBG("BigSyncLostEvt[%u][%02x]", evt->big_handle, evt->reason);

    if (evt->big_handle >= ARRAY_SIZE(bigs)) {
        LOG_ERR("InvBigHdl[%u][%u]", evt->big_handle, ARRAY_SIZE(bigs));
        return;
    }

    big = lookup_big_by_handle(evt->big_handle);

    LOG_DBG("BigSyncLost[%u][%p]", big->handle, big);

    big_disconnect(big, evt->reason);
}

static int hci_le_big_create_sync(const struct bt_le_per_adv_sync *sync, struct bt_iso_big *big,
                                  const struct bt_iso_big_sync_param *param)
{
    struct bt_hci_cp_le_big_create_sync *req;
    struct net_buf *buf;
    int err;
    uint8_t bit_idx = 0;

    LOG_DBG("BigCreateSync[%u][%u]", big->handle, big->num_bis);

    buf = bt_hci_cmd_create(BT_HCI_OP_LE_BIG_CREATE_SYNC, sizeof(*req) + big->num_bis);
    if (!buf) {
        return -ENOBUFS;
    }

    req = net_buf_add(buf, sizeof(*req) + big->num_bis);
    req->big_handle = big->handle;
    req->sync_handle = sys_cpu_to_le16(sync->handle);
    req->encryption = param->encryption;
    if (req->encryption) {
        memcpy(req->bcode, param->bcode, sizeof(req->bcode));
    } else {
        memset(req->bcode, 0, sizeof(req->bcode));
    }
    req->mse = param->mse;
    req->sync_timeout = sys_cpu_to_le16(param->sync_timeout);
    req->num_bis = big->num_bis;
    /* Transform from bitfield to array */
    for (int i = 1; i <= BT_ISO_MAX_GROUP_ISO_COUNT; i++) {
        if (param->bis_bitfield & BT_ISO_BIS_INDEX_BIT(i)) {
            if (bit_idx == big->num_bis) {
                LOG_ERR("BigCannotContainBis[%u]", bit_idx + 1);
                net_buf_unref(buf);
                return -EINVAL;
            }
            req->bis[bit_idx++] = i;
        }
    }

    if (bit_idx != big->num_bis) {
        LOG_ERR("BitfieldNotMatchNumBis[%u][%u]", bit_idx, big->num_bis);
        net_buf_unref(buf);
        return -EINVAL;
    }

    err = bt_hci_cmd_send_sync(BT_HCI_OP_LE_BIG_CREATE_SYNC, buf, NULL);

    return err;
}

int bt_iso_big_sync(struct bt_le_per_adv_sync *sync, struct bt_iso_big_sync_param *param,
                    struct bt_iso_big **out_big)
{
    struct bt_iso_chan **bis_channels;
    int err;
    struct bt_iso_chan *bis;
    struct bt_iso_big *big;
    uint32_t bitfield_copy;

    LOG_DBG("IsoBigSync");

    CHECKIF(sync == NULL) {
        LOG_ERR("SyncNull");
        return -EINVAL;
    }

    CHECKIF(param == NULL) {
        LOG_ERR("ParamNull");
        return -EINVAL;
    }

    CHECKIF(out_big == NULL) {
        LOG_ERR("OutBigNull");
        return -EINVAL;
    }

    if (!atomic_test_bit(sync->flags, BT_PER_ADV_SYNC_SYNCED)) {
        LOG_ERR("PaNotSynced");
        return -EINVAL;
    }

    CHECKIF(param->mse > BT_ISO_SYNC_MSE_MAX) {
        LOG_ERR("InvMse[%02x]", param->mse);
        return -EINVAL;
    }

    CHECKIF(param->sync_timeout < BT_ISO_SYNC_TIMEOUT_MIN ||
            param->sync_timeout > BT_ISO_SYNC_TIMEOUT_MAX) {
        LOG_ERR("InvSyncTimeout[%04x]", param->sync_timeout);
        return -EINVAL;
    }

    CHECKIF(!BT_ISO_VALID_BIS_BITFIELD(param->bis_bitfield)) {
        LOG_ERR("InvBisBitfield[%08x]", param->bis_bitfield);
        return -EINVAL;
    }

    CHECKIF(!param->bis_channels) {
        LOG_ERR("BisChansNull");
        return -EINVAL;
    }

    CHECKIF(!param->num_bis) {
        LOG_ERR("InvNumBis[%u]", param->num_bis);
        return -EINVAL;
    }

    for (uint8_t i = 0; i < param->num_bis; i++) {
        struct bt_iso_chan *param_bis = param->bis_channels[i];

        CHECKIF(param_bis == NULL) {
            LOG_ERR("BisChanNull[%u]", i);
            return -EINVAL;
        }

        if (param_bis->iso) {
            LOG_WRN("BisChanExist[%u]", i);
            return -EALREADY;
        }

        CHECKIF(param_bis->qos == NULL) {
            LOG_ERR("BisChanQosNull[%u]", i);
            return -EINVAL;
        }

        CHECKIF(param_bis->qos->rx == NULL) {
            LOG_ERR("BisChanQosRxNull[%u]", i);
            return -EINVAL;
        }
    }

    big = get_free_big();

    if (!big) {
        return -ENOMEM;
    }

    bitfield_copy = param->bis_bitfield;
    bis_channels = param->bis_channels;
    for (uint8_t i = 0; i < param->num_bis; i++) {
        const uint8_t bis_number = find_lsb_set(bitfield_copy);

        if (bis_number == 0) {
            LOG_ERR("BitfieldExhausted[%u][%u]", i, param->num_bis);
            cleanup_big(big);
            return -EINVAL;
        }

        /* pop the least significant bit from the bitfield */
        bitfield_copy &= ~BIT(bis_number - 1U);

        /* The least significant bit will be the bis_number, so that if the bitfield is
         * 0b0101 then the first pop will be 1 and the bitfield_copy will be 0b0100 and
         * then the second pop will be 3 and the bitfield_copy will be 0b0000
         * Since find_lsb_set returns the index starting from 1 we can use the result
         * directly as the bis_numbers also start from 1.
         */
        err = big_init_bis(big, bis_channels[i], bis_number, false);
        if (err != 0) {
            LOG_DBG("InitBisFail[%u]: %d", i, err);
            cleanup_big(big);

            return err;
        }
    }

    big->num_bis = param->num_bis;

    err = hci_le_big_create_sync(sync, big, param);
    if (err) {
        LOG_DBG("CreateBigSyncFail[%d]", err);
        cleanup_big(big);
        return err;
    }

    SYS_SLIST_FOR_EACH_CONTAINER(&big->bis_channels, bis, node) {
        bt_iso_chan_set_state(bis, BT_ISO_STATE_CONNECTING);
    }

    *out_big = big;

    LOG_DBG("OutBig[%u]", big->handle);

    return 0;
}
#endif /* CONFIG_BT_ISO_SYNC_RECEIVER */
#endif /* CONFIG_BT_ISO_BROADCAST */

void bt_iso_reset(void)
{
    LOG_DBG("IsoReset");

#if defined(CONFIG_BT_ISO_CENTRAL)
    for (size_t i = 0U; i < ARRAY_SIZE(cigs); i++) {
        struct bt_iso_cig *cig = &cigs[i];
        struct bt_iso_chan *cis;

        /* Disconnect any connected CIS and call the callback
         * We cannot use bt_iso_chan_disconnected directly here, as that
         * attempts to also remove the ISO data path, which we shouldn't attempt to
         * during the reset, as that sends HCI commands.
         */
        SYS_SLIST_FOR_EACH_CONTAINER(&cig->cis_channels, cis, node) {
            if (cis->state != BT_ISO_STATE_DISCONNECTED) {
                bt_iso_chan_set_state(cis, BT_ISO_STATE_DISCONNECTED);
                bt_iso_cleanup_acl(cis->iso);
                if (cis->ops != NULL && cis->ops->disconnected != NULL) {
                    cis->ops->disconnected(cis, BT_HCI_ERR_UNSPECIFIED);
                }
            }
        }

        cleanup_cig(cig);
    }
#endif /* CONFIG_BT_ISO_CENTRAL */

#if defined(CONFIG_BT_ISO_BROADCAST)
    for (size_t i = 0U; i < ARRAY_SIZE(bigs); i++) {
        struct bt_iso_big *big = &bigs[i];

        big_disconnect(big, BT_HCI_ERR_UNSPECIFIED);
    }
#endif /* CONFIG_BT_ISO_BROADCAST */
}

/************************ Thread-safe wrapper functions ************************/

int bt_iso_setup_data_path_safe(const struct bt_iso_chan *chan, uint8_t dir,
                                const struct bt_iso_chan_path *path)
{
    int err;
    bt_le_host_lock();
    err = bt_iso_setup_data_path(chan, dir, path);
    bt_le_host_unlock();
    return err;
}

int bt_iso_remove_data_path_safe(const struct bt_iso_chan *chan, uint8_t dir)
{
    int err;
    bt_le_host_lock();
    err = bt_iso_remove_data_path(chan, dir);
    bt_le_host_unlock();
    return err;
}

int bt_iso_chan_get_info_safe(const struct bt_iso_chan *chan, struct bt_iso_info *info)
{
    int err;
    bt_le_host_lock();
    err = bt_iso_chan_get_info(chan, info);
    bt_le_host_unlock();
    return err;
}

#if CONFIG_BT_ISO_TX
int bt_iso_chan_send_safe(struct bt_iso_chan *chan, struct net_buf *buf, uint16_t seq_num)
{
    int err;
    bt_le_host_lock();
    err = bt_iso_chan_send(chan, buf, seq_num);
    bt_le_host_unlock();
    return err;
}

int bt_iso_chan_send_ts_safe(struct bt_iso_chan *chan, struct net_buf *buf, uint16_t seq_num,
                             uint32_t ts)
{
    int err;
    bt_le_host_lock();
    err = bt_iso_chan_send_ts(chan, buf, seq_num, ts);
    bt_le_host_unlock();
    return err;
}

int bt_iso_chan_get_tx_sync_safe(const struct bt_iso_chan *chan, struct bt_iso_tx_info *info)
{
    int err;
    bt_le_host_lock();
    err = bt_iso_chan_get_tx_sync(chan, info);
    bt_le_host_unlock();
    return err;
}
#endif /* CONFIG_BT_ISO_TX */

#if CONFIG_BT_ISO_UNICAST
int bt_iso_chan_disconnect_safe(struct bt_iso_chan *chan)
{
    int err;
    bt_le_host_lock();
    err = bt_iso_chan_disconnect(chan);
    bt_le_host_unlock();
    return err;
}

#if CONFIG_BT_ISO_PERIPHERAL
int bt_iso_server_register_safe(struct bt_iso_server *server)
{
    int err;
    bt_le_host_lock();
    err = bt_iso_server_register(server);
    bt_le_host_unlock();
    return err;
}

int bt_iso_server_unregister_safe(struct bt_iso_server *server)
{
    int err;
    bt_le_host_lock();
    err = bt_iso_server_unregister(server);
    bt_le_host_unlock();
    return err;
}
#endif /* CONFIG_BT_ISO_PERIPHERAL */

#if CONFIG_BT_ISO_CENTRAL
int bt_iso_cig_create_safe(const struct bt_iso_cig_param *param, struct bt_iso_cig **out_cig)
{
    int err;
    bt_le_host_lock();
    err = bt_iso_cig_create(param, out_cig);
    bt_le_host_unlock();
    return err;
}

int bt_iso_cig_reconfigure_safe(struct bt_iso_cig *cig, const struct bt_iso_cig_param *param)
{
    int err;
    bt_le_host_lock();
    err = bt_iso_cig_reconfigure(cig, param);
    bt_le_host_unlock();
    return err;
}

int bt_iso_cig_terminate_safe(struct bt_iso_cig *cig)
{
    int err;
    bt_le_host_lock();
    err = bt_iso_cig_terminate(cig);
    bt_le_host_unlock();
    return err;
}

int bt_iso_chan_connect_safe(const struct bt_iso_connect_param *param, size_t count)
{
    int err;
    bt_le_host_lock();
    err = bt_iso_chan_connect(param, count);
    bt_le_host_unlock();
    return err;
}
#endif /* CONFIG_BT_ISO_CENTRAL */
#endif /* CONFIG_BT_ISO_UNICAST */

#if CONFIG_BT_ISO_BROADCAST
#if CONFIG_BT_ISO_BROADCASTER
int bt_iso_big_register_cb_safe(struct bt_iso_big_cb *cb)
{
    int err;
    bt_le_host_lock();
    err = bt_iso_big_register_cb(cb);
    bt_le_host_unlock();
    return err;
}

int bt_iso_big_create_safe(struct bt_le_ext_adv *padv, struct bt_iso_big_create_param *param,
                           struct bt_iso_big **out_big)
{
    int err;
    bt_le_host_lock();
    err = bt_iso_big_create(padv, param, out_big);
    bt_le_host_unlock();
    return err;
}
#endif /* CONFIG_BT_ISO_BROADCASTER */

int bt_iso_big_terminate_safe(struct bt_iso_big *big)
{
    int err;
    bt_le_host_lock();
    err = bt_iso_big_terminate(big);
    bt_le_host_unlock();
    return err;
}

#if CONFIG_BT_ISO_SYNC_RECEIVER
int bt_iso_big_sync_safe(struct bt_le_per_adv_sync *sync, struct bt_iso_big_sync_param *param,
                         struct bt_iso_big **out_big)
{
    int err;
    bt_le_host_lock();
    err = bt_iso_big_sync(sync, param, out_big);
    bt_le_host_unlock();
    return err;
}
#endif /* CONFIG_BT_ISO_SYNC_RECEIVER */
#endif /* CONFIG_BT_ISO_BROADCAST */

void bt_iso_reset_safe(void)
{
    bt_le_host_lock();
    bt_iso_reset();
    bt_le_host_unlock();
}
