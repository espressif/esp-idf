/******************************************************************************
 *
 *  Copyright (C) 2014 Google, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/
#if (defined(L2CAP_CLIENT_INCLUDED) && L2CAP_CLIENT_INCLUDED == TRUE)
#include <string.h>
#include "bt_trace.h"
#include "bt_defs.h"
#include "bdaddr.h"
#include "allocator.h"
#include "buffer.h"
#include "list.h"
#include "osi.h"
#include "l2cap_client.h"
#include "l2c_api.h"

struct l2cap_client_t {
    l2cap_client_callbacks_t callbacks;
    void *context;

    uint16_t local_channel_id;
    uint16_t remote_mtu;
    bool configured_self;
    bool configured_peer;
    bool is_congested;
    list_t *outbound_fragments;
};

static void connect_completed_cb(uint16_t local_channel_id, uint16_t error_code);
static void config_request_cb(uint16_t local_channel_id, tL2CAP_CFG_INFO *requested_parameters);
static void config_completed_cb(uint16_t local_channel_id, tL2CAP_CFG_INFO *negotiated_parameters);
static void disconnect_request_cb(uint16_t local_channel_id, bool ack_required);
static void disconnect_completed_cb(uint16_t local_channel_id, uint16_t error_code);
static void congestion_cb(uint16_t local_channel_id, bool is_congested);
static void read_ready_cb(uint16_t local_channel_id, BT_HDR *packet);
static void write_completed_cb(uint16_t local_channel_id, uint16_t packets_completed);

static void fragment_packet(l2cap_client_t *client, buffer_t *packet);
static void dispatch_fragments(l2cap_client_t *client);
static l2cap_client_t *find(uint16_t local_channel_id);

// From the Bluetooth Core specification.
static const uint16_t L2CAP_MTU_DEFAULT = 672;
static const uint16_t L2CAP_MTU_MINIMUM = 48;

static const tL2CAP_APPL_INFO l2cap_callbacks = {
    .pL2CA_ConnectCfm_Cb       = connect_completed_cb,
    .pL2CA_ConfigInd_Cb        = config_request_cb,
    .pL2CA_ConfigCfm_Cb        = config_completed_cb,
    .pL2CA_DisconnectInd_Cb    = disconnect_request_cb,
    .pL2CA_DisconnectCfm_Cb    = disconnect_completed_cb,
    .pL2CA_CongestionStatus_Cb = congestion_cb,
    .pL2CA_DataInd_Cb          = read_ready_cb,
    .pL2CA_TxComplete_Cb       = write_completed_cb,
};

static list_t *l2cap_clients;  // A list of l2cap_client_t. Container does not own objects.

buffer_t *l2cap_buffer_new(size_t size)
{
    buffer_t *buf = buffer_new(size + L2CAP_MIN_OFFSET);
    buffer_t *slice = NULL;
    if (buf) {
        slice = buffer_new_slice(buf, size);
    }
    buffer_free(buf);
    return slice;
}

l2cap_client_t *l2cap_client_new(const l2cap_client_callbacks_t *callbacks, void *context)
{
    assert(callbacks != NULL);
    assert(callbacks->connected != NULL);
    assert(callbacks->disconnected != NULL);
    assert(callbacks->read_ready != NULL);
    assert(callbacks->write_ready != NULL);

    if (!l2cap_clients) {
        l2cap_clients = list_new(NULL);
        if (!l2cap_clients) {
            LOG_ERROR("%s unable to allocate space for L2CAP client list.", __func__);
            return NULL;
        }
    }

    l2cap_client_t *ret = (l2cap_client_t *)osi_calloc(sizeof(l2cap_client_t));
    if (!ret) {
        LOG_ERROR("%s unable to allocate L2CAP client.", __func__);
        goto error;
    }

    ret->callbacks = *callbacks;
    ret->context = context;

    ret->remote_mtu = L2CAP_MTU_DEFAULT;
    ret->outbound_fragments = list_new(NULL);
    if (!ret) {
        LOG_ERROR("%s unable to allocate outbound L2CAP fragment list.", __func__);
        goto error;
    }

    list_append(l2cap_clients, ret);

    return ret;

error:;
    osi_free(ret);
    return NULL;
}

void l2cap_client_free(l2cap_client_t *client)
{
    if (!client) {
        return;
    }

    list_remove(l2cap_clients, client);
    l2cap_client_disconnect(client);
    list_free(client->outbound_fragments);
    osi_free(client);
}

bool l2cap_client_connect(l2cap_client_t *client, const bt_bdaddr_t *remote_bdaddr, uint16_t psm)
{
    assert(client != NULL);
    assert(remote_bdaddr != NULL);
    assert(psm != 0);
    assert(!bdaddr_is_empty(remote_bdaddr));
    assert(client->local_channel_id == 0);
    assert(!client->configured_self);
    assert(!client->configured_peer);
    assert(!L2C_INVALID_PSM(psm));

    client->local_channel_id = L2CA_ConnectReq(psm, (uint8_t *)remote_bdaddr);
    if (!client->local_channel_id) {
        LOG_ERROR("%s unable to create L2CAP connection.", __func__);
        return false;
    }

    L2CA_SetConnectionCallbacks(client->local_channel_id, &l2cap_callbacks);
    return true;
}

void l2cap_client_disconnect(l2cap_client_t *client)
{
    assert(client != NULL);

    if (client->local_channel_id && !L2CA_DisconnectReq(client->local_channel_id)) {
        LOG_ERROR("%s unable to send disconnect message for LCID 0x%04x.", __func__, client->local_channel_id);
    }

    client->local_channel_id = 0;
    client->remote_mtu = L2CAP_MTU_DEFAULT;
    client->configured_self = false;
    client->configured_peer = false;
    client->is_congested = false;

    for (const list_node_t *node = list_begin(client->outbound_fragments); node != list_end(client->outbound_fragments); node = list_next(node)) {
        osi_free(list_node(node));
    }

    list_clear(client->outbound_fragments);
}

bool l2cap_client_is_connected(const l2cap_client_t *client)
{
    assert(client != NULL);

    return client->local_channel_id != 0 && client->configured_self && client->configured_peer;
}

bool l2cap_client_write(l2cap_client_t *client, buffer_t *packet)
{
    assert(client != NULL);
    assert(packet != NULL);
    assert(l2cap_client_is_connected(client));

    if (client->is_congested) {
        return false;
    }

    fragment_packet(client, packet);
    dispatch_fragments(client);
    return true;
}

static void connect_completed_cb(uint16_t local_channel_id, uint16_t error_code)
{
    assert(local_channel_id != 0);

    l2cap_client_t *client = find(local_channel_id);
    if (!client) {
        LOG_ERROR("%s unable to find L2CAP client for LCID 0x%04x.", __func__, local_channel_id);
        return;
    }

    if (error_code != L2CAP_CONN_OK) {
        LOG_ERROR("%s error connecting L2CAP channel: %d.", __func__, error_code);
        client->callbacks.disconnected(client, client->context);
        return;
    }

    // Use default L2CAP parameters.
    tL2CAP_CFG_INFO desired_parameters = { 0 };
    if (!L2CA_ConfigReq(local_channel_id, &desired_parameters)) {
        LOG_ERROR("%s error sending L2CAP config parameters.", __func__);
        client->callbacks.disconnected(client, client->context);
    }
}

static void config_request_cb(uint16_t local_channel_id, tL2CAP_CFG_INFO *requested_parameters)
{
    tL2CAP_CFG_INFO response = { 0 };
    l2cap_client_t *client = find(local_channel_id);

    if (!client) {
        LOG_ERROR("%s unable to find L2CAP client matching LCID 0x%04x.", __func__, local_channel_id);
        return;
    }

    response.result = L2CAP_CFG_OK;

    if (requested_parameters->mtu_present) {
        // Make sure the peer chose an MTU at least as large as the minimum L2CAP MTU defined
        // by the Bluetooth Core spec.
        if (requested_parameters->mtu < L2CAP_MTU_MINIMUM) {
            response.mtu = L2CAP_MTU_MINIMUM;
            response.mtu_present = true;
            response.result = L2CAP_CFG_UNACCEPTABLE_PARAMS;
        } else {
            client->remote_mtu = requested_parameters->mtu;
        }
    }

    if (requested_parameters->fcr_present) {
        if (requested_parameters->fcr.mode != L2CAP_FCR_BASIC_MODE) {
            response.fcr_present = true;
            response.fcr = requested_parameters->fcr;
            response.fcr.mode = L2CAP_FCR_BASIC_MODE;
            response.result = L2CAP_CFG_UNACCEPTABLE_PARAMS;
        }
    }

    if (!L2CA_ConfigRsp(local_channel_id, &response)) {
        LOG_ERROR("%s unable to send config response for LCID 0x%04x.", __func__, local_channel_id);
        l2cap_client_disconnect(client);
        return;
    }

    // If we've configured both endpoints, let the listener know we've connected.
    client->configured_peer = true;
    if (l2cap_client_is_connected(client)) {
        client->callbacks.connected(client, client->context);
    }
}

static void config_completed_cb(uint16_t local_channel_id, tL2CAP_CFG_INFO *negotiated_parameters)
{
    l2cap_client_t *client = find(local_channel_id);

    if (!client) {
        LOG_ERROR("%s unable to find L2CAP client matching LCID 0x%04x.", __func__, local_channel_id);
        return;
    }

    switch (negotiated_parameters->result) {
    // We'll get another configuration response later.
    case L2CAP_CFG_PENDING:
        break;

    case L2CAP_CFG_UNACCEPTABLE_PARAMS:
        // TODO: see if we can renegotiate parameters instead of dropping the connection.
        LOG_WARN("%s dropping L2CAP connection due to unacceptable config parameters.\n", __func__);
        l2cap_client_disconnect(client);
        break;

    case L2CAP_CFG_OK:
        // If we've configured both endpoints, let the listener know we've connected.
        client->configured_self = true;
        if (l2cap_client_is_connected(client)) {
            client->callbacks.connected(client, client->context);
        }
        break;

    // Failure, no further parameter negotiation possible.
    default:
        LOG_WARN("%s L2CAP parameter negotiation failed with error code %d.\n", __func__, negotiated_parameters->result);
        l2cap_client_disconnect(client);
        break;
    }
}

static void disconnect_request_cb(uint16_t local_channel_id, bool ack_required)
{
    l2cap_client_t *client = find(local_channel_id);
    if (!client) {
        LOG_ERROR("%s unable to find L2CAP client with LCID 0x%04x.\n", __func__, local_channel_id);
        return;
    }

    if (ack_required) {
        L2CA_DisconnectRsp(local_channel_id);
    }

    // We already sent a disconnect response so this LCID is now invalid.
    client->local_channel_id = 0;
    l2cap_client_disconnect(client);

    client->callbacks.disconnected(client, client->context);
}

static void disconnect_completed_cb(uint16_t local_channel_id, UNUSED_ATTR uint16_t error_code)
{
    assert(local_channel_id != 0);

    l2cap_client_t *client = find(local_channel_id);
    if (!client) {
        LOG_ERROR("%s unable to find L2CAP client with LCID 0x%04x.\n", __func__, local_channel_id);
        return;
    }

    client->local_channel_id = 0;
    l2cap_client_disconnect(client);

    client->callbacks.disconnected(client, client->context);
}

static void congestion_cb(uint16_t local_channel_id, bool is_congested)
{
    assert(local_channel_id != 0);

    l2cap_client_t *client = find(local_channel_id);
    if (!client) {
        LOG_ERROR("%s unable to find L2CAP client matching LCID 0x%04x.\n", __func__, local_channel_id);
        return;
    }

    client->is_congested = is_congested;

    if (!is_congested) {
        // If we just decongested, dispatch whatever we have left over in our queue.
        // Once that's done, if we're still decongested, notify the listener so it
        // can start writing again.
        dispatch_fragments(client);
        if (!client->is_congested) {
            client->callbacks.write_ready(client, client->context);
        }
    }
}

static void read_ready_cb(uint16_t local_channel_id, BT_HDR *packet)
{
    assert(local_channel_id != 0);

    l2cap_client_t *client = find(local_channel_id);
    if (!client) {
        LOG_ERROR("%s unable to find L2CAP client matching LCID 0x%04x.\n", __func__, local_channel_id);
        return;
    }

    // TODO(sharvil): eliminate copy from BT_HDR.
    buffer_t *buffer = buffer_new(packet->len);
    memcpy(buffer_ptr(buffer), packet->data + packet->offset, packet->len);
    osi_free(packet);

    client->callbacks.read_ready(client, buffer, client->context);
    buffer_free(buffer);
}

static void write_completed_cb(UNUSED_ATTR uint16_t local_channel_id, UNUSED_ATTR uint16_t packets_completed)
{
    // Do nothing. We update congestion state based on the congestion callback
    // and we've already removed items from outbound_fragments list so we don't
    // really care how many packets were successfully dispatched.
}

static void fragment_packet(l2cap_client_t *client, buffer_t *packet)
{
    assert(client != NULL);
    assert(packet != NULL);

    // TODO(sharvil): eliminate copy into BT_HDR.
    BT_HDR *bt_packet = osi_malloc(buffer_length(packet) + L2CAP_MIN_OFFSET);
    bt_packet->offset = L2CAP_MIN_OFFSET;
    bt_packet->len = buffer_length(packet);
    memcpy(bt_packet->data + bt_packet->offset, buffer_ptr(packet), buffer_length(packet));

    for (;;) {
        if (bt_packet->len <= client->remote_mtu) {
            if (bt_packet->len > 0) {
                list_append(client->outbound_fragments, bt_packet);
            } else {
                osi_free(bt_packet);
            }
            break;
        }

        BT_HDR *fragment = osi_malloc(client->remote_mtu + L2CAP_MIN_OFFSET);
        fragment->offset = L2CAP_MIN_OFFSET;
        fragment->len = client->remote_mtu;
        memcpy(fragment->data + fragment->offset, bt_packet->data + bt_packet->offset, client->remote_mtu);

        list_append(client->outbound_fragments, fragment);

        bt_packet->offset += client->remote_mtu;
        bt_packet->len -= client->remote_mtu;
    }
}

static void dispatch_fragments(l2cap_client_t *client)
{
    assert(client != NULL);
    assert(!client->is_congested);

    while (!list_is_empty(client->outbound_fragments)) {
        BT_HDR *packet = (BT_HDR *)list_front(client->outbound_fragments);
        list_remove(client->outbound_fragments, packet);

        switch (L2CA_DataWrite(client->local_channel_id, packet)) {
        case L2CAP_DW_CONGESTED:
            client->is_congested = true;
            return;

        case L2CAP_DW_FAILED:
            LOG_ERROR("%s error writing data to L2CAP connection LCID 0x%04x; disconnecting.", __func__, client->local_channel_id);
            l2cap_client_disconnect(client);
            return;

        case L2CAP_DW_SUCCESS:
            break;
        }
    }
}

static l2cap_client_t *find(uint16_t local_channel_id)
{
    assert(local_channel_id != 0);

    for (const list_node_t *node = list_begin(l2cap_clients); node != list_end(l2cap_clients); node = list_next(node)) {
        l2cap_client_t *client = (l2cap_client_t *)list_node(node);
        if (client->local_channel_id == local_channel_id) {
            return client;
        }
    }

    return NULL;
}

#endif /*L2CAP_CLIENT_INCLUDED*/
